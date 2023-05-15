
/********************************************************************
 * Workfile   : fdi_nvram_unpack.c
 * Project    : HDR-1000S
 * Author     : 
 * Description: Commands Template
 *
 *                                 Copyright (c) 2009 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#include <string.h>

#include "htype.h"
#include "di_err.h"
#include "di_nvram.h"

#include "fvk.h"
#include "fdi_common.h"
#include "fdi_nvram.h"
#include "drv_err.h"
#include "vkernel.h"

typedef enum
{
	PACK_TAG_START,
	PACK_TAG_FILENAME,
	PACK_TAG_PARTITION,
	PACK_TAG_ADD,
	PACK_TAG_RM,
	PACK_TAG_DATA,
	PACK_TAG_MOD,
	PACK_TAG_OWNER,
	PACK_TAG_MAX,
	PACK_TAG_END = 0xFFFFFFFF,
} CFG_DATA_TYPE_E;

typedef enum
{
	FILE_ADD,
	FILE_RM
} RM_TYPE_E;

typedef struct
{
	HUINT8		*target;		// for "target"
	HUINT32		partition;		// for partition
	HUINT32		partition_set;	// for partition
	RM_TYPE_E	add_rm;			// for add or rm
	HUINT32		mod;			// for add or rm
	HUINT32		mod_set;		// for add or rm
	HUINT8		*owner;			// for "target"
	HUINT8		*pData;
	HUINT32		dataLen;
} UNPACK_LIST_T;

typedef struct
{
	HUINT8 *pBuf;
	HUINT32 len;
	HUINT32 read;
} BUF_T;


#define get32bit(q) (unsigned int)(((*(unsigned char *)(q)) << 24) | (*((unsigned char *)(q)+1) << 16) | (*((unsigned char *)(q)+2) << 8) | (*((unsigned char *)(q)+3)) )

static HUINT32 read4bytes(BUF_T *pBuf, HUINT32 *data)
{
	HUINT8	buf[4];

	VK_memcpy(buf, pBuf->pBuf+pBuf->read, 4);
	pBuf->read += 4;
	*data = get32bit(buf);

	return ERR_OK;
}

static HUINT32 readnbytes(BUF_T *pBuf, HUINT32 len, HUINT8 **data)
{
	*data = DD_MEM_Alloc(len+1);
	if (*data == NULL)
	{
		return ERR_FAIL;
	}

	VK_memset(*data, 0, len+1);

	VK_memcpy(*data, pBuf->pBuf+pBuf->read, len);
	pBuf->read += len;

	return ERR_OK;
}

static HUINT32 freeUnpackData(UNPACK_LIST_T *pUnpackList)
{
	if (pUnpackList == NULL)
		return ERR_FAIL;

	if (pUnpackList->target)
		DD_MEM_Free(pUnpackList->target);
	if (pUnpackList->owner)
		DD_MEM_Free(pUnpackList->owner);
	if (pUnpackList->pData)
		DD_MEM_Free(pUnpackList->pData);

	VK_memset(pUnpackList, 0, sizeof(UNPACK_LIST_T));
	return ERR_OK;
}

FDI_ERR_CODE FDI_NVRAM_UnpackNvramCfg(unsigned char *pBuf, unsigned int length)
{
	HUINT32			tag;
	HUINT32			len;
	HUINT32			data;
	HUINT8			*pData;
	BUF_T 			stBuf;
	UNPACK_LIST_T	s_stUnpackDataList;

	if (pBuf == NULL)
	{
		return ERR_FAIL;
	}

	VK_memset(&stBuf, 0, sizeof(BUF_T));
	stBuf.len = length;
	stBuf.pBuf = pBuf;

	VK_memset(&s_stUnpackDataList, 0, sizeof(UNPACK_LIST_T));
	pData = NULL;

	while (stBuf.read < stBuf.len)
	{
		if (read4bytes(&stBuf, &tag) != ERR_OK)
			goto RTN_Err;
		
		if (read4bytes(&stBuf, &len) != ERR_OK)
			goto RTN_Err;

		if (tag != PACK_TAG_START)
		{
			if (readnbytes(&stBuf, len, &pData) != ERR_OK)
				goto RTN_Err;
			
			if (pData != NULL)
			{
				DD_MEM_Free(pData);
				pData = NULL;
			}
			
			continue;
		}
		
		if (read4bytes(&stBuf, &data) != ERR_OK)
			goto RTN_Err;

		while (tag != PACK_TAG_END)
		{
			if (read4bytes(&stBuf, &tag) != ERR_OK)
				goto RTN_Err;
			
			if (read4bytes(&stBuf, &len) != ERR_OK)
				goto RTN_Err;

			switch (tag)
			{
			case PACK_TAG_FILENAME :
				if (readnbytes(&stBuf, len, &s_stUnpackDataList.target) != ERR_OK)
					goto RTN_Err;
				
				break;
				
			case PACK_TAG_PARTITION :
				if (read4bytes(&stBuf, &s_stUnpackDataList.partition) != ERR_OK)
					goto RTN_Err;
				
				s_stUnpackDataList.partition_set = 1;
				
				break;
				
			case PACK_TAG_ADD :
				if (read4bytes(&stBuf, &data) != ERR_OK)
					goto RTN_Err;
				
				s_stUnpackDataList.add_rm = FILE_ADD;
				
				break;
				
			case PACK_TAG_RM :
				if (read4bytes(&stBuf, &data) != ERR_OK)
					goto RTN_Err;
				
				s_stUnpackDataList.add_rm = FILE_RM;
				
				break;
				
			case PACK_TAG_DATA :
				if (readnbytes(&stBuf, len, &s_stUnpackDataList.pData) != ERR_OK)
					goto RTN_Err;
				
				s_stUnpackDataList.dataLen = len;
				
				break;
				
			case PACK_TAG_MOD :
				if (read4bytes(&stBuf, &s_stUnpackDataList.mod) != ERR_OK)
					goto RTN_Err;
				
				s_stUnpackDataList.mod_set = 1;
				
				break;
				
			case PACK_TAG_OWNER :
				if (readnbytes(&stBuf, len, &s_stUnpackDataList.owner) != ERR_OK)
					goto RTN_Err;
				
				break;
				
			case PACK_TAG_END :
				if (read4bytes(&stBuf, &data) != ERR_OK)
					goto RTN_Err;
				
				break;
			}
		}

		if (s_stUnpackDataList.add_rm == FILE_RM)
		{
			DI_NVRAM_Remove(s_stUnpackDataList.partition, s_stUnpackDataList.target);
		}
		else
		{
			FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_NVRAM_UnpackNvramCfg] : NVRAM Update [%s]\n", s_stUnpackDataList.target));
			
			DI_NVRAM_Write(s_stUnpackDataList.partition, s_stUnpackDataList.target, 0, s_stUnpackDataList.pData, s_stUnpackDataList.dataLen);
		}
		
		freeUnpackData(&s_stUnpackDataList);
	}

	return FDI_NO_ERR;

RTN_Err:
	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_NVRAM_UnpackNvramCfg] : Error\n"));

	return FDI_ERR;
}

