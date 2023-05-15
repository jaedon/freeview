/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __CRYPTOTEST_H__
#define __CRYPTOTEST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "tclib.h"
#include "tstutils.h"

#include <oempkcrypto.h>
#include <oemcipher.h>
#include <drmutilities.h>
#include <oemshahash.h>
#include <oemshahmac.h>
#include <oemaes.h>
#include <oemrsaoaeppss.h>
#include <oemsha256.h>
#include <oemeccp256.h>

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define MAX_RANDOM_OFFSET 100
#define MAX_CB_SYMMETRIC_BUFFER 102400

extern DRM_BYTE  g_pszPlainKey[DRM_ECC160_PLAINTEXT_LEN];
extern DRM_BYTE *g_pszKey;
extern DRM_BYTE *g_pbCryptoContext;
extern DRM_CRYPTO_CONTEXT g_Crypto;
extern DRM_BYTE *g_pbRandBufIn;
extern DRM_BYTE *g_pbCipher;
extern DRM_BYTE *g_pbDecryptBufOut;
extern PUBKEY   *g_pPubKey;
extern PRIVKEY  *g_pPrivKey;

extern DRM_CIPHER_CONTEXT  g_Context;
extern DRM_CIPHER_CONTEXT *g_pContext;
extern RC4_KEYSTRUCT       g_rc4ks;
extern DESTable            g_desTable;
extern DRM_BYTE           *g_pbSymmetricEncryptedData;
extern DRM_BYTE           *g_pbSymmetricClearData;
extern DRM_UINT64          g_qwSymmetricDataID ;
extern DRM_BYTE   *g_pbOrgDataCBC;
extern DRM_BYTE   *g_pbEncDataCBC;
extern DRM_CBCKey  g_mackey;
extern DRM_CBCKey  g_invmackey;

extern DRM_AES_KEY  g_aesKey;
extern DRM_AES_COUNTER_MODE_CONTEXT g_AesCtrContext;

DECLARE_MODULE_DEFS(Crypto)

EXIT_PKTEST_NAMESPACE

#endif  /* __CRYPTOTEST_H__ */

