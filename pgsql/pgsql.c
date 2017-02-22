#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "misc.h"
#include "pgsql.h"

#include "debug.h"

static void error_set (pg_t *pg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pg->error_message, sizeof(pg->error_message), fmt, ap);
    va_end(ap);

    fprintf(stderr, "[error_set] %s\n", chomp(pg->error_message));
}

static void error_clear (pg_t *pg) {
    pg->error_message[0] = '\0';
}

static int connect_to_postgresql (pg_t *pg) {
    pg->conn = PQconnectdb(pg->connect_string);

    if (!pg->conn) {
        error_set(pg, "libpq error: PQconnectdb returned NULL.");
        return -1;
    }
    if (PQstatus(pg->conn) != CONNECTION_OK) {
        error_set(pg, PQerrorMessage(pg->conn));
        return -1;
    }

    pg->connected = 1;
    return 0;
}

pg_t *pg_new (const char *connect_string) {
    pg_t *pg = malloc(sizeof(*pg));
    if (pg == NULL) {
        fprintf(stderr, "pg_new() returned NULL");
        return NULL;
    }
    bzero(pg, sizeof(*pg)); // minden NULL

    scpy(pg->connect_string, connect_string);
    connect_to_postgresql(pg);
    return pg;
}

int pg_query (pg_t *pg, const char *fmt, ...) {
    error_clear(pg);

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pg->buf_query, sizeof(pg->buf_query), fmt, ap);
    va_end(ap);

    pg->res = PQexec(pg->conn, pg->buf_query);

    if (PQresultStatus(pg->res) == PGRES_BAD_RESPONSE ||
        PQresultStatus(pg->res) == PGRES_NONFATAL_ERROR ||
        PQresultStatus(pg->res) == PGRES_FATAL_ERROR
    ) {
        error_set(pg, PQerrorMessage(pg->conn));
        PQclear(pg->res);
        pg->res = NULL;
        return -1;
    }

    return 0;    
}

// ITT TARTOK: az adatokat vissza kell adni a hívónak. Ehhez a ds4 / db.c
// megvalósítást kell alkalmazni!
void pg_get_res (pg_t *pg) {
    int nFields, i, j;

    /* first, print out the attribute names */
    nFields = PQnfields(pg->res);
    for (i = 0; i < nFields; i++)
        printf("%-15s", PQfname(pg->res, i));
    printf("\n\n");

    /* next, print out the rows */
    for (i = 0; i < PQntuples(pg->res); i++)
    {
        for (j = 0; j < nFields; j++)
            printf("%-15s", PQgetvalue(pg->res, i, j));
        printf("\n");
    }
}

const char *pg_error (pg_t *pg) {
    if (!strlen(pg->error_message)) {
        return NULL;
    } else {
        return pg->error_message;
    }
}

