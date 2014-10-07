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

#include "pnv.h"

#undef fmt
#define fmt "%-11s %-14s %-15s %-20s %-10s %-10s\n"

void p (const char *locale, const char *phone_number) {
    pnv_t *pnv;
    pnv = pnv_new(phone_number);
    pnv_set_locale(pnv, locale);

    enum pnv_state state = pnv_validate(pnv);
    printf(fmt
        , pnv_state_str(state)
        , pnv_get_phone_number_original(pnv)
        , pnv_get_phone_number_converted(pnv)
        , pnv_get_country(pnv)
        , pnv_get_msg_fail(pnv)
        , pnv_get_msg_ok(pnv)
    );
    pnv_destroy(pnv);
}

int main (int argc, const char *argv[]) {
    printf(fmt, "STATE", "NUM-ORIG", "NUM-CONVERTED", "COUNTRY", "FAIL MSG", "OK MSG");
    printf("----------------------------------------------------------------------------------------\n");
    p("hu", "06201234567");
    p("hu", "06301234567");
    p("hu", "06701234567");
    p("hu", "06311234567");
    p("hu", "0670123456");
    p("hu", "067012345678");
    p("hu", "0613095200");
    p("hu", "1717");
    p("hu", "1270");
    p("hu", "0680123456");
    p("hu", "0690123456");
    p("", "06701234567");
    p("", "+3620123456");
    p("", "+36201234567");
    p("", "+06201234567");
    p("", "+9999999999");
    p("hu", "0620");
    p("hu", "062");
    p("hu", "06");
    p("", "+");
    p("hu", "+362012+4567");

    return 0;
}

