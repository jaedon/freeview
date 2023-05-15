/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_DOMAINSTORE_TEST_API_H
#define __DRM_DOMAINSTORE_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#include <drmmanager.h>

/*
** ---------------------------------------
** Domain Test Global Variables
** ---------------------------------------
*/
extern DRM_APP_CONTEXT *m_pContextManagerDomainStore;

/*
** ---------------------------------------
** Domain Store Test API Functions
** ---------------------------------------
*/

DRM_RESULT DRM_CALL TestDRM_DOMST_OpenStore(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CloseStore(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_AddKey(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_GetKey(__in long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteKey(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteKeys(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareKey(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_InitEnumKey(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_EnumNextKey(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_AddCert(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_GetCert(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteCert(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareCert(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareCerts(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_InitEnumCert(__in long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_EnumNextCert(__in long argc, __in_ecount(argc) char **argv);


DECLARE_MODULE_DEFS(DomainStore)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_DOMAINSTORE_TEST_API_H */
