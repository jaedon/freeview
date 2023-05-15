APPLIBS_TOP := ${shell pwd}/../../../
include $(APPLIBS_TOP)common/common.inc



THEAPPS = tr69c

#APPS = $(THEAPPS)	


.PHONY: api clean

OBJDIR=./obj
BINDIR=./bin


#$(APPS): api



# This builds the settop api
#api:



# Always build with debug
CFLAGS += -g



CFLAGS += -I./main
CFLAGS += -I./bcmLibIF
CFLAGS += -I./nanoxml
CFLAGS += -I./webproto
CFLAGS += -I./standard

#SRCS += subtitles.c subtitle_control.c subtitle_priv.c subtitle_parse.c
SRCS += ./main/testSetup.c \
		./main/tr69c.c\
		./main/utils.c \
		./main/event.c \
		./main/md5.c \
		./main/informer.c \
		./main/acsListener.c \
		./main/httpDownload.c \
		./main/httpUpload.c \
		./bcmLibIF/bcmWrapper.c \
		./bcmLibIF/bcmLanIF.c \
		./bcmLibIF/bcmWanIF.c \
		./bcmLibIF/bcmUserIF.c \
		./bcmLibIF/bcmSysIF.c \
		./nanoxml/nanoxml.c \
		./SOAPParser/RPCState.c \
		./SOAPParser/xmlParserSM.c \
		./SOAPParser/xmlTables.c \
		./SOAPParser/CPEframework.c \
		./webproto/protocol.c \
		./webproto/www.c \
		./webproto/wget.c \
		./standard/standardparams.c \
		./standard/rootparams.c \
		./standard/baselineparams.c \
		./standard/baselinehandlers.c \
		./standard/lanparams.c \
		./standard/wanparams.c \
		./standard/lanparams.c \
		./standard/routeparams.c \
		./standard/ippingparams.c \
		./standard/timeparams.c \
		./standard/userinterfaceparams.c \

OBJS = $(foreach file, $(SRCS), $(OBJDIR)/$(notdir $(patsubst %.c, %.o, $(file))))
SHARED_LIB= libTR69.so
vpath %.c $(foreach file, $(SRCS), $(dir $(patsubst %.c, %.o, $(file))))

all: $(BINDIR)/$(SHARED_LIB) $(BINDIR)/$(THEAPPS)

$(BINDIR)/$(THEAPPS): $(OBJS)
	@echo "[Link...... $(notdir $@)]"
	$(Q_)$(CC) -o $@ $^


$(BINDIR)/$(SHARED_LIB): $(OBJS) 
	@echo "[Link...... $(notdir $@)]"
	$(Q_)$(CC) -shared -o $@ $^


$(OBJDIR)/%.o: %.c ${OBJDIR}/exists ${BINDIR}/exists
	@echo [Compile... $(notdir $<)]
	$(Q_)$(CC) -c -fPIC -o $@ $< $(CFLAGS)

$(OBJDIR)/exists:
	mkdir -p $(OBJDIR)
	@touch $@

$(BINDIR)/exists:
	mkdir -p $(BINDIR)
	@touch $@


# This cleans local apps
clean:
	-$(RM) -r $(BINDIR) $(OBJDIR)

#tr69c$(APPSUFFIX): $(OBJS) $(VXOBJS)
#	@echo [Link... $@]
#	$(Q_)$(CC) -o $@ $(filter %.o, $^) $(LDFLAGS)
#	cp -f $@ ${FONTS} ${NEXUS_BIN_DIR}



#install: all
