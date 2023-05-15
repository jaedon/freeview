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
 * $brcm_Workfile: bchp_xpt_pb11.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/28/11 1:57a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 17:16:54 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_xpt_pb11.h $
 * 
 * Hydra_Software_Devel/1   10/28/11 1:57a vanessah
 * SW7425-1620: add 7425 B0 rdb header file
 *
 ***************************************************************************/

#ifndef BCHP_XPT_PB11_H__
#define BCHP_XPT_PB11_H__

/***************************************************************************
 *XPT_PB11 - Playback 11 Control Registers
 ***************************************************************************/
#define BCHP_XPT_PB11_CTRL1                      0x00908680 /* Playback Control 1 Register */
#define BCHP_XPT_PB11_CTRL2                      0x00908684 /* Playback Control 2 Register */
#define BCHP_XPT_PB11_CTRL3                      0x00908688 /* Playback Control 3 Register */
#define BCHP_XPT_PB11_CTRL4                      0x0090868c /* Playback Control 4 Register */
#define BCHP_XPT_PB11_FIRST_DESC_ADDR            0x00908690 /* Playback First Descriptor Address Register */
#define BCHP_XPT_PB11_CURR_DESC_ADDR             0x00908694 /* Playback Current Descriptor Address Register */
#define BCHP_XPT_PB11_CURR_BUFF_ADDR             0x00908698 /* Playback Current Buffer Address Register */
#define BCHP_XPT_PB11_BLOCKOUT                   0x0090869c /* Data Transport Playback Block Out Control */
#define BCHP_XPT_PB11_PKTZ_CONTEXT0              0x009086a0 /* Data Transport Playback Packetize Mode Context 0 Control */
#define BCHP_XPT_PB11_TS_ERR_BOUND_EARLY         0x009086a4 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB11_TS_ERR_BOUND_LATE          0x009086a8 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB11_PARSER_CTRL1               0x009086ac /* Data Transport Playback Parser Control Register */
#define BCHP_XPT_PB11_PARSER_CTRL2               0x009086b0 /* Data Transport Playback Parser Control Register 2 */
#define BCHP_XPT_PB11_PARSER_TIMESTAMP           0x009086b4 /* Data Transport Playback Parser Local Timestamp */
#define BCHP_XPT_PB11_INTR                       0x009086b8 /* Playback Processing Error and Status Interrupt Register */
#define BCHP_XPT_PB11_INTR_EN                    0x009086bc /* Playback Processing Error and Status Interrupt Enable Register */
#define BCHP_XPT_PB11_INTR_TAGS                  0x009086c0 /* Playback Interrupt Tag Register */
#define BCHP_XPT_PB11_PACING_PCR_PID             0x009086c4 /* Playback PCR Based Pacing PCR Pid */
#define BCHP_XPT_PB11_PWR_CTRL                   0x009086c8 /* Playback Power Control Register */
#define BCHP_XPT_PB11_MISC_STATUS_REG            0x009086cc /* Miscellaneous Playback Status Registers */
#define BCHP_XPT_PB11_ASF_CTRL                   0x009086d0 /* ASF Control Register */
#define BCHP_XPT_PB11_ASF_DATA_PACKET_LENGTH     0x009086d4 /* ASF_DATA_PACKET_LENGTH */
#define BCHP_XPT_PB11_ASF_SUB_PAYLOAD_PKT_TYPE   0x009086d8 /* ASF_SUB_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB11_ASF_PAYLOAD_PKT_TYPE       0x009086dc /* ASF_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB11_PLAYBACK_PARSER_BAND_ID    0x009086e0 /* PLAYBACK_PARSER_BAND_ID */
#define BCHP_XPT_PB11_NEXT_PACKET_PACING_TIMESTAMP 0x009086e4 /* Next Packet Pacing Timestamp Value */
#define BCHP_XPT_PB11_PKT2PKT_PACING_TIMESTAMP_DELTA 0x009086e8 /* PKT2PKT Pacing TimeStamp Delta */
#define BCHP_XPT_PB11_PACING_COUNTER             0x009086ec /* Pacing Counter Instantaneous value */
#define BCHP_XPT_PB11_ERROR_VALUE_WATERMARK      0x009086f0 /* WaterMark level of the Pacing Error Bound Reached */
#define BCHP_XPT_PB11_PES_BASED_PACING_CTRL      0x009086f4 /* PES Based Pacing Controls */

#endif /* #ifndef BCHP_XPT_PB11_H__ */

/* End of File */