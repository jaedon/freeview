/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     sxdescparse_unt.c
	@brief    sxdescparse_unt.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Unt(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
	ENUM_TO_STR(eSIxDESCTAG_UNT_SCHEDULING);
	ENUM_TO_STR(eSIxDESCTAG_UNT_UPDATE);
	ENUM_TO_STR(eSIxDESCTAG_UNT_SSU_LOCATION);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_MESSAGE);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_SSU_EVENT_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_TARGET_SMARTCARD);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_TARGET_IP_ADDRESS);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS);
 	ENUM_TO_STR(eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION);
	default:
		break;
	}

	return NULL;
}

// ==================================================================================
// = Internal Function Template
// = DO NOT Delete!!!
// ==================================================================================
#if 0
//XXX
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxXXXDes_t));
}

static SIxXXXDes_t *allocXXXDes(void)
{
	return (SIxXXXDes_t *)silib_desc_alloc(sizeof(SIxXXXDes_t));
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxXXXDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	return eSIxERR_FAIL;
}
#endif

/************************************************************************
 * eSIxDESCTAG_UNT_SCHEDULING							(TAG =  0x01)
 ************************************************************************/
//Scheduling
static void initSchedulingDes(SIxSchedulingDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSchedulingDes_t));
}

static SIxSchedulingDes_t *allocSchedulingDes(void)
{
	return (SIxSchedulingDes_t *)silib_desc_alloc(sizeof(SIxSchedulingDes_t));
}

static void freeSchedulingDes(SIxSchedulingDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseSchedulingDes(HUINT8 *p, SIxSchedulingDes_t *pstDes)
{
	HUINT8			ucDesLen;
	UNIXTIME		ulStartDateTime, ulEndDateTime;
	HUINT8			ucUtcTime[5];
	HUINT8			ucTemp;
	HUINT8			ucFinalAvailability, ucPeriodicityFlag, ucPeriodUnit, ucDurationUnit, ucEstimatedCycleTimeUnit;
	HUINT8			ucPeriod, ucDuration, ucEstimatedCycleTime;

	HxLOG_Info("parseSchedulingDes() Entered!");

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseSchedulingDes Format!\n");
		return eSIxERR_INVALID;
	}

	// start date time - UTC time (5byte)
	HxSTD_MemCopy(ucUtcTime, p, 5);
	p += 5; ucDesLen -= 5;
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime (ucUtcTime, &ulStartDateTime);
	HxLOG_Print("\t\t[SchedulingDes] start date time(UTC time) = 0x%08X)\n", ulStartDateTime);


	// end date time - UTC time (5byte)
	HxSTD_MemCopy(ucUtcTime, p, 5);
	p += 5; ucDesLen -= 5;
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime (ucUtcTime, &ulEndDateTime);
	HxLOG_Print("\t\t[SchedulingDes] end date time(UTC time) = 0x%08X)\n", ulEndDateTime);

	// final_availability, periodicity_flag, period_unit, duration_unit and estimated_cycle_time_unit (1byte)
	ucTemp = *p++;
	ucDesLen--;
	ucFinalAvailability = (HUINT8)((ucTemp & 0x80) >> 7);
	ucPeriodicityFlag = (HUINT8)((ucTemp & 0x40) >> 6);
	ucPeriodUnit = (HUINT8)((ucTemp & 0x30) >> 4);
	ucDurationUnit = (HUINT8)((ucTemp & 0x0C) >> 2);
	ucEstimatedCycleTimeUnit = (HUINT8)(ucTemp & 0x03);
	HxLOG_Print("\t\t[SchedulingDes] final_availability = 0x%X\n", ucFinalAvailability);
	HxLOG_Print("\t\t[SchedulingDes] periodicity_flag = 0x%X\n", ucPeriodicityFlag);
	HxLOG_Print("\t\t[SchedulingDes] period_unit = 0x%X\n", ucPeriodUnit);
	HxLOG_Print("\t\t[SchedulingDes] duration_unit = 0x%X\n", ucDurationUnit);
	HxLOG_Print("\t\t[SchedulingDes] estimated_cycle_time_unit = 0x%X\n", ucEstimatedCycleTimeUnit);

	// Period (1byte)
	ucPeriod = *p++;
	ucDesLen--;
	HxLOG_Print("\t\t[SchedulingDes] Period = %d\n", ucPeriod);

	// Duration (1byte)
	ucDuration = *p++;
	ucDesLen--;
	HxLOG_Print("\t\t[SchedulingDes] Duration = %d\n", ucDuration);

	// estimated_cycle_time (1byte)
	ucEstimatedCycleTime = *p++;
	ucDesLen--;
	HxLOG_Print("\t\t[SchedulingDes] estimated_cycle_time = %d\n", ucEstimatedCycleTime);

	// assign parsing results
	pstDes->startDateTime			 = ulStartDateTime;
	pstDes->endDateTime				 = ulEndDateTime;
	pstDes->ucIsFinalAvailable		 = ucFinalAvailability;
	pstDes->ucIsPeriodic			 = ucPeriodicityFlag;
	pstDes->ucPeriodUnit			 = ucPeriodUnit;
	pstDes->ucDurationUnit			 = ucDurationUnit;
	pstDes->ucEstimatedCycleUnit	 = ucEstimatedCycleTimeUnit;
	pstDes->ucPeriod				 = ucPeriod;
	pstDes->ucDuration				 = ucDuration;
	pstDes->ucEstimatedCycle		 = ucEstimatedCycleTime;

	HxLOG_Info(" parseSchedulingDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySchedulingDes(SIxSchedulingDes_t *pstDest, const SIxSchedulingDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSchedulingDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_UNT_UPDATE								(TAG =  0x02)
 ************************************************************************/
//Update
static void initUpdateDes(SIxUpdateDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUpdateDes_t));
}

static SIxUpdateDes_t *allocUpdateDes(void)
{
	return (SIxUpdateDes_t *)silib_desc_alloc(sizeof(SIxUpdateDes_t));
}

static void freeUpdateDes(SIxUpdateDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseUpdateDes(HUINT8 *p, SIxUpdateDes_t *pstDes)
{
	HUINT8			ucDesLen;
	HUINT8			ucTemp;
	HUINT8			ucUpdateFlag, ucUpdateMethod, ucUpdatePriority;

	HxLOG_Info("parseUpdateDes() Entered!");

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseUpdateDes Format!\n");
		return eSIxERR_INVALID;
	}

	// update_flag, update_method and update_priority (1byte)
	ucTemp = *p++;
	ucDesLen--;
	ucUpdateFlag = (HUINT8)((ucTemp & 0xC0) >> 6);
	ucUpdateMethod = (HUINT8)((ucTemp & 0x3C) >> 2);
	ucUpdatePriority = (HUINT8)(ucTemp & 0x03);
	HxLOG_Print("\t\t(update_flag = 0x%X)\n", ucUpdateFlag);
	HxLOG_Print("\t\t(update_method = 0x%X)\n", ucUpdateMethod);
	HxLOG_Print("\t\t(update_priority = 0x%X)\n", ucUpdatePriority);

	// assign parsing results
	pstDes->ucUpdateFlag		= ucUpdateFlag;
	pstDes->ucUpdateMethod		= ucUpdateMethod;
	pstDes->ucUpdatePriority	= ucUpdatePriority;

	HxLOG_Info(" parseUpdateDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyUpdateDes(SIxUpdateDes_t *pstDest, const SIxUpdateDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxUpdateDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_UNT_SSU_LOCATION						(TAG =  0x03)
 ************************************************************************/
//SsuLocation
static void initSsuLocationDes(SIxSsuLocationDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSsuLocationDes_t));
}

static SIxSsuLocationDes_t *allocSsuLocationDes(void)
{
	return (SIxSsuLocationDes_t *)silib_desc_alloc(sizeof(SIxSsuLocationDes_t));
}

static void freeSsuLocationDes(SIxSsuLocationDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseSsuLocationDes(HUINT8 *p, SIxSsuLocationDes_t *pstDes)
{
	HUINT8				ucDesLen;
	HUINT16				usDataBroadcastId;
	HUINT16				usAssociationTag;

	HxLOG_Info("parseSsuLocationDes() Entered!");

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 2)
	{
		HxLOG_Print("[WARN] Unknown parseSsuLocationDes Format!\n");
		return eSIxERR_INVALID;
	}

	// data_broadcast_id (2byte)
	usDataBroadcastId = HxMACRO_Get16Bit(p);
	p += 2; ucDesLen -= 2;
	HxLOG_Print("\t\t(data_broadcast_id = 0x%04X)\n", usDataBroadcastId);

	// association_tag (2byte)
	if(usDataBroadcastId == 0x000A)
	{
		usAssociationTag = HxMACRO_Get16Bit(p);
		p += 2; ucDesLen -= 2;
		HxLOG_Print("\t\t(association_tag = 0x%04X)\n", usAssociationTag);
		pstDes->usAssociationTag = usAssociationTag;
	}

	// assign parsing results
	pstDes->usDataBroadcastId = usDataBroadcastId;

	HxLOG_Info(" parseSsuLocationDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySsuLocationDes(SIxSsuLocationDes_t *pstDest, const SIxSsuLocationDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSsuLocationDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * DESCTAG_UNT_MESSAGE								(TAG =  0x04)
 ************************************************************************/
//Msg
static void initMsgDes(SIxMsgDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxMsgDes_t));
}

static SIxMsgDes_t *allocMsgDes(void)
{
	return (SIxMsgDes_t *)silib_desc_alloc(sizeof(SIxMsgDes_t));
}

static void freeMsgDes(SIxMsgDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pszText);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseMsgDes(HUINT8 *p, SIxMsgDes_t *pstDes)
{
	HUINT8		ucDesLen;
	HUINT8		ucDesNum, ucLastDesNum;
	HUINT8		szIso639LangCode[4], *pszText = NULL;
	HUINT8		ucTemp, ucTextLen;
	HUINT32 	nTempTextLen, ulSize;

	HxLOG_Info("parseMessageDes() Entered!");

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 5)
	{
		HxLOG_Print("[WARN] Unknown parseMessageDes Format!\n");
		return eSIxERR_INVALID;
	}

	// descriptor number & last descriptor number (1byte)
	ucTemp = *p++;
	ucDesLen--;
	ucDesNum = (HUINT8)((ucTemp & 0xf0) >> 4);
	ucLastDesNum = ucTemp & 0x0f;
	HxLOG_Print("\t\t(descriptor number = %d)\n", ucDesNum);
	HxLOG_Print("\t\t(last descriptor number = %d)\n", ucLastDesNum);

	// ISO 639 language code (3byte)
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	p += 3;
	ucDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

	// text length
	ucTextLen = ucDesLen;

	// Max allocation limit & allocation
	// nAssignedTextLen = (ucTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucTextLen);

	// event name
	nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, ucTextLen, (const HUINT8 *)p, &pszText);
	if(pszText == NULL)
		goto err_exit;

	p += ucDesLen;

	HxLOG_Print("\t\t(text = %s)\n", pszText);

	// Descriptor 데이타 구성.
	pstDes->pszText = silib_desc_alloc (nTempTextLen + 1);
	if (pstDes->pszText == NULL)
		goto err_exit;

	// set message data
	pstDes->ucDesNum = ucDesNum;
	pstDes->ucLastDesNum = ucLastDesNum;
	ulSize = silib_desc_util_dvbStrlen(szIso639LangCode);
	HxSTD_MemCopy(pstDes->szIso639LangCode, szIso639LangCode, ulSize);
	pstDes->szIso639LangCode[ulSize] = '\0';
	ulSize = silib_desc_util_dvbStrlen(pszText);
	HxSTD_MemCopy(pstDes->pszText, pszText, ulSize);
	pstDes->pszText[ulSize] = '\0';

	silib_desc_free(pszText);

	HxLOG_Info(" parseMessageDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszText);
	return eSIxERR_FAIL;
}

static SIxError_e copyMsgDes(SIxMsgDes_t *pstDest, const SIxMsgDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMsgDes_t));
		pstDest->pszText = NULL;
		if (pstSource->pszText)
		{
			HUINT32 ulSize;
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszText);
			pstDest->pszText = silib_desc_alloc(ulSize+1);
			if (NULL == pstDest->pszText)
				goto err_exit;
			HxSTD_MemCopy(pstDest->pszText, pstSource->pszText, ulSize);
			pstDest->pszText[ulSize] = '\0';
		}

		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pszText);
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_UNT_SSU_EVENT_NAME						(TAG =  0x05)
 ************************************************************************/
/************************************************************************
 * eSIxDESCTAG_UNT_TARGET_SMARTCARD					(TAG =  0x06)
 ************************************************************************/
/************************************************************************
 * eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS					(TAG =  0x07)
 ************************************************************************/
/************************************************************************
 * eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER				(TAG =  0x08)
 ************************************************************************/
/************************************************************************
 * eSIxDESCTAG_UNT_TARGET_IP_ADDRESS					(TAG =  0x09)
 ************************************************************************/
/************************************************************************
 * eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS					(TAG =  0x0A)
 ************************************************************************/

/************************************************************************
 * eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION			(TAG =  0x0B)
 ************************************************************************/
//SsuSubgroupAssociation
static void initSsuSubgroupAssociationDes(SIxSsuSubgroupAssociationDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSsuSubgroupAssociationDes_t));
}

static SIxSsuSubgroupAssociationDes_t *allocSsuSubgroupAssociationDes(void)
{
	return (SIxSsuSubgroupAssociationDes_t *)silib_desc_alloc(sizeof(SIxSsuSubgroupAssociationDes_t));
}

static void freeSsuSubgroupAssociationDes(SIxSsuSubgroupAssociationDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseSsuSubgroupAssociationDes(HUINT8 *p, SIxSsuSubgroupAssociationDes_t *pstDes)
{
	HUINT8			ucDesLen;
	HUINT8			ucSubgroupTag[6];

	HxLOG_Info("parseSsuSubgroupAssociationDes() Entered!");

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen > 5 || ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseSsuSubgroupAssociationDes Format!\n");
		return eSIxERR_INVALID;
	}

	// subgroup_tag (5byte)
	HxSTD_MemCopy(ucSubgroupTag, p, 5);
	ucSubgroupTag[5] = '\0';
	p += 5; ucDesLen -= 5;
	HxLOG_Print("\t\t[parseSsuSubgroupAssociationDes] (subgroup_tag = 0x%02X%02X %02X%02X%02X)\n",
		ucSubgroupTag[0], ucSubgroupTag[1], ucSubgroupTag[2], ucSubgroupTag[3], ucSubgroupTag[4]);

	// assign parsing results
	pstDes->subgroupTag[0] = ucSubgroupTag[0];
	pstDes->subgroupTag[1] = ucSubgroupTag[1];
	pstDes->subgroupTag[2] = ucSubgroupTag[2];
	pstDes->subgroupTag[3] = ucSubgroupTag[3];
	pstDes->subgroupTag[4] = ucSubgroupTag[4];

	HxLOG_Info(" parseSsuSubgroupAssociationDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySsuSubgroupAssociationDes(SIxSsuSubgroupAssociationDes_t *pstDest, const SIxSsuSubgroupAssociationDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSsuSubgroupAssociationDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


// ==================================================================================
// = Internal Function Template
// = DO NOT Delete!!!
// ==================================================================================
#if 0
//XXX
static SIxDescriptor_t *silib_desc_allocXXXDes(void)
{
	SIxXXXDes_t	*pstDes;

	pstDes = allocXXXDes();
	if (pstDes)
	{
		initXXXDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeXXXDes(SIxDescriptor_t *pstDes)
{
	freeXXXDes((SIxXXXDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseXXXDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseXXXDes((HUINT8 *)pucRawData, (SIxXXXDes_t *)pstDes);
}

static SIxError_e silib_desc_copyXXXDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyXXXDes((SIxXXXDes_t *)pstDest, (SIxXXXDes_t *)pstSource);
}

static void silib_desc_printXXXDes(const SIxDescriptor_t *pstDes)
{
	//(SIxXXXDes_t *)pstDes;
}
#endif

/*	eSIxDESCTAG_UNT_SCHEDULING					(0x01)					*/
//Scheduling
static SIxDescriptor_t *silib_desc_allocSchedulingDes(void)
{
	SIxSchedulingDes_t	*pstDes;

	pstDes = allocSchedulingDes();
	if (pstDes)
	{
		initSchedulingDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSchedulingDes(SIxDescriptor_t *pstDes)
{
	freeSchedulingDes((SIxSchedulingDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSchedulingDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSchedulingDes((HUINT8 *)pucRawData, (SIxSchedulingDes_t *)pstDes);
}

static SIxError_e silib_desc_copySchedulingDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySchedulingDes((SIxSchedulingDes_t *)pstDest, (SIxSchedulingDes_t *)pstSource);
}

static void silib_desc_printSchedulingDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSchedulingDes_t *)pstDes;
}

/*	eSIxDESCTAG_UNT_UPDATE						(0x02)					*/
//Update
static SIxDescriptor_t *silib_desc_allocUpdateDes(void)
{
	SIxUpdateDes_t	*pstDes;

	pstDes = allocUpdateDes();
	if (pstDes)
	{
		initUpdateDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeUpdateDes(SIxDescriptor_t *pstDes)
{
	freeUpdateDes((SIxUpdateDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseUpdateDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseUpdateDes((HUINT8 *)pucRawData, (SIxUpdateDes_t *)pstDes);
}

static SIxError_e silib_desc_copyUpdateDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyUpdateDes((SIxUpdateDes_t *)pstDest, (SIxUpdateDes_t *)pstSource);
}

static void silib_desc_printUpdateDes(const SIxDescriptor_t *pstDes)
{
	//(SIxUpdateDes_t *)pstDes;
}

/*	eSIxDESCTAG_UNT_SSU_LOCATION				(0x03)					*/
//SsuLocation
static SIxDescriptor_t *silib_desc_allocSsuLocationDes(void)
{
	SIxSsuLocationDes_t	*pstDes;

	pstDes = allocSsuLocationDes();
	if (pstDes)
	{
		initSsuLocationDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSsuLocationDes(SIxDescriptor_t *pstDes)
{
	freeSsuLocationDes((SIxSsuLocationDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSsuLocationDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSsuLocationDes((HUINT8 *)pucRawData, (SIxSsuLocationDes_t *)pstDes);
}

static SIxError_e silib_desc_copySsuLocationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySsuLocationDes((SIxSsuLocationDes_t *)pstDest, (SIxSsuLocationDes_t *)pstSource);
}

static void silib_desc_printSsuLocationDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSsuLocationDes_t *)pstDes;
}

/* 	eSIxDESCTAG_UNT_MESSAGE					(0x04)					*/
//Msg
static SIxDescriptor_t *silib_desc_allocMsgDes(void)
{
	SIxMsgDes_t	*pstDes;

	pstDes = allocMsgDes();
	if (pstDes)
	{
		initMsgDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeMsgDes(SIxDescriptor_t *pstDes)
{
	freeMsgDes((SIxMsgDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseMsgDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseMsgDes((HUINT8 *)pucRawData, (SIxMsgDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMsgDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMsgDes((SIxMsgDes_t *)pstDest, (SIxMsgDes_t *)pstSource);
}

static void silib_desc_printMsgDes(const SIxDescriptor_t *pstDes)
{
	//(SIxMsgDes_t *)pstDes;
}

/* 	eSIxDESCTAG_UNT_SSU_EVENT_NAME				(0x05)					*/

/* 	eSIxDESCTAG_UNT_TARGET_SMARTCARD			(0x06)					*/

/* 	eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS			(0x07)					*/

/* 	eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER		(0x08)					*/

/* 	eSIxDESCTAG_UNT_TARGET_IP_ADDRESS			(0x09)					*/

/* 	eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS		(0x0A)					*/

/* 	eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION	(0x0B)					*/
//SsuSubgroupAssociation
static SIxDescriptor_t *silib_desc_allocSsuSubgroupAssociationDes(void)
{
	SIxSsuSubgroupAssociationDes_t	*pstDes;

	pstDes = allocSsuSubgroupAssociationDes();
	if (pstDes)
	{
		initSsuSubgroupAssociationDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSsuSubgroupAssociationDes(SIxDescriptor_t *pstDes)
{
	freeSsuSubgroupAssociationDes((SIxSsuSubgroupAssociationDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSsuSubgroupAssociationDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSsuSubgroupAssociationDes((HUINT8 *)pucRawData, (SIxSsuSubgroupAssociationDes_t *)pstDes);
}

static SIxError_e silib_desc_copySsuSubgroupAssociationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySsuSubgroupAssociationDes((SIxSsuSubgroupAssociationDes_t *)pstDest, (SIxSsuSubgroupAssociationDes_t *)pstSource);
}

static void silib_desc_printSsuSubgroupAssociationDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSsuSubgroupAssociationDes_t *)pstDes;
}



SIxDescriptor_t *silib_desc_alloc_Unt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
	case eSIxDESCTAG_UNT_SCHEDULING:				return silib_desc_allocSchedulingDes();				break;
	case eSIxDESCTAG_UNT_UPDATE:					return silib_desc_allocUpdateDes();					break;
	case eSIxDESCTAG_UNT_SSU_LOCATION: 			return silib_desc_allocSsuLocationDes();			break;
	case eSIxDESCTAG_UNT_MESSAGE:					return silib_desc_allocMsgDes();					break;
	case eSIxDESCTAG_UNT_SSU_EVENT_NAME:			break; // return silib_desc_allocXXXDes(); 			break;
	case eSIxDESCTAG_UNT_TARGET_SMARTCARD: 		break; // return silib_desc_allocXXXDes(); 			break;
	case eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS:		break; // return silib_desc_allocXXXDes(); 			break;
	case eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER: 	break; // return silib_desc_allocXXXDes(); 			break;
	case eSIxDESCTAG_UNT_TARGET_IP_ADDRESS:		break; // return silib_desc_allocXXXDes(); 			break;
	case eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS:		break; // return silib_desc_allocXXXDes(); 			break;
	case eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION: return silib_desc_allocSsuSubgroupAssociationDes();	break;
	default:									break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Unt(SIxDescriptor_t *pstDes)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_UNT_SCHEDULING:				eErr = silib_desc_freeSchedulingDes(pstDes);				break;
	case eSIxDESCTAG_UNT_UPDATE:					eErr = silib_desc_freeUpdateDes(pstDes);					break;
	case eSIxDESCTAG_UNT_SSU_LOCATION: 			eErr = silib_desc_freeSsuLocationDes(pstDes);				break;
	case eSIxDESCTAG_UNT_MESSAGE:					eErr = silib_desc_freeMsgDes(pstDes);						break;
	case eSIxDESCTAG_UNT_SSU_EVENT_NAME:			break; // eErr = silib_desc_freeXXXDes(pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_SMARTCARD: 		break; // eErr = silib_desc_freeXXXDes(pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS:		break; // eErr = silib_desc_freeXXXDes(pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER: 	break; // eErr = silib_desc_freeXXXDes(pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_IP_ADDRESS:		break; // eErr = silib_desc_freeXXXDes(pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS:		break; // eErr = silib_desc_freeXXXDes(pstDes); 			break;
	case eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION: eErr = silib_desc_freeSsuSubgroupAssociationDes(pstDes);	break;
	default:									break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Unt(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_UNT_SCHEDULING:				eErr = silib_desc_parseSchedulingDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_UNT_UPDATE:					eErr = silib_desc_parseUpdateDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_UNT_SSU_LOCATION: 			eErr = silib_desc_parseSsuLocationDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_UNT_MESSAGE:					eErr = silib_desc_parseMsgDes(pucRawData, pstDes);						break;
	case eSIxDESCTAG_UNT_SSU_EVENT_NAME:			break; // eErr = silib_desc_parseXXXDes(pucRawData, pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_SMARTCARD: 		break; // eErr = silib_desc_parseXXXDes(pucRawData, pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS:		break; // eErr = silib_desc_parseXXXDes(pucRawData, pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER: 	break; // eErr = silib_desc_parseXXXDes(pucRawData, pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_IP_ADDRESS:		break; // eErr = silib_desc_parseXXXDes(pucRawData, pstDes); 			break;
	case eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS:		break; // eErr = silib_desc_parseXXXDes(pucRawData, pstDes); 			break;
	case eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION: eErr = silib_desc_parseSsuSubgroupAssociationDes(pucRawData, pstDes);	break;
	default:									break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Unt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_UNT_SCHEDULING:				eErr = silib_desc_copySchedulingDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_UNT_UPDATE:					eErr = silib_desc_copyUpdateDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_UNT_SSU_LOCATION:				eErr = silib_desc_copySsuLocationDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_UNT_MESSAGE:					eErr = silib_desc_copyMsgDes(pstDest, pstSource);						break;
	case eSIxDESCTAG_UNT_SSU_EVENT_NAME:			break; // eErr = silib_desc_copyXXXDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_UNT_TARGET_SMARTCARD:			break; // eErr = silib_desc_copyXXXDes(pstDest, pstSource);				break;
 	case eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS:		break; // eErr = silib_desc_copyXXXDes(pstDest, pstSource);				break;
 	case eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER:		break; // eErr = silib_desc_copyXXXDes(pstDest, pstSource);				break;
 	case eSIxDESCTAG_UNT_TARGET_IP_ADDRESS:		break; // eErr = silib_desc_copyXXXDes(pstDest, pstSource);				break;
 	case eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS:		break; // eErr = silib_desc_copyXXXDes(pstDest, pstSource);				break;
 	case eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION:	eErr = silib_desc_copySsuSubgroupAssociationDes(pstDest, pstSource);	break;
	default:									break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Unt(const SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Unt(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
	case eSIxDESCTAG_UNT_SCHEDULING:				silib_desc_printSchedulingDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_UPDATE:					silib_desc_printUpdateDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_SSU_LOCATION:				silib_desc_printSsuLocationDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_MESSAGE:					silib_desc_printMsgDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_SSU_EVENT_NAME:			break;	// silib_desc_printXXXDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_TARGET_SMARTCARD:			break;	// silib_desc_printXXXDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS:		break;	// silib_desc_printXXXDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER:		break;	// silib_desc_printXXXDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_TARGET_IP_ADDRESS:		break;	// silib_desc_printXXXDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS:		break;	// silib_desc_printXXXDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION:	silib_desc_printSsuSubgroupAssociationDes(pstDes);	eErr = eSIxERR_OK;	break;
	default:									break;
	}

	return eErr;
}

