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
 * $brcm_Workfile: bchp_xpt_pb10.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/17/12 6:13p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 17 13:26:47 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b2/bchp_xpt_pb10.h $
 * 
 * Hydra_Software_Devel/1   1/17/12 6:13p vanessah
 * SW7425-2202: add B2 headers
 *
 ***************************************************************************/

#ifndef BCHP_XPT_PB10_H__
#define BCHP_XPT_PB10_H__

/***************************************************************************
 *XPT_PB10 - Playback 10 Control Registers
 ***************************************************************************/
#define BCHP_XPT_PB10_CTRL1                      0x00908600 /* Playback Control 1 Register */
#define BCHP_XPT_PB10_CTRL2                      0x00908604 /* Playback Control 2 Register */
#define BCHP_XPT_PB10_CTRL3                      0x00908608 /* Playback Control 3 Register */
#define BCHP_XPT_PB10_CTRL4                      0x0090860c /* Playback Control 4 Register */
#define BCHP_XPT_PB10_FIRST_DESC_ADDR            0x00908610 /* Playback First Descriptor Address Register */
#define BCHP_XPT_PB10_CURR_DESC_ADDR             0x00908614 /* Playback Current Descriptor Address Register */
#define BCHP_XPT_PB10_CURR_BUFF_ADDR             0x00908618 /* Playback Current Buffer Address Register */
#define BCHP_XPT_PB10_BLOCKOUT                   0x0090861c /* Data Transport Playback Block Out Control */
#define BCHP_XPT_PB10_PKTZ_CONTEXT0              0x00908620 /* Data Transport Playback Packetize Mode Context 0 Control */
#define BCHP_XPT_PB10_TS_ERR_BOUND_EARLY         0x00908624 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB10_TS_ERR_BOUND_LATE          0x00908628 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB10_PARSER_CTRL1               0x0090862c /* Data Transport Playback Parser Control Register */
#define BCHP_XPT_PB10_PARSER_CTRL2               0x00908630 /* Data Transport Playback Parser Control Register 2 */
#define BCHP_XPT_PB10_PARSER_TIMESTAMP           0x00908634 /* Data Transport Playback Parser Local Timestamp */
#define BCHP_XPT_PB10_INTR                       0x00908638 /* Playback Processing Error and Status Interrupt Register */
#define BCHP_XPT_PB10_INTR_EN                    0x0090863c /* Playback Processing Error and Status Interrupt Enable Register */
#define BCHP_XPT_PB10_INTR_TAGS                  0x00908640 /* Playback Interrupt Tag Register */
#define BCHP_XPT_PB10_PACING_PCR_PID             0x00908644 /* Playback PCR Based Pacing PCR Pid */
#define BCHP_XPT_PB10_PWR_CTRL                   0x00908648 /* Playback Power Control Register */
#define BCHP_XPT_PB10_MISC_STATUS_REG            0x0090864c /* Miscellaneous Playback Status Registers */
#define BCHP_XPT_PB10_ASF_CTRL                   0x00908650 /* ASF Control Register */
#define BCHP_XPT_PB10_ASF_DATA_PACKET_LENGTH     0x00908654 /* ASF_DATA_PACKET_LENGTH */
#define BCHP_XPT_PB10_ASF_SUB_PAYLOAD_PKT_TYPE   0x00908658 /* ASF_SUB_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB10_ASF_PAYLOAD_PKT_TYPE       0x0090865c /* ASF_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB10_PLAYBACK_PARSER_BAND_ID    0x00908660 /* PLAYBACK_PARSER_BAND_ID */
#define BCHP_XPT_PB10_NEXT_PACKET_PACING_TIMESTAMP 0x00908664 /* Next Packet Pacing Timestamp Value */
#define BCHP_XPT_PB10_PKT2PKT_PACING_TIMESTAMP_DELTA 0x00908668 /* PKT2PKT Pacing TimeStamp Delta */
#define BCHP_XPT_PB10_PACING_COUNTER             0x0090866c /* Pacing Counter Instantaneous value */
#define BCHP_XPT_PB10_ERROR_VALUE_WATERMARK      0x00908670 /* WaterMark level of the Pacing Error Bound Reached */
#define BCHP_XPT_PB10_PES_BASED_PACING_CTRL      0x00908674 /* PES Based Pacing Controls */

#endif /* #ifndef BCHP_XPT_PB10_H__ */

/* End of File */