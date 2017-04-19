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
#include <time.h>
#include <stdlib.h> //system
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/*Prototype for process: */
int split(int, int, int);


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
  char *command = "wc -l";
  strcat(command, filepath);

  int lines = system(command);
  
  //Process the file - split into pieces
  int returnValue = split(infd, linesperfile, lines);

  //Close and finish read_process.
  close(infd);

  return returnValue;
}



//Get rid of argv and argv, change to files.
//fd - file descriptor | lines_splitFs - lines per split file
//lines_F - lines in original file.
int split(int fd, int lines_splitFs, int lines_F) {

  /* A file will be created for each split */
  int num_files = lines_F / lines_splitFs;
 
  //Dealing with integer division:
  if (num_files * lines_splitFs < lines_F){
    ++num_files;
  }
     

  // construct our fan - One for each file
  pid_t childpid;
  int i;
  char* name = 0;
  for (i = 1; i <= num_files; ++i) {
    name = "fd";
//    char* intstr; itoa(i, intstr, 10);
//    strcat(name, intstr);
    if ((childpid = fork()) <= 0)
      break;
  }
  
  // have something to do
  srand(time(NULL) * i);
  sleep(rand() % (2 * num_files));

  // parent waits for all the children
  if (childpid != 0) {

    pid_t nchildpid = 0;
    while ((nchildpid = wait(NULL))) {
      if (errno == ECHILD)
	break;
      fprintf(stderr, "Child %ld is done\n", (long)nchildpid);
    }
  }
  
  fprintf(stderr, "i:%d Name: %s Processing: process ID: %ld  parent ID: %ld  child ID: %ld\n",
	  i, name, (long) getpid(), (long) getppid(), (long) childpid);

  free(name);
  
  return 0;
}
