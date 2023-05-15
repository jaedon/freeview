#####################################################
# Module dependent link and library
#####################################################

APP-LINK-$(CONFIG_PACKAGE_JANSSON) += -ljansson

#ifeq ($(CONFIG_PROD_CPU_BCM_ARM),y)
APP-LINK-y += -ldl
#endif
