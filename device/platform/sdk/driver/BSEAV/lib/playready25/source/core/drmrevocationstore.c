/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMREVOCATIONSTORE_C
#include <drmrevocationstore.h>
#include <drmapprevocation.h>
#include <drmdevicerevocation.h>
#include <drmrevocationconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVS_LoopkupRevocationLIDFromGUID(
    __in  const DRM_GUID  *f_pRevGUID,
    __out const DRM_ID   **f_pLID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC__LoopkupRevocationLIDFromGUID );

    DRMASSERT( f_pRevGUID != NULL );
    DRMASSERT( f_pLID     != NULL );
    __analysis_assume( f_pLID != NULL );

    *f_pLID = NULL;

    if( MEMCMP( f_pRevGUID, &g_guidRevocationTypeRevInfo, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidRevocationInfo;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidRevocationInfo2;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypePlayReadySilverLightRuntime, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidPRRTRevocation;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypePlayReadySilverLightApplication, SIZEOF( DRM_GUID ) ) == 0 )
    {
        *f_pLID = &g_lidPRAppRevocation;
    }
    else if( MEMCMP( f_pRevGUID, &g_guidRevocationTypeWMDRMNET, SIZEOF( DRM_GUID ) ) == 0 )
    {
        /*
        ** Always make this CRL available even if DRM_WMDRMNET_IsWmdrmnetSupported is false.
        ** We need to store/fetch (read/write) the WMDRMNET revocation list on all clients.
        ** This is so that when sending licenses to other clients (e.g. via PRND protocol), we
        **  can send the RevInfo2 and all CRLs it requires in case the target client supports it.
        ** WMDRMNET is currently the only CRL which RevInfo2 depends on which is also feature-dependent.
        ** The others (RevInfo2 itself and the two PlayReady lists) are already always supported.
        */
        *f_pLID = &g_lidWMDRMNET_Revocation;
    }
    else if( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
          && ( MEMCMP( f_pRevGUID, &g_guidRevocationTypeDevice, SIZEOF( DRM_GUID ) ) == 0 ) )
    {
        *f_pLID = &g_lidDeviceRevocation;
    }
    else if( DRM_APPREVOCATION_IsAppRevocationSupported()
          && ( MEMCMP( f_pRevGUID, &g_guidRevocationTypeApp, SIZEOF( DRM_GUID ) ) == 0 ) )
    {
        *f_pLID = &g_lidAppRevocation;
    }
    else
    {
        ChkDR( DRM_E_REVOCATION_GUID_NOT_RECOGNIZED );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;


