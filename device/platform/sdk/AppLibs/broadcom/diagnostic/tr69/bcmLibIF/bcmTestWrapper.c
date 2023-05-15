
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
 * File Name  : bcmWrapper.c
 *
 * Description: Functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.27 $
 * $Id: bcmTestWrapper.c,v 1.27 2006/02/17 21:12:51 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
 
#include <syslog.h>
#include "../main/utils.h"
#include "../inc/appdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h" /* defines for ACS state */
#include "bcmTestWrapper.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "../inc/tr69cdefs.h" /* defines for ACS state */
extern ACSState acsState;

#define DOWNLOADTOFILE     /* set to write image file to /var directory */ 
static time_t  startTime;

/************************************************************************/
/* These routines are required by the framework. They return generic    */
/* information and status that are required by the framework to connect */
/* to the ACS or to listen for ACS connection requests                  */
/*                                                                      */
/************************************************************************/
 
 /* Returns state of WAN interface to be used by tr69 client */
eWanState getWanState(void)
{
    /* add BCM shared library call here to determine status of outbound I/F*/
    return eWAN_ACTIVE;
}

/* getACSContactedState returns eACS_NEVERCONTACTED if the ACS has never */
/* been contacted since initial customer configuration.  eACS_CONTACTED is */
/* returned if the ACS has been contacted before. This is used to determine*/
/* the boot event in the Inform */

eACSContactedState acsContactedState;

eACSContactedState getACSContactedState()
{
    /* Need to fetch from persistent memory  */
    return eACS_NEVERCONTACTED;
}
void saveACSContactedState()
{
	fprintf(stderr, "saveACSContactedState called\n");
}
void setACSContactedState(eACSContactedState state)
{
    acsContactedState = state;
}

/* getNewInstanceId return an instance id in the integer range that
* has not yet been used.This should be save in persistent memory 
* somewhere and initialized on startup.
* Need to think about integer wrap-around and asigning duplicates.
*  ?????????
*/
static int lastInstanceId;
int getNewInstanceId()
{
    return ++lastInstanceId;
}

/*
* Save the TR69 state values across the reboot
*/
void  saveTR69StatusItems( ACSState *a )
{
    fprintf(stderr, "Save TR69 State Variables\n");
    fprintf(stderr, "CommandKey(Reboot)=%s\n", a->rebootCommandKey);
    fprintf(stderr, "CommandKey(DL)=%s\n", a->downloadCommandKey);
    fprintf(stderr, "ParameterKey=%s\n", a->parameterKey);
    fprintf(stderr, "ACS Contacted State=%d\n", acsContactedState);
}

void wrapperReboot(eACSContactedState rebootContactValue)
{
	setACSContactedState( rebootContactValue );	 /* reset on BcmCfm_download error */
	saveTR69StatusItems(&acsState);
	#ifdef DEBUG
	fprintf(stderr, "CPE is REBOOTING with rebootContactValue =%d\n", rebootContactValue);
	#endif
}
/*
* Call library factory reset 
*/
void wrapperFactoryReset()
{
    fprintf(stderr, "CPE is reset to factory settings\n");
}

void wrapperSaveConfigurations(void)
{
    //BcmCfm_saveConfig();
    fprintf(stderr, "CPE is SAVING its configurations\n");
}

/* preDownloadSetup(ACSState *acs, DownloadReq *)
* This function is called before the framework begins downloading of the image file.
* Any unnecessary applications or memory resources should be freed.
* Return 1: to continue download.
*        0: to Abort download.
*/
int  preDownloadSetup(ACSState *acs, DownloadReq *r)
{
    fprintf(stderr, "preDownloadSetup: URL=%s\n", r->url);
    fprintf(stderr, "User/pw: %s:%s\n", r->user, r->pwd);
    fprintf(stderr, "Required memory buffer size will be %d\n", r->fileSize);
    return 1;  
}
/* downloadComplete()
*  Called when image has been downloaded. If successful the *buf will point to the
*  image buffer. If *buf is NULL the download failed.
*  Control is returned to the framework with a 1 if the flash was successful.
*  If the flash image failed the return is a 0. Free the buffer prior
*  to return to the framework.
*/
int  downloadComplete(DownloadReq *r, char *buf)
{
    if (buf) {
        fprintf(stderr, "downloadComplete --save ACSState and  flash image here\n");
        fprintf(stderr, "Image size is %d\n", r->fileSize);

		#ifdef DOWNLOADTOFILE
		{
			size_t	wlth;
			FILE	*f = fopen("dload.image", "w");
			if (f) {
				int mlth = r->fileSize;
				wlth = fwrite(buf, 1, mlth, f);
				fclose(f);
				fprintf(stderr, "dload.image file written with %d bytes\n", wlth);
				updateDownLoadKey( r );
				/* does not return */
				acsState.dlFaultStatus = 0;	/* no download fault */
				acsState.dlFaultMsg = "Download successful";
				saveTR69StatusItems(&acsState);
			}
		}
		#endif
        free(buf);
        return 1;
    } else
        return 0;
}
/* 
* Lock cfm configuration data
* Return 0: Successful
* 	     -1: Error - data not locked
*/
int CfmDbLock(void)
{
	fprintf(stderr, "!!!!!!!!!! CfmDbLock >>>>>>>>>>>>>>>>>>\n");
	/*
    void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_MGMT_LOCK, &info, 0);
	BcmCfm_objSet(BCMCFM_OBJ_SYS_MGMT_LOCK, info, 0);
	*/
	return 0;
}
/* 
* Unlock cfm configuration data
*/
void CfmDbUnlock(void)
{
	fprintf(stderr, "!!!!!!!!!! CfmDbUnlock() <<<<<<<<<<<<<<\n");
	/*
    void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_MGMT_LOCK, &info, 0);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_MGMT_LOCK, info, 0);
	*/
}

/**********************************************************************
* TEST STUBS for get/setParameter****************
**********************************************************************/
/* InternetGatewayDevice.Layer3Forwarding.Forwarding.{i}. */
/* The following struct is for testing the object instance routines*/
typedef struct L3ForwardDesc {
    int     enable;
    /*char    *status; not readable -- realtime status*/
    char    *type;
    char    *destIP;
    char    *descMask;
    char    *srcIP;
    char    *srcMask;
    char    *gwIP;
    char    *interface;
    int     metric;
} L3ForwardDesc;

/* TEST STUBS -- maybe replaced for BCM963xx implementation */

TRX_STATUS getL3ForwardMetric(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->metric? strdup(itoa(l3fd->metric)) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardMetric(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            l3fd->metric = atoi(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardIF(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->interface? strdup(l3fd->interface) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardIF(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->interface);
            l3fd->interface = strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardGWIP(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->gwIP? strdup(l3fd->gwIP) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardGWIP(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->gwIP);
            l3fd->gwIP = strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardSMask(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->srcMask? strdup(l3fd->srcMask) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardSMask(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->srcMask);
            l3fd->srcMask = strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardSIP(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->srcIP? strdup(l3fd->srcIP) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardSIP(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->srcIP);
            l3fd->srcIP = strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardDMask(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->descMask? strdup(l3fd->descMask) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardDMask(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->descMask);
            l3fd->descMask= strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardDIP(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->destIP? strdup(l3fd->destIP): NULL;
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardDIP(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->destIP);
            l3fd->destIP= strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardType(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = l3fd->type? strdup(l3fd->type) : strdup("");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardType(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            free(l3fd->type);
            l3fd->type = strdup(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS getL3ForwardStatus(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = strdup("Error");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}

TRX_STATUS getL3ForwardEnable(char **value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            *value = strdup(l3fd->enable?"true":"false");
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
TRX_STATUS setL3ForwardEnable(char *value)
{
    InstanceDesc *idp;
    L3ForwardDesc *l3fd;
    if( idp=getCurrentInstanceDesc()){
        if (l3fd=(L3ForwardDesc *)idp->hwUserData) {
            l3fd->enable = testBoolean(value);
            return TRX_OK;
        }
    }
    return TRX_ERR;
}
/* The AddForwardI is adding a place holder for a forwarding rule.   */

TRX_STATUS addForwardI(char **value)
{
    InstanceDesc *idp;
    if ( idp= getNewInstanceDesc( getCurrentNode(),getCurrentInstanceDesc(),  0) ) {
        L3ForwardDesc *l3fd;
        *value = strdup(itoa(idp->instanceID));
        l3fd = (L3ForwardDesc *)malloc(sizeof(struct L3ForwardDesc));
        memset(l3fd, 0, sizeof(struct L3ForwardDesc));
        idp->hwUserData = (void *)l3fd; /* Link to bcmData */
        return TRX_OK; /* or TRX_BOOT if necessary */
    }
    return TRX_ERR;
}

TRX_STATUS delForwardI(char *value)
{
    TRxObjNode *n;
    InstanceDesc *idp;
    int     id = atoi(value);
    L3ForwardDesc *l3fd;

    if (idp=findInstanceDesc(n=getCurrentNode(),id)) {
        l3fd = (L3ForwardDesc *)idp->hwUserData; /* find bcmDAta */
        /* call bcmlib to remove fowarding instance data */
        if ( !deleteInstanceDesc(n, id))
            return TRX_OK; /* or TRX_BOOT if necessary */
        free(l3fd); /* free bcmdata desc. */
    }
    return TRX_ERR;
}

extern TRxObjNode  forwardingDesc[];
TRX_STATUS getL3ForwardingNoI(char **value)
{
    TRxObjNode *n;
    *value = strdup( itoa(getInstanceCount(forwardingDesc)));
    return TRX_OK;
}

TRX_STATUS getWANLANDeviceNumEntries(char **value)
{
    *value = strdup("1");
    return TRX_OK;
}


TRX_STATUS getManufacturer(char **value)
{
    *value = strdup("Broadcom");
    return TRX_OK;
}
static char *oui;
TRX_STATUS getManufacturerOUI(char **value)
{
	if (oui)
		*value = strdup(oui);
	else
		*value = strdup("010001");
    return TRX_OK;
}
TRX_STATUS setManufacturerOUI(char *value)
{
	oui = strdup(value);
	return TRX_OK;
}
static char *serialNum;
TRX_STATUS getSerialNumber(char **value)
{

	if (serialNum)
		*value = strdup(serialNum);
	else
		*value = strdup("123456789abc");
    return TRX_OK;
}

TRX_STATUS setSerialNumber(char *value)
{
	serialNum = strdup(value);
	return TRX_OK;
}

TRX_STATUS getSoftwareVersion(char **value)
{
    *value = strdup("3.0.3");
    return TRX_OK;
}

TRX_STATUS getProductClass (char **value)
{
    *value = strdup("BCRM-test-client");
    return TRX_OK;
}

TRX_STATUS getModelName(char **value)
{
    *value = strdup("BCM 96348ADSL");
    return TRX_OK;
}
TRX_STATUS getHardwareVersion(char **value)
{
    *value = strdup("3.0");
    return TRX_OK;
}
TRX_STATUS getSpecVersion(char **value)
{
    *value = strdup("1");
    return TRX_OK;
}
TRX_STATUS getProvisioningCode(char **value)
{
    if (acsState.provisioningCode)
        *value = strdup(acsState.provisioningCode);
	else 
        *value = NULL; /* is null on bootstrap */
    return TRX_OK;
}
TRX_STATUS setProvisioningCode(char *value)
{
    if (acsState.provisioningCode)
		free(acsState.provisioningCode);
	acsState.provisioningCode = strdup(value);
    return TRX_OK;
}
TRX_STATUS getUpTime(char **value)
{
    time_t  upt;
    
    upt = time(NULL)-startTime;
    *value = strdup(itoa(upt));
    return TRX_OK;
}

TRX_STATUS getDeviceLog(char **value)
{
    *value = strdup("log entries would be here ......<");
    return TRX_OK;
}
/* 
* .MangementServer.
*/
TRX_STATUS setMSrvrURL(const char *value)
{
    free(acsState.acsURL);
    acsState.acsURL = strdup(value);
    /* call bcmlib set function here */
    return TRX_OK;
}
TRX_STATUS getMSrvrURL(char **value)
{
    *value = acsState.acsURL? strdup(acsState.acsURL) : strdup("");
    return TRX_OK;
}

TRX_STATUS setMSrvrUsername(const char *value)
{
    free(acsState.acsUser);
    acsState.acsUser = strdup(value);
    /* call bcmlib set function here */
    return TRX_OK;
}

TRX_STATUS getMSrvrUsername(char **value)
{
    *value = acsState.acsUser? strdup(acsState.acsUser) : strdup("");
    return TRX_OK;
}

TRX_STATUS getMSrvrPassword(char **value)
{
    *value = acsState.acsPwd? strdup(acsState.acsPwd) : strdup("");
    return TRX_OK;
}
TRX_STATUS setMSrvrPassword(const char *value)
{
    free(acsState.acsPwd);
    acsState.acsPwd = strdup(value);
    /* call bcmlib set function here */
    return TRX_OK;
}

TRX_STATUS setMSrvrInformEnable(const char *value)
{
    acsState.informEnable = testBoolean(value);
    /* call bcmlib set function here */
    return TRX_OK;
}
TRX_STATUS getMSrvrInformEnable(char **value)
{
    *value = strdup(acsState.informEnable? "1": "0");
    return TRX_OK;
}
TRX_STATUS setMSrvrInformInterval(const char *value)
{
    acsState.informInterval = atoi(value);
	resetPeriodicInform(acsState.informInterval);
    return TRX_OK;
}
TRX_STATUS getMSrvrInformInterval(char **value)
{
    char    buf[10];
    snprintf(buf,sizeof(buf),"%d",acsState.informInterval);
    *value = strdup(buf);
    return TRX_OK;
}

TRX_STATUS getMSrvrInformTime(char **value)
{
    char    buf[30];
	if (acsState.informTime) {
		struct tm *bt=localtime(&acsState.informTime);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
		*value = strdup(buf);
	}
	else
		*value = strdup("0000-00-00T00:00:00");
    return TRX_OK;
}
TRX_STATUS setMSrvrInformTime(const char *value)
{                                      
    struct tm bt;
    strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
    /* acsState.informTime = bt;  ???????????????????????*/
    return TRX_OK;
}

/* The ParameterKey needs to survice a power-off/reboot cycle */
TRX_STATUS getMSrvrParameterKey(char **value)
{
    *value = acsState.parameterKey? strdup(acsState.parameterKey) : strdup("");
    return TRX_OK;
}

TRX_STATUS getConnectionReqURL(char **value)
{
    *value = acsState.connReqURL? strdup(acsState.connReqURL) : strdup("");
    return TRX_OK;
}

TRX_STATUS getConnectionUsername(char **value)
{
    *value = acsState.connReqUser? strdup(acsState.connReqUser) : strdup("");
    return TRX_OK;
}

TRX_STATUS setConnectionUsername(const char *value)
{
    free(acsState.connReqUser);
    acsState.connReqUser= strdup(value);
    return TRX_OK;
}

TRX_STATUS getConnectionPassword(char **value)
{
    *value = acsState.connReqPwd? strdup(acsState.connReqPwd) : strdup("");
    return TRX_OK;
}
TRX_STATUS setConnectionPassword(const char *value)
{
    free(acsState.connReqPwd);
    acsState.connReqPwd= strdup(value);
    return TRX_OK;
}
TRX_STATUS getKickURL(char **value)
{
    *value = acsState.kickURL? strdup(acsState.kickURL) : strdup("");
    return TRX_OK;
}

TRX_STATUS setKickURL(const char *value)
{
    free(acsState.kickURL);
    acsState.kickURL= strdup(value);
    return TRX_REBOOT;
}
TRX_STATUS getUpgradesManaged(char **value)
{
    *value = strdup(acsState.acsURL?"1":"0");
    return TRX_OK;
}

TRX_STATUS setUpgradesManaged(const char *value)
{
    acsState.upgradesManaged = testBoolean(value);
    /* call manufacture function here */
    return TRX_OK;
}

TRX_STATUS getDeviceSummary (char **value)
{
    *value = strdup("InternetGatewayDevice:1.0[](Baseline:1)");
    return TRX_OK;
}

/* 
* Initialize all the instance nodes in the parameter tree based
* on the current configuration data.
* 
*/

/* externs for instance obect initialization in bcmWrapper */
extern TRxObjNode   LANDeviceInstanceDesc[];
extern TRxObjNode   WANDeviceInstanceDesc[];
extern TRxObjNode   WANDeviceWANConnectionDeviceInstanceDesc[];
extern TRxObjNode   WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc[];

void reInitInstances(void)
{
	fprintf(stderr, "reInitInstance()\n");
}
void initInstances(void)
{
    TRxObjNode *n;
    int id;
    InstanceDesc    *idp;
    InstanceDesc    *wanDeviceIdp;
    InstanceDesc    *connectionIdp1;
    InstanceDesc    *connectionIdp2;

	lastInstanceId = 10;  /* start at 10 for testing */
	initLANDeviceInstances();
    /* wanDeviceDesc */
    n = WANDeviceInstanceDesc;
    id = 1; /* getWanDeviceInstanceId() This value needs to be persistent */
    wanDeviceIdp = getNewInstanceDesc(n, NULL, id);

    /* Create: */
    /* InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.{i}. */
    n = WANDeviceWANConnectionDeviceInstanceDesc;
    id = 1;
    connectionIdp1 = getNewInstanceDesc(n,wanDeviceIdp, id );
    id = 2;
    //connectionIdp2 = getNewInstanceDesc(n,wanDeviceIdp, id );
    n = WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc;
    id = 1;
    idp = getNewInstanceDesc(n, connectionIdp1, id);
	id = 2;
	idp = getNewInstanceDesc(n,connectionIdp1, id );
    id = 3;
    //idp = getNewInstanceDesc(n, connectionIdp2, id);
	id = 4;
	//idp = getNewInstanceDesc(n,connectionIdp2, id );
	id = 1;
	//idp = getNewInstanceDesc(n,connectionIdp2, id );
	/*
    id = 3;
	getNewInstanceDesc(n,connectionIdp, id );
	*/
	/*
	id = 4;
	getNewInstanceDesc(n,connectionIdp, id );
	id = 5;
	getNewInstanceDesc(n,connectionIdp, id );
	*/
	/*
	id = 2;
	getNewInstanceDesc(n,connectionIdp, id );
	*/
    /* ForwardingDesc */
        n = forwardingDesc;
        id = 1; /* getForwardingInstanceId() This value needs to be persistent */
        if (idp = getNewInstanceDesc(n,NULL,id)){
			L3ForwardDesc *l3fd;
			l3fd = (L3ForwardDesc *)malloc(sizeof(struct L3ForwardDesc));
			memset(l3fd, 0, sizeof(struct L3ForwardDesc));
			idp->hwUserData = (void *)l3fd; /* Link to bcmData */
		}

    /* other .....*/
}
/*
* Called to initialize the interface to the BCM shared lib
* and also the instance objects as requried by the cfm configuration
*/
void initBCMLibIF(void)
{
    /* init BCM LIB ?????? */

    /* init start time */
    startTime = time(NULL);

    /* init all object Instances */
    initInstances();
}

int tr69RetrieveFromStore(AttSaveBuf **bufp, int *size)
{
	char	*buf;
	int		bufSz;
	int f;
	int	r = 0;
	struct stat status;
	*size = 0;
	if (stat("tr69attrsave", &status)==0 ){
		bufSz = status.st_size;
		if (buf=malloc(bufSz)) {
            *bufp = (AttSaveBuf *)buf;
			if ( f=open("tr69attrsave", 0 )) {
				r = read(f, (void *)buf, bufSz);
				*size = r;
				close(f);
			}
		}
	}
	return r;
}

int	tr69SaveToStore( AttSaveBuf *ap )
{
	int f;
	int	r = 0;
    int		saveSz;

	ap->sigValue = 0x4321;
	saveSz = sizeof(struct AttSaveBuf)+ sizeof(struct AttEntry)*ap->numAttSaved;
	fprintf(stderr, "saveAttributes size =%d entries=%d\n", saveSz, ap->numAttSaved);
	if ( f=open("tr69attrsave", O_WRONLY|O_CREAT, S_IWUSR|S_IRUSR)) {
	    r = write(f, (void *)ap, saveSz);
		if (r<0) {
			fprintf(stderr, "Error writing tr69attrsave %s\n", strerror(errno));
		}
		close(f);
	}
	return r;
}


int getRAMSize(void)
{
	return 230000000 ;
}
