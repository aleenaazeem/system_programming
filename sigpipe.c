// C program to illustrate
// pipe system call in C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#define MSGSIZE 200
char* msg1 = "Welcome to COMP8567 Advanced Systems Programming";
//Illustration of SIGPIPE- When you try to write into a pipe whose read fd is closed 


int main()
{
    char inbuf[200];
    int p[2], i;

    if (pipe(p) < 0) //Invoke the pipe() system call
        exit(1);

        close(p[0]);

    //get the values of PIPE_BUF
    printf("\nThe value of PIPE_BUF is %d\n", PIPE_BUF);
    //write msg1 into the pipe using the write FD p[1]

    write(p[1], msg1, 200);

    //read the contents of the pipe into inbuf using the read FD p[0]


    int n= read(p[0], inbuf,200);
    printf("The contents of the pipe are\n%s", inbuf);

    printf("\nThe number of bytes read were %d\n",n);
    return 0;
}
