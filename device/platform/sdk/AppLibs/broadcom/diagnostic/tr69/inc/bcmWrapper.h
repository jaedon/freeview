#ifndef BCMWRAPPER_H
#define BCMWRAPPER_H
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
 * File Name  : bcmTestWrapper.h
 *
 * Description: Prototypes for interface functions to Broadcom 
 *              library and hardware environment. 
 * $Revision: 1.16 $
 * $Id: bcmWrapper.h,v 1.16 2006/02/17 21:12:52 dmounday Exp $
 *----------------------------------------------------------------------*/
 
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h"

#ifndef uint32
#define uint32 unsigned long
#endif

typedef struct gwStateData {
        char    rebootCommandKey[33];
        char    downloadCommandKey[33];
        char    uploadCommandKey[33];
        char    newParameterKey[33];
        int       lastInstaceID;                          
        int       contactedState;
                int             dlFaultStatus;
                time_t  startDLTime;
                time_t  endDLTime;
        char    padforexpansion[498];             
} GWStateData, *PGWStateData;

/* general system status and configruation retrieval */
/* Returns state of WAN interface to be used by tr69 client */
typedef enum {
    eWAN_INACTIVE,
    eWAN_ACTIVE
} eWanState;

eWanState getWanState(void);


int reInitInstancesFromBCMObj(uint32 objId, TRxObjNode *pmNode,
                                                           InstanceDesc *pppParentIdp);
eACSContactedState getACSContactedState(void);
int getNewInstanceId(void);     /* return a new unused instance id */
void reInitInstances(void);
void initBCMLibIF(void);
void saveTR69StatusItems( ACSState *a );
int tr69RetrieveFromStore(AttSaveBuf **bufp, int *size);
int     tr69SaveToStore( AttSaveBuf *ap);
void wrapperSaveConfigurations(void);
void wrapperReboot(eACSContactedState rebootContactValue);
void wrapperFactoryReset(void);
int  preDownloadSetup(ACSState *acs, DownloadReq *r);
int  downloadComplete(DownloadReq *r, char *buf);
int  preUploadSetup(ACSState *acs, UploadReq *r);
int  uploadComplete(UploadReq *r, char *buf);
void setACSContactedState(eACSContactedState state);
void saveACSContactedState(void);
int     CfmDbLock(void);
void CfmDbUnlock(void);
int getRAMSize(void);
void init_tr69_def(void);
void initTr69(void);
#endif /*BCMWRAPPER_H*/
