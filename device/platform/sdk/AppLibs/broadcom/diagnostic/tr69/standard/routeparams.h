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
//  Filename:       routeparams.h
//
******************************************************************************
//  Description:
//             Layer3Forwarding profile supported object varable names
//
//
*****************************************************************************/

#ifndef ROUTE_PARAMS_H
#define ROUTE_PARAMS_H

#include "../inc/tr69cdefs.h"

/* InternetGatewayDevice.Layer3Forwarding.Forwarding.[i]. */
SVAR(DefaultConnectionService);
SVAR(ForwardNumberOfEntries);
#if 0
//SVAR(Status);
//SVAR(Type);
#endif
SVAR(DestIPAddress);
SVAR(DestSubnetMask);
SVAR(SourceIPAddress);
SVAR(SourceSubnetMask);
SVAR(GatewayIPAddress);
SVAR(ForwardingMetric);
SVAR(MTU);
/* InternetGatewayDevice.Layer3Forwarding.Forwarding. */
SVAR(Forwarding);

#endif   /* ROUTE_PARAMS_H*/
