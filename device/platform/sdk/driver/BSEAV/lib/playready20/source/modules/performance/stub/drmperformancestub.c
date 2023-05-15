/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmperformance.h>
#include <drmutilitieslite.h>
#include <oemcommon.h>
#include <drmint64.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_PERF_AllocateLogBuffer(
    __in const DRM_DWORD      f_dwNumberOfEntries )
{
    UNREFERENCED_PARAMETER( f_dwNumberOfEntries );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL DRM_PERF_FreeLogBuffer()
{
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_PERF_DumpLogBuffer(
    __in_z const DRM_CHAR     *f_pszFilename )
{
    UNREFERENCED_PARAMETER( f_pszFilename );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_PERF_On()
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL DRM_PERF_Off()
{
    return;
}

DRM_API DRM_VOID DRM_CALL DRM_PERF_LogMicrosecondCount(
    __in   const      eDRM_PERF_LogTag      f_eLogTag,
    __in   const      eDRM_PERF_LogFunction f_eLogFunction )
{
    UNREFERENCED_PARAMETER( f_eLogTag      );
    UNREFERENCED_PARAMETER( f_eLogFunction );
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_PERF_LogSeparator()
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

