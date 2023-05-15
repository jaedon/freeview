/*****************************************************************************
//
//  Copyright (c) 2004  Broadcom Corporation
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
//  Filename:       sharedparams.h
//
******************************************************************************
//  Description:
//             Shared object varable names
//
//
*****************************************************************************/

#ifndef SHARED_PARAMS_H
#define SHARED_PARAMS_H

#include "../inc/tr69cdefs.h"

/* they are used by lan and wan profiles*/
SVAR(Enable);
SVAR(Status);
SVAR(SubnetMask);
SVAR(DNSServers);
SVAR(Stats);
SVAR(TotalBytesSent);
SVAR(TotalBytesReceived);
SVAR(TotalPacketsSent);
SVAR(TotalPacketsReceived);

/* It is used by lan and wireless profiles */
SVAR(MACAddressControlEnabled);

/* It is used by lan and ipping profiles */
SVAR(Host);

/* they are used by baseline and wan profiles*/
SVAR(Username);
SVAR(Password);
#if 0
// they are used by wan and ipping profiles
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANATMF5LoopbackDiagnostics. */
#endif
SVAR(DiagnosticsState);
SVAR(NumberOfRepetitions);
SVAR(Timeout);
SVAR(SuccessCount);
SVAR(FailureCount);
SVAR(AverageResponseTime);
SVAR(MinimumResponseTime);
SVAR(MaximumResponseTime);

/* It is used by ipping and route profiles*/
SVAR(Interface);

/* they are used by lan and voiceservice profiles */
/* InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig.Stats. */
SVAR(BytesSent);
SVAR(BytesReceived);
SVAR(PacketsSent);
SVAR(PacketsReceived);

/* It is used by wan and voiceservice profiles*/
SVAR(Name);

#endif   /* SHARED_PARAMS_H */
