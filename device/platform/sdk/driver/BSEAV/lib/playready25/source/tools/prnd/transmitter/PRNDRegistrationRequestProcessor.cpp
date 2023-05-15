/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDRegistrationRequestProcessor.h>
#include <FileLogger.h>
#include <HelperFunctions.h>

ENTER_PK_NAMESPACE_CODE;
extern  DRM_DWORD g_dwPRNDSessionTimeout; //defined in oeminfo.c
extern  DRM_LONG  g_lOEMTimeOffset;       //defined in oemtime.c
EXIT_PK_NAMESPACE_CODE;

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CPRNDRegistrationRequestProcessor::CPRNDRegistrationRequestProcessor()
{
    memset( &m_drmidSessionID, 0, sizeof( DRM_ID ) );
    m_bFailRegistration         = false;
    m_bNoRegistrationResponse   = false;

    m_spProximityDetectionConfig= make_shared< CProximityDetectionConfig >();
}

CPRNDRegistrationRequestProcessor::~CPRNDRegistrationRequestProcessor()
{

}

std::shared_ptr<CProximityDetectionConfig> CPRNDRegistrationRequestProcessor::GetProximityDetectionConfig()
{
    return m_spProximityDetectionConfig;
}

HRESULT CPRNDRegistrationRequestProcessor::PrepareErrorMessage(
                                                DRM_APP_CONTEXT *poAppContext,
                                                HRESULT          drResult )
{
    PRNDLogFunctionEntry();

    HRESULT     dr   = DRM_SUCCESS;

    ChkArg( FAILED( drResult ) );
    ChkArg( m_pbMessageResponse == NULL );
    ChkArg( m_cbMessageResponseSize == 0 );

    ChkDR( Drm_Prnd_Transmitter_RegistrationError_Generate(
            poAppContext,
            drResult,
            DRM_PRND_FLAG_NO_FLAGS,
            &m_pbMessageResponse,
            &m_cbMessageResponseSize ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDRegistrationRequestProcessor::ParseTransmitterAction( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;

    string                  strTagTransmitterAction     = "TransmitterAction";
    DRM_ANSI_CONST_STRING   dastrTag                    = { strTagTransmitterAction.c_str(), ( DRM_DWORD ) strTagTransmitterAction.size() };
    string strAction;

    ChkDR( DRM_XML_GetNodeA( strXml.c_str(), pdasstrXML, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    strAction = string( strXml, dasstrData.m_ich, dasstrData.m_cch );
    PRNDLogMessage( "Transmitter action = %s", strAction.c_str() );

    if( string_compare_case_insensitive( strAction, "FailRegistration" ) )
    {
        m_bFailRegistration = true;
    }

    if( string_compare_case_insensitive( strAction, "NoRegistrationResponse" ) )
    {
        m_bNoRegistrationResponse = true;
    }

    if( string_compare_case_insensitive( strAction, "NoPDStartResponse" ) )
    {
        m_spProximityDetectionConfig->m_bNoPDStartResponse = true;
    }

    if( string_compare_case_insensitive( strAction, "NoPDResponseResponse" ) )
    {
        m_spProximityDetectionConfig->m_bNoPDResponseResponse = true;
    }


ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDRegistrationRequestProcessor::ParseRegistration( string &strXml, DRM_SUBSTRING *pdasstrXML )
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrNode      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrNodeData  = EMPTY_DRM_SUBSTRING;
    DWORD                   cNodes          = 0;

    ChkDR( DRM_XML_CountMatchingNodesA( strXml.c_str(), pdasstrXML,  NULL, NULL, NULL, &cNodes ));
    PRNDLogMessage( "Node count = %d", cNodes );

    //Traverse all child nodes
    for( DWORD iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

        ChkDR( DRM_XML_EnumNextNodeA( strXml.c_str(),
                                    pdasstrXML,
                                    iNode,
                                   &dasstrTag,
                                   &dasstrNode,
                                   &dasstrNodeData,
                                    NULL,
                                    NULL ));

        string strTag( strXml, dasstrTag.m_ich, dasstrTag.m_cch );
        PRNDLogMessage( "Tag = %s", strTag.c_str() );

        string strNode( strXml, dasstrNode.m_ich, dasstrNode.m_cch );
        PRNDLogMessage( "Node = %s", strNode.c_str() );

        string strNodeData( strXml, dasstrNodeData.m_ich, dasstrNodeData.m_cch );
        PRNDLogMessage( "Node data = %s", strNodeData.c_str() );

        if( string_compare_case_insensitive( strTag, "TransmitterAction" ) )
        {
            ChkDR( ParseTransmitterAction( strXml, &dasstrNode ) );
        }

        if( string_compare_case_insensitive( strTag, "PRNDSessionTimeout" ) )
        {
            g_dwPRNDSessionTimeout = ( DRM_DWORD )std::stoi( strNodeData );
        }

        if( string_compare_case_insensitive( strTag, "ProximityFailureDuration" ) )
        {
            m_spProximityDetectionConfig->m_dwProximityFailureDuration = ( DWORD )std::stoi( strNodeData );
        }

        if( string_compare_case_insensitive( strTag, "TxRxClockDrift" ) )
        {
            g_lOEMTimeOffset = ( DRM_LONG )std::stoi( strNodeData );
        }

    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDRegistrationRequestProcessor::ParseRequestCustomData()
{
    PRNDLogFunctionEntry();

    HRESULT                 dr              = DRM_SUCCESS;
    DRM_SUBSTRING           dasstrData      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrOffSet    = EMPTY_DRM_SUBSTRING;

    string                  strXml;
    string                  strTagRegistration  = "Registration";
    DRM_ANSI_CONST_STRING   dastrTag            = { strTagRegistration.c_str(), ( DRM_DWORD ) strTagRegistration.size() };

    if( m_pbRequestCustomData == NULL )
    {
        goto ErrorExit;
    }
    strXml = string( ( char* )m_pbRequestCustomData, m_cbRequestCustomDataSize );

    //Traverse to Registration node data
    dasstrOffSet.m_ich = 0;
    dasstrOffSet.m_cch = ( DRM_DWORD ) strXml.length();
    ChkDR( DRM_XML_GetNodeA( strXml.c_str(),&dasstrOffSet, &dastrTag, NULL, NULL, 0, NULL, &dasstrData ) );

    ( void ) ParseRegistration( strXml, &dasstrData );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDRegistrationRequestProcessor::ProcessRequestCustomData()
{
    PRNDLogFunctionEntry();

    HRESULT  dr = DRM_SUCCESS;

    ChkDR( ParseRequestCustomData() );
    if( m_bFailRegistration )
    {
        dr = 0x80009999;
        PRNDLogMessage( "Requesting registration failure..." );
        goto ErrorExit;
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


/*
-------Custom Data Format Expected------------------
<Registration>
    <TransmitterAction>FailRegistration</TransmitterAction>                //Authorization failure
    <TransmitterAction>NoRegistrationResponse</TransmitterAction>
    <TransmitterAction>NoPDStartResponse</TransmitterAction>
    <TransmitterAction>NoPDResponseResponse</TransmitterAction>
    <PRNDSessionTimeout>60</PRNDSessionTimeout>                            //Timeout in seconds
    <ProximityFailureDuration>60</ProximityFailureDuration>                //Continue to fail for n seconds
    <TxRxClockDrift>-60</TxRxClockDrift>                                   //n or -n seconds
</Registration>
*/

HRESULT  DRM_CALL CPRNDRegistrationRequestProcessor::Drm_Prnd_Data_Callback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *pvDataCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *pCertificateData,
    __in_opt                             const DRM_ID                                          *pdrmidCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *pbCustomData,
    __in                                       DRM_DWORD                                        cbCustomData )
{
    PRNDLogFunctionEntry();
    HRESULT     dr   = DRM_SUCCESS;
    CPRNDRegistrationRequestProcessor   *pRequestProcessor = ( CPRNDRegistrationRequestProcessor* ) pvDataCallbackContext;

    if( pbCustomData != NULL )
    {
        PRNDLogHexDump( (BYTE* ) pbCustomData, cbCustomData, true );
        ChkDR( pRequestProcessor->SetRequestCustomData( pdrmidCustomDataTypeID, ( BYTE* ) pbCustomData, cbCustomData ) );
        ChkDR( pRequestProcessor->ProcessRequestCustomData() );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CPRNDRegistrationRequestProcessor::ProcessMessage(
                                        __in DRM_APP_CONTEXT   *poAppContext,
                                        __inout CPRNDMemHolder &message,
                                        __inout CPRNDMemHolder &response
                                        )
{
    PRNDLogFunctionEntry();
    HRESULT                             dr                                      = DRM_SUCCESS;
    DRM_PRND_PROXIMITY_DETECTION_TYPE   eProximityDetectionType;
    DRM_BYTE                           *pbTransmitterProximityDetectionChannel  = NULL;
    DRM_DWORD                           cbTransmitterProximityDetectionChannel  = 0;
    DRM_DWORD                           dwFlagsOut                              = 0;

    DRMASSERT( m_pbMessageRequest == NULL );
    DRMASSERT( m_cbMessageRequestSize == 0 );

    AdoptMessage( message );

    DRMASSERT( m_pbMessageRequest != NULL );
    DRMASSERT( m_cbMessageRequestSize != 0 );

    DRMASSERT( m_pbMessageResponse == NULL );
    DRMASSERT( m_cbMessageResponseSize == 0 );

    PRNDLogMessage( "Generating response from PK.." );
    dr = Drm_Prnd_Transmitter_RegistrationRequest_Process(
            poAppContext,
            m_pbMessageRequest,
            m_cbMessageRequestSize,
            CPRNDRegistrationRequestProcessor::Drm_Prnd_Data_Callback,
            this,
           &m_drmidSessionID,
           &eProximityDetectionType,
           &pbTransmitterProximityDetectionChannel,
           &cbTransmitterProximityDetectionChannel,
           &dwFlagsOut );

    if( pbTransmitterProximityDetectionChannel != NULL )
    {
        PRNDLogMessage( "TransmitterProximityDetectionChannel:" );
        PRNDLogHexDump( pbTransmitterProximityDetectionChannel, cbTransmitterProximityDetectionChannel, true );
        DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );
    }

    if( SUCCEEDED( dr ) )
    {
        dr = CopyRequestCustomDataToResponseCustomData();
    }

    if( SUCCEEDED( dr ) )
    {
        dr = Drm_Prnd_Transmitter_RegistrationResponse_Generate(
                poAppContext,
                m_pbResponseCustomData == NULL ? NULL : &m_drmidResponseCustomDataTypeID,
                m_pbResponseCustomData,
                m_cbResponseCustomDataSize,
                DRM_PRND_FLAG_NO_FLAGS,
               &m_pbMessageResponse,
               &m_cbMessageResponseSize );
    }

    if( FAILED( dr ) )
    {
        //
        //since registration failed, no need to validate session ID
        //
        m_bValidateSessionID = false;
        PRNDLogMessage( "PrepareErrorMessage for dr  = 0x%X", dr );
        ChkDR( PrepareErrorMessage( poAppContext, dr ) );
    }

    if( !m_bNoRegistrationResponse )
    {
        ExtractResponse( response );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


