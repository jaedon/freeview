CC=$(ARCH)gcc
OBJDIR=$(ARCH)obj/
CFLAGS = -mips32 -o2 -Wall -fPIC -Wall -Wextra -Wcast-align -Wno-unused-parameter -Wshadow -Wwrite-strings -Wcast-qual -fno-strict-aliasing -fsigned-char -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-common -ansi -pedantic -lstdc++ -lrt
NAME=uapi_example

OBJ= RemoTI.o QuickSet.o callbacks.o uapi_rf4ce_if.o
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) libuapi.so -Wl,-rpath -Wl,./ -o $(NAME) -lpthread

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf $(NAME)
