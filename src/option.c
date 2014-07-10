/* option.c - parse command line arguments
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <libgen.h>     // basename()
#include <getopt.h>
#include <ctype.h>      // isprint()

#include "option.h"
#include "misc.h"
#include "debug.h"

#define DUMPI(e) fprintf(stderr, "%13s = %d\n", #e, option.e)
#define DUMPS(e) fprintf(stderr, "%13s = \"%s\"\n", #e, option.e)
void option_dump () {
    // DUMPI(debug);
    DUMPS(config);
    DUMPI(flash);
    DUMPI(help);
    DUMPI(stdin);
    DUMPI(verbose);
    DUMPS(message_text);
    DUMPS(phone_number);
}

static struct option longopts[] = { // please :sort this table after modify
    // {"debug",       no_argument,        &option.debug,      1},
    {"config",      required_argument,  0,                  'c'},
    {"flash",       no_argument,        0,                  'f'},
    {"help",        no_argument,        0,                  'h'},
    {"stdin",       no_argument,        0,                  's'},
    {"verbose",     no_argument,        0,                  'v'},
    {0, 0, 0, 0}
};

static const char *optstring = "c:dfhsv";
static char my_basename[64];
static char error_message[512];

static void print_logo () {
    printf(
        "JSS SMS Sender version %s\n"
        "(c) 2012-2014 JSS&Hayer - http://amisms.jss.hu\n"
        "\n"
        , VERSION
    );
}

void option_print_usage (char *fmt, ...) {
    print_logo();
        printf("for help: sms -h\n");
        printf("usage:    sms <phone number> [message text]\n");

    if (fmt != NULL) {
        chomp(fmt);
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, "\n");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        va_end(ap);
    }
    exit(1);
}

void option_print_help (char *fmt, ...) {
    print_logo();

    if (fmt != NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n\n");
        va_end(ap);
    } else {
        printf("usage: sms <phone number> <message text>\n");
        printf("  or   sms [OPTIONS] <phone number> [message text]\n");
        printf("\n");
        printf("Options\n");
        printf(" -v, --verbose               verbose output to stderr\n");
        printf(" -f, --flash                 flash SMS\n");
        printf(" -s, --stdin                 read message text from stdin\n");
    }
    exit(1);
}

int option_parse_args (int argc, char *argv[]) {
    scpy(my_basename, basename((char*)argv[0]));

    int getopt_parse_error = 0;
    opterr = 0; // tell getopt to avoid print internal error messages

    for (;;) {
        /* getopt_long stores the option index here. */
        int option_index = 0;

        int c = getopt_long(argc, argv, optstring, longopts, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
            case 'c':
                scpy(option.config, optarg);
                break;

            case 'f':
                option.flash = 1;
                break;

            case 'h':
                option_print_help(NULL);
                break;

            case 's':
                option.stdin = 1;
                break;

            case 'v':
                option.verbose++;
                break;

            case '?':
                getopt_parse_error = 1;
                if (optopt == 'c')
                    concatf(error_message, "Option -c need config file name!\n");
                else if (isprint(optopt))
                    concatf(error_message, "Unknown option: `-%c'\n", optopt);
                else if (optopt == '\0')
                    concatf(error_message, "Unknown option: `%s'\n", argv[optind-1]);
                else 
                    concatf(error_message, "Unknown option character `\\x%x'\n", optopt);
                break;

            default:
                abort();
        }
    }

    if (getopt_parse_error)
        option_print_usage(error_message);

    // 1st argument
    if (argc - optind < 1) {
        option_print_usage(NULL);
    }
    scpy(option.phone_number, argv[optind++]);

    // read message text from 2nd argument if the stdin mode not defined
    if (!option.stdin) {
        if (argc - optind < 1) {
            option_print_usage(
                "No message text defined in argument!\n"
                "You can also specify -s option to read text from stdin."
            );
        }
        scpy(option.message_text, argv[optind++]);
    } else {
        // read from stdin
        printf("Text? (press control+d to send)\n");
        option.message_text[0] = '\0';
        read_lines_from_stdin(option.message_text, sizeof(option.message_text));
    }

    return 0;
}

