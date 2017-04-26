/*
  read_process.c

  process:
  Fan-structured processes that handles the splitting of 
    the burst file.

  read_process: Open the burst file for read, then process.
    Finally close it.

  Jacob Liddy
  jpl61@zips.uakron.edu
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <errno.h> //For fileno? 
#include <fcntl.h>
#include <sys/stat.h> //S_IRUSR
/*Prototype for process: */
int split(int, const char*);


//TODO add options for argument to process.
int read_process(const char* filepath, int linesperfile){
  /*Infile file descriptor*/
  
  int infd = open(filepath, O_RDONLY);
  #ifdef DEBUG
    fprintf(stderr, "Filedescriptor: %d\n", infd); 
  #endif

  /* Handle bad file descriptors: */
  if (infd < 0){
    fprintf(stderr, "Could not open %s\n", filepath);
    return -1;
  }


  /*TODO Fix this **nasty** hack to get the line number of lines
  a file */
  //Process the file - split into pieces
  int returnValue = split(infd, filepath);

  //Close and finish read_process.
  close(infd);
  
  return returnValue;
}


//Assume file is 501 to 999 lines
//Get rid of argv and argv, change to files.
//fd - file descriptor | lines_splitFs - lines per split file
//lines_F - lines in original file.
struct threaddata_t {
    int id; // id
    int status; // status
    pthread_t tid; // thread id
};


int split(int fd, const char* filename) {
  //One thread for each file
  int numfiles = 2;


  if (numfiles < 0) {
    fprintf(stderr, "Less than 1 file attempted");
    return 1;
  }

  //Allocate memory for thread info - deal with 2 files for now:
  struct threaddata_t* threadinfo = calloc(numfiles, sizeof(struct threaddata_t));

  if (!threadinfo) {
    fprintf(stderr, "Unable to allocate thread info\n");
    return 1;
  }

  /* A file will be created for each split */     
  int num_files = 2;

    


  return 0;
}
