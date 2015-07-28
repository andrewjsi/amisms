CC = gcc
CFLAGS = -Wall -ggdb -I../../libamievent/libc-jss -DUSE_ICONV

OBJ += pdu.o charset.o compat.o ../verbose.o
PROGS=test

.PHONY: all
all: $(patsubst %, %.o, $(PROGS)) $(OBJ) $(PROGS)

%: %.o $(OBJ)
	gcc $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o $(OBJ) $(PROGS)

