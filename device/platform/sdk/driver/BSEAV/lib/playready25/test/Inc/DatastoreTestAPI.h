/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DATASTORETESTAPI_H
#define __DATASTORETESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define MAX_NAMESPACE_NUM    5

/*
** Shared Variable Names
*/
#define DST_VAR                   "SharedDST"
#define DST_CONTEXT_VAR           "SharedDSTContext"
#define DST_NAMESPACE_CONTEXT_VAR "SharedDSTNamespaceContext"
#define DST_ENUM_CONTEXT_VAR      "SharedDSTEnumContext"
#define DST_SLOT_CONTEXT_VAR      "SharedDSSlotContext"
#define DST_SLOT_HINT_VAR         "SharedDSTSlotHint"
#define DST_PBSTOREDATA_VAR       "SharedDSTPBStoreData"
#define DST_PCBSTOREDATA_VAR      "SharedDSTPCBStoreData"

DRM_RESULT DRM_CALL TestDatastoreInit                   ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreUninit                 ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCreateStore            ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreOpenStore              ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCloseStore             ( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL TestDatastoreCommitStore            ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCleanupStore           ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreOpenNamespace          ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCloseNamespace         ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreDeleteNamespace        ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCommitNamespace        ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreOpenSlot               ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreMakeSlotHint           ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCloseSlot              ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreDeleteSlot             ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreInitSlotEnum           ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotEnumNext           ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotEnumReloadCurrent  ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotEnumDeleteCurrent  ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotResize             ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotSeek               ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotRead               ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreSlotWrite              ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDatastoreCheckESTDirtyBit       ( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( DATASTORE )

EXIT_PKTEST_NAMESPACE

#endif /* __DATASTORETESTAPI_H */
