/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory CAS Driver Interface
* version : loader code 의 cas_ir_util.c 파일을 기반으로 포팅.
*	- 2012.10.15 svn r.4705
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"

#include "fvk.h"
#include "fdi_common.h"
#include "fdi_cas_ir_util.h"

#include "di_err.h"
#include "di_flash.h"
#include "di_eeprom.h"
#include "di_nvram.h"
#include "di_system.h"
#include "di_factory.h"
#include "di_crypt.h"
#include "di_power.h"

#include "drv_err.h"
#include "drv_micom.h"




/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

#define DEBUG_DUMP	0	//디버깅을 위한 AES key 및 CRC dump

#define DECRYPT_USERKEY_SIZE		16

#if defined(CONFIG_ENCRYPT_APPL_IMAGE) && defined(CONFIG_CAS_IR)
#define SIZE_CAS_IR_KEY 21
#endif



/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

#if defined(SUPPORT_IRDETO_CCB_SKIP_CLEARBLK)
static HBOOL	s_bIsClerBlkInit = FALSE;
static HUINT8	s_aucClearBlk[IR_FLASH_APPL_BLOCK_SIZE];
#endif

static HUINT8 aucDecryptUserKey[FDI_CAS_IR_CRYPT_KEY_MAX][DECRYPT_USERKEY_SIZE];

static const unsigned long	_crc32Tbl[256] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};	/* _crc32Tbl */



static unsigned long HxUtil_Get32Bits(unsigned char *base);
static unsigned long HxUtil_GetCrc32(unsigned char *p, unsigned long n);
static unsigned long HxUtil_GetCrc32Fast(void *inP, unsigned long NumberOfByte);

static HERROR local_crypt_ConvertCryptModePalToDi(FDI_CAS_IR_CRYPT_CryptMode ePalMode, DI_CRYPT_CryptMode *peDiMode);
static HERROR local_crypt_ConvertKeyTypePalToDi(FDI_CAS_IR_CRYPT_CryptKeyType ePalType, DI_CRYPT_CryptKeyType *peDiType);
static HERROR local_crypt_ConvertDataFormatPalToDi(FDI_CAS_IR_CRYPT_CryptDataFormat ePalFormat, DI_CRYPT_CryptDataFormat *peDiFormat);
static HERROR local_crypt_ConvertCryptInfoPalToDi(FDI_CAS_IR_Crypt_t *pstPalInfo, stCrypt_t *pstDiInfo);
static void	*INTRN_FDI_CAS_IR_CRYPT_MemAlloc(HUINT32 size);
static void	INTRN_FDI_CAS_IR_CRYPT_MemFree(void *pvData);
static void *INTRN_FDI_CAS_IR_SYSTEM_GetMemory(HUINT32 ulSize);
static HERROR INTRN_FDI_CAS_IR_SYSTEM_ReturnMemory(void *pvBuf);
static HERROR INTRN_FDI_CAS_IR_Flash_Read(HUINT32 ulOffset, HUINT8 * pucBuffer, HUINT32 ulSize);
static HERROR	INTRN_FDI_CAS_IR_CRYPT_GetUserKey(FDI_CAS_IR_CRYPT_KeyType eKeyType, HUINT8 *OutData);
static HERROR	INTRN_FDI_CAS_IR_CRYPT_SetUserKey(FDI_CAS_IR_CRYPT_KeyType eKeyType, HUINT8 *InData, HUINT32 uSize);
static HERROR	INTRN_FDI_CAS_IR_CRYPT_Decrypt(FDI_CAS_IR_Crypt_t *pstCryptInfo);
static HERROR	INTRN_FDI_CAS_IR_CRYPT_Encrypt(FDI_CAS_IR_Crypt_t *pstCryptInfo);

#if defined(CONFIG_ENCRYPT_APPL_IMAGE) && defined(CONFIG_CAS_IR)
static HERROR INTRN_FDI_CAS_IR_SYSTEM_IR_LoadEncryptionKey(void);
#endif

static HERROR local_nvram_ConvertPalToDi(FDI_CAS_IR_NVRAM_FIELD_T ePalField, DI_NVRAM_FIELD_T *peDiField);
static HERROR INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize);
static HERROR INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize);

static HERROR local_cas_ir_GetOffsetCrcAreaByBlockNumber(HUINT32 ulBlkNum, HUINT32 *pulOffset);
static void *	INTRN_FDI_CAS_IR_MemAlloc(HUINT32 ulSize);
static HINT32	INTRN_FDI_CAS_IR_MemFree(void *pBuf);
static HERROR	INTRN_FDI_CAS_IR_ReadFlash(HUINT32 ulOffset, HUINT8 *pucBuffer, HUINT32 ulSize);
static HERROR INTRN_FDI_CAS_IR_MakePartCRC32ByBlkNum(HUINT32 ulBlkNum, HUINT32 *pulCrcOut, HUINT8 *pucP_Mac);
static void INTRN_FDI_CAS_IR_PrepareCpcbFromDnHeader(HUINT8 *pucCpcbBuffer, HUINT32 ulSize, DLH_STRUCT *pstDnHeader);
static HUINT32 INTRN_FDI_CAS_IR_GetElapsedTimeInSeconds(HBOOL bReset);
static HERROR	INTRN_FDI_CAS_IR_EraseCPCB(void);
static HERROR	INTRN_FDI_CAS_IR_WriteCPCB(HUINT8 *pucData, HUINT16 usOffset, HUINT32 ulLength);
static HERROR	INTRN_FDI_CAS_IR_ReadCPCB(HUINT8 *pucData, HUINT16 usOffset, HUINT32 ulLength);
static HERROR	INTRN_FDI_CAS_IR_Read_pureCPCB(HUINT8 *pucData, HUINT16 usOffset, HUINT32 ulLength);


#if defined(CONFIG_CAS_IR_LOADER_V32)
static HERROR  INTRN_FDI_CAS_IR_GetMACbuf(HUINT8 *pbBlock, HUINT32 uLen, HUINT8 *pOutbuffer);
static void INTRN_FDI_CAS_IR_UpdatePureCpcb(HUINT8 *pucBaseBuf, HUINT32 ulSize);
static HERROR  INTRN_FDI_CAS_IR_UpdateSPCB(HUINT8 *pucBaseBuf, HUINT32 ulSize);
static HERROR INTRN_FDI_CAS_IR_MakeSPCB(void);
static HERROR INTRN_FDI_CAS_IR_PrintSPCBHeader(void);
#endif


/* *************************************************************************************************** */
#define ___UTIL_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

static unsigned long HxUtil_Get32Bits(unsigned char *base)
{
	unsigned long temp = 0 ;

	temp |= *(base + 0) ;
	temp <<= 8 ;
	temp |= *(base + 1) ;
	temp <<= 8 ;
	temp |= *(base + 2) ;
	temp <<= 8 ;
	temp |= *(base + 3) ;

	return temp ;
} /* HxUtil_Get32Bits */

static unsigned long HxUtil_GetCrc32(unsigned char *p, unsigned long n)
{
	unsigned long	crc, *tbl;
	unsigned long	len;

	len = n;

	crc = 0xFFFFFFFF;
	tbl = (unsigned long*)_crc32Tbl;
	while (n--)
	{
		  crc = (crc >> 8) ^ tbl[(crc ^ *p) & 0xff];
		  p++;
	}
	return ~crc;
} /* HxUtil_GetCrc32 */


static unsigned long HxUtil_GetCrc32Fast(void *inP, unsigned long NumberOfByte)
{
	register unsigned long		CRC ;
	unsigned long *aligned_4;
	unsigned long pre_count, end_count,loop_4;
	unsigned long data;
	unsigned char *BufferPtr = inP;

	if (NumberOfByte < 4)
	{
		return HxUtil_GetCrc32(inP, NumberOfByte);
	}
	pre_count = ((unsigned long)BufferPtr & 0x03);
	if(pre_count!=0)
		pre_count = 4-pre_count;
	aligned_4 = (unsigned long *)((unsigned long)BufferPtr + pre_count);
	loop_4 = (NumberOfByte-pre_count)>>2;
	end_count = NumberOfByte-(loop_4<<2)-pre_count;

	CRC = 0xFFFFFFFF;
	while((pre_count--)!=0)
	{
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ *BufferPtr++) & 0xff] ;
	}

	BufferPtr=(unsigned char *)aligned_4+(loop_4<<2);

	while((loop_4--)!=0)
	{
		data= *aligned_4++;
#if defined(CONFIG_BIG_ENDIAN)
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data>>24)) & 0xff];
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data>>16)) & 0xff];
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data>> 8)) & 0xff];
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data	 )) & 0xff];
#else
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data	 )) & 0xff];
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data>> 8)) & 0xff];
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data>>16)) & 0xff];
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ (data>>24)) & 0xff];
#endif
	}

	while((end_count--)!=0)
	{
		CRC = (CRC >> 8) ^ _crc32Tbl[(CRC ^ *BufferPtr++) & 0xff] ;
	}

	return ~CRC ;

} /* GetCrc16 */


/* *************************************************************************************************** */
#define ___SYSTEM_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

static void *INTRN_FDI_CAS_IR_SYSTEM_GetMemory(HUINT32 ulSize)
{
	if(ulSize == 0)
		return NULL;

	return FVK_MEM_Alloc(ulSize);
}

static HERROR INTRN_FDI_CAS_IR_SYSTEM_ReturnMemory(void *pvBuf)
{
	HINT32	nRes = 0;
	if(pvBuf != NULL)
	{
		nRes = FVK_MEM_Free(pvBuf);
	}

	return (nRes == 0) ? ERR_OK : ERR_FAIL;
}

#if defined(CONFIG_ENCRYPT_APPL_IMAGE) && defined(CONFIG_CAS_IR)
static HERROR INTRN_FDI_CAS_IR_SYSTEM_IR_LoadEncryptionKey(void)
{
	HERROR				 hErr;
	DI_ERR_CODE			 etRtnCode;
	HUINT16				 usKeySize = 0;
	HUINT8				*pucAddress = NULL ;
	void				*pSrc = NULL, *pDst = NULL;
	HERROR			 rtCode = ERR_OK;
	FDI_CAS_IR_Crypt_t 		 stCryptInfo;
	HUINT8 uccount;

	pucAddress = INTRN_FDI_CAS_IR_SYSTEM_GetMemory(SIZE_CAS_IR_KEY);
	if(pucAddress == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s:%d Error ERR_OUTOFMEMORY\n", __FUNCTION__, __LINE__));
		return ERR_FAIL;
	}

#if 1
	etRtnCode = INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_PRIVATE, 0, pucAddress, SIZE_CAS_IR_KEY);
#else
	etRtnCode = DI_FLASH_Read(ulAddress, pucAddress, usStoredKeySize);
#endif
	if (etRtnCode != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("Error read Flash : %d, %x\n", etRtnCode, pucAddress));
		rtCode = ERR_FAIL;
		goto END_FUNC;
	}
#if DEBUG_DUMP
// aeskey 확인	
	FDI_PRINT(FDI_PRT_ERROR, ("\n ************************************** nvram secret key ***********************************\n"));
	for( uccount=0 ; uccount < SIZE_CAS_IR_KEY ; uccount++)
	{
		FDI_PRINT(FDI_PRT_ERROR, (" 0x%02x", pucAddress[uccount]));
	}
	FDI_PRINT(FDI_PRT_ERROR, ("\n ******************************************************************************************\n"));
#endif

	pSrc = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(SIZE_CAS_IR_KEY);
	if(pSrc == NULL){
		FDI_PRINT(FDI_PRT_ERROR, ("%s:%d INTRN_FDI_CAS_IR_CRYPT_MemAlloc failed\n", __FUNCTION__, __LINE__));
		rtCode = ERR_FAIL;
		goto END_FUNC;
	}

	pDst = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(SIZE_CAS_IR_KEY);
	if(pDst == NULL){
		FDI_PRINT(FDI_PRT_ERROR, ("%s:%d INTRN_FDI_CAS_IR_CRYPT_MemAlloc failed\n", __FUNCTION__, __LINE__));
		rtCode = ERR_FAIL;
		goto END_FUNC;
	}

	if (pucAddress[0] == 0x48 && pucAddress[1] == 0x4B && pucAddress[2] == 0x53)
	{
		// encryption이 안된 상태다.. encryption을 하자.
		FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CAS_IR_SYSTEM_LoadEncryptionKey] Encrypt\n"));

#if 1
		etRtnCode = INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_PRIVATE, 0, pucAddress, SIZE_CAS_IR_KEY);
#else
		etRtnCode = DI_FLASH_Read(ulAddress, pTempBuffer, usStoredKeySize);
#endif

		if (etRtnCode != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Error read Flash : %d, %x\n", etRtnCode, pucAddress));
			rtCode = ERR_FAIL;
			goto END_FUNC;
		}

		VK_MEM_Memcpy(pSrc, (void *)pucAddress, SIZE_CAS_IR_KEY);

		// key 부분만 encrypt해요.
		FVK_MEM_Set(&stCryptInfo, 0, sizeof(FDI_CAS_IR_Crypt_t));
		stCryptInfo.etCryptMode = FDI_CAS_IR_CRYPT_AES;
		stCryptInfo.etCryptKey  = FDI_CAS_IR_CRYPT_DEFAULT_SYSTEM_KEY;
		stCryptInfo.etCryptData = FDI_CAS_IR_CRYPT_BLOCK;
		stCryptInfo.pKey 		= NULL;
		stCryptInfo.ulKeySize  = 16;
		stCryptInfo.pSrcBuffer = pSrc;
		stCryptInfo.pDstBuffer = pDst;
		stCryptInfo.ulBuffSize = SIZE_CAS_IR_KEY;
		stCryptInfo.pIvKey = NULL;

		hErr  = INTRN_FDI_CAS_IR_CRYPT_Encrypt(&stCryptInfo);
		if (hErr != ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Error encryptiion fail : %d, %x\n", etRtnCode));
			rtCode = ERR_FAIL;
			goto END_FUNC;
		}

#if DEBUG_DUMP
// aeskey 확인	
		FDI_PRINT(FDI_PRT_ERROR, ("\n ************************************** encrypt AES key ***********************************\n"));
		for( uccount=0 ; uccount < SIZE_CAS_IR_KEY ; uccount++)
		{
			FDI_PRINT(FDI_PRT_ERROR, (" 0x%02x", stCryptInfo.pDstBuffer[uccount]));
		}
		FDI_PRINT(FDI_PRT_ERROR, ("\n ******************************************************************************************\n"));
#endif

		FDI_PRINT(FDI_PRT_DBGINFO, ("\t\t Encrypt src : 0x%08x, dst : 0x%08x\n", *((unsigned long*) pSrc),
					*((unsigned long*) pDst)));		

		// AES KEY 영역을 다시 flash에 써요.
#if 1
		etRtnCode = INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_PRIVATE, 0, stCryptInfo.pDstBuffer, SIZE_CAS_IR_KEY);
#else
		etRtnCode = DI_FLASH_Write(ulAddress, stCryptInfo.pDstBuffer, usStoredKeySize);
#endif
		if (etRtnCode != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Error write Flash : %d, %x\n", etRtnCode, pucAddress));
			rtCode = ERR_FAIL;
			goto END_FUNC;
		}

		FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CAS_IR_SYSTEM_LoadEncryptionKey] Encrypt success\n"));
	}
	else
	{
		VK_MEM_Memcpy(pSrc, (void *)pucAddress, SIZE_CAS_IR_KEY);

		// decrypt 하자.
		FVK_MEM_Set(&stCryptInfo, 0, sizeof(FDI_CAS_IR_Crypt_t));
		stCryptInfo.etCryptMode = FDI_CAS_IR_CRYPT_AES;
		stCryptInfo.etCryptKey = FDI_CAS_IR_CRYPT_DEFAULT_SYSTEM_KEY;
		stCryptInfo.etCryptData = FDI_CAS_IR_CRYPT_BLOCK;
		stCryptInfo.pKey = NULL;
		stCryptInfo.ulKeySize = 16;   // system key로 encryption 할 때 확인 해라.
		stCryptInfo.pSrcBuffer = pSrc;
		stCryptInfo.pDstBuffer = pDst;		
		stCryptInfo.ulBuffSize = SIZE_CAS_IR_KEY;
		stCryptInfo.pIvKey = NULL;

		hErr  = INTRN_FDI_CAS_IR_CRYPT_Decrypt(&stCryptInfo);
		if (hErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Error decryptiion fail : %d, %x\n", etRtnCode));
			rtCode = ERR_FAIL;
			goto END_FUNC;
		}

#if DEBUG_DUMP
// aeskey 확인	
		FDI_PRINT(FDI_PRT_ERROR, ("\n ************************************** decrypt AES key ***********************************\n"));
		for( uccount=0 ; uccount < SIZE_CAS_IR_KEY ; uccount++)
		{
			FDI_PRINT(FDI_PRT_ERROR, (" 0x%02x", stCryptInfo.pDstBuffer[uccount]));
		}
		FDI_PRINT(FDI_PRT_ERROR, ("\n ******************************************************************************************\n"));
#endif

		// decrypt된 부분만 다시 copy해요
		VK_MEM_Memcpy(pucAddress, pDst, SIZE_CAS_IR_KEY);
		if (pucAddress[0] != 0x48 || pucAddress[1] != 0x4B || pucAddress[2] != 0x53) // org : if (pucAddress[0] != 0x48 && pucAddress[1] != 0x4B && pucAddress[2] != 0x53)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Error identifier : %x, %x, %x\n", pucAddress[0], pucAddress[1], pucAddress[2]));
			rtCode = ERR_FAIL;
			goto END_FUNC;
		}
		
#if 0
		// aeskey 확인
		PrintError("\n ******************* decrypt key ****************\n");
		for( uccount=0 ; uccount < SIZE_CAS_IR_KEY ; uccount++)
		{
			PrintError(" 0x%02x", pucAddress[uccount]);
		}
		PrintError("\n *********************************************\n");
#endif
		FDI_PRINT(FDI_PRT_DBGINFO, ("Key decryption success !!! \n"));
	}

	usKeySize  = pucAddress[3] << 8;
	usKeySize |= pucAddress[4];

	pucAddress += 5;


	/*  key 값을 저장. */
	FDI_PRINT(FDI_PRT_DBGINFO, ("Key decryption size(%d) !!! \n",usKeySize));
	INTRN_FDI_CAS_IR_CRYPT_SetUserKey(FDI_CAS_IR_CRYPT_KEY_FOR_APPL_IMG, pucAddress, usKeySize);

END_FUNC:

#if 1 // 아래 buffer를 다른 모듈에서 사용할 수 있다 */
	if(pSrc != NULL)
	{
		INTRN_FDI_CAS_IR_CRYPT_MemFree(pSrc);
	}

	if(pDst != NULL)
	{
		INTRN_FDI_CAS_IR_CRYPT_MemFree(pDst);
	}
#endif
#if 0 // system memory free 할 때 죽는다 이유는 ??????? */
	if(pucAddress != NULL)
	{
		INTRN_FDI_CAS_IR_SYSTEM_ReturnMemory(pucAddress);
	}
#endif
	return rtCode;
}
#endif



/* *************************************************************************************************** */
#define ___CRYPT_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

static HERROR local_crypt_ConvertCryptModePalToDi(FDI_CAS_IR_CRYPT_CryptMode ePalMode, DI_CRYPT_CryptMode *peDiMode)
{
	switch(ePalMode)
	{
		case FDI_CAS_IR_CRYPT_AES:
			*peDiMode = DI_CRYPT_AES;
			break;

		case FDI_CAS_IR_CRYPT_DES:
			*peDiMode = DI_CRYPT_DES;
			break;

		case FDI_CAS_IR_CRYPT_TDES:
			*peDiMode = DI_CRYPT_TDES;
			break;

		case FDI_CAS_IR_CRYPT_AES_CTR:
			*peDiMode = DI_CRYPT_AES_CTR;
			break;

		case FDI_CAS_IR_CRYPT_AES_CBC_MAC:
			*peDiMode = DI_CRYPT_AES_CBC_MAC;
			break;

		default:
			FDI_PRINT(FDI_PRT_ERROR, ("%s, Error Unkown Crypt Mode\n", __FUNCTION__));
			return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_crypt_ConvertKeyTypePalToDi(FDI_CAS_IR_CRYPT_CryptKeyType ePalType, DI_CRYPT_CryptKeyType *peDiType)
{
	switch(ePalType)
	{
		case FDI_CAS_IR_CRYPT_DEFAULT_SYSTEM_KEY:
			*peDiType = DI_CRYPT_DEFAULT_SYSTEM_KEY;
			break;

		case FDI_CAS_IR_CRYPT_INTERNAL_CAS_KEY:
			*peDiType = DI_CRYPT_INTERNAL_CAS_KEY;
			break;

		case FDI_CAS_IR_CRYPT_USER_KEY:
			*peDiType = DI_CRYPT_USER_KEY;
			break;

		default:
			FDI_PRINT(FDI_PRT_ERROR, ("%s, Error Unkown Crypt Key Type\n", __FUNCTION__));
			return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_crypt_ConvertDataFormatPalToDi(FDI_CAS_IR_CRYPT_CryptDataFormat ePalFormat, DI_CRYPT_CryptDataFormat *peDiFormat)
{
	switch(ePalFormat)
	{
		case FDI_CAS_IR_CRYPT_BLOCK:
			*peDiFormat = DI_CRYPT_BLOCK;
			break;

		case FDI_CAS_IR_CRYPT_STREAM:
			*peDiFormat = DI_CRYPT_STREAM;
			break;

		default:
			FDI_PRINT(FDI_PRT_ERROR, ("%s, Error Unkown Crypt Data Format\n", __FUNCTION__));
			return ERR_FAIL;
	}

	return ERR_OK;
}


static HERROR local_crypt_ConvertCryptInfoPalToDi(FDI_CAS_IR_Crypt_t *pstPalInfo, stCrypt_t *pstDiInfo)
{
	HERROR hErr = ERR_OK;

	if(pstPalInfo == NULL || pstDiInfo == NULL)
	{
		return ERR_FAIL;
	}

	hErr = local_crypt_ConvertCryptModePalToDi(pstPalInfo->etCryptMode, &pstDiInfo->etCryptMode);

	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = local_crypt_ConvertKeyTypePalToDi(pstPalInfo->etCryptKey, &pstDiInfo->etCryptKey);

	if(hErr != ERR_OK)
	{
		return hErr;
	}
/* 이전에 local_crypt_ConvertCryptModePalToDi 로 되어 있었다 확인 필.*/
	hErr = local_crypt_ConvertDataFormatPalToDi(pstPalInfo->etCryptData, &pstDiInfo->etCryptData);

	if(hErr != ERR_OK)
	{
		return hErr;
	}

	pstDiInfo->pKey 		= pstPalInfo->pKey;
	pstDiInfo->ulKeySize	= pstPalInfo->ulKeySize;
	pstDiInfo->pSrcBuffer	= pstPalInfo->pSrcBuffer;
	pstDiInfo->pDstBuffer	= pstPalInfo->pDstBuffer;
	pstDiInfo->pIvKey		= pstPalInfo->pIvKey;
	pstDiInfo->ulBuffSize	= pstPalInfo->ulBuffSize;

	return hErr;
}


static void	*INTRN_FDI_CAS_IR_CRYPT_MemAlloc(HUINT32 size)
{
#if 1
	return DI_CRYPT_MemAlloc(size);
#else
	return FVK_MEM_Alloc(size);
#endif
}

static void	INTRN_FDI_CAS_IR_CRYPT_MemFree(void *pvData)
{
	if(pvData == NULL)
	{
		return;
	}

#if 1
	DI_CRYPT_MemFree(pvData);
#else
	FVK_MEM_Free(pvData);
#endif
}





static HERROR INTRN_FDI_CAS_IR_Flash_Read(HUINT32 ulOffset, HUINT8 * pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE diErr;

#if defined(FACTORY_USE_NAND_FLASH) //DST_yhkim
	diErr = DI_NANDFLASH_Read(ulOffset, pucBuffer, ulSize);
#else	
	diErr = DI_FLASH_Read(ulOffset, pucBuffer, ulSize);
#endif

	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error:\n", __FUNCTION__, __LINE__));
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	INTRN_FDI_CAS_IR_CRYPT_GetUserKey(FDI_CAS_IR_CRYPT_KeyType eKeyType, HUINT8 *OutData)
{
	if(OutData == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("\n Fail Get ucDecryptUserKey \n"));
		return ERR_FAIL;
	}

	FVK_MEM_Copy(OutData, aucDecryptUserKey[eKeyType], DECRYPT_USERKEY_SIZE);
#if DEBUG_DUMP
{
	HUINT8 uccount = 0;
	FDI_PRINT(FDI_PRT_ERROR, ("\n ****************************** Get ucDecryptUserKey **********************************\n"));
	for( uccount=0 ; uccount < DECRYPT_USERKEY_SIZE ; uccount++)
	{
		FDI_PRINT(FDI_PRT_ERROR, (" 0x%02x", OutData[uccount]));
	}
	FDI_PRINT(FDI_PRT_ERROR, ("\n ************************************************************************************\n"));
}
#endif
	return ERR_OK;
}

static HERROR	INTRN_FDI_CAS_IR_CRYPT_SetUserKey(FDI_CAS_IR_CRYPT_KeyType eKeyType, HUINT8 *InData, HUINT32 uSize)
{
	if(InData == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("\n Fail Set ucDecryptUserKey \n"));
		return ERR_FAIL;
	}

	FVK_MEM_Copy(aucDecryptUserKey[eKeyType], InData, uSize);
#if DEBUG_DUMP
{
	HUINT8 uccount = 0;
	FDI_PRINT(FDI_PRT_ERROR, ("\n ****************************** Set ucDecryptUserKey **********************************\n"));
	for( uccount=0 ; uccount < DECRYPT_USERKEY_SIZE ; uccount++)
	{
		FDI_PRINT(FDI_PRT_ERROR, (" 0x%02x", aucDecryptUserKey[eKeyType][uccount]));
	}
	FDI_PRINT(FDI_PRT_ERROR, ("\n ************************************************************************************\n"));
}
#endif
	return ERR_OK;
}


static HERROR	INTRN_FDI_CAS_IR_CRYPT_Decrypt(FDI_CAS_IR_Crypt_t *pstCryptInfo)
{
	DI_ERR_CODE diErr = DI_ERR_OK;
	stCrypt_t stDiInfo;

	if(local_crypt_ConvertCryptInfoPalToDi(pstCryptInfo, &stDiInfo) != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Error local_crypt_ConvertCryptInfoPalToDi [%d]\n", __FUNCTION__));
		return ERR_FAIL;
	}

#if 1
	diErr = DI_CRYPT_Decrypt(&stDiInfo);
#else
	FVK_MEM_Copy(pstCryptInfo->pDstBuffer, pstCryptInfo->pSrcBuffer, pstCryptInfo->ulBuffSize);
#endif

	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Error DI_CRYPT_Decrypt [%d]\n", __FUNCTION__, diErr));
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	INTRN_FDI_CAS_IR_CRYPT_Encrypt(FDI_CAS_IR_Crypt_t *pstCryptInfo)
{
	DI_ERR_CODE diErr = DI_ERR_OK;
	stCrypt_t stDiInfo;

	if(local_crypt_ConvertCryptInfoPalToDi(pstCryptInfo, &stDiInfo) != ERR_OK)
	{
		return ERR_FAIL;
	}

#if 1
	diErr = DI_CRYPT_Encrypt(&stDiInfo);
#else
	FVK_MEM_Copy(pstCryptInfo->pDstBuffer, pstCryptInfo->pSrcBuffer, pstCryptInfo->ulBuffSize);
#endif

	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Error DI_CRYPT_Encrypt [%d]\n", __FUNCTION__, diErr));
		return ERR_FAIL;
	}

	return ERR_OK;
}




/* *************************************************************************************************** */
#define ___NVRAM_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


static HERROR local_nvram_ConvertPalToDi(FDI_CAS_IR_NVRAM_FIELD_T ePalField, DI_NVRAM_FIELD_T *peDiField)
{
	switch(ePalField)
	{
		case FDI_CAS_IR_NVRAM_FIELD_SYSTEM_ID:
			*peDiField = DI_NVRAM_FIELD_SYSTEM_ID;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_SFLASH:
			*peDiField = DI_NVRAM_FIELD_SFLASH;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_SERIAL_NO:
			*peDiField = DI_NVRAM_FIELD_SERIAL_NO;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_MAC_ADDR:
			*peDiField = DI_NVRAM_FIELD_MAC_ADDR;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_HDCP_KEY:
			*peDiField = DI_NVRAM_FIELD_HDCP_KEY;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_DTCP_KEY:
			*peDiField = DI_NVRAM_FIELD_DTCP_KEY;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CI_PLUS_KEY:
			*peDiField = DI_NVRAM_FIELD_CI_PLUS_KEY;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_PARING_DATA:
			*peDiField = DI_NVRAM_FIELD_PARING_DATA;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_PRIVATE:
			*peDiField = DI_NVRAM_FIELD_PRIVATE;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CSC_KEY:
			*peDiField = DI_NVRAM_FIELD_CSC_KEY;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CM_SERIAL_NO:
			*peDiField = DI_NVRAM_FIELD_CM_SERIAL_NO;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CM_MAC_ADDR:
			*peDiField = DI_NVRAM_FIELD_CM_MAC_ADDR;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CM_MAC_ADDR_ANOTHER:
			*peDiField = DI_NVRAM_FIELD_CM_MAC_ADDR_ANOTHER;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_BBCB:
			*peDiField = DI_NVRAM_FIELD_IRDETO_BBCB;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_UC_SECURE_ID:
			*peDiField = DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA:
			*peDiField = DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA;
			break;

		case FDI_CAS_IR_NVRAM_FIELD_OTA_TYPE1:				// FDI_CAS_IR_NVRAM_OTA_TYPE_E
			*peDiField = DI_NVRAM_FIELD_OTA_TYPE1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTA_TYPE2: 						// FDI_CAS_IR_NVRAM_OTA_TYPE_E
			*peDiField = DI_NVRAM_FIELD_OTA_TYPE2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTA_TYPE3:						// FDI_CAS_IR_NVRAM_OTA_TYPE_E
			*peDiField = DI_NVRAM_FIELD_OTA_TYPE3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CH_TYPE1:						// FDI_CAS_IR_NVRAM_OTA_CHANNEL_E
			*peDiField = DI_NVRAM_FIELD_CH_TYPE1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CH_TYPE2:						// FDI_CAS_IR_NVRAM_OTA_CHANNEL_E
			*peDiField = DI_NVRAM_FIELD_CH_TYPE2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CH_TYPE3:						// FDI_CAS_IR_NVRAM_OTA_CHANNEL_E
			*peDiField = DI_NVRAM_FIELD_CH_TYPE3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTA_FLAG1:						// FDI_CAS_IR_NVRAM_OTA_FLAG_E
			*peDiField = DI_NVRAM_FIELD_OTA_FLAG1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTA_FLAG2:						// FDI_CAS_IR_NVRAM_OTA_FLAG_E
			*peDiField = DI_NVRAM_FIELD_OTA_FLAG2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTA_FLAG3:						// FDI_CAS_IR_NVRAM_OTA_FLAG_E
			*peDiField = DI_NVRAM_FIELD_OTA_FLAG3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TUNER_ID1:						// Tuner Id
			*peDiField = DI_NVRAM_FIELD_TUNER_ID1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TUNER_ID2:						// Tuner Id
			*peDiField = DI_NVRAM_FIELD_TUNER_ID2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TUNER_ID3:						// Tuner Id
			*peDiField = DI_NVRAM_FIELD_TUNER_ID3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ANTENA_POWER1:					// External Antena Power for Ter: 0:Off/1:On
			*peDiField = DI_NVRAM_FIELD_ANTENA_POWER1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ANTENA_POWER2:					// External Antena Power for Ter: 0:Off/1:On
			*peDiField = DI_NVRAM_FIELD_ANTENA_POWER2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ANTENA_POWER3:					// External Antena Power for Ter: 0:Off/1:On
			*peDiField = DI_NVRAM_FIELD_ANTENA_POWER3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ES_PID1:							// Element Stream PID for OTA
			*peDiField = DI_NVRAM_FIELD_ES_PID1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ES_PID2:							// Element Stream PID for OTA
			*peDiField = DI_NVRAM_FIELD_ES_PID2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ES_PID3:							// Element Stream PID for OTA
			*peDiField = DI_NVRAM_FIELD_ES_PID3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TRANSACTION_ID1:					// Transaction Id for Nordic OTA
			*peDiField = DI_NVRAM_FIELD_TRANSACTION_ID1;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TRANSACTION_ID2:					// Transaction Id for Nordic OTA
			*peDiField = DI_NVRAM_FIELD_TRANSACTION_ID2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TRANSACTION_ID3:					// Transaction Id for Nordic OTA
			*peDiField = DI_NVRAM_FIELD_TRANSACTION_ID3;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CH_INFO_S:						// FDI_CAS_IR_CHANNEL_PARAM
			*peDiField = DI_NVRAM_FIELD_CH_INFO_S;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CH_INFO_T:						// FDI_CAS_IR_CHANNEL_PARAM
			*peDiField = DI_NVRAM_FIELD_CH_INFO_T;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CH_INFO_C:						// FDI_CAS_IR_CHANNEL_PARAM
			*peDiField = DI_NVRAM_FIELD_CH_INFO_C;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_NET_PORTAL_IP:					// portal ip address
			*peDiField = DI_NVRAM_FIELD_NET_PORTAL_IP;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_NET_DOWN_PATH:					// Network Download Path
			*peDiField = DI_NVRAM_FIELD_NET_DOWN_PATH;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_NET_DEVICE_ID:					// ethernet device id 0 = eth0
			*peDiField = DI_NVRAM_FIELD_NET_DEVICE_ID;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_NET_IP_CONFIG:					// DHCP/Manual
			*peDiField = DI_NVRAM_FIELD_NET_IP_CONFIG;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_NET_IP_SETTING:					// Manual IP setting
			*peDiField = DI_NVRAM_FIELD_NET_IP_SETTING;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTA_FTP_INFO:					// FDI_CAS_IR_NVRAM_FTP_OTA_SETTING_T
			*peDiField = DI_NVRAM_FIELD_OTA_FTP_INFO;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_FILE_SIZE:						// file size of backgrould ota
			*peDiField = DI_NVRAM_FIELD_FILE_SIZE;
			break;

		case FDI_CAS_IR_NVRAM_FIELD_LOADER_VERSION:					// Version number of boot Loader
			*peDiField = DI_NVRAM_FIELD_LOADER_VERSION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTALDR_VERSION:					// Version number of ota Loader
			*peDiField = DI_NVRAM_FIELD_OTALDR_VERSION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_FACLDR_VERSION:					// Version number of factory loader
			*peDiField = DI_NVRAM_FIELD_FACLDR_VERSION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_APP_VERSION:						// Version number of Application
			*peDiField = DI_NVRAM_FIELD_APP_VERSION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_APP_VERSION_NEXT:				// next app version for ota
			*peDiField = DI_NVRAM_FIELD_APP_VERSION_NEXT;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_LOADER_VERSION_NEXT:				// next launcher loader version for ota
			*peDiField = DI_NVRAM_FIELD_LOADER_VERSION_NEXT;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OTALDR_VERSION_NEXT:				// next ota loader version
			*peDiField = DI_NVRAM_FIELD_OTALDR_VERSION_NEXT;
			break;

		case FDI_CAS_IR_NVRAM_FIELD_LANGUAGE:						// OSD language code in ISO 639 code
			*peDiField = DI_NVRAM_FIELD_LANGUAGE;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_COUNTRY_CODE:					// HUINT8 a[3]
			*peDiField = DI_NVRAM_FIELD_COUNTRY_CODE;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_TV_STANDARD:						// FDI_CAS_IR_VIDEO_SD_STANDARD
			*peDiField = DI_NVRAM_FIELD_TV_STANDARD;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_SCART_RGB:						// FDI_CAS_IR_NVRAM_SCART_SETTING_VALUE
			*peDiField = DI_NVRAM_FIELD_SCART_RGB;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_HD_RESOULTION:					// FDI_CAS_IR_NVRAM_VIDEO_RESOLUTION_E
			*peDiField = DI_NVRAM_FIELD_HD_RESOULTION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_FIELD_RATE:						// FDI_CAS_IR_NVRAM_HDMI_VIDEO_FIELD_RATE_E
			*peDiField = DI_NVRAM_FIELD_FIELD_RATE;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_SD_STANDARD:						// FDI_CAS_IR_NVRAM_HDMI_VIDEO_FIELD_RATE_E
			*peDiField = DI_NVRAM_FIELD_SD_STANDARD;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OAD_VERSION:						// for MHEG ???
			*peDiField = DI_NVRAM_FIELD_OAD_VERSION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_RCU_CUSTOM_CODE:					// for RCU custom code
			*peDiField = DI_NVRAM_FIELD_RCU_CUSTOM_CODE;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_RCU_CUSTOM_CODE2:				// for alternative RCU custom code
			*peDiField = DI_NVRAM_FIELD_RCU_CUSTOM_CODE2;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_PANEL_DISPLAY:					// FDI_CAS_IR_OTA_PANELDISPLAY_t
			*peDiField = DI_NVRAM_FIELD_PANEL_DISPLAY;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_PANEL_DIMMING:					// HUINT8
			*peDiField = DI_NVRAM_FIELD_PANEL_DIMMING;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_OSD_DISPLAY:						// FDI_CAS_IR_NVRAM_OSD_DISPLAY_E
			*peDiField = DI_NVRAM_FIELD_OSD_DISPLAY;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_START_BANK_FLAG:					// 두 개의 bank중 어디로 부팅 할 지 결정
			*peDiField = DI_NVRAM_FIELD_START_BANK_FLAG;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CHECK_USB:						// 부팅 시 USB 마운트해서 hdf를 검사할지 말지 결정
			*peDiField = DI_NVRAM_FIELD_CHECK_USB;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_CHECK_USB_COUNT:					// 위 동작 실패 시 몇 번 재 시도 할지 count를 저장
			*peDiField = DI_NVRAM_FIELD_CHECK_USB_COUNT;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_ASPECT_RATIO:					// FDI_CAS_IR_NVRAM_ASPECT_RATIO_E
			*peDiField = DI_NVRAM_FIELD_ASPECT_RATIO;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_USAGE_ID:						// for UPC OTA
			*peDiField = DI_NVRAM_FIELD_USAGE_ID;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_BIN8K:							// for eeprom emulation
			*peDiField = DI_NVRAM_FIELD_BIN8K;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB:						// for IRDETO CPCB
			*peDiField = DI_NVRAM_FIELD_IRDETO_CPCB;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED: 			// for IRDETO CPCB (encrypted)
			*peDiField = DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_CA_PARTITION:				// for IRDETO CA PARTITION
			*peDiField = DI_NVRAM_FIELD_IRDETO_CA_PARTITION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_LOADER_PARTITION:			// for IRDETO LOADER PARTITION
			*peDiField = DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_IR_PARTITION:				// for IRDETO IR PARTITION
			*peDiField = DI_NVRAM_FIELD_IRDETO_IR_PARTITION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION:		// for IRDETO IR PARTITION (back up)
			*peDiField = DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_DISPLAY_FACTORY_INFO:		// for display factory info
			*peDiField = DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_FACTORY_GANG_WRITED:		//for factory gang writed
			*peDiField = DI_NVRAM_FIELD_FACTORY_GANG_WRITED;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_FACTORY_GANG_CRC:		// for display factory CRC
			*peDiField = DI_NVRAM_FIELD_GANG_CRC;
			break;

		case FDI_CAS_IR_NVRAM_FIELD_RAWFS:
			*peDiField = DI_NVRAM_FIELD_RAWFS;
			break;
		case FDI_CAS_IR_NVRAM_FIELD_DUMMY:
			*peDiField = DI_NVRAM_FIELD_DUMMY;
			break;
		default:
			FDI_PRINT(FDI_PRT_ERROR, ("unknown NVRAM Field[%d]\n", ePalField));
			return ERR_FAIL;
	}

	return ERR_OK;
}


static HERROR INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DI_ERR_CODE diErr;
	DI_NVRAM_FIELD_T eDiField;

	if(local_nvram_ConvertPalToDi(field, &eDiField) != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: (%d)\n", __FUNCTION__, __LINE__, eDiField));
		return ERR_FAIL;
	}

	diErr = DI_NVRAM_SetField(eDiField, offset, data, nDataSize);

	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: (%d)\n", __FUNCTION__, __LINE__, eDiField));
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DI_ERR_CODE diErr;
	DI_NVRAM_FIELD_T eDiField;

	if(local_nvram_ConvertPalToDi(field, &eDiField) != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: (%d)\n", __FUNCTION__, __LINE__, eDiField));
		return ERR_FAIL;
	}

	diErr = DI_NVRAM_GetField(eDiField, offset, data, nDataSize);

	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: (%d)\n", __FUNCTION__, __LINE__, eDiField));
		return ERR_FAIL;
	}

	return ERR_OK;
}


/* *************************************************************************************************** */
#define ___CAS_IR_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

static HERROR local_cas_ir_GetOffsetCrcAreaByBlockNumber(HUINT32 ulBlkNum, HUINT32 *pulOffset)
{
	HUINT32 ulOffset = 0;

#if defined(CONFIG_CAS_IR) && defined(CONFIG_ROOTFS_ON_RAM)
	if(ulBlkNum > (IR_FLASH_CRC_COUNT+IR_FLASH_NON_CRC_2_COUNT) )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Too Big Block Number[%d]\n", __FUNCTION__, ulBlkNum));
		return ERR_FAIL;
	}
#else
	if(ulBlkNum > (IR_FLASH_CRC_COUNT+IR_FLASH_NON_CRC_1_COUNT+IR_FLASH_NON_CRC_2_COUNT+IR_FLASH_NON_CRC_3_COUNT) )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Too Big Block Number[%d]\n", __FUNCTION__, ulBlkNum));
		return ERR_FAIL;
	}
#endif

#if defined(IR_KERNEL_PARTITION_DUMMY)
	if(ulBlkNum >= (IR_ROOTFS_PARTITION_SIZE/IR_FLASH_APPL_BLOCK_SIZE) && ulBlkNum <= (IR_FLASH_CRC_SIZE/IR_FLASH_APPL_BLOCK_SIZE))
	{
		ulOffset = ulBlkNum*IR_FLASH_APPL_BLOCK_SIZE + IR_ROOTFS_PARTITION_DUMMY;
	}
	else
#endif
	{
		ulOffset = ulBlkNum*IR_FLASH_APPL_BLOCK_SIZE;
	}

#if defined(CONFIG_PROD_VTCHD06)
	// NAND 128M MAP
	if(ulBlkNum < IR_FLASH_CRC_COUNT /* rootfs + kernel */ )
	{
		ulOffset += IR_FLASH_CRC_START_ADDR; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else if(ulBlkNum < (IR_FLASH_CRC_COUNT+IR_FLASH_NON_CRC_1_COUNT) /* rootfs + kernel + DB Backup */ )
	{
		ulOffset = IR_FLASH_NON_CRC_1_START_ADDR /*0x05600000 */ + ((ulBlkNum-IR_FLASH_CRC_COUNT)*IR_FLASH_APPL_BLOCK_SIZE);
	}
	else //if(ulBlkNum >= (74+8) /* rootfs + kernel + DB Backup */ )
	{
		ulOffset = IR_FLASH_NON_CRC_2_START_ADDR /*0x05B00000 */ + ((ulBlkNum-IR_FLASH_CRC_COUNT-IR_FLASH_NON_CRC_1_COUNT)*IR_FLASH_APPL_BLOCK_SIZE);
	}

	if(ulOffset > (IR_FLASH_NON_CRC_2_START_ADDR + IR_FLASH_NON_CRC_2_SIZE))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Invaild offset[0x%08x]\n", __FUNCTION__, ulOffset));
		return ERR_FAIL;
	}
#elif defined(CONFIG_CAS_IR) && defined(CONFIG_ROOTFS_ON_RAM)
	// NAND 128M MAP
	if(ulBlkNum < IR_FLASH_CRC_COUNT /* rootfs + kernel */ )
	{
		ulOffset += IR_FLASH_CRC_START_ADDR; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else  /* rootfs + kernel + DB  */  //  remove DB_bacup , DB_User both
	{
		ulOffset = IR_FLASH_NON_CRC_2_START_ADDR /*0x0AC0.0000 */ + ((ulBlkNum-IR_FLASH_CRC_COUNT)*IR_FLASH_APPL_BLOCK_SIZE);
	}

	// NON_CRC 1 영역에 포함 하는 주소가 나오면 fail
	if((ulOffset >= (IR_FLASH_NON_CRC_1_START_ADDR )) && (ulOffset <= (IR_FLASH_NON_CRC_1_START_ADDR  + IR_FLASH_NON_CRC_1_SIZE)) )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, [NON_CRC 1] Invaild offset[0x%08x]\n", __FUNCTION__, ulOffset));
		return ERR_FAIL;

	}

	// NON_CRC 2 영역를 넘어 가면 fail
	if(ulOffset > (IR_FLASH_NON_CRC_2_START_ADDR + IR_FLASH_NON_CRC_2_SIZE))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, [NON_CRC 3]  Invaild offset[0x%08x]\n", __FUNCTION__, ulOffset));
		return ERR_FAIL;
	}

#else
	// NAND 128M MAP ver 2 (for 3 non crc area like UD-FOX/HD)
	if(ulBlkNum < IR_FLASH_CRC_COUNT /* rootfs + kernel */ )
	{
		ulOffset += IR_FLASH_CRC_START_ADDR; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else if(ulBlkNum < (IR_FLASH_CRC_COUNT+IR_FLASH_NON_CRC_1_COUNT) /* rootfs + kernel + DB Backup */ )
	{
		ulOffset = IR_FLASH_NON_CRC_1_START_ADDR /*0x05600000 */ + ((ulBlkNum-IR_FLASH_CRC_COUNT)*IR_FLASH_APPL_BLOCK_SIZE);
	}
	else if(ulBlkNum < (IR_FLASH_CRC_COUNT+IR_FLASH_NON_CRC_1_COUNT+IR_FLASH_NON_CRC_2_COUNT) /* rootfs + kernel + DB Backup + DB */ )
	{
		ulOffset = IR_FLASH_NON_CRC_2_START_ADDR /*0x05B00000 */ + ((ulBlkNum-IR_FLASH_CRC_COUNT-IR_FLASH_NON_CRC_1_COUNT)*IR_FLASH_APPL_BLOCK_SIZE);
	}
	else
	{
		ulOffset = IR_FLASH_NON_CRC_3_START_ADDR /*0x06000000 */;
	}

	if(ulOffset > (IR_FLASH_NON_CRC_3_START_ADDR + IR_FLASH_NON_CRC_3_SIZE))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Invaild offset[0x%08x]\n", __FUNCTION__, ulOffset));
		return ERR_FAIL;
	}
#endif
	DI_UART_Print("%s(%d) Blk (%d) Offset (0x08%x)\n",__FUNCTION__, __LINE__, ulBlkNum, ulOffset);

	*pulOffset = ulOffset;

	return ERR_OK;
}


//IRDRV_MEM_Alloc
static void *	INTRN_FDI_CAS_IR_MemAlloc(HUINT32 ulSize)
{
	return INTRN_FDI_CAS_IR_SYSTEM_GetMemory(ulSize);
}

//IRDRV_MEM_Free
static HINT32	INTRN_FDI_CAS_IR_MemFree(void *pBuf)
{
	return INTRN_FDI_CAS_IR_SYSTEM_ReturnMemory(pBuf);
}


//IRDRV_MEM_FLASH_Read
static HERROR	INTRN_FDI_CAS_IR_ReadFlash(HUINT32 ulOffset, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	if(pucBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s, %d] Target is null\n", __FUNCTION__, __LINE__));
		return ERR_FAIL;
	}

	return INTRN_FDI_CAS_IR_Flash_Read(ulOffset, pucBuffer, ulSize);
}



//IRDRV_MEM_MakePartCRC32ByBlkNum
static HERROR INTRN_FDI_CAS_IR_MakePartCRC32ByBlkNum(HUINT32 ulBlkNum, HUINT32 *pulCrcOut, HUINT8 *pucP_Mac)
{
        HUINT32                     ulOffset;
        HUINT8                      *pucBuf = NULL;
        HERROR	                        hErr = ERR_OK;
#if defined(CONFIG_CAS_IR) && defined(CONFIG_ENCRYPT_APPL_IMAGE)
        FDI_CAS_IR_Crypt_t 	    stCryptInfo;
        HUINT8	                        UserKer[16]={0,};
        HUINT32	                ulCrcBlkNum=0;
#if !defined(CONFIG_CAS_IR_LOADER_V32)
        HUINT32	            ulTempCacheAddr	=0;
        HUINT8 	                    *pucTempCacheAddr=NULL;
#endif
#endif
		
#if defined(CONFIG_CAS_IR_LOADER_V32)
        HUINT8 *pOutbuffer=NULL;
#endif
        HUINT32         ulBlockSize = IR_FLASH_APPL_BLOCK_SIZE;

#if DEBUG_DUMP
	HUINT32 ii;
#endif

	if(local_cas_ir_GetOffsetCrcAreaByBlockNumber(ulBlkNum, &ulOffset) != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Invaild Offset\n", __FUNCTION__));
		return ERR_FAIL;
	}

#if defined(SUPPORT_IRDETO_CCB_SKIP_CLEARBLK)
	if(s_bIsClerBlkInit == FALSE)
	{
		VK_MEM_Memset(s_aucClearBlk, 0xFF, IR_FLASH_APPL_BLOCK_SIZE);
		s_bIsClerBlkInit = TRUE;
	}
#endif

	DI_UART_Print("[%s, %d] ulBlkNum = %d, ulOffset = 0x%X\n", __FUNCTION__, __LINE__, ulBlkNum, ulOffset );
#if defined(CONFIG_CAS_IR_LOADER_V32)
	pOutbuffer = FVK_MEM_Alloc(IR_FLASH_APPL_BLOCK_SIZE);
	if(pOutbuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Memory Alloc Error for Loader V3.2 \n", __FUNCTION__, __LINE__));
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_CAS_IR) && defined(CONFIG_ENCRYPT_APPL_IMAGE)
	ulCrcBlkNum = (IR_FLASH_CRC_SIZE/IR_FLASH_APPL_BLOCK_SIZE);
	pucBuf = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(IR_FLASH_APPL_BLOCK_SIZE);
#else
	pucBuf = INTRN_FDI_CAS_IR_MemAlloc(IR_FLASH_APPL_BLOCK_SIZE);
#endif

	if(pucBuf == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, Memory Alloc Error\n", __FUNCTION__));
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

#if defined(IR_KERNEL_PARTITION_DUMMY)
	if (ulBlkNum == (IR_FLASH_CRC_SIZE/IR_FLASH_APPL_BLOCK_SIZE-1))
	{
		ulBlockSize	= (IR_FLASH_APPL_BLOCK_SIZE - IR_KERNEL_PARTITION_DUMMY);
		DI_UART_Print("[%s, %d] ucBlockNum(%d), (%08x)\n", __FUNCTION__, __LINE__, ulBlkNum, ulBlockSize);
	}
	else
#endif
	{
		ulBlockSize	= IR_FLASH_APPL_BLOCK_SIZE;
	}

	if(DI_ERR_OK !=	INTRN_FDI_CAS_IR_ReadFlash(ulOffset, pucBuf, ulBlockSize))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s, %d] FLASH_APPL_BLOCK_SIZE: flash read error occured\n", __FUNCTION__, __LINE__));
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

#if 0 // test check decrypt data
	if (ulBlkNum == 0)
	{
		DI_UART_Print("*** CCB En/De Test : CRC Encrypted flash data uSize (ulSize:0x%08x)/n",IR_FLASH_APPL_BLOCK_SIZE);
		DI_UART_Dump(pucBuf, IR_FLASH_APPL_BLOCK_SIZE, 16);
	}
#endif

#if defined(CONFIG_CAS_IR) && defined(CONFIG_ENCRYPT_APPL_IMAGE)
	if(ulBlkNum < ulCrcBlkNum)
	{
		DI_UART_Print("[%s, %d] ulBlkNum: %d, ulCrcBlkNum: %d\n", __FUNCTION__, __LINE__, ulBlkNum, ulCrcBlkNum);
		FVK_MEM_Set(&stCryptInfo, 0, sizeof(FDI_CAS_IR_Crypt_t));
		INTRN_FDI_CAS_IR_CRYPT_GetUserKey(FDI_CAS_IR_CRYPT_KEY_FOR_APPL_IMG, UserKer);

		stCryptInfo.etCryptMode = FDI_CAS_IR_CRYPT_AES;
		stCryptInfo.etCryptKey	= FDI_CAS_IR_CRYPT_USER_KEY;
		stCryptInfo.pKey		= UserKer;
		stCryptInfo.ulKeySize	= 16;
		stCryptInfo.ulBuffSize	= IR_FLASH_APPL_BLOCK_SIZE;

		stCryptInfo.pSrcBuffer = pucBuf;
		if(stCryptInfo.pSrcBuffer == NULL)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] INTRN_FDI_CAS_IR_SYSTEM_GetMemory \n", __FUNCTION__));
			goto END_FUNC;

		}

		stCryptInfo.pDstBuffer = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(IR_FLASH_APPL_BLOCK_SIZE);
		if(stCryptInfo.pDstBuffer == NULL)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] pDstBuffer DI_CRYPT_MemAlloc fail\n", __FUNCTION__));
			hErr = ERR_FAIL;
			goto END_FUNC;

		}

#if DEBUG_DUMP
		//if(ulBlkNum < 10)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Dump Encrypt Appl Block(%d)\n", ulBlkNum));
						
			for(ii=0;ii<(16);ii++) //각 block 의 앞 16byte 만 dump 하여 확인.
			{
				//if(!(ii%16))
				//	FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
							
				FDI_PRINT(FDI_PRT_ERROR, ("0x%02x ", stCryptInfo.pSrcBuffer[ii]));
			}
			FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
		}
#endif

		hErr = INTRN_FDI_CAS_IR_CRYPT_Decrypt(&stCryptInfo);
		if(hErr != ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] error INTRN_FDI_CAS_IR_CRYPT_Decrypt \n", __FUNCTION__));
			hErr = ERR_FAIL;
			goto END_FUNC;
		}

#if DEBUG_DUMP
		//if(ulBlkNum < 10)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("Dump Decrypt Appl Block(%d)\n", ulBlkNum));
						
			for(ii=0;ii<(16);ii++) //각 block 의 앞 16byte 만 dump 하여 확인.
			{
				//if(!(ii%16))
				//	FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
							
				FDI_PRINT(FDI_PRT_ERROR, ("0x%02x ", stCryptInfo.pDstBuffer[ii]));
			}
			FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
		}
#endif		

#if 0 // test check decrypt data
		ulEndTick = VK_TIMER_GetSystemTick();
		ultotalTick += ulEndTick-ulStartTick;
		if (ulBlkNum == 73 ) PrintError("Decrypt Time of Block(%d) : %d \n",ulBlkNum,ultotalTick);

		if (ulBlkNum == 73 )
		{
			DI_UART_Print("*** CCB En/De Test : CRC Decrypted data uSize (ulSize:0x%08x)/n",IR_FLASH_APPL_BLOCK_SIZE);
			DI_UART_Dump(stCryptInfo.pDstBuffer, IR_FLASH_APPL_BLOCK_SIZE, 16);
		}
#endif
	}
#endif

#if defined(SUPPORT_IRDETO_CCB_SKIP_CLEARBLK)
	if(0xFFFFFFFF == HxUtil_Get32Bits(pucBuf))
	{
		if( 0 == VK_memcpy(s_aucClearBlk, pucBuf, IR_FLASH_APPL_BLOCK_SIZE))
		{
			*pulCrcOut = 0x154803CC;
			hErr = ERR_OK;
			goto END_FUNC;
		}
	}
#endif

#if defined(CONFIG_CAS_IR) && defined(CONFIG_ENCRYPT_APPL_IMAGE)

#if defined(CONFIG_CAS_IR_LOADER_V32)
	if(ulBlkNum < ulCrcBlkNum)
	{
		INTRN_FDI_CAS_IR_GetMACbuf(stCryptInfo.pDstBuffer, IR_FLASH_APPL_BLOCK_SIZE, pOutbuffer);
	}
	else
	{
		INTRN_FDI_CAS_IR_GetMACbuf(pucBuf, IR_FLASH_APPL_BLOCK_SIZE, pOutbuffer);
	}
	VK_MEM_Memcpy((void*)pucP_Mac,&pOutbuffer[IR_FLASH_APPL_BLOCK_SIZE-16],16);
#else

	if(ulBlkNum < ulCrcBlkNum)
	{
		/*  cache address 변환 해서 crc32  계산.  */
		ulTempCacheAddr = (HUINT32)stCryptInfo.pDstBuffer ;
		ulTempCacheAddr = ulTempCacheAddr - 0x20000000;
		pucTempCacheAddr = (HUINT8 *)ulTempCacheAddr;
		FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] pDstBuffer:pucTempCacheAddr(0x%08x:0x%08x) \n", __FUNCTION__,stCryptInfo.pDstBuffer,pucTempCacheAddr));
		*pulCrcOut = HxUtil_GetCrc32Fast(pucTempCacheAddr, IR_FLASH_APPL_BLOCK_SIZE);
	}
	else
	{
		*pulCrcOut = HxUtil_GetCrc32Fast(pucBuf, IR_FLASH_APPL_BLOCK_SIZE);
	}
#endif

#else

#if defined(CONFIG_CAS_IR_LOADER_V32)
            INTRN_FDI_CAS_IR_GetMACbuf(pucBuf, IR_FLASH_APPL_BLOCK_SIZE, pOutbuffer);
            VK_MEM_Memcpy((void*)pucP_Mac,&pOutbuffer[IR_FLASH_APPL_BLOCK_SIZE-16],16);
#else

	*pulCrcOut = HxUtil_GetCrc32Fast(pucBuf, IR_FLASH_APPL_BLOCK_SIZE);
#endif

#endif

END_FUNC:

#if defined(CONFIG_CAS_IR) && defined(CONFIG_ENCRYPT_APPL_IMAGE)
	if(ulBlkNum < ulCrcBlkNum)
	{
		if(stCryptInfo.pDstBuffer!=NULL)
		{
			INTRN_FDI_CAS_IR_CRYPT_MemFree(stCryptInfo.pDstBuffer);
		}

		if(stCryptInfo.pSrcBuffer!=NULL)
		{
			INTRN_FDI_CAS_IR_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
		}
	}
	else
	{
		if(pucBuf != NULL)
		{
			INTRN_FDI_CAS_IR_CRYPT_MemFree(pucBuf);
		}
	}
#else
	if(pucBuf != NULL)
	{
		INTRN_FDI_CAS_IR_MemFree(pucBuf);
	}
#endif

#if defined(CONFIG_CAS_IR_LOADER_V32)
        if(pOutbuffer != NULL)
        {
         	FVK_MEM_Free(pOutbuffer);
        }
#endif

	return hErr;

}


//IRDL_CPCB_PREPARE_FROM_DH
static void INTRN_FDI_CAS_IR_PrepareCpcbFromDnHeader(HUINT8 *pucCpcbBuffer, HUINT32 ulSize, DLH_STRUCT *pstDnHeader)
{
	//HUINT32 partition_length;
	HUINT32 crc_32;
	HUINT32 offcrc;
	HUINT8	ucCrcPtCount, ucPt;
	HUINT8  ucP_Mac[16]={0,};

	VK_MEM_Memset(pucCpcbBuffer , 0xff , ulSize); //Added by jino, 2005/09/13

	pucCpcbBuffer[CCB_Load_Sequence_Num] = pstDnHeader->bLoad_sequence_number;

	//mininam 2004-12-23
	//partition count contains the total number of partitions in use bythe high-level code.
	//it is different with the partition count filed in the download_header.
	pucCpcbBuffer[CCB_Partition_Count] = (HUINT8)IR_FLASH_TOTAL_PARTITION_COUNT;

	pucCpcbBuffer[CCB_Bin2Ird_Version] = (HUINT8)((pstDnHeader->uBin2ird_version >>24)&0xFF);
	pucCpcbBuffer[CCB_Bin2Ird_Version+1] = (HUINT8)((pstDnHeader->uBin2ird_version >>16) &0xFF);
	pucCpcbBuffer[CCB_Bin2Ird_Version+2] = (HUINT8)((pstDnHeader->uBin2ird_version >>8) & 0xFF);
	pucCpcbBuffer[CCB_Bin2Ird_Version+3] = (HUINT8)((pstDnHeader->uBin2ird_version) &0xFF);

	pucCpcbBuffer[CCB_Bin2Ird_Serial_Num] = (HUINT8)((pstDnHeader->uBin2ird_serial_number >>24)&0xFF);
	pucCpcbBuffer[CCB_Bin2Ird_Serial_Num+1] = (HUINT8)((pstDnHeader->uBin2ird_serial_number >>16) &0xFF);
	pucCpcbBuffer[CCB_Bin2Ird_Serial_Num+2] = (HUINT8)((pstDnHeader->uBin2ird_serial_number >>8) & 0xFF);
	pucCpcbBuffer[CCB_Bin2Ird_Serial_Num+3] =(HUINT8) ((pstDnHeader->uBin2ird_serial_number) &0xFF);

#if 0
	pucCpcbBuffer[CCB_Sign_Time_Date] = (HUINT8)((_sign_MJD >>8) & 0xFF);
	pucCpcbBuffer[CCB_Sign_Time_Date+1] = (HUINT8)((_sign_MJD) &0xFF);

	pucCpcbBuffer[CCB_Sign_Time_Date+2] = _sign_hour;
	pucCpcbBuffer[CCB_Sign_Time_Date+3] = _sign_min;
	pucCpcbBuffer[CCB_Sign_Time_Date+4] = _sign_sec;
#else
	VK_MEM_Memcpy(&pucCpcbBuffer[CCB_Sign_Time_Date], pstDnHeader->abSign_time_date, 5);
#endif

	/* the total number of CRC partitions inside the user flash. */
#if defined(IR_FLASH_CRC)
	ucCrcPtCount = IR_FLASH_CRC_COUNT;
#else
	ucCrcPtCount = (HUINT8)(IR_FLASH_TOTAL_PARTITION_COUNT - pstDnHeader->bNon_crc_partitioncount);
#endif
	for( ucPt=0; ucPt < IR_FLASH_TOTAL_PARTITION_COUNT; ucPt++ )
	{
		offcrc = (ucPt * CCB_Part_One_Crc_Info_Length) + CCB_Part_Crc_Info_Start;
		if(ucPt < ucCrcPtCount) /* CRC pts */
		{

			INTRN_FDI_CAS_IR_MakePartCRC32ByBlkNum(ucPt, &crc_32, ucP_Mac);
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_NonCrcMask] = 0;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_PartitionLength] 	= 0;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_PartitionLength+1] = 0;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_PartitionLength+2] = 0;

#if defined(CONFIG_CAS_IR_LOADER_V32)
			VK_MEM_Memcpy(&pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32],ucP_Mac,16);
			DI_UART_Print("[%s, %d], ucPt : %d\n", __FUNCTION__, __LINE__, ucPt);
			DI_UART_Dump(ucP_Mac, 16, 16);
#else
			FDI_PRINT(FDI_PRT_DBGINFO, ("IRDL_CPCB_PREPARE_FROM_DH (part idx=%d) P_crc32=0x%x\n", ucPt, crc_32));
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32] 	  = (HUINT8)((crc_32 >>24)&0xFF);
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32+1]  = (HUINT8)((crc_32 >>16)&0xFF);
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32+2]  = (HUINT8)((crc_32 >>8)  &0xFF);
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32+3]  = (HUINT8)(crc_32 &0xFF);
#endif

		}
		else /* Non-CRC pts */
		{
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_NonCrcMask]  = 0xA5;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_PartitionLength] 	= 0;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_PartitionLength+1]	= 0;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_PartitionLength+2]	= 0;

#if defined(CONFIG_CAS_IR_LOADER_V32)
			VK_MEM_Memset(&pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32],0xA5,16);
			DI_UART_Print("[%s, %d], ucPt : %d\n", __FUNCTION__, __LINE__, ucPt);
			DI_UART_Dump(ucP_Mac, 16, 16);
#else
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32] 	  = 0xA5;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32+1]  = 0xA5;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32+2]  = 0xA5;
			pucCpcbBuffer[offcrc + CCB_Part_Crc_Info_P_CRC_32+3]  = 0xA5;
#endif
		}
	}
}

//IRDRV_OSD_DrawTime
static HUINT32 INTRN_FDI_CAS_IR_GetElapsedTimeInSeconds(HBOOL bReset)
{
	static	HUINT32 	s_ulStartTime;
	HUINT32 	ulCursec;
	HUINT32 	ulSec;

	ulCursec = (HUINT32)VK_TIMER_GetSystemTick();
	FDI_PRINT(FDI_PRT_DBGINFO, ("%s, bReset = %d, ulCursec = %d\n", __FUNCTION__ , bReset , ulCursec));
	if (bReset)
	{
		s_ulStartTime = ulCursec;
	}

	ulSec = (ulCursec - s_ulStartTime) ;

	return (ulSec/1000);
}

//IRDRV_MEM_EraseCPCB
static HERROR	INTRN_FDI_CAS_IR_EraseCPCB(void)
{
	HERROR	hErr = ERR_OK;
	HUINT8	*pTemp = NULL;

	pTemp = (HUINT8 *)INTRN_FDI_CAS_IR_MemAlloc(IR_CPCB_SIZE);
	if(pTemp == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, mem alloc failed\n", __FUNCTION__));
		return ERR_FAIL;
	}

	VK_MEM_Memset(pTemp, 0xFF, IR_CPCB_SIZE);

#if defined(CONFIG_CAS_IR_CPCB_CRYPT)
	hErr = INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED, 0, pTemp, IR_CPCB_SIZE);
#else
	hErr = INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB, 0, pTemp, IR_CPCB_SIZE);
#endif
	INTRN_FDI_CAS_IR_MemFree(pTemp);

	return hErr;
}

//IRDRV_MEM_WriteCPCB
static HERROR	INTRN_FDI_CAS_IR_WriteCPCB(HUINT8 *pucData, HUINT16 usOffset, HUINT32 ulLength)
{
#if defined(CONFIG_CAS_IR_CPCB_CRYPT)
	return INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED, usOffset, pucData, ulLength);
#else
	return INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB, usOffset, pucData, ulLength);
#endif
}

//IRDRV_MEM_ReadCPCB
static HERROR	INTRN_FDI_CAS_IR_ReadCPCB(HUINT8 *pucData, HUINT16 usOffset, HUINT32 ulLength)
{
#if defined(CONFIG_CAS_IR_CPCB_CRYPT)
	return INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED, usOffset, pucData, ulLength);
#else
	return INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB, usOffset, pucData, ulLength);
#endif
}

static HERROR	INTRN_FDI_CAS_IR_Read_pureCPCB(HUINT8 *pucData, HUINT16 usOffset, HUINT32 ulLength)
{
	return INTRN_FDI_CAS_IR_NVRAM_GetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB, usOffset, pucData, ulLength);
}

#if defined(CONFIG_CAS_IR_LOADER_V32)
static HERROR  INTRN_FDI_CAS_IR_GetMACbuf(HUINT8 *pbBlock, HUINT32 uLen, HUINT8 *pOutbuffer)
{
	HERROR			hErr = ERR_OK;
	FDI_CAS_IR_Crypt_t 	stCryptInfo;

	FVK_MEM_Set(&stCryptInfo, 0, sizeof(FDI_CAS_IR_Crypt_t));
	stCryptInfo.etCryptMode = FDI_CAS_IR_CRYPT_AES_CBC_MAC;
	stCryptInfo.etCryptKey 	= FDI_CAS_IR_CRYPT_DEFAULT_SYSTEM_KEY;
	stCryptInfo.pKey 		= NULL;
	stCryptInfo.ulKeySize 	= 16;
	stCryptInfo.ulBuffSize 	= uLen;
//  stCryptInfo.etCryptData = FDI_CAS_IR_CRYPT_BLOCK;
	stCryptInfo.pSrcBuffer = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(uLen);

	if(stCryptInfo.pSrcBuffer == NULL)
	{
		hErr = ERR_FAIL;
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] pSrcBuffer  error \n", __FUNCTION__));
		goto END_FUNC;
	}

	VK_MEM_Memcpy((void *)stCryptInfo.pSrcBuffer, pbBlock ,uLen);

	stCryptInfo.pDstBuffer = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(uLen);
	if(stCryptInfo.pDstBuffer == NULL)
	{
		hErr = ERR_FAIL;
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] pDstBuffer error \n", __FUNCTION__));
		goto END_FUNC;
	}

	hErr = INTRN_FDI_CAS_IR_CRYPT_Encrypt(&stCryptInfo);
	if(hErr != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] error INTRN_FDI_CAS_IR_CRYPT_Encrypt = 0x%x\n",__FUNCTION__ , hErr));
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

//    PrintError("\n %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n", stCryptInfo.pDstBuffer[uLen-16],stCryptInfo.pDstBuffer[uLen-16+1],stCryptInfo.pDstBuffer[uLen-16+2],stCryptInfo.pDstBuffer[uLen-16+3],stCryptInfo.pDstBuffer[uLen-16+4],stCryptInfo.pDstBuffer[uLen-16+5],stCryptInfo.pDstBuffer[uLen-16+6],stCryptInfo.pDstBuffer[uLen-16+7],
//                                                                                   stCryptInfo.pDstBuffer[uLen-16+8],stCryptInfo.pDstBuffer[uLen-16+9],stCryptInfo.pDstBuffer[uLen-16+10],stCryptInfo.pDstBuffer[uLen-16+11],stCryptInfo.pDstBuffer[uLen-16+12],stCryptInfo.pDstBuffer[uLen-16+13],stCryptInfo.pDstBuffer[uLen-16+14],stCryptInfo.pDstBuffer[uLen-16+15]);

	if(pOutbuffer != NULL)
	{
		VK_MEM_Memcpy((void *)pOutbuffer,stCryptInfo.pDstBuffer,uLen);
		FDI_PRINT(FDI_PRT_DBGINFO, ("\n %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x", pOutbuffer[uLen-16],pOutbuffer[uLen-16+1],pOutbuffer[uLen-16+2],pOutbuffer[uLen-16+3],pOutbuffer[uLen-16+4],pOutbuffer[uLen-16+5],pOutbuffer[uLen-16+6],pOutbuffer[uLen-16+7],
                                                                                                         pOutbuffer[uLen-16+8],pOutbuffer[uLen-16+9],pOutbuffer[uLen-16+10],pOutbuffer[uLen-16+11],pOutbuffer[uLen-16+12],pOutbuffer[uLen-16+13],pOutbuffer[uLen-16+14],pOutbuffer[uLen-16+15]));
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] MAC error \n",__FUNCTION__ ));
	}

END_FUNC:

	if(stCryptInfo.pSrcBuffer != NULL)
	{
		INTRN_FDI_CAS_IR_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
	}

	if(stCryptInfo.pDstBuffer != NULL)
	{
		INTRN_FDI_CAS_IR_CRYPT_MemFree(stCryptInfo.pDstBuffer);
	}

	return hErr;
}

/* ======= Facory mode 에서 사용할 APIs ========= */
static void INTRN_FDI_CAS_IR_UpdatePureCpcb(HUINT8 *pucBaseBuf, HUINT32 ulSize)
{
	HUINT8	*pTemp = NULL;
    	HUINT8 pP_Mac[17]={0,};
    	HUINT8 *pOutbuffer = NULL; // test

	pTemp = (HUINT8 *)INTRN_FDI_CAS_IR_MemAlloc(IR_CPCB_SIZE);
	if(pTemp == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, mem alloc failed\n", __FUNCTION__));
		return;
	}
	VK_MEM_Memset(pTemp, 0xFF, IR_CPCB_SIZE);
	FVK_MEM_Copy(pTemp, pucBaseBuf, ulSize);

    	pOutbuffer = FVK_MEM_Alloc(ulSize - 16);
    	if(pOutbuffer == NULL)
    	{
    		FDI_PRINT(FDI_PRT_ERROR, ("%s, MAC mem alloc failed\n", __FUNCTION__));
		return;
    	}

	INTRN_FDI_CAS_IR_GetMACbuf((HUINT8 *)pucBaseBuf, ulSize - 16, pOutbuffer);

    	VK_MEM_Memcpy((void*)pP_Mac,&pOutbuffer[ulSize-16-16],16);

    	if(pOutbuffer != NULL)
    	{
        	FVK_MEM_Free(pOutbuffer);
    	}

    	VK_MEM_Memcpy(&pTemp[CCB_Crc32],pP_Mac,16);

	INTRN_FDI_CAS_IR_NVRAM_SetField(FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB, 0, pTemp, IR_CPCB_SIZE);
    	INTRN_FDI_CAS_IR_MemFree(pTemp);
}


/* flash SPCB Write 한다.*/
static HERROR  INTRN_FDI_CAS_IR_UpdateSPCB(HUINT8 *pucBaseBuf, HUINT32 ulSize)
{
	HUINT32			usDnloadTimeStamp;
	HERROR			hErr;
	FDI_CAS_IR_Crypt_t		stCryptInfo;
	HUINT8 pP_Mac[17]={0,};
    	HUINT8 *pOutbuffer = NULL; // test

#if DEBUG_DUMP
	HUINT32 ii;
#endif

	/* Download Header를 받은 시간부터 Flash write가 완료된 때까지 소요된 시간*/
	usDnloadTimeStamp = INTRN_FDI_CAS_IR_GetElapsedTimeInSeconds(FALSE);
	FDI_PRINT(FDI_PRT_DBGINFO, ("duration : 0x%x\n", usDnloadTimeStamp));
	pucBaseBuf[CCB_Download_Duration] 	= (HUINT8)((usDnloadTimeStamp >> 8) & 0xFF);
	pucBaseBuf[CCB_Download_Duration+1] = (HUINT8)(usDnloadTimeStamp & 0xFF);

/* For SECURED_LOADER
The start position is pucBaseBuf + CCB_Part_Crc_Info_Start */
// no ccb encryption 시 하기 define을 0으로 할것.

#if defined(CONFIG_DEBUG)
	INTRN_FDI_CAS_IR_UpdatePureCpcb(pucBaseBuf, ulSize);
#endif

	VK_MEM_Memset(&stCryptInfo, 0, sizeof(FDI_CAS_IR_Crypt_t));

	stCryptInfo.pSrcBuffer = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(CCB_Crc32 - CCB_Part_Crc_Info_Start);
	if(stCryptInfo.pSrcBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: Factory SPCB pSrcBuffer alloc \n", __FUNCTION__, __LINE__));
	}

	stCryptInfo.pDstBuffer = INTRN_FDI_CAS_IR_CRYPT_MemAlloc(CCB_Crc32 - CCB_Part_Crc_Info_Start);
	if(stCryptInfo.pDstBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: Factory SPCB pDstBuffer alloc \n", __FUNCTION__, __LINE__));
	}

	stCryptInfo.etCryptMode = FDI_CAS_IR_CRYPT_TDES;
	stCryptInfo.etCryptKey = FDI_CAS_IR_CRYPT_DEFAULT_SYSTEM_KEY;
	stCryptInfo.etCryptData = FDI_CAS_IR_CRYPT_BLOCK;
	stCryptInfo.ulBuffSize = CCB_Crc32 - CCB_Part_Crc_Info_Start;
	stCryptInfo.pKey 		= NULL;

	VK_MEM_Memcpy(stCryptInfo.pSrcBuffer, (void *)(pucBaseBuf + CCB_Part_Crc_Info_Start), CCB_Crc32 - CCB_Part_Crc_Info_Start);

#if DEBUG_DUMP
	FDI_PRINT(FDI_PRT_ERROR, ("Dump CRC(clean)\n"));
			
	for(ii=0;ii<(CCB_Crc32 - CCB_Part_Crc_Info_Start);ii++)
	{
		if(!(ii%16))
			FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
				
		FDI_PRINT(FDI_PRT_ERROR, ("0x%02x ", stCryptInfo.pSrcBuffer[ii]));
	}
	FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
#endif

	hErr = INTRN_FDI_CAS_IR_CRYPT_Encrypt(&stCryptInfo);

#if DEBUG_DUMP
	FDI_PRINT(FDI_PRT_ERROR, ("Dump CRC(encrypt)\n"));
				
	for(ii=0;ii<(CCB_Crc32 - CCB_Part_Crc_Info_Start);ii++)
	{
		if(!(ii%16))
			FDI_PRINT(FDI_PRT_ERROR, ("\n"));	
					
		FDI_PRINT(FDI_PRT_ERROR, ("0x%02x ", stCryptInfo.pDstBuffer[ii]));
	}
	FDI_PRINT(FDI_PRT_ERROR, ("\n"));
#endif

	if(hErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: Factory SPCB Encrypt \n", __FUNCTION__, __LINE__));
	}

	VK_MEM_Memcpy((void *)(pucBaseBuf + CCB_Part_Crc_Info_Start), stCryptInfo.pDstBuffer, CCB_Crc32-CCB_Part_Crc_Info_Start);

	INTRN_FDI_CAS_IR_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
	INTRN_FDI_CAS_IR_CRYPT_MemFree(stCryptInfo.pDstBuffer);

     	pOutbuffer = FVK_MEM_Alloc(ulSize - 16);
     	if(pOutbuffer == NULL)
     	{
     		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: SPCB alloc \n", __FUNCTION__, __LINE__));
     	}

     	INTRN_FDI_CAS_IR_GetMACbuf((HUINT8 *)pucBaseBuf, ulSize - 16, pOutbuffer);

     	VK_MEM_Memcpy((void*)pP_Mac,&pOutbuffer[ulSize-16-16],16);
    	if(pOutbuffer != NULL)
     	{
         	FVK_MEM_Free(pOutbuffer);
     	}

     	VK_MEM_Memcpy(&pucBaseBuf[CCB_Crc32], pP_Mac, 16);

	/* DO ERASE WITHOUT COMPARING WITH PREVIOUS DATA*/
	if(INTRN_FDI_CAS_IR_EraseCPCB())
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: Factory SPCB erase \n", __FUNCTION__, __LINE__));
		return ERR_FAIL;
	}

	if(ERR_OK != INTRN_FDI_CAS_IR_WriteCPCB(pucBaseBuf, 0, ulSize))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s:%d] Error: Factory SPCB Write \n", __FUNCTION__, __LINE__));
		return ERR_FAIL;
	}

	return ERR_OK;
}	/* CCBStoreInFlash */


static HERROR INTRN_FDI_CAS_IR_MakeSPCB(void)
{
	HERROR			 hErr = ERR_OK;
   	DLH_STRUCT		pstDnHeader;
	HUINT8	        *pucCpcbData=NULL;
	HUINT8	        *pucCpcbHeaderData=NULL;

	HUINT32         _dnld_MJD=51179, _dnld_hour=0, _dnld_min=0 ,_dnld_sec = 0; // 1999.01.01
	HUINT32         _sign_MJD=51179, _sign_hour=0, _sign_min=0 ,_sign_sec = 0; // 1999.01.01

	pucCpcbData = (HUINT8 *)FVK_MEM_Alloc(IR_CPCB_SIZE);
	if(pucCpcbData == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, mem alloc failed\n", __FUNCTION__));
		hErr = ERR_OK ;
		goto END_FUNC;
	}
	
	pucCpcbHeaderData = (HUINT8 *)FVK_MEM_Alloc(CCB_Part_Crc_Info_Start + 1);
	if(pucCpcbHeaderData == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("%s, mem alloc failed\n", __FUNCTION__));
		hErr = ERR_OK ;
		goto END_FUNC;
	}

	
	VK_MEM_Memset((void *)&pstDnHeader, 0xff , sizeof(DLH_STRUCT));
#if 1
	FDI_PRINT(FDI_PRT_DBGINFO, (" _dnld_MJD = 0x%X, _dnld_hour= 0x%X, _dnld_min= 0x%X, _dnld_sec= 0x%X\n",	_dnld_MJD, _dnld_hour, _dnld_min, _dnld_sec));

	if(INTRN_FDI_CAS_IR_Read_pureCPCB(pucCpcbHeaderData, 0, CCB_Part_Crc_Info_Start) != ERR_OK)
	{
		hErr = ERR_OK ;
		goto END_FUNC;
	}

	_dnld_MJD = pucCpcbHeaderData[CCB_Download_Time_Date] <<8 | pucCpcbHeaderData[CCB_Download_Time_Date + 1];
	_dnld_hour = pucCpcbHeaderData[CCB_Download_Time_Date + 2];
	_dnld_min = pucCpcbHeaderData[CCB_Download_Time_Date + 3];
	_dnld_sec = pucCpcbHeaderData[CCB_Download_Time_Date + 4];

	_sign_MJD = pucCpcbHeaderData[CCB_Sign_Time_Date] <<8 | pucCpcbHeaderData[CCB_Sign_Time_Date + 1];;
	_sign_hour = pucCpcbHeaderData[CCB_Sign_Time_Date + 2];
	_sign_min = pucCpcbHeaderData[CCB_Sign_Time_Date + 3];
	_sign_sec = pucCpcbHeaderData[CCB_Sign_Time_Date + 4];
	
#endif

	/* next download ¸| ?§C? default °?.*/
	pstDnHeader.bLoad_sequence_number   = pucCpcbHeaderData[CCB_Load_Sequence_Num];
	pstDnHeader.uBin2ird_version        = pucCpcbHeaderData[CCB_Bin2Ird_Version]<<24 | pucCpcbHeaderData[CCB_Bin2Ird_Version+1] <<16 | pucCpcbHeaderData[CCB_Bin2Ird_Version+2] << 8| pucCpcbHeaderData[CCB_Bin2Ird_Version+3];
	pstDnHeader.uBin2ird_serial_number  = pucCpcbHeaderData[CCB_Bin2Ird_Serial_Num]<<24 | pucCpcbHeaderData[CCB_Bin2Ird_Serial_Num+1] <<16 | pucCpcbHeaderData[CCB_Bin2Ird_Serial_Num+2] << 8 | pucCpcbHeaderData[CCB_Bin2Ird_Serial_Num+3];
	pstDnHeader.bNon_crc_partitioncount = 8;

	INTRN_FDI_CAS_IR_PrepareCpcbFromDnHeader(pucCpcbData, IR_CPCB_SIZE, &pstDnHeader);

	pucCpcbData[CCB_Download_Time_Date]   =  (HUINT8)((_dnld_MJD>>8) &0xFF);
	pucCpcbData[CCB_Download_Time_Date+1] =  (HUINT8)(_dnld_MJD&0xFF);
	pucCpcbData[CCB_Download_Time_Date+2] = _dnld_hour;
	pucCpcbData[CCB_Download_Time_Date+3] = _dnld_min;
	pucCpcbData[CCB_Download_Time_Date+4] = _dnld_sec;

	pucCpcbData[CCB_Sign_Time_Date]   =  (HUINT8)((_sign_MJD>>8) &0xFF);
	pucCpcbData[CCB_Sign_Time_Date+1] =  (HUINT8)(_sign_MJD&0xFF);
	pucCpcbData[CCB_Sign_Time_Date+2] = _sign_hour;
	pucCpcbData[CCB_Sign_Time_Date+3] = _sign_min;
	pucCpcbData[CCB_Sign_Time_Date+4] = _sign_sec;


	hErr = INTRN_FDI_CAS_IR_UpdateSPCB(pucCpcbData,IR_CPCB_SIZE);

END_FUNC:
	if(pucCpcbData != NULL)
	{
		FVK_MEM_Free(pucCpcbData);
	}

	if(pucCpcbHeaderData != NULL)
	{
		FVK_MEM_Free(pucCpcbHeaderData);
	}

	return hErr;
}

static HERROR INTRN_FDI_CAS_IR_PrintSPCBHeader(void)
{
	HERROR			 hErr = ERR_OK;
	HUINT8	        *pucCpcbData=NULL;

    	pucCpcbData = (HUINT8 *)FVK_MEM_Alloc(IR_CPCB_SIZE);
    	if(pucCpcbData == NULL)
    	{
    		FDI_PRINT(FDI_PRT_ERROR, ("%s, mem alloc failed\n", __FUNCTION__));        	
        	hErr = ERR_OK ;
        	goto END_FUNC;
    	}

	if(INTRN_FDI_CAS_IR_ReadCPCB(pucCpcbData, 0, IR_CPCB_SIZE) == ERR_OK)
	{
/*      pucCpcbData[CCB_Load_Sequence_Num];
        pucCpcbData[CCB_Partition_Count];
        pucCpcbData[CCB_Bin2Ird_Version];
        pucCpcbData[CCB_Bin2Ird_Serial_Num];
        pucCpcbData[CCB_Sign_Time_Date];
        pucCpcbData[CCB_Download_Time_Date];
        pucCpcbData[CCB_Download_Duration];
*/
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Load_Sequence_Num : %d \n", pucCpcbData[CCB_Load_Sequence_Num]));
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Partition_Count   : %d \n", pucCpcbData[CCB_Partition_Count]));
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Bin2Ird_Version   : 0x%02x:%02x:%02x:%02x \n", pucCpcbData[CCB_Bin2Ird_Version], pucCpcbData[CCB_Bin2Ird_Version+1], pucCpcbData[CCB_Bin2Ird_Version+2], pucCpcbData[CCB_Bin2Ird_Version+3]));
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Bin2Ird_Serial_Num: 0x%02x:%02x:%02x:%02x \n", pucCpcbData[CCB_Bin2Ird_Serial_Num], pucCpcbData[CCB_Bin2Ird_Serial_Num+1], pucCpcbData[CCB_Bin2Ird_Serial_Num+2], pucCpcbData[CCB_Bin2Ird_Serial_Num+3]));
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Sign_Time_Date    : %d:%d:%d:%d:%d \n", pucCpcbData[CCB_Sign_Time_Date],pucCpcbData[CCB_Sign_Time_Date+1],pucCpcbData[CCB_Sign_Time_Date+2],pucCpcbData[CCB_Sign_Time_Date+3],pucCpcbData[CCB_Sign_Time_Date+4]));
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Download_Time_Date: %d:%d:%d:%d:%d \n", pucCpcbData[CCB_Download_Time_Date], pucCpcbData[CCB_Download_Time_Date+1], pucCpcbData[CCB_Download_Time_Date+2], pucCpcbData[CCB_Download_Time_Date+3], pucCpcbData[CCB_Download_Time_Date+4]));
		FDI_PRINT(FDI_PRT_ERROR, ("CCB_Download_Duration : %d \n", pucCpcbData[CCB_Download_Duration]));
    	}

END_FUNC:
    	if(pucCpcbData != NULL)
    	{
        	FVK_MEM_Free(pucCpcbData);
    	}

}

#endif

/* *************************************************************************************************** */
#define ___FDI_API_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


/*******************************************************************************
* function : FDI_CAS_IR_MakeSPCB
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CAS_IR_MakeSPCB(void)
{
	HERROR hErr = ERR_OK;

	FDI_PRINT(FDI_PRT_ERROR, ("---> start SPCB \n"));
#if defined(CONFIG_ENCRYPT_APPL_IMAGE) && defined(CONFIG_CAS_IR)
	hErr = INTRN_FDI_CAS_IR_SYSTEM_IR_LoadEncryptionKey();
	if(hErr != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("Warning: INTRN_FDI_CAS_IR_SYSTEM_IR_LoadEncryptionKey() Fail\n"));
		//return FDI_ERR;
	}
#endif

#if defined(CONFIG_CAS_IR_LOADER_V32)
	hErr = INTRN_FDI_CAS_IR_MakeSPCB();
	if(hErr != ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("---> Fail SPCB \n"));
		return FDI_ERR;
	}

	INTRN_FDI_CAS_IR_PrintSPCBHeader();	
#endif

	FDI_PRINT(FDI_PRT_ERROR, ("---> end SPCB \n"));

	return FDI_NO_ERR;
}


