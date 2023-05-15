/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib_keyloader.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 9/6/12 5:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/src/core65nm/askm/bhdcplib_keyloader.c $
 * 
 * Hydra_Software_Devel/10   9/6/12 5:51p vle
 * SW7425-3763: Remove symlinks from Magnum HDCPLIB syslib modules
 * 
 * Hydra_Software_Devel/SW7425-3763/1   8/31/12 3:27p vle
 * SW7425-3763: Remove symlinks from magnum HDCPLIB syslib Remove other
 * implementation of keyloader from this file
 * 
 * Hydra_Software_Devel/9   7/6/10 4:24p vle
 * SW7420-874: Update print out message indicating which version of
 * hdcplib keyloader is being used.
 * 
 * Hydra_Software_Devel/8   7/2/10 5:54p vle
 * SW7420-874: 7410 shares the same HSM code base as 7420 B0
 * 
 * Hydra_Software_Devel/7   6/3/10 6:31p vle
 * SW7550-452: Update hdcplib keyloader as latest 7550 HSM code has
 * changed.
 * 
 * Hydra_Software_Devel/6   1/18/10 4:59p vle
 * SW7550-102: Add new HDCP keyloader routine for 7550/7408.
 * 
 * Hydra_Software_Devel/5   12/9/09 1:56a vle
 * SW7468-18: Add HDCPLIB support for 7468, 7408, 7208 and 7550
 * 
 * Hydra_Software_Devel/4   11/6/09 11:12a vle
 * SW7125-68: uses new keyloading process for new HSM
 * 
 * Hydra_Software_Devel/3   4/10/09 4:41p vle
 * PR47665: Update hdcplib keyloader routine to use new HSM PI
 * 
 * Hydra_Software_Devel/2   3/18/08 7:18p vle
 * PR 39991: Rename HDMIlib to HDCPlib. Remove unused implementation.
 * 
 * Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
 * PR 39991: Renaming new HDMlib to HDCPlib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/3   2/21/08 5:01p vle
 * PR 31924: Update to use the update BHDCPlib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/2   2/15/08 10:03p vle
 * PR 31924: Update to use the improved HDMI Syslib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/1   1/31/08 10:36p vle
 * PR 31924: Add implementation of BHDCPLIB_FastLoadEncryptedHdcpKeys
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bhdcplib_keyloader.h"

#include "bhsm.h"
#include "bhsm_keyladder.h"
#include "bhsm_keyladder_enc.h"

BDBG_MODULE(BHDCPLIB_KEYLOADER) ;


BERR_Code BHDCPlib_FastLoadEncryptedHdcpKeys(
	BHDCPlib_Handle hHDCPlib)
{
	BERR_Code errCode = BERR_SUCCESS;
#if DEBUG_PROCOUT	
	BHSM_ProcOutCmdIO_t procOutCmdIO;	
#endif
	BHSM_GenerateRouteKeyIO_t		generateRouteKeyIO;
	BHSM_ConfigKeySlotIDDataIO_t	configKeySlotIDDataIO;
	
	unsigned int i;
	uint32_t KeyParameter ;
	BHDCPlib_EncryptedHdcpKeyStruct * EncryptedHdcpKeys;
	
	BDBG_ASSERT(hHDCPlib) ;

	BDBG_ERR(("Version 20090403 - HDCPlib keyloader for 97125/97420/97410/97340/97342/97468/97208")) ; 


	/* Get HDCP Encrypted Keys */
	EncryptedHdcpKeys = (BHDCPlib_EncryptedHdcpKeyStruct *) &hHDCPlib->stHdcpConfiguration.TxKeySet.TxKeyStructure;

	BKNI_Memset( &generateRouteKeyIO, 0, sizeof(BHSM_GenerateRouteKeyIO_t))  ;	
	BKNI_Memset( &configKeySlotIDDataIO, 0, sizeof(BHSM_ConfigKeySlotIDDataIO_t));

	for (i =0; i< BAVC_HDMI_HDCP_N_PRIVATE_KEYS; i++)
	{
		if (!(hHDCPlib->stHdcpConfiguration.RxInfo.RxBksv[i / 8] & (1 << (i % 8))))
			continue ;

		/* Call BHSM_configKeySlotIDData() to set up ID part of  configuration odd key */
		configKeySlotIDDataIO.keyDestBlckType  = BCMD_KeyDestBlockType_eCA;
		configKeySlotIDDataIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;
		configKeySlotIDDataIO.unKeySlotNum	  = i;
		configKeySlotIDDataIO.caKeySlotType    = BCMD_XptSecKeySlot_eType0;
		configKeySlotIDDataIO.CAVendorID	= 0x1234;
		configKeySlotIDDataIO.STBOwnerIDSelect = BCMD_STBOwnerID_eOTPVal;
		configKeySlotIDDataIO.ModuleID	   = BCMD_ModuleID_eModuleID_3;
		configKeySlotIDDataIO.TestKey2Select   = 0;

		errCode = BHSM_ConfigKeySlotIDData(hHDCPlib->stDependencies.hHsm, &configKeySlotIDDataIO); 
		if (errCode != 0) 
		{
			BDBG_ERR(("BHSM_ConfigKeySlotIDData errCode: %x\n", errCode));
			goto done;
		} 

		/* generate Key3 from custom key */
		generateRouteKeyIO.keyLadderType = EncryptedHdcpKeys[i].Alg;
		
		generateRouteKeyIO.rootKeySrc =  BCMD_RootKeySrc_eCusKey; 
		generateRouteKeyIO.customerSubMode = BCMD_CustomerSubMode_eGeneralPurpose1;
		generateRouteKeyIO.ucSwizzle1Index = 0;
		generateRouteKeyIO.swizzleType = BCMD_SwizzleType_eSwizzle0;
		
		generateRouteKeyIO.bUseCustKeyLowDecrypt =	false ;
		generateRouteKeyIO.ucCustKeyLow = EncryptedHdcpKeys[i].cusKeySel;	 
		generateRouteKeyIO.ucKeyVarLow = EncryptedHdcpKeys[i].cusKeyVarL;	
		
		generateRouteKeyIO.bUseCustKeyHighDecrypt = false ;
		generateRouteKeyIO.ucCustKeyHigh = EncryptedHdcpKeys[i].cusKeySel;	
		generateRouteKeyIO.ucKeyVarHigh = EncryptedHdcpKeys[i].cusKeyVarH;	

		generateRouteKeyIO.virtualKeyLadderID = BCMD_VKL0;
		generateRouteKeyIO.keyLayer = BCMD_KeyRamBuf_eKey3 ;
		generateRouteKeyIO.bIs3DESDecrypt = false ; /* true - encryption, false - decryption */

		generateRouteKeyIO.keyLadderType = BCMD_KeyLadderType_e3DESABA;
		
		generateRouteKeyIO.keySize= BCMD_KeySize_e64;
		generateRouteKeyIO.ucKeyDataLen = 8 ;
		
		BKNI_Memset(&generateRouteKeyIO.aucKeyData, 0, sizeof(generateRouteKeyIO.aucKeyData));				
		KeyParameter = EncryptedHdcpKeys[i].HdcpKeyHi;
		generateRouteKeyIO.aucKeyData[0]  =  (KeyParameter >> 24) & 0xff ;
		generateRouteKeyIO.aucKeyData[1]  =  (KeyParameter >> 16) & 0xff ;
		generateRouteKeyIO.aucKeyData[2]  =  (KeyParameter >>	8) & 0xff ;
		generateRouteKeyIO.aucKeyData[3]  =  (KeyParameter) 		  & 0xff ;
		
		KeyParameter = EncryptedHdcpKeys[i].HdcpKeyLo;
		generateRouteKeyIO.aucKeyData[4]  =  (KeyParameter >> 24) & 0xff ;
		generateRouteKeyIO.aucKeyData[5]  =  (KeyParameter >> 16) & 0xff ;
		generateRouteKeyIO.aucKeyData[6]  =  (KeyParameter >>  8) & 0xff ;
		generateRouteKeyIO.aucKeyData[7]  =  (KeyParameter) 		  & 0xff ;					
			
		generateRouteKeyIO.bIsRouteKeyRequired = true;
		generateRouteKeyIO.keyDestBlckType = BCMD_KeyDestBlockType_eHdmi;

		generateRouteKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;	

		generateRouteKeyIO.caKeySlotType = BCMD_XptSecKeySlot_eType0 ;
		generateRouteKeyIO.keyMode = BCMD_KeyMode_eRegular;

		generateRouteKeyIO.unKeySlotNum =	i ; 
		errCode= BHSM_GenerateRouteKey (hHDCPlib->stDependencies.hHsm, &generateRouteKeyIO) ;	
		BDBG_MSG(("generateRouteKeyIO key 3 unStatus = 0x%08X\n", generateRouteKeyIO.unStatus)) ;
		if (errCode != 0) 
		{
			BDBG_ERR(("BHSM_GenerateRouteKey key3 errCode: %x\n", errCode ));
			goto done;
		}	
	}

done:
	return( errCode );
}



