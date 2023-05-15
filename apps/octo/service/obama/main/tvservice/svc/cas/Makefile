#######################################
# set compile flag
#######################################
COMPILE_FLAG=0

ifeq ($(CONFIG_MW_CAS),y)
	COMPILE_FLAG=1
endif

ifeq ($(CONFIG_MW_CI),y)
	COMPILE_FLAG=1
endif


#######################################
# include
#######################################
# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mw

ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/cas/local_include

#######################################
# make
#######################################
ifeq ($(COMPILE_FLAG),1)
	obj-y += svc_cas.o
	obj-y += svc_cas_sub_api.o
	obj-y += svc_cas_sub_dev_api.o
	obj-y += svc_cas_sub_svc_api.o
	obj-y += core/
	obj-y += port/

	obj-$(CONFIG_DEBUG) += cmd/
else
	obj-y += svc_cas_null.o
endif

