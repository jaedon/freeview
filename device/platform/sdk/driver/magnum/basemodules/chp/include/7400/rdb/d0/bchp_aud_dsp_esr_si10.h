/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_aud_dsp_esr_si10.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/23/07 1:15p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jul 23 12:02:33 2007
 *                 MD5 Checksum         61f9c4d8dcdcd06017506dddbf23f434
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008004
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_aud_dsp_esr_si10.h $
 * 
 * Hydra_Software_Devel/1   7/23/07 1:15p maivu
 * PR 32842: Initial revision
 *
 ***************************************************************************/

#ifndef BCHP_AUD_DSP_ESR_SI10_H__
#define BCHP_AUD_DSP_ESR_SI10_H__

/***************************************************************************
 *AUD_DSP_ESR_SI10 - Audio DSP Interrupts Status Register (Context1 Soft Interrupts to DSP) 0
 ***************************************************************************/
#define BCHP_AUD_DSP_ESR_SI10_INT_STATUS         0x00603080 /* DSP Core Interrupt Status Register */
#define BCHP_AUD_DSP_ESR_SI10_INT_SET            0x00603084 /* DSP Core Interrupt Set Register */
#define BCHP_AUD_DSP_ESR_SI10_INT_CLEAR          0x00603088 /* DSP Core Interrupt Clear Register */
#define BCHP_AUD_DSP_ESR_SI10_MASK_STATUS        0x0060308c /* DSP Core Interrupt Mask Status Register */
#define BCHP_AUD_DSP_ESR_SI10_MASK_SET           0x00603090 /* DSP Core Interrupt Mask Set Register */
#define BCHP_AUD_DSP_ESR_SI10_MASK_CLEAR         0x00603094 /* DSP Core Interrupt Mask Clear Register */

#endif /* #ifndef BCHP_AUD_DSP_ESR_SI10_H__ */

/* End of File */