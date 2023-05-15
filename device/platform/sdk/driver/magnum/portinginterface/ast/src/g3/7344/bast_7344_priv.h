/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7344_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 10/15/12 4:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/7344/bast_7344_priv.h $
 * 
 * Hydra_Software_Devel/18   10/15/12 4:16p enavarro
 * SWSATFE-220: renamed BAST_Gx_BUILD_VERSION to BAST_G3_BUILD_VERSION
 * 
 * Hydra_Software_Devel/17   10/15/12 10:57a enavarro
 * SWSATFE-220: implement BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/16   10/8/12 2:06p ronchan
 * SWSATFE-75: increment minor revision to 0x2
 * 
 * Hydra_Software_Devel/15   9/7/12 10:02a ronchan
 * SWSATFE-214: renamed header definition
 * 
 * Hydra_Software_Devel/14   6/12/12 2:52p ckpark
 * SWSATFE-202: version 1.0
 * 
 * Hydra_Software_Devel/13   5/10/12 9:49a ronchan
 * SW7360-13: remove cwc shift mapping
 * 
 * Hydra_Software_Devel/12   4/20/12 2:57p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/11   10/17/11 2:15p enavarro
 * SWSATFE-150: added some cwc definitions
 * 
 * Hydra_Software_Devel/10   10/14/11 1:32p ronchan
 * SW7344-178: revert diseqc timers for A0 as well
 * 
 * Hydra_Software_Devel/9   10/13/11 11:41a ronchan
 * SW7344-178: switched diseqc timers back for B0
 * 
 * Hydra_Software_Devel/8   9/16/11 9:51a ronchan
 * SW7358-131: fix value for BCHP_VER
 * 
 * Hydra_Software_Devel/7   8/22/11 2:22p ronchan
 * SW7344-178: redefined FTM PHY ana registers for B0
 * 
 * Hydra_Software_Devel/6   8/22/11 10:59a ronchan
 * SW7344-178: replace notch filters with cw canceller for B0
 * 
 * Hydra_Software_Devel/5   5/5/11 5:50p ronchan
 * SWSATFE-75: diseqc timers still swapped
 * 
 * Hydra_Software_Devel/4   4/1/11 8:15a enavarro
 * SWSATFE-86: move BAST_DEFAULT_SAMPLE_FREQ definition in chip-specific
 * file
 * 
 * Hydra_Software_Devel/3   1/28/11 4:11p ronchan
 * SWSATFE-75: diseqc timers swapped back in rdb
 * 
 * Hydra_Software_Devel/2   12/8/10 2:12p enavarro
 * SWSATFE-75: added turbo sync detect interrupt
 * 
 * Hydra_Software_Devel/1   11/15/10 5:19p ronchan
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#ifndef _BAST_7344_PRIV_H__
#define _BAST_7344_PRIV_H__               

#include "bchp_afec.h"
#include "bchp_afec_global.h"
#include "bchp_afec_gr_bridge.h"
#include "bchp_afec_intr_ctrl2.h"

#include "bchp_sds_agc.h"
#include "bchp_sds_bert.h"
#include "bchp_sds_bl.h"
#include "bchp_sds_cg.h"
#include "bchp_sds_cl.h"
#include "bchp_sds_dft.h"
#include "bchp_sds_dsec.h"
#include "bchp_sds_eq.h"
#include "bchp_sds_fe.h"
#include "bchp_sds_fec.h"
#include "bchp_sds_gr_bridge.h"
#include "bchp_sds_hp.h"
#include "bchp_sds_intr2_0.h"
#include "bchp_sds_intr2_1.h"
#include "bchp_sds_misc.h"
#if (BCHP_VER == BCHP_VER_A0)
#include "bchp_sds_ntch.h"
#else
#include "bchp_sds_cwc.h"
#endif
#include "bchp_sds_oi.h"
#include "bchp_sds_snr.h"
#include "bchp_sds_tuner.h"
#include "bchp_sds_vit.h"

#include "bchp_tfec.h"
#include "bchp_tfec_gr_bridge.h"
#include "bchp_tfec_intr2.h"
#include "bchp_tfec_misc.h"

#include "bchp_ftm_gr_bridge.h"
#include "bchp_ftm_intr2.h"
#include "bchp_ftm_phy.h"
#include "bchp_ftm_skit.h"
#include "bchp_ftm_uart.h"
#if (BCHP_VER > BCHP_VER_A0)
#include "bchp_ftm_phy_ana.h"
#define BCHP_FTM_PHY_ANA0_0 BCHP_FTM_PHY_ANA_CTL0_0
#define BCHP_FTM_PHY_ANA0_1 BCHP_FTM_PHY_ANA_CTL0_1
#define BCHP_FTM_PHY_ANA1_0 BCHP_FTM_PHY_ANA_CTL1_0
#define BCHP_FTM_PHY_ANA1_1 BCHP_FTM_PHY_ANA_CTL1_1
#endif


#define BAST_G3_BUILD_VERSION 1 


#define BCHP_INT_ID_SDS_LOCK_IS_0               BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_rvb_in_lock_SHIFT)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_0           BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_rvb_out_lock_SHIFT)
#define BCHP_INT_ID_SDS_BTM_IS_0                BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_bclktimer_int_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_0               BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_bertimer_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_gentimer1_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_gentimer2_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS3_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_gentimer3_int_SHIFT)
#define BCHP_INT_ID_SDS_HP_IS_0                 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_hp_state_match_SHIFT)
#define BCHP_INT_ID_MI2C_IS_0                   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_mi2c_int_SHIFT)

#define BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh_0   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_sar_vol_gt_thresh_SHIFT)
#define BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh_0   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_sar_vol_lt_thresh_SHIFT)
#define BCHP_INT_ID_DSDN_IS_0                   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_done_SHIFT)
#define BCHP_INT_ID_DISEQC_TIMER1_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_timer1_SHIFT)
#define BCHP_INT_ID_DISEQC_TIMER2_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_timer2_SHIFT)
#define BCHP_INT_ID_DISEQC_tx_fifo_a_empty_0    BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_tx_fifo_a_empty_SHIFT)

#define BCHP_INT_ID_TURBO_LOCK_IS_0             BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TIL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_0         BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TOL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_SYNC_0                BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TRIL_INTR_SHIFT)

#define BCHP_INT_ID_AFEC_LOCK_IS_0              BCHP_INT_ID_CREATE(BCHP_AFEC_INTR_CTRL2_CPU_STATUS, BCHP_AFEC_INTR_CTRL2_CPU_STATUS_MP_LOCK_INTR_SHIFT)
#define BCHP_INT_ID_AFEC_NOT_LOCK_IS_0          BCHP_INT_ID_CREATE(BCHP_AFEC_INTR_CTRL2_CPU_STATUS, BCHP_AFEC_INTR_CTRL2_CPU_STATUS_MP_LOCK_NEG_INTR_SHIFT)

#define BAST_G3_MAX_CHANNELS 1
#define BAST_G3_XTAL_FREQ    54000000
#define BAST_G3_CHIP         0x7344

#define BAST_DEFAULT_SAMPLE_FREQ 184500000

#endif /* BAST_7344_PRIV_H__ */

