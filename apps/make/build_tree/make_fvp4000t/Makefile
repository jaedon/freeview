
TARGET_MODEL_NAME := fvp4000t_buildtree

TARGET_TFT_FOLDER := /tftpboot
TARGET_NFSROOT_FOLDER := /nfsroot/$(USER)/$(TARGET_MODEL_NAME)
TARGET_NFSROOT_ROOT_FOLDER := $(TARGET_NFSROOT_FOLDER)/root

include	Makefile.common

mheg:
	@echo ------
	@echo $@
	@echo ------
	$(HUMAX_CC) -Wl,--start-group $(LDPATHFLAGS) $(LDFLAGS_FOR_MHEG) -Wl,--end-group -o $@
	
install:
	@echo ------
	@echo $@
	@echo ------	
	cp mheg  $(TARGET_NFSROOT_FOLDER)/root/home/mheg/mheg
	
extract:
	@echo ------
	@echo $@
	@echo ------
	@test -d $(TARGET_NFSROOT_ROOT_FOLDER) || install -d -m 755 $(TARGET_NFSROOT_ROOT_FOLDER)
	@test -d $(TARGET_NFSROOT_FOLDER)/image || install -d -m 755 $(TARGET_NFSROOT_FOLDER)/image 
	@sudo tar -xvf $(CURDIR)/rootfs/rootfs.tar.gz -C $(TARGET_NFSROOT_FOLDER)/
	@sudo chown -R $(USER) $(TARGET_NFSROOT_ROOT_FOLDER)
	@sudo chown -R root:root $(TARGET_NFSROOT_ROOT_FOLDER)/dev/*
	@sudo chown -R $(USER) $(TARGET_NFSROOT_ROOT_FOLDER)/usr
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/lib/ld-*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/lib/libpthread*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/etc/init.d/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/usr/sbin/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/usr/bin/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/sbin/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/bin/*
	@cp $(CURDIR)/kernel/vmlinuz $(TARGET_TFT_FOLDER)/vmlinuz-$(USER)-$(TARGET_MODEL_NAME)

clean:
	rm -rf mheg mheg_*

distclean: clean
	rm -rf $(CURDIR)/include
	rm -rf $(CURDIR)/kernel
	rm -rf $(CURDIR)/rootfs
	rm -rf $(CURDIR)/static_libs
	rm -rf $(CURDIR)/__this_is_*__

world: extract mheg install FORCE
	@echo ------
	@echo $@
	@echo ------

FORCE:
