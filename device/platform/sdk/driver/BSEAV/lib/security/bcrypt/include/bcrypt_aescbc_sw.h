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
 * $brcm_Workfile: bcrypt_aescbc_sw.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/23/12 2:23p $
 *
 * openssl implementation for AES CBC.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/security/bcrypt/include/bcrypt_aescbc_sw.h $
 * 
 * 1   5/23/12 2:23p celysee
 * SW7425-2894: Adding common drm adn dtcp-ip
 * 
 * 1   8/16/11 2:23p cdisc
 * SW7425-1114: adding Common DRM for 7425 into AppLibs
 * 
 * dtv_v3.0/1   6/15/11 5:29p cdisc
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
 * Hydra_Software_Devel/2   2/20/07 3:29p alirezah
 * PR 25461: Update the BCRYPT API documentation for 7440 and 7401.
 * 
 * Hydra_Software_Devel/1   11/6/06 3:22p alirezah
 * PR 24954: Add BCRYPT support for 7440A0.
 * 
 * Hydra_Software_Devel/1   9/13/06 2:53p alirezah
 * PR 24240: Prepare 7401 BCRYPT library.
 * 
 * Hydra_Software_Devel/1   5/5/06 4:37p qiangye
 * PR19086:added for sw aescbc test
 *
 ***************************************************************************/

#ifndef BCRYPT_AESCBC_SW_H__
#define BCRYPT_AESCBC_SW_H__


#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
Parameters for the SW AES algorithm in the CBC mode of operation.


Description:
This Structure defines the parameters for the SW AES algorithm for the CBC 
mode of operation. It contains the input key, initialization vector, input and output streams,
and the crypto mode.


See Also:
BCrypt_AESCBCSw

****************************************************************************/
typedef struct BCRYPT_S_AESCBCSwCtrl{
	unsigned char 	*pIn;	/* In: A pointer to the input data steam */
	unsigned char *piv;  		/* In: A pointer to the CBC initialization vector. */
	unsigned char *pkey; 		/* In: A pointer to the encryption key */
	unsigned long 	len;			/* In: Length of the input data steam */
	bool bEncFlag; 			/* In: Encrypt flag: 1: encrypt, 0: decrypt */
	unsigned long keybits;  	/* In: key length in bits. 128, 192, 256 only.*/
	
	unsigned char  *pOut;		/* Out: A pointer to the encrypted output data */

} BCRYPT_S_AESCBCSwCtrl_t;




/* Basic Module Functions */


/*****************************************************************************
Summary:
This function implements the AES algorithm in the CBC mode of operation in SW.


Description:
This function implements the AES algorithm in the CBC mode of operation in software. It is 
based on the AES implementation of the openssl code. All key lengths of 128, 192, and 256 
are supported. The binaries of the openssl library is already included into the build 
environment. Since this is a software implementation for the AES algorithm in the CBC mode 
of operation, it can only be used with the clear key. 
If the application requires an encrypted key, then the key must be decrypted before sending 
in to this function call.


Calling Context:
The function shall be called from application level or from driver level, for example in Linux
during insmod.


Performance and Timing:
This is a synchronous/blocking function that will not return until it is done or failed.


Input:
hBcrypt  - BCRYPT_Handle
inoutp_AESCBCSwCtrl - BCRYPT_S_AESCBCSwCtrl_t, a ref/pointer to the parameters 
		for the SW implementation of the AES algorithm in the CBC mode of operation. 

			
Returns:
BCRYPT_STATUS_eOK - success
BCRYPT_STATUS_eFAILED - failure 


See Also:
BCrypt_AESECBSw

******************************************************************************/
BCRYPT_STATUS_eCode BCrypt_AESCBCSw( 
	BCRYPT_Handle 	hBcrypt,
	BCRYPT_S_AESCBCSwCtrl_t *inoutp_AESCBCSwCtrl
);

#ifdef __cplusplus
}
#endif

#endif 



