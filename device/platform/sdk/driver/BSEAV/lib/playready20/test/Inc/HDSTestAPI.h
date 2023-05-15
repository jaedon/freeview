/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __HDSTEST_H
#define __HDSTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define MAX_NAMESPACE_NUM    5 

/*
** Shared Variable Names
*/
#define HDS_CONTEXT_VAR "SharedHDSContext"
#define HDS_NAMESPACE_CONTEXT_VAR "SharedHDSNamespaceContext"
#define HDS_ENUM_CONTEXT_VAR "SharedHDSEnumContext"
#define HDS_SLOT_CONTEXT_VAR "SharedHDSSlotContext"
#define HDS_SLOT_HINT_VAR "SharedHDSSlotHint"
#define HDS_QUERY_CONTEXT_VAR "SharedHDSQueryContext"
#define HDS_NAME_VAR "SharedHDSName"
 
DRM_RESULT DRM_CALL TestHdsCreateStore(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsOpenStore(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsCloseStore(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsDeleteNamespace(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsOpenNamespace(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsCloseNamespace(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsCommitStore(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsInitSlotEnum(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsSlotEnumNext(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsOpenSlot(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsCloseSlot(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsDeleteSlot(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsSlotResize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsSlotSeek(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsSlotRead(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsSlotWrite(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsReinitialize(long argc, __in_ecount(argc) char **argv);
  
DRM_RESULT DRM_CALL TestHdsCreateStore2(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsOpenStore2(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestHdsBlockScanDelete(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestDeleteFile(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCleanupStore(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCommitNamespace(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestMakeSlotHint(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestCheckpoint(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSlotEnumReloadCurrent(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSlotEnumDeleteCurrent(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( HDS )

EXIT_PKTEST_NAMESPACE

#endif /* __HDSTEST_H */
