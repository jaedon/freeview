/*
 * RootClass.h
 */

#ifndef __ROOTCLASS_H__
#define __ROOTCLASS_H__

#if !defined(WIN32)
#include <stdbool.h>
#endif

#include "ISO13522-MHEG-5.h"

void RootClass_dup(RootClass *, RootClass *);

void RootClass_registerObject(RootClass *);
void RootClass_registerClonedObject(LIST_TYPE(GroupItem) *, RootClass *, unsigned int, unsigned int);
void RootClass_unregisterObject(RootClass *);

void RootClass_Preparation(RootClass *);
void RootClass_Activation(RootClass *);
void RootClass_Deactivation(RootClass *);
void RootClass_Destruction(RootClass *);

void RootClass_GetAvailabilityStatus(RootClass *, ObjectReference *, OctetString *);
void RootClass_GetRunningStatus(RootClass *, ObjectReference *, OctetString *);

void RootClass_contentAvailable(RootClass *, OctetString *);

#endif	/* __ROOTCLASS_H__ */

