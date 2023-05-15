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
 *----------------------------------------------------------------------*/

#ifndef _WGET_H_
#define _WGET_H_

/*----------------------------------------------------------------------*/
typedef enum {
  iWgetStatus_Ok = 0,
  iWgetStatus_InternalError,
  iWgetStatus_ConnectionError,
  iWgetStatus_Error,
  iWgetStatus_HttpError
} tWgetStatus;

typedef struct {
  tWgetStatus status;
  tProtoCtx *pc;
  tHttpHdrs *hdrs;
  const char *msg;  /* Msg associated with status */
  void *handle;
} tWget;

typedef enum {
    eCloseConnection=0,
    eKeepConnectionOpen  /* used by wConnect and wClose */
} tConnState;
typedef enum {
    eUndefined,
    eConnect,
    ePostData,
    eGetData,
    ePutData,
    eDisconnect
} tRequest;

typedef struct XtraPostHdr{
    struct XtraPostHdr *next;
    char    *hdr;   /* header string */
    char    *value; /* value string*/
} XtraPostHdr;

typedef struct {
    tConnState  keepConnection;
    int         status;
    tRequest    request;
    int         cbActive; /* set to 1 if callback from report status */
    tProtoCtx *pc;
    EventHandler cb;
    void *handle;
    char *proto;
    char *host;
    tIpAddr host_addr;
    int port;
    char *uri;
    tHttpHdrs *hdrs;
    CookieHdr      *cookieHdrs;
    XtraPostHdr    *xtraPostHdrs;
    char *content_type;
    char *postdata;
    int  datalen;
} tWgetInternal;
/*----------------------------------------------------------------------*
 * returns
 *   0 if sending request succeded
 *  -1 on URL syntax error
 *
 * The argument to the callback is of type (tWget *)
 */
int wget_GetData(tWgetInternal *wg,EventHandler callback, void *handle);
int wget_Get(const char *url, EventHandler callback, void *handle);
int wget_Post(const char *url, char *arg_keys[], char *arg_values[], EventHandler callback, void *handle);
//int wget_Put(const char *url, char *arg_keys[], char *arg_values[], EventHandler callback, void *handle);
int wget_PostRaw(const char *url, char *content_type, char *data, int len, EventHandler callback, void *handle);
int wget_GetFile(const char *url, const char *filename, EventHandler callback, void *handle);
int wget_PostXmlFile(const char *url, char *data, int len, const char *filename, EventHandler callback, void *handle);
int wget_PutFile(const char *url, char *data, int len, const char *filename, EventHandler callback, void *handle);
tWgetInternal *wget_Connect(const char *url, EventHandler callback, void *handle);
int wget_PostData(tWgetInternal *,char *data, int datalen, char *contenttype, EventHandler callback, void *handle);
int wget_PutData(tWgetInternal *,char *data, int datalen, char *contenttype, EventHandler callback, void *handle);
int wget_PostDataClose(tWgetInternal *,char *data, int datalen, char *contenttype, EventHandler callback, void *handle);
int wget_Disconnect(tWgetInternal *);
const char *wget_LastErrorMsg(void);

int wget_AddPostHdr( tWgetInternal *wio, char *xhdrname, char *value);
void wget_ClearPostHdrs( tWgetInternal *wio);
#endif

