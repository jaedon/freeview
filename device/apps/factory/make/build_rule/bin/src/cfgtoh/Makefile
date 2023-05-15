# Makefile for unix
CC=gcc
LDFLAGS=-lm

cfgtoh: cfgtoh.o
	$(CC) $(LDFLAGS) -o $@ $^
	cp $@ ../..

clean:
	$(RM) *.o *~ cfgtoh Makefile.bak
