# Makefile

HOSTARCH := $(shell uname -m | \
		sed -e s/i.86/i386/ \
				-e s/sun4u/sparc64/ \
				-e s/arm.*/arm/ \
				-e s/sall0/arm/ \
				-e s/powerpc/ppc/ \
				-e s/macppc/ppc/)
HOSTOS := $(shell uname -s | tr '[:upper:]' '[:lower:]' | \
		sed -e 's/\(cygwin\).*/cygwin/')
export HOSTARCH HOSTOS

TOPDIR := $(shell if [ "$$PWD" != "" ]; then \
											echo $$PWD; \
									else \
											pwd; \
									fi)
export	TOPDIR
# Load other configuration
CROSS_COMPILE = /opt/toolchains/stbgcc-4.5.3-1.3/bin/mipsel-linux-uclibc-
# define base dir path
HUMAX_MAKE_DIR=$(shell pwd)
HUMAX_WORK_DIR=$(subst /make,,$(HUMAX_MAKE_DIR))
#NEXUS_TOP=$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus
USER=`whoami`
-include $(HUMAX_MAKE_DIR)/.config

PLATFORM=$(shell echo $(CONFIG_PLATFORM) | sed "/^.\"/d")
BCHP_VER=$(shell echo $(CONFIG_CHIP_REV) | sed "/^.\"/d")
BCHP_CHIP=$(shell echo $(CONFIG_CHIP) | sed "/^.\"/d")
#include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# library output dir path
HUMAX_OUTPUT_DIR := $(HUMAX_MAKE_DIR)/output
NEXUS_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib

AS  = $(CROSS_COMPILE)as
LD  = $(CROSS_COMPILE)ld
CC  = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
AR  = $(CROSS_COMPILE)ar
NM  = $(CROSS_COMPILE)nm
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

gccincdir := $(shell $(CC) -print-file-name=include)

DBGFLAGS :=
OPTFLAGS := -O0
RELFLAGS :=
INC :=

CFLAGS += -c -g -Wall -D_GNU_SOURCE -D_REENTRANT -DHUMAX_PLATFORM_BASE
INC			= -I$(gccincdir) -I./
INC		 += -I${TOPDIR}
INC		 += -I${TOPDIR}/device/include/hos
INC		 += -I${NEXUS_LIB_DIR}
INC		 += -I../../../../../include/hos
INC 	 += $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS))

debug:
				@echo MAKE = $(MAKE) 
				@echo HOSTARCH = $(HOSTARCH)
				@echo HOSTOS = $(HOSTOS)
				@echo TOPDIR = $(TOPDIR)
				@echo LINUXKERNEL_INSTALL_DIR = $(LINUXKERNEL_INSTALL_DIR)
				@echo CFLAGS = $(CFLAGS)
				@echo INC = $(INC)
				@echo PLATFORM = $(PLATFORM)
				@echo NEXUS_TOP = $(NEXUS_TOP)
				@echo HUMAX_MAKE_DIR = $(HUMAX_MAKE_DIR)
				@echo HUMAX_WORK_DIR = $(HUMAX_WORK_DIR)
				@echo HUMAX_OUTPUT_DIR = $(HUMAX_OUTPUT_DIR)
				@echo NEXUS_LIB_DIR = $(NEXUS_LIB_DIR)
				@echo NEXUS_APP_INCLUDE_PATHS = $(NEXUS_APP_INCLUDE_PATHS)
				@echo NEXUS_APP_DEFINES = $(NEXUS_APP_DEFINES)
				@echo USER = ${USER}
				
TARGET_CLIENT = ipc_client_test
TARGET_SERVER = ipc_server_test

#SRCS 	 = $(CLIENT_OBJS:.o=.c)
CLIENT_OBJS	 = ipc_client_test.o
SERVER_OBJS	 = ipc_server_test.o
				 
LIBS		=
LIBS	 	+= -lvkernel
LIBS	 	+= -lpthread
LIBS	 	+= -L../../../../../make/prepare/rootfs-package/usr/lib/

all : $(TARGET_SERVER) $(TARGET_CLIENT)
$(TARGET_SERVER): $(SERVER_OBJS)
			$(CC) $(OPTFLAGS) -o $@ $(SERVER_OBJS) $(LIBS)

$(TARGET_CLIENT): $(CLIENT_OBJS)
			$(CC) $(OPTFLAGS) -o $@ $(CLIENT_OBJS) $(LIBS)
					
.c.o: 
			$(CC) $(INC) $(CFLAGS) $< -o $@
					
clean:
			rm -rf $(CLIENT_OBJS) $(TARGET_CLIENT) core
			rm -rf $(SERVER_OBJS) $(TARGET_SERVER) core
					
install:
			@chmod 777 $(TARGET_CLIENT) $(TARGET_SERVER)
			@echo ========================================================
			@cp -av $(TARGET_CLIENT) /nfsroot/${USER}/deogyu/root/usr/sbin
			@cp -av $(TARGET_SERVER) /nfsroot/${USER}/deogyu/root/usr/sbin
			@echo ========== HDDTESTD installed ==========================
