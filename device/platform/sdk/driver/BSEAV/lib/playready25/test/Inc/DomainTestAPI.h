/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_DOMAIN_TEST_API_H
#define __DRM_DOMAIN_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#include <drmmanager.h>

/*
** ---------------------------------------
** Domain Test Global Variables
** ---------------------------------------
*/
extern DRM_APP_CONTEXT *m_pContextManagerDomain;

/*
** ---------------------------------------
** Domain Test API Functions
** ---------------------------------------
*/

DRM_RESULT DRM_CALL TestDrm_GenerateChallenge(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDrm_ProcessResponse(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDrm_GetAdditionalResponseData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDrm_DomainCert_Find(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDrm_DomainCert_InitEnum(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDrm_DomainCert_EnumNext(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS(Domain)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_DOMAIN_TEST_API_H */
