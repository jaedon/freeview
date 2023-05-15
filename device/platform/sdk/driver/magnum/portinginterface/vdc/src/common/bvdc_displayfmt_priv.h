/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_displayfmt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/71 $
 * $brcm_Date: 10/15/12 4:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_displayfmt_priv.h $
 * 
 * Hydra_Software_Devel/71   10/15/12 4:01p tdo
 * SW7425-977: refactor display format info usage in HDMI path programming
 * for 4kx2k format support
 *
 * Hydra_Software_Devel/70   8/10/12 3:27p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/69   2/16/12 1:56p darnstein
 * SW7358-227:  Work on this JIRA left breakage in 7405, 7400, 7468, and
 * 7408.
 *
 * Hydra_Software_Devel/68   2/16/12 11:30a yuxiaz
 * SW7358-227: Enable VF_ENABLE for 7405, 7400, 7468 and 7408.
 *
 * Hydra_Software_Devel/67   2/13/12 7:04p darnstein
 * SW7358-227: handle chips that do not have register
 * VF_n.NEG_SYNC_AMPLITUDE_EXTN.
 *
 * Hydra_Software_Devel/66   2/13/12 4:21p darnstein
 * SW7358-227: still not programming VF_n.NEG_SYNC_AMPLITUDE_EXTN
 * correctly.
 *
 * Hydra_Software_Devel/65   11/9/11 3:06p darnstein
 * SW7425-1658: finish programming VF NEG_SYNC amplitude (extension,
 * VALUE2).
 *
 * Hydra_Software_Devel/64   7/18/11 5:32p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/63   6/24/11 3:48p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/62   5/24/11 7:31p albertl
 * SW7425-169: Converted 10-bit and 8-bit filters to compact hex format
 * and updated to use latest recommended filter coefficients.
 *
 * Hydra_Software_Devel/61   2/23/11 3:50p yuxiaz
 * SW7346-85, SW7422-51: Treat ED formats as SD for TNT settings.
 *
 * Hydra_Software_Devel/60   12/16/10 11:40p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/59   12/1/10 6:10p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/58   11/3/10 1:42p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/57   8/26/10 5:25p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/56   7/13/10 11:28a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/55   6/28/10 2:27p darnstein
 * SW7335-696: Work-around for 7335 video shift issue.
 *
 * Hydra_Software_Devel/54   6/24/10 3:07p yuxiaz
 * SW7405-4549, SW7420-543:  No Audio or video on HDMI outputs for 480p60.
 *
 * Hydra_Software_Devel/53   5/19/10 11:35a pntruong
 * SW7400-2769: To keep ntsc(cvbs/svideo) as 59.94hz and 480i(component)
 * can be 59.94hz or 60.00hz.
 *
 * Hydra_Software_Devel/52   5/11/10 3:19p darnstein
 * SW7340-174: DVI_DTG_0_DTG_CONFIG.AUTO_RESTART only depends on
 * DVI_DTG_0_DTG_CONFIG.SLAVE_MODE now.
 *
 * Hydra_Software_Devel/51   3/1/10 3:51p darnstein
 * SW7400-2620,SW7400-2700: Integrate Nutan's new microcode for 480P. This
 * fixes the 480P "drop lines" problem. Also, DVI-DTG toggle settings are
 * now associated with individual microcode arrays. A new function
 * BVDC_P_GetDviDtgToggles_isr() manages this. When all microcode arrays
 * use the same settings, this function can be removed again.
 *
 * Hydra_Software_Devel/50   2/23/10 3:07p darnstein
 * SW7400-2620: add support for double rate 480P.
 *
 * Hydra_Software_Devel/49   2/1/10 6:37p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/2   1/14/10 5:18p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: fixed build error due to the
 * changes on other mainline VDC code
 *
 * Hydra_Software_Devel/48   1/11/10 1:17p rpan
 * SW7400-2620: Undo HDMI pixel replication. The 7400/7405 HDMI core
 * doesn't support this feature.
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:08p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/47   12/11/09 3:42p rpan
 * SW7400-2620: Added HDMI pixel replication for 480p display. This is for
 * chips such as 7400 and 7405 only.
 *
 * Hydra_Software_Devel/46   10/16/09 5:22p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/45   10/15/09 4:57p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/44   10/13/09 5:48p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/43   5/28/09 5:57p darnstein
 * PR48884: BVDC_P_GetRmTable_isr() no longer modifies its DisplayInfo
 * argument. Instead, it returns a new agrument with the modified data.
 * There are two consequences:
 * 1. When BVDC_P_GetRmTable_isr() is called, a DisplayInfo may have to be
 * modified by the caller.
 * 2. BVDC_P_HdmiRmTableEx_isr() gets a new argument, the same new
 * argument as BVDC_P_GetRmTable_isr().
 *
 * Hydra_Software_Devel/42   4/24/09 3:24p rpan
 * PR54567: Removed the policy of which video formats allow decimation.
 * Let VDC PI caller enforce it.
 *
 * Hydra_Software_Devel/41   4/23/09 3:00p rpan
 * PR54437: Merged 7420 version bvdc_displayfmt_priv.c/h back to mainline.
 * Updated digitial 480p@60hz, 576p@50hz, and 720p@50hz ucode with the
 * updated checksum and date.
 *
 * Hydra_Software_Devel/40   2/20/09 9:02p albertl
 * PR51940: Changed display matrix min max values to allow conversion for
 * different platforms.  Incorporated interface changes of
 * BVDC_P_Display_GetCscTable_isr from 7420.
 *
 * Hydra_Software_Devel/39   11/19/08 6:09p pntruong
 * PR49412: Coverity Defect ID:11584 DEADCODE bvdc_display.c.
 *
 * Hydra_Software_Devel/38   11/10/08 3:03p tdo
 * PR46484: Fix VF_FORMAT_ADDER settings
 *
 * Hydra_Software_Devel/37   11/7/08 5:35p darnstein
 * PR46484: Add functions to print out contents of some important register
 * value tables. Must be activated with #define BVDC_P_DISPLAY_DUMP.
 *
 * Hydra_Software_Devel/36   11/7/08 1:40p pntruong
 * PR45239, PR48758: Added 1080i/1080p/720p@ 60hz LVDS output support.
 * Updated respective digital ucode to be able to run in master mode.
 *
 * Hydra_Software_Devel/35   11/6/08 11:21p tdo
 * PR46484: Roll back previous check in
 * Hydra_Software_Devel/34   11/6/08 11:11p tdo
 * PR46484: Fix VF_FORMAT_ADDER settings
 *
 * Hydra_Software_Devel/33   11/5/08 5:12p pntruong
 * PR45239: Added temporary support for DVI master mode 1080p/720p/1080i.
 *
 * Hydra_Software_Devel/32   10/23/08 9:45p tdo
 * PR46484: Fix compiling error
 *
 * Hydra_Software_Devel/31   10/23/08 9:39p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/30   8/11/08 8:57p pntruong
 * PR45333: Set correct default pedestal settings according programming
 * spreadsheet.  Default sync on master channel instead of all-channel
 * for non-pc input.
 *
 * Hydra_Software_Devel/29   8/6/08 4:45p pntruong
 * PR45219:  Added new RGB formats.
 *
 * Hydra_Software_Devel/28   8/5/08 10:42p pntruong
 * PR45214: Enforced autodetection list.
 *
 * Hydra_Software_Devel/PR45214/1   8/4/08 11:57p mlei
 * PR45214:add the feature that user can customize the format for auto
 * detection
 *
 * Hydra_Software_Devel/27   5/1/08 2:39p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/26   3/27/08 5:07p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/25   3/11/08 10:46p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/24   3/10/08 9:11p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/23   1/13/08 3:29a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/22   10/30/07 7:15p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/21   10/15/07 3:16p pntruong
 * PR35137: Add NTSC443 support for cvbs/Svideo.
 *
 * Hydra_Software_Devel/20   8/30/07 4:25p darnstein
 * PR33711: declare static tables to be const. This is required by
 * Broadcom's programming standards.
 *
 * Hydra_Software_Devel/19   6/28/07 7:26p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/1   6/26/07 2:22p hongtaoz
 * PR32130: added window game mode buffer delay control;
 *
 * Hydra_Software_Devel/18   3/7/07 10:54a maivu
 * PR 27609: Add DVI support for 7400B0
 *
 * Hydra_Software_Devel/17   1/16/07 4:19p hongtaoz
 * PR23260, PR23280, PR23204: added 59.94 and 50Hz 1080p panel custom
 * format support;
 *
 * Hydra_Software_Devel/16   12/18/06 11:38p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   12/13/06 5:22p hongtaoz
 * PR26039: updated 480i/1080i/720p analog microcode for the new shaper
 * settings;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   10/9/06 2:39p hongtaoz
 * PR23196, PR24727: add DVO RM support for 3563; add 1600x1200p (162MHz)
 * pixel clock RM table;
 *
 * Hydra_Software_Devel/14   9/14/06 6:52p pntruong
 * PR23222, PR23225: Support VDEC ATSC & PC formats.  Added fields to
 * internal format look up table to make distinction about standard
 * definition (SD), extended definition (ED), and high definition (HD) as
 * many fields in vdec are base on these fields.
 *
 * Hydra_Software_Devel/12   8/16/06 3:28p pntruong
 * PR23522: Add supporting hdmi pc input 800x600@56Hz and 800x600@85Hz
 *
 * Hydra_Software_Devel/11   6/30/06 5:44p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/10   6/19/06 6:14p hongtaoz
 * PR22081: add SECAM support for dvi output;
 *
 * Hydra_Software_Devel/9   5/19/06 1:47p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/8   5/10/06 2:58p jessem
 * PR 21447: Added ulVertRateTime to BVDC_P_FormatInfo. Added a table of
 * VSYNC time in usecs as a counterpart to VSYNC rate in Hz. This is
 * needed to determine if time between VSYNC isrs exceeds 1 field or
 * frame time.
 *
 * Hydra_Software_Devel/7   4/7/06 10:33a yuxiaz
 * PR20598: Add support to break larger buffer into smaller buffers in
 * VDC.
 *
 * Hydra_Software_Devel/6   2/21/06 4:26p yuxiaz
 * PR19258: Need to increase memory for 1366x768 case. Added
 * eBufferFormat_HD in BVDC_Settings.
 *
 * Hydra_Software_Devel/5   2/9/06 4:15p pntruong
 * PR19270: HDCP glitches at wrap of Sarnoff 1080i DYNPICTS.TRP stream.
 * Update hdmi rate manager in vertical blanking to avoid glitches.
 *
 * Hydra_Software_Devel/3   9/30/05 4:24p albertl
 * PR17320:  Reenabled autorestart for the formats that need them,
 * accidentally removed in code restructuring.
 *
 * Hydra_Software_Devel/2   9/22/05 11:02a pntruong
 * PR16459: Fixed macrovision build errors.
 *
 * Hydra_Software_Devel/1   9/21/05 7:51p albertl
 * PR16459:  Initial Revision.  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 ***************************************************************************/
#ifndef BVDC_DISPLAYFMT_PRIV_H__
#define BVDC_DISPLAYFMT_PRIV_H__

#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_display_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *  Defines
 ****************************************************************/
#define BVDC_P_480i_DROP_TABLE_SIZE      2
#define BVDC_P_480p_DROP_TABLE_SIZE      4
#define BVDC_P_480i_DROP_LINES_MAX       2
#define BVDC_P_480p_DROP_LINES_MAX       3

#ifdef BVDC_TEST_FORMAT
	/* update below macros */
#endif

/****************************************************************
 *  Macros
 ****************************************************************/
/* Video format is PAL */
#define VIDEO_FORMAT_IS_PAL_BB1D1G(fmt) \
	((BFMT_VideoFmt_ePAL_B == (fmt)) || (BFMT_VideoFmt_ePAL_B1 == (fmt)) || \
	(BFMT_VideoFmt_ePAL_D1 == (fmt)) || (BFMT_VideoFmt_ePAL_G == (fmt)))

#define VIDEO_FORMAT_IS_PAL(fmt) \
	(VIDEO_FORMAT_IS_PAL_BB1D1G(fmt) || (BFMT_VideoFmt_ePAL_H == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_I == (fmt)) || (BFMT_VideoFmt_ePAL_D == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_K == (fmt)) || (BFMT_VideoFmt_ePAL_M == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_N == (fmt)) || (BFMT_VideoFmt_ePAL_NC == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_60 == (fmt)))

#define VIDEO_FORMAT_IS_PAL_443(fmt) \
	(VIDEO_FORMAT_IS_PAL_BB1D1G(fmt) || (BFMT_VideoFmt_ePAL_H == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_I == (fmt)) || (BFMT_VideoFmt_ePAL_D == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_K == (fmt)) ||  (BFMT_VideoFmt_ePAL_N == (fmt)))

/* Video format is NTSC-J for Japan */
#define VIDEO_FORMAT_IS_NTSC_J(fmt) \
	((BFMT_VideoFmt_eNTSC_J == (fmt)) || (BFMT_VideoFmt_e720x482_NTSC_J == (fmt)))

/* Video format is NTSC-M for North America */
#define VIDEO_FORMAT_IS_NTSC_M(fmt) \
	((BFMT_VideoFmt_eNTSC == (fmt)) || (BFMT_VideoFmt_e720x482_NTSC == (fmt)) || \
	 (BFMT_VideoFmt_eNTSC_443 == (fmt)))

/* Video format is NTSC */
#define VIDEO_FORMAT_IS_NTSC(fmt) \
	(VIDEO_FORMAT_IS_NTSC_J(fmt) || VIDEO_FORMAT_IS_NTSC_M(fmt))

/* Video format is NTSC */
#define VIDEO_FORMAT_IS_480P(fmt) \
	((BFMT_VideoFmt_e480p == (fmt)) || (BFMT_VideoFmt_e720x483p == (fmt)))

/* Video format is SECAM */
#define VIDEO_FORMAT_IS_SECAM(fmt) \
	((BFMT_VideoFmt_eSECAM_B == (fmt)) || (BFMT_VideoFmt_eSECAM_D == (fmt)) || \
	 (BFMT_VideoFmt_eSECAM_G == (fmt)) || (BFMT_VideoFmt_eSECAM_H == (fmt)) || \
	 (BFMT_VideoFmt_eSECAM_K == (fmt)) || (BFMT_VideoFmt_eSECAM_L == (fmt)))

/* 525-lines video format */
#define VIDEO_FORMAT_IS_525_LINES(fmt) \
	(VIDEO_FORMAT_IS_NTSC(fmt) || \
	 (BFMT_VideoFmt_ePAL_M == (fmt)) || (BFMT_VideoFmt_ePAL_60 == (fmt)))

/* 625-lines video format */
#define VIDEO_FORMAT_IS_625_LINES(fmt) \
	(VIDEO_FORMAT_IS_PAL_BB1D1G(fmt)|| (BFMT_VideoFmt_ePAL_H == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_I == (fmt)) || (BFMT_VideoFmt_ePAL_D == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_K == (fmt)) || (BFMT_VideoFmt_ePAL_N == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_NC == (fmt)) || VIDEO_FORMAT_IS_SECAM(fmt))

#if BFMT_LEGACY_3DTV_SUPPORT
/* 3D 1080p video formats */
#define VIDEO_FORMAT_IS_CUSTOM_1080P3D(fmt) \
	((BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz == (fmt)) || \
	 (BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz == (fmt)))

/* 3D 1080p video formats that too large and require legacy BVN to take
 * TOP in Over/Under 3d format. */
#define VIDEO_FORMAT_IS_1080P_3DOU_AS(fmt) \
	((BFMT_VideoFmt_e1080p_24Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_30Hz_3DOU_AS == (fmt)))
#endif

/* Special format with for DCR/Blank level */
#define VIDEO_FORMAT_IS_SPECIAL_BLANKLEVEL(fmt) \
	(VIDEO_FORMAT_IS_NTSC(fmt) || \
	 (BFMT_VideoFmt_ePAL_M == fmt) || (BFMT_VideoFmt_ePAL_N == fmt) || \

/* Video format is PROGRESSIVE */
#define VIDEO_FORMAT_IS_PROGRESSIVE(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bProgressive)

/* Video format is SD */
#define VIDEO_FORMAT_IS_SD(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bSd)

#define VIDEO_FORMAT_IS_ED(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bEd)

/* Video format is HD (inlusion of Ed and Hd). */
#define VIDEO_FORMAT_IS_HD(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bHd || BVDC_P_aFormatInfoTable[fmt].bEd)

/* Video format is HD (inlusion of Ed and Hd). */
#define VIDEO_FORMAT_IS_VESA(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bVesa)

/* Video format color space*/
#define VIDEO_FORMAT_CMP_COLORSPACE(fmt, xvycc) \
	( (VIDEO_FORMAT_IS_NTSC(fmt) || VIDEO_FORMAT_IS_480P(fmt)) ? ((xvycc) ? BVDC_P_CmpColorSpace_eXvYcc_601 : BVDC_P_CmpColorSpace_eNtscSdYCrCb) : \
	  ((VIDEO_FORMAT_IS_PAL(fmt) || (BFMT_VideoFmt_e576p_50Hz == fmt)) ? BVDC_P_CmpColorSpace_ePalSdYCrCb : \
	  ((BFMT_VideoFmt_eCustom0 == fmt) || (BFMT_VideoFmt_eCustom0 == fmt) || (BFMT_VideoFmt_eCustom0 == fmt)) ? BVDC_P_CmpColorSpace_eCustom : \
	  BVDC_P_CmpColorSpace_eHdYCrCb ))

/* Video format color space index */
#define VIDEO_FORMAT_COLOR_SPACE_IDX(fmt, xvycc) \
	( (VIDEO_FORMAT_IS_NTSC(fmt) || VIDEO_FORMAT_IS_480P(fmt)) ? ((xvycc) ? BVDC_P_CmpColorSpace_eXvYcc_601 : BVDC_P_CmpColorSpace_eNtscSdYCrCb) : \
	  (VIDEO_FORMAT_IS_PAL(fmt) || (BFMT_VideoFmt_e576p_50Hz == fmt)) ? BVDC_P_CmpColorSpace_ePalSdYCrCb : \
	  BVDC_P_CmpColorSpace_eHdYCrCb )

/* Video format runs @ 27Mhz */
#define VIDEO_FORMAT_27Mhz(pxl_freq_bit_mask) \
	(BFMT_PXL_27MHz & (pxl_freq_bit_mask))

/* Video format supports HDMI */
#define VIDEO_FORMAT_IS_HDMI(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bHdmi)

/* Video format supports macrovision */
#define VIDEO_FORMAT_SUPPORTS_MACROVISION(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bMacrovision)

/* Video format supports DCS */
#define VIDEO_FORMAT_SUPPORTS_DCS(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bDcs)

/* Video format supports drop (line) table */
#define VIDEO_FORMAT_SUPPORTS_DROP_LINE(fmt) \
	(BVDC_P_aFormatInfoTable[fmt].bUseDropTbl)

/* Register manipulation helper macros */
#if BVDC_P_ORTHOGONAL_VEC
#define BVDC_P_SRC_FIELD_ENUM(Register, Field, Name) \
 	BCHP_FIELD_ENUM(SDSRC_0_##Register,Field, Name)

#define BVDC_P_IT_FIELD_ENUM(Register, Field, Name) \
 	BCHP_FIELD_ENUM(IT_0_##Register,Field, Name)

#define BVDC_P_IT_FIELD_DATA(Register, Field, Data) \
 	BCHP_FIELD_DATA(IT_0_##Register,Field, Data)

#define BVDC_P_RM_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(RM_0_##Register,Field, Data)

#define BVDC_P_VF_FIELD_ENUM(Register, Field, Name) \
	BCHP_FIELD_ENUM(VF_0_##Register,Field, Name)

#define BVDC_P_VF_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(VF_0_##Register,Field, Data)

#define BVDC_P_SM_FIELD_ENUM(Register, Field, Name) \
	BCHP_FIELD_ENUM(SM_0_##Register,Field, Name)

#define BVDC_P_SM_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(SM_0_##Register,Field, Data)
#else
#define BVDC_P_SRC_FIELD_ENUM(Register, Field, Name) \
	BCHP_FIELD_ENUM(PRIM_SRC_##Register,Field, Name)

#define BVDC_P_IT_FIELD_ENUM(Register, Field, Name) \
 	BCHP_FIELD_ENUM(PRIM_IT_##Register,Field, Name)

#define BVDC_P_IT_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(PRIM_IT_##Register,Field, Data)

#define BVDC_P_RM_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(PRIM_RM_##Register,Field, Data)

#define BVDC_P_VF_FIELD_ENUM(Register, Field, Name) \
	BCHP_FIELD_ENUM(PRIM_VF_##Register,Field, Name)

#define BVDC_P_VF_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(PRIM_VF_##Register,Field, Data)

#define BVDC_P_SM_FIELD_ENUM(Register, Field, Name) \
	BCHP_FIELD_ENUM(PRIM_SM_##Register,Field, Name)

#define BVDC_P_SM_FIELD_DATA(Register, Field, Data) \
	BCHP_FIELD_DATA(PRIM_SM_##Register,Field, Data)
#endif


/***************************************************************************/

#define BVDC_P_MAKE_IT_ADDR(mc3, mc2, mc1, mc0, mc6, mc5, mc4) \
	BVDC_P_IT_FIELD_DATA(ADDR_0_3,MC_3_START_ADDR, mc3) |   \
	BVDC_P_IT_FIELD_DATA(ADDR_0_3,MC_2_START_ADDR, mc2) |   \
	BVDC_P_IT_FIELD_DATA(ADDR_0_3,MC_1_START_ADDR, mc1) |   \
	BVDC_P_IT_FIELD_DATA(ADDR_0_3,MC_0_START_ADDR, mc0),    \
	BVDC_P_IT_FIELD_DATA(ADDR_4_6,MC_6_START_ADDR, mc6) |   \
	BVDC_P_IT_FIELD_DATA(ADDR_4_6,MC_5_START_ADDR, mc5) |   \
	BVDC_P_IT_FIELD_DATA(ADDR_4_6,MC_4_START_ADDR, mc4)

#define BVDC_P_MAKE_IT_STACK(reg1, reg0, reg3, reg2, reg5, reg4, reg7, reg6) \
	BVDC_P_IT_FIELD_DATA(STACK_reg_0_1, REG_1, reg1) | \
	BVDC_P_IT_FIELD_DATA(STACK_reg_0_1, REG_0, reg0),  \
	BVDC_P_IT_FIELD_DATA(STACK_reg_2_3, REG_3, reg3) | \
	BVDC_P_IT_FIELD_DATA(STACK_reg_2_3, REG_2, reg2),  \
	BVDC_P_IT_FIELD_DATA(STACK_reg_4_5, REG_5, reg5) | \
	BVDC_P_IT_FIELD_DATA(STACK_reg_4_5, REG_4, reg4),  \
	BVDC_P_IT_FIELD_DATA(STACK_reg_6_7, REG_7, reg7) | \
	BVDC_P_IT_FIELD_DATA(STACK_reg_6_7, REG_6, reg6)

#define BVDC_P_MAKE_IT_EVENT(mc6, mc5, mc4, mc3, mc2, mc1) \
	BVDC_P_IT_FIELD_DATA(EVENT_SELECTION, MC_6, mc6) | \
	BVDC_P_IT_FIELD_DATA(EVENT_SELECTION, MC_5, mc5) | \
	BVDC_P_IT_FIELD_DATA(EVENT_SELECTION, MC_4, mc4) | \
	BVDC_P_IT_FIELD_DATA(EVENT_SELECTION, MC_3, mc3) | \
	BVDC_P_IT_FIELD_DATA(EVENT_SELECTION, MC_2, mc2) | \
	BVDC_P_IT_FIELD_DATA(EVENT_SELECTION, MC_1, mc1)

#define BVDC_P_MAKE_IT_PCL_0(mux_sel, term_4, term_3, term_2, term_1,     \
	term_0, mux_e, mux_d, mux_4, mux_3)                                   \
	BVDC_P_IT_FIELD_ENUM(PCL_0, VBI_DATA_ACTIVE_ENABLE,          ON) | \
	BVDC_P_IT_FIELD_DATA(PCL_0, VBI_DATA_ACTIVE_MUX_SELECT, mux_sel) | \
	BVDC_P_IT_FIELD_ENUM(PCL_0, NEGSYNC_AND_TERM_4 ,         term_4) | \
	BVDC_P_IT_FIELD_ENUM(PCL_0, NEGSYNC_AND_TERM_3 ,         term_3) | \
	BVDC_P_IT_FIELD_ENUM(PCL_0, NEGSYNC_AND_TERM_2 ,         term_2) | \
	BVDC_P_IT_FIELD_ENUM(PCL_0, NEGSYNC_AND_TERM_1 ,         term_1) | \
	BVDC_P_IT_FIELD_ENUM(PCL_0, NEGSYNC_AND_TERM_0 ,         term_0) | \
	BVDC_P_IT_FIELD_DATA(PCL_0, NEGSYNC_MUX_E_SELECT,         mux_e) | \
	BVDC_P_IT_FIELD_DATA(PCL_0, NEGSYNC_MUX_D_SELECT,         mux_d) | \
	BVDC_P_IT_FIELD_DATA(PCL_0, NEGSYNC_MUX_4_SELECT,         mux_4) | \
	BVDC_P_IT_FIELD_DATA(PCL_0, NEGSYNC_MUX_3_SELECT,         mux_3)

#define BVDC_P_MAKE_IT_PCL_1(term_2, term_1, term_0)                     \
	BVDC_P_IT_FIELD_DATA(PCL_1, reserved0,                0)        | \
	BVDC_P_IT_FIELD_ENUM(PCL_1, BOTTLES_ENABLE,           DISABLED) | \
	BVDC_P_IT_FIELD_DATA(PCL_1, BOTTLES_MUX_A_SELECT,     2)        | \
	BVDC_P_IT_FIELD_DATA(PCL_1, BOTTLES_MUX_0_SELECT,     2)        | \
	BVDC_P_IT_FIELD_DATA(PCL_1, reserved1,                0)        | \
	BVDC_P_IT_FIELD_ENUM(PCL_1, COLOR_BURST_AND_TERM_2 ,  term_2)   | \
	BVDC_P_IT_FIELD_ENUM(PCL_1, COLOR_BURST_AND_TERM_1 ,  term_1)   | \
	BVDC_P_IT_FIELD_ENUM(PCL_1, COLOR_BURST_AND_TERM_0 ,  term_0)   | \
	BVDC_P_IT_FIELD_DATA(PCL_1, COLOR_BURST_MUX_C_SELECT, 1)        | \
	BVDC_P_IT_FIELD_DATA(PCL_1, COLOR_BURST_MUX_B_SELECT, 0)        | \
	BVDC_P_IT_FIELD_DATA(PCL_1, COLOR_BURST_MUX_2_SELECT, 2)        | \
	BVDC_P_IT_FIELD_DATA(PCL_1, COLOR_BURST_MUX_1_SELECT, 0)

#define BVDC_P_MAKE_IT_PCL_2_3(v_flip_en, v_mux_a, v_mux_0,                \
	sec_neg_sync, u_flip_en, u_mux_a, u_mux_0, new_line)                   \
	BVDC_P_IT_FIELD_DATA(PCL_2, reserved0 ,                        0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_2, V_FLIP_ENABLE,             v_flip_en) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, V_FLIP_MUX_A_SELECT ,        v_mux_a) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, V_FLIP_MUX_0_SELECT ,        v_mux_0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_2, SEC_NEG_SYNC_ENABLE,    sec_neg_sync) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, SEC_NEG_SYNC_MUX_A_SELECT,         0) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, SEC_NEG_SYNC_MUX_0_SELECT,         0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_2, EXT_VSYNC_ENABLE,                 ON) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, EXT_VSYNC_MUX_SELECT,              0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_2, EXT_HSYNC_ENABLE,                 ON) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, EXT_HSYNC_MUX_SELECT,              0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_2, U_FLIP_ENABLE,             u_flip_en) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, U_FLIP_MUX_A_SELECT ,        u_mux_a) | \
	BVDC_P_IT_FIELD_DATA(PCL_2, U_FLIP_MUX_0_SELECT ,        u_mux_0),  \
	BVDC_P_IT_FIELD_DATA(PCL_3, reserved0,                      0)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, LINE_COUNT_CLEAR_ENABLE,       ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, LINE_COUNT_CLEAR_SELECT,        1)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, NEW_LINE_ENABLE,               ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, NEW_LINE_MUX_SELECT,     new_line)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, V_ACTIVE_ENABLE,               ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, V_ACTIVE_MUX_SELECT,            1)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, H_ACTIVE_ENABLE,               ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, H_ACTIVE_MUX_SELECT,            1)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, ODD_EVEN_ENABLE,               ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, ODD_EVEN_MUX_SELECT,            3)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, VSYNC_ENABLE,                  ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, VSYNC_MUX_SELECT,               0)    | \
	BVDC_P_IT_FIELD_ENUM(PCL_3, VBLANK_ENABLE,                 ON)    | \
	BVDC_P_IT_FIELD_DATA(PCL_3, VBLANK_MUX_SELECT,              3)

#define BVDC_P_MAKE_IT_PCL_4(psa_1, psa_0, psa_mux_b, psa_mux_a,  \
	psa_mux_1, psa_mux_0, psb_2, psb_1, psb_0, psb_mux_c,         \
	psb_mux_b, psb_mux_2, psb_mux_1)                              \
	BVDC_P_IT_FIELD_DATA(PCL_4, reserved0,                0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSA_AND_TERM_1,       psa_1) | \
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSA_AND_TERM_0,       psa_0) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_B_SELECT, psa_mux_b) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_A_SELECT, psa_mux_a) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_1_SELECT, psa_mux_1) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_0_SELECT, psa_mux_0) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, reserved1,                0) | \
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSB_AND_TERM_2,       psb_2) | \
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSB_AND_TERM_1,       psb_1) | \
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSB_AND_TERM_0,       psb_0) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_C_SELECT, psb_mux_c) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_B_SELECT, psb_mux_b) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_2_SELECT, psb_mux_2) | \
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_1_SELECT, psb_mux_1)

#define BVDC_P_MAKE_IT_TG(cycle_cnt, mc_en, line_phase)                    \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, TRIGGER_CNT_CLR_COND,          0) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, BVB_FRAME_CYCLE_COUNT, cycle_cnt) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, BVB_PHASE_SYNC,                0) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, MC_VIDEO_STREAM_SELECT,        0) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, INPUT_STREAM_ENABLE,           1) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, MC_ENABLES,                mc_en) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, SLAVE_MODE,                    0) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, LINE_PHASE,           line_phase) | \
	BVDC_P_IT_FIELD_DATA(TG_CONFIG, ARBITER_LATENCY,              11)

#define BVDC_P_MAKE_VF_FORMAT_ADDER(nsync, mode, c2_psync, c2_comp, c2_offset, \
	c1_psync, c1_comp, c1_offset, c0_psync, c0_comp, c0_sync, sync, offset)    \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, reserved0,                0) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, SECOND_NEGATIVE_SYNC, nsync) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, CLAMP_MODE,            mode) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C2_POSITIVESYNC,   c2_psync) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C2_COMP,            c2_comp) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C2_OFFSET,        c2_offset) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C1_POSITIVESYNC,   c1_psync) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C1_COMP,            c1_comp) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C1_OFFSET,        c1_offset) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C0_POSITIVESYNC,   c0_psync) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C0_COMP,            c0_comp) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, C0_SYNC,            c0_sync) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, ADD_SYNC_TO_OFFSET,    sync) | \
	BVDC_P_VF_FIELD_DATA(FORMAT_ADDER, OFFSET,              offset)

#ifdef BVDC_P_WSE_VER3 /** { **/
#define BVDC_P_MAKE_VF_MISC(sum_taps, upsample, sav_remove, vbi_pre, vbi_en) \
	BVDC_P_VF_FIELD_DATA(MISC, reserved0,               0) | \
	BVDC_P_VF_FIELD_ENUM(MISC, VF_ENABLE,              ON) | \
	BVDC_P_VF_FIELD_ENUM(MISC, SUM_OF_TAPS,      sum_taps) | \
	BVDC_P_VF_FIELD_ENUM(MISC, UPSAMPLE2X,       upsample) | \
	BVDC_P_VF_FIELD_DATA(MISC, BVB_SAV_REMOVE, sav_remove) | \
	BVDC_P_VF_FIELD_ENUM(MISC, VBI_PREFERRED,     vbi_pre) | \
	BVDC_P_VF_FIELD_ENUM(MISC, VBI_ENABLE,            OFF) | \
	BVDC_P_VF_FIELD_ENUM(MISC, C2_RAMP,      SYNC_TRANS_1) | \
	BVDC_P_VF_FIELD_ENUM(MISC, C1_RAMP,      SYNC_TRANS_1) | \
	BVDC_P_VF_FIELD_ENUM(MISC, C0_RAMP,      SYNC_TRANS_0)
#else /** } { **/
#define BVDC_P_MAKE_VF_MISC(sum_taps, upsample, sav_remove, vbi_pre, vbi_en) \
	BVDC_P_VF_FIELD_DATA(MISC, reserved0,               0) | \
	BVDC_P_VF_FIELD_ENUM(MISC, VF_ENABLE,              ON) | \
	BVDC_P_VF_FIELD_ENUM(MISC, SUM_OF_TAPS,      sum_taps) | \
	BVDC_P_VF_FIELD_ENUM(MISC, UPSAMPLE2X,       upsample) | \
	BVDC_P_VF_FIELD_DATA(MISC, BVB_SAV_REMOVE, sav_remove) | \
	BVDC_P_VF_FIELD_ENUM(MISC, VBI_PREFERRED,     vbi_pre) | \
	BVDC_P_VF_FIELD_ENUM(MISC, VBI_ENABLE,         vbi_en) | \
	BVDC_P_VF_FIELD_ENUM(MISC, C2_RAMP,      SYNC_TRANS_1) | \
	BVDC_P_VF_FIELD_ENUM(MISC, C1_RAMP,      SYNC_TRANS_1) | \
	BVDC_P_VF_FIELD_ENUM(MISC, C0_RAMP,      SYNC_TRANS_0)
#endif /** } BVDC_P_WSE_VER3 **/

#if (BVDC_P_SUPPORT_VEC_VF_VER >= 2)
	#define BVDC_P_MAKE_VF_SYNC(n2, n1, n0, p1, p0)         \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE1, n1) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE0, n0),  \
		BVDC_P_VF_FIELD_DATA(POS_SYNC_VALUES, VALUE1, p1) | \
		BVDC_P_VF_FIELD_DATA(POS_SYNC_VALUES, VALUE0, p0)
	#define BVDC_P_MAKE_VF_SYNC_EXTN(seld,n3,n2) \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_AMPLITUDE_EXTN, SEL_D_SYNC, seld) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_AMPLITUDE_EXTN,     VALUE3,   n3) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_AMPLITUDE_EXTN,     VALUE2,   n2)
#elif (BVDC_P_SUPPORT_VEC_VF_VER == 1)
	#define BVDC_P_MAKE_VF_SYNC(n2, n1, n0, p1, p0)         \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE2, n2) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE1, n1) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE0, n0),  \
		BVDC_P_VF_FIELD_DATA(POS_SYNC_VALUES, VALUE1, p1) | \
		BVDC_P_VF_FIELD_DATA(POS_SYNC_VALUES, VALUE0, p0)
	#define BVDC_P_MAKE_VF_SYNC_EXTN(seld,n3,n2) \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_AMPLITUDE_EXTN, SEL_D_SYNC, seld) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_AMPLITUDE_EXTN,     VALUE3,   n3)
#else
	#define BVDC_P_MAKE_VF_SYNC(n2, n1, n0, p1, p0)         \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE2, n2) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE1, n1) | \
		BVDC_P_VF_FIELD_DATA(NEG_SYNC_VALUES, VALUE0, n0),  \
		BVDC_P_VF_FIELD_DATA(POS_SYNC_VALUES, VALUE1, p1) | \
		BVDC_P_VF_FIELD_DATA(POS_SYNC_VALUES, VALUE0, p0)
	#define BVDC_P_MAKE_VF_SYNC_EXTN(seld,n3,n2) \
		0
#endif

#define BVDC_P_MAKE_VF_SYNC_TRANS(trans, tap3, tap2, tap1, tap0) \
	BVDC_P_VF_FIELD_DATA(SYNC_TRANS_##trans, TAP3, tap3) | \
	BVDC_P_VF_FIELD_DATA(SYNC_TRANS_##trans, TAP2, tap2) | \
	BVDC_P_VF_FIELD_DATA(SYNC_TRANS_##trans, TAP1, tap1) | \
	BVDC_P_VF_FIELD_DATA(SYNC_TRANS_##trans, TAP0, tap0)


/***************************************************************************
 * Structures
 ***************************************************************************/
typedef struct BVDC_P_FormatInfo
{
	BFMT_VideoFmt               eVideoFmt;
	bool                        bHd; /* High Definition 720p/1080i/PC/HDMI formats */
	bool                        bEd; /* Extended Defination NTSCP/PALP progessive. */
	bool                        bSd; /* Standard Definition NTSC/PAL interlaced */
	bool                        bVesa;/* VESA standard */
	bool                        bProgressive;
	bool                        bMacrovision;
	bool                        bDcs;
	bool                        bHdmi;
	bool                        bUseDropTbl;
} BVDC_P_FormatInfo;

typedef struct BVDC_P_FormatData
{
	BFMT_VideoFmt               eVideoFmt;
	const uint32_t             *pRamBVBInput;
	const uint32_t             *pDtRamBVBInput;
	const uint32_t             * const * apDtRamBVBInput_DropTbl;
	const uint32_t             *pItTable;
	const uint32_t             *pulItConfig;
} BVDC_P_FormatData;

typedef struct BVDC_P_SmTableInfo
{
	BFMT_VideoFmt               eVideoFmt;
	const uint32_t             *pSmTable;
} BVDC_P_SmTableInfo;

typedef struct BVDC_P_FilterTableInfo
{
	BVDC_P_OutputFilter        eOutputFilter;
	const uint32_t             *pChFilter_Ch0;
	const uint32_t             *pChFilter_Ch1;
	const uint32_t             *pChFilter_Ch2;
} BVDC_P_FilterTableInfo;

typedef struct BVDC_P_SrcControlInfo
{
	BFMT_VideoFmt               eVideoFmt;
	const uint32_t             *pulSrcControl;
} BVDC_P_SrcControlInfo;

typedef struct BVDC_P_VfTableInfo
{
	BFMT_VideoFmt               eVideoFmt;
	const uint32_t             *pVfTable;
} BVDC_P_VfTableInfo;

typedef struct BVDC_P_RmTableInfo
{
	uint32_t                    ulPixelClkRate;
	const uint32_t             *pRmTable;
	const char                 *pString;
} BVDC_P_RmTableInfo;

typedef struct BVDC_P_ColorSpaceData
{
	BVDC_P_Output                 eOutputColorSpace;
	const BVDC_P_SmTableInfo     *pSmTable_Tbl;
	const BVDC_P_OutputFilter     eSdOutputFilter;
	const BVDC_P_OutputFilter     eHdOutputFilter;
	const BVDC_P_VfTableInfo     *pSdVfTable_Tbl;
	const BVDC_P_VfTableInfo     *pHdVfTable_Tbl;
} BVDC_P_ColorSpaceData;

#if BVDC_P_ORTHOGONAL_VEC
typedef struct BVDC_P_EnvelopGeneratorSetting
{
	BFMT_VideoFmt               eVideoFmt;
	const uint32_t              ulSetting;
} BVDC_P_EnvelopGeneratorSetting;

typedef struct BVDC_P_EnvelopGeneratorInfo
{
	BVDC_P_Output                          eOutputCS;
	const BVDC_P_EnvelopGeneratorSetting  *pEG_Tbl;
} BVDC_P_EnvelopGeneratorInfo;
#endif

/***************************************************************************
 * Private tables
 ***************************************************************************/
extern const BVDC_P_FormatInfo   BVDC_P_aFormatInfoTable[];


/***************************************************************************
 * Display private functions
 ***************************************************************************/
const uint32_t* BVDC_P_GetRamTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	bool                     bArib480p
);

const uint32_t* BVDC_P_GetRamTableMv_isr
(
#if (!BVDC_P_ORTHOGONAL_VEC)
	BVDC_P_VecPath           eVecPath,
#endif
	const BVDC_P_DisplayInfo *pDispInfo,
	bool                     bArib480p
);

const uint32_t* BVDC_P_GetItTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
);

const uint32_t* BVDC_P_GetItTableMv_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
);

uint32_t BVDC_P_GetItConfig_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
);

uint32_t BVDC_P_GetItConfigMv_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
);

const uint32_t* BVDC_P_GetDtramTable_isr
(
	const BVDC_P_DisplayInfo    *pDispInfo,
	const BFMT_VideoInfo        *pFmtInfo,
	bool                         bArib480p
);

const uint32_t* BVDC_P_Get656DtramTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
);

const uint32_t* BVDC_P_GetSmTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	BVDC_P_Output			  eOutputCS
);

BERR_Code BVDC_P_GetChFilters_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	BVDC_P_Output             eOutputColorSpace,
	const uint32_t          **ppChFilter_Ch0,
	const uint32_t          **ppChFilter_Ch1,
	const uint32_t          **ppChFilter_Ch2
);

uint32_t BVDC_P_GetSrcControl_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
);

void BVDC_P_GetVfTable_isr
(
	const BVDC_P_DisplayInfo            *pDispInfo,
	BVDC_P_Output                        eOutputColorSpace,
	const uint32_t                     **ppTable,
	BVDC_P_Display_ShaperSettings       *pstShaper
);

#if BVDC_P_ORTHOGONAL_VEC
uint32_t BVDC_P_GetVfEnvelopGenerator_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	BVDC_P_Output             eOutputCS
);
#endif

BERR_Code BVDC_P_GetRmTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	const BFMT_VideoInfo     *pFmtInfo,
	const uint32_t          **ppTable,
	bool                      bFullRate,
	BAVC_VdcDisplay_Info     *pRateInfo
);

const char* BVDC_P_GetRmString_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	const BFMT_VideoInfo     *pFmtInfo
);

const BVDC_P_RateInfo* BVDC_P_HdmiRmTable_isr
(
	BFMT_VideoFmt             eVideoFmt,
	uint32_t                  ulPixelClkRate,
	BAVC_HDMI_BitsPerPixel    eHdmiColorDepth,
	BAVC_HDMI_PixelRepetition eHdmiPixelRepetition
);

const BVDC_P_FormatInfo* BVDC_P_GetFormatInfoPtr_isr
(
	const BFMT_VideoInfo    *pFmtInfo
);

void BVDC_P_Display_GetCscTable_isr
	( const BVDC_P_DisplayInfo        *pDispInfo,
	  BVDC_P_Output                    eOutputColorSpace,
	  const BVDC_P_DisplayCscMatrix  **ppCscTable );

void BVDC_P_Display_GetDviCscTable_isr
	( const BVDC_P_DisplayInfo        *pDispInfo,
	  const BVDC_P_DisplayCscMatrix  **ppCscTable );

void BVDC_P_Display_Get656CscTable_isr
	( const BVDC_P_DisplayInfo        *pDispInfo,
	  bool                             bBypass,
	  const BVDC_P_DisplayCscMatrix  **ppCscTable );

/* Returns H, then V. */
const uint32_t* BVDC_P_GetDviDtgToggles_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
);

#ifdef BVDC_P_DISPLAY_DUMP
void BVDC_P_Display_Dump_aulVfTable (const char* name, const uint32_t* table);
void BVDC_P_Display_Dump_aulChFilterTbl (
	const char* name, const uint32_t* table);
void BVDC_P_Display_Dump_aulRmTable (const char* name, const uint32_t* table);
void BVDC_P_Display_Dump_aulItTable (const char* name, const uint32_t* table);
void BVDC_P_Display_Dump_ulItConfig (const char* name, uint32_t value);
void BVDC_P_Display_Dump_aulSmTable (const char* name, const uint32_t* table);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_DISPLAYFMT_PRIV_H__ */
/* End of file. */
