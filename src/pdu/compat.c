#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "compat.h"

char *strcpyo(char *dest, const char *src)
{
  size_t i;

  for (i = 0; src[i] != '\0'; i++)
    dest[i] = src[i];

  dest[i] = '\0';

  return dest;
}

void writelogfile (int severity, int trouble, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

void writelogfile0 (int severity, int trouble, char *text) {
    writelogfile(severity, trouble, "%s", text);
}

char *tb_sprintf (char* format, ...) {
    va_list argp;

    va_start(argp, format);
    vsnprintf(tb, sizeof(tb), format, argp);
    va_end(argp);

    return tb;
}

