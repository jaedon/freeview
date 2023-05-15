#####################################################
# Module dependent link and library
#####################################################

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib -lhapi

# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi
#############################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

