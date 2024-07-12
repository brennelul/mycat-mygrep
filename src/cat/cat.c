#include "cat.h"

int main(int argc, char *argv[]) {
  int flagsarr[100];
  int isFlagOnArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  flags(flagsarr, argv, argc, isFlagOnArray);
  for (int i = 1; i < argc; i++) {
    if (flagsarr[i] == -1) {
      FILE *fp = fopen(argv[i], "r");
      print_file(fp, isFlagOnArray, argv[i]);
      if (i + 1 <= argc) flagsarr[i + 1] = -1;
    }
  }
  return 0;
}

void print_file(FILE *fp, const int *isFlagOnArray, char *filename) {
  if (fp != NULL) {
    char ch;
    FILE *temp = fopen(".temp", "w+");
    while ((ch = getc(fp)) != EOF) {
      fputc(ch, temp);
    }
    if (isFlagOnArray[0] == 1) {
      temp = squeeze(temp);
    }
    if (isFlagOnArray[1] == 1) {
      temp = visible(temp);
      temp = tab(temp);
    }
    if (isFlagOnArray[2] == 1) {
      temp = tab(temp);
    }
    if (isFlagOnArray[3] == 1) {
      int nbflag;
      if (isFlagOnArray[5] == 1 || isFlagOnArray[6] == 1)
        nbflag = 1;
      else
        nbflag = 0;
      temp = number_nonblank(temp, nbflag);
    }
    if (isFlagOnArray[4] == 1 && isFlagOnArray[3] != 1) {
      temp = number(temp);
    }
    if (isFlagOnArray[5] == 1) {
      temp = visible(temp);
      temp = dollar_sign(temp);
    }
    if (isFlagOnArray[6] == 1) {
      temp = dollar_sign(temp);
    }
    if (isFlagOnArray[7] == 1) {
      temp = visible(temp);
    }
    rewind(temp);
    while ((ch = getc(temp)) != EOF) {
      printf("%c", ch);
    }
    fclose(temp);
    fclose(fp);
  } else {
    fprintf(stderr, "cat: %s: No such file or directory\n", filename);
  }
  remove(".ntemp");
  remove(".temp");
}

FILE *number_nonblank(FILE *fp, int nbflag) {
  FILE *new = fopen(".ntemp", "w+");
  char ch, prev = '\n';
  int i = 1;
  rewind(fp);
  while ((ch = getc(fp)) != EOF) {
    if (ch != '\n' && prev == '\n') {
      fprintf(new, "%6d\t", i);
      i++;
    } else if (nbflag == 1 && prev == '\n' && ch == '\n') {
      fprintf(new, "%6c\t", 32);
    }
    fputc(ch, new);
    prev = ch;
  }
  return new;
}

FILE *number(FILE *fp) {
  FILE *new = fopen(".ntemp", "w+");
  char ch, prev = '\n';
  int i = 1;
  rewind(fp);
  while ((ch = getc(fp)) != EOF) {
    if (prev == '\n') {
      fprintf(new, "%6d\t", i);
      i++;
    }
    fputc(ch, new);
    prev = ch;
  }
  return new;
}

FILE *visible(FILE *fp) {
  FILE *new = fopen(".ntemp", "w+");
  char ch;
  rewind(fp);
  while ((ch = getc(fp)) != EOF) {
    if (ch < 32 && ch != 9 && ch != 10) {
      fputc('^', new);
      ch += 64;
    } else if (ch == 127) {
      fputc('^', new);
      ch = 63;
    }
    fputc(ch, new);
  }
  return new;
}

FILE *squeeze(FILE *fp) {
  FILE *new = fopen(".ntemp", "w+");
  char ch, prev = '\n', prevprev = ' ';
  rewind(fp);
  while ((ch = getc(fp)) != EOF) {
    if (!(ch == '\n' && prev == '\n' && prevprev == '\n')) fputc(ch, new);
    if (getc(fp) == EOF) break;
    fseek(fp, -1, SEEK_CUR);
    prevprev = prev;
    prev = ch;
  }
  return new;
}

FILE *dollar_sign(FILE *fp) {
  FILE *new = fopen(".ntemp", "w+");
  char ch;
  rewind(fp);
  while ((ch = getc(fp)) != EOF) {
    if (ch == '\n') fputc('$', new);
    fputc(ch, new);
  }
  return new;
}

FILE *tab(FILE *fp) {
  FILE *new = fopen(".ntemp", "w+");
  char ch;
  rewind(fp);
  while ((ch = getc(fp)) != EOF) {
    if (ch == '\t')
      fputs("^I", new);
    else
      fputc(ch, new);
  }
  return new;
}

void flags(int *flagsarr, char *argv[], int argc, int *isFlagOnArray) {
  char *flags;
  int errorflag = 0, count = 1, len;
  flagsarr[0] = 15;
  for (int i = 1; i < argc; i++) {
    if (strstr(argv[i], "-") != NULL && errorflag == 0) {
      flags = argv[i];
      len = strlen(flags);
      for (int j = 1; j < len; j++) {
        if (flags[j] == 's')
          TURNFLAGON(0)
        else if (flags[j] == 't')
          TURNFLAGON(1)
        else if (flags[j] == 'T')
          TURNFLAGON(2)
        else if (flags[j] == 'b')
          TURNFLAGON(3)
        else if (flags[j] == 'n')
          TURNFLAGON(4)
        else if (flags[j] == 'e')
          TURNFLAGON(5)
        else if (flags[j] == 'E')
          TURNFLAGON(6)
        else if (flags[j] == 'v')
          TURNFLAGON(7)
        else if (flags[j] == '-') {
          flagsarr[count] = 1;
          gnu_flags(isFlagOnArray, flags);
          j = len;
        } else {
          fprintf(stderr,
                  "cat: illegal option -- %c\nusage: cat [-benstv] [file ...]",
                  flags[j]);
          errorflag = -1;
          j = len;  // break
        }
      }
    } else if (errorflag == -1)
      break;
    else {
      flagsarr[i] = -1;
      errorflag = 1;
    }
  }
}

void gnu_flags(int *isFlagOnArray, char *flag) {
  if (strcmp("--number-nonblank", flag) == 0)
    isFlagOnArray[3] = 1;
  else if (strcmp("--number", flag) == 0)
    isFlagOnArray[4] = 1;
  else if (strcmp("--squeeze-blank", flag) == 0)
    isFlagOnArray[1] = 1;
}