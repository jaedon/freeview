/*****************************************************************************
//
//  Copyright (c) 2005  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       wirelessparams.c
//
******************************************************************************
//  Description:
//             Wireless supported objects
//
*****************************************************************************/

#ifdef WIRELESS

#include "sharedparams.h"
#include "wirelessparams.h" 

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.WEPKey. */

TRXSFUNC(setLANDeviceWLANConfigurationWEPKey_WEPKey);
TRXGFUNC(getLANDeviceWLANConfigurationWEPKey_WEPKey);
TRxObjNode LANDeviceWLANConfigurationWEPKeyDesc[] = {
	{WEPKey,{{tStringSOnly,128}},
	  setLANDeviceWLANConfigurationWEPKey_WEPKey,
	  getLANDeviceWLANConfigurationWEPKey_WEPKey,
	  NULL},
	{NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.WEPKey. */

TRxObjNode LANDeviceWLANConfigurationWEPKeyInstanceDesc[] = {
	{instanceIDMASK,{{0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationWEPKeyDesc},
};

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration. */
/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.{i}. */

TRXGFUNC(getLANDeviceWLANConfiguration_Enable);
TRXSFUNC(setLANDeviceWLANConfiguration_Enable);
TRXGFUNC(getLANDeviceWLANConfiguration_Status);
TRXGFUNC(getLANDeviceWLANConfiguration_BSSID);
TRXGFUNC(getLANDeviceWLANConfiguration_MaxBitRate);
TRXSFUNC(setLANDeviceWLANConfiguration_MaxBitRate);
TRXGFUNC(getLANDeviceWLANConfiguration_Channel);
TRXSFUNC(setLANDeviceWLANConfiguration_Channel);
TRXGFUNC(getLANDeviceWLANConfiguration_SSID);
TRXSFUNC(setLANDeviceWLANConfiguration_SSID);
TRXGFUNC(getLANDeviceWLANConfiguration_BeaconType);
TRXSFUNC(setLANDeviceWLANConfiguration_BeaconType);
TRXGFUNC(getLANDeviceWLANConfiguration_MACAddressControlEnabled);
TRXSFUNC(setLANDeviceWLANConfiguration_MACAddressControlEnabled);
TRXGFUNC(getLANDeviceWLANConfiguration_Standard);
TRXGFUNC(getLANDeviceWLANConfiguration_WEPKeyIndex);
TRXSFUNC(setLANDeviceWLANConfiguration_WEPKeyIndex);
TRXSFUNC(setLANDeviceWLANConfiguration_KeyPassphrase);
TRXGFUNC(getLANDeviceWLANConfiguration_WEPEncryptionLevel);
TRXGFUNC(getLANDeviceWLANConfiguration_BasicEncryptionModes);
TRXSFUNC(setLANDeviceWLANConfiguration_BasicEncryptionModes);
TRXGFUNC(getLANDeviceWLANConfiguration_BasicAuthenticationMode);
TRXSFUNC(setLANDeviceWLANConfiguration_BasicAuthenticationMode);
TRXGFUNC(getLANDeviceWLANConfiguration_WPAEncryptionModes);
TRXSFUNC(setLANDeviceWLANConfiguration_WPAEncryptionModes);
TRXGFUNC(getLANDeviceWLANConfiguration_WPAAuthenticationMode);
TRXSFUNC(setLANDeviceWLANConfiguration_WPAAuthenticationMode);
TRXGFUNC(getLANDeviceWLANConfiguration_IEEE11iEncryptionModes);
TRXSFUNC(setLANDeviceWLANConfiguration_IEEE11iEncryptionModes);
TRXGFUNC(getLANDeviceWLANConfiguration_IEEE11iAuthenticationMode);
TRXSFUNC(setLANDeviceWLANConfiguration_IEEE11iAuthenticationMode);
TRXGFUNC(getLANDeviceWLANConfiguration_PossibleChannels);
TRXGFUNC(getLANDeviceWLANConfiguration_BasicDataTransmitRates);
TRXSFUNC(setLANDeviceWLANConfiguration_BasicDataTransmitRates);
TRXGFUNC(getLANDeviceWLANConfiguration_OperationalDataTransmitRates);
TRXSFUNC(setLANDeviceWLANConfiguration_OperationalDataTransmitRates);
TRXGFUNC(getLANDeviceWLANConfiguration_PossibleDataTransmitRates);
TRXGFUNC(getLANDeviceWLANConfiguration_InsecureOOBAccessEnabled);
TRXSFUNC(setLANDeviceWLANConfiguration_InsecureOOBAccessEnabled);
TRXGFUNC(getLANDeviceWLANConfiguration_BeaconAdvertisementEnabled);
TRXSFUNC(setLANDeviceWLANConfiguration_BeaconAdvertisementEnabled);
TRXGFUNC(getLANDeviceWLANConfiguration_RadioEnabled);
TRXSFUNC(setLANDeviceWLANConfiguration_RadioEnabled);
TRXGFUNC(getLANDeviceWLANConfiguration_AutoRateFallBackEnabled);
TRXSFUNC(setLANDeviceWLANConfiguration_AutoRateFallBackEnabled);
TRXGFUNC(getLANDeviceWLANConfiguration_LocationDescription);
TRXSFUNC(setLANDeviceWLANConfiguration_LocationDescription);
TRXGFUNC(getLANDeviceWLANConfiguration_RegulatoryDomain);
TRXSFUNC(setLANDeviceWLANConfiguration_RegulatoryDomain);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalPSKFailures);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalIntegrityFailures);
TRXGFUNC(getLANDeviceWLANConfiguration_ChannelsInUse);
TRXGFUNC(getLANDeviceWLANConfiguration_DeviceOperationMode);
TRXSFUNC(setLANDeviceWLANConfiguration_DeviceOperationMode);
TRXGFUNC(getLANDeviceWLANConfiguration_DistanceFromRoot);
TRXSFUNC(setLANDeviceWLANConfiguration_DistanceFromRoot);
TRXGFUNC(getLANDeviceWLANConfiguration_PeerBSSID);
TRXSFUNC(setLANDeviceWLANConfiguration_PeerBSSID);
TRXGFUNC(getLANDeviceWLANConfiguration_AuthenticationServiceMode);
TRXSFUNC(setLANDeviceWLANConfiguration_AuthenticationServiceMode);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalBytesSent);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalBytesReceived);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalPacketsSent);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalPacketsReceived);
TRXGFUNC(getLANDeviceWLANConfiguration_TotalAssociations);
TRxObjNode LANDeviceWLANConfigurationDesc[] = {
	{Enable,{{tBool,0}},
	  setLANDeviceWLANConfiguration_Enable,
	  getLANDeviceWLANConfiguration_Enable,
	  NULL,NULL},
	{Status,{{tString,16}},
	  NULL,
	  getLANDeviceWLANConfiguration_Status,
	  NULL,NULL},
	#if 0
	{BSSID,{{tString,16}},
	  NULL,
	  getLANDeviceWLANConfiguration_BSSID,
	  NULL,NULL},
	{MaxBitRate,{{tString,4}},
	  setLANDeviceWLANConfiguration_MaxBitRate,
	  getLANDeviceWLANConfiguration_MaxBitRate,
	  NULL,NULL},
	#endif
	{Channel,{{tUnsigned,0}},
	  setLANDeviceWLANConfiguration_Channel,
	  getLANDeviceWLANConfiguration_Channel,
	  NULL,NULL},
	{SSID,{{tString,32}},
	  setLANDeviceWLANConfiguration_SSID,
	  getLANDeviceWLANConfiguration_SSID,
	  NULL,NULL},
	#if 0
	{BeaconType,{{tString,16}},
	  setLANDeviceWLANConfiguration_BeaconType,
	  getLANDeviceWLANConfiguration_BeaconType,
	  NULL,NULL},
	#endif
	{MACAddressControlEnabled,{{tBool,0}},
	  setLANDeviceWLANConfiguration_MACAddressControlEnabled,
	  getLANDeviceWLANConfiguration_MACAddressControlEnabled,
	  NULL,NULL},
	#if 0
	{Standard,{{tString,16}},
	  NULL,
	  getLANDeviceWLANConfiguration_Standard,
	  NULL,NULL},
	#endif
	{WEPKeyIndex,{{tUnsigned,0}},
	  setLANDeviceWLANConfiguration_WEPKeyIndex,
	  getLANDeviceWLANConfiguration_WEPKeyIndex,
	  NULL,NULL},
	#if 0
	{KeyPassphrase,{{tStringSOnly,63}},
	  setLANDeviceWLANConfiguration_KeyPassphrase,
	  NULL,
	  NULL,NULL},
	#endif
	{WEPEncryptionLevel,{{tString,64}},
	  NULL,
	  getLANDeviceWLANConfiguration_WEPEncryptionLevel,
	  NULL,NULL},
	{BasicEncryptionModes,{{tString,31}},
	  setLANDeviceWLANConfiguration_BasicEncryptionModes,
	  getLANDeviceWLANConfiguration_BasicEncryptionModes,
	  NULL,NULL},
	#if 0
	{BasicAuthenticationMode,{{tString,31}},
	  setLANDeviceWLANConfiguration_BasicAuthenticationMode,
	  getLANDeviceWLANConfiguration_BasicAuthenticationMode,
	  NULL,NULL},
	{WPAEncryptionModes,{{tString,31}},
	  setLANDeviceWLANConfiguration_WPAEncryptionModes,
	  getLANDeviceWLANConfiguration_WPAEncryptionModes,
	  NULL,NULL},
	{WPAAuthenticationMode,{{tString,31}},
	  setLANDeviceWLANConfiguration_WPAAuthenticationMode,
	  getLANDeviceWLANConfiguration_WPAAuthenticationMode,
	  NULL,NULL},
	{IEEE11iEncryptionModes,{{tString,31}},
	  setLANDeviceWLANConfiguration_IEEE11iEncryptionModes,
	  getLANDeviceWLANConfiguration_IEEE11iEncryptionModes,
	  NULL,NULL},
	{IEEE11iAuthenticationMode,{{tString,31}},
	  setLANDeviceWLANConfiguration_IEEE11iAuthenticationMode,
	  getLANDeviceWLANConfiguration_IEEE11iAuthenticationMode,
	  NULL,NULL},
	{PossibleChannels,{{tString,1024}},
	  NULL,
	  getLANDeviceWLANConfiguration_PossibleChannels,
	  NULL,NULL},
	{BasicDataTransmitRates,{{tString,256}},
	  setLANDeviceWLANConfiguration_BasicDataTransmitRates,
	  getLANDeviceWLANConfiguration_BasicDataTransmitRates,
	  NULL,NULL},
	{OperationalDataTransmitRates,{{tString,256}},
	  setLANDeviceWLANConfiguration_OperationalDataTransmitRates,
	  getLANDeviceWLANConfiguration_OperationalDataTransmitRates,
	  NULL,NULL},
	{PossibleDataTransmitRates,{{tString,256}},
	  NULL,
	  getLANDeviceWLANConfiguration_PossibleDataTransmitRates,
	  NULL,NULL},
	{InsecureOOBAccessEnabled,{{tBool,0}},
	  setLANDeviceWLANConfiguration_InsecureOOBAccessEnabled,
	  getLANDeviceWLANConfiguration_InsecureOOBAccessEnabled,
	  NULL,NULL},
	{BeaconAdvertisementEnabled,{{tBool,0}},
	  setLANDeviceWLANConfiguration_BeaconAdvertisementEnabled,
	  getLANDeviceWLANConfiguration_BeaconAdvertisementEnabled,
	  NULL,NULL},
	#endif 
	#if 0
	{RadioEnabled,{{tBool,0}},
	  setLANDeviceWLANConfiguration_RadioEnabled,
	  getLANDeviceWLANConfiguration_RadioEnabled,
	  NULL,NULL},
	{AutoRateFallBackEnabled,{{tBool,0}},
	  setLANDeviceWLANConfiguration_AutoRateFallBackEnabled,
	  getLANDeviceWLANConfiguration_AutoRateFallBackEnabled,
	  NULL,NULL},
	{LocationDescription,{{tString,4096}},
	  setLANDeviceWLANConfiguration_LocationDescription,
	  getLANDeviceWLANConfiguration_LocationDescription,
	  NULL,NULL},
	{RegulatoryDomain,{{tString,3}},
	  setLANDeviceWLANConfiguration_RegulatoryDomain,
	  getLANDeviceWLANConfiguration_RegulatoryDomain,
	  NULL,NULL},
	{TotalPSKFailures,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalPSKFailures,
	  NULL,NULL},
	{TotalIntegrityFailures,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalIntegrityFailures,
	  NULL,NULL},
	{ChannelsInUse,{{tString,1024,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_ChannelsInUse,
	  NULL,NULL},
	{DeviceOperationMode,{{tString,31}},
	  setLANDeviceWLANConfiguration_DeviceOperationMode,
	  getLANDeviceWLANConfiguration_DeviceOperationMode,
	  NULL,NULL},
	{DistanceFromRoot,{{tUnsigned,0}},
	  setLANDeviceWLANConfiguration_DistanceFromRoot,
	  getLANDeviceWLANConfiguration_DistanceFromRoot,
	  NULL,NULL},
	{PeerBSSID,{{tString,16}},
	  setLANDeviceWLANConfiguration_PeerBSSID,
	  getLANDeviceWLANConfiguration_PeerBSSID,
	  NULL,NULL},
	{AuthenticationServiceMode,{{tString,16}},
	  setLANDeviceWLANConfiguration_AuthenticationServiceMode,
	  getLANDeviceWLANConfiguration_AuthenticationServiceMode,
	  NULL,NULL},
	#endif	  
	{TotalBytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalBytesSent,
	  NULL,NULL},
	{TotalBytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalBytesReceived,
	  NULL,NULL},
	{TotalPacketsSent,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalPacketsSent,
	  NULL,NULL},
	{TotalPacketsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalPacketsReceived,
	  NULL,NULL},
	#if 0	  
	{TotalAssociations,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceWLANConfiguration_TotalAssociations,
	  NULL,NULL},
	{AssociatedDevice,{{tInstance,0,1}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationAssociatedDeviceInstanceDesc},
	#endif
	{WEPKey,{{tInstance,0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationWEPKeyInstanceDesc},
	#if 0
	{PreSharedKey,{{tInstance,0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationPreSharedKeyInstanceDesc},
	#endif
	{NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration. */

TRxObjNode LANDeviceWLANConfigurationInstanceDesc[] = {
	{instanceIDMASK,{{0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationDesc},
};

#endif // #ifdef WIRELESS
