V =

ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = echo
	Q =
endif
export E Q
ARCH=mips

PROJECTNAME=udfoxhd
BUILD_OUTPUT=humaxtv

OUTPUT=$(BUILD_OUTPUT)

CUR_BUILD_DIR = $(shell pwd)
USERNAME ?= $(shell whoami)

IMAGE_DIR = $(CUR_BUILD_DIR)/../image
EXTERNAL_LIB_DIR = $(CUR_BUILD_DIR)/../LIB

#OUTPUT_DIR = /nfsroot/$(USERNAME)/$(PROJECTNAME)
OUTPUT_DIR = $(CUR_BUILD_DIR)/../output
TOOLS_DIR = $(IMAGE_DIR)/tools

BUILD_USER_DIR = $(OUTPUT_DIR)
TARGET_DIR = $(OUTPUT_DIR)/root

ifeq ($(DEBUG),y)
ODIR = DEBUG
else
ODIR = RELEASE
CONFIG_SUPPORT_NETWORK=y
include $(TOOLS_DIR)/Makefile.in
endif

################     Compiler Setting
TOOLCHAIN_PATH :=/opt/toolchains/stbgcc-4.5.3-1.3

PATH = $(TOOLCHAIN_PATH)/bin:/bin:/sbin:/usr/bin:/usr/sbin

CROSS	?= mipsel-linux-uclibc-
CC	= $(CROSS)gcc
AR	= $(CROSS)ar
AS	= $(CROSS)as
NM	= $(CROSS)nm
STRIP	= $(CROSS)strip
CPP	= $(CC) -E
LD  = $(CROSS)ld

OBJECTS =  $(IMAGE_DIR)/object/$(ODIR)/main.o

LDFLAGS = -L$(IMAGE_DIR)/static_lib -lapp
LDFLAGS += -L/$(TOOLCHAIN_PATH)/mipsel-linux/lib -pthread
LDFLAGS += -L/$(IMAGE_DIR)/lib -ljpeg -lfreetype -lpng -lz -lid3 -lz -lstdc++ -lexif -lblkid -lcom_err -le2p -lss -luuid -lext2fs
LDFLAGS += -L/$(IMAGE_DIR)/static_lib -lant -ldsmcc  -lmxkernel -lmxfile -licplay -lmxplayer -licav -lmxmedia -lmxres -lmxfile -lmxkernel -lmxdlna_mscp -lmxdlna_dms -lmxdbc -lmxsqlite -lmxdlna_core -lmxdtcp -lFLAC
LDFLAGS += -L/$(IMAGE_DIR)/static_lib -ldriver -lnexus -lrt

## external library
ifeq ($(DEBUG),y)
EXTERNAL_LDFLAGS = -L/$(EXTERNAL_LIB_DIR) -lcak6_3_dbg -lop_aps-hd_dbg
else
EXTERNAL_LDFLAGS = -L/$(EXTERNAL_LIB_DIR) -lcak6_3_rel -lop_aps-hd_rel
endif

################	Build Rules
$(BUILD_OUTPUT) :
	$(Q) echo ______________________________________
	$(Q) echo
	$(Q) echo Application Program Link 
ifeq ($(DEBUG),y)
	$(Q) echo DEBUG mode
else
	$(Q) echo RELEASE mode
endif
	$(Q) echo ______________________________________
	$(Q) echo
	$(E) $(LDFLAGS)
	$(E) $(EXTERNAL_LDFLAGS)

	$(Q) $(CC) -o $@ $(OBJECTS) \
		   -L$(STATIC_LIB_DIR) -lapp $(LDFLAGS) \
		   $(EXTERNAL_LDFLAGS)

	$(Q) echo ______________________________________
	$(Q) echo
	$(Q) echo size --- name - not stripped
	$(Q) du -h $(BUILD_OUTPUT)
	$(Q) echo
	$(Q) file $(@)
	$(Q) echo 
	$(Q) echo ______________________________________

ifneq ($(DEBUG),y)
	$(Q) $(STRIP) $@
	$(Q) echo
	$(Q) echo size --- name - stripped
	$(Q) du -h humaxtv
	$(Q) echo
	$(Q) file $(@)
	$(Q) echo
	$(Q) echo ______________________________________
endif

################	Compile Rules
################	Install Rules


image:
	@test -d $(OUTPUT_DIR)/root || install -d -m 755 $(OUTPUT_DIR)/root
	@test -d $(OUTPUT_DIR)/image || install -d -m 755 $(OUTPUT_DIR)/image
ifeq ($(DEBUG),y)
	@tar -xvzf $(IMAGE_DIR)/debug/rootfs.tar.gz -C $(OUTPUT_DIR)
	@cp $(IMAGE_DIR)/debug/vmlinuz $(OUTPUT_DIR)/image
else
	@tar -xvzf $(IMAGE_DIR)/release/rootfs.tar.gz -C $(OUTPUT_DIR)
	@cp $(IMAGE_DIR)/release/vmlinuz $(OUTPUT_DIR)/image
endif
	@cp -dpR $(IMAGE_DIR)/lib/* $(OUTPUT_DIR)/root/usr/lib
	@cp $(IMAGE_DIR)/share/share.squa $(OUTPUT_DIR)/image
	@sudo chown -R $(USERNAME).root $(OUTPUT_DIR)/image/share.squa
	@cp $(BUILD_OUTPUT) $(OUTPUT_DIR)/root/usr/bin
	
	@sudo cp $(TOOLS_DIR)/target_device/device_table.txt $(OUTPUT_DIR)/root/dev/device_table.txt
	$(TOOLS_DIR)/target_device/gendev.sh $(OUTPUT_DIR)/root
ifeq ($(DEBUG),y)
install: $(BUILD_OUTPUT) image
else
install: $(BUILD_OUTPUT) image squashfs-tools
endif
world: install

################	Clean Rules
clean:
	$(E) "  CLEAN   "
	$(Q) $(RM)  System.map
	$(Q) $(RM) $(OUTPUT)
	$(Q) sudo $(RM) -rf $(OUTPUT_DIR)/image $(OUTPUT_DIR)/root

clean_all: squashfs-tools-clean clean

distclean: clean_all

.PHONY: clean clean_all distclean install world

