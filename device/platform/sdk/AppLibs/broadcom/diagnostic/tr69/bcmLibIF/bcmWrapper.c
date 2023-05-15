
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
 * $Revision: 1.42 $
 * $Id: bcmWrapper.c,v 1.42 2006/02/17 21:12:52 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <syslog.h>
/*#include "bcmtypes.h"*/
#include "bcmWrapper.h"
#ifdef WT104_SUPPORT  
#include "bcmVoiceWrapper.h"
#endif
#include "../main/utils.h"
#include "../inc/tr69cdefs.h"
#include "../inc/appdefs.h"
#include "../main/informer.h"
#include "../SOAPParser/CPEframework.h"
#include "../main/httpDownload.h"
#include "../main/httpUpload.h"
#include "bcm_types.h"
#if 0
//#include "bcmcfmntwk.h"
//#include "board_api.h"
//#include "dbapi.h"
//#include "ifcwanapi.h"
#endif
#include "syscall.h"
/*#define DEBUG*/

#include "../inc/tr69cdefs.h" /* defines for ACS state */
 ACSState acsState;

//removing extern for Trellis Use. Will have to look into it
 BcmCfm_TR069Cfg_t      defTr69;

eACSContactedState acsContactedState;

void BcmCfm_flash(char *buf, int fileSize);

void init_tr69_def(void)
{
        acsState.acsURL                 = strdup(ACS_URL);
        acsState.acsUser                = strdup(ACS_USER);
        acsState.acsPwd                 = strdup(ACS_PASSWD);
        acsState.connReqPath    = strdup(ACSCONNPATH);
        acsState.connReqUser    = strdup(ACSCONN_USER);
        acsState.connReqPwd     = strdup(ACSCONN_PASSWD);
        acsState.kickURL                = strdup(ACS_KICK_URL);
        acsState.provisioningCode = strdup(ACS_DEF_STRING);
        acsState.downloadCommandKey = NULL;
        acsState.uploadCommandKey = NULL;
        acsState.rebootCommandKey       = NULL;
        acsState.rebootCommandKey       = NULL;
        acsState.parameterKey           = strdup(ACS_DEF_STRING);
        acsState.newParameterKey        = NULL;
        acsState.upgradesManaged = 1;
        acsState.dlFaultStatus = 0;
        acsState.startDLTime = 0;
        acsState.endDLTime = 0;
        acsState.ulFaultStatus = 0;
        acsState.startULTime = 0;
        acsState.endULTime = 0;
        acsState.informEnable = 0;
        acsState.informInterval = 10;
        acsState.informTime = 0;
}


//initTR69() to intilize the everything in TR69 when used as trellis service
void initTr69(void)
{
     // printf("Initialize basic config data from default values\n");
    init_tr69_def();
    /* INIT Protocol http, ssl */
       // printf("init protocol\n");
    proto_Init();
    /* init bcm library interface */
       // printf("init BCMLibIF\n");
    initBCMLibIF();
    /* Just booted so send initial Inform */
       // printf("init Informer\n");
    initInformer();
    /* init cfm listener */
    //printf("init CFMListentner\n");
    initCFMListener();

}



/* getACSContactedState returns eACS_NEVERCONTACTED if the ACS has never */
/* been contacted since initial customer configuration.  eACS_CONTACTED is */
/* returned if the ACS has been contacted before. This is used to determine*/
/* the boot event in the Inform */

eACSContactedState getACSContactedState(void)
{
#if 0
        /* Need to fetch from persistent memory  */
        void *objValue;
        uint32 len = sizeof(GWStateData);
        eACSContactedState state = eACS_NEVERCONTACTED;

        if ( BcmCfm_scratchPadGet("tr69c_acsState", &objValue,  &len) == BcmCfm_Ok ) {
                PGWStateData pObj = (PGWStateData)objValue;
                state = pObj->contactedState;
                BcmCfm_free(objValue);
        } else
                slog(LOG_ERR, "tr69c: Unable to read tr69c acs state data from scratch pad");

        acsContactedState = state;
#ifdef DEBUG
        fprintf(stderr, "getACSContactedState returns %d\n", acsContactedState);
#endif        
        return state;
#endif
        return eACS_NEVERCONTACTED;
}

void setACSContactedState(eACSContactedState state)
{
        acsContactedState = state;
}


/*
* Save the TR69 state values across the reboot
*/
void  saveTR69StatusItems( ACSState *a )
{
#if 0
        GWStateData     gwState;

        /* init strings*/
        memset(&gwState, 0, sizeof(GWStateData));

        /* fill State Data structure from acsState data */
        if ( a->downloadCommandKey )
                strncpy(gwState.downloadCommandKey, a->downloadCommandKey, sizeof(gwState.downloadCommandKey));
        if ( a->rebootCommandKey )
                strncpy(gwState.rebootCommandKey, a->rebootCommandKey, sizeof(gwState.rebootCommandKey));
        if ( a->newParameterKey )
                strncpy(gwState.newParameterKey, a->newParameterKey, sizeof(gwState.newParameterKey));
        else if (a->parameterKey)/* otherwise keep old one */
                strncpy(gwState.newParameterKey, a->parameterKey, sizeof(gwState.newParameterKey));
                
        /*gwState.lastInstaceID = lastInstanceId;*/
        gwState.contactedState = acsContactedState;
        gwState.dlFaultStatus = a->dlFaultStatus;
        gwState.startDLTime = a->startDLTime;
        gwState.endDLTime = a->endDLTime;

        if (BcmCfm_scratchPadSet("tr69c_acsState", &gwState, sizeof(GWStateData))!= BcmCfm_Ok)
                slog(LOG_ERR, "tr69c: Unable to save TR69 status in scratch PAD");
#endif
        BSTD_UNUSED(a);
}

/* save to scratch pad */
void saveACSContactedState()
{
        saveTR69StatusItems(&acsState);
        #if 0
        void *objValue;
        uint32 len = sizeof(GWStateData);

        if ( BcmCfm_scratchPadGet("tr69c_acsState", &objValue,  &len) == BcmCfm_Ok ) {
                PGWStateData pObj = (PGWStateData)objValue;
                pObj->contactedState = acsContactedState;
                if (BcmCfm_scratchPadSet("tr69c_acsState", objValue, sizeof(GWStateData))!= BcmCfm_Ok)
                        slog(LOG_ERR, "tr69c: Unable to save ACS contact state in scratch PAD");
                BcmCfm_free(objValue);
        } else
                slog(LOG_ERR, "tr69c: Unable to retrieve ACS contact state in scratch PAD");
        #endif
}

void retrieveTR69StatusItems(void) {
#if 0
        void *objValue;
        uint32 len = sizeof(GWStateData);

        if ( BcmCfm_scratchPadGet("tr69c_acsState", (void*)&objValue,  &len) == BcmCfm_Ok ) {
                PGWStateData pObj = (PGWStateData)objValue;

                if ( acsState.downloadCommandKey ) {
                        free(acsState.downloadCommandKey);
                        acsState.downloadCommandKey = NULL;
                }
                if ( pObj->downloadCommandKey )
                        acsState.downloadCommandKey = strdup(pObj->downloadCommandKey);

                if ( acsState.rebootCommandKey ) {
                        free(acsState.rebootCommandKey);
                        acsState.rebootCommandKey = NULL;
                }
                if ( pObj->rebootCommandKey )
                        acsState.rebootCommandKey = strdup(pObj->rebootCommandKey);

                if ( acsState.newParameterKey ) {
                        free(acsState.newParameterKey);
                        acsState.newParameterKey = NULL;
                }
                if ( pObj->newParameterKey )
                        acsState.parameterKey = strdup(pObj->newParameterKey);
                acsState.dlFaultStatus = pObj->dlFaultStatus;
                acsState.startDLTime = pObj->startDLTime;
                acsState.endDLTime = pObj->endDLTime;

                BcmCfm_free(objValue);
        }
#endif
}

void wrapperReboot(eACSContactedState rebootContactValue)
{
        setACSContactedState( rebootContactValue );      /* reset on BcmCfm_download error */
        saveTR69StatusItems(&acsState);
        #ifdef DEBUG
        fprintf(stderr, "CPE is REBOOTING with rebootContactValue =%d\n", rebootContactValue);
        #endif
        BcmCfm_reboot();
}

/*
* Call library factory reset 
*/
void wrapperFactoryReset(void)
{
#if 0
        BcmCfm_restoreDefault();
        BcmCfm_reset();
#endif
}
/*
* Call library save configuration 
*/
void wrapperSaveConfigurations(void)
{
        BcmCfm_saveConfig();
}
/* preDownloadSetup(ACSState *acs, DownloadReq *)
* This function is called before the framework begins downloading of the image file.
* Any unnecessary applications or memory resources should be freed.
* Return 1: to continue download.
*        0: to Abort download.
*/
int  preDownloadSetup(ACSState *acs, DownloadReq *r)
{
/*#ifdef DEBUG*/
#if 1
        fprintf(stderr, "preDownloadSetup: URL=%s\n", r->url);
        fprintf(stderr, "User/pw: %s:%s\n", r->user, r->pwd);
        fprintf(stderr, "Required memory buffer size will be %d\n", r->fileSize);
#endif        
        BSTD_UNUSED(acs);
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
        if ( buf ) {
/*#ifdef DEBUG*/
#if 1
                fprintf(stderr, "downloadComplete -- save flash image\n");
#endif                
                setACSContactedState( eACS_DOWNLOADREBOOT );
                if ( r->efileType == eFirmwareUpgrade ||
                         r->efileType == eFirmwareUpgrade ||
                         r->efileType == eVendorConfig)
                {
                        updateDownLoadKey( r );
                        /* does not return */
                        acsState.dlFaultStatus = 0;     /* no download fault */
                        acsState.dlFaultMsg = "Download successful";
                        saveTR69StatusItems(&acsState);
/*#ifdef DEBUG*/
#if 1
                        fprintf(stderr, "downloadComplete -- save ACSState\n");
#endif                        
                        BcmCfm_flash(buf, r->fileSize);
                }
                else
                {
                        acsState.dlFaultStatus = 9010; /* download failure*/
                        acsState.dlFaultMsg = "Bad flash image format";
                        setACSContactedState( eACS_CONTACTED );  /* reset on BcmCfm_download error */
                        saveTR69StatusItems(&acsState);
                        free(buf);
                        return 1;
                }
        }

        return 0;
}


/* preUploadSetup(ACSState *acs, UploadReq *)
* This function is called before the framework begins uploading of the file.
* Any unnecessary applications or memory resources should be freed.
* Return 1: to continue download.
*        0: to Abort download.
*/
int  preUploadSetup(ACSState *acs, UploadReq *r)
{
/*#ifdef DEBUG*/
#if 1
        fprintf(stderr, "preUploadSetup: URL=%s\n", r->url);
        fprintf(stderr, "User/pw: %s:%s\n", r->user, r->pwd);
        fprintf(stderr, "Required memory buffer size will be %d\n", r->fileSize);
#endif
        BSTD_UNUSED(acs);
        return 1;
}

/* uploadComplete()
*  Called when file has been uploaded. If successful the *buf will point to the
*  image buffer. If *buf is NULL the upload failed.
*  Control is returned to the framework with a 1 if the flash was successful.
*  If the flash image failed the return is a 0. Free the buffer prior
*  to return to the framework.
*/
int  uploadComplete(UploadReq *r, char *buf)
{
        if ( buf ) {
/*#ifdef DEBUG*/
#if 1
                fprintf(stderr, "Upload Complete\n");
#endif
                //setACSContactedState( eACS_DOWNLOADREBOOT );
                if ( r->efileType == eVendorLog ||
                         r->efileType == eVendorLog ||
                         r->efileType == eVendorConfig)
                {
                        updateUpLoadKey( r );
                        /* does not return */
                        acsState.ulFaultStatus = 0;     /* no upload fault */
                        acsState.ulFaultMsg = "Upload successful";
                        saveTR69StatusItems(&acsState);
/*#ifdef DEBUG*/
#if 1
                        fprintf(stderr, "upComplete -- save ACSState\n");
#endif
                        //need to figure out what to do here !!!!
                        //BcmCfm_flash(buf, r->fileSize);
                }
                else
                {
                        acsState.ulFaultStatus = 9011; /* upload failure*/
                        acsState.ulFaultMsg = "not able to Upload file";
                        setACSContactedState( eACS_CONTACTED );  /* reset on BcmCfm_upload error */
                        saveTR69StatusItems(&acsState);
                        free(buf);
                        return 1;
                }
        }

        return 0;
}



int getRAMSize(void)
{
#if 0
         return sysGetSdramSize();
#endif
        return 1;
}
/* Returns state of WAN interface to be used by tr69 client */
eWanState getWanState(void)
{
        /* add BCM shared library call here to determine status of outbound I/F*/
        return eWAN_ACTIVE;
}

/* getNewInstanceId return an instance id in the integer range that
* has not yet been used.This should be save in persistent memory 
* somewhere and initialized on startup.
* Need to think about integer wrap-around and asigning duplicates.
*  ?????????
*/
static int lastInstanceId;
int getNewInstanceId(void)
{
        return ++lastInstanceId;
}

/* Used to save and restore tr69 parameter attributes.
*/

int tr69RetrieveFromStore(AttSaveBuf **bufp, int *size)
{
#if 0
        void    *buf;
        AttSaveBuf      *ap;
        uint32  bufSz = sizeof(struct AttSaveBuf);
        /* read header to determine size of object*/
        if ( BcmCfm_scratchPadGet("tr69c_attrsave", &buf,  &bufSz) == BcmCfm_Ok ) {
                ap = (AttSaveBuf *)buf;
                if ( ap->sigValue==0x4321) {
                        bufSz = ap->numAttSaved*sizeof(struct AttEntry) + sizeof(AttSaveBuf);
                        BcmCfm_free(buf);
                        if ( BcmCfm_scratchPadGet("tr69c_attrsave", &buf,  &bufSz) == BcmCfm_Ok ) {
                if ( (ap = (AttSaveBuf *)malloc(bufSz)) ) {
                                        *size = bufSz;
                                        memcpy( (void *)ap, buf, bufSz);
                                        *bufp = ap;
#ifdef DEBUG
                                        fprintf(stderr, "retrieving %d bytes\n", *size);
#endif
                                }
                                BcmCfm_free(buf);
                        }
                } else {
                        slog(LOG_ERR, "Parameter attribute scratch pad corrupt");
#ifdef DEBUG
                        fprintf(stderr,"Parameter attribute scratch pad corrupt- sig missmatch\n");
#endif
                }
        } else
                slog(LOG_ERR, "tr69c: Unable to retrieve attributes in scratch PAD");
        return bufSz;
#endif
        BSTD_UNUSED(bufp);
        BSTD_UNUSED(size);
        return 0;
}

int     tr69SaveToStore( AttSaveBuf *ap )
{
#if 0
        void    *obj = (void *)ap;
        int             saveSz;

        ap->sigValue = 0x4321;
        saveSz = sizeof(struct AttSaveBuf)+ sizeof(struct AttEntry)*ap->numAttSaved;
#ifdef DEBUG
        fprintf(stderr, "saveAttributes size =%d entries=%d\n", saveSz, ap->numAttSaved);
#endif
        if (BcmCfm_scratchPadSet("tr69c_attrsave", obj, saveSz)!= BcmCfm_Ok){
                slog(LOG_ERR, "tr69c: Unable to save attributes in scratch PAD");
                return 0;
        }
#endif
        BSTD_UNUSED(ap);
        return 1;
}
/**********************************************************************
* STUBS for get/setParameter****************
**********************************************************************/


/* InternetGatewayDevice.Layer3Forwarding.Forwarding.{i}. */

TRX_STATUS getL3ForwardMetric(char **value)
{
        /* metric is not supported yet*/
        *value = strdup("");
        return TRX_OK;
}
TRX_STATUS setL3ForwardMetric(char *value)
{
        /* metric is not supported yet*/
        BSTD_UNUSED(value);
        return TRX_OK;
}
TRX_STATUS getL3ForwardIF(char **value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                uint32 index = idp->instanceID;
                char uName[IFC_LARGE_LEN];
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        if ( BcmCfmNtwk_indexToUserName(l3fd->intfIndex, uName) == BcmCfm_Ok )
                        {
                                char *paramName;
                                if ( (paramName = buildInterfaceNameParamPath(uName)))
                                        *value = strdup(paramName);
                                else
                                        *value = strdup("");
                        }
                        else
                                *value = strdup("");
                        BcmCfm_objFree(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, idp->hwUserData);
                        return TRX_OK;
                }
        }
#endif

        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS setL3ForwardIF(char *value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        char paramName[266];
                        char *uName = NULL;
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        /* use parameter path to get interface name */
                        snprintf(paramName, sizeof(paramName), "%s.Name", value);
                        if ( getGWParameterValue( paramName, &uName)==TRX_OK){
                                if ( BcmCfmNtwk_userNameToIndex(uName, &(l3fd->intfIndex)) == BcmCfm_Ok ) {
                                        if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                                                idp->hwUserData, index) == BcmCfm_Ok ) {
                                                if (uName != NULL) free(uName);
                                                return TRX_OK;
                                        }
                                }
                                if (uName != NULL) free(uName);
                        }
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS getL3ForwardGWIP(char **value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                struct in_addr inaddr;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        inaddr.s_addr = l3fd->gateway;
                        BcmCfm_objFree(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, idp->hwUserData);
                        *value = strdup(inet_ntoa(inaddr));
                        return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS setL3ForwardGWIP(char *value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                struct in_addr inaddr;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        inet_aton(value, &inaddr);
                        l3fd->gateway = inaddr.s_addr;
                        if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                           idp->hwUserData, index) == BcmCfm_Ok )
                                return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS getL3ForwardSMask(char **value)
{
        /* source subnet mask is not supported yet*/
        *value = strdup("");
        return TRX_OK;
}
TRX_STATUS setL3ForwardSMask(char *value)
{
        /* source subnet mask is not supported yet */
        BSTD_UNUSED(value);
        return TRX_OK;
}
TRX_STATUS getL3ForwardSIP(char **value)
{
        /* source IP is not supported yet */
        *value = strdup("");
        return TRX_OK;
}
TRX_STATUS setL3ForwardSIP(char *value)
{
        /* source IP is not supported yet */
        BSTD_UNUSED(value);
        return TRX_OK;
}
TRX_STATUS getL3ForwardDMask(char **value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                struct in_addr inaddr;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        inaddr.s_addr = l3fd->subnetMask;
                        BcmCfm_objFree(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, idp->hwUserData);
                        *value = strdup(inet_ntoa(inaddr));
                        return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS setL3ForwardDMask(char *value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp=getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                struct in_addr inaddr;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        inet_aton(value, &inaddr);
                        l3fd->subnetMask = inaddr.s_addr;
                        if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                           idp->hwUserData, index) == BcmCfm_Ok )
                                return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS getL3ForwardDIP(char **value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                struct in_addr inaddr;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        inaddr.s_addr = l3fd->ipAddress;
                        BcmCfm_objFree(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, idp->hwUserData);
                        *value = strdup(inet_ntoa(inaddr));
                        return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS setL3ForwardDIP(char *value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                struct in_addr inaddr;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        inet_aton(value, &inaddr);
                        l3fd->ipAddress = inaddr.s_addr;
                        if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                           idp->hwUserData, index) == BcmCfm_Ok )
                                return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS getL3ForwardType(char **value)
{
        /* type is not supported yet */
        *value = strdup("");
        return TRX_OK;
}
TRX_STATUS setL3ForwardType(char *value)
{
        /* type is not supported yet */
        BSTD_UNUSED(value);
        return TRX_OK;
}
TRX_STATUS getL3ForwardStatus(char **value)
{
        /* status is not supported yet */
        *value = strdup("");
        return TRX_OK;
}

TRX_STATUS getL3ForwardEnable(char **value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        if ( l3fd->status == BcmCfm_CfgEnabled )
                                *value = strdup("1");
                        else
                                *value = strdup("0");
                        BcmCfm_objFree(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, idp->hwUserData);
                        return TRX_OK;
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}
TRX_STATUS setL3ForwardEnable(char *value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getCurrentInstanceDesc()) ) {
                BcmCfm_NtwkRouteEntryCfg_t *l3fd;
                uint32 index = idp->instanceID;
                if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
                        l3fd = (BcmCfm_NtwkRouteEntryCfg_t *)idp->hwUserData;
                        if ( strcmp(value, "1") == 0 )
                                l3fd->status = BcmCfm_CfgEnabled;
                        else
                                l3fd->status = BcmCfm_CfgDisabled;
                        if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, 
                                                           idp->hwUserData, index) == BcmCfm_Ok )
                                return TRX_OK;
                }
        }
#endif

        BSTD_UNUSED(value);
        return TRX_ERR;
}
/* The AddForwardI is adding a place holder for a forwarding rule.   */

TRX_STATUS addForwardI(char **value)
{
#if 0
        InstanceDesc *idp;

        if ( (idp = getNewInstanceDesc(getCurrentNode(), getCurrentInstanceDesc(), 0)) ) {
                uint32 index = 0;
                if ( BcmCfm_objCreate(BCMCFM_OBJ_NTWK_ROUTE_ENTRY,
                                                          &index) == BcmCfm_Ok ) {
                        idp->instanceID = index;
                        idp->hwUserData = NULL;
                        *value = strdup(itoa(idp->instanceID));
                        return TRX_OK; /* or TRX_BOOT if necessary */
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}

TRX_STATUS delForwardI(char *value)
{
#if 0
        TRxObjNode *n;
        InstanceDesc *idp;
        int     id = atoi(value);

        if ( (idp = findInstanceDesc(n=getCurrentNode(), id)) ) {
                uint32 index = idp->instanceID;
                if ( BcmCfm_objDelete(BCMCFM_OBJ_NTWK_ROUTE_ENTRY,
                                                          index) == BcmCfm_Ok ) {
                        if ( !deleteInstanceDesc(n, id) )
                                return TRX_OK; /* or TRX_BOOT if necessary */
                }
        }
#endif
        BSTD_UNUSED(value);
        return TRX_ERR;
}

extern TRxObjNode  forwardingDesc[];
TRX_STATUS getL3ForwardingNoI(char **value)
{
        *value = strdup( itoa(getInstanceCount(forwardingDesc)));
        return TRX_OK;
}

int getDfltExtIP(void)
{
        return(readIp("192.168.1.1"));     /* ????????????? */
}

TRX_STATUS getWANLANDeviceNumEntries(char **value)
{
        *value = strdup("1");
        return TRX_OK;
}

/* 
* .MangementServer.
*/
TRX_STATUS setMSrvrURL(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        BcmCfm_free((void *)pTr69->acsURL);
        BcmCfm_alloc(strlen(value) + 1, (void **)&(pTr69->acsURL));
        strcpy(pTr69->acsURL, value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        free(acsState.acsURL);
        acsState.acsURL = strdup(value);
#endif
        free(acsState.acsURL);
        acsState.acsURL = strdup(value);
        return TRX_OK;
}
TRX_STATUS getMSrvrURL(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.acsURL);
        acsState.acsURL = strdup(pTr69->acsURL);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

        *value = acsState.acsURL?strdup(acsState.acsURL):strdup("");
#endif
        *value = acsState.acsURL?strdup(acsState.acsURL):strdup("");
        return TRX_OK;
}

TRX_STATUS getMSrvrUsername(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.acsUser);
        acsState.acsUser = strdup(pTr69->acsUser);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

        *value = acsState.acsUser?strdup(acsState.acsUser):strdup("");
#endif
        *value = acsState.acsUser?strdup(acsState.acsUser):strdup("");

        return TRX_OK;
}

TRX_STATUS setMSrvrUsername(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        BcmCfm_free((void *)pTr69->acsUser);
        BcmCfm_alloc(strlen(value) + 1, (void **)&(pTr69->acsUser));
        strcpy(pTr69->acsUser, value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        free(acsState.acsUser);
        acsState.acsUser = strdup(value);
#endif
        free(acsState.acsUser);
        acsState.acsUser= strdup(value);
        return TRX_OK;
}

TRX_STATUS getMSrvrPassword(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.acsPwd);
        acsState.acsPwd = strdup(pTr69->acsPwd);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);
        *value = acsState.acsPwd?strdup(acsState.acsPwd):strdup("");
#endif
        *value = acsState.acsPwd?strdup(acsState.acsPwd):strdup("");
        return TRX_OK;
}

TRX_STATUS setMSrvrPassword(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        BcmCfm_free((void *)pTr69->acsPwd);
        BcmCfm_alloc(strlen(value) + 1, (void **)&(pTr69->acsPwd));
        strcpy(pTr69->acsPwd, value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        free(acsState.acsPwd);
        acsState.acsPwd = strdup(value);
#endif
        free(acsState.acsPwd);
        acsState.acsPwd= strdup(value);

        return TRX_OK;
}

TRX_STATUS setMSrvrInformEnable(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        pTr69->informEnable = testBoolean(value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        acsState.informEnable = testBoolean(value);
#endif

        acsState.informEnable = testBoolean(value);
        return TRX_OK;
}
TRX_STATUS getMSrvrInformEnable(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        acsState.informEnable = pTr69->informEnable;
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);
        
        *value = strdup(acsState.informEnable? "1": "0");
#endif
        *value = strdup(acsState.informEnable? "1": "0");
        return TRX_OK;
}
TRX_STATUS setMSrvrInformInterval(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        pTr69->informInterval = strtoul(value, (char **)NULL, 0);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        acsState.informInterval = atoi(value);
        resetPeriodicInform(acsState.informInterval);
#endif
        acsState.informInterval = atoi(value);
        resetPeriodicInform(acsState.informInterval);
        return TRX_OK;
}
TRX_STATUS getMSrvrInformInterval(char **value)
{
#if 0   
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        acsState.informInterval = pTr69->informInterval;
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

        char    buf[10];
        snprintf(buf,sizeof(buf),"%d",(int)acsState.informInterval);
        *value = strdup(buf);
#endif
        char    buf[10];
        snprintf(buf,sizeof(buf),"%d",(int)acsState.informInterval);
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
        extern char *strptime(const char *s, const char *format, struct tm *tm);
        struct tm bt;
        strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
        /* acsState.informTime = bt;  ???????????????????????*/
        return TRX_OK;
}

/* The ParameterKey needs to survice a power-off/reboot cycle */
TRX_STATUS getMSrvrParameterKey(char **value)
{
        /**     Just use what is in the acsState *****
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.parameterKey);
        acsState.parameterKey = strdup(pTr69->parameterKey);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);
        ****/
        *value = acsState.parameterKey?strdup(acsState.parameterKey):strdup("");
        return TRX_OK;
}
TRX_STATUS getConnectionReqURL(char **value)
{
        *value = acsState.connReqURL?strdup(acsState.connReqURL):strdup("");
        return TRX_OK;
}

TRX_STATUS getConnectionUsername(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.connReqUser);
        acsState.connReqUser = strdup(pTr69->connReqUser);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

        *value = acsState.connReqUser?strdup(acsState.connReqUser):strdup("");
#endif
        *value = acsState.connReqUser?strdup(acsState.connReqUser):strdup("");

        return TRX_OK;
}
TRX_STATUS setConnectionUsername(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        BcmCfm_free((void *)pTr69->connReqUser);
        BcmCfm_alloc(strlen(value) + 1, (void **)&(pTr69->connReqUser));
        strcpy(pTr69->connReqUser, value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        free(acsState.connReqUser);
        acsState.connReqUser= strdup(value);
#endif
        free(acsState.connReqUser);
        acsState.connReqUser= strdup(value);
        return TRX_OK;
}
TRX_STATUS getConnectionPassword(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.connReqPwd);
        acsState.connReqPwd = strdup(pTr69->connReqPwd);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

        *value = acsState.connReqPwd?strdup(acsState.connReqPwd):strdup("");
#endif
        *value = acsState.connReqPwd?strdup(acsState.connReqPwd):strdup("");

        return TRX_OK;
}
TRX_STATUS setConnectionPassword(const char *value)
{
#if 0    
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        BcmCfm_free((void *)pTr69->connReqPwd);
        BcmCfm_alloc(strlen(value) + 1, (void **)&(pTr69->connReqPwd));
        strcpy(pTr69->connReqPwd, value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        free(acsState.connReqPwd);
        acsState.connReqPwd= strdup(value);
#endif

        free(acsState.connReqPwd);
        acsState.connReqPwd= strdup(value);
        return TRX_OK;
}

TRX_STATUS getKickURL(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        free(acsState.kickURL);
        acsState.kickURL = strdup(pTr69->kickURL);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

        *value = acsState.kickURL?strdup(acsState.kickURL):strdup("");
#endif
        *value = acsState.kickURL?strdup(acsState.kickURL):strdup("");
        return TRX_OK;
}

TRX_STATUS setKickURL(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        BcmCfm_free((void *)pTr69->kickURL);
        BcmCfm_alloc(strlen(value) + 1, (void **)&(pTr69->kickURL));
        strcpy(pTr69->kickURL, value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        free(acsState.kickURL);
        acsState.kickURL= strdup(value);
#endif
        free(acsState.kickURL);
        acsState.kickURL= strdup(value);
        return TRX_OK;
}
TRX_STATUS getUpgradesManaged(char **value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
#endif

        acsState.upgradesManaged = defTr69.upgradesManaged;
        /* BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);*/

        *value = strdup(acsState.upgradesManaged?"1":"0");

        return TRX_OK;
}

TRX_STATUS setUpgradesManaged(const char *value)
{
#if 0
        void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
        PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
        pTr69->upgradesManaged = testBoolean(value);
        BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, info, 0);

        acsState.upgradesManaged = testBoolean(value);
#endif
        acsState.upgradesManaged = testBoolean(value);
        return TRX_OK;
}

TRX_STATUS getDeviceSummary (char **value)
{
#ifdef WT104_SUPPORT
    *value = strdup("InternetGatewayDevice:1.0[](Baseline:1, Time:1, Layer3Forwarding:1, LANDevice:1, WANDevice:1, IPPingDiagnostics:1), VoiceService:1.0[1](Endpoint:1, SIPEndpoint:1)");
#else
    *value = strdup("InternetGatewayDevice:1.0[](Baseline:1, Time:1, Layer3Forwarding:1, LANDevice:1, WANDevice:1, IPPingDiagnostics:1)");
#endif
    return TRX_OK;
}

void initForwardingInstances(void) {
#if 0
        TRxObjNode *n= forwardingDesc;
        int id = 0;
        InstanceDesc    *idp = NULL;
        uint32 objIndex = 0;
        void *objValue = NULL;
        PBcmCfm_NtwkRouteEntryCfg_t l3fw = NULL;
        
        /* Get the objects one after another till we fail. */
        while ( (BcmCfm_objGet(BCMCFM_OBJ_NTWK_ROUTE_ENTRY,
                                                   &objValue, &objIndex)) == BcmCfm_Ok ) {
                l3fw = (PBcmCfm_NtwkRouteEntryCfg_t)objValue;
                if ( l3fw->status == BcmCfm_CfgEnabled ) {
                        id = (int)objIndex;
                if ( findInstanceDescNoPathCheck(n, id) == NULL ) {
                                idp = getNewInstanceDesc(n, NULL, id);
                                idp->instanceID = id;
                                idp->hwUserData = NULL;
#ifdef DEBUG
                                fprintf(stderr, "L3Forwarding Desc created new Instance Desc id=%d\n", idp->instanceID);
#endif                                
                        }
                }
                /* Free the mem allocated this object by the get API.*/
                BcmCfm_objFree(BCMCFM_OBJ_NTWK_ROUTE_ENTRY, objValue);
                objIndex++;
        }
#endif
}

/* 
* Lock cfm configuration data
* Return 0: Successful
*            -1: Error - data not locked
*/
int CfmDbLock(void)
{
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
        /*
    void *info;

        BcmCfm_objGet(BCMCFM_OBJ_SYS_MGMT_LOCK, &info, 0);
        BcmCfm_objFree(BCMCFM_OBJ_SYS_MGMT_LOCK, info, 0);
        */
}

/*
* Step thru the objects and create any new instances that are missing.
* Then step thru the instaceDesc and delete any without objects.
*/
int reInitInstancesFromBCMObj(uint32 objId, TRxObjNode *pmNode, InstanceDesc *pppParentIdp)
{
        InstanceDesc *idp;
        int id = 0;
        int cnt;
        uint32 objIndex = 0;
        void *objValue;

        void* pObj = NULL;
        /* Get the objects one after another till we fail. */
        while ( (BcmCfm_objGet(objId, &objValue, &objIndex)) == BcmCfm_Ok ) {
                pObj = objValue;
                if ( pObj != NULL ) {
                        id = (int)objIndex;
                        /* Free the mem allocated this object by the get API. */
                        BcmCfm_objFree(objId, objValue);
                        if (!findInstanceDescNoPathCheck(pmNode, id)) {
                                /* new object so add instance */
                                idp = getNewInstanceDesc(pmNode, pppParentIdp, id);
#ifdef DEBUG
                                fprintf(stderr, "Add portmapping instance %d=%p\n", id, idp);
#endif
            }
                }else{
#ifdef DEBUG
                        fprintf(stderr, "No object found for index = %lu\n", objIndex);
#endif
                        break;
                }
                objIndex++;
        }
        /* Now find if any have been object have been deleted */
        idp = pmNode->paramAttrib.instance;
        while (idp!=NULL) {
                InstanceDesc *next = idp->next;
                objIndex = idp->instanceID;
                if ( BcmCfm_objGet(objId, &objValue, &objIndex) == BcmCfm_Ok ){
            /* something is there so just free it */
                        BcmCfm_objFree(objId, objValue);
                        if (objIndex != (uint32)idp->instanceID){
                                /* not there-- need to delete this instance */
#ifdef DEBUG
                                fprintf(stderr, "Delete portmapping instance %d\n", id);
#endif
                                deleteInstanceDescNoPathCheck(pmNode,idp->instanceID);
                                /* start at beginning since next is now invalid */
                                idp = pmNode->paramAttrib.instance;
                        } else
                                idp = next;
                } else {
#ifdef DEBUG
                        fprintf(stderr, "objGet BCMCFM_OBJ_SEC_VIRTUAL_SERVER objIndex %d failed\n", idp->instanceID);
#endif
                        deleteInstanceDescNoPathCheck(pmNode,idp->instanceID);
                        idp = pmNode->paramAttrib.instance;
                }
        }

        cnt = getInstanceCountNoPathCheck(pmNode);
    return cnt;
}
/* externs for instance obect initialization in bcmWrapper */

void initLANDeviceInstances(void);
void initWANDeviceInstances(void);
void initLANDeviceHostsHostInstance(void);

void reInitInstances(void)
{
        char    *value;

        /* get items that may have been change by WEB-UI */
        getMSrvrInformEnable(&value);
        /* need to free value since it's allocated memory by strdup        */
        if ( value != NULL ) free(value);        
        getMSrvrInformInterval(&value);
        if ( value != NULL ) free(value);        
        getMSrvrInformTime(&value);
        if ( value != NULL ) free(value);        
        getMSrvrURL(&value);
        if ( value != NULL ) free(value);        
        getMSrvrUsername(&value);
        if ( value != NULL ) free(value);        
        getMSrvrPassword(&value);
        if ( value != NULL ) free(value);        
        getConnectionUsername(&value);
        if ( value != NULL ) free(value);        
        getConnectionPassword(&value);
        if ( value != NULL ) free(value);        
    /* Reinitialize WAN devices and static routes in order add new instances that have been added by the
         * Web user interface or by some other non-TR69 method.
         */
printf("reInitInstances: initWANDeviceInstances\n");
        initWANDeviceInstances();
printf("reInitInstances: initForwardingInstances\n");
        initForwardingInstances();
printf("reInitInstances: initLANDeviceInstances\n");
        initLANDeviceInstances();
printf("reInitInstances: initLANDeviceHostsHostInstance\n");
        initLANDeviceHostsHostInstance();
}

extern void initBaslineProfile(void);
TRXGFUNC(getProvisioningCode);
/* 
* Called to initialize the interface to the BCM shared lib
* and also the instance objects as requried by the cfm configuration
*/
void initBCMLibIF(void)
{
        char *value;

        /* initBaslineProfile just gets timestamp */
        initBaslineProfile();        
#if 0
        getUpgradesManaged(&value);
        if ( value != NULL ) free(value);       /*JJC */       
        getConnectionReqURL(&value);
        if ( value != NULL ) free(value);       /*JJC */       
        /*getMSrvrParameterKey(&value);*/
        getKickURL(&value);
        if ( value != NULL ) free(value);       /*JJC */       
        getProvisioningCode(&value);
        if ( value != NULL ) free(value);       /*JJC */       
        retrieveTR69StatusItems();
#endif

printf("initBCMLibIF 10\n");
        reInitInstances();
printf("initBCMLibIF 11\n");

#ifdef WT104_SUPPORT  
printf("initBCMLibIF 12\n");
    initVoiceInstances();
#endif 
        BSTD_UNUSED(value);
}

/* 
* The following are functions that are called by the cfm thread.
* They must not use the event.c event, timer and signaling handling
* functions since they are not thread safe.
*/
static int udp_send(short port, void *data, int len)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  /* fill in server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = readIp("127.0.0.1");
  serv_addr.sin_port        = htons(port);

  /* open udp socket */
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    slog(LOG_ERR, "Could not open socket for send\n");
    return -1; /* could not open socket */
  }

  /* bind any local address for us */ 
  memset(&cli_addr, 0, sizeof(cli_addr));
  cli_addr.sin_family      = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port        = htons(0);

  if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
    slog(LOG_ERR, "Could not bind client socket\n");
    return -2; /* could not bind client socket */
  }
 
  /* send the data */
  if (sendto(sockfd, data, len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != len) {
    slog(LOG_ERR,  "Could not sendto\n");
    return -3; /* could not sendto */
  }
  close(sockfd);
  return 0;
}
void BcmTr69c_CfmMsg( eCFMMsg msg)
{
        int cfmMsg=msg;

        if (udp_send(CFMLISTENPORT, (void*)&cfmMsg, sizeof(cfmMsg))) {
                ;
        }
        return;
}

BcmCfm_Status BcmCfm_objCreate(uint32 objId, void *objValue, uint32 *objIndex)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
        BSTD_UNUSED(objIndex);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_objGet(uint32 objId, void **objValue, uint32 *objIndex)
{
        
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
        BSTD_UNUSED(objIndex);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_objSet(uint32 objId, void *objValue, uint32 objIndex)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
        BSTD_UNUSED(objIndex);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_objDelete(uint32 objId, uint32 objIndex)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objIndex);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_objAlloc(uint32 objId, void **objValue)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
   return TRX_OK;
}

BcmCfm_Status BcmCfm_objFree(uint32 objId, void *objValue)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_alloc(uint32 size, void **objValue)
{
        BSTD_UNUSED(size);
        BSTD_UNUSED(objValue);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_free(void *objValue)
{
        BSTD_UNUSED(objValue);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_stsGet(uint32 objId, void **objValue, uint32 objIndex)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
        BSTD_UNUSED(objIndex);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_stsClear(uint32 objId, uint32 objIndex)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objIndex);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_stsFree(uint32 objId, void *objValue)
{
        BSTD_UNUSED(objId);
        BSTD_UNUSED(objValue);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_reboot(void)
{
    return TRX_OK;
}

BcmCfm_Status BcmCfm_isRebootNeeded(uint32 *needReboot)
{
        BSTD_UNUSED(needReboot);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_restoreDefault(void)
{
    return TRX_OK;
}

BcmCfm_Status BcmCfm_saveConfig(void)
{
    return TRX_OK;
}

BcmCfm_Status BcmCfm_download(char *data, uint32 size)
{
        BSTD_UNUSED(data);
        BSTD_UNUSED(size);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_upload(char *data, uint32 size)
{
        BSTD_UNUSED(data);
        BSTD_UNUSED(size);
    return TRX_OK;
}

BcmCfm_Status BcmCfm_isConfigChanged(uint32 *changeConfig)
{
        BSTD_UNUSED(changeConfig);
    return TRX_OK;
}

void BcmCfm_flash(char *buf, int fileSize)
{
        int fd;
        fd = open("/usr/local/bin/trellis/test", O_CREAT|O_RDWR);

        if (fd < 0)
        {
                printf("!!!BcmCfm_flash: Error opening a file\n");
                return;
        }
        else
        {
                printf("BcmCfm_flash: save the image to flash. size = %d\n", fileSize);
        }
        write(fd, buf, fileSize);
        close(fd); 
}

