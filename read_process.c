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

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h> //system
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
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
int split(int fd, const char* filename) {

  /* A file will be created for each split */     
  int num_files = 2;


  return 0;
}
