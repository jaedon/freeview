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
# $brcm_Revision: 18 $
# $brcm_Date: 9/6/12 11:28p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/lib/mlibs/Makefile $
# 
# 18   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 17   3/24/11 5:01p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 16   5/19/08 12:03p erickson
# PR42752: added BDBG_OBJECT checks
# 
# 15   12/5/06 7:25p haisongw
# PR26369: mtimer doesn't work after system time gets updated
# 
# 13   7/14/06 11:30a jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/2   7/6/06 4:43p jgarrett
# PR 19909: Merging to latest baseline
# 
# 12   5/31/06 4:18p tokushig
# PR21891: Merging updated picture viewer into mainline.
# 
# SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
# merge from main
# 
# 11   4/17/06 5:04p jgarrett
# PR 20951: Merging skinned UI into mainline
# 
# SanDiego_Brutus_Skin/1   3/21/06 10:27a tokushig
# removed -O2 and add -g compilation flags for debugger support
# 
# PR19909/1   7/6/06 10:51a jgarrett
# PR 19909: Reducing brutus build output
# 
# 10   1/18/06 12:02p rjlewis
# PR19044: support for vxworks 6.
# 
# 9   12/12/05 4:58p hkuhnert
# PR15072: Fix mkdir error for windows vxWorks builds that I introduced
# in previous file.
# 
# 8   12/8/05 8:51a hkuhnert
# PR15072: Change 'move' to 'mv' to allow Linux host builds.  Only mkdir
# $(ODIR) if it doesn't exist.
# 
# 7   8/8/05 5:22p erickson
# PR15139: vxworks build fix
# 
# 6   8/4/05 4:44p erickson
# PR15139: consolidated system-specific build configuration into
# BSEAV/build/refsw_inc.mak
# 
# 5   5/5/05 2:31p erickson
# PR15159: added double quotes for DOS builds
# 
# 4   2/23/05 2:31p erickson
# PR14180: rreorder system and tools
# 
# 3   2/23/05 10:50a erickson
# PR14180: DEBUG is now defaults in tools
# 
# 2   2/17/05 1:52p erickson
# PR9497: make more vxworks-friendly
# 
# 1   2/7/05 11:08p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/16   7/28/04 11:58a erickson
# PR11768: added DBG support
#
# Irvine_BSEAVSW_Devel/15   5/12/04 10:29a erickson
# PR10967: use std header
############################################################

# ARCH allows you to put all the binaries into a subdirectory
# so that you can support multiple platforms at the same time.
# If ARCH is blank, then use ., which puts the binaries in
# the current directory.
#
# For instance, ARCH=mipsel-linux or ARCH=i386-linux
#
# However, ARCH does not change your compiler definitions.
# Instead you should define CXX and AR in your environment.
#

include ../../api/build/tools.mak
BSEAV = $(shell cd "../.." && ${PWD})
include $(BSEAV)/build/refsw_inc.mak
BWIN_SUPPORT = y
ODIR=$(B_REFSW_ARCH).$(DEBUG_SUFFIX)

MWIDGETS_LIB=$(ODIR)/libmwidgets.a
MCOMMON_LIB=$(ODIR)/libmcommon.a
MNET_LIB=$(ODIR)/libmnet.a

MCOMMON_SRC=\
	mstring.cpp mgeom.cpp mstringlist.cpp \
	mvoidlist.cpp mregexp.cpp mstringhash.cpp
#MCOMMON_SRC += mdataarray.cpp

MNET_SRC=\
	murl.cpp mxmlelement.cpp mxmlparser.cpp mhttpclient.cpp \
	mxmlrpc.cpp mxpath.cpp mhttpserver.cpp mhttpengine.cpp \
	mhttpsocketserver.cpp mhttpfileserver.cpp

MWIDGETS_SRC=\
	mapplication.cpp mframebuffer.cpp mwidget.cpp mpainter.cpp \
	mbutton.cpp mlabel.cpp mfont.cpp mpixmap.cpp \
	mtimer.cpp mlistbox.cpp \
	mscrollbar.cpp mlineedit.cpp mmessagebox.cpp \
	mradiobuttongroup.cpp mstyle.cpp \
	mfontmetrics.cpp mscrollview.cpp mlistview.cpp \
	mlistviewdialog.cpp \
	mfileview.cpp miodevice.cpp

MCOMMON_OBJECTS=$(foreach src,$(MCOMMON_SRC:.cpp=.o),$(ODIR)/$(src))
MWIDGETS_OBJECTS=$(foreach src,$(MWIDGETS_SRC:.cpp=.o),$(ODIR)/$(src))
MNET_OBJECTS=$(foreach src,$(MNET_SRC:.cpp=.o),$(ODIR)/$(src))

CXXFLAGS = -MD -O2 -fno-rtti -fno-exceptions
CXXFLAGS += $(B_REFSW_CFLAGS) $(B_REFSW_GENERIC_MAGNUM_CFLAGS) $(B_REFSW_MAGNUM_INCLUDE_DIRS)

.PHONY:tools mcommon mnet mwidgets

all: tools mcommon mnet mwidgets 

# need to add bdbg.inc source to build these apps
#$(ODIR)/webserver $(ODIR)/webclient

tools:
ifneq ($(VERBOSE),)
	@echo "mlibs build tools for $(ODIR):"
	@echo "  CXX = $(CXX)"
	@echo "  LD = $(LD)"
	@echo "  AR = $(AR)"
endif

mwidgets: dirs $(MWIDGETS_LIB)

mcommon: dirs $(MCOMMON_LIB)

mnet: dirs $(MNET_LIB)

ifeq ($(OSTYPE),linux)
dirs:
	-@if [ ! -d $(ODIR) ]; then $(MKDIR) $(ODIR); fi
else
dirs:
	${Q_}$(MKDIR) $(ODIR)
endif

ifeq ($(BWIN_SUPPORT),y)
include $(BSEAV)/lib/bwin/include/bwin.mak
CXXFLAGS += ${BWIN_CFLAGS} -DBWIN_SUPPORT
endif

ifeq ($(DSG_SUPPORT), y) 
CXXFLAGS += -DDSG_SUPPORT
endif

$(ODIR)/%.o: %.cpp
	@echo [Compile... $(notdir $<)]
	${Q_}$(CXX) -c -o $@ $< $(CXXFLAGS)
ifeq ($(SYSTEM),vxworks)
ifneq ($(vxWorksVersion),6)
	@$(MV) $(patsubst %.o,%.d,$(notdir $@)) $(ODIR)
endif
endif

$(MCOMMON_LIB): $(MCOMMON_OBJECTS)
	@echo [Linking... $(notdir $@)]
	@$(AR) cr $@ $^
$(MWIDGETS_LIB): $(MWIDGETS_OBJECTS)
	@echo [Linking... $(notdir $@)]
	@$(AR) cr $@ $^
$(MNET_LIB): $(MNET_OBJECTS)
	@echo [Linking... $(notdir $@)]
	@$(AR) cr $@ $^

clean:
	-${Q_}$(RM) *.o *.a *.d webserver webclient
	-${Q_}$(RM) $(ODIR)/*.o $(ODIR)/*.a $(ODIR)/*.d $(ODIR)/webserver $(ODIR)/webclient

-include $(ODIR)/*.d

$(ODIR)/webserver: $(ODIR)/webserver.o $(MNET_LIB) $(MCOMMON_LIB)
	@echo [Linking... $(notdir $@)]
	${Q_}$(CXX) -o $@ $^ $(B_REFSW_LDFLAGS) $(LDFLAGS) 
$(ODIR)/webclient: $(ODIR)/webclient.o $(MNET_LIB) $(MCOMMON_LIB)
	@echo [Linking... $(notdir $@)]
	${Q_}$(CXX) -o $@ $^ $(B_REFSW_LDFLAGS) $(LDFLAGS) 

