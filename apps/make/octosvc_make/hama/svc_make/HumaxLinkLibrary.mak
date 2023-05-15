#####################################################
# Module dependent link and library
#####################################################

NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)
ifneq ($(NEXUS_CLIENT),)
APP-LINK-y			+= -lnexus_client
endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
APP-LINK-y			+= -lhcrlib
endif
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi -lhapi 
# library for HLIB
APP-LINK-$(CONFIG_PACKAGE_CAP)      	+= -lcap -lattr
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
#############################################################################

APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)		+= -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz

APP-LINK-y								+= -pthread -lssp

