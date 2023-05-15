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
 * $brcm_Workfile: bchp_xpt_dpcr1.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/22/11 10:05p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 22 16:17:33 2011
 *                 MD5 Checksum         f1fe9dd101680af6476d6b0b4e1d855e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b0/bchp_xpt_dpcr1.h $
 * 
 * Hydra_Software_Devel/2   6/22/11 10:05p pntruong
 * SW7231-196: Synced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_DPCR1_H__
#define BCHP_XPT_DPCR1_H__

/***************************************************************************
 *XPT_DPCR1 - XPT DPCR1 Control Registers
 ***************************************************************************/
#define BCHP_XPT_DPCR1_PID_CH                    0x00a07080 /* Data Transport PCR PID Channel Register */
#define BCHP_XPT_DPCR1_CTRL                      0x00a07084 /* Data Transport PCR Control Register */
#define BCHP_XPT_DPCR1_INTR_STATUS_REG           0x00a07088 /* Interrupt Status Register */
#define BCHP_XPT_DPCR1_INTR_STATUS_REG_EN        0x00a0708c /* Interrupt Status Enable Register */
#define BCHP_XPT_DPCR1_STC_EXT_CTRL              0x00a07090 /* Data Transport PCR STC Extension Control Register */
#define BCHP_XPT_DPCR1_MAX_PCR_ERROR             0x00a070a0 /* Data Transport PCR Max PCR Error Register */
#define BCHP_XPT_DPCR1_SEND_BASE                 0x00a070a4 /* Data Transport PCR Send Base Register */
#define BCHP_XPT_DPCR1_SEND_EXT                  0x00a070a8 /* Data Transport PCR Send Extension Register */
#define BCHP_XPT_DPCR1_STC_EXT_CTRL27            0x00a070ac /* Data Transport PCR STC Extension Control Register (Test Only) */
#define BCHP_XPT_DPCR1_PWM_CTRLVALUE             0x00a070b8 /* Data Transport PCR PWM Control Value Register */
#define BCHP_XPT_DPCR1_LAST_PCR_HI               0x00a070bc /* Data Transport PCR Last PCR MSBs Register */
#define BCHP_XPT_DPCR1_LAST_PCR_LO               0x00a070c0 /* Data Transport PCR Last PCR LSBs Register */
#define BCHP_XPT_DPCR1_STC_BASE_LSBS             0x00a070c8 /* Data Transport PCR STC Base LSBs Register */
#define BCHP_XPT_DPCR1_PHASE_ERROR               0x00a070cc /* Timebase Last Phase Error */
#define BCHP_XPT_DPCR1_LOOP_CTRL                 0x00a070d0 /* Timebase Control */
#define BCHP_XPT_DPCR1_REF_PCR_PRESCALE          0x00a070d4 /* Timebase Frequency Reference Prescale Control */
#define BCHP_XPT_DPCR1_REF_PCR_INC               0x00a070d8 /* Timebase Frequency Reference Increment Control */
#define BCHP_XPT_DPCR1_CENTER                    0x00a070dc /* Timebase Center Frequency */
#define BCHP_XPT_DPCR1_STC_HI                    0x00a070b0 /* Data Transport PCR STC MSBs Register */
#define BCHP_XPT_DPCR1_STC_LO                    0x00a070b4 /* Data Transport PCR STC LSBs Register */
#define BCHP_XPT_DPCR1_ACCUM_VALUE               0x00a070e0 /* Timebase Loop Filter Integrator */
#define BCHP_XPT_DPCR1_PCR_COUNT                 0x00a070e4 /* Data Transport PCR Phase Error Register */
#define BCHP_XPT_DPCR1_SOFT_PCR_CTRL             0x00a070e8 /* Data Transport Soft PCR Control Register */
#define BCHP_XPT_DPCR1_SOFT_PCR_BASE             0x00a070ec /* Data Transport Soft PCR BASE Register */
#define BCHP_XPT_DPCR1_SOFT_PCR_EXT              0x00a070f0 /* Data Transport Soft PCR Extension Register */
#define BCHP_XPT_DPCR1_PHASE_ERROR_CLAMP         0x00a070f4 /* Timebase Phase Error Control */

#endif /* #ifndef BCHP_XPT_DPCR1_H__ */

/* End of File */