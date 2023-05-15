ifeq (${TESTBOX},true)
OBJS = rootparams.o baselineparams.o 
else
OBJS =  standardparams.o rootparams.o baselineparams.o baselinehandlers.o lanparams.o wanparams.o routeparams.o 
OBJS += ippingparams.o timeparams.o userinterfaceparams.o
endif
OBIN = stdtr69clib.o
HDRS =  sharedparams.h rootparams.h baselineparams.h lanparams.h wirelessparams.h wanparams.h routeparams.h
HDRS += ippingparams.h timeparams.h userinterfaceparams.h

ifeq ($(strip $(WIRELESS)),1)
OBJS += wirelessparams.o
endif

ifeq ($(strip $(BRCM_VODSL_CFGMGR)),1)
ifeq ($(strip $(SIPLOAD)),1)
OBJS += voiceserviceparams.o
CFLAGS += -DWT104_SUPPORT
CFLAGS += -DVOIP_CFGMGR
endif
endif

$(OBIN):	$(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	rm -f *~ $(OBJS) $(OBIN)

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)




