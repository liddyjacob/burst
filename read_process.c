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
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/*Prototype for process: */
int process(int fd);


//TODO add options for argument to process.
int read_process(const char* filepath){
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
  
  //Process the file -
  int returnValue = process(infd);

  //Close and finish read_process.
  close(infd);

  return returnValue;
}



//Get rid of argv and argv, change to files.
int process(int fd) {



  if (argc < 2) {
    fprintf(stderr, "Usage: %s processes\n", argv[0]);
    return 1;
  }
  
  // determine the fan length
  int n = argc - 1; // atoi(argv[1]);

  // construct our fan
  pid_t childpid;
  int i;
  char* name = 0;
  for (i = 1; i <= n; ++i) {
    name = strdup(argv[i]);
    if ((childpid = fork()) <= 0)
      break;
  }
  
  // have something to do
  srand(time(NULL) * i);
  sleep(rand() % (2 * n));

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
