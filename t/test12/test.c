/* test.c
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

int main (int argc, const char *argv[]) {
    system("../../src/pnv_json 06301234567 hu");
    system("../../src/pnv_json 0630123456 hu");
    system("../../src/pnv_json 0630123456");
    system("../../src/pnv_json +36-70.123.4567");
    return 0;
}
