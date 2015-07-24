#ifndef PG_H_INCLUDED
#define PG_H_INCLUDED

#include <libpq-fe.h>

typedef struct pg_t {
    char connect_string[256];                   // PostgreSQL connect string
    char buf_query[4096];                       // lekérdezés buffere
    int connected:1;                            // PostgreSQL kapcsolat aktív?
    PGconn *conn;                               // PGconn
    PGresult *res;                              // PGresult
    char error_message[512];                    // hibaüzenet ide kerül
} pg_t;

pg_t *pg_new (const char *connect_string);

int pg_query (pg_t *pg, const char *fmt, ...);

const char *pg_error (pg_t *pg);

void pg_get_res (pg_t *pg);

#endif // #ifndef PG_H_INCLUDED
