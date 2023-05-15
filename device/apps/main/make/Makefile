V =

ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = echo
	Q =
endif
export E Q

BUILD_OUTPUT=humaxtv

OUTPUT=$(BUILD_OUTPUT)

CUR_BUILD_DIR = $(shell pwd)
USERNAME ?= $(shell whoami)

SOURCE_DIR = $(CUR_BUILD_DIR)/..
ifeq ($(DEBUG),y)
ODIR = DEBUG
else
ODIR = RELEASE
endif

include Makefile.inc
include Makefile.obj

-include $(TOP_DIR)/make/BUILD/static_lib/app.ld

################     Compiler Setting
CROSS	?= mipsel-linux-uclibc-
CC	= $(CROSS)gcc
AR	= $(CROSS)ar
AS	= $(CROSS)as
NM	= $(CROSS)nm
STRIP	= $(CROSS)strip
CPP	= $(CC) -E
LD  = $(CROSS)ld

incdir-y += $(DI_INC_DIR)

CFLAGS  = $(addprefix -I,$(incdir-y)) $(addprefix -D,$(defines-y)) 

ifeq ($(DEBUG),y)
CFLAGS +=  -g -O -W -Wall -Wextra -Wconversion -Wshadow -Wcast-qual  -Wno-div-by-zero -Wuninitialized

CFLAGS += -DDEBUG
endif
CFLAGS +=  -O2 -fdata-sections -ffunction-sections

ifeq ($(MODULE_NAME),CAB_EURO_DOCSIS_ETHERNET)
LDFLAGS += -lstdc++ -lrt -ldsgcc
endif

#ifeq ($(findstring OPENSSL,$(MODULE_NAME)))
ifneq ($(subst _OPENSSL,,$(MODULE_NAME)), $(MODULE_NAME))
LDFLAGS += -L$(TOP_DIR)/make/lib -lssl -lcrypto
endif

LDFLAGS += -Wl,--gc-sections

################	Implicit Rule
$(ODIR):
	$(Q) test -d $(ODIR) || install -d -m 755 $(ODIR)

$(ODIR)/%.o:%.c
	$(Q) $(CC) $(CFLAGS) -c -o $@ $<

################	Build Rules
$(BUILD_OUTPUT) : $(ODIR) $(objs-y)
	$(Q) echo ______________________________________
	$(Q) echo
	$(Q) echo Application Program Link 
	$(Q) echo ______________________________________
	$(Q) echo
#	$(E) $(LDFLAGS)

	$(Q) $(CC) -o $@ $(objs-y) -L$(STATIC_LIB_DIR) -lapp $(LDFLAGS)

	$(Q) echo ______________________________________
	$(Q) echo
	$(Q) echo size --- name - not stripped
	$(Q) du -h $(BUILD_OUTPUT)
	$(Q) echo
	$(Q) file $(@)
	$(Q) echo 
	$(Q) echo ______________________________________

	$(Q) $(NM) $(BUILD_OUTPUT) | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > System.map

ifneq ($(subst LOG,,$(MODULE_NAME)), $(MODULE_NAME))
else
ifneq ($(DEBUG_IMAGE),y)
	$(Q) $(STRIP) $@
	$(Q) echo
	$(Q) echo size --- name - stripped
	$(Q) du -h humaxtv
	$(Q) echo
	$(Q) file $(@)
	$(Q) echo
	$(Q) echo ______________________________________
endif
endif

################	Compile Rules
################	Install Rules
install: $(BUILD_OUTPUT)
	$(Q)cp $(OUTPUT) $(TARGET_DIR)/usr/bin

################	Clean Rules
clean:
	$(E) "  CLEAN   "
	$(Q) $(RM) lastcompile.log System.map compile.log
	$(Q) $(RM) $(ODIR)/*.o $(ODIR)/*.d
	$(Q) $(RM) $(OUTPUT)

clean_all: clean

distclean: clean_all
	$(Q) $(RM) -rf $(ODIR)

.PHONY: clean clean_all distclean install

################     Read all saved dependencies
dep_files := $(wildcard $(objs-y:.o=.d))
ifneq ($(dep_files),)
  include $(dep_files)
endif
