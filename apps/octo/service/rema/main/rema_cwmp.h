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



#ifndef	__REMA_CWMP_H__
#define	__REMA_CWMP_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

//#include "hCwmpApi.h"
//#include "hCwmpDMApi.h"
//#include "CWMP_CPE_API.h"
//#include "CWMP_STB_API.h"
//#include "CWMP_define.h"
#include <napi.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define TRPORT_MACADDR_LEN		20
/* Start of Storage - UserDB*/
#define USERITEM_TEXT_LEN			22
#define UDB_TEXT_LEN			128

#define LENGTH_OF_DM_TOKEN	64

#define STRING_LEN_32			32
#define STRING_LEN_64			64
#define STRING_LEN_256			256

typedef enum
{

	//2 Single Instance

	eTRXDMInstance_Exception_ISDBC = -2,
	eTRXDMInstance_Exception_Docsis = -1,
	eTRXDMInstance_None = 0,
	eTR157A3_Device_DeviceSummary,
	eTR157A3_Device_SmartCardReaderNumberOfEntries,
	eTR135A1_Device_Services_STBServiceNumberOfEntries,
	eTR140A1_Device_Services_StorageServiceNumberOfEntries,
	//eTR135A1_DSSTB_1_Capa_,	//eTR135A1_DSSTB_1_Capa_
	eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams,
	eTR135A1_DSSTB_1_Capa_MaxActiveAVPlayers,
	eTR135A1_DSSTB_1_Capa_PVR_MaxIOStreams,
	eTR135A1_DSSTB_1_Capa_PVR_MaxRecordingStreams,
	eTR135A1_DSSTB_1_Capa_PVR_MaxPlaybackStreams,
	eTR135A1_DSSTB_1_Capa_PVR_MaxTimeDelay,
	eTR135A1_DSSTB_1_Capa_AudioDecoder_AudioStandards,	//spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoDecoder_VideoStandards,	//spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_AudioStandards,	//spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_AudioStandards,	//spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevelNumberOfEntries,
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_AudioStandards,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries,
	//Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.
	eTR135A1_DSSTB_1_Capa_AudioOutput_AudioFormats,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoOutput_CompositeVideoStandards,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoOutput_VideoFormats,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoOutput_Macrovision,
	eTR135A1_DSSTB_1_Capa_VideoOutput_HDCP,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_VideoOutput_DisplayFormats,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_HDMI_SupportedResolutions,	// spec len : 16
	eTR135A1_DSSTB_1_Capa_HDMI_CECSupport,
	eTR135A1_DSSTB_1_Capa_HDMI_HDMI3D,
	eTR135A1_DSSTB_1_Capa_CA_CASystems,	// spec len : 1024
	eTR135A1_DSSTB_1_Capa_DRM_DRMSystems,	// spec len : 1024

	//No Support:JCOM
	//Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.

	//eTR135A1_DSSTB_1_Comp_,	//eTR135A1_Device_Services_STBService_1_Components
	eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,
	eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries,
	eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,
	eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,
	eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,
	eTR135A1_DSSTB_1_Comp_CANumberOfEntries,
	eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,
	eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Frequency,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Constellation,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_SNR,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_BER,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_CBER,	//appkit 에처 처리
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Power,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Locked,

	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Service_CurrentLogicalChannel,

	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_Status,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_IPAddress,	//spec len : 45
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_MACAddress,	//spec len : 17
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_Frequency,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_Constellation,	//spec len : 16
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_BER,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_SNR,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_Power,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_Frequency,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_ChannelId,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_BandWidth,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_Power,
	eTR135A1_DSSTB_1_Comp_PVR_StorageNumberOfEntries,

	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_Status,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_Name,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_EEDID,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_SupportedResolutions,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_PreferredResolution,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_VideoLatency,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_CECSupport,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport,
	eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_HDMI3DPresent,

	//eTR135A1_DSSTB_1_AVS_,	//eTR135A1_Device_Services_STBService_1_AVStreams
	eTR135A1_DSSTB_1_AVS_ActiveAVStreams,
	eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,
	//eTR135A1_DSSTB_1_AVP_,	//eTR135A1_Device_Services_STBService_1_AVPlayers
	eTR135A1_DSSTB_1_AVP_ActiveAVPlayers,
	eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,
	eTR135A1_DSSTB_1_AVP_PreferredAudioLanguage,
	eTR135A1_DSSTB_1_AVP_PreferredSubtitlingLanguage,
	eTR135A1_DSSTB_1_AVP_PreferredBehaviour,
	eTR135A1_DSSTB_1_AVP_ResetPINCode,
	eTR135A1_DSSTB_1_AVP_X_JCOM_PreferredSuperimposeLanguage,
	eTR135A1_DSSTB_1_AVP_X_JCOM_Zoom,
	//eTR135A1_DSSTB_1_JCOM_,	//eTR135A1_Device_Services_STBService_1_X_JCOM_Config
	eTR135A1_DSSTB_1_JCOM_PostalCode,	// 1
	eTR135A1_DSSTB_1_JCOM_TuningRange,	// 1
	eTR135A1_DSSTB_1_JCOM_URL_Navi,	// 1
	eTR135A1_DSSTB_1_JCOM_URL_InteracTV,	// 1
	eTR135A1_DSSTB_1_JCOM_URL_VOD,	// 1
	eTR135A1_DSSTB_1_JCOM_URL_AppDLS,	//new
	eTR135A1_DSSTB_1_JCOM_URL_recommendServer,//new
	eTR135A1_DSSTB_1_JCOM_URL_OpenIDServer,//new
	eTR135A1_DSSTB_1_JCOM_URL_EPGServer,//new
	eTR135A1_DSSTB_1_JCOM_AutoFirmwareUpdate,	// 1
	eTR135A1_DSSTB_1_JCOM_ParentalControl,	// 1
	eTR135A1_DSSTB_1_JCOM_PINCodeExpiration,	// 1
	eTR135A1_DSSTB_1_JCOM_StandbyMode,	// 1
	eTR135A1_DSSTB_1_JCOM_PowerStatus,	// 1
	eTR135A1_DSSTB_1_JCOM_ClientEquipmentNumberOfEntries,	//new
	eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries,	//new
	eTR135A1_DSSTB_1_JCOM_HomeNetwork_Enable,	//new
	eTR135A1_DSSTB_1_JCOM_ChannelMasking_Status,	// 1
	eTR135A1_DSSTB_1_JCOM_ChannelMasking_ForcedOperation,
	eTR135A1_DSSTB_1_JCOM_ChannelMasking_Enable,	// 1
	eTR135A1_DSSTB_1_JCOM_Internet_ConnectionTypeUserChange,
	//Device.Services.STBService.{i}.X_JCOM_Config.Internet.HTTPProxy.
	eTR135A1_DSSTB_1_JCOM_MediaServer_Names,
	eTR135A1_DSSTB_1_JCOM_MediaServer_Name,	// 1
	eTR135A1_DSSTB_1_JCOM_MediaServer_Clients,
	eTR135A1_DSSTB_1_JCOM_PurchasedPPV_TotalPriceOfThisMonth,
	eTR135A1_DSSTB_1_JCOM_PurchasedPPV_TotalPriceOfPreviousMonth,
	eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries,
	eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries, // 1
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Start,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Status,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Progress,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_AreaCode,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_DTT,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_BS,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CS1,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CS2,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CATV,
	eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_JC_HITS,
	eTR135A1_DSSTB_1_JCOM_USB_HDD_PictureQuarity,
	eTR135A1_DSSTB_1_JCOM_USB_HDD_Selected,
	eTR135A1_DSSTB_1_JCOM_USB_HDD_AutoChapter,
	eTR135A1_DSSTB_1_JCOM_LAN_HDD_ServerName,
	eTR135A1_DSSTB_1_JCOM_LAN_HDD_RecordingPlace,
	eTR135A1_DSSTB_1_JCOM_HomeServer_Enable,
	eTR135A1_DSSTB_1_JCOM_HomeServer_Name,
	eTR135A1_DSSTB_1_JCOM_ClientEquipment_M_Enable,
	eTR135A1_DSSTB_1_JCOM_ClientEquipment_M_MACAddress,
	eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Enable,
	eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Name,
	eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Nickname,
	eTR140A1_DSStorageSvc_Capabilities,
	eTR140A1_DSStorageSvc_NetInfo,
	eTR140A1_DSStorageSvc_NetworkServer,
	//Device.Services.StorageService.{i}.NetworkServer.
	//Device.Capabilities.
#if 0 //CPE
	eTR157A3_Device_Capabilities_PerformanceDiagnostic_DownloadTransports,
	eTR157A3_Device_Capabilities_PerformanceDiagnostic_UploadTransports,
#endif
	eTR157A3_Device_DeviceInfo_Manufacturer,
	eTR157A3_Device_DeviceInfo_ManufacturerOUI,
	eTR157A3_Device_DeviceInfo_ModelName,
	eTR157A3_Device_DeviceInfo_Description,
	eTR157A3_Device_DeviceInfo_ProductClass,
	eTR157A3_Device_DeviceInfo_SerialNumber,
	eTR157A3_Device_DeviceInfo_VendorConfigFileNumberOfEntries,
	eTR157A3_Device_DeviceInfo_HardwareVersion,
	eTR157A3_Device_DeviceInfo_AdditionalHardwareVersion,
	eTR157A3_Device_DeviceInfo_SoftwareVersion,
	eTR157A3_Device_DeviceInfo_AdditionalSoftwareVersion,
	eTR157A3_Device_DeviceInfo_ProvisioningCode,
	eTR157A3_Device_DeviceInfo_DeviceStatus,
	eTR157A3_Device_DeviceInfo_UpTime,
#if 0 //CPE
	eTR157A3_Device_DeviceInfo_FirstUseDate,
#endif
	eTR157A3_Device_DeviceInfo_DeviceLog,
#if 1 //CPE
	eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,
#endif
	eTR157A3_Device_DeviceInfo_ProcessorNumberOfEntries,
	eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries,
	eTR157A3_Device_DeviceInfo_X_JCOM_SubscriberID,
	eTR157A3_Device_DeviceInfo_X_JCOM_ServiceID,
	eTR157A3_Device_DeviceInfo_X_JCOM_CorpCode,
	eTR157A3_Device_DeviceInfo_X_JCOM_FamilyID,
	eTR157A3_Device_DeviceInfo_X_JCOM_BusinessCode,

	eTR157A3_Device_DeviceInfo_MemoryStatus_Total,
	eTR157A3_Device_DeviceInfo_MemoryStatus_Free,
	eTR157A3_Device_DeviceInfo_ProcessStatus_CPUUsage,
	eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries,
	eTR157A3_Device_DeviceInfo_TemperatureStatus_TemperatureSensorNumberOfEntries,	
	eTR157A3_Device_MgmtSvr_URL,
	eTR157A3_Device_MgmtSvr_Username,
	eTR157A3_Device_MgmtSvr_Password,
	eTR157A3_Device_MgmtSvr_PeriodicInformEnable,
	eTR157A3_Device_MgmtSvr_PeriodicInformInterval,
	eTR157A3_Device_MgmtSvr_PeriodicInformTime,
	eTR157A3_Device_MgmtSvr_ParameterKey,
	eTR157A3_Device_MgmtSvr_ConnectionRequestURL,
	eTR157A3_Device_MgmtSvr_ConnectionRequestUsername,
	eTR157A3_Device_MgmtSvr_ConnectionRequestPassword,
	eTR157A3_Device_MgmtSvr_UpgradesManaged,
#if 1 //CPE
	eTR157A3_Device_MgmtSvr_DefaultActiveNotificationThrottle,	//CPE
	eTR157A3_Device_MgmtSvr_CWMPRetryMinimumWaitInterval,
	eTR157A3_Device_MgmtSvr_CWMPRetryIntervalMultiplier,
	eTR157A3_Device_MgmtSvr_AutonomousTransferCompletePolicy_Enable,
	eTR157A3_Device_MgmtSvr_AutonomousTransferCompletePolicy_TransferTypeFilter,
	eTR157A3_Device_MgmtSvr_AutonomousTransferCompletePolicy_FileTypeFilter,
#endif
	eTR157A3_Device_LAN_AddressingType,
	eTR157A3_Device_LAN_IPAddress,
	eTR157A3_Device_LAN_SubnetMask,
	eTR157A3_Device_LAN_DefaultGateway,
	eTR157A3_Device_LAN_DNSServers,
	eTR157A3_Device_LAN_MACAddress,
	eTR157A3_Device_LAN_Stats_ConnectionUpTime,
	eTR157A3_Device_LAN_Stats_TotalBytesSent,
	eTR157A3_Device_LAN_Stats_TotalBytesReceived,
	eTR157A3_Device_LAN_Stats_TotalPacketsSent,
	eTR157A3_Device_LAN_Stats_TotalPacketsReceived,
	eTR157A3_Device_LAN_Stats_CurrentDayInterval,
	eTR157A3_Device_LAN_Stats_CurrentDayBytesSent,
	eTR157A3_Device_LAN_Stats_CurrentDayBytesReceived,
	eTR157A3_Device_LAN_Stats_CurrentDayPacketsSent,
	eTR157A3_Device_LAN_Stats_CurrentDayPacketsReceived,
	eTR157A3_Device_LAN_Stats_QuarterHourInterval,
	eTR157A3_Device_LAN_Stats_QuarterHourBytesSent,
	eTR157A3_Device_LAN_Stats_QuarterHourBytesReceived,
	eTR157A3_Device_LAN_Stats_QuarterHourPacketsSent,
	eTR157A3_Device_LAN_Stats_QuarterHourPacketsReceived,
#if 1 //CPE
	eTR157A3_Device_LAN_IPPingDiagnostics_DiagnosticsState,
	eTR157A3_Device_LAN_IPPingDiagnostics_Host,
	eTR157A3_Device_LAN_IPPingDiagnostics_NumberOfRepetitions,
	eTR157A3_Device_LAN_IPPingDiagnostics_Timeout,
	eTR157A3_Device_LAN_IPPingDiagnostics_DataBlockSize,
	eTR157A3_Device_LAN_IPPingDiagnostics_DSCP,
//	eTR157A3_Device_LAN_IPPingDiagnostics_SuccessCount,
//	eTR157A3_Device_LAN_IPPingDiagnostics_FailureCount,
//	eTR157A3_Device_LAN_IPPingDiagnostics_AverageResponseTime,
//	eTR157A3_Device_LAN_IPPingDiagnostics_MinimumResponseTime,
//	eTR157A3_Device_LAN_IPPingDiagnostics_MaximumResponseTime,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_DiagnosticsState,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_Host,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_Timeout,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_DataBlockSize,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_MaxHopCount,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_DSCP,
//	eTR157A3_Device_LAN_TraceRouteDiagnostics_ResponseTime,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops,
#endif
	eTR157A3_Device_DownloadDiagnostics_DiagnosticsState,	//CPE
	eTR157A3_Device_DownloadDiagnostics_Interface,
	eTR157A3_Device_DownloadDiagnostics_DownloadURL,
	eTR157A3_Device_DownloadDiagnostics_DSCP,
	eTR157A3_Device_DownloadDiagnostics_EthernetPriority,
#if 0 //CPE
	eTR157A3_Device_DownloadDiagnostics_ROMTime,
	eTR157A3_Device_DownloadDiagnostics_BOMTime,
	eTR157A3_Device_DownloadDiagnostics_EOMTime,
	eTR157A3_Device_DownloadDiagnostics_TestBytesReceived,
	eTR157A3_Device_DownloadDiagnostics_TotalBytesReceived,
#endif
	eTR157A3_Device_UploadDiagnostics_DiagnosticsState,		//CPE
	eTR157A3_Device_UploadDiagnostics_Interface,
	eTR157A3_Device_UploadDiagnostics_UploadURL,
	eTR157A3_Device_UploadDiagnostics_DSCP,
	eTR157A3_Device_UploadDiagnostics_TestFileLength,
	eTR157A3_Device_UploadDiagnostics_EthernetPriority,
#if 0 //CPE
	eTR157A3_Device_UploadDiagnostics_ROMTime,
	eTR157A3_Device_UploadDiagnostics_BOMTime,
	eTR157A3_Device_UploadDiagnostics_EOMTime,
	eTR157A3_Device_UploadDiagnostics_TotalBytesSent,
#endif
	eTR157A3_Device_UPnP_Device_Enable,
	eTR157A3_Device_UPnP_Device_UPnPMediaServer,
	eTR157A3_Device_UPnP_Device_UPnPMediaRenderer,
	//Device.UPnP.
	//Device.UPnP.Device.Capabilities.
#if 1//maybe not supported
	eTR157A3_Device_UPnP_Device_Capabilities_UPnPArchitecture,
	eTR157A3_Device_UPnP_Device_Capabilities_UPnPArchitectureMinorVer,
	eTR157A3_Device_UPnP_Device_Capabilities_UPnPMediaServer,
	eTR157A3_Device_UPnP_Device_Capabilities_UPnPMediaRenderer,
#endif
	eTR157A3_Device_DLNA_Capabilities_HNDDeviceClass,
	eTR157A3_Device_DLNA_Capabilities_DeviceCapability,
	eTR157A3_Device_DLNA_Capabilities_HIDDeviceClass,
	eTR157A3_Device_DLNA_Capabilities_ImageClassProfileID,
	eTR157A3_Device_DLNA_Capabilities_AudioClassProfileID,
	eTR157A3_Device_DLNA_Capabilities_AVClassProfileID,
#if 1//maybe not supported
	eTR157A3_Device_UPnP_Discovery_RootDeviceNumberOfEntries,
	eTR157A3_Device_UPnP_Discovery_DeviceNumberOfEntries,
	eTR157A3_Device_UPnP_Capabilities_HNDDeviceClass,
	eTR157A3_Device_UPnP_Capabilities_ImageClassProfileID,
	eTR157A3_Device_UPnP_Capabilities_AudioClassProfileID,
	eTR157A3_Device_UPnP_Capabilities_AVClassProfileID,
#endif
	eTR157A3_Device_SmartCardReader_SmartCard,
	eTR157A3_Device_SelfTestDiagnostics_DiagnosticsState,
	eTR157A3_Device_SelfTestDiagnostics_Results,
	eTR157A3_Device_NSLookupDiagnostics_DiagnosticsState,
	eTR157A3_Device_NSLookupDiagnostics_Interface,
	eTR157A3_Device_NSLookupDiagnostics_HostName,
	eTR157A3_Device_NSLookupDiagnostics_DNSServer,
	eTR157A3_Device_NSLookupDiagnostics_Timeout,
	eTR157A3_Device_NSLookupDiagnostics_NumberOfRepetitions,
	//eTR157A3_Device_NSLookupDiagnostics_SuccessCount,
	eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,
	eTR157A3_Device_USBHosts_HostNumberOfEntries,
	//Device.SoftwareModules.
	//Device.X_JLABS_Management.
#if 0 //CPE
	eTR157A3_Device_X_JLABS_Management_AudienceMeasurement_CombinedAccess,
	eTR157A3_Device_X_JLABS_Management_AudienceMeasurement_PeriodicInformationInterval,
	eTR157A3_Device_X_JLABS_Management_AudienceMeasurement_PeriodicInformationTime,
	eTR157A3_Device_X_JLABS_Management_AudienceMeasurement_PermissionLevel,
#endif
	eTR157A3_Device_X_JCOM_HomeNetwork_Enable,
	eTR157A3_Device_X_JCOM_HomeNetwork_Status,
	eTR157A3_Device_X_JCOM_HomeNetwork_AddressingType,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPAddress,
	eTR157A3_Device_X_JCOM_HomeNetwork_SubnetMask,
	eTR157A3_Device_X_JCOM_HomeNetwork_DefaultGateway,
	eTR157A3_Device_X_JCOM_HomeNetwork_DNSServers,
	eTR157A3_Device_X_JCOM_HomeNetwork_MACAddress,
	eTR157A3_Device_X_JCOM_HomeNetwork_Stats_ConnectionUpTime,
	eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalBytesSent,
	eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalBytesReceived,
	eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalPacketsSent,
	eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalPacketsReceived,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_DiagnosticsState,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_Host,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_NumberOfRepetitions,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_Timeout,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_DataBlockSize,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_SuccessCount,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_FailureCount,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_AverageResponseTime,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_MinimumResponseTime,
	eTR157A3_Device_X_JCOM_HomeNetwork_IPPingDiagnostics_MaximumResponseTime,
	eTR157A3_Device_X_JCOM_Wifi_Enable,
	eTR157A3_Device_X_JCOM_Wifi_ConfigMethod,
	eTR157A3_Device_X_JCOM_Wifi_SSID,
	eTR157A3_Device_X_JCOM_Wifi_SecurityMethod,
	eTR157A3_Device_X_JCOM_Wifi_SecurityLevel,
	eTR157A3_Device_X_JCOM_Wifi_SecurityKey,
	eTR157A3_Device_X_JCOM_Wifi_KeyIndex,
	eTR157A3_Device_X_JCOM_Wifi_AddressingType,
	eTR157A3_Device_X_JCOM_Wifi_IPAddress,
	eTR157A3_Device_X_JCOM_Wifi_SubnetMask,
	eTR157A3_Device_X_JCOM_Wifi_DefaultGateway,

	eTR157A3_Device_UPnP_Discovery_ServiceNumberOfEntries,
	eTR157A3_Device_SoftwareModules_ExecEnvNumberOfEntries,

	//181
	eTR181A1_Device_WiFi_AccessPointNumberOfEntries,




	//2 MultiInstance
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevel_M,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG2Part2_ProfileLevel_M_Profile,	// spec len : 16
		eTR135A1_DSSTB_1_Capa_VDec_MPEG2Part2_ProfileLevel_M_Level,	// spec len : 16
		eTR135A1_DSSTB_1_Capa_VDec_MPEG2Part2_ProfileLevel_M_MaximumDecodingCapability,
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevel_M,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part2_ProfileLevel_M_Profile,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part2_ProfileLevel_M_Level,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part2_ProfileLevel_M_MaximumDecodingCapability,
	eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_Profile,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_Level,
		eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_MaximumDecodingCapability,
	//Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}.
	eTR135A1_DSSTB_1_Comp_FrontEnd_M,
		eTR135A1_DSSTB_1_Comp_FrontEnd_M_Status,
		eTR135A1_DSSTB_1_Comp_FrontEnd_M_Name,
	eTR135A1_DSSTB_1_Comp_PVR_Storage_M,
		eTR135A1_DSSTB_1_Comp_PVR_Storage_M_Reference,
	eTR135A1_DSSTB_1_Comp_AudioDecoder_M,
		eTR135A1_DSSTB_1_Comp_AudioDecoder_M_Status,
		eTR135A1_DSSTB_1_Comp_AudioDecoder_M_Name,
		eTR135A1_DSSTB_1_Comp_AudioDecoder_M_AudioStandard,
	eTR135A1_DSSTB_1_Comp_VideoDecoder_M,
		eTR135A1_DSSTB_1_Comp_VideoDecoder_M_Status,
		eTR135A1_DSSTB_1_Comp_VideoDecoder_M_Name,
		eTR135A1_DSSTB_1_Comp_VideoDecoder_M_MPEG2Part2,
		eTR135A1_DSSTB_1_Comp_VideoDecoder_M_MPEG4Part2,
		eTR135A1_DSSTB_1_Comp_VideoDecoder_M_MPEG4Part10,
		//eTR135A1_DSSTB_1_Comp_VideoDecoder_M_SMPTEVC1,
		eTR135A1_DSSTB_1_Comp_VideoDecoder_M_ContentAspectRatio,
	eTR135A1_DSSTB_1_Comp_AudioOutput_M,
		eTR135A1_DSSTB_1_Comp_AudioOutput_M_Status,
		eTR135A1_DSSTB_1_Comp_AudioOutput_M_Name,
		eTR135A1_DSSTB_1_Comp_AudioOutput_M_AudioFormat,
		eTR135A1_DSSTB_1_Comp_AudioOutput_M_AudioLevel, //w
		eTR135A1_DSSTB_1_Comp_AudioOutput_M_CancelMute, //w
	eTR135A1_DSSTB_1_Comp_SPDIF_M,
		eTR135A1_DSSTB_1_Comp_SPDIF_M_Status,
		eTR135A1_DSSTB_1_Comp_SPDIF_M_Name,
		eTR135A1_DSSTB_1_Comp_SPDIF_M_ForcePCM,	//w
		eTR135A1_DSSTB_1_Comp_SPDIF_M_Passthrough,
		eTR135A1_DSSTB_1_Comp_SPDIF_M_AudioDelay,
	eTR135A1_DSSTB_1_Comp_VideoOutput_M,
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_Status,
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_Name,
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_CompositeVideoStandard,
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_VideoFormat,	//w
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_AspectRatioBehaviour,
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_DisplayFormat,	//w
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_Macrovision,
		eTR135A1_DSSTB_1_Comp_VideoOutput_M_HDCP,
	eTR135A1_DSSTB_1_Comp_HDMI_M,
		eTR135A1_DSSTB_1_Comp_HDMI_M_Status,	//어떤 상태인지?? String이 어떤값인지?
		eTR135A1_DSSTB_1_Comp_HDMI_M_Name,
		eTR135A1_DSSTB_1_Comp_HDMI_M_ResolutionMode,	//w
		eTR135A1_DSSTB_1_Comp_HDMI_M_ResolutionValue,	//w
		eTR135A1_DSSTB_1_Comp_HDMI_M_X_JCOM_CECEnabled,	//w
		eTR135A1_DSSTB_1_Comp_HDMI_M_X_JCOM_3DOutput,	//w
	eTR135A1_DSSTB_1_Comp_CA_M,
		eTR135A1_DSSTB_1_Comp_CA_M_Status,
		eTR135A1_DSSTB_1_Comp_CA_M_Name,
		eTR135A1_DSSTB_1_Comp_CA_M_SmartCardReader,
	eTR135A1_DSSTB_1_AVS_AVStream_M,
		eTR135A1_DSSTB_1_AVS_AVStream_M_Status,
		eTR135A1_DSSTB_1_AVS_AVStream_M_Name,
		eTR135A1_DSSTB_1_AVS_AVStream_M_PVRState,
		eTR135A1_DSSTB_1_AVS_AVStream_M_FrontEnd,
		eTR135A1_DSSTB_1_AVS_AVStream_M_AudioDecoder,
		eTR135A1_DSSTB_1_AVS_AVStream_M_VideoDecoder,
		eTR135A1_DSSTB_1_AVS_AVStream_M_CA,
	eTR135A1_DSSTB_1_AVP_AVPlayer_M,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_Status,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_Name,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_AudioLanguage,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_SubtitlingStatus,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_SubtitlingLanguage,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_AudioOutputs,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_VideoOutputs,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_MainStream,
		eTR135A1_DSSTB_1_AVP_AVPlayer_M_PIPStreams,
	eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M,
		eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_IsdbId,
		eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Name,
		eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Date,
		eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Price,
	eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_NetworkId,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_Name,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries,
	eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId,
		eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency,
	eTR140A1_DSStorageSvc_M,
		eTR140A1_DSStorageSvc_M_Enable,
		eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,
		eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,
		eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,
	eTR140A1_DSStorageSvc_M_Capabilities,
		eTR140A1_DSStorageSvc_M_Capabilities_SupportedFileSystemTypes,
	eTR140A1_DSStorageSvc_M_NetInfo,
		eTR140A1_DSStorageSvc_M_NetInfo_HostName,
		eTR140A1_DSStorageSvc_M_NetInfo_DomainName,
	eTR140A1_DSStorageSvc_PhysicalMedium_M,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_Name,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_Vendor,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_Model,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_SerialNumber,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_ConnectionType,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_Removable,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_Capacity,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_Status,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_HotSwappable,
	eTR140A1_DSStorageSvc_PhysicalMedium_M_X_JCOM,
		eTR140A1_DSStorageSvc_PhysicalMedium_M_X_JCOM_PowerSupply_Interlocking,
	eTR140A1_DSStorageSvc_LogicalVolume_M,
		eTR140A1_DSStorageSvc_LogicalVolume_M_Name,
		eTR140A1_DSStorageSvc_LogicalVolume_M_Status,
		eTR140A1_DSStorageSvc_LogicalVolume_M_Enable,
		eTR140A1_DSStorageSvc_LogicalVolume_M_PhysicalReference,
		eTR140A1_DSStorageSvc_LogicalVolume_M_FileSystem,
		eTR140A1_DSStorageSvc_LogicalVolume_M_Capacity,
		eTR140A1_DSStorageSvc_LogicalVolume_M_UsedSpace,
		eTR140A1_DSStorageSvc_LogicalVolume_M_Encrypted,
		eTR140A1_DSStorageSvc_LogicalVolume_M_FolderNumberOfEntries,
	eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M,
		eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_RegistrationEnable,
		eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_USB_HDDEnable,
		eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Status,
		eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Name,
		eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Initialization,
	//Device.Services.StorageService.{i}.LogicalVolume.{i}.Folder.{i}.
	eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Diagnostics,
		eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState,
	eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M,
		eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_PID,
		eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_Command,
		eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_Size,
		eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_Priority,
		eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_CPUTime,
		eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_State,
#if 0 //CPE
	eTR157A3_Device_DeviceInfo_SupportedDataModel_M,
#endif
	eTR157A3_Device_DeviceInfo_VendorConfigFile_M,
		eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Name,
		eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Version,
		eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Date,
		eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Description,
	eTR157A3_Device_DeviceInfo_Processor_M,
		eTR157A3_Device_DeviceInfo_Processor_M_Architecture,
	eTR157A3_Device_DeviceInfo_VendorLogFile_M,
		eTR157A3_Device_DeviceInfo_VendorLogFile_M_Name,
		eTR157A3_Device_DeviceInfo_VendorLogFile_M_MaximumSize,
		eTR157A3_Device_DeviceInfo_VendorLogFile_M_Persistent,
#if 0 //CPE
	eTR157A3_Device_LAN_TraceRouteDiagnostics_RouteHops_M,
#endif
	eTR157A3_Device_UPnP_Discovery_RootDevice_M,
		eTR157A3_Device_UPnP_Discovery_RootDevice_M_Status,
	eTR157A3_Device_UPnP_Discovery_Device_M,
		eTR157A3_Device_UPnP_Discovery_Device_M_Status,
	eTR157A3_Device_SmartCardReader_M,
		eTR157A3_Device_SmartCardReader_M_Enable,
		eTR157A3_Device_SmartCardReader_M_Status,
		eTR157A3_Device_SmartCardReader_M_Name,
		eTR157A3_Device_SmartCardReader_M_Reset,
		eTR157A3_Device_SmartCardReader_M_ResetTime,
	eTR157A3_Device_SmartCardReader_M_SmartCard,
		eTR157A3_Device_SmartCardReader_M_SmartCard_Status,
		eTR157A3_Device_SmartCardReader_M_SmartCard_Type,
		eTR157A3_Device_SmartCardReader_M_SmartCard_Application,
		eTR157A3_Device_SmartCardReader_M_SmartCard_SerialNumber,
		eTR157A3_Device_SmartCardReader_M_SmartCard_ATR,
		eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_CardType,
		eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries,
	eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupID_M,
		eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number,
#if 0 //CPE
	eTR157A3_Device_NSLookupDiagnostics_Result_M,
#endif
	eTR157A3_Device_USBHosts_Host_M,
		eTR157A3_Device_USBHosts_Host_M_Name,
		eTR157A3_Device_USBHosts_Host_M_Type,
		eTR157A3_Device_USBHosts_Host_M_USBVersion,
		eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,
	eTR157A3_DUSBHosts_HDev_M,
		eTR157A3_DUSBHosts_HDev_M_DeviceNumber,
		eTR157A3_DUSBHosts_HDev_M_USBVersion,
		eTR157A3_DUSBHosts_HDev_M_DeviceClass,
		eTR157A3_DUSBHosts_HDev_M_DeviceSubClass,
		eTR157A3_DUSBHosts_HDev_M_DeviceVersion,
		eTR157A3_DUSBHosts_HDev_M_DeviceProtocol,
		eTR157A3_DUSBHosts_HDev_M_ProductID,
		eTR157A3_DUSBHosts_HDev_M_VendorID,
		eTR157A3_DUSBHosts_HDev_M_Manufacturer,
		eTR157A3_DUSBHosts_HDev_M_ProductClass,
		eTR157A3_DUSBHosts_HDev_M_SerialNumber,
		eTR157A3_DUSBHosts_HDev_M_Port,
		eTR157A3_DUSBHosts_HDev_M_Rate,
		eTR157A3_DUSBHosts_HDev_M_Parent,
		eTR157A3_DUSBHosts_HDev_M_MaxChildren,
		eTR157A3_DUSBHosts_HDev_M_IsSuspended,
		eTR157A3_DUSBHosts_HDev_M_IsSelfPowered,
		eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,
	eTR157A3_DUSBHosts_HDev_Configuration_M,
		eTR157A3_DUSBHosts_HDev_Configuration_M_ConfigurationNumber,
		eTR157A3_DUSBHosts_HDev_Configuration_M_InterfaceNumberOfEntries,
	eTR157A3_DUSBHosts_HDev_Configuration_Interface_M,
		eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceNumber,
		eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceClass,
		eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceSubClass,
		eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceProtocol,

	eTR157A3_Device_PeriodicStatistics,
		eTR157A3_Device_PeriodicStatistics_MinSampleInterval,
		eTR157A3_Device_PeriodicStatistics_MaxReportSamples,
		eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,
	eTR157A3_Device_PeriodicStatistics_SampleSet_M,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Enable,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Status,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Name,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_SampleInterval,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_ReportSamples,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_TimeReference,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_FetchSamples,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_ForceSample,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_ReportStartTime,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_ReportEndTime,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_SampleSeconds,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,
	eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Enable,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Reference,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Values,
		eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Failures,
	eTR157A3_Device_SoftwareModules_ExecEnv_M,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_Enable,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_Status,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_Name,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_Type,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_Vendor,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_Version,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_AllocatedDiskSpace,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_AvailableDiskSpace,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_AllocatedMemory,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_AvailableMemory,
		eTR157A3_Device_SoftwareModules_ExecEnv_M_ActiveExecutionUnits,
	eTRXDMInstance_Max,
} eTRXDMInstancetype_e;


typedef enum
{
	eFLAG_NEED_TO_RESET =0,
	eFLAG_NO_NEED_TO_RESET
}trport_reset_flag_e;


typedef enum
{
	eMULTI_DM_DBIndex_None = 0,
	eTR135A1_Device_Services_STBService_M_Index,

	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevel_M_Index,
	eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevel_M_Index,
	eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_Index,
	eTR135A1_DSSTB_1_Comp_FrontEnd_M_Index,
	eTR135A1_DSSTB_1_Comp_PVR_Storage_M_Index,
	eTR135A1_DSSTB_1_Comp_AudioDecoder_M_Index,
	eTR135A1_DSSTB_1_Comp_VideoDecoder_M_Index,
	eTR135A1_DSSTB_1_Comp_AudioOutput_M_Index,
	eTR135A1_DSSTB_1_Comp_SPDIF_M_Index,
	eTR135A1_DSSTB_1_Comp_VideoOutput_M_Index,
	eTR135A1_DSSTB_1_Comp_HDMI_M_Index,
	eTR135A1_DSSTB_1_Comp_CA_M_Index,
	eTR135A1_DSSTB_1_AVS_AVStream_M_Index,
	eTR135A1_DSSTB_1_AVP_AVPlayer_M_Index,
	eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Index,
	eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_Index,
	eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Index,

	eTR140A1_DSStorageSvc_M_Index,
	eTR140A1_DSStorageSvc_M_PhysicalMedium_M_Index,
	eTR140A1_DSStorageSvc_M_LogicalVolume_M_Index,
	eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Index,

	eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_Index,
	eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Index,
	eTR157A3_Device_DeviceInfo_SupportedDataMode_M_Index,
	eTR157A3_Device_DeviceInfo_Processor_M_Index,
	eTR157A3_Device_DeviceInfo_VendorLogFile_M_Index,

	eTR157A3_Device_UPnP_Discovery_RootDevice_M_Index,
	eTR157A3_Device_UPnP_Discovery_Device_M_Index,
	eTR157A3_Device_NSLookupDiagnostics_Result_M_Index,

	eTR157A3_Device_SmartCardReader_M_Index,
	eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupID_M_Index,

	eTR157A3_DUSBHosts_Host_M_Index,
	eTR157A3_DUSBHosts_HDev_M_Index,
	eTR157A3_DUSBHosts_HDev_Configuration_M_Index,
	eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_Index,

	eTR157A3_Device_PeriodicStatistics_SampleSet_M_Index,
	eTR157A3_Device_PeriodicStatistics_SampleSet_Parameter_M_Index,
	eTR157A3_Device_LAN_TraceRouteDiagnostics_M_Index,
	eTR135A1_DSSTB_1_JCOM_ClientEquipment_M_Index,

	eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Index,
	eTR157A3_Device_UPnP_Discovery_Service_M_Index,
	eTR157A3_Device_SoftwareModules_ExecEnv_M_Index,

	eTR181A1_Device_WiFi_AccessPoint_M_Index,

	eMULTI_DM_DBIndex_MAX,

}MULTI_DM_DBIndex_e;

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
	TRDMType_S=0,
	TRDMType_M,
	TRDMType_Max,
}TRDMType_e;

typedef HERROR (*pfn_getCB_t)(void* output);
typedef HERROR (*pfn_setCB_t)(void* input);

typedef void (*REMA_OUTPUT_VIDEO_Notifier_t) (void);


typedef struct TRPORT_Object_Format_t
{
	HCHAR						*ElementName;
	HxJSON_Type_e				ElementType;
	HUINT32						ulen;
	pfn_getCB_t 				pfnCbGetData;
	pfn_setCB_t 				pfnCbSetData;
	struct TRPORT_Object_Format_t *s_objectform;
	TRDMType_e					eDMType;
	eTRXDMInstancetype_e		eTRXDMInstancetype;
}TRPORT_OBJECT_FORMAT_t;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ______________________________TR_135______________________________
#define ______________________STATIC_FUNCTION_TRP_GET______________________
extern HERROR TRP135_Get_Service_STBServiceNumberOfEntries(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_PVR_MaxIOStreams(HINT32 *pnNum);
extern HERROR TRP135_Get_Capa_PVR_MaxRecordingStreams(HCHAR *pnNum);
extern HERROR TRP135_Get_Capa_PVR_MaxPlaybackStreams(HCHAR *pnNum);
extern HERROR TRP135_Get_Capa_PVR_MaxTimeDelay(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_AudioDecoder_AudioStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP2P2_AudioStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevelNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_Profile(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_Level(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P2_AudioStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevelNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_Profile(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_Level(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P10_AudioStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevelNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_Profile(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_Level(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Profile(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Level(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_SMPTEVC1_AudioStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevelNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_Capa_VDec_VideoStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_AOutput_AudioFormats(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VOutput_CompositeVideoStandards(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VOutput_VideoFormats(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VOutput_Macrovision(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VOutput_HDCP(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_VOutput_DisplayFormats(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_HDMI_SupportedResolutions(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_HDMI_CECSupport(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_HDMI_HDMI3D(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_CA_CASystems(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_DRM_DRMSystems(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_ServiceMonitoring_MaxActiveMainStreams(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_ServiceMonitoring_MinSampleInterval(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_ServiceMonitoring_MaxReportSamples(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_ServiceMonitoring_HighLevelMetricNames(HCHAR *pszOutput);
extern HERROR TRP135_Get_Capa_MaxActiveAVStreams(HCHAR *pnNum);
extern HERROR TRP135_Get_Capa_MaxActiveAVPlayers(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_FrontEndNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_AudioDecoderNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_VideoDecoderNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_AudioOutputNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_VideoOutputNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_CANumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_HDMINumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_SPDIFNumberOfEntries(HINT32 *pnNum);
extern HERROR TRP135_Get_Comp_FrontEnd_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Frequency(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Constellation(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_SNR(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_BER(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_CBER(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Power(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Locked(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Service_CurrentLogicalChannel(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_IPAddress(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_MACAddress(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Frequency(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Constellation(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_BER(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_SNR(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Power(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Frequency(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_ChannelId(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_BandWidth(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Power(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_PVR_StorageNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_Comp_PVR_Storage_M_Reference(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioDecoder_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioDecoder_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioDecoder_M_AudioStandard(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoDecoder_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoDecoder_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoDecoder_M_MPEG2Part2(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoDecoder_M_MPEG4Part2(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoDecoder_M_MPEG4Part10(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoDecoder_M_SMPTEVC1(HCHAR *pszOutput);

extern HERROR TRP135_Get_Comp_VideoDecoder_M_ContentAspectRatio(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioOutput_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioOutput_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioOutput_M_AudioFormat(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioOutput_M_AudioLevel(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_AudioOutput_M_CancelMute(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_SPDIF_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_SPDIF_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_SPDIF_M_ForcePCM(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_SPDIF_M_Passthrough(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_SPDIF_M_AudioDelay(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_CompositeVideoStandard(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_VideoFormat(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_AspectRatioBehaviour(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_DisplayFormat(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_Macrovision(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_VideoOutput_M_HDCP(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_ResolutionMode(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_ResolutionValue(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_JCOM_CECEnabled(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_JCOM_3DOutput(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_EEDID(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_SupportedResolutions(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_PreferredResolution(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_VideoLatency(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_CECSupport(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_HDMI3DPresent(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_CA_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_CA_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_Comp_CA_M_SmartCardReader(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_ActiveAVStreams(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStreamNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_PVRState(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_FrontEnd(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_AudioDecoder(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_VideoDecoder(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVStreams_AVStream_M_CA(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_ActiveAVPlayers(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayerNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_AVPlayers_PreferredAudioLanguage(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_PreferredSubtitlingLanguage(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_PreferredBehaviour(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_ResetPINCode(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_JCOM_PreferredSuperimposeLanguage(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_JCOM_Zoom(HCHAR *pszOutput);

extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_Alias(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_AudioLanguage(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_SubtitlingStatus(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_SubtitlingLanguage(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_AudioOutputs(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_VideoOutputs(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_MainStream(HCHAR *pszOutput);
extern HERROR TRP135_Get_AVPlayers_AVPlayer_M_PIPStreams(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PostalCode(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_TuningRange(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_Navi(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_InteracTV(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_VOD(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_AppDLS(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_recommendServer(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_OpenIDServer(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_URL_EPGServer(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_AutoFirmwareUpdate(HCHAR *pszOutput);

extern HERROR TRP135_Get_JCOM_Config_ParentalControl(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PINCodeExpiration(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_StandbyMode(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PowerStatus(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ClientEquipmentNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_RemoteEquipmentNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_HomeNetwork_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ChannelMasking_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ChannelMasking_ForcedOperation(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ChannelMasking_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_Internet_ConnectionType(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_Internet_ConnectionTypeUserChange(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_IPAddress(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_PortNumber(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_UserChange(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_MediaServer_Names(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_MediaServer_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_MediaServer_Clients(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_TotalPriceOfThisMonth(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_TotalPriceOfPreviousMonth(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_HistoryNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_IsdbId(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Date(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Price(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabaseNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Start(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Status(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Progress(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_AreaCode(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_DTT(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_BS(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CS1(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CS2(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CATV(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_NetworkId(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LCNOfEntries(HINT32 *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_USB_HDD_PictureQuarity(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_USB_HDD_Selected(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_USB_HDD_AutoChapter(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_LAN_HDD_ServerName(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_LAN_HDD_RecordingPlace(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_HomeServer_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_HomeServer_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ClientEquipment_M_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_ClientEquipment_M_MACAddress(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_RemoteEquipment_M_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_RemoteEquipment_M_Name(HCHAR *pszOutput);
extern HERROR TRP135_Get_JCOM_Config_RemoteEquipment_M_Nickname(HCHAR *pszOutput);



#define ______________________PUBLIC_FUNCTION_TRP_SET______________________

extern HERROR TRP135_Set_JCOM_Config_PostalCode(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_TuningRange(HCHAR *pszRange);
extern HERROR TRP135_Set_JCOM_Config_URL_Navi(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_URL_InteracTV(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_URL_VOD(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_URL_AppDLS(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_URL_recommendServer(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_URL_OpenIDServer(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_URL_EPGServer(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_AutoFirmwareUpdate(HCHAR *pszAutoDownEnable);

extern HERROR TRP135_Set_JCOM_Config_ParentalControl(HCHAR *ratingValue);
extern HERROR TRP135_Set_JCOM_Config_PINCodeExpiration(HCHAR *pszPin);
extern HERROR TRP135_Set_JCOM_Config_StandbyMode(HCHAR *pszStandbyMode);
extern HERROR TRP135_Set_JCOM_Config_PowerStatus(HCHAR *pszStandbyMode);
extern HERROR TRP135_Set_JCOM_Config_ChannelMasking_ForcedOperation(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_ChannelMasking_Enable(HCHAR *pszChMaskEnable);
extern HERROR TRP135_Set_JCOM_Config_MediaServer_Name(HCHAR *pszFriendlyName);
extern HERROR TRP135_Set_Comp_AudioOutput_M_AudioLevel(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_AudioOutput_M_CancelMute(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_SPDIF_M_ForcePCM(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_VideoOutput_M_VideoFormat(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_VideoOutput_M_DisplayFormat(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_HDMI_M_ResolutionMode(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_HDMI_M_ResolutionValue(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_HDMI_M_JCOM_CECEnabled(HCHAR *pszInput);
extern HERROR TRP135_Set_Comp_HDMI_M_JCOM_3DOutput(HCHAR *pszInput);
extern HERROR TRP135_Set_AVPlayers_PreferredAudioLanguage(HCHAR *pszInput);
extern HERROR TRP135_Set_AVPlayers_PreferredSubtitlingLanguage(HCHAR *pszInput);
extern HERROR TRP135_Set_AVPlayers_PreferredBehaviour(HCHAR *pszInput);
extern HERROR TRP135_Set_AVPlayers_ResetPINCode(HCHAR *pszInput);
extern HERROR TRP135_Set_AVPlayers_JCOM_PreferredSuperimposeLanguage(HCHAR *pszInput);
extern HERROR TRP135_Set_AVPlayers_JCOM_Zoom(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_Internet_ConnectionType(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_Internet_ConnectionTypeUserChange(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_Enable(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_IPAddress(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_PortNumber(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_UserChange(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Start(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_AreaCode(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_DTT(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_BS(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CS1(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CS2(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CATV(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_USB_HDD_PictureQuarity(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_USB_HDD_Selected(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_USB_HDD_AutoChapter(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_HomeServer_Enable(HCHAR *pszInput);
extern HERROR TRP135_Set_JCOM_Config_HomeServer_Name(HCHAR *pszInput);

#define ______________________________TR_140______________________________
#define ______________________PUBLIC_FUNCTION_TRP_GET______________________

extern HERROR TRP140_Get_Service_StorageServiceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP140_Get_Service_StorageNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Enable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMediumNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolumeNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_USB_HDDNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_FTPCapable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_SFTPCapable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_HTTPCapable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_HTTPSCapable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_HTTPWritable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_SupportedNetworkProtocols(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_SupportedFileSystemTypes(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_Capa_VolumeEncryptionCapable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_NetInfo_HostName(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_NetInfo_DomainName(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_NetworkServer_AFPEnable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_NetworkServer_NFSEnable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_NetworkServer_SMBEnable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Name(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Vendor(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Model(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_SerialNumber(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_FirmwareVersion(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_ConnectionType(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Removable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Capacity(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Status(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Uptime(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_SMARTCapable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Health(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_HotSwappable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_JCOM_PowerSupply_Interlocking(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Name(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Status(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Enable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_PhysicalReference(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_FileSystem(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Capacity(HUINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_UsedSpace(HUINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Encrypted(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_FolderNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_Name(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Status(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Name(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Initialization(HCHAR *pszOutput);
extern HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(HCHAR *pszOutput);

extern HERROR TRP140_Set_StorageSvc_M_NetInfo_HostName(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_NetInfo_DomainName(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_NetworkServer_AFPEnable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_NetworkServer_NFSEnable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_NetworkServer_SMBEnable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_PhysicalMedium_M_Name(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Name(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Enable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_PhysicalReference(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Capacity(HCHAR *pszOutput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_Name(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_Enable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Status(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Name(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Initialization(HCHAR *pszInput);
extern HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(HCHAR *pszInput);


#define ______________________________TR_157______________________________
#define ______________________STATIC_FUNCTION_TRP_GET______________________
extern HERROR TRP_Get_Device_DeviceSummary(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_Manufacturer(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_ModelName(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_Description(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_ProductClass(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_SerialNumber(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_HardwareVersion(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_AdditionalHardwareVersion(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_SoftwareVersion(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_DeviceStatus(HCHAR *pszOutput);
extern HERROR TRP_Get_DeviceInfo_UpTime(HUINT32 *pszOutput);
extern HERROR TRP_Get_DeviceInfo_FirstUseDate(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_URL(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_Username(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_Password(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_PeriodicInformTime(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_ParameterKey(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput);
extern HERROR TRP_Get_ManagementServer_UpgradesManaged(HCHAR *pszOutput);
extern HERROR TRP_Get_LAN_AddressingType(HCHAR *pszOutput);
extern HERROR TRP_Get_LAN_IPAddress(HCHAR *pszOutput);
extern HERROR TRP_Get_LAN_SubnetMask(HCHAR *pszOutput);
extern HERROR TRP_Get_LAN_DefaultGateway(HCHAR *pszOutput);
extern HERROR TRP_Get_LAN_DNSServers(HCHAR *pszOutput);
extern HERROR TRP_Get_LAN_MACAddress(HCHAR *pszOutput);
extern HERROR TRP_Get_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput);
extern HERROR TRP_Get_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput);
extern HERROR TRP_Get_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput);
extern HERROR TRP_Get_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput);

extern HERROR TRP157_Get_SmartCardReaderNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorConfigFileNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_AdditionalSoftwareVersion(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProvisioningCode(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_UpTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_FirstUseDate(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_DeviceLog(HCHAR *pszOutput);


extern HERROR TRP157_Get_DevInfo_ProcessorNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorLogFileNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_MemoryStatus_Total(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_MemoryStatus_Free(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_CPUUsage(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_ProcessNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_PID(HUINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_Command(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_Size(HUINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_Priority(HUINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_CPUTime(HUINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_State(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensorNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensor_M_Value(HUINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensor_M_MaxValue(HUINT32 *pszOutput);
extern HERROR TRP157_Get_DevInfo_SupportedDataModel_M_URL(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_SupportedDataModel_M_URN(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_SupportedDataModel_M_Features(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Name(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Version(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Date(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Description(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_Processor_M_Architecture(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorLogFile_M_Name(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorLogFile_M_MaximumSize(HCHAR *pszOutput);
extern HERROR TRP157_Get_DevInfo_VendorLogFile_M_Persistent(HCHAR *pszOutput);


extern HERROR TRP157_Set_LAN_Stats_InAdvanceOfGpv_ConnectionUpTime(HBOOL hHomeNetwork);
extern HERROR TRP157_Get_LAN_Stats_ConnectionUpTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_TotalBytesSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_TotalBytesReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_TotalPacketsSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_TotalPacketsReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_CurrentDayInterval(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_CurrentDayBytesSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_CurrentDayBytesReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_CurrentDayPacketsSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_CurrentDayPacketsReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_QuarterHourInterval(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_QuarterHourBytesSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_QuarterHourBytesReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_QuarterHourPacketsSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_LAN_Stats_QuarterHourPacketsReceived(HCHAR *pszOutput);



extern HERROR TRP157_Get_UPnP_Device_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_UPnPMediaServer(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_UPnPMediaRenderer(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_UPnPWLANAccessPoint(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_UPnPQoSDevice(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_UPnPQoSPolicyHolder(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_UPnPIGD(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPArchitecture(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPArchitectureMinorVer(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPMediaServer(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPMediaRenderer(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPWLANAccessPoint(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPBasicDevice(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPQoSDevice(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPQoSPolicyHolder(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Device_Capa_UPnPIGD(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDeviceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_DeviceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_ServiceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDevice_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDevice_UUID(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDevice_USN(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDevice_LeaseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDevice_Location(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_RootDevice_Server(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Device_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Device_UUID(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Device_USN(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Device_LeaseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Device_Location(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Device_Server(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Service_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Service_USN(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Service_LeaseTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Service_Location(HCHAR *pszOutput);
extern HERROR TRP157_Get_UPnP_Discovery_Service_Server(HCHAR *pszOutput);

extern HERROR TRP157_Get_DLNA_Capabilities_HNDDeviceClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_DeviceCapability(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_HIDDeviceClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_ImageClassProfileID(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_AudioClassProfileID(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_AVClassProfileID(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_MediaCollectionProfileID(HCHAR *pszOutput);
extern HERROR TRP157_Get_DLNA_Capabilities_PrinterClassProfileID(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_Name(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_Reset(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_ResetTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_Type(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_Application(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_SerialNumber(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_ATR(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_CardType(HCHAR *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number(HCHAR *pszOutput);
extern HERROR TRP157_Get_SelfTestDiagnostics_DiagnosticsState(HCHAR *pszOutput);
extern HERROR TRP157_Get_SelfTestDiagnostics_Results(HCHAR *pszOutput);



extern HERROR TRP157_Get_USBHosts_HostNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Name(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Type(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Reset(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_PowerManagementEnable(HCHAR *pszOutput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_Reference(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Status(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Name(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_SampleInterval(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ReportSamples(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_TimeReference(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_FetchSamples(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ForceSample(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ReportStartTime(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ReportEndTime(HCHAR *pszInput);
extern HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_SampleSeconds(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_AddressingType(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_IPAddress(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_SubnetMask(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_DefaultGateway(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_HomeNet_DNSServers(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_ConfigMethod(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_SSID(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_SecurityMethod(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_SecurityLevel(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_SecurityKey(HCHAR *pszInput);
extern HERROR TRP157_Set_X_JCOM_Wifi_KeyIndex(HCHAR *pszInput);
extern HERROR TRP157_Set_SoftwareModules_ExecEnv_M_Enable(HCHAR *pszInput);




extern HERROR TRP157_Get_USBHosts_Host_M_USBVersion(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_DeviceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceNumber(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_USBVersion(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceSubClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceVersion(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceProtocol(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_ProductID(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_VendorID(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Manufacturer(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_ProductClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_SerialNumber(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Port(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Rate(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Parent(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_MaxChildren(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_IsSuspended(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_IsSelfPowered(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_ConfigurationNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_ConfigurationNumber(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_InterfaceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceNumber(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceSubClass(HCHAR *pszOutput);
extern HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceProtocol(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_MinSampleInterval(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_MaxReportSamples(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSetNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Name(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_SampleInterval(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ReportSamples(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_TimeReference(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_FetchSamples(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ForceSample(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ReportStartTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ReportEndTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_SampleSeconds(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Reference(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Values(HCHAR *pszOutput);
extern HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Failures(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnvNumberOfEntries(HINT32 *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Name(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Type(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Vendor(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Version(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AllocatedDiskSpace(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AvailableDiskSpace(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AllocatedMemory(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AvailableMemory(HCHAR *pszOutput);
extern HERROR TRP157_Get_SoftwareModules_ExecEnv_M_ActiveExecutionUnits(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Status(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_AddressingType(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_IPAddress(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_SubnetMask(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_DefaultGateway(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_DNSServers(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_MACAddress(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Stats_ConnectionUpTime(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalBytesSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalBytesReceived(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalPacketsSent(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalPacketsReceived(HCHAR *pszOutput);


extern HERROR TRP157_Get_X_JCOM_Wifi_Enable(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_ConfigMethod(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_SSID(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_SecurityMethod(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_SecurityLevel(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_SecurityKey(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_KeyIndex(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_AddressingType(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_IPAddress(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_SubnetMask(HCHAR *pszOutput);
extern HERROR TRP157_Get_X_JCOM_Wifi_DefaultGateway(HCHAR *pszOutput);
extern HERROR TRP157_Get_Capa_PerformanceDiagnostic_DownloadTransports(HCHAR *pszOutput);
extern HERROR TRP157_Get_Capa_PerformanceDiagnostic_UploadTransports(HCHAR *pszOutput);

#define ______________________STATIC_FUNCTION_TRP_SET______________________

//DB FUNCTION
extern HERROR TRP_SET_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput);
extern HERROR TRP_SET_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput);
extern HERROR TRP_SET_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszInput);
extern HERROR TRP_SET_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_URL(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_Username(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_Password(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_PeriodicInformEnable(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_PeriodicInformInterval(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_PeriodicInformTime(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_ConnectionRequestURL(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput);
extern HERROR TRP_SET_ManagementServer_UpgradesManaged(HCHAR *pszInput);
extern HERROR TRP_SET_LAN_AddressingType(HCHAR *pszInput);
extern HERROR TRP_SET_LAN_IPAddress(HCHAR *pszInput);
extern HERROR TRP_SET_LAN_SubnetMask(HCHAR *pszInput);
extern HERROR TRP_SET_LAN_DefaultGateway(HCHAR *pszInput);
extern HERROR TRP_SET_LAN_DNSServers(HCHAR *pszInput);
extern HERROR TRP_SET_LAN_MACAddress(HCHAR *pszInput);
extern HERROR TRP_SET_AudienceMeasurement_CombinedAccess(HCHAR *pszInput);
extern HERROR TRP_SET_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput);
extern HERROR TRP_SET_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput);
extern HERROR TRP_SET_AudienceMeasurement_PermissionLevel(HCHAR *pszInput);

extern HERROR TR135_module_Init();
extern HERROR TR135_module_DeInit();

extern HERROR TR157_module_Init();
extern HERROR TR157_module_Set_NetInfo(NAPINetConfInfo_t* netInfo);
extern HERROR TR157_module_DeInit();

extern HERROR TRP157_Set_UPnP_Device_Enable(HCHAR* pszInput);
extern HERROR TRP157_Set_UPnP_Device_UPnPMediaServer(HCHAR* pszInput);
extern HERROR TRP157_Set_UPnP_Device_UPnPMediaRenderer(HCHAR* pszInput);
extern HERROR TRP157_Set_UPnP_Device_UPnPWLANAccessPoint(HCHAR* pszInput);
extern HERROR TRP157_Set_UPnP_Device_UPnPQoSDevice(HCHAR* pszInput);
extern HERROR TRP157_Set_UPnP_Device_UPnPQoSPolicyHolder(HCHAR* pszInput);
extern HERROR TRP157_Set_UPnP_Device_UPnPIGD(HCHAR* pszInput);
extern HERROR TRP157_Set_SmartCardReader_M_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_SmartCardReader_M_Reset(HCHAR *pszInput);
extern HERROR TRP157_Set_USBHosts_Host_M_Enable(HCHAR *pszInput);
extern HERROR TRP157_Set_USBHosts_Host_M_Reset(HCHAR *pszInput);
extern HERROR TRP157_Set_USBHosts_Host_M_PowerManagementEnable(HCHAR *pszInput);
extern HERROR TRP157_Set_DevInfo_ProvisioningCode(HCHAR *pszInput);

extern HERROR TR181_module_Init(void);
extern HERROR TR181_module_DeInit(void);
extern HERROR TR181_module_Init_WIfi(void);
extern HERROR TR181_NAPI_Init(void);


extern HERROR TR181_get_Device_Wifi_RadioNumberOfEntries(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSIDNumberOfEntries(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPointNumberOfEntries(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_EndPointNumberOfEntries(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_Enable(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_Status(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_Alias(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_Name(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_LastChange(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_LowerLayers(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_BSSID(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_MACAddress(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_SSID(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_fgdn(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_frequency(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_frequencyunits(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_level(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_linkspeedunits(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_networkid(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_rssi(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_hiddenssid(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_SSID_supplicantstate(HINT32 *pszOutput);

extern HERROR TR181_get_Device_Wifi_AccessPoint_Enable(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Status(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Alias(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_SSIDReference(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_SSIDAdvertisementEnabled(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_RetryLimit(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_WMMCapability(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_UAPSDCapability(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_WMMEnable(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_UAPSDEnable(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_AssociatedDeviceNumberOfEntries(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_MaxAssociatedDevices(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_IsolationEnable(HINT32 *pszOutput);

extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_Reset(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_ModesSupported(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_ModeEnabled(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_WEPKey(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_PreSharedKey(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_KeyPassphrase(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_RekeyingInterval(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_RadiusServerIPAddr(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusServerIPAddr(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_RadiusServerPort(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusServerPort(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_RadiusSecret(HINT32 *pszOutput);
extern HERROR TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusSecret(HINT32 *pszOutput);

//extern HERROR _TR181_get_Wifi_AccessPoint_Security_ModeStr(eNapiWifiSecurity	security_type,eNapiWifiAuth auth_type, eNapiWifiEncrypt encrypt_type);



















#ifdef CONFIG_DEBUG
extern HERROR TR181_get_Device_Wifi_SSID_Test(HINT32 *pszOutput);
#endif


extern HERROR rema_cwmp_Init(void);
extern HERROR rema_cwmp_DeInit(void);

#ifdef __cplusplus
}
#endif


#endif
