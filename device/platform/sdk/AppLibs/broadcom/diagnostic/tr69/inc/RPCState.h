#ifndef RPCSTATE_H
#define RPCSTATE_H
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
 * File Name  : RPCState.h
 *
 * Description: RPC states and data structures 
 * $Revision: 1.23 $
 * $Id: RPCState.h,v 1.23 2006/01/30 23:01:25 dmounday Exp $
 *----------------------------------------------------------------------*/
 
/* fault codes */
#define NO_FAULT    0
#define RPC_PENDING 1
#define RPC_FAULTS  9000

/* Has the ACS been contacted,
* This is used to determine the type of Inform Event on 
* startup.
*/
typedef enum {
    eACS_NEVERCONTACTED = 0,
    eACS_CONTACTED,                  /* NOTICE: These enum are order dependent*/
    eACS_DOWNLOADREBOOT,
    eACS_SETVALUEREBOOT,
        eACS_ADDOBJECTREBOOT,
        eACS_DELOBJECTREBOOT,
        eACS_RPCREBOOT,
        eACS_UPLOADREBOOT
} eACSContactedState;            /* NOTICE: These enum are order dependent*/
#define NOREBOOT eACS_NEVERCONTACTED

/* status enum for acs connection and msg xfer */
typedef enum {
    eOK,
    eConnectError,
    eGetError,
    ePostError,
    eAuthError,
    eDownloadDone,
    eAcsDone,
    eUploadDone,
}AcsStatus;

/* rpcRun return status */
typedef enum {
        eRPCRunOK,              /* sent RPC response to ACS */
        eRPCRunEnd,             /* sent NULL http msg to ACS */
        eRPCRunFail             /* RPC run failed no reponse to send */
                                        /*  this should probably send a fault */
}RunRPCStatus;

/* inform event enumb -- kind of inform msg */
typedef enum {
    eIEBootStrap,
    eIEBoot,
    eIEPeriodix,
    eIEScheduled,
    eIEValueChanged,
    eIEKicked,
    eIEConnectionRequest,
    eIETransferComplete,
    eIEDiagnostics,
    eIEMethodResult,
    eIEXVendor
} eInformEvent;


typedef enum {
    rpcUnknown=0,
    rpcGetRPCMethods,
    rpcSetParameterValues,
    rpcGetParameterValues,
    rpcGetParameterNames,
    rpcGetParameterAttributes,
    rpcSetParameterAttributes,
    rpcAddObject,
    rpcDeleteObject,
    rpcReboot,
    rpcDownload,
    rpcUpload,
    rpcFactoryReset,            /******** last rpc method ******/
    rpcInformResponse,          /* responses start here */
    rpcTransferCompleteResponse,
        rpcGetRPCMethodsResponse,
    rpcFault                                    /* soapenv:Fault response from ACS */
} eRPCMethods;
#define LAST_RPC_METHOD    rpcFactoryReset     /* see above enumeration */

#define MAXINFORMEVENTS 16 
typedef struct InformEvList {
    eInformEvent     informEvList[MAXINFORMEVENTS];
    int              informEvCnt;   /* number of events in list */
        eRPCMethods              mMethod;               /* set if M <method> event required */
} InformEvList;

typedef enum {
    eFirmwareUpgrade=1,
    eWebContent=2,
    eVendorConfig=3,
    eVendorLog=4
} eFileType;

typedef struct DownloadReq {
    eFileType efileType;
        char    *commandKey;
    char    *url;
    char    *user;
    char    *pwd;
    int     fileSize;
    char    *fileName;   /* ignore in this implementation- everything is in memory */
    int     delaySec;
} DownloadReq;

typedef struct UploadReq {
    eFileType efileType;
        char    *commandKey;
    char    *url;
    char    *user;
    char    *pwd;
    int     fileSize;
    char    *fileName;   /* ignore in this implementation- everything is in memory */
    int     delaySec;
} UploadReq;


typedef struct ParamItem {
    struct ParamItem   *next;
    char   *pname;
    char   *pvalue;
        char   *pOrigValue;
        int             fault;          /* 0 or set fault code */
}ParamItem;

typedef struct AttributeItem {
    struct AttributeItem *next;
    char   *pname;
    int    notification;
    int    chgNotify;
    int    chgAccess;
    int    subAccess;   /* need to add list here if spec changes or vendor reqmts */
} AttributeItem;

typedef struct ParamNamesReq {
    char    *parameterPath;
    int     nextLevel;
} ParamNamesReq;

typedef struct AddDelObject {
    char    *objectName;    /* For Add the object name is xxx.yyy. */
                            /* for Delete the object anme is xxx.yyy.i. */
                            /* where i is the instance number */
} AddDelObject;

typedef struct RPCAction {
    char   *ID;    /* pointer to ID string */
    char    *informID;  /* ID string sent with last inform */
    eRPCMethods rpcMethod;
    int     arrayItemCnt;   /* cnt of items in parameter list-not used */
    char    *commandKey;    /* */
    char    *parameterKey;  /* for setParameterValue key */
    union {
        ParamItem       *pItem;
        AttributeItem   *aItem;
        ParamNamesReq   paramNamesReq;
        AddDelObject    addDelObjectReq;
        DownloadReq     downloadReq;
        UploadReq       uploadReq;
        /* more items here later for each rpc method*/
    } ud;
} RPCAction;

/* structures to save notification entries */
typedef struct AttEntry {
        short int       nodeIndex;                      /* index in CPE param table */
        short int       attrValue;                      /* attribute value (1..2)  0 is not saved*/
        int                     instanceId;                     /* Id of instance or zero */
} AttEntry;

typedef struct AttSaveBuf {
        short int       sigValue;
        short int       numAttSaved;            /* number of notification attributes saved*/
        AttEntry        attEntry[100];
} AttSaveBuf;


RPCAction* newRPCAction(void);
void freeRPCAction(RPCAction *item);

void dumpAcsState(void);
void dumpRpcAction(RPCAction *);
char *buildInform(RPCAction *a, InformEvList *, int);
void updateKeys( RPCAction *a);
RunRPCStatus runRPC(void);
int  checkActiveNotifications(void);
void initNotification(void);
int  getAllNotifications(void);
void resetNotificationBuf(void);
void saveConfigurations(void);
void rebootCompletion(void);
void factoryResetCompletion(void);
char *sendTransferComplete(void);
int     restoreNotificationAttr(void);
void saveNotificationAttributes(void);
#endif
