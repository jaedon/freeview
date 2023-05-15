V =

ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif
export E Q

BUILD_OUTPUT=hwtest
BUILD_LIB=libapp.a

OUTPUT=$(BUILD_LIB)

CUR_BUILD_DIR = $(shell pwd)
SOURCE_DIR = $(CUR_BUILD_DIR)/../src
DRIVER_TOP ?= $(DI_DIR)
DRIVER_LIB ?= $(STATIC_LIB_DIR)
USERNAME ?= $(shell whoami)

ifeq ($(DEBUG),y)
ODIR = DEBUG
else
ODIR = RELEASE
endif

defines-y += NUM_OF_FRONTEND_CHANNEL=$(CHANNEL_NUM)



ifneq ($(subst SAT,,$(MODULE_NAME)), $(MODULE_NAME))
defines-y += FRONTEND_SAT
endif

ifneq ($(subst CAB,,$(MODULE_NAME)), $(MODULE_NAME))
defines-y += FRONTEND_CAB
endif

ifneq ($(subst TER,,$(MODULE_NAME)), $(MODULE_NAME))
defines-y += FRONTEND_TER
endif

include Makefile.inc
include Makefile.obj

################     Compiler Setting
CROSS	?= mipsel-linux-uclibc-
CC	= $(CROSS)gcc
AR	= $(CROSS)ar
AS	= $(CROSS)as
NM	= $(CROSS)nm
STRIP	= $(CROSS)strip
CPP	= $(CC) -E

incdir-y += $(DI_INC_DIR)

CFLAGS	= $(addprefix -I,$(incdir-y)) $(addprefix -D,$(defines-y)) $(DI_CFLAG) $(EXTRA_CFLAGS) $(HUMAX_DEFINES)

ifeq ($(DEBUG),y)
#CFLAGS +=  -g -O -W -Wall -Wextra -Wconversion -Wshadow -Wcast-qual  -Wno-div-by-zero -Wuninitialized
CFLAGS +=  -g -O -W -Wall -Wextra -Wshadow -Wcast-qual  -Wno-div-by-zero -Wuninitialized

else
CFLAGS +=  -O2 -fdata-sections -ffunction-sections
endif

CFLAGS += -DCI_VK_TASK_GETCURRENTID_WORKAROUND -DVK_Print=printf -DPRODUCT_NAME_A=\"$(PRODUCT_NAME)\"
LDFLAGS := -lpthread  -L/$(STATIC_LIB_DIR) -ldriver -L/$(NEXUS_LIB_DIR) -lnexus 


################	Implicit Rule
$(ODIR):
	$(Q) test -d $(ODIR) || install -d -m 755 $(ODIR)

$(ODIR)/%.o:%.c
	$(Q) $(CC) $(CFLAGS) -c -o $@ $<
	@echo "DRIVER_TOP : $(DRIVER_TOP)"
	
################	Build Rules
$(OUTPUT) : $(ODIR) $(objs-y)
	$(Q) echo ______________________________________
	$(Q) echo
	$(Q) echo Command Test Program Compile 
	$(Q) echo ______________________________________
	$(Q) echo
	
	$(Q) $(AR) rcs $(BUILD_LIB) $(objs-y)

#	$(Q) $(CC) -o $@ $(objs-y) $(LDFLAGS)

#	$(Q) $(NM) $(BUILD_OUTPUT) | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > System.map

################	Compile Rules
hwtest: $(OUTPUT) install

################	Install Rules
install:
	$(Q)cp $(BUILD_LIB) $(STATIC_LIB_DIR)
	echo "LDFLAGS = $(LDFLAGS)" > $(STATIC_LIB_DIR)/app.ld
	$(Q) echo MODULE_NAME $(MODULE_NAME)
	$(Q) echo DI_MODULE $(DI_MODULE)
	$(Q) echo DI_CFLAG $(DI_CFLAG)

#	$(Q)cp $(OUTPUT) $(TARGET_DIR)/usr/bin

################	Clean Rules
clean:
	$(E) "  CLEAN   "
	$(Q) $(RM) lastcompile.log System.map
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
