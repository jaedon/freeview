##
# Set up defines
##
DEFINES := COMPONENT_CORSAIR

LOCAL_DEFINES   :=

##
# Component configuration
##
SOURCES 	:= src/corsair.cpp\
			   src/corsair_gfx.cpp\
			   src/corsair_surface.cpp\
			   src/corsair_hbbtv_surface.cpp\
			   src/corsair_web_surface.cpp\
			   src/corsair_env.cpp\
			   src/corsair_ait_manager.cpp\
			   src/corsair_ait_impl.cpp\
			   src/corsair_dsmcc_uri.cpp\
			   src/corsair_dsmcc_filerequest.cpp\
			   src/corsair_dsmcc_filerequestmanager.cpp\
			   src/corsair_dsmcc_objectcarousel.cpp\
			   src/corsair_dsmcc_objectcarousel_appid.cpp\
			   src/corsair_dsmcc_objectcarouselmanager.cpp\
			   src/corsair_dsmcc_streameventlistener.cpp\
			   src/corsair_dsmcc_xmlstreameventlistener.cpp\
			   src/corsair_dsmcc_streameventlistenermanager.cpp\
			   src/corsair_dsmcc_streameventmessage.cpp\
			   src/corsair_dsmcc_manager.cpp\
			   src/corsair_dsmcc_impl.cpp\
			   src/corsair_input.cpp\
			   src/corsair_input_default.cpp\
			   src/corsair_input_bcm.cpp\
			   src/corsair_utils.cpp\
			   src/corsair_tools.cpp\
			   src/corsair_ime.cpp\
			   src/corsair_ime_jlabs.cpp\
			   src/corsair_tokenizer.cpp\
			   src/corsair_cert_manager.cpp\
			   src/corsair_hinput.cpp\
			   src/corsair_debug.cpp\
			   src/corsair_bookmark.cpp\
			   src/corsair_xml.cpp\
			   src/corsair_hama.cpp\
			   src/corsair_postman.cpp\
			   src/corsair_otvs.cpp \
			   src/corsair_otvs_surface.cpp	\
			   src/corsair_product_env.cpp	\
			   src/corsair_hbbtv_application.cpp	\
			   src/corsair_hbbtv_action.cpp	\
			   src/corsair_hbbtv_manager.cpp	\
			   src/corsair_fps_displayer.cpp	\
			   src/corsair_security_manager.cpp	\

ifeq ($(USE_NATIVE_PRISM), y)
SOURCES += src/corsair_native_prism_manager.cpp	\
			   src/corsair_native_prism_event_receiver.cpp	\
			   src/corsair_native_prism_msg_sender.cpp	
endif

SOURCES += ../../tools/sedec/sources/sedec/src/common/bit_readwriter.cpp \
					../../tools/sedec/sources/sedec/src/common/descriptor.cpp \
					../../tools/sedec/sources/sedec/src/common/descriptor_factory.cpp \
					../../tools/sedec/sources/sedec/src/common/section.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/application.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/application_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/application_name_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/application_usage_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/parental_rating_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/application_recording_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/connection_requirement_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/simple_application_location_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/simple_application_boundary_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/descriptors/transport_protocol_descriptor.cpp \
					../../tools/sedec/sources/sedec/src/sections/ait_section.cpp \
					../../tools/sedec/sources/sedec/src/sections/ait_section_transcoder.cpp \

OUTPUT		:= corsair

LIBS		:= pthread operamalloc opera_hbbtv opera_oipf opera 
LIBS		+= bpl appkit hlib dlib clib silib sitextlib rlib oapi hapi uapi dapi papi samalib sapi thapi tapi 
LIBS		+= freetype sqlite3 directfb fusion direct z png jpeg id3 vkernel exif ssp crypto rt curl ssl xml2
LIBS		+= memory-null
#LIBS		+= memory-debug

ifeq ($(CONFIG_SUPPORT_NIMA), y)
LIBS += napi
endif

ifeq ($(CONFIG_SUPPORT_NOVA_APP), y)
LIBS += novaapi
endif

ifeq ($(CONFIG_SUPPORT_IMMA), y)
LIBS += calypso prdy ViewRightWebWrapper ViewRightWebClient mi avutil avcodec avfilter avformat
endif

NEXUS_CLIENT=$(wildcard $(NATIVE_APP_PATH)/apps/make/_gen_rootfs/root/usr/lib/libnexus_client.so)
ifneq ($(NEXUS_CLIENT),)
LIBS		+= nexus_client
else
LIBS		+= nexus
endif


LIBPATH         := $(NATIVE_PORT_PATH)/src/ $(NATIVE_APP_PATH)/apps/make/_gen_rootfs/root/usr/lib/ 
LIBPATH					+= $(NATIVE_APP_PATH)/packages/log4c/lib/ 
LIBPATH         += $(GOGI_SDK_PATH)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib

TYPE		:= EXECUTABLE

INCPATH		:= ../components/corsair/include\
		   ../components/corsair/oipf/include\
		   ../jsplugins/oipf/include\
			 ../../../humax-impl/tools/sedec/sources/sedec/include\
		   $(NATIVE_APP_PATH)/apps/octo/library/hlib/include\
		   $(NATIVE_APP_PATH)/apps/octo/library/dlib/include\
		   $(NATIVE_APP_PATH)/apps/octo/library/clib/include\
		   $(NATIVE_APP_PATH)/apps/octo/library/rlib/include\
		   $(NATIVE_APP_PATH)/apps/octo/library/silib/include\
		   $(NATIVE_APP_PATH)/apps/octo/library/memory\
		   $(NATIVE_APP_PATH)/apps/octo/service/obama/include\
		   $(NATIVE_APP_PATH)/apps/octo/service/hama/include\
		   $(NATIVE_APP_PATH)/apps/octo/service/dama/include\
		   $(NATIVE_APP_PATH)/apps/octo/service/umma/include\
		   $(NATIVE_APP_PATH)/apps/octo/service/thma/include\
		   $(NATIVE_APP_PATH)/apps/octo/service/sama/include\
		   $(NATIVE_APP_PATH)/device/include/hos\
		   $(NATIVE_APP_PATH)/apps/octo/appkit/include\
		   $(NATIVE_APP_PATH)/apps/make/include/\
		   $(GOGI_SDK_PATH)/include\
		   $(OOIF_PATH)/include\
		   $(NATIVE_APP_PATH)/packages/libxml2/include\

LOCAL_DEFINES   += OIPF HBBTV HBBTV_1_2_1 HBBTV_SECURITY

ifeq ($(CONFIG_HUMAX_CRASHREPORT), y)
LIBS		+= hcrlib
INCPATH		+= $(NATIVE_APP_PATH)/apps/octo/library/hcrlib/include
endif

#EXTRAFLAGS := $(shell $(DIRECTFBBASE)/bin/directfb-config --cflags)

#EXTRA_LFLAGS := $(CORSAIR_EXTRA_LFLAGS) -rdynamic $(shell $(DIRECTFBBASE)/bin/directfb-config --libs)

#ifeq ($(DYNAMIC_FREETYPE),YES)
# EXTRA_LFLAGS += $(shell $(FREETYPEDIR)/bin/freetype-config --libs)
#endif


# Used to configure DSM-CC if the Opera supplied DSM-CC is used.
ifeq ($(CORSAIR_DSMCC_EXTERNAL),YES)
 ifneq ($(CORSAIR_DSMCC_EXTERNAL_PATH),)
  EXTRA_LFLAGS += -L$(CORSAIR_DSMCC_EXTERNAL_PATH)
 endif
 ifneq ($(CORSAIR_DSMCC_EXTERNAL_INCLUDES),)
  INCPATH += $(CORSAIR_DSMCC_EXTERNAL_INCLUDES)
 endif
 # Can be used with the Opera supplied DSM-CC, if the DVB API is not yet implemented
 ifeq ($(CORSAIR_DVB_DUMMY_IMPLEMENTATION), YES)
  LOCAL_DEFINES += CORSAIR_DVB_DUMMY_IMPLEMENTATION
  SOURCES += src/corsair_dvb_impl.cpp
 endif
 LIBS += z dsmcc
endif

ifeq ($(DVB_PLATFORM_BUILD),YES)
	LIBS += dvbplatform
	DEPENDENCIES += dvb_platform
	INCPATH += ../components/dvb_platform/include
endif

ifeq ($(CORSAIR_DVB_DUMMY_IMPLEMENTATION),YES)
	LOCAL_DEFINES += CORSAIR_DVB_DUMMY_IMPLEMENTATION
endif
#END

# Can be used if the DSM-CC API is not yet implemented.
ifeq ($(CORSAIR_DSMCC_DUMMY_IMPLEMENTATION),YES)
 LOCAL_DEFINES += CORSAIR_DSMCC_DUMMY_IMPLEMENTATION
endif

# Can be used if the AIT API is not yet implemented.
ifeq ($(CORSAIR_AIT_DUMMY_IMPLEMENTATION),YES)
 LOCAL_DEFINES += CORSAIR_AIT_DUMMY_IMPLEMENTATION
endif

#  Enabling this will enable code in the sigsegv handler in Corsair
#  that will print a traceback on error conditions. This uses system
#  functions defined in the "execinfo.h", such as backtrace() and
#  backtrace_symbols(). This might not be available on some systems.
ifeq ($(CORSAIR_SEGV_HANDLER_WITH_TRACEBACK),YES)
 LOCAL_DEFINES += CORSAIR_SEGV_HANDLER_WITH_TRACEBACK
endif

# Enable hardware acceleration.
ifeq ($(USE_VEGA_DFB),YES)
 LOCAL_DEFINES += USE_VEGA_DFB
endif

# Defines extra library path/s that might be needed when building Corsair.
ifneq ($(CORSAIR_EXTRA_LIBPATH),)
 LIBPATH += $(CORSAIR_EXTRA_LIBPATH)
endif

# Set DFBSurfaceDescription.resource_id to the defined value on Corsair surfaces
ifneq ($(CORSAIR_USE_DFB_RESOURCE_ID),)
  DEFINES += CORSAIR_USE_DFB_RESOURCE_ID=$(CORSAIR_USE_DFB_RESOURCE_ID)
endif

# Enables Corsair to use DirectFB windows instead of surface.
#ifeq ($(CORSAIR_USE_DFB_WINDOW),YES)
LOCAL_DEFINES += CORSAIR_USE_DFB_WINDOW
#endif

# Enables alternative input handling.
ifeq ($(CORSAIR_INPUT_BROADCOM),YES)
 LOCAL_DEFINES += CORSAIR_INPUT_BROADCOM
else
 LOCAL_DEFINES += CORSAIR_INPUT_DEFAULT
endif

ifeq ($(OIPF_BUILD),YES)
   LOCAL_DEFINES += OIPF
   INCPATH += ../components/oipf/include
   SOURCES += oipf/src/asteriks.cpp \
              oipf/src/oipf_surface.cpp \

#   LIBS += opera_oipf
#   DEPENDENCIES += oipf
endif

# Disable flipping of the primary DFB surface on ST7108 platform
ifeq ($(PLATFORM), sh4-ST7108-STLinux-2.4)
  LOCAL_DEFINES += NOFLIP
endif

# Add mumin libraries

EXTRA_LFLAGS += -Wl,--allow-shlib-undefined
LIBS += uva

ifeq ($(UVA_GSTREAMER_BACKEND),YES)
  ifeq ($(PLATFORM),x86)
    LIBS += uvabackend-gst-dfb dfbwindowsink
    LOCAL_DEFINES += CORSAIR_USE_DFB_WINDOW=YES
  else
    LIBS += uvabackend-gst
  endif

  ifeq ($(GSTREAMER_PATH),)
    GST_LDFLAGS := $(shell PKG_CONFIG_SYSROOT_DIR=$(PKG_CONFIG_SYSROOT_DIR) PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs-only-L gstreamer-0.10 gstreamer-base-0.10 gstreamer-video-0.10 glib-2.0)
  else
    GST_LDFLAGS := -Wl,-rpath-link=$(GSTREAMER_PATH)/lib
  endif

  EXTRA_LFLAGS += $(GST_LDFLAGS)
else
  LIBS += uvabackend-hmp
endif # UVA_GSTREAMER_BACKEND

#################################################################
# add humax options
#################################################################
ifeq ($(USE_DIRECTFBGL),YES)
INCPATH			+= $(NATIVE_APP_PATH)/device/include/opengl/
INCPATH			+= $(NATIVE_APP_PATH)/device/include/directfb/
INCPATH			+= $(NATIVE_APP_PATH)/device/include/hos/
INCPATH			+= $(NATIVE_APP_PATH)/device/include/fusion/
LIBS			+= v3ddriver dbpl
LOCAL_DEFINES 	+= USE_DIRECTFBGL
SOURCES			+= src/corsair_gfx_egl.cpp
SOURCES			+= src/corsair_opengl_fbo.cpp
SOURCES			+= src/corsair_opengl_debug.cpp
endif

ifeq ($(USE_OPERA_TVSTORE),YES)
LOCAL_DEFINES 	+= ENABLE_OPERA_TVSTORE
endif

################################################################################
# Set the value for logging
################################################################################
ifeq ($(CONFIG_DEBUG),y)
LOCAL_DEFINES += CONFIG_CORSAIR_LOG
endif

ifeq ($(CONFIG_DEBUG_RELEASE_LOG),y)
LOCAL_DEFINES += CONFIG_CORSAIR_LOG
endif

ifeq ($(CONFIG_DEVELOP_RELEASE),y)
LOCAL_DEFINES += CONFIG_CORSAIR_LOG
endif
