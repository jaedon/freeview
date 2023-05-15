/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_errors.h $
 * $brcm_Revision: Hydra_Software_Devel/66 $
 * $brcm_Date: 3/2/12 4:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_errors.h $
 * 
 * Hydra_Software_Devel/66   3/2/12 4:01p pntruong
 * SW7231-584: Added artificial display vsync generation to support dsp
 * transcoding.
 *
 * Hydra_Software_Devel/65   12/1/11 4:32p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/SW7425-968/1   11/7/11 11:11a yuxiaz
 * SW7425-968: Added independent source clipping of right window in 3D
 * modes.
 *
 * Hydra_Software_Devel/64   8/26/10 2:40p vanessah
 * SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
 * mode
 *
 * Hydra_Software_Devel/SW7420-965/1   8/20/10 1:25p vanessah
 * SW7420-965:  MCVP game mode support
 *
 * Hydra_Software_Devel/63   3/18/10 3:59p syang
 * SW7550-271: added error check for unsupported 7550 GFD1 features
 *
 * Hydra_Software_Devel/62   11/24/09 5:46p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 *
 * Hydra_Software_Devel/61   5/26/09 6:39p darnstein
 * PR48884: Create an error code for missing rate manager table entry.
 *
 * Hydra_Software_Devel/60   3/16/09 10:46p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/1   2/13/09 10:03a syang
 * PR 45796: first time check in for GFD VSCL support
 *
 * Hydra_Software_Devel/59   1/30/09 3:04p rpan
 * PR45807: Restart VEC alignment process in case timestamp taken is not
 * valid.
 *
 * Hydra_Software_Devel/58   1/29/09 4:48p rpan
 * PR45804: Added support for alignment.
 *
 * Hydra_Software_Devel/57   11/13/08 5:40p darnstein
 * PR48884: Create error code BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT.
 * Not in use yet.
 *
 * Hydra_Software_Devel/56   10/3/08 9:24a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/55   9/29/08 4:36p jessem
 * PR 46489: Added TN2T support.
 *
 * Hydra_Software_Devel/54   9/22/08 4:52p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/53   3/31/08 2:35p jessem
 * PR 39335: Added CM3D support.
 *
 * Hydra_Software_Devel/52   3/18/08 7:07p pntruong
 * PR34806: Added initial adc type for scart input.
 *
 * Hydra_Software_Devel/51   3/10/08 9:12p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/50   11/28/07 2:02p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/49   10/23/07 11:15a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:48a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/48   6/21/07 2:15p yuxiaz
 * PR32023: Added video format tolerance to HDDVI.
 *
 * Hydra_Software_Devel/47   5/15/07 11:45a yuxiaz
 * PR29940: Add support to set and get Hstart and Vstart for HDDVI.
 *
 * Hydra_Software_Devel/46   1/10/07 5:01p syang
 * PR 22569:  add error code BVDC_ERR_BAD_SRC_TYPE
 *
 * Hydra_Software_Devel/45   1/5/07 5:45p syang
 * PR 22569:  fix compile error for 7400/7403/7440
 *
 * Hydra_Software_Devel/44   12/18/06 11:28p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/15/06 11:58a syang
 * PR 25097: add error code for shared-resource control
 *
 * Hydra_Software_Devel/43   8/31/06 5:48p syang
 * PR 23977: return error for 7118 lbox enabling
 *
 * Hydra_Software_Devel/42   8/31/06 12:28p syang
 * PR 23977: report error when mad is called for 7118
 *
 * Hydra_Software_Devel/41   6/14/06 6:50p hongtaoz
 * PR22057, PR20716, PR21804: validate destination rect not to be out of
 * canvas bound; added ClearRect support for mosaic mode; modified mosaic
 * mode api;
 *
 * Hydra_Software_Devel/40   2/21/06 4:27p yuxiaz
 * PR19258: Need to increase memory for 1366x768 case. Added
 * eBufferFormat_HD in BVDC_Settings.
 *
 * Hydra_Software_Devel/39   1/12/06 1:33p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/8/05 6:50p hongtaoz
 * PR18233: add Mosaic mode support (single window working);
 *
 * Hydra_Software_Devel/38   11/10/05 4:09p jessem
 * PR 17972: Added new errors for user capture.
 *
 * Hydra_Software_Devel/37   9/16/05 3:52p hongtaoz
 * PR15888: bypass window must be full screen;
 *
 * Hydra_Software_Devel/36   9/12/05 2:49p jessem
 * PR 12739: Added 3 new error defines to support user capture of fields
 * and/or frames.
 * BVDC_ERR_CAPTURE_BUFFERS_MORE_THAN_MAX ,
 * BVDC_ERR_CAPTURED_BUFFER_NOT_FOUND,  and
 * BVDC_ERR_USER_STILL_HAS_CAPTURE_BUFFER
 *
 * Hydra_Software_Devel/35   8/18/05 1:15p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/16/05 2:01p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/34   8/10/05 6:28p syang
 * PR 12132, PR 14862, PR 15146: half coding checkin for adding src and
 * dst aspect ratio canvas control
 *
 * Hydra_Software_Devel/33   3/8/05 7:42p hongtaoz
 * PR12769: implement non-linear horizontal scaling;
 *
 * Hydra_Software_Devel/32   2/4/05 4:55p hongtaoz
 * PR13793: added compile option to use scaler_3 for bypass window;
 *
 * Hydra_Software_Devel/31   1/20/05 4:00p syang
 * PR 13282: added api func to query which surface is currently fetched by
 * hardware
 *
 * Hydra_Software_Devel/30   12/16/04 12:56p syang
 * PR 13576: change BERR_BOX_* to BVDC_ERR_BOX_*.
 *
 * Hydra_Software_Devel/29   12/10/04 4:18p yuxiaz
 * PR12995: Added API BVDC_Window_SetDelayOffset and
 * BVDC_Window_GetDelayOffset to set and get programmable delay offset to
 * each display path.
 *
 * Hydra_Software_Devel/28   11/4/04 3:51p yuxiaz
 * PR13031: Added bDisableComponentVDEC in default setting to disable s-
 * video and component VDEC support. ADC registers will not be
 * programmmed by VDC in bDisableComponentVDEC mode.
 *
 * Hydra_Software_Devel/27   10/20/04 11:34a yuxiaz
 * PR13017: Fixed BVDC_P_Source_Validate to validate source even there is
 * no window connect to it. Clean up ADC configuration.
 *
 * Hydra_Software_Devel/26   10/1/04 5:40p syang
 * PR 12821: now update gfd clip info according to offset in scaler-out
 * rect
 *
 * Hydra_Software_Devel/25   9/7/04 4:43p albertl
 * PR 7664:  Added error checking for source and display size when setting
 * vdec phase adjustment mode.
 *
 * Hydra_Software_Devel/24   8/10/04 11:40a yuxiaz
 * PR 11584: Added S-Video VDEC support
 *
 * Hydra_Software_Devel/23   7/26/04 6:03p tdo
 * PR 11971: add TAB and color clip support
 *
 * Hydra_Software_Devel/22   7/16/04 6:51p hongtaoz
 * PR11168: adding VBI pass through support;
 *
 * Hydra_Software_Devel/21   7/13/04 1:16p syang
 * PR 11901: add error code for box detect support
 *
 * Hydra_Software_Devel/20   7/6/04 8:29a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/19   6/30/04 9:45a yuxiaz
 * PR 7664: Added API to set and get VDEC/VEC phase adjustment mode. Added
 * code to set phase in VDC. Added new error code.
 *
 * Hydra_Software_Devel/18   5/24/04 5:09p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/21/04 6:57p hongtaoz
 * PR10944: added initial MAD32 support of 480i->480p;
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/17/04 7:34p hongtaoz
 * PR10944: added initial MAD32 support;
 *
 * Hydra_Software_Devel/17   4/6/04 10:15a maivu
 * PR 10157: Add new error code to specify invalid mode selected for a
 * given Vec path
 *
 * Hydra_Software_Devel/16   4/5/04 12:39p syang
 * PR 10477: Changed to copy ApplyChange activated user setting into
 * current inside ApplyChange, and to copy intr surface context into
 * current  at the beginning of RUL building; 2) Use Dirty bit field to
 * indicate change between new context and current context, and change
 * between current context and las RUL; Use Flag bits field to replace
 * bool to save size and copying time.
 *
 * Hydra_Software_Devel/15   3/24/04 12:13p hongtaoz
 * PR9163: Add support for BVDC_Display_SetRfmConfiguration;
 * validate the VDC configurations to make sure no more than one VEC path
 * are associated with shared RFM/HDMI/656 outputs ; added debug trace
 * for display module;
 *
 * Hydra_Software_Devel/14   2/25/04 2:28p pntruong
 * PR9790: BVDC_P_Scaler_SetInfo_isr will fail with floating point
 * exception if decode window size is set to 1,1.
 *
 * Hydra_Software_Devel/13   2/13/04 3:02p maivu
 * PR 9221: Added BVDC_Display_UseFrameRate
 *
 * Hydra_Software_Devel/12   2/5/04 7:55p pntruong
 * PR9577: Added additional error codes for adc validations.
 *
 * Hydra_Software_Devel/11   1/8/04 6:34p syang
 * PR 9119: further updated the desc of Window_SetBlendFactor,
 * Source_Enable/DisableAlphaPreMultiply, Source_Enable/DisableColorKey,
 * and added error check to Window_SetBlendFactor.
 *
 * Hydra_Software_Devel/10   12/4/03 4:19p pntruong
 * PR 8851: Change rectangle to offset in BVDC_Window_SetSrcClipRect.
 *
 * Hydra_Software_Devel/9   11/14/03 2:28p pntruong
 * Added error codes for source.
 *
 * Hydra_Software_Devel/8   11/7/03 10:02a pntruong
 * Added more descriptive error codes.
 *
 * Hydra_Software_Devel/7   10/29/03 11:45a pntruong
 * Added window and compositor error codes.
 *
 * Hydra_Software_Devel/6   10/8/03 6:24p maivu
 * Added error code for Rfm
 *
 * Hydra_Software_Devel/5   9/11/03 10:17a maivu
 * Added more error codes for Display submodule
 *
 * Hydra_Software_Devel/4   9/4/03 4:03p yuxiaz
 * Added new error code.
 *
 * Hydra_Software_Devel/3   8/27/03 10:56a maivu
 * Added new error codes for Display submodule.
 *
 * Hydra_Software_Devel/2   8/19/03 5:10p syang
 * moved gfx feeder error code into bvdc_error.h
 *
 * Hydra_Software_Devel/1   8/13/03 5:22p pntruong
 * Error codes for VDC.
 *
 ***************************************************************************/
#ifndef BVDC_ERRORS_H__
#define BVDC_ERRORS_H__

#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes unique to BVDC.  Error has been reserved by berr module.
 * Each module has 0xfffff (64k) of errors. */

/* */
#define BVDC_ERR_VIDEOFMT_OUTPUT_MISMATCH            BERR_MAKE_CODE(BERR_VDC_ID, 0x0000)
#define BVDC_ERR_INVALID_656OUT_MODE                 BERR_MAKE_CODE(BERR_VDC_ID, 0x0001)
#define BVDC_ERR_INVALID_656OUT_USE                  BERR_MAKE_CODE(BERR_VDC_ID, 0x0002)
#define BVDC_ERR_INVALID_HDMI_MODE                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0003)
#define BVDC_ERR_INVALID_HDMI_PATH                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0004)
#define BVDC_ERR_INVALID_HDMI_USE                    BERR_MAKE_CODE(BERR_VDC_ID, 0x0005)
#define BVDC_ERR_INVALID_MACROVISON_MODE             BERR_MAKE_CODE(BERR_VDC_ID, 0x0006)
#define BVDC_ERR_INVALID_MACROVISON_PATH             BERR_MAKE_CODE(BERR_VDC_ID, 0x0007)
#define BVDC_ERR_INVALID_MPAA_MODE                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0008)
#define BVDC_ERR_INVALID_MPAA_PATH                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0009)
#define BVDC_ERR_INVALID_DAC_SETTINGS                BERR_MAKE_CODE(BERR_VDC_ID, 0x000a)
#define BVDC_ERR_INVALID_RFM_PATH                    BERR_MAKE_CODE(BERR_VDC_ID, 0x000b)
#define BVDC_ERR_INVALID_FRAMERATE_USE               BERR_MAKE_CODE(BERR_VDC_ID, 0x000c)
#define BVDC_ERR_RFMOUT_WITHOUT_COMPOSITE            BERR_MAKE_CODE(BERR_VDC_ID, 0x000d)
#define BVDC_ERR_RFMOUT_MORE_THAN_ONE                BERR_MAKE_CODE(BERR_VDC_ID, 0x000e)
#define BVDC_ERR_HDMIOUT_MORE_THAN_ONE               BERR_MAKE_CODE(BERR_VDC_ID, 0x000f)
#define BVDC_ERR_656OUT_MORE_THAN_ONE                BERR_MAKE_CODE(BERR_VDC_ID, 0x0010)
#define BVDC_ERR_INVALID_MODE_PATH                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0011)
#define BVDC_ERR_DISPLAY_ALREADY_CREATED             BERR_MAKE_CODE(BERR_VDC_ID, 0x0012)
#define BVDC_ERR_INVALID_DCS_MODE                    BERR_MAKE_CODE(BERR_VDC_ID, 0x0013)
#define BVDC_ERR_INVALID_DCS_PATH                    BERR_MAKE_CODE(BERR_VDC_ID, 0x0014)
#define BVDC_ERR_UNSUPPORTED_PIXEL_RATE              BERR_MAKE_CODE(BERR_VDC_ID, 0x0015)


/* gfx feeder error code: 0x0100 - 0x01ff */
#define BVDC_ERR_GFX_SUR_SIZE_MISMATCH               BERR_MAKE_CODE(BERR_VDC_ID, 0x0100)
#define BVDC_ERR_GFX_SUR_FMT_MISMATCH                BERR_MAKE_CODE(BERR_VDC_ID, 0x0101)
#define BVDC_ERR_GFX_HORIZONTAL_SHRINK               BERR_MAKE_CODE(BERR_VDC_ID, 0x0102)
#define BVDC_ERR_GFX_VERTICAL_SCALE                  BERR_MAKE_CODE(BERR_VDC_ID, 0x0103)
#define BVDC_ERR_GFX_SRC_OVER_CLIP                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0104)
#define BVDC_ERR_GFX_CLUT_REUSE                      BERR_MAKE_CODE(BERR_VDC_ID, 0x0105)
#define BVDC_ERR_GFX_INFINITE_SCALE                  BERR_MAKE_CODE(BERR_VDC_ID, 0x0106)
#define BVDC_ERR_GFX_SUR_UNSURE                      BERR_MAKE_CODE(BERR_VDC_ID, 0x0107)
#define BVDC_ERR_GFX_HSCL_OUT_OF_RANGE               BERR_MAKE_CODE(BERR_VDC_ID, 0x0108)
#define BVDC_ERR_GFX_VSCL_OUT_OF_RANGE               BERR_MAKE_CODE(BERR_VDC_ID, 0x0109)
#define BVDC_ERR_GFX_UNSUPPORTED_GAMMATABLE          BERR_MAKE_CODE(BERR_VDC_ID, 0x010a)

#define BVDC_ERR_SRC_IS_NOT_MPEG                     BERR_MAKE_CODE(BERR_VDC_ID, 0x0200)

#define BVDC_ERR_SOURCE_WINDOW_MISMATCH              BERR_MAKE_CODE(BERR_VDC_ID, 0x0300)
#define BVDC_ERR_VIDEO_FORMAT_NOT_SUPPORTED          BERR_MAKE_CODE(BERR_VDC_ID, 0x0301)
#define BVDC_ERR_DST_SIZE_LARGER_THAN_CANVAS         BERR_MAKE_CODE(BERR_VDC_ID, 0x0302)
#define BVDC_ERR_APL_SIZE_LARGER_THAN_CANVAS         BERR_MAKE_CODE(BERR_VDC_ID, 0x0303)
#define BVDC_ERR_DST_SIZE_LARGER_THAN_SCL_OUTPUT     BERR_MAKE_CODE(BERR_VDC_ID, 0x0304)
#define BVDC_ERR_SOURCE_ALREADY_CREATED              BERR_MAKE_CODE(BERR_VDC_ID, 0x0305)
#define BVDC_ERR_WINDOW_ALREADY_CREATED              BERR_MAKE_CODE(BERR_VDC_ID, 0x0306)
#define BVDC_ERR_COMPOSITOR_ALREADY_CREATED          BERR_MAKE_CODE(BERR_VDC_ID, 0x0307)
#define BVDC_ERR_DUPLICATED_ZORDER                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0308)
#define BVDC_ERR_WINDOW_NOT_AVAILABLE                BERR_MAKE_CODE(BERR_VDC_ID, 0x0309)
#define BVDC_ERR_INVALID_ADC_FOR_COMPOSITE           BERR_MAKE_CODE(BERR_VDC_ID, 0x030A)
#define BVDC_ERR_INVALID_ADC_FOR_SVIDEO              BERR_MAKE_CODE(BERR_VDC_ID, 0x030B)
#define BVDC_ERR_INVALID_ADC_FOR_COMPONENT           BERR_MAKE_CODE(BERR_VDC_ID, 0x030C)
#define BVDC_ERR_INVALID_ADC_FOR_IFDEMOD             BERR_MAKE_CODE(BERR_VDC_ID, 0x030D)
#define BVDC_ERR_ILLEGAL_CLIPPING_VALUES             BERR_MAKE_CODE(BERR_VDC_ID, 0x030E)
#define BVDC_ERR_ILLEGAL_GFX_WIN_BLEND               BERR_MAKE_CODE(BERR_VDC_ID, 0x030F)
#define BVDC_ERR_SCALER_OUTPUT_OUT_OF_BOUND          BERR_MAKE_CODE(BERR_VDC_ID, 0x0310)
#define BVDC_ERR_DEINTERLACE_TO_INTERLACED_FORMAT    BERR_MAKE_CODE(BERR_VDC_ID, 0x0311)
#define BVDC_ERR_DEINTERLACE_NON_MAIN_WINDOW         BERR_MAKE_CODE(BERR_VDC_ID, 0x0312)
#define BVDC_ERR_MULTI_PHASE_ADJUST_VALUES           BERR_MAKE_CODE(BERR_VDC_ID, 0x0313)
#define BVDC_ERR_BOX_DETECT_GFX                      BERR_MAKE_CODE(BERR_VDC_ID, 0x0314)
#define BVDC_ERR_BOX_DETECT_HW_NOT_AVAILABLE         BERR_MAKE_CODE(BERR_VDC_ID, 0x0315)
#define BVDC_ERR_WINDOW_VBI_PASS_THROUGH             BERR_MAKE_CODE(BERR_VDC_ID, 0x0316)
#define BVDC_ERR_ADC_NOT_SUPPORTED                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0317)
#define BVDC_ERR_COMPONENT_VDEC_DISABLED_BY_DEFAULT  BERR_MAKE_CODE(BERR_VDC_ID, 0x0318)
#define BVDC_ERR_PHASE_ADJUST_ON_ILLEGAL_SOURCE      BERR_MAKE_CODE(BERR_VDC_ID, 0x0319)
#define BVDC_ERR_PHASE_ADJUST_ON_DOWNSCALED_WINDOWS  BERR_MAKE_CODE(BERR_VDC_ID, 0x031A)
#define BVDC_ERR_LIP_SYNC_DELAY_MORE_THAN_MAX        BERR_MAKE_CODE(BERR_VDC_ID, 0x031B)
#define BVDC_ERR_BYPASS_SOURCE_DISPLAY_MISMATCH      BERR_MAKE_CODE(BERR_VDC_ID, 0x031C)
#define BVDC_ERR_INVALID_NONLINEAR_SCALE             BERR_MAKE_CODE(BERR_VDC_ID, 0x031D)
#define BVDC_ERR_INVALID_SRC_ASPECT_RATIO_RECT       BERR_MAKE_CODE(BERR_VDC_ID, 0x031E)
#define BVDC_ERR_INVALID_DISP_ASPECT_RATIO_RECT      BERR_MAKE_CODE(BERR_VDC_ID, 0x031F)
#define BVDC_ERR_CAPTURE_BUFFERS_MORE_THAN_MAX       BERR_MAKE_CODE(BERR_VDC_ID, 0x0320)
#define BVDC_ERR_CAPTURED_BUFFER_NOT_FOUND           BERR_MAKE_CODE(BERR_VDC_ID, 0x0321)
#define BVDC_ERR_USER_STILL_HAS_CAPTURE_BUFFER       BERR_MAKE_CODE(BERR_VDC_ID, 0x0322)
#define BVDC_ERR_NO_AVAIL_CAPTURE_BUFFER             BERR_MAKE_CODE(BERR_VDC_ID, 0x0323)
#define BVDC_ERR_NO_VIDEO_SOURCE                     BERR_MAKE_CODE(BERR_VDC_ID, 0x0324)
#define BVDC_ERR_BYPASS_WINDOW_NOT_FULL_SCREEN       BERR_MAKE_CODE(BERR_VDC_ID, 0x0325)
#define BVDC_ERR_BYPASS_WINDOW_INVALID_AR_MODE       BERR_MAKE_CODE(BERR_VDC_ID, 0x0326)
#define BVDC_ERR_INVALID_HD_BUFFER_FORMAT            BERR_MAKE_CODE(BERR_VDC_ID, 0x0327)
#define BVDC_ERR_DST_RECT_OUT_OF_BOUND               BERR_MAKE_CODE(BERR_VDC_ID, 0x0328)
#define BVDC_ERR_MAD_NOT_SUPPORTED                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0329)
#define BVDC_ERR_RESOURCE_NOT_AVAILABLE              BERR_MAKE_CODE(BERR_VDC_ID, 0x0330)
#define BVDC_ERR_RESOURCE_NOT_RECORDED               BERR_MAKE_CODE(BERR_VDC_ID, 0x0331)
#define BVDC_ERR_RESOURCE_NOT_ACQUIRED               BERR_MAKE_CODE(BERR_VDC_ID, 0x0332)
#define BVDC_ERR_ANR_HW_NOT_AVAILABLE                BERR_MAKE_CODE(BERR_VDC_ID, 0x0333)
#define BVDC_ERR_BAD_SRC_TYPE                        BERR_MAKE_CODE(BERR_VDC_ID, 0x0334)
#define BVDC_ERR_INVALID_HVSTART                     BERR_MAKE_CODE(BERR_VDC_ID, 0x0335)
#define BVDC_ERR_INVALID_TOLERANCE                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0336)
#define BVDC_ERR_FGT_NOT_SUPPORTED                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0337)
#define BVDC_ERR_INVALID_ADC_FOR_SCART               BERR_MAKE_CODE(BERR_VDC_ID, 0x0338)
#define BVDC_ERR_MCVP_NOT_SUPPORTED                  BERR_MAKE_CODE(BERR_VDC_ID, 0x0339)

#define BVDC_ERR_PEP_WINDOW_NOT_SUPPORT              BERR_MAKE_CODE(BERR_VDC_ID, 0x0400)
#define BVDC_ERR_PEP_INVALID_PARAMETER               BERR_MAKE_CODE(BERR_VDC_ID, 0x0401)
#define BVDC_ERR_CM3D_WINDOW_NOT_SUPPORTED           BERR_MAKE_CODE(BERR_VDC_ID, 0x0402)
#define BVDC_ERR_CM3D_INVALID_PARAMETER              BERR_MAKE_CODE(BERR_VDC_ID, 0x0403)
#define BVDC_ERR_VIDEO_TestFeature1_NON_MAIN_WINDOW   BERR_MAKE_CODE(BERR_VDC_ID, 0x0404)
#define BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED            BERR_MAKE_CODE(BERR_VDC_ID, 0x0405)
#define BVDC_ERR_INVALID_TARGET_DISPLAY              BERR_MAKE_CODE(BERR_VDC_ID, 0x0406)
#define BVDC_ERR_INVALID_TIMESTAMP                   BERR_MAKE_CODE(BERR_VDC_ID, 0x0407)
#define BVDC_ERR_3D_INDEP_SRC_CLIP_NOT_SUPPORTED     BERR_MAKE_CODE(BERR_VDC_ID, 0x0408)

#define BVDC_ERR_INVALID_MOSAIC_MODE                 BERR_MAKE_CODE(BERR_VDC_ID, 0x0501)
#define BVDC_ERR_INVALID_TUNE_INTERFACE              BERR_MAKE_CODE(BERR_VDC_ID, 0x0502)
#define BVDC_ERR_INVALID_TUNE_STRUCTURE              BERR_MAKE_CODE(BERR_VDC_ID, 0x0503)
#define BVDC_ERR_INVALID_TUNE_FIELD                  BERR_MAKE_CODE(BERR_VDC_ID, 0x0504)
#define BVDC_ERR_TUNE_NOT_SUPPORTED                  BERR_MAKE_CODE(BERR_VDC_ID, 0x0505)

#define BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT    BERR_MAKE_CODE(BERR_VDC_ID, 0x0600)

#define BVDC_ERR_FEATURE_NOT_SUPPORTED               BERR_MAKE_CODE(BERR_VDC_ID, 0x0700)

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_ERRORS_H__ */
/* End of file. */
