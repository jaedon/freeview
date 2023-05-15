/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    TestManagerEncrypt
**
** Synopsis:    Encrypts random data using current DECRYPT_CONTEXT_VAR test variable
**              and stores the result in ENCRYPTED_DATA_VAR test variable
**
** Arguments:   argv[0] : Cipher type: RC4 or AES 
**              argv[1] : Data length
**              argv[2] : Key to use (if NULL or not specified, will use default) (optional)
**              argv[3] : Reset IV for counter-mode context (optional). If NULL, use default.
**              argv[4] : Reset Byte offset for counter-mode context (optional).
**              argv[5] : Fetch the content key from this variable
**                        (defaults to RC4_KEY_VAR or AES_KEY_VAR for RC4 or AES respectively, ignored if argv[2] is set)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: Args 3-4 are AES-only
**
** These have the form ###-### where the first set of digits is the high dword and the second set 
** is the low dword. If the character after the first se is not '-' then the first set is treated
** as the low DWORD and the high DWORD is 0.
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerEncrypt( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr                                       = DRM_SUCCESS;
    DRM_CHAR  *pszTemp                                  = NULL;
    DRM_BYTE   rgbKey[__CB_DECL( DRMCIPHERKEYLEN_MAX )] = { 0 };

    /*
    ** These are to reference shared test vars
    */
    DRM_DWORD                    *pcbData          = NULL;
    DRM_BYTE                     *pbData           = NULL;
    DRM_BYTE                     *pbEncryptedData  = NULL;
    DRM_DECRYPT_CONTEXT          *poDecryptContext = NULL;
    DRM_BYTE                     *prgbSharedKey    = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT *pAesCtrContext   = NULL;

    /*
    ** 2 args are required
    */
    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    /*
    ** Get shared test vars
    */
    MAP_TO_GLOBAL_TEST_VAR( pbData, DRMMGR_READER_DATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcbData, DRMMGR_READER_DATA_SIZE_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, ENCRYPTED_DATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( poDecryptContext, DECRYPT_CONTEXT_VAR );
    
    /*
    ** Get Data Length
    */ 
    *pcbData = strtoul( argv[1], &pszTemp, 0 );
    ChkArg( pszTemp != argv[1] );

    /*
    ** Encrypt random data
    */
    Oem_Random_GetBytes( NULL, pbData, *pcbData );    
    MEMCPY( pbEncryptedData, pbData, *pcbData );
    
    /*
    ** Check for optional args
    */
    if ( argc >= 3 && argv[2] != NULL )
    {
        /*
        ** Get Key
        */
        DRM_SUBSTRING dasstrTemp = { 0 };
        DRM_DWORD     cbTemp     = DRMCRT_strlen( argv[2] );

        dasstrTemp.m_cch = cbTemp;
        
        cbTemp = SIZEOF( rgbKey );
        
        ChkDR( DRM_B64_DecodeA( argv[2], &dasstrTemp, &cbTemp, rgbKey, 0 ) );
    }    
    
    if ( strncmp( argv[0], "RC4", 3 ) == 0 )
    {
        if( argc < 3 || argv[2] == NULL )
        {
            /*
            ** Get shared RC4 key (if it won't be overridden by argv[2])
            */
            MAP_TO_GLOBAL_TEST_VAR( prgbSharedKey, ( argc >= 6 && argv[5] != NULL ) ? argv[5] : RC4_KEY_VAR );
        }

        /*
        ** Encrypt using RC4
        */
        ChkDR( DRM_CPHR_Init( ( DRM_CIPHER_CONTEXT * )poDecryptContext,
                              eDRM_RC4_CIPHER,
                              DRMCIPHERKEYLEN_RC4,
                              ( argc >= 3 && argv[2] != NULL ) ? rgbKey : prgbSharedKey ) );        
        
        ChkDR( DRM_CPHR_Encrypt( ( DRM_CIPHER_CONTEXT * )poDecryptContext,
                                 NULL,
                                 *pcbData,
                                 pbEncryptedData ) );       
    }
    else if ( strncmp( argv[0], "AES", 3 ) == 0 )
    {
        if( argc < 3 || argv[2] == NULL )
        {
            /*
            ** Get shared AES Key (if it won't be overridden by argv[2])
            */
            MAP_TO_GLOBAL_TEST_VAR( prgbSharedKey, ( argc >= 6 && argv[5] != NULL ) ? argv[5] : AES_KEY_VAR );
        }

        /*
        ** Get shared AES Context
        */
        MAP_TO_GLOBAL_TEST_VAR( pAesCtrContext, AES_COUNTER_MODE_CONTEXT_VAR );

        ChkDR( DRM_CPHR_Init( ( DRM_CIPHER_CONTEXT * )poDecryptContext,
                              eDRM_AES_COUNTER_CIPHER,
                              DRM_AES_KEYSIZE_128,
                              ( argc >=3 && argv[2] != NULL ) ? rgbKey : prgbSharedKey ) );

        
        /*
        ** Counter-mode parameters
        */
        if ( argc >= 4 && argv[3] != NULL )
        {
            ChkDR( TST_UTL_AtoUI64( argv[3], DRMCRT_strlen( argv[3] ), &(pAesCtrContext->qwInitializationVector) ) );
        }
        else
        {
            pAesCtrContext->qwInitializationVector = g_qwDefaultInitializationVector;
        }
        if ( argc >= 5 && argv[4] != NULL )
        {
            ChkDR( TST_UTL_AtoUI64( argv[4], DRMCRT_strlen( argv[4] ), &(pAesCtrContext->qwBlockOffset) ) );
        }        
        else
        {
            pAesCtrContext->qwBlockOffset = DRM_UI64( 0 );
        }        
        pAesCtrContext->bByteOffset = 0x00;

        ChkDR( DRM_CPHR_Encrypt( ( DRM_CIPHER_CONTEXT * )poDecryptContext,
                                 pAesCtrContext,
                                 *pcbData,
                                 pbEncryptedData ) );
    }
    else
    {
        ChkArg( FALSE );
    }
        
 ErrorExit:        
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
