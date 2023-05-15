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
//  Filename:       timeparams.h
//
******************************************************************************
//  Description:
//             Time profile supported object varable names
//
//
*****************************************************************************/

#ifndef TIME_PARAMS_H
#define TIME_PARAMS_H

#include "../inc/tr69cdefs.h"

/* InternetGatewayDevice.Time. */
SVAR(NTPServer1);
SVAR(NTPServer2);
#if 0
//SVAR(NTPServer3);
//SVAR(NTPServer4);
//SVAR(NTPServer5);
#endif
SVAR(CurrentLocalTime);
SVAR(LocalTimeZone);
SVAR(LocalTimeZoneName);
SVAR(DaylightSavingsUsed);
SVAR(DaylightSavingsStart);
SVAR(DaylightSavingsEnd);

#endif   /* TIME_PARAMS_H */
