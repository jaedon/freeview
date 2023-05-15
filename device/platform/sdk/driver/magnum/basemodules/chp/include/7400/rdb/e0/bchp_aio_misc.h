/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_aio_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 9:41a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 08:55:29 2008
 *                 MD5 Checksum         847dc12a9d71c4c68a648bbf19a883e3
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_aio_misc.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 9:41a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_AIO_MISC_H__
#define BCHP_AIO_MISC_H__

/***************************************************************************
 *AIO_MISC - Audio I/O Miscellaneous Registers
 ***************************************************************************/
#define BCHP_AIO_MISC_REVISION                   0x00620000 /* Audio I/O System Revision */
#define BCHP_AIO_MISC_RESET                      0x00620004 /* AIO Soft Reset */
#define BCHP_AIO_MISC_CTRL                       0x00620008 /* AIO Misc Controls */
#define BCHP_AIO_MISC_PWRDOWN                    0x0062000c /* Audio Powerdown Control */
#define BCHP_AIO_MISC_MBIST_TM                   0x00620010 /* MBIST TM Controls */

/***************************************************************************
 *REVISION - Audio I/O System Revision
 ***************************************************************************/
/* AIO_MISC :: REVISION :: reserved0 [31:16] */
#define BCHP_AIO_MISC_REVISION_reserved0_MASK                      0xffff0000
#define BCHP_AIO_MISC_REVISION_reserved0_SHIFT                     16

/* AIO_MISC :: REVISION :: MAJOR [15:08] */
#define BCHP_AIO_MISC_REVISION_MAJOR_MASK                          0x0000ff00
#define BCHP_AIO_MISC_REVISION_MAJOR_SHIFT                         8

/* AIO_MISC :: REVISION :: MINOR [07:00] */
#define BCHP_AIO_MISC_REVISION_MINOR_MASK                          0x000000ff
#define BCHP_AIO_MISC_REVISION_MINOR_SHIFT                         0

/***************************************************************************
 *RESET - AIO Soft Reset
 ***************************************************************************/
/* AIO_MISC :: RESET :: reserved0 [31:03] */
#define BCHP_AIO_MISC_RESET_reserved0_MASK                         0xfffffff8
#define BCHP_AIO_MISC_RESET_reserved0_SHIFT                        3

/* AIO_MISC :: RESET :: RESET_FMM_B [02:02] */
#define BCHP_AIO_MISC_RESET_RESET_FMM_B_MASK                       0x00000004
#define BCHP_AIO_MISC_RESET_RESET_FMM_B_SHIFT                      2
#define BCHP_AIO_MISC_RESET_RESET_FMM_B_Inactive                   0
#define BCHP_AIO_MISC_RESET_RESET_FMM_B_Reset                      1

/* AIO_MISC :: RESET :: reserved_for_eco1 [01:00] */
#define BCHP_AIO_MISC_RESET_reserved_for_eco1_MASK                 0x00000003
#define BCHP_AIO_MISC_RESET_reserved_for_eco1_SHIFT                0

/***************************************************************************
 *CTRL - AIO Misc Controls
 ***************************************************************************/
/* AIO_MISC :: CTRL :: reserved0 [31:08] */
#define BCHP_AIO_MISC_CTRL_reserved0_MASK                          0xffffff00
#define BCHP_AIO_MISC_CTRL_reserved0_SHIFT                         8

/* AIO_MISC :: CTRL :: reserved_for_eco1 [07:05] */
#define BCHP_AIO_MISC_CTRL_reserved_for_eco1_MASK                  0x000000e0
#define BCHP_AIO_MISC_CTRL_reserved_for_eco1_SHIFT                 5

/* AIO_MISC :: CTRL :: HIFI_DAC_CLK_54MHZ [04:04] */
#define BCHP_AIO_MISC_CTRL_HIFI_DAC_CLK_54MHZ_MASK                 0x00000010
#define BCHP_AIO_MISC_CTRL_HIFI_DAC_CLK_54MHZ_SHIFT                4
#define BCHP_AIO_MISC_CTRL_HIFI_DAC_CLK_54MHZ_CLK_27MHZ            0
#define BCHP_AIO_MISC_CTRL_HIFI_DAC_CLK_54MHZ_CLK_54MHZ            1

/* AIO_MISC :: CTRL :: reserved_for_eco2 [03:01] */
#define BCHP_AIO_MISC_CTRL_reserved_for_eco2_MASK                  0x0000000e
#define BCHP_AIO_MISC_CTRL_reserved_for_eco2_SHIFT                 1

/* AIO_MISC :: CTRL :: FMM_CMP_REQ_SEL [00:00] */
#define BCHP_AIO_MISC_CTRL_FMM_CMP_REQ_SEL_MASK                    0x00000001
#define BCHP_AIO_MISC_CTRL_FMM_CMP_REQ_SEL_SHIFT                   0
#define BCHP_AIO_MISC_CTRL_FMM_CMP_REQ_SEL_CMP0_REQ                0
#define BCHP_AIO_MISC_CTRL_FMM_CMP_REQ_SEL_CMP1_REQ                1

/***************************************************************************
 *PWRDOWN - Audio Powerdown Control
 ***************************************************************************/
/* AIO_MISC :: PWRDOWN :: reserved0 [31:02] */
#define BCHP_AIO_MISC_PWRDOWN_reserved0_MASK                       0xfffffffc
#define BCHP_AIO_MISC_PWRDOWN_reserved0_SHIFT                      2

/* AIO_MISC :: PWRDOWN :: DAC1 [01:01] */
#define BCHP_AIO_MISC_PWRDOWN_DAC1_MASK                            0x00000002
#define BCHP_AIO_MISC_PWRDOWN_DAC1_SHIFT                           1
#define BCHP_AIO_MISC_PWRDOWN_DAC1_Normal                          0
#define BCHP_AIO_MISC_PWRDOWN_DAC1_Powerdown                       1

/* AIO_MISC :: PWRDOWN :: DAC0 [00:00] */
#define BCHP_AIO_MISC_PWRDOWN_DAC0_MASK                            0x00000001
#define BCHP_AIO_MISC_PWRDOWN_DAC0_SHIFT                           0
#define BCHP_AIO_MISC_PWRDOWN_DAC0_Normal                          0
#define BCHP_AIO_MISC_PWRDOWN_DAC0_Powerdown                       1

/***************************************************************************
 *MBIST_TM - MBIST TM Controls
 ***************************************************************************/
/* AIO_MISC :: MBIST_TM :: TM7 [31:28] */
#define BCHP_AIO_MISC_MBIST_TM_TM7_MASK                            0xf0000000
#define BCHP_AIO_MISC_MBIST_TM_TM7_SHIFT                           28

/* AIO_MISC :: MBIST_TM :: TM6 [27:24] */
#define BCHP_AIO_MISC_MBIST_TM_TM6_MASK                            0x0f000000
#define BCHP_AIO_MISC_MBIST_TM_TM6_SHIFT                           24

/* AIO_MISC :: MBIST_TM :: TM5 [23:20] */
#define BCHP_AIO_MISC_MBIST_TM_TM5_MASK                            0x00f00000
#define BCHP_AIO_MISC_MBIST_TM_TM5_SHIFT                           20

/* AIO_MISC :: MBIST_TM :: TM4 [19:16] */
#define BCHP_AIO_MISC_MBIST_TM_TM4_MASK                            0x000f0000
#define BCHP_AIO_MISC_MBIST_TM_TM4_SHIFT                           16

/* AIO_MISC :: MBIST_TM :: TM3 [15:12] */
#define BCHP_AIO_MISC_MBIST_TM_TM3_MASK                            0x0000f000
#define BCHP_AIO_MISC_MBIST_TM_TM3_SHIFT                           12

/* AIO_MISC :: MBIST_TM :: TM2 [11:08] */
#define BCHP_AIO_MISC_MBIST_TM_TM2_MASK                            0x00000f00
#define BCHP_AIO_MISC_MBIST_TM_TM2_SHIFT                           8

/* AIO_MISC :: MBIST_TM :: TM1 [07:04] */
#define BCHP_AIO_MISC_MBIST_TM_TM1_MASK                            0x000000f0
#define BCHP_AIO_MISC_MBIST_TM_TM1_SHIFT                           4

/* AIO_MISC :: MBIST_TM :: TM0 [03:00] */
#define BCHP_AIO_MISC_MBIST_TM_TM0_MASK                            0x0000000f
#define BCHP_AIO_MISC_MBIST_TM_TM0_SHIFT                           0

#endif /* #ifndef BCHP_AIO_MISC_H__ */

/* End of File */