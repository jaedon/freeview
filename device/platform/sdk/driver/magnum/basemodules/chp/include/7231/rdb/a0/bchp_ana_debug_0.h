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
 * $brcm_Workfile: bchp_ana_debug_0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/22/11 5:41p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 22 15:30:10 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/a0/bchp_ana_debug_0.h $
 * 
 * Hydra_Software_Devel/2   6/22/11 5:41p pntruong
 * SW7231-196: Synced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_ANA_DEBUG_0_H__
#define BCHP_ANA_DEBUG_0_H__

/***************************************************************************
 *ANA_DEBUG_0 - Debug block ANA_DEBUG_0
 ***************************************************************************/
#define BCHP_ANA_DEBUG_0_DEBUG_REV_ID            0x00683700 /* Revision ID register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG            0x00683704 /* Debug Config Register */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR             0x00683708 /* Debug Errors Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK        0x0068370c /* Debug Errors Mask Register */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR       0x00683710 /* Debug Errors Clear Register	(Write Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START 0x00683714 /* Debug Window Start-point Register */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP  0x00683718 /* Debug Window Stop-point Register */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1 0x0068371c /* Debug Statistics-1 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2 0x00683720 /* Debug Statistics-2 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3 0x00683724 /* Debug Statistics-3 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4 0x00683728 /* Debug Statistics-4 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5 0x0068372c /* Debug Statistics-5 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6 0x00683730 /* Debug Statistics-6 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7 0x00683734 /* Debug Statistics-7 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8 0x00683738 /* Debug Statistics-8 Register	(Read Only) */
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF        0x0068373c /* Debug BVB Horizontal Size Reference Register */
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF    0x00683740 /* Debug BVB Vertical Top(Field_0/Frame) Size Reference Register */
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF    0x00683744 /* Debug BVB Vertical Bottom(Field_1) Size Reference Register */

/***************************************************************************
 *DEBUG_REV_ID - Revision ID register	(Read Only)
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_REV_ID :: reserved0 [31:16] */
#define BCHP_ANA_DEBUG_0_DEBUG_REV_ID_reserved0_MASK               0xffff0000
#define BCHP_ANA_DEBUG_0_DEBUG_REV_ID_reserved0_SHIFT              16

/* ANA_DEBUG_0 :: DEBUG_REV_ID :: REVISION_ID [15:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_REV_ID_REVISION_ID_MASK             0x0000ffff
#define BCHP_ANA_DEBUG_0_DEBUG_REV_ID_REVISION_ID_SHIFT            0
#define BCHP_ANA_DEBUG_0_DEBUG_REV_ID_REVISION_ID_DEFAULT          12288

/***************************************************************************
 *DEBUG_CONFIG - Debug Config Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_CONFIG :: reserved0 [31:20] */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_reserved0_MASK               0xfff00000
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_reserved0_SHIFT              20

/* ANA_DEBUG_0 :: DEBUG_CONFIG :: STATUS_SELECT [19:16] */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_STATUS_SELECT_MASK           0x000f0000
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_STATUS_SELECT_SHIFT          16
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_STATUS_SELECT_DEFAULT        0

/* ANA_DEBUG_0 :: DEBUG_CONFIG :: EVENT_SELECT [15:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_EVENT_SELECT_MASK            0x0000f000
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_EVENT_SELECT_SHIFT           12
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_EVENT_SELECT_DEFAULT         0

/* ANA_DEBUG_0 :: DEBUG_CONFIG :: PATH_SELECT [11:02] */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_PATH_SELECT_MASK             0x00000ffc
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_PATH_SELECT_SHIFT            2
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_PATH_SELECT_DEFAULT          0

/* ANA_DEBUG_0 :: DEBUG_CONFIG :: MODE [01:01] */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_MODE_MASK                    0x00000002
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_MODE_SHIFT                   1
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_MODE_DEFAULT                 0
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_MODE_DIAG_MODE               0
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_MODE_DEBUG_MODE              1

/* ANA_DEBUG_0 :: DEBUG_CONFIG :: LOGGING [00:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_LOGGING_MASK                 0x00000001
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_LOGGING_SHIFT                0
#define BCHP_ANA_DEBUG_0_DEBUG_CONFIG_LOGGING_DEFAULT              0

/***************************************************************************
 *DEBUG_ERROR - Debug Errors Register	(Read Only)
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_ERROR :: reserved0 [31:16] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_reserved0_MASK                0xffff0000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_reserved0_SHIFT               16

/* ANA_DEBUG_0 :: DEBUG_ERROR :: ERROR_HAPPENED [15:15] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_ERROR_HAPPENED_MASK           0x00008000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_ERROR_HAPPENED_SHIFT          15

/* ANA_DEBUG_0 :: DEBUG_ERROR :: VBI_PROTOCOL_MISMATCH [14:14] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_VBI_PROTOCOL_MISMATCH_MASK    0x00004000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_VBI_PROTOCOL_MISMATCH_SHIFT   14

/* ANA_DEBUG_0 :: DEBUG_ERROR :: BVB_HANG [13:13] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_BVB_HANG_MASK                 0x00002000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_BVB_HANG_SHIFT                13

/* ANA_DEBUG_0 :: DEBUG_ERROR :: VF_FIFO_OVERFLOW [12:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_VF_FIFO_OVERFLOW_MASK         0x00001000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_VF_FIFO_OVERFLOW_SHIFT        12

/* ANA_DEBUG_0 :: DEBUG_ERROR :: VF_FIFO_UNDERFLOW [11:11] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_VF_FIFO_UNDERFLOW_MASK        0x00000800
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_VF_FIFO_UNDERFLOW_SHIFT       11

/* ANA_DEBUG_0 :: DEBUG_ERROR :: SYNC_LOST_SLAVE [10:10] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SYNC_LOST_SLAVE_MASK          0x00000400
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SYNC_LOST_SLAVE_SHIFT         10

/* ANA_DEBUG_0 :: DEBUG_ERROR :: DRAIN_FIELD_1 [09:09] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_DRAIN_FIELD_1_MASK            0x00000200
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_DRAIN_FIELD_1_SHIFT           9

/* ANA_DEBUG_0 :: DEBUG_ERROR :: DRAIN_FIELD_0_FRAME [08:08] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_DRAIN_FIELD_0_FRAME_MASK      0x00000100
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_DRAIN_FIELD_0_FRAME_SHIFT     8

/* ANA_DEBUG_0 :: DEBUG_ERROR :: LONG_FIELD_1 [07:07] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_FIELD_1_MASK             0x00000080
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_FIELD_1_SHIFT            7

/* ANA_DEBUG_0 :: DEBUG_ERROR :: LONG_FIELD_0_FRAME [06:06] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_FIELD_0_FRAME_MASK       0x00000040
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_FIELD_0_FRAME_SHIFT      6

/* ANA_DEBUG_0 :: DEBUG_ERROR :: SHORT_FIELD_1 [05:05] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_FIELD_1_MASK            0x00000020
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_FIELD_1_SHIFT           5

/* ANA_DEBUG_0 :: DEBUG_ERROR :: SHORT_FIELD_0_FRAME [04:04] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_FIELD_0_FRAME_MASK      0x00000010
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_FIELD_0_FRAME_SHIFT     4

/* ANA_DEBUG_0 :: DEBUG_ERROR :: LONG_LINE_ACTIVE [03:03] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_LINE_ACTIVE_MASK         0x00000008
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_LINE_ACTIVE_SHIFT        3

/* ANA_DEBUG_0 :: DEBUG_ERROR :: LONG_LINE_FULL [02:02] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_LINE_FULL_MASK           0x00000004
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_LONG_LINE_FULL_SHIFT          2

/* ANA_DEBUG_0 :: DEBUG_ERROR :: SHORT_LINE_ACTIVE [01:01] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_LINE_ACTIVE_MASK        0x00000002
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_LINE_ACTIVE_SHIFT       1

/* ANA_DEBUG_0 :: DEBUG_ERROR :: SHORT_LINE_FULL [00:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_LINE_FULL_MASK          0x00000001
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_SHORT_LINE_FULL_SHIFT         0

/***************************************************************************
 *DEBUG_ERROR_MASK - Debug Errors Mask Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: reserved0 [31:16] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_reserved0_MASK           0xffff0000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_reserved0_SHIFT          16

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: ERROR_HAPPENED [15:15] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_ERROR_HAPPENED_MASK      0x00008000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_ERROR_HAPPENED_SHIFT     15
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_ERROR_HAPPENED_DEFAULT   0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: VBI_PROTOCOL_MISMATCH [14:14] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VBI_PROTOCOL_MISMATCH_MASK 0x00004000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VBI_PROTOCOL_MISMATCH_SHIFT 14
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VBI_PROTOCOL_MISMATCH_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: BVB_HANG [13:13] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_BVB_HANG_MASK            0x00002000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_BVB_HANG_SHIFT           13
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_BVB_HANG_DEFAULT         0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: VF_FIFO_OVERFLOW [12:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VF_FIFO_OVERFLOW_MASK    0x00001000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VF_FIFO_OVERFLOW_SHIFT   12
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VF_FIFO_OVERFLOW_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: VF_FIFO_UNDERFLOW [11:11] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VF_FIFO_UNDERFLOW_MASK   0x00000800
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VF_FIFO_UNDERFLOW_SHIFT  11
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_VF_FIFO_UNDERFLOW_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: SYNC_LOST_SLAVE [10:10] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SYNC_LOST_SLAVE_MASK     0x00000400
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SYNC_LOST_SLAVE_SHIFT    10
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SYNC_LOST_SLAVE_DEFAULT  0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: DRAIN_FIELD_1 [09:09] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_DRAIN_FIELD_1_MASK       0x00000200
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_DRAIN_FIELD_1_SHIFT      9
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_DRAIN_FIELD_1_DEFAULT    0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: DRAIN_FIELD_0_FRAME [08:08] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_DRAIN_FIELD_0_FRAME_MASK 0x00000100
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_DRAIN_FIELD_0_FRAME_SHIFT 8
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_DRAIN_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: LONG_FIELD_1 [07:07] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_FIELD_1_MASK        0x00000080
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_FIELD_1_SHIFT       7
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_FIELD_1_DEFAULT     0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: LONG_FIELD_0_FRAME [06:06] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_FIELD_0_FRAME_MASK  0x00000040
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_FIELD_0_FRAME_SHIFT 6
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: SHORT_FIELD_1 [05:05] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_FIELD_1_MASK       0x00000020
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_FIELD_1_SHIFT      5
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_FIELD_1_DEFAULT    0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: SHORT_FIELD_0_FRAME [04:04] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_FIELD_0_FRAME_MASK 0x00000010
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_FIELD_0_FRAME_SHIFT 4
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: LONG_LINE_ACTIVE [03:03] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_LINE_ACTIVE_MASK    0x00000008
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_LINE_ACTIVE_SHIFT   3
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_LINE_ACTIVE_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: LONG_LINE_FULL [02:02] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_LINE_FULL_MASK      0x00000004
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_LINE_FULL_SHIFT     2
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_LONG_LINE_FULL_DEFAULT   0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: SHORT_LINE_ACTIVE [01:01] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_LINE_ACTIVE_MASK   0x00000002
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_LINE_ACTIVE_SHIFT  1
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_LINE_ACTIVE_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_MASK :: SHORT_LINE_FULL [00:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_LINE_FULL_MASK     0x00000001
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_LINE_FULL_SHIFT    0
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_MASK_SHORT_LINE_FULL_DEFAULT  0

/***************************************************************************
 *DEBUG_ERROR_CLEAR - Debug Errors Clear Register	(Write Only)
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: reserved0 [31:16] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_reserved0_MASK          0xffff0000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_reserved0_SHIFT         16

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: ERROR_HAPPENED [15:15] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_ERROR_HAPPENED_MASK     0x00008000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_ERROR_HAPPENED_SHIFT    15
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_ERROR_HAPPENED_DEFAULT  0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: VBI_PROTOCOL_MISMATCH [14:14] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VBI_PROTOCOL_MISMATCH_MASK 0x00004000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VBI_PROTOCOL_MISMATCH_SHIFT 14
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VBI_PROTOCOL_MISMATCH_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: BVB_HANG [13:13] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_BVB_HANG_MASK           0x00002000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_BVB_HANG_SHIFT          13
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_BVB_HANG_DEFAULT        0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: VF_FIFO_OVERFLOW [12:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VF_FIFO_OVERFLOW_MASK   0x00001000
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VF_FIFO_OVERFLOW_SHIFT  12
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VF_FIFO_OVERFLOW_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: VF_FIFO_UNDERFLOW [11:11] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VF_FIFO_UNDERFLOW_MASK  0x00000800
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VF_FIFO_UNDERFLOW_SHIFT 11
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_VF_FIFO_UNDERFLOW_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: SYNC_LOST_SLAVE [10:10] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SYNC_LOST_SLAVE_MASK    0x00000400
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SYNC_LOST_SLAVE_SHIFT   10
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SYNC_LOST_SLAVE_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: DRAIN_FIELD_1 [09:09] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_DRAIN_FIELD_1_MASK      0x00000200
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_DRAIN_FIELD_1_SHIFT     9
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_DRAIN_FIELD_1_DEFAULT   0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: DRAIN_FIELD_0_FRAME [08:08] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_DRAIN_FIELD_0_FRAME_MASK 0x00000100
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_DRAIN_FIELD_0_FRAME_SHIFT 8
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_DRAIN_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: LONG_FIELD_1 [07:07] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_FIELD_1_MASK       0x00000080
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_FIELD_1_SHIFT      7
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_FIELD_1_DEFAULT    0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: LONG_FIELD_0_FRAME [06:06] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_FIELD_0_FRAME_MASK 0x00000040
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_FIELD_0_FRAME_SHIFT 6
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: SHORT_FIELD_1 [05:05] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_FIELD_1_MASK      0x00000020
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_FIELD_1_SHIFT     5
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_FIELD_1_DEFAULT   0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: SHORT_FIELD_0_FRAME [04:04] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_FIELD_0_FRAME_MASK 0x00000010
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_FIELD_0_FRAME_SHIFT 4
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: LONG_LINE_ACTIVE [03:03] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_LINE_ACTIVE_MASK   0x00000008
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_LINE_ACTIVE_SHIFT  3
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_LINE_ACTIVE_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: LONG_LINE_FULL [02:02] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_LINE_FULL_MASK     0x00000004
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_LINE_FULL_SHIFT    2
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_LONG_LINE_FULL_DEFAULT  0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: SHORT_LINE_ACTIVE [01:01] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_LINE_ACTIVE_MASK  0x00000002
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_LINE_ACTIVE_SHIFT 1
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_LINE_ACTIVE_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_ERROR_CLEAR :: SHORT_LINE_FULL [00:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_LINE_FULL_MASK    0x00000001
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_LINE_FULL_SHIFT   0
#define BCHP_ANA_DEBUG_0_DEBUG_ERROR_CLEAR_SHORT_LINE_FULL_DEFAULT 0

/***************************************************************************
 *DEBUG_PIXEL_POINT_START - Debug Window Start-point Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_PIXEL_POINT_START :: PIXEL [31:18] */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_PIXEL_MASK        0xfffc0000
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_PIXEL_SHIFT       18
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_PIXEL_DEFAULT     0

/* ANA_DEBUG_0 :: DEBUG_PIXEL_POINT_START :: LINE [17:05] */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_LINE_MASK         0x0003ffe0
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_LINE_SHIFT        5
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_LINE_DEFAULT      0

/* ANA_DEBUG_0 :: DEBUG_PIXEL_POINT_START :: FRAME [04:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_FRAME_MASK        0x0000001f
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_FRAME_SHIFT       0
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_START_FRAME_DEFAULT     0

/***************************************************************************
 *DEBUG_PIXEL_POINT_STOP - Debug Window Stop-point Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_PIXEL_POINT_STOP :: PIXEL [31:18] */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_PIXEL_MASK         0xfffc0000
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_PIXEL_SHIFT        18
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_PIXEL_DEFAULT      0

/* ANA_DEBUG_0 :: DEBUG_PIXEL_POINT_STOP :: LINE [17:05] */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_LINE_MASK          0x0003ffe0
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_LINE_SHIFT         5
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_LINE_DEFAULT       0

/* ANA_DEBUG_0 :: DEBUG_PIXEL_POINT_STOP :: FRAME [04:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_FRAME_MASK         0x0000001f
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_FRAME_SHIFT        0
#define BCHP_ANA_DEBUG_0_DEBUG_PIXEL_POINT_STOP_FRAME_DEFAULT      0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_1 - Debug Statistics-1 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_1 :: DEBUG_MODE :: DEBUG_REG_1 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DEBUG_MODE_DEBUG_REG_1_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DEBUG_MODE_DEBUG_REG_1_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DEBUG_MODE_DEBUG_REG_1_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_1 :: DIAG_MODE :: reserved0 [31:29] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_reserved0_MASK 0xe0000000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_reserved0_SHIFT 29

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_1 :: DIAG_MODE :: H_FP [28:17] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_FP_MASK 0x1ffe0000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_FP_SHIFT 17
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_FP_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_1 :: DIAG_MODE :: H_BP [16:08] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_BP_MASK 0x0001ff00
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_BP_SHIFT 8
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_BP_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_1 :: DIAG_MODE :: H_SYNC [07:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_SYNC_MASK 0x000000ff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_SYNC_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_1_DIAG_MODE_H_SYNC_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_2 - Debug Statistics-2 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_2 :: DEBUG_MODE :: DEBUG_REG_2 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DEBUG_MODE_DEBUG_REG_2_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DEBUG_MODE_DEBUG_REG_2_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DEBUG_MODE_DEBUG_REG_2_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_2 :: DIAG_MODE :: reserved0 [31:24] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_reserved0_MASK 0xff000000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_reserved0_SHIFT 24

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_2 :: DIAG_MODE :: H_FULL [23:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_H_FULL_MASK 0x00fff000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_H_FULL_SHIFT 12
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_H_FULL_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_2 :: DIAG_MODE :: H_ACTIVE [11:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_H_ACTIVE_MASK 0x00000fff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_H_ACTIVE_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_2_DIAG_MODE_H_ACTIVE_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_3 - Debug Statistics-3 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_3 :: DEBUG_MODE :: DEBUG_REG_3 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DEBUG_MODE_DEBUG_REG_3_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DEBUG_MODE_DEBUG_REG_3_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DEBUG_MODE_DEBUG_REG_3_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_3 :: DIAG_MODE :: reserved0 [31:25] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_reserved0_MASK 0xfe000000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_reserved0_SHIFT 25

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_3 :: DIAG_MODE :: V_EQ_FIELD_0_FRAME [24:17] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_EQ_FIELD_0_FRAME_MASK 0x01fe0000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_EQ_FIELD_0_FRAME_SHIFT 17
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_EQ_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_3 :: DIAG_MODE :: V_FP_FIELD_0_FRAME [16:11] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_FP_FIELD_0_FRAME_MASK 0x0001f800
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_FP_FIELD_0_FRAME_SHIFT 11
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_FP_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_3 :: DIAG_MODE :: V_BP_FIELD_0_FRAME [10:04] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_BP_FIELD_0_FRAME_MASK 0x000007f0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_BP_FIELD_0_FRAME_SHIFT 4
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_BP_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_3 :: DIAG_MODE :: V_SYNC_FIELD_0_FRAME [03:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_SYNC_FIELD_0_FRAME_MASK 0x0000000f
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_SYNC_FIELD_0_FRAME_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_3_DIAG_MODE_V_SYNC_FIELD_0_FRAME_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_4 - Debug Statistics-4 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_4 :: DEBUG_MODE :: DEBUG_REG_4 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DEBUG_MODE_DEBUG_REG_4_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DEBUG_MODE_DEBUG_REG_4_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DEBUG_MODE_DEBUG_REG_4_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_4 :: DIAG_MODE :: reserved0 [31:24] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_reserved0_MASK 0xff000000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_reserved0_SHIFT 24

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_4 :: DIAG_MODE :: V_FULL_FIELD_0_FRAME [23:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_V_FULL_FIELD_0_FRAME_MASK 0x00fff000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_V_FULL_FIELD_0_FRAME_SHIFT 12
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_V_FULL_FIELD_0_FRAME_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_4 :: DIAG_MODE :: V_ACTIVE_FIELD_0_FRAME [11:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_V_ACTIVE_FIELD_0_FRAME_MASK 0x00000fff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_V_ACTIVE_FIELD_0_FRAME_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_4_DIAG_MODE_V_ACTIVE_FIELD_0_FRAME_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_5 - Debug Statistics-5 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_5 :: DEBUG_MODE :: DEBUG_REG_5 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DEBUG_MODE_DEBUG_REG_5_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DEBUG_MODE_DEBUG_REG_5_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DEBUG_MODE_DEBUG_REG_5_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_5 :: DIAG_MODE :: reserved0 [31:25] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_reserved0_MASK 0xfe000000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_reserved0_SHIFT 25

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_5 :: DIAG_MODE :: V_EQ_FIELD_1 [24:17] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_EQ_FIELD_1_MASK 0x01fe0000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_EQ_FIELD_1_SHIFT 17
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_EQ_FIELD_1_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_5 :: DIAG_MODE :: V_FP_FIELD_1 [16:11] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_FP_FIELD_1_MASK 0x0001f800
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_FP_FIELD_1_SHIFT 11
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_FP_FIELD_1_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_5 :: DIAG_MODE :: V_BP_FIELD_1 [10:04] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_BP_FIELD_1_MASK 0x000007f0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_BP_FIELD_1_SHIFT 4
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_BP_FIELD_1_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_5 :: DIAG_MODE :: V_SYNC_FIELD_1 [03:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_SYNC_FIELD_1_MASK 0x0000000f
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_SYNC_FIELD_1_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_5_DIAG_MODE_V_SYNC_FIELD_1_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_6 - Debug Statistics-6 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_6 :: DEBUG_MODE :: DEBUG_REG_6 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DEBUG_MODE_DEBUG_REG_6_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DEBUG_MODE_DEBUG_REG_6_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DEBUG_MODE_DEBUG_REG_6_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_6 :: DIAG_MODE :: reserved0 [31:24] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_reserved0_MASK 0xff000000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_reserved0_SHIFT 24

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_6 :: DIAG_MODE :: V_FULL_FIELD_1 [23:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_V_FULL_FIELD_1_MASK 0x00fff000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_V_FULL_FIELD_1_SHIFT 12
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_V_FULL_FIELD_1_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_6 :: DIAG_MODE :: V_ACTIVE_FIELD_1 [11:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_V_ACTIVE_FIELD_1_MASK 0x00000fff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_V_ACTIVE_FIELD_1_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_6_DIAG_MODE_V_ACTIVE_FIELD_1_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_7 - Debug Statistics-7 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_7 :: DEBUG_MODE :: DEBUG_REG_7 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DEBUG_MODE_DEBUG_REG_7_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DEBUG_MODE_DEBUG_REG_7_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DEBUG_MODE_DEBUG_REG_7_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_7 :: DIAG_MODE :: reserved0 [31:16] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_reserved0_MASK 0xffff0000
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_reserved0_SHIFT 16

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_7 :: DIAG_MODE :: COLOUR_WIDTH [15:08] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_COLOUR_WIDTH_MASK 0x0000ff00
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_COLOUR_WIDTH_SHIFT 8
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_COLOUR_WIDTH_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_7 :: DIAG_MODE :: COLOUR_START [07:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_COLOUR_START_MASK 0x000000ff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_COLOUR_START_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_7_DIAG_MODE_COLOUR_START_DEFAULT 0

/***************************************************************************
 *DEBUG_STATUS_STATISTICS_8 - Debug Statistics-8 Register	(Read Only)
 ***************************************************************************/
/* union - case DEBUG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_8 :: DEBUG_MODE :: DEBUG_REG_8 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8_DEBUG_MODE_DEBUG_REG_8_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8_DEBUG_MODE_DEBUG_REG_8_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8_DEBUG_MODE_DEBUG_REG_8_DEFAULT 0

/* union - case DIAG_MODE [31:00] */
/* ANA_DEBUG_0 :: DEBUG_STATUS_STATISTICS_8 :: DIAG_MODE :: DIAG_FIELD_8 [31:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8_DIAG_MODE_DIAG_FIELD_8_MASK 0xffffffff
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8_DIAG_MODE_DIAG_FIELD_8_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_STATUS_STATISTICS_8_DIAG_MODE_DIAG_FIELD_8_DEFAULT 0

/***************************************************************************
 *DEBUG_H_SIZE_REF - Debug BVB Horizontal Size Reference Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_H_SIZE_REF :: reserved0 [31:24] */
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_reserved0_MASK           0xff000000
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_reserved0_SHIFT          24

/* ANA_DEBUG_0 :: DEBUG_H_SIZE_REF :: H_SIZE_REF_FULL [23:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_H_SIZE_REF_FULL_MASK     0x00fff000
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_H_SIZE_REF_FULL_SHIFT    12
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_H_SIZE_REF_FULL_DEFAULT  0

/* ANA_DEBUG_0 :: DEBUG_H_SIZE_REF :: H_SIZE_REF_ACTIVE [11:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_H_SIZE_REF_ACTIVE_MASK   0x00000fff
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_H_SIZE_REF_ACTIVE_SHIFT  0
#define BCHP_ANA_DEBUG_0_DEBUG_H_SIZE_REF_H_SIZE_REF_ACTIVE_DEFAULT 0

/***************************************************************************
 *DEBUG_V_TOP_SIZE_REF - Debug BVB Vertical Top(Field_0/Frame) Size Reference Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_V_TOP_SIZE_REF :: reserved0 [31:24] */
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_reserved0_MASK       0xff000000
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_reserved0_SHIFT      24

/* ANA_DEBUG_0 :: DEBUG_V_TOP_SIZE_REF :: V_SIZE_REF_FULL [23:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_V_SIZE_REF_FULL_MASK 0x00fff000
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_V_SIZE_REF_FULL_SHIFT 12
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_V_SIZE_REF_FULL_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_V_TOP_SIZE_REF :: V_SIZE_REF_ACTIVE [11:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_V_SIZE_REF_ACTIVE_MASK 0x00000fff
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_V_SIZE_REF_ACTIVE_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_V_TOP_SIZE_REF_V_SIZE_REF_ACTIVE_DEFAULT 0

/***************************************************************************
 *DEBUG_V_BOT_SIZE_REF - Debug BVB Vertical Bottom(Field_1) Size Reference Register
 ***************************************************************************/
/* ANA_DEBUG_0 :: DEBUG_V_BOT_SIZE_REF :: reserved0 [31:24] */
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_reserved0_MASK       0xff000000
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_reserved0_SHIFT      24

/* ANA_DEBUG_0 :: DEBUG_V_BOT_SIZE_REF :: V_SIZE_REF_FULL [23:12] */
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_V_SIZE_REF_FULL_MASK 0x00fff000
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_V_SIZE_REF_FULL_SHIFT 12
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_V_SIZE_REF_FULL_DEFAULT 0

/* ANA_DEBUG_0 :: DEBUG_V_BOT_SIZE_REF :: V_SIZE_REF_ACTIVE [11:00] */
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_V_SIZE_REF_ACTIVE_MASK 0x00000fff
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_V_SIZE_REF_ACTIVE_SHIFT 0
#define BCHP_ANA_DEBUG_0_DEBUG_V_BOT_SIZE_REF_V_SIZE_REF_ACTIVE_DEFAULT 0

#endif /* #ifndef BCHP_ANA_DEBUG_0_H__ */

/* End of File */
