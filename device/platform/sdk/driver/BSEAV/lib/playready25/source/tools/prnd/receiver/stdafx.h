/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlfile.h>
#include <atltime.h>

#include <drmcommon.h>
#include <drmmanager.h>
#include <drmm2tsencryptor.h>
#include <drmlicgen.h>
#include <drmtoolsinit.h>
#include <drmprndprotocol.h>
#include <drmxmlparser.h>
#include <byteorder.h>


#pragma warning( disable:4702 ) /* unreachable code */

/*
** ignore deprecated api warnings in standard headers
*/
#define _HAS_EXCEPTIONS 0
#include <map>
#include <string>

inline HRESULT DRFromWin32( __in LONG code )
{
    return HRESULT_FROM_WIN32( static_cast<ULONG>( code ) );
}

inline HRESULT VerifyWin32( __in BOOL condition )
{
    HRESULT dr = S_OK;

    if( !condition )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        if( SUCCEEDED( dr ) )
        {
            // Unexpected:
            // GetLastError returns success after a Win32 failure
            DRMASSERT( !"error code expected" );
            dr = E_UNEXPECTED;
        }
    }

    return dr;
}

static const DRM_ID EMPTY_DRM_ID = { 0 };
