#include "cmdline.h"

static char doc[] = "A toy parser for Palm Docs used in HaoDoo.";
const char* argp_program_version = "0.0.1";
static char args_doc[] = "PATH";

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *) state->input;

  switch (key) {
  case 'v':
    arguments->verbose = 1;
    break;
  case 'o':
    arguments->outfile_path = arg;
    break;

  case 'h':
    return 1;

  case ARGP_KEY_ARG:
    if (state->arg_num >= POS_ARGS_COUNT) {
      /* Too many arguments. */
      argp_usage(state);
      // return ARGP_ERR_
    }

    arguments->args[state->arg_num] = arg;

    break;

  case ARGP_KEY_END:
    if (state->arg_num < POS_ARGS_COUNT) {
      /* Not enough arguments. */
      argp_usage(state);
    }
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output" },
  {"output",   'o', "FILE", 0,
   "Output to FILE instead of standard output" },
  { 0 }
};

static struct argp argp = {
  .options = options,
  .parser = parse_opt,
  .args_doc = args_doc,
  .doc = doc
};

error_t parse_args(int argc, char** argv, struct arguments* args) {
    return argp_parse(&argp, argc, argv, 0, 0, args);
}
