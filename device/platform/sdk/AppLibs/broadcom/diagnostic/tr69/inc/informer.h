#ifndef INFORMER_H
#define INFORMER_H
 
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
 * File Name  : informer.h
 *
 * Description: Inform logic prototypes for tr69
 * $Revision: 1.8 $
 * $Id: informer.h,v 1.8 2005/09/19 20:19:56 dmounday Exp $
 *----------------------------------------------------------------------*/
#include "../SOAPParser/RPCState.h"

typedef enum {
    eJustBooted=0,
    eACSInformed
} eInformState;


extern eInformState     informState;

typedef void (*CallBack)(AcsStatus );
void resetPeriodicInform(unsigned long interval);
void initInformer(void);
void sendDownloadFault(void);
void sendUploadFault(void);
int sendToAcs(char *);
void initCFMListener(void);
 
#endif /*INFORMER_H*/
 
