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
//  Filename:       ippingparams.c
//
******************************************************************************
//  Description:
//             IPPing profile supported objects
//
*****************************************************************************/

/*
* Declare instances of CPE strings
*/
#include "sharedparams.h"
#include "ippingparams.h" /* profiles for parameter callbacks */

/** */
/** InternetGatewayDevice.IPPingDiagnostics. */
TRXGFUNC(getIPPingDiagnostics_DiagnosticsState);
TRXSFUNC(setIPPingDiagnostics_DiagnosticsState);
TRXGFUNC(getIPPingDiagnostics_Interface);
TRXSFUNC(setIPPingDiagnostics_Interface);
TRXGFUNC(getIPPingDiagnostics_Host);
TRXSFUNC(setIPPingDiagnostics_Host);
TRXGFUNC(getIPPingDiagnostics_NumberOfRepetitions);
TRXSFUNC(setIPPingDiagnostics_NumberOfRepetitions);
TRXGFUNC(getIPPingDiagnostics_Timeout);
TRXSFUNC(setIPPingDiagnostics_Timeout);
TRXGFUNC(getIPPingDiagnostics_DataBlockSize);
TRXSFUNC(setIPPingDiagnostics_DataBlockSize);
TRXGFUNC(getIPPingDiagnostics_DSCP);
TRXSFUNC(setIPPingDiagnostics_DSCP);
TRXGFUNC(getIPPingDiagnostics_SuccessCount);
TRXGFUNC(getIPPingDiagnostics_Failurecount);
TRXGFUNC(getIPPingDiagnostics_AverageResponseTime);
TRXGFUNC(getIPPingDiagnostics_MaximumResponseTime);
TRXGFUNC(getIPPingDiagnostics_MinimumResponseTime);
TRxObjNode IPPingDiagnosticsDesc[] = {
	{DiagnosticsState,{{tString,16,0}},
	 setIPPingDiagnostics_DiagnosticsState,
	 getIPPingDiagnostics_DiagnosticsState,
	 NULL,NULL},
	{Interface,{{tString,256,0}},
	 setIPPingDiagnostics_Interface,
	 getIPPingDiagnostics_Interface,
	 NULL,NULL},
	{Host,{{tString,256,0}},
	 setIPPingDiagnostics_Host,
	 getIPPingDiagnostics_Host,
	 NULL,NULL},
	{NumberOfRepetitions,{{tUnsigned,0,0}},
	 setIPPingDiagnostics_NumberOfRepetitions,
	 getIPPingDiagnostics_NumberOfRepetitions,
	 NULL,NULL},
	{Timeout,{{tUnsigned,0,0}},
	 setIPPingDiagnostics_Timeout,
	 getIPPingDiagnostics_Timeout,
	 NULL,NULL},
	{DataBlockSize,{{tUnsigned,0,0}},
	 setIPPingDiagnostics_DataBlockSize,
	 getIPPingDiagnostics_DataBlockSize,
	 NULL,NULL},
	{DSCP,{{tUnsigned,0,0}},
	 setIPPingDiagnostics_DSCP,
	 getIPPingDiagnostics_DSCP,
	 NULL,NULL},
	{SuccessCount,{{tUnsigned,0,0}},
	 NULL,
	 getIPPingDiagnostics_SuccessCount,
	 NULL,NULL},
	{FailureCount,{{tUnsigned,0,0}},
	 NULL,
	 getIPPingDiagnostics_Failurecount,
	 NULL,NULL},
	{AverageResponseTime,{{tUnsigned,0,0}},
	 NULL,
	 getIPPingDiagnostics_AverageResponseTime,
	 NULL,NULL},
	{MaximumResponseTime,{{tUnsigned,0,0}},
	 NULL,
	 getIPPingDiagnostics_MaximumResponseTime,
	 NULL,NULL},
	{MinimumResponseTime,{{tUnsigned,0,0}},
	 NULL,
	 getIPPingDiagnostics_MinimumResponseTime,
	 NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
