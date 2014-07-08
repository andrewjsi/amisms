/* option.h - parse command line arguments
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

void option_set_basename (const char *bn);
int option_parse_args (int argc, char *argv[]);
void option_dump ();

// if you modify this struct, please consult the option_dump() function in option.c
struct option_t {
    char phone_number[128];
    char message_text[1024];
    char config[128];
    int help;
    int flash;
    int verbose;
    int stdin;
};

// this variable holds the all options
struct option_t option;

