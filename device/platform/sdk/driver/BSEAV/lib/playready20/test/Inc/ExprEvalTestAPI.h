/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_EXPREVALTEST_H_
#define _DRM_EXPREVALTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** ExprEval Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** ExprEval Non-APIMAP Functions 
** ---------------------------------------
*/

/*
** ---------------------------------------
** ExprEval APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestLicEvalContextSetting2(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestDRMEvaluateExpression(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( ExprEval )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_EXPREVALTEST_H_ */
