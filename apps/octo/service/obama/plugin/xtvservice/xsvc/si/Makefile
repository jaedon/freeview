#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------

obj-y += base/

## Satellite
obj-$(CONFIG_OP_MIDDLE_EAST) += mesat/
obj-$(CONFIG_OP_SES) += ses/
obj-$(CONFIG_OP_ERTELECOM) += ertelecom/
obj-$(CONFIG_OP_AUS_DTT) += ausdtt/
obj-$(CONFIG_OP_FREESAT) += ukfsat/
obj-$(CONFIG_OP_UK_DTT) += ukdtt/
obj-$(CONFIG_OP_ZIGGO) += ziggo/
obj-$(CONFIG_OP_ITALY_DBOOK) += itadtt/
obj-$(CONFIG_OP_ORF) += hdfoxirconnect/
obj-$(CONFIG_OP_NORDIG) += nordig/
obj-$(CONFIG_OP_MBC) += mbc/
obj-$(CONFIG_OP_AMX) += amx/

obj-y += xsvc_si.o
obj-$(CONFIG_DEBUG) += xsvc_si_cmd.o
obj-y += xsvc_si_lib.o
obj-y += xsvc_si_swup.o
obj-y += xsvc_si_time.o

obj-y += psvc_si.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include
