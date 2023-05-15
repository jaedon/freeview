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
 * $brcm_Workfile: bchp_xpt_dpcr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 12:03p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 09:05:31 2008
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
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_xpt_dpcr2.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 12:03p rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_DPCR2_H__
#define BCHP_XPT_DPCR2_H__

/***************************************************************************
 *XPT_DPCR2 - XPT DPCR2 Control Registers
 ***************************************************************************/
#define BCHP_XPT_DPCR2_PID                       0x0020a200 /* Data Transport PCR PID Register */
#define BCHP_XPT_DPCR2_CTRL                      0x0020a204 /* Data Transport PCR Control Register */
#define BCHP_XPT_DPCR2_INTR_STATUS_REG           0x0020a208 /* Interrupt Status Register */
#define BCHP_XPT_DPCR2_INTR_STATUS_REG_EN        0x0020a20c /* Interrupt Status Enable Register */
#define BCHP_XPT_DPCR2_STC_EXT_CTRL              0x0020a210 /* Data Transport PCR STC Extension Control Register */
#define BCHP_XPT_DPCR2_MAX_PCR_ERROR             0x0020a220 /* Data Transport PCR Max PCR Error Register */
#define BCHP_XPT_DPCR2_SEND_BASE                 0x0020a224 /* Data Transport PCR Send Base Register */
#define BCHP_XPT_DPCR2_SEND_EXT                  0x0020a228 /* Data Transport PCR Send Extension Register */
#define BCHP_XPT_DPCR2_STC_EXT_CTRL27            0x0020a22c /* Data Transport PCR STC Extension Control Register (Test Only) */
#define BCHP_XPT_DPCR2_PWM_CTRLVALUE             0x0020a238 /* Data Transport PCR PWM Control Value Register */
#define BCHP_XPT_DPCR2_LAST_PCR_HI               0x0020a23c /* Data Transport PCR Last PCR MSBs Register */
#define BCHP_XPT_DPCR2_LAST_PCR_LO               0x0020a240 /* Data Transport PCR Last PCR LSBs Register */
#define BCHP_XPT_DPCR2_STC_BASE_LSBS             0x0020a248 /* Data Transport PCR STC Base LSBs Register */
#define BCHP_XPT_DPCR2_PHASE_ERROR               0x0020a24c /* Timebase Last Phase Error */
#define BCHP_XPT_DPCR2_LOOP_CTRL                 0x0020a250 /* Timebase Control */
#define BCHP_XPT_DPCR2_REF_PCR                   0x0020a254 /* Timebase Frequency Reference Control */
#define BCHP_XPT_DPCR2_CENTER                    0x0020a258 /* Timebase Center Frequency */
#define BCHP_XPT_DPCR2_STC_HI                    0x0020a230 /* Data Transport PCR STC MSBs Register */
#define BCHP_XPT_DPCR2_STC_LO                    0x0020a234 /* Data Transport PCR STC LSBs Register */
#define BCHP_XPT_DPCR2_ACCUM_VALUE               0x0020a25c /* Timebase Loop Filter Integrator */
#define BCHP_XPT_DPCR2_PCR_COUNT                 0x0020a260 /* Data Transport PCR Phase Error Register */
#define BCHP_XPT_DPCR2_SOFT_PCR_CTRL             0x0020a264 /* Data Transport Soft PCR Control Register */
#define BCHP_XPT_DPCR2_SOFT_PCR_BASE             0x0020a268 /* Data Transport Soft PCR BASE Register */
#define BCHP_XPT_DPCR2_SOFT_PCR_EXT              0x0020a26c /* Data Transport Soft PCR Extension Register */

#endif /* #ifndef BCHP_XPT_DPCR2_H__ */

/* End of File */