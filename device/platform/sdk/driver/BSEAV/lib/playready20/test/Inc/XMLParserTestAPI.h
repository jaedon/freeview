/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_XMLPARSERTEST_H_
#define _DRM_XMLPARSERTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** XMLParser Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLParser Non-APIMAP Functions 
** ---------------------------------------
*/

/*
** ---------------------------------------
** XMLParser APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNode(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeAttribute(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLSubNode(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeA(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeAttributeA(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLSubNodeA(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilReinitialize(long iIndex, __in_opt char **argv); 

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeCData(long argc, __in_ecount(argc) char **argv);

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeCDataA(
    long argc, 
    __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestXMLPUtilEnumNextSubNode(
                      long argc, 
    __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestXMLPUtilGetSubNodeByPath(
                                                long argc, 
                              __in_ecount(argc) char **argv );

DRM_RESULT DRM_CALL TestXMLPUtilGetSubNodeByPathA(
                                                long argc, 
                              __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS( XMLParser )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_XMLPARSERTEST_H_ */
