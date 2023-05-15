/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>

#include <rema_int.h>

#include "rema_MOD.h"
#include "rema_module_Internal.h"

#define ___DEFINES___
#define DSTR_NSLOOKUP_DiagnosticsState            "DiagnosticsState"
#define DSTR_NSLOOKUP_Interface					  "Interface"
#define DSTR_NSLOOKUP_HostName					  "HostName"
#define DSTR_NSLOOKUP_DNSServer					  "DNSServer"
#define DSTR_NSLOOKUP_Timeout					  "Timeout"
#define DSTR_NSLOOKUP_NumberOfRepetitions		  "NumberOfRepetitions"
#define DSTR_NSLOOKUP_SuccessCount				  "SuccessCount"

#define DSTR_NSLOOKUP_RS_Status                   "Status"
#define DSTR_NSLOOKUP_RS_AnswerType				  "AnswerType"
#define DSTR_NSLOOKUP_RS_HostNameReturned		  "HostNameReturned"
#define DSTR_NSLOOKUP_RS_IPAddresses			  "IPAddresses"
#define DSTR_NSLOOKUP_RS_DNSServerIP			  "DNSServerIP"
#define DSTR_NSLOOKUP_RS_ResponseTime			  "ResponseTime"


typedef enum NSLookupDiagState_e
{
	eNSLDiagST_None = 0,
	eNSLDiagST_Requested, 
	eNSLDiagST_Complete, 
	eNSLDiagST_Error_DNSServerNotResolved, 
	eNSLDiagST_Error_Internal, 
	eNSLDiagST_Error_Other, 
}NSLookupDiagState_e;

#define ___MACROS___

#define ___TYPEDEF___
typedef struct DiagNSLookupResult_t
{
	HCHAR			*pszStatus;
	HCHAR			*pszAnswerType;
	HCHAR			*pszHostNameReturned;
	HCHAR			*pszIPAddresses;
	HCHAR			*pszDNSServerIP;
	HUINT32			 unResponseTime;
}DiagNSLookupResult_t;

typedef struct DiagNSLookupInfo_t
{
	NSLookupDiagState_e eDiagState;
	HCHAR			szInterface[257];
	HCHAR			szHostName[257];
	HCHAR			szDNSServer[257];
	HUINT32			unTimeout;
	HUINT32			unNumberOfRepetitions;

	HUINT32			unSuccessCount;
	HUINT32			unResultNumberOfEntries;


	DiagNSLookupResult_t *pstResults;
	int				nResult;
	unsigned long   ulStartTime;
}DiagNSLookupInfo_t;


typedef struct ParameterInfo_t
{
	HCHAR			Reference[257];
	HCHAR           *pszSampleSeconds;
	HCHAR           *pszSuspectData;
	HCHAR           *pszValues;
}ParameterInfo_t;

typedef struct SampleSetInfo_t
{
	HCHAR			szName[129];
	HUINT32			unSampleInterval;
	HUINT32			unReportSamples;
	HUINT64			ullReportStartTime;
	HUINT64			ullReportEndTime;
	HCHAR           *pszSampleSeconds;
	//ParameterInfo_t *pstParamInfo;
	HxVector_t	*phvcParamInfo;
}SampleSetInfo_t;

typedef struct PeriodicStatisticsInfo_t
{
	HUINT32			unMinSampleInterval;
	HUINT32			unMaxReportSamples;
	//SampleSetInfo_t *pstSampeSet;
	HxVector_t		*phvcSampeSet;
}PeriodicStatisticsInfo_t;

typedef struct RMI_NSLookup_t
{
	Base_MOD_t stMOD;
	DiagNSLookupInfo_t *pstDiagNSLook;
} RMI_NSLookup_t;


typedef struct RMI_Statistics_t
{
	Base_MOD_t stMOD;
	PeriodicStatisticsInfo_t *pstStatisticsInfo;
} RMI_Statistics_t;

#define ___STATIC_VARIABLES___

RMI_NSLookup_t			g_stNsLookup;
RMI_Statistics_t		g_stStatisics;

#define ___INTERNAL_FUNCTIONS___

static HINT32 rema_MOD_Internal_NSLookup_Init(void)
{
	REM_FUNC_ENTER;

	g_stNsLookup.pstDiagNSLook = (DiagNSLookupInfo_t*) HLIB_STD_MemAlloc(sizeof(DiagNSLookupInfo_t));
	if(g_stNsLookup.pstDiagNSLook)
	{
		HxSTD_MemSet(g_stNsLookup.pstDiagNSLook, 0, sizeof(DiagNSLookupInfo_t));
	}

	REM_FUNC_LEAVE;

	return 0;
}
static HINT32 rema_MOD_Internal_NSLookup_Deinit(void)
{
	REM_FUNC_ENTER;

	if(g_stNsLookup.pstDiagNSLook)
		HLIB_STD_MemFree(g_stNsLookup.pstDiagNSLook  );

	REM_FUNC_LEAVE;

	return 0;
}


static HINT32 rema_MOD_Internal_NSLookup_checkCTX(void *pvCtx)
{
	REM_FUNC_ENTER;
	if(!pvCtx)
		return -1;

	if(!g_stNsLookup.pstDiagNSLook)
		return -1;

	if( pvCtx != g_stNsLookup.pstDiagNSLook)
		return -1;

	REM_FUNC_LEAVE;
	return 0;
}

static HINT32 NSLookupDiag_GetStateName(NSLookupDiagState_e eState, HCHAR *pszBuf, HINT32 nBuflen)
{
	HINT32 nRs =-1;
	switch (eState)
	{
#undef NSLDIAG_CASE_STATE
#define NSLDIAG_CASE_STATE(x) case eNSLDiagST_ ## x : nRs = 0; HxSTD_PrintToStrN(pszBuf, nBuflen, "%s", #x); break;
		NSLDIAG_CASE_STATE(None);
		NSLDIAG_CASE_STATE(Requested);
		NSLDIAG_CASE_STATE(Complete);
		NSLDIAG_CASE_STATE(Error_DNSServerNotResolved);
		NSLDIAG_CASE_STATE(Error_Internal);
		NSLDIAG_CASE_STATE(Error_Other);
#undef NSLDIAG_CASE_STATE
		default : 
			RemLOG_Debug("default(%d)\n", eState);
			break;
	}

	return nRs;
}

static HINT32 NSLookupDiag_SetStateName(const HCHAR *pszBuf , NSLookupDiagState_e *peState)
{
	HINT32 nRs =-1;

#undef NSLDIAG_CASE_STRCMP
#define NSLDIAG_CASE_STRCMP(x) if(nRs != 0 && !HxSTD_StrCmp(pszBuf, #x )) { *peState =  eNSLDiagST_ ## x ; nRs = 0; RemLOG_Debug("pszBuf(%s) %d \n", pszBuf, *peState);}
		NSLDIAG_CASE_STRCMP(None);
		NSLDIAG_CASE_STRCMP(Requested);
		NSLDIAG_CASE_STRCMP(Complete);
		NSLDIAG_CASE_STRCMP(Error_DNSServerNotResolved);
		NSLDIAG_CASE_STRCMP(Error_Internal);
		NSLDIAG_CASE_STRCMP(Error_Other);
#undef NSLDIAG_CASE_STRCMP

		if(nRs != 0 )
		{
			RemLOG_Debug("pszBuf(%d)\n", pszBuf);
		}
	return nRs;
}

void * Vecter_mk_Param(void *pvData)
{
	ParameterInfo_t *psParam = (ParameterInfo_t*) pvData;
	if(psParam)
	{
		memset(psParam, 0, sizeof(ParameterInfo_t));
	}
	return psParam;	
}

void * Vecter_rk_Param(void *pvData)
{
	ParameterInfo_t *psParam = (ParameterInfo_t*) pvData;
	if(psParam)
	{
		HLIB_STD_MemFree(psParam);
		psParam=NULL;
	}
	return psParam;	
}

void * Vecter_mk_SampeSet(void *pvData)
{
	SampleSetInfo_t *pstSampleSet = (SampleSetInfo_t *)pvData;
	if(pstSampleSet)
	{
		memset(pstSampleSet, 0, sizeof(SampleSetInfo_t));
		pstSampleSet->phvcParamInfo = HLIB_VECTOR_NewEx(NULL, sizeof(ParameterInfo_t *), 
			Vecter_mk_Param, Vecter_rk_Param, NULL);
	}
	return pstSampleSet;	
}

void * Vecter_rk_SampeSet(void *pvData)
{
	SampleSetInfo_t *pstSampleSet = (SampleSetInfo_t *)pvData;
	if(pstSampleSet)
	{
		if(pstSampleSet->phvcParamInfo)
		{
			HLIB_VECTOR_Delete(pstSampleSet->phvcParamInfo);
			g_stStatisics.pstStatisticsInfo->phvcSampeSet = NULL;
		}
		HLIB_STD_MemFree(pstSampleSet);
		pstSampleSet=NULL;
	}
	return pstSampleSet;
}
static HINT32 rema_MOD_Internal_Statisics_Init(void)
{
	PeriodicStatisticsInfo_t* pstInfo =NULL;

	REM_FUNC_ENTER;

	

	g_stStatisics.pstStatisticsInfo = (PeriodicStatisticsInfo_t*) HLIB_STD_MemAlloc(sizeof(PeriodicStatisticsInfo_t));
	if(g_stStatisics.pstStatisticsInfo)
	{
		HxSTD_MemSet(g_stStatisics.pstStatisticsInfo, 0, sizeof(PeriodicStatisticsInfo_t));
	}

	g_stStatisics.pstStatisticsInfo->unMinSampleInterval = 120;
	g_stStatisics.pstStatisticsInfo->unMaxReportSamples  = 1;
	g_stStatisics.pstStatisticsInfo->phvcSampeSet = HLIB_VECTOR_NewEx(NULL, sizeof(SampleSetInfo_t *),
		Vecter_mk_SampeSet, Vecter_rk_SampeSet, NULL);
	///

	 pstInfo = g_stStatisics.pstStatisticsInfo;
	if(pstInfo->phvcSampeSet)
	{
		SampleSetInfo_t * pstTemp = NULL;
		ParameterInfo_t * pstParam =NULL;
		pstTemp = (SampleSetInfo_t *)HLIB_STD_MemAlloc(sizeof(SampleSetInfo_t));
		HLIB_VECTOR_Add (pstInfo->phvcSampeSet, (void *) pstTemp);
		sprintf(pstTemp->szName, "dddd(%d)", HLIB_VECTOR_Length(pstInfo->phvcSampeSet));
	
		
		pstParam = (ParameterInfo_t *)HLIB_STD_MemAlloc(sizeof(ParameterInfo_t));

		HLIB_VECTOR_Add (pstTemp->phvcParamInfo, (void *) pstParam);
		sprintf(pstParam->Reference, "xxxx(%d)==(%d)", 
			HLIB_VECTOR_Length(pstInfo->phvcSampeSet),
			HLIB_VECTOR_Length(pstTemp->phvcParamInfo));


		pstParam = (ParameterInfo_t *)HLIB_STD_MemAlloc(sizeof(ParameterInfo_t));
		sprintf(pstParam->Reference, "xxxx(%d)==(%d)", 
			HLIB_VECTOR_Length(pstInfo->phvcSampeSet),
			HLIB_VECTOR_Length(pstTemp->phvcParamInfo));
		HLIB_VECTOR_Add (pstTemp->phvcParamInfo, (void *) pstParam);


		pstTemp = (SampleSetInfo_t *)HLIB_STD_MemAlloc(sizeof(SampleSetInfo_t));
		sprintf(pstTemp->szName, "dddd(%d)", HLIB_VECTOR_Length(pstInfo->phvcSampeSet));
		HLIB_VECTOR_Add (pstInfo->phvcSampeSet, (void *) pstTemp);


		pstParam = (ParameterInfo_t *)HLIB_STD_MemAlloc(sizeof(ParameterInfo_t));
		sprintf(pstParam->Reference, "xxxx(%d)==(%d)", 
			HLIB_VECTOR_Length(pstInfo->phvcSampeSet),
			HLIB_VECTOR_Length(pstTemp->phvcParamInfo));
		HLIB_VECTOR_Add (pstTemp->phvcParamInfo, (void *) pstParam);


		pstTemp = (SampleSetInfo_t *)HLIB_STD_MemAlloc(sizeof(SampleSetInfo_t));
		sprintf(pstTemp->szName, "dddd(%d)", HLIB_VECTOR_Length(pstInfo->phvcSampeSet));
		HLIB_VECTOR_Add (pstInfo->phvcSampeSet, (void *) pstTemp);

	}

	///
	REM_FUNC_LEAVE;

	return 0;
}

static HINT32 rema_MOD_Internal_Statisics_Deinit(void)
{
	REM_FUNC_ENTER;

	if(g_stStatisics.pstStatisticsInfo)
	{
		if(g_stStatisics.pstStatisticsInfo->phvcSampeSet)
		{
			HLIB_VECTOR_Delete(g_stStatisics.pstStatisticsInfo->phvcSampeSet);
			g_stStatisics.pstStatisticsInfo->phvcSampeSet = NULL;
		}
		//
		//if(g_stStatisics.pstStatisticsInfo->pstSampeSet)
		//	HLIB_STD_MemFree(g_stStatisics.pstStatisticsInfo->pstSampeSet);
		HLIB_STD_MemFree(g_stStatisics.pstStatisticsInfo);
		g_stStatisics.pstStatisticsInfo = NULL;
	}

	REM_FUNC_LEAVE;

	return 0;
}

static HINT32 rema_MOD_Internal_Statisics_checkCTX(void *pvCtx)
{
	REM_FUNC_ENTER;
	if(!pvCtx)
		return -1;

	if(!g_stStatisics.pstStatisticsInfo)
		return -1;

	if( pvCtx != g_stStatisics.pstStatisticsInfo)
		return -1;

	REM_FUNC_LEAVE;
	return 0;
}
#define ___PUBLIC_FUNCTIONS___

HERROR rema_MOD_Internal_Init(void)
{
	REM_FUNC_ENTER;

	g_stNsLookup.stMOD.bIsInit = false;
	g_stNsLookup.stMOD.bIsUpdate = false;
	g_stNsLookup.stMOD.pcbMod_Init = rema_MOD_Internal_NSLookup_Init;
	g_stNsLookup.stMOD.pcbMod_Deinit = rema_MOD_Internal_NSLookup_Deinit;
	g_stNsLookup.stMOD.pcbMod_Update = NULL;

	g_stStatisics.stMOD.bIsInit = false;
	g_stStatisics.stMOD.bIsUpdate = false;
	g_stStatisics.stMOD.pcbMod_Init = rema_MOD_Internal_Statisics_Init;
	g_stStatisics.stMOD.pcbMod_Deinit = rema_MOD_Internal_Statisics_Deinit;
	g_stStatisics.stMOD.pcbMod_Update = NULL;


	rema_MOD_QInit(g_stNsLookup);
	rema_MOD_QInit(g_stStatisics);


	REM_FUNC_LEAVE;

	return ERR_OK;
}


void rema_MOD_Internal_Deinit(void)
{
	REM_FUNC_ENTER;

	rema_MOD_QDeinit(g_stNsLookup);
	rema_MOD_QDeinit(g_stStatisics);
	REM_FUNC_LEAVE;
	return;
}
HINT32 rema_MOD_Internal_resetUpdate(void)
{
	rema_MOD_QResetUpdate(g_stNsLookup);
	rema_MOD_QResetUpdate(g_stStatisics);
	return 0 ; 
}

#define ___EXTERNAL_FUNCTIONS___
void * rema_MOD_Internal_GetCTXNSLookupInfo(void)
{
	return (void *)g_stNsLookup.pstDiagNSLook;
}

HERROR rema_MOD_Internal_GetStrNSLookupInfo(void *pvCtx, const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen)
{
	DiagNSLookupInfo_t *pstInfo = NULL;

	REM_FUNC_ENTER;

	if( rema_MOD_Internal_NSLookup_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	if(!pszName || !pBuf || nBufLen <= 0 )
	{
		RemLOG_Debug("pszName(%s),pBuf(%s) nBufLen(%d)\n", 
			REM_STR_CHECKNULL(pszName),
			REM_STR_CHECKNULL(pBuf),
			nBufLen);
		return ERR_FAIL;
	}

	pstInfo = (DiagNSLookupInfo_t *)pvCtx;

	RemLOG_Debug("pszName(%s)\n", pszName);
	if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_DiagnosticsState))
	{
		NSLookupDiag_GetStateName(pstInfo->eDiagState, pBuf, nBufLen);
		//RemLOG_Debug("pBuf(%s)\n", pBuf);
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_Interface))
	{
		HxSTD_PrintToStrN(pBuf, nBufLen, pstInfo->szInterface);
		RemLOG_Debug("pBuf(%s)\n", pBuf);
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_HostName))
	{
		HxSTD_PrintToStrN(pBuf, nBufLen, pstInfo->szHostName);
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_DNSServer))
	{
		HxSTD_PrintToStrN(pBuf, nBufLen, pstInfo->szDNSServer);
	}
	else
	{
		RemLOG_Debug("erro pszName(%s)\n", pszName);
		return ERR_FAIL;
	}
	

	REM_FUNC_LEAVE;

	return ERR_OK; 
}
HERROR rema_MOD_Internal_SetStrNSLookupInfo(void *pvCtx, const HCHAR *pszName, const HCHAR *pBuf)
{
	DiagNSLookupInfo_t *pstInfo = NULL;

	REM_FUNC_ENTER;

	if( rema_MOD_Internal_NSLookup_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	if(!pszName)
		return ERR_FAIL;

	pstInfo = (DiagNSLookupInfo_t *)pvCtx;

	//RemLOG_Debug("pszName(%s)\n", pszName);
	if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_DiagnosticsState))
	{
		NSLookupDiag_SetStateName(pBuf, &pstInfo->eDiagState);
		RemLOG_Debug("pBuf(%s) %d\n", pBuf, pstInfo->eDiagState);
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_Interface))
	{
		HxSTD_PrintToStrN(pstInfo->szInterface, 257, pBuf);
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_HostName))
	{
		HxSTD_PrintToStrN(pstInfo->szHostName, 257, pBuf);
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_DNSServer))
	{
		HxSTD_PrintToStrN(pstInfo->szDNSServer, 257, pBuf);
	}
	else
	{	
		RemLOG_Debug("erro pszName(%s)\n", pszName);
		return ERR_FAIL;
	}
	REM_FUNC_LEAVE;

	return ERR_OK; 
}
HERROR rema_MOD_Internal_GetUINTNSLookupInfo(void *pvCtx, const HCHAR *pszName, HUINT32 *punData)
{
	DiagNSLookupInfo_t *pstInfo = NULL;
	if( rema_MOD_Internal_NSLookup_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	if(!pszName)
		return ERR_FAIL;

	pstInfo = (DiagNSLookupInfo_t *)pvCtx;
	
	if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_Timeout))
	{
		*punData = pstInfo->unTimeout;
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_NumberOfRepetitions))
	{
		*punData = pstInfo->unNumberOfRepetitions;
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_SuccessCount))
	{
		*punData = pstInfo->unSuccessCount;
	}
	else
	{
		return ERR_FAIL;
	}


	return ERR_OK; 
}
HERROR rema_MOD_Internal_SetUINTNSLookupInfo(void *pvCtx, const HCHAR *pszName, const HUINT32 unData)
{
	DiagNSLookupInfo_t *pstInfo = NULL;
	if( rema_MOD_Internal_NSLookup_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	if(!pszName)
		return ERR_FAIL;

	pstInfo = (DiagNSLookupInfo_t *)pvCtx;

	if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_Timeout))
	{
		 pstInfo->unTimeout = unData;
	}
	else if(!HxSTD_StrCmp(pszName, DSTR_NSLOOKUP_NumberOfRepetitions))
	{
		pstInfo->unNumberOfRepetitions = unData;
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK; 
}

void *rema_MOD_Internal_GetCTXStatisicsInfo(void)
{
	return (void *)g_stStatisics.pstStatisticsInfo;
}


HERROR rema_MOD_Internal_GetUINTStatisic_MinSampleInterval(void *pvCtx, HUINT32 *punData)
{
	PeriodicStatisticsInfo_t *pstInfo = NULL;
	if( rema_MOD_Internal_Statisics_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	
	pstInfo = (PeriodicStatisticsInfo_t *)pvCtx;
	*punData = pstInfo->unMinSampleInterval;
	
	return ERR_OK;
}

HERROR rema_MOD_Internal_GetUINTStatisic_MaxReportSamples(void *pvCtx, HUINT32 *punData)
{
	PeriodicStatisticsInfo_t *pstInfo = NULL;
	if( rema_MOD_Internal_Statisics_checkCTX(pvCtx) != 0)
		return ERR_FAIL;

	pstInfo = (PeriodicStatisticsInfo_t *)pvCtx;
	*punData = pstInfo->unMaxReportSamples;
	
	return ERR_OK;
}
HERROR rema_MOD_Internal_GetStatisic_SampleSet_Max(void *pvCtx, HUINT32 *punMax)
{
	int unTotal = 0 ; 
	PeriodicStatisticsInfo_t *pstInfo = NULL;
	if( rema_MOD_Internal_Statisics_checkCTX(pvCtx) != 0)
		return ERR_FAIL;

	pstInfo = (PeriodicStatisticsInfo_t *)pvCtx;
	if(!pstInfo->phvcSampeSet)
	{
		unTotal = 0;
	}
	else 
	{
		unTotal = HLIB_VECTOR_Length(pstInfo->phvcSampeSet);
	}

	*punMax = unTotal;

	return ERR_OK;
}
HERROR rema_MOD_Internal_AddStatisic_SampleSet(void *pvCtx, HUINT32 *punAddedNum)
{
	PeriodicStatisticsInfo_t *pstInfo = NULL;
	SampleSetInfo_t *pstTemp = NULL;
	if( rema_MOD_Internal_Statisics_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	pstInfo = (PeriodicStatisticsInfo_t *)pvCtx;
	
	
	if(pstInfo->phvcSampeSet)
	{
		pstTemp = (SampleSetInfo_t *)HLIB_STD_MemAlloc(sizeof(SampleSetInfo_t));
		sprintf(pstTemp->szName, "dddd(%d)", HLIB_VECTOR_Length(pstInfo->phvcSampeSet));
		HLIB_VECTOR_Add (pstInfo->phvcSampeSet, (void *) pstTemp);

		*punAddedNum = HLIB_VECTOR_Length(pstInfo->phvcSampeSet)-1;
	}

	return ERR_OK;
}
HERROR rema_MOD_Internal_DelStatisic_SampleSet(void *pvCtx, HUINT32 unDelNum)
{
	PeriodicStatisticsInfo_t *pstInfo = NULL;
	//SampleSetInfo_t *pstTemp = NULL;
	if( rema_MOD_Internal_Statisics_checkCTX(pvCtx) != 0)
		return ERR_FAIL;
	pstInfo = (PeriodicStatisticsInfo_t *)pvCtx;
	
	if(HLIB_VECTOR_Length(pstInfo->phvcSampeSet) <= unDelNum)
		return ERR_FAIL;

	if(pstInfo->phvcSampeSet )
	{
		HLIB_VECTOR_Remove(pstInfo->phvcSampeSet, unDelNum);
	}
	return ERR_OK; 
}

void *rema_MOD_Internal_GetCTXAddStatisic_SampleSet(void *pvCtx, HUINT32 unNum)
{
	SampleSetInfo_t *pstSampleSet=NULL;
	PeriodicStatisticsInfo_t *pstInfo = NULL;
	//SampleSetInfo_t *pstTemp = NULL;
	if( rema_MOD_Internal_Statisics_checkCTX(pvCtx) != 0)
		return pstSampleSet;
	pstInfo = (PeriodicStatisticsInfo_t *)pvCtx;

	pstSampleSet = HLIB_VECTOR_ItemAt(pstInfo->phvcSampeSet, unNum);
	return pstSampleSet;
}
HERROR rema_MOD_Internal_GetStatisic_SampleSet_Name(void *pvCtx, HCHAR *pBuf, HINT32 nBufLen)
{
	
	if(!pBuf)
		return ERR_FAIL;

	//RemLOG_Debug("erro unIndex(%d)\n", unIndex);
	//if(rema_MOD_QUpdate(g_stStatisics))
	{
		SampleSetInfo_t *pstSampleSet=(SampleSetInfo_t *)pvCtx;
		//pstSampleSet = HLIB_VECTOR_ItemAt(g_stStatisics.pstStatisticsInfo->phvcSampeSet, unIndex);
		if(pstSampleSet)
		{
			RemLOG_Debug("erro unIndex(%s)\n", pstSampleSet->szName);
			HxSTD_StrCpy(pBuf, pstSampleSet->szName);
		}
	}
	return ERR_OK;
}

HERROR rema_MOD_Internal_GetStatisic_SampleSet_Parameter_Max(void *pvCtx, HUINT32 *punMax)
{
	int unTotal = 0 ; 
	SampleSetInfo_t *pstInfo = NULL;


	pstInfo = (SampleSetInfo_t *)pvCtx;
	if(!pstInfo->phvcParamInfo)
	{
		unTotal = 0;
	}
	else 
	{
		unTotal = HLIB_VECTOR_Length(pstInfo->phvcParamInfo);
	}

	*punMax = unTotal;

	return ERR_OK;
}
#define _____END_OF_FILE_________

