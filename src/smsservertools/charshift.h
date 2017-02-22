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

#ifndef DISABLE_NATIONAL_LANGUAGE_SHIFT_TABLES
#ifndef CHARSHIFT_H
#define CHARSHIFT_H

char *get_language_name
(
  int value
);

int parse_language_setting(
  char *value
);

int select_language_shift_tables
(
  int *language,
  int *language_ext,
  int DEVICE_language,
  int DEVICE_language_ext
);

int utf2gsm_shift
(
  char *text,
  size_t text_size,
  int *textlen,
  int *language,
  int *language_ext,
  char **notice
);

int get_language_shift
(
  char *udh,
  int *language,
  int *language_ext
);

int gsm2utf8_shift
(
  char *buffer,
  size_t buffer_size,
  int userdatalength,
  int language,
  int language_ext
);

void print_language_tables
(
void
);

#endif
#endif
