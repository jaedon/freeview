/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmdla.h>
#include <drmtoolsinit.h>
#include <drmsyncstore.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

/*****************************************************************************
** Function:    TestManagerInitLicenseSyncEnum
**
** Synopsis:    Initialize the enumeration of the sync list
**
** Arguments:   argv[0]             -- ignored
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerInitLicenseSyncEnum( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_APP_CONTEXT *poManagerContext   = NULL;

    if( DRM_WMDRMDLA_IsWMDRMDLASupported()
     && DRM_SNC_IsLicenseSyncSupported() )
    {
        GET_SHARED_APP_CONTEXT( poManagerContext );

        ChkDR( Drm_SyncListItem_InitEnum( poManagerContext ) );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerLicenseSyncEnumNext
**
** Synopsis:    Get the next license from the sync list
**
** Arguments:
**              argv[0]  -- maxCount, NULL means use DRM_SYNC_IGNORE_THRESHOLD_PARAMETER
**              argv[1]  -- maxHours, NULL means use DRM_SYNC_IGNORE_THRESHOLD_PARAMETER
**              argv[2]  -- if NULL, NULL manager context is used.
**              argv[3]  -- KID output variable-- NULL or NORMAL. if NULL then argv[4+] is checked against the value in the header context.
**                          NULL if missing.
**              argv[4+] -- expected KIDs
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerLicenseSyncEnumNext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_DWORD        cMaxCount          = 0;
    DRM_DWORD        cMaxHours          = 0;
    DRM_KID          kidOut             = {0};
    DRM_DWORD        cbKidOut           = SIZEOF( kidOut.rgb );
    DRM_BOOL         fGoodKidOut        = FALSE;
    DRM_APP_CONTEXT *poManagerContext   = NULL;

    if( DRM_WMDRMDLA_IsWMDRMDLASupported()
     && DRM_SNC_IsLicenseSyncSupported() )
    {
        GET_SHARED_APP_CONTEXT( poManagerContext );

        ChkArg( argc >= 2 );

        if ( argv[0] != NULL )
        {
            cMaxCount = (DRM_DWORD)atoi(argv[0]);
        }
        if ( argv[1] != NULL )
        {
            cMaxHours = (DRM_DWORD)atoi(argv[1]);
        }
        if ( argc >= 4 && argv[3] != NULL )
        {
            fGoodKidOut = TRUE;
        }

        ChkDR( Drm_SyncListItem_EnumNext( ( argc >= 3 && argv[2] == NULL ) ? NULL : poManagerContext,
                                            argv[0] == NULL ? DRM_SYNC_IGNORE_THRESHOLD_PARAMETER : cMaxCount,
                                            argv[1] == NULL ? DRM_SYNC_IGNORE_THRESHOLD_PARAMETER : cMaxHours,
                                            fGoodKidOut ? &kidOut : NULL ) );

        if ( argc >= 5 )
        {
            DRM_RESULT    drInner     = DRM_E_TEST_UNEXPECTED_OUTPUT;
            DRM_SUBSTRING dsstrTemp   = EMPTY_DRM_SUBSTRING;
            DRM_KID       KidIn       = {0};
            DRM_DWORD     cbKidIn     = SIZEOF( DRM_KID );
            long          iKID        = 0;              /* use a signed type since argc is signed */

            const DRM_APP_CONTEXT_INTERNAL *pContextMGR = (const DRM_APP_CONTEXT_INTERNAL *) poManagerContext;
            for( iKID = 4; iKID < argc ; iKID++ )
            {
                ChkArg( argc > iKID  );
                ChkArg( argv[ iKID ] != NULL );

                ZEROMEM( &KidIn.rgb, SIZEOF( DRM_KID ) );

                dsstrTemp.m_ich = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[iKID] ), &dsstrTemp.m_cch ) );

                cbKidIn = SIZEOF( DRM_KID );

                ChkDR( DRM_B64_DecodeA( argv[iKID], &dsstrTemp, &cbKidIn, KidIn.rgb, 0 ) );

                if( !fGoodKidOut )
                {
                    DRM_CONST_STRING dcstrHeader = { 0 };
                    dcstrHeader.pwszString = ( const DRM_WCHAR * )pContextMGR->pbDRMHeaderData;
                    dcstrHeader.cchString = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

                    cbKidOut = SIZEOF( kidOut.rgb );

                    ChkDR( DRM_B64_DecodeW( &dcstrHeader, &cbKidOut, kidOut.rgb, 0 ) );
                }

                if( MEMCMP( KidIn.rgb, kidOut.rgb, cbKidOut ) == 0 )
                {
                    drInner = DRM_SUCCESS;
                    break;
                }
            }
            ChkDR( drInner );
        }
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestPrintSyncStore
**
** Synopsis:    Print the sync store
**
** Arguments:
**              argv[0]  -- ignored
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestPrintSyncStore( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 iLicenses            = 0;
    DRM_APP_CONTEXT          *pMgrContext          = NULL;
    DRM_BYTE                 *pbOpaqueBuffer       = NULL;
    DRM_APP_CONTEXT_INTERNAL *pMgrInternal         = NULL;
    DRM_CHAR                 *pszHeader            = NULL;
    DRM_CONST_STRING         *pwszDeviceStore      = NULL;
    DRM_DWORD                 cchKid               = CCH_BASE64_EQUIV(SIZEOF( DRM_KID ));
    DRM_CHAR rgchKid[CCH_BASE64_EQUIV(SIZEOF( DRM_KID )) + 1] = {0};

    if( DRM_WMDRMDLA_IsWMDRMDLASupported()
     && DRM_SNC_IsLicenseSyncSupported() )
    {
        GET_SHARED_DEVICE_STORE( pwszDeviceStore );

        ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, pwszDeviceStore, TEST_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pMgrContext ) );

        pMgrInternal = ( DRM_APP_CONTEXT_INTERNAL * ) pMgrContext;


        ChkDR( Drm_SyncListItem_InitEnum( pMgrContext ) );

        while( DRM_SUCCEEDED( dr ) )
        {
            DRM_LICENSE_STATE_DATA    lsd = {0};
            DRM_KID                   kid = {0};
            DRM_DWORD                 cchHeader = DRM_MAX_HEADERDATASIZE / SIZEOF( DRM_WCHAR );

            dr = Drm_SyncListItem_EnumNext( pMgrContext,
                                              DRM_SYNC_IGNORE_THRESHOLD_PARAMETER,
                                              DRM_SYNC_IGNORE_THRESHOLD_PARAMETER,
                                             &kid );
            if ( dr == DRM_E_NOMORE )
            {
                break;
            }
            else
            {
                ChkDR( dr );
            }
            iLicenses++;
            ChkDR( DRM_SNC_GetSyncStoreEntry( &pMgrInternal->contextSync,
                                              &kid,
                                              &lsd,
                                              ( DRM_WCHAR * ) pMgrInternal->pbDRMHeaderData,
                                              &cchHeader ) );
            pMgrInternal->cbHeaderData = cchHeader * SIZEOF( DRM_DWORD );

            cchKid = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
            ZEROMEM( rgchKid, cchKid );
            ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * ) &kid, SIZEOF( DRM_KID ), rgchKid, &cchKid, 0 ) );

            ChkMem( pszHeader = (DRM_CHAR *) Oem_MemAlloc( cchHeader+1 ) );
            ZEROMEM( ( DRM_BYTE * )pszHeader, cchHeader + 1 );
            DRM_UTL_DemoteUNICODEtoASCII( ( const DRM_WCHAR * ) pMgrInternal->pbDRMHeaderData, pszHeader, cchHeader );
            pszHeader[cchHeader] = '\0';

            Log( "", "license %d : %s \n %s", iLicenses, rgchKid, pszHeader );

            ChkDR( TST_UTL_LogLicenseStateData( "      ", &lsd ) );

            SAFE_OEM_FREE( pszHeader );
        }
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    SAFE_OEM_FREE( pszHeader );

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pMgrContext );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

