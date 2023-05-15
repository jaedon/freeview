/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <nova_port.h>
#include <apk.h>
#include <nova_cas.h>

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____IMPLEMENTATION_______________________
/**********************************************************************************************************/

static HERROR nova_ca_GetNuid(HUINT32 *pChipsetNuid)
{
	NOVA_CAS_Info_t *pCaInfo = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	pCaInfo = NOVA_CAS_CA_GetCaInfo();
	if(pCaInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCaInfo-> is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_CA_ERR);
		return ERR_FAIL;
	}

	// TODO : 정리 필요 아니면 obama에서 nuid만 다시 받아오도록 하자.
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[11] - 0x30) * 1;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[10] - 0x30) * 1*10;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[9] - 0x30) * 1*100;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[8] - 0x30) * 1*1000;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[6] - 0x30) * 1*10000;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[5] - 0x30) * 1*100000;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[4] - 0x30) * 1*1000000;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[3] - 0x30) * 1*10000000;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[1] - 0x30) * 1*100000000;
	*pChipsetNuid  += (pCaInfo->ucChipsetUid[0] - 0x30) * 1*1000000000;

	HxLOG_Info("*\t  new nuid(%s) = %d, 0x%08x\n", pCaInfo->ucChipsetUid, *pChipsetNuid, *pChipsetNuid);

	return ERR_OK;
}

NOVA_CAS_Info_t * nova_ca_GetCaInfo(void)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_Info_t *pCaInfo = NULL;
	HUINT8 ucData[MAX_NOVA_DATA_SIZE];

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return NULL;
	}

	pCaInfo = &pCas->stCaInfo;
	if(pCaInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCaInfo is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return NULL;
	}

	/* 	CaInfo Example
		--------------------------------------------------------------------------------------
		Attribute 		Usage 											Example
		--------------------------------------------------------------------------------------
		cs-uid			NUID											22 2222 0815 12
		cs-type			Type											12 3456 789A BC
		cs-revision		Revision											A1
		ca-vendor		constant										Nagra
		ca-version		Nagra CAS version and revision					R-AHNAH-ADFBE-DOLAI
		ca-number		Nagra CAS serial number (pairing key)				00 4711 0815 12
		sc-inserted		"true" if smartcard is inserted						true
		sc-suitable		"true" if inserted smartcard is suitable for HD PLUS	true
		sc-type			Smartcard type									DNASP142 RevGC6
		sc-number		Smartcard serial number							17 0815 0816 69
		sc-status		Smartcard status as defined below					activating
		--------------------------------------------------------------------------------------
	**/

	APK_CAS_NAGRA_PRM_GetCaInfo(&ucData[0]);

	NOVA_memcpy(pCaInfo, ucData, sizeof(NOVA_CAS_Info_t));

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t MAX_NOVA_DATA_SIZE = %d\n", MAX_NOVA_DATA_SIZE);
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
	return pCaInfo;
}

static void nova_ca_InitCaInfo(void)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_Info_t *pCaInfo = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pCaInfo = &pCas->stCaInfo;
	if(pCaInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCaInfo is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	NOVA_memset(pCaInfo, 0, sizeof(NOVA_CAS_Info_t));

	pCaInfo->ucSmartcardStatus = NOVASPREAD_CA_INFO_SMARTCARD_STATUS_ACTIVATING;
	pCaInfo->ucSmartcardInserted = FALSE;
	pCaInfo->ucSmartcardSuitable = FALSE;
	pCaInfo->ulExpirationDate = 0;

	return;
}

static HINT32 nova_ca_GetServiceUsageRulesIndexFromTsId(HUINT32  ulTransportSessionId)
{
	int i = 0;
	HINT32 ulIndex = NOVA_INVALID_INDEX;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NOVA_INVALID_INDEX;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pServiceUsageRules = &pCas->stNotifier.stTs[i].stServiceUsageRules;
		if(pServiceUsageRules == NULL)
		{
			continue;
		}

		if(pServiceUsageRules->ulTransportSessionId == ulTransportSessionId)
		{
			ulIndex = i;
			break;
		}
	}

	return ulIndex;
}

static void nova_ca_FreePlatformUsageRules(NOVA_CAS_PlatformUsageRulesNotifier_t *pPlatformUsageRules)
{
	if(pPlatformUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pPlatformUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	if(pPlatformUsageRules->pUsageRules)
	{
		HLIB_STD_MemFree(pPlatformUsageRules->pUsageRules);
		pPlatformUsageRules->pUsageRules = NULL;
	}
	pPlatformUsageRules->ulSize = 0;	
}

static void nova_ca_InitPlatformUsageRules(void)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_PlatformUsageRulesNotifier_t *pPlatformUsageRules = NULL;

	HxLOG_Info("-%s-\n", NOVA_CAS_DRM_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pPlatformUsageRules = &pCas->stNotifier.stPlatformUsageRules;
	if(pPlatformUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pPlatformUsageRules->CaPlatformUsageRulesReceivedListener  = NULL;
	pPlatformUsageRules->pContext = NULL;

	nova_ca_FreePlatformUsageRules(pPlatformUsageRules);

	return;
}

static void nova_ca_PlatformUsageRulesNotifier(HUINT8 *pucUsageRules, HUINT32 ulSize)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_PlatformUsageRulesNotifier_t *pPlatformUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pPlatformUsageRules = &pCas->stNotifier.stPlatformUsageRules;
	if(pPlatformUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCaInfo is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pPlatformUsageRules->pUsageRules = (HUINT8 *)NOVA_MEM_Alloc(sizeof(HUINT8)*ulSize+1);
	pPlatformUsageRules->ulSize = ulSize;

	NOVA_memset(pPlatformUsageRules->pUsageRules, 0, sizeof(HUINT8)*ulSize+1);
	NOVA_memcpy(pPlatformUsageRules->pUsageRules, pucUsageRules, ulSize);

	NOVA_PORT_PlatformUsageRulesNotifier(pPlatformUsageRules);
	nova_ca_FreePlatformUsageRules(pPlatformUsageRules);
}

static void nova_ca_SetPlatformUsageRulesReceivedListener (NovaspreadTCaPlatformUsageRulesReceivedListener *pListener, void * pContext)
{	
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_PlatformUsageRulesNotifier_t *pPlatformUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	if(pContext == NOVASPREAD_NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pPlatformUsageRules = &pCas->stNotifier.stPlatformUsageRules;
	if(pPlatformUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pPlatformUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pPlatformUsageRules->pContext = pContext;
	pPlatformUsageRules->CaPlatformUsageRulesReceivedListener = *pListener;

	if(*pListener == NOVASPREAD_NULL)
	{
		// *pListener == NOVASPREAD_NULL 이면 obama로 listener를 전달하지 않도록 하자.
		HxLOG_Error("[%s:%d] -%s- pListener is NOVASPREAD_NULL\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	return;
}

static void nova_ca_FreeServiceUsageRules(NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules)
{
	if(pServiceUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pPlatformUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	if(pServiceUsageRules->pUsageRules)
	{
		HLIB_STD_MemFree(pServiceUsageRules->pUsageRules);
		pServiceUsageRules->pUsageRules = NULL;
	}
	pServiceUsageRules->ulSize = 0;	
}

static void nova_ca_InitServiceUsageRulesFromIndex(HUINT32 ulIndex)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_ServiceUsageRulesNofitier_t 	*pServiceUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	if(ulIndex >= NOVA_MAX_NUM_OF_TS)
	{
		HxLOG_Error("[%s:%d] -%s- ulIndex is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pServiceUsageRules = &pCas->stNotifier.stTs[ulIndex].stServiceUsageRules;
	if(pServiceUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pServiceUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pServiceUsageRules->pContext; // pContext는 Nova-S로부터 전달받으므로 여기서 NULL하지 않는다.
	pServiceUsageRules->ulTransportSessionId = NOVA_INVALID_TSID;
	pServiceUsageRules->CaServiceUsageRulesReceivedListener = NULL;

	if(pServiceUsageRules->pUsageRules)
	{
		HLIB_STD_MemFree(pServiceUsageRules->pUsageRules);
		pServiceUsageRules->pUsageRules = NULL;
	}
	pServiceUsageRules->ulSize = 0;

	return;
}
static int nova_ca_ReleaseListener(HUINT32 ulTransportSessionId)
{
	HINT32 ulIndex = 0;

	ulIndex = nova_ca_GetServiceUsageRulesIndexFromTsId(ulTransportSessionId);

	if(ulIndex == NOVA_INVALID_INDEX)
	{
		HxLOG_Error("[%s:%d] -%s- ulIndex is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return ERR_FAIL;
	}

	nova_ca_InitServiceUsageRulesFromIndex(ulIndex);

	return ERR_OK;
}

NOVA_CAS_ServiceUsageRulesNofitier_t * nova_ca_GetServiceUsageRulesFromTsId(HUINT32  ulTransportSessionId)
{
	int i = 0;
	HBOOL bValidURFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NULL;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pServiceUsageRules = &pCas->stNotifier.stTs[i].stServiceUsageRules;
		if(pServiceUsageRules == NULL)
		{
			continue;
		}

		if(pServiceUsageRules->ulTransportSessionId == ulTransportSessionId)
		{
			bValidURFlag = TRUE;
			break;
		}
	}

	if(bValidURFlag == FALSE)
	{
		return NULL;
	}

	return pServiceUsageRules;
}

static NOVA_CAS_ServiceUsageRulesNofitier_t * nova_cas_GetValidCaServiceURReceivedListener(void)
{
	int i = 0;
	HBOOL bValidFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;	
	NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return NULL;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pServiceUsageRules = &pCas->stNotifier.stTs[i].stServiceUsageRules;
		if(pServiceUsageRules == NULL)
		{
			continue;
		}

		// 우선 valid한지 tsid와 context로 check하자 
		if(pServiceUsageRules->pContext == NULL || pServiceUsageRules->ulTransportSessionId == NOVA_INVALID_TSID)
		{
			bValidFlag = TRUE;
			break;
		}
	}

	if(bValidFlag == FALSE)
	{
		HxLOG_Error("[%s:%d] -%s- bValidFlag is fail\n",__FUNCTION__, __LINE__, NOVA_INVALID_TSID);
		return NULL;
	}

	return pServiceUsageRules;
}

static void nova_ca_ServiceUsageRulesNotifier(HUINT32 ulTransportSessionId, HUINT8 *pucUsageRules, HUINT32 ulSize)
{
	NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	if(pucUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pucUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	if(ulSize == 0)
	{
		HxLOG_Error("[%s:%d] -%s- ulSize is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}

	pServiceUsageRules = nova_ca_GetServiceUsageRulesFromTsId(ulTransportSessionId);
	if(pServiceUsageRules == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pServiceUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return;
	}


	pServiceUsageRules->pUsageRules = (HUINT8 *)NOVA_MEM_Alloc(sizeof(HUINT8)*ulSize+1);
	pServiceUsageRules->ulSize = ulSize;

	NOVA_memset(pServiceUsageRules->pUsageRules, 0, sizeof(HUINT8)*ulSize+1);
	NOVA_memcpy(pServiceUsageRules->pUsageRules, pucUsageRules, ulSize);

	NOVA_PORT_ServiceUsageRulesNotifier(pServiceUsageRules);
	nova_ca_FreeServiceUsageRules(pServiceUsageRules);
}

static int nova_ca_SetServiceUsageRulesReceivedListener ( HUINT32 ulTransportSessionId, NovaspreadTCaServiceUsageRulesReceivedListener Listener, void * pContext )
{
	HERROR hRet = ERR_OK;
	NOVA_CAS_ServiceUsageRulesNofitier_t *pServiceUsageRules = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_CA_INFO);

	if(pContext == NOVASPREAD_NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return NOVA_BOOL(ERR_FAIL);		
	}

	if(ulTransportSessionId < 0)
	{
		return NOVA_BOOL(ERR_FAIL);
	}

	// 동일 TSID가 존재하는경우 어떻게 처리할지.
	pServiceUsageRules = nova_ca_GetServiceUsageRulesFromTsId(ulTransportSessionId);
	if(pServiceUsageRules != NULL)
	{
		#if 1
		pServiceUsageRules = nova_cas_GetValidCaServiceURReceivedListener();
		if(pServiceUsageRules == NULL)
		{
			HxLOG_Error("[%s:%d] -%s- pServiceUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
			return NOVA_BOOL(ERR_FAIL);
		}
		#else
			// 이런 case는 없어야한다.
			HxLOG_Error("[%s:%d] -%s- pServiceUsageRules is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
			return NOVA_BOOL(ERR_FAIL);
		#endif
	}
	else
	{
		HxLOG_Error("[%s:%d] -%s- pServiceUsageRules is already exist\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return NOVA_BOOL(ERR_FAIL);
	}

	HxLOG_Debug("\tpContext(0x%08x), ulTransportSessionId(%d)\n", pContext, ulTransportSessionId);

	pServiceUsageRules->ulTransportSessionId = ulTransportSessionId;
	pServiceUsageRules->pContext = pContext;
	pServiceUsageRules->CaServiceUsageRulesReceivedListener = Listener;

	if(Listener == NOVASPREAD_NULL)
	{
		HxLOG_Error("[%s:%d] -%s- Listener unset\n",__FUNCTION__, __LINE__, NOVA_CAS_CA_ERR);
		return NOVA_BOOL(hRet);
	}

	return NOVA_BOOL(hRet);
}


void NOVA_CAS_CA_Init(void)
{
	int i = 0;

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		nova_ca_InitServiceUsageRulesFromIndex(i);
	}

	NOVA_CAS_CA_InitCaInfo();

	nova_ca_InitPlatformUsageRules();

	(void)APK_CAS_NAGRA_UnregisterNovaPlatformUsageRulesNotifier();

	(void)APK_CAS_NAGRA_UnregisterNovaServiceUsageRulesNotifier();

	(void)APK_CAS_NAGRA_RegisterNovaPlatformUsageRulesNotifier((APK_CAS_NAGRA_NovaPlatformUsageRulesNotifier_t)nova_ca_PlatformUsageRulesNotifier);

	(void)APK_CAS_NAGRA_RegisterNovaServiceUsageRulesNotifier((APK_CAS_NAGRA_NovaServiceUsageRulesNotifier_t)nova_ca_ServiceUsageRulesNotifier);
}


void NOVA_CAS_CaSetPlatformUsageRulesReceivedListener (NovaspreadTCaPlatformUsageRulesReceivedListener *pListener, void * pContext)
{
	return nova_ca_SetPlatformUsageRulesReceivedListener(pListener, pContext);
}

int NOVA_CAS_CA_SetServiceUsageRulesReceivedListener ( HUINT32 ulTransportSessionId, NovaspreadTCaServiceUsageRulesReceivedListener Listener, void * pContext )
{
	return nova_ca_SetServiceUsageRulesReceivedListener( ulTransportSessionId, Listener, pContext);
}

HERROR NOVA_CAS_CA_GetNuid(HUINT32 *pChipsetNuid)
{
	return nova_ca_GetNuid(pChipsetNuid);
}

void NOVA_CAS_CA_InitCaInfo(void)
{
	nova_ca_InitCaInfo();
}

NOVA_CAS_Info_t * NOVA_CAS_CA_GetCaInfo(void)
{
	return nova_ca_GetCaInfo();
}
