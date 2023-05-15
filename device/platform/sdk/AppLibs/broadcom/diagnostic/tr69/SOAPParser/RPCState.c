
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
 * File Name  : RPCState.c
 *
 * Description: RPC routines 
 * $Revision: 1.54 $
 * $Id: RPCState.c,v 1.54 2006/02/03 15:53:05 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <syslog.h>

#include "../inc/tr69cdefs.h" /* defines for ACS state */
#include "../standard/baselineparams.h"
#include "../inc/appdefs.h"
#include "../main/utils.h"
#include "../main/event.h"
#include "../nanoxml/nanoxml.h"
#include "xmlParserSM.h"
#include "CPEframework.h"
#include "RPCState.h"
#include "xmlTables.h"
#include "../main/informer.h"
#include "../main/httpDownload.h"
#include "../main/httpUpload.h"

#ifdef TESTBOX
#include "../bcmLibIF/bcmTestWrapper.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#else
/*#include "bcmtypes.h"*/
/*#include "bcmcfm.h"    */
#include "../bcmLibIF/bcmWrapper.h"
#endif

#define DEBUG

#define empty(x) ((x) == NULL || strcmp((x), "") == 0)
#define PNAME_LTH 257         /* parameter name length+1 */
#define MORE 0
#define NOMORE 1
/* forwards */

static const char *getRPCMethodName(eRPCMethods );
extern eInformState informState;

RPCAction *rpcAction;
RPCAction *dloadAction, *uloadAction;

/* State of ACS maintained here */
ACSState acsState;

int transferCompletePending;
int sendGETRPC;

static eACSContactedState rebootFlag;                   /* a TRX_REBOOT has been returned from a setxxx */
static int saveConfigFlag;              /* save config on disconnect */
int saveNotificationsFlag;
/* Active Notification paramValueStruct buffer pointer and size */
static int activeNotifications;
static int notifyChanges;
static char *valueBuf;
static int valueBufSz;

RPCAction* newRPCAction( void )
{
    RPCAction* rpc;
    if ( (rpc = (RPCAction *)malloc(sizeof(RPCAction)))) {
        memset(rpc,0,sizeof(RPCAction));
    }
    return rpc;
}

#define NUMREQINFORMPARMS 7    /* see following array of pointers */
char *informParameters[NUMREQINFORMPARMS] ={
    "InternetGatewayDevice.DeviceInfo.SpecVersion",
    "InternetGatewayDevice.DeviceInfo.HardwareVersion",
    "InternetGatewayDevice.DeviceInfo.SoftwareVersion",
    "InternetGatewayDevice.DeviceInfo.ProvisioningCode",
    "InternetGatewayDevice.ManagementServer.ConnectionRequestURL",
    "InternetGatewayDevice.ManagementServer.ParameterKey",
    NULL                        /* this is filled in by buildExternalConnParamName() */
};

char *informEventStr[]={
    "0 BOOTSTRAP",
    "1 BOOT",
    "2 PERIODIC",
    "3 SCHECULED",
    "4 VALUE CHANGE",
    "5 KICKED",
    "6 CONNECTION REQUEST",
    "7 TRANSFER COMPLETE",
    "8 DIAGNOSTICS COMPLETE"
};

static char *getInformEventStr(eInformEvent iev, eRPCMethods rpcMethod)
{
    static char eventstrbuf[30];
/*    if (iev>=eIEBootStrap &&  iev<eIEMethodResult)*/
        /* just check against maximum since iev is always greater than eIEBootStrap */
    if (iev<eIEMethodResult)
        return informEventStr[iev];
    else if (iev==eIEMethodResult && rpcMethod!=rpcUnknown ) {
        snprintf(eventstrbuf,sizeof(eventstrbuf),"M %s",getRPCMethodName(rpcMethod));
        return eventstrbuf;
    }
    eventstrbuf[0]='\0';
    return eventstrbuf;
}

#ifdef DEBUG
void dumpAcsState(void)
{
    fprintf(stderr, "ACS State dump\n");
    fprintf(stderr, "HoldRequests       %d\n", acsState.holdRequests);
    fprintf(stderr, "NoMoreRequest      %d\n", acsState.noMoreRequests);
    fprintf(stderr, "CommandKey(DL)     %s\n", acsState.downloadCommandKey);
    fprintf(stderr, "CommandKey(UL)     %s\n", acsState.uploadCommandKey);
    fprintf(stderr, "CommandKey(Reboot  %s\n", acsState.rebootCommandKey);
    fprintf(stderr, "ParameterKey       %s\n", acsState.parameterKey);
    fprintf(stderr, "MaxEnvelopes       %d\n", acsState.maxEnvelopes);
    fprintf(stderr, "RPC Methods supported by ACS:\n");
    fprintf(stderr, "   GetRpcMethods           %s\n", acsState.acsRpcMethods.rpcGetRPCMethods?
            "Yes": "No");
    fprintf(stderr, "   SetParameterValues      %s\n", acsState.acsRpcMethods.rpcSetParameterValues?
            "Yes": "No");
    fprintf(stderr, "   GetParameterValues      %s\n", acsState.acsRpcMethods.rpcGetParameterValues?
            "Yes": "No");
    fprintf(stderr, "   GetParameterNames       %s\n", acsState.acsRpcMethods.rpcGetParameterNames?
            "Yes": "No");
    fprintf(stderr, "   GetParameterAttributes  %s\n", acsState.acsRpcMethods.rpcGetParameterAttributes?
            "Yes": "No");
    fprintf(stderr, "   SetParameterAttributes  %s\n", acsState.acsRpcMethods.rpcSetParameterAttributes?
            "Yes": "No");
    fprintf(stderr, "   Reboot                  %s\n", acsState.acsRpcMethods.rpcReboot?
            "Yes": "No");
    fprintf(stderr, "   FactoryReset            %s\n", acsState.acsRpcMethods.rpcFactoryReset?
            "Yes": "No");
    fprintf(stderr, "   rpcDownload             %s\n", acsState.acsRpcMethods.rpcDownload?
            "Yes": "No");
    /* adding Upload RPC to the tr69CPE */
    fprintf(stderr, "   rpcUpload             %s\n", acsState.acsRpcMethods.rpcUpload?
            "Yes": "No");
}
void dumpRpcAction(RPCAction *a)
{
    fprintf(stderr, "RPC description: RPC Method = %s ID=%s\n",
             getRPCMethodName(a->rpcMethod), a->ID);

}
#endif

/* rebootCompletion routine */
/* This is envoked following the ACS response to the rebootresponse msg */
void rebootCompletion(void)
{
 if (rebootFlag>=eACS_DOWNLOADREBOOT && rebootFlag<=eACS_RPCREBOOT) {
 closeAllFds();
 saveTR69StatusItems( &acsState );
 wrapperReboot(rebootFlag);
 }
 return;
}

/* factoryResetCompletion routine */
/* This is envoked following the ACS response to the FactoryResetResponse msg */
static int factoryResetFlag;
void factoryResetCompletion(void)
{
 if (factoryResetFlag){
 wrapperFactoryReset();
 factoryResetFlag = 0;
 }
 return;
}

/* save Configuration routine */
/* This is envoked following the ACS response to the a set/add/delete RPC msg */
void saveConfigurations(void)
{
  if ( saveConfigFlag ){
  wrapperSaveConfigurations();
  saveConfigFlag = 0;
  }
  return;
}



/* Utility routines for data structures */
static const char *getRPCMethodName(eRPCMethods m)
{
    const char *t;
    switch (m) {
    case rpcGetRPCMethods:
        t="GetRPCMethods";
        break;
    case rpcSetParameterValues:
        t = "SetParameterValues";
        break;
    case rpcGetParameterValues:
        t="GetParameterValues";
        break;
    case rpcGetParameterNames:
        t="GetParamterNames";
        break;
    case rpcGetParameterAttributes:
        t="GetParameterAttributes";
        break;
    case rpcSetParameterAttributes:
        t="SetParameterAttributes";
        break;
    case rpcAddObject:
        t="AddObject";
        break;
    case rpcDeleteObject:
        t="DeleteObject";
        break;
    case rpcReboot:
        t="Reboot";
        break;
    case rpcDownload:
        t="Download";
        break;
    /* adding Upload RPC to the tr69CPE */
    case rpcUpload:
        t="Upload";
        break;
    case rpcFactoryReset:
        t="FactoryReset";
        break;
    case rpcInformResponse:
        t="InformResponse";
        break;
    default:
        t="no RPC methods";
        break;
    }
    return t;
}

static void freeParamItems( ParamItem *item)
{
    ParamItem *next;
    while(item)
    {
        next = item->next;
        free(item->pname);        /* free data */
        free(item->pvalue);
        free(item->pOrigValue);
        free(item);                       /* free ParamItem */
        item = next;
    }
}


static void freeAttributeItems( AttributeItem *item)
{
    AttributeItem *next;
    while(item){
        next = item->next;
        free(item->pname);
        free(item);
        item = next;
    }
}

static void freeDownloadReq( DownloadReq *r)
{
  free(r->url);
  free(r->user);
  free(r->pwd);
  free(r->fileName);
}

/* adding Upload RPC to the tr69CPE */
static void freeUploadReq( UploadReq *r)
{
  free(r->url);
  free(r->user);
  free(r->pwd);
  free(r->fileName);
}


/*
* item is undefined on return
*/
void freeRPCAction(RPCAction *item){
    free(item->ID);
    free(item->parameterKey);
    free(item->commandKey);
    free(item->informID);
    switch(item->rpcMethod){
    case rpcGetParameterNames:
        free(item->ud.paramNamesReq.parameterPath);
        break;
    case rpcSetParameterValues:
    case rpcGetParameterValues:
        freeParamItems(item->ud.pItem);
        break;
    case rpcSetParameterAttributes:
    case rpcGetParameterAttributes:
        freeAttributeItems(item->ud.aItem);
        break;
    case rpcAddObject:
    case rpcDeleteObject:
        free(item->ud.addDelObjectReq.objectName);
        break;
    case rpcDownload:
        freeDownloadReq( &item->ud.downloadReq );
        break;
    case rpcUpload:
        freeUploadReq( &item->ud.uploadReq );
        break;
    default:
        break;
    }
    free(item);
}
/*----------------------------------------------------------------------*
 * memory printf
 */
static void mprintf(char **ps, int *left, const char *fmt, ...)
{
    int n;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(*ps, *left, fmt, ap);
    va_end(ap);
    #ifdef DEBUGXML
    {
        fprintf(stdout, "%s", *ps);
    }
    #endif
    *left -= n;
    *ps += n;
    if (*left < 0) {
    /* out of memory */ 
    slog(LOG_ERR, "xml: mprintf: out of memory");
    *left = 0;
    acsState.fault = 9004;
    return;
    }
}
/*----------------------------------------------------------------------*/
static void xml_mprintf(char **ps, int *left, char *s)
{
  if (s) {
    for (; *s; s++) {
      switch (*s) {
        case '&':
          mprintf(ps, left, "&amp;");
          break;
        case '<':
          mprintf(ps, left, "&lt;");
          break;
        case '>':
          mprintf(ps, left, "&gt;");
          break;
        case '"':
          mprintf(ps, left, "&quot;");
          break;
        case '\'':
          mprintf(ps,left, "&apos;");
        case 9:
        case 10:
        case 13:
          mprintf(ps, left, "&#%d;", *s);
          break;
        default:
          if (isprint(*s))
            mprintf(ps, left, "%c", *s);
          else
            mprintf(ps,left," ");
          break;
      }
    }
  }
}
#ifdef OMIT_INDENT
#define xml_mIndent(A,B,C) ;
#else
#define xml_mIndent(A,B,C) XMLmIndent( A, B, C);
#endif
/*----------------------------------------------------------------------*/
static void XMLmIndent(char **ps, int *left, int indent)
{
    int i;

    if (indent < 0)
      return;

    indent *= 2;
    for (i = 0; i < indent; i++) {
      mprintf(ps, left, " ");
    }
}


static void closeBodyEnvelope(char **ps, int *lth)
{
    xml_mIndent(ps, lth, 2);
    mprintf(ps, lth, "</%sBody>\n", nsSOAP);
    mprintf(ps, lth, "</%sEnvelope>\n", nsSOAP);
}
/* Add <SOAP:Body>
*/
static void openBody(char **ps, int *lth)
{
    xml_mIndent(ps, lth, 2);
    mprintf(ps, lth, "<%sBody>\n", nsSOAP);
}
/*
* Add <SOAP:Envelope
 *      xmlns:....
 *      <..:Header>
 *      ....
 *      </..:Header>
 * to the buffer
 */
static void openEnvWithHeader(int noMore, char *idstr, char **ps, int *lth)
{
    NameSpace   *ns;
    mprintf(ps, lth, "<%sEnvelope", nsSOAP);
    /* generate Namespace declarations */
    ns = nameSpaces;
    while(ns->sndPrefix){
        char    pbuf[40];
        char    *e;
        mprintf(ps,lth,"\n");
        strncpy(pbuf,ns->sndPrefix, sizeof(pbuf));
        e=strchr(pbuf,':');
        if (e) *e='\0'; /* find : in prefix */
        xml_mIndent(ps,lth, 2);
        mprintf(ps,lth,"xmlns:%s=\"%s\"", pbuf, ns->nsURL);
        ++ns;
    }
    mprintf(ps,lth,">\n");
    if(idstr || noMore){
        xml_mIndent(ps,lth, 2);
        mprintf(ps, lth, "<%sHeader>\n", nsSOAP);
        if (idstr) {
            xml_mIndent(ps,lth, 3);
            mprintf(ps, lth, "<%sID %smustUnderstand=\"1\">%s</%sID>\n",
                        nsCWMP, nsSOAP,idstr , nsCWMP);
        }
        /* add HoldRequest here if needed */
        if (noMore) {
            xml_mIndent(ps,lth, 3);
            mprintf(ps, lth, "<%sNoMoreRequests>1</%sNoMoreRequests>\n",
                        nsCWMP, nsCWMP);
        }
        xml_mIndent(ps,lth, 2);
        mprintf(ps, lth, "</%sHeader>\n",nsSOAP);
    }
}

static const char *getFaultCode( int fault )
{
  const char *r;
  switch(fault){
    case 9000:
    case 9001:
    case 9002:
    case 9004:
    case 9009:
    case 9010:
    case 9011:
    case 9012:
    case 9013:
      r = "Server";
      break;
    case 9003:
    case 9005:
    case 9006:
    case 9007:
    case 9008:
      r = "Client";
      break;
    default:
      r = "Vendor";
      break;
  }
  return r;
}

static const char *getFaultStr( int fault)
{
    const char *detailFaultStr;

    switch(fault){
    case 9000:
        detailFaultStr = "Method not supported";
        break;
    case 9001:
        detailFaultStr = "Request denied";
        break;
    case 9002:
        detailFaultStr = "Internal Error";
        break;
    case 9003:
        detailFaultStr = "Invalid arguments";
        break;
    case 9004:
        detailFaultStr = "Resources Exceeded";
        break;
    case 9005:
        detailFaultStr = "Invalid Parameter Name";
        break;
    case 9006:
        detailFaultStr = "Invalid parameter type";
        break;
    case 9007:
        detailFaultStr = "Invalid parameter value";
        break;
    case 9008:
        detailFaultStr = "Attempt to set a non-writeable parameter";
        break;
    case 9009:
        detailFaultStr = "Notification request rejected";
        break;
    case 9010:
        detailFaultStr = "Download failure";
        break;
    case 9011:
        detailFaultStr = "Upload failure";
        break;
    case 9012:
        detailFaultStr = "File transfer server authentication failure";
        break;
    case 9013:
        detailFaultStr = "Unsupported protocol for file transfer";
        break;
    case 9014:
        detailFaultStr = "MaxEnvelopes exceeded";
        break;
    default:
        detailFaultStr = "Vendor defined fault";
        break;
    }
    return detailFaultStr;
}

static void writeSoapFault( RPCAction *a,char **ps, int *lth, int fault )
{

    openEnvWithHeader(MORE, a->ID, ps, lth);
    openBody(ps,lth);
    xml_mIndent(ps,lth, 3);
    mprintf(ps,lth,"<%sFault>\n", nsSOAP);
    xml_mIndent(ps,lth, 4);
    mprintf(ps,lth,"<faultcode>%s</faultcode>\n",getFaultCode(fault));
    xml_mIndent(ps,lth, 4);
    mprintf(ps,lth,"<faultstring>CWMP fault</faultstring>\n");
    xml_mIndent(ps,lth, 5);
    mprintf(ps,lth,"<detail>\n");
    xml_mIndent(ps,lth, 6);
    mprintf(ps,lth,"<%sFault>\n",nsCWMP);
    xml_mIndent(ps,lth, 7);
    mprintf(ps,lth,"<FaultCode>%d</FaultCode>\n",fault);
    xml_mIndent(ps,lth, 7);
    mprintf(ps,lth,"<FaultString>%s</FaultString>\n",getFaultStr(fault));
    if (a->rpcMethod == rpcSetParameterValues) {
        ParamItem   *pi = a->ud.pItem;
        /* walk thru parameters to generate errors */
        while (pi != NULL ) {
            if (pi->fault) {
                xml_mIndent(ps,lth, 7);
                mprintf(ps,lth,"<SetParameterValuesFault>\n");
                xml_mIndent(ps,lth, 8);
                mprintf(ps,lth,"<ParameterName>%s</ParameterName>\n", pi->pname);
                xml_mIndent(ps,lth, 8);
                mprintf(ps,lth,"<FaultCode>%d</FaultCode>\n", pi->fault);
                xml_mIndent(ps,lth, 8);
                mprintf(ps,lth,"<FaultString>%s</FaultString>\n", getFaultStr(pi->fault));
                xml_mIndent(ps,lth, 7);
                mprintf(ps,lth,"</SetParameterValuesFault>\n");
            }
            pi = pi->next;
        }
    }
    xml_mIndent(ps,lth, 6);
    mprintf(ps,lth,"</%sFault>\n",nsCWMP);
    xml_mIndent(ps,lth, 5);
    mprintf(ps,lth,"</detail>\n");
    xml_mIndent(ps,lth, 3);
    mprintf(ps,lth,"</%sFault>\n", nsSOAP);
    closeBodyEnvelope(ps,lth);
}

static char *doGetRPCMethods(RPCAction *a)
{
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    eRPCMethods m;
    if (buf) {
        openEnvWithHeader( MORE, a->ID, &xs, &bufsz);
        openBody(&xs, &bufsz);
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"<%sGetRPCMethodsResponse>\n", nsCWMP);
        xml_mIndent(&xs,&bufsz, 4);
        #ifdef SUPPRESS_SOAP_ARRAYTYPE
        mprintf(&xs, &bufsz,"<MethodList>\n");
        #else
        mprintf(&xs, &bufsz,"<MethodList %sarrayType=\"%sstring[%d]\">\n",
                nsSOAP_ENC, nsXSD, LAST_RPC_METHOD );
        #endif

        for(m=rpcGetRPCMethods; m<=LAST_RPC_METHOD; ++m){
            xml_mIndent(&xs,&bufsz, 5);
            mprintf(&xs, &bufsz, "<string>%s</string>\n",
                    getRPCMethodName(m));
        }
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"</MethodList>\n");
        xml_mIndent(&xs,&bufsz,3);
        mprintf(&xs,&bufsz,"</%sGetRPCMethodsResponse>\n", nsCWMP);
        closeBodyEnvelope(&xs,&bufsz);
    }
    return buf;
}

typedef void (*SoapOutCB)(TRxObjNode *node, char **buf, int *bufsz, int isFragment);
static char paramName[PNAME_LTH];
static int  paramCnt;

static void writeGetAttribute( TRxObjNode *node, char **bp, int *bufsz, int isFragment)
{
    InstanceDope *pDope = findDopeInstance(node);
    int accessIndex = pDope? pDope->accessListIndex: 0;

    BSTD_UNUSED(isFragment);

    xml_mIndent(bp,bufsz,5);
    mprintf(bp,bufsz,"<ParameterAttributeStruct>\n");
    xml_mIndent(bp,bufsz,6);
    mprintf(bp,bufsz,"<Name>%s</Name>\n", paramName);
    xml_mIndent(bp,bufsz,6);
    mprintf(bp,bufsz,"<Notification>%d</Notification>\n", pDope? pDope->notification: 0);
    xml_mIndent(bp,bufsz,6);                 /*** ?????? lot more here is more than just subscriber access */
    #ifdef SUPPRESS_SOAP_ARRAYTYPE
    mprintf(bp,bufsz,"<AccessList>\n");
    #else
    mprintf(bp,bufsz,"<AccessList %sarrayType=\"%sstring[%d]\">\n", nsSOAP_ENC, nsXSD, 
                                                  accessIndex);
    #endif
    if(accessIndex) {
        xml_mIndent(bp,bufsz,7);
        mprintf(bp,bufsz,"<%sstring>%s</%sstring>\n",nsXSD, "Subscriber", nsXSD);
    }
    xml_mIndent(bp,bufsz,6);
    mprintf(bp,bufsz,"</AccessList>\n");
    xml_mIndent(bp,bufsz,5);
    mprintf(bp,bufsz,"</ParameterAttributeStruct>\n");
    ++paramCnt;
}
static void writeGetPName( TRxObjNode *node, char **bp, int *bufsz, int isFragment)
{
    int writeable = 0;
    int makeFragName =0;

    xml_mIndent(bp,bufsz,5);
    mprintf(bp,bufsz,"<ParameterInfoStruct>\n");
    if (node->name != instanceIDMASK && node->paramAttrib.attrib.etype==tInstance) {
        /* current node is tInstance. It points to the instance Node */
        if (node->objDetail && ((TRxObjNode *)(node->objDetail))->setTRxParam) {
            writeable = 1;
        }
        makeFragName = 1;                 /* if instance object then make it a name fragment */
    } else {
        writeable = node->setTRxParam!=NULL;
        makeFragName = node->name == instanceIDMASK;
    }
    xml_mIndent(bp,bufsz,6);
    mprintf(bp,bufsz,"<Name>%s%s</Name>\n", paramName, (isFragment&&makeFragName)?".":"");
    xml_mIndent(bp,bufsz,6);
    mprintf(bp,bufsz,"<Writable>%s</Writable>\n", writeable?"1":"0");
    xml_mIndent(bp,bufsz,5);
    mprintf(bp,bufsz,"</ParameterInfoStruct>\n");
    ++paramCnt;
}
/*
* Input:
 * node: Current node to link from
 * cback; Output routine to display node information
 * buf:   *pointer to next available buffer space.
 * bufsz: pointner to size remaining in buffer(buf).
 * noDescend: Just walk next level of tree- don't desend if true
 * oparam: Parameter passed to output (cback) function as parameter 4
 * 
 * If the node->name[0] is '#' then the node is ignored by the tree traversal.
 * This name allows pseudo parameter names to be added to the paraemter tree 
 * to be used to allow calling of getter/setter functions that are not directly related
 * to  TR069 parameters. These routines are accessed by the CPEFramework searchTree
 * functions.
*/
static void traverseTree(TRxObjNode *node, SoapOutCB cback, char **buf, int *bufsz, int noDesend, int oparam)
{
    InstanceDesc *idp;
    TRxObjNode  *n = node->objDetail;
    int     pNameEnd, plth;
         
        pNameEnd = plth = strlen(paramName);
    
    if ( paramName[pNameEnd-1]!='.' && plth<(PNAME_LTH-1))
        strcpy( &paramName[pNameEnd++], ".");
    
    if (n!=NULL && n->name==instanceIDMASK) {
        /* this is an object instance -- step thru the instances if present */
        idp = n->paramAttrib.instance; 
                while (idp!=NULL && !acsState.fault) {
                        if (checkInstancePath(idp)) {
                                pushInstance(idp);
                                snprintf(&paramName[pNameEnd],PNAME_LTH-pNameEnd,"%d", idp->instanceID);
                                (cback)(n,buf,bufsz, oparam);
                                if (!noDesend)
                                        traverseTree(n,cback,buf,bufsz, 0, oparam);
                                popInstance();
                        }
                        /* truncate paramName */
                        paramName[pNameEnd] = '\0';
                        idp = idp->next;
                }
        return;
    }
    while ( n!=NULL && n->name!=NULL && !acsState.fault ) {
                if (n->name[0] != '#') { /* check for pseudo parameter */
                        strncpy( &paramName[pNameEnd], n->name, PNAME_LTH-pNameEnd);
                        if (n->objDetail==NULL) {
                                (cback)(n, buf, bufsz, oparam);   /* Call output function*/
                        } else if (!noDesend) {
                                /* add current object name to paramName */
                                traverseTree(n,cback,buf,bufsz, 0, oparam);
                                /* truncate paramName */
                                paramName[pNameEnd] = '\0';
                        } else if (n->paramAttrib.attrib.etype == tInstance) {
                                (cback)(n, buf, bufsz, oparam);   /* special test to get instance write indicator*/
                        }                                     /* to indicate AddObject/DeleteObject capability*/
                }
                ++n;
    }
    return;
}


/* 
* GetParameterNames requests a single parameter path or single parameter path fragment 
*/
static char *doGetParameterNames(RPCAction *a)
{
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    int isFragment;
    if (buf) {
        TRxObjNode  *n;
        const char        *pp = a->ud.paramNamesReq.parameterPath;
        if ( pp==NULL || strlen(pp)==0){ 
            n = rootDevice;
            pp = n->name;
            isFragment = 1;
        }
        else {
            n = findGWParameter(pp);
            isFragment = pp[strlen(pp)-1]=='.';
        }

        if (n) {
            /* path found: generate response */
            char *plist;
            int plistsz = PARAMLISTSZ;
            char *xbufp;

            if (n->objDetail==NULL && (a->ud.paramNamesReq.nextLevel || isFragment)) {
                /* invalid paramter */
                acsState.fault = 9003;  
            } else {
                openEnvWithHeader( MORE, a->ID, &xs, &bufsz);
                openBody(&xs, &bufsz);
                xml_mIndent(&xs,&bufsz, 3);
                mprintf(&xs,&bufsz,"<%sGetParameterNamesResponse>\n", nsCWMP);
                plist = (char *)malloc(PARAMLISTSZ);
                xbufp = plist;
                paramCnt=0;
                strncpy(paramName, pp, PNAME_LTH   );
                if (isFragment ) {
                    traverseTree( n, writeGetPName, &xbufp, &plistsz, a->ud.paramNamesReq.nextLevel, isFragment);
                } else {
                    writeGetPName(n, &xbufp, &plistsz, 0);
                }
                xml_mIndent(&xs,&bufsz,4);
                                #ifdef SUPPRESS_SOAP_ARRAYTYPE
                mprintf(&xs,&bufsz,"<ParameterList>\n");
                                #else
                mprintf(&xs,&bufsz,"<ParameterList %sarrayType=\"%sParameterInfoStruct[%d]\">\n",
                     nsSOAP_ENC, nsCWMP, paramCnt);
                                #endif
                                if (paramCnt>0)
                                        mprintf(&xs,&bufsz, plist);  /* copy parameter list */
                free (plist);
                xml_mIndent(&xs, &bufsz, 4);
                mprintf(&xs,&bufsz,"</ParameterList>\n");
                xml_mIndent(&xs,&bufsz,3);
                mprintf(&xs,&bufsz,"</%sGetParameterNamesResponse>\n", nsCWMP);
                closeBodyEnvelope(&xs,&bufsz);
            }
        } else {
            acsState.fault = 9005; /* Invalid Parameter Name */
        }
        if( acsState.fault != 0) {
            /* reset buffer and build fault here */
            xs = buf;
            bufsz = XMLBUFSZ;
            writeSoapFault(a,&xs,&bufsz, acsState.fault);
            #ifdef DEBUG
            fprintf(stderr, "Fault in GetParameterNames %d\n", acsState.fault);
            #endif 
        }
    }
    return buf;
}
/*
* set the Attributes in the DopeDescriptor for the Node,instance.
 * Returns: 0 or fault value.
 * 
*/
/*static int setAttributes( TRxObjNode *n, AttributeItem *pi )*/
static void setAttributes( TRxObjNode *n, char **p, int *ret, int fragment)
{
        InstanceDope    *pDope;
        *ret = 0;
        AttributeItem *pi = (AttributeItem *)p; 
        if ( (pDope=findDopeInstance(n))) {
                /* found instance dope */
                if (pi->chgNotify ){
                        if (!(pi->notification==ACTIVE_NOTIFICATION
                                  && n->paramAttrib.attrib.inhibitActiveNotify))
                                pDope->notification = pi->notification;
                        else if (!fragment)
                                *ret=9009;
                }
                if (pi->chgAccess)
                        pDope->accessListIndex = pi->subAccess;
        } else
                *ret=9002;
}
static char *doSetParameterAttributes(RPCAction *a)
{

    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    if (buf) {
        AttributeItem   *pi = a->ud.aItem;
        /* first set attributes */
        while (pi!=NULL && !acsState.fault ) {
                        int     isFragment = 0;
                        TRxObjNode  *n;
                        const char        *pp = pi->pname;
                        if ( pp==NULL || strlen(pp)==0){ 
                                n = rootDevice;
                                pp = n->name;
                                isFragment = 1;
                        }
                        else {
                                n = findGWParameter(pp);
                                isFragment = pp[strlen(pp)-1]=='.';
                        }
                        if (n) {
                                /* path found: apply attribute change */
                                int status=0;
                                if (n->name!=instanceIDMASK && n->paramAttrib.attrib.etype!=tInstance )
                                        setAttributes(n, (char **)pi, &status , isFragment);
                                if (isFragment)
                                        /*traverseTreeAttributes( n, pi, isFragment);*/
                                        traverseTree(n, setAttributes, (char **)pi, &status, 0, isFragment);
                                acsState.fault = status;
                        } else {
                                acsState.fault = 9005; /* invalid parameter name */
                        }
                        pi = pi->next;
                }
        if (!acsState.fault) {
                        saveNotificationsFlag = 1;
            /* build good response */
            openEnvWithHeader( MORE, a->ID, &xs, &bufsz);
            openBody(&xs, &bufsz);
            xml_mIndent(&xs,&bufsz, 3);
            mprintf(&xs,&bufsz,"<%sSetParameterAttributesResponse/>\n", nsCWMP);
            closeBodyEnvelope(&xs,&bufsz);
        } else {
            writeSoapFault(a,&xs,&bufsz, acsState.fault);
            #ifdef DEBUG
            fprintf(stderr, "Fault in SetParameterAttribute %d\n", acsState.fault);
            #endif
        }
    }
    return buf;
}

/* 
* GetParameterAttributes requests a single parameter path or single parameter path fragment
* This RPC uses the paramItem union member. 
*/
static char *doGetParameterAttributes(RPCAction *a)
{

    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    ParamItem   *pi = a->ud.pItem;
    int cnt = 0;    /* parameter counter -- need to count fragment name expansion */
    char *plist;                    /* buffer for param-value */
    int plistsz = PARAMLISTSZ;
    const char *pname = NULL; /* pointer to bad name */
    char *xbufp;

    plist = (char *)malloc(PARAMLISTSZ);
    xbufp = plist;
    if (buf) {
        if (pi!= NULL) {
            /* create response msg start */
            openEnvWithHeader(MORE, a->ID, &xs, &bufsz);
            openBody(&xs, &bufsz);
            xml_mIndent(&xs,&bufsz, 3);
            mprintf(&xs,&bufsz,"<%sGetParameterAttributesResponse>\n", nsCWMP);
            /* step thru the requested parameter list some may be parameter fragments. */
            while (pi!=NULL && !acsState.fault ) {
                int     isFragment = 0;
                TRxObjNode  *n;
                const char        *pp = pi->pname;
                if ( pp==NULL || strlen(pp)==0){ 
                    n = rootDevice;
                    pp = n->name;
                    isFragment = 1;
                }
                else {
                    n = findGWParameter(pp);
                    isFragment = pp[strlen(pp)-1]=='.';
                }
                if (n) {
                    /* path found: generate response */
                    paramCnt=0;
                    strncpy(paramName, pp, PNAME_LTH   );
                    if (isFragment ) {
                        traverseTree( n, writeGetAttribute, &xbufp, &plistsz, 0, isFragment);
                        cnt += paramCnt;
                    } else {
                        writeGetAttribute(n, &xbufp, &plistsz, 0);
                        ++cnt;
                    }
                } else {
                    pname = pp;
                    acsState.fault = 9005; /* invalid parameter name */
                }
                pi = pi->next;
            }
        }else
            acsState.fault = 9003; /* no parameter specified - Invalid arguments */
        if (!acsState.fault) {
            /* good response */
            xml_mIndent(&xs,&bufsz,4);
                        #ifdef SUPPRESS_SOAP_ARRAYTYPE
                        mprintf(&xs,&bufsz,"<ParameterList>\n");
                        #else
            mprintf(&xs,&bufsz,"<ParameterList %sarrayType=\"%sParameterAttributeStruct[%d]\">\n",
                 nsSOAP_ENC, nsCWMP, cnt);
                        #endif
                        if (cnt>0)
                                mprintf(&xs,&bufsz, plist);  /* copy parameter list */
            free (plist);
            xml_mIndent(&xs, &bufsz, 4);
            mprintf(&xs,&bufsz,"</ParameterList>\n");
            xml_mIndent(&xs,&bufsz,3);
            mprintf(&xs,&bufsz,"</%sGetParameterAttributesResponse>\n", nsCWMP);
            closeBodyEnvelope(&xs,&bufsz);
        } else {
            /* reset buffer and build fault here */
            xs = buf;
            bufsz = XMLBUFSZ;
            writeSoapFault(a,&xs,&bufsz, acsState.fault);
            #ifdef DEBUG
            fprintf(stderr, "Fault in GetParameterAttribute %d\n", acsState.fault);
            #endif
        }
    }
    return buf;
}

static char *doSetParameterValues(RPCAction *a)
{
        int     setParamReboot = 0;;
    char *buf =(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    char *pname = NULL;
    
    if (buf) {
        TRxObjNode  *n;
        ParamItem   *pi = a->ud.pItem;
        acsState.fault = 0;             /* Init to no fault */
        /* first set parameters */
        if (pi!= NULL) {
            while (pi!=NULL) {
                TRX_STATUS  stat;
                pname = pi->pname; /* save name for fault msg */
                if ( (n=findGWParameter(pname)) ){
                    /* found parameter node */
                    if (n->setTRxParam!= NULL){
                                                if (empty(pi->pvalue) && n->paramAttrib.attrib.etype!=tString){
                                                        pi->fault = 9007;
                                                        acsState.fault = 9003;
                                                }else {
                                                        /* first save value for restore */
                            if (n->getTRxParam) {
                                if ( pi->pOrigValue != NULL ) {
                                        free(pi->pOrigValue);
                                                                        pi->pOrigValue = NULL;
                                                                }
                                                                /* ignore errors here */
                                                                n->getTRxParam( &pi->pOrigValue );
                            }                                                                
                            if (pi->pvalue==NULL) /* only valid for tSting params */
                                                                /* fake up a null string to avoid NULL ptr problem*/
                                                                pi->pvalue = strdup("");
                                                        if ((n->setTRxParam != NULL) &&
                                                                ((stat = n->setTRxParam(pi->pvalue))!=TRX_ERR)) {
                                                                if( stat==TRX_REBOOT )
                                                                        setParamReboot = 1;
                                                                pi->fault = 0;
                                                        }
                                                        else {
                                                                slog(LOG_ERR, "Error setting %s to value %s\n", pname, pi->pvalue);
                                                                pi->fault = acsState.fault = 9002; /* Use Internal error for now */
                                                        }
                                                }
                    } else{
                                                pi->fault = 9008;  /* non-writeable param */
                        acsState.fault = 9003;
                                        }
                } else {
                                        pi->fault = 9005; /* invalid parameter name */
                    acsState.fault = 9003; 
                                }
                pi = pi->next;
            }
                        /* if fault above then restore orig parameter values */
                        if (acsState.fault) {
                pi = a->ud.pItem;
                                while (pi!=NULL ) {
                                        pname = pi->pname; /* save name for fault msg */
                                        if ( (n=findGWParameter(pname)) ){
                                                /* found parameter node */
                                                if (n->setTRxParam!= NULL){
                                                        if (pi->pOrigValue==NULL)
                                                                pi->pOrigValue = strdup("");
                                                        n->setTRxParam(pi->pOrigValue);
                                                }
                                        }
                                        pi = pi->next;
                                }
                        }
        }else
            acsState.fault = 9003; /* no parameter specified - Invalid arguments */
        if (!acsState.fault) {
            /* build good response */
            openEnvWithHeader(rebootFlag?NOMORE:MORE, a->ID, &xs, &bufsz);
            openBody(&xs, &bufsz);
            xml_mIndent(&xs,&bufsz, 3);
            mprintf(&xs,&bufsz,"<%sSetParameterValuesResponse>\n", nsCWMP);
            xml_mIndent(&xs,&bufsz, 4);
            if ( setParamReboot )
                rebootFlag = eACS_SETVALUEREBOOT;
            mprintf(&xs,&bufsz,"<Status>%s</Status>\n", rebootFlag?"1": "0");
            xml_mIndent(&xs,&bufsz, 3);
            mprintf(&xs,&bufsz,"</%sSetParameterValuesResponse>\n", nsCWMP);
            closeBodyEnvelope(&xs,&bufsz);
                        saveConfigFlag = 1;
        } else {
            writeSoapFault(a,&xs,&bufsz, acsState.fault);
            #ifdef DEBUG
            fprintf(stderr, "Fault in SetParameterValue %d for%s\n", acsState.fault, pname);
            #endif
        }
    }
    return buf;
}
/*
* write SOAP value with type at buffer location **bp. Limit to size *bufsz.
 * increments paramCnt for each value written to buffer.
 *
*/
static void writeParamValueStruct( TRxObjNode *n, char *val, char **bp, int *bufsz)
{
        /* now fill in ParameterValueStruct in response */
        xml_mIndent(bp,bufsz, 6);
        mprintf(bp,bufsz,"<ParameterValueStruct>\n");
        xml_mIndent(bp,bufsz, 7);
        mprintf(bp,bufsz,"<Name>%s</Name>\n", paramName);
        xml_mIndent(bp,bufsz, 7);

        mprintf(bp,bufsz,"<Value %stype=\"%s%s\">",nsXSI, nsXSD,
                        getValTypeStr(n->paramAttrib.attrib.etype));
        #ifdef SUPPRESS_EMPTY_PARAM
        if (empty(val)) 
                xml_mprintf(bp,bufsz,"empty");
        else
                xml_mprintf(bp,bufsz,val);
        #else
        xml_mprintf(bp,bufsz,val);
        #endif
        mprintf(bp,bufsz,"</Value>\n");
        #ifdef DEBUG
        fprintf(stderr, "%s=%s %s\n",paramName, getValTypeStr(n->paramAttrib.attrib.etype),val?val:"NULL");
        #endif
        xml_mIndent(bp,bufsz, 6);
        mprintf(bp,bufsz,"</ParameterValueStruct>\n");
        ++paramCnt;
        return;
}

static void writeGetPValue( TRxObjNode *n, char **bp, int *bufsz, int isFragment)
{
    if ( n->name==instanceIDMASK)
        return;       /* instance node : no value to get */
    if (n->getTRxParam!= NULL){
        char    *val = NULL; /* getTRxParam allocated value buffer -- need to free it*/
                                            /* init to NULL in case getter returns OK but no data*/
                #ifdef DEBUG
                fprintf(stderr, "> %s =", paramName);
                #endif
                if (n->paramAttrib.attrib.etype == tStringSOnly) {
                        /* used to protect passwords, etc.Return null string */
                        writeParamValueStruct(n,"",bp,bufsz);
                }
        else if (n->getTRxParam(&val)!=TRX_OK){
            acsState.fault = 9002; /* Use Internal error for now */
                        slog(LOG_ERR, "Error getting param value for %s\n", paramName);
                }
        else {
            /* now fill in ParameterValueStruct in response */
                        writeParamValueStruct(n,val,bp,bufsz);
            free(val); /* clean up after getTRxParam */
        }
    } else if (!isFragment )   /* if not walking tree then   */
        acsState.fault = 9005; /* non-existant read function */
}

static char *doGetParameterValues(RPCAction *a)
{

    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    ParamItem   *pi = a->ud.pItem;
    char *plist;                    /* buffer for param-value */
    int plistsz = PARAMLISTSZ;
    const char *pname = NULL; /* pointer to bad name */
    char *xbufp;

    plist = (char *)malloc(PARAMLISTSZ);
    xbufp = plist;
    if (buf) {
        if (pi!= NULL) {

            /* create response msg start */
            openEnvWithHeader( MORE, a->ID, &xs, &bufsz);
            openBody(&xs, &bufsz);
            xml_mIndent(&xs,&bufsz, 3);
                        paramCnt=0;
            mprintf(&xs,&bufsz,"<%sGetParameterValuesResponse>\n", nsCWMP);
            /* step thru the requested parameter list some may be parameter fragments. */
            while (pi!=NULL && !acsState.fault ) {
                int     isFragment = 0;
                TRxObjNode  *n;
                const char        *pp = pi->pname;
                if ( pp==NULL || strlen(pp)==0){ 
                    n = rootDevice;
                    pp = n->name;
                    isFragment =1;
                }
                else {
                    n = findGWParameter(pp);
                    isFragment = pp[strlen(pp)-1]=='.';
                }
                if (n) {
                    /* path found: generate response */
                    strncpy(paramName, pp, PNAME_LTH   );
                    if (isFragment )
                        traverseTree( n, writeGetPValue, &xbufp, &plistsz, 0, isFragment);
                    else if (n->name!=instanceIDMASK)
                        writeGetPValue(n, &xbufp, &plistsz, 0);
                    else
                                                acsState.fault = 9005; /* can't get from an instance */
                } else {
                    pname = pp;
                    acsState.fault = 9005; /* invalid parameter name */
                }
                pi = pi->next;
            }
        }else
            acsState.fault = 9003; /* no parameter specified - Invalid arguments */
        if (!acsState.fault) {
            /* good response */
            xml_mIndent(&xs,&bufsz,4);
            #ifdef SUPPRESS_SOAP_ARRAYTYPE
            mprintf(&xs,&bufsz,"<ParameterList>\n");
                        #else
            mprintf(&xs,&bufsz,"<ParameterList %sarrayType=\"%sParameterValueStruct[%d]\">\n",
                 nsSOAP_ENC, nsCWMP, paramCnt);
                        #endif
                        if (paramCnt>0)
                                mprintf(&xs,&bufsz, plist);  /* copy parameter list */
                        xml_mIndent(&xs,&bufsz, 4);
            mprintf(&xs,&bufsz,"</ParameterList>\n");
            xml_mIndent(&xs,&bufsz, 3);
            mprintf(&xs,&bufsz,"</%sGetParameterValuesResponse>\n", nsCWMP);
            closeBodyEnvelope(&xs,&bufsz);
        } else {
            /* reset buffer and build fault here */
            xs = buf;
            bufsz = XMLBUFSZ;
            writeSoapFault(a,&xs,&bufsz, acsState.fault);
            #ifdef DEBUG
            fprintf(stderr, "Fault in GetParameterValue %d\n", acsState.fault);
            #endif
        }
    }
    free(plist);
    return buf;
}

/* AddObject
*
*/

static char *doAddObject(RPCAction *a)
{
    TRxObjNode  *n;
    char *pp = a->ud.addDelObjectReq.objectName;
    char    *value = NULL;
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

    if ( buf ) {
        if ( (pp[strlen(pp)-1]=='.') && (n = findGWParameter(pp))) {
            if (n->paramAttrib.attrib.etype==tInstance) {
                TRxObjNode  *inode;
                TRX_STATUS  trStatus;

                if ( (inode=n->objDetail) 
                         && (inode->name==instanceIDMASK)
                         && (inode->getTRxParam!=NULL)
                    ) {

                    if( (trStatus = (inode->getTRxParam(&value))) != TRX_ERR){
                        if (trStatus==TRX_REBOOT)
                                                        rebootFlag = eACS_ADDOBJECTREBOOT;

                                                saveConfigFlag = 1;
                        /* build AddObject response */
                        openEnvWithHeader(rebootFlag?NOMORE:MORE, a->ID, &xs,&bufsz);
                        openBody(&xs, &bufsz);
                        xml_mIndent(&xs, &bufsz, 3);
                        mprintf(&xs, &bufsz, "<%sAddObjectResponse>\n", nsCWMP);
                        xml_mIndent(&xs, &bufsz, 4);
                        mprintf(&xs, &bufsz, "<InstanceNumber>%s</InstanceNumber>\n", value);
                        xml_mIndent(&xs, &bufsz, 4);
                        mprintf(&xs, &bufsz, "<Status>%s</Status>\n", trStatus==TRX_REBOOT?"1":"0" );
                        xml_mIndent(&xs, &bufsz, 3);
                        mprintf(&xs, &bufsz, "</%sAddObjectResponse>\n", nsCWMP);
                        closeBodyEnvelope(&xs,&bufsz);
                        free (value);
                                                initNotification();
                    } else {
                        /* fault codes */
                        acsState.fault = 9002;
                    } /* TrStatus == TRX_ERR */
                } else
                    acsState.fault = 9003;
            } /* (n->paramAttrib.attrib.etype==tInstance) */
            else {
              /* not an instance */
              acsState.fault = 9005;
            }
        } else  /* if (pp[strlen(pp)-1]=='.') */
            acsState.fault = 9005;


        if (acsState.fault) {
            writeSoapFault(a, &xs, &bufsz,acsState.fault);
        }
    } /* if (buf) */
    return buf;
}


static TRX_STATUS deleteTree(TRxObjNode *node )
{
    InstanceDesc *idp;
    TRxObjNode  *n = node->objDetail;
        char    *instanceIDstr;
        TRX_STATUS trStatus = TRX_OK;
    
    if (n!=NULL && n->name==instanceIDMASK) {
        /* this is an object instance -- step thru the instances if present */
        idp = n->paramAttrib.instance; 
                while (idp!=NULL && !acsState.fault && trStatus!= TRX_ERR) {
                        pushInstance(idp);
                        if (checkInstancePath(idp)) {
                                trStatus = deleteTree(n);
                                if (trStatus==TRX_REBOOT)
                                        rebootFlag = eACS_DELOBJECTREBOOT;
                                saveConfigFlag = 1;
                                if (n->setTRxParam != NULL) {
                                        instanceIDstr = strdup(itoa(idp->instanceID));
                    trStatus = n->setTRxParam( instanceIDstr);
                                        if (trStatus==TRX_REBOOT )
                                                rebootFlag = eACS_DELOBJECTREBOOT;
                                        saveConfigFlag = 1;
                                        free(instanceIDstr);
                                }
                        }
            popInstance();
                        /* truncate paramName */
                        idp = idp->next;
                }
                /* must return here, instance descriptor arrays are not null terminated */
        return trStatus;
    }
    while ( n!=NULL && n->name!=NULL && !acsState.fault && trStatus!=TRX_ERR) {
        if ( n->objDetail!=NULL) {
            trStatus = deleteTree(n);
            /* truncate paramName */
        }
                ++n;
    }
    return trStatus;
}
/* DeleteObject
*
*/

static char *doDeleteObject(RPCAction *a)
{
    TRxObjNode  *n;
    char *pp = a->ud.addDelObjectReq.objectName;
    int instanceID;
    char *instanceIDstr;
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    TRX_STATUS  trStatus;

    if ( buf ) {
        if ( (pp[strlen(pp)-1]=='.') && (n = findGWParameter(pp)) ){
            if (n->name== instanceIDMASK 
                    && n->setTRxParam!=NULL
                    && (instanceID = getCurrentInstanceID())!= -1) {
                if ( (trStatus = deleteTree(n))!= TRX_ERR) {
                                        if (trStatus==TRX_REBOOT )
                                                rebootFlag = eACS_DELOBJECTREBOOT;
                                        saveConfigFlag = 1;
                                        instanceIDstr = strdup(itoa(instanceID));
                                        /* Needed so checkInstancePath() succeeds.*/
                                        popInstance();
        
                                        if( (trStatus = n->setTRxParam( instanceIDstr)) != TRX_ERR){
                                                if (trStatus==TRX_REBOOT )
                                                        rebootFlag = eACS_DELOBJECTREBOOT;
                                                saveConfigFlag = 1;
                                                /* build DeleteObject response */
                                                openEnvWithHeader(rebootFlag?NOMORE:MORE, a->ID, &xs,&bufsz);
                                                openBody(&xs, &bufsz);
                                                xml_mIndent(&xs, &bufsz, 3);
                                                mprintf(&xs, &bufsz, "<%sDeleteObjectResponse>\n", nsCWMP);
                                                xml_mIndent(&xs, &bufsz, 4);
                                                mprintf(&xs, &bufsz, "<Status>%s</Status>\n", rebootFlag?"1":"0" );
                                                xml_mIndent(&xs, &bufsz, 3);
                                                mprintf(&xs, &bufsz, "</%sDeleteObjectResponse>\n", nsCWMP);
                                                closeBodyEnvelope(&xs,&bufsz);
                                        } else 
                                                /* fault codes */
                                                acsState.fault = 9002;
                                        free (instanceIDstr);
                                } else
                                        acsState.fault = 9003;
            } else
                acsState.fault = 9003;
        } else
            acsState.fault = 9005;
        if (acsState.fault)
            writeSoapFault(a, &xs, &bufsz, acsState.fault);
    }
    return buf;
}


static char *doRebootRPC(RPCAction *a)
{
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

    openEnvWithHeader(NOMORE, a->ID, &xs,&bufsz);
        rebootFlag = eACS_RPCREBOOT;
    openBody(&xs, &bufsz);
    xml_mIndent(&xs, &bufsz, 3);
    mprintf(&xs, &bufsz, "<%sRebootResponse/>\n", nsCWMP);
    closeBodyEnvelope(&xs,&bufsz);
    return buf;
}

static char *doFactoryResetRPC(RPCAction *a)
{
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

        factoryResetFlag = 1;
    openEnvWithHeader( NOMORE, a->ID, &xs,&bufsz);
    openBody(&xs, &bufsz);
    xml_mIndent(&xs, &bufsz, 3);
    mprintf(&xs, &bufsz, "<%sFactoryResetResponse/>\n", nsCWMP);
    closeBodyEnvelope(&xs,&bufsz);
    return buf;
}

/* Build an Inform msg        ???????????????? much more complicated 
* Probably will add a lot more args to the call 
*/

TRXGFUNC(getManufacturer);
TRXGFUNC(getManufacturerOUI);
TRXGFUNC(getProductClass);
TRXGFUNC(getSerialNumber);

char *buildInform(RPCAction *a, InformEvList *infEvent, int myExtIP)
{
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;
    int  i;
    char *pbuf=(char *)malloc(XMLBUFSZ);
    char *ps = pbuf;
    int pbufsz = XMLBUFSZ;
    time_t  curtime = time(NULL);
    
        paramCnt = 0;
    if (buf && pbuf) {
        char *val;
        a->informID = strdup( itoa(rand()));
        openEnvWithHeader(MORE, a->informID, &xs, &bufsz);
        openBody(&xs, &bufsz);
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"<%sInform>\n", nsCWMP);
        xml_mIndent(&xs,&bufsz, 4);
        /* create DeviceId structure */
        mprintf(&xs, &bufsz,"<DeviceId>\n");
        getManufacturer(&val);          /* go direct to parameter since we know it here*/
        xml_mIndent(&xs,&bufsz, 5);
        mprintf(&xs, &bufsz,"<%s>%s</%s>\n",Manufacturer, val, Manufacturer);
        free(val);
        getManufacturerOUI(&val);          /* go direct to parameter since we know it here*/
        xml_mIndent(&xs,&bufsz, 5);
        mprintf(&xs, &bufsz,"<%s>%s</%s>\n", "OUI", val, "OUI");
        free(val);
        getProductClass(&val);          /* go direct to parameter since we know it here*/
        xml_mIndent(&xs,&bufsz, 5);
        mprintf(&xs, &bufsz,"<%s>%s</%s>\n",ProductClass, val, ProductClass);
        free(val);
        getSerialNumber(&val);          /* go direct to parameter since we know it here*/
        xml_mIndent(&xs,&bufsz, 5);
        mprintf(&xs, &bufsz,"<%s>%s</%s>\n",SerialNumber, val, SerialNumber);
        free(val);
        xml_mIndent(&xs,&bufsz, 4);
        mprintf(&xs, &bufsz,"</DeviceId>\n");
        xml_mIndent(&xs,&bufsz, 4);

        #ifdef SUPPRESS_SOAP_ARRAYTYPE
                mprintf(&xs, &bufsz,"<Event>\n");
                #else
        mprintf(&xs, &bufsz,"<Event %sarrayType=\"%sEventStruct[%d]\">\n",
                nsSOAP_ENC, nsCWMP, infEvent->informEvCnt);
                #endif
        for (i=0; i < infEvent->informEvCnt; ++i) {
            char    *ck = NULL;
            xml_mIndent(&xs,&bufsz, 5);
            mprintf(&xs, &bufsz,"<EventStruct>\n");
            xml_mIndent(&xs,&bufsz, 6);
            mprintf(&xs, &bufsz,"<EventCode>%s</EventCode>\n",
                     getInformEventStr(infEvent->informEvList[i], infEvent->mMethod) );
            xml_mIndent(&xs,&bufsz, 6);
            if (infEvent->informEvList[i]==eIEMethodResult && infEvent->mMethod==rpcReboot ) 
                ck=acsState.rebootCommandKey;
            else if (infEvent->informEvList[i]==eIEMethodResult && infEvent->mMethod==rpcDownload ) 
                ck=acsState.downloadCommandKey;
            /* adding Upload RPC to the tr69CPE */
            else if (infEvent->informEvList[i]==eIEMethodResult && infEvent->mMethod==rpcUpload )
                ck=acsState.uploadCommandKey;
            #ifdef SUPPRESS_EMPTY_PARAM
            if (empty(ck))
                mprintf(&xs, &bufsz,"<CommandKey>empty</CommandKey>\n");
                        else
                mprintf(&xs, &bufsz,"<CommandKey>%s</CommandKey>\n", ck);
            #else
            mprintf(&xs, &bufsz,"<CommandKey>%s</CommandKey>\n", ck? ck: "");
            #endif
            xml_mIndent(&xs,&bufsz, 5);
            mprintf(&xs, &bufsz,"</EventStruct>\n");
        }
        xml_mIndent(&xs,&bufsz, 4);
        mprintf(&xs, &bufsz,"</Event>\n");
        xml_mIndent(&xs,&bufsz, 4);
        mprintf(&xs, &bufsz,"<MaxEnvelopes>1</MaxEnvelopes>\n");
        xml_mIndent(&xs,&bufsz, 4);
        mprintf(&xs, &bufsz,"<CurrentTime>%s</CurrentTime>\n", getXSIdateTime(&curtime));
        xml_mIndent(&xs,&bufsz, 4);
        mprintf(&xs, &bufsz,"<RetryCount>%d</RetryCount>\n", acsState.retryCount);
        xml_mIndent(&xs,&bufsz, 4);
        /* the External IP address parameter name is itself variable based on the current WAN connection configuration */
        /* Need to construct is first. The return buffer is a static char buffer */
#ifdef DEBUG
                printf("build inform: 1. myExtIP = %x\n", myExtIP);
#endif
        informParameters[NUMREQINFORMPARMS-1] = buildExternalConnParamName(myExtIP);
#ifdef DEBUG
                printf("build inform: 2\n");
#endif
/*        for (i=0; i<NUMREQINFORMPARMS; ++i) {*/
                /* Can not locate the last parameter. See what happens if we do one less? */
        for (i=0; i<NUMREQINFORMPARMS-1; ++i) {
            TRxObjNode *n; 
            if ( (n=findGWParameter(informParameters[i])) ){ 
                /* found parameter node */ /* need to verify that its an item check TRxType ?????????*/
                if (n->getTRxParam!= NULL){
                    char    *val; /* getTRxParam allocated value buffer -- need to free it*/
                    if ( n->getTRxParam(&val)!=TRX_OK)
                        acsState.fault = 9002; /* Use Internal error for now */
                    else {
                                                strcpy(paramName,informParameters[i]); 
                                                writeParamValueStruct(n,val,&ps,&pbufsz);
                                                free(val);
                    }
                } else {
                    #ifdef DEBUG
                    fprintf(stderr, "Unable to read %s\n", informParameters[i])
                    #endif
                    ;
                }
            } else {
                #ifdef DEBUG
                fprintf(stderr, "Unable to locate %s\n", informParameters[i]);
                #endif
                ;
            }
        }

        #ifdef SUPPRESS_SOAP_ARRAYTYPE
                mprintf(&xs, &bufsz,"<ParameterList>\n");
                #else
        mprintf(&xs, &bufsz,"<ParameterList %sarrayType=\"%sParameterValueStruct[%d]\">\n",
                                nsSOAP_ENC,nsCWMP,paramCnt+notifyChanges);
                #endif
        if (paramCnt>0)
                        mprintf(&xs, &bufsz, pbuf);  /* append parameter buffer */
        free (pbuf);
                if (notifyChanges) {
                        mprintf(&xs, &bufsz, valueBuf);  /* append notify change parameter buffer */
                }
                /* notify changes are released after ACS response */
        xml_mIndent(&xs,&bufsz, 4);
        mprintf(&xs, &bufsz,"</ParameterList>\n");
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"</%sInform>\n", nsCWMP);
        closeBodyEnvelope(&xs,&bufsz);
    } else
    {
        free(pbuf);
        free(buf);
        buf = NULL;
    }
    acsState.retryCount++;  /* increment retry count -- reset to zero by inform response */
    return buf;
}

char *sendGetRPCMethods(void)
{
        char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

        #ifdef DEBUG
        slog(LOG_DEBUG, "sendGetRPCMethods\n");
        #endif
    if (buf) {
        openEnvWithHeader(MORE, NULL, &xs, &bufsz);
        openBody(&xs, &bufsz);
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"<%sGetRPCMethods>\n", nsCWMP);
        xml_mIndent(&xs,&bufsz,3);
        mprintf(&xs,&bufsz,"</%sGetRPCMethods>\n", nsCWMP);
        closeBodyEnvelope(&xs,&bufsz);
        }
        return buf;
}

char *sendTransferComplete(void)
{
        char *ck;
        char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

        #ifdef DEBUG
        slog(LOG_DEBUG, "sendTransferComplete\n");
        #endif
    if (buf) {
        openEnvWithHeader(MORE, NULL, &xs, &bufsz);
        openBody(&xs, &bufsz);
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"<%sTransferComplete>\n", nsCWMP);
        xml_mIndent(&xs,&bufsz, 4);
                ck=acsState.downloadCommandKey;
                #ifdef SUPPRESS_EMPTY_PARAM
                if (ck && strlen(ck)>0)
                        mprintf(&xs, &bufsz,"<CommandKey>%s</CommandKey>\n", ck);
                else
                        mprintf(&xs, &bufsz,"<CommandKey>empty</CommandKey>\n");
                #else
                mprintf(&xs, &bufsz,"<CommandKey>%s</CommandKey>\n", ck? ck: "");
                #endif
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs, &bufsz,"<FaultStruct>\n");
        xml_mIndent(&xs, &bufsz, 5);
        mprintf(&xs, &bufsz,"<FaultCode>%d</FaultCode>\n",acsState.dlFaultStatus);
        xml_mIndent(&xs, &bufsz, 5);
        mprintf(&xs, &bufsz,"<FaultString>%s</FaultString>\n", acsState.dlFaultMsg?
                #ifdef SUPPRESS_EMPTY_PARAM
                                acsState.dlFaultMsg: "empty");
                #else
                                acsState.dlFaultMsg: "");
                #endif
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs, &bufsz,"</FaultStruct>\n");
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs, &bufsz,"<StartTime>%s</StartTime>\n", getXSIdateTime(&acsState.startDLTime));
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs, &bufsz,"<CompleteTime>%s</CompleteTime>\n", getXSIdateTime(&acsState.endDLTime));
        xml_mIndent(&xs,&bufsz,3);
        mprintf(&xs,&bufsz,"</%sTransferComplete>\n", nsCWMP);
        closeBodyEnvelope(&xs,&bufsz);
    }
        return buf;
}

static char *doDownload( RPCAction *a)
{
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

printf("\n###doDownload: \n");
    if (dloadAction)  /* may have tried before so clean up */
        freeRPCAction(dloadAction);
    dloadAction = a;   /* save rpc Action */
    if ( a == rpcAction)
        rpcAction = NULL;  /* if *a is copy of rpcAction. set to NULL */
    if ( preDownloadSetup( &acsState, &a->ud.downloadReq ) )
    {
printf("\n###doDownload: start timer\n");
        setTimer( startDownload, (void *)dloadAction, (1+a->ud.downloadReq.delaySec)*1000 );
printf("\n###doDownload: build good response\n");
        /* build good response */
        openEnvWithHeader(NOMORE, a->ID, &xs, &bufsz);
        openBody(&xs, &bufsz);
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"<%sDownloadResponse>\n", nsCWMP);
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"<Status>1</Status>\n");
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"<StartTime>0001-01-01T00:00:00Z</StartTime>\n");
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"<CompleteTime>0001-01-01T00:00:00Z</CompleteTime>\n");
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"</%sDownloadResponse>\n", nsCWMP);
        closeBodyEnvelope(&xs,&bufsz);
    } 
    else 
    {
        xs = buf;
        bufsz = XMLBUFSZ;
        writeSoapFault(a,&xs,&bufsz, acsState.fault);
    }
    return buf; 
}

/* adding Upload RPC to the tr69CPE */
static char *doUpload( RPCAction *a)
{

    //need to check implementation of upload
    char *buf=(char *)malloc(XMLBUFSZ);
    char *xs = buf;
    int  bufsz = XMLBUFSZ;

printf("\n###doUpload: \n");
    if (uloadAction)  /* may have tried before so clean up */
        freeRPCAction(uloadAction);
    uloadAction = a;   /* save rpc Action */
    if ( a == rpcAction)
        rpcAction = NULL;  /* if *a is copy of rpcAction. set to NULL */
    if ( preUploadSetup( &acsState, &a->ud.uploadReq ) )
    {
printf("\n###doUpload: start timer\n");
        setTimer( startUpload, (void *)uloadAction, (1+a->ud.uploadReq.delaySec)*1000 );
printf("\n###doUpload: build good response\n");
        /* build good response */
        openEnvWithHeader(NOMORE, a->ID, &xs, &bufsz);
        openBody(&xs, &bufsz);
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"<%sUploadResponse>\n", nsCWMP);
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"<Status>1</Status>\n");
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"<StartTime>0001-01-01T00:00:00Z</StartTime>\n");
        xml_mIndent(&xs, &bufsz, 4);
        mprintf(&xs,&bufsz,"<CompleteTime>0001-01-01T00:00:00Z</CompleteTime>\n");
        xml_mIndent(&xs,&bufsz, 3);
        mprintf(&xs,&bufsz,"</%sUploadResponse>\n", nsCWMP);
        closeBodyEnvelope(&xs,&bufsz);
    }
    else
    {
        xs = buf;
        bufsz = XMLBUFSZ;
        writeSoapFault(a,&xs,&bufsz, acsState.fault);
    }
    return buf;
}

/*
* Update the ACSState parameterKey or commandKeys if they are in
* the RPC.
*/
void updateKeys(RPCAction *rpcAction)
{
    if (rpcAction->parameterKey) {
        if (rebootFlag == NOREBOOT ) {
                        /* not rebooting also copy to current parameter key */
                        free(acsState.parameterKey);
            acsState.parameterKey = strdup(rpcAction->parameterKey);
                }
        if (acsState.newParameterKey)
            free(acsState.newParameterKey);
        acsState.newParameterKey = rpcAction->parameterKey;
        rpcAction->parameterKey = NULL;
        }
    if (rpcAction->rpcMethod==rpcReboot && rpcAction->commandKey) {
        if (acsState.rebootCommandKey)
            free(acsState.rebootCommandKey);
        acsState.rebootCommandKey = rpcAction->commandKey;
        rpcAction->commandKey = NULL;
        }
}

RunRPCStatus runRPC(void)
{
    char    *rspBuf = NULL;
        RunRPCStatus  ret = eRPCRunOK;
        informState = eACSInformed;
        rebootFlag = NOREBOOT;
    acsState.fault = 0;
    if (rpcAction!=NULL) {
                printf("runRPC: received rpcMethod=%d\n", rpcAction->rpcMethod); 
        switch (rpcAction->rpcMethod) {
        case rpcGetRPCMethods:
            rspBuf = doGetRPCMethods(rpcAction);
            break;
        case rpcSetParameterValues:
            rspBuf = doSetParameterValues(rpcAction);
#ifndef TESTBOX
#if 0
            if (!acsState.fault)
                BcmCfm_apply();         // perform the save and live action for wl (for now)
#endif
#endif
            break;
        case rpcGetParameterValues:
            rspBuf = doGetParameterValues(rpcAction);
            break;
        case rpcGetParameterNames:
            rspBuf = doGetParameterNames(rpcAction);
            break;
        case rpcGetParameterAttributes:
            rspBuf = doGetParameterAttributes(rpcAction);
            break;
        case rpcSetParameterAttributes:
            rspBuf = doSetParameterAttributes(rpcAction);
            break;
        case rpcAddObject:
            rspBuf = doAddObject(rpcAction);
            break;
        case rpcDeleteObject:
            rspBuf = doDeleteObject(rpcAction);
            break;
        case rpcReboot:
            rspBuf = doRebootRPC(rpcAction);
            break;
        case rpcFactoryReset:
            rspBuf = doFactoryResetRPC(rpcAction);
            break;
        case rpcDownload:
            rspBuf = doDownload(rpcAction);
            break;
        /* adding Upload RPC to the tr69CPE */
        case rpcUpload:
            rspBuf = doUpload(rpcAction);
            break;
        case rpcInformResponse:
            acsState.retryCount = 0;            /* must have been successful */
                        if ( transferCompletePending ){
                                /* make any callbacks that were setup when RPC started */
                                rspBuf = sendTransferComplete();
                                transferCompletePending = 0;
                        } else if ( sendGETRPC ) {
                                rspBuf = sendGetRPCMethods();
                                sendGETRPC = 0;
                        } else { /* send empty message to indcate no more requests */
                                rspBuf = strdup("");                /* following a InformResponse send a NULL msg */
                                ret=eRPCRunEnd;
                        }
                        initNotification();             /* update notifications following informResponse*/
            break;
                case rpcTransferCompleteResponse:
                        rspBuf = strdup("");
                        ret=eRPCRunEnd;
                        break;
                case rpcGetRPCMethodsResponse:
                        rspBuf = strdup("");
                        ret=eRPCRunEnd;
                        break;
                case rpcFault:
                default:
                        printf("runRPC: received unknown rpcMethod\n"); 
            ret = eRPCRunFail;
            break;
        }
                if (rspBuf)
                        sendToAcs(rspBuf);
        /* if no faults then update ACS state with parameter key or command key.
        */
        if (ret!=eRPCRunFail && acsState.fault==NO_FAULT && rpcAction!=NULL ) 
        {
            /* in the case of download this must wait until the download completes*/
                        printf("###runRPC: updateKeys\n\n");
            updateKeys( rpcAction );
        }
                else
                {
                        printf("###runRPC: ret = %x\n", ret);
                        printf("###runRPC: acsState.fault = %x\n", ret);
                }
    }
        return ret;
}



typedef int (*WalkTreeCB)(TRxObjNode *node, InstanceDesc *idp, char **buf, int *bufsz, int update);

static int      walkIndex; /* number of node visits- used to id node for saved attributes */
/*
 * getNotifyParams
 * idp is NULL if not instance parameter, such as, .DeviceInfo.ProvisioningCode.
 * idp points to instanceDesc for parameter. Framework stack must be valid.
* If update flag is set then update the copy of the data with the new getTRxParam() data.
* If buf is not NULL then call writeParamValueStruct to fill buf.if notification change found 
* return 0: no notification parameter change found
* return 1: notification found change found.
*           If the parameter has an active notification flags set then activeNotifications is
*           incremented.
*/
static int getNotifyParams( TRxObjNode *n, InstanceDesc *idp, char **buf, int *bufsz, int update)
{
        char    *pValue = NULL;
        InstanceDope    *pDope,    /* pointer to Dope data chained off of parameter node*/
        **pLast;   /* pointer to lask link variable. */

        pLast = &n->instanceDope;
        if ( (pDope=n->instanceDope) ) {
                /* find dope that matches instance path */
                while ( pDope ) {
                        if ( (idp==NULL && pDope->instance==NULL)
                                 || (pDope->instance==idp && checkInstanceStackPath(pDope->instance))) {
                                /* found instance dope */
                                if (pDope->notification>NOTIFICATION_OFF) {
                                        /*fprintf(stderr, "getParam %s\n", paramName);*/
                                        if ( n->getTRxParam && n->getTRxParam(&pValue)==TRX_OK) {
                                                if (pDope->pdata && (pValue!=NULL && strcmp(pValue, pDope->pdata)==0)) {
                                                        /* no change */
                                                        free(pValue);
                                                        return 0;
                                                } else {
                                                        /* parameter changed */
                                                        #ifdef DEBUG
                                                        fprintf(stderr, "%s new>%s\n", paramName, pValue);
                                                        #endif
                                                        if (buf)
                                                                writeParamValueStruct( n, pValue, buf, bufsz);
                                                        if (update) { /* replace Dope data if update is set */
                                                                free (pDope->pdata);
                                                                pDope->pdata = pValue;
                                                        } else
                                                                free(pValue);
                                                        if (pDope->notification==ACTIVE_NOTIFICATION)
                                                                ++activeNotifications;
                                                        return 1;
                                                }
                                        } else
                                                return 0;
                                }else
                                        return 0;
                        }
                        pLast = &pDope->next;
                        pDope = pDope->next;
                }
        }
        /* This is necessary following an AddObject */
        /* didn't find idp -- add it to either *n->instancedope or to end of dope list */
        if ( (pDope = (InstanceDope *) malloc(sizeof(struct InstanceDope))) ) {
                memset(pDope,0,sizeof(struct InstanceDope));
                *pLast = pDope;
                pDope->instance = idp;
#if 0
                //#ifdef DEBUG
                //fprintf(stderr, "New InstanceDope(%d) %s\n",walkIndex, paramName);
                //#endif
#endif
        }
        return 0;
}
/*
 * Inputs       *n - node pointer to start at.
 *                      *idp - Current Instance descriptor or NULL.
 *                      update - Flag to pass to getNotifyParams to indicate it should update is copy of 
 *                                      the save parameter data.
 *                      buf - If not NULL the place to put the parameter data for notification (SOAP format).
 *                      bufsz - size of buffer for parameter message struct entries.
 * static global:
 *                      walkIndex - Should be set to zero before first call. Reflects the order in which
 *                                      nodes are visited. Used to remember a specific node for attribute storate.
 *         
* return 0: not found
*                >0: number of notify changes found
*/

static int walkTree( TRxObjNode *n, InstanceDesc *idp, WalkTreeCB callb, int update, char **buf, int *bufsz  )
{
        int             pathLth = strlen(paramName);
        int             changes = 0;
        InstanceDesc    *idpParent = idp;

        /*fprintf(stderr, "searchTree %s paramPath=%s\n", n->name, paramPath); */

    while (n && n->name) {
                pushNode(n);
                walkIndex++;
                if ( streq(n->name,instanceIDMASK)) {
                        InstanceDesc    *idp;
                        if ( (idp = n->paramAttrib.instance) ) {
                                while (idp) {
                                        if (idp->parent == idpParent) { 
                                        pushInstance(idp);
                                        sprintf(paramName, "%s%d.", paramName, idp->instanceID);
                                        if (n->objDetail)
                                                 changes += walkTree( n->objDetail, idp, callb, update, buf, bufsz);
                                        popInstance();
                                        paramName[pathLth]='\0';
                                        }
                                        idp = idp->next;
                                }
                        } else if (n->objDetail) {
                                /* walk to node past instance descriptor- primary purpose is to keep walkIndex the same */
                                /* with or without instances */
                                changes = walkTree(n->objDetail, NULL, NULL, 0, NULL, 0);
                        }
                        popNode();
                        return changes;   /* return here instance nodes donot have a null terminator*/
                } else if (n->paramAttrib.attrib.etype==tObject
                               || n->paramAttrib.attrib.etype==tInstance) {
                        sprintf(paramName, "%s%s.", paramName, n->name);
                        changes += walkTree( n->objDetail, idp, callb, update, buf, bufsz );
                } else if ( callb!=NULL) {
                        sprintf(paramName, "%s%s", paramName, n->name);
                        changes += callb(n,idp,buf,bufsz,update);
                }
        paramName[pathLth]='\0';
        ++n;  /* try next item in node list */
                popNode();
    }
    return changes;
}

void resetNotificationBuf(void)
{
        free (valueBuf);
        valueBuf = NULL;
        valueBufSz = 0;
        notifyChanges = 0;
}
int checkActiveNotifications(void)
{
        int active;
        walkIndex = activeNotifications = 0;
        paramName[0] = '\0';
        walkTree(rootDevice, NULL,getNotifyParams,0, NULL, NULL);
        active = activeNotifications;
        activeNotifications = 0;
        #ifdef DEBUG
        fprintf(stderr, "checkActiveNotificaton active = %d\n", active);
        #endif
        return active;
}
/*
* Get all notifications before inform.
* do not update dope data until inform response has been received.
*/
int getAllNotifications(void)
{
        char *ps;
    walkIndex = notifyChanges = activeNotifications = 0;
        if (valueBuf==NULL)
        valueBuf = (char *)malloc(PARAMLISTSZ);
        ps = valueBuf;
        valueBufSz = PARAMLISTSZ;
        paramName[0] = '\0';
        notifyChanges = walkTree(rootDevice,NULL, getNotifyParams, 0/* no updatecopies*/, &ps, &valueBufSz);
        #ifdef DEBUG
        fprintf(stderr, "getAllNotificaton notifyChanges = %d\n", notifyChanges);
        #endif
        return notifyChanges;
}
/*
* called on start up, following InformResponse and whenever the ACS sets an Attribute
 * , adds or delete objects or sets parameter values.
*/
void initNotification(void)
{
        walkIndex = activeNotifications = notifyChanges = 0;
        paramName[0] = '\0';
        clearStacks();
        walkTree(rootDevice, NULL, getNotifyParams, 1/*update*/, NULL, NULL);
}



/* callback to save node attributes in save buffer */
static int saveNotifyAttr( TRxObjNode *n, InstanceDesc *idp, char **buf, int *bufsz, int update)
{
        /* some recasting needed for pointers */
        AttEntry        *ap = *(AttEntry **)buf;
        InstanceDope *pDope = findDopeInstance(n);
        
        BSTD_UNUSED(update);

        if (pDope) {
                if ( (pDope->notification > NOTIFICATION_OFF 
                          && *bufsz>(int)sizeof(struct AttEntry)) ){
                        /* we only care about parameter with notification values */
                        ap->nodeIndex = walkIndex;;
                        ap->instanceId = idp==NULL? 0: idp->instanceID;
                        ap->attrValue = pDope->notification;
                        *buf += sizeof(struct AttEntry);
                        *bufsz -= sizeof(struct AttEntry);
                        #ifdef DEBUG
                        fprintf(stderr, "svAttr %s at %d\n",paramName, ap->nodeIndex);
                        #endif
                        return 1;
                }
        } 
        return 0;
}
/*
* callback to restore Notify attributes:
 * The walkTree call this function for each node. This function scans the 
 * retrieved notify table to attempt to match the walkIndex to a AttEntry. If
 * an entry matches the attributes are applied to the node and a 1 is returned
 * as the function value. If no indexs match a 0 is returned.
 */
static int restoreNodeNotifyAttr( TRxObjNode *n, InstanceDesc *idp, char **buf, int *bufsz, int update)
{
        int i;
        /* some recasting needed for pointers */
        AttSaveBuf      *ap = *(AttSaveBuf **)buf;
        InstanceDope *pDope = findDopeInstance(n);

        BSTD_UNUSED(bufsz);
        BSTD_UNUSED(update);

        for (i=0; i<ap->numAttSaved; ++i) {
                AttEntry *e = &ap->attEntry[i];
                if ( walkIndex == e->nodeIndex) {
                        if (!pDope ) {
                                fprintf(stderr, "Notification initialization error- no InstanceDope for %s.%d\n",
                                                 n->name, idp?idp->instanceID:0);
                                return 0;
                        }
                        if (n->paramAttrib.attrib.etype == tString
                                || n->paramAttrib.attrib.etype == tInt
                                || n->paramAttrib.attrib.etype == tUnsigned
                                || n->paramAttrib.attrib.etype == tBool
                                || n->paramAttrib.attrib.etype == tDateTime
                                || n->paramAttrib.attrib.etype == tBase64 ) {
                                /* passed simple validation */
                                pDope->notification = e->attrValue;
                                #ifdef DEBUG
                                fprintf(stderr, "rdAttr %s %d=%d\n",paramName, e->nodeIndex, e->attrValue);
                                #endif
                                return 1;
                        } else
                                slog(LOG_ERROR, "Unable to set notification attribute for %s\n", paramName);
                        return 0;
                }
        }
        return 0;
}

int     restoreNotificationAttr(void)
{
        AttSaveBuf      *ap;
        int                     bufSz;
        int                     attRestored;

        if (tr69RetrieveFromStore(&ap, &bufSz)) {
                #ifdef DEBUG
                {
                        int i;
                        for (i=0; i<ap->numAttSaved; ++i) {
                                fprintf(stderr, "nodeIndex[%d] = %d\n", i, ap->attEntry[i].nodeIndex);
                        }
                }
                #endif
        paramName[0] = '\0';
                walkIndex = 0;
                attRestored = walkTree(rootDevice,NULL, restoreNodeNotifyAttr, 0,(char **)&ap, &bufSz);
                free(ap);
                fprintf(stderr, "Parameter Attributes restored from scratch pad = %d\n", attRestored);
                return 1;
        }
        slog(LOG_ERROR, "Stored Parameter Attribute data is corrupt or missing");
        return 0;
}
/* 
*/
void saveNotificationAttributes(void)
{
        void *buf;
        AttSaveBuf *ap;
        char *ps;
        int             bufSz;

        if ( saveNotificationsFlag) {
                saveNotificationsFlag = 0;
                if ( (buf = (char *)malloc(ATTRIBUTESAVESZ))){
                        ap = (AttSaveBuf *)buf;
                        ps = (char *)&ap->attEntry;
                        bufSz = ATTRIBUTESAVESZ-sizeof(struct AttSaveBuf);
                        paramName[0] = '\0';
                        walkIndex = 0;
                        clearStacks();
                        ap->numAttSaved = walkTree(rootDevice,NULL, saveNotifyAttr, 1, &ps, &bufSz);
                        tr69SaveToStore(ap);
                        free (buf);
                }
        }
        return;
}

