/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_XMLBUILDERATEST_H_
#define _DRM_XMLBUILDERATEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** XMLBuilderA Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLBuilderA Non-APIMAP Functions 
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLBuilderA APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL  TestXMLBACreateDocument(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBACloseDocument(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBAOpenNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBACloseCurrNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBAAddAttribute(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBAAddData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBAAddCData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL  TestXMLBAReserveSpace(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBAUtilReinitialize(
    IN long iIndex,
    IN __in_opt char **argv );


DECLARE_MODULE_DEFS( XMLBuilderA )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_XMLBUILDERATEST_H_ */
