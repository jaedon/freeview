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
 * $brcm_Workfile: bchp_queue_0_cntrl.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 9:20p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:20:44 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_queue_0_cntrl.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 9:20p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_QUEUE_0_CNTRL_H__
#define BCHP_QUEUE_0_CNTRL_H__

/***************************************************************************
 *QUEUE_0_CNTRL
 ***************************************************************************/
#define BCHP_QUEUE_0_CNTRL_SIZE                  0x04201a00 /* Queue Size Register */
#define BCHP_QUEUE_0_CNTRL_CFGA                  0x04201a04 /* Queue Config A Register */
#define BCHP_QUEUE_0_CNTRL_CFGB                  0x04201a08 /* Queue Config B Register */
#define BCHP_QUEUE_0_CNTRL_STS                   0x04201a0c /* Queue Status Register */

/***************************************************************************
 *SIZE - Queue Size Register
 ***************************************************************************/
/* QUEUE_0_CNTRL :: SIZE :: Q_HEAD_PTR [31:18] */
#define BCHP_QUEUE_0_CNTRL_SIZE_Q_HEAD_PTR_MASK                    0xfffc0000
#define BCHP_QUEUE_0_CNTRL_SIZE_Q_HEAD_PTR_SHIFT                   18

/* QUEUE_0_CNTRL :: SIZE :: Q_TAIL_PTR [17:04] */
#define BCHP_QUEUE_0_CNTRL_SIZE_Q_TAIL_PTR_MASK                    0x0003fff0
#define BCHP_QUEUE_0_CNTRL_SIZE_Q_TAIL_PTR_SHIFT                   4

/* QUEUE_0_CNTRL :: SIZE :: reserved0 [03:02] */
#define BCHP_QUEUE_0_CNTRL_SIZE_reserved0_MASK                     0x0000000c
#define BCHP_QUEUE_0_CNTRL_SIZE_reserved0_SHIFT                    2

/* QUEUE_0_CNTRL :: SIZE :: Q_TKN_SIZE [01:00] */
#define BCHP_QUEUE_0_CNTRL_SIZE_Q_TKN_SIZE_MASK                    0x00000003
#define BCHP_QUEUE_0_CNTRL_SIZE_Q_TKN_SIZE_SHIFT                   0

/***************************************************************************
 *CFGA - Queue Config A Register
 ***************************************************************************/
/* QUEUE_0_CNTRL :: CFGA :: Q_SIZE [31:16] */
#define BCHP_QUEUE_0_CNTRL_CFGA_Q_SIZE_MASK                        0xffff0000
#define BCHP_QUEUE_0_CNTRL_CFGA_Q_SIZE_SHIFT                       16

/* QUEUE_0_CNTRL :: CFGA :: Q_START_ADDR [15:00] */
#define BCHP_QUEUE_0_CNTRL_CFGA_Q_START_ADDR_MASK                  0x0000ffff
#define BCHP_QUEUE_0_CNTRL_CFGA_Q_START_ADDR_SHIFT                 0

/***************************************************************************
 *CFGB - Queue Config B Register
 ***************************************************************************/
/* QUEUE_0_CNTRL :: CFGB :: reserved0 [31:30] */
#define BCHP_QUEUE_0_CNTRL_CFGB_reserved0_MASK                     0xc0000000
#define BCHP_QUEUE_0_CNTRL_CFGB_reserved0_SHIFT                    30

/* QUEUE_0_CNTRL :: CFGB :: Q_NUM_TKNS [29:16] */
#define BCHP_QUEUE_0_CNTRL_CFGB_Q_NUM_TKNS_MASK                    0x3fff0000
#define BCHP_QUEUE_0_CNTRL_CFGB_Q_NUM_TKNS_SHIFT                   16

/* QUEUE_0_CNTRL :: CFGB :: reserved1 [15:14] */
#define BCHP_QUEUE_0_CNTRL_CFGB_reserved1_MASK                     0x0000c000
#define BCHP_QUEUE_0_CNTRL_CFGB_reserved1_SHIFT                    14

/* QUEUE_0_CNTRL :: CFGB :: Q_LOW_WATERMARK [13:00] */
#define BCHP_QUEUE_0_CNTRL_CFGB_Q_LOW_WATERMARK_MASK               0x00003fff
#define BCHP_QUEUE_0_CNTRL_CFGB_Q_LOW_WATERMARK_SHIFT              0

/***************************************************************************
 *STS - Queue Status Register
 ***************************************************************************/
/* QUEUE_0_CNTRL :: STS :: reserved0 [31:14] */
#define BCHP_QUEUE_0_CNTRL_STS_reserved0_MASK                      0xffffc000
#define BCHP_QUEUE_0_CNTRL_STS_reserved0_SHIFT                     14

/* QUEUE_0_CNTRL :: STS :: Q_AVL_TKN_SPACE [13:00] */
#define BCHP_QUEUE_0_CNTRL_STS_Q_AVL_TKN_SPACE_MASK                0x00003fff
#define BCHP_QUEUE_0_CNTRL_STS_Q_AVL_TKN_SPACE_SHIFT               0

#endif /* #ifndef BCHP_QUEUE_0_CNTRL_H__ */

/* End of File */