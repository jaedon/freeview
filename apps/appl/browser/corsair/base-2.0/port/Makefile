#
#	Makefile for BPL
#
#	Written by Jongwon Kim
#		2011. 8. 19.
#

PREFIX		:= /usr/
INST_INC	:= $(PREFIX)include/
INST_LIB	:= $(PREFIX)lib/

H_LIST		:= $(shell ls -1 include | grep .h$$)
L_LIST		:= $(shell ls -1 src | grep .a$$)

MAKE	:= make

### Targets
all: portlib

portlib:
	$(MAKE) -C src

ex:
	$(MAKE) -C samples

clean:
	$(MAKE) -C src clean
	$(MAKE) -C samples clean

#install:
#	-mkdir -p $(INST_INC)
#	-mkdir -p $(INST_LIB)
#	cp $(addprefix include/, $(H_LIST)) $(INST_INC)
#	cp $(addprefix src/, $(L_LIST)) $(INST_LIB)

uninstall:
	-rm $(addprefix $(INST_INC), $(H_LIST))
	-rm $(addprefix $(INST_LIB), $(L_LIST))

lsav: clean
	tar cvfz ../portlib -`date +%Y-%m-%d-%H-%M-%S`.tgz *	

cl:
	rm -f ChangeLog
	cvs2cl --no-wrap -S -T --hide-filenames
