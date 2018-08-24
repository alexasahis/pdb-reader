#ifndef _PDB_CMDLINE_H_
#define _PDB_CMDLINE_H_

#include <argp.h>

enum { POS_ARGS_COUNT = 1 };

struct arguments {
  char* args[POS_ARGS_COUNT];
  char* outfile_path;
  int verbose;
};

error_t parse_args(int, char**, struct arguments*);

#endif  // _PDB_CMDLINE_H_
