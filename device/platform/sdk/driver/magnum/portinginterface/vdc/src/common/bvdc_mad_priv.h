/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_mad_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/88 $
 * $brcm_Date: 3/30/12 3:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_mad_priv.h $
 * 
 * Hydra_Software_Devel/88   3/30/12 3:35p pntruong
 * SW7420-2286: Added tri-state option for pq policies for mad.
 *
 * Hydra_Software_Devel/87   1/12/12 5:39p vanessah
 * SW7425-2162: change 7425 MADR QM buffer allocation
 *
 * Hydra_Software_Devel/86   7/22/11 11:13p pntruong
 * SW7425-983: Corrected deinterlacer's hscaler threshold.
 *
 * Hydra_Software_Devel/85   1/4/11 4:07p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/84   8/4/10 10:28p vanessah
 * SW3548-2981:  Merge into main line
 *
 * Hydra_Software_Devel/83   8/3/10 1:57p pntruong
 * SW7125-424: Initialized the fields for game mode.  Corrected the
 * deinterlacer version.
 *
 * Hydra_Software_Devel/82   6/18/10 4:21p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/81   4/7/10 11:32a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/80   4/5/10 4:10p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/79   2/11/10 11:00a pntruong
 * SW7405-3829: Moved shrinkwidth to mad settings.
 *
 * Hydra_Software_Devel/78   1/28/10 11:11a syang
 * SW7405-3829: add option for app to shrink src width to use deinterlace
 *
 * Hydra_Software_Devel/77   9/14/09 4:12p hongtaoz
 * SW3548-2471: added window API to allow smooth transition for MAD game
 * modes; updated MAD 3-field game modes QM constellations;
 *
 * Hydra_Software_Devel/76   8/28/09 1:24p yuxiaz
 * SW3556-672: BVDC_Window_GetDeinterlaceDefaultConfiguration should
 * Return Dynamic Defaults.
 *
 * Hydra_Software_Devel/75   7/22/09 1:33p hongtaoz
 * PR56938: added MAD hard start sw workaround to avoid stalling when test
 * feature1 is on; basically disable MAD feeders when the corresponding
 * field stores are uninitialized;
 *
 * Hydra_Software_Devel/74   6/18/09 5:51p syang
 * PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
 *
 * Hydra_Software_Devel/73   4/3/09 4:33p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/72   3/16/09 10:48p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/26/09 11:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/71   2/26/09 6:11p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/13/09 6:01p tdo
 * PR 45785, PR 45789: merge from main branch on 2/13/09
 *
 * Hydra_Software_Devel/70   2/13/09 11:15a yuxiaz
 * PR52030: MAD QM do not use DCX logic.
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:30p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/69   12/4/08 1:21p pntruong
 * PR46484: Removed unused code.
 *
 * Hydra_Software_Devel/68   11/12/08 3:02p hongtaoz
 * PR48213: implemented 3:2 reverse pulldown soft algorithm for 5-field 1-
 * delay MAD game mode; re-ordered the pixel constellation for 5-field 1-
 * delay game mode;
 *
 * Hydra_Software_Devel/67   10/23/08 9:40p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/66   10/6/08 1:12p pntruong
 * PR47595, PR47072: Fixed pixel formats for cap/mad.
 *
 * Hydra_Software_Devel/65   10/3/08 9:25a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/64   10/1/08 8:33p pntruong
 * PR47303, PR46309: Reused the TestFeature1 type in surface.
 *
 * Hydra_Software_Devel/63   9/30/08 6:30p hongtaoz
 * PR46738: fixed MAD/ANR memory saving mode;
 *
 * Hydra_Software_Devel/62   9/29/08 12:58p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/61   9/26/08 3:25p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/60   9/24/08 6:20p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/59   9/17/08 8:43p pntruong
 * PR47013: B0: Initial vdc support.
 *
 * Hydra_Software_Devel/58   9/9/08 9:04p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/57   8/18/08 3:29p tdo
 * PR45707: Update MAD: 3:2 pulldown setting with ANR fix (for demo
 * release)
 *
 * Hydra_Software_Devel/56   8/4/08 12:07p hongtaoz
 * PR45232: updated some of soft22 settings; added MAD soft22 debug
 * compile option to use RDC scratch registers to debug soft22 algorithm
 * settings;
 *
 * Hydra_Software_Devel/55   4/28/08 11:10a hongtaoz
 * PR38878: increased BVDC_P_MAD_MIN_USABLE_PCC to 2100 to work with
 * component input;
 *
 * Hydra_Software_Devel/54   4/1/08 10:58a hongtaoz
 * PR38878: increaed BVDC_P_MAD_MIN_USABLE_PCC to 1050;
 *
 * Hydra_Software_Devel/53   3/19/08 7:38p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/52   2/29/08 4:18p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/51   2/28/08 10:12p pntruong
 * PR39154: Resolved clearcase merged.
 *
 * Hydra_Software_Devel/50   2/21/08 2:14p hongtaoz
 * PR39154: added a new flag to signal MAD user changes;
 *
 * Hydra_Software_Devel/49   2/19/08 10:35p pntruong
 * PR39154: Removed 3548 warnings.
 *
 * Hydra_Software_Devel/48   12/6/07 2:21p pntruong
 * PR37754: Need to update MAD_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET on offset
 * changes.
 *
 * Hydra_Software_Devel/47   11/13/07 4:07p rpan
 * PR31084: Added support for getting default de-interlace settings.
 *
 * Hydra_Software_Devel/46   11/7/07 4:11p pntruong
 * PR36539: Added for field freeze after hardstart, to avoid display old
 * pictures in mad fifo.
 *
 * Hydra_Software_Devel/45   11/5/07 11:47a syang
 * PR 29617:  Set SCL's top/left cut with vsync delay according to MAD
 *
 * Hydra_Software_Devel/44   11/2/07 2:19p syang
 * PR 29617:  prepare to set clip in SCL with delay according to MAD
 *
 * Hydra_Software_Devel/43   10/19/07 6:17p hongtaoz
 * PR35308:Support 2:2 pulldown detect algorithm for MAD 3 field low
 * bandwidth game mode;
 *
 * Hydra_Software_Devel/PR35308/1   10/10/07 5:43p xhuang
 * PR35308:Support 2:2 pulldown detect algorithm for MAD 3 field low
 * bandwidth game mode
 *
 * Hydra_Software_Devel/42   9/19/07 9:31a hongtaoz
 * PR34569: left-shifted MAD 2:2 upper/lower match thresholds;
 *
 * Hydra_Software_Devel/41   8/30/07 3:13p pntruong
 * PR34348: MAD optimizing Low Angle Settings for reduce gray noise.
 *
 * Hydra_Software_Devel/PR34348/1   8/29/07 4:04p xhuang
 * PR34348:add low angle customer settings
 *
 * Hydra_Software_Devel/40   8/2/07 11:32p hongtaoz
 * PR32942: added reverse 3:2 pulldown software algorithm to 3-field low
 * bandwidth game mode;
 *
 * Hydra_Software_Devel/39   8/2/07 1:52p pntruong
 * PR31983: C0: Need to enable 10-bit MAD capture mode when source is
 * 480i.
 *
 * Hydra_Software_Devel/38   7/31/07 1:58p pntruong
 * PR32487: Vertical scaler needs to be programmed to low-pass filter
 * chroma for MAD source.  Added debug to print out name of selected
 * coeffs set.
 *
 * Hydra_Software_Devel/37   7/30/07 2:10p hongtaoz
 * PR26785, PR33560, PR33020: added MAD game mode hard start; removed old
 * transition workaround;
 *
 * Hydra_Software_Devel/36   7/16/07 5:24p pntruong
 * PR32829: Set MAD error in PIP.  Offset were also missing for mad_1.
 *
 * Hydra_Software_Devel/35   7/16/07 1:33p pntruong
 * PR32829: Set MAD error in PIP.  Fixed loopback value lookup for mad 1.
 *
 * Hydra_Software_Devel/34   7/8/07 9:10p tdo
 * PR31512: Fine tuning sw 2:2 algorithm for repeated buffer case
 *
 * Hydra_Software_Devel/33   6/21/07 5:01p tdo
 * PR31512:Update Mad setting for reduce flicker
 *
 * Hydra_Software_Devel/32   6/21/07 5:23a albertl
 * PR31821:  Changed MAD game modes to allocate only the necessary number
 * of buffers and to reconfigure the vnet when buffer numbers change.
 *
 * Hydra_Software_Devel/31   5/18/07 4:21p albertl
 * PR28894:  Added new 3-field low bandwidth MAD game mode for 3563 C0.
 *
 * Hydra_Software_Devel/30   5/17/07 3:44p pntruong
 * PR30364: Provide API to control MAD up/down sampling.
 *
 * Hydra_Software_Devel/29   5/10/07 9:58a syang
 * PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
 * support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
 * drainVnet state
 *
 * Hydra_Software_Devel/28   4/27/07 7:31p albertl
 * PR29661:  Changed mad horizontal scalar assignment for new usage modes.
 *
 * Hydra_Software_Devel/27   4/17/07 5:46p albertl
 * PR26785:  Mad game modes are turned off for a few vsyncs on source clip
 * change.
 *
 * Hydra_Software_Devel/26   4/11/07 6:59p albertl
 * PR29189:  Lip sync delay now calculated using Mad game mode delay.
 *
 * Hydra_Software_Devel/25   4/5/07 2:02p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/24   4/2/07 11:29a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/23   2/23/07 6:29p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/22   1/10/07 11:00a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/21   12/18/06 11:33p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   11/22/06 3:51p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/25/06 10:13p pntruong
 * PR 24763: fix compile warning
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/19/06 3:53p albertl
 * PR23521:  Added MAD game mode functionality.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   10/12/06 5:47p tdo
 * PR 23377:  Add support for MAD 3:2 and 2:2 reverse pulldown
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:48p syang
 * PR 20884: add SCL chroma coeff
 *
 * Hydra_Software_Devel/19   9/14/06 8:40p albertl
 * PR20874:  Added dual had core support for 3563:
 *
 * Hydra_Software_Devel/18   9/7/06 6:47p pntruong
 * PR23744: Update RDB with MAD_0, DNR_0, and TAB_0.
 *
 * Hydra_Software_Devel/17   8/17/06 4:05p hongtaoz
 * PR21401: added support for MAD_0 chip header file;
 *
 * Hydra_Software_Devel/16   6/14/06 1:41p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/15   5/26/06 3:41p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/14   5/19/06 1:48p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/13   10/13/05 11:52a hongtaoz
 * PR17532: added a test API to turn on OSD for deinterlacer;
 *
 * Hydra_Software_Devel/12   8/18/05 1:17p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/2/05 1:07p pntruong
 * PR16484: (1241) Progressive outputs give flickery picture.
 *
 * Hydra_Software_Devel/11   4/15/05 8:09p hongtaoz
 * PR14691: reset window-specific blocks when destroying window;
 *
 * Hydra_Software_Devel/10   4/1/05 5:13p pntruong
 * PR14686: Refresh mad mstart addresses on re-enable and fixed vdec 3d
 * comb mstart alignment.
 *
 * Hydra_Software_Devel/9   1/27/05 12:12p hongtaoz
 * PR13740: improved field repeat detection;
 *
 * Hydra_Software_Devel/8   1/6/05 12:13p hongtaoz
 * PR13740: added MAD support for 7038 C0;
 *
 * Hydra_Software_Devel/7   11/19/04 2:43p yuxiaz
 * PR 10855: Allow user to specify number of capture buffers in VDC
 * default settings. Additional work needed.
 *
 * Hydra_Software_Devel/6   10/1/04 4:05p hongtaoz
 * PR12851: reduced scaler, mad32 and compositor RUL size using block
 * write;
 *
 * Hydra_Software_Devel/5   8/17/04 4:02p pntruong
 * PR12323: Added support for B1 in VDC.
 *
 * Hydra_Software_Devel/4   7/30/04 5:54p pntruong
 * PR10752: channel change will sometimes cause video output to disappear
 * forever.
 *
 * Hydra_Software_Devel/3   7/6/04 8:33a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/2   6/17/04 4:37p pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/1   5/24/04 5:10p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/17/04 7:33p hongtaoz
 * PR10944: added initial MAD_ support;
 *
 ***************************************************************************/
#ifndef BVDC_MAD_PRIV_H__
#define BVDC_MAD_PRIV_H__

#include "bvdc.h"
#include "bavc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"

/***************************************************************************
 * Private defines
 ***************************************************************************/
/* TODO: See also bvdc_feeder_priv.h as example for updating comment */
#define BVDC_P_MAD_VER_0                            (0)
#define BVDC_P_MAD_VER_1                            (1)
#define BVDC_P_MAD_VER_2                            (2)
#define BVDC_P_MAD_VER_3                            (3)
#define BVDC_P_MAD_VER_4                            (4)
#define BVDC_P_MAD_VER_5                            (5)
#define BVDC_P_MAD_VER_6                            (6)
/* 3548 B0:
 * Removed MAD_0_SCHG_MOTION_THRESHOLD
 * Removed MAD_0_DITHER_*
 * TODO: added many */
#define BVDC_P_MAD_VER_7                            (7)

/* 7125C0 added PIXEL_FEED_x_ENABLE */
#define BVDC_P_MAD_VER_8                            (8)


#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/
#if (0!=BVDC_P_SUPPORT_MAD_VER) && (0<BVDC_P_SUPPORT_MAD)

#define BVDC_P_MAD_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_MAD_0_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_MAD_GET_REG_DATA(reg) \
	(hMad->aulRegs[BVDC_P_MAD_GET_REG_IDX(reg)])
#define BVDC_P_MAD_SET_REG_DATA(reg, data) \
	(BVDC_P_MAD_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get with index. */
#define BVDC_P_MAD_GET_REG_DATA_I(reg, idx) \
	(hMad->aulRegs[BVDC_P_MAD_GET_REG_IDX(reg) + (idx)])

/* Get field */
#define BVDC_P_MAD_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_MAD_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_MAD_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_MAD_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_MAD_GET_REG_DATA(reg), reg, field, name))

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_MAD_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hMad->ulRegOffset); \
	*addr_ptr++ = BVDC_P_MAD_GET_REG_DATA(reg); \
}

/* This macro does a block write into RUL */
#define BVDC_P_MAD_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) \
do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hMad->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hMad->aulRegs[BVDC_P_MAD_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while(0)

/* number of registers in one block. */
#define BVDC_P_MAD_REGS_COUNT    \
	BVDC_P_REGS_ENTRIES(MAD_0_REG_START, MAD_0_REG_END)

/* get register offset base on id. */
#define BVDC_P_MAD_GET_REG_OFFSET(mad_id)     (0)

/* Set horizontal scale down ratio */
#define BVDC_P_MAD_SET_HORZ_RATIO(ratio) \
{ \
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_STEP) &= ~( \
		BCHP_MASK(MAD_0_HORIZ_STEP, SIZE)); \
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_STEP) |=  ( \
		BCHP_FIELD_DATA(MAD_0_HORIZ_STEP, SIZE, ratio)); \
}

/* fixed point stuffs */
#define BVDC_P_MAD_H_PAN_SCAN_I_BITS          (1)
#define BVDC_P_MAD_H_PAN_SCAN_F_BITS          (6)

#define BVDC_P_MAD_H_RATIO_I_BITS             (4)
#define BVDC_P_MAD_H_RATIO_F_BITS             (17)

#define BVDC_P_MAD_LUMA_INIT_PHASE_I_BITS     (11)
#define BVDC_P_MAD_LUMA_INIT_PHASE_F_BITS     (6)

#define BVDC_P_MAD_CHROMA_INIT_PHASE_I_BITS   (11)
#define BVDC_P_MAD_CHROMA_INIT_PHASE_F_BITS   (6)

#define BVDC_P_MAD_COEFFS_I_BITS              (1)
#define BVDC_P_MAD_COEFFS_F_BITS              (10)

#define BVDC_P_MAD_LARGEST_F_BITS             (17)
#define BVDC_P_MAD_ZERO_F_BITS                (0)

/* to normalize everything into S14.17 fixed format */
#define BVDC_P_MAD_NORMALIZE(value, f_bit) ((value) << (BVDC_P_MAD_LARGEST_F_BITS - (f_bit)))

/* to innormalize everything from S14.17 fixed format */
#define BVDC_P_MAD_NORM_2_SPEC(value, f_bit) ((value) >> (BVDC_P_MAD_LARGEST_F_BITS - (f_bit)))

/* Miscellaneous constants */
#define BVDC_P_MAD_HORZ_REGIONS_COUNT         (1)

#define BVDC_P_MAD_HORZ_FIR_TAP_COUNT         (8)
#define BVDC_P_MAD_HORZ_FIR_PHASE_COUNT       (8)

#define BVDC_P_MAD_FIR_TAP_COUNT_MAX          (8)
#define BVDC_P_MAD_FIR_PHASE_COUNT_MAX        (8)

#define BVDC_P_MAD_4TAP_HORZ_THRESHOLD_0      (1280)
#define BVDC_P_MAD_4TAP_HORZ_THRESHOLD_1      (1024)

#define BVDC_P_MAD_HORZ_HWF_FACTOR            (2)

/* Make Horizontal ratio */
#define BVDC_P_MAD_MAKE_H_RATIO(src, dst) \
	(BVDC_P_MAD_NORM_2_SPEC((src), BVDC_P_MAD_H_RATIO_F_BITS) / (dst))

#define BVDC_P_MAD_HORZ_1_FIXED BVDC_P_FLOAT_TO_FIXED(1.000, \
	BVDC_P_MAD_LUMA_INIT_PHASE_I_BITS, BVDC_P_MAD_LUMA_INIT_PHASE_F_BITS)

#define BVDC_P_MAD_FIR_COEFFS_MAX \
	(BVDC_P_MAD_FIR_TAP_COUNT_MAX * BVDC_P_MAD_FIR_PHASE_COUNT_MAX)

#define BVDC_P_MAD_LAST UINT32_C(-1)

/* PR26785:  vsyncs to wait before starting 3field, 4field, or 5field game mode */
#define BVDC_P_MAD_GAME_MODE_START_DELAY          (6)

/* PR36539: vsyncs to waiting hardstart finish, before set Mad freeze buffer */
#define BVDC_P_MAD_TRICK_MODE_START_DELAY         (3)

#else  /* #if (0!=BVDC_P_SUPPORT_MAD_VER) */
#define BVDC_P_MAD_REGS_COUNT               1
#endif

/***************************************************************************
 * Software 2:2 reverse pulldown
 ***************************************************************************/
/*
   This value is the minimum PCC count required to use the PCC algorithm.
   If the PCC is below this, the hardware 2:2 algorithm is used
   (as implemented in software).
*/
#define BVDC_P_MAD_MIN_USABLE_PCC           2100 /* PR38878 */

/*
   This value is the multiplier used by the pixel weave check.
*/
#define BVDC_P_MAD_PW_MATCH_MULTIPLIER      0x10

/*
   Indicates how strong the non-weave PCC counter has to be in relation to
   the weave PCC counter. A larger value means that the algorithm would
   be more selective in declaring 2:2 lock.
*/
#define BVDC_P_MAD_PCC_NONMATCH_MATCH_RATIO 8

/*
   Multiplier to apply to histogram[4] (measure of motion) used in conjunction
   with HISTOGRAM4_OFFSET.
*/
#define BVDC_P_MAD_HISTOGRAM4_RATIO_NTSC         8
#define BVDC_P_MAD_HISTOGRAM4_RATIO_PAL          10

/*
   Allowable PCC count in the weave direction assuming that histogram[4]
   (measure of motion) is zero.  A larger value means that the algorithm
   would be less likely to lose lock in low motion.
*/
#define BVDC_P_MAD_HISTOGRAM4_OFFSET_NTSC        (0x2c0)
#define BVDC_P_MAD_HISTOGRAM4_OFFSET_PAL         (0x300)
#define BVDC_P_MAD_HISTOGRAM4_OFFSET_PAL_OVERSAMPLE         (0x900)

/*
   Amount to decrease the phase counter if the motion-adjusted
   (histogram[4]) PCC weave is too high. Recommendation is to make
   the value at least equal to REV22_LOCK_SAT_LEVEL - REV22_EXIT_LOCK_LEVEL
   so that the check can take the chip out of lock immediately.
*/
#define BVDC_P_MAD_HISTOCHECK_DEC           15

/*
   Tolerable delta between PCC counters in the weave and nonweave direction
   before we decrement the phase counter.
*/
#define BVDC_P_MAD_WEAVE_22_THRESHOLD       150
#define BVDC_P_MAD_WEAVE_22_THRESHOLD_OVERSAMPLE       0

/*
  (Emulates RDB register function when PCC counters do not
  exceed MIN_USABLE_PCC).
*/
#define BVDC_P_MAD_UPPER_MATCH_THRESH       (625 << 5)
#define BVDC_P_MAD_LOWER_NONMATCH_THRESH    (468 << 5)
#define BVDC_P_MAD_NONMATCH_MATCH_RATIO     8
#define BVDC_P_MAD_REV22_LOCK_SAT_LEVEL     32
#define BVDC_P_MAD_REV22_ENTER_LOCK_LEVEL   25
#define BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL    20

/*
   If both PCC counts are below this value, we're not getting enough
   information for the PCC method to be useful.
*/
#define BVDC_P_MAD_REV22_DONTCARE           150

/*
   Bad weave threshold for sudden increases in PCC value.
*/
#define BVDC_P_MAD_MAX_PCC_CHANGE           7400

/*
   If PCC in the weave direction is higher than this threshold,
   algorithm will perform a check on the PCC against the repf_motion.
*/
#define BVDC_P_MAD_RM_CHECK_THRESH_NTSC     2000
#define BVDC_P_MAD_RM_CHECK_THRESH_PAL      2750

/*
   Multiplier used in repf_motion check.
*/
#define BVDC_P_MAD_RM_CHECK_RATIO_NTSC      29
#define BVDC_P_MAD_RM_CHECK_RATIO_PAL       28

#define BVDC_P_Mad_MuxAddr(hMad)        (BCHP_VNET_F_MAD_0_SRC + (hMad)->eId * sizeof(uint32_t))
#if (BVDC_P_SUPPORT_MAD > 1)
#define BVDC_P_Mad_PostMuxValue(hMad)   \
   ((0 == (hMad)->eId)? BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_0 : BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_1)
#else
#define BVDC_P_Mad_PostMuxValue(hMad)   (BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_0)
#endif


/***************************************************************************
 * Mad private data structures
 ***************************************************************************/
typedef struct BVDC_P_MadRev22Statistics
{
	uint32_t   ulMatchWeave;
	uint32_t   ulNonMatchWeave;
	uint32_t   ulMatchUM;
	uint32_t   ulNonMatchUM;
	uint32_t   ulAvgWeave;
	uint32_t   ulPixelWeave;
	uint32_t   ulRepfMotion;
} BVDC_P_MadRev22Statistics;

typedef struct BVDC_P_MadRev32Statistics
{
	uint32_t           ulBwvCtrl5;
	uint32_t           ulPhaseCalc0;
	uint32_t           ulPhaseCalc1;
	uint32_t           ulPhaseCalc2;
	uint32_t           ulPhaseCalc8;
	uint32_t           ulPccLumaPcc;
	uint32_t           ulPccChromaPcc;
	uint32_t           ulPrevLumaPcc;
	uint32_t           ulPrevChromaPcc;

	uint32_t           ulWndBias;

	bool               abRev32Locked[5];
	uint32_t           aulSigma[5];
	uint32_t           aulX[5];
	uint32_t           aulP[5];
	uint32_t           aulV[5];
} BVDC_P_MadRev32Statistics;

typedef struct BVDC_P_MadGameModeInfo
{
	BVDC_MadGameMode               eMode;
	uint16_t                       usDelay;
	uint16_t                       usPixelBufferCnt;
	const char                    *pchModeName;

} BVDC_P_MadGameModeInfo;


typedef struct BVDC_P_MadContext
{
	BDBG_OBJECT(BVDC_MAD)

	BREG_Handle                    hRegister;
	BVDC_Heap_Handle               hHeap;
	BVDC_P_WindowId                eWinId;

	BVDC_P_TestFeature1_Settings    stTestFeature1;

	/* flag initial state, require reset; */
	bool                           bInitial;
	uint32_t                       ulResetRegAddr;
	uint32_t                       ulResetMask;

	/* OSD feature for MAD */
	bool                           bEnableOsd;
	uint32_t                       ulOsdHpos;
	uint32_t                       ulOsdVpos;

	/* flag for changes */
	uint32_t                       ulUpdateAll;
	uint32_t                       ulUpdateMisc;
	uint32_t                       ulUpdatePullDown;
	uint32_t                       ulUpdate22PullDown;
	uint32_t                       ulUpdateChromaSettings;
	uint32_t                       ulUpdateMotionSettings;

	/* parameters to used in software 2:2 pulldown algorithm */
	bool                           bReverse22Pulldown;
	bool                           bReverse32Pulldown;

	int32_t                        ulRev22NonMatchMatchRatio;
	int32_t                        ulRev22EnterLockLevel;
	int32_t                        ulRev22LocksatLevel;
	int32_t                        ulRev22BwvLumaThreshold;
	uint32_t                       ulRev22Candidate;
	uint32_t                       ulPrevCtIndexLuma;
	uint32_t                       ulPrevCtIndexChroma;
	BVDC_P_CtInput                 ePrevCtInputType;

	/* Optimized */
	int32_t                        lMadCutLeft; /* S11.6, same fmt as SclCut->lLeft */

	/* for 3:2 pulldown alg */
#if (BVDC_P_SUPPORT_MAD_VER >= 5)
	BVDC_P_MadRev32Statistics      stRev32Statistics;
#endif

	/* for both 3:2 and 2:2 pulldown alg */
	uint32_t                       ulPhaseCounter;
	uint32_t                       ulPhaseCounter22;
	int32_t                        alPhase[5];
	int32_t                        alPhase22[2];

	/* parameters to used in optimize algorithm */
	uint32_t                       ulRepfMotion;
	uint32_t                       aulSigma;
	uint32_t                       ulPreFieldMotion;
	uint32_t                       ulStillFieldNum;
	uint32_t                       ulChangeFieldNum;
	bool                           b5FieldMode;
	bool                           bOptimizeStill;

	/* Fir coeff tables */
	const BVDC_P_FirCoeffTbl      *pHorzFirCoeffTbl;
	const BVDC_P_FirCoeffTbl      *pChromaHorzFirCoeffTbl;

	/* private fields. */
	BVDC_P_MadId                   eId;
	uint32_t                       ulRegOffset; /* MAD_0, MAD_1, and etc. */
	uint32_t                       ulMaxWidth; /* max width limited by line buf size */
	uint32_t                       ulMaxHeight; /* max height limited by RTS */
	uint32_t                       aulRegs[BVDC_P_MAD_REGS_COUNT];

	/* Pixel Field Memory Store */
	BVDC_P_HeapNodePtr             apPixelHeapNode[BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT];
	/* Quantized Motion Field Memory Store */
	BVDC_P_HeapNodePtr             apQmHeapNode[BVDC_P_MAD_QM_BUFFER_COUNT];

	/* values for MAD game mode */
	uint16_t                       usFeedCapture;
	uint16_t                       usCurQm;
	uint16_t                       usGameModeStartDelay;
	uint16_t                       usGameModeQmDelay;
	uint16_t                       usPixelBufferCnt;
	bool                           bBufferCntChanged;

	/* mad user setting */
	BVDC_MadGameMode               eGameMode;
	bool                           bRev32Custom;
	bool                           bRev22Custom;
	bool                           bChromaCustom;
	bool                           bMotionCustom;

	/* flag user changes to optimize MAD code */
	bool                           bUsrChanges;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

	/* last field saved PCC_FWD */
	uint32_t                       ulPccFwd;
	uint32_t                       ulPrePccFwd;

	/* Dither */
	BVDC_P_DitherSetting           stDither;

	/* delay for MAD freeze transition */
	uint16_t                       usTrickModeStartDelay;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
	/* mem saving mode between MAD/ANR */
	bool                            bMemSaving;
#endif

#if (BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
	BVDC_P_Hscaler_Handle           hHscaler;
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) /* PR56938: software workaround */
	uint32_t                        ulHardStartCountdown;
#endif

	uint32_t                        ulGameModeTransCnt;
} BVDC_P_MadContext;


/***************************************************************************
 * Mad private functions
 ***************************************************************************/
BERR_Code BVDC_P_Mad_Create
	( BVDC_P_Mad_Handle            *phMad,
	  BREG_Handle                   hRegister,
	  BVDC_P_MadId                  eMadId,
	  BVDC_P_Resource_Handle        hResource );

void BVDC_P_Mad_Destroy
	( BVDC_P_Mad_Handle             hMad );

BERR_Code BVDC_P_Mad_AcquireConnect
	( BVDC_P_Mad_Handle             hMad,
	  BVDC_Heap_Handle              hHeap,
	  BVDC_P_WindowId               eWinId );

BERR_Code BVDC_P_Mad_ReleaseConnect_isr
	( BVDC_P_Mad_Handle            *phMad );

void BVDC_P_Mad_SetVnetAllocBuf_isr
	( BVDC_P_Mad_Handle             hMad,
	  uint32_t                      ulSrcMuxValue,
	  BVDC_P_VnetPatch              eVnetPatchMode,
	  BVDC_P_BufferHeapId           eMadPixelBufHeapId,
	  BVDC_P_BufferHeapId           eMadQmBufHeapId,
	  uint16_t                      usMadPixelSdBufferCnt);

void BVDC_P_Mad_UnsetVnetFreeBuf_isr
	( BVDC_P_Mad_Handle             hMad );

void BVDC_P_Mad_BuildRul_isr
	( const BVDC_P_Mad_Handle       hMad,
	  BVDC_P_ListInfo              *pList,
	  BVDC_P_State                  eVnetState,
	  BVDC_P_PicComRulInfo         *pPicComRulInfo );

BERR_Code BVDC_P_Mad_SetInfo_isr
	( BVDC_P_Mad_Handle             hMad,
	  const BVDC_Window_Handle      hWindow,
	  const BVDC_P_PictureNodePtr   pPicture );

BERR_Code BVDC_P_Mad_SetEnable_isr
	( BVDC_P_Mad_Handle             hMad,
	  bool                          bEnable );

/***************************************************************************
 * Return the user set of mad chroma
 */
void BVDC_P_Mad_GetUserChroma_isr
	( BVDC_P_Mad_Handle                hMad,
	  BVDC_Deinterlace_ChromaSettings *pstChromaSettings );

/***************************************************************************
 * Return the user set of mad motino
 */
void BVDC_P_Mad_GetUserMotion_isr
	( BVDC_P_Mad_Handle                hMad,
	  BVDC_Deinterlace_MotionSettings *pstMotionSettings );

/***************************************************************************
 * Return the user set of mad 3:2 pulldown
 */
void BVDC_P_Mad_GetUserReverse32_isr
	( BVDC_P_Mad_Handle                   hMad,
	  BVDC_Deinterlace_Reverse32Settings *pstRev32Settings );

/***************************************************************************
 * Return the user set of mad chroma
 */
void BVDC_P_Mad_GetUserReverse22_isr
	( BVDC_P_Mad_Handle                   hMad,
	  BVDC_Deinterlace_Reverse22Settings *pstRev22Settings );

void BVDC_P_Mad_Init_Default
	( BVDC_MadGameMode                      *peGameMode,
	  BPXL_Format                           *pePxlFormat,
	  BVDC_Mode                             *pePqEnhancement,
	  BVDC_Video_TestFeature1_Settings      *pVideoTestFeature1,
	  bool                                  *pbShrinkWidth,
	  bool                                  *pbReverse32Pulldown,
	  BVDC_Deinterlace_Reverse32Settings    *pReverse32Settings,
	  bool                                  *pbReverse22Pulldown,
	  BVDC_Deinterlace_Reverse22Settings    *pReverse22Settings,
	  BVDC_Deinterlace_ChromaSettings       *pChromaSettings,
	  BVDC_Deinterlace_MotionSettings       *pMotionSettings );

void BVDC_P_Mad_Init_Custom
	( BVDC_422To444UpSampler          *pUpSampler,
	  BVDC_444To422DnSampler          *pDnSampler,
	  BVDC_Deinterlace_LowAngleSettings *pLowAngles );

BERR_Code BVDC_P_Mad_Init_DynamicDefault
	( BVDC_Window_Handle                  hWindow,
	  BVDC_Deinterlace_Reverse32Settings *pReverse32Settings,
	  BVDC_Deinterlace_Reverse22Settings *pReverse22Settings,
	  BVDC_Deinterlace_ChromaSettings    *pChromaSettings );

uint16_t BVDC_P_Mad_GetVsyncDelayNum_isr
	( BVDC_MadGameMode                 eGameMode );

uint16_t BVDC_P_Mad_GetPixBufCnt_isr
	(  BVDC_MadGameMode                eGameMode );

bool BVDC_P_Mad_BeHardStart_isr
	( BVDC_P_Mad_Handle                hMad );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_MAD_PRIV_H__ */
/* End of file. */
