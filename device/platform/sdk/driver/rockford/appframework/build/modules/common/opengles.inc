#############################################################################
#
#       Copyright (c) 2005, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: opengles.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 10/26/05 2:25p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/modules/common/opengles.inc $
# 
# Hydra_Software_Devel/1   10/26/05 2:25p nissen
# PR 17656: Relocating file.
# 
# Hydra_Software_Devel/3   10/12/05 9:03p nissen
# PR 12814: Changed directories.
# 
# Hydra_Software_Devel/2   9/22/05 6:51p nissen
# PR 17194: Update for FrameWork.
# 
# Hydra_Software_Devel/1   5/23/05 4:54p jasonh
# PR 15572: include file for opengl ES middleware.
# 
#############################################################################

OPENGLES_DIR = $(ROCKFORD_DIR)/middleware/opengles

COMMON_INCLUDES += \
	$(OPENGLES_DIR) \
	$(OPENGLES_DIR)/GLES

COMMON_SOURCES += \
	$(OPENGLES_DIR)/native_magnum.c \
	$(OPENGLES_DIR)/egl_buffer.c \
	$(OPENGLES_DIR)/egl_config.c \
	$(OPENGLES_DIR)/egl_context.c \
	$(OPENGLES_DIR)/egl_error.c \
	$(OPENGLES_DIR)/egl_init.c \
	$(OPENGLES_DIR)/egl_surface.c \
	$(OPENGLES_DIR)/gl_activetexture.c \
	$(OPENGLES_DIR)/gl_alphablend.c \
	$(OPENGLES_DIR)/gl_clear.c \
	$(OPENGLES_DIR)/gl_clip.c \
	$(OPENGLES_DIR)/gl_color.c \
	$(OPENGLES_DIR)/gl_depth.c \
	$(OPENGLES_DIR)/gl_enable.c \
	$(OPENGLES_DIR)/gl_error.c \
	$(OPENGLES_DIR)/gl_execute.c \
	$(OPENGLES_DIR)/gl_fog.c \
	$(OPENGLES_DIR)/gl_get.c \
	$(OPENGLES_DIR)/gl_hint.c \
	$(OPENGLES_DIR)/gl_light.c \
	$(OPENGLES_DIR)/gl_logicop.c \
	$(OPENGLES_DIR)/gl_material.c \
	$(OPENGLES_DIR)/gl_matrix.c \
	$(OPENGLES_DIR)/gl_normal.c \
	$(OPENGLES_DIR)/gl_pixel.c \
	$(OPENGLES_DIR)/gl_primmodes.c \
	$(OPENGLES_DIR)/gl_samplecoverage.c \
	$(OPENGLES_DIR)/gl_stencil.c \
	$(OPENGLES_DIR)/gl_texcoord.c \
	$(OPENGLES_DIR)/gl_teximage.c \
	$(OPENGLES_DIR)/gl_texture.c \
	$(OPENGLES_DIR)/gl_transform.c \
	$(OPENGLES_DIR)/gl_vertex.c \
	$(OPENGLES_DIR)/gl_vertexarray.c \
	$(OPENGLES_DIR)/gl_viewport.c
