/*
  Project: Burst

  Contributers:
  Jacob Liddy
  mlcollard
  av95

  Description: 
    Split a file into several smaller files, process them individually

  Contributer Description:
--------------------------------------------|
  Jacob Liddy: Student - designer of project|
                                            |
  mlcollard: Professor, many lines of code  |
    are borrowed from in class examples, and|
    textbook.                               |
                                            |
  av95: Teaching Assistant.                 |
--------------------------------------------|

*/

#include <stdbool.h>//Bools
#include "makeargv.h" // Make argument array
#include <unistd.h> // Fork()
#include <stdlib.h> // Free()
#include "read_process.h"
#include <stdio.h> //printf


//TODO: THIS is really messy - fix it.
int main(int argc, char* argv[]){
    /* First do error handling, input processing*/

    
    char** nargv;
    int n_tokens = makeargv(argv[1], " ", &nargv); // How many arguments

    //Display arguments, for debugging
    #ifdef DEBUG
    for (int i=0; i < n_tokens; ++i)
      printf("|%s|\n", nargv[i]);
    #endif


    int lines = 500;
    bool verbose = false;

    if (n_tokens > 1){
    
    if (strcmp(nargv[1], "-l") == 0 ){

        if (n_tokens <= 2){
            fprintf(stderr, "Usage: -l (Number of lines)");
            return 1;
        }
 
        char* input = strdup(nargv[2]);
        
        int ln = strlen(input);

         

        for( int i = 0; i < ln; i++){
          if( !isdigit(input[i]) ){
            fprintf(stderr, "Usage: -l (Number of lines)");
            return 1;
          }
        }

        lines = atoi(input);
    } else if (strcmp(nargv[1], "-v") == 0 ){
      verbose = true;
    } else {
      fprintf(stderr, "burst: invalid option -- %s\n", nargv[1]);
      return 1;
    }
    
    }
    
    if (n_tokens > 2){ 

    if (strcmp(nargv[2], "-l") == 0 ){

        if (n_tokens == 3){
          fprintf(stderr, "Usage: -l (Number of lines)\n");
          return 1;
        }
        char* input = strdup(nargv[3]);
        
        int ln = strlen(input);

         


        for( int i = 0; i < ln; i++){
          if( !isdigit(input[i]) ){
            fprintf(stderr, "Usage: -l (Number of lines)\n");
            return 1;
          }
        }

        lines = atoi(input);
    }
    if (n_tokens >= 4)
    if (strcmp(nargv[3], "-v") == 0){
      verbose = true;
    } else if (strcmp(nargv[2], "-l") != 0){
      fprintf(stderr, "burst: invalid option -- %s\n", nargv[3]);
      return 1;
    }
 
    }
   
#ifdef DEBUG
fprintf(stderr, "Entering the function\n");
#endif
    read_process(nargv[0], lines, verbose);
    

    /* Then create files, and use fork to split into different processes */

    free(nargv[0]);
    return 0;
}
