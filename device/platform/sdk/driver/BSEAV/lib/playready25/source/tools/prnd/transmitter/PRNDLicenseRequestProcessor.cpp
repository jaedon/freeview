/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDLicenseRequestProcessor.h>
#include <FileLogger.h>
#include <HelperFunctions.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CPRNDLicenseRequestProcessor::CPRNDLicenseRequestProcessor()
{
    m_spLicenseGenerator = make_shared<CLicenseGenerator>();
    m_bNoLicenseResponse   = false;
}

CPRNDLicenseRequestProcessor::~CPRNDLicenseRequestProcessor()
{
    PRNDLogFunctionEntry();

    PRNDLogFunctionExit();
}

HRESULT CPRNDLicenseRequestProcessor::PrepareErrorMessage(
                                                DRM_APP_CONTEXT *poAppContext,
                                                HRESULT          drResult )
{
    PRNDLogFunctionEntry();

    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( FAILED( drResult ) );
    DRM_PRND_SAFEMEMFREE( m_pbMessageResponse );
    m_cbMessageResponseSize = 0;

    ChkDR( Drm_Prnd_Transmitter_LicenseError_Generate(  poAppContext,
                                                        NULL,
                                                        drResult,
                                                        DRM_PRND_FLAG_NO_FLAGS,
                                                       &m_pbMessageResponse,
                                                       &m_cbMessageResponseSize ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDLicenseRequestProcessor::ParseTransmitterAction( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;

    string                  strTagTransmitterAction     = "TransmitterAction";
    DRM_ANSI_CONST_STRING   dastrTag                    = { strTagTransmitterAction.c_str(), ( DRM_DWORD ) strTagTransmitterAction.size() };

    string strAction;

    //Traverse to TransmitterAction node data
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    strAction = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
    PRNDLogMessage( "Transmitter action = %s", strAction.c_str() );

    if( string_compare_case_insensitive( strAction, "NoLicenseResponse" ) )
    {
        m_bNoLicenseResponse = true;
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDLicenseRequestProcessor::ProcessRequestCustomData()
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrOffSet    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DWORD                   cNodes          = 0;
    string                  strXml;
    string                  strTagLicenseRequest    = "LicenseRequest";
    DRM_ANSI_CONST_STRING   dastrTag            = { strTagLicenseRequest.c_str(), ( DRM_DWORD ) strTagLicenseRequest.size() };

    if( m_pbRequestCustomData == NULL )
    {
        goto ErrorExit;
    }

    strXml = string( ( char* )m_pbRequestCustomData, m_cbRequestCustomDataSize );
    strXml = string_trim( strXml, " " );
    if( strXml.empty() )
    {
        goto ErrorExit;
    }

    //Traverse to LicenseRequest node first...
    dasstrOffSet.m_ich = 0;
    dasstrOffSet.m_cch = ( DRM_DWORD ) strXml.length();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), &dasstrOffSet, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    // then count the children of the LicenseRequest node...
    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), &dasstrData,  NULL, NULL, NULL, &cNodes ));
    PRNDLogMessage( "Node count = %d", cNodes );

    //Traverse all child nodes
    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                    &dasstrData,
                                    iNode,
                                   &dasstrTag,
                                   &dasstrNode,
                                    NULL,
                                    NULL,
                                    NULL ));

        string strTag( strXml, dasstrTag.m_ich, dasstrTag.m_cch );
        PRNDLogMessage( "Tag = %s", strTag.c_str() );

        string strNode( strXml, dasstrNode.m_ich, dasstrNode.m_cch );
        PRNDLogMessage( "Node = %s", strNode.c_str() );

        if( string_compare_case_insensitive( strTag, "TransmitterAction" ) )
        {
            ChkDR( ParseTransmitterAction( strXml, &dasstrNode ) );
        }

        if( string_compare_case_insensitive( strTag, "RightsPool" ) )
        {
            ChkDR( m_spLicenseGenerator->ProcessRequestCustomData( strXml, &dasstrNode ) );
        }
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


// called before Drm_Prnd_Content_Identifier_Callback by PK
HRESULT  DRM_CALL CPRNDLicenseRequestProcessor::Drm_Prnd_Data_Callback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *pvDataCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *pbCustomData,
    __in                                       DRM_DWORD                                        cbCustomData )
{
    PRNDLogFunctionEntry();
    HRESULT                         dr                  = DRM_SUCCESS;
    CPRNDLicenseRequestProcessor   *pRequestProcessor   = ( CPRNDLicenseRequestProcessor* ) pvDataCallbackContext;
    DRM_KID                         drmidCustomDataType = {0};

    if( pbCustomData == NULL )
    {
        string strDefaultCustomData = "<LicenseRequest>";
        strDefaultCustomData += "<RightsPool>";
        strDefaultCustomData += "<KeyRotation Required=\"true\" Duration=\"20\"></KeyRotation>";
        strDefaultCustomData += "<LicenseRights MinimumSecurityLevel=\"150\" Type=\"RemoteBoundRoot\" > <Play></Play></LicenseRights>";
        strDefaultCustomData += "<LicenseRights MinimumSecurityLevel=\"150\" Type=\"Leaf\" > <Play></Play></LicenseRights>";
        strDefaultCustomData += "</RightsPool>";
        strDefaultCustomData += "</LicenseRequest>";

        PRNDLogHexDump( (BYTE* ) strDefaultCustomData.c_str(), ( DRM_DWORD ) strDefaultCustomData.size(), true );
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&drmidCustomDataType, SIZEOF( DRM_KID )) );
        ChkDR( pRequestProcessor->SetRequestCustomData( &drmidCustomDataType, ( BYTE* ) strDefaultCustomData.c_str(), ( DRM_DWORD ) strDefaultCustomData.size() ) );
    }
    else
    {
        PRNDLogHexDump( (BYTE* ) pbCustomData, cbCustomData, true );
        ChkDR( pRequestProcessor->SetRequestCustomData( f_pCustomDataTypeID, ( BYTE* ) pbCustomData, cbCustomData ) );
    }
    ChkDR( pRequestProcessor->ProcessRequestCustomData() );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


// called after Drm_Prnd_Data_Callback by PK
DRM_RESULT DRM_CALL CPRNDLicenseRequestProcessor::Drm_Prnd_Content_Identifier_Callback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *pvContentIdentifierCallbackContext,
    __in                                       DRM_PRND_CONTENT_IDENTIFIER_TYPE                 eContentIdentifierType,
    __in_bcount( f_cbContentIdentifier ) const DRM_BYTE                                        *pbContentIdentifier,
    __in                                       DRM_DWORD                                        cbContentIdentifier,
    __in_ecount_opt( 1 )                       DRM_KID                                         *pkidContent )
{
    PRNDLogFunctionEntry();
    HRESULT     dr   = DRM_SUCCESS;
    DRM_KID    oKid  = {0};

    ChkArg( pvContentIdentifierCallbackContext != NULL );
    CPRNDLicenseRequestProcessor   *pRequestProcessor = ( CPRNDLicenseRequestProcessor* ) pvContentIdentifierCallbackContext;

    //
    // KID can be for simple license or root license
    //
    if( eContentIdentifierType == DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID )
    {
        PRNDLogMessage( "Content identifier type = KID" );
        pRequestProcessor->SetKeyId( ( DRM_GUID* )pkidContent );
    }
    else if( eContentIdentifierType == DRM_PRND_CONTENT_IDENTIFIER_TYPE_CUSTOM )
    {
        PRNDLogMessage( "Content identifier type = CUSTOM" );
        if( cbContentIdentifier == sizeof( DRM_GUID ) )
        {
            pRequestProcessor->SetKeyId( ( DRM_GUID* )pbContentIdentifier );
        }
        else
        {
            PRNDLogMessage( "Content identifier type not supported, id size = %d", cbContentIdentifier );
            ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&oKid, SIZEOF( DRM_KID )) );
            pRequestProcessor->SetKeyId( ( DRM_GUID* )&oKid );
        }
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

shared_ptr<CLicenseGenerator> CPRNDLicenseRequestProcessor::GetLicenseGenerator()
{
    PRNDLogFunctionEntry();
    return m_spLicenseGenerator;
}

HRESULT CPRNDLicenseRequestProcessor::SetKeyId(  DRM_GUID  *pKeyId )
{
    PRNDLogFunctionEntry();
    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( pKeyId != NULL );
    m_spLicenseGenerator->SetKeyId( pKeyId );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDLicenseRequestProcessor::ProcessMessage(
                                        __in    DRM_APP_CONTEXT *poAppContext,
                                        __inout CPRNDMemHolder  &message,
                                        __inout CPRNDMemHolder  &response
                                        )
{
    PRNDLogFunctionEntry();
    HRESULT     dr          = DRM_SUCCESS;
    DRM_DWORD   dwFlagsOut  = 0;

    DRMASSERT( m_pbMessageRequest == NULL );
    DRMASSERT( m_cbMessageRequestSize == 0 );

    AdoptMessage( message );

    DRMASSERT( m_pbMessageRequest != NULL );
    DRMASSERT( m_cbMessageRequestSize != 0 );

    DRMASSERT( m_pbMessageResponse == NULL );
    DRMASSERT( m_cbMessageResponseSize == 0 );

    PRNDLogMessage( "Calling Drm_Prnd_Transmitter_LicenseRequest_Process()" );
    ChkDR( Drm_Prnd_Transmitter_LicenseRequest_Process(
        poAppContext,
        m_pbMessageRequest,
        m_cbMessageRequestSize,
        Drm_Prnd_Data_Callback,
        this,
        Drm_Prnd_Content_Identifier_Callback,
        this,
        &dwFlagsOut ));

    ChkDR( CopyRequestCustomDataToResponseCustomData() );
    ChkDR( m_spLicenseGenerator->CreateLicensesForDelivery( poAppContext ) );

    PRNDLogMessage( "Calling Drm_Prnd_Transmitter_LicenseTransmit_Generate()" );
    ChkDR( Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        poAppContext,
        m_pbResponseCustomData == NULL ? NULL : &m_drmidResponseCustomDataTypeID,
        m_pbResponseCustomData,
        m_cbResponseCustomDataSize,
        DRM_PRND_FLAG_NO_FLAGS,
       &m_pbMessageResponse,
       &m_cbMessageResponseSize ));

ErrorExit:
    if( FAILED( dr ) )
    {
        PRNDLogMessage( "ErrorExit %s() with result = 0x%X", __FUNCTION__, dr );
        PrepareErrorMessage( poAppContext, dr );
    }

    if( !m_bNoLicenseResponse )
    {
        ExtractResponse( response );
    }

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}
