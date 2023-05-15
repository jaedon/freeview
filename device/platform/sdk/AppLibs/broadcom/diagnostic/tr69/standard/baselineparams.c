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
//  Filename:       baselineparams.c
//
******************************************************************************
//  Description:
//             Baseline profile supported objects
//
*****************************************************************************/

#include "sharedparams.h"
#include "baselineparams.h"

/* InternetGatewayDevice.DeviceInfo. */
TRXGFUNC(getManufacturer);
TRXGFUNC(getManufacturerOUI);
TRXGFUNC(getModelName);
TRXGFUNC(getProductClass);
TRXGFUNC(getSerialNumber);
TRXGFUNC(getSoftwareVersion);
TRXGFUNC(getHardwareVersion);
TRXGFUNC(getSpecVersion);
TRXGFUNC(getProvisioningCode);
TRXSFUNC(setProvisioningCode);
TRXGFUNC(getUpTime);
TRXGFUNC(getDeviceLog);

TRxObjNode  deviceInfoDesc[] = {
    {Manufacturer,{{tString,64,0}}, NULL,getManufacturer,NULL,NULL},
    {ManufacturerOUI,{{tString,6,0}}, NULL,getManufacturerOUI,NULL,NULL},
    {ModelName,{{tString,64,0}}, NULL,getModelName,NULL,NULL},
    {Description,{{tString,256,0}}, NULL,NULL,NULL,NULL},
    {ProductClass,{{tString,256,0}},NULL,getProductClass,NULL,NULL},
    {SerialNumber,{{tString,256,0}}, NULL,getSerialNumber,NULL,NULL},
    {SoftwareVersion,{{tString,64,0}}, NULL,getSoftwareVersion,NULL,NULL},
    {HardwareVersion,{{tString,64,0}}, NULL,getHardwareVersion,NULL,NULL},
    {SpecVersion,{{tString,16,0}}, NULL,getSpecVersion,NULL,NULL},
    {ProvisioningCode,{{tString,64,0}}, setProvisioningCode,getProvisioningCode,NULL,NULL},
    {UpTime,{{tUnsigned,0,1}}, NULL,getUpTime,NULL,NULL},
    {DeviceLog,{{tString,32767,1}}, NULL,getDeviceLog,NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/* InternetGatewayDevice.MangementServer. */
/* A copy of most of this data is stored in the acsState structure */
TRXGFUNC(getMSrvrURL);
TRXGFUNC(getMSrvrInformEnable);
TRXGFUNC(getMSrvrInformInterval);
TRXGFUNC(getMSrvrInformTime);
TRXGFUNC(getMSrvrParameterKey);
TRXGFUNC(getConnectionReqURL);
TRXGFUNC(getConnectionUsername);
TRXGFUNC(getConnectionPassword);
TRXGFUNC(getKickURL);
TRXGFUNC(getUpgradesManaged);
TRXSFUNC(setMSrvrURL);
TRXSFUNC(setMSrvrUsername);
TRXGFUNC(getMSrvrUsername);
TRXSFUNC(setMSrvrPassword);
TRXGFUNC(getMSrvrPassword);
TRXSFUNC(setMSrvrInformEnable);
TRXSFUNC(setMSrvrInformInterval);
TRXSFUNC(setMSrvrInformTime);
TRXSFUNC(setConnectionUsername);
TRXSFUNC(setConnectionPassword);
TRXSFUNC(setKickURL);
TRXSFUNC(setUpgradesManaged);
TRxObjNode  mgmtServerDesc[] = {
    {URL,{{tString,256, 1}}, setMSrvrURL,getMSrvrURL, NULL,NULL},
    {Username,{{tString,256, 1}}, setMSrvrUsername,getMSrvrUsername, NULL,NULL},
    {Password,{{tStringSOnly,256, 1}}, setMSrvrPassword,getMSrvrPassword, NULL,NULL},
    {PeriodicInformEnable,{{tBool, 0, 1}}, setMSrvrInformEnable,getMSrvrInformEnable, NULL,NULL},
    {PeriodicInformInterval,{{tUnsigned, 0, 1}}, setMSrvrInformInterval,getMSrvrInformInterval,NULL,NULL},
    {PeriodicInformTime,{{tDateTime, 0, 1}},setMSrvrInformTime,getMSrvrInformTime,NULL,NULL},
    {ParameterKey,{{tString,32,1}},NULL,getMSrvrParameterKey, NULL,NULL},
    {ConnectionRequestURL,{{tString,256, 1}},NULL,getConnectionReqURL, NULL,NULL},
    {ConnectionRequestUsername,{{tString,256, 1}}, setConnectionUsername, getConnectionUsername, NULL,NULL},
    {ConnectionRequestPassword,{{tStringSOnly,256, 1}}, setConnectionPassword, getConnectionPassword, NULL,NULL},
    {KickURL,{{tString,256, 1}}, setKickURL, getKickURL, NULL,NULL},
    {UpgradesManaged,{{tBool, 0, 1}}, setUpgradesManaged, getUpgradesManaged, NULL,NULL},
    /*{DownloadProgressURL,{{tString,256}}, NULL,NULL, NULL,NULL},*/
    {NULL, {{tUnsigned,0,1}}, NULL, NULL, NULL, NULL}
};
