/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMINITPARSER_C
#include <drmxmlparser.h>
#include <drmutilities.h>
#include <drmheaderparser.h>
#include <drmconstants.h>
#include <drminitparserconstants.h>
#include <drmmanager.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************************
**
** Function:    _ParseDomainInitiator
**
** Synopsis:    Function to parse JoinDomain or LeaveDomain PlayReady
**              Initiators message that both have the same format.
**              Example:
**              <LeaveDomain>
**                <DomainController>http://dc.contoso.com/dc.asmx</DomainController>
**                <DS_ID>AH+03juKbUGbHl1V/QIwRA==</DS_ID>
**                <AccountID>NjU0MzIxMDk4NzY1NDMyMQ==</AccountID>
**                <CustomData>This is custom data</CustomData>
**              </LeaveDomain>
**
**
** Arguments:
**
** [f_pdstrXML]         - pointer to PlayReady Initiator message to be parsed,
**                        either JoinDomain or LeaveDomain.
** [f_poData]           - pointer to output structure with join/leave domain data
**
** Returns:               DRM_SUCCESS on success.
**                        DRM_E_INITIATORS_MISSING_DC_URL if Domain controller URL is not found.
**                        DRM_E_DOMAIN_INVALID_GUID if service ID or account ID data is not valid.
**
** Notes:                 Only <DomainController> is required field.
**                        If present Service and Account IDs are base64-encoded.
**********************************************************************************/

static DRM_RESULT _ParseDomainInitiator(
    __in  const DRM_CONST_STRING     *f_pdstrXML,
    __out       DRM_INIT_DOMAIN_DATA *f_poData )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_STRING dstrDomainID  = EMPTY_DRM_STRING;
    DRM_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD  cbSize        = 0;

    ChkArg( f_poData != NULL );

    /*
    ** Look for domain server URL
    ** <DomainController>
    */
    dr = DRM_XML_GetSubNodeByPath(  f_pdstrXML,
                                    &g_dstrDomainControllerTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &(f_poData->dstrDomainServer),
                                    g_wchForwardSlash );

    ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_E_INITIATORS_MISSING_DC_URL : dr );

    /*
    ** <DS_ID> - service ID, optional field
    */
    ZEROMEM( &(f_poData->oServiceID), SIZEOF( f_poData->oServiceID ) );

    dr = DRM_XML_GetSubNodeByPath(  f_pdstrXML,
                                    &g_dstrServiceIDTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    (DRM_CONST_STRING*) &dstrDomainID,
                                    g_wchForwardSlash );

    if ( dr != DRM_E_XMLNOTFOUND )
    {
        ChkDR(dr);

        cbSize = SIZEOF( DRM_GUID );

        if ( DRM_FAILED( DRM_B64_DecodeW( (DRM_CONST_STRING*) &dstrDomainID,
                                          &cbSize,
                                          ( DRM_BYTE * )&(f_poData->oServiceID),
                                          0 ) )
            || cbSize != SIZEOF( DRM_GUID ) )
        {
            ChkDR( DRM_E_INITIATORS_INVALID_SERVICEID );
        }
    }

    /*
    ** AccountID - optional
    */
    ZEROMEM( &(f_poData->oAccountID), SIZEOF( f_poData->oAccountID ) );

    dr = DRM_XML_GetSubNodeByPath(  f_pdstrXML,
                                    &g_dstrAccountIDTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    (DRM_CONST_STRING*) &dstrAccountID,
                                    g_wchForwardSlash );

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR(dr);

        cbSize = SIZEOF( DRM_GUID );

        if ( DRM_FAILED( DRM_B64_DecodeW( (DRM_CONST_STRING*) &dstrAccountID,
                                          &cbSize,
                                          ( DRM_BYTE * )&(f_poData->oAccountID),
                                          0 ) )
            || cbSize != SIZEOF( DRM_GUID ) )
        {
            ChkDR( DRM_E_INITIATORS_INVALID_ACCOUNTID );
        }
    }

ErrorExit:
    return dr;
}


/**********************************************************************************
**
** Function:    _ParseLicAcqInitiator
**
** Synopsis:    Function to parse LicenseAcquisition PlayReady Initiator. Example:
**              <LicenseAcquisition>
**                <Header>
**                  <WRMHEADER version="4.0.0.0">
**                    <DATA>
**                      <LA_URL>http://rm.contoso.com/rightsmanager.asmx</LA_URL>
**                      <DS_ID>AH+03juKbUGbHl1V/QIwRA==</DS_ID>
**                      <KID>lFmb2gxg0Cr5bfEnJXgJeA==</KID>
**                      <CHECKSUM>P7ORpD2IpA==</CHECKSUM>
**                    </DATA>
**                  </WRMHEADER>
**                </Header>
**                <CustomData>This is custom data</CustomData>
**                <Content>http://content.contoso.com/lFmb2gxg0Cr5bfEnJdA.wma</Content>
**                <WMConvert>lFmb2gxg0Cr5bfEnJdA.pya</WMConvert>
**              </LicenseAcquisition>
**
** Arguments:
**
** [f_pdstrLicAcqXML]     - pointer to a LicenseAcquisition PlayReady Initiator message.
** [f_poData]             - pointer to output structure with license/content acquisition data
**
** Returns:  DRM_SUCCESS on success.
**           DRM_E_INITIATORS_MISSING_CONTENT_HEADER if content header data is not found.
**********************************************************************************/
static DRM_RESULT _ParseLicAcqInitiator(
    __in    const DRM_CONST_STRING     *f_pdstrLicAcqXML,
    __inout       DRM_INIT_LICACQ_DATA *f_poData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdstrLicAcqXML != NULL );
    ChkArg( f_poData != NULL );

    /* Look for the content header <Header> node. */
    dr = DRM_XML_GetSubNodeByPath(  f_pdstrLicAcqXML,
                                    &g_dstrHeaderTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &(f_poData->dstrContentHeader),
                                    g_wchForwardSlash );

    ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_E_INITIATORS_MISSING_CONTENT_HEADER : dr );

    /* Look for content server URL <Content> node. */
    dr = DRM_XML_GetSubNodeByPath(  f_pdstrLicAcqXML,
                                    &g_dstrContentURLTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &(f_poData->dstrContentURL),
                                    g_wchForwardSlash );

    ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_SUCCESS : dr );

    /* Look for the optional <WMConvert> node. */
    dr = DRM_XML_GetSubNodeByPath(  f_pdstrLicAcqXML,
                                    &g_dstrWMConvertTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &(f_poData->dstrWMConvert),
                                    g_wchForwardSlash );

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        f_poData->fWMConvert = FALSE;

        ChkDR( DRM_SUCCESS );
    }
    else
    {
        ChkDR( dr );

        f_poData->fWMConvert = TRUE;

        dr = DRM_HDR_GetAttribute( &f_poData->dstrContentHeader,
                                   NULL,
                                   DRM_HEADER_ATTRIB_LICENSE_ACQ_V4_URL,
                                   &f_poData->dstrLAURL,
                                   0 );

        ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_E_INITIATORS_MISSING_LAURL_IN_CONTENT_HEADER : dr );

        dr = DRM_HDR_GetAttribute( &f_poData->dstrContentHeader,
                                   NULL,
                                   DRM_HEADER_ATTRIB_LICENSE_UI_URL,
                                   &f_poData->dstrLUIURL,
                                   0 );

        ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_SUCCESS : dr );

        dr = DRM_HDR_GetAttribute( &f_poData->dstrContentHeader,
                                   NULL,
                                   DRM_HEADER_ATTRIB_DOMAIN_SERVICE_ID,
                                   &f_poData->dstrDSID,
                                   0 );

        ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_SUCCESS : dr );
    }

ErrorExit:
    return dr;
}


/**************************************************************************************************
**
** Function:    _ParseMeteringInitiator
**
** Synopsis:    Function to parse Metering PlayReady Initiator. Example:
**              <Metering>
**                  <CertificateServer>http://cert.contoso.com/cert.asmx</CertificateServer>
**                  <MeteringID>pIKx8Wsy5aYRQN8LLIIcTQ==</MeteringID>
**                  <CustomData>This is custom data</CustomData>
**                  <MaxPackets>3</MaxPackets>
**              </Metering>
**
** Arguments:
**
** [f_pdstrMeteringInitiatorXML] - pointer to a Metering PlayReady Initiator message.
** [f_poData]                    - pointer to output structure with metering data
**
** Returns:  DRM_SUCCESS on success.
**           DRM_E_INITIATORS_MISSING_METERCERT_URL if metering certificates server URL is not found.
**************************************************************************************************/
static DRM_RESULT _ParseMeteringInitiator(
    __in    const DRM_CONST_STRING       *f_pdstrMeteringInitiatorXML,
    __inout       DRM_INIT_METERING_DATA *f_poData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STRING dstrMaxPackets = EMPTY_DRM_STRING;
    DRM_STRING dstrMid        = EMPTY_DRM_STRING;
    DRM_DWORD  cbMid = DRM_ID_SIZE;

    ChkArg( f_pdstrMeteringInitiatorXML != NULL );
    ChkArg( f_poData != NULL );

    /*
    ** Look for certificate server URL
    ** <CertificateServer>
    */
    dr = DRM_XML_GetSubNodeByPath(  f_pdstrMeteringInitiatorXML,
                                    &g_dstrCertificateServerTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &(f_poData->dstrMeteringCertServer),
                                    g_wchForwardSlash );

    ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_E_INITIATORS_MISSING_METERCERT_URL : dr );

    /*
    ** Look for <MeteringID>: it is a based64-encoded GUID, optional node
    */
    ZEROMEM( &(f_poData->oMID), SIZEOF( f_poData->oMID ) );

    dr = DRM_XML_GetSubNodeByPath(  f_pdstrMeteringInitiatorXML,
                                    &g_dstrMeteringIDTag,
                                    NULL,
                                    NULL,
                                    NULL,
                                    (DRM_CONST_STRING*)&dstrMid,
                                    g_wchForwardSlash );

    if ( dr != DRM_E_XMLNOTFOUND )
    {
        ChkDR( dr );
        if( DRM_FAILED( DRM_B64_DecodeW( (DRM_CONST_STRING*)&dstrMid,
                                &cbMid,
                                ( DRM_BYTE * )&(f_poData->oMID),
                                0 ) )
                                || cbMid != DRM_ID_SIZE )
        {
            ChkDR( DRM_E_INITIATORS_INVALID_MID );
        }
    }

    /*
    ** Look for <MaxPackets>: this value is optional, try to read it and convert to int
    */
    dr = DRM_XML_GetSubNodeByPath( f_pdstrMeteringInitiatorXML,
                                   &g_dstrMaxPacketsTag,
                                   NULL,
                                   NULL,
                                   NULL,
                                   (DRM_CONST_STRING*)&dstrMaxPackets,
                                   g_wchForwardSlash );

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
        dr = DRMCRT_wcsntol ( dstrMaxPackets.pwszString, dstrMaxPackets.cchString, &(f_poData->lMaxPackets) );

        if ( DRM_FAILED(dr) )
        {
            f_poData->lMaxPackets = 0;
        }
    }

ErrorExit:
    return dr;
}


/***************************************************************************************
** Function:    Drm_Initiator_InitEnum
**
** Synopsis:    Function that receives a list of PlayReady initiator messages,
**              verifies that the top-level XML tag is present,
**              returns the XML with Initiators messages to the caller
**              and total number of Initiators messages.
**
**               <?xml version="1.0" encoding="utf-8"?>
**               <PlayReadyInitiator xmlns="http://schemas.microsoft.com/DRM/2007/03/protocols/">
**                 <JoinDomain>
**                   <DomainController>http://dc.contoso.com/dc.asmx</DomainController>
**                   <DS_ID>AH+03juKbUGbHl1V/QIwRA==</DS_ID>
**                   <AccountID>NjU0MzIxMDk4NzY1NDMyMQ==</AccountID>
**                   <CustomData>This is custom data</CustomData>
**                 </JoinDomain>
**                 <LicenseAcquisition>
**                   <Header>
**                     <WRMHEADER version="4.0.0.0">
**                       <DATA>
**                         <LA_URL>http://rm.contoso.com/rightsmanager.asmx</LA_URL>
**                         <DS_ID>AH+03juKbUGbHl1V/QIwRA==</DS_ID>
**                         <KID>lFmb2gxg0Cr5bfEnJXgJeA==</KID>
**                         <CHECKSUM>P7ORpD2IpA==</CHECKSUM>
**                       </DATA>
**                     </WRMHEADER>
**                   </Header>
**                   <CustomData>This is custom data</CustomData>
**                   <Content>http://content.contoso.com/lFmb2gxg0Cr5bfEnJdA.wma</Content>
**                 </LicenseAcquisition>
**               </PlayReadyInitiator>
**
** Arguments:
**
** [f_pdstrXML]      - pointer to a DRM_STRING with PlayReady Initiators list,
**                     should not be empty.
**
** [f_poEnumContext] - pointer to initialized enumerator context structure
**
** Returns:  DRM_SUCCESS on success.
**           DRM_E_XMLNOTFOUND - found invalid XML in data string.
**
** Note:     The function should be called before parsing a message list for verification purpose.
**           Assumed that API like DRM_STR_UTF8toDSTR() was already called to convert the message
**           from UTF-8 or UTF-16 encoding into WCHARs.
*****************************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Initiator_InitEnum(
    __in    const DRM_CONST_STRING            *f_pdstrXML,
    __inout       DRM_INITIATORS_ENUM_CONTEXT *f_poEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( f_pdstrXML != NULL);
    ChkDRMString(f_pdstrXML);
    ChkArg( f_poEnumContext != NULL );

    /*
    ** Verify that there is a top-level <PlayReadyInitiator> tag
    ** and set context data into whatever is between <PlayReadyInitiator> tags
    */
    ChkDR( DRM_XML_GetNode( f_pdstrXML,
                          &g_dstrPRInitiatorTag,
                          NULL,
                          NULL,
                          0,
                          NULL,
                          (DRM_CONST_STRING*) &f_poEnumContext->dstrXMLData ) );

    ASSIGN_DRM_STRING( f_poEnumContext->dstrCurrent, f_poEnumContext->dstrXMLData );

ErrorExit:
    return dr;
}


/****************************************************************************************
** Function:    Drm_Initiator_EnumNext
**
** Synopsis:    Function that retrieves the initiator message requested by its index
**              in the list.
**
**              Example of the list:
**
**             <JoinDomain>
**               <DomainController>http://dc.contoso.com/dc.asmx</DomainController>
**               <DS_ID>AH+03juKbUGbHl1V/QIwRA==</DS_ID>
**               <AccountID>NjU0MzIxMDk4NzY1NDMyMQ==</AccountID>
**               <CustomData>This is custom data</CustomData>
**             </JoinDomain>
**             <LicenseAcquisition>
**               <Header>
**                 <WRMHEADER version="4.0.0.0">
**                   <DATA>
**                     <LA_URL>http://rm.contoso.com/rightsmanager.asmx</LA_URL>
**                     <DS_ID>AH+03juKbUGbHl1V/QIwRA==</DS_ID>
**                     <KID>lFmb2gxg0Cr5bfEnJXgJeA==</KID>
**                     <CHECKSUM>P7ORpD2IpA==</CHECKSUM>
**                   </DATA>
**                 </WRMHEADER>
**               </Header>
**               <CustomData>This is custom data</CustomData>
**               <Content>http://content.contoso.com/lFmb2gxg0Cr5bfEnJdA.wma</Content>
**             </LicenseAcquisition>
**
** Arguments:
**
** [f_poEnumContext]  - pointer to enumerator context structure
**
** [f_oInitiatorData] - pointer to structure with initiator data, can be NULL.
**
** Returns:  DRM_SUCCESS on success.
**           DRM_E_XMLNOTFOUND if there was invalid XML in data string.
**           DRM_E_NOMORE if the list is empty, or read the last message in the list
**           DRM_E_INITIATORS_UNKNOWN_TYPE if XML tag name is not recognized.
****************************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Initiator_EnumNext(
    __inout     DRM_INITIATORS_ENUM_CONTEXT *f_poEnumContext,
    __inout_opt DRM_INITIATOR_DATA          *f_poInitiatorData )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_STRING       dstrTagName = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMsg = EMPTY_DRM_STRING;
    DRM_BOOL         fIsLeaf = FALSE;
    DRM_INITIATOR_TYPE eType = DRM_INIT_UNKNOWN;

    ChkArg( f_poEnumContext != NULL );
    ChkDRMString( &f_poEnumContext->dstrXMLData );
    ChkArg( f_poEnumContext->dstrXMLData.cchString >= f_poEnumContext->dstrCurrent.cchString );

    if ( f_poEnumContext->dstrCurrent.cchString == 0 )
    {
        ChkDR( DRM_E_NOMORE );
    }

    /*
    ** Read one initiator message
    */
    ChkDR( DRM_XML_EnumNextNode( &f_poEnumContext->dstrCurrent,
                                 (DRM_CONST_STRING*)&dstrTagName,
                                 &dstrMsg,
                                 NULL,
                                 NULL,
                                 NULL,
                                 &fIsLeaf ) );


    if ( DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING*)&dstrTagName, &g_dstrMeteringInitiator ) )
    {
        eType = DRM_INIT_METERING;
    }
    else if( DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING*)&dstrTagName, &g_dstrJoinDomainInitiator ) )
    {
        eType = DRM_INIT_JOINDOMAIN;
    }
    else if( DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING*)&dstrTagName, &g_dstrLeaveDomainInitiator ) )
    {
        eType = DRM_INIT_LEAVEDOMAIN;
    }
    else if( DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING*)&dstrTagName, &g_dstrLicenseAcquisitionInitiator ) )
    {
        eType = DRM_INIT_LICENSEACQUISITION;
    }
    else
    {
        ChkDR( DRM_E_INITIATORS_UNKNOWN_TYPE ); /* don't allow unrecognized tags on initiator type level */
    }

    if ( f_poInitiatorData != NULL )
    {
        ZEROMEM( f_poInitiatorData, SIZEOF( DRM_INITIATOR_DATA ) );

        f_poInitiatorData->eType = eType;

        switch ( eType )
        {
            case DRM_INIT_METERING:
                ChkDR( _ParseMeteringInitiator( &dstrMsg, &(f_poInitiatorData->Data.oMeteringData) ) );
                break;

            case DRM_INIT_JOINDOMAIN:
            case DRM_INIT_LEAVEDOMAIN:
                ChkDR( _ParseDomainInitiator( &dstrMsg, &(f_poInitiatorData->Data.oDomainData) ) );
                break;

            case DRM_INIT_LICENSEACQUISITION:
                ChkDR( _ParseLicAcqInitiator( &dstrMsg, &(f_poInitiatorData->Data.oLicAcqData) ) );
                break;

            default:
                ChkDR( DRM_E_INITIATORS_UNKNOWN_TYPE );
        }

        /*
        ** Extract custom data
        */
        dr = DRM_XML_GetSubNodeByPath(  &f_poEnumContext->dstrCurrent,
                                        &g_dstrCustomDataInitiatorTag,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &f_poInitiatorData->dstrCustomData,
                                        g_wchForwardSlash );

        ChkDR( dr == DRM_E_XMLNOTFOUND ? DRM_SUCCESS : dr );
    }
    /*
    ** Move forward
    */
    f_poEnumContext->dstrCurrent.pwszString = f_poEnumContext->dstrCurrent.pwszString + dstrMsg.cchString;
    f_poEnumContext->dstrCurrent.cchString = f_poEnumContext->dstrCurrent.cchString - dstrMsg.cchString;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

