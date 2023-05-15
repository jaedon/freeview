/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_vf_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 10:11p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:10:48 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_vf_1.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 10:11p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_VF_1_H__
#define BCHP_VF_1_H__

/***************************************************************************
 *VF_1 - Video Formatter VF_1
 ***************************************************************************/
#define BCHP_VF_1_VF_REV_ID                      0x00883200 /* Revision ID register */
#define BCHP_VF_1_FORMAT_ADDER                   0x00883208 /* Video formatter adder register */
#define BCHP_VF_1_MISC                           0x0088320c /* Miscellaneous register */
#define BCHP_VF_1_NEG_SYNC_VALUES                0x00883210 /* Negative Sync and Blank amplitude register */
#define BCHP_VF_1_POS_SYNC_VALUES                0x00883214 /* Positive Sync amplitude register */
#define BCHP_VF_1_SYNC_TRANS_0                   0x00883218 /* Sync pulse transition 0 register */
#define BCHP_VF_1_SYNC_TRANS_1                   0x0088321c /* Sync pulse transition 1 register */
#define BCHP_VF_1_CH0_TAP1                       0x00883220 /* Channel 0 band-limiting filter (taps 1  & 19) */
#define BCHP_VF_1_CH0_TAP2                       0x00883224 /* Channel 0 band-limiting filter (taps 2  & 18) */
#define BCHP_VF_1_CH0_TAP3                       0x00883228 /* Channel 0 band-limiting filter (taps 3  & 17) */
#define BCHP_VF_1_CH0_TAP4                       0x0088322c /* Channel 0 band-limiting filter (taps 4  & 16) */
#define BCHP_VF_1_CH0_TAP5                       0x00883230 /* Channel 0 band-limiting filter (taps 5  & 15) */
#define BCHP_VF_1_CH0_TAP6                       0x00883234 /* Channel 0 band-limiting filter (taps 6  & 14) */
#define BCHP_VF_1_CH0_TAP7                       0x00883238 /* Channel 0 band-limiting filter (taps 7  & 13) */
#define BCHP_VF_1_CH0_TAP8                       0x0088323c /* Channel 0 band-limiting filter (taps 8  & 12) */
#define BCHP_VF_1_CH0_TAP9                       0x00883240 /* Channel 0 band-limiting filter (taps 9  & 11) */
#define BCHP_VF_1_CH0_TAP10                      0x00883244 /* Channel 0 band-limiting filter (tap 10  ) */
#define BCHP_VF_1_CH1_TAP1                       0x00883248 /* Channel 1 band-limiting filter (taps 1  & 19) */
#define BCHP_VF_1_CH1_TAP2                       0x0088324c /* Channel 1 band-limiting filter (taps 2  & 18) */
#define BCHP_VF_1_CH1_TAP3                       0x00883250 /* Channel 1 band-limiting filter (taps 3  & 17) */
#define BCHP_VF_1_CH1_TAP4                       0x00883254 /* Channel 1 band-limiting filter (taps 4  & 16) */
#define BCHP_VF_1_CH1_TAP5                       0x00883258 /* Channel 1 band-limiting filter (taps 5  & 15) */
#define BCHP_VF_1_CH1_TAP6                       0x0088325c /* Channel 1 band-limiting filter (taps 6  & 14) */
#define BCHP_VF_1_CH1_TAP7                       0x00883260 /* Channel 1 band-limiting filter (taps 7  & 13) */
#define BCHP_VF_1_CH1_TAP8                       0x00883264 /* Channel 1 band-limiting filter (taps 8  & 12) */
#define BCHP_VF_1_CH1_TAP9                       0x00883268 /* Channel 1 band-limiting filter (taps 9  & 11) */
#define BCHP_VF_1_CH1_TAP10                      0x0088326c /* Channel 1 band-limiting filter (tap 10  ) */
#define BCHP_VF_1_CH2_TAP1                       0x00883270 /* Channel 2 band-limiting filter (taps 1  & 19) */
#define BCHP_VF_1_CH2_TAP2                       0x00883274 /* Channel 2 band-limiting filter (taps 2  & 18) */
#define BCHP_VF_1_CH2_TAP3                       0x00883278 /* Channel 2 band-limiting filter (taps 3  & 17) */
#define BCHP_VF_1_CH2_TAP4                       0x0088327c /* Channel 2 band-limiting filter (taps 4  & 16) */
#define BCHP_VF_1_CH2_TAP5                       0x00883280 /* Channel 2 band-limiting filter (taps 5  & 15) */
#define BCHP_VF_1_CH2_TAP6                       0x00883284 /* Channel 2 band-limiting filter (taps 6  & 14) */
#define BCHP_VF_1_CH2_TAP7                       0x00883288 /* Channel 2 band-limiting filter (taps 7  & 13) */
#define BCHP_VF_1_CH2_TAP8                       0x0088328c /* Channel 2 band-limiting filter (taps 8  & 12) */
#define BCHP_VF_1_CH2_TAP9                       0x00883290 /* Channel 2 band-limiting filter (taps 9  & 11) */
#define BCHP_VF_1_CH2_TAP10                      0x00883294 /* Channel 2 band-limiting filter (tap 10  ) */
#define BCHP_VF_1_BVB_STATUS                     0x00883298 /* VEC's BVB Status Register */
#define BCHP_VF_1_SHAPER                         0x0088329c /* VEC's Shaper Register */
#define BCHP_VF_1_HSYNC_AMP                      0x00883300 /* VEC's Hsync Amplitude Register */
#define BCHP_VF_1_VIDEOBUS_SA_CONFIG             0x00883304 /* VIDEO-BUS SA CONFIGURATION REGISTER */
#define BCHP_VF_1_VIDEOBUS_SA_CH0_STATUS         0x00883308 /* VIDEO-BUS CHANNEL0 SA  STATUS REGISTER */
#define BCHP_VF_1_VIDEOBUS_SA_CH1_STATUS         0x0088330c /* VIDEO-BUS CHANNEL1 SA  STATUS REGISTER */
#define BCHP_VF_1_VIDEOBUS_SA_CH2_STATUS         0x00883310 /* VIDEO-BUS CHANNEL2 SA  STATUS REGISTER */
#define BCHP_VF_1_CONTROLBUS_SA_CONFIG           0x00883314 /* CONTROL-BUS SA CONFIGURATION REGISTER */
#define BCHP_VF_1_CONTROLBUS_SA_STATUS           0x00883318 /* CONTROL-BUS SA STATUS REGISTER */
#define BCHP_VF_1_ENVELOPE_GENERATOR             0x0088331c /* ENVELOPE GENERATOR CONTROL REGISTER */
#define BCHP_VF_1_NEG_SYNC_AMPLITUDE_EXTN        0x00883320 /* Negative Sync amplitude extension register */
#define BCHP_VF_1_POS_SYNC_AMPLITUDE_EXTN        0x00883324 /* Positive Sync amplitude extension register */

#endif /* #ifndef BCHP_VF_1_H__ */

/* End of File */