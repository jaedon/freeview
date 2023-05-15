#####################################################
# Module dependent link and library
#####################################################

APP-LINK-$(CONFIG_SUPPORT_COMMA_ASAN)			+= -lasan

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib

# library for HLIB
APP-LINK-$(CONFIG_PACKAGE_CAP) 			+= -lcap -lattr
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
#############################################################################

APP-LINK-y								+= -pthread -lssp

