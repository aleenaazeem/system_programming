/*  Assignment 1
    Name: Aleena Ali Azeem
    Roll number: 110190830
    Course: Advanced Computer Programming
    Video Link: https://drive.google.com/file/d/1ZJtIX9bZdDmEgaK4qCQApx2BSGcSwaVr/view?usp=sharing
 */
#define _XOPEN_SOURCE 500 //This will enable the feature like nftw() as we are getting Xopen version 500
#include <limits.h>//it gives a place to store the full file or directory paths
#include <ftw.h> //This is the header library for nftw() walking through directory tree
#include <stdio.h> //Standard library to use like printf(), perror()
#include <stdlib.h> //this helps in managing dynamic memory managemnt , exit() etc
#include <string.h>  //we access the string functions using this library function
#include <sys/stat.h> //this is used to get the size of the file
#include <unistd.h> //this is used to get POSIX functions like access(), unlink() close()
#include <dirent.h> //its opens and read directory basically we can use it for directory manipulation
#include <fcntl.h> //file control options like open() read() , write() etc
// Global buffers for matching and tracking
//Here we are defining a variable which will store basically an argument given by the user , its 256 because it can be a path to a file
char userarg[256];
//This variable will store the argument such dlt lext etc which are short so its size doesnt have to be big
char userextension[64];
//These two variables are created inorder to count the files
int counting_files = 0;
//count the number of directories
int counting_directories = 0;
//this is used to get the size of the file in bytes
off_t totalSize = 0;
off_t maxFileSize = 0; 
//triggered by nftw()
//checks if the current file matches the target file and prints it
int recur_search(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
       //FTW_F means this is a regular file
 if (typeflag == FTW_F && strcmp(fpath + ftwbuf->base, userarg) == 0) {
      //If the file matches the file we are searching for then print the full path
  printf("%s\n", fpath);
    }
    return 0;
}
//THIS IS INTIAL SEARCH FUNTION
void execution_search(const char *filename, const char *root_dir) {
    //Here we are using strcpy to copy the name of the file to the userarg so it becomes global
    strcpy(userarg, filename);
    //Here we are calling the above function to run and walk all the file directories
    if (nftw(root_dir, recur_search, 20, 0) == -1)
       perror("nftw");
}

//This function will list all the directories
//this is triggered by nftw()
//it compares names
int recurs_listing(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
   //here we are checking if the current entry is a directory 
   if (typeflag == FTW_D && strcmp(fpath + ftwbuf->base, userarg) == 0) {
       //printing the directory name and seeing if it matches the targer to print full path 
       printf("%s\n", fpath);
    }
    return 0; //continue walking the tree (looping)
}
//THIS IS INITIAL LISTING THE DIRECTORIES FUNCTION
void execution_listing(const char *dirname, const char *root_dir) {
    //we are making the userarg gloabl so it can be accessed by the call back function 
   strcpy(userarg, dirname);
   //We begin the walking of the directory tree from root
    if (nftw(root_dir, recurs_listing, 20, 0) == -1)
       perror("nftw");
}

// THIS WILL LIST ALL THE DIRECTORIES AND FILES
//this is a callback function triggered by nftw()
//simply printing the path of all the files
int recurs_fdlisting(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    //Here we are printing the absolute path of the current file to show
    printf("%s\n", fpath);
    return 0; //keep the loop going
}
//This is the inital function to trigger the walking of files
//It calls the callback function for every item found in the tree
void execution_fdlisting(const char *root_dir) {
   //Traverse the file tree rooted at root_dir
    //Here for each file we will callback list will be executed
    if (nftw(root_dir, recurs_fdlisting, 20, 0) == -1)
        perror("nftw");
}

//This part is the callback which is triggered by nftw()
//it checks if a file ends with a specific extension
int recur_lexical(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F && strstr(fpath, userextension)) {
      //Only consider regular files (not directories etc)
       //This is will print if the file exists at the said place
        printf("%s\n", fpath);
    }
    return 0;
}
//This is the initial function which would trigger the nftw to execute the walk and find file
void execution_lexical(const char *root_dir, const char *ext) {
    //First we copy the name of the file to make it global
    strcpy(userextension, ext);
     //Now we start the walk in order to get if it exists or not
    if (nftw(root_dir, recur_lexical, 20, 0) == -1) 
      perror("nftw");
}

// This is used to find the number of the files
//triggered by again nftw()
//It increments the global declared variable by us
int recurs_countingfiles(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
     //FTW_F means this item is a regular file
     if (typeflag == FTW_F || typeflag == FTW_SL)
     //Written this for debugging
      printf("Checking: %s (typeflag = %d)\n",fpath,typeflag);
       counting_files++; //this increases the global count
    return 0;
}
//This is an initial function
//Here we are traversing the directory to get the count of the file
void execution_countfile(const char *root_dir) {
    counting_files = 0; //we are reseting the counter to zero
    nftw(root_dir, recurs_countingfiles, 20, 0); //recurs_countingfiles for each file and directory
    //Now we will print the number of the files
    printf("The number of the files are: %d\n", counting_files);
}

//This is the counting function for the directors of a particular name
//This is triggered by nftw()
//It increments the global variable declared for counting the directories by 1
int recurs_countingdir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) 
    //Here we are incrementing the global variable by 1 when we see the target  
   counting_directories++;
    return 0; //the loop keeps going on
}
 //this funtion is the one that triggers the count of the directors
void executing_countingdir(const char *root_dir) {
    counting_directories = 0; //first we will set the varibale to zero and start the callback function
    nftw(root_dir, recurs_countingdir, 20, 0); //this traverses the entire directory
    printf("The number of Directory: %d\n", counting_directories); //this will print the number of the directory
}

//This function returns the size of the files
int recurs_file_size(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F || typeflag == FTW_SL )
    //here the offset valriable is being adding the file size to the running total
    totalSize += sb->st_size;
    return 0;
}
//this is the initial function that triggers the nftw()
void execution_sizeof(const char *root_dir) {
    totalSize = 0;//here we are setting the value to zero to which it can be incremented later
    nftw(root_dir, recurs_file_size, 20, 0); //this will parse
    printf("Total file size: %lld bytes\n", (long long)totalSize); //this will eventually print the
     //number of the total file size
}
// 3rd funtion
//This is the part of the code where we will be copying a file from one place to another
void copy_file(const char *src, const char *dst) {
    //First declare a function that takes source that should be copied and destination of where it
   //should be copied to
    int in = open(src, O_RDONLY);//Here we are using source file in read only mode
    //Here we are opening destination file in a write only mode and checking if it already exists
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    //This is a temporary buff to hold chunks of data while copying
    char b[4096];
    ssize_t bytes; //this variable will store how many bytes were read
    //we keep reading from the source and writing to destination until done
    while ((bytes = read(in, b, sizeof(b))) > 0)
       //here we write in the file line by line
      write(out, b, bytes);
     //this close both of the files
    close(in);
    close(out);
}
//callback function called by nftw() for each file or directory
int copy_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    char rel[PATH_MAX]; //this is a buffer store the path of the file
     //this gets relative path by skipping the source root prefix from full paht
    snprintf(rel, sizeof(rel), "%s", fpath + strlen(userarg));
    char dest_path[PATH_MAX]; //the final destination path
     //here we are concatenating destination root with relative path to get full destination path
    snprintf(dest_path, sizeof(dest_path), "%s%s", userextension, rel);
      //If the directory then create the same directory at the destination given by the user
    if (typeflag == FTW_D) mkdir(dest_path, 0755);
     //if its a file then you can copy the file 
    else if (typeflag == FTW_F) copy_file(fpath, dest_path);
    return 0;
}
//This is the initial function which triggers the callback of the nftw()
void duplicating_files(const char *src, const char *dest) {
    //this will copy the string of the source
    strcpy(userarg, src);
   //this will copy the strong of the destination to the userextension
    strcpy(userextension, dest);
   //then we will traverse using the nftw call
    nftw(src, copy_callback, 20, FTW_DEPTH);
}

//This will be move the directory by first copying it to a new location and then delecting the original folder and its contents
void moving_files(const char *src, const char *dest) {
    duplicating_files(src, dest); //we are calling the funciton that is defined above
     printf("Moving the file to: %s",dest); //this will be tell its a debugging feature
    char cmd[PATH_MAX + 20]; //this is a buffer that will holld the shell command string
    snprintf(cmd, sizeof(cmd), "rm -rf '%s'", src); //here we will construct a shell command to recursively delete the source directory
    system(cmd); //this will execute the command using system() which runs it in the shell
}

//This is the last functionality of our assignment 
//here we will delete the file that the user has passed on
//this is a callback triggered by nftw()
int delete_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F && strstr(fpath, userextension)) {
         printf("The file being deleted is %s\n",fpath);
        remove(fpath); //deleting the file at hand
    }
    return 0; //looping through
}
void execution_deletion(const char *root_dir, const char *ext) {
    strcpy(userextension, ext); //here we make the name of the file globsl
    // we use the function of strcpy
    //start walking the directory tree from root_dir
     //For each file, call delete_callback(0
    nftw(root_dir, delete_callback, 20, 0);
}

//This is the main of our program it is the initializer of all the functions that we have built to traverse our directory tree
int main(int argc, char *argv[]) {
    //If the user has given less than two arguments then 
     //exit the code
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [arguments]\n", argv[0]);
        return 1;
    }
    //To match the command entered by the user with the supported operations and call the
    //respective function with the expected number of the arguments
     //First we call the search function to find the file:
    if (strcmp(argv[1], "-srch") == 0 && argc == 4) execution_search(argv[2], argv[3]);
     //The we run the listing directory funtion
     //we use strcmp to compare the strings and then run the function
    else if (strcmp(argv[1], "-listdir") == 0 && argc == 4) execution_listing(argv[2], argv[3]);
     //Here we will be list the files and directories given unfer a path
    else if (strcmp(argv[1], "-list") == 0 && argc == 3) execution_fdlisting(argv[2]);
     //Now we will run the file matching for example looking for .c files in the code
    else if (strcmp(argv[1], "-lext") == 0 && argc == 4) execution_lexical(argv[2], argv[3]);
    //counting the nummber of files
    else if (strcmp(argv[1], "-countf") == 0 && argc == 3) execution_countfile(argv[2]);
      //counting the number of directories
    else if (strcmp(argv[1], "-countd") == 0 && argc == 3) executing_countingdir(argv[2]);
    //getting the size of the files
    else if (strcmp(argv[1], "-sizef") == 0 && argc == 3) execution_sizeof(argv[2]);
    //copying one file or directory
    else if (strcmp(argv[1], "-cp") == 0 && argc == 4) duplicating_files(argv[2], argv[3]);
     //here we are moving the file to a new destination and deleting the old one
    else if (strcmp(argv[1], "-mv") == 0 && argc == 4) moving_files(argv[2], argv[3]);
    //this is used to delete the file
    else if (strcmp(argv[1], "-dlt") == 0 && argc == 4) execution_deletion(argv[2], argv[3]);
    else { //this is written for error handling
        fprintf(stderr, "Invalid command or arguments.\n");
        return 1;
    }
    return 0;
}
