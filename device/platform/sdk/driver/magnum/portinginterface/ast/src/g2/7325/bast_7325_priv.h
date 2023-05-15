/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7325_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/37 $
 * $brcm_Date: 10/15/12 4:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/7325/bast_7325_priv.h $
 * 
 * Hydra_Software_Devel/37   10/15/12 4:07p enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/36   9/6/12 6:22p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/35   8/9/12 4:13p ronchan
 * SW7325-30: incremented code version to 0x1B
 * 
 * Hydra_Software_Devel/34   4/11/12 4:16p ronchan
 * SW7325-30: incremented code version to 0x1A
 * 
 * Hydra_Software_Devel/33   3/18/11 2:55p ronchan
 * SW7325-30: incremented code version to 0x19
 * 
 * Hydra_Software_Devel/32   1/28/11 11:45a ronchan
 * SW7325-30: incremented code version to 0x18
 * 
 * Hydra_Software_Devel/31   9/29/10 2:41p ronchan
 * SW7325-30: incremented code version to 0x17
 * 
 * Hydra_Software_Devel/30   3/19/10 4:28p ronchan
 * SW7325-30: incremented code version to 0x16
 * 
 * Hydra_Software_Devel/29   3/9/10 4:28p ronchan
 * SW7325-30: incremented code version to 0x15
 * 
 * Hydra_Software_Devel/28   2/15/10 11:19a ronchan
 * SW7325-30: incremented code version to 0x14
 * 
 * Hydra_Software_Devel/27   1/25/10 3:46p ronchan
 * SW7325-30: incremented code version to 0x13
 * 
 * Hydra_Software_Devel/26   12/18/09 9:14a enavarro
 * SW7325-30: incremented code version to 0x12
 * 
 * Hydra_Software_Devel/25   11/12/09 2:48p ronchan
 * SW7325-30: incremented code version to 0x11
 * 
 * Hydra_Software_Devel/24   11/3/09 4:41p ronchan
 * SW7325-30: incremented code version to 0x10
 * 
 * Hydra_Software_Devel/22   9/23/09 3:11p ronchan
 * SW7325-30: incremented code version to 0xF
 * 
 * Hydra_Software_Devel/21   8/4/09 6:06p ronchan
 * PR 37769: incremented code version to 0xE
 * 
 * Hydra_Software_Devel/20   7/21/09 4:41p ronchan
 * PR 37769: use turbo interrupt shift definitions
 * 
 * Hydra_Software_Devel/19   7/7/09 2:06p ronchan
 * PR 37769: incremented code version to 0xD
 * 
 * Hydra_Software_Devel/18   6/3/09 10:02a ronchan
 * PR 37769: incremented code version to 0xC
 * 
 * Hydra_Software_Devel/17   3/19/09 1:58p ronchan
 * PR 37769: incremented code version to 0xB
 * 
 * Hydra_Software_Devel/16   2/18/09 3:39p ronchan
 * PR 37769: incremented code version to 0xA
 * 
 * Hydra_Software_Devel/15   1/16/09 3:02p ronchan
 * PR 37769: incremented code version to 0x9
 * 
 * Hydra_Software_Devel/14   12/17/08 11:15a ronchan
 * PR 37769: incremented code version to 0x8
 * 
 * Hydra_Software_Devel/13   11/20/08 4:08p ronchan
 * PR 37769: incremented code version to 0x7
 * 
 * Hydra_Software_Devel/12   10/13/08 3:08p ronchan
 * PR 37769: removed config version
 * 
 * Hydra_Software_Devel/11   10/9/08 10:11a ronchan
 * PR 37769: incremented code version to 0x6
 * 
 * Hydra_Software_Devel/10   9/18/08 5:06p ronchan
 * PR 37769: incremented code version
 * 
 * Hydra_Software_Devel/9   8/21/08 2:29p ronchan
 * PR 37769: incremented code version
 * 
 * Hydra_Software_Devel/8   8/5/08 4:57p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/7   4/28/08 2:47p ronchan
 * PR 37769: added vsense interrupts, increased default dac div ratio
 * 
 * Hydra_Software_Devel/6   3/14/08 11:03a enavarro
 * PR 37769: set AFEC lock irq to be mpeg lock irq
 * 
 * Hydra_Software_Devel/5   3/10/08 4:33p enavarro
 * PR 37769: set BAST_73XX_CODE_VERSION to 0x03
 * 
 * Hydra_Software_Devel/4   2/12/08 9:45a enavarro
 * PR 37769: change LDPC not locked irq source to be mpeg lost lock
 * interrupt
 * 
 * Hydra_Software_Devel/3   1/25/08 11:09a enavarro
 * PR 37769: set BAST_73XX_CODE_VERSION to 0x02
 * 
 * Hydra_Software_Devel/2   12/14/07 1:23p ronchan
 * PR 37769: added tuner dac div ratio
 * 
 * Hydra_Software_Devel/1   11/21/07 11:35a ronchan
 * PR 32395: initial version
 *
 ***************************************************************************/
#ifndef _BAST_7325_PRIV_H__
#define _BAST_7325_PRIV_H__               

#include "bchp_qpsk.h"
#include "bchp_qpsk_intr2_0.h"
#include "bchp_qpsk_intr2_1.h"
#include "bchp_sds.h"
#include "bchp_sds_intr2_0.h"
#include "bchp_sds_intr2_1.h"
#include "bchp_tuner.h"
#include "bchp_tfec.h"
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

#define BCHP_INT_ID_TURBO_LOCK_IS_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS3_TIL_IS_SHIFT + 16)
#define BCHP_INT_ID_TURBO_LOCK_IS_1             0
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_0         BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_IRQSTS3_TOL_IS_SHIFT + 16)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_1         0


/* #define BCHP_INT_ID_AFEC_LOCK_IS_0              BCHP_INT_ID_CREATE(BCHP_AFEC_uP_IRQ_STAT, BCHP_AFEC_uP_IRQ_STAT_uP_ldpc_lock_SHIFT) */
#define BCHP_INT_ID_AFEC_LOCK_IS_0              BCHP_INT_ID_CREATE(BCHP_AFEC_uP_IRQ_STAT, BCHP_AFEC_uP_IRQ_STAT_uP_mp_lock_SHIFT)
#define BCHP_INT_ID_AFEC_LOCK_IS_1              0
#define BCHP_INT_ID_AFEC_NOT_LOCK_IS_0          BCHP_INT_ID_CREATE(BCHP_AFEC_uP_IRQ_STAT, BCHP_AFEC_uP_IRQ_STAT_uP_mp_lock_edge_SHIFT)
#define BCHP_INT_ID_AFEC_NOT_LOCK_IS_1          0

#define BAST_G2_MAX_CHANNELS 2
#define BAST_G2_XTAL_FREQ 27000000
#define BAST_G2_CHIP 0x7325
#define BAST_G2_BUILD_VERSION   0x1C
#define BAST_G2_TUNER_DAC_DIV_RATIO 0x27

#endif /* BAST_7325_PRIV_H__ */

