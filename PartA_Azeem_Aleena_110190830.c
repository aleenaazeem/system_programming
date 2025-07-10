#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    //here we are opening and creating the file "new.txt"
    //fd is our pointer to the file
    int fd = open("new.txt",O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd <0){ //we are throwing an error here
        perror("open");
        return 1;
    }
    // Redirecting to STDOUT from fd1 to fd
    if (dup2(fd,STDOUT_FILENO)<0){
        perror("dup2");
        close(fd);
        return 1;
    }
    // Run the ls command so that the output goes to the new.txt now
    execlp("ls","ls",NULL);
    //If execlp fails
    perror("execlp");
    return 1;
}
