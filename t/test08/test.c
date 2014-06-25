#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

int main (int argc, char *argv[]) {
    conf_set_config_file("data/test.ini");

    printf("Loading config ...\n");
    if (conf_load())
        return 0;

    printf("default device name = %s\n", conf_root()->default_device_name);
    printf("default's port = %d\n", conf_device(conf_root()->default_device_name)->port);
    printf("default's username = %s\n", conf_device(conf_root()->default_device_name)->username);
    printf("port of non-existent device = %d\n", conf_device("akarmi")->port);
    printf("host of non-existent device = %s\n", conf_device("akarmi")->host);

    printf("Done\n");

    return 0;
}

