#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int
main()
{
  free(get_file_name(NULL, NULL));
  exit(EXIT_SUCCESS);
}
