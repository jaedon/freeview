/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
 * DrmXmr.c
 *
 * This file implements the basic XMR License unpacking
 */

#include "drmcommon.h"
#include "NdtDrmXmr.h"
#include "oemrsaoaeppss.h"
#include "oemaes.h"
#include "drmcrt.h"
#include "ndttypes.h"
#include "oemndt.h"
#include "ndtnetmsg.h"
#include "drmxmr.h"

ENTER_PKTEST_NAMESPACE_CODE

#define DRM_XMR_CHAINED_CHECKSUM_LENGTH                 16
#define DRM_XMR_CHAINED_CHECKSUM_SHA_ITERATIONS         5

static DRM_RESULT
_InternalDrmXmrGetKeyFromLicenseEx(
    __in       DRM_BOOL      bRSAPrivateKey,
    __in const DRM_BYTE     *pbDecryptionKey,
    __in const XMR_LICENSE  *pXmrLicense,
    __in       DRM_BOOL      bGetContentEncryptionKey,
    __out_opt  DRM_ID       *pIDChainedChecksum,
    __out      DRM_AES_KEY **ppAesKey
    )
{
    DRM_RESULT dr;

    DRM_BYTE           *pbAesKey   = NULL;
    DRM_DWORD           cbAesKey   = 0;
    DRM_CRYPTO_CONTEXT *pCryptoCtx = NULL;

    if( pXmrLicense == NULL || ppAesKey == NULL ) {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    ChkMem( *ppAesKey = (DRM_AES_KEY *)Oem_MemAlloc( SIZEOF( DRM_AES_KEY ) ) );


    /*
    ** Allocate the crypto Context
    */
    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );

    /*
     * Ensure there is an encrypted key in the License
     */
    if ( pXmrLicense->km.ck.pbEncryptedKey == NULL ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrGetKeyFromLicense: No encrypted key in the License.\n" ));
        goto ErrorExit;
    }

    /*
     * Ensure the key is a type we understand
     */

    if ( pXmrLicense->km.ck.wSymmetricCipherType != XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR
     &&  pXmrLicense->km.ck.wSymmetricCipherType != XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrGetKeyFromLicense: Symmetric cipher type is invalid\n" ));
        goto ErrorExit;
    }
    if ( bRSAPrivateKey && pXmrLicense->km.ck.wAsymmetricCipherType != XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024 ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrGetKeyFromLicense: Asymmetric cipher type is invalid (private key and asymmetric encryption not rsa 1024)\n" ));
        goto ErrorExit;
    }
    if ( !bRSAPrivateKey && pXmrLicense->km.ck.wAsymmetricCipherType != XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrGetKeyFromLicense: Asymmetric cipher type is invalid (not private key and asymmetric encryption not chained)\n" ));
        goto ErrorExit;
    }

    /*
     * Decrypt all of the keys
     */
    if ( bRSAPrivateKey )
    {
        ChkMem( pbAesKey = (DRM_BYTE *)Oem_MemAlloc( __CB_DECL( DRM_RSA_CB_MAX_PLAINTEXT_MAX_2048BIT ) ) );
        cbAesKey = DRM_RSA_CB_MAX_PLAINTEXT_MAX_2048BIT;

        dr = OEM_RSA_OaepDecrypt_2048BIT((const DRM_RSA_PRIVATE_KEY_2048BIT *)pbDecryptionKey,
                                         pXmrLicense->km.ck.cbEncryptedKey,
                                         pXmrLicense->km.ck.pbEncryptedKey,
                                         &cbAesKey,
                                         pbAesKey,
                                         pCryptoCtx );
    } else {
        pbAesKey = (DRM_BYTE *)Oem_MemAlloc( DRM_AES_KEYSIZE_128 * 2 );
        ChkMem( pbAesKey );

        cbAesKey = DRM_AES_KEYSIZE_128 * 2;

        if ( pXmrLicense->km.ck.cbEncryptedKey != DRM_AES_KEYSIZE_128 * 2 ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            goto ErrorExit;
        }

        MEMCPY(pbAesKey, pXmrLicense->km.ck.pbEncryptedKey, DRM_AES_KEYSIZE_128 * 2 );

        dr = Oem_Aes_EcbDecryptData(
                                  (const DRM_AES_KEY *)pbDecryptionKey,
                                  pbAesKey,
                                  DRM_AES_KEYSIZE_128 * 2 );
    }

    if ( dr != DRM_SUCCESS ) {
        TRACE(( "DrmXmrGetKeyFromLicense: Cannot decrypt AES keys.\n" ));
        goto ErrorExit;
    }

    /*
     * Ensure the decrypted keys are long enough
     */
    if ( cbAesKey < DRM_AES_KEYSIZE_128*2 ) {
        TRACE(( "DrmXmrGetKeyFromLicense: AES key is too small.\n" ));
        goto ErrorExit;
    }

    /*
     * Calculate chained checksum, if requested.
     */
    if (pIDChainedChecksum){
        DRM_SHA1_CONTEXT ShaCtx;
        DRM_BYTE rgcChainedChecksum[ DRM_SHA1_DIGEST_LEN +1 ];
        int i;

        MEMSET( rgcChainedChecksum, 0, SIZEOF(rgcChainedChecksum) );
        MEMCPY( rgcChainedChecksum, pbAesKey, DRM_AES_KEYSIZE_128 );

        for(i=0; i<DRM_XMR_CHAINED_CHECKSUM_SHA_ITERATIONS; i++) {
            DRM_SHA1_Init( &ShaCtx );
            DRM_SHA1_Update( &rgcChainedChecksum[0], DRM_SHA1_DIGEST_LEN+1, &ShaCtx );
            DRM_SHA1_Finalize( &ShaCtx, &rgcChainedChecksum[0] );
        }

        MEMCPY( pIDChainedChecksum, &rgcChainedChecksum[0], DRM_AES_KEYSIZE_128 );
    }

    /*
     * Get the correct key from the decrypted buffer
     */

    Oem_Aes_SetKey( (bGetContentEncryptionKey? pbAesKey : pbAesKey+__CB_DECL(DRM_AES_KEYSIZE_128)), *ppAesKey );

    /*
     * Success
     */
    dr = DRM_SUCCESS;

ErrorExit:

    /*
     * Free locally used resources
     */

    if ( pbAesKey != NULL ) {
        MEMSET( pbAesKey, 0, cbAesKey );
        Oem_MemFree( pbAesKey );
    }
    if( DRM_FAILED( dr )
            && ppAesKey != NULL
            && *ppAesKey != NULL )
    {
            Oem_Aes_ZeroKey( *ppAesKey );
    }

    SAFE_OEM_FREE( pCryptoCtx );

    return dr;
}


/*
 * The DrmXmrGetKeyFromLicense gets the content encryption or integrity key from the XMR License
 *
 * Parameters:
 *
 *      pPrivateKey - Specifies the RSA private key used to decrypt the key (as returned from DrmRsaSetPrivateKey)
 *          NULL indicates that the private key baked into the device should be used
 *
 *      pXmrLicense - Specifies the XMR License that contains the encrypted key
 *
 *      bGetContentEncryptionKey - Specify TRUE to get the content encryption key
 *          Specify FALSE to get the content integrity key
 *
 *      ppAesKey - Returns the requested AES key
 *          The buffer must be freed using DrmAesDeleteKey()
 *
 * Return Value:
 *  DRM_SUCCESS - The operation completed successfully
 *  DRM_E_INVALIDARG - One of the parameters is invalid
 *  DRM_E_ND_INVALID_MESSAGE - The key is not in the License or is a type not understood
 *  DRM_E_OUTOFMEMORY
 */

DRM_RESULT
DrmXmrGetKeyFromLicense(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey,
    __in const XMR_LICENSE *pXmrLicense ,
    __in DRM_BOOL bGetContentEncryptionKey,
    __out DRM_AES_KEY **ppAesKey
    )
{
    return _InternalDrmXmrGetKeyFromLicenseEx(TRUE,
                                              (const DRM_BYTE *)pPrivateKey,
                                              pXmrLicense,
                                              bGetContentEncryptionKey,
                                              NULL,
                                              ppAesKey);
}

DRM_RESULT
DrmXmrGetKeyFromLicenseEx(
    __in DRM_BOOL bRSAPrivateKey,
    __in const DRM_BYTE *pbDecryptionKey,
    __in const XMR_LICENSE *pXmrLicense ,
    __in DRM_BOOL bGetContentEncryptionKey,
    __out DRM_AES_KEY **ppAesKey
    )
{
    return _InternalDrmXmrGetKeyFromLicenseEx(bRSAPrivateKey,
                                              pbDecryptionKey,
                                              pXmrLicense,
                                              bGetContentEncryptionKey,
                                              NULL,
                                              ppAesKey);
}

static DRM_RESULT
_InternalDrmXmrUnpackLicense(
    __in DRM_BOOL bChainedLicense,
    __in DRM_BOOL bLeafLicense,
    __in_opt XMR_ROOT_LICENSE_INFO *pRootLicenseInfo,
    __in const DRM_BYTE *pbDecryptionKey,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_LICENSE **ppXmrLicense
    )
{
    DRM_RESULT dr;

    /*
     * Parsed parts of the message
     */
    DRM_DWORD dwMagicConstant;
    DRM_DWORD dwVersion;
    DRM_WORD wFlags;
    DRM_WORD wObjectType;
    DRM_DWORD dwObjectSize;
    DRM_BYTE *pChainedChecksum;

    DRM_WORD wSignatureType;
    DRM_WORD cbSignature;
    DRM_BYTE *pbSignature = NULL;
    DRM_DWORD cbSignedMessage;

    DRM_BYTE *pbUnknown;

    DRM_BYTE *pbSignedMessage = NULL;

    DRM_AES_KEY *pAesKey = NULL;

    /*
     * Define characteristics that we know about the object types we understand
     */

    /*
     * Define which objects are optional
     *  Note: If all versions are included,
     *  XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT and XMR_OBJECT_TYPE_SECURITY_LEVEL
     *  are both "optional" where exactly one must be specified,
     *  but because this code requires V1 licenses, XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT
     *  is the only one of those two objects which can be specified.
     *  Therefore, XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT is marked as NON-optional
     *  while XMR_OBJECT_TYPE_SECURITY_LEVEL is marked as OPTIONAL in this array.
     */
    DRM_BOOL bIsOptional[] = {
        TRUE,   /* XMR_OBJECT_TYPE_INVALID */
        FALSE,  /* XMR_OBJECT_TYPE_OUTER_CONTAINER */
        FALSE,  /* XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER */
        FALSE,  /* XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT */
        FALSE,  /* XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
        FALSE,  /* XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_UPLINK_KID_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT */
        FALSE,  /* XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER */
        FALSE,  /* XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT */
        FALSE,  /* XMR_OBJECT_TYPE_SIGNATURE_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_SETTINGS_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_PRIORITY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EXPIRATION_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_ISSUEDATE_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_METERING_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT */
        TRUE,   /* <deprecated> */
        TRUE,   /* <deprecated> */
        TRUE,   /* XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_COPYCOUNT_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT */
        TRUE,   /* <deprecated> */
        TRUE,   /* <deprecated> */
        TRUE,   /* XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_SOURCEID_OBJECT */
        TRUE,   /* <deprecated> */
        TRUE,   /* <deprecated> */
        TRUE,   /* XMR_OBJECT_TYPE_REVOCATION_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_USERID_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_SECURITY_LEVEL */
        TRUE,   /* XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_READ_POLICY_CONTAINER> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_42> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_43> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_44> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_45> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_46> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_47> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_48> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_49> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4A> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4B> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4C> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4D> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4E> */
        TRUE,   /* <reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4F> */
        TRUE,   /* XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_AUX_KEY_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_UPLINKX_OBJECT */
        TRUE,   /* XMR_OBJECT_TYPE_INVALID_RESERVED_53 */
        TRUE,   /* XMR_OBJECT_TYPE_APPLICATION_ID_LIST */
        TRUE,   /* XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION */
        TRUE,   /* XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER */
        TRUE,   /* XMR_OBJECT_TYPE_ND_TX_AUTH_OBJECT */
    };


    /*
     * Count of the number of any particular object found
     */
    DRM_BOOL bObjectFound[XMR_OBJECT_TYPE_MAXIMUM_DEFINED+1];

    /*
     * Maintain a stack of the current nested object type
     */
#define XMR_MAX_NESTING_LEVEL 4 /* invalid + outer + playback + Explicit */
    DRM_WORD wContainerType[XMR_MAX_NESTING_LEVEL];
    DRM_DWORD EndOfThisContainerOffset[XMR_MAX_NESTING_LEVEL];
    DRM_LONG lContainerStackIndex = 0;


    /*
     * The message itself
     */
    XMR_LICENSE *pXmrLicense = NULL;
    XMR_LICENSE *pXmrRootLicense = NULL;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    /*
    ** Ensure that we have initializers for all of the defined object types
    */
    DRMCASSERT( NO_OF( bIsOptional ) == XMR_OBJECT_TYPE_MAXIMUM_DEFINED+1 );


    if( ppXmrLicense == NULL ) {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    if ( bLeafLicense ) {
        /*
         * pRootLicenseInfo must be present
         */
        if ( pRootLicenseInfo == NULL ) {
            dr = DRM_E_INVALIDARG;
            goto ErrorExit;
        }

        pXmrRootLicense = pRootLicenseInfo->pXmrLicense;

        /*
         * Uplink KID is required for leaf licenses
         */
        bIsOptional[XMR_OBJECT_TYPE_UPLINK_KID_OBJECT] = FALSE;
    }

    /*
     * Initialization
     */
    *ppXmrLicense = NULL;
    MEMSET( bObjectFound, 0, sizeof(bObjectFound) );

    /*
     * Allocate a descriptor for parsing the message
     */
    ChkDR( WmdrmNetAllocateMessageDescriptor( Message, MessageSize, &MessageDescriptor ) );

    /*
     * Define a dummy object on top of the object container stack
     */
    lContainerStackIndex = 0;
    wContainerType[0] = XMR_OBJECT_TYPE_INVALID;
    EndOfThisContainerOffset[0] = MessageDescriptor->BufferSize;

    /*
     * Allocate the returned XMR License structure
     */
    pXmrLicense = (XMR_LICENSE *)Oem_MemAlloc( SIZEOF(XMR_LICENSE) );
    ChkMem( pXmrLicense );

    MEMSET( pXmrLicense, 0, sizeof(XMR_LICENSE) );

    /*
     * Check the magic constant
     */
    if ( !WmdrmNetUnpackDword( MessageDescriptor, &dwMagicConstant) ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrUnpackLicense: License too short (magic constant)\n" ));
        goto ErrorExit;
    }
    if ( dwMagicConstant != XMR_MAGIC_CONSTANT ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrUnpackLicense: License magic constant wrong.\n" ));
        goto ErrorExit;
    }

    /*
     * Check the version
     */
    if ( !WmdrmNetUnpackDword( MessageDescriptor, &dwVersion) ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrUnpackLicense: License too short (version)\n" ));
        goto ErrorExit;
    }
    if ( dwVersion != XMR_VERSION_1 ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrUnpackLicense: License version wrong.\n" ));
        goto ErrorExit;
    }

    /*
     * Get the rights ID
     */
    if ( !WmdrmNetUnpackVal128( MessageDescriptor, &pXmrLicense->RightsId) ) {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "DrmXmrUnpackLicense: License too short (rights ID)\n" ));
        goto ErrorExit;
    }

    /*
     * Loop processing each XMR object
     *
     * Loop until we popped the "entire message" container off the stack
     */
    while ( lContainerStackIndex >= 0 ) {
        DRM_DWORD dwBytesLeftInContainer;

        /*
         * Compute the number of bytes left in the container
         */
        dwBytesLeftInContainer = (DRM_DWORD)(EndOfThisContainerOffset[lContainerStackIndex] - MessageDescriptor->Offset);

        /*
         * Get the base object descriptor
         */
#define XMR_BASE_SIZE 8
        if ( !WmdrmNetUnpackWord( MessageDescriptor, &wFlags) ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: License too short (flags)\n" ));
            goto ErrorExit;
        }
        if ( !WmdrmNetUnpackWord( MessageDescriptor, &wObjectType) ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: License too short (Object type)\n" ));
            goto ErrorExit;
        }
        if ( !WmdrmNetUnpackDword( MessageDescriptor, &dwObjectSize) ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: License too short (Object Size)\n" ));
            goto ErrorExit;
        }

        /*
         * Ensure the object fits into the current container
         */
        if ( dwObjectSize > dwBytesLeftInContainer ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: Object Size extends past end of container\n" ));
            goto ErrorExit;
        }

        /*
         * Adjust the object size to not include the base object descriptor
         */
        if ( dwObjectSize < XMR_BASE_SIZE ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: Object Size must include object descriptor.\n" ));
            goto ErrorExit;
        }
        dwObjectSize -= XMR_BASE_SIZE;


        /* Skip objects we don't know about and don't need to understand */
        if( ( XMR_OBJECT_TYPE_INVALID == g_xmrCommonDefines[wObjectType].wParent ) &&
            ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0 )
        {
            /*
             * If the object was the last object in its container,
             *  pop back to the previous container
             */
            while ( lContainerStackIndex >= 0 &&
                    (MessageDescriptor->Offset) == EndOfThisContainerOffset[lContainerStackIndex] )
            {

                TRACE(( "DrmXmrUnpackLicense: Container Popped: %2.2lx %8.8lx\n", wContainerType[lContainerStackIndex], EndOfThisContainerOffset[lContainerStackIndex] ));
                lContainerStackIndex --;

            }

            /*
             * Handle objects that are too big for their container
             */

            if ( lContainerStackIndex >= 0 &&
                 MessageDescriptor->Offset > EndOfThisContainerOffset[lContainerStackIndex] ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx is too large for its container. 0x%8.8lx\n", wObjectType, MessageDescriptor->Buffer+MessageDescriptor->Offset ));
                goto ErrorExit;
            }
            continue;
        }

        /*
         * Do extra checking on objects we know about
         */
        if ( wObjectType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED )
        {
            /*
             * Ensure the object is correctly marked as a container or not
             */
            if ( g_xmrCommonDefines[wObjectType].fIsContainer ) {
                if ((wFlags & XMR_FLAGS_CONTAINER) == 0 ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Object %2.2lx is a container and isn't so marked\n", wObjectType ));
                    goto ErrorExit;
                }
            } else {
                if ((wFlags & XMR_FLAGS_CONTAINER) != 0 ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Object %2.2lx isn't a container but is marked as one\n", wObjectType ));
                    goto ErrorExit;
                }
            }

            /*
             * Ensure the object has the correct parent
             */
            if ( g_xmrCommonDefines[wObjectType].wParent != wContainerType[lContainerStackIndex] ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx has %2.2lx as a parent instead of %2.2lx.\n",
                        wObjectType, wContainerType[lContainerStackIndex], g_xmrCommonDefines[wObjectType].wParent ));
                goto ErrorExit;
            }

            /*
            ???
             * Ensure that we find only one instance of each leaf object
             */
            if ( !g_xmrCommonDefines[wObjectType].fIsContainer && wObjectType != XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT ) {
                if ( bObjectFound[wObjectType] ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Object %2.2lx cannot appear multiple times.\n", wObjectType ));
                    goto ErrorExit;
                }
            }
            bObjectFound[wObjectType] = TRUE;
        }

        /*
         * Handle each object type
         *
         * Handle the containers
         */
        switch ( wObjectType ) {
        case XMR_OBJECT_TYPE_OUTER_CONTAINER:
        case XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER:
        case XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER:
        case XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER:
        case XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER:

            /*
             * Ensure this container isn't nest too deeply
             */
            if ( lContainerStackIndex+1 >= XMR_MAX_NESTING_LEVEL ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx internal error.  Nested too deeply.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Push this container onto the nested container stack
             */
            lContainerStackIndex++;
            wContainerType[lContainerStackIndex] = wObjectType;
            EndOfThisContainerOffset[lContainerStackIndex] = MessageDescriptor->Offset + dwObjectSize;

            break;

        /*
         * Handle the minimum environment object
         */
        case XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT:

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != SIZEOF(DRM_WORD) + 2*SIZEOF(DRM_DWORD)) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the fields
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->gr.wMinAppSecurityLevel) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Min App Security Level)\n" ));
                goto ErrorExit;
            }
            if ( !WmdrmNetUnpackDword( MessageDescriptor, &pXmrLicense->gr.dwReserved1) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Reserved Field 1)\n" ));
                goto ErrorExit;
            }
            if ( !WmdrmNetUnpackDword( MessageDescriptor, &pXmrLicense->gr.dwReserved2) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Reserved Field 2)\n" ));
                goto ErrorExit;
            }

            /*
             * Check that the leaf security level is equal to or less than the root
             */
            if ( bLeafLicense && (pXmrLicense->gr.wMinAppSecurityLevel > pXmrRootLicense->gr.wMinAppSecurityLevel) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: leaf security level not less than or equal to root\n" ));
                goto ErrorExit;
            }

            break;


        /*
         * Handle the device identification object
         */
        case XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT:

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != SIZEOF(DRM_ID)) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the fields
             */
            if ( !WmdrmNetUnpackVal128( MessageDescriptor, &pXmrLicense->gr.SerialNumber) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Serial Number)\n" ));
                goto ErrorExit;
            }

            break;

        /*
         * Handle the XMR settings object
         */
        case XMR_OBJECT_TYPE_SETTINGS_OBJECT:

            /*


             * Ensure the size is right
             */
            if ( dwObjectSize != SIZEOF(DRM_WORD)) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the fields
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->gr.wReserved) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Reserved Field)\n" ));
                goto ErrorExit;
            }

            break;

        /*
         * Handle the source ID object
         */
        case XMR_OBJECT_TYPE_SOURCEID_OBJECT:

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != SIZEOF(DRM_DWORD)) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the field
             */
            if ( !WmdrmNetUnpackDword( MessageDescriptor, &pXmrLicense->gr.dwSourceId) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Source ID)\n" ));
                goto ErrorExit;
            }

            break;

        /*
         * Handle the inclusion list object
         */
        case XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT:
        {
            DRM_UINT64 cbGUIDs;

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize < SIZEOF(DRM_DWORD) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the number of guids in the list
             */
            if ( !WmdrmNetUnpackDword( MessageDescriptor, &pXmrLicense->gr.il.cGUIDS) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (cGUIDs)\n" ));
                goto ErrorExit;
            }

            /*
             * Now that the number of guids is known, ensure the size is right again.  Guard against overflow
             */
            cbGUIDs = DRM_UI64Mul( DRM_UI64(SIZEOF(DRM_GUID)), DRM_UI64(pXmrLicense->gr.il.cGUIDS) );
            if ( !DRM_UI64Les( DRM_UI64Add(DRM_UI64(SIZEOF(DRM_DWORD)), cbGUIDs), DRM_UI64HL(1, 0) ) )
            {
                dr = DRM_E_ARITHMETIC_OVERFLOW;
                TRACE(( "DrmXmrUnpackLicense: Arithmetic overflow due to excessive GUID Entries\n" ));
                goto ErrorExit;
            }

            if ( dwObjectSize != (SIZEOF(DRM_DWORD) + DRM_UI64Low32(cbGUIDs)) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the GUIDS
             */
            DRMSIZEASSERT( SIZEOF(DRM_ID), SIZEOF(DRM_GUID) );
            if ( !WmdrmNetUnpackOctets( MessageDescriptor, (DRM_BYTE **) &pXmrLicense->gr.il.pGUIDS, DRM_UI64Low32(cbGUIDs) ) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (pGUIDS)\n" ));
                goto ErrorExit;
            }
#if TARGET_LITTLE_ENDIAN == 0
            {
                DRM_DWORD i = 0;
                for( i = 0; i < pXmrLicense->gr.il.cGUIDS; i++)
                {
                    FIX_ENDIAN_DWORD( pXmrLicense->gr.il.pGUIDS[i].Data1 );
                    FIX_ENDIAN_WORD( pXmrLicense->gr.il.pGUIDS[i].Data2 );
                    FIX_ENDIAN_WORD( pXmrLicense->gr.il.pGUIDS[i].Data3 );
                }
            }
#endif
        }
        break;

        /*
         * Handle the Device Key
         */
        case XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT:
        {
            /*
             * Ensure the size is right
             */
            if ( dwObjectSize < (SIZEOF(DRM_DWORD) + SIZEOF(DRM_WORD)) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the Exponent
             */
            if ( !WmdrmNetUnpackDword( MessageDescriptor, &pXmrLicense->km.dk.dwExponent) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (DEVICE KEY Exponent)\n" ));
                goto ErrorExit;
            }

            /*
             * Unpack the Modulus Length
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->km.dk.wModulusLength) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (DEVICE KEY Modulus Length)\n" ));
                goto ErrorExit;
            }

            /*
             * Now that the Modulus Length is known, ensure the size is right again
             */
            if ( dwObjectSize < (SIZEOF(DRM_DWORD) + SIZEOF(DRM_WORD) + pXmrLicense->km.dk.wModulusLength) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the Modulus
             */
            if ( !WmdrmNetUnpackOctets( MessageDescriptor, &pXmrLicense->km.dk.pbModulus, pXmrLicense->km.dk.wModulusLength) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (DEVICE KEY Modulus)\n" ));
                goto ErrorExit;
            }
        }

        break;

        /*
         * Handle the output protection object
         */
        case XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT:

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != 5*SIZEOF(DRM_WORD)) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the fields
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->mopl.wReserved1) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Reserved Field 1)\n" ));
                goto ErrorExit;
            }
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->mopl.wUncompressedDigitalVideo) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Uncomp Dig Vid)\n" ));
                goto ErrorExit;
            }
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->mopl.wAnalogVideo) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Analog Vid)\n" ));
                goto ErrorExit;
            }
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->mopl.wReserved2) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Reserved Field 2)\n" ));
                goto ErrorExit;
            }
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->mopl.wUncompressedDigitalAudio) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Uncomp Dig Aud)\n" ));
                goto ErrorExit;
            }

            break;

        /*
         * Handle the video output configuration object
         */
        case XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT:
            {
                XMR_VIDEO_OUTPUT_PROTECTION * pVop;
                DRM_UINT64 cbVop;

                /*
                 * Ensure the size is large enough
                 */
                if ( dwObjectSize < SIZEOF(DRM_ID) ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                    goto ErrorExit;
                }

                /*
                 * Allocate a bigger (or initial) array to put the entry in. Guard against overflow
                 */
                cbVop = DRM_UI64Mul( DRM_UI64(SIZEOF(XMR_VIDEO_OUTPUT_PROTECTION)), DRM_UI64(pXmrLicense->vopi.cEntries) );
                cbVop = DRM_UI64Add( DRM_UI64(SIZEOF(XMR_VIDEO_OUTPUT_PROTECTION)), cbVop );
                if ( !DRM_UI64Les( cbVop, DRM_UI64HL(1, 0) ) )
                {
                    dr = DRM_E_ARITHMETIC_OVERFLOW;
                    TRACE(( "DrmXmrUnpackLicense: Arithmetic overflow due to excessive Video Output Configuration Entries\n" ));
                    goto ErrorExit;
                }

                pXmrLicense->vopi.rgVop = (XMR_VIDEO_OUTPUT_PROTECTION *)Oem_MemAlloc(DRM_UI64Low32(cbVop) );
                ChkMem( pXmrLicense->vopi.rgVop );

                pVop = &pXmrLicense->vopi.rgVop[pXmrLicense->vopi.cEntries];
                pVop->cbConfigData = 0;
                pVop->pbConfigData = NULL;
                pXmrLicense->vopi.cEntries ++;

                /*
                 * Unpack the GUID of the entry
                 */
                DRMSIZEASSERT( SIZEOF(DRM_ID), SIZEOF(DRM_GUID) );
                if ( !WmdrmNetUnpackVal128( MessageDescriptor, (DRM_ID *)&pVop->guidId) ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: License too short (Vid Config Guid)\n" ));
                    goto ErrorExit;
                }

                /*
                 * Handle any configuration data left over
                 */
                if ( dwObjectSize > SIZEOF(DRM_ID) ) {

                    /*
                     * Determine size of the configuration data
                     */
                    pVop->cbConfigData = dwObjectSize - SIZEOF(DRM_ID);

                    /*
                     * Unpack the configuration data
                     */
                    if ( !WmdrmNetUnpackOctets( MessageDescriptor, (DRM_BYTE **) &pVop->pbConfigData, pVop->cbConfigData) ) {
                        dr = DRM_E_ND_INVALID_MESSAGE;
                        TRACE(( "DrmXmrUnpackLicense: License too short (Vid Config data)\n" ));
                        goto ErrorExit;
                    }
                }
            }

            break;

        /*
         * Unpack the encrypted keys
         */
        case XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT:
            /*
             * Get the key ID
             */
            DRMSIZEASSERT( sizeof(DRM_ID), sizeof(DRM_GUID) );
            if ( !WmdrmNetUnpackVal128( MessageDescriptor, (DRM_ID *)&pXmrLicense->km.ck.guidKeyId) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Reserved GUID field)\n" ));
                goto ErrorExit;
            }

            /*
             * Ensure the Symmetric Cipher type is valid
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->km.ck.wSymmetricCipherType) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Symmetric cipher type)\n" ));
                goto ErrorExit;
            }

            if ( (bLeafLicense && pXmrLicense->km.ck.wSymmetricCipherType != XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR)
              || (!bLeafLicense && !bChainedLicense && pXmrLicense->km.ck.wSymmetricCipherType != XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR)
              || (!bLeafLicense &&  bChainedLicense && pXmrLicense->km.ck.wSymmetricCipherType != XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Symmetric cipher type is invalid\n" ));
                goto ErrorExit;
            }

            /*
             * Ensure the Asymmetric Cipher type is valid
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->km.ck.wAsymmetricCipherType) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Asymmetric cipher type)\n" ));
                goto ErrorExit;
            }

            if ( bLeafLicense ){

                if ( pXmrLicense->km.ck.wAsymmetricCipherType != XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Asymmetric cipher type is invalid\n" ));
                    goto ErrorExit;
                }

            } else {

                if ( pXmrLicense->km.ck.wAsymmetricCipherType != XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024 ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Asymmetric cipher type is invalid\n" ));
                    goto ErrorExit;
                }

            }

            /*
             * Get the length of the Encrypted key
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &pXmrLicense->km.ck.cbEncryptedKey) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Encrypted key length)\n" ));
                goto ErrorExit;
            }

            /*
             * Unpack the encrypted key from the buffer
             */
            if ( !WmdrmNetUnpackOctets( MessageDescriptor, (DRM_BYTE **) &pXmrLicense->km.ck.pbEncryptedKey, pXmrLicense->km.ck.cbEncryptedKey) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Encrypted key)\n" ));
                goto ErrorExit;
            }

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != SIZEOF( DRM_GUID ) + 3*SIZEOF(DRM_WORD) + pXmrLicense->km.ck.cbEncryptedKey ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            break;

        /*
         * Unpack the uplink kid object
         */
        case XMR_OBJECT_TYPE_UPLINK_KID_OBJECT:
            {
                DRM_WORD cbChainedChecksum;

                /*
                 * This shouldn't be present in a root license
                 */
                if ( FALSE == bLeafLicense ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    goto ErrorExit;
                }

                /*
                 * Get the uplink KID
                 */
                DRMSIZEASSERT( sizeof(DRM_ID), sizeof(DRM_GUID) );
                if ( !WmdrmNetUnpackVal128( MessageDescriptor, (DRM_ID *)&pXmrLicense->km.uk.idKID) ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: License too short (Uplink KID field)\n" ));
                    goto ErrorExit;
                }

                /*
                 * Get the length of the chained checksum
                 */
                if ( !WmdrmNetUnpackWord( MessageDescriptor, &cbChainedChecksum) ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: License too short (Chained checksum length)\n" ));
                    goto ErrorExit;
                }

                /*
                 * Make sure the checksum length is what we expect
                 */
                if ( cbChainedChecksum != DRM_XMR_CHAINED_CHECKSUM_LENGTH ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    goto ErrorExit;
                }

                /*
                 * Unpack the checksum from the buffer
                 */
                if ( !WmdrmNetUnpackOctets( MessageDescriptor, &pChainedChecksum, cbChainedChecksum ) ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: License too short (Chained checksum)\n" ));
                    goto ErrorExit;
                }
                MEMCPY( pXmrLicense->km.uk.idChainedChecksum.rgb, pChainedChecksum, cbChainedChecksum );
                Oem_MemFree( pChainedChecksum );

                /*
                 * Ensure correct length
                 */
                if ( dwObjectSize != SIZEOF( DRM_GUID ) + SIZEOF(DRM_WORD) + cbChainedChecksum ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                    goto ErrorExit;
                }

                /*
                 * The leaf license information must match the root license information
                 */
                if ( MEMCMP(&pXmrLicense->km.uk.idKID, &pXmrRootLicense->km.ck.guidKeyId, sizeof( DRM_GUID ) ) ||
                     MEMCMP(pXmrLicense->km.uk.idChainedChecksum.rgb, &pRootLicenseInfo->pIDChainedChecksum[0], DRM_XMR_CHAINED_CHECKSUM_LENGTH ) ) {
                    dr = DRM_E_ND_INVALID_MESSAGE;
                    goto ErrorExit;
                }
            }
            break;

        /*
         * Verify the signature
         */
        case XMR_OBJECT_TYPE_SIGNATURE_OBJECT:

            /*
             * Remember how many bytes to sign
             */
            DRMASSERT( MessageDescriptor->Offset >= XMR_BASE_SIZE );
            if( MessageDescriptor->Offset < XMR_BASE_SIZE ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Signature Object)\n" ));
                goto ErrorExit;
            }

            cbSignedMessage = MessageDescriptor->Offset-XMR_BASE_SIZE;

            /*
             * Ensure the Symmetric Cipher type is valid
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &wSignatureType) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Signature Type)\n" ));
                goto ErrorExit;
            }
            if ( wSignatureType != XMR_SIGNATURE_TYPE_AES_128_OMAC ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Signature Type is invalid.\n" ));
                goto ErrorExit;
            }

            /*
             * Get the signature data size
             */
            if ( !WmdrmNetUnpackWord( MessageDescriptor, &cbSignature) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Signature Length)\n" ));
                goto ErrorExit;
            }
            if ( cbSignature != DRM_AES_BLOCKLEN ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Signature Length is wrong.\n" ));
                goto ErrorExit;
            }

            /*
             * Get the signature itself
             */
            if ( !WmdrmNetUnpackOctets( MessageDescriptor, &pbSignature, cbSignature) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (Signature)\n" ));
                goto ErrorExit;
            }

            /*
             * Ensure the signature is in the last few bytes of the message
             */
            if ( MessageDescriptor->BufferSize != MessageDescriptor->Offset ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Signature must be last few bytes of the message.\n" ));
                goto ErrorExit;
            }


            /*
             * Get the content integrity key
             */

            if ( bLeafLicense ) {

                dr = DrmXmrGetKeyFromLicenseEx(FALSE,
                                               pbDecryptionKey,
                                               pXmrLicense,
                                               FALSE,
                                               &pAesKey);

            } else {

                dr = DrmXmrGetKeyFromLicenseEx(TRUE,
                                               pbDecryptionKey,
                                               pXmrLicense,
                                               FALSE,
                                               &pAesKey);
            }

            if ( dr != DRM_SUCCESS ) {
                TRACE(( "DrmXmrUnpackLicense: Cannot get AES content integrity key.\n" ));
                goto ErrorExit;
            }


            /*
             * Verify the signature
             */

            dr = Oem_Omac1_Verify( pAesKey, MessageDescriptor->Buffer, 0, cbSignedMessage, pbSignature, 0 );

            if ( dr != DRM_SUCCESS ) {
                TRACE(( "DrmXmrUnpackLicense: License signature is invalid.\n" ));
                goto ErrorExit;
            }

            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != 2*SIZEOF(DRM_WORD) + cbSignature ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            break;


        case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT:
            /*
             * Ensure the size is right
             */
            if ( dwObjectSize != SIZEOF(DRM_DWORD)) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx size is wrong.\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Unpack the field
             */
            if ( !WmdrmNetUnpackDword( MessageDescriptor, &pXmrLicense->gr.dwRevInfoSeq) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (revocation information sequence number)\n" ));
                goto ErrorExit;
            }

            break;

        /*
         * Handle objects we don't understand
         */
        default:
            /*
             * Ensure we aren't required to understand the type
             */
            if ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx must be understood but we don't\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * We at least understand that it cannot be at the root level
             */
            if ( wContainerType[lContainerStackIndex] == XMR_OBJECT_TYPE_INVALID ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: Object %2.2lx cannot be at root level\n", wObjectType ));
                goto ErrorExit;
            }

            /*
             * Skip past the object we don't understand
             */
            if ( !WmdrmNetUnpackOctets( MessageDescriptor, &pbUnknown, dwObjectSize) ) {
                dr = DRM_E_ND_INVALID_MESSAGE;
                TRACE(( "DrmXmrUnpackLicense: License too short (not understood object)\n" ));
                goto ErrorExit;
            }
            SAFE_OEM_FREE(pbUnknown);
            break;

        }


        /*
         * If the object was the last object in its container,
         *  pop back to the previous container
         */

        while ( lContainerStackIndex >= 0 &&
                (MessageDescriptor->Offset) == EndOfThisContainerOffset[lContainerStackIndex] )
        {

            lContainerStackIndex --;

        }

        /*
         * Handle objects that are too big for their container
         */

        if ( lContainerStackIndex >= 0 &&
             MessageDescriptor->Offset > EndOfThisContainerOffset[lContainerStackIndex] ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: Object %2.2lx is too large for its container. 0x%8.8lx\n", wObjectType, MessageDescriptor->Buffer+MessageDescriptor->Offset ));
            goto ErrorExit;
        }

    }

    /*
     * Ensure that required objects were found
     */
    for ( wObjectType=0; wObjectType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED; wObjectType++ ) {
        if ( !bObjectFound[wObjectType] && !bIsOptional[wObjectType] ) {
            dr = DRM_E_ND_INVALID_MESSAGE;
            TRACE(( "DrmXmrUnpackLicense: Object %2.2lx isn't optional\n", wObjectType ));
            goto ErrorExit;
        }
    }

    /*
     * Resolve disagreements between root and leaf license.
     */
    if ( bLeafLicense ) {


        /*
         * Minimum App Revocation List Version...
         */
        if ( pXmrLicense->gr.dwReserved1 < pXmrRootLicense->gr.dwReserved1 )
            pXmrLicense->gr.dwReserved1 = pXmrRootLicense->gr.dwReserved1;

        /*
         * Minimum Device Revocation List Versoin...
         */
        if ( pXmrLicense->gr.dwReserved2 < pXmrRootLicense->gr.dwReserved2 )
            pXmrLicense->gr.dwReserved2 = pXmrRootLicense->gr.dwReserved2;

        /*
         * OPLs...
         */
        if ( pXmrLicense->mopl.wReserved1 < pXmrRootLicense->mopl.wReserved1 )
            pXmrLicense->mopl.wReserved1 = pXmrRootLicense->mopl.wReserved1;

        if ( pXmrLicense->mopl.wUncompressedDigitalVideo < pXmrRootLicense->mopl.wUncompressedDigitalVideo )
            pXmrLicense->mopl.wUncompressedDigitalVideo = pXmrRootLicense->mopl.wUncompressedDigitalVideo;

        if ( pXmrLicense->mopl.wAnalogVideo < pXmrRootLicense->mopl.wAnalogVideo )
            pXmrLicense->mopl.wAnalogVideo = pXmrRootLicense->mopl.wAnalogVideo;

        if ( pXmrLicense->mopl.wReserved2 < pXmrRootLicense->mopl.wReserved2 )
            pXmrLicense->mopl.wReserved2 = pXmrRootLicense->mopl.wReserved2;

        if ( pXmrLicense->mopl.wUncompressedDigitalAudio < pXmrRootLicense->mopl.wUncompressedDigitalAudio )
            pXmrLicense->mopl.wUncompressedDigitalAudio = pXmrRootLicense->mopl.wUncompressedDigitalAudio;
    }

    /*
     * Success
     */
    *ppXmrLicense = pXmrLicense;
    pXmrLicense = NULL;
    dr = DRM_SUCCESS;

ErrorExit:

    /*
     * Free locally used resources
     */

    if ( MessageDescriptor != NULL ) {
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, (dr == DRM_SUCCESS) );
    }

    if ( pXmrLicense != NULL ) {
        DrmXmrFreeLicense( pXmrLicense );
    }

    if ( pbSignedMessage != NULL ) {
        Oem_MemFree( pbSignedMessage );
    }

    if ( pAesKey ) {
        Oem_Aes_ZeroKey( pAesKey );
        SAFE_OEM_FREE( pAesKey );
    }
    SAFE_OEM_FREE( pbSignature );

    return dr;
}

/*
 * The DrmXmrUnpackLicense routine unpacks an XMR License
 *
 * Parameters:
 *
 *      pPrivateKey - Specifies the RSA private key used to decrypt the content integrity key (as returned from DrmRsaSetPrivateKey)
 *          NULL indicates that the private key baked into the device should be used
 *
 *      Message - Specifies the buffer containing the packed XMR License
 *
 *      MessageSize - Specifies the size (in bytes) of the packed XMR License
 *
 *      ppXmrLicense - Returns the resulting XMR License
 *          The buffer must be freed using DrmXmrFreeLicense()
 *
 * Return Value:
 *  DRM_SUCCESS - The operation completed successfully
 *  DRM_E_ND_INVALID_MESSAGE
 *  DRM_E_OUTOFMEMORY
 */

DRM_RESULT
DrmXmrUnpackLicense(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_LICENSE **ppXmrLicense
    )
{
    return _InternalDrmXmrUnpackLicense(FALSE,
                                        FALSE,
                                        NULL,
                                        (const DRM_BYTE *)pPrivateKey,
                                        Message,
                                        MessageSize,
                                        ppXmrLicense);
}

DRM_RESULT
DrmXmrUnpackRootLicense(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_ROOT_LICENSE_INFO *pXmrRootLicenseInfo
    )
{
    DRM_RESULT dr;

    if ( NULL == pXmrRootLicenseInfo ) {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    /*
     * Unpack the license
     */
    dr = _InternalDrmXmrUnpackLicense(TRUE,
                                      FALSE,
                                      NULL,
                                      (const DRM_BYTE *)pPrivateKey,
                                      Message,
                                      MessageSize,
                                      &pXmrRootLicenseInfo->pXmrLicense);

    if ( dr != DRM_SUCCESS ) {
        goto ErrorExit;
    }

    /*
     * Get the content key and chained checksum from the license
     */
    pXmrRootLicenseInfo->pIDChainedChecksum = (DRM_ID *)Oem_MemAlloc( DRM_XMR_CHAINED_CHECKSUM_LENGTH );
    ChkMem( pXmrRootLicenseInfo->pIDChainedChecksum );

    dr = _InternalDrmXmrGetKeyFromLicenseEx(TRUE,
                                            (const DRM_BYTE *)pPrivateKey,
                                            pXmrRootLicenseInfo->pXmrLicense,
                                            TRUE,
                                            pXmrRootLicenseInfo->pIDChainedChecksum,
                                            &pXmrRootLicenseInfo->pAesContentKey);

    if ( dr != DRM_SUCCESS ) {
        goto ErrorExit;
    }

 ErrorExit:

    if ( dr != DRM_SUCCESS && pXmrRootLicenseInfo != NULL ) {
        if ( pXmrRootLicenseInfo->pXmrLicense ) {
            DrmXmrFreeLicense( pXmrRootLicenseInfo->pXmrLicense );
            pXmrRootLicenseInfo->pXmrLicense = NULL;
        }

        if ( pXmrRootLicenseInfo->pAesContentKey ) {
            Oem_Aes_ZeroKey( pXmrRootLicenseInfo->pAesContentKey );
            pXmrRootLicenseInfo->pAesContentKey = NULL;
        }

        if ( pXmrRootLicenseInfo->pIDChainedChecksum ) {
            Oem_MemFree( pXmrRootLicenseInfo->pIDChainedChecksum );
            pXmrRootLicenseInfo->pIDChainedChecksum = NULL;
        }

    }

    return dr;
}

DRM_RESULT
DrmXmrUnpackLeafLicense(
    __in XMR_ROOT_LICENSE_INFO XmrRootLicenseInfo,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_LICENSE **ppXmrLicense
    )
{
    /*
     * Unpack the license
     */
    return _InternalDrmXmrUnpackLicense(TRUE,
                                        TRUE,
                                        &XmrRootLicenseInfo,
                                        (const DRM_BYTE *)XmrRootLicenseInfo.pAesContentKey,
                                        Message,
                                        MessageSize,
                                        ppXmrLicense);

#if 0
#endif
}

/*
 * The DrmXmrFreeLicense routine free an XMR License structure and its children
 *
 * Parameters:
 *
 *      pXmrLicense - Specifies a pointer to the XMR License
 *
 * Return Value:
 *  None
 */

DRM_VOID
DrmXmrFreeLicense(
    __in XMR_LICENSE *pXmrLicense
    )
{
    DRM_DWORD i;

    if( pXmrLicense == NULL ) {
        return;
    }

    /*
     * Free any output configuration
     */
    if ( pXmrLicense->vopi.rgVop != NULL ) {
        for ( i=0; i<pXmrLicense->vopi.cEntries; i++ ) {
            if ( pXmrLicense->vopi.rgVop[i].pbConfigData != NULL ) {
                Oem_MemFree( pXmrLicense->vopi.rgVop[i].pbConfigData );
            }
        }
        Oem_MemFree( pXmrLicense->vopi.rgVop );
    }

    /*
     * Free the encrypted key
     */
    if ( pXmrLicense->km.ck.pbEncryptedKey != NULL ) {
      MEMSET( pXmrLicense->km.ck.pbEncryptedKey, 0, pXmrLicense->km.ck.cbEncryptedKey );
        Oem_MemFree( pXmrLicense->km.ck.pbEncryptedKey );
    }

    /*
     * Free the device key
     */
    if ( pXmrLicense->km.dk.pbModulus != NULL ) {
        MEMSET( pXmrLicense->km.dk.pbModulus, 0, pXmrLicense->km.dk.wModulusLength );
        Oem_MemFree( pXmrLicense->km.dk.pbModulus );
    }


    /*
     * Free the playback inclusion list
     */
    if ( pXmrLicense->gr.il.pGUIDS != NULL ) {
        MEMSET( pXmrLicense->gr.il.pGUIDS, 0, pXmrLicense->gr.il.cGUIDS * SIZEOF(DRM_GUID) );
        Oem_MemFree( pXmrLicense->gr.il.pGUIDS );
    }

    /*
     * Free the License itself
     */
    Oem_MemFree( pXmrLicense );
}

EXIT_PKTEST_NAMESPACE_CODE

