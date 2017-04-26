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
int split(struct archive* a, const char*);


//TODO add options for argument to process.
int read_process(const char* filepath, int linesperfile){


  //Setup many archive types to be opened
  struct archive* a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_raw(a);

  //Open archive:
  archive_read_open_filename(a, filepath, 10240);

  //Process the file - split into pieces
  int err = split(a, filepath);

  //Close and finish read_process.
  archive_read_close(a);

  
  return err;
}


/*
  Assume file is so many bytes(for now)
  TODO: FIX this. Allow different byte sizes.
  TODO: CONVERT this. Specify lines, instead of byte size.
  TODO: output files.
  TODO: Add option to output an archive.

*/

/*
  threaddata_t - the information needed to be 
    passed in

*/
struct threaddata_t {
    int id; // id
    int status; // status

    char buf[BLKSIZE]; //Buncha data from a file 
    int size; //Amount of stuff
    pthread_t tid; // thread id
};

void* process_file(void* args){

  struct threaddata_t* data = args;
  #ifdef DEBUG
    fprintf(stderr, "Amount of data: %d\n", data->size);
  #endif

  char buf[3] = "abc";

  //Output this threads info, eventually to a unique file.
  write(STDOUT_FILENO, data->buf, data->size);

  #ifdef DEBUG
    fprintf(stderr, "Process %d starting\n", data->id);
  #endif

  //Process - Filestuff here
  srand(time(NULL) * data->id);
  sleep(rand() % 4); // Up to 4 seconds of sleep. For testing.

  fprintf(stderr, "Process %d ending\n", data->id);
  
  return &(data->status); //Did the thread finish?
}


int split(struct archive* a, const char* filename) {
  
  int numfiles = 2;  

  //One thread for each file
  //Array of threads
  //Allocate memory for thread info - deal with 2 files for now:
  struct threaddata_t* threadinfo = calloc(numfiles, sizeof(struct threaddata_t));

  if (!threadinfo) {
    fprintf(stderr, "Unable to allocate thread info\n");
    return 1;
  }
  //Set up archive:
  struct archive_entry* entry;
  archive_read_next_header(a, &entry);

  //The buffer of data for each thread, to be filled
  char buf[512];

  //Create a thread for each file:
  for (int i = 0; i < numfiles; ++i){
    threadinfo[i].id = i;
    int size_load = archive_read_data(a, buf, 512);
    #ifdef DEBUG
      fprintf(stderr, "Size load: %d\n", size_load);
    #endif
    if (size_load < 0)
      return 1;

    if (size_load == 0)
      break;

    threadinfo[i].size = size_load;
    #ifdef DEBUG
      fprintf(stderr, "Size: %d\n", threadinfo[i].size);
    #endif

    strcpy(threadinfo[i].buf, buf);
    write(STDERR_FILENO, threadinfo[i].buf, size_load);
    
    #ifdef DEBUG
    fprintf(stderr, "Thread %d ready\n", i);
    #endif
    //Here is where the thread branches off to do some work:
    pthread_create(&threadinfo[i].tid, NULL, process_file, &threadinfo[i]);
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
