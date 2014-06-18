// Compatibility file for pdu.c diverge

#include <linux/limits.h>

// Maxmum size of a single sms, can be 160/140 or less
#define maxsms_pdu 160
#define maxsms_ucs2 140
#define maxsms_binary 140

// Sizes for some buffers:
#define SIZE_TO 100
#define SIZE_SMSC 100
#define SIZE_UDH_DATA 500
#define SIZE_UDH_TYPE 4096
#define SIZE_TB 1024
#define MAXTEXT 39016
#define LENGTH_PDU_DETAIL_REC 70

#define isdigitc(ch) isdigit((int)(ch))

char international_prefixes[PATH_MAX +1];
char national_prefixes[PATH_MAX +1];
int validity_period;            // Validity period for messages.
int log_charconv;               // 1 if character set conversion is logged.
int incoming_utf8;              // 1 if incoming files are saved using UTF-8 character set.
int outgoing_utf8;              // 1 if outgoing files are automatically converted from UTF-8 to ISO and GSM.

// Text buffer for error messages:
char tb[SIZE_TB];

// compat.c függvényei
char *strcpyo(char *dest, const char *src);
void writelogfile (int severity, int trouble, const char *fmt, ...);
void writelogfile0 (int severity, int trouble, char *text);
char *tb_sprintf (char* format, ...);

