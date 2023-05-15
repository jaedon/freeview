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
 * $brcm_Workfile: bchp_sdsrc_0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/22/11 6:16p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 22 15:27:15 2011
 *                 MD5 Checksum         4c7f7c173e488659edce1ceafec7f9f4
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/a0/bchp_sdsrc_0.h $
 * 
 * Hydra_Software_Devel/2   6/22/11 6:16p pntruong
 * SW7231-196: Synced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDSRC_0_H__
#define BCHP_SDSRC_0_H__

/***************************************************************************
 *SDSRC_0 - Sample Rate converter SDSRC_0
 ***************************************************************************/
#define BCHP_SDSRC_0_SRC_REV_ID                  0x00683500 /* Revision ID register */
#define BCHP_SDSRC_0_SRC_CONTROL                 0x00683504 /* Sample Rate Converter control register */
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1             0x00683508 /* Analog Scale Mult Value and offset Register for options 0,1 */
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3             0x0068350c /* Analog Scale Mult value and Offset Register for options 2,3 */

/***************************************************************************
 *SRC_REV_ID - Revision ID register
 ***************************************************************************/
/* SDSRC_0 :: SRC_REV_ID :: reserved0 [31:16] */
#define BCHP_SDSRC_0_SRC_REV_ID_reserved0_MASK                     0xffff0000
#define BCHP_SDSRC_0_SRC_REV_ID_reserved0_SHIFT                    16

/* SDSRC_0 :: SRC_REV_ID :: REVISION_ID [15:00] */
#define BCHP_SDSRC_0_SRC_REV_ID_REVISION_ID_MASK                   0x0000ffff
#define BCHP_SDSRC_0_SRC_REV_ID_REVISION_ID_SHIFT                  0
#define BCHP_SDSRC_0_SRC_REV_ID_REVISION_ID_DEFAULT                12288

/***************************************************************************
 *SRC_CONTROL - Sample Rate Converter control register
 ***************************************************************************/
/* SDSRC_0 :: SRC_CONTROL :: reserved0 [31:16] */
#define BCHP_SDSRC_0_SRC_CONTROL_reserved0_MASK                    0xffff0000
#define BCHP_SDSRC_0_SRC_CONTROL_reserved0_SHIFT                   16

/* SDSRC_0 :: SRC_CONTROL :: reserved_for_eco1 [15:09] */
#define BCHP_SDSRC_0_SRC_CONTROL_reserved_for_eco1_MASK            0x0000fe00
#define BCHP_SDSRC_0_SRC_CONTROL_reserved_for_eco1_SHIFT           9
#define BCHP_SDSRC_0_SRC_CONTROL_reserved_for_eco1_DEFAULT         0

/* SDSRC_0 :: SRC_CONTROL :: HSYNC [08:07] */
#define BCHP_SDSRC_0_SRC_CONTROL_HSYNC_MASK                        0x00000180
#define BCHP_SDSRC_0_SRC_CONTROL_HSYNC_SHIFT                       7
#define BCHP_SDSRC_0_SRC_CONTROL_HSYNC_Minus_6dB                   0
#define BCHP_SDSRC_0_SRC_CONTROL_HSYNC_Minus_11dB                  1
#define BCHP_SDSRC_0_SRC_CONTROL_HSYNC_Minus_18dB                  2

/* SDSRC_0 :: SRC_CONTROL :: H_CLAMP [06:06] */
#define BCHP_SDSRC_0_SRC_CONTROL_H_CLAMP_MASK                      0x00000040
#define BCHP_SDSRC_0_SRC_CONTROL_H_CLAMP_SHIFT                     6
#define BCHP_SDSRC_0_SRC_CONTROL_H_CLAMP_DEFAULT                   0
#define BCHP_SDSRC_0_SRC_CONTROL_H_CLAMP_UNSIGNED                  0
#define BCHP_SDSRC_0_SRC_CONTROL_H_CLAMP_SIGNED                    1

/* SDSRC_0 :: SRC_CONTROL :: CH12_CLAMP [05:05] */
#define BCHP_SDSRC_0_SRC_CONTROL_CH12_CLAMP_MASK                   0x00000020
#define BCHP_SDSRC_0_SRC_CONTROL_CH12_CLAMP_SHIFT                  5
#define BCHP_SDSRC_0_SRC_CONTROL_CH12_CLAMP_DEFAULT                1
#define BCHP_SDSRC_0_SRC_CONTROL_CH12_CLAMP_UNSIGNED               0
#define BCHP_SDSRC_0_SRC_CONTROL_CH12_CLAMP_SIGNED                 1

/* SDSRC_0 :: SRC_CONTROL :: CH0_CLAMP [04:04] */
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_CLAMP_MASK                    0x00000010
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_CLAMP_SHIFT                   4
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_CLAMP_DEFAULT                 0
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_CLAMP_UNSIGNED                0
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_CLAMP_SIGNED                  1

/* SDSRC_0 :: SRC_CONTROL :: CH1_2 [03:02] */
#define BCHP_SDSRC_0_SRC_CONTROL_CH1_2_MASK                        0x0000000c
#define BCHP_SDSRC_0_SRC_CONTROL_CH1_2_SHIFT                       2
#define BCHP_SDSRC_0_SRC_CONTROL_CH1_2_DEFAULT                     2
#define BCHP_SDSRC_0_SRC_CONTROL_CH1_2_Minus_6dB                   0
#define BCHP_SDSRC_0_SRC_CONTROL_CH1_2_Minus_11dB                  1
#define BCHP_SDSRC_0_SRC_CONTROL_CH1_2_Minus_18dB                  2

/* SDSRC_0 :: SRC_CONTROL :: CH0 [01:00] */
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_MASK                          0x00000003
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_SHIFT                         0
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_DEFAULT                       2
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_Minus_6dB                     0
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_Minus_11dB                    1
#define BCHP_SDSRC_0_SRC_CONTROL_CH0_Minus_18dB                    2

/***************************************************************************
 *SRC_ANA_SCL_0_1 - Analog Scale Mult Value and offset Register for options 0,1
 ***************************************************************************/
/* SDSRC_0 :: SRC_ANA_SCL_0_1 :: reserved0 [31:26] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_reserved0_MASK                0xfc000000
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_reserved0_SHIFT               26

/* SDSRC_0 :: SRC_ANA_SCL_0_1 :: OFFSET_1 [25:22] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_OFFSET_1_MASK                 0x03c00000
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_OFFSET_1_SHIFT                22
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_OFFSET_1_DEFAULT              0

/* SDSRC_0 :: SRC_ANA_SCL_0_1 :: VALUE_1 [21:13] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_VALUE_1_MASK                  0x003fe000
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_VALUE_1_SHIFT                 13
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_VALUE_1_DEFAULT               256

/* SDSRC_0 :: SRC_ANA_SCL_0_1 :: OFFSET_0 [12:09] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_OFFSET_0_MASK                 0x00001e00
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_OFFSET_0_SHIFT                9
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_OFFSET_0_DEFAULT              0

/* SDSRC_0 :: SRC_ANA_SCL_0_1 :: VALUE_0 [08:00] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_VALUE_0_MASK                  0x000001ff
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_VALUE_0_SHIFT                 0
#define BCHP_SDSRC_0_SRC_ANA_SCL_0_1_VALUE_0_DEFAULT               256

/***************************************************************************
 *SRC_ANA_SCL_2_3 - Analog Scale Mult value and Offset Register for options 2,3
 ***************************************************************************/
/* SDSRC_0 :: SRC_ANA_SCL_2_3 :: reserved0 [31:26] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_reserved0_MASK                0xfc000000
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_reserved0_SHIFT               26

/* SDSRC_0 :: SRC_ANA_SCL_2_3 :: OFFSET_3 [25:22] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_OFFSET_3_MASK                 0x03c00000
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_OFFSET_3_SHIFT                22
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_OFFSET_3_DEFAULT              0

/* SDSRC_0 :: SRC_ANA_SCL_2_3 :: VALUE_3 [21:13] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_VALUE_3_MASK                  0x003fe000
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_VALUE_3_SHIFT                 13
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_VALUE_3_DEFAULT               256

/* SDSRC_0 :: SRC_ANA_SCL_2_3 :: OFFSET_2 [12:09] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_OFFSET_2_MASK                 0x00001e00
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_OFFSET_2_SHIFT                9
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_OFFSET_2_DEFAULT              0

/* SDSRC_0 :: SRC_ANA_SCL_2_3 :: VALUE_2 [08:00] */
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_VALUE_2_MASK                  0x000001ff
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_VALUE_2_SHIFT                 0
#define BCHP_SDSRC_0_SRC_ANA_SCL_2_3_VALUE_2_DEFAULT               256

#endif /* #ifndef BCHP_SDSRC_0_H__ */

/* End of File */