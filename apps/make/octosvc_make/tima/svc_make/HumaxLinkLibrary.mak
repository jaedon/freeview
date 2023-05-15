#####################################################
# Module dependent link and library
#####################################################

APP-LINK-$(CONFIG_SUPPORT_TIMA_ASAN) += -lasan
APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
APP-LINK-y			+= -lhcrlib
endif

# library for HLIB
APP-LINK-$(CONFIG_PACKAGE_CAP)          += -lcap -lattr
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi -lhapi 

###########################################################################

APP-LINK-y								+= -pthread -lssp