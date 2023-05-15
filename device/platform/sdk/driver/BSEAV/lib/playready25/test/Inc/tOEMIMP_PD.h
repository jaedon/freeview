/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __tOEMIMP_PD_H
#define __tOEMIMP_PD_H

#include <drmcommon.h>
#include <drmcontextsizes.h>
#include <tNetHeaders.h>

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE;

typedef DRM_RESULT (DRM_CALL *PFNPROGRESSCALLBACK)(DRM_DWORD f_dwTotal, DRM_DWORD f_dwCurrent, DRM_CONST_STRING *f_pdstrMessage );

DRM_RESULT DRM_CALL tOEM_SendNetDataDSTR(
    __in                           const DRM_CONST_STRING     *f_pdstrURL,
    __in                                 eDRM_TEST_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )         DRM_BYTE             *f_pbChallenge,
    __in                                 DRM_DWORD             f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse ) DRM_BYTE            **f_ppbResponse,
    __out_ecount( 1 )                    DRM_DWORD            *f_pcbResponse);

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Function signature must match across various implementations");
DRM_RESULT DRM_CALL tOEM_SendNetData(
    __in_z                               DRM_CHAR             *f_pszUrl,
    __in                                 eDRM_TEST_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )         DRM_BYTE             *f_pbChallenge,
    __in                                 DRM_DWORD             f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse ) DRM_BYTE            **f_ppbResponse,
    __out_ecount( 1 )                    DRM_DWORD            *f_pcbResponse);
PREFAST_POP /* Function signature must match across various implementations */

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Function signature must match across various implementations");
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Function signature must match across various implementations");
DRM_RESULT DRM_CALL tOEM_NetSaveFile(
    __in_z                               DRM_CHAR             *f_pszUrl,
    __in                                 DRM_CONST_STRING     *f_pdstrFile,
    __in_opt                             PFNPROGRESSCALLBACK   f_fnProgress);
PREFAST_POP /* Function signature must match across various implementations */
PREFAST_POP /* Function signature must match across various implementations */


DRM_RESULT DRM_CALL tOEM_NetSaveFileDSTR(
    __in                           const DRM_CONST_STRING     *f_pdstrURL,
    __in                                 DRM_CONST_STRING     *f_pdstrFile,
    __in_opt                             PFNPROGRESSCALLBACK   f_fnProgress);


DRM_RESULT DRM_CALL SetDeviceEnv(
    const DRM_WCHAR  *f_wszDevcertTemplateFileName,
    const DRM_WCHAR  *f_wszPrvKeyFileName,
    const DRM_WCHAR  *f_wszGroupCertFileName,
    const DRM_WCHAR  *f_wszGroupCertPrvKeyFileName,
          DRM_BOOL    f_fCreateDevCert );

DRM_RESULT DRM_CALL InitWithOemData(__in_opt DRM_VOID *f_pOEMContext);

DRM_RESULT DRM_CALL SetNDREnv(
    const DRM_WCHAR  *f_wszNDRCertTemplateFileName,
    const DRM_WCHAR  *f_wszNDRGroupPrvKeyFileName,
          DRM_BOOL    f_fCreateDevCert );

/* Add options later. Now just set machine time to the secure store as secure clock.
*  Refresh date is one-month later.
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Function signature must match across various implementations");
DRM_RESULT DRM_CALL tSetSecureClock(DRM_DST* poDST, DRM_BYTE* rgbSSTPassword);
PREFAST_POP /* Function signature must match across various implementations */

DRM_RESULT DRM_CALL tGetBBContext(DRM_BB_CONTEXT **ppBB);
void DRM_CALL tFreeBBContext(DRM_BB_CONTEXT *pBB);
void DRM_CALL tGetBBVersion(
    const DRM_BB_CONTEXT *pBB,
          DRM_BYTE       *pBBVer );

EXIT_PKTEST_NAMESPACE;

#endif /* __tOEMIMP_PD_H */
