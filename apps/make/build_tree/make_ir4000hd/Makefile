
TARGET_MODEL_NAME := ir4000hd

TARGET_TFT_FOLDER := /tftpboot
TARGET_NFSROOT_FOLDER := /nfsroot/$(USER)/$(TARGET_MODEL_NAME)
TARGET_NFSROOT_ROOT_FOLDER := $(TARGET_NFSROOT_FOLDER)/root
TARGET_NFSROOT_KERNEL_FOLDER := $(TARGET_NFSROOT_FOLDER)/image
HUMAX_GCSECTION := -Wl,--gc-sections 
CFLAGS := -fno-stack-protector

CONFIG_DEBUG :=
#CONFIG_RELEASE :=

include	Makefile.common
ifeq ($(CONFIG_RELEASE),y)
include Makefile.squashfs
endif

obama: 
	@echo ------
	@echo $@
	@echo ------
	$(HUMAX_CC) $(CFLAGS) $(HUMAX_FLAG) $(HUMAX_GCSECTION) $(STATICLIB_OBJ)/main.o -Wl,--start-group $(LDPATHFLAGS) $(LDFLAGS) -Wl,--end-group -o $@ 
ifeq ($(CONFIG_RELEASE),y)
	$(HUMAX_STRIP) --strip-all $@
endif	
install:
	@echo ------
	@echo $@
	@echo ------	
	@sudo rm  $(TARGET_NFSROOT_FOLDER)/root/usr/bin/obama
	@sudo cp obama  $(TARGET_NFSROOT_FOLDER)/root/usr/bin/obama
	@sudo cp $(CURDIR)/static_libs/libacs_va.a $(TARGET_NFSROOT_ROOT_FOLDER)/usr/lib/	
	
extract:
	@echo ------
	@echo $@
	@echo ------
	@test -d $(TARGET_NFSROOT_ROOT_FOLDER) || install -d -m 755 $(TARGET_NFSROOT_ROOT_FOLDER)
	@test -d $(TARGET_NFSROOT_FOLDER)/image || install -d -m 755 $(TARGET_NFSROOT_FOLDER)/image 
	@sudo tar xvfz $(CURDIR)/rootfs/rootfs.tar.gz  -C $(TARGET_NFSROOT_FOLDER)/
	@sudo mv $(TARGET_NFSROOT_FOLDER)/nfsroot/hjlee3/ir4000hd/root $(TARGET_NFSROOT_FOLDER)/
	@sudo rm -rf $(TARGET_NFSROOT_FOLDER)/nfsroot
	@sudo cp $(CURDIR)/kernel/vmlinuz $(TARGET_NFSROOT_KERNEL_FOLDER)/
	@sudo chown -R $(USER) $(TARGET_NFSROOT_ROOT_FOLDER)
	@sudo chown -R root:root $(TARGET_NFSROOT_ROOT_FOLDER)/dev/*
	@sudo chown -R $(USER) $(TARGET_NFSROOT_ROOT_FOLDER)/usr
	@sudo find $(TARGET_NFSROOT_ROOT_FOLDER) -type f -exec sudo chmod 600 {} \;
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/lib/ld-*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/lib/libpthread*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/etc/init.d/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/usr/sbin/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/usr/bin/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/sbin/*
	@sudo chmod 700 $(TARGET_NFSROOT_ROOT_FOLDER)/bin/*
	@sudo cp $(CURDIR)/kernel/vmlinuz $(TARGET_TFT_FOLDER)/vmlinuz-$(USER)-$(TARGET_MODEL_NAME)

	
clean:
	rm -rf obama obama_*

distclean: clean
	rm -rf $(CURDIR)/include
	rm -rf $(CURDIR)/kernel
	rm -rf $(CURDIR)/rootfs
	rm -rf $(CURDIR)/static_libs
	rm -rf $(CURDIR)/__this_is_*__
	rm -rf $(TARGET_NFSROOT_KERNEL_FOLDER)/*

# extract obama install app_squashfs
ifeq ($(CONFIG_RELEASE),y)
world: extract obama install app_squashfs FORCE
	@echo ------
	@echo $@ release
	@echo ------

else
world: extract obama install FORCE
	@echo ------
	@echo $@ debug
	@echo ------
endif

app_squashfs: squashfs-tools
	@echo app_squashfs!!



FORCE:
