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
#include <oemrsa.h>
#include <drmcmdlnpars.h>
#include <oempkcrypto.h>
#include <drmlicense.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmxmlrsakeys.h>
#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmconstants.h>

typedef enum  { DAC_GEN_REQUEST_KEY_WMDRM_PD, DAC_GEN_REQUEST_KEY_FALLBACK, DAC_GEN_REQUEST_KEY_WMDRM_MD } DAC_GEN_REQUEST_KEY_TYPES;

#define     XML_KEY_GEN_INIT_SIZE   1024*10

#define     DAC_GEN_REQUEST_KEY_COUNT_160   2
#define     DAC_GEN_REQUEST_KEY_COUNT_ALL   3

const DRM_CONST_STRING  *g_dstrKeyNames[DAC_GEN_REQUEST_KEY_COUNT_ALL] = { &g_dstrTagDAC, &g_dstrTagFallback, &g_dstrTagDAC_WMDRM_MD };

typedef struct DRMSTATE
{
    PUBKEY              pubkey[DAC_GEN_REQUEST_KEY_COUNT_160];
    PRIVKEY             privkey[DAC_GEN_REQUEST_KEY_COUNT_160];
    PUBKEY_P256         pubkeyDAC_P256;
    PRIVKEY_P256        privkeyDAC_P256;
    DRM_RSA_PUBLIC_KEY_2048BIT  pubkeyDAC_RSA1024;
    DRM_RSA_PRIVATE_KEY_2048BIT privkeyDAC_RSA1024;
    DRM_STRING          dstrPubKey[DAC_GEN_REQUEST_KEY_COUNT_ALL];
    DRM_STRING          dstrPrivKey[DAC_GEN_REQUEST_KEY_COUNT_ALL];
    DRM_CONST_STRING    dstrManufName;
    OEM_FILEHDL         fpDACRequest;
    OEM_FILEHDL         fpOEMPrivKeys;
    _XMBContext         *pbXML;
    DRM_CRYPTO_CONTEXT  *pcontextCrypto;
}DRMSTATE;

/******************************************************************************
**
** Function :   PrintUsage
**
** Synopsis :   Print a command menu to console
**
** Arguments :  [pwszAppName] - name of the application (tool)
**
******************************************************************************/
static void PrintUsage( __in const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-?] \n\
            -r:RequestFile       - Path to the output file with public keys and company name\n\
            -p:PrivateKeyFile    - Path to the output file with private keys\n\
            -m:ManufacturerName  - Name of manufacturing company. If white space is needed in the name, use quotes. Example -m:\"My Manufacturing Company\"\n", pwszAppName);
}

/******************************************************************************
**
** Function :   DRMInit
**
** Synopsis :   Initialize the internal context data structure
**
** Arguments :  [f_ppState] - pointer to tool's state structure
**
** Returns :    DRM_SUCCESS if initialization was successful
**
******************************************************************************/
DRM_RESULT DRMInit( __inout DRMSTATE **f_ppState )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkMem ( *f_ppState = (DRMSTATE*) Oem_MemAlloc(SIZEOF(DRMSTATE)) );
    ZEROMEM( *f_ppState, SIZEOF(DRMSTATE) );
    ChkMem ( (*f_ppState)->pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc(SIZEOF(DRM_CRYPTO_CONTEXT)) );
    ZEROMEM( (*f_ppState)->pcontextCrypto, SIZEOF(DRM_CRYPTO_CONTEXT) );

    ChkMem( (*f_ppState)->pbXML = (_XMBContext *) Oem_MemAlloc( XML_KEY_GEN_INIT_SIZE ) );
    ZEROMEM( (*f_ppState)->pbXML, XML_KEY_GEN_INIT_SIZE );

    (*f_ppState)->fpDACRequest = OEM_INVALID_HANDLE_VALUE;
    (*f_ppState)->fpOEMPrivKeys = OEM_INVALID_HANDLE_VALUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRMClose
**
** Synopsis :   Closes opened files and frees the allocated memory
**
** Arguments :  [f_pState] - pointer to structute that contans current status
**                           Structure will be released
**
** Returns :
**
******************************************************************************/
void DRMClose( __in DRMSTATE **f_ppState )
{
    DRM_DWORD dwCount = 0;

    if (f_ppState == NULL || *f_ppState == NULL)
    {
        return;
    }

    if ( (*f_ppState)->fpDACRequest != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( (*f_ppState)->fpDACRequest );
    }

    if ( (*f_ppState)->fpOEMPrivKeys != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( (*f_ppState)->fpOEMPrivKeys );
    }

    SAFE_OEM_FREE( (*f_ppState)->pcontextCrypto );

    for ( dwCount = 0; dwCount < DAC_GEN_REQUEST_KEY_COUNT_ALL; dwCount++ )
    {
        SAFE_OEM_FREE( (*f_ppState)->dstrPubKey[dwCount].pwszString );

        SAFE_OEM_FREE( (*f_ppState)->dstrPrivKey[dwCount].pwszString );
    }

    SAFE_OEM_FREE( (*f_ppState)->pbXML );
    Oem_MemFree(*f_ppState);

    *f_ppState = NULL;
}

/******************************************************************************
**
** Function :   _CreateDACRequest
**
** Synopsis :   Creates files containing manufacturer's request
**
** Arguments :  [f_pState] - pointer to structute that contans current status
**
** Returns :    DRM_SUCCESS if request file was created successfully
**
** Notes :
**
**  <RequestDAC>
**      <DAC><PUBKEY>base64 encoded DAC public key</PUBKEY></DAC>
**      <FALLBACK>
**          <PUBKEY>base64 encoded FALLBACKVERIFICATION public key</PUBKEY>
**      </FALLBACK>
**  </RequestDAC>
**
******************************************************************************/
DRM_RESULT _CreateDACRequest( __in DRMSTATE *f_pState )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_STRING dstrPubkeyModulus  = EMPTY_DRM_STRING;
    DRM_STRING dstrPubkeyExponent = EMPTY_DRM_STRING;
    DRM_STRING dstrClose          = EMPTY_DRM_STRING;
    DRM_DWORD  cbWrite   = 0;
    DRM_DWORD  cbWritten = 0;
    DRM_DWORD  dwCount   = 0;

    ChkDR( DRM_XMB_CreateDocument( XML_KEY_GEN_INIT_SIZE,
                                   f_pState->pbXML,
                                   &g_dstrTagRequestDAC ) );


    for ( dwCount = 0; dwCount < NO_OF( g_dstrKeyNames ); dwCount++ )
    {
        CallXmlBuilder(f_pState->pbXML, DRM_XMB_OpenNode(f_pState->pbXML, g_dstrKeyNames[dwCount]));

        CallXmlBuilder(f_pState->pbXML,
                        DRM_XMB_WriteTag(f_pState->pbXML,
                                          &g_dstrTagDataPubKey,
                                          (DRM_CONST_STRING *)&(f_pState->dstrPubKey[dwCount]),
                                          NULL,
                                          NULL,
                                          wttClosed));
        CallXmlBuilder(f_pState->pbXML,
            DRM_XMB_CloseCurrNode(f_pState->pbXML, NULL));
    }

    /*
    ** Writes RSA public key inside <WMDRMND> tag
    */
    CallXmlBuilder( f_pState->pbXML,
        DRM_XMB_OpenNode(f_pState->pbXML, &g_dstrTagDAC_WMDRM_ND) );

    ChkDR( DRM_XML_RSA_WritePublicKeyNode( f_pState->pbXML,
                                           &g_dstrTagDataPubKey,
                                           &f_pState->pubkeyDAC_RSA1024 ) );
    CallXmlBuilder(f_pState->pbXML,
        DRM_XMB_CloseCurrNode(f_pState->pbXML, NULL));

    /*
    ** Writes manufacturer name
    */
    CallXmlBuilder( f_pState->pbXML, DRM_XMB_OpenNode( f_pState->pbXML, &g_dstrTagManufName ) );
    CallXmlBuilder( f_pState->pbXML, DRM_XMB_AddData( f_pState->pbXML, (DRM_CONST_STRING *)&(f_pState->dstrManufName) ) );
    CallXmlBuilder( f_pState->pbXML, DRM_XMB_CloseCurrNode( f_pState->pbXML, NULL ) );

    /*
    **  close the root node
    */
    CallXmlBuilder(f_pState->pbXML,
        DRM_XMB_CloseDocument(f_pState->pbXML, &dstrClose));

    cbWrite = SIZEOF( g_rgbFFFE );
    ChkBOOL( Oem_File_Write( f_pState->fpDACRequest, (DRM_BYTE*)g_rgbFFFE, cbWrite, &cbWritten )==TRUE, DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( cbWritten == cbWrite, DRM_E_FILE_WRITE_ERROR );

    cbWrite = CB_DSTR( &dstrClose );
    ChkBOOL( Oem_File_Write( f_pState->fpDACRequest, dstrClose.pwszString, cbWrite, &cbWritten )==TRUE, DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( cbWritten == cbWrite, DRM_E_FILE_WRITE_ERROR );

ErrorExit:

    SAFE_OEM_FREE( dstrPubkeyModulus.pwszString );
    SAFE_OEM_FREE( dstrPubkeyExponent.pwszString );
    return dr;
}

/******************************************************************************
**
** Function :   _CreateOEMPrivKeys
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
**      <DAC><PRIVKEY>base64 encoded DAC private key</PRIVKEY></DAC>
**      <FALLBACK>
**          <PRIVKEY>base64 encoded FALLBACKVERIFICATION private key</PRIVKEY>
**      </FALLBACK>
**      <PlayReady>
**          <PRIVKEY> base64 encoded PlayReady private key</PRIVKEY>
**      </PlayReady>
**      <WMDRMND>
**          <PRIVKEY>RSA key - several XML tags </PRIVKEY>
**      </WMDRMND>
**  </OEMPrivateKeys>
**
******************************************************************************/
DRM_RESULT _CreateOEMPrivKeys( __in DRMSTATE *f_pState )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_STRING dstrClose               = EMPTY_DRM_STRING;
    DRM_DWORD  cbWrite     = 0;
    DRM_DWORD  cbWritten   = 0;
    DRM_DWORD  dwCount     = 0;

    ChkDR( DRM_XMB_CreateDocument( XML_KEY_GEN_INIT_SIZE,
                                   f_pState->pbXML,
                                   &g_dstrTagOEMPrivateKeys ) );


    for ( dwCount = 0; dwCount < NO_OF( g_dstrKeyNames ); dwCount++ )
    {
        CallXmlBuilder(f_pState->pbXML,
            DRM_XMB_OpenNode(f_pState->pbXML, g_dstrKeyNames[dwCount]));

        CallXmlBuilder(f_pState->pbXML,
                        DRM_XMB_WriteTag(f_pState->pbXML,
                                        &g_dstrTagPrivkey,
                                        (DRM_CONST_STRING *)&(f_pState->dstrPrivKey[dwCount]),
                                         NULL,
                                         NULL,
                                         wttClosed));
        CallXmlBuilder(f_pState->pbXML,
            DRM_XMB_CloseCurrNode(f_pState->pbXML, NULL));
    }

    /*
    ** Writes RSA private key inside <WMDRMND> tag
    */
    CallXmlBuilder( f_pState->pbXML,
            DRM_XMB_OpenNode(f_pState->pbXML, &g_dstrTagDAC_WMDRM_ND) );

    ChkDR( DRM_XML_RSA_WritePrivateKeyNode( f_pState->pbXML,
                                            &g_dstrTagPrivkey,
                                            &f_pState->privkeyDAC_RSA1024 ) );

    CallXmlBuilder(f_pState->pbXML,
        DRM_XMB_CloseCurrNode(f_pState->pbXML, NULL));

    /*
    ** Writes manufacturer name
    */
    CallXmlBuilder( f_pState->pbXML, DRM_XMB_OpenNode( f_pState->pbXML, &g_dstrTagManufName ) );
    CallXmlBuilder( f_pState->pbXML, DRM_XMB_AddData( f_pState->pbXML, (DRM_CONST_STRING *)&(f_pState->dstrManufName) ) );
    CallXmlBuilder( f_pState->pbXML, DRM_XMB_CloseCurrNode( f_pState->pbXML, NULL ) );

    /*
    **  close the root node
    */
    CallXmlBuilder(f_pState->pbXML,
        DRM_XMB_CloseDocument(f_pState->pbXML, &dstrClose));

    cbWrite = SIZEOF( g_rgbFFFE );
    ChkBOOL( Oem_File_Write( f_pState->fpOEMPrivKeys, (DRM_BYTE*)g_rgbFFFE, cbWrite, &cbWritten )==TRUE, DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( cbWritten == cbWrite, DRM_E_FILE_WRITE_ERROR );

    cbWrite = CB_DSTR( &dstrClose );
    ChkBOOL( Oem_File_Write( f_pState->fpOEMPrivKeys, dstrClose.pwszString, cbWrite, &cbWritten )==TRUE, DRM_E_FILE_WRITE_ERROR );
    ChkBOOL( cbWritten == cbWrite, DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   CreateDACRequest
**
** Synopsis : Creates DAC request file
**
** Arguments : [f_pState] - pointer to structute that contans current status
**
** Returns : DRM_SUCCESS - Successfully created DAC request file
**
******************************************************************************/
DRM_RESULT CreateDACRequest( __in DRMSTATE *f_pState )
{

    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_DWORD   dwCount   = 0;

    for ( dwCount = 0; dwCount < DAC_GEN_REQUEST_KEY_COUNT_160; dwCount++ )
    {
        ChkDR(DRMTOOLS_CreateKeyPair(f_pState->pcontextCrypto,
                               eDRM_ECC_P160,
                               eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT,
                               ( DRM_BYTE * )&(f_pState->pubkey[dwCount]),
                               SIZEOF( f_pState->pubkey[dwCount] ),
                               ( DRM_BYTE * )&(f_pState->privkey[dwCount]),
                               SIZEOF( f_pState->privkey[dwCount] ),
                               &(f_pState->dstrPubKey[dwCount]),
                               &(f_pState->dstrPrivKey[dwCount])));
    }

    ChkDR( DRMTOOLS_CreateKeyPair( f_pState->pcontextCrypto,
                                   eDRM_ECC_P256,
                                   eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT,
                                   ( DRM_BYTE * )&f_pState->pubkeyDAC_P256,
                                   SIZEOF( f_pState->pubkeyDAC_P256 ),
                                   ( DRM_BYTE * )&f_pState->privkeyDAC_P256,
                                   SIZEOF( f_pState->privkeyDAC_P256 ),
                                   &f_pState->dstrPubKey[DAC_GEN_REQUEST_KEY_WMDRM_MD],
                                   &f_pState->dstrPrivKey[DAC_GEN_REQUEST_KEY_WMDRM_MD] ) );

    /* No base-64 encoding for RSA keys */
    ChkDR( DRMTOOLS_CreateKeyPair( f_pState->pcontextCrypto,
                                   eDRM_RSA,
                                   eDRM_RSA_KEY_LENGTH_1024_2048BIT,
                                   (DRM_BYTE*) &f_pState->pubkeyDAC_RSA1024,
                                   DRM_RSA_CB_PUBKEY_2048BIT,
                                   (DRM_BYTE*) &f_pState->privkeyDAC_RSA1024,
                                   DRM_RSA_CB_PRIVKEY_2048BIT,
                                   NULL,
                                   NULL ) );

    printf("Creating DAC Request...\n");
    ChkDR(_CreateDACRequest(f_pState));
    printf("Creating Private key file...\n");
    ChkDR(_CreateOEMPrivKeys(f_pState));

ErrorExit:
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
** Returns :    DRM_SUCCESS if private key file and request file were successfully created
**
******************************************************************************/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR** argv )
{
    DRM_RESULT       dr                      = DRM_SUCCESS;
    DRM_CONST_STRING dstrDACRequestFile      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOEMPrivateKeyFile   = EMPTY_DRM_STRING;
    DRM_LONG         iParam                  = 0;
    DRMSTATE        *pState                  = NULL;

    PrintCopyrightInfo( "PlayReady, PlayReady for Network Devices & WMDRM-PD Certificate Request Creation Tool" );

    /*
    **  Check input
    */
    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }
    if ( argc > 4 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    /*
    **  Initialize state
    */

    ChkDR(DRMInit(&pState));

    ChkArg( argv != NULL );
    for (iParam = 1; iParam < argc; iParam++)
    {
        DRM_WCHAR        wchOption = 0;
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

        if (!DRM_CMD_ParseCmdLine(argv[iParam], &wchOption, &dstrParam, NULL))
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        switch(DRMCRT_towlower(wchOption))
        {
        case WCHAR_CAST('r'):
            ASSIGN_DRM_STRING(dstrDACRequestFile, dstrParam);
            break;

        case WCHAR_CAST('p'):
            ASSIGN_DRM_STRING(dstrOEMPrivateKeyFile, dstrParam);
            break;

        case WCHAR_CAST('m'):
            ASSIGN_DRM_STRING(pState->dstrManufName, dstrParam);
            break;

        case WCHAR_CAST('?'):
            goto _PrintUsage;

        default:
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    if (dstrDACRequestFile.cchString    == 0
     || dstrOEMPrivateKeyFile.cchString == 0)
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    if (dstrDACRequestFile.cchString    > MAX_FILENAME
     || dstrOEMPrivateKeyFile.cchString > MAX_FILENAME)
    {
        printf("File name is too big. Max Filename is %d \n", MAX_FILENAME);
        ChkDR(DRM_E_FILE_READ_ERROR);
    }

    pState->fpDACRequest = Oem_File_Open( NULL,
                                          dstrDACRequestFile.pwszString,
                                          OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                                          OEM_FILE_SHARE_READ,
                                          OEM_CREATE_NEW,
                                          OEM_ATTRIBUTE_NORMAL );

    if ( pState->fpDACRequest == OEM_INVALID_HANDLE_VALUE )
    {
        printf("DAC Request file could not be opened\n");
        ChkDR(DRM_E_FILE_WRITE_ERROR);
    }

    pState->fpOEMPrivKeys = Oem_File_Open( NULL,
                                           dstrOEMPrivateKeyFile.pwszString,
                                           OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                                           OEM_FILE_SHARE_READ,
                                           OEM_CREATE_NEW,
                                           OEM_ATTRIBUTE_NORMAL );

    if ( pState->fpOEMPrivKeys == OEM_INVALID_HANDLE_VALUE )
    {
        printf("OEM private key file could not be opened\n");
        ChkDR(DRM_E_FILE_READ_ERROR);
    }

    printf( "Creating Request...\n" );
    ChkDR( CreateDACRequest( pState ) );
    printf( "Done.\n" );

ErrorExit:

    if (pState)
    {
        DRMClose(&pState);
    }

    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;

_PrintUsage:
    PrintUsage(argv == NULL ? &g_wchNull : argv[0]);
    return dr;
}
