#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common.h"

char *
get_file_name(char *prefix, char *suffix)
{
  char *name;
  int prelen, suflen, sumlen;
  time_t current;
  struct tm *date;

  prelen = strlen((prefix == NULL) ? "" : prefix);
  suflen = strlen((suffix == NULL) ? "tmp" : suffix);
  sumlen = prelen + suflen + 24;
  name = (char *)malloc(sumlen * sizeof(char));
  time(&current);
  date = localtime(&current);
  sprintf(name,
	  "%s-%04d-%02d-%02d-%ld.%s",
	  (prefix == NULL) ? "" : prefix,
	  date->tm_year + 1900,
	  date->tm_mon + 1,
	  date->tm_mday,
	  current,
	  (suffix == NULL) ? "tmp" : suffix);
  return name;
}
