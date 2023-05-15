/***************************************************************************
 *	   (c)2007-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhdcplib_keyloader.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/6/12 5:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/src/core65nm/nonaskm/bhdcplib_keyloader.c $
 * 
 * Hydra_Software_Devel/1   9/6/12 5:52p vle
 * SW7425-3763: Remove symlinks from Magnum HDCPLIB syslib modules
 * 
 * Hydra_Software_Devel/SW7425-3763/1   8/31/12 3:37p vle
 * SW7425-3763: Remove symlinks from magnum HDCPLIB syslib
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
	
	unsigned int i;
	uint32_t KeyParameter ;
	BHDCPlib_EncryptedHdcpKeyStruct * EncryptedHdcpKeys;
	
	BDBG_ASSERT(hHDCPlib) ;

	BDBG_MSG(("Version 20080114")) ; 


	/* Get HDCP Encrypted Keys */
	EncryptedHdcpKeys = (BHDCPlib_EncryptedHdcpKeyStruct *) &hHDCPlib->stHdcpConfiguration.TxKeySet.TxKeyStructure;

	BKNI_Memset( &generateRouteKeyIO, 0, sizeof(BHSM_GenerateRouteKeyIO_t))  ;	

	for (i =0; i< BAVC_HDMI_HDCP_N_PRIVATE_KEYS; i++)
	{
		if (!(hHDCPlib->stHdcpConfiguration.RxInfo.RxBksv[i / 8] & (1 << (i % 8))))
			continue ;

		/* generate Key3 from custom key */
		generateRouteKeyIO.keyLadderType = EncryptedHdcpKeys[i].Alg;
		
		generateRouteKeyIO.rootKeySrc =  BCMD_RootKeySrc_eCusKey; 
		generateRouteKeyIO.ucSwizzle1Index = 0;
		generateRouteKeyIO.swizzleType = BCMD_SwizzleType_eSwizzle0;
		
		generateRouteKeyIO.bUseCustKeyLowDecrypt =	false ;
		generateRouteKeyIO.ucCustKeyLow = EncryptedHdcpKeys[i].cusKeySel;	 
		generateRouteKeyIO.ucKeyVarLow = EncryptedHdcpKeys[i].cusKeyVarL;	
		
		generateRouteKeyIO.bUseCustKeyHighDecrypt = false ;
		generateRouteKeyIO.ucCustKeyHigh = EncryptedHdcpKeys[i].cusKeySel;	
		generateRouteKeyIO.ucKeyVarHigh = EncryptedHdcpKeys[i].cusKeyVarH;	

		generateRouteKeyIO.keyDest= BCMD_KeyRamBuf_eKey3KeyLadder3 ;
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

		generateRouteKeyIO.unKeySlotNum =	i ; 
		errCode= BHSM_GenerateRouteKey (hHDCPlib->stDependencies.hHsm, &generateRouteKeyIO) ;	
		BDBG_MSG(("generateRouteKeyIO unStatus = 0x%08X\n", generateRouteKeyIO.unStatus)) ;
		if (errCode != 0) 
		{
			BDBG_ERR(("BHSM_GenerateRouteKey errCode: %x\n", errCode ));
			return errCode;
		}	
	}

	return( errCode );
}

