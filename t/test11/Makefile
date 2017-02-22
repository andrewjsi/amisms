CC = gcc
CFLAGS = -Wall 
CFLAGS += -I../../libamievent/libc-jss
CFLAGS += -I../../src
CFLAGS += -ggdb
LDFLAGS = -lev -lm

OBJ += ../../libamievent/libc-jss/misc.o
OBJ += ../../src/pnv.o
PROGS=test

CFLAGS += -DVERSION=\"test\"

.PHONY: all run clean
all: $(patsubst %, %.o, $(PROGS)) $(OBJ) $(PROGS)

%: %.o $(OBJ)
	gcc $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o $(OBJ) $(PROGS)

run:
	./test >output
