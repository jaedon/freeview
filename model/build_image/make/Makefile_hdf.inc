#
# Copyright (C) 2015 HUMAX Co., Ltd. All rights reserved.
#

# define base dir path
HUMAX_TOP_DIR         ?= $(shell pwd)/../../..
HUMAX_NFS_TOP_DIR     ?= /nfsroot/$(shell whoami)/$(CONFIG_PRODUCT_NAME)

# inherite dir
BUILD_IMAGE_PATH       = $(HUMAX_TOP_DIR)/model/build_image
MAKE_PATH              = $(BUILD_IMAGE_PATH)/make
APP_MAKE_PATH					 = $(BUILD_IMAGE_PATH)/App
GANG_MAKE_PATH				 = $(BUILD_IMAGE_PATH)/Gang
INPUT_IMAGE_PATH       = $(BUILD_IMAGE_PATH)/Image
OUTPUT_IMAGE_PATH      = $(BUILD_IMAGE_PATH)/Output

# define output file name
SYSTEM_APP_VER_HEAD    = $(shell dd if=$(HUMAX_NFS_TOP_DIR)/image/APP_VERSION.bin bs=1 count=6 2> /dev/null)
SYSTEM_APP_VERSION     = $(shell printf "%d.%02d.%02d" `od -j 6 -N 3 -An -vtu1 $(HUMAX_NFS_TOP_DIR)/image/APP_VERSION.bin`)
CURRENT_DATE_TIME      = $(shell date +'%y%m%d_%H%M')
APP_HDF_NAME           = $(CONFIG_PRODUCT_NAME)_upgrade.$(SYSTEM_APP_VER_HEAD).$(SYSTEM_APP_VERSION).$(CURRENT_DATE_TIME).app.hdf
GANG_HDF_NAME          = $(CONFIG_PRODUCT_NAME)_upgrade.$(SYSTEM_APP_VER_HEAD).$(SYSTEM_APP_VERSION).$(CURRENT_DATE_TIME).gang.hdf

# define tool path
TOOL_PATH              = $(BUILD_IMAGE_PATH)/Util/linux
HUMAX_MAKEHDF          = $(TOOL_PATH)/../makehdf
HUMAX_MG               = $(TOOL_PATH)/mg/mg
HUMAX_MAKENVRAM        = $(TOOL_PATH)/makenvram/makenvram
HUMAX_CRC32            = $(TOOL_PATH)/crc/crc32

# define script path
APP_MAKE_SCRIPT				 = $(BUILD_IMAGE_PATH)/App/make_app.sh
GANG_MAKE_SCRIPT			 = $(BUILD_IMAGE_PATH)/Gang/make_gang.sh

hdf_diagnostics:
	@echo HUMAX_NFS_TOP_DIR = $(HUMAX_NFS_TOP_DIR)
	@echo HUMAX_TOP_DIR = $(HUMAX_TOP_DIR)
	@echo BUILD_IMAGE_PATH = $(BUILD_IMAGE_PATH)
	@echo INPUT_IMAGE_PATH = $(INPUT_IMAGE_PATH)
	@echo OUTPUT_IMAGE_PATH = $(OUTPUT_IMAGE_PATH)
	@echo CONFIG_PRODUCT_NAME = $(CONFIG_PRODUCT_NAME)
	@echo SYSTEM_APP_VER_HEAD = $(SYSTEM_APP_VER_HEAD)
	@echo SYSTEM_APP_VERSION = $(SYSTEM_APP_VERSION)
	@echo CURRENT_DATE_TIME = $(CURRENT_DATE_TIME)
	
$(INPUT_IMAGE_PATH)/root.squa: $(HUMAX_NFS_TOP_DIR)/image/root.squa
	cp $< $@

$(INPUT_IMAGE_PATH)/vmlinuz: $(HUMAX_NFS_TOP_DIR)/image/vmlinuz
	cp $< $@

$(INPUT_IMAGE_PATH)/APP_VERSION.bin: $(HUMAX_NFS_TOP_DIR)/image/APP_VERSION.bin
	cp $< $@

check_input: $(INPUT_IMAGE_PATH)/root.squa $(INPUT_IMAGE_PATH)/vmlinuz $(INPUT_IMAGE_PATH)/APP_VERSION.bin
	mkdir -p $(OUTPUT_IMAGE_PATH)

app_hdf: check_input make_app_hdf

make_app_hdf:
	(cd $(APP_MAKE_PATH) && $(APP_MAKE_SCRIPT) $(CONFIG_PRODUCT_NAME))
	(cp $(APP_MAKE_PATH)/output/* $(OUTPUT_IMAGE_PATH))

gang_hdf: check_input make_gang_hdf

make_gang_hdf:
	(cd $(GANG_MAKE_PATH) && $(GANG_MAKE_SCRIPT) $(CONFIG_PRODUCT_NAME))
	(cp $(GANG_MAKE_PATH)/output/* $(OUTPUT_IMAGE_PATH))
	(cp $(GANG_MAKE_PATH)/ubi_code.crc $(OUTPUT_IMAGE_PATH))

hdf: app_hdf gang_hdf

hdf-distclean:
	rm -f $(INPUT_IMAGE_PATH)/root.squa
	rm -f $(INPUT_IMAGE_PATH)/vmlinuz
	rm -f $(INPUT_IMAGE_PATH)/APP_VERSION.bin
	rm -f $(MAKE_PATH)/*.bin
	rm -f $(MAKE_PATH)/*.crc
