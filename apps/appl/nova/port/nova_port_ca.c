/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_port.h>
#include <NovaspreadDvbId.h>
#include <nova_cas.h>

/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
void NOVA_PORT_ServiceUsageRulesNotifier(NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules)
{
	HxLOG_Info("[%s:%d]-%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_CA_INFO);

	if(pServiceUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pListener is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	if(pServiceUsageRules->pContext == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	if(pServiceUsageRules->pUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	if(pServiceUsageRules->ulSize == 0)
	{
		HxLOG_Error("[%s:%d] -%s- ulSize is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	pServiceUsageRules->CaServiceUsageRulesReceivedListener(pServiceUsageRules->pContext, pServiceUsageRules->ulTransportSessionId, pServiceUsageRules->pUsageRules, pServiceUsageRules->ulSize);
}

void NOVA_PORT_PlatformUsageRulesNotifier(NOVA_CAS_PlatformUsageRulesNotifier_t *pPlatformUsageRules)
{
	HxLOG_Info("[%s:%d]-%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_CA_INFO);

	if(pPlatformUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pPlatformUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	if(pPlatformUsageRules->pContext == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	if(pPlatformUsageRules->pUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}

	if(pPlatformUsageRules->ulSize == 0)
	{
		HxLOG_Error("[%s:%d] -%s- ulSize is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return;
	}	

	pPlatformUsageRules->CaPlatformUsageRulesReceivedListener(pPlatformUsageRules->pContext, pPlatformUsageRules->pUsageRules, pPlatformUsageRules->ulSize);
}

/****************************************************************************
* FUNCTION: NovaspreadCaGetInfo
*
* DESCRIPTION:
*   This function returns information about the CA system. The returned
*   CaInfo will be destroyed by NovaspreadServer.
*
* RETURNS:
*   A new CaInfo if successful. NOVASPREAD_NULL otherwise.
*
* EXAMPLE:
*   // An implementation of this function shall proceed as follows:
*
*   PUBLIC NovaspreadTCaInfo
*   NovaspreadCaGetInfo ( void )
*   {
*     NovaspradTCaInfo  caInfo;
*     const char *      caVendor = "Nagra";
*
*     caInfo = NovaspreadCaInfoCreate();
*     if ( ! caInfo )
*       return NOVASPREAD_NULL;
*
*     // For Nagra, set the NUId by calling
*     // NovaspreadCaInfoSetChipsetUid().
*
*     NovaspreadCaInfoSetCaVendor( caInfo, caVendor );
*
*     // Call NovaspreadCaInfoSet..() functions here to set
*     // information about the CA system and the smartcard.
*
*     return caInfo;
*   }
*
* SEE ALSO:
*   NovaspreadTCaInfo
****************************************************************************/
PUBLIC NovaspreadTCaInfo NovaspreadCaGetInfo ( void )
{
	NovaspreadTCaInfo novaCaInfo = NULL;
	NOVA_CAS_Info_t *pCaInfo = NULL;
	HBOOL bDistoryFlag = FALSE;

	HxLOG_Info("[%s:%d]-%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_CA_INFO);
#if 0
	novaCaInfo = NovaspreadCaInfoCreate();
	if (novaCaInfo == NOVASPREAD_NULL )
	{
		HxLOG_Error("[%s:%d] -%s- novaCaInfo is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return NOVASPREAD_NULL;
	}
#endif

	pCaInfo = NOVA_CAS_CA_GetCaInfo();
	if(pCaInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCaInfo-> is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return NOVASPREAD_NULL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t pChipsetUid = %s\n", pCaInfo->ucChipsetUid);
	HxLOG_Debug("*\t pucCaVendor = %s\n", pCaInfo->ucCaVendor);
	HxLOG_Debug("*\t pucChipsetType = %s\n", pCaInfo->ucChipsetType);
	HxLOG_Debug("*\t pucChipsetRevision = %s\n",  pCaInfo->ucChipsetRevision);
	HxLOG_Debug("*\t pucCaVersion = %s\n", pCaInfo->ucCaVersion);
	HxLOG_Debug("*\t pucCaNumber = %s\n", pCaInfo->ucCaNumber);
	HxLOG_Debug("*\t pbSmartcardInserted = %d\n",pCaInfo->ucSmartcardInserted);
	HxLOG_Debug("*\t pbSmartcardSuitable = %d\n",pCaInfo->ucSmartcardSuitable);
	HxLOG_Debug("*\t pucCaNumber = %s\n",pCaInfo->ucSmartcardType);
	HxLOG_Debug("*\t pucSmartcardNumber = %s\n",pCaInfo->ucSmartcardNumber);
	HxLOG_Debug("*\t pucSmartcardStatus = %d\n", pCaInfo->ucSmartcardStatus);
	HxLOG_Debug("*\t pulExpirationDate = %d\n\n", pCaInfo->ulExpirationDate);
#endif

#if 0
	/* Sets the chipset unique ID (NUId) */
	if(NovaspreadCaInfoSetChipsetUid(novaCaInfo, &pCaInfo->usChipsetUid)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aChipsetUid is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the CAS vendor */
	if(NovaspreadCaInfoSetCaVendor(novaCaInfo, &pCaInfo->usCaVendor)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aCaVendor is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the chipset type */
	if(NovaspreadCaInfoSetChipsetType(novaCaInfo, &pCaInfo->usChipsetType)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aChipsetType is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the chipset revision */
	if(NovaspreadCaInfoSetChipsetRevision(novaCaInfo, &pCaInfo->usChipsetRevision)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aChipsetRevision is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the CAS version. */
	if(NovaspreadCaInfoSetCaVersion(novaCaInfo, &pCaInfo->usCaVersion)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aCaVersion is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the CAS version. */
	if(NovaspreadCaInfoSetCaNumber(novaCaInfo, &pCaInfo->usCaNumber)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aCaNumber is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets whether a smartcard is inserted or not. */
	if(NovaspreadCaInfoSetSmartcardInserted(novaCaInfo, pCaInfo->bSmartcardInserted == TRUE ? NOVASPREAD_TRUE : NOVASPREAD_FALSE)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aCaNumber is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets whether a smartcard is inserted or not. */
	if(NovaspreadCaInfoSetSmartcardSuitable(novaCaInfo, pCaInfo->bSmartcardSuitable == TRUE ? NOVASPREAD_TRUE : NOVASPREAD_FALSE)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aSuitable is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the type and/or version of the smartcard */
	if(NovaspreadCaInfoSetSmartcardType(novaCaInfo, &pCaInfo->usSmartcardType)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aSuitable is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the smartcard's serial number */
	if(NovaspreadCaInfoSetSmartcardNumber(novaCaInfo, &pCaInfo->usSmartcardNumber)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aNumber is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the smartcard status information as defined by the operator */
	if(NovaspreadCaInfoSetSmartcardStatus(novaCaInfo, pCaInfo->eSmartcardStatus)  != NOVASPREAD_FALSE)
	{
		bDistoryFlag = TRUE;
		HxLOG_Error("[%s:%d] -%s- aStatus is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	/* Sets the expiration date. The expiration date shall be set if the status of the smartcard is ACTIVATED */
	if(pCaInfo->eSmartcardStatus == NOVASPREAD_CA_INFO_SMARTCARD_STATUS_ACTIVATED)
	{
		if(NovaspreadCaInfoSetExpirationDate(novaCaInfo, pCaInfo->ulExpirationDate)  != NOVASPREAD_FALSE)
		{
			bDistoryFlag = TRUE;
			HxLOG_Error("[%s:%d] -%s- aDate is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		}
	}
#endif

	if(bDistoryFlag == TRUE)
	{
		#if 0
		HLIB_STD_MemFree(pCaInfo);
		pCaInfo = NULL;

		return NULL;
		#endif
	}

	return novaCaInfo;
}

/****************************************************************************
 * FUNCTION: NovaspreadCaInfoDestroy
 *
 * DESCRIPTION:
 *   Destroys the given CaInfo.
 *
 * PARAMETERS:
 *   This - This CaInfo
 *
 *SEE ALSO
 *  NovaspreadCaInfoCreate()
 *
 ****************************************************************************/
PUBLIC void NovaspreadCaInfoDestroy (NovaspreadTCaInfo This )
{
	HxLOG_Info("[%s:%d]-%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_CA_INFO);

	HLIB_STD_MemFree(This);
	This = NULL;

	NOVA_CAS_CA_InitCaInfo();
}

/****************************************************************************
* FUNCTION: NovaspreadCaSetDvbId
*
* DESCRIPTION:
*   Sets the DvbId which is needed for transcryption.
*
* PARAMETERS:
*   aTransportSessionId - For this TransportSession the DvbId is set.
*   aDvbId              - The DvbId to be set.
*
* RETURNS:
*   NOVASPREAD_TRUE   if successful
*   NOVASPREAD_FALSE  otherwise
****************************************************************************/
#if 0	// 2015/8/25 mhkang: Novaspread ver 0.53 에서 NovaspreadCaSetDvbId 함수가 없어졌음. 확인필요.
PUBLIC NovaspreadTBoolean NovaspreadCaSetDvbId ( NovaspreadTUInt32  aTransportSessionId, NovaspreadTCaDvbId aDvbId )
{
	//	TODO:....
	return NOVASPREAD_TRUE;
}
#endif

/****************************************************************************
* FUNCTION: NovaspreadCaSetPlatformUsageRulesReceivedListener
*
* DESCRIPTION:
*   This functions sets a listener, which shall be called when platform
*   specific usage rules are received.
*
* PARAMETERS:
*   aListener - The PlatformUsageRulesReceivedListener to be set.
*               NOVASPREAD_NULL is passed to unset the listener.
*   aContext  - The context which shall be passed unchanged to the listener.
*
* SEE ALSO:
*   NovaspreadTunerGetTransportSessionId()
*   NovaspreadTCaPlatformUsageRulesReceivedListener
****************************************************************************/

PUBLIC void NovaspreadCaSetPlatformUsageRulesReceivedListener ( NovaspreadTCaPlatformUsageRulesReceivedListener * aListener, void * aContext )
{
	HxLOG_Info("[%s:%d]-%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_CA_INFO);

	NOVA_CAS_CaSetPlatformUsageRulesReceivedListener(aListener, aContext);
}


/****************************************************************************
* FUNCTION: NovaspreadCaSetServiceUsageRulesReceivedListener
*
* DESCRIPTION:
*   Sets a ServiceUsageRulesReceivedListener. The TransportSessionId
*   identifies the stream received by a Tuner for which the UsageRules
*   should be acquired. The TransportSessionId can be retrieved with the
*   function NovaspreadTunerGetTransportSessionId().
*
* PARAMETERS:
*   aTransportSessionId - For this TransportSession the listener is set.
*   aListener           - The ServiceUsageRulesReceivedListener to be set.
*                         NOVASPREAD_NULL is passed to unset the listener.
*   aContext            - This context shall be passed unchanged to the
*                         listener.
*
* RETURNS:
*   NOVASPREAD_TRUE   if successful
*   NOVASPREAD_FALSE  otherwise
*
* SEE ALSO:
*   NovaspreadTunerGetTransportSessionId()
*   NovaspreadTCaServiceUsageRulesReceivedListener
****************************************************************************/
PUBLIC NovaspreadTBoolean NovaspreadCaSetServiceUsageRulesReceivedListener ( NovaspreadTUInt32 aTransportSessionId, 
									NovaspreadTCaServiceUsageRulesReceivedListener aListener,
									void * aContext )
{
	NovaspreadTBoolean bRet = NOVASPREAD_FALSE;

	HxLOG_Info("[%s:%d]-%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_CA_INFO);

	bRet = NOVA_CAS_CA_SetServiceUsageRulesReceivedListener(aTransportSessionId, aListener, aContext);

	if(bRet == NOVASPREAD_FALSE)
{
		HxLOG_Error("[%s:%d] -%s- bRet is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
	}

	return bRet;
}



/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/
