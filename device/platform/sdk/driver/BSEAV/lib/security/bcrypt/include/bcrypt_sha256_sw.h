/***************************************************************************
 *     (c)2006-2011 Broadcom Corporation
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
 * $brcm_Workfile: bcrypt_sha256_sw.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/23/12 2:24p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/security/bcrypt/include/bcrypt_sha256_sw.h $
 * 
 * 1   5/23/12 2:24p celysee
 * SW7425-2894: Adding common drm adn dtcp-ip
 * 
 * 1   8/16/11 2:23p cdisc
 * SW7425-1114: adding Common DRM for 7425 into AppLibs
 * 
 * dtv_v3.0/1   6/15/11 5:31p cdisc
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
 * Hydra_Software_Devel/2   2/20/07 4:33p alirezah
 * PR 25461: Update the BCRYPT API documentation for 7440 and 7401.
 * 
 * Hydra_Software_Devel/1   11/6/06 4:43p alirezah
 * PR 24954: Add BCRYPT support for 7440A0.
 * 
 * Hydra_Software_Devel/1   9/13/06 4:06p alirezah
 * PR 24240: Prepare 7401 BCRYPT library.
 * 
 * 
 * Hydra_Software_Devel/1   8/30/06 1:52a alirezah
 * First SHA256 
 * 
 *
 ***************************************************************************/
 #ifndef BCRYPT_SHA256_SW_H__
#define BCRYPT_SHA256_SW_H__


/***************************************************************************
Summary:
Parameters for SHA-256 algorithm in software.


Description:
This Structure defines the parameters for SHA-256 algorithm in software. It contains input
source data, the length on input data in bytes, SHA-256 context, inital SHA-256 context, the
generated digest, and the length of digset in bytes.

See Also:
BCrypt_Sha256Sw

****************************************************************************/
typedef struct BCRYPT_Sha256Sw {
	unsigned char 	*pucSrcData;	/* In: A pointer to the input source data */
	unsigned long 	ulSrcDataLenInByte;	/* In: Length of the input data in bytes */
	unsigned long ulctxnum; 			/* In: Sha1 context, MUST be within 0-15. */
	bool   binitctx; 					/* In: Init SHA-1 context. */
	bool   bfinal;  					/*In: Generate final digest. */

	unsigned char 	*pucDigest;		/* Out: A pointer to the output digest value */
	unsigned char	ucDigestLenInByte;	/* Out: Length of the output digest in bytes */
	
}  BCRYPT_Sha256Sw_t;



/* Basic Module Functions */


/*****************************************************************************
Summary:
This function implements the SHA-256 algorithm in SW.


Description:
This function implements the SHA-156 algorithm in software. It is 
based on the SHA-256 implementation of the openssl code. The binaries of the openssl library is 
already included into the build environment.


Calling Context:
The function shall be called from application level or from driver level, for example in Linux
during insmod.


Performance and Timing:
This is a synchronous/blocking function that will not return until it is done or failed.


Input:
hBcrypt  - BCRYPT_Handle
inoutp_sha256SwIO -BCRYPT_Sha256Sw_t, a ref/pointer to the parameters 
		for the SW implementation of the SHA-256 algorithm. 

			
Returns:
BCRYPT_STATUS_eOK - success
BCRYPT_STATUS_eFAILED - failure 


See Also:
BCrypt_Sha1Sw

******************************************************************************/
BCRYPT_STATUS_eCode BCrypt_Sha256Sw( 
		BCRYPT_Handle 	hBcrypt,
		BCRYPT_Sha256Sw_t 	*inoutp_sha256SwIO
);

#endif /* BCRYPT_SHA256_SW_H__ */

