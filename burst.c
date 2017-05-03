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

#include "makeargv.h" // Make argument array
#include <unistd.h> // Fork()
#include <stdlib.h> // Free()
#include "read_process.h"
#ifdef DEBUG
#include <stdio.h> //printf
#endif


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

    
//    parse(nargv);//

    //First argument 
    //500 default of lines per file:
    read_process(nargv[0], 100);
    

    /* Then create files, and use fork to split into different processes */

    fork();



    free(nargv[0]);
    return 0;
}
