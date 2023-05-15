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
//  Filename:       routeparams.c
//
******************************************************************************
//  Description:
//             Layer3Forwarding profile supported objects
//
*****************************************************************************/

#include "sharedparams.h"
#include "routeparams.h" 

/* InternetGatewayDevice.Layer3Forwarding.Forwarding.{i}. */

TRXGFUNC(getL3ForwardEnable);
TRXGFUNC(getL3ForwardStatus);
TRXGFUNC(getL3ForwardType);
TRXGFUNC(getL3ForwardDIP);
TRXGFUNC(getL3ForwardDMask);
TRXGFUNC(getL3ForwardSIP);
TRXGFUNC(getL3ForwardSMask);
TRXGFUNC(getL3ForwardGWIP);
TRXGFUNC(getL3ForwardIF);
TRXGFUNC(getL3ForwardMetric);
TRXSFUNC(setL3ForwardEnable);
TRXSFUNC(setL3ForwardType);
TRXSFUNC(setL3ForwardDIP);
TRXSFUNC(setL3ForwardDMask);
TRXSFUNC(setL3ForwardSIP);
TRXSFUNC(setL3ForwardSMask);
TRXSFUNC(setL3ForwardGWIP);
TRXSFUNC(setL3ForwardIF);
TRXSFUNC(setL3ForwardMetric);
TRxObjNode  forwardingInstanceDesc[] = {
    {Enable,{{tBool,256,0}},setL3ForwardEnable,getL3ForwardEnable,NULL,NULL},
    /*{Status,{{tString}},NULL,getL3ForwardStatus,NULL,NULL},*/
    /*{Type,{{tString}},setL3ForwardType,getL3ForwardType,NULL,NULL},*/
    {DestIPAddress,{{tString,16,0}},setL3ForwardDIP,getL3ForwardDIP,NULL,NULL},
    {DestSubnetMask,{{tString,16,0}},setL3ForwardDMask,getL3ForwardDMask,NULL,NULL},
    /*{SourceIPAddress,{{tString}},setL3ForwardSIP,getL3ForwardSIP,NULL,NULL},*/
    /*{SourceSubnetMask,{{tString}},setL3ForwardSMask,getL3ForwardSMask,NULL,NULL},*/
    {GatewayIPAddress,{{tString,16,0}},setL3ForwardGWIP,getL3ForwardGWIP,NULL,NULL},
    {Interface,{{tString,16,0}},setL3ForwardIF,getL3ForwardIF,NULL,NULL},
    /*{ForwardingMetric,{{tInt}},setL3ForwardMetric,getL3ForwardMetric,NULL,NULL},*/
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
/* InternetGatewayDevice.Layer3Forwarding.Forwarding */
TRXGFUNC(addForwardI);
TRXSFUNC(delForwardI);
TRxObjNode  forwardingDesc[] = {
    {instanceIDMASK,{{0,0,0}},delForwardI,addForwardI,forwardingInstanceDesc,NULL}
};

TRXGFUNC(getL3ForwardingNoI);
/* InternetGatewayDevice.Layer3Forwarding. */
TRxObjNode  layer3ForwardingDesc[] = {
    {DefaultConnectionService,{{tString,256,0}},NULL,NULL,NULL,NULL},
    {ForwardNumberOfEntries,{{tUnsigned,0,0}},NULL,getL3ForwardingNoI,NULL,NULL},
    {Forwarding,{{tInstance,0,0}},NULL,NULL,forwardingDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
