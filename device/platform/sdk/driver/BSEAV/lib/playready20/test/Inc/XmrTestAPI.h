/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XMRTEST_TEST_API_H
#define __DRM_XMRTEST_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** XMR Test Global Variable  Names
** ---------------------------------------
*/
#define XMR_BUILDERCONTEXT          "XMRBuilderContext"
#define XMRLICENSE                  "XMRLicense"
#define XMRLICENSE_BUFFER           "XMRLicenseBuffer"
#define XMRLICENSE_BUILDER_STACK    "XMRLicenseBuilderStack"
#define XMRLICENSE_PARSER_STACK     "XMRLicenseParserStack"
#define XMRLICENSE_BUFFER_BACKUP    "XMRLicenseBufferBackup"
#define XMRLICENSE_BUFFER_USEBACKUP "XMRLicenseBufferUseBackup"

/*
** ---------------------------------------
** XMR Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL  TestStartLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL  TestParseOuterContainersAndFinishLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL  TestUnpackLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL  TestVerifyUnpackedLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL  TestAddXMRObjectsAndFinishLicense( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL  TestAddUnknownXMRObject( long argc, __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS(xmr)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_XMR_TEST_API_H */
