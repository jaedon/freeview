
-include .config

obj-y := $(HUMAX_HWTEST_DIR)/src/
obj-y += $(HUMAX_HWTEST_DIR)/src/ci_stack/
obj-y += $(HUMAX_HWTEST_DIR)/src/cmd_hwtest_ext/

ifeq ($(filter hwtest_av,$(MAKECMDGOALS)),hwtest_av)

obj-y += $(HUMAX_HWTEST_DIR)/src/cmd_hwtest_av/
else
ifeq ($(filter hwtest_osd,$(MAKECMDGOALS)),hwtest_osd)
obj-y += $(HUMAX_HWTEST_DIR)/src/cmd_hwtest_osd/
else
obj-y += $(HUMAX_HWTEST_DIR)/src/cmd_hwtest_core/
endif
endif

inccflag-y := -I$(HUMAX_WORK_DIR)/include/di
inccflag-y += -I$(HUMAX_WORK_DIR)/include/hos
inccflag-y += -I$(HUMAX_WORK_DIR)/include/cmd
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/ext
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/ext/channel
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/ext/thermal
inccflag-y += -I$(HUMAX_HWTEST_DIR)/src/tester/include/
inccflag-y += -I$(HUMAX_HWTEST_DIR)/src/ci_stack/include
inccflag-y += -I$(HUMAX_HWTEST_DIR)/src/ci_stack/include/ext
inccflag-y += -I$(HUMAX_DRIVER_DIR)/utils/include/
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/drv/include/

HUMAX_CONFIG_DIR := $(HUMAX_WORK_DIR)/product_config/apps/hwtest
HUMAX_KCONFIG_PARAMS := Kconfig=$(HUMAX_WORK_DIR)/product_config/apps/hwtest/kconfigs/Kconfig KconfigOutputDir=$(HUMAX_HWTEST_MAKE_DIR)
HUMAX_CONFIG_FILE_SRC := $(HUMAX_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/hwtest_config.cfg

HUMAX_EXTRA_CFLAG := $(inccflag-y) -include autoconf.h 
HUMAX_EXTRA_CFLAG += -include $(HUMAX_DRIVER_DIR)/autoconf.h
HUMAX_EXTRA_CFLAG += -DCONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME)
ifeq ($(CONFIG_PRODUCT_APP_DEBUG_LOG), y)
HUMAX_EXTRA_CFLAG += -DCONFIG_DEBUG=1
endif

-include $(HUMAX_LIB_LIST_FILE)

HUMAX_LD_LIB_PATH = -L$(CONFIG_TOOLCHAIN_PATH)/mipsel-linux/lib \
					-L$(HUMAX_OUTPUT_DIR)/lib \
					-L$(HUMAX_NFS_INSTALL_DIR)/lib \
					-L$(HUMAX_NFS_INSTALL_DIR)/usr/lib \
					-L$(HUMAX_WORK_DIR)/packages/libext2fs/lib

export HUMAX_EXTRA_CFLAG
export HUMAX_MODULE=APPS

include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak

hwtest_av: autoconf.h 
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' LIBNAME=$@ static-library

clean_hwtest_av:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' clean

menuconfig silentoldconfig oldconfig: .config
	$(HUMAX_MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) $@ $(HUMAX_KCONFIG_PARAMS)

clean_config:
	$(HUMAX_MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) mrproper $(HUMAX_KCONFIG_PARAMS)
	$(HUMAX_RM) .config autoconf.h

autoconf.h : .config
	make -C $(HUMAX_KCONFIG_TOOL_DIR) silentoldconfig $(HUMAX_KCONFIG_PARAMS)

.config : $(HUMAX_CONFIG_FILE_SRC) 
	$(HUMAX_CP) $< $@

hwtest_osd: autoconf.h 
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' LIBNAME=$@ static-library

clean_hwtest_osd:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' clean

hwtest_core: autoconf.h 
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' LIBNAME=$@ static-library

clean_hwtest_core:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' clean

link_av:
	$(HUMAX_CC) $(HUMAX_WORK_DIR)/apps/hwtest_multi/src/cmd_hwtest_av/cmd_hwtest_av.o -Wl,--start-group $(filter-out -lhwtest_osd,$(sort $(HUMAX_LIB_LIST))) -lblkid -lcom_err -le2p -lss -luuid -lext2fs -lpthread $(HUMAX_LD_LIB_PATH) $(sort $(LDFLAGS)) -L$(DLNA_TOP)/bin -lbdlna -lbdlna-dmp -pthread -lbdlna-nexus -lb_playback_ip -lnexus -lb_os -lm -lbdlna-dms -lbdlna-dms-aal -lb_psip -lexpat -Wl,--end-group -o $(HUMAX_NFS_INSTALL_DIR)/usr/bin/hwtest_av

link_osd:
	$(HUMAX_CC) $(HUMAX_WORK_DIR)/apps/hwtest_multi/src/cmd_hwtest_osd/cmd_hwtest_osd.o -Wl,--start-group $(filter-out -lhwtest_av,$(sort $(HUMAX_LIB_LIST))) -lblkid -lcom_err -le2p -lss -luuid -lext2fs -lpthread $(HUMAX_LD_LIB_PATH) $(sort $(LDFLAGS)) -L$(DLNA_TOP)/bin -lbdlna -lbdlna-dmp -pthread -lbdlna-nexus -lb_playback_ip -lnexus -lb_os -lm -lbdlna-dms -lbdlna-dms-aal -lb_psip -lexpat -Wl,--end-group -o $(HUMAX_NFS_INSTALL_DIR)/usr/bin/hwtest_osd

link_core:
	$(HUMAX_CC) $(HUMAX_WORK_DIR)/apps/hwtest_multi/src/cmd_hwtest_core/cmd_hwtest_core.o -Wl,--start-group -lhwtest_core -ldriver -lvkernel -lnexus -lblkid -lcom_err -le2p -lss -luuid -lext2fs -lpthread $(HUMAX_LD_LIB_PATH) $(sort $(LDFLAGS)) -L$(DLNA_TOP)/bin -lbdlna -lbdlna-dmp -pthread -lbdlna-nexus -lb_playback_ip -lnexus -lb_os -lm -lbdlna-dms -lbdlna-dms-aal -lb_psip -lexpat -Wl,--end-group -o $(HUMAX_NFS_INSTALL_DIR)/usr/bin/hwtest_core
