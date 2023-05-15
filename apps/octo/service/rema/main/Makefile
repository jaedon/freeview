obj-y += datamodel/
obj-y += module/
obj-y += cpe_peer/
ifeq  ($(DUNIT_TEST_REMA), no)
#ifdef $(DUNIT_TEST_REMA)
  obj-y += rema.o
endif

obj-y += rema_core.o
obj-y += rema_cwmp.o
obj-y += rema_cmd.o
obj-y += rema_MOD.o
obj-y += rema_DM.o
obj-y += rema_util.o

obj-y += rema_cwmp_tr135.o
obj-y += rema_cwmp_tr140.o
obj-y += rema_cwmp_tr157.o
obj-y += rema_cwmp_tr181.o

ccflags-y += -I$(THIRDPARTY_DIR)/cwmp/include
ccflags-y += -I$(REMA_MAIN_DIR)/datamodel
ccflags-y += -I$(REMA_MAIN_DIR)/module
ccflags-y += -I$(REMA_MAIN_DIR)/include
ccflags-y += -I$(REMA_MAIN_DIR)/cpe_peer
ccflags-y += -I$(REMA_MAIN_DIR)/datamodel/network
