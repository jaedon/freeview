/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmhdrbuilderconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_WCHAR      g_rgwchAttributeHeaderXmlnsName              [] =  { ONE_WCHAR('x', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0', '\0') };
/* "http://schemas.microsoft.com/DRM/2007/03/PlayReadyHeader" */
const                DRM_WCHAR      g_rgwchAttributeHeaderXmlnsValue             [] =  { ONE_WCHAR('h', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('f', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('2', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('7', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('3', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('\0', '\0')  };

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeHeaderXmlnsName              = CREATE_DRM_STRING( g_rgwchAttributeHeaderXmlnsName );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeHeaderXmlnsValue             = CREATE_DRM_STRING( g_rgwchAttributeHeaderXmlnsValue );

EXIT_PK_NAMESPACE_CODE;

