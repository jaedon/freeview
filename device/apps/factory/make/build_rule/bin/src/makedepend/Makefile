# Makefile for unix
CC=gcc
CFLAGS=-g -DSTANDALONE=1 -DOBJSUFFIX=\"o\"

makedepend: main.o pr.o parse.o include.o cppsetup.o ifparser.o
	$(CC) -o $@ $^
	cp $@ ../..
		
clean:
	$(RM) *.o *~ makedepend Makefile.bak

# DO NOT DELETE
