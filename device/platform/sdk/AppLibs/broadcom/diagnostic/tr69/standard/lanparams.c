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
//  Filename:       lanparams.c
//
******************************************************************************
//  Description:
//             LANDevice profile supported objects
//
*****************************************************************************/

#include "sharedparams.h"
#include "lanparams.h" 
#ifdef WIRELESS
#include "wirelessparams.h" 
#endif /* #ifdef WIRELESS*/

/** */
/** InternetGatewayDevice.LANDevice.LANHostConfigManagement.IPInterface. */

TRXGFUNC(getLANDeviceLANHostConfigManagementIPInterface_Enable);
TRXSFUNC(setLANDeviceLANHostConfigManagementIPInterface_Enable);
TRXGFUNC(getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress);
TRXSFUNC(setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress);
TRXGFUNC(getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask);
TRXSFUNC(setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask);
TRXGFUNC(getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType);
TRXSFUNC(setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType);
TRxObjNode LANDeviceLANHostConfigManagementIPInterfaceDesc[] = {
	{Enable,{{tBool,0,0}},
	  setLANDeviceLANHostConfigManagementIPInterface_Enable,
	  getLANDeviceLANHostConfigManagementIPInterface_Enable,
	  NULL,NULL},
	{IPInterfaceIPAddress,{{tString,16,0}},
	  setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress,
	  getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress,
	  NULL,NULL},
	{IPInterfaceSubnetMask,{{tString,16,0}},
	  setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask,
	  getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask,
	  NULL,NULL},
	{IPInterfaceAddressingType,{{tString,16,0}},
	  setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType,
	  getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.LANHostConfigManagement.IPInterface. */

TRXGFUNC(addLANDeviceLANHostConfigManagementIPInterfaceI);
TRXSFUNC(delLANDeviceLANHostConfigManagementIPInterfaceI);
TRxObjNode LANDeviceLANHostConfigManagementIPInterfaceInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  NULL, /* delLANDeviceLANHostConfigManagementIPInterfaceI,*/
	  NULL, /* addLANDeviceLANHostConfigManagementIPInterfaceI,*/
	  LANDeviceLANHostConfigManagementIPInterfaceDesc, NULL},
};

/** */
/** InternetGatewayDevice.LANDevice.LANHostConfigManagement. */

TRXGFUNC(getLANDeviceLANHostConfigManagement_DHCPServerConfigurable);
TRXSFUNC(setLANDeviceLANHostConfigManagement_DHCPServerConfigurable);
TRXGFUNC(getLANDeviceLANHostConfigManagement_DHCPServerEnable);
TRXSFUNC(setLANDeviceLANHostConfigManagement_DHCPServerEnable);
TRXGFUNC(getLANDeviceLANHostConfigManagement_DHCPRelay);
TRXGFUNC(getLANDeviceLANHostConfigManagement_MinAddress);
TRXSFUNC(setLANDeviceLANHostConfigManagement_MinAddress);
TRXGFUNC(getLANDeviceLANHostConfigManagement_MaxAddress);
TRXSFUNC(setLANDeviceLANHostConfigManagement_MaxAddress);
TRXGFUNC(getLANDeviceLANHostConfigManagement_ReservedAddresses);
TRXSFUNC(setLANDeviceLANHostConfigManagement_ReservedAddresses);
TRXGFUNC(getLANDeviceLANHostConfigManagement_SubnetMask);
TRXSFUNC(setLANDeviceLANHostConfigManagement_SubnetMask);
TRXGFUNC(getLANDeviceLANHostConfigManagement_DNSServers);
TRXSFUNC(setLANDeviceLANHostConfigManagement_DNSServers);
TRXGFUNC(getLANDeviceLANHostConfigManagement_DomainName);
TRXSFUNC(setLANDeviceLANHostConfigManagement_DomainName);
TRXGFUNC(getLANDeviceLANHostConfigManagement_IPRouters);
TRXSFUNC(setLANDeviceLANHostConfigManagement_IPRouters);
TRXGFUNC(getLANDeviceLANHostConfigManagement_DHCPLeaseTime);
TRXSFUNC(setLANDeviceLANHostConfigManagement_DHCPLeaseTime);
TRXGFUNC(getLANDeviceLANHostConfigManagement_UseAllocatedWAN);
TRXSFUNC(setLANDeviceLANHostConfigManagement_UseAllocatedWAN);
TRXGFUNC(getLANDeviceLANHostConfigManagement_AssociatedConnection);
TRXSFUNC(setLANDeviceLANHostConfigManagement_AssociatedConnection);
TRXGFUNC(getLANDeviceLANHostConfigManagement_PassthroughLease);
TRXSFUNC(setLANDeviceLANHostConfigManagement_PassthroughLease);
TRXGFUNC(getLANDeviceLANHostConfigManagement_PassthroughMACAddress);
TRXSFUNC(setLANDeviceLANHostConfigManagement_PassthroughMACAddress);
TRXGFUNC(getLANDeviceLANHostConfigManagement_AllowedMACAddresses);
TRXSFUNC(setLANDeviceLANHostConfigManagement_AllowedMACAddresses);
TRXGFUNC(getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries);
TRXGFUNC(addLANDeviceLANHostConfigManagementI);
TRXSFUNC(delLANDeviceLANHostConfigManagementI);
TRxObjNode LANDeviceLANHostConfigManagementDesc[] = {
	#if 0
	{DHCPServerConfigurable,{{tBool,0}},
	  setLANDeviceLANHostConfigManagement_DHCPServerConfigurable,
	  getLANDeviceLANHostConfigManagement_DHCPServerConfigurable,
	  NULL,NULL},
	#endif
	{DHCPServerEnable,{{tBool,0,0}},
	  setLANDeviceLANHostConfigManagement_DHCPServerEnable,
	  getLANDeviceLANHostConfigManagement_DHCPServerEnable,
	  NULL,NULL},
	#if  0
	{DHCPRelay,{{tBool,0}},
	  NULL,
	  getLANDeviceLANHostConfigManagement_DHCPRelay,
	  NULL,NULL},
	{MinAddress,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_MinAddress,
	  getLANDeviceLANHostConfigManagement_MinAddress,
	  NULL,NULL},
	{MaxAddress,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_MaxAddress,
	  getLANDeviceLANHostConfigManagement_MaxAddress,
	  NULL,NULL},
	{ReservedAddresses,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_ReservedAddresses,
	  getLANDeviceLANHostConfigManagement_ReservedAddresses,
	  NULL,NULL},
	{SubnetMask,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_SubnetMask,
	  getLANDeviceLANHostConfigManagement_SubnetMask,
	  NULL,NULL},
	#endif
	{DNSServers,{{tString,16,0}},
	  setLANDeviceLANHostConfigManagement_DNSServers,
	  getLANDeviceLANHostConfigManagement_DNSServers,
	  NULL,NULL},
	{DomainName,{{tString,64,0}},
	  setLANDeviceLANHostConfigManagement_DomainName,
	  getLANDeviceLANHostConfigManagement_DomainName,
	  NULL,NULL},
	{IPRouters,{{tString,16,0}},
	  setLANDeviceLANHostConfigManagement_IPRouters,
	  getLANDeviceLANHostConfigManagement_IPRouters,
	  NULL,NULL},
	{DHCPLeaseTime,{{tInt,0,0}},
	  setLANDeviceLANHostConfigManagement_DHCPLeaseTime,
	  getLANDeviceLANHostConfigManagement_DHCPLeaseTime,
	  NULL,NULL},
	#if 0
	{UseAllocatedWAN,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_UseAllocatedWAN,
	  getLANDeviceLANHostConfigManagement_UseAllocatedWAN,
	  NULL,NULL},
	{AssociatedConnection,{{tString,256}},
	  setLANDeviceLANHostConfigManagement_AssociatedConnection,
	  getLANDeviceLANHostConfigManagement_AssociatedConnection,
	  NULL,NULL},
	{PassthroughLease,{{tUnsigned,0}},
	  setLANDeviceLANHostConfigManagement_PassthroughLease,
	  getLANDeviceLANHostConfigManagement_PassthroughLease,
	  NULL,NULL},
	{PassthroughMACAddress,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_PassthroughMACAddress,
	  getLANDeviceLANHostConfigManagement_PassthroughMACAddress,
	  NULL,NULL},
	{AllowedMACAddresses,{{tString,16}},
	  setLANDeviceLANHostConfigManagement_AllowedMACAddresses,
	  getLANDeviceLANHostConfigManagement_AllowedMACAddresses,
	  NULL,NULL},
	{IPInterfaceNumberOfEntries,{{tUnsigned,0}},
	  NULL,
	  getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries,
	  NULL,NULL},
	#endif
	{IPInterface,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANHostConfigManagementIPInterfaceInstanceDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig.Stats. */
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfigStats_BytesSent);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfigStats_BytesReceived);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfigStats_PacketsSent);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfigStats_PacketsReceived);
TRxObjNode LANDeviceLANEthernetInterfaceConfigStatsDesc[] = {
	{BytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANEthernetInterfaceConfigStats_BytesSent,
	  NULL,NULL},
	{BytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANEthernetInterfaceConfigStats_BytesReceived,
	  NULL,NULL},
	{PacketsSent,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANEthernetInterfaceConfigStats_PacketsSent,
	  NULL,NULL},
	{PacketsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANEthernetInterfaceConfigStats_PacketsReceived,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig. */

TRXGFUNC(getLANDeviceLANEthernetInterfaceConfig_Enable);
TRXSFUNC(setLANDeviceLANEthernetInterfaceConfig_Enable);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfig_Status);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfig_MACAddress);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled);
TRXSFUNC(setLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfig_MaxBitRate);
TRXSFUNC(setLANDeviceLANEthernetInterfaceConfig_MaxBitRate);
TRXGFUNC(getLANDeviceLANEthernetInterfaceConfig_DuplexMode);
TRXSFUNC(setLANDeviceLANEthernetInterfaceConfig_DuplexMode);
TRxObjNode LANDeviceLANEthernetInterfaceConfigDesc[] = {
	{Enable,{{tBool,0,0}},
	  setLANDeviceLANEthernetInterfaceConfig_Enable,
	  getLANDeviceLANEthernetInterfaceConfig_Enable,
	  NULL,NULL},
	{Status,{{tString,16,0}},
	  NULL,
	  getLANDeviceLANEthernetInterfaceConfig_Status,
	  NULL,NULL},
	{MACAddress,{{tString,16,0}},
	  NULL,
	  getLANDeviceLANEthernetInterfaceConfig_MACAddress,
	  NULL,NULL},
	#if 0
	{MACAddressControlEnabled,{{tBool,0}},
	  setLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled,
	  getLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled,
	  NULL,NULL},
	{MaxBitRate,{{tString,16}},
	  setLANDeviceLANEthernetInterfaceConfig_MaxBitRate,
	  getLANDeviceLANEthernetInterfaceConfig_MaxBitRate,
	  NULL,NULL},
	{DuplexMode,{{tString,16}},
	  setLANDeviceLANEthernetInterfaceConfig_DuplexMode,
	  getLANDeviceLANEthernetInterfaceConfig_DuplexMode,
	  NULL,NULL},
	#endif
	{Stats,{{tObject,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANEthernetInterfaceConfigStatsDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig. */

TRxObjNode LANDeviceLANEthernetInterfaceConfigInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANEthernetInterfaceConfigDesc,NULL},
};

/** */
/** InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig.Stats. */

TRXGFUNC(getLANDeviceLANUSBInterfaceConfigStats_BytesSent);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfigStats_BytesReceived);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfigStats_CellsSent);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfigStats_CellsReceived);
TRxObjNode LANDeviceLANUSBInterfaceConfigStatsDesc[] = {
	{BytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfigStats_BytesSent,
	  NULL,NULL},
	{BytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfigStats_BytesReceived,
	  NULL,NULL},
	{CellsSent,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfigStats_CellsSent,
	  NULL,NULL},
	{CellsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfigStats_CellsReceived,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig. */

TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_Enable);
TRXSFUNC(setLANDeviceLANUSBInterfaceConfig_Enable);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_Status);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_MACAddress);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_MACAddressControlEnabled);
TRXSFUNC(setLANDeviceLANUSBInterfaceConfig_MACAddressControlEnabled);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_Standard);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_Type);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_Rate);
TRXGFUNC(getLANDeviceLANUSBInterfaceConfig_Power);
TRxObjNode LANDeviceLANUSBInterfaceConfigDesc[] = {
	{Enable,{{tBool,0,0}},
	  setLANDeviceLANUSBInterfaceConfig_Enable,
	  getLANDeviceLANUSBInterfaceConfig_Enable,
	  NULL,NULL},
	{Status,{{tString,16,0}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfig_Status,
	  NULL,NULL},
	#if 0
	{MACAddress,{{tString,16}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfig_MACAddress,
	  NULL,NULL},
	{MACAddressControlEnabled,{{tBool,0}},
	  setLANDeviceLANUSBInterfaceConfig_MACAddressControlEnabled,
	  getLANDeviceLANUSBInterfaceConfig_MACAddressControlEnabled,
	  NULL,NULL},
	{Standard,{{tString,6}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfig_Standard,
	  NULL,NULL},
	{Type,{{tString,16}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfig_Type,
	  NULL,NULL},
	{Rate,{{tString,16}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfig_Rate,
	  NULL,NULL},
	{Power,{{tString,16}},
	  NULL,
	  getLANDeviceLANUSBInterfaceConfig_Power,
	  NULL,NULL},
	#endif
	{Stats,{{tObject,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANUSBInterfaceConfigStatsDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig. */

TRxObjNode LANDeviceLANUSBInterfaceConfigInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANUSBInterfaceConfigDesc,NULL},
};

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.AssociatedDevice. */
#if 0
TRXGFUNC(getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress);
TRXGFUNC(getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress);
TRXGFUNC(getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState);
TRXGFUNC(getLANDeviceWLANConfigurationAssociatedDevice_LastRequestedUnicastCipher);
TRXGFUNC(getLANDeviceWLANConfigurationAssociatedDevice_LastRequestedMulticastCipher);
TRXGFUNC(getLANDeviceWLANConfigurationAssociatedDevice_LastPMKId);
TRxObjNode LANDeviceWLANConfigurationAssociatedDeviceDesc[] = {
	{AssociatedDeviceMACAddress,{{tString,16,1}},
	  NULL,
	  getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress,
	  NULL,NULL},
	{AssociatedDeviceIPAddress,{{tString,64,1}},
	  NULL,
	  getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress,
	  NULL,NULL},
	{AssociatedDeviceAuthenticationState,{{tBool,0,1}},
	  NULL,
	  getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState,
	  NULL,NULL},
	{LastRequestedUnicastCipher,{{tString,256,1}},
	  NULL,
	  getLANDeviceWLANConfigurationAssociatedDevice_LastRequestedUnicastCipher,
	  NULL,NULL},
	{LastRequestedMulticastCipher,{{tString,256,1}},
	  NULL,
	  getLANDeviceWLANConfigurationAssociatedDevice_LastRequestedMulticastCipher,
	  NULL,NULL},
	{LastPMKId,{{tString,256,1}},
	  NULL,
	  getLANDeviceWLANConfigurationAssociatedDevice_LastPMKId,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.AssociatedDevice. */

TRxObjNode LANDeviceWLANConfigurationAssociatedDeviceInstanceDesc[] = {
	{instanceIDMASK,{{0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationAssociatedDeviceDesc},
};
#endif

#if 0
/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.PreSharedKey. */

TRXSFUNC(setLANDeviceWLANConfigurationPreSharedKey_PreSharedKey);
TRXSFUNC(setLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase);
TRXGFUNC(getLANDeviceWLANConfigurationPreSharedKey_AssociatedDeviceMACAddress);
TRXSFUNC(setLANDeviceWLANConfigurationPreSharedKey_AssociatedDeviceMACAddress);
TRxObjNode LANDeviceWLANConfigurationPreSharedKeyDesc[] = {
	{PreSharedKey,{{tStringSOnly,64}},
	  setLANDeviceWLANConfigurationPreSharedKey_PreSharedKey,
	  NULL,
	  NULL,NULL},
	{KeyPassphrase,{{tStringSOnly,63}},
	  setLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase,
	  NULL,
	  NULL,NULL},
	{AssociatedDeviceMACAddress,{{tString,16}},
	  setLANDeviceWLANConfigurationPreSharedKey_AssociatedDeviceMACAddress,
	  getLANDeviceWLANConfigurationPreSharedKey_AssociatedDeviceMACAddress,
	  NULL,NULL},
	{NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.PreSharedKey. */

TRxObjNode LANDeviceWLANConfigurationPreSharedKeyInstanceDesc[] = {
	{instanceIDMASK,{{0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationPreSharedKeyDesc},
};
#endif

/** */
/** InternetGatewayDevice.LANDevice.Hosts.Host. */

TRXGFUNC(getLANDeviceHostsHost_IPAddress);
TRXGFUNC(getLANDeviceHostsHost_AddressSource);
TRXGFUNC(getLANDeviceHostsHost_LeaseTimeRemaining);
TRXGFUNC(getLANDeviceHostsHost_MACAddress);
TRXGFUNC(getLANDeviceHostsHost_HostName);
TRXGFUNC(getLANDeviceHostsHost_InterfaceType);
TRXGFUNC(getLANDeviceHostsHost_Active);
TRxObjNode LANDeviceHostsHostDesc[] = {
	{IPAddress,{{tString,16,0}},
	  NULL,
	  getLANDeviceHostsHost_IPAddress,
	  NULL,NULL},
	{AddressSource,{{tString,16,0}},
	  NULL,
	  getLANDeviceHostsHost_AddressSource,
	  NULL,NULL},
	{LeaseTimeRemaining,{{tInt,0,1}},
	  NULL,
	  getLANDeviceHostsHost_LeaseTimeRemaining,
	  NULL,NULL},
	{MACAddress,{{tString,16,0}},
	  NULL,
	  getLANDeviceHostsHost_MACAddress,
	  NULL,NULL},
	#if 0
	{HostName,{{tString,64}},
	  NULL,
	  getLANDeviceHostsHost_HostName,
	  NULL,NULL},
	#endif
	{InterfaceType,{{tString,16,0}},
	  NULL,
	  getLANDeviceHostsHost_InterfaceType,
	  NULL,NULL},
	{Active,{{tBool,0,0}},
	  NULL,
	  getLANDeviceHostsHost_Active,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice.Hosts.Host. */

TRxObjNode LANDeviceHostsHostInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  NULL,
	  NULL,
	  LANDeviceHostsHostDesc,NULL},
};

/** */
/** InternetGatewayDevice.LANDevice.Hosts. */

TRXGFUNC(getLANDeviceHosts_HostNumberOfEntries);
TRxObjNode LANDeviceHostsDesc[] = {
	{HostNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getLANDeviceHosts_HostNumberOfEntries,
	  NULL,NULL},
	{Host,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  LANDeviceHostsHostInstanceDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice. */

TRXGFUNC(getLANDevice_LANEthernetInterfaceNumberOfEntries);
TRXGFUNC(getLANDevice_LANUSBInterfaceNumberOfEntries);

#ifdef WIRELESS
TRXGFUNC(getLANDevice_LANWLANConfigurationNumberOfEntries);
extern TRxObjNode LANDeviceWLANConfigurationInstanceDesc[];
#endif /* #ifdef WIRELESS*/

TRxObjNode LANDeviceDesc[] = {
	{LANEthernetInterfaceNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getLANDevice_LANEthernetInterfaceNumberOfEntries,
	  NULL,NULL},
	{LANUSBInterfaceNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getLANDevice_LANUSBInterfaceNumberOfEntries,
	  NULL,NULL},
#ifdef WIRELESS
	{LANWLANConfigurationNumberOfEntries,{{tUnsigned,0}},
	  NULL,
	  getLANDevice_LANWLANConfigurationNumberOfEntries,
	  NULL,NULL},
#endif /* #ifdef WIRELESS*/
	{LANHostConfigManagement,{{tObject,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANHostConfigManagementDesc,NULL},
	{LANEthernetInterfaceConfig,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANEthernetInterfaceConfigInstanceDesc,NULL},
	{LANUSBInterfaceConfig,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  LANDeviceLANUSBInterfaceConfigInstanceDesc,NULL},
#ifdef WIRELESS
	{WLANConfiguration,{{tInstance,0}},
	  NULL,
	  NULL,
	  LANDeviceWLANConfigurationInstanceDesc},
#endif /* #ifdef WIRELESS*/
	{Hosts,{{tObject,0,0}},
	  NULL,
	  NULL,
	  LANDeviceHostsDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.LANDevice. */

TRxObjNode LANDeviceInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  NULL,
	  NULL,
	  LANDeviceDesc,NULL},
};
