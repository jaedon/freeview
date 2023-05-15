SRCS = RPCState.c xmlParserSM.c xmlTables.c
SRCS+= CPEframework.c
OBJS = $(SRCS:%.c=%.o)
HEADERS = parseUtil.h RPCState.h xmlParserSM.h CPEframework.h
HEADERS += xmlTables.h

OBIN=   soaplib.o

ifeq ($(strip $(BRCM_VODSL_CFGMGR)),1)
ifeq ($(strip $(SIPLOAD)),1)
CFLAGS += -DWT104_SUPPORT
CFLAGS += -DVOIP_CFGMGR
endif
endif

$(OBIN):        $(OBJS) $(HEADERS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	rm -f *~ $(OBJS) $(OBIN)

%.o:%.c %.h
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)


