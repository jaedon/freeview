TAR_DIR = $(HUMAX_WORK_DIR)/platform/sdk/tar
KERNEL_TAR = $(shell find $(TAR_DIR) -maxdepth 2 -name stblinux-*)

H_KERNEL_PATCH_DIR = $(HUMAX_WORK_DIR)/platform/sdk/patches/kernel
KERNEL_EXTRACT_DIR = $(HUMAX_WORK_DIR)/platform/sdk/os/kernel-extract

kernel-unpack : $(KERNEL_TAR) $(KERNEL_EXTRACT_DIR)
	@tar xvf $(KERNEL_TAR) -C $(KERNEL_EXTRACT_DIR)
	mv $(KERNEL_EXTRACT_DIR)/linux $(KERNEL_DIR)
	rm -rf $(KERNEL_EXTRACT_DIR)

humax-kernel : $(H_KERNEL_PATCH_DIR)
	cp -a $</* $(KERNEL_DIR)

$(KERNEL_EXTRACT_DIR) :
	@if [ ! -d $@ ]; then\
		mkdir -p $@; \
	fi

$(KERNEL_DIR) : kernel-unpack
	@if [ ! -d $@ ]; then\
		mkdir -p $@; \
	fi
