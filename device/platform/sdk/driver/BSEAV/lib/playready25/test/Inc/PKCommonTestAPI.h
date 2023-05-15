/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PK_COMMON_TEST_API_H
#define __DRM_PK_COMMON_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** Redefine DEFAULT_TEST_MODULE_LIST_START so that PKCommon is always the first module loaded
*/
#ifdef DEFAULT_TEST_MODULE_LIST_START
#undef DEFAULT_TEST_MODULE_LIST_START
#define DEFAULT_TEST_MODULE_LIST_START \
    PKTEST_MODULE_ENTRY(PKCommon)
#endif

#define REVOCATION_BUFFER_VAR "SharedRevocationBuffer"

DRM_RESULT DRM_CALL TestScanDevicePrivateKey( DRM_APP_CONTEXT *pManagerContext );

DRM_RESULT DRM_CALL RestoreDeviceStore( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL SaveDeviceStore( long argc, __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestDrmInitializeWithActivation(
    __in                                    DRM_APP_CONTEXT     *f_poAppContext,
    __in                                    DRM_VOID            *f_pOEMContext,
    __in_bcount( f_cbOpaqueBuffer )         DRM_BYTE            *f_pbOpaqueBuffer,
    __in                                    DRM_DWORD            f_cbOpaqueBuffer,
    __in                              const DRM_CONST_STRING    *f_pdstrDeviceStoreName );

DRM_RESULT DRM_CALL TestDrm_Platform_Initialize();
DRM_RESULT DRM_CALL TestDrm_Platform_Uninitialize();

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
DRM_BOOL DRM_CALL LaunchDRMHelperRoutine(LPCTSTR pwszProgrm, LPCTSTR pwszParams);

#define REG_SUBKEY_WM7_MODEL_STRING    L"Software\\Microsoft\\PlayReady\\Model"
#define REG_VALUENAME_WM7_MODEL_STRING L"MSO"
#endif //endif: DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7


DECLARE_MODULE_DEFS(PKCommon)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_PK_COMMON_TEST_API_H */
