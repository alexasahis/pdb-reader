CC     = gcc
CFLAGS = -Wall -g
BIN    = pdb
CSRCS  = $(wildcard *.c)
COBJS  = $(CSRCS:.c=.o)
CHDRS  = cmdline.h reader.h

.PHONY: all clean depend

all: depend $(BIN)
$(BIN): $(COBJS)
clean:
	rm $(BIN) $(COBJS)

pdb.o: pdb.h cmdline.h reader.h
cmdline.o: cmdline.h
reader.o: reader.h
