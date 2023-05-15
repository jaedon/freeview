/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmdeletedstore.h>
#include <drmdatastore.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_DELETEDSTORE_UpdateRecentExpired(
    __in       DRM_DST            *pDatastore,
    __in const DRM_KID            *pKID )
{
    UNREFERENCED_PARAMETER( pDatastore );
    UNREFERENCED_PARAMETER( pKID );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DELETEDSTORE_CheckRecentExpired(
    __in                  DRM_VOID                *f_pcontextOEM,
    __in                  DRM_DST                 *f_pDatastore,
    __in            const DRM_KID                 *f_pKID,               /* kid to query */
    __out_ecount(1)       DRM_BOOL                *f_pfRecentExpired )   /* query result */
{
    UNREFERENCED_PARAMETER( f_pcontextOEM );
    UNREFERENCED_PARAMETER( f_pDatastore );
    UNREFERENCED_PARAMETER( f_pKID );
    UNREFERENCED_PARAMETER( f_pfRecentExpired );

    /*
    ** Return success without changing f_pfRecentExpired, i.e. no recently expired license
    */
    return DRM_SUCCESS;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

