/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_webhif_cpu_intr1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 6:46p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:20 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_webhif_cpu_intr1.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 6:46p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_WEBHIF_CPU_INTR1_H__
#define BCHP_WEBHIF_CPU_INTR1_H__

/***************************************************************************
 *WEBHIF_CPU_INTR1 - WEBHIF CPU1 Thread Processor 0 Level 1 Interrupt Controller Registers
 ***************************************************************************/
#define BCHP_WEBHIF_CPU_INTR1_INTR_W0_STATUS     0x00420600 /* Interrupt Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W1_STATUS     0x00420604 /* Interrupt Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W2_STATUS     0x00420608 /* Interrupt Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W3_STATUS     0x0042060c /* Interrupt Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W0_MASK_STATUS 0x00420610 /* Interrupt Mask Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W1_MASK_STATUS 0x00420614 /* Interrupt Mask Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W2_MASK_STATUS 0x00420618 /* Interrupt Mask Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W3_MASK_STATUS 0x0042061c /* Interrupt Mask Status Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W0_MASK_SET   0x00420620 /* Interrupt Mask Set Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W1_MASK_SET   0x00420624 /* Interrupt Mask Set Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W2_MASK_SET   0x00420628 /* Interrupt Mask Set Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W3_MASK_SET   0x0042062c /* Interrupt Mask Set Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W0_MASK_CLEAR 0x00420630 /* Interrupt Mask Clear Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W1_MASK_CLEAR 0x00420634 /* Interrupt Mask Clear Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W2_MASK_CLEAR 0x00420638 /* Interrupt Mask Clear Register */
#define BCHP_WEBHIF_CPU_INTR1_INTR_W3_MASK_CLEAR 0x0042063c /* Interrupt Mask Clear Register */

#endif /* #ifndef BCHP_WEBHIF_CPU_INTR1_H__ */

/* End of File */