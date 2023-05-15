/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
//#if defined OIPF && defined JLABS
#if defined OIPF 

#ifndef __NATIVE_JLABS_CDSSEARCHER
#define __NATIVE_JLABS_CDSSEARCHER

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsCDSSearcherHandle;
/* Methods */
OOIFReturnCode native_jlabsDMPAction( int actionType, OOIFNumber *retval );
OOIFReturnCode native_jlabsCDSSearchGetAvailableDMSList( OOIFNumber *retval );

OOIFReturnCode native_jlabsCDSSearchRegisterEventHandler(void  (cbFunc)(int event, int size, void *data) );
OOIFReturnCode native_jlabsCDSSearchUnRegisterEventHandler(void);

OOIFReturnCode native_jlabsCDSSearch( jlabsCDSSearcherHandle *dmslist, int *pDMS_cnt, int nTimeout );
OOIFReturnCode native_jlabsGetDMSList(unsigned int *count, void **Handles);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 

#endif /* __NATIVE_JLABS_CDSSEARCHER */
