/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>

#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmconstants.h>
#include <drmcmdlnpars.h>
#include <oempkcrypto.h>
#include <oemeccp256.h>
#include <drmlicense.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmplayreadyobj.h>
#include <drmmathsafe.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmtoolsasfparser.h>

ENTER_PK_NAMESPACE_CODE;

/*
** These two macros do some useful operations related to 64 bit integers.
*/

#define SAFE_ADD_UI64( a, b, c ) do {              \
        ChkOverflowUI64( DRM_UI64Add( a, b ), a ); \
        c = DRM_UI64Add( a, b );                   \
    } while( FALSE )

#define SAFE_SUB_UI64( a, b, c ) do {              \
        ChkOverflowUI64( a, DRM_UI64Sub( a, b ) ); \
        c = DRM_UI64Sub( a, b );                   \
    } while( FALSE )

/*********************************************************************
**
** Function: _ReadASFObjectBasicInfo
**
** Synopsis: Reads the GUID and size of an ASF object.
**           Format of all ASF objects:
**              Object GUID     [16 bytes]
**              Object Size     [8 bytes]     must be >= 24
**              Object Data     [N bytes, N = Object Size - 24]
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFObjectBasicInfo( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           cbRead          = 0;

    ChkArg( f_poAsfContext != NULL );

    /* Read the Object ID GUID */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &(f_poAsfContext->guidCurObjectID), SIZEOF( DRM_GUID ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_GUID) )
    {
        printf( "Failed to read the header object ID.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ), f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_GUID(f_poAsfContext->guidCurObjectID);

    /* Read the object size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &(f_poAsfContext->qwCurObjSize), SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "Failed to read the header object size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ), f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(f_poAsfContext->qwCurObjSize);

    if ( f_poAsfContext->fVerbose )
    {
        printf( "\n Header Object: %08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X\n Has Byte Size: 0X%08X%08X.\n",
                f_poAsfContext->guidCurObjectID.Data1,
                f_poAsfContext->guidCurObjectID.Data2,
                f_poAsfContext->guidCurObjectID.Data3,
                f_poAsfContext->guidCurObjectID.Data4[0],
                f_poAsfContext->guidCurObjectID.Data4[1],
                f_poAsfContext->guidCurObjectID.Data4[2],
                f_poAsfContext->guidCurObjectID.Data4[3],
                f_poAsfContext->guidCurObjectID.Data4[4],
                f_poAsfContext->guidCurObjectID.Data4[5],
                f_poAsfContext->guidCurObjectID.Data4[6],
                f_poAsfContext->guidCurObjectID.Data4[7],
                (DRM_DWORD)DRM_UI64High32(f_poAsfContext->qwCurObjSize),
                (DRM_DWORD)DRM_UI64Low32(f_poAsfContext->qwCurObjSize) );
    }
ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _SkipData
**
** Synopsis: Skips data in the ASF context. It takes the size to skip as a QWORD
**
** Arguments:
**
** [f_poAsfContext]               -- The parser context.
** [f_qwSkip]                     -- The amount of data to skip.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _SkipData( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext, __in DRM_UINT64 f_qwSkip )
{
    DRM_RESULT      dr               = DRM_SUCCESS;

    ChkArg( f_poAsfContext  != NULL );

    /*
    ** Loop through reading in blocks of size LONG_MAX until
    ** the remaining size to skip is less than LONG_MAX
    */
    while ( DRM_UI64High32( f_qwSkip ) > 0 )
    {
        ChkBOOL( Oem_File_SetFilePointer( f_poAsfContext->hFileIn, LONG_MAX, OEM_FILE_CURRENT, NULL ), DRM_E_FILE_READ_ERROR );

        /* Reduce oObjDataSize to reflect how much data has already been skipped */
        SAFE_SUB_UI64( f_qwSkip, DRM_UI64( LONG_MAX ), f_qwSkip );

        /* Update the total number of bytes read */
        SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( LONG_MAX ), f_poAsfContext->qwReadTotal );

    }

    {
        DRM_LONG lSkip = 0;
        ChkDR( DRM_DWordToLong( DRM_UI64Low32( f_qwSkip ), &lSkip ) );
        ChkBOOL( Oem_File_SetFilePointer( f_poAsfContext->hFileIn, lSkip, OEM_FILE_CURRENT, NULL ), DRM_E_FILE_READ_ERROR );
    }

    /* Update the total number of bytes read */
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, f_qwSkip, f_poAsfContext->qwReadTotal );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _SkipASFObject
**
** Synopsis: Reads and discards the data from an ASF object.
**           The Object GUID and size are assumed to have been read already.
**           Format of all ASF objects:
**              Object GUID     [16 bytes]
**              Object Size     [8 bytes]     must be >= 24
**              Object Data     [N bytes, N = Object Size - 24]
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _SkipASFObject( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT      dr               = DRM_SUCCESS;
    DRM_UINT64      qwObjDataSize    = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64      qwAsfObjMinSize  = ASF_OBJ_MIN_SIZE;

    ChkArg( f_poAsfContext  != NULL );

    /* Determine size of data which needs to be skipped */
    SAFE_SUB_UI64( f_poAsfContext->qwCurObjSize, qwAsfObjMinSize, qwObjDataSize );

    ChkDR( _SkipData( f_poAsfContext, qwObjDataSize ) );

ErrorExit:
    if ( DRM_FAILED( dr ) && f_poAsfContext != NULL )
    {
        printf( "Failed to skip the data of header object %08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X.\n",
                f_poAsfContext->guidCurObjectID.Data1,
                f_poAsfContext->guidCurObjectID.Data2,
                f_poAsfContext->guidCurObjectID.Data3,
                f_poAsfContext->guidCurObjectID.Data4[0],
                f_poAsfContext->guidCurObjectID.Data4[1],
                f_poAsfContext->guidCurObjectID.Data4[2],
                f_poAsfContext->guidCurObjectID.Data4[3],
                f_poAsfContext->guidCurObjectID.Data4[4],
                f_poAsfContext->guidCurObjectID.Data4[5],
                f_poAsfContext->guidCurObjectID.Data4[6],
                f_poAsfContext->guidCurObjectID.Data4[7] );
    }
    return dr;
}

/*********************************************************************
**
** Function: _ReadASFV4ContentEncryptionObject
**
** Synopsis: Reads the V4 Content Encryption Object.
**           The Object GUID and size are assumed to have been read already.
**           Format of  V4 Content Encryption Object:
**              Object GUID     [16 bytes]
**              Object Size     [8 bytes]     must be >= 24
**
**              System GUID              [16 bytes]
**              System Version           [4 bytes]
**              Data Size                [4 bytes]
**              Data                     [varies]
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
** [f_pbDRMHeaderData]                  -- buffer to hold the DRM Header if it's found.
** [f_cbDRMHeaderData]                  -- Size of buffer to hold the DRM header if it's found.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFV4ContentEncryptionObject(
    __inout                             DRM_TOOLS_ASF_CONTEXT *f_poAsfContext,
    __out_bcount( f_cbDRMHeaderData )   DRM_BYTE              *f_pbDRMHeaderData,
    __in                                DRM_DWORD              f_cbDRMHeaderData )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_GUID            guidSystemID    = EMPTY_DRM_GUID;
    DRM_DWORD           dwSystemVersion = 0;
    DRM_DWORD           cbDataSize      = 0;
    DRM_DWORD           cbRead          = 0;

    ChkArg( f_poAsfContext  != NULL );
    ChkArg( f_pbDRMHeaderData != NULL && f_cbDRMHeaderData > 0 );

    /*
    ** Read the system ID field
    */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &guidSystemID, SIZEOF( DRM_GUID ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_GUID ) )
    {
        printf( "\t\tFailed to read the first reserved field of the header extension object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_GUID(guidSystemID);

    /*
    ** Check that the system ID matches the PlayReady system ID
    */
    if ( MEMCMP( &guidSystemID, &g_guidSystemIDPlayReady, SIZEOF( DRM_GUID ) ) != 0 )
    {
        printf( "\t\tV4 content encryption object does not contain PlayReady system ID.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /*
    ** Read the system version
    */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &dwSystemVersion, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the second reserved byte of the header object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(dwSystemVersion);

    if ( dwSystemVersion != 1 )
    {
        printf( "\t\tUnknown PlayReady system version number.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /*
    ** Read the data size
    */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbDataSize, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the V4 content encryption data size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbDataSize);

    /*
    ** Read in the playready header object to a byte buffer
    */
    ChkBOOL( f_cbDRMHeaderData >= cbDataSize, DRM_E_BUFFERTOOSMALL );
    ZEROMEM( f_pbDRMHeaderData,  f_cbDRMHeaderData  );
    if ( Oem_File_Read( f_poAsfContext->hFileIn, f_pbDRMHeaderData, cbDataSize, &cbRead ) == FALSE ||
         cbRead != cbDataSize )
    {
        printf( "\t\tFailed to read playready header object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    f_poAsfContext->ibDRMHeaderOffsetInFile = DRM_UI64Low32( f_poAsfContext->qwReadTotal );
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );
    f_poAsfContext->cbDRMHeader = cbDataSize;

    f_poAsfContext->oFoundObjects.fFoundV4 = TRUE;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _ReadASFHeaderObjectInExtension
**
** Synopsis: Reads basic info of an ASF object in the ASF Header Extension.
**           If the object ID is known, this will call the appropriate parsing function.
**           If the object ID is not known, this will skip past the object data
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFHeaderObjectInExtension( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT          dr              = DRM_SUCCESS;

    ChkArg( f_poAsfContext != NULL );

    /* Read the ASF Object ID and Size */
    ChkDR( _ReadASFObjectBasicInfo( f_poAsfContext ) );

    /* See if the object ID matches one of the objects we care about, otherwise skip it */
    if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFAdvancedContentEncryptionObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        printf( "\t\tFound Advanced Content Encryption Object!  This contains the V3 header, which this tools currently skips.\n" );
        ChkDR ( _SkipASFObject( f_poAsfContext ) );
    }
    else
    {
        ChkDR ( _SkipASFObject( f_poAsfContext ) );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _ReadASFHeaderExtensionObject
**
** Synopsis: Reads data of the ASF Header Extension Object, and then reads through
**           all ASF objects contained in the ASF Header Extension Object.
**           Format of ASF Header Extension Object (GUID and size should already be read)
**              Object GUID     [16 bytes]
**              Object Size     [8 bytes]     must be >= 24
**
**              Reserved 1               [16 bytes] must be ABD3D211-A9BA-11cf-8EE6-00C00C205365
**              Reserved 2               [2 bytes]  must be 6
**              Data Size                [4 bytes]  must be Object Size - 46
**              Data                     [varies]
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFHeaderExtensionObject( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           cbRead          = 0;
    DRM_UINT64          qwHeaderExtensionSize = DRM_UI64LITERAL( 0, 0 );
    DRM_GUID            guidHdrExtRes1  = EMPTY_DRM_GUID;
    DRM_WORD            wHdrExtRes2     = 0;
    DRM_DWORD           cbDataSize      = 0;
    DRM_UINT64          qwEstDataSize   = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64          qwEstReadSize   = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64          qwAsfHdrextMinSize  = ASF_HDREXT_MIN_SIZE;


    ChkArg( f_poAsfContext != NULL );

    /* Store the header extension object size locally */
    qwHeaderExtensionSize = f_poAsfContext->qwCurObjSize;

    /* Check that header object size is not too small */
    SAFE_SUB_UI64( qwHeaderExtensionSize, qwAsfHdrextMinSize, qwEstDataSize );

    /* Read the first reserved field */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &guidHdrExtRes1, SIZEOF( DRM_GUID ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_GUID ) )
    {
        printf( "\t\tFailed to read the first reserved field of the header extension object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_GUID(guidHdrExtRes1);

    /* Check that the first reserved guid matches expected value */
    if ( MEMCMP( &guidHdrExtRes1, &g_guidASFReserved1, SIZEOF( DRM_GUID ) ) != 0 )
    {
        printf( "\t\tFirst reserved field of the ASF header extension object does not match ASF spec requirements.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Read the second reserved field */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &wHdrExtRes2, SIZEOF( DRM_WORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_WORD ) )
    {
        printf( "\t\tFailed to read the second reserved field of the header extension object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_WORD(wHdrExtRes2);

    if ( wHdrExtRes2 != 6 )
    {
        printf( "\t\tSecond reserved field of the ASF header extension object does not match ASF spec requirements.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }


    /* Read the data size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbDataSize, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the header extension data size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbDataSize);

    if ( !DRM_UI64Eql( qwEstDataSize, DRM_UI64( cbDataSize ) ) )
    {
        printf( "\t\tASF Header extension data size and object size are incompatable.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Check the header object flags */
    if ( f_poAsfContext->oFoundObjects.fFoundHdrExt == TRUE )
    {
        printf( "\t\tWARNING  This file contains multiple header extension objects.\n" );
    }
    f_poAsfContext->oFoundObjects.fFoundHdrExt = TRUE;
    if ( ( f_poAsfContext->oFoundObjects.fFoundFileProperties != TRUE || f_poAsfContext->oFoundObjects.fFoundDigitalSignature != FALSE ) && f_poAsfContext->fVerbose )
    {
        printf( "\t\tWARNING  The header extension object is unsigned.\n" );
    }

    /* Read the objects within the header */
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbDataSize ), qwEstReadSize );

    while( DRM_UI64Les( f_poAsfContext->qwReadTotal, qwEstReadSize ) )
    {
        ChkDR( _ReadASFHeaderObjectInExtension( f_poAsfContext ) );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _ReadASFContentEncryptionObject
**
** Synopsis: Reads data of the ASF Content Encryption Object.
**           Format of ASF Content Encryption Object (GUID and Size should already be read)
**              Object GUID     [16 bytes]
**              Object Size     [8 bytes]     must be >= 24
**
**              Secret Data Length       [4 bytes]
**              Secret Data              [varies]
**              Protection Type Length   [4 bytes]
**              Protection Type          [varies]
**              Key ID Length            [4 bytes]
**              Key ID                   [varies]
**              License URL Length       [4 bytes]
**              License URL              [varies]
**
** Arguments:
**
** [f_poAsfContext]                    -- Parser Context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFContentEncryptionObject( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT          dr                   = DRM_SUCCESS;
    DRM_DWORD           cbRead               = 0;
    DRM_DWORD           cbSecretData         = 0;
    DRM_DWORD           cbProtectionType     = 0;
    DRM_BYTE            *pbProtectionType    = NULL;
    DRM_DWORD           cbKID                = 0;
    DRM_DWORD           cbURL                = 0;
    DRM_UINT64          qwV1MinSize          = ASF_V1_MIN_SIZE;

    ChkArg( f_poAsfContext != NULL );


    /* Check that header object size is not too small */
    if ( DRM_UI64Les( f_poAsfContext->qwCurObjSize, qwV1MinSize ) )
    {
        printf( "\t\tASF Header Content Encryption Object size is too small.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Read the secret data size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbSecretData, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the content encryption (V1 header) secret data size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbSecretData);

    f_poAsfContext->oV1Data.cbSecretData = cbSecretData;


    /* Read the secret data */
    ChkMem( f_poAsfContext->oV1Data.pbSecretData = ( DRM_BYTE * )Oem_MemAlloc( cbSecretData ) );
    if ( Oem_File_Read( f_poAsfContext->hFileIn, ( DRM_VOID * ) f_poAsfContext->oV1Data.pbSecretData, cbSecretData, &cbRead ) == FALSE ||
         cbRead != cbSecretData )
    {
        printf( "\t\tFailed to read content encryption (V1 header) secret data.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    /* Read the protection data size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbProtectionType, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the content encryption (V1 header) protection type size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbProtectionType);

    /* Read the protection data */
    ChkMem( pbProtectionType = ( DRM_BYTE * )Oem_MemAlloc( cbProtectionType ) );
    if ( Oem_File_Read( f_poAsfContext->hFileIn, pbProtectionType, cbProtectionType, &cbRead ) == FALSE ||
         cbRead != cbProtectionType )
    {
        printf( "\t\tFailed to read content encryption (V1 header) protection type.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    /* Read the KID data size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbKID, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the content encryption (V1 header) KID size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbKID);

    f_poAsfContext->oV1Data.cbKeyID = cbKID;


    /* Read the KID */
    ChkMem( f_poAsfContext->oV1Data.pbKeyID = ( DRM_BYTE * )Oem_MemAlloc( cbKID ) );
    if ( Oem_File_Read( f_poAsfContext->hFileIn, ( DRM_VOID * ) f_poAsfContext->oV1Data.pbKeyID, cbKID, &cbRead ) == FALSE ||
         cbRead != cbKID )
    {
        printf( "\t\tFailed to read content encryption (V1 header) KID.\n" );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    /* Read the URL data size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbURL, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the content encryption (V1 header) URL size.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbURL);

    f_poAsfContext->oV1Data.cbURL = cbURL;

    /* Read the URL data */
    ChkMem( f_poAsfContext->oV1Data.pbURL = ( DRM_BYTE * )Oem_MemAlloc( cbURL ) );
    if ( Oem_File_Read( f_poAsfContext->hFileIn, ( DRM_VOID * ) f_poAsfContext->oV1Data.pbURL, cbURL, &cbRead ) == FALSE ||
         cbRead != cbURL )
    {
        printf( "\t\tFailed to read content encryption (V1 header) URL.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    /* Check the header object flags */
    if ( f_poAsfContext->oFoundObjects.fFoundV1 == TRUE )
    {
        printf( "\t\tWARNING  This file contains multiple V1 headers.\n" );
    }
    f_poAsfContext->oFoundObjects.fFoundV1 = TRUE;
    if ( f_poAsfContext->oFoundObjects.fFoundFileProperties != TRUE || f_poAsfContext->oFoundObjects.fFoundDigitalSignature != FALSE )
    {
        printf( "\t\tWARNING  The V1 header is outside of the signed portion of the header.\n" );
    }

ErrorExit:
    SAFE_OEM_FREE( pbProtectionType );
    if( !DRM_SUCCEEDED( dr ) )
    {
        if( f_poAsfContext != NULL )
        {
            SAFE_OEM_FREE( f_poAsfContext->oV1Data.pbKeyID );
            SAFE_OEM_FREE( f_poAsfContext->oV1Data.pbURL );
            SAFE_OEM_FREE( f_poAsfContext->oV1Data.pbSecretData );
        }
    }
    return dr;
}

/*********************************************************************
**
** Function: _ReadASFExtendedContentEncryptionObject
**
** Synopsis: Reads data of the ASF Extended Content Encryption Object.
**           Format of ASF Extended Content Encryption Object (GUID and size should already be read)
**              Object GUID     [16 bytes]
**              Object Size     [8 bytes]     must be >= 24

**              Data Size                [4 bytes]  must be Object Size - 24
**              Data                     [varies]
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
** [f_pbDRMHeaderData]                  -- buffer to hold the DRM Header if it's found.
** [f_cbDRMHeaderData]                  -- Size of buffer to hold the DRM header if it's found.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFExtendedContentEncryptionObject(
    __inout                             DRM_TOOLS_ASF_CONTEXT *f_poAsfContext,
    __out_bcount( f_cbDRMHeaderData )   DRM_BYTE              *f_pbDRMHeaderData,
    __in                                DRM_DWORD              f_cbDRMHeaderData )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           cbRead          = 0;
    DRM_WORD            wTemp           = 0;

    ChkArg( f_poAsfContext != NULL );
    ChkArg( f_pbDRMHeaderData != NULL && f_cbDRMHeaderData > 0 );

    /* Read the data size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &f_poAsfContext->cbDRMHeader, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the extended content encryption data size (V2 header).\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(f_poAsfContext->cbDRMHeader);

    /* I can't find the spec covering V2 header in ASF, but all the old test contents
    ** have two leading bytes before the XML, so I am assuming this is expected. */
    ChkDR( f_poAsfContext->cbDRMHeader > 2);
    /* Skip the next word to get to the XML header. */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &wTemp, SIZEOF( DRM_WORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_WORD ) )
    {
        printf( "\t\tFailed to read the first 2 bytes of data in extended content encryption (V2 header).\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_WORD(wTemp);

    /* reduce f_poAsfContext->cbDRMHeader by two */
    f_poAsfContext->cbDRMHeader = f_poAsfContext->cbDRMHeader - 2;

    /* Read the V2 XML header */
    ChkBOOL( f_cbDRMHeaderData >= f_poAsfContext->cbDRMHeader , DRM_E_BUFFERTOOSMALL );
    ZEROMEM( f_pbDRMHeaderData,  f_cbDRMHeaderData  );
    if ( Oem_File_Read( f_poAsfContext->hFileIn, f_pbDRMHeaderData, f_poAsfContext->cbDRMHeader, &cbRead ) == FALSE ||
         cbRead != f_poAsfContext->cbDRMHeader )
    {
        printf( "\t\tFailed to read V2 XML header.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    /* Check the header object flags */
    if ( f_poAsfContext->oFoundObjects.fFoundV2 == TRUE )
    {
        printf( "\t\tWARNING  This file contains multiple V2 headers.\n" );
    }
    f_poAsfContext->oFoundObjects.fFoundV2 = TRUE;
    if ( ( f_poAsfContext->oFoundObjects.fFoundFileProperties != TRUE || f_poAsfContext->oFoundObjects.fFoundDigitalSignature != FALSE ) && f_poAsfContext->fVerbose )
    {
        printf( "\t\tWARNING  The V2 header is outside of the signed portion of the header.\n" );
    }

ErrorExit:
    return dr;

}

/*********************************************************************
**
** Function: _ReadASFFilePropertiesObject
**
** Synopsis: Reads data of the ASF File Properties Object.
**           Format of ASF File Properties Object (GUID and size should already be read)
**              Object GUID               [16 bytes]
**              Object Size               [8 bytes]
**
**              File GUID                 [16 bytes]
**              File Size                 [8 bytes]
**              Creation Date             [8 bytes]
**              Packet Count              [8 bytes]
**              Play Duration             [8 bytes]
**              Send Duration             [8 bytes]
**              Preroll                   [8 bytes]
**              Flags                     [4 bytes]
**              Minimum Data Packet Size  [4 bytes]
**              Maximum Data Packet Size  [4 bytes]
**              Maximum Bitrate           [4 bytes]
**
**
** Arguments:
**
** [f_poAsfContext]                     -- The parser context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFFilePropertiesObject( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_DWORD           cbRead              = 0;
    DRM_UINT64          qwTemp              = DRM_UI64LITERAL(0, 0);
    DRM_DWORD           dwTemp              = 0;
    DRM_DWORD           cbMinPacketSz       = 0;
    DRM_DWORD           cbMaxPacketSz       = 0;
    DRM_UINT64          qwAsfFilePropMinSize= ASF_FILEPROP_MIN_SIZE;

    ChkArg( f_poAsfContext != NULL );

    /* Check that header object size is not too small */
    if ( DRM_UI64Les( f_poAsfContext->qwCurObjSize, qwAsfFilePropMinSize ) )
    {
        printf( "\t\tASF File Properties Object is too small (read as %ld, but min size is %ld).\n", DRM_UI64Low32( f_poAsfContext->qwCurObjSize ), DRM_UI64Low32( qwAsfFilePropMinSize ) );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Read the File GUID */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &(f_poAsfContext->guidFileID), SIZEOF( DRM_GUID ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_GUID ) )
    {
        printf( "\t\tFailed to read the File GUID from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_GUID(f_poAsfContext->guidFileID);

    /* Read the File Size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &(f_poAsfContext->qwFileSize), SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "\t\tFailed to read the File Size from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(f_poAsfContext->qwFileSize);

    /* Skip the Creation Date */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &qwTemp, SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "\t\tFailed to read the Creation Date from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(qwTemp);

    /* Read the Packet Count */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &(f_poAsfContext->qwPacketCount), SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "\t\tFailed to read the Packet Count from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(f_poAsfContext->qwPacketCount);

    /* Skip the Play Duration */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &qwTemp, SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "\t\tFailed to read the Play Duration from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(qwTemp);

    /* Skip the Send Duration */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &qwTemp, SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "\t\tFailed to read the Send Duration from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(qwTemp);

    /* Skip the Preroll */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &qwTemp, SIZEOF( DRM_UINT64 ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_UINT64 ) )
    {
        printf( "\t\tFailed to read the Preroll from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_QWORD(qwTemp);

    /* Skip the Flags */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &dwTemp, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the Flags from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(dwTemp);

    /* Read the Min Packet Size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbMinPacketSz, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the Min Packet Size from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbMinPacketSz);

    /* Read the Max Packet Size */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &cbMaxPacketSz, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the Max Packet Size from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(cbMaxPacketSz);

    /* Skip the Max Bitrate */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &dwTemp, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the Max Bitrate from File Properties Object.\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(dwTemp);

    /* This tool only likes fixed packet sizes.  Check that this is the case */
    if ( cbMaxPacketSz != cbMinPacketSz )
    {
        printf( "\t\tVariable packet sizes are not supported.  This file has packet size range of %d-%d.\n", cbMinPacketSz, cbMaxPacketSz );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    f_poAsfContext->cbPacketSize = cbMinPacketSz;

ErrorExit:

    return dr;
}

static DRM_RESULT _ReadASFStreamPropertiesObject( __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext )
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_GUID   guidStreamType        = EMPTY_DRM_GUID;
    DRM_WORD   wFlags                = 0;
    DRM_WORD   wEncryptedContentFlag = 32768;
    DRM_WORD   wMaxStreamNumber      = 127;
    DRM_WORD   wFormatTag            = 0;
    DRM_DWORD  cbRead                = 0;
    DRM_DWORD  dwTypeSpecificData    = 0;
    DRM_UINT64 qwReadBeginning       = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 qwSkip                = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 qwAsfStrPropMinSize   = ASF_STREAM_PROPERTIES_MIN_SIZE;
    DRM_UINT64 qwAsfStrPropSkipSize  = ASF_STREAM_PROPERTIES_SKIP_SIZE;
    DRM_UINT64 qwAsfObjMinSize       = ASF_OBJ_MIN_SIZE;

    ChkArg( f_poAsfContext != NULL );

    /* Check that header object size is not too small */
    if ( DRM_UI64Les( f_poAsfContext->qwCurObjSize, qwAsfStrPropMinSize ) )
    {
        printf( "\t\tASF Stream Properties Object is too small (read as %ld, but min size is %ld).\n", DRM_UI64Low32( f_poAsfContext->qwCurObjSize ), DRM_UI64Low32( qwAsfStrPropMinSize ) );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    qwReadBeginning = f_poAsfContext->qwReadTotal;

    /* Read the stream type */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &guidStreamType, SIZEOF( DRM_GUID ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_GUID ) )
    {
        printf( "\t\tFailed to read stream type from ASF Stream Properties Object.\n" );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_GUID(guidStreamType);

    /*
    ** Skip fields that don't matter right now
    */
    ChkDR( _SkipData( f_poAsfContext, qwAsfStrPropSkipSize ) );

    /*
    ** Read the length of the Type Specific Data
    */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &dwTypeSpecificData, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\t\tFailed to read the length of the Type Specific Data within the ASF Stream Properties Object.\n" );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(dwTypeSpecificData);

    /*
    ** Skip the length of the error correction data
    */
    ChkDR( _SkipData( f_poAsfContext, DRM_UI64( SIZEOF( DRM_DWORD ) ) ) );

    /*
    ** Read the flags
    */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &wFlags, SIZEOF( DRM_WORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_WORD ) )
    {
        printf( "\t\tFailed to read the flags within the ASF Stream Properties Object.\n" );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_WORD(wFlags);

    /*
    ** Skip the reserved field
    */
    ChkDR( _SkipData( f_poAsfContext, DRM_UI64( SIZEOF( DRM_DWORD ) ) ) );

    /*
    ** Read the stream specific info to get the format information
    */
    if ( MEMCMP( &guidStreamType, &(g_guidASFXStreamTypeAcmAudio), SIZEOF( DRM_GUID ) ) == 0 )
    {
        /*
        ** Read the Format Tag
        */
        if ( Oem_File_Read( f_poAsfContext->hFileIn, &wFormatTag, SIZEOF( DRM_WORD ), &cbRead ) == FALSE ||
             cbRead != SIZEOF( DRM_WORD ) )
        {
            printf( "\t\tFailed to read the format tag within the ASF Stream Properties Object.\n" );
            ChkDR( DRM_E_FILE_READ_ERROR );
        }
        SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

        FIX_ENDIAN_WORD(wFormatTag);
    }
    else if ( MEMCMP( &guidStreamType, &(g_guidASFXStreamTypeIcmVideo), SIZEOF( DRM_GUID ) ) != 0 )
    {
        printf( "\t\tThis ASF stream has an unknown stream type.\n");
    }

    //
    // Check for the stream encryption flag and that the number of encrypted streams is less than the max
    //
    if( ( ( wFlags & wEncryptedContentFlag ) == wEncryptedContentFlag ) &&
        ( f_poAsfContext->dwNumEncryptedStreams < DRMTOOLS_MAX_ENCRYPTED_STREAMS ) )
    {
        ChkDR( DRM_WordToByte( (DRM_WORD)( wFlags & wMaxStreamNumber ), &f_poAsfContext->rgoEncryptedStreams[f_poAsfContext->dwNumEncryptedStreams].bStreamID ) );
        f_poAsfContext->rgoEncryptedStreams[f_poAsfContext->dwNumEncryptedStreams].guidEncryptedStreamType = guidStreamType;
        f_poAsfContext->rgoEncryptedStreams[f_poAsfContext->dwNumEncryptedStreams].wFormatTag = wFormatTag;
        f_poAsfContext->dwNumEncryptedStreams++;
    }
    /*
    ** Skip past the rest of the Object's data
    */
    SAFE_SUB_UI64( f_poAsfContext->qwReadTotal, qwReadBeginning, qwSkip );      /* See how far we've traveled since we started reading */
    SAFE_SUB_UI64( f_poAsfContext->qwCurObjSize, qwSkip, qwSkip );              /* See how much of the total size would be left        */
    SAFE_SUB_UI64( qwSkip, qwAsfObjMinSize, qwSkip );    /* Need to not count the bytes for the GUID and Size   */

    ChkDR( _SkipData( f_poAsfContext, qwSkip ) );
ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _ReadASFObjectInHeader
**
** Synopsis: Reads basic info of an ASF object in the ASF Header.
**           If the object ID is known, this will call the appropriate parsing function.
**           If the object ID is not known, this will skip past the object data
**
** Arguments:
**
** [f_poAsfContext]                     -- The tool context.
** [f_pbDRMHeaderData]                  -- buffer to hold the DRM Header if it's found.
** [f_cbDRMHeaderData]                  -- Size of buffer to hold the DRM header if it's found.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
static DRM_RESULT _ReadASFObjectInHeader(
    __inout                             DRM_TOOLS_ASF_CONTEXT *f_poAsfContext,
    __out_bcount( f_cbDRMHeaderData )   DRM_BYTE              *f_pbDRMHeaderData,
    __in                                DRM_DWORD              f_cbDRMHeaderData )
{
    DRM_RESULT          dr              = DRM_SUCCESS;

    ChkArg( f_poAsfContext != NULL );
    ChkArg( f_pbDRMHeaderData != NULL && f_cbDRMHeaderData > 0 );

    /* Read the ASF Object ID and Size */
    ChkDR( _ReadASFObjectBasicInfo( f_poAsfContext ) );

    /* See if the object ID matches one of the objects we care about, otherwise skip it */
    if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFContentEncryptionObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound Content Encryption Object!  This contains the V1 header.\n" );
        }
        ChkDR ( _ReadASFContentEncryptionObject( f_poAsfContext ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFExtendedContentEncryptionObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound Extended Content Encryption Object!  This contains the V2 header.\n" );
        }
        ChkDR ( _ReadASFExtendedContentEncryptionObject( f_poAsfContext, f_pbDRMHeaderData, f_cbDRMHeaderData ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(DRM_PR_PROTECTION_SYSTEM_ID), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound V4 Content Encryption Object!  This contains the V4 header.\n" );
        }
        ChkDR ( _ReadASFV4ContentEncryptionObject( f_poAsfContext, f_pbDRMHeaderData, f_cbDRMHeaderData ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFHeaderExtensionObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound Header Extension Object!\n" );
        }
        ChkDR ( _ReadASFHeaderExtensionObject( f_poAsfContext ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFFilePropertiesObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound File Properties Object. (Start of signed part of header).\n" );
        }
        f_poAsfContext->oFoundObjects.fFoundFileProperties = TRUE;
        ChkDR ( _ReadASFFilePropertiesObject( f_poAsfContext ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFDigitalSignatureObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound Digital Signature Object. (End of signed part of header).\n" );
        }
        f_poAsfContext->oFoundObjects.fFoundDigitalSignature = TRUE;
        ChkDR ( _SkipASFObject( f_poAsfContext ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFStreamPropertiesObject), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound Stream Properties Object.\n" );
        }
        ChkDR ( _ReadASFStreamPropertiesObject( f_poAsfContext ) );
    }
    else if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFStreamPropertiesObjectEx), SIZEOF( DRM_GUID ) ) == 0 )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\tFound Extended Stream Properties Object. Skipping for now.\n" );
        }
        ChkDR ( _SkipASFObject( f_poAsfContext ) );
    }
    else
    {
        ChkDR ( _SkipASFObject( f_poAsfContext ) );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRMTOOLS_OpenASFContext
**
** Synopsis: Reads data of the ASF Header Object, and then reads through
**           all ASF objects contained in the ASF Header Extension Object.
**           Format of ASF Header Extension Object
**              Object GUID              [16 bytes]
**              Object Size              [8 bytes]  must be >= 30
**              Number of Header Objects [4 bytes]
**              Reserved 1               [1 byte]   should be 0X01, but is ignored
**              Reserved 2               [1 byte]   must be 0x02
**
** Arguments:
**
** [f_poAsfContext]                     -- The tool context.
** [f_pbDRMHeaderData]                  -- Buffer to holder the DRM header.
** [f_cbDRMHeaderData]                  -- Size of buffer to hold the DRM header.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_OpenASFContext(
    __in                                OEM_FILEHDL            f_hFile,
    __out                               DRM_TOOLS_ASF_CONTEXT *f_poAsfContext,
    __out_bcount( f_cbDRMHeaderData )   DRM_BYTE              *f_pbDRMHeaderData,
    __in                                DRM_DWORD              f_cbDRMHeaderData )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           cbRead          = 0;
    DRM_UINT64          qwHeaderSize    = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD           coHeaderObjects = 0;
    DRM_BYTE            bHeaderRes1     = 0;
    DRM_BYTE            bHeaderRes2     = 0;
    DRM_DWORD           i               = 0;
    DRM_UINT64          qwAsfHdrMinSize = ASF_HDR_MIN_SIZE;

    ChkArg( f_hFile        != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_poAsfContext != NULL );
    ChkArg( f_pbDRMHeaderData != NULL && f_cbDRMHeaderData > 0 );

    f_poAsfContext->hFileIn=f_hFile;

    if ( f_poAsfContext->fVerbose )
    {
        printf( "Reading ASF Header...\n" );
    }
    /* Read the ASF Header Object ID and Size */
    ChkDR( _ReadASFObjectBasicInfo( f_poAsfContext ) );

    /* Check that Object ID matches expected value */
    if ( MEMCMP( &(f_poAsfContext->guidCurObjectID), &(g_guidASFHeaderObject), SIZEOF( DRM_GUID ) ) != 0 )
    {
        printf( "\tCould not find the ASF header object ID.  This is not a valid ASF file.\n\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    if ( f_poAsfContext->fVerbose )
    {
        printf( "\tFound Top-Level ASF Header Object!\n" );
    }

    /* Store the header object size locally */
    qwHeaderSize = f_poAsfContext->qwCurObjSize;

    /* Check that header object size is not too small */
    if ( DRM_UI64Les( qwHeaderSize, qwAsfHdrMinSize ) )
    {
        printf( "\tASF Header Object size is too small.\n\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Read the number of objects in the header */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &coHeaderObjects, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_DWORD ) )
    {
        printf( "\tFailed to read the number of header objects.\n\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    FIX_ENDIAN_DWORD(coHeaderObjects);

    /* Read the first reserved byte */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &bHeaderRes1, SIZEOF( DRM_BYTE ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_BYTE ) )
    {
        printf( "\tFailed to read the first reserved byte of the header object.\n\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );

    /* Read the second reserved byte */
    if ( Oem_File_Read( f_poAsfContext->hFileIn, &bHeaderRes2, SIZEOF( DRM_BYTE ), &cbRead ) == FALSE ||
         cbRead != SIZEOF( DRM_BYTE ) )
    {
        printf( "\tFailed to read the second reserved byte of the header object.\n\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    SAFE_ADD_UI64( f_poAsfContext->qwReadTotal, DRM_UI64( cbRead ) , f_poAsfContext->qwReadTotal );
    if ( bHeaderRes2 != 2 )
    {
        printf( "\tSecond reserved byte of the ASF header object does not match ASF spec requirements.\n\n" );

        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Read the objects within the header */
    for (i = 0; i < coHeaderObjects; i++ )
    {
        ChkDR( _ReadASFObjectInHeader( f_poAsfContext, f_pbDRMHeaderData, f_cbDRMHeaderData ) );
    }

    /* Check that the total number of bytes read equals the size of the header object */
    if ( !DRM_UI64Eql( qwHeaderSize, f_poAsfContext->qwReadTotal ) )
    {
        printf( "\n\tUnexpected end of ASF Header Object:\n" );
        printf( "\t\tExpected Size: 0X%08X%08X\n", (DRM_DWORD)DRM_UI64High32( qwHeaderSize ), (DRM_DWORD)DRM_UI64Low32( qwHeaderSize ) );
        printf( "\t\tActual Size:   0X%08X%08X\n", (DRM_DWORD)DRM_UI64High32( f_poAsfContext->qwReadTotal ), (DRM_DWORD)DRM_UI64Low32( f_poAsfContext->qwReadTotal ) );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /* Check if this file has a valid set of headers */
    if ( f_poAsfContext->oFoundObjects.fFoundV4 == TRUE && f_poAsfContext->oFoundObjects.fFoundV2 == TRUE )
    {
        printf( "\n\tFound V2 and V4 headers.  \n\tThis is not a valid file, since V4 and V2 headers are mutually exclusive header objects.\n\n" );
        ChkDR( DRM_E_FAIL );
    }
    else if ( f_poAsfContext->oFoundObjects.fFoundV2 == TRUE )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\n\tFound V2 header.  \n\tThis should be valid PlayReady/WMDRMPD content using cocktail encryption.\n\n" );
        }
    }
    else if ( f_poAsfContext->oFoundObjects.fFoundV4 == TRUE )
    {
        if ( f_poAsfContext->fVerbose )
        {
            printf( "\n\tFound V4 header.  \n\tThis should be valid PlayReady content using AES encryption.\n\n" );
        }
    }
    else
    {
        printf( "\n\tFound neither V2 nor V4 headers.  \n\tThis is not a valid protected file for PlayReady.\n\n" );
        ChkDR( DRM_E_FAIL );
    }
    if ( f_poAsfContext->oFoundObjects.fFoundV1 == FALSE && f_poAsfContext->fVerbose )
    {
        printf( "\n\tWarning: \n\tCould not find V1 header...\n\n" );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRMTOOLS_CloseASFContext
**
** Synopsis: Closes the handles and releases memory used by an ASF file context.
**
** Arguments:
**
** [f_poAsfContext]                     -- The tool context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_FILEOPEN             -- Tried to close an unopened file.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_CloseASFContext(
    __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( f_poAsfContext->hFileIn != OEM_INVALID_HANDLE_VALUE )
    {
        ChkBOOL( Oem_File_Close( f_poAsfContext->hFileIn ), DRM_E_FILEOPEN );
        f_poAsfContext->hFileIn = OEM_INVALID_HANDLE_VALUE;
    }
    SAFE_OEM_FREE( f_poAsfContext->oV1Data.pbKeyID );
    SAFE_OEM_FREE( f_poAsfContext->oV1Data.pbURL );
    SAFE_OEM_FREE( f_poAsfContext->oV1Data.pbSecretData );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
