/**
	@file     trport.c
	@brief    trport.c CWMP library porting

	Description: ggggggggggg		\n
	Module: 			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include "trport_common.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define TRPORT_TR_DB_FILENAME		"tr069.json"
#define TRPORT_DL_DB_FILENAME		"download.json"

#define TRPORT_DB_BACKUP_PATH		"/var/lib/humaxtv/tr069_backup/"
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
//tr069 process가 필요한 data를 관리하기 위한 db;
//Tr069 DataModel중 Octo에 없는 data들은 JSON DB를 이용하여 tr069 process에서 관리한다.
//임시 => 향후 Octo에서 관리할 경우 이부분에서 제외
typedef struct trport_object_default
{
	HCHAR							*parameterName;
	HCHAR							*defaultValue;
	HUINT32							ulen;
}trport_object_default_t;

trport_object_default_t s_HardcodingDefaultValue[] = {
	{"DeviceSummary", 			"", 1024},
	{"X_JCOM_SubscriberID", 	"", 256},
	{"X_JCOM_ServiceID", 		"", 256},
	{"X_JCOM_CorpCode", 		"", 256},
	{"Description", 			"Humax STB WA-7000", 256},
	{"ProductClass", 			"NGS_STB", 64},
	{"HardwareVersion",			"1.00.00", 64},
	{"Username", 				"jlabscpe", 256},
	{"Password", 				"d7PvfYx1", 256},
	{"PeriodicInformEnable",	"true", 256},
	{"PeriodicInformInterval", 	"8000", 256},
	{"PeriodicInformTime", 		"2013-01-01T00:00:00", 20},
	{"ParameterKey", 			"", 32},
	{"ConnectionRequestUsername", "humaxjpn", 256},
	{"ConnectionRequestPassword", "RdY6kqcD", 256},
	{"UpgradesManaged", 		"true", 8},
	{"CombinedAccess", 			"false", 256},
	{"PeriodicInformationInterval",  "8000", 11},
	{"PeriodicInformationTime",	"2013-01-01T00:00:00", 256},
	{"PermissionLevel", 		"1", 256},
	{NULL, NULL, 0}
};

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static	HxUDBHandle		_trdbHandle = NULL;
static	HxUDBHandle		_dldbHandle = NULL;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _____STATIC_FUNCTION________________________________________
HERROR	TRP_DB_getTextData(HCHAR *token, HCHAR *text);
HERROR	TRP_DB_setTextData(HCHAR *token, HCHAR *text);

#define TRPORT_DB_DOWNLOADID_TYPE			"type"
#define TRPORT_DB_DOWNLOADID_STATE			"state"
#define TRPORT_DB_DOWNLOADID_FILEPATH		"filepath"
#define TRPORT_DB_DOWNLOADID_FILENAME		"filename"
#define TRPORT_DB_DOWNLOADID_FILESIZE		"size"
#define TRPORT_DB_DOWNLOADID_RESET			"resetFlag"
#define TRPORT_DB_DOWNLOADID_PREVIOUS_VERSION "prevVersion"

static HERROR TRP_DB_LoadDefaultTRDB(void)
{
	trport_object_default_t	*objectloop;

	TRENTER;
	HxLOG_Assert(_trdbHandle);

	objectloop = s_HardcodingDefaultValue;
	while(objectloop->parameterName)
	{
		HxLOG_Debug("parameterName=[%-32s], value=[%s]\n", objectloop->parameterName, objectloop->defaultValue);
		TRP_DB_setTextData(objectloop->parameterName, objectloop->defaultValue);
		objectloop++;
	}

	TRLEAVE(ERR_OK);
}

static HERROR TRP_DB_LoadDefaultDNLDB(void)
{
	HUINT8					i;
	trport_download_info_t	stDefaultData;

	TRENTER;
	HxLOG_Assert(_dldbHandle);

	TRP_DB_SetResetFlag(eFLAG_NEED_TO_RESET);
	TRP_DB_SetPrevVersion(DEFAULT_PREV_SW_VERSION);

	VK_memset (&stDefaultData, 0x00, sizeof (trport_download_info_t));
	for(i=eDOWNLOAD_TYPE_FIRMWARE; i<eDOWNLOAD_TYPE_MAX; i++)
	{
		TRP_DB_StoreDownloadInfo(i, &stDefaultData);
	}

	TRLEAVE(ERR_OK);
}

HERROR	TRP_DB_getTextData(HCHAR *token, HCHAR *text)
{
	HCHAR	szBuffer[32];

	HxSTD_snprintf(szBuffer, 32, "%s", token);
	HLIB_UDB_GetText(_trdbHandle, szBuffer, text, 0);

	return ERR_OK;
}

HERROR	TRP_DB_setTextData(HCHAR *token, HCHAR *text)
{
	HCHAR	szBuffer[32];

	HxSTD_snprintf(szBuffer, 32, "%s", token);
	HLIB_UDB_SetText(_trdbHandle, szBuffer, text);
	HLIB_UDB_Sync(_trdbHandle);

	return ERR_OK;
}

#define _____GLOBAL_FUNCTION________________________________________
HERROR TRP_DB_Init(void)
{
	HBOOL		bNewFile, bReset;
	HERROR		hErr;
	TRENTER;

	HLIB_DIR_Make(TRPORT_DB_PATH);
	_trdbHandle = HLIB_UDB_CreateInstance(TRPORT_DB_PATH, TRPORT_TR_DB_FILENAME, &bNewFile, TRPORT_DB_BACKUP_PATH, 0);
	if (bNewFile)
	{
		HxLOG_Debug("[%s:%d] make new TR069 DB FILE[%s%s]\n",__FUNCTION__,__LINE__,TRPORT_DB_PATH, TRPORT_TR_DB_FILENAME);
		TRP_DB_LoadDefaultTRDB();
	}

	_dldbHandle = HLIB_UDB_CreateInstance(TRPORT_DB_PATH, TRPORT_DL_DB_FILENAME, &bNewFile, TRPORT_DB_BACKUP_PATH, 0);
	hErr = TRP_DB_GetResetFlag(&bReset);
	if (bNewFile || bReset == eFLAG_NEED_TO_RESET)
	{
		HxLOG_Debug("[%s:%d] make new download DB FILE[%s%s]\n",__FUNCTION__,__LINE__,TRPORT_DB_PATH, TRPORT_DL_DB_FILENAME);
		TRP_DB_LoadDefaultDNLDB();
	}
	else//download db도 존재하고, reset flag가 eFLAG_NO_NEED_TO_RESET일경우  => library가 reset을 시킨경우
	{
		TRP_DB_SetResetFlag(eFLAG_NEED_TO_RESET);
	}

	TRLEAVE(ERR_OK);
}

void TRP_DB_Deinit(void)
{
	TRENTER;
	HxLOG_Assert(_trdbHandle);

	HLIB_UDB_DestroyInstance(_trdbHandle, TRUE);
	HLIB_UDB_DestroyInstance(_dldbHandle, TRUE);
}

HERROR TRP_DB_GetDownloadInfo(HUINT32 ulId, trport_download_info_t *pstInfo)
{
	HERROR 		hErr = ERR_OK;
	HCHAR		szId[4];

	HxLOG_Debug("[%s] ulId : [%d]\n", __FUNCTION__, ulId);
	CHECK_NULL(pstInfo);

	HxSTD_snprintf(szId, 4, "%02d", ulId);
	HLIB_UDB_ClearStartNode(_dldbHandle);
	if (HLIB_UDB_SetStartNode(_dldbHandle, szId) != ERR_OK)
	{
		HxLOG_Error("Can't find id [%s]\n", szId);
		TRLEAVE(ERR_FAIL);
	}

	hErr |= HLIB_UDB_GetValue(_dldbHandle, TRPORT_DB_DOWNLOADID_STATE, &(pstInfo->state));
	hErr |= HLIB_UDB_GetText(_dldbHandle, TRPORT_DB_DOWNLOADID_FILEPATH, pstInfo->filepath, TRPORT_DB_PATH_LEN);
	hErr |= HLIB_UDB_GetText(_dldbHandle, TRPORT_DB_DOWNLOADID_FILENAME, pstInfo->filename, TRPORT_DB_FILE_LEN);
	hErr |= HLIB_UDB_GetValue(_dldbHandle, TRPORT_DB_DOWNLOADID_FILESIZE, &(pstInfo->filesize));

	if(hErr != ERR_OK)
	{
		HxLOG_Error("Can't find node in [%s]\n", szId);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

HERROR TRP_DB_StoreDownloadInfo(HUINT32 ulId, trport_download_info_t *stInfo)
{
	HCHAR	szBuffer[64];

	HLIB_UDB_ClearStartNode(_dldbHandle);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_STATE);
	HLIB_UDB_SetValue(_dldbHandle, szBuffer, stInfo->state);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_FILEPATH);
	HLIB_UDB_SetText(_dldbHandle, szBuffer, stInfo->filepath);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_FILENAME);
	HLIB_UDB_SetText(_dldbHandle, szBuffer, stInfo->filename);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_FILESIZE);
	HLIB_UDB_SetValue(_dldbHandle, szBuffer, stInfo->filesize);

	HLIB_UDB_Sync(_dldbHandle);
	TRLEAVE(ERR_OK);
}

void TRP_DB_SetResetFlag(HBOOL flag)
{
	HLIB_UDB_ClearStartNode(_dldbHandle);
	HLIB_UDB_SetValue(_dldbHandle, TRPORT_DB_DOWNLOADID_RESET, flag);
}

HERROR TRP_DB_GetResetFlag(HBOOL *flag)
{
	TRLEAVE(HLIB_UDB_GetValue(_dldbHandle, TRPORT_DB_DOWNLOADID_RESET, flag));
}

void TRP_DB_SetPrevVersion(HCHAR *pszVer)
{
	HLIB_UDB_ClearStartNode(_dldbHandle);
	HLIB_UDB_SetText(_dldbHandle, TRPORT_DB_DOWNLOADID_PREVIOUS_VERSION, pszVer);
}

HERROR TRP_DB_GetPrevVersion(HCHAR *pszVer)
{
	TRLEAVE(HLIB_UDB_GetText(_dldbHandle, TRPORT_DB_DOWNLOADID_PREVIOUS_VERSION, pszVer, 0));
}

#define __________GET_FUNCTION__________
HERROR	TRP_DB_getData_Device_DeviceSummary(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("DeviceSummary", pszOutput);
}

HERROR	TRP_DB_getData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("X_JCOM_SubscriberID", pszOutput);
}

HERROR	TRP_DB_getData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("X_JCOM_ServiceID", pszOutput);
}

HERROR	TRP_DB_getData_DeviceInfo_X_JCOM_CorpCode(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("X_JCOM_CorpCode", pszOutput);
}

HERROR	TRP_DB_getData_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("ManufacturerOUI", pszOutput);
}

HERROR	TRP_DB_getData_DeviceInfo_Description(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("Description", pszOutput);
}

HERROR	TRP_DB_getData_DeviceInfo_ProductClass(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("ProductClass", pszOutput);
}

HERROR TRP_DB_getData_DeviceInfo_HardwareVersion(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("HardwareVersion", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_Username(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("Username", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_Password(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("Password", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("PeriodicInformEnable", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("PeriodicInformInterval", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_PeriodicInformTime(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("PeriodicInformTime", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_ParameterKey(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("ParameterKey", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("ConnectionRequestURL", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("ConnectionRequestUsername", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("ConnectionRequestPassword", pszOutput);
}

HERROR TRP_DB_getData_ManagementServer_UpgradesManaged(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("UpgradesManaged", pszOutput);
}

HERROR TRP_DB_getData_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("CombinedAccess", pszOutput);
}

HERROR TRP_DB_getData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("PeriodicInformationInterval", pszOutput);
}

HERROR TRP_DB_getData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("PeriodicInformationTime", pszOutput);
}

HERROR TRP_DB_getData_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput)
{
	HxLOG_Assert(_trdbHandle);
	CHECK_NULL(pszOutput);
	return TRP_DB_getTextData("PermissionLevel", pszOutput);
}

#define __________SET_FUNCTION__________
HERROR TRP_DB_setData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("X_JCOM_SubscriberID", pszInput);
}

HERROR TRP_DB_setData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("X_JCOM_ServiceID", pszInput);
}

HERROR TRP_DB_setData_DeviceInfo_X_JCOM_CorpCode(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("X_JCOM_CorpCode", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_Username(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("Username", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_Password(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("Password", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_PeriodicInformEnable(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("PeriodicInformEnable", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_PeriodicInformInterval(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("PeriodicInformInterval", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_PeriodicInformTime(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("PeriodicInformTime", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("ConnectionRequestUsername", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("ConnectionRequestPassword", pszInput);
}

HERROR TRP_DB_setData_ManagementServer_UpgradesManaged(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("UpgradesManaged", pszInput);
}

HERROR TRP_DB_setData_AudienceMeasurement_CombinedAccess(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("CombinedAccess", pszInput);
}

HERROR TRP_DB_setData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("PeriodicInformationInterval", pszInput);
}

HERROR TRP_DB_setData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("PeriodicInformationTime", pszInput);
}

HERROR TRP_DB_setData_AudienceMeasurement_PermissionLevel(HCHAR *pszInput)
{
	HxLOG_Assert(_trdbHandle);
	return TRP_DB_setTextData("PermissionLevel", pszInput);
}









