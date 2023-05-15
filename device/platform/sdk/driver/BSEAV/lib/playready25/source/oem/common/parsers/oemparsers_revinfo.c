/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMREVOCATION_C

#include <drmutftypes.h>
#include <oemparsers.h>
#include <drmerr.h>
#include <drmprofile.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_FindEntryInRevInfo(
    __in                     const DRM_RLVI    *f_prlvi,
    __in_bcount(f_cbRevInfo) const DRM_BYTE    *f_pbRevInfo,
    __in                           DRM_DWORD    f_cbRevInfo,
    __in                     const DRM_GUID    *f_pguidEntry,
    __out                          DRM_DWORD   *f_pdwVersion )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_BOOL    fFound  = FALSE;
    DRM_DWORD   iRev    = 0;

    ChkArg( f_pdwVersion != NULL );
    ChkArg( f_pbRevInfo != NULL );
    ChkArg( f_prlvi != NULL );
    ChkArg( f_cbRevInfo > 0 );
    ChkArg( f_pguidEntry != NULL );

    *f_pdwVersion = DRM_NO_PREVIOUS_CRL;

    /* Check buffer is big enough */
    ChkBOOL( f_cbRevInfo >= f_prlvi->ibEntries + ( f_prlvi->head.dwRecordCount * SIZEOF( DRM_RLVI_RECORD ) ), DRM_E_INVALID_REV_INFO );

    /* Find the entry in the rev info corresponding to the supplied GUID */
    for ( iRev = 0; iRev < f_prlvi->head.dwRecordCount; iRev++ )
    {
        DRM_INT64  qwrlviVersion;
        DRM_DWORD  dwrlviVersion = 0;
        DRM_GUID   rlviGUID;

        /* Extract the CRL GUID from the iRev-th DRM_RLVI_RECORD in the revinfo */
        OEM_SECURE_MEMCPY( &rlviGUID, &f_pbRevInfo[f_prlvi->ibEntries + ( iRev * SIZEOF( DRM_RLVI_RECORD ) )], SIZEOF( DRM_GUID ) );

        /* Extract the CRL version from the iRev-th DRM_RLVI_RECORD in the revinfo */
        NETWORKBYTES_TO_QWORD( qwrlviVersion,
                        f_pbRevInfo,
                        f_prlvi->ibEntries + ( iRev * SIZEOF( DRM_RLVI_RECORD ) ) + SIZEOF( DRM_GUID ) );  /* Add ibEntries offset to the offset of the current record and skip the GUID */
        dwrlviVersion = DRM_I64ToUI32(qwrlviVersion);

        if( OEM_SECURE_ARE_EQUAL( &rlviGUID, f_pguidEntry, SIZEOF(DRM_GUID) ) )
        {
            fFound = TRUE;
            *f_pdwVersion = dwrlviVersion;
            break;
        }
    }
ErrorExit:
    if( DRM_SUCCEEDED( dr ) )
    {
        if( fFound )
        {
            dr = DRM_SUCCESS;
        }
        else
        {
            dr = DRM_S_FALSE;
        }
    }
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_ParseRevocationInfoHeader(
    __in_bcount( f_cbRevInfo ) const DRM_BYTE           *f_pbRevInfo,
    __in                             DRM_DWORD           f_cbRevInfo,
    __out                            DRM_RLVI           *f_pRLVI,
    __inout                          DRM_DWORD          *f_pidxRevInfo )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_pbRevInfo   != NULL );
    ChkArg( f_pRLVI       != NULL );
    ChkArg( f_pidxRevInfo != NULL );
    ChkArg( SIZEOF( DRM_RLVI_HEAD ) <= f_cbRevInfo );

    OEM_SECURE_ZERO_MEMORY( f_pRLVI, SIZEOF( DRM_RLVI ) );
    *f_pidxRevInfo = 0;

    /* ID */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.dwID, f_pbRevInfo, *f_pidxRevInfo );
    ChkArg( RLVI_MAGIC_NUM_V1 == f_pRLVI->head.dwID || RLVI_MAGIC_NUM_V2 == f_pRLVI->head.dwID );
    *f_pidxRevInfo += SIZEOF( DRM_DWORD );

    /* Length */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.cbSignedBytes, f_pbRevInfo, *f_pidxRevInfo );
    ChkArg( SIZEOF( DRM_RLVI_HEAD ) <= f_pRLVI->head.cbSignedBytes );
    *f_pidxRevInfo += SIZEOF( DRM_DWORD );

    /* Format Version */
    f_pRLVI->head.bFormatVersion = f_pbRevInfo[*f_pidxRevInfo];
    ChkArg( (RLVI_FORMAT_VERSION_V1 == f_pRLVI->head.bFormatVersion && f_pRLVI->head.dwID == RLVI_MAGIC_NUM_V1 )
        || ( RLVI_FORMAT_VERSION_V2 == f_pRLVI->head.bFormatVersion && f_pRLVI->head.dwID == RLVI_MAGIC_NUM_V2 ) );
    *f_pidxRevInfo += 1;

    /* Reserved */
    f_pRLVI->head.bReserved[0] = f_pbRevInfo[*f_pidxRevInfo];
    *f_pidxRevInfo += 1;
    f_pRLVI->head.bReserved[1] = f_pbRevInfo[*f_pidxRevInfo];
    *f_pidxRevInfo += 1;
    f_pRLVI->head.bReserved[2] = f_pbRevInfo[*f_pidxRevInfo];
    *f_pidxRevInfo += 1;

    /* Sequence Nubmer */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.dwRIV, f_pbRevInfo, *f_pidxRevInfo );
    *f_pidxRevInfo += SIZEOF( DRM_DWORD );

    /* Issued Time: Stored in little endian order for V1 and big endian for V2 */
    if( RLVI_MAGIC_NUM_V1 == f_pRLVI->head.dwID )
    {
        LITTLEENDIAN_BYTES_TO_DWORD( f_pRLVI->head.ftIssuedTime.dwLowDateTime, f_pbRevInfo, *f_pidxRevInfo );
        *f_pidxRevInfo += SIZEOF( DRM_DWORD );
        LITTLEENDIAN_BYTES_TO_DWORD( f_pRLVI->head.ftIssuedTime.dwHighDateTime, f_pbRevInfo, *f_pidxRevInfo );
        *f_pidxRevInfo += SIZEOF( DRM_DWORD );
    }
    else if( RLVI_MAGIC_NUM_V2 == f_pRLVI->head.dwID )
    {
        DRM_UINT64 ui64IssueTime = DRM_UI64LITERAL(0,0);

        NETWORKBYTES_TO_QWORD( ui64IssueTime, f_pbRevInfo, *f_pidxRevInfo );
        UI64_TO_FILETIME(ui64IssueTime, f_pRLVI->head.ftIssuedTime);
        *f_pidxRevInfo += SIZEOF( DRM_UINT64 );
    }
    else
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    /* Entry Count */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.dwRecordCount, f_pbRevInfo, *f_pidxRevInfo );
    *f_pidxRevInfo += SIZEOF( DRM_DWORD );

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   DRM_RVK_ParseRevocationInfo
**
** Synopsis :   Unpack revocation info into DRM_RLVI structure
**
** Arguments :  [f_pbRevInfo]      - rev info buffer (already b64 decoded)
**              [f_pcbRevInfo]     - size of rev info buffer
**              [f_pRLVI]          - Revocation Info info structure.
**              [f_pcbSignedBytes] - Returns the number of bytes signed by
**                                   the RIV signature.
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RVK_ParseRevocationInfo(
    __in_bcount( f_cbRevInfo ) const DRM_BYTE           *f_pbRevInfo,
    __in                             DRM_DWORD           f_cbRevInfo,
    __out_ecount(1)                  DRM_RLVI           *f_pRLVI,
    __out_ecount_opt(1)              DRM_DWORD          *f_pcbSignedBytes )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr                = DRM_SUCCESS;
    DRM_DWORD       cbRevInfo         = f_cbRevInfo;
    DRM_DWORD       cbEntries         = 0;
    DRM_DWORD       dwRevInfo         = 0;

    ChkArg( f_pbRevInfo != NULL
         && f_pRLVI     != NULL );
    ChkArg( SIZEOF( DRM_RLVI_HEAD ) <= f_cbRevInfo );

    ChkDR( DRM_RVK_ParseRevocationInfoHeader(
        f_pbRevInfo,
        f_cbRevInfo,
        f_pRLVI,
        &dwRevInfo ) );

    /* Entries */
    cbEntries = SIZEOF( DRM_RLVI_RECORD ) * f_pRLVI->head.dwRecordCount;
    ChkArg( f_cbRevInfo - dwRevInfo >= cbEntries + ( SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE ) + SIZEOF( DRM_DWORD ) );
    f_pRLVI->ibEntries = dwRevInfo;
    dwRevInfo += cbEntries;

    if( f_pcbSignedBytes != NULL )
    {
        *f_pcbSignedBytes = dwRevInfo;
    }

    /* Signature Type */
    f_pRLVI->signature.bSignatureType = f_pbRevInfo[dwRevInfo];
    dwRevInfo += SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE;
    if( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_1 )
    {
        ChkArg( RLVI_SIGNATURE_SIZE_1 + SIZEOF(DRM_DWORD) <= (cbRevInfo - dwRevInfo) );
        f_pRLVI->signature.cbSignature = RLVI_SIGNATURE_SIZE_1;
    }
    else if( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_2 )
    {
        NETWORKBYTES_TO_WORD( f_pRLVI->signature.cbSignature, f_pbRevInfo, dwRevInfo );
        dwRevInfo += SIZEOF( DRM_WORD );
        ChkArg( f_pRLVI->signature.cbSignature <= cbRevInfo - dwRevInfo );
    }
    else
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    /* Signature */
    f_pRLVI->signature.ibSignature = dwRevInfo;
    dwRevInfo += f_pRLVI->signature.cbSignature;

    /*
    ** Certificate Chain Length.
    ** For RLVI_SIGNATURE_TYPE_1 the size is written before the certificate.
    ** For RLVI_SIGNATURE_TYPE_2 the certificate is PlayReady cert and size is not written.
    */
    if ( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_1 )
    {
        NETWORKBYTES_TO_DWORD( f_pRLVI->certchain.cbCertChain, f_pbRevInfo, dwRevInfo );
        dwRevInfo += SIZEOF( DRM_DWORD );
    }
    else if( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_2 )
    {
        f_pRLVI->certchain.cbCertChain = f_cbRevInfo - dwRevInfo;
    }
    else
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    /* Certificate Chain */
    ChkArg( f_cbRevInfo - dwRevInfo >= f_pRLVI->certchain.cbCertChain );
    f_pRLVI->certchain.ibCertChain = dwRevInfo;

ErrorExit:

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

