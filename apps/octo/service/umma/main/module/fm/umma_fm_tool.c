/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_tool.c
	@brief

	Description:  									\n
	Module: Umma Toolkit Module						\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/vfs.h>
#include	<unistd.h>
#include	<dirent.h>
#include	<malloc.h>

#include	<vkernel.h>
#include	<hlib.h>

#include	<uapi.h>
#include	<umma_int.h>
#include	<umma_fm.h>
#include	<umma_fs.h>

#if defined(CONFIG_FUNC_USE_DECRYPTCOPY) /* direct M2M decryption thru shared CDI(chip driver interface) */
#include "di_crypt.h"
#include "di_pvr.h"
#include "oapi_sysinfo_stb.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	UMMA_FM_BLOCK_OP_NUM		1024
#define	UMMA_FM_BLOCK_NUM			4
#define	UMMA_FM_BLOCK_SIZE			(getpagesize() * UMMA_FM_BLOCK_NUM)

#define	UMMA_FM_REMOVE_STEP_SIZE	((HINT64)((HINT64)UMMA_FM_BLOCK_SIZE * (HINT64)UMMA_FM_BLOCK_OP_NUM * 5ll))

#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
#define UMMA_FM_BLOCK_DECRYPT_SIZE ((getpagesize() * 3  ) * 12)//12k ??? ?????? ??u ??? ??
#define UMMA_FM_CDI_MAX_BUFFER_SIZE	(1024*1024*2)
#define UMMA_FM_CDI_ALIGN_UNIT		(192)
#define UMMA_FM_CDI_DECRYPT_SIZE	UMMA_FM_BLOCK_DECRYPT_SIZE /* maintain page align size */
#define	UMMA_FM_CDI_KEY_LEN			DI_PVR_CRYPTO_KEY_LEN /* 16 */
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HxCBUF_t		nCBuf;
	HINT32			nCopyBuffSize;
	void			*pvCopyBuffer;

#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
	stCrypt_t	stDecrypt;
#endif
} UMMA_FM_TKINFO_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
STATIC HUINT8 s_aucEncKey[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; /* Change encKey[UMMA_FM_CDI_KEY_LEN] by chipID */
STATIC HUINT8 s_aucEncIv[] = {0xad, 0xd6, 0x9e, 0xa3, 0x89, 0xc8, 0x17, 0x72, 0x1e, 0xd4, 0x0e, 0xab, 0x3d, 0xbc, 0x7a, 0xf2};
#endif

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static HCHAR*	umma_fm_toolkit_GetChunkName(UMMA_OPS_t *pstOps, UMMA_FM_TKINFO_t *pstTkInfo);
static HUINT32	umma_fm_toolkit_CheckSpace(UMMA_FM_TKINFO_t *pstTkInfo, const HCHAR *pszSourceName, const HCHAR *pszDestName, const HUINT64 ullReservedKb);


#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
HERROR umma_fm_toolkit_cryptoRead(HxFILE_t hFile, HUINT8 *buf, HUINT32 readsize, HUINT32 *sizep)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return 0;
#else
	HERROR hError = ERR_OK;
	HINT32 fd = (-1);
	DI_ERR_CODE diError = DI_ERR_OK;
	HUINT8 *pucSecureSrc=NULL;
	HUINT8 *pucSecureDst=NULL;
	stCrypt_t stCrypt;
	HUINT32 ulBufSize = 0, ulHeadSize=0, ulTailSize = 0;
	HUINT32	ulReadSize = 0;
	HINT64 ulCurPosition = 0;
	HINT64 nOffset = 0;
	HOffset_t nCurOffset = 0;

	*sizep = (-1);
	if ((buf == NULL) || (readsize == 0))
	{
		hError = ERR_FAIL;
		goto func_error;
	}
	fd = fileno(hFile);

	if( readsize > UMMA_FM_CDI_MAX_BUFFER_SIZE )
	{
		HxLOG_Error("[cryptoRead] Error> Too large buffer size (%d, max:%d)\n", (int)readsize, (int)UMMA_FM_CDI_MAX_BUFFER_SIZE);
		hError = ERR_FAIL;
		goto func_error;
	}

	// make 192 byte align
	(void)HLIB_FILE_Tell( hFile, &ulCurPosition );
	ulHeadSize = ulCurPosition % UMMA_FM_CDI_ALIGN_UNIT;
	ulTailSize = (ulCurPosition+readsize) % UMMA_FM_CDI_ALIGN_UNIT;
	if( ulTailSize != 0 )
	{
		ulTailSize = (UMMA_FM_CDI_ALIGN_UNIT- ulTailSize);
	}

	ulBufSize = ulHeadSize + readsize + ulTailSize;

	//	HxLOG_Print("[cryptoRead] ulCur:%lld, ulHeadSize:%d, size:%d, ulTailSize:%d, ulBufSize:%d\r\n",
	//			ulCurPosition, ulHeadSize, size, ulTailSize, ulBufSize);

	pucSecureSrc = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureSrc == NULL )
	{
		HxLOG_Error("[cryptoRead] Error> Out of memory(%d)\n", (int)ulBufSize);
		hError = ERR_FAIL;
		goto func_error;
	}

	pucSecureDst = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureDst == NULL )
	{
		HxLOG_Error("[cryptoRead] Error> Out of memory(%d)\n", (int)ulBufSize);
		hError = ERR_FAIL;
		goto func_error;
	}

	if( ulHeadSize != 0 )
	{
		nOffset = ulCurPosition - ulHeadSize;
		hError = HLIB_FILE_Seek(hFile, nOffset, eHxFILE_SEEK_SET, &nCurOffset);
		if( hError != ERR_OK )
		{
			HxLOG_Error("[cryptoRead] Error> seek() failed, err:0x%X, size:%d\n", hError, (int)ulHeadSize);
			hError = ERR_FAIL;
			goto func_error;
		}

		// HxLOG_Print("[cryptoRead] ulHeadSize:%d, nOffset:%lld, nCurOffset:%lld\r\n", ulHeadSize, nOffset, nCurOffset);
	}

	ulReadSize = HLIB_FILE_Read(hFile, pucSecureSrc, 1, ulBufSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("[cryptoRead] Error> read() failed, err:0x%X, bufsize:%d, readsize:%d\n", hError, (int)ulBufSize, (int)ulReadSize);
		hError = ERR_OK; //hError = ERR_FAIL;
		*sizep = ulReadSize; //sizep = (-1);
		goto func_error;
	}

	if( ulTailSize != 0 )
	{
		nOffset= ulCurPosition+readsize;
		hError = HLIB_FILE_Seek(hFile, nOffset, eHxFILE_SEEK_SET, &nCurOffset);
		if( hError != ERR_OK )
		{
			HxLOG_Error("[cryptoRead] Error> seek() failed, err:0x%X, size:%d\n", hError, (int)ulTailSize);
			hError = ERR_FAIL;
			goto func_error;
		}
		// HxLOG_Print("[cryptoRead] ulTailSize:%d, nOffset:%lld, nCurOffset:%lld\r\n",
		//			ulTailSize, nOffset, nCurOffset);
	}

	HxSTD_MemSet( &stCrypt, 0, sizeof(stCrypt_t) );

	stCrypt.etCryptMode = DI_CRYPT_AES;	/* Changed PVR encryption with AES from TDES */
	stCrypt.etCryptKey = DI_CRYPT_USER_KEY;
	stCrypt.etCryptData = DI_CRYPT_STREAM;
	stCrypt.ulKeySize = DI_PVR_CRYPTO_KEY_LEN;
	stCrypt.pSrcBuffer =  pucSecureSrc;
	stCrypt.pDstBuffer = pucSecureDst;
	stCrypt.ulBuffSize = ulBufSize;
	stCrypt.pKey = s_aucEncKey;
	stCrypt.pIvKey = s_aucEncIv;  /* Added IV */

	diError = DI_CRYPT_Decrypt( &stCrypt );
	if( diError != DI_ERR_OK )
	{
		HxLOG_Error("[cryptoRead] Error> DI_CRYPT_Decrypt: 0x%X\n", diError);
		hError = ERR_FAIL;
		goto func_error;
	}

	VK_memcpy32( buf, pucSecureDst+ulHeadSize, readsize );
	if( ulReadSize < readsize )
	{
		*sizep = ulReadSize;
	}
	else
	{
		*sizep = readsize;
	}
	hError = ERR_OK;

func_error:
	if (pucSecureSrc != NULL)
	{
		DI_CRYPT_MemFree( (void *)pucSecureSrc);
		pucSecureSrc = NULL;
	}

	if (pucSecureDst != NULL)
	{
		DI_CRYPT_MemFree( (void *)pucSecureDst);
		pucSecureDst = NULL;
	}
	return hError;
#endif /* CONFIG_PROD_OS_EMULATOR */
}

static HUINT32	umma_fm_toolkit_HexToInt(const HCHAR *s)
{
	HUINT32		val = 0;
	HINT32		x = 0;

	if(s[x] == '0' && (s[x+1]=='x' || s[x+1]=='X')) x+=2;

	while(s[x]!='\0')
	{
		if((s[x] == 'X') || (s[x] == 'x'))
		{
			x++;
			continue;
		}
		else if(s[x] >= '0' && s[x] <='9')
		{
		  val = (val << 4) + s[x] - '0';
		}
		else if(s[x]>='A' && s[x] <='F')
		{
		  val = (val << 4) + s[x] - 'A' + 10;
		}
		else if(s[x]>='a' && s[x] <='f')
		{
		  val = (val << 4) + s[x] - 'a' + 10;
		}
		else return val;

	   x++;
	}
	return val;
}
#endif /* CONFIG_FUNC_USE_DECRYPTCOPY */

static UMMA_FM_TKINFO_t*	umma_fm_toolkit_GetTkInfo(void)
{
	static UMMA_FM_TKINFO_t		*s_pstCopyInfo = NULL;
#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
	HERROR hErr = 0;
	HCHAR	szChipId[OAPI_SYSINFO_NAME_LEN]; /* OAPI_SYSINFO_NAME_LEN */
	HUINT32 uChipId = 0x0;
#endif

	if (s_pstCopyInfo == NULL)
	{
		s_pstCopyInfo = (UMMA_FM_TKINFO_t *)UMMA_Calloc(sizeof(UMMA_FM_TKINFO_t));

		s_pstCopyInfo->nCopyBuffSize = UMMA_FM_BLOCK_SIZE;
		s_pstCopyInfo->pvCopyBuffer  = memalign(s_pstCopyInfo->nCopyBuffSize, s_pstCopyInfo->nCopyBuffSize);
		s_pstCopyInfo->nCBuf         = HLIB_CBUF_New(4096);

#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
		{
			/*
			 * Assumed that the following initializations has already been performed
			 *   - nexus platform join
			 *   - initializing for di_crypto
			 *   - OAPI_SYSINFO_STB_Init();
			 */
			hErr = OAPI_SYSINFO_STB_GetChipId(szChipId, OAPI_SYSINFO_NAME_LEN);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Error> Cant get chipID!\n");
			}

			uChipId = umma_fm_toolkit_HexToInt(szChipId);
			if (uChipId == 0x0)
			{
				HxLOG_Error("Error> invalid chipID(%s)\n", szChipId);
				/* What todo? */
			}

			HxSTD_MemSet (s_aucEncKey, 0, UMMA_FM_CDI_KEY_LEN);
			HxSTD_MemCopy (s_aucEncKey, (char *)&uChipId, 4);

  #if defined(CONFIG_DEBUG)
			{
				int i;
				HxLOG_Info(" Key value: ");
				for( i=0; i<16; i++)
				{
					HxLOG_Info(" %02X", s_aucEncKey[i]);
				}
				HxLOG_Info("\n");
			}
  #endif
		}
#endif /* CONFIG_FUNC_USE_DECRYPTCOPY */
	}
	return s_pstCopyInfo;
}

static void*	umma_fm_toolkit_ReadFile(UMMA_OPS_t *pstOps, UMMA_FM_TKINFO_t *pstTkInfo, HINT32 *pnRead)
{
	HINT32		nRead;
	void		*pvBuffer;
	HBOOL		bDecrypt;

	pvBuffer = pstTkInfo->pvCopyBuffer;
	bDecrypt = pstOps->nFlags & UAPI_COPY_FLAG_DECRYPT? TRUE:FALSE;

read_rtn:
#if defined(CONFIG_FUNC_USE_DECRYPTCOPY)
	if (bDecrypt == TRUE)
	{
		HERROR hErr = ERR_OK;

		hErr = umma_fm_toolkit_cryptoRead(pstOps->stDetail.pstSourceHandle, pvBuffer, pstTkInfo->nCopyBuffSize, &nRead);
		if ((nRead < 0) || (hErr != ERR_OK))
		{
			HxLOG_Error("nRead:%d, hErr:%d\n", nRead, hErr);
			if (EINTR == errno)
				goto read_rtn;
			else
				return NULL;
		}
	}
	else
#endif
	{
		/* Process here including no encrypted record (HMS-1000T, ...) */
		nRead = HLIB_FILE_Read(pstOps->stDetail.pstSourceHandle, pvBuffer, 1, pstTkInfo->nCopyBuffSize);
		if (nRead < 0)
		{
			if (EINTR == errno)
				goto read_rtn;
			else
				return NULL;
		}
	}
	*pnRead = nRead;

	return pvBuffer;
}

static HCHAR*	umma_fm_toolkit_GetChunkName(UMMA_OPS_t *pstOps, UMMA_FM_TKINFO_t *pstTkInfo)
{
	HCHAR	*pszSource;

	pszSource = HLIB_CBUF_GetBuffer(pstTkInfo->nCBuf, 1024);
	HxSTD_snprintf(pszSource, 1024, "%s/chunk.%d", pstOps->pszSourceName, ++pstOps->stDetail.ex.nChunkNo);
	HxLOG_Debug("chunk name : [%s]\n", pszSource);

	return pszSource;
}

#define	UMMA_GET_SIZE(block, bsize)		(((block) * (bsize) + 512ll) / 1024ll)
HERROR	umma_fm_toolkit_GetDeviceSizeKB(const HCHAR *pszMountPath, HUINT64 *pullTotalKBytes, HUINT64 *pullUsedKBytes, HUINT64 *pullFreeKBytes)
{
	struct statfs	fstatfs;

	if (statfs(pszMountPath, &fstatfs))
	{
		HxLOG_Critical("Mounted but can not find mount information[%s]!!!\n", pszMountPath);
		return ERR_FAIL;
	}

	if (fstatfs.f_blocks > 0)
	{
		if (pullTotalKBytes)
			*pullTotalKBytes = UMMA_GET_SIZE(fstatfs.f_blocks, fstatfs.f_bsize);
		if (pullUsedKBytes)
			*pullUsedKBytes  = UMMA_GET_SIZE(fstatfs.f_blocks - fstatfs.f_bavail, fstatfs.f_bsize);
		if (pullFreeKBytes)
			*pullFreeKBytes  = UMMA_GET_SIZE(fstatfs.f_bavail, fstatfs.f_bsize);

		return ERR_OK;
	}

	return ERR_FAIL;
}

static HUINT32	umma_fm_toolkit_CheckSpace(UMMA_FM_TKINFO_t *pstTkInfo, const HCHAR *pszSourceName, const HCHAR *pszDestName, const HUINT64 ullReservedKb)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return UMERR_OK;
#else
#define	MARGIN_FREE_SIZE	10240LL		//	10MB
#define	SIZE_4G				(4 * 1024 * 1024)	//	4G
	HCHAR	*pszMountPath, szfsType[16];
	struct stat	fstat;
	HUINT64	ullFreeSize, ullFileSize;

	pszMountPath = HLIB_CBUF_GetBuffer(pstTkInfo->nCBuf, 256);
	if (HLIB_DIR_GetMountPath(pszDestName, pszMountPath, 256) == ERR_OK)
	{
		HxLOG_Debug("pszDestName [%s], pszMountPath [%s]\n", pszDestName, pszMountPath);
		if (umma_fm_toolkit_GetDeviceSizeKB(pszMountPath, NULL, NULL, &ullFreeSize) == ERR_OK)
		{
			HxLOG_Debug("ullFreeSize : [%lld], ullReservedKb : [%lld]\n", ullFreeSize, ullReservedKb);
			HxLOG_Debug("pszSourceName : [%s]\n", pszSourceName);
			if (stat(pszSourceName, &fstat) == 0)
			{
				if (!S_ISREG(fstat.st_mode))
				{
					HxLOG_Error("Not file descriptor!! [%s]\n", pszSourceName);

					return UMERR_REASON_INVALID_ARG;
				}

				ullFileSize = (fstat.st_size / 1024);
				if (ullFileSize > SIZE_4G)
				{
					if (HLIB_DIR_GetMountFstype(pszMountPath, szfsType, 16) == ERR_OK)
					{
						#define	LIMIT_4G_FS			"fat32,fat16,vfat"
						if (HLIB_STD_StrCaseStr(LIMIT_4G_FS, szfsType))
							return UMERR_REASON_SIZE_LIMIT_4G;
					}
						else
					return UMERR_REASON_INVALID_ARG;
				}

				ullFileSize += MARGIN_FREE_SIZE;

				if ((HINT64)ullFileSize > (HINT64)(ullFreeSize - ullReservedKb))
				{
					HxLOG_Error("Not enough space [%s](%lld KBytes)\n", pszSourceName, ullFileSize);
					return UMERR_REASON_NOT_ENOUGH_SPACE;
				}

				return UMERR_OK;
			}
			HxLOG_Error("Fail to get source info(%s) [%s] \n", strerror(errno), pszSourceName);
			return UMERR_REASON_INVALID_ARG;
		}
		return UMERR_REASON_INVALID_ARG;
	} else
	{
		//	we assumed it was network device --> forced copy.
		return UMERR_OK;
	}
#endif /* CONFIG_PROD_OS_EMULATOR */
}

HERROR umma_fm_toolkit_CheckPath(const HCHAR *pszSrc,const HCHAR *pszDest,HUINT32 *RetErrCode)
{
    DxStorage_Info_t storage;

    if(UMMA_FS_GetStorageInfoByPath(pszSrc,&storage) ==ERR_FAIL)
    {
        *RetErrCode = UMERR_REASON_FAIL_TO_READ;
        return ERR_FAIL;
    }

    if(UMMA_FS_GetStorageInfoByPath(pszDest,&storage) ==ERR_FAIL)
    {
         *RetErrCode = UMERR_REASON_FAIL_TO_WRITE;
         return ERR_FAIL;
    }
    return ERR_OK;
}

HUINT32	UMMA_FM_TOOLKIT_Copy(UMMA_OPS_t *pstOps, HBOOL bStatus, HUINT64 ullReservedKb)
{
	void	*pvData;
	HCHAR	*pszSourceName;
	HINT32	i, nRead, nWritten;
	HUINT32	ulRet;
	UMMA_FM_TKINFO_t	*pstTkInfo = umma_fm_toolkit_GetTkInfo();
	UMMA_OPS_DETAIL_t	*pstDetail = &pstOps->stDetail;
	HINT32              retErrCode = UMERR_OK;

	//	Cancel Copy
	if (bStatus == FALSE)
	{
		if (pstDetail->pstSourceHandle)
			HLIB_FILE_Close(pstDetail->pstSourceHandle);
		if (pstDetail->pstDstHandle)
			HLIB_FILE_Close(pstDetail->pstDstHandle);

		return UMERR_REASON_CANCELED;
	}

	if (pstDetail->bOnGoing == FALSE)
	{
		HCHAR	*pszDirName;

		//	if chunk flag is on
		if (pstDetail->bChunked)
			pszSourceName = umma_fm_toolkit_GetChunkName(pstOps, pstTkInfo);
		else
			pszSourceName = pstOps->pszSourceName;

		ulRet = umma_fm_toolkit_CheckSpace(pstTkInfo, pszSourceName, pstOps->pszDestName, ullReservedKb);
		if (ulRet != UMERR_OK)
			return ulRet;

		pstDetail->pstSourceHandle = HLIB_FILE_Open(pszSourceName, "rb");
		if (pstDetail->pstSourceHandle == NULL)
		{
			HxLOG_Error("Source is not valid (%s)\n", pszSourceName);
			return UMERR_REASON_INVALID_ARG;
		}

		HxLOG_Debug("Copy Start [%s - %s]\n", pstOps->pszSourceName, pstOps->pszDestName);
		pszDirName = HLIB_CBUF_GetBuffer(pstTkInfo->nCBuf, 1024);
		HLIB_PATH_DirName(pstOps->pszDestName, pszDirName, 1024);

		HxLOG_Debug("pszDirName : [%s]\n", pszDirName);
		if (HLIB_DIR_IsExist(pszDirName) == FALSE)
			HLIB_DIR_Make(pszDirName);

		pstDetail->pstDstHandle = HLIB_FILE_Open(pstOps->pszDestName, "wb");
		if (pstDetail->pstDstHandle == NULL)
		{
			HxLOG_Error("Destination is not valid (%s)\n", pstOps->pszDestName);
			HLIB_FILE_Close(pstDetail->pstSourceHandle);
			return UMERR_REASON_INVALID_ARG;
		}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(pstOps->pszDestName, UID_umma, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", pstOps->pszDestName);
		}

		if (HLIB_EXTRA_Chmod(pstOps->pszDestName, 0660) != ERR_OK)
		{
			HxLOG_Error("	Fail to chmod path = %s \n", pstOps->pszDestName);
		}
#endif

		pstDetail->bOnGoing = TRUE;
	}
	for (i = 0; i < UMMA_FM_BLOCK_OP_NUM; i++)
	{
		// 강제 제거된 디바이스...
        if (umma_fm_toolkit_CheckPath(pstOps->pszSourceName,pstOps->pszDestName, &retErrCode) == ERR_FAIL)
        {
            if (pstDetail->pstSourceHandle)
            {
                HLIB_FILE_Close(pstDetail->pstSourceHandle);
            }
            if (pstDetail->pstDstHandle)
            {
                HLIB_FILE_Close(pstDetail->pstDstHandle);
            }
            return retErrCode;
        }

		pvData = umma_fm_toolkit_ReadFile(pstOps, pstTkInfo, &nRead);
		if (pvData)
		{
			if (nRead == 0)
			{
				//	Check Chunk file if needs;
				if (pstDetail->bChunked)
				{
					HBOOL bIsExist = FALSE;
					if (pstDetail->pstSourceHandle)
						HLIB_FILE_Close(pstDetail->pstSourceHandle);

					pszSourceName = umma_fm_toolkit_GetChunkName(pstOps, pstTkInfo);
					bIsExist = HLIB_FILE_Exist(pszSourceName);
					if(bIsExist != TRUE)
					{
						HxLOG_Print("End of chunk. srcName(%s) \n",pszSourceName);
						if(pstDetail->pstDstHandle)
							HLIB_FILE_Close(pstDetail->pstDstHandle);

						return UMERR_OK;
					}
					ulRet = umma_fm_toolkit_CheckSpace(pstTkInfo, pszSourceName, pstOps->pszDestName, ullReservedKb);
					if (ulRet != UMERR_OK)
						return ulRet;
					pstDetail->pstSourceHandle = HLIB_FILE_Open(pszSourceName, "rb");
					if (pstDetail->pstSourceHandle)
						return UMERR_ON_GOING;
				}

				HxLOG_Debug("Copy Complete (%s) (%s)\n", pstOps->pszSourceName, pstOps->pszDestName);
				if (pstDetail->pstSourceHandle)
					HLIB_FILE_Close(pstDetail->pstSourceHandle);
				if (pstDetail->pstDstHandle)
					HLIB_FILE_Close(pstDetail->pstDstHandle);

				return UMERR_OK;
			} else
			{
			write_rtn:
				nWritten = HLIB_FILE_Write(pstDetail->pstDstHandle, pvData, 1, nRead);
				if (nWritten < 0)
				{
					if (EINTR == errno)
						goto write_rtn;
					else
					{
						HLIB_FILE_Close(pstDetail->pstSourceHandle);
						HLIB_FILE_Close(pstDetail->pstDstHandle);
						return UMERR_REASON_FAIL_TO_WRITE;
					}
				}
			}
		} else
		{
			HLIB_FILE_Close(pstDetail->pstSourceHandle);
			HLIB_FILE_Close(pstDetail->pstDstHandle);

			return UMERR_REASON_FAIL_TO_READ;
		}
	}

	return UMERR_ON_GOING;
}

HUINT32	UMMA_FM_TOOLKIT_Delete(const HCHAR *pszFileName)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return UMERR_ON_GOING;
#else
	HINT64		llSize;
	struct stat	fstat;

	if (stat(pszFileName, &fstat) != 0)
		return UMERR_REASON_FAIL_TO_STAT;

	llSize = fstat.st_size;
	if (llSize < UMMA_FM_REMOVE_STEP_SIZE)
	{
		HLIB_FILE_Delete(pszFileName);

		return UMERR_OK;
	}

	llSize = llSize - UMMA_FM_REMOVE_STEP_SIZE;
	truncate(pszFileName, llSize);

	return UMERR_ON_GOING;
#endif/* CONFIG_PROD_OS_EMULATOR*/
}

HERROR	UMMA_FM_TOOLKIT_GetDeviceSizeKB(const HCHAR *pszMountPath, HUINT64 *pullTotalKBytes, HUINT64 *pullUsedKBytes, HUINT64 *pullFreeKBytes)
{
	return umma_fm_toolkit_GetDeviceSizeKB( pszMountPath, pullTotalKBytes, pullUsedKBytes, pullFreeKBytes );
}

HCHAR ** UMMA_FM_TOOLKIT_GetExtRec (void)
{
	static HUINT8 *s_acRecExt[] = {
					".hjm", ".hjtsm", ".hmt", ".nts",
					NULL};

	return (HCHAR **)s_acRecExt;
}

HBOOL	UMMA_FM_TOOLKIT_ExtSelector (HCHAR *pszName, HCHAR *filterstrarray[])
{
	HINT32	i, len_filename, len_ext;

	if (pszName == NULL)
	{
		return FALSE;
	}

	if (filterstrarray == NULL)
	{
		return TRUE;
	}

	for (i = 0; filterstrarray[i] != NULL; i++)
	{
		len_filename = HxSTD_StrLen(pszName);
		len_ext = HxSTD_StrLen(filterstrarray[i]);
		if (len_filename < len_ext)
		{
			continue;
		}

		if (HLIB_STD_StrNCaseCmp((pszName + (len_filename - len_ext)), filterstrarray[i], len_ext) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

