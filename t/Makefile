# test top-dir Makefile

MAKEFLAGS += --no-print-directory
TESTS := $(shell find -maxdepth 1 -type d -name 'test*' | sort -n)

.PHONY: all clean
.DEFAULT: all

all: compile run check_output

compile:
	echo $(TESTS) >testlist
	@for dir in ${TESTS}; do ( cd $$dir && ${MAKE} all ); done

run:
	@chmod 600 */data/test.ini
	@for dir in ${TESTS}; do ( cd $$dir && ${MAKE} run ); done

check_output:
	./check_output.sh

clean:
	@for dir in ${TESTS}; do ( cd $$dir && ${MAKE} clean ); done
	@rm -f testlist

