/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TESTSETCLIENTID_H
#define _TESTSETCLIENTID_H

#include "drmcommon.h"
#include "drmlicense.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* This is the clear client ID */
extern CLIENTID g_ClientID;

DRM_RESULT TestLicResponseSetClientID(CLIENTID *pClientID);

EXIT_PKTEST_NAMESPACE

#endif
