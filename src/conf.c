/* conf.c - config file handling
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#include <errno.h>
#include <sys/stat.h>
#include <wordexp.h>

#include "debug.h"
#include "misc.h"
#include "ini/ini.h"
#include "uthash.h"
#include "conf.h"


static char config_file[128];
static struct conf_root_t *conf_root_struct;

// device hash head
static struct conf_device_t *conf_device_hash = NULL;

// dummy hash item for mismatched search in conf_device()
static const struct conf_device_t conf_device_hash_dummy;

static int parse_error = 0;


void conf_dump () {
    printf("default = %s\n", conf_root_struct->default_device_name);

    printf("\n");
    struct conf_device_t *s, *tmp;
    HASH_ITER(hh, conf_device_hash, s, tmp) {
        printf("%s\n", s->device_name);
        printf("\thost     = %s\n", s->host);
        printf("\tport     = %s\n", s->port);
        printf("\tusername = %s\n", s->username);
        printf("\tpassword = %s\n", s->password);
        printf("\tdongle   = %s\n", s->dongle);
    }
}

// this function called on every line while parsing config file
static int ini_handler (void *userdata, const char *section, const char *name, const char *value) {
    #define MATCH(s) (strcmp(name, s) == 0)

    // without any sections
    if (!strcmp(section, "")) {
        #undef CPY
        #define CPY(h) strncpy(conf_root_struct->h, value, sizeof(conf_root_struct->h) - 1)
        if (MATCH("default")) {
            CPY(default_device_name);

        } else {
            printf("unknown config option \"%s\" in section \"%s\"\n", name, section);
            parse_error = 1;
            return 0; // unknown section/name error ???

        }

    // with named sections
    } else {
        #undef CPY
        #define CPY(h) strncpy(item->h, value, sizeof(item->h) - 1)
        struct conf_device_t *item;
        HASH_FIND_STR(conf_device_hash, section, item);

        // create new hash item if not exists
        if (item == NULL) {
            item = (struct conf_device_t*)malloc(sizeof(*item));
            memset(item, 0, sizeof(*item));
            scpy(item->device_name, section);
            HASH_ADD_STR(conf_device_hash, device_name, item);

        }

        if (MATCH("host")) {
            CPY(host);

        } else if (MATCH("port")) {
            CPY(port);

        } else if (MATCH("username")) {
            CPY(username);

        } else if (MATCH("password")) {
            CPY(password);

        } else if (MATCH("dongle")) {
            CPY(dongle);

        } else {
            printf("unknown config option \"%s\" in section \"%s\"\n", name, section);
            parse_error = 1;
            return 0; // unknown section/name error ???

        }

    }

    return 1;
}

void config_check () {
    struct conf_device_t *s, *tmp;
    int at_least_one_device = 0;

    HASH_ITER(hh, conf_device_hash, s, tmp) {
        at_least_one_device = 1;
        if (!strlen(s->host)) {
            strcpy(s->host, "localhost");
        }

        if (!strlen(s->port))
            strcpy(s->port, "5038");

        if (atoi(s->port) < 1 || atoi(s->port) > 65534) {
            printf("unknown port \"%s\" in device \"%s\"\n", s->port, s->device_name);
            parse_error = 1;
        }

        if (!strlen(s->username)) {
            printf("no username defined in device \"%s\"\n", s->device_name);
            parse_error = 1;
        }

        if (!strlen(s->password)) {
            printf("no password defined in device \"%s\"\n", s->device_name);
            parse_error = 1;
        }

        if (!strlen(s->dongle)) {
            printf("no dongle defined in device \"%s\"\n", s->device_name);
            parse_error = 1;
        }
    }

    if (!at_least_one_device) {
        printf("At least one device must be specified in configuration!\n");
        parse_error = 1;
    }

    // if no default device specified, then the first device to be default
    if (!strlen(conf_root_struct->default_device_name)) {
        if (conf_device_hash != NULL) {
            scpy(conf_root_struct->default_device_name, conf_device_hash->device_name);
        }
    }

    HASH_FIND_STR(conf_device_hash, conf_root_struct->default_device_name, s);
    if (s == NULL) {
        if (strlen(conf_root_struct->default_device_name)) {
            printf("Default device not found: \"%s\"\n", conf_root_struct->default_device_name);
        }
        parse_error = 1;
    }
}

void conf_set_config_file (const char *file_expression) {
    if (file_expression == NULL || strlen(file_expression) == 0)
        return;

    // parse config file name as shell expression
    wordexp_t p;
    if (wordexp(file_expression, &p, WRDE_NOCMD)) {
        printf("Config file name mismatch: \"%s\"\n", file_expression);
        parse_error = 1;
        config_file[0] = '\0'; // empty string
        return;
    }

    scpy(config_file, p.we_wordv[0]);
    wordfree(&p);
}

void conf_unload () {
    if (conf_root_struct != NULL) {
        free(conf_root_struct);
        conf_root_struct = NULL;
    }

    if (conf_device_hash != NULL) {
        struct conf_device_t *s, *tmp;
        HASH_ITER(hh, conf_device_hash, s, tmp) {
            HASH_DEL(conf_device_hash, s);
            free(s);
        }
        conf_device_hash = NULL;
    }
}

int conf_load () {
    parse_error = 0;
    conf_root_struct = malloc(sizeof(*conf_root_struct));
    memset(conf_root_struct, 0, sizeof(*conf_root_struct));

    // memset(&conf_device_hash_dummy, 0, sizeof(conf_device_hash_dummy));

    struct stat cstat;
    if (stat(config_file, &cstat)) {
        printf("Can't open configuration from %s: %s\n", config_file, strerror(errno));
        goto err;
    }

    // permissions in octal, eg: "100644"
    char perm[7];
    snprintf(perm, sizeof(perm), "%lo", (unsigned long) cstat.st_mode);
    if (perm[5] != '0') {
        printf("Config file is world-readable (mode %c%c%c)\n", perm[3], perm[4], perm[5]);
        printf("The configuration file, only the owner can have read\n");
        printf("access in order to remain safe on the AMI credentials.\n");
        printf("Please correct the permissions with this command:\n");
        printf("    chmod o= %s\n", config_file);
        goto err;
    }

    // config betöltése
    if (ini_parse(config_file, ini_handler, NULL) < 0) {
        printf("Can't load configuration from %s: %s\n", config_file, strerror(errno));
        goto err;
    }

    config_check();

    if (parse_error) {
        printf("Parse error in config file: %s\n", config_file);
        goto err;
    }

    return 0;

err:
    conf_unload();
    return -1;
}

const struct conf_root_t *conf_root () {
    return conf_root_struct;
}

const struct conf_device_t *conf_device (const char *device_name) {
    struct conf_device_t *item;
    HASH_FIND_STR(conf_device_hash, device_name, item);

    if (item == NULL)
        return &conf_device_hash_dummy;
    else
        return item; // TODO: segfault veszély, ha nem létező device_name esetén a hívó nem kezeli a NULL eseményt
}

int conf_device_exists (const char *device_name) {
    struct conf_device_t *item;
    HASH_FIND_STR(conf_device_hash, device_name, item);

    return (item == NULL) ? 0 : 1;
}

