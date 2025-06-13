//Name : Aleena Ali Azeem
// SID : 110190830
#include <stdio.h> //these are the libraries we have included in our code 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
//We have the hiery of our code as Aleena_Azeem_110190830.c
//The we create two directory Child one and child two
//The we create a grand child one from child one
// And a grandchild two from child two
//Further we create fork process and change directories using execvp
//we use waitpid() in order to wait for the processes 

int main() {
    //Here we are creating 4 processess
    pid_t c1;  //This is our child one
    pid_t c2;   //This will be our child 2
    pid_t gc1;   //This is our grandchild 1
    pid_t gc2;  //This is our grandchild two 
    int status;  //This will give the exit status 
     //In this file we will be creating 2 fork statements which will create 3 decendents in our main functions

    c1 = fork();
    //Child returns 0 so we are in the child process of first child
    if (c1 == 0) {
        // This is child 1 and our intial fork
        gc1 = fork(); //This is the grandchild one process forking
        if (gc1 == 0) { 
            //We are in the grandchild process here
            printf("GC1 will now differentiate\n"); //This will be printed initially
            chdir("/home/azeem7/labs/chapter5/GC1"); //Here we are changing the directory from chapter 5 to GC1
           //We will be executing the ls -l pathway by creating args 
           char *args[] = {"ls", "-1", "/home/azeem7/labs/chapter5/GC1", NULL};
            execvp("ls", args); //we are using the execvp which is vector to change directory
          //This will give an eroor 
           perror("execvp failed for GC1 which sad");
           //to stop the zombie attack and code overloading we exit yes best practices  
          exit(1);
        } else {
            //This is the message printed by the parent process
            printf("C1 is waiting for GC1 like a good parent\n");
            //we wait till the first of our program is executed
            waitpid(gc1, &status, 0);
          //Child 1 will exit after the grandchild one is finishing 
           exit(0);
        }
    } else { 
        // Now we will create our child two in the parent process
        c2 = fork();
        //We are inside the child 2 process of our program
        if (c2 == 0) {
            // This is the child 2 of our process
            gc2 = fork();
            if (gc2 == 0) {
                //Here we are inside our grandchild 2 process
                printf("GC2 will now differentiate to pwd\n");  //This message will be printed inorder to run
                chdir("/home/azeem7/labs/chapter5/GC2"); //this is the change directory to the said and written
                char *args[] = {"pwd", NULL}; //here we are using args to change the directory
                execvp("pwd", args);  //We are using execvp to change our directory
                perror("The execvp vect failed for GC2");  //this message is printed if the program fails to fun
                exit(1); //here we are using exit to get out of the program
            } else {
                //Here we are waiting for C2 to execute and we will print a message on the console
                printf("C2 is waiting for GC2\n");
                //we are creating a variable to wait for the process and we will give it process ide as zero because it is the child
                int ret = waitpid(gc2, &status, 0);
                //Now we will print the waitpid on the screen with the message
                printf("The child two: waitpid() give is = %d\n", ret);
                if (WIFEXITED(status)) { //We are using this signal to check if our program executed gracefully
                    printf("In the child two: the grand child two exited with the status give = %d\n", WEXITSTATUS(status)); //this will give us the status of our program
                }
                exit(0); //We use exit to get out of the program
            }
        } else {
            //This is the main process and it waits for both of the processes so that there are no zombies
            waitpid(c1, NULL, 0); //this is waiting for child one 
            waitpid(c2, NULL, 0); //this is waiting for child two
        }
    }

    return 0;
}
