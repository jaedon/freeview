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
 * $Revision: 1.17 $
 * $Id: bcmTestWrapper.h,v 1.17 2006/02/17 21:12:52 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h"
#ifndef uint32
#define uint32 unsigned long
#endif
/* general system status and configruation retrieval */
/* Returns state of WAN interface to be used by tr69 client */
typedef enum {
    eWAN_INACTIVE,
    eWAN_ACTIVE
} eWanState;

typedef enum {
	PARAM_CHANGE=0,
	SEND_GETRPC,
	SEND_DIAG_COMPLETE,
	SEND_ENABLE_INFORM,
	SEND_DISABLE_INFORM
} eCFMMsg;

eWanState getWanState();


eACSContactedState getACSContactedState(void);
int getNewInstanceId(void);     /* return a new unused instance id */

void initBCMLibIF(void);
void saveTR69StatusItems( ACSState *a );
void wrapperReboot(eACSContactedState rebootContactValue);
void wrapperFactoryReset(void);
int  preDownloadSetup(ACSState *acs, DownloadReq *r);
int  downloadComplete(DownloadReq *r, char *buf);
void saveACSContactedState(void);
int	tr69SaveToStore( AttSaveBuf *ap);
int tr69RetrieveFromStore(AttSaveBuf **bufp, int *size);

void BcmTr69c_CfmMsg( eCFMMsg msg);
int getRAMSize(void);
#endif /*BCMWRAPPER_H*/
