############# for Kernel Tools #############
KERNEL_TOOLS_DIR=$(KERNEL_DIR)/tools
define kernel-tools
	$(HUMAX_MAKE) -C $(KERNEL_TOOLS_DIR)/$(1) $(2) $(3)
endef

ifeq ($(CONFIG_ARM), y)
TOOLS_CONFIGS ?= ARCH=arm
else
TOOLS_CONFIGS ?= ARCH=mips
endif
TOOLS_CONFIGS += CROSS_COMPILE=$(HUMAX_CROSS)

################## PERF ###################
GETTEXT_PATH = $(HUMAX_PACKAGE_MAKE_DIR)gettext
GETTEXT_LIB_PATH = $(GETTEXT_PATH)/lib
GETTEXT_INCLUDE_PATH = $(GETTEXT_PATH)/include
GETTEXT_LIBS = -lintl

ELFUTILS_PATH = $(HUMAX_PACKAGE_MAKE_DIR)elfutils/usr/local
ELFUTILS_LIB_PATH = $(ELFUTILS_PATH)/lib
ELFUTILS_INCLUDE_PATH = $(ELFUTILS_PATH)/include
ELFUTILS_LIBS = -lelf

PERF_CFLAGS ?= -DHUMAX_PLATFORM_BASE -I$(GETTEXT_INCLUDE_PATH) -I$(ELFUTILS_INCLUDE_PATH)
PERF_LDFLAGS ?= -L$(GETTEXT_LIB_PATH) -L$(ELFUTILS_LIB_PATH) $(GETTEXT_LIBS) $(ELFUTILS_LIBS)

PERF_CONFIGS ?= EXTRA_CFLAGS="$(PERF_CFLAGS)" LDFLAGS="$(PERF_LDFLAGS) $(PERF_LIBS)"

perf-check-libs:
	@if [ ! -d $(GETTEXT_PATH) ]; then \
		echo "Need to download gettext library !!!"; \
		/bin/false; \
	fi;
	@if [ ! -d $(ELFUTILS_PATH) ]; then \
		echo "Need to download elfutils library !!!"; \
		/bin/false; \
	fi;
	@if [ ! -f $(KERNEL_DIR)/.config ]; then \
		echo "Need to compile kernel !!!"; \
		/bin/false; \
	else \
		if (! cat $(KERNEL_DIR)/.config | grep CONFIG_PERF_EVENTS=y ); then \
			echo "Need to enable CONFIG_PERF_EVENT in kernel config file !!!"; \
			/bin/false; \
		fi; \
	fi; 

perf-install: perf
	@echo " [ Copy perf / libelf / libintl ] "
	@cp $(KERNEL_TOOLS_DIR)/perf/perf $(HUMAX_NFS_INSTALL_DIR)/usr/bin/
	@cp -drp $(GETTEXT_LIB_PATH)/*intl*.so* $(HUMAX_NFS_INSTALL_DIR)/usr/lib/
	@cp -drp $(ELFUTILS_LIB_PATH)/*elf*.so* $(HUMAX_NFS_INSTALL_DIR)/usr/lib/
	@echo " [ Install Done ] "	

perf: perf-check-libs
	$(call kernel-tools,perf,$(TOOLS_CONFIGS) $(PERF_CONFIGS),)
perf-clean:
	$(call kernel-tools,perf,$(TOOLS_CONFIGS) $(PERF_CONFIGS),clean)
perf-distclean:
	$(call kernel-tools,perf,$(TOOLS_CONFIGS) $(PERF_CONFIGS),distclean)

################# PERF ####################
