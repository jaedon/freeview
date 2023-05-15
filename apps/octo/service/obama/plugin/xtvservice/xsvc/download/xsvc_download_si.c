/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <stdio.h>
#include <string.h>
#include <octo_common.h>

#include <pal_sef.h>

#include <xsvc_download.h>

/** @brief local header file  */
#include <_svc_download.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
*/


#define	SvcDownload_SI_DEFAULT_TIMEOUT			10000

STATIC HERROR xsvc_download_DmxGetDefaultTimeout(SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	switch(eTableType)
	{
		case eSI_TableType_PAT:			*pulTimeout = 10000;							break;
		case eSI_TableType_CAT:			*pulTimeout = 10000;							break;
		case eSI_TableType_PMT:			*pulTimeout = 10000;							break;
		case eSI_TableType_NIT_ACT:		*pulTimeout = 25000;							break; /* SAT model의 경우 motor의 최대 동작 시간으로 정의 한다. */
		case eSI_TableType_NIT_OTH:		*pulTimeout = 25000;							break;
		case eSI_TableType_SDT_ACT:		*pulTimeout = 35000;							break;
		case eSI_TableType_SDT_OTH:		*pulTimeout = 35000;							break;
		case eSI_TableType_BAT:			*pulTimeout = 15000;							break; //20000	//mininam reducted waiting time(it seemed too long)
		case eSI_TableType_TDT:			*pulTimeout = 30000;							break;
		case eSI_TableType_TOT:			*pulTimeout = 30000;							break;
		case eSI_TableType_DSI:			*pulTimeout = 100*1000;							break;
		case eSI_TableType_DDB:			*pulTimeout = 20*1000;							break;
		case eSI_TableType_EIT_ACTPF:	*pulTimeout = 5000;								break; // 5 sec
		case eSI_TableType_EIT_OTHPF:	*pulTimeout = 30000;							break; // 30 sec
		case eSI_TableType_EIT_ACTSC:	*pulTimeout = 900000;							break; // 15 min
		case eSI_TableType_EIT_OTHSC:	*pulTimeout = 3600000;							break; // 60 min
		case eSI_TableType_UNT:			*pulTimeout = 25000;							break;
		case eSI_TableType_CIT:			*pulTimeout = 30000;							break;
		case eSI_TableType_CPT:			*pulTimeout = 20000;							break;
		case eSI_TableType_SGT:			*pulTimeout = 15000;							break;
		case eSI_TableType_AIT:			*pulTimeout = 2000;								break;
		case eSI_TableType_PMT_N:		*pulTimeout = 5000;								break; // (pmt timeout / 2)
		case eSI_TableType_ICT:			*pulTimeout = 30000;							break; //50000  //mininam reducted waiting time(it seemed too long)
		case eSI_TableType_DIT:			*pulTimeout = 25000;							break;
		case eSI_TableType_BIT:			*pulTimeout = 11000;							break;
		case eSI_TableType_SIT:			*pulTimeout = 30000;							break;
		default:						*pulTimeout = SvcDownload_SI_DEFAULT_TIMEOUT;	break;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_download_DmxGetTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	HERROR	hErr;

	hErr = svc_download_SiGetTableTimeOut(hAction, eTableType, pulTimeout);
	if (ERR_OK != hErr)
	{
		hErr = xsvc_download_DmxGetDefaultTimeout(eTableType, pulTimeout);
	}

	return hErr;
}

HERROR xsvc_download_DmxStopOneSection(HUINT16 usFilterId)
{
	PAL_SEF_StopSecFilter(usFilterId);
	return ERR_DOWN_RESULT_OK;
}

HERROR xsvc_download_DmxRetrieveDsiSection(SiRetrieveParam_t *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	hErr = xsvc_download_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DSI, &ulTimeout);
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
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_DOWN_RESULT_OK;
}

HERROR xsvc_download_DmxRetrieveDdbSection(SiRetrieveParam_t *pstTable,
											void *pfnParse,
											void *pfnFree,
											HUINT16 *pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_DOWN_SOURCE_NULL; }
	if (pusFilterId == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	hErr = xsvc_download_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DDB, &ulTimeout);
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
										pstTable->ucTimeoutMode,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_DOWN_RESULT_OK;
}

HERROR xsvc_download_DmxRetrieveDescSection(SiRetrieveParam_t *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId)
{
	HERROR					hErr;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	hErr = xsvc_download_DmxGetTimeout(HANDLE_NULL, eSI_TableType_DSI, &ulTimeout);
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
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}

/*********************** End of File ******************************/
