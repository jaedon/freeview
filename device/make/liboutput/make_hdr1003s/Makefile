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

PROJECTNAME=hdr1003s
BUILD_OUTPUT=humaxtv
TAR_DEBUG=$(PROJECTNAME)_build_tree_debug_`date +%F`.tgz
TAR_RELEASE=$(PROJECTNAME)_build_tree_release_`date +%F`.tgz

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
include Makefile.squashfs
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
LDFLAGS += -L/$(IMAGE_DIR)/lib -ljpeg -lfreetype -lpng -lz -lid3 -lz -lstdc++ -lexif -lblkid -lcom_err -le2p -lss -luuid -lext2fs -lssl -lcrypto -lcurl
LDFLAGS += -L/$(IMAGE_DIR)/browser/lib -lopera -looif -lopera_hbbtv -lplugin_sysinfo -lplugin_upgrade -lplugin_systeminfoobject -lplugin_ipconfig -lplugin_netstate -lplugin_lanconfigobject -lplugin_wlanconfigobject -lwebtv
LDFLAGS += -L/$(IMAGE_DIR)/static_lib -lmxkernel -lmxfile -licplay -lmxplayer -licav -lmxmedia -lmxres -lmxfile -lmxkernel -lmxdlna_mscp -lmxdlna_dms -lmxdbc -lmxsqlite -lmxdlna_core -lmxdtcp -lFLAC
LDFLAGS += -L/$(IMAGE_DIR)/static_lib -lvkernel -lnexus -lrt
ifeq ($(DEBUG),y)
LDFLAGS += -L$(TARGET_DIR)/lib -lssp
LDFLAGS += -L/$(IMAGE_DIR)/debug -ldriver
else
LDFLAGS += -L/$(IMAGE_DIR)/release -ldriver
endif

LDFLAGS += -ls3_dvb_client_3_13_0
LDFLAGS += -lredkey
LDFLAGS += $(IMAGE_DIR)/static_lib/license.lo

################	Build Rules
$(BUILD_OUTPUT) : FORCE
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

	@cp $(BUILD_OUTPUT) $(OUTPUT_DIR)/root/usr/bin
################	Compile Rules
################	Install Rules


image:
	@test -d $(OUTPUT_DIR)/root || install -d -m 755 $(OUTPUT_DIR)/root
	@test -d $(OUTPUT_DIR)/image || install -d -m 755 $(OUTPUT_DIR)/image
ifeq ($(DEBUG),y)
	@tar -xvzf $(IMAGE_DIR)/debug/rootfs.tar.gz --exclude=dev -C $(OUTPUT_DIR)
	@tar -xvzf $(IMAGE_DIR)/debug/libs.tar.gz -C $(OUTPUT_DIR)
	@test -d $(OUTPUT_DIR)/root/dev || mkdir $(OUTPUT_DIR)/root/dev
	@cp $(IMAGE_DIR)/debug/vmlinuz-$(PROJECTNAME) $(OUTPUT_DIR)/image/vmlinuz
else
	@tar -xvzf $(IMAGE_DIR)/release/rootfs.tar.gz --exclude=dev -C $(OUTPUT_DIR)
	@tar -xvzf $(IMAGE_DIR)/release/libs.tar.gz -C $(OUTPUT_DIR)
	@test -d $(OUTPUT_DIR)/root/dev || mkdir $(OUTPUT_DIR)/root/dev
	@cp $(IMAGE_DIR)/release/vmlinuz-$(PROJECTNAME) $(OUTPUT_DIR)/image/vmlinuz
endif
	@cp -dpR $(IMAGE_DIR)/lib/* $(OUTPUT_DIR)/root/usr/lib
	@cp -dpR $(IMAGE_DIR)/share/* $(OUTPUT_DIR)/root/opt/share

	@echo ______________________________________
	@echo
	@echo Each model specific Opera browser runtime environment copy

	@rm -rf $(OUTPUT_DIR)/root/usr/browser
	@mkdir -p $(OUTPUT_DIR)/root/usr/browser
	@mkdir -p $(OUTPUT_DIR)/root/usr/browser/lib

	@cp -a $(IMAGE_DIR)/browser/lib/*.so* $(OUTPUT_DIR)/root/usr/browser/lib/
	@cp -a $(IMAGE_DIR)/browser/opera_dir $(OUTPUT_DIR)/root/usr/browser/
	@cp -a $(IMAGE_DIR)/browser/opera_home $(OUTPUT_DIR)/root/usr/browser/

	@ln -fs /usr/browser/lib/libopera_hbbtv.so $(BUILD_USER_DIR)/root/usr/lib/libopera_hbbtv.so
	@ln -fs /usr/browser/lib/libooif.so $(BUILD_USER_DIR)/root/usr/lib/libooif.so
	@ln -fs /usr/browser/lib/libplugin_sysinfo.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_sysinfo.so
	@ln -fs /usr/browser/lib/libplugin_upgrade.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_upgrade.so
	@ln -fs /usr/browser/lib/libplugin_systeminfoobject.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_systeminfoobject.so
	@ln -fs /usr/browser/lib/libplugin_ipconfig.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_ipconfig.so
	@ln -fs /usr/browser/lib/libplugin_netstate.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_netstate.so
	@ln -fs /usr/browser/lib/libplugin_lanconfigobject.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_lanconfigobject.so
	@ln -fs /usr/browser/lib/libplugin_wlanconfigobject.so $(BUILD_USER_DIR)/root/usr/lib/libplugin_wlanconfigobject.so

	find $(OUTPUT_DIR)/root/usr/browser -name .svn -type d -print | xargs rm -rf

	@sudo find $(OUTPUT_DIR)/root/etc/init.d -type f -exec sudo chmod 700 {} \;
	@sudo find $(OUTPUT_DIR)/root/lib -type f -exec sudo chmod 700 {} \;
	@sudo find $(OUTPUT_DIR)/root/usr/lib -type f -exec sudo chmod 700 {} \;
	@sudo find $(OUTPUT_DIR)/root/usr/sbin -type f -exec sudo chmod 700 {} \;
	@sudo find $(OUTPUT_DIR)/root/usr/browser/lib -type f -exec sudo chmod 700 {} \;
	@echo ______________________________________
	
	@sudo cp $(TOOLS_DIR)/target_device/device_table.txt $(OUTPUT_DIR)/root/dev/device_table.txt
	$(TOOLS_DIR)/target_device/gendev.sh $(OUTPUT_DIR)/root

ifeq ($(DEBUG),y)
install: image $(BUILD_OUTPUT)
else
install: image $(BUILD_OUTPUT) squashfs-tools
endif
world: install

tar_debug:
	./copy_image.sh
	make world DEBUG=y
	cd ..; tar czf $(TAR_DEBUG) output image LIB make_$(PROJECTNAME)/*
	@echo
	@echo ______________________________________
	@echo
	@echo $(TAR_DEBUG) is created.
	@echo ______________________________________

tar_release:
	./copy_image.sh
	make world
	cd ..; tar czf $(TAR_RELEASE) output image LIB make_$(PROJECTNAME)/*
	@echo
	@echo ______________________________________
	@echo
	@echo $(TAR_RELEASE) is created.
	@echo ______________________________________

################	Clean Rules
clean:
	$(E) "  CLEAN   "
	$(Q) $(RM)  System.map
	$(Q) $(RM) $(OUTPUT)
	$(Q) sudo $(RM) -rf $(OUTPUT_DIR)/image $(OUTPUT_DIR)/root

clean_all: squashfs-tools-clean clean

distclean: clean_all

.PHONY: clean clean_all distclean install world

FORCE :
