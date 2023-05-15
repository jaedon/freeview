#####################################################
# Module dependent link and library
#####################################################

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y                      += -lnexus
else
APP-LINK-y                      += -lnexus_client
endif

APP-LINK-y			+= -ldi_network
APP-LINK-$(CONFIG_MW_INET_WLAN) += -ldi_wifi -ldrv_hotplug
APP-LINK-y			+= -ldriver

APP-LINK-$(CONFIG_PROD_DEVICE_BLUETOOTH) += -lbsa -ldi_bt
APP-LINK-$(CONFIG_MW_INET_PPPOE) += -ldi_pppoe

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib -lhapi
APP-LINK-$(CONFIG_HUMAX_CRASHREPORT) += -lhcrlib
# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

