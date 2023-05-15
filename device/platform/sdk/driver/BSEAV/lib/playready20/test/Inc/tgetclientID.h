/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _tGetClientID_h
#define _tGetClientID_h

#include "drmcommon.h"
#include "drmlicense.h"
#include "PKTestNameSpace.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
    TestLicGenInit needs CLIENTID.
    This helper function gets the CLIENTID from WMDRMPD blackbox. On the PC side, the caller
    needs to get the CLIENTID from the real blackbox.dll. This way, we can use a single TestLicGen
    to generate local licenses.
*/
DRM_RESULT TestGetClientID(CLIENTID *pClientID);

EXIT_PKTEST_NAMESPACE

#endif
