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
 * $brcm_Workfile: bchp_lmb_bridge.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 11:06a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 08:54:03 2008
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
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_lmb_bridge.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 11:06a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_LMB_BRIDGE_H__
#define BCHP_LMB_BRIDGE_H__

/***************************************************************************
 *LMB_BRIDGE - LMB_BRIDGE Registers
 ***************************************************************************/
#define BCHP_LMB_BRIDGE_CORE_REV_ID              0x00000600 /* LMB Bridge Revision ID Register */
#define BCHP_LMB_BRIDGE_DUMMY_CMD                0x00000604 /* Dummy SCB Command */
#define BCHP_LMB_BRIDGE_DUMMY_REQ_CNT_CPU0       0x00000608 /* Dummy Request Count CPU0 */
#define BCHP_LMB_BRIDGE_DUMMY_REQ_CNT_CPU1       0x0000060c /* Dummy Request Count CPU1 */
#define BCHP_LMB_BRIDGE_CNTR_RST                 0x00000610 /* Reset Request Counters */
#define BCHP_LMB_BRIDGE_CNTR_FREEZE              0x00000614 /* Freeze Request Counters */
#define BCHP_LMB_BRIDGE_VAL_REQ_CNT_CPU0         0x00000618 /* Valid Request Count CPU0 */
#define BCHP_LMB_BRIDGE_VAL_REQ_CNT_CPU1         0x0000061c /* Valid Request Count CPU1 */

/***************************************************************************
 *CORE_REV_ID - LMB Bridge Revision ID Register
 ***************************************************************************/
/* LMB_BRIDGE :: CORE_REV_ID :: reserved0 [31:16] */
#define BCHP_LMB_BRIDGE_CORE_REV_ID_reserved0_MASK                 0xffff0000
#define BCHP_LMB_BRIDGE_CORE_REV_ID_reserved0_SHIFT                16

/* LMB_BRIDGE :: CORE_REV_ID :: MAJOR [15:08] */
#define BCHP_LMB_BRIDGE_CORE_REV_ID_MAJOR_MASK                     0x0000ff00
#define BCHP_LMB_BRIDGE_CORE_REV_ID_MAJOR_SHIFT                    8

/* LMB_BRIDGE :: CORE_REV_ID :: MINOR [07:00] */
#define BCHP_LMB_BRIDGE_CORE_REV_ID_MINOR_MASK                     0x000000ff
#define BCHP_LMB_BRIDGE_CORE_REV_ID_MINOR_SHIFT                    0

/***************************************************************************
 *DUMMY_CMD - Dummy SCB Command
 ***************************************************************************/
/* LMB_BRIDGE :: DUMMY_CMD :: reserved0 [31:09] */
#define BCHP_LMB_BRIDGE_DUMMY_CMD_reserved0_MASK                   0xfffffe00
#define BCHP_LMB_BRIDGE_DUMMY_CMD_reserved0_SHIFT                  9

/* LMB_BRIDGE :: DUMMY_CMD :: CMD [08:00] */
#define BCHP_LMB_BRIDGE_DUMMY_CMD_CMD_MASK                         0x000001ff
#define BCHP_LMB_BRIDGE_DUMMY_CMD_CMD_SHIFT                        0

/***************************************************************************
 *DUMMY_REQ_CNT_CPU0 - Dummy Request Count CPU0
 ***************************************************************************/
/* LMB_BRIDGE :: DUMMY_REQ_CNT_CPU0 :: COUNT [31:00] */
#define BCHP_LMB_BRIDGE_DUMMY_REQ_CNT_CPU0_COUNT_MASK              0xffffffff
#define BCHP_LMB_BRIDGE_DUMMY_REQ_CNT_CPU0_COUNT_SHIFT             0

/***************************************************************************
 *DUMMY_REQ_CNT_CPU1 - Dummy Request Count CPU1
 ***************************************************************************/
/* LMB_BRIDGE :: DUMMY_REQ_CNT_CPU1 :: COUNT [31:00] */
#define BCHP_LMB_BRIDGE_DUMMY_REQ_CNT_CPU1_COUNT_MASK              0xffffffff
#define BCHP_LMB_BRIDGE_DUMMY_REQ_CNT_CPU1_COUNT_SHIFT             0

/***************************************************************************
 *CNTR_RST - Reset Request Counters
 ***************************************************************************/
/* LMB_BRIDGE :: CNTR_RST :: reserved0 [31:04] */
#define BCHP_LMB_BRIDGE_CNTR_RST_reserved0_MASK                    0xfffffff0
#define BCHP_LMB_BRIDGE_CNTR_RST_reserved0_SHIFT                   4

/* LMB_BRIDGE :: CNTR_RST :: RST_DUMMY_REQ_CNT_CPU1 [03:03] */
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_DUMMY_REQ_CNT_CPU1_MASK       0x00000008
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_DUMMY_REQ_CNT_CPU1_SHIFT      3

/* LMB_BRIDGE :: CNTR_RST :: RST_DUMMY_REQ_CNT_CPU0 [02:02] */
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_DUMMY_REQ_CNT_CPU0_MASK       0x00000004
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_DUMMY_REQ_CNT_CPU0_SHIFT      2

/* LMB_BRIDGE :: CNTR_RST :: RST_VAL_REQ_CNT_CPU1 [01:01] */
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_VAL_REQ_CNT_CPU1_MASK         0x00000002
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_VAL_REQ_CNT_CPU1_SHIFT        1

/* LMB_BRIDGE :: CNTR_RST :: RST_VAL_REQ_CNT_CPU0 [00:00] */
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_VAL_REQ_CNT_CPU0_MASK         0x00000001
#define BCHP_LMB_BRIDGE_CNTR_RST_RST_VAL_REQ_CNT_CPU0_SHIFT        0

/***************************************************************************
 *CNTR_FREEZE - Freeze Request Counters
 ***************************************************************************/
/* LMB_BRIDGE :: CNTR_FREEZE :: reserved0 [31:04] */
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_reserved0_MASK                 0xfffffff0
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_reserved0_SHIFT                4

/* LMB_BRIDGE :: CNTR_FREEZE :: FRZ_DUMMY_REQ_CNT_CPU1 [03:03] */
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_DUMMY_REQ_CNT_CPU1_MASK    0x00000008
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_DUMMY_REQ_CNT_CPU1_SHIFT   3

/* LMB_BRIDGE :: CNTR_FREEZE :: FRZ_DUMMY_REQ_CNT_CPU0 [02:02] */
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_DUMMY_REQ_CNT_CPU0_MASK    0x00000004
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_DUMMY_REQ_CNT_CPU0_SHIFT   2

/* LMB_BRIDGE :: CNTR_FREEZE :: FRZ_VAL_REQ_CNT_CPU1 [01:01] */
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_VAL_REQ_CNT_CPU1_MASK      0x00000002
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_VAL_REQ_CNT_CPU1_SHIFT     1

/* LMB_BRIDGE :: CNTR_FREEZE :: FRZ_VAL_REQ_CNT_CPU0 [00:00] */
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_VAL_REQ_CNT_CPU0_MASK      0x00000001
#define BCHP_LMB_BRIDGE_CNTR_FREEZE_FRZ_VAL_REQ_CNT_CPU0_SHIFT     0

/***************************************************************************
 *VAL_REQ_CNT_CPU0 - Valid Request Count CPU0
 ***************************************************************************/
/* LMB_BRIDGE :: VAL_REQ_CNT_CPU0 :: COUNT [31:00] */
#define BCHP_LMB_BRIDGE_VAL_REQ_CNT_CPU0_COUNT_MASK                0xffffffff
#define BCHP_LMB_BRIDGE_VAL_REQ_CNT_CPU0_COUNT_SHIFT               0

/***************************************************************************
 *VAL_REQ_CNT_CPU1 - Valid Request Count CPU1
 ***************************************************************************/
/* LMB_BRIDGE :: VAL_REQ_CNT_CPU1 :: COUNT [31:00] */
#define BCHP_LMB_BRIDGE_VAL_REQ_CNT_CPU1_COUNT_MASK                0xffffffff
#define BCHP_LMB_BRIDGE_VAL_REQ_CNT_CPU1_COUNT_SHIFT               0

#endif /* #ifndef BCHP_LMB_BRIDGE_H__ */

/* End of File */