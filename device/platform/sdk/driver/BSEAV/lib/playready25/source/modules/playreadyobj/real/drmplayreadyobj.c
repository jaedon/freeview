/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPLAYREADYOBJ_C
#include <drmplayreadyobj.h>
#include <drmxmlparser.h>
#include <drmcontract.h>
#include <drmest.h>
#include <drmutilitieslite.h>
#include <drmlicense.h>
#include <drmplayreadyobjconstants.h>
#include <drmbytemanip.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* "DATA/PROTECTINFO/ALGID" */
static const DRM_WCHAR g_rgwchAlgorithmPath[] = { ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'L', '\0' ), ONE_WCHAR( 'G', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '\0', '\0' ) };
static const DRM_CONST_STRING g_dstrAlgorithmPath = CREATE_DRM_STRING( g_rgwchAlgorithmPath );

/* "DATA/PROTECTINFO/KID" */
static const DRM_WCHAR g_rgwchV4_1KIDPath[] = { ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '\0', '\0' ) };
static const DRM_CONST_STRING g_dstrV4_1KIDPath = CREATE_DRM_STRING( g_rgwchV4_1KIDPath );

/* "<CUSTOMATTRIBUTES xmlns="">" */
static const DRM_WCHAR g_rgwchOpenNodeCustomAttributes[] = { ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( ' ', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'm', '\0' ),
                                                             ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( '=', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '\0', '\0' ) };
static const DRM_CONST_STRING g_dstrOpenNodeCustomAttributes = CREATE_DRM_STRING( g_rgwchOpenNodeCustomAttributes );


#define V2_BOM ( 0xFEFF )
#define SIZEOF_V2_BOM ( SIZEOF( DRM_WORD ) )
#define STR_SIZE( x ) ( SIZEOF( x ) / SIZEOF( x[ 0 ] ) - 1 )
#define RECORDTYPE_WRMHEADER ( 1 )

#define SAFE_MEMCPY( to, toSize, toIndex, from, fromSize )                      \
do                                                                              \
{                                                                               \
    ChkOverflow( ( toIndex ) + ( fromSize ), toIndex );                         \
    ChkBOOL( ( toIndex ) + ( fromSize ) <= ( toSize ), DRM_E_BUFFERTOOSMALL );  \
    MEMCPY( ( to ) + ( toIndex ), ( from ), ( fromSize ) );                     \
                                                                                \
    toIndex += fromSize;                                                        \
}                                                                               \
while ( FALSE )


#define SAFE_WRITE_DRM_WCHAR( pBufferStruct, wchar )    \
do                                                      \
{                                                       \
    DRM_WCHAR __x = wchar;                              \
    DRMASSERT( NULL != pBufferStruct );                 \
                                                        \
    SAFE_MEMCPY(   pBufferStruct->pbBuffer,             \
                   pBufferStruct->cbBuffer,             \
                   pBufferStruct->dwCurrentPos,         \
                 ( DRM_WCHAR* )&__x,                    \
                   SIZEOF( DRM_WCHAR ) );               \
}                                                       \
while ( FALSE )


typedef struct __tagV4Header
{
    DRM_DWORD m_cbSize;
    DRM_WORD  m_coRecordCount;
    DRM_WORD  m_wRecordType;
    DRM_WORD  m_wRecordLength;
} V4Header;

#define _IsEmptyDstr( f_dstrIsEmpty ) ( ( NULL == f_dstrIsEmpty ) || ( NULL == PB_DSTR( f_dstrIsEmpty ) ) || ( 0 == CB_DSTR( f_dstrIsEmpty ) ) )

DRM_RESULT _GetCustAttributes(
    __in_ecount( 1 )  const DRM_CONST_STRING *f_pstrRMHeader,
    __out_opt               DRM_CONST_STRING *f_pstrDestination,
    __out_ecount( 1 )       DRM_DWORD        *f_pcchCustAttrLen );

static DRM_BOOL _IsCustomAttrNode(
    __in_ecount( 1 ) const DRM_CONST_STRING *f_pstrNodeTag );

/**********************************************************************
**
** Function:    _GetCustAttributes
**
** Synopsis:    Gets len in wchars of all the custom attributes in a v2 header
**
** Arguments:
** [f_pstrRMHeader]:    -- Pointer to a RMHeader string.
** [f_pstrDestination]  -- Optional output parameter, if specified, then the
**                         custom attributes will be copied to this string.
** [f_pcchCustAttrLen]  -- If DRM_SUCCESS, then the number of wchars written to param 2.
**                         If DRM_E_BUFFERTOOSMALL, then size required to write.
**
** Returns:     DRM_SUCCESS - on success AND true.
**              DRM_E_ error message on other failure.
**              Sets f_pcchCustAttrLen, could be set to 0.
**
**********************************************************************/
DRM_RESULT _GetCustAttributes(
    __in_ecount( 1 )  const DRM_CONST_STRING *f_pstrRMHeader,
    __out_opt               DRM_CONST_STRING *f_pstrDestination,
    __out_ecount( 1 )       DRM_DWORD        *f_pcchWritten )
{
    DRM_RESULT       dr      = DRM_SUCCESS;
    DRM_DWORD        i       = 0;
    DRM_CONST_STRING strData = EMPTY_DRM_STRING;
    DRM_DWORD        cchLen  = 0;

    ChkArg( NULL != f_pcchWritten );
    ChkArg( NULL != f_pstrRMHeader );
    ChkArg( NULL != f_pstrRMHeader->pwszString );
    ChkArg( 0    != f_pstrRMHeader->cchString );

    /*
    ** All custom attributes sit under the DATA node, so get that node now.
    */
    ChkDR( DRM_XML_GetSubNode(  f_pstrRMHeader,
                               &g_dstrTagData,
                                NULL,
                                NULL,
                                0,
                                NULL,
                               &strData,
                                1 ) );

    *f_pcchWritten = 0;

    /*
    ** Now loop through all sub nodes.
    */
    while( 0 != strData.cchString )
    {
        DRM_CONST_STRING strNode = EMPTY_DRM_STRING;
        DRM_CONST_STRING strTag  = EMPTY_DRM_STRING;
        DRM_BOOL         fIsLeaf = FALSE;

        /*
        ** Get the next node, even though we don't use all the params, they have to be supplied.
        */
        ChkDR( DRM_XML_EnumNextSubNode( &strData,
                                        &strTag,
                                        &strNode,
                                         NULL,
                                         NULL,
                                         NULL,
                                        &fIsLeaf,
                                        0 ) );

        /*
        ** if this was detected to be a custom data node, then lets add the len to our output
        */
        if ( _IsCustomAttrNode( &strTag ) )
        {
            cchLen += strNode.cchString;

            if (    NULL != f_pstrDestination
                 && NULL != f_pstrDestination->pwszString
                 && cchLen <= f_pstrDestination->cchString )
            {
                SAFE_MEMCPY( PB_DSTR( f_pstrDestination ),
                             CB_DSTR( f_pstrDestination ),
                             i,
                             PB_DSTR( &strNode ),
                             CB_DSTR( &strNode ) );
            }
        }

        /*
        ** Setup of next iteration, increment our pointer to the end of the node we just read,
        ** decrement the len of our array by the amount we are moving forward.
        */
        strData.pwszString += strNode.cchString;
        ChkOverflow( strData.cchString, strData.cchString - strNode.cchString ); /* underflow check */
        strData.cchString -= strNode.cchString;
    }

    if (      0 != cchLen
         && ( NULL == f_pstrDestination || cchLen > f_pstrDestination->cchString ) )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pcchWritten = cchLen;


ErrorExit:
   return dr;
}

/**********************************************************************
**
** Function:    _IsCustomAttrNode
**
** Synopsis:    Tells if we are a custom attribute node or not.
**
** Arguments:
** [f_pstrNodeTag]  -- The node tag to be tested.
**
** Returns:     TRUE  if we are a custom attribute node tag.
**              FALSE if we are not or if invalid arg.
**
**********************************************************************/
static DRM_BOOL _IsCustomAttrNode(
    __in_ecount( 1 ) const DRM_CONST_STRING *f_pstrNodeTag )
{
    if (    f_pstrNodeTag             != NULL
         && f_pstrNodeTag->pwszString != NULL
         && f_pstrNodeTag->cchString  != 0
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagSignature )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagContentXML )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagProtectInfo )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagLAINFO )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagData )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagChecksum )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagKID )
         && !DRM_UTL_DSTRStringsEqual( f_pstrNodeTag, &g_dstrTagAlgID ) )
    {
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************
**
** Function:    DRM_PRO_GetRMHeaderVersion
**
** Synopsis:    Gets the version of the RMHEADER passed in. It currently
**              handles WMDRM header ( with or without BOM ) and  PlayReady
**              header ( wrapped in a PlayReady object record ) only.
**
** Arguments:
** [f_pbHeader]     -- WMDRM header to be tested.
** [f_cbHeader]     -- Size of f_pbHeader in bytes.
** [f_peVersion]    -- Return value of the version type, must not be null.
**
** Returns:     DRM_SUCCESS - on success.
**              DRM_E_ error message on other failure.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_GetRMHeaderVersion(
    __in_bcount( f_cbHeader ) const DRM_BYTE            *f_pbHeader,
    __in                      const DRM_DWORD            f_cbHeader,
    __out_ecount( 1 )               eDRM_HEADER_VERSION *f_peVersion )
{
    DRM_RESULT       dr                  = DRM_SUCCESS;
    const V4Header  *poHeader            = ( const V4Header* )f_pbHeader;
    DRM_CONST_STRING strXmlData          = EMPTY_DRM_STRING;
    DRM_CONST_STRING strWRMHeaderNode    = EMPTY_DRM_STRING;
    DRM_CONST_STRING strVersionAttribute = EMPTY_DRM_STRING;

    ChkArg( NULL != f_pbHeader );
    ChkArg( NULL != f_peVersion );

    /* Minimum size for safe PlayReady header checks. */
    ChkArg( SIZEOF( V4Header ) < f_cbHeader );

    *f_peVersion = DRM_HEADER_VERSION_UNKNOWN;

    /*
    ** We can be given either the WRMHeader PRO Record, v2 object or the WRMHeader bytes. In the case of the v2
    ** or v4 record objects, skip directly to the XML data.
    */
    if ( RECORDTYPE_WRMHEADER == poHeader->m_wRecordType )
    {
        ChkBOOL( poHeader->m_cbSize <= f_cbHeader, DRM_E_INVALID_PLAYREADY_OBJECT );
        ChkBOOL( poHeader->m_wRecordLength <= f_cbHeader - SIZEOF( V4Header ),
                 DRM_E_INVALID_PLAYREADY_OBJECT );

        /*
        ** Skip the v4 header struct when initializing string.
        */
        strXmlData.pwszString = ( const DRM_WCHAR* )( f_pbHeader + SIZEOF( V4Header ) );
        strXmlData.cchString  = ( poHeader->m_wRecordLength / SIZEOF( DRM_WCHAR ) );
    }
    else if ( V2_BOM == ( ( DRM_WCHAR* )f_pbHeader )[ 0 ] )
    {
        /* Skip the BOM, 2 bytes or 1 wchar. */
        strXmlData.pwszString = ( const DRM_WCHAR* )( f_pbHeader + SIZEOF_V2_BOM );
        strXmlData.cchString  = ( f_cbHeader - SIZEOF_V2_BOM ) / SIZEOF( DRM_WCHAR );
    }
    else
    {
        strXmlData.pwszString = ( const DRM_WCHAR* )f_pbHeader;
        strXmlData.cchString  = f_cbHeader / SIZEOF( DRM_WCHAR );
    }

    /*
    ** Get the WRMHEADER node and then extract the version attribute from it
    */
    ChkDR( DRM_XML_GetSubNode( &strXmlData,
                               &g_dstrTagWrmHeader,
                                NULL,
                                NULL,
                                0,
                               &strWRMHeaderNode,
                                NULL,
                                0 ) );
    ChkDR( DRM_XML_GetNodeAttribute( &strWRMHeaderNode, &g_dstrAttributeVersion, &strVersionAttribute ) );

    if ( DRM_UTL_DSTRStringsEqual( &g_dstrTagV4_1, &strVersionAttribute ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_4_1;
    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrTagV4, &strVersionAttribute ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_4;
    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrTagV2_4, &strVersionAttribute ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_2_4;
    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrTagV2, &strVersionAttribute ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_2;
    }
    else
    {
        *f_peVersion = DRM_HEADER_VERSION_UNKNOWN;
        ChkDR( DRM_E_CH_UNSUPPORTED_VERSION );
    }

ErrorExit:

    return dr;
}

/**********************************************************************
**
** Function:    DRM_PRO_IsValidObject
**
** Synopsis:    This method validates structure of PlayReady object
**
** Arguments:
** [f_pbBuffer]             -- Pointer to PlayReady object
** [f_cbBuffer]             -- Size of buffer
** [f_pdwRecordTypeBitMask] -- A bitmask representing what types of records are available
**                             A value of zero means zero records are present.
**                              Note that a valid PRO can have zero records, so callers should not treat such a PRO as invalid.
**                             Use IS_RECORD_TYPE_BIT_SET( *f_pdwRecordTypeBitMask, member-of-eDRMPlayReady_RECORD_TYPES )
**                              to determine whether a specific record is available.
**
** Returns:     DRM_SUCCESS       if byte array represents PlayReady object
**              DRM_S_FALSE       if byte array does not represent PlayReady object
**              DRM_E_INVALIDARG  if at least one input parameter is invalid
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRO_IsValidObject(
    __in_bcount( f_cbBuffer ) const DRM_BYTE   *f_pbBuffer,
    __in                      const DRM_DWORD   f_cbBuffer,
    __out_opt                       DRM_DWORD  *f_pdwRecordTypeBitMask )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_DWORD        dwRemainingSize = f_cbBuffer;
    DRM_WORD         wCounter        = 0;
    DRM_DWORD        dwHeaderLength  = 0;
    DRM_WORD         wRecordCount    = 0;
    DRM_WORD         wRecordType     = 0;
    DRM_WORD         wDataLength     = 0;
    const DRM_BYTE  *pbCurrent       = f_pbBuffer;
    DRM_BOOL         fWRMHeaderFound = FALSE;

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_cbBuffer > 0 );

    if( f_pdwRecordTypeBitMask != NULL )
    {
        *f_pdwRecordTypeBitMask = 0;
    }

    /* Make sure there is enough data to process DRMPlayReadyDataHeader */
    ChkBOOL( f_cbBuffer >= SIZE_OF_DRM_PLAYREADY_DATA_HEADER, DRM_S_FALSE );

    /* Read the size of the PlayReadyObject */
    BYTES_TO_DWORD( dwHeaderLength, pbCurrent );
    ChkBOOL( f_cbBuffer >= dwHeaderLength, DRM_S_FALSE );

    BYTES_TO_WORD( wRecordCount, pbCurrent + SIZEOF( DRM_DWORD ) );
    if( wRecordCount == 0 )
    {
        goto ErrorExit;
    }

    /* Make sure there is enough data to process PlayReady Record header */
    ChkBOOL( f_cbBuffer >= SIZE_OF_DRM_PLAYREADY_DATA_HEADER + SIZE_OF_DRM_PLAYREADY_RECORD_HEADER, DRM_S_FALSE );

    pbCurrent += SIZE_OF_DRM_PLAYREADY_DATA_HEADER;
    dwRemainingSize -= SIZE_OF_DRM_PLAYREADY_DATA_HEADER;

    /* iterate through the records and verify the size */
    for( wCounter = 0; wCounter < wRecordCount; wCounter++ )
    {
        /* Verify the remaining buffer size before proceeding */
        ChkBOOL( dwRemainingSize > 2*SIZEOF( DRM_WORD ), DRM_S_FALSE );

        BYTES_TO_WORD( wRecordType, pbCurrent );
        BYTES_TO_WORD( wDataLength, ( pbCurrent + SIZEOF( DRM_WORD ) ) );

        /* Do not allow zero length */
        ChkBOOL( wDataLength != 0, DRM_S_FALSE );

        /* Check if have enough buffer data for current record */
        ChkBOOL( dwRemainingSize >= SIZE_OF_DRM_PLAYREADY_RECORD_HEADER + wDataLength, DRM_S_FALSE );

        /* PLAYREADY_V4_XML_HEADER must be present in PlayReady header only once */
        if( wRecordType == PLAYREADY_WRMHEADER )
        {
            ChkBOOL( !fWRMHeaderFound, DRM_S_FALSE );
            fWRMHeaderFound = TRUE;
        }

        if( f_pdwRecordTypeBitMask != NULL )
        {
            /* Ignore any record type larger than what the output can hold.  It's an invalid record anyway. */
            if( wRecordType <= (BITS_PER_STANDARD_BYTE * SIZEOF(DRM_DWORD) ) )
            {
                *f_pdwRecordTypeBitMask = *f_pdwRecordTypeBitMask | RECORD_TYPE_TO_BIT(wRecordType);
            }
        }

        /* Subtract the size of current record */
        dwRemainingSize -= ( SIZE_OF_DRM_PLAYREADY_RECORD_HEADER + wDataLength );

        /* Get to the next record */
        pbCurrent += SIZE_OF_DRM_PLAYREADY_RECORD_HEADER + wDataLength;
    }

    /* All buffer data should be consumed at this point */
    ChkBOOL( wCounter == wRecordCount, DRM_S_FALSE );
    ChkBOOL( dwRemainingSize == 0, DRM_S_FALSE );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_PRO_GetRecord
**
** Synopsis:    This function extracts the record with specified record type.
**              It can be used to extract Embedded store and V4 XML header.
**              This function validates structure of PlayReady object, but it does not
**              validate the extracted record whether it is embedded store or V4 XML record.
**
** Arguments:
** [f_pbBuffer]          -- Pointer to PlayReady object
** [f_cbBuffer]          -- Size of PlayReady object
** [f_wRecordType]       -- type of record to extract.
** [f_ppbEmbeddedRecord] -- Returns a pointer to the embedded store within f_pbBuffer.
** [f_pcbEmbeddedRecord] -- Returns the length of the embedded store record.
**
** Returns:     DRM_SUCCESS                    - The Embedded Store was successfully extracted
**              DRM_E_RECORD_NOT_FOUND         - Record with requested type is not found. The PlayReady object has correct structure.
**              DRM_E_INVALIDARG               - At least one input parameter is invalid
**                                               or input f_pbBuffer does not contain valid PlayReady object
**              DRM_E_INVALID_PLAYREADY_OBJECT - Parsing of PlayReady object failed
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRO_GetRecord(
    __in_bcount( f_cbBuffer )                const DRM_BYTE   *f_pbBuffer,
    __in                                     const DRM_DWORD   f_cbBuffer,
    __in                                           DRM_WORD    f_wRecordType,
    __deref_out_bcount(*f_pcbEmbeddedRecord)       DRM_BYTE  **f_ppbEmbeddedRecord,
    __out                                          DRM_DWORD  *f_pcbEmbeddedRecord )
{
   DRM_RESULT dr                    = DRM_SUCCESS;
   DRM_WORD   wRecordType           = 0;
   DRM_WORD   wDataLength           = 0;
   DRM_WORD   wCounter              = 0;
   DRM_WORD   wRecordCount          = 0;
   DRM_DWORD  ibCur                 = 0;
   DRM_DWORD  dwRecordTypeBitMask   = 0;

   ChkArg( f_ppbEmbeddedRecord != NULL );
   ChkArg( f_pcbEmbeddedRecord != NULL );

   /*
   ** Preset size of returned object to zero in case it is not found.
   */
   *f_pcbEmbeddedRecord = 0;

   /*
   ** Validates the PlayReadyObject structure.
   */
   ChkDRMap( DRM_PRO_IsValidObject( f_pbBuffer, f_cbBuffer, &dwRecordTypeBitMask ), DRM_S_FALSE, DRM_E_INVALIDARG );
   ChkBOOL( dwRecordTypeBitMask != 0, DRM_E_INVALIDARG );
   ChkBOOL( IS_RECORD_TYPE_BIT_SET( dwRecordTypeBitMask, f_wRecordType ), DRM_E_RECORD_NOT_FOUND );

   /*
   ** Make sure there is enough data to process DRMPlayReadyDataHeader
   */
   ChkBOOL( f_cbBuffer >= SIZE_OF_DRM_PLAYREADY_DATA_HEADER, DRM_E_INVALID_PLAYREADY_OBJECT );

   /*
   ** Read count of records from header of PlayReady object.
   ** PlayReady object must contains at least one record
   ** Record count is located after the DWORD length in the data header
   */
   BYTES_TO_WORD( wRecordCount, f_pbBuffer + ibCur + SIZEOF( DRM_DWORD ) );
   ChkBOOL( wRecordCount > 0, DRM_E_INVALID_PLAYREADY_OBJECT );
   ibCur += SIZE_OF_DRM_PLAYREADY_DATA_HEADER;

   /*
   ** iterate through the records and verify the size
   */
   for( wCounter = 0; wCounter < wRecordCount; wCounter++ )
   {
        /* Verify the remaining buffer size before proceeding */
        ChkBOOL( ibCur + 2*SIZEOF( DRM_WORD ) < f_cbBuffer, DRM_E_INVALID_PLAYREADY_OBJECT );

        BYTES_TO_WORD( wRecordType, f_pbBuffer + ibCur );
        ibCur += SIZEOF( DRM_WORD );
        BYTES_TO_WORD( wDataLength, f_pbBuffer + ibCur );
        ibCur += SIZEOF( DRM_WORD );

        /*
        ** Do not allow zero length
        */
        ChkBOOL( wDataLength > 0, DRM_E_INVALID_PLAYREADY_OBJECT );

        /*
        ** Check the type of the record. If the type is the same as requested in f_wRecordType
        ** then it means we found the object and need to return it.
        */

        if( wRecordType == f_wRecordType )
        {
            ChkBOOL( wDataLength <= MAX_EMBEDDED_STORE_LEN, DRM_E_INVALID_PLAYREADY_OBJECT );
            *f_ppbEmbeddedRecord = ( DRM_BYTE * )( f_pbBuffer + ibCur );
            *f_pcbEmbeddedRecord = wDataLength;
            goto ErrorExit;
        }

       /*
       ** Get to the next record
       */
       ibCur += wDataLength;
   }

   /*
   ** Failed to find embedded record. Should never happen because we used IS_RECORD_TYPE_BIT_SET above.
   */
   DRMASSERT( FALSE );
   dr = DRM_E_RECORD_NOT_FOUND;

ErrorExit:
    return dr;
}


typedef struct __tagDRM_BUFFER_STREAM
{
    DRM_BYTE    *pbBuffer;
    DRM_DWORD    cbBuffer;
    DRM_DWORD    dwCurrentPos;
} DRM_BUFFER_STREAM;


/******************************************************************************
**
** Function:   _drm_BufferStreamWriteData
**
** Synopsis:   Writes DWORD into stream
**
** Arguments:
**
** [f_pHeaderStream]    -- DRM_BUFFER_STREAM structure that keeps track of the buffer.,
** [f_pbData]           -- Pointer to buffer to write.
** f_cbData             -- Size of the buffer.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer specified by f_pHeaderStream is too small.
**
******************************************************************************/
static DRMFORCEINLINE DRM_RESULT _drm_BufferStreamWriteData(
    __inout                         DRM_BUFFER_STREAM *f_pHeaderStream,
    __in_bcount_opt(f_cbData) const DRM_BYTE          *f_pbData,
    __in                            DRM_WORD           f_cbData )

{   DRM_RESULT dr                = DRM_SUCCESS;

    ChkArg( f_pHeaderStream != NULL );
    ChkArg( f_pbData != NULL );

    /*
    ** If there is no space for extra WORD, return error.
    */
    if ( f_pHeaderStream->dwCurrentPos + f_cbData > f_pHeaderStream->cbBuffer )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Writes data and update current position.
    */
    DRMCRT_memcpy( f_pHeaderStream->pbBuffer + f_pHeaderStream->dwCurrentPos, f_pbData, f_cbData );
    f_pHeaderStream->dwCurrentPos +=f_cbData;

ErrorExit :
    return dr;
}

/******************************************************************************
**
** Function:   _drm_BufferStreamWriteWORD
**
** Synopsis:   Writes WORD into stream
**
** Arguments:
**
** [f_pHeaderStream]    -- DRM_BUFFER_STREAM structure that keeps track of the buffer.,
** [f_wValue]           -- WORD value.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer specifieded by f_pHeaderStream is too small.
**
******************************************************************************/
static DRMFORCEINLINE DRM_RESULT _drm_BufferStreamWriteWORD( __inout DRM_BUFFER_STREAM *f_pHeaderStream, __in DRM_WORD f_wValue )

{   DRM_RESULT dr                = DRM_SUCCESS;

    DRMASSERT( f_pHeaderStream != NULL );
    __analysis_assume( f_pHeaderStream != NULL );

    /*
    ** If there is no space for extra WORD, return error.
    */
    ChkOverflow( f_pHeaderStream->dwCurrentPos + SIZEOF( f_wValue ), f_pHeaderStream->dwCurrentPos );
    if ( f_pHeaderStream->dwCurrentPos + SIZEOF( f_wValue ) > f_pHeaderStream->cbBuffer )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Writes data and update current position.
    */
    WORD_TO_BYTES( f_pHeaderStream->pbBuffer + f_pHeaderStream->dwCurrentPos, f_wValue );
    f_pHeaderStream->dwCurrentPos += SIZEOF( f_wValue );

ErrorExit :
    return dr;
}

/******************************************************************************
**
** Function:   _drm_BufferStreamWriteDWORD
**
** Synopsis:   Writes DWORD into stream
**
** Arguments:
**
** [f_pHeaderStream]    -- DRM_BUFFER_STREAM structure that keeps track of the buffer.,
** [f_dwValue]          -- DWORD value.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer pointed by f_pbPlayReadyHeader is too small or f_pbPlayReadyHeader is NULL.
**
******************************************************************************/
static DRMFORCEINLINE DRM_RESULT _drm_BufferStreamWriteDWORD( __inout DRM_BUFFER_STREAM *f_pHeaderStream, __in DRM_DWORD f_dwValue )

{   DRM_RESULT dr                = DRM_SUCCESS;

    DRMASSERT( f_pHeaderStream != NULL );
    __analysis_assume( f_pHeaderStream != NULL );

    /*
    ** If there is no space for extra DWORD, return error.
    */
    ChkOverflow( f_pHeaderStream->dwCurrentPos + SIZEOF( f_dwValue ), f_pHeaderStream->dwCurrentPos );
    if ( f_pHeaderStream->dwCurrentPos + SIZEOF( f_dwValue ) > f_pHeaderStream->cbBuffer )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Writes data and update current position.
    */
    DWORD_TO_BYTES( f_pHeaderStream->pbBuffer + f_pHeaderStream->dwCurrentPos, f_dwValue );
    f_pHeaderStream->dwCurrentPos += SIZEOF( f_dwValue );

ErrorExit :
    return dr;
}

/******************************************************************************
**
** Function:   _drm_InsertRecord
**
** Synopsis:   Insert record into PlayReady object
**
** Arguments:
**
** [f_pHeaderStream]         -- DRM_BUFFER_STREAM strcture that keeps track of the buffer.,
**
** [f_wRecordType]           -- Record type.
**
** [f_pRecord]               -- Pointer to inserted record data. If it is NULL space just reserved for f_wRecordType type
**                              Output: size of PlayReady object header (including PlayReady header)
**
**[f_cbRecord]               -- Size of the record.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pRecord is NULL.
**
******************************************************************************/
static DRM_RESULT _drm_InsertRecord(
    __inout DRM_BUFFER_STREAM * f_pHeaderStream,
    __in    DRM_WORD            f_wRecordType,
    __in_bcount_opt(f_cbRecord) const DRM_BYTE  *f_pRecord,
    __in    DRM_WORD            f_cbRecord )

{   DRM_RESULT dr                = DRM_SUCCESS;

    DRMASSERT( f_pHeaderStream != NULL );
    __analysis_assume( f_pHeaderStream != NULL );

    /* Record type, pre-defined constants */
    ChkDR( _drm_BufferStreamWriteWORD( f_pHeaderStream, f_wRecordType ) );

    /* Writes size of the record */
    ChkDR( _drm_BufferStreamWriteWORD( f_pHeaderStream, f_cbRecord ) );

    /*
    ** If pointer to record data is provided, we write it to the target buffer.
    ** If pointer is NULL, means the space should be reserved and we move the current position forward
    */
    if ( f_pRecord != NULL )
    {
        ChkDR( _drm_BufferStreamWriteData( f_pHeaderStream, f_pRecord, f_cbRecord ) );
    }
    else
    {
        ChkBOOL( f_pHeaderStream->dwCurrentPos + f_cbRecord <= f_pHeaderStream->cbBuffer, DRM_E_BUFFERTOOSMALL );

        if ( f_wRecordType == PLAYREADY_EMBEDDED_LICENSE_STORE )
        {
            DRM_BYTE *pbELS = f_pHeaderStream->pbBuffer + f_pHeaderStream->dwCurrentPos;

            /* Initialize the embedded store. */
            MEMCPY( pbELS, DRM_EST_HEADER_CONST, DRM_EST_HEADER_CONST_LEN );

            WORD_TO_NETWORKBYTES( pbELS, DRM_EST_HEADER_CONST_LEN, DRM_EST_CURRENT_VERSION );
            WORD_TO_NETWORKBYTES( pbELS, DRM_EST_HEADER_CONST_LEN + SIZEOF( DRM_WORD ), DRM_EST_MINIMUM_VERSION );
            DWORD_TO_NETWORKBYTES( pbELS, DRM_EST_HEADER_CONST_LEN + 2 * SIZEOF( DRM_WORD ), f_cbRecord );
        }

        f_pHeaderStream->dwCurrentPos += f_cbRecord;
    }

ErrorExit :
    return dr;
}


/******************************************************************************
**
** Function:   _drm_WriteFullOpenXmlNode
**
** Synopsis:   A simple helper function to write a full XML note string
**             into supplied buffer at specified offset
**
** Arguments:
**
** [f_pBuffer]          -- DRM_BUFFER_STREAM structure that keeps track of the buffer. Required.
** [f_pdstrOpenNode]    -- Full Node open brace (e.g. <NODE>). Required.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pRecord is NULL.
**
** Warning:     This function does not verify that open/close braces conform to the XML syntax!
**              It assumes that the caller knows what he is doing.
**
******************************************************************************/
static DRM_RESULT _drm_WriteFullOpenXmlNode(
    __inout_ecount( 1 )       DRM_BUFFER_STREAM *f_pBuffer,
    __in_ecount( 1 )    const DRM_CONST_STRING  *f_pdstrOpenNode )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pBuffer );
    ChkArg( NULL != f_pBuffer->pbBuffer );
    ChkArg( 0    != f_pBuffer->cbBuffer );
    ChkDRMString( f_pdstrOpenNode );

    SAFE_MEMCPY( f_pBuffer->pbBuffer,
                 f_pBuffer->cbBuffer,
                 f_pBuffer->dwCurrentPos,
                 PB_DSTR( f_pdstrOpenNode ),
                 CB_DSTR( f_pdstrOpenNode ) );

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function:   _drm_WriteXmlNode
**
** Synopsis:   A simple helper function to write XML node into supplied buffer at specified offset
**
** Arguments:
**
** [f_pBuffer]               -- DRM_BUFFER_STREAM structure that keeps track of the buffer.
** [f_pdstrNodeOpenBrace]    -- Node open brace (e.g. <NODE>). Optional.
** [f_pdstrNodeContents]     -- Node contents - arbitrary string. Optional.
** [f_pdstrNodeCloseBrace]   -- Node close brace (e.g. </NODE>). Optional.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pRecord is NULL.
**
** Warning:     This function does not verify that open/close braces conform to the XML syntax!
**              It assumes that the caller knows what he is doing.
**
******************************************************************************/
static DRM_RESULT _drm_WriteXmlNode(
    __inout_ecount( 1 )        DRM_BUFFER_STREAM *f_pBuffer,
    __in_ecount_opt( 1 ) const DRM_CONST_STRING  *f_pdstrNodeOpenBrace,
    __in_ecount_opt( 1 ) const DRM_CONST_STRING  *f_pdstrNodeContents,
    __in_ecount_opt( 1 ) const DRM_CONST_STRING  *f_pdstrNodeCloseBrace )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    ** Buffer must be always supplied
    */
    ChkArg( NULL != f_pBuffer &&
            NULL != f_pBuffer->pbBuffer &&
               0 != f_pBuffer->cbBuffer );

    /*
    ** Open brace must be either valid non-empty string, or be NULL
    */
    ChkArg( ( NULL == f_pdstrNodeOpenBrace ) ||
            ( NULL != f_pdstrNodeOpenBrace &&
              NULL != PB_DSTR( f_pdstrNodeOpenBrace ) &&
                 0 != CB_DSTR( f_pdstrNodeOpenBrace ) ) );

    /*
    ** Node contents must be either valid non-empty string, or be NULL
    */
    ChkArg( ( NULL == f_pdstrNodeContents ) ||
            ( NULL != f_pdstrNodeContents &&
              NULL != PB_DSTR( f_pdstrNodeContents ) &&
                 0 != CB_DSTR( f_pdstrNodeContents ) ) );

    /*
    ** Close brace must be either valid non-empty string, or be NULL
    */
    ChkArg( ( NULL == f_pdstrNodeCloseBrace ) ||
            ( NULL != f_pdstrNodeCloseBrace &&
              NULL != PB_DSTR( f_pdstrNodeCloseBrace ) &&
                 0 != CB_DSTR( f_pdstrNodeCloseBrace ) ) );

    /*
    ** Write open brace for the Node if it is present
    */
    if ( NULL != f_pdstrNodeOpenBrace )
    {
        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '<', '\0' ) );

        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( f_pdstrNodeOpenBrace ),
                     CB_DSTR( f_pdstrNodeOpenBrace ) );

        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '>', '\0' ) );
    }

    /*
    ** Write Node contents if present
    */
    if ( NULL != f_pdstrNodeContents )
    {
        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( f_pdstrNodeContents ),
                     CB_DSTR( f_pdstrNodeContents ) );
    }

    /*
    ** Write close brace for the Node if it is present
    */
    if ( NULL != f_pdstrNodeCloseBrace )
    {
        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '<', '\0' ) );
        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '/', '\0' ) );

        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( f_pdstrNodeCloseBrace ),
                     CB_DSTR( f_pdstrNodeCloseBrace ) );

        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '>', '\0' ) );
    }

ErrorExit :
    return dr;
}


/******************************************************************************
**
** Function:   _drm_WriteV4_1KIDNode
**
** Synopsis:   A simple helper function to write v4.1 KID node into
**             supplied buffer at specified offset
**
** Arguments:
**
** [f_pBuffer]               -- DRM_BUFFER_STREAM structure that keeps track of the buffer.
** [f_pdstrNodeOpenBrace]    -- KID corrosponding to a license. Required.
** [f_pdstrNodeContents]     -- Checksum value for the License. Optional.
** [f_pdstrNodeCloseBrace]   -- AlgID  for the License. Must be set to either:
**                              eDRM_AES_COUNTER_CIPHER or eDRM_RC4_CIPHER
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pRecord is NULL.
**
******************************************************************************/
static DRM_RESULT _drm_WriteV4_1KIDNode(
    __inout_ecount( 1 )        DRM_BUFFER_STREAM     *f_pBuffer,
    __in_ecount( 1 )     const DRM_CONST_STRING      *f_pdstrKIDValue,
    __in_ecount_opt( 1 ) const DRM_CONST_STRING      *f_pdstrCheckSumValue,
    __in_ecount( 1 )     const DRM_SUPPORTED_CIPHERS  f_eCipherType )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pBuffer && NULL != f_pBuffer->pbBuffer && 0 != f_pBuffer->cbBuffer );
    ChkArg( !_IsEmptyDstr( f_pdstrKIDValue ) );
    ChkArg( eDRM_AES_COUNTER_CIPHER == f_eCipherType || eDRM_RC4_CIPHER == f_eCipherType );

    /*
    ** Write (includes '<' and space): '<KID '
    */
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '<', '\0' ) );

    SAFE_MEMCPY( f_pBuffer->pbBuffer,
                 f_pBuffer->cbBuffer,
                 f_pBuffer->dwCurrentPos,
                 PB_DSTR( &g_dstrTagKID ),
                 CB_DSTR( &g_dstrTagKID ) );

    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( ' ', '\0' ) );

    /*
    ** The attributes have to be in alphanumeric oder. i.e. in the order of
    ** ALGID, CHECKSUM and VALUE.  If not it will not pass license server's
    ** signature verification.
    */

    /*
    ** Write (includes spaces and quotes): 'ALGID="{COCKTAIL,AESCTR}" '
    */
    SAFE_MEMCPY( f_pBuffer->pbBuffer,
                 f_pBuffer->cbBuffer,
                 f_pBuffer->dwCurrentPos,
                 PB_DSTR( &g_dstrTagAlgID ),
                 CB_DSTR( &g_dstrTagAlgID ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '=', '\0' ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '\"', '\0' ) );

    if ( eDRM_AES_COUNTER_CIPHER == f_eCipherType )
    {
        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( &g_dstrTagAESCTR ),
                     CB_DSTR( &g_dstrTagAESCTR ) );
    }
    else
    {
        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( &g_dstrTagCOCKTAIL ),
                     CB_DSTR( &g_dstrTagCOCKTAIL ) );
    }

    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '\"', '\0' ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( ' ', '\0' ) );

    /*
    ** Write (includes spaces and quotes): 'CHECKSUM="{CHECKSUM VALUE}" '. Optional Attribute.
    */
    if ( !_IsEmptyDstr( f_pdstrCheckSumValue ) )
    {
        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( &g_dstrTagChecksum ),
                     CB_DSTR( &g_dstrTagChecksum ) );

        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '=', '\0' ) );
        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '\"', '\0' ) );

        SAFE_MEMCPY( f_pBuffer->pbBuffer,
                     f_pBuffer->cbBuffer,
                     f_pBuffer->dwCurrentPos,
                     PB_DSTR( f_pdstrCheckSumValue ),
                     CB_DSTR( f_pdstrCheckSumValue ) );

        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '\"', '\0' ) );
        SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( ' ', '\0' ) );
    }

    /*
    ** Write (includes spaces and quotes): 'VALUE="{KID VALUE}"'
    */
    SAFE_MEMCPY( f_pBuffer->pbBuffer,
                 f_pBuffer->cbBuffer,
                 f_pBuffer->dwCurrentPos,
                 PB_DSTR( &g_dstrTagValue ),
                 CB_DSTR( &g_dstrTagValue ) );

    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '=', '\0' ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '\"', '\0' ) );

    SAFE_MEMCPY( f_pBuffer->pbBuffer,
                 f_pBuffer->cbBuffer,
                 f_pBuffer->dwCurrentPos,
                 PB_DSTR( f_pdstrKIDValue ),
                 CB_DSTR( f_pdstrKIDValue ) );

    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '\"', '\0' ) );

    /*
    ** Close off the node, write: '></KID>'
    */
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '>', '\0' ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '<', '\0' ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '/', '\0' ) );
    SAFE_MEMCPY( f_pBuffer->pbBuffer,
                 f_pBuffer->cbBuffer,
                 f_pBuffer->dwCurrentPos,
                 PB_DSTR( &g_dstrTagKID ),
                 CB_DSTR( &g_dstrTagKID ) );
    SAFE_WRITE_DRM_WCHAR( f_pBuffer, ONE_WCHAR( '>', '\0' ) );

ErrorExit :
    return dr;
}


/******************************************************************************
**
** Function:   _drm_GetCipherNodesStringsForRightsManagement
**
** Synopsis:    A simple helper function that returns KEYLEN and ALGID RM Header
**              XML nodes strings for a given encryption type
**
** Arguments:
**
** [f_eCipherType]           -- Cypher type (AES, COCKTAIL).
** [f_pdstrKeyLenNodeData]   -- string data for KEYLEN XML node returned here.
** [f_pdstrAlgIDNodeData]    -- string data ALGID XML node returned here.
**
** Returns:       DRM_SUCCESS          if successful
**                E_INVALIDARG         if arguments are invalid
**
******************************************************************************/
static DRMFORCEINLINE DRM_RESULT _drm_GetCipherNodesStringsForRightsManagement(
    __in                                const DRM_SUPPORTED_CIPHERS             f_eCipherType,
    __inout_ecount( 1 )                       DRM_CONST_STRING                 *f_pdstrKeyLenNodeData,
    __inout_ecount( 1 )                       DRM_CONST_STRING                 *f_pdstrAlgIDNodeData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pdstrKeyLenNodeData );
    ChkArg( NULL != f_pdstrAlgIDNodeData );

    switch ( f_eCipherType )
    {
        case eDRM_AES_COUNTER_CIPHER:
            ASSIGN_DRM_STRING( *f_pdstrKeyLenNodeData, g_dstrKeyLenNodeDataAESCTR );
            ASSIGN_DRM_STRING( *f_pdstrAlgIDNodeData, g_dstrTagAESCTR );
            break;

        case eDRM_RC4_CIPHER:
            ASSIGN_DRM_STRING( *f_pdstrKeyLenNodeData, g_dstrKeyLenNodeDataCocktail );
            ASSIGN_DRM_STRING( *f_pdstrAlgIDNodeData, g_dstrTagCOCKTAIL );
            break;

        default:
            ChkArg( FALSE );
    };

ErrorExit :
    return dr;
}


/******************************************************************************
**
** Function:   _drm_GetProtectInfoLengthV4_1
**
** Synopsis:   static method for calculating the length of the PROTECTINFO node
**             for v4.1 WRMHeaders
**
** Arguments:
**
** [f_pdstrKID]              -- string data for KID value.
** [f_pdstrCheckSum]         -- string data for CHECKSUM value. Optional.
** [f_eCipherType]           -- Cypher type (AES, COCKTAIL).
** [f_pcchProtectInfoLen]    -- Output parameter to be set with length of
**                              the PROTECTINFO node.
**
** Returns:       DRM_SUCCESS          if successful
**                E_INVALIDARG         if arguments are invalid
**
******************************************************************************/
static DRMFORCEINLINE DRM_RESULT _drm_GetProtectInfoLengthV4_1(
    __in_ecount( 1 )     const DRM_CONST_STRING      *f_pdstrKID,
    __in_ecount_opt( 1 ) const DRM_CONST_STRING      *f_pdstrCheckSum,
    __in                 const DRM_SUPPORTED_CIPHERS  f_eCipherType,
    __out_ecount( 1 )          DRM_DWORD             *f_pcchProtectInfoLen )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pdstrKID );
    ChkArg( NULL != f_pcchProtectInfoLen );
    ChkArg( eDRM_AES_COUNTER_CIPHER == f_eCipherType || eDRM_RC4_CIPHER == f_eCipherType );

    *f_pcchProtectInfoLen =   CB_DSTR( &g_dstrTagProtectInfo ) + ( 2 * SIZEOF( DRM_WCHAR ) )  /* '<PROTECTINFO>' */
                            + CB_DSTR( &g_dstrTagProtectInfo ) + ( 3 * SIZEOF( DRM_WCHAR ) ); /* '</PROTECTINFO>' */

    /*
    ** Calculate the size of the KID node now. We have to remember the spaces, quotes and {<,/,>}s.
    */
    *f_pcchProtectInfoLen  +=   CB_DSTR( &g_dstrTagKID ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<KID ' */
                              + CB_DSTR( &g_dstrTagValue ) + CB_DSTR( f_pdstrKID ) + ( 3 * SIZEOF( DRM_WCHAR ) ); /* 'VALUE="{b64 KID}"' */

    if( !_IsEmptyDstr( f_pdstrCheckSum ) )
    {
        __analysis_assume( f_pdstrCheckSum != NULL );
        *f_pcchProtectInfoLen += CB_DSTR( &g_dstrTagChecksum ) + CB_DSTR( f_pdstrCheckSum ) + ( 4 * SIZEOF( DRM_WCHAR ) ); /* 'CHECKSUM="{b64 checksum}" ' */
    }

    if ( eDRM_AES_COUNTER_CIPHER == f_eCipherType )
    {
        /* 'ALGID="AESCTR" ' */
        *f_pcchProtectInfoLen += CB_DSTR( &g_dstrTagAlgID ) + CB_DSTR( &g_dstrTagAESCTR ) + ( 4 * SIZEOF( DRM_WCHAR ) );
    }
    else
    {
        /* 'ALGID="COCKTAIL" ' */
        *f_pcchProtectInfoLen += CB_DSTR( &g_dstrTagAlgID ) + CB_DSTR( &g_dstrTagCOCKTAIL ) + ( 4 * SIZEOF( DRM_WCHAR ) );
    }

    /* '></KID>' */
    *f_pcchProtectInfoLen += CB_DSTR( &g_dstrTagKID ) + ( 4 * SIZEOF( DRM_WCHAR ) );

ErrorExit :
    return dr;
}



/**********************************************************************
**
** Function:    _drm_CreateV4RightsManagementHeader
**
** Synopsis:    Creates a PlayReady Rights Management Header from given parameters.
**              This is the worker function that does the actual job.
**
** Arguments:
** [f_eCipherType]      -- Cypher type (AES, COCKTAIL).
** [f_pdstrKID]         -- Contains base64-encoded KID value.
** [f_pdstrCheckSum]    -- Contains checksum calculated using KeyId and content key.
**                         If not used then new header will not contain checksum node. Optional.
** [f_pdstrPlayReadySilentURL]
**                      -- PlayReady server silent LA URL, optional. If not
**                         used then new header will not contain LA_URL node. Optional.
** [f_pdstrPlayReadyNonSilentURL]
**                      -- PlayReady server non silent LA URL, optional. If not
**                         used then new header will not contain LUI_URL node. Optional.
** [f_pdstrServiceID]   -- Domain service ID, optional. If not used then new header
**                         will not have DS_ID node. Optional.
** [f_pdstrCustomAttributesXML]
**                      -- Content author can add arbitrary XML inside this element. Optional.
** [f_pdstrV2Header]    -- V2 (WMDRM) Header to take the custom attributes from.
**                         This API will call _GetCustAttributes() function to extract the custom attributes from a given V2 Header.
**                         The reason for this is that the caller might not be able to allocate a scratch buffer for the custom
**                         attributes extracted from the V2 Header, and it is not known where in the f_pbRightsManagementHeader
**                         to put them until this function creates most of the Rights Management Header. Optional.
** [f_pbRightsManagementHeader]
**                      -- Pointer to a buffer to receive the PlayReady Rights Management header created.
** [f_pcbRightsManagementHeader]
**                      -- Size of f_pbPlayReadyHeader in bytes. If
**                         f_pbPlayReadyHeader is too small or null, then this
**                         will be set to the required size.
**                         On successful execution, this will be the size written.
**
** Returns:     DRM_SUCCESS - on success.
**              DRM_E_BUFFTOOSMALL if f_pbPlayReadyHeader is not big enough or is NULL.
**              DRM_E_ fail codes on other failures.
**
** Example Output (Note that the LA_URL node will only be inserted if f_pdstrPlayReadySilentURL
** is a valid non-empty string):
**     <WRMHEADER xmlns="http://schemas.microsoft.com/DRM/2007/03/PlayReadyHeader" version="4.0.0.0">
**         <DATA>
**             <PROTECTINFO>
**                 <KEYLEN>16</KEYLEN>
**                 <ALGID>AESCTR</ALGID>
**             </PROTECTINFO>
**             <KID>P5GlZb7/CEKig6gco/eQQw==</KID>
**             <CHECKSUM>jbzXJoUdhw==</CHECKSUM>
**             <LA_URL>http://PlayReady_Silent_LA_URL</LA_URL>
**             <LUI_URL>http://PlayReady_Non_Silent_LA_URL</LUI_URL>
**         </DATA>
**     </WRMHEADER>
**
**********************************************************************/
static DRM_RESULT _drm_CreateV4RightsManagementHeader(
    __in                                const DRM_SUPPORTED_CIPHERS             f_eCipherType,
    __in_ecount_nz( 1 )                 const DRM_CONST_STRING                 *f_pdstrKID,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING                 *f_pdstrCheckSum,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING                 *f_pdstrPlayReadySilentURL,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING                 *f_pdstrPlayReadyNonSilentURL,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING                 *f_pdstrServiceID,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING                 *f_pdstrCustomAttributesXML,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING                 *f_pdstrV2Header,
    __inout_bcount_opt( *f_pcbRightsManagementHeader )
                                              DRM_BYTE                         *f_pbRightsManagementHeader,
    __in                                      DRM_DWORD                        *f_pcbRightsManagementHeader )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  cbV4Size = 0;
    DRM_DWORD  cchCustAttrFromV2Header       = 0;
    DRM_BOOL   fAddCustomAttributesNode      = FALSE;
    DRM_CONST_STRING  dstrAlgIDNodeData      = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrKeyLenNodeData     = EMPTY_DRM_STRING;
    DRM_BUFFER_STREAM RightsManagementHeader = { NULL, 0, 0 };

    /*
    ** Arg checks, f_pbPlayReadyHeader can be null, in which case we return
    ** a size in *f_pcbPlayReadyHeader.
    */
    ChkArg( NULL != f_pcbRightsManagementHeader  );
    ChkDRMString( f_pdstrKID );
    ChkArg(      _IsEmptyDstr( f_pdstrCheckSum )
            || ( NULL != PB_DSTR( f_pdstrCheckSum ) && CB_DSTR( f_pdstrCheckSum ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrPlayReadySilentURL )
            || ( NULL != PB_DSTR( f_pdstrPlayReadySilentURL ) && CB_DSTR( f_pdstrPlayReadySilentURL ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrPlayReadyNonSilentURL )
            || ( NULL != PB_DSTR( f_pdstrPlayReadyNonSilentURL ) && CB_DSTR( f_pdstrPlayReadyNonSilentURL ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrServiceID )
            || ( NULL != PB_DSTR( f_pdstrServiceID ) && CB_DSTR( f_pdstrServiceID ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrCustomAttributesXML )
            || ( NULL != PB_DSTR( f_pdstrCustomAttributesXML ) && CB_DSTR( f_pdstrCustomAttributesXML ) > 0 ) );
    ChkDR( _drm_GetCipherNodesStringsForRightsManagement( f_eCipherType, &dstrKeyLenNodeData, &dstrAlgIDNodeData ) );

    /* Calculate the size needed. */
    cbV4Size =   CB_DSTR( &g_dstrOpenNodeWRMHeaderV4 )
               + CB_DSTR( &g_dstrTagWrmHeader )   + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</WRMHEADER> */
               + CB_DSTR( &g_dstrTagData )        + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<DATA>' */
               + CB_DSTR( &g_dstrTagData )        + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</DATA>' */
               + CB_DSTR( &g_dstrTagProtectInfo ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<PROTECTINFO>' */
               + CB_DSTR( &g_dstrTagProtectInfo ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</PROTECTINFO>' */
               + CB_DSTR( &g_dstrTagKeyLen )      + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<KEYLEN>' */
               + CB_DSTR( &g_dstrTagKeyLen )      + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</KEYLEN>' */
               + CB_DSTR( &dstrKeyLenNodeData )
               + CB_DSTR( &g_dstrTagAlgID )       + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<ALGID>' */
               + CB_DSTR( &g_dstrTagAlgID )       + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</ALGID>' */
               + CB_DSTR( &dstrAlgIDNodeData )
               + CB_DSTR( &g_dstrTagKID )         + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<KID>' */
               + CB_DSTR( &g_dstrTagKID )         + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</KID>' */
               + CB_DSTR( f_pdstrKID );

    /*
    ** We use CUSTOMATTRIBUTES node only if there were custom attributes supplied by caller - either directly,
    ** or inside a V2 header.
    */

    /*
    ** Did the caller supply custom attributes as a string?
    */
    if ( !_IsEmptyDstr( f_pdstrCustomAttributesXML ) )
    {
        cbV4Size += CB_DSTR( f_pdstrCustomAttributesXML );
        fAddCustomAttributesNode = TRUE;
    }

    /*
    ** Did the caller supply a V2 header that we could get custom attributes from?
    */
    if ( !_IsEmptyDstr( f_pdstrV2Header ) )
    {
        dr = _GetCustAttributes( f_pdstrV2Header, NULL, &cchCustAttrFromV2Header );

        ChkBOOL( DRM_SUCCESS == dr ||
                 DRM_E_BUFFERTOOSMALL == dr,
                 DRM_E_FAIL );

        if ( cchCustAttrFromV2Header > 0 )
        {
            cbV4Size += cchCustAttrFromV2Header * SIZEOF( DRM_WCHAR );
            fAddCustomAttributesNode = TRUE;
        }
    }

    if ( fAddCustomAttributesNode )
    {
        cbV4Size +=   CB_DSTR( &g_dstrOpenNodeCustomAttributes )
                    + CB_DSTR( &g_dstrTagCustomAttributes ) + ( 3 *  SIZEOF( DRM_WCHAR ) ); /* '</CUSTOMATTRIBUTES>' */
    }

    /*
    ** We use Checksum node only if Checksum was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrCheckSum ) )
    {
        cbV4Size +=  CB_DSTR( &g_dstrTagChecksum ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<CHECKSUM>'  */
                   + CB_DSTR( &g_dstrTagChecksum ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</CHECKSUM>' */
                   + CB_DSTR( f_pdstrCheckSum );
    }

    /*
    ** We use LA_URL node only if silent license acquisition URL was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrPlayReadySilentURL ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagLAURL ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<LAURL>'  */
                    + CB_DSTR( &g_dstrTagLAURL ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</LAURL>' */
                    + CB_DSTR( f_pdstrPlayReadySilentURL );
    }

    /*
    ** We use LUI_URL node only if non-silent license acquisition URL was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrPlayReadyNonSilentURL ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagLUIURL ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<LUIURL>'  */
                    + CB_DSTR( &g_dstrTagLUIURL ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</LUIURL>' */
                    + CB_DSTR( f_pdstrPlayReadyNonSilentURL );
    }

    /*
    ** We use DS_ID node only if domain service ID was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrServiceID ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagDSID ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<DS_ID>'  */
                    + CB_DSTR( &g_dstrTagDSID ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</DS_ID>' */
                    + CB_DSTR( f_pdstrServiceID );
    }

    /* If the size is too small set the output param and return the correct error code. */
    if ( NULL == f_pbRightsManagementHeader || cbV4Size > *f_pcbRightsManagementHeader )
    {
        *f_pcbRightsManagementHeader = cbV4Size;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Zero out the input/output buffer to be written. */
    OEM_SECURE_ZERO_MEMORY( f_pbRightsManagementHeader, *f_pcbRightsManagementHeader );

    RightsManagementHeader.pbBuffer = f_pbRightsManagementHeader;
    RightsManagementHeader.cbBuffer = *f_pcbRightsManagementHeader;
    RightsManagementHeader.dwCurrentPos = 0;

    /*
    ** Now lets do the writing.
    */

    /*
    ** Open the WRMHEADER open, DATA open, PROTECTINFO open, KEYLEN, ALGID
    ** and the PROTECTINFO close nodes.
    */
    ChkDR( _drm_WriteFullOpenXmlNode( &RightsManagementHeader, &g_dstrOpenNodeWRMHeaderV4 ) );
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagData, NULL, NULL ) );
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagProtectInfo, NULL, NULL ) );
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagKeyLen, &dstrKeyLenNodeData, &g_dstrTagKeyLen ) );
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagAlgID, &dstrAlgIDNodeData, &g_dstrTagAlgID ) );
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagProtectInfo ) );

    /* Write the KID node. */
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagKID, f_pdstrKID, &g_dstrTagKID ) );

    /* Write the checksum node if the checksum was supplied. */
    if ( !_IsEmptyDstr( f_pdstrCheckSum ) )
    {
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagChecksum,
                                   f_pdstrCheckSum,
                                  &g_dstrTagChecksum ) );
    }

    /* Write LA_URL node if the PlayReady Silent URL was supplied. */
    if ( !_IsEmptyDstr( f_pdstrPlayReadySilentURL ) )
    {
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagLAURL,
                                   f_pdstrPlayReadySilentURL,
                                  &g_dstrTagLAURL ) );
    }

    /* Write LUI_URL node if the PlayReady Non Silent URL was supplied. */
    if ( !_IsEmptyDstr( f_pdstrPlayReadyNonSilentURL ) )
    {
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagLUIURL,
                                   f_pdstrPlayReadyNonSilentURL,
                                  &g_dstrTagLUIURL ) );
    }

    /* Write DS_ID node if the domain service ID was supplied. */
    if ( !_IsEmptyDstr( f_pdstrServiceID ) )
    {
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagDSID,
                                   f_pdstrServiceID,
                                  &g_dstrTagDSID ) );
    }

    /* Write custom attributes (if supplied). */
    if ( fAddCustomAttributesNode )
    {
        ChkDR( _drm_WriteFullOpenXmlNode( &RightsManagementHeader, &g_dstrOpenNodeCustomAttributes ) );

        if ( !_IsEmptyDstr( f_pdstrCustomAttributesXML ) )
        {
            ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, f_pdstrCustomAttributesXML, NULL ) );
        }

        if ( cchCustAttrFromV2Header > 0 )
        {
            DRM_DWORD        cchWritten  = 0;
            DRM_CONST_STRING dstrV4Header = EMPTY_DRM_STRING;

            dstrV4Header.cchString  = ( RightsManagementHeader.cbBuffer - RightsManagementHeader.dwCurrentPos ) / SIZEOF( DRM_WCHAR );
            dstrV4Header.pwszString = ( const DRM_WCHAR* )( RightsManagementHeader.pbBuffer + RightsManagementHeader.dwCurrentPos );
            ChkDR( _GetCustAttributes( f_pdstrV2Header, &dstrV4Header, &cchWritten ) );
            RightsManagementHeader.dwCurrentPos += cchWritten * SIZEOF( DRM_WCHAR );
            DRMASSERT( cchWritten == cchCustAttrFromV2Header );
        }

        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagCustomAttributes ) );
    }

    /* Close the Data and WRMHEADER nodes. */
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagData ) );
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagWrmHeader ) );

    /* Now set the amount of bytes we wrote. */
    DRMASSERT( cbV4Size == RightsManagementHeader.dwCurrentPos );
    *f_pcbRightsManagementHeader = cbV4Size;

ErrorExit:

    return dr;
}


/**********************************************************************
**
** Function:    _drm_CreateV4_1RightsManagementHeader
**
** Synopsis:    Creates a PlayReady Rights Management Header from given parameters.
**              This is the worker function that does the actual job.
**
** Arguments:
** [f_eCipherType]      -- Cypher type (AES, COCKTAIL). Optional. Required is KID is set.
** [f_pdstrKID]         -- Contains base64-encoded KID value, if not specificed then new header will
**                         not contain a KID node or checksum/algid attributes. Optional
** [f_pdstrCheckSum]    -- Contains checksum calculated using KeyId and content key.
**                         If not used then new header will not contain checksum attribute in KID node. Optional.
** [f_pdstrPlayReadySilentURL]
**                      -- PlayReady server silent LA URL, optional. If not
**                         used then new header will not contain LA_URL node. Optional.
** [f_pdstrPlayReadyNonSilentURL]
**                      -- PlayReady server non silent LA URL, optional. If not
**                         used then new header will not contain LUI_URL node. Optional.
** [f_pdstrServiceID]   -- Domain service ID, optional. If not used then new header
**                         will not have DS_ID node. Optional.
** [f_pdstrCustomAttributesXML]
**                      -- Content author can add arbitrary XML inside this element. Optional.
** [f_pdstrDecryptorSetup]
**                      -- Decryptor setup hint to tell application whether or not they should
**                         wait until after the media graph has been fully setup before it creates
**                         the decryptor. Valid values: STANDARD, ONDEMAND. Optional. Setting this
**                         value will cause a v4.1 (as opposed to v4) header to be created.
** [f_pbRightsManagementHeader]
**                      -- Pointer to a buffer to receive the PlayReady Rights Management header created.
** [f_pcbRightsManagementHeader]
**                      -- Size of f_pbPlayReadyHeader in bytes. If
**                         f_pbPlayReadyHeader is too small or null, then this
**                         will be set to the required size.
**                         On successful execution, this will be the size written.
**
** Returns:     DRM_SUCCESS - on success.
**              DRM_E_BUFFTOOSMALL if f_pbPlayReadyHeader is not big enough or is NULL.
**              DRM_E_ fail codes on other failures.
**
** Example Output:
**     <WRMHEADER xmlns="http://schemas.microsoft.com/DRM/2007/03/PlayReadyHeader" version="4.1.0.0">
**         <DATA>
**             <PROTECTINFO>
**                 <KID VALUE="P5GlZb7/CEKig6gco/eQQw==" CHECKSUM="jbzXJoUdhw==" ALGID="AESCTR" />
**             </PROTECTINFO>
**             <DECRYPTORSETUP>STANDARD</DECRYPTORSETUP>
**             <LA_URL>http://PlayReady_Silent_LA_URL</LA_URL>
**             <LUI_URL>http://PlayReady_Non_Silent_LA_URL</LUI_URL>
**         </DATA>
**     </WRMHEADER>
**
**********************************************************************/
static DRM_RESULT _drm_CreateV4_1RightsManagementHeader(
    __in                                               const DRM_SUPPORTED_CIPHERS  f_eCipherType,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrKID,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrCheckSum,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrPlayReadySilentURL,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrPlayReadyNonSilentURL,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrServiceID,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrCustomAttributesXML,
    __in_ecount_nz_opt( 1 )                            const DRM_CONST_STRING      *f_pdstrDecryptorSetup,
    __inout_bcount_opt( *f_pcbRightsManagementHeader )       DRM_BYTE              *f_pbRightsManagementHeader,
    __inout_ecount( 1 )                                      DRM_DWORD             *f_pcbRightsManagementHeader )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbV4Size   = 0;
    DRM_BOOL   fBuildKID  = FALSE;
    DRM_BOOL   fBuildData = FALSE;
    DRM_BUFFER_STREAM RightsManagementHeader = { NULL, 0, 0 };

    /*
    ** All params except for f_pcbRightsManagementHeader are optional.
    ** If KID value is set then algid must also be set.
    */
    if ( NULL != f_pdstrKID && NULL != PB_DSTR( f_pdstrKID ) )
    {
        ChkArg( 0 != CB_DSTR ( f_pdstrKID ) );
        ChkArg( eDRM_AES_COUNTER_CIPHER == f_eCipherType || eDRM_RC4_CIPHER == f_eCipherType );
    }
    else
    {
        ChkArg( ( NULL == f_pdstrCheckSum ) ||
                ( NULL == PB_DSTR(f_pdstrCheckSum) && CB_DSTR(f_pdstrCheckSum) == 0 ) );
    }
    ChkArg( NULL != f_pcbRightsManagementHeader );
    ChkArg(      _IsEmptyDstr( f_pdstrCheckSum )
            || ( NULL != PB_DSTR( f_pdstrCheckSum ) && CB_DSTR( f_pdstrCheckSum ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrPlayReadySilentURL )
            || ( NULL != PB_DSTR( f_pdstrPlayReadySilentURL ) && CB_DSTR( f_pdstrPlayReadySilentURL ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrPlayReadyNonSilentURL )
            || ( NULL != PB_DSTR( f_pdstrPlayReadyNonSilentURL ) && CB_DSTR( f_pdstrPlayReadyNonSilentURL ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrServiceID )
            || ( NULL != PB_DSTR( f_pdstrServiceID ) && CB_DSTR( f_pdstrServiceID ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrCustomAttributesXML )
            || ( NULL != PB_DSTR( f_pdstrCustomAttributesXML ) && CB_DSTR( f_pdstrCustomAttributesXML ) > 0 ) );
    ChkArg(      _IsEmptyDstr( f_pdstrDecryptorSetup )
            || ( NULL != PB_DSTR( f_pdstrDecryptorSetup ) && CB_DSTR( f_pdstrDecryptorSetup ) > 0 ) );

    /*
    ** determine if we need to build the KID node (which implies the protect info node too) and
    ** the data node. Since everything is optional, if nothing is specified then don' bother with
    ** a data node.
    */
    fBuildKID  = !_IsEmptyDstr( f_pdstrKID );
    fBuildData =      fBuildKID
                  || !_IsEmptyDstr( f_pdstrPlayReadySilentURL )
                  || !_IsEmptyDstr( f_pdstrPlayReadyNonSilentURL )
                  || !_IsEmptyDstr( f_pdstrServiceID )
                  || !_IsEmptyDstr( f_pdstrCustomAttributesXML )
                  || !_IsEmptyDstr( f_pdstrDecryptorSetup );

    /*
    ** Calculate the size needed.
    */

    cbV4Size =   CB_DSTR( &g_dstrOpenNodeWRMHeaderV4_1 )
               + CB_DSTR( &g_dstrTagWrmHeader ) + ( 3 * SIZEOF( DRM_WCHAR ) ); /* '</WRMHEADER>' */

    /*
    ** Do we need to build a data node?
    */
    if ( fBuildData )
    {
        /* Calculate the size needed. */
        cbV4Size +=   CB_DSTR( &g_dstrTagData ) + ( 2 * SIZEOF( DRM_WCHAR ) )  /* '<DATA>'  */
                    + CB_DSTR( &g_dstrTagData ) + ( 3 * SIZEOF( DRM_WCHAR ) ); /* '</DATA>' */
    }

    /*
    ** Did the caller supply enough data to require a ProtectInfo node (Requires data for 1 full KID node)
    */
    if ( fBuildKID )
    {
        DRM_DWORD dwLen = 0;

        ChkDR( _drm_GetProtectInfoLengthV4_1( f_pdstrKID, f_pdstrCheckSum, f_eCipherType, &dwLen ) );
        cbV4Size += dwLen;
    }

    /*
    ** Did the caller supply custom attributes as a string?
    */
    if ( !_IsEmptyDstr( f_pdstrCustomAttributesXML ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrOpenNodeCustomAttributes )
                    + CB_DSTR( &g_dstrTagCustomAttributes ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</CUSTOMATTRIBUTES>' */
                    + CB_DSTR( f_pdstrCustomAttributesXML );
    }

    /*
    ** We use LA_URL node only if silent license acquisition URL was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrPlayReadySilentURL ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagLAURL ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<LAURL>'  */
                    + CB_DSTR( &g_dstrTagLAURL ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</LAURL>' */
                    + CB_DSTR( f_pdstrPlayReadySilentURL );
    }

    /*
    ** We use LUI_URL node only if non-silent license acquisition URL was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrPlayReadyNonSilentURL ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagLUIURL ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<LUIURL>'  */
                    + CB_DSTR( &g_dstrTagLUIURL ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</LUIURL>' */
                    + CB_DSTR( f_pdstrPlayReadyNonSilentURL );
    }

    /*
    ** We use DS_ID node only if domain service ID was supplied by the caller.
    */
    if ( !_IsEmptyDstr( f_pdstrServiceID ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagDSID ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<DS_ID>'  */
                    + CB_DSTR( &g_dstrTagDSID ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</DS_ID>' */
                    + CB_DSTR( f_pdstrServiceID );
    }

    /*
    ** We use DECRYPTORSETUP node only if DecryptorSetup was supplied by the caller.
    ** This will also force us into creating a v4.1 header
    */
    if ( !_IsEmptyDstr( f_pdstrDecryptorSetup ) )
    {
        cbV4Size +=   CB_DSTR( &g_dstrTagDecryptorSetup ) + ( 2 * SIZEOF( DRM_WCHAR ) ) /* '<DECRYPTORSETUP>'  */
                    + CB_DSTR( &g_dstrTagDecryptorSetup ) + ( 3 * SIZEOF( DRM_WCHAR ) ) /* '</DECRYPTORSETUP>' */
                    + CB_DSTR( f_pdstrDecryptorSetup );
    }

    /* If the size is too small set the output param and return the correct error code. */
    if ( NULL == f_pbRightsManagementHeader || cbV4Size > *f_pcbRightsManagementHeader )
    {
        *f_pcbRightsManagementHeader = cbV4Size;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Zero out the input/output buffer to be written. */
    OEM_SECURE_ZERO_MEMORY( f_pbRightsManagementHeader, *f_pcbRightsManagementHeader );

    RightsManagementHeader.pbBuffer = f_pbRightsManagementHeader;
    RightsManagementHeader.cbBuffer = *f_pcbRightsManagementHeader;
    RightsManagementHeader.dwCurrentPos = 0;

    /*
    ** Now lets do the writing.
    */

    /* Open the WRMHEADER node */
    ChkDR( _drm_WriteFullOpenXmlNode( &RightsManagementHeader, &g_dstrOpenNodeWRMHeaderV4_1 ) );

    /* Open the DATA Node if we are need to write it */
    if ( fBuildData )
    {
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagData, NULL, NULL ) );
    }

    /* Write the KID node. */
    if ( fBuildKID )
    {
        DRMASSERT( fBuildData );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, &g_dstrTagProtectInfo, NULL, NULL ) );
        ChkDR( _drm_WriteV4_1KIDNode( &RightsManagementHeader, f_pdstrKID, f_pdstrCheckSum, f_eCipherType ) );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagProtectInfo ) );
    }

    /* Write LA_URL node if the PlayReady Silent URL was supplied. */
    if ( !_IsEmptyDstr( f_pdstrPlayReadySilentURL ) )
    {
        DRMASSERT( fBuildData );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagLAURL,
                                   f_pdstrPlayReadySilentURL,
                                  &g_dstrTagLAURL ) );
    }

    /* Write LUI_URL node if the PlayReady Non Silent URL was supplied. */
    if ( !_IsEmptyDstr( f_pdstrPlayReadyNonSilentURL ) )
    {
        DRMASSERT( fBuildData );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagLUIURL,
                                   f_pdstrPlayReadyNonSilentURL,
                                  &g_dstrTagLUIURL ) );
    }

    /* Write DS_ID node if the domain service ID was supplied. */
    if ( !_IsEmptyDstr( f_pdstrServiceID ) )
    {
        DRMASSERT( fBuildData );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagDSID,
                                   f_pdstrServiceID,
                                  &g_dstrTagDSID ) );
    }

    /* Write DECRYPTORSETUP node if the DecryptorSetup data was supplied. */
    if ( ( NULL != f_pdstrDecryptorSetup ) &&
         ( CB_DSTR( f_pdstrDecryptorSetup ) > 0) )
    {
        DRMASSERT( fBuildData );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                  &g_dstrTagDecryptorSetup,
                                   f_pdstrDecryptorSetup,
                                  &g_dstrTagDecryptorSetup ) );
    }

    /* Write custom attributes (if supplied). */
    if ( !_IsEmptyDstr( f_pdstrCustomAttributesXML ) )
    {
        DRMASSERT( fBuildData );
        ChkDR( _drm_WriteFullOpenXmlNode( &RightsManagementHeader,&g_dstrOpenNodeCustomAttributes ) );
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader,
                                   NULL,
                                   f_pdstrCustomAttributesXML,
                                  &g_dstrTagCustomAttributes ) );
    }

    /* Close the Data and WRMHEADER nodes. */
    if ( fBuildData )
    {
        ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagData ) );
    }
    ChkDR( _drm_WriteXmlNode( &RightsManagementHeader, NULL, NULL, &g_dstrTagWrmHeader ) );

    /* Now set the amount of bytes we wrote. */
    DRMASSERT( cbV4Size == RightsManagementHeader.dwCurrentPos );
    *f_pcbRightsManagementHeader = cbV4Size;

ErrorExit:

    return dr;
}


/******************************************************************************
**
** Function:   DRM_PRO_Create
**
** Synopsis:   Wraps a PlayReady header into a PlayReady object.
**
** Arguments:
**
** [f_pdstrPlayReadyHeader]   -- Optional Input: Pointer to a DRM string with PlayReady header.
**
** [f_pbPlayReadyObject]      -- Pointer to a buffer to receive the PlayReady object header.
**
** [f_pcbPlayReadyObject]     -- Input: Size of the memory block pointed by f_pbPlayReadyObject.
**                               Output: Size of PlayReady object created.
**
** [f_cbEmbeddedStore]        -- Input: Size of the desired memory block for the embedded store
**                                      If f_cbEmbeddedStore is more than MAX_EMBEDDED_STORE_LEN,
**                                      then MAX_EMBEDDED_STORE_LEN will be used instead.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_INVALIDARG     if input parameters is invalid
**                DRM_E_BUFFERTOOSMALL if buffer pointed by f_pbPlayReadyObject is too small
**                                     or f_pbPlayReadyObject is NULL.
**
** Please make sure to update Drm_PlayReadyObject_ConvertFromWmdrmHeader()
** if the PlayReady object format changes.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_Create(
    __in_opt DRM_CONST_STRING           *f_pdstrPlayReadyHeader,
    __out_bcount_opt( *f_pcbPlayReadyObject )
             DRM_BYTE                   *f_pbPlayReadyObject,
    __inout  DRM_DWORD                  *f_pcbPlayReadyObject,
    __in     DRM_DWORD                   f_cbEmbeddedStore )
{
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_DWORD  cbPlayReadyObject = 0;
    DRM_WORD   cbEmbeddedStore   = 0;
    DRM_BUFFER_STREAM    oBufferStream = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_Create );

    ChkArg( f_pcbPlayReadyObject != NULL );

    if( f_pdstrPlayReadyHeader != NULL )
    {
        ChkDRMString( f_pdstrPlayReadyHeader );

        ChkOverflow( f_pdstrPlayReadyHeader->cchString * SIZEOF( DRM_WCHAR ),
                     f_pdstrPlayReadyHeader->cchString );
    }

    ChkDR( DRM_DWordToWord( MIN( f_cbEmbeddedStore, MAX_EMBEDDED_STORE_LEN ), &cbEmbeddedStore ) );

    /*
    ** Calculates the size of the PlayReady object.
    */
    if( f_pdstrPlayReadyHeader != NULL )
    {
        cbPlayReadyObject = f_pdstrPlayReadyHeader->cchString * SIZEOF( DRM_WCHAR ) + SIZE_OF_DRM_PLAYREADY_RECORD_HEADER;
    }

    cbPlayReadyObject += SIZE_OF_DRM_PLAYREADY_DATA_HEADER   +
                         SIZE_OF_DRM_PLAYREADY_RECORD_HEADER +
                         cbEmbeddedStore;

    /*
    ** If f_pbPlayReadyObject is NULL or buffer is not large enough, we just return the size.
    */
    if ( f_pbPlayReadyObject == NULL || *f_pcbPlayReadyObject < cbPlayReadyObject )
    {
        *f_pcbPlayReadyObject = cbPlayReadyObject;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ZEROMEM( f_pbPlayReadyObject, cbPlayReadyObject );

    *f_pcbPlayReadyObject = cbPlayReadyObject;

    oBufferStream.pbBuffer = f_pbPlayReadyObject;
    oBufferStream.cbBuffer = *f_pcbPlayReadyObject;
    oBufferStream.dwCurrentPos = 0;

    /*
    ** Write the size.
    */
    ChkDR( _drm_BufferStreamWriteDWORD( &oBufferStream, cbPlayReadyObject ) );

    /*
    ** Set record count.
    */
    if( f_pdstrPlayReadyHeader != NULL )
    {
        ChkDR( _drm_BufferStreamWriteWORD( &oBufferStream, 2 ) );
    }
    else
    {
        ChkDR( _drm_BufferStreamWriteWORD( &oBufferStream, 1 ) );
    }

    /*
    **  Insert PlayReady header.
    */
    if( f_pdstrPlayReadyHeader != NULL )
    {
        ChkDR( _drm_InsertRecord( &oBufferStream,
                                  PLAYREADY_WRMHEADER,
                                  (DRM_BYTE *)f_pdstrPlayReadyHeader->pwszString,
                                  (DRM_WORD)f_pdstrPlayReadyHeader->cchString * SIZEOF( DRM_WCHAR ) ) );
    }

    /*
    ** Insert empty embedded store.
    */
    ChkDR( _drm_InsertRecord( &oBufferStream, PLAYREADY_EMBEDDED_LICENSE_STORE, NULL, cbEmbeddedStore ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function:   DRM_PRO_GetCipherTypeFromPlayReadyHeader
**
** Synopsis:   Returns the cipher type in the PlayReady header.
**
** Arguments:
**
** [f_pbPlayReadyHeader]      -- Pointer to a byte buffer that contains the PlayReady header.
** [f_cbPlayReadyHeader]      -- Size of the buffer that contains the PlayReady header.
** [f_peCipherType]           -- Pointer to a variable that receives the cipher type in the
**                               PlayReady header.
**
** Returns:       DRM_SUCCESS          if successful
**                DRM_E_INVALIDARG     if input parameters is invalid
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_GetCipherTypeFromPlayReadyHeader(
    __in_bcount( f_cbPlayReadyHeader )
            DRM_BYTE              *f_pbPlayReadyHeader,
    __in    DRM_DWORD              f_cbPlayReadyHeader,
    __out   DRM_SUPPORTED_CIPHERS *f_peCipherType )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_CONST_STRING    dstrV4Header    = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrAlgorithm   = EMPTY_DRM_STRING;
    eDRM_HEADER_VERSION eWRMHeaderVer   = DRM_HEADER_VERSION_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetCipherTypeFromPlayReadyHeader );

    ChkArg( f_pbPlayReadyHeader != NULL && f_cbPlayReadyHeader > 0 );
    ChkArg( f_peCipherType != NULL );

    *f_peCipherType = eDRM_CIPHER_INVALID;

    /*
    ** Get the version out.
    */
    ChkDR( DRM_PRO_GetRMHeaderVersion( f_pbPlayReadyHeader, f_cbPlayReadyHeader, &eWRMHeaderVer ) );

    DSTR_FROM_PB( &dstrV4Header, f_pbPlayReadyHeader, f_cbPlayReadyHeader );

    /*
    ** Get the Algorithm out the header XML. The location of this is different between the v4.1 and v4.0 headers
    */
    ChkDR( DRM_HDR_GetAttribute( &dstrV4Header, NULL, DRM_HEADER_ATTRIB_ALGID, &dstrAlgorithm, g_wchForwardSlash ) );

    if ( DRM_UTL_DSTRStringsEqual( &dstrAlgorithm, &g_dstrTagCOCKTAIL ) )
    {
        *f_peCipherType = eDRM_RC4_CIPHER;
    }
    else if ( DRM_UTL_DSTRStringsEqual( &dstrAlgorithm, &g_dstrTagAESCTR ) )
    {
        *f_peCipherType = eDRM_AES_COUNTER_CIPHER;
    }
    else
    {
        *f_peCipherType = eDRM_CIPHER_INVALID;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function:   DRM_PRO_GetCipherType
**
** Synopsis:   Returns the cipher type in the PlayReady object.
**
** Arguments:
**
** [f_pbPlayReadyObject]      -- Pointer to a byte buffer that contains the PlayReady object.
** [f_cbPlayReadyObject]      -- Size of the buffer that contains the PlayReady object.
** [f_peCipherType]           -- Pointer to a variable that receives the cipher type in the
**                               PlayReady object.
**
** Returns:       DRM_SUCCESS             if successful
**                DRM_E_INVALIDARG        if input parameters is invalid
**                DRM_E_RECORD_NOT_FOUND  if PRO does not have a WRM header
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_GetCipherType(
    __in_bcount( f_cbPlayReadyObject ) const DRM_BYTE                 *f_pbPlayReadyObject,
    __in                                     DRM_DWORD                 f_cbPlayReadyObject,
    __out                                    DRM_SUPPORTED_CIPHERS    *f_peCipherType )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbV4Header = NULL;
    DRM_DWORD cbV4Header = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetCipherType );

    ChkArg( f_pbPlayReadyObject != NULL && f_cbPlayReadyObject > 0 );
    ChkArg( f_peCipherType != NULL );

    *f_peCipherType = eDRM_CIPHER_INVALID;

    ChkDR( DRM_PRO_GetRecord( f_pbPlayReadyObject,
                              f_cbPlayReadyObject,
                              PLAYREADY_WRMHEADER,
                              &pbV4Header,
                              &cbV4Header ) );

    ChkDR( DRM_PRO_GetCipherTypeFromPlayReadyHeader( pbV4Header,
                                                     cbV4Header,
                                                     f_peCipherType ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function:   DRM_PRO_GetDecryptorSetupTypeFromPlayReadyHeader
**
** Synopsis:   Returns the decryptor setup type a v4.1 PlayReady header.
**
** Arguments:
**
** [f_pbPlayReadyHeader]      -- Pointer to a byte buffer that contains the PlayReady header.
** [f_cbPlayReadyHeader]      -- Size of the buffer that contains the PlayReady header.
** [f_peDecryptorSetupType]   -- Pointer to a variable that receives the decryptor setup type
**                               in the setup PlayReady header.
**
** Returns:       DRM_SUCCESS             if successful
**                DRM_E_INVALIDARG        if input parameters is invalid
**                DRM_E_CH_INVALID_HEADER if header is not v4_1 or invalid decryptor setup node.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_GetDecryptorSetupTypeFromPlayReadyHeader(
    __in_ecount( f_cbPlayReadyHeader ) DRM_BYTE                 *f_pbPlayReadyHeader,
    __in                               DRM_DWORD                 f_cbPlayReadyHeader,
    __out_ecount( 1 )                  eDRM_DECRYPTORSETUP_TYPE *f_peDecryptorSetupType )
{
    DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_CONST_STRING    dstrV4_1Header      = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrDecryptorSetup  = EMPTY_DRM_STRING;
    eDRM_HEADER_VERSION eWRMHeaderVer       = DRM_HEADER_VERSION_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetDecryptorSetupTypeFromPlayReadyHeader );

    ChkArg( f_pbPlayReadyHeader != NULL && f_cbPlayReadyHeader > 0 );
    ChkArg( f_peDecryptorSetupType != NULL );

    *f_peDecryptorSetupType = DRM_DECRYPTORSETUP_INVALID;

    /*
    ** Make sure the PRO is valid and get the version out.
    */
    ChkDR( DRM_PRO_GetRMHeaderVersion( f_pbPlayReadyHeader, f_cbPlayReadyHeader, &eWRMHeaderVer ) );
    ChkBOOL( DRM_HEADER_VERSION_4_1 == eWRMHeaderVer, DRM_E_CH_INVALID_HEADER );

    DSTR_FROM_PB( &dstrV4_1Header, f_pbPlayReadyHeader, f_cbPlayReadyHeader );

    /*
    ** Get the DecryptorSetup out the header XML.
    */
    ChkDR( DRM_HDR_GetAttribute( &dstrV4_1Header,
                                  NULL,
                                  DRM_HEADER_ATTRIB_DECRYPTOR_SETUP,
                                 &dstrDecryptorSetup,
                                  g_wchForwardSlash ) );

    if ( DRM_UTL_DSTRStringsEqual( &dstrDecryptorSetup, &g_dstrTagONDEMAND ) )
    {
        *f_peDecryptorSetupType = DRM_DECRYPTORSETUP_ONDEMAND;
    }
    else
    {
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function:   DRM_PRO_GetDecryptorSetupType
**
** Synopsis:   Returns the decryptor setup type in the PlayReady object.
**
** Arguments:
**
** [f_pbPlayReadyObject]      -- Pointer to a byte buffer that contains the PlayReady object.
** [f_cbPlayReadyObject]      -- Size of the buffer that contains the PlayReady object.
** [f_peDecryptorSetupType]   -- Pointer to a variable that receives the decryptor setup type
**                               in the PlayReady object.
**
** Returns:       DRM_SUCCESS             if successful
**                DRM_E_INVALIDARG        if input parameters is invalid
**                DRM_E_CH_INVALID_HEADER if header is not v4_1 or invalid decryptor setup node.
**                DRM_E_RECORD_NOT_FOUND  if PRO does not have a WRM header
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_GetDecryptorSetupType(
    __in_ecount( f_cbPlayReadyObject ) const DRM_BYTE                 *f_pbPlayReadyObject,
    __in                                     DRM_DWORD                 f_cbPlayReadyObject,
    __out_ecount( 1 )                        eDRM_DECRYPTORSETUP_TYPE *f_peDecryptorSetupType )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BYTE   *pbWRMHeader = NULL;
    DRM_DWORD   cbWRMHeader = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetDecryptorSetupType );

    ChkArg( f_pbPlayReadyObject != NULL && f_cbPlayReadyObject > 0 );
    ChkArg( f_peDecryptorSetupType != NULL );

    *f_peDecryptorSetupType = DRM_DECRYPTORSETUP_INVALID;

    ChkDR( DRM_PRO_GetRecord( f_pbPlayReadyObject, f_cbPlayReadyObject, PLAYREADY_WRMHEADER, &pbWRMHeader, &cbWRMHeader ) );

    ChkDR( DRM_PRO_GetDecryptorSetupTypeFromPlayReadyHeader( pbWRMHeader, cbWRMHeader, f_peDecryptorSetupType ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}




/**********************************************************************
**
** Function:    DRM_PRO_CreateRMHeader
**
** Synopsis:    Creates a PlayReady Rights Management Header from given parameters.
**
** Arguments:
** [f_eWRMHeaderVersion]-- Version of the WRMHeader to build.
** [f_eCipherType]      -- Cypher type (AES, COCKTAIL).
** [f_pdstrKID]         -- Contains base64-encoded KID value.
** [f_pdstrCheckSum]    -- Contains checksum calculated using KeyId and content key.
** [f_pdstrPlayReadySilentURL]
**                      -- PlayReady server silent LA URL, optional. If not
**                         used then new header will not contain LA_URL node. Optional.
** [f_pdstrPlayReadyNonSilentURL]
**                      -- PlayReady server non silent LA URL, optional. If not
**                         used then new header will not contain LUI_URL node. Optional.
** [f_pdstrServiceID]   -- Domain service ID, optional. If not used then new header
**                         will not have DS_ID node. Optional.
** [f_pdstrCustomAttributesXML]
**                      -- Content author can add arbitrary XML inside this element. Optional.
** [f_pbRightsManagementHeader]
**                      -- Pointer to a buffer to receive the PlayReady Rights Management header created.
** [f_pcbRightsManagementHeader]
**                      -- Size of f_pbPlayReadyHeader in bytes. If
**                         f_pbPlayReadyHeader is too small or null, then this
**                         will be set to the required size.
**                         On successful execution, this will be the size written.
**
** Returns:     DRM_SUCCESS - on success.
**              DRM_E_BUFFTOOSMALL if f_pbPlayReadyHeader is not big enough or is NULL.
**              DRM_E_ fail codes on other failures.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRO_CreateRMHeader(
    __in                                const eDRM_HEADER_VERSION    f_eWRMHeaderVersion,
    __in                                const DRM_SUPPORTED_CIPHERS  f_eCipherType,
    __in_ecount_nz( 1 )                 const DRM_CONST_STRING      *f_pdstrKID,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING      *f_pdstrCheckSum,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING      *f_pdstrPlayReadySilentURL,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING      *f_pdstrPlayReadyNonSilentURL,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING      *f_pdstrServiceID,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING      *f_pdstrCustomAttributesXML,
    __in_ecount_nz_opt( 1 )             const DRM_CONST_STRING      *f_pdstrDecryptorSetup,
    __inout_bcount_opt( *f_pcbRightsManagementHeader )
                                              DRM_BYTE              *f_pbRightsManagementHeader,
    __in                                      DRM_DWORD             *f_pcbRightsManagementHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_CreateRightsManagementHeader );

    if ( DRM_HEADER_VERSION_4_1  == f_eWRMHeaderVersion )
    {
        ChkDR( _drm_CreateV4_1RightsManagementHeader( f_eCipherType,
                                                      f_pdstrKID,
                                                      f_pdstrCheckSum,
                                                      f_pdstrPlayReadySilentURL,
                                                      f_pdstrPlayReadyNonSilentURL,
                                                      f_pdstrServiceID,
                                                      f_pdstrCustomAttributesXML,
                                                      f_pdstrDecryptorSetup,
                                                      f_pbRightsManagementHeader,
                                                      f_pcbRightsManagementHeader ) );
    }
    else if ( DRM_HEADER_VERSION_4 == f_eWRMHeaderVersion )
    {
        ChkDR( _drm_CreateV4RightsManagementHeader( f_eCipherType,
                                                    f_pdstrKID,
                                                    f_pdstrCheckSum,
                                                    f_pdstrPlayReadySilentURL,
                                                    f_pdstrPlayReadyNonSilentURL,
                                                    f_pdstrServiceID,
                                                    f_pdstrCustomAttributesXML,
                                                    NULL,
                                                    f_pbRightsManagementHeader,
                                                    f_pcbRightsManagementHeader ) );
    }
    else
    {
        ChkDR( DRM_E_CH_UNSUPPORTED_VERSION );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    DRM_PRO_ConvertHeaderFromWmdrmToPlayReady
**
** Synopsis:    Generates a v4.0 PlayReady header from a WMDRM header. The conversion
**              can be seen in the examples below.
**
** Arguments:
** [f_pbWmdrmHeader]    -- WMDRM header to be converted. It may contain the BOM
**                         at the very beginning.
** [f_cbWmdrmHeader]    -- Size of f_pbWmdrmHeader in bytes (including BOM if exists).
** [f_pwchPlayReadySilentURL]
**                      -- PlayReady server silent LA URL, optional. If not
**                         used then new header will not contain LA_URL node.
** [f_cchPlayReadySilentURL]
**                      -- Size of f_pwchPlayReadySilentURL in WCHARs. If
**                         f_pwchPlayReadySilentURL is NULL then
**                         this values should be 0.
** [f_pwchPlayReadyNonSilentURL]
**                      -- PlayReady server non silent LA URL, optional. If not
**                         used then new header will not contain LUI_URL node.
** [f_cchPlayReadyNonSilentURL]
**                      -- Size of f_pwchPlayReadyNonSilentURL in WCHARs. If
**                         f_pwchPlayReadyNonSilentURL is NULL then
**                         this values should be 0.
** [f_pwchServiceID]    -- Domain service ID, optional. If not used then new header
**                         will not have DS_ID node.
** [f_pcchServiceID]    -- Size of f_pwchServiceID in WCHARs. If f_pwchServiceID
**                         is NULL then this values should be 0.
** [f_pwchCustomAttributes]-- Custom attributes to set in xml
** [f_cchCustomAttributes] -- Size of f_pwchCustomAttributes in WCHARs.
** [f_pbPlayReadyHeader]-- Pointer to a buffer to receive the PlayReady header created.
** [f_pcbPlayReadyHeader]
**                      -- Size of f_pbPlayReadyHeader in bytes. If
**                         f_pbPlayReadyHeader is too small or null, then this
**                         will be set to the required size.
**                         On successful execution, this will be the size written.
**
** Returns:     DRM_SUCCESS - on success.
**              DRM_E_BUFFTOOSMALL if f_pbPlayReadyHeader is not big enough or is NULL.
**              DRM_E_ fail codes on other failures.
**              DRM_E_CH_INCOMPATIBLE_HEADER_TYPE if the header to be converted is
**              not of type WMDRM.
**
** Warning:     This API modifies the original WMDRM Header, replacing '!'->'+' and '*'->'/' in the
**              B64-encoded checksum.
**
** Example Input:
**   <WRMHEADER version="2.0.0.0">
**       <DATA>
**           <RID>5</RID>
**           <CID>500</CID>
**           <LAINFO>http://WMDRM_Silent_LA_URL</LAINFO>
**           <KID>P5GlZb7/CEKig6gco/eQQw==</KID>
**           <CHECKSUM>jbzXJoUdhw==</CHECKSUM>
**       </DATA>
**       <SIGNATURE>
**           <HASHALGORITHM type="SHA"></HASHALGORITHM>
**           <SIGNALGORITHM type="MSDRM"></SIGNALGORITHM>
**           <VALUE>XQd7zeXfy5PO75w41ZoOAIEn0ga5nubBzlXf8udrZASTNOKEUwUzGQ==</VALUE>
**       </SIGNATURE>
**   </WRMHEADER>
**
** Example Output (Note that the LA_URL node will only be inserted if f_pwchf_cchPlayReadySilentURL
** is not NULL):
**     <WRMHEADER xmlns="http://schemas.microsoft.com/DRM/2007/03/PlayReadyHeader" version="4.0.0.0">
**         <DATA>
**             <PROTECTINFO>
**                 <KEYLEN>7</KEYLEN>
**                 <ALGID>COCKTAIL</ALGID>
**             </PROTECTINFO>
**             <KID>P5GlZb7/CEKig6gco/eQQw==</KID>
**             <CHECKSUM>jbzXJoUdhw==</CHECKSUM>
**             <LA_URL>http://PlayReady_Silent_LA_URL</LA_URL>
**             <LUI_URL>http://PlayReady_Non_Silent_LA_URL</LUI_URL>
**         </DATA>
**     </WRMHEADER>
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PRO_ConvertHeaderFromWmdrmToPlayReady(
    __in_bcount( f_cbWmdrmHeader )      const DRM_BYTE  *f_pbWmdrmHeader,
    __in                                const DRM_DWORD  f_cbWmdrmHeader,
    __in_ecount_nz_opt( f_cchPlayReadySilentURL )
                                        const DRM_WCHAR *f_pwchPlayReadySilentURL,
    __in                                const DRM_DWORD  f_cchPlayReadySilentURL,
    __in_ecount_nz_opt( f_cchPlayReadyNonSilentURL )
                                        const DRM_WCHAR *f_pwchPlayReadyNonSilentURL,
    __in                                const DRM_DWORD  f_cchPlayReadyNonSilentURL,
    __in_ecount_nz_opt( f_cchServiceID )
                                        const DRM_WCHAR *f_pwchServiceID,
    __in                                const DRM_DWORD  f_cchServiceID,
    __in_ecount_nz_opt( f_cchCustomAttributes )
                                        const DRM_WCHAR *f_pwchCustomAttributes,
    __in                                const DRM_DWORD  f_cchCustomAttributes,
    __inout_bcount_opt( *f_pcbPlayReadyHeader )
                                              DRM_BYTE  *f_pbPlayReadyHeader,
    __in                                      DRM_DWORD *f_pcbPlayReadyHeader )
{
    DRM_RESULT          dr                        = DRM_SUCCESS;
    DRM_CONST_STRING    dstrKID                   = EMPTY_DRM_STRING;
    DRM_STRING          dstrCheckSum              = EMPTY_DRM_STRING;
    eDRM_HEADER_VERSION eRMHeaderVer              = DRM_HEADER_VERSION_UNKNOWN;
    DRM_CONST_STRING    dstrV2Header              = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrPlayReadySilentURL    = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrPlayReadyNonSilentURL = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrServiceID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCustomAttributes         = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_ConvertHeaderFromWmdrmToPlayReady );

    /*
    ** Arg checks, f_pbPlayReadyHeader can be null, in which case we return
    ** a size in *f_pcbPlayReadyHeader.
    */
    ChkArg( NULL != f_pbWmdrmHeader );
    ChkArg( NULL != f_pcbPlayReadyHeader  );
    ChkArg( ( f_pwchPlayReadySilentURL != NULL && f_cchPlayReadySilentURL > 0 ) ||
            ( f_pwchPlayReadySilentURL == NULL && f_cchPlayReadySilentURL == 0 ) );
    ChkArg( ( f_pwchPlayReadyNonSilentURL != NULL && f_cchPlayReadyNonSilentURL > 0 ) ||
            ( f_pwchPlayReadyNonSilentURL == NULL && f_cchPlayReadyNonSilentURL == 0 ) );
    ChkArg( ( f_pwchServiceID != NULL && f_cchServiceID > 0 ) ||
            ( f_pwchServiceID == NULL && f_cchServiceID == 0 ) );
    ChkArg( f_cbWmdrmHeader >= SIZEOF_V2_BOM );

    dstrPlayReadySilentURL.pwszString = f_pwchPlayReadySilentURL;
    dstrPlayReadySilentURL.cchString = f_cchPlayReadySilentURL;

    dstrPlayReadyNonSilentURL.pwszString = f_pwchPlayReadyNonSilentURL;
    dstrPlayReadyNonSilentURL.cchString = f_cchPlayReadyNonSilentURL;

    dstrServiceID.pwszString = f_pwchServiceID;
    dstrServiceID.cchString = f_cchServiceID;

    dstrCustomAttributes.pwszString = f_pwchCustomAttributes;
    dstrCustomAttributes.cchString  = f_cchCustomAttributes;

    if ( V2_BOM == ( ( DRM_WCHAR * )f_pbWmdrmHeader )[ 0 ] )
    {
        /* Skip the BOM. */
        dstrV2Header.pwszString = ( const DRM_WCHAR* )( f_pbWmdrmHeader + SIZEOF( DRM_WCHAR ) );
        dstrV2Header.cchString = ( f_cbWmdrmHeader - SIZEOF( DRM_WCHAR ) ) / SIZEOF( DRM_WCHAR );
    }
    else
    {
        dstrV2Header.pwszString = ( const DRM_WCHAR* )f_pbWmdrmHeader;
        dstrV2Header.cchString = f_cbWmdrmHeader / SIZEOF( DRM_WCHAR );
    }

    /* Make sure that we were given a WMDRM header. */
    ChkDR( DRM_PRO_GetRMHeaderVersion( f_pbWmdrmHeader, f_cbWmdrmHeader, &eRMHeaderVer ) );
    ChkBOOL( DRM_HEADER_VERSION_2 == eRMHeaderVer, DRM_E_CH_INVALID_HEADER );

    /* Get the needed data from the v2 header: KID, checksum & sizeof custom attributes. */
    {
        DRM_DWORD   dwCount = 0;

        /* Get the KID, then the checksum, then the size of the custom attributes (could be zero). */
        ChkDR( DRM_XML_GetSubNode( &dstrV2Header,
                                   &g_dstrTagKID,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                   &dstrKID,
                                    2 ) );

        ChkDR( DRM_XML_GetSubNode( &dstrV2Header,
                                   &g_dstrTagChecksum,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                   ( DRM_CONST_STRING * )&dstrCheckSum,
                                    2 ) );

        /*
        ** Checksum is in the form of a B64 string.
        ** Translate from the old alphabet used in WMDRM: convert '!' to '+'; '*' to '/'.
        */
        for ( dwCount = 0; dwCount < dstrCheckSum.cchString; dwCount++ )
        {
            if ( dstrCheckSum.pwszString[ dwCount ] == WCHAR_CAST( '!' ) )
            {
                dstrCheckSum.pwszString[ dwCount ] = WCHAR_CAST( '+' );
            }
            else if ( dstrCheckSum.pwszString[ dwCount ] == WCHAR_CAST( '*' ) )
            {
                dstrCheckSum.pwszString[ dwCount ] = WCHAR_CAST( '/' );
            }
        }
    }

    ChkDR( _drm_CreateV4RightsManagementHeader(   eDRM_RC4_CIPHER,
                                                 &dstrKID,
                                                ( DRM_CONST_STRING* )&dstrCheckSum,
                                                 &dstrPlayReadySilentURL,
                                                 &dstrPlayReadyNonSilentURL,
                                                 &dstrServiceID,
                                                 &dstrCustomAttributes,
                                                 &dstrV2Header,
                                                  f_pbPlayReadyHeader,
                                                  f_pcbPlayReadyHeader ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function:    Drm_PlayReadyObject_ConvertFromWmdrmHeader
**
** Synopsis:    Generates a PlayReady object from a WMDRM header.
**
** Arguments:
**
** [f_pbWmdrmHeader]        -- WMDRM header to be converted. It may contain the BOM
**                             at the very beginning.
** [f_cbWmdrmHeader]        -- Size of f_pbWmdrmHeader in bytes (including BOM if exists).
** [f_pwchPlayReadySilentURL]
**                          -- PlayReady server silent LA URL, optional. If not
**                             used then new header will not contain LA_URL node.
** [f_cchPlayReadySilentURL]
**                          -- Size of f_pwchPlayReadySilentURL in WCHARs. If
**                             f_pwchPlayReadySilentURL is NULL then
**                             this values should be 0.
** [f_pwchPlayReadyNonSilentURL]
**                          -- PlayReady server non silent LA URL, optional. If not
**                             used then new header will not contain LUI_URL node.
** [f_cchPlayReadyNonSilentURL]
**                          -- Size of f_pwchPlayReadyNonSilentURL in WCHARs. If
**                             f_pwchPlayReadyNonSilentURL is NULL then
**                             this values should be 0.
** [f_pwchServiceID]        -- Domain service ID, optional. If not used then new header
**                             will not have DS_ID node.
** [f_pcchServiceID]        -- Size of f_pwchServiceID in WCHARs. If f_pwchServiceID
**                             is NULL then this values should be 0.
** [f_fIncludeELS]          -- Flag indicating whether to include an embedded license store.
** [f_pwchCustomAttributes] -- Custom attributes to set in xml
** [f_cchCustomAttributes]  -- Size of f_pwchCustomAttributes in WCHARs.
** [f_pbPlayReadyObject]    -- Pointer to a buffer to receive the PlayReady object converted.
** [f_pcbPlayReadyObject]   -- Size of f_pbPlayReadyObject in bytes. If f_pbPlayReadyObject is
**                             too small or null, then this will be set to the required size.
**                             On successful execution, this will be the size written.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_CH_INCOMPATIBLE_HEADER_TYPE if the header to be converted
**                          is not of type WMDRM.
**                          DRM_E_BUFFERTOOSMALL - output buffer too small.
**                          Other error codes as defined in drmresults.h.
**
** Please make sure to update DRM_PRO_Create() if the PlayReady object format changes.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_PlayReadyObject_ConvertFromWmdrmHeader(
    __in_bcount( f_cbWmdrmHeader )  const DRM_BYTE  *f_pbWmdrmHeader,
    __in                            const DRM_DWORD  f_cbWmdrmHeader,
    __in_ecount_nz_opt( f_cchPlayReadySilentURL )
                                    const DRM_WCHAR *f_pwchPlayReadySilentURL,
    __in                            const DRM_DWORD  f_cchPlayReadySilentURL,
    __in_ecount_nz_opt( f_cchPlayReadyNonSilentURL )
                                    const DRM_WCHAR *f_pwchPlayReadyNonSilentURL,
    __in                            const DRM_DWORD  f_cchPlayReadyNonSilentURL,
    __in_ecount_nz_opt( f_cchServiceID )
                                    const DRM_WCHAR *f_pwchServiceID,
    __in                            const DRM_DWORD  f_cchServiceID,
    __in                                  DRM_BOOL   f_fIncludeELS,
    __in_ecount_nz_opt( f_cchCustomAttributes )
                                        const DRM_WCHAR *f_pwchCustomAttributes,
    __in                                const DRM_DWORD  f_cchCustomAttributes,
    __inout_bcount_opt( *f_pcbPlayReadyObject )
                                          DRM_BYTE  *f_pbPlayReadyObject,
    __inout                               DRM_DWORD *f_pcbPlayReadyObject )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbPlayReadyHeader = 0;
    DRM_DWORD cbPlayReadyObject = 0;
    DRM_BUFFER_STREAM oBufferStream = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_Drm_PlayReadyObject_ConvertFromWmdrmHeader );

    ChkArg( f_pbWmdrmHeader != NULL && f_cbWmdrmHeader > 0 );
    ChkArg( f_pcbPlayReadyObject != NULL );
    ChkArg( ( f_pbPlayReadyObject != NULL && *f_pcbPlayReadyObject > 0 ) ||
            ( f_pbPlayReadyObject == NULL && *f_pcbPlayReadyObject == 0 ) );

    /* Calculate the size of the PlayReady header converted. */
    dr = DRM_PRO_ConvertHeaderFromWmdrmToPlayReady(
                f_pbWmdrmHeader,
                f_cbWmdrmHeader,
                f_pwchPlayReadySilentURL,
                f_cchPlayReadySilentURL,
                f_pwchPlayReadyNonSilentURL,
                f_cchPlayReadyNonSilentURL,
                f_pwchServiceID,
                f_cchServiceID,
                f_pwchCustomAttributes,
                f_cchCustomAttributes,
                NULL,
                &cbPlayReadyHeader );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );

        /* Should not reach here. */
        DRMASSERT( FALSE );
    }
    else
    {
        ChkDR( DRM_SUCCESS );
    }

    /* Calculate the overall size of the PlayReady object generated. */
    cbPlayReadyObject = cbPlayReadyHeader +
                        SIZE_OF_DRM_PLAYREADY_DATA_HEADER +
                        SIZE_OF_DRM_PLAYREADY_RECORD_HEADER;

    if ( f_fIncludeELS )
    {
        cbPlayReadyObject += SIZE_OF_DRM_PLAYREADY_RECORD_HEADER;
        cbPlayReadyObject += MAX_EMBEDDED_STORE_LEN;
    }

    /* If f_pbPlayReadyObject is NULL or the buffer is not large enough, just return the size. */
    if ( f_pbPlayReadyObject == NULL || *f_pcbPlayReadyObject < cbPlayReadyObject )
    {
        *f_pcbPlayReadyObject = cbPlayReadyObject;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ZEROMEM( f_pbPlayReadyObject, cbPlayReadyObject );

    *f_pcbPlayReadyObject = cbPlayReadyObject;

    oBufferStream.pbBuffer = f_pbPlayReadyObject;
    oBufferStream.cbBuffer = *f_pcbPlayReadyObject;
    oBufferStream.dwCurrentPos = 0;

    /* Write the overall size of the PlayReady object. */
    ChkDR( _drm_BufferStreamWriteDWORD( &oBufferStream, cbPlayReadyObject ) );

    /*
    ** Write the record count. There are 2 records, one for PlayReady header,
    ** the other for embedded store.
    */
    ChkDR( _drm_BufferStreamWriteWORD( &oBufferStream, f_fIncludeELS ? (DRM_WORD)2 : (DRM_WORD)1 ) );

    /* Write the type of the PlayReady header record. */
    ChkDR( _drm_BufferStreamWriteWORD( &oBufferStream, PLAYREADY_WRMHEADER ) );

    /* Write size of the PlayReady header record. */
    ChkDR( _drm_BufferStreamWriteWORD( &oBufferStream, ( DRM_WORD )cbPlayReadyHeader ) );

    /* Convert and write the PlayReady header. */
    ChkDR( DRM_PRO_ConvertHeaderFromWmdrmToPlayReady(
                f_pbWmdrmHeader,
                f_cbWmdrmHeader,
                f_pwchPlayReadySilentURL,
                f_cchPlayReadySilentURL,
                f_pwchPlayReadyNonSilentURL,
                f_cchPlayReadyNonSilentURL,
                f_pwchServiceID,
                f_cchServiceID,
                f_pwchCustomAttributes,
                f_cchCustomAttributes,
                oBufferStream.pbBuffer + __CB_DECL( oBufferStream.dwCurrentPos ),
                &cbPlayReadyHeader ) );

    oBufferStream.dwCurrentPos += cbPlayReadyHeader;

    if ( f_fIncludeELS )
    {
        /* Insert empty embedded store record. */
        ChkDR( _drm_InsertRecord( &oBufferStream, PLAYREADY_EMBEDDED_LICENSE_STORE, NULL, MAX_EMBEDDED_STORE_LEN ) );
    }

ErrorExit:

    ChkECC( ECC_Drm_PlayReadyObject_ConvertFromWmdrmHeader, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
