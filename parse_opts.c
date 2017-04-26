/*
  parse_opts.c

  Parse the options and 
  return a flag for option handling

  Based 
  
  Jacob Liddy 
*/

#include <stdio.h>
#include <getopt.h>

extern char* optarg;
extern int optind, opterr, optopt;

struct option longopts[] = {
  { "help",               no_argument,       NULL, 'h'},
  { "version",            no_argument,       NULL, 'V'},

  { "encoding",           required_argument, NULL, 'E'},
  { "src-encoding",       required_argument, NULL, 'S'},

  { "no-xml-declaration", no_argument,       NULL, 'N'},

  { "verbose",            no_argument,       NULL, 'v'},

  { "language",           required_argument, NULL, 'l'},
  0
};

int parse_opts(int argc, char** argv) {

  int flag = 0;
  int option_result;
  int oc;
  int longindex;
  while ((oc = getopt_long(argc, argv, "hVE:S:XNvl:", longopts, &longindex)) != -1) {

    // invalid options
    if (oc == '?') {
      fprintf(stderr, "-%c\n", oc);
      continue;
    }

    // keep track of which options
    flag |= (1 << longindex);

    // where we are at in the options
    fprintf(stderr, "%d\t%c\n", longindex, oc);

    // generic argument reporter
    fprintf(stderr, "--%s", longopts[longindex].name);
    if (longopts[longindex].has_arg == required_argument)
      fprintf(stderr, "=%s", optarg);
    fprintf(stderr, "\n");

    // action based argument reporter
    switch (oc) {
    case 'h':
      fprintf(stderr, "--help\n");
      break;
    case 'V':
      fprintf(stderr, "--version\n");
      break;
    case 'E':
      fprintf(stderr, "--encoding=%s\n", optarg);
      break;
    case 'S':
      fprintf(stderr, "--src-encoding=%s\n", optarg);
      break;
    case 'N':
      fprintf(stderr, "--no-xml-declaration=%s\n", optarg);
      break;
    case 'v':
      fprintf(stderr, "--verbose\n");
      break;
    case 'l':
      fprintf(stderr, "--language=%s\n", optarg);
      break;

    default:

      break;
    };
  }

  // end result of flag
  fprintf(stderr, "flag=%04x\n", flag);

  return 0;
}
             
