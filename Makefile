CC = gcc
CFLAGS += -Wall -pedantic -O2
OBJS = main.o board.o cards.o gameplay.o

.PHONY: clean runtest

onitama: $(OBJS)
	$(CC) -o onitama $(OBJS)

dumpcards: cards.o dumpcards.o
	$(CC) -o dumpcards cards.o dumpcards.o

clean:
	rm -f onitama dumpcards *.o .*.dep

.%.dep: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq "$(wildcard .*.dep)" ""
include .*.dep
endif

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c
	@if [ ! -e .$*.dep ]; then $(CC) -MM $(CFLAGS) $*.c > .$*.dep ; fi
