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

/******************************************************************************/
/**
 * @file	  		nx_port_meta_antenna.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_port.h>
#include <nx_common.h>


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static HCHAR *	nx_port_meta_antenna_getAntInfoUdbKeyBySlot(HINT32 slot);
static HCHAR *	nx_port_meta_antenna_getTpUdbKeyBySlot(HINT32 slot);
static HCHAR *	nx_port_meta_antenna_getAntSlotUdbKeyBySlot(HINT32 slot);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static HCHAR *	nx_port_meta_antenna_getAntInfoUdbKeyBySlot(HINT32 slot)
{
	HCHAR *pszKey = NULL;

	switch (slot)
	{
		case eNxSat_ANTENAA_SLOT1:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT0;			break;
		case eNxSat_ANTENAA_SLOT2:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT1;			break;
		case eNxSat_ANTENAA_SLOT3:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT2;			break;
		case eNxSat_ANTENAA_SLOT4:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT3;			break;
		case eNxSat_ANTENAA_SLOT5:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT4;			break;
		case eNxSat_ANTENAA_SLOT6:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT5;			break;
		case eNxSat_ANTENAA_SLOT7:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT6;			break;
		case eNxSat_ANTENAA_SLOT8:			pszKey 	= USERDB_ANTSET_ANTINFO_SLOT7;			break;
		default:
			pszKey 	= NULL;
			NX_APP_Error("[%s:%d] Error!! Invalid Slot:[%d]\n", __FUNCTION__, __LINE__, slot);
			break;
	}

	return pszKey;
}


static HCHAR *	nx_port_meta_antenna_getTpUdbKeyBySlot(HINT32 slot)
{
	HCHAR *pszKey = NULL;

	switch (slot)
	{
		case eNxSat_ANTENAA_SLOT1:			pszKey 	= USERDB_ANTSET_TP_SLOT0;			break;
		case eNxSat_ANTENAA_SLOT2:			pszKey 	= USERDB_ANTSET_TP_SLOT1;			break;
		case eNxSat_ANTENAA_SLOT3:			pszKey 	= USERDB_ANTSET_TP_SLOT2;			break;
		case eNxSat_ANTENAA_SLOT4:			pszKey 	= USERDB_ANTSET_TP_SLOT3;			break;
		case eNxSat_ANTENAA_SLOT5:			pszKey 	= USERDB_ANTSET_TP_SLOT4;			break;
		case eNxSat_ANTENAA_SLOT6:			pszKey 	= USERDB_ANTSET_TP_SLOT5;			break;
		case eNxSat_ANTENAA_SLOT7:			pszKey 	= USERDB_ANTSET_TP_SLOT6;			break;
		case eNxSat_ANTENAA_SLOT8:			pszKey 	= USERDB_ANTSET_TP_SLOT7;			break;
		case eNxSat_ANTENAA_SLOT_TEST:		pszKey 	= USERDB_ANTSET_TP_TEST;			break;
		default:
			pszKey 	= NULL;
			NX_APP_Error("[%s:%d] Error!! Invalid Slot:[%d]\n", __FUNCTION__, __LINE__, slot);
			break;
	}

	return pszKey;
}


static HCHAR *	nx_port_meta_antenna_getAntSlotUdbKeyBySlot(HINT32 slot)
{
	HCHAR *pszKey = NULL;

	switch (slot)
	{
		case eNxSat_ANTENAA_SLOT1:			pszKey 	= USERDB_FIRST_ANTENNA_ID;			break;
		case eNxSat_ANTENAA_SLOT2:			pszKey 	= USERDB_SECOND_ANTENNA_ID;			break;
		case eNxSat_ANTENAA_SLOT3:			pszKey 	= USERDB_THIRD_ANTENNA_ID;			break;
		case eNxSat_ANTENAA_SLOT4:			pszKey 	= USERDB_FOURTH_ANTENNA_ID;			break;
		case eNxSat_ANTENAA_SLOT5:			pszKey 	= USERDB_FIFTH_ANTENNA_ID;			break;
		case eNxSat_ANTENAA_SLOT6:			pszKey 	= USERDB_SIXTH_ANTENNA_ID;			break;
		case eNxSat_ANTENAA_SLOT7:			pszKey 	= USERDB_SEVENTH_ANTENNA_ID;		break;
		case eNxSat_ANTENAA_SLOT8:			pszKey 	= USERDB_EIGHTH_ANTENNA_ID;			break;
		default:
			pszKey 	= NULL;
			NX_APP_Error("[%s:%d] Error!! Invalid Slot:[%d]\n", __FUNCTION__, __LINE__, slot);
			break;
	}

	return pszKey;
}


#define ________________RELATED_META_ANTENNA_________________

HINT32	NX_PORT_META_ANTENNA_Add (const DxAntInfo_t *antenna)
{
	return APK_META_ANTENNA_Add (antenna);
}

void	NX_PORT_META_ANTENNA_RemoveAll (void)
{
	APK_META_ANTENNA_RemoveAll();
}

void	NX_PORT_META_ANTENNA_SetInfo(HINT32 uid, const DxAntInfo_t *info)
{
	APK_META_ANTENNA_SetInfo(uid, info);
}

void 	NX_PORT_META_ANTENNA_InfoFree (DxAntInfo_t *p)
{
	APK_META_ANTENNA_InfoFree (p);
}

HERROR	NX_PORT_META_ANTENNA_Save (void)
{
	return APK_META_ANTENNA_Save();
}

HERROR	NX_PORT_META_ANTENNA_Load (void)
{
	return APK_META_ANTENNA_Load();
}

HUINT32	NX_PORT_META_ANTENNA_Count (void)
{
	return	APK_META_ANTENNA_Count();
}

HINT32	NX_PORT_META_ANTENNA_GetUidAt (HINT32 index)
{
	return	APK_META_ANTENNA_GetUidAt(index);
}

DxAntInfo_t * NX_PORT_META_ANTENNA_GetInfo (HINT32 uid)
{
	return	APK_META_ANTENNA_GetInfo (uid);
}

HERROR	NX_PORT_META_ANTENNA_GetConnectionType(DxAntennaConnection_e *eAntConnetion)
{
	return APK_SCENARIO_GetAntennaConnection(eAntConnetion);
}

#define ________________RELATED_USER_ANTENNA_________________

HUINT32		NX_PORT_META_ANTENNA_GetAntennaUDBCount(void)
{
	HCHAR		*pszKey = NULL;
	HINT32		i = 0, lValue = NX_ANT_IDX_NULL;
	HUINT32		ulCount = 0;
	HERROR		hError = ERR_FAIL;

	for (i = 0; i < NX_PORT_ANTENNA_MAX_ANT_NUM; i++)
	{
		pszKey = nx_port_meta_antenna_getAntSlotUdbKeyBySlot(i+1);
		if (pszKey)
		{
			hError = NX_PORT_DB_GetInt(pszKey, &lValue);
			if (hError == ERR_OK)
			{
				if (lValue != (HINT32)NX_ANT_IDX_NULL)
				{
					ulCount++;
				}
			}
			else
			{
				NX_APP_Error("[%s:%d] Error!! ID : [%s][%d]\n", __FUNCTION__, __LINE__, pszKey, lValue);
			}
		}
	}

	return	ulCount;
}


HERROR		NX_PORT_META_ANTENNA_SetAntIndexUDB (HINT32 lSlot, HINT32 lAntIdx)
{
	HCHAR		*pszKey = NULL;
	HERROR		hError = ERR_FAIL;

	pszKey = nx_port_meta_antenna_getAntSlotUdbKeyBySlot(lSlot);
	if (pszKey)
	{
		hError = NX_PORT_DB_SetInt(pszKey, lAntIdx);
		if (hError != ERR_OK)
		{
			NX_APP_Error("[%s:%d] Error!! ID : [%s][%d]\n", __FUNCTION__, __LINE__, pszKey, lAntIdx);
		}
	}
	else
	{
		NX_APP_Error("[%s:%d] Error!! lAntIdx [%d]\n", __FUNCTION__, __LINE__, lAntIdx);
	}

	return	ERR_OK;
}


HERROR		NX_PORT_META_ANTENNA_GetAntIndexUDB (HINT32 lSlot, HINT32 *plAntIdx)
{
	HCHAR		*pszKey = NULL;
	HERROR		hError = ERR_FAIL;

	*plAntIdx = NX_ANT_IDX_NULL;
	pszKey = nx_port_meta_antenna_getAntSlotUdbKeyBySlot(lSlot);
	if (pszKey)
	{
		hError = NX_PORT_DB_GetInt(pszKey, plAntIdx);
		if (hError != ERR_OK)
		{
			NX_APP_Error("[%s:%d] Error!! ID : [%s][%d]\n", __FUNCTION__, __LINE__, pszKey, plAntIdx);
		}
	}

	return	ERR_OK;
}


HERROR 	NX_PORT_META_ANTENNA_SaveUserAntenna(HINT32 slot, DxAntInfo_t *pstAntInfo)
{
	HCHAR	*pszKey = NULL;
	HCHAR	szdata[NX_TEXT_SIZE_128] = {'\0',};
	HCHAR 	szInfo[NX_TEXT_SIZE_128]	= {'\0',};

	pszKey = nx_port_meta_antenna_getAntInfoUdbKeyBySlot(slot);
	if (pszKey == NULL)
	{
		HxLOG_Debug("this invalid slot!!!\n");
		return ERR_FAIL;
	}

	if (pstAntInfo->antType == eDxANT_TYPE_LNB_ONLY)
	{
		HxSTD_sprintf(szInfo, "%u:%d",
				pstAntInfo->info.lnb.ulLnbFreq,
				pstAntInfo->info.lnb.lnbVoltage);
	}
	else if (pstAntInfo->antType == eDxANT_TYPE_DISEQC)
	{
		HxSTD_sprintf(szInfo, "%u:%d:%d:%d:%d:%d:%u:%d",
				pstAntInfo->info.diseqc.ulLnbFreq,
				pstAntInfo->info.diseqc.lnbVoltage,
				pstAntInfo->info.diseqc.b22kTone,
				pstAntInfo->info.diseqc.diseqcVersion,
				pstAntInfo->info.diseqc.diseqcInput,
				pstAntInfo->info.diseqc.toneBurst,
				pstAntInfo->info.diseqc.ucMotorPosition,
				pstAntInfo->info.diseqc.sSatLonggitude);
	}
	else if (pstAntInfo->antType == eDxANT_TYPE_SCD)
	{
		HxSTD_sprintf(szInfo, "%u:%d:%d:%d:%u:%u:%u:%u:%u:%u:%u:%u",
				pstAntInfo->info.scd.ulLnbFreq,
				pstAntInfo->info.scd.lnbVoltage,
				pstAntInfo->info.scd.b22kTone,
				pstAntInfo->info.scd.scdInput,
				pstAntInfo->info.scd.ulScdUserBandNum,
				pstAntInfo->info.scd.ulScdUserBandFreq,
				pstAntInfo->info.scd.ulScdUserBandNum1,
				pstAntInfo->info.scd.ulScdUserBandFreq1,
				pstAntInfo->info.scd.ulScdUserBandNum2,
				pstAntInfo->info.scd.ulScdUserBandFreq2,
				pstAntInfo->info.scd.ulScdUserBandNum3,
				pstAntInfo->info.scd.ulScdUserBandFreq3);
	}
	else if (pstAntInfo->antType == eDxANT_TYPE_SMATV)
	{
		// TODO: [JHLEE] Add info ??
		NX_APP_Error("Error!! Not Support Format eDxANT_TYPE_SMATV\n");
	}
	else if (pstAntInfo->antType == eDxANT_TYPE_MONOBLOCK_LNB)
	{
		// TODO: [JHLEE] Add info ??
		NX_APP_Error("Error!! Not Support Format eDxANT_TYPE_MONOBLOCK_LNB\n");
	}
	else
	{
		// TODO: [JHLEE] Add info ??
		NX_APP_Error("Error!! Not Support Format Unknow Type:(%d)\n", pstAntInfo->antType);
	}

	HxSTD_sprintf(szdata, "antinfo:%d:%d:%s:%s:%s",
			pstAntInfo->antType,
			pstAntInfo->uid,
			pstAntInfo->satType,
			pstAntInfo->satName,
			szInfo);

	HxLOG_Debug("SET ANTENNA USER DB (%s) (%s) \n",pszKey, szdata);

	return NX_PORT_DB_SetStr(pszKey, szdata);
}


HERROR NX_PORT_META_ANTENNA_LoadUserAntenna(HINT32 slot, DxAntInfo_t *dstAntInfo)
{
	HCHAR	*pszKey = NULL;
	HCHAR	szdata[NX_TEXT_SIZE_128] = {'\0',};
	HCHAR	buf[NX_TEXT_SIZE_128] = {'\0',}, *token = NULL, *save = NULL;

	DxAntInfo_t	antInfo;

	if (dstAntInfo == NULL)
	{
		HxLOG_Debug("dstAntInfo is NULL !\n");
		return ERR_FAIL;
	}

	pszKey = nx_port_meta_antenna_getAntInfoUdbKeyBySlot(slot);
	if (pszKey == NULL)
	{
		HxLOG_Debug("this invalid slot!!!\n");
		return ERR_FAIL;
	}

	if (NX_PORT_DB_GetStr(pszKey, szdata, 128) != ERR_OK)
	{
		HxLOG_Debug("Failed load user anntenna info data!!\n");
		return ERR_FAIL;
	}


	if (!HLIB_STD_StrStartWith((const HCHAR *)szdata, "antinfo:"))
	{
		HxLOG_Debug("No AntInfo Data! (slot:%d)\n", slot);
        return ERR_FAIL;
	}

	HxSTD_MemSet(&antInfo, 0, sizeof(DxAntInfo_t));

    HLIB_STD_StrNCpySafe(buf, (const HCHAR *)szdata + NX_PORT_META_ANTENNA_ANTINFO_HEAD, 128);


	// anttype
    token = HxSTD_StrTok(buf, ":", &save);
    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
	antInfo.antType	= (int)HLIB_STD_StrToINT32(token);

	// uid
	token = HxSTD_StrTok(save, ":", &save);
    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
	antInfo.uid	= (int)HLIB_STD_StrToINT32(token);

	// sattype
	token = HxSTD_StrTok(save, ":", &save);
    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	HLIB_STD_StrUtf8NCpy(antInfo.satType, token, DxANTINFO_NAME_LEN);

	// satname
	token = HxSTD_StrTok(save, ":", &save);
    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	HLIB_STD_StrUtf8NCpy(antInfo.satName, token, DxANTINFO_NAME_LEN);

	if (antInfo.antType == eDxANT_TYPE_LNB_ONLY)
	{
		// ulLnbFreq
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.lnb.ulLnbFreq	= (int)HLIB_STD_StrToUINT32(token);

		// lnbVoltage
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.lnb.lnbVoltage	= (int)HLIB_STD_StrToINT32(token);
	}
	else
	if (antInfo.antType == eDxANT_TYPE_DISEQC)
	{
		// ulLnbFreq
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.ulLnbFreq	= (int)HLIB_STD_StrToUINT32(token);

		// lnbVoltage
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.lnbVoltage	= (int)HLIB_STD_StrToINT32(token);

		// b22kTone
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.b22kTone	= (int)HLIB_STD_StrToINT32(token);

		// diseqcVersion
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.diseqcVersion	= (int)HLIB_STD_StrToINT32(token);

		//diseqcInput
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.diseqcInput	= (int)HLIB_STD_StrToINT32(token);

		//toneBurst
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.toneBurst	= (int)HLIB_STD_StrToINT32(token);

		//ucMotorPosition
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.ucMotorPosition	= (int)HLIB_STD_StrToUINT32(token);

		//sSatLonggitude
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.diseqc.sSatLonggitude	= (int)HLIB_STD_StrToINT32(token);
	}
	else
	if (antInfo.antType == eDxANT_TYPE_SCD)
	{
		//ulLnbFreq
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulLnbFreq		= (int)HLIB_STD_StrToUINT32(token);

		//lnbVoltage
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.lnbVoltage		= (int)HLIB_STD_StrToINT32(token);

		//b22kTone
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.b22kTone		= (int)HLIB_STD_StrToINT32(token);

		//scdInput
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.scdInput		= (int)HLIB_STD_StrToINT32(token);

		//ulScdUserBandNum
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandNum	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandFreq
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandFreq	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandNum1
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandNum1	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandFreq1
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandFreq1	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandNum2
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandNum2	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandFreq2
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandFreq2	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandNum3
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandNum3	= (int)HLIB_STD_StrToUINT32(token);

		//ulScdUserBandFreq
		token = HxSTD_StrTok(save, ":", &save);
	    if (token == NULL){	HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
		//if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n");		return ERR_FAIL;	}
		antInfo.info.scd.ulScdUserBandFreq3	= (int)HLIB_STD_StrToUINT32(token);
	}
	else
	if (antInfo.antType == eDxANT_TYPE_SMATV){	}
	else
	if (antInfo.antType == eDxANT_TYPE_MONOBLOCK_LNB){	}


	HxSTD_MemCopy(dstAntInfo, &antInfo, sizeof(DxAntInfo_t));

	return ERR_OK;
}


HERROR 	NX_PORT_META_ANTENNA_SaveUserTp(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo)
{
	HCHAR	*pszKey = NULL;
	HCHAR	szdata[NX_TEXT_SIZE_128] = {'\0',};
	HCHAR	szInfo[NX_TEXT_SIZE_128] = {'\0',};

	pszKey = nx_port_meta_antenna_getTpUdbKeyBySlot(slot);
	if (pszKey == NULL)
	{
		HxLOG_Error("this invalid slot!!!\n");
		return	ERR_FAIL;
	}

	switch (pstTpInfo->eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			HxSTD_sprintf(szInfo, "%u:%u:%d:%d:%d:%d:%d:%d:%s:%u",
					pstTpInfo->stNxUserTpInfo.sat.ulFrequency,
					pstTpInfo->stNxUserTpInfo.sat.ulSymbolRate,
					pstTpInfo->stNxUserTpInfo.sat.eFecCodeRate,
					pstTpInfo->stNxUserTpInfo.sat.ePolarization,
					pstTpInfo->stNxUserTpInfo.sat.eTransSpec,
					pstTpInfo->stNxUserTpInfo.sat.ePskMod,
					pstTpInfo->stNxUserTpInfo.sat.ePilot,
					pstTpInfo->stNxUserTpInfo.sat.eRollOff,
					pstTpInfo->stNxUserTpInfo.sat.satType,
					pstTpInfo->stNxUserTpInfo.sat.antuid );
			break;
		case eDxDELIVERY_TYPE_TER:
			// TODO: add
			break;
		case eDxDELIVERY_TYPE_CAB:
			// TODO: add
			break;

		case eDxDELIVERY_TYPE_ALL:
		case eDxDELIVERY_TYPE_MULTICAST:
		default:
			HxLOG_Error("Not Support DELIVERY :[0x%x]\n", pstTpInfo->eDeliType);
			break;
	}

	HxSTD_sprintf(szdata, "tpinfo:%u:%d:%d:%u:%u:%u:%s",
			pstTpInfo->uid,	pstTpInfo->ucAvailable, pstTpInfo->ucUserTpDefined, pstTpInfo->eNxSearchTpType,
			pstTpInfo->ulTpInfoIndex, pstTpInfo->eDeliType,	szInfo);

	HxLOG_Debug("SET TP USER DB (%s) (%s) \n", pszKey, szdata);

	return	NX_PORT_DB_SetStr(pszKey, szdata);
}


HERROR	NX_PORT_META_ANTENNA_LoadUserTp(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo)
{
	HCHAR	*pszKey = NULL;
	HCHAR	szdata[NX_TEXT_SIZE_128] = {'\0',};
	HCHAR	buf[NX_TEXT_SIZE_128] = {'\0',}, *token = NULL, *save = NULL;
	Nx_TpSelectInfo_t	stTpInfo;

	if (pstTpInfo == NULL)
	{
		HxLOG_Error("pstTpInfo is NULL !\n");
		return	ERR_FAIL;
	}

	pszKey = nx_port_meta_antenna_getTpUdbKeyBySlot(slot);
	if (pszKey == NULL)
	{
		HxLOG_Error("this invalid slot!!!\n");
		return	ERR_FAIL;
	}

	if (NX_PORT_DB_GetStr(pszKey, szdata, 128) != ERR_OK)
	{
		HxLOG_Error("Failed load user tp info data!!\n");
		return	ERR_FAIL;
	}

	if (!HLIB_STD_StrStartWith((const HCHAR *)szdata, "tpinfo:"))
	{
		HxLOG_Error("No tp Data! (slot:%d)\n", slot);
		return	ERR_FAIL;
	}

	HxLOG_Debug("===== [Load User TP Info (%s)] =====\n", pszKey);

	HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
	HLIB_STD_StrNCpySafe(buf, (const HCHAR *)szdata + NX_PORT_META_ANTENNA_TPINFO_HEAD, 128);

	// uid
	token = HxSTD_StrTok(buf, ":", &save);
	if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n"); 	return ERR_FAIL;	}
	stTpInfo.uid = (int)HLIB_STD_StrToINT32(token);
	HxLOG_Debug("===== uid : [%u] =====\n", stTpInfo.uid);

	// ucAvailable
	token = HxSTD_StrTok(save, ":", &save);
	if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n"); 	return ERR_FAIL;	}
	stTpInfo.ucAvailable = (int)HLIB_STD_StrToINT32(token);
	HxLOG_Debug("===== ucAvailable : [%u] =====\n", stTpInfo.ucAvailable);

	// ucUserTpDefined
	token = HxSTD_StrTok(save, ":", &save);
	if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n"); 	return ERR_FAIL;	}
	stTpInfo.ucUserTpDefined = (int)HLIB_STD_StrToINT32(token);
	HxLOG_Debug("===== ucUserTpDefined : [%u] =====\n", stTpInfo.ucUserTpDefined);

	// eNxSearchTpType
	token = HxSTD_StrTok(save, ":", &save);
	if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n"); 	return ERR_FAIL;	}
	stTpInfo.eNxSearchTpType = (int)HLIB_STD_StrToINT32(token);
	HxLOG_Debug("===== eNxSearchTpType : [%u] =====\n", stTpInfo.eNxSearchTpType);

	// ulTpInfoIndex
	token = HxSTD_StrTok(save, ":", &save);
	if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n"); 	return ERR_FAIL;	}
	stTpInfo.ulTpInfoIndex = (int)HLIB_STD_StrToINT32(token);
	HxLOG_Debug("===== ulTpInfoIndex : [%u] =====\n", stTpInfo.ulTpInfoIndex);

	// eDeliType
	token = HxSTD_StrTok(save, ":", &save);
	if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
	if (!HLIB_STD_IsInteger(token)) {	HxLOG_Debug("Not Integer!!\n"); 	return ERR_FAIL;	}
	stTpInfo.eDeliType = (int)HLIB_STD_StrToINT32(token);
	HxLOG_Debug("===== eDeliType : [%u] =====\n", stTpInfo.eDeliType);

	switch (stTpInfo.eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			// ulFrequency
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.ulFrequency	= (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== ulFrequency : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.ulFrequency);

			// ulSymbolRate
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.ulSymbolRate = (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== ulSymbolRate : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.ulSymbolRate);

			// eFecCodeRate
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.eFecCodeRate = (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== eFecCodeRate : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.eFecCodeRate);

			// ePolarization
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.ePolarization = (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== ePolarization : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.ePolarization);

			// eTransSpec
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.eTransSpec = (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== eTransSpec : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.eTransSpec);

			// ePskMod
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.ePskMod = (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== ePskMod : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.ePskMod);

			// ePilot
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.ePilot = (int)HLIB_STD_StrToUINT32(token);

			// eRollOff
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.eRollOff = (int)HLIB_STD_StrToUINT32(token);

			// satType
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			HLIB_STD_StrUtf8NCpy(stTpInfo.stNxUserTpInfo.sat.satType, token, DxANTINFO_NAME_LEN);
			HxLOG_Debug("===== satType : [%s] =====\n", stTpInfo.stNxUserTpInfo.sat.satType);

			// antuid
			token = HxSTD_StrTok(save, ":", &save);
			if (token == NULL){ HxLOG_Debug("parse Error!!");	return ERR_FAIL;	}
			stTpInfo.stNxUserTpInfo.sat.antuid = (int)HLIB_STD_StrToUINT32(token);
			HxLOG_Debug("===== antuid : [%d] =====\n", stTpInfo.stNxUserTpInfo.sat.antuid);
			break;
		case eDxDELIVERY_TYPE_TER:
			// TODO: add
			break;
		case eDxDELIVERY_TYPE_CAB:
			// TODO: add
			break;

		case eDxDELIVERY_TYPE_ALL:
		case eDxDELIVERY_TYPE_MULTICAST:
		default:
			HxLOG_Error("Not Support DELIVERY :[0x%x]\n", stTpInfo.eDeliType);
			break;
	}

	HxLOG_Debug("=============================\n");

	HxSTD_MemCopy(pstTpInfo, &stTpInfo, sizeof(Nx_TpSelectInfo_t));

	return	ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

