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
#include <math.h> // log
#include <archive.h> //libarchive - 
#include <archive_entry.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>//strstr and others.
#include <time.h>
#include <errno.h> //For fileno? 
#include <fcntl.h>
#include <sys/stat.h> //S_IRUSR
#include <stdlib.h>

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
  TODO: Add option to output an archive.

*/

/*
  threaddata_t - the information needed to be 
    passed in

*/
struct threaddata_t{
    int id; // id
    int status; // status

    int fd; //Filedescriptor - for writing data
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

  //Output this threads info to file descriptor it holds
  write(data->fd, data->buf, data->size);

  #ifdef DEBUG
    fprintf(stderr, "Process %d starting\n", data->id);
  #endif

  fprintf(stderr, "Process %d ending\n", data->id);
  
  return &(data->status); //Did the thread finish?
}


//void newname - make a file go from file.extension to file-index.

void new_name(char* newname, char* oldname, int file_index);
 
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

  int numfiles = 0;

  //save=head_thread
  //head thread->malloc
  //head thread->pnext_thread = save 
  //Create a thread for each file:
  //Loop until as many files are created as needed:
  while (1){
    
    struct threaddata_t* save = head_thread;

    int size_load = archive_read_data(a, buf, 512);

    if (size_load < 0)
      return 1;
    if (size_load == 0)
      break;

    //Create thread after there is data to be put in:
    head_thread = malloc(sizeof(struct threaddata_t));
    head_thread->pnxt_thread = save;//Put in the next thread
    if (!head_thread) {
      fprintf(stderr, "Unable to allocate thread info\n");
      return 1;
    } 

    head_thread->id = numfiles;
    head_thread->size = size_load;

    //Create the name of the file
    /*
      file(index).ext
      
      index = numfiles + 1
      malloc(sizeof(filename) + (int)log(numfiles + 1) + 1); 
      Explination:

  sizeof(filename)       -- must reserve memory for sizeof(file.ext)
  (int)log(numfiles+1)   -- must reserve memory for log(index) -- see table
  +1                     -- must reserve memory for rounding down of ^
       n 1 2 3 4 5 6 7 8 9 10
intlog n 0 0 0 0 0 0 0 0 0 1

    */
    //Delete newfname when filedescriptor has been created-
    int filesize = (int)sizeof(filename) + (int)log(numfiles + 1) + 2;
    char* newfname = malloc(filesize); 
    new_name(newfname, filename, numfiles + 1);
    
    #ifdef DEBUG
    fprintf(stderr, "New file name: %s", newfname);
    #endif

    //Now open file based off name
    int newfd = open(newfname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //Finally, pass this to the thread
    
    free(newfname);

    if (newfd < 0){
      fprintf(stderr, "New file %s could not be create\n", newfname);
      return 1;
    }
    head_thread->fd = newfd; 

    strcpy(head_thread->buf, buf);
    
    #ifdef DEBUG
    fprintf(stderr, "Thread %d ready\n", numfiles);
    #endif

    //Here is where the thread branches off to do some work:
    pthread_create(&head_thread->tid, NULL, process_file, head_thread);  
    numfiles++;
  }

  struct threaddata_t* curr_thread = head_thread;

  for (int i = 0; i < numfiles; ++i){

    #ifdef DEBUG
    fprintf(stderr, "read() %d waiting\n", curr_thread->id);
    #endif
    
    pthread_join(curr_thread->tid, NULL);

    #ifdef DEBUG
    fprintf(stderr, "read() %d finished\n", curr_thread->id);
    #endif
    curr_thread = curr_thread->pnxt_thread;
  }

  //Go through every thread and delete it. Also - when dynamic buffers
  //happen, make sure buffer is deleted too!


  //TODO: Close the damn files.
  for (int i = 0; i < numfiles; i++){

    #ifdef DEBUG
    fprintf(stderr, "Deleting thread %d\n", i);
    #endif
    if (close(head_thread->fd) == -1)
      fprintf(stderr, "Filedescriptor %d could not close\n", head_thread->fd);

    struct threaddata_t* save = head_thread->pnxt_thread;

    free(head_thread);

    head_thread = save;
  }

  curr_thread = head_thread; 


  return 0;
}



void new_name(char* newname, char* oldname, int file_index){
    //const char lookfor = '.'; //Look for a .

//      char* ret;
//      ret = strstr(oldname, ".");
  int i=0;
  while(*(oldname + i) != '.'){
    newname[i] = oldname[i];
    ++i;
  }

  char* buffer = malloc(log(file_index) + 3);  
  int n;
  n = sprintf(buffer, "-%d", file_index);
 
  strcat(newname, buffer);
  strcat(newname, strstr(oldname, "."));
  
  free(buffer);
}

