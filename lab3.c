#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//We are working for la 3 where have to do few steps
int main()
{
 //Step1 we will write "Welcome to COMP8567,University of Windsor" into lab3.txt is using write()
    //this is going to be set to 0666
    umask(0000);
   char buff1[35];
   char first_message[] = "Welcome to COMP8567, University of Windsor";
   char additional_message[] = "School of Computer Science, ";
   char last_message[] = "Summer 2025";
  //First we will create a file called lab3.txt using the system call open()
   int fd = open("lab3.txt",O_CREAT | O_RDWR, 0666);
   if (fd <0){
    perror("Error open file");
    return 1;
}
    //Now we will write our first_message in it
    write(fd, first_message, strlen(first_message));
    //Now we will read it into buff1 that we have created earlier
    lseek(fd, -strlen("University of Windsor"),SEEK_END);
    //here we are  reading the statement into the buff1
     read(fd, buff1,strlen("University of Windsor"));
     //now we will add a null terminator at the end of the line
     buff1[strlen("University of Windsor")] = '\0';
     
    //Now we will insert "School of Computer Science" After the first statement 
    lseek(fd, strlen("Welcome to COMP8567, "), SEEK_SET);
     write(fd, additional_message,strlen(additional_message));
    //Here we will write the content of the buff1 immediately afterwards
    write(fd, buff1, strlen(buff1));

   //Now we will inset the hypen between the course and code

   lseek(fd,strlen("Welcome to COMP"),SEEK_SET);
   write(fd, "-",1);

   off_t end = lseek(fd,0,SEEK_END);
   lseek(fd,end+12,SEEK_SET);
  write(fd,last_message,strlen(last_message));

   //Now we are going to remove the 12 null character added at the end of the text and before the last message
  char ch;
  for(int i=0; i<end+12+strlen(last_message); i++){
   lseek(fd,i,SEEK_SET);
    read(fd, &ch,1);
  if(ch == '\0'){
   lseek(fd,i,SEEK_SET);
   write(fd," ",1);
}
}
close(fd);
return 0;
}

