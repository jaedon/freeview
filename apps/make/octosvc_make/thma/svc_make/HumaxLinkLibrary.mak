#####################################################
# Module dependent link and library
#####################################################

APP-LINK-$(CONFIG_SUPPORT_THMA_ASAN) += -lasan
APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib -lhapi -lpapi -loapi -lrlib -ldlib -lsitextlib
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
APP-LINK-y			+= -lhcrlib
endif
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi

# library for HLIB
APP-LINK-$(CONFIG_PACKAGE_CAP)          += -lcap -lattr
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
###########################################################################

APP-LINK-y								+= -pthread -lssp

