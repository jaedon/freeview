/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_int_id_hdmi_tx_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/18/11 4:19p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue May 17 04:42:18 2011
 *                 MD5 Checksum         f5f09b2bf7ad40890d2e5dc57d4789b6
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/a0/bchp_int_id_hdmi_tx_intr2.h $
 * 
 * Hydra_Software_Devel/2   5/18/11 4:19p xhuang
 * SW7552-2: update with central RDB
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_hdmi_tx_intr2.h"

#ifndef BCHP_INT_ID_HDMI_TX_INTR2_H__
#define BCHP_INT_ID_HDMI_TX_INTR2_H__

#define BCHP_INT_ID_DRIFT_FIFO_ALMOST_EMPTY_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_DRIFT_FIFO_ALMOST_EMPTY_INTR_SHIFT)
#define BCHP_INT_ID_DRIFT_FIFO_ALMOST_FULL_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_DRIFT_FIFO_ALMOST_FULL_INTR_SHIFT)
#define BCHP_INT_ID_DRIFT_FIFO_EMPTY_MINUS_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_DRIFT_FIFO_EMPTY_MINUS_INTR_SHIFT)
#define BCHP_INT_ID_DRIFT_FIFO_FULL_MINUS_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_DRIFT_FIFO_FULL_MINUS_INTR_SHIFT)
#define BCHP_INT_ID_DVP_FORMAT_DET_SATURATED_COUNT_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_DVP_FORMAT_DET_SATURATED_COUNT_INTR_SHIFT)
#define BCHP_INT_ID_DVP_FORMAT_DET_UPDATE_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_DVP_FORMAT_DET_UPDATE_INTR_SHIFT)
#define BCHP_INT_ID_HDCP_AN_READY_INTR        BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HDCP_AN_READY_INTR_SHIFT)
#define BCHP_INT_ID_HDCP_PJ_INTR              BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HDCP_PJ_INTR_SHIFT)
#define BCHP_INT_ID_HDCP_REPEATER_ERR_INTR    BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HDCP_REPEATER_ERR_INTR_SHIFT)
#define BCHP_INT_ID_HDCP_RI_INTR              BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HDCP_RI_INTR_SHIFT)
#define BCHP_INT_ID_HDCP_V_MATCH_INTR         BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HDCP_V_MATCH_INTR_SHIFT)
#define BCHP_INT_ID_HDCP_V_MISMATCH_INTR      BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HDCP_V_MISMATCH_INTR_SHIFT)
#define BCHP_INT_ID_HOTPLUG_INTR              BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_HOTPLUG_INTR_SHIFT)
#define BCHP_INT_ID_ILLEGAL_WRITE_TO_ACTIVE_RAM_PACKET_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_ILLEGAL_WRITE_TO_ACTIVE_RAM_PACKET_INTR_SHIFT)
#define BCHP_INT_ID_MAI_FORMAT_UPDATE_INTR    BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_MAI_FORMAT_UPDATE_INTR_SHIFT)
#define BCHP_INT_ID_PACKET_FIFO_OVERFLOW_INTR BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_PACKET_FIFO_OVERFLOW_INTR_SHIFT)
#define BCHP_INT_ID_PJ_MISMATCH_INTR          BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_PJ_MISMATCH_INTR_SHIFT)
#define BCHP_INT_ID_RGB_UNDER_RANGE_INTR      BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_RGB_UNDER_RANGE_INTR_SHIFT)
#define BCHP_INT_ID_RI_A_MISMATCH_INTR        BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_RI_A_MISMATCH_INTR_SHIFT)
#define BCHP_INT_ID_RI_B_MISMATCH_INTR        BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_RI_B_MISMATCH_INTR_SHIFT)
#define BCHP_INT_ID_RSEN_UPDATE_INTR          BCHP_INT_ID_CREATE(BCHP_HDMI_TX_INTR2_CPU_STATUS, BCHP_HDMI_TX_INTR2_CPU_STATUS_RSEN_UPDATE_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_HDMI_TX_INTR2_H__ */

/* End of File */