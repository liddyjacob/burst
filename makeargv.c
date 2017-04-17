/*
  makeargv.c

  Makes an argument list, similar to argv

  Michael Collard
  collard@uakron.edu
*/

#include "makeargv.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int makeargv(const char* str, const char* delimiter, char*** pnargv) {

  /* check for invalid parameters */
  if ((str == 0) || (delimiter == 0) || (pnargv == 0)) {
    errno = EINVAL;
    return -1;
  }
  
  /* copy command string so we can alter it */
  char* s = malloc(strlen(str) + 1);
  if (!s) {
    return -1;
  }
  strcpy(s, str);

  /* find the number of tokens in the string */
  int numtokens = 0;
  if (strtok(s, delimiter)) {
    ++numtokens;
    while (strtok(NULL, delimiter)) {
      ++numtokens;
    }
  }

  /* create an null-terminated argument array of strings */
  (*pnargv) = malloc((numtokens + 1) * sizeof(char*));
  if (!(*pnargv)) {
    free(s);
    return -1;
  }

  /* restore the original string */
  strcpy(s, str);
  
  /* fill up the argument array with pointers to strings in s */
  if ((*pnargv)[0] = strtok(s, delimiter))
    for (int i = 1; i < numtokens; ++i)
      (*pnargv)[i] = strtok(NULL, delimiter);
  (*pnargv)[numtokens] = NULL;

  return numtokens;
}

/* frees up memory allocated using makeargv */
void freemakeargv(char** nargv) {

  free(nargv[0]);
  free(nargv);
}

