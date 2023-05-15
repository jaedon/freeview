/***************************************************************************
 *     Copyright (c) 2005-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef _BWFE_7364_PRIV_H__
#define _BWFE_7364_PRIV_H__

#include "bchp_leap_l1.h"
#include "bchp_sds_dsec_0.h"
#include "bchp_sds_dsec_ap_0.h"
#include "bchp_sds_dsec_gr_bridge_0.h"
#include "bchp_sds_dsec_intr2_0.h"

#define BDSQ_G1_BUILD_VERSION 0x01

#define BDSQ_NUM_CHANNELS 1

/* DSEC channel 0 interrupts */
#define BCHP_INT_ID_DSEC_TIMER1_0         BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_timer2_SHIFT)
#define BCHP_INT_ID_DSEC_TIMER2_0         BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_timer1_SHIFT)
#define BCHP_INT_ID_DSEC_DONE_0           BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_done_SHIFT)
#define BCHP_INT_ID_DSEC_TX_A_EMPTY_0     BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_tx_fifo_a_empty_SHIFT)
#define BCHP_INT_ID_DSEC_RX_A_EMPTY_0     BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_rx_fifo_a_empty_SHIFT)
#define BCHP_INT_ID_DSEC_ACW_DONE_0       BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_acw_done_SHIFT)
#define BCHP_INT_ID_DSEC_VOL_LT_THRESH_0  BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_sar_vol_lt_thresh_SHIFT)
#define BCHP_INT_ID_DSEC_VOL_GT_THRESH_0  BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_dsec_sar_vol_gt_thresh_SHIFT)
#define BCHP_INT_ID_DSEC_TONE_RISE_0      BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_tone_rise_irq_SHIFT)
#define BCHP_INT_ID_DSEC_IDLE_TIMEOUT_0   BCHP_INT_ID_CREATE(BCHP_SDS_DSEC_INTR2_0_CPU_STATUS, BCHP_SDS_DSEC_INTR2_0_CPU_STATUS_idle_timeout_irq_SHIFT)

/* SDS_DSEC base registers */
#define BCHP_SDS_DSEC_DSRST               BCHP_SDS_DSEC_0_DSRST
#define BCHP_SDS_DSEC_DSCGDIV             BCHP_SDS_DSEC_0_DSCGDIV
#define BCHP_SDS_DSEC_DSCTL00             BCHP_SDS_DSEC_0_DSCTL00
#define BCHP_SDS_DSEC_DSCTL01             BCHP_SDS_DSEC_0_DSCTL01
#define BCHP_SDS_DSEC_DSCTL02             BCHP_SDS_DSEC_0_DSCTL02
#define BCHP_SDS_DSEC_DSCTL03             BCHP_SDS_DSEC_0_DSCTL03
#define BCHP_SDS_DSEC_DSCMD               BCHP_SDS_DSEC_0_DSCMD
#define BCHP_SDS_DSEC_DSRPLY              BCHP_SDS_DSEC_0_DSRPLY
#define BCHP_SDS_DSEC_DSCMEMADR           BCHP_SDS_DSEC_0_DSCMEMADR
#define BCHP_SDS_DSEC_DSCMEMDAT           BCHP_SDS_DSEC_0_DSCMEMDAT
#define BCHP_SDS_DSEC_DSFIRCTL            BCHP_SDS_DSEC_0_DSFIRCTL
#define BCHP_SDS_DSEC_DS_MISC_CONTROL     BCHP_SDS_DSEC_0_DS_MISC_CONTROL
#define BCHP_SDS_DSEC_DS_PARITY           BCHP_SDS_DSEC_0_DS_PARITY
#define BCHP_SDS_DSEC_ADCTL               BCHP_SDS_DSEC_0_ADCTL
#define BCHP_SDS_DSEC_Q15T                BCHP_SDS_DSEC_0_Q15T
#define BCHP_SDS_DSEC_Q15T_TB             BCHP_SDS_DSEC_0_Q15T_TB
#define BCHP_SDS_DSEC_TB_LENGTH           BCHP_SDS_DSEC_0_TB_LENGTH
#define BCHP_SDS_DSEC_TPWC                BCHP_SDS_DSEC_0_TPWC
#define BCHP_SDS_DSEC_RXBT                BCHP_SDS_DSEC_0_RXBT
#define BCHP_SDS_DSEC_RXRT                BCHP_SDS_DSEC_0_RXRT
#define BCHP_SDS_DSEC_RBDT                BCHP_SDS_DSEC_0_RBDT
#define BCHP_SDS_DSEC_SLEW                BCHP_SDS_DSEC_0_SLEW
#define BCHP_SDS_DSEC_RERT                BCHP_SDS_DSEC_0_RERT
#define BCHP_SDS_DSEC_DSCT                BCHP_SDS_DSEC_0_DSCT
#define BCHP_SDS_DSEC_DTCT                BCHP_SDS_DSEC_0_DTCT
#define BCHP_SDS_DSEC_DDIO                BCHP_SDS_DSEC_0_DDIO
#define BCHP_SDS_DSEC_RTDC1               BCHP_SDS_DSEC_0_RTDC1
#define BCHP_SDS_DSEC_RTDC2               BCHP_SDS_DSEC_0_RTDC2
#define BCHP_SDS_DSEC_TCTL                BCHP_SDS_DSEC_0_TCTL
#define BCHP_SDS_DSEC_CICC                BCHP_SDS_DSEC_0_CICC
#define BCHP_SDS_DSEC_FCIC                BCHP_SDS_DSEC_0_FCIC
#define BCHP_SDS_DSEC_SCIC                BCHP_SDS_DSEC_0_SCIC
#define BCHP_SDS_DSEC_TSTM                BCHP_SDS_DSEC_0_TSTM
#define BCHP_SDS_DSEC_DST1                BCHP_SDS_DSEC_0_DST1
#define BCHP_SDS_DSEC_DST2                BCHP_SDS_DSEC_0_DST2
#define BCHP_SDS_DSEC_DS_SAR_THRSH        BCHP_SDS_DSEC_0_DS_SAR_THRSH
#define BCHP_SDS_DSEC_DS_SAR_DATA_OUT     BCHP_SDS_DSEC_0_DS_SAR_DATA_OUT
#define BCHP_SDS_DSEC_DS_SAR_DC_OFFSET    BCHP_SDS_DSEC_0_DS_SAR_DC_OFFSET
#define BCHP_SDS_DSEC_DS_SAR_LPF_INT      BCHP_SDS_DSEC_0_DS_SAR_LPF_INT
#define BCHP_SDS_DSEC_DS_SAR_CONTROL      BCHP_SDS_DSEC_0_DS_SAR_CONTROL
#define BCHP_SDS_DSEC_DS_COMMON_CONTROL   BCHP_SDS_DSEC_0_DS_COMMON_CONTROL
#define BCHP_SDS_DSEC_DSTMRCTL            BCHP_SDS_DSEC_0_DSTMRCTL
#define BCHP_SDS_DSEC_DSGENTMR1           BCHP_SDS_DSEC_0_DSGENTMR1
#define BCHP_SDS_DSEC_DSGENTMR2           BCHP_SDS_DSEC_0_DSGENTMR2
#define BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_CTL BCHP_SDS_DSEC_0_DS_IDLE_TIMEOUT_CTL
#define BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_STS BCHP_SDS_DSEC_0_DS_IDLE_TIMEOUT_STS

/* SDS_DSEC_AP base registers */
#define BCHP_SDS_DSEC_AP_ANA_CTL          BCHP_SDS_DSEC_AP_0_ANA_CTL
#define BCHP_SDS_DSEC_AP_SW_SPARE0        BCHP_SDS_DSEC_AP_0_SW_SPARE0
#define BCHP_SDS_DSEC_AP_SW_SPARE1        BCHP_SDS_DSEC_AP_0_SW_SPARE1

/* SDS_DSEC_GR_BRIDGE base registers */
#define BCHP_SDS_DSEC_GR_BRIDGE_REVISION     BCHP_SDS_DSEC_GR_BRIDGE_0_REVISION
#define BCHP_SDS_DSEC_GR_BRIDGE_CTRL         BCHP_SDS_DSEC_GR_BRIDGE_0_CTRL
#define BCHP_SDS_DSEC_GR_BRIDGE_SW_INIT_0    BCHP_SDS_DSEC_GR_BRIDGE_0_SW_INIT_0
#define BCHP_SDS_DSEC_GR_BRIDGE_SW_INIT_1    BCHP_SDS_DSEC_GR_BRIDGE_0_SW_INIT_1

/* SDS_DSEC_INTR2 base registers */
#define BCHP_SDS_DSEC_INTR2_CPU_STATUS       BCHP_SDS_DSEC_INTR2_0_CPU_STATUS
#define BCHP_SDS_DSEC_INTR2_CPU_SET          BCHP_SDS_DSEC_INTR2_0_CPU_SET
#define BCHP_SDS_DSEC_INTR2_CPU_CLEAR        BCHP_SDS_DSEC_INTR2_0_CPU_CLEAR
#define BCHP_SDS_DSEC_INTR2_CPU_MASK_STATUS  BCHP_SDS_DSEC_INTR2_0_CPU_MASK_STATUS
#define BCHP_SDS_DSEC_INTR2_CPU_MASK_SET     BCHP_SDS_DSEC_INTR2_0_CPU_MASK_SET
#define BCHP_SDS_DSEC_INTR2_CPU_MASK_CLEAR   BCHP_SDS_DSEC_INTR2_0_CPU_MASK_CLEAR
#define BCHP_SDS_DSEC_INTR2_PCI_STATUS       BCHP_SDS_DSEC_INTR2_0_PCI_STATUS
#define BCHP_SDS_DSEC_INTR2_PCI_SET          BCHP_SDS_DSEC_INTR2_0_PCI_SET
#define BCHP_SDS_DSEC_INTR2_PCI_CLEAR        BCHP_SDS_DSEC_INTR2_0_PCI_CLEAR
#define BCHP_SDS_DSEC_INTR2_PCI_MASK_STATUS  BCHP_SDS_DSEC_INTR2_0_PCI_MASK_STATUS
#define BCHP_SDS_DSEC_INTR2_PCI_MASK_SET     BCHP_SDS_DSEC_INTR2_0_PCI_MASK_SET
#define BCHP_SDS_DSEC_INTR2_PCI_MASK_CLEAR   BCHP_SDS_DSEC_INTR2_0_PCI_MASK_CLEAR

#endif /* BWFE_7364_PRIV_H__ */