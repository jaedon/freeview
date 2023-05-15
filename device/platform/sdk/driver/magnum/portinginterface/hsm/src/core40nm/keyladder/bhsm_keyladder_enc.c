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
 * $brcm_Workfile:  bhsm_keyladder_enc.c$
 * $brcm_Revision:  1$
 * $brcm_Date:  1/24/11 10:10a$
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log:  /magnum/portinginterface/hsm/7422/a0/keyladder/bhsm_keyladder_enc.c$
 *
 * 1   1/24/11 10:10a atruong
 * SW7422-26: Add HSM PI Support for 7422A0
 * 2   8/04/11  4:30p  atruong
 * SWSECURITY-53: Add keyDestIVType to BSP GenerateRouteKey command block
 *
 ******************************************************************************/


#include "bhsm_keyladder_enc.h" 

#ifdef __KERNEL__
#define printf printk
#endif


BDBG_MODULE(BHSM);



vklCustSubModeAssoc_t	vklCustAssocTable[BHSM_MAX_VKL];



bool BHSM_VKLCustSubModeAssociate(
		BCMD_CustomerSubMode_e		custSubMode,
		BCMD_VKLID_e				*pVKLID
)
{

	uint32_t		index = 0, entryAvail = 0, entryCust = 0;
	bool			foundAvail = false;
	bool			foundCust  = false;
	bool			retVal     = false;

	if (*pVKLID == BHSM_MAX_VKL)
	{
		/* first try to find an existing association */
		for (index =0; index < BHSM_MAX_VKL; index++)
		{
			if (!vklCustAssocTable[index].bAssociated)    /* available VKL entry to possibly use */
			{
				entryAvail = index;
				foundAvail = true;
			}
			if ((vklCustAssocTable[index].custSubMode == custSubMode) &&
			    (vklCustAssocTable[index].bAssociated))                  /* exisiting association */
			{
				entryCust = index;
				foundCust = true;
				break;
			}
		}
		/* try to use existing association */
		if (foundCust)
		{
			*pVKLID = entryCust;
			retVal  = true;
		}
		else if (foundAvail)         /* no existing association; try using any available */
		{
			*pVKLID = entryAvail;
			retVal  = true;
		}
		/* No more VKL avaliable.  Fail the request. */

	}
	else
	{
		/* try to see if this requested VKL is available */
		if (vklCustAssocTable[*pVKLID].bAssociated)                     /* already associated */
		{
			if (custSubMode != vklCustAssocTable[*pVKLID].custSubMode)  /* to a different custSubMode; have to reject this request */
			{
				retVal = false;
			}
			else                                                        /* to the same custSubMode; OK the request */
			{
				retVal = true;
			}
		}
		else                                                            /* not yet associated */
		{
			retVal = true;                                              /* OK the request.  Table updated if cmd is successful */
		}
		
	}

	return (retVal);

}


BERR_Code BHSM_GenerateRouteKey(
		BHSM_Handle					in_handle,
		BHSM_GenerateRouteKeyIO_t	*inoutp_generateRouteKeyIO
)
{
	BERR_Code						errCode = BERR_SUCCESS;
	BHSM_ChannelHandle				channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 						unParamLen = 0, 
									i , 
									j, 
									unTmp;
	BHSM_P_CommandData_t			commandData;
	unsigned char					ucKeySlotStartOffset = 0 ;
	BHSM_P_M2MKeySlotAlgorithm_t	*pM2MKeySlotAlgorithm = NULL;
	BHSM_P_XPTKeySlotAlgorithm_t	*pXPTKeySlotAlgorithm = NULL;
	BHSM_P_KeySlotIDData_t			*pKeySlotIDData = NULL;
	uint32_t						ASKMEnabled = 0;
	bool							bVKLAssociationQuery = false;

	unsigned int					unKeySlotNum  = inoutp_generateRouteKeyIO->unKeySlotNum;
	



	BDBG_ENTER(BHSM_GenerateRouteKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_generateRouteKeyIO );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_GENERATE_ROUTE_KEY;
	commandData.unContMode = 0;

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyLadderType/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			((inoutp_generateRouteKeyIO->keyLadderType & 0xFF)  |
			 ((inoutp_generateRouteKeyIO->subCmdID & 0xFF) << 8) |
	         ((inoutp_generateRouteKeyIO->bASKM3DesKLRootKeySwapEnable & 0xFF) << 16) |
	         ((inoutp_generateRouteKeyIO->keyLadderSelect & 0xFF) << 24) );
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eRootKeySrc/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyLadderType/4) * 4;

#if HSM_IS_ASKM_40NM_ZEUS_1_0
	if (inoutp_generateRouteKeyIO->subCmdID == (unsigned int)BCMD_KeyGenSubCmdID_eVKLAssociationQuery)
#else
	if (inoutp_generateRouteKeyIO->subCmdID == (unsigned int)BCMD_VKLAssociationQueryFlag_eQuery)
#endif
	{
		bVKLAssociationQuery = true;
		/* send the command now.  Other parameters are not required */
		goto BHSM_P_SUBMIT_CMD;
	}
	/* check for the settings of VKL and CustomerSubMode */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_generateRouteKeyIO->virtualKeyLadderID > BHSM_MAX_VKL) );

	/* The app wants to have its own choice of VKL-CustomerSubMode association,*/
	/* or it wants HSM PI to select an available VKL for it (virtualKeyLadderID == BCMD_VKL_eMax*/
	if (!BHSM_VKLCustSubModeAssociate(inoutp_generateRouteKeyIO->customerSubMode, &(inoutp_generateRouteKeyIO->virtualKeyLadderID)))
	{
		/* VKL Association/Allocation fails; return result back to app */
		errCode = BHSM_STATUS_VKL_ASSOCIATION_ERR;
		goto BHSM_P_DONE_LABEL;
	}

	/* Get pointer to structure BHSM_P_KeySlotAlgorithm_t for this keyslot */
	/* and pointer to structure BHSM_P_KeySlotIDData */
	switch (inoutp_generateRouteKeyIO->keyDestBlckType)
	{
		case BCMD_KeyDestBlockType_eMem2Mem :

			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );
			pM2MKeySlotAlgorithm = &(in_handle->aunM2MKeySlotInfo[unKeySlotNum].aKeySlotAlgorithm[inoutp_generateRouteKeyIO->keyDestEntryType]);
			pKeySlotIDData	     = &(in_handle->aunM2MKeySlotInfo[unKeySlotNum].KeySlotIDData[inoutp_generateRouteKeyIO->keyDestEntryType]);
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_UNCONFIGURED_KEYSLOT_ERR, 
				(pM2MKeySlotAlgorithm->ulM2MControlWordHi == 0xFFFFFFFF) );
			break;

		case BCMD_KeyDestBlockType_eCA :
		case BCMD_KeyDestBlockType_eCPDescrambler :
		case BCMD_KeyDestBlockType_eCPScrambler :

			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= in_handle->aunCAKeySlotTypeInfo[inoutp_generateRouteKeyIO->caKeySlotType].unKeySlotNum) );
			ucKeySlotStartOffset = 
				in_handle->aunCAKeySlotTypeInfo[inoutp_generateRouteKeyIO->caKeySlotType].ucKeySlotStartOffset;
			pXPTKeySlotAlgorithm = &(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_generateRouteKeyIO->keyDestEntryType]);
			pKeySlotIDData	     = &(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].KeySlotIDData[inoutp_generateRouteKeyIO->keyDestEntryType]);
			
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_UNCONFIGURED_KEYSLOT_ERR, 
				((pXPTKeySlotAlgorithm->ulCAControlWordHi  == 0xFFFFFFFF) &&
				 (pXPTKeySlotAlgorithm->ulCPDControlWordHi == 0xFFFFFFFF) &&
				 (pXPTKeySlotAlgorithm->ulCPSControlWordHi == 0xFFFFFFFF))	);
			break;

		default :
			/* no checking for other types; no key slot associated */
			break;
	}
	

	if (inoutp_generateRouteKeyIO->bASKMModeEnabled)
		ASKMEnabled = 1;
	

	unTmp = inoutp_generateRouteKeyIO->rootKeySrc | 
	        (inoutp_generateRouteKeyIO->customerSubMode << BHSM_KEYLADDER_CUSTSUBMODE_SHIFT) |
			(ASKMEnabled << 16) | 
	        (inoutp_generateRouteKeyIO->keyGenMode << 24);
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eRootKeySrc/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eSwizzle1IndexSel/4) * 4
		 - (BCMD_GenKey_InCmd_eRootKeySrc/4) * 4;

	/* limit Swizzle1 Index to 5 bits only */
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eSwizzle1IndexSel/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->ucSwizzle1Index & 0x1F;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eSwizzleType/4) * 4
		 - (BCMD_GenKey_InCmd_eSwizzle1IndexSel/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eSwizzleType/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->swizzleType;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eCusKeySelL/4) * 4
		 - (BCMD_GenKey_InCmd_eSwizzleType/4) * 4;

	unTmp = inoutp_generateRouteKeyIO->ucCustKeyLow | 
		( inoutp_generateRouteKeyIO->bUseCustKeyLowDecrypt << 6);
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCusKeySelL/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyVarL/4) * 4
		 - (BCMD_GenKey_InCmd_eCusKeySelL/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyVarL/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->ucKeyVarLow;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eCusKeySelH/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyVarL/4) * 4;	

	unTmp = inoutp_generateRouteKeyIO->ucCustKeyHigh | 
		( inoutp_generateRouteKeyIO->bUseCustKeyHighDecrypt << 6);
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCusKeySelH/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyVarH/4) * 4
		 - (BCMD_GenKey_InCmd_eCusKeySelH/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyVarH/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->ucKeyVarHigh;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eVKLID/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyVarH/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eVKLID/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->virtualKeyLadderID;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyLayer/4) * 4
		 - (BCMD_GenKey_InCmd_eVKLID/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyLayer/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			(inoutp_generateRouteKeyIO->keyLayer |
			 (inoutp_generateRouteKeyIO->key3Op << 8));
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyLadderOpera/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyLayer/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyLadderOpera/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->bIsKeyLadder3DESEncrypt;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eProcIn/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyLadderOpera/4) * 4;

	for (i=0, 
		j = (BCMD_GenKey_InCmd_eProcIn/4 - BCMD_GenKey_InCmd_eKeyLadderType/4)
			+ ( BHSM_GEN_ROUTE_KEY_DATA_LEN-inoutp_generateRouteKeyIO->ucKeyDataLen)/4;
		i < inoutp_generateRouteKeyIO->ucKeyDataLen; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_generateRouteKeyIO->aucKeyData[i]   << 24 |
			inoutp_generateRouteKeyIO->aucKeyData[i+1] << 16 |
			inoutp_generateRouteKeyIO->aucKeyData[i+2] << 8 |
			inoutp_generateRouteKeyIO->aucKeyData[i+3]  ;
			
	}

	/* 12/14: Fixed command length */
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySize/4) * 4
		 - (BCMD_GenKey_InCmd_eProcIn/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySize/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keySize;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eRouteKeyFlag/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySize/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eRouteKeyFlag/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			(inoutp_generateRouteKeyIO->bIsRouteKeyRequired | 
			 ( inoutp_generateRouteKeyIO->bSwapAesKey << 8 ) );
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eBlkType/4) * 4
		 - (BCMD_GenKey_InCmd_eRouteKeyFlag/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eBlkType/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keyDestBlckType;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eEntryType/4) * 4
		 - (BCMD_GenKey_InCmd_eBlkType/4) * 4;
	
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eEntryType/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			(inoutp_generateRouteKeyIO->keyDestEntryType |
			 ( inoutp_generateRouteKeyIO->keyDestIVType << 8 ) );
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySlotType/4) * 4
		 - (BCMD_GenKey_InCmd_eEntryType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySlotType/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->caKeySlotType;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySlotNumber/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySlotType/4) * 4;
   
	/* if dest block is HDMI, do the port emun - HDMI serializer address conversion */
	if (inoutp_generateRouteKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eHdmi)
	{
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySlotNumber/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				BHSM_P_HDMIEnumToSerializerAddress(inoutp_generateRouteKeyIO->unKeySlotNum);
	}
	else
	{
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySlotNumber/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				inoutp_generateRouteKeyIO->unKeySlotNum;
	}
	
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyMode/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySlotNumber/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyMode/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			(inoutp_generateRouteKeyIO->keyMode |
			 (inoutp_generateRouteKeyIO->RpipeSC01Val << 8) |
			 (inoutp_generateRouteKeyIO->GpipeSC01Val << 16));
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eCtrlWord0/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyMode/4) * 4;		

	if ((inoutp_generateRouteKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eCA)            ||
	    (inoutp_generateRouteKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPDescrambler) || 
	    (inoutp_generateRouteKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPScrambler)     )
	{

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord0/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pXPTKeySlotAlgorithm->ulCAControlWordHi;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord1/4) * 4
		 	 - (BCMD_GenKey_InCmd_eCtrlWord0/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord1/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pXPTKeySlotAlgorithm->ulCAControlWordLo;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord2/4) * 4
		 	 - (BCMD_GenKey_InCmd_eCtrlWord1/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord2/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pXPTKeySlotAlgorithm->ulCPSControlWordHi;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord3/4) * 4
		 	 - (BCMD_GenKey_InCmd_eCtrlWord2/4) * 4;

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord3/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pXPTKeySlotAlgorithm->ulCPSControlWordLo;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord4/4) * 4
		 	 - (BCMD_GenKey_InCmd_eCtrlWord3/4) * 4;

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord4/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pXPTKeySlotAlgorithm->ulCPDControlWordHi;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord5/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord4/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord5/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pXPTKeySlotAlgorithm->ulCPDControlWordLo;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eSTBOwnerIDSel/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord5/4) * 4;
	}
	else if (inoutp_generateRouteKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eMem2Mem)
	{
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord0/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pM2MKeySlotAlgorithm->ulM2MControlWordHi;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord1/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord0/4) * 4;
		
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord1/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pM2MKeySlotAlgorithm->ulM2MControlWordLo;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord2/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord1/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord2/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord3/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord2/4) * 4;

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord3/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord4/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord3/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord4/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eCtrlWord5/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord4/4) * 4;

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCtrlWord5/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eSTBOwnerIDSel/4) * 4
			 - (BCMD_GenKey_InCmd_eCtrlWord5/4) * 4;
	}
	else
	{
		/* adjust command buffer pointer for the other types */
		commandData.unInputParamLen += 24;
	}

	/* more stuffs needed for ASKM mode or space filler */
	if (inoutp_generateRouteKeyIO->bASKMModeEnabled)
	{
		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eSTBOwnerIDSel/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pKeySlotIDData->ulSTBOwnerIDSelect;
		commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eCAVendorID/4) * 4
			 - (BCMD_GenKey_InCmd_eSTBOwnerIDSel/4) * 4;		

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eCAVendorID/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pKeySlotIDData->ulCAVendorID;
		commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eModuleID/4) * 4
			 - (BCMD_GenKey_InCmd_eCAVendorID/4) * 4;		

		commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eModuleID/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
				pKeySlotIDData->ulModuleIDKey2Select;
		commandData.unInputParamLen +=	4;
	}
#if !HSM_IS_ASKM_40NM_ZEUS_1_0
	else
	{
		/* adjust command buffer pointer to point past ASKM info fields */
		commandData.unInputParamLen += 12;
	}
	
	/* To handle HW Key Ladder */
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eHwKlLength/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->hwklLength | 
			(inoutp_generateRouteKeyIO->hwklDestAlg << 8);
	commandData.unInputParamLen +=	(BCMD_GenKey_InCmd_eActivationCode/4) * 4
		 - (BCMD_GenKey_InCmd_eHwKlLength/4) * 4;		

	/* And Activation code, big endian array */
		
	for (i=0, 
		j = (BCMD_GenKey_InCmd_eActivationCode/4 - BCMD_GenKey_InCmd_eKeyLadderType/4)
			+ BHSM_KL_ACTCODE_LEN / 4;
		i < BHSM_KL_ACTCODE_LEN; j++, i+=4) 
	{
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_generateRouteKeyIO->activationCode[i]   << 24 |
			inoutp_generateRouteKeyIO->activationCode[i+1] << 16 |
			inoutp_generateRouteKeyIO->activationCode[i+2] << 8 |
			inoutp_generateRouteKeyIO->activationCode[i+3]  ;

		commandData.unInputParamLen +=	4;
	}

#endif

/*
	printf("Key ladder Data length = %d\n", commandData.unInputParamLen );
	printf("Key ladder generate route key data :\n");	
	for ( i= 0; i<commandData.unInputParamLen/4; i++)
		printf ("data[%d]= %08lx\n ",i,commandData.unInputParamsBuf[i]);
*/

BHSM_P_SUBMIT_CMD:

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_generateRouteKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_generateRouteKeyIO->unStatus != 0) );

	/* Check to see if it was just the subcommand BCMD_KeyGenSubCmdID_eVKLAssociationQuery */
	if (bVKLAssociationQuery)
	{
		/* store the just retrieved VKL-CustSubMode association table */
		for (i = 0; i < BHSM_MAX_VKL; i++)
		{
			if (commandData.unOutputParamsBuf[i + 1] == 0x000000FF)
				vklCustAssocTable[i].bAssociated = false;
			else
			{
				vklCustAssocTable[i].custSubMode = (BCMD_CustomerSubMode_e)commandData.unOutputParamsBuf[i + 1];
				vklCustAssocTable[i].bAssociated = true;
			}
			unParamLen += 4;
		}
		goto BHSM_P_DONE_LABEL;
	}
	/* The command was successful, so update the VKL-CustSubMode association table */
	vklCustAssocTable[inoutp_generateRouteKeyIO->virtualKeyLadderID].bAssociated = true;
	vklCustAssocTable[inoutp_generateRouteKeyIO->virtualKeyLadderID].custSubMode =
				inoutp_generateRouteKeyIO->customerSubMode;
	
	
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_GenerateRouteKey);
	return( errCode );
}




BERR_Code  BHSM_RetrieveVKL_CustSubModeAssociation (
		BHSM_Handle							in_handle,
		BHSM_VKLCustSubModeAssociationIO_t	*inoutp_VKLCustSubModeAssociationIO
)
{
	BERR_Code						errCode = BERR_SUCCESS;
	BHSM_ChannelHandle				channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 						unParamLen = 0, i, unTmp;
	BHSM_P_CommandData_t			commandData;
	uint32_t						ASKMEnabled = 0;



	BDBG_ENTER(BHSM_RetrieveVKL_CustSubModeAssociation);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_VKLCustSubModeAssociationIO );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_GENERATE_ROUTE_KEY;
	commandData.unContMode = 0;

	/* subcommand has to be VKL-CustSubMode association Query */
#if HSM_IS_ASKM_40NM_ZEUS_1_0
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_VKLCustSubModeAssociationIO->subCmdID != (unsigned int)BCMD_KeyGenSubCmdID_eVKLAssociationQuery) );
#else
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_VKLCustSubModeAssociationIO->subCmdID !=  (unsigned int)BCMD_VKLAssociationQueryFlag_eQuery) );
#endif

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyLadderType/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			((inoutp_VKLCustSubModeAssociationIO->keyLadderType                 & 0xFF)        |
			 ((inoutp_VKLCustSubModeAssociationIO->subCmdID                     & 0xFF) << 8)  |
	         ((inoutp_VKLCustSubModeAssociationIO->bASKM3DesKLRootKeySwapEnable & 0xFF) << 16) |
	         ((inoutp_VKLCustSubModeAssociationIO->keyLadderSelect              & 0xFF) << 24) );
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eRootKeySrc/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyLadderType/4) * 4;

	/* check for the settings of VKL and CustomerSubMode */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_VKLCustSubModeAssociationIO->virtualKeyLadderID > BCMD_VKL_eMax) );


	if (inoutp_VKLCustSubModeAssociationIO->bASKMModeEnabled)
		ASKMEnabled = 1;
	

	unTmp = inoutp_VKLCustSubModeAssociationIO->rootKeySrc | 
	        (inoutp_VKLCustSubModeAssociationIO->customerSubMode << BHSM_KEYLADDER_CUSTSUBMODE_SHIFT) |
			(ASKMEnabled << 16) | 
	        (inoutp_VKLCustSubModeAssociationIO->keyGenMode << 24);
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eRootKeySrc/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eSwizzle1IndexSel/4) * 4
		 - (BCMD_GenKey_InCmd_eRootKeySrc/4) * 4;

	/* skip the next 6 command words */
	commandData.unInputParamLen += 24;


	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eVKLID/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_VKLCustSubModeAssociationIO->virtualKeyLadderID;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyLayer/4) * 4
		 - (BCMD_GenKey_InCmd_eVKLID/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyLayer/4 - BCMD_GenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_VKLCustSubModeAssociationIO->keyLayer;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyLadderOpera/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyLayer/4) * 4;

	/* skip the next 11 commnad words (32 bit words) to clear out the key routing flag */
	/* as needed by FW */
	commandData.unInputParamLen += 44;

 
/*
	printf("Key ladder Data length = %d\n", commandData.unInputParamLen );
	printf("Key ladder generate route key data :\n");	
	for ( i= 0; i<commandData.unInputParamLen/4; i++)
		printf ("data[%d]= %08lx\n ",i,commandData.unInputParamsBuf[i]);
*/


	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_VKLCustSubModeAssociationIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_VKLCustSubModeAssociationIO->unStatus != 0) );

	/* store the just retrieved VKL-CustSubMode association table */
	for (i = 0; i < BHSM_MAX_VKL; i++)
	{
		vklCustAssocTable[i].vkl = (BCMD_VKLID_e)i;
		if (commandData.unOutputParamsBuf[i + 1] == 0x000000FF)
			vklCustAssocTable[i].bAssociated = false;
		else
		{
			vklCustAssocTable[i].custSubMode = (BCMD_CustomerSubMode_e)commandData.unOutputParamsBuf[i + 1];
			vklCustAssocTable[i].bAssociated = true;
		}
		inoutp_VKLCustSubModeAssociationIO->vklCustSubmode[i] = commandData.unOutputParamsBuf[i + 1];
		unParamLen += 4;
	}
	
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_RetrieveVKL_CustSubModeAssociation);
	return( errCode );

}



BERR_Code BHSM_ProcOutCmd (
		BHSM_Handle			in_handle,
		BHSM_ProcOutCmdIO_t	*inoutp_procOutCmdIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0,  i,  j;
	BHSM_P_CommandData_t 	commandData;


	BDBG_ENTER(BHSM_ProcOutCmd);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_procOutCmdIO );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_procOutCmdIO->unProcInLen > BHSM_MAX_PROC_IN_DATA_LEN) );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_PROC_OUT_CMD;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_ProcOut_InCmd_eVKLID)/4 - 
			(BCMD_CommonBufferFields_eStatus)/4] = 
			inoutp_procOutCmdIO->virtualKeyLadderID;
	commandData.unInputParamLen +=  (BCMD_ProcOut_InCmd_eProcIn/4) * 4
		 - (BCMD_ProcOut_InCmd_eVKLID/4) * 4;

	for (i=0, 
		j = (BCMD_ProcOut_InCmd_eProcIn/4 - BCMD_ProcOut_InCmd_eVKLID/4);
		i<inoutp_procOutCmdIO->unProcInLen; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_procOutCmdIO->aucProcIn[i] << 24 |
			inoutp_procOutCmdIO->aucProcIn[i+1] << 16 |
			inoutp_procOutCmdIO->aucProcIn[i+2] << 8 |
			inoutp_procOutCmdIO->aucProcIn[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}	
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_procOutCmdIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	


	inoutp_procOutCmdIO->unProcOutLen = commandData.unOutputParamLen  - unParamLen;
		
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_procOutCmdIO->unStatus != 0) );		

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_procOutCmdIO->unProcOutLen > BHSM_MAX_PROC_OUT_DATA_LEN) );
	
	for (i=0, j = 1; i<inoutp_procOutCmdIO->unProcOutLen; j++, i+=4) {

		inoutp_procOutCmdIO->aucProcOut[i]     = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_procOutCmdIO->aucProcOut[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
		inoutp_procOutCmdIO->aucProcOut[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
		inoutp_procOutCmdIO->aucProcOut[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 
	}

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_procOutCmdIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProcOutCmd);
	return( errCode );
}


#if !HSM_IS_ASKM_40NM_ZEUS_1_0

BERR_Code BHSM_PklInit (
		BHSM_Handle			in_handle,
		BHSM_PklCmdIO_t	*inoutp_pklCmdIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t 	commandData;


	BDBG_ENTER(BHSM_PklInit);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_pklCmdIO );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

#if	defined(PKL)	/* jipeng - need match bsp_s_command.h */
	commandData.cmdId = BCMD_cmdType_eSESSION_PKL;
#else
	commandData.cmdId = BCMD_cmdType_eRESERVED_93;
#endif	
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_PKL_InCmd_SubCmd)/4 - 
			(BCMD_CommonBufferFields_eStatus)/4] = 
			inoutp_pklCmdIO->pklSubCmdId;
	commandData.unInputParamLen +=  (BCMD_ProcOut_InCmd_eProcIn/4) * 4
		 - (BCMD_PKL_InCmd_SubCmd/4) * 4;
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_pklCmdIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	
		
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_pklCmdIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_PklInit);
	return( errCode );
}


#endif


#if BHSM_DS2_KEY_ROUTING 
BERR_Code  BHSM_RouteDataSectionKey (
		BHSM_Handle						in_handle,
		BHSM_RouteDataSectionKeyIO_t	*inoutp_RouteDataSectionKeyIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t 	commandData;	

	BDBG_MSG(("Inside BHSM_RouteDataSectionKey\n"));
	BDBG_ENTER(BHSM_RouteDataSectionKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
								(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_ROUTE_OTP_DATA_SECTION_KEY;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_RouteDataSectionKey_InCmdInit_eM2MSlotNum/4 - BCMD_CommonBufferFields_eStatus/4 ] 
							= 	inoutp_RouteDataSectionKeyIO->unKeySlotNumber;
	


	commandData.unInputParamLen = (BCMD_RouteDataSectionKey_InCmdInit_eM2MSlotNum/4 - BCMD_CommonBufferFields_eStatus/4)*4+4 ;    
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

 	unParamLen =0;    
					
	/* Parse the command specific output parameters */
	inoutp_RouteDataSectionKeyIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;
	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_RouteDataSectionKeyIO->unStatus != 0) );
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_RouteDataSectionKey);
	return( errCode );
}


#endif

