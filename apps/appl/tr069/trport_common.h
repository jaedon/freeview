
#include <hlib.h>
#include <vkernel.h>

#ifndef _TR069_COMMON_H_
#define _TR069_COMMON_H_

#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define	TRENTER				HxLOG_Info("[%s():%d] Enter!!\n",__FUNCTION__,__LINE__)
#define	TRLEAVE(x)			HxLOG_Info("[%s():%d] Leave!!\n",__FUNCTION__,__LINE__);return x
#define	Critical(y)			HxLOG_Critical("[%s:%d]:%s",__FUNCTION__, __LINE__,y);
#else /* defined(CONFIG_DEBUG) */
#define	LOG					HxLog_NullPrint
#define	TRENTER				HxLog_NullPrint
#define	TRLEAVE(x)			HxLog_NullPrint;return x
#define	Critical(y)			HxLog_NullPrint
#endif /* defined(CONFIG_DEBUG) */

#if CONFIG_DEBUG
#define PRINT_JSON(json) \
	{	HCHAR *pString;	pString = HLIB_JSON_Encode(json, NULL, 0);	HxLOG_Debug("json:[%s]\n",pString);	HLIB_STD_MemFree(pString);}
#else
#define PRINT_JSON(json)
#endif


#define	CHECK_NULL(a)		{if (a == NULL) {Critical("Null Pointer\n"); TRLEAVE(ERR_FAIL);}}


#define TRPORT_HUMAX_OUI		"000378"
#define TRPORT_IPADDR_LEN 		16
#define TRPORT_MACADDR_LEN		20

#define TRPORT_DB_FILE_LEN 			256
#define TRPORT_DB_PATH_LEN 			256
#define TRPORT_DB_FULLPATH_LEN 		TRPORT_DB_FILE_LEN+TRPORT_DB_PATH_LEN

#define TRPORT_DB_PATH				"/var/lib/humaxtv/tr069/"
#define TRPORT_TMP_CONFIG_PATH		"/tmp/"
#define DEFAULT_PREV_SW_VERSION			""

#define TRPORT_SOFTWARE_VERSION_PREFIX		6

typedef enum
{
	TRPORT_OBJECT_PATH = 0,
	TRPORT_PARAMETER_PATH
}trportPathInfo_t;


typedef HERROR (*pfn_getCB_t)(void* output);
typedef HERROR (*pfn_setCB_t)(void* input);

typedef struct TRPORT_Object_Format_t
{
	HCHAR						*ElementName;
	HxJSON_Type_e				ElementType;
	HUINT32						ulen;
	pfn_getCB_t 				pfnCbGetData;
	pfn_setCB_t 				pfnCbSetData;
	struct TRPORT_Object_Format_t *s_objectform;
}TRPORT_OBJECT_FORMAT_t;

typedef enum
{
	//eDOWNLOAD_TYPE_UNDIFINED = -1,

	eDOWNLOAD_TYPE_FIRMWARE = 0,
	eDOWNLOAD_TYPE_WEBCONTENT,
	eDOWNLOAD_TYPE_VENDOR_CFG,
	eDOWNLOAD_TYPE_TONE,
	eDOWNLOAD_TYPE_RINGER,
	eDOWNLOAD_TYPE_X_JCOM_CFG, 					// 5
	eDOWNLOAD_TYPE_X_JLABS_AM_CFG, 				// 6
	eDOWNLOAD_TYPE_X_JCOM_PrivateMessage,  // 7

	eDOWNLOAD_TYPE_MAX
}trport_download_type_e;

typedef enum
{
	eDOWNLOAD_STATE_STANDBY =0,
	eDOWNLOAD_STATE_DOWNLOADING,
	eDOWNLOAD_STATE_FINISH
}trport_download_state_e;

typedef enum
{
	eFLAG_NEED_TO_RESET =0,
	eFLAG_NO_NEED_TO_RESET
}trport_reset_flag_e;

typedef struct trport_download_info
{
	trport_download_state_e	state;
	HCHAR					filepath[TRPORT_DB_PATH_LEN];
	HCHAR					filename[TRPORT_DB_FILE_LEN];
	HUINT32 				filesize;
} trport_download_info_t;



/*
* trport.c
*/
extern 	void 	TRP_TEST_PrintAllObject(void);
#ifdef CONFIG_DEBUG
extern 	void 	TRP_TEST_PrintAllDownloadInfor(void);
extern 	HERROR TRP_TEST_CreateDeviceJson(void);
extern 	HERROR TRP_TEST_DestroyDeviceJson(void);
#endif
extern 	HERROR TRP_SetParam(char* input);
extern 	HCHAR* TRP_GetParam(HCHAR *key);
extern 	HERROR TRP_ApplyFirmware(HUINT32 ulId);
extern 	HERROR TRP_CheckFirmware(HUINT32 ulId);
extern 	HERROR TRP_ApplySTBconfig(HUINT32 ulId);

extern 	HERROR TRP_GetDownloadInfo(trport_download_type_e type, trport_download_info_t **pstInfo);
extern 	HERROR TRP_GetDownloadPath(trport_download_type_e type , HCHAR* filename, HUINT32 filesize, HCHAR **pszfullpath, HCHAR **pszid);
extern 	HERROR TRP_LoadDownloadInfosFromDB(void);

extern 	void	TRP_EVENT_Reboot(void);
extern 	void	TRP_EVENT_FactoryReset(void);

/*
* trport_db.c
*/
extern 	HERROR TRP_DB_Init(void);
extern 	void 	TRP_DB_Deinit(void);

extern 	HERROR TRP_DB_GetDownloadInfo(HUINT32 ulId, trport_download_info_t *pstInfo);
extern 	HERROR TRP_DB_StoreDownloadInfo(HUINT32 ulId, trport_download_info_t *stIInfo);
extern 	void 	TRP_DB_SetResetFlag(HBOOL flag);
extern 	HERROR TRP_DB_GetResetFlag(HBOOL *flag);
extern 	void 	TRP_DB_SetPrevVersion(HCHAR *pszVer);
extern 	HERROR TRP_DB_GetPrevVersion(HCHAR *pszVer);

extern 	HERROR	TRP_DB_getData_Device_DeviceSummary(HCHAR *pszOutput);
extern 	HERROR	TRP_DB_getData_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput);
extern 	HERROR	TRP_DB_getData_DeviceInfo_Description(HCHAR *pszOutput);
extern 	HERROR	TRP_DB_getData_DeviceInfo_ProductClass(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_DeviceInfo_HardwareVersion(HCHAR *pszOutput);
extern	HERROR	TRP_DB_getData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput);
extern	HERROR	TRP_DB_getData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput);
extern	HERROR	TRP_DB_getData_DeviceInfo_X_JCOM_CorpCode(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_Username(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_Password(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_PeriodicInformTime(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_ParameterKey(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_ManagementServer_UpgradesManaged(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput);
extern 	HERROR TRP_DB_getData_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput);

extern	HERROR TRP_DB_setData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput);
extern	HERROR TRP_DB_setData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput);
extern	HERROR TRP_DB_setData_DeviceInfo_X_JCOM_CorpCode(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_Username(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_Password(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_PeriodicInformEnable(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_PeriodicInformInterval(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_PeriodicInformTime(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_ManagementServer_UpgradesManaged(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_AudienceMeasurement_CombinedAccess(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput);
extern 	HERROR TRP_DB_setData_AudienceMeasurement_PermissionLevel(HCHAR *pszInput);


/*
* trport_draw.c
*/
extern 	void	TRP_GFX_CreateWindow(void);
extern	void	TRP_GFX_DestroyWindow(void);
extern 	void	TRP_GFX_DrawRebootDialog(void);
extern	void 	TRP_GFX_ShowMsg(void);

/*
* trport_flash.c
*/
extern 	HERROR TRP_FLASH_Open(void);
extern 	HERROR TRP_FLASH_Close(void);
extern 	unsigned int TRP_FLASH_Write(const void *buf, size_t nbyte);

#endif//#ifndef _TR069_COMMON_H_
