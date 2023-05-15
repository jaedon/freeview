#############################################################################
#
#       Copyright (c) 2004, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: opengles.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 7/27/04 8:21p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/middleware/opengles/linux/opengles.inc $
# 
# Hydra_Software_Devel/1   7/27/04 8:21p nissen
# PR 9132: Initial version.
# 
#############################################################################

vpath %.c $(ROOT_DIR)/rockford/middleware/opengles/

INCLUDES += \
	-I$(ROOT_DIR)/rockford/middleware/opengles/ \
	-I$(ROOT_DIR)/rockford/middleware/opengles/GLES

OBJS += \
	native_magnum.o \
	egl_buffer.o \
	egl_config.o \
	egl_context.o \
	egl_error.o \
	egl_init.o \
	egl_surface.o \
	gl_activetexture.o \
	gl_alphablend.o \
	gl_clear.o \
	gl_clip.o \
	gl_color.o \
	gl_depth.o \
	gl_enable.o \
	gl_error.o \
	gl_execute.o \
	gl_fog.o \
	gl_get.o \
	gl_hint.o \
	gl_light.o \
	gl_logicop.o \
	gl_material.o \
	gl_matrix.o \
	gl_normal.o \
	gl_pixel.o \
	gl_primmodes.o \
	gl_samplecoverage.o \
	gl_stencil.o \
	gl_texcoord.o \
	gl_teximage.o \
	gl_texture.o \
	gl_transform.o \
	gl_vertex.o \
	gl_vertexarray.o \
	gl_viewport.o

