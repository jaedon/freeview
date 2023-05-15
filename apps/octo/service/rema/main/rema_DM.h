/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  re_init.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
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



#ifndef	__rema_DM_H__
#define	__rema_DM_H__

#include "rema_cwmp.h"


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define STBCONFIG_FILE_NAME "var/lib/humaxtv/dlogger/"

#define LENGTH_OF_DM_TOKEN	64
#define TRPORT_DB_FILE_LEN 			256
#define TRPORT_DB_PATH_LEN 			256
#define TRPORT_DB_PATH				"/var/lib/humaxtv/tr069/"
#define TRPORT_TR_DB_FILENAME		"tr069.json"
#define TRPORT_DL_DB_FILENAME		"download.json"

#define TRPORT_DB_BACKUP_PATH		"/var/lib/humaxtv/tr069_backup/"

#define TRPORT_DB_DOWNLOADID_TYPE			"type"
#define TRPORT_DB_DOWNLOADID_STATE			"state"
#define TRPORT_DB_DOWNLOADID_FILEPATH		"filepath"
#define TRPORT_DB_DOWNLOADID_FILENAME		"filename"
#define TRPORT_DB_DOWNLOADID_FILESIZE		"size"
#define TRPORT_DB_DOWNLOADID_RESET			"resetFlag"
#define TRPORT_DB_DOWNLOADID_PREVIOUS_VERSION "prevVersion"

#define TRPORT_DB_TR069_UPGRADE_ACSURL		"UpgradeProductionServer"

#define DEFAULT_PREV_SW_VERSION			""
#define ENTRY_OFFSET	1

#define TRPORT_DB_FULLPATH_LEN 		TRPORT_DB_FILE_LEN+TRPORT_DB_PATH_LEN
#define RDM_IPADDR_LEN 		16


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eDOWNLOAD_STATE_STANDBY =0,
	eDOWNLOAD_STATE_DOWNLOADING,
	eDOWNLOAD_STATE_FINISH
}trport_download_state_e;



typedef struct trport_download_info
{
	trport_download_state_e	state;
	HCHAR					filepath[TRPORT_DB_PATH_LEN];
	HCHAR					filename[TRPORT_DB_FILE_LEN];
	HUINT32 				filesize;
} trport_download_info_t;



/*******************************************************************/
/********************      Public Functions     ********************/
/*****************************************************************/

/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/


extern 	HERROR rema_DM_Init(void);
extern 	void 	rema_DM_Deinit(void);

extern 	HERROR rema_DM_GetDownloadInfo(HUINT32 ulId, trport_download_info_t *pstInfo);
extern 	HERROR rema_DM_StoreDownloadInfo(HUINT32 ulId, trport_download_info_t *stIInfo);
extern 	void	rema_DM_SetResetFlag(HBOOL flag);
extern 	HERROR rema_DM_GetResetFlag(HBOOL *flag);
extern 	void 	rema_DM_SetPrevVersion(HCHAR *pszVer);
extern 	HERROR rema_DM_GetPrevVersion(HCHAR *pszVer);
extern 	HINT32	rema_DM_MultiInstance_GetEntry(eTRXDMInstancetype_e eId);
extern 	HERROR	rema_DM_MultiInstance_SetEntry(eTRXDMInstancetype_e eId, HINT32 nEntry);
extern 	HERROR	rema_DM_MultiInstance_GetEntryCount(eTRXDMInstancetype_e eId, HINT32 *pnEntryCount);
extern	HERROR	rema_DM_MultiInstance_SetEntryCount(eTRXDMInstancetype_e eId, HINT32 nEntryCount);
extern 	HERROR	rema_DM_MultiInstance_IncreaseEntryCount(eTRXDMInstancetype_e eId);
extern 	HERROR	rema_DM_MultiInstance_PrintEntryCount(void);

extern 	HERROR	rema_DM_getData_Device_DeviceSummary(HCHAR *pszOutput);
extern 	HERROR	rema_DM_getData_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput);
extern 	HERROR	rema_DM_getData_DeviceInfo_Description(HCHAR *pszOutput);
extern 	HERROR	rema_DM_getData_DeviceInfo_ProductClass(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_DeviceInfo_HardwareVersion(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_DeviceInfo_AdditionalHardwareVersion(HCHAR *pszOutput);
extern	HERROR	rema_DM_getData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput);
extern	HERROR	rema_DM_getData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput);
extern	HERROR	rema_DM_getData_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszOutput);
extern	HERROR	rema_DM_getData_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_Username(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_Password(HCHAR *pszOutput);
extern	HERROR rema_DM_getData_ManagementServer_Url(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_PeriodicInformTime(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_ParameterKey(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_ManagementServer_UpgradesManaged(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput);
extern 	HERROR rema_DM_getData_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput);

extern	HERROR rema_DM_setData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput);
extern	HERROR rema_DM_setData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput);
extern	HERROR rema_DM_setData_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszInput);
extern	HERROR rema_DM_setData_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_Username(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_Password(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_Url(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_PeriodicInformEnable(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_PeriodicInformInterval(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_PeriodicInformTime(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_ConnectionRequestURL(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_ManagementServer_UpgradesManaged(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_AudienceMeasurement_CombinedAccess(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput);
extern 	HERROR rema_DM_setData_AudienceMeasurement_PermissionLevel(HCHAR *pszInput);



extern HERROR TRP_Set_SoftwareModules_ExecEnv_Enable(HCHAR *pszInput);
extern HERROR TRP_Set_AMConfigurationName(HCHAR *pszInput);
extern HERROR TRP_Set_FirstUseDate(HCHAR *pszInput);
extern HERROR TRP_Set_ProvisioningCode(HCHAR *pszInput);


extern HERROR TRP_Get_STBConfigDownloadResult(HBOOL *pbResult);
extern HERROR TRP_Get_SoftwareModules_ExecEnv_Enable(HCHAR *pszOutput);
extern HERROR TRP_Get_AMConfigurationName(HCHAR *pszOutput);
extern HERROR TRP_Get_FirstUseDate(HCHAR *pszOutput);
extern HERROR TRP_Get_ProvisioningCode(HCHAR *pszOutput);


extern HERROR TRP135_Get_JCOM_Config_WebBrowserURLInput(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_WebBrowserURLInput(HCHAR *pszInput);


extern HERROR TRP157_Get_MgmtServer_DefaultActiveNotificationThrottle(HCHAR *pszOutput);
extern HERROR TRP157_Get_MgmtServer_CWMPRetryMinimumWaitInterval(HCHAR *pszOutput);
extern HERROR TRP157_Get_MgmtServer_CWMPRetryIntervalMultiplier(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_SupportedDataModelNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(HCHAR *pszOutput);
extern HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_Host(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_Timeout(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_DataBlockSize(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_DSCP(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_SuccessCount(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_FailureCount(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_AverageResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_Host(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_Timeout(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DataBlockSize(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_MaxHopCount(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DSCP(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_ResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_NumberOfRouteHops(HINT32 *pszOutput);
extern HERROR TRP157_Get_LAN_TraceRouteDiagnostics_RouteHops_M_HopHost(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_Interface(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_DownloadURL(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_DSCP(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_EthernetPriority(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_ROMTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_BOMTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_EOMTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_TestBytesReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_DownloadDiagnostics_TotalBytesReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_Interface(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_UploadURL(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_DSCP(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_EthernetPriority(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_TestFileLength(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_ROMTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_BOMTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_EOMTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_UploadDiagnostics_TotalBytesSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Interface(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_HostName(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_DNSServer(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Timeout(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_NumberOfRepetitions(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_SuccessCount(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_ResultNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Result_M_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Result_M_AnswerType(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Result_M_HostNameReturned(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Result_M_IPAddresses(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Result_M_DNSServerIP(HCHAR *pszOutput);
extern HERROR TRP157_Get_NSLookupDiagnostics_Result_M_ResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_Host(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_Timeout(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_DataBlockSize(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_SuccessCount(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_FailureCount(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_AverageResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszOutput);



extern HERROR TRP157_Set_MgmtServer_DefaultActiveNotificationThrottle(HCHAR *pszInput);
extern HERROR TRP157_Set_MgmtServer_CWMPRetryMinimumWaitInterval(HCHAR *pszInput);
extern HERROR TRP157_Set_MgmtServer_CWMPRetryIntervalMultiplier(HCHAR *pszInput);
extern HERROR TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(HCHAR *pszInput);
extern HERROR TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_Host(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_Timeout(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_DataBlockSize(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_DSCP(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_SuccessCount(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_FailureCount(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_AverageResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_Host(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_Timeout(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_DataBlockSize(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_MaxHopCount(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_DSCP(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_ResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_NumberOfRouteHops(HINT32 *pszInput);
extern HERROR TRP157_Set_LAN_TraceRouteDiagnostics_M_RouteHops_HopHost(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_Interface(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_DownloadURL(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_DSCP(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_EthernetPriority(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_ROMTime(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_BOMTime(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_EOMTime(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_TestBytesReceived(HCHAR *pszInput);
extern HERROR TRP157_Set_DownloadDiagnostics_TotalBytesReceived(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_Interface(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_UploadURL(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_DSCP(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_EthernetPriority(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_TestFileLength(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_ROMTime(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_BOMTime(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_EOMTime(HCHAR *pszInput);
extern HERROR TRP157_Set_UploadDiagnostics_TotalBytesSent(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Interface(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_HostName(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_DNSServer(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Timeout(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_NumberOfRepetitions(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_SuccessCount(HCHAR *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_ResultNumberOfEntries(HINT32 *pszInput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Result_M_Status(HCHAR *pszOutput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Result_M_AnswerType(HCHAR *pszOutput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Result_M_HostNameReturned(HCHAR *pszOutput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Result_M_IPAddresses(HCHAR *pszOutput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Result_M_DNSServerIP(HCHAR *pszOutput);
extern HERROR TRP157_Set_NSLookupDiagnostics_Result_M_ResponseTime(HCHAR *pszOutput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_Host(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_Timeout(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_DataBlockSize(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_SuccessCount(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_FailureCount(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_AverageResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszInput);
extern HERROR TRP157_Set_SelfTestDiagnostics_DiagnosticsState(HCHAR *pszInput);
extern HERROR TRP157_Set_DevInfo_SupportedDataModelNumberOfEntries(HINT32 *pszInput);
extern HERROR TRP157_Set_DevInfo_SupportedDataModel_M_URL(HCHAR *pszInput);
extern HERROR TRP157_Set_DevInfo_SupportedDataModel_M_URN(HCHAR *pszInput);
extern HERROR TRP157_Set_DevInfo_SupportedDataModel_M_Features(HCHAR *pszInput);



#ifdef __cplusplus
}
#endif

#endif
