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
//  Filename:       standardparams.c
//
******************************************************************************
//  Description:
//             Generate string names for all standard objects
//
*****************************************************************************/

#define CPEVARNAMEINSTANCE
#include "sharedparams.h"
#include "rootparams.h"
#include "baselineparams.h"
#include "lanparams.h"
#include "wanparams.h"
#include "routeparams.h"
#include "ippingparams.h"
#include "timeparams.h"
#include "userinterfaceparams.h"
#ifdef WIRELESS
#include "wirelessparams.h" 
#endif /* #ifdef WIRELESS*/
#ifdef WT104_SUPPORT
#include "voiceserviceparams.h"
#endif

const char instanceIDMASK[]="##";
