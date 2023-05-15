/***************************************************************************
 *     (c)2003-2011 Broadcom Corporation
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
 * $brcm_Workfile: bcrypt_rng_sw.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/23/12 2:24p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/security/bcrypt/include/bcrypt_rng_sw.h $
 * 
 * 1   5/23/12 2:24p celysee
 * SW7425-2894: Adding common drm adn dtcp-ip
 * 
 * 1   8/16/11 2:23p cdisc
 * SW7425-1114: adding Common DRM for 7425 into AppLibs
 * 
 * dtv_v3.0/1   6/15/11 5:30p cdisc
 * SWDTV-7493:[ see Broadcom Issue Tracking JIRA for more info ].
 *  General_Comments: This checkin adds support for Openssl functionality
 *  and multithreading
 * File_Specific_Comments:
 * Users should see drm_common_swcrypto.h for new API definitions.
 * Reviewed_By:
 * QA_Concerns:
 * What_Else_Affected: N/A
 * Unit_Testing_Performed: Netflix/WV work on CARD and Vizio 35230.
 * Review_Comments: N/A
 * 
 * dev_dtv_v3.0_SWDTV-7493/1   6/15/11 11:39a cdisc
 * SWDTV-7493: adding openssl and multithread support
 * 
 * Hydra_Software_Devel/2   2/20/07 4:29p alirezah
 * PR 25461: Update the BCRYPT API documentation for 7440 and 7401.
 * 
 * Hydra_Software_Devel/1   11/6/06 4:36p alirezah
 * PR 24954: Add BCRYPT support for 7440A0.
 * 
 * Hydra_Software_Devel/1   9/13/06 4:00p alirezah
 * PR 24240: Prepare 7401 BCRYPT library.
 * 
 * Hydra_Software_Devel/1   2/6/06 5:34p qiangye
 * PR19086:initial bcrypt checkin
 *
 ***************************************************************************/
#ifndef BCRYPT_RNG_SW_H__
#define BCRYPT_RNG_SW_H__


#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
Parameters for the SW RNG algorithm.


Description:
This Structure defines the parameters for the SW RNG algorithm. It contains the pointer to
the data buffer and the length of random number in bytes.


See Also:
BCrypt_RNGSw

****************************************************************************/
typedef struct BCRYPT_S_RNGSwCtrl {
	unsigned char *pucBuf; 		/* In: A pointer to the data buffer. */
	unsigned long ulLen; 			/* In: Length of random number in bytes. */
} BCRYPT_S_RNGSwCtrl_t;



/* Basic Module Functions */


/*****************************************************************************
Summary:
This function implements the RNG algorithm in SW.


Description:
This function implements the random numnber generation process in software. It is 
based on the RNG implementation of the openssl code. The binaries of the openssl library is 
already included into the build environment.


Calling Context:
The function shall be called from application level or from driver level, for example in Linux
during insmod.


Performance and Timing:
This is a synchronous/blocking function that will not return until it is done or failed.


Input:
hBcrypt  - BCRYPT_Handle
inoutp_RNGSwIO - BCRYPT_S_RNGSwCtrl_t, a ref/pointer to the parameters 
		for the SW implementation of the random number generation.

			
Returns:
BCRYPT_STATUS_eOK - success
BCRYPT_STATUS_eFAILED - failure 


See Also:
BCrypt_RNGHw

******************************************************************************/
BCRYPT_STATUS_eCode BCrypt_RNGSw( 
		BCRYPT_Handle 	hBcrypt,
		BCRYPT_S_RNGSwCtrl_t 	*inoutp_RNGSwIO
);

#ifdef __cplusplus
}
#endif

#endif 

