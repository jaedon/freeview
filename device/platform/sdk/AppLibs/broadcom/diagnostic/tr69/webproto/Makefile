OBJS = protocol.o www.o wget.o 
OBIN=	protocollib.o

$(OBIN):	$(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	rm -f *~ $(OBJS) $(OBIN)

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)




