/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __V3MeteringTestHelpers_H__
#define __V3MeteringTestHelpers_H__

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL TST_MTR_VerifyChallengeData(
                         __in_bcount(f_cbChallenge) const DRM_BYTE  *f_pbChallenge,
                         __in                       const DRM_DWORD  f_cbChallenge );

DRM_RESULT DRM_CALL TST_MTR_DecryptV3ChallengeData(
    __in_bcount(f_cbChallenge)                  const DRM_BYTE     *f_pbChallenge,
    __in                                        const DRM_DWORD     f_cbChallenge,
    __in                                              PRIVKEY_P256 *f_pPrivKey,
    __deref_out_bcount_opt(*f_pcbDecryptedData)       DRM_BYTE    **f_ppbDecryptedData,
    __inout                                           DRM_DWORD    *f_pcbDecryptedData,
    __out_opt                                         DRM_BYTE     *f_pbKey );

DRM_RESULT DRM_CALL TST_MTR_ValidateV3Metering(
    __in                       const DRM_WCHAR *f_pwszPrivKey,
    __in                       const DRM_DWORD  f_cchPrivKey,
    __in_bcount(f_cbChallenge)       DRM_BYTE  *f_pbChallenge,
    __in                       const DRM_DWORD  f_cbChallenge,
    __in                             long       argc,
    __in_ecount(argc)                char     **argv );

DRM_RESULT DRM_CALL TST_HLP_GetKidFromFile(
                                    __in_z  DRM_CHAR        *f_pszFileName,
                                    __inout DRM_ANSI_STRING *f_pdastrKid );

DRM_RESULT DRM_CALL TST_MTR_GenerateV3Response(
    __in                       const DRM_WCHAR *f_pwszPrivKey,
    __in                       const DRM_DWORD  f_cchPrivKey,
    __in_bcount(f_cbChallenge)       DRM_BYTE  *f_pbChallenge,
    __in                       const DRM_DWORD  f_cbChallenge,
    __inout_bcount(f_pcbResponse)    DRM_BYTE **f_ppbResponse,
    __inout                          DRM_DWORD *f_pcbResponse );

EXIT_PKTEST_NAMESPACE

#endif __V3MeteringTestHelpers_H__
