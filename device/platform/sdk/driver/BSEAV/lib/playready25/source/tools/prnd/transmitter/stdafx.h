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
#include <ws2tcpip.h>
#include <Windows.h>
#include <atlbase.h>
#include <atlcom.h>

#include <drmfeatures.h>
#include <drmcommon.h>
#include <drmmanager.h>
#include <drmm2tsencryptor.h>
#include <drmlicgen.h>
#include <drmtoolsinit.h>
#include <drmprndprotocol.h>
#include <drmxmlparser.h>

#include <time.h>
#include <Locks.h>

#pragma warning( disable:4244 ) /* conversion from 'type1' to 'type2', possible loss of data */
#pragma warning( disable:4242 ) /* conversion from 'int' to 'char', possible loss of data */

#define _HAS_EXCEPTIONS 0
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <memory>
#include <deque>

#define DELETE_OBJECT( p )        if( p != NULL ) { delete p; p = NULL; }
#define DELETE_OBJECT_ARRAY( p )  if( p != NULL ) { delete[] p; p = NULL;  }

#define CLOSE_HANDLE( h )   if( h != INVALID_HANDLE_VALUE ) { CloseHandle( h ); h = INVALID_HANDLE_VALUE; }

#define TS_PACKET_SIZE  ( 188 * 128 )

