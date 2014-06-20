CC = gcc
CFLAGS = -Wall -I../libamievent -I../libamievent/libc-jss -I../pdu
# CFLAGS += -ggdb
LDFLAGS = -lev -lm

OBJ += ../libamievent/libc-jss/netsocket.o
OBJ += ../libamievent/libc-jss/logger.o
OBJ += ../libamievent/libc-jss/misc.o
OBJ += ../libamievent/ami.o
OBJ += pdu/charset.o
OBJ += pdu/compat.o
OBJ += pdu/pdu.o
OBJ += banner.o
PROGS=sms

.PHONY: all
all: $(patsubst %, %.o, $(PROGS)) $(OBJ) $(PROGS)

%: %.o $(OBJ)
	gcc $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o $(OBJ) $(PROGS)

