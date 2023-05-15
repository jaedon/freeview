
.PHONY: direcftb-defines gfx-defines sys-defines app-defines ir-inputmode

##
#
# sys-defines: create the directfb system defines.
#
sys-defines:
	@if [ -e "${DIRECTFB_UNPACKED_FLAG}" ]; then \
		$(call fn-create-sys-defines); \
	fi

##
#
# gfx-defines: create the directfb graphics defines.
#
gfx-defines:
	@if [ -e "${DIRECTFB_UNPACKED_FLAG}" ]; then \
		$(call fn-create-gfx-defines); \
	fi

##
#
# app-defines: create the directfb application defines.
#
app-defines:
	@if [ -e "${DIRECTFB_UNPACKED_FLAG}" ]; then \
		$(call fn-create-app-defines); \
	fi
##
#
# ir-inputmode: create the directfb input defines.
#
ir-inputmode:
	@if [ -e "${DIRECTFB_UNPACKED_FLAG}" ]; then \
		$(call fn-create-ir-inputmode); \
	fi

##
#
# directfb-defines: autogenerate the directfb defines include files.
#
directfb-defines: directfb-source sys-defines gfx-defines app-defines ir-inputmode


##
#
# Function to auto-generate the Broadcom DirectFB Graphics Defines file.
#
ifeq ($(shell test -e ${NEXUS_TOP}/base/include/nexus_types.h && echo "y"),y)
NEXUS_TYPES_H := ${NEXUS_TOP}/base/include/nexus_types.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/core/${BCHP_CHIP}/include/nexus_types.h && echo "y"),y)
NEXUS_TYPES_H := ${NEXUS_TOP}/modules/core/${BCHP_CHIP}/include/nexus_types.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/core/include/nexus_types.h && echo "y"),y)
NEXUS_TYPES_H := ${NEXUS_TOP}/modules/core/include/nexus_types.h
else
$(error unknown location of nexus_types.h in nexus)
endif
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/base/include/nexus_video_types.h && echo "y"),y)
NEXUS_VIDEO_TYPES_H := ${NEXUS_TOP}/base/include/nexus_video_types.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/core/${BCHP_CHIP}/include/nexus_video_types.h && echo "y"),y)
NEXUS_VIDEO_TYPES_H := ${NEXUS_TOP}/modules/core/${BCHP_CHIP}/include/nexus_video_types.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/core/include/nexus_video_types.h && echo "y"),y)
NEXUS_VIDEO_TYPES_H := ${NEXUS_TOP}/modules/core/include/nexus_video_types.h
else
$(error unknown location of nexus_video_types.h in nexus)
endif
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/modules/ir_input/${BCHP_CHIP}/include/nexus_ir_input.h && echo "y"),y)
NEXUS_IR_INPUT_H := ${NEXUS_TOP}/modules/ir_input/${BCHP_CHIP}/include/nexus_ir_input.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/ir_input/include/nexus_ir_input.h && echo "y"),y)
NEXUS_IR_INPUT_H := ${NEXUS_TOP}/modules/ir_input/include/nexus_ir_input.h
else
$(error unknown location of nexus_ir_input.h in nexus)
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/modules/graphics2d/src/nexus_graphics2d.c && echo "y"),y)
NEXUS_GRAPHICS2D_C := ${NEXUS_TOP}/modules/graphics2d/src/nexus_graphics2d.c
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/graphics2d/${BCHP_CHIP}/src/nexus_graphics2d.c && echo "y"),y)
NEXUS_GRAPHICS2D_C := ${NEXUS_TOP}/modules/graphics2d/${BCHP_CHIP}/src/nexus_graphics2d.c
else
$(error unknown location of nexus_graphics2d.c in nexus)
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/modules/graphics2d/include/nexus_graphics2d.h && echo "y"),y)
NEXUS_GRAPHICS2D_H := ${NEXUS_TOP}/modules/graphics2d/include/nexus_graphics2d.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/graphics2d/${BCHP_CHIP}/include/nexus_graphics2d.h && echo "y"),y)
NEXUS_GRAPHICS2D_H := ${NEXUS_TOP}/modules/graphics2d/${BCHP_CHIP}/include/nexus_graphics2d.h
else
$(error unknown location of nexus_graphics2d.h in nexus)
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/modules/display/include/nexus_display.h && echo "y"),y)
NEXUS_DISPLAY_H := ${NEXUS_TOP}/modules/display/include/nexus_display.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/display/${BCHP_CHIP}/include/nexus_display.h && echo "y"),y)
NEXUS_DISPLAY_H := ${NEXUS_TOP}/modules/display/${BCHP_CHIP}/include/nexus_display.h
else
$(error unknown location of nexus_display.h in nexus)
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/modules/display/include/nexus_display_cursor.h && echo "y"),y)
NEXUS_DISPLAY_CURSOR_H := ${NEXUS_TOP}/modules/display/include/nexus_display_cursor.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/display/${BCHP_CHIP}/include/nexus_display_cursor.h && echo "y"),y)
NEXUS_DISPLAY_CURSOR_H := ${NEXUS_TOP}/modules/display/${BCHP_CHIP}/include/nexus_display_cursor.h
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/modules/picture_decoder/include/nexus_picture_decoder.h && echo "y"),y)
NEXUS_PICTURE_DECODER_H := ${NEXUS_TOP}/modules/picture_decoder/include/nexus_picture_decoder.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/modules/picture_decoder/${BCHP_CHIP}/include/nexus_picture_decoder.h && echo "y"),y)
NEXUS_PICTURE_DECODER_H := ${NEXUS_TOP}/modules/picture_decoder/${BCHP_CHIP}/include/nexus_picture_decoder.h
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/platforms/${NEXUS_PLATFORM}/include/nexus_platform_client.h && echo "y"),y)
NEXUS_PLATFORM_CLIENT_H := ${NEXUS_TOP}/platforms/${NEXUS_PLATFORM}/include/nexus_platform_client.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/platforms/common/include/nexus_platform_client.h && echo "y"),y)
NEXUS_PLATFORM_CLIENT_H := ${NEXUS_TOP}/platforms/common/include/nexus_platform_client.h
else
$(error Unknown location for nexus_platform_client.h in nexus)
endif
endif

ifeq ($(shell test -e ${NEXUS_TOP}/platforms/${NEXUS_PLATFORM}/include/nexus_platform_server.h && echo "y"),y)
NEXUS_PLATFORM_SERVER_H := ${NEXUS_TOP}/platforms/${NEXUS_PLATFORM}/include/nexus_platform_server.h
else
ifeq ($(shell test -e ${NEXUS_TOP}/platforms/common/include/nexus_platform_server.h && echo "y"),y)
NEXUS_PLATFORM_SERVER_H := ${NEXUS_TOP}/platforms/common/include/nexus_platform_server.h
else
$(error Unknown location for nexus_platform_server.h in nexus)
endif
endif

ifeq ($(shell test -e ${MAGNUM}/portinginterface/vdc/${BCHP_CHIP}/bvdc_common_priv.h && echo "y"),y)
MAGNUM_VDC_COMMON_PRIV_H := ${MAGNUM}/portinginterface/vdc/${BCHP_CHIP}/bvdc_common_priv.h
else
MAGNUM_VDC_COMMON_PRIV_H := $(shell find ${MAGNUM}/portinginterface/vdc/ -name bvdc_common_priv.h)
ifeq (${MAGNUM_VDC_COMMON_PRIV_H},"")
$(error Unknown location for bvdc_common_priv.h in magnum)
endif
endif

ifeq ($(shell test -e ${MAGNUM}/portinginterface/vdc/${BCHP_CHIP}/bvdc.h && echo "y"),y)
MAGNUM_VDC_H := ${MAGNUM}/portinginterface/vdc/${BCHP_CHIP}/bvdc.h
else
ifeq ($(shell test -e ${MAGNUM}/portinginterface/vdc/include/bvdc.h && echo "y"),y)
MAGNUM_VDC_H := ${MAGNUM}/portinginterface/vdc/include/bvdc.h
else
$(error unknown location for bvdc.h in magnum)
endif
endif

ifeq ($(shell test -e ${MAGNUM}/portinginterface/grc/${BCHP_CHIP}/bgrc_private.h && echo "y"),y)
MAGNUM_GRC_PRIV_H := ${MAGNUM}/portinginterface/grc/${BCHP_CHIP}/bgrc_private.h
else
ifeq ($(shell test -e ${MAGNUM}/portinginterface/grc/src/bgrc_private.h && echo "y"),y)
MAGNUM_GRC_PRIV_H := ${MAGNUM}/portinginterface/grc/src/bgrc_private.h
else
$(error unknown location for bgrc_private.h in magnum)
endif
endif

ifeq ($(shell test -e ${MAGNUM}/portinginterface/grc/${BCHP_CHIP}/bm2mc_packet.h && echo "y"),y)
MAGNUM_M2MC_PACKET_H := ${MAGNUM}/portinginterface/grc/${BCHP_CHIP}/bm2mc_packet.h
else
ifeq ($(shell test -e ${MAGNUM}/portinginterface/grc/include/bm2mc_packet.h && echo "y"),y)
MAGNUM_M2MC_PACKET_H := ${MAGNUM}/portinginterface/grc/include/bm2mc_packet.h
else
$(error unknown location for bm2mc_packet.h in magnum)
endif
endif

ifeq ($(shell test -e ${MAGNUM}/portinginterface/grc/${BCHP_CHIP}/bgrc_packet_priv.c && echo "y"),y)
MAGNUM_GRC_PACKET_PRIV_C := ${MAGNUM}/portinginterface/grc/${BCHP_CHIP}/bgrc_packet_priv.c
else
ifeq ($(shell test -e ${MAGNUM}/portinginterface/grc/src/bgrc_packet_priv.c && echo "y"),y)
MAGNUM_GRC_PACKET_PRIV_C := ${MAGNUM}/portinginterface/grc/src/bgrc_packet_priv.c
else
$(error unknown location for bgrc_packet_priv.c in magnum)
endif
endif

SED_PF = $(SED) -ne "/^typedef enum NEXUS_PixelFormat/,/} NEXUS_PixelFormat/p" "${NEXUS_TYPES_H}"
SED_BS = $(SED) -ne "/^typedef struct NEXUS_Graphics2DBlitSettings/,/} NEXUS_Graphics2DBlitSettings/p" "${NEXUS_GRAPHICS2D_H}"
SED_GS = $(SED) -ne "/^typedef struct NEXUS_Graphics2DSettings/,/} NEXUS_Graphics2DSettings/p" "${NEXUS_GRAPHICS2D_H}"
SED_PD = $(SED) -ne "/^NEXUS_Error NEXUS_Graphics2D_PorterDuffFill/,/}/p" "${NEXUS_GRAPHICS2D_C}"
SED_VF = $(SED) -ne "/^typedef enum NEXUS_VideoFormat/,/} NEXUS_VideoFormat/p" "${NEXUS_VIDEO_TYPES_H}"

define fn-create-gfx-defines
	$(CAT) ${DIRECTFB_BRCM_HEADER_FILE} > ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	$(ECHO) "/* This file has been auto-generated by the Broadcom DirectFB Makefile - do NOT delete! */" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	$(ECHO) "#ifndef __BCMNEXUS_GFX_DEFINES_H__" >>  ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	$(ECHO)  "#define __BCMNEXUS_GFX_DEFINES_H__" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	if [ -e "${NEXUS_GRAPHICS2D_H}" ]; then \
		if ($(SED_BS) | $(GREP) -qw "alphaPremultiplySourceEnabled"); then \
			if (! $(GREP) -qw "Alpha premultiply source is not supported" "${NEXUS_GRAPHICS2D_C}"); then \
				$(ECHO) "#define BCMNEXUS_ALPHA_PREMULT_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
			fi; \
		fi; \
		if ($(SED_BS) | $(GREP) -qw "patternSettings"); then \
			if (! $(GREP) -qw "pattern setting iis not supported" "${NEXUS_GRAPHICS2D_C}"); then \
				if (! $(GREP) -qw "pattern setting is not supported" "${NEXUS_GRAPHICS2D_C}"); then \
					$(ECHO) "#define BCMNEXUS_ROP_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
				fi; \
			fi; \
		fi; \
		if ($(SED_BS) | $(GREP) -qw "mirrorSourceVertically"); then \
			if (! $(GREP) -qw "Mirror source or destination is not supported" "${NEXUS_GRAPHICS2D_C}"); then \
				$(ECHO) "#define BCMNEXUS_MIRRORING_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
			fi; \
		fi; \
		if ($(SED_GS) | $(GREP) -qw "checkpointCallback"); then \
		    $(ECHO) "#define BCMNEXUS_CHECKPOINT_IN_SETTINGS_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(GREP) -q "NEXUS_Error NEXUS_Graphics2D_PorterDuffFill" "${NEXUS_GRAPHICS2D_H}"); then \
			if (! $(SED_PD) | $(GREP) -qw "NEXUS_NOT_SUPPORTED"); then \
				$(ECHO) "#define BCMNEXUS_PORTERDUFF_FILL_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
			fi; \
		fi; \
		if ($(GREP) -q "packetFifoThreshold" "${NEXUS_GRAPHICS2D_H}"); then \
			$(ECHO) "#define BCMNEXUS_GFX_PACKET_FIFO_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_TYPES_H}" ]; then \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eA0"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_A0_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eA1"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_A1_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eA2"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_A2_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eA4"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_A4_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eX1_R5_G5_B5"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_X1_R5_G5_B5_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eX1_B5_G5_R5"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_X1_B5_G5_R5_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eX4_R4_G4_B4"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_X4_R4_G4_B4_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eR8_G8_B8"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eX8_R8_G8_B8"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_X8_R8_G8_B8_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eA8_B8_G8_R8"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_A8_B8_G8_R8_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_PF) | $(GREP) -qw "NEXUS_PixelFormat_eB5_G6_R5"); then \
			$(ECHO) "#define BCMNEXUS_PIXELFORMAT_B5_G6_R5_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${MAGNUM_GRC_PRIV_H}" ]; then \
		$(ECHO) -n "#define BCMNEXUS_GFX_STRIP_WIDTH_MAX " >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		$(CPP) -dD ${BRCM_CFLAGS}  ${MAGNUM_GRC_PRIV_H} | \
		$(GREP) STRIP_WIDTH_MAX | $(CUT) -d\  -f3- >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		$(ECHO) -n "#define BCMNEXUS_GFX_SCALE_DOWN_MAX_X " >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		$(CPP) -dD ${BRCM_CFLAGS} ${MAGNUM_GRC_PRIV_H} | \
		$(GREP) -w BGRC_P_SCALE_DOWN_MAX | $(CUT) -d\  -f3- >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		$(ECHO) -n "#define BCMNEXUS_GFX_SCALE_DOWN_MAX_Y " >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		$(CPP) -dD ${BRCM_CFLAGS} ${MAGNUM_GRC_PRIV_H} | \
		$(GREP) -w BGRC_P_SCALE_DOWN_MAX_Y | $(CUT) -d\  -f3- >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	fi; \
	if [ "${DIRECTFB_GFX_3D}" = "v3d" ]; then \
		$(ECHO) "#define BCMNEXUS_GFX_HAS_V3D" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	elif [ "${DIRECTFB_GFX_3D}" = "p3d" ]; then \
		$(ECHO) "#define BCMNEXUS_GFX_HAS_P3D" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		if [ "${DIRECTFB_GFX_SOFT_MATRIX_SUPPORT}" = "y" ]; then \
			$(ECHO) "#define BCMNEXUS_SOFT3D_MATRIX_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${MAGNUM_M2MC_PACKET_H}" ]; then \
		if ($(GREP) -qw "BM2MC_PACKET_WRITE_DestinationColor(" ${MAGNUM_M2MC_PACKET_H} ); then \
			if [ "${DIRECTFB_GFX_PACKET_BUFFER}" = "y" ]; then \
				$(ECHO) "#define BCMNEXUS_GFX_PACKET_BUFFER" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
				if [ "${DIRECTFB_GFX_TRAPEZOID_SUPPORT}" = "y" ] && $(GREP) -qw "DFXL_FILLTRAPEZOID" ${DIRECTFB_INCLUDE_DIR}/directfb.h; then \
					$(ECHO) "#define BCMNEXUS_TRAPEZOID_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
				fi; \
			fi; \
		fi; \
		if ($(GREP) -qw "BM2MC_PACKET_WRITE_FixedScale(" ${MAGNUM_M2MC_PACKET_H}); then \
			if ($(GREP) -qw "uint32_t actual_ver_step" ${MAGNUM_GRC_PACKET_PRIV_C} && \
			    $(GREP) -qw "uint32_t min_phase" ${MAGNUM_GRC_PACKET_PRIV_C} && \
                test `grep "brcm_Revision" ${MAGNUM_GRC_PACKET_PRIV_C} | cut -d/ -f2- | cut -d\  -f1` -ge 139 ); then \
				$(ECHO) "#define BCMNEXUS_FIXEDSCALE_SUPPORTED" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
			fi; \
		fi; \
	fi; \
	if [ "${DIRECTFB_HW_DITHERING}" = "y" ]; then \
		$(ECHO) "#define BCMNEXUS_HW_DITHERING 1" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	fi; \
	$(ECHO)  "#endif /* __BCMNEXUS_GFX_DEFINES_H__ */" >> ${DIRECTFB_GFX_DEFINES_FILE}.$$$$; \
	if [ -e "${DIRECTFB_GFX_DEFINES_FILE}" ]; then \
		if ($(DIFF) ${DIRECTFB_GFX_DEFINES_FILE} ${DIRECTFB_GFX_DEFINES_FILE}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_GFX_DEFINES_FILE}.$$$$ ${DIRECTFB_GFX_DEFINES_FILE}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_GFX_DEFINES_FILE}.$$$$ ${DIRECTFB_GFX_DEFINES_FILE}; \
	fi; \
	$(RM) ${DIRECTFB_GFX_DEFINES_FILE}.$$$$
endef

##
#
# Function to auto-generate the Broadcom DirectFB System Defines file.
#
#$(GREP) BVDC_P_SUPPORT_GFD_VER | $(CUT) -d\  -f3- >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; 
#$(ECHO) "($(shell grep -w BVDC_P_SUPPORT_3D_VIDEO ${MAGNUM}/portinginterface/vdc/${BCHP_CHIP}/bvdc_common_priv.h))";
define fn-create-sys-defines
	$(CAT) ${DIRECTFB_BRCM_HEADER_FILE} > ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "/* This file has been auto-generated by the Broadcom DirectFB Makefile - do NOT delete! */" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#ifndef __BCMNEXUS_SYS_DEFINES_H__" >>  ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define __BCMNEXUS_SYS_DEFINES_H__" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) -n "#define BCMNEXUS_VDC_SUPPORT_3D_VIDEO " >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	if ($(CPP) -dD ${BRCM_CFLAGS} ${MAGNUM_VDC_COMMON_PRIV_H} | $(GREP) -qw BVDC_P_SUPPORT_3D_VIDEO ); then \
			$(CPP) -dD ${BRCM_CFLAGS} ${MAGNUM_VDC_COMMON_PRIV_H} | \
                        $(GREP) -w BVDC_P_SUPPORT_3D_VIDEO | $(CUT) -d\  -f3- >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	else \
		$(ECHO) "(0)" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x800p_60Hz"); then \
		$(ECHO) "" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X800P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1440x900p60hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1440X900P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1680x1050p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1680X1050P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X800P_60HZ_RED_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1600x1200p_75Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1600X1200P_75HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1600X900P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1680X1050P_60HZ_RED_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1920x1200p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1920X1200P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1920X1200P_60HZ_RED_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x720p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X720P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x720p_75Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X720P_75HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x720p_85Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X720P_85HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x864p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X864P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x864p_85Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X864P_85HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x870p_75Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X870P_75HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x900p_66Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X900P_66HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1152x900p_76Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1152X900P_76HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1170x584p_50Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1170X584P_50HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x1024p_70Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_70HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x1024p_72Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_72HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x1024p_76Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_76HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x768p_50Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X768P_50HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1280x960p_75Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1280X960P_75HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1600x1024p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1600X1024P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1600x1024p_76Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1600X1024P_76HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa1728x1080p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA1728X1080P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa800x600p_100Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA800X600P_100HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa800x600p_90Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA800X600P_90HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa848x480p_75Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA848X480P_75HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa848x480p_85Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA848X480P_85HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eVesa852x480p_60Hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_VESA852X480P_60HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_e720p_3DOU_AS"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_720P_3DOU_AS_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_e720p50hz_3DOU_AS"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_720P50HZ_3DOU_AS_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_e720p30hz_3DOU_AS"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_720P30HZ_3DOU_AS_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_e720p24hz_3DOU_AS"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_720P24HZ_3DOU_AS_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_e1080p24hz_3DOU_AS"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_1080P24HZ_3DOU_AS_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_e1080p30hz_3DOU_AS"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_1080P30HZ_3DOU_AS_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eCustom_3D_720p"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eCustom_3D_720p_50hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_50HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eCustom_3D_720p_30hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_30HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eCustom_3D_720p_24hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_24HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eCustom_3D_1080p_24hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_1080P_24HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if ($(SED_VF) | $(GREP) -qw "NEXUS_VideoFormat_eCustom_3D_1080p_30hz"); then \
		$(ECHO) "#define BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_1080P_30HZ_SUPPORTED" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	$(ECHO) -n "#define BCMNEXUS_GFD_VER " >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(CPP) -dD ${BRCM_CFLAGS} ${MAGNUM_VDC_COMMON_PRIV_H} | \
	$(GREP) BVDC_P_SUPPORT_GFD_VER | $(CUT) -d\  -f3- >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#if (BCMNEXUS_GFD_VER >= 3)" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_GFD0_HAS_VERTICAL_SCALER" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_GFD1_HAS_VERTICAL_SCALER" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#endif" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#if (BCMNEXUS_GFD_VER >= 4)" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_GFD_SCALE_FILTER_COEFF DFB_PlatformFilterCoeffs_eBilinear" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#else" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_GFD_SCALE_FILTER_COEFF DFB_PlatformFilterCoeffs_eAnisotropic" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#endif" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) -n "#define BCMNEXUS_VEC_HAS_1080P_60HZ_SUPPORT " >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(CPP) -dD ${BRCM_CFLAGS} ${MAGNUM_VDC_COMMON_PRIV_H} | \
	$(GREP) BVDC_P_SUPPORT_1080p_60HZ | $(CUT) -d\  -f3- >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_MAX_HD_DISPLAY_WIDTH  1920" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_MAX_HD_DISPLAY_HEIGHT 1080" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_MAX_SD_DISPLAY_WIDTH   720" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define BCMNEXUS_MAX_SD_DISPLAY_HEIGHT  576" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(ECHO) -n "#define BCMNEXUS_Z_ORDER_MAX " >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	$(GREP) "#define BVDC_Z_ORDER_MAX" ${MAGNUM_VDC_H} | \
	$(CUT) -d\  -f3- >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	if [ -e "${NEXUS_DISPLAY_H}" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_DisplaySettings/,/} NEXUS_DisplaySettings/p" \
		    ${NEXUS_DISPLAY_H} | $(GREP) -qw "vsyncCallback" ); then \
			$(ECHO) "#define BCMNEXUS_DISPLAY_HAS_VSYNC_CALLBACK 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED) -ne "/^typedef struct NEXUS_GraphicsFramebuffer3D/,/} NEXUS_GraphicsFramebuffer3D/p" \
			${NEXUS_DISPLAY_H} | $(GREP) -qw "orientation" ); then \
			$(ECHO) "#define BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED) -ne "/^typedef struct NEXUS_GraphicsSettings/,/} NEXUS_GraphicsSettings/p" \
			${NEXUS_DISPLAY_H} | $(GREP) -qw "bool visible" ); then \
			$(ECHO) "#define BCMNEXUS_DISPLAY_GRAPHICS_HAS_VISIBLE_FLAG" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_PLATFORM_SERVER_H}" ]; then \
		$(ECHO) "#define BCMNEXUS_HAS_IPC 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		if ($(SED) -ne "/^typedef struct NEXUS_PlatformStartServerSettings/,/} NEXUS_PlatformStartServerSettings/p" \
			${NEXUS_PLATFORM_SERVER_H} | $(GREP) -qw "defaultUntrustedHeap" ); then \
			$(ECHO) "#define BCMNEXUS_DEFAULT_UNTRUSTED_HEAP_SUPPORT 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED) -ne "/^typedef struct NEXUS_PlatformStartServerSettings/,/} NEXUS_PlatformStartServerSettings/p" \
			${NEXUS_PLATFORM_SERVER_H} | $(GREP) -qw "allowTrustedClientsToCrash" ); then \
			$(ECHO) "#define BCMNEXUS_ALLOW_TRUSTED_CLIENTS_TO_CRASH_SUPPORT 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED) -ne "/^typedef struct NEXUS_PlatformStartServerSettings/,/} NEXUS_PlatformStartServerSettings/p" \
			${NEXUS_PLATFORM_SERVER_H} | $(GREP) -qw "trustUnauthenticatedClients" ); then \
			$(ECHO) "#define BCMNEXUS_TRUST_UNAUTHENTICATED_CLIENTS_SUPPORT 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_PLATFORM_CLIENT_H}" ]; then \
		if ($(SED) -ne "/^typedef enum NEXUS_ClientMode/,/} NEXUS_ClientMode/p" \
			${NEXUS_PLATFORM_CLIENT_H} | $(GREP) -qw "NEXUS_ClientMode_eProtected" ); then \
			$(ECHO) "#define BCMNEXUS_PROTECTED_CLIENT_SUPPORT 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_TOP}/modules/video_decoder/${BCHP_CHIP}/include/nexus_video_decoder_init.h" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_VideoDecoderModuleSettings/,/} NEXUS_VideoDecoderModuleSettings/p" \
			${NEXUS_TOP}/modules/video_decoder/${BCHP_CHIP}/include/nexus_video_decoder_init.h | $(GREP) -qw "unsigned avdMapping" ); then \
			$(ECHO) "#define BCMNEXUS_VIDEO_DECODER_MFD_AVD_MAPPING 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_PICTURE_DECODER_H}" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_PictureDecoderStatus/,/} NEXUS_PictureDecoderStatus/p" \
			${NEXUS_PICTURE_DECODER_H} | $(GREP) -qw "bool hasTransparentData" ); then \
			$(ECHO) "#define BCMNEXUS_IMAGE_TRANSPARENCY_DATA_PRESENT" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_DISPLAY_CURSOR_H}" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_DisplayCursor/,/} NEXUS_DisplayCursor/p" \
			${NEXUS_DISPLAY_CURSOR_H} | $(GREP) -qw "NEXUS_SurfaceHandle surface" ); then \
			$(ECHO) "#define BCMNEXUS_HW_CURSOR_SUPPORT" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ "${DIRECTFB_NSC_SUPPORT}" = "y" -o "${DIRECTFB_NSC_SUPPORT}" = "multibuffer" ]; then \
		$(ECHO) "#define BCMNEXUS_NSC_SUPPORT BCMNEXUS_NSC_MULTIBUFFER_MODE" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	if [ "${DIRECTFB_NSC_SUPPORT}" = "tunnelled" ]; then \
		$(ECHO) "#define BCMNEXUS_NSC_SUPPORT BCMNEXUS_NSC_TUNNELLED_MODE" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
		$(ECHO) "DIRECTFB_GFX_PACKET_BUFFER $${DIRECTFB_GFX_PACKET_BUFFER}"; \
	fi; \
	if [ "${DIRECTFB_NIR_SUPPORT}" = "y" ]; then \
		$(ECHO) "#define BCMNEXUS_NIR_SUPPORT 1" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	fi; \
	$(ECHO) "#endif /* __BCMNEXUS_SYS_DEFINES_H__ */" >> ${DIRECTFB_SYS_DEFINES_FILE}.$$$$; \
	if [ -e "${DIRECTFB_SYS_DEFINES_FILE}" ]; then \
		if ($(DIFF) ${DIRECTFB_SYS_DEFINES_FILE} ${DIRECTFB_SYS_DEFINES_FILE}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_SYS_DEFINES_FILE}.$$$$ ${DIRECTFB_SYS_DEFINES_FILE}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_SYS_DEFINES_FILE}.$$$$ ${DIRECTFB_SYS_DEFINES_FILE}; \
	fi; \
	$(RM) ${DIRECTFB_SYS_DEFINES_FILE}.$$$$
endef

##
#
# Function to auto-generate the Broadcom DirectFB App Defines file.
#
SED_FE = $(SED) -ne "/^typedef enum NEXUS_FrontendOfdmMode/,/} NEXUS_FrontendOfdmMode/p" "${NEXUS_TOP}/modules/frontend/common/include/nexus_frontend_ofdm.h"

define fn-create-app-defines
	$(CAT) ${DIRECTFB_BRCM_HEADER_FILE} > ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
	$(ECHO) "/* This file has been auto-generated by the Broadcom DirectFB Makefile - do NOT delete! */" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
	$(ECHO) "#ifndef __BCMNEXUS_APP_DEFINES_H__" >>  ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
	$(ECHO) "#define __BCMNEXUS_APP_DEFINES_H__" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
	if [ -e "${NEXUS_TOP}/modules/frontend/common/include/nexus_frontend_ofdm.h" ]; then \
		if ($(SED_FE) | $(GREP) -qw "NEXUS_FrontendOfdmMode_eDvbt"); then \
		    $(ECHO) "#define BCMNEXUS_FRONTEND_DVBT_SUPPORTED" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
		fi; \
		if ($(SED_FE) | $(GREP) -qw "NEXUS_FrontendOfdmMode_eIsdbt"); then \
		    $(ECHO) "#define BCMNEXUS_FRONTEND_ISDBT_SUPPORTED" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_TOP}/modules/simple_decoder/include/nexus_simple_video_decoder.h" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_SimpleVideoDecoderStartSettings/,/} NEXUS_SimpleVideoDecoderStartSettings/p" \
		    ${NEXUS_TOP}/modules/simple_decoder/include/nexus_simple_video_decoder.h | $(GREP) -qw "NEXUS_VideoDecoderStartSettings" ); then \
			$(ECHO) "#define BCMNEXUS_SIMPLE_VIDEO_DECODER_HAS_START_SETTINGS 1" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_TOP}/modules/simple_decoder/include/nexus_simple_audio_decoder.h" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_SimpleAudioDecoderStartSettings/,/} NEXUS_SimpleAudioDecoderStartSettings/p" \
		    ${NEXUS_TOP}/modules/simple_decoder/include/nexus_simple_audio_decoder.h | $(GREP) -qw "NEXUS_AudioDecoderStartSettings" ); then \
			$(ECHO) "#define BCMNEXUS_SIMPLE_AUDIO_DECODER_HAS_START_SETTINGS 1" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	if [ -e "${NEXUS_TOP}/modules/playback/include/nexus_playback.h" ]; then \
		if ($(SED) -ne "/^typedef struct NEXUS_PlaybackPidChannelSettings/,/} NEXUS_PlaybackPidChannelSettings/p" \
		    ${NEXUS_TOP}/modules/playback/include/nexus_playback.h | $(GREP) -qw "NEXUS_SimpleVideoDecoderHandle" ); then \
			$(ECHO) "#define BCMNEXUS_PLAYBACK_PID_CHANNEL_HAS_SIMPLE_DECODER 1" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
		fi; \
	fi; \
	$(ECHO) "#endif /* __BCMNEXUS_APP_DEFINES_H__ */" >> ${DIRECTFB_APP_DEFINES_FILE}.$$$$; \
	if [ -e "${DIRECTFB_APP_DEFINES_FILE}" ]; then \
		if ($(DIFF) ${DIRECTFB_APP_DEFINES_FILE} ${DIRECTFB_APP_DEFINES_FILE}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_APP_DEFINES_FILE}.$$$$ ${DIRECTFB_APP_DEFINES_FILE}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_APP_DEFINES_FILE}.$$$$ ${DIRECTFB_APP_DEFINES_FILE}; \
	fi; \
	$(RM) ${DIRECTFB_APP_DEFINES_FILE}.$$$$
endef

##
#
# Function to auto-generate the Broadcom DirectFB IR InputMode file.
#
define fn-create-ir-inputmode
	$(CAT) ${DIRECTFB_BRCM_HEADER_FILE} > ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "/* This file has been auto-generated by the Broadcom DirectFB Makefile - do NOT delete! */" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "#ifndef __BCMNEXUS_IR_INPUTMODE_H__" >>  ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "#define __BCMNEXUS_IR_INPUTMODE_H__" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "#include <strings.h>" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "typedef enum DFB_PlatformIrInputMode" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "{" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	for ir in `$(CPP) -dD ${BRCM_CFLAGS} ${NEXUS_IR_INPUT_H} | \
	$(SED) -ne "/^typedef enum NEXUS_IrInputMode/,/} NEXUS_IrInputMode/p" | $(SED) -ne "/NEXUS_IrInputMode_e/p" | \
	$(SED) -e "/NEXUS_IrInputMode_eMax/d" | $(CUT) -d, -f1|cut -d= -f1 | $(SED) -e "s/NEXUS_IrInputMode_//g"`; do \
	    $(ECHO) "    DFB_PlatformIrInputMode_$$ir," >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	       done; \
	$(ECHO) "    DFB_PlatformIrInputMode_eMax">> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "} DFB_PlatformIrInputMode;" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) -n "#define DFB_PlatformIrInputMode_eMin " >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(CPP) -dD ${BRCM_CFLAGS} "${NEXUS_IR_INPUT_H}" | \
	$(SED) -ne "/^typedef enum NEXUS_IrInputMode/,/NEXUS_IrInputMode_e/p" | $(SED) -ne "/NEXUS_IrInputMode_e/p" | \
	$(CUT) -d, -f1|cut -d= -f1 | $(SED) -e "s/NEXUS_/DFB_Platform/g" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "static const char *DFB_PlaformIrInputModeName_Table[] = " >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "{" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	for ir in `$(CPP) -dD ${BRCM_CFLAGS} "${NEXUS_IR_INPUT_H}" | \
	$(SED) -ne "/^typedef enum NEXUS_IrInputMode/,/} NEXUS_IrInputMode/p" | $(SED) -ne "/NEXUS_IrInputMode_e/p" | \
	$(SED) -e "/NEXUS_IrInputMode_eMax/d" | $(CUT) -d, -f1|cut -d= -f1 | $(SED) -e "s/NEXUS_IrInputMode_e//g"`; do \
	    $(ECHO) "    [DFB_PlatformIrInputMode_e$$ir] = \"$$ir\"," >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	       done; \
	$(ECHO) "};" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "static inline DFB_PlatformIrInputMode DFB_PlatformLookupIrInputmode (const char* name) {" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "    DFB_PlatformIrInputMode inputMode;" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "    for (inputMode = DFB_PlatformIrInputMode_eMin; inputMode < DFB_PlatformIrInputMode_eMax; inputMode++)" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "    { " >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "        if (strcasecmp(name, DFB_PlaformIrInputModeName_Table[inputMode]) == 0)" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "            return inputMode;" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "    }" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "    return DFB_PlatformIrInputMode_eMax;" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
               $(ECHO) "}" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(ECHO) "#endif /* __BCMNEXUS_IR_INPUTMODE_H__ */" >> ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	if [ -e "${DIRECTFB_IR_INPUTMODE_FILE}" ]; then \
		if ($(DIFF) ${DIRECTFB_IR_INPUTMODE_FILE} ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$ ${DIRECTFB_IR_INPUTMODE_FILE}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$ ${DIRECTFB_IR_INPUTMODE_FILE}; \
	fi; \
	$(RM) ${DIRECTFB_IR_INPUTMODE_FILE}.$$$$; \
	$(CAT) ${DIRECTFB_BRCM_HEADER_FILE} > ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "/* This file has been auto-generated by the Broadcom DirectFB Makefile - do NOT delete! */" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "#ifndef __BCMNEXUS_IR_CONVERT_H__" >>  ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "#define __BCMNEXUS_IR_CONVERT_H__" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "const NEXUS_IrInputMode " >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "DFB_Platform_P_DFBtoNexusIrModeTable[] =" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "{" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	for ir in `$(CPP) -dD ${BRCM_CFLAGS} "${NEXUS_IR_INPUT_H}" | \
	$(SED) -ne "/^typedef enum NEXUS_IrInputMode/,/} NEXUS_IrInputMode/p" | $(SED) -ne "/NEXUS_IrInputMode_e/p" | \
	$(SED) -e "/NEXUS_IrInputMode_eMax/d" | $(CUT) -d, -f1|cut -d= -f1 | $(SED) -e "s/NEXUS_IrInputMode_e//g"`; do \
	    $(ECHO) "    [DFB_PlatformIrInputMode_e$$ir] = NEXUS_IrInputMode_e$$ir," >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	       done; \
	$(ECHO) "};" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "const DFB_PlatformIrInputMode " >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "DFB_Platform_P_NexustoDFBIrModeTable[] =" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "{" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	for ir in `$(CPP) -dD ${BRCM_CFLAGS} "${NEXUS_IR_INPUT_H}" | \
	$(SED) -ne "/^typedef enum NEXUS_IrInputMode/,/} NEXUS_IrInputMode/p" | $(SED) -ne "/NEXUS_IrInputMode_e/p" | \
	$(SED) -e "/NEXUS_IrInputMode_eMax/d" | $(CUT) -d, -f1|cut -d= -f1 | $(SED) -e "s/NEXUS_IrInputMode_e//g"`; do \
	    $(ECHO) "    [NEXUS_IrInputMode_e$$ir] = DFB_PlatformIrInputMode_e$$ir," >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	       done; \
	$(ECHO) "};" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	$(ECHO) "#endif /* __BCMNEXUS_IR_CONVERT_H__ */" >> ${DIRECTFB_IR_CONVERT_FILE}.$$$$; \
	if [ -e "${DIRECTFB_IR_CONVERT_FILE}" ]; then \
		if ($(DIFF) ${DIRECTFB_IR_CONVERT_FILE} ${DIRECTFB_IR_CONVERT_FILE}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_IR_CONVERT_FILE}.$$$$ ${DIRECTFB_IR_CONVERT_FILE}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_IR_CONVERT_FILE}.$$$$ ${DIRECTFB_IR_CONVERT_FILE}; \
	fi; \
	$(RM) ${DIRECTFB_IR_CONVERT_FILE}.$$$$
endef
