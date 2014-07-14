/* pdu.h
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

// void make_pdu(char* number, char* message, int messagelen, int alphabet, int flash_sms, int report, int udh,
              // char* udh_data, char* mode, char* pdu, int validity, int replace_msg, int system_msg, int number_type, char *smsc);

enum pdu_alphabet {
    ALPHABET_GSM = 0,
    ALPHABET_ISO,
    ALPHABET_BINARY,
    ALPHABET_UCS2,
};

typedef struct pdu_h {
    char pdu_data[1000];
    char smsc[64];
    char number[64];
    enum pdu_alphabet alphabet;
    int flash;
    int report;
} pdu_h;
