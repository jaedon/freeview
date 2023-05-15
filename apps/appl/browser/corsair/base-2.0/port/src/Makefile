#
#	Makefile for BPL
#
#	Written by Jongwon Kim
#		2011. 8. 19.
#

TOPDIR		:= ../
-include $(TOPDIR)makehead.env
-include $(PWD)/.globalconfig

OCTODIR		:= $(PWD)/../octo
LIBDIR		:= $(OCTODIR)/library
SVCDIR		:= $(OCTODIR)/service
DEVICEDIR	:= $(PWD)/../../device/include
INCDIR		:= -I$(OCTODIR)/appkit/base
INCDIR		+= -I$(LIBDIR)/hlib/include
INCDIR		+= -I$(LIBDIR)/dlib/include
INCDIR		+= -I$(LIBDIR)/clib/include
INCDIR		+= -I$(LIBDIR)/rlib/include
INCDIR		+= -I$(LIBDIR)/silib/include
INCDIR		+= -I$(SVCDIR)/obama/include
INCDIR		+= -I$(SVCDIR)/hama/include
INCDIR		+= -I$(SVCDIR)/pama/include
INCDIR		+= -I$(SVCDIR)/umma/include
INCDIR		+= -I$(SVCDIR)/thma/include
INCDIR		+= -I$(SVCDIR)/dama/include
INCDIR		+= -I$(OCTODIR)/appkit/include
INCDIR		+= -I$(PWD)/../appl/nova/include
INCDIR		+= -I$(LIBDIR)/memory
INCDIR		+= -I$(PWD)/include
INCDIR		+= -I./opl
INCDIR		+= -I../include/opera4
INCDIR		+= -I$(DEVICEDIR)/hos
INCDIR		+= -I$(PWD)/../../packages/libxml2/include

ifeq ($(strip $(CONFIG_ALEXA)),y)
ifeq ($(strip $(CONFIG_PROD_FVP4000T)),y)
INCDIR		+= -I$(DEVICEDIR)/directfb
endif
endif

ifeq ($(strip $(CONFIG_NETFLIX_APP)),y)
INCDIR		+= -I$(PWD)/../appl/netflix/netflixmanager
endif

CFLAGS		+= -Wall -include $(PWD)/include/globalconfig.h
CFLAGS      += $(INCDIR)
CFLAGS      += $(HUMAX_EXTRA_CFLAG)

QTMETASRC	:=
#_SRC		:=	bpl/bplgraphic.c
C_SRC		:=	bpl/bplinit.c		\
				bpl/bplait.c		\
				bpl/bpldsmcc.c		\
				bpl/bpldb.c		\
				opl/Common/oplevent.c		\
				opl/Common/oplbookmark.c	\
				opl/LocalSystem/oplswupdate.c	\
				opl/VideoResource/oplbroadcast.c	\
				opl/DVB/oplchannel.c	\
				opl/DVB/oplfavourite.c	\
				opl/LocalSystem/opllocalsystem.c	\
				opl/PVR/oplrecorder.c			\
				opl/MetaSearch/oplsearchresult.c			\
				opl/FileSystem/opldiscinfo.c			\
				opl/Programme/oplprogramme.c		\
				opl/MetaSearch/oplquery.c		\
				opl/Common/oplcollection.c		\
				opl/MetaSearch/oplmetadatasearch.c		\
				opl/PVR/oplscheduledprogramme.c		\
				opl/PVR/oplrecording.c		\
				opl/PVR/oplrecorditem.c		\
				opl/PVR/oplscheduler.c		\
				opl/DVB/oplantinfo.c		\
				opl/LocalSystem/oplpcmanager.c		\
				opl/CASnDRM/oplmail.c \
				opl/IPService/opldial.c \
				opl/IPService/oplsatip.c \
				opl/IPService/oplalexahelper.c \
				opl/LocalSystem/oplnagrapcmanager.c

ifeq ($(strip $(CONFIG_OP_FREESAT)),y)
C_SRC		+=		opl/DVB/oplsqcinfo_freesat.c
C_SRC		+=		opl/Common/oploperator_freesat.c
endif

ifeq ($(strip $(CONFIG_OP_JAPAN)),y)
C_SRC		+=		opl/Programme/oplprogramme_japan.c
C_SRC		+=		opl/LocalSystem/opljlabslocalsystem.c
C_SRC		+=		opl/LocalSystem/opljlabssysteminfo.c
C_SRC		+=		opl/DVB/oplcasbc.c
C_SRC		+=		opl/DVB/oplbuilder_japan.c
C_SRC		+=		opl/Programme/oplparser_japan.c
C_SRC		+=		opl/Common/oplppvhistory.c
else
C_SRC		+=		opl/DVB/oplbuilder.c
endif

ifeq ($(strip $(CONFIG_OP_MIDDLE_EAST)),y)
C_SRC		+=		opl/CASnDRM/oplhmxdrm.c
endif

ifeq ($(strip $(CONFIG_OP_AUS_DTT)),y)
C_SRC		+=		opl/CASnDRM/oplhmxdrm.c
endif

ifeq ($(strip $(CONFIG_OP_SES)),y)
INCDIR		+= -I./opl/PVR/sat_record
C_SRC		+=		opl/PVR/sat_record/crc.c
C_SRC		+=		opl/PVR/sat_record/md5.c
C_SRC		+=		opl/PVR/sat_record/rr.c
endif

C_SRC		+=		opl/Programme/oplparser.c

CXX_SRC		:=	bpl/bplait_x.cpp		\
				bpl/bplglobal_x.cpp	\
				bpl/bplcookie.cpp	\
				opl/VideoResource/oplbroadcast_x.cpp	\
				opl/DVB/oplchannel_x.cpp	\
				opl/LocalSystem/opllocalsystem_x.cpp	\
				opl/FileSystem/opldiscinfo_x.cpp	\
				opl/LocalSystem/oplnetwork_x.cpp \
				opl/FileSystem/oplstorageutil_x.cpp \
				opl/FileSystem/oplstorageutilprivate.cpp \
				opl/IPService/opldmp_x.cpp	\
				opl/CASnDRM/oplcas_x.cpp	\
				opl/VideoResource/oplvod.cpp	\
				opl/VideoResource/oplvod_avc.cpp	\
				opl/VideoResource/oplvod_html5.cpp	\
				opl/VideoResource/oplvod_extapp.cpp	\
				opl/ApplicationManager/oplappmgr.cpp	\
				opl/IPService/oplwoon_x.cpp	\
				opl/FileSystem/oplfilemgr.cpp		\
				opl/IPService/opldmr_x.cpp	\
				opl/IPService/opldms_x.cpp	\
				opl/IPService/oplairplay_x.cpp \
				opl/VideoResource/cvideoresource.cpp		\
				opl/VideoResource/cvideoresourcemanager.cpp		\
				opl/VideoResource/oplvideohandle.cpp		\
				opl/VideoResource/oplvideobroadcasthandle.cpp		\
				opl/VideoResource/oplvodhandle.cpp		\
				opl/VideoResource/oplvodavchandle.cpp		\
				opl/VideoResource/oplvodhtml5handle.cpp		\
				opl/opera4/OplVideoOnDemand.cpp	\
				opl/opera4/OplVideoBroadcast.cpp	\
				opl/opera4/OplAudioControl.cpp	\
				opl/FileSystem/oplsamba.cpp	\
				opl/LocalSystem/oplftpmgr.cpp	\
				opl/MetaSearch/oplsearchmanager.cpp	\
				opl/MetaSearch/oplmetadatasearchhandle.cpp	\
				opl/MetaSearch/oplsearchresultshandle.cpp		\
				opl/MetaSearch/oplqueryhandle.cpp		\
				opl/CASnDRM/opldrmagent.cpp		\
				opl/LocalSystem/oplbluetoothmanager.cpp	\
				opl/DVB/oplfavouritehandle.cpp	\
				opl/DVB/cfavouritemanager.cpp	\
				opl/DVB/oplprogramme_x.cpp \
				opl/LocalSystem/oplnetflix.cpp	\
				opl/VideoResource/oplvideoutil.cpp \
#				opl/FileSystem/oplmediascan_x.cpp \

ifeq ($(strip $(CONFIG_OP_JAPAN)),y)
CXX_SRC		+=	opl/LocalSystem/opljlabslocalsystem_x.cpp
CXX_SRC		+=	opl/Common/oplrcf.cpp
CXX_SRC		+=	opl/VideoResource/oplvod_jlabs.cpp
CXX_SRC		+=  opl/VideoResource/oplvodjlabshandle.cpp
endif

ifeq ($(strip $(CONFIG_PROD_HD9000I)),y)
CXX_SRC		+=	opl/LocalSystem/oplwebappupgrade.cpp
endif

TARGET		:= libbpl.so

### Targets
$(TARGET): $(OBJS)
	$(CC) -shared $(OBJS) -o libbpl.so
#	$(AR) rs $@ $(OBJS)
#	$(STRIP) $(TARGET)

include $(TOPDIR)maketail.env
