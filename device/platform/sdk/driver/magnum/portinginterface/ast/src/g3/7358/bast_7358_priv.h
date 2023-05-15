/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7358_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 10/15/12 4:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/7358/bast_7358_priv.h $
 * 
 * Hydra_Software_Devel/19   10/15/12 4:16p enavarro
 * SWSATFE-220: renamed BAST_Gx_BUILD_VERSION to BAST_G3_BUILD_VERSION
 * 
 * Hydra_Software_Devel/18   10/15/12 10:52a enavarro
 * SWSATFE-220: implement new version numbering scheme
 * 
 * Hydra_Software_Devel/17   10/8/12 1:55p ronchan
 * SWSATFE-75: increment minor revision to 0x9
 * 
 * Hydra_Software_Devel/16   9/7/12 10:15a ronchan
 * SWSATFE-214: renamed header definition
 * 
 * Hydra_Software_Devel/15   7/23/12 3:08p ronchan
 * SWSATFE-75: increment minor revision to 0x8
 * 
 * Hydra_Software_Devel/14   4/20/12 2:55p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/13   4/3/12 10:23a ronchan
 * SWSATFE-75: increment minor revision to 0x7
 * 
 * Hydra_Software_Devel/12   3/1/12 4:22p ronchan
 * SWSATFE-75: increment minor revision to 0x6
 * 
 * Hydra_Software_Devel/11   12/5/11 3:44p ronchan
 * SWSATFE-75: increment minor revision to 0x5
 * 
 * Hydra_Software_Devel/10   10/25/11 10:50a ronchan
 * SWSATFE-75: increment minor revision to 0x4
 * 
 * Hydra_Software_Devel/9   8/3/11 5:11p ronchan
 * SWSATFE-75: increment minor revision to 0x3
 * 
 * Hydra_Software_Devel/8   6/28/11 6:38p ronchan
 * SWSATFE-75: increment minor revision to 0x2
 * 
 * Hydra_Software_Devel/7   6/20/11 3:05p enavarro
 * SWSATFE-130: use sds ahb interrupts for AFEC lock/not_lock callbacks
 * 
 * Hydra_Software_Devel/6   6/16/11 9:10a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/5   4/1/11 8:17a enavarro
 * SWSATFE-86: move BAST_DEFAULT_SAMPLE_FREQ definition in chip-specific
 * file
 * 
 * Hydra_Software_Devel/4   12/8/10 2:12p enavarro
 * SWSATFE-75: added turbo sync detect interrupt
 * 
 * Hydra_Software_Devel/3   10/14/10 2:23p ronchan
 * SWSATFE-75: switch diseqc timers in SDS_INTR2_1_CPU_STATUS
 * 
 * Hydra_Software_Devel/2   9/17/10 5:21p ronchan
 * SWSATFE-75: added required bchp headers
 * 
 * Hydra_Software_Devel/1   8/31/10 2:15p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#ifndef _BAST_7358_PRIV_H__
#define _BAST_7358_PRIV_H__               

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
#include "bchp_sds_ntch.h"
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

#define BAST_G3_BUILD_VERSION 10  /* increment this for every 7358 ast pi release */


#define BCHP_INT_ID_SDS_LOCK_IS_0               BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_rvb_in_lock_SHIFT)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_0           BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_rvb_out_lock_SHIFT)
#define BCHP_INT_ID_SDS_BTM_IS_0                BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_bclktimer_int_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_0               BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_bertimer_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_gentimer1_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_gentimer2_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS3_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_gentimer3_int_SHIFT)
#define BCHP_INT_ID_SDS_HP_IS_0                 BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_hp_state_match_SHIFT)
#define BCHP_INT_ID_HP_FRAME_BOUNDARY_0         BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_hp_frame_boundary_SHIFT)
#define BCHP_INT_ID_MI2C_IS_0                   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_mi2c_int_SHIFT)

#define BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh_0   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_sar_vol_gt_thresh_SHIFT)
#define BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh_0   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_sar_vol_lt_thresh_SHIFT)
#define BCHP_INT_ID_DSDN_IS_0                   BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_done_SHIFT)
#define BCHP_INT_ID_DISEQC_TIMER1_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_timer2_SHIFT)  /* NOTE diseqc timers are switched in SDS_INTR2_1_CPU_STATUS */
#define BCHP_INT_ID_DISEQC_TIMER2_0             BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_timer1_SHIFT)  /* NOTE diseqc timers are switched in SDS_INTR2_1_CPU_STATUS */
#define BCHP_INT_ID_DISEQC_tx_fifo_a_empty_0    BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_1_CPU_STATUS, BCHP_SDS_INTR2_1_CPU_STATUS_dsec_tx_fifo_a_empty_SHIFT)

#define BCHP_INT_ID_TURBO_LOCK_IS_0             BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TIL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_0         BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TOL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_SYNC_0                BCHP_INT_ID_CREATE(BCHP_TFEC_INTR2_CPU_STATUS, BCHP_TFEC_INTR2_CPU_STATUS_TRIL_INTR_SHIFT)

#define BCHP_INT_ID_AFEC_LOCK_IS_0              BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_ahb_in_lock_SHIFT)
#define BCHP_INT_ID_AFEC_NOT_LOCK_IS_0          BCHP_INT_ID_CREATE(BCHP_SDS_INTR2_0_CPU_STATUS, BCHP_SDS_INTR2_0_CPU_STATUS_ahb_out_lock_SHIFT)

#define BAST_G3_MAX_CHANNELS 1
#define BAST_G3_XTAL_FREQ    54000000
#define BAST_G3_CHIP         0x7358

#define BAST_DEFAULT_SAMPLE_FREQ 184500000

#endif /* BAST_7358_PRIV_H__ */

