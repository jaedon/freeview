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
 * $brcm_Workfile: bchp_dvi_dvf_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 8:18p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:28:09 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_dvi_dvf_0.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 8:18p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_DVI_DVF_0_H__
#define BCHP_DVI_DVF_0_H__

/***************************************************************************
 *DVI_DVF_0 - DVI Frontend Sync Insertion 0
 ***************************************************************************/
#define BCHP_DVI_DVF_0_DVF_REV_ID                0x00884a00 /* Revision ID register */
#define BCHP_DVI_DVF_0_DVF_CONFIG                0x00884a0c /* Configuration register */
#define BCHP_DVI_DVF_0_DVF_VALUES                0x00884a10 /* Misc. Value register */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS            0x00884a14 /* DVF's BVB Status Register */

/***************************************************************************
 *DVF_REV_ID - Revision ID register
 ***************************************************************************/
/* DVI_DVF_0 :: DVF_REV_ID :: reserved0 [31:16] */
#define BCHP_DVI_DVF_0_DVF_REV_ID_reserved0_MASK                   0xffff0000
#define BCHP_DVI_DVF_0_DVF_REV_ID_reserved0_SHIFT                  16

/* DVI_DVF_0 :: DVF_REV_ID :: REVISION_ID [15:00] */
#define BCHP_DVI_DVF_0_DVF_REV_ID_REVISION_ID_MASK                 0x0000ffff
#define BCHP_DVI_DVF_0_DVF_REV_ID_REVISION_ID_SHIFT                0

/***************************************************************************
 *DVF_CONFIG - Configuration register
 ***************************************************************************/
/* DVI_DVF_0 :: DVF_CONFIG :: reserved0 [31:16] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_reserved0_MASK                   0xffff0000
#define BCHP_DVI_DVF_0_DVF_CONFIG_reserved0_SHIFT                  16

/* DVI_DVF_0 :: DVF_CONFIG :: reserved_for_eco1 [15:14] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_reserved_for_eco1_MASK           0x0000c000
#define BCHP_DVI_DVF_0_DVF_CONFIG_reserved_for_eco1_SHIFT          14

/* DVI_DVF_0 :: DVF_CONFIG :: DOWNSAMPLE2X [13:13] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_MASK                0x00002000
#define BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_SHIFT               13
#define BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_OFF                 0
#define BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_ON                  1

/* DVI_DVF_0 :: DVF_CONFIG :: PASSTHROUGH_COUNT [12:03] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_PASSTHROUGH_COUNT_MASK           0x00001ff8
#define BCHP_DVI_DVF_0_DVF_CONFIG_PASSTHROUGH_COUNT_SHIFT          3

/* DVI_DVF_0 :: DVF_CONFIG :: VBI_PREFERRED [02:02] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_PREFERRED_MASK               0x00000004
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_PREFERRED_SHIFT              2
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_PREFERRED_OFF                0
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_PREFERRED_ON                 1

/* DVI_DVF_0 :: DVF_CONFIG :: VBI_ENABLE [01:01] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_ENABLE_MASK                  0x00000002
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_ENABLE_SHIFT                 1
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_ENABLE_OFF                   0
#define BCHP_DVI_DVF_0_DVF_CONFIG_VBI_ENABLE_ON                    1

/* DVI_DVF_0 :: DVF_CONFIG :: UPSAMPLE2X [00:00] */
#define BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_MASK                  0x00000001
#define BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_SHIFT                 0
#define BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_OFF                   0
#define BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_ON                    1

/***************************************************************************
 *DVF_VALUES - Misc. Value register
 ***************************************************************************/
/* DVI_DVF_0 :: DVF_VALUES :: CH0_VBI_OFFSET [31:24] */
#define BCHP_DVI_DVF_0_DVF_VALUES_CH0_VBI_OFFSET_MASK              0xff000000
#define BCHP_DVI_DVF_0_DVF_VALUES_CH0_VBI_OFFSET_SHIFT             24

/* DVI_DVF_0 :: DVF_VALUES :: CH2_BLANK [23:16] */
#define BCHP_DVI_DVF_0_DVF_VALUES_CH2_BLANK_MASK                   0x00ff0000
#define BCHP_DVI_DVF_0_DVF_VALUES_CH2_BLANK_SHIFT                  16

/* DVI_DVF_0 :: DVF_VALUES :: CH1_BLANK [15:08] */
#define BCHP_DVI_DVF_0_DVF_VALUES_CH1_BLANK_MASK                   0x0000ff00
#define BCHP_DVI_DVF_0_DVF_VALUES_CH1_BLANK_SHIFT                  8

/* DVI_DVF_0 :: DVF_VALUES :: CH0_BLANK [07:00] */
#define BCHP_DVI_DVF_0_DVF_VALUES_CH0_BLANK_MASK                   0x000000ff
#define BCHP_DVI_DVF_0_DVF_VALUES_CH0_BLANK_SHIFT                  0

/***************************************************************************
 *DVF_BVB_STATUS - DVF's BVB Status Register
 ***************************************************************************/
/* DVI_DVF_0 :: DVF_BVB_STATUS :: reserved0 [31:09] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_reserved0_MASK               0xfffffe00
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_reserved0_SHIFT              9

/* DVI_DVF_0 :: DVF_BVB_STATUS :: LATE_BF_SOURCE [08:08] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LATE_BF_SOURCE_MASK          0x00000100
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LATE_BF_SOURCE_SHIFT         8

/* DVI_DVF_0 :: DVF_BVB_STATUS :: LONG_BF_SOURCE [07:07] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LONG_BF_SOURCE_MASK          0x00000080
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LONG_BF_SOURCE_SHIFT         7

/* DVI_DVF_0 :: DVF_BVB_STATUS :: SHORT_BF_SOURCE [06:06] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_SHORT_BF_SOURCE_MASK         0x00000040
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_SHORT_BF_SOURCE_SHIFT        6

/* DVI_DVF_0 :: DVF_BVB_STATUS :: LATE_TF_SOURCE [05:05] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LATE_TF_SOURCE_MASK          0x00000020
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LATE_TF_SOURCE_SHIFT         5

/* DVI_DVF_0 :: DVF_BVB_STATUS :: LONG_TF_SOURCE [04:04] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LONG_TF_SOURCE_MASK          0x00000010
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LONG_TF_SOURCE_SHIFT         4

/* DVI_DVF_0 :: DVF_BVB_STATUS :: SHORT_TF_SOURCE [03:03] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_SHORT_TF_SOURCE_MASK         0x00000008
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_SHORT_TF_SOURCE_SHIFT        3

/* DVI_DVF_0 :: DVF_BVB_STATUS :: LATE_LINE [02:02] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LATE_LINE_MASK               0x00000004
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LATE_LINE_SHIFT              2

/* DVI_DVF_0 :: DVF_BVB_STATUS :: LONG_LINE [01:01] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LONG_LINE_MASK               0x00000002
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_LONG_LINE_SHIFT              1

/* DVI_DVF_0 :: DVF_BVB_STATUS :: SHORT_LINE [00:00] */
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_SHORT_LINE_MASK              0x00000001
#define BCHP_DVI_DVF_0_DVF_BVB_STATUS_SHORT_LINE_SHIFT             0

#endif /* #ifndef BCHP_DVI_DVF_0_H__ */

/* End of File */