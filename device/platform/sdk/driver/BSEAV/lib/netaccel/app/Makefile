############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 9 $
# $brcm_Date: 9/6/12 11:28p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/netaccel/app/Makefile $
# 
# 9   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 8   5/23/08 2:42p ssood
# PR42242: compile udp streamer & receiver sample apps
# 
# 7   3/14/08 9:40p ssood
# PR39645: adding new test programs for testing HTTP client & server
# functionality
# 
# 6   10/4/07 3:27p ssood
# PR33786: fixed udpstreamer build issue: not properly linking w/
# libsettop
# 
# 5   10/4/07 2:53p ssood
# PR33786: fixed udpstreamer build issue: not properly linking w/
# libsettop
# 
# 4   10/3/07 3:02p ssood
# PR33786: udpstreamer enhancements to utilize the pcr based pacing
# 
# 3   9/6/07 4:41p jrubio
# PR34597: netaccel library name change
# 
# 2   8/20/07 7:03p ssood
# PR33786: adding standalone udpserver app
# 
# 1   8/10/07 12:18a ssood
# PR33786: changes to accomodate Net DMA & IP Streamer refactoring into
# bcmnetaccel driver
# 
# Previous history
# 3   5/18/07 5:23p jrubio
# PR30010: modified top level makefiles
# 
# 2   4/6/07 9:40a jrubio
# PR29565: new ipstreamer code base
# 
# 1   10/11/06 3:37p jrubio
# PR24672:Initial checkin for IPStreamer lib. Yasantha's ip streamer code
# with some mods.
#
############################################################

include ../../../api/build/tools.mak
all: $(TARGETS)
TOP_DIR := $(shell cd ".." && ${PWD})
OS:=linuxuser
SYSTEM:=linux
BSEAV:= $(shell cd "../../.." && ${PWD})
MAGNUM:=${BSEAV}/../magnum
include ${TOP_DIR}/include.mak


#
# Driver Test Targets
#

UDPSTREAMER	= udpstreamer
HTTP_TEST_CLIENT	= http_test_client
UDP_TEST_CLIENT		= udp_test_client
UDP_TEST_SERVER		= udp_test_server
HTTP_TEST_SERVER = http_test_server
STREAMER	= streamer
NETRECORD	= netrecord
HTTPSTREAMER	= httpstreamer


# SGIL Test Targets
#

BOUNCE		= bounce


#
# include definitions for building libraries and applications for the settop
#

LDFLAGS += -L$(TOP_DIR)/lib/obj -L$(TOP_DIR)/bin/test -L$(BSEAV)/bin

#
# Program objects
#

STREAMER_OBJS := streamer.o
UDPSTREAMER_OBJS		= udpstreamer.o
HTTP_TEST_CLIENT_OBJS		= http_test_client.o
UDP_TEST_CLIENT_OBJS		= udp_test_client.o
UDP_TEST_SERVER_OBJS		= udp_test_server.o
HTTP_TEST_SERVER_OBJS = http_test_server.o
HTTP_TEST_SERVER_OBJS = http_test_server.o
HTTP_TEST_SERVER_OBJS = http_test_server.o
NETRECORD_OBJS		= netrecord.o
HTTPSTREAMER_OBJS		= httpstreamer.o


#
# Set up VPATH(s)
#

vpath %.h $(BCM_APP_INC)
vpath %.c $(BCM_APP_SRC)

NETACCELLIB                 = netaccel
BCMINDEXER		= bcmindexer
BCMPLAYER		= bcmplayer
SETTOP			= settop
#DRIVERS is what is copied over to bin dir 
DRIVERS:=httpstreamer udpstreamer http_test_client http_test_server udp_test_client udp_test_server
#
# Targets
#

CFLAGS+= -DBDBG_DEBUG_BUILD ${B_REFSW_MAGNUM_INCLUDE_DIRS} ${B_REFSW_CFLAGS} -I../module -DCMDLINE_TEST -I../lib
CFLAGS += -I${TOP_DIR}/include -I${TOP_DIR}/module/bne -I${TOP_DIR}/module/core -I${TOP_DIR}/module/drv -I${BSEAV}/api/include -I${BSEAV}/lib/bfile


TARGETS = \
	$(HTTPSTREAMER)\
	$(HTTP_TEST_CLIENT)\
	$(HTTP_TEST_SERVER)\
	$(UDP_TEST_CLIENT)\
	$(UDP_TEST_SERVER)\
	$(UDPSTREAMER) 
#      $(STREAMER) \

all: $(TARGETS)
	
$(NETRECORD): $(NETRECORD_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic -l$(NETACCELLIB) -lpthread

$(HTTPSTREAMER): $(HTTPSTREAMER_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic -l$(NETACCELLIB) -lpthread

$(HTTP_TEST_SERVER): $(HTTP_TEST_SERVER_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic

$(HTTP_TEST_CLIENT): $(HTTP_TEST_CLIENT_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic 

$(UDP_TEST_CLIENT): $(UDP_TEST_CLIENT_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic 

$(UDP_TEST_SERVER): $(UDP_TEST_SERVER_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic 

$(UDPSTREAMER): $(UDPSTREAMER_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -l$(NETACCELLIB) -lpthread 

$(STREAMER): $(STREAMER_OBJS)
	$(Q_)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Bstatic -l$(NETACCELLIB) \
		-lpthread


clean:
	${Q_}$(RM) $(TARGETS) -f *.o *.d *.map *~ core

%.o: %.c
	@echo "[Compiling... $<]"
	$(Q_)$(CC) $(CFLAGS) -MMD -c $< -o $@

# include individual depend files
-include *.d

