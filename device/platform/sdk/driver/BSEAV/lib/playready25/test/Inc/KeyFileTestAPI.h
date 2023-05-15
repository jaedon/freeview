/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __KEYFILETEST_H
#define __KEYFILETEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

typedef struct _tagTST_DWORD_VALUE_MAP_ENTRY
{
    DRM_ANSI_CONST_STRING dastrName;
    DRM_DWORD             dwValue;
} TST_DWORD_VALUE_MAP_ENTRY;

#define DECLARE_DWORD_MAP_ENTRY(x) { CREATE_DRM_ANSI_STRING(#x), (DRM_DWORD) x }
#define EMPTY_DWORD_MAP_ENTRY      { EMPTY_DRM_STRING, 0 }

DRM_RESULT DRM_CALL TST_MAP_GetDWORD( const TST_DWORD_VALUE_MAP_ENTRY *f_pMap,
                                      const DRM_ANSI_CONST_STRING     *f_pdastrSearch,
                                            DRM_DWORD                 *f_pdwValue );

DRM_RESULT DRM_CALL Test_KF_CreateContext(       long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_FreeContext(         long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_Initialize(          long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_Uninitialize(        long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_OpenKeyFile(         long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_CloseKeyFile(        long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_LoadKeyFile(         long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_StoreKeyFile(        long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_SetCertificate(      long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_GetCertificate(      long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_SetPrivateKey(       long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_KF_GetPrivateKey(       long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_Oem_KeyFile_EncryptKey( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_Oem_KeyFile_DecryptKey( long argc, __in_ecount( argc ) char **argv );


DECLARE_MODULE_DEFS( KeyFile )

EXIT_PKTEST_NAMESPACE

#endif /* __KEYFILETEST_H */
