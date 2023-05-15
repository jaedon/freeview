/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
 
#include <drmcommon.h>
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
#include <drmutf.h>
#include <drmxmlrsakeys.h>
#include <drmndcertbuilder.h>
#include <drmdevcertconstants.h>
#include <drmndcertconstants.h>

typedef struct DRM_PLAYREADYND_DATA
{
    DRM_RSA_PRIVATE_KEY  oCompanyRSAPrivKey; 
    DRM_ANSI_STRING      dastrCompanyPubKeyModulusB64; /* contains pointer into company cert data, no need to be free */ 
    DRM_ANSI_STRING      dastrUnsignedTemplate;
    DRM_ANSI_STRING      dastrCompanyCert;
    DRM_BOOL             fTemplateSecurityLevelIsSpecified;          
    DRM_BOOL             fTemplateKeyUsageIsSpecified;    
    OEM_FILEHDL          hModelCert;           /* handle to model certificate */
    OEM_FILEHDL          hModelCertKey;        /* handle to model key file */
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength ; /* Key length of the certificate */
    
} DRM_PLAYREADYND_DATA;

typedef struct DRMSTATE
{  
    DRM_STRING          dstrDACResponse;        /* content of DACResponseFile */
    DRM_STRING          dstrAuthCert;
    DRM_STRING          dstrAuthRootCert;
    DRM_STRING          dstrUnsignedTemplate;   /* template data to put into group certificate, contains security level, features, etc. */
    DRM_STRING          dstrOEMPrivKeys;        /* content of OEMPrivateKeyFile, now also includes ECC-256 key */
    OEM_FILEHDL         hDevcertTemplate;      /* dev cert template file handle */
    OEM_FILEHDL         hDeviceKeys;           /* handle to device keys file */
    PUBKEY              pubkeyGC;
    PRIVKEY             privkeyGC;
    PRIVKEY             privkeyDAC;
    PRIVKEY             privkeyFallback;
    DRM_STRING          dstrGCPubKey;
    DRM_STRING          dstrGCPrivKey;
    DRM_STRING          dstrFalbackPrivKey;
    DRM_STRING          dstrSecurityVersion;
    _XMBContext         *pbTemplate;
    DRM_CRYPTO_CONTEXT  *pcontextCrypto;
    /*
    ** Additional information to build binary certificates
    */
    DRM_BYTE             *pbBinaryCertData;       /* binary presentation of a parent certificate */
    DRM_DWORD             cbBinaryCertDataLength;  /* size in bytes of pbBinaryCertData */
    OEM_FILEHDL           fpBinaryCertFile;        /* handle to a resultant binary cert file */
    PRIVKEY_P256          privKeyGC_P256;          /* GC private key written into fpDeviceKeys_P256 */
    PUBKEY_P256           pubKeyGC_P256;           /* GC public key written into a binary cert */
    OEM_FILEHDL           fpDeviceKeys_P256;       /* handle to ECC-256 priv keys file, analogue to fpDeviceKeys */
    PRIVKEY_P256          privKeyDAC_P256;         /* key to sign a certificate, parsed out from dstrOEMPrivKeys */

    DRM_PLAYREADYND_DATA *pPlayReadyNDData; /* data structure for RSA-based certificate */

}DRMSTATE;


#define XML_RSA_KEY_SIZE           5*1024
#define SIGNED_TEMPLATE_EXTRA_SIZE 2048
#define PRIVKEY_FILE_SIZE          2048
#define OBFBYTESLEN                4096
#define SECURITYLEVEL_2000         2000
#define PR_SETBITS                 0x00010000
#define PD_SETBITS                 0x00020000
#define ND_SETBITS                 0x00030000

DRMSTATE g_oState = { 0 };        

static DRM_CRYPTO_CONTEXT g_CryptoCtx = { 0 }; 

/*
** Paths to features in UnsignedTemplate file
*/

/* FEATURES/CLOCK */
const DRM_WCHAR  g_rgwchCLOCK[] =  {
    ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'L', '\0' ), ONE_WCHAR( 'O', '\0' ),
    ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/* FEATURES/SUPPORT_REVOCATION */
const DRM_WCHAR g_rgwchSUPPORT_REVOCATION[] = {
    ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'P', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), 
    ONE_WCHAR( '_', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'V', '\0' ), 
    ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/* FEATURES/TRANSMITTER */
const DRM_WCHAR g_rgwchTRANSMITTER[] = {
    ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'A', '\0' ), 
    ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'R', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
   
/* FEATURES/RECEIVER */
const DRM_WCHAR g_rgwchRECEIVER[] = {
    ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'C', '\0' ),
    ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'E', '\0' ),
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/*
** -z parameter values
*/

/* PR */
const DRM_WCHAR g_rgwchPR[] = {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/* PD */
const DRM_WCHAR g_rgwchPD[] = {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/* ND */
const DRM_WCHAR g_rgwchND[] = {
    ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/*
** help parameter values
*/

/* legacy */
const DRM_WCHAR g_rgwchLEGACY[] = {
    ONE_WCHAR( 'L', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'G', '\0' ), ONE_WCHAR( 'A', '\0' ),
    ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '\0', '\0' )
};

/*
** In this array index matches a feature value as it is defined in binary cert specification,
** strings are XPaths to features nodes
*/
const DRM_CONST_STRING g_rdstrFeatureXPaths[ DRM_BCERT_MAX_FEATURES + 1 ] = {
        EMPTY_DRM_STRING,                              /* 0-th index, has no meaning */
        CREATE_DRM_STRING( g_rgwchTRANSMITTER),        /* DRM_BCERT_FEATURE_TRANSMITTER, do not support in v1 */
        CREATE_DRM_STRING( g_rgwchRECEIVER),           /* DRM_BCERT_FEATURE_RECEIVER, do not support in v1 */
        EMPTY_DRM_STRING,                              /* DRM_BCERT_FEATURE_SHARED_CERTIFICATE, do not support in v1 */
        EMPTY_DRM_STRING,                              /* DRM_BCERT_FEATURE_SECURE_CLOCK - in unsigned template it's <CLOCK> with 1 for rollback and 2 for secure clock */
        CREATE_DRM_STRING( g_rgwchCLOCK ),             /* DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK */
        EMPTY_DRM_STRING,                              /* DRM_BCERT_FEATURE_METERING  - obsolete */
        EMPTY_DRM_STRING,                              /* DRM_BCERT_FEATURE_LICENSE_SYNC  - obsolete */
        EMPTY_DRM_STRING,                              /* DRM_BCERT_FEATURE_SYMMETRIC_OPTIMIZATION - obsolete */
        CREATE_DRM_STRING( g_rgwchSUPPORT_REVOCATION ) /* DRM_BCERT_FEATURE_SUPPORTS_CRLS */
};

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintLegacyUsage( 
    __in const DRM_WCHAR *pwszAppName )
{
    printf( "Syntax: \t%S [-?:legacy]\n\
-b:PR-DACResponse           - Path to the input file with a binary company certificate\n\
-u:UnsignedTemplateFile     - Path to the input file with parameters for the new certificate, should be XML file saved as unicode.\n\
-f:OEMPrivateKeysFile       - Path to the input file with private keys used to sign new certificates. Usually it is created by generatecompanycertrequest.exe\n\
[-g:PR-ModelCert]           - Optional parameter. Path to the output file that receives a binary OEM certificate\n\
[-h:PR-ModelCertKey]        - Optional parameter. Path to the output file with PlayReady private key of a new binary OEM certificate\n\
[-d:PD-DACResponse]         - Optional parameter. Path to the input file with a WMDRM-PD company certificate\n\
[-t:PD-DevCertTemplate]     - Optional parameter. Path to the output file that receives a WMDRM-PD OEM certificate.\n\
[-k:PD-DevCertTemplateKeys] - Optional parameter. Path to the output file with a private key of a new WMDRM-PD OEM certificate.\n\
[-c:PR-NDCompanyCert]       - Optional parameter. Path to the input file with PlayReady-ND company certificate\n\
[-m:PR-NDModelCert]         - Optional parameter. Path to the ouput file that receives PlayReady-ND model certificate\n\
[-n:PR-NDModelKey]          - Optional parameter. Path to the ouput file that receives PlayReady-ND model private key\n\
[-v:PR-NDUnsignedTemplate]  - Optional parameter. Path to the input file with parameters for the new certificate, should be XML file saved as ASCII\n",
            pwszAppName );
}

static void PrintUsage( 
    __in const DRM_WCHAR *pwszAppName )
{
    printf( "Syntax: \t%S [-?]\n\
-z:CertType             - PR/PD/ND\n\
-b:CompanyCert          - Path to the input file of the company certificate or DACResponse file\n\
-f:CompanyCertPrivKeys  - Path to the input file with private keys used to sign new certificates. This is the file generated by generatecompanycertrequest.exe when you created the request file for a company certificate\n\
-u:UnsignedTemplateFile - Path to the input file with parameters for the new  certificate, should be XML file saved as unicode (PlayReady or WMDRM-PD) or an XML file saved as ASCII (ND)\n\
-g:ModelCert            - Path to the output file that receives the model certificate (PlayReady) or group certificate (WMDRM-PD)\n\
-h:ModelCertPrivKey     - Path to the output file for the private keys of the model certificate (PlayReady) or group certificate (WMDRM-PD)\n\n\
For legacy support help type generatemodelcert.exe [-?:legacy]",
            pwszAppName );
}

/******************************************************************************
** 
** Function :   DRMInit
** 
** Synopsis :   Initialize the internal context data structure
** 
** Arguments :  None
** 
** Returns :    
** 
** Notes :      
** 
******************************************************************************/
DRM_RESULT DRMInit( void )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkMem( g_oState.pcontextCrypto = 
            (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    MEMSET( g_oState.pcontextCrypto, 0, SIZEOF( DRM_CRYPTO_CONTEXT ) );
    ChkMem ( g_oState.pPlayReadyNDData = (DRM_PLAYREADYND_DATA *) Oem_MemAlloc(SIZEOF(DRM_PLAYREADYND_DATA) ) );
    ZEROMEM( g_oState.pPlayReadyNDData, SIZEOF(DRM_PLAYREADYND_DATA) );    
    g_oState.pPlayReadyNDData->hModelCert = OEM_INVALID_HANDLE_VALUE;
    g_oState.pPlayReadyNDData->hModelCertKey = OEM_INVALID_HANDLE_VALUE;
    
    g_oState.hDevcertTemplate       = OEM_INVALID_HANDLE_VALUE;
    g_oState.hDeviceKeys            = OEM_INVALID_HANDLE_VALUE;   
    g_oState.pbBinaryCertData        = NULL;
    g_oState.fpBinaryCertFile        = OEM_INVALID_HANDLE_VALUE;
    g_oState.fpDeviceKeys_P256       = OEM_INVALID_HANDLE_VALUE;   
    
ErrorExit:
    return dr;
}


/******************************************************************************
** 
** Function :   _DRMClose
** 
** Synopsis :   Shutdown the internal context data structure, free allocated
**              buffers and file pointers
** 
** Arguments :  None
** 
** Returns :    
** 
** Notes :      
** 
******************************************************************************/
static void _DRMClose( void )
{

    if ( g_oState.pPlayReadyNDData != NULL )
    {
        if (g_oState.pPlayReadyNDData->hModelCertKey != OEM_INVALID_HANDLE_VALUE)
        {
            Oem_File_Close(g_oState.pPlayReadyNDData->hModelCertKey);
        }
        if (g_oState.pPlayReadyNDData->hModelCert != OEM_INVALID_HANDLE_VALUE)
        {
            Oem_File_Close(g_oState.pPlayReadyNDData->hModelCert);
        }
        g_oState.pPlayReadyNDData->dastrCompanyPubKeyModulusB64.pszString = NULL;
        SAFE_OEM_FREE( g_oState.pPlayReadyNDData->dastrCompanyCert.pszString );
        SAFE_OEM_FREE( g_oState.pPlayReadyNDData->dastrUnsignedTemplate.pszString );
        SAFE_OEM_FREE( g_oState.pPlayReadyNDData );
    }

    SAFE_OEM_FREE( g_oState.dstrDACResponse.pwszString );
    SAFE_OEM_FREE( g_oState.dstrUnsignedTemplate.pwszString );
    SAFE_OEM_FREE( g_oState.dstrOEMPrivKeys.pwszString );

    if (g_oState.hDevcertTemplate != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close(g_oState.hDevcertTemplate);
    }
    if (g_oState.fpBinaryCertFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( g_oState.fpBinaryCertFile );
    }
    if (g_oState.fpDeviceKeys_P256 != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( g_oState.fpDeviceKeys_P256 );
    }
    if (g_oState.hDeviceKeys != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( g_oState.hDeviceKeys );
    }    
     
    SAFE_OEM_FREE( g_oState.pbTemplate );
    SAFE_OEM_FREE( g_oState.pcontextCrypto );
    SAFE_OEM_FREE( g_oState.pbBinaryCertData );
}

/******************************************************************************
** 
** Function :   _ParseDACResponse
** 
** Synopsis :   
** 
** Arguments :  
** 
** Returns :    
** 
** Notes :      
** 
******************************************************************************/
DRM_RESULT _ParseDACResponse()
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    ChkDR(DRM_XML_GetSubNode((DRM_CONST_STRING *)&(g_oState.dstrDACResponse), 
                               &g_dstrTagCertificate, 
                               &g_dstrAttributeType,
                               &g_dstrCerttypeAuth, 
                               0, 
                               (DRM_CONST_STRING *)&(g_oState.dstrAuthCert), 
                               NULL,
                               1));

    ChkDR(DRM_XML_GetSubNode((DRM_CONST_STRING *)&(g_oState.dstrDACResponse), 
                               &g_dstrTagCertificate, 
                               &g_dstrAttributeType,
                               &g_dstrCerttypeAuthRoot, 
                               0, 
                               (DRM_CONST_STRING *)&(g_oState.dstrAuthRootCert), 
                               NULL,
                               1));

    ChkDR(DRM_XML_GetSubNodeByPath((DRM_CONST_STRING *)&(g_oState.dstrDACResponse), 
                                     &g_dstrXPathFallbackSecVer, 
                                     NULL,
                                     NULL, 
                                     NULL,
                                     (DRM_CONST_STRING *)&(g_oState.dstrSecurityVersion),
                                     g_wchForwardSlash));    
ErrorExit:
    return dr;
}

static DRM_RESULT _GetRootKeys( 
    __in                      const DRM_CONST_STRING *f_pdstrRootKeys, 
    __in                      const DRM_CONST_STRING *f_pdstrXMLPath,
    __inout                         DRM_CONST_STRING *f_pdstrKey )
{
    DRM_RESULT       dr           = DRM_SUCCESS;

    DRMASSERT( f_pdstrRootKeys != NULL );
    DRMASSERT( f_pdstrXMLPath != NULL );
    DRMASSERT( f_pdstrKey != NULL );

    ChkDR( DRM_XML_GetSubNodeByPath( f_pdstrRootKeys,
                                     f_pdstrXMLPath,
                                     NULL,
                                     NULL,
                                     NULL,
                                     f_pdstrKey,
                                     g_wchForwardSlash ) );
ErrorExit:
    return dr;
}

/******************************************************************************
** 
** Function :   _ObtainOEMKeys
** 
** Synopsis :   Parses the OEM key file to obtain the necessary private keys
** 
** Arguments :  
**      [f_fWMDRMPDCertRequired] - TRUE if tool is building WMDRMPD cert
**      [f_NDCertRequired]       - TRUE if tool is building ND cert
**      [f_PlayReadyCertRequired]       - TRUE if tool is building PlayReady cert
** 
** Returns :    
** 
** Notes :   
**  
**  <OEMPrivateKeys>
**      <DAC><PRIVKEY>base64 encoded DAC private key</PRIVKEY></DAC>
**      <FALLBACK>
**          <PRIVKEY>base64 encoded FALLBACKVERIFICATION private key</PRIVKEY>
**      </FALLBACK>
**      <PlayReady>
**          <PRIVKEY>base64 encoded ECC-256 PlayReady private key</PRIVKEY>
**      </PlayReady>
**      <PRIVKEY>
**          <Prime0>v3n8v...XFf/yg==</Prime0> 
**          <Prime1>Rz55W...pWKSwg==</Prime1> 
**          <CRTExponent0>b7jMrA...yR6E1wQ==</CRTExponent0> 
**          <CRTExponent1>G37B20...6CeFnGw==</CRTExponent1> 
**          <IQMP>VOrUow...8ytSebVKEY/AKHQ4=</IQMP> 
**      </PRIVKEY>
**  </OEMPrivateKeys>
**
** 
******************************************************************************/
static DRM_RESULT _ObtainOEMKeys( 
   __in DRM_BOOL f_fWMDRMPDCertRequired,
   __in DRM_BOOL f_NDCertRequired,
   __in DRM_BOOL f_PlayReadyCertRequired)
{
    DRM_STRING dstrPrivKey = EMPTY_DRM_STRING;
    DRM_RESULT dr          = DRM_SUCCESS;    
    DRM_DWORD  cbSize      = 0;

    if ( f_fWMDRMPDCertRequired )
    {
        /*
        **  Parse XML to get the DAC private key
        */
        ChkDR(DRM_XML_GetSubNodeByPath((DRM_CONST_STRING *)&(g_oState.dstrOEMPrivKeys),
                                         &g_dstrXPathDACPrivKey,
                                         NULL,
                                         NULL,
                                         NULL,
                                         (DRM_CONST_STRING *)&dstrPrivKey,
                                         g_wchForwardSlash));
        
        /*
        **  Decode this to get the private key
        */    
        cbSize = DRM_ECC160_PRIVATE_KEY_LEN;
        ChkDR(DRM_B64_DecodeW((DRM_CONST_STRING *)&dstrPrivKey, 
                               &cbSize, 
                               (DRM_BYTE*)&(g_oState.privkeyDAC), 
                               0));

        /*
        **  Parse XML to get the Fallback private key
        */
        ChkDR(DRM_XML_GetSubNodeByPath((DRM_CONST_STRING *)&(g_oState.dstrOEMPrivKeys),
                                         &g_dstrXPathFallbackPrivKey,
                                         NULL,
                                         NULL,
                                         NULL,
                                         (DRM_CONST_STRING *)&(g_oState.dstrFalbackPrivKey),
                                         g_wchForwardSlash)); 

        /*
        **  Decode this to get the private key
        */    
        cbSize = DRM_ECC160_PRIVATE_KEY_LEN;
        ChkDR(DRM_B64_DecodeW((DRM_CONST_STRING *)&(g_oState.dstrFalbackPrivKey),
                               &cbSize, 
                               (DRM_BYTE*)&(g_oState.privkeyFallback), 
                               0));   
    }
    
    /*
    ** Parse PlayReady key string
    */
    if( f_PlayReadyCertRequired )
    {
        dstrPrivKey.pwszString = NULL;
        dstrPrivKey.cchString = 0;
        
        ChkDR(DRM_XML_GetSubNodeByPath((DRM_CONST_STRING *)&(g_oState.dstrOEMPrivKeys),
                                         &g_dstrXPathWMDRMPrivKey,
                                         NULL,
                                         NULL,
                                         NULL,
                                         (DRM_CONST_STRING *)&dstrPrivKey,
                                         g_wchForwardSlash)); 

        

        cbSize = ECC_P256_INTEGER_SIZE_IN_BYTES;
        ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *)&dstrPrivKey, 
                                &cbSize, 
                                (DRM_BYTE*)&( g_oState.privKeyDAC_P256 ), 
                                0 ) );
    }

    if ( f_NDCertRequired )
    {
        /* Temp data for RSA public/private key parsing */
        DRM_CONST_STRING dstrPrivKeyPrime0       = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPrivKeyPrime1       = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPrivKeyCRTExponent0 = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPrivKeyCRTExponent1 = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPrivKeyIQMP         = EMPTY_DRM_STRING;
        DRM_CONST_STRING *pdstrKeys = (DRM_CONST_STRING *)&g_oState.dstrOEMPrivKeys;
        
        ChkDR( _GetRootKeys( pdstrKeys, &g_dstrPlayReadyNDPRIVKEYPrime0,       &dstrPrivKeyPrime0 ) );
        ChkDR( _GetRootKeys( pdstrKeys, &g_dstrPlayReadyNDPRIVKEYPrime1,       &dstrPrivKeyPrime1 ) );
        ChkDR( _GetRootKeys( pdstrKeys, &g_dstrPlayReadyNDPRIVKEYCRTExponent0, &dstrPrivKeyCRTExponent0 ) );
        ChkDR( _GetRootKeys( pdstrKeys, &g_dstrPlayReadyNDPRIVKEYCRTExponent1, &dstrPrivKeyCRTExponent1 ) );
        ChkDR( _GetRootKeys( pdstrKeys, &g_dstrPlayReadyNDPRIVKEYIQMP,         &dstrPrivKeyIQMP ) );

        /*
        **  Parse XML to get private key
        */
        ChkDR( DRM_XML_RSA_ParseBase64PrivateKey( &dstrPrivKeyPrime0, 
                                                  &dstrPrivKeyPrime1,
                                                  &dstrPrivKeyCRTExponent0,
                                                  &dstrPrivKeyCRTExponent1,
                                                  &dstrPrivKeyIQMP,
                                                  &(g_oState.pPlayReadyNDData->oCompanyRSAPrivKey) ) );
    }

ErrorExit:
   return dr;
}

/******************************************************************************
** 
** Function :   VerifyDevcert
** 
** Synopsis :   Verify the signatures on the devcerts
** 
** Arguments :      --
**              f_pdstrDevcertTemplate  --
** 
** Returns :    
** 
******************************************************************************/
DRM_RESULT VerifyDevcert( 
    IN DRM_STRING *f_pdstrDevcertTemplate )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    
    ChkDR( DRM_DCP_VerifyDeviceCert( NULL,
                                     (DRM_CONST_STRING *)f_pdstrDevcertTemplate,
                                     DRM_DCP_VERIFY_GROUP_CERT,
                                     NULL,
                                     g_oState.pcontextCrypto ) );    
    
ErrorExit:
   return dr;
}


/******************************************************************************
** 
** Function :   _parseDataFromUnsignedTemplate
** 
** Synopsis :   Reads some data for building a binary certificate from a template file.
** 
** Arguments : f_poBCertBuilderData - pointer to data structure for a binary cert builder
** 
** Returns :    
** 
** Notes :  Building XML certificate does not require any template parsing 
**          because a template is just embedded into a cert.
******************************************************************************/
DRM_RESULT _parseDataFromUnsignedTemplate(
    DRM_BCERT_BUILDER_DATA  *f_poBCertBuilderData
    )
{
    DRM_RESULT        dr        = DRM_SUCCESS;
    DRM_STRING        dstrData  = EMPTY_DRM_STRING;
    DRM_DWORD         dwCount   = 0;

    const DRM_CONST_STRING* g_rpdstrTagNames[2] = { 
                                                  &g_dstrTagName,       /* <NAME> */
                                                  &g_dstrModel };       /* <MODEL> */                                                  
    DRM_BCERT_MANUFACTURER_STRING* g_rgpManufacturerData[2] = { 0 };

    ChkArg( f_poBCertBuilderData != NULL );

    g_rgpManufacturerData[0] = &f_poBCertBuilderData->pManufacturerStrings->ModelName;
    g_rgpManufacturerData[1] = &f_poBCertBuilderData->pManufacturerStrings->ModelNumber;

    /*
    ** read manufacturer information
    */
    for ( dwCount = 0; dwCount < 2; dwCount++ )
    {
        dr = DRM_XML_GetSubNodeByPath( 
                                (DRM_CONST_STRING *)&(g_oState.dstrUnsignedTemplate), 
                                g_rpdstrTagNames[ dwCount ], 
                                NULL,
                                NULL,
                                NULL,
                                (DRM_CONST_STRING *)&dstrData,
                                g_wchForwardSlash );
        if ( DRM_FAILED( dr ) )
        {
            printf( "%S XML tag is required in unsigned template file, exitting...\n", g_rpdstrTagNames[ dwCount ]->pwszString );
            ChkDR( dr );
        }

        ZEROMEM( g_rgpManufacturerData[ dwCount ]->rgb, DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH );

        /* Convert dstrData to UTF8 */ 
        g_rgpManufacturerData[ dwCount ]->cb = DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH;
        dr = DRM_STR_DSTRtoUTF8( &dstrData, (DRM_CHAR*)g_rgpManufacturerData[ dwCount ]->rgb, &g_rgpManufacturerData[ dwCount ]->cb );
        if ( DRM_FAILED( dr ) )
        {
            printf( "Failure to convert %S data from unsigned template file to UTF8, exitting...\n", g_rpdstrTagNames[ dwCount ]->pwszString );
            ChkDR( dr );
        }
        
        ChkBOOL( g_rgpManufacturerData[ dwCount ]->cb < DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, DRM_E_BCERT_MANUFACTURER_STRING_TOO_LONG );
        g_rgpManufacturerData[ dwCount ]->cb++;
        g_rgpManufacturerData[ dwCount ]->rgb[g_rgpManufacturerData[ dwCount ]->cb - 1] = '\0';
    }

    /*
    ** read security level
    */
    dr = DRM_XML_GetSubNodeByPath( 
                            ( DRM_CONST_STRING * )&( g_oState.dstrUnsignedTemplate ), 
                            &g_dstrTagSecurityLevel, 
                            NULL,
                            NULL,
                            NULL,
                            (DRM_CONST_STRING *)&dstrData,
                            g_wchForwardSlash  );
    if ( DRM_FAILED( dr ) )
    {
        printf( "%S XML tag is required in unsigned template file, exitting...\n", g_dstrTagSecurityLevel.pwszString );
        ChkDR( dr );
    }
    
    dr = DRMCRT_wcsntol( dstrData.pwszString,
                           dstrData.cchString,
                           ( DRM_LONG* )&f_poBCertBuilderData->dwSecurityLevel );
    if ( DRM_FAILED( dr )
        || f_poBCertBuilderData->dwSecurityLevel > DRM_BCERT_SECURITYLEVEL_2000
        || f_poBCertBuilderData->dwSecurityLevel == 0 ) /* 0 likely means there were no digits. Also we do not allow zero secutiry level */
    {
        printf("Invalid %S value in unsigned template file, exitting...\n", g_dstrTagSecurityLevel.pwszString );
        if ( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_INVALIDARG;
        }
        ChkDR( dr );
    }

    /*
    ** read security version information
    */
    f_poBCertBuilderData->SecurityVersion.fValid = FALSE;
    dr = DRM_XML_GetSubNodeByPath( 
             ( DRM_CONST_STRING * )&( g_oState.dstrUnsignedTemplate ), 
             &g_dstrTagDataSecVer, 
             NULL,
             NULL,
             (DRM_CONST_STRING *)&dstrData,
             NULL,
             g_wchForwardSlash  );

    if ( DRM_SUCCEEDED( dr ) )
    {
        DRM_STRING dstrSubData = EMPTY_DRM_STRING;

        dr = DRM_XML_GetSubNodeByPath( 
                 ( DRM_CONST_STRING * ) &dstrData, 
                 &g_dstrTagDataSecVerPlatform, 
                 NULL,
                 NULL,
                 NULL,
                 (DRM_CONST_STRING *)&dstrSubData,
                 g_wchForwardSlash  );
        if ( DRM_FAILED( dr ) )
        {
            printf( "%S XML tag is required in unsigned template file when %S is present, exiting...\n", 
                    g_dstrTagDataSecVerPlatform.pwszString, 
                    g_dstrTagDataSecVer.pwszString );
            ChkDR( dr );
        }

        dr = DRMCRT_wcsntol( dstrSubData.pwszString,
                             dstrSubData.cchString,
                             ( DRM_LONG* )&f_poBCertBuilderData->SecurityVersion.dwPlatformIdentifier );
        if ( DRM_FAILED( dr ) )
        {
            printf("Invalid %S value in unsigned template file, exiting...\n", g_dstrTagDataSecVerPlatform.pwszString );
            if ( DRM_SUCCEEDED( dr ) )
            {
                dr = DRM_E_INVALIDARG;
            }
            ChkDR( dr );
        }

        dr = DRM_XML_GetSubNodeByPath( 
                 ( DRM_CONST_STRING * ) &dstrData, 
                 &g_dstrTagDataSecVerNumber, 
                 NULL,
                 NULL,
                 NULL,
                 (DRM_CONST_STRING *)&dstrSubData,
                 g_wchForwardSlash  );
        if ( DRM_FAILED( dr ) )
        {
            printf( "%S XML tag is required in unsigned template file when %S is present, exiting...\n", 
                    g_dstrTagDataSecVerNumber.pwszString,
                    g_dstrTagDataSecVer.pwszString );
            ChkDR( dr );
        }

        /*
        ** Convert from A.B.C.D to a DWORD
        */
        ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSubData.pwszString,
                                                    dstrSubData.cchString,
                                                    &f_poBCertBuilderData->SecurityVersion.dwSecurityVersion ) );
        f_poBCertBuilderData->SecurityVersion.fValid = TRUE;
    }
    dr = DRM_SUCCESS; /* we don't care if we don't have a security version. Resetting the error code */


    /*
    ** Now read various features: index in g_rdstrFeatureXPaths is feature's value with exception of <CLOCK>.
    ** Note that symmetric optimization is always included.
    */
    f_poBCertBuilderData->dwNumFeatureEntries = 0;
    
    for ( dwCount = 1; dwCount <= DRM_BCERT_MAX_FEATURES; dwCount++ )
    {
        if ( g_rdstrFeatureXPaths[dwCount].pwszString == NULL )
        {
            continue; /* empty string, do not read this feature's information from the template */
        }
        if ( DRM_FAILED( DRM_XML_GetSubNodeByPath( 
                            ( DRM_CONST_STRING * )&( g_oState.dstrUnsignedTemplate ), 
                            &g_rdstrFeatureXPaths[dwCount], 
                            NULL,
                            NULL,
                            NULL,
                            (DRM_CONST_STRING *)&dstrData,
                            g_wchForwardSlash ) ) )
        {
            continue; /* feature was not specified i.e. it is not set */          
        }
        /* 
        ** CLOCK tag needs some extra parsing
        */
        if ( dwCount == DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK )
        {
            DRM_DWORD dwValue = 0;
            dr = DRMCRT_wcsntol( dstrData.pwszString,
                                   dstrData.cchString,
                                   ( DRM_LONG* )&dwValue );
            if ( DRM_FAILED( dr ) )
            {
                printf("Invalid %S value in unsigned template file, exitting...\n", g_rdstrFeatureXPaths[dwCount].pwszString );
                ChkDR( dr );
            }

            if ( dwValue == DRM_CLOCK_ANTIROLLBACK )
            {
               f_poBCertBuilderData->prgdwFeatureSet[ f_poBCertBuilderData->dwNumFeatureEntries ] = DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK;
               f_poBCertBuilderData->dwNumFeatureEntries++;
            } 
            else if ( dwValue == DRM_CLOCK_SECURE )
            {
               f_poBCertBuilderData->prgdwFeatureSet[ f_poBCertBuilderData->dwNumFeatureEntries ] = DRM_BCERT_FEATURE_SECURE_CLOCK;
               f_poBCertBuilderData->dwNumFeatureEntries++;
            }
            else
            {
                printf("Invalid %S value in unsigned template file, exitting...\n", g_rdstrFeatureXPaths[dwCount].pwszString );
                ChkDR( dr );
            }
        }
        else
        {
           f_poBCertBuilderData->prgdwFeatureSet[ f_poBCertBuilderData->dwNumFeatureEntries ] = dwCount;
           f_poBCertBuilderData->dwNumFeatureEntries++;
        }
    }
    
ErrorExit:
   return dr;
}
    
/******************************************************************************
**
** Function :   GenerateBinaryCertificate
** 
** Synopsis :   Builds a binary certificate
** 
** Arguments :
** 
** Returns :
** 
** Notes :
**
******************************************************************************/
DRM_RESULT GenerateBinaryCertificate()
{
    DRM_RESULT             dr                   = DRM_SUCCESS;
    DRM_BOOL               fResult              = FALSE;
    DRM_DWORD              cbWritten            = 0;
    DRM_BCERT_CERTIFICATE  oCertificate         = { 0 };
    DRM_DWORD              dwOffset             = 0;
    DRM_BCERT_BUILDER_DATA oBCertBuilderData    = { 0 };
    DRM_BCERT_CERT_ID      oCertificateID       = { 0 };
    DRM_DWORD              rgdwFeatureSet[ DRM_BCERT_MAX_FEATURES ] = { 0 };
    DRM_DWORD              rgdwKeyUsageSet[] = { DRM_BCERT_KEYUSAGE_SIGN, DRM_BCERT_KEYUSAGE_ISSUER_DEVICE };
    PUBKEY_P256            oParentPubKey        = { 0 };
    DRM_BYTE               *pbNewCert           = NULL;
    DRM_DWORD              cbNewCert            = 0;
    DRMFILETIME            ftCurrent            = { 0 };
    DRM_BCERT_BUILDER_CERTKEY oCertKey          = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};

    /*
    ** Verify a parent certificate
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrent );

    ChkDR( DRM_BCert_InitVerificationContext( &ftCurrent,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_ISSUER,
                                              &g_CryptoCtx,
                                              TRUE,
                                              FALSE,
                                              NULL,
                                              0,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_ParseCertificateChain(
                g_oState.pbBinaryCertData,
                g_oState.cbBinaryCertDataLength,
                &oVerificationContext ) );

    /*
    ** Parse the first certificate in chain
    */
    dwOffset += SIZEOF( DRM_BCERT_CHAIN_HEADER ); /* skip 5 DWORDs */
    

    ChkDR( DRM_BCert_GetCertificate(
                g_oState.pbBinaryCertData,
                g_oState.cbBinaryCertDataLength,
                &dwOffset,
                &oCertificate,
                DRM_BCERT_CERTTYPE_ISSUER ) );

    /*
    ** Parent cert's public key
    */
    DRM_BYT_CopyBytes( oParentPubKey.m_rgbPubkey, 
                       0, 
                       oCertificate.KeyInfo.rgoKeys[0].pValue->m_rgbPubkey, 
                       0, 
                       SIZEOF( PUBKEY_P256  ) );

    /*
    ** Certificate ID
    */
    ChkDR( Oem_Random_GetBytes( NULL, 
                                oCertificateID.rgb, 
                                __CB_DECL( DRM_BCERT_CERT_ID_LENGTH ) ) );
    
    /*
    ** Generate this certificate's key pair
    */
    ChkDR(DRMTOOLS_CreateKeyPair( g_oState.pcontextCrypto,
                                  eDRM_ECC_P256,
                                  eDRM_RSA_UNKNOWN,
                                  ( DRM_BYTE * )&( g_oState.pubKeyGC_P256 ),
                                  SIZEOF( g_oState.pubKeyGC_P256 ),
                                  ( DRM_BYTE * )&( g_oState.privKeyGC_P256 ),
                                  SIZEOF( g_oState.privKeyGC_P256 ),
                                  NULL,
                                  NULL ) );

    /*
    ** Initialize input data for a cert builder: key usage
    */
    oCertKey.dwNumKeyUsages = 2;
    oCertKey.pKeyUsageSet = (DRM_DWORD*)rgdwKeyUsageSet;
    oCertKey.wKeyType = DRM_BCERT_KEYTYPE_ECC256;
    oCertKey.wKeyLength = ECC_P256_PUBKEY_SIZE_IN_BITS;
    DRM_BYT_CopyBytes( oCertKey.KeyValue.m_rgbPubkey, 
                       0, 
                       g_oState.pubKeyGC_P256.m_rgbPubkey, 
                       0, 
                       SIZEOF( PUBKEY_P256  ) );

    /*
    ** Fill in more fields with defaults
    */
    BCert_SetDefaultBuilderData(
            &oCertificateID,
            DRM_BCERT_CERTTYPE_ISSUER,
            &g_oState.privKeyDAC_P256,
            &oParentPubKey,
            &oBCertBuilderData );

    oBCertBuilderData.pCertificateKeys = &oCertKey;
    oBCertBuilderData.dwNumCertificateKeys = 1;

    oBCertBuilderData.prgdwFeatureSet = rgdwFeatureSet;
    oBCertBuilderData.pManufacturerStrings = (DRM_BCERT_MANUFACTURER_STRINGS*) Oem_MemAlloc( SIZEOF(DRM_BCERT_MANUFACTURER_STRINGS) );

    /*
    ** Read the manufacturer name from the previous certificate - if it's present
    */
    if ( oCertificate.ManufacturerInfo.ManufacturerStrings.ManufacturerName.cb > 0 )
    {
        oBCertBuilderData.pManufacturerStrings->ManufacturerName.cb = oCertificate.ManufacturerInfo.ManufacturerStrings.ManufacturerName.cb;
        ZEROMEM( oBCertBuilderData.pManufacturerStrings->ManufacturerName.rgb, DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH );
        
        DRM_BYT_CopyBytes( oBCertBuilderData.pManufacturerStrings->ManufacturerName.rgb,
                           0,
                           oCertificate.ManufacturerInfo.ManufacturerStrings.ManufacturerName.rgb,
                           0,
                           oCertificate.ManufacturerInfo.ManufacturerStrings.ManufacturerName.cb );
    }

    /*
    ** Get the rest of data needed from UnsignedTemplate XML file
    */
    ChkDR( _parseDataFromUnsignedTemplate( &oBCertBuilderData ) );

    /*
    ** Add a new cert to a chain - but first find out how much memory to allocate 
    */
    dr = BCert_AddCert( g_oState.pbBinaryCertData, 
                        pbNewCert, 
                        &cbNewCert, 
                        &oBCertBuilderData, 
                        &g_CryptoCtx,
                        NULL );
    
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbNewCert = (DRM_BYTE*)Oem_MemAlloc( cbNewCert ) );

        ChkDR( BCert_AddCert( g_oState.pbBinaryCertData, 
                              pbNewCert, 
                              &cbNewCert, 
                              &oBCertBuilderData, 
                              &g_CryptoCtx,
                              NULL ) );
    }

    /*
    ** Verify the result
    */
    oVerificationContext.rgdwRequiredKeyUsages[0] = DRM_BCERT_KEYUSAGE_SIGN;
    oVerificationContext.rgdwRequiredKeyUsages[1] = DRM_BCERT_KEYUSAGE_ISSUER_DEVICE;
    oVerificationContext.cRequiredKeyUsages = 2;
    
    ChkDR( DRM_BCert_ParseCertificateChain(
                pbNewCert,
                cbNewCert,
                &oVerificationContext ) );    

    /*
    ** Write a new cert into a file
    */
    fResult = Oem_File_Write( g_oState.fpBinaryCertFile,
                              pbNewCert,
                              cbNewCert,
                              &cbWritten );
    if ( !fResult || ( cbWritten != cbNewCert ) )
    {
        ChkDR( DRM_E_FILEWRITEERROR );
    }
    
ErrorExit:
    SAFE_OEM_FREE( pbNewCert );
    SAFE_OEM_FREE( oBCertBuilderData.pManufacturerStrings );
    
    return dr;
}


/******************************************************************************
** 
** Function :   _checkRequiredUnsignedTemplateXMLTags
** 
** Synopsis :   Helper function that checks for presense of several XML tags
**              in unsigned template file required for WMDRM and PlayReady certificates.
**
** Arguments :  [f_pdstrData] - unsigned template data, presumably valid XML with
**              several required XML tags.
** 
** Returns :    DRM_SUCCESS
** 
******************************************************************************/
static DRM_RESULT _checkRequiredUnsignedTemplateXMLTags(
    __in  DRM_STRING    *f_pdstrData
    )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_STRING dstrData       = EMPTY_DRM_STRING;
    DRM_DWORD  dwCount        = 0;
    DRM_LONG   lSecurityLevel = 0;

    const DRM_CONST_STRING* g_rpdstrTagNames[7] = { 
                                                  &g_dstrTagSecurityLevel, /* SECURITYLEVEL*/
                                                  &g_dstrTagName,          /* NAME */
                                                  &g_dstrManufacturer,     /* MANUFACTURER */
                                                  &g_dstrMake,             /* MAKE */
                                                  &g_dstrModel,            /* MODEL */
                                                  &g_dstrCertFeatures,     /* FEATURES */
                                                  &g_dstrLimits            /* LIMITS */
                                                  };                                                    

    for ( dwCount = 0; dwCount < 7; dwCount++ )
    {
        dr = DRM_XML_GetSubNodeByPath( 
                                (DRM_CONST_STRING *)f_pdstrData, 
                                g_rpdstrTagNames[ dwCount ], 
                                NULL,
                                NULL,
                                NULL,
                                (DRM_CONST_STRING *)&dstrData,
                                g_wchForwardSlash );
        
        if ( DRM_FAILED( dr ) )
        {
            printf( "%S XML tag is required in unsigned template file, exitting...\n", g_rpdstrTagNames[ dwCount ]->pwszString );
            ChkDR( dr );
        }
        
        if ( dwCount == 0 ) /* additional verification for security level */
        {
            dr = DRMCRT_wcsntol( dstrData.pwszString,
                                 dstrData.cchString,
                                 ( DRM_LONG* )&lSecurityLevel );
            if ( DRM_FAILED( dr ) 
                || lSecurityLevel > SECURITYLEVEL_2000
                || lSecurityLevel == 0 ) /* 0 likely means there was no digits. Also we do not allow zero secutiry level */
            {
                printf("Invalid %S value in unsigned template file, exitting...\n", g_dstrTagSecurityLevel.pwszString);
                if ( DRM_SUCCEEDED( dr ) )
                {
                    dr = DRM_E_INVALIDARG;
                }
                ChkDR( dr );
            }
        }
    }
    
ErrorExit:
    return dr;
}


/******************************************************************************
** 
** Function :   GenerateDevcert
** 
** Synopsis :   Generate the devcert
** 
** Arguments :  
** 
** Returns :    
** 
** Notes :   
**  
** 
******************************************************************************/
DRM_RESULT GenerateDevcert( )
{
    DRM_BYTE        rgbSignature[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )] = { 0 };   
    DRM_DWORD       cbSignature     = DRM_ECC160_SIGNATURE_LEN;
    DRM_BOOL        fResult         = FALSE;
    DRM_DWORD       dwApproxSize    = 0;
    DRM_DWORD       dwAdditional    = 0;
    DRM_DWORD       cbWritten       = 0;
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_STRING      dstrClose       = EMPTY_DRM_STRING;
    DRM_STRING      dstrSignature   = EMPTY_DRM_STRING;
    DRM_STRING      dstrGroupData   = EMPTY_DRM_STRING;
    
    ChkDR(_ParseDACResponse());

    /*
    ** Check for XML tags which are required for WMDRM certificate.
    ** This function will take care of diagnostic output.
    */
    ChkDR( _checkRequiredUnsignedTemplateXMLTags( &(g_oState.dstrUnsignedTemplate) ) );
        
    /*
    **  Allocate buffer for devcert template
    */
    dwApproxSize = CB_DSTR(&(g_oState.dstrAuthCert));
    
    dwAdditional = CB_DSTR(&(g_oState.dstrAuthRootCert));                 
    ChkOverflow(dwApproxSize + dwAdditional, dwAdditional);
    dwApproxSize += dwAdditional;

    dwAdditional = CB_DSTR(&(g_oState.dstrUnsignedTemplate));
    ChkOverflow(dwApproxSize + dwAdditional, dwAdditional);
    dwApproxSize += dwAdditional;

    dwAdditional = SIGNED_TEMPLATE_EXTRA_SIZE;  
    ChkOverflow(dwApproxSize + dwAdditional, dwAdditional);
    dwApproxSize += dwAdditional;

    ChkMem(g_oState.pbTemplate = (_XMBContext *) Oem_MemAlloc(dwApproxSize));
    MEMSET(g_oState.pbTemplate, 0, dwApproxSize);

    /*
    **  Generate a GC private public keypair
    */
    ChkDR(DRMTOOLS_CreateKeyPair( g_oState.pcontextCrypto,
                                  eDRM_ECC_P160,
                                  eDRM_RSA_UNKNOWN,
                                  ( DRM_BYTE * )&(g_oState.pubkeyGC),
                                  SIZEOF( g_oState.pubkeyGC ),
                                  ( DRM_BYTE * )&(g_oState.privkeyGC),
                                  SIZEOF( g_oState.privkeyGC ),
                                  &(g_oState.dstrGCPubKey), 
                                  &(g_oState.dstrGCPrivKey)));
            
    
    /*
    **  Create Devcert template document
    */
    ChkDR(DRM_XMB_CreateDocument(dwApproxSize, g_oState.pbTemplate, &g_dstrTagDevCert));

    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_AddAttribute(g_oState.pbTemplate, 
                                          &g_dstrAttributeVersion, 
                                          &g_dstrVersion_1_0));

    /*  <CERTIFICATE type="device"> */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_OpenNode(g_oState.pbTemplate, &g_dstrTagCertificate));
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_AddAttribute(g_oState.pbTemplate, 
                                          &g_dstrAttributeType,
                                          &g_dstrCertTypeDevice));

    /*  <DATA>  */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_OpenNode(g_oState.pbTemplate, &g_dstrTagData));

    /*  <UNIQUEID private=\"1\">CgoKCgoKCgoKCgoKCgoKCgoKCgo=</UNIQUEID> */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagDNIUniqueID, 
                                      NULL,
                                      &g_dstrRootSigValueVersionTag,
                                      &g_dstrRootSigValueVersionVal,
                                      wttClosed));

    /*  <PUBLICKEY private=\"1\">Device public key</PUBLICKEY> */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagDataPubKey, 
                                      NULL,
                                      &g_dstrRootSigValueVersionTag,
                                      &g_dstrRootSigValueVersionVal,
                                      wttClosed));

    /*  <KEYDATA> 
            template for Device Private key encrypted using the GC Public Key 
        </KEYDATA>   */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrPFRootTag, 
                                      NULL,
                                      NULL,
                                      NULL,
                                      wttClosed));

    /* <NAME>
        Model Name
       </NAME>
    */
    CallXmlBuilder(g_oState.pbTemplate, 
                DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                  &g_dstrTagName, 
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttClosed));

    /*  Close the data node */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_CloseCurrNode(g_oState.pbTemplate, NULL));

    /*  <MSDRM_SIGNATURE_VALUE> 
            template for Signature over DATA using GC private key 
        </MSDRM_SIGNATURE_VALUE>   */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagMSDRMSignature, 
                                      NULL,
                                      NULL,
                                      NULL,
                                      wttClosed));

    /*
    ** In theory we might wrap this next line of code in an
    ** "if( DRM_SYMOPT_IsSymOptSupported() )" block, but there's
    ** no reason for us to do so because it doesn't hurt to 
    ** always have the SYMSIGNATURE node.
    */

    /*  <SYMSIGNATURE> </SYMSIGNATURE>   */
    CallXmlBuilder( g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagSymSig, 
                                      NULL,
                                      NULL,
                                      NULL,
                                      wttClosed ) );

    /*  Close the certificate node */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_CloseCurrNode(g_oState.pbTemplate, NULL));


    /*  <FALLBACK>  */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_OpenNode(g_oState.pbTemplate, &g_dstrTagFallback));

    /*  <SECURITYVERSION>2.3.101.0</SECURITYVERSION>    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagSecurityVersion, 
                                      (DRM_CONST_STRING *)&(g_oState.dstrSecurityVersion),
                                      NULL,
                                      NULL,
                                      wttClosed));

    /*  <CERTIFICATE private="1">
            template for Device Public key signed with the Fallback signing key.
            Standard CERT structure w/2.3.101.0 as version.
        </CERTIFICATE>  */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagCertificate, 
                                      NULL,
                                      &g_dstrRootSigValueVersionTag,
                                      &g_dstrRootSigValueVersionVal,
                                      wttClosed));

    /*  Close the Fallback node */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_CloseCurrNode(g_oState.pbTemplate, NULL));

    /*  <SIGNING>  */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_OpenNode(g_oState.pbTemplate, &g_dstrTagSigning));

    /*  <SECURITYVERSION>2.3.101.0</SECURITYVERSION>    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagSecurityVersion, 
                                      (DRM_CONST_STRING *)&(g_oState.dstrSecurityVersion),
                                      NULL,
                                      NULL,
                                      wttClosed));

    /*  <CERTIFICATE private="1">
            template for Device Public key signed with the Fallback signing key.
            Standard CERT structure w/2.3.101.0 as version.
        </CERTIFICATE>  */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagCertificate, 
                                      NULL,
                                      &g_dstrRootSigValueVersionTag,
                                      &g_dstrRootSigValueVersionVal,
                                      wttClosed));

    /*  Close the Signing node */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_CloseCurrNode(g_oState.pbTemplate, NULL));

    /*  <Certificate type="GROUP"> */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_OpenNode(g_oState.pbTemplate, &g_dstrTagCertificate));
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_AddAttribute(g_oState.pbTemplate, 
                                          &g_dstrAttributeType,
                                          &g_dstrCerttypeGroup));

    /*  <DATA> */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_OpenNode(g_oState.pbTemplate, &g_dstrTagData));
    
    /*
    **  Get the group data inside Unsigned template 
    */
    ChkDR(DRM_XML_GetNode((DRM_CONST_STRING *)&(g_oState.dstrUnsignedTemplate), 
                            &g_dstrUnsignedtemplate, 
                            NULL,
                            NULL,
                            0, 
                            NULL,
                            (DRM_CONST_STRING *)&dstrGroupData));

    /*
    **  Append the data into the devcert template 
    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_AddData(g_oState.pbTemplate, 
                                     (DRM_CONST_STRING *)&dstrGroupData));
    
    /*  <PUBLICKEY>GC Public key</PUBLICKEY>    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagDataPubKey, 
                                      (DRM_CONST_STRING *)&(g_oState.dstrGCPubKey),
                                      NULL,
                                      NULL,
                                      wttClosed));

    /* Close data node */
    CallXmlBuilder(g_oState.pbTemplate, 
        DRM_XMB_CloseCurrNode(g_oState.pbTemplate, &dstrClose));

    /* Now we need to sign the data portion with DAC private key */
    ChkDR(DRM_PK_Sign(g_oState.pcontextCrypto->rgbCryptoContext, 
                      eDRM_ECC_P160,
                      DRM_ECC160_PRIVATE_KEY_LEN,
                      ( const DRM_BYTE * )&g_oState.privkeyDAC, 
                      CB_DSTR(&dstrClose),  
                      PB_DSTR(&dstrClose), 
                      &cbSignature,
                      rgbSignature)); 

    dstrSignature.cchString = CCH_BASE64_EQUIV(cbSignature);

    ChkOverflow(dstrSignature.cchString * SIZEOF(DRM_WCHAR), dstrSignature.cchString);
    ChkMem(dstrSignature.pwszString = (DRM_WCHAR *)Oem_MemAlloc(dstrSignature.cchString * SIZEOF(DRM_WCHAR)));

    ChkDR(DRM_B64_EncodeW(rgbSignature, 
                          cbSignature, 
                          dstrSignature.pwszString, 
                         &dstrSignature.cchString, 
                          0));

    /*  <MSDRM_SIGNATURE_VALUE> 
            template for Signature over DATA using GC private key 
        </MSDRM_SIGNATURE_VALUE>   */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_WriteTag(g_oState.pbTemplate, 
                                      &g_dstrTagMSDRMSignature, 
                                      (DRM_CONST_STRING *)&dstrSignature,
                                      NULL,
                                      NULL,
                                      wttClosed));    

    /*  Close the GROUP node    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_CloseCurrNode(g_oState.pbTemplate, NULL));
    
    /*
    **  Append the Authorization cert
    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_AppendNode(g_oState.pbTemplate, 
                                        (DRM_CONST_STRING *)&(g_oState.dstrAuthCert)));
    /*
    **  Append the Authorization Root cert
    */
    CallXmlBuilder(g_oState.pbTemplate, 
                    DRM_XMB_AppendNode(g_oState.pbTemplate, 
                                        (DRM_CONST_STRING *)&(g_oState.dstrAuthRootCert)));
    
    /* close the root node */
    CallXmlBuilder(g_oState.pbTemplate, 
        DRM_XMB_CloseDocument(g_oState.pbTemplate, &dstrClose));    

    printf("Verifying Devcert template...\n");

    ChkDR( VerifyDevcert( &dstrClose ) );
    
    fResult = Oem_File_Write( g_oState.hDevcertTemplate,
                   (DRM_BYTE*)g_rgbFFFE,
                      SIZEOF( g_rgbFFFE ),
                             &cbWritten );

    if (!fResult || (cbWritten != SIZEOF(g_rgbFFFE)))
    {
        ChkDR(DRM_E_FILEWRITEERROR);
    }

    fResult = Oem_File_Write( g_oState.hDevcertTemplate, 
                              dstrClose.pwszString, 
                      CB_DSTR(&dstrClose),                        
                              &cbWritten);
    if (!fResult || (cbWritten != CB_DSTR(&dstrClose)))
    {
        ChkDR(DRM_E_FILEWRITEERROR);
    }
    
ErrorExit:
    if (dstrSignature.pwszString != NULL)
    {
        Oem_MemFree(dstrSignature.pwszString);
    }
   return dr;
}


/******************************************************************************
** 
** Function :   GeneratePrivKeysFile
** 
** Synopsis :   Generate the private key file for device
** 
** Arguments :  
** 
** Returns :    
** 
** Notes :   
**  <PRIVATEKEYS>
**      <GROUP/>
**      <FALLBACK/>
**  </PRIVATEKEYS>
** 
******************************************************************************/
DRM_RESULT GeneratePrivKeysFile()
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BOOL   fResult   = FALSE;
    DRM_DWORD  cbWritten = 0;
    PRIVKEY    rgPrivKeys        [2];
        
    MEMCPY(&rgPrivKeys [0], (DRM_BYTE*) &g_oState.privkeyGC,       SIZEOF(PRIVKEY));
    MEMCPY(&rgPrivKeys [1], (DRM_BYTE*) &g_oState.privkeyFallback, SIZEOF(PRIVKEY));
    
    if ( g_oState.hDeviceKeys != OEM_INVALID_HANDLE_VALUE )
    {
        fResult = Oem_File_Write( g_oState.hDeviceKeys,
                                  (DRM_BYTE*)rgPrivKeys,
                                  SIZEOF( rgPrivKeys ),
                                  &cbWritten );

        if ( !fResult || ( cbWritten != SIZEOF( rgPrivKeys ) ) )
        {
            ChkDR( DRM_E_FILEWRITEERROR );
        }        
    }
    
ErrorExit:
    return dr;
}    


/******************************************************************************
** 
** Function :   GeneratePrivKeysFile_P256
** 
** Synopsis :   Generate the ECC-256 private key file for device
** 
** Arguments :  
** 
** Returns :    
** 
******************************************************************************/
DRM_RESULT GeneratePrivKeysFile_P256()
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BOOL   fResult   = FALSE;
    DRM_DWORD  cbWritten = 0;
    PRIVKEY_P256  privKey = { 0 };
        
    MEMCPY( &privKey, 
            ( DRM_BYTE* ) &g_oState.privKeyGC_P256,
            SIZEOF( PRIVKEY_P256 ) );
    
    fResult = Oem_File_Write( g_oState.fpDeviceKeys_P256,
                              (DRM_BYTE*)&privKey,
                              SIZEOF(PRIVKEY_P256),
                              &cbWritten );
    if ( !fResult || ( cbWritten != SIZEOF(PRIVKEY_P256) ) )
    {
        ChkDR( DRM_E_FILEWRITEERROR );
    }
    
ErrorExit:
    return dr;
}    

/******************************************************************************
** 
** Function :   GenerateRSAPrivKeyFile
** 
** Synopsis :   Creates file containing private keys
** 
** Arguments : [f_pState] - pointer to structute that contans current status
** 
** Returns :   DRM_SUCCESS if key was created successfully
** 
** Notes :      
**
**  <OEMPrivateKeys>
**      <PRIVKEY>base64 encoded RSA private key</PRIVKEY>
**  </OEMPrivateKeys>
**
******************************************************************************/
static DRM_RESULT GenerateRSAPrivKeyFile( 
    __in DRM_RSA_PRIVATE_KEY *f_pRSAPrivKey,
    __in OEM_FILEHDL          f_fpPrivKey )
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    DRM_STRING   dstrClose = EMPTY_DRM_STRING;
    DRM_DWORD    cbWrite   = 0;
    DRM_DWORD    cbWritten = 0;
    _XMBContext *pbXML     = NULL;

    DRMASSERT( f_pRSAPrivKey != NULL );

    ChkMem( pbXML = (_XMBContext *) Oem_MemAlloc( XML_RSA_KEY_SIZE ) );
    ZEROMEM( pbXML, XML_RSA_KEY_SIZE );

    ChkDR( DRM_XMB_CreateDocument( XML_RSA_KEY_SIZE, 
                                   pbXML, 
                                   &g_dstrTagOEMPrivateKeys ) );
    /*
    ** Writes RSA private key.
    */
    ChkDR( DRM_XML_RSA_WritePrivateKeyNode( pbXML, 
                                            &g_dstrTagPrivkey, 
                                            f_pRSAPrivKey ) );

    /*
    **  close the root node
    */
    CallXmlBuilder( pbXML, 
                    DRM_XMB_CloseDocument( pbXML, &dstrClose ) );

    cbWrite = SIZEOF( g_rgbFFFE );
    ChkBOOL( Oem_File_Write( f_fpPrivKey, (DRM_BYTE*)g_rgbFFFE, cbWrite, &cbWritten )==TRUE, DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == cbWrite, DRM_E_FILEWRITEERROR );

    cbWrite = CB_DSTR( &dstrClose );
    ChkBOOL( Oem_File_Write( f_fpPrivKey, dstrClose.pwszString, cbWrite, &cbWritten )==TRUE, DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == cbWrite, DRM_E_FILEWRITEERROR );

ErrorExit:
    SAFE_OEM_FREE( pbXML );
    return dr;
}


#define RSA_KEY_ENTRIES 2
/******************************************************************************
** 
** Function :   ParsePlayReadyNDCompanyCert
** 
** Synopsis :   Parses PlayReady-ND company certificate and gets  company base64 encoded
**              public key modulus and RSA key length
**
** Arguments :  [f_pState] - pointer to tool's state structure and data
** 
** Returns :    DRM_SUCCESS if certificate was successfully parsed
** 
******************************************************************************/
static DRM_RESULT ParsePlayReadyNDCompanyCert( __in DRM_PLAYREADYND_DATA *f_pData )
{
    DRM_RESULT            dr = DRM_SUCCESS;
    DRM_SUBSTRING         dasstrCompanyCertCollection = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrCompanyCert           = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_STRING       dastrCompanyCertLocal       = EMPTY_DRM_SUBSTRING;
    DRM_DWORD             cEntries                    = RSA_KEY_ENTRIES;
    DRM_SUBSTRING         rgdasstrRSAKeyAttrs[ RSA_KEY_ENTRIES ] = { 0 };

    DRMASSERT( f_pData != NULL );

    /* Get public key */
    dasstrCompanyCertCollection.m_cch = f_pData->dastrCompanyCert.cchString;

    ChkDR( DRM_XML_GetNodeA( f_pData->dastrCompanyCert.pszString,
                            &dasstrCompanyCertCollection, 
                            &g_dastrTagCertificateCollection, 
                             NULL, 
                             NULL, 
                             0, 
                             NULL,
                            &dasstrCompanyCert ) );

    dastrCompanyCertLocal.pszString = f_pData->dastrCompanyCert.pszString + 
                                          dasstrCompanyCert.m_ich;
    dastrCompanyCertLocal.cchString = dasstrCompanyCert.m_cch;

    dasstrCompanyCert.m_ich = 0;

    ChkDR( DRM_WCP_GetAttribute( dastrCompanyCertLocal.pszString,
                                 &dasstrCompanyCert,
                                 DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA,
                                 rgdasstrRSAKeyAttrs,
                                 &cEntries ) );

    f_pData->dastrCompanyPubKeyModulusB64.pszString = dastrCompanyCertLocal.pszString + rgdasstrRSAKeyAttrs[1].m_ich;
    f_pData->dastrCompanyPubKeyModulusB64.cchString = rgdasstrRSAKeyAttrs[1].m_cch;
    f_pData->eKeyLength = eDRM_RSA_1024;

ErrorExit:
    return dr;
}



/******************************************************************************
** 
** Function :   _checkRequiredNDUnsignedTemplateXMLTagsA
** 
** Synopsis :   Helper function that checks for presense of several XML tags
**              in ND unsigned template file.
**
** Arguments :  [f_pdastrData] - unsigned template data, presumably valid XML with
**              several required XML tags.
** 
** Returns :    DRM_SUCCESS
** 
******************************************************************************/
static DRM_RESULT _checkRequiredNDUnsignedTemplateXMLTagsA(
    __in  DRM_ANSI_CONST_STRING    *f_pdastrData
    )
{
    DRM_RESULT    dr             = DRM_SUCCESS;
    DRM_SUBSTRING dasstrData     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrNodeData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD     dwCount        = 0;

    const DRM_ANSI_CONST_STRING* g_rpdastrTagNames[4] = { 
                                                  &g_dastrTagName,                   /* NAME */
                                                  &g_dastrManufacturer,              /* MANUFACTURER */
                                                  &g_dastrMake,                      /* MAKE */
                                                  &g_dastrModel                      /* MODEL */
                                                  };                                                    
    dasstrData.m_ich = 0;
    dasstrData.m_cch = f_pdastrData->cchString;

    for ( dwCount = 0; dwCount < 4; dwCount++ )
    {
        dr = DRM_XML_GetSubNodeByPathA( f_pdastrData->pszString,
                                        &dasstrData,
                                        g_rpdastrTagNames[ dwCount ], 
                                        NULL,
                                        NULL,
                                        NULL,
                                        &dasstrNodeData,
                                        g_chForwardSlash );
        if ( DRM_FAILED( dr ) )
        {
            printf( "%s XML tag is required in ND unsigned template file, exitting...\n", g_rpdastrTagNames[ dwCount ]->pszString );
            ChkDR( dr );
        }
    }
    
ErrorExit:
    return dr;
}


/******************************************************************************
** 
** Function :   _UpdateNDTemplate
** 
** Synopsis :   Updates PlayReady-ND unsigned template with default data,
**              if those data weren't specified in the template
**
** Note:        This function reallocate memory that used by unsigned template
**              (releasing old buffer and allocates new) This memory should be released 
**              in _DRMClose method
** Arguments :  [f_pState]            - pointer to tool's state structure and data
**              [f_pdastrCurTemplate] - pointer to current template
**              [f_pdastrNewTemplate] - pointer to updated template
** 
** Returns :    DRM_SUCCESS if template was successfully updated
** 
******************************************************************************/
static DRM_RESULT _UpdateNDTemplate (
    __inout    DRM_PLAYREADYND_DATA   *f_pData )
{
    DRM_RESULT             dr = DRM_SUCCESS;
    _XMBContextA          *pXMLContextA = NULL;
    DRM_CHAR              *pchXMLData = NULL;
    DRM_SUBSTRING          dasstr = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrTemplate    = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING  dastrTemplateData = EMPTY_DRM_STRING;
    const DRM_ANSI_CONST_STRING *rgpdastrValues [1] = { 0 };
    
    DRMASSERT( f_pData != NULL );
    DRMASSERT( f_pData->dastrUnsignedTemplate.cchString != 0 );

    ChkMem( pXMLContextA = (_XMBContextA*) Oem_MemAlloc( MAX_DEVICE_CERT_TEMPLATE_SIZE ) );
    ZEROMEM( pXMLContextA, MAX_DEVICE_CERT_TEMPLATE_SIZE );
    pXMLContextA->cbContext = MAX_DEVICE_CERT_TEMPLATE_SIZE;

    ChkDR( DRM_XMB_CreateDocumentA( pXMLContextA->cbContext, (DRM_BYTE*)pXMLContextA, &g_dastrTagUnsignedTemplate ) );    

    dasstrTemplate.m_cch = f_pData->dastrUnsignedTemplate.cchString;
    ChkDR( DRM_XML_GetNodeA( f_pData->dastrUnsignedTemplate.pszString,  
                             &dasstrTemplate, 
                             &g_dastrTagUnsignedTemplate, 
                             NULL, 
                             NULL, 
                             0, 
                             NULL,
                             &dasstrData ) );
    if ( dasstrData.m_cch != 0 )
    {
        dastrTemplateData.pszString = f_pData->dastrUnsignedTemplate.pszString + dasstrData.m_ich;
        dastrTemplateData.cchString = dasstrData.m_cch;
        ChkDR( DRM_XMB_AppendNodeA( pXMLContextA, &dastrTemplateData ) );
    }
    
    /* Add default security level if necessary */
    if ( !f_pData->fTemplateSecurityLevelIsSpecified )
    { 
        rgpdastrValues[0] = (DRM_ANSI_CONST_STRING*)&g_dastrDefaultSecurityLevel;
        ChkDR( DRM_ND_CB_AddAttribute( pXMLContextA,
                                       DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL,
                                       rgpdastrValues,
                                       1 ) );
    }

    /* Add key usage if necessary */    
    if ( !f_pData->fTemplateKeyUsageIsSpecified )
    {
        rgpdastrValues[0] = (DRM_ANSI_CONST_STRING*)&g_dastrKeyUsageSignCert;
        ChkDR( DRM_ND_CB_AddAttribute( pXMLContextA,
                                       DRM_WMDRM_CERT_ATTR_KEY_USAGE,
                                       rgpdastrValues,
                                       1 ) );
    }

    ChkDR( DRM_XMB_CloseDocumentA( pXMLContextA, &dasstr)); /* </c:CertificateCollection> */

    ChkDR( DRM_XMB_GetXMLBaseA( pXMLContextA, &pchXMLData ) );

    SAFE_OEM_FREE( f_pData->dastrUnsignedTemplate.pszString ) ;

    f_pData->dastrUnsignedTemplate.cchString = dasstr.m_cch+1;
    ChkMem( f_pData->dastrUnsignedTemplate.pszString = (DRM_CHAR*) Oem_MemAlloc( f_pData->dastrUnsignedTemplate.cchString ) );
    ZEROMEM( f_pData->dastrUnsignedTemplate.pszString, f_pData->dastrUnsignedTemplate.cchString );
    
    MEMCPY( (DRM_BYTE*)f_pData->dastrUnsignedTemplate.pszString, (DRM_BYTE*)pchXMLData, dasstr.m_cch );
    
ErrorExit:   
    SAFE_OEM_FREE( pXMLContextA );
    return dr;
}

/******************************************************************************
** 
** Function :   ProcessUnsignedNDTemplate
** 
** Synopsis :   Verifies unsigned template, updates default data if necessary
**
** Arguments :  [f_pState] - pointer to tool's state structure and data
** 
** Returns :    DRM_SUCCESS if unsigned template was correct and (if necessary)
**              successfully updated
** 
******************************************************************************/
static DRM_RESULT ProcessUnsignedNDTemplate( __inout DRM_PLAYREADYND_DATA *f_pData )
{
    DRM_RESULT    dr                  = DRM_SUCCESS;
    DRM_SUBSTRING dasstrData          = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrTemplate      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrKeyUsage      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSecurityLevel = EMPTY_DRM_SUBSTRING;
    
    DRMASSERT( f_pData != NULL );
    DRMASSERT( f_pData->dastrUnsignedTemplate.cchString != 0 );

    dasstrTemplate.m_cch = f_pData->dastrUnsignedTemplate.cchString;

    /* Validate unsigned template */      
    dr = DRM_XML_GetNodeA( f_pData->dastrUnsignedTemplate.pszString,  
                           &dasstrTemplate, 
                           &g_dastrTagUnsignedTemplate, 
                           NULL, 
                           NULL, 
                           0, 
                           NULL,
                           &dasstrData );
    if ( DRM_FAILED( dr ) )
    {
        printf( "Invalid PlayReady-ND unsigned template format, exitting...\n" );
        goto ErrorExit;
    }

    /* Validate key usage field */
    dr = DRM_XML_GetNodeA( f_pData->dastrUnsignedTemplate.pszString,  
                           &dasstrData, 
                           &g_dastrTagWMDRMCertKeyUsage, 
                           NULL, 
                           NULL, 
                           0, 
                           NULL,
                           &dasstrKeyUsage );    
    if ( DRM_SUCCEEDED( dr ) )
    {
        f_pData->fTemplateKeyUsageIsSpecified = TRUE;
    }
    else if ( dr == DRM_E_XMLNOTFOUND )
    {
        f_pData->fTemplateKeyUsageIsSpecified = FALSE;
    }
    else 
    {
        ChkDR( dr );
    }

    /* Validate security level field */
    dr = DRM_XML_GetNodeA( f_pData->dastrUnsignedTemplate.pszString,  
                           &dasstrData, 
                           &g_dastrTagWMDRMCertSecurityLevel, 
                           NULL, 
                           NULL, 
                           0, 
                           NULL,
                           &dasstrSecurityLevel );

    if ( DRM_SUCCEEDED( dr ) )
    {
        f_pData->fTemplateSecurityLevelIsSpecified = TRUE;
    }
    else if ( dr == DRM_E_XMLNOTFOUND )
    {
        f_pData->fTemplateSecurityLevelIsSpecified = FALSE;
        printf( "Default security level is used\n" );
    }
    else 
    {
        ChkDR( dr );
    }

    ChkDR( _UpdateNDTemplate( f_pData ) );
    
ErrorExit:
    return dr;
}

/******************************************************************************
** 
** Function :   _ReadUnsignedTemplateFile
** 
** Synopsis :   Reads unsigned template file and makes sure it's in unicode format
**              and contains valid XML.
**              If failed function prints out diagnostic message and returns error.
**
** Arguments :  [f_pdstrFileName] - unsigned template file name
**              [f_pdstrFileData] - unicode string containing unsigned template data.
** 
** Returns :    DRM_SUCCESS
**
** Notes:       We cannot use conversion from UTF-16 into UTF-8 or similar API
**              to verify whether the file in unicode format because
**              unsigned template data mistakenly saved in ASCII format
**              will likely map into valid UTF-16 symbols.
******************************************************************************/
static DRM_RESULT _ReadUnsignedTemplateFile (
    __in  const DRM_CONST_STRING    *f_pdstrFileName,
    __out       DRM_STRING          *f_pdstrFileData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   *pbFileData;
    DRM_DWORD  cbFileDataLength;
    DRM_STRING dstrTempStr = EMPTY_DRM_STRING;

    dr = DRMTOOLS_ReadBinaryFileData( f_pdstrFileName,
                                        &pbFileData,
                                        &cbFileDataLength );
    if ( DRM_FAILED(dr) )
    {
        printf( "Failure while reading unsigned template file, exitting...\n" );
        ChkDR( dr );
    }

    if ( ( cbFileDataLength % SIZEOF(DRM_WCHAR) ) != 0 )
    {
        printf( "Unsigned template file is not in Unicode format, exitting...\n" );
        ChkDR( DRM_E_UTF_INVALID_CODE );
    }

    /*
    ** Copy data into the string
    */
    f_pdstrFileData->pwszString = (DRM_WCHAR *)pbFileData;
    f_pdstrFileData->cchString = cbFileDataLength / SIZEOF(DRM_WCHAR);

    /*
    ** Look for UNSIGNEDTEMPLATE XML tag - if it is not present 
    ** then the file is either not a valid unicode format
    ** or does not contain unsigned template data.
    */
    dr = DRM_XML_GetNode( (DRM_CONST_STRING *)f_pdstrFileData, 
                          &g_dstrUnsignedtemplate, 
                          NULL,
                          NULL,
                          0, 
                          NULL,
                          (DRM_CONST_STRING *)&dstrTempStr );
    if ( DRM_FAILED( dr ) )
    {
        printf( "Unsigned template file is not in Unicode format or contains no valid data, exitting...\n" );
        ChkDR( dr );
    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( pbFileData );
        f_pdstrFileData->pwszString = NULL;
        f_pdstrFileData->cchString = 0;
    }
    
    return dr;
}

/******************************************************************************
** 
** Function :   GeneratePlayReadyNDModelCert
** 
** Synopsis :   Generates PlayReady-ND model certificate, verifies correctness
**              and saves it to file, file handler: [f_pState->pPlayReadyNDData->fpModelCert] 
**
** Arguments :  [f_pState] - pointer to tool's state structure and data
** 
** Returns :    DRM_SUCCESS if certificate was successfully created, verified and saved
** 
******************************************************************************/
static DRM_RESULT GeneratePlayReadyNDModelCert( __in DRMSTATE *f_pState )
{   
    DRM_RESULT            dr                 = DRM_SUCCESS;
    _XMBContextA         *pXMLCtxA           = NULL;
    DRM_RSA_PRIVATE_KEY  *poRSAModelPrivKey  = NULL; 
    DRM_RSA_PUBLIC_KEY   *poRSAModelPubKey   = NULL; 
    DRM_BOOL              fResult            = FALSE;
    DRM_DWORD             cbWritten          = 0;
    DRM_CRYPTO_CONTEXT   *pCryptoCtx         = NULL;
    DRM_PLAYREADYND_DATA *pPlayReadyNDData   = NULL;
    DRM_BYTE              rgbModulus [__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    DRM_CHAR              rgchModulusB64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX ) )] = { 0 };
    DRM_DWORD             cbModulus          = DRM_RSA_CB_MODULUS_MAX;
    DRM_DWORD             cchModulusB64      = CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX );
    DRM_SUBSTRING         dasstrCompanyCertCollection = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrCompanyCerts  = EMPTY_DRM_STRING;
    DRM_SUBSTRING         dasstrCompanyCerts = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pState != NULL );
    ChkArg( f_pState->pPlayReadyNDData != NULL );

    pPlayReadyNDData = f_pState->pPlayReadyNDData;
    pCryptoCtx = f_pState->pcontextCrypto;

    ChkMem( pXMLCtxA = (_XMBContextA*) Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ) );
    ZEROMEM( pXMLCtxA, MAX_DEVICE_CERT_SIZE );
    pXMLCtxA->cbContext = MAX_DEVICE_CERT_SIZE;

    ChkMem( poRSAModelPrivKey = (DRM_RSA_PRIVATE_KEY*) Oem_MemAlloc( SIZEOF( DRM_RSA_PRIVATE_KEY ) ) );
    ZEROMEM( poRSAModelPrivKey, SIZEOF( poRSAModelPrivKey ) );
    ChkMem( poRSAModelPubKey = (DRM_RSA_PUBLIC_KEY*) Oem_MemAlloc( SIZEOF( DRM_RSA_PUBLIC_KEY ) ) );
    ZEROMEM( poRSAModelPubKey, SIZEOF( poRSAModelPubKey ) );

    /* Validate source certificate */
    ChkDR( ParsePlayReadyNDCompanyCert( pPlayReadyNDData ) );

    /* Process unsigned template, update missing data if necessary */
    ChkDR( ProcessUnsignedNDTemplate( pPlayReadyNDData ) );

    ChkDR( DRMTOOLS_CreateKeyPair( pCryptoCtx,
                                   eDRM_RSA,
                                   pPlayReadyNDData->eKeyLength,
                                   (DRM_BYTE*) poRSAModelPubKey,
                                   DRM_RSA_CB_PUBKEY,
                                   (DRM_BYTE*) poRSAModelPrivKey,
                                   DRM_RSA_CB_PRIVKEY,
                                   NULL,
                                   NULL ) );

    {
        ChkDR( OEM_RSA_ParsePublicKey( poRSAModelPubKey,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &cbModulus,
                                       rgbModulus ) );

        ChkDR( DRM_B64_EncodeA( rgbModulus, 
                                cbModulus, 
                                rgchModulusB64, 
                                &cchModulusB64, 
                                0 ) );
    }

    /*
    ** Unwrap company cert chain from c:certCollection tag
    */
    dasstrCompanyCertCollection.m_cch = pPlayReadyNDData->dastrCompanyCert.cchString;

    ChkDR( DRM_XML_GetNodeA( pPlayReadyNDData->dastrCompanyCert.pszString,
                            &dasstrCompanyCertCollection, 
                            &g_dastrTagCertificateCollection, 
                             NULL, 
                             NULL, 
                             0, 
                             NULL,
                            &dasstrCompanyCerts ) );

    dastrCompanyCerts.pszString = pPlayReadyNDData->dastrCompanyCert.pszString
                                  + dasstrCompanyCerts.m_ich;
    dastrCompanyCerts.cchString = dasstrCompanyCerts.m_cch;

    ChkDR( DRM_ND_CB_CreateCertificateFile( pXMLCtxA, 
                                            certtypeRoot,
                                            poRSAModelPubKey,
                                            (const DRM_ANSI_CONST_STRING*) &pPlayReadyNDData->dastrCompanyPubKeyModulusB64,
                                            &g_dastrWMDRMCertExponent,
                                            &pPlayReadyNDData->oCompanyRSAPrivKey,
                                            (DRM_ANSI_CONST_STRING*)&pPlayReadyNDData->dastrUnsignedTemplate,
                                            &dastrCompanyCerts,
                                            pCryptoCtx ) );
    {
        /* Verify created certificate collection */ 
        DRM_SUBSTRING dasstrCollection    = EMPTY_DRM_SUBSTRING;
        dasstrCollection.m_ich = 0;
        dasstrCollection.m_cch = pXMLCtxA->cchBuffer;
        
        ChkDR( DRM_WCP_VerifyCertificateCollection( pXMLCtxA->rgchXML,
                                                    &dasstrCollection,
                                                    NULL,
                                                    NULL,
                                                    0,
                                                    NULL,
                                                    NULL,
                                                    pCryptoCtx ) );
    }    

    fResult = Oem_File_Write( f_pState->pPlayReadyNDData->hModelCert, 
                              pXMLCtxA->rgchXML, 
                              pXMLCtxA->cchBuffer,                        
                             &cbWritten);
    ChkBOOL( fResult && (cbWritten == pXMLCtxA->cchBuffer ), DRM_E_FILEWRITEERROR );


    /* Save model private key */
    ChkDR( GenerateRSAPrivKeyFile( poRSAModelPrivKey, pPlayReadyNDData->hModelCertKey ) );

ErrorExit:
    SAFE_OEM_FREE( pXMLCtxA );
    OEM_RSA_ZeroPublicKey( poRSAModelPubKey );
    OEM_RSA_ZeroPrivateKey( poRSAModelPrivKey );
    SAFE_OEM_FREE( poRSAModelPubKey );
    SAFE_OEM_FREE( poRSAModelPrivKey );
    return dr;
}

/******************************************************************************
** 
** Function :   DRM_Main
** 
** Synopsis :   The main method of the tool
**
** Arguments :  [argc] - number of arguments in command line
**              [argv] - Command line arguments (wide character strings)
** 
** Returns :    DRM_SUCCESS if certificate was successfully created, verified and saved
** 
******************************************************************************/
DRM_LONG DRM_CALL DRM_Main( 
    DRM_LONG argc, 
    DRM_WCHAR** argv )
{
    DRM_RESULT       dr                              = DRM_SUCCESS;
    DRM_CONST_STRING dstrParentBinaryCertFile        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutputBinaryCertFile        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrUnsignedTemplateFile        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDACResponseFile             = EMPTY_DRM_STRING;    
    DRM_CONST_STRING dstrOEMPrivateKeyFile           = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDevcertTemplateFile         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDeviceKeysFile              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDeviceKeysFile_P256         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPlayReadyNDCompanyCert      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPlayReadyNDModelCert        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPlayReadyNDModelKeyFile     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPlayReadyNDUnsignedTemplate = EMPTY_DRM_STRING;
    DRM_BOOL         fNDCertRequired                 = FALSE;
    DRM_BOOL         fWMDRMPDCertRequired            = FALSE;
    DRM_BOOL         fPlayReadyCertRequired          = FALSE;
    DRM_LONG         iParam                          = 0;  
    DRM_DWORD        dwMappingFlags                  = 0;

    PrintCopyrightInfo( "PlayReady Model Certificate & WMDRM-PD Devcert Template Creation Tool" );

    /*
    **  Check input
    */
    if ( argc <= 1 )
    { 
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    /*
    **  Check how to remap parameters based on the type of certificate, if a certificate type is not found the arguments will not remap (for legacy support)
    */
    for( iParam = 1; iParam < argc; iParam++ )
    {
        if ( argv[iParam][0] == g_wchMinus || argv[iParam][0] == g_wchForwardSlash )
        {
            if ( DRMCRT_towlower( argv[iParam][1] ) == WCHAR_CAST( 'z' ) )
            {
                if(argv[iParam][2] != g_wchColon)
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }

                if ( DRMCRT_wcsicmp( &(argv[iParam][3]), g_rgwchPR) == 0)
                {
                    dwMappingFlags = PR_SETBITS;
                    break;
                }
                else if ( DRMCRT_wcsicmp( &(argv[iParam][3]), g_rgwchPD) == 0 )
                {
                    dwMappingFlags = PD_SETBITS;
                    break;
                }
                else if ( DRMCRT_wcsicmp( &(argv[iParam][3]), g_rgwchND) == 0 )
                {
                    dwMappingFlags = ND_SETBITS;
                    break;
                }
                else
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
        }
        else
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    for( iParam = 1; iParam < argc; iParam++ )
    {
        DRM_WCHAR        wchOption      = 0;
        DRM_DWORD        dwMappedOption = 0;
        DRM_CONST_STRING dstrParam      = EMPTY_DRM_STRING;
        
        if ( !DRM_CMD_ParseCmdLine( argv[iParam], &wchOption, &dstrParam, NULL ) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        dwMappedOption = (DRM_DWORD)(DRMCRT_towlower( wchOption )) | dwMappingFlags;
        
        switch( dwMappedOption )
        {
        case              (DRM_DWORD)(WCHAR_CAST( 'b' )):
        case PR_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'b' )):
            ASSIGN_DRM_STRING( dstrParentBinaryCertFile, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'g' )):
        case PR_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'g' )):
            ASSIGN_DRM_STRING( dstrOutputBinaryCertFile, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'h' )):
        case PR_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'h' )):
            ASSIGN_DRM_STRING( dstrDeviceKeysFile_P256, dstrParam );
            break;
            
        case              (DRM_DWORD)(WCHAR_CAST( 'd' )):
        case PD_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'b' )):
            ASSIGN_DRM_STRING( dstrDACResponseFile, dstrParam );
            break;
                    
        case              (DRM_DWORD)(WCHAR_CAST( 'u' )):
        case PR_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'u' )):
        case PD_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'u' )):
            ASSIGN_DRM_STRING( dstrUnsignedTemplateFile, dstrParam );
            break;
        
        case              (DRM_DWORD)(WCHAR_CAST( 'f' )):
        case PR_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'f' )):
        case PD_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'f' )):
        case ND_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'f' )):
            ASSIGN_DRM_STRING( dstrOEMPrivateKeyFile, dstrParam );
            break;
                    
        case              (DRM_DWORD)(WCHAR_CAST( 't' )):
        case PD_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'g' )):
            ASSIGN_DRM_STRING( dstrDevcertTemplateFile, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'k' )):
        case PD_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'h' )):
            ASSIGN_DRM_STRING( dstrDeviceKeysFile, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'c' )):
        case ND_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'b' )):
            ASSIGN_DRM_STRING( dstrPlayReadyNDCompanyCert, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'm' )):
        case ND_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'g' )):
            ASSIGN_DRM_STRING( dstrPlayReadyNDModelCert, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'n' )):
        case ND_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'h' )):
            ASSIGN_DRM_STRING( dstrPlayReadyNDModelKeyFile, dstrParam );
            break;

        case              (DRM_DWORD)(WCHAR_CAST( 'v' )):
        case ND_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'u' )): 
            ASSIGN_DRM_STRING( dstrPlayReadyNDUnsignedTemplate, dstrParam );
            break;

        case PR_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'z' )):
        case PD_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'z' )):
        case ND_SETBITS | (DRM_DWORD)(WCHAR_CAST( 'z' )):
            break;

        case (DRM_DWORD)(WCHAR_CAST( '?' )):
            if(dstrParam.cchString && DRMCRT_wcsicmp(dstrParam.pwszString, g_rgwchLEGACY) == 0)
            {
                goto _PrintLegacyUsage;
            }
            goto _PrintUsage;

        default:
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    if ( dstrDACResponseFile.cchString             > MAX_FILENAME 
      || dstrUnsignedTemplateFile.cchString        > MAX_FILENAME 
      || dstrOEMPrivateKeyFile.cchString           > MAX_FILENAME 
      || dstrDevcertTemplateFile.cchString         > MAX_FILENAME 
      || dstrDeviceKeysFile.cchString              > MAX_FILENAME
      || dstrDeviceKeysFile_P256.cchString         > MAX_FILENAME
      || dstrParentBinaryCertFile.cchString        > MAX_FILENAME
      || dstrOutputBinaryCertFile.cchString        > MAX_FILENAME 
      || dstrPlayReadyNDCompanyCert.cchString      > MAX_FILENAME 
      || dstrPlayReadyNDModelCert.cchString        > MAX_FILENAME 
      || dstrPlayReadyNDModelKeyFile.cchString     > MAX_FILENAME
      || dstrPlayReadyNDUnsignedTemplate.cchString > MAX_FILENAME )
    {
        printf( "File name is too big. Max Filename is %d \n", MAX_FILENAME );
        ChkArg( FALSE );
    }

    if( dstrOEMPrivateKeyFile.cchString    != 0 ) /* all cert types share OEM privkey file data */
    {
        if ( dstrUnsignedTemplateFile.cchString != 0 ) /* PlayReady and WMDRM-PD certs share unsigned template file */
        {
          /*
          ** Check if we can build PlayReady certificate
          */
            if ( dstrDeviceKeysFile_P256.cchString  != 0
              || dstrParentBinaryCertFile.cchString != 0
              || dstrOutputBinaryCertFile.cchString != 0 )
            {
                if ( dstrUnsignedTemplateFile.cchString   == 0
                  || dstrOEMPrivateKeyFile.cchString      == 0
                  || dstrDeviceKeysFile_P256.cchString    == 0
                  || dstrParentBinaryCertFile.cchString   == 0
                  || dstrOutputBinaryCertFile.cchString   == 0 )
                {
                    if ( dwMappingFlags == 0 )
                    {
                        printf( "Warning: to build PlayReady certificate these parameters are required: -f, -u, -b, -g, -h\n ");
                    }
                    else if( dwMappingFlags == PR_SETBITS )
                    {
                        printf( "Warning: to build PlayReady certificate these parameters are required: -z:PR, -f, -u, -b, -g, -h\n ");
                    }
                }
                else
                {
                    fPlayReadyCertRequired = TRUE;
                }
            }
            /*
            ** Check if we can build WMDRM-PD certificate
            */
            if ( dstrDACResponseFile.cchString != 0
                || dstrDevcertTemplateFile.cchString != 0
                || dstrDeviceKeysFile.cchString != 0 )
            {
                if ( dstrUnsignedTemplateFile.cchString == 0
                    || dstrOEMPrivateKeyFile.cchString == 0
                    || dstrDACResponseFile.cchString == 0
                    || dstrDevcertTemplateFile.cchString == 0
                    || dstrDeviceKeysFile.cchString == 0 )
                {
                    if ( dwMappingFlags == 0 )
                    {
                        printf( "Warning: to build WMDRM-PD certificate these parameters are required: -f, -u, -d, -t, -k\n ");
                    }
                    else if( dwMappingFlags == PD_SETBITS )
                    {
                        printf( "Warning: to build PlayReady certificate these parameters are required: -z:PD, -f, -u, -b, -g, -h\n ");
                    }
                }
                else
                {
                    fWMDRMPDCertRequired = TRUE;
                }
            }
        }
        /*
        ** Check if we can build PlayReady-ND certificate that uses different unsigned template file.
        */
        if ( dstrPlayReadyNDCompanyCert.cchString != 0 
          || dstrPlayReadyNDModelCert.cchString != 0 
          || dstrPlayReadyNDModelKeyFile.cchString != 0 
          || dstrPlayReadyNDUnsignedTemplate.cchString != 0 )
        {
            if ( dstrPlayReadyNDCompanyCert.cchString == 0 
              || dstrPlayReadyNDModelCert.cchString == 0 
              || dstrPlayReadyNDModelKeyFile.cchString == 0
              || dstrPlayReadyNDUnsignedTemplate.cchString == 0 )
            {
                if ( dwMappingFlags == 0 )
                {
                    printf( "Warning: to build PlayReady-ND certificate these parameters are required: -f, -v, -c, -m, -n\n ");
                }
                else if( dwMappingFlags == ND_SETBITS )
                {
                    printf( "Warning: to build PlayReady certificate these parameters are required: -z:ND, -f, -u, -b, -g, -h\n ");
                }
            }
            else
            {
                fNDCertRequired = TRUE;
            }
        }
    }

    /*
    ** Check if we can build at least one or the desired type of cert
    */
    switch( dwMappingFlags & ( PR_SETBITS | PD_SETBITS | ND_SETBITS ) )
    {
    case PR_SETBITS:
        if ( !fPlayReadyCertRequired )
        {
            printf( "Error: not enough parameters for building PlayReady certificate of any supported type.\n" );
            ChkArg( FALSE );
        }
        break;
    case PD_SETBITS:
        if ( !fWMDRMPDCertRequired )
        {
            printf( "Error: not enough parameters for building WMDRM-PD certificate of any supported type.\n" );
            ChkArg( FALSE );
        }
        break;
    case ND_SETBITS:
        if ( !fNDCertRequired )
        {
            printf( "Error: not enough parameters for building ND certificate of any supported type.\n" );
            ChkArg( FALSE );
        }
        break;
    default:
        if ( !fPlayReadyCertRequired && !fWMDRMPDCertRequired && !fNDCertRequired )
        {
            printf( "Error: not enough parameters for building certificate of any supported type.\n" );
            ChkArg( FALSE );
        }
    }

    /*
    **  Initialize state
    */
    ChkDR( DRMInit() );

    /*
    ** Open file with private keys for the new certificates
    */
    printf( "Reading OEM private keys...\n" );    
    ChkDR( DRMTOOLS_ReadFileData( &dstrOEMPrivateKeyFile, &(g_oState.dstrOEMPrivKeys) ) );


    if( fPlayReadyCertRequired || fWMDRMPDCertRequired )
    {
        /*
        **  Open unsigned template
        */
        printf( "Reading unsigned template...\n" );
        ChkDR( _ReadUnsignedTemplateFile( &dstrUnsignedTemplateFile, &(g_oState.dstrUnsignedTemplate) ) );
    }

    if( fPlayReadyCertRequired )
    {
        /*
        ** Open parent binary cert file
        */
        printf( "Reading binary company certificate...\n" );    
        ChkDR( DRMTOOLS_ReadBinaryFileData( &dstrParentBinaryCertFile,
                                            &g_oState.pbBinaryCertData,
                                            &g_oState.cbBinaryCertDataLength ) );    

        /*
        ** Open output file for binary cert
        */
        ChkDR( DRMTOOLS_OpenOutputFile( &dstrOutputBinaryCertFile,
                                        &g_oState.fpBinaryCertFile ) );

        /*
        ** Open output file for ECC256 device private key
        */
        ChkDR( DRMTOOLS_OpenOutputFile( &dstrDeviceKeysFile_P256,
                                        &g_oState.fpDeviceKeys_P256 ) );
    }    

    if ( fWMDRMPDCertRequired )
    {
        printf( "Reading WMDRM-PD company certificate...\n" );    
        ChkDR( DRMTOOLS_ReadFileData( &dstrDACResponseFile, &(g_oState.dstrDACResponse) ) );
        /*
        **  Open output file for XML cert
        */    
        ChkDR( DRMTOOLS_OpenOutputFile( &dstrDevcertTemplateFile,
                                        &g_oState.hDevcertTemplate ) );
        /*
        **  Open output file for ECC160 device private key
        */    
        ChkDR( DRMTOOLS_OpenOutputFile( &dstrDeviceKeysFile,
                                        &g_oState.hDeviceKeys ) );
    }

    if ( fNDCertRequired ) 
    {
        /*
        ** Read PlayReady-ND unsigned template
        */
        printf( "Reading PlayReady-ND unsigned template...\n" );
        ChkDR( DRMTOOLS_ReadBinaryFileData( &dstrPlayReadyNDUnsignedTemplate, 
                                            (DRM_BYTE**)&g_oState.pPlayReadyNDData->dastrUnsignedTemplate.pszString,
                                            &g_oState.pPlayReadyNDData->dastrUnsignedTemplate.cchString) );

        if ( g_oState.pPlayReadyNDData->dastrUnsignedTemplate.cchString == 0 )
        {          
            printf( "PlayReady-ND unsigned template cannot be empty\n" );
            ChkDR( DRM_E_FILEREADERROR );
        }

        /*
        ** Read PlayReady-ND Company cert
        */
        printf( "Reading PlayReady-PD company certificate...\n" );            
        ChkDR( DRMTOOLS_ReadBinaryFileData( &dstrPlayReadyNDCompanyCert, 
                                            (DRM_BYTE**)&g_oState.pPlayReadyNDData->dastrCompanyCert.pszString,
                                            &g_oState.pPlayReadyNDData->dastrCompanyCert.cchString) );

        if ( g_oState.pPlayReadyNDData->dastrCompanyCert.cchString == 0 )
        {          
            printf( "PlayReady-ND company certificate cannot be empty\n" );
            ChkDR( DRM_E_FILEREADERROR );
        }
        
        /*
        ** Open Model cert and Model key files for PlayReady-ND 
        */
        ChkDR( DRMTOOLS_OpenOutputFile( &dstrPlayReadyNDModelCert,
                                        &g_oState.pPlayReadyNDData->hModelCert ) );
        ChkDR( DRMTOOLS_OpenOutputFile( &dstrPlayReadyNDModelKeyFile,
                                        &g_oState.pPlayReadyNDData->hModelCertKey ) );
    }
    /*
    ** Parse OEM keys
    */
    printf( "Parsing OEM keys file to obtain necessary private keys...\n" );
    ChkDR( _ObtainOEMKeys( fWMDRMPDCertRequired, fNDCertRequired, fPlayReadyCertRequired ) );

    if( fPlayReadyCertRequired )
    {
        /*
        ** Generate PlayReady certificate file
        */
        printf( "Generating binary certificate...\n" );
        ChkDR( GenerateBinaryCertificate() );

        /*
        ** Write ECC256 key file
        */
        ChkDR( GeneratePrivKeysFile_P256() );
    }

    if ( fWMDRMPDCertRequired )
    {
        printf( "Generating WMDRM-PD certificate...\n" );
        ChkDR( GenerateDevcert() );

        /*
        ** Write ECC160 key file
        */
        printf( "Generating keys file for the device...\n" );
        ChkDR( GeneratePrivKeysFile() );
    }

    if ( fNDCertRequired ) 
    {
        printf( "Generating PlayReady-ND certificate...\n" );
        ChkDR( GeneratePlayReadyNDModelCert( &g_oState ) );
    }
    
ErrorExit:    
    _DRMClose();

    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\n.", dr );
    }
    return dr;
    
_PrintUsage:
    PrintUsage( argv[0] );
    return dr;
_PrintLegacyUsage:
    PrintLegacyUsage( argv[0] );
    return dr;
}

