#include "grep.h"

int main(int argc, char *argv[]) {
  grep flags = {0};
  set_flags(&flags, argc, argv);
  to_strings_to_files(&flags, argc, argv);
  int return_value = do_grep(&flags);
  return return_value == 0 ? 1 : 0;
}

void set_flags(grep *flags, int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && flags->check_str[i] == 0) {
      size_t len = strlen(argv[i]);
      flags->check_str[i] = -1;
      if (len == 1) break;
      for (size_t j = 1; j < len; j++) {
        if (argv[i][j] == 'e') {
          flags->e = 1;
          flags->check_str[i + 1] = GREP_TEMPLATE;
        } else if (argv[i][j] == 'i')
          flags->i = 1;
        else if (argv[i][j] == 'v')
          flags->v = 1;
        else if (argv[i][j] == 'c')
          flags->c = 1;
        else if (argv[i][j] == 'l')
          flags->l = 1;
        else if (argv[i][j] == 'n')
          flags->n = 1;
        else if (argv[i][j] == 'h')
          flags->h = 1;
        else if (argv[i][j] == 's')
          flags->s = 1;
        else if (argv[i][j] == 'o')
          flags->o = 1;
        else if (argv[i][j] == 'f') {
          flags->f = 1;
          flags->check_str[i + 1] = GREP_TEMPLATEFILE;
        } else {
          fprintf(stderr, "grep: invalid option -- %c\n", argv[i][j]);
          exit(1);
        }
      }
    }
  }
  size_t first_time = 1;
  for (int i = 1; i < argc; i++) {
    if (flags->check_str[i] == 0) {  // 5 = template, 10 = filename
      size_t temporfile =
          flags->e == 1 || flags->f == 1 ? GREP_FILE : GREP_TEMPLATE;
      flags->check_str[i] = first_time == 1 ? temporfile : GREP_FILE;
      first_time = 0;
    }
  }
}

void to_strings_to_files(grep *flags, int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (flags->check_str[i] == GREP_TEMPLATE) {
      strcpy(flags->strings[flags->count_s], argv[i]);
      flags->count_s++;
    } else if (flags->check_str[i] == GREP_FILE) {
      strcpy(flags->files[flags->count_f], argv[i]);
      flags->count_f++;
    } else if (flags->check_str[i] == GREP_TEMPLATEFILE) {
      FILE *fp = fopen(argv[i], "r");
      if (fp != NULL) {
        char temp[1000];
        while (fgets(temp, 1000, fp) != NULL && strcmp(temp, "\0")) {
          if (temp[1] == '\n') flags->print_all = 1;
          temp[strcspn(temp, "\n")] = '\0';
          strcpy(flags->strings[flags->count_s], temp);
          flags->count_s++;
        }
        fclose(fp);
      } else if (flags->s != 1)
        fprintf(stderr, "grep: %s: No such file or directroy\n", argv[i]);
    }
  }
}

int do_grep(grep *flags) {
  size_t count = 1;
  for (size_t i = 0; i < flags->count_f; i++) {
    FILE *fp = fopen(flags->files[i], "r");
    if (fp == NULL && flags->s == 0) {
      fprintf(stderr, "grep: %s: No such file or directroy\n", flags->files[i]);
      continue;
    }
    char fstr[1000];
    count = 1;
    size_t printstr = 0, print_lines = 0;
    while (feof(fp) == 0 && getc(fp) != EOF) {
      printstr = 0;
      fseek(fp, -1, SEEK_CUR);
      fgets(fstr, 1000, fp);
      size_t old_o_count = flags->count_o;
      if (flags->o == 1 && flags->v == 0) {
        printstr = flag_o(flags, fstr, &printstr);
        if (flags->print_all) printstr = 1;
        if (flags->print_all && !flags->v) printstr = 0;
      } else {
        for (size_t j = 0; j < flags->count_s; j++) {
          regexecute(flags, fstr, &printstr, flags->strings[j]);
        }
      }
      if (printstr == 1 && flags->v == 1) {
        printstr = 0;
      } else if (printstr == 0 && flags->v == 1 && flags->print_all == 0) {
        printstr = 1;
      }
      if (flags->o == 0 || (flags->o == 1 && flags->v == 1)) {
        old_o_count = -1;
      }
      if (printstr == 1 || (flags->print_all == 1 && flags->v == 0)) {
        print_lines++;
      }
      for (size_t j = 0; j < flags->count_o - old_o_count; j++)
        printres(*flags, printstr, count, fstr, flags->files[i]);
      count++;
    }
    printetc(*flags, print_lines, flags->files[i]);
    fclose(fp);
  }
  return count;
}

char *regexecute(grep *flags, char *fstr, size_t *printstr,
                 char *cur_template) {
  char *first_match = NULL;
  regex_t regex;
  regmatch_t match;
  size_t error;
  if (flags->i == 1) {
    error = regcomp(&regex, cur_template, REG_ICASE);
  } else {
    error = regcomp(&regex, cur_template, REG_EXTENDED);
  }
  if (error == 0) {
    error = regexec(&regex, fstr, 1, &match, 0);
  }
  if (error == 0) {
    *printstr = 1;
    first_match = fstr + match.rm_so;
  }
  if (flags->o == 1 && flags->v == 0 && first_match != NULL && *printstr == 1) {
    flags->check_o = 1;
  }
  if (strcmp(cur_template, ".") == 0) {
    if (strcmp(fstr, "\n"))
      first_match = fstr;
    else {
      *printstr = 0;
    }
  }
  if (flags->print_all == 1 && flags->o == 0) {
    *printstr = 1;
  } else if (flags->print_all == 1 && flags->o == 1 && flags->c == 0) {
    flags->check_o = 0;
    first_match = NULL;
  }
  regfree(&regex);
  return first_match;
}

int flag_o(grep *flags, char *fstr, size_t *printstr) {
  char *ptr1 = NULL;
  for (size_t i = 0; i < flags->count_s; i++) {
    while ((ptr1 = regexecute(flags, fstr, printstr, flags->strings[i])) !=
           NULL) {
      char temp[1000] = "";
      memcpy(temp, ptr1, strlen(flags->strings[i]));
      memcpy(fstr, ptr1 + strlen(flags->strings[i]), strlen(ptr1));
      if (flags->c == 0) {
        strcpy(flags->flag_o_strings[flags->count_o], temp);
        flags->count_o++;
      }
    }
  }
  return *printstr;
}

void printres(grep flags, size_t printstr, int count, char *fstr,
              char *filename) {
  if (printstr == 1 && flags.c == 0 && flags.l == 0) {
    if (flags.count_f > 1 && printstr == 1 && flags.h == 0) {
      printf("%s:", filename);
    }
    if (flags.n == 1 && printstr == 1) {
      printf("%d:", count);
    }
    if (printstr == 1 && ((flags.o == 0) || (flags.o == 1 && flags.v == 1))) {
      printf("%s", fstr);
    }
    if (flags.o == 0 && printstr == 1 && strchr(fstr, '\n') == NULL) {
      printf("%c", '\n');
    }
  }
  if (printstr == 1 && flags.o == 1 && flags.v == 0 && flags.l == 0 &&
      flags.c == 0) {
    printf("%s\n", flags.flag_o_strings[flags.count_o - 1]);
  }
}

void printetc(grep flags, int print_lines, char *filename) {
  if (flags.c == 1) {
    if (flags.l == 1 && print_lines > 0) {
      print_lines = 1;
    }
    if (flags.count_f > 1 && flags.h == 0) {
      printf("%s:", filename);
    }
    printf("%d\n", print_lines);
  }
  if (flags.l == 1 && print_lines > 0) {
    printf("%s\n", filename);
  }
}