/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMIMPL_C
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <oemcommon.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
** Platform Initialization functions
***********************************************************************
*/

DRM_API DRM_RESULT DRM_CALL Oem_Platform_Init()
{
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_Platform_UnInit()
{
    return DRM_SUCCESS;
}


/**********************************************************************
** Memory allocation functions
***********************************************************************
*/

/**********************************************************************
** Function:    Oem_MemAlloc
**
** Synopsis:    Allocates memory from heap.
**
** Arguments:   [f_cbSize] Number of bytes to allocate.
**
** Returns:     Valid pointer if succeeds, NULL if failed.
**
***********************************************************************/
DRM_API DRM_VOID * DRM_CALL Oem_MemAlloc(
    __in DRM_DWORD f_cbSize )
{
    if ( f_cbSize == 0 )
    {
        return NULL;
    }
#if SIXTEEN_BIT_ADDRESSING
    DRM_DWORD cb = f_cbSize / CB_NATIVE_BYTE;

    if ( ( f_cbSize % CB_NATIVE_BYTE ) != 0 )
    {
        cb++;
    }

    return malloc( cb );
#else
    return malloc( f_cbSize );
#endif
}

/**********************************************************************
** Function:    Oem_MemFree
**
** Synopsis:    Frees allocated memory.
**
** Arguments:   [f_pv] Pointer to memory buffer.
**
** Returns:     None
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_MemFree(
    __in DRM_VOID *f_pv )
{
    free( f_pv );
}

/**********************************************************************
** Function:    Oem_MemRealloc
**
** Synopsis:    Re-allocates memory from heap.
**
** Arguments:   [f_pv]     Pointer to previous allocated memory buffer.
**              [f_cbSize] Number of bytes to allocate.
**
** Returns:     Valid pointer if succeeds, NULL if failed.
**
***********************************************************************/
DRM_API DRM_VOID* DRM_CALL Oem_MemRealloc(
    __inout DRM_VOID *f_pv,
    __in DRM_DWORD f_cbSize )
{
#if SIXTEEN_BIT_ADDRESSING
    DRM_DWORD cb = f_cbSize / CB_NATIVE_BYTE;

    if ( ( f_cbSize % CB_NATIVE_BYTE ) != 0 )
    {
        cb++;
    }

    if( f_pv != NULL )
    {
        return realloc( f_pv, cb );
    }

    return malloc( cb );
#else
    if( f_pv != NULL )
    {
        return realloc( f_pv, f_cbSize );
    }

    return malloc( f_cbSize );
#endif
}


/**********************************************************************
** Function:    Oem_GetCurrentThreadId
**
** Synopsis:    Function that returns the current thread ID.
**
** Arguments:   N/A
**
** Returns:     The ID of the current thread.
**
***********************************************************************/
DRM_API DRM_DWORD DRM_CALL Oem_GetCurrentThreadId( DRM_VOID )
{
    /* Return a fixed number as thread ID. */
    return 1;
}


/**********************************************************************
** Function:    Oem_CodeCoverage_FlushData
**
** Synopsis:    Write any accumulated data to persistent storage.
**
** Arguments:   [f_pOEMContext] -- OEM specified context
**
** Returns:     DRM_VOID
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_CodeCoverage_FlushData(
    __in_opt const DRM_VOID  *f_pOEMContext )
{
    UNREFERENCED_PARAMETER( f_pOEMContext );
}

DRM_API DRM_VOID DRM_CALL Oem_Test_Mem_Alloc_Check_Leakscan() {}
DRM_API DRM_VOID DRM_CALL Oem_Test_Mem_Alloc_Clear_Leakscan() {}

EXIT_PK_NAMESPACE_CODE;
