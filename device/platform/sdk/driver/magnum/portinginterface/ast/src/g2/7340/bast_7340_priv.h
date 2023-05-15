/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7340_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 10/15/12 4:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/7340/bast_7340_priv.h $
 * 
 * Hydra_Software_Devel/15   10/15/12 4:07p enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/14   9/6/12 6:24p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/13   8/28/12 10:43a ronchan
 * SW7340-13: incremented code version to 0x9
 * 
 * Hydra_Software_Devel/12   4/5/12 11:43a ronchan
 * SW7340-13: incremented code version to 0x8
 * 
 * Hydra_Software_Devel/11   6/11/11 5:30p ronchan
 * SW7340-13: incremented code version to 0x7
 * 
 * Hydra_Software_Devel/10   5/13/11 5:56p ronchan
 * SW7340-13: incremented code version to 0x6
 * 
 * Hydra_Software_Devel/9   3/22/11 3:05p ronchan
 * SW7340-13: incremented code version to 0x5
 * 
 * Hydra_Software_Devel/8   9/29/10 2:57p ronchan
 * SW7340-13: incremented code version to 0x4
 * 
 * Hydra_Software_Devel/7   8/9/10 3:16p ronchan
 * SW7340-13: incremented code version to 0x3
 * 
 * Hydra_Software_Devel/6   7/1/10 2:27p ronchan
 * SW7340-13: incremented code version to 0x2
 * 
 * Hydra_Software_Devel/5   1/11/10 10:32a ronchan
 * SW7340-13: incremented code version to 0x1
 * 
 * Hydra_Software_Devel/4   8/11/09 3:41p ronchan
 * PR 55235: update turbo interrupts
 * 
 * Hydra_Software_Devel/3   8/10/09 3:45p ronchan
 * PR 55235: changed xtal frequency to 54 MHz
 * 
 * Hydra_Software_Devel/2   7/21/09 4:48p ronchan
 * PR 55235: use turbo interrupt shift definitions
 * 
 * Hydra_Software_Devel/1   6/16/09 2:05p ronchan
 * PR 55235: add support for 7340
 *
 ***************************************************************************/
#ifndef _BAST_7340_PRIV_H__
#define _BAST_7340_PRIV_H__               

#include "bchp_qpsk.h"
#include "bchp_qpsk_intr2_0.h"
#include "bchp_qpsk_intr2_1.h"
#include "bchp_sds.h"
#include "bchp_sds_intr2_0.h"
#include "bchp_sds_intr2_1.h"
#include "bchp_tuner.h"
#include "bchp_tfec.h"
#include "bchp_tfec_intr2.h"
#include "bchp_afec.h"
#include "bchp_grb.h"
#include "bchp_ftm_intr2.h"
#include "bchp_ftm_phy.h"
#include "bchp_ftm_uart.h"
#include "bchp_ftm_skit.h"


#define BCHP_INT_ID_SDS_LOCK_IS_0               BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS2_RVIL_IS_SHIFT + 8)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_0           BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS2_RVOL_IS_SHIFT + 8)
#define BCHP_INT_ID_SDS_LOCK_IS_1               BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS2_RVIL_IS_SHIFT + 8)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_1           BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS2_RVOL_IS_SHIFT + 8)
#define BCHP_INT_ID_SDS_BTM_IS_0                BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_BTM_IS_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_0               BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_BRTM_IS_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_GENTM_IS1_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_GENTM_IS2_SHIFT)
#define BCHP_INT_ID_SDS_XTLTM_IS_0              BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_XTLTM_IS_SHIFT)
#define BCHP_INT_ID_SDS_BTM_IS_1                BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_BTM_IS_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_1               BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_BRTM_IS_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_1             BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_GENTM_IS1_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_1             BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_GENTM_IS2_SHIFT)
#define BCHP_INT_ID_SDS_XTLTM_IS_1              BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_XTLTM_IS_SHIFT)
#define BCHP_INT_ID_SDS_if_agc_le_thresh_rise_0 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_if_agc_le_thresh_rise_SHIFT)
#define BCHP_INT_ID_SDS_if_agc_le_thresh_fall_0 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_if_agc_le_thresh_fall_SHIFT)
#define BCHP_INT_ID_SDS_rf_agc_le_thresh_rise_0 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_rf_agc_le_thresh_rise_SHIFT)
#define BCHP_INT_ID_SDS_rf_agc_le_thresh_fall_0 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_rf_agc_le_thresh_fall_SHIFT)
#define BCHP_INT_ID_SDS_rf_agc_gt_max_change_0  BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_rf_agc_gt_max_change_SHIFT)
#define BCHP_INT_ID_SDS_if_agc_le_thresh_rise_1 BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_if_agc_le_thresh_rise_SHIFT)
#define BCHP_INT_ID_SDS_if_agc_le_thresh_fall_1 BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_if_agc_le_thresh_fall_SHIFT)
#define BCHP_INT_ID_SDS_rf_agc_le_thresh_rise_1 BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_rf_agc_le_thresh_rise_SHIFT)
#define BCHP_INT_ID_SDS_rf_agc_le_thresh_fall_1 BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_rf_agc_le_thresh_fall_SHIFT)
#define BCHP_INT_ID_SDS_rf_agc_gt_max_change_1  BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_1_CPU_STATUS, BCHP_SDS_IRQMSK5_rf_agc_gt_max_change_SHIFT)
#define BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh_0   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_sar_vol_gt_hi_thrsh_SHIFT)
#define BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh_0   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_IRQSTS5_sar_vol_lt_lo_thrsh_SHIFT)
#define BCHP_INT_ID_DSDN_IS_0                   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_DSDN_IS_SHIFT)
#define BCHP_INT_ID_DSDN_IS_1                   BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_DSDN_IS_SHIFT)
#define BCHP_INT_ID_SDS_HP_IS_0                 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS2_HP_IS_SHIFT + 8)
#define BCHP_INT_ID_SDS_HP_IS_1                 BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS2_HP_IS_SHIFT + 8)
#define BCHP_INT_ID_MI2C_IS_0                   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_MIC_IS_SHIFT)
#define BCHP_INT_ID_MI2C_IS_1                   BCHP_INT_ID_CREATE(BCHP_QPSK_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS1_MIC_IS_SHIFT)

#define BCHP_INT_ID_TURBO_LOCK_IS_0             BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TIL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_LOCK_IS_1             0
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_0         BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TOL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_1         0


/* #define BCHP_INT_ID_AFEC_LOCK_IS_0              BCHP_INT_ID_CREATE(BCHP_AFEC_uP_IRQ_STAT, BCHP_AFEC_uP_IRQ_STAT_uP_ldpc_lock_SHIFT) */
#define BCHP_INT_ID_AFEC_LOCK_IS_0              BCHP_INT_ID_CREATE(BCHP_AFEC_uP_IRQ_STAT, BCHP_AFEC_uP_IRQ_STAT_uP_mp_lock_SHIFT)
#define BCHP_INT_ID_AFEC_LOCK_IS_1              0
#define BCHP_INT_ID_AFEC_NOT_LOCK_IS_0          BCHP_INT_ID_CREATE(BCHP_AFEC_uP_IRQ_STAT, BCHP_AFEC_uP_IRQ_STAT_uP_mp_lock_edge_SHIFT)
#define BCHP_INT_ID_AFEC_NOT_LOCK_IS_1          0

#define BAST_G2_MAX_CHANNELS 2
#define BAST_G2_XTAL_FREQ 54000000
#define BAST_G2_CHIP 0x7340
#define BAST_G2_BUILD_VERSION   0x0A
#define BAST_G2_TUNER_DAC_DIV_RATIO 0x13

#endif /* BAST_7340_PRIV_H__ */

