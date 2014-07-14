/* test.c
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "option.h"
#include "debug.h"

void dump () {
    debi(option.flash);
    debi(option.help);
    debi(option.stdin);
    debi(option.verbose);
    debs(option.config);
    debs(option.message_text);
    debs(option.phone_number);
}

int main (int argc, char *argv[]) {
    option_parse_args(argc, argv);

    printf("After parsing:\n");
    dump();
    printf("\n");

    return 0;
}

