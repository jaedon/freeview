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
 * $brcm_Workfile: bchp_int_id_hdmi_rx_intr2_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 7:14p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:43:43 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_int_id_hdmi_rx_intr2_0.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 7:14p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_hdmi_rx_intr2_0.h"

#ifndef BCHP_INT_ID_HDMI_RX_INTR2_0_H__
#define BCHP_INT_ID_HDMI_RX_INTR2_0_H__

#define BCHP_INT_ID_AKSV_UPDATE               BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_AKSV_UPDATE_SHIFT)
#define BCHP_INT_ID_AUDIO_FIFO_OVER_FLOW      BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_AUDIO_FIFO_OVER_FLOW_SHIFT)
#define BCHP_INT_ID_AUDIO_FIFO_UNDER_FLOW     BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_AUDIO_FIFO_UNDER_FLOW_SHIFT)
#define BCHP_INT_ID_AUDIO_TYPE_CHANGE         BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_AUDIO_TYPE_CHANGE_SHIFT)
#define BCHP_INT_ID_AUDIO_VALIDITY_BIT_UPDATE BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_AUDIO_VALIDITY_BIT_UPDATE_SHIFT)
#define BCHP_INT_ID_AV_MUTE_UPDATE            BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_AV_MUTE_UPDATE_SHIFT)
#define BCHP_INT_ID_CHANNEL_STATUS_UPDATE     BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_CHANNEL_STATUS_UPDATE_SHIFT)
#define BCHP_INT_ID_DFE_ALIGN_LOSS            BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_DFE_ALIGN_LOSS_SHIFT)
#define BCHP_INT_ID_DFE_LOCK_UPDATE           BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_DFE_LOCK_UPDATE_SHIFT)
#define BCHP_INT_ID_DVI_TO_HDMI               BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_DVI_TO_HDMI_SHIFT)
#define BCHP_INT_ID_ERROR_INTERRUPT           BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_ERROR_INTERRUPT_SHIFT)
#define BCHP_INT_ID_EXCESSIVE_PACKET_ERRORS   BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_EXCESSIVE_PACKET_ERRORS_SHIFT)
#define BCHP_INT_ID_FORMAT_DETECT_COUNT_SATURATED BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_FORMAT_DETECT_COUNT_SATURATED_SHIFT)
#define BCHP_INT_ID_HDMI_TO_DVI               BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_HDMI_TO_DVI_SHIFT)
#define BCHP_INT_ID_I2C_TRANSACTION_COMPLETE  BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_I2C_TRANSACTION_COMPLETE_SHIFT)
#define BCHP_INT_ID_INVALID_DATA_ISLAND_LENGTH BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_INVALID_DATA_ISLAND_LENGTH_SHIFT)
#define BCHP_INT_ID_LAYOUT_UPDATE             BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_LAYOUT_UPDATE_SHIFT)
#define BCHP_INT_ID_PACKET_SYNC_ERROR         BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_PACKET_SYNC_ERROR_SHIFT)
#define BCHP_INT_ID_PJ_UPDATE                 BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_PJ_UPDATE_SHIFT)
#define BCHP_INT_ID_RAM_PACKET_STOP           BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_RAM_PACKET_STOP_SHIFT)
#define BCHP_INT_ID_RAM_PACKET_UPDATE         BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_RAM_PACKET_UPDATE_SHIFT)
#define BCHP_INT_ID_REQUEST_KEYS              BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_REQUEST_KEYS_SHIFT)
#define BCHP_INT_ID_REQUEST_KSVS              BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_REQUEST_KSVS_SHIFT)
#define BCHP_INT_ID_RGB_UNDER_RANGE           BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_RGB_UNDER_RANGE_SHIFT)
#define BCHP_INT_ID_SET_AV_MUTE               BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_SET_AV_MUTE_SHIFT)
#define BCHP_INT_ID_SYMBOL_LOSS               BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_SYMBOL_LOSS_SHIFT)
#define BCHP_INT_ID_VERTICAL_BLANK_END        BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_VERTICAL_BLANK_END_SHIFT)
#define BCHP_INT_ID_VSYNC_LEAD_EDGE           BCHP_INT_ID_CREATE(BCHP_HDMI_RX_INTR2_0_CPU_STATUS, BCHP_HDMI_RX_INTR2_0_CPU_STATUS_VSYNC_LEAD_EDGE_SHIFT)

#endif /* #ifndef BCHP_INT_ID_HDMI_RX_INTR2_0_H__ */

/* End of File */