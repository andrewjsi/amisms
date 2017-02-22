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
#include "conf.h"
#include "debug.h"
#include "pnv.h"


static int phone_number_validation (const char *phone_number) {
    // új pnv objektum. A locale környezeti változókból megállapítja az
    // alapértelmezett régiót (default-locale). Ez felülírható a config által.
    pnv_t *pnv = pnv_new(phone_number);

    // Ha a configban meg van adva, akkor a default-locale alapján beállítjuk a
    // pnv alapértelmezett régióját, ami a helyi formátumú számokhoz kell.
    if (strlen(conf_root()->default_locale))
        pnv_set_locale(pnv, conf_root()->default_locale);

    // validálás
    switch (pnv_validate(pnv)) {
        case PNV_OK:
            printf("Phone number validation OK\n");
            printf("Phone number after convert: %s\n", pnv_get_phone_number_converted(pnv));
            printf("Provider: %s %s\n", pnv_get_msg_ok(pnv), pnv_get_locale(pnv));
            return 0;

        case PNV_FAIL:
            printf("Phone number validation FAIL\n");
            printf("FAILED: %s\n", pnv_get_msg_fail(pnv));
            return -1;

        case PNV_UNKNOWN:
            printf("Phone number maybe not valid: %s\n", pnv_get_msg_fail(pnv));
            if (conf_root()->pnv == CONF_PNV_FORCE) {
                printf("FAILED: %s\n", pnv_get_msg_fail(pnv));
                return -1;
            } else {
                return 0;
            }
    }

    return 0;
}

int main (int argc, char *argv[]) {
    // config file set to default
    conf_set_config_file(DEFAULT_CONFIG_FILE);

    // config file set to SMSAMI_CONFIG environment variable, if exists
    if (getenv("SMSAMI_CONFIG"))
        conf_set_config_file(getenv("SMSAMI_CONFIG"));

    if (conf_load())
        return 1;

    if (argc < 2) {
        printf("usage: phone_number_validator <phone number>\n");
        return -1;
    }

    return phone_number_validation(argv[1]);
}

