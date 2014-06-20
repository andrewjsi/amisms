#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

static void print_logo () {
    printf("JSS SMS Sender\n"
           "(c) 2012-2014 JSS&Hayer - http://www.jsshayer.hu/libamievent\n"
           "\n"
    );
}

void help (char *fmt, ...) {
    print_logo();
    if (fmt != NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n\n");
        va_end(ap);
    } else {
        printf("usage: sms <phone number> [text]\n");
    }
    exit(1);
}

