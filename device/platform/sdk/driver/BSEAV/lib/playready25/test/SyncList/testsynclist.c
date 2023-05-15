/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <SyncListTestAPI.h>

#include <drmlicense.h>
#include <drmheaderparser.h>
#include <drmlicstore.h>
#include <oempkcrypto.h>
#include <drmhds.h>
#include <drmdla.h>
#include <drmsyncstore.h>
#include <drmwmdrm.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

typedef struct _tagOEM_GENERIC_CONTENT_HEADER
{
    DRM_DWORD  dwSize;
    DRM_DWORD  iChunkSize;
    DRM_DWORD  cbDrmHdr;
} OEM_GENERIC_CONTENT_HEADER;

/*global variables*/

static DRM_CONST_STRING *g_pwszDeviceStoreName = NULL;
static DRM_APP_CONTEXT *g_pManagerContext = NULL;

static DRM_BOOL g_fReinitContextBeforeProcess = FALSE;

#define RESPONSECONTEXTSIZE 50000


/*to do items
    1. add test codes for API at drmsynclist.h
*/

DRM_RESULT DRM_CALL TestSyncListReset(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;

    FREE_SHARED_APP_CONTEXT( g_pManagerContext, g_pwszDeviceStoreName );

    if (1==atoi(argv[0])){
        RemoveDRMFile(RMFILE_STORE);
    }
    INIT_SHARED_APP_CONTEXT( g_pManagerContext, g_pwszDeviceStoreName );

ErrorExit:
    return dr;
}
/*
DRM_RESULT DRM_CALL DRM_SNC_GenerateSyncChallenge (
    IN     DRM_DST                     *f_pDatastore,
    IN     DRM_STACK_ALLOCATOR_CONTEXT *f_pcontextStack,
    IN     DRM_DWORD                    f_cMaxCount,   smallest number is 1
    IN     DRM_DWORD                    f_cMaxHours,   smallest number is 1
    IN     DRM_DWORD                    f_iKIDStart,   zero based index
    IN     DRM_DWORD                    f_cKIDs,
       OUT DRM_DWORD                   *f_piKIDNext,
       OUT DRM_DWORD                   *f_pcKIDs,
       OUT DRM_BYTE                    *f_pbChallenge,
    IN OUT DRM_DWORD                   *f_pcbChallenge);
*/
DRM_RESULT DRM_CALL TestGenerateSyncListChallenge(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD cCountMin=0,cHoursMin=0;
    int iNULLContext=0;
    DRM_DWORD cchChallenge= 0;
    DRM_DWORD iInKIDs       = 0;
    DRM_DWORD iOutKIDs       = 0;
    DRM_DWORD iKIDStart    = 0;
    DRM_DWORD iKIDNext    = 0;
    DRM_BYTE *pszChallenge = NULL;
    DRM_WCHAR wszConvertedKID[64]     = {0};
    DRM_CONST_STRING wszKID1 = {0};
    DRM_CONST_STRING wszKID2 = {0};
    DRM_CONST_STRING wszKID11 = {0};
    DRM_CONST_STRING wszKID22 = {0};
    DRM_BYTE *pbKID1=NULL, *pbKID2=NULL;
    DRM_DWORD cbKID1=0, cbKID2=0;

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        int iOption=0;

        ChkArg( argc > 7 );

        if(argv[0]==NULL){
            iNULLContext=1;
        }

        if(-1==atol(argv[1])){
            cCountMin=DRM_SYNC_IGNORE_THRESHOLD_PARAMETER;
        }
        else{
            cCountMin=(DRM_DWORD)atol(argv[1]);
        }
        if(-1==atol(argv[2])){
            cHoursMin=DRM_SYNC_IGNORE_THRESHOLD_PARAMETER;
        }
        else{
            cHoursMin=(DRM_DWORD)atol(argv[2]);
        }

        iKIDStart=(DRM_DWORD)atol(argv[6]);
        iInKIDs=(DRM_DWORD)atol(argv[7]);


        dr = Drm_Sync_GenerateChallenge(((iNULLContext > 1) ? NULL : g_pManagerContext),
                                           cCountMin,
                                           cHoursMin,
                                           iKIDStart,
                                           iInKIDs,
                                           &iKIDNext,
                                           &iOutKIDs,
                                           NULL,
                                          &cchChallenge);

        if(dr == DRM_E_BUFFERTOOSMALL){

        ChkMem (pszChallenge = (DRM_BYTE*)Oem_MemAlloc(cchChallenge));

            ChkDR  (Drm_Sync_GenerateChallenge(g_pManagerContext,
                                                  cCountMin,
                                                  cHoursMin,
                                                  iKIDStart,
                                                  iInKIDs,
                                                  &iKIDNext,
                                                  &iOutKIDs,
                                        (DRM_BYTE*)pszChallenge,
                                                 &cchChallenge));


        } else {
            ChkDR(dr);
        }

        iOption=(int)atoi(argv[5]);
        /*iOption=1 contains the KID argv[3];
          iOption=2 does not contain check argv[3];
          iOption=4 contain argv[4];
          iOption=8 contain argv[4];
          */

        /*verify the list argv[3] and argv[4]*/
        if (argv[3] && pszChallenge){
            /*challenge contains the kid*/
            ChkDR( MakeDRMString(&wszKID1, argv[3]) );
            ChkDR(tDRM_B64_Decode(&wszKID1, &cbKID1, &pbKID1)); /* This is a 7 byte key */
            ChkDR(tDRM_B64_Encode(pbKID1, cbKID1, &wszKID11));
            memset(wszConvertedKID, 0, sizeof(wszConvertedKID));
            MEMCPY(wszConvertedKID,wszKID11.pwszString, min( SIZEOF( DRM_WCHAR ) *  cbKID1, SIZEOF( wszConvertedKID ) ));
            if(1&iOption){
                DRM_DWORD cch1 = 0;
                DRM_DWORD cch2 = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( (const DRM_WCHAR*)pszChallenge ), &cch1 ) );
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszConvertedKID ), &cch2 ) );
                if (!DRMCRT_wcsnstr((const DRM_WCHAR*)pszChallenge, cch1, wszConvertedKID, cch2 ) ){
                    ChkDR(DRM_E_FAIL);
                }
            }
            else if(2&iOption){
                DRM_DWORD cch1 = 0;
                DRM_DWORD cch2 = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( (const DRM_WCHAR*)pszChallenge ), &cch1 ) );
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszConvertedKID ), &cch2 ) );
                if (DRMCRT_wcsnstr((const DRM_WCHAR*)pszChallenge, cch1, wszConvertedKID, cch2 ) ){
                    ChkDR(DRM_E_FAIL);
                }
            }
        }

        if (argv[4] && pszChallenge){
            /*challenge contains the kid*/
            ChkDR( MakeDRMString(&wszKID2, argv[4]) );
            ChkDR(tDRM_B64_Decode(&wszKID2, &cbKID2, &pbKID2)); /* This is a 7 byte key */
            ChkDR(tDRM_B64_Encode(pbKID2, cbKID2, &wszKID22));
            memset (wszConvertedKID, 0, SIZEOF( wszConvertedKID ) );
            MEMCPY( wszConvertedKID,wszKID22.pwszString, min( SIZEOF( DRM_WCHAR ) *  cbKID2, SIZEOF( wszConvertedKID ) ) );
            if(4&iOption){
                DRM_DWORD cch1 = 0;
                DRM_DWORD cch2 = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( (const DRM_WCHAR*)pszChallenge ), &cch1 ) );
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszConvertedKID ), &cch2 ) );
                if (!DRMCRT_wcsnstr((const DRM_WCHAR*)pszChallenge, cch1, wszConvertedKID, cch2 ) ){
                    ChkDR(DRM_E_FAIL);
                }
            }
            else if(8&iOption){
                DRM_DWORD cch1 = 0;
                DRM_DWORD cch2 = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( (const DRM_WCHAR*)pszChallenge ), &cch1 ) );
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszConvertedKID ), &cch2 ) );
                if (DRMCRT_wcsnstr((const DRM_WCHAR*)pszChallenge, cch1, wszConvertedKID, cch2 ) ){
                    ChkDR(DRM_E_FAIL);
                }
            }
        }

        if(argc > 8 && argv[8] != NULL )
        {
            ChkBOOL( iKIDNext==(DRM_DWORD)atol(argv[8]), DRM_E_FAIL);
        }

        if(argc > 9 && argv[9] != NULL )
        {
            ChkBOOL( iOutKIDs==(DRM_DWORD)atol(argv[9]), DRM_E_FAIL);
        }
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    FREEDRMSTRING( wszKID1 );
    FREEDRMSTRING( wszKID2 );
    FREEDRMSTRING( wszKID11 );
    FREEDRMSTRING( wszKID22 );
    SAFE_OEM_FREE(pbKID1);
    SAFE_OEM_FREE(pbKID2);
    SAFE_OEM_FREE(pszChallenge);
    return dr;
}

/*****************************************************************************
** Function:    TestSetHeader
**
** Synopsis:    Test wrapper function for Drm_Content_SetProperty when setting
**              V2 headers, V2.4 headers, or V4 headers
**
** Arguments:   argv[0]    : DRMManager context status: NULL or Normal
**              argv[1]    : Header file name or NULL
**              argv[2]    : Optional header file type: "UTF-8" or "UTF-16" or "BINARY"
**              argv[3]    : Optional length for the header in characters
**              argv[4]    : Optional header type: "V2", "V2.4", "V4", "PLAYREADY", or "AUTO" (default)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
#define FILE_TYPE_UNDEFINED 0
#define FILE_TYPE_UTF8      1
#define FILE_TYPE_UTF16     2
#define FILE_TYPE_BINARY    3

DRM_RESULT DRM_CALL TestSetHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr           = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD                cbHeader     = 0;    /* Main buffer counter */
    DRM_BYTE                 *pbHeader    = NULL; /* Main byte buffer */
    DRM_CONST_STRING         dstrHeader   = EMPTY_DRM_STRING; /* Buffer for UTF-8 -> UTF-16 conversion */
    DRM_CONTENT_SET_PROPERTY eProperty    = DRM_CSP_HEADER_NOT_SET;
    eDRM_HEADER_VERSION      eVersion     = DRM_HEADER_VERSION_UNKNOWN;
    DRM_WORD                 wFileType    = FILE_TYPE_UTF8;

    ChkArg( argc >= 2 );

    if ( argv[1] != NULL )
    {
        /* Check if file type is specified */
        if ( ( argc >= 3 ) && ( argv[2] != NULL ) )
        {
            if ( ( DRMCRT_strlen( argv[2] ) >= 6 ) &&
                 ( DRMCRT_strncmp( argv[2], "UTF-16", 6) == 0 ) )
            {
                wFileType = FILE_TYPE_UTF16;
            }
            if ( ( DRMCRT_strlen( argv[2] ) >= 6 ) &&
                 ( DRMCRT_strncmp( argv[2], "BINARY", 6) == 0 ) )
            {
                wFileType = FILE_TYPE_BINARY;
            }
        }

        ChkArg(LoadTestFile("", argv[1], &pbHeader, &cbHeader));


        if ( wFileType == FILE_TYPE_UTF16 )
        {
            dstrHeader.pwszString = (const DRM_WCHAR*)pbHeader;
            dstrHeader.cchString = cbHeader;
        }
        else if ( wFileType == FILE_TYPE_UTF8 )
        {
            ChkDR( MakeDRMString(&dstrHeader, (DRM_CHAR*)pbHeader) );
        }

        /*
        ** Validation if header version should be here because
        ** below is modification of header size
        ** Validation will not work after header size modification
        */
        if ( wFileType != FILE_TYPE_BINARY )
        {
            ChkDR( DRM_HDR_GetHeaderVersion( &dstrHeader, &eVersion ) );
        }
    }

    if ( ( argc > 3 ) && ( argv[3] != NULL ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[3]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[3], cch, 10, &cbHeader ) );
        ChkArg(cbHeader <= dstrHeader.cchString);
        dstrHeader.cchString = cbHeader;
    }

    if ( wFileType == FILE_TYPE_UTF16 )
    {
        Log("", "Header:%S", (DRM_WCHAR*) pbHeader);
    }
    else if ( wFileType == FILE_TYPE_UTF8 )
    {
        Log("", "Header:%s", (DRM_CHAR*) pbHeader);
    }
    else if ( wFileType == FILE_TYPE_BINARY )
    {
        Log("", "Header is binary" );
    }

    /* Check if the header type to use is specified */
    eProperty = DRM_CSP_AUTODETECT_HEADER;
    if ( ( argc > 4 ) && ( argv[4] != NULL ) )
    {
        if ( ( DRMCRT_strlen( argv[4] ) >= 4 ) &&
             ( DRMCRT_strncmp( argv[4], "V2.4", 4) == 0 ) )
        {
            eProperty = DRM_CSP_V2_4_HEADER;
            Log("", "Specified header type is V2.4" );
        }
        else if ( ( DRMCRT_strlen( argv[4] ) >= 2 ) &&
                  ( DRMCRT_strncmp( argv[4], "V2", 2) == 0 ) )
        {
            eProperty = DRM_CSP_V2_HEADER;
            Log("", "Specified header type is V2" );
        }
        else if ( ( DRMCRT_strlen( argv[4] ) >= 4 ) &&
                  ( DRMCRT_strncmp( argv[4], "V4.1", 4) == 0 ) )
        {
            eProperty = DRM_CSP_V4_1_HEADER;
            Log("", "Specified header type is V4.1" );
        }
        else if ( ( DRMCRT_strlen( argv[4] ) >= 2 ) &&
                  ( DRMCRT_strncmp( argv[4], "V4", 2) == 0 ) )
        {
            eProperty = DRM_CSP_V4_HEADER;
            Log("", "Specified header type is V4" );
        }
        else if ( ( DRMCRT_strlen( argv[4] ) >= 4 ) &&
                  ( DRMCRT_strncmp( argv[4], "PLAYREADY", 2) == 0 ) )
        {
            eProperty = DRM_CSP_AUTODETECT_HEADER;
            Log("", "Specified use of autodetection for header type " );
        }
        else if ( ( DRMCRT_strlen( argv[4] ) >= 4 ) &&
                  ( DRMCRT_strncmp( argv[4], "AUTO", 2) == 0 ) )
        {
            eProperty = DRM_CSP_AUTODETECT_HEADER;
            Log("", "Specified use of autodetection for header type " );
        }
        else
        {
            Log("", "Unknown header type specified: %s", argv[4]);
        }
    }

    if ( wFileType != FILE_TYPE_BINARY )
    {
        ChkDR( Drm_Content_SetProperty( argv[0] ? g_pManagerContext : NULL,
                                        eProperty,
                                        PB_DSTR( &dstrHeader ),
                                        CB_DSTR( &dstrHeader ) ) );
    }
    else
    {
        ChkDR( Drm_Content_SetProperty( argv[0] ? g_pManagerContext : NULL,
                                        eProperty,
                                        pbHeader,
                                        cbHeader ) );
    }

ErrorExit:
    SAFE_OEM_FREE(pbHeader);
    if ( wFileType == FILE_TYPE_UTF8 )
    {
        FREEDRMSTRING( dstrHeader );
    }
    return dr;
}

DRM_RESULT DRM_CALL TestSetEnhancedData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD cbHeader=0;
    DRM_BYTE *pbHeader=NULL;
    OEM_GENERIC_CONTENT_HEADER oHeader;
    FILE *fp=NULL;
    DRM_CHAR *pszContentFileName = NULL;
    DRM_DWORD cchContentFileName = 0;

    ChkDR( TST_UTL_MakePathA( NULL, argv[0], &pszContentFileName, &cchContentFileName, FALSE ) );

    fp=fopen(pszContentFileName, "rb");
    if(NULL==fp){
        Log("Trace", "Cannot load file: %s", pszContentFileName);
        ChkDR(DRM_E_FAIL);
    }
    if( sizeof( OEM_GENERIC_CONTENT_HEADER ) != fread( &oHeader, sizeof( DRM_BYTE) ,
        sizeof( OEM_GENERIC_CONTENT_HEADER ), fp ) )
    {
        Log("Trace",  "Header couldn't be read");
        ChkDR(DRM_E_FAIL);
    }

    cbHeader=oHeader.cbDrmHdr;
    ChkMem(pbHeader = (DRM_BYTE *)Oem_MemAlloc(cbHeader));
    if(cbHeader!=fread(pbHeader, 1, cbHeader, fp ) )
    {
        Log("Trace",  "Header couldn't be read");
        ChkDR(DRM_E_FAIL);
    }

    ChkDR( Drm_Content_SetProperty( g_pManagerContext,
                                    DRM_CSP_V2_HEADER,
                                    pbHeader,
                                    cbHeader ) );
ErrorExit:

    SAFE_OEM_FREE(pbHeader);

    return dr;
}

static DRM_RESULT tGetSystemTimeString( __inout_ecount(18) char* szSystemTimeString, long loffset)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRMSYSTEMTIME tSysTime;
    DRMFILETIME   mcFileTime;
    DRM_UINT64    u64;
    DRM_WORD      wTemp = 0;

    ChkArg(szSystemTimeString && loffset);

    Oem_Clock_GetSystemTimeAsFileTime(NULL, &mcFileTime);

    /* offset the file time */
    FILETIME_TO_UI64(mcFileTime, u64);
    if(loffset > 0)
        u64 = DRM_UI64Add(u64, DRM_UI64Mul(DRM_UI64(loffset), DRM_UI64(FILETIME_TO_SECONDS)));
    else
        u64 = DRM_UI64Sub(u64, DRM_UI64Mul(DRM_UI64(-loffset), DRM_UI64(FILETIME_TO_SECONDS)));
    UI64_TO_FILETIME(u64, mcFileTime);

    /*convert back to system time from file time */
    ChkArg(Oem_Clock_FileTimeToSystemTime(&mcFileTime, &tSysTime));

    /*e.g. 20090101 10:00:00*/
    wTemp = tSysTime.wYear;
    PUT_CHAR( szSystemTimeString, 3, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 2, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 1, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 0, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );

    wTemp = tSysTime.wMonth;
    PUT_CHAR( szSystemTimeString, 5, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 4, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );

    wTemp = tSysTime.wDay;
    PUT_CHAR( szSystemTimeString, 7, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 6, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );

    PUT_CHAR( szSystemTimeString, 8, ' ' );

    wTemp = tSysTime.wHour;
    PUT_CHAR( szSystemTimeString, 10, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 9, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    PUT_CHAR( szSystemTimeString, 11, ':' );

    wTemp = tSysTime.wMinute;
    PUT_CHAR( szSystemTimeString, 13, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 12, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    PUT_CHAR( szSystemTimeString, 14, ':' );

    wTemp = tSysTime.wSecond;
    PUT_CHAR( szSystemTimeString, 16, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 15, (DRM_BYTE)TST_UTL_ByteValueToASCII( (DRM_BYTE)(wTemp % 10) ) );
    PUT_CHAR( szSystemTimeString, 17, '\0' );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestAddLicenseToStore(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_CHAR szExtraXML[2000]={0};
    DRM_CONST_STRING pwszLicense;
    ELicenseType eLicType = MAKE_SIMPLE_LICENSE;
    _XMBContext *pbResponseContext = NULL;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse=0;
    DRM_LICENSE_RESPONSE oLicenseResponse = { eUnknownProtocol, 0 };

    ChkArg( argc > 6 );

    /*initialize the response xml frame*/
    SAFE_OEM_FREE(pbResponseContext);
    ChkMem(pbResponseContext = (_XMBContext *)Oem_MemAlloc(RESPONSECONTEXTSIZE));
    ChkDR(TestResponseInit(pbResponseContext, RESPONSECONTEXTSIZE));

    /*
    argv[0]: KID
    argv[1]: LID
    argv[2]: option in creating a test license
    argv[3]: extra xml string to add to the license
    argv[4]: offset time
    argv[5]: content sign key
    argv[6]: key seed
    argv[7]: optional license type: 0(simple), 1(leaf), 2(root)
    argv[8]: optional uplink kid
    argv[9]: optional WMRM header, Reinitializes manager context if not present.
    */
    /*add encrypted and encoded license*/
    ChkDR(TestLicGenInit(10000));
    TestLicGenSetKeys("LICENSE_KID",argv[0]);
    TestLicGenSetKeys("LICENSE_LID",argv[1]);
    if( argv[3] != NULL )
    {
        if(strstr(argv[3],"ExpirationDate=#" ))
        {
            char        pSystemTime[20];
            const char *pszXMLEnding  = "Z#:0]]></ACTION></ONSTORE>";
            long        lOffset       = 0;
            DRM_DWORD   cchSystemTime = 0;
            DRM_DWORD   cchXMLEnding  = ( DRM_DWORD ) strlen( pszXMLEnding );
            DRM_DWORD   cchArg        = 0;

            ChkArg(argv[4]);
            lOffset=atol(argv[4])*60*60;
            ChkDR(tGetSystemTimeString(pSystemTime,lOffset));

            cchSystemTime = ( DRM_DWORD ) strlen( pSystemTime );
            cchArg        = ( DRM_DWORD ) strlen( argv[3] );
            ChkBOOL( SIZEOF( szExtraXML ) >= ( cchSystemTime + cchArg + cchXMLEnding + 1 ), DRM_E_FAIL );

            DRM_BYT_CopyBytes( szExtraXML, 0, argv[3], 0, cchArg );
            DRM_BYT_CopyBytes( szExtraXML, cchArg, pSystemTime, 0, cchSystemTime );
            DRM_BYT_CopyBytes( szExtraXML, cchArg + cchSystemTime, pszXMLEnding, 0, cchXMLEnding);
            TRACE((szExtraXML));
        }
        else
        {
            MEMCPY(szExtraXML, argv[3], min( SIZEOF( szExtraXML ), ( DRM_DWORD ) strlen( argv[3] ) + 1 ) );
        }
    }
    TestLicGenSetKeys("CONTENT_OWNER_PUB_KEY",argv[5]);
    TestLicGenSetKeys("LICENSE_SERVER_KEYSEED", argv[6]);

    if (argc > 7)
    {
        eLicType = (ELicenseType)atol(argv[7]);
        if(argc>8)
        {
            ChkDR(TestLicGenSetKeys("LICENSE_UPLINK_KID",argv[8]));
        }
    }

    ChkDR(TestLicGenMakeLicense(
        (DRM_DWORD)atol(argv[2]),
        argv[3]?szExtraXML:NULL,
        NULL,
        eLicType,
        &pwszLicense,
        NULL,
        FALSE));
    ChkDR(TestResponseAddLicense(pbResponseContext,&pwszLicense));
    TestLicGenShutDown();

    /*get the response*/
    ChkDR(TestResponseGetResponse(pbResponseContext,&pbResponse,&cbResponse));

    if ( argc > 9 )
    {
        DRM_WCHAR rgwcWMRM[500] = { 0 };
        DRM_SUBSTRING dasstrWMRM;
        DRM_STRING dstrWMRM;

        dasstrWMRM.m_ich = 0;
        dasstrWMRM.m_cch = ( DRM_DWORD ) strlen(argv[9]);

        dstrWMRM.pwszString = rgwcWMRM;
        dstrWMRM.cchString = 0;

        DRM_UTL_PromoteASCIItoUNICODE( argv[9], &dasstrWMRM, &dstrWMRM );

        ChkDR( Drm_Content_SetProperty( g_pManagerContext,
                                        DRM_CSP_V2_HEADER,
                                        (DRM_BYTE*)dstrWMRM.pwszString,
                                        dstrWMRM.cchString * SIZEOF(DRM_WCHAR) ) );
    }

    if ( g_fReinitContextBeforeProcess )
    {
        ChkDR( Drm_Reinitialize( g_pManagerContext ) );
    }

    ChkDR( Drm_LicenseAcq_ProcessResponse( g_pManagerContext,
                                           DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                           NULL,
                                           NULL,
                                           pbResponse,
                                           cbResponse,
                                           &oLicenseResponse ) );

ErrorExit:
    SAFE_OEM_FREE(pbResponse);
    SAFE_OEM_FREE(pbResponseContext);
    return dr;
}


DRM_RESULT DRM_CALL TestPlayback(long argc, __in_ecount(argc) char **argv)
{
    DRM_DWORD numRights=0;
    DRM_CONST_STRING *pwszRights[1];
    DRM_DECRYPT_CONTEXT *pDecryptContext = NULL;
    DRM_RESULT dr=DRM_SUCCESS;

    /*set rights*/
    ChkMem(pwszRights[numRights] = (DRM_CONST_STRING*)Oem_MemAlloc(sizeof(DRM_CONST_STRING)));
    ChkDR( MakeDRMString(pwszRights[numRights], argv[0]) );
    numRights++;
    ChkMem(pDecryptContext = (DRM_DECRYPT_CONTEXT*)Oem_MemAlloc(sizeof(DRM_DECRYPT_CONTEXT)));

    OEM_SECURE_ZERO_MEMORY( pDecryptContext, sizeof(DRM_DECRYPT_CONTEXT) );

    /*bind the contentwith license*/
    ChkDR(Drm_Reader_Bind(g_pManagerContext,
        (const DRM_CONST_STRING **)pwszRights,
        numRights,
        NULL,
        NULL,
        pDecryptContext));

    ChkDR(Drm_Reader_Commit( g_pManagerContext, NULL, NULL ));
ErrorExit:
    while (numRights > 0) {
        numRights--;
        FREEDRMSTRINGPOINTER( pwszRights[numRights] ) ;
        SAFE_OEM_FREE(pwszRights[numRights]);
    }

    if( pDecryptContext != NULL )
    {
        Drm_Reader_Close( pDecryptContext );
    }

    SAFE_OEM_FREE(pDecryptContext);
    return dr;
}

/*argv[0]: offset in seconds, positive to go forward, negative to rollback*/
DRM_RESULT DRM_CALL TestSynclistChangeTime(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long lOffset;

    ChkArg(argc == 1 && argv[0]);
    lOffset = atol(argv[0]);
    tChangeSystemTime(lOffset);

ErrorExit:
    return dr;
}

/*
** Copied from DRMManager
*/
static DRM_RESULT _GetLicenseEnum2(const DRM_KID *pKID, DRM_DWORD* pdwCount, DRM_LICSTORE_CONTEXT* pLicStoreContext, DRM_LICSTOREENUM_CONTEXT** ppLicEnumContext)
{
    DRM_RESULT dr=DRM_SUCCESS;

    ChkArg( pKID != NULL );
    ChkArg(pdwCount && ppLicEnumContext);

    ChkMem(*ppLicEnumContext = (DRM_LICSTOREENUM_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_LICSTOREENUM_CONTEXT)));
    ZEROMEM(*ppLicEnumContext, SIZEOF(DRM_LICSTOREENUM_CONTEXT));


    ChkDR(DRM_LST_InitEnum(pLicStoreContext,
                              pKID,
                              FALSE, //prioritize
                              *ppLicEnumContext));

    ChkDR(DRM_LST_LicCount(*ppLicEnumContext,
                             pdwCount));
ErrorExit:

    return dr;
}

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif
/*
**    argv[0]: KID
**    argv[1]: filename to dump license
**    argv[2]: optional, expected number of licenses in the Enum
*/
DRM_RESULT DRM_CALL TestEnumLicense2(long argc, __in_ecount(argc) char **argv)
{
    DRM_KID                        kid;
    DRM_SUBSTRING                  ssKID                = EMPTY_DRM_SUBSTRING;
    DRM_DWORD                      dwSize               = DRM_ID_SIZE;
    DRM_LICSTOREENUM_CONTEXT*      pLicStoreEnumContext = NULL;
    DRM_DWORD                      dwNOLicenses         = 0;
    DRM_DST                        oDst                 = { eDRM_DST_NONE, { 0 }, NULL };
    DRM_DST_CONTEXT                oDstContext          = { 0 };
    DRM_LICSTORE_CONTEXT           oLicStore;
    DRM_RESULT                     dr                   = DRM_SUCCESS;

    ChkArg( argc > 1 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL || ( argc > 2 && argv[2] != NULL ) );

    ssKID.m_ich = 0;
    ssKID.m_cch = ( DRM_DWORD ) strlen(argv[0]);

    //convert from LPSTR to KID
    ChkDR(DRM_B64_DecodeA(argv[0],
                              &ssKID,
                              &dwSize,
                              kid.rgb,
                              0));

    ChkDR( DRM_DST_Init( eDRM_DST_HDS, &oDstContext, SIZEOF( oDstContext ), &oDst ) );

    ChkDR( DRM_DST_OpenStore( NULL,
                              ( DRM_BYTE * )g_pwszDeviceStoreName->pwszString,
                              g_pwszDeviceStoreName->cchString * SIZEOF( DRM_WCHAR ),
                              0,
                              &oDst ) );
    ChkDR(DRM_LST_Open(&oLicStore, &oDst,eDRM_LICENSE_STORE_XML));

    ChkDR(_GetLicenseEnum2(&kid, &dwNOLicenses, &oLicStore, &pLicStoreEnumContext));

    if ( argc > 2 && argv[2] != NULL ) //check the number of licenses expected
    {
        if (atoi(argv[2]) != (DRM_LONG) dwNOLicenses)
        {
            Log("Trace", "Number of License (%d)  with the KID is not match expected", dwNOLicenses);
            ChkDR(DRM_E_FAIL);
        }
    }

    if (0 != strcmp(argv[1], "") )
    {
        //dump the licenses to the specified file
    }

ErrorExit:

    DRM_LST_Close(&oLicStore);
    DRM_DST_CloseStore(&oDst);
    DRM_DST_Uninit(&oDst);

    SAFE_OEM_FREE(pLicStoreEnumContext);

    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif

/*
**  Arguments:
**              argv[0] -- KID to get
**              argv[1] -- string expected to be in the returned content header.
**              argv[2] -- opt, if NULL, use a NULL Manager Context otherwise normal
**              argv[3] -- opt, if NULL, use a NULL KID pointer
**              argv[4] -- opt, if NULL, use a NULL Content Header Pointer
**              argv[5] -- opt, if NULL, use a NULL pointer for the context header size pointer
**                          if not null then it is a parsed as an int and added to cchContentHeader
**                          to adjust the size parameter
*/
DRM_RESULT DRM_CALL TestGetContentHeaderByKID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dstrKID = {0};
    DRM_DWORD   cbKID =  SIZEOF( DRM_KID );
    DRM_WCHAR  *pwszContentHeader = NULL;
    DRM_DWORD   cchContentHeader = 0 ;
    DRM_KID     kid = {0};
    DRM_CHAR    rgszContentHeader[DRM_MAX_HEADERDATASIZE];

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg( argc >= 2 );
        ChkArg( argv[0] != NULL );
        ChkArg( argv[1] != NULL );

        dstrKID.m_cch = ( DRM_DWORD ) strlen(argv[0]);
        dstrKID.m_ich = 0;
        ChkDR( DRM_B64_DecodeA( argv[0],
                            &dstrKID,
                            &cbKID,
                            kid.rgb,
                            0 ) );

        dr = Drm_LicenseAcq_GetContentHeader( g_pManagerContext, &kid, pwszContentHeader, &cchContentHeader);

        if ( dr != DRM_E_BUFFERTOOSMALL )
        {
            ChkDR( dr );
        }

        /*
        ** If content header was not found still need to allocate something (null ptr is a different test case)
        */
        pwszContentHeader = (DRM_WCHAR*)Oem_MemAlloc( max(1, cchContentHeader * SIZEOF(DRM_WCHAR)) );

        if ( argc > 5 && argv[5] != NULL )
        {
            cchContentHeader += atol( argv[5] );
        }

        ChkDR( Drm_LicenseAcq_GetContentHeader( ( argc > 2 && argv[2] == NULL ) ? NULL :  g_pManagerContext,
                                              ( argc > 3 && argv[3] == NULL ) ? NULL : &kid,
                                              ( argc > 4 && argv[4] == NULL ) ? NULL :  pwszContentHeader,
                                              ( argc > 5 && argv[5] == NULL ) ? NULL : &cchContentHeader ) );

        ChkBOOL( cchContentHeader == ( DRM_DWORD ) strlen( argv[1] ), DRM_E_TEST_UNEXPECTED_OUTPUT );

        if ( argc <= 4 || argv[4] != NULL )
        {
            DRM_UTL_DemoteUNICODEtoASCII( pwszContentHeader, rgszContentHeader, min( cchContentHeader, DRM_MAX_HEADERDATASIZE ) );
            ChkBOOL( strncmp( rgszContentHeader, argv[1], cchContentHeader ) == 0 , DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    SAFE_OEM_FREE(pwszContentHeader);
    return dr;
}

/*
**  Arguments:
**              argv[0] --  KID to grab, can be NULL
**              argv[1] --  opt, uses NULL DRM_LICENSE_STATE_DATA    value if NULL, else checks the category value
**              argv[2] --  opt, if NULL use a NULL pointer for the header data string, if not null, looks for the value in the header (strstr)
**                              if this is an empty string then the test assumes that the header should not be in the sync store and fails if it is.
**              argv[3] --  opt, use valid Sync Context pointer? NULL -> null pointer; TRUE or Missing -> not null
**              argv[4] --  opt, cchHeader size to use.
*/
DRM_RESULT DRM_CALL TestGetSyncStoreEntry(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                   dr           = DRM_SUCCESS;
    DRM_BOOL                     fValidSync   = TRUE;
    DRM_BOOL                     fValidString = TRUE;
    DRM_DWORD                    cchHeader    = DRM_MAX_HEADERDATASIZE / SIZEOF( DRM_WCHAR );
    DRM_KID                      kid          = {0};
    DRM_DWORD                    cbKid        = SIZEOF( kid.rgb );
    DRM_STRING                   dstrTest     = EMPTY_DRM_STRING;
    DRM_SUBSTRING                dsstrTemp    = EMPTY_DRM_SUBSTRING;
    DRM_APP_CONTEXT_INTERNAL    *pManager     = ( DRM_APP_CONTEXT_INTERNAL * ) g_pManagerContext;
    DRM_LICENSE_STATE_DATA       lsd          = {0};

    if( DRM_WMDRMDLA_IsWMDRMDLASupported()
     && DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg( argc >= 1 );

        if ( argv[0] != NULL )
        {
            dsstrTemp.m_ich = 0;
            dsstrTemp.m_cch = ( DRM_DWORD ) strlen( argv[0] );
            ChkDR( DRM_B64_DecodeA( argv[0], &dsstrTemp, &cbKid, kid.rgb, 0 ) );
        }

        if ( argc >= 3 )
        {
            if ( argv[2] == NULL )
            {
                fValidString = FALSE;
                cchHeader = 0;
            }
            else
            {
                dstrTest.cchString = ( DRM_DWORD ) strlen( argv[2] );
                ChkMem( dstrTest.pwszString = ( DRM_WCHAR * ) Oem_MemAlloc( ( dstrTest.cchString + 1 ) * SIZEOF( DRM_WCHAR ) ) );
                dsstrTemp.m_cch = dstrTest.cchString;
                dsstrTemp.m_ich = 0;
                DRM_UTL_PromoteASCIItoUNICODE( argv[2] , &dsstrTemp, &dstrTest );
                dstrTest.pwszString[dstrTest.cchString] = ONE_WCHAR('\0','\0');
            }
        }
        if ( argc >= 4 )
        {
            if ( argv[3] == NULL )
            {
                fValidSync = FALSE;
            }
        }

        if ( argc >= 5 && argv[4] != NULL )
        {
            ChkArg(  strtoul( argv[4], NULL, 0 ) <= DRM_MAX_HEADERDATASIZE / SIZEOF( DRM_WCHAR )  );

            cchHeader = strtoul( argv[4], NULL, 0 );
        }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
        ChkDR( DRM_SNC_GetSyncStoreEntry( !fValidSync ? NULL : &pManager->contextSync,
                                          argv[0] == NULL ? NULL : &kid,
                                          ( argc >= 2 && argv[1] == NULL ) ? NULL : &lsd,
                                          !fValidString ? NULL : ( DRM_WCHAR * ) pManager->pbDRMHeaderData,
                                         &cchHeader ) );
PREFAST_POP

        pManager->cbHeaderData = cchHeader * SIZEOF( DRM_DWORD );

        if ( argc >= 2 && argv[1] != NULL && argv[1][0] != '\0' )
        {
            if( (DRM_LONG)lsd.dwCategory != atoi(argv[1]) )
            {
                Log( "Error","dwCategory: %d", lsd.dwCategory );
                ChkDR(DRM_E_TEST_SYNC_LSD_INCORRECT);
            }
        }

        if ( fValidString && argc >= 3)
        {
            if ( *argv[2] != '\0' )
            {
                DRM_DWORD cch1 = 0;
                DRM_DWORD cch2 = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( (DRM_WCHAR*)pManager->pbDRMHeaderData ), &cch1 ) );
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( dstrTest.pwszString ), &cch2 ) );
                ChkBOOL( DRMCRT_wcsnstr( (DRM_WCHAR*)pManager->pbDRMHeaderData, cch1, dstrTest.pwszString, cch2 ) != 0, DRM_E_FAIL );
            }
            else
            {
                ChkBOOL( cchHeader == 0, DRM_E_TEST_DLA_NO_CONTENT_HEADER );
            }
        }

        ChkBOOL( fValidString || cchHeader != 0, DRM_E_TEST_DLA_NO_CONTENT_HEADER );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    if( fValidString && argc >= 3 )
    {
        FREEDRMSTRING( dstrTest );
    }

    return dr;
}

/*
** Synopsis:    Runs test cases for DRM_SNC_UpdateKID
**
** Arguments:
**              argv[0] -- KID, can be NULL, NULL if Missing (shouldn't be missing though!)
**              argv[1] -- opt, Content Header to update, Defaults to NULL if Missing
**              argv[2] -- opt, if NULL, use a NULL Sync Context Pointer
**              argv[3] -- opt, if NULL, use a NULL View_RIGHTS_CONTEXT pointer
**              argv[4] -- opt, Specifies the Category State. Values are "FORCE", "AGGREGATE", and "NO_LICENSE"
**
*/
DRM_RESULT DRM_CALL TestSyncUpdateKID( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT    dr         = DRM_SUCCESS;
    DRM_KID       kid        = {0};
    DRM_DWORD     cbKid      = SIZEOF( kid.rgb );
    DRM_STRING    dstrHeader = EMPTY_DRM_STRING;
    DRM_SUBSTRING dsstrTemp  = EMPTY_DRM_SUBSTRING;

    DRM_SYNC_UPDATE_CATEGORY  eSyncCategory = DRM_SYNC_UPDATE_AGGREGATE;
    DRM_APP_CONTEXT_INTERNAL *pManager      = ( DRM_APP_CONTEXT_INTERNAL * ) g_pManagerContext;

    if( DRM_WMDRMDLA_IsWMDRMDLASupported()
     && DRM_SNC_IsLicenseSyncSupported() )
    {
        if ( argc >= 1 && argv[0] != NULL )
        {
            dsstrTemp.m_ich = 0;
            dsstrTemp.m_cch = ( DRM_DWORD ) strlen( argv[0] );
            ChkDR( DRM_B64_DecodeA( argv[0], &dsstrTemp, &cbKid, kid.rgb, 0 ) );
        }
        if( argc >= 2 && argv[1] != NULL )
        {
            dsstrTemp.m_ich = 0;
            dsstrTemp.m_cch = ( DRM_DWORD ) strlen( argv[1] );
            dstrHeader.cchString = ( DRM_DWORD ) strlen( argv[1] );
            ChkMem( dstrHeader.pwszString = ( DRM_WCHAR * ) Oem_MemAlloc( ( dstrHeader.cchString + 1 ) * SIZEOF( DRM_WCHAR ) ) );
            DRM_UTL_PromoteASCIItoUNICODE( argv[1] , &dsstrTemp, &dstrHeader );
            dstrHeader.pwszString[dstrHeader.cchString] = ONE_WCHAR('\0','\0');
        }

        if( argc >= 5 && argv[4] != NULL )
        {
            if( 0 == DRMCRT_strncmp( argv[4], "AGGREGATE", 9 ) )
            {
                eSyncCategory = DRM_SYNC_UPDATE_AGGREGATE;
            }
            else if( 0 == DRMCRT_strncmp( argv[4], "FORCE", 5 ) )
            {
                eSyncCategory = DRM_SYNC_UPDATE_FORCE_SYNC;
            }
            else if( 0 == DRMCRT_strncmp( argv[4], "NO_LICENSE", 10 ) )
            {
                eSyncCategory = DRM_SYNC_UPDATE_NO_LICENSE;
            }
            else
            {
                ChkArg( !"Update State Not Supported" );
            }
        }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
        ChkDR( DRM_SNC_UpdateKID( ( argc >= 3 && argv[2] == NULL ) ? NULL : &pManager->contextSync,
                                  ( argc >= 4 && argv[3] == NULL ) ? NULL : &pManager->oViewRightsContext,
                                  ( argc >= 1 && argv[0] != NULL ) ? &kid : NULL,
                                  ( argc >= 2 && argv[1] != NULL ) ? ( DRM_CONST_STRING * ) &dstrHeader : NULL,
                                  eSyncCategory ) );
PREFAST_POP
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    FREEDRMSTRING( dstrHeader );
    return dr;
}


/*
** Sets g_fReinitContextBeforeProcess to the value in the argument.
**
**  argv[0] -- TRUE or FALSE
*/
DRM_RESULT DRM_CALL TestSetManagerReinitialize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc >= 1);
    ChkArg( argv[0] != NULL);

    ChkDR( StringToBool( argv[0], &g_fReinitContextBeforeProcess ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(SyncList)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr;
    CLIENTID clientID;

    ChkDR(Oem_Clock_SetResetState(NULL, FALSE));

    GET_SHARED_APP_CONTEXT( g_pManagerContext );
    GET_SHARED_DEVICE_STORE( g_pwszDeviceStoreName );

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR(TestGetClientID(&clientID));
        ChkDR(TestLicResponseSetClientID(&clientID));
    }

ErrorExit:
    g_fReinitContextBeforeProcess = FALSE;
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(SyncList)(long lTCID, const char *strTCName)
{
    tResetSystemTimeOffset(); /* Reverse any time changes in the test case.*/

    RemoveDRMFile(RMFILE_STORE);

    return DRM_SUCCESS;
}


BEGIN_TEST_API_LIST(SyncList)
    API_ENTRY(TestSyncListReset)
    API_ENTRY(TestGenerateSyncListChallenge)
    API_ENTRY(TestAddLicenseToStore)
    API_ENTRY(TestSetEnhancedData)
    API_ENTRY(TestSetHeader)
    API_ENTRY(TestPlayback)
    API_ENTRY(TestSynclistChangeTime)
    API_ENTRY(TestEnumLicense2)
    API_ENTRY(TestGetContentHeaderByKID)
    API_ENTRY(TestGetSyncStoreEntry)
    API_ENTRY(TestSyncUpdateKID)
    API_ENTRY(TestSetManagerReinitialize)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

