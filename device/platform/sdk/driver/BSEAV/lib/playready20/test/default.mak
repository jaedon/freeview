include $(DRM_TEST_PATH)/tools.mak

DRM_CFLAGS += -DDRM_BUILD_PROFILE=10
DRM_CFLAGS += -DDRM_SUPPORT_LOCKING=0
DRM_CFLAGS += -DDRM_SUPPORT_MULTI_THREADING=0
DRM_CFLAGS += -DDRM_SUPPORT_DATASTORE_PREALLOC=1
DRM_CFLAGS += -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1
DRM_CFLAGS += -DDRM_SUPPORT_FORCE_ALIGN=0
DRM_CFLAGS += -DDRM_SUPPORT_ASSEMBLY=1
DRM_CFLAGS += -DDRM_SUPPORT_PRECOMPUTE_GTABLE=0
DRM_CFLAGS += -DDRM_SUPPORT_TRACING=0
DRM_CFLAGS += -DDRM_SUPPORT_PRIVATE_KEY_CACHE=0
DRM_CFLAGS += -DDRM_SUPPORT_EMBEDDED_CERTS=0
DRM_CFLAGS += -DDRM_SUPPORT_DEVICE_SIGNING_KEY=0
DRM_CFLAGS += -DDRM_SUPPORT_NONVAULTSIGNING=1
DRM_CFLAGS += -DDRM_USE_IOCTL_HAL_GET_DEVICE_INFO=0
DRM_CFLAGS += -D_DATASTORE_WRITE_THRU=1
DRM_CFLAGS += -D_ADDLICENSE_WRITE_THRU=0
DRM_CFLAGS += -DDRM_HDS_COPY_BUFFER_SIZE=32768
DRM_CFLAGS += -DDRM_TEST_SUPPORT_NET_IO=0
DRM_CFLAGS += -DUSE_PK_NAMESPACES=0
DRM_CFLAGS += -DDRM_INCLUDE_PK_NAMESPACE_USING_STATEMENT=0
DRM_CFLAGS += -DDRM_KEYFILE_VERSION=3
DRM_CFLAGS += -DDBG=1
DRM_CFLAGS += -DPK_TEST=1
DRM_CFLAGS += -D_CRT_SECURE_NO_DEPRECATE
DRM_CFLAGS += -D_CRT_OBSOLETE_NO_DEPRECATE
DRM_CFLAGS += -D_CRT_SECURE_FORCE_DEPRECATE
DRM_CFLAGS += -D_X86_
#DRM_CFLAGS += -D_M_IX86
DRM_CFLAGS += -DTARGET_LITTLE_ENDIAN=1
DRM_CFLAGS += $(DRM_MODULE_CFLAGS)

DRM_INC += -I. -I.. 
DRM_INC += -I$(DRM_SOURCE_PATH)/inc -I$(DRM_SOURCE_PATH)/results -I$(DRM_SOURCE_PATH)/certs -I$(DRM_SOURCE_PATH)/xmr
DRM_INC += -I$(DRM_SOURCE_PATH)/ndtinc -I$(DRM_SOURCE_PATH)/keyfile -I$(DRM_SOURCE_PATH)/keyfileformat 
DRM_INC += -I$(DRM_SOURCE_PATH)/oem/common/inc -I$(DRM_SOURCE_PATH)/oem/ansi/inc -I$(DRM_SOURCE_PATH)/oem/ansi/trace
DRM_INC += -I$(DRM_SOURCE_PATH)/tools/common
DRM_INC += -I$(DRM_TEST_PATH)/Inc
DRM_INC += -I$(DRM_TEST_PATH)/tstutils/utils
DRM_INC += -I$(DRM_TEST_PATH)/tOEMImp/win32/inc
DRM_INC += -I$(DRM_TEST_PATH)/common
DRM_INC += -I$(DRM_SOURCE_PATH)/initiators

DRM_LDFLAGS +=

ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
BINDIR      = $(DRM_ROOT_PATH)/output/bin/playready_robust
LIBDIR      = $(DRM_ROOT_PATH)/output/lib/playready_robust
else
BINDIR	    = $(DRM_ROOT_PATH)/output/bin/playready
LIBDIR      = $(DRM_ROOT_PATH)/output/lib/playready
endif

DRM_TOOLS_DIR      = Tools
DRM_TEST_DIR       = test

DRM_CFLAGS += -g -fshort-wchar -O3 -fpic

ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
CFLAGS += -DPRDY_ROBUSTNESS=1
endif

# GGM: added to allow -shared option. There are references to libsettop.so
LNKFLAGS		   = -static

TOOLS_INCLUDE	   = -I$(DRM_ROOT_PATH)/tools/common
CLIB             = $(CC)

AR_FLAGS = rcv

CFLAGS += $(DRM_CFLAGS) $(DRM_INC)
LDFLAGS += $(DRM_LDFLAGS)

SYSLIBS		   = -lc
OBJDIR		   = .
OBJOUT		   = -o

OBJEXT		 = o
LIBEXT		 = a

OBJLIST = $(SOURCES:.c=.o)


ifeq ($(TARGETTYPE),PROGRAM)
TARGETEXE = $(TARGETNAME)
else
ifeq ($(TARGETTYPE),LIBRARY)
TARGETLIB = lib$(TARGETNAME).$(LIBEXT)
endif
endif

ifeq ($(wildcard ./dirs),./dirs) # dirs 파일이 있다면
ifneq ($(DRM_DIRS),) # DRM_DIRS가 있다면
BUILDDIRS = $(DRM_DIRS:%=build-%)
CLEANDIRS = $(DRM_DIRS:%=clean-%)
else
BUILDDIRS = $(DIRS:%=build-%)
CLEANDIRS = $(DIRS:%=clean-%)
endif
else
BUILDDIRS =
CLEANDIRS =
endif

SYSLIBS = $(wildcard $(LIBDIR)/*)

all: $(BUILDDIRS) $(TARGETLIB) $(TARGETEXE)
#	@echo DRM_DIRS = $(DRM_DIRS)
#	@echo DIRS = $(DIRS)
#	@echo BUILDDIRS = $(BUILDDIRS)
#	@echo SOURCES = $(SOURCES)
#	@echo OBJLIST = $(OBJLIST)

clean: $(CLEANDIRS)
ifneq ($(TARGETLIB),)
	$(RM) $(OBJLIST) $(OBJLIST:%.o=%.d)
	$(RM) $(TARGETLIB)
endif
ifneq ($(TARGETEXE),)
	$(RM) $(OBJLIST) $(OBJLIST:%.o=%.d)
	$(RM) $(TARGETEXE)
endif

distclean : clean
	$(RM) $(BINDIR)/*
	$(RM) $(LIBDIR)/*

$(TARGETEXE): $(OBJLIST)
	$(CC) $(LDFLAGS) $^ -Wl,--allow-multiple-definition -Wl,--start-group $(SYSLIBS) -Wl,--end-group -o $@
	$(CP) $@ $(BINDIR)/$@
	
%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MD -MF $(@:%.o=%.d) -c -o $@ $<

%.a: $(OBJLIST)
	$(AR) $(AR_FLAGS) $@ $(OBJLIST)
	$(CP) $@ $(LIBDIR)

%.so: $(OBJLIST)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJLIST)
	$(CP) $@ $(LIBDIR)

$(BUILDDIRS):
	@echo "Building directory [$(@:build-%=%)]"
	$(MAKE) -C $(@:build-%=%)

$(CLEANDIRS):
	@echo "Cleaning directory [$(@:clean-%=%)]"
	$(MAKE) -C $(@:clean-%=%) clean
	
-include $(OBJLIST:.o=.d)

.PHONY: subdirs $(DIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all clean

