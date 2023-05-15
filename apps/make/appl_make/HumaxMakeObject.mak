#
# Copyright (C) 2006 HUMAX Co., Ltd. All rights reserved.
#
define build_testapp
	@$(MAKE) -C $(APPL_MAKE_DIR)/testapp $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_android
	@$(MAKE) -C $(APPL_MAKE_DIR)/android $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_bml
	@$(MAKE) -C $(APPL_MAKE_DIR)/bml $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_cmduartconsole
	@$(MAKE) -C $(APPL_MAKE_DIR)/cmduartconsole $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_prism
	@$(MAKE) -C $(APPL_MAKE_DIR)/prism $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_keyboard
	@$(MAKE) -C $(APPL_MAKE_DIR)/keyboard $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_irldvt
	@$(MAKE) -C $(APPL_MAKE_DIR)/irldvt $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_wind3
	@$(MAKE) -C $(APPL_MAKE_DIR)/wind3 $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_mhegib
	@$(MAKE) -C $(APPL_MAKE_DIR)/mhegib $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_mheg
	@$(MAKE) -C $(APPL_MAKE_DIR)/mheg $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_novaapi
	@$(MAKE) -C $(APPL_MAKE_DIR)/nova/api_make $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nova
	@$(MAKE) -C $(APPL_MAKE_DIR)/nova/app_make $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_remoteapp
	@$(MAKE) -C $(APPL_MAKE_DIR)/remoteapp $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nativeapp
	@$(MAKE) -C $(APPL_MAKE_DIR)/nativeapp $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_rfvod
	@$(MAKE) -C $(APPL_MAKE_DIR)/rfvod $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_tr069
	@$(MAKE) -C $(APPL_MAKE_DIR)/tr069 $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_ipepg
	@$(MAKE) -C $(APPL_MAKE_DIR)/ipepg $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_sotalupgrade
	@$(MAKE) -C $(APPL_MAKE_DIR)/sotalupgrade $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_restnode
	@$(MAKE) -C $(APPL_MAKE_DIR)/restnode $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nodebinder
	@$(MAKE) -C $(APPL_MAKE_DIR)/nodebinder $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_cloudsync
	@$(MAKE) -C $(APPL_MAKE_DIR)/cloudsync $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_netflix
	@$(MAKE) -C $(APPL_MAKE_DIR)/netflix $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nrd
	@$(MAKE) -C $(APPL_MAKE_DIR)/netflix $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nxlink
	@$(MAKE) -C $(APPL_MAKE_DIR)/netflix/nxlink.build $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nxmanager
	@$(MAKE) -C $(APPL_MAKE_DIR)/netflix/nxmanager.build $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

define build_nxtools
	@$(MAKE) -C $(APPL_MAKE_DIR)/netflix/nxtools.build $(APP_CONFIG) $(1) 2>&1 || exit -1;
endef

###############################################################

## resource make in HumaxResource.mak
-include $(APPL_MAKE_DIR)/HumaxResource.mak

testapp:
	$(call build_testapp,testapp)
ifeq ($(CONFIG_MW_CAS_VIACCESS), y)
	@sudo $(HUMAX_CP) $(APPL_MAKE_DIR)/testapp/libacs_va.a /nfsroot/$(USER)/$(PRODUCT_NAME)/root/usr/lib/
	@sudo $(HUMAX_CP) $(APPL_MAKE_DIR)/testapp/libtestapp.a /nfsroot/$(USER)/$(PRODUCT_NAME)/root/usr/lib/
endif
testapp-clean:
	$(call build_testapp,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/testapp-compile.log
testapp-distclean:
	$(call build_testapp,distclean)

.PHONY: testapp testapp-clean testapp-distclean

prism:
	@$(MAKE) -C $(APPL_MAKE_DIR)/prism $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
prism-clean:
	@$(MAKE) -C $(APPL_MAKE_DIR)/prism $(APP_CONFIG) clean 2>&1 || exit -1;
prism-distclean:
	@$(MAKE) -C $(APPL_MAKE_DIR)/prism $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: prism prism-clean prism-distclean

bml:
	$(call build_bml,bml)
bml-clean:
	$(call build_bml,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/bml-compile.log
bml-distclean:
	$(call build_bml,distclean)

.PHONY: bml bml-clean bml-distclean

bsautomsg:
	$(call build_bsautomsg,bsautomsg)
bsautomsg-clean:
	$(call build_bsautomsg,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/bsautomsg-compile.log
bsautomsg-distclean:
	$(call build_bsautomsg,distclean)

.PHONY: bsautomsg bsautomsg-clean bsautomsg-distclean

keyboard:
	$(call build_keyboard,keyboard)
keyboard-clean:
	$(call build_keyboard,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/keyboard-compile.log
keyboard-distclean:
	$(call build_keyboard,distclean)

.PHONY: keyboard keyboard-clean keyboard-distclean

irldvt:
	$(call build_irldvt,irldvt)
irldvt-clean:
	$(call build_irldvt,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/irldvt-compile.log
irldvt-distclean:
	$(call build_irldvt,distclean)

.PHONY: irldvt irldvt-clean irldvt-distclean

remoteapp:
	$(call build_remoteapp,remoteapp)
remoteapp-clean:
	$(call build_remoteapp,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/remoteapp-compile.log
remoteapp-distclean:
	$(call build_remoteapp,distclean)

.PHONY: remoteapp remoteapp-clean remoteapp-distclean

nativeapp:
	$(call build_nativeapp,nativeapp)
nativeapp-clean:
	$(call build_nativeapp,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/nativeapp-compile.log
nativeapp-distclean:
	$(call build_nativeapp,distclean)

.PHONY: nativeapp nativeapp-clean nativeapp-distclean

tr069:
	$(call build_tr069,tr069)
tr069-clean:
	$(call build_tr069,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/tr069-compile.log
tr069-distclean:
	$(call build_tr069,distclean)

.PHONY: tr069 tr069-clean tr069-distclean

ipepg:
	$(call build_ipepg,ipepg)
ipepg-clean:
	$(call build_ipepg,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/ipepg-compile.log
ipepg-distclean:
	$(call build_ipepg,distclean)

.PHONY: ipepg ipepg-clean ipepg-distclean


rfvod:
	$(call build_rfvod,rfvod)
rfvod-clean:
	$(call build_rfvod,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/rfvod-compile.log
rfvod-distclean:
	$(call build_rfvod,distclean)

.PHONY: rfvod rfvod-clean rfvod-distclean

wind3:
	$(call build_wind3,wind3)
wind3-clean:
	$(call build_wind3,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/wind3-compile.log
wind3-distclean:
	$(call build_wind3,distclean)

.PHONY: wind3 wind3-clean wind3-distclean

mhegib:
	$(call build_mhegib,mhegib)
mhegib-clean:
	$(call build_mhegib,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/mhegib-compile.log
mhegib-distclean:
	$(call build_mhegib,distclean)

.PHONY: mhegib mhegib-clean mhegib-distclean


android:
	$(call build_android,android)
android-clean:
	$(call build_android,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/android-compile.log
android-distclean:
	$(call build_android,distclean)

.PHONY: android android-clean android-distclean

mheg:
	$(call build_mheg,mheg)
mheg-clean:
	$(call build_mheg,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/mheg-compile.log
mheg-distclean:
	$(call build_mheg,distclean)

.PHONY: mheg mheg-clean mheg-distclean

novaapi:
	$(call build_novaapi,novaapi)
novaapi-clean:
	$(call build_novaapi,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/novaapi-compile.log
novaapi-distclean:
	$(call build_novaapi,distclean)

.PHONY: novaapi novaapi-clean novaapi-distclean


nova:
	$(call build_nova,nova)
nova-clean:
	$(call build_nova,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/nova-compile.log
nova-distclean:
	$(call build_nova,distclean)

.PHONY: nova nova-clean nova-distclean

cmduartconsole:
	@$(MAKE) -C $(APPL_MAKE_DIR)/cmduartconsole $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
cmduartconsole-clean:
	@$(MAKE) -C $(APPL_MAKE_DIR)/cmduartconsole $(APP_CONFIG) clean  2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/cmduartconsole-compile.log
cmduartconsole-distclean:
	@$(MAKE) -C $(APPL_MAKE_DIR)/cmduartconsole $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: cmduartconsole cmduartconsole-clean cmduartconsole-distclean

sotalupgrade:
	$(call build_sotalupgrade,sotalupgrade)
sotalupgrade-clean:
	$(call build_sotalupgrade,clean)
sotalupgrade-distclean:
	$(call build_sotalupgrade,distclean)

.PHONY: sotalupgrade sotalupgrade-clean sotalupgrade-distclean

restnode:
	$(call build_restnode,restnode)
restnode-clean:
	$(call build_restnode,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/restnode-compile.log
restnode-distclean:
	$(call build_restnode,distclean)

.PHONY: restnode restnode-clean restnode-distclean

nodebinder:
	$(call build_nodebinder)
nodebinder-clean:
	$(call build_nodebinder,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/nodebinder-compile.log
nodebinder-distclean:
	$(call build_nodebinder,distclean)

.PHONY: nodebinder nodebinder-clean nodebinder-distclean

cloudsync:
	@$(MAKE) -C $(APPL_MAKE_DIR)/cloudsync $(APP_CONFIG)  2>&1 || exit -1;
cloudsync-clean:
	@$(MAKE) -C $(APPL_MAKE_DIR)/cloudsync $(APP_CONFIG) clean 2>&1 || exit -1;	
	@$(RM) $(HUMAX_MAKE_DIR)/cloudsync-compile.log
cloudsync-distclean:
	@$(MAKE) -C $(APPL_MAKE_DIR)/cloudsync $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: cloudsync cloudsync-clean cloudsync-distclean



netflix:
	$(call build_nxlink,nxlink)
	$(call build_netflix,netflix)
	$(call build_nxmanager,nxmanager)
	$(call build_nxtools,nxtools)	
netflix-clean:
	$(call build_nxlink,clean)
	$(call build_netflix,clean)
	$(call build_nxmanager,clean)
	$(call build_nxtools,clean)	
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/netflix-compile.log
netflix-distclean:
	$(call build_netflix,distclean)

.PHONY: netflix netflix-clean netflix-distclean

nrd:
	$(call build_nrd,nrd)
nrd-clean:
	$(call build_nrd,clean)
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/nrd-compile.log
nrd-distclean:
	$(call build_nrd,distclean)

.PHONY: nrd nrd-clean nrd-distclean

nxlink:
	$(call build_nxlink,nxlink)
nxlink-clean:
	$(call build_nxlink,clean)
nxlink-distclean:
	$(call build_nxlink,distclean)

.PHONY: nxlink nxlink-clean nxlink-distclean

nxmanager:
	$(call build_nxmanager,nxmanager)
nxmanager-clean:
	$(call build_nxmanager,clean)
nxmanager-distclean:
	$(call build_nxmanager,distclean)

.PHONY: nxmanager nxmanager-clean nxmanager-distclean

nxtools:
	$(call build_nxtools,nxtools)
nxtools-clean:
	$(call build_nxtools,clean)
nxtools-distclean:
	$(call build_nxtools,distclean)
	
.PHONY: nxtools nxtools-clean nxtools-distclean
