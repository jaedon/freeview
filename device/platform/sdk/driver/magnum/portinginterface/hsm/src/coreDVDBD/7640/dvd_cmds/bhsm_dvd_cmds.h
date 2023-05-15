/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_dvd_cmds.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 11:54a $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7640/a0/dvd_cmds/bhsm_dvd_cmds.h $
 * 
 * Hydra_Software_Devel/1   2/8/11 11:54a cdisc
 * SWBLURAY-23691: branching files from 7630 dvd_cmds
 * 
 * Hydra_Software_Devel/2   9/24/09 8:33p atruong
 * SW7630-18: HSM PI Bringup on 7630B0
 * 
 * Hydra_Software_Devel/1   8/12/08 6:01p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/8   1/17/08 2:52p qxie
 * PR38806: HSM: add new commad to support BD-RE
 * 
 * Hydra_Software_Devel/7   4/13/07 10:38a lyang
 * PR 27763: HSM to add 2 new PIs to support 7440B0 and some length change
 * on 4/13/07 according to BSP change
 * 
 * Hydra_Software_Devel/6   4/3/07 4:01p lyang
 * PR 27763: HSM to add 2 new PIs to support 7440B0
 * 
 * Hydra_Software_Devel/5   4/2/07 5:35p lyang
 * PR 27763: HSM to add 2 new PIs to support 7440B0
 * 
 * Hydra_Software_Devel/4   3/8/07 3:21p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/3   3/5/07 9:10p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/2   2/13/07 7:04p lyang
 * PR 27763: HSM to add a new PI to support 7440B0, net DMA, SHARF
 * 
 * Hydra_Software_Devel/1   11/28/06 10:50a btan
 * PR 26037: Added BHSM_UnwrapMultipleContentKeys(),
 * BHSM_UpdateSecretKey() and BHSM_CalculateMediaKey() to support
 * GenerateRouteContentKeys and MKB processing commands.
 * 
 * 
 *
 ***************************************************************************/

#ifndef BHSM_DVD_CMDS_H__
#define BHSM_DVD_CMDS_H__


#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_CHIP != 7630)
#include "bsp_s_multi_key.h"
#endif
#include "bsp_s_calc_media_key.h"
#include "bsp_s_keycommon.h"

/* Module Specific Functions */


#define BHSM_MULTIPLE_CONTENT_TITLE_KEY_DATA_LEN	    (((BCMD_UserMultipleKeys_CmdInputField_eTitleKey1/4)*4) - ((BCMD_UserMultipleKeys_CmdInputField_eTitleKey0/4)*4))
#define BHSM_MULTIPLE_CONTENT_MAX_KEY_SLOT_LEN	    (((BCMD_UserMultipleKeys_CmdInputField_eContentKey0/4)*4) - ((BCMD_UserMultipleKeys_CmdInputField_eKeySlotMapBase/4)*4))
#define BHSM_MULTIPLE_CONTENT_MAX_KEY_SLOT_SIZE 	16
#define BHSM_MULTIPLE_CONTENT_CONTENT_KEY_DATA_LEN	    (((BCMD_UserMultipleKeys_CmdInputField_eContentKey1/4)*4) - ((BCMD_UserMultipleKeys_CmdInputField_eContentKey0/4)*4))

typedef struct BHSM_MultipleContentKeysIO {    /* a.k.a. UserMultipleKeys, Generate/Route Content Keys */

	/* input */			
	BCMD_KeyRamBuf_e		aucKeyID0;			/* Kvu/kvvu index, for title key A, LSB byte in first word*/
			/* Valid choices are 
				BCMD_KeyRamBuf_eSecretKey_Km      
				BCMD_KeyRamBuf_eSecretKey_Kvu
				BCMD_KeyRamBuf_eSecretKey_Kvm0
				BCMD_KeyRamBuf_eSecretKey_Kvm1
				BCMD_KeyRamBuf_eSecretKey_Kvm2
				BCMD_KeyRamBuf_eSecretKey_Kvm3
				BCMD_KeyRamBuf_eSecretKey_Kvm4
				BCMD_KeyRamBuf_eSecretKey_Kvm5
				BCMD_KeyRamBuf_eSecretKey_Kvvu0
				BCMD_KeyRamBuf_eSecretKey_Kvvu1
				BCMD_KeyRamBuf_eSecretKey_Kvvu2
				BCMD_KeyRamBuf_eSecretKey_Kvvu3
				BCMD_KeyRamBuf_eSecretKey_Kvvu4
					  
			*/
	BCMD_KeyRamBuf_e		aucKeyID1;			/* Kvu/kvvu index, for title key B,LSB byte in second word*/
			/* Valid choices are 
				BCMD_KeyRamBuf_eSecretKey_Km      
				BCMD_KeyRamBuf_eSecretKey_Kvu
				BCMD_KeyRamBuf_eSecretKey_Kvm0
				BCMD_KeyRamBuf_eSecretKey_Kvm1
				BCMD_KeyRamBuf_eSecretKey_Kvm2
				BCMD_KeyRamBuf_eSecretKey_Kvm3
				BCMD_KeyRamBuf_eSecretKey_Kvm4
				BCMD_KeyRamBuf_eSecretKey_Kvm5
				BCMD_KeyRamBuf_eSecretKey_Kvvu0
				BCMD_KeyRamBuf_eSecretKey_Kvvu1
				BCMD_KeyRamBuf_eSecretKey_Kvvu2
				BCMD_KeyRamBuf_eSecretKey_Kvvu3
				BCMD_KeyRamBuf_eSecretKey_Kvvu4
					  
			*/
			
	unsigned char 			aucTitleKeyAData[16];  /* title KeyA key seed, big endian  byte array*/
	unsigned char 			aucTitleKeyBData[16]; /* title KeyB key seed, big endian  byte array*/
	BCMD_UserMultipleKeys_e 	aesVariant;	 	/* 0 for AesG, 1 for AesEx */
	unsigned char 			aucNumContentKeys;	  /* should be no more than MaxKeySlot */

													/* looks the capacity defined here is 16, the actual used later is 8 */
	uint32_t					unTitleKeyIndexMap;	  	/* bit0 for content key 0; bitx=0 to use Title KeyA, =1 to use TitleKeyB, */		
	unsigned char 			aucKeySlotsMap[16];	  	/* list M2M keyslot numbers for corresponding content keys, [0] for contentkey0... */	

	unsigned char 			aucContentKeyData[16][16]; /* key seed, [0][16] for content key 0,  big endian key byte array*/
			
	
	/* Output */
	uint32_t			unStatus;
	

} BHSM_MultipleContentKeysIO_t; 

/*TODO: description */
BERR_Code  BHSM_UnwrapMultipleContentKeys(
		BHSM_Handle						in_handle,
		BHSM_MultipleContentKeysIO_t		*inoutp_updateMultiContentKeyIO
);

/* as of Nov 16, ly
    TODO:	once the share files are fixed, the specific numbers will be using those enums in the shared files
*/

#define BHSM_UPDATE_SECRET_KEY_DATA_LEN	    256

typedef struct BHSM_UpdateSecretKeyIO {   

	/* input */			
	BCMD_KeyRamBuf_e		eKeyId;

	 /* verfication data read from MKB file used to validate the caculated Media key */
	unsigned char 			aucKeyData[BHSM_UPDATE_SECRET_KEY_DATA_LEN];

	/* Output */
	uint32_t			unStatus;	
} BHSM_UpdateSecretKeyIO_t;

BERR_Code  BHSM_UpdateSecretKey(
		BHSM_Handle				  	in_handle,
		BHSM_UpdateSecretKeyIO_t		*inoutp_updateSecretKeyIO
);

#define BHSM_MEDIA_KEY_DATA_LEN	    (((BCMD_UserMediaKey_CmdInputField_eKCDFlag/4)*4) - ((BCMD_UserMediaKey_CmdInputField_eMediaKeyData/4)*4))
#define BHSM_MEDIA_KCD_LEN	    (((BCMD_UserMediaKey_CmdInputField_eVolumeID/4)*4) - ((BCMD_UserMediaKey_CmdInputField_eKCD/4)*4))
#define BHSM_MEDIA_KEY_VOLUME_ID_LEN	    (((BCMD_UserMediaKey_CmdInputField_eVerificationData/4)*4) - ((BCMD_UserMediaKey_CmdInputField_eVolumeID/4)*4))
#define BHSM_MEDIA_KEY_VERIFICATION_DATA_LEN	    (((BCMD_UserMediaKey_CmdInputField_eFree0/4)*4) - ((BCMD_UserMediaKey_CmdInputField_eVerificationData/4)*4))


typedef struct BHSM_CalculateMediaKeyIO {   

	/* input */			
	BCMD_KeyRamBuf_e		eDevKeyLoc;			/* In: keyladder location. Only BCMD_KeyRamBuf_eKey4KeyLadder1 is valid */

	BCMD_VKLID_e			eVirtualKeyLadder; /* In: virtual key ladder */

	uint32_t   				unUV;			/*In:  uv number for explicit subst-difference record*/
	uint32_t   				unMaskValue;	  /* In:  mask value for explicit subst-difference record*/
	uint32_t   				unMaskValuePrime;	  /* In:  mask value for Device Key Dcryption*/
	
	unsigned char 			aucMediaKeyData[16]; /* In: media Key key seed read from MKB file, big endian  byte array*/
	
	bool 					bKCDFlag;		 	/* In:  0-don't use KCD, 1 - use KCD  */
	unsigned char 			aucKCD[16];			 /* In:  KCD value read from disc */	
	unsigned char 			aucVolumeID[16];		 /* In: volume ID read from disc */	
	unsigned char 			aucVerificationData[16];	 /* In: verfication data read from MKB file used to validate the caculated Media key */		


	/* Out:   0 for success, others are failed */
	uint32_t			unStatus;
	
	/* Out:   (based on doc) to tell which media key is actually updated at BSP (Km 0/1) */
	uint32_t			unWhichKeyUpdated;

} BHSM_CalculateMediaKeyIO_t; 

/*******************************************************************************************
 * This command is used to calculate the K_m and Kvu.
 * BSP will perform the Device key derivation operation defined by Hans Hsu in the document
 * BCM-BCG-BSE-SEC-B-06-054v0.6.
 * Device key derivation uses the AES-G3 operation and the final device keys operations as defined in
 * AACS specification section 3.2.2 and 3.2.4 respectively to obtain the final device key
 *
 * The encrypted Device Key will be unwrapped using BSP generate route key command and will be stored in one of the Key ladder location.
 * The generic route key command must be called before calling this command.
 ******************************************************************************************/
BERR_Code  BHSM_CalculateMediaKey(
		BHSM_Handle				  		  in_handle,
		BHSM_CalculateMediaKeyIO_t		*inoutp_calcMediaKeyIO
);


typedef struct BHSM_CalculateUnifiedMediaKeyIO {

	/* input */
	BCMD_KeyRamBuf_e		eDevKeyLoc;			/* In: keyladder location. Only BCMD_KeyRamBuf_eKey4KeyLadder1 is valid */
	BCMD_VKLID_e			eVirtualKeyLadder; /* In: virtual key ladder */
	uint32_t   				unUV;			/*In:  uv number for explicit subst-difference record*/
	uint32_t   				unMaskValue;	  /* In:  mask value for explicit subst-difference record*/
	uint32_t   				unMaskValuePrime;	  /* In:  mask value for Device Key Dcryption*/

	unsigned char 			aucMediaKeyVariantData[16]; /* In: media Key key seed read from MKB file, big endian  byte array*/

	unsigned char 			aucNonce[16];			 /* In:  Nonce value read from disc */
	unsigned char 			aucVolumeID[16];		 /* In: volume ID read from disc */
	unsigned char 			aucVerificationData[16];	 /* In: verfication data read from MKB file used to validate the caculated Media key */


	/* Out:   0 for success, others are failed */
	uint32_t			unStatus;

	/* Out:   (based on doc) to tell which media key is actually updated at BSP (Km 0/1) */
	uint32_t			unKvn;

} BHSM_CalculateUnifiedMediaKeyIO_t;

/*******************************************************************************************
 * This command is used to calculate the K_mv, Kvvu and K_vn.
 * BSP will perform the Unified MKB processing to derive the kmv, kvn and kvvu.
 * In order to process to the Unified MKB, the secure processor must perform the
 * Device key derivation as performed by MKB processing command.
 * The key derivation uses the AES-G3 operation and the final device keys operations
 * as defined in AACS specification section 3.2.2 and 3.2.4 respectively
 * to obtain the final device key.
 *******************************************************************************************/
BERR_Code  BHSM_CalculateUnifiedMediaKey(
		BHSM_Handle				  		  in_handle,
		BHSM_CalculateUnifiedMediaKeyIO_t		*inoutp_calcUnifiedMediaKeyIO
);



/* as of Nov 16, ly
    TODO:	once the share files are fixed, the specific numbers will be using those enums in the shared files
*/

#define BCMD_KEYSAFTEROP_MAX       (16)
#define BCMD_KEYSAFTEROP_KEYDATA_MAX       (256)		/* in bytes*/

typedef struct BHSM_KeyAfterOpIO {    


	/* In: up to 16 keys permitted if  eKeySize is 8 or 16 bytes; up to 10 keys if eKeySize is 24 bytes*/
	unsigned char 		      aucNumOfKeys;	

	/* In: all these keys have the same size*/
	BCMD_KeySize_e   			eKeySize;		

	/*In:  only a subset of BCMD_KeyDestBlockType_e are legal, i.e. 
			BCMD_KeyDestBlockType_eMem2Mem=4
			BCMD_KeyDestBlockType_eSharf =8, 
	         all others will be treated as BCMD_KeyDestBlockType_eInvalid*/
	BCMD_KeyDestBlockType_e  eKeyBlkType;	
	
	/*In:  NOP, AES-128D */
	BCMD_UserLoadKeyAfterOp_OperationType_e		eOptType;
	
	/* In:  used to describe operation, RFU 
		   unOptDetails[0]  is a keyID specified by BCMD_KeyRamBuf_e,  only a subset of 
		   BCMD_KeyRamBuf_e are legal,  Km, Kvu, Kvm0~5, Kvvu0~5
			i.e. BCMD_KeyRamBuf_eSecretKey_Km ~ 	BCMD_KeyRamBuf_eSecretKey_Kvvu5
		   all others will be treated invalid, i.e. BCMD_KeyRamBuf_eInvalid_DVD.
		   unOptDetails[1~3] is reserved so far.
	*/	
	uint32_t   				unOptDetails[4];	  

	BCMD_CustomerSubMode_e	eCustSubMode;

	/* In:  M2M key slot index for those 'aucNumOfKeys' keys, big endian  byte array*/
	unsigned char 			aucKeySlotIndexMap[BCMD_KEYSAFTEROP_MAX]; 

	/* In:  M2M key slot index for these 'aucNumOfKeys' keys correspondingly, big endian,  byte array
	          not pre-slotted at a fixed location per key data, just continuously filled in from the first byte.
	          if eKeySize=24bytes, aucNumOfKeys=16, then (24*16) > 256, bumped back with a parameter error
	 */
	unsigned char 			aucKeyData[BCMD_KEYSAFTEROP_KEYDATA_MAX]; 
	

	uint32_t   				unReserved13;	  /* In:  RFU, might be used for slot type */	
	uint32_t   				unReserved14;	  /* In:  RFU, might be used for entry type */		
	uint8_t   				opData0XorWithKey[16];  /* In:  RFU*/	
	uint8_t   				opData1XorWithDataOut[16];  /* In:  RFU*/	
	uint32_t   				unFree;  		  /* In:  RFU, TODO*/
					
	
	
	uint32_t					unStatus;    /* Out:  */
	
} BHSM_KeyAfterOpIO_t; 

/*TODO: description */
BERR_Code  BHSM_LoadKeysAfterOperation(
		BHSM_Handle				  in_handle,
		BHSM_KeyAfterOpIO_t		*inoutp_loadKeyAfterOpIO
);


#define BCMD_LOADSECRETKEY_KEYSLOT_MAX		(16)
typedef struct BHSM_LoadSecretKeyIO {    

	/* In:  only a subset of BCMD_KeyRamBuf_e are legal,  Km, Kvu, Kvm0~5, Kvvu0~5
			i.e. BCMD_KeyRamBuf_eSecretKey_Km ~ 	BCMD_KeyRamBuf_eSecretKey_Kvvu5
		    all others will be treated invalid, i.e. BCMD_KeyRamBuf_eInvalid_DVD.
	*/
	BCMD_KeyRamBuf_e		eKeyId; 
		
	/*In:  only a subset of BCMD_KeyDestBlockType_e are legal, i.e. 
			BCMD_KeyDestBlockType_eMem2Mem=4
			BCMD_KeyDestBlockType_eSharf =8, 
	         all others will be treated as BCMD_KeyDestBlockType_eInvalid*/
	BCMD_KeyDestBlockType_e  eKeyBlkType;	

	BCMD_CustomerSubMode_e	eCustSubMode;

	/* In: keyslot number for M2M;  TBD for SHARF*/
	unsigned int 		      		unKeySlot;

	uint32_t   				unReserved[1];  /* In:  RFU, */
	
	/* Out: 0 for success, otherwise failed  */
	uint32_t					unStatus;   
	
} BHSM_LoadSecretKeyIO_t; 


/*TODO:  this is a special backdoor command at development stage for the convenince of testing.
              It will be disabled later and will NOT be released to any customers
*/
BERR_Code  BHSM_LoadSecretKey(
		BHSM_Handle				 	 in_handle,
		BHSM_LoadSecretKeyIO_t		*inoutp_loadSecretKeyIO
);




/* End of Module Specific Functions */


#ifdef __cplusplus
}
#endif

#endif /* BHSM_DVD_CMDS_H__ */







