/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4517.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 7/13/12 10:45a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/g3/4517/bast_4517.h $
 * 
 * Hydra_Software_Devel/7   7/13/12 10:45a enavarro
 * SWSATFE-140: added new registers
 * 
 * Hydra_Software_Devel/6   6/7/12 3:55p enavarro
 * SWSATFE-140: fixed compile problem on 4517
 * 
 * Hydra_Software_Devel/5   4/20/12 3:15p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/4   1/11/12 10:12a ronchan
 * SWSATFE-140: switch diseqc timers in DSEC_INTR2_CPU_STATUS
 * 
 * Hydra_Software_Devel/3   1/6/12 8:14a enavarro
 * SWSATFE-140: fixed interrupt IDs
 * 
 * Hydra_Software_Devel/2   10/17/11 3:35p enavarro
 * SWSATFE-140: added cwc definitions
 * 
 * Hydra_Software_Devel/1   10/17/11 3:18p enavarro
 * SWSATFE-140: initial check-in
 *
 ***************************************************************************/
#ifndef _BAST_4517_H__
#define _BAST_4517_H__               

#include "bchp_leap_l1.h"
#include "bchp_sds_agc_0.h"
#include "bchp_sds_agc_1.h"
#include "bchp_sds_agc_2.h"
#include "bchp_sds_bert_0.h"
#include "bchp_sds_bert_1.h"
#include "bchp_sds_bert_2.h"
#include "bchp_sds_bl_0.h"
#include "bchp_sds_bl_1.h"
#include "bchp_sds_bl_2.h"
#include "bchp_sds_cg_0.h"
#include "bchp_sds_cg_1.h"
#include "bchp_sds_cg_2.h"
#include "bchp_sds_cl_0.h"
#include "bchp_sds_cl_1.h"
#include "bchp_sds_cl_2.h"
#include "bchp_sds_dft_0.h"
#include "bchp_sds_dft_1.h"
#include "bchp_sds_dft_2.h"
#include "bchp_sds_dsec.h"
#include "bchp_sds_eq_0.h"
#include "bchp_sds_eq_1.h"
#include "bchp_sds_eq_2.h"
#include "bchp_sds_fe_0.h"
#include "bchp_sds_fe_1.h"
#include "bchp_sds_fe_2.h"
#include "bchp_sds_fec_0.h"
#include "bchp_sds_fec_1.h"
#include "bchp_sds_fec_2.h"
#include "bchp_sds_gr_bridge_0.h"
#include "bchp_sds_gr_bridge_1.h"
#include "bchp_sds_gr_bridge_2.h"
#include "bchp_sds_hp_0.h"
#include "bchp_sds_hp_1.h"
#include "bchp_sds_hp_2.h"
#include "bchp_sds_intr2_0_0.h"
#include "bchp_sds_intr2_0_1.h"
#include "bchp_sds_intr2_0_2.h"
#include "bchp_sds_misc_0.h"
#include "bchp_sds_misc_1.h"
#include "bchp_sds_misc_2.h"
#include "bchp_sds_cwc_0.h"
#include "bchp_sds_cwc_1.h"
#include "bchp_sds_cwc_2.h"
#include "bchp_sds_oi_0.h"
#include "bchp_sds_oi_1.h"
#include "bchp_sds_oi_2.h"
#include "bchp_sds_snr_0.h"
#include "bchp_sds_snr_1.h"
#include "bchp_sds_snr_2.h"
#include "bchp_sds_tuner_0.h"
#include "bchp_sds_tuner_1.h"
#include "bchp_sds_tuner_2.h"
#include "bchp_sds_vit_0.h"
#include "bchp_sds_vit_1.h"
#include "bchp_sds_vit_2.h"
#include "bchp_sds_dsec_intr2.h"
#include "bchp_tfec_0.h"
#include "bchp_tfec_1.h"
#include "bchp_tfec_2.h"
#include "bchp_tfec_gr_bridge_0.h"
#include "bchp_tfec_gr_bridge_1.h"
#include "bchp_tfec_gr_bridge_2.h"
#include "bchp_tfec_intr2_0.h"
#include "bchp_tfec_intr2_1.h"
#include "bchp_tfec_intr2_2.h"
#include "bchp_tfec_misc_0.h"
#include "bchp_tfec_misc_1.h"
#include "bchp_tfec_misc_2.h"
#include "bchp_ftm_intr2.h"
#include "bchp_ftm_phy.h"
#include "bchp_ftm_skit.h"
#include "bchp_ftm_uart.h"
#include "bchp_ftm_phy_ana.h"


#define BAST_G3_MAX_CHANNELS 3
#define BAST_G3_XTAL_FREQ    54000000
#define BAST_G3_CHIP         0x4517
#define BAST_G3_MAJOR_VERSION 0x00
#define BAST_G3_MINOR_VERSION 0x01

#define BAST_DEFAULT_SAMPLE_FREQ 184500000

#define BAST_LEAP_INT_ID_CREATE(L1,L2) ((L1<<8)|L2)

/* channel 0 interrupts */
#define BCHP_INT_ID_SDS_LOCK_IS_0               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_in_lock_SHIFT)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_0           BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_out_lock_SHIFT)
#define BCHP_INT_ID_SDS_BTM_IS_0                BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_bclktimer_int_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_0               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_bertimer_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_0             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer1_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_0             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer2_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS3_0             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer3_int_SHIFT)
#define BCHP_INT_ID_SDS_HP_IS_0                 BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_state_match_SHIFT)
#define BCHP_INT_ID_MI2C_IS_0                   BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS0_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_mi2c_int_SHIFT)
#define BCHP_INT_ID_TURBO_LOCK_IS_0             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC0_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TIL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_0         BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC0_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TOL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_SYNC_0                BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC0_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TRIL_INTR_SHIFT)

/* channel 1 interrupts */
#define BCHP_INT_ID_SDS_LOCK_IS_1               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_in_lock_SHIFT)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_1           BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_out_lock_SHIFT)
#define BCHP_INT_ID_SDS_BTM_IS_1                BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_bclktimer_int_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_1               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_bertimer_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_1             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer1_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_1             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer2_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS3_1             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer3_int_SHIFT)
#define BCHP_INT_ID_SDS_HP_IS_1                 BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_state_match_SHIFT)
#define BCHP_INT_ID_MI2C_IS_1                   BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS1_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_mi2c_int_SHIFT)
#define BCHP_INT_ID_TURBO_LOCK_IS_1             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC1_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TIL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_1         BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC1_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TOL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_SYNC_1                BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC1_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TRIL_INTR_SHIFT)

/* channel 2 interrupts */
#define BCHP_INT_ID_SDS_LOCK_IS_2               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_in_lock_SHIFT)
#define BCHP_INT_ID_SDS_NOT_LOCK_IS_2           BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_out_lock_SHIFT)
#define BCHP_INT_ID_SDS_BTM_IS_2                BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_bclktimer_int_SHIFT)
#define BCHP_INT_ID_SDS_BRTM_IS_2               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_bertimer_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS1_2             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer1_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS2_2             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer2_int_SHIFT)
#define BCHP_INT_ID_SDS_GENTM_IS3_2             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer3_int_SHIFT)
#define BCHP_INT_ID_SDS_HP_IS_2                 BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_state_match_SHIFT)
#define BCHP_INT_ID_MI2C_IS_2                   BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_SDS2_INTR_SHIFT, BCHP_SDS_INTR2_0_0_CPU_STATUS_mi2c_int_SHIFT)
#define BCHP_INT_ID_TURBO_LOCK_IS_2             BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC2_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TIL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_NOT_LOCK_IS_2         BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC2_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TOL_INTR_SHIFT)
#define BCHP_INT_ID_TURBO_SYNC_2                BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_TFEC2_INTR_SHIFT, BCHP_TFEC_INTR2_0_CPU_STATUS_TRIL_INTR_SHIFT)

/* diseqc interrupts */
#define BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh     BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_DSEC0_INTR_SHIFT, BCHP_SDS_DSEC_INTR2_CPU_STATUS_dsec_sar_vol_gt_thresh_SHIFT)
#define BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh     BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_DSEC0_INTR_SHIFT, BCHP_SDS_DSEC_INTR2_CPU_STATUS_dsec_sar_vol_lt_thresh_SHIFT)
#define BCHP_INT_ID_DSDN_IS                     BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_DSEC0_INTR_SHIFT, BCHP_SDS_DSEC_INTR2_CPU_STATUS_dsec_done_SHIFT)
#define BCHP_INT_ID_DISEQC_TIMER1               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_DSEC0_INTR_SHIFT, BCHP_SDS_DSEC_INTR2_CPU_STATUS_dsec_timer2_SHIFT) /* NOTE diseqc timers are switched in SDS_DSEC_INTR2_CPU_STATUS */
#define BCHP_INT_ID_DISEQC_TIMER2               BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_DSEC0_INTR_SHIFT, BCHP_SDS_DSEC_INTR2_CPU_STATUS_dsec_timer1_SHIFT) /* NOTE diseqc timers are switched in SDS_DSEC_INTR2_CPU_STATUS */
#define BCHP_INT_ID_DISEQC_tx_fifo_a_empty_0    BAST_LEAP_INT_ID_CREATE(BCHP_LEAP_L1_INTR_STATUS_DSEC0_INTR_SHIFT, BCHP_SDS_DSEC_INTR2_CPU_STATUS_dsec_tx_fifo_a_empty_SHIFT)


/* register definitions for multiple channels */
#define BCHP_SDS_AGC_AGCCTL BCHP_SDS_AGC_0_AGCCTL
#define BCHP_SDS_AGC_IAGCTH BCHP_SDS_AGC_0_IAGCTH
#define BCHP_SDS_AGC_DSGIN BCHP_SDS_AGC_0_DSGIN
#define BCHP_SDS_AGC_ATHR BCHP_SDS_AGC_0_ATHR
#define BCHP_SDS_AGC_ABW BCHP_SDS_AGC_0_ABW
#define BCHP_SDS_AGC_AII BCHP_SDS_AGC_0_AII
#define BCHP_SDS_AGC_AGI BCHP_SDS_AGC_0_AGI
#define BCHP_SDS_AGC_AIT BCHP_SDS_AGC_0_AIT
#define BCHP_SDS_AGC_AGT BCHP_SDS_AGC_0_AGT
#define BCHP_SDS_AGC_AGCLI BCHP_SDS_AGC_0_AGCLI
#define BCHP_SDS_BERT_BERCTL BCHP_SDS_BERT_0_BERCTL
#define BCHP_SDS_BERT_BEIT BCHP_SDS_BERT_0_BEIT
#define BCHP_SDS_BERT_BERC BCHP_SDS_BERT_0_BERC
#define BCHP_SDS_BERT_BEM1 BCHP_SDS_BERT_0_BEM1
#define BCHP_SDS_BERT_BEM2 BCHP_SDS_BERT_0_BEM2
#define BCHP_SDS_BERT_BEM3 BCHP_SDS_BERT_0_BEM3
#define BCHP_SDS_BERT_BEST BCHP_SDS_BERT_0_BEST
#define BCHP_SDS_BERT_ACMCTL BCHP_SDS_BERT_0_ACMCTL
#define BCHP_SDS_BL_BLPCTL BCHP_SDS_BL_0_BLPCTL
#define BCHP_SDS_BL_PFCTL BCHP_SDS_BL_0_PFCTL
#define BCHP_SDS_BL_BRSW BCHP_SDS_BL_0_BRSW
#define BCHP_SDS_BL_BRLC BCHP_SDS_BL_0_BRLC
#define BCHP_SDS_BL_BRIC BCHP_SDS_BL_0_BRIC
#define BCHP_SDS_BL_BRI BCHP_SDS_BL_0_BRI
#define BCHP_SDS_BL_BFOS BCHP_SDS_BL_0_BFOS
#define BCHP_SDS_BL_BRFO BCHP_SDS_BL_0_BRFO
#define BCHP_SDS_BL_BNCO BCHP_SDS_BL_0_BNCO
#define BCHP_SDS_CG_RSTCTL BCHP_SDS_CG_0_RSTCTL
#define BCHP_SDS_CG_CGDIV00 BCHP_SDS_CG_0_CGDIV00
#define BCHP_SDS_CG_CGDIV01 BCHP_SDS_CG_0_CGDIV01
#define BCHP_SDS_CG_SPLL_NPDIV BCHP_SDS_CG_0_SPLL_NPDIV
#define BCHP_SDS_CG_SPLL_MDIV_CTRL BCHP_SDS_CG_0_SPLL_MDIV_CTRL
#define BCHP_SDS_CG_SPLL_CTRL BCHP_SDS_CG_0_SPLL_CTRL
#define BCHP_SDS_CG_SPLL_SSC_CTRL1 BCHP_SDS_CG_0_SPLL_SSC_CTRL1
#define BCHP_SDS_CG_SPLL_SSC_CTRL0 BCHP_SDS_CG_0_SPLL_SSC_CTRL0
#define BCHP_SDS_CG_SPLL_STATUS BCHP_SDS_CG_0_SPLL_STATUS
#define BCHP_SDS_CG_SPLL_PWRDN_RST BCHP_SDS_CG_0_SPLL_PWRDN_RST
#define BCHP_SDS_CL_CLCTL1 BCHP_SDS_CL_0_CLCTL1
#define BCHP_SDS_CL_CLCTL2 BCHP_SDS_CL_0_CLCTL2
#define BCHP_SDS_CL_FLLC BCHP_SDS_CL_0_FLLC
#define BCHP_SDS_CL_FLLC1 BCHP_SDS_CL_0_FLLC1
#define BCHP_SDS_CL_FLIC BCHP_SDS_CL_0_FLIC
#define BCHP_SDS_CL_FLIC1 BCHP_SDS_CL_0_FLIC1
#define BCHP_SDS_CL_FLSW BCHP_SDS_CL_0_FLSW
#define BCHP_SDS_CL_FLI BCHP_SDS_CL_0_FLI
#define BCHP_SDS_CL_FLIF BCHP_SDS_CL_0_FLIF
#define BCHP_SDS_CL_FLPA BCHP_SDS_CL_0_FLPA
#define BCHP_SDS_CL_FLTD BCHP_SDS_CL_0_FLTD
#define BCHP_SDS_CL_PEEST BCHP_SDS_CL_0_PEEST
#define BCHP_SDS_CL_PLTD BCHP_SDS_CL_0_PLTD
#define BCHP_SDS_CL_PLC BCHP_SDS_CL_0_PLC
#define BCHP_SDS_CL_PLC1 BCHP_SDS_CL_0_PLC1
#define BCHP_SDS_CL_PLSW BCHP_SDS_CL_0_PLSW
#define BCHP_SDS_CL_PLI BCHP_SDS_CL_0_PLI
#define BCHP_SDS_CL_PLPA BCHP_SDS_CL_0_PLPA
#define BCHP_SDS_CL_CRBFD BCHP_SDS_CL_0_CRBFD
#define BCHP_SDS_CL_CLHT BCHP_SDS_CL_0_CLHT
#define BCHP_SDS_CL_CLLT BCHP_SDS_CL_0_CLLT
#define BCHP_SDS_CL_CLLA BCHP_SDS_CL_0_CLLA
#define BCHP_SDS_CL_CLCT BCHP_SDS_CL_0_CLCT
#define BCHP_SDS_CL_CLFFCTL BCHP_SDS_CL_0_CLFFCTL
#define BCHP_SDS_CL_FFLPA BCHP_SDS_CL_0_FFLPA
#define BCHP_SDS_CL_CLFBCTL BCHP_SDS_CL_0_CLFBCTL
#define BCHP_SDS_CL_FBLC BCHP_SDS_CL_0_FBLC
#define BCHP_SDS_CL_FBLI BCHP_SDS_CL_0_FBLI
#define BCHP_SDS_CL_FBPA BCHP_SDS_CL_0_FBPA
#define BCHP_SDS_CL_CLDAFECTL BCHP_SDS_CL_0_CLDAFECTL
#define BCHP_SDS_CL_DAFELI BCHP_SDS_CL_0_DAFELI
#define BCHP_SDS_CL_DAFEINT BCHP_SDS_CL_0_DAFEINT
#define BCHP_SDS_DFT_CTRL0 BCHP_SDS_DFT_0_CTRL0
#define BCHP_SDS_DFT_CTRL1 BCHP_SDS_DFT_0_CTRL1
#define BCHP_SDS_DFT_STATUS BCHP_SDS_DFT_0_STATUS
#define BCHP_SDS_DFT_RANGE_START BCHP_SDS_DFT_0_RANGE_START
#define BCHP_SDS_DFT_RANGE_END BCHP_SDS_DFT_0_RANGE_END
#define BCHP_SDS_DFT_DDFS_FCW BCHP_SDS_DFT_0_DDFS_FCW
#define BCHP_SDS_DFT_PEAK_POW BCHP_SDS_DFT_0_PEAK_POW
#define BCHP_SDS_DFT_PEAK_BIN BCHP_SDS_DFT_0_PEAK_BIN
#define BCHP_SDS_DFT_TOTAL_POW BCHP_SDS_DFT_0_TOTAL_POW
#define BCHP_SDS_DFT_MEM_RADDR BCHP_SDS_DFT_0_MEM_RADDR
#define BCHP_SDS_DFT_MEM_RDATA BCHP_SDS_DFT_0_MEM_RDATA
#if 0
#define BCHP_SDS_DSEC_DSRST BCHP_SDS_DSEC_DSRST
#define BCHP_SDS_DSEC_DSCGDIV BCHP_SDS_DSEC_DSCGDIV
#define BCHP_SDS_DSEC_DSCTL00 BCHP_SDS_DSEC_DSCTL00
#define BCHP_SDS_DSEC_DSCTL01 BCHP_SDS_DSEC_DSCTL01
#define BCHP_SDS_DSEC_DSCTL02 BCHP_SDS_DSEC_DSCTL02
#define BCHP_SDS_DSEC_DSCTL03 BCHP_SDS_DSEC_DSCTL03
#define BCHP_SDS_DSEC_DSCMD BCHP_SDS_DSEC_DSCMD
#define BCHP_SDS_DSEC_DSRPLY BCHP_SDS_DSEC_DSRPLY
#define BCHP_SDS_DSEC_DSCMEMADR BCHP_SDS_DSEC_DSCMEMADR
#define BCHP_SDS_DSEC_DSCMEMDAT BCHP_SDS_DSEC_DSCMEMDAT
#define BCHP_SDS_DSEC_DSFIRCTL BCHP_SDS_DSEC_DSFIRCTL
#define BCHP_SDS_DSEC_DS_MISC_CONTROL BCHP_SDS_DSEC_DS_MISC_CONTROL
#define BCHP_SDS_DSEC_DS_PARITY BCHP_SDS_DSEC_DS_PARITY
#define BCHP_SDS_DSEC_ADCTL BCHP_SDS_DSEC_ADCTL
#define BCHP_SDS_DSEC_Q15T BCHP_SDS_DSEC_Q15T
#define BCHP_SDS_DSEC_TPWC BCHP_SDS_DSEC_TPWC
#define BCHP_SDS_DSEC_RXBT BCHP_SDS_DSEC_RXBT
#define BCHP_SDS_DSEC_RXRT BCHP_SDS_DSEC_RXRT
#define BCHP_SDS_DSEC_RBDT BCHP_SDS_DSEC_RBDT
#define BCHP_SDS_DSEC_SLEW BCHP_SDS_DSEC_SLEW
#define BCHP_SDS_DSEC_RERT BCHP_SDS_DSEC_RERT
#define BCHP_SDS_DSEC_DSCT BCHP_SDS_DSEC_DSCT
#define BCHP_SDS_DSEC_DTCT BCHP_SDS_DSEC_DTCT
#define BCHP_SDS_DSEC_DDIO BCHP_SDS_DSEC_DDIO
#define BCHP_SDS_DSEC_RTDC1 BCHP_SDS_DSEC_RTDC1
#define BCHP_SDS_DSEC_RTDC2 BCHP_SDS_DSEC_RTDC2
#define BCHP_SDS_DSEC_TCTL BCHP_SDS_DSEC_TCTL
#define BCHP_SDS_DSEC_CICC BCHP_SDS_DSEC_CICC
#define BCHP_SDS_DSEC_FCIC BCHP_SDS_DSEC_FCIC
#define BCHP_SDS_DSEC_SCIC BCHP_SDS_DSEC_SCIC
#define BCHP_SDS_DSEC_TSTM BCHP_SDS_DSEC_TSTM
#define BCHP_SDS_DSEC_DST1 BCHP_SDS_DSEC_DST1
#define BCHP_SDS_DSEC_DST2 BCHP_SDS_DSEC_DST2
#define BCHP_SDS_DSEC_DS_SAR_THRSH BCHP_SDS_DSEC_DS_SAR_THRSH
#define BCHP_SDS_DSEC_DS_SAR_DATA_OUT BCHP_SDS_DSEC_DS_SAR_DATA_OUT
#define BCHP_SDS_DSEC_DS_SAR_DC_OFFSET BCHP_SDS_DSEC_DS_SAR_DC_OFFSET
#define BCHP_SDS_DSEC_DS_SAR_LPF_INT BCHP_SDS_DSEC_DS_SAR_LPF_INT
#define BCHP_SDS_DSEC_DS_SAR_CONTROL BCHP_SDS_DSEC_DS_SAR_CONTROL
#define BCHP_SDS_DSEC_DS_COMMON_CONTROL BCHP_SDS_DSEC_DS_COMMON_CONTROL
#define BCHP_SDS_DSEC_DSTMRCTL BCHP_SDS_DSEC_DSTMRCTL
#define BCHP_SDS_DSEC_DSGENTMR1 BCHP_SDS_DSEC_DSGENTMR1
#define BCHP_SDS_DSEC_DSGENTMR2 BCHP_SDS_DSEC_DSGENTMR2
#endif
#define BCHP_SDS_EQ_EQMISCCTL BCHP_SDS_EQ_0_EQMISCCTL
#define BCHP_SDS_EQ_EQFFECTL BCHP_SDS_EQ_0_EQFFECTL
#define BCHP_SDS_EQ_EQCFAD BCHP_SDS_EQ_0_EQCFAD
#define BCHP_SDS_EQ_EQFRZCTL BCHP_SDS_EQ_0_EQFRZCTL
#define BCHP_SDS_EQ_F0B BCHP_SDS_EQ_0_F0B
#define BCHP_SDS_EQ_HD8PSK1 BCHP_SDS_EQ_0_HD8PSK1
#define BCHP_SDS_EQ_HD8PSK2 BCHP_SDS_EQ_0_HD8PSK2
#define BCHP_SDS_EQ_HDQPSK BCHP_SDS_EQ_0_HDQPSK
#define BCHP_SDS_EQ_HD16QAM BCHP_SDS_EQ_0_HD16QAM
#define BCHP_SDS_EQ_CMA BCHP_SDS_EQ_0_CMA
#define BCHP_SDS_EQ_CMATH BCHP_SDS_EQ_0_CMATH
#define BCHP_SDS_EQ_VLCTL BCHP_SDS_EQ_0_VLCTL
#define BCHP_SDS_EQ_VLCI BCHP_SDS_EQ_0_VLCI
#define BCHP_SDS_EQ_VLCQ BCHP_SDS_EQ_0_VLCQ
#define BCHP_SDS_EQ_VCOS BCHP_SDS_EQ_0_VCOS
#define BCHP_SDS_EQ_TSFT BCHP_SDS_EQ_0_TSFT
#define BCHP_SDS_EQ_EQSFT BCHP_SDS_EQ_0_EQSFT
#define BCHP_SDS_EQ_PILOTCTL BCHP_SDS_EQ_0_PILOTCTL
#define BCHP_SDS_EQ_PLDCTL BCHP_SDS_EQ_0_PLDCTL
#define BCHP_SDS_EQ_HDRD BCHP_SDS_EQ_0_HDRD
#define BCHP_SDS_EQ_HDRA BCHP_SDS_EQ_0_HDRA
#define BCHP_SDS_EQ_XSEED BCHP_SDS_EQ_0_XSEED
#define BCHP_SDS_EQ_XTAP1 BCHP_SDS_EQ_0_XTAP1
#define BCHP_SDS_EQ_XTAP2 BCHP_SDS_EQ_0_XTAP2
#define BCHP_SDS_EQ_LUPD BCHP_SDS_EQ_0_LUPD
#define BCHP_SDS_EQ_LUPA BCHP_SDS_EQ_0_LUPA
#define BCHP_SDS_EQ_SDSLEN BCHP_SDS_EQ_0_SDSLEN
#define BCHP_SDS_EQ_SDSIG BCHP_SDS_EQ_0_SDSIG
#define BCHP_SDS_EQ_MGAIND BCHP_SDS_EQ_0_MGAIND
#define BCHP_SDS_EQ_MGAINA BCHP_SDS_EQ_0_MGAINA
#define BCHP_SDS_FE_ADCPCTL BCHP_SDS_FE_0_ADCPCTL
#define BCHP_SDS_FE_DCOCTL BCHP_SDS_FE_0_DCOCTL
#define BCHP_SDS_FE_DCOI BCHP_SDS_FE_0_DCOI
#define BCHP_SDS_FE_IQCTL BCHP_SDS_FE_0_IQCTL
#define BCHP_SDS_FE_IQAEST BCHP_SDS_FE_0_IQAEST
#define BCHP_SDS_FE_IQPEST BCHP_SDS_FE_0_IQPEST
#define BCHP_SDS_FE_MIXCTL BCHP_SDS_FE_0_MIXCTL
#define BCHP_SDS_FE_DSTGCTL BCHP_SDS_FE_0_DSTGCTL
#define BCHP_SDS_FE_FILTCTL BCHP_SDS_FE_0_FILTCTL
#define BCHP_SDS_FE_DFCTL BCHP_SDS_FE_0_DFCTL
#define BCHP_SDS_FE_AGFCTL BCHP_SDS_FE_0_AGFCTL
#define BCHP_SDS_FE_AGF BCHP_SDS_FE_0_AGF
#define BCHP_SDS_FE_AIF BCHP_SDS_FE_0_AIF
#define BCHP_SDS_FE_NVCTL BCHP_SDS_FE_0_NVCTL
#define BCHP_SDS_FEC_FECTL BCHP_SDS_FEC_0_FECTL
#define BCHP_SDS_FEC_FSYN BCHP_SDS_FEC_0_FSYN
#define BCHP_SDS_FEC_FRS BCHP_SDS_FEC_0_FRS
#define BCHP_SDS_FEC_FMOD BCHP_SDS_FEC_0_FMOD
#define BCHP_SDS_FEC_FERR BCHP_SDS_FEC_0_FERR
#define BCHP_SDS_FEC_FRSV BCHP_SDS_FEC_0_FRSV
#define BCHP_SDS_GR_BRIDGE_REVISION BCHP_SDS_GR_BRIDGE_0_REVISION
#define BCHP_SDS_GR_BRIDGE_CTRL BCHP_SDS_GR_BRIDGE_0_CTRL
#define BCHP_SDS_GR_BRIDGE_SW_INIT_0 BCHP_SDS_GR_BRIDGE_0_SW_INIT_0
#define BCHP_SDS_GR_BRIDGE_SW_INIT_1 BCHP_SDS_GR_BRIDGE_0_SW_INIT_1
#define BCHP_SDS_HP_HPCONTROL BCHP_SDS_HP_0_HPCONTROL
#define BCHP_SDS_HP_HPCONFIG BCHP_SDS_HP_0_HPCONFIG
#define BCHP_SDS_HP_FNORM BCHP_SDS_HP_0_FNORM
#define BCHP_SDS_HP_HPOVERRIDE BCHP_SDS_HP_0_HPOVERRIDE
#define BCHP_SDS_HP_FROF1 BCHP_SDS_HP_0_FROF1
#define BCHP_SDS_HP_FROF2 BCHP_SDS_HP_0_FROF2
#define BCHP_SDS_HP_FROF3 BCHP_SDS_HP_0_FROF3
#define BCHP_SDS_HP_FROF1_SW BCHP_SDS_HP_0_FROF1_SW
#define BCHP_SDS_HP_FROF2_SW BCHP_SDS_HP_0_FROF2_SW
#define BCHP_SDS_HP_FROF3_SW BCHP_SDS_HP_0_FROF3_SW
#define BCHP_SDS_HP_M_N_PEAK_VERIFY BCHP_SDS_HP_0_M_N_PEAK_VERIFY
#define BCHP_SDS_HP_M_N_RECEIVER_VERIFY BCHP_SDS_HP_0_M_N_RECEIVER_VERIFY
#define BCHP_SDS_HP_M_N_RECEIVER_LOCK BCHP_SDS_HP_0_M_N_RECEIVER_LOCK
#define BCHP_SDS_HP_DCORR_THRESHOLD BCHP_SDS_HP_0_DCORR_THRESHOLD
#define BCHP_SDS_HP_PLHDRSCR1 BCHP_SDS_HP_0_PLHDRSCR1
#define BCHP_SDS_HP_PLHDRSCR2 BCHP_SDS_HP_0_PLHDRSCR2
#define BCHP_SDS_HP_PLHDRSCR3 BCHP_SDS_HP_0_PLHDRSCR3
#define BCHP_SDS_HP_ACM_CHECK BCHP_SDS_HP_0_ACM_CHECK
#define BCHP_SDS_HP_FRAME_LENGTH_INITIAL BCHP_SDS_HP_0_FRAME_LENGTH_INITIAL
#define BCHP_SDS_HP_FRAME_LENGTH_DUMMY_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_DUMMY_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_QPSK_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_QPSK_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_8PSK_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_8PSK_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_16APSK_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_16APSK_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_32APSK_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_32APSK_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_RESERVED_29_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_RESERVED_29_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_RESERVED_30_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_RESERVED_30_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_RESERVED_31_NORMAL BCHP_SDS_HP_0_FRAME_LENGTH_RESERVED_31_NORMAL
#define BCHP_SDS_HP_FRAME_LENGTH_DUMMY_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_DUMMY_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_QPSK_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_QPSK_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_8PSK_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_8PSK_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_16APSK_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_16APSK_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_32APSK_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_32APSK_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_RESERVED_29_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_RESERVED_29_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_RESERVED_30_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_RESERVED_30_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_RESERVED_31_SHORT BCHP_SDS_HP_0_FRAME_LENGTH_RESERVED_31_SHORT
#define BCHP_SDS_HP_FRAME_LENGTH_SAMPLE BCHP_SDS_HP_0_FRAME_LENGTH_SAMPLE
#define BCHP_SDS_HP_PEAK_SAMPLE_1_0 BCHP_SDS_HP_0_PEAK_SAMPLE_1_0
#define BCHP_SDS_HP_PEAK_SAMPLE_3_2 BCHP_SDS_HP_0_PEAK_SAMPLE_3_2
#define BCHP_SDS_INTR2_0_CPU_STATUS BCHP_SDS_INTR2_0_0_CPU_STATUS
#define BCHP_SDS_INTR2_0_CPU_SET BCHP_SDS_INTR2_0_0_CPU_SET
#define BCHP_SDS_INTR2_0_CPU_CLEAR BCHP_SDS_INTR2_0_0_CPU_CLEAR
#define BCHP_SDS_INTR2_0_CPU_MASK_STATUS BCHP_SDS_INTR2_0_0_CPU_MASK_STATUS
#define BCHP_SDS_INTR2_0_CPU_MASK_SET BCHP_SDS_INTR2_0_0_CPU_MASK_SET
#define BCHP_SDS_INTR2_0_CPU_MASK_CLEAR BCHP_SDS_INTR2_0_0_CPU_MASK_CLEAR
#define BCHP_SDS_INTR2_0_PCI_STATUS BCHP_SDS_INTR2_0_0_PCI_STATUS
#define BCHP_SDS_INTR2_0_PCI_SET BCHP_SDS_INTR2_0_0_PCI_SET
#define BCHP_SDS_INTR2_0_PCI_CLEAR BCHP_SDS_INTR2_0_0_PCI_CLEAR
#define BCHP_SDS_INTR2_0_PCI_MASK_STATUS BCHP_SDS_INTR2_0_0_PCI_MASK_STATUS
#define BCHP_SDS_INTR2_0_PCI_MASK_SET BCHP_SDS_INTR2_0_0_PCI_MASK_SET
#define BCHP_SDS_INTR2_0_PCI_MASK_CLEAR BCHP_SDS_INTR2_0_0_PCI_MASK_CLEAR
#if (BCHP_VER >= BCHP_VER_B0)
#define BCHP_SDS_HP_PEAK_SAMPLE_2 BCHP_SDS_HP_0_PEAK_SAMPLE_2
#define BCHP_SDS_HP_PEAK_SAMPLE_3 BCHP_SDS_HP_0_PEAK_SAMPLE_3
#define BCHP_SDS_HP_HP_DAFE BCHP_SDS_HP_0_HP_DAFE
#define BCHP_SDS_HP_NEW_STATE BCHP_SDS_HP_0_NEW_STATE
#define BCHP_SDS_HP_IGNORE_PHI_FROM_DAFE BCHP_SDS_HP_0_IGNORE_PHI_FROM_DAFE
#endif
#define BCHP_SDS_INTR2_1_CPU_STATUS BCHP_SDS_INTR2_1_0_CPU_STATUS
#define BCHP_SDS_INTR2_1_CPU_SET BCHP_SDS_INTR2_1_0_CPU_SET
#define BCHP_SDS_INTR2_1_CPU_CLEAR BCHP_SDS_INTR2_1_0_CPU_CLEAR
#define BCHP_SDS_INTR2_1_CPU_MASK_STATUS BCHP_SDS_INTR2_1_0_CPU_MASK_STATUS
#define BCHP_SDS_INTR2_1_CPU_MASK_SET BCHP_SDS_INTR2_1_0_CPU_MASK_SET
#define BCHP_SDS_INTR2_1_CPU_MASK_CLEAR BCHP_SDS_INTR2_1_0_CPU_MASK_CLEAR
#define BCHP_SDS_INTR2_1_PCI_STATUS BCHP_SDS_INTR2_1_0_PCI_STATUS
#define BCHP_SDS_INTR2_1_PCI_SET BCHP_SDS_INTR2_1_0_PCI_SET
#define BCHP_SDS_INTR2_1_PCI_CLEAR BCHP_SDS_INTR2_1_0_PCI_CLEAR
#define BCHP_SDS_INTR2_1_PCI_MASK_STATUS BCHP_SDS_INTR2_1_0_PCI_MASK_STATUS
#define BCHP_SDS_INTR2_1_PCI_MASK_SET BCHP_SDS_INTR2_1_0_PCI_MASK_SET
#define BCHP_SDS_INTR2_1_PCI_MASK_CLEAR BCHP_SDS_INTR2_1_0_PCI_MASK_CLEAR
#define BCHP_SDS_MISC_REVID BCHP_SDS_MISC_0_REVID
#define BCHP_SDS_MISC_IICTL1 BCHP_SDS_MISC_0_IICTL1
#define BCHP_SDS_MISC_IICTL2 BCHP_SDS_MISC_0_IICTL2
#define BCHP_SDS_MISC_IICCNT BCHP_SDS_MISC_0_IICCNT
#define BCHP_SDS_MISC_IICHPA BCHP_SDS_MISC_0_IICHPA
#define BCHP_SDS_MISC_MIICTX1 BCHP_SDS_MISC_0_MIICTX1
#define BCHP_SDS_MISC_MIICTX2 BCHP_SDS_MISC_0_MIICTX2
#define BCHP_SDS_MISC_MIICRX1 BCHP_SDS_MISC_0_MIICRX1
#define BCHP_SDS_MISC_MIICRX2 BCHP_SDS_MISC_0_MIICRX2
#define BCHP_SDS_MISC_MI2CSA BCHP_SDS_MISC_0_MI2CSA
#define BCHP_SDS_MISC_TMRCTL BCHP_SDS_MISC_0_TMRCTL
#define BCHP_SDS_MISC_GENTMR3 BCHP_SDS_MISC_0_GENTMR3
#define BCHP_SDS_MISC_GENTMR2 BCHP_SDS_MISC_0_GENTMR2
#define BCHP_SDS_MISC_GENTMR1 BCHP_SDS_MISC_0_GENTMR1
#define BCHP_SDS_MISC_BERTMR BCHP_SDS_MISC_0_BERTMR
#define BCHP_SDS_MISC_BTMR BCHP_SDS_MISC_0_BTMR
#define BCHP_SDS_MISC_TPDIR BCHP_SDS_MISC_0_TPDIR
#define BCHP_SDS_MISC_TPODS BCHP_SDS_MISC_0_TPODS
#define BCHP_SDS_MISC_TPDS BCHP_SDS_MISC_0_TPDS
#define BCHP_SDS_MISC_TPCTL1 BCHP_SDS_MISC_0_TPCTL1
#define BCHP_SDS_MISC_TPCTL2 BCHP_SDS_MISC_0_TPCTL2
#define BCHP_SDS_MISC_TPCTL3 BCHP_SDS_MISC_0_TPCTL3
#define BCHP_SDS_MISC_TPOUT BCHP_SDS_MISC_0_TPOUT
#define BCHP_SDS_MISC_MISCTL BCHP_SDS_MISC_0_MISCTL
#define BCHP_SDS_MISC_INTR_RAW_STS0 BCHP_SDS_MISC_0_INTR_RAW_STS0
#define BCHP_SDS_MISC_INTR_RAW_STS1 BCHP_SDS_MISC_0_INTR_RAW_STS1
#define BCHP_SDS_OI_OIFCTL00 BCHP_SDS_OI_0_OIFCTL00
#define BCHP_SDS_OI_OIFCTL01 BCHP_SDS_OI_0_OIFCTL01
#define BCHP_SDS_OI_OIFCTL02 BCHP_SDS_OI_0_OIFCTL02
#define BCHP_SDS_OI_OPLL BCHP_SDS_OI_0_OPLL
#define BCHP_SDS_OI_OPLL2 BCHP_SDS_OI_0_OPLL2
#define BCHP_SDS_OI_FERC BCHP_SDS_OI_0_FERC
#define BCHP_SDS_OI_FRC BCHP_SDS_OI_0_FRC
#define BCHP_SDS_OI_OSIGPN BCHP_SDS_OI_0_OSIGPN
#define BCHP_SDS_OI_OSUBD BCHP_SDS_OI_0_OSUBD
#define BCHP_SDS_OI_OCOEF BCHP_SDS_OI_0_OCOEF
#define BCHP_SDS_OI_OFI BCHP_SDS_OI_0_OFI
#define BCHP_SDS_OI_OPLL_NPDIV BCHP_SDS_OI_0_OPLL_NPDIV
#define BCHP_SDS_OI_OPLL_MDIV_CTRL BCHP_SDS_OI_0_OPLL_MDIV_CTRL
#define BCHP_SDS_OI_OPLL_CTRL BCHP_SDS_OI_0_OPLL_CTRL
#define BCHP_SDS_OI_OPLL_SSC_CTRL1 BCHP_SDS_OI_0_OPLL_SSC_CTRL1
#define BCHP_SDS_OI_OPLL_SSC_CTRL0 BCHP_SDS_OI_0_OPLL_SSC_CTRL0
#define BCHP_SDS_OI_OPLL_STATUS BCHP_SDS_OI_0_OPLL_STATUS
#define BCHP_SDS_OI_OPLL_PWRDN_RST BCHP_SDS_OI_0_OPLL_PWRDN_RST
#define BCHP_SDS_SNR_SNRCTL BCHP_SDS_SNR_0_SNRCTL
#define BCHP_SDS_SNR_SNRHT BCHP_SDS_SNR_0_SNRHT
#define BCHP_SDS_SNR_SNRLT BCHP_SDS_SNR_0_SNRLT
#define BCHP_SDS_SNR_SNRE BCHP_SDS_SNR_0_SNRE
#define BCHP_SDS_SNR_SNORETP BCHP_SDS_SNR_0_SNORETP
#define BCHP_SDS_SNR_SNORESP BCHP_SDS_SNR_0_SNORESP
#define BCHP_SDS_SNR_SNORECTL BCHP_SDS_SNR_0_SNORECTL
#define BCHP_SDS_TUNER_DAISY_R01 BCHP_SDS_TUNER_0_DAISY_R01
#define BCHP_SDS_TUNER_BGBIAS_R01 BCHP_SDS_TUNER_0_BGBIAS_R01
#define BCHP_SDS_TUNER_REFPLL_R01 BCHP_SDS_TUNER_0_REFPLL_R01
#define BCHP_SDS_TUNER_REFPLL_R02 BCHP_SDS_TUNER_0_REFPLL_R02
#define BCHP_SDS_TUNER_REFPLL_R03 BCHP_SDS_TUNER_0_REFPLL_R03
#define BCHP_SDS_TUNER_REFPLL_R04 BCHP_SDS_TUNER_0_REFPLL_R04
#define BCHP_SDS_TUNER_ADC_R01 BCHP_SDS_TUNER_0_ADC_R01
#define BCHP_SDS_TUNER_ADC_R02 BCHP_SDS_TUNER_0_ADC_R02
#define BCHP_SDS_TUNER_ADC_R03 BCHP_SDS_TUNER_0_ADC_R03
#define BCHP_SDS_TUNER_ADC_R04 BCHP_SDS_TUNER_0_ADC_R04
#define BCHP_SDS_TUNER_IFPGA_R01 BCHP_SDS_TUNER_0_IFPGA_R01
#define BCHP_SDS_TUNER_LNA_R01 BCHP_SDS_TUNER_0_LNA_R01
#define BCHP_SDS_TUNER_LNA_R02 BCHP_SDS_TUNER_0_LNA_R02
#define BCHP_SDS_TUNER_LO_R01 BCHP_SDS_TUNER_0_LO_R01
#define BCHP_SDS_TUNER_LO_R02 BCHP_SDS_TUNER_0_LO_R02
#define BCHP_SDS_TUNER_LO_R03 BCHP_SDS_TUNER_0_LO_R03
#define BCHP_SDS_TUNER_LO_R04 BCHP_SDS_TUNER_0_LO_R04
#define BCHP_SDS_TUNER_LO_R05 BCHP_SDS_TUNER_0_LO_R05
#define BCHP_SDS_TUNER_LO_R06 BCHP_SDS_TUNER_0_LO_R06
#define BCHP_SDS_TUNER_LO_R07 BCHP_SDS_TUNER_0_LO_R07
#define BCHP_SDS_TUNER_LPF_R01 BCHP_SDS_TUNER_0_LPF_R01
#define BCHP_SDS_TUNER_RFPGA_R01 BCHP_SDS_TUNER_0_RFPGA_R01
#define BCHP_SDS_TUNER_BBAGC_R01 BCHP_SDS_TUNER_0_BBAGC_R01
#define BCHP_SDS_TUNER_BBAGC_R02 BCHP_SDS_TUNER_0_BBAGC_R02
#define BCHP_SDS_TUNER_BBAGC_R03 BCHP_SDS_TUNER_0_BBAGC_R03
#define BCHP_SDS_TUNER_BBAGC_R04 BCHP_SDS_TUNER_0_BBAGC_R04
#define BCHP_SDS_TUNER_BBAGC_R05 BCHP_SDS_TUNER_0_BBAGC_R05
#define BCHP_SDS_TUNER_CALDAC_R01 BCHP_SDS_TUNER_0_CALDAC_R01
#define BCHP_SDS_TUNER_LODDFS_R01 BCHP_SDS_TUNER_0_LODDFS_R01
#define BCHP_SDS_TUNER_LODDFS_R02 BCHP_SDS_TUNER_0_LODDFS_R02
#define BCHP_SDS_TUNER_MXFGA_R01 BCHP_SDS_TUNER_0_MXFGA_R01
#define BCHP_SDS_TUNER_MXFGA_R02 BCHP_SDS_TUNER_0_MXFGA_R02
#define BCHP_SDS_TUNER_POSTDCO_R01 BCHP_SDS_TUNER_0_POSTDCO_R01
#define BCHP_SDS_TUNER_PREDCO_R01 BCHP_SDS_TUNER_0_PREDCO_R01
#define BCHP_SDS_TUNER_RFAGC_R01 BCHP_SDS_TUNER_0_RFAGC_R01
#define BCHP_SDS_TUNER_RFAGC_R02 BCHP_SDS_TUNER_0_RFAGC_R02
#define BCHP_SDS_TUNER_RFAGC_R03 BCHP_SDS_TUNER_0_RFAGC_R03
#define BCHP_SDS_TUNER_RFAGC_R04 BCHP_SDS_TUNER_0_RFAGC_R04
#define BCHP_SDS_TUNER_RFAGC_R05 BCHP_SDS_TUNER_0_RFAGC_R05
#define BCHP_SDS_TUNER_DDFS_R01 BCHP_SDS_TUNER_0_DDFS_R01
#define BCHP_SDS_TUNER_PLLSTAT_R01 BCHP_SDS_TUNER_0_PLLSTAT_R01
#define BCHP_SDS_TUNER_PREDCOI_R01 BCHP_SDS_TUNER_0_PREDCOI_R01
#define BCHP_SDS_TUNER_PREDCOI_R02 BCHP_SDS_TUNER_0_PREDCOI_R02
#define BCHP_SDS_TUNER_PREDCOI_R03 BCHP_SDS_TUNER_0_PREDCOI_R03
#define BCHP_SDS_TUNER_PREDCOI_R04 BCHP_SDS_TUNER_0_PREDCOI_R04
#define BCHP_SDS_TUNER_PREDCOQ_R01 BCHP_SDS_TUNER_0_PREDCOQ_R01
#define BCHP_SDS_TUNER_PREDCOQ_R02 BCHP_SDS_TUNER_0_PREDCOQ_R02
#define BCHP_SDS_TUNER_PREDCOQ_R03 BCHP_SDS_TUNER_0_PREDCOQ_R03
#define BCHP_SDS_TUNER_PREDCOQ_R04 BCHP_SDS_TUNER_0_PREDCOQ_R04
#define BCHP_SDS_TUNER_POSTDCOI_R01 BCHP_SDS_TUNER_0_POSTDCOI_R01
#define BCHP_SDS_TUNER_POSTDCOI_R02 BCHP_SDS_TUNER_0_POSTDCOI_R02
#define BCHP_SDS_TUNER_POSTDCOI_R03 BCHP_SDS_TUNER_0_POSTDCOI_R03
#define BCHP_SDS_TUNER_POSTDCOI_R04 BCHP_SDS_TUNER_0_POSTDCOI_R04
#define BCHP_SDS_TUNER_POSTDCOQ_R01 BCHP_SDS_TUNER_0_POSTDCOQ_R01
#define BCHP_SDS_TUNER_POSTDCOQ_R02 BCHP_SDS_TUNER_0_POSTDCOQ_R02
#define BCHP_SDS_TUNER_POSTDCOQ_R03 BCHP_SDS_TUNER_0_POSTDCOQ_R03
#define BCHP_SDS_TUNER_POSTDCOQ_R04 BCHP_SDS_TUNER_0_POSTDCOQ_R04
#define BCHP_SDS_TUNER_RESET_R01 BCHP_SDS_TUNER_0_RESET_R01
#define BCHP_SDS_TUNER_AGC_IF_CTL BCHP_SDS_TUNER_0_AGC_IF_CTL
#define BCHP_SDS_TUNER_FILCAL_DDFS_CTL BCHP_SDS_TUNER_0_FILCAL_DDFS_CTL
#define BCHP_SDS_TUNER_TPCTL BCHP_SDS_TUNER_0_TPCTL
#define BCHP_SDS_TUNER_PWRUP_COMMON_R01 BCHP_SDS_TUNER_0_PWRUP_COMMON_R01
#define BCHP_SDS_TUNER_PWRUP_R01 BCHP_SDS_TUNER_0_PWRUP_R01
#define BCHP_SDS_VIT_VTCTL BCHP_SDS_VIT_0_VTCTL
#define BCHP_SDS_VIT_V10 BCHP_SDS_VIT_0_V10
#define BCHP_SDS_VIT_V32 BCHP_SDS_VIT_0_V32
#define BCHP_SDS_VIT_V54 BCHP_SDS_VIT_0_V54
#define BCHP_SDS_VIT_V76 BCHP_SDS_VIT_0_V76
#define BCHP_SDS_VIT_VINT BCHP_SDS_VIT_0_VINT
#define BCHP_SDS_VIT_VCNT BCHP_SDS_VIT_0_VCNT
#define BCHP_SDS_VIT_VSTC BCHP_SDS_VIT_0_VSTC
#define BCHP_SDS_VIT_VST BCHP_SDS_VIT_0_VST
#define BCHP_SDS_VIT_VREC BCHP_SDS_VIT_0_VREC
#define BCHP_SDS_VIT_VRCV BCHP_SDS_VIT_0_VRCV

#define BCHP_TFEC_TFECTL BCHP_TFEC_0_TFECTL
#define BCHP_TFEC_TNBLK BCHP_TFEC_0_TNBLK
#define BCHP_TFEC_TCBLK BCHP_TFEC_0_TCBLK
#define BCHP_TFEC_TBBLK BCHP_TFEC_0_TBBLK
#define BCHP_TFEC_TCSYM BCHP_TFEC_0_TCSYM
#define BCHP_TFEC_TFMT BCHP_TFEC_0_TFMT
#define BCHP_TFEC_TPAK BCHP_TFEC_0_TPAK
#define BCHP_TFEC_TSSQ BCHP_TFEC_0_TSSQ
#define BCHP_TFEC_TSYN BCHP_TFEC_0_TSYN
#define BCHP_TFEC_TTUR BCHP_TFEC_0_TTUR
#define BCHP_TFEC_TZPK BCHP_TFEC_0_TZPK
#define BCHP_TFEC_TZSY BCHP_TFEC_0_TZSY
#define BCHP_TFEC_TITR BCHP_TFEC_0_TITR
#define BCHP_TFEC_TCIL BCHP_TFEC_0_TCIL
#define BCHP_TFEC_TRSD BCHP_TFEC_0_TRSD
#define BCHP_TFEC_TPN BCHP_TFEC_0_TPN
#define BCHP_TFEC_TSIGCNT BCHP_TFEC_0_TSIGCNT
#define BCHP_TFEC_TSIGITD BCHP_TFEC_0_TSIGITD
#define BCHP_TFEC_TSIGXPT BCHP_TFEC_0_TSIGXPT
#define BCHP_TFEC_TTPCTL BCHP_TFEC_0_TTPCTL
#define BCHP_TFEC_GR_BRIDGE_REVISION BCHP_TFEC_GR_BRIDGE_0_REVISION
#define BCHP_TFEC_GR_BRIDGE_CTRL BCHP_TFEC_GR_BRIDGE_0_CTRL
#define BCHP_TFEC_GR_BRIDGE_SW_INIT_0 BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0
#define BCHP_TFEC_GR_BRIDGE_SW_INIT_1 BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1
#define BCHP_TFEC_INTR2_CPU_STATUS BCHP_TFEC_INTR2_0_CPU_STATUS
#define BCHP_TFEC_INTR2_CPU_SET BCHP_TFEC_INTR2_0_CPU_SET
#define BCHP_TFEC_INTR2_CPU_CLEAR BCHP_TFEC_INTR2_0_CPU_CLEAR
#define BCHP_TFEC_INTR2_CPU_MASK_STATUS BCHP_TFEC_INTR2_0_CPU_MASK_STATUS
#define BCHP_TFEC_INTR2_CPU_MASK_SET BCHP_TFEC_INTR2_0_CPU_MASK_SET
#define BCHP_TFEC_INTR2_CPU_MASK_CLEAR BCHP_TFEC_INTR2_0_CPU_MASK_CLEAR
#define BCHP_TFEC_INTR2_PCI_STATUS BCHP_TFEC_INTR2_0_PCI_STATUS
#define BCHP_TFEC_INTR2_PCI_SET BCHP_TFEC_INTR2_0_PCI_SET
#define BCHP_TFEC_INTR2_PCI_CLEAR BCHP_TFEC_INTR2_0_PCI_CLEAR
#define BCHP_TFEC_INTR2_PCI_MASK_STATUS BCHP_TFEC_INTR2_0_PCI_MASK_STATUS
#define BCHP_TFEC_INTR2_PCI_MASK_SET BCHP_TFEC_INTR2_0_PCI_MASK_SET
#define BCHP_TFEC_INTR2_PCI_MASK_CLEAR BCHP_TFEC_INTR2_0_PCI_MASK_CLEAR
#define BCHP_TFEC_MISC_POST_DIV_CTL BCHP_TFEC_MISC_0_POST_DIV_CTL
#define BCHP_TFEC_MISC_REGF_STBY BCHP_TFEC_MISC_0_REGF_STBY
#define BCHP_TFEC_MISC_MISCCTL BCHP_TFEC_MISC_0_MISCCTL

/* interrupt bit definitions for multiple channels */
#define BCHP_SDS_INTR2_0_CPU_STATUS_fecii_in_lock_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_fecii_in_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_fecii_out_lock_MASK        BCHP_SDS_INTR2_0_0_CPU_STATUS_fecii_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_fecii_ov_MASK              BCHP_SDS_INTR2_0_0_CPU_STATUS_fecii_ov_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_vit_in_sync_MASK           BCHP_SDS_INTR2_0_0_CPU_STATUS_vit_in_sync_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_vit_out_sync_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_vit_out_sync_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_rvb_in_lock_MASK           BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_in_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_rvb_out_lock_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_rvb_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_ahb_in_lock_MASK           BCHP_SDS_INTR2_0_0_CPU_STATUS_ahb_in_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_ahb_out_lock_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_ahb_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_mi2c_int_MASK              BCHP_SDS_INTR2_0_0_CPU_STATUS_mi2c_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_dft_done_int_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_dft_done_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_spll_in_lock_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_spll_in_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_spll_out_lock_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_spll_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_opll_in_lock_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_opll_in_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_opll_out_lock_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_opll_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_refpll_out_lock_MASK       BCHP_SDS_INTR2_0_0_CPU_STATUS_refpll_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_mixpll_out_lock_MASK       BCHP_SDS_INTR2_0_0_CPU_STATUS_mixpll_out_lock_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_agc_3stage_MASK            BCHP_SDS_INTR2_0_0_CPU_STATUS_agc_3stage_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_gentimer1_int_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer1_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_gentimer2_int_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer2_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_gentimer3_int_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_gentimer3_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_bertimer_int_MASK          BCHP_SDS_INTR2_0_0_CPU_STATUS_bertimer_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_bclktimer_int_MASK         BCHP_SDS_INTR2_0_0_CPU_STATUS_bclktimer_int_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_hp_state_match_MASK        BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_state_match_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_hp_state_change_MASK       BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_state_change_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_hp_pk_smpl_cmlt_MASK       BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_pk_smpl_cmlt_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_hp_reacq_MASK              BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_reacq_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_hp_frof_change_MASK        BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_frof_change_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_hp_frame_boundary_MASK     BCHP_SDS_INTR2_0_0_CPU_STATUS_hp_frame_boundary_MASK
#define BCHP_SDS_INTR2_0_CPU_STATUS_dafe_loop_ramp_err_MASK    BCHP_SDS_INTR2_0_0_CPU_STATUS_dafe_loop_ramp_err_MASK
#define BCHP_SDS_OI_OPLL_MDIV_CTRL_channel_load_en_MASK        BCHP_SDS_OI_0_OPLL_MDIV_CTRL_channel_load_en_MASK
#define BCHP_SDS_OI_OIFCTL01_loop_en_MASK                      BCHP_SDS_OI_0_OIFCTL01_loop_en_MASK

#define BCHP_SDS_CWC_CTRL1 BCHP_SDS_CWC_0_CTRL1
#define BCHP_SDS_CWC_CTRL2 BCHP_SDS_CWC_0_CTRL2
#define BCHP_SDS_CWC_LEAK BCHP_SDS_CWC_0_LEAK
#define BCHP_SDS_CWC_SPUR_FCW1 BCHP_SDS_CWC_0_SPUR_FCW1
#define BCHP_SDS_CWC_SPUR_FCW2 BCHP_SDS_CWC_0_SPUR_FCW2
#define BCHP_SDS_CWC_SPUR_FCW3 BCHP_SDS_CWC_0_SPUR_FCW3
#define BCHP_SDS_CWC_SPUR_FCW4 BCHP_SDS_CWC_0_SPUR_FCW4
#define BCHP_SDS_CWC_SPUR_FCW5 BCHP_SDS_CWC_0_SPUR_FCW5
#define BCHP_SDS_CWC_SPUR_FCW6 BCHP_SDS_CWC_0_SPUR_FCW6
#define BCHP_SDS_CWC_LFC1 BCHP_SDS_CWC_0_LFC1
#define BCHP_SDS_CWC_LFC2 BCHP_SDS_CWC_0_LFC2
#define BCHP_SDS_CWC_LFC3 BCHP_SDS_CWC_0_LFC3
#define BCHP_SDS_CWC_LFC4 BCHP_SDS_CWC_0_LFC4
#define BCHP_SDS_CWC_LFC5 BCHP_SDS_CWC_0_LFC5
#define BCHP_SDS_CWC_LFC6 BCHP_SDS_CWC_0_LFC6
#define BCHP_SDS_CWC_INT1 BCHP_SDS_CWC_0_INT1
#define BCHP_SDS_CWC_INT2 BCHP_SDS_CWC_0_INT2
#define BCHP_SDS_CWC_INT3 BCHP_SDS_CWC_0_INT3
#define BCHP_SDS_CWC_INT4 BCHP_SDS_CWC_0_INT4
#define BCHP_SDS_CWC_INT5 BCHP_SDS_CWC_0_INT5
#define BCHP_SDS_CWC_INT6 BCHP_SDS_CWC_0_INT6
#define BCHP_SDS_CWC_COEFF_RWCTRL BCHP_SDS_CWC_0_COEFF_RWCTRL
#define BCHP_SDS_CWC_COEFF BCHP_SDS_CWC_0_COEFF
#define BCHP_SDS_CWC_FOFS1 BCHP_SDS_CWC_0_FOFS1
#define BCHP_SDS_CWC_FOFS2 BCHP_SDS_CWC_0_FOFS2
#define BCHP_SDS_CWC_FOFS3 BCHP_SDS_CWC_0_FOFS3
#define BCHP_SDS_CWC_FOFS4 BCHP_SDS_CWC_0_FOFS4
#define BCHP_SDS_CWC_FOFS5 BCHP_SDS_CWC_0_FOFS5
#define BCHP_SDS_CWC_FOFS6 BCHP_SDS_CWC_0_FOFS6

#define BCHP_FTM_PHY_ANA0_0 BCHP_FTM_PHY_ANA_CTL0_0
#define BCHP_FTM_PHY_ANA0_1 BCHP_FTM_PHY_ANA_CTL0_1
#define BCHP_FTM_PHY_ANA1_0 BCHP_FTM_PHY_ANA_CTL1_0
#define BCHP_FTM_PHY_ANA1_1 BCHP_FTM_PHY_ANA_CTL1_1

#define BCHP_SDS_CWC_LFC1_LF_CINT_SHIFT BCHP_SDS_CWC_0_LFC1_LF_CINT_SHIFT
#define BCHP_SDS_CWC_LFC1_LF_CLIN_SHIFT BCHP_SDS_CWC_0_LFC1_LF_CLIN_SHIFT

#endif /* _BAST_4517_H__ */
