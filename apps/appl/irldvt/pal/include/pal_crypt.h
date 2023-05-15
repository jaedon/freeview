/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PALCRYPT_H__
#define	__PALCRYPT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <vkernel.h>
#include <irldvt_stb_params.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePAL_PVR_ENCRYPTION_TYPE_NONE,
	ePAL_PVR_ENCRYPTION_TYPE_3DES,
	ePAL_PVR_ENCRYPTION_TYPE_AES,
	ePAL_PVR_ENCRYPTION_TYPE_CURRENT /* Used for delayed record(or split) */
} PalCrypt_EncryptionType_e;

typedef struct
{
	PalCrypt_EncryptionType_e	 eEncType;
	HUINT8			 aucKey[PAL_PVR_CRYPTO_KEY_LEN];
	HUINT8			 aucIv[PAL_PVR_CRYPTO_KEY_LEN];
	HUINT8			 aucProtectionKey[PAL_PVR_CRYPTO_KEY_LEN];
} PalCrypt_Setting_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/



/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
#if 0
HERROR		PAL_CRYPT_Deskey(HUINT8 *pucKey, HUINT16 usMode);
HERROR		PAL_CRYPT_Des3key(HUINT8 *pucHexKey, HUINT16 usMode);
HERROR		PAL_CRYPT_Des(HUINT8 *pucInblock, HUINT8 *pucOutblock);
#endif

HERROR		PAL_CRYPT_Desinit(HUINT8 *pucKey);
HERROR		PAL_CRYPT_Des3init(HUINT8 *pucKey);
HERROR		PAL_CRYPT_DesDecryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock);
HERROR		PAL_CRYPT_Des3DecryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock);

HERROR		PAL_CRYPT_DesEncryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock);
HERROR		PAL_CRYPT_Des3EncryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock);



#endif /* !__PALCRYPT_H__ */
