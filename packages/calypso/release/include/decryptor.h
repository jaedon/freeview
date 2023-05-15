#ifndef __DECRYPTOR_H__
#define __DECRYPTOR_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
* Decrypt Type
*/
typedef enum
{
	DECRYPTOR_TYPE_UNKNOWN = 0,	/**< UNKNOWN */
	DECRYPTOR_TYPE_AES_128_CTR,
	DECRYPTOR_TYPE_AES_128_CBC,
	DECRYPTOR_TYPE_AES_128_ECB,
	DECRYPTOR_TYPE_AES_128_CTR_EX,
	DECRYPTOR_TYPE_WMDrmPd,
	DECRYPTOR_TYPE_RC4,
	DECRYPTOR_TYPE_SECURE_KEYHANDLE,
	DECRYPTOR_TYPE_MAX
} DECRYPTOR_Type_e;

typedef void* DECRYPTOR_Context_t;

int DECRYPTOR_Init(void);
int DECRYPTOR_Uninit(void);

DECRYPTOR_Context_t DECRYPTOR_Create(DECRYPTOR_Type_e tPumpingType);
int DECRYPTOR_Destroy(DECRYPTOR_Context_t tDecCtx);

int DECRYPTOR_DoDecrypt(DECRYPTOR_Context_t tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv);
int DECRYPTOR_DoDecrypt_ExCTR(DECRYPTOR_Context_t tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv, unsigned char *ecount_buf, unsigned int *num);

/* SOC Memory */
void *DECRYPTOR_Malloc(unsigned int unBytes);
void DECRYPTOR_Free(void *pMemory);

void DECRYPTOR_SetThreshold(unsigned long long ullThreshold_SW);
void DECRYPTOR_GetThreshold(unsigned long long *ullThreshold_SW);

int DECRYPTOR_SetupSecureHandle(DECRYPTOR_Context_t tDecCtx, void *pSecureHandle);

#ifdef  __cplusplus
}
#endif

#endif /* __DECRYPTOR_H__ */
