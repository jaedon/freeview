#
# This makefile builds a nocs3 driver link library
# It should be included from the makefile in the nocs3 driver directory
# which defines the target name and the objects
#

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

ifndef NEXUS_TOP
$(error NEXUS_TOP is not defined)
endif

ifndef TARGET
$(error TARGET is not defined)
endif

ifndef OBJECTS
$(error OBJECTS is not defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# Object and target directories
export OBJDIR=$(CURDIR)/../obj.$(PLATFORM)/$(shell basename $(CURDIR))
export BINDIR=$(CURDIR)/../obj.$(PLATFORM)/bin

# Determine the endianness of the build
LIB_ENDIAN=_le
ifeq ($(B_REFSW_ARCH),mips-linux)
LIB_ENDIAN=_be
CFLAGS += -DCFG_BIG_ENDIAN=1
else
CFLAGS += -DCFG_BIG_ENDIAN=0
endif


_TARGET = $(BINDIR)/lib$(TARGET)$(LIB_ENDIAN).a
_OBJECTS = $(addprefix $(OBJDIR)/, $(OBJECTS))

PORTINGINTERFACE = $(NEXUS_TOP)/../magnum/portinginterface
NOCS3_ROOT = $(shell cd $(CURDIR)/.. && pwd)
SECURITY_ROOT = $(shell cd $(CURDIR)/../../../../.. && pwd)

# HUMAX_PLATFORM_BASE modify include path based on drr_package
TOMCRYPT_INC ?= $(NEXUS_TOP)/../opensource/libtom/libtomcrypt-1.17/src/headers
TOMMATCH_INC ?= $(NEXUS_TOP)/../opensource/libtom/libtommath-0.42.0
OPENSSL_INC ?= $(NEXUS_TOP)/../BSEAV/lib/openssl/openssl-1.0.0f/97241/usr/local/include

# This is the minimum needed to compile and link with Nexus
# The last three -Is should be customized.
CFLAGS += \
  $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) \
  -I$(NEXUS_TOP)/../BSEAV/lib/netaccel/include \
  -I$(NEXUS_TOP)/modules/security/include \
  -I$(NEXUS_TOP)/modules/security/include/priv  \
  -I$(NEXUS_TOP)/modules/security/src \
  -I$(NEXUS_TOP)/modules/core/include/ \
  -I$(NEXUS_TOP)/modules/core/include/priv \
  -I$(NEXUS_TOP)/base/include \
  -I$(NEXUS_TOP)/base/include/linuxuser \
  -I$(PORTINGINTERFACE)/../basemodules/mem \
  -I$(PORTINGINTERFACE)/../basemodules/int \
  -I$(PORTINGINTERFACE)/hsm/src/core40nm/common \
  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus20/aegis \
  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus20/cust_nagra/aegis \
  -I$(PORTINGINTERFACE)/hsm/src/core40nm/keyladder \
  -I$(PORTINGINTERFACE)/hsm/src/core40nm/otpmsp \
  -I$(PORTINGINTERFACE)/dma/ \
  -I$(NOCS3_ROOT)/csd/include \
  -I$(NOCS3_ROOT)/csd/src \
  -I$(NOCS3_ROOT)/sec/include \
  -I$(NOCS3_ROOT)/cert/include \
  -I$(NOCS3_ROOT)/nocscommon/include \
  -I$(NOCS3_ROOT)/../include \
  -I$(TOMCRYPT_INC) \
  -I$(TOMMATH_INC) \
  -I$(OPENSSL_INC) 

ifneq ($(RPC_TEST_HARNESS_WORKAROUND),)
CFLAGS += -DRPC_TEST_HARNESS_WORKAROUND
endif

ifeq ($(NO_C99),)
# openssl uses "long long", which requires at least c99
CFLAGS += -std=c99
else
CFLAGS += -DNO_C99
endif

# HUMAX_PLATFORM_BASE
#CFLAGS += -Wall -Werror
CFLAGS += -Wall

# required to build SEC
# HUMAX_PLATFORM_BASE
#CFLAGS += -DLTM_DESC
CFLAGS += -DLTM_DESC -DUSE_OSSL_FOR_HMAC

all: $(OBJDIR) $(BINDIR) $(_TARGET)

# Include auto-generated dependencies
-include $(_OBJECTS:.o=.d)

$(OBJDIR) $(BINDIR):
	$(Q_)mkdir -p $@

$(_TARGET): $(_OBJECTS)
	$(Q_)echo "[Link... $(shell basename $@) (nocs3)]"
	$(Q_)ar rcs $(_TARGET) $(_OBJECTS)

# Default C-rule. Also generates dependency files
$(OBJDIR)/%.o: src/%.c
	$(Q_)echo "[Compile... $(shell basename $@) (nocs3)]"
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)
	$(Q_)$(CC) -MM -c $< $(CFLAGS) >$(OBJDIR)/$*.d.tmp
	$(Q_)sed "s|$(shell basename $@):|$@:|" <$(OBJDIR)/$*.d.tmp >$(OBJDIR)/$*.d
	$(Q_)rm $(OBJDIR)/$*.d.tmp

clean:
	$(Q_)rm -rf $(OBJDIR) $(_TARGET)

