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
 * $brcm_Workfile: bchp_vf_2.h $
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
 * Date:           Generated on         Fri Jan 22 20:30:33 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_vf_2.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 10:11p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_VF_2_H__
#define BCHP_VF_2_H__

/***************************************************************************
 *VF_2 - Video Formatter VF_2
 ***************************************************************************/
#define BCHP_VF_2_VF_REV_ID                      0x00883400 /* Revision ID register */
#define BCHP_VF_2_FORMAT_ADDER                   0x00883408 /* Video formatter adder register */
#define BCHP_VF_2_MISC                           0x0088340c /* Miscellaneous register */
#define BCHP_VF_2_NEG_SYNC_VALUES                0x00883410 /* Negative Sync and Blank amplitude register */
#define BCHP_VF_2_POS_SYNC_VALUES                0x00883414 /* Positive Sync amplitude register */
#define BCHP_VF_2_SYNC_TRANS_0                   0x00883418 /* Sync pulse transition 0 register */
#define BCHP_VF_2_SYNC_TRANS_1                   0x0088341c /* Sync pulse transition 1 register */
#define BCHP_VF_2_CH0_TAP1                       0x00883420 /* Channel 0 band-limiting filter (taps 1  & 19) */
#define BCHP_VF_2_CH0_TAP2                       0x00883424 /* Channel 0 band-limiting filter (taps 2  & 18) */
#define BCHP_VF_2_CH0_TAP3                       0x00883428 /* Channel 0 band-limiting filter (taps 3  & 17) */
#define BCHP_VF_2_CH0_TAP4                       0x0088342c /* Channel 0 band-limiting filter (taps 4  & 16) */
#define BCHP_VF_2_CH0_TAP5                       0x00883430 /* Channel 0 band-limiting filter (taps 5  & 15) */
#define BCHP_VF_2_CH0_TAP6                       0x00883434 /* Channel 0 band-limiting filter (taps 6  & 14) */
#define BCHP_VF_2_CH0_TAP7                       0x00883438 /* Channel 0 band-limiting filter (taps 7  & 13) */
#define BCHP_VF_2_CH0_TAP8                       0x0088343c /* Channel 0 band-limiting filter (taps 8  & 12) */
#define BCHP_VF_2_CH0_TAP9                       0x00883440 /* Channel 0 band-limiting filter (taps 9  & 11) */
#define BCHP_VF_2_CH0_TAP10                      0x00883444 /* Channel 0 band-limiting filter (tap 10  ) */
#define BCHP_VF_2_CH1_TAP1                       0x00883448 /* Channel 1 band-limiting filter (taps 1  & 19) */
#define BCHP_VF_2_CH1_TAP2                       0x0088344c /* Channel 1 band-limiting filter (taps 2  & 18) */
#define BCHP_VF_2_CH1_TAP3                       0x00883450 /* Channel 1 band-limiting filter (taps 3  & 17) */
#define BCHP_VF_2_CH1_TAP4                       0x00883454 /* Channel 1 band-limiting filter (taps 4  & 16) */
#define BCHP_VF_2_CH1_TAP5                       0x00883458 /* Channel 1 band-limiting filter (taps 5  & 15) */
#define BCHP_VF_2_CH1_TAP6                       0x0088345c /* Channel 1 band-limiting filter (taps 6  & 14) */
#define BCHP_VF_2_CH1_TAP7                       0x00883460 /* Channel 1 band-limiting filter (taps 7  & 13) */
#define BCHP_VF_2_CH1_TAP8                       0x00883464 /* Channel 1 band-limiting filter (taps 8  & 12) */
#define BCHP_VF_2_CH1_TAP9                       0x00883468 /* Channel 1 band-limiting filter (taps 9  & 11) */
#define BCHP_VF_2_CH1_TAP10                      0x0088346c /* Channel 1 band-limiting filter (tap 10  ) */
#define BCHP_VF_2_CH2_TAP1                       0x00883470 /* Channel 2 band-limiting filter (taps 1  & 19) */
#define BCHP_VF_2_CH2_TAP2                       0x00883474 /* Channel 2 band-limiting filter (taps 2  & 18) */
#define BCHP_VF_2_CH2_TAP3                       0x00883478 /* Channel 2 band-limiting filter (taps 3  & 17) */
#define BCHP_VF_2_CH2_TAP4                       0x0088347c /* Channel 2 band-limiting filter (taps 4  & 16) */
#define BCHP_VF_2_CH2_TAP5                       0x00883480 /* Channel 2 band-limiting filter (taps 5  & 15) */
#define BCHP_VF_2_CH2_TAP6                       0x00883484 /* Channel 2 band-limiting filter (taps 6  & 14) */
#define BCHP_VF_2_CH2_TAP7                       0x00883488 /* Channel 2 band-limiting filter (taps 7  & 13) */
#define BCHP_VF_2_CH2_TAP8                       0x0088348c /* Channel 2 band-limiting filter (taps 8  & 12) */
#define BCHP_VF_2_CH2_TAP9                       0x00883490 /* Channel 2 band-limiting filter (taps 9  & 11) */
#define BCHP_VF_2_CH2_TAP10                      0x00883494 /* Channel 2 band-limiting filter (tap 10  ) */
#define BCHP_VF_2_BVB_STATUS                     0x00883498 /* VEC's BVB Status Register */
#define BCHP_VF_2_SHAPER                         0x0088349c /* VEC's Shaper Register */
#define BCHP_VF_2_HSYNC_AMP                      0x00883500 /* VEC's Hsync Amplitude Register */
#define BCHP_VF_2_VIDEOBUS_SA_CONFIG             0x00883504 /* VIDEO-BUS SA CONFIGURATION REGISTER */
#define BCHP_VF_2_VIDEOBUS_SA_CH0_STATUS         0x00883508 /* VIDEO-BUS CHANNEL0 SA  STATUS REGISTER */
#define BCHP_VF_2_VIDEOBUS_SA_CH1_STATUS         0x0088350c /* VIDEO-BUS CHANNEL1 SA  STATUS REGISTER */
#define BCHP_VF_2_VIDEOBUS_SA_CH2_STATUS         0x00883510 /* VIDEO-BUS CHANNEL2 SA  STATUS REGISTER */
#define BCHP_VF_2_CONTROLBUS_SA_CONFIG           0x00883514 /* CONTROL-BUS SA CONFIGURATION REGISTER */
#define BCHP_VF_2_CONTROLBUS_SA_STATUS           0x00883518 /* CONTROL-BUS SA STATUS REGISTER */
#define BCHP_VF_2_ENVELOPE_GENERATOR             0x0088351c /* ENVELOPE GENERATOR CONTROL REGISTER */
#define BCHP_VF_2_NEG_SYNC_AMPLITUDE_EXTN        0x00883520 /* Negative Sync amplitude extension register */
#define BCHP_VF_2_POS_SYNC_AMPLITUDE_EXTN        0x00883524 /* Positive Sync amplitude extension register */

#endif /* #ifndef BCHP_VF_2_H__ */

/* End of File */