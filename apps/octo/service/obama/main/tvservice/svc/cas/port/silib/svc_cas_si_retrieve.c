/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>
#include <pal_sef.h>
#include <pal_pipe.h>

/** @brief local header file  */
#include "../local_include/_svc_cas_si_api.h"
#include <_svc_cas_sub_svc_api.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
// default table timeout
#define SI_DEFAULT_TIMEOUT			10000
STATIC HERROR svc_cas_DmxGetDefaultTimeout(SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	switch(eTableType)
	{
		case eSI_TableType_PAT:			*pulTimeout = 10000;				break;
		case eSI_TableType_CAT:			*pulTimeout = 10000;				break;
		case eSI_TableType_PMT:			*pulTimeout = 10000;				break;
		case eSI_TableType_NIT_ACT:		*pulTimeout = 25000;				break; /* SAT model의 경우 motor의 최대 동작 시간으로 정의 한다. */
		case eSI_TableType_NIT_OTH:		*pulTimeout = 25000;				break;
		case eSI_TableType_SDT_ACT:		*pulTimeout = 35000;				break;
		case eSI_TableType_SDT_OTH:		*pulTimeout = 35000;				break;
		case eSI_TableType_BAT:			*pulTimeout = 15000;				break; //20000	//mininam reducted waiting time(it seemed too long)
		case eSI_TableType_TDT:			*pulTimeout = 30000;				break;
		case eSI_TableType_TOT:			*pulTimeout = 30000;				break;
		case eSI_TableType_DSI:			*pulTimeout = 100*1000;				break;
		case eSI_TableType_DDB:			*pulTimeout = 20*1000;				break;
		case eSI_TableType_EIT_ACTPF:	*pulTimeout = 5000;					break; // 5 sec
		case eSI_TableType_EIT_OTHPF:	*pulTimeout = 30000;				break; // 30 sec
		case eSI_TableType_EIT_ACTSC:	*pulTimeout = 900000;				break; // 15 min
		case eSI_TableType_EIT_OTHSC:	*pulTimeout = 3600000;				break; // 60 min
		case eSI_TableType_UNT:			*pulTimeout = 25000;				break;
		case eSI_TableType_CIT:			*pulTimeout = 30000;				break;
		case eSI_TableType_CPT:			*pulTimeout = 20000;				break;
		case eSI_TableType_SGT:			*pulTimeout = 15000;				break;
		case eSI_TableType_AIT:			*pulTimeout = 2000;					break;
		case eSI_TableType_PMT_N:		*pulTimeout = 5000;					break; // (pmt timeout / 2)
		case eSI_TableType_ICT:			*pulTimeout = 30000;				break; //50000  //mininam reducted waiting time(it seemed too long)
		case eSI_TableType_DIT:			*pulTimeout = 25000;				break;
		case eSI_TableType_BIT:			*pulTimeout = 11000;				break;
		case eSI_TableType_SIT:			*pulTimeout = 30000;				break;
		default:						*pulTimeout = SI_DEFAULT_TIMEOUT;	break;
	}

	return ERR_OK;
}

HERROR svc_cas_DmxGetTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	HERROR	hErr = ERR_FAIL;

#if !defined(CONFIG_PROD_OS_EMULATOR)
	hErr = svc_cas_SiGetTableTimeOut(hAction, eTableType, pulTimeout);
	if (ERR_OK != hErr)
	{
		hErr = svc_cas_DmxGetDefaultTimeout(eTableType, pulTimeout);
	}
#endif

	return hErr;
}

HERROR svc_cas_DmxSetDefaultRetrieveParam(svcCas_SiRetrieveParam_t	*pstRetrieveParam,
										HUINT32						 hAction,
										HUINT16						 usTsUniqId,
										HUINT32						 ulDemuxId)
{

	if (pstRetrieveParam == NULL)	{ return ERR_CAS_SI_TARGET_NULL; }

	HxSTD_memset(pstRetrieveParam, 0, sizeof(svcCas_SiRetrieveParam_t));

	pstRetrieveParam->hAction		= hAction;
	pstRetrieveParam->usTsUniqId 	= usTsUniqId;
	pstRetrieveParam->ucDemuxId		= (HUINT8)ulDemuxId;
	pstRetrieveParam->usPid			= PID_NULL;
	pstRetrieveParam->ucMode 		= eSiFilterMode_Once;
	pstRetrieveParam->ucHead		= ACT_HEAD;
	pstRetrieveParam->bTableFilter	= TRUE;
	pstRetrieveParam->bUseExtId		= FALSE;
	pstRetrieveParam->bUseVerNum	= FALSE;
	pstRetrieveParam->bCrc			= TRUE; /* crc 깨진 section에 대한 처리가 안되어 있는 경우도 있으므로, default가 TRUE인 것이 맞음 */
	pstRetrieveParam->usExtId 		= 0;
	pstRetrieveParam->ucVerNum		= 0;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxStopOneSection(HUINT16 usFilterId)
{
	PAL_SEF_StopSecFilter(usFilterId);
	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveSectionBase(svcCas_SiRetrieveParam_t	*pstTable,
									HUINT8						 ucTableId,
									HUINT8						*pucFilterValue,
									HUINT8						*pucFilterMask,
									HUINT32						 ulFilterSize,
									HUINT32						 ulFilterTimeout,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HUINT16					usFilterId;
	HUINT32					ulIndex;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));

	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulFilterTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	// Filter를 제대로 걸었을 경우에는 Filter를 쓴다.
	ulFilterSize = (ulFilterSize >= PAL_SEF_MAX_FILTER_LEN) ? PAL_SEF_MAX_FILTER_LEN - 1 : ulFilterSize;
	if (pucFilterValue != NULL && pucFilterMask != NULL && ulFilterSize != 0)
	{
		for (ulIndex = 0; ulIndex < ulFilterSize; ulIndex++)
		{
			stFilterParam.pFilterValue[ulIndex]	= pucFilterValue[ulIndex];
			stFilterParam.pFilterMask[ulIndex]	= pucFilterMask[ulIndex];
		}
	}

	stFilterParam.eFilterLen = (PalSef_FilterLength_e)ulFilterSize;
	// continue mode에서 timeout 처리를 위한 모드 설정
	stFilterParam.eTimeoutMode = (PalSef_TimeOutMode_e)pstTable->ucTimeoutMode;

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrievePatSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_PAT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										PAT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										PAT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxRetrieveCatSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_CAT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										CAT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										CAT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxRetrievePmtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_PMT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										PMT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxRetrieveNitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT8					ucTableId;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, (pstTable->ucHead == ACT_HEAD) ? eSI_TableType_NIT_ACT : eSI_TableType_NIT_OTH, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	ucTableId = (pstTable->ucHead == ACT_HEAD) ? NIT_ACTUAL_TABLE_ID : NIT_OTHER_TABLE_ID;

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										NIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveSdtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16		 		 		*pusFilterId)
{
	HERROR				hErr;
	HUINT32				ulTimeout;
	HUINT8				ucTableId;
	HUINT16				usFilterId;
	PalSef_FilterParam_t		stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, (pstTable->ucHead == ACT_HEAD) ? eSI_TableType_SDT_ACT : eSI_TableType_SDT_OTH, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	ucTableId = (pstTable->ucHead == ACT_HEAD) ? SDT_ACTUAL_TABLE_ID : SDT_OTHER_TABLE_ID;

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										SDT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxRetrieveEitSchOtherSection(svcCas_SiRetrieveParam_t	*pstTable,
											HUINT8						 ucTableId,
											SvcCas_SiParseCb_t			 fnParse,
											SvcCas_SiFreeCb_t			 fnFree,
											HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_EIT_OTHSC, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										EIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);
	/* eit sch에서만 이런 처리를 한다 */
	if(eSiFilterMode_Continue == pstTable->ucMode)
	{
		stFilterParam.nSecCntInPackage = pstTable->nSecCntInPackage;
		stFilterParam.nDurationInPackage = pstTable->nDurationInPackage;
	}


	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxRetrieveEitSchSection(svcCas_SiRetrieveParam_t	*pstTable,
										SvcCas_SiParseCb_t			 fnParse,
										SvcCas_SiFreeCb_t			 fnFree,
										HUINT16		 		 		*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT8					ucTableId;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, (pstTable->ucHead == ACT_HEAD) ? eSI_TableType_EIT_ACTSC : eSI_TableType_EIT_OTHSC, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	ucTableId = (pstTable->ucHead == ACT_HEAD) ? EIT_ACTUAL_SCHEDULE_TABLE_ID_0 : EIT_OTHER_SCHEDULE_TABLE_ID_0;

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										EIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);
	/* eit sch에서만 이런 처리를 한다 */
	if(eSiFilterMode_Continue == pstTable->ucMode)
	{
		stFilterParam.nSecCntInPackage = pstTable->nSecCntInPackage;
		stFilterParam.nDurationInPackage = pstTable->nDurationInPackage;
	}

	// EIT_XXXX_SCHEDULE_TABLE_ID_0 ~ EIT_XXXX_SCHEDULE_TABLE_ID_15
	stFilterParam.pFilterMask[0] = 0xf0;
	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}



HERROR svc_cas_DmxMonitorEitSchSection(svcCas_SiRetrieveParam_t		*pstTable,
									SvcCas_SiParseCb_t				 fnParse,
									SvcCas_SiFreeCb_t				 fnFree,
									HUINT16							*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT8					ucTableId;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, (pstTable->ucHead == ACT_HEAD) ? eSI_TableType_EIT_ACTSC : eSI_TableType_EIT_OTHSC, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	ucTableId = (pstTable->ucHead == ACT_HEAD) ? EIT_ACTUAL_SCHEDULE_TABLE_ID_0 : EIT_OTHER_SCHEDULE_TABLE_ID_0;

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										EIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);


	/* eit sch에서만 이런 처리를 한다 */
	if(eSiFilterMode_Continue == pstTable->ucMode)
	{
		stFilterParam.nSecCntInPackage = pstTable->nSecCntInPackage;
		stFilterParam.nDurationInPackage = pstTable->nDurationInPackage;
	}

	// EIT_XXXX_SCHEDULE_TABLE_ID_0 ~ EIT_XXXX_SCHEDULE_TABLE_ID_15
	stFilterParam.pFilterMask[0] = 0xf0;

	// only section number 0
	stFilterParam.eFilterLen = eSefFilterLen_8Bytes;
	stFilterParam.pFilterValue[4] = 0;
	stFilterParam.pFilterMask[4] = 0xff;

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxMonitorEitPfSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT8					ucTableId;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, (pstTable->ucHead == ACT_HEAD) ? eSI_TableType_EIT_ACTPF : eSI_TableType_EIT_OTHPF, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	ucTableId = (pstTable->ucHead == ACT_HEAD) ? EIT_ACTUAL_TABLE_ID : EIT_OTHER_TABLE_ID;

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										EIT_PID,
										FALSE, 	/* pstTable->bTableFilter */
										TRUE,	/* pstTable->bUseExtId */
										TRUE,	/* pstTable->bUseVerNum */
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										eSiFilterMode_Once,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)NULL);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}




HERROR svc_cas_DmxRetrieveEitPfSection(svcCas_SiRetrieveParam_t		*pstTable,
									SvcCas_SiParseCb_t				 fnParse,
									SvcCas_SiFreeCb_t				 fnFree,
									HUINT16							*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT8					ucTableId;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, (pstTable->ucHead == ACT_HEAD) ? eSI_TableType_EIT_ACTPF : eSI_TableType_EIT_OTHPF, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	ucTableId = (pstTable->ucHead == ACT_HEAD) ? EIT_ACTUAL_TABLE_ID : EIT_OTHER_TABLE_ID;

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										EIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveTdtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_TDT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										TDT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										TDT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveBatSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_BAT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										BAT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										BAT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveDescSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DSI, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										DESC_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveDsiSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DSI, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										DSI_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}



HERROR svc_cas_DmxRetrieveDdbSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DDB, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");


	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										DDB_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveCitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_CIT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										CIT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}



HERROR svc_cas_DmxRetrieveCptSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_CPT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										CPT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveEcmSection(svcCas_SiRetrieveParam_t	*pstTable,
									HUINT8						 ucTableId,
									HUINT8						*pucFilterValue,
									HUINT8						*pucFilterMask,
									HUINT32						 ulFilterSize,
									HUINT32						 ulFilterTimeout,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	return svc_cas_DmxRetrieveSectionBase(pstTable, ucTableId, pucFilterValue, pucFilterMask, ulFilterSize, ulFilterTimeout, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_DmxRetrieveEmmSection (svcCas_SiRetrieveParam_t	*pstTable,
									HUINT8						 ucTableId,
									HUINT8						*pucFilterValue,
									HUINT8						*pucFilterMask,
									HUINT32						 ulFilterSize,
									HUINT32						 ulFilterTimeout,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	return svc_cas_DmxRetrieveSectionBase(pstTable, ucTableId, pucFilterValue, pucFilterMask, ulFilterSize, ulFilterTimeout, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_DmxRetrieveRctSection (svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HUINT8					ucTableId;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));

	ucTableId = RCT_TABLE_ID;

	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										ucTableId,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										RCT_TIMEOUT_VALUE,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveBitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_BIT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										BIT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										BIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveSitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_SIT, &ulTimeout);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										SIT_ARIB_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										SIT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveLowProtectionSdttSection(svcCas_SiRetrieveParam_t	*pstTable,
												SvcCas_SiParseCb_t			 fnParse,
												SvcCas_SiFreeCb_t			 fnFree,
												HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_LowProtectionSDTT, &ulTimeout);
	if(hErr != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	ulTimeout = 0;
	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										SDTT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										LOW_PROTECTION_SDTT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}


HERROR svc_cas_DmxRetrieveHighProtectionSdttSection(svcCas_SiRetrieveParam_t	*pstTable,
													SvcCas_SiParseCb_t			 fnParse,
													SvcCas_SiFreeCb_t			 fnFree,
													HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_HighProtectionSDTT, &ulTimeout);
	if(hErr != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	ulTimeout = 0;
	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										SDTT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										HIGH_PROTECTION_SDTT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveCdtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_CDT, &ulTimeout);
	if(hErr != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	ulTimeout = 0;
	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										CDT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										CDT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_DmxRetrieveDiiSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DII, &ulTimeout);
	if(hErr != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	/* 일본향에서는 timeout 값 사용하지 않음, 운용사양에 따라 timeout 값이 틀려지기 때문 */
	ulTimeout = 0;
	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										DII_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)fnParse,
										(PalSef_Callback_t)fnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_CAS_SI_RESULT_OK;
}

/* end of file */
