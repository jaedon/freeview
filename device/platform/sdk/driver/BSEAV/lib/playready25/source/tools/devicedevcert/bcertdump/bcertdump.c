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
#include <drmcmdlnpars.h>
#include <oempkcrypto.h>
#include <drmlicense.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmtoolsmacros.h>
#include <drmtoolsconstants.h>
#include <drmtoolsutils.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <bcertdump.h>
#include <drmconstants.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

static DRM_CRYPTO_CONTEXT g_CryptoCtx                     = {0};
/* "b" */
const DRM_WCHAR           g_rgwchBINCERT_OPTION_STRING   []= { ONE_WCHAR('b', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_CONST_STRING    g_dstrBINCERT_OPTION_STRING      = CREATE_DRM_STRING( g_rgwchBINCERT_OPTION_STRING );
/* "format" */
const DRM_WCHAR           g_rgwchFORMAT_OPTION_STRING    []= { ONE_WCHAR('f', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_CONST_STRING    g_dstrFORMAT_OPTION_STRING       = CREATE_DRM_STRING( g_rgwchFORMAT_OPTION_STRING );
/* "v" */
const DRM_WCHAR           g_rgwchVERBOSE_OPTION_STRING   []= { ONE_WCHAR('v', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_CONST_STRING    g_dstrVERBOSE_OPTION_STRING      = CREATE_DRM_STRING( g_rgwchVERBOSE_OPTION_STRING );
/* "hex" */
const DRM_WCHAR           g_rgwchHEX_FORMAT_STRING      []= { ONE_WCHAR('h', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_CONST_STRING    g_dstrHEX_FORMAT_STRING         = CREATE_DRM_STRING( g_rgwchHEX_FORMAT_STRING );
/* "b64" */
const DRM_WCHAR           g_rgwchB64_FORMAT_STRING      []= { ONE_WCHAR('b', '\0'),  ONE_WCHAR('6', '\0'),  ONE_WCHAR('4', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_CONST_STRING    g_dstrB64_FORMAT_STRING         = CREATE_DRM_STRING( g_rgwchB64_FORMAT_STRING );
const DRM_WCHAR           g_rgwchINPUTFORMAT_OPTION_STRING   []= { ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_CONST_STRING    g_dstrINPUTFORMAT_OPTION_STRING      = CREATE_DRM_STRING( g_rgwchINPUTFORMAT_OPTION_STRING );

/**********************************************************************************************************
** Function:    _printObjectHeader
**
** Synopsis:    Prints out the data in an object header
**
** Arguments:   [f_poHeader]       : pointer to object header structure
**              [f_fVerboseOutput] : if FALSE then print out only basic information, otherwise
**                                   print more data.
**              [f_fOpen]          : flag indicating whether the output is the open XML tag.
**              [f_funcPrint]      : the function that does the actual print.
**
** Returns:     FALSE  - if header object is NULL or object length is zero so print nothing.
**              TRUE   - otherwise
**
**********************************************************************************************************/
static DRM_BOOL _printObjectHeader(
    const DRM_BCERT_OBJECT_HEADER *f_poHeader,
    DRM_BOOL f_fVerboseOutput,
    DRM_BOOL f_fOpen,
    PRINTFUNC f_funcPrint)
{
    if( f_poHeader == NULL
        || f_poHeader->cbLength == 0 )
    {
        return FALSE;
    }

    f_funcPrint("<");

    if ( !f_fOpen )
    {
        f_funcPrint("/");
    }

    switch( f_poHeader->wType )
    {
        case DRM_BCERT_OBJTYPE_BASIC:
            f_funcPrint("Basic_Data");
            break;
        case DRM_BCERT_OBJTYPE_DOMAIN:
            f_funcPrint("Domain_Data");
            break;
        case DRM_BCERT_OBJTYPE_PC:
            f_funcPrint("PC_Data");
            break;
        case DRM_BCERT_OBJTYPE_SILVERLIGHT:
            f_funcPrint("Silverlight_Data");
            break;
        case DRM_BCERT_OBJTYPE_METERING:
            f_funcPrint("Metering_Data");
            break;
        case DRM_BCERT_OBJTYPE_DEVICE:
            f_funcPrint("Device_Data");
            break;
        case DRM_BCERT_OBJTYPE_FEATURE:
            f_funcPrint("Features");
            break;
        case DRM_BCERT_OBJTYPE_KEY:
            f_funcPrint("Key_Data");
            break;
        case DRM_BCERT_OBJTYPE_MANUFACTURER:
            f_funcPrint("Manufacturer");
            break;
        case DRM_BCERT_OBJTYPE_SIGNATURE:
            f_funcPrint("Signature");
            break;
        case DRM_BCERT_OBJTYPE_EXTDATASIGNKEY:
            f_funcPrint("ExtData_Sign_Key");
            break;
        case DRM_BCERT_OBJTYPE_EXTDATACONTAINER:
            f_funcPrint("ExtData_Container");
            break;
        case DRM_BCERT_OBJTYPE_EXTDATA_HWID:
            f_funcPrint("ExtData_Record_HWID");
            break;
        case DRM_BCERT_OBJTYPE_EXTDATASIGNATURE:
            f_funcPrint("ExtData_Signature");
            break;
        case DRM_BCERT_OBJTYPE_SECURITY_VERSION:
            f_funcPrint("Robustness_Data");
            break;
        case DRM_BCERT_OBJTYPE_SECURITY_VERSION_2:
            f_funcPrint("Robustness_Data_2");
            break;
        default:
            f_funcPrint("Unknown");
    }

    if ( f_fVerboseOutput && f_fOpen)
    {
        f_funcPrint(" flag=\"");
        f_funcPrint( ( (f_poHeader->wFlags & DRM_BCERT_OBJFLAG_MUST_UNDERSTAND) != 0 ) ? "required" : "optional" );
        f_funcPrint( ( (f_poHeader->wFlags & DRM_BCERT_OBJFLAG_CONTAINER_OBJ) != 0 ) ? ",container" : "" );
        f_funcPrint("\"");

        f_funcPrint( " length=\"%d\"", f_poHeader->cbLength );
    }

    f_funcPrint(">\n");

    return TRUE;
}


/**********************************************************************************************************
** Function:    _printKeyUsageEntry
**
** Synopsis:    Prints out the name of key usage
**
** Arguments:   [f_dwKeyUsage] : key usage value
**
**********************************************************************************************************/
static DRM_VOID _printKeyUsageEntry(
    DRM_DWORD f_dwKeyUsage,
    PRINTFUNC f_funcPrint
    )
{
    switch( f_dwKeyUsage )
    {
        case DRM_BCERT_KEYUSAGE_SIGN:
            f_funcPrint("Sign");
            break;
        case DRM_BCERT_KEYUSAGE_ENCRYPT_KEY:
            f_funcPrint("Encrypt Key");
            break;
        case DRM_BCERT_KEYUSAGE_SIGN_CRL:
            f_funcPrint("Sign CRL");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_ALL:
            f_funcPrint("Issuer-All");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_INDIV:
            f_funcPrint("Issuer-Indiv");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_DEVICE:
            f_funcPrint("Issuer-Device");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_LINK:
            f_funcPrint("Issuer-Link");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN:
            f_funcPrint("Issuer-Domain");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT:
            f_funcPrint("Issuer-Silverlight");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_CRL:
            f_funcPrint("Issuer-Crl");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION:
            f_funcPrint("Issuer-Application");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_METERING:
            f_funcPrint("Issuer-Metering");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE:
            f_funcPrint("Issuer-KeyFileSigner");
            break;
        case DRM_BCERT_KEYUSAGE_SIGN_KEYFILE:
            f_funcPrint("Sign KeyFile");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_SERVER:
            f_funcPrint("Issuer-Server");
            break;
        case DRM_BCERT_KEYUSAGE_ENCRYPTKEY_SAMPLE_PROTECTION:
            f_funcPrint("EncryptKey-SampleProtection");
            break;
        case DRM_BCERT_KEYUSAGE_ISSUER_SIGN_LICENSE:
            f_funcPrint("Issuer-LicenseSigner");
            break;
        case DRM_BCERT_KEYUSAGE_SIGN_LICENSE:
            f_funcPrint("Sign License");
            break;
         case DRM_BCERT_KEYUSAGE_SIGN_RESPONSE:
            f_funcPrint("Sign Response");
            break;
        case DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY:
            f_funcPrint("PRND Encrypt"); 
            break; 
        default:
            f_funcPrint("Unknown");
    }
    f_funcPrint(",");
}


/**********************************************************************************************************
** Function:    _printCertificateType
**
** Synopsis:    Prints out the name of certificate type
**
** Arguments:   [f_dwCertType] : feature values
**********************************************************************************************************/
static DRM_VOID _printCertificateType(
    DRM_DWORD f_dwCertType,
    PRINTFUNC f_funcPrint
)
{
    switch( f_dwCertType )
    {
        case DRM_BCERT_CERTTYPE_PC:
            f_funcPrint("PC");
            break;
        case DRM_BCERT_CERTTYPE_SILVERLIGHT:
            f_funcPrint("Silverlight");
            break;
        case DRM_BCERT_CERTTYPE_APPLICATION:
            f_funcPrint("Application");
            break;
        case DRM_BCERT_CERTTYPE_DEVICE:
            f_funcPrint("Device");
            break;
        case DRM_BCERT_CERTTYPE_DOMAIN:
            f_funcPrint("Domain");
            break;
        case DRM_BCERT_CERTTYPE_ISSUER:
            f_funcPrint("Issuer");
            break;
        case DRM_BCERT_CERTTYPE_CRL_SIGNER:
            f_funcPrint("CRL Signer");
            break;
        case DRM_BCERT_CERTTYPE_SERVICE:
            f_funcPrint("Service");
            break;
        case DRM_BCERT_CERTTYPE_METERING:
            f_funcPrint("Metering");
            break;
        case DRM_BCERT_CERTTYPE_SERVER:
            f_funcPrint("Server");
            break;
        case DRM_BCERT_CERTTYPE_KEYFILESIGNER:
            f_funcPrint("KeyFileSigner");
            break;
        case DRM_BCERT_CERTTYPE_LICENSESIGNER:
            f_funcPrint("LicenseSigner");
            break;
        default:
            f_funcPrint("Unknown");
    }
}


/**********************************************************************************************************
** Function:    _printFeatureEntry
**
** Synopsis:    Prints out feature name
**
** Arguments:   [f_dwFeature] : feature values
**********************************************************************************************************/
static DRM_VOID _printFeatureEntry(
    DRM_DWORD f_dwFeature,
    PRINTFUNC f_funcPrint
)
{
    switch( f_dwFeature )
    {
        case DRM_BCERT_FEATURE_TRANSMITTER:
            f_funcPrint("Transmitter");
            break;
        case DRM_BCERT_FEATURE_RECEIVER:
            f_funcPrint("Receiver");
            break;
        case DRM_BCERT_FEATURE_SHARED_CERTIFICATE:
            f_funcPrint("Shared Certificate");
            break;
        case DRM_BCERT_FEATURE_SECURE_CLOCK:
            f_funcPrint("Secure Clock");
            break;
        case DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK:
            f_funcPrint("Antirollback Clock");
            break;
        case DRM_BCERT_FEATURE_SUPPORTS_CRLS:
            f_funcPrint("CRL Support");
            break;
        case DRM_BCERT_FEATURE_SERVER_BASIC_EDITION:
            f_funcPrint("Server Basic Edition");
            break;
        case DRM_BCERT_FEATURE_SERVER_STANDARD_EDITION:
            f_funcPrint("Server Standard Edition");
            break;
        case DRM_BCERT_FEATURE_SERVER_PREMIUM_EDITION:
            f_funcPrint("Server Premium Edition");
            break;
        default:
            f_funcPrint("Unknown");
    }
    f_funcPrint(",");
}


/**********************************************************************************************************
** Function:    _printBytes
**
** Synopsis:    Prints out binary data as a string of hexes.
**
** Arguments:   [f_pbData]      : binary data buffer
**              [f_cbData]      : size of f_pbData in bytes
**              [f_fB64Encode]  : convert into B64 encoding or not
**********************************************************************************************************/
static DRM_VOID _printBytes(
    const DRM_BYTE  *f_pbData,
          DRM_DWORD  f_cbData,
          DRM_BOOL   f_fB64Encode,
          PRINTFUNC  f_funcPrint )
{
    DRM_DWORD dwIndex = 0;
    DRM_CHAR *pchString = NULL;
    DRM_DWORD dwStringLen = 0;

    if ( f_pbData == NULL )
    {
        f_funcPrint("Null");
        return;
    }

    if( f_fB64Encode )
    {
        dwStringLen = CCH_BASE64_EQUIV(f_cbData);
        pchString = (DRM_CHAR*)Oem_MemAlloc( dwStringLen+1 );
        ZEROMEM(pchString, dwStringLen+1);

        if ( DRM_FAILED( DRM_B64_EncodeA( f_pbData,
                                          f_cbData,
                                          pchString,
                                          &dwStringLen,
                                          0 ) ) )
        {
            f_funcPrint("Error");
        }
        else
        {
            f_funcPrint( pchString  );
        }
        SAFE_OEM_FREE(pchString);
    }
    else
    {
        for ( dwIndex = 0; dwIndex < f_cbData; dwIndex++ )
        {
            if ( f_pbData[ dwIndex ] > 0xF )
            {
                f_funcPrint( "%2X", f_pbData[ dwIndex ] );
            }
            else
            {
                f_funcPrint( "0%1X", f_pbData[ dwIndex ] );
            }
        }
    }
}


/**********************************************************************************************************
** Function:    _printCertTime
**
** Synopsis:    Converts DWORD which is "unsigned number of seconds since Jan 1, 1970 represented in UTC"
**              into a human-readable string.
**
** Arguments:   [f_dwTime]      : DWORD value that represent time according to binary cert format. May be
**                                set into DRM_BCERT_DEFAULT_EXPIRATION_DATE in which case the date is not set.
**********************************************************************************************************/
static DRM_VOID _printCertTime(
        DRM_DWORD  f_dwTime,
        PRINTFUNC f_funcPrint
)
{
    DRMFILETIME   fTime      = { 0 };
    DRMSYSTEMTIME systemTime = { 0 };

    if( f_dwTime == DRM_BCERT_DEFAULT_EXPIRATION_DATE )
    {
        f_funcPrint("Not set");
        return;
    }

    CREATE_FILE_TIME( f_dwTime, fTime );
    if ( !Oem_Clock_FileTimeToSystemTime( &fTime, &systemTime ) )
    {
        f_funcPrint("Error");
        return;
    }

    f_funcPrint( "%02d/%02d/%04d %02d:%02d:%02d GMT",
            systemTime.wMonth,
            systemTime.wDay,
            systemTime.wYear,
            systemTime.wHour,
            systemTime.wMinute,
            systemTime.wSecond );
}

/**********************************************************************************************************
** Function:    _printSecurityVersion
**
** Synopsis:    Prints out the DWORD Security version as an A.B.C.D format string
**
** Arguments:   [f_dwSecurityVersion] : DWORD to print
**              [f_funcPrint]         : Function pointer for printing
**********************************************************************************************************/
static DRM_VOID _printSecurityVersion( DRM_DWORD f_dwSecurityVersion, PRINTFUNC f_funcPrint )
{
    f_funcPrint( "%d.%d.%d.%d",
                 ( f_dwSecurityVersion & 0xFF000000 ) >> 24,
                 ( f_dwSecurityVersion & 0x00FF0000 ) >> 16,
                 ( f_dwSecurityVersion & 0x0000FF00 ) >> 8,
                 f_dwSecurityVersion & 0x000000FF );
}

/**********************************************************************************************************
** Function:    _printCertificate
**
** Synopsis:    Prints out the content of one certificate.
**
** Arguments:   [f_poCertificate]       : pointer to a structure with certificate data
**              [f_fVerboseOutput]      : if FALSE then print out only basic information, otherwise
**                                        print more data.
**              [f_fUseB64]             : B64-encode data
**              [f_funcPrint]           : Function pointer for printing
**********************************************************************************************************/
static DRM_VOID _printCertificate(
    const DRM_BCERT_CERTIFICATE  *f_poCertificate,
          DRM_BOOL                f_fVerboseOutput,
          DRM_BOOL                f_fUseB64,
          PRINTFUNC               f_funcPrint)
{
    DRM_DWORD dwIndex = 0;
    DRM_DWORD dwKeys  = 0;
    if ( f_poCertificate == NULL )
    {
        f_funcPrint("Null");
        return;
    }

    if ( f_fVerboseOutput )
    {
        f_funcPrint( "        <Version>%d</Version>\n", f_poCertificate->Header.dwVersion );
        f_funcPrint( "        <Total_Length>%d</Total_Length>\n", f_poCertificate->Header.cbCertificate );
        f_funcPrint( "        <Signed_Length>%d</Signed_Length>\n", f_poCertificate->Header.cbCertificateSigned );
    }

    /* Basic info */
    f_funcPrint( "        " );
    _printObjectHeader( &f_poCertificate->BasicInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

    f_funcPrint( "            <Cert_Type>");
    _printCertificateType( f_poCertificate->BasicInfo.dwType, f_funcPrint );
    f_funcPrint( "</Cert_Type>\n");

    f_funcPrint( "            <Security_Level>%d</Security_Level>\n", f_poCertificate->BasicInfo.dwSecurityLevel );

    f_funcPrint( "            <Expiration>");
    _printCertTime( f_poCertificate->BasicInfo.dwExpirationDate, f_funcPrint );
    f_funcPrint( "</Expiration>\n");

    if ( f_fVerboseOutput )
    {
        f_funcPrint( "            <Flags>0x%X</Flags>\n", f_poCertificate->BasicInfo.dwFlags );

        f_funcPrint( "            <Certificate_Id>" );
        _printBytes( f_poCertificate->BasicInfo.CertificateID.rgb, DRM_BCERT_CERT_ID_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Certificate_Id>\n" );

        f_funcPrint( "            <Digest>" );
        _printBytes( f_poCertificate->BasicInfo.DigestValue.m_rgbDigest, SHA256_DIGEST_SIZE_IN_BYTES, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Digest>\n" );

        f_funcPrint( "            <Client_Id>" );
        _printBytes( f_poCertificate->BasicInfo.ClientID.rgb, DRM_BCERT_CLIENT_ID_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Client_Id>\n");
    }

    if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_PC )
    {
        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.PCInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Security_Version>" );
        _printSecurityVersion( f_poCertificate->Type.PCInfo.dwSecurityVersion, f_funcPrint );
        f_funcPrint("</Security_Version>\n");

        f_funcPrint( "        " );
        _printObjectHeader( &f_poCertificate->Type.PCInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_SILVERLIGHT )
    {
        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.SilverLightInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Security_Version>" );
        _printSecurityVersion( f_poCertificate->Type.SilverLightInfo.dwSecurityVersion, f_funcPrint );
        f_funcPrint("</Security_Version>\n");
        f_funcPrint( "                <Platform_Identifier>%d", f_poCertificate->Type.SilverLightInfo.dwPlatformIdentifier );

        switch (f_poCertificate->Type.SilverLightInfo.dwPlatformIdentifier)
        {
            case DRM_BCERT_SILVERLIGHT_PLATFORM_WINDOWS_X86:
                f_funcPrint(" (Windows X86)");
                break;
            case DRM_BCERT_SILVERLIGHT_PLATFORM_OSX_X86:
                f_funcPrint(" (MacOS X86)");
                break;
            default:
                break;
        }
        f_funcPrint( "</Platform_Identifier>\n" );

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.SilverLightInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_APPLICATION )
    {
        /* no additional fields */
    }
    else if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_DEVICE )
    {
        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.DeviceInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Max_License_Size>%d</Max_License_Size>\n", f_poCertificate->Type.DeviceInfo.cbMaxLicense );
        f_funcPrint( "                <Max_Header_Size>%d</Max_Header_Size>\n", f_poCertificate->Type.DeviceInfo.cbMaxHeader );
        f_funcPrint( "                <Max_Lic_Chain_Depth>%d</Max_Lic_Chain_Depth>\n", f_poCertificate->Type.DeviceInfo.dwMaxChainDepth );

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.DeviceInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_DOMAIN )
    {
        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.DomainInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Service_Id>");
        _printBytes( f_poCertificate->Type.DomainInfo.ServiceID.rgb, DRM_BCERT_SERVICE_ID_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Service_Id>\n" );

        f_funcPrint( "                <Account_Id>");
        _printBytes( f_poCertificate->Type.DomainInfo.AccountID.rgb, DRM_BCERT_ACCOUNT_ID_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Account_Id>\n" );

        f_funcPrint( "                <Revision>");
        _printCertTime( f_poCertificate->Type.DomainInfo.dwRevision, f_funcPrint );
        f_funcPrint( "</Revision>\n" );

        f_funcPrint( "                <URL>%s</URL>\n", f_poCertificate->Type.DomainInfo.DomainURL.rgb );
        f_funcPrint( "                <URL_Length>%d</URL_Length>\n", f_poCertificate->Type.DomainInfo.DomainURL.cb );

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.DomainInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_METERING )
    {
        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.MeteringInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Metering_Id>");
        _printBytes( f_poCertificate->Type.MeteringInfo.MeteringID.rgb, DRM_BCERT_METERING_ID_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Metering_Id>\n" );

        f_funcPrint( "                <URL>%s</URL>\n", f_poCertificate->Type.MeteringInfo.MeteringURL.rgb );
        f_funcPrint( "                <URL_Length>%d</URL_Length>\n", f_poCertificate->Type.MeteringInfo.MeteringURL.cb );

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.MeteringInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else if ( f_poCertificate->BasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVER )
    {
        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.ServerInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Warning_Days>%d</Warning_Days>\n", f_poCertificate->Type.ServerInfo.dwWarningDays );

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->Type.ServerInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }

    f_funcPrint( "            " );
    _printObjectHeader( &f_poCertificate->FeatureInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

    if ( f_poCertificate->FeatureInfo.dwNumFeatureEntries == 0 )
    {
        f_funcPrint( "Empty");
    }
    else
    {
        for ( dwIndex = 0; dwIndex < f_poCertificate->FeatureInfo.dwNumFeatureEntries; dwIndex++ )
        {
            _printFeatureEntry( f_poCertificate->FeatureInfo.rgdwFeatureSet[ dwIndex ], f_funcPrint );
        }
    }

    f_funcPrint( "\n            " );
    _printObjectHeader( &f_poCertificate->FeatureInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );

    f_funcPrint( "        " );
    _printObjectHeader( &f_poCertificate->BasicInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );

    /* Key info */
    f_funcPrint( "        " );
    _printObjectHeader( &f_poCertificate->KeyInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

    for ( dwKeys = 0; dwKeys < f_poCertificate->KeyInfo.dwNumKeys; dwKeys++ )
    {
        f_funcPrint( "            <KEY_%d>\n", dwKeys );
        f_funcPrint( "                <Key_Type>0x%X</Key_Type>\n", f_poCertificate->KeyInfo.rgoKeys[dwKeys].wType );
        f_funcPrint( "                <Length>%d bit</Length>\n", f_poCertificate->KeyInfo.rgoKeys[dwKeys].wLength );
        f_funcPrint( "                <Key_Value>" );
        _printBytes( f_poCertificate->KeyInfo.rgoKeys[dwKeys].pValue->m_rgbPubkey, DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Key_Value>\n" );
        f_funcPrint( "                <Key_Usage>" );
        if ( f_poCertificate->KeyInfo.rgoKeys[dwKeys].dwUsageSet == 0 )
        {
            f_funcPrint( "Empty");
        }
        else
        {
            for ( dwIndex = 1; dwIndex <= DRM_BCERT_KEYUSAGE_MAX_VALUE; dwIndex++ )
            {
                if ( ( ( f_poCertificate->KeyInfo.rgoKeys[dwKeys].dwUsageSet ) & BCERT_KEYUSAGE_BIT( dwIndex ) ) > 0 )
                {
                    _printKeyUsageEntry( dwIndex, f_funcPrint );
                }
            }
        }
        f_funcPrint( "</Key_Usage>\n" );
        f_funcPrint( "            </KEY_%d>\n", dwKeys );
    }

    f_funcPrint( "        " );
    _printObjectHeader( &f_poCertificate->KeyInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );

    /* Security version */
    f_funcPrint( "        " );
    if ( _printObjectHeader( &f_poCertificate->SecurityVersion.Header, f_fVerboseOutput, TRUE, f_funcPrint ) )
    {
        f_funcPrint( "            <Security_Version>" );
        _printSecurityVersion( f_poCertificate->SecurityVersion.dwSecurityVersion, f_funcPrint );
        f_funcPrint( "</Security_Version>\n");
        f_funcPrint( "            <Platform_Identifier>%d", f_poCertificate->SecurityVersion.dwPlatformIdentifier );
        switch (f_poCertificate->SecurityVersion.dwPlatformIdentifier)
        {
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WINDOWS:
                f_funcPrint(" (Windows)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_OSX:
                f_funcPrint(" (Mac OSX)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WM_7:
                f_funcPrint(" (Windows Mobile 7)");
                break;
            default:
                f_funcPrint(" (Invalid)");
                break;
        }
        f_funcPrint( "</Platform_Identifier>\n" );

        f_funcPrint( "        " );
        _printObjectHeader( &f_poCertificate->SecurityVersion.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else
    {
        f_funcPrint( "\n" );
    }

    /* Security version 2 */
    f_funcPrint( "        " );
    if (_printObjectHeader( &f_poCertificate->SecurityVersion2.Header, f_fVerboseOutput, TRUE, f_funcPrint ) )
    {
        f_funcPrint( "            <Security_Version>" );
        _printSecurityVersion( f_poCertificate->SecurityVersion2.dwSecurityVersion, f_funcPrint );
        f_funcPrint( "</Security_Version>\n");
        f_funcPrint( "            <Platform_Identifier>%d", f_poCertificate->SecurityVersion2.dwPlatformIdentifier );
        switch (f_poCertificate->SecurityVersion2.dwPlatformIdentifier)
        {
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WINDOWS:
                f_funcPrint(" (Windows)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_OSX:
                f_funcPrint(" (Mac OSX)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WINDOWS_ARM:
                f_funcPrint(" (Windows On ARM)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WM_7:
                f_funcPrint(" (Windows Mobile 7)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_IOS_ARM:
                f_funcPrint(" (iOS on ARM)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_XBOX_PPC:
                f_funcPrint(" (XBOX on PPC)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WP8_ARM:
                f_funcPrint(" (WP8 on ARM)");
                break;
            case DRM_BCERT_SECURITY_VERSION_PLATFORM_WP8_X86:
                f_funcPrint(" (WP8 on X86)");
                break;
            default:
                f_funcPrint(" (UNKNOWN but not necessarily invalid)");
                break;
        }
        f_funcPrint( "</Platform_Identifier>\n" );

        f_funcPrint( "        " );
        _printObjectHeader( &f_poCertificate->SecurityVersion2.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else
    {
        f_funcPrint( "\n" );
    }

    f_funcPrint( "        " );
    if ( _printObjectHeader( &f_poCertificate->ManufacturerInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint ) )
    {
        f_funcPrint( "            <Manufacturer>%s</Manufacturer>\n", f_poCertificate->ManufacturerInfo.ManufacturerStrings.ManufacturerName.rgb );
        f_funcPrint( "            <Model_Name>%s</Model_Name>\n", f_poCertificate->ManufacturerInfo.ManufacturerStrings.ModelName.rgb );
        f_funcPrint( "            <Model_Number>%s</Model_Number>\n", f_poCertificate->ManufacturerInfo.ManufacturerStrings.ModelNumber.rgb );

        f_funcPrint( "        " );
        _printObjectHeader( &f_poCertificate->ManufacturerInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else
    {
        f_funcPrint( "\n" );
    }

    /*
    ** Extended data signing key
    */
    f_funcPrint( "        " );
    if ( _printObjectHeader( &f_poCertificate->ExtDataSignatureInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint ) )
    {
        f_funcPrint( "            <Key_Type>0x%X</Key_Type>\n", f_poCertificate->ExtDataSignatureInfo.wType );
        f_funcPrint( "            <Length>%d bit</Length>\n", f_poCertificate->ExtDataSignatureInfo.wLength );
        f_funcPrint( "            <Key_Value>" );
        _printBytes( f_poCertificate->ExtDataSignatureInfo.pValue->m_rgbPubkey, DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH, f_fUseB64, f_funcPrint );
        f_funcPrint( "</Key_Value>\n");

        f_funcPrint( "        " );
        _printObjectHeader( &f_poCertificate->ExtDataSignatureInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else
    {
        f_funcPrint( "\n" );
    }

    /* Cert signature */
    f_funcPrint( "        " );
    _printObjectHeader( &f_poCertificate->SignatureInfo.Header, f_fVerboseOutput, TRUE, f_funcPrint );

    f_funcPrint( "            <Signature_Type>0x%X</Signature_Type>\n", f_poCertificate->SignatureInfo.wSignatureType );
    f_funcPrint( "            <Issuer_Key_Length>%d</Issuer_Key_Length>\n", f_poCertificate->SignatureInfo.dwIssuerKeyLength );
    f_funcPrint( "            <Issuer_Key_Value>" );
    _printBytes( f_poCertificate->SignatureInfo.pIssuerKey->m_rgbPubkey, DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH, f_fUseB64, f_funcPrint );
    f_funcPrint( "</Issuer_Key_Value>\n");

    f_funcPrint( "        " );
    _printObjectHeader( &f_poCertificate->SignatureInfo.Header, f_fVerboseOutput, FALSE, f_funcPrint );

    /* Extended data container (PC only) */
    f_funcPrint( "        " );
    if ( _printObjectHeader( &f_poCertificate->ExtDataContainer.Header, f_fVerboseOutput, TRUE, f_funcPrint ) )
    {
        for ( dwIndex = 0; dwIndex < f_poCertificate->ExtDataContainer.cbRecordsCount; dwIndex++ )
        {
            f_funcPrint( "            " );
            _printObjectHeader( &f_poCertificate->ExtDataContainer.Records[dwIndex].Header, f_fVerboseOutput, TRUE, f_funcPrint );

            f_funcPrint( "                <Length>%d</Length>\n", f_poCertificate->ExtDataContainer.Records[dwIndex].dwDataLength );
            f_funcPrint( "                <Data>");
            _printBytes( f_poCertificate->ExtDataContainer.Records[dwIndex].pbData,
                         f_poCertificate->ExtDataContainer.Records[dwIndex].dwDataLength,
                         f_fUseB64,
                         f_funcPrint );
            f_funcPrint( "</Data>\n");

            f_funcPrint( "            " );
            _printObjectHeader( &f_poCertificate->ExtDataContainer.Records[dwIndex].Header, f_fVerboseOutput, FALSE, f_funcPrint );
        }

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->ExtDataContainer.Signature.Header, f_fVerboseOutput, TRUE, f_funcPrint );

        f_funcPrint( "                <Signature_Type>0x%X</Signature_Type>\n", f_poCertificate->ExtDataContainer.Signature.wSignatureType );
        f_funcPrint( "                <Data>" );
        _printBytes( f_poCertificate->ExtDataContainer.Signature.SignatureData.pValue,
                     f_poCertificate->ExtDataContainer.Signature.SignatureData.cb,
                     f_fUseB64,
                     f_funcPrint );
        f_funcPrint( "</Data>\n");

        f_funcPrint( "            " );
        _printObjectHeader( &f_poCertificate->ExtDataContainer.Signature.Header, f_fVerboseOutput, FALSE, f_funcPrint );

        f_funcPrint( "        " );
        _printObjectHeader( &f_poCertificate->ExtDataContainer.Header, f_fVerboseOutput, FALSE, f_funcPrint );
    }
    else
    {
        f_funcPrint( "\n" );
    }
}


/**********************************************************************************************************
** Function:    DRM_BCDM_PrintCertChain
**
** Synopsis:    Parses certificate chain and prints out the content.
**
** Arguments:   [f_pbBinaryCertData]       : data buffer with a certificate chain
**              [f_cbBinaryCertDataLength] : size of the data buffer (in bytes)
**              [f_fVerboseOutput]         : if FALSE then print out only basic information, otherwise
**                                           print more data.
**              [f_fUseB64]                : Flag is pring binary data in hex or b64 encoded format.
**              [f_funcPrint]              : Pointer to a function that provides output. Normally printf.
**
** Returns:     DRM_SUCCESS      - on success
**
**********************************************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_BCDM_PrintCertChain (
    __in_bcount(f_cbBinaryCertDataLength) const DRM_BYTE  *f_pbBinaryCertData,
    __in                                        DRM_DWORD  f_cbBinaryCertDataLength,
    __in                                        DRM_BOOL   f_fVerboseOutput,
    __in                                        DRM_BOOL   f_fUseB64,
    __in                                        PRINTFUNC  f_funcPrint )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  dwIndex = 0;
    DRM_DWORD  bCurrOffset = 0;
    DRM_BOOL   fStopParsingOnError = FALSE;
    DRM_BCERT_CERTIFICATE  oCertificate = { 0 };
    DRM_BCERT_CHAIN_HEADER oCertChainHeader = { 0 };
    DRM_CRYPTO_CONTEXT *pCryptoContext      = NULL;
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRMFILETIME  ftCurrentTime = {0};

    f_funcPrint( "<PlayReady_Cert_Chain>\n" );

    ChkArg( f_pbBinaryCertData != NULL );

    /*
    ** Allocate memory for crypto context.
    */
    ChkMem( pCryptoContext    = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );

    /*
    ** Verify and parse a parent certificate:
    ** See if cert chain fails any signatures verification (which is fatal error)
    ** or any other fatal error occurs,
    ** or there is a non-empty list of verification errors.
    */
    /*
    **
    */

    ChkDR( DRM_BCert_InitVerificationContext( &ftCurrentTime,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_UNKNOWN,
                                              pCryptoContext,
                                              TRUE,
                                              FALSE,
                                              NULL,
                                              0,
                                              FALSE,    /* do not collect errors, just stop parsing */
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );
    dr = DRM_BCert_ParseCertificateChain( f_pbBinaryCertData,
                                          f_cbBinaryCertDataLength,
                                          &oVerificationContext );

    if ( DRM_FAILED( dr ) )
    {
        if ( dr == DRM_E_BCERT_HWIDINFO_IS_MISSING )
        {
            f_funcPrint( "<Message>Warning: Extended data is missing!</Message>\n" );
        }
        else if ( dr == DRM_E_INVALID_SIGNATURE )
        {
            f_funcPrint( "<Message>Error: Certificate's signature is invalid!</Message>\n" );
        }
        else if ( dr == DRM_E_BCERT_BASICINFO_CERT_EXPIRED )
        {
            f_funcPrint( "<Message>Warning: certificate is expired!</Message>\n" );
        }
        /*
        ** parse again, collect non-fatal errors
        */
        ChkDR( DRM_BCert_InitVerificationContext( NULL, /* no expiration time check */
                                                  NULL,
                                                  DRM_BCERT_CERTTYPE_UNKNOWN,
                                                  NULL,
                                                  FALSE, /* no signatures check */
                                                  TRUE, /* no extended data check */
                                                  NULL,
                                                  0,
                                                  TRUE, /* collect non-fatal errors */
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  &oVerificationContext ) );
        dr = DRM_BCert_ParseCertificateChain( f_pbBinaryCertData,
                                              f_cbBinaryCertDataLength,
                                              &oVerificationContext );

        if ( DRM_FAILED( dr ) )
        {
            if ( dr == DRM_E_BCERT_VERIFICATION_ERRORS )
            {
                f_funcPrint( "<Message>Error: Verification errors found: \n" );
                for ( dwIndex = 0; dwIndex < oVerificationContext.cResults; dwIndex++ )
                {
                    f_funcPrint( "Certificate #%d Error=0x%X;\n",
                                 oVerificationContext.rgoResults[dwIndex].cCertNumber + 1,
                                 oVerificationContext.rgoResults[dwIndex].dwResult );
                }
                f_funcPrint( "</Message>\n" );
            }
            else
            {
                f_funcPrint( "<Message>Error: Errors found while parsing certificate chain, error code=0x%X</Message>\n", dr );
            }
        }
    }

    dr = DRM_BCert_GetChainHeader( f_pbBinaryCertData,
                                   f_cbBinaryCertDataLength,
                                   &bCurrOffset,
                                   &oCertChainHeader );
    if ( DRM_FAILED( dr ) )
    {
        f_funcPrint( "<Message>Error: Cannot parse the certificate chain header, error code=0x%X</Message>\n", dr );
        goto ErrorExit;
    }

    if ( f_fVerboseOutput )
    {
        f_funcPrint( "    <Format>v.%d</Format>\n", oCertChainHeader.dwVersion );
        f_funcPrint( "    <Length>%d</Length>\n", oCertChainHeader.cbChain );
    }

    f_funcPrint( "    <Number_of_Certs>%d</Number_of_Certs>\n", oCertChainHeader.cCerts );

    for ( dwIndex = 0; dwIndex < oCertChainHeader.cCerts ; dwIndex++ )
    {
        f_funcPrint( "\n" );
        f_funcPrint( "    <Certificate_%d>\n", dwIndex + 1 );

        dr = DRM_BCert_GetCertificate( f_pbBinaryCertData,
                                       f_cbBinaryCertDataLength,
                                       &bCurrOffset,
                                       &oCertificate,
                                       DRM_BCERT_CERTTYPE_UNKNOWN );

        if ( dr == DRM_E_BCERT_HWIDINFO_IS_MISSING )
        {
            dr = DRM_SUCCESS;
            f_funcPrint( "<Message>Partially built certificate: HWID is missing</Message>\n" );
        }

        if ( DRM_FAILED( dr ) )
        {
            f_funcPrint( "<Message>Cannot parse the certificate, error code=0x%X</Message>\n", dr );
            if ( fStopParsingOnError )
            {
                f_funcPrint( "<Message>Stop parsing</Message>\n" );
                f_funcPrint( "</Certificate_%d>\n", dwIndex + 1 );
                f_funcPrint( "</PlayReady_Cert_Chain>\n" );
                goto ErrorExit;
            }
            fStopParsingOnError = TRUE; /* try parsing one more certificate */
        }
        else
        {
            _printCertificate( &oCertificate, f_fVerboseOutput, f_fUseB64, f_funcPrint );
        }

        ZEROMEM( &oCertificate, SIZEOF(DRM_BCERT_CERTIFICATE) );

        f_funcPrint( "\n    </Certificate_%d>\n", dwIndex + 1 );
        f_funcPrint( "\n" );

        if ( dr == DRM_S_FALSE )
        {
            dr = DRM_SUCCESS; /* it means that there are no more certs in the chain i.e. success */
            break;
        }
    }

ErrorExit:
    SAFE_OEM_FREE(pCryptoContext);

    f_funcPrint( "</PlayReady_Cert_Chain>\n" );
    return dr;
}

#if BUILD_EXE
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR** argv )
{
    DRM_RESULT       dr                     = DRM_SUCCESS;
    DRM_CONST_STRING dstrBinaryCertFile     = EMPTY_DRM_STRING;
    DRM_BOOL         fVerboseOutput         = FALSE;
    DRM_BOOL         fB64Input              = FALSE;
    DRM_BYTE*        pbCertData             = NULL;
    DRM_DWORD        cbCertData             = 0;
    DRM_SUBSTRING    dastrB64CertData       = EMPTY_DRM_SUBSTRING;
    DRM_BYTE*        pbBinaryCertData       = NULL;
    DRM_DWORD        cbBinaryCertDataLength = 0;
    DRM_LONG         iParam                 = 0;
    DRM_BOOL         fUseB64                = TRUE;

    PrintCopyrightInfo( "Binary certificate viewer" );

    /*
    **  Check input
    */
    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    ChkArg( argv != NULL );
    for( iParam = 1; iParam < argc; iParam++ )
    {
        DRM_CONST_STRING dstrOption = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrParam  = EMPTY_DRM_STRING;

        if ( !DRM_CMD_ParseCmdLineMultiChar( argv[iParam], &dstrOption, &dstrParam, NULL ) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        if ( DRM_UTL_DSTRStringsEqual( &dstrOption, &g_dstrBINCERT_OPTION_STRING ) )
        {
            ASSIGN_DRM_STRING( dstrBinaryCertFile, dstrParam );
        }
        else if( DRM_UTL_DSTRStringsEqual( &dstrOption, &g_dstrVERBOSE_OPTION_STRING ) )
        {
            fVerboseOutput = TRUE;
        }
        else if( DRM_UTL_DSTRStringsEqual( &dstrOption, &g_dstrINPUTFORMAT_OPTION_STRING ) )
        {
            fB64Input = TRUE;
        }
        else if ( DRM_UTL_DSTRStringsEqual( &dstrOption, &g_dstrFORMAT_OPTION_STRING ) )
        {
            if ( DRM_UTL_DSTRStringsEqual( &dstrParam, &g_dstrHEX_FORMAT_STRING ) )
            {
                fUseB64 = FALSE;
            }
            else if ( DRM_UTL_DSTRStringsEqual( &dstrParam, &g_dstrB64_FORMAT_STRING ) )
            {
                fUseB64 = TRUE;
            }
            else
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

        }
        else
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    if ( dstrBinaryCertFile.cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    if ( dstrBinaryCertFile.cchString > MAX_FILENAME )
    {
        printf( "File name is too big. Max Filename is %d \n", MAX_FILENAME );
        ChkArg( FALSE );
    }

    /*
    ** Open binary cert file
    */
    ChkDR( DRMTOOLS_ReadBinaryFileData( &dstrBinaryCertFile,
                                        &pbBinaryCertData,
                                        &cbBinaryCertDataLength ) );

    if( fB64Input )
    {
        dastrB64CertData.m_ich = 0;
        dastrB64CertData.m_cch = cbBinaryCertDataLength;
        cbCertData     = CB_BASE64_DECODE( dastrB64CertData.m_cch );
        ChkMem( pbCertData = (DRM_BYTE *)Oem_MemAlloc(cbCertData) );
        ChkDR( DRM_B64_DecodeA( (DRM_CHAR*)pbBinaryCertData, &dastrB64CertData, &cbCertData, pbCertData, 0 ) );

        SAFE_OEM_FREE( pbBinaryCertData );
    }
    else
    {
        pbCertData = pbBinaryCertData;
        cbCertData = cbBinaryCertDataLength;
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_FUNCTION_NEEDS_REVIEW, "printf usage reviewed." )
    ChkDR( DRM_BCDM_PrintCertChain( pbCertData, cbCertData, fVerboseOutput, fUseB64, printf ) );
PREFAST_POP /* __WARNING_FUNCTION_NEEDS_REVIEW */

ErrorExit:

    SAFE_OEM_FREE( pbCertData );

    if ( DRM_FAILED( dr ) )
    {
        printf( "Failure: 0x%X\n", dr );
    }
    return dr;

_PrintUsage:
    printf("Syntax: %S [-?]\n\
-b:CertFileName    - Path to the file with PlayReady certificate\n\
[-format:hex|b64]  - Optionally specify output format.  Default is b64\n\
[-e]               - Optionally indicate input file data is b64 encoded\n\
[-v]               - Optional Parameter for verbose output\n", argv == NULL ? &g_wchNull : argv[0]);

    return dr;
}
#endif /* BUILD_EXE */

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

