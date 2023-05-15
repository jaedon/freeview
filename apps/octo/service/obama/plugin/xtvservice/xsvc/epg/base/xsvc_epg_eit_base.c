/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "util.h"
#include "linkedlist.h"
#include "db_svc.h"
#include <db_param.h>
#include "isosvc.h"
#include <svc_epg.h>
#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_eit_lib.h"

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
#define ____GLOBAL_FUNC____

HERROR				xsvc_epgeit_GetSectionInfo_Base(HUINT8 *pucRawSection, svcEpg_ServiceInfo_t *pstSectionInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT8				ucTemp = 0;
	HINT32				lCrcOffset = 0;
	HINT32				lSectionBasicLength = 0;
	HINT32				lSecLen = 0;

	if(pucRawSection == NULL || pstSectionInfo == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_EITLIB_FUNC;
	}

	lSectionBasicLength = EIT_NEW_EVENT_AREA_OFFSET + SECTION_BYTES_CRC - SECTION_BYTES_SKIP;
	lSecLen = (HINT32)HxMACRO_Get12Bit(&(pucRawSection[EIT_SECTION_LENGTH_OFFSET]));
	if (lSecLen > MAX_SECTION_LIMIT || lSecLen < lSectionBasicLength)
	{
		HxLOG_Error("invalid section length  fail!!\n");
		goto EXIT_EITLIB_FUNC;
	}

	// table
	pstSectionInfo->ucTableId = pucRawSection[EIT_TABLE_ID_OFFSET];
	HxLOG_Info("usTableId(%02x)\n", pstSectionInfo->ucTableId);

	// service id
	pstSectionInfo->usSvcId = (HUINT16)HxMACRO_Get16Bit(&(pucRawSection[EIT_SERVICE_ID_OFFSET]));
	HxLOG_Info("usSvcId(%04x)\n", pstSectionInfo->usSvcId);

	// transport stream id
	pstSectionInfo->usTsId = (HUINT16)HxMACRO_Get16Bit(&(pucRawSection[EIT_TRANSPORT_STREAM_ID_OFFSET]));
	HxLOG_Info("usTsId(%04x)\n", pstSectionInfo->usTsId);

	// original network id (2byte)
	pstSectionInfo->usOnId = (HUINT16)HxMACRO_Get16Bit(&(pucRawSection[EIT_ORIGINAL_NETWORK_ID_OFFSET]));
	HxLOG_Info("usOrgNetId(%04x)\n", pstSectionInfo->usOnId);

	// version number (5bit)
	ucTemp = pucRawSection[EIT_VERSION_NUMBER_OFFSET];
	pstSectionInfo->ucVersion = 0x1f & ((ucTemp & 0x3e) >> 1);
	HxLOG_Info("ucVersion(%04x)\n", pstSectionInfo->ucVersion);

	// section number (8bit)
	pstSectionInfo->ucSectionNumber = pucRawSection[EIT_SECTION_NUMBER_OFFSET];
	HxLOG_Info("ucSectionNumber(%04x)\n", pstSectionInfo->ucSectionNumber);

	// last section number (8bit)
	pstSectionInfo->ucLastSectionNumber = pucRawSection[EIT_LAST_SECTION_NUMBER_OFFSET];
	HxLOG_Info("ucLastSectionNumber(%04x)\n", pstSectionInfo->ucLastSectionNumber);

	// segment last section number (8bit)
	pstSectionInfo->ucSegmentLastSectionNumber = pucRawSection[EIT_SEGMENT_LAST_SECTION_NUMBER_OFFSET];
	HxLOG_Info("ucSegmentLastSectionNumber(%04x)\n", pstSectionInfo->ucSegmentLastSectionNumber);

	pstSectionInfo->ucLastTableId = pucRawSection[EIT_NEW_LAST_TABLE_ID_OFFSET];

	// crc
	lCrcOffset = lSecLen + SECTION_BYTES_SKIP - SECTION_BYTES_CRC;
	pstSectionInfo->ulCrc = (HUINT32)HxMACRO_Get32Bit(&(pucRawSection[lCrcOffset]));
	HxLOG_Info("ulCrc(%04x)\n", pstSectionInfo->ulCrc);

	hError = ERR_EPG_OK;

EXIT_EITLIB_FUNC :

	return hError;

}



HERROR			xsvc_epgeit_GetSectionSimpleInfo_Base(	HUINT8 *pucRawSection,
															HUINT8 *pucTableId,
															HUINT8 *pucVersion,
															HUINT8 *pucSecNum,
															HUINT32 *pulCrc)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT8				ucTemp = 0;
	HINT32				lCrcOffset = 0;
	HINT32				lSectionBasicLength = 0;
	HINT32				lSecLen = 0;

	if(pucRawSection == NULL || pucVersion == NULL || pucSecNum == NULL || pulCrc == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_EITLIB_FUNC;
	}

	lSectionBasicLength = EIT_NEW_EVENT_AREA_OFFSET + SECTION_BYTES_CRC - SECTION_BYTES_SKIP;
	lSecLen = (HINT32)get12bit(&(pucRawSection[EIT_SECTION_LENGTH_OFFSET]));
	if (lSecLen > MAX_SECTION_LIMIT || lSecLen < lSectionBasicLength)
	{
		HxLOG_Error("invalid section length  fail!!\n");
		goto EXIT_EITLIB_FUNC;
	}

	*pucTableId = pucRawSection[EIT_TABLE_ID_OFFSET];
	HxLOG_Info("usTableId(%02x)\n", *pucTableId);

	// version number (5bit)
	ucTemp = pucRawSection[EIT_VERSION_NUMBER_OFFSET];
	*pucVersion = 0x1f & ((ucTemp & 0x3e) >> 1);
	HxLOG_Info("ucVersion(%04x)\n", *pucVersion);

	// section number (8bit)
	*pucSecNum = pucRawSection[EIT_SECTION_NUMBER_OFFSET];
	HxLOG_Info("ucSectionNumber(%04x)\n", *pucSecNum);

	// crc
	lCrcOffset = lSecLen + SECTION_BYTES_SKIP - SECTION_BYTES_CRC;
	*pulCrc = (HUINT32)get32bit(&(pucRawSection[lCrcOffset]));
	HxLOG_Info("ulCrc(%04x)\n", *pulCrc);

	hError = ERR_EPG_OK;

EXIT_EITLIB_FUNC :

	return hError;

}

/* end of file */
