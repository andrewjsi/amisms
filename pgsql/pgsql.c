#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "misc.h"
#include "pgsql.h"

pg_t *pg_new (const char *connect_string) {
    pg_t *pg = malloc(sizeof(*pg));
    if (pg == NULL) {
        fprintf(stderr, "pg_new() returned NULL");
        return NULL;
    }
    bzero(pg, sizeof(*pg)); // minden NULL

    scpy(pg->connect_string, connect_string);

    return pg;
}

int pg_query (pg_t *pg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pg->buf_query, sizeof(pg->buf_query), fmt, ap);
    va_end(ap);

    return 0;    
}

static void error (pg_t *pg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pg->error_message, sizeof(pg->error_message), fmt, ap);
    va_end(ap);

}

static int connect_to_postgresql (pg_t *pg) {
    pg->pg_conn = PQconnectdb("hostaddr = '127.0.0.1' port = '5432' dbname = 'sms' user = 'sms' password = 'SokSMS'");

    if (!pg->pg_conn) {
        error(pg, "libpq error: PQconnectdb returned NULL.");
        return -1;
    }
    if (PQstatus(pg->pg_conn) != CONNECTION_OK) {
        error(pg, "Can't connect to PosgreSQL: %s", PQerrorMessage(pg->pg_conn));
        return -1;
    }

    return 0;
}



