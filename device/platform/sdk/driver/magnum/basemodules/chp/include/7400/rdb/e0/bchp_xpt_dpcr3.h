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
 * $brcm_Workfile: bchp_xpt_dpcr3.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 12:04p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 09:05:05 2008
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
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_xpt_dpcr3.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 12:04p rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_DPCR3_H__
#define BCHP_XPT_DPCR3_H__

/***************************************************************************
 *XPT_DPCR3 - XPT DPCR3 Control Registers
 ***************************************************************************/
#define BCHP_XPT_DPCR3_PID                       0x0020a300 /* Data Transport PCR PID Register */
#define BCHP_XPT_DPCR3_CTRL                      0x0020a304 /* Data Transport PCR Control Register */
#define BCHP_XPT_DPCR3_INTR_STATUS_REG           0x0020a308 /* Interrupt Status Register */
#define BCHP_XPT_DPCR3_INTR_STATUS_REG_EN        0x0020a30c /* Interrupt Status Enable Register */
#define BCHP_XPT_DPCR3_STC_EXT_CTRL              0x0020a310 /* Data Transport PCR STC Extension Control Register */
#define BCHP_XPT_DPCR3_MAX_PCR_ERROR             0x0020a320 /* Data Transport PCR Max PCR Error Register */
#define BCHP_XPT_DPCR3_SEND_BASE                 0x0020a324 /* Data Transport PCR Send Base Register */
#define BCHP_XPT_DPCR3_SEND_EXT                  0x0020a328 /* Data Transport PCR Send Extension Register */
#define BCHP_XPT_DPCR3_STC_EXT_CTRL27            0x0020a32c /* Data Transport PCR STC Extension Control Register (Test Only) */
#define BCHP_XPT_DPCR3_PWM_CTRLVALUE             0x0020a338 /* Data Transport PCR PWM Control Value Register */
#define BCHP_XPT_DPCR3_LAST_PCR_HI               0x0020a33c /* Data Transport PCR Last PCR MSBs Register */
#define BCHP_XPT_DPCR3_LAST_PCR_LO               0x0020a340 /* Data Transport PCR Last PCR LSBs Register */
#define BCHP_XPT_DPCR3_STC_BASE_LSBS             0x0020a348 /* Data Transport PCR STC Base LSBs Register */
#define BCHP_XPT_DPCR3_PHASE_ERROR               0x0020a34c /* Timebase Last Phase Error */
#define BCHP_XPT_DPCR3_LOOP_CTRL                 0x0020a350 /* Timebase Control */
#define BCHP_XPT_DPCR3_REF_PCR                   0x0020a354 /* Timebase Frequency Reference Control */
#define BCHP_XPT_DPCR3_CENTER                    0x0020a358 /* Timebase Center Frequency */
#define BCHP_XPT_DPCR3_STC_HI                    0x0020a330 /* Data Transport PCR STC MSBs Register */
#define BCHP_XPT_DPCR3_STC_LO                    0x0020a334 /* Data Transport PCR STC LSBs Register */
#define BCHP_XPT_DPCR3_ACCUM_VALUE               0x0020a35c /* Timebase Loop Filter Integrator */
#define BCHP_XPT_DPCR3_PCR_COUNT                 0x0020a360 /* Data Transport PCR Phase Error Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_CTRL             0x0020a364 /* Data Transport Soft PCR Control Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_BASE             0x0020a368 /* Data Transport Soft PCR BASE Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_EXT              0x0020a36c /* Data Transport Soft PCR Extension Register */

#endif /* #ifndef BCHP_XPT_DPCR3_H__ */

/* End of File */