HUMAX_VERBOSE := @
#HUMAX_VERBOSE :=

HUMAX_CROSS := $(CONFIG_TOOLCHAIN_PATH)/bin/mipsel-linux-uclibc-
HUMAX_ECHO := @echo
HUMAX_MAKE := $(HUMAX_VERBOSE)make --no-print-directory
HUMAX_CC := $(HUMAX_VERBOSE)$(HUMAX_CROSS)gcc
HUMAX_CXX := $(HUMAX_VERBOSE)$(HUMAX_CROSS)g++
HUMAX_AR := $(HUMAX_VERBOSE)$(HUMAX_CROSS)ar
HUMAX_LD := $(HUMAX_VERBOSE)$(HUMAX_CROSS)ld
HUMAX_STRIP := $(HUMAX_VERBOSE)$(HUMAX_CROSS)strip
HUMAX_SED := sed
HUMAX_CP := cp
HUMAX_RM := rm -rf
HUMAX_MV := mv
HUMAX_LN := $(HUMAX_VERBOSE)ln
HUMAX_MKDIR := $(HUMAX_VERBOSE)mkdir -p
HUMAX_NM := $(HUMAX_VERBOSE)$(HUMAX_CROSS)nm

ifeq ($(CONFIG_BUILD_DEBUG),y)
	HUMAX_OPT_LEVEL = -O0
	HUMAX_DBG_SYMBOL = -ggdb3
else
	HUMAX_OPT_LEVEL = -O2
	HUMAX_DBG_SYMBOL = 
endif

ifeq ($(CONFIG_BUILD_DEBUG),y)
HUMAX_WRN_LEVEL = -Wall -Wno-unused-value -Wno-unused-function -Wstrict-prototypes -Wno-strict-aliasing -Wdeclaration-after-statement -Wno-pointer-sign
else
HUMAX_WRN_LEVEL = -Wall -Wno-unused-value -Wno-unused-function -Wstrict-prototypes -Wno-strict-aliasing -Wdeclaration-after-statement -Wno-pointer-sign
endif

#
# include os_flags.inc
#
HUMAX_DEFAULT_CFLAG := -DLINUX -pipe -D_FILE_OFFSET_BITS=64
HUMAX_DEFAULT_CFLAG += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -mips32
HUMAX_DEFAULT_CFLAG += -DUCLIBC_VERSION=\"libuClibc-0.9.29.so\"
HUMAX_DEFAULT_CFLAG += -fdata-sections -ffunction-sections
HUMAX_DEFAULT_CFLAG += -fno-strict-aliasing
#### below option is needed by NAGRA requirement. it need -lssp link option.
HUMAX_DEFAULT_CFLAG += -fstack-protector-all
# -std=gnu99

HUMAX_CFLAG := $(HUMAX_DEFAULT_CFLAG)
HUMAX_CFLAG += $(HUMAX_OPT_LEVEL) $(HUMAX_DBG_SYMBOL) $(HUMAX_WRN_LEVEL)
HUMAX_CFLAG += $(ccflags-y)
HUMAX_CFLAG += $(HUMAX_EXTRA_CFLAG)
HUMAX_CFLAG += $(PACKAGE_INC_DIR-y)

ifeq ($(HUMAX_VERBOSE),@)
HUMAX_AR_FLAGS := rc
else
HUMAX_AR_FLAGS := rcv
endif

HUMAX_LD_FLAGS := $(HUMAX_LINK_LIST)
