/*
SMS Server Tools 3
Copyright (C) 2006- Keijo Kasvi
http://smstools3.kekekasvi.com/

Based on SMS Server Tools 2, http://stefanfrings.de/smstools/
SMS Server Tools version 2 and below are Copyright (C) Stefan Frings.

This program is free software unless you got it under another license directly
from the author. You can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation.
Either version 2 of the License, or (at your option) any later version.
*/

#ifndef CHARSET_H
#define CHARSET_H

// Both functions return the size of the converted string
// max limits the number of characters to be written into
// destination
// size is the size of the source string
// max is the maximum size of the destination string
// The GSM character set contains 0x00 as a valid character

int gsm2iso(char* source, int size, char* destination, int max);
int decode_ucs2(char *buffer, int len);

int iso_utf8_2gsm(char* source, int size, char* destination, int max, int *missing, char **notice);
int iso2utf8_file(FILE *fp, char *ascii, int userdatalength);

int decode_7bit_packed(char *text, char *dest, size_t size_dest);
int encode_7bit_packed(char *text, char *dest, size_t size_dest);

int utf8bytes(char *s);
int utf8chars(char *s);
int utf8_to_ucs2_char(char *utf8, int *len, char *ucs2);
int utf8_to_ucs2_buffer(char *utf8, char *ucs2, size_t ucs2_size);
int ucs2_to_utf8_char(char *ucs2, char *utf8);
int ucs2_to_utf8_buffer(char *ucs2, size_t ucs2_buffer_len, char *utf8, size_t utf8_size);
size_t ucs2utf(char *buf, size_t len, size_t maxlen);
size_t utf2ucs(char *buf, size_t maxlen);
int utf8_to_iso_char(char *utf8, unsigned char *iso);

#endif
