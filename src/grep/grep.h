#ifndef GREP
#define GREP
#define GREP_FILE 10
#define GREP_TEMPLATEFILE 11
#define GREP_TEMPLATE 5

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct grep {
  size_t e, i, v, c, l, n, h, s, o, f;
  size_t check_str[100];
  size_t count_s;
  char strings[100][100];
  size_t count_f;
  char files[100][100];
  size_t count_o;
  char flag_o_strings[100][100];
  size_t flag_o_counter;
  size_t check_o;
  size_t print_all;
} grep;

void set_flags(grep *flags, int argc, char *argv[]);
void to_strings_to_files(grep *flags, int argc, char *argv[]);
int do_grep(grep *flags);
char *regexecute(grep *flags, char *fstr, size_t *printstr, char *cur_template);
int flag_o(grep *flags, char *fstr, size_t *printstr);
void printres(grep flags, size_t printstr, int count, char *fstr,
              char *filename);
void printetc(grep flags, int print_lines, char *filename);

#endif