/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_int_id_upg_aux_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 5:01p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:08:31 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_int_id_upg_aux_intr2.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 5:01p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_upg_aux_intr2.h"

#ifndef BCHP_INT_ID_UPG_AUX_INTR2_H__
#define BCHP_INT_ID_UPG_AUX_INTR2_H__

#define BCHP_INT_ID_TMON_DATA_READY           BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_DATA_READY_SHIFT)
#define BCHP_INT_ID_TMON_OVER_TEMPERATURE     BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_OVER_TEMPERATURE_SHIFT)
#define BCHP_INT_ID_UART_DMA_RX_ABORT         BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_UART_DMA_RX_ABORT_SHIFT)
#define BCHP_INT_ID_UART_DMA_RX_DONE          BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_UART_DMA_RX_DONE_SHIFT)
#define BCHP_INT_ID_UART_DMA_TX_ABORT         BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_UART_DMA_TX_ABORT_SHIFT)
#define BCHP_INT_ID_UART_DMA_TX_DONE          BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_UART_DMA_TX_DONE_SHIFT)
#define BCHP_INT_ID_ctk_app_reg_written       BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_app_reg_written_SHIFT)
#define BCHP_INT_ID_ctk_fifo_inact_event      BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_fifo_inact_event_SHIFT)
#define BCHP_INT_ID_ctk_fifo_lvl_event        BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_fifo_lvl_event_SHIFT)
#define BCHP_INT_ID_ctk_fifo_overflow         BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_fifo_overflow_SHIFT)
#define BCHP_INT_ID_ctk_fifo_underflow        BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_fifo_underflow_SHIFT)
#define BCHP_INT_ID_ctk_irq_status_read       BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_irq_status_read_SHIFT)
#define BCHP_INT_ID_ctk_key_event             BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_key_event_SHIFT)
#define BCHP_INT_ID_ctk_potential_touch       BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_potential_touch_SHIFT)
#define BCHP_INT_ID_ctk_re_cal                BCHP_INT_ID_CREATE(BCHP_UPG_AUX_INTR2_CPU_STATUS, BCHP_UPG_AUX_INTR2_CPU_STATUS_ctk_re_cal_SHIFT)

#endif /* #ifndef BCHP_INT_ID_UPG_AUX_INTR2_H__ */

/* End of File */