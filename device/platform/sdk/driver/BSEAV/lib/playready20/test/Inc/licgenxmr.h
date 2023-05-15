/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __LICGENXMR_H
#define __LICGENXMR_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define SLK_DATA_SIZE_IN_BYTES  128
#define SLK_DATA_SIZE_IN_CHARS  CCH_BASE64_EQUIV( SLK_DATA_SIZE_IN_BYTES )

#define SLK_ID_SIZE_IN_BYTES    16
#define SLK_ID_SIZE_IN_CHARS    CCH_BASE64_EQUIV( SLK_ID_SIZE_IN_BYTES )

#define DRM_COCKTAIL_PADDED_KEYLEN 16

#define AES_KEY_VAR                 "SharedAESKey"
#define RC4_KEY_VAR                 "SharedRC4Key"

#define CONTENT_ID_VAR              "SharedContentID"
#define LICENSE_NONCE_VAR           "SharedLicenseNonce"
#define LICENSE_ID_VAR              "SharedLicenseID"
#define LICENSE_REMOVAL_DATE_VAR    "SharedLicenseRemovalDate"

#define RANDOM_KID_VAR              "SharedRandomKeyID"
#define RESPONSE_SYMM_OPT           "OptimizeResponse"
#define RESPONSE_USE_SLK            "UseSlk"
#define RESPONSE_V3_SLK             "SymmetricKey"
#define RESPONSE_V3_SLK_ID          "SymmetricKeyID"
#define RESPONSE_V3_SLK_ENCRYPTED   "SymmetricKeyEncrypted"
#define RESPONSE_XPOL_RIGHT_VAR     "Resp_ExtensibleRight"
#define RESPONSE_XPOL_RESTRICTION_VAR   "Resp_ExtensibleRestriction"

typedef struct
{
    DRM_BOOL                 fDuplicateAllowed;
    DRM_BYTE                 fSecureStateType;
    DRM_WORD                 wObjectType;
    DRM_WORD                 wParent;
    DRM_WORD                 wFlags;
    DRM_WORD                 cStateDateCountorBinary;
    DRM_DWORD                cbAllData;
    DRM_DWORD                cbXPolData;
    DRM_VOID                *pStateDateCountorBinary;
    DRM_BYTE                *pbXPolData;
} DRM_TEST_XPOL_DATA;

#define CallHeaderBuilder(context, fn)           \
    do                                           \
    {                                            \
        dr = (fn);                               \
        if ( dr == DRM_E_BUFFERTOOSMALL )        \
        {                                        \
            ChkDR(Test_ReallocHBContext(&(context)));\
            ChkDR((fn));                         \
        }                                        \
        else                                     \
        {                                        \
            ChkDR(dr);                           \
        }                                        \
    }while(FALSE)

DRM_RESULT DRM_CALL Test_ReallocHBContext( DRM_BYTE **ppbHBContext );

DRM_RESULT DRM_CALL Test_BuildV3LicenseResponse(
    IN DRM_BB_CONTEXT        *f_poBBXContext,
    IN DRM_ANSI_CONST_STRING  f_rgdastrXMRLic[],
    IN DRM_DWORD              f_cXMRLic,
    IN DRM_ANSI_CONST_STRING *f_pdastrRevInfo,
    IN DRM_ANSI_CONST_STRING *f_pdastrCustomData,
    IN DRM_ANSI_CONST_STRING *f_pdastrTransactionID,
    OUT DRM_BYTE             *f_pbXMRLicResp,
    IN OUT DRM_DWORD         *f_pcbXMRLicResp );

DRM_RESULT DRM_CALL Test_CreateXMRLicense( 
    __deref_out_ecount( *f_pcbLicense ) DRM_BYTE **f_ppbLicense,
    __out DRM_DWORD *f_pcbLicense,
    __in DRM_BOOL f_fLog, 
    long argc, 
    __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL Test_InitLicGenXMR( void );

EXIT_PKTEST_NAMESPACE

#endif /* __LICGENXMR_H */
