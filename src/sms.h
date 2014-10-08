/* sms.h
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#define EXIT_OK 0           // SMS rendben elküldve, nincs hiba
#define EXIT_ERR 1          // hiba, az SMS nem küldhető el
#define EXIT_AGAIN 2        // az SMS nem lett elküldve, de lehet, hogy legközelebb sikerül

