#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

int main (int argc, char *argv[]) {
    conf_set_config_file("data/test.ini");

    if (conf_load())
        return 0;

    conf_dump();
    conf_unload();
    return 0;
}

