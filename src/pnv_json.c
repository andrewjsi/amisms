#include <stdio.h>
#include <string.h>
#include <ev.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>

#include "defaults.h"
#include "debug.h"
#include "pnv.h"

static int phone_number_validation (const char *phone_number, const char *locale) {
    // új pnv objektum. A locale környezeti változókból megállapítja az
    // alapértelmezett régiót (default-locale). Ez felülírható a config által.
    pnv_t *pnv = pnv_new(phone_number);

    if (locale)
        pnv_set_locale(pnv, locale);

    // validálás
    int state = pnv_validate(pnv);

    printf("{"
        "\"state\": \"%s\", "
        "\"provider\": \"%s\", "
        "\"invalidMsg\": \"%s\", "
        "\"revisedNumber\": \"%s\"}\n"
        , pnv_state_str(state)
        , (state) ? "" : pnv_get_msg_ok(pnv)
        , (state) ? pnv_get_msg_fail(pnv) : ""
        , (state) ? "" : pnv_get_phone_number_converted(pnv)
    );

    return 0;
}

int main (int argc, char *argv[]) {
    char *phone_number = NULL;
    char *locale = NULL;

    if (argc < 2) {
        printf("JSS AMISMS Phone Number Validator utility\n");
        printf("usage: pnv_json <phone number> [locale]\n");
        return -1;
    }

    phone_number = argv[1];

    // ha van második param
    if (argc > 2)
        locale = argv[2];

    return phone_number_validation(phone_number, locale);
}

