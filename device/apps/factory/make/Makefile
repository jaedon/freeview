V =

ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif
export E Q

BUILD_OUTPUT=factory
BUILD_LIB ?= libapp.a

OUTPUT=$(BUILD_LIB)

CUR_BUILD_DIR = $(shell pwd)
SOURCE_DIR = $(CUR_BUILD_DIR)/../src
DRIVER_TOP ?= $(DI_DIR)
DRIVER_LIB ?= $(STATIC_LIB_DIR)
USERNAME ?= $(shell whoami)


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

#ifeq ($(DEBUG),y)
#CFLAGS +=  -g -O -W -Wall -Wextra -Wconversion -Wshadow -Wcast-qual  -Wno-div-by-zero -Wuninitialized
#else
#CFLAGS +=  -O2 -fdata-sections -ffunction-sections
#endif

#CFLAGS += -DCI_VK_TASK_GETCURRENTID_WORKAROUND -DVK_Print=printf

LDFLAGS := -lpthread  -L/$(STATIC_LIB_DIR) -ldriver -L/$(NEXUS_LIB_DIR) -lnexus 

################	Implicit Rule
build : factory_lib install


################	Compile Rules
factory_lib:
	$(MAKE) -C build_rule build BUILD_LIB=$(BUILD_LIB) FACTORY_COMPILE_FLAGS=$(FACTORY_COMPILE_FLAGS)

################	Install Rules
install:
	$(Q)cp ../$(BUILD_LIB) $(STATIC_LIB_DIR)
	echo "LDFLAGS = $(LDFLAGS)" > $(STATIC_LIB_DIR)/app.ld

#	$(Q)cp $(OUTPUT) $(TARGET_DIR)/usr/bin

################	Clean Rules
clean:
	$(E) "  CLEAN   "
	$(Q) $(RM) lastcompile.log System.map
	$(MAKE) -C build_rule clean BUILD_LIB=$(BUILD_LIB)

clean_all: clean

distclean: clean_all
	$(Q) $(RM) -rf $(ODIR)

.PHONY: clean clean_all distclean install

################     Read all saved dependencies
dep_files := $(wildcard $(objs-y:.o=.d))
ifneq ($(dep_files),)
  include $(dep_files)
endif
