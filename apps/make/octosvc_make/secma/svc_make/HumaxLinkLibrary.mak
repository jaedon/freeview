#####################################################
# Private application link option
#####################################################

NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)
ifneq ($(NEXUS_CLIENT),)
APP-LINK-y			+= -lnexus_client
endif
APP-LINK-y			+= -ldi

APP-LINK-$(CONFIG_SUPPORT_SECMA_ASAN) += -lasan
APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

######################################################################################
#
#        Package Include Path
#
######################################################################################
