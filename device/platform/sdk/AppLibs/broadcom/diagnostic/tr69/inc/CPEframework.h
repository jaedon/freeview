#ifndef CPRFRAMEWORK_H
#define  CPRFRAMEWORK_H

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
 * File Name  : CPEframework.h
 *
 * Description: CPE table access/mgmt functions                                                   $
 *----------------------------------------------------------------------*/
#include "../inc/tr69cdefs.h"

#define MAX_DEPTH   20      /* maximum nodes in parameter tree path */
extern const char instanceIDMASK[];
extern TRxObjNode  rootDevice[];


const char *getValTypeStr(eTRxType etype);
int isWriteable(TRxObjNode *n);
TRxObjNode  *findGWParameter(const char *pstr);
TRxObjNode  *getCurrentNode(void);
int         getCurrentInstanceID(void);
int         getInstanceCount( TRxObjNode *n);
int         getInstanceCountNoPathCheck( TRxObjNode *n);
InstanceDesc *getCurrentInstanceDesc(void);
InstanceDesc  *getHigherInstanceDesc(int level);
InstanceDesc *findInstanceDesc( TRxObjNode *n, int id);
InstanceDesc *findInstanceDescNoPathCheck( TRxObjNode *n, int id);
InstanceDesc *getNewInstanceDesc( TRxObjNode *n, InstanceDesc *parent, int id);
InstanceDope *findDopeInstance( TRxObjNode *n );
int deleteCurrentInstanceDesc(void);
int deleteInstanceDescNoPathCheck( TRxObjNode *n, int id);
int deleteInstanceDesc( TRxObjNode *n, int id);
int checkInstancePath(InstanceDesc *idp);
int checkInstanceStackPath(InstanceDesc *idp);

void pushNode(TRxObjNode *n);
void popNode(void);
void pushInstance(InstanceDesc *idp);
void popInstance(void);
void replaceInstanceTop(InstanceDesc *idp);
void clearInstanceStack(void);
void clearNodeStack(void);
void clearStacks(void);

char *buildInterfaceNameParamPath(char *ifname);
char *buildExternalConnParamName(int extIP);

TRX_STATUS getGWParameterValue( const char *pstr, char **value);




#endif /* CPRFRAMEWORK_H */
