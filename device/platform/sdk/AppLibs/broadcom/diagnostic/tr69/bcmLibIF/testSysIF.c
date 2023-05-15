 /*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : testSysIF.c
 *
 * Description: System object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.3 $
 * $Id: testSysIF.c,v 1.3 2006/01/18 18:38:10 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "../main/utils.h"
#include "../SOAPParser/CPEframework.h"

TRX_STATUS tr69c_setDNSServers(char *value)
{
	return TRX_OK;
}

TRX_STATUS tr69c_getDNSServers(char **value)
{
	 *value = strdup("");
	return TRX_OK;
}

TRX_STATUS tr69c_setDefaultGateway(char *value)
{
	return TRX_OK;
}

TRX_STATUS tr69c_getDefaultGateway(char **value)
{
	 *value = strdup("");
	return TRX_OK;
}


/** */
/** InternetGatewayDevice.Time. */

TRX_STATUS setTime_NTPServer1(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_NTPServer1(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_NTPServer2(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_NTPServer2(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_NTPServer3(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_NTPServer3(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_NTPServer4(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_NTPServer4(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_NTPServer5(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_NTPServer5(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_CurrentLocalTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_LocalTimeZone(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_LocalTimeZone(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_LocalTimeZoneName(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_LocalTimeZoneName(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_DaylightSavingsUsed(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_DaylightSavingsUsed(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_DaylightSavingsStart(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_DaylightSavingsStart(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setTime_DaylightSavingsEnd(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getTime_DaylightSavingsEnd(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}

/** */
/** InternetGatewayDevice.IPPingDiagnostics. */

TRX_STATUS setIPPingDiagnostics_DiagnosticsState(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_DiagnosticsState(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_Interface(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Interface(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_Host(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Host(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_NumberOfRepetitions(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_NumberOfRepetitions(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_Timeout(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Timeout(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_DataBlockSize(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_DataBlockSize(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_DSCP(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_DSCP(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_SuccessCount(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_FailureCount(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_AverageResponseTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_MinimumResponseTmie(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_MaximumResponseTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Failurecount(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}


TRX_STATUS getIPPingDiagnostics_MinimumResponseTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}

