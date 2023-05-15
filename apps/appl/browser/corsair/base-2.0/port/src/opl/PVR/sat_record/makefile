CC=$(TARGET)gcc

.PHONY: clean
.DEFAULT_GOAL=remote_record_sample

OBJS := main.o rr.o md5.o crc.o
HEADERS := $(wildcard *.h)

%.c: $(HEADERS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c -o $*.o

remote_record_sample: $(OBJS)
	$(CC) $(OBJS) -o remote_record_sample

clean:
	rm -f *.o remote_record_sample
