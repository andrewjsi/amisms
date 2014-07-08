// TODO: SIGALRM stílusú absolute timeout

#include <stdio.h>
#include <string.h>
#include <ev.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "ami.h"
#include "conf.h"
#include "pdu/pdu.h"
#include "debug.h"
#include "misc.h"

#ifdef DEBUG
#define CON_DEBUG
#endif
#include "logger.h"

#include "option.h"

#define DEFAULT_CONFIG_FILE "~/.smsrc"

/*****************************************************************************/

ami_t *ami;
const char *selected_device = NULL;

/*****************************************************************************/

void quit (int code) {
    exit(code);
}

static void event_donglesmsstatus (ami_event_t *event) {
    const char *status = ami_getvar(event, "Status");
    con_debug("DongleSMSStatus (ID=%s, Status=%s)", ami_getvar(event, "ID"), status);

    if (!strcmp(status, "Sent")) {
        printf("Sent OK\n");
        quit(0);
    } else {
        printf("FAILED: not sent\n");
        quit(-1);
    }
}

static void response_donglesendpdu (ami_event_t *event) {
    const char *id = ami_getvar(event, "ID");
    con_debug("Received DongleSendPDU response (ID=%s, Message=%s", id, ami_getvar(event, "Message"));

    con_debug("Registering event DongleSMSStatus with ID %s", id);
    ami_event_register(ami, event_donglesmsstatus, NULL,
        "Event: DongleSMSStatus\n"
        "ID: %s\n"
        , id
    );
    printf("Sending ...\n");
}

void convert_and_send_sms () {
    int alphabet;

    // ISO-8859-2 formátumban kell tolni neki. Wapmonsteren minden látszik, kalapos ő és ű ékezetek (max. 140 char).
    // alphabet = ALPHABET_BINARY;

    // Ékezetek helyett krix-krax (max 160 char).
    alphabet = ALPHABET_GSM;

    // ugyan olyan, mint az ALPHABET_GSM
    // alphabet = ALPHABET_ISO;

    // wide-char (max 70 char).
    // alphabet = ALPHABET_UCS2;

    int report = 0;
    int with_udh = 0;
    char *udh_data = "";

    con_debug("phone_number = \"%s\", message_text=\"%s\"", option.phone_number, option.message_text);
    char pdu[4096];

    make_pdu(
        option.phone_number, option.message_text, strlen(option.message_text), alphabet, option.flash,
        report, with_udh, udh_data, "new", pdu, 1440, 0, 0, 0, NULL);

    con_debug("Sending DongleSendPDU action with PDU \"%s\"", pdu);
    ami_action(ami, response_donglesendpdu, NULL,
        "Action: DongleSendPDU\n"
        "Device: %s\n"
        "PDU: %s\n"
        , conf_device(selected_device)->dongle
        , pdu
    );
}

void event_disconnect (ami_event_t *event) {
    printf("FAILED: %s %s(%s:%s): %s\n",
        (!strcmp(ami_getvar(event, "WasAuthenticated"), "1")) ? "Disconnected from" : "Can't connect to",
        ami_getvar(event, "Host"),
        ami_getvar(event, "IP"),
        ami_getvar(event, "Port"),
        ami_getvar(event, "Reason"));
    quit(-1);
}

void event_connect (ami_event_t *event) {
    con_debug("* Connected to %s(%s:%s)\n",
        ami_getvar(event, "Host"),
        ami_getvar(event, "IP"),
        ami_getvar(event, "Port"));

    convert_and_send_sms();
}

int main (int argc, char *argv[]) {
    // parse command-line arguments and save to option structure (in option.h)
    option_parse_args(argc, argv);

    // config file set to default
    conf_set_config_file(DEFAULT_CONFIG_FILE);

    // config file set to SMSAMI_CONFIG environment variable, if exists
    if (getenv("SMSAMI_CONFIG"))
        conf_set_config_file(getenv("SMSAMI_CONFIG"));

    // config file set to argument, if specified --config option
    if (strlen(option.config))
        conf_set_config_file(option.config);

    // 160 karakternél levágjuk a stringet. Ez majd megszűnik, ha rendben lesz
    // a PDU és a multi-part SMS
    option.message_text[160] = '\0';

    if (conf_load())
        return 1;

    if (selected_device == NULL)
        selected_device = conf_root()->default_device_name;

    if (!conf_device_exists(selected_device)) {
        printf("Device \"%s\" not found in configuration!\n", selected_device);
        return 1;
    }

    ami = ami_new(EV_DEFAULT);
    if (ami == NULL) {
        con_debug("ami_new() returned NULL");
        return 1;
    }

    ami_credentials(ami,
        conf_device(selected_device)->username,
        conf_device(selected_device)->password,
        conf_device(selected_device)->host,
        conf_device(selected_device)->port
    );

    ami_event_register(ami, event_disconnect, NULL, "Disconnect");
    ami_event_register(ami, event_connect, NULL, "Connect");
    ami_connect(ami);

    ev_loop(EV_DEFAULT, 0);

    ami_destroy(ami);
    return 0;
}

