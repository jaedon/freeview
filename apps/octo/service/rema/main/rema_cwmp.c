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
#include "rema_DM.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define TRPORT_DATAMODEL_ROOT_OBJECT 		"Device"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.MemoryStatus.
/*
	Total	unsignedInt
	Free	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_MemoryStatus_ObjectForm[] =
{
	{"Total",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_MemoryStatus_Total, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Free",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_MemoryStatus_Free, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.ProcessStatus.Process.{i}.
/*
	PID	unsignedInt
	Command	string(256)
	Size	unsignedInt
	Priority	unsignedInt[:99]
	CPUTime	unsignedInt
	State	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_ProcessStatus_Process_ObjectForm[] =
{
	{"PID",	eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_Process_M_PID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Command",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_Process_M_Command, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Size", 		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_Process_M_Size, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Priority", 		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_Process_M_Priority, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"CPUTime", 		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_Process_M_CPUTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"State", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_Process_M_State, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.ProcessStatus.
/*
	CPUUsage	unsignedInt[:100]
	ProcessNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_ProcessStatus_ObjectForm[] =
{
	{"CPUUsage",	eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_CPUUsage, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProcessNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_ProcessNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Process",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_DevInfo_ProcessStatus_ProcessNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_ProcessStatus_Process_ObjectForm, TRDMType_S, eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.TemperatureStatus.TemperatureSensor.{i}.
/*
	Value		Int
	MaxValue		Int
*/
const static TRPORT_OBJECT_FORMAT_t	s_TemperatureStatus_TemperatureSensor_ObjectForm[] =
{
	{"Value",		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensor_M_Value, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaxValue",	eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensor_M_MaxValue, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.TemperatureStatus.
/*
	TemperatureSensorNumberOfEntries	unsignedInt
	TemperatureSensor				Array
*/
const static TRPORT_OBJECT_FORMAT_t	s_TemperatureStatus_ObjectForm[] =
{
	{"TemperatureSensorNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensorNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TemperatureSensor",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensorNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_TemperatureStatus_TemperatureSensor_ObjectForm, TRDMType_S, eTR157A3_Device_DeviceInfo_TemperatureStatus_TemperatureSensorNumberOfEntries},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.SupportedDataModel.{i}.
// CPE
/*
	URL	string(256)
	URN	string(256)
	Features	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_SupportedDataModel_ObjectForm[] =
{
	{"URL",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_SupportedDataModel_M_URL, (pfn_setCB_t)TRP157_Set_DevInfo_SupportedDataModel_M_URL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"URN",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_SupportedDataModel_M_URN, (pfn_setCB_t)TRP157_Set_DevInfo_SupportedDataModel_M_URN, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Features", 		eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_SupportedDataModel_M_Features, (pfn_setCB_t)TRP157_Set_DevInfo_SupportedDataModel_M_Features, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.VendorConfigFile.{i}.
/*
	Name	string(64)
	Version	string(16)
	Date	dateTime
	Description	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_VendorConfigFile_ObjectForm[] =
{
	{"Name",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorConfigFile_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Version",	eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorConfigFile_M_Version, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Date", 		eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorConfigFile_M_Date, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Description", 		eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorConfigFile_M_Description, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.Processor.{i}.
/*
	Architecture	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Processor_ObjectForm[] =
{
	{"Architecture",	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP157_Get_DevInfo_Processor_M_Architecture, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.VendorLogFile.{i}.
/*
	Name	string(64)
	MaximumSize	unsignedInt
	Persistent	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_VendorLogFile_ObjectForm[] =
{
	{"Name",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorLogFile_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaximumSize",	eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorLogFile_M_MaximumSize, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Persistent", 		eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorLogFile_M_Persistent, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DeviceInfo.
/*
	Manufacturer	string(64)
	ManufacturerOUI	string(6)
	ModelName	string(64)
	Description	string(256)
	ProductClass	string(64)
	SerialNumber	string(64)
	VendorConfigFileNumberOfEntries	unsignedInt
	HardwareVersion	string(64)
	AdditionalHardwareVersion	string(64)
	SoftwareVersion	string(64)
	AdditionalSoftwareVersion	string(64)
	ProvisioningCode	string(64)	W
	DeviceStatus	string
	UpTime	unsignedInt
	FirstUseDate	dateTime
	DeviceLog	string(32768)
	SupportedDataModelNumberOfEntries	unsignedInt
	ProcessorNumberOfEntries	unsignedInt
	VendorLogFileNumberOfEntries	unsignedInt
	X_JCOM_SubscriberID	string(256)	W
	X_JCOM_ServiceID	string(256)	W
	X_JCOM_FamilyID	string(256)	W
	X_JCOM_BusinessCode	string(256)	W
*/

const static TRPORT_OBJECT_FORMAT_t	s_DeviceInfo_ObjectForm[] =
{
	{"Manufacturer",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_DeviceInfo_Manufacturer, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ManufacturerOUI",	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP_Get_DeviceInfo_ManufacturerOUI, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ModelName", 		eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_DeviceInfo_ModelName, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Description", 	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_DeviceInfo_Description, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProductClass", 	eHLIB_JSON_String, SIZE_128,  (pfn_getCB_t)TRP_Get_DeviceInfo_ProductClass, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SerialNumber", 	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_DeviceInfo_SerialNumber, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VendorConfigFileNumberOfEntries", 	eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorConfigFileNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HardwareVersion",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_DeviceInfo_HardwareVersion, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AdditionalHardwareVersion", eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_DeviceInfo_AdditionalHardwareVersion, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SoftwareVersion",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_DeviceInfo_SoftwareVersion, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AdditionalSoftwareVersion",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP157_Get_DevInfo_AdditionalSoftwareVersion, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProvisioningCode",	eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TRP_Get_ProvisioningCode, (pfn_setCB_t)TRP_Set_ProvisioningCode, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DeviceStatus", 	eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TRP_Get_DeviceInfo_DeviceStatus, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UpTime",	 		eHLIB_JSON_Integer, SIZE_32,  (pfn_getCB_t)TRP_Get_DeviceInfo_UpTime, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"FirstUseDate",	 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP_Get_DeviceInfo_FirstUseDate, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DeviceLog",	 		eHLIB_JSON_String, SIZE_1024,  (pfn_getCB_t)TRP157_Get_DevInfo_DeviceLog, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SupportedDataModelNumberOfEntries",	 		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_SupportedDataModelNumberOfEntries, (pfn_setCB_t)TRP157_Set_DevInfo_SupportedDataModelNumberOfEntries, NULL, TRDMType_S, eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries},
	{"ProcessorNumberOfEntries",	 		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_ProcessorNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VendorLogFileNumberOfEntries",	 		eHLIB_JSON_Integer, SIZE_16,  (pfn_getCB_t)TRP157_Get_DevInfo_VendorLogFileNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_SubscriberID",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP_Get_DeviceInfo_X_JCOM_SubscriberID, (pfn_setCB_t)TRP_SET_DeviceInfo_X_JCOM_SubscriberID, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_ServiceID", eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP_Get_DeviceInfo_X_JCOM_ServiceID, (pfn_setCB_t)TRP_SET_DeviceInfo_X_JCOM_ServiceID, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_FamilyID",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP_Get_DeviceInfo_X_JCOM_FamilyID, (pfn_setCB_t)TRP_SET_DeviceInfo_X_JCOM_FamilyID, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_BusinessCode",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP_Get_DeviceInfo_X_JCOM_BusinessCode, (pfn_setCB_t)TRP_SET_DeviceInfo_X_JCOM_BusinessCode, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MemoryStatus",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_MemoryStatus_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"ProcessStatus",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_ProcessStatus_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"TemperatureStatus",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_TemperatureStatus_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"SupportedDataModel",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_DevInfo_SupportedDataModelNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SupportedDataModel_ObjectForm, TRDMType_S, eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries},
	{"VendorConfigFile",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_DevInfo_VendorConfigFileNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_VendorConfigFile_ObjectForm, TRDMType_S, eTR157A3_Device_DeviceInfo_VendorConfigFileNumberOfEntries},
	{"Processor",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_DevInfo_ProcessorNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Processor_ObjectForm, TRDMType_S, eTR157A3_Device_DeviceInfo_ProcessorNumberOfEntries},
	{"VendorLogFile",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_DevInfo_VendorLogFileNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_VendorLogFile_ObjectForm, TRDMType_S, eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.ManagementServer.AutonomousTransferCompletePolicy.
/*
	Enable	boolean
	TransferTypeFilter	string
	FileTypeFilter	string(1024)
*/
//CPE
const static TRPORT_OBJECT_FORMAT_t	s_ManagementServer_AutonomousTransferCompletePolicy_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_Enable, (pfn_setCB_t)TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TransferTypeFilter",	eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter, (pfn_setCB_t)TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter, NULL, TRDMType_S, eTRXDMInstance_None},
	{"FileTypeFilter", 		eHLIB_JSON_String, SIZE_1024,  (pfn_getCB_t)TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter, (pfn_setCB_t)TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.ManagementServer.
/*
	URL	string(256)	W
	Username	string(256)	W
	Password	string(256)	W
	PeriodicInformEnable	boolean	W
	PeriodicInformInterval	unsignedInt[1:]	W
	PeriodicInformTime	dateTime	W
	ParameterKey	string(32)
	ConnectionRequestURL	string(256)
	ConnectionRequestUsername	string(256)	W
	ConnectionRequestPassword	string(256)	W
	UpgradesManaged	boolean	W
	DefaultActiveNotificationThrottle	unsignedInt	W
	CWMPRetryMinimumWaitInterval	unsignedInt[1:65535]	W
	CWMPRetryIntervalMultiplier	unsignedInt[1000:65535]	W


*/
const static TRPORT_OBJECT_FORMAT_t	s_ManagmentServer_ObjectForm[] =
{
	{"URL", 					eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_ManagementServer_URL, (pfn_setCB_t)TRP_SET_ManagementServer_URL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Username", 				eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_ManagementServer_Username, (pfn_setCB_t)TRP_SET_ManagementServer_Username, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Password", 				eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_ManagementServer_Password, (pfn_setCB_t)TRP_SET_ManagementServer_Password, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PeriodicInformEnable", 	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP_Get_ManagementServer_PeriodicInformEnable, (pfn_setCB_t)TRP_SET_ManagementServer_PeriodicInformEnable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PeriodicInformInterval",	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP_Get_ManagementServer_PeriodicInformInterval, (pfn_setCB_t)TRP_SET_ManagementServer_PeriodicInformInterval, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PeriodicInformTime", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP_Get_ManagementServer_PeriodicInformTime, (pfn_setCB_t)TRP_SET_ManagementServer_PeriodicInformTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ParameterKey", 			eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TRP_Get_ManagementServer_ParameterKey, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ConnectionRequestURL", 	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_ManagementServer_ConnectionRequestURL, (pfn_setCB_t)TRP_SET_ManagementServer_ConnectionRequestURL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ConnectionRequestUsername", eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_ManagementServer_ConnectionRequestUsername, (pfn_setCB_t)TRP_SET_ManagementServer_ConnectionRequestUsername, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ConnectionRequestPassword",  eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TRP_Get_ManagementServer_ConnectionRequestPassword, (pfn_setCB_t)TRP_SET_ManagementServer_ConnectionRequestPassword, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UpgradesManaged", 		eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TRP_Get_ManagementServer_UpgradesManaged, (pfn_setCB_t)TRP_SET_ManagementServer_UpgradesManaged, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DefaultActiveNotificationThrottle",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_MgmtServer_DefaultActiveNotificationThrottle, (pfn_setCB_t)TRP157_Set_MgmtServer_DefaultActiveNotificationThrottle, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CWMPRetryMinimumWaitInterval",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_MgmtServer_CWMPRetryMinimumWaitInterval, (pfn_setCB_t)TRP157_Set_MgmtServer_CWMPRetryMinimumWaitInterval, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CWMPRetryIntervalMultiplier",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_MgmtServer_CWMPRetryIntervalMultiplier, (pfn_setCB_t)TRP157_Set_MgmtServer_CWMPRetryIntervalMultiplier, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AutonomousTransferCompletePolicy",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_ManagementServer_AutonomousTransferCompletePolicy_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, 						eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.LAN.IPPingDiagnostics.
/*
	DiagnosticsState	string
	Host	string(256)
	NumberOfRepetitions	unsignedInt[1:]
	Timeout	unsignedInt[1:]
	DataBlockSize	unsignedInt[1:65535]
	DSCP	unsignedInt[0:63]
	SuccessCount	unsignedInt
	FailureCount	unsignedInt
	AverageResponseTime	unsignedInt
	MinimumResponseTime	unsignedInt
	MaximumResponseTime	unsignedInt
*/
//CPE
const static TRPORT_OBJECT_FORMAT_t	s_LAN_IPPingDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Host",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_Host, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_Host, NULL, TRDMType_S, eTRXDMInstance_None},
	{"NumberOfRepetitions",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_NumberOfRepetitions, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_NumberOfRepetitions, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Timeout",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_Timeout, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_Timeout, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DataBlockSize",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_DataBlockSize, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_DataBlockSize, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DSCP",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_DSCP, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_DSCP, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SuccessCount",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_SuccessCount, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_SuccessCount, NULL, TRDMType_S, eTRXDMInstance_None},
	{"FailureCount",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_FailureCount, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_FailureCount, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AverageResponseTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_AverageResponseTime, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_AverageResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MinimumResponseTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_MinimumResponseTime, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_MinimumResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaximumResponseTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_IPPingDiagnostics_MaximumResponseTime, (pfn_setCB_t)TRP157_Set_LAN_IPPingDiagnostics_MaximumResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.LAN.TraceRouteDiagnostics.RouteHops.{i}.
/*
	HopHost	string(256)
*/
//CPE
const static TRPORT_OBJECT_FORMAT_t	s_TraceRouteDiagnostics_RouteHops_ObjectForm[] =
{
	{"HopHost",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_RouteHops_M_HopHost, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_M_RouteHops_HopHost, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.LAN.TraceRouteDiagnostics.
/*
	DiagnosticsState	string	W
	Host	string(256)	W
	Timeout	unsignedInt[1:]	W
	DataBlockSize	unsignedInt[1:65535]	W
	MaxHopCount	unsignedInt[1:64]	W
	DSCP	unsignedInt[0:63]	W
	ResponseTime	unsignedInt
	NumberOfRouteHops	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_LAN_TraceRouteDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Host",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_Host, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_Host, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Timeout",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_Timeout, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_Timeout, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DataBlockSize",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_DataBlockSize, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_DataBlockSize, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxHopCount",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_MaxHopCount, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_MaxHopCount, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DSCP",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_DSCP, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_DSCP, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ResponseTime",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_ResponseTime, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_ResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"NumberOfRouteHops",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_NumberOfRouteHops, (pfn_setCB_t)TRP157_Set_LAN_TraceRouteDiagnostics_NumberOfRouteHops, NULL, TRDMType_S, eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops},
	{"RouteHops",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_LAN_TraceRouteDiagnostics_NumberOfRouteHops, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_TraceRouteDiagnostics_RouteHops_ObjectForm, TRDMType_S, eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.LAN.Stats.
/*
	ConnectionUpTime	unsignedInt
	TotalBytesSent	unsignedInt
	TotalBytesReceived	unsignedInt
	TotalPacketsSent	unsignedInt
	TotalPacketsReceived	unsignedInt
	CurrentDayInterval	unsignedInt
	CurrentDayBytesSent	unsignedInt
	CurrentDayBytesReceived	unsignedInt
	CurrentDayPacketsSent	unsignedInt
	CurrentDayPacketsReceived	unsignedInt
	QuarterHourInterval	unsignedInt
	QuarterHourBytesSent	unsignedInt
	QuarterHourBytesReceived	unsignedInt
	QuarterHourPacketsSent	unsignedInt
	QuarterHourPacketsReceived	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_LAN_Stats_ObjectForm[] =
{
	{"ConnectionUpTime",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_ConnectionUpTime, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalBytesSent", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_TotalBytesSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalBytesReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_TotalBytesReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalPacketsSent",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_TotalPacketsSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalPacketsReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_TotalPacketsReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CurrentDayInterval",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_CurrentDayInterval, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CurrentDayBytesSent", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_CurrentDayBytesSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CurrentDayBytesReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_CurrentDayBytesReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CurrentDayPacketsSent",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_CurrentDayPacketsSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CurrentDayPacketsReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_CurrentDayPacketsReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"QuarterHourInterval",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_QuarterHourInterval, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"QuarterHourBytesSent", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_QuarterHourBytesSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"QuarterHourBytesReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_QuarterHourBytesReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"QuarterHourPacketsSent",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_QuarterHourPacketsSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"QuarterHourPacketsReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_LAN_Stats_QuarterHourPacketsReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.LAN.
/*
	AddressingType	string	W
	IPAddress	string(45)	W
	SubnetMask	string(45)	W
	DefaultGateway	string	W
	DNSServers	string(256)	W
	MACAddress	string(17)	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_LAN_ObjectForm[] =
{
	{"AddressingType",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP_Get_LAN_AddressingType, (pfn_setCB_t)TRP_SET_LAN_AddressingType, NULL, TRDMType_S, eTRXDMInstance_None},
	{"IPAddress", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP_Get_LAN_IPAddress, (pfn_setCB_t)TRP_SET_LAN_IPAddress, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SubnetMask", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP_Get_LAN_SubnetMask, (pfn_setCB_t)TRP_SET_LAN_SubnetMask, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DefaultGateway",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP_Get_LAN_DefaultGateway, (pfn_setCB_t)TRP_SET_LAN_DefaultGateway, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DNSServers", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP_Get_LAN_DNSServers, (pfn_setCB_t)TRP_SET_LAN_DNSServers, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MACAddress", 		eHLIB_JSON_String, TRPORT_MACADDR_LEN, (pfn_getCB_t)TRP_Get_LAN_MACAddress, (pfn_setCB_t)TRP_SET_LAN_MACAddress, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Stats",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_LAN_Stats_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"IPPingDiagnostics",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_LAN_IPPingDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"TraceRouteDiagnostics",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_LAN_TraceRouteDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},


	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//Device.DownloadDiagnostics. // DB만 관리
/*
	DiagnosticsState	string	W
	Interface	string(256)	W
	DownloadURL	string(256)	W
	DSCP	unsignedInt[0:63]	W
	EthernetPriority	unsignedInt[0:7]	W
	ROMTime	dateTime
	BOMTime	dateTime
	EOMTime	dateTime
	TestBytesReceived	unsignedInt
	TotalBytesReceived	unsignedInt
*/
//CPE
const static TRPORT_OBJECT_FORMAT_t	s_DownloadDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Interface", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_Interface, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_Interface, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DownloadURL", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_DownloadURL, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_DownloadURL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DSCP",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_DSCP, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_DSCP, NULL, TRDMType_S, eTRXDMInstance_None},
	{"EthernetPriority", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_EthernetPriority, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_EthernetPriority, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ROMTime", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_ROMTime, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_ROMTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"BOMTime", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_BOMTime, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_BOMTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"EOMTime", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_EOMTime, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_EOMTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TestBytesReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_TestBytesReceived, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_TestBytesReceived, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalBytesReceived", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_DownloadDiagnostics_TotalBytesReceived, (pfn_setCB_t)TRP157_Set_DownloadDiagnostics_TotalBytesReceived, NULL, TRDMType_S, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Device.UploadDiagnostics.	// DB만 관리
/*
	DiagnosticsState	string	W
	Interface	string(256)	W
	UploadURL	string(256)	W
	DSCP	unsignedInt[0:63]	W
	EthernetPriority	unsignedInt[0:7]	W
	TestFileLength	unsignedInt	W
	ROMTime	dateTime
	BOMTime	dateTime
	EOMTime	dateTime
	TotalBytesSent	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_UploadDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Interface", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_Interface, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_Interface, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UploadURL", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_UploadURL, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_UploadURL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DSCP",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_DSCP, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_DSCP, NULL, TRDMType_S, eTRXDMInstance_None},
	{"EthernetPriority", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_EthernetPriority, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_EthernetPriority, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TestFileLength", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_TestFileLength, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_TestFileLength, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ROMTime", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_TestFileLength, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_ROMTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"BOMTime", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_TestFileLength, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_BOMTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"EOMTime", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_TestFileLength, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_EOMTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalBytesSent", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UploadDiagnostics_TestFileLength, (pfn_setCB_t)TRP157_Set_UploadDiagnostics_TotalBytesSent, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.UPnP.Device.Capabilities.	object
/*
UPnPArchitecture	unsignedInt
UPnPArchitectureMinorVer	unsignedInt
UPnPMediaServer	unsignedInt
UPnPMediaRenderer	unsignedInt
UPnPWLANAccessPoint	unsignedInt
UPnPBasicDevice	unsignedInt
UPnPQoSDevice	unsignedInt
UPnPQoSPolicyHolder	unsignedInt
UPnPIGD	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_Device_Capabilities_ObjectForm[] =
{
	{"UPnPArchitecture", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPArchitecture, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPArchitectureMinorVer", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPArchitectureMinorVer, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPMediaServer", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPMediaServer, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPMediaRenderer", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPMediaRenderer, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPWLANAccessPoint", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPWLANAccessPoint, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPBasicDevice", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPBasicDevice, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPQoSDevice", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPQoSDevice, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPQoSPolicyHolder", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPQoSPolicyHolder, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPIGD", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Device_Capa_UPnPIGD, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.UPnP.Device.
/*
	Enable	boolean	W
	UPnPMediaServer	boolean	W
	UPnPMediaRenderer	boolean	W
	UPnPWLANAccessPoint	boolean	W
	UPnPQoSDevice 	boolean	W
	UPnPQoSPolicyHolder	boolean	W
	UPnPIGD	boolean	W

*/
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_Device_ObjectForm[] =
{
	{"Enable", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_Enable, (pfn_setCB_t)TRP157_Set_UPnP_Device_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPMediaServer", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_UPnPMediaServer, (pfn_setCB_t)TRP157_Set_UPnP_Device_UPnPMediaServer, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPMediaRenderer", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_UPnPMediaRenderer, (pfn_setCB_t)TRP157_Set_UPnP_Device_UPnPMediaRenderer, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPWLANAccessPoint", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_UPnPWLANAccessPoint, (pfn_setCB_t)TRP157_Set_UPnP_Device_UPnPWLANAccessPoint, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPQoSDevice", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_UPnPQoSDevice, (pfn_setCB_t)TRP157_Set_UPnP_Device_UPnPQoSDevice, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPQoSPolicyHolder", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_UPnPQoSPolicyHolder, (pfn_setCB_t)TRP157_Set_UPnP_Device_UPnPQoSPolicyHolder, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UPnPIGD", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Device_UPnPIGD, (pfn_setCB_t)TRP157_Set_UPnP_Device_UPnPIGD, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Capabilities",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_Device_Capabilities_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.UPnP.Discovery.Device.{i}.
/*
	Status	string
	UUID	string(36)
	USN	string(256)
	LeaseTime	unsignedInt
	Location	string(256)
	Server	string(256)

*/
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_Discovery_Device_ObjectForm[] =
{
	{"Status", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Device_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"UUID", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Device_UUID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"USN", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Device_USN, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LeaseTime", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Device_LeaseTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Location", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Device_Location, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Server", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Device_Server, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.UPnP.Discovery.RootDevice.{i}.	object
/*
	Status	string
	UUID	string(36)
	USN	string(256)
	LeaseTime	unsignedInt
	Location	string(256)
	Server	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_Discovery_RootDevice_ObjectForm[] =
{
	{"Status", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDevice_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"UUID", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDevice_UUID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"USN", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDevice_USN, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LeaseTime", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDevice_LeaseTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Location", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDevice_Location, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Server", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDevice_Server, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.UPnP.Discovery.Service.{i}.	object
/*
	Status	string
	USN	string(256)
	LeaseTime	unsignedInt
	Location	string(256)
	Server	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_Discovery_Service_ObjectForm[] =
{
	{"Status", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Service_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"USN", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Service_USN, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LeaseTime", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Service_LeaseTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Location", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Service_Location, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Server", 		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_Service_Server, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.UPnP.Discovery.
/*
	RootDeviceNumberOfEntries	unsignedInt
	DeviceNumberOfEntries	unsignedInt
	ServiceNumberOfEntries	unsignedInt

*/
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_Discovery_ObjectForm[] =
{
	{"RootDeviceNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDeviceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DeviceNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_DeviceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ServiceNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_ServiceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RootDevice",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_RootDeviceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_Discovery_RootDevice_ObjectForm, TRDMType_S, eTR157A3_Device_UPnP_Discovery_RootDeviceNumberOfEntries},
	{"Device",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_DeviceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_Discovery_Device_ObjectForm, TRDMType_S, eTR157A3_Device_UPnP_Discovery_DeviceNumberOfEntries},
	{"Service",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_UPnP_Discovery_ServiceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_Discovery_Service_ObjectForm, TRDMType_S, eTR157A3_Device_UPnP_Discovery_ServiceNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//NOT SUPPORTED
const static TRPORT_OBJECT_FORMAT_t	s_UPnP_ObjectForm[] =
{
	{"Device",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_Device_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"Discovery",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_Discovery_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.DLNA.Capabilities.
/*
	HNDDeviceClass	string(256)
	DeviceCapability	string(256)
	HIDDeviceClass	string(256)
	ImageClassProfileID	string(256)
	AudioClassProfileID	string(256)
	AVClassProfileID	string(256)
	MediaCollectionProfileID	string(256)
	PrinterClassProfileID	string(256)

*/
const static TRPORT_OBJECT_FORMAT_t	s_DLNA_Capabilities_ObjectForm[] =
{
	{"HNDDeviceClass",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_HNDDeviceClass, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DeviceCapability",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_DeviceCapability, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HIDDeviceClass",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_HIDDeviceClass, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ImageClassProfileID", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_ImageClassProfileID, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AudioClassProfileID", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_AudioClassProfileID, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AVClassProfileID",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_AVClassProfileID, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MediaCollectionProfileID",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_MediaCollectionProfileID, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PrinterClassProfileID",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP157_Get_DLNA_Capabilities_PrinterClassProfileID, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
const static TRPORT_OBJECT_FORMAT_t	s_DLNA_ObjectForm[] =
{
	{"Capabilities",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_DLNA_Capabilities_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.SmartCardReader.{i}.SmartCard.X_JCOM_GroupId.{i}.
/*
	Number	srint(20)
*/
const static TRPORT_OBJECT_FORMAT_t	s_SmartCard_X_JCOM_GroupId_ObjectForm[] =
{
	{"Number",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.SmartCardReader.{i}.SmartCard.	object
/*
	Status	string
	Type	string
	Application	string
	SerialNumber	string(256)
	ATR	string(1024)
	X_JCOM_CardType	string(4)
	X_JCOM_GroupIdNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_SmartCardReader_SmartCard_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_Status, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Type", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_Type, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Application",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_Application, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SerialNumber", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_SerialNumber, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ATR",	eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_ATR, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_CardType", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_CardType, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_GroupIdNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_GroupId",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SmartCard_X_JCOM_GroupId_ObjectForm, TRDMType_S, eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.SmartCardReader.{i}.
/*
	///Enable	boolean	W //not supported
	Status	string
	Name	string(256)
	///Reset	boolean	W	//not supported
	///ResetTime	dateTime	//not supported

*/
const static TRPORT_OBJECT_FORMAT_t	s_SmartCardReader_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_Enable, (pfn_setCB_t)TRP157_Set_SmartCardReader_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Reset",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_Reset, (pfn_setCB_t)TRP157_Set_SmartCardReader_M_Reset, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ResetTime",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_SmartCardReader_M_ResetTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SmartCard",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SmartCardReader_SmartCard_ObjectForm, TRDMType_M, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.SelfTestDiagnostics.
/*
	DiagnosticsState	string	W
	Results	string(1024)

*/
const static TRPORT_OBJECT_FORMAT_t	s_SelfTestDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SelfTestDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_SelfTestDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Results", 		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TRP157_Get_SelfTestDiagnostics_Results, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.NSLookupDiagnostics.Result.{i}.
/*
	Status	string
	AnswerType	string
	HostNameReturned	string(256)
	IPAddresses	string(45)
	DNSServerIP	string(45)
	ResponseTime	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_NSLookupDiagnostics_Result_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Result_M_Status, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Result_M_Status, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AnswerType", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Result_M_AnswerType, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Result_M_AnswerType, NULL, TRDMType_M, eTRXDMInstance_None},
	{"HostNameReturned",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Result_M_HostNameReturned, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Result_M_HostNameReturned, NULL, TRDMType_M, eTRXDMInstance_None},
	{"IPAddresses", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Result_M_IPAddresses, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Result_M_IPAddresses, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DNSServerIP",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Result_M_DNSServerIP, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Result_M_DNSServerIP, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ResponseTime", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Result_M_ResponseTime, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Result_M_ResponseTime, NULL, TRDMType_M, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.NSLookupDiagnostics.
/*
	DiagnosticsState	string	W
	Interface	string(256)	W
	HostName	string(256)	W
	DNSServer	string(256)	W
	Timeout	unsignedInt	W
	NumberOfRepetitions	unsignedInt	W
	SuccessCount	unsignedInt
	ResultNumberOfEntries	unsignedInt

*/
const static TRPORT_OBJECT_FORMAT_t	s_NSLookupDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Interface", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Interface, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Interface, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HostName",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_HostName, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_HostName, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DNSServer", 		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_DNSServer, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_DNSServer, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Timeout",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_Timeout, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_Timeout, NULL, TRDMType_S, eTRXDMInstance_None},
	{"NumberOfRepetitions", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_NumberOfRepetitions, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_NumberOfRepetitions, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SuccessCount",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_SuccessCount, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_SuccessCount, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ResultNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_ResultNumberOfEntries, (pfn_setCB_t)TRP157_Set_NSLookupDiagnostics_ResultNumberOfEntries, NULL, TRDMType_S, eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries},
	{"Result",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_NSLookupDiagnostics_ResultNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_NSLookupDiagnostics_Result_ObjectForm, TRDMType_S, eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.USBHosts.Host.{i}.Device.{i}.Configuration.{i}.Interface.{i}.
/*
	InterfaceNumber	unsignedInt[:255]
	InterfaceClass	hexBinary(1)
	InterfaceSubClass	hexBinary(1)
	InterfaceProtocol	hexBinary(1)
*/
const static TRPORT_OBJECT_FORMAT_t	s_USBHosts_Host_Device_Configuration_Interface_ObjectForm[] =
{
	{"InterfaceNumber",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceNumber, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"InterfaceClass", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceClass, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"InterfaceSubClass",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceSubClass, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"InterfaceProtocol", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceProtocol, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.USBHosts.Host.{i}.Device.{i}.Configuration.{i}.
/*
	ConfigurationNumber	unsignedInt
	InterfaceNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_USBHosts_Host_Device_Configuration_ObjectForm[] =
{
	{"ConfigurationNumber",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_ConfigurationNumber, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"InterfaceNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_InterfaceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Interface",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Config_M_InterfaceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_USBHosts_Host_Device_Configuration_Interface_ObjectForm, TRDMType_S, eTR157A3_DUSBHosts_HDev_Configuration_M_InterfaceNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.USBHosts.Host.{i}.Device.{i}.
/*
	DeviceNumber	unsignedInt
	USBVersion	string(4)
	DeviceClass	hexBinary(1)
	DeviceSubClass	hexBinary(1)
	DeviceVersion	unsignedInt[:65535]
	DeviceProtocol	hexBinary(1)
	ProductID	unsignedInt[:65535]
	VendorID	unsignedInt[:65535]
	Manufacturer	string(64)
	ProductClass	string(64)
	SerialNumber	string(64)
	Port	unsignedInt[:255]
	Rate	string
	Parent	string
	MaxChildren	unsignedInt
	IsSuspended	boolean
	IsSelfPowered	boolean
	ConfigurationNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_USBHosts_Host_Device_ObjectForm[] =
{
	{"DeviceNumber",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_DeviceNumber, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"USBVersion", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_USBVersion, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DeviceClass",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_DeviceClass, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DeviceSubClass", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_DeviceSubClass, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DeviceVersion",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_DeviceVersion, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DeviceProtocol", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_DeviceProtocol, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ProductID",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_ProductID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"VendorID", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_VendorID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Manufacturer",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Manufacturer, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ProductClass", 		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_ProductClass, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SerialNumber",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_SerialNumber, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Port", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Port, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Rate",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Rate, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Parent", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_Parent, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaxChildren",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_MaxChildren, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"IsSuspended", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_IsSuspended, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"IsSelfPowered",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_IsSelfPowered, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ConfigurationNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_ConfigurationNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Configuration",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Device_M_ConfigurationNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_USBHosts_Host_Device_Configuration_ObjectForm, TRDMType_S, eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.USBHosts.Host.{i}.
/*
	///Enable	boolean	W
	Name	string(64)
	Type	string
	///Reset	boolean	W
	///PowerManagementEnable	boolean	W
	USBVersion	string(4)
	DeviceNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_USBHosts_Host_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Enable, (pfn_setCB_t)TRP157_Set_USBHosts_Host_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Type", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Type, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Reset",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_Reset, (pfn_setCB_t)TRP157_Get_USBHosts_Host_M_Reset, NULL, TRDMType_M, eTRXDMInstance_None},
	{"PowerManagementEnable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_PowerManagementEnable, (pfn_setCB_t)TRP157_Set_USBHosts_Host_M_PowerManagementEnable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"USBVersion",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_USBVersion, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DeviceNumberOfEntries", 		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_DeviceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Device",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_USBHosts_Host_M_DeviceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_USBHosts_Host_Device_ObjectForm, TRDMType_S, eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.USBHosts.
/*
	HostNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_USBHosts_ObjectForm[] =
{
	{"HostNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_USBHosts_HostNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Host",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_USBHosts_HostNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_USBHosts_Host_ObjectForm, TRDMType_S, eTR157A3_Device_USBHosts_HostNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.PeriodicStatistics.SampleSet.{i}.Parameter.{i}.	object
/*
	Enable	boolean	W
	Reference	string(256)	W
	SampleMode	string	W
	CalculationMode	string	W
	LowThreshold	int	W
	HighThreshold	int	W
	SampleSeconds	string
	SuspectData	string
	Values	string
	Failures	unsignedInt

*/
const static TRPORT_OBJECT_FORMAT_t	s_PeriodicStatistics_SampleSet_Parameter_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Enable, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Reference",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Reference, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_Reference, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SampleMode",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode, NULL, TRDMType_M, eTRXDMInstance_None},
	{"CalculationMode",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LowThreshold",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold, NULL, TRDMType_M, eTRXDMInstance_None},
	{"HighThreshold",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SampleSeconds",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SuspectData",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Values",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Values, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Failures",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Failures, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.PeriodicStatistics.SampleSet.{i}.	object
/*
	Enable	boolean	W
	Status	string
	Name	string(128)	W
	SampleInterval	unsignedInt[1:]	W
	ReportSamples	unsignedInt[1:]	W
	TimeReference	dateTime	W
	FetchSamples	unsignedInt	W
	ForceSample	boolean	W
	ReportStartTime	dateTime
	ReportEndTime	dateTime
	SampleSeconds	string
	ParameterNumberOfEntries	unsignedInt

*/
const static TRPORT_OBJECT_FORMAT_t	s_PeriodicStatistics_SampleSet_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Enable, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_Name, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_Name, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SampleInterval",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_SampleInterval, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_SampleInterval, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ReportSamples",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ReportSamples, (pfn_setCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ReportSamples, NULL, TRDMType_M, eTRXDMInstance_None},
	{"TimeReference",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_TimeReference, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_TimeReference, NULL, TRDMType_M, eTRXDMInstance_None},
	{"FetchSamples",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_FetchSamples, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_FetchSamples, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ForceSample",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ForceSample, (pfn_setCB_t)TRP157_Set_PeriodicStatistics_SampleSet_M_ForceSample, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ReportStartTime",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ReportStartTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ReportEndTime",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ReportEndTime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SampleSeconds",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_SampleSeconds, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ParameterNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Parameter",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_PeriodicStatistics_SampleSet_Parameter_ObjectForm, TRDMType_S, eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.PeriodicStatistics.
/*
	MinSampleInterval	unsignedInt
	MaxReportSamples	unsignedInt
	SampleSetNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_PeriodicStatistics_ObjectForm[] =
{
	{"MinSampleInterval",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_MinSampleInterval, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxReportSamples",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_MaxReportSamples, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SampleSetNumberOfEntries",	eHLIB_JSON_Integer, 16, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSetNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SampleSet",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_PeriodicStatistics_SampleSetNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_PeriodicStatistics_SampleSet_ObjectForm, TRDMType_S, eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Capabilities.PerformanceDiagnostic.
/*
	DownloadTransports	string
	UploadTransports	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_PerformanceDiagnostic_ObjectForm[] =
{
	{"DownloadTransports",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_Capa_PerformanceDiagnostic_DownloadTransports, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UploadTransports",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_Capa_PerformanceDiagnostic_UploadTransports, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_ObjectForm[] =
{
	{"PerformanceDiagnostic",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_PerformanceDiagnostic_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.SoftwareModules.ExecEnv.{i}.
/*
	Enable	boolean	W
	Status	string
	Name	string(32)
	Type	string(64)
	Vendor	string(128)
	Version	string(32)
	AllocatedDiskSpace	int[-1:]
	AvailableDiskSpace	int[-1:]
	AllocatedMemory	int[-1:]
	AvailableMemory	int[-1:]
	ActiveExecutionUnits	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_SoftwareModules_ExecEnv_ObjectForm[] =
{
	{"Enable", 			eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_Enable, 			(pfn_setCB_t)TRP157_Set_SoftwareModules_ExecEnv_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",  eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_Name, 	(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Type", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_Type, 			(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Vendor", 			eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_Vendor, 			(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Version",  eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_Version, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AllocatedDiskSpace",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_AllocatedDiskSpace, 	(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AvailableDiskSpace", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_AvailableDiskSpace, 			(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AllocatedMemory", 			eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_AllocatedMemory, 			(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AvailableMemory",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_AvailableMemory, 	(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ActiveExecutionUnits", 		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnv_M_ActiveExecutionUnits, 			(pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, 						eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.SoftwareModules.
/*
	ExecEnvNumberOfEntries	unsignedInt
*/
//2 Not Supported.
const static TRPORT_OBJECT_FORMAT_t	s_SoftwareModules_ObjectForm[] =
{
	{"ExecEnvNumberOfEntries", 			eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnvNumberOfEntries, 			(pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ExecEnv",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_SoftwareModules_ExecEnvNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SoftwareModules_ExecEnv_ObjectForm, TRDMType_S, eTR157A3_Device_SoftwareModules_ExecEnvNumberOfEntries},
	{NULL, 						eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Device.X_JLABS_Management.AudienceMeasurement
	CombinedAccess	boolean	W
	PeriodicInformationInterval	unsignedInt[1:]	W
	PeriodicInformationTime	dateTime	W
	PermissionLevel	unsignedInt[0:2]	W

*/
const static TRPORT_OBJECT_FORMAT_t	s_AudienceMeasurement_ObjectForm[] =
{
	{"CombinedAccess", 			eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP_Get_AudienceMeasurement_CombinedAccess, 			(pfn_setCB_t)TRP_SET_AudienceMeasurement_CombinedAccess, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PeriodicInformationInterval",  eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP_Get_AudienceMeasurement_PeriodicInformationInterval, (pfn_setCB_t)TRP_SET_AudienceMeasurement_PeriodicInformationInterval, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PeriodicInformationTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP_Get_AudienceMeasurement_PeriodicInformationTime, 	(pfn_setCB_t)TRP_SET_AudienceMeasurement_PeriodicInformationTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PermissionLevel", 		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP_Get_AudienceMeasurement_PermissionLevel, 			(pfn_setCB_t)TRP_SET_AudienceMeasurement_PermissionLevel, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, 						eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Device.X_JLABS_Management
+ AudienceMeasurement
*/

const static TRPORT_OBJECT_FORMAT_t	s_X_JLABS_Management_ObjectForm[] =
{
	{"AudienceMeasurement",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_AudienceMeasurement_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.X_JCOM_HomeNetwork.Stats.	object
/*
	ConnectionUpTime	unsignedInt
	TotalBytesSent	unsignedInt
	TotalBytesReceived	unsignedInt
	TotalPacketsSent	unsignedInt
	TotalPacketsReceived	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_HomeNetwork_Stats_ObjectForm[] =
{
	{"ConnectionUpTime",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Stats_ConnectionUpTime, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalBytesSent",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Stats_TotalBytesSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalBytesReceived",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Stats_TotalBytesReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalPacketsSent",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Stats_TotalPacketsSent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalPacketsReceived",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Stats_TotalPacketsReceived, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.X_JCOM_HomeNetwork.IPPingDiagnostics.	object
/*
	DiagnosticsState	string
	Host	string(256)
	NumberOfRepetitions	unsignedint[1:]
	Timeout	unsignedint[1:]
	DataBlockSize	unsignedInt[1:65535]
	SuccessCount	unsignedint
	FailureCount	unsignedint
	AverageResponseTime	unsignedint
	MinimumResponseTime	unsignedint
	MaximumResponseTime	unsignedint
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_HomeNetwork_IPPingDiagnostics_ObjectForm[] =
{
	{"DiagnosticsState",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_DiagnosticsState, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Host",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_Host, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_Host, NULL, TRDMType_S, eTRXDMInstance_None},
	{"NumberOfRepetitions",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_NumberOfRepetitions, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_NumberOfRepetitions, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Timeout",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_Timeout, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_Timeout, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DataBlockSize",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_DataBlockSize, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_DataBlockSize, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SuccessCount",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_SuccessCount, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_SuccessCount, NULL, TRDMType_S, eTRXDMInstance_None},
	{"FailureCount",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_FailureCount, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_FailureCount, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AverageResponseTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_AverageResponseTime, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_AverageResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MinimumResponseTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_MinimumResponseTime, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_MinimumResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaximumResponseTime",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_MaximumResponseTime, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_MaximumResponseTime, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.X_JCOM_HomeNetwork.	object
/*
	Enable	boolean	W
	Status	string
	AddressingType	string	W
	IPAddress	string(45)	W
	SubnetMask	string(45)	W
	DefaultGateway	string	W
	DNSServers	string(256)	W
	MACAddress	string(17)
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_HomeNetwork_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Enable, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_Status, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AddressingType",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_AddressingType, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_AddressingType, NULL, TRDMType_S, eTRXDMInstance_None},
	{"IPAddress",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_IPAddress, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_IPAddress, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SubnetMask",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_SubnetMask, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_SubnetMask, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DefaultGateway",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_DefaultGateway, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_DefaultGateway, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DNSServers",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_DNSServers, (pfn_setCB_t)TRP157_Set_X_JCOM_HomeNet_DNSServers, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MACAddress",	eHLIB_JSON_String, TRPORT_MACADDR_LEN, (pfn_getCB_t)TRP157_Get_X_JCOM_HomeNet_MACAddress, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"IPPingDiagnostics",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_HomeNetwork_IPPingDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"Stats",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_HomeNetwork_Stats_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.X_JCOM_Wifi.	object
/*
	Enable	boolean	W
	ConfigMethod	string	W
	SSID	string(32)	W
	SecurityMethod	string	W
	SecurityLevel	string	W
	SecurityKey	string(64)	W
	KeyIndex	unsignedInt[0:3]	W
	AddressingType	string
	IPAddress	string(45)
	SubnetMask	string(45)
	DefaultGateway	string

*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Wifi_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_Enable, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ConfigMethod",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_ConfigMethod, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_ConfigMethod, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SSID",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_SSID, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_SSID, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SecurityMethod",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_SecurityMethod, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_SecurityMethod, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SecurityLevel",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_SecurityLevel, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_SecurityLevel, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SecurityKey",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_SecurityKey, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_SecurityKey, NULL, TRDMType_S, eTRXDMInstance_None},
	{"KeyIndex",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_KeyIndex, (pfn_setCB_t)TRP157_Set_X_JCOM_Wifi_KeyIndex, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AddressingType",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_AddressingType, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"IPAddress",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_IPAddress, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SubnetMask",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_SubnetMask, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DefaultGateway",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP157_Get_X_JCOM_Wifi_DefaultGateway, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},

	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

#define ___TR135___
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// STBService //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.PVR.
/*
	MaxIOStreams	int[-1:]
	MaxRecordingStreams	int[-1:]
	MaxPlaybackStreams	int[-1:]
	//////MaxTimeDelay	int[-1:]
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_PVR_ObjectForm[] =
{
	{"MaxIOStreams",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_PVR_MaxIOStreams, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxRecordingStreams",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_PVR_MaxRecordingStreams, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxPlaybackStreams",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_PVR_MaxPlaybackStreams, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxTimeDelay",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_PVR_MaxTimeDelay, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//Device.Services.STBService.{i}.Capabilities.AudioDecoder.
//AudioStandards	string
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_AudioDecoder_ObjectForm[] =
{
	{"AudioStandards",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_AudioDecoder_AudioStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
////////////////////////////////////////////////// MPEG2Part2 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG2Part2_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_Profile, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Level",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_Level, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaximumDecodingCapability",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_MaximumDecodingCapability, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG2Part2_ObjectForm[] =
{
	{"AudioStandards",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP2P2_AudioStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevelNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP2P2_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevel",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP2P2_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG2Part2_ProfileLevel_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
////////////////////////////////////////////////// MPEG4Part2 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part2.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MP4P2_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_Profile, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Level",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_Level, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaximumDecodingCapability",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_MaximumDecodingCapability, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part2.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MP4P2_ObjectForm[] =
{
	{"AudioStandards",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P2_AudioStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevelNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P2_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevel",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P2_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG2Part2_ProfileLevel_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevelNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
////////////////////////////////////////////////// MPEG4Part10 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MP4P10_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_Profile, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Level",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_Level, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaximumDecodingCapability",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_MaximumDecodingCapability, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MP4P10_ObjectForm[] =
{
	{"AudioStandards",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P10_AudioStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevelNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P10_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevel",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Capa_VDec_MP4P10_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MP4P10_ProfileLevel_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

////////////////////////////////////////////////// SMPTEVC1 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
//2 Not Supported
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_SMPTEVC1_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Profile, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Level",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Level, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaximumDecodingCapability",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_MaximumDecodingCapability, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
//2 Not Supported
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_SMPTEVC1_ObjectForm[] =
{
	{"AudioStandards",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_SMPTEVC1_AudioStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevelNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ProfileLevel",	eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevelNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_SMPTEVC1_ProfileLevel_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
////////////////////////////////////////////////// VideoDecoder //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.
/*
	VideoStandards	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_ObjectForm[] =
{
	{"VideoStandards",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_VDec_VideoStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MPEG2Part2",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG2Part2_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"MPEG4Part2",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MP4P2_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"MPEG4Part10",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MP4P10_ObjectForm, TRDMType_S, eTRXDMInstance_None},
// Not Supported
	{"SMPTEVC1",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_SMPTEVC1_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//Device.Services.STBService.{i}.Capabilities.AudioOutput.
/*
	AudioFormats	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_AudioOutput_ObjectForm[] =
{
	{"AudioFormats",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_AOutput_AudioFormats, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//Device.Services.STBService.{i}.Capabilities.VideoOutput.
/*
	CompositeVideoStandards	string
	VideoFormats	string
	Macrovision	string(32)
	HDCP	string(32)
	DisplayFormats	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoOutput_ObjectForm[] =
{
	{"CompositeVideoStandards",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Capa_VOutput_CompositeVideoStandards, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VideoFormats",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Capa_VOutput_VideoFormats, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Macrovision",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VOutput_Macrovision, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HDCP",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_VOutput_HDCP, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DisplayFormats",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Capa_VOutput_DisplayFormats, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//Device.Services.STBService.{i}.Capabilities.HDMI.
/*
	SupportedResolutions	string
	CECSupport	boolean
	HDMI3D	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_HDMI_ObjectForm[] =
{
	{"SupportedResolutions",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Capa_HDMI_SupportedResolutions, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CECSupport",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_HDMI_CECSupport, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HDMI3D",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_HDMI_HDMI3D, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//Device.Services.STBService.{i}.Capabilities.CA.
/*
	CASystems	string(1024)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_CA_ObjectForm[] =
{
	{"CASystems",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Capa_CA_CASystems, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//Device.Services.STBService.{i}.Capabilities.DRM.
/*
	DRMSystems	string(1024)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_DRM_ObjectForm[] =
{
	{"DRMSystems",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Capa_DRM_DRMSystems, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.
/*
	MaxActiveMainStreams	int[-1:]
	MinSampleInterval	int[-1:]
	MaxReportSamples	int[-1:]
	HighLevelMetricNames	string(1024)
*/
#if 1 //Not Supported
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_ServiceMonitoring_ObjectForm[] =
{
	{"MaxActiveMainStreams",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_ServiceMonitoring_MaxActiveMainStreams, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MinSampleInterval",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_ServiceMonitoring_MinSampleInterval, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxReportSamples",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_ServiceMonitoring_MaxReportSamples, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HighLevelMetricNames",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Capa_ServiceMonitoring_HighLevelMetricNames, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
#endif

//Device.Services.STBService.{i}.Capabilities.
/*
	MaxActiveAVStreams	int[-1:]
	MaxActiveAVPlayers	int[-1:]
*/
const static TRPORT_OBJECT_FORMAT_t	s_STBService_Capabilities_ObjectForm[] =
{
	{"MaxActiveAVStreams",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_MaxActiveAVStreams, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MaxActiveAVPlayers",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Capa_MaxActiveAVPlayers, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PVR",					eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_PVR_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"AudioDecoder",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_AudioDecoder_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"VideoDecoder",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"AudioOutput",			eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_AudioOutput_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"VideoOutput",			eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoOutput_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"HDMI",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_HDMI_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"CA",					eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_CA_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"DRM",					eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_DRM_ObjectForm, TRDMType_S, eTRXDMInstance_None},
#if 1 //Not Supported
	{"ServiceMonitoring",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_ServiceMonitoring_ObjectForm, TRDMType_S, eTRXDMInstance_None},
#endif
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_ISDBC.Modulation.
/*
	Frequency	unsignedInt
	Constellation	string
	SNR	unsignedInt
	BER	unsignedInt
	--> Not Supported : CBER	unsignedInt
	Power	unsignedInt
	Locked	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_X_JCOM_ISDBC_Modulation_ObjectForm[] =
{
	{"Frequency",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Frequency, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Constellation",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Constellation, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SNR",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_SNR, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"BER",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_BER, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CBER",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_CBER, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Power",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Power, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Locked",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Locked, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_ISDBC.Service.
/*
	CurrentLogicalChannel	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_X_JCOM_ISDBC_Service_ObjectForm[] =
{
	{"CurrentLogicalChannel",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Service_CurrentLogicalChannel, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_X_JCOM_ISDBC_ObjectForm[] =
{
	{"Modulation",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_X_JCOM_ISDBC_Modulation_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"Service",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_X_JCOM_ISDBC_Service_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_DOCSIS.download.
/*
	Frequency	unsignedInt
	Constellation	string
	BER	unsignedInt
	SNR	unsignedInt
	Power	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_X_JCOM_DOCSIS_download_ObjectForm[] =
{
	{"Frequency",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Frequency, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Constellation",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Constellation, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"BER",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_BER, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SNR",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_SNR, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Power",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Power, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_DOCSIS.upload.
/*
	Frequency	unsignedInt
	ChannelId	unsignedInt
	BandWidth	unsignedInt
	Power	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_X_JCOM_DOCSIS_upload_ObjectForm[] =
{
	{"Frequency",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Frequency, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ChannelId",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_ChannelId, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"BandWidth",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_BandWidth, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Power",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Power, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_DOCSIS.
/*
	Status	string
	IPAddress	string(45)
	MACAddress	string(17)
*/
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_X_JCOM_DOCSIS_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_Status, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"IPAddress",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_IPAddress, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"MACAddress",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_MACAddress, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"download",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_X_JCOM_DOCSIS_download_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"upload",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_X_JCOM_DOCSIS_upload_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.
/*
	Status	string
	Name	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_FrontEnd_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_FrontEnd_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"X_JCOM_ISDBC",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_X_JCOM_ISDBC_ObjectForm, TRDMType_S, eTRXDMInstance_Exception_ISDBC},
	{"X_JCOM_DOCSIS",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_X_JCOM_DOCSIS_ObjectForm, TRDMType_S, eTRXDMInstance_Exception_Docsis},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.PVR.Storage.{i}.
/*
	Reference	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_PVR_Storage_ObjectForm[] =
{
	{"Reference",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_PVR_Storage_M_Reference, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.PVR.
/*
	StorageNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_PVR_ObjectForm[] =
{
	{"StorageNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_PVR_StorageNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Storage",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_PVR_StorageNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_PVR_Storage_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_PVR_StorageNumberOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.AudioDecoder.{i}.
/*
	Status	string
	Name	string(256)
	AudioStandard	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_AudioDecoder_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioDecoder_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioDecoder_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioStandard",	eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_Comp_AudioDecoder_M_AudioStandard, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.VideoDecoder.{i}.
/*
	Status	string
	Name	string(256)
	MPEG2Part2	string(256)
	MPEG4Part2	string(256)
	MPEG4Part10	string(256)
	--> Not Supported : SMPTEVC1	string(256)
	ContentAspectRatio	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_VideoDecoder_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MPEG2Part2",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_MPEG2Part2, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MPEG4Part2",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_MPEG4Part2, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MPEG4Part10",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_MPEG4Part10, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SMPTEVC1",	eHLIB_JSON_String, 2, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_SMPTEVC1, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ContentAspectRatio",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoder_M_ContentAspectRatio, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.AudioOutput.{i}.
/*
	Status	string
	Name	string(256)
	AudioFormat	string
	AudioLevel	unsignedInt[0:100]	W
	CancelMute	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_AudioOutput_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioOutput_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioOutput_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioFormat",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Comp_AudioOutput_M_AudioFormat, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioLevel",	eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_Comp_AudioOutput_M_AudioLevel, (pfn_setCB_t)TRP135_Set_Comp_AudioOutput_M_AudioLevel, NULL, TRDMType_M, eTRXDMInstance_None},
	{"CancelMute",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioOutput_M_CancelMute, (pfn_setCB_t)TRP135_Set_Comp_AudioOutput_M_CancelMute, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.SPDIF.{i}.
/*
	Status	string
	Name	string(256)
	ForcePCM	boolean	W
	Passthrough	boolean
	AudioDelay	unsigned int
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_SPDIF_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_SPDIF_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_SPDIF_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ForcePCM",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_SPDIF_M_ForcePCM, (pfn_setCB_t)TRP135_Set_Comp_SPDIF_M_ForcePCM, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Passthrough",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_SPDIF_M_Passthrough, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioDelay",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_SPDIF_M_AudioDelay, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.VideoOutput.{i}.
/*
	Status	string
	Name	string(256)
	CompositeVideoStandard	string
	VideoFormat	string	W
	AspectRatioBehaviour	string
	DisplayFormat	string	W
	Macrovision	boolean
	HDCP	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_VideoOutput_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"CompositeVideoStandard",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_CompositeVideoStandard, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"VideoFormat",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_VideoFormat, (pfn_setCB_t)TRP135_Set_Comp_VideoOutput_M_VideoFormat, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AspectRatioBehaviour",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_AspectRatioBehaviour, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DisplayFormat",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_DisplayFormat, (pfn_setCB_t)TRP135_Set_Comp_VideoOutput_M_DisplayFormat, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Macrovision",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_Macrovision, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"HDCP",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutput_M_HDCP, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.HDMI.{i}.DisplayDevice.
/*
	Status	string
	Name	string(256)
	EEDID	hexBinary(256)
	SupportedResolutions	string
	PreferredResolution	string
	VideoLatency	unsignedInt
	CECSupport	boolean
	AutoLipSyncSupport	boolean
	HDMI3DPresent	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_HDMI_DisplayDevice_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_Status, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_Name, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"EEDID",	eHLIB_JSON_String, SIZE_256+1, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_EEDID, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SupportedResolutions",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_SupportedResolutions, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PreferredResolution",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_PreferredResolution, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VideoLatency",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_VideoLatency, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CECSupport",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_CECSupport, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AutoLipSyncSupport",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HDMI3DPresent",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_DisplayDevice_HDMI3DPresent, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.HDMI.{i}.
/*
	Status	string
	Name	string(256)
	ResolutionMode	string	W
	ResolutionValue	string	W
	X_JCOM_CECEnabled	boolean	W
	X_JCOM_3DOutput	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_HDMI_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ResolutionMode",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_ResolutionMode, (pfn_setCB_t)TRP135_Set_Comp_HDMI_M_ResolutionMode, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ResolutionValue",	eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_ResolutionValue, (pfn_setCB_t)TRP135_Set_Comp_HDMI_M_ResolutionValue, NULL, TRDMType_M, eTRXDMInstance_None},
	{"X_JCOM_CECEnabled",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_JCOM_CECEnabled, (pfn_setCB_t)TRP135_Set_Comp_HDMI_M_JCOM_CECEnabled, NULL, TRDMType_M, eTRXDMInstance_None},
	{"X_JCOM_3DOutput",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMI_M_JCOM_3DOutput, (pfn_setCB_t)TRP135_Set_Comp_HDMI_M_JCOM_3DOutput, NULL, TRDMType_M, eTRXDMInstance_None},
	{"DisplayDevice",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_HDMI_DisplayDevice_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.CA.{i}.
/*
	Status	string
	Name	string(256)
	SmartCardReader	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Component_CA_ObjectForm[] =
{
	{"Status",	eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_CA_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_CA_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SmartCardReader",	eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_Comp_CA_M_SmartCardReader, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.
/*
	FrontEndNumberOfEntries	unsignedInt
	AudioDecoderNumberOfEntries	unsignedInt
	VideoDecoderNumberOfEntries	unsignedInt
	AudioOutputNumberOfEntries	unsignedInt
	VideoOutputNumberOfEntries	unsignedInt
	CANumberOfEntries	unsignedInt
	HDMINumberOfEntries	unsignedInt
	SPDIFNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Components_ObjectForm[] =
{
	{"FrontEndNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_FrontEndNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AudioDecoderNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioDecoderNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VideoDecoderNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoDecoderNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AudioOutputNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_AudioOutputNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VideoOutputNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_VideoOutputNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"CANumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_CANumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HDMINumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_HDMINumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SPDIFNumberOfEntries",	eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Comp_SPDIFNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"FrontEnd",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_FrontEndNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_FrontEnd_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries},
	{"PVR",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_PVR_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"AudioDecoder",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_AudioDecoderNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_AudioDecoder_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries},
	{"VideoDecoder",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_VideoOutputNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_VideoDecoder_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries},
	{"AudioOutput",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_AudioOutputNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_AudioOutput_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries},
	{"SPDIF",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_SPDIFNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_SPDIF_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries},
	{"VideoOutput",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_VideoOutputNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_VideoOutput_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries},
	{"HDMI",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_HDMINumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_HDMI_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries},
	{"CA",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Comp_CANumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Component_CA_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_Comp_CANumberOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.AVStreams.AVStream.{i}.
/*
	Status	string
	Name	string(256)
	PVRState	string
	FrontEnd	string(256)
	AudioDecoder	string(256)
	VideoDecoder	string(256)
	CA	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_AVStreams_AVStream_ObjectForm[] =
{
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"PVRState",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_PVRState, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"FrontEnd",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_FrontEnd, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioDecoder",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_AudioDecoder, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"VideoDecoder",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_VideoDecoder, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"CA",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_AVStreams_AVStream_M_CA, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.AVStreams.
/*
	ActiveAVStreams	unsignedInt
	AVStreamNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_AVStreams_ObjectForm[] =
{
	{"ActiveAVStreams",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVStreams_ActiveAVStreams, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AVStreamNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_AVStreams_AVStreamNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AVStream",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_AVStreams_AVStreamNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_AVStreams_AVStream_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.
/*
	Enable	boolean
	Status	string
	Alias		string(64)
	Name	string(256)
	AudioLanguage	string(64)
	SubtitlingStatus	string
	SubtitlingLanguage	string(64)
	AudioOutputs	string(1024)
	VideoOutputs	string(1024)
	MainStream	string(256)
	PIPStreams	string(1024)
*/
const static TRPORT_OBJECT_FORMAT_t	s_AVPlayers_AVPlayer_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_Enable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Alias",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_Alias, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioLanguage",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_AudioLanguage, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SubtitlingStatus",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_SubtitlingStatus, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SubtitlingLanguage",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_SubtitlingLanguage, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AudioOutputs",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_AudioOutputs, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"VideoOutputs",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_VideoOutputs, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MainStream",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_MainStream, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"PIPStreams",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayer_M_PIPStreams, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.AVPlayers.
/*
	ActiveAVPlayers	unsignedInt
	AVPlayerNumberOfEntries	unsignedInt
	PreferredAudioLanguage	string(64)	W
	PreferredSubtitlingLanguage	string(64)	W
	PreferredBehaviour	string(64)	W
	ResetPINCode	boolean	W
	X_JCOM_PreferredSuperimposeLanguage	string(64)	W
	X_JCOM_Zoom	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_AVPlayers_ObjectForm[] =
{
	{"ActiveAVPlayers",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_ActiveAVPlayers, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AVPlayerNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayerNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PreferredAudioLanguage",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_AVPlayers_PreferredAudioLanguage, (pfn_setCB_t)TRP135_Set_AVPlayers_PreferredAudioLanguage, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PreferredSubtitlingLanguage",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_AVPlayers_PreferredSubtitlingLanguage, (pfn_setCB_t)TRP135_Set_AVPlayers_PreferredSubtitlingLanguage, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PreferredBehaviour",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_AVPlayers_PreferredBehaviour, (pfn_setCB_t)TRP135_Set_AVPlayers_PreferredBehaviour, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ResetPINCode",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_ResetPINCode, (pfn_setCB_t)TRP135_Set_AVPlayers_ResetPINCode, NULL, TRDMType_S, eTRXDMInstance_None},
//	{"X_JCOM_PreferredSuperimposeLanguage",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP135_Get_AVPlayers_JCOM_PreferredSuperimposeLanguage, (pfn_setCB_t)TRP135_Set_AVPlayers_JCOM_PreferredSuperimposeLanguage, NULL, TRDMType_S, eTRXDMInstance_None},
//	{"X_JCOM_Zoom",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_AVPlayers_JCOM_Zoom, (pfn_setCB_t)TRP135_Set_AVPlayers_JCOM_Zoom, NULL, TRDMType_S, eTRXDMInstance_None},

	{"AVPlayer",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_AVPlayers_AVPlayerNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_AVPlayers_AVPlayer_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},

};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.HomeNetwork.	object
/*
	Enable	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_HomeNetwork_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_HomeNetwork_Enable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ChannelMasking.
/*
	Status	string
	ForcedOperation	boolean	W
	Enable	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ChannelMasking_ObjectForm[] =
{
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ChannelMasking_Status, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ForcedOperation",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ChannelMasking_ForcedOperation, (pfn_setCB_t)TRP135_Set_JCOM_Config_ChannelMasking_ForcedOperation, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ChannelMasking_Enable, (pfn_setCB_t)TRP135_Set_JCOM_Config_ChannelMasking_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

#if 1 // Not Supported
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.Internet.HTTPProxy.
/*
	Enable	boolean	W
	IPAddress	string	W
	PortNumber	unsignedint[0:65535]	W
	UserChange	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_Internet_HTTPProxy_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_Internet_HTTPProxy_Enable, (pfn_setCB_t)TRP135_Set_JCOM_Config_Internet_HTTPProxy_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"IPAddress",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_Internet_HTTPProxy_IPAddress, (pfn_setCB_t)TRP135_Set_JCOM_Config_Internet_HTTPProxy_IPAddress, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PortNumber",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_Internet_HTTPProxy_PortNumber, (pfn_setCB_t)TRP135_Set_JCOM_Config_Internet_HTTPProxy_PortNumber, NULL, TRDMType_S, eTRXDMInstance_None},
	{"UserChange",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_Internet_HTTPProxy_UserChange, (pfn_setCB_t)TRP135_Set_JCOM_Config_Internet_HTTPProxy_UserChange, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.Internet.
/*
	ConnectionType	string	W
	ConnectionTypeUserChange	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_Internet_ObjectForm[] =
{
	{"ConnectionType",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_Internet_ConnectionType, (pfn_setCB_t)TRP135_Set_JCOM_Config_Internet_ConnectionType, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ConnectionTypeUserChange",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_Internet_ConnectionTypeUserChange, (pfn_setCB_t)TRP135_Set_JCOM_Config_Internet_ConnectionTypeUserChange, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HTTPProxy",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_Internet_HTTPProxy_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.MediaServer.
/*
	Names	string
	Name	string	W
	--> Not Supported : Clients	string(256)
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_MediaServer_ObjectForm[] =
{
	{"Names",		eHLIB_JSON_String, UDB_TEXT_LEN, (pfn_getCB_t)TRP135_Get_JCOM_Config_MediaServer_Names, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, UDB_TEXT_LEN, (pfn_getCB_t)TRP135_Get_JCOM_Config_MediaServer_Name, (pfn_setCB_t)TRP135_Set_JCOM_Config_MediaServer_Name, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Clients",		eHLIB_JSON_String, SIZE_512, (pfn_getCB_t)TRP135_Get_JCOM_Config_MediaServer_Clients, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.PurchasedPPV.History.{i}.
/*
	IsdbId	string(12)
	Name	string(128)
	Date	dateTime
	Price	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_PurchasedPPV_History_ObjectForm[] =
{
	{"IsdbId",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_History_M_IsdbId, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Date",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Date, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Price",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Price, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.PurchasedPPV.
/*
	TotalPriceOfThisMonth	unsignedInt
	TotalPriceOfPreviousMonth	unsignedInt
	HistoryNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_PurchasedPPV_ObjectForm[] =
{
	{"TotalPriceOfThisMonth",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_TotalPriceOfThisMonth, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TotalPriceOfPreviousMonth",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_TotalPriceOfPreviousMonth, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HistoryNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_HistoryNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"History",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_JCOM_Config_PurchasedPPV_HistoryNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_PurchasedPPV_History_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ISDBC.Install.
/*
	Start	int[0:2]	W
	Status	string
	Progress	unsignedint[0:100]
	AreaCode	unsignedInt[0:63]	W
	Frequency_DTT	string(32)	W
	Frequency_BS	unsignedInt	W
	Frequency_CS1	unsignedInt	W
	Frequency_CS2	unsignedInt	W
	Frequency_CATV	string(32)	W
	Frequency_JC-HITS	string(32)	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ISDBC_Install_ObjectForm[] =
{
	{"Start",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Start, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Start, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Status, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Progress",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Progress, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AreaCode",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_AreaCode, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_AreaCode, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Frequency_DTT",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_DTT, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_DTT, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Frequency_BS",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_BS, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_BS, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Frequency_CS1",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CS1, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CS1, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Frequency_CS2",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CS2, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CS2, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Frequency_CATV",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CATV, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CATV, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Frequency_JC-HITS",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_JC_HITS, (pfn_setCB_t)TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_JC_HITS, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ISDBC.ServiceListDatabase.{i}.LogicalChannel.{i}.
/*
	LogicalChannelNumber	unsignedInt
	Name	string(32)
	IsdbId	string(12)
	Frequency	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ISDBC_ServiceListDatabase_LogicalChannel_ObjectForm[] =
{
	{"LogicalChannelNumber",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"IsdbId",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Frequency",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ISDBC.ServiceListDatabase.{i}.
/*
NetworkId	unsignedInt
Name	string(32)
LogicalChannelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ISDBC_ServiceListDatabase_ObjectForm[] =
{
	{"NetworkId",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_NetworkId, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LogicalChannelNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LCNOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LogicalChannel",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LCNOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ISDBC_ServiceListDatabase_LogicalChannel_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ISDBC.
/*
	ServiceListDatabaseNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ISDBC_ObjectForm[] =
{
	{"ServiceListDatabaseNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabaseNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Install",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ISDBC_Install_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"ServiceListDatabase",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabaseNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ISDBC_ServiceListDatabase_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.USB-HDD.	object
/*
	PictureQuarity	unsignedInt[0:4]	W
	Selected	unsignedInt[0:8]	W
	AutoChapter	unsignedInt[0:4]	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_USB_HDD_ObjectForm[] =
{
	{"PictureQuarity",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_USB_HDD_PictureQuarity, (pfn_setCB_t)TRP135_Set_JCOM_Config_USB_HDD_PictureQuarity, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Selected",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_USB_HDD_Selected, (pfn_setCB_t)TRP135_Set_JCOM_Config_USB_HDD_Selected, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AutoChapter",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_USB_HDD_AutoChapter, (pfn_setCB_t)TRP135_Set_JCOM_Config_USB_HDD_AutoChapter, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.LAN-HDD.	object
/*
	ServerName	string(128)
	RecordingPlace	string(128)
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_LAN_HDD_ObjectForm[] =
{
	{"ServerName",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_JCOM_Config_LAN_HDD_ServerName, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RecordingPlace",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_JCOM_Config_LAN_HDD_RecordingPlace, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.HomeServer.	object
/*
	Enable	boolean	W
	Name	string	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_HomeServer_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_HomeServer_Enable, (pfn_setCB_t)TRP135_Set_JCOM_Config_HomeServer_Enable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_JCOM_Config_HomeServer_Name, (pfn_setCB_t)TRP135_Set_JCOM_Config_HomeServer_Name, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ClientEquipment.	object
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.ClientEquipment.{i}.	object
/*
	Enable	boolean
	MACAddress	string(17)
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ClientEquipment_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ClientEquipment_M_Enable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MACAddress",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_ClientEquipment_M_MACAddress, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.RemoteEquipment.	object
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.RemoteEquipment.{i}.	object
/*
	Enable	boolean
	Name	string(128)
	Nickname	string(128)
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_RemoteEquipment_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_RemoteEquipment_M_Enable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_JCOM_Config_RemoteEquipment_M_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Nickname",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP135_Get_JCOM_Config_RemoteEquipment_M_Nickname, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.X_JCOM_Config.
/*
	PostalCode	string(7)	W
	TuningRange	string	W
	URL_Navi	string(256)	W
	URL_InteracTV	string(256)	W
	URL_VOD	string(256)	W
	URL_AppDLS	string(256)	W
	URL_recommendServer	string(256)	W
	URL_OpenIDServer	string(256)	W
	URL_EPGServer	string(256)	W
	AutoFirmwareUpdate	boolean	W
	WebBrowserURLInput	boolean	W
	ParentalControl	unsignedint[4:20]	W
	PINCodeExpiration	string	W
	StandbyMode	string	W
	PowerStatus	boolean	W
	ClientEquipmentNumberOfEntries	unsignedInt
	RemoteEquipmentNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_X_JCOM_Config_ObjectForm[] =
{
	{"PostalCode",		eHLIB_JSON_String, 8, (pfn_getCB_t)TRP135_Get_JCOM_Config_PostalCode, (pfn_setCB_t)TRP135_Set_JCOM_Config_PostalCode, NULL, TRDMType_S, eTRXDMInstance_None},
	{"TuningRange",		eHLIB_JSON_String, SIZE_32, (pfn_getCB_t)TRP135_Get_JCOM_Config_TuningRange, (pfn_setCB_t)TRP135_Set_JCOM_Config_TuningRange, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_Navi",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_Navi, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_Navi, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_InteracTV",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_InteracTV, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_InteracTV, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_VOD",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_VOD, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_VOD, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_AppDLS",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_AppDLS, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_AppDLS, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_recommendServer",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_recommendServer, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_recommendServer, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_OpenIDServer",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_OpenIDServer, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_OpenIDServer, NULL, TRDMType_S, eTRXDMInstance_None},
	{"URL_EPGServer",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP135_Get_JCOM_Config_URL_EPGServer, (pfn_setCB_t)TRP135_Set_JCOM_Config_URL_EPGServer, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AutoFirmwareUpdate",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_AutoFirmwareUpdate, (pfn_setCB_t)TRP135_Set_JCOM_Config_AutoFirmwareUpdate, NULL, TRDMType_S, eTRXDMInstance_None},
	{"WebBrowserURLInput",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_WebBrowserURLInput, (pfn_setCB_t)TRP135_Set_JCOM_Config_WebBrowserURLInput, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ParentalControl",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ParentalControl, (pfn_setCB_t)TRP135_Set_JCOM_Config_ParentalControl, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PINCodeExpiration",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PINCodeExpiration, (pfn_setCB_t)TRP135_Set_JCOM_Config_PINCodeExpiration, NULL, TRDMType_S, eTRXDMInstance_None},
	{"StandbyMode",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_StandbyMode, (pfn_setCB_t)TRP135_Set_JCOM_Config_StandbyMode, NULL, TRDMType_S, eTRXDMInstance_None},
	{"PowerStatus",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_PowerStatus, (pfn_setCB_t)TRP135_Set_JCOM_Config_PowerStatus, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ClientEquipmentNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_ClientEquipmentNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RemoteEquipmentNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_JCOM_Config_RemoteEquipmentNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},

	{"HomeNetwork",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_HomeNetwork_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"ChannelMasking",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ChannelMasking_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"MediaServer",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_MediaServer_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"PurchasedPPV",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_PurchasedPPV_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"ISDBC",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ISDBC_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"Internet",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_Internet_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"USB-HDD",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_USB_HDD_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"LAN-HDD",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_LAN_HDD_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"ClientEquipment",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_JCOM_Config_ClientEquipmentNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ClientEquipment_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_JCOM_ClientEquipmentNumberOfEntries},
	{"RemoteEquipment",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_JCOM_Config_RemoteEquipmentNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_RemoteEquipment_ObjectForm, TRDMType_S, eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries},
	{"HomeServer",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_HomeServer_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
const static TRPORT_OBJECT_FORMAT_t	s_STBService_ObjectForm[] =
{
	{"Capabilities",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_STBService_Capabilities_ObjectForm, TRDMType_M, eTRXDMInstance_None},
	{"Components",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Components_ObjectForm, TRDMType_M, eTRXDMInstance_None},
	{"AVStreams",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_AVStreams_ObjectForm, TRDMType_M, eTRXDMInstance_None},
	{"AVPlayers",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_AVPlayers_ObjectForm, TRDMType_M, eTRXDMInstance_None},
	{"X_JCOM_Config",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Config_ObjectForm, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

#define ___TR140___

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.StorageService.{i}.Capabilities.
/*
	FTPCapable	boolean
	SFTPCapable	boolean
	HTTPCapable	boolean
	HTTPSCapable	boolean
	HTTPWritable	boolean
	SupportedNetworkProtocols	string
	SupportedFileSystemTypes	string
	VolumeEncryptionCapable	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_Capabilities_ObjectForm[] =
{
	{"FTPCapable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_FTPCapable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SFTPCapable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_SFTPCapable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HTTPCapable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_HTTPCapable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HTTPSCapable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_HTTPSCapable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"HTTPWritable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_HTTPWritable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SupportedNetworkProtocols",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_SupportedNetworkProtocols, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SupportedFileSystemTypes",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_SupportedFileSystemTypes, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"VolumeEncryptionCapable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Capa_VolumeEncryptionCapable, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.StorageService.{i}.NetInfo.
/*
	HostName	string(64)	W
	DomainName	string(255)	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_NetInfo_ObjectForm[] =
{
	{"HostName",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP140_Get_StorageSvc_M_NetInfo_HostName, (pfn_setCB_t)TRP140_Set_StorageSvc_M_NetInfo_HostName, NULL, TRDMType_S, eTRXDMInstance_None},
	{"DomainName",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP140_Get_StorageSvc_M_NetInfo_DomainName, (pfn_setCB_t)TRP140_Set_StorageSvc_M_NetInfo_DomainName, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1	// Not Supported
//Device.Services.StorageService.{i}.NetworkServer.
/*
	AFPEnable	boolean	W
	NFSEnable	boolean	W
	SMBEnable	boolean	W
	NetworkProtocolAuthReq	boolean	W
*/
//NOT SUPPORTED
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_NetworkServer_ObjectForm[] =
{
	{"AFPEnable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_NetworkServer_AFPEnable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_NetworkServer_AFPEnable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"NFSEnable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_NetworkServer_NFSEnable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_NetworkServer_NFSEnable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SMBEnable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_NetworkServer_SMBEnable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_NetworkServer_SMBEnable, NULL, TRDMType_S, eTRXDMInstance_None},
	{"NetworkProtocolAuthReq",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq, (pfn_setCB_t)TRP140_Set_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq, NULL, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.StorageService.{i}.PhysicalMedium.{i}.
/*
	Name	string(64)	W
	Vendor	string(64)
	Model	string(128)
	SerialNumber	string(64)
	--> Not Supported : FirmwareVersion	string(64)
	ConnectionType	string
	Removable	boolean
	Capacity	unsignedInt
	Status	string ??
	--> Not Supported : Uptime	unsignedInt
	--> Not Supported : SMARTCapable	boolean
	--> Not Supported : Health	string
	HotSwappable	boolean
	X_JCOM_PowerSupply-Interlocking	boolean

*/

const static TRPORT_OBJECT_FORMAT_t	s_StorageService_PhysicalMedium_ObjectForm[] =
{
	{"Name",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Name, (pfn_setCB_t)TRP140_Set_StorageSvc_M_PhysicalMedium_M_Name, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Vendor",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Vendor, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Model",		eHLIB_JSON_String, SIZE_128, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Model, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SerialNumber",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_SerialNumber, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"FirmwareVersion",		eHLIB_JSON_String, 2, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_FirmwareVersion, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"ConnectionType",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_ConnectionType, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Removable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Removable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Capacity",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Capacity, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Uptime",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Uptime, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SMARTCapable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_SMARTCapable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Health",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_Health, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"HotSwappable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_HotSwappable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"X_JCOM_PowerSupply-Interlocking",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMedium_M_JCOM_PowerSupply_Interlocking, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
#if 1	// Not Supported
//Device.Services.StorageService.{i}.LogicalVolume.{i}.Folder.{i}.
/*
	Name	string(64)	W
	Enable	boolean	W
	UserAccountAccess	unsignedInt[0:3]	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_LogicalVolume_Folder_ObjectForm[] =
{
	{"Name",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_Name, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_Name, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_Enable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"UserAccountAccess",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess, NULL, TRDMType_M, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.StorageService.{i}.LogicalVolume.{i}.
/*
	Name	string(64)	W
	Status	string
	/////Enable	boolean	W
	PhysicalReference	string(256)	W
	FileSystem	string
	Capacity	unsignedInt	W
	UsedSpace	unsignedInt
	Encrypted	boolean
	FolderNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_LogicalVolume_ObjectForm[] =
{
	{"Name",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Name, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_Name, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Enable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"PhysicalReference",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_PhysicalReference, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_PhysicalReference, NULL, TRDMType_M, eTRXDMInstance_None},
	{"FileSystem",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_FileSystem, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Capacity",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Capacity, (pfn_setCB_t)TRP140_Set_StorageSvc_M_LogicalVolume_M_Capacity, NULL, TRDMType_M, eTRXDMInstance_None},
	{"UsedSpace",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_UsedSpace, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Encrypted",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_Encrypted, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"FolderNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_FolderNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Folder",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolume_M_FolderNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_LogicalVolume_Folder_ObjectForm, TRDMType_S, eTR140A1_DSStorageSvc_LogicalVolume_M_FolderNumberOfEntries},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Device.Services.StorageService.{i}.X_JCOM_USB-HDD.{i}.Diagnostics.	object
/*
	DiagnosticsState	string	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_X_JCOM_USB_HDD_Diagnostics_ObjectForm[] =
{
	{"DiagnosticsState",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState, (pfn_setCB_t)TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState, NULL, TRDMType_S, eTRXDMInstance_None},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.StorageService.{i}.X_JCOM_USB-HDD.{i}.	object
/*
	RegistrationEnable	boolean	W
	USB-HDDEnable	boolean	W
	Status	string	W
	Name	String(64)	W
	Initialization	boolean	W
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_X_JCOM_USB_HDD_ObjectForm[] =
{
	{"RegistrationEnable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"USB-HDDEnable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Status, (pfn_setCB_t)TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Status, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Name, (pfn_setCB_t)TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Name, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Initialization",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Initialization, (pfn_setCB_t)TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Initialization, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Diagnostics",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_X_JCOM_USB_HDD_Diagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.StorageService.{i}.
/*
	//////Enable	boolean	W
	PhysicalMediumNumberOfEntries	unsignedInt
	LogicalVolumeNumberOfEntries	unsignedInt
	USB-HDDNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_ObjectForm[] =
{
	{"Enable",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_Enable, (pfn_setCB_t)TRP140_Set_StorageSvc_M_Enable, NULL, TRDMType_M, eTRXDMInstance_None},
	{"PhysicalMediumNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMediumNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LogicalVolumeNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolumeNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"X_JCOM_USB-HDDNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_StorageSvc_M_USB_HDDNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Capabilities",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_Capabilities_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"NetInfo",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_NetInfo_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"NetworkServer",		eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_NetworkServer_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"PhysicalMedium",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP140_Get_StorageSvc_M_PhysicalMediumNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_PhysicalMedium_ObjectForm, TRDMType_S, eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries},
	{"LogicalVolume",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP140_Get_StorageSvc_M_LogicalVolumeNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_LogicalVolume_ObjectForm, TRDMType_S, eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries},
	{"X_JCOM_USB-HDD",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP140_Get_StorageSvc_M_USB_HDDNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_X_JCOM_USB_HDD_ObjectForm, TRDMType_S, eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

/*
Device.Services.	object
STBServiceNumberOfEntries	unsignedInt
StorageServiceNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Service_ObjectForm[] =
{
	{"STBServiceNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP135_Get_Service_STBServiceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"StorageServiceNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP140_Get_Service_StorageServiceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"STBService",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP135_Get_Service_STBServiceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_STBService_ObjectForm, TRDMType_S, eTR135A1_Device_Services_STBServiceNumberOfEntries},
	{"StorageService",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP140_Get_Service_StorageServiceNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_ObjectForm, TRDMType_S, eTR140A1_Device_Services_StorageServiceNumberOfEntries},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

STATIC HINT32 nFrontEndIndex;

//Device.WiFi.SSID.{i}.
/*
	Enable boolean
	Status String
	Alias String64
	Name string64
	LastChange unsignedInt
	LowerLayers string­(1024)
	BSSID string(17)
	MACAddress string(17)
	SSID string(32)
*/

const static TRPORT_OBJECT_FORMAT_t	s_SSID_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_Enable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Alias", 		eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_Alias, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Name", 		eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_Name, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"LastChange", 		eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_LastChange, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
    {"LowerLayers", 		eHLIB_JSON_String, SIZE_1024,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_LowerLayers, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
    {"BSSID", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_BSSID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MACAddress", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_MACAddress, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SSID", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_SSID, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},

// CVC 확장 구현
	{"fqdn", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_fgdn, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"frequency", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_frequency, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"frequencyunits", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_frequencyunits, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"level", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_level, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"linkspeedunits", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_linkspeedunits, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"networkid", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_networkid, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"rssi", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_rssi, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"hiddenssid", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_hiddenssid, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"supplicantstate", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_supplicantstate, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},


#ifdef CONFIG_DEBUG
	//TEST Function
	{"Test", eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_Test, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
#endif
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};

/*
Device.WiFi.AccessPoint.{i}.Security.
Reset	boolean
ModesSupported	string
ModeEnabled	string
WEPKey	hexBinary(5:5, 13:13)
PreSharedKey	hexBinary(32)
KeyPassphrase	string(8:63)
RekeyingInterval	unsignedInt
RadiusServerIPAddr	string(45)
SecondaryRadiusServerIPAddr	string(45)
RadiusServerPort	unsignedInt
SecondaryRadiusServerPort	unsignedInt
RadiusSecret	string
SecondaryRadiusSecret	string
*/

const static TRPORT_OBJECT_FORMAT_t	s_AccessPoint_Security_ObjectForm[] =
{
	{"Reset",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_Reset, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ModesSupported",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_ModesSupported, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"ModeEnabled",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_ModeEnabled, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)NULL, TRDMType_S, eTRXDMInstance_None},
	{"WEPKey",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_WEPKey, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},

	{"PreSharedKey",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_PreSharedKey, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"KeyPassphrase",		eHLIB_JSON_String, SIZE_1024, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_KeyPassphrase, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RekeyingInterval",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_RekeyingInterval, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RadiusServerIPAddr",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_RadiusServerIPAddr, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SecondaryRadiusServerIPAddr",		eHLIB_JSON_String, SIZE_64, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_WEPKey, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RadiusServerPort",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_RadiusServerPort, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SecondaryRadiusServerPort",		eHLIB_JSON_String, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusServerPort, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"RadiusSecret",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_RadiusSecret, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SecondaryRadiusSecret",		eHLIB_JSON_String, SIZE_256, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusSecret, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


/*
Device.WiFi.AccessPoint.{i}.
Enable	boolean
Status	string
Alias		string(64)
SSIDReference	string(256)
SSIDAdvertisementEnabled	boolean
RetryLimit	unsignedInt­[0:7]
WMMCapability	boolean
UAPSDCapability	boolean
WMMEnable	boolean
APSDEnable	boolean
AssociatedDeviceNumberOfEntries	unsignedInt
MaxAssociatedDevices	unsignedInt
IsolationEnable	boolean

Device.WiFi.AccessPoint.{i}.Security.

*/
const static TRPORT_OBJECT_FORMAT_t	s_AccessPoint_ObjectForm[] =
{
	{"Enable",	eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Enable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Status",	eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Status, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"Alias", 		eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_Alias, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},

	{"SSIDReference", 		eHLIB_JSON_String, SIZE_64,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_SSIDReference, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"SSIDAdvertisementEnabled", 		eHLIB_JSON_String, SIZE_256,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_SSIDAdvertisementEnabled, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
    {"RetryLimit", 		eHLIB_JSON_String, SIZE_1024,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_RetryLimit, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
    {"WMMCapability", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_WMMCapability, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"UAPSDCapability", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_UAPSDCapability, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"WMMEnable", 		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_WMMEnable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"UAPSDEnable",		eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_UAPSDEnable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"AssociatedDeviceNumberOfEntries", 	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_AssociatedDeviceNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"MaxAssociatedDevices", 	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_MaxAssociatedDevices, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
	{"IsolationEnable",	eHLIB_JSON_String, SIZE_32,  (pfn_getCB_t)TR181_get_Device_Wifi_AccessPoint_IsolationEnable, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},

	{"Security", eHLIB_JSON_Object, SIZE_32,  (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_DeviceInfo_ObjectForm, TRDMType_S, eTRXDMInstance_None},

#ifdef CONFIG_DEBUG
	//TEST Function
	{"Test", eHLIB_JSON_String, SIZE_16,  (pfn_getCB_t)TR181_get_Device_Wifi_SSID_Test, (pfn_setCB_t)NULL, NULL, TRDMType_M, eTRXDMInstance_None},
#endif
	{NULL, 				eHLIB_JSON_NULL,   0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


/*
Wifi.SSIDNumberOfEntries

*/

const static TRPORT_OBJECT_FORMAT_t	s_Device_Wifi_ObjectForm[] =
{
	{"RadioNumberOfEntries", eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_RadioNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SSIDNumberOfEntries", eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_SSIDNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"AccessPointNumberOfEntries", eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPointNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"EndPointNumberOfEntries", eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TR181_get_Device_Wifi_EndPointNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},

	{"SSID",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TR181_get_Device_Wifi_SSIDNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SSID_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"AccessPoint",		eHLIB_JSON_Array, 0, (pfn_getCB_t)TR181_get_Device_Wifi_AccessPointNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SSID_ObjectForm, TRDMType_S, eTR181A1_Device_WiFi_AccessPointNumberOfEntries},
	{NULL,				eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


//+ Object
//- Parameter
/*
Device.
- DeviceSummary
SmartCardReaderNumberOfEntries	unsignedInt
+ DeviceInfo
+ ManagementServer
+ LAN
+ X_JLABS_Management
*/
const static TRPORT_OBJECT_FORMAT_t	s_Device_ObjectForm[] =
{
	{"DeviceSummary", 		eHLIB_JSON_String, 32, (pfn_getCB_t)TRP_Get_Device_DeviceSummary, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"SmartCardReaderNumberOfEntries",		eHLIB_JSON_Integer, SIZE_16, (pfn_getCB_t)TRP157_Get_SmartCardReaderNumberOfEntries, (pfn_setCB_t)NULL, NULL, TRDMType_S, eTRXDMInstance_None},
	{"Services",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Service_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"DeviceInfo", 			eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_DeviceInfo_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{"ManagementServer", 	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_ManagmentServer_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"LAN", 				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_LAN_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"X_JLABS_Management",	eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JLABS_Management_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{"DownloadDiagnostics",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_DownloadDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"UploadDiagnostics",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UploadDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{"UPnP",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_UPnP_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"DLNA",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_DLNA_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"SmartCardReader",				eHLIB_JSON_Array, 0, (pfn_getCB_t)TRP157_Get_SmartCardReaderNumberOfEntries, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SmartCardReader_ObjectForm, TRDMType_S, eTR157A3_Device_SmartCardReaderNumberOfEntries},
	{"SelfTestDiagnostics",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SelfTestDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"NSLookupDiagnostics",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_NSLookupDiagnostics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"USBHosts",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_USBHosts_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"PeriodicStatistics",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_PeriodicStatistics_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_HomeNetwork",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_HomeNetwork_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"X_JCOM_Wifi",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JCOM_Wifi_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"SoftwareModules",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_SoftwareModules_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"Capabilities",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_ObjectForm, TRDMType_S, eTRXDMInstance_None},
	{"WiFi",				eHLIB_JSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Device_Wifi_ObjectForm, TRDMType_S, eTRXDMInstance_None},

	{NULL, eHLIB_JSON_NULL, 0, NULL, NULL, NULL, TRDMType_S, eTRXDMInstance_None},
};


#define __________STATIC_FUNCTION________________________________________


static const HCHAR* rema_cwmp_json_next_token (HCHAR *token, HINT32 n, const HCHAR *p)
{
	if (*p == '\0')
		return NULL;

	if (*p == '.')
		p++;

	while (n-- && *p != '\0' && *p != '.')
	{
		*token++ = *p++;
	}

	if (n < 0)
		return NULL;

	*token = '\0';

	return p;
}

//입력 object전체의 {parameter:value} pair를 json구조에 추가한다.
static HERROR rema_cwmp_add_object2json(HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat, HCHAR *token, HINT32 nEntryCount)
{
	HERROR hErr = ERR_OK;
	HxJSON_t subjson;
	HCHAR *pszstr = NULL;
	HUINT32 retVal;
	HBOOL	bNewInstance = TRUE;
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	REM_ENTER;
	CHECK_NULL(token);

	RemLOG_Print("[1] #### %s ####\r\n",__func__);

	objectloop = objectFormat;

	if(objectloop->ElementType == eHLIB_JSON_Array)
	{
		subjson = HxJSON_Array_New();
	}
	else
	{
		subjson = HxJSON_Object_New();

		if(objectloop->eDMType == TRDMType_M)
		{
			bNewInstance = FALSE;
		}
		else
		{
			//do nothing
		}

	}
	if (NULL == subjson)
	{
		RemLOG_Print("return %d\r\n",__LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	RemLOG_Print("[3] nEntry(%d),token(%s))\r\n",nEntryCount,token);

	while(objectloop->ElementName)
	{
		RemLOG_Print("[4] ElementName(%s),ElementType(%d),eDMType(%d)\r\n",objectloop->ElementName,objectloop->ElementType,objectloop->eDMType);
		switch(objectloop->ElementType)
		{
			case eHLIB_JSON_Object:
				if(objectloop->s_objectform != NULL)
				{
					RemLOG_Print("################ ENTER AA Point\r\n");


					//아래, " [ FRONT-END EXCEPTION CASE ] " 주석 부분 참고.
					if(objectloop->eTRXDMInstancetype == eTRXDMInstance_Exception_ISDBC)
					{
						if(nFrontEndIndex < 2)
						{
							RemLOG_Print("Only use from second item\n");
							break;
						}
					}
					else if(objectloop->eTRXDMInstancetype == eTRXDMInstance_Exception_Docsis)
					{
						if(nFrontEndIndex > 1)
						{
							RemLOG_Print("Only use first item \n");
							break;
						}
					}


					hErr |= rema_cwmp_add_object2json(subjson, objectloop->s_objectform, objectloop->ElementName,0);
				}
				else
				{
					RemLOG_Print("s_objectform is not registered! %s\n",objectloop->ElementName);
				}
				break;

			case eHLIB_JSON_String:
				if(objectloop->pfnCbGetData != NULL)
				{
					pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
					HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
					if(pszstr != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HCHAR*)pszstr);
						RemLOG_Print("token : [%s],  objectloop->ElementName [%s], value : [%s]\n",token,  objectloop->ElementName, pszstr);
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(pszstr));

						#ifdef CONFIG_DEBUG
						PRINT_JSON(subjson);
						#endif
						RemLOG_Print("[5] @@@@@ Do Json set \r\n");
						HLIB_STD_MemFree(pszstr);
					}
				}
				else
				{
					RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			case eHLIB_JSON_Integer:

				if(objectloop->pfnCbGetData != NULL)
				{
					pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
					HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
					if(pszstr != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);
						RemLOG_Print("Integet retVal(%d)\r\n",retVal);
						HxSTD_PrintToStr(pszstr,"%d",retVal);

						//Data Model의 Integer Data Type은 실제 String으로 처리함
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(pszstr));
						HLIB_STD_MemFree(pszstr);
					}
				}
				else
				{
					RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			case eHLIB_JSON_TRUE:
			case eHLIB_JSON_FALSE:
				if(objectloop->pfnCbGetData != NULL)
				{
					pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
					HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
					if(pszstr != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HCHAR*)pszstr);
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(pszstr));
						HLIB_STD_MemFree(pszstr);
					}
				}
				else
				{
					RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
				}
				break;
			case eHLIB_JSON_Array:
				if((objectloop->s_objectform != NULL) && (objectloop->pfnCbGetData != NULL))
				{
					HxJSON_t subsubjson;
					HINT32 i;
					HCHAR	*szName = NULL;
					HUINT32 nameLen		= 0;
					retVal = 0;
					subsubjson = HxJSON_Array_New();
					hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);

					RemLOG_Print("%s, ElementName[%s], Entry(%d)\n",__func__,objectloop->ElementName,retVal);

					nameLen = HxSTD_StrLen(objectloop->ElementName);
					szName	= HLIB_STD_MemAlloc(nameLen+1);
					if(szName == NULL)
					{
						hErr = ERR_FAIL;
						goto EXIT_FUNC;
					}
					
					HxSTD_MemSet(szName, 0x0, nameLen+1);
					HxSTD_MemCopy(szName, objectloop->ElementName, nameLen);

					rema_DM_MultiInstance_SetEntry(objectloop->eTRXDMInstancetype,retVal);

					if(retVal > 0)
					{
						 //Multi 의 초기 값은 '1'
						for(i=1; i<= retVal; i++)
						{
							HxJSON_t element;
							element = HxJSON_Object_New();

							rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype,i);

							hErr |= HxJSON_Object_Set(element, "index", HxJSON_Integer_New(i));

							if(objectloop->eTRXDMInstancetype == eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries)
							{
								/*
								[ FRONT-END EXCEPTION CASE ]
									- J:COM에서 제공한 Data Model Sheet의 해석을 달리한 P사때문에 아래와 같은 예외처리가 필요함
									- FrontEnd는 현재 4개
									- 1번째는 무조건 DOCSIS, 그리고, 나머지는 QAM 으로 올려달라는 요구사항이 있었음

									1. DOCSIS
									2. QAM 1
									3. QAM 2
									4. QAM 3
								*/
								RemLOG_Print("FRONT-END EXCEPTION CASE.....\n");
								nFrontEndIndex = i;
							}
							//RemLOG_Print("################ ENTER BB Point\r\n");

							hErr |= rema_cwmp_add_object2json(element, objectloop->s_objectform,token, i);

							HLIB_JSON_Array_Append(subsubjson, element);
						}

						#ifdef CONFIG_DEBUG
						{
							RemLOG_Print("@@@@@@ After process multi-instance. retVal(%d)\r\n",retVal);
							RemLOG_Print("@@@@@@ ElementName(%s),szName(%s)\r\n",objectloop->ElementName,szName);
							RemLOG_Print("====================================================================================\r\n");
							PRINT_JSON(subsubjson);
							RemLOG_Print("====================================================================================\r\n");
						}
						#endif

						hErr |= HxJSON_Object_Set(subjson, szName, subsubjson);
					}
					else
					{
						RemLOG_Print("Empty Array\n");
						 //Multi 지만, 아무런 element가 없는 경우 빈값 []  으로 채움.
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, subsubjson);
					}

					if(szName)
					{
						HLIB_STD_MemFree(szName);
						szName = NULL;
					}
				}
				else
				{
					RemLOG_Print("s_objectform is not registered! %s\n",objectloop->ElementName);
				}

				break;
			default:
				hErr = ERR_FAIL;
				RemLOG_Print("Not Support!! name:%s, type:%d\n", objectloop->ElementName, objectloop->ElementType);
				break;
		}

		objectloop++;
	}

EXIT_FUNC :
	if(bNewInstance == TRUE)
		hErr |= HxJSON_Object_Set(json, token, subjson);
	else
		hErr |= HLIB_JSON_Object_Update(json, subjson);

	if (hErr != ERR_OK)
	{
		HLIB_JSON_Delete(subjson);
		subjson = NULL;
	}
	REM_LEAVE(hErr);
}


//key값에 해당하는 parameter를 json구조로 만들어서 return한다.
//key값이 object이면서 .으로 긑나면 object내의 모든 {parameter:value} pair를 json구조로 만든다.
//key값이 단일 parameter일 경우 parameter에 해당하는 {parameter:value} pair를 json 구조로 만든다.
//key값 example
//ex> "Device.ManagementServer."
//ex> "Device.Lan.IPAddress"
static HERROR rema_cwmp_build_json_using_key(HCHAR *key, HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat)
{
	HERROR		hErr =  ERR_OK;
	HCHAR 		token[LENGTH_OF_DM_TOKEN];
	HCHAR 		*pszstr;
	HBOOL		bfind = FALSE;
	HUINT32		retVal = 0;
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	CHECK_NULL(key && objectFormat);

	RemLOG_Print("#### %s ####\r\n",__func__);

	key = (HCHAR*)rema_cwmp_json_next_token(token, LENGTH_OF_DM_TOKEN, key);
	CHECK_NULL(key);
	RemLOG_Print("[token:%s][key:%s] strlen:%d\n", token, key, strlen(key));
	objectloop = objectFormat;

	while(objectloop->ElementName)
	{
		bfind = FALSE;
		if(HxSTD_StrCmp(token, objectloop->ElementName) == 0)
		{
			RemLOG_Print("ElementName[%s], ElementType[%d]\n",objectloop->ElementName, objectloop->ElementType);
			bfind = TRUE;
			switch(objectloop->ElementType)
			{
				case eHLIB_JSON_Object:
					if(objectloop->s_objectform == NULL)
					{
						RemLOG_Print("s_objectform is NULL! [%s]", token);
						REM_LEAVE(ERR_FAIL);
					}

					if(*key=='.' && strlen(key)==1)//마지막 글자가 .일 경우 object로 return된다. 하위 element들을 모두 추가한다.
					{
						RemLOG_Print("json type(%d)\r\n",HLIB_JSON_TypeOf(json));
						//RemLOG_Print("################ ENTER A Point\r\n");
						hErr |= rema_cwmp_add_object2json(json, objectloop->s_objectform, token, 0);
					}
					else
					{
						hErr |= HxJSON_Object_Set(json, token, HxJSON_Object_New());
						json  = HxJSON_Object_Get(json, token);
						hErr |= rema_cwmp_build_json_using_key(key, json, objectloop->s_objectform);
					}
					break;

				case eHLIB_JSON_String:
					if(objectloop->pfnCbGetData != NULL)
					{
						pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
						HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
						if(pszstr != NULL)
						{
							hErr |= objectloop->pfnCbGetData((HCHAR*)pszstr);
							RemLOG_Print("token : [%s], value : [%s]\n",token, pszstr);
							hErr |= HxJSON_Object_Set(json, token, HxJSON_String_New(pszstr));
							HLIB_STD_MemFree(pszstr);
						}
					}
					else
					{
						RemLOG_Print("pfnCbGetData was not registered! %s\n",objectloop->ElementName);
						REM_LEAVE(ERR_FAIL);
					}
					break;

				case eHLIB_JSON_Integer:
					if(objectloop->pfnCbGetData != NULL)
					{
						pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
						HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
						if(pszstr != NULL)
						{
							hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);
							HxSTD_PrintToStr(pszstr,"%d",retVal);

							//Data Model의 Integer Data Type은 실제 String으로 처리함
							hErr |= HxJSON_Object_Set(json, token, HxJSON_String_New(pszstr));
							HLIB_STD_MemFree(pszstr);
						}
					}
					else
					{
						RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
					}

					break;

				case eHLIB_JSON_Array:
					{
						if(objectloop->s_objectform == NULL)
						{
							RemLOG_Print("s_objectform is NULL! [%s]", token);
							REM_LEAVE(ERR_FAIL);
						}

						if(*key=='.' && strlen(key)==1)//마지막 글자가 .일 경우 object로 return된다. 하위 element들을 모두 추가한다.
						{
							HxJSON_t subjson;
							HINT32 i;
							subjson = HxJSON_Array_New();
							RemLOG_Print("json type(%d)\r\n",HLIB_JSON_TypeOf(json));

							hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);
							RemLOG_Print("ElementName[%s], Entry(%d)\n", objectloop->ElementName,retVal);

							for(i=1; i<= retVal; i++)
							{
								HxJSON_t element;
								element = HxJSON_Object_New();

								hErr |= HxJSON_Object_Set(element, "index", HxJSON_Integer_New(i));

								//RemLOG_Print("################ ENTER B Point\r\n");
								rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype,i);


								if(objectloop->eTRXDMInstancetype == eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries)
								{
									/*
									[ FRONT-END EXCEPTION CASE ]
										- J:COM에서 제공한 Data Model Sheet의 해석을 달리한 P사때문에 아래와 같은 예외처리가 필요함
										- FrontEnd는 현재 4개
										- 1번째는 무조건 DOCSIS, 그리고, 나머지는 QAM 으로 올려달라는 요구사항이 있었음

										1. DOCSIS
										2. QAM 1
										3. QAM 2
										4. QAM 3
									*/
									RemLOG_Print("FRONT-END EXCEPTION CASE.....\n");
									nFrontEndIndex = i;
								}
								hErr |= rema_cwmp_add_object2json(element, objectloop->s_objectform,token, i);
								HLIB_JSON_Array_Append(subjson, element);
							}
							hErr |= HxJSON_Object_Set(json, token, subjson);
						}
						else
						{
							// 처리 대상 : Device.SmartCardReader.1.SmartCard.X_JCOM_CardType
							{
								HCHAR	szCnt[3] = {0,};
								HINT32	nCnt = 0;
								HINT32	nAdjust = 0, i = 0;
								HCHAR	szNewKey[256] = {0,};

								if(objectloop->s_objectform == NULL)
								{
									RemLOG_Print("s_objectform is NULL! [%s]", token);
									REM_LEAVE(ERR_FAIL);
								}
								if(szNewKey == NULL)
								{
									RemLOG_Print("newKey is NULL! [%s]", token);
									REM_LEAVE(ERR_FAIL);
								}

								if(key[2] == '.')	// 1 자릿수 Index
								{
									nAdjust = 2;
									HxSTD_PrintToStr(szCnt,"%c",key[1]);
								}
								else if(key[3] == '.')	// 2 자릿수 Index
								{
									nAdjust = 3;
									HxSTD_PrintToStr(szCnt,"%c%c",key[1],key[2]);
								}
								else
								{
									RemLOG_Print("%s/%d skip to process array\r\n",__func__,__LINE__);

									hErr |= HxJSON_Object_Set(json, token, HxJSON_Object_New());
									json  = HxJSON_Object_Get(json, token);
									hErr |= rema_cwmp_build_json_using_key(key, json, objectloop->s_objectform);
									break;
								}

								nCnt = PARAM_ATOI(szCnt);
								hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);

								if(nCnt <= retVal && nCnt > 0)
								{
									i = 0;

									// Index를 제거하고 Recursive로 보낸다. (ex: .2.SmartCard.X_JCOM_CardType ==> .SmartCard.X_JCOM_CardType)
									while(1)
									{
										if(key[i] == '\0')	break;

										szNewKey[i] = key[i+nAdjust];

										i++;
									}
									if(szNewKey[0] != '\0')
									{
										HxJSON_t subjson;
										HxJSON_t element;

										subjson = HxJSON_Array_New();
										element = HxJSON_Object_New();

										hErr |= HxJSON_Object_Set(element, "index", HxJSON_Integer_New(nCnt));

										rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype, nCnt);

										hErr |= rema_cwmp_build_json_using_key(szNewKey, element, objectloop->s_objectform);
										HLIB_JSON_Array_Append(subjson, element);
										hErr |= HxJSON_Object_Set(json, token, subjson);
									}
								}
							}
						}
					}
					break;

				default:
					RemLOG_Print("Not Support!! token:[%s], type:[%d]\n", token, objectloop->ElementType);
					hErr = ERR_FAIL;
					break;
			}
			break;
		}

		objectloop++;
	}

	if(bfind == FALSE)
	{
		RemLOG_Print("key does not belong to DataModel! [%s]\n", key);
	}

	REM_LEAVE(hErr);
}




//입력받은 json구조에 존재하는 {parameter:value} pair를 이용하여, db값을 수정한다.
static HERROR rema_cwmp_revise_data_using_json(HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat)
{
	const HCHAR *token;
	HERROR		hErr =  ERR_OK;
	void		*iter;
	HCHAR		*pszstr;
	HxJSON_t 	value;
	//HxJSON_t 	_value;
	HBOOL		bfind = FALSE;
	const TRPORT_OBJECT_FORMAT_t *objectloop;
	HINT32 nEntry = 0;
	HINT32 *pEntry = NULL;

	CHECK_NULL(json && objectFormat);

	iter = HLIB_JSON_Object_Iterator(json);
	PRINT_JSON(json);

	if(!iter){
		//check array iterator
		objectloop = objectFormat;
		if(HxJSON_IsArray(json) && (objectloop->ElementType == eHLIB_JSON_Array) && (objectloop->s_objectform != NULL)
				&& (HLIB_JSON_GetSize(json) != 0))
		{
			void		*_iter;
			void		*_iter_index;
			const HCHAR *_index_token;
			const HCHAR *_token;
			HxJSON_t 	_value;
			HxJSON_t 	_value2;
			HINT32		nIndex;
			HxJSON_t 	array_value;
			HxJSON_t 	element;
			TRPORT_OBJECT_FORMAT_t *pSubObjectloop = objectloop->s_objectform;

			HxJSON_ARRAY_EACH(json, array_value, {

				/* For Index : N */
				PRINT_JSON(array_value);
				_iter_index = HLIB_JSON_Object_IteratorAt(array_value,"index");
				if(!_iter_index){
					//It is not [{"index":1,"Key":"Value"}] or [{"Key":"Value","index":1}]
					RemLOG_Print("\n####### _iter_index NULL\n");
					REM_LEAVE(hErr);
				}
				_iter = HLIB_JSON_Object_Iterator(array_value);
				_index_token = HLIB_JSON_Object_IteratorKey(_iter);
				_value = HLIB_JSON_Object_IteratorValue(_iter);


				if((HxSTD_StrCmp(_index_token,"Name")==0) && _iter_index)
				{
				/*
					If the Key Array [{"index": 1, "Name": "Value"}] is passed,
					there is a bug that find the first token is the 'Name' not as 'index' in the parsing from the Json library, skipping 'index' key.
					[{"Name": "Value", "index": 1}], and non-key Name like name, enable have normal operation, so as to check the exception code is written.
				*/
					element = HLIB_JSON_Get(array_value, (void*)_index_token);
					nIndex = HxJSON_Integer_Get(element);
					rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype,nIndex);
					_token = _index_token;
					while(pSubObjectloop->ElementName)
					{
						if(HxSTD_StrCmp(_token, pSubObjectloop->ElementName) == 0)
						{
							if((pSubObjectloop->ElementType == eHLIB_JSON_Object))
							{
								hErr = rema_cwmp_revise_data_using_json(_value, pSubObjectloop->s_objectform);
								if(hErr != ERR_OK)
								{
									RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
									REM_LEAVE(hErr);
								}
							}
							else if((pSubObjectloop->ElementType == eHLIB_JSON_String))
							{
								/* Process None Object */
								if(pSubObjectloop->pfnCbSetData == NULL)
								{
									RemLOG_Print("pSubObjectloop was not registered! %s\n",pSubObjectloop->ElementName);
									break;
								}
								pszstr = (HCHAR *)HxJSON_String_Get(_value);
								RemLOG_Print("\n####### Array eHLIB_JSON_String\n");
								hErr = pSubObjectloop->pfnCbSetData((HCHAR*)pszstr);
								if(hErr != ERR_OK)
								{
									RemLOG_Print("pSubObjectloop returns Error! ElementName=%s [%s:%d]\n",pSubObjectloop->ElementName, __FUNCTION__, __LINE__);
									REM_LEAVE(hErr);
								}
							}
							else if((pSubObjectloop->ElementType == eHLIB_JSON_Array))
							{
								hErr = rema_cwmp_revise_data_using_json(_value, pSubObjectloop);
								if(hErr != ERR_OK)
								{
									RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
									REM_LEAVE(hErr);
								}
							}
							else
							{
								RemLOG_Print("else.. what should i do?\n");
							}
						}
						pSubObjectloop++;
					}
					REM_LEAVE(hErr);
				}

				if(HxSTD_StrCmp(_index_token,"index") == 0)
				{
					element = HLIB_JSON_Get(array_value, (void*)_index_token);
					nIndex = HxJSON_Integer_Get(element);
					rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype,nIndex);

					/* For Member in JSON*/
					_iter = HLIB_JSON_Object_NextIterator(array_value, _iter);
					_token = HLIB_JSON_Object_IteratorKey(_iter);
					_value2 = HLIB_JSON_Object_IteratorValue(_iter);

					while(pSubObjectloop->ElementName)
					{
//						RemLOG_Print("Array ElementName %s\n",__FUNCTION__, pSubObjectloop->ElementName);
						if(HxSTD_StrCmp(_token, pSubObjectloop->ElementName) == 0)
						{
							if((pSubObjectloop->ElementType == eHLIB_JSON_Object))
							{
								hErr = rema_cwmp_revise_data_using_json(_value2, pSubObjectloop->s_objectform);
								if(hErr != ERR_OK)
								{
									RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
									REM_LEAVE(hErr);
								}
							}
							else if((pSubObjectloop->ElementType == eHLIB_JSON_String))
							{
								/* Process None Object */
								if(pSubObjectloop->pfnCbSetData == NULL)
								{
									RemLOG_Print("pSubObjectloop was not registered! %s\n",pSubObjectloop->ElementName);
									break;
								}
								if(pSubObjectloop->s_objectform)
								{
									pszstr = (HCHAR *)HxJSON_String_Get(_value);
								}
								else
								{
									pszstr = (HCHAR *)HxJSON_String_Get(_value2);
								}
								RemLOG_Print("Array token : %s, value : %s\n",_token, pszstr);
								hErr = pSubObjectloop->pfnCbSetData((HCHAR*)pszstr);
								if(hErr != ERR_OK)
								{
									RemLOG_Print("pSubObjectloop returns Error! ElementName=%s [%s:%d]\n",pSubObjectloop->ElementName, __FUNCTION__, __LINE__);
									REM_LEAVE(hErr);
								}
							}
							else if((pSubObjectloop->ElementType == eHLIB_JSON_Array))
							{
								hErr = rema_cwmp_revise_data_using_json(_value2, pSubObjectloop);
								if(hErr != ERR_OK)
								{
										RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
										REM_LEAVE(hErr);
								}
							}
							else
							{
								RemLOG_Print("else.. what should i do?\n");
							}
						}
						pSubObjectloop++;
					}
				}
			});
		}
	}

	while(iter)
	{
		token = HLIB_JSON_Object_IteratorKey(iter);
		value = HLIB_JSON_Object_IteratorValue(iter);
		RemLOG_Print("token : %s\n",token);

		bfind = FALSE;
		objectloop = objectFormat;
		while(objectloop->ElementName)
		{
			//RemLOG_Print("AAA : token(%s), objectloop->ElementName : %s\n",token, objectloop->ElementName);
			if(HxSTD_StrCmp(token, objectloop->ElementName) == 0)
			{
				bfind = TRUE;
				RemLOG_Print("ElementName[%-32s], HxJSON_TypeOf[%d], ElementType[%d]\n",objectloop->ElementName, HLIB_JSON_TypeOf(value), objectloop->ElementType);

				if(HxJSON_IsObject(value) && (objectloop->ElementType == eHLIB_JSON_Object) && (objectloop->s_objectform != NULL))
				{
					hErr = rema_cwmp_revise_data_using_json(value, objectloop->s_objectform);
					if(hErr != ERR_OK)
					{
						RemLOG_Print("rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
						REM_LEAVE(hErr);
					}
				}

				if(HxJSON_IsString(value) && (objectloop->ElementType == eHLIB_JSON_String))
				{
					if(objectloop->pfnCbSetData == NULL)
					{
						RemLOG_Print("pfnCbSetData was not registered! %s\n",objectloop->ElementName);
						break;
					}

					pszstr = (HCHAR *)HxJSON_String_Get(value);
					RemLOG_Print("token : %s, value : %s\n",token, pszstr);

					hErr = objectloop->pfnCbSetData((HCHAR*)pszstr);
					if(hErr != ERR_OK)
					{
						RemLOG_Print("pfnCbSetData returns Error! ElementName=%s [%s:%d]\n",objectloop->ElementName, __FUNCTION__, __LINE__);
						REM_LEAVE(hErr);
					}
				}
				if(HxJSON_IsInteger(value) && (objectloop->ElementType == eHLIB_JSON_Integer))
				{
					if(objectloop->pfnCbSetData == NULL)
					{
						RemLOG_Print("pfnCbSetData was not registered! %s\n",objectloop->ElementName);
						break;
					}

					pszstr = (HCHAR *)HxJSON_String_Get(value);
					RemLOG_Print("token : %s, pszstr : %s\n",token, pszstr);

					hErr = objectloop->pfnCbSetData(pszstr);
					if(hErr != ERR_OK)
					{
						RemLOG_Print("pfnCbSetData returns Error! ElementName=%s [%s:%d]\n",objectloop->ElementName, __FUNCTION__, __LINE__);
						REM_LEAVE(hErr);
					}
				}

				// Multi Instance Entry Process
				if(HxJSON_IsString(value) && (objectloop->ElementType == eHLIB_JSON_Integer))
				{
					if(objectloop->pfnCbSetData == NULL)
					{
						RemLOG_Print("pfnCbSetData was not registered! %s\n",objectloop->ElementName);
						break;
					}
					pszstr = (HCHAR *)HxJSON_String_Get(value);
					RemLOG_Print("token : %s, pszstr : %s\n",token, pszstr);

					nEntry = PARAM_ATOI(pszstr);
					pEntry = &nEntry;

					hErr = objectloop->pfnCbSetData(pEntry);
					if(hErr != ERR_OK)
					{
						RemLOG_Print("pfnCbSetData returns Error! ElementName=%s [%s:%d]\n",objectloop->ElementName, __FUNCTION__, __LINE__);
						REM_LEAVE(hErr);
					}

					//Entry Set 위치 변경. => Old Mem Free 후, Entry set
					rema_DM_MultiInstance_SetEntry(objectloop->eTRXDMInstancetype,nEntry);

				}

				if(HxJSON_IsArray(value) && (objectloop->ElementType == eHLIB_JSON_Array) && (objectloop->s_objectform != NULL)
					&& (HLIB_JSON_GetSize(value) != 0))
				{


					void		*_iter;
					const HCHAR *_index_token;
					const HCHAR *_token;
					HxJSON_t 	_value;
					HxJSON_t 	_value2;
					HINT32		nIndex;
					HxJSON_t 	array_value;
					HxJSON_t 	element;
					TRPORT_OBJECT_FORMAT_t *pSubObjectloop = objectloop->s_objectform;

					RemLOG_Print("# pSubObjectloop->ElementName(%s)\r\n",pSubObjectloop->ElementName);

					HxJSON_ARRAY_EACH(value, array_value, {

						/* For Index : N */
						_iter = HLIB_JSON_Object_Iterator(array_value);
						_index_token = HLIB_JSON_Object_IteratorKey(_iter);
						_value = HLIB_JSON_Object_IteratorValue(_iter);


						RemLOG_Print("_index_token : %s\n",_index_token);

						if(HxSTD_StrCmp(_index_token,"index") == 0)
						{

							element = HLIB_JSON_Get(array_value, (void*)_index_token);
							nIndex = HxJSON_Integer_Get(element);

							/* For Member in JSON*/
							_iter = HLIB_JSON_Object_NextIterator(array_value, _iter);
							_token = HLIB_JSON_Object_IteratorKey(_iter);
							_value2 = HLIB_JSON_Object_IteratorValue(_iter);

							RemLOG_Print("_token : %s\n",_token);

							rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype,nIndex);


							//if(pSubObjectloop->s_objectform) RemLOG_Print("pSubObjectloop->s_objectform->ElementName(%s)\r\n",pSubObjectloop->s_objectform->ElementName);

							while(pSubObjectloop->ElementName)
							{
								//RemLOG_Print("_token(%s),ElementName(%s)\r\n",_token, pSubObjectloop->ElementName);
								if(HxSTD_StrCmp(_token, pSubObjectloop->ElementName) == 0)
								{
									#if 0
									/* Process Object */
									if(pSubObjectloop->s_objectform != NULL)
									{
										hErr = rema_cwmp_revise_data_using_json(_value2, pSubObjectloop->s_objectform);
										if(hErr != ERR_OK)
										{
											RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
											REM_LEAVE(hErr);
										}
									}
									#endif
									if((pSubObjectloop->ElementType == eHLIB_JSON_Object))
									{
										/*input : {"Device":{"Services":{"STBService":[{"index":1,"X_JCOM_Config":{"URL_EPGServer":"http://www.jcom.co.jp/"}}]}}}*/

										hErr = rema_cwmp_revise_data_using_json(_value2, pSubObjectloop->s_objectform);
										if(hErr != ERR_OK)
										{
											RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
											REM_LEAVE(hErr);
										}
									}
									else if((pSubObjectloop->ElementType == eHLIB_JSON_String))
									{
										/* Process None Object */
										if(pSubObjectloop->pfnCbSetData == NULL)
										{
											RemLOG_Print("pSubObjectloop was not registered! %s\n",pSubObjectloop->ElementName);
											break;
										}
										if(pSubObjectloop->s_objectform)
										{
											pszstr = (HCHAR *)HxJSON_String_Get(value);
										}
										else
										{
											pszstr = (HCHAR *)HxJSON_String_Get(_value2);
										}
										RemLOG_Print("token : %s, value : %s\n",_token, pszstr);

										hErr = pSubObjectloop->pfnCbSetData((HCHAR*)pszstr);
										if(hErr != ERR_OK)
										{
											RemLOG_Print("pSubObjectloop returns Error! ElementName=%s [%s:%d]\n",pSubObjectloop->ElementName, __FUNCTION__, __LINE__);
											REM_LEAVE(hErr);
										}
									}
									else if((pSubObjectloop->ElementType == eHLIB_JSON_Array))
									{
										hErr = rema_cwmp_revise_data_using_json(_value2, pSubObjectloop);
										if(hErr != ERR_OK)
										{
											RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
											REM_LEAVE(hErr);
										}
									}
									else
									{
										RemLOG_Print("else.. what should i do?\n");
									}
								}
								pSubObjectloop++;
							}
						}
						else
						{

							RemLOG_Print("Not Array Member\r\n");
							while(pSubObjectloop->ElementName)
							{
								//RemLOG_Print("_token(%s),ElementName(%s)\r\n",_index_token, pSubObjectloop->ElementName);
								if(HxSTD_StrCmp(_index_token, pSubObjectloop->ElementName) == 0)
								{
									//if(pSubObjectloop->s_objectform) RemLOG_Print("pSubObjectloop->s_objectform->ElementName(%s)\r\n",pSubObjectloop->s_objectform->ElementName);
									hErr = rema_cwmp_revise_data_using_json(_value, pSubObjectloop->s_objectform);
									if(hErr != ERR_OK)
									{
										RemLOG_Print("+rema_cwmp_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
										REM_LEAVE(hErr);
									}
								}
								pSubObjectloop++;
							}
						}
					});
				}
				break;
			}

			objectloop++;
		}

		if(bfind == FALSE)
		{
			RemLOG_Print("token does not belong to DataModel! %s\n", token);
			REM_LEAVE(ERR_FAIL);
		}

		iter = HLIB_JSON_Object_NextIterator(json, iter);
	}

	REM_LEAVE(hErr);
}




//입력 object전체의 {parameter:value} pair를 json구조에 추가한다.
static HERROR rema_DM_add_object2json(HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat, HCHAR *token, HINT32 nEntryCount)
{
	HERROR hErr = ERR_OK;
	HxJSON_t subjson;
	HCHAR *pszstr = NULL;
	HUINT32 retVal;
	HBOOL	bNewInstance = TRUE;
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	REM_ENTER;
	CHECK_NULL(token);

	RemLOG_Print("[1] #### %s ####\r\n",__func__);

	objectloop = objectFormat;

	if(objectloop->ElementType == eHLIB_JSON_Array)
	{
		subjson = HxJSON_Array_New();
	}
	else
	{
		subjson = HxJSON_Object_New();

		if(objectloop->eDMType == TRDMType_M)
		{
			bNewInstance = FALSE;
		}
		else
		{
			//do nothing
		}

	}
	if (NULL == subjson)
	{
		RemLOG_Print("return %d\r\n",__LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	RemLOG_Print("[3] nEntry(%d),token(%s))\r\n",nEntryCount,token);

	while(objectloop->ElementName)
	{
		RemLOG_Print("[4] ElementName(%s),ElementType(%d),eDMType(%d)\r\n",objectloop->ElementName,objectloop->ElementType,objectloop->eDMType);
		switch(objectloop->ElementType)
		{
			case eHLIB_JSON_Object:
				if(objectloop->s_objectform != NULL)
				{
					RemLOG_Print("################ ENTER AA Point\r\n");


					//아래, " [ FRONT-END EXCEPTION CASE ] " 주석 부분 참고.
					//일본 시나리오 참고
					/*
					if(objectloop->eTRXDMInstancetype == eTRXDMInstance_Exception_ISDBC)
					{
						if(nFrontEndIndex < 2)
						{
							RemLOG_Print("Only use from second item\n");
							break;
						}
					}
					else if(objectloop->eTRXDMInstancetype == eTRXDMInstance_Exception_Docsis)
					{
						if(nFrontEndIndex > 1)
						{
							RemLOG_Print("Only use first item \n");
							break;
						}
					}
					*/

					hErr |= rema_DM_add_object2json(subjson, objectloop->s_objectform, objectloop->ElementName,0);
				}
				else
				{
					RemLOG_Print("s_objectform is not registered! %s\n",objectloop->ElementName);
				}
				break;

			case eHLIB_JSON_String:
				if(objectloop->pfnCbGetData != NULL)
				{
					pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
					HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
					if(pszstr != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HCHAR*)pszstr);
						RemLOG_Print("token : [%s],  objectloop->ElementName [%s], value : [%s]\n",token,  objectloop->ElementName, pszstr);
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(pszstr));

						#ifdef CONFIG_DEBUG
						PRINT_JSON(subjson);
						#endif
						RemLOG_Print("[5] @@@@@ Do Json set \r\n");
						HLIB_STD_MemFree(pszstr);
					}
				}
				else
				{
					RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			case eHLIB_JSON_Integer:

				if(objectloop->pfnCbGetData != NULL)
				{
					pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
					HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
					if(pszstr != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);
						RemLOG_Print("Integet retVal(%d)\r\n",retVal);
						HxSTD_PrintToStr(pszstr,"%d",retVal);

						//Data Model의 Integer Data Type은 실제 String으로 처리함
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(pszstr));
						HLIB_STD_MemFree(pszstr);
					}
				}
				else
				{
					RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			case eHLIB_JSON_TRUE:
			case eHLIB_JSON_FALSE:
				if(objectloop->pfnCbGetData != NULL)
				{
					pszstr = (HCHAR *)HLIB_STD_MemAlloc(objectloop->ulen);
					HxSTD_MemSet(pszstr, 0x00, objectloop->ulen);
					if(pszstr != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HCHAR*)pszstr);
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(pszstr));
						HLIB_STD_MemFree(pszstr);
					}
				}
				else
				{
					RemLOG_Print("pfnCbGetData is not registered! %s\n",token);
				}
				break;
			case eHLIB_JSON_Array:
				if((objectloop->s_objectform != NULL) && (objectloop->pfnCbGetData != NULL))
				{
					HxJSON_t subsubjson;
					HINT32 i;
					HCHAR	*szName = NULL;	
					HUINT32	nameLen = 0;
					retVal = 0;
					subsubjson = HxJSON_Array_New();
					hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);

					RemLOG_Print("%s, ElementName[%s], Entry(%d)\n",__func__,objectloop->ElementName,retVal);

					nameLen = HxSTD_StrLen(objectloop->ElementName);
					szName	= HLIB_STD_MemAlloc(nameLen+1);
					if(szName == NULL)
					{
						hErr = ERR_FAIL;
						goto EXIT_FUNC;
					}
					
					HxSTD_MemSet(szName, 0x0, nameLen+1);
					HxSTD_MemCopy(szName, objectloop->ElementName, nameLen);
	
					rema_DM_MultiInstance_SetEntry(objectloop->eTRXDMInstancetype,retVal);

					if(retVal > 0)
					{
						 //Multi 의 초기 값은 '1'
						for(i=1; i<= retVal; i++)
						{
							HxJSON_t element;
							element = HxJSON_Object_New();

							rema_DM_MultiInstance_SetEntryCount(objectloop->eTRXDMInstancetype,i);

							hErr |= HxJSON_Object_Set(element, "index", HxJSON_Integer_New(i));

							if(objectloop->eTRXDMInstancetype == eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries)
							{
								/*
								[ FRONT-END EXCEPTION CASE ]
									- J:COM에서 제공한 Data Model Sheet의 해석을 달리한 P사때문에 아래와 같은 예외처리가 필요함
									- FrontEnd는 현재 4개
									- 1번째는 무조건 DOCSIS, 그리고, 나머지는 QAM 으로 올려달라는 요구사항이 있었음

									1. DOCSIS
									2. QAM 1
									3. QAM 2
									4. QAM 3
								*/
								RemLOG_Print("FRONT-END EXCEPTION CASE.....\n");
								nFrontEndIndex = i;
							}
							//RemLOG_Print("################ ENTER BB Point\r\n");

							hErr |= rema_cwmp_add_object2json(element, objectloop->s_objectform,token, i);

							HLIB_JSON_Array_Append(subsubjson, element);
						}

						#ifdef CONFIG_DEBUG
						{
							RemLOG_Print("@@@@@@ After process multi-instance. retVal(%d)\r\n",retVal);
							RemLOG_Print("@@@@@@ ElementName(%s),szName(%s)\r\n",objectloop->ElementName,szName);
							RemLOG_Print("====================================================================================\r\n");
							PRINT_JSON(subsubjson);
							RemLOG_Print("====================================================================================\r\n");
						}
						#endif

						hErr |= HxJSON_Object_Set(subjson, szName, subsubjson);
					}
					else
					{
						RemLOG_Print("Empty Array\n");
						 //Multi 지만, 아무런 element가 없는 경우 빈값 []  으로 채움.
						hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, subsubjson);
					}

					if(szName)
					{
						HLIB_STD_MemFree(szName);
						szName = NULL;
					}

				}
				else
				{
					RemLOG_Print("s_objectform is not registered! %s\n",objectloop->ElementName);
				}

				break;
			default:
				hErr = ERR_FAIL;
				RemLOG_Print("Not Support!! name:%s, type:%d\n", objectloop->ElementName, objectloop->ElementType);
				break;
		}

		objectloop++;
	}

EXIT_FUNC:
	if(bNewInstance == TRUE)
		hErr |= HxJSON_Object_Set(json, token, subjson);
	else
		hErr |= HLIB_JSON_Object_Update(json, subjson);

	if (hErr != ERR_OK)
	{
		HLIB_JSON_Delete(subjson);
		subjson = NULL;
	}
	REM_LEAVE(hErr);
}


#define ___INTERNAL_FUNCTIONS___


HBOOL _StrStartWith (const HCHAR *s, const HCHAR *key)
{
	HUINT32	len;

	len = HxSTD_StrLen(key);
	if (len == 0)
		return FALSE;

	if (HxSTD_StrNCmp(s, key, len) == 0)
		return TRUE;
	return FALSE;
}


#define ___EXTERNAL_FUNCTIONS___


HERROR rema_cwmp_Init()
{
	REM_FUNC_ENTER;

	TR135_module_Init();	
	TR157_module_Init();
	TR181_module_Init();

	REM_FUNC_LEAVE;

	return TRUE;
}

HERROR rema_cwmp_DeInit()
{

	REM_FUNC_ENTER;

	TR135_module_DeInit();	
	TR181_module_DeInit();


	REM_FUNC_LEAVE;

	return TRUE;
}


HERROR rema_cwmp_SetParam(char* input){
	HERROR 			hErr = ERR_OK;
	void			*iter;
	HxJSON_t 		json;
	const HCHAR* 	token;

	REM_ENTER;

	CHECK_NULL(input);
	RemLOG_Print("input:%s\n",input);

	json = HLIB_JSON_Decode(input);

	PRINT_JSON(json);

	iter = HLIB_JSON_Object_Iterator(json);
	token = HLIB_JSON_Object_IteratorKey(iter);

	if (!HLIB_STD_StrStartWith(token, TRPORT_DATAMODEL_ROOT_OBJECT))
	{
		RemLOG_Print("[%s:%d] Syntax Error[input SHALL start with \"Device\"]: %s\n", __FUNCTION__, __LINE__, token);
		REM_LEAVE(ERR_FAIL);
	}

	json = HxJSON_Object_Get(json, token);
	hErr = rema_cwmp_revise_data_using_json(json, s_Device_ObjectForm);

	REM_LEAVE(hErr);

}





HCHAR* rema_cwmp_GetParam(HCHAR *key){
	HERROR		hErr = ERR_OK;
	HxJSON_t 	json, outroot;
	HCHAR 		token[LENGTH_OF_DM_TOKEN];
	HCHAR		*szParam;

	REM_ENTER;

	RemLOG_Print("key:[%s][%s:%d]\n", key, __FUNCTION__, __LINE__);

	key = (HCHAR*)rema_cwmp_json_next_token(token, LENGTH_OF_DM_TOKEN, key);

	if (!_StrStartWith(token, TRPORT_DATAMODEL_ROOT_OBJECT))
	{
		RemLOG_Print("[%s:%d] Syntax Error[key have to start with \"Device\"]: %s\n", __FUNCTION__, __LINE__, token);
		return NULL;
	}

	outroot = HxJSON_Object_New();

	/*if last character is equal to dot(.) and length is equal to 1. then input must be "Devie."*/
	if(*key=='.' && strlen(key)==1)
	{
		RemLOG_Print("%s, (%d)\r\n",__func__,__LINE__);
		hErr = rema_DM_add_object2json(outroot, s_Device_ObjectForm, TRPORT_DATAMODEL_ROOT_OBJECT, 0);
	}
	else
	{
		RemLOG_Print("%s, (%d)\r\n",__func__,__LINE__);
		HxJSON_Object_Set(outroot, TRPORT_DATAMODEL_ROOT_OBJECT, HxJSON_Object_New());
		json  = HxJSON_Object_Get(outroot, TRPORT_DATAMODEL_ROOT_OBJECT);

		hErr = rema_cwmp_build_json_using_key(key, json, s_Device_ObjectForm);
	}
	if(hErr != ERR_OK)
	{
		RemLOG_Print("[%s:%d] fail to GPV, but keep going\n", __FUNCTION__, __LINE__);
	}

	if(HxJSON_IsNULL(outroot))
	{
		RemLOG_Print("@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#\n");
		RemLOG_Print("@#@#@#@#@# JSON is NULL @#@#@#@#@#\n");
		RemLOG_Print("@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#\n");

	}
	PRINT_JSON(outroot);

	szParam = HLIB_JSON_Encode(outroot, NULL, 0);
	RemLOG_Print("\r\r\n output : %s\n",szParam);

	return szParam;



}





#define _____END_OF_FILE_________


