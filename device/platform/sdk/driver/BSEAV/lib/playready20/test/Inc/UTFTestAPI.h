/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __UTFTEST_H
#define __UTFTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define MAX_BUFFER_SIZE 5

DRM_RESULT DRM_CALL TestDRM_UTF8_VerifyBytes(
    IN long argc,
    __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_RoundtripForSymbolUTF8(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_RoundtripForSymbolUTF32(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF8toUTF16(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF16toUTF8(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF16toUTF32(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF32toUTF16(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_FileASCIItoUTF16(
    IN long argc,
    IN __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF16toASCII(
    IN long argc,
    IN __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS( UTF )

EXIT_PKTEST_NAMESPACE

#endif /* __UTFTEST_H */

