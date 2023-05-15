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
 * $brcm_Workfile: bchp_xpt_pb2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 2:43p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:31:49 2011
 *                 MD5 Checksum         d03d08c4839c3311c9d35c4cd5e10373
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_xpt_pb2.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 2:43p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_PB2_H__
#define BCHP_XPT_PB2_H__

/***************************************************************************
 *XPT_PB2 - Playback 2 Control Registers
 ***************************************************************************/
#define BCHP_XPT_PB2_CTRL1                       0x00387100 /* Playback Control 1 Register */
#define BCHP_XPT_PB2_CTRL2                       0x00387104 /* Playback Control 2 Register */
#define BCHP_XPT_PB2_CTRL3                       0x00387108 /* Playback Control 3 Register */
#define BCHP_XPT_PB2_CTRL4                       0x0038710c /* Playback Control 4 Register */
#define BCHP_XPT_PB2_FIRST_DESC_ADDR             0x00387110 /* Playback First Descriptor Address Register */
#define BCHP_XPT_PB2_CURR_DESC_ADDR              0x00387114 /* Playback Current Descriptor Address Register */
#define BCHP_XPT_PB2_CURR_BUFF_ADDR              0x00387118 /* Playback Current Buffer Address Register */
#define BCHP_XPT_PB2_BLOCKOUT                    0x0038711c /* Data Transport Playback Block Out Control */
#define BCHP_XPT_PB2_PKTZ_CONTEXT0               0x00387120 /* Data Transport Playback Packetize Mode Context 0 Control */
#define BCHP_XPT_PB2_TS_ERR_BOUND_EARLY          0x00387124 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB2_TS_ERR_BOUND_LATE           0x00387128 /* Data Transport Playback Timestamp Error Bound Register */
#define BCHP_XPT_PB2_PARSER_CTRL1                0x0038712c /* Data Transport Playback Parser Control Register */
#define BCHP_XPT_PB2_PARSER_CTRL2                0x00387130 /* Data Transport Playback Parser Control Register 2 */
#define BCHP_XPT_PB2_PARSER_TIMESTAMP            0x00387134 /* Data Transport Playback Parser Local Timestamp */
#define BCHP_XPT_PB2_INTR                        0x00387138 /* Playback Processing Error and Status Interrupt Register */
#define BCHP_XPT_PB2_INTR_EN                     0x0038713c /* Playback Processing Error and Status Interrupt Enable Register */
#define BCHP_XPT_PB2_INTR_TAGS                   0x00387140 /* Playback Interrupt Tag Register */
#define BCHP_XPT_PB2_PACING_PCR_PID              0x00387144 /* Playback PCR Based Pacing PCR Pid */
#define BCHP_XPT_PB2_PWR_CTRL                    0x00387148 /* Playback Power Control Register */
#define BCHP_XPT_PB2_MISC_STATUS_REG             0x0038714c /* Miscellaneous Playback Status Registers */
#define BCHP_XPT_PB2_ASF_CTRL                    0x00387150 /* ASF Control Register */
#define BCHP_XPT_PB2_ASF_DATA_PACKET_LENGTH      0x00387154 /* ASF_DATA_PACKET_LENGTH */
#define BCHP_XPT_PB2_ASF_SUB_PAYLOAD_PKT_TYPE    0x00387158 /* ASF_SUB_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB2_ASF_PAYLOAD_PKT_TYPE        0x0038715c /* ASF_PAYLOAD_PKT_TYPE */
#define BCHP_XPT_PB2_PLAYBACK_PARSER_BAND_ID     0x00387160 /* PLAYBACK_PARSER_BAND_ID */

#endif /* #ifndef BCHP_XPT_PB2_H__ */

/* End of File */