############################################################

OPENGLES_DIR = $(MAGNUM)/../rockford/middleware/opengles

MAGNUM_MODULES += OPENGLES

ifneq ($(DIRECTFB_GLES_SUPPORT),y)
OPENGLES_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
OPENGLES_INCLUDES += $(NEXUS_TOP)/base/include
OPENGLES_INCLUDES += $(NEXUS_TOP)/base/include/public
OPENGLES_INCLUDES += $(NEXUS_TOP)/base/include/$(B_REFSW_OS)
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/core/include
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/surface/src
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/surface/include
#OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/surface/include/priv
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/graphics2d/src
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/graphics2d/include
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/graphics3d/src/p3d
OPENGLES_INCLUDES += $(NEXUS_TOP)/modules/graphics3d/include/p3d

OPENGLES_INCLUDES += $(OPENGLES_DIR)
OPENGLES_INCLUDES += $(OPENGLES_DIR)/GLES

OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_buffer.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_buffer.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_config.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_context.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_error.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_init.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/egl_surface.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_activetexture.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_alphablend.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_clear.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_clip.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_color.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_depth.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_enable.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_error.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_execute.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_fog.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_get.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_hint.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_light.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_logicop.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_material.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_matrix.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_normal.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_pixel.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_primmodes.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_samplecoverage.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_stencil.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_texcoord.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_teximage.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_texture.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_transform.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_vertex.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_vertexarray.c
OPENGLES_SOURCES += $(OPENGLES_DIR)/gl_viewport.c
ifdef MODE
NEXUS_MODE ?= $(MODE)
endif
ifeq ($(NEXUS_MODE),proxy)
OPENGLES_SOURCES += $(OPENGLES_DIR)/native_nexus_proxy.c
OPENGLES_INCLUDES += $(NEXUS_TOP)/platforms/$(PLATFORM)/src/linuxuser.proxy
else 
ifeq ($(DIRECTFB_SUPPORT),y)
DIRECTFB_DIR = $(MAGNUM)/../AppLibs/opensource/directfb/src/DirectFB-1.4.1
FUSION_DIR   = $(MAGNUM)/../AppLibs/opensource/directfb/src/driver/linux-fusion-8.1.1
OPENGLES_INCLUDES += $(DIRECTFB_DIR)/include
OPENGLES_INCLUDES += $(DIRECTFB_DIR)/lib
OPENGLES_INCLUDES += $(DIRECTFB_DIR)/src
OPENGLES_INCLUDES += $(DIRECTFB_DIR)/include
OPENGLES_INCLUDES += $(FUSION_DIR)/linux/include
OPENGLES_SOURCES += $(OPENGLES_DIR)/native_directfb.c
else
OPENGLES_SOURCES += $(OPENGLES_DIR)/native_nexus.c
endif
endif
endif
