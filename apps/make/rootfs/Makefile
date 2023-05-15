
-include $(CURDIR)/../.config

HUMAX_APP_STAGGING_TOP_DIR := $(CURDIR)/../_gen_rootfs
HUMAX_APP_STAGGING_ROOT_DIR := $(HUMAX_APP_STAGGING_TOP_DIR)/root

# determine if release_log_defconfig
RELEASE_LOG =
ifeq ($(CONFIG_DEBUG_RELEASE_LOG),y)
	RELEASE_LOG = y
endif # CONFIG_DEBUG_RELEASE_LOG

base-rootfs:
# no CONFIG_USES_NFSROOT is RELEASE BUILD
ifneq ($(CONFIG_USES_NFSROOT),y)
# belows are release rootfs configuration
	@-if [ -d "$(CURDIR)/basefiles-rel" ] ; then \
		sudo cp -drpf $(CURDIR)/basefiles-rel/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
ifeq ($(RELEASE_LOG),y)
	@-if [ -d "$(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/log" ] ; then \
		sudo cp -drpf $(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/log/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
else	# RELEASE_LOG
	@-if [ -d "$(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/rel" ] ; then \
		sudo cp -drpf $(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/rel/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
endif	# RELEASE_LOG
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
	@-if [ -d "$(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/rel" ] ; then \
		sudo cp -drpf $(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/rel/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
ifeq ($(CONFIG_ENABLE_SANDBOXING),y)
	@-if [ -d "$(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/sandbox/rel" ] ; then \
		sudo cp -adrpf $(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/sandbox/rel/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
endif
endif

ifeq ($(CONFIG_DEBUG_AGING_TEST),y)
	@-if [ -d "$(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/aging" ] ; then \
		sudo cp -drpf $(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/aging/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
	@-if [ -d "$(CURDIR)/basefiles-aging" ] ; then \
		sudo cp -drpf $(CURDIR)/basefiles-aging/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
		sudo sed -i 's:/hama:/hama \&:' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop; \
		sudo sed -i 's:/hama &launcher:/hamalauncher:' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop; \
		sudo sed -i 's:/hama \& \&:/hama \&:' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop; \
	fi;
endif

else # CONFIG_USES_NFSROOT

# belows are debug rootfs configuration
	@-if [ -d "$(CURDIR)/basefiles-dbg" ] ; then \
		sudo cp -drpf $(CURDIR)/basefiles-dbg/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
	@-if [ -d "$(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/dbg" ] ; then \
		sudo cp -drpf $(CURDIR)/product/$(CONFIG_PRODUCT_NAME)/dbg/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
	@-if [ -d "$(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/dbg" ] ; then \
		sudo cp -drpf $(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/dbg/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
ifeq ($(CONFIG_ENABLE_SANDBOXING),y)
	@-if [ -d "$(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/sandbox/dbg" ] ; then \
		sudo cp -adrpf $(CURDIR)/security/$(CONFIG_PRODUCT_NAME)/sandbox/dbg/* $(HUMAX_APP_STAGGING_ROOT_DIR)/; \
	fi;
endif
endif

endif

ifeq ($(CONFIG_PROD_GCC_GLIBC_ASAN),y)
	@-sudo sed -i 's/0x20000000/0x40000000/g' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/default/modules
	@-sudo sed -i '/ASAN_OPTIONS/d' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop
	@-sudo sed -i '2 i\export ASAN_OPTIONS=handle_segv=1,disable_core=0,abort_on_error=1' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop
endif

ifeq ($(CONFIG_DEVELOP_RELEASE),y)
	@-sudo sed -i 's:/hama:/hama \&:' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop;
	@-sudo sed -i 's:/hama &launcher:/hamalauncher:' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop;
	@-sudo sed -i 's:/hama \& \&:/hama \&:' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/init.d/S90settop;
	@-sudo sed -i -e 's/^::once/#::once/' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/inittab;
	@-sudo sed -i -e 's/^\/dev/#\/dev/' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/inittab;
	@-sudo sed -i -e 's/^#ttyS0/ttyS0/' $(HUMAX_APP_STAGGING_ROOT_DIR)/etc/inittab;
endif
	@echo ---------- start remove .svn ------------
	-sudo find $(HUMAX_APP_STAGGING_ROOT_DIR) -name ".svn" | xargs -r sudo rm -rf
	@echo -----------end remove .svn --------------

