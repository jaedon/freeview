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

DRM_RESULT DRM_CALL TestDRM_DOMST_OpenStore(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CloseStore(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_AddKey(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_GetKey(IN long argc, IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteKey(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteKeys(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareKey(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_InitEnumKey(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_EnumNextKey(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_AddCert(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_GetCert(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_DeleteCert(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareCert(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_CompareCerts(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_InitEnumCert(IN long argc, IN __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRM_DOMST_EnumNextCert(IN long argc, IN __in_ecount(argc) char **argv);


DECLARE_MODULE_DEFS(DomainStore)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_DOMAINSTORE_TEST_API_H */
