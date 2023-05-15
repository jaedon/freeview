/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMCLEANSTORE_C
#include <drmcleanstore.h>
#include <drmmanager.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function :   DRM_LST_Clean
**
** Synopsis :   Cleanup the XML and XMR license stores specified in
**              f_pContextLSTXML and f_pContextLSTXMR. For PC
**              ( if DRM_SUPPORT_CLEANUP is defined ), it will remove
**              unusable licenses from the License store, and then "defrag"
**              the store. For devices, it will only defrag the store
**
** Arguments :
**      [f_pContextLEVL]         : Preinitialized License eval context
**      [f_pContextLSTXML]       : Preinitialized LicStore context ( XML store )
**      [f_pContextLSTXMR]       : Preinitialized LicStore context ( XMR store )
**                                 May be NULL
**      [f_pLicStoreEnumContext] : Pointer to the license store enum context
**      [f_dwFlags]              : Flags to indicate what type of clean operations to
**                                 perform.  Valid values are:
**                                    DRM_STORE_CLEANUP_ALL
**                                 or any combination of:
**                                    DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES
**                                    DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES
**      [f_pbBuffer]             : Buffer supplied by user
**      [f_cbBuffer]             : Size of f_pbBuffer
**      [f_pvCallerData]         : Opaque context data to be funnelled
**                                 back to caller during callback. Could be NULL.
**      [f_dwCallbackInterval]   : % completion after which callback ( if any )
**                                 should be invoked. e.g. if dwCallbackInterval = 5,
**                                 callback will be invoked after 5%, 10%, 15%, etc.
**                                 completion
**      [f_pfnCallback]          : Optional caller supplied callback function
**      [f_pDatastore]           : Preinitialized DST context
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer arguments was NULL
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LST_Clean(
    __in                        DRM_BYTE                  *f_pContextLEVL,
    __in_bcount( SIZEOF( DRM_LICSTORE_CONTEXT ) )
                                DRM_BYTE                  *f_pContextLSTXML,
    __in_bcount( SIZEOF( DRM_LICSTORE_CONTEXT ) )
                                DRM_BYTE                  *f_pContextLSTXMR,
    __in                        DRM_LICSTOREENUM_CONTEXT  *f_pLicStoreEnumContext,
    __in                        DRM_DWORD                  f_dwFlags,
    __in_bcount(f_cbBuffer )    DRM_BYTE                  *f_pbBuffer,
    __in                        DRM_DWORD                  f_cbBuffer,
    __in_opt              const DRM_VOID                  *f_pvCallerData,
    __in                        DRM_DWORD                  f_dwCallbackInterval,
    __in_opt                    pfnStoreCleanupProgress    f_pfnCallback,
    __in                        DRM_DST                   *f_pDatastore )
{
    DRM_RESULT           dr                    = DRM_SUCCESS;

    ChkArg( f_pbBuffer             != NULL );
    ChkArg( f_pDatastore           != NULL );
    ChkArg( f_pLicStoreEnumContext != NULL );

    if (   f_dwFlags & DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES
        || f_dwFlags & DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES )
    {
        ChkDR( DRM_CLEANSTORE_RemoveUnusableLicenses(
            f_pContextLEVL,
            f_pContextLSTXML,
            f_pContextLSTXMR,
            f_pLicStoreEnumContext,
            f_dwFlags,
            f_pbBuffer,
            f_cbBuffer,
            f_pvCallerData,
            f_dwCallbackInterval,
            f_pfnCallback,
            f_pDatastore ) );
    }

    /*
    **  Defrag the license store
    */
    ChkDR( DRM_DST_CleanupStore( f_pDatastore, TRUE ) );

ErrorExit:
    /*
    ** Remap DRM_E_NOMORE to DRM_SUCCESS
    */
    if( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }

    return dr;
}


EXIT_PK_NAMESPACE_CODE;

