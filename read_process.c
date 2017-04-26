/*
  read_process.c

  process:
  thread-structured processes that handles the splitting of 
    the burst file.

  read_process: Open the burst file for read, then process.
    Finally close it.

  Jacob Liddy
  jpl61@zips.uakron.edu
*/

//Dependancy *HELL*
//Do you believe in our lord and savior, LINUS TORVALDS?
#include <archive.h> //libarchive - 
#include <archive_entry.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <errno.h> //For fileno? 
#include <fcntl.h>
#include <sys/stat.h> //S_IRUSR

//Take in 512 bytes at a time
#define BLKSIZE 512

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


  //Process the file - split into pieces
  int errno = split(infd, filepath);

  //Close and finish read_process.
  close(infd);
  
  return errno;
}


//Assume file is 501 to 999 lines
//Get rid of argv and argv, change to files.
//fd - file descriptor | lines_splitFs - lines per split file
//lines_F - lines in original file.
struct threaddata_t {
    int id; // id
    int status; // status
    char buf[BLKSIZE]; //Buncha data from a file 
    pthread_t tid; // thread id
};

void* process_file(void* args){

  //Unpack arguments to data
  struct threaddata_t* data = args;

  //If debug, say we started
  #ifdef DEBUG
  fprintf(stderr, "Process %d starting\n", data->id);
  #endif

  //Process - Filestuff here
  srand(time(NULL) * data->id);
  sleep(rand() % 4); // Up to 4 seconds of sleep.

  fprintf(stderr, "Process %d ending\n", data->id);
  
  return &(data->status);
}







int split(int fd, const char* filename) {
  //One thread for each file
  int numfiles = 2;


  if (numfiles < 0) {
    fprintf(stderr, "Less than 1 file attempted");
    return 1;
  }

  //Array of threads
  //Allocate memory for thread info - deal with 2 files for now:
  struct threaddata_t* threadinfo = calloc(numfiles, sizeof(struct threaddata_t));

  if (!threadinfo) {
    fprintf(stderr, "Unable to allocate thread info\n");
    return 1;
  }

  //Create a thread for each file:

  for (int i = 0; i < numfiles; ++i){
    threadinfo[i].id = i;

    //Here is where the thread branches off to do some work:
    pthread_create(&threadinfo[i].tid, NULL, process_file, &threadinfo);
  }

  for (int i = 0; i < numfiles; ++i){

    #ifdef DEBUG
    fprintf(stderr, "read() %d waiting\n", threadinfo[i].id);
    #endif
    
    pthread_join(threadinfo[i].tid, NULL);

    #ifdef DEBUG
    fprintf(stderr, "read() %d finished\n", threadinfo[i].id);
    #endif
  }

  //Free threadinfo array memory
  free(threadinfo);   


  return 0;
}
