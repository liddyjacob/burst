/*
  read_process.h

  Include file for read_process() functions

  Jacob Liddy
  jpl61@zips.uakron.edu
*/

#ifndef INCLUDED_READ_PROCESS_H
#define INCLUDED_READ_PROCESS_H

/* Read the file, then fork it in split*/
int read_process(const char* filepath, int linesperfile, bool verbos);

/* Fork the file depending on linesperfile */
void split(int, const char*);

#endif

