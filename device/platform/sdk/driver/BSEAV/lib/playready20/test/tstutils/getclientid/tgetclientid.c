/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "tgetclientID.h"
#include "drmcontextsizes.h"
#include "drmkeyfile.h"
#include <drmactivation.h>
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

DRM_RESULT TestGetClientID(CLIENTID *pClientID)
{
    DRM_RESULT dr;    
    DRM_BYTE   *pbBuffer = NULL;
    DRM_DWORD   cbBuffer = 0;
    DRM_STRING  dstrDevCert;
    DRM_BB_CONTEXT *poBBContext = NULL;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    ChkBOOL( DRM_WMDRM_IsWMDRMSupported(), DRM_E_NOTIMPL );

    ChkMem( poBBContext = ( DRM_BB_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_BB_CONTEXT ) ) );
    ZEROMEM( poBBContext, SIZEOF( DRM_BB_CONTEXT ) );

    ChkMem( poKeyfileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
    ZEROMEM( poKeyfileContext, SIZEOF( DRM_KEYFILE_CONTEXT ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    poBBContext->pKeyFileContext = poKeyfileContext;
    ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    // Don't validate keyfile signature since it may be a different DLL that created it with a different key
    ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, FALSE ) );
    ChkDR( DRM_KF_Close( poKeyfileContext ) );

    ChkDR( DRM_KF_GetCertificate (
                poKeyfileContext,
                eKF_CERT_TYPE_WMDRM,
                &pbBuffer,
                &cbBuffer ) );
    DSTR_FROM_PB( &dstrDevCert, pbBuffer, cbBuffer );

    ChkDR( DRM_DCP_LoadPropertiesCache( 
                    NULL,
                    (DRM_CONST_STRING*) &dstrDevCert, 
                    NULL,
                    &poBBContext->CryptoContext,
                    &poBBContext->cachedCertValues ) );

    /* Initialize blackbox */
    ChkDR(DRM_BBX_Initialize(NULL, poBBContext));

    ChkDR(DRM_BBX_GetClientId(pClientID, poBBContext));
ErrorExit:
    SAFE_OEM_FREE( poKeyfileContext );

    if( poBBContext != NULL )
    {
        DRM_BBX_Shutdown(poBBContext);
    }
    SAFE_OEM_FREE( poBBContext );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

