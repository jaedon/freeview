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
 * $brcm_Workfile: bchp_sds_fe_6.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/12/11 8:26p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan 12 18:40:32 2011
 *                 MD5 Checksum         6e6727f6c827233acdd395c9a9032c98
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_sds_fe_6.h $
 * 
 * Hydra_Software_Devel/1   1/12/11 8:26p pntruong
 * HW4528-2: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_FE_6_H__
#define BCHP_SDS_FE_6_H__

/***************************************************************************
 *SDS_FE_6 - SDS Frontend Register Set 6
 ***************************************************************************/
#define BCHP_SDS_FE_6_ADCPCTL                    0x000e0080 /* ADC Capture Control (Formerly, ADCDBIT, ADCPCLP, ADCPCTL) */
#define BCHP_SDS_FE_6_DCOCTL                     0x000e0088 /* DC Offset Cancellor Control (Formerly, DCOLKCTL, DCOCTL2, DCOCTL1) */
#define BCHP_SDS_FE_6_DCOI                       0x000e008c /* DCO Integrator Values (Formerly, DCO) */
#define BCHP_SDS_FE_6_IQCTL                      0x000e0094 /* I/Q Imbalance Control (Formerly, IQAMP, IQPHS) */
#define BCHP_SDS_FE_6_IQAEST                     0x000e0098 /* IQ Imbalance Amplitude Integrator Value (Formerly, AEST) */
#define BCHP_SDS_FE_6_IQPEST                     0x000e009c /* IQ Imbalance Phase Integrator Value (Formerly, PEST) */
#define BCHP_SDS_FE_6_MIXCTL                     0x000e00a0 /* Frontend Mixer Control */
#define BCHP_SDS_FE_6_DSTGCTL                    0x000e00a4 /* Destagger Control */
#define BCHP_SDS_FE_6_FILTCTL                    0x000e00a8 /* Decimating Halfband Filter/Non-decimating Filter Control (Formerly, NDFCTL,FILTCTL) */
#define BCHP_SDS_FE_6_DFCTL                      0x000e00ac /* Decimation Filter Bank Upsampling Control */
#define BCHP_SDS_FE_6_AGFCTL                     0x000e00b0 /* Automatic Fine Gain Control */
#define BCHP_SDS_FE_6_AGF                        0x000e00b4 /* AGF Gain and Threshold Control */
#define BCHP_SDS_FE_6_AIF                        0x000e00b8 /* AGF Integrator Value */
#define BCHP_SDS_FE_6_NVCTL                      0x000e00bc /* Nyquist and VID Filter Control */

#endif /* #ifndef BCHP_SDS_FE_6_H__ */

/* End of File */