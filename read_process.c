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

#include <stdbool.h> //Booleans
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
#include <stdlib.h> //itoa

//Take in 512 bytes at a time
#define BLKSIZE 512
#define MAX_LINESIZE 5122

/*Prototype for process: */
int split(struct archive* a, const char*, int);


//TODO add options for argument to process.
int read_process(const char* filepath, int linesperfile){


  //Setup many archive types to be opened
  struct archive* a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_raw(a);

  //Open archive:
  archive_read_open_filename(a, filepath, 10240);

  //Process the file - split into pieces
  int err = split(a, filepath, linesperfile);

  if (err != 0){
    fprintf(stderr, "An error ocurred in split()\n");
  }

  //Close and finish read_process.
  archive_read_close(a);
  return err;
}


/*
  threaddata_t - the information needed to be 
    passed in
    Also: A linked list that will contain SIZE bytes of charaters
*/
struct threaddata_t{
    int id; // id
    int status; // status
    pthread_t tid; // thread id


    int fd; //Filedescriptor - for writing data
    char* buf; //Buncha data from a file 
    int size; //Amount of stuff


    struct threaddata_t* pnxt_thread;
};

void* process_file(void* args){
  #ifdef DEBUG
    fprintf(stderr, "Process %d starting\n");//, data->id);
  #endif


  struct threaddata_t* data = args;

  //Output this threads info to file descriptor it holds
  write(data->fd, data->buf, data->size);

  #ifdef DEBUG
    fprintf(stderr, "Process %d Finished: Data written to file.\n", data->id);
  #endif

  return &(data->status); 
}

/*
  bufnode: A linked list of blocks of data
*/
struct bufnode{
  char buf[BLKSIZE];
  int size;
  struct bufnode* next;
  struct bufnode* prev;
};

//Like bufnode, but blocks have no set size, but a set amount of lines(\n's):
struct linenode{
  char* buf;
  int size;
  struct linenode* next;
};

//void newname - make a file go from file.extension to file-index.

void new_name(char* newname, const char* oldname, int file_index);

 
int split(struct archive* a, const char* filename, int linesperfile) {
  //One thread for each file
  //Linked list of threads
  struct threaddata_t* head_thread = NULL;

  //Set up archive:
  struct archive_entry* entry;
  archive_read_next_header(a, &entry);

  //The buffer of data for each thread, to be filled
  char buf[BLKSIZE];
  int size_load = 0;
  int lastsize = 0;

  struct bufnode* head_buf = NULL;
  struct bufnode* tail_buf = NULL;

  int numblocks = 0;
  bool looped = false;
  //Preload buffers into linkedlist of buffers:
  while(1){
    size_load = archive_read_data(a, buf, 512);

    if (size_load < 0)
      return 1;
    if (size_load == 0)
      break;
    lastsize = size_load;

    struct bufnode* save = head_buf;

    #ifdef DEBUG
    fprintf(stderr, "Block number: %d\nSize of load: %d\n\n", numblocks, size_load);
    #endif

    head_buf = malloc(sizeof(struct bufnode));
    head_buf->next = save;

    if (looped == false){
      tail_buf = head_buf;
      looped = true;
    } else {
      save->prev = head_buf;
    }

    //                ||                                            /\
    //WHY THE HELL IS \/ NOT SAVING TO SIZE LOAD                    || 
    head_buf->size = size_load;
    

    #ifdef DEBUG
    fprintf(stderr, "%d == %d",head_buf->size, size_load);
    #endif

    strcpy(head_buf->buf, buf);
    ++numblocks;
  }

  head_buf->prev = NULL;
  head_buf->size = lastsize;

  #ifdef DEBUG
  fprintf(stderr, "Head size: %d\n", head_buf->size);
  #endif

  #ifdef DEBUG
  fprintf(stderr, "lastsize: %d\n", lastsize);
  #endif
  //Set up to turn lined list of buffers into linked list of lines:

  int bufindex = 0;
  int numlines = 0;
  int sizeoflines = 0;
  int numfiles = 0;  

  int this_loadsize = tail_buf->size;
  char* lineblock = malloc(linesperfile * MAX_LINESIZE);
  struct linenode* head_line = NULL;

  //Turn buffers into linked list of lines
  while(1){
    
    if (numlines == linesperfile){
      struct linenode* save = head_line;
      head_line = malloc(sizeof(struct linenode));
      head_line->size = sizeoflines; 
      head_line->buf = lineblock == NULL ? NULL : strndup(lineblock, sizeoflines);
      head_line->next = save;
      #ifdef DEBUG
      fprintf(stderr, "Block of line number: %d\nSize of block: %d\n\n", numfiles, sizeoflines);
      #endif
      //reset variables
      sizeoflines = 0;
      numlines = 0;    

      //A new file must be created for this
      ++numfiles;
    }
    if (bufindex >= this_loadsize){
      //Go to next node:
      struct bufnode* save = tail_buf->prev;
  
      #ifdef DEBUG
      fprintf(stderr, "End of buffer\n");
      #endif


      if (save == NULL){ //Put last set of lines in
        
        struct linenode* save_l = head_line;
        head_line = malloc(sizeof(struct linenode));
        head_line->size = sizeoflines; 
        head_line->buf = lineblock == NULL ? NULL : strndup(lineblock, sizeoflines);        
        head_line->next = save_l;
      
      #ifdef DEBUG
      fprintf(stderr, "END:Block of line number: %d\nSize of block: %d\n\n", numfiles, sizeoflines);
      #endif

        free(tail_buf);
        tail_buf = save;
        ++numfiles;

        break;
      } 
      //Otherwise:
      free(tail_buf);
      tail_buf = save;

      //reset variables
      bufindex = 0;
      this_loadsize = tail_buf->size;
      fprintf(stderr, "this_loadsize: %d\n", this_loadsize);
    }

    //Now push 
    lineblock[sizeoflines] = tail_buf->buf[bufindex];
    if (tail_buf->buf[bufindex] == '\n'){
      ++numlines;
    }
    ++sizeoflines;
    ++bufindex;
  }
  free(lineblock);


  #ifdef DEBUG
  fprintf(stderr, "out of loop 2\n");
  #endif

  struct threaddata_t* threadarray = calloc(numfiles, sizeof(struct threaddata_t));


  if (!threadarray) {
    fprintf(stderr, "Unable to allocate thread array\n");
    return 1;
  }

  //save=head_thread
  //head thread->malloc
  //head thread->pnext_thread = save 
  //Create a thread for each file:
  //Loop until as many files are created as needed:

  // Because I can't figure out file names, I am going to use a through z.

  char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < numfiles; ++i) {
   int fileno = i + 1;



    //Create thread after there is data to be put in:
    threadarray[i].buf = strndup(head_line->buf, head_line->size); 
    threadarray[i].id = i;
    threadarray[i].size = head_line->size;
    fprintf(stderr, "Time to start this fukin' party again goddammit\n");
 
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
    char newfname[4];// = alphabet[i];// = malloc(filesize); 
  
    sprintf(newfname, "%c.txt", alphabet[i]); 

    #ifdef DEBUG
    fprintf(stderr, "New file name: %s", newfname);
    #endif

    //Now open file based off name
    int newfd = open(newfname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //Finally, pass this to the thread
    
//    free(newfname);

    if (newfd < 0){
      fprintf(stderr, "New file %s could not be create\n", newfname);
      return 1;
    }
    threadarray[i].fd = newfd; 

    #ifdef DEBUG
    fprintf(stderr, "Thread %d ready\n", i);
    #endif

    struct linenode* save_l = head_line->next;
    free(head_line);
    head_line = save_l;

    #ifdef DEBUG
    fprintf(stderr, "Did the fucking linked list cause another fucking error?\n");
    #endif
    //Here is where the thread branches off to do some work:
    pthread_create(&threadarray[i].tid, NULL, process_file, &threadarray[i]);  
    
  }

  //Wait for threads  to finish  
  for (int i = 0; i < numfiles; ++i){

    #ifdef DEBUG
    fprintf(stderr, "read() %d waiting\n", threadarray[i].id);
    #endif
    
    pthread_join(threadarray[i].tid, NULL);

    #ifdef DEBUG
    fprintf(stderr, "read() %d finished\n", threadarray[i].id);
    #endif
  }

  //Go through every thread and delete it. Also - when dynamic buffers
  //happen, make sure buffer is deleted too!

  free(threadarray);

  return 0;
}



void new_name(char* newname, const char* oldname, int file_index){
  fprintf(stderr, "Hello\n\n\n\nHELLOFKER\n");

  oldname = strdup(newname);
  char buffer[20];
  snprintf(buffer,12, "%d", file_index);
  strcat(newname, buffer);
  fprintf(stderr, "Hello\n\n\n\nHELLOFKER\n");
//  sprintf(newname, "%s(%d)",oldname, file_index);
}


