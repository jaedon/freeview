#####################################################
# Module dependent link and library
#####################################################

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug
APP-LINK-y			+= -lappkit
APP-LINK-y			+= -loapi -lhapi -ldapi -ldlib -lsitextlib -lsilib -lsapi -lrlib -luapi -lsamalib -lpapi -lhlib -ltapi 
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi

# library for HLIB
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_SUPPORT_NIMA)			+= -lnapi
###########################################################################

APP-LINK-y								+= -pthread -lssp

