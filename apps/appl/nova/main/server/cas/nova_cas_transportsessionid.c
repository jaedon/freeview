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
#include <nova_cas.h>
#include "nova_errcode.h"
#include "nova_utils.h"

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

static void nova_cas_PrintTransportSessionInfo(void)
{
	int i = 0; 
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

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
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		HxLOG_Info("*\t TransportSessionInfo[%d] hAction(0x%08x), ulTransportSessionId(%d), usOnId(0x%08x), usTsId(0x%08x), usSvcId(0x%08x), usAudioPid(0x%08x), usVideoPid(0x%08x)\n", 
			i, pTsIdInfo->hAction, pTsIdInfo->ulTransportSessionId, pTsIdInfo->usOnId, pTsIdInfo->usTsId, pTsIdInfo->usSvcId, pTsIdInfo->usAudioPid, pTsIdInfo->usVideoPid);
	}
}

static void nova_tsid_TransportSessionInfoInitByIndex(HUINT32 ulIndex)
{
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

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

	pTsIdInfo = &pCas->stNotifier.stTs[ulIndex].stTsIdInfo;
	if(pTsIdInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pTsIdInfo is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	pTsIdInfo->hAction = HANDLE_NULL;
	pTsIdInfo->ulTransportSessionId = NOVA_INVALID_TSID;
	pTsIdInfo->usOnId = NOVA_INVALID_TRIPLEID;
	pTsIdInfo->usTsId = NOVA_INVALID_TRIPLEID;
	pTsIdInfo->usSvcId = NOVA_INVALID_TRIPLEID;
	pTsIdInfo->usAudioPid = NOVA_INVALID_PID;	
	pTsIdInfo->usVideoPid = NOVA_INVALID_PID;	

	return;
}

static NOVA_CAS_TransportSessionInfo_t * nova_tsid_GetTransportSessionInfoFromTripleId(HUINT32 hAction, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId)
{
	int i = 0;
	HBOOL bValidFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

	HxLOG_Info("[%s:%d] -%s- hAction(0x%08x), usOnId(0x%x), usTsId(0x%x), usSvcId(0x%x)\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO, hAction, usOnId, usTsId, usSvcId);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NULL;
	}

	// 동일한 정보가 존재하는지 check함.d
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		if(pTsIdInfo->hAction == hAction && pTsIdInfo->usOnId == usOnId && pTsIdInfo->usTsId == usTsId && pTsIdInfo->usSvcId == usSvcId)
		{
			HxLOG_Error("[%s:%d] -%s- tsid is already exist ---- fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
			bValidFlag = TRUE;
			break;
		}
	}

	if(bValidFlag)
	{
		HxLOG_Info("*\t hAction(0x%08x), ulTransportSessionId(%d), ulOnId(0x%08x), ulTsId(0x%08x), ulSvcId(0x%08x), usAudioPid(0x%08x), usVideoPid(0x%08x)\n", 
			pTsIdInfo->hAction, pTsIdInfo->ulTransportSessionId, pTsIdInfo->usOnId, pTsIdInfo->usTsId+NOVA_TRANSPORTSESSIONID_SUM, pTsIdInfo->usSvcId, pTsIdInfo->usAudioPid, pTsIdInfo->usVideoPid);

		return pTsIdInfo;
	}
	else
	{
		HxLOG_Error("[%s:%d] -%s- tsid is not exist ---- fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NULL;
	}
}

static HUINT32 nova_tsid_GetTransportSessionIdFromTripleId(HUINT32 hAction, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId)
{
	int i = 0;
	HBOOL bValidFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

	HxLOG_Info("[%s:%d] -%s- hAction(0x%08x), usOnId(0x%x), usTsId(0x%x), usSvcId(0x%x)\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO, hAction, usOnId, usTsId, usSvcId);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NOVA_TRANSPORTSESSIONID_SUM;
	}

	// 동일한 정보가 존재하는지 check함.d
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		if( pTsIdInfo->hAction == hAction && pTsIdInfo->usOnId == usOnId && pTsIdInfo->usTsId == usTsId && pTsIdInfo->usSvcId == usSvcId)
		{
			HxLOG_Info("[%s:%d] -%s- tsid is got from obama\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);
			bValidFlag = TRUE;
			break;
		}
	}

	if(bValidFlag)
	{
		HxLOG_Info("*\t hAction(0x%08x), ulTransportSessionId(%d), ulOnId(0x%08x), ulTsId(0x%08x), ulSvcId(0x%08x)\n", 
			pTsIdInfo->hAction, pTsIdInfo->ulTransportSessionId, pTsIdInfo->usOnId, pTsIdInfo->usTsId+NOVA_TRANSPORTSESSIONID_SUM, pTsIdInfo->usSvcId);

		return pTsIdInfo->ulTransportSessionId;
	}
	else
	{
		HxLOG_Error("[%s:%d] -%s- tsid is not exist ---- fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NOVA_TRANSPORTSESSIONID_SUM;
	}
}

static NOVA_CAS_TransportSessionInfo_t * nova_tsid_GetValidTransportSessionInfo(HUINT32 hAction, HUINT32 ulTransportSessionId, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId)
{
	int i = 0;
	HBOOL bValidFlag = FALSE;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

	HxLOG_Info("[%s:%d] -%s- hAction(0x%08x), ulTransportSessionId(%d), usOnId(0x%x), usTsId(0x%x), usSvcId(0x%x)\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO, hAction, ulTransportSessionId, usOnId, usTsId, usSvcId);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NULL;
	}

	// 동일한 정보가 존재하는지 check함.
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		if(pTsIdInfo->hAction == hAction && pTsIdInfo->ulTransportSessionId == ulTransportSessionId && pTsIdInfo->usOnId == usOnId && pTsIdInfo->usTsId == usTsId && pTsIdInfo->usSvcId == usSvcId)
		{
			HxLOG_Error("[%s:%d] -%s- tsid is already exist ---- fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
			bValidFlag = TRUE;			
			break;
		}
	}

	if(bValidFlag)
	{
		HxLOG_Error("[%s:%d] -%s- tsid is already exist\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return pTsIdInfo;
	}

	// Get Valid TsIdInfo
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		if(pTsIdInfo->ulTransportSessionId == NOVA_INVALID_TSID)
		{
			bValidFlag = TRUE;
			break;
		}
	}

	if(bValidFlag == FALSE)
	{
		HxLOG_Error("[%s:%d] -%s- bValidFlag is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NULL;
	}

	return pTsIdInfo;;
}

static int nova_tsid_GetTransportSessionInfoIndexFromTsId(HUINT32  ulTransportSessionId)
{
	int i = 0;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

	HxLOG_Info("[%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return -1;
	}

	// Get Valid TsIdInfo
	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		if(pTsIdInfo->ulTransportSessionId == ulTransportSessionId)
		{
			return i;
		}
	}

	return NOVA_MAX_NUM_OF_TS;
}

static void nova_tsid_ReleaseTransportSessionInfo(HUINT32  ulTransportSessionId)
{
	HUINT32 ulIndex = 0;

	HxLOG_Info("[%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	ulIndex = nova_tsid_GetTransportSessionInfoIndexFromTsId(ulTransportSessionId);
	if(ulIndex == NOVA_INVALID_INDEX)
	{
		HxLOG_Error("[%s:%d] -%s- ulIndex is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	nova_tsid_TransportSessionInfoInitByIndex(ulIndex);

	return;
}

static void nova_cas_SetTransportSessionInfo(NOVA_CAS_TransportSessionInfo_t *pTsIdInfo, HUINT32 hAction, HUINT32 ulTransportSessionId, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId, HUINT16 usAudioPid, HUINT16 usVideoPid)
{
	if(pTsIdInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pTsIdInfo is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return;
	}

	pTsIdInfo->hAction = hAction;
	pTsIdInfo->ulTransportSessionId = ulTransportSessionId + NOVA_TRANSPORTSESSIONID_SUM; // 다른 transportSessionId와 충돌이 안나게 NOVA_TRANSPORTSESSIONID_SUM을 더해준다..다시 확인 필요 TODO
	pTsIdInfo->usOnId = usOnId;
	pTsIdInfo->usTsId = usTsId;
	pTsIdInfo->usSvcId = usSvcId;
	pTsIdInfo->usAudioPid = usAudioPid;
	pTsIdInfo->usVideoPid = usVideoPid;	

	HxLOG_Info("*\t hAction(0x%08x), ulTransportSessionId(%d), usOnId(0x%08x), usTsId(0x%08x), usSvcId(0x%08x), usAudioPid(0x%08x), usVideoPid(0x%08x)\n", 
		pTsIdInfo->hAction, pTsIdInfo->ulTransportSessionId, pTsIdInfo->usOnId, pTsIdInfo->usTsId, pTsIdInfo->usSvcId, pTsIdInfo->usAudioPid, pTsIdInfo->usVideoPid);	
}

static void nova_tsid_TransportSessionInfoNotifier(HUINT32 hAction, HUINT32 ulTransportSessionId, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId, HUINT16 usAudioPid, HUINT16 usVideoPid)
{
	NOVA_CAS_TransportSessionInfo_t * pTsIdInfo = NULL;

	HxLOG_Info("[%s:%d]-%s-\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_INFO);

	pTsIdInfo = nova_tsid_GetValidTransportSessionInfo(hAction, ulTransportSessionId, usOnId, usTsId, usSvcId);
	if(pTsIdInfo == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pTsIdInfo is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_DRM_ERR);
		return;
	}

	nova_cas_SetTransportSessionInfo(pTsIdInfo, hAction, ulTransportSessionId, usOnId, usTsId, usSvcId, usAudioPid, usVideoPid);
}

void NOVA_CAS_TransportSessionInfoInit(void)
{
	int i = 0;

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		nova_tsid_TransportSessionInfoInitByIndex(i);
	}

	(void)APK_CAS_NAGRA_UnregisterNovaTransportSessionInfoNotifier();
	(void)APK_CAS_NAGRA_RegisterNovaTransportSessionInfoNotifier((APK_CAS_NAGRA_NovaTransportSessionInfoNotifier_t) nova_tsid_TransportSessionInfoNotifier);	
}

//---------------------------------------------------------------------------------------------

void NOVA_CAS_ReleaseTransportSessionInfo(HUINT32  ulTransportSessionId)
{
	return nova_tsid_ReleaseTransportSessionInfo(ulTransportSessionId);
}

NOVA_CAS_TransportSessionInfo_t * NOVA_CAS_GetTransportSessionInfoFromTripleId(HUINT32 hAction, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId)
{
	return nova_tsid_GetTransportSessionInfoFromTripleId(hAction, usOnId, usTsId, usSvcId);
}

HUINT32 NOVA_CAS_GetTransportSessionIdFromTripleId(HUINT32 hAction, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId)
{
	return nova_tsid_GetTransportSessionIdFromTripleId(hAction, usOnId, usTsId, usSvcId);
}

void NOVA_CAS_PrintTransportSessionInfo(void)
{
	nova_cas_PrintTransportSessionInfo();
}
