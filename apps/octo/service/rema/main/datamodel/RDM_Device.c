/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


#include "rema_cwmp.h"
#include "RDM_Dvc_DeviceInfo.h"
#include "RDM_Dvc_ManagementServer.h"


#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___

#if 0
static hCwmpResult_e get_Com_Dummy(HCWMP_GETTER_PARAM)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}


static hCwmpResult_e set_Com_Dummy(HCWMP_SETTER_PARAM)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}
static  hCwmpResult_e update_Com_Dummy(HCWMP_UPDATER_PARAM)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}

static  hCwmpResult_e Adder_Com_Dummy(HCWMP_ADDER_PARAM)
{
	REM_FUNC_ENTER;
	RemLOG_Debug("%s\n", pszObjectName);
	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}
static  hCwmpResult_e Deleter_Com_Dummy(HCWMP_DELETER_PARAM)
{
	REM_FUNC_ENTER;
	RemLOG_Debug("%s\n", pszObjectName);
	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}
#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___


HBOOL RDM_Device_Init(void)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return TRUE;
}

HBOOL RDM_Device_Reister(hCwmpDMManager_t *pstDMManager)
{
	//hcwmp_engine internal
	//추후 변경 예정
	REM_FUNC_ENTER;

	
	printf("\n test test=========\n");
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceSummary",
		ehCwmp_PTYPE_STRING, NULL,
		get_Com_Dummy, 
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);

	
	//rema_cwmp_AddObject(pstDMManager, 
	//	"Device.Services", NULL, NULL, NULL, NULL, NULL);
	
	rema_cwmp_AddObject(pstDMManager, 
		"Device.DeviceInfo", NULL, update_Com_Dummy, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.Manufacturer",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_Manufacturer, 
		NULL, 
		ehCwmp_NFB_NON_VOLATILE, false);
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ManufacturerOUI",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_ManufacturerOUI, 
		NULL, 
		ehCwmp_NFB_NON_VOLATILE, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ModelName",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_ModelName,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.Description",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_Description,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
	/*
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProductClass",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_ProductClass,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
		*/
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.SerialNumber",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_SerialNumber,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.HardwareVersion",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_HardwareVersion,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.SoftwareVersion",
		ehCwmp_PTYPE_STRING,
		NULL,
		get_RDM_Dvc_DvcInf_SoftwareVersion,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, true);
	/*
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.AdditionalSoftwareVersion",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_AdditionalSoftwareVersion,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
		*/
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.DeviceStatus",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_DeviceStatus,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.UpTime",
		ehCwmp_PTYPE_UINT, NULL,
		get_RDM_Dvc_DvcInf_UpTime,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
#if 0

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.CPUUsage",
		ehCwmp_PTYPE_UINT, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_CPUUsage,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);


	rema_cwmp_AddObject(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process", 
		NULL, 
		DMO_Update_DeviceInfo_ProcessStatus_Process,
		NULL, 
		NULL, 
		"Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries");

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process.{i}.PID",
		ehCwmp_PTYPE_UINT, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_Process_M_PID,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process.{i}.Command",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_Process_M_Command,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process.{i}.Size",
		ehCwmp_PTYPE_UINT, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_Process_M_Size,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process.{i}.Priority",
		ehCwmp_PTYPE_UINT, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_Process_M_Priority,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process.{i}.CPUTime",
		ehCwmp_PTYPE_UINT, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_Process_M_CPUTime,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.DeviceInfo.ProcessStatus.Process.{i}.State",
		ehCwmp_PTYPE_STRING, NULL,
		get_RDM_Dvc_DvcInf_ProcessStatus_Process_M_State,
		NULL,
		ehCwmp_NFB_NON_VOLATILE, false);
#endif 

//hcwmp_engine internal
#if 0
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.URL");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.Username");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.Password");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.PeriodicInformEnable");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.PeriodicInformInterval");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.PeriodicInformTime");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.ParameterKey");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.ConnectionRequestURL");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.ConnectionRequestUsername");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.ConnectionRequestPassword");
#endif
#if 1

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.ManagementServer.UpgradesManaged",
		ehCwmp_PTYPE_BOOL, NULL,
		get_RDM_Dvc_MS_UpgradesManaged,
		set_RDM_Dvc_MS_UpgradesManaged,
		ehCwmp_NFB_NON_VOLATILE, false);
#endif
//hcwmp_engine internal
#if 0
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.DefaultActiveNotificationThrottle");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.CWMPRetryMinimumWaitInterval");
	rema_cwmp_AddParameter(pstDMManager, "Device.ManagementServer.CWMPRetryIntervalMultiplier");
#endif
	

	REM_FUNC_LEAVE;

	return TRUE;
}

void RDM_Device_Deinit(void)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;
}
#endif
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

