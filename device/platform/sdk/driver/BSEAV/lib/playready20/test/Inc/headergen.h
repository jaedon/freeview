/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _HEADERGEN_H
#define _HEADERGEN_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE;

extern const DRM_CONST_STRING g_dstrVersion4;
extern const DRM_CONST_STRING g_dstrVersion4_1;
extern const DRM_CONST_STRING g_dstrVersionUnknown;
extern const DRM_CONST_STRING g_dstrALGIDAESCTR;
extern const DRM_CONST_STRING g_dstrALGIDRC4;

typedef struct {
    DRM_CONST_STRING dstrWRMHeaderVersion;
    DRM_CONST_STRING dstrPlayReadySilentURL;
    DRM_CONST_STRING dstrPlayReadyNonSilentURL;
    DRM_CONST_STRING dstrServiceID;
    DRM_CONST_STRING dstrCustomAttriburesXML;
    DRM_CONST_STRING dstrDecryptorSetup;
    DRM_CONST_STRING dstrCipherType;
    DRM_CONST_STRING dstrKID;    
    DRM_CONST_STRING dstrContentKey;
    DRM_BOOL         bOverrideSet;
} TestWRMHeaderOverrides;

typedef struct _TestWRMHeaderProtectInfoParams
{
    DRM_SUPPORTED_CIPHERS                   eCipherType;
    DRM_CONST_STRING                        dstrKID;
    DRM_CONST_STRING                        dstrContentKey;
    struct _TestWRMHeaderProtectInfoParams *pNext;
} TestWRMHeaderProtectInfoParams;

typedef struct 
{
    eDRM_HEADER_VERSION             eWRMHeaderVersion;
    DRM_CONST_STRING                dstrPlayReadySilentURL;
    DRM_CONST_STRING                dstrPlayReadyNonSilentURL;
    DRM_CONST_STRING                dstrServiceID;
    DRM_CONST_STRING                dstrCustomAttriburesXML;
    DRM_CONST_STRING                dstrDecryptorSetup;
    TestWRMHeaderProtectInfoParams *pProtectInfoList;
} TestWRMHeaderParams;


/*
** There is only one special testcase TEST_PRO_NoWRMHeaderRecord 
** meaning a PRO with empty ELS store of maximum size and no WRMHeader record.
** Other variations like an empty WRMHeader record, no ELS, zero ELS, etc. 
** are not supported until required by some test scenario.
*/
#define TEST_PRO_NoSpecialTestcases 0x00000000 
#define TEST_PRO_NoWRMHeaderRecord  0x00000001 


DRM_API DRM_RESULT DRM_CALL TST_UTL_TestGenerateWRMHeader( __in                             const DRM_CONST_STRING *f_pdstrPool,
                                                           __in                             const DRM_CONST_STRING *f_pdstrId,
                                                           __inout_bcount_opt(*f_pcbHeader) DRM_BYTE               *f_pbHeader,
                                                           __out                            DRM_DWORD              *f_pcbHeader,
                                                           __in_opt                         TestWRMHeaderOverrides *f_pOverrideParams );


DRM_API DRM_RESULT DRM_CALL TST_UTL_TestGeneratePRO( __in_bcount_opt(f_cbWRMHeader) const DRM_BYTE  *f_pbWRMHeader,
                                                     __in                           const DRM_DWORD  f_cbWRMHeader,
                                                     __in                           DRM_DWORD        f_dwTestParams,
                                                     __deref_out_bcount(*f_pcbPRO)  DRM_BYTE       **f_ppbPRO,
                                                     __out                          DRM_DWORD       *f_pcbPRO );


DRM_API DRM_RESULT DRM_CALL TST_UTL_TestAddLicenseBufferToPRO( __in                          DRM_CONST_STRING *f_pdstrKID,
                                                               __in                          DRM_CONST_STRING *f_pdstrLID,
                                                               __in_bcount_opt(f_cbLicense)  const DRM_BYTE   *f_pbLicense,
                                                               __in                          const DRM_DWORD   f_cbLicense,
                                                               __inout_bcount(f_cbPROBuffer) const DRM_BYTE   *f_pbPROBuffer,
                                                               __in                          const DRM_DWORD   f_cbPROBuffer );

EXIT_PKTEST_NAMESPACE

#endif /* _HEADERGEN_H */

