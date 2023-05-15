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
 * $brcm_Workfile:   bhsm_keyladder_enc.h$
 * $brcm_Revision:  1$
 * $brcm_Date: 1/24/11 10:10a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log:  /magnum/portinginterface/hsm/7422/a0/keyladder/bhsm_keyladder_enc.h$
 *
 * 1  1/24/11 10:10a atruong
 * SW7422-26: Add HSM PI Support for 7422A0
 *
 ******************************************************************************/


#ifndef BHSM_KEYLADDER_ENC_H__
#define BHSM_KEYLADDER_ENC_H__


/* added to remove header file dependency */
#include "bhsm.h"

#if !HSM_IS_ASKM_40NM_ZEUS_1_0
#include "bsp_s_pkl.h"
#endif



#ifdef __cplusplus
extern "C" {
#endif


#define BHSM_GEN_ROUTE_KEY_DATA_LEN	    	(((BCMD_GenKey_InCmd_eKeySize/4)*4) - ((BCMD_GenKey_InCmd_eProcIn/4)*4))
#define BHSM_KEYLADDER_CUSTSUBMODE_SHIFT	0x08
#define	BHSM_KL_ACTCODE_LEN					16

/**************************************************************************************************
Summary:

Description:
this structure defines all the required setting and key seed to generate an intermediate or final key, 
and route it into a pre-allocated key slot if required.

See Also:
BHSM_GenerateRouteKey
**************************************************************************************************/ 
typedef struct BHSM_GenerateRouteKeyIO {

	/* In: Key Ladder selection - FW, HW, or Poway */
	/*      Of type BCMD_KeyLadderSelection_e for Zeus 2.0 and above */
	unsigned int					keyLadderSelect;

	/* In: ASKM 3Des Key ladder root key swap enable */
	bool							bASKM3DesKLRootKeySwapEnable;

	/* In: Sub Command ID - to retrieve VKL association table */
	/*      BCMD_KeyGenSubCmdID_e for Zeus 1.0; BCMD_VKLAssociationQueryFlag_e for others */
	unsigned int					subCmdID;  

	/* In: 0/DES, 1/3DESABA*/
	BCMD_KeyLadderType_e			keyLadderType;	

	 /* In: needed and meaningful only for key3 generation, to select a key source,
	 	values in BCMD_RootKeySrc_e	 	*/
	BCMD_RootKeySrc_e				rootKeySrc;

	/* In: Customer Sub-Mode Select - VKLs are allocated based on these */
	BCMD_CustomerSubMode_e			customerSubMode;

	/* In: ASKM Mode Enabled - 1 : Disabled - 0 */
	bool                        	bASKMModeEnabled;

	/* In: Key Generation Mode - Generate or do not generate key */
	/*      of type BCMD_KeyGenFlag_e for Zeus 2.0 and above */
	unsigned int					keyGenMode;
	 
	/* In: needed and meaningful only for key3 generation, valid only when Swizzle1 is used */
	unsigned char					ucSwizzle1Index; 

	/* In: needed and meaningful only for key3 generation, to select a swizzle type as 
	defined in BCMD_SwizzleType_e*/
	BCMD_SwizzleType_e				swizzleType;		 

	/* In: needed and meaningful only for key3 generation,0 - encryption, 1 decryption */
	bool							bUseCustKeyLowDecrypt;  

	/* In: needed and meaningful only for key3 generation, the low bypte of custom key selection pair */
	unsigned char					ucCustKeyLow;	

	/* In: needed and meaningful only for key3 generation,0 - encryption, 1 decryption */
	bool							bUseCustKeyHighDecrypt;

	/* In: needed and meaningful only for key3 generation,the high bypte of custom key selection pair */
	unsigned char					ucCustKeyHigh;	

	/* In: needed and meaningful only for key3 generation,the low bypte of key variable pair */
	unsigned char					ucKeyVarLow;	

	/* In: needed and meaningful only for key3 generation,the high bypte of key variable pair */
	unsigned char					ucKeyVarHigh;	

	/* In: Virtual Key Ladder ID */
	BCMD_VKLID_e					virtualKeyLadderID;

	/* In: Key Layer used for key ladder key generation key3 - key5 */
	BCMD_KeyRamBuf_e				keyLayer;

	/* In: K3 Operation information */
	BCMD_Key3Op_e					key3Op;

	/* In: 0 - decryption, 1- encryption */
	bool							bIsKeyLadder3DESEncrypt;  
	
	 /* In:  how many bytes from the begging of "aucKeyData" is actually used as key seed*/
	unsigned char					ucKeyDataLen;   
	 
	 /* In: big endian byte array, to input from the 1st byte, padding at bottom*/
	unsigned char					aucKeyData[BHSM_GEN_ROUTE_KEY_DATA_LEN];  
	 
	 /* In:  the size of generated key is 64/128/192bits as defined in BCMD_KeySize_e */
	BCMD_KeySize_e					keySize;			
	 
	 /* In: route this generated into the key slot, true for final key, false for intermediate key*/
	bool							bIsRouteKeyRequired;	

	bool							bSwapAesKey;

	 /* In: which kind of key slot destination, CA/RMX/M2M/HDMI/etc, see BCMD_KeyDestBlockType_e*/
	BCMD_KeyDestBlockType_e			keyDestBlckType;	

	/* In: which entry type of key destination, odd/even key entry or IV entry or else, see 
	BCMD_KeyDestEntryType_e
	 */
	BCMD_KeyDestEntryType_e			keyDestEntryType;

	/* In: IV type for this particular key element */
	BCMD_KeyDestIVType_e			keyDestIVType;

	 /* In:  value of the allocated key slot type  */
	BCMD_XptSecKeySlot_e			caKeySlotType;		

	 /* In:  value of the allocated key slot number or, for HDMI, HDMI Key Serializer address*/
	unsigned int					unKeySlotNum;		

	 /* In:  which mode to use the key, BCMD_KeyMode_eRegular normally, see BCMD_KeyMode_e */
	BCMD_KeyMode_e					keyMode;

	/* In: The equivalent value of SC bits 0x01 for R-pipe (either odd (0x11) or clear (0x00))   */
	unsigned char               	RpipeSC01Val;
	
	/* In: The equivalent value of SC bits 0x01 for G-pipe (either odd (0x11) or clear (0x00))   */
	unsigned char               	GpipeSC01Val;

	/* In: Hardware (HW) key ladder destination algorithm */
	BCMD_XptM2MSecCryptoAlg_e		hwklDestAlg;

	/* In: HW Key Ladder length */
	/*      of Type BCMD_HwKeyLadderLength_e */
	unsigned int					hwklLength;

	/* In: 128-bit Activation code - BE array */
	unsigned char					activationCode[BHSM_KL_ACTCODE_LEN]; 
	
	 
	/* Out: 0 for success, otherwise failed */
	uint32_t						unStatus;

} BHSM_GenerateRouteKeyIO_t;



/*****************************************************************************
Summary:

This function can be used to decrypt an intermediate encrypted key and store the intermediate key
as key3, key4 or key5.  It can also route this intermediate key to CA, M2M, HDMI, IV 
and REMUX modules.

Description:

Secure key management is definitely one of the most important BSP security features.
BSP can decrypt keys and route the decrypted keys to various destination securely.
These keys can be generated through BSP key ladder, where the top root key 
can only be accessed by hardware. Using this top root key, BSP can generate different
intermediate keys, such as key3, key4 or key5.  

This function can be used to decrypt the intermediate key. If bIsRouteKeyRequired is true,
this function will also route the decrypted key to CA, M2M, HDMI, IV or REMUX modules.   
Since MIPS cannot access the intermediate keys,  this method is therefore much more secure 
than routing control word into those modules.

At any given time, BSP can support up to 3 set of key ladders. Each key ladder can store
a set of key3, key4 and key5.  However, depend on the access control matrix and 
OTP progamming, intermediate key in certain key ladder may be blocked from routing to 
certain destination.  Refer to ACL Document for the detail security policies.

Key slot of certain destination, for example CA, may contain a few keys, such as odd, even, and IV 
key.  This function can be used to decrypt only one key at a time. If the key is decrypted
and routed to the same slot entry more than once, the last call may overwrite any previous result.

The key will be kept valid in the key slot until either the key slot is invalidated by
BHSM_InvalidateKey function or the chip is reset. The loaded algorithm and control bits will be 
kept in the key slot until either the key slot is reset by BHSM_ResetKeySlotCtrlBits or the 
chip is reset.

The key order shall be in big-endianness. The length of decrypted key could be 64/128/192 bits. 
If the actual key bytes to load is shorter than 192 bits, the encrypted will be loaded at the most 
significant byte of the aucKeyData variable. For example 64-bit DES key is occupying first 8 
bytes of aucKeyData variable.

Refer to Load key documentation on how to load 3DES-ABA and 3DES-ABC keys.

Note that some systems may require to load the control word for CA and CP. BHSM_LoadRouteUserKey
can be used for this purpose.


Calling Context:

This function shall be called after BHSM_ConfigAlgorithm configure the algorithm.


Performance and Timing:
This is a synchronous/blocking function that will not return until it is done or failed. 



Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_generateRouteKeyIO  - BHSM_GenerateRouteKeyIO_t.
			
Output:
inoutp_generateRouteKeyIO  - BHSM_GenerateRouteKeyIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_LoadRouteUserKey
BHSM_FreeCAKeySlot
BHSM_AllocateCAKeySlot
BHSM_ConfigAlgorithm

*****************************************************************************/
BERR_Code   BHSM_GenerateRouteKey (
		BHSM_Handle					in_handle,
		BHSM_GenerateRouteKeyIO_t	*inoutp_generateRouteKeyIO
);


/**************************************************************************************************
Summary:

Description:
this structure defines all the required settings for retrieving the VKL-CustSubMode association table. 

See Also:
BHSM_RetrieveVKL_CustSubModeAssociation

**************************************************************************************************/ 
typedef struct BHSM_VKLCustSubModeAssociationIO
{

	/* In: Key Ladder selection - FW, HW, or Poway */
	/*      Of type BCMD_KeyLadderSelection_e for Zeus 2.0 and above */
	unsigned int					keyLadderSelect;

	/* In: ASKM 3Des Key ladder root key swap enable */
	bool							bASKM3DesKLRootKeySwapEnable;

	/* In: Sub Command ID - to retrieve VKL association table */
	/*      BCMD_KeyGenSubCmdID_e for Zeus 1.0; BCMD_VKLAssociationQueryFlag_e for others */
	unsigned int					subCmdID;  
	
	/* In: 0/DES, 1/3DESABA*/
	BCMD_KeyLadderType_e			keyLadderType;	

	 /* In: needed and meaningful only for key3 generation, to select a key source,
	 	values in BCMD_RootKeySrc_e	 	*/
	BCMD_RootKeySrc_e				rootKeySrc;

	/* In: Customer Sub-Mode Select - VKLs are allocated based on these */
	BCMD_CustomerSubMode_e			customerSubMode;

	/* In: ASKM Mode Enabled - 1 : Disabled - 0 */
	bool                        	bASKMModeEnabled;

	/* In: Key Generation Mode - Generate or do not generate key */
	/*      of type BCMD_KeyGenFlag_e for Zeus 2.0 and above */
	unsigned int					keyGenMode;
	 
	/* In: Virtual Key Ladder ID */
	BCMD_VKLID_e					virtualKeyLadderID;

	/* In: Key Layer used for key ladder key generation key3 - key5 */
	BCMD_KeyRamBuf_e				keyLayer;

	/* Out: 0 for success, otherwise failed */
	uint32_t						unStatus;

	/* Out: The following is the VKL-CustSubMode association table , an 0xFF means the VKL is free  */
	uint32_t						vklCustSubmode[BHSM_MAX_VKL];
	
} BHSM_VKLCustSubModeAssociationIO_t;



/**************************************************************************************************
Summary:
This function retrieves the VKL-CustSubMode association table.

Description:
This function retrieves the VKL-CustSubMode association table.


Calling Context:
This function is called by application or higher layer SW like Nexus.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_routeDataSectionKey  - BHSM_RouteDataSectionKeyIO_t.
			
Output:
inoutp_outeDataSectionKeyIO  - BHSM_RouteDataSectionKeyIO_t.
			
			
Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_RetrieveVKL_CustSubModeAssociation (
		BHSM_Handle							in_handle,
		BHSM_VKLCustSubModeAssociationIO_t	*inoutp_VKLCustSubModeAssociationIO
);


#define BHSM_MAX_PROC_IN_DATA_LEN		16
#define BHSM_MAX_PROC_OUT_DATA_LEN	32

/**************************************************************************************************
Summary:

Description:
this structure defines all data members to get an unwrapped key or decrypted data

See Also:
BHSM_ProcOutCmd
**************************************************************************************************/ 
typedef struct BHSM_ProcOutCmdIO {

	/* In: key from which virtual key ladder  is used to decrypt the input data or key */ 
	BCMD_VKLID_e				virtualKeyLadderID;
	
	/* In: how many bytes of the provided data in "aucProcIn" are used to do the decryption   */
	uint32_t					unProcInLen;	

	/* In: the input encrypted data or key to be decrypted, big endian, input from the top, padding
	 at the bottom */
	unsigned char				aucProcIn[BHSM_MAX_PROC_IN_DATA_LEN];

	/* Out: 0 for success, otherwise failed */
	uint32_t					unStatus;

	/* Out: the length in bytes of the returned decrypted key or data in "aucProcOut"  */
	uint32_t					unProcOutLen;	
	
	/* Out: the actual returned decrypted key or data, big endian, filled up from the 1st byte */
	unsigned char				aucProcOut[BHSM_MAX_PROC_OUT_DATA_LEN];
	
} BHSM_ProcOutCmdIO_t;


/*****************************************************************************
Summary:

This function returns a decrypted output which can be used to indirectly verify the result of 
each intermediate key in the specific key ladder.


Description:

BSP does not allow key3, key4 and key5 to be returned to the host since they can
be used in CA or CP destinations. Via this function, BSP allows the host to use key5 to 
decrypt an input data with 128-bit AES-ECB algorithm and return the result back
to the host. 

However, depending on the access control matrix and OTP progamming, this operation 
may be blocked for security reasons.  Refer to ACL Document for the detail security policies.


Calling Context:
After BHSM_GenerateRouteKey has been called three times to generate Key5, this function will 
then be called to generate output for verification.


Performance and Timing:
This is a synchronous/blocking function that will not return until it is done or failed. 


Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_procOutCmdIO  - BHSM_ProcOutCmdIO_t.

Output:
inoutp_procOutCmdIO  - BHSM_ProcOutCmdIO_t.



Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_GenerateRouteKey

*****************************************************************************/
BERR_Code   BHSM_ProcOutCmd (
		BHSM_Handle			in_handle,
		BHSM_ProcOutCmdIO_t	*inoutp_procOutCmdIO
);



/**************************************************************************************************
Summary:

Description:

See Also:
**************************************************************************************************/ 
typedef struct BHSM_PklCmdIO {

	/* In: key from which virtual key ladder  is used to decrypt the input data or key */
	/*      Of type  BCMD_PKL_SubCmd_e for Zeus 2.0 and above                           */
	unsigned int				pklSubCmdId;
	
	/* Out: 0 for success, otherwise failed */
	uint32_t					unStatus;

	
} BHSM_PklCmdIO_t;

BERR_Code BHSM_PklInit (
		BHSM_Handle			in_handle,
		BHSM_PklCmdIO_t		*inoutp_pklCmdIO
);


/* End of Module Specific Functions */

#ifdef BHSM_DS2_KEY_ROUTING

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_RouteDataSectionKeyIO members

See Also:
BHSM_DebugDumpExc
**************************************************************************************************/
typedef struct BHSM_RouteDataSectionKeyIO {

	/* In: M2M keyslot number */
	uint32_t			unKeySlotNumber;
	
	/* Out: 0 for success, otherwise failed. */
	unsigned int		unStatus;
	
} BHSM_RouteDataSectionKeyIO_t;

/**************************************************************************************************
Summary:
This function routes the Data section key to M2M keyslot.

Description:
This function routes the Data section key to M2M keyslot.

Calling Context:
This function is called by application or higher layer SW like Nexus.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_routeDataSectionKey  - BHSM_RouteDataSectionKeyIO_t.
			
Output:
inoutp_outeDataSectionKeyIO  - BHSM_RouteDataSectionKeyIO_t.
			
			
Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_RouteDataSectionKey (
		BHSM_Handle					in_handle,
		BHSM_RouteDataSectionKeyIO_t		*inoutp_RouteDataSectionKeyIO
);

#endif 

#ifdef __cplusplus
}
#endif

#endif /* BHSM_KEYLADDER_ENC_H__ */







