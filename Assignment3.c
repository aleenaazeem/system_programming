//We have to implement a bash terminal
#include <stdio.h>  //printf, fgets, perror
#include <stdlib.h>   //exit, EXIT_FAILURE
#include <unistd.h>   //fork, execvp, dup2, pipe
#include <string.h>    //strtok, strlen, strcmp
#include <sys/wait.h>   //wait, waitpid
#include <stdbool.h>    //for true/false support
#include <fcntl.h> //This is used to open files

int background_count = 0;

char input[500];
char *args[6];
char *commands[5]; // store pipe segments

// Tokenize a command and detect &
int tokenx(char *input, char *args[], bool *is_background) {
    int i = 0;
    char *token = strtok(input, " ");

    while (token != NULL && i < 5) {
        if (strcmp(token, "&") == 0) {
            *is_background = true;
        } else if (token[strlen(token) - 1] == '&') {
            token[strlen(token) - 1] = '\0';
            *is_background = true;
            if (strlen(token) > 0)
                args[i++] = token;
        } else {
            args[i++] = token;
        }

        token = strtok(NULL, " ");
    }

    args[i] = NULL;
    return i;
}
int piper(char *input){
         char *commands[5];
         int num_cmds = 0;
       //Split the string give by user at | and get the commands
        char *cmd = strtok(input, "|");
         //here we are looping and tokenizing
            while (cmd != NULL && num_cmds < 5) {
                commands[num_cmds++] = cmd;
                cmd = strtok(NULL, "|");
            }

            int pipefd[2 * (4)];
            for (int i = 0; i < num_cmds - 1; i++) {
                if (pipe(pipefd + i * 2) < 0) {
                    perror("Pipe failed");
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < num_cmds; i++) {
                pid_t pid = fork();
                if (pid == 0) {
                    // INPUT
                    if (i > 0) {
                        dup2(pipefd[(i - 1) * 2], 0); // read end
                    }

                    // OUTPUT
                    if (i < num_cmds - 1) {
                        dup2(pipefd[i * 2 + 1], 1); // write end
                    }

                    // Close all pipe fds
                    for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                        close(pipefd[j]);
                    }

                    // Parse and run current command
                    bool bg = false;
                    char *cmd_args[6];
                    tokenx(commands[i], cmd_args, &bg);
                    execvp(cmd_args[0], cmd_args);
                    perror("Execution failed");
                    exit(EXIT_FAILURE);
                }
            }

            // Close pipes in parent
            for (int i = 0; i < 2 * (num_cmds - 1); i++) {
                close(pipefd[i]);
            }

            // Wait for all children
            for (int i = 0; i < num_cmds; i++) {
                wait(NULL);
            } 
}
//----------------------------------- << <
void greater(char *input){
    char *args[6];
    bool is_background = false;

    char *cmd = NULL;
    char *filename = NULL;
    int redir_type = 0; // 1 = <, 2 = >, 3 = >>

    // Scan the string manually to find operator
    for (int i = 0; input[i]; i++) {
        if (input[i] == '<') {
            redir_type = 1;
            input[i] = '\0';  // Split the string at '<'
            cmd = input;
            filename = input + i + 1;
            break;
        }
        else if (input[i] == '>') {
            if (input[i + 1] == '>') {
                redir_type = 3;
                input[i] = '\0';          // Split at '>'
                input[i + 1] = '\0';      // Null-terminate >>
                cmd = input;
                filename = input + i + 2;
            } else {
                redir_type = 2;
                input[i] = '\0';  // Split at '>'
                cmd = input;
                filename = input + i + 1;
            }
            break;
        }
    }

    if (!cmd || !filename) {
        printf("Invalid redirection format.\n");
        return;
    }

    // Remove leading spaces from filename
    while (*filename == ' ') filename++;

    tokenx(cmd, args, &is_background);

    pid_t pid = fork();
    if (pid == 0) {
        int fd;
        if (redir_type == 1) {
            fd = open(filename, O_RDONLY);
            if (fd < 0) {
                perror("Input open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, 0); // stdin
        } else if (redir_type == 2) {
            fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Output open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, 1); // stdout
        } else if (redir_type == 3) {
            fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("Append open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, 1); // stdout
        }

        close(fd);
        execvp(args[0], args);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Fork failed");
    }
}

int main() {

    while (1) {
        printf("mbash25$ ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) break;

        int len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (strcmp(input, "killterm") == 0) {
            printf("We are terminating mbash25$...\n");
            break;
        }

        // -------------------- PIPE SUPPORT ---------------------
        if (strchr(input, '|')) {
            piper(input);
        }
// ------------------------------WE ARE CHECKING FOR REDIRECTION
        if(strchr(input,'<')||strstr(input,">>")||strchr(input,'>')){
            greater(input);
           continue;
         }
        // -------------------- NO PIPES (normal or background) ---------------------
        else {
            bool is_background = false;
            int num_tokens = tokenx(input, args, &is_background);
            if (num_tokens == 0) continue;

            pid_t pid = fork();

            if (pid == 0) {
                execvp(args[0], args);
                perror("Execution failed");
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                if (is_background) {
                    if (background_count < 4) {
                        printf("[mbash25$] background process %d started\n", pid);
                        background_count++;
                    } else {
                        printf("Too many background processes (limit = 4).\n");
                        waitpid(pid, NULL, 0);
                    }
                } else {
                    waitpid(pid, NULL, 0);
                }
            } else {
                perror("Fork failed");
            }
        }
    }

    return 0;
}
