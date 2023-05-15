#
OBIN=tr69c.o

OBJS=testSetup.o
OBJS+=main.o utils.o event.o md5.o informer.o acsListener.o httpDownload.o httpUpload.o
HDRS+=appdefs.h event.h md5.h utils.h informer.h acsListener.h
HDRS+=httpDownload.h httpUpload.h

$(OBIN):	$(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	rm -f *.o

%.o:%.c %.h
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)

