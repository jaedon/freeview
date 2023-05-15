#
# Copyright (C) 2010 HUMAX Co., Ltd. All rights reserved.
#

# Configuration file
APP_CFG = $(CONFIG_DIR)/app.cfg
GANG_CFG = $(CONFIG_DIR)/gang.cfg
GANG_SIGNED_CFG = $(CONFIG_DIR)/enc_sign_kernel_N_rootfs.cfg
GANG_NAND_MAP_CFG = $(CONFIG_DIR)/nand_img.cfg
GANG_UBI_AREA_CFG = $(CONFIG_DIR)/ubiarea.cfg
GANG_UBIFS_AREA_CFG = $(CONFIG_DIR)/ubifsarea.cfg
GANG_UBI_LIST = $(CONFIG_DIR)/ubi.lst
GANG_SPLIT_LIST = $(CONFIG_DIR)/split.lst

# Input/Output Directory
CONFIG_DIR = ./config
INPUT_IMAGE_DIR = ./input
OUTPUT_IMAGE_DIR = ./output

# Application List/Name
APP_HDF = $(CONFIG_PRODUCT_NAME)_upgrade_app.hdf

# Gang Image List/Name
GANG_ECC_TOOL = add_bch4_ecc

GANG_DB_DATA_SRC_DIR = dbdata_source
GANG_DB_BACKUP_SRC_DIR = dbbackup_source
GANG_DB_USER_SRC_DIR = dbuser_source
GANG_DB_DATA = dbdata
GANG_DB_BACKUP = dbbackup
GANG_DB_USER = dbuser

GANG_UBI_CODE = ubi_code.bin
GANG_UBI_RAWFS = ubi_rawfs.bin
GANG_UBI_NVRAM = ubi_nvram.bin
GANG_UBI_NVRAM_MG = ubi_nvram_mg.bin
GANG_UBI_CRC = ubi_code.crc
GANG_UBI_AREA_IMAGE = ubiarea.img
GANG_UBIFS_AREA_IMAGE = ubifsarea.img

GANG_FACTORY_LOADER = loader_f.bin
GANG_TEMP_FACTORY_LOADER = factory.bin
GANG_LOADER_BIN = $(CONFIG_PRODUCT_NAME)_gang_factory_loader.bin
GANG_SIGNED_HDF = $(CONFIG_PRODUCT_NAME)_enc_N_sign_tmp.hdf
GANG_HDF = $(CONFIG_PRODUCT_NAME)_gang.hdf
GANG_BIN = $(CONFIG_PRODUCT_NAME)_gang.bin
GANG_MASTER_IMAGE = $(CONFIG_PRODUCT_NAME)_fullgang

make_hdf:
	@echo Make Application Image
	rm -f $(INPUT_IMAGE_DIR)/vmlinuz.wSign $(INPUT_IMAGE_DIR)/root.squa.wSign
	$(HUMAX_MAKEHDF_DIR)/makehdf $(APP_CFG) $(OUTPUT_IMAGE_DIR)/$(APP_HDF)
	rm -f $(INPUT_IMAGE_DIR)/vmlinuz.wSign $(INPUT_IMAGE_DIR)/root.squa.wSign

make_gang: make_gang_prepare make_gang_master

make_gang_prepare:
	$(call do_cfg_substitutions,$(GANG_UBI_AREA_CFG).in,$(GANG_UBI_AREA_CFG))
	rm -f $(INPUT_IMAGE_DIR)/vmlinuz.wSign $(INPUT_IMAGE_DIR)/root.squa.wSign $(OUTPUT_IMAGE_DIR)/$(GANG_SIGNED_HDF)
	@echo Make Gang Image Binaries
	@echo Unzip DB and NVRAM files
	unzip -o $(INPUT_IMAGE_DIR)/db.bin.zip -d $(INPUT_IMAGE_DIR)
	unzip -o $(INPUT_IMAGE_DIR)/dbbackup.bin.zip -d $(INPUT_IMAGE_DIR)
	unzip -o $(INPUT_IMAGE_DIR)/dbuser.bin.zip -d $(INPUT_IMAGE_DIR)
	unzip -o $(INPUT_IMAGE_DIR)/nvram.bin.zip -d $(INPUT_IMAGE_DIR)
	@echo Create Signed / Encrypted Kernel/Rootfs Binary
	$(HUMAX_MAKEHDF_DIR)/makehdf $(GANG_SIGNED_CFG) $(OUTPUT_IMAGE_DIR)/$(GANG_SIGNED_HDF)
	@echo Make UBI Binary
	$(HUMAX_MG_DIR)/mg $(GANG_UBI_LIST) UBI_CODE $(INPUT_IMAGE_DIR)/$(GANG_UBI_CODE)
	$(HUMAX_MG_DIR)/mg $(GANG_UBI_LIST) UBI_RAWFS $(INPUT_IMAGE_DIR)/$(GANG_UBI_RAWFS)
	$(HUMAX_MG_DIR)/mg $(GANG_UBI_LIST) UBI_NVRAM $(INPUT_IMAGE_DIR)/$(GANG_UBI_NVRAM)
	@echo Split NVRAM Binary
	$(HUMAX_SPLITTER_DIR)/splitter $(GANG_SPLIT_LIST) NVRAMSPL $(INPUT_IMAGE_DIR)/$(GANG_UBI_NVRAM)
	@echo Make CRC of UBI Code Area
	$(HUMAX_CRC_DIR)/crc32 $(INPUT_IMAGE_DIR)/$(GANG_UBI_CODE) $(INPUT_IMAGE_DIR)/$(GANG_UBI_CRC)
	@echo Merge UBI Area
	$(HUMAX_MG_DIR)/mg $(GANG_UBI_LIST) MG_NVRAM $(INPUT_IMAGE_DIR)/$(GANG_UBI_NVRAM_MG)
	$(HUMAX_MG_DIR)/mg $(GANG_UBI_LIST) RF_K_NVR $(OUTPUT_IMAGE_DIR)/$(GANG_BIN)
	@echo Make Gang HDF Image
	$(HUMAX_MAKEHDF_DIR)/makehdf $(GANG_CFG) $(OUTPUT_IMAGE_DIR)/$(GANG_HDF)
	@echo Copy Factory Loader Image
	cp $(INPUT_IMAGE_DIR)/$(GANG_FACTORY_LOADER) $(OUTPUT_IMAGE_DIR)/$(GANG_LOADER_BIN)
	rm -f $(INPUT_IMAGE_DIR)/vmlinuz.wSign $(INPUT_IMAGE_DIR)/root.squa.wSign $(OUTPUT_IMAGE_DIR)/$(GANG_SIGNED_HDF)

make_gang_master:
	rm -rf *.img *.bin *.csv $(GANG_DB_BACKUP) $(GANG_DB_DATA) $(GANG_DB_USER) $(GANG_ECC_TOOL)
	@echo Make Full Gang Image
	@echo Make DB area directories
	mkdir $(GANG_DB_BACKUP) $(GANG_DB_DATA) $(GANG_DB_USER)
	@echo DB contents copy to dbdata, dbbackup, user area without hidden folder.
	rsync -avz --force --exclude=.svn $(INPUT_IMAGE_DIR)/$(GANG_DB_DATA_SRC_DIR)/ $(GANG_DB_DATA)
	rsync -avz --force --exclude=.svn $(INPUT_IMAGE_DIR)/$(GANG_DB_BACKUP_SRC_DIR)/ $(GANG_DB_BACKUP)
	rsync -avz --force --exclude=.svn $(INPUT_IMAGE_DIR)/$(GANG_DB_USER_SRC_DIR)/ $(GANG_DB_USER)
	@echo Make UBIFS
	$(HUMAX_MAKE_FS_UBIFS_DIR)/mkfs.ubifs -m $(CONFIG_NAND_IO_SIZE) -e $(CONFIG_NAND_LEB_SIZE)KiB -c $(CONFIG_UBIFS_DB_DATA_MAX_FS_SIZE) -q -r ./$(GANG_DB_DATA) $(GANG_DB_DATA).img
	$(HUMAX_MAKE_FS_UBIFS_DIR)/mkfs.ubifs -m $(CONFIG_NAND_IO_SIZE) -e $(CONFIG_NAND_LEB_SIZE)KiB -c $(CONFIG_UBIFS_DB_BACKUP_MAX_FS_SIZE) -q -r ./$(GANG_DB_BACKUP) $(GANG_DB_BACKUP).img
	$(HUMAX_MAKE_FS_UBIFS_DIR)/mkfs.ubifs -m $(CONFIG_NAND_IO_SIZE) -e $(CONFIG_NAND_LEB_SIZE)KiB -c $(CONFIG_UBIFS_DB_USER_MAX_FS_SIZE) -q -r ./$(GANG_DB_USER) $(GANG_DB_USER).img
	@echo Copy Temporary Binary
	cp $(OUTPUT_IMAGE_DIR)/$(GANG_BIN) $(GANG_BIN)
	cp $(OUTPUT_IMAGE_DIR)/$(GANG_LOADER_BIN) $(GANG_TEMP_FACTORY_LOADER)
	@echo Compile 4bit ECC tool
	gcc -m32 $(HUMAX_ECC_GEN_DIR)/$(GANG_ECC_TOOL).c -o $(GANG_ECC_TOOL)
	@echo Merge.....
	$(HUMAX_MAKE_FS_UBIFS_DIR)/ubinize -o $(GANG_UBI_AREA_IMAGE) -p $(CONFIG_NAND_PEB_SIZE)KiB -m $(CONFIG_NAND_IO_SIZE) $(GANG_UBI_AREA_CFG)
	$(HUMAX_MAKE_FS_UBIFS_DIR)/ubinize -o $(GANG_UBIFS_AREA_IMAGE) -p $(CONFIG_NAND_PEB_SIZE)KiB -m $(CONFIG_NAND_IO_SIZE) $(GANG_UBIFS_AREA_CFG)
	@echo Make NAND Master Image
	./$(GANG_ECC_TOOL) $(GANG_NAND_MAP_CFG) $(CONFIG_NAND_TOTAL_SIZE) $(GANG_MASTER_IMAGE).bin $(GANG_MASTER_IMAGE).csv
	tar -zcf $(OUTPUT_IMAGE_DIR)/$(GANG_MASTER_IMAGE).tgz $(GANG_MASTER_IMAGE).bin $(GANG_MASTER_IMAGE).csv
	rm -rf *.img *.bin *.csv $(GANG_DB_BACKUP) $(GANG_DB_DATA) $(GANG_DB_USER) $(GANG_ECC_TOOL)

do_cfg_substitutions = \
        echo "/* GENERATED FILE - DO NOT EDIT */" > $2; \
        sed -e 's|@CONFIG_PRODUCT_NAME@|$(CONFIG_PRODUCT_NAME)|g' \
        < $1 >> $2

clean:
	rm -f $(INPUT_IMAGE_DIR)/vmlinuz $(INPUT_IMAGE_DIR)/vmlinuz.wSign $(INPUT_IMAGE_DIR)/root.squa $(INPUT_IMAGE_DIR)/root.squa.wSign $(INPUT_IMAGE_DIR)/*.*
	rm -f $(OUTPUT_IMAGE_DIR)/*.hdf $(OUTPUT_IMAGE_DIR)/*.bin $(OUTPUT_IMAGE_DIR)/*.tgz
	rm -rf *.img *.bin *.csv $(GANG_DB_BACKUP) $(GANG_DB_DATA) $(GANG_DB_USER) $(GANG_ECC_TOOL) $(GANG_UBI_AREA_CFG)

distclean: clean

.PHONY: make_hdf make_gang make_gang_prepare make_gang_master clean distclean

