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
//  Filename:       rootparams.c
//
******************************************************************************
//  Description:
//             Root supported objects
//
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "rootparams.h"

TRXGFUNC(getWANLANDeviceNumEntries);
TRXGFUNC(getDeviceSummary);

extern TRxObjNode  deviceInfoDesc[];
extern TRxObjNode  mgmtServerDesc[];
extern TRxObjNode  UserInterfaceDesc[];
extern TRxObjNode  layer3ForwardingDesc[];
extern TRxObjNode  servicesDesc[];
extern TRxObjNode  LANDeviceInstanceDesc[];
extern TRxObjNode  WANDeviceInstanceDesc[];
extern TRxObjNode  TimeDesc[];
extern TRxObjNode  IPPingDiagnosticsDesc[];
#ifdef PROPRIETARY    
extern TRxObjNode  ProprietaryDesc[];
#endif    

/* InternetGatewayDevice. */
TRxObjNode  internetGatewayDeviceDesc[] = {
    {LANDeviceNumberOfEntries,{{tUnsigned,0,0}}, NULL,getWANLANDeviceNumEntries,NULL,NULL},
    {WANDeviceNumberOfEntries,{{tUnsigned,0,0}}, NULL,getWANLANDeviceNumEntries,NULL,NULL},
    {DeviceSummary,{{tString,1024,0}},NULL,getDeviceSummary,NULL,NULL},
    {DeviceInfo,{{tObject,0,0}}, NULL,NULL, deviceInfoDesc,NULL},
    {ManagementServer,{{tObject,0,0}}, NULL,NULL, mgmtServerDesc,NULL},
    {UserInterface,{{tObject,0,0}}, NULL,NULL, UserInterfaceDesc,NULL},
    {Layer3Forwarding,{{tObject,0,0}}, NULL,NULL, layer3ForwardingDesc,NULL},
/*   {Services,{{tObject,0,0}}, NULL,NULL, servicesDesc,NULL},*/
    {LANDevice,{{tInstance,0,0}}, NULL,NULL, LANDeviceInstanceDesc,NULL},
    {WANDevice,{{tInstance,0,0}}, NULL,NULL, WANDeviceInstanceDesc,NULL},      
    {Time,{{tObject,0,0}}, NULL,NULL, TimeDesc,NULL},
    {IPPingDiagnostics,{{tObject,0,0}}, NULL,NULL, IPPingDiagnosticsDesc,NULL},
#ifdef PROPRIETARY    
    {X_BROADCOM_COM,{{tObject,0,0}}, NULL,NULL, ProprietaryDesc,NULL},
#endif    
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/* . */
TRxObjNode  rootDevice[] = {
    {InternetGatewayDevice,{{tObject,0,0}}, NULL,NULL, internetGatewayDeviceDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

