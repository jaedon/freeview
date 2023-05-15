-include ./dirs

ifeq ($(DIRS),)
TARGET = oem_ansi

SOURCES =                \
	../oemdebug.c ../oemimpl.c ../oemfileio.c ../oeminfo.c ../oemtime.c ../oemprofile.c ../oemcriticalsection.c \
	../oem_init.c ../oem_config.c ../oem_security.c ../oem_m2m_dma.c ../oem_key_region.c 
	
endif
   
include $(DRM_SOURCE_PATH)/default.mak
