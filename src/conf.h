#include "uthash.h"

struct conf_device_t {
    char device_name[64];   // config file [section] name
    char host[64];
    char port[8];
    char username[64];
    char password[64];
    char dongle[64];
    UT_hash_handle hh;  // makes this structure hashable with Troy D. Hanson's uthash.h
};

struct conf_root_t {
    char default_device_name[64];   // default device to use
    char default_locale[8];         // default two-letter locale, used by pnv
    enum {
        CONF_PNV_ON,
        CONF_PNV_OFF,
        CONF_PNV_FORCE,
    } pnv;
};

void conf_unload ();
void conf_set_config_file (const char *file_expression);
int conf_load ();
void conf_dump ();
const struct conf_root_t *conf_root ();
const struct conf_device_t *conf_device (const char *device_name);
int conf_device_exists (const char *device_name);
