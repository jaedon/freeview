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
 * $brcm_Workfile: bchp_xpt_dpcr3.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/17/12 6:06p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 17 13:26:49 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b2/bchp_xpt_dpcr3.h $
 * 
 * Hydra_Software_Devel/1   1/17/12 6:06p vanessah
 * SW7425-2202: add B2 headers
 *
 ***************************************************************************/

#ifndef BCHP_XPT_DPCR3_H__
#define BCHP_XPT_DPCR3_H__

/***************************************************************************
 *XPT_DPCR3 - XPT DPCR3 Control Registers
 ***************************************************************************/
#define BCHP_XPT_DPCR3_PID_CH                    0x00907180 /* Data Transport PCR PID Channel Register */
#define BCHP_XPT_DPCR3_CTRL                      0x00907184 /* Data Transport PCR Control Register */
#define BCHP_XPT_DPCR3_INTR_STATUS_REG           0x00907188 /* Interrupt Status Register */
#define BCHP_XPT_DPCR3_INTR_STATUS_REG_EN        0x0090718c /* Interrupt Status Enable Register */
#define BCHP_XPT_DPCR3_STC_EXT_CTRL              0x00907190 /* Data Transport PCR STC Extension Control Register */
#define BCHP_XPT_DPCR3_MAX_PCR_ERROR             0x009071a0 /* Data Transport PCR Max PCR Error Register */
#define BCHP_XPT_DPCR3_SEND_BASE                 0x009071a4 /* Data Transport PCR Send Base Register */
#define BCHP_XPT_DPCR3_SEND_EXT                  0x009071a8 /* Data Transport PCR Send Extension Register */
#define BCHP_XPT_DPCR3_STC_EXT_CTRL27            0x009071ac /* Data Transport PCR STC Extension Control Register (Test Only) */
#define BCHP_XPT_DPCR3_PWM_CTRLVALUE             0x009071b8 /* Data Transport PCR PWM Control Value Register */
#define BCHP_XPT_DPCR3_LAST_PCR_HI               0x009071bc /* Data Transport PCR Last PCR MSBs Register */
#define BCHP_XPT_DPCR3_LAST_PCR_LO               0x009071c0 /* Data Transport PCR Last PCR LSBs Register */
#define BCHP_XPT_DPCR3_STC_BASE_LSBS             0x009071c8 /* Data Transport PCR STC Base LSBs Register */
#define BCHP_XPT_DPCR3_PHASE_ERROR               0x009071cc /* Timebase Last Phase Error */
#define BCHP_XPT_DPCR3_LOOP_CTRL                 0x009071d0 /* Timebase Control */
#define BCHP_XPT_DPCR3_REF_PCR_PRESCALE          0x009071d4 /* Timebase Frequency Reference Prescale Control */
#define BCHP_XPT_DPCR3_REF_PCR_INC               0x009071d8 /* Timebase Frequency Reference Increment Control */
#define BCHP_XPT_DPCR3_CENTER                    0x009071dc /* Timebase Center Frequency */
#define BCHP_XPT_DPCR3_STC_HI                    0x009071b0 /* Data Transport PCR STC MSBs Register */
#define BCHP_XPT_DPCR3_STC_LO                    0x009071b4 /* Data Transport PCR STC LSBs Register */
#define BCHP_XPT_DPCR3_ACCUM_VALUE               0x009071e0 /* Timebase Loop Filter Integrator */
#define BCHP_XPT_DPCR3_PCR_COUNT                 0x009071e4 /* Data Transport PCR Phase Error Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_CTRL             0x009071e8 /* Data Transport Soft PCR Control Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_BASE             0x009071ec /* Data Transport Soft PCR BASE Register */
#define BCHP_XPT_DPCR3_SOFT_PCR_EXT              0x009071f0 /* Data Transport Soft PCR Extension Register */
#define BCHP_XPT_DPCR3_PHASE_ERROR_CLAMP         0x009071f4 /* Timebase Phase Error Control */

#endif /* #ifndef BCHP_XPT_DPCR3_H__ */

/* End of File */