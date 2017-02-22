CC = gcc
CFLAGS = -O2 -Wall -Wno-strict-aliasing
CFLAGS += -ggdb
CFLAGS += -I../libamievent -I../libamievent/libc-jss
CFLAGS += -I/usr/include/postgresql
LDFLAGS = -lev -lm -lpq

OBJ += ../libamievent/libc-jss/ini/ini.o
OBJ += ../libamievent/libc-jss/logger.o
OBJ += ../libamievent/libc-jss/misc.o
OBJ += pgsql.o
PROGS=smsd-pgsql

.PHONY: all
all: $(patsubst %, %.o, $(PROGS)) $(OBJ) $(PROGS)

%: %.o $(OBJ)
	gcc $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o $(OBJ) $(PROGS)

