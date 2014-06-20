// TODO: SIGALRM stílusú absolute timeout

#include <stdio.h>
#include <string.h>
#include <ev.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>     // basename()
#include <ctype.h>

#include "ami.h"
#include "pdu/pdu.h"
#include "debug.h"
#include "misc.h"
#define CON_DEBUG
#include "logger.h"

#include "banner.h"

char phone_number[128];
char text[1024];
ami_t *ami;

int option_flash_sms = 0;
char *option_config_file = "~/.smsrc";

char my_basename[64];

void quit (int code) {
    exit(code);
}

static void event_donglesmsstatus (ami_event_t *event) {
    char *status = ami_getvar(event, "Status");
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
    char *id = ami_getvar(event, "ID");
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

    int flash_sms = option_flash_sms; // FLASH
    int report = 0;
    int with_udh = 0;
    char *udh_data = "";

    con_debug("phone_number = \"%s\", text=\"%s\"", phone_number, text);
    char pdu[4096];

    make_pdu(
        phone_number, text, strlen(text), alphabet, flash_sms,
        report, with_udh, udh_data, "new", pdu, 1440, 0, 0, 0, NULL);

    con_debug("Sending DongleSendPDU action with PDU \"%s\"", pdu);
    ami_action(ami, response_donglesendpdu, NULL,
        "Action: DongleSendPDU\n"
        "Device: dongle0\n"
        "PDU: %s\n"
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

// STDIN-ről beolvasása EOF-ig. A sorokat összefűzi és a text stringbe menti,
// vigyázva a text string hosszára
void read_text_from_stdin () {
    char *line = NULL;  // ideiglenes buffer a getline-nak
    size_t len = 0;
    ssize_t read;
    int remaining_size; // text stringbe még ennyi byte fér el

    remaining_size = sizeof(text) - 1;

    while ((read = getline(&line, &len, stdin)) != -1) {
        strncat(text, line, remaining_size);
        remaining_size -= read;
        if (remaining_size <= 0)
            break;
    }
    chomp(text); // utolsó \n karakter chompolása
    free(line);
}

int main (int argc, char *argv[]) {
    strncpy(my_basename, basename(argv[0]), sizeof(my_basename) - 1);

    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "fc:")) != -1) {
        switch (c) {
            case 'f':
                option_flash_sms = 1;
                break;
            case 'c':
                option_config_file = optarg;
                break;

            case '?':
                if (optopt == 'c')
                    help("Option -c need config file name!");
                else if (isprint (optopt))
                    help("Unknown option `-%c'", optopt);
                else
                    help("Unknown option character `\\x%x'", optopt);

            default:
                help(NULL);
                break;
        }
    }

    if (argc - optind < 2) {
        help(NULL);
        quit(-1);
    }

    strncpy(phone_number, argv[optind++], sizeof(phone_number) - 1);
    strncpy(text, argv[optind++], sizeof(text) - 1);

    if (!strcmp(text, "-")) {
        printf("Text? (press control+d to send)\n");
        text[0] = '\0';
        read_text_from_stdin();
    }

    text[160] = '\0';

    ami = ami_new(EV_DEFAULT);
    if (ami == NULL) {
        con_debug("ami_new() returned NULL");
        return 1;
    }

    ami_credentials(ami, "jsi", "pwd", "localhost", "5038");
    ami_event_register(ami, event_disconnect, NULL, "Disconnect");
    ami_event_register(ami, event_connect, NULL, "Connect");
    ami_connect(ami);

    ev_loop(EV_DEFAULT, 0);

    ami_destroy(ami);
    return 0;
}

