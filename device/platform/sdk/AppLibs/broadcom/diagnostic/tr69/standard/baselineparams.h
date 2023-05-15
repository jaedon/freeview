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
//  Filename:       baselineparams.h
//
******************************************************************************
//  Description:
//             Baseline profile supported object varable names
//
//
*****************************************************************************/

#ifndef BASELINE_PARAMS_H
#define BASELINE_PARAMS_H

#include "../inc/tr69cdefs.h"

/* InternetGatewayDevice.DeviceInfo. */

SVAR(Manufacturer);
SVAR(ManufacturerOUI);
SVAR(ModelName);
SVAR(Description);
SVAR(ProductClass);
SVAR(SerialNumber);
SVAR(SoftwareVersion);
SVAR(HardwareVersion);
SVAR(SpecVersion);
SVAR(ProvisioningCode);
SVAR(UpTime);
SVAR(DeviceLog);

/* InternetGatewayDevice.ManagementServer. */
SVAR(URL);
/*SVAR(Username); */
/*SVAR(Password); */
SVAR(PeriodicInformEnable );
SVAR(PeriodicInformInterval );
SVAR(PeriodicInformTime );
SVAR(ParameterKey );
SVAR(ConnectionRequestURL );
SVAR(ConnectionRequestUsername );
SVAR(ConnectionRequestPassword );
SVAR(KickURL );
SVAR(UpgradesManaged);
/*SVAR( DownloadProgressURL);*/

#endif   /* BASELINE_PARAMS_H */
