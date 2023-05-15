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
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Sep 19 16:05:39 2011
 *                 MD5 Checksum         f84a7991a99ffcf5cce3ba6b166ba0af
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_XPT_PB6_H__
#define BCHP_XPT_PB6_H__

/***************************************************************************
 *XPT_PB6 - Playback 6 Control Registers
 ***************************************************************************/
#define BCHP_XPT_PB6_CTRL1                       0x00948400 /* Playback Control 1 Register */
#define BCHP_XPT_PB6_CTRL2                       0x00948404 /* Playback Control 2 Register */
#define BCHP_XPT_PB6_CTRL3                       0x00948408 /* Playback Control 3 Register */
#define BCHP_XPT_PB6_CTRL4                       0x0094840c /* Playback Control 4 Register */
#define BCHP_XPT_PB6_FIRST_DESC_ADDR             0x00948410 /* Playback First Descriptor Address Register */
#define BCHP_XPT_PB6_CURR_DESC_ADDR              0x00948414 /* Playback Current Descriptor Address Register */
#define BCHP_XPT_PB6_CURR_BUFF_ADDR              0x00948418 /* Playback Current Buffer Address Register */
#define BCHP_XPT_PB6_BLOCKOUT                    0x0094841c /* Data Transport Playback Block Out Control */
#define BCHP_XPT_PB6_PKTZ_CONTEXT0               0x00948420 /* Data Transport Playback Packetize Mode Context 0 Control */
#define BCHP_XPT_PB6_TS_ERR_BOUND_EARLY          0x00948424 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB6_TS_ERR_BOUND_LATE           0x00948428 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB6_PARSER_CTRL1                0x0094842c /* Data Transport Playback Parser Control Register */
#define BCHP_XPT_PB6_PARSER_CTRL2                0x00948430 /* Data Transport Playback Parser Control Register 2 */
#define BCHP_XPT_PB6_PARSER_TIMESTAMP            0x00948434 /* Data Transport Playback Parser Local Timestamp */
#define BCHP_XPT_PB6_INTR                        0x00948438 /* Playback Processing Error and Status Interrupt Register */
#define BCHP_XPT_PB6_INTR_EN                     0x0094843c /* Playback Processing Error and Status Interrupt Enable Register */
#define BCHP_XPT_PB6_INTR_TAGS                   0x00948440 /* Playback Interrupt Tag Register */
#define BCHP_XPT_PB6_PACING_PCR_PID              0x00948444 /* Playback PCR Based Pacing PCR Pid */
#define BCHP_XPT_PB6_PWR_CTRL                    0x00948448 /* Playback Power Control Register */
#define BCHP_XPT_PB6_MISC_STATUS_REG             0x0094844c /* Miscellaneous Playback Status Registers */
#define BCHP_XPT_PB6_ASF_CTRL                    0x00948450 /* ASF Control Register */
#define BCHP_XPT_PB6_ASF_DATA_PACKET_LENGTH      0x00948454 /* ASF_DATA_PACKET_LENGTH */
#define BCHP_XPT_PB6_ASF_SUB_PAYLOAD_PKT_TYPE    0x00948458 /* ASF_SUB_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB6_ASF_PAYLOAD_PKT_TYPE        0x0094845c /* ASF_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB6_PLAYBACK_PARSER_BAND_ID     0x00948460 /* PLAYBACK_PARSER_BAND_ID */
#define BCHP_XPT_PB6_NEXT_PACKET_PACING_TIMESTAMP 0x00948464 /* Next Packet Pacing Timestamp Value */
#define BCHP_XPT_PB6_PKT2PKT_PACING_TIMESTAMP_DELTA 0x00948468 /* PKT2PKT Pacing TimeStamp Delta */
#define BCHP_XPT_PB6_PACING_COUNTER              0x0094846c /* Pacing Counter Instantaneous value */
#define BCHP_XPT_PB6_ERROR_VALUE_WATERMARK       0x00948470 /* WaterMark level of the Pacing Error Bound Reached */
#define BCHP_XPT_PB6_PES_BASED_PACING_CTRL       0x00948474 /* PES Based Pacing Controls */

#endif /* #ifndef BCHP_XPT_PB6_H__ */

/* End of File */