/*
  makeargv.h

  Include file for makeargv() functions

  Michael Collard
  collard@uakron.edu
*/

#ifndef INCLUDED_MAKEARGV_H
#define INCLUDED_MAKEARGV_H

/* tokenizes the command line according to the delimiter into a null-terminated
   argv array of strings */
int makeargv(const char* commandline, const char* delimiter, char*** pnargv);

/* Free up memory allocated in makeargv */
void freemakeargv(char** nargv);

#endif

