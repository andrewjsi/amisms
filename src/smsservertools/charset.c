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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <ctype.h>

#include "charset.h"
#include "logging.h"
#include "smsd_cfg.h"
#include "pdu.h"
#include "extras.h"
#include "charshift.h"

// For incoming character 0x24 conversion:
// Change this if other than Euro character is wanted, like '?' or '$'.
#define GSM_CURRENCY_SYMBOL_TO_ISO 0xA4

// For incoming character 0x09 conversion:
// (some reference: ftp://www.unicode.org/Public/MAPPINGS/ETSI/GSM0338.TXT)
// Uncomment this if you want that C-CEDILLA is represented as small c-cedilla:
//#define INCOMING_SMALL_C_CEDILLA

// iso = ISO8859-15 (you might change the table to any other 8-bit character set)
// sms = sms character set used by mobile phones

//                  iso   sms
char charset[] = { '@' , 0x00, // COMMERCIAL AT
		   0xA3, 0x01, // POUND SIGN
		   '$' , 0x02, // DOLLAR SIGN
		   0xA5, 0x03, // YEN SIGN
		   0xE8, 0x04, // LATIN SMALL LETTER E WITH GRAVE
		   0xE9, 0x05, // LATIN SMALL LETTER E WITH ACUTE
		   0xF9, 0x06, // LATIN SMALL LETTER U WITH GRAVE
		   0xEC, 0x07, // LATIN SMALL LETTER I WITH GRAVE
		   0xF2, 0x08, // LATIN SMALL LETTER O WITH GRAVE

#ifdef INCOMING_SMALL_C_CEDILLA
		   0xE7, 0x09, // LATIN SMALL LETTER C WITH CEDILLA
#else
		   0xC7, 0x09, // LATIN CAPITAL LETTER C WITH CEDILLA
#endif

		   0x0A, 0x0A, // LF
		   0xD8, 0x0B, // LATIN CAPITAL LETTER O WITH STROKE
		   0xF8, 0x0C, // LATIN SMALL LETTER O WITH STROKE
		   0x0D, 0x0D, // CR
		   0xC5, 0x0E, // LATIN CAPITAL LETTER A WITH RING ABOVE
		   0xE5, 0x0F, // LATIN SMALL LETTER A WITH RING ABOVE

// ISO8859-7, Capital greek characters
//		   0xC4, 0x10,
//		   0x5F, 0x11,
//		   0xD6, 0x12,
//		   0xC3, 0x13,
//		   0xCB, 0x14,
//		   0xD9, 0x15,
//		   0xD0, 0x16,
//		   0xD8, 0x17,
//		   0xD3, 0x18,
//		   0xC8, 0x19,
//		   0xCE, 0x1A,

// ISO8859-1, ISO8859-15
		   0x81, 0x10, // GREEK CAPITAL LETTER DELTA
		   0x5F, 0x11, // LOW LINE
		   0x82, 0x12, // GREEK CAPITAL LETTER PHI
		   0x83, 0x13, // GREEK CAPITAL LETTER GAMMA
		   0x84, 0x14, // GREEK CAPITAL LETTER LAMDA
		   0x85, 0x15, // GREEK CAPITAL LETTER OMEGA
		   0x86, 0x16, // GREEK CAPITAL LETTER PI
		   0x87, 0x17, // GREEK CAPITAL LETTER PSI
		   0x88, 0x18, // GREEK CAPITAL LETTER SIGMA
		   0x89, 0x19, // GREEK CAPITAL LETTER THETA
		   0x8A, 0x1A, // GREEK CAPITAL LETTER XI

		   0x1B, 0x1B, // ESC
		   0xC6, 0x1C, // LATIN CAPITAL LETTER AE
		   0xE6, 0x1D, // LATIN SMALL LETTER AE
		   0xDF, 0x1E, // LATIN SMALL LETTER SHARP S
		   0xC9, 0x1F, // LATIN CAPITAL LETTER E WITH ACUTE
		   ' ' , 0x20, // SPACE
		   '!' , 0x21, // EXCLAMATION MARK
		   0x22, 0x22, // QUOTATION MARK
		   '#' , 0x23, // NUMBER SIGN

                   // GSM character 0x24 is a "currency symbol".
                   // This character is never sent. Incoming character is converted without conversion tables.

		   '%' , 0x25, // PERSENT SIGN
		   '&' , 0x26, // AMPERSAND
		   0x27, 0x27, // APOSTROPHE
		   '(' , 0x28, // LEFT PARENTHESIS
		   ')' , 0x29, // RIGHT PARENTHESIS
		   '*' , 0x2A, // ASTERISK
		   '+' , 0x2B, // PLUS SIGN
		   ',' , 0x2C, // COMMA
		   '-' , 0x2D, // HYPHEN-MINUS
		   '.' , 0x2E, // FULL STOP
		   '/' , 0x2F, // SOLIDUS
		   '0' , 0x30, // DIGIT 0...9
		   '1' , 0x31,
		   '2' , 0x32,
		   '3' , 0x33,
		   '4' , 0x34,
		   '5' , 0x35,
		   '6' , 0x36,
		   '7' , 0x37,
		   '8' , 0x38,
		   '9' , 0x39,
		   ':' , 0x3A, // COLON
		   ';' , 0x3B, // SEMICOLON
		   '<' , 0x3C, // LESS-THAN SIGN
		   '=' , 0x3D, // EQUALS SIGN
		   '>' , 0x3E, // GREATER-THAN SIGN
		   '?' , 0x3F, // QUESTION MARK
		   0xA1, 0x40, // INVERTED EXCLAMATION MARK
		   'A' , 0x41, // LATIN CAPITAL LETTER A...Z
		   'B' , 0x42,
		   'C' , 0x43,
		   'D' , 0x44,
		   'E' , 0x45,
		   'F' , 0x46,
		   'G' , 0x47,
		   'H' , 0x48,
		   'I' , 0x49,
		   'J' , 0x4A,
		   'K' , 0x4B,
		   'L' , 0x4C,
		   'M' , 0x4D,
		   'N' , 0x4E,
		   'O' , 0x4F,
		   'P' , 0x50,
		   'Q' , 0x51,
		   'R' , 0x52,
		   'S' , 0x53,
		   'T' , 0x54,
		   'U' , 0x55,
		   'V' , 0x56,
		   'W' , 0x57,
		   'X' , 0x58,
		   'Y' , 0x59,
		   'Z' , 0x5A,
		   0xC4, 0x5B, // LATIN CAPITAL LETTER A WITH DIAERESIS
		   0xD6, 0x5C, // LATIN CAPITAL LETTER O WITH DIAERESIS
		   0xD1, 0x5D, // LATIN CAPITAL LETTER N WITH TILDE
		   0xDC, 0x5E, // LATIN CAPITAL LETTER U WITH DIAERESIS
		   0xA7, 0x5F, // SECTION SIGN
		   0xBF, 0x60, // INVERTED QUESTION MARK
		   'a' , 0x61, // LATIN SMALL LETTER A...Z
		   'b' , 0x62,
		   'c' , 0x63,
		   'd' , 0x64,
		   'e' , 0x65,
		   'f' , 0x66,
		   'g' , 0x67,
		   'h' , 0x68,
		   'i' , 0x69,
		   'j' , 0x6A,
		   'k' , 0x6B,
		   'l' , 0x6C,
		   'm' , 0x6D,
		   'n' , 0x6E,
		   'o' , 0x6F,
		   'p' , 0x70,
		   'q' , 0x71,
		   'r' , 0x72,
		   's' , 0x73,
		   't' , 0x74,
		   'u' , 0x75,
		   'v' , 0x76,
		   'w' , 0x77,
		   'x' , 0x78,
		   'y' , 0x79,
		   'z' , 0x7A,
		   0xE4, 0x7B, // LATIN SMALL LETTER A WITH DIAERESIS
		   0xF6, 0x7C, // LATIN SMALL LETTER O WITH DIAERESIS
		   0xF1, 0x7D, // LATIN SMALL LETTER N WITH TILDE
		   0xFC, 0x7E, // LATIN SMALL LETTER U WITH DIAERESIS
		   0xE0, 0x7F, // LATIN SMALL LETTER A WITH GRAVE

// Moved to the special char handling:
//		   0x60, 0x27, // GRAVE ACCENT
//                   0xE1, 0x61,  // replacement for accented a
//                   0xED, 0x69,  // replacement for accented i
//                   0xF3, 0x6F,  // replacement for accented o
//                   0xFA, 0x75,  // replacement for accented u

		   0   , 0     // End marker
		 };

// Extended characters. In GSM they are preceeded by 0x1B.

char ext_charset[] = { 0x0C, 0x0A, // <FF>
		       '^' , 0x14, // CIRCUMFLEX ACCENT
		       '{' , 0x28, // LEFT CURLY BRACKET
		       '}' , 0x29, // RIGHT CURLY BRACKET
		       '\\', 0x2F, // REVERSE SOLIDUS
		       '[' , 0x3C, // LEFT SQUARE BRACKET
		       '~' , 0x3D, // TILDE
		       ']' , 0x3E, // RIGHT SQUARE BRACKET
		       0x7C, 0x40, // VERTICAL LINE
		       0xA4, 0x65, // EURO SIGN
		       0   , 0     // End marker
	             };


// This table is used for outgoing (to GSM) conversion only:

char iso_8859_15_chars[] = {
	0x80, 0x20, // 3.1.16beta: NONBREAKABLE SPACE --> SPACE
	0x09, 0x20, // 3.1.16beta: TAB --> SPACE
	0x60, 0x27, // GRAVE ACCENT --> APOSTROPHE
	0xA0, 0x20, // NO-BREAK SPACE --> SPACE
	0xA2, 0x63, // CENT SIGN --> c
	0xA6, 0x53, // LATIN CAPITAL LETTER S WITH CARON --> S
	0xA8, 0x73, // LATIN SMALL LETTER S WITH CARON --> s
	0xA9, 0x43, // COPYRIGHT SIGN --> C
	0xAA, 0x61, // FEMININE ORDINAL INDICATOR --> a
	0xAB, 0x3C, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK --> <
	0xAC, 0x2D, // NOT SIGN --> -
	0xAD, 0x2D, // SOFT HYPHEN --> -
	0xAE, 0x52, // REGISTERED SIGN --> R
	0xAF, 0x2D, // MACRON --> -
	0xB0, 0x6F, // DEGREE SIGN --> o
	0xB1, 0x2B, // PLUS-MINUS SIGN --> +
	0xB2, 0x32, // SUPERSCRIPT TWO --> 2
	0xB3, 0x33, // SUPERSCRIPT THREE --> 3
	0xB4, 0x5A, // LATIN CAPITAL LETTER Z WITH CARON --> Z
	0xB5, 0x75, // MICRO SIGN --> u
	0xB6, 0x49, // PILCROW SIGN --> I
	0xB7, 0x2E, // MIDDLE DOT --> .
	0xB8, 0x7A, // LATIN SMALL LETTER Z WITH CARON --> z
	0xB9, 0x31, // SUPERSCRIPT ONE --> 1
	0xBA, 0x6F, // MASCULINE ORDINAL INDICATOR --> o
	0xBB, 0x3E, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK --> >
	0xBC, 0x4F, // LATIN CAPITAL LIGATURE OE --> O
	0xBD, 0x6F, // LATIN SMALL LIGATURE OE --> o
	0xBE, 0x59, // LATIN CAPITAL LETTER Y WITH DIAERESIS --> Y
	0xC0, 0x41, // LATIN CAPITAL LETTER A WITH GRAVE --> A
	0xC1, 0x41, // LATIN CAPITAL LETTER A WITH ACUTE --> A
	0xC2, 0x41, // LATIN CAPITAL LETTER A WITH CIRCUMFLEX --> A
	0xC3, 0x41, // LATIN CAPITAL LETTER A WITH TILDE --> A
	0xC7, 0x09, // LATIN CAPITAL LETTER C WITH CEDILLA --> 0x09 (LATIN CAPITAL LETTER C WITH CEDILLA)
	0xC8, 0x45, // LATIN CAPITAL LETTER E WITH GRAVE --> E
	0xCA, 0x45, // LATIN CAPITAL LETTER E WITH CIRCUMFLEX --> E
	0xCB, 0x45, // LATIN CAPITAL LETTER E WITH DIAERESIS --> E
	0xCC, 0x49, // LATIN CAPITAL LETTER I WITH GRAVE --> I
	0xCD, 0x49, // LATIN CAPITAL LETTER I WITH ACUTE --> I
	0xCE, 0x49, // LATIN CAPITAL LETTER I WITH CIRCUMFLEX --> I
	0xCF, 0x49, // LATIN CAPITAL LETTER I WITH DIAERESIS --> I
	0xD0, 0x44, // LATIN CAPITAL LETTER ETH --> D
	0xD2, 0x4F, // LATIN CAPITAL LETTER O WITH GRAVE --> O
	0xD3, 0x4F, // LATIN CAPITAL LETTER O WITH ACUTE --> O
	0xD4, 0x4F, // LATIN CAPITAL LETTER O WITH CIRCUMFLEX --> O
	0xD5, 0x4F, // LATIN CAPITAL LETTER O WITH TILDE --> O
	0xD7, 0x78, // MULTIPLICATION SIGN --> x
	0xD9, 0x55, // LATIN CAPITAL LETTER U WITH GRAVE --> U
	0xDA, 0x55, // LATIN CAPITAL LETTER U WITH ACUTE --> U
	0xDB, 0x55, // LATIN CAPITAL LETTER U WITH CIRCUMFLEX --> U
	0xDD, 0x59, // LATIN CAPITAL LETTER Y WITH ACUTE --> Y
	0xDE, 0x62, // LATIN CAPITAL LETTER THORN --> b
	0xE1, 0x61, // LATIN SMALL LETTER A WITH ACUTE --> a
	0xE2, 0x61, // LATIN SMALL LETTER A WITH CIRCUMFLEX --> a
	0xE3, 0x61, // LATIN SMALL LETTER A WITH TILDE --> a
	0xE7, 0x09, // LATIN SMALL LETTER C WITH CEDILLA --> LATIN CAPITAL LETTER C WITH CEDILLA
	0xEA, 0x65, // LATIN SMALL LETTER E WITH CIRCUMFLEX --> e
	0xEB, 0x65, // LATIN SMALL LETTER E WITH DIAERESIS --> e
	0xED, 0x69, // LATIN SMALL LETTER I WITH ACUTE --> i
	0xEE, 0x69, // LATIN SMALL LETTER I WITH CIRCUMFLEX --> i
	0xEF, 0x69, // LATIN SMALL LETTER I WITH DIAERESIS --> i
	0xF0, 0x6F, // LATIN SMALL LETTER ETH --> o
	0xF3, 0x6F, // LATIN SMALL LETTER O WITH ACUTE --> o
	0xF4, 0x6F, // LATIN SMALL LETTER O WITH CIRCUMFLEX --> o
	0xF5, 0x6F, // LATIN SMALL LETTER O WITH TILDE --> o
	0xF7, 0x2F, // DIVISION SIGN --> / (SOLIDUS)
	0xFA, 0x75, // LATIN SMALL LETTER U WITH ACUTE --> u
	0xFB, 0x75, // LATIN SMALL LETTER U WITH CIRCUMFLEX --> u
	0xFD, 0x79, // LATIN SMALL LETTER Y WITH ACUTE --> y
	0xFE, 0x62, // LATIN SMALL LETTER THORN --> b
	0xFF, 0x79, // LATIN SMALL LETTER Y WITH DIAERESIS --> y

	0   , 0
};

int special_char2gsm(char ch, char *newch)
{
  int table_row = 0;
  char *table = iso_8859_15_chars;

  if (!DEVICE.cs_convert_optical)
    return 0;

  while (table[table_row *2])
  {
    if (table[table_row *2] == ch)
    {
      if (newch)
        *newch = table[table_row *2 +1];
      return 1;
    }
    table_row++;
  }

  return 0;
}

// Return value:
// 0 = ch not found.
// 1 = ch found from normal table
// 2 = ch found from extended table
int char2gsm(char ch, char *newch)
{
  int result = 0;
  int table_row;

  // search in normal translation table
  table_row=0;
  while (charset[table_row*2])
  {
    if (charset[table_row*2] == ch)
    {
      if (newch)
        *newch = charset[table_row*2+1];
      result = 1;
      break;
    }
    table_row++;
  }

  // if not found in normal table, then search in the extended table
  if (result == 0)
  {
    table_row=0;
    while (ext_charset[table_row*2])
    {
      if (ext_charset[table_row*2] == ch)
      {
        if (newch)
          *newch = ext_charset[table_row*2+1];
        result = 2;
        break;
      }
      table_row++;
    }
  }

  return result;
}

int gsm2char(char ch, char *newch, int which_table)
{
  int table_row = 0;
  char *table;

  if (which_table == 1)
    table = charset;
  else if (which_table == 2)
    table = ext_charset;
  else
    return 0;

  while (table[table_row *2])
  {
    if (table[table_row *2 +1] == ch)
    {
      *newch = table[table_row *2];
      return 1;
    }
    table_row++;
  }

  return 0;
}

int iso_utf8_2gsm(char* source, int size, char* destination, int max, int *missing, char **notice)
{
  int source_count = 0;
  int char_count = 0;
  int dest_count = 0;
  int found;
  char newch;
  char logtmp[51];
  char tmpch;
  int bytes;
  unsigned char ch;
  char utf8bytes[32];
  char utf8char[16];
  int i;

  destination[dest_count] = 0;
  if (source == 0 || size <= 0)
    return 0;

  if (missing)
    *missing = 0;

#ifdef DEBUGMSG
  log_charconv = 1;
#endif

  if (log_charconv)
  {
    *logch_buffer = 0;
    logch("!! iso_utf8_2gsm(source=%.*s, size=%i)", size, source, size);
    logch(NULL);
  }

  // Convert each character until end of string
  while (source_count < size && dest_count < max)
  {
    *utf8bytes = 0;
    *utf8char = 0;
    ch = source[source_count];

    if (outgoing_utf8)
    {
      if ((bytes = utf8_to_iso_char(&source[source_count], &ch)) <= 0)
      {
        char *p = "NOTICE: UTF-8 to ISO conversion failed.";

        writelogfile(LOG_ERR, 0, p + 8);
        if (notice)
          strcat_realloc(notice, p, "\n");

        break;
      }
      else
      {
        strcpy(utf8bytes, "(");
        for (i = 0; i < bytes; i++)
        {
          sprintf(strchr(utf8bytes, 0), "%02X", (unsigned char)source[source_count +i]);
          sprintf(strchr(utf8char, 0), "%c", (unsigned char)source[source_count +i]);
        }
        strcat(utf8bytes, ")");

        source_count += bytes;
      }
    }
    else
      source_count++;

    char_count++;
    found = 0;

    if (DEVICE.cs_convert_optical)
    {
      found = special_char2gsm(ch, &newch);
      if (found)
      {
        destination[dest_count++] = newch;
        if (log_charconv)
        {
          sprintf(logtmp, "%s%02X[%c]~>%02X", utf8bytes, ch, prch(ch), (unsigned char)newch);
          if (gsm2char(newch, &tmpch, 1))
            sprintf(strchr(logtmp, 0), "[%c]", tmpch);
          logch("%s ", logtmp);
        }
      }
    }

    if (!found)
    {
      found = char2gsm(ch, &newch);

      if (found == 2)
      {
        if (dest_count >= max -2)
          break;
        destination[dest_count++] = 0x1B;
      }

      if (found >= 1)
      {
        destination[dest_count++] = newch;
        if (log_charconv)
        {
          sprintf(logtmp, "%s%02X[%c]", utf8bytes, ch, prch(ch));
          if (found > 1 || ch != newch)
          {
            sprintf(strchr(logtmp, 0), "->%s%02X", (found == 2)? "Esc-" : "", (unsigned char)newch);
            if (gsm2char(newch, &tmpch, found))
              sprintf(strchr(logtmp, 0), "[%c]", tmpch);
          }
          logch("%s ", logtmp);
        }
      }
    }

    if (!found)
    {
      if (missing)
        (*missing)++;

      tb_sprintf("NOTICE: Cannot convert %i. character %s%s to GSM.", char_count, utf8bytes, utf8char);
      writelogfile0(LOG_NOTICE, 0, tb + 8);
      if (notice)
        strcat_realloc(notice, tb, "\n");

#ifdef DEBUGMSG
  printf("%s\n", tb);
#endif
    }
  }

  if (log_charconv)
    logch(NULL);

  // Terminate destination string with 0, however 0x00 are also allowed within the string.
  destination[dest_count] = 0;

  return dest_count;
}

// Outputs to the file. Return value: 0 = ok, -1 = error.
int iso2utf8_file(FILE *fp, char *ascii, int userdatalength)
{
  int result = 0;
  int idx;
  unsigned int c;
  char tmp[10];
  int len;
  char logtmp[51];
  int i;
  char ucs2[2];

  if (!fp || userdatalength < 0)
    return -1;

#ifdef DEBUGMSG
  log_charconv = 1;
#endif

  if (log_charconv)
  {
    *logch_buffer = 0;
    logch("!! iso2utf8_file(..., userdatalength=%i)", userdatalength);
    logch(NULL);
  }

  for (idx = 0; idx < userdatalength; idx++)
  {
    c = (unsigned char)ascii[idx];

    if (c >= 0x81 && c <= 0x8A)
    {
      ucs2[0] = 0x03;
      switch (c)
      {
        case 0x81: ucs2[1] = 0x94; break;
        case 0x82: ucs2[1] = 0xA6; break;
        case 0x83: ucs2[1] = 0x93; break;
        case 0x84: ucs2[1] = 0x9B; break;
        case 0x85: ucs2[1] = 0xA9; break;
        case 0x86: ucs2[1] = 0xA0; break;
        case 0x87: ucs2[1] = 0xA8; break;
        case 0x88: ucs2[1] = 0xA3; break;
        case 0x89: ucs2[1] = 0x98; break;
        case 0x8A: ucs2[1] = 0x9E; break;
      }
    }
    else
    {
      // Euro character is E282AC in UTF-8, 20AC in UCS-2, but A4 in ISO-8859-15:
      if (c == 0xA4)
        c = 0x20AC;

      ucs2[0] = (unsigned char)((c & 0xFF00) >> 8);
      ucs2[1] = (unsigned char)(c & 0xFF);
    }

    len = ucs2_to_utf8_char(ucs2, tmp);

    if (len == 0)
    {
      if (log_charconv)
        logch(NULL);
      writelogfile0(LOG_NOTICE, 0,
        tb_sprintf("UTF-8 conversion error with %i. ch 0x%02X %c.", idx +1, c, (char)c));
#ifdef DEBUGMSG
  printf("%s\n", tb);
#endif
    }
    else
    {
      if (log_charconv)
      {
        sprintf(logtmp, "%02X[%c]", (unsigned char)ascii[idx], prch(ascii[idx]));
        if (len > 1 || ascii[idx] != tmp[0])
        {
          strcat(logtmp, "->");
          for (i = 0; i < len; i++)
            sprintf(strchr(logtmp, 0), "%02X", (unsigned char)tmp[i]);
        }
        logch("%s ", logtmp);
      }

      if (fwrite(tmp, 1, len, fp) != (size_t)len)
      {
        if (log_charconv)
          logch(NULL);
        writelogfile0(LOG_NOTICE, 0, tb_sprintf("Fatal file write error in UTF-8 conversion"));
#ifdef DEBUGMSG
  printf("%s\n", tb);
#endif
        result = -1;
        break;
      }
    }
  }

  if (log_charconv)
    logch(NULL);
  return result;
}

int gsm2iso(char* source, int size, char* destination, int max)
{
  int source_count=0;
  int dest_count=0;
  char newch;

  if (source==0 || size==0)
  {
    destination[0]=0;
    return 0;
  }

  // Convert each character untl end of string
  while (source_count<size && dest_count<max)
  {
    if (source[source_count]!=0x1B)
    {  
      // search in normal translation table
      if (gsm2char(source[source_count], &newch, 1))
        destination[dest_count++] = newch;
      else if (source[source_count] == 0x24)
        destination[dest_count++] = (char)GSM_CURRENCY_SYMBOL_TO_ISO;
      else
      {
        writelogfile0(LOG_NOTICE, 0,
          tb_sprintf("Cannot convert GSM character 0x%02X to ISO, you might need to update the 1st translation table.",
          source[source_count]));
#ifdef DEBUGMSG
  printf("%s\n", tb);
#endif
      }
    }
    else if (++source_count<size)
    {
      // search in extended translation table
      if (gsm2char(source[source_count], &newch, 2))
        destination[dest_count++] = newch;
      else
      {
        writelogfile0(LOG_NOTICE, 0,
          tb_sprintf("Cannot convert extended GSM character 0x1B 0x%02X, you might need to update the 2nd translation table.",
          source[source_count]));
#ifdef DEBUGMSG
  printf("%s\n", tb);
#endif
      }
    }
    source_count++;
  }
  // Terminate destination string with 0, however 0x00 are also allowed within the string.
  destination[dest_count]=0;
  return dest_count;
}

int decode_ucs2(char *buffer, int len)
{
  int i;
  char *d = buffer;
  unsigned char *s = (unsigned char *)buffer;

  for (i = 0; i < len; i += 2)
  {
    if (!s[i])
      *(d++) = s[i +1];
    else if (s[i] == 0x20 && s[i +1] == 0xAC)
      *(d++) = 0xA4;
    else if (s[i] == 0x01 && s[i +1] == 0x60)
      *(d++) = 0xA6;
    else if (s[i] == 0x01 && s[i +1] == 0x61)
      *(d++) = 0xA8;
    else if (s[i] == 0x01 && s[i +1] == 0x7D)
      *(d++) = 0xB4;
    else if (s[i] == 0x01 && s[i +1] == 0x7E)
      *(d++) = 0xB8;
    else if (s[i] == 0x01 && s[i +1] == 0x52)
      *(d++) = 0xBC;
    else if (s[i] == 0x01 && s[i +1] == 0x53)
      *(d++) = 0xBD;
    else if (s[i] == 0x01 && s[i +1] == 0x78)
      *(d++) = 0xBE;
    else
      writelogfile(LOG_NOTICE, 0,
        "Cannot convert UCS2 character 0x%02X 0x%02X to ISO-8859-15. Consider using incoming_utf8 = yes.",
        s[i], s[i +1]);
  }

  *d = '\0';
  i = d - buffer;

  return i;
}

int iso2utf8(
	//
	// Converts to the buffer. Returns -1 in case of error, >= 0 = length of dest.
	//
	char *ascii,
	int userdatalength,
	size_t ascii_size
)
{
	int result = 0;
	int idx;
	unsigned int c;
	char tmp[10];
	int len;
	char logtmp[51];
	int i;
	char *buffer;
	char ucs2[2];

	if (userdatalength < 0)
		return -1;

	if (!(buffer = (char *) malloc(ascii_size)))
		return -1;

#ifdef DEBUGMSG
	log_charconv = 1;
#endif

	if (log_charconv)
	{
		*logch_buffer = 0;
		logch("!! iso2utf8(..., userdatalength=%i)", userdatalength);
		logch(NULL);
	}

	for (idx = 0; idx < userdatalength; idx++)
	{
		c = ascii[idx] & 0xFF;
		// Euro character is E282AC in UTF-8, 20AC in UCS-2, but A4 in ISO-8859-15:
		if (c == 0xA4)
			c = 0x20AC;

		ucs2[0] = (unsigned char)((c & 0xFF00) >> 8);
		ucs2[1] = (unsigned char)(c & 0xFF);
		len = ucs2_to_utf8_char(ucs2, tmp);

		if (len == 0)
		{
			if (log_charconv)
				logch(NULL);
			writelogfile0(LOG_NOTICE, 0, tb_sprintf("UTF-8 conversion error with %i. ch 0x%02X %c.", idx + 1, c, (char) c));
#ifdef DEBUGMSG
			printf("%s\n", tb);
#endif
		}
		else
		{
			if (log_charconv)
			{
				sprintf(logtmp, "%02X[%c]", (unsigned char) ascii[idx], prch(ascii[idx]));
				if (len > 1 || ascii[idx] != tmp[0])
				{
					strcat(logtmp, "->");
					for (i = 0; i < len; i++)
						sprintf(strchr(logtmp, 0), "%02X", (unsigned char) tmp[i]);
				}
				logch("%s ", logtmp);
			}

			if ((size_t) (result + len) < ascii_size - 1)
			{
				strncpy(buffer + result, tmp, len);
				result += len;
			}
			else
			{
				if (log_charconv)
					logch(NULL);
				writelogfile0(LOG_NOTICE, 0, tb_sprintf("Fatal error (buffer too small) in UTF-8 conversion"));
#ifdef DEBUGMSG
				printf("%s\n", tb);
#endif
				result = -1;
				break;
			}
		}
	}

	if (log_charconv)
		logch(NULL);

	if (result >= 0)
	{
		memcpy(ascii, buffer, result);
		ascii[result] = 0;
	}

	free(buffer);

	return result;
}

int encode_7bit_packed(
	//
	// Encodes a string to GSM 7bit (USSD) packed format.
	// Returns number of septets.
	// Handles padding as defined on GSM 03.38 version 5.6.1 (ETS 300 900) page 17.
	//
	char *text,
	char *dest,
	size_t size_dest
)
{
	int len;
	int i;
	char buffer[512];
	char buffer2[512];
	char padding = '\r';
        int save_outgoing_utf8 = outgoing_utf8;

        outgoing_utf8 = 1;
	len = iso_utf8_2gsm(text, strlen(text), buffer2, sizeof(buffer2), 0, 0);
        outgoing_utf8 = save_outgoing_utf8;

#ifdef DEBUGMSG
	printf("characters: %i\n", len);
	printf("characters %% 8: %i\n", len % 8);
#endif

	if ((len % 8 == 7) || (len % 8 == 0 && len && buffer2[len - 1] == padding))
	{
		if ((size_t) len < sizeof(buffer2) - 1)
		{
			buffer2[len++] = padding;
#ifdef DEBUGMSG
			printf("adding padding, characters: %i\n", len);
#endif
		}
	}

	i = text2pdu(buffer2, len, buffer, 0);
	snprintf(dest, size_dest, "%s", buffer);

#ifdef DEBUGMSG
	printf("octets: %i\n", strlen(buffer) / 2);
	for (len = 0; buffer[len]; len += 2)
		printf("%.2s ", buffer + len);
	printf("\n");
#endif
	return i;
}

int decode_7bit_packed(
	//
	// Decodes GSM 7bit (USSD) packed string.
	// Returns length of dest. -1 in the case or error and "ERROR" in dest.
	// Handles padding as defined on GSM 03.38 version 5.6.1 (ETS 300 900) page 17.
	//
	char *text,
	char *dest,
	size_t size_dest
)
{
	int len;
	int i;
	char buffer[512];
	char buffer2[512];
	char *p;
	int septets;
	int padding = '\r';

	snprintf(buffer, sizeof(buffer), "%s", text);
	while ((p = strchr(buffer, ' ')))
		strcpyo(p, p + 1);
	for (i = 0; buffer[i]; i++)
		buffer[i] = toupper((int) buffer[i]);

	i = strlen(buffer);
	if (i % 2)
	{
		snprintf(dest, size_dest, "ERROR");
		return -1;
	}

	septets = i / 2 * 8 / 7;
	snprintf(buffer2, sizeof(buffer2), "%02X%s", septets, buffer);

#ifdef DEBUGMSG
	printf("septets: %i (0x%02X)\n", septets, septets);
	printf("septets %% 8: %i\n", septets % 8);
	printf("%s\n", buffer2);
#endif
	memset(buffer, 0, sizeof(buffer));
	pdu2text(buffer2, buffer, &len, 0, 0, 0, 0, 0);

	if ((septets % 8 == 0 && len && buffer[len - 1] == padding) || (septets % 8 == 1 && len > 1 && buffer[len - 1] == padding && buffer[len - 2] == padding))
	{
		len--;
#ifdef DEBUGMSG
		printf("removing padding, characters: %i\n", len);
#endif
	}

	i = gsm2iso(buffer, len, buffer2, sizeof(buffer2));
	if (incoming_utf8)
		i = iso2utf8(buffer2, i, sizeof(buffer2));
	snprintf(dest, size_dest, "%s", buffer2);

	return i;
}

int utf8bytes(char *s)
{
  int result = 1;
  int i;
  unsigned char ch = s[0];

  if (ch >= 0x80)
  {
    while (((ch <<= 1) & 0x80) != 0)
      result++;

    if (result == 1 || result > 6)
      return -1;

    for (i = 1; i < result; i++)
      if ((s[i] & 0xC0) != 0x80)
        return -1;
  }

  return result;
}

int utf8chars(char *s)
{
  int result = 0;
  char *p = s;
  int i;

  while (*p)
  {
    if ((i = utf8bytes(p)) <= 0)
      return -1;

    result++;
    p += i;
  }

  return result;
}

int utf8_to_ucs2_char(char *utf8, int *len, char *ucs2)
{
  unsigned int c = 0;
  int i;

  i = utf8bytes(utf8);
  if (len)
    *len = i;

  switch (i)
  {
    case 1:
      c = utf8[0];
      break;

    case 2:
      c = (utf8[0] & 0x1F) << 6 | (utf8[1] & 0x3F);
      break;

    case 3:
      c = (utf8[0] & 0x0F) << 12 | (utf8[1] & 0x3F) << 6 | (utf8[2] & 0x3F);
      break;

    default:
      return 0;
  }

  ucs2[0] = (unsigned char)((c & 0xFF00) >> 8);
  ucs2[1] = (unsigned char)(c & 0xFF);

  return 1;
}

// Note: Returns the number of UCS2 characters, not bytes.
int utf8_to_ucs2_buffer(char *utf8, char *ucs2, size_t ucs2_size)
{
  char *p = utf8;
  char *end = utf8 +strlen(utf8);
  int bytes;
  size_t dest = 0;
  int result = 0;

  while (p < end)
  {
    if (dest >= ucs2_size -1)
      break;

    if (!utf8_to_ucs2_char(p, &bytes, &ucs2[dest]))
      break;

    p += bytes;
    dest += 2;
    result++;
  }

  return result;
}

// Returns the number of utf8 bytes.
int ucs2_to_utf8_char(char *ucs2, char *utf8)
{
  int result;
  unsigned int c = (ucs2[0] << 8) | (unsigned char)ucs2[1];

  if (c <= 0x7F)
  {
    utf8[0] = (unsigned char)c;
    result = 1;
  }
  else if (c <= 0x7FF)
  {
    utf8[1] = (unsigned char)(0x80 | (c & 0x3F));
    c = (c >> 6);
    utf8[0] = (unsigned char)(0xC0 | c);
    result = 2;
  }
  else if (c <= 0xFFFF)
  {
    utf8[2] = (unsigned char)(0x80 | (c & 0x3F));
    c = (c >> 6);
    utf8[1] = (unsigned char)(0x80 | (c & 0x3F));
    c = (c >> 6);
    utf8[0] = (unsigned char)(0xE0 | c);
    result = 3;
  }
  else
    result = 0;

  utf8[result] = '\0';
  return result;
}

// Returns number of utf8 characters, not bytes.
int ucs2_to_utf8_buffer(char *ucs2, size_t ucs2_buffer_len, char *utf8, size_t utf8_size)
{
  int result = 0;
  char *p = ucs2;
  char *end = ucs2 + ucs2_buffer_len;
  char utf8char[7];
  size_t len = 0;
  int i;

  while (p < end)
  {
    if (!(i = ucs2_to_utf8_char(p, utf8char)))
      break;

    if (len + i >= utf8_size)
      break;

    strcpy(&utf8[len], utf8char);
    len += i;
    p += 2;
    result++;
  }

  return result;
}

// Returns number of bytes.
size_t ucs2utf(char *buf, size_t len, size_t maxlen)
{
  char *ucs2 = (char *)malloc(len);

  if (!ucs2)
    return 0;

  memcpy(ucs2, buf, len);
  ucs2_to_utf8_buffer(ucs2, len, buf, maxlen +1);

  free(ucs2);

  return strlen(buf);
}

// Returns number of bytes.
size_t utf2ucs(char *buf, size_t maxlen)
{
  size_t ucs2_size = utf8chars(buf) * 2; // Not NULL terminated.
  char *ucs2;
  size_t bytes;

  if (ucs2_size > maxlen + 1)
    ucs2_size = maxlen + 1;

  if (!(ucs2 = (char *)malloc(ucs2_size)))
    return 0;

  bytes = 2 * utf8_to_ucs2_buffer(buf, ucs2, ucs2_size);
  memcpy(buf, ucs2, bytes);

  free(ucs2);

  return bytes;
}

// Returns number of utf8 bytes. -1 in case of error.
int utf8_to_iso_char(char *utf8, unsigned char *iso)
{
  unsigned char *s = (unsigned char *)utf8;

  *iso = '\0';

  if (*s < 128)
  {
    *iso = *s;
    return 1;
  }

  if (s[0] == 0xCE)
  {
    // ΔΦΓΛΩΠΨΣΘΞ --> 0x81 ... 0x8A
    switch (s[1])
    {
      case 0x94: *iso = 0x81; return 2;
      case 0xA6: *iso = 0x82; return 2;
      case 0x93: *iso = 0x83; return 2;
      case 0x9B: *iso = 0x84; return 2;
      case 0xA9: *iso = 0x85; return 2;
      case 0xA0: *iso = 0x86; return 2;
      case 0xA8: *iso = 0x87; return 2;
      case 0xA3: *iso = 0x88; return 2;
      case 0x98: *iso = 0x89; return 2;
      case 0x9E: *iso = 0x8A; return 2;
    }
  }

  if (s[0] == 226 && s[1] == 130 && s[2] == 172)
  {
    *iso = 164;
    return 3;
  }

  if (s[0] == 194 && s[1] >= 128 && s[1] <= 191)
  {
    *iso = s[1];
    return 2;
  }

  if (s[0] == 195 && s[1] >= 128 && s[1] <= 191)
  {
    *iso = s[1] + 64;
    return 2;
  }

  if (s[0] == 197)
  {
    switch (s[1])
    {
      case 160: *iso = 166; return 2;
      case 161: *iso = 168; return 2;
      case 189: *iso = 180; return 2;
      case 190: *iso = 184; return 2;
      case 146: *iso = 188; return 2;
      case 147: *iso = 189; return 2;
      case 184: *iso = 190; return 2;
    }
  }

  if (s[0] >= 192 && s[0] < 224 &&
      s[1] >= 128 && s[1] < 192)
    return 2;

  if (s[0] >= 224 && s[0] < 240 &&
      s[1] >= 128 && s[1] < 192 &&
      s[2] >= 128 && s[2] < 192)
    return 3;

  if (s[0] >= 240 && s[0] < 248 &&
      s[1] >= 128 && s[1] < 192 &&
      s[2] >= 128 && s[2] < 192 &&
      s[3] >= 128 && s[3] < 192)
    return 4;

  if (s[0] >= 248 && s[0] < 252 &&
      s[1] >= 128 && s[1] < 192 &&
      s[2] >= 128 && s[2] < 192 &&
      s[3] >= 128 && s[3] < 192 &&
      s[4] >= 128 && s[4] < 192)
    return 5;

  if (s[0] >= 252 && s[0] < 254 &&
      s[1] >= 128 && s[1] < 192 &&
      s[2] >= 128 && s[2] < 192 &&
      s[3] >= 128 && s[3] < 192 &&
      s[4] >= 128 && s[4] < 192 &&
      s[5] >= 128 && s[5] < 192)
    return 6;

  return -1;
}
