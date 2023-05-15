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
 * $brcm_Workfile: bchp_xpt_dpcr5.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/11 6:16p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:27:38 2011
 *                 MD5 Checksum         c1b53a9408066d1e6123c0860dfb8065
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_xpt_dpcr5.h $
 * 
 * Hydra_Software_Devel/1   4/14/11 6:16p albertl
 * SW7346-143: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_DPCR5_H__
#define BCHP_XPT_DPCR5_H__

/***************************************************************************
 *XPT_DPCR5 - XPT DPCR5 Control Registers
 ***************************************************************************/
#define BCHP_XPT_DPCR5_PID_CH                    0x00a07280 /* Data Transport PCR PID Channel Register */
#define BCHP_XPT_DPCR5_CTRL                      0x00a07284 /* Data Transport PCR Control Register */
#define BCHP_XPT_DPCR5_INTR_STATUS_REG           0x00a07288 /* Interrupt Status Register */
#define BCHP_XPT_DPCR5_INTR_STATUS_REG_EN        0x00a0728c /* Interrupt Status Enable Register */
#define BCHP_XPT_DPCR5_STC_EXT_CTRL              0x00a07290 /* Data Transport PCR STC Extension Control Register */
#define BCHP_XPT_DPCR5_MAX_PCR_ERROR             0x00a072a0 /* Data Transport PCR Max PCR Error Register */
#define BCHP_XPT_DPCR5_SEND_BASE                 0x00a072a4 /* Data Transport PCR Send Base Register */
#define BCHP_XPT_DPCR5_SEND_EXT                  0x00a072a8 /* Data Transport PCR Send Extension Register */
#define BCHP_XPT_DPCR5_STC_EXT_CTRL27            0x00a072ac /* Data Transport PCR STC Extension Control Register (Test Only) */
#define BCHP_XPT_DPCR5_PWM_CTRLVALUE             0x00a072b8 /* Data Transport PCR PWM Control Value Register */
#define BCHP_XPT_DPCR5_LAST_PCR_HI               0x00a072bc /* Data Transport PCR Last PCR MSBs Register */
#define BCHP_XPT_DPCR5_LAST_PCR_LO               0x00a072c0 /* Data Transport PCR Last PCR LSBs Register */
#define BCHP_XPT_DPCR5_STC_BASE_LSBS             0x00a072c8 /* Data Transport PCR STC Base LSBs Register */
#define BCHP_XPT_DPCR5_PHASE_ERROR               0x00a072cc /* Timebase Last Phase Error */
#define BCHP_XPT_DPCR5_LOOP_CTRL                 0x00a072d0 /* Timebase Control */
#define BCHP_XPT_DPCR5_REF_PCR_PRESCALE          0x00a072d4 /* Timebase Frequency Reference Prescale Control */
#define BCHP_XPT_DPCR5_REF_PCR_INC               0x00a072d8 /* Timebase Frequency Reference Increment Control */
#define BCHP_XPT_DPCR5_CENTER                    0x00a072dc /* Timebase Center Frequency */
#define BCHP_XPT_DPCR5_STC_HI                    0x00a072b0 /* Data Transport PCR STC MSBs Register */
#define BCHP_XPT_DPCR5_STC_LO                    0x00a072b4 /* Data Transport PCR STC LSBs Register */
#define BCHP_XPT_DPCR5_ACCUM_VALUE               0x00a072e0 /* Timebase Loop Filter Integrator */
#define BCHP_XPT_DPCR5_PCR_COUNT                 0x00a072e4 /* Data Transport PCR Phase Error Register */
#define BCHP_XPT_DPCR5_SOFT_PCR_CTRL             0x00a072e8 /* Data Transport Soft PCR Control Register */
#define BCHP_XPT_DPCR5_SOFT_PCR_BASE             0x00a072ec /* Data Transport Soft PCR BASE Register */
#define BCHP_XPT_DPCR5_SOFT_PCR_EXT              0x00a072f0 /* Data Transport Soft PCR Extension Register */
#define BCHP_XPT_DPCR5_PHASE_ERROR_CLAMP         0x00a072f4 /* Timebase Phase Error Control */

#endif /* #ifndef BCHP_XPT_DPCR5_H__ */

/* End of File */