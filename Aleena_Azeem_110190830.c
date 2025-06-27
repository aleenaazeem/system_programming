//Lab 6 we are supposed to create an infinite loop
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
//Here we are creating a global variable which will record the times ctrl c 
//has been pressed by the user
volatile sig_atomic_t initial_time = 0;
//Now we will define a signal handler that will work when we press ctrl + c
//If this was not defined that the default handler would have been accessed
void our_handler(int signal) {
    time_t app_time = time(NULL);
    //if ctrl + c is pressed in the last 5 seconds than we do this
      //we check the difference of first press to last then print exiting and also exit
    if (initial_time != 0 && (app_time - initial_time) <= 5) {
        printf("....\n...\n3\n ..\n2\n ..\n1\n We are Exiting...\n");
        _exit(0);
    } else {
        //This one handles if ctrl c has been pressed once or there has been a delay
       //that is more than 5 seconds
        initial_time = app_time;
    }
}

int main() {
    // Register the signal handler for Ctrl+C
    signal(SIGINT, our_handler);
    //Here we have created an infinite loop that keeps on going
    while (1) {
        printf("Entering Ctrl-C only once will not work, you need to enter it twice within a 5 second interval\n");
        sleep(1);
    }

    return 0;
}

