############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 5 $
# $brcm_Date: 11/3/10 12:40p $
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/bwidgets/tests/Makefile $
# 
# 5   11/3/10 12:40p erickson
# SW7420-1148: update speed
# 
# 4   10/27/10 11:17a erickson
# SW7420-1148: update
# 
# 3   10/26/10 11:02a erickson
# SW7420-1148: create client/server gfx app
# 
# 2   10/22/10 3:21p erickson
# SWDEPRECATED-626: revive bwidgets on nexus
# 
############################################################
Q_=@
NEXUS_TOP=$(shell cd ../../../../nexus;pwd)

APPS=test stb_server stb_gallery_client
all: $(APPS) bwidgets

include ../include/bwidgets.mak
include ../build/tools.mak
include $(NEXUS_TOP)/../nexus/platforms/$(PLATFORM)/build/platform_app.inc

CFLAGS += $(BWIDGETS_CFLAGS)
LDFLAGS += $(BWIDGETS_LDFLAGS)
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
LDFLAGS += ${NEXUS_LDFLAGS}
LDFLAGS += -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm

.PHONY: bwidgets clean-bwidgets

bwidgets:
	make -C ../../bwin/build
	make -C ../build

clean_bwidgets:
	make -C ../../bwin/build clean
	make -C ../build clean
	
$(OBJS): bwidgets

test: test.o b_app_server.o b_app_client.o bwidgets
	@echo Link $@
	${Q_}$(CC) -o $@ $(filter %.o,$^) $(LDFLAGS)

stb_server: stb_server.o b_app_server.o b_app_client.o bwidgets
	@echo Link $@
	${Q_}$(CC) -o $@ $(filter %.o,$^) $(LDFLAGS)

stb_gallery_client: stb_gallery_client.o b_app_client.o bwidgets
	@echo Link $@
	${Q_}$(CC) -o $@ $(filter %.o,$^) $(LDFLAGS)

%.o: %.c
	@echo Compile $@
	${Q_}$(CC) -c -o $@ $< $(CFLAGS)

clean_apps: 
	$(RM) $(APPS) *.o

clean: clean_apps clean_bwidgets

ifneq ($(DESTDIR),)
install:
	$(CP) $(APPS) $(DESTDIR)
endif
