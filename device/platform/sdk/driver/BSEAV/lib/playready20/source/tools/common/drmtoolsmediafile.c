/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <oempkcrypto.h>
#include <oemeccp256.h>
#include <drmlicense.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmenvelope.h>
#include <drmplayreadyobj.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmtoolsmediafile.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
**
** Function:    _IsEnvelope
**
** Synopsis:    Tests the first DWORD of the file pointer given to see if it matches
**              the signature of a PlayReady Envelope file.
**
** Arguments:
**
** [f_hFile]         -- an opened file handle
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
static DRM_BOOL DRM_CALL _IsEnvelope( OEM_FILEHDL f_hFile)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwSignature = 0;
    DRM_DWORD  cbRead      = 0;

    ChkArg( f_hFile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( Oem_File_Read( f_hFile, ( DRM_BYTE * )&dwSignature, sizeof( dwSignature ), &cbRead ), DRM_E_FAIL );

    FIX_ENDIAN_DWORD(dwSignature);

    ChkBOOL( dwSignature == DRM_ENVELOPE_FILE_SIGNATURE, DRM_E_FAIL );

ErrorExit:
    /*
    ** No matter what the result is, reset the file pointer to the beginning of the file
    */
    if( f_hFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_SetFilePointer( f_hFile, 0, OEM_FILE_BEGIN, NULL );
    }
    return DRM_SUCCEEDED( dr );
}

/*****************************************************************************
**
** Function:    _OpenEnvelope
**
** Synopsis:    Opens a file within the envelope archive.
**
** Arguments:
**
** [f_hFile]                    -- File Handle for reading.
** [f_pibDRMHeaderOffsetInFile] -- Returns the file offset to the header.
** [f_pEnvFile]                 -- Returns an initialize enveloped file context.
** [f_pbDRMHeaderData]          -- Buffer to holder the RM header.
** [f_cbDRMHeaderData]          -- Size of buffer to holder the RM header.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
static DRM_RESULT DRM_CALL _OpenEnvelope(
    IN                                  OEM_FILEHDL                  f_hFile,
       OUT                              DRM_DWORD                   *f_pibDRMHeaderOffsetInFile,
       OUT                              DRM_ENVELOPED_FILE_CONTEXT  *f_pEnvFile,
    __out_bcount( f_cbDRMHeaderData )   DRM_BYTE                    *f_pbDRMHeaderData,
    __in                                DRM_DWORD                    f_cbDRMHeaderData )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 cbRead               = 0;
    /* Temporary buffer to read the envelope header into. Due to slack in the struct, the
    ** actual envelope header could potentially be smaller than SIZEOF( DRM_ENVELOPE_HEADER )
    ** but should never be larger
    */
    DRM_BYTE rgbHeaderBuffer[SIZEOF( DRM_ENVELOPE_HEADER )] = {0};

    ChkArg( f_hFile    != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pibDRMHeaderOffsetInFile != NULL );
    ChkArg( f_pbDRMHeaderData != NULL );

    ZEROMEM( f_pEnvFile, SIZEOF( DRM_ENVELOPED_FILE_CONTEXT ) );
    f_pEnvFile->pEnvFileHandle = f_hFile;

    /*
    ** Read the file header.
    */
    ChkBOOL( Oem_File_Read(  f_pEnvFile->pEnvFileHandle,
                             rgbHeaderBuffer,
                             DRM_ENVELOPE_MINIMUM_HEADER_SIZE,
                            &cbRead ), DRM_E_FILEREADERROR );

    ChkBOOL( cbRead == DRM_ENVELOPE_MINIMUM_HEADER_SIZE, DRM_E_FILEREADERROR );



    /*
    ** Load the header from the byte buffer to the struct. Data isn't read directly into the
    ** struct with ReadFile due to the potential for different packing on varying compilers
    */
    cbRead = 0;
    LITTLEENDIAN_BYTES_TO_DWORD( f_pEnvFile->oEnvHeader.dwFileSignature, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_pEnvFile->oEnvHeader.cbHeaderSize, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_pEnvFile->oEnvHeader.dwFileDataOffset, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_WORD( f_pEnvFile->oEnvHeader.wFormatVersion, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_WORD( f_pEnvFile->oEnvHeader.wCompatibleVersion, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_pEnvFile->oEnvHeader.dwCipherType, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    DRM_BYT_CopyBytes( f_pEnvFile->oEnvHeader.rgbCipherData, 0, rgbHeaderBuffer, cbRead, DRM_ENVELOPE_CIPHER_DATA_SIZE );
    cbRead += DRM_ENVELOPE_CIPHER_DATA_SIZE;

    LITTLEENDIAN_BYTES_TO_WORD( f_pEnvFile->oEnvHeader.cbOriginalFilename, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_pEnvFile->oEnvHeader.cbDrmHeaderLen, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    /*
    ** Check the file signature, make sure reported header size is sane, and check that the version is OK.
    */
    ChkBOOL( f_pEnvFile->oEnvHeader.dwFileSignature == DRM_ENVELOPE_FILE_SIGNATURE, DRM_E_ENVELOPE_CORRUPT );
    ChkBOOL( f_pEnvFile->oEnvHeader.cbHeaderSize >= DRM_ENVELOPE_MINIMUM_HEADER_SIZE, DRM_E_ENVELOPE_CORRUPT );
    ChkBOOL( f_pEnvFile->oEnvHeader.wCompatibleVersion <= DRM_ENVELOPE_CURRENT_FORMAT_VERSION, DRM_E_ENVELOPE_FILE_NOT_COMPATIBLE );
    ChkBOOL( f_pEnvFile->oEnvHeader.dwFileDataOffset >= f_pEnvFile->oEnvHeader.cbHeaderSize, DRM_E_ENVELOPE_CORRUPT );

    /*
    ** Check that the header length is valid.
    */
    ChkBOOL( f_pEnvFile->oEnvHeader.cbDrmHeaderLen <= f_cbDRMHeaderData, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( f_pEnvFile->oEnvHeader.cbDrmHeaderLen % SIZEOF( DRM_WCHAR ) == 0, DRM_E_ENVELOPE_CORRUPT );

    /*
    ** Skip over the original filename.
    */
    ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                      f_pEnvFile->oEnvHeader.cbOriginalFilename,
                                      OEM_FILE_CURRENT,
                                      NULL ), DRM_E_FILESEEKERROR );

    /*
    ** Store the Location of the DRM Header.
    */
    *f_pibDRMHeaderOffsetInFile = f_pEnvFile->oEnvHeader.cbOriginalFilename + DRM_ENVELOPE_MINIMUM_HEADER_SIZE;

    /*
    ** Read the RM content header itself.
    */
    ChkBOOL( Oem_File_Read(  f_pEnvFile->pEnvFileHandle,
                             f_pbDRMHeaderData,
                             f_pEnvFile->oEnvHeader.cbDrmHeaderLen,
                            &cbRead ), DRM_E_FILEREADERROR );

    ChkBOOL( f_pEnvFile->oEnvHeader.cbDrmHeaderLen == cbRead, DRM_E_FILEREADERROR );

    /*
    ** Parse the cipher intiailization values.
    */
    switch ( f_pEnvFile->oEnvHeader.dwCipherType )
    {
        case eDRM_AES_COUNTER_CIPHER:
            /* Copy the initial AES counter */
            MEMCPY( ( DRM_BYTE * )&f_pEnvFile->qwInitialCipherCounter,
                    f_pEnvFile->oEnvHeader.rgbCipherData,
                    SIZEOF( DRM_UINT64 ) );

            FIX_ENDIAN_QWORD( f_pEnvFile->qwInitialCipherCounter );
            break;

        case eDRM_RC4_CIPHER:
            break;

        default:
            ChkDR( DRM_E_ENVELOPE_FILE_NOT_COMPATIBLE );
    }

    f_pEnvFile->dwFileDataStart = f_pEnvFile->oEnvHeader.dwFileDataOffset;

    /*
    ** Seek to the beginning of the file data.
    */
    if ( f_pEnvFile->dwFileDataStart > f_pEnvFile->oEnvHeader.cbHeaderSize )
    {
        ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                          f_pEnvFile->dwFileDataStart,
                                          OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );
    }

ErrorExit:

    return dr;
}

/*****************************************************************************
**
** Function:    DRMTOOLS_OpenMediaFile
**
** Synopsis:    Opens a Media File and initializes the context with information
**              read from the file.
**
** Arguments:
**
** [f_dstrFileName]         -- Pointer to a string contiaing the path to the file to read.
** [f_poMediaFile]          -- Pointer to an unopened media file context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_OpenMediaFile( IN     DRM_CONST_STRING       *f_dstrFileName,
                                                    IN OUT DRM_MEDIA_FILE_CONTEXT *f_poMediaFile )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;

    ChkArg( f_dstrFileName->cchString  != 0 );
    ChkArg( f_dstrFileName->pwszString != NULL );
    ChkArg( f_poMediaFile              != NULL );
    ChkArg( !f_poMediaFile->fOpened );

    hFile = Oem_File_Open( NULL,
                           f_dstrFileName->pwszString,
                           OEM_GENERIC_READ|OEM_GENERIC_WRITE,
                           OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE,
                           OEM_OPEN_EXISTING,
                           OEM_ATTRIBUTE_NORMAL );

    if( hFile == OEM_INVALID_HANDLE_VALUE )
    {
        /*
        ** If we failed to open the file let's see if we can open it just for reading.
        */
        hFile = Oem_File_Open( NULL,
                               f_dstrFileName->pwszString,
                               OEM_GENERIC_READ,
                               OEM_FILE_SHARE_READ,
                               OEM_OPEN_EXISTING,
                               OEM_ATTRIBUTE_NORMAL );

        ChkBOOL( hFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );
        f_poMediaFile->fReadOnly = TRUE;
    }
    else
    {
        f_poMediaFile->fReadOnly = FALSE;
    }

    if ( _IsEnvelope( hFile ) )
    {
        f_poMediaFile->eFileType = eDRM_MEDIA_FILE_TYPE_ENVELOPE;
        ChkDR( _OpenEnvelope( hFile,
                              &(f_poMediaFile->ibDRMHeaderOffsetInFile),
                              &(f_poMediaFile->FormatContext.oEnvFile),
                              f_poMediaFile->rgbDRMHeaderData,
                              SIZEOF( f_poMediaFile->rgbDRMHeaderData ) ) );
        f_poMediaFile->cbDRMHeaderData = f_poMediaFile->FormatContext.oEnvFile.oEnvHeader.cbDrmHeaderLen;
    }
    else
    {
        f_poMediaFile->eFileType = eDRM_MEDIA_FILE_TYPE_ASF;
        ChkDR( DRMTOOLS_OpenASFContext( hFile,
                                        &(f_poMediaFile->FormatContext.oAsfFile),
                                        f_poMediaFile->rgbDRMHeaderData,
                                        SIZEOF( f_poMediaFile->rgbDRMHeaderData ) ) );
        f_poMediaFile->cbDRMHeaderData = f_poMediaFile->FormatContext.oAsfFile.cbDRMHeader;
        f_poMediaFile->ibDRMHeaderOffsetInFile = f_poMediaFile->FormatContext.oAsfFile.ibDRMHeaderOffsetInFile;
    }
    f_poMediaFile->fOpened = TRUE;
ErrorExit:
    return dr;
}

/*****************************************************************************
**
** Function:    DRMTOOLS_CloseMediaFile
**
** Synopsis:    Closes the Media File context and associated handles.
**              This also releases any memory that needs to be released.
**
** Arguments:
**
** [f_poMediaFile]          -- Pointer to an opened media file context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_CloseMediaFile( IN OUT DRM_MEDIA_FILE_CONTEXT *f_poMediaFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( f_poMediaFile->fOpened );

    switch( f_poMediaFile->eFileType )
    {
    case eDRM_MEDIA_FILE_TYPE_ENVELOPE:
        ChkDR( Drm_Envelope_Close( &(f_poMediaFile->FormatContext.oEnvFile) ) );
        break;
    case eDRM_MEDIA_FILE_TYPE_ASF:
        ChkDR( DRMTOOLS_CloseASFContext( &(f_poMediaFile->FormatContext.oAsfFile) ) );
        break;
    default:
        ChkDR( DRM_E_FAIL );
    }
    ZEROMEM( f_poMediaFile->rgbDRMHeaderData, SIZEOF( f_poMediaFile->rgbDRMHeaderData ) );
    f_poMediaFile->cbDRMHeaderData = 0;
    f_poMediaFile->ibDRMHeaderOffsetInFile = 0;
    f_poMediaFile->eFileType = eDRM_MEDIA_FILE_TYPE_UNKNOWN;
    f_poMediaFile->fOpened = FALSE;
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_UpdatePlayReadyObject( IN DRM_MEDIA_FILE_CONTEXT *f_poMediaFile )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbWritten = 0;

    ChkBOOL( !f_poMediaFile->fReadOnly, DRM_E_FILEWRITEERROR );

    if( f_poMediaFile->eFileType == eDRM_MEDIA_FILE_TYPE_ENVELOPE )
    {
        fp = f_poMediaFile->FormatContext.oEnvFile.pEnvFileHandle;
    }
    else
    {
        fp = f_poMediaFile->FormatContext.oAsfFile.hFileIn;
    }
    ChkBOOL( Oem_File_SetFilePointer( fp, f_poMediaFile->ibDRMHeaderOffsetInFile, OEM_FILE_BEGIN, NULL ), DRM_E_FAIL );
    ChkBOOL( Oem_File_Write( fp, f_poMediaFile->rgbDRMHeaderData, f_poMediaFile->cbDRMHeaderData, &cbWritten )
          && cbWritten == f_poMediaFile->cbDRMHeaderData,
             DRM_E_FAIL );

ErrorExit:
    return dr;
}

/*****************************************************************************
**
** Function:    DRMTOOLS_SaveContentHeaderToFile
**
** Synopsis:    Saves the Content Header stored in the provided DRM_MEDIA_FILE_CONTEXT to a file
**
** Arguments:
**
** [f_poMediaFile]          -- Pointer to an opened media file context.
** [f_dstrOutFileName]      -- String containing the path to the file to which to write.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_SaveContentHeaderToFile( IN DRM_MEDIA_FILE_CONTEXT *f_poMediaFile,
                                                              IN DRM_CONST_STRING       *f_dstrOutFileName )
{
    DRM_RESULT dr              = DRM_SUCCESS;

    ChkArg( f_poMediaFile->fOpened );
    ChkArg( f_dstrOutFileName->pwszString != NULL );

    ChkDR( DRMTOOLS_WriteBufferToFile( f_dstrOutFileName, f_poMediaFile->rgbDRMHeaderData, f_poMediaFile->cbDRMHeaderData ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
