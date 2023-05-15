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
 * $brcm_Workfile: bchp_upg_aux_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 2:30p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:35:24 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_upg_aux_intr2.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 2:30p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_UPG_AUX_INTR2_H__
#define BCHP_UPG_AUX_INTR2_H__

/***************************************************************************
 *UPG_AUX_INTR2 - UPG AUX Level 2 Interrupt Registers
 ***************************************************************************/
#define BCHP_UPG_AUX_INTR2_CPU_STATUS            0x00406e80 /* CPU interrupt Status Register */
#define BCHP_UPG_AUX_INTR2_CPU_SET               0x00406e84 /* CPU interrupt Set Register */
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR             0x00406e88 /* CPU interrupt Clear Register */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS       0x00406e8c /* CPU interrupt Mask Status Register */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET          0x00406e90 /* CPU interrupt Mask Set Register */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR        0x00406e94 /* CPU interrupt Mask Clear Register */
#define BCHP_UPG_AUX_INTR2_PCI_STATUS            0x00406e98 /* PCI interrupt Status Register */
#define BCHP_UPG_AUX_INTR2_PCI_SET               0x00406e9c /* PCI interrupt Set Register */
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR             0x00406ea0 /* PCI interrupt Clear Register */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS       0x00406ea4 /* PCI interrupt Mask Status Register */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET          0x00406ea8 /* PCI interrupt Mask Set Register */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR        0x00406eac /* PCI interrupt Mask Clear Register */

/***************************************************************************
 *CPU_STATUS - CPU interrupt Status Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: CPU_STATUS :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_reserved0_MASK               0xffff8000
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_reserved0_SHIFT              15

/* UPG_AUX_INTR2 :: CPU_STATUS :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_DATA_READY_MASK         0x00004000
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_DATA_READY_SHIFT        14
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_DATA_READY_DEFAULT      0

/* UPG_AUX_INTR2 :: CPU_STATUS :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_OVER_TEMPERATURE_MASK   0x00002000
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_OVER_TEMPERATURE_SHIFT  13
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: CPU_STATUS :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_reserved1_MASK               0x00001fff
#define BCHP_UPG_AUX_INTR2_CPU_STATUS_reserved1_SHIFT              0

/***************************************************************************
 *CPU_SET - CPU interrupt Set Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: CPU_SET :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_CPU_SET_reserved0_MASK                  0xffff8000
#define BCHP_UPG_AUX_INTR2_CPU_SET_reserved0_SHIFT                 15

/* UPG_AUX_INTR2 :: CPU_SET :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_CPU_SET_TMON_DATA_READY_MASK            0x00004000
#define BCHP_UPG_AUX_INTR2_CPU_SET_TMON_DATA_READY_SHIFT           14
#define BCHP_UPG_AUX_INTR2_CPU_SET_TMON_DATA_READY_DEFAULT         0

/* UPG_AUX_INTR2 :: CPU_SET :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_CPU_SET_TMON_OVER_TEMPERATURE_MASK      0x00002000
#define BCHP_UPG_AUX_INTR2_CPU_SET_TMON_OVER_TEMPERATURE_SHIFT     13
#define BCHP_UPG_AUX_INTR2_CPU_SET_TMON_OVER_TEMPERATURE_DEFAULT   0

/* UPG_AUX_INTR2 :: CPU_SET :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_CPU_SET_reserved1_MASK                  0x00001fff
#define BCHP_UPG_AUX_INTR2_CPU_SET_reserved1_SHIFT                 0

/***************************************************************************
 *CPU_CLEAR - CPU interrupt Clear Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: CPU_CLEAR :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_reserved0_MASK                0xffff8000
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_reserved0_SHIFT               15

/* UPG_AUX_INTR2 :: CPU_CLEAR :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_TMON_DATA_READY_MASK          0x00004000
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_TMON_DATA_READY_SHIFT         14
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_TMON_DATA_READY_DEFAULT       0

/* UPG_AUX_INTR2 :: CPU_CLEAR :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_TMON_OVER_TEMPERATURE_MASK    0x00002000
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_TMON_OVER_TEMPERATURE_SHIFT   13
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: CPU_CLEAR :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_reserved1_MASK                0x00001fff
#define BCHP_UPG_AUX_INTR2_CPU_CLEAR_reserved1_SHIFT               0

/***************************************************************************
 *CPU_MASK_STATUS - CPU interrupt Mask Status Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: CPU_MASK_STATUS :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_reserved0_MASK          0xffff8000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_reserved0_SHIFT         15

/* UPG_AUX_INTR2 :: CPU_MASK_STATUS :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_TMON_DATA_READY_MASK    0x00004000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_TMON_DATA_READY_SHIFT   14
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_TMON_DATA_READY_DEFAULT 0

/* UPG_AUX_INTR2 :: CPU_MASK_STATUS :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_TMON_OVER_TEMPERATURE_MASK 0x00002000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_TMON_OVER_TEMPERATURE_SHIFT 13
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: CPU_MASK_STATUS :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_reserved1_MASK          0x00001fff
#define BCHP_UPG_AUX_INTR2_CPU_MASK_STATUS_reserved1_SHIFT         0

/***************************************************************************
 *CPU_MASK_SET - CPU interrupt Mask Set Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: CPU_MASK_SET :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_reserved0_MASK             0xffff8000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_reserved0_SHIFT            15

/* UPG_AUX_INTR2 :: CPU_MASK_SET :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_TMON_DATA_READY_MASK       0x00004000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_TMON_DATA_READY_SHIFT      14
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_TMON_DATA_READY_DEFAULT    0

/* UPG_AUX_INTR2 :: CPU_MASK_SET :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_TMON_OVER_TEMPERATURE_MASK 0x00002000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_TMON_OVER_TEMPERATURE_SHIFT 13
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: CPU_MASK_SET :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_reserved1_MASK             0x00001fff
#define BCHP_UPG_AUX_INTR2_CPU_MASK_SET_reserved1_SHIFT            0

/***************************************************************************
 *CPU_MASK_CLEAR - CPU interrupt Mask Clear Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: CPU_MASK_CLEAR :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_reserved0_MASK           0xffff8000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_reserved0_SHIFT          15

/* UPG_AUX_INTR2 :: CPU_MASK_CLEAR :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_TMON_DATA_READY_MASK     0x00004000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_TMON_DATA_READY_SHIFT    14
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_TMON_DATA_READY_DEFAULT  0

/* UPG_AUX_INTR2 :: CPU_MASK_CLEAR :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_TMON_OVER_TEMPERATURE_MASK 0x00002000
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_TMON_OVER_TEMPERATURE_SHIFT 13
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: CPU_MASK_CLEAR :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_reserved1_MASK           0x00001fff
#define BCHP_UPG_AUX_INTR2_CPU_MASK_CLEAR_reserved1_SHIFT          0

/***************************************************************************
 *PCI_STATUS - PCI interrupt Status Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: PCI_STATUS :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_reserved0_MASK               0xffff8000
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_reserved0_SHIFT              15

/* UPG_AUX_INTR2 :: PCI_STATUS :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_TMON_DATA_READY_MASK         0x00004000
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_TMON_DATA_READY_SHIFT        14
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_TMON_DATA_READY_DEFAULT      0

/* UPG_AUX_INTR2 :: PCI_STATUS :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_TMON_OVER_TEMPERATURE_MASK   0x00002000
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_TMON_OVER_TEMPERATURE_SHIFT  13
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: PCI_STATUS :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_reserved1_MASK               0x00001fff
#define BCHP_UPG_AUX_INTR2_PCI_STATUS_reserved1_SHIFT              0

/***************************************************************************
 *PCI_SET - PCI interrupt Set Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: PCI_SET :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_PCI_SET_reserved0_MASK                  0xffff8000
#define BCHP_UPG_AUX_INTR2_PCI_SET_reserved0_SHIFT                 15

/* UPG_AUX_INTR2 :: PCI_SET :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_PCI_SET_TMON_DATA_READY_MASK            0x00004000
#define BCHP_UPG_AUX_INTR2_PCI_SET_TMON_DATA_READY_SHIFT           14
#define BCHP_UPG_AUX_INTR2_PCI_SET_TMON_DATA_READY_DEFAULT         0

/* UPG_AUX_INTR2 :: PCI_SET :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_PCI_SET_TMON_OVER_TEMPERATURE_MASK      0x00002000
#define BCHP_UPG_AUX_INTR2_PCI_SET_TMON_OVER_TEMPERATURE_SHIFT     13
#define BCHP_UPG_AUX_INTR2_PCI_SET_TMON_OVER_TEMPERATURE_DEFAULT   0

/* UPG_AUX_INTR2 :: PCI_SET :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_PCI_SET_reserved1_MASK                  0x00001fff
#define BCHP_UPG_AUX_INTR2_PCI_SET_reserved1_SHIFT                 0

/***************************************************************************
 *PCI_CLEAR - PCI interrupt Clear Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: PCI_CLEAR :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_reserved0_MASK                0xffff8000
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_reserved0_SHIFT               15

/* UPG_AUX_INTR2 :: PCI_CLEAR :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_TMON_DATA_READY_MASK          0x00004000
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_TMON_DATA_READY_SHIFT         14
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_TMON_DATA_READY_DEFAULT       0

/* UPG_AUX_INTR2 :: PCI_CLEAR :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_TMON_OVER_TEMPERATURE_MASK    0x00002000
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_TMON_OVER_TEMPERATURE_SHIFT   13
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: PCI_CLEAR :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_reserved1_MASK                0x00001fff
#define BCHP_UPG_AUX_INTR2_PCI_CLEAR_reserved1_SHIFT               0

/***************************************************************************
 *PCI_MASK_STATUS - PCI interrupt Mask Status Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: PCI_MASK_STATUS :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_reserved0_MASK          0xffff8000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_reserved0_SHIFT         15

/* UPG_AUX_INTR2 :: PCI_MASK_STATUS :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_TMON_DATA_READY_MASK    0x00004000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_TMON_DATA_READY_SHIFT   14
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_TMON_DATA_READY_DEFAULT 0

/* UPG_AUX_INTR2 :: PCI_MASK_STATUS :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_TMON_OVER_TEMPERATURE_MASK 0x00002000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_TMON_OVER_TEMPERATURE_SHIFT 13
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: PCI_MASK_STATUS :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_reserved1_MASK          0x00001fff
#define BCHP_UPG_AUX_INTR2_PCI_MASK_STATUS_reserved1_SHIFT         0

/***************************************************************************
 *PCI_MASK_SET - PCI interrupt Mask Set Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: PCI_MASK_SET :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_reserved0_MASK             0xffff8000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_reserved0_SHIFT            15

/* UPG_AUX_INTR2 :: PCI_MASK_SET :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_TMON_DATA_READY_MASK       0x00004000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_TMON_DATA_READY_SHIFT      14
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_TMON_DATA_READY_DEFAULT    0

/* UPG_AUX_INTR2 :: PCI_MASK_SET :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_TMON_OVER_TEMPERATURE_MASK 0x00002000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_TMON_OVER_TEMPERATURE_SHIFT 13
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: PCI_MASK_SET :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_reserved1_MASK             0x00001fff
#define BCHP_UPG_AUX_INTR2_PCI_MASK_SET_reserved1_SHIFT            0

/***************************************************************************
 *PCI_MASK_CLEAR - PCI interrupt Mask Clear Register
 ***************************************************************************/
/* UPG_AUX_INTR2 :: PCI_MASK_CLEAR :: reserved0 [31:15] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_reserved0_MASK           0xffff8000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_reserved0_SHIFT          15

/* UPG_AUX_INTR2 :: PCI_MASK_CLEAR :: TMON_DATA_READY [14:14] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_TMON_DATA_READY_MASK     0x00004000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_TMON_DATA_READY_SHIFT    14
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_TMON_DATA_READY_DEFAULT  0

/* UPG_AUX_INTR2 :: PCI_MASK_CLEAR :: TMON_OVER_TEMPERATURE [13:13] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_TMON_OVER_TEMPERATURE_MASK 0x00002000
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_TMON_OVER_TEMPERATURE_SHIFT 13
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_TMON_OVER_TEMPERATURE_DEFAULT 0

/* UPG_AUX_INTR2 :: PCI_MASK_CLEAR :: reserved1 [12:00] */
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_reserved1_MASK           0x00001fff
#define BCHP_UPG_AUX_INTR2_PCI_MASK_CLEAR_reserved1_SHIFT          0

#endif /* #ifndef BCHP_UPG_AUX_INTR2_H__ */

/* End of File */