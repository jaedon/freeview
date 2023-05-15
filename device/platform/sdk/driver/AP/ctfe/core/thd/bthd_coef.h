/*************************************************************************** *     
 *     (c)2005-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: bthd_coef.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 10/13/11 7:33p $
 *
 * [File Description:]
 *
 * Revision History:
 * 
 *
 ***************************************************************************/
#ifndef _BTHD_COEF_H_
#define _BTHD_COEF_H_

#define THD_Qam_N           3
#define THD_GuardInterval_N 4
#define THD_CodeRate_N      5
#define THD_FrequencyInterpolatorCoefLength   396

/***************************************************************************
  Transmission mode (FFT size) lookup table  (indexing must match enumerated BTHD_TransmissionMode_t)
****************************************************************************/
extern const uint32_t bthd_transmission_mode[];

#ifdef BTHD_ISDBT_SUPPORT
extern const uint32_t bthd_transmission_mode_isdbt[]; 
#endif

/***************************************************************************
  Guard interval (fraction) lookup table  (indexing must match enumerated BTHD_GuardInterval_t)
****************************************************************************/
extern const uint8_t bthd_guard_interval[]; 

/***************************************************************************
  Bandwidth lookup table (indexing must match enumerated BTHD_Bandwidth_t)
****************************************************************************/
extern const uint8_t bthd_bandwidth[]; 

/***************************************************************************
  Output interface control word lookup table
****************************************************************************/
extern const uint8_t bthd_rate_denominator_table[]; 

#ifdef BTHD_ISDBT_SUPPORT 
extern const uint32_t bthd_freq_coef_table[5*THD_FrequencyInterpolatorCoefLength];
#else
extern const uint32_t bthd_freq_coef_table[2*THD_FrequencyInterpolatorCoefLength];
#endif


/***************************************************************************
 * Output interface control word lookup table
 ***************************************************************************/
typedef struct bthd_oi_cw_s
{
  uint32_t  n;
  uint32_t  d;
} bthd_oi_cw_t;

/* freq_interp_polyphase_3_24, y = ce_freq_h(:,12); y = y.*exp(j*2*pi*[0:71]'*1/72); */
extern const int16_t interpI[72];
extern const int16_t interpQ[72];

extern const bthd_oi_cw_t bthd_oi_table[THD_GuardInterval_N][THD_Qam_N][THD_CodeRate_N];

/* Exponential exp(-j*2*pi*[0:255]/256) for channel span dft */
extern const int16_t dft_table[256][2];


/* #define BTHD_POWER_MANAGEMENT */



#endif /* _BTHD_COEF_H_ */
