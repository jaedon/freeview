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
 * $brcm_Workfile: bchp_xpt_pb9.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 7:02p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:18 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_xpt_pb9.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 7:02p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_XPT_PB9_H__
#define BCHP_XPT_PB9_H__

/***************************************************************************
 *XPT_PB9 - Playback 9 Control Registers
 ***************************************************************************/
#define BCHP_XPT_PB9_CTRL1                       0x00948580 /* Playback Control 1 Register */
#define BCHP_XPT_PB9_CTRL2                       0x00948584 /* Playback Control 2 Register */
#define BCHP_XPT_PB9_CTRL3                       0x00948588 /* Playback Control 3 Register */
#define BCHP_XPT_PB9_CTRL4                       0x0094858c /* Playback Control 4 Register */
#define BCHP_XPT_PB9_FIRST_DESC_ADDR             0x00948590 /* Playback First Descriptor Address Register */
#define BCHP_XPT_PB9_CURR_DESC_ADDR              0x00948594 /* Playback Current Descriptor Address Register */
#define BCHP_XPT_PB9_CURR_BUFF_ADDR              0x00948598 /* Playback Current Buffer Address Register */
#define BCHP_XPT_PB9_BLOCKOUT                    0x0094859c /* Data Transport Playback Block Out Control */
#define BCHP_XPT_PB9_PKTZ_CONTEXT0               0x009485a0 /* Data Transport Playback Packetize Mode Context 0 Control */
#define BCHP_XPT_PB9_TS_ERR_BOUND_EARLY          0x009485a4 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB9_TS_ERR_BOUND_LATE           0x009485a8 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB9_PARSER_CTRL1                0x009485ac /* Data Transport Playback Parser Control Register */
#define BCHP_XPT_PB9_PARSER_CTRL2                0x009485b0 /* Data Transport Playback Parser Control Register 2 */
#define BCHP_XPT_PB9_PARSER_TIMESTAMP            0x009485b4 /* Data Transport Playback Parser Local Timestamp */
#define BCHP_XPT_PB9_INTR                        0x009485b8 /* Playback Processing Error and Status Interrupt Register */
#define BCHP_XPT_PB9_INTR_EN                     0x009485bc /* Playback Processing Error and Status Interrupt Enable Register */
#define BCHP_XPT_PB9_INTR_TAGS                   0x009485c0 /* Playback Interrupt Tag Register */
#define BCHP_XPT_PB9_PACING_PCR_PID              0x009485c4 /* Playback PCR Based Pacing PCR Pid */
#define BCHP_XPT_PB9_PWR_CTRL                    0x009485c8 /* Playback Power Control Register */
#define BCHP_XPT_PB9_MISC_STATUS_REG             0x009485cc /* Miscellaneous Playback Status Registers */
#define BCHP_XPT_PB9_ASF_CTRL                    0x009485d0 /* ASF Control Register */
#define BCHP_XPT_PB9_ASF_DATA_PACKET_LENGTH      0x009485d4 /* ASF_DATA_PACKET_LENGTH */
#define BCHP_XPT_PB9_ASF_SUB_PAYLOAD_PKT_TYPE    0x009485d8 /* ASF_SUB_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB9_ASF_PAYLOAD_PKT_TYPE        0x009485dc /* ASF_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB9_PLAYBACK_PARSER_BAND_ID     0x009485e0 /* PLAYBACK_PARSER_BAND_ID */
#define BCHP_XPT_PB9_NEXT_PACKET_PACING_TIMESTAMP 0x009485e4 /* Next Packet Pacing Timestamp Value */
#define BCHP_XPT_PB9_PKT2PKT_PACING_TIMESTAMP_DELTA 0x009485e8 /* PKT2PKT Pacing TimeStamp Delta */
#define BCHP_XPT_PB9_PACING_COUNTER              0x009485ec /* Pacing Counter Instantaneous value */
#define BCHP_XPT_PB9_ERROR_VALUE_WATERMARK       0x009485f0 /* WaterMark level of the Pacing Error Bound Reached */
#define BCHP_XPT_PB9_PES_BASED_PACING_CTRL       0x009485f4 /* PES Based Pacing Controls */

#endif /* #ifndef BCHP_XPT_PB9_H__ */

/* End of File */