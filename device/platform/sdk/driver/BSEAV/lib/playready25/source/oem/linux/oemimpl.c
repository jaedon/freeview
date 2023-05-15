/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMIMPL_C

#include <stdlib.h>
#include <drmtypes.h>
#include <drmresults.h>
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
	return BCM_Oem_MemAlloc(f_cbSize);
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
	BCM_Oem_MemFree(f_pv);
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
    __inout DRM_VOID   *f_pv,
    __in    DRM_DWORD   f_cbSize )
{
	return BCM_Oem_MemRealloc(f_pv, f_cbSize);
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

EXIT_PK_NAMESPACE_CODE;
