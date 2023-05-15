/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMBCRLPARSER_C

#include <drmutftypes.h>
#include <oemparsers.h>
#include <drmerr.h>
#include <drmprofile.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function:    DRM_BCrl_ParseUnsignedCrl
**
** Synopsis:    Parses the unsigned portion of Playready CRL from binary to data structure.
**
** Arguments:   [f_pbCrlData]  : A pointer to the raw binary CRL data
**              [f_cbCrlData]  : Number of bytes in the raw binary CRL data
**              [f_cbSignedMessageLength] : Total length of the signed portion of data
**              [f_poCrl]      : A pointer to the structure to hold the parsed CRL data
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm or the CRL data is NULL
**
** Notes:       The parser does not make copies of the DRM_RevocationEntry
**              data, it just points to them in the f_pbCrlData buffer.  Thus
**              the caller cannot free the f_pbCrlData buffer and still have a
**              valid f_poCrl data structure.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_ParseUnsignedCrl(
    __in_bcount(f_cbCrlData) const DRM_BYTE       *f_pbCrlData,
    __in                     const DRM_DWORD       f_cbCrlData,
    __out                    DRM_DWORD            *f_pcbSignedMessageLength,
    __out                    DRM_BCRL             *f_poCrl )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;

    ChkArg( f_poCrl != NULL );
    ChkArg( f_pbCrlData != NULL);
    ChkArg( f_pcbSignedMessageLength != NULL);

    /*
    ** Retrieving crl header
    */
    COPY_FROMBUFFER( f_poCrl->Identifier,
                     f_pbCrlData,
                     dwOffset,
                     SIZEOF(f_poCrl->Identifier),
                     f_cbCrlData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCrl->dwVersion,
                                      f_pbCrlData,
                                      dwOffset,
                                      f_cbCrlData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCrl->cRevocationEntries,
                                      f_pbCrlData,
                                      dwOffset,
                                      f_cbCrlData );
    /*
    ** Retrieving revocation entries
    */
    if ( f_poCrl->cRevocationEntries > 0 )
    {
        ChkBoundsLT( dwOffset, f_cbCrlData );
        f_poCrl->Entries = (DRM_RevocationEntry*) ( f_pbCrlData + dwOffset );
    }
    else
    {
        f_poCrl->Entries = NULL;
    }
    ChkOverflow( f_poCrl->cRevocationEntries * SIZEOF( DRM_RevocationEntry ),
                 f_poCrl->cRevocationEntries );
    ChkOverflow( dwOffset + f_poCrl->cRevocationEntries * SIZEOF( DRM_RevocationEntry ),
                 f_poCrl->cRevocationEntries * SIZEOF( DRM_RevocationEntry ) );
    dwOffset += f_poCrl->cRevocationEntries * SIZEOF( DRM_RevocationEntry );

    ChkBoundsLT( dwOffset, f_cbCrlData );

    *f_pcbSignedMessageLength = dwOffset;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    DRM_BCrl_ParseSignedCrl
**
** Synopsis:    Parses Playready CRL from binary to data structure up to its
**              signature. This function does NOT verify the CRL signature
**              itself - use DRM_BCrl_ParseCrl for that functionality
**
** Arguments:   [f_pbCrlData]  : A pointer to the raw binary CRL data
**              [f_cbCrlData]  : Number of bytes in the raw binary CRL data
**              [f_pcbSignedMessageLength]: Returns the length of data
**                                          that is signed.
**              [f_poCrl]      : A pointer to the structure to hold the parsed CRL data
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm or the CRL data is NULL
**
** Notes:       The parser does not make copies of the DRM_RevocationEntry
**              data, it just points to them in the f_pbCrlData buffer.  Thus
**              the caller cannot free the f_pbCrlData buffer and still have a
**              valid f_poCrl data structure.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCrl_ParseSignedCrl(
    __in_bcount(f_cbCrlData) const DRM_BYTE        *f_pbCrlData,
    __in                     const DRM_DWORD        f_cbCrlData,
    __out_ecount_opt(1)            DRM_DWORD       *f_pcbSignedMessageLength,
    __out                          DRM_BCRL_Signed *f_poCrl )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;
    DRM_DWORD dwSignedMessageLength = 0;

    ChkArg( f_poCrl != NULL );
    ChkArg( f_pbCrlData != NULL);

    ChkDR( DRM_BCrl_ParseUnsignedCrl( f_pbCrlData,
                                      f_cbCrlData,
                                      &dwSignedMessageLength,
                                      &f_poCrl->Crl ) );

    dwOffset = dwSignedMessageLength;

    /*
    ** Retrieving signature
    */
    COPY_FROMBUFFER( f_poCrl->Signature.type,
                     f_pbCrlData,
                     dwOffset,
                     SIZEOF( f_poCrl->Signature.type ),
                     f_cbCrlData );

    ChkArg( PLAYREADY_DRM_BCRL_SIGNATURE_TYPE == f_poCrl->Signature.type );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poCrl->Signature.cb,
                                     f_pbCrlData,
                                     dwOffset,
                                     f_cbCrlData );

    ChkArg( SIZEOF( SIGNATURE_P256 ) == f_poCrl->Signature.cb );

    COPY_FROMBUFFER( f_poCrl->Signature.rgb,
                     f_pbCrlData,
                     dwOffset,
                     f_poCrl->Signature.cb,
                     f_cbCrlData );

    /*
    ** Retrieving certificate chain
    */
    ChkBoundsLT( dwOffset, f_cbCrlData );
    f_poCrl->pbCertificateChain = (DRM_BYTE*)( f_pbCrlData + dwOffset );
    f_poCrl->cbCertificateChain = f_cbCrlData - dwOffset; /*this is verified below */

    if( f_pcbSignedMessageLength != NULL )
    {
        *f_pcbSignedMessageLength = dwSignedMessageLength;
    }

ErrorExit:
    return dr;
}
EXIT_PK_NAMESPACE_CODE;

