/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PRNDNOASSETTXTEST_H
#define __DRM_PRNDNOASSETTXTEST_H

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE;

/*
**  Success path message types
*/
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_REQUEST      1
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE     2
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_START                  3
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_CHLG                   4
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_RESP                   5
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_PD_RSLT                   6
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_LICENSE_REQUEST           7
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_LICENSE_RESPONSE          8

/*
**  Error path message types
*/
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_REQUEST_EXPIRED_REVINFO      9
#define PRND_NO_ASSET_TX_MESSAGE_TYPE_REGISTRATION_RESPONSE_FAILURE            10

#define PRND_NO_ASSET_TX_CLEAR_CONTENT_STRING   "Hello, world!!!"

/*
** ---------------------------------------
** Test PRND No Asset Tx Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_StartTx( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_EndTx( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_PerformRegistration( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_PerformProximityDetection( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDNoAssetTx_PerformLicenseFetch( long argc, __in_ecount( argc ) char **argv );

#if !SKIP_DECLARE_MODULE_DEFS
DECLARE_MODULE_DEFS(prndnoassettxtest)
#endif  /* !SKIP_DECLARE_MODULE_DEFS */

EXIT_PKTEST_NAMESPACE;

#endif /* __DRM_PRNDNOASSETTXTEST_H */

