/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <initparsertest.h>
#include <drmmathsafe.h>
#include <drmmanager.h>

ENTER_PKTEST_NAMESPACE_CODE;

/***************************************************************************
**
** Function: TestDrm_Initiator_EnumNext
**
** Synopsis: Function tests Drm_Initiator_EnumNext() API
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
**
** argv[0]    -- XML Data string. May be set into a special values
**               "nullstring" to imitate empty string (pwszString == NULL, cchString == 0)
**               "nullptr" to pass null ptr instead of DRM_STRING,
**               "zerolen" to pass string with cchString == 0.
**
** argv[1]    -- index of initiator message to read (optional, default is 0)
** argv[2]    -- expected initiator type (optional).
**               If it is not set into >= 1 && <= 4 then argv[4]...[6] must be present.
** argv[3]    -- expected custom data (optional, "nullstring" is not present in the msg)
** argv[4]    -- expected URL field (optional). Can be DC URL, meter cert URL or content download URL
**               depending on expected initiator type. For some LA initiators may not be present,
**               put "nullstring".
** argv[5]    -- Optional parameter 1 whose meaning depends on expected initiator type:
**               Join/LeaveDomain - expected service ID in base64 encoding
**               LicAcqusition    - expected content header
**               Metering         - expected MID in base64 encoding
** argv[6]    -- Optional parameter 2 whose meaning depends on expected initiator type:
**               Join/LeaveDomain - expected account ID in base64 encoding
**               LicAcqusition    - ignored
**               Metering         - expected number of packets
**
** Returns:      DRM_RESULT
***************************************************************************/
DRM_RESULT DRM_CALL TestDrm_Initiator_EnumNext(
        long argc,
        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr                  = DRM_SUCCESS;
    DRM_STRING dstrTmpStr = EMPTY_DRM_STRING;
    DRM_WCHAR        wszXMLData[1024] = { 0 };
    DRM_DWORD  dwIndex = 0;
    DRM_DWORD  dwExpectedIndex = 0;
    DRM_INITIATORS_ENUM_CONTEXT oEnumContext = {0};
    DRM_INITIATORS_ENUM_CONTEXT *poEnumContext = NULL;
    DRM_INITIATOR_DATA oInitiatorData = EMPTY_DRM_INITIATOR_DATA;

    DRM_INITIATOR_TYPE eExpectedType = DRM_INIT_UNKNOWN;

    DRM_WCHAR  wszExpectedCustomData[1024] = { 0 };
    DRM_WCHAR  wszExpectedURL[256] = { 0 };
    DRM_WCHAR  wszParam2[64] = { 0 };
    DRM_STRING dstrParam1 = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 && argc <= 8 );

    if ( strcmp( argv[0], "nullstring" ) == 0 )
    {
        oEnumContext.dstrXMLData.pwszString = NULL;
        oEnumContext.dstrXMLData.cchString = 0;

        ASSIGN_DRM_STRING(oEnumContext.dstrCurrent, oEnumContext.dstrXMLData);

        poEnumContext = &oEnumContext;
    }
    else if ( strcmp( argv[0], "nullptr" ) == 0 )
    {
        poEnumContext = NULL;
    }
    else if ( strcmp( argv[0], "zerolen" ) == 0 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );

        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 ); /* assign the string to something != NULL */
        oEnumContext.dstrXMLData.pwszString = wszXMLData;

        oEnumContext.dstrXMLData.cchString = 0;

        ASSIGN_DRM_STRING(oEnumContext.dstrCurrent, oEnumContext.dstrXMLData);

        poEnumContext = &oEnumContext;
    }
    else
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );

        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );
        oEnumContext.dstrXMLData.pwszString = wszXMLData;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszXMLData ), &oEnumContext.dstrXMLData.cchString ) );

        ASSIGN_DRM_STRING(oEnumContext.dstrCurrent, oEnumContext.dstrXMLData);

        poEnumContext = &oEnumContext;
    }

    if ( argc >= 2 )
    {
        dwExpectedIndex = (DRM_DWORD)atol( argv[1] );
    }

    if ( argc >= 3 )
    {
        DRM_DWORD i = (DRM_DWORD) atol( argv[2] );
        switch (i)
        {
            case 1:
                eExpectedType = DRM_INIT_JOINDOMAIN;
                break;
            case 2:
                eExpectedType = DRM_INIT_LEAVEDOMAIN;
                break;
            case 3:
                eExpectedType = DRM_INIT_LICENSEACQUISITION;
                break;
            case 4:
                eExpectedType = DRM_INIT_METERING;
                break;
            default:
                eExpectedType = DRM_INIT_UNKNOWN;
        }
    }

    if ( eExpectedType != DRM_INIT_UNKNOWN ) /* if expected initiator message type is present then the rest of expected data should be here too. */
    {
        if ( argc < 5 )
        {
            Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: error in number of arguments." );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        if ( strcmp( argv[3], "nullstring" ) )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[3]), &cch ) );
            ChkArg( cch + 1 <= NO_OF( wszExpectedCustomData ) );

            TST_UTL_mbstowcs( wszExpectedCustomData, argv[3], cch + 1 );
        }

        if ( strcmp( argv[4], "nullstring" ) )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[4]), &cch ) );
            ChkArg( cch + 1 <= NO_OF( wszExpectedURL ) );

            TST_UTL_mbstowcs( wszExpectedURL, argv[4], cch + 1 );
        }
    }

    /*
    ** Read most of the arguments (param1 and param2 will be read later)
    ** Now get initiator message
    */
    while ( dr != DRM_E_NOMORE && dwIndex <= dwExpectedIndex )
    {
        dr = Drm_Initiator_EnumNext( poEnumContext, &oInitiatorData );
        if ( dr == DRM_E_NOMORE )
        {
            break;
        }
        ChkDR(dr);
        if (dwIndex != dwExpectedIndex)
        {
            dwIndex++;
            continue;
        }

        /* verify this initiator's data */

        if (eExpectedType == DRM_INIT_UNKNOWN ) /*test is done, the goal was to simply reach this message, so exit*/
        {
            goto ErrorExit;
        }

        if ( oInitiatorData.eType != eExpectedType )
        {
            Log("Trace", "\t\tInitiator message type %d does not match expected %d", oInitiatorData.eType, eExpectedType);
            ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
        }
        else
        {
            /*
            ** Compare the rest of initiator data
            */
            dstrTmpStr.pwszString = wszExpectedCustomData;
            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszExpectedCustomData ), &dstrTmpStr.cchString ) );
            if ( dstrTmpStr.cchString > 0
                && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oInitiatorData.dstrCustomData),
                                              (const DRM_CONST_STRING *)&dstrTmpStr ) )
            {
                Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: custom data does not match" );
                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            }

            dstrTmpStr.pwszString = wszExpectedURL;
            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszExpectedURL ), &dstrTmpStr.cchString ) );
            switch( oInitiatorData.eType )
            {
                case DRM_INIT_JOINDOMAIN:
                case DRM_INIT_LEAVEDOMAIN:
                    if ( dstrTmpStr.cchString > 0
                        && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oInitiatorData.Data.oDomainData.dstrDomainServer),
                                                      (const DRM_CONST_STRING *)&dstrTmpStr ) )
                    {
                        Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: Domain URL does not match" );
                        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                    }
                    break;
                case DRM_INIT_LICENSEACQUISITION:
                    if ( dstrTmpStr.cchString > 0
                        && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oInitiatorData.Data.oLicAcqData.dstrContentURL),
                                                      (const DRM_CONST_STRING *)&dstrTmpStr ) )
                    {
                        Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: content download URL does not match" );
                        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                    }
                    break;
                case DRM_INIT_METERING:
                    if ( dstrTmpStr.cchString > 0
                        && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oInitiatorData.Data.oMeteringData.dstrMeteringCertServer),
                                                      (const DRM_CONST_STRING *)&dstrTmpStr ) )
                    {
                        Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: metering certs server URL does not match" );
                        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                    }
                    break;
                default:
                    ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            }
            if ( argc >= 6 )
            {
                if ( strcmp( argv[5], "nullstring" ) )
                {
                    FREEDRMSTRING( dstrParam1 );
                    ChkDR( MakeDRMString( ( DRM_CONST_STRING * ) &dstrParam1, argv[5] ) );
                    dstrTmpStr = dstrParam1;
                }
                else
                {
                    dstrTmpStr.pwszString = NULL;
                    dstrTmpStr.cchString = 0;
                }

                switch( oInitiatorData.eType )
                {
                    case DRM_INIT_JOINDOMAIN:
                    case DRM_INIT_LEAVEDOMAIN:
                        if ( dstrTmpStr.cchString > 0 )
                        {
                            DRM_DWORD cbSize             = SIZEOF( DRM_GUID );
                            DRM_GUID  oExpectedServiceID = EMPTY_DRM_GUID;

                            ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *)&dstrTmpStr, &cbSize, ( DRM_BYTE * )&oExpectedServiceID, 0 ) );

                            if ( !IDENTICAL_GUIDS(&(oInitiatorData.Data.oDomainData.oServiceID), &oExpectedServiceID ) )
                            {
                                Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: service ID does not match" );
                                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                            }
                        }
                        break;
                    case DRM_INIT_LICENSEACQUISITION:
                        if ( dstrTmpStr.cchString > 0
                            && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oInitiatorData.Data.oLicAcqData.dstrContentHeader),
                                                          (const DRM_CONST_STRING *)&dstrTmpStr ) )
                        {
                            Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: content header does not match" );
                            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                        }
                        break;
                    case DRM_INIT_METERING:
                        if ( dstrTmpStr.cchString > 0 )
                        {
                            DRM_MID   oExpectedMid = {0};
                            DRM_DWORD cbMid        = SIZEOF( oExpectedMid.rgb );

                            ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *)&dstrTmpStr, &cbMid, oExpectedMid.rgb, 0 ) );

                            if ( DRM_BYT_CompareBytes( oInitiatorData.Data.oMeteringData.oMID.rgb, 0, oExpectedMid.rgb, 0, SIZEOF( oExpectedMid.rgb ) ) )
                            {
                                Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: MID does not match" );
                                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                            }
                        }
                        break;
                    default:
                        ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
                }
            }
            if ( argc == 7 )
            {
                DRM_DWORD cch = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[6]), &cch ) );
                ChkArg( cch + 1 <= NO_OF( wszParam2 ) );

                TST_UTL_mbstowcs( wszParam2, argv[6], cch + 1 );
                switch( oInitiatorData.eType )
                {
                    case DRM_INIT_JOINDOMAIN:
                    case DRM_INIT_LEAVEDOMAIN:
                        if ( dstrTmpStr.cchString > 0 )
                        {
                            DRM_DWORD cbSize             = SIZEOF( DRM_GUID );
                            DRM_GUID  oExpectedAccountID = EMPTY_DRM_GUID;

                            dstrTmpStr.pwszString = wszParam2;
                            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszParam2 ), &dstrTmpStr.cchString ) );
                            ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *)&dstrTmpStr, &cbSize, ( DRM_BYTE * )&oExpectedAccountID, 0 ) );

                            if ( !IDENTICAL_GUIDS(&(oInitiatorData.Data.oDomainData.oAccountID), &oExpectedAccountID ) )
                            {
                                Log( "Trace", "\t\tFrom TestDrm_Initiator_EnumNext: account ID does not match" );
                                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                            }
                        }
                        break;
                    case DRM_INIT_LICENSEACQUISITION:
                        break;
                    case DRM_INIT_METERING:
                        if ( dstrTmpStr.cchString > 0 )
                        {
                            DRM_LONG lExpectedPackets = 0;
                            DRM_DWORD cch2 = 0;
                            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszParam2 ), &cch2 ) );
                            ChkDR( DRMCRT_wcsntol ( wszParam2, cch2, &lExpectedPackets ) );

                            if ( oInitiatorData.Data.oMeteringData.lMaxPackets != lExpectedPackets )
                            {
                                Log( "Trace", "\t\tFrom TestDRM_INIT_ParseMeteringInitiator: number of packets does not match" );
                                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                            }
                        }
                        break;
                    default:
                        ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
                }
            }
        }
        break;
    }

ErrorExit:
    FREEDRMSTRING( dstrParam1 );
    return dr;
}

/***************************************************************************
**
** Function: Drm_Initiator_InitEnum
**
** Synopsis: Function tests Drm_Initiator_InitEnum() API
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
** argv[0]    -- XML Data string. May be set into a special values
**               "nullstring" to imitate empty string (pwszString == NULL, cchString == 0)
**               "nullptr" to pass null ptr instead of DRM_STRING,
**               "zerolen" to pass string with cchString == 0.
**
** argv[1]    -- expected initiator messages body (optional)
**
** Returns:      DRM_RESULT
***************************************************************************/
DRM_RESULT DRM_CALL TestDrm_Initiator_InitEnum(
        long argc,
        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr                  = DRM_SUCCESS;
    DRM_WCHAR  wszXMLData[1024]    = { 0 };
    DRM_WCHAR  wszInitiators[1024] = { 0 };
    DRM_CONST_STRING dstrXMLData   = EMPTY_DRM_STRING;
    const DRM_CONST_STRING *pdstrXMLData = NULL;
    DRM_INITIATORS_ENUM_CONTEXT oEnumContext = {0};

    DRM_STRING dstrTmpStr = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 && argc <= 2 );

    if ( strcmp( argv[0], "nullstring" ) == 0 )
    {
        dstrXMLData.pwszString = NULL;
        dstrXMLData.cchString = 0;

        ChkDR( Drm_Initiator_InitEnum( &dstrXMLData, &oEnumContext ) );
    }
    else if ( strcmp( argv[0], "nullptr" ) == 0 )
    {
        ChkDR( Drm_Initiator_InitEnum( pdstrXMLData, &oEnumContext ) );
    }
    else if ( strcmp( argv[0], "zerolen" ) == 0 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );

        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );
        dstrXMLData.pwszString = wszXMLData;
        dstrXMLData.cchString = 0;

        ChkDR( Drm_Initiator_InitEnum( &dstrXMLData, &oEnumContext ) );
    }
    else
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );

        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );
        dstrXMLData.pwszString = wszXMLData;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszXMLData ), &dstrXMLData.cchString ) );

        ChkDR( Drm_Initiator_InitEnum( &dstrXMLData, &oEnumContext ) );
    }

    if ( argc >= 2 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszInitiators ) );

        TST_UTL_mbstowcs( wszInitiators, argv[1], cch + 1 );

        dstrTmpStr.pwszString = wszInitiators;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszInitiators ), &dstrTmpStr.cchString ) );

        if ( dstrTmpStr.cchString > 0
            && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oEnumContext.dstrCurrent),
                                          (const DRM_CONST_STRING *)&dstrTmpStr ) )
        {
            Log( "Trace", "\t\tFrom TestDrm_Initiator_InitEnum: message body does not match" );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
ErrorExit:
    return dr;
}


/***************************************************************************
**
** Function: TestDRM_Initiator_ParseList
**
** Synopsis: Function tests PlayReady Initiators parser and compares result
**           with predefined values (scenario-based testing)
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
** argv[0]    -- XML Data string. May be set into a special values
**               "nullstring" to imitate empty string (pwszString == NULL, cchString == 0)
**               "nullptr" to pass null ptr instead of DRM_STRING,
**               "zerolen" to pass string with cchString == 0.
**               If it starts with "file:" then the rest of it is a file name,
**               and need to read the data string from the file.
**
** argv[1]    -- index of initiator message in the list (optional, default is 0)
** argv[2]    -- expected type of initiator message (optional)
** argv[3]    -- expected custom data (optional, only if argv[2] is present)
**
** Returns:      DRM_RESULT
***************************************************************************/
DRM_RESULT DRM_CALL TestDrm_Initiator_ParseList(
        long argc,
        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr                  = DRM_SUCCESS;
    DRM_WCHAR  wszXMLData[1024]    = { 0 };

    DRM_CONST_STRING dstrFileName = EMPTY_DRM_STRING;
    DRM_WCHAR   *pbLargeDataString  = NULL;
    DRM_DWORD  dwLargeDataSize = 0;

    DRM_WCHAR  wszCustomData[1024] = { 0 };

    DRM_INITIATORS_ENUM_CONTEXT oEnumContext = {0};
    DRM_INITIATOR_DATA oInitiatorData = EMPTY_DRM_INITIATOR_DATA;

    DRM_INITIATOR_TYPE eExpectedType = DRM_INIT_UNKNOWN;
    DRM_DWORD dwIndex = 0;
    DRM_DWORD dwExpectedIndex    = 0;

    DRM_STRING dstrExpectedCustomData = EMPTY_DRM_STRING;

    DRM_CONST_STRING dstrXMLData   = EMPTY_DRM_STRING;
    const DRM_CONST_STRING *pdstrXMLData = NULL;

    ChkArg( argc >= 1 && argc <= 4 );

    /*
    ** Read XML data input and initialize list
    */
    if ( strcmp( argv[0], "nullstring" ) == 0 )
    {
        ChkDR( Drm_Initiator_InitEnum( &dstrXMLData, &oEnumContext ) );
    }
    else if ( strcmp( argv[0], "nullptr" ) == 0 )
    {
        ChkDR( Drm_Initiator_InitEnum( pdstrXMLData, &oEnumContext ) );
    }
    else if ( strcmp( argv[0], "zerolen" ) == 0 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );

        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );

        dstrXMLData.pwszString = wszXMLData;
        dstrXMLData.cchString = 0;

        ChkDR( Drm_Initiator_InitEnum( &dstrXMLData, &oEnumContext ) );
    }
    else
    {
        if ( strncmp( argv[0], "file:", 5 ) == 0 )
        {
            if ( DRMCRT_strlen(argv[0]) > 5 )  /* read the file name */
            {
                ChkDR( MakeDRMString( &dstrFileName, argv[0] + 5 ) );

                ChkBOOL( LoadTestFileW( NULL, dstrFileName.pwszString , (DRM_BYTE**)&pbLargeDataString, &dwLargeDataSize ), DRM_E_FILEOPEN );

                dstrXMLData.pwszString = (DRM_WCHAR*)pbLargeDataString;
                dstrXMLData.cchString = dwLargeDataSize / SIZEOF(DRM_WCHAR);
            }
            else
            {
                Log( "Trace", "\t\tFrom TestDRM_INIT_ParseList: empty file name" );
                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
        }
        else
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
            ChkArg( cch + 1 <= NO_OF( wszXMLData ) );

            TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );

            dstrXMLData.pwszString = wszXMLData;
            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszXMLData ), &dstrXMLData.cchString ) );
        }

        ChkDR( Drm_Initiator_InitEnum( &dstrXMLData, &oEnumContext ) );

    }

    if ( argc >= 2 )
    {
        dwExpectedIndex = (DRM_DWORD)atol( argv[1] );
    }

    if ( argc >= 3 )
    {
        DRM_DWORD i = (DRM_DWORD) atol( argv[2] );
        switch (i)
        {
            case 1:
                eExpectedType = DRM_INIT_JOINDOMAIN;
                break;
            case 2:
                eExpectedType = DRM_INIT_LEAVEDOMAIN;
                break;
            case 3:
                eExpectedType = DRM_INIT_LICENSEACQUISITION;
                break;
            case 4:
                eExpectedType = DRM_INIT_METERING;
                break;
            default:
                eExpectedType = DRM_INIT_UNKNOWN;
        }
    }

    if ( argc == 4 )
    {
        if ( strcmp( argv[3], "nullstring" ) )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
            ChkArg( cch + 1 <= NO_OF( wszCustomData ) );

            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[3]), &cch ) );
            TST_UTL_mbstowcs( wszCustomData, argv[3], cch + 1 );

            dstrExpectedCustomData.pwszString = wszCustomData;
            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszCustomData ), &dstrExpectedCustomData.cchString ) );
        }
    }

    while ( dr != DRM_E_NOMORE && dwIndex <= dwExpectedIndex )
    {
        dr = Drm_Initiator_EnumNext( &oEnumContext, &oInitiatorData );
        if ( dr == DRM_E_NOMORE )
        {
            break;
        }
        ChkDR(dr);

        if ( dwIndex == dwExpectedIndex ) /* verify this initiator's data */
        {
            if (eExpectedType != DRM_INIT_UNKNOWN && oInitiatorData.eType != eExpectedType )
            {
                Log("Trace", "\t\tInitiator message type %d does not match expected %d", oInitiatorData.eType, eExpectedType);
                ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
            }

            if ( dstrExpectedCustomData.cchString > 0
                && !DRM_UTL_DSTRStringsEqual( (const DRM_CONST_STRING *)&(oInitiatorData.dstrCustomData),
                                              (const DRM_CONST_STRING *)&dstrExpectedCustomData ) )
            {
                Log( "Trace", "\t\tFrom TestDRM_INIT_ParseList: custom data does not match" );
                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
        }
        dwIndex++;
    }

ErrorExit:
    FREEDRMSTRING( dstrFileName );
    SAFE_OEM_FREE(pbLargeDataString);
    return dr;
}

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE(InitParser)

BEGIN_TEST_API_LIST( InitParser )
    API_ENTRY(TestDrm_Initiator_EnumNext)
    API_ENTRY(TestDrm_Initiator_ParseList)
    API_ENTRY(TestDrm_Initiator_InitEnum)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
