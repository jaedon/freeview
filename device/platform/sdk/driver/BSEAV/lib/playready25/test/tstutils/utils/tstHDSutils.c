/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <tstHDSutils.h>
#include <drmcrt.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

static DRM_WORD     _dwDeviceStoreBlockSize=32768;
static DRM_DST_BLKNUMTYPE _eDeviceStoreBlockNumType=eDRM_DSTBLKNUM_DWORD;


/* Allocates the hds context and opens the specific store. If fails, then *ppHds==NULL */
DRM_RESULT DRM_CALL OpenHDS(DRM_DST **ppHds, const DRM_WCHAR *pwszStorePath, DRM_BOOL fCreateNew)
{
    DRM_RESULT dr;
    DRM_BOOL fHdsInited = FALSE;
    DRM_DWORD cbLen = 0;
    DRM_HDS_CONTEXT *pHdsContext = NULL;

    ChkArg(ppHds);
    ChkMem(*ppHds = (DRM_DST*)Oem_MemAlloc(SIZEOF(DRM_DST)));
    ChkMem(pHdsContext = (DRM_HDS_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_HDS_CONTEXT)));

    ChkDR( DRM_DST_Init( eDRM_DST_HDS, (DRM_DST_CONTEXT *)pHdsContext, SIZEOF(DRM_HDS_CONTEXT), *ppHds ) );
    fHdsInited = TRUE;

    if ( pwszStorePath != NULL )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( pwszStorePath ), &cbLen ) );
    }

    dr = DRM_DST_CreateStore( NULL,
                             ( const DRM_BYTE * )pwszStorePath,
                             cbLen,
                             _dwDeviceStoreBlockSize,
                             _eDeviceStoreBlockNumType,
                             fCreateNew,
                             0,
                             *ppHds );
    if (fCreateNew || dr != DRM_E_DST_EXISTS) {
        ChkDR(dr);
    }
    ChkDR(DRM_DST_OpenStore( NULL,
                            ( DRM_BYTE * )pwszStorePath,
                            cbLen,
                            0,
                            *ppHds));
ErrorExit:
    if (DRM_FAILED(dr) && ppHds && *ppHds) {
        if (fHdsInited)
            DRM_DST_Uninit(*ppHds);
        SAFE_OEM_FREE(pHdsContext);
        SAFE_OEM_FREE(*ppHds);
        *ppHds = NULL;
    }
    return dr;
}

/* Close the hds store and release the context. *pHds is invalid after the call succeeds. */
DRM_RESULT DRM_CALL CloseHDS(DRM_DST *pHds)
{
    DRM_RESULT dr = DRM_SUCCESS;
    if (pHds) {
        ChkDR(DRM_DST_CloseStore(pHds));
        ChkDR(DRM_DST_Uninit(pHds));
        SAFE_OEM_FREE(pHds->poContextDST);
        SAFE_OEM_FREE(pHds);
    }
ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

/***********************************************************
**
**  Function: TST_CheckForLicenseSecureStoreEntry
**
**  Synopsis: Checks if a license has a secure store entry.
**
**  Arguments:
**      [f_pwszFullPath] - The full path to the HDS.
**      [f_pLID]         - The license ID / rights ID to check if a secure store entry exists for.
**
**  Returns:
**      DRM_SUCCESS if license has a secure store entry
**      DRM_S_FALSE if license does not have a secure store entry
***********************************************************/
DRM_RESULT DRM_CALL TST_CheckForLicenseSecureStoreEntry( __in const DRM_WCHAR *f_pwszFullPath,
                                                         __in const DRM_ID    *f_pLID )
{
    DRM_RESULT            dr          = DRM_SUCCESS;
    DRM_SECSTORE_CONTEXT *pcontextSST = NULL;
    DRM_DST              *pDST        = NULL;
    DRM_BOOL              fKeyOpen    = FALSE;
    DRM_BOOL              fHDSOpen    = FALSE;

    ChkArg( f_pwszFullPath != NULL );
    ChkArg( f_pLID != NULL );

    ChkDR( OpenHDS( &pDST, f_pwszFullPath, FALSE ) );
    fHDSOpen = TRUE;

    ChkMem( pcontextSST = (DRM_SECSTORE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_SECSTORE_CONTEXT ) ) );

    dr = DRM_SST_OpenKeyTokens( pcontextSST, f_pLID, NULL, NULL, DRM_SECURE_STORE_OPEN_ALWAYS, SECURE_STORE_LICENSE_DATA, pDST );
    fKeyOpen = ( dr == DRM_SUCCESS );
    ChkDR( dr );

    dr = DRM_SST_LoadKeyTokens( pcontextSST, DRM_SECURE_STORE_OPEN_EXISTING, pDST );
    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        dr = DRM_S_FALSE;
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    if( fKeyOpen )
    {
        DRM_SST_CloseKey( pcontextSST, pDST, NULL, NULL );
    }
    if( fHDSOpen )
    {
        CloseHDS( pDST );
    }
    SAFE_OEM_FREE( pcontextSST );
    return dr;
}

/***********************************************************
**
**  Function: TST_GetNameSpaceSlotCount
**
**  Synopsis: Returns the number of used slots in the given name space.
**            Usefull when needing to know if bulk of data has been cleaned up
**            without having to enumerate each data.
**
**  Arguments:
**      [f_pwszFullPath]      - The full path to the HDS.
**      [f_pszNameSpaceName]  - The name of the name space to get slot usage data on.
**      [f_pdwInUse]          - Returns the number of slots in active use.
**
***********************************************************/
DRM_RESULT DRM_CALL TST_GetNameSpaceSlotCount( __in const DRM_WCHAR *f_pwszFullPath,
                                               __in const DRM_CHAR  *f_pszNameSpaceName,
                                               __out DRM_DWORD      *f_pdwInUse )
{
    DRM_RESULT                dr              = DRM_SUCCESS;
    DRM_DST                  *pDST            = NULL;
    DRM_BOOL                  fHDSOpen        = FALSE;
    DRM_BOOL                  fNsOpened       = FALSE;
    DRM_BOOL                  fSlotOpen       = FALSE;
    DRM_DWORD                 dwCount         = 0;
    DRM_DWORD                 cbName          = 0;
    DRM_DWORD                 cbSlot          = 0;
    DRM_ID                    oID1            = { 0 };
    DRM_ID                    oID2            = { 0 };
    DRM_DST_NAMESPACE_CONTEXT oNsContext      = { 0 };
    DRM_DST_SLOT_CONTEXT      oDstSlotContext = { 0 };
    DRM_DST_ENUM_CONTEXT      oDstEnumContext = { 0 };
    DRM_DST_NAMESPACE         oNameSpaceID    = { 0 };

    ChkArg( f_pwszFullPath != NULL );
    ChkArg( f_pszNameSpaceName != NULL );
    ChkArg( f_pdwInUse != NULL );

    *f_pdwInUse = 0;

    ChkDR( OpenHDS( &pDST, f_pwszFullPath, FALSE ) );
    fHDSOpen = TRUE;

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pszNameSpaceName ), &cbName ) );
    ChkArg( cbName <= __CB_DECL( DRM_DST_CONTEXT_LEN ) );
    memcpy( oNameSpaceID.rgbOpaqueData, f_pszNameSpaceName, cbName );

    ChkDR( DRM_DST_OpenNamespace( pDST,
                                  &oNameSpaceID,
                                  eDRM_DST_OPEN_EXISTING,
                                  0,
                                  0,
                                  &oNsContext ) );

    fNsOpened = TRUE;

    ChkDR( DRM_DST_InitSlotEnum( pDST,
                                 &oNsContext,
                                 NULL,
                                 eDRM_DST_LOCKSHARED,
                                 &oDstEnumContext ) );
    while( TRUE )
    {
        /*  Look for next available slot */
        dr = DRM_DST_SlotEnumNext( pDST,
                                   &oDstEnumContext,
                                   &oDstSlotContext,
                                   &oID1,
                                   &oID2,
                                   &cbSlot );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR(dr);
        fSlotOpen = TRUE;

        dwCount++;

        /*  Close the slot  */
        ChkDR( DRM_DST_CloseSlot( pDST, &oDstSlotContext ) );
        fSlotOpen = FALSE;
    }

    *f_pdwInUse = dwCount;

ErrorExit:
    if( fSlotOpen )
    {
        DRM_DST_CloseSlot( pDST, &oDstSlotContext );
    }

    if ( fNsOpened )
    {
        DRM_DST_CloseNamespace( pDST, &oNsContext );
    }

    if( fHDSOpen )
    {
        CloseHDS( pDST );
    }
    return dr;
}

