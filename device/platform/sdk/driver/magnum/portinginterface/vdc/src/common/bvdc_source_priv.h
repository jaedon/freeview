/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_source_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/231 $
 * $brcm_Date: 10/12/12 3:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_source_priv.h $
 * 
 * Hydra_Software_Devel/234   5/9/13 5:07p yuxiaz
 * SW7425-4785: Clear rect is not shown for all mosaic windows. Optimize
 * mosaic zorder code.
 * 
 * Hydra_Software_Devel/233   5/8/13 11:47a yuxiaz
 * SW7425-4736: Fixed kernel mode crash on 7468 and 7208.
 * 
 * Hydra_Software_Devel/231   10/12/12 3:26p vanessah
 * SW7445-8: add Cap8 for 7445 and simplify source
 * 
 * Hydra_Software_Devel/230   8/16/12 2:43p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/229   8/9/12 11:20a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 *
 * Hydra_Software_Devel/228   6/14/12 3:44p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/227   5/23/12 6:47p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/226   5/9/12 10:54a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/225   4/12/12 4:18p vanessah
 * SW7425-2831: use the source for the field inversion handling in
 * ignorepicture
 *
 * Hydra_Software_Devel/224   11/30/11 1:47p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/223   9/14/11 11:47a yuxiaz
 * SW7405-5490: Reduce slave slot to 1 for each MFD source in mosaic mode.
 *
 * Hydra_Software_Devel/222   6/28/11 4:58p vanessah
 * SW7425-686: change Srcpolarity when XVD and BVN refreshrate mismatch
 *
 * Hydra_Software_Devel/221   6/28/11 9:41a vanessah
 * SW7425-686: Get rid of black frame
 *
 * Hydra_Software_Devel/220   6/27/11 7:58p pntruong
 * SW7425-772: Fine tuned deinterlacer policy.
 *
 * Hydra_Software_Devel/219   6/26/11 5:56p vanessah
 * SW7425-686: fix XVD and BVN fmt missync by mute MFD
 *
 * Hydra_Software_Devel/218   3/29/11 1:42p yuxiaz
 * SW7422-143, SW7425-236: Added new source dirty bit for orientation
 * change. Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/217   12/20/10 5:46p yuxiaz
 * SW7422-34: Added BVDC_Source_SetOrientation and
 * BVDC_Source_GetOrientation.
 *
 * Hydra_Software_Devel/216   11/11/10 7:31p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/215   11/11/10 7:13p hongtaoz
 * SW7425-51: reduced combo trigger use in VDC to 1 per MFD source;
 *
 * Hydra_Software_Devel/214   11/1/10 1:14p vanessah
 * SW7422-43:  merge 2D into 3D with a unified interface, change input
 * data structure as const
 *
 * Hydra_Software_Devel/213   10/29/10 5:08p vanessah
 * SW7422-43:  3D graphics render,merge 2D into 3D with a unified
 * interface
 *
 * Hydra_Software_Devel/212   10/25/10 7:23p vanessah
 * SW7422-43:  Handle GFD programming for 3D.
 *
 * Hydra_Software_Devel/211   10/18/10 10:06a yuxiaz
 * SW7422-39: Added BAVC_SourceId_eMpeg2 support in VDC.
 *
 * Hydra_Software_Devel/210   10/11/10 12:43p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/SW7420-173/3   6/4/10 11:08a jessem
 * SW7420-173: Reorganized validation of user provided surface. Also added
 * check if a window assigned to a VFD source is being used by a
 * different source.
 *
 * Hydra_Software_Devel/SW7420-173/2   6/1/10 2:46p jessem
 * SW7420-173: Corrected polarity used when programming the VFD.
 *
 * Hydra_Software_Devel/SW7420-173/1   5/24/10 5:09p jessem
 * SW7420-173: Added support for using VFD as  a  source.
 *
 * Hydra_Software_Devel/209   5/7/10 7:20p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/208   4/7/10 11:35a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/207   4/5/10 4:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/206   4/2/10 2:57p yuxiaz
 * SW7405-3965: Mosaic decoder can't be attached to the window of the 2nd
 * display.
 *
 * Hydra_Software_Devel/205   12/2/09 3:04p yuxiaz
 * SW7405-3120: Added support for overlapping mosaic videos with z-
 * ordering
 *
 * Hydra_Software_Devel/204   11/17/09 10:01p pntruong
 * SW3556-866: Expose vdec frontend csc for adc calibration for scart
 * input.
 *
 * Hydra_Software_Devel/203   5/28/09 2:19p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/202   5/22/09 11:33a tdo
 * PR55364: Disable dithering when any test feature is enabled
 *
 * Hydra_Software_Devel/201   5/12/09 10:45p pntruong
 * PR51338: Added support for adc fullrange raw 11-bit.
 *
 * Hydra_Software_Devel/200   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/199   4/14/09 2:10p yuxiaz
 * PR54096: Video and OSD both disappear if Input source is HDMI or
 * Component of 1080I.
 *
 * Hydra_Software_Devel/198   4/10/09 6:29p hongtaoz
 * PR54064: source pending callback must fire after window shutdown;
 *
 * Hydra_Software_Devel/197   4/3/09 4:34p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/196   3/31/09 5:02p hongtaoz
 * PR53687, PR53688: mpeg window creation should not take over synclock if
 * the window's source or display is in sync-lock transition;
 *
 * Hydra_Software_Devel/195   3/30/09 10:26p pntruong
 * PR53137: ADC conflict when doing PIP/PBP between PC/Component and
 * SIF(audio).
 *
 * Hydra_Software_Devel/194   3/16/09 10:51p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/2/09 4:22p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/193   3/2/09 3:47p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/26/09 11:18p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/192   2/26/09 6:11p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:31p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/191   12/4/08 5:24p pntruong
 * PR50002: VDC: Added function to force source pending.
 *
 * Hydra_Software_Devel/190   11/14/08 6:08p pntruong
 * PR47952: PR48969: VDEC: Support HD component signals with bi-level
 * hsync.
 *
 * Hydra_Software_Devel/PR47952/1   11/14/08 5:44p mlei
 * PR48969: VDEC: Support HD component signals with bi-level hsync
 *
 * Hydra_Software_Devel/189   10/22/08 10:38p pntruong
 * PR48248: Need change ADC5 clock setting to CLK_PC.
 *
 * Hydra_Software_Devel/188   10/9/08 5:29p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/187   10/3/08 12:21p syang
 * pr 47129, pr 45402 :  1). improve  the VDC internally used videoFmtCode
 * approximate; 2). return w, h, frmRate and bInterlaced with src-pending
 * callback for non-enumerated mpeg-src video format
 *
 * Hydra_Software_Devel/186   9/15/08 9:17p pntruong
 * PR46514: Upon destroyed and disabled slot disable adc if not used.
 *
 * Hydra_Software_Devel/184   9/2/08 6:05p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 *
 * Hydra_Software_Devel/183   8/22/08 10:28a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/182   6/18/08 3:08p syang
 * PR35549: add dirty bit bAddWin
 *
 * Hydra_Software_Devel/181   5/8/08 5:54p pntruong
 * PR35549: Added more preparations for source pending.
 *
 * Hydra_Software_Devel/180   4/29/08 7:48p pntruong
 * PR35549: Make room for source pending dirty bit.
 *
 * Hydra_Software_Devel/179   4/21/08 11:17p pntruong
 * PR34711: Prepared to add new features into vdec.  Removed vdec1
 * support.
 *
 * Hydra_Software_Devel/179   4/21/08 11:14p pntruong
 * PR34711: Prepared to add new features into vdec.  Removed vdec1
 * support.
 *
 * Hydra_Software_Devel/178   3/20/08 2:52p hongtaoz
 * PR40797: using uint32_t instead of bool in the dirty bits struture;
 *
 * Hydra_Software_Devel/177   3/18/08 7:07p pntruong
 * PR34806: Added initial adc type for scart input.
 *
 * Hydra_Software_Devel/176   3/17/08 1:39p pntruong
 * PR40448: Avoid unncessary reseting pll/adc in pip case.
 *
 * Hydra_Software_Devel/175   2/21/08 5:01p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/174   12/17/07 1:14p pntruong
 * PR37992: Added validation of duplicated adc usage, turn off unused adc,
 * protect usage of pll and pc_in block.  Main/Pip work but more work
 * needed for mixing 216Mhz and 108Mhz.
 *
 * Hydra_Software_Devel/173   12/10/07 4:03p pntruong
 * PR38046: Added work-around by preventing HD_DVI_1 connecting to drain 1
 * and 2.
 *
 * Hydra_Software_Devel/172   11/29/07 1:49p pntruong
 * PR36216: [PC]:Support component input formats with PC input.  Tighten
 * const.
 *
 * Hydra_Software_Devel/171   11/5/07 8:13p pntruong
 * PR36462: PR33011:If CVBS signal doesn't contain burst signal, need S/W
 * assume a default format
 * PR36563:for RF channel, mute the video 20 vysnc when video format
 * change. This is to fix the transition that cause by RF auto detection:
 * PAL-M/PAL-Nc/NTSC
 * PR33797:to fix the transtion and anr hanging up during the switch
 * channel
 * PR36462: decrease the delay for VCR detection
 *
 * Hydra_Software_Devel/170   10/18/07 12:05a pntruong
 * PR36137: [PC]:Set pc input video format without disable auto format
 * detection.
 *
 * Hydra_Software_Devel/169   10/3/07 4:53p pntruong
 * PR35412: Expose API to enable/Disable of Coring
 * (VDEC_BE_0_DITHER_CORING_ENABLE).
 *
 * Hydra_Software_Devel/168   9/21/07 4:00p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/167   9/17/07 5:48p pntruong
 * PR34817: SD format(480X480) decoding problem.  Updated hddvi code to
 * use the h/v size from hdr/xvd.
 *
 * Hydra_Software_Devel/166   9/10/07 1:55p pntruong
 * PR34690: [PC]:Smooth the performance of manual clock. Toggle the
 * H_threshold when manual clock to make sure the position accurate.
 *
 * Hydra_Software_Devel/PR34690/1   9/10/07 8:58p gorgon
 * PR34690:[PC]:Smooth the performance of manual clock. Toggle the
 * H_threshold when manual clock to make sure the position accurate.
 *
 * Hydra_Software_Devel/165   8/30/07 12:05p pntruong
 * PR33614:  Removed vnet reset, and delay auto-trigger to avoid re-enable
 * errors in anr.
 *
 * Hydra_Software_Devel/164   8/13/07 5:43p syang
 * PR 33614: add a vnet-reconfig for 3D comb off to on if anr is enabeld
 *
 * Hydra_Software_Devel/163   7/20/07 3:24p pntruong
 * PR33011, PR33012, PR33173: Merged from pr branch and refactored extra
 * codes.
 *
 * Hydra_Software_Devel/162   7/19/07 2:38p tdo
 * PR33034: Modify DNR API to extend MNR/BNR/DCR filter level
 *
 * Hydra_Software_Devel/161   7/11/07 8:24p pntruong
 * PR32939: [VDEC]: The ATSC and PC FD tables should be separated.
 *
 * Hydra_Software_Devel/160   6/29/07 11:48a pntruong
 * PR32503: Updated mad settings for oversampling.
 *
 * Hydra_Software_Devel/159   6/27/07 10:04p pntruong
 * PR32503: [VDEC] Add oversampling support.
 *
 * Hydra_Software_Devel/158   6/27/07 12:32p tdo
 * PR32489: Add support for custom adjust DNR
 *
 * Hydra_Software_Devel/157   6/21/07 7:39p pntruong
 * PR30427: Allow blackoffset to be custom even for component input.
 *
 ***************************************************************************/
#ifndef BVDC_SOURCE_PRIV_H__
#define BVDC_SOURCE_PRIV_H__

#include "bstd.h"             /* standard types */
#include "bkni.h"             /* memcpy calls */
#include "bvdc.h"             /* Video display */
#include "bvdc_common_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_vdec_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_anr_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/

#define BVDC_P_SRC_IS_ANALOG(source_id) \
	((BAVC_SourceId_eVdec0==(source_id)) || \
	 (BAVC_SourceId_eVdec1==(source_id)))

#define BVDC_P_SRC_IS_ITU656(source_id) \
	((BAVC_SourceId_e656In0==(source_id)) || \
	 (BAVC_SourceId_e656In1==(source_id)))

#define BVDC_P_SRC_IS_MPEG(source_id) \
	 (BAVC_SourceId_eMpegMax>=(source_id))

#define BVDC_P_SRC_IS_GFX(source_id) \
	((BAVC_SourceId_eGfx0<=(source_id)) && \
	 (BAVC_SourceId_eGfxMax>=(source_id)))

#define BVDC_P_SRC_IS_HDDVI(source_id) \
	((BAVC_SourceId_eHdDvi0==(source_id)) || \
	 (BAVC_SourceId_eHdDvi1==(source_id)))

#define BVDC_P_SRC_IS_DS(source_id) \
	((BAVC_SourceId_eDs0==(source_id)))

#define BVDC_P_SRC_IS_VIDEO(source_id) \
	((BVDC_P_SRC_IS_MPEG(source_id)) || \
	 (BVDC_P_SRC_IS_VFD(source_id)) || \
	 (BVDC_P_SRC_IS_HDDVI(source_id)) || \
	 (BVDC_P_SRC_IS_ITU656(source_id)) || \
	 (BVDC_P_SRC_IS_ANALOG(source_id)) || \
	 (BVDC_P_SRC_IS_DS(source_id)))

#define BVDC_P_SRC_NEED_DRAIN(source_id) \
	((BVDC_P_SRC_IS_HDDVI(source_id)) || \
	 (BVDC_P_SRC_IS_ITU656(source_id)) || \
	 (BVDC_P_SRC_IS_ANALOG(source_id)))

#define BVDC_P_SRC_IS_VFD(source_id) \
	((BAVC_SourceId_eVfd0<=(source_id)) && \
	 (BAVC_SourceId_eVfdMax>=(source_id)))

#define BVDC_P_SRC_GET_LIST_IDX(polarity_id, idx) \
	(BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT * (polarity_id) + (idx))

/* For RUL multi-buffering. */
#define BVDC_P_SRC_NEXT_RUL(pSource, polarity_id) \
	((pSource)->aulRulIdx[(polarity_id)] = \
	BVDC_P_NEXT_RUL_IDX((pSource)->aulRulIdx[(polarity_id)]))

/* Get the current list pointed by aulRulIdx[field]. */
#define BVDC_P_SRC_GET_LIST(pSource, polarity_id) \
	((pSource)->ahList[BVDC_P_SRC_GET_LIST_IDX(polarity_id, \
		(pSource)->aulRulIdx[polarity_id])])

/* MosaicMode: rotate all 4 slave RULs, to avoid premature overwrite of slave
 * RUL in case it's not executed yet, which is easy to happen since slave RULs
 * execution(multiple triggers) spreads across the whole frame and back-to-back
 * callbacks would easily run over the double-buffer slave RULs once for
 * progressive format;
 */
#define BVDC_P_SRC_GET_NEXT_SLAVE_RUL_IDX(pSource) \
	((pSource)->ulSlaveRulIdx = (((pSource)->ulSlaveRulIdx + 1) & 3))

/* Src uses all T/B/F slots. */
#define BVDC_P_SRC_MAX_SLOT_COUNT \
	(BVDC_P_MAX_POLARITY)

#define BVDC_P_SRC_MAX_LIST_COUNT \
	(BVDC_P_SRC_MAX_SLOT_COUNT * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT)

/* Get the current slot pointed by field. */
#define BVDC_P_SRC_GET_SLOT(pSource, polarity_id) \
	((pSource)->ahSlot[(polarity_id)])


/* Get ADC index */
#define BVDC_P_SRC_GET_ADC_IDX(adc) \
	((BVDC_Adc_0==(adc)) ? 0 \
	:(BVDC_Adc_1==(adc)) ? 1 \
	:(BVDC_Adc_2==(adc)) ? 2 \
	:(3))

#define BVDC_P_SRC_FIELD_DATA_MASK(r, f, v) (BCHP_FIELD_DATA(r, f, v) & BCHP_MASK(r, f))
#define BVDC_P_SRC_VALIDATE_FIELD(r, f, v) (BVDC_P_SRC_FIELD_DATA_MASK(r, f, v) == (BCHP_FIELD_DATA(r, f, v)))

/* This is to ensure that we don't have a big scale up. */
#define BVDC_P_SRC_INPUT_H_MIN             (64)
#define BVDC_P_SRC_INPUT_V_MIN             (64)
#define BVDC_P_VDEC_CHANNELS               (5)  /* R,G,B, CALIBRATION, SIF */

/* Number of actual physical ADC, aka VAFE */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
#define BVDC_P_MAX_PHYSICAL_ADC            (5)
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
#define BVDC_P_MAX_PHYSICAL_ADC            (4)
#else
#define BVDC_P_MAX_PHYSICAL_ADC            (6)
#endif

#if (BVDC_P_SUPPORT_VDEC > 1)
#define BVDC_P_Vdec_PostMuxValue(hVdec)    \
   ((0 == (hVdec)->eId)? BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_0 : BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_1)
#else
#define BVDC_P_Vdec_PostMuxValue(hVdec)    (BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_0)
#endif

/* 7440 uses loopback 1 as downsample source 0 mux value */
#define BVDC_P_Ds_PostMuxValue(hDs)        (BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_1)
#define BVDC_P_HdDvi_PostMuxValue(hHdDvi)  (BCHP_VNET_F_SCL_0_SRC_SOURCE_HD_DVI_0 + (hHdDvi)->eId)

#if (BVDC_P_SUPPORT_656_IN > 1)
#define BVDC_P_656In_PostMuxValue(h656In)    \
   ((0 == (h656In)->eId)? BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_0 : BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_1)
#else
#define BVDC_P_656In_PostMuxValue(h656In)    (BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_0)
#endif

#define BVDC_P_Source_PostMuxValue(hSrc) \
   (BVDC_P_SRC_IS_MPEG((hSrc)->eId) ?    BVDC_P_Feeder_PostMuxValue((hSrc)->hMpegFeeder) : \
    BVDC_P_SRC_IS_VFD((hSrc)->eId) ?    BVDC_P_Feeder_PostMuxValue((hSrc)->hVfdFeeder) : \
    BVDC_P_SRC_IS_ANALOG((hSrc)->eId) ?  BVDC_P_Vdec_PostMuxValue((hSrc)->hVdec)         : \
    BVDC_P_SRC_IS_ITU656((hSrc)->eId) ?  BVDC_P_656In_PostMuxValue((hSrc)->h656In)       : \
    BVDC_P_SRC_IS_DS((hSrc)->eId) ?      BVDC_P_Ds_PostMuxValue((hSrc)->hDs)             : \
    /*BVDC_P_SRC_IS_HDDVI((hSrc)->eId)?*/BVDC_P_HdDvi_PostMuxValue((hSrc)->hHdDvi))

/* Oversample */
#define BVDC_P_OVER_SAMPLE(factor, value)            (((factor)+1) * (value))

/* Who control the slot triggering. */
typedef enum BVDC_P_TriggerCtrl
{
	BVDC_P_TriggerCtrl_eSource = 0,
	BVDC_P_TriggerCtrl_eXfering,
	BVDC_P_TriggerCtrl_eDisplay

} BVDC_P_TriggerCtrl;

#define BVDC_P_NULL_SOURCE                 ((BAVC_SourceId)(-1))


/****************************************************************************
 * Source dirty bits for building RUL.  New and other flags should be moving
 * as needed.
 */
typedef union
{
	struct
	{
		uint32_t                         bWinChanges       : 1; /* Bit[0]: from window user setting */
		uint32_t                         bUserChanges      : 1; /* Bit[1]: from user, no detect */

		/* Common dirty bits (for all source) */
		uint32_t                         bPicCallback      : 1; /* Bit[2]: from user */
		uint32_t                         bGenCallback      : 1; /* Bit[3]: from user */
		uint32_t                         bWindowNum        : 1; /* Bit[4]: from window */
		uint32_t                         bAddWin           : 1; /* Bit[5]: from window */
		uint32_t                         bRecAdjust        : 1; /* Bit[6]: ScanOut Rect changed. */

		uint32_t                         bInputFormat      : 1; /* Bit[7]: from user for VDEC
																* from user or XVD for HD_DVI */

		uint32_t                         bAspectRatio      : 1; /* Bit[8]: from user */
		uint32_t                         bAspectRatioClip  : 1; /* Bit[9]: from user */

		/* VDEC's dirty bits */
		uint32_t                         bAdcChange        : 1; /* Bit[10]: from user */
		uint32_t                         bAdcSetting       : 1; /* Bit[11]: from user */
		uint32_t                         b3DComb           : 1; /* Bit[12]: from user */
		uint32_t                         bAutoDetectFmt    : 1; /* Bit[13]: from user */
		uint32_t                         bManualSync       : 1; /* Bit[14]: from user */
		uint32_t                         bManualClk        : 1; /* Bit[15]: from user */
		uint32_t                         bManualPos        : 1; /* Bit[16]: from user */
		uint32_t                         bVcrHeadSwitch    : 1; /* Bit[17]: from detection */
		uint32_t                         bVideoDetected    : 1; /* Bit[18]: from detection */
		uint32_t                         bMvDetected       : 1; /* Bit[19]: from detection */
		uint32_t                         bHPllLocked       : 1; /* Bit[20]: from detection */
		uint32_t                         bFrameRateCode    : 1; /* Bit[21]: from detection */
		uint32_t                         bNoisy            : 1; /* Bit[22]: from detection */
		uint32_t                         bFvFhShift        : 1; /* Bit[23]: from detection */
		uint32_t                         bManualAcgc       : 1; /* Bit[24]: from detection */

		/* user CSC for VDEC or HD_DVI or xvd/mvd for MPEG */
		uint32_t                         bColorspace       : 1; /* Bit[25]: from xvd/mvd or user */

		/* HD_DVI dirty bits */
		uint32_t                         bMiscCtrl         : 1; /* Bit[26]: from user */

		/* DNR: MNR, BNR */
		uint32_t                         bDnrAdjust        : 1; /* Bit[27]: from user */

		/* MPEG's dirty bit */
		uint32_t                         bPsfMode          : 1; /* Bit[28]: from user */

		/* MPEG, HDDVI, VDEC: Indicate to resume */
		uint32_t                         bResume           : 1; /* Bit[29]: from user */

		/* vdec, internal fine tuning video quality */
		uint32_t                         bVdecTuning        : 1; /* Bit[30]: from detection */

		/* MPEG, HDDVI */
		uint32_t                         bOrientation       : 1; /* Bit[31]: from user */
	} stBits;

	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_Source_DirtyBits;


/***************************************************************************
 * Source Context
 ***************************************************************************/
typedef struct BVDC_P_Source_Info
{
	/*+-----------------------+
	  | Common                |
	  +-----------------------+ */
	/* Color used for fixed color feed for MPEG feeder */
	uint8_t                          ucRed;
	uint8_t                          ucGreen;
	uint8_t                          ucBlue;
	uint32_t                         ulMuteColorYCrCb;
	BVDC_MuteMode                    eMuteMode;

	/* Fix color for each field polarity */
	bool                             bFixColorEnable;
	uint32_t                         aulFixColorYCrCb[3];

	/* User CSC */
	bool                             bUserCsc;
	uint32_t                         ulUserShift;
	int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT];

	/* User Frontend CSC */
	bool                             bUserFrontendCsc;
	uint32_t                         ulUserFrontendShift;
	int32_t                          pl32_FrontendMatrix[BVDC_CSC_COEFF_COUNT];

	/* aspect-ratio canvas clipping (in pixel unit) */
	BVDC_P_ClipRect                  stAspRatRectClip;

	/* Input video format. Used by VDEC, HD_DVI, and DS source */
	const BFMT_VideoInfo            *pFmtInfo;
	const BVDC_P_FormatInfo         *pVdcFmt;
	bool                             bAutoFmtDetection;
	bool                             bAutoDetect;
	uint32_t                         ulNumFormats;
	BFMT_VideoFmt                    aeFormats[BFMT_VideoFmt_eMaxCount];
	BFMT_AspectRatio                 eAspectRatio;
	uint32_t                         ulWindows;      /* Number of window created with this source. */

	/* PR36137: */
	bool                             bReMapFormats;
	uint32_t                         ulReMapFormats;
	BVDC_VideoFmtPair                aReMapFormats[BFMT_VideoFmt_eMaxCount];

	/* HVStart config for PC In and HD_DVI source */
	bool                             bHVStartOverride;
	uint32_t                         ulHstart;
	uint32_t                         ulVstart;

	/* Orientation */
	bool                             bOrientationOverride;
	BFMT_Orientation                 eOrientation;

	/* DNR filter user settings */
	bool                             bDnr;
	BVDC_Dnr_Settings                stDnrSettings;

	/* This is used for demo mode */
	BVDC_Source_SplitScreenSettings  stSplitScreenSetting;

	/* Set by source detection, and/or window parameter.  */
	bool                             bDeinterlace;
	bool                             bVdecPhaseAdj;

	/* Source generic callback */
	BVDC_CallbackFunc_isr            pfGenericCallback;
	void                            *pvGenericParm1;
	int                              iGenericParm2;

	/* Source Pending flags */
	BVDC_ResumeMode                  eResumeMode;
	bool                             bForceSrcPending;

	/*+------------------------+
	  | VDEC Source            |
	  +------------------------+ */
	BVDC_AdcInput                    aeAdcInput[BVDC_P_MAX_ADC];
	bool                             bEnable3DComb;
	BVDC_Source_SyncSettings         stPcInSyncSettings;

	/* derived in validate for ease of access, and save computation in _isr. */
	bool                             bIntIfd;      /* Internal IFD use */
	bool                             bIfdInput;    /* IF Demodulator */
	bool                             bCvbsInput;   /* CVBS input */
	bool                             bRgbInput;    /* Compoent RGB */
	bool                             bYPrPbInput;  /* Component YPrPb */
	bool                             bScartInput;  /* CVBS + RGB/SVideo */
	bool                             bSvidInput;   /* SVideo Input */
	bool                             bCompInput;   /* Component YPrPb or Rgb */
	bool                             bPcInput;     /* PC Input */
	bool                             bFullRange;   /* full range 11-bit adc */
	const BVDC_P_AdcEntry           *apAdc[BVDC_P_VDEC_CHANNELS];
	const BVDC_P_FdtEntry           *pFdEntry;
	const BVDC_P_CdCfgEntry         *pCdCfg;
	const BVDC_P_YcCfgEntry         *pYcCfg;
	const BVDC_P_PcInCfgEntry       *pPcInCfg;
	BVDC_P_VdecInput                 eInputType;
	BVDC_P_CtInput                   eCtInputType;

	/* PR37992: Add support for Main/PIP for PC/CVBS. */
	const BVDC_P_AdcEntry           *apUsedAdc[BVDC_P_MAX_PHYSICAL_ADC];
	uint32_t                         ulAdcTurnOn;  /* In on/off input */

	/* PR29659: C0: Support 1080p @ 60 through component input. */
	bool                             bUsePllClk;    /* Use PLL, or running at 216Mhz for non-pc */

	/* Format detection table & count */
	const BVDC_P_FdtEntry           *aFdtFmtTbl;
	uint32_t                         ulFdtCount;
	uint32_t                         ulFdtMemOffset;

	/* User customized internal settings. */
	bool                             bCustomCfg; /* customed vdec settings. */
	BVDC_Vdec_Settings               stVdecSettings;
	BVDC_Vdec_AwcSettings            stAwc;
	BVDC_Vdec_CtiSettings            stCti;
	BVDC_Vdec_DcrSettings            stDcr;
	BVDC_Vdec_CombSettings           stComb;
	BVDC_Vdec_AnalogGainSettings     stAGain;
	BVDC_Vdec_DigitalGainSettings    stDGain;
	BVDC_Vdec_DelaySettings          stDelay;
	BVDC_Vdec_BackendSettings        stBackend;

	/*+-----------------------------+
	  | HD_DVI, VFD, and GFX Source |
	  +-----------------------------+ */
	/* Callback function for HD_DVI, VFD, and GFX source */
	BVDC_Source_PictureCallback_isr  pfPicCallbackFunc;
	void                            *pvParm1;
	int                              iParm2;
	uint32_t                         ulInputPort;
	BVDC_HdDvi_Settings              stHdDviSetting;

	/*+-----------------------+
	  | Mpeg Feeder Source    |
	  +-----------------------+ */
	bool                             bMosaicMode;
	bool                             bPsfEnable;
	/*  SW7425-686 XVD and BVN display rate mismatch */
	bool                             bFrameRateMismatch;


	/* Dirty bits of source. */
	BVDC_P_Source_DirtyBits          stDirty;

	/* Flag to indicate last error from user setting */
	bool                             bErrorLastSetting;

} BVDC_P_Source_Info;

typedef struct
{
	/*+-----------------------+
	  | Common                |
	  +-----------------------+ */
	/* aspect-ratio canvas clipping (in pixel unit) */
	BVDC_P_ClipRect                  stAspRatRectClip;

	/* Input video format. Used by VDEC and HD_DVI source */
	BFMT_AspectRatio                 eAspectRatio;

	/* Dirty bits of new isr setting */
	BVDC_P_Source_DirtyBits          stDirty;

	/* for copying activated isr setting into new info in next ApplyChanges */
	BVDC_P_Source_DirtyBits          stActivated;

	/* Flag to indicate last error from user setting */
	bool                             bErrorLastSetting;

} BVDC_P_Source_IsrInfo;

typedef struct BVDC_P_SourceContext
{
	BDBG_OBJECT(BVDC_SRC)

	/* Input signal loss / bad causes unable to startFeed to downstream */
	bool                      bStartFeed;
	bool                      bPrevStartFeed;

	/* public fields that expose thru API. */
	BVDC_P_Source_Info        stNewInfo;
	BVDC_P_Source_Info        stCurInfo;
	BVDC_P_Source_IsrInfo     stIsrInfo;

	/* Set to true when new & old validated by apply changes   These
	 * flags get updated at applychanges. */
	bool                      bUserAppliedChanges;

	BAVC_SourceId             eId;            /* Identify the source */
	BVDC_P_State              eState;         /* Context state. */
	bool                      bInitial;       /* No Rul executed yet */
	uint32_t                  ulConnectedWindow;
	uint32_t                  ulTransferLock;
	BVDC_Compositor_Handle    hCmpToBeLocked;
	bool                      bFieldSwap;
	bool                      bPrevFieldSwap;

	/* Event to nofify that changes has been applied to hardware. */
	BKNI_EventHandle          hAppliedDoneEvent;

	/* To support interlaced pc input. */
	BAVC_Polarity             ePcFieldId;

	/* RUL use for this source */
	BAVC_Polarity             eNextFieldId;
	BAVC_Polarity             eNextFieldIntP;
	BAVC_Polarity             eNextFieldFake;
	uint32_t                  ulSlotUsed;
	uint32_t                  aulRulIdx[BVDC_P_SRC_MAX_SLOT_COUNT];
	BRDC_Trigger              aeTrigger[BVDC_P_SRC_MAX_SLOT_COUNT];
	BRDC_Trigger              eCombTrigger;
	BRDC_Slot_Handle          ahSlot[BVDC_P_SRC_MAX_SLOT_COUNT];
	BRDC_List_Handle          ahList[BVDC_P_SRC_MAX_LIST_COUNT];
	uint32_t                  ulSlaveRulIdx; /* current slave list index to ahListSlave */
	BRDC_Slot_Handle          hSlotSlave;    /* only need 1 slave slot each source */
	BRDC_List_Handle          ahListSlave[2 * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT];
	BINT_CallbackHandle       ahCallback[BVDC_P_SRC_MAX_SLOT_COUNT]; /* For HdDvi, 656, & Analog */

	/* Trigger lost, and artificial trigger work-around. */
	BINT_CallbackHandle       hTrigger0Cb;
	BINT_CallbackHandle       hTrigger1Cb;
	BVDC_P_TriggerCtrl        eTrigCtrl;
	uint32_t                  ulSrcTrigger;
	uint32_t                  ulVecTrigger;

	/* Slot's force trigger reg addr. */
	uint32_t                  aulImmTriggerAddr[BVDC_P_SRC_MAX_SLOT_COUNT];

	/* Saved dirty bits */
	BVDC_P_Source_DirtyBits   astOldDirty[BVDC_P_SRC_MAX_SLOT_COUNT];

	/* One of these source. */
	BVDC_P_Vdec_Handle        hVdec;
	BVDC_P_HdDvi_Handle       hHdDvi;
	BVDC_P_GfxFeeder_Handle   hGfxFeeder;
	BVDC_P_Feeder_Handle      hMpegFeeder;
	BVDC_P_Feeder_Handle      hVfdFeeder;
	BVDC_P_656In_Handle       h656In;
	BVDC_P_DownSample_Handle  hDownSample;

	bool                      bSrcIs444;
	bool                      bSrcInterlaced;

	/* Hold the previous field from mvd. */
	BAVC_MVD_Field            stPrevMvdField;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BAVC_MVD_Field            stNewPic[BAVC_MOSAIC_MAX];
#else
	BAVC_MVD_Field            stNewPic;
#endif
#endif
	BAVC_VDC_HdDvi_Picture    stNewXvdField;
	bool                      bPictureChanged;
	bool                      bRasterChanged;     /* I <-> P */
	BAVC_MatrixCoefficients   eMatrixCoefficients; /* Source color space */

	/* video format info used with callback for ext (non-enumerated) mpeg video format */
	BFMT_VideoInfo            stExtVideoFmtInfo;

	/* the CRC capture flag of two pictures ago
	 * Note: when a picture is required to capture CRC, its RUL is executed one
	 *       vsync later; and its CRC computation is completed at end of that
	 *       picture, i.e. another vsync later! */
	bool                      bCaptureCrc;

	/* The windows using/displaying this source. */
	BVDC_Window_Handle        ahWindow[BVDC_P_MAX_WINDOW_COUNT];

	/* Source information prepare to hand back to user when it's changed. */
	BVDC_Source_CallbackData  stSourceCbData;

	/* TODO: get rid off ANR's status */
	BVDC_P_AnrStatus          stAnrStatus;

	/* created from VDC */
	BVDC_Handle               hVdc;          /* Created from this Vdc */

	/* Internal VDC or App handed down. */
	BVDC_Heap_Handle          hHeap;

	/* Source compositor for downsample source */
	BVDC_CompositorId         eDsSrcCompId;

	/* Derived flags from context */
	BVDC_Compositor_Handle    hSyncLockCompositor;
	BVDC_P_Rect               stScanOut;

	/* Source refresh rate; */
	uint32_t                  ulVertFreq;            /* Vert refresh rate in Hz */
	uint32_t                  ulClkPerVsync;         /* Sysclk per vsync */
	BAVC_FrameRateCode        eFrameRateCode;        /* convert to frame rate code */

	/* Source content rate */
	uint32_t                  ulStreamVertFreq;

	/* for src aspect ratio override */
	bool                      bNewUserModeAspRatio;

	/* number of shutdown windows */
	uint32_t                  ulShutdownWins;

	/* vnet drain for the source */
	BVDC_P_DrainContext       stDrain;

	/* Oversample? */
	uint32_t                  ulSampleFactor; /* (1+ulSampleFactor) */

	/* PsF scanout state: if just being scaned-out, don't do it again; */
	bool                      bPsfScanout; /* store the scanout state of the previous 1080p */
	uint32_t                  ulDropFramesCnt;
	uint32_t                  ulDispVsyncFreq;/* MPG driving display vsync rate */

	/* Source is currently in pending, free memory allocations */
	bool                      bPending;

	/* defer source pending callback until all its windows are shutdown! */
	bool                      bDeferSrcPendingCb;
	/* source fresh rate mismatch with VEC */
	bool                      bFreshRateMismatch;

	/* TestFeature1 */
	bool                      bTestFeature1;
	bool                      bWait4ReconfigVnet;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	uint32_t                  aulMosaicZOrderIndex[BVDC_P_MAX_MOSAIC];
#else
	uint8_t                   aucMosaicZOrder[BVDC_P_MAX_MOSAIC];
#endif
	uint32_t                  ulMosaicCount;
	/* Used for cadence handling in mosaic mode */
	uint32_t                  ulMosaicFirstUnmuteRectIndex;
	bool                      bMosaicFirstUnmuteRectIndexSet;

	/* Used for ignore picture handling in NRT mode */
#if BVDC_P_STG_NRT_CADENCE_WORKAROUND
	bool                      bPrevIgnorePicture;
#endif
} BVDC_P_SourceContext;


/***************************************************************************
 * Private functions
 ***************************************************************************/
BERR_Code BVDC_P_Source_Create
	( BVDC_Handle                      hVdc,
	  BVDC_Source_Handle              *phSource,
	  BAVC_SourceId                    eSourceId,
	  BVDC_P_Resource_Handle           hResource,
	  bool                             b3dSrc );

void BVDC_P_Source_Destroy
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_Init
	( BVDC_Source_Handle               hSource,
	  const BVDC_Source_Settings      *pDefSettings );

BERR_Code BVDC_P_Source_ValidateChanges
	( const BVDC_Source_Handle         ahSource[] );

void BVDC_P_Source_ApplyChanges_isr
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_AbortChanges
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_UpdateSrcState_isr
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_UpdateStatus_isr
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_ConnectWindow_isr
	( BVDC_Source_Handle               hSource,
	  BVDC_Window_Handle               hWindow );

void BVDC_P_Source_DisconnectWindow_isr
	( BVDC_Source_Handle               hSource,
	  BVDC_Window_Handle               hWindow );

void BVDC_P_Source_GetScanOutRect_isr
	( BVDC_Source_Handle               hSource,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  BVDC_P_Rect                     *pScanOutRect );

void BVDC_P_Source_GetWindowVnetmodeInfo_isr
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_MpegGetStatus_isr
	( const BVDC_Source_Handle         hSource,
	  bool                            *pbVideoDetected );

void BVDC_P_Source_BuildRul_isr
	( const BVDC_Source_Handle         hSource,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_Source_CleanupSlots_isr
	( BVDC_Source_Handle               hSource );

void BVDC_P_Source_FindLockWindow_isr
	( BVDC_Source_Handle               hSource,
	  bool                             bUpdate );

void BVDC_P_Source_AnalogDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 );

void BVDC_P_Source_VfdGfxDataReady_isr
	( BVDC_Source_Handle               hSource,
	  BVDC_Window_Handle               hWindow,
	  BVDC_P_ListInfo                 *pList,
	  int                              iParam2 );

void BVDC_P_Source_MfdGfxCallback_isr
	( void                            *pvSourceHandle,
	  int                              iField);

void BVDC_P_Source_HdDviDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 );

void BVDC_P_Source_DownSampleDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 );

void BVDC_P_Source_UpdateFrameRate_isr
	( const BFMT_VideoInfo            *pFmtInfo,
	  uint32_t                         ulClkPerVsync,
	  uint32_t                         ulDelta,
	  BAVC_FrameRateCode              *peFrameRateCode );

void BVDC_P_Source_CheckAndIssueCallback
	( BVDC_P_SourceContext            *pSource,
	  BVDC_Source_DirtyBits           *pCbDirty );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_SOURCE_PRIV_H__ */
/* End of file. */
