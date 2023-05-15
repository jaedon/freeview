/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMDEBUG_C
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <oemdebug.h>
#include <drmtrace.h>
#include <drmlastinclude.h>

#define TRACE_MAX_STRING_BUFFER         1024

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
** Function:   _oem_trace
**
** Synopsis:   static function that prints to console
**
** Arguments:  [f_pszFormat] -- Format string of the variable arguments to follow.
**             [f_ArgList]   -- Pointer to variable argument list.
**
** Returns:    None
**
***********************************************************************/
static void DRM_CALL _oem_trace(
    __in const DRM_CHAR     *f_pszFormat,
    __in       DRM_VA_LIST   f_ArgList )
{
    DRM_CHAR rgchBuffer[ TRACE_MAX_STRING_BUFFER + 1 ] = { 0 };

    vsprintf_s( rgchBuffer,
                TRACE_MAX_STRING_BUFFER,
                f_pszFormat,
                (va_list)f_ArgList );

    DRM_TRACE_NO_SCOPE( ( rgchBuffer ) );
}

/**********************************************************************
** Type definition and pointer to global callback for the tracing.
***********************************************************************/

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "Trace callback is meant to be a global function pointer.")
Oem_Debug_Trace_Hook_Callback g_TraceCallback = _oem_trace;
PREFAST_POP;

/**********************************************************************
** Function:   Oem_Debug_Trace
**
** Synopsis:   Trace function that prints that calls global callback
**             to pring data to file or to console.
**
** Arguments:  [f_szFmt] -- Format string of the variable arguments to follow.
**
** Returns:    None
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_Debug_Trace(
    __in const DRM_CHAR *f_szFmt,
    ... )
{
    va_list alist;
    va_start( alist, f_szFmt );
    g_TraceCallback( f_szFmt, (DRM_VA_LIST)alist );
    va_end( alist );
}

/**********************************************************************
** Function:    Oem_Debug_HookTrace
**
** Synopsis:    Change the global trace handler to one of the callers choosing.
**
** Arguments:   [pNewTraceFcn] -- Ptr to a function that handles traces.
**
** Returns:     None
**
** Notes:
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_Debug_HookTrace(
    __in Oem_Debug_Trace_Hook_Callback f_pNewTraceFcn )
{
#if DBG
    if ( NULL == f_pNewTraceFcn )
    {
        g_TraceCallback = _oem_trace;
    }
    else
    {
        g_TraceCallback = f_pNewTraceFcn;
    }
#endif
}

/**********************************************************************
** Function:   __drmassert
**
** Synopsis:   Implements assert functionality.
**             If fAssert is false, then trace string that assert
**             was rised and calls int 3 on x86 system.
**
** Arguments:  [f_fAssert]   -- Condiiton to test. If FALSE - rise assert
**             [f_assertcmd] -- Text string explainin the condition.
**             [f_file]      -- File where assert happened.
**             [f_line]      -- Line where assert happened.
**
** Returns:    None
**
***********************************************************************/
static DRM_VOID DRM_CALL __drmassert(
    __in       DRM_BOOL  f_fAssert,
    __in const DRM_CHAR *f_assertcmd,
    __in const DRM_CHAR *f_file,
    __in       DRM_LONG  f_line )
{
#if DBG
    if ( !f_fAssert )
    {
        DRM_TRACE_NO_SCOPE( ( "DRMASSERT %s failed at %s(%d)\r\n", f_assertcmd, f_file, f_line ) );

#if _M_IX86
        __asm int 3
#endif
    }
#endif /* DBG */
}

/***************************************************************************
**  This type is to allow the ASSERT macro to have another implementation
**    defined by the caller of Oem_Debug_HookAssert
***************************************************************************/

/* Global callback assert function. It is changed by test framework during BVT test */
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "Assert callback is meant to be a global function pointer.")
Oem_Debug_Assert_Hook_Callback g_AssertCallback = __drmassert;
PREFAST_POP;

/**********************************************************************
** Function:   Oem_Debug_Assert
**
** Synopsis:   Implements assert functionality. Can either create visual
**             feedback or log to file depending on implementation.
**
** Arguments:  [f_fAssert]   -- Condiiton to test. If FALSE - rise assert
**             [f_assertcmd] -- Text string explainin the condition.
**             [f_file]      -- File where assert happened.
**             [f_line]      -- Line where assert happened.
**
** Returns:    None
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_Debug_Assert(
    __in       DRM_BOOL  f_fAssert,
    __in const DRM_CHAR *f_assertcmd,
    __in const DRM_CHAR *f_file,
    __in       DRM_LONG  f_line )
{
    g_AssertCallback( f_fAssert, f_assertcmd, f_file, f_line );
}

/**********************************************************************
** Function:    Oem_Debug_HookAssert
**
** Synopsis:    Change the global assert handler to one of the callers choosing.
**
** Arguments:   [pNewAssertFcn] -- Ptr to a function that handles asserts.
**
** Returns:     None
**
** Notes:
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_Debug_HookAssert(
    __in Oem_Debug_Assert_Hook_Callback pNewAssertFcn )
{
#if DBG
    if(NULL == pNewAssertFcn)
    {
        g_AssertCallback = __drmassert;
    }
    else
    {
        g_AssertCallback = pNewAssertFcn;
    }
#endif
}

EXIT_PK_NAMESPACE_CODE;

