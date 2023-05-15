SRCS = nanoxml.c
OBJS = $(SRCS:%.c=%.o)
HEADERS = nanoxml.h

OBIN=   nanoxmllib.o

$(OBIN):        $(OBJS) $(HEADERS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	rm -f *~ $(OBJS) $(OBIN)

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)


