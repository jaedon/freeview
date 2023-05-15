/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: bhsm_user_cmds.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/15/12 1:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7425/b0/user_misc_cmd/bhsm_user_cmds.h $
 * 
 * Hydra_Software_Devel/2   2/15/12 1:04p atruong
 * SWSECURITY-109: HMAC-SHA command support for 40-nm platforms
 * 
 * Hydra_Software_Devel/1   10/12/11 8:36p atruong
 * SW7425-1251: HSM PI Support For BCM7425B0
 * 
 * Hydra_Software_Devel/1   1/25/11 6:42p atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/


#ifndef BHSM_USER_CMDS_H__
#define BHSM_USER_CMDS_H__


#include "bsp_s_user_random_number.h"
#include "bsp_s_hmac_sha1.h"


#include "bhsm.h"
#include "bhsm_keyladder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Module Specific Functions */

#define UINT32_SIZE_IN_BYTES			4
#define BSP_RETURN_SUCESS				0
#define BHSM_MAX_RANDOM_NUM_LEN			352

typedef enum 
{
	BHSM_RandomNumber_Type_eRNGRAW    = 0,
	BHSM_RandomNumber_Type_eRNGSHA    = 1

}BHSM_RandomNumber_Type_e;


typedef struct BHSM_UserRandomNumberIO 
{

	/* In: Selects the random number generation type:
			0x00 = raw random number from RNG, i.e. BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGRAW 
			0x01 = raw random number from SHA, i.e. BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGSHA
	*/	
	BHSM_RandomNumber_Type_e		RandomNumType; 

	/* In:  specify the destination of the output
		RANDOM_NUMBER_OUTPUT_TO_HOST=0x00, Output as many random numbers as specified in the Length field below to Host; 
	*/
	unsigned char					keyDest;  


     /* In: Length of the Random Number in bytes. It should be 32-bit word aligned.
             If keyDest is RANDOM_NUMBER_OUTPUT_TO_HOST, the maximum size shall be 352 bytes.*/
	unsigned int					unDataLen;

	/* Out:  0x00 - for success, 
 	 	    	0x01 - a generic error
 	 	    	0x91 - bad parameters  */
	uint32_t						unStatus;  

	/* Out: the returned/generated random number according to the inputs*/
	unsigned char					aucData[BHSM_MAX_RANDOM_NUM_LEN];

} BHSM_UserRandomNumberIO_t;

/*****************************************************************************
Summary:

This function is provided for MIPS host to use the random number generating (RNG) engine inside BSP to produce random numbers.
TBD.

Description:

TBD.

Calling Context:

This function can be called any time after the system and BSP is initialized, when the applications decides to utilize the 
RNG engine inside BSP.


Performance and Timing:
This is a synchronous/blocking function that won'tl return until it is done or failed.  


Input:
in_handle                           - BHSM_Handle, Host Secure module handle.
inoutp_userRandomNumberIO - BHSM_UserRandomNumberIO_t.


Output:
inoutp_userRandomNumberIO - BHSM_UserRandomNumberIO_t.


Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
*****************************************************************************/
BERR_Code     BHSM_UserRandomNumber (
		BHSM_Handle					in_handle,
		BHSM_UserRandomNumberIO_t	*inoutp_userRandomNumberIO
);



#define BSP_NEED_MORE_DATA_PKE 			0xa3	 /* BCMD_STATUS_ePKEMoreData 0xa3 to 0xa8*/
#define BSP_NEED_MORE_DATA_HMACSHA1 	0xa8

#define BHSM_HMACSHA_KEY_LEN            32       /* 20 for Sha1, 28 for Sha224, 32 for Sha256 */

#define BHSM_HMACSHA_DIGEST_LEN	        32       /* in bytes, same as HMACSHA key length */

#define BHSM_HMACSHA_OP_KEY_MASK        0xFFFFFF /* 24 bits */
#define	BHSM_HMACSHA_OP_SHIFT           0x0
#define BHSM_HMACSHA_KL_SHIFT           0x8      /* bits   8-15 - Key Layer */
#define	BHSM_HMACSHA_VKL_SHIFT          0x10     /* bits 16-23 - Virtual Key Ladder */

#define BHSM_HMACSHA_TYPE_CXT_MASK      0xFFFFFF /* 24 bits */
#define BHSM_HMACSHA_CTX_SHIFT          0x0      /* bits  0 - 7 - BSP Context ID */
#define BHSM_HMACSHA_SHATYPE_SHIFT      0x8      /* bits 8 - 15 - SHA Type        */
#define BHSM_HMACSHA_DRAM_SHIFT         0x10     /* bits 16-23 - DRAM mode flag */
#define BHSM_HMACSHA_KEYINC_SHIFT       0x18     /* bits 24-31 - Key Inclusion Mode */


typedef enum 
{
	BHSM_HMACSHA_ContinualMode_eAllSeg  = 0x00,
	BHSM_HMACSHA_ContinualMode_eMoreSeg = 0x01,
	BHSM_HMACSHA_ContinualMode_eLastSeg = 0x10

}BHSM_HMACSHA_ContinualMode_e;

typedef enum 
{
	BHSM_HMACSHA_DataSource_eCmdBuff = 0,
	BHSM_HMACSHA_DataSource_eDRAM    = 1

}BHSM_HMACSHA_DataSource_e;

typedef enum 
{
	BHSM_HMACSHA_KeySource_eKeyVal    = 0,
	BHSM_HMACSHA_KeySource_eKeyLadder = 1

}BHSM_HMACSHA_KeySource_e;

typedef enum 
{
	BHSM_HMACSHA_KeyInclusion_eNo        = 0,
	BHSM_HMACSHA_KeyInclusion_eAppend    = 1

}BHSM_HMACSHA_KeyInclusion_e;


typedef struct BHSM_UserHmacShaIO 
{
	/* In: Which digest calculating mode: Sha-1/ShaX or HMAC */
	BPI_HmacSha1_Op_e			oprMode;
	/* In: Which SHA mode - Sha160(Sha1), Sha224, or Sha256  */
	BPI_HmacSha1_ShaType_e		shaType;
	/* In: Virtual Key Ladder ID - For HMAC calculation */
	BCMD_VKLID_e				VirtualKeyLadderID;
	/* In: Key Layer - For HMAC calculation */
	BCMD_KeyRamBuf_e			keyLayer;
	/* In: Which BSP internal context to use - Only 0xE or 0xF is valid here */
	BPI_HmacSha1_Context_e		contextSwitch;
	/* In: Which data source to use.  Only DRAM data is supported now */
	BHSM_HMACSHA_DataSource_e	dataSource;
	/* In: Which key source to use.  Embedded key or key ladder */
	BHSM_HMACSHA_KeySource_e	keySource;
	/* In: How to include key into calculation.  */
	BHSM_HMACSHA_KeyInclusion_e keyIncMode;
	unsigned int				unKeyLength;
	/* In: clear key, valid only for HMAC-clear-key opr mode, 20 bytes of big endian byte array */	
	unsigned char				keyData	[BHSM_HMACSHA_KEY_LEN];  
	/* In: the length of the message data for digest calculation */
	unsigned int				unInputDataLen;
	/* In: DRAM address of the buffer holding data for digest calculation */		
	unsigned char				*pucInputData;

	/* In: data passed in for HMAC or SHA digest calculation can be contained in one buffer or multiple 
	    buffers.  In the case of single buffer data, this needs to be set to  BHSM_HMACSHA_ContinualMode_eAllSeg,
	    0x00.  In the case of multiple buffer data, all of the commands except the last one need to have this
	    field set to BHSM_HMACSHA_ContinualMode_eMoreSeg, 0x01.  The last command for the last buffer
	    needs to have this field set to BHSM_HMACSHA_ContinualMode_eLastSeg. */
	BHSM_HMACSHA_ContinualMode_e	contMode;

	/* set to true to initiate HSM PI byte swapping */
	bool                            byteSwap;

	/* Out: Status of the command */
	uint32_t					unStatus;
	/* Out: size of the Digest string */
	uint32_t                    digestSize;
	/* Out: SHA or HMAC digest, max 32 byte array, big endian */	
	unsigned char				aucOutputDigest[BHSM_HMACSHA_DIGEST_LEN];  

} BHSM_UserHmacShaIO_t;

/*****************************************************************************
Summary:

This function is to calculate out a digial digest for a given message, using SHA1 or HMAC with clear key or HMAC with secure key. 

Description:

The given message can be of any length as long as the memory operation is succesful. The supported clear key is 20 bytes in length though 
the max key length can be 64 bytes theorically. The supported secure key to be generated can be 64/128/192 bits in length (normally use 128 bits). 
The output digest is 20 bytes always for any SHA1 or HMAC operation.

SHA1 operation need no any key. HMAC clear key operation need a provided key and message data in a same function call. HMAC secure key operation 
need a preceding Keyladder1 operation to generate a key5 ( via calling BHSM_GenerateRouteKey function three times, provided 3 sets of key seed/procin),
when the Key5 of KeyLadder1 is ready, call this BHSM_UserHmacSha1 function to provide the message data to BSP.


Calling Context:

- this function with SHA1 operation can be called any time after the system and BSP is initialized.
- this function with HMAC-with-clear-key can be called any time after the system and BSP is initialized.
- this function with HMAC-with-secure-key can only be called after a successful Keyladder1 operation to generate a valid Key5.
(of course the system and BSP had been initialized.)


Performance and Timing:
This is a synchronous/blocking function that won'tl return until it is done or failed.  Some bench test shows
        -  HMAC,  key=20 bytes, data =187 bytes, time =2369 us, rate =93500 Bps
        -  HMAC,  key=20 bytes, data =48128 bytes, time =339965 us, rate =141970 Bps
        -  SHA1,  no key,    data =1024 bytes, time =8376 us, rate =128000 Bps
        -  SHA1,  no key,    data =32768 bytes, time =231412 us, rate =141852 Bps


Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_userHmacSha1IO  - BHSM_UserHmacSha1IO_t, in the input section
	'oprMode' is always needed to select digest calculation mode,SHA1/0, HMAC-clear-key/1, HMAC-secure-key/2, others invalid
	      Keyladder1 operation to generate Key5 is mandatory in advance if to select HAMC-secure-key
	'contextSwitch' is always needed to pick up a BSP internal context to use:0x0c~0x0F only, others invalid.			
	'keyData' is only needed for HMAC-clear-key/1 as selected in the 1st member
	'unInputDataLen' is always needed to state the actual length of message data for digest calculation
	'pucInputData' is always needed to provide the message. If not the last piece of message data, must be multiple of 64bytes.
	
	the output section can be any value (normally input as 0).
			
Output:
inoutp_userHmacSha1IO  - BHSM_UserHmacSha1IO_t, the member 'unStatus' in output section will be modified 
to reflect the command processing status at BSP side, 0 for success, non-zero for certain error.

	'aucOutputData' contains the returned digest, 20 bytes of big endian byte array.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_GenerateRouteKey
*****************************************************************************/
BERR_Code      BHSM_UserHmacSha (
		BHSM_Handle				in_handle,
		BHSM_UserHmacShaIO_t	*inoutp_userHmacShaIO
);



/* End of Module Specific Functions */



#ifdef __cplusplus
}
#endif

#endif /* BHSM_USER_CMDS_H__ */







