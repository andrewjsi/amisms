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

#ifndef WHITE_H
#define WHITE_H

char whitelist[256];	// Filename of the white-list

/* Used with >= 3.1x */
int inwhitelist_q(char* msisdn, char *queuename);

#endif
