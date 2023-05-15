/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __TOOLTEST_H
#define __TOOLTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* globals */

#define LOCAL_HDS_FILE  "tempstore.hds"
#define TOOLTEST_DEFAULT_PATH "C:\\wmdrmpd\\"

DRM_RESULT DRM_CALL SetupDRMDir(long argc, __in_ecount_opt(argc) char **argv );
DRM_RESULT DRM_CALL RunPregenTool(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL RunPregenToolEx(long argc, __in_ecount(argc) char **argv);
DECLARE_MODULE_DEFS( ToolTest )

EXIT_PKTEST_NAMESPACE

#endif /* __TOOLTEST_H */
