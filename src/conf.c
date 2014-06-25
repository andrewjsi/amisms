/* conf.c - config file handling
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#include <errno.h>
#include <sys/stat.h>

#include "debug.h"
#include "ini/ini.h"
#include "uthash.h"

struct conf_device_t {
    char device_name[64];   // config file [section] name
    char host[64];
    int port;
    char username[64];
    char password[64];
    char dongle[64];
    UT_hash_handle hh;  // makes this structure hashable with Troy D. Hanson's uthash.h
};

struct conf_root_t {
    char default_device_name[64];   // default device to use
};


static char config_file[128];
static struct conf_root_t *conf_root;
static struct conf_device_t *conf_device_hash = NULL;       // hash head
static int parse_error = 0;


void conf_dump () {
    printf("default = %s\n", conf_root->default_device_name);

    printf("\n");
    struct conf_device_t *s, *tmp;
    HASH_ITER(hh, conf_device_hash, s, tmp) {
        printf("%s\n", s->device_name);
        printf("\thost     = %s\n", s->host);
        printf("\tport     = %d\n", s->port);
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
        #define CPY(h) strncpy(conf_root->h, value, sizeof(conf_root->h) - 1)
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
            strncpy(item->device_name, section, sizeof(item->device_name) - 1);
            HASH_ADD_STR(conf_device_hash, device_name, item);

        }

        if (MATCH("host")) {
            CPY(host);

        } else if (MATCH("port")) {
            if (value)
                item->port = atoi(value);

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

        if (s->port == 0)
            s->port = 5038;

        if (s->port < 1 || s->port > 65534) {
            printf("unknown port \"%d\" in device \"%s\"\n", s->port, s->device_name);
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
    if (!strlen(conf_root->default_device_name)) {
        strncpy(conf_root->default_device_name, conf_device_hash->device_name, sizeof(conf_root->default_device_name) - 1);
    }

    HASH_FIND_STR(conf_device_hash, conf_root->default_device_name, s);
    if (s == NULL) {
        printf("Default device not found: \"%s\"\n", conf_root->default_device_name);
        parse_error = 1;
    }
}

void conf_set_config_file (const char *file) {
    strncpy(config_file, file, sizeof(config_file) - 1);
}

void conf_unload () {
    if (conf_root != NULL) {
        free(conf_root);
        conf_root = NULL;
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
    conf_root = malloc(sizeof(*conf_root));
    memset(conf_root, 0, sizeof(*conf_root));

    struct stat cstat;
    if (stat(config_file, &cstat)) {
        printf("Can't open configuration from %s: %s\n", config_file, strerror(errno));
        goto err;
    }

    char perm[7];
    snprintf(perm, sizeof(perm), "%lo (octal)\n", (unsigned long) cstat.st_mode);
    if (strcmp(perm, "100600")) {
        printf("Config file is word-readable (mode %s)\n", perm);
        printf("Please correct with \"chmod 600 %s\" command!\n", config_file);
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

