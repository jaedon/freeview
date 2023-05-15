/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DEVICEDEVCERT_C
#include <devicedevcert.h>
#include <drmbcertparser.h>
#include <oemkeyfile.h>
#include <drmactivation.h>
#include <drmactivationimp.h>
#include <drmdevcertkeygen.h>
#include <drmmetering.h>
#include <drmwmdrm.h>
#include <drmantirollbackclock.h>
#include <drmsecureclock.h>
#include <drmprnd.h>
#include <oemeccp160.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define MAX_WMDRMPD_UNIQUEID_LEN        256

/*****************************************************************************
** Function:    _MatchXMLCertRobustnessPlatformAndModel
**
** Synopsis:    Compares platform ID and robustness version retrieved from
**              model-level XML cert with the same data from PlayReady.dll.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_pdstrXMLCert]: A pointer to a string with XML certificate.
**              [f_fDeviceCert] : If TRUE then this is device-level cert,
**                                otherwise it's a model-level cert.
**
** Returns:     DRM_SUCCESS             - on success, the identifiers match
**              DRM_E_INVALIDARG        - if the machine ID parameter is invalid
**              DRM_E_KEYFILE_ROBUSTNESSVERSION_MISMATCH
**              DRM_E_KEYFILE_PLATFORMID_MISMATCH
**              DRM_E_DEVCERT_MODEL_MISMATCH
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _MatchXMLCertRobustnessPlatformAndModel(
    __in_opt       DRM_VOID         *f_pOEMContext,
    __in     const DRM_CONST_STRING *f_pdstrXMLCert,
    __in           DRM_BOOL          f_fDeviceCert )
{
    DRM_RESULT dr   = DRM_SUCCESS;
#if !DRM_TEST_IGNORE_UNIQUE_ID
    DRM_DWORD  dwRobustnessVersion     = 0;
    DRM_DWORD  dwPlatform              = 0;
    DRM_DWORD  dwCertRobustnessVersion = 0;
    DRM_DWORD  dwCertPlatform          = 0;
    DRM_CONST_STRING dstrRetVal        = EMPTY_DRM_STRING;

    ChkDR( Oem_Keyfile_GetRobustnessVersion ( f_pOEMContext,
                                              KF_CERT_TYPE_WMDRM,
                                             &dwRobustnessVersion,
                                             &dwPlatform ) );

    /*
    ** Retrieve the robustness version info
    */
    dr = DRM_DCP_GetAttribute(
        NULL,
        f_pdstrXMLCert,
        DRM_DEVCERT_GROUPSECURITYVERSIONNUM,
        NULL,
        &dstrRetVal );

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        /*
        ** Set robustness version into 0 if not found.
        ** However if robustness version is supported then it will never be 0 in PlayReady.dll
        ** and this XML cert will fail verification.
        */
        dwCertRobustnessVersion = 0;
    }
    else
    {
        ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrRetVal.pwszString,
                                                    dstrRetVal.cchString,
                                                    &dwCertRobustnessVersion ) );
    }

    dr = DRM_DCP_GetAttribute(
        NULL,
        f_pdstrXMLCert,
        DRM_DEVCERT_GROUPPLATFORMID,
        NULL,
        &dstrRetVal);

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        dwCertPlatform = 0;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR(DRMCRT_wcsntol( dstrRetVal.pwszString, dstrRetVal.cchString, (DRM_LONG *) &dwCertPlatform ) );
    }

    ChkBOOL( dwRobustnessVersion == dwCertRobustnessVersion,
             DRM_E_KEYFILE_ROBUSTNESSVERSION_MISMATCH );
    ChkBOOL( dwPlatform == dwCertPlatform,
             DRM_E_KEYFILE_PLATFORMID_MISMATCH );

    if( f_fDeviceCert )
    {
        DRM_WCHAR rgwchDecodedVal[(DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR)) + 1] = {0};
        DRM_DWORD cchDecodedVal         = (DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR)) + 1;
        DRM_WCHAR rgwchOEMVal[(DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR)) + 1] = {0};
        DRM_DWORD cchOEMVal = 0;

        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            ChkDR( dr );
        }
        cchOEMVal = (DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR)) + 1;
        dr = Oem_Device_GetModelInfo( f_pOEMContext, rgwchOEMVal, &cchOEMVal );
        if( dr != DRM_E_NOTIMPL )
        {
            /*
            ** Retrieve the model info
            */
            ChkDR( DRM_DCP_GetAttribute(
                NULL,
                (DRM_CONST_STRING*) f_pdstrXMLCert,
                DRM_DEVCERT_DEVICEMODELNAME,
                NULL,
                &dstrRetVal ) );

            /*
            ** Model info retrieved is XML encoded, need to decode
            */
            ChkDR( DRM_UTL_XMLDecode( dstrRetVal.pwszString, dstrRetVal.cchString, rgwchDecodedVal, &cchDecodedVal ) );

            if( cchOEMVal != cchDecodedVal
             || MEMCMP( rgwchDecodedVal, rgwchOEMVal, cchOEMVal * SIZEOF(DRM_WCHAR) ) != 0 )
            {
                ChkDR( DRM_E_DEVCERT_MODEL_MISMATCH );
            }
        }
        else
        {
            ChkDR( DRM_SUCCESS );
        }
    }
   ErrorExit:
#endif /* DRM_TEST_IGNORE_UNIQUE_ID */
    return dr;
}


/*****************************************************************************
** Function:    DRM_DDC_MatchRobustnessPlatformAndModel
**
** Synopsis:    Compares platform ID and robustness version retrieved from
**              model-level binary cert with the same data from PlayReady.dll.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_pbCertData]  : A pointer to binary cert chain
**              [f_cbCertData]  : Size of binary cert chain data
**              [f_fDeviceCert] : If TRUE then this is device-level cert,
**                                otherwise it's a model-level cert.
**
** Returns:     DRM_SUCCESS             - on success, the identifiers match
**              DRM_E_INVALIDARG        - if the machine ID parameter is invalid
**              DRM_E_KEYFILE_ROBUSTNESSVERSION_MISMATCH
**              DRM_E_KEYFILE_PLATFORMID_MISMATCH
**              DRM_E_DEVCERT_MODEL_MISMATCH
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_MatchRobustnessPlatformAndModel(
    __in_opt                        DRM_VOID  *f_pOEMContext,
    __in_bcount(f_cbCertData) const DRM_BYTE  *f_pbCertData,
    __in                            DRM_DWORD f_cbCertData,
    __in                            DRM_BOOL  f_fDeviceCert )
{
    DRM_RESULT dr                      = DRM_SUCCESS;
#if !DRM_TEST_IGNORE_UNIQUE_ID
    DRM_DWORD  dwRobustnessVersion     = 0;
    DRM_DWORD  dwPlatform              = 0;
    DRM_DWORD  dwCertRobustnessVersion = 0;
    DRM_DWORD  dwCertPlatform          = 0;

    ChkDR( Oem_Keyfile_GetRobustnessVersion ( f_pOEMContext,
                                              KF_CERT_TYPE_PLAYREADY,
                                             &dwRobustnessVersion,
                                             &dwPlatform ) );

    /*
    ** Now parse binary cert
    */
    ChkDR( DRM_BCert_GetSecurityVersion( f_pbCertData,
                                         f_cbCertData,
                                        &dwCertRobustnessVersion,
                                        &dwCertPlatform ) );

    ChkBOOL( dwRobustnessVersion == dwCertRobustnessVersion,
             DRM_E_KEYFILE_ROBUSTNESSVERSION_MISMATCH );
    ChkBOOL( dwPlatform == dwCertPlatform,
             DRM_E_KEYFILE_PLATFORMID_MISMATCH );

    if( f_fDeviceCert )
    {
        /* Model string is only checked for leaf device cert */
        DRM_DWORD cchOEMVal = 0;


        dr = Oem_Device_GetModelInfo( f_pOEMContext, NULL, &cchOEMVal );
        if( dr != DRM_E_NOTIMPL )
        {
            DRM_BCERT_CHAIN_HEADER  ChainHeader          = {0};
            DRM_BCERT_CERTIFICATE   oDeviceCert          = {0};
            DRM_DWORD               bCurrOffset          = 0;
            DRM_WCHAR               rgwchOEMVal[(DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR)) + 1] = {0};

            if(dr != DRM_E_BUFFERTOOSMALL )
            {
                ChkDR( dr );
            }
            cchOEMVal = (DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF(DRM_WCHAR)) + 1;
            ChkDR( Oem_Device_GetModelInfo( f_pOEMContext, rgwchOEMVal, &cchOEMVal ) );

            /*
            ** Skip the chain header to get to the individual certs
            */
            ChkDR( DRM_BCert_GetChainHeader( f_pbCertData,
                                             f_cbCertData,
                                             &bCurrOffset,
                                             &ChainHeader ) );


            /*
            ** Load leaf cert into a cert sturcture
            */
            ChkDR( DRM_BCert_GetCertificate( f_pbCertData,
                                             f_cbCertData,
                                             &bCurrOffset,
                                             &oDeviceCert,
                                             DRM_BCERT_CERTTYPE_DEVICE ) );

            ChkBOOL( cchOEMVal * SIZEOF(DRM_WCHAR) == oDeviceCert.ManufacturerInfo.ManufacturerStrings.ModelName.cb
              && MEMCMP(oDeviceCert.ManufacturerInfo.ManufacturerStrings.ModelName.rgb, rgwchOEMVal, cchOEMVal * SIZEOF( DRM_WCHAR )) == 0,
                     DRM_E_DEVCERT_MODEL_MISMATCH );
        }
        else
        {
            ChkDR( DRM_SUCCESS );
        }
    }

ErrorExit:
#endif /* DRM_TEST_IGNORE_UNIQUE_ID */
    return dr;
}



/*****************************************************************************
** Function:    _CompareBinaryCertsModelPublicKeys
**
** Synopsis:    Compares public key retrieved from
**              device-level binary cert with the same data from PlayReady.dll.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_pDeviceCertChain]  : device cert retrieved from keyfile
**              [f_cbDeviceCertChain] : size of f_pDeviceCertChain
**              [f_pModelCertChain]   : model cert retrieved from PlayReady.dll
**              [f_cbModelCertChain]  : size of f_pModelCertChain
**
**
** Returns:     DRM_SUCCESS             - on success, the identifiers match
**              DRM_E_INVALIDARG
**              DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH -
**              certificate issuer key does not match public key of
**              a model level certificate in PlayReady.dll
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _CompareBinaryCertsModelPublicKeys(
    __in_opt                               DRM_VOID  *f_pOEMContext,
    __in_bcount(f_cbDeviceCertChain) const DRM_BYTE  *f_pDeviceCertChain,
    __in                                   DRM_DWORD  f_cbDeviceCertChain,
    __in_bcount(f_cbModelCertChain)  const DRM_BYTE  *f_pModelCertChain,
    __in                                   DRM_DWORD  f_cbModelCertChain )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    PUBKEY_P256 oDeviceCertSigningPubKey = {0};
    PUBKEY_P256 oModelCertPubKey         = {0};

    ChkDR( DRM_BCert_GetPublicKey( f_pDeviceCertChain,
                                   f_cbDeviceCertChain,
                                   1,
                                   &oDeviceCertSigningPubKey ) );

    ChkDR( DRM_BCert_GetPublicKey( f_pModelCertChain,
                                   f_cbModelCertChain,
                                   0,
                                   &oModelCertPubKey ) );

    ChkBOOL( DRM_BYT_CompareBytes(
                oDeviceCertSigningPubKey.m_rgbPubkey,
                0,
                oModelCertPubKey.m_rgbPubkey,
                0,
                ECC_P256_POINT_SIZE_IN_BYTES) == 0, DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH );
ErrorExit:
    OEM_ECC_ZeroPublicKey_P256( &oDeviceCertSigningPubKey );
    OEM_ECC_ZeroPublicKey_P256( &oModelCertPubKey );
    return dr;
}


/*****************************************************************************
** Function:    _CompareXMLCertsModelPublicKeys
**
** Synopsis:    Compares public keys retrieved from
**              device-level XML cert with the same data from PlayReady.dll.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_poKeyFileContext] : Pointer to a key file context.
**              [f_pdstrDevCertStr]   : device cert retrieved from keyfile
**              [f_pdstrModelCertStr] : model cert retrieved from PlayReady.dll
**
** Returns:     DRM_SUCCESS             - on success, the identifiers match
**              DRM_E_INVALIDARG
**              DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH -
**              certificate issuer key does not match public key of
**              a model level certificate in PlayReady.dll
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _CompareXMLCertsModelPublicKeys(
    __in_opt          DRM_VOID            *f_pOEMContext,
    __in        const DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __in              DRM_STRING          *f_pdstrDevCertStr,
    __in              DRM_STRING          *f_pdstrModelCertStr )
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_STRING dstrSigningKey        = EMPTY_DRM_STRING;
    DRM_STRING dstrModelPubKey       = EMPTY_DRM_STRING;
    PUBKEY     oDeviceCertSigningKey = { 0 };
    PUBKEY     oModelCertPubkey      = { 0 };
    DRM_DWORD  cbDestination         = 0;
    DRM_DEVICE_CERT_PARSER_CACHE oParserCache = { 0 };

    ChkDR( DRM_DCP_StartCertParserCache( ( const DRM_CONST_STRING * )f_pdstrDevCertStr,
                                         f_poKeyFileContext,
                                         &oParserCache ) );

    /*
    ** Locate model-level Public key i.e. signing key of the device cert
    */
    ChkDR( DRM_DCP_GetAttribute( NULL,
                                 (DRM_CONST_STRING*)f_pdstrDevCertStr,
                                 DRM_DEVCERT_GROUPCERTPUBKEY,
                                 NULL,
                                 (DRM_CONST_STRING*)&dstrSigningKey ) );
    cbDestination = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_B64_DecodeW( (const DRM_CONST_STRING*)&dstrSigningKey, &cbDestination, oDeviceCertSigningKey.y, 0 );


    /*
    ** Read public of model cert
    */
    ChkDR( DRM_DCP_GetAttribute( NULL,
                                 (DRM_CONST_STRING*)f_pdstrModelCertStr,
                                 DRM_DEVCERT_GROUPCERTPUBKEY,
                                 NULL,
                                 (DRM_CONST_STRING*)&dstrModelPubKey ) );
    cbDestination = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_B64_DecodeW( (const DRM_CONST_STRING*)&dstrModelPubKey, &cbDestination, oModelCertPubkey.y, 0 );

    ChkBOOL( DRM_BYT_CompareBytes(
                oDeviceCertSigningKey.y,
                0,
                oModelCertPubkey.y,
                0,
                DRM_ECC160_PUBLIC_KEY_LEN ) == 0, DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH );

ErrorExit:
    OEM_ECC_ZeroPublicKey_P160( &oDeviceCertSigningKey );
    OEM_ECC_ZeroPublicKey_P160( &oModelCertPubkey );
    return dr;
}

/*****************************************************************************
** Function:    _CompareMachineId
**
** Synopsis:    Compares the machine ID (passed in) with the unique ID of the device.
**
** Arguments:   [f_pOEMContext]  : A pointer to the OEM Context; may be NULL
**              [pdstrMachineId] : A pointer to a DRM String containing a machine ID
**
** Returns:     DRM_SUCCESS             - on success, the identifiers match
**              DRM_E_INVALIDARG        - if the machine ID parameter is invalid
**              DRM_E_MACHINE_ID_MISMATCH - if there is a mismatch
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _CompareMachineId(
    __in_opt       DRM_VOID          *pOEMContext,
    __in     const DRM_CONST_STRING  *pdstrMachineId )
{
    DRM_RESULT       dr                                      = DRM_SUCCESS;
    DRM_CONST_STRING dstrID                                  = EMPTY_DRM_STRING;
    DRM_WCHAR        wchUniqueId[ MAX_WMDRMPD_UNIQUEID_LEN ] = {0};
    DRM_DWORD        cbUniqueId                              = SIZEOF( wchUniqueId );


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICEDEVCERT, PERF_FUNC__CompareMachineId );

    ChkDRMString( pdstrMachineId );

    ChkDR( Oem_Device_GetUniqueId( pOEMContext, DRM_DIT_WMDRMPD_ID, (DRM_BYTE *)wchUniqueId, &cbUniqueId ) );

    dstrID.cchString  = cbUniqueId / SIZEOF( DRM_WCHAR );
    dstrID.pwszString = wchUniqueId;

#if !DRM_TEST_IGNORE_UNIQUE_ID
    ChkBOOL( DRM_UTL_DSTRStringsEqual( pdstrMachineId, &dstrID ), DRM_E_MACHINE_ID_MISMATCH );
#endif

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_DDC_LoadBinaryCertPropertiesCache
**
**  Synopsis:  Loads the binary cert properties cache
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_LoadBinaryCertPropertiesCache(
    __in_opt                                DRM_VOID         *f_pOEMContext,
    __in                                    DRM_BB_CONTEXT   *f_pBlackBoxContext,
    __in_bcount(f_cbBinaryDeviceCert) const DRM_BYTE         *f_pbBinaryDeviceCert,
    __in                                    DRM_DWORD         f_cbBinaryDeviceCert )
{

    DRM_RESULT dr = DRM_SUCCESS;
#if !DRM_TEST_IGNORE_UNIQUE_ID
    DRM_DWORD                cbUniqueID                 = DRM_ID_SIZE;
    DRM_BYTE                 rgbUniqueId[ DRM_ID_SIZE ] = {0};
#endif /* !DRM_TEST_IGNORE_UNIQUE_ID */


    ChkArg( f_pBlackBoxContext != NULL );

    ChkDR( DRM_BCert_LoadPropertiesCache(
                    f_pbBinaryDeviceCert,
                    f_cbBinaryDeviceCert,
                    &f_pBlackBoxContext->cachedBCertValues,
                    DRM_BBX_GetKeyHandleCallbacks( f_pBlackBoxContext ),
                    f_pBlackBoxContext ) );

    /*
    ** Compare the serial number from the device certificate against the unique ID of this device.
    */
#if !DRM_TEST_IGNORE_UNIQUE_ID
    ChkDR( Oem_Device_GetUniqueId( f_pOEMContext, DRM_DIT_PLAYREADY_ID, rgbUniqueId, &cbUniqueID ) );

    ChkBOOL( cbUniqueID == DRM_ID_SIZE, DRM_E_MACHINE_ID_MISMATCH );

    ChkBOOL( MEMCMP( f_pBlackBoxContext->cachedBCertValues.DeviceSerialNumber.rgb,
                     rgbUniqueId,
                     DRM_ID_SIZE ) == 0,
             DRM_E_MACHINE_ID_MISMATCH );
#endif /* !DRM_TEST_IGNORE_UNIQUE_ID */
ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_DDC_LoadXMLCertPropertiesCache
**
**  Synopsis:  Loads the XML cert properties cache
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_LoadXMLCertPropertiesCache(
    __in_opt       DRM_VOID                        *f_pOEMContext,
    __in_opt       DRM_DEVICE_CERT_PARSER_CACHE    *f_poParserCache,
    __in           DRM_BB_CONTEXT                  *f_pBlackBoxContext,
    __in_opt       DRM_KEYFILE_CONTEXT             *f_poKeyFileContext,
    __in     const DRM_CONST_STRING                *f_pdstrXMLDeviceCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;

    ChkBOOL( DRM_WMDRM_IsWMDRMSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pBlackBoxContext != NULL );
    ChkArg( f_pdstrXMLDeviceCert != NULL );

    ChkDR( DRM_DCP_LoadPropertiesCache(
                    f_poParserCache,
                    f_pdstrXMLDeviceCert,
                    f_poKeyFileContext,
                    &f_pBlackBoxContext->CryptoContext,
                    &f_pBlackBoxContext->cachedCertValues ) );

    /*
    ** Compare the serial number from the device certificate against the unique ID of this device.
    */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                 f_pdstrXMLDeviceCert,
                                 DRM_DEVCERT_SERIALNUMBER,
                                 NULL,
                                 &dstrValue ) );

    ChkDR( _CompareMachineId( f_pOEMContext, &dstrValue ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    DRM_DDC_Certificates_Initialize
**
** Synopsis:    Performs all of the tasks necessary to begin using
**              WMDRM and PlayReady device certificate chains.
**
** Arguments:   [f_pContext]    : A pointer to the DRM application context
**              [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the application context pointer is NULL
**              or other return code from a function called within
**
** Notes:       Use rgbDRMLicense buffer to temporarily load XML cert
**              and rgbDRMHeaderData for PlayReady cert.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_Certificates_Initialize(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_VOID        *f_pOEMContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal      = (DRM_APP_CONTEXT_INTERNAL *)f_poAppContext;
    DRM_STRING               dstrXMLDeviceCert          = EMPTY_DRM_STRING;
    DRM_BOOL                 fKeyfileOpened             = FALSE;
    DRM_BOOL                 fCreateNewXMLCert          = FALSE;
    DRM_BOOL                 fClearKeyHistory           = FALSE;
    DRM_BOOL                 fCreateNewBinaryCert       = FALSE;
    DRM_DWORD                cbBinaryDeviceCert         = 0;
    DRM_DEVICE_CERT_PARSER_CACHE oParserCache           = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DEVICEDEVCERT, PERF_FUNC_DRM_DDC_Certificates_Initialize);

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->pKeyFileContext != NULL );

    /*
    ** Verify that robustness version and platform ID inside PlayReady dll match:
    */

    /* size of pbDRMHeaderData or smaller */
    cbBinaryDeviceCert = poAppContextInternal->cbDRMHeaderData - ( poAppContextInternal->cbDRMHeaderData % SIZEOF( DRM_DWORD_PTR ) );

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_ACT_LoadAndVerifyKeyfile(poAppContextInternal->pKeyFileContext) );
        fKeyfileOpened = TRUE;

        dr = DRM_DDC_GetBinaryDeviceCertificate(
                    f_pOEMContext,
                    poAppContextInternal->pKeyFileContext,
                    poAppContextInternal->pbDRMHeaderData,
                    &cbBinaryDeviceCert,
                    DRM_DCP_VERIFY_DEVCERT,
                    &poAppContextInternal->oBlackBoxContext.CryptoContext );

        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_DDC_LoadBinaryCertPropertiesCache( f_pOEMContext,
                                                        &poAppContextInternal->oBlackBoxContext,
                                                        poAppContextInternal->pbDRMHeaderData,
                                                        cbBinaryDeviceCert );
        }
        if( DRM_FAILED( dr ) )
        {
            ChkDR( DRM_E_ACTIVATION_REQUIRED );
        }
    }
    else /* DRM_ACTIVATION_IsActivationSupported( ) */
    {
        ChkDR( Oem_Device_GetCert( f_pOEMContext,
                                   DRM_DCT_PLAYREADY_TEMPLATE,
                                   (DRM_BYTE*)poAppContextInternal->pbDRMHeaderData,
                                   &cbBinaryDeviceCert ) );

        ChkDR( DRM_DDC_MatchRobustnessPlatformAndModel( f_pOEMContext,
                                                        poAppContextInternal->pbDRMHeaderData,
                                                        cbBinaryDeviceCert,
                                                        FALSE ) );

        /*
        ** Load the key file. If the keyfile does not exist yet this call will not fail.
        */
        ChkDR( DRM_KF_Open( poAppContextInternal->pKeyFileContext,
                            TRUE,
                            OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
        fKeyfileOpened = TRUE;

        /*
        ** Load internal keyfile structure. If keyfile does not exist this call will not fail.
        */
        ChkDR( DRM_KF_LoadFromFile( poAppContextInternal->pKeyFileContext, TRUE ) );

        if( DRM_WMDRM_IsWMDRMSupported() )
        {
            /*
            ** First try to load WMDRM-MD certificate
            */
            DSTR_FROM_PB( &dstrXMLDeviceCert,
                          poAppContextInternal->pbDRMLicense,
                          poAppContextInternal->cbDRMLicense );

            dr = DRM_DDC_GetXMLDeviceCertificate( f_pOEMContext,
                                                 poAppContextInternal->pKeyFileContext,
                                                 &dstrXMLDeviceCert,
                                                 DRM_DCP_VERIFY_DEVICE_CERT,
                                                 &poAppContextInternal->oBlackBoxContext.CryptoContext );

            if( DRM_SUCCEEDED( dr ) )
            {
                DRM_RESULT dr1 = DRM_SUCCESS;

                ChkDR( DRM_DCP_StartCertParserCache( ( const DRM_CONST_STRING * )&dstrXMLDeviceCert,
                                                     poAppContextInternal->pKeyFileContext,
                                                     &oParserCache ) );

                dr1 = DRM_DDC_LoadXMLCertPropertiesCache( f_pOEMContext,
                                                         &oParserCache,
                                                         &poAppContextInternal->oBlackBoxContext,
                                                         poAppContextInternal->pKeyFileContext,
                                                         (const DRM_CONST_STRING*)&dstrXMLDeviceCert );

                ChkDR( DRM_DCP_StopCertParserCache( poAppContextInternal->pKeyFileContext,
                                                    &oParserCache ) );

                dr = dr1;
            }

            if( DRM_FAILED( dr ) )
            {
               /* The only error we should get here is if the certificate wasn't found or robustness version mismatch
               ** or if the issuer keys mismatch or if the signature is invalid since our group key just changed,
               ** or our hardware ID mismatches.
               */
               DRMASSERT( dr == DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND
                       || dr == DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH
                       || dr == DRM_E_INVALID_SIGNATURE
                       || dr == DRM_E_MACHINE_ID_MISMATCH
                       || dr == DRM_E_KEYFILE_ROBUSTNESSVERSION_MISMATCH
                       || dr == DRM_E_KEYFILE_PLATFORMID_MISMATCH
                       || dr == DRM_E_DEVCERT_MODEL_MISMATCH );


                if( dr == DRM_E_MACHINE_ID_MISMATCH )
                {
                    /* If the HWID mismatches we need to clear away all the old keys */
                    fClearKeyHistory = TRUE;
                }

                fCreateNewXMLCert = TRUE;
            }
            else
            {
                ChkDR( dr );
            }
        }

        /*
        ** Now try to load PlayReady certificate
        */
        cbBinaryDeviceCert = poAppContextInternal->cbDRMHeaderData - ( poAppContextInternal->cbDRMHeaderData % SIZEOF( DRM_DWORD_PTR ) );
        dr = DRM_DDC_GetBinaryDeviceCertificate(
                    f_pOEMContext,
                    poAppContextInternal->pKeyFileContext,
                    poAppContextInternal->pbDRMHeaderData,
                    &cbBinaryDeviceCert,
                    DRM_DCP_VERIFY_DEVCERT,
                    &poAppContextInternal->oBlackBoxContext.CryptoContext );

        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_DDC_LoadBinaryCertPropertiesCache( f_pOEMContext,
                                                        &poAppContextInternal->oBlackBoxContext,
                                                        poAppContextInternal->pbDRMHeaderData,
                                                        cbBinaryDeviceCert );
        }

        if( DRM_FAILED( dr ) )
        {
           /* The only error we should get here is if the certificate wasn't found or robustness version mismatch
           ** or if the issuer keys mismatch or if the signature is invalid since our group key just changed,
           ** or our hardware ID mismatches.
           */
           DRMASSERT( dr == DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND
                   || dr == DRM_E_KEYFILE_ROBUSTNESSVERSION_MISMATCH
                   || dr == DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH
                   || dr == DRM_E_INVALID_SIGNATURE
                   || dr == DRM_E_MACHINE_ID_MISMATCH
                   || dr == DRM_E_DEVCERT_MODEL_MISMATCH );


            if( dr == DRM_E_MACHINE_ID_MISMATCH )
            {
                /* If the HWID mismatches we need to clear away all the old keys */
                fClearKeyHistory = TRUE;
            }

            fCreateNewBinaryCert = TRUE;
        }
        else
        {
            ChkDR( dr );
        }

        if( fClearKeyHistory )
        {
            /* Clear out any existing keypairs */
            poAppContextInternal->pKeyFileContext->keyfile.KeyHistoryV2.fValid = FALSE;
            poAppContextInternal->pKeyFileContext->keyfile.KeyHistoryV2.pKeyPairHeadv2 = NULL;
        }

        if( DRM_WMDRM_IsWMDRMSupported() )
        {
            if ( fCreateNewXMLCert || fClearKeyHistory )
            {
                ChkDR( DRM_DDC_CreateXMLDeviceCertificateAndKey( f_pOEMContext,
                                                                 poAppContextInternal->pKeyFileContext,
                                                                 &oParserCache,
                                                                 &poAppContextInternal->oBlackBoxContext,
                                                                 &dstrXMLDeviceCert ) );
            }
        }

        if ( fCreateNewBinaryCert || fClearKeyHistory )
        {
            cbBinaryDeviceCert = poAppContextInternal->cbDRMHeaderData - ( poAppContextInternal->cbDRMHeaderData % SIZEOF( DRM_DWORD_PTR ) );

            ChkDR( DRM_DDC_CreateBinaryDeviceCertificateAndKey( f_pOEMContext,
                                                                poAppContextInternal->pKeyFileContext,
                                                                poAppContextInternal->pbDRMHeaderData,
                                                                &cbBinaryDeviceCert,
                                                                &poAppContextInternal->oBlackBoxContext ) );
        }

        if( fCreateNewXMLCert
         || fClearKeyHistory
         || fCreateNewBinaryCert )
        {
            ChkDR( DRM_DDC_CreateSecureStoreKeyAndSaveKeyFile( f_pOEMContext, &poAppContextInternal->oBlackBoxContext ) );
        }
    } /* DRM_ACTIVATION_IsActivationSupported( ) */

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        /*
        ** Verify that certificate feature flags are consistent with runtime support.
        */

        if( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK )
         || ( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_SECURE_CLOCK) ) )
        {
            /*
            ** If any certificate indicates that the device supports secure clock, it is a compliance violation to not
            ** use the PlayReady-defined secure clock service and the certificates should match.  Hence, these asserts.
            */
            AssertChkFeature( DRM_CLK_IsSecureClockSupported() );
            AssertChkFeature( 0 != ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK ) );
            AssertChkFeature( 0 != ( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_SECURE_CLOCK) ) );
        }

        if( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_ANTI_ROLLBACK_CLOCK )
         || ( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK) ) )
        {
            /*
            ** If any certificate indicates that the device supports antirollback clock, it is NOT a compliance violation to
            ** define a device's own mechanism of support.  Hence, no assert for DRM_ARCLK_IsAntirollbackClockSupported by itself.
            ** The certificates should still match.  However, just to make things difficult,
            ** If secure clock is enabled in the certificate and the device supports anti-rollback clock at runtime,
            ** support for anti-rollback clock is implied.  In this case, DRM_DCP_LoadPropertiesCache will set the anti-rollback
            ** clock feature flag for the WMDRM certificate in the cache while the binary certificate equivalent will not.
            ** This was a design decision for the latter in that the binary certificate cache only has what's really in the
            ** certificate, i.e. no implied values.
            ** Thus, checking that the WMDRM certificate matches is simple as the flag will be set in either case
            ** while checking that the binary certificate matches is complex as the flag will not be set in the implied case.
            */
            AssertChkFeature( 0 != ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_ANTI_ROLLBACK_CLOCK ) );

            AssertChkFeature( ( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK) )
                           || ( DRM_ARCLK_IsAntirollbackClockSupported() && ( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_SECURE_CLOCK) ) ) );
        }
    }
    else /* DRM_WMDRM_IsWMDRMSupported() */
    {
        /*
        ** Copy feature flags from the binary certificate cache
        ** to the legacy certificate cache so we can always use
        ** the legacy cache when checking them.
        */

        if( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_SECURE_CLOCK) )
        {
            /*
            ** If the certificate indicates that the device supports secure clock, it is a compliance violation to not
            ** use the PlayReady-defined secure clock service.  Hence, this assert.
            */
            AssertChkFeature( DRM_CLK_IsSecureClockSupported() );
            poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags |= DRM_FEATURE_SECURE_CLOCK;

            if( DRM_ARCLK_IsAntirollbackClockSupported() )
            {
                /*
                ** If secure clock is enabled in the certificate and the device supports anti-rollback clock at runtime,
                ** support for anti-rollback clock is implied.
                ** Setting this feature flag here is required for consistency with DRM_DCP_LoadPropertiesCache.
                */
                poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags |= DRM_FEATURE_ANTI_ROLLBACK_CLOCK;
            }
        }

        if( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK) )
        {
            /*
            ** If the certificate indicates that the device supports antirollback clock, it is NOT a compliance violation to
            ** define a device's own mechanism of support.  Hence, no assert.
            */
            poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags |= DRM_FEATURE_ANTI_ROLLBACK_CLOCK;
        }

        /* For binary certificates, metering support is implied. */
        poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags |= ( DRM_METERING_IsMeteringSupported() ? DRM_FEATURE_METERING : 0 );
    } /* DRM_WMDRM_IsWMDRMSupported() */

    /*
    ** Verify that PRND feature flags are consistent with runtime support.
    */
    if( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_RECEIVER) )
    {
        if( DRM_PRND_IsPRNDReceiverSupported() )
        {
            /* A device's certificate should have the Receiver PRND Encryption Key if the certificate supports it AND the feature is supported. */
            AssertChkFeature( poAppContextInternal->oBlackBoxContext.cachedBCertValues.fDeviceKeyPrndEncryptSet );
        }
        /* A device's certificate can support the feature flag without being linked with real PRND Receiver code.  Do not assert DRM_PRND_IsPRNDReceiverSupported. */
    }
    else
    {
        /* A device's certificate should NOT have the Receiver PRND Encryption Key if it does not support the feature flag. */
        AssertChkFeature( !poAppContextInternal->oBlackBoxContext.cachedBCertValues.fDeviceKeyPrndEncryptSet );

        /* A device should not be linked with real PRND Receiver code unless its certificate supports the feature flag. */
        AssertChkFeature( DRM_PRND_IsPRNDReceiverUnsupported() );
    }
    if( poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_TRANSMITTER) )
    {
        /* A device's certificate can support the feature flag without being linked with real PRND Transmitter code.  Do not assert DRM_PRND_IsPRNDTransmitterSupported. */
        /* No-op. */
    }
    else
    {
        /* A device should not be linked with real PRND Transmitter code unless its certificate supports the feature flag. */
        AssertChkFeature( DRM_PRND_IsPRNDTransmitterUnsupported() );
    }

ErrorExit:
    /*
    ** Close key file
    */
    if ( fKeyfileOpened )
    {
        DRM_KF_Close( poAppContextInternal->pKeyFileContext );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/********************************************************************************
** Function:    DRM_DDC_GetXMLDeviceCertificate
**
** Synopsis:    Reads XML device certificate and verifies its robustness version
**              and platform ID
** Arguments:   [f_pOEMContext]       -- A pointer to the OEM Context; may be NULL
**              [f_poKeyFileContext]  -- keyfile context
**              [f_pdstrWMDRMCertStr] -- A pointer to a DRM String to contain the
**                                       XML device certificate
**              [f_dwFlags]           -- Flags that define function behavior
**                                       (see DRM_DCP_... in drmdevcert.h)
**              [f_pcontextCRYP] -- A pointer to the crypto context
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND - certificate does not exist
**              DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH -
**              certificate issuer key does not match public key of
**              a model level certificate in PlayReady.dll
** Notes:
**
*********************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Crypto contexts can not be const.")
DRM_API DRM_RESULT DRM_CALL DRM_DDC_GetXMLDeviceCertificate(
    __in_opt       DRM_VOID            *f_pOEMContext,
    __in           DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __inout        DRM_STRING          *f_pdstrWMDRMCertStr,
    __in     const DRM_DWORD            f_dwFlags,
    __inout        DRM_CRYPTO_CONTEXT  *f_pcontextCRYP )
PREFAST_POP
{
    DRM_RESULT       dr                       = DRM_SUCCESS;
    DRM_BYTE         *pXMLModelCert           = NULL;
    DRM_DWORD        cbXMLModelCert           = 0;
    DRM_BYTE         *pXMLDevCert              = NULL;
    DRM_DWORD        cbXMLDevCert             = 0;
    DRM_STRING       dstrXMLDeviceCert        = EMPTY_DRM_STRING;
    DRM_DWORD        cchInputBuffer           = 0;

    /* TASK: perf profiling */

    ChkBOOL( DRM_WMDRM_IsWMDRMSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pcontextCRYP != NULL );
    ChkDRMString( f_pdstrWMDRMCertStr );

    cbXMLDevCert = 0;
    pXMLDevCert  = NULL;
    cchInputBuffer = f_pdstrWMDRMCertStr->cchString;

    /*
    ** First try to get device certificate from keyfile
    */
    dr = DRM_KF_GetCertificate (
            f_poKeyFileContext,
            eKF_CERT_TYPE_WMDRM,
            &pXMLDevCert,
            &cbXMLDevCert );

    if ( DRM_SUCCEEDED( dr ) )
    {
        DSTR_FROM_PB( &dstrXMLDeviceCert, pXMLDevCert, cbXMLDevCert );

#if !DRM_SUPPORT_KEYFILE
        if( DRM_SYMOPT_IsSymOptSupported() )
        {
            ChkDR( DRM_SYMOPT_DDC_VerifySymmetricSignature( f_pOEMContext, (DRM_CONST_STRING*) &dstrXMLDeviceCert ) );
        }
        else
        {
            ChkDR( DRM_DCP_VerifyDeviceCert( NULL,
                                             (DRM_CONST_STRING*)&dstrXMLDeviceCert,
                                             f_dwFlags,
                                             f_poKeyFileContext,
                                             f_pcontextCRYP ) );
        }
#endif  /* !DRM_SUPPORT_KEYFILE */

        /*
        ** Match robustness version, platform and model of the XML cert
        */
        ChkDR( _MatchXMLCertRobustnessPlatformAndModel( f_pOEMContext,
                                                        (DRM_CONST_STRING*) &dstrXMLDeviceCert,
                                                        TRUE ) );
        /*
        ** Match public key of model cert in PlayReady.dll with the signing key of device cert.
        ** Use f_pdstrWMDRMCertStr string buffer to store model cert, then return device cert in it.
        */
        pXMLModelCert = PB_DSTR( f_pdstrWMDRMCertStr );
        cbXMLModelCert = CB_DSTR( f_pdstrWMDRMCertStr );

        ChkDR( Oem_Device_GetCert(
                    f_pOEMContext,
                    DRM_DCT_WMDRMPD_TEMPLATE,
                    pXMLModelCert,
                    &cbXMLModelCert ) );

        ChkDR( _CompareXMLCertsModelPublicKeys( f_pOEMContext,
                                                f_poKeyFileContext,
                                                &dstrXMLDeviceCert,
                                                f_pdstrWMDRMCertStr ) );


        ChkBOOL( dstrXMLDeviceCert.cchString <= cchInputBuffer, DRM_E_BUFFERTOOSMALL );
        MEMCPY( f_pdstrWMDRMCertStr->pwszString, dstrXMLDeviceCert.pwszString, dstrXMLDeviceCert.cchString * SIZEOF(DRM_WCHAR));
        f_pdstrWMDRMCertStr->cchString = dstrXMLDeviceCert.cchString;
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    DRM_DDC_GetBinaryDeviceCertificate
**
** Synopsis:    Gets the Device's binary certificate chain, creating the leaf node if necessary.
**
** Arguments:   [f_pOEMContext]           : A pointer to the OEM Context
**              [f_dwFlags]               : Behavioral flags
**              [f_pBinaryDevCertChain]   : A pointer to a bcert chain buffer to fill in
**              [f_pcbBinaryDevCertChain] : The size of the bcert chain buffer; on input
**                                          should be enough to fill both device and model certs in it.
**                                          On return will indicate the size on device cert chain.
**              [f_dwFlags]               : DRM_DCP_VERIFY_DEVCERT - verify signature
**              [f_pCryptoCtx]            : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS                            - on success
**              DRM_E_INVALIDARG                       - if any required pointer parameter is NULL
**              or other return code from a function called within
**              DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH -
**              certificate issuer key does not match public key of
**              a model level certificate in PlayReady.dll**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_GetBinaryDeviceCertificate(
    __in_opt                               DRM_VOID            *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE            *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD           *f_pcbBinaryDevCertChain,
    __in                                   const DRM_DWORD      f_dwFlags,
    __inout                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
)
{
    DRM_RESULT               dr                  = DRM_SUCCESS;
    DRMFILETIME              ftUnlimited         = {0};
    DRM_DWORD                rgdwKeyUsages[]     = { DRM_BCERT_KEYUSAGE_SIGN, DRM_BCERT_KEYUSAGE_ENCRYPT_KEY };
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_BYTE                 *pbBinaryDeviceCert = NULL;
    DRM_DWORD                cbBinaryDeviceCert  = 0;
    DRM_BYTE                 *pbBinaryModelCert  = NULL;
    DRM_DWORD                cbBinaryModelCert   = 0;


    ChkArg( f_pcbBinaryDevCertChain != NULL );
    ChkArg( f_pCryptoCtx            != NULL );

    /*
    ** First, try to get an existing device certificate
    */
    ChkDR( DRM_KF_GetCertificate ( f_poKeyFileContext,
                                 eKF_CERT_TYPE_PLAYREADY,
                                 &pbBinaryDeviceCert,
                                 &cbBinaryDeviceCert ) );

    /*
    ** Can return dev cert size right away if that's what we wanted.
    */
    if ( f_pBinaryDevCertChain == NULL )
    {
        *f_pcbBinaryDevCertChain = cbBinaryDeviceCert;
        ChkDR( DRM_E_BUFFERTOOSMALL ); /* the caller code still expects this error */
    }
    else
    {
        if( *f_pcbBinaryDevCertChain < cbBinaryDeviceCert )
        {
            *f_pcbBinaryDevCertChain = cbBinaryDeviceCert;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
    }

    /*
    ** Initialize verification context: full verification.
    ** We only accept certs that don't expire (i.e. expire at 0xFFFFFFFF).
    ** Device cert key must have Sign and Encrypt key usages
    */
    CREATE_FILE_TIME( MAX_UNSIGNED_TYPE( DRM_DWORD ), ftUnlimited );

    ChkDR( DRM_BCert_InitVerificationContext( &ftUnlimited,
                                              (const PUBKEY_P256*) g_rgbMSPlayReadyRootIssuerPubKey,
                                              DRM_BCERT_CERTTYPE_DEVICE,
                                              f_pCryptoCtx,
                                              0 != (f_dwFlags & DRM_DCP_VERIFY_DEVCERT),
                                              FALSE,
                                              rgdwKeyUsages,
                                              NO_OF(rgdwKeyUsages),
                                              FALSE,
                                              NULL, /* Cert caching parameters NULL as we generally don't verify signatures on device certs so optimization not needed here */
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    if( DRM_SUCCEEDED( dr ) )
    {
        ChkDR( DRM_BCert_ParseCertificateChain( pbBinaryDeviceCert,
                                                cbBinaryDeviceCert,
                                                &oVerificationContext ) );

        /*
        ** See if robustness version and platform ID of the cert
        ** matches same data in PlayReady.dll
        */
        ChkDR( DRM_DDC_MatchRobustnessPlatformAndModel( f_pOEMContext,
                                                        pbBinaryDeviceCert,
                                                        cbBinaryDeviceCert,
                                                        TRUE ) );

        /*
        ** Match public key of model cert in PlayReady.dll with the signing key of device cert.
        ** This is not required if the entire cert chain is in the keyfile (activation case).
        */
        if( !DRM_ACTIVATION_IsActivationSupported() )
        {
            /*
            ** Note: retrieving model level cert into f_pBinaryDevCertChain buffer temporarily.
            */
            pbBinaryModelCert = f_pBinaryDevCertChain;
            cbBinaryModelCert = *f_pcbBinaryDevCertChain;

            ChkDR( Oem_Device_GetCert(
                    f_pOEMContext,
                    DRM_DCT_PLAYREADY_TEMPLATE,
                    pbBinaryModelCert,
                    &cbBinaryModelCert ) );

            ChkDR( _CompareBinaryCertsModelPublicKeys(
                    f_pOEMContext,
                    pbBinaryDeviceCert,
                    cbBinaryDeviceCert,
                    pbBinaryModelCert,
                    cbBinaryModelCert ) );
        }

        /*
        ** Then return the pointer to device binary cert in f_pBinaryDevCertChain.
        */
        MEMCPY( f_pBinaryDevCertChain, pbBinaryDeviceCert, cbBinaryDeviceCert );
        *f_pcbBinaryDevCertChain = cbBinaryDeviceCert;
    }

 ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

