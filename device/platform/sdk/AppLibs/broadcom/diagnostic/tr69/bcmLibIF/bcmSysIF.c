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
 * File Name  : bcmSysIF.c
 *
 * Description: System object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.2 $
 * $Id: bcmSysIF.c,v 1.2 2005/12/28 20:15:24 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include "../main/utils.h"
#include "../SOAPParser/CPEframework.h"
#include "bcmSysIF.h"

#include "../inc/appdefs.h"
#include "bcm_types.h"
#include "syscall.h"
#if 0
//#include "bcmtypes.h"
//#include "bcmcfm.h"
//#include "bcmcfmntwk.h"
//#include "dbapi.h"
//#include "bcmcfmdiag.h"
#endif

TRX_STATUS tr69c_setDNSServers(char *value)
{
#if 0
	char *p1, *p2;
	char dns[IFC_LARGE_LEN];
	struct in_addr dns1, dns2;
	void *info;
	uint32 index=0;

	if ( value == NULL ) return TRX_ERR;
	strcpy(dns, value);
	
	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DNS, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDnsCfg_t pDns = (PBcmCfm_NtwkDnsCfg_t)info;

		if ( (p1 = strchr(dns, ',')) != NULL )
			*p1 = '\0';
		inet_aton(dns, &dns1);
		pDns->preferredDns = dns1.s_addr;
		if ( p1 != NULL ) {
			if ( (p2 = strchr(p1 + 1, ',')) != NULL )
				*p2 = '\0';
			inet_aton(p1 + 1, &dns2);
			pDns->alternateDns = dns2.s_addr;
		}
		if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_DNS, info, index) == BcmCfm_Ok )
			return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS tr69c_getDNSServers(char **value)
{
#if 0
	char dns1[IFC_TINY_LEN], dns2[IFC_TINY_LEN];
	char dns[IFC_LARGE_LEN];
	struct in_addr inaddr;
	void *info;
	uint32 index=0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DNS, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDnsCfg_t pDns = (PBcmCfm_NtwkDnsCfg_t)info;
		if ( pDns != NULL ) {
			inaddr.s_addr = pDns->preferredDns;
			strcpy(dns1, inet_ntoa(inaddr));
			inaddr.s_addr = pDns->alternateDns;
			strcpy(dns2, inet_ntoa(inaddr));
			sprintf(dns, "%s,%s", dns1, dns2);
			*value = strdup(dns);   
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_DNS, info);
		} else
			*value = strdup("");   

		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS tr69c_setDefaultGateway(char *value)
{
#if 0
	void *obj;
	PBcmCfm_NtwkDefaultGatewayCfg_t pObj;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DEFAULT_GATEWAY, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkDefaultGatewayCfg_t)obj;
		if ( pObj != NULL ){
			pObj->defaultGateway = readIp(value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_DEFAULT_GATEWAY, obj, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS tr69c_getDefaultGateway(char **value)
{
#if 0
	void *obj;
	PBcmCfm_NtwkDefaultGatewayCfg_t pObj;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DEFAULT_GATEWAY, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkDefaultGatewayCfg_t)obj;
		if ( pObj != NULL ){
			*value = writeIp(pObj->defaultGateway);
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_DEFAULT_GATEWAY, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}



/** */
/** InternetGatewayDevice.Time. */
/*#ifdef SUPPORT_SNTP*/
#if 1
TRX_STATUS getTime_NTPServer(char **value, int server)
{
#if 0
	void *info;
	uint32 index = 1;
	char *curNtpServer = NULL;

	if (BcmCfmImpl_getNtpInfo(&info, &index) == BcmCfm_Ok) {
		PBcmCfm_NtpCfg_t pNtpObj = (PBcmCfm_NtpCfg_t)info;
		switch (server) {
			case 1:
				curNtpServer = pNtpObj->ntpServer1;
				break;
			case 2:
				curNtpServer = pNtpObj->ntpServer2;
				break;
			default:
				break;
		}				
		*value = strdup (curNtpServer);
		BcmCfmImpl_freeNtpInfo(info);
	}
	else
		return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	BSTD_UNUSED(server);
	return TRX_OK;
}

TRX_STATUS getTime_NTPServer1(char **value)
{
#if 0
	return (getTime_NTPServer(value, 1));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS getTime_NTPServer2(char **value)
{
#if 0
	return (getTime_NTPServer(value, 2));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS setTime_NTPServer(const char *value, int server)
{
#if 0
	void *info;
	uint32 index = 1;
	char **curNtpServer = NULL;
	int len = strlen(value);
	
	if (len <=  0)
		return TRX_ERR;
					
	if (BcmCfmImpl_getNtpInfo(&info, &index) == BcmCfm_Ok) {
		PBcmCfm_NtpCfg_t pNtpObj = (PBcmCfm_NtpCfg_t)info;
		switch (server) {
			case 1:
				  curNtpServer = &(pNtpObj->ntpServer1);
				break;
			case 2:
				  curNtpServer = &(pNtpObj->ntpServer2);
				break;
			default:
				break;
		}				
		BcmCfm_free(*curNtpServer);
	      if (BcmCfm_alloc(len + 1, (void **)curNtpServer) == BcmCfm_Ok) {
			strncpy(*curNtpServer, value, len);
		    	if (BcmCfmImpl_setNtpInfo(info, index) != BcmCfm_Ok)
				return TRX_ERR;
	      	}
	}
#endif	
	BSTD_UNUSED(value);
	BSTD_UNUSED(server);
	return TRX_OK;
}

TRX_STATUS setTime_NTPServer1(const char *value)
{
#if 0
	return (setTime_NTPServer(value, 1));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS setTime_NTPServer2(const char *value)
{
#if 0
	return (setTime_NTPServer(value, 2));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS  getTime_CurrentLocalTime(char **value)
{
#if 0
	time_t  curtime = time(NULL);
	*value = strdup (getXSIdateTime(&curtime));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS getTime_LocalTimeZone(char **value)
{
#if 0
	void *info;
	uint32 index = 1;
	char buf	[IFC_LARGE_LEN];

	if (BcmCfmImpl_getNtpInfo(&info, &index) == BcmCfm_Ok) {
		PBcmCfm_NtpCfg_t pNtpObj = (PBcmCfm_NtpCfg_t)info;
		strncpy(buf, BcmCfmImpl_indexToTzone(pNtpObj->timeZone), IFC_LARGE_LEN-1);
		buf[ZONE_GM_OFFSET+LOCAL_ZONE_SIZE] = '\0';
		*value = strdup (buf+ZONE_GM_OFFSET);
		BcmCfmImpl_freeNtpInfo(info);
	}
	else
		return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}	

TRX_STATUS setTime_LocalTimeZone(const char *value)
{
#if 0
	void *info;
	uint32 index = 1;
	BcmCfm_TimeZone newZone = (BcmCfm_TimeZone) -1;
	
	if ((newZone = BcmCfmImpl_tzoneToIndex((char *)value, ZONE_GM_OFFSET)) ==  (BcmCfm_TimeZone)-1)
		return TRX_ERR;
	
	if (BcmCfmImpl_getNtpInfo(&info, &index) == BcmCfm_Ok) {
		PBcmCfm_NtpCfg_t pNtpObj = (PBcmCfm_NtpCfg_t)info;
		pNtpObj->timeZone = newZone;
		if (BcmCfmImpl_setNtpInfo(info, index) != BcmCfm_Ok)
			return TRX_ERR;
	}
#endif	
	BSTD_UNUSED(value);
	return TRX_OK;
}	

TRX_STATUS getTime_LocalTimeZoneName(char **value)
{
#if 0
	void *info;
	uint32 index = 1;
	char buf	[IFC_LARGE_LEN];

	if (BcmCfmImpl_getNtpInfo(&info, &index) == BcmCfm_Ok) {
		PBcmCfm_NtpCfg_t pNtpObj = (PBcmCfm_NtpCfg_t)info;
		strncpy(buf, BcmCfmImpl_indexToTzone(pNtpObj->timeZone), IFC_LARGE_LEN-1);
		*value = strdup (buf);
		BcmCfmImpl_freeNtpInfo(info);
	}
	else
		return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS setTime_LocalTimeZoneName(char *value)
{
#if 0
	void *info;
	uint32 index = 1;
	BcmCfm_TimeZone newZone = (BcmCfm_TimeZone) -1;

	if ((newZone = BcmCfmImpl_tzoneToIndex(value, ZONE_NAME_OFFSET)) ==  (BcmCfm_TimeZone)-1)
		return TRX_ERR;
	if (BcmCfmImpl_getNtpInfo(&info, &index) == BcmCfm_Ok) {
		PBcmCfm_NtpCfg_t pNtpObj = (PBcmCfm_NtpCfg_t)info;
		pNtpObj->timeZone = newZone;
		if (BcmCfmImpl_setNtpInfo(info, index) != BcmCfm_Ok)
			return TRX_ERR;
	}
#endif	
	BSTD_UNUSED(value);
	return TRX_OK;
}

#if 0 /* Not implementated. */
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
#endif

#endif  /* SUPPORT_SNTP*/


/** */
/** InternetGatewayDevice.IPPingDiagnostics. */
/*TRX_STATUS getDiagPingData(PBcmCfm_DiagPing pData)*/
TRX_STATUS getDiagPingData(void * pData)
{
#if 0
  BcmDiag_getPingDiagData(pData);
#endif
	BSTD_UNUSED(pData);
  return TRX_OK;
}

/*void resetDiagPingData(PBcmCfm_DiagPing pData)*/
void resetDiagPingData(void *pData)
{
#if 0
  if (strcmp(pData->state,"Complete") == 0) {
    strcpy(pData->state,"None");
    pData->repetition = 0;
    memset(pData->interface,0,sizeof(pData->interface));
    memset(pData->host,0,sizeof(pData->host));
    pData->size = 0;
  }
#endif
	BSTD_UNUSED(pData);
}

TRX_STATUS getIPPingDiagnostics_DiagnosticsState(char **value)
{
#if 0
	BcmCfm_DiagPing data;
	if (getDiagPingData(&data) == TRX_OK) {
	  *value = strdup(data.state);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;

}
TRX_STATUS setIPPingDiagnostics_DiagnosticsState(char *value)
{
#if 0 
  BcmCfm_DiagPing data;
  if (strcmp(value,"Requested") != 0)
    return TRX_ERR;
  if (getDiagPingData(&data) == TRX_OK) {
    resetDiagPingData(&data);
    strcpy(data.state,value);
    BcmDiag_setPingDiagData(&data);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Interface(char **value)
{
#if 0
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    *value = strdup(data.interface);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_Interface(char *value)
{
#if 0
  BcmCfm_DiagPing data;
  char paramName[266];
  char *uName = NULL;
  uint32 intfIndex = 0;

  /* check to make sure the interface is valid */
  snprintf(paramName, sizeof(paramName), "%s.Name", value);
  if ( getGWParameterValue( paramName, &uName) != TRX_OK) {
    return TRX_ERR;
  }
  if ( BcmCfmNtwk_userNameToIndex(uName, &intfIndex) == BcmCfm_Ok ) {
    if (getDiagPingData(&data) == TRX_OK) {
      resetDiagPingData(&data);
      strcpy(data.interface,value);
      BcmDiag_setPingDiagData(&data);
      if (uName != NULL)
	free(uName);
      return TRX_OK;
    }
  }
  if (uName != NULL)
    free(uName);
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}

TRX_STATUS getIPPingDiagnostics_Host(char **value)
{
#if 0
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    *value = strdup(data.host);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_Host(char *value)
{
#if 0
  BcmCfm_DiagPing data;

  if (strlen(value) <= 0)
    return TRX_ERR;

  if (getDiagPingData(&data) == TRX_OK) {
    resetDiagPingData(&data);
    strcpy(data.host,value);
    BcmDiag_setPingDiagData(&data);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_NumberOfRepetitions(char **value)
{
#if 0
  char repetition[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(repetition,"%lu",data.repetition);
    *value = strdup(repetition);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_NumberOfRepetitions(char *value)
{
#if 0
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    resetDiagPingData(&data);
    data.repetition = strtoul(value, (char **)NULL, 0);
    if (data.repetition < 1)
      return TRX_ERR;
    BcmDiag_setPingDiagData(&data);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Timeout(char **value)
{
#if 0
  char timeout[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(timeout,"%lu",data.timeout);
    *value = strdup(timeout);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_Timeout(char *value)
{
#if 0
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    resetDiagPingData(&data);
    data.timeout = strtoul(value, (char **)NULL, 0);
    if (data.timeout < 1)
      return TRX_ERR;
    BcmDiag_setPingDiagData(&data);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_DataBlockSize(char **value)
{
#if 0
  char size[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(size,"%lu",data.size);
    *value = strdup(size);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_DataBlockSize(char *value)
{
#if 0
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    resetDiagPingData(&data);
    data.size = strtoul(value, (char **)NULL, 0);
    if (data.size < 1)
      return TRX_ERR;
    BcmDiag_setPingDiagData(&data);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_DSCP(char **value)
{
#if 0
  char dscp[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(dscp,"%lu",data.dscp);
    *value = strdup(dscp);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS setIPPingDiagnostics_DSCP(char *value)
{
#if 0
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    resetDiagPingData(&data);
    data.dscp = strtoul(value, (char **)NULL, 0);
    if (data.dscp < 0)
      return TRX_ERR;
    BcmDiag_setPingDiagData(&data);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_SuccessCount(char **value)
{
#if 0
  char count[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(count,"%lu",data.successCount);
    *value = strdup(count);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_Failurecount(char **value)
{
#if 0
  char count[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(count,"%lu",data.failCount);
    *value = strdup(count);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
TRX_STATUS getIPPingDiagnostics_AverageResponseTime(char **value)
{
#if 0
  char count[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(count,"%lu",data.avgResponseTime);
    *value = strdup(count);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}

TRX_STATUS getIPPingDiagnostics_MinimumResponseTime(char **value)
{
#if 0
  char count[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(count,"%lu",data.minResponseTime);
    *value = strdup(count);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}

TRX_STATUS getIPPingDiagnostics_MaximumResponseTime(char **value)
{
#if 0
  char count[IFC_TINY_LEN];
  BcmCfm_DiagPing data;

  if (getDiagPingData(&data) == TRX_OK) {
    sprintf(count,"%lu",data.maxResponseTime);
    *value = strdup(count);
    return TRX_OK;
  }
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;
}
