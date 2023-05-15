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
 * $brcm_Workfile: bchp_moca_timer0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 9:08p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:13:46 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_moca_timer0.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 9:08p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_MOCA_TIMER0_H__
#define BCHP_MOCA_TIMER0_H__

/***************************************************************************
 *MOCA_TIMER0 - MOCA_TIMER Timer 0
 ***************************************************************************/
#define BCHP_MOCA_TIMER0_TIMER                   0x002a0000 /* Timer Up Counter State */
#define BCHP_MOCA_TIMER0_REFERENCE               0x002a0004 /* Timer Reference Register */
#define BCHP_MOCA_TIMER0_CTRL                    0x002a0008 /* Timer Control Register */
#define BCHP_MOCA_TIMER0_INTERRUPT_STATUS        0x002a000c /* Timer Interrupt Status Register */
#define BCHP_MOCA_TIMER0_RESTART                 0x002a0010 /* Timer Arm Register */

/***************************************************************************
 *TIMER - Timer Up Counter State
 ***************************************************************************/
/* MOCA_TIMER0 :: TIMER :: cnt [31:00] */
#define BCHP_MOCA_TIMER0_TIMER_cnt_MASK                            0xffffffff
#define BCHP_MOCA_TIMER0_TIMER_cnt_SHIFT                           0

/***************************************************************************
 *REFERENCE - Timer Reference Register
 ***************************************************************************/
/* MOCA_TIMER0 :: REFERENCE :: ref [31:00] */
#define BCHP_MOCA_TIMER0_REFERENCE_ref_MASK                        0xffffffff
#define BCHP_MOCA_TIMER0_REFERENCE_ref_SHIFT                       0

/***************************************************************************
 *CTRL - Timer Control Register
 ***************************************************************************/
/* MOCA_TIMER0 :: CTRL :: reserved0 [31:03] */
#define BCHP_MOCA_TIMER0_CTRL_reserved0_MASK                       0xfffffff8
#define BCHP_MOCA_TIMER0_CTRL_reserved0_SHIFT                      3

/* MOCA_TIMER0 :: CTRL :: ie [02:02] */
#define BCHP_MOCA_TIMER0_CTRL_ie_MASK                              0x00000004
#define BCHP_MOCA_TIMER0_CTRL_ie_SHIFT                             2

/* MOCA_TIMER0 :: CTRL :: mode [01:00] */
#define BCHP_MOCA_TIMER0_CTRL_mode_MASK                            0x00000003
#define BCHP_MOCA_TIMER0_CTRL_mode_SHIFT                           0

/***************************************************************************
 *INTERRUPT_STATUS - Timer Interrupt Status Register
 ***************************************************************************/
/* MOCA_TIMER0 :: INTERRUPT_STATUS :: reserved0 [31:01] */
#define BCHP_MOCA_TIMER0_INTERRUPT_STATUS_reserved0_MASK           0xfffffffe
#define BCHP_MOCA_TIMER0_INTERRUPT_STATUS_reserved0_SHIFT          1

/* MOCA_TIMER0 :: INTERRUPT_STATUS :: is [00:00] */
#define BCHP_MOCA_TIMER0_INTERRUPT_STATUS_is_MASK                  0x00000001
#define BCHP_MOCA_TIMER0_INTERRUPT_STATUS_is_SHIFT                 0

/***************************************************************************
 *RESTART - Timer Arm Register
 ***************************************************************************/
/* MOCA_TIMER0 :: RESTART :: reserved0 [31:01] */
#define BCHP_MOCA_TIMER0_RESTART_reserved0_MASK                    0xfffffffe
#define BCHP_MOCA_TIMER0_RESTART_reserved0_SHIFT                   1

/* MOCA_TIMER0 :: RESTART :: arm [00:00] */
#define BCHP_MOCA_TIMER0_RESTART_arm_MASK                          0x00000001
#define BCHP_MOCA_TIMER0_RESTART_arm_SHIFT                         0

#endif /* #ifndef BCHP_MOCA_TIMER0_H__ */

/* End of File */