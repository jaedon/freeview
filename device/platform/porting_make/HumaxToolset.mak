#HUMAX_VERBOSE := @
HUMAX_VERBOSE :=

-include $(HUMAX_MAKE_DIR)/.config
-include $(HUMAX_PLATFORM_CONFIG_DIR)/.config

TOOLCHAIN_VERSION=$(shell echo $(CONFIG_TOOLCHAIN_PATH) | cut -f 2 -d "-")
TOOLCHAIN_MAJOR=$(shell echo $(TOOLCHAIN_VERSION) | cut -f 1 -d ".")
TOOLCHAIN_MINOR=$(shell echo $(TOOLCHAIN_VERSION) | cut -f 2 -d ".")

ifeq ($(CONFIG_ARM), y)
HUMAX_CROSS_COMPILE := arm-linux-
#If toolchain >= 4.8, arm-linux-gnueabihf
#else ,arm-linux-uclibcgnueabi
HUMAX_UCLIBC_OLD_DIR := $(CONFIG_TOOLCHAIN_PATH)/arm-linux-uclibcgnueabi
ifeq ($(shell expr $(TOOLCHAIN_MAJOR) ">" 4),1)
	HUMAX_UCLIBC_OLD_DIR := $(CONFIG_TOOLCHAIN_PATH)/arm-linux-gnueabihf
else
	ifeq ($(shell expr $(TOOLCHAIN_MAJOR) "==" 4),1)
		ifeq ($(shell expr $(TOOLCHAIN_MINOR) ">=" 8),1)
			HUMAX_UCLIBC_OLD_DIR := $(CONFIG_TOOLCHAIN_PATH)/arm-linux-gnueabihf
		endif
	endif
endif	
HUMAX_CROSS := $(CONFIG_TOOLCHAIN_PATH)/bin/arm-linux-
else
ifeq ($(CONFIG_OS_UCOS), y)
	HUMAX_CROSS_COMPILE := sde-
	HUMAX_CROSS := $(CONFIG_TOOLCHAIN_PATH)/bin/sde-
else
	HUMAX_CROSS_COMPILE := mipsel-linux-
    ifeq ($(shell expr $(TOOLCHAIN_MAJOR) "==" 4).$(shell expr $(TOOLCHAIN_MINOR) ">=" 8),1.1)
	    HUMAX_UCLIBC_OLD_DIR := $(CONFIG_TOOLCHAIN_PATH)/mipsel-linux-gnu
	    HUMAX_CROSS := $(CONFIG_TOOLCHAIN_PATH)/bin/mipsel-linux-
    else
	    HUMAX_UCLIBC_OLD_DIR := $(CONFIG_TOOLCHAIN_PATH)/mipsel-linux-uclibc
	    HUMAX_CROSS := $(CONFIG_TOOLCHAIN_PATH)/bin/mipsel-linux-uclibc-
    endif
endif
endif

HUMAX_ECHO := @echo
HUMAX_MAKE := $(HUMAX_VERBOSE)make
HUMAX_CC := $(HUMAX_VERBOSE)$(HUMAX_CROSS)gcc
HUMAX_AR := $(HUMAX_VERBOSE)$(HUMAX_CROSS)ar
HUMAX_LD := $(HUMAX_VERBOSE)$(HUMAX_CROSS)ld
HUMAX_STRIP := $(HUMAX_VERBOSE)$(HUMAX_CROSS)strip
HUMAX_SED := $(HUMAX_VERBOSE)sed
HUMAX_CP := $(HUMAX_VERBOSE)cp
HUMAX_RM := $(HUMAX_VERBOSE)rm -rf
HUMAX_MV := $(HUMAX_VERBOSE)mv
HUMAX_MKDIR := $(HUMAX_VERBOSE)mkdir -p

ifneq ($(CONFIG_CCACHE),)
CCACHE_DIR	= $(HUMAX_MAKE_DIR)/ccache
export CCACHE_DIR
HUMAX_CC := ccache $(HUMAX_CC)
endif

DISTCC = $(wildcard /usr/bin/distcc)
ifneq ($(DISTCC),)
HUMAX_CC := $(DISTCC) $(HUMAX_CC)
endif

HUMAX_MULTI_JOBS := -j8

ifeq ($(HUMAX_MODULE), APPS)
	ifeq ($(CONFIG_PRODUCT_APP_DEBUG_LOG),y)
		HUMAX_OPT_LEVEL = -O0
		HUMAX_DBG_SYMBOL = -g
	else
		HUMAX_OPT_LEVEL = -O2
		HUMAX_DBG_SYMBOL =
	endif
	HUMAX_WRN_LEVEL = -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value -Wdeclaration-after-statement
endif

ifeq ($(HUMAX_MODULE), MAIN)
	ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
		HUMAX_OPT_LEVEL = -O0
		HUMAX_DBG_SYMBOL = -g
	else
		HUMAX_OPT_LEVEL = -O2
		HUMAX_DBG_SYMBOL =
	endif
	HUMAX_WRN_LEVEL = -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value
endif

ifeq ($(HUMAX_MODULE), DRIVER)
	ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
		HUMAX_OPT_LEVEL = -O0
		HUMAX_DBG_SYMBOL = -g
	else
		HUMAX_OPT_LEVEL = -O2
		HUMAX_DBG_SYMBOL =
	endif
	ifeq ($(CONFIG_OS_UCOS),y)
		ifeq ($(CONFIG_ARM), y)
	HUMAX_WRN_LEVEL = -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value -Wno-pointer-sign
		else
			HUMAX_WRN_LEVEL = -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value
		endif
	else
		HUMAX_WRN_LEVEL = -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value -Wno-pointer-sign
	endif
endif

ifeq ($(HUMAX_MODULE), PACKAGE)
	ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y) #todo:need config
		HUMAX_OPT_LEVEL = -O0
		HUMAX_DBG_SYMBOL = -g
	else
		HUMAX_OPT_LEVEL = -O2
		HUMAX_DBG_SYMBOL =
	endif
	HUMAX_WRN_LEVEL = -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value
endif

ifeq ($(CONFIG_DEVELOP),y)
	HUMAX_DBG_SYMBOL += -g
endif

#
# include os_flags.inc
#
ifeq ($(CONFIG_OS_LINUX),y)
HUMAX_DEFAULT_CFLAG := -DLINUX -pipe -D_FILE_OFFSET_BITS=64 -DTOOLCHAIN_MAJOR=$(TOOLCHAIN_MAJOR) -DTOOLCHAIN_MINOR=$(TOOLCHAIN_MINOR)
else
HUMAX_DEFAULT_CFLAG := -DUCOS -pipe -D_FILE_OFFSET_BITS=64
endif
ifeq ($(CONFIG_ARM),y)
HUMAX_DEFAULT_CFLAG += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -marm -march=armv7-a
else
HUMAX_DEFAULT_CFLAG += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -mips32 -EL
endif
#HUMAX_DEFAULT_CFLAG += -DUCLIBC_VERSION=\"libuClibc-0.9.29.so\"  
HUMAX_DEFAULT_CFLAG += -fdata-sections -ffunction-sections -std=gnu99 -fno-strict-aliasing
ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
ifeq ($(CONFIG_OS_LINUX),y)
HUMAX_DEFAULT_CFLAG += -funwind-tables
endif
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_DEBUG),y)
ifeq ($(CONFIG_ASAN),y)
	HUMAX_DEFAULT_CFLAG += -fsanitize=address -fno-omit-frame-pointer
endif
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_OCTO),y)
HUMAX_DEFAULT_CFLAG += -fstack-protector-all
endif

HUMAX_CFLAG := $(HUMAX_DEFAULT_CFLAG) 
HUMAX_CFLAG += $(HUMAX_OPT_LEVEL) $(HUMAX_DBG_SYMBOL) $(HUMAX_WRN_LEVEL)
HUMAX_CFLAG += $(inccflag-y) $(inccflags-y)
HUMAX_CFLAG += $(HUMAX_EXTRA_CFLAG)

ifneq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
HUMAX_CFLAG += -DDI_UART_Print -DDI_UART_BlockablePrint -DVK_DBG_Print
endif

HUMAX_AR_FLAGS := rcv

HUMAX_LD_FLAGS := $(HUMAX_LINK_LIST)
