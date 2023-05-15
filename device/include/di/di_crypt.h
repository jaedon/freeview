#ifndef _DI_CRYPT_H_
#define _DI_CRYPT_H_
//=============================================================================
//
//  Copyright (C) 2008.  HUMAX INC.
//  All rights reserved.  No part of this program may be reproduced.
//
//
//=============================================================================
//
// Module       : di_crypt.h
// Description  : crypt (AES, DES)
//
//=============================================================================

//=============================================================================
//Include Files
//=============================================================================
#include "htype.h"
#include "di_err.h"

/******************************************************************************
* Definitions, typedef & const data
*******************************************************************************/

typedef enum
{
   DI_CRYPT_AES, // Assumed AES-CBC. So, you can use DI_CRYPT_AES_CBC also.
   DI_CRYPT_DES,
   DI_CRYPT_TDES,
   DI_CRYPT_AES_CTR,
   DI_CRYPT_AES_CBC_MAC,
   DI_CRYPT_TDES_DTCPIP,
   DI_CRYPT_AES_ECB,
   DI_CRYPT_MAX
} DI_CRYPT_CryptMode;

typedef enum
{
   DI_CRYPT_DEFAULT_SYSTEM_KEY,
   DI_CRYPT_INTERNAL_CAS_KEY,
   DI_CRYPT_USER_KEY,
   DI_CRYPT_INTERNAL_M2M_LPK_KEY
} DI_CRYPT_CryptKeyType;

typedef enum
{
   DI_CRYPT_BLOCK,
   DI_CRYPT_STREAM
} DI_CRYPT_CryptDataFormat;


typedef struct
{
	DI_CRYPT_CryptMode etCryptMode;
	DI_CRYPT_CryptKeyType etCryptKey;
	DI_CRYPT_CryptDataFormat etCryptData;
	HUINT8  *pKey;
	HUINT32 ulKeySize;
	HUINT8 *pSrcBuffer;
	HUINT8 *pDstBuffer;
	HUINT8	*pIvKey;
	HUINT32 ulBuffSize;
} stCrypt_t;


typedef enum
{
	eDI_CRYPT_VideoCodec_None			= 0x00000000,   /* not coded video */
	eDI_CRYPT_VideoCodec_Mpeg1			= 0x00000001,   /* MPEG-1 Video (ISO/IEC 11172-2) */
	eDI_CRYPT_VideoCodec_Mpeg2			= 0x00000002,   /* MPEG-2 Video (ISO/IEC 13818-2) */
	eDI_CRYPT_VideoCodec_Mpeg2Dtv		= 0x00000004,   /* MPEG2DTV Video */
	eDI_CRYPT_VideoCodec_Mpeg2Dtv_Pes	= 0x00000008,   /* MPEG2DTV_PES Video */
	eDI_CRYPT_VideoCodec_Mpeg4_Part2	= 0x00000010,   /* MPEG-4 Part 2 Video */
	eDI_CRYPT_VideoCodec_H261			= 0x00000020,   /**< H.261 Video */
	eDI_CRYPT_VideoCodec_H263			= 0x00000040,   /* H.263 Video. The value of the enum is not based on PSI standards. */
	eDI_CRYPT_VideoCodec_H264			= 0x00000080,   /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	eDI_CRYPT_VideoCodec_Vc1_Sm			= 0x00000100,   /* VC-1 Simple&Main Profile */
	eDI_CRYPT_VideoCodec_Vc1			= 0x00000200,   /* VC-1 Advanced Profile */
	eDI_CRYPT_VideoCodec_Avs			= 0x00000400,   /* AVS Video */
	eDI_CRYPT_VideoCodec_Divx_311		= 0x00000800,   /* DivX 3.11 coded video */
	eDI_CRYPT_VideoCodec_H265			= 0x00008000    /* H.265 */
} DI_CRYPT_VideoCodec;


typedef enum
{
	eDI_CRYPT_PIDTYPE_VIDEO = 0,
	eDI_CRYPT_PIDTYPE_AUDIO_MAIN,
	eDI_CRYPT_PIDTYPE_AUDIO_SUB,
	eDI_CRYPT_PIDTYPE_AUDIO_SPDIF,
	eDI_CRYPT_PIDTYPE_PCR,
	eDI_CRYPT_PIDTYPE_PES = 5,
	eDI_CRYPT_PIDTYPE_PES26 = 31,
	eDI_CRYPT_PIDTYPE_MAX
} DI_CRYPT_PIDTYPE;


#define DI_CRYPT_PID_NULL 0xFFFF

typedef struct
{
	DI_CRYPT_VideoCodec eVideoCodec;
	HUINT16 usPid[eDI_CRYPT_PIDTYPE_MAX];
} stCryptPid_t;


DI_ERR_CODE DI_CRYPT_Encrypt(stCrypt_t *stCryptInfo);
DI_ERR_CODE DI_CRYPT_Decrypt(stCrypt_t *stCryptInfo);
void *DI_CRYPT_MemAlloc(HUINT32 size);
void DI_CRYPT_MemFree(void *p);
DI_ERR_CODE DI_CRYPT_Init(void);
DI_ERR_CODE DI_CRYPT_DecryptCps(stCrypt_t *stCryptInfo, stCryptPid_t *pPid);

#endif /* _DI_AUDIO_H_ */

