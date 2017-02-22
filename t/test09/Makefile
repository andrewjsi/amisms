CC = gcc
CFLAGS = -Wall 
CFLAGS += -I../../libamievent
CFLAGS += -I../../libamievent/libc-jss
CFLAGS += -I../../src
CFLAGS += -ggdb
LDFLAGS = -lev -lm

OBJ += ../../src/option.o ../../libamievent/libc-jss/misc.o ../../src/verbose.o
PROGS=test

CFLAGS += -DVERSION=\"test\"

.PHONY: all run clean
all: $(patsubst %, %.o, $(PROGS)) $(OBJ) $(PROGS)

%: %.o $(OBJ)
	gcc $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o $(OBJ) $(PROGS)

run:
	./test --config=conf -f 59 61 >output
