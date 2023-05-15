/******************************************************************************
**
** This file is here temporarily. It defines PlayReady types necessary to compile the project.
** Needs to be changed to include actual PlayReady headers, instead of redfining the types
**
*******************************************************************************
*/

#ifndef __OEMHALDEFS_H__
#define __OEMHALDEFS_H__

#include <inttypes.h>

#define DRM_API
#define DRM_CALL
#define DRM_NO_INLINE
#define DRM_NO_INLINE_ATTRIBUTE
#define ENTER_PK_NAMESPACE
#define EXIT_PK_NAMESPACE

#define TRUE 1
#define FALSE 0

#define __in
#define __in_opt

#define __out
#define __out_opt

#define __inout
#define __inout_opt

#define __in_bcount(x)
#define __in_bcount_opt(x)

#define __out_bcount(x)
#define __out_bcount_opt(x)

#define __inout_bcount(x)
#define __inout_bcount_opt(x)

#define __out_ecount(x)
#define __out_ecount_opt(x)

#define __inout_ecount(x)
#define __inout_ecount_opt(x)

typedef int32_t DRM_LONG;
typedef uint32_t DRM_ENUM;

typedef int32_t DRM_BOOL;
typedef uint8_t DRM_BYTE;
typedef uint32_t DRM_DWORD;
typedef uint16_t DRM_WORD;
typedef void DRM_VOID;
typedef uint32_t OEM_HAL_KEY_REGISTER_INDEX;
typedef DRM_LONG DRM_RESULT;

typedef DRM_ENUM OEM_HAL_HASH_TYPE;
typedef DRM_ENUM OEM_HAL_KEY_TYPE;
typedef DRM_ENUM OEM_HAL_SAMPLE_ENCRYPTION_MODE;
typedef DRM_ENUM OEM_HAL_SIGNATURE_SCHEME;

typedef struct {
    unsigned long val[2];
} DRM_UINT64;

typedef struct {
} DRM_CRYPTO_CONTEXT;

typedef struct {
} OEM_HAL_SAMPLE_METADATA;

typedef struct {
    DRM_DWORD Data1;
    DRM_WORD  Data2;
    DRM_WORD  Data3;
    DRM_BYTE  Data4 [8];
} DRM_GUID;

typedef struct {
    DRM_WORD       wKeypairDecryptionMethod;
    DRM_WORD       wKeySize;
    DRM_BYTE       rgbInitializationVector[64];
    DRM_BYTE      *pbEncryptedKey;
    DRM_DWORD      cbEncryptedKey;
    DRM_GUID       guidKeypairDecryptionKeyID;
    DRM_WORD       wReserved;
} DRM_ENCRYPTED_KEY;

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CHECKSUM_LENGTH     7
#define DRM_AES_CK_CHECKSUM_LENGTH 8
#define LEGACY_KID_B64ENC_LEN   24
#define DRM_MAX_CHECKSUM        MAX( MAX( CHECKSUM_LENGTH, LEGACY_KID_B64ENC_LEN ), DRM_AES_CK_CHECKSUM_LENGTH )

typedef struct {
    DRM_BOOL   m_fIgnoreChecksum;
    DRM_BYTE   m_rgbChecksumData[DRM_MAX_CHECKSUM];
    DRM_DWORD  m_cbChecksum;  /* Should be 20 bytes for normal checksum, 24 bytes for V1 header checksum */
    DRM_BYTE   m_KID[16];
    DRM_BYTE  *m_pbV1KID;     /* If not NULL then use the old V1 checksum algorithm */
} DRM_BB_CHECKSUM_CONTEXT;

typedef struct
{
    const DRM_BYTE                *pbXMRLic;
    DRM_DWORD                      cbXMRLic;
    const DRM_BB_CHECKSUM_CONTEXT *poChecksum;
} DRM_XMR_UNWRAP_INFO;


typedef DRM_VOID* OEM_ENCRYPTION_HANDLE;
typedef DRM_VOID* OEM_OPAQUE_BUFFER_HANDLE;

#define OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT 0
#define OEM_HAL_KEY_TYPE_AES_KEYFILE 11

#endif /* __OEMHALDEFS_H__ */
