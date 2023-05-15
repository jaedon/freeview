/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmheaderparser.h>
#include <drmxmlparser.h>
#include <drmutilitieslite.h>
#include <drmwmdrm.h>
#include <drmheaderparserconstants.h>

ENTER_PK_NAMESPACE_CODE;


static DRM_RESULT DRM_CALL _VerifyLegacyHeader(
    __in const DRM_CONST_STRING   *f_pdstrContentHeader,
    __in const DRM_CONST_STRING   *f_pdstrPubKey,
    __in       DRM_CRYPTO_CONTEXT *f_pContext )
{
    DRM_RESULT       dr                   = DRM_SUCCESS;
    DRM_CONST_STRING dstrData             = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue            = EMPTY_DRM_STRING;
    DRM_DWORD        dwDecodedChecksumLen = 0;

    ChkDRMString( f_pdstrContentHeader );

    /* Get the data section. */
    if( DRM_FAILED( DRM_XML_GetSubNode(  f_pdstrContentHeader,
                                        &g_dstrTagData,
                                         NULL,
                                         NULL,
                                         0,
                                        &dstrData,
                                         NULL,
                                         1 ) ) )
    {
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }

    /* Get the KID */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData,
                                              &g_dstrTagKID,
                                               NULL,
                                               NULL,
                                               NULL,
                                              &dstrValue,
                                               g_wchForwardSlash ) ) )
    {
        ChkDR( DRM_E_CH_KID_MISSING );
    }


    /* Get the LAINFO */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData,
                                              &g_dstrTagLAINFO,
                                               NULL,
                                               NULL,
                                               NULL,
                                              &dstrValue,
                                               g_wchForwardSlash ) ) )
    {
        ChkDR( DRM_E_CH_LAINFO_MISSING );
    }

    /* Get the CHECKSUM */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData,
                                              &g_dstrTagChecksum,
                                               NULL,
                                               NULL,
                                               NULL,
                                              &dstrValue,
                                               g_wchForwardSlash ) ) )
    {
        ChkDR( DRM_E_CH_CHECKSUM_MISSING );
    }

    /* Make sure that the Checksum is the correct size */
    dr = DRM_B64_DecodeW( &dstrValue, &dwDecodedChecksumLen, NULL, 0 );
    ChkBOOL(    ( CHECKSUM_LENGTH == dwDecodedChecksumLen || DRM_AES_CK_CHECKSUM_LENGTH == dwDecodedChecksumLen )
             &&   dr == DRM_E_BUFFERTOOSMALL,
             DRM_E_CH_INVALID_CHECKSUM );
    dr = DRM_SUCCESS;

    /*
    ** Signature check is optional - if specified by parameter then check signature.
    */
    if ( NULL != f_pdstrPubKey )
    {
        ChkDR( DRM_WMDRM_HDR_VerifyLegacySignature( f_pdstrContentHeader, f_pdstrPubKey, &dstrData, f_pContext ) );
    }

ErrorExit:
    return ( dr );
}


static DRM_RESULT DRM_CALL _VerifyV4Header(
    __in const DRM_CONST_STRING *f_pdstrContentHeader )
{
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_CONST_STRING dstrData  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;

    ChkArg( NULL != f_pdstrContentHeader );

    /* Get the data section. */
    if( DRM_FAILED( DRM_XML_GetSubNode(  f_pdstrContentHeader,
                                        &g_dstrTagData,
                                         NULL,
                                         NULL,
                                         0,
                                        &dstrData,
                                         NULL,
                                         1 ) ) )
    {
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }

    /* Get the KID */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData,
                                              &g_dstrTagKID,
                                               NULL,
                                               NULL,
                                               NULL,
                                              &dstrValue,
                                               g_wchForwardSlash ) ) )
    {
        ChkDR( DRM_E_CH_KID_MISSING );
    }

    /* Get the checksum node. It is optional, if it exists then validate the legnth */
    dr = DRM_XML_GetSubNodeByPath( &dstrData,
                                   &g_dstrTagChecksum,
                                    NULL,
                                    NULL,
                                    NULL,
                                   &dstrValue,
                                    g_wchForwardSlash );
    if ( DRM_E_XMLNOTFOUND == dr )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        DRM_DWORD dwDecodedChecksumLen = 0;

        ChkDR( dr );

        /* Make sure that the Checksum is the correct size */
        dr = DRM_B64_DecodeW( &dstrValue, &dwDecodedChecksumLen, NULL, 0 );
        ChkBOOL(    ( CHECKSUM_LENGTH == dwDecodedChecksumLen || DRM_AES_CK_CHECKSUM_LENGTH == dwDecodedChecksumLen )
                 &&   dr == DRM_E_BUFFERTOOSMALL,
                 DRM_E_CH_INVALID_CHECKSUM );

        dr = DRM_SUCCESS;
    }

ErrorExit:
    return ( dr );
}


static DRM_RESULT DRM_CALL _VerifyV4_1Header(
    __in const DRM_CONST_STRING *f_pdstrContentHeader )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_CONST_STRING dstrData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrProtectInfo = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSubData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAttribute   = EMPTY_DRM_STRING;

    ChkArg( NULL != f_pdstrContentHeader );

    /*
    ** technically the v4.1 header does not really need any other nodes besides WRMHEADER
    ** but do a little format checking if certain nodes exist.
    */
    if( DRM_SUCCEEDED( DRM_XML_GetSubNode(  f_pdstrContentHeader,
                                           &g_dstrTagData,
                                            NULL,
                                            NULL,
                                            0,
                                           &dstrData,
                                            NULL,
                                            1 ) ) )
    {
        if( DRM_SUCCEEDED( DRM_XML_GetSubNode(  f_pdstrContentHeader,
                                               &g_dstrTagData,
                                                NULL,
                                                NULL,
                                                1,  /* Look for second DATA node */
                                               &dstrData,
                                                NULL,
                                                1 ) ) )
        {
            /* Multiple DATA nodes are not allowed */
            ChkDR( DRM_E_CH_INVALID_HEADER );
        }

        /*
        ** PROTECTINFO section is optional, if specified do a little more checking...
        */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNode( &dstrData,
                                               &g_dstrTagProtectInfo,
                                                NULL,
                                                NULL,
                                                0,
                                               &dstrProtectInfo,
                                                NULL,
                                                1 ) ) )
        {
            if( DRM_SUCCEEDED( DRM_XML_GetSubNode( &dstrData,
                                                   &g_dstrTagProtectInfo,
                                                    NULL,
                                                    NULL,
                                                    1, /* Look for second PROTECTINFO node */
                                                   &dstrProtectInfo,
                                                    NULL,
                                                    1 ) ) )
            {
                /* Multiple PROTECTINFO nodes are not allowed */
                ChkDR( DRM_E_CH_INVALID_HEADER );
            }

            /*
            ** KID section is optional, if specified then there must be the ALGID and VALUE attributes and *no* data
            */
            if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrProtectInfo,
                                                         &g_dstrTagKID,
                                                          NULL,
                                                          NULL,
                                                         &dstrKID,
                                                         &dstrSubData,
                                                          g_wchForwardSlash ) ) )
            {
                if (    DRM_FAILED( DRM_XML_GetNodeAttribute( &dstrKID, &g_dstrTagAlgID,    &dstrAttribute ) )
                     || DRM_FAILED( DRM_XML_GetNodeAttribute( &dstrKID, &g_dstrTagValue,    &dstrAttribute ) )
                     || dstrSubData.cchString > 0 )
                {
                    ChkDR( DRM_E_CH_INVALID_HEADER );
                }
            }
        }

        /*
        ** LA_URL section is optional, if specified then there must be data
        */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData,
                                                     &g_dstrTagLAURL,
                                                      NULL,
                                                      NULL,
                                                      NULL,
                                                     &dstrSubData,
                                                      g_wchForwardSlash ) ) )
        {
            if( dstrSubData.cchString == 0 )
            {
                ChkDR( DRM_E_CH_INVALID_HEADER );
            }
        }

        /*
        ** LUI_URL section is optional, if specified then there must be data
        */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData,
                                                     &g_dstrTagLUIURL,
                                                      NULL,
                                                      NULL,
                                                      NULL,
                                                     &dstrSubData,
                                                      g_wchForwardSlash ) ) )
        {
            if( dstrSubData.cchString == 0 )
            {
                ChkDR( DRM_E_CH_INVALID_HEADER );
            }
        }

        /*
        ** DS_ID section is optional, if specified then there must be data
        */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData,
                                                     &g_dstrTagDSID,
                                                      NULL,
                                                      NULL,
                                                      NULL,
                                                     &dstrSubData,
                                                      g_wchForwardSlash ) ) )
        {
            if( dstrSubData.cchString == 0 )
            {
                ChkDR( DRM_E_CH_INVALID_HEADER );
            }
        }
    }

ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    DRM_HDR_GetHeaderVersion
**
** Synopsis:    This function returns version of XML header
**
** Arguments:
** [f_pdstrContentHeader] -- Pointer to a content header
** [f_peProperty]         -- Pointer to header type to store retrieved value
**
** Returns:     DRM_SUCCESS                  if version header was successfully detected
**              DRM_E_CH_UNSUPPORTED_VERSION if unsupported version of the header was found
**              DRM_E_CH_VERSION_MISSING     if version of header is missing
**              DRM_E_CH_INVALID_HEADER      if no WRMHEADER was found or header is too big
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDR_GetHeaderVersion(
    __in const DRM_CONST_STRING     *f_pdstrContentHeader,
    __out      eDRM_HEADER_VERSION  *f_peVersion )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrData = EMPTY_DRM_STRING;

    ChkArg( f_pdstrContentHeader != NULL );
    ChkArg( f_peVersion != NULL );

    *f_peVersion = DRM_HEADER_VERSION_UNKNOWN;

    /* Make sure that header has valid size */
    ChkBOOL( f_pdstrContentHeader->cchString * SIZEOF( DRM_WCHAR ) <= DRM_HEADER_MAX_SIZE_IN_BYTES, DRM_E_CH_INVALID_HEADER );

    /* Ensure the outter tag is WRMHEADER */
    if( DRM_FAILED( DRM_XML_GetNode( f_pdstrContentHeader, &g_dstrTagWrmHeader, NULL, NULL, 0, NULL, &dstrData ) ) )
    {
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }

    /* Make sure there is a version number */
    if( DRM_FAILED( DRM_XML_GetNodeAttribute( f_pdstrContentHeader, &g_dstrAttributeVersion, &dstrData ) ) ||
        dstrData.cchString == 0 )
    {
        ChkDR( DRM_E_CH_VERSION_MISSING );
    }

    /* Let's check version number */
    if ( DRM_UTL_DSTRStringsEqual( &dstrData, &g_dstrAttributeVersion2Value ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_2;
        /* Get the data section. */
        if( DRM_FAILED( DRM_XML_GetSubNode( f_pdstrContentHeader, &g_dstrTagData, NULL, NULL, 0, &dstrData, NULL, 1 ) ) )
        {
           ChkDR( DRM_E_CH_INVALID_HEADER );
        }
        if( DRM_SUCCEEDED( DRM_XML_GetSubNode( &dstrData, &g_dstrTagV4DATA, NULL, NULL, 0, &dstrData, NULL, 1 ) ) )
        {
           *f_peVersion = DRM_HEADER_VERSION_2_4;
        }
    }
    else if ( DRM_UTL_DSTRStringsEqual( &dstrData, &g_dstrAttributeVersion4Value ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_4;
    }
    else if ( DRM_UTL_DSTRStringsEqual( &dstrData, &g_dstrAttributeVersion4_1Value ) )
    {
        *f_peVersion = DRM_HEADER_VERSION_4_1;
    }
    else
    {
        ChkDR( DRM_E_CH_UNSUPPORTED_VERSION );
    }

ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    DRM_HDR_GetAttribute
**
** Synopsis:    This function retrieves attribute from XML content header
**
** Arguments:
** [pdstrContentHeader] -- Pointer to a content header
** [pdstrAttrName]      -- Attribute name, should be NULL for everything but custom attributes
** [eAttribute]         -- Enum value specifying some standard attribute's value
** [pdstrValue]         -- Value of requested attribute
** [chXPathSeparator]   -- Separator value
**
** Returns:     DRM_SUCCESS                  if attribute was retrived successfully
**              DRM_E_CH_INVALID_HEADER      if no WRMHEADER was found or no DATA section in the header
**              DRM_E_CH_VERSION_MISSING     if version of header is missing
**              DRM_E_CH_UNSUPPORTED_VERSION if unsupported header version is provided
**              DRM_E_CH_KID_MISSING         if KID tag is missing
**              DRM_E_CH_LAINFO_MISSING      if LAINFO tag is missing
**              DRM_E_CH_CHECKSUM_MISSING    if CHECKSUM tag is missing
**              DRM_E_INVALIDARG             if attribute specified incorrectly
**              DRM_E_INVALIDXMLTAG          if XML error is detected
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDR_GetAttribute(
    __in     const DRM_CONST_STRING       *pdstrContentHeader,
    __in_opt const DRM_CONST_STRING       *pdstrAttrName,
    __in           eDRM_HEADER_ATTRIBUTES  eAttribute,
    __out          DRM_CONST_STRING       *pdstrValue,
    __in           DRM_WCHAR               chXPathSeparator )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    const DRM_CONST_STRING  *pdstrString    = NULL;
    DRM_CONST_STRING         dstrData       = EMPTY_DRM_STRING;
    eDRM_HEADER_VERSION      eHeaderVersion = DRM_HEADER_VERSION_UNKNOWN;
    DRM_LONG                 lResult       = 0;

    ChkDRMString( pdstrContentHeader );
    ChkArg( pdstrValue != NULL );

    if( chXPathSeparator == 0 )
    {
        chXPathSeparator = g_wchForwardSlash;
    }

    if( (eAttribute != DRM_HEADER_ATTRIB_OTHER) && ( NULL != pdstrAttrName ) )
    {
        /* If caller wants a known attribute -- we don't allow an attribute string */
        ChkDR( DRM_E_INVALIDARG );
    }

    ChkDR( DRM_HDR_GetHeaderVersion( pdstrContentHeader, &eHeaderVersion ) );
    ChkDR( DRM_HDR_Verify( pdstrContentHeader, NULL, NULL, &lResult ) );
    ChkBOOL( 1 == lResult, DRM_E_CH_INVALID_HEADER );
    ChkDR( DRM_XML_GetSubNode( pdstrContentHeader, &g_dstrTagData, NULL, NULL, 0, &dstrData, NULL, 1 ) );

    switch( eAttribute )
    {
    case DRM_HEADER_ATTRIB_VERSION:
        pdstrString = &g_dstrAttributeVersion;
        ChkDR( DRM_XML_GetNodeAttribute( pdstrContentHeader, pdstrString, pdstrValue ) );
        break;
    case DRM_HEADER_ATTRIB_KID:
        if ( DRM_HEADER_VERSION_4_1 == eHeaderVersion )
        {
            DRM_CONST_STRING dstrProtectInfo = EMPTY_DRM_STRING;
            DRM_CONST_STRING dstrKID         = EMPTY_DRM_STRING;

            ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                             &g_dstrTagProtectInfo,
                                              NULL,
                                              NULL,
                                             &dstrProtectInfo,
                                              NULL,
                                              chXPathSeparator ) );

            ChkDR( DRM_XML_GetSubNodeByPath( &dstrProtectInfo,
                                             &g_dstrTagKID,
                                              NULL,
                                              NULL,
                                             &dstrKID,
                                              NULL,
                                              chXPathSeparator ) );

            ChkDR( DRM_XML_GetNodeAttribute( &dstrKID, &g_dstrTagValue, pdstrValue ) );
        }
        else
        {
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                             &g_dstrTagKID,
                                              NULL,
                                              NULL,
                                              NULL,
                                              pdstrValue,
                                              chXPathSeparator ) );
        }
        break;
    case DRM_HEADER_ATTRIB_LAINFO:
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagLAINFO,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    case DRM_HEADER_ATTRIB_CHECKSUM:
        if ( DRM_HEADER_VERSION_4_1 == eHeaderVersion )
        {
            DRM_CONST_STRING dstrProtectInfo = EMPTY_DRM_STRING;
            DRM_CONST_STRING dstrKID         = EMPTY_DRM_STRING;

            ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                             &g_dstrTagProtectInfo,
                                              NULL,
                                              NULL,
                                             &dstrProtectInfo,
                                              NULL,
                                              chXPathSeparator ) );

            ChkDR( DRM_XML_GetSubNodeByPath( &dstrProtectInfo,
                                             &g_dstrTagKID,
                                              NULL,
                                              NULL,
                                             &dstrKID,
                                              NULL,
                                              chXPathSeparator ) );

            ChkDR( DRM_XML_GetNodeAttribute( &dstrKID, &g_dstrTagChecksum, pdstrValue ) );
        }
        else
        {
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                             &g_dstrTagChecksum,
                                              NULL,
                                              NULL,
                                              NULL,
                                              pdstrValue,
                                              chXPathSeparator ) );
        }
        break;
    case DRM_HEADER_ATTRIB_ALGID:
    {
        DRM_CONST_STRING dstrProtectInfo = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrKID         = EMPTY_DRM_STRING;

        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagProtectInfo,
                                          NULL,
                                          NULL,
                                         &dstrProtectInfo,
                                          NULL,
                                          chXPathSeparator ) );


        if ( DRM_HEADER_VERSION_4_1 == eHeaderVersion )
        {
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrProtectInfo,
                                             &g_dstrTagKID,
                                              NULL,
                                              NULL,
                                             &dstrKID,
                                              NULL,
                                              chXPathSeparator ) );

            ChkDR( DRM_XML_GetNodeAttribute( &dstrKID, &g_dstrTagAlgID, pdstrValue ) );

        }
        else
        {
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrProtectInfo,
                                             &g_dstrTagAlgID,
                                              NULL,
                                              NULL,
                                              NULL,
                                              pdstrValue,
                                              chXPathSeparator ) );
        }
        break;
    }
    case DRM_HEADER_ATTRIB_SECURITYVERSION:
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagSecurityVersion,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    case DRM_HEADER_ATTRIB_OTHER:
        ChkArg( pdstrAttrName );
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                          pdstrAttrName,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    case DRM_HEADER_ATTRIB_LICENSE_ACQ_V4_URL:
        if ( DRM_HEADER_VERSION_2_4 == eHeaderVersion )
        {
            ChkDR( DRM_XML_GetSubNode( &dstrData,
                                       &g_dstrTagV4DATA,
                                        NULL,
                                        NULL,
                                        0,
                                       &dstrData,
                                        NULL,
                                        1 ) );
        }

        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagLAURL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    case DRM_HEADER_ATTRIB_LICENSE_UI_URL:
        if ( DRM_HEADER_VERSION_2_4 == eHeaderVersion )
        {
            ChkDR( DRM_XML_GetSubNode( &dstrData,
                                       &g_dstrTagV4DATA,
                                        NULL,
                                        NULL,
                                        0,
                                       &dstrData,
                                        NULL,
                                        1 ) );
        }

        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagLUIURL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    case DRM_HEADER_ATTRIB_DOMAIN_SERVICE_ID:
        if ( DRM_HEADER_VERSION_2_4 == eHeaderVersion )
        {
            ChkDR( DRM_XML_GetSubNode( &dstrData,
                                       &g_dstrTagV4DATA,
                                        NULL,
                                        NULL,
                                        0,
                                       &dstrData,
                                        NULL,
                                        1 ) );
        }

        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagDSID,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    case DRM_HEADER_ATTRIB_DECRYPTOR_SETUP:
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrData,
                                         &g_dstrTagDecryptorSetup,
                                          NULL,
                                          NULL,
                                          NULL,
                                          pdstrValue,
                                          chXPathSeparator ) );
        break;
    default:
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    return( dr );
}


/**********************************************************************
**
** Function:    DRM_HDR_GetUplink
**
** Synopsis:    This function returns value of DATA\UPLINKS tag (uplink KID of a specific level)
**
** Arguments:
** [pdstrContentHeader] -- Pointer to a content header
** [dwUplinkNumber]     -- Uplink KID level
** [pdstrKid]           -- requiested uplink KID
**
** Returns:     DRM_SUCCESS             if uplink KID was retrived successfully
**              DRM_E_INVALIDARG        if input parameter specified incorrectly
**              DRM_E_INVALIDXMLTAG     if XML error in content header is detected
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_HDR_GetUplink(
    __in const DRM_CONST_STRING *pdstrContentHeader,
    __in       DRM_DWORD         dwUplinkNumber,
    __out      DRM_CONST_STRING *pdstrKid )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrString = EMPTY_DRM_STRING;

    ChkDRMString( pdstrContentHeader );
    ChkArg( pdstrKid != NULL );

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrContentHeader, &g_dstrXPathHeaderUplinks, NULL, NULL, &dstrString, NULL, g_wchForwardSlash ) );
    ChkDR( DRM_XML_GetSubNode( &dstrString, &g_dstrTagUplink, NULL, NULL, dwUplinkNumber, NULL, pdstrKid, 1) );
ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    DRM_HDR_Verify
**
** Synopsis:    This function verifies content header
**
** Arguments:
** [pdstrContentHeader] -- Pointer to a content header
** [pdstrPubKey]        -- Public key. Optional.
** [pContext]           -- Cryptographic context. Optional but required if pdstrPubKey is set.
** [plResult]           -- Result of verification (0 or 1)
**
** Returns:     DRM_SUCCESS                  if uplink KID was retrived successfully
**              DRM_E_CH_INVALID_HEADER      if no WRMHEADER was found or no DATA section in the header
**              DRM_E_CH_VERSION_MISSING     if version of header is missing
**              DRM_E_CH_UNSUPPORTED_VERSION if unsupported header version is provided
**              DRM_E_CH_KID_MISSING         if KID tag is missing
**              DRM_E_CH_LAINFO_MISSING      if LAINFO tag is missing
**              DRM_E_CH_CHECKSUM_MISSING    if CHECKSUM tag is missing
**              DRM_E_CH_UNABLE_TO_VERIFY    if signature decoding fails
**              DRM_E_INVALIDARG             if attribute specified incorrectly
**              DRM_E_INVALIDXMLTAG          if XML error is detected
**              DRM_E_LOGICERR               on any other error
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDR_Verify(
    __in const DRM_CONST_STRING   *pdstrContentHeader,
    __in const DRM_CONST_STRING   *pdstrPubKey,
    __in       DRM_CRYPTO_CONTEXT *pContext,
    __out      DRM_LONG           *plResult )
{
    DRM_RESULT          dr             = DRM_SUCCESS;
    eDRM_HEADER_VERSION eHeaderVersion = DRM_HEADER_VERSION_UNKNOWN;

    ChkDRMString( pdstrContentHeader );
    ChkArg( plResult != NULL );

    ChkDR( DRM_HDR_GetHeaderVersion( pdstrContentHeader, &eHeaderVersion ) );

    switch ( eHeaderVersion )
    {
    case DRM_HEADER_VERSION_2: /* fallthrough */
    case DRM_HEADER_VERSION_2_4:
        ChkDR( _VerifyLegacyHeader( pdstrContentHeader, pdstrPubKey, pContext ) );
        break;
    case DRM_HEADER_VERSION_4:
        ChkDR( _VerifyV4Header( pdstrContentHeader ) );
        break;
    case DRM_HEADER_VERSION_4_1:
        ChkDR( _VerifyV4_1Header( pdstrContentHeader ) );
        break;
    case DRM_HEADER_VERSION_UNKNOWN: /* fallthrough */
    default:
        ChkDR( DRM_E_CH_UNSUPPORTED_VERSION );
    }

    *plResult = 1;

ErrorExit:
    if ( DRM_FAILED( dr ) && NULL != plResult )
    {
        *plResult = 0;
    }

    return ( dr );
}

EXIT_PK_NAMESPACE_CODE;

