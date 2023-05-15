/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_BCERTTEST_H_
#define _DRM_BCERTTEST_H_

#include <drmbcertbuilder.h>

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* Non-APIMAP Functions */
DRM_RESULT DRM_CALL _tGetSetBCChainBuffer( DRM_BYTE *pbBinCertChain, DRM_DWORD *pcbBinCertChain, DRM_BOOL fGet);
DRM_RESULT DRM_CALL _tGetSetBinCertList( DRM_BCERT_CERTIFICATE *pBinCerts, DRM_DWORD *cBinCerts, DRM_BOOL fGet);
DRM_RESULT DRM_CALL _tGetSetCurCertBuildData( DRM_BCERT_BUILDER_DATA *pData, DRM_BOOL fGet);

/* APIMAP Functions */
DRM_RESULT DRM_CALL Test_LoadBinCertChain(long argc, __in_ecount(argc) char **argv);

/* Binary Cert Builder Test Functions */
DRM_RESULT DRM_CALL Test_SetDefaultBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_ClearBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_AddKeyToBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetSecurityLevel(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetExpiration(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetPCBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetDomainBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetDeviceBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetSilverLightBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetMeteringBuilderData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetBuilderFlagData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetManufacturerInfo(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetFeatureInfo(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_AddCert(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_AddExtendedDataToCert(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_CompareCertChains(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SignAndAddCert(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetServerBuilderData(long argc, __in_ecount(argc) char **argv);

/* Binary Cert Parser Test Functions */
DRM_RESULT DRM_CALL Test_ParseChainHeader(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_ParseCertificate(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_ParseCertificateChain(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_VerifyCertFromChain(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_VerifySecurityVersionForCert(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_FindObjectInCertByType(long argc, __in_ecount(argc) char **argv);

/* ND cert functions */
DRM_RESULT DRM_CALL Test_CreateDeviceCertificate(long argc, __in_ecount( argc ) char **argv);
DRM_RESULT DRM_CALL Test_CreateModelCertificate(long argc, __in_ecount( argc ) char **argv);
DRM_RESULT DRM_CALL Test_CreateCompanyCertificate(long argc, __in_ecount( argc ) char **argv);

/* RSA keys functions */
DRM_RESULT DRM_CALL Test_RSAToXML(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( Certs )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_BCERTTEST_H_ */
