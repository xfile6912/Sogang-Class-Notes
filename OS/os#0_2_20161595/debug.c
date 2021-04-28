#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

void
debug_panic (const char *file, int line, const char *function,
             const char *message, ...)
{
  va_list args;

  printf ("ABORT at %s:%d in %s(): ", file, line, function);

  va_start (args, message);
  vprintf (message, args);
  printf ("\n");
  va_end (args);
  exit(1);
}

