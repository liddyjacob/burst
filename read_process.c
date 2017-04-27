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
    struct threaddata_t* pnxt_thread;
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
  
//  int numfiles = 2;  

  //One thread for each file
  //Linked list of threads
  //create head of linked list:
  struct threaddata_t* head_thread = NULL;//malloc(sizeof(struct threaddata_t));

  //Set up archive:
  struct archive_entry* entry;
  archive_read_next_header(a, &entry);

  //The buffer of data for each thread, to be filled
  char buf[512];

//  struct threaddata_t* next_thread = head_thread;

    int size_load = 1;

  int numfiles = 0;

  //Create a thread for each file:
  //Loop until as many files are created as needed:
  struct threaddata_t* next_thread = head_thread;
  while (1){
    int size_load = archive_read_data(a, buf, 512);

    #ifdef DEBUG
      fprintf(stderr, "Size load: %d\n", size_load);
    #endif

    if (size_load < 0)
      return 1;
    if (size_load == 0)
      break;


    //Create thread after there is data to be put in:
    next_thread = malloc(sizeof(struct threaddata_t));
    if (!next_thread) {
      fprintf(stderr, "Unable to allocate thread info\n");
      return 1;
    } 

    next_thread->id = numfiles;
    next_thread->size = size_load;


    #ifdef DEBUG
      fprintf(stderr, "Size: %d\n", next_thread->size);
    #endif

    strcpy(next_thread->buf, buf);
    
    #ifdef DEBUG
    fprintf(stderr, "Thread %d ready\n", numfiles);
    #endif

    //Here is where the thread branches off to do some work:
    pthread_create(&next_thread->tid, NULL, process_file, next_thread);
    
    next_thread = next_thread->pnxt_thread;
    next_thread = NULL;
    numfiles++;
  }

  #ifdef DEBUG
  fprintf(stderr, "Out of loop. Files: %d\n", numfiles);
  #endif

  struct threaddata_t* curr_thread = head_thread;

  #ifdef DEBUG
  printf(stderr, "next thread = head thread\n");
  #endif

  for (int i = 0; i < numfiles; ++i){

    #ifdef DEBUG
    fprintf(stderr, "read() %d waiting\n", next_thread->id);
    #endif
    
    pthread_join(next_thread->tid, NULL);

    #ifdef DEBUG
    fprintf(stderr, "read() %d finished\n", next_thread->id);
    #endif
    next_thread = next_thread->pnxt_thread;
  }



  //Free threadinfo array memory
  //free(threadinfo);   


  return 0;
}
