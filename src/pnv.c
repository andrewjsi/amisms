/* pnv.c - Phone Number Validation
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <locale.h>

#include "debug.h"
#include "misc.h"
#include "pnv.h"

int pnv_hu_local (pnv_t *pnv);
int pnv_hu_international (pnv_t *pnv);

// TODO: http://countrycode.org/ (a magyar kódok nem teljesen jók)

// A táblázatban a több jegyű országkódoknak feljebb kell lenniük, mint a
// kevesebb jegyű országkódoknak. Azért, mert a kereső függvény föntről lefele
// halad és az első illeszkedésnél befejezi a vizsgálatot.
struct pnv_table {
    char *iso;
    char *code;
    char *country;
    int (*local)(pnv_t*);
    int (*international)(pnv_t*);
} pnv_table[] = {
    {   .iso            = "HU",
        .code           = "36",
        .country        = "Hungary",
        .local          = pnv_hu_local,
        .international  = pnv_hu_international,
    },
    {   .iso            = "DE",
        .code           = "49",
        .country        = "Germany",
        .local          = NULL,
        .international  = NULL,
    },
};

int foreach_pnv_table_i; // FOREACH_PNV_TABLE belső használatára
                        // BUG: nem teljesen korrekt a globális változó volta
                        // miatt. Ha egymásba ágyazott FOREACH_PNV_TABLE ciklusokat
                        // használunk, akkor a két iteráció ugyan azt a
                        // foreach_pnv_table_i változót fogja használni, ami anomáliához
                        // vezet.

// FOREACH_PNV_TABLE (s)
// Végigmegy az pnv_table struktúra tömbön, és a soron következő struktúra címét
// átadja a paraméterül kapott "s" mutatónak. Segfault veszély van, ha a
// cikluson kívül megpróbáljuk használni az s mutató területét, ugyanis az egy
// nemlétező tömbelemre fog mutatni.
#define FOREACH_PNV_TABLE(s)                                                   \
    for (foreach_pnv_table_i = 0, s = &pnv_table[0];                           \
         foreach_pnv_table_i < (sizeof(pnv_table) / sizeof(struct pnv_table)); \
         foreach_pnv_table_i++, s = &pnv_table[foreach_pnv_table_i])

#define ZONES_NUM (sizeof(zones) / sizeof(struct zone))

// pnv->phone_number_converted stringbe ír. Átlapolás lehetséges, mert az pnv->tmp
// ideiglenes string-et használja
#define PNV_PRINTF(...)                                                        \
    do {                                                                       \
        snprintf(pnv->tmp, sizeof(pnv->tmp), __VA_ARGS__);                     \
        scpy(pnv->phone_number_converted, pnv->tmp);                           \
    } while (0)

#define PNV_RETURN_FAIL(s)                                                     \
    do {                                                                       \
        if ((s) && strlen(s))                                                  \
            pnv->result_msg_fail = (s);                                        \
        return PNV_FAIL;                                                       \
    } while (0)

#define PNV_RETURN_UNKNOWN(s)                                                  \
    do {                                                                       \
        if ((s) && strlen(s))                                                  \
            pnv->result_msg_fail = (s);                                        \
        return PNV_UNKNOWN;                                                    \
    } while (0)

#define PNV_RETURN_OK(s)                                                       \
    do {                                                                       \
        if ((s) && strlen(s))                                                  \
            pnv->result_msg_ok = (s);                                          \
        return PNV_OK;                                                         \
    } while (0)

#define PNV_LEN (strlen(pnv->phone_number_converted))

#define PNV_DELCHARS(s) strdelchars(pnv->phone_number_converted, (s))

#define PNV_IF_REGEX(r) if (! _pnv_regex_func((r), pnv->phone_number_converted))
static int _pnv_regex_func (const char *regular_expression, const char *pattern) {
    regex_t regex;
    int regret;

    if (regcomp(&regex, regular_expression, REG_EXTENDED)) {
        fprintf(stderr, "Could not compile regex: \"%s\"\n", regular_expression);
        return -1;
    }

    regret = regexec(&regex, pattern, 0, NULL, 0);
    if (regret && regret != REG_NOMATCH) {
        char error_message[128];
        regerror(regret, &regex, error_message, sizeof(error_message));
        fprintf(stderr, "Regex match failed: %s\n", error_message);
    }

    regfree(&regex);
    return regret;
}

const char *pnv_state_str (enum pnv_state state) {
    switch (state) {
        case PNV_OK:         return "PNV_OK";
        case PNV_FAIL:       return "PNV_FAIL";
        case PNV_UNKNOWN:    return "PNV_UNKNOWN";
        default:            return "???";
    }
}

int pnv_hu_local (pnv_t *pnv) {
    // szóköz, kötőjel, vessző és pont karakterek kivágása
    PNV_DELCHARS(" -,.");

    // 70/940-5300 formátum
    PNV_IF_REGEX("^../") {
        PNV_PRINTF("+36%s", pnv->phone_number_converted);
    }

    // / karakterek kivágása
    PNV_DELCHARS("/");

    // 06 helyett +36
    PNV_IF_REGEX("^06") {
        PNV_PRINTF("+36%s", &pnv->phone_number_converted[2]);
    }

    // 36 helyett +36
    PNV_IF_REGEX("^36") {
        PNV_PRINTF("+%s", &pnv->phone_number_converted[0]);
    }

    // 20....... és 70....... helyett +3620 és +3670
    PNV_IF_REGEX("^[27]0.......$") {
        PNV_PRINTF("+36%s", &pnv->phone_number_converted[0]);
    }

    // 30....... és 31....... helyett +3630 és +3631
    PNV_IF_REGEX("^3[01].......$") {
        PNV_PRINTF("+36%s", &pnv->phone_number_converted[0]);
    }

    // 00 helyett +
    PNV_IF_REGEX("^00") {
        PNV_PRINTF("+%s", &pnv->phone_number_converted[2]);
    }

    PNV_IF_REGEX("^12..$")       PNV_RETURN_OK("SMS szolgálatás");
    PNV_IF_REGEX("^17..$")       PNV_RETURN_FAIL("emeltdíjas");

    // hibás szám formátum
    if (pnv->phone_number_converted[0] != '+') {
        PNV_RETURN_FAIL("hibás szám vagy körzet");
    }

    PNV_RETURN_OK("");
}

int pnv_hu_international (pnv_t *pnv) {
    // legalább 6 jegyűnek (a + karaktert is beleértve) kell lennie a
    // teleofnszámnak
    if (PNV_LEN < 6)
        PNV_RETURN_FAIL("kevés számjegy");

    PNV_IF_REGEX("^\\+3640")  PNV_RETURN_FAIL("kék szám");
    PNV_IF_REGEX("^\\+3651")  PNV_RETURN_FAIL("internetszolgáltató");
    PNV_IF_REGEX("^\\+3660")  PNV_RETURN_FAIL("szolgáltató megszűnt");
    PNV_IF_REGEX("^\\+3680")  PNV_RETURN_FAIL("zöld szám");
    PNV_IF_REGEX("^\\+3681")  PNV_RETURN_FAIL("emeltdíjas");
    PNV_IF_REGEX("^\\+3690")  PNV_RETURN_FAIL("emeltdíjas");
    PNV_IF_REGEX("^\\+3691")  PNV_RETURN_FAIL("emeltdíjas");

    PNV_IF_REGEX("^\\+3620") {
        if (PNV_LEN == 12)
            PNV_RETURN_OK("Telenor");
        else
            PNV_RETURN_FAIL("kevés vagy sok számjegy");
    }

    PNV_IF_REGEX("^\\+3630") {
        if (PNV_LEN == 12)
            PNV_RETURN_OK("T-Mobile");
        else
            PNV_RETURN_FAIL("kevés vagy sok számjegy");
    }

    PNV_IF_REGEX("^\\+3631") {
        if (PNV_LEN == 12)
            PNV_RETURN_OK("Tesco mobile");
        else
            PNV_RETURN_FAIL("kevés vagy sok számjegy");
    }

    PNV_IF_REGEX("^\\+3670") {
        if (PNV_LEN == 12)
            PNV_RETURN_OK("Vodafone");
        else
            PNV_RETURN_FAIL("kevés vagy sok számjegy");
    }

    PNV_RETURN_FAIL("nem mobil körzet");
}

#define PNV_DEBUG(...)
// #define PNV_DEBUG(...) printf(__VA_ARGS__)
enum pnv_state pnv_validate (pnv_t *pnv) {
    struct pnv_table *row;
    int ret = 0;

    trim(pnv->phone_number_converted);

    // ha nincs + a szám elején, akkor helyi (local) számozást kell vizsgálnunk
    if (pnv->phone_number_converted[0] != '+') {

        // ha nincs beállítva a helyi locale, akkor nem tudjuk ellenőrizni a
        // helyi számozást, és konvertálni sem tudjuk nemzetközi formátumra
        if (!strlen(pnv->locale)) {
            PNV_RETURN_UNKNOWN("helyi számot nem lehet vizsgálni ismeretlen régióban");
        }

        // megkeressük a helyi régióhoz tartozó local() függvényt
        FOREACH_PNV_TABLE (row) {
            if (row->iso && !strcmp(pnv->locale, row->iso)) {
                pnv->result_country = row->country;
                PNV_DEBUG("Found country: %s\n", row->country);

                // ha be van állítva a local() függvény
                if (row->local) {
                    ret = row->local(pnv); // callback hívás
                    PNV_DEBUG("local() returned %s\n", ret_str(ret));
                }
                break;
            }
        }
    }

    if (ret) {
        PNV_DEBUG("result_msg_fail: %s\n", pnv->result_msg_fail);
        return ret;
    }

    // ha továbbra sincs nemzetközi formátumunk, akkor az azért lehet, mert a
    // local() függvény egy olyan szolgáltatás számát engedélyezte, ami csak
    // belföldről elérhető (pl. 12xx SMS számok)
    if (pnv->phone_number_converted[0] != '+') {
        if (!pnv->result_msg_ok || !strlen(pnv->result_msg_ok)) {
            PNV_RETURN_FAIL("belső hiba, a local() függvény OK-ot adott vissza, de nem írt a result_msg_ok -ba, és nem is konvertálta át a telefonszámot nemzetközi (+) formátumba!");
        }
        return ret;
    }

    // ezen a ponton a local() függvény jóváhagyta és átkonvertálta a
    // telefonszámot "+" -al kezdődő nemzetközi formátumúvá.

    if (PNV_LEN < 4)
        PNV_RETURN_FAIL("kevés számjegy");

    // ha az első + karakteren kívül van másik + karakter is benne
    if (index(&pnv->phone_number_converted[1], '+'))
        PNV_RETURN_FAIL("túl sok + karakter");

    // 0-val nem kezdődik országkód. Ez kiszűri az esetleges félreütéseket,
    // mint pl. "000" a magyar formátumban.
    PNV_IF_REGEX("^\\+0") {
        PNV_RETURN_FAIL("rossz nemzetközi formátum");
    }

    ret = PNV_UNKNOWN;

    FOREACH_PNV_TABLE (row) {
        if (row->code && !strncmp(&pnv->phone_number_converted[1], row->code, strlen(row->code))) {
            pnv->result_country = row->country;
            PNV_DEBUG("Found country: %s\n", row->country);

            // ha be van állítva a local() függvény
            if (row->international) {
                ret = row->international(pnv); // callback hívás
                PNV_DEBUG("international() returned %s\n", ret_str(ret));
            }
            break;
        }
    }

    if (ret == PNV_UNKNOWN) {
        PNV_RETURN_UNKNOWN("országkód nem található");
    }

    return ret;
}

static void determine_locale (pnv_t *pnv) {
    char *locale;
    locale = setlocale(LC_TELEPHONE, "");

    if (!locale || !strcmp(locale, "C") || !strcmp(locale, "POSIX"))
        return;

    pnv_set_locale(pnv, locale);
}

pnv_t *pnv_new (const char *phone_number_converted) {
    pnv_t *pnv = malloc(sizeof(*pnv));
    if (pnv == NULL) {
        fprintf(stderr, "pnv_new(): out of memory");
        return NULL;
    }
    memset(pnv, 0, sizeof(*pnv));

    scpy(pnv->phone_number_converted, phone_number_converted);
    scpy(pnv->phone_number_original, phone_number_converted);
    determine_locale(pnv);

    return pnv;
}

void pnv_destroy (pnv_t *pnv) {
    if (pnv)
        free(pnv);

    return;
}

const char *pnv_get_country (pnv_t *pnv) {
    if (!pnv)
        return NULL;
    return pnv->result_country;
}

const char *pnv_get_msg_ok (pnv_t *pnv) {
    if (!pnv)
        return NULL;
    return pnv->result_msg_ok;
}

const char *pnv_get_msg_fail (pnv_t *pnv) {
    if (!pnv)
        return NULL;
    return pnv->result_msg_fail;
}

const char *pnv_get_phone_number_converted (pnv_t *pnv) {
    if (!pnv)
        return NULL;
    return pnv->phone_number_converted;
}

const char *pnv_get_phone_number_original (pnv_t *pnv) {
    if (!pnv)
        return NULL;
    return pnv->phone_number_original;
}

void pnv_set_locale (pnv_t *pnv, const char *locale) {
    if (!pnv)
        return;

    if (!locale || !strlen(locale)) {
        pnv->locale[0] = '\0';
        return;
    }

    if (strlen(locale) >= 2) {
        pnv->locale[0] = toupper(locale[0]);
        pnv->locale[1] = toupper(locale[1]);
        pnv->locale[2] = '\0';
    }
}

const char *pnv_get_locale (pnv_t *pnv) {
    if (!pnv)
        return NULL;
    return pnv->locale;
}

