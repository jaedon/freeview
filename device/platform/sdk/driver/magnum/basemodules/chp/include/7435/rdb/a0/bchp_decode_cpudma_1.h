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
 * $brcm_Workfile: bchp_decode_cpudma_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 5:08p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 10:59:42 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_decode_cpudma_1.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 5:08p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_CPUDMA_1_H__
#define BCHP_DECODE_CPUDMA_1_H__

/***************************************************************************
 *DECODE_CPUDMA_1
 ***************************************************************************/
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_SD_ADDR    0x00001800 /* SDRAM address */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR   0x00001804 /* Local Memory Address */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN        0x00001808 /* Length */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_SD_ADDR    0x00001810 /* REG_DMA1_SD_ADDR */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR   0x00001814 /* REG_DMA1_LCL_ADDR */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN        0x00001818 /* REG_DMA1_LEN */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_SD_ADDR    0x00001820 /* REG_DMA2_SD_ADDR */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR   0x00001824 /* REG_DMA2_LCL_ADDR */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN        0x00001828 /* REG_DMA2_LEN */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_SD_ADDR    0x00001830 /* REG_DMA3_SD_ADDR */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR   0x00001834 /* REG_DMA3_LCL_ADDR */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN        0x00001838 /* REG_DMA3_LEN */
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS      0x00001840 /* REG_DMA_STATUS */
#define BCHP_DECODE_CPUDMA_1_REG_CPUDMA_END      0x000018fc /* REG_CPUDMA_END */

/***************************************************************************
 *REG_DMA0_SD_ADDR - SDRAM address
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA0_SD_ADDR :: Sd_Addr [31:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_SD_ADDR_Sd_Addr_MASK         0xfffffffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_SD_ADDR_Sd_Addr_SHIFT        2

/* DECODE_CPUDMA_1 :: REG_DMA0_SD_ADDR :: reserved0 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_SD_ADDR_reserved0_MASK       0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_SD_ADDR_reserved0_SHIFT      0

/***************************************************************************
 *REG_DMA0_LCL_ADDR - Local Memory Address
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA0_LCL_ADDR :: reserved0 [31:28] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_reserved0_MASK      0xf0000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_reserved0_SHIFT     28

/* DECODE_CPUDMA_1 :: REG_DMA0_LCL_ADDR :: CpuLdSt [27:27] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_CpuLdSt_MASK        0x08000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_CpuLdSt_SHIFT       27

/* DECODE_CPUDMA_1 :: REG_DMA0_LCL_ADDR :: reserved1 [26:15] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_reserved1_MASK      0x07ff8000
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_reserved1_SHIFT     15

/* DECODE_CPUDMA_1 :: REG_DMA0_LCL_ADDR :: Addr [14:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_Addr_MASK           0x00007ffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_Addr_SHIFT          2

/* DECODE_CPUDMA_1 :: REG_DMA0_LCL_ADDR :: reserved2 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_reserved2_MASK      0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LCL_ADDR_reserved2_SHIFT     0

/***************************************************************************
 *REG_DMA0_LEN - Length
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA0_LEN :: Swap [31:31] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_Swap_MASK                0x80000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_Swap_SHIFT               31

/* DECODE_CPUDMA_1 :: REG_DMA0_LEN :: Alt_dest [30:30] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_Alt_dest_MASK            0x40000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_Alt_dest_SHIFT           30

/* DECODE_CPUDMA_1 :: REG_DMA0_LEN :: reserved0 [29:11] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_reserved0_MASK           0x3ffff800
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_reserved0_SHIFT          11

/* DECODE_CPUDMA_1 :: REG_DMA0_LEN :: Length [10:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_Length_MASK              0x000007fc
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_Length_SHIFT             2

/* DECODE_CPUDMA_1 :: REG_DMA0_LEN :: reserved1 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_reserved1_MASK           0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA0_LEN_reserved1_SHIFT          0

/***************************************************************************
 *REG_DMA1_SD_ADDR - REG_DMA1_SD_ADDR
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA1_SD_ADDR :: Sd_Addr [31:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_SD_ADDR_Sd_Addr_MASK         0xfffffffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_SD_ADDR_Sd_Addr_SHIFT        2

/* DECODE_CPUDMA_1 :: REG_DMA1_SD_ADDR :: reserved0 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_SD_ADDR_reserved0_MASK       0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_SD_ADDR_reserved0_SHIFT      0

/***************************************************************************
 *REG_DMA1_LCL_ADDR - REG_DMA1_LCL_ADDR
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA1_LCL_ADDR :: reserved0 [31:28] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_reserved0_MASK      0xf0000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_reserved0_SHIFT     28

/* DECODE_CPUDMA_1 :: REG_DMA1_LCL_ADDR :: CpuLdSt [27:27] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_CpuLdSt_MASK        0x08000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_CpuLdSt_SHIFT       27

/* DECODE_CPUDMA_1 :: REG_DMA1_LCL_ADDR :: reserved1 [26:15] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_reserved1_MASK      0x07ff8000
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_reserved1_SHIFT     15

/* DECODE_CPUDMA_1 :: REG_DMA1_LCL_ADDR :: Addr [14:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_Addr_MASK           0x00007ffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_Addr_SHIFT          2

/* DECODE_CPUDMA_1 :: REG_DMA1_LCL_ADDR :: reserved2 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_reserved2_MASK      0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LCL_ADDR_reserved2_SHIFT     0

/***************************************************************************
 *REG_DMA1_LEN - REG_DMA1_LEN
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA1_LEN :: Swap [31:31] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_Swap_MASK                0x80000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_Swap_SHIFT               31

/* DECODE_CPUDMA_1 :: REG_DMA1_LEN :: reserved0 [30:11] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_reserved0_MASK           0x7ffff800
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_reserved0_SHIFT          11

/* DECODE_CPUDMA_1 :: REG_DMA1_LEN :: Length [10:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_Length_MASK              0x000007fc
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_Length_SHIFT             2

/* DECODE_CPUDMA_1 :: REG_DMA1_LEN :: reserved1 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_reserved1_MASK           0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA1_LEN_reserved1_SHIFT          0

/***************************************************************************
 *REG_DMA2_SD_ADDR - REG_DMA2_SD_ADDR
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA2_SD_ADDR :: Sd_Addr [31:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_SD_ADDR_Sd_Addr_MASK         0xfffffffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_SD_ADDR_Sd_Addr_SHIFT        2

/* DECODE_CPUDMA_1 :: REG_DMA2_SD_ADDR :: reserved0 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_SD_ADDR_reserved0_MASK       0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_SD_ADDR_reserved0_SHIFT      0

/***************************************************************************
 *REG_DMA2_LCL_ADDR - REG_DMA2_LCL_ADDR
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA2_LCL_ADDR :: reserved0 [31:28] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_reserved0_MASK      0xf0000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_reserved0_SHIFT     28

/* DECODE_CPUDMA_1 :: REG_DMA2_LCL_ADDR :: CpuLdSt [27:27] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_CpuLdSt_MASK        0x08000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_CpuLdSt_SHIFT       27

/* DECODE_CPUDMA_1 :: REG_DMA2_LCL_ADDR :: reserved1 [26:15] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_reserved1_MASK      0x07ff8000
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_reserved1_SHIFT     15

/* DECODE_CPUDMA_1 :: REG_DMA2_LCL_ADDR :: Addr [14:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_Addr_MASK           0x00007ffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_Addr_SHIFT          2

/* DECODE_CPUDMA_1 :: REG_DMA2_LCL_ADDR :: reserved2 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_reserved2_MASK      0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LCL_ADDR_reserved2_SHIFT     0

/***************************************************************************
 *REG_DMA2_LEN - REG_DMA2_LEN
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA2_LEN :: Swap [31:31] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_Swap_MASK                0x80000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_Swap_SHIFT               31

/* DECODE_CPUDMA_1 :: REG_DMA2_LEN :: reserved0 [30:11] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_reserved0_MASK           0x7ffff800
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_reserved0_SHIFT          11

/* DECODE_CPUDMA_1 :: REG_DMA2_LEN :: Length [10:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_Length_MASK              0x000007fc
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_Length_SHIFT             2

/* DECODE_CPUDMA_1 :: REG_DMA2_LEN :: reserved1 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_reserved1_MASK           0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA2_LEN_reserved1_SHIFT          0

/***************************************************************************
 *REG_DMA3_SD_ADDR - REG_DMA3_SD_ADDR
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA3_SD_ADDR :: Sd_Addr [31:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_SD_ADDR_Sd_Addr_MASK         0xfffffffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_SD_ADDR_Sd_Addr_SHIFT        2

/* DECODE_CPUDMA_1 :: REG_DMA3_SD_ADDR :: reserved0 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_SD_ADDR_reserved0_MASK       0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_SD_ADDR_reserved0_SHIFT      0

/***************************************************************************
 *REG_DMA3_LCL_ADDR - REG_DMA3_LCL_ADDR
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA3_LCL_ADDR :: reserved0 [31:28] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_reserved0_MASK      0xf0000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_reserved0_SHIFT     28

/* DECODE_CPUDMA_1 :: REG_DMA3_LCL_ADDR :: CpuLdSt [27:27] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_CpuLdSt_MASK        0x08000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_CpuLdSt_SHIFT       27

/* DECODE_CPUDMA_1 :: REG_DMA3_LCL_ADDR :: reserved1 [26:15] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_reserved1_MASK      0x07ff8000
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_reserved1_SHIFT     15

/* DECODE_CPUDMA_1 :: REG_DMA3_LCL_ADDR :: Addr [14:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_Addr_MASK           0x00007ffc
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_Addr_SHIFT          2

/* DECODE_CPUDMA_1 :: REG_DMA3_LCL_ADDR :: reserved2 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_reserved2_MASK      0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LCL_ADDR_reserved2_SHIFT     0

/***************************************************************************
 *REG_DMA3_LEN - REG_DMA3_LEN
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA3_LEN :: Swap [31:31] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_Swap_MASK                0x80000000
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_Swap_SHIFT               31

/* DECODE_CPUDMA_1 :: REG_DMA3_LEN :: reserved0 [30:11] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_reserved0_MASK           0x7ffff800
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_reserved0_SHIFT          11

/* DECODE_CPUDMA_1 :: REG_DMA3_LEN :: Length [10:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_Length_MASK              0x000007fc
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_Length_SHIFT             2

/* DECODE_CPUDMA_1 :: REG_DMA3_LEN :: reserved1 [01:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_reserved1_MASK           0x00000003
#define BCHP_DECODE_CPUDMA_1_REG_DMA3_LEN_reserved1_SHIFT          0

/***************************************************************************
 *REG_DMA_STATUS - REG_DMA_STATUS
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_DMA_STATUS :: reserved0 [31:04] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_reserved0_MASK         0xfffffff0
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_reserved0_SHIFT        4

/* DECODE_CPUDMA_1 :: REG_DMA_STATUS :: Act3 [03:03] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act3_MASK              0x00000008
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act3_SHIFT             3

/* DECODE_CPUDMA_1 :: REG_DMA_STATUS :: Act2 [02:02] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act2_MASK              0x00000004
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act2_SHIFT             2

/* DECODE_CPUDMA_1 :: REG_DMA_STATUS :: Act1 [01:01] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act1_MASK              0x00000002
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act1_SHIFT             1

/* DECODE_CPUDMA_1 :: REG_DMA_STATUS :: Act0 [00:00] */
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act0_MASK              0x00000001
#define BCHP_DECODE_CPUDMA_1_REG_DMA_STATUS_Act0_SHIFT             0

/***************************************************************************
 *REG_CPUDMA_END - REG_CPUDMA_END
 ***************************************************************************/
/* DECODE_CPUDMA_1 :: REG_CPUDMA_END :: reserved0 [31:00] */
#define BCHP_DECODE_CPUDMA_1_REG_CPUDMA_END_reserved0_MASK         0xffffffff
#define BCHP_DECODE_CPUDMA_1_REG_CPUDMA_END_reserved0_SHIFT        0

#endif /* #ifndef BCHP_DECODE_CPUDMA_1_H__ */

/* End of File */