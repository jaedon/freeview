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

#ifndef	__EPG_EITLIB_H__
#define	__EPG_EITLIB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_svc_epg_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define MAX_RAWDESCRIPTOR_LEN	256

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	EIT_TYPE_ERR,
	EIT_TYPE_NEW,
	EIT_TYPE_OLD
}SvcEpg_EitType_e;


typedef enum
{
	eNeed_EventInfo,
	eNeed_ExtendInfo,

	eNeed_Max
}SvcEpg_EitNeed_e;


typedef enum
{
	EIT_TABLE_ID_OFFSET								= 0,
	EIT_SECTION_SYSTAX_INDICATOR_OFFSET			= 1,
	EIT_SECTION_LENGTH_OFFSET						= 1,
	EIT_SERVICE_ID_OFFSET								= 3,
	EIT_VERSION_NUMBER_OFFSET						= 5,
	EIT_CURRENT_NEXT_INDICATOR_OFFSET				= 5,
	EIT_SECTION_NUMBER_OFFSET						= 6,
	EIT_LAST_SECTION_NUMBER_OFFSET					= 7,
	EIT_TRANSPORT_STREAM_ID_OFFSET					= 8,
	EIT_ORIGINAL_NETWORK_ID_OFFSET					= 10,
	EIT_OLD_LAST_TABLE_ID_OFFSET						= 12,
	EIT_OLD_EVENT_AREA_OFFSET						= 13,

	EIT_SEGMENT_LAST_SECTION_NUMBER_OFFSET		= 12,
	EIT_NEW_LAST_TABLE_ID_OFFSET					= 13,
	EIT_NEW_EVENT_AREA_OFFSET						= 14,



	EIT_MAX_OFFSET
}SvcEpg_EitMapSection_e;


typedef enum
{
	EIT_EVENT_ID_OFFSET								= 0,
	EIT_EVENT_START_TIME_OFFSET						= 2,
	EIT_EVENT_DURATION_OFFSET						= 7,
	EIT_EVENT_RUNNING_STATUS_OFFSET				= 10,
	EIT_EVENT_FREE_CA_MODE_OFFSET					= 10,
	EIT_EVENT_DESCRIPTORS_LOOP_LENGTH_OFFSET		= 10,
	EIT_EVENT_DESCRIPTORS_LOOP_OFFSET				= 12,

	EIT_EVENT_DESCRIPTOR_TAG_OFFSET					= 0,
	EIT_EVENT_DESCRIPTOR_LENGTH_OFFSET				= 1,
	EIT_EVENT_DESCRIPTOR_AREA_OFFSET				= 2,

	EIT_EVENT_MAX_OFFSET
}SvcEpg_EitEventMapSection_e;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR				SVC_EPGEIT_GetSectionInfo(EPG_SpecType_e eSpec, HUINT8 *pucRawSection, svcEpg_ServiceInfo_t *pstSectionInfo);

#define ____RAW_EPG_MGR_FUNC____

HUINT32		SVC_EPGEIT_GetPriorityByCountryIso3166(EPG_SpecType_e eSpec, SvcEpg_FindParam_t *pParam, HUINT8 *pIso3166);
HUINT32		SVC_EPGEIT_GetPriorityByLangIso639(EPG_SpecType_e eSpec, SvcEpg_FindParam_t *pParam, HUINT8 *pIso639);





#endif /* !__EPG_EITLIB_H__ */
