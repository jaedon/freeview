
#ifndef SEC_IMPL_H
#define SEC_IMPL_H

#ifdef USE_OSSL_FOR_HMAC
#if 0 /* HUMAX_PLATFORM_BASE */
#include "sha.h"
#include "hmac.h"
#else
#include "openssl/sha.h"
#include "openssl/hmac.h"
#endif
#endif

#include "nexus_dma.h"
#include "nexus_security_datatypes.h"
/*#include "nexus_security_priv.h"*/
#include "nexus_key_slot.h"
#include "nexus_security.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nexus_playpump.h"

#include "nexus_hmac_sha_cmd.h"

#include "nexus_message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NO_C99
#include <tomcrypt.h>
#include <tommath.h>
#endif


#define NAGRA_TEST

/******************************************************** 
**   For debugging only 
********************************************************/
#undef BRCM_DBG
/*#define BRCM_DBG */

#ifdef BRCM_DBG

#define NOCS_CSD_TRACE(x)    printf x
#define NOCS_CSD_ERR(x)        printf x
#else
#define NOCS_CSD_TRACE(x)   
#define NOCS_CSD_ERR(x)     printf x



#endif /* BRCM_DBG */

#ifdef BRCM_DBG
#define NOCS_CTD_TRACE(x)    printf x
#define NOCS_CTD_ERR(x)        printf x
#else
#define NOCS_CTD_TRACE(x)   
#define NOCS_CTD_ERR(x)     
#endif /* BRCM_DBG */

typedef struct SecInitParameters TSecInitParameters;

typedef struct SecTerminateParameters TSecTerminateParameters;


#define SEC_NUID_LENGTH		(64)    /* in bits,  possible to be 64 or whatever*/
#define SEC_NUID_LENGTH_32		(32)    /* in bits,  possible to be 64 or whatever*/


	/* The maximum number of CSD that can be active at one time (1 video and 1 audio). */
#define SEC_R2R_NB_OF_PROTECTING_KEYS 2
#define SEC_FPK_NB_OF_PROTECTING_KEYS 2
#define SEC_SCR_NB_OF_PROTECTING_KEYS 2
#define SEC_DSC_NB_OF_PROTECTING_KEYS 2

#define SEC_MAX_IV_SIZE		(16)    /* in bytes*/
#define SEC_SHA_MAX_DATA_SIZE 384


/* deprecated CSD types are defined in CSD! */
/*
typedef enum {
	CSD_ACTION_INIT=1,
	CSD_ACTION_TERMINATE=2,		
	CSD_ACTION_REL_KEYSLOT_CA=4,
	CSD_ACTION_REL_KEYSLOT_M2M=8
	
}SCsdAction_Enum_t;

typedef enum ECsdAlgorithm
{
	SEC_Algorithm_DVB_CSA2 = 0x0,
	SEC_Algorithm_DVB_CSA3 = 0x1,
	SEC_Algorithm_NAGRA_ASA =0x10,
	SEC_Algorithm_AES_128_CBC_IV_0_SCTE52 = 0x20, 
	SEC_Algorithm_AES_128_ECB_CLEAR = 0x21,
	SEC_Algorithm_AES_128_CBC_CLEAR = 0x22,
	SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52 = 0x40,
	SEC_Algorithm_TDES_ABA_ECB_CLEAR = 0x41,
	SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR = 0x4020,
	SEC_Algorithm_RAW_AES_128_ECB_CLEAR = 0x4021,
	SEC_Algorithm_RAW_0x4022 = 0x4022,
	SEC_Algorithm_RAW_0x4023 = 0x4023,
	SEC_Algorithm_RAW_0x4024 = 0x4024,
	SEC_Algorithm_RAW_AES_128_CBC_CLEAR = 0x4026,
	SEC_Algorithm_RAW_AES_128_CTR = 0x4027,
	SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR = 0x4040,
	SEC_Algorithm_RAW_TDES_ABA_ECB_CLEAR = 0x4041,
	SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR = 0x4043
} ECsdAlgorithm;



typedef enum
{
  SCSD_CRYPTO_MODE_TDES_ECB,
  SCSD_CRYPTO_MODE_TDES_CBC,
  SCSD_CRYPTO_MODE_AES_ECB,
  SCSD_CRYPTO_MODE_AES_CBC,
  LAST_SCSD_CRYPTO_MODE

} ScsdCryptoMode;
*/

typedef enum
{
  SEC_SESSION_INVALID_TYPE=0x0,
  SEC_SESSION_STREAM_DECRYPT_TYPE,
  SEC_SESSION_STREAM_ENCRYPT_TYPE,
  SEC_SESSION_R2R_TYPE,
  SEC_SESSION_SCR_TYPE,
  SEC_SESSION_MAX
} TCipherSessionType;

typedef struct SSecCipherSession 
{
	NEXUS_KeySlotHandle keyHandle; /* only for R2R untill it is migrated to csd*/
	bool bEncrypt;
    bool bKeySet;
	TUnsignedInt16   xEmi; 
	TUnsignedInt8* initData;
	size_t		initDataSize;
    TTransportSessionId     xTransportSessionId;
    TCipherSessionType type;
    size_t              xMetadataSize;
    TUnsignedInt8*     pxMetadata;
    struct SCsdDscKeyPathHandle* pDscKeyPath;
    struct SCsdR2RKeyPathHandle* pR2RKeyPath;
    struct SCsdScrKeyPathHandle* pScrKeyPath;

    /* Context information for stream sessions */
    NEXUS_PlaypumpHandle playpumpHandle;
    TUnsignedInt8 xIv[16];
    size_t        xIvSize;
} SSecCipherSession;

typedef enum
{
  SEC_SHA_DATA_MODE_eAllin=0x0,
  SEC_SHA_DATA_MODE_eMore=0x1,
  SEC_SHA_DATA_MODE_eLast=0x10,
  
  SEC_SHA_DATA_MODE_eMax

} SecSHADataMode;


typedef struct SMessageChunk   *chunkNode; 

typedef struct SMessageChunk 
{
    const TUnsignedInt8*  pMessageChunk;
    size_t           xChunkSize;
    chunkNode   next;
} SMessageChunk;
 
typedef struct SSecHashContext 
{
#ifdef USE_OSSL_FOR_HMAC
	SHA_CTX c;
	SHA256_CTX c256;
	SHA512_CTX c384;
	HMAC_CTX hmac;
#endif
	NEXUS_HMACSHA_OpSettings hmacSha1Settings;
    SMessageChunk *pMessageChunkList;
    TUnsignedInt16   numberofChunks; 
    TUnsignedInt16   totalDataSize; 
#ifndef NO_C99
    hash_state *md;
#endif
    SecSHADataMode datamode;
} SSecHashContext;


#endif	

/* END OF FILE */	
