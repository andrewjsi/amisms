/* pnv.h - Phone Number Validation
 * Copyright © 2014, Andras Jeszenszky, JSS & Hayer IT - http://www.jsshayer.hu
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

enum pnv_state {
    PNV_OK = 0,
    PNV_FAIL = -1,
    PNV_UNKNOWN = -2,
};

typedef struct pnv_t {
    char tmp[128];                      // string temp, makrók használják
    char phone_number_original[128];    // paraméterben megadott telefonszám kerül ide
    char phone_number_converted[128];   // ide is, de a regexpek itt fognak vele játszani
    char default_locale[16];
    char *result_country;
    char *result_iso_code;
    char *result_msg_ok;
    char *result_msg_fail;
} pnv_t;


pnv_t *pnv_new (const char *phone_number);
void pnv_destroy (pnv_t *pnv);
enum pnv_state pnv_validate (pnv_t *pnv);
const char *pnv_state_str (enum pnv_state state);
const char *pnv_get_country (pnv_t *pnv);
const char *pnv_get_msg_ok (pnv_t *pnv);
const char *pnv_get_msg_fail (pnv_t *pnv);
const char *pnv_get_phone_number_converted (pnv_t *pnv);
const char *pnv_get_phone_number_original (pnv_t *pnv);
void pnv_set_default_locale (pnv_t *pnv, const char *locale);

