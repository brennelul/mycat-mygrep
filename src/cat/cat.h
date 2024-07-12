#ifndef CAT_H
#define CAT_H

#include <stdio.h>
#include <string.h>
#define TURNFLAGON(x)     \
  {                       \
    isFlagOnArray[x] = 1; \
    flagsarr[count] = 1;  \
    count++;              \
  }
void print_file(FILE *fp, const int *isFlagOnArray, char *filename);
void flags(int *flagsarr, char *argv[], int argc, int *isFlagOnArray);
void gnu_flags(int *isFlagOnArray, char *flag);
FILE *number(FILE *fp);
FILE *number_nonblank(FILE *fp, int nbflag);
FILE *visible(FILE *fp);
FILE *dollar_sign(FILE *fp);
FILE *squeeze(FILE *fp);
FILE *tab(FILE *fp);
#endif