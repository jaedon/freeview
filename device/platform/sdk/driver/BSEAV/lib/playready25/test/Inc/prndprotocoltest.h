/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PRNDPROTOCOLTEST_H
#define __DRM_PRNDPROTOCOLTEST_H

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE;

/*
** ---------------------------------------
** Test PRND Protocol Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestPRNDProtocolRegistration( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDProtocolProximityDetection( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDProtocolLicenseFetch( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDCRLTimeOffset( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDRevInfoOffset( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(prndprotocoltest)

#define TEST_VAR_PRND_RXCTX      "ReceiverContext"
#define TEST_VAR_PRND_TXCTX      "TransmitterContext"
#define TEST_VAR_PROXIMITY_NONCE "PrndProximityNonce"

extern DRM_ID    g_customDataTypeID;
extern DRM_ID    g_badCustomDataTypeID;
extern DRM_BYTE  g_rgbCustomData[];
extern DRM_DWORD g_cbCustomData;

/* Helper method */
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
DRM_RESULT DRM_CALL _CertificateCallback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *f_pvCertificateCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *f_pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *f_pbCustomData,
    __in                                       DRM_DWORD                                        f_cbCustomData );
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

EXIT_PKTEST_NAMESPACE;

#endif /* __DRM_PRNDPROTOCOLTEST_H */

