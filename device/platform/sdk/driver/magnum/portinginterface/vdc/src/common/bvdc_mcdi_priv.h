/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_mcdi_priv.h $
* $brcm_Revision: Hydra_Software_Devel/42 $
* $brcm_Date: 8/23/12 1:21p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_mcdi_priv.h $
* 
* Hydra_Software_Devel/42   8/23/12 1:21p vanessah
* SW7429-243: add support for 7429B0/7435B0/7548A0
* 
* Hydra_Software_Devel/41   5/11/12 4:33p vanessah
* SW7346-819: fix compile warnings
* 
* Hydra_Software_Devel/40   5/11/12 1:49p vanessah
* SW7346-819: mcdi xchroma setting depends on source
* 
* Hydra_Software_Devel/39   5/8/12 2:19p vanessah
* SW7435-25: align 7435 MCDI with C model golden value
* 
* Hydra_Software_Devel/38   5/7/12 6:23p vanessah
* SW7418-96: fix the testfeature propogation error
* 
* Hydra_Software_Devel/37   3/26/12 10:56a yuxiaz
* SW7425-2753: Fixed BVN errors for 3d conversion.
* 
* Hydra_Software_Devel/36   3/8/12 5:28p vanessah
* SW7435-31: Sync MADR golden value and removal EoP trigger workaround
* 
* Hydra_Software_Devel/35   1/12/12 5:40p vanessah
* SW7425-2162: change 7425 MADR QM buffer allocation
* 
* Hydra_Software_Devel/34   11/23/11 4:23p vanessah
* SW7425-1765: sync with 7425B0 golden set
* 
* Hydra_Software_Devel/33   11/2/11 7:11p vanessah
* SW7425-1115: mcvp code cleanup
* 
* Hydra_Software_Devel/32   9/28/11 4:16p pntruong
* SW7425-969: Refactored mcvp/madr code to be multi-instances friendly.
* Adapt to vnet naming changes.
*
* Hydra_Software_Devel/31   9/23/11 5:18p pntruong
* SW7231-383: Correct programming of game mode and delay callback
* reporting for madr/mcvp.
*
* Hydra_Software_Devel/30   9/23/11 10:37a pntruong
* HW7552-62, HW7428-4, HWBCM7231B0-81, HW7425-938: Excluded work-arounds
* for chips with fix.
*
* Hydra_Software_Devel/29   8/26/11 4:15p pntruong
* SW7552-43: Deinterlacer hardware requires 3:2 pulldown to be off if
* content is 50hz base.
*
* Hydra_Software_Devel/28   7/18/11 8:47a vanessah
* SW7425-309: mcdi default value set
*
* Hydra_Software_Devel/27   7/11/11 6:10p vanessah
* SW7231-291: SW7425-689: change QM default value and sync application
* and PI MADR compression value
*
* Hydra_Software_Devel/26   6/29/11 11:42a vanessah
* SW7425-488: update default OBTS register value
*
* Hydra_Software_Devel/25   6/28/11 12:59p tdo
* SW7420-1967: Video blinking is occurred whenever the channel changes.
* [Deinterlace on]
*
* Hydra_Software_Devel/24   5/5/11 2:42p vanessah
* SW7425-309: SW7425-488: undo the wrong merge of version 54
*
* Hydra_Software_Devel/23   5/5/11 12:06p syang
* SW7420-1768: mcdi/madr can not set freeze bit in 1st 4 fields
*
* Hydra_Software_Devel/21   5/4/11 4:03p vanessah
* SW7425-309: Seperate MCDI and MADR initialization
*
* Hydra_Software_Devel/20   4/19/11 6:09p vanessah
* SW7425-314: change REV32_LOCK_SAT_LEVEL from 24 to 32 for spiderman
* stream
*
* Hydra_Software_Devel/19   4/19/11 5:36p vanessah
* SW7425-314: change REV32_LOCK_SAT_LEVEL default value from 12 to 28
*
* Hydra_Software_Devel/18   4/18/11 11:47a vanessah
* SW7425-314: change the MIPS reading to RUL one
*
* Hydra_Software_Devel/17   4/18/11 8:20a vanessah
* SW7425-374: turn on osd for Mcdi debug
*
* Hydra_Software_Devel/16   3/29/11 1:51p tdo
* SW7344-36: Playback trick modes cause Video flicker and bands
*
* Hydra_Software_Devel/15   2/25/11 10:21a vanessah
* SW7425-124: fix compile error for 7358 755 7231 7346 7344 7420 chip
*
* Hydra_Software_Devel/14   1/7/11 4:46p yuxiaz
* SW7422-174: Added DCX support to MCVP and MADR.
*
* Hydra_Software_Devel/13   1/6/11 9:47a pntruong
* SW7422-174: Initialized madr with compression enabled to match up with
* default rts for reference usage.
*
* Hydra_Software_Devel/12   12/28/10 5:02p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/11   11/24/10 1:24p tdo
* SW7422-52: Bring-up MAD-R
*
* Hydra_Software_Devel/10   11/15/10 4:41p tdo
* SW7422-36: Handle "left only" method for BVN internal data path
*
* Hydra_Software_Devel/9   11/5/10 3:09p vanessah
* SW7422-52:  Two kinds of deinterlacers in the same chip
*
* Hydra_Software_Devel/8   11/3/10 10:24p tdo
* SW7420-1224: Add trick mode setting to MCDI
*
* Hydra_Software_Devel/7   9/2/10 1:37p vanessah
* SW7422-52:  MAD + MCVP both on 7422
*
* Hydra_Software_Devel/6   8/26/10 2:38p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:28p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/SW7420-588/1   8/9/10 7:22p vanessah
* SW7420-588:  panscan to zoom video loss
*
* Hydra_Software_Devel/4   5/7/10 7:19p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings
*
* Hydra_Software_Devel/3   4/7/10 11:32a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/2   4/5/10 4:11p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/1   3/16/09 11:10p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/2   2/12/09 6:16p tdo
* PR 45785, PR 45789: Use correct delay for MCDI
*
* Hydra_Software_Devel/7420_mcvp/1   1/23/09 8:56p syang
* PR 45785, PR 45789: init MCVP implementation
*
***************************************************************************/
#ifndef BVDC_MCDI_PRIV_H__
#define BVDC_MCDI_PRIV_H__

#include "bvdc.h"
#include "bavc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_window_priv.h"

/***************************************************************************
 * Private defines
 ***************************************************************************/
/***************************************************************************
 * MCDI
 ***************************************************************************/
/* 7420 */
#define BVDC_P_MCDI_VER_0                        (0)
#define BVDC_P_MCDI_VER_1                        (1)
/* 7422 */
#define BVDC_P_MCDI_VER_2                        (2)
/* 7425 */
#define BVDC_P_MCDI_VER_3                        (3)
/* 7425Bx */
#define BVDC_P_MCDI_VER_4                        (4)
/* 7435Ax */
#define BVDC_P_MCDI_VER_5                        (5)


/***************************************************************************
 * MADR
 ***************************************************************************/
/* 7358 7552 7231 7346 7344 */
#define BVDC_P_MADR_VER_2                        (2)

/* 7422Ax 7425Ax, needs BVDC_P_MADR_HSIZE_WORKAROUND, BVDC_P_MADR_PICSIZE_WORKAROUND */
#define BVDC_P_MADR_VER_3                        (3)

/* 7358A1:
 * fixed BVDC_P_MADR_HSIZE_WORKAROUND */
#define BVDC_P_MADR_VER_4                        (4)

/* 7231Bx, 7344Bx, 7346Bx, 7552Bx, 7425Bx, 7429Ax, 7435Ax
 * fixed BVDC_P_MADR_HSIZE_WORKAROUND and BVDC_P_MADR_PICSIZE_WORKAROUND,
 * WAIT_DMA_DONE, etc. */
#define BVDC_P_MADR_VER_5                        (5)
/* 7435Ax SUSPEND_EOP */
#define BVDC_P_MADR_VER_6                        (6)
/* 7435Bx/7429Bx/7584Ax Low delay from 1 field to zero */
#define BVDC_P_MADR_VER_7                        (7)
#ifdef __cplusplus
extern "C" {
#endif

	/***************************************************************************
	* Private register cracking macros
	***************************************************************************/

#define BVDC_P_MCDI_SRC_HORZ_THRESHOLD         (960)

	/***************************************************************************
	* Software 2:2 reverse pulldown
	***************************************************************************/
	/*
	This value is the minimum PCC count required to use the PCC algorithm.
	If the PCC is below this, the hardware 2:2 algorithm is used
	(as implemented in software).
	*/
#define BVDC_P_MCDI_MIN_USABLE_PCC           2100 /* PR38878 */

	/*
	This value is the multiplier used by the pixel weave check.
	*/
#define BVDC_P_MCDI_PW_MATCH_MULTIPLIER      0x10

	/*
	Indicates how strong the non-weave PCC counter has to be in relation to
	the weave PCC counter. A larger value means that the algorithm would
	be more selective in declaring 2:2 lock.
	*/
#define BVDC_P_MCDI_PCC_NONMATCH_MATCH_RATIO 8

	/*
	Multiplier to apply to histogram[4] (measure of motion) used in conjunction
	with HISTOGRAM4_OFFSET.
	*/
#define BVDC_P_MCDI_HISTOGRAM4_RATIO_NTSC         8
#define BVDC_P_MCDI_HISTOGRAM4_RATIO_PAL          10

	/*
	Allowable PCC count in the weave direction assuming that histogram[4]
	(measure of motion) is zero.  A larger value means that the algorithm
	would be less likely to lose lock in low motion.
	*/
#define BVDC_P_MCDI_HISTOGRAM4_OFFSET_NTSC        (0x2c0)
#define BVDC_P_MCDI_HISTOGRAM4_OFFSET_PAL         (0x300)
#define BVDC_P_MCDI_HISTOGRAM4_OFFSET_PAL_OVERSAMPLE         (0x900)

	/*
	Amount to decrease the phase counter if the motion-adjusted
	(histogram[4]) PCC weave is too high. Recommendation is to make
	the value at least equal to REV22_LOCK_SAT_LEVEL - REV22_EXIT_LOCK_LEVEL
	so that the check can take the chip out of lock immediately.
	*/
#define BVDC_P_MCDI_HISTOCHECK_DEC           15

	/*
	Tolerable delta between PCC counters in the weave and nonweave direction
	before we decrement the phase counter.
	*/
#define BVDC_P_MCDI_WEAVE_22_THRESHOLD       150
#define BVDC_P_MCDI_WEAVE_22_THRESHOLD_OVERSAMPLE       0

	/*
	(Emulates RDB register function when PCC counters do not
	exceed MIN_USABLE_PCC).
	*/
#define BVDC_P_MCDI_UPPER_MATCH_THRESH       (625 << 5)
#define BVDC_P_MCDI_LOWER_NONMATCH_THRESH    (468 << 5)
#define BVDC_P_MCDI_NONMATCH_MATCH_RATIO     8
#define BVDC_P_MCDI_REV22_LOCK_SAT_LEVEL     32

	/*
	If both PCC counts are below this value, we're not getting enough
	information for the PCC method to be useful.
	*/
#define BVDC_P_MCDI_REV22_DONTCARE           150

	/*
	Bad weave threshold for sudden increases in PCC value.
	*/
#define BVDC_P_MCDI_MAX_PCC_CHANGE           7400

	/*
	If PCC in the weave direction is higher than this threshold,
	algorithm will perform a check on the PCC against the repf_motion.
	*/
#define BVDC_P_MCDI_RM_CHECK_THRESH_NTSC     2000
#define BVDC_P_MCDI_RM_CHECK_THRESH_PAL      2750

	/*
	Multiplier used in repf_motion check.
	*/
#define BVDC_P_MCDI_RM_CHECK_RATIO_NTSC      29
#define BVDC_P_MCDI_RM_CHECK_RATIO_PAL       28


	/* constant value of 0 and 1	   */
#define BVDC_P_MCDI_ZERO                      0x0
#define BVDC_P_MCDI_ONE                       0x1

#define BVDC_MADR_DCXS_HALFBITS_PER_PIXEL     0x12
#define BVDC_P_MADR_DCXS_COMPRESSION(ulBitsPerPixel)     \
	(ulBitsPerPixel>BVDC_MADR_DCXS_HALFBITS_PER_PIXEL)?11:9
	/*
	MADR register golden set value
	*/
	/*
	FCN Module
	*/

	/* IT_FIELD_PHASE_CALC_CONTROL_0.            */
#define BVDC_P_MADR_REV32_ENTER_LOCK_LEVEL                 0x8
#define BVDC_P_MADR_REV32_EXIT_LOCK_LEVEL                  0x7
#define BVDC_P_MADR_REV32_PHASE_MATCH_THRESH               0x4

	/* IT_FIELD_PHASE_CALC_CONTROL_2             */
#define BVDC_P_MADR_REV32_LOCK_SAT_LEVEL                   0x20
#define BVDC_P_MADR_REV32_BAD_EDIT_LEVEL                   0x927

	/* OBTS_DECAY								 */
#define BVDC_P_MADR_OBTS_DECAY                             0x8

	/* OBTS_HOLDOFF 							 */
#define BVDC_P_MADR_OBTS_HOLDOFF                           0x3840

	/* OBTS_MAX_HOLDOFF 						 */
#define BVDC_P_MADR_OBTS_MAX_HOLDOFF                       0x23280


	/* OBTS_OBTS_CORE                            */
#define BVDC_P_MADR_OBTS_CTRL_CORE                         0x7080
	/*
	PPB Module
	*/
	/* MDI_PPB_0_QM_MAPPING_RANGE                   */
#define BVDC_P_MADR_QM_MAPPING_RANGE_VALUE_1               0x20
#define BVDC_P_MADR_QM_MAPPING_RANGE_VALUE_2               0x40
#define BVDC_P_MADR_QM_MAPPING_RANGE_VALUE_3               0x60

	/* MDI_PPB_0_MOTION_CAL_CONTROL                 */
#define BVDC_P_MADR_WEAVE_DETECT_THR                       0x10
#define BVDC_P_MADR_WEAVE_MOTION_THR_LOW                   0x16

	/* XCHROMA_CONTROL_4                            */
#define BVDC_P_MADR_XCHROMA_CTRL_MAX                       0xff

	/* LA_SCALE_0                         */
#define BVDC_P_MADR_LA_SCALE_0_DEGREE                      8

	/*
	MCDI register golden set value
	*/

/*
	FCB Module
	*/

	/*MC_DECISION_CONTROL_0                      */
#define BVDC_P_MCDI_FCB_MC_CTRL_PCC_TOTAL_THRESH           0x180

	/*IT_FIELD_PHASE_CALC_CONTROL_1*/
#if (BVDC_P_MADR_VER_6<=BVDC_P_SUPPORT_MADR_VER)
#define BVDC_P_MCDI_REV32_REPF_VETO_LEVEL_SD               0x120
#else
#define BVDC_P_MCDI_REV32_REPF_VETO_LEVEL_SD               0x300
#endif
#define BVDC_P_MCDI_REV32_REPF_VETO_LEVEL_HD               0xF00
#define BVDC_P_MCDI_REV32_REPF_PIX_LEVEL_SD                0x300
#define BVDC_P_MCDI_REV32_REPF_PIX_LEVEL_HD                0xF00

	/*IT_FIELD_PHASE_CALC_CONTROL_2*/
#if (BVDC_P_MADR_VER_6<=BVDC_P_SUPPORT_MADR_VER)
#define BVDC_P_MCDI_REV32_BAD_EDIT_LEVEL_SD                0x1000
#else
#define BVDC_P_MCDI_REV32_BAD_EDIT_LEVEL_SD                0xa00
#endif
#define BVDC_P_MCDI_REV32_BAD_EDIT_LEVEL_HD                0x3200

	/*REV32_IT_FIELD_PHASE_CALC_CONTROL_6*/
#if (BVDC_P_MADR_VER_6<=BVDC_P_SUPPORT_MADR_VER)
#define BVDC_P_MCDI_REV32_BW_LG_PCC_RATIO                  0x2a
#else
#define BVDC_P_MCDI_REV32_BW_LG_PCC_RATIO                  0x32
#endif
#define BVDC_P_MCDI_REV32_BW_LG_PCC_MIN                    0x28

	/*REV32_IT_FIELD_PHASE_CALC_CONTROL_4*/
	/*REV32_IT_FIELD_PHASE_CALC_CONTROL_4_MCDI   */
#define BVDC_P_MCDI_REV32_REPF_FL_RATIO                    0x8
#define BVDC_P_MCDI_REV32_REPF_FL_MIN                      0x400

	/* 	REV22_IT_FIELD_PHASE_CALC_CONTROL_0.     */
#define BVDC_P_MCDI_REV22_NONMATCH_MATCH_RATIO             0x8
#if (BVDC_P_MADR_VER_6<=BVDC_P_SUPPORT_MADR_VER)
#define BVDC_P_MCDI_REV22_ENTER_LOCK_LEVEL                 0xf
#define BVDC_P_MCDI_REV22_EXIT_LOCK_LEVEL                  0xe
#else
#define BVDC_P_MCDI_REV22_ENTER_LOCK_LEVEL                 0x19
#define BVDC_P_MCDI_REV22_EXIT_LOCK_LEVEL                  0x14
#endif

	/* 	REV22_IT_FIELD_PHASE_CALC_CONTROL_3.     */
#define BVDC_P_MCDI_REV22_MIN_USABLE_PCC                   0x514
#define BVDC_P_MCDI_REV22_PW_MATCH_MULTIPLIER              0x240

	/*	REV22_IT_FIELD_PHASE_CALC_CONTROL_3_MCDI.*/
#define BVDC_P_MCDI_REV22_MIN_USABLE_PCC_MCDI              0x514
#define BVDC_P_MCDI_REV22_PW_MATCH_MULTIPLIER_MCDI         0x3E8

	/*	REV22_IT_FIELD_PHASE_CALC_CONTROL_8.     */
#define BVDC_P_MCDI_REV22_ALMOST_LOCK_LEVEL                0x17

	/*	REV22_IT_FIELD_PHASE_CALC_CONTROL_9.     */
#define BVDC_P_MCDI_REV22_BW_LG_PCC_RATIO                  0xB0
#define BVDC_P_MCDI_REV22_BW_LG_PCC_MIN                    0xA

	/*	REV22_IT_FIELD_PHASE_CALC_CONTROL_11.    */
#define BVDC_P_MCDI_REV22_BW_LG_PCC_MAXIMUM                0x7D0

	/*	BCHP_MDI_FCB_0_BWV_CONTROL_1             */
#define BVDC_P_MCDI_BWV_LUMA32_THD_SD                      0x44c
#define BVDC_P_MCDI_BWV_LUMA32_THD_HD                      0x3a98
#define BVDC_P_MCDI_BWV_LUMA32_AVG_THD_SD                  0x7d0
#define BVDC_P_MCDI_BWV_LUMA32_AVG_THD_HD                  0x2710
	/*	BCHP_MDI_FCB_0_BWV_CONTROL_3             */
#define BVDC_P_MCDI_BWV_TKR_PCC_MULT                       0xffff
#define BVDC_P_MCDI_BWV_TKR_VETO_LEVEL_SD                  0x80
#define BVDC_P_MCDI_BWV_TKR_VETO_LEVEL_HD                  0x280

#define BVDC_P_MCDI_SC_THD                                 0x20
#define BVDC_P_MCDI_SC_PST_THD                             0x8
	/*
	PPB Module
	*/
	/* MC_BLEND                                  */
#define BVDC_P_MCDI_MC_BLEND_K0                            0x120

	/* MEMC_CTRL                                 */
#if (BVDC_P_MCDI_VER_3 >= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_MEMC_CTRL_LAMDA                        0x4
#elif (BVDC_P_MCDI_VER_4 <= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_MEMC_CTRL_LAMDA                        0x10
#endif

	/*MC_COST_CTRL_02                            */
#define BVDC_P_MCDI_PCC_CNT_THD_1                          0x5
#define BVDC_P_MCDI_PCC_CNT_THD_0                          0x3
#define BVDC_P_MCDI_PCC_CNT_THD                            0x4
#define BVDC_P_MCDI_ZERO_PCC_CORE_THD                      0x10
#define BVDC_P_MCDI_MC_PCC_CORE_THD                        0x4

	/*MC_COST_CTRL_04                            */
#define BVDC_P_MCDI_EDGE_K2                                0xe
#define BVDC_P_MCDI_EDGE_K1                                0x2
#define BVDC_P_MCDI_EDGE_K0                                0xc

	/* MC_MC_THD                                 */
#if (BVDC_P_MCDI_VER_3 >= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_MC_MC_THD_HIGH                         0x28
#elif (BVDC_P_MCDI_VER_4 <= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_MC_MC_THD_HIGH                         0x27
#endif
#define BVDC_P_MCDI_MC_MC_THD_LOW                          0x8

	/* MC_ZERO_THD_0                             */
#define BVDC_P_MCDI_ZERO_THD_LOW_1                         0xc
#define BVDC_P_MCDI_ZERO_THD_LOW_0                         0xc

	/* MC_ZERO_THD_1                             */
#if (BVDC_P_MCDI_VER_3 >= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_ZERO_THD_HIGH                          0x14
#elif (BVDC_P_MCDI_VER_4 <= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_ZERO_THD_HIGH                          0x13
#endif

	/* MC_SHIFT_MC_THD                           */
#define BVDC_P_MCDI_SHIFT_MC_THD_1                         0x74a
#if (BVDC_P_MCDI_VER_3 >= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_SHIFT_MC_THD_0                         0x80
#elif (BVDC_P_MCDI_VER_4 <= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_SHIFT_MC_THD_0                         0x81
#endif
	/* MC_EDGE_THD_0                             */
#define BVDC_P_MCDI_EDGE_THD_LOW_1                         0x2
#define BVDC_P_MCDI_EDGE_THD_LOW_0                         0x0

	/* MC_EDGE_THD_1                             */
#define BVDC_P_MCDI_EDGE_THD_HIGH_1                        0xfe

#if (BVDC_P_MCDI_VER_3 >= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_EDGE_THD_HIGH_0                        0x64
	/* MC_EDGE_THD_2                             */
#define BVDC_P_MCDI_EDGE_THD_HIGH_2                        0xFA0
#elif (BVDC_P_MCDI_VER_4 <= BVDC_P_SUPPORT_MCDI_VER )
#define BVDC_P_MCDI_EDGE_THD_HIGH_0                        0x65
	/* MC_EDGE_THD_2                             */
#define BVDC_P_MCDI_EDGE_THD_HIGH_2                        0xFA1
#endif

	/* MC_VM_THD                                 */
#define BVDC_P_MCDI_VM_THD_1                               0x0
#define BVDC_P_MCDI_VM_THD_0                               0x0

	/* MC_SHIFT_ZERO_THD                         */
#define BVDC_P_MCDI_SHIFT_ZERO_THD                         0x3B8

	/* MOVIE_MC_THD                              */
#define BVDC_P_MCDI_MOVIE_MC_THD                           0xa8

	/* BCHP_MDI_PPB_0_MVD_K1_VALUE               */
#define BVDC_P_MVD_K1_VALUE                                0xc80

	/* XCHROMA_CONTROL_6                         */
#define BVDC_P_MCDI_MA_420422_BLEND_STRENGTH               0x8
#define BVDC_P_MCDI_IT_420422_BLEND_STRENGTH               0x8

	/* BCHP_MDI_PPB_0_GMV_NUM_THD                */
#define BVDC_P_MCDI_GMV_NUM_THD                            0x9c40


	/* MH_MAPPING_VALUE                          */
#define BVDC_P_MCDI_MH_MAPPING_VALUE_3                     0xFF
#define BVDC_P_MCDI_MH_MAPPING_VALUE_2                     0x60
#define BVDC_P_MCDI_MH_MAPPING_VALUE_1                     0x30

	/* MA_MC_BLEND_CTRL_0                        */
#define BVDC_P_MCDI_STAT_THRESH_GMV_CHOSEN                 0x3FF
#define BVDC_P_MCDI_STAT_THRESH                            0x17

	/* MA_MC_BLEND_CTRL_1                        */
#define BVDC_P_MCDI_STAT_SCALE_GMV_CHOSEN                  0x100
#define BVDC_P_MCDI_STAT_SCALE                             0x10

	/*
	QM Address mask 32-byte alignment
	*/
#define BVDC_P_QM_ADDR_MASK                                0xffffffe0

	/*
	due to HW requirement, FIELD_FREEZE can not be set for the first 4 fields
	*/
#define BVDC_P_MCDI_TRICK_MODE_START_DELAY                  4

/***************************************************************************
* Mcdi private data structures
***************************************************************************/
typedef struct BVDC_P_McdiRev22Statistics
{
	uint32_t   ulMatchWeave;
	uint32_t   ulNonMatchWeave;
	uint32_t   ulMatchUM;
	uint32_t   ulNonMatchUM;
	uint32_t   ulAvgWeave;
	uint32_t   ulPixelWeave;
	uint32_t   ulRepfMotion;
} BVDC_P_McdiRev22Statistics;

typedef struct BVDC_P_McdiRev32Statistics
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
	uint32_t           ulHistogramBin0Reg;
	uint32_t           ulHistogramBin1Reg;
	uint32_t           ulHistogramBin2Reg;
	uint32_t           ulHistogramBin3Reg;
	uint32_t           ulHistogramBin4Reg;
	uint32_t           ulWndBias;

	bool               abRev32Locked[5];
	uint32_t           aulSigma[5];
	uint32_t           aulX[5];
	uint32_t           aulP[5];
	uint32_t           aulV[5];
} BVDC_P_McdiRev32Statistics;

typedef struct BVDC_P_McdiGameModeInfo
{
	BVDC_MadGameMode               eMode;
	uint16_t                       usDelay;
	uint16_t                       usPixelBufferCnt;
	const char                    *pchModeName;

} BVDC_P_McdiGameModeInfo;

/****************************************************************************
* Mcdi dirty bits to makr RUL building and executing dirty.
*/
typedef union
{
	struct
	{
		uint32_t                   bSize           : 1;
		uint32_t                   bPxlFmt         : 1;
	} stBits;

	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_McdiDirtyBits;

typedef struct BVDC_P_McdiContext
{
	BDBG_OBJECT(BVDC_MDI)

	BREG_Handle                    hRegister;
	BVDC_Heap_Handle               hHeap;
	BVDC_P_WindowId                eWinId;

	BVDC_P_TestFeature1_Settings   stTestFeature1;

	/* flag initial state, require reset; */
	bool                           bInitial;

	/* OSD feature for MCDI */
	bool                           bEnableOsd;
	uint32_t                       ulOsdHpos;
	uint32_t                       ulOsdVpos;

	/* flag for changes */
	uint32_t                       ulUpdateAll;

	/* Optimized */
	int32_t                        lMcdiCutLeft; /* S11.6, same fmt as SclCut->lLeft */

	/* private fields. */
	BVDC_P_McdiId                  eId;
	uint32_t                       ulRegOffset; /* MCDI_0, MCDI_1, and etc. */
	bool                           bMadr;

	/*uint32_t                     aulRegs[BVDC_P_MAD_REGS_COUNT];*/

	/* Pixel Field Memory Store */
	BVDC_P_HeapNodePtr             apHeapNode[BVDC_P_MAX_MCDI_BUFFER_COUNT];

	/* QM Field Memory Store */
	BVDC_P_HeapNodePtr             apQmHeapNode[BVDC_P_MCDI_QM_BUFFER_COUNT];

	/* set as change is marked, clear after built into RUL */
	BVDC_P_McdiDirtyBits           stSwDirty;

	/* set after built into RUL, clear after executed */
	BVDC_P_McdiDirtyBits           stHwDirty;

	uint32_t                       ulHSize;
	uint32_t                       ulVSize;

	/* values for MCDI game mode */
	uint16_t                       usFeedCapture;
	uint16_t                       usCurQm;
	uint16_t                       usGameModeStartDelay;
	uint16_t                       usGameModeQmDelay;
	uint16_t                       usAllocBufferCnt;
	bool                           bBufferCntChanged;

	/* video source and fmt */
	BVDC_Deinterlace_ChromaSettings *pChromaSettings;
	BVDC_P_ChromaType               eChromaType;

	/* mcdi user setting */
	BVDC_MadGameMode               eGameMode;
	bool                           bRev32Pulldown;
	bool                           bRev22Pulldown;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

	/* delay for MCDI freeze transition */
	uint16_t                       usTrickModeStartDelay;

	BFMT_Orientation               ePrevDispOrientation;
	BFMT_Orientation               ePrevSrcOrientation;
	
} BVDC_P_McdiContext;


/***************************************************************************
 * Mcdi private functions
 ***************************************************************************/
#define BVDC_P_MCDI_GET_REG_OFFSET(mad_id)     (0)
#define BVDC_P_Mcdi_MuxAddr(hMcdi)             (0)
#define BVDC_P_Mcdi_PostMuxValue(hMcdi)        (0)


/***************************************************************************
* {private}
*/
BERR_Code BVDC_P_Mcdi_Create
	( BVDC_P_Mcdi_Handle           *phMcdi,
	  BVDC_P_McdiId                 eMcdiId,
	  BREG_Handle                   hRegister,
	  BVDC_P_Resource_Handle        hResource );

void BVDC_P_Mcdi_Destroy
	( BVDC_P_Mcdi_Handle            hMcdi );

void BVDC_P_Mcdi_Init
	( BVDC_P_Mcdi_Handle            hMcdi );

void BVDC_P_Mcdi_BuildRul_SrcInit_isr
	( BVDC_P_McdiContext           *pMcdi,
	  BVDC_P_ListInfo              *pList );

void BVDC_P_Mcdi_BuildRul_SetEnable_isr
	( BVDC_P_McdiContext            *pMcdi,
	  BVDC_P_ListInfo               *pList,
	  bool                           bEnable,
	  bool                           bRepeat,
	  BAVC_Polarity                  eSrcNxtFldId,
	  BAVC_FrameRateCode             eFrameRate,
	  BFMT_Orientation               eOrientation,
	  bool                           bInit );

bool BVDC_P_Mcdi_BeHardStart_isr
	( BVDC_P_Mcdi_Handle             hMcdi );

void BVDC_P_Mcdi_GetUserConf_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_P_Deinterlace_Settings   *pMadSettings);

void BVDC_P_Mcdi_Init_Chroma_DynamicDefault
	(BVDC_P_Mcdi_Handle                 hMcdi,
	BVDC_Deinterlace_ChromaSettings   *pChromaSettings,
	const BFMT_VideoInfo               *pFmtInfo,
	bool                                bMfdSrc);

uint16_t BVDC_P_Mcdi_GetVsyncDelayNum_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_MadGameMode               eGameMode);

uint16_t BVDC_P_Mcdi_GetPixBufCnt_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_MadGameMode               eGameMode);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef BVDC_MCDI_PRIV_H__ */
/* End of file. */

