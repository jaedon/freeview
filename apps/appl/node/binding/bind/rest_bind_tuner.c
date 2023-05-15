/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_tuner.c
	@brief

	Description:  									\n
	Module: node/binding/bind					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <dapi.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32				 	ulTunerNum;
	DxAntennaConnection_e 	eConnectionType;
	RestTunerInst_t			*pstTunerArray;
} restTunerContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_bRestBindTuner_Inited = FALSE;
STATIC restTunerContext_t			 s_stRestBindTuner_Context;
STATIC HxList_t						*s_pstRestBindTuner_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___

/**
 * Tuner의 상태가 변경될 수 있으므로, Static하게 Tuner 정보를 들고 있으면 안됨..
 * 현재는 요청이 올때마다 Tuner의 정보를 Get해야 함..
 * 혹은 Tuner의 상태 변경 이벤트가 올라올때마다 갱신해야 함
*/
#if 0
STATIC RestTunerInst_t *rest_bind_tuner_GetTuner (HUINT32 ulTunerId)
{
	if (TRUE == s_bRestBindTuner_Inited)
	{
		if ((ulTunerId < s_stRestBindTuner_Context.ulTunerNum) && (NULL != s_stRestBindTuner_Context.pstTunerArray))
		{
			return &(s_stRestBindTuner_Context.pstTunerArray[ulTunerId]);
		}
	}

	return NULL;	
}
#endif
#define _____NOTIFIER_____


STATIC void rest_bind_tuner_InitContext (void)
{
	HERROR				 hErr;
	
	if (TRUE != s_bRestBindTuner_Inited)
	{
		HUINT32				 	ulTunerNum = 0;
		DxAntennaConnection_e 	eConnectionType;
		
		hErr = APK_SYSINFO_TUNER_GetDeviceNum(&ulTunerNum);
		if ((ERR_OK != hErr) || (0 == ulTunerNum))
		{
			HxLOG_Error("!!! APK_SYSINFO_TUNER_GetDeviceNum err: !!!\n");
			return;
		}

		hErr = APK_SCENARIO_GetAntennaConnection(&eConnectionType);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("APK_SCENARIO_GetAntennaConnection err:\n");
			eConnectionType = eDxANTCONN_ONECABLE;
		}


		/* Tuner의 상태가 변경될 수 있으므로, Static하게 Tuner 정보를 들고 있으면 안됨..
		현재는 요청이 올때마다 Tuner의 정보를 Get해야 함..
		혹은 Tuner의 상태 변경 이벤트가 올라올때마다 갱신해야 함
		*/
#if 0
		HINT32				nTunerIdx;
		RestTunerInst_t 	*pstTuner, *pstTunerArray = NULL;

		pstTunerArray = (RestTunerInst_t *)HLIB_STD_MemCalloc(sizeof(RestTunerInst_t) * ulTunerNum);
		if (NULL == pstTunerArray)
		{
			HxLOG_Error("Memory allocation failed:\n");
			return;
		}

		for (nTunerIdx = 0; nTunerIdx < (HINT32)ulTunerNum; nTunerIdx++)
		{
			pstTuner = &(pstTunerArray[nTunerIdx]);

			pstTuner->nTunerId = nTunerIdx;
			hErr = APK_SYSINFO_TUNER_GetDevType( nTunerIdx, &( pstTuner->eTunerType ) );
			if( ERR_OK != hErr )
			{
				HxLOG_Warning("APK_SYSINFO_TUNER_GetDevType err: TunerID(%d)\n", nTunerIdx);
			}
			
			// Locked
			hErr = APK_SYSINFO_TUNER_IsLocked( nTunerIdx, &( pstTuner->bLocked ) );
			if( ERR_OK != hErr )
			{
				HxLOG_Warning("APK_SYSINFO_TUNER_IsLocked err: TunerID(%d)\n", nTunerIdx);
			}
			
			// Tuner가 Locking 되어 있을때는 아래의 추가 정보
			if( pstTuner->bLocked )
			{
				// Signal Level
				hErr = APK_SYSINFO_TUNER_GetSignalLevel( nTunerIdx, &( pstTuner->nSignalLevel ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetSignalLevel err: TunerID(%d)\n", nTunerIdx);
				}
			
				// Signal Quality
				hErr = APK_SYSINFO_TUNER_GetSignalQuality( nTunerIdx, &( pstTuner->nSignalQuality ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetSignalQuality err: TunerID(%d)\n", nTunerIdx);
				}
			
				// agc
				hErr = APK_SYSINFO_TUNER_GetAgc( nTunerIdx, &( pstTuner->nAgc ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetAgc err: TunerID(%d)\n", nTunerIdx);
				}
			
				// ber
				hErr = APK_SYSINFO_TUNER_GetBer( nTunerIdx, &( pstTuner->fBer ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetBer err: TunerID(%d)\n", nTunerIdx);
				}
			
				// snr
				hErr = APK_SYSINFO_TUNER_GetSnr( nTunerIdx, &( pstTuner->fSnr ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetSnr err: TunerID(%d)\n", nTunerIdx);
				}
			
				// Input Power
				hErr = APK_SYSINFO_TUNER_GetInputPower( nTunerIdx, &( pstTuner->fInputPower ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetInputPower err: TunerID(%d)\n", nTunerIdx);
				}
			
				// Tune Param
				hErr = APK_SYSINFO_TUNER_GetTuneParam( nTunerIdx, &( pstTuner->stTuneParam ) );
				if( ERR_OK != hErr )
				{
					HxLOG_Warning("APK_SYSINFO_TUNER_GetTuneParam err: TunerID(%d)\n", nTunerIdx);
				}
			}
		}
#endif
		s_stRestBindTuner_Context.ulTunerNum		= ulTunerNum;
		s_stRestBindTuner_Context.eConnectionType 	= eConnectionType;
//		s_stRestBindTuner_Context.pstTunerArray		= pstTunerArray;

		s_bRestBindTuner_Inited = TRUE;
	}
}



#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_tuner_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindTuner_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindTuner_Notifier = HLIB_LIST_Append(s_pstRestBindTuner_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_tuner_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t			 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_tuner_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_tuner_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindTuner_Notifier = HLIB_LIST_Remove(s_pstRestBindTuner_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_tuner_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t			 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_tuner_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC RestTunerInst_t *rest_bind_tuner_GetTunerStatus (HINT32 nTunerId)
{
	RestTunerInst_t			*pstStatus = NULL;
	HERROR					 hErr;

	pstStatus = (RestTunerInst_t *)HLIB_MEM_Calloc(sizeof(RestTunerInst_t));
	if (NULL == pstStatus)
	{
		HxLOG_Error("RestPlayerStatus_t allocation failed:\n");
		return NULL;
	}


	// Tuner ID
	pstStatus->nTunerId = nTunerId;

	// DeliveryType
	// TODO: 아래 함수 호출시 nTunerId가 없는 값일 경우 Crash !!!!!!!!!!
	hErr = APK_SYSINFO_TUNER_GetDevType( nTunerId, &( pstStatus->eTunerType ) );
	if( ERR_OK != hErr )
	{
		HxLOG_Warning("APK_SYSINFO_TUNER_GetDevType err: TunerID(%d)\n", nTunerId);
	}

	// Locked
	hErr = APK_SYSINFO_TUNER_IsLocked( nTunerId, &( pstStatus->bLocked ) );
	if( ERR_OK != hErr )
	{
		HxLOG_Warning("APK_SYSINFO_TUNER_IsLocked err: TunerID(%d)\n", nTunerId);
	}

	// Tuner가 Locking 되어 있을때는 아래의 추가 정보
	if( pstStatus->bLocked )
	{
		// Signal Level
		hErr = APK_SYSINFO_TUNER_GetSignalLevel( nTunerId, &( pstStatus->nSignalLevel ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetSignalLevel err: TunerID(%d)\n", nTunerId);
		}

		// Signal Quality
		hErr = APK_SYSINFO_TUNER_GetSignalQuality( nTunerId, &( pstStatus->nSignalQuality ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetSignalQuality err: TunerID(%d)\n", nTunerId);
		}

		// agc
		hErr = APK_SYSINFO_TUNER_GetAgc( nTunerId, &( pstStatus->nAgc ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetAgc err: TunerID(%d)\n", nTunerId);
		}

		// ber
		hErr = APK_SYSINFO_TUNER_GetBer( nTunerId, &( pstStatus->fBer ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetBer err: TunerID(%d)\n", nTunerId);
		}

		// snr
		hErr = APK_SYSINFO_TUNER_GetSnr( nTunerId, &( pstStatus->fSnr ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetSnr err: TunerID(%d)\n", nTunerId);
		}

		// Input Power
		hErr = APK_SYSINFO_TUNER_GetInputPower( nTunerId, &( pstStatus->fInputPower ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetInputPower err: TunerID(%d)\n", nTunerId);
		}

		// Tune Param
		hErr = APK_SYSINFO_TUNER_GetTuneParam( nTunerId, &( pstStatus->stTuneParam ) );
		if( ERR_OK != hErr )
		{
			HxLOG_Warning("APK_SYSINFO_TUNER_GetTuneParam err: TunerID(%d)\n", nTunerId);
		}
	}

	return pstStatus;
}


STATIC HCHAR *rest_bind_tuner_CmdGetOneTunerStatus (RestInstance_t *pstInst, HINT32 nTunerId, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestTunerInst_t			*pstStatus = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;


	// nTunerId가 Tuner 갯수보다 많으면 없는 Resource임
	if(s_stRestBindTuner_Context.ulTunerNum <= nTunerId)
	{
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;			szErrString = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;		
	}


	pstStatus = rest_bind_tuner_GetTunerStatus(nTunerId);
	if (NULL == pstStatus)
	{
		HxLOG_Error("Resource Not Found:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NODATA;
		goto END_FUNC;
	}



	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Play Status
		(void)HxJSONWRITER_StringValue(nJWriter, "tunerStatus");
		(void)rest_datatype_JsonizeTunerStatus(nJWriter, pstStatus, szMember);

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	nJWriter = 0;

END_FUNC:
	if (0 != nJWriter)
	{
		rest_bind_CloseJsonWriter(nJWriter);
	}

	if (NULL != pstStatus)
	{
		HLIB_STD_MemFree(pstStatus);
	}

	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}


STATIC HCHAR *rest_bind_tuner_CmdGetTunerInfo (RestInstance_t *pstInst, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	// 결과값을 JSON화...
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Tuners Overview
		HxJSONWRITER_StringValue(nJWriter, "tunersOverview");
		rest_datatype_JsonizeTunerOverview(nJWriter, (HINT32)s_stRestBindTuner_Context.ulTunerNum, s_stRestBindTuner_Context.eConnectionType, szMember);
		

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

	return szResultStr;
}

STATIC HCHAR *rest_bind_tuner_CmdGetTunerList (RestInstance_t *pstInst, HINT32 nStartIdx, HINT32 nCnt, HCHAR *szMember)
{
	HINT32					nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	HINT32					i = 0;
	RestTunerInst_t			*pstStatus = NULL;
	HINT32					nCount = 0;
	HINT32					 nErrCode = REST_ERR_CODE_OK;
	HCHAR					*szErrString = REST_ERR_STRING_OK;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	nCount = nCnt;
	if( nCount <= 0 || nCount > 10 )
	{
		nCount = 10;
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{

		HxJSONWRITER_Array(nJWriter, "Tuners");

		for( i = nStartIdx ; i < nStartIdx + nCount ; ++i )
		{
			if( i >= s_stRestBindTuner_Context.ulTunerNum )
			{
				break;
			}

			pstStatus = rest_bind_tuner_GetTunerStatus(i);
			if (NULL == pstStatus)
			{
				HxLOG_Error("Resource Not Found:\n");
				nErrCode = REST_ERR_CODE_NOMEM;
				szErrString = REST_ERR_STRING_NOMEM;
				break;
			}

			(void)rest_datatype_JsonizeTunerStatus(nJWriter, pstStatus, szMember);
		}
		HxJSONWRITER_ArrayEnd(nJWriter);

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, nErrCode, szErrString);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

	return szResultStr;
}


// URL: http://127.0.0.1/stb-api/v1/tuners
STATIC HERROR rest_bind_tuner_CbGet (void *pvArg)
{
	HCHAR			*szResult = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t		jsArgs = NULL;
	HINT32			nStartIdx = -1, nCnt = -1;
	HCHAR			*szMember = NULL;
	RestInstance_t	*pstInst = NULL;

	if (NULL != pstMsg)
	{
		HCHAR				*szArgJson = ( HCHAR * )pstMsg->aulArgs[0];

		pstInst = rest_task_GetInstance(pstMsg->szSessionId);
		if (NULL == pstInst)
		{
			HxLOG_Error("rest_task_GetInstance err:\n");
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
			goto END_FUNC;
		}

		jsArgs = HLIB_JSON_Decode((const HCHAR *)szArgJson);
		if( NULL == jsArgs )
		{
			HxLOG_Error("Invalid JSON err:\n");
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
			goto END_FUNC;
		}

		(void)rest_bind_GetNumberValueFromJson(jsArgs, "startidx", &nStartIdx);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "cnt", &nCnt);
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);

		if( nStartIdx < 0 )
		{
			// 전체 Tuner의 overview
			szResult = rest_bind_tuner_CmdGetTunerInfo(pstInst, szMember);
		}
		else
		{
			if(	nStartIdx >= s_stRestBindTuner_Context.ulTunerNum )
			{
				// startIdx 값이 전체 tuner 갯수보다 같거나 큰경우는 error
				HxLOG_Error("Invalid Value err:\n");
				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_RESOURCE_NOTFOUND, REST_ERR_STRING_RESOURCE_NOTFOUND, TRUE);
				goto END_FUNC;
			}
			else
			{
				// TunerList
				szResult = rest_bind_tuner_CmdGetTunerList(pstInst, nStartIdx, nCnt, szMember);
			}
		}

	}

END_FUNC:
	if (NULL != pstMsg)			{ pstMsg->pvResult = (void *)szResult; }
	rest_task_SyncOut();

	return ERR_OK;
}



// URL: http://127.0.0.1/stb-api/v1/tuners/0[?Options]
STATIC HERROR rest_bind_tuner_CbGetOne (void *pvArg)
{
	HCHAR			*szResult = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t		jsArgs = NULL;
	HCHAR			*szMember = NULL;
	RestInstance_t	*pstInst = NULL;


	if (NULL != pstMsg)
	{
		HUINT32				 nTunerId = (HINT32)pstMsg->aulArgs[0];
		HCHAR				*szArgJson = ( HCHAR * )pstMsg->aulArgs[1];

		pstInst = rest_task_GetInstance(pstMsg->szSessionId);
		if (NULL == pstInst)
		{
			HxLOG_Error("rest_task_GetInstance err:\n");
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
			goto END_FUNC;
		}


		jsArgs = HLIB_JSON_Decode((const HCHAR *)szArgJson);
		if( NULL == jsArgs )
		{
			HxLOG_Error("Invalid JSON err:\n");
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
			goto END_FUNC;
		}
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);
		
		// URL: http://127.0.0.1/stb-api/v1/tuners/0[?Options]
		szResult = rest_bind_tuner_CmdGetOneTunerStatus(pstInst, nTunerId, szMember);

	}

END_FUNC:
	if (NULL != pstMsg)			{ pstMsg->pvResult = (void *)szResult; }
	rest_task_SyncOut();

	return ERR_OK;
}



#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_tuner_Init (void)
{
	STATIC HBOOL		 s_bFirst = FALSE;
	
	if (TRUE != s_bFirst)
	{
		HxSTD_MemSet(&s_stRestBindTuner_Context, 0, sizeof(restTunerContext_t));
		
		// Initialize the APPKIT
		//(void)APK_SYSINFO_TUNER_RegisterListener(
		(void)APK_SYSINFO_TUNER_Init();

		s_bFirst = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_TUNER_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_tuner_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_TUNER_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_tuner_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// GET http://127.0.0.1:37564/stb-api/v1/tuners
char *REST_BIND_TUNER_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;

	rest_bind_tuner_InitContext();
	
	hErr = rest_task_RequestToExecute(rest_bind_tuner_CbGet, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		goto ERROR;
	}

	rest_task_SyncIn();

	szResult = (char *)stMsg.pvResult;
	if (NULL == szResult)
	{
		HxLOG_Error("cmd not executed:\n");
		goto ERROR;
	}

	rest_datatype_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}

// GET http://127.0.0.1:37564/stb-api/v1/tuners/0
char *REST_BIND_TUNER_GetOne (char *szSessionId, int nTunerId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nTunerId;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;

	rest_bind_tuner_InitContext();
	
	hErr = rest_task_RequestToExecute(rest_bind_tuner_CbGetOne, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		goto ERROR;
	}

	rest_task_SyncIn();

	szResult = (char *)stMsg.pvResult;
	if (NULL == szResult)
	{
		HxLOG_Error("cmd not executed:\n");
		goto ERROR;
	}

	rest_datatype_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}

