#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    //We need 3 pipes to connect 4 commands in a chain
     //cat sample.txt | grep 2026 | wc | wc -w
    int p1[2];
    int p2[2];
    int p3[2];

    // Creating 3 pipes
    pipe(p1);
    pipe(p2);
    pipe(p3);

    // First command: cat sample.txt
    //So this will be our last child
    if (fork() == 0) {
          //Redirect stdout to write-end of p1
        dup2(p1[1], STDOUT_FILENO); // cat writes to pipe1
         //close all unused pipe ends to avoid leaks or blocking
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        //Execute 'cat sample.txt"
        execlp("cat", "cat", "sample.txt", NULL);
         //If execlp failed so we need to show an error
        perror("cat failed");
        exit(1);
    }

    // Second command: grep 2026
     //now we are creaking second child
    if (fork() == 0) {
        //Input comes from read-end of p1
        dup2(p1[0], STDIN_FILENO);   // read from pipe1
        //output goes to write-end of p2
        dup2(p2[1], STDOUT_FILENO);  // write to pipe2
        //Here we are closing all unused pipes
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        //We will be executing the grep 2026 here
        execlp("grep", "grep", "2026", NULL);
        //We throw an error if it doesnt work 
        perror("grep failed");
        exit(1);
    }

    // Third command: wc
    if (fork() == 0) {
        dup2(p2[0], STDIN_FILENO);   // read from pipe2
        dup2(p3[1], STDOUT_FILENO);  // write to pipe3
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        execlp("wc", "wc", NULL);
        perror("wc failed");
        exit(1);
    }

    // Fourth command: wc -w (final output to terminal)
    if (fork() == 0) {
        dup2(p3[0], STDIN_FILENO);  // read from pipe3
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        execlp("wc", "wc", "-w", NULL);
        perror("wc -w failed");
        exit(1);
    }
     //We are in the parent process here 
    // Parent closes all pipe ends
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);

    // Wait for all 4 child processes
    for (int i = 0; i < 4; i++) wait(NULL);

    return 0;
}

