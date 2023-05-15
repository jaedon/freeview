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
//  Filename:       lanparams.h
//
******************************************************************************
//  Description:
//             LANDevice profile supported object varable names
//
//
*****************************************************************************/

#ifndef LAN_PARAMS_H
#define LAN_PARAMS_H

#include "../inc/tr69cdefs.h"

/* */
/* InternetGatewayDevice.LANDevice.LANHostConfigManagement.IPInterface. */

SVAR(IPInterfaceIPAddress);
SVAR(IPInterfaceSubnetMask);
SVAR(IPInterfaceAddressingType);

/* */
/* InternetGatewayDevice.LANDevice.LANHostConfigManagement.IPInterface. */


/* */
/* InternetGatewayDevice.LANDevice.LANHostConfigManagement. */

SVAR(DHCPServerConfigurable);
SVAR(DHCPServerEnable);
SVAR(DHCPRelay);
SVAR(MinAddress);
SVAR(MaxAddress);
SVAR(ReservedAddresses);
SVAR(DomainName);
SVAR(IPRouters);
SVAR(DHCPLeaseTime);
SVAR(UseAllocatedWAN);
SVAR(AssociatedConnection);
SVAR(PassthroughLease);
SVAR(PassthroughMACAddress);
SVAR(AllowedMACAddresses);
SVAR(IPInterfaceNumberOfEntries);
SVAR(IPInterface);

/* */
/* InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig. */

SVAR(MACAddress);
SVAR(MaxBitRate);
SVAR(DuplexMode);

/* */
/* InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig. */


/* */
/* InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig.Stats. */

#if 0
/*SVAR(BytesSent); */
/*SVAR(BytesReceived); */
#endif
SVAR(CellsSent);
SVAR(CellsReceived);

/* */
/* InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig. */

#if 0
//SVAR(Enable);
//SVAR(Status);
//SVAR(MACAddress);
#endif

SVAR(Standard);
SVAR(Type);
SVAR(Rate);
SVAR(Power);
/*SVAR(Stats);*/

/* */
/* InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig. */

/* */
/* InternetGatewayDevice.LANDevice.Hosts.Host. */

SVAR(IPAddress);
SVAR(AddressSource);
SVAR(LeaseTimeRemaining);
/*SVAR(MACAddress);*/
SVAR(HostName);
SVAR(InterfaceType);
SVAR(Active);

/* */
/* InternetGatewayDevice.LANDevice.Hosts.Host. */


/* */
/* InternetGatewayDevice.LANDevice.Hosts. */

SVAR(HostNumberOfEntries);

/* */
/* InternetGatewayDevice.LANDevice. */

#if 0
//SVAR(LANEthernetInterfaceNumberOfEntries);
//SVAR(LANUSBInterfaceNumberOfEntries);
#endif

SVAR(LANHostConfigManagement);
SVAR(LANEthernetInterfaceConfig);
SVAR(LANUSBInterfaceConfig);
SVAR(Hosts);

/* */
/* InternetGatewayDevice.LANDevice. */

SVAR(LANEthernetInterfaceNumberOfEntries);
SVAR(LANUSBInterfaceNumberOfEntries);

#endif   /* LAN_PARAMS_H*/
