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
 * $brcm_Workfile: bchp_xpt_dpcr3.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/19/10 1:01p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Aug 18 16:21:27 2010
 *                 MD5 Checksum         15259d31203bcf46d3606fbfea3c771d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/a0/bchp_xpt_dpcr3.h $
 * 
 * Hydra_Software_Devel/1   8/19/10 1:01p pntruong
 * SW7552-2: Create scripts and checkin initial version of RDB header
 * files.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_DPCR3_H__
#define BCHP_XPT_DPCR3_H__

/***************************************************************************
 *XPT_DPCR3 - XPT DPCR3 Control Registers
 ***************************************************************************/
#define BCHP_XPT_DPCR3_PID_CH                    0x00a06180 /* Data Transport PCR PID Channel Register */
#define BCHP_XPT_DPCR3_CTRL                      0x00a06184 /* Data Transport PCR Control Register */
#define BCHP_XPT_DPCR3_INTR_STATUS_REG           0x00a06188 /* Interrupt Status Register */
#define BCHP_XPT_DPCR3_INTR_STATUS_REG_EN        0x00a0618c /* Interrupt Status Enable Register */
#define BCHP_XPT_DPCR3_STC_EXT_CTRL              0x00a06190 /* Data Transport PCR STC Extension Control Register */
#define BCHP_XPT_DPCR3_MAX_PCR_ERROR             0x00a061a0 /* Data Transport PCR Max PCR Error Register */
#define BCHP_XPT_DPCR3_SEND_BASE                 0x00a061a4 /* Data Transport PCR Send Base Register */
#define BCHP_XPT_DPCR3_SEND_EXT                  0x00a061a8 /* Data Transport PCR Send Extension Register */
#define BCHP_XPT_DPCR3_STC_EXT_CTRL27            0x00a061ac /* Data Transport PCR STC Extension Control Register (Test Only) */
#define BCHP_XPT_DPCR3_PWM_CTRLVALUE             0x00a061b8 /* Data Transport PCR PWM Control Value Register */
#define BCHP_XPT_DPCR3_LAST_PCR_HI               0x00a061bc /* Data Transport PCR Last PCR MSBs Register */
#define BCHP_XPT_DPCR3_LAST_PCR_LO               0x00a061c0 /* Data Transport PCR Last PCR LSBs Register */
#define BCHP_XPT_DPCR3_STC_BASE_LSBS             0x00a061c8 /* Data Transport PCR STC Base LSBs Register */
#define BCHP_XPT_DPCR3_PHASE_ERROR               0x00a061cc /* Timebase Last Phase Error */
#define BCHP_XPT_DPCR3_LOOP_CTRL                 0x00a061d0 /* Timebase Control */
#define BCHP_XPT_DPCR3_REF_PCR_PRESCALE          0x00a061d4 /* Timebase Frequency Reference Prescale Control */
#define BCHP_XPT_DPCR3_REF_PCR_INC               0x00a061d8 /* Timebase Frequency Reference Increment Control */
#define BCHP_XPT_DPCR3_CENTER                    0x00a061dc /* Timebase Center Frequency */
#define BCHP_XPT_DPCR3_STC_HI                    0x00a061b0 /* Data Transport PCR STC MSBs Register */
#define BCHP_XPT_DPCR3_STC_LO                    0x00a061b4 /* Data Transport PCR STC LSBs Register */
#define BCHP_XPT_DPCR3_ACCUM_VALUE               0x00a061e0 /* Timebase Loop Filter Integrator */
#define BCHP_XPT_DPCR3_PCR_COUNT                 0x00a061e4 /* Data Transport PCR Phase Error Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_CTRL             0x00a061e8 /* Data Transport Soft PCR Control Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_BASE             0x00a061ec /* Data Transport Soft PCR BASE Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_EXT              0x00a061f0 /* Data Transport Soft PCR Extension Register */
#define BCHP_XPT_DPCR3_PHASE_ERROR_CLAMP         0x00a061f4 /* Timebase Phase Error Control */

#endif /* #ifndef BCHP_XPT_DPCR3_H__ */

/* End of File */