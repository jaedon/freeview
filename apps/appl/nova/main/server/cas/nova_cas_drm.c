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
#include <apk.h>
#include "nova_cas.h"

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

static void nova_drm_printAllLicenseInfo(void)
{
	int i = 0; 
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmLicense = NULL;
	
	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return ;
	}

	// 동일한 정보가 존재하는지 check함.d
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pDrmLicense = &pCas->stNotifier.stTs[i].stDrmLicense;
		if(pDrmLicense == NULL)
		{
			continue;
		}


		HxLOG_Info("*\t License [%d] ulTransportSessionId(%d), ulRecordHandle(0x%08x), pContext(0x%08x), DrmLicenseChangeListener(0x%08x)\n"
			, i
			,  pDrmLicense->ulTransportSessionId
			,  pDrmLicense->ulRecordHandle
			, pDrmLicense->pContext
			, pDrmLicense->DrmLicenseChangeListener);
	}
}

static void nova_drm_PrintLicenseParameters(NovaspreadTDrmLicenseParameters *pDrmLicenseParameters)
{
#if defined(CONFIG_DEBUG)
	NovaspreadTDrmLicenseParameters *pParam = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pParam = pDrmLicenseParameters;
	if(pParam == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pParam is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	if(pParam->OldLicense.LicenseLength == 0 || pParam->OldLicense.License == NULL)
	{
		HxLOG_Debug("*\t OldLicense[%d] is not exist\n", pParam->OldLicense.LicenseLength);
	}
	else
	{
		HxLOG_Debug("*\t OldLicense : Exist\n");
		NOVA_CAS_PrintHexDump("OldLicense", pParam->OldLicense.License, pParam->OldLicense.LicenseLength);
	}

	HxLOG_Debug("*\t CollectionId(%d)\n", pParam->CollectionId);
	HxLOG_Debug("*\t Duration(%ld)\n", pParam->Duration);
	NOVA_CAS_PrintHexDump("UsageRules", pParam->UsageRules, pParam->UsageRulesLength);
#endif
}

static void nova_drm_PrintKeyHandlerInfo(NOVA_CAS_KeyHandlerInfo_t *pKeyHandler)
{
	if(pKeyHandler == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pKeyHandler is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	HxLOG_Info("[%s:%d] -%s-, ulTransportSessionId(%d)\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO, pKeyHandler->ulTransportSessionId);
	HxLOG_Info("[%s:%d] -%s-, ulKeyHandler(0x%08x)\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO, pKeyHandler->ulKeyHandler);
	if(pKeyHandler->ulKeyLength > 0 && pKeyHandler->ulKeyLength < 128)
	{
		NOVA_CAS_PrintHexDump("ClearKey", &pKeyHandler->ucKey[0], pKeyHandler->ulKeyLength);
	}
}

static NOVA_CAS_DrmLicenseNotifier_t * nova_drm_GetListenerFromTsId(HUINT32  ulTransportSessionId)
{
	int i = 0;
	HBOOL bValidDrmFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmLicense = NULL;

	HxLOG_Info("[%s:%d] -%s-, ulTransportSessionId(%d)\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO, ulTransportSessionId);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NULL;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pDrmLicense = &pCas->stNotifier.stTs[i].stDrmLicense;
		if(pDrmLicense == NULL)
		{
			continue;
		}
		HxLOG_Info("*\t [%d] tsid(%d)(%d)\n", i, pDrmLicense->ulTransportSessionId, ulTransportSessionId);
		if(pDrmLicense->ulTransportSessionId == ulTransportSessionId)
		{
			bValidDrmFlag = TRUE;
			break;
		}
	}

	if(bValidDrmFlag == FALSE)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NULL;
	}

	return pDrmLicense;
}

static HINT32 nova_drm_GetDrmListenerIndexFromTsId(HUINT32  ulTransportSessionId)
{
	int i = 0;
	HINT32 ulIndex = NOVA_INVALID_INDEX;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmLicense = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NOVA_INVALID_INDEX;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pDrmLicense = &pCas->stNotifier.stTs[i].stDrmLicense;
		if(pDrmLicense == NULL)
		{
			continue;
		}

		if(pDrmLicense->ulTransportSessionId == ulTransportSessionId)
		{
			ulIndex = i;
			break;
		}
	}

	return ulIndex;
}

static HUINT32 nova_drm_GetKeyHandlerFromTsId(HUINT32 ulTransportSessionId)
{
	int i = 0;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_KeyHandlerInfo_t *pKeyHandler = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NOVA_INVALID_INDEX;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pKeyHandler = &pCas->stNotifier.stTs[i].stKeyHandler;
		if(pKeyHandler == NULL)
		{
			continue;
		}

		if(pKeyHandler->ulTransportSessionId == ulTransportSessionId)
		{
#if 0//defined(CONFIG_DEBUG)
			nova_drm_PrintKeyHandlerInfo(pKeyHandler);
#endif
			return pKeyHandler->ulKeyHandler;
		}
	}

	return NOVA_INVALID_KEYHANDLE;
}

static NOVA_CAS_KeyHandlerInfo_t * nova_drm_GetKeyHandlerInfoFromTsId(HUINT32 ulTransportSessionId)
{
	int i = 0;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_KeyHandlerInfo_t *pKeyHandler = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NULL;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pKeyHandler = &pCas->stNotifier.stTs[i].stKeyHandler;
		if(pKeyHandler == NULL)
		{
			continue;
		}

		if(pKeyHandler->ulTransportSessionId == ulTransportSessionId)
		{
			if(pKeyHandler->ulKeyLength != 0)
			{
#if 0//defined(CONFIG_DEBUG)
				nova_drm_PrintKeyHandlerInfo(pKeyHandler);
#endif

				return pKeyHandler;
			}
		}
	}

	return NULL;
}


static void nova_drm_InitKeyHandlerFromIndex(HUINT32 ulIndex)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_KeyHandlerInfo_t *pKeyHandler = NULL;

	if(ulIndex >= NOVA_MAX_NUM_OF_TS)
	{
		HxLOG_Error("[%s:%d] -%s- ulIndex is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pKeyHandler = &pCas->stNotifier.stTs[ulIndex].stKeyHandler;
	if(pKeyHandler == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pKeyHandler is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pKeyHandler->ulKeyHandler = NOVA_INVALID_KEYHANDLE;
	pKeyHandler->ulTransportSessionId = NOVA_INVALID_TSID;
	pKeyHandler->ulKeyLength = 0;
	NOVA_memset(&pKeyHandler->ucKey[0], 0, sizeof(pKeyHandler->ucKey));
}

static void nova_drm_InitListenerFromIndex(HUINT32 ulIndex)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmListener = NULL;

	HxLOG_Info("[%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	if(ulIndex >= NOVA_MAX_NUM_OF_TS)
	{
		HxLOG_Error("[%s:%d] -%s- ulIndex is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pDrmListener = &pCas->stNotifier.stTs[ulIndex].stDrmLicense;
	if(pDrmListener == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	HxLOG_Info("[%s:%d]-%s- : ulRecordHandle(0x%08x)\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO, pDrmListener->ulRecordHandle);
	//if(pDrmListener->ulRecordHandle)
	{
		APK_CAS_NAGRA_PRM_NovaDrmRelease(pDrmListener->ulTransportSessionId, pDrmListener->ulRecordHandle);
	}

	nova_drm_InitKeyHandlerFromIndex(ulIndex);

	pDrmListener->ulTransportSessionId = NOVA_INVALID_TSID;
	pDrmListener->ulRecordHandle = 0;
	pDrmListener->pContext = NULL;
	pDrmListener->DrmLicenseChangeListener  = NULL;

	if(pDrmListener->pLicense)
	{
		HLIB_STD_MemFree(pDrmListener->pLicense);
		pDrmListener->pLicense = NULL;
	}
	pDrmListener->ulSize = 0;

	return;
}

static NOVA_CAS_DrmLicenseNotifier_t * nova_drm_CreateListener(void)
{
	int i = 0;
	HBOOL bValidDrmFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmLicense = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NULL;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pDrmLicense = &pCas->stNotifier.stTs[i].stDrmLicense;
		if(pDrmLicense == NULL)
		{
			continue;
		}
		HxLOG_Info("*\t [%d] tsid(%d)\n", i, pDrmLicense->ulTransportSessionId);
		if(pDrmLicense->ulTransportSessionId == NOVA_INVALID_TSID)
		{
			bValidDrmFlag = TRUE;
			break;
		}
	}

	if(bValidDrmFlag == FALSE)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NULL;
	}

	return pDrmLicense;
}

static void nova_drm_RemoveLicense(NOVA_CAS_DrmLicenseNotifier_t *pDrmListener)
{
	if(pDrmListener == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pLicense is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	if(pDrmListener->pLicense)
	{
		NOVA_MEM_Free(pDrmListener->pLicense);
		pDrmListener->pLicense = NULL;
	}
	pDrmListener->ulSize = 0;
}

static int nova_drm_ReleaseDrmLisntener(HUINT32  ulTransportSessionId)
{
	HUINT32 ulIndex = 0;

	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t\t NOVA DRM : Remove Listener\n");
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t [%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);
	HxLOG_Info("*\n");
	HxLOG_Info("*\t ulTransportSessionId(%d)\n", ulTransportSessionId);
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");

	ulIndex = NOVA_CAS_DRM_GetDrmListenerIndexFromTsId(ulTransportSessionId);

	if(ulIndex == NOVA_INVALID_INDEX)
	{
		HxLOG_Error("[%s:%d] -%s- ulIndex is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return ERR_FAIL;
	}

	nova_drm_InitListenerFromIndex(ulIndex);

	return ERR_OK;
}

static void nova_drm_LicenseNotifier(HUINT32  ulTransportSessionId, HUINT32 ulRecordHandle, HUINT8 *pLicense, HUINT32 ulLicenseLength, HBOOL bRequestLicenseAgain )
{
	NOVA_CAS_DrmLicenseNotifier_t * pDrmListener = NULL;

	HxLOG_Info("[%s:%d]-%s- ulTransportSessionId(%d), ulRecordHandle(0x%08x), bRequestLicenseAgain(%d), size(%d), License(0x%x, 0x%x, 0x%x)\n"
										,__FUNCTION__
										, __LINE__
										, NOVA_CAS_DRM_INFO
										, ulTransportSessionId
										, ulRecordHandle
										, bRequestLicenseAgain
										, ulLicenseLength
										, pLicense[0], pLicense[1], pLicense[2]);

	pDrmListener = nova_drm_GetListenerFromTsId(ulTransportSessionId);
	if(pDrmListener == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pDrmListener is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	if(pDrmListener->DrmLicenseChangeListener == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- DrmLicenseChangeListener is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	if(/*pLicense == NULL || ulLicenseLength == 0 ||*/ bRequestLicenseAgain == TRUE)
	{
		HxLOG_Error("[%s:%d] -%s- bRequestLicenseAgain ==> Go \n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		NOVA_PORT_LicenseNotifier(pDrmListener, FALSE);
		return;
	}

	pDrmListener->ulRecordHandle = ulRecordHandle;

	pDrmListener->ulSize = ulLicenseLength;
	if(pDrmListener->pLicense != NULL)
	{
		nova_drm_RemoveLicense(pDrmListener);
	}

	pDrmListener->pLicense = (HUINT8 *)NOVA_MEM_Alloc(sizeof(HUINT8)*ulLicenseLength+1);

	NOVA_memset(pDrmListener->pLicense, 0, sizeof(HUINT8)*ulLicenseLength+1);
	NOVA_memcpy(pDrmListener->pLicense, pLicense, ulLicenseLength);

#if 1//defined(TARA_SES_DEMO) // TARA & SES Demo onlystatic HUINT8 TempLicenseBuf[1024];
{
	NOVA_memset(pDrmListener->pLicense, 0, sizeof(HUINT8)*ulLicenseLength+1);
	apk_cas_nagra_GetTempLicenseBuf(pDrmListener->pLicense, ulLicenseLength);
}
#endif

	NOVA_PORT_LicenseNotifier(pDrmListener, TRUE);
	/* When this callback returned, NovaspreadServer does no longer access the memory of this license. So it can be released */
	nova_drm_RemoveLicense(pDrmListener);
}

static void nova_drm_KeyHandlerNotifier(HUINT8 *pucData)
{
	int i = 0;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmLicense = NULL;
	NOVA_CAS_KeyHandlerInfo_t *pKeyHandler = NULL;
	NOVA_CAS_KeyHandlerInfo_t stKeyHandler;	
	HUINT8 ucData[MAX_NOVA_DATA_SIZE];

	if(pucData == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pucData is null \n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	NOVA_memset(&ucData[0], 0, sizeof(ucData));
	NOVA_memcpy(&ucData[0], pucData, MAX_NOVA_DATA_SIZE);

	NOVA_memset(&stKeyHandler, 0, sizeof(NOVA_CAS_KeyHandlerInfo_t));
	NOVA_memcpy(&stKeyHandler, &ucData[0], sizeof(NOVA_CAS_KeyHandlerInfo_t));

	HxLOG_Info("[%s:%d]-%s- ulTransportSessionId(%d), ulKeyHandler(0x%08x), ulKeyLength(%d)\n"
										,__FUNCTION__
										, __LINE__
										, NOVA_CAS_DRM_INFO
										, stKeyHandler.ulTransportSessionId
										, stKeyHandler.ulKeyHandler
										, stKeyHandler.ulKeyLength);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	// license후에 keyHandler가 올라오기때문에 license로 비교해도된다.
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pDrmLicense = &pCas->stNotifier.stTs[i].stDrmLicense;
		if(pDrmLicense == NULL)
		{
			continue;
		}
		HxLOG_Info("*\t [%d] tsid(%d)(%d)\n", i, pDrmLicense->ulTransportSessionId, stKeyHandler.ulTransportSessionId);
		if(pDrmLicense->ulTransportSessionId == stKeyHandler.ulTransportSessionId)
		{
			pKeyHandler = &pCas->stNotifier.stTs[i].stKeyHandler;
			if(pKeyHandler == NULL)
			{
				continue;
			}
			pKeyHandler->ulTransportSessionId = stKeyHandler.ulTransportSessionId;
			pKeyHandler->ulKeyHandler = stKeyHandler.ulKeyHandler;
			if(stKeyHandler.ulKeyLength > 0)
			{
				pKeyHandler->ulKeyLength = stKeyHandler.ulKeyLength;
				NOVA_memset(&pKeyHandler->ucKey[0], 0, sizeof(pKeyHandler->ucKey));
				NOVA_memcpy(&pKeyHandler->ucKey[0], &stKeyHandler.ucKey[0], sizeof(pKeyHandler->ucKey));
			}
#if defined(CONFIG_DEBUG)
			nova_drm_PrintKeyHandlerInfo(pKeyHandler);
#endif				
			break;
		}
	}
}

int NOVA_CAS_DRM_DrmSetParameters (HUINT32 ulTransportSessionId, NovaspreadTDrmLicenseParameters * pLicenseParameters)
{
	HERROR hRet = ERR_OK;

	HxLOG_Info("[%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	if(ulTransportSessionId < 0)
	{
		HxLOG_Error("[%s:%d] -%s- ulTransportSessionId is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NOVA_BOOL(ERR_FAIL);
	}

	if(pLicenseParameters == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pLicenseParameters is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NOVA_BOOL(ERR_FAIL);
	}

#if defined(CONFIG_DEBUG)
//	nova_drm_printAllLicenseInfo();
	HxLOG_Debug("* \taTransportSessionId(0x%08x)\n", ulTransportSessionId);
	NOVA_CAS_DRM_PrintLicenseParameters(pLicenseParameters);
#endif

	hRet = APK_CAS_NAGRA_PRM_NovaDrmSetParameters(ulTransportSessionId
							, pLicenseParameters->OldLicense.License
							, pLicenseParameters->OldLicense.LicenseLength
							, pLicenseParameters->CollectionId
							, pLicenseParameters->Duration
							, pLicenseParameters->UsageRules
							, pLicenseParameters->UsageRulesLength);

	return NOVA_BOOL(hRet);
}

int  NOVA_CAS_DRM_DrmSetLicenseChangeListener (HUINT32  ulTransportSessionId, NovaspreadTDrmLicenseChangeListener Listener,  void * pContext)
{
	HERROR hRet = ERR_OK;
	NOVA_CAS_DrmLicenseNotifier_t *pDrmListener = NULL;

	HxLOG_Info("[%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	if(ulTransportSessionId < 0)
	{
		HxLOG_Error("[%s:%d] -%s- ulTransportSessionId is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NOVA_BOOL(ERR_FAIL);
	}

	// TransportSessionId에 대한 listener가 NOVASPREAD_NULL인경우 init를 한다.
	if(Listener == NOVASPREAD_NULL)
	{
		HxLOG_Info("[%s:%d]-%s- Drm license will be remove because License from nova-Server is NULL\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);
		nova_drm_ReleaseDrmLisntener(ulTransportSessionId);
		NOVA_CAS_ReleaseTransportSessionInfo(ulTransportSessionId);
		return NOVA_BOOL(ERR_OK);
	}

	if(pContext == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return NOVA_BOOL(ERR_FAIL);
	}

#if defined(CONFIG_DEBUG)
	nova_drm_printAllLicenseInfo();
#endif

	// 동일 TSID가 존재하는경우 어떻게 처리할지.
	pDrmListener = nova_drm_GetListenerFromTsId(ulTransportSessionId);

	if(pDrmListener == NULL)
	{
		pDrmListener = nova_drm_CreateListener();
		if(pDrmListener == NULL)
		{
			HxLOG_Error("[%s:%d] -%s- pDrmListener is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
			return NOVA_BOOL(ERR_FAIL);
		}
	}
	else
	{
		HxLOG_Error("[%s:%d] -%s- pDrmListener is already exist\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
	}

	HxLOG_Debug("\tpContext(0x%08x), ulTransportSessionId(0x%08x)\n", pContext, ulTransportSessionId);

	pDrmListener->ulTransportSessionId = ulTransportSessionId;
	pDrmListener->pContext = pContext;
	pDrmListener->DrmLicenseChangeListener = Listener;
#if defined(CONFIG_DEBUG)
	nova_drm_printAllLicenseInfo();
#endif

	return NOVA_BOOL(hRet);
}

void NOVA_CAS_DRM_init(void)
{
	int i = 0;
	
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		nova_drm_InitListenerFromIndex(i);
	}

	(void)APK_CAS_NAGRA_UnregisterNovaLicenseNotifier();
	(void)APK_CAS_NAGRA_RegisterNovaLicenseNotifier((APK_CAS_NAGRA_NovaLicenseNotifier_t) nova_drm_LicenseNotifier);

	(void)APK_CAS_NAGRA_UnregisterNovaKeyHandlerNotifier();
	(void)APK_CAS_NAGRA_RegisterNovaKeyHandlerNotifier((APK_CAS_NAGRA_NovaKeyHandlerNotifier_t) nova_drm_KeyHandlerNotifier);	
}

void NOVA_CAS_DRM_PrintLicenseParameters(NovaspreadTDrmLicenseParameters *pDrmLicenseParameters)
{
	nova_drm_PrintLicenseParameters(pDrmLicenseParameters);
}

int NOVA_CAS_DRM_ReleaseDrmLisntener(HUINT32  ulTransportSessionId)
{
	return nova_drm_ReleaseDrmLisntener(ulTransportSessionId);
}

HINT32 NOVA_CAS_DRM_GetDrmListenerIndexFromTsId(HUINT32  ulTransportSessionId)
{
	return nova_drm_GetDrmListenerIndexFromTsId(ulTransportSessionId);
}

HUINT32 NOVA_CAS_DRM_GetKeyHandlerFromTsId(HUINT32 ulTransportSessionId)
{
	return nova_drm_GetKeyHandlerFromTsId(ulTransportSessionId);
}

NOVA_CAS_KeyHandlerInfo_t *  NOVA_CAS_DRM_GetKeyInfoFromTsId(HUINT32 ulTransportSessionId)
{
	return nova_drm_GetKeyHandlerInfoFromTsId(ulTransportSessionId);
}
