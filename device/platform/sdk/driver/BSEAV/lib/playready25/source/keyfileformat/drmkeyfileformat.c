/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMKEYFILEFORMAT_C
#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <oemeccp256.h>
#include <oemkeyfile.h>
#include <drmbcertparser.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

const static DRM_XB_ELEMENT_DESCRIPTION s_kfPlatformObjectDescription[] =
{
    /* Type                  offsets                                        size  */
    { XB_ELEMENT_TYPE_WORD, DRM_OFFSET_OF( KF_PLATFORM_OBJECT, wPlatform ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD, DRM_OFFSET_OF( KF_PLATFORM_OBJECT, wReserved ), XB_SIZE_IGNORED },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfKeypair1ObjectDescription[] =
{
    /* Type                     offsets                                                         size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V1_OBJECT, wCipherType ),             XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V1_OBJECT, wKeySize   ),              XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V1_OBJECT, xbbaPublicKeyData ),       XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_QWORD,    DRM_OFFSET_OF( KF_KEYPAIR_V1_OBJECT, qwInitializationVector ),  XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V1_OBJECT, xbbaEncryptedPrivateKey ), XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfKeypair2ObjectDescription[] =
{
    /* Type                     offsets                                                            size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, wCipherType ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, wKeySize   ),                 XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, xbbaPublicKeyData ),          XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, xbbaInitializationVector ),   XB_SIZE_ABSOLUTE(64)},
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, xbbaEncryptedPrivateKey ),    XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_GUID,     DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, guidKeypairDecryptionKeyID ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, wKeypairDecryptionMethod ),   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V2_OBJECT, wReserved ),                  XB_SIZE_IGNORED },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfKeypair3ObjectDescription[] =
{
    /* Type                     offsets                                                            size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, wCipherType ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, wKeySize   ),                 XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, xbbaPublicKeyData ),          XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_QWORD,    DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, qwInitializationVector ),     XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, xbbaEncryptedPrivateKey ),    XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_GUID,     DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, guidKeypairDecryptionKeyID ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, wKeypairDecryptionMethod ),   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_KEYPAIR_V3_OBJECT, wReserved ),                  XB_SIZE_IGNORED },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfHWIDObjectDescription[] =
{
    /* Type                     offsets                                                            size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_HWID_OBJECT, wType ),                            XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_HWID_OBJECT, xbbaHWID ),                         XB_SIZE_RELATIVE_WORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfSecureStoreKey1Object[] =
{
    /* Type                     offsets                                                                     size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V1_OBJECT, wCipherType ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V1_OBJECT, wKeySize   ),                 XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_QWORD,    DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V1_OBJECT, qwInitializationVector ),     XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V1_OBJECT, xbbaEncryptedSecureStoreKey ),XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfSecureStoreKey2Object[] =
{
    /* Type                     offsets                                                                     size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, wCipherType ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, wKeySize   ),                 XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, xbbaInitializationVector ),   XB_SIZE_ABSOLUTE(64) },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, xbbaEncryptedSecureStoreKey ),XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_GUID,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, guidKeypairDecryptionKeyID ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, wKeypairDecryptionMethod ),   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V2_OBJECT, wReserved ),                  XB_SIZE_IGNORED },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfSecureStoreKey3Object[] =
{
    /* Type                     offsets                                                                     size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, wCipherType ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, wKeySize   ),                 XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_QWORD,    DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, qwInitializationVector ),     XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, xbbaEncryptedSecureStoreKey ),XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_GUID,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, guidKeypairDecryptionKeyID ), XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, wKeypairDecryptionMethod ),   XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SECURE_STORE_KEY_V3_OBJECT, wReserved ),                  XB_SIZE_IGNORED },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfCertParserCacheObject[] =
{
    /* Type                     offsets                                                                    size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_CERT_PARSER_CACHE_OBJECT, wVersion   ),                  XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_CERT_PARSER_CACHE_OBJECT, xbbaHash ),                    XB_SIZE_ABSOLUTE(DRM_SHA1_DIGEST_LEN) },
    { XB_ELEMENT_TYPE_WORDLIST, DRM_OFFSET_OF( KF_CERT_PARSER_CACHE_OBJECT, wlData ),                      XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfSignatureObject[] =
{
    /* Type                     offsets                                                                    size  */
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SIGNATURE_OBJECT, xbbaSigningCertificate ),              XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SIGNATURE_OBJECT, wSignatureType ),                      XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SIGNATURE_OBJECT, xbbaSignatureData ),                   XB_SIZE_RELATIVE_WORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfCertificateChain1ObjectDescription[] =
{
    /* Type                     offsets                                                                   size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_CERTIFICATE_CHAIN_OBJECT_V1, wCertificateType ),        XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_CERTIFICATE_CHAIN_OBJECT_V1, xbbaCertificateChain ),    XB_SIZE_RELATIVE_WORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfCertificateChain2ObjectDescription[] =
{
    /* Type                     offsets                                                                   size  */
    { XB_ELEMENT_TYPE_DWORD,    DRM_OFFSET_OF( KF_CERTIFICATE_CHAIN_OBJECT_V2, dwCertificateType ),       XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_CERTIFICATE_CHAIN_OBJECT_V2, xbbaCertificateChain ),    XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfSymmetricKeyObjectDescription[] =
{
    /* Type                     offsets                                                                   size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SYMMETRICKEY_OBJECT, wCipherType ),                     XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_SYMMETRICKEY_OBJECT, wKeySize ),                        XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,    DRM_OFFSET_OF( KF_SYMMETRICKEY_OBJECT, dwIndivEscrowKeyVersion ),         XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_QWORD,    DRM_OFFSET_OF( KF_SYMMETRICKEY_OBJECT, qwInitializationVector ),          XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_SYMMETRICKEY_OBJECT, xbbaEncryptedKey ),                XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfASymmetricKeyObjectDescription[] =
{
    /* Type                     offsets                                                                   size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_ASYMMETRICKEY_OBJECT, wCipherType ),                    XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_ASYMMETRICKEY_OBJECT, wKeySize ),                       XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_ASYMMETRICKEY_OBJECT, xbbaPublicKey ),                  XB_SIZE_RELATIVE_DWORD },
    { XB_ELEMENT_TYPE_QWORD,    DRM_OFFSET_OF( KF_ASYMMETRICKEY_OBJECT, qwInitializationVector ),         XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY,DRM_OFFSET_OF( KF_ASYMMETRICKEY_OBJECT, xbbaEncryptedPrivateKey ),        XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfDeviceAppDataObject[] =
{
    /* Type                      offsets                                                           size  */
    { XB_ELEMENT_TYPE_DWORD,     DRM_OFFSET_OF( KF_DEVICE_APP_DATA, dwAppVersion ),                XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_BYTEARRAY, DRM_OFFSET_OF( KF_DEVICE_APP_DATA, xbbaAppId    ),                XB_SIZE_RELATIVE_DWORD },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfKeyFileIdObject[] =
{
    /* Type                 offsets                                               size  */
    { XB_ELEMENT_TYPE_GUID, DRM_OFFSET_OF( KF_KEYFILE_ID_OBJECT, guidKeyFileId ), XB_SIZE_IGNORED },
};


/* Define the array of entries (objects and containers) that are found in keyfile format */
const static DRM_XB_ENTRY_DESCRIPTION s_kf3EntryDescriptions[KF3_ENTRY_TYPE_COUNT] =
{
    /* Entry Type                                       Flags                                           Parent Type                             Dups?  Opt?   offsets/size,                                                           sort,    entry/element descriptor*/
    { XB_OBJECT_GLOBAL_HEADER,                          XB_FLAGS_NONE,                                  XB_OBJECT_TYPE_INVALID,                 FALSE, FALSE, 0,SIZEOF(DRM_KEYFILE),                                                   0,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_PLATFORM_OBJECT,                    XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      Platform         ),  1,       XB_USE_DESCRIPTOR(s_kfPlatformObjectDescription) },
    { KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,            XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER,  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      EscrowedData     ),  2,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_SYMMETRIC_KEY_OBJECT,               XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    SymmetricKeyv1   ),  3,       XB_USE_DESCRIPTOR(s_kfSymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_ASYMMETRIC_KEY_OBJECT,              XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    ASymmetricKey    ),  5,       XB_USE_DESCRIPTOR(s_kfASymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_KEYPAIR_OBJECT_2,                   XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_KEYHISTORY_CONTAINER_2,   TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( KF_KEYHISTORY_V2, pKeyPairHeadv2   ),  6,       XB_USE_DESCRIPTOR(s_kfKeypair2ObjectDescription) },
    { KF_ENTRY_TYPE_KEYPAIR_OBJECT_3,                   XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_KEYHISTORY_CONTAINER_2,   TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( KF_KEYHISTORY_V2, pKeyPairHeadv3   ),  7,       XB_USE_DESCRIPTOR(s_kfKeypair3ObjectDescription) },
    { KF_ENTRY_TYPE_CERT_PARSER_CACHE_OBJECT,           XB_FLAGS_NONE,                                  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      CertParserCache  ),  8,       XB_USE_DESCRIPTOR(s_kfCertParserCacheObject) },
    { KF_ENTRY_TYPE_KEYHISTORY_CONTAINER_2,             XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER,  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      KeyHistoryV2     ),  9,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2,         XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( DRM_KEYFILE,      pCertChainV2     ), 10,       XB_USE_DESCRIPTOR(s_kfCertificateChain2ObjectDescription) },
    { KF_ENTRY_TYPE_HWID_VALUE,                         XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      HWIDObject       ), 11,       XB_USE_DESCRIPTOR(s_kfHWIDObjectDescription) },
    { KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_2,          XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      SecureStoreKeyV2 ), 12,       XB_USE_DESCRIPTOR(s_kfSecureStoreKey2Object) },
    { KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_3,          XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      SecureStoreKeyV3 ), 13,       XB_USE_DESCRIPTOR(s_kfSecureStoreKey3Object) },
    { KF_ENTRY_TYPE_ESCROWED_SECURE_STORE_KEY_OBJECT,   XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    sstKey           ), 14,       XB_USE_DESCRIPTOR(s_kfSecureStoreKey3Object) }, /* Yes it is intentionally using the same description table as s_kfSecureStoreKey3Object */
    { KF_ENTRY_TYPE_ESCROWED_KEYPAIR_OBJECT,            XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( KF_ESCROWDATA,    pKeyPairHead     ), 15,       XB_USE_DESCRIPTOR(s_kfKeypair3ObjectDescription) }, /* Yes it is intentionally using the same description table as s_kfKeypair3ObjectDescription */
    { KF_ENTRY_TYPE_DEVICE_APP_DATA,                    XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      DeviceAppData    ), 16,       XB_USE_DESCRIPTOR(s_kfDeviceAppDataObject) },
    { KF_ENTRY_TYPE_KEYFILE_ID_OBJECT,                  XB_FLAGS_NONE,                                  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      KeyFileIdObject  ), 17,       XB_USE_DESCRIPTOR(s_kfKeyFileIdObject) },
    { KF_ENTRY_TYPE_SIGNATURE_OBJECT,                   XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      Signature        ), 18,       XB_USE_DESCRIPTOR(s_kfSignatureObject) },
};


/* Define the array of entries (objects and containers) that are found in keyfile format */
const static DRM_XB_ENTRY_DESCRIPTION s_kf2EntryDescriptions[KF2_ENTRY_TYPE_COUNT] =
{
    /* Entry Type                               Flags                                           Parent Type                             Dups?  Opt?   offsets/size,                                                            sort,    entry/element descriptor */
    { XB_OBJECT_GLOBAL_HEADER,                  XB_FLAGS_NONE,                                  XB_OBJECT_TYPE_INVALID,                 FALSE, FALSE, 0, SIZEOF(DRM_KEYFILE),                                                  0,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_PLATFORM_OBJECT,            XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      Platform         ),  1,       XB_USE_DESCRIPTOR(s_kfPlatformObjectDescription) },
    { KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,    XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER,  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      EscrowedData     ),  2,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_SYMMETRIC_KEY_OBJECT,       XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    SymmetricKeyv1   ),  3,       XB_USE_DESCRIPTOR(s_kfSymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_ASYMMETRIC_KEY_OBJECT,      XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    ASymmetricKey    ),  4,       XB_USE_DESCRIPTOR(s_kfASymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_KEYPAIR_OBJECT_2,           XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_KEYHISTORY_CONTAINER_2,   TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( KF_KEYHISTORY_V2, pKeyPairHeadv2   ),  5,       XB_USE_DESCRIPTOR(s_kfKeypair2ObjectDescription) },
    { KF_ENTRY_TYPE_CERT_PARSER_CACHE_OBJECT,   XB_FLAGS_NONE,                                  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      CertParserCache  ),  6,       XB_USE_DESCRIPTOR(s_kfCertParserCacheObject) },
    { KF_ENTRY_TYPE_KEYHISTORY_CONTAINER_2,     XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER,  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      KeyHistoryV2     ),  7,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2, XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( DRM_KEYFILE,      pCertChainV2     ),  8,       XB_USE_DESCRIPTOR(s_kfCertificateChain2ObjectDescription)} ,
    { KF_ENTRY_TYPE_HWID_VALUE,                 XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      HWIDObject       ),  9,       XB_USE_DESCRIPTOR(s_kfHWIDObjectDescription) },
    { KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_2,  XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      SecureStoreKeyV2 ), 10,       XB_USE_DESCRIPTOR(s_kfSecureStoreKey2Object) },
    { KF_ENTRY_TYPE_SIGNATURE_OBJECT,           XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      Signature        ), 11,       XB_USE_DESCRIPTOR(s_kfSignatureObject) },
};


/* Define the array of entries (objects and containers) that are found in keyfile format */
const static DRM_XB_ENTRY_DESCRIPTION s_kf1EntryDescriptions[KF1_ENTRY_TYPE_COUNT] =
{
    /* Entry Type                             Flags                                           Parent Type                             Dups?  Opt?   offsets/size,                                                           sort,    entry/element descriptor */
    { XB_OBJECT_GLOBAL_HEADER,                XB_FLAGS_NONE,                                  XB_OBJECT_TYPE_INVALID,                 FALSE, FALSE, 0, SIZEOF(DRM_KEYFILE),                                                 0,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER,  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      EscrowedData     ), 1,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_SYMMETRIC_KEY_OBJECT,     XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    SymmetricKeyv1   ), 2,       XB_USE_DESCRIPTOR(s_kfSymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_ASYMMETRIC_KEY_OBJECT,    XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_ESCROWED_DATA_CONTAINER,  FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( KF_ESCROWDATA,    ASymmetricKey    ), 3,       XB_USE_DESCRIPTOR(s_kfASymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_KEYPAIR_OBJECT,           XB_FLAGS_MUST_UNDERSTAND,                       KF_ENTRY_TYPE_KEYHISTORY_CONTAINER,     TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( KF_KEYHISTORY_V1, pHead            ), 5,       XB_USE_DESCRIPTOR(s_kfKeypair1ObjectDescription) },
    { KF_ENTRY_TYPE_KEYHISTORY_CONTAINER,     XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER,  XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      KeyHistoryV1     ), 4,       NULL, SIZEOF(DRM_DWORD) }, /* Super special hack.  For a non-std container, this size is extra data to skip in the container header */
    { KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT, XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                TRUE,  TRUE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( DRM_KEYFILE,      pCertChainV1     ), 6,       XB_USE_DESCRIPTOR(s_kfCertificateChain1ObjectDescription) },
    { KF_ENTRY_TYPE_HWID_VALUE,               XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      HWIDObject       ), 7,       XB_USE_DESCRIPTOR(s_kfHWIDObjectDescription) },
    { KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT,  XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, TRUE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      SecureStoreKeyV1 ), 8,       XB_USE_DESCRIPTOR(s_kfSecureStoreKey1Object) },
    { KF_ENTRY_TYPE_SIGNATURE_OBJECT,         XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,                FALSE, FALSE, XB_DECLARE_OFFSET_AND_SIZE      ( DRM_KEYFILE,      Signature        ), 9,       XB_USE_DESCRIPTOR(s_kfSignatureObject) },
};

const static DRM_XB_ELEMENT_DESCRIPTION s_kfHeaderExtraDataDescription[] =
{
    /* Type                     offsets                                                                  size  */
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_HEADER_EXTRA_DATA_V1, wPlatform ),                    XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_WORD,     DRM_OFFSET_OF( KF_HEADER_EXTRA_DATA_V1, wReserved ),                    XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,    DRM_OFFSET_OF( KF_HEADER_EXTRA_DATA_V1, dwLength ),                     XB_SIZE_IGNORED },
    { XB_ELEMENT_TYPE_DWORD,    DRM_OFFSET_OF( KF_HEADER_EXTRA_DATA_V1, dwSignedLength ),               XB_SIZE_IGNORED },
};

const static DRM_XB_ENTRY_DESCRIPTION s_kf1HeaderEntryDescription =
{
    0, 0, 0,  FALSE, FALSE, DRM_OFFSET_OF( DRM_KEYFILE, HeaderExtra ),        SIZEOF(KF_HEADER_EXTRA_DATA_V1),       0,       XB_USE_DESCRIPTOR(s_kfHeaderExtraDataDescription),
};

                                                               /* Identifier length          DWORD identifier                                  version,                extra data */
const static DRM_XB_HEADER_DESCRIPTION s_kf1HeaderDescription = { XB_FORMAT_ID_LENGTH_DWORD, XB_DEFINE_DWORD_FORMAT_ID( 'P', 'R', 'K', 'F' ),  KEYFILE_VERSION_1,      DRM_OFFSET_OF(KF_HEADER_EXTRA_DATA_V1,dwLength), &s_kf1HeaderEntryDescription };
const static DRM_XB_HEADER_DESCRIPTION s_kf2HeaderDescription = { XB_FORMAT_ID_LENGTH_DWORD, XB_DEFINE_DWORD_FORMAT_ID( 'P', 'R', 'K', 'F' ),  KEYFILE_VERSION_2,      NULL,                                             0 };
const static DRM_XB_HEADER_DESCRIPTION s_kf3HeaderDescription = { XB_FORMAT_ID_LENGTH_DWORD, XB_DEFINE_DWORD_FORMAT_ID( 'P', 'R', 'K', 'F' ),  KEYFILE_VERSION_3,      NULL,                                             0 };

const DRM_XB_FORMAT_DESCRIPTION g_kfFormatDescription[] =
{
    /* header                       container/object table      count of entries              */
    { &s_kf1HeaderDescription,      s_kf1EntryDescriptions,    NO_OF( s_kf1EntryDescriptions ) },
    { &s_kf2HeaderDescription,      s_kf2EntryDescriptions,    NO_OF( s_kf2EntryDescriptions ) },
    { &s_kf3HeaderDescription,      s_kf3EntryDescriptions,    NO_OF( s_kf3EntryDescriptions ) }
};

/* Activation: define the array of entries (objects and containers) that are for activation escrowed data */
const static DRM_XB_ENTRY_DESCRIPTION s_ad1EntryDescriptions[AD1_ENTRY_TYPE_COUNT] =
{
    /* Entry Type                                       Flags                                           Parent Type               Dups?  Opt?   offsets/size,                                                               sort,    entry/element descriptor*/
    { XB_OBJECT_GLOBAL_HEADER,                          XB_FLAGS_NONE,                                  XB_OBJECT_TYPE_INVALID,   FALSE, FALSE,  0,SIZEOF(DRM_ACTIVATION_DATA),                                                        0,       XB_NULL_DESCRIPTOR },
    { KF_ENTRY_TYPE_SYMMETRIC_KEY_OBJECT,               XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,  FALSE, FALSE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_ACTIVATION_DATA,    SymmetricKey   ), 1,       XB_USE_DESCRIPTOR(s_kfSymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_ASYMMETRIC_KEY_OBJECT,              XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,  FALSE, FALSE,  XB_DECLARE_OFFSET_AND_SIZE      ( DRM_ACTIVATION_DATA,    ASymmetricKey    ), 2,       XB_USE_DESCRIPTOR(s_kfASymmetricKeyObjectDescription) },
    { KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2,         XB_FLAGS_MUST_UNDERSTAND,                       XB_OBJECT_GLOBAL_HEADER,  TRUE,  FALSE,  XB_DECLARE_OFFSET_AND_SIZE_DEREF( DRM_ACTIVATION_DATA,    pCertChainV2     ), 3,       XB_USE_DESCRIPTOR(s_kfCertificateChain2ObjectDescription) },
};

const static DRM_XB_HEADER_DESCRIPTION s_ad1HeaderDescription = { XB_FORMAT_ID_LENGTH_DWORD, XB_DEFINE_DWORD_FORMAT_ID( 'P', 'R', 'A', 'D' ),  ACTIVATION_DATA_VERSION_1, NULL, 0 };

const DRM_XB_FORMAT_DESCRIPTION g_adFormatDescription[] =
{
    /* header                       container/object table      count of entries              */
    { &s_ad1HeaderDescription,      s_ad1EntryDescriptions,    NO_OF( s_ad1EntryDescriptions ) }
};

/*
** Returns an error specific to keyfile instead of a generic out-of-memory error
** thrown by a stack allocator.
*/
#define TranslateKeyfileOOMError(_dr) \
    ( ( _dr == DRM_E_OUTOFMEMORY ) ? DRM_E_KEYFILE_TOO_LARGE : _dr )


DRM_DWORD _VersionToFormatDescriptionTranslation( DRM_DWORD f_dwVersion )
{
    switch( f_dwVersion )
    {
    case KEYFILE_VERSION_1: return 0;
    case KEYFILE_VERSION_2: return 1;
    case KEYFILE_VERSION_3: return 2;
    default: DRMASSERT( !"Invalid keyfile version"); return 0;
    }
}

DRM_API DRM_RESULT DRM_CALL DRM_KF_Start(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __in                              DRM_DWORD                f_dwVersion,
    __in_opt                    const KF_SIGNATURE_OBJECT     *f_psignatureData,
    __inout_ecount( 1 )               DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( f_dwVersion == KEYFILE_VERSION_1 )
    {
        /* Building a V1 keyfile is not supported */
        ChkDR( DRM_E_INVALIDARG );
    }

    ChkDR( DRM_XB_StartFormat( f_pbStack,
        f_cbStack,
        g_kfFormatDescription[ADJUST_VERSION_TO_INDEX(f_dwVersion)].pHeaderDescription->dwFormatVersion,
        f_pcontextBuilder,
        &g_kfFormatDescription[ADJUST_VERSION_TO_INDEX(f_dwVersion)] ) );

    if( f_psignatureData != NULL )
    {
        ChkDR( DRM_XB_AddEntry( f_pcontextBuilder, KF_ENTRY_TYPE_SIGNATURE_OBJECT, f_psignatureData ) );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_KF_Finish(
    __inout_ecount( 1 )           const DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder,
    __inout_bcount(*f_pcbKeyFile)       DRM_BYTE                *f_pbKeyFile,
    __inout                             DRM_DWORD               *f_pcbKeyFile,
    __in                                DRM_KF_CB_SIGNATURE      f_pfnSignatureCallback,
    __in                          const DRM_VOID                *f_pvSignatureContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_XB_BUILDER_CONTEXT_INTERNAL *pbuilderInternal =
        (const DRM_XB_BUILDER_CONTEXT_INTERNAL*) f_pcontextBuilder;
    DRM_DWORD iObject = 0;
    DRM_DWORD cbObject = 0;
    DRM_DWORD iSignatureFromObjectStart = 0;

    ChkDR( DRM_XB_FinishFormat( f_pcontextBuilder,
        f_pbKeyFile,
        f_pcbKeyFile) );

    ChkDR( DRM_XB_FindObject( KF_ENTRY_TYPE_SIGNATURE_OBJECT, pbuilderInternal->pformat, f_pbKeyFile, *f_pcbKeyFile, &iObject, &cbObject ) );

    DRMASSERT( cbObject >= ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

    /* The signature is the *last* N bytes of the object */
    iSignatureFromObjectStart = cbObject - ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
    ChkDR( f_pfnSignatureCallback( f_pvSignatureContext,
                                   f_pbKeyFile,
                                  *f_pcbKeyFile - cbObject,
                                   f_pbKeyFile,
                                   ECDSA_P256_SIGNATURE_SIZE_IN_BYTES,
                                   iObject + iSignatureFromObjectStart ) );

ErrorExit:
    return dr;
}

static DRM_RESULT _ValidateKeyFileAssumptions(
    __in const DRM_KEYFILE *f_pKeyFile )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /* Some of these checks are likely unnecessary as the XB parser should
    fail when some of the V1 only objects are found in V2 and vice-versa */
    if( f_pKeyFile->dwVersion == KEYFILE_VERSION_1 )
    {
        /* The platform object is a V2 only object */
        ChkBOOL( !f_pKeyFile->Platform.fValid, DRM_E_KEYFILE_INVALID_PLATFORM );
    }
    else if( f_pKeyFile->dwVersion == KEYFILE_VERSION_2 )
    {
        ChkBOOL( f_pKeyFile->Platform.wPlatform == KEYFILE_PLATFORM_WINDOWSMOBILE7, DRM_E_KEYFILE_INVALID_PLATFORM );
    }
    else
    {
        /* WM7 code hasn't been updated for v3 yet */
        ChkBOOL( f_pKeyFile->Platform.wPlatform == KEYFILE_PLATFORM_PC
              || f_pKeyFile->Platform.wPlatform == KEYFILE_PLATFORM_FIJI
              || f_pKeyFile->Platform.wPlatform == KEYFILE_PLATFORM_PK, DRM_E_KEYFILE_INVALID_PLATFORM );
    }

    /* The signature object is required always even if we aren't validating it */
    ChkBOOL( f_pKeyFile->Signature.fValid, DRM_E_KEYFILE_INVALID_SIGNATURE );
    if( f_pKeyFile->Signature.wSignatureType == KEYFILE_SIGNATURE_TYPE_OMAC_1 )
    {
        ChkBOOL( f_pKeyFile->Signature.xbbaSigningCertificate.cbData == SIZEOF( DRM_GUID ), DRM_E_KEYFILE_FORMAT_INVALID );
        ChkBOOL( f_pKeyFile->Signature.xbbaSignatureData.cbData == DRM_AES_BLOCKLEN, DRM_E_KEYFILE_FORMAT_INVALID );
    }
    else if( f_pKeyFile->Signature.wSignatureType == KEYFILE_SIGNATURE_TYPE_ECDSA_256_SHA_256 )
    {
        ChkBOOL( f_pKeyFile->Signature.xbbaSignatureData.cbData == ECDSA_P256_SIGNATURE_SIZE_IN_BYTES, DRM_E_KEYFILE_FORMAT_INVALID );
    }
    else
    {
        ChkDR( DRM_E_KEYFILE_FORMAT_INVALID );
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _PerformVersionFixups(
                                 __in                             DRM_KEYFILE             *f_pKeyFile ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _PerformVersionFixups(
                                 __in                             DRM_KEYFILE             *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    if( f_pKeyFile->dwVersion == KEYFILE_VERSION_1 )
    {
        KF_CERTIFICATE_CHAIN_OBJECT_V1 *pCertChain;

        /* In version 1 the cert types were mapped different.
        ** 1 was playready and 2 was NDT.
        ** In v2 and V3 1 is WMDRM, 2 is NDT, and 3 is Playready
        ** Since V1 keyfiles don't support WMDRM we only need to
        ** map 1 (playready) to KF_CERT_TYPE_PLAYREADY
        */

        for( pCertChain = f_pKeyFile->pCertChainV1;
            pCertChain != NULL && pCertChain->fValid;
            pCertChain = pCertChain->pNext )
        {
            if( pCertChain->wCertificateType == 1 )
            {
                pCertChain->wCertificateType = KF_CERT_TYPE_PLAYREADY;
            }
        }
    }
    else if( f_pKeyFile->dwVersion == KEYFILE_VERSION_2 )
    {
        if( f_pKeyFile->SecureStoreKeyV2.fValid )
        {
            /* In the PK V2 keyfile a value of 4 was used to describe the AES keytype
            ** This was incorrect and a value of 2 should have been used
            ** Alter a 4 to 2 here to normalize the value for future readers
            */
            if( f_pKeyFile->SecureStoreKeyV2.wKeySize == 4 )
            {
                f_pKeyFile->SecureStoreKeyV2.wKeySize = 2;
            }
        }
    }
    return DRM_SUCCESS;
}



DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_Parse(
    __in                              DRM_VOID                *f_pOEMContext,
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __in_bcount( f_cbStack )    const DRM_BYTE                *f_pbKeyFile,
    __in                              DRM_DWORD                f_cbKeyFile,
    __in                              DRM_BOOL                 f_fValidateSignature,
    __out                             DRM_KEYFILE             *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT parserStack = {0};

    ChkDR( DRM_STK_Init( &parserStack, f_pbStack, f_cbStack ) );
    ZEROMEM( f_pKeyFile, SIZEOF( *f_pKeyFile ) );

    ChkDR( DRM_XB_UnpackBinary(
        f_pbKeyFile,
        f_cbKeyFile,
        &parserStack,
        g_kfFormatDescription,
        NO_OF( g_kfFormatDescription ),
        &(f_pKeyFile->dwVersion),
        f_pKeyFile ) );


    ChkDR( _PerformVersionFixups( f_pKeyFile ) );

    if( f_fValidateSignature )
    {
        DRM_DWORD  iSigObject  = 0;
        DRM_DWORD  cbSigObject = 0;

        ChkDR( _ValidateKeyFileAssumptions( f_pKeyFile ) );

        ChkDR( DRM_XB_FindObject(
            KF_ENTRY_TYPE_SIGNATURE_OBJECT,
            &g_kfFormatDescription[ADJUST_VERSION_TO_INDEX(f_pKeyFile->dwVersion)],
            f_pbKeyFile,
            f_cbKeyFile,
            &iSigObject,
            &cbSigObject ) );

        if( f_pKeyFile->Signature.wSignatureType == KEYFILE_SIGNATURE_TYPE_OMAC_1 )
        {
            DRM_BYTE rgbSignature[DRM_AES_BLOCKLEN] = {0};

            ChkDR( DRM_BBX_KF_CalculateOmac(
                f_pOEMContext,
                (DRM_GUID*)(f_pKeyFile->Signature.xbbaSigningCertificate.pbDataBuffer + f_pKeyFile->Signature.xbbaSigningCertificate.iData),
                FALSE,
                f_pbKeyFile,
                iSigObject,
                rgbSignature ) );

            ChkBOOL( MEMCMP( rgbSignature,
                f_pKeyFile->Signature.xbbaSignatureData.pbDataBuffer + f_pKeyFile->Signature.xbbaSignatureData.iData,
                DRM_AES_BLOCKLEN ) == 0, DRM_E_KEYFILE_INVALID_SIGNATURE );
        }
        else /* KEYFILE_SIGNATURE_TYPE_ECDSA_256_SHA_256 */
        {
            DRM_BCERT_VERIFICATIONCONTEXT oVerificationCtx = { 0 };
            DRMFILETIME ftCurrent = { 0 };
            PUBKEY_P256 PubKey = { 0 };
            DRM_DWORD dwOffset = 0;
            DRM_BCERT_CHAIN_HEADER bcertChainHeader = { 0 };
            DRM_BCERT_CERTIFICATE bcertCertificate = { 0 };

            /* Make sure is a KeyFile signing certificate. */
            ChkDR(DRM_BCert_GetChainHeader(
                f_pKeyFile->Signature.xbbaSigningCertificate.pbDataBuffer + f_pKeyFile->Signature.xbbaSigningCertificate.iData,
                f_pKeyFile->Signature.xbbaSigningCertificate.cbData,
                &dwOffset,
                &bcertChainHeader));

            ChkDR(DRM_BCert_GetCertificate(
                f_pKeyFile->Signature.xbbaSigningCertificate.pbDataBuffer + f_pKeyFile->Signature.xbbaSigningCertificate.iData,
                f_pKeyFile->Signature.xbbaSigningCertificate.cbData,
                &dwOffset,
                &bcertCertificate,
                DRM_BCERT_CERTTYPE_UNKNOWN));

            ChkBOOL(bcertCertificate.BasicInfo.dwType == DRM_BCERT_CERTTYPE_KEYFILESIGNER,
                DRM_E_KEYFILE_FORMAT_INVALID);

            /* Verify the signing certificate by chaining up to PleyReady DRM root. */
            Oem_Clock_GetSystemTimeAsFileTime(NULL, &ftCurrent);

            ChkDR( DRM_BCert_InitVerificationContext( &ftCurrent,
                (PUBKEY_P256 *)(g_rgbMSPlayReadyRootIssuerPubKey),
                DRM_BCERT_CERTTYPE_UNKNOWN,
                &f_pKeyFile->CryptoCtx,
                TRUE,
                FALSE,
                NULL,
                0,
                FALSE,
                NULL,
                NULL,
                NULL,
                &oVerificationCtx ) );

            ChkDR(DRM_BCert_ParseCertificateChain(f_pKeyFile->Signature.xbbaSigningCertificate.pbDataBuffer + f_pKeyFile->Signature.xbbaSigningCertificate.iData,
                f_pKeyFile->Signature.xbbaSigningCertificate.cbData,
                &oVerificationCtx));

            ChkDR(DRM_BCert_GetPublicKey(f_pKeyFile->Signature.xbbaSigningCertificate.pbDataBuffer + f_pKeyFile->Signature.xbbaSigningCertificate.iData,
                f_pKeyFile->Signature.xbbaSigningCertificate.cbData,
                0,
                &PubKey));

            if( DRM_FAILED( OEM_ECDSA_Verify_P256(
                f_pbKeyFile,
                iSigObject,
                &PubKey,
                (SIGNATURE_P256*)(f_pKeyFile->Signature.xbbaSignatureData.pbDataBuffer + f_pKeyFile->Signature.xbbaSignatureData.iData),
                (struct bigctx_t *)&f_pKeyFile->CryptoCtx ) ) )
            {
                ChkDR( DRM_E_KEYFILE_INVALID_SIGNATURE );
            }
        }
    }

ErrorExit:
    return TranslateKeyfileOOMError(dr);
}

// support for parsing activation data
DRM_API DRM_RESULT DRM_CALL DRM_AD_Parse(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __in_bcount( f_cbActivationData ) const DRM_BYTE          *f_pbActivationData,
    __in                              DRM_DWORD                f_cbActivationData,
    __out                             DRM_ACTIVATION_DATA     *f_pActivationData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT parserStack = {0};

    ChkDR( DRM_STK_Init( &parserStack, f_pbStack, f_cbStack ) );
    ZEROMEM( f_pActivationData, SIZEOF( *f_pActivationData ) );

    ChkDR( DRM_XB_UnpackBinary(
        f_pbActivationData,
        f_cbActivationData,
        &parserStack,
        g_adFormatDescription,
        NO_OF( g_adFormatDescription ),
        &(f_pActivationData->dwVersion),
        f_pActivationData ) );

ErrorExit:
    return TranslateKeyfileOOMError(dr);
}

static DRM_DWORD _activationDataVersionToFormatDescTranslation( DRM_DWORD f_dwVersion )
{
    switch( f_dwVersion )
    {
    case ACTIVATION_DATA_VERSION_1: return 0;
    default: DRMASSERT( !"Invalid activation data version"); return 0;
    }
}

DRM_API DRM_RESULT DRM_CALL DRM_AD_Start(
    __inout_bcount( f_cbStack ) DRM_BYTE                *f_pbStack,
    __in                        DRM_DWORD                f_cbStack,
    __inout_ecount( 1 )         DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR(DRM_XB_StartFormat( f_pbStack,
         f_cbStack,
         g_adFormatDescription[_activationDataVersionToFormatDescTranslation(CURRENT_ACTIVATION_DATA_VERSION)].pHeaderDescription->dwFormatVersion,
         f_pcontextBuilder,
         &g_adFormatDescription[_activationDataVersionToFormatDescTranslation(CURRENT_ACTIVATION_DATA_VERSION)] ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

