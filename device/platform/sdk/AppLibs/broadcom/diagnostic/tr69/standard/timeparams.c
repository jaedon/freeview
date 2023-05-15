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
//  Filename:       timeparams.c
//
******************************************************************************
//  Description:
//             Time profile supported objects
//
*****************************************************************************/

#include "sharedparams.h"
#include "timeparams.h" 

/** */
/** InternetGatewayDevice.Time. */

TRXGFUNC(getTime_NTPServer1);
TRXSFUNC(setTime_NTPServer1);
TRXGFUNC(getTime_NTPServer2);
TRXSFUNC(setTime_NTPServer2);
#if 0
TRXGFUNC(getTime_NTPServer3);
TRXSFUNC(setTime_NTPServer3);
TRXGFUNC(getTime_NTPServer4);
TRXSFUNC(setTime_NTPServer4);
TRXGFUNC(getTime_NTPServer5);
TRXSFUNC(setTime_NTPServer5);
#endif
TRXGFUNC(getTime_CurrentLocalTime);
TRXGFUNC(getTime_LocalTimeZone);
TRXSFUNC(setTime_LocalTimeZone);
TRXGFUNC(getTime_LocalTimeZoneName);
TRXSFUNC(setTime_LocalTimeZoneName);
#if 0
TRXGFUNC(getTime_DaylightSavingsUsed);
TRXSFUNC(setTime_DaylightSavingsUsed);
TRXGFUNC(getTime_DaylightSavingsStart);
TRXSFUNC(setTime_DaylightSavingsStart);
TRXGFUNC(getTime_DaylightSavingsEnd);
TRXSFUNC(setTime_DaylightSavingsEnd);
#endif
TRxObjNode TimeDesc[] = {
	{NTPServer1,{{tString,64,0}},
	  setTime_NTPServer1,
	  getTime_NTPServer1,
	  NULL,NULL},
	{NTPServer2,{{tString,64,0}},
	  setTime_NTPServer2,
	  getTime_NTPServer2,
	  NULL,NULL},
	#if 0 /* not supported at this time*/
	{NTPServer3,{{tString,64}},
	  setTime_NTPServer3,
	  getTime_NTPServer3,
	  NULL,NULL},
	{NTPServer4,{{tString,64}},
	  setTime_NTPServer4,
	  getTime_NTPServer4,
	  NULL,NULL},
	{NTPServer5,{{tString,64}},
	  setTime_NTPServer5,
	  getTime_NTPServer5,
	  NULL,NULL},
	#endif 
	{CurrentLocalTime,{{tDateTime,0,1}},
	  NULL,
	  getTime_CurrentLocalTime,
	  NULL,NULL},
	{LocalTimeZone,{{tString,6,0}},
	  setTime_LocalTimeZone,
	  getTime_LocalTimeZone,
	  NULL,NULL},
	{LocalTimeZoneName,{{tString,64,0}},
	  setTime_LocalTimeZoneName,
	  getTime_LocalTimeZoneName,
	  NULL,NULL},
	#if 0	  /* Not implemented.*/
	{DaylightSavingsUsed,{{tBool,0}},
	  setTime_DaylightSavingsUsed,
	  getTime_DaylightSavingsUsed,
	  NULL,NULL},
	{DaylightSavingsStart,{{tDateTime,0}},
	  setTime_DaylightSavingsStart,
	  getTime_DaylightSavingsStart,
	  NULL,NULL},
	{DaylightSavingsEnd,{{tDateTime,0}},
	  setTime_DaylightSavingsEnd,
	  getTime_DaylightSavingsEnd,
	  NULL,NULL},
	#endif
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
