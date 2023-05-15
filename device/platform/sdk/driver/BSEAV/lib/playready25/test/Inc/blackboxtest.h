/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __BLACKBOXTEST_H
#define __BLACKBOXTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* globals */
extern DRM_BB_CONTEXT g_BBContext;

#define MAX_GLOBAL_BINDING_BLOB 2
extern DRM_BINDING_INFO g_binding[MAX_GLOBAL_BINDING_BLOB];

DRM_RESULT DRM_CALL BBXInitWithCerts( DRM_BB_CONTEXT *pcontextBBX );
DRM_RESULT DRM_CALL Test_CreateBindingInfo(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_CanBind(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_GetClientID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_VerifyClientID(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_GenRandomBlob(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_HashValue(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_BlobOps(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_PKEncryptLarge(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_DecryptLicense(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_CipherKeySetup(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_RenewBlackboxContext(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestBBXSymmetricCipher( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_ECCP256_Decrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_ECCP256_Sign_Verify( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDFunctions( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDRebindLicense( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestKeyUnwrapAndWrap( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_BBXCryptoHandleCaching( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_CreateAndVerifyOMAC1Signature( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_CreateAndVerifyMessageSignature( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_DecryptContent( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_HalRevocation( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_InvalidPubKeyWrapUsage( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_InvalidPubKeySignUsage( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_InvalidKeyFileUsage( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( BlackBox )

EXIT_PKTEST_NAMESPACE

#endif /* __BLACKBOXTEST_H */

