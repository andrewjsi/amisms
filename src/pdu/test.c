#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "pdu.h"

int main () {
    // Make the PDU string from a mesage text and destination phone number.
    // The destination variable pdu has to be big enough. 
    // alphabet indicates the character set of the message.
    // flash_sms enables the flash flag.
    // mode select the pdu version (old or new).
    // if udh is true, then udh_data contains the optional user data header in hex dump, example: "05 00 03 AF 02 01"

    //     void make_pdu(
    //         char* number, char* message, int messagelen, int alphabet, int
    //         flash_sms, int report, int udh, char* udh_data, char* mode, char* pdu,
    //         int validity, int replace_msg, int system_msg, int number_type, char
    //         *smsc
    //     );


    char pdu[4096];
    
    char *phone_number = "06709405300";
    char *message = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxyyyyyyyyyyzzzzzzzzzzmmmmmmmmmm";

    make_pdu(
        phone_number,
        message,
        strlen(message),
        ALPHABET_GSM,
        0,                  // flash
        1,                  // report
        0,                  // udh
        NULL,               // udh_data
        "new",              // mode ("old" vagy "new")
        pdu,
        0,                  // validity
        0,                  // replace_msg
        0,                  // system_msg (0 vagy 1 vagy 2)
        0,                  // number_type
        NULL                // char *smsc
    );

    printf("%s\n", pdu);

    return 0;
}


