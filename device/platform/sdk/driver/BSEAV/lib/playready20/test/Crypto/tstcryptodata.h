/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __TSTCRYPTODATA_H__
#define __TSTCRYPTODATA_H__


#include "cryptotest.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

extern const DRM_BYTE g_szSHA256TestVector1[__CB_DECL(4) ];
extern const DRM_BYTE g_szSHA256TestVector2[56];
extern const DRM_BYTE g_szSHA256TestVector3[__CB_DECL(44) ];
extern const DRM_BYTE g_szSHA256TestVector4[__CB_DECL(44) ];
extern const DRM_BYTE g_szSHA256TestVector5[__CB_DECL(2) ];
extern const DRM_BYTE g_szSHA256TestVector6[64];
extern const DRM_BYTE g_szSHA256TestVector7[__CB_DECL(46) ];

#define TEST_VECTOR1_SIZE_IN_BYTES (3)  /* "abc" */
#define TEST_VECTOR2_SIZE_IN_BYTES (56) /* "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" */
#define TEST_VECTOR3_SIZE_IN_BYTES (43) /* "The quick brown fox jumps over the lazy dog" */
#define TEST_VECTOR4_SIZE_IN_BYTES (43) /* "The quick brown fox jumps over the lazy cog" */
#define TEST_VECTOR5_SIZE_IN_BYTES (0)  /* "" */
#define TEST_VECTOR6_SIZE_IN_BYTES (1000000) /* 1 million 'a's */
#define TEST_VECTOR7_SIZE_IN_BYTES (TEST_VECTOR3_SIZE_IN_BYTES) /* "The quick brown fox jumps over the lazy dog", non-aligned */

extern const DRM_BYTE g_szSHA256TestVector1Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];
extern const DRM_BYTE g_szSHA256TestVector2Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];
extern const DRM_BYTE g_szSHA256TestVector3Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];
extern const DRM_BYTE g_szSHA256TestVector4Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];
extern const DRM_BYTE g_szSHA256TestVector5Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];
extern const DRM_BYTE g_szSHA256TestVector6Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];
extern const DRM_BYTE g_szSHA256TestVector7Result[__CB_DECL(SHA256_DIGEST_SIZE_IN_BYTES) ];

extern const DRM_CHAR g_pszRC4KnownClearText[];
extern DRM_BYTE       g_rgbRC4KnownCipherText[];
extern DRM_BYTE g_rgbRC4KnownContentKey[DRMCIPHERKEYLEN_RC4 + 1];

extern const DRM_BYTE g_rgbAESECBKnownPlaintext[];
extern const DRM_BYTE g_rgbAESECB128Key[];
extern const DRM_BYTE g_rgbAESECB128KnownCiphertext[];
extern const DRM_BYTE g_rgbAESECB192Key[];
extern const DRM_BYTE g_rgbAESECB192KnownCiphertext[];
extern const DRM_BYTE g_rgbAESECB256Key[];
extern const DRM_BYTE g_rgbAESECB256KnownCiphertext[];
extern const DRM_BYTE g_rgbAESCtrKnownPlaintext[];
extern const DRM_BYTE g_rgbAESCtr128Key[];
extern const DRM_BYTE g_rgbAESCtr128KnownCiphertext[];
extern const DRM_BYTE g_rgbAESCtr192Key[];
extern const DRM_BYTE g_rgbAESCtr192KnownCiphertext[];
extern const DRM_BYTE g_rgbAESCtr256Key[];
extern const DRM_BYTE g_rgbAESCtr256KnownCiphertext[];
extern const DRM_BYTE g_rgbAESCbcKnownPlaintext[];
extern const DRM_BYTE g_rgbAESCbc128Key[];
extern const DRM_BYTE g_rgbAESCbc128IV[];
extern const DRM_BYTE g_rgbAESCbc128KnownCiphertext[];

extern const DRM_DWORD g_cbAESECBKnownPlaintext;
extern const DRM_DWORD g_cbAESECB128Key;
extern const DRM_DWORD g_cbAESECB128KnownCiphertext;
extern const DRM_DWORD g_cbAESECB192Key;
extern const DRM_DWORD g_cbAESECB192KnownCiphertext;
extern const DRM_DWORD g_cbAESECB256Key;
extern const DRM_DWORD g_cbAESECB256KnownCiphertext;
extern const DRM_DWORD g_cbAESCtrKnownPlaintext;
extern const DRM_DWORD g_cbAESCtr128Key;
extern const DRM_DWORD g_cbAESCtr128KnownCiphertext;
extern const DRM_DWORD g_cbAESCtr192Key;
extern const DRM_DWORD g_cbAESCtr192KnownCiphertext;
extern const DRM_DWORD g_cbAESCtr256Key;
extern const DRM_DWORD g_cbAESCtr256KnownCiphertext;
extern const DRM_DWORD g_cbAESCbcKnownPlaintext;
extern const DRM_DWORD g_cbAESCbc128Key;
extern const DRM_DWORD g_cbAESCbc128IV;
extern const DRM_DWORD g_cbAESCbc128KnownCiphertext;


extern const DRM_BYTE  g_omacKey[];
extern const DRM_DWORD g_cbOmacKey;
extern const DRM_BYTE  g_omacMsg[];
extern const DRM_DWORD SIZEOF_OMACMSG;
extern const DRM_BYTE  g_omacTag[];

extern const DRM_BYTE g_rgbRSAModulus1024[__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_1024 ) )];
extern const DRM_BYTE g_rgbRSAPrivExp1024[__CB_DECL( DRM_RSA_CB_PRIVEXP( eDRM_RSA_1024 ) )];
extern const DRM_BYTE g_rgbRSAPrimes1024[2][__CB_DECL( DRM_RSA_CB_PRIME( eDRM_RSA_1024 ) )];
extern const DRM_BYTE g_rgbRSAKnownPlaintext1024[2][__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_1024 ) )];
extern const DRM_BYTE g_rgbRSAKnownCiphertext1024[2][__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_1024 ) )];

extern const DRM_BYTE g_rgbRSAModulus2048[__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_2048 ) )];
extern const DRM_BYTE g_rgbRSAPrivExp2048[__CB_DECL( DRM_RSA_CB_PRIVEXP( eDRM_RSA_2048 ) )];
extern const DRM_BYTE g_rgbRSAPrimes2048[2][__CB_DECL( DRM_RSA_CB_PRIME( eDRM_RSA_2048 ) )];
extern const DRM_BYTE g_rgbRSAKnownPlaintext2048[2][__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_2048 ) )];
extern const DRM_BYTE g_rgbRSAKnownCiphertext2048[2][__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_2048 ) )];

extern DRM_RSA_PUBLIC_KEY   g_RsaPubKey;
extern DRM_RSA_PRIVATE_KEY  g_RsaPrivKey;
extern DRM_DWORD            g_cbRsaCiphertext;
extern DRM_BYTE             g_rgbRSACiphertext[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )];
extern DRM_BYTE             g_rgbRSAPlaintext[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )];
#define g_rgbRSAPubExp rgbDRM_RSA_DEFAULT_PUBEXP /* 65537 */

EXIT_PKTEST_NAMESPACE

#endif  /* __TSTCRYPTODATA_H__ */

