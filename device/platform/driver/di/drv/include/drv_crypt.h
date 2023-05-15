#ifndef __DRV_CRYPT_H__
#define __DRV_CRYPT_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#include "di_crypt.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define ENCRYPTION_KEY_LEN 			16
#define IV_KEY_LEN					16

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_CRYPT_EndecryptMedia(DI_CRYPT_CryptMode eCryptMode,HUINT8* pucSrcData, HUINT32 uiSrcSize, HUINT8* pucDestData, HUINT8* pucKey, HUINT32 uiKeySize, HUINT8* pucIV, HUINT32 uiIVSize, HBOOL bEncrypt);
void *DRV_CRYPT_MemAlloc(HUINT32 size);
void DRV_CRYPT_MemFree(void *p);
DRV_Error DRV_CRYPT_Decrypt(stCrypt_t *stCryptInfo);
DRV_Error DRV_CRYPT_Encrypt(stCrypt_t *stCryptInfo);
void DRV_CRYPT_ExpandKeyLength(HUINT8 *psrcKey, HUINT8 *pdstKey);
DRV_Error DRV_CRYPT_Init(void);

#endif
