-include ./dirs

ifeq ($(DIRS),)
#TARGET = drmcerts
TARGETEXE = drmchkhds

DRM_MODULE_CFLAGS = -I$(DRM_SOURCE_PATH)/modules/hds/real

SOURCES =                \
	chkhds.c 
	
endif	
   
include $(DRM_SOURCE_PATH)/default.mak
