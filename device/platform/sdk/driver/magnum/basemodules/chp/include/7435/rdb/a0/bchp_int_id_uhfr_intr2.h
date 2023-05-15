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
 * $brcm_Workfile: bchp_int_id_uhfr_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 7:20p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:44:41 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_int_id_uhfr_intr2.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 7:20p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_uhfr_intr2.h"

#ifndef BCHP_INT_ID_UHFR_INTR2_H__
#define BCHP_INT_ID_UHFR_INTR2_H__

#define BCHP_INT_ID_ALT_MAX_TIMEOUT           BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_ALT_MAX_TIMEOUT_SHIFT)
#define BCHP_INT_ID_ALT_PACKET_END            BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_ALT_PACKET_END_SHIFT)
#define BCHP_INT_ID_ALT_SFR                   BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_ALT_SFR_SHIFT)
#define BCHP_INT_ID_COLLISION_FALL            BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_COLLISION_FALL_SHIFT)
#define BCHP_INT_ID_COLLISION_RISE            BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_COLLISION_RISE_SHIFT)
#define BCHP_INT_ID_CORR_ALT_CHSM_ERR         BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_CHSM_ERR_SHIFT)
#define BCHP_INT_ID_CORR_ALT_NO_GAP           BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_NO_GAP_SHIFT)
#define BCHP_INT_ID_CORR_ALT_NO_HLTR          BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_NO_HLTR_SHIFT)
#define BCHP_INT_ID_CORR_ALT_NO_LHTR          BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_NO_LHTR_SHIFT)
#define BCHP_INT_ID_CORR_ALT_WRONG_TR         BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_WRONG_TR_SHIFT)
#define BCHP_INT_ID_CORR_BCH_ERR              BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_SHIFT)
#define BCHP_INT_ID_CORR_DATA_CORR_VLD        BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_DATA_CORR_VLD_SHIFT)
#define BCHP_INT_ID_CORR_DECODE_PR1_END       BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_DECODE_PR1_END_SHIFT)
#define BCHP_INT_ID_CORR_DECODE_PR2_END       BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_DECODE_PR2_END_SHIFT)
#define BCHP_INT_ID_CORR_PR1_DETECTED         BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_PR1_DETECTED_SHIFT)
#define BCHP_INT_ID_CORR_PR1_PACKET_END       BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_PR1_PACKET_END_SHIFT)
#define BCHP_INT_ID_CORR_PR2_DETECTED         BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_PR2_DETECTED_SHIFT)
#define BCHP_INT_ID_CORR_PR2_PACKET_END       BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_PR2_PACKET_END_SHIFT)
#define BCHP_INT_ID_CORR_STATE_INT            BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_CORR_STATE_INT_SHIFT)
#define BCHP_INT_ID_IRR_PR1_DETECTED_FALL     BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_IRR_PR1_DETECTED_FALL_SHIFT)
#define BCHP_INT_ID_IRR_PR1_DETECTED_RISE     BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_IRR_PR1_DETECTED_RISE_SHIFT)
#define BCHP_INT_ID_IRR_PR2_DETECTED_FALL     BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_IRR_PR2_DETECTED_FALL_SHIFT)
#define BCHP_INT_ID_IRR_PR2_DETECTED_RISE     BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_IRR_PR2_DETECTED_RISE_SHIFT)
#define BCHP_INT_ID_PTP_TM_TMOUT              BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_PTP_TM_TMOUT_SHIFT)
#define BCHP_INT_ID_SLOW_RSSI_FALL            BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_SLOW_RSSI_FALL_SHIFT)
#define BCHP_INT_ID_SLOW_RSSI_RISE            BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_SLOW_RSSI_RISE_SHIFT)
#define BCHP_INT_ID_SS_DONE                   BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_SS_DONE_SHIFT)
#define BCHP_INT_ID_STRB_20KHZ                BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_STRB_20KHZ_SHIFT)
#define BCHP_INT_ID_STRB_500KHZ               BCHP_INT_ID_CREATE(BCHP_UHFR_INTR2_CPU_STATUS, BCHP_UHFR_INTR2_CPU_STATUS_STRB_500KHZ_SHIFT)

#endif /* #ifndef BCHP_INT_ID_UHFR_INTR2_H__ */

/* End of File */