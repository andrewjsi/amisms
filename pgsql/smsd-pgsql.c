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

int main (int argc, char *argv[]) {
    pg1 = pg_new("hostaddr = '127.0.0.1' port = '5432' dbname = 'sms' user = 'sms' password = 'SokSMS'");

    pg_query(pg1, "\
CREATE TABLE IF NOT EXISTS sms ( \
    id              SERIAL PRIMARY KEY, \
    phone_number    TEXT \
);");

    return 0;
}


