/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include "SetClientID.h"
#include <oemcipher.h>
#include <tLicGen.h>

ENTER_PKTEST_NAMESPACE_CODE

/* ClientID in the clear */
CLIENTID g_ClientID;


/* Decrypt the client id blob using the server private key. This is what the license server does
    to get the client id from the encrypted license request. */
DRM_RESULT tDecryptClientID(DRM_VOID *pCryptoContext, CLIENTID *pClientID)
{
    /* This key is used for secure communication between the clients and the license server object. */
    DRM_BYTE ComLicSrvpriv[] = { 0x23, 0x45, 0xae, 0xb5, 0x91, 0x2a, 0x35, 0xbc,
        0xd7, 0xc5, 0x87, 0xc3, 0x3b, 0xa5, 0x75, 0x21, 0xf4, 0xec,
        0x53, 0xae, 0xeb, 0x7e, 0x98, 0x00};

    DRM_RESULT dr;
    long lIndex;
    PRIVKEY    privacyKey;
/*TWO_BYTES Macro is used to make it compatible with 16-bit. Only one byte is of our use, but since one word is 2 bytes on 16 bit platform, we need to use the TWO_BYTES macro*/
    DRM_BYTE  pdeb[] = {0x37, 0x37};
    DRM_BYTE  plainText[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )] = { 0 };   /* Used for encryption */
    DRM_DWORD cbPlainText = DRM_ECC160_PLAINTEXT_LEN;
    RC4_KEYSTRUCT rc4ks; /* For use with RC4. */

    /*Devide the offset by CB_NATIVE_BYTE since each word has 2 bytes on 16-bit platform*/
    /* reveal the private key */
   MEMCPY(&privacyKey, ComLicSrvpriv+(2/CB_NATIVE_BYTE), DRM_ECC160_PRIVATE_KEY_LEN );
    for (lIndex = 0; lIndex < DRM_ECC160_PRIVATE_KEY_LEN/CB_NATIVE_BYTE; lIndex++)
    {
        privacyKey.x[lIndex] = (DRM_BYTE)( privacyKey.x[lIndex] ^ pdeb[0] );
    }

    /* decrypt the rc4 key */
    ChkDR(DRM_PK_Decrypt(pCryptoContext,
                         eDRM_ECC_P160,
                         DRM_ECC160_PRIVATE_KEY_LEN,
                         ( const DRM_BYTE * )&privacyKey,
                         DRM_ECC160_CIPHERTEXT_LEN,
                         pClientID->randNum,
                         &cbPlainText,
                         plainText));

    DRM_RC4_KeySetup(&rc4ks, DRMCIPHERKEYLEN_RC4, plainText);
    DRM_RC4_Cipher(&rc4ks, SIZEOF(PKCERT), (DRM_BYTE*)&(pClientID->pk));
ErrorExit:
    return dr;
}

DRM_RESULT TestLicResponseSetClientID(const CLIENTID *pClientID)
{
    DRM_RESULT dr;
    DRM_BYTE *rgbCryptoContext = NULL;
    ChkArg(pClientID);

    ChkMem(rgbCryptoContext = (DRM_BYTE*)Oem_MemAlloc(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    MEMSET(rgbCryptoContext, 0, DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE);

    MEMCPY(&g_ClientID, pClientID, SIZEOF(CLIENTID));
    ChkDR(tDecryptClientID(rgbCryptoContext, &g_ClientID)); /* ClientID is in the clear. */


ErrorExit:

    SAFE_OEM_FREE(rgbCryptoContext);

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

