CC = gcc
CFLAGS = -Wall 
CFLAGS += -I../../libamievent
CFLAGS += -I../../libamievent/libc-jss
CFLAGS += -I../../src/pdu
CFLAGS += -I../../src
CFLAGS += -ggdb
LDFLAGS = -lev -lm

OBJ += ../../libamievent/libc-jss/ini/ini.o
OBJ += ../../src/conf.o
PROGS=test

.PHONY: all run clean
all: $(patsubst %, %.o, $(PROGS)) $(OBJ) $(PROGS)

%: %.o $(OBJ)
	gcc $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o $(OBJ) $(PROGS)

run:
	./test >output
