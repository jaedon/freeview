/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_XMLBUILDERTEST_H_
#define _DRM_XMLBUILDERTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** XMLBuilder Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLBuilder Non-APIMAP Functions
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLBuilder APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestXMLBCreateDocument(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBReallocDocument(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBCloseDocument(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBOpenNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBCloseCurrNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBGetCurrNodeName(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBGetContextSize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBAddAttribute(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBAddData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBAddCData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBAppendNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBUtilReinitialize(long iIndex, __in_opt char **argv);
DRM_RESULT DRM_CALL TestXMLBWriteTag(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBWriteCDATATag(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBAddNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestXMLBValidateDocument(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( XMLBuilder )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_XMLBUILDERTEST_H_ */
