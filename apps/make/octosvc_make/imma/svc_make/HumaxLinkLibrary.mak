#####################################################
# Private application link option
#####################################################

## Calypso Core Library 
APP-LINK-y += -lcalypso
## Calypso IPC Library 
APP-LINK-y += -lcso_ipc
## PlayReady Library 
APP-LINK-y += -lprdy 
## ViewRightWeb Library
APP-LINK-y += -lViewRightWebWrapper -lViewRightWebClient -lstdc++
## Calypso OEM Porting Library 
APP-LINK-y += -lmi 

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y                      += -lnexus
else
APP-LINK-y                      += -lnexus_client
endif

APP-LINK-$(CONFIG_SUPPORT_IMMA_ASAN) += -lasan
APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi
APP-LINK-y += -lhlib -lhapi -lpapi

# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

APP-LINK-$(CONFIG_PACKAGE_FFMPEG)		+= -lavcodec -lavfilter -lavformat -lavutil -lswresample -lswscale
APP-LINK-$(CONFIG_PACKAGE_OPENSSL)		+= -lssl -lcrypto
APP-LINK-$(CONFIG_PACKAGE_LIBXML2)		+= -lxml2

