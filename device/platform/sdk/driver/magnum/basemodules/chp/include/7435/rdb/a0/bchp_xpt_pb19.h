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
 * $brcm_Workfile: bchp_xpt_pb19.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 10:08p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:26:55 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_xpt_pb19.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 10:08p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_PB19_H__
#define BCHP_XPT_PB19_H__

/***************************************************************************
 *XPT_PB19 - Playback 19 Control Registers
 ***************************************************************************/
#define BCHP_XPT_PB19_CTRL1                      0x00948a80 /* Playback Control 1 Register */
#define BCHP_XPT_PB19_CTRL2                      0x00948a84 /* Playback Control 2 Register */
#define BCHP_XPT_PB19_CTRL3                      0x00948a88 /* Playback Control 3 Register */
#define BCHP_XPT_PB19_CTRL4                      0x00948a8c /* Playback Control 4 Register */
#define BCHP_XPT_PB19_FIRST_DESC_ADDR            0x00948a90 /* Playback First Descriptor Address Register */
#define BCHP_XPT_PB19_CURR_DESC_ADDR             0x00948a94 /* Playback Current Descriptor Address Register */
#define BCHP_XPT_PB19_CURR_BUFF_ADDR             0x00948a98 /* Playback Current Buffer Address Register */
#define BCHP_XPT_PB19_BLOCKOUT                   0x00948a9c /* Data Transport Playback Block Out Control */
#define BCHP_XPT_PB19_PKTZ_CONTEXT0              0x00948aa0 /* Data Transport Playback Packetize Mode Context 0 Control */
#define BCHP_XPT_PB19_TS_ERR_BOUND_EARLY         0x00948aa4 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB19_TS_ERR_BOUND_LATE          0x00948aa8 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB19_PARSER_CTRL1               0x00948aac /* Data Transport Playback Parser Control Register */
#define BCHP_XPT_PB19_PARSER_CTRL2               0x00948ab0 /* Data Transport Playback Parser Control Register 2 */
#define BCHP_XPT_PB19_PARSER_TIMESTAMP           0x00948ab4 /* Data Transport Playback Parser Local Timestamp */
#define BCHP_XPT_PB19_INTR                       0x00948ab8 /* Playback Processing Error and Status Interrupt Register */
#define BCHP_XPT_PB19_INTR_EN                    0x00948abc /* Playback Processing Error and Status Interrupt Enable Register */
#define BCHP_XPT_PB19_INTR_TAGS                  0x00948ac0 /* Playback Interrupt Tag Register */
#define BCHP_XPT_PB19_PACING_PCR_PID             0x00948ac4 /* Playback PCR Based Pacing PCR Pid */
#define BCHP_XPT_PB19_PWR_CTRL                   0x00948ac8 /* Playback Power Control Register */
#define BCHP_XPT_PB19_MISC_STATUS_REG            0x00948acc /* Miscellaneous Playback Status Registers */
#define BCHP_XPT_PB19_ASF_CTRL                   0x00948ad0 /* ASF Control Register */
#define BCHP_XPT_PB19_ASF_DATA_PACKET_LENGTH     0x00948ad4 /* ASF_DATA_PACKET_LENGTH */
#define BCHP_XPT_PB19_ASF_SUB_PAYLOAD_PKT_TYPE   0x00948ad8 /* ASF_SUB_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB19_ASF_PAYLOAD_PKT_TYPE       0x00948adc /* ASF_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB19_PLAYBACK_PARSER_BAND_ID    0x00948ae0 /* PLAYBACK_PARSER_BAND_ID */
#define BCHP_XPT_PB19_NEXT_PACKET_PACING_TIMESTAMP 0x00948ae4 /* Next Packet Pacing Timestamp Value */
#define BCHP_XPT_PB19_PKT2PKT_PACING_TIMESTAMP_DELTA 0x00948ae8 /* PKT2PKT Pacing TimeStamp Delta */
#define BCHP_XPT_PB19_PACING_COUNTER             0x00948aec /* Pacing Counter Instantaneous value */
#define BCHP_XPT_PB19_ERROR_VALUE_WATERMARK      0x00948af0 /* WaterMark level of the Pacing Error Bound Reached */
#define BCHP_XPT_PB19_PES_BASED_PACING_CTRL      0x00948af4 /* PES Based Pacing Controls */

#endif /* #ifndef BCHP_XPT_PB19_H__ */

/* End of File */