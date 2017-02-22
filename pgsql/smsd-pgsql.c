// TODO: SIGALRM stílusú absolute timeout

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <libpq-fe.h>

#include "pgsql.h"

pg_t *pg1;

void create_database () {
    if (pg_query(pg1, "\
DROP TABLE IF EXISTS sms; \
CREATE TABLE IF NOT EXISTS sms ( \
    id              SERIAL PRIMARY KEY, \
    phone_number    CHARACTER VARYING(32) NOT NULL, \
    text            CHARACTER VARYING(1080) NOT NULL, \
    sent            BOOLEAN DEFAULT FALSE, \
    delivered       BOOLEAN DEFAULT FALSE, \
    error           BOOLEAN DEFAULT FALSE, \
    error_message   CHARACTER VARYING(128) DEFAULT NULL \
);")) {
    fprintf(stderr, "pg_query(): %s\n", pg_error(pg1));
}

    pg_query(pg1, "INSERT INTO sms (phone_number, text) VALUES ('06-20-772-0300', 'proba teszt');");
    pg_query(pg1, "INSERT INTO sms (phone_number, text) VALUES ('06207720300', 'teszt2');");
}

int main (int argc, char *argv[]) {
    pg1 = pg_new("hostaddr = '127.0.0.1' port = '5432' dbname = 'sms' user = 'sms' password = 'SokSMS'");

    if (pg_error(pg1)) {
        fprintf(stderr, "Hiba pg_new() után: %s\n", pg_error(pg1));
    }

    create_database();

    pg_query(pg1, "SELECT * FROM sms WHERE sent=false;");
    
    pg_get_res(pg1);

    return 0;
}


