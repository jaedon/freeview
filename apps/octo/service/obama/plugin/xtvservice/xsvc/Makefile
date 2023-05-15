#######################################
# set compile flag
#######################################
COMPILE_CAS_FLAG=0

ifeq ($(CONFIG_MW_CAS),y)
	COMPILE_CAS_FLAG=1
endif

ifeq ($(CONFIG_MW_CI),y)
	COMPILE_CAS_FLAG=1
endif


#######################################
# include
#######################################
# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path

#######################################
# make
#######################################

obj-y += fs/
obj-y += epg/
obj-y += si/
obj-y += sys/

obj-$(CONFIG_MW_DOWNLOAD) += download/
obj-$(CONFIG_MW_LOGCAPTURE) += logcapture/
ifeq ($(COMPILE_CAS_FLAG),1)
	obj-y += cas/
endif

