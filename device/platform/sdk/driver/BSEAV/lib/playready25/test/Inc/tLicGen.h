/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _tLicGen_h
#define _tLicGen_h

#include "PKTestNameSpace.h"

#include "drmcommon.h"
#include "drmlicense.h"
#include "drmxmr.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT TestLicResponseSetClientID(const CLIENTID *pClientID);
DRM_RESULT tDRM_B64_Encode(const DRM_BYTE *pbData, DRM_DWORD cbData, DRM_CONST_STRING *pszEncode);
DRM_RESULT tDRM_B64_Decode(DRM_CONST_STRING *szEncoded, DRM_DWORD *pcbDecode, DRM_BYTE **ppbDecode);

DRM_RESULT TestLicGenInit(DRM_DWORD dwLicMaxLength);
DRM_RESULT TestLicGenShutDown();

/*    setWhat values:
        "CONTENT_OWNER_PUB_KEY",
        "CONTENT_OWNER_SIGNINGKEY",
        "REVOKED_CONTENT_PUB_KEY",
        "REVOKED_CONTENT_SIGNINGKEY",
        "LICENSE_SERVER_PUB_KEY",
        "LICENSE_SERVER_SIGNINGKEY",
        "LICENSE_SERVER_CERT_ROOT",
        "LICENSE_SERVER_CERT_LS",
        "LICENSE_SERVER_CERT_LS0",
        "LICENSE_SERVER_KEYSEED",
        "LICENSE_KID",
        "LICENSE_LID",
        "LICENSE_UPLINK_KID",
        "DEVICE_REVOCATION_LIST_INDEX"

    szKey: the key string value

    szKeyName: the key names from testcerts.txt:
        PUBKEY,
        PRVKEY,
        CERT0,
        CERT1,
        CERT2,
        EXPIREDCERT0,
        EXPIREDCERT1,
        EXPIREDCERT2,
        EXPIREDCERTLS,
        EXPIREDCERTROOT,
*/
DRM_RESULT TestLicGenSetKeys(const char *setWhat, const char *szKey);
DRM_RESULT TestLicGenSetKeysByName(const char *setWhat, const char *szKeyName);

#define OP_LICGEN_UNKNOWN_HASH_ALGORITHM 0x1
#define OP_LICGEN_UNKNOWN_SIGN_ALGORITHM 0x2
#define OP_LICGEN_TAMPER_DATA 0x3
#define OP_LICGEN_MISSING_SIGNATURE 0x4
#define OP_LICGEN_MISSING_SIGNATURE_VALUE 0x5
#define OP_LICGEN_EMPTY_SIGNATURE 0x6
#define OP_LICGEN_EMPTY_SIGNATURE_VALUE 0x7
#define OP_LICGEN_MISSING_HASH_ALGORITHM 0x8
#define OP_LICGEN_MISSING_SIGN_ALGORITHM 0x9

#define OP_LICGEN_SIGNATURE_TEST_OPTION_MASK 0xf

#define OP_LICGEN_TEST_RESTORE_SIGNATURE 0x10
#define OP_LICGEN_TEST_CONTENT_REVOCATION_SIGNATURE 0x20

#define OP_LICGEN_NO_DEFAULT_RIGHTS 0x40
#define OP_LICGEN_RESTORE 0x80
#define OP_LICGEN_EMPTY_RESTOREINFO 0x100
#define OP_LICGEN_NO_RESTORE_LID 0x200
#define OP_LICGEN_TAMPER_RESTORE_LID 0x400
#define OP_LICGEN_NO_LICENSE_PRIORITY 0x800
#define OP_LICGEN_CONTENT_REVOCATION_NO_CONTENTPUBKEY 0x1000
#define OP_LICGEN_CONTENT_REVOCATION_MULTIPLE 0x2000
#define OP_LICGEN_NO_CONTENT_REVOCATION 0x4000
#define OP_LICGEN_SYM_OPTIMIZED 0x8000

extern const char *szLicGenDefaultKID;

typedef enum {
    MAKE_SIMPLE_LICENSE = 0,
    MAKE_LEAF_LICENSE,
    MAKE_ROOT_LICENSE
} ELicenseType;

DRM_RESULT TestLicGenMakeLicense(
    DRM_DWORD dwOptions,
    const char *szExtraAttributes,
    const char *szExtraAttributesFile,
    ELicenseType eLicenseType,
    DRM_CONST_STRING *pszLic,
    DRM_ANSI_STRING *pdastrSLK,
    DRM_BOOL fNoSymVal
    );

/* Decrypt the client id blob using the server private key. This is what the license server does
    to get the client id from the encrypted license request. */
DRM_RESULT tDecryptClientID(DRM_VOID *pCryptoContext, CLIENTID *pClientID);

/*
    Obtain keys and test certificates from a text file in the current directory.
    All test apps should use this function to get test certs instead of hardcoding them.
    The calling app should free the returning pointer. Returns NULL in error.

    Valid szGetWhat values:
        "CONTENTPUBKEY", "CONTENTPRVKEY", "LSPUBKEY", "LSPRVKEY", "CERT0", "CERT1", "CERT2",
        "EXPIREDCERTLS", "EXPIREDCERTROOT",
        "EXPIREDCERT0", "EXPIREDCERT1", "EXPIREDCERT2"
*/
char* GetTestCert(const char *szGetWhat);


EXIT_PKTEST_NAMESPACE

#endif
