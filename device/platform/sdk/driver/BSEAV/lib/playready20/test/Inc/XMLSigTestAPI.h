/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_XMLSIGTEST_H_
#define _DRM_XMLSIGTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** XMLSig Test Global Variable Names
** ---------------------------------------
*/
#define XMLSIG_BBXCTX_VAR "XMLSig_BBXCTX_Var"

/*
** ---------------------------------------
** XMLSig Non-APIMAP Functions 
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLSig APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestDRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL  TestDRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_GetSignatureNodeCharCount(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_GetEncryptedDataNodeCharCount(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyName(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyValue(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildSignatureNode(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildEncryptedDataNode(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_ExtractSimpleECC256Key(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDRM_XMLSIG_VerifySignature(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS( XMLSig )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_XMLSIGTEST_H_ */
