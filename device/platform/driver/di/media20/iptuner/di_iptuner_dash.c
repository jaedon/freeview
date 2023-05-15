/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dash.c  $
 * Version:			$Revision:   $
 * Original Author:
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     DASH - VFIO Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup DASH_VFIO_CONTROLLER
 * @brief DASH VFIO Controller. This is interlocked with Nexus playback.
 */

/**
 * @author Jeehune Park (parkjh4@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief DASH VFIO Controller. This is interlocked with Nexus playback.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_dash.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"

#include <inttypes.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "htype.h"

#include "bstd.h"
#include "vfio20.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_media20.h"
#include "drv_media20.h"
#include "di_uart.h"
#include "di_iptuner20.h"
#include "di_iptuner_http.h"
#include "di_iptuner_pool.h"
#include "di_iptuner_adaptive.h"
#include "di_iptuner_dash.h"
#include "util_url.h"
#include "util_sys.h"
#include "xmlmemory.h"
#include "parserInternals.h"
#include "parser.h"
#include "tree.h"

#include "util_mp4header_parser.h"

#include "stream_global.h"
#include "stream_http.h"

//#define DASH_MEMORY_DEBUG
#ifdef DASH_MEMORY_DEBUG
#include "util_memtool.h"
#define VK_MEM_Alloc(X)		EIKI_MEM_Alloc((HUINT32)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#define VK_MEM_Free(X)		EIKI_MEM_Free((void*)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#endif

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
//#define DI_IPTUNER_DASH_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_DASH_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)


#define DASH_FOURCC_H264 "H264"
#define DASH_FOURCC_WVC1 "WVC1"
#define DASH_FOURCC_AVC1 "AVC1"
#define DASH_FOURCC_AACL "AACL"
#define DASH_FOURCC_WMAP "WMAP"
#define DASH_FOURCC_AC_3 "AC-3"
#define DASH_FOURCC_EC_3 "EC-3"

/* ISO/IEC 13818-7:2005(E) */
/* Table 35 . Sampling frequency dependent on sampling_frequency_index */
static const unsigned b_aac_adts_sample_rate[]={
	96000,
	88200,
	64000,
	48000,
	44100,
	32000,
	24000,
	22050,
	16000,
	12000,
	11025,
	8000
};

/* Table 42 . Channel Configuration */
static const uint8_t b_aac_adts_channels[]={
	0,
	1,
	2,
	3,
	4,
	5,
	5+1,
	7+1
};

/* End static variable */
/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
static size_t mem_file_write(DASH_MEM_FILE_t *file,const void *ptr, size_t size)
{
/*    BDBG_ASSERT(file->offset + size<= file->buf_len); */
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
	if(file->offset + size > file->buf_len)
	{
		return -1;
	}
    VK_memcpy((uint8_t *)file->buf + file->offset, ptr, size);
    file->offset += size;
    if(file->length<file->offset) {
        file->length = file->offset;
    }
    return size;
}

static size_t mem_file_read(DASH_MEM_FILE_t *file, void *ptr, size_t size)
{
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
    if(file->offset + size > file->buf_len)
	{
		return -1;
	}
    VK_memcpy(ptr,(uint8_t *)file->buf + file->offset, size);
    file->offset += size;
    return size;
}

static size_t mem_file_tell(DASH_MEM_FILE_t *file)
{
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
    return file->offset;
}

static size_t mem_file_seek(DASH_MEM_FILE_t *file, size_t offset)
{
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
	if(offset > file->buf_len)
	{
		return -1;
	}
    file->offset = offset;
    return offset;
}

static DASH_MEM_FILE_t* mem_file_open(size_t nLen)
{
	DASH_MEM_FILE_t* file = NULL;
	file = (DASH_MEM_FILE_t*)VK_MEM_Alloc(sizeof(DASH_MEM_FILE_t));
	if(file == NULL)
	{
		return NULL;
	}
	VK_MEM_Memset(file,0x00,sizeof(DASH_MEM_FILE_t));
	file->buf = VK_MEM_Alloc(nLen);
	if(file->buf == NULL)
	{
		if(file != NULL)
		{
			VK_MEM_Free(file);
			file = NULL;
		}
		return NULL;
	}
	VK_MEM_Memset(file->buf,0x00,nLen);
	file->length = 0;
	file->buf_len = nLen;
	file->bIsOpened = TRUE;
	return file;
}

static HBOOL mem_file_close(DASH_MEM_FILE_t *file)
{
	if(file == NULL)
	{
		return FALSE;
	}
	if(!file->bIsOpened)
	{
		return FALSE;
	}
	if(file->buf != NULL)
	{
		VK_MEM_Free(file->buf);
		file->buf = NULL;
	}
	VK_MEM_Memset(file,0x00,sizeof(DASH_MEM_FILE_t));
	file->bIsOpened = FALSE;
	VK_MEM_Free(file);
	file = NULL;
	return TRUE;
}


static size_t mem_file_bound(DASH_MEM_FILE_t *file)
{
    return file->length;
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function for bmp4 box buffer ( uint8 ) <br>
 * Write given parameter in unit8 to bmp4 box buffer <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] d	: value that will be written to mem_file

 * @return  #  void <br>
 */
static void
write_uint8(DASH_MEM_FILE_t *fout, uint8_t d)
{
	mem_file_write(fout, &d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function ( uint24_big_endian ) <br>
 * Write given parameter in uint24 as big-endian to bmp4 box buffer <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] data	: value that will be written to mem_file

 * @return  #  void <br>
 */
static void
write_uint24_be(DASH_MEM_FILE_t *fout, uint32_t data)
{
	uint8_t d[3];
	d[0] = data >> 16;
	d[1] = data >> 8;
	d[2] = data;
	mem_file_write(fout, d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function ( uint32_big_endian ) <br>
 * Write given parameter in uint32 as big-endian to bmp4 box buffer <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] data	: value that will be written to mem_file
 * @return  #  void <br>
 */
static void
write_uint32_be(DASH_MEM_FILE_t *fout, uint32_t data)
{
	uint8_t d[4];
	d[0] = data >> 24;
	d[1] = data >> 16;
	d[2] = data >> 8;
	d[3] = data;
	mem_file_write(fout, d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function ( uint64_big_endian ) <br>
 * Write given parameter in uint64 as big-endian to bmp4 box buffer <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] data	: value that will be written to mem_file
 * @return  #  void <br>
 */
static void
write_uint64_be(DASH_MEM_FILE_t *fout, uint64_t data)
{
	uint8_t d[8];
	d[0] = data >> 56;
	d[1] = data >> 48;
	d[2] = data >> 40;
	d[3] = data >> 32;
	d[4] = data >> 24;
	d[5] = data >> 16;
	d[6] = data >> 8;
	d[7] = data;
	mem_file_write(fout, d, sizeof(d));
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function ( *uint8 ) <br>
 * Write given parameter in uint8 pointer as big-endian to bmp4 box buffer <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] data	: pointer that is pointing to data which will be written to mem_file
 * @param[in] len	: size of data
 *
 * @return  #  void <br>
 */
static void
write_data(DASH_MEM_FILE_t *fout, const uint8_t *data, size_t len)
{
	mem_file_write(fout, data, len);
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function ( FourCC ) <br>
 * Write given FourCC to bmp4 box buffer <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] fourcc	: FourCC value that will be written to mem_file
 *
 * @return  #  void <br>
 */
static void
write_fourcc(DASH_MEM_FILE_t *fout, const char *fourcc)
{
	write_data(fout, (const uint8_t *)fourcc, 4);
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function ( box type ) <br>
 * This function is used when MP4 Box start. This initialize a box size then write box type to mem_file I/O handle <br>
 * The structure of MP4 FullBox is below <br>
 *     | Box Size (32bits) | BoxType (32bits) | Version (8bits) | Flags (24bits) | Value | <br>
 * The structure of bmp4 box is below <br>
 *     | Box Size (32bits) | BoxType "bmp4" (32bits) | MP4 Full Boxes ... |
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] type	: type value that will be written to mem_file
 *
 * @return  #  offset after writing <br>
 */
static int
start_mp4_box(DASH_MEM_FILE_t *fout, const char *type)
{
	int offset = mem_file_tell(fout);
	write_uint32_be(fout, 0); /* size */
	write_fourcc(fout, type); /* type */
	return offset;
}

/**
 * @b Function @b Description <br>
 * Virtual File I/O Sub Function (box type) <br>
 * This function is used when finishing MP4 Box. <br>
 * Box Size indicates only size of value. <br>
 * Parameter 'offset' is a start offset of value <br>
 *
 * @param[in] mem_file	: mem_file I/O handle
 * @param[in] offset	: start off set of box value
 *
 * @return  # void <br>
 */
static void
finish_mp4_box(DASH_MEM_FILE_t *fout, int offset)
{
	int current = mem_file_tell(fout);
	mem_file_seek(fout, offset);
	write_uint32_be(fout, current-offset); /* size */
	mem_file_seek(fout, current);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( box type ) <br>
 * bmp4 Box 종료시 사용하며 bmp4 box buffer의 0 offset에 Box Size를 Write 한다.  <br>
 * 인자에 사용되는 Size는 bmp4 box의 MP4 Fullboxes 의 전체 크기이다.  <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] size	: MP4 Fullboxes 의 전체 크기
 *
 * @return  # void <br>
 */
static void
close_mp4_box(DASH_MEM_FILE_t *fout, int size)
{
	int current = mem_file_tell(fout);
	mem_file_seek(fout, 0);
	write_uint32_be(fout, size); /* size */
	mem_file_seek(fout, current);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( box type ) <br>
 * MP4 Full Box 시작시 사용하며 Box Size를 초기화하고 Box Type, Version, Flags 의 값을 Write 한다.  <br>
 * MP4 FullBox의 구조는 | Box Size (32bits) | BoxType (32bits) | Version (8bits) | Flags (24bits) | Value | 로 구성된다. <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] type	: Write 할 type value
 * @param[in] version	: Write 할 version value
 * @param[in] flags	: Write 할 flags value
 *
 * @return  #  Write 후 이동한 offset <br>
 */
static int
start_mp4_fullbox(DASH_MEM_FILE_t *fout, const char *type, unsigned version, uint32_t flags)
{
	int offset = start_mp4_box(fout, type);
	write_uint8(fout, version);
	write_uint24_be(fout, flags);
	return offset;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( TREX BOX ) <br>
 * trex Box(full box) 를 생성한다. <br>
 * Track ID는 Setup 시 사용되는 Stream ID와 같아야한다.
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] track_ID : Write 할 Track ID (Stream ID)
 * @param[in] default_sample_description_index : Write 할 default_sample_description_index value
 * @param[in] default_sample_duration : Write 할 default_sample_duration value
 * @param[in] default_sample_size : Write 할 default_sample_size value
 * @param[in] default_sample_flags : Write 할 default_sample_flags value
 *
 * @return  #  void <br>
 */
static void
write_trex_box(DASH_MEM_FILE_t *fout, uint32_t track_ID, uint32_t default_sample_description_index, uint32_t default_sample_duration, uint32_t default_sample_size, uint32_t default_sample_flags)
{
	int offset = start_mp4_fullbox(fout, "trex", 0, 0);
	write_uint32_be(fout, track_ID);
	write_uint32_be(fout, default_sample_description_index);
	write_uint32_be(fout, default_sample_duration);
	write_uint32_be(fout, default_sample_size);
	write_uint32_be(fout, default_sample_flags);
	finish_mp4_box(fout, offset);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( BDAT BOX ) <br>
 * bdat Box(full box) 를 생성한다. <br>
 * 인자로 사용되는 data는 CodecPrivateData를 나타낸다.
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] data : Write 할 CodecPrivateData
 * @param[in] data_len : CodecPrivateData 의 크기
 *
 * @return  #  void <br>
 */
static void
write_bdat_box(DASH_MEM_FILE_t *fout,	const uint8_t *data, size_t data_len)
{
	int offset = start_mp4_box(fout, "bdat");
	write_data(fout, data, data_len);
	finish_mp4_box(fout, offset);
	return;
}

/**
 * @b Function @b Description <br>
 * bmp4 box buffer에 대한 Virture File I/O Sub Function ( BHED BOX ) <br>
 * bhed Box(full box) 를 생성한다. <br>
 *
 * @param[in] mem_file	: bmp4 box의 mem_file I/O handle
 * @param[in] timescale : timescale
 * @param[in] start_time : 해당 fragment의 start_time
 * @param[in] fourcc : 해당 fragment의 fourcc
 *
 * @return  #  void <br>
 */
static void
write_bhed_box(DASH_MEM_FILE_t *fout, uint64_t timescale, uint64_t start_time, const char *fourcc)
{
	int offset = start_mp4_box(fout, "bhed");
	write_uint64_be(fout, timescale);
	write_uint64_be(fout, start_time);
	write_fourcc(fout, fourcc);
	finish_mp4_box(fout, offset);
	return;
}

static void
uint2array(char *dst, unsigned int src)
{
	int mask = 0xff;
	dst[0] = mask & (src >> 24);
	dst[1] = mask & (src >> 16);
	dst[2] = mask & (src >> 8);
	dst[3] = mask & src;
}

static DASH_MP4_CACHE_t* P_STREAM_DASH_MP4CACHE_Create(HINT32 nrCacheElement)
{
	DASH_MP4_CACHE_t* cache = VK_MEM_Alloc(sizeof(DASH_MP4_CACHE_t));
	if(cache == NULL)
	{
		PrintError("%s(%d) Not enough memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_MEM_Memset(cache, 0x0, sizeof(DASH_MP4_CACHE_t));
	cache->szUnused = nrCacheElement;
	return cache;
}

static HINT32 P_STREAM_DASH_MP4CACHE_Write(DASH_MP4_CACHE_t* hCache, void* pBuf, HINT32 szBuf, HINT32 nCacheID, HINT32 nElementType)
{
	HUINT32 nListSize = 0;
	HUINT32 loopcnt = 0;

	if(hCache == NULL)
	{
		PrintError("%s(%d) Cache handle is NULL! \n", __FUNCTION__, __LINE__);
		return 0;
	}
	if(pBuf == NULL)
	{
		PrintError("%s(%d) Buffer is NULL! \n",__FUNCTION__, __LINE__);
		return 0;
	}
	if(szBuf == 0)
	{
		PrintError("%s(%d) Buffer size is 0! \n",__FUNCTION__, __LINE__);
		return 0;
	}

	nListSize = UTIL_DLLIST_Size(&hCache->cache);
	for(loopcnt = 0 ; loopcnt < nListSize; loopcnt++)
	{
		UTIL_DLLNode_t* pNode = UTIL_DLLIST_Get_n_Node(&hCache->cache, loopcnt);
		DASH_MP4_CACHE_ELEMENT_t* pElement = pNode->pData;
		if(pElement->nCacheID == nCacheID && pElement->nElementType == nElementType)
		{
			PrintError("%s(%d) Same element ID and Same element type exists in cache\n",__FUNCTION__, __LINE__);
			return 0;
		}
	}

	DASH_MP4_CACHE_ELEMENT_t* pCacheElement = VK_MEM_Alloc(sizeof(DASH_MP4_CACHE_ELEMENT_t));
	if(pCacheElement == NULL)
	{
		PrintError("%s(%d) Not enough memory\n", __FUNCTION__, __LINE__);
		return 0;
	}
	VK_MEM_Memset(pCacheElement, 0x0, sizeof(DASH_MP4_CACHE_ELEMENT_t));
	pCacheElement->nCacheID = nCacheID;
	pCacheElement->nElementType = nElementType;
	pCacheElement->szBuf = szBuf;
	pCacheElement->mp4buf = VK_MEM_Alloc(szBuf);
	VK_MEM_Memcpy(pCacheElement->mp4buf, pBuf, szBuf);
	UTIL_DLLIST_Append(&hCache->cache, pCacheElement);

	hCache->szUsed++;
	hCache->szUnused--;

	return szBuf;
}

static HINT32 P_STREAM_DASH_MP4CACHE_Read(DASH_MP4_CACHE_t* hCache, void* pReadBuf, HINT32 nCacheID, HINT32 nElementType)
{
	DASH_MP4_CACHE_ELEMENT_t* pElement = NULL;
	HUINT32 loopcnt = 0;

	if(hCache == NULL)
	{
		PrintError("%s(%d) Cache handle is NULL! \n", __FUNCTION__, __LINE__);
		return 0;
	}
	if(pReadBuf == NULL)
	{
		PrintError("%s(%d) Buffer is NULL! \n",__FUNCTION__, __LINE__);
		return 0;
	}

	for(loopcnt = 0 ; loopcnt < UTIL_DLLIST_Size(&hCache->cache); loopcnt++)
	{
		UTIL_DLLNode_t* pNode = UTIL_DLLIST_Get_n_Node(&hCache->cache, loopcnt);
		pElement = pNode->pData;
		if(pElement->nCacheID == nCacheID && pElement->nElementType == nElementType)
		{
			break;
		}
	}

	if(loopcnt == UTIL_DLLIST_Size(&hCache->cache))
	{
		PrintError("%s(%d) Requested element does not exist in cache\n", __FUNCTION__, __LINE__);
		return 0;
	}
#if 0
	UTIL_DLLNode_t* node = NULL;
	for(int i=0; i < UTIL_DLLIST_Size(&pElement->mp4buf->moov.mvex.trex); i++)
	{
		node = UTIL_DLLIST_Get_n_Node(&pElement->mp4buf->moov.mvex.trex,i);
		UTIL_MP4_PARSER_TREX_t* trex = node->pData;
		VK_printf("default_sample_description_index : %d\n", trex->default_sample_description_index);
		VK_printf("default_sample_duration : %d\n", trex->default_sample_duration);
		VK_printf("default_sample_flags : %d\n", trex->default_sample_flags);
		VK_printf("default_sample_size : %d\n", trex->default_sample_size);
	}
#endif
	VK_MEM_Memcpy(pReadBuf, pElement->mp4buf, pElement->szBuf);

	return pElement->szBuf;
}

static HBOOL P_STREAM_DASH_MP4CACHE_IsExist(DASH_MP4_CACHE_t* hCache, HINT32 nCacheID, HINT32 nElementType)
{
	DASH_MP4_CACHE_ELEMENT_t* pElement = NULL;
	HUINT32 loopcnt = 0;

	if(hCache == NULL)
	{
		PrintError("%s(%d) Cache handle is NULL! \n", __FUNCTION__, __LINE__);
		return 0;
	}

	for(loopcnt = 0 ; loopcnt < UTIL_DLLIST_Size(&hCache->cache); loopcnt++)
	{
		UTIL_DLLNode_t* pNode = UTIL_DLLIST_Get_n_Node(&hCache->cache, loopcnt);
		pElement = pNode->pData;
		if(pElement->nCacheID == nCacheID && pElement->nElementType == nElementType)
		{
			break;
		}
	}

	if(loopcnt == UTIL_DLLIST_Size(&hCache->cache))
	{
		return FALSE;
	}

	return TRUE;
}

static void P_STREAM_DASH_MP4CACHE_Reclaim(DASH_MP4_CACHE_t* hCache, HINT32 nElementType)
{
	if(hCache == NULL)
	{
		PrintError("%s(%d) Cache handle is NULL! \n", __FUNCTION__, __LINE__);
		return;
	}
	if(UTIL_DLLIST_Size(&hCache->cache) > 0)
	{
		UTIL_DLLNode_t* pNode = UTIL_DLLIST_ExtractHead(&hCache->cache);
		while(pNode != NULL)
		{
			DASH_MP4_CACHE_ELEMENT_t* pElement = pNode->pData;
			if(pElement != NULL)
			{
				if(pElement->nElementType == nElementType)
				{
					UTIL_MP4_PARSER_Release(pElement->mp4buf);
					VK_MEM_Free(pElement->mp4buf);
					pElement->mp4buf = NULL;
					VK_MEM_Free(pElement);
					pElement = NULL;
					pNode->pData = NULL;

					UTIL_DLLIST_DeleteNodes(pNode);
					hCache->szUsed--;
					hCache->szUnused++;
					break;
				}
			}
			pNode = UTIL_DLLIST_NextNode(pNode);
		}
	}
}

static void P_STREAM_DASH_MP4CACHE_Destroy(DASH_MP4_CACHE_t* hCache)
{
	HINT32 loopcnt = 0;
	HINT32 nrCacheElement = 0;
	if(hCache == NULL)
	{
		PrintError("%s(%d) Cache handle is NULL! \n", __FUNCTION__, __LINE__);
		return;
	}

	nrCacheElement = UTIL_DLLIST_Size(&hCache->cache);

	for(loopcnt = 0; loopcnt < nrCacheElement; loopcnt++)
	{
		UTIL_DLLNode_t* pNode = UTIL_DLLIST_ExtractHead(&hCache->cache);
		DASH_MP4_CACHE_ELEMENT_t* pElement = pNode->pData;

		// mp4buf destroy 필요
		if(pElement->mp4buf != NULL)
		{
			UTIL_MP4_PARSER_Release(pElement->mp4buf);
			VK_MEM_Free(pElement->mp4buf);
			pElement->mp4buf = NULL;
		}
		VK_MEM_Free(pElement);
		pElement = NULL;
		pNode->pData = NULL;
		UTIL_DLLIST_DeleteNodes(pNode);
	}
	VK_MEM_Free(hCache);
}

static DASH_ERROR_e P_STREAM_DASH_SetBufferCommand(MEDIA_DASH_HANDLE_t pMDASH, DASH_BUFFERCMD_e eCmd)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pMDASH->ulSema);
	PrintDebug("%s(%d) DASH_BUFFER_COMMAND was changed (%d -> %d)\n", __FUNCTION__, __LINE__, pMDASH->buffercmd, eCmd);
	pMDASH->buffercmd = eCmd;
	VK_SEM_Release(pMDASH->ulSema);

	return DASH_ERR_OK;
}

static DASH_ERROR_e P_STREAM_DASH_SetPreconnectorCommand(MEDIA_DASH_HANDLE_t pMDASH, DASH_PRECONCMD_e eCmd)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pMDASH->ulSema);
	//PrintDebug("%s(%d) DASH_PRECON_COMMAND was changed (%d -> %d)\n", __FUNCTION__, __LINE__, pController->preconcmd, eCmd);
	pMDASH->preconcmd = eCmd;
	VK_SEM_Release(pMDASH->ulSema);

	return DASH_ERR_OK;
}

static DASH_BUFFERCMD_e P_STREAM_DASH_GetBufferCommand(MEDIA_DASH_HANDLE_t pMDASH)
{
	DASH_BUFFERCMD_e buffercmd;
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_BUFFERCMD_INVALID;
	}
	VK_SEM_Get(pMDASH->ulSema);
	buffercmd = pMDASH->buffercmd;
	VK_SEM_Release(pMDASH->ulSema);

	return buffercmd;
}

static DASH_PRECONCMD_e P_STREAM_DASH_GetPreconnectorCommand(MEDIA_DASH_HANDLE_t pMDASH)
{
	DASH_PRECONCMD_e preconcmd;

	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_PRECONCMD_INVALID;
	}
	VK_SEM_Get(pMDASH->ulSema);
	preconcmd = pMDASH->preconcmd;
	VK_SEM_Release(pMDASH->ulSema);

	return preconcmd;
}

static DASH_ERROR_e P_STREAM_DASH_SetEventResult(MEDIA_DASH_HANDLE_t pMDASH, DASH_EVENT_RESULT_e eResult)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pMDASH->ulSema);
	pMDASH->eventResult = eResult;
	VK_SEM_Release(pMDASH->ulSema);

	return DASH_ERR_OK;
}

static DASH_EVENT_RESULT_e P_STREAM_DASH_GetEventResult(MEDIA_DASH_HANDLE_t pMDASH)
{
	DASH_EVENT_RESULT_e ret;

	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_PRECONCMD_INVALID;
	}
	VK_SEM_Get(pMDASH->ulSema);
	ret = pMDASH->eventResult;
	VK_SEM_Release(pMDASH->ulSema);

	return ret;
}

static DASH_ERROR_e P_STREAM_DASH_ClearEventResult(MEDIA_DASH_HANDLE_t pMDASH)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pMDASH->ulSema);
	pMDASH->eventResult = DASH_EVENT_RESULT_NONE;
	VK_SEM_Release(pMDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * convert DASH codec to DI codec <br>
 *
 * @param[in] codec	: DASH codec
 * @param[in] isAV : codec filter(AUDIO/VIDEO)
 *
 * @return  #  DI codec <br>
 */
static HINT32 P_STREAM_DASH_DASHCodecToDICodec(DASH_CODEC_e codec, DASH_CODEC_e isAV)
{
	/* DASH codec is described as bit field */

	/* codec filtering : Audio or Video */
	codec &= isAV;

	if(isAV == DASH_AUDIO_CODECS)
	{
		if((codec & DASH_AUDIO_CODEC_AAC) != 0)
		{
			return DI_AUDFORMAT_AAC;
		}
	}
	else if(isAV == DASH_VIDEO_CODECS)
	{
		if((codec & DASH_VIDEO_CODEC_AVC1) != 0)
		{
			return DI_VIDEO_FORMAT_MEDIA_H264;
		}
	}
	/** need more case **/
	return 0;
}

void P_STREAM_DASH_ConvertDASHCodecToFourCC(DASH_CODEC_e eCodec, HINT8* pBuf)
{
	PrintEnter;

	switch(eCodec)
	{
		case DASH_AUDIO_CODEC_AAC :
			pBuf[0] = 'A';
			pBuf[1] = 'A';
			pBuf[2] = 'C';
			pBuf[3] = 'L';
			break;
		case DASH_VIDEO_CODEC_AVC1 :
			pBuf[0] = 'A';
			pBuf[1] = 'V';
			pBuf[2] = 'C';
			pBuf[3] = '1';
			break;
		default :
			PrintDebug("%s(%d) cannot convert dash codec(%d) to fourcc\n", __FUNCTION__, __LINE__, eCodec);
			break;
	}
	PrintExit;
}

DASH_ERROR_e P_STREAM_DASH_ClearBox (MEDIA_DASH_HANDLE_t pMDASH)
{
	PrintEnter;
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Err> DASH Master handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pMDASH->mp4box->length = 0;
	pMDASH->mp4box->offset = 0;
	pMDASH->mp4box->bBoxing = FALSE;
	VK_MEM_Memset(pMDASH->mp4box->buf,0x00,pMDASH->mp4box->buf_len);

	PrintExit;
	return DASH_ERR_OK;
}

HUINT32	P_STREAM_DASH_MakeBox (MEDIA_DASH_HANDLE_t pMDASH, DASH_PRECONNECTOR_ITEM_t* pItem)
{
	DASH_MASTER_HANDLE_t pDASH;
	DASH_CONTROLLER_HANDLE_t pController;
	DASH_ERROR_e derr = DASH_ERR_OK;
	size_t offset = 0;
	HINT8 aFourCC[5] = {0,};
	HINT8 codecPrivData[DASH_CODECPRIVDATA_BUFFER] = {0};
	HINT32 eAudioCodec = 0;
	HINT32 eVideoCodec = 0;
	HUINT32 nCodecPrivateDataLen = 0;
	HUINT32 szRead = 0;
	HUINT32 loopcnt = 0;
	HUINT32 subloopcnt = 0;
	HUINT32 ulCodecPrivateData = 0;
	HUINT32 ulIndex = 0;
	HUINT32	szList = 0;
	HUINT32 szSubList = 0;
	HUINT64 ullTimeScale = 0;
	HUINT64 ullStartTime = 0;
	HBOOL bGotTrex = FALSE;
	UTIL_MP4_PARSER_t tInitSegment;
	UTIL_MP4_PARSER_TREX_t* trex = NULL;
	UTIL_DLLNode_t* dlllist = NULL;
	UTIL_DLLNode_t* subdlllist = NULL;
	UTIL_DLLNode_t* entrylist = NULL;

	PrintEnter;

	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Err> DASH Master handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pDASH = pMDASH->hDASH;
	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) Err> Cannot get controller handle\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	VK_MEM_Memset(aFourCC, 0x0, sizeof(aFourCC));
	VK_MEM_Memset(&tInitSegment, 0x0, sizeof(UTIL_MP4_PARSER_t));
	VK_MEM_Memset(codecPrivData, 0x0, DASH_CODECPRIVDATA_BUFFER);
	if(pMDASH->mp4box->bBoxing == TRUE)
	{
		PrintExit;
		return 0;
	}
	else
	{
		/* Get stream ID */
		if(pItem->eStreamType == DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY)
		{
			ulIndex = DASH_VIDEO_STREAM_ID;
		}
		else if(pItem->eStreamType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
		{
			ulIndex = DASH_AUDIO_STREAM_ID;
		}
		else if(pItem->eStreamType == DASH_ADAPTATIONSET_TYPE_AV)
		{
			ulIndex = DASH_VIDEO_STREAM_ID;
		}
		/* Get codec information */
		eAudioCodec = P_STREAM_DASH_DASHCodecToDICodec(pItem->eCodec, DASH_AUDIO_CODECS);
		eVideoCodec = P_STREAM_DASH_DASHCodecToDICodec(pItem->eCodec, DASH_VIDEO_CODECS);
		if(eAudioCodec == 0 && eVideoCodec == 0)
		{
			PrintError("%s(%d) Err> Codec information in MPD segment is NULL\n",__FUNCTION__,__LINE__);
			DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			PrintExit;
			return 0;
		}
		offset = start_mp4_box(pMDASH->mp4box, "bmp4");
		P_STREAM_DASH_ConvertDASHCodecToFourCC(pItem->eCodec, aFourCC);
		/* Build codec private data */
		if(eVideoCodec == DI_VIDEO_FORMAT_MEDIA_H264)
		{
			UTIL_DLLNode_t* nallist = NULL;
			HUINT32 NALloopcnt = 0;
			HUINT32 nPos = 0;
			/* get H264 codec info from parsed MP4 data */

			/* Read MP4 header from cache (It must be exist) */
			szRead = P_STREAM_DASH_MP4CACHE_Read(pMDASH->InitData, &tInitSegment, pItem->nBandwidth, pItem->eStreamType);
			if(szRead == 0)
			{
				PrintDebug("%s(%d) Initialisation segment data was not found in cache\n", __FUNCTION__, __LINE__);
				PrintExit;
				return 0;
			}
			szList = UTIL_DLLIST_Size(&tInitSegment.moov.trak);
			for(loopcnt = 0; loopcnt < szList; loopcnt++)
			{
				/* get tracks */
				dlllist= UTIL_DLLIST_Get_n_Node(&tInitSegment.moov.trak, loopcnt);
				UTIL_MP4_PARSER_TRAK_t* trakbox = dlllist->pData;
				szSubList = UTIL_DLLIST_Size(&trakbox->mdia.minf.stbl.stsd.codecInfo);
				for(subloopcnt = 0; subloopcnt < szSubList; subloopcnt++)
				{
					/* get codec info */
					subdlllist = UTIL_DLLIST_Get_n_Node(&trakbox->mdia.minf.stbl.stsd.codecInfo, subloopcnt);
					UTIL_MP4_CODEC_t* codec = subdlllist->pData;
					if(*codec == UTIL_MP4_CODEC_AVC1)
					{
						PrintDebug("%s(%d) codec is AVC1\n",__FUNCTION__,__LINE__);
						ullTimeScale = trakbox->mdia.mdhd.timescale;
						ullStartTime = pItem->ullStarttime / DASH_CTRL_GetTimeScale(pController) * ullTimeScale;
						write_bhed_box(pMDASH->mp4box, ullTimeScale, ullStartTime, (char*)aFourCC);
						/* get entry's NAL units */
						entrylist = UTIL_DLLIST_Get_n_Node(&trakbox->mdia.minf.stbl.stsd.entry, subloopcnt);
						UTIL_MP4_PARSER_AVC1_t* avc = entrylist->pData;
						codecPrivData[nPos++] = 0x00;
						codecPrivData[nPos++] = 0x00;
						codecPrivData[nPos++] = 0x00;
						codecPrivData[nPos++] = 0x01;
						nCodecPrivateDataLen += 4;
						nCodecPrivateDataLen += avc->avcc.SPSlen;
						for(NALloopcnt = 0; NALloopcnt < avc->avcc.SPSlen; NALloopcnt++)
						{
							nallist = UTIL_DLLIST_Get_n_Node(&avc->avcc.SPSNAL, NALloopcnt);
							codecPrivData[nPos+NALloopcnt] = *((HUINT8*)nallist->pData);
						}
						nPos = nPos+NALloopcnt;
						codecPrivData[nPos++] = 0x00;
						codecPrivData[nPos++] = 0x00;
						codecPrivData[nPos++] = 0x00;
						codecPrivData[nPos++] = 0x01;
						nCodecPrivateDataLen += 4;
						nCodecPrivateDataLen += avc->avcc.PPSlen;
						for(NALloopcnt = 0; NALloopcnt < avc->avcc.PPSlen; NALloopcnt++)
						{
							nallist = UTIL_DLLIST_Get_n_Node(&avc->avcc.PPSNAL, NALloopcnt);
							codecPrivData[nPos+NALloopcnt] = *((HUINT8*)nallist->pData);
						}

						//현재 track과 관련된 trex 박스 정보를 가져온다
						for(HUINT32 trexcnt = 0; trexcnt < UTIL_DLLIST_Size(&tInitSegment.moov.mvex.trex); trexcnt++)
						{
							UTIL_DLLNode_t* dllist = UTIL_DLLIST_Get_n_Node(&tInitSegment.moov.mvex.trex, trexcnt);
							trex = dllist->pData;
							if(trex->trackID == trakbox->tkhd.track_ID)
							{
								bGotTrex = TRUE;
							}
						}
					}
				}
			}
			write_bdat_box(pMDASH->mp4box, (unsigned char*)codecPrivData, nCodecPrivateDataLen);
			/* if trex box exists, write trex box information with default sample value */
			if(bGotTrex == TRUE)
			{
				write_trex_box(pMDASH->mp4box, ulIndex, trex->default_sample_description_index,
								trex->default_sample_duration, trex->default_sample_size,
								trex->default_sample_flags);
			}
			else if(bGotTrex == FALSE)
			{
				write_trex_box(pMDASH->mp4box, ulIndex, 0, 0, 0, 0);
			}
			close_mp4_box(pMDASH->mp4box, pMDASH->mp4box->length + pItem->ullSegmentsize);
		}

		/* Get AAC codec info from parsed MP4 data */
		if(eAudioCodec == DI_AUDFORMAT_AAC)
		{
			HINT32 nProfile = 2; /* Actually 2 */
			HINT32 nSamplingRateIndex = -1;
			HINT32 nChannelsIndex = -1;
			HUINT32 ulSamplingRate = 0;
			HUINT32 ulChannels = 0;
			/* get AAC codec info from parsed MP4 data */
			szRead = P_STREAM_DASH_MP4CACHE_Read(pMDASH->InitData, &tInitSegment, pItem->nBandwidth, pItem->eStreamType);
			if(szRead == 0)
			{
				PrintDebug("%s(%d) Initialisation segment data was not found in cache\n", __FUNCTION__, __LINE__);
				PrintExit;
				return 0;
			}
			szList = UTIL_DLLIST_Size(&tInitSegment.moov.trak);
			for(loopcnt = 0; loopcnt < szList; loopcnt++)
			{
				dlllist= UTIL_DLLIST_Get_n_Node(&tInitSegment.moov.trak, loopcnt);
				UTIL_MP4_PARSER_TRAK_t* trakbox = dlllist->pData;
				szSubList = UTIL_DLLIST_Size(&trakbox->mdia.minf.stbl.stsd.codecInfo);
				for(subloopcnt = 0; subloopcnt < szSubList; subloopcnt++)
				{
					subdlllist = UTIL_DLLIST_Get_n_Node(&trakbox->mdia.minf.stbl.stsd.codecInfo, subloopcnt);
					UTIL_MP4_CODEC_t*codec = subdlllist->pData;
					if(*codec == UTIL_MP4_CODEC_MP4A)
					{
						/* convert scaled start time by dash to scaled start time by mp4 */
						PrintDebug("%s(%d) codec is MP4A\n",__FUNCTION__,__LINE__);
						ullTimeScale = trakbox->mdia.mdhd.timescale;
						ullStartTime = pItem->ullStarttime / DASH_CTRL_GetTimeScale(pController) * ullTimeScale;
						write_bhed_box(pMDASH->mp4box, ullTimeScale, ullStartTime, (char*)aFourCC);
						entrylist = UTIL_DLLIST_Get_n_Node(&trakbox->mdia.minf.stbl.stsd.entry, subloopcnt);
						UTIL_MP4_PARSER_MP4A_t* mp4a = entrylist->pData;
						ulSamplingRate = mp4a->time_scale;
						ulChannels = mp4a->channel_count;

						/* get 'trex' info related to current track */
						for(HUINT32 trexcnt = 0; trexcnt < UTIL_DLLIST_Size(&tInitSegment.moov.mvex.trex); trexcnt++)
						{
							UTIL_DLLNode_t* dllist = UTIL_DLLIST_Get_n_Node(&tInitSegment.moov.mvex.trex, trexcnt);
							trex = dllist->pData;
							if(trex->trackID == trakbox->tkhd.track_ID)
							{
								bGotTrex = TRUE;
							}
						}
					}
				}
			}

			for(unsigned int i = 0; i < sizeof(b_aac_adts_sample_rate)/sizeof(*b_aac_adts_sample_rate); i++)
			{
				if (b_aac_adts_sample_rate[i] == ulSamplingRate)
				{
					nSamplingRateIndex = i;
					break;
				}
			}
			for(unsigned int i = 0; i < sizeof(b_aac_adts_channels)/sizeof(*b_aac_adts_channels); i++)
			{
				if (b_aac_adts_channels[i] == ulChannels)
				{
					nChannelsIndex = i;
					break;
				}
			}
			if(nSamplingRateIndex >= 0 && nChannelsIndex >= 0)
			{
				ulCodecPrivateData = ((nProfile << 28) | (nSamplingRateIndex << 24) | (nChannelsIndex << 20));
				ulCodecPrivateData = ulCodecPrivateData >> 1;
				uint2array((char*)codecPrivData,ulCodecPrivateData);
			}
			nCodecPrivateDataLen = 3;
			write_bdat_box(pMDASH->mp4box, (unsigned char*)codecPrivData, nCodecPrivateDataLen);
			if(bGotTrex == TRUE)
			{
				write_trex_box(pMDASH->mp4box, ulIndex, trex->default_sample_description_index,
								trex->default_sample_duration, trex->default_sample_size,
								trex->default_sample_flags);
			}
			else if(bGotTrex == FALSE)
			{
				write_trex_box(pMDASH->mp4box, ulIndex, 0, 0, 0, 0);
			}

			close_mp4_box(pMDASH->mp4box, pMDASH->mp4box->length + pItem->ullSegmentsize);
		}
	}

	pMDASH->mp4box->bBoxing = TRUE;

	PrintExit;
	return pMDASH->mp4box->length;
}

/**
 * @b Function @b Description <br>
 * convert [segment information] in MPD Parser to [segment item] in preconnector <br>
 *
 * @param[in] pSegmentInfo	: segment information
 * @param[out] pItem : segment item
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_ConvertSegmentInfoToItem(DASH_SEGMENTINFO_t* pSegmentInfo, DASH_PRECONNECTOR_ITEM_t* pItem)
{
	HUINT32 szString = 0;

	pItem->nID = 0;
	pItem->ullStartOffset = pSegmentInfo->startoffset;
	pItem->ullEndOffset = pSegmentInfo->endoffset;
	pItem->ullStarttime = pSegmentInfo->llSegmentStarttime;
	pItem->ullDuration = pSegmentInfo->llSegmentDuration;
	pItem->eSegmentType = pSegmentInfo->eSegmentType;
	pItem->ullRequestedPos = pSegmentInfo->llSegmentSeektime;
	pItem->bIsLastSegment = pSegmentInfo->bIsLastSegment;
	pItem->eCodec = pSegmentInfo->eCodec;
	pItem->eStreamType = pSegmentInfo->eTrackType;
	pItem->nBandwidth = pSegmentInfo->nBandwidth;

	if(pItem->bIsLastSegment != TRUE)
	{
		szString = VK_strlen(pSegmentInfo->pucSegmentURL)+1;
		if(pItem->pSegBuffer != NULL)
		{
			VK_MEM_Free(pItem->pSegBuffer);
			pItem->pSegBuffer = NULL;
		}
		if(pItem->pucURL != NULL)
		{
			VK_MEM_Free(pItem->pucURL);
			pItem->pucURL = NULL;
		}
		pItem->pucURL = VK_MEM_Alloc(szString);
		if(pItem->pucURL == NULL)
		{
			PrintError("%s(%d) Error : Not enough memory\n", __FUNCTION__, __LINE__);
			return DASH_ERR_ERROR;
		}
		VK_MEM_Memset(pItem->pucURL, 0, szString);
		VK_strncpy(pItem->pucURL, pSegmentInfo->pucSegmentURL, szString);
	}

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Allocate stream structure <br>
 *
 * @param[out] ppAccess	: pointer which stream will be allocated
 * @param[in] pucURL : destination URL
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_AllocStream(STREAM_ACCESS_t** ppAccess, HINT8* pucURL)
{
	HUINT32 szURL = 0;
	STREAM_ACCESS_t* pAccess = NULL;

	szURL = VK_strlen(pucURL)+1;
	if(pAccess != NULL)
	{
		VK_MEM_Free(pAccess);
		pAccess = NULL;
	}
	pAccess = VK_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		return DASH_ERR_ERROR;
	}
	VK_MEM_Memset(pAccess, 0x0, sizeof(STREAM_ACCESS_t));
	pAccess->psz_path = VK_MEM_Alloc(szURL);
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		return DASH_ERR_ERROR;
	}
	VK_MEM_Memset(pAccess->psz_path, 0x0, szURL);
	VK_strncpy(pAccess->psz_path, pucURL, szURL);
	*ppAccess = pAccess;

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Release stream structure <br>
 *
 * @param[in] pAccess	: pointer which will be released
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_ReleaseStream(STREAM_ACCESS_t* pAccess)
{
	if(pAccess != NULL)
	{
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}
		(void)STREAM_HTTP_Close(pAccess);
		VK_MEM_Free(pAccess);
		pAccess = NULL;
	}
	else
	{
		PrintError("%s(%d) pAccess is NULL\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Simple Download function <br>
 * Download small file such as MPD, Initialisation segment, Index segment <br>
 *
 * @param[in] pUrl	: file URL
 * @param[out] pBuf : buffer pointer
 * @param[in] startoffset: In case of range access, this is start offset( if whole file access, this is 0)
 * @param[in] endoffset : In case of range access, this is end offset(if whole file access, this is 0)
 *
 * @return  #  downloaded size <br>
 */
static HUINT64 P_STREAM_DASH_Download(HINT8 *pUrl, HINT8 **pBuf, off_t startoffset, off_t endoffset)
{
	HINT32 szTry = 0;
	HINT64 nContentLength = 0;
	HINT8  *pRecvBuf = NULL;
	HINT32 nRecvLength = 0;
	HINT64 nSumLength = 0;
	HINT32 nPos = 0;
	DASH_ERROR_e derr;
	STREAM_ACCESS_t *pAccess = NULL;

	if(pUrl == NULL)
	{
		PrintError ("%s (%d) Error>  Url is NULL \n",__FUNCTION__,__LINE__);
		return 0;
	}
	else if (NULL == pBuf)
	{
		PrintError ("%s (%d) Error>  Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	else
	{
		derr = P_STREAM_DASH_AllocStream(&pAccess, pUrl);
		if(derr != DASH_ERR_OK)
		{
			PrintError ("%s (%d) Error>  Cannot Allocate Stream access\n",__FUNCTION__,__LINE__);
			return 0;
		}
		/* if offset is not defined, connect to 0 */
		if(startoffset == 0 && endoffset == 0)
		{
			if(DI_IPT_SUCCESS != STREAM_HTTP_Open(pAccess))
			{
				PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				goto __download_error;
			}
		}
		else
		{
			if(DI_IPT_SUCCESS != STREAM_HTTP_Open(pAccess))
			{
				PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				goto __download_error;
			}
			if(DI_IPT_SUCCESS != STREAM_HTTP_Seek(pAccess, startoffset))
			{
				PrintError("%s(%d) : Error> STREAM_HTTP_Seek Fail.\n",__FUNCTION__,__LINE__);
				goto __download_error;
			}
		}
	}
	if((startoffset == 0 && endoffset == 0) || (endoffset > pAccess->info.i_size))
	{
		nContentLength = pAccess->info.i_size;	/* whole segment access */
	}
	else
	{
		nContentLength = endoffset - startoffset; /* byte range access */
	}
	PrintDebug ("%s(%d): Open %s\n",__FUNCTION__,__LINE__,pUrl);
	PrintDebug ("%s(%d): nContentLength (%lld)bytes \n",__FUNCTION__,__LINE__,nContentLength);
	if(nContentLength <= 0)
	{
		return nContentLength;
	}
	*pBuf = (HINT8*) VK_MEM_Alloc(nContentLength);
	if(*pBuf == NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%"PRId64") !!\n",__FUNCTION__,__LINE__,nContentLength);
		goto __download_error;
	}
	VK_MEM_Memset(*pBuf, 0, nContentLength);
	pRecvBuf = (HINT8*) VK_MEM_Alloc(DASH_TEMPORAL_RECV_BUFFER_SIZE);
	if(pRecvBuf == NULL)
	{
		PrintError("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !! \n", __FUNCTION__, __LINE__,DASH_TEMPORAL_RECV_BUFFER_SIZE);
		goto __download_error;
	}
	if(nContentLength < DASH_TEMPORAL_RECV_BUFFER_SIZE)
	{
		szTry = nContentLength;
	}
	else
	{
		szTry = DASH_TEMPORAL_RECV_BUFFER_SIZE;
	}
	do
	{
		nRecvLength = 0;
		nRecvLength = STREAM_HTTP_Read(pAccess, (HUINT8 *)pRecvBuf, szTry);
		if(nRecvLength <= 0)
		{
			/* maximum attemption is defined out of here */
			break;
		}
		else
		{
			if(nContentLength < nPos + nRecvLength)
			{
				PrintError ("%s(%d) : Error > BufferOverFlow !!Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nRecvLength);
				VK_MEM_Memcpy(*pBuf+nPos, pRecvBuf, (nPos+nRecvLength) - nContentLength);
				break;
			}
			else
			{
				VK_MEM_Memcpy(*pBuf+nPos, pRecvBuf, nRecvLength);
				nPos+=nRecvLength;
				nSumLength+=nRecvLength;
			}
		}
	}
	while(nSumLength < nContentLength);

	if(pRecvBuf != NULL)
	{
		VK_MEM_Free(pRecvBuf);
		pRecvBuf = NULL;
	}
	P_STREAM_DASH_ReleaseStream(pAccess);
	nContentLength = nSumLength;
	return nContentLength;
__download_error:
	if(*pBuf != NULL)
	{
		VK_MEM_Free(*pBuf);
		*pBuf = NULL;
	}
	if(pRecvBuf != NULL)
	{
		VK_MEM_Free(pRecvBuf);
		pRecvBuf = NULL;
	}
	P_STREAM_DASH_ReleaseStream(pAccess);
	return 0;

}

/**
 * @b Function @b Description <br>
 * download xlinks in MPD <br>
 * This is a callback function used by MPD parser <br>
 *
 * @param[in] xlink	: xlink list in each layer
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_XLINK_Download(DASH_XLINK_LIST_t* xlink)
{
	DASH_AdaptationSet_t* adaptation = NULL;
	DASH_Period_t* period = NULL;
	DASH_SegmentList_t* segmentlist = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32 nradaptation;
	HUINT32 nrperiod;
	HUINT32 nrsegmentlist;
	HINT8* downloadbuf = NULL;
	HUINT32	nContentLength;
	HUINT32 unRetry = 0;
	void* parsedXML = NULL;
	void* xmlroot = NULL;

#if 0
	VK_printf("xlinks in adaptation : %d %x\n",(unsigned int*)UTIL_DLLIST_Size(&xlink->adaptations), &xlink->adaptations);
	VK_printf("xlinks in period : %d %x\n",(unsigned int*)UTIL_DLLIST_Size(&xlink->periods), &xlink->periods);
	VK_printf("xlinks in segmentlist : %d%x\n",(unsigned int*)UTIL_DLLIST_Size(&xlink->segmentlist), &xlink->segmentlist);
#endif

	for(nradaptation = 0; nradaptation < UTIL_DLLIST_Size(&xlink->adaptations); nradaptation++)
	{
		dllnode = UTIL_DLLIST_ExtractHead(&xlink->adaptations);
		adaptation = dllnode->pData;
		dllnode->pData = NULL;

		/* try to download MPD */
		while((nContentLength = P_STREAM_DASH_Download((void *)adaptation->xlink.href.str ,&downloadbuf, 0, 0)) <= 0)
		{
			PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Retrying...\n",__FUNCTION__,__LINE__);
			if(unRetry > DASH_MAXIMUM_CONNECT_TRY)
			{
				PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Exit...\n",__FUNCTION__,__LINE__);
				return DASH_ERR_ERROR;
			}
			unRetry++;
		}
		parsedXML = (void *)xmlParseMemory((const char*)downloadbuf, nContentLength);
		if( parsedXML == NULL)
		{
			PrintError("%s(%d) : Error> XML parsing error...\n",__FUNCTION__,__LINE__);
			return DASH_ERR_ERROR;
		}
		xmlroot = (void*)xmlDocGetRootElement(parsedXML);
		if( xmlroot == NULL)
		{
			return DASH_ERR_ERROR;
		}
		if(DASH_MPD_ERR_PARSE_ERROR == DASH_MPD_Parse_AdaptationSet(xmlroot, adaptation))
		{
			return DASH_ERR_ERROR;
		}

		if(downloadbuf != NULL)
		{
			VK_MEM_Free(downloadbuf);
			downloadbuf = NULL;
		}
		xmlFreeDoc(parsedXML);
		unRetry = 0;
	}
	UTIL_DLLIST_Reset(&xlink->adaptations);
	for(nrperiod = 0; nrperiod < UTIL_DLLIST_Size(&xlink->periods); nrperiod++)
	{
		dllnode = UTIL_DLLIST_ExtractHead(&xlink->periods);
		period = dllnode->pData;
		dllnode->pData = NULL;

		/* try to download MPD */
		while((nContentLength = P_STREAM_DASH_Download((void *)period->xlink.href.str ,&downloadbuf, 0, 0)) <= 0)
		{
			PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Retrying...\n",__FUNCTION__,__LINE__);
			if(unRetry > DASH_MAXIMUM_CONNECT_TRY)
			{
				PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Exit...\n",__FUNCTION__,__LINE__);
				return DASH_ERR_ERROR;
			}
			unRetry++;
		}
		parsedXML = (void *)xmlParseMemory((const char*)downloadbuf, nContentLength);
		if( parsedXML == NULL)
		{
			PrintError("%s(%d) : Error> XML parsing error...\n",__FUNCTION__,__LINE__);
			return DASH_ERR_ERROR;
		}
		xmlroot = (void*)xmlDocGetRootElement(parsedXML);
		if( xmlroot == NULL)
		{
			PrintError("%s(%d) : Error> XML root is NULL...\n",__FUNCTION__,__LINE__);
			return DASH_ERR_ERROR;
		}
		DASH_MPD_Parse_Period(xmlroot, period);

		if(downloadbuf != NULL)
		{
			VK_MEM_Free(downloadbuf);
			downloadbuf = NULL;
		}
		xmlFreeDoc(parsedXML);
		unRetry = 0;
	}
	UTIL_DLLIST_Reset(&xlink->periods);
	for(nrsegmentlist = 0; nrsegmentlist < UTIL_DLLIST_Size(&xlink->segmentlist); nrsegmentlist++)
	{
		dllnode = UTIL_DLLIST_ExtractHead(&xlink->segmentlist);
		segmentlist = dllnode->pData;
		dllnode->pData = NULL;
		/* try to download MPD */
		while((nContentLength = P_STREAM_DASH_Download((void *)segmentlist->xlink.href.str ,&downloadbuf, 0, 0)) <= 0)
		{
			PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Retrying...\n",__FUNCTION__,__LINE__);
			if(unRetry > DASH_MAXIMUM_CONNECT_TRY)
			{
				PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Exit...\n",__FUNCTION__,__LINE__);
				return DASH_ERR_ERROR;
			}
			unRetry++;
		}
		parsedXML = (void *)xmlParseMemory((const char*)downloadbuf, nContentLength);
		if( parsedXML == NULL)
		{
			PrintError("%s(%d) : Error> XML parsing error...\n",__FUNCTION__,__LINE__);
			return DASH_ERR_ERROR;
		}
		xmlroot = (void*)xmlDocGetRootElement(parsedXML);
		if( xmlroot == NULL)
		{
			return DASH_ERR_ERROR;
		}
		DASH_MPD_Parse_SegmentList(xmlroot, segmentlist);

		if(downloadbuf != NULL)
		{
			VK_MEM_Free(downloadbuf);
			downloadbuf = NULL;
		}
		xmlFreeDoc(parsedXML);
		unRetry = 0;
	}
	UTIL_DLLIST_Reset(&xlink->segmentlist);
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Download media segment <br>
 *
 * @param[in] pHSession	: Session handle
 * @param[in] item	: Preconnector item
 * @param[out] szDownload	: size of downloaded segment
 * @param[out] szSpendTime	: turnaround time while downloading
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_Segment_Download(SESSION_HANDLE pHSession, DASH_PRECONNECTOR_ITEM_t* item, HUINT32* szDownload, HULONG* szSpendTime)
{
	DI_IPTUNER_t* pHTuner;
	STREAM_ACCESS_t *pAccess;
	DASH_BUFFERCMD_e buffercmd;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_SEGMENT_FORMAT_e eSegFormat = DASH_SEGMENT_UNKNOWN;
	ADP_BUFFERMGR_STATUS_t tBufferStat;
	ADP_BUFFER_UserData_t segInfo;
	ADP_BUFFER_ERR berr = ADP_BUFFER_OK;
	DASH_ERROR_e derr = DASH_ERR_OK;
	HUINT32 unRet;
	HINT32 	nReadSize = 0;
	HINT32	nWriteSize = 0;
	HINT32  szBoxSize = 0;
	HULONG 	unStartTick;
	HULONG 	unEndTick;
	HUINT32 unRetry = 0;
	size_t 	szRemaining = 0;
	size_t 	szTry = 0;
	size_t 	szReal = 0;
	size_t 	szRealTotal = 0;
	HUINT64 szWritten = 0;

	PrintEnter;

	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> DASH HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	if(item->pucURL == NULL)
	{
		PrintError("%s(%d) Error> URL is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	/* get handles */
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid DASH handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	derr = DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) Invalid buffer manager handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) Invalid controller handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	if(item->pAccess == NULL)
	{
		PrintError("%s(%d) The item was not preconnected! (pAccess is NULL)\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	pAccess = (STREAM_ACCESS_t*)item->pAccess;

	if(pHTuner->bExitDownload == TRUE)
	{
		PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DASH_ERR_STOP_FORCE;
	}

	/* fill user data */
	VK_MEM_Memset(&segInfo, 0, sizeof(ADP_BUFFER_UserData_t));
	segInfo.ulSegId = (HUINT64)item->ullStarttime;	/* starttime is also used as segment id ! */
	segInfo.ullStartTime = item->ullStarttime;
	segInfo.ullDuration = item->ullDuration;
	//segInfo.ulCategory = item->eSegmentType;
	segInfo.ulCategory = item->eStreamType;
	segInfo.ullSegmentSize = item->ullSegmentsize;

	/******************************************
	*       1. Open buffer manager
	******************************************/
	ADP_BUFFERMGR_GetStatus(pBufferMgr, &tBufferStat);
	do
	{
		berr = ADP_BUFFERMGR_Open(pBufferMgr, &segInfo);
		if(berr == ADP_BUFFER_NOFREEBLOCK)
		{
			ADP_BUFFERMGR_GetStatus(pBufferMgr, &tBufferStat);
#if 0
			VK_printf("total block cnt (%d)\n", tBufferStat.ulTotalBlocks);
			VK_printf("used block cnt (%d)\n", tBufferStat.ulUsedBlocks);
			VK_printf("free block cnt (%d)\n", tBufferStat.ulFreedBlocks);
			VK_printf("before ratain : %"PRId64"\n", tBufferStat.ullFreedSize);
#endif
			if(tBufferStat.ulTotalBlocks * ((double)DASH_RETAIN_THRESHOLD / (double)100) < tBufferStat.ulUsedBlocks)
			{
				ADP_BUFFERMGR_SimpleRetainBlock(pBufferMgr, tBufferStat.ulTotalBlocks * (double)DASH_RETAIN_BLOCK_AT_ONCE / (double)100);
			}
			/* buffercmd reaction while reclaiming */
			buffercmd = P_STREAM_DASH_GetBufferCommand(pMDASH);
			if(buffercmd == DASH_BUFFERCMD_QUIT)
			{
				PrintError("%s(%d) DASH_BUFFERCMD_QUIT\n", __FUNCTION__, __LINE__);
				PrintExit;
				return DASH_ERR_STOP_BY_QUITCMD;
			}
			else if(buffercmd == DASH_BUFFERCMD_FLUSH)
			{
				PrintDebug("%s(%d) Break Buffering because Flush has been occured\n", __FUNCTION__, __LINE__);
				PrintExit;
				return DASH_ERR_STOP_BY_FLUSH;
			}
			else if(buffercmd == DASH_BUFFERCMD_PAUSE)
			{
				continue;
			}
			else if(pHTuner->bExitDownload == TRUE || buffercmd == DASH_BUFFERCMD_STOP)
			{
				PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				PrintExit;
				return DASH_ERR_STOP_FORCE;
			}
			VK_TASK_Sleep(DASH_BUFFERING_TASK_SLEEP);
		}
	}
	while (berr != ADP_BUFFER_OK);

#if 1
	PrintDebug("%s(%d) --------------[Download info]-------------------\n", __FUNCTION__, __LINE__);
	PrintDebug("%s(%d) item->ullSegmentsize : %"PRId64"\n", __FUNCTION__, __LINE__, item->ullSegmentsize);
	PrintDebug("%s(%d) item->ullStartOffset : %"PRId64"\n", __FUNCTION__, __LINE__, item->ullStartOffset);
	PrintDebug("%s(%d) item->ullEndOffset : %"PRId64"\n", __FUNCTION__, __LINE__, item->ullEndOffset);
	PrintDebug("%s(%d) item->ullDuration : %"PRId64"(%"PRId64")\n", __FUNCTION__, __LINE__, item->ullDuration,
							DASH_CTRL_ConvertNonScaledTimetoMs(pController, item->ullDuration));
	PrintDebug("%s(%d) item->ullRequestedPos : %"PRId64"(%"PRId64")\n", __FUNCTION__, __LINE__, item->ullRequestedPos,
							DASH_CTRL_ConvertNonScaledTimetoMs(pController, item->ullRequestedPos));
	PrintDebug("%s(%d) item->ullStarttime : %"PRId64"(%"PRId64")\n", __FUNCTION__, __LINE__, item->ullStarttime,
							DASH_CTRL_ConvertNonScaledTimetoMs(pController, item->ullStarttime));
	PrintDebug("%s(%d) item->SegmentType : %d\n", __FUNCTION__, __LINE__, item->eSegmentType);
	PrintDebug("%s(%d) item->StreamType : %d\n", __FUNCTION__, __LINE__, item->eStreamType);
	PrintDebug("%s(%d) item->Bandwidth : %d\n", __FUNCTION__, __LINE__, item->nBandwidth);
	PrintDebug("%s(%d) ------------------------------------------------\n",__FUNCTION__, __LINE__);
#endif

	/******************************************
	*  2. Initializaing temporal buffer
	******************************************/
	if(item->ullEndOffset == 0ULL)
	{
		szRemaining = item->ullSegmentsize;
	}
	else
	{
		/* calculate the size of segment with seek position */
		szRemaining = item->ullEndOffset - item->ullStartOffset;
		szRemaining = szRemaining - item->ullSegmentsize * ((item->ullRequestedPos - item->ullStarttime)/(double)item->ullDuration);
	}
	if(item->pSegBuffer == NULL)
	{
		item->pSegBuffer = VK_MEM_Alloc(DASH_SEGMENT_RECV_SIZE);
		if(item->pSegBuffer == NULL)
		{
			PrintError("%s(%d) Not enough memory\n", __FUNCTION__, __LINE__);
			return DASH_ERR_ERROR;
		}
	}
	ADP_BUFFERMGR_GetStatus(pBufferMgr, &tBufferStat);
	PrintDebug("%s(%d) Download start(%s)\n",__FUNCTION__, __LINE__, item->pucURL);
	eSegFormat = DASH_CTRL_GetSegmentFormat(pController);
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

	/******************************************
	*     3. Make BRCM specific MP4 Box and write to buffer manager
	*		(if media format is MP4)
	******************************************/
	if(eSegFormat == DASH_SEGMENT_MP4)
	{
		szWritten = 0;
		P_STREAM_DASH_ClearBox(pMDASH);
		szBoxSize = P_STREAM_DASH_MakeBox(pMDASH, item);
		if(szBoxSize > 0)
		{
			while(szBoxSize > 0)
			{
				if(pHTuner->bExitDownload == TRUE)
				{
					PrintDebug("%s(%d) Break Loop(Box Write)\n",__FUNCTION__,__LINE__);
					break;
				}
				nWriteSize = ADP_BUFFERMGR_Write(pBufferMgr, pMDASH->mp4box->buf + szWritten , szBoxSize);
				if(nWriteSize == 0)
				{
					VK_TASK_Sleep(100);
					continue;
				}
				szWritten += nWriteSize;
				szBoxSize -= nWriteSize;
			}
		}
		else
		{
			PrintError("%s(%d) Make HEADER BOX Fail\n",__FUNCTION__,__LINE__);
			return DASH_ERR_ERROR;
		}
	}

	/******************************************
	*     4. Segment download (60KB at once)
	******************************************/
	do  /* while(szRemaining > 0) */
	{
		if(szRemaining > DASH_SEGMENT_RECV_SIZE)
		{
			szTry = DASH_SEGMENT_RECV_SIZE;
		}
		else
		{
			szTry = szRemaining;
		}
		szReal = 0;
		szWritten = 0;
		do /* while(szReal < szTry)*/
		{
			/* scheduler에서 seek을 체크하여 flush를 전달함. 별도의 seek 판단은 필요없음 */
			buffercmd = P_STREAM_DASH_GetBufferCommand(pMDASH);
			if(buffercmd == DASH_BUFFERCMD_QUIT)
			{
				PrintError("%s(%d) DASH_BUFFERCMD_QUIT\n", __FUNCTION__, __LINE__);
				ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
				ADP_BUFFERMGR_Close(pBufferMgr);
				PrintExit;
				return DASH_ERR_STOP_BY_QUITCMD;
			}
			else if(buffercmd == DASH_BUFFERCMD_FLUSH)
			{
				PrintDebug("%s(%d) Break buffering >>> flush has been occured\n", __FUNCTION__, __LINE__);
				ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
				ADP_BUFFERMGR_Close(pBufferMgr);
				PrintExit;
				return DASH_ERR_STOP_BY_FLUSH;
			}
			else if(pHTuner->bExitDownload == TRUE || buffercmd == DASH_BUFFERCMD_STOP)
			{
				PrintDebug("%s(%d) Break buffering\n",__FUNCTION__,__LINE__);
				ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
				ADP_BUFFERMGR_Close(pBufferMgr);
				PrintExit;
				return DASH_ERR_STOP_FORCE;
			}
			VK_MEM_Memset(item->pSegBuffer, 0, DASH_SEGMENT_RECV_SIZE);
			nReadSize = STREAM_HTTP_Read(pAccess, (HUINT8*)item->pSegBuffer, szTry-szReal);
			if(nReadSize <= 0 || nReadSize > (HINT32)szTry-(HINT32)szReal)
			{
				if(unRetry >= DASH_MAXIMUM_CONNECT_TRY)
				{
					PrintError ("%s(%d) Stop!! STREAM_HTTP_Read Fail !!!\n",__FUNCTION__,__LINE__);
					ADP_BUFFERMGR_Close(pBufferMgr);
					PrintExit;
					return DASH_ERR_EXCEED_MAXIMUM_CONNECT_TRY;
				}
				PrintError ("%s(%d) STREAM_HTTP_Read Fail Retry...!!! \n",__FUNCTION__,__LINE__);
				unRetry++;
				continue;
		 	}
			//PrintDebug("Downloaded : %d\n", nReadSize);
			if(pDASH->bDump == TRUE)
			{
				FILE *fp = fopen("/download_dump.bin","a+");
				if(fp==NULL) exit(1);
				fwrite((void*)item->pSegBuffer, sizeof(HINT8), nReadSize, fp);
				VK_fflush(fp);
				fclose(fp);
			}
			do
			{
				/* write partial data into buffer */
				szWritten += ADP_BUFFERMGR_Write(pBufferMgr, (HUINT8*)(item->pSegBuffer+szWritten), nReadSize-szWritten);
				/* check buffer */
				if(nReadSize != (HINT32)szWritten)
				{
					ADP_BUFFERMGR_GetStatus(pBufferMgr, &tBufferStat);
#if 0
					VK_printf("total block cnt (%d)\n", tBufferStat.ulTotalBlocks);
					VK_printf("used block cnt (%d)\n", tBufferStat.ulUsedBlocks);
					VK_printf("free block cnt (%d)\n", tBufferStat.ulFreedBlocks);
					VK_printf("before ratain : %"PRId64"\n", tBufferStat.ullFreedSize);
#endif
					if(tBufferStat.ulTotalBlocks * ((double)DASH_RETAIN_THRESHOLD / (double)100) < tBufferStat.ulUsedBlocks)
					{
						unRet = ADP_BUFFERMGR_SimpleRetainBlock(pBufferMgr, tBufferStat.ulTotalBlocks * (double)DASH_RETAIN_BLOCK_AT_ONCE / (double)100);
					}
					UNUSED_PARAM(unRet);
					ADP_BUFFERMGR_GetStatus(pBufferMgr, &tBufferStat);
					buffercmd = P_STREAM_DASH_GetBufferCommand(pMDASH);
					if(buffercmd == DASH_BUFFERCMD_QUIT)
					{
						PrintError("%s(%d) DASH_BUFFERCMD_QUIT\n", __FUNCTION__, __LINE__);
						ADP_BUFFERMGR_Close(pBufferMgr);
						PrintExit;
						return DASH_ERR_STOP_BY_QUITCMD;
					}
					else if(buffercmd == DASH_BUFFERCMD_FLUSH)
					{
						PrintDebug("%s(%d) Break buffering because >>> flush has been occured\n", __FUNCTION__, __LINE__);
						ADP_BUFFERMGR_Close(pBufferMgr);
						PrintExit;
						return DASH_ERR_STOP_BY_FLUSH;
					}
					else if(pHTuner->bExitDownload == TRUE || buffercmd == DASH_BUFFERCMD_STOP)
					{
						PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
						ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
						ADP_BUFFERMGR_Close(pBufferMgr);
						PrintExit;
						return DASH_ERR_STOP_FORCE;
					}
				}
			}
			while(nReadSize != (HINT32)szWritten);

			szReal += nReadSize;
			szRealTotal += nReadSize;
			szWritten = 0;
			nReadSize = 0;

			if(szReal < szTry)
			{
				VK_TASK_Sleep(DASH_BUFFERING_TASK_SLEEP);
			}
		}
		while(szReal < szTry);
		szRemaining = szRemaining - szReal;
	}
	while(szRemaining > 0);

	unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	*szSpendTime = unEndTick - unStartTick;
	if(*szSpendTime == 0)
	{
		*szSpendTime = 10;
	}
	PrintDebug("%s(%d) Download complete(%s, %d)\n",__FUNCTION__, __LINE__, item->pucURL, szRealTotal);
	*szDownload = szRealTotal;
	ADP_BUFFERMGR_Close(pBufferMgr);

	PrintExit;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Userevent processor <br>
 * Consume main event and set additional process flag(eventResult) <br>
 * additional flag will be used in scheduler and DI_STREAM_Control <br>
 *
 * @param[in] pDASH	: DASH handle
 * @param[in] pController	: Controller handle
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_UserEventProcess(MEDIA_DASH_HANDLE_t pMDASH, DASH_CONTROLLER_HANDLE_t pController)
{
	HINT64	ullSeekPos = 0ULL;
	HUINT64 requestOffset = 0ULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
#if 0
	HUINT64 ullPrevAudioConnectTime = 0ULL;
	HUINT64 ullPrevVideoConnectTime = 0ULL;
#endif
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	ADP_BUFFER_UserData_t pUserVideoData;
	ADP_BUFFER_UserData_t pUserAudioData;
	ADP_BUFFER_ERR berrVideo = ADP_BUFFER_OK;
	ADP_BUFFER_ERR berrAudio = ADP_BUFFER_OK;
	DASH_ERROR_e errVideo = DASH_ERR_OK;
	DASH_ERROR_e errAudio = DASH_ERR_OK;
	HUINT32 unVideoBlockId = 0;
	HUINT32 unAudioBlockId = 0;
	HUINT64 ullVideoBlockOffset = 0ULL;
	HUINT64 ullAudioBlockOffset = 0ULL;
	DASH_ERROR_e ret = DASH_ERR_OK;
	DASH_SEGMENTINFO_t tSegInfo;
	DASH_SEGMENTINFO_t tAudSegInfo;
	pDASH = pMDASH->hDASH;
	DASH_USER_EVENT_e evt = DASH_MAIN_GetOccuredUserEvent(pDASH);
	/* when seek is occured */
	if(evt != DASH_USER_EVENT_NONE)
	{
		switch(evt)
		{
			case DASH_USER_EVENT_SEEK:
				/* Getting segment info based on seek pos */
				VK_MEM_Memset(&tSegInfo, 0x0, sizeof(DASH_SEGMENTINFO_t));
				DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
				ullSeekPos = DASH_MAIN_GetSeekPosition(pDASH);
				PrintDebug("DASH seek occured! (%"PRId64")\n", ullSeekPos);

				/* 현재 AV type을 확인하여 seek position의 세그먼트 획득 */
				errVideo = DASH_CTRL_FindSegmentInfo(
					pController
					, DASH_SEGMENT_MEDIA
					, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY
					, pController->CurrentBandwidth
					//, DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_VIDEO)
					, ullSeekPos
					, &tSegInfo);

				if(DASH_ADAPTATIONSET_TYPE_AV != DASH_CTRL_GetCurrentAVType(pController))
				{
					/*
					errAudio = DASH_CTRL_GetSegmentInfo(
						pController
						, DASH_SEGMENT_MEDIA
						, DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY
						, &tAudSegInfo);
					*/
					errAudio = DASH_CTRL_FindSegmentInfo(
						pController
						, DASH_SEGMENT_MEDIA
						, DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY
						, pController->CurrentBandwidth
						, ullSeekPos
						, &tAudSegInfo);
				}

				VK_printf("result : %d, %d\n",errVideo, errAudio);

				if(errVideo == DASH_ERR_EOF || errAudio == DASH_ERR_EOF)
				{
					PrintError("%s(%d) reached end of stream\n", __FUNCTION__, __LINE__);
					P_STREAM_DASH_SetEventResult(pMDASH, DASH_EVENT_RESULT_EOF);
					DI_STREAM_DASH_SaveState(pMDASH);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_PAUSE);
					P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_FLUSH);
					P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_FLUSH);
					while( P_STREAM_DASH_GetPreconnectorCommand(pMDASH) != DASH_PRECONCMD_PAUSE
						|| P_STREAM_DASH_GetBufferCommand(pMDASH) != DASH_BUFFERCMD_PAUSE )
					{
						VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
					}
					DASH_CTRL_SetCurrentConnectingTime(pController, ullSeekPos, DASH_CONNECTTIME_VIDEO);
					DASH_CTRL_SetCurrentConnectingTime(pController, ullSeekPos, DASH_CONNECTTIME_AUDIO);
					DASH_CTRL_SetCurrentBufferingTime(pController, ullSeekPos);
					ret = DASH_ERR_OK;
					break;
				}
				else if(errVideo != DASH_ERR_OK || errAudio != DASH_ERR_OK)
				{
					PrintError("%s(%d) DASH_SEEK_FAILED\n", __FUNCTION__, __LINE__);
					P_STREAM_DASH_SetEventResult(pMDASH, DASH_EVENT_RESULT_ERROR);
					DI_STREAM_DASH_SaveState(pMDASH);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_PAUSE);
					ret = DASH_ERR_ERROR;
					break;
				}
				berrVideo = ADP_BUFFERMGR_GetUserData(pBufferMgr, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, tSegInfo.llSegmentStarttime, &pUserVideoData);
				if(berrVideo != ADP_BUFFER_OK)
				{
					goto __EVT_SEEK_OUTBUF;
				}
				if(DASH_ADAPTATIONSET_TYPE_AV != DASH_CTRL_GetCurrentAVType(pController))
				{
					berrAudio = ADP_BUFFERMGR_GetUserData(pBufferMgr, DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY, tAudSegInfo.llSegmentStarttime, &pUserAudioData);
					if(berrAudio != ADP_BUFFER_OK)
					{
						goto __EVT_SEEK_OUTBUF;
					}
				}
				//TS 일 경우 내부 accurate seek position을 calculation
				if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_TS)
				{
					requestOffset = pUserVideoData.ullSegmentSize *
						((ullSeekPos-(DASH_CTRL_ConvertMStoNonScaledTime(pController,pUserVideoData.ullStartTime)
						/ (double)DASH_CTRL_ConvertMStoNonScaledTime(pController,pUserVideoData.ullDuration))));
					berrVideo = ADP_BUFFERMGR_FindBlock(pBufferMgr, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, tSegInfo.llSegmentStarttime, requestOffset, &unVideoBlockId, &ullVideoBlockOffset);

					if(DASH_ADAPTATIONSET_TYPE_AV != DASH_CTRL_GetCurrentAVType(pController))
					{
						requestOffset = pUserAudioData.ullSegmentSize *
							((ullSeekPos-(DASH_CTRL_ConvertMStoNonScaledTime(pController,pUserAudioData.ullStartTime)
							/ (double)DASH_CTRL_ConvertMStoNonScaledTime(pController,pUserAudioData.ullDuration))));
						berrAudio = ADP_BUFFERMGR_FindBlock(pBufferMgr, DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY, tAudSegInfo.llSegmentStarttime, requestOffset, &unAudioBlockId, &ullAudioBlockOffset);
					}
				}
				//MP4 처리
				else if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_MP4)
				{
					/* mp4 accurate seek은 지원안함 ( segment 내 offset 0 에서 시작 ) */
					requestOffset = 0;
					berrVideo = ADP_BUFFERMGR_FindBlock(pBufferMgr, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, tSegInfo.llSegmentStarttime, requestOffset, &unVideoBlockId, &ullVideoBlockOffset);
					if(DASH_ADAPTATIONSET_TYPE_AV != DASH_CTRL_GetCurrentAVType(pController))
					{
						requestOffset = 0;
						berrAudio = ADP_BUFFERMGR_FindBlock(pBufferMgr, DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY, tAudSegInfo.llSegmentStarttime, requestOffset, &unAudioBlockId, &ullAudioBlockOffset);
					}
				}

				/* if seek pos is not in buffered region */
				if(berrVideo == ADP_BUFFER_FAIL || berrAudio == ADP_BUFFER_FAIL)
				{
__EVT_SEEK_OUTBUF:
					PrintDebug("%s(%d) required stream is not found in buffermgr(%"PRId64")\n",
						__FUNCTION__, __LINE__, ullSeekPos);

					P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_FLUSH);
					while( P_STREAM_DASH_GetPreconnectorCommand(pMDASH) != DASH_PRECONCMD_PAUSE )
					{
						VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
					}
					P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_FLUSH);
					while( P_STREAM_DASH_GetBufferCommand(pMDASH) != DASH_BUFFERCMD_PAUSE )
					{
						VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
					}
#if 0
					pDASH->bDump = TRUE;
#endif
					DASH_CTRL_SetCurrentConnectingTime(pController, ullSeekPos, DASH_CONNECTTIME_VIDEO);
					DASH_CTRL_SetCurrentConnectingTime(pController, ullSeekPos, DASH_CONNECTTIME_AUDIO);
					DASH_CTRL_SetCurrentBufferingTime(pController, ullSeekPos);
					P_STREAM_DASH_SetEventResult(pMDASH, DASH_EVENT_RESULT_SEEK_OUTBUF);
					ret = DASH_ERR_OK;
				}
				/* seek pos is in buffered region */
				else if(berrVideo == ADP_BUFFER_OK && berrAudio == ADP_BUFFER_OK)
				{
					PrintDebug("%s(%d) buffered data found!(%"PRId64")\n",
						__FUNCTION__, __LINE__, ullSeekPos);

					/* Video는 Audio보다 먼저 buffering 된다. read position을 video 기준으로 이동 */
					ADP_BUFFERMGR_SetReadPos(pBufferMgr, unVideoBlockId, ullVideoBlockOffset);
					P_STREAM_DASH_SetEventResult(pMDASH, DASH_EVENT_RESULT_SEEK_BUFREGION);
					ret = DASH_ERR_OK;
				}
				else
				{
					PrintDebug("%s(%d) Seek processing failed!\n", __FUNCTION__, __LINE__);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
					P_STREAM_DASH_SetEventResult(pMDASH, DASH_EVENT_RESULT_ERROR);
					ret = DASH_ERR_ERROR;
				}

				DASH_CTRL_SetIsEndOfStream(pController, FALSE);
				DI_STREAM_DASH_SaveState(pMDASH);
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_PAUSE);
				break;
			case DASH_USER_EVENT_PAUSE:
				P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_PAUSE);
				P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_PAUSE);
				break;
			case DASH_USER_EVENT_START:
				P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_RUN);
				P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_RUN);
				break;
			case DASH_USER_EVENT_STOP:
				P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_STOP);
				P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_STOP);
				break;
			case DASH_USER_EVENT_TRICK:
				break;
			case DASH_USER_EVENT_QUIT:
				break;
			case DASH_USER_EVENT_MPDUPDATE:
				/* DASH_MPD_SetMPDCommand(mpd, DASH_MPDCMD_UPDATE) */
				break;
			default:
				break;
		}
		/* Consuming userevent */
		DASH_MAIN_ClearUserEvent(pDASH);
	}
	return ret;
}

/**
 * @b Function @b Description <br>
 * Preconnector task<br>
 * pre-connect socket for segment required by DASH <br>
 *
 * @param[in] pParam	: Session handle
 *
 * @return  #  void <br>
 */
static void P_STREAM_DASH_Preconnector_Task(void *pParam)
{
	DI_IPTUNER_t* pHTuner = NULL;
	DASH_ERROR_e derr = DASH_ERR_OK;
	SESSION_HANDLE pHSession = (SESSION_HANDLE)pParam;
	DASH_PRECONCMD_e ePreconCmd = DASH_PRECONCMD_PAUSE;
	DASH_PRECONNECTOR_NOTCONNECTED_t Connecting = NULL;
	DASH_PRECONNECTOR_CONNECTED_t node = NULL;
	DASH_PRECONNECTOR_ITEM_t* item = NULL;
	DASH_PRECONNECTOR_HANDLE_t pPrecon = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_TIME_t ullSeekPos = 0ULL;
	HFLOAT64 	fAccuratePos = 0.0;
	HUINT64 	seekOffset = 0ULL;
	HUINT64		startOffset = 0ULL;
	HINT32 		nQueueSize = 0;
	HINT32 		nRet = 0;
	HINT32		nRetSub = 0;

	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		goto __Preconnector_item_close;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		goto __Preconnector_item_close;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		goto __Preconnector_item_close;
	}

	/* get handles */
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid DASH handle\n", __FUNCTION__, __LINE__);
		goto __Preconnector_item_close;
	}

	DASH_MAIN_GetPreconnectorHandle(pDASH, &pPrecon);
	if(pPrecon == NULL)
	{
		PrintError("%s(%d) Invalid preconnector handle\n", __FUNCTION__, __LINE__);
		goto __Preconnector_item_close;
	}
	DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid controller handle\n", __FUNCTION__, __LINE__);
		goto __Preconnector_item_close;
	}
	while(TRUE)
	{
		/******************************************
		*       1. Command check
		******************************************/
		ePreconCmd = P_STREAM_DASH_GetPreconnectorCommand(pMDASH);
		if(ePreconCmd == DASH_PRECONCMD_QUIT)
		{
			PrintError("%s(%d) DASH_PRECONCMD_QUIT\n", __FUNCTION__, __LINE__);
			do
			{
				derr = DASH_PRECON_GetConnected(pPrecon, &node);
				if(derr == DASH_ERR_LIST_EMPTY)
				{
					break;
				}
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
			}
			while(derr != DASH_ERR_LIST_EMPTY);

			DASH_PRECON_Flush(pPrecon);
			goto __Preconnector_quit;
		}
		else if(ePreconCmd == DASH_PRECONCMD_FLUSH)
		{
			PrintDebug("%s(%d) DASH_PRECONCMD_FLUSH\n", __FUNCTION__, __LINE__);
			do
			{
				derr = DASH_PRECON_GetConnected(pPrecon, &node);
				if(derr == DASH_ERR_LIST_EMPTY)
				{
					break;
				}
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
			}
			while(derr != DASH_ERR_LIST_EMPTY);

			DASH_PRECON_Flush(pPrecon);
			P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_PAUSE);
			continue;
		}
		else if(ePreconCmd == DASH_PRECONCMD_PAUSE)
		{
			VK_TASK_Sleep(DASH_PRECONNECTOR_TASK_SLEEP);
			continue;
		}
		else if(ePreconCmd != DASH_PRECONCMD_RUN)
		{
			VK_TASK_Sleep(DASH_PRECONNECTOR_TASK_SLEEP);
			continue;
		}

		/******************************************
		*       2. Get connected item
		******************************************/
		/* first get cmd lock */
		nQueueSize = DASH_PRECON_GetCntNotConnected(pPrecon);
		if(nQueueSize < 0)
		{
			PrintDebug("Handle is invalid!!\n");
			break;
		}
		else if (nQueueSize == 0)
		{
			//PrintDebug("%s(%d) There is nothing to connect, go to PAUSE state (%x)\n", __FUNCTION__, __LINE__, pHTuner);
			P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_PAUSE);
			continue;
		}
		/* Make a connection */
		DASH_PRECON_GetNotConnected(pPrecon, &Connecting);
		item = Connecting->pData;
#if 1
		PrintDebug("---------Preconnector connecting info-------------\n");
		PrintDebug("Tuner : %x\n",pHTuner);
		PrintDebug("item->ullRequestedPos : %"PRId64"(%"PRId64")\n", item->ullRequestedPos,
								DASH_CTRL_ConvertMStoNonScaledTime(pController, item->ullRequestedPos));
		PrintDebug("item->ullStarttime : %"PRId64"(%"PRId64")\n", item->ullStarttime,
								DASH_CTRL_ConvertMStoNonScaledTime(pController, item->ullStarttime));
		PrintDebug("item->ullDuration : %"PRId64"(%"PRId64")\n", item->ullDuration,
								DASH_CTRL_ConvertMStoNonScaledTime(pController, item->ullDuration));
		PrintDebug("item->ullStartOffset : %"PRId64"\n", item->ullStartOffset);
		PrintDebug("item->ullEndOffset : %"PRId64"\n", item->ullEndOffset);
		PrintDebug("item->bIsLastSegment : %d\n", item->bIsLastSegment);
		PrintDebug("item->pucURL : %s\n", item->pucURL);
		PrintDebug("item->bandwidth : %d\n", item->nBandwidth);
		PrintDebug("item->streamType : %d\n", item->eStreamType);
		PrintDebug("item->segmentType : %d\n", item->eSegmentType);
		PrintDebug("--------------------------------------------------\n");
#endif
		/******************************************
		*	2. Make a connection and Fill accruate info in item
		******************************************/
		if(item->bIsLastSegment == TRUE)
		{
			DASH_PRECON_MovetoConnected(pPrecon, Connecting);
			P_STREAM_DASH_SetBufferCommand(pMDASH,DASH_BUFFERCMD_RUN);
		}
		else
		{
			derr = P_STREAM_DASH_AllocStream((STREAM_ACCESS_t**)(&item->pAccess), item->pucURL);
			if(derr != DASH_ERR_OK)
			{
				PrintError("%s(%d) : Error> alloc STREAM_ACCESS Fail.\n",__FUNCTION__,__LINE__);
				nRet = DASH_ERR_ERROR;
			}
			VK_printf("segment size in pAccess : %"PRId64"\n", ((STREAM_ACCESS_t*)item->pAccess)->info.i_size);
			VK_printf("segment url : %s\n", ((STREAM_ACCESS_t*)item->pAccess)->psz_path);
			nRet = STREAM_HTTP_Open((STREAM_ACCESS_t*)item->pAccess);
			if(nRet != DI_IPT_SUCCESS)
			{
				// TODO: open fail 처리 필요 skip?
				PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				nRet = DASH_ERR_ERROR;
			}
			if(nRet != DASH_ERR_ERROR)
			{
				item->ullSegmentsize = ((STREAM_ACCESS_t*)item->pAccess)->info.i_size;
				VK_printf("segment url : %s\n", ((STREAM_ACCESS_t*)item->pAccess)->psz_path);
				VK_printf("segment size in pAccess : %"PRId64"\n", ((STREAM_ACCESS_t*)item->pAccess)->info.i_size);
				VK_printf("segment size : %"PRId64"\n", item->ullSegmentsize);
				startOffset = item->ullStartOffset;
				ullSeekPos = item->ullRequestedPos;
				if(ullSeekPos < item->ullStarttime)
				{
					PrintError("%s(%d) : Error> ullSeekPos is invalid(less than segment starttime)\n",__FUNCTION__,__LINE__);
					goto __Preconnector_item_close;
				}
				else if(ullSeekPos > item->ullStarttime + item->ullDuration)
				{
					PrintError("%s(%d) : Error> ullSeekPos is invalid(greater than segment endtime)\n",__FUNCTION__,__LINE__);
					goto __Preconnector_item_close;
				}
				else
				{
					if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_MP4)
					{
						// TODO: MP4의 경우 항상 세그먼트 처음부터 시작하도록 처리
						seekOffset = 0;
					}
					else if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_TS)
					{
						/* if ullSeekPos is over the starttime of segment, that is,
						It attempts to seek somewhere in segment duration except the beginning of segment */
						if(ullSeekPos - item->ullStarttime > 0)
						{
							fAccuratePos = (double)(ullSeekPos - item->ullStarttime) / (double)item->ullDuration;
							seekOffset = item->ullSegmentsize * fAccuratePos;
							startOffset = startOffset + seekOffset;
							/* duration 보정 - 실제 받을 세그먼트 사이즈에 해당하는 duration으로 */
							item->ullDuration = item->ullStarttime + item->ullDuration - item->ullRequestedPos;
							PrintDebug("%s(%d) Seek pos is inner segment (offset : %"PRId64")\n", __FUNCTION__, __LINE__, startOffset);
						}
					}
				}

				/* need to check command */
				ePreconCmd = P_STREAM_DASH_GetPreconnectorCommand(pMDASH);
				if(ePreconCmd == DASH_PRECONCMD_QUIT)
				{
					PrintError("%s(%d) DASH_PRECONCMD_QUIT\n", __FUNCTION__, __LINE__);
					/* release just connected item */
					if(item != NULL)
					{
						P_STREAM_DASH_ReleaseStream(item->pAccess);
					}
					DASH_PRECON_Free(pPrecon, Connecting);
					/* release item in connected list */
					do
					{
						derr = DASH_PRECON_GetConnected(pPrecon, &node);
						if(derr == DASH_ERR_LIST_EMPTY)
						{
							break;
						}
						item = node->pData;
						if(item != NULL)
						{
							P_STREAM_DASH_ReleaseStream(item->pAccess);
						}
						DASH_PRECON_Free(pPrecon, node);
					}
					while(derr != DASH_ERR_LIST_EMPTY);
					DASH_PRECON_Flush(pPrecon);
					goto __Preconnector_quit;
				}

				/* byte-range access or seek position is not the beginning of segment */
				if(startOffset > 0ULL)
				{
					PrintDebug("%s(%d) adjusted startOffset : %"PRId64"\n", __FUNCTION__,__LINE__,startOffset);
					if(item->ullEndOffset == 0ULL)
					{
						item->ullSegmentsize = ((STREAM_ACCESS_t*)item->pAccess)->info.i_size - startOffset;
					}
					else if(item->ullEndOffset != 0ULL)
					{
						item->ullSegmentsize = item->ullEndOffset - startOffset;
					}
					nRetSub = STREAM_HTTP_Seek(item->pAccess, startOffset);
					if(nRetSub != DI_IPT_SUCCESS)
					{
						PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
						/* skip this segment */
						if(item != NULL)
						{
							P_STREAM_DASH_ReleaseStream(item->pAccess);
						}
						DASH_PRECON_Free(pPrecon, Connecting);
						continue;
					}
				}
				VK_SEM_Get(pController->ulCmdlock);
				DASH_PRECON_MovetoConnected(pPrecon, Connecting);
				P_STREAM_DASH_SetBufferCommand(pMDASH,DASH_BUFFERCMD_RUN);
				VK_SEM_Release(pController->ulCmdlock);
			}
			else /* if(nRet != DASH_ERR_ERROR) */
			{
				/* skip this segment */
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, Connecting);
			}
		}
#if 0
		PrintDebug("%s(%d) Pool size (%d)\n", __FUNCTION__, __LINE__, UTIL_DLLIST_Size(&pPrecon->Pool));
		PrintDebug("%s(%d) NotConnected size (%d)\n", __FUNCTION__, __LINE__, DASH_PRECON_GetCntNotConnected(pPrecon));
		PrintDebug("%s(%d) Connectedlist size (%d)\n", __FUNCTION__, __LINE__, DASH_PRECON_GetCntConnected(pPrecon));
		PrintDebug("%s(%d) ---------------------------------------------\n", __FUNCTION__, __LINE__);
#endif
		VK_TASK_Sleep(DASH_PRECONNECTOR_TASK_SLEEP);
	}
__Preconnector_item_close:
	P_STREAM_DASH_ReleaseStream(item->pAccess);
	if(Connecting != NULL)
	{
		DASH_PRECON_Free(pPrecon, Connecting);
	}
__Preconnector_quit:
	PrintDebug("%s(%d) : TASK TERMINATED <PRECONNECTOR> \n",__FUNCTION__,__LINE__);
	P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_QUITOK);
}

/**
 * @b Function @b Description <br>
 * MPD task<br>
 * Download, parse and maintain MPD
 *
 * @param[in] pParam	: Session handle
 *
 * @return  #  void <br>
 */
static void P_STREAM_DASH_MPD_Task(void *pParam)
{
	DI_IPTUNER_t* pHTuner = NULL;
	SESSION_HANDLE pHSession = (SESSION_HANDLE)pParam;
	DASH_MPD_ERROR_e merr = DASH_ERR_OK;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_MPDPARSER_HANDLE_t	mpd = NULL;
	DASH_MPDPARSER_HANDLE_t	newmpd = NULL;
	DASH_XLINK_LIST_t *xlinks = NULL;
	HINT8* 	pMPDBuf = NULL;
	HUINT32 unRetry = 0;
	HUINT32	nContentLength = 0;
	HUINT64 ullWaitingTime = 0;
	HUINT64 ullWaitingFactor = 0;

	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		return;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return;
	}
	/* get handles */
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;
	DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid controller handle\n", __FUNCTION__, __LINE__);
		return;
	}
	DASH_MAIN_GetMPDParserHandle(pDASH, &mpd);
	if(mpd == NULL)
	{
		PrintError("%s(%d) Invalid mpd handle\n", __FUNCTION__, __LINE__);
		return;
	}
	/* waiting for initial MPD setup */
	while(pController->MPDUrl == NULL)
	{
		PrintDebug("%s(%d) Waiting for MPDUrl Setup...\n", __FUNCTION__, __LINE__);
		VK_TASK_Sleep(DASH_MPD_TASK_SLEEP_INTERVAL);
	}
	while(TRUE)
	{
		merr = DASH_MPD_SetState(mpd, DASH_MPD_STATE_UPDATING);
		if(merr != DASH_MPD_ERR_OK)
		{
			PrintError("%s(%d) : Error> MPD handle is not available...\n",__FUNCTION__,__LINE__);
			goto __ERROR_PROC;
		}
		newmpd = DASH_MPD_Create();
		if(newmpd == NULL)
		{
			PrintError("%s(%d) new_mpd create failed\n", __FUNCTION__, __LINE__);
			return;
		}
		VK_MEM_Memset(newmpd, 0x0, sizeof(DASH_MPDParser_t));
		newmpd->revision = mpd->revision+1;
		/* try to download MPD */
		while((nContentLength = P_STREAM_DASH_Download(
			(void *) UTIL_GetOriginalURL ((const char *)pController->MPDUrl) ,&pMPDBuf, 0, 0)) <= 0)
		{
			PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Retrying...\n",__FUNCTION__,__LINE__);
			if(unRetry > DASH_MAXIMUM_CONNECT_TRY)
			{
				PrintError("%s(%d) : Error> P_STREAM_MPD_Download Fail Exit...\n",__FUNCTION__,__LINE__);
				goto __ERROR_PROC;
			}
			unRetry++;
		}

		/* XML parsing */
		merr = DASH_MPD_ParseXML(newmpd, pMPDBuf, nContentLength);
		if(merr != DASH_MPD_ERR_OK)
		{
			PrintError("%s(%d) : Error> Attempt to parse invalid MPD file...\n",__FUNCTION__,__LINE__);
			goto __ERROR_PROC;
		}
		PrintDebug("%s(%d) : XML Parsing...OK\n",__FUNCTION__,__LINE__);
		if(pMPDBuf != NULL)
		{
			VK_MEM_Free(pMPDBuf);
			pMPDBuf = NULL;
		}

		/* MPD parsing */
		merr = DASH_MPD_ParseMPD(newmpd);
		if(merr != DASH_MPD_ERR_OK)
		{
			if(merr == DASH_MPD_ERR_PARSE_ERROR)
			{
				PrintError("%s(%d) : Error> Can not parse the MPD\n",__FUNCTION__,__LINE__);
				goto __ERROR_PROC;
			}
		}
#if 0
		PrintDebug("revision : %d\n", newmpd->revision);
		DASH_MPD_Print_MPD(newmpd->parsedMPD);
#endif
		PrintDebug("%s(%d) : MPD Parsing...OK\n",__FUNCTION__,__LINE__);
		/* get onload type xlink element */
		merr = DASH_MPD_GetXlink(newmpd, DASH_XLINK_ACTUATE_ONLOAD, &xlinks);
		if(merr != DASH_MPD_ERR_OK)
		{
			if(xlinks != NULL)
			{
				VK_MEM_Free(xlinks);
				xlinks = NULL;
			}
			if(merr == DASH_MPD_ERR_PARSE_ERROR)
			{
				PrintError("%s(%d) : Error> Can not parse the MPD\n",__FUNCTION__,__LINE__);
				goto __ERROR_PROC;
			}
			else if(merr == DASH_MPD_ERR_MERGE_ERROR)
			{
				PrintError("%s(%d) : Error> Can not merge the MPD\n",__FUNCTION__,__LINE__);
				goto __ERROR_PROC;
			}
		}
		/* download onload xlink element and apply directly*/
		merr = P_STREAM_DASH_XLINK_Download(xlinks);
		if(xlinks != NULL)
		{
			VK_MEM_Free(xlinks);	// TODO: internal list remove
			xlinks = NULL;
		}
		if(merr != DASH_MPD_ERR_OK)
		{
			PrintError("%s(%d) : Error> Can not retrieve XLINKs in MPD\n",__FUNCTION__,__LINE__);
			goto __ERROR_PROC;
		}
		/* postprocess : timing and so on */
		merr = DASH_MPD_PostProcess(newmpd);
		if(merr != DASH_MPD_ERR_OK)
		{
			PrintError("%s(%d) : Error> Can not postprocess the MPD\n",__FUNCTION__,__LINE__);
		}

		/* merge mpd */
		merr = DASH_MPD_MergeMPD(mpd, newmpd);
		if(merr != DASH_MPD_ERR_OK)
		{
			if(merr == DASH_MPD_ERR_MERGE_ERROR)
			{
				PrintError("%s(%d) : Error> Can not merge between MPDs\n",__FUNCTION__,__LINE__);
				goto __ERROR_PROC;
			}
		}
#if 0
		PrintDebug("revision : %d\n", mpd->revision);
		DASH_MPD_Print_MPD(mpd->parsedMPD);
#endif
		merr == DASH_MPD_SetState(mpd, DASH_MPD_STATE_READY);
		if(merr != DASH_MPD_ERR_OK)
		{
			PrintError("%s(%d) : Error> MPD handle is not available...\n",__FUNCTION__,__LINE__);
			goto __ERROR_PROC;
		}
		if(DASH_CTRL_GetIsLiveUpdate(pController) == TRUE)
		{
			ullWaitingTime = DASH_CTRL_GetUpdatePeriod(pController);
			ullWaitingFactor = ullWaitingTime / DASH_MPD_TASK_SLEEP_INTERVAL;
			while(DASH_MAIN_GetOccuredUserEvent(pDASH) != DASH_USER_EVENT_MPDUPDATE)
			{
				VK_TASK_Sleep(DASH_MPD_TASK_SLEEP_INTERVAL);
				if(ullWaitingFactor == 0)
				{
					break;
				}
				ullWaitingFactor--;
			}
			ullWaitingTime = 0;
			continue;
		}
		else
		{
			/* if MPD is not live update, no need to require this task */
			PrintDebug("%s(%d) : Task will be terminated (static MPD)\n",__FUNCTION__,__LINE__);
			goto __EXIT_TASK;
		}
	}

__ERROR_PROC:
	PrintError("%s(%d) : Error> Task has been terminated (error occured)\n",__FUNCTION__,__LINE__);
	DASH_MPD_Destroy(mpd);
	DASH_MPD_Destroy(newmpd);
	DASH_MPD_SetState(mpd, DASH_MPD_STATE_ERROR);
__EXIT_TASK:
	if(pMPDBuf != NULL)
	{
		VK_MEM_Free(pMPDBuf);
		pMPDBuf = NULL;
	}

	return;
}

/**
 * @b Function @b Description <br>
 * Buffering task<br>
 * Download segment which is pre-connected by preconnector
 *
 * @param[in] pParam	: Session handle
 *
 * @return  #  void <br>
 */
static void P_STREAM_DASH_Buffering_Task(void *pParam)
{
	DI_IPTUNER_t* pHTuner = NULL;
	SESSION_HANDLE pHSession = (SESSION_HANDLE)pParam;
	DASH_ERROR_e derr = DASH_ERR_OK;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_BUFFERMGR_HANDLE_t	pBufferMgr = NULL;
	DASH_PRECONNECTOR_HANDLE_t	pPrecon = NULL;
	DASH_PRECONNECTOR_CONNECTED_t node= NULL;
	DASH_PRECONNECTOR_ITEM_t* item = NULL;
	DASH_HEURISTICS_HANDLE_t pHeuristics = NULL;
	DASH_BUFFERCMD_e buffercmd = DASH_BUFFERCMD_PAUSE;
	ADP_BUFFERMGR_STATUS_t tBufferStat;
	HUINT32 nSkippedSegment = 0;
	HUINT32 szDownload = 0;
	HULONG 	szSpendTime = 0;

	PrintEnter;

	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		goto __Buffering_error;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		goto __Buffering_error;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		goto __Buffering_error;
	}

	/* get handles */
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;
	DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid controller handle\n", __FUNCTION__, __LINE__);
		goto __Buffering_error;
	}
	if(pController->MPDUrl == NULL)
	{
		PrintError("%s(%d) MPDUrl is NULL\n", __FUNCTION__, __LINE__);
	}
	DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Invalid buffer manager handle\n", __FUNCTION__, __LINE__);
		goto __Buffering_error;
	}
	DASH_MAIN_GetPreconnectorHandle(pDASH, &pPrecon);
	if(pPrecon == NULL)
	{
		PrintError("%s(%d) Invalid preconnector handle\n", __FUNCTION__, __LINE__);
		goto __Buffering_error;
	}
	DASH_MAIN_GetHeuristicsHandle(pDASH, &pHeuristics);
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Invalid heuristics handle\n", __FUNCTION__, __LINE__);
		goto __Buffering_error;
	}
	if(pDASH->bPreconnectorTaskInit != TRUE)
	{
		PrintError("%s(%d) preconnector task has not initialized\n", __FUNCTION__, __LINE__);
		goto __Buffering_error;
	}

	while(TRUE)
	{
		/******************************************
		*       1. Command check
		******************************************/
		buffercmd = P_STREAM_DASH_GetBufferCommand(pMDASH);
		if (buffercmd == DASH_BUFFERCMD_QUIT)
		{
			PrintError("%s(%d) DASH_BUFFERCMD_QUIT\n", __FUNCTION__, __LINE__);
			ADP_BUFFERMGR_Flush(pBufferMgr);
			if(node != NULL)
			{
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
				node = NULL;
			}
			pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_STOP;
			goto __Buffering_quit;
		}
		else if(buffercmd == DASH_BUFFERCMD_FLUSH)
		{
			PrintDebug("%s(%d) DASH_BUFFERCMD_FLUSH\n", __FUNCTION__, __LINE__);
			ADP_BUFFERMGR_Flush(pBufferMgr);
			if(node != NULL)
			{
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
				node = NULL;
			}
			P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_PAUSE);
			continue;
		}
		else if (buffercmd == DASH_BUFFERCMD_PAUSE)
		{
			VK_TASK_Sleep(DASH_BUFFERING_TASK_SLEEP);
			continue;
		}
		else if (buffercmd != DASH_BUFFERCMD_RUN)
		{
			VK_TASK_Sleep(DASH_BUFFERING_TASK_SLEEP);
			continue;
		}

		/******************************************
		*		2. Buffer stat check
		******************************************/
		ADP_BUFFERMGR_GetStatus(pBufferMgr, &tBufferStat);
		if(tBufferStat.ulTotalBlocks * ((double)DASH_RETAIN_THRESHOLD / (double)100) < tBufferStat.ulUsedBlocks)
		{
			ADP_BUFFERMGR_SimpleRetainBlock(pBufferMgr, tBufferStat.ulTotalBlocks * (double)DASH_RETAIN_BLOCK_AT_ONCE / (double)100);
		}

		/******************************************
		*      3. Get connected item
		******************************************/
		VK_SEM_Get(pController->ulCmdlock);
		if(DASH_PRECON_GetCntConnected(pPrecon) <= 0)
		{
			PrintDebug("%s(%d) Connected list in preconnector is empty, go to PAUSE state\n", __FUNCTION__, __LINE__);
			P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_PAUSE);
			VK_SEM_Release(pController->ulCmdlock);
			continue;
		}
		VK_SEM_Release(pController->ulCmdlock);

		/* get preconnected item from connected list */
		derr = DASH_PRECON_GetConnected(pPrecon, &node);
		if(derr != DASH_ERR_OK)
		{
			if(node != NULL)
			{
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
				node = NULL;
			}
			PrintError("%s(%d) getting connected list was failed\n", __FUNCTION__, __LINE__);
			VK_TASK_Sleep(DASH_BUFFERING_TASK_SLEEP);
			continue;
		}
		item = (DASH_PRECONNECTOR_ITEM_t*)node->pData;
		/* if item has last segment indicator */
		if(item->bIsLastSegment == TRUE)
		{
			if(node != NULL)
			{
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
				node = NULL;
			}
			pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_PAUSE;
			PrintDebug("%s(%d) Media Buffering finished\n", __FUNCTION__, __LINE__);
			P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_PAUSE);
			continue;
		}
		/* Notify that this item will be downloaded soon to controller */
		DASH_CTRL_SetCurrentBufferingItem(pController, item);

		/******************************************
		*  4. check whether new initialization segment is required
		******************************************/
		if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_MP4)
		{
			if(P_STREAM_DASH_MP4CACHE_IsExist(pMDASH->InitData, item->nBandwidth, item->eStreamType) != TRUE)
			{
				PrintDebug("%s(%d) New MP4 initialization information (ID : %d, Type : %d)\n",
					__FUNCTION__, __LINE__, item->nBandwidth, item->eStreamType);
				DI_STREAM_DASH_MP4Preprocess(pMDASH, item->eStreamType, item->nBandwidth, item->ullStarttime);
			}
		}

		/******************************************
		*  5. download whole segment and store it
		******************************************/
		PrintDebug("%s(%d) Tuner of current buffering task : %x\n",__FUNCTION__,__LINE__,pHTuner);
		derr = P_STREAM_DASH_Segment_Download(pHSession, item, &szDownload, &szSpendTime);
		if(derr != DASH_ERR_OK)
		{
			switch(derr)
			{
				case DASH_ERR_ERROR:
					nSkippedSegment++;
					PrintError("%s(%d) Error was occured during segment download or before segment download\n", __FUNCTION__, __LINE__);
					break;
				case DASH_ERR_STOP_BY_FLUSH:
					nSkippedSegment = 0;
					break;
				case DASH_ERR_STOP_FORCE:
					nSkippedSegment = 0;
					PrintError("%s(%d) Buffering Stop!\n", __FUNCTION__, __LINE__);
					break;
				case DASH_ERR_EXCEED_MAXIMUM_CONNECT_TRY:
					nSkippedSegment++;
					PrintError("%s(%d) Maximum connect attempt reached\n", __FUNCTION__, __LINE__);
					break;
				case DASH_ERR_STOP_BY_QUITCMD:
					goto __Buffering_error;
					/* break; */
				default:
					goto __Buffering_error;
					/* break; */
			}
			(void)STREAM_HTTP_Close((STREAM_ACCESS_t*)item->pAccess);
			if(node != NULL)
			{
				item = node->pData;
				if(item != NULL)
				{
					P_STREAM_DASH_ReleaseStream(item->pAccess);
				}
				DASH_PRECON_Free(pPrecon, node);
				node = NULL;
			}
			continue;
		}
		else
		{
			nSkippedSegment = 0;
			if(DASH_CTRL_GetHeuristicsSwitch(pController) == TRUE)
			{
				if(item->eStreamType != DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
				{
					ADP_HEURISTICS_Run(pHeuristics, item->ullSegmentsize, szSpendTime);
				}
			}
		}
		if(nSkippedSegment > DASH_MAX_CONSECUTIVE_SKIPPED_SEGMENT)
		{
			PrintError("%s(%d) Exeed maximum consecutive skipped segment... \n",__FUNCTION__, __LINE__);
		}
		DASH_CTRL_SetCurrentBufferingTime(pController, item->ullStarttime + item->ullDuration);
		if(node != NULL)
		{
			item = node->pData;
			if(item != NULL)
			{
				P_STREAM_DASH_ReleaseStream(item->pAccess);
			}
			DASH_PRECON_Free(pPrecon, node);
			node = NULL;
		}
		VK_TASK_Sleep(DASH_BUFFERING_TASK_SLEEP);
	}
__Buffering_error:
__Buffering_quit:
	PrintDebug("%s(%d) : TASK TERMINATED <BUFFERING> \n",__FUNCTION__,__LINE__);
	if(item != NULL)
	{
		P_STREAM_DASH_ReleaseStream(item->pAccess);
	}
	if(node != NULL)
	{
		DASH_PRECON_Free(pPrecon, node);
		node = NULL;
	}
	P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_QUITOK);
	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_COMPLETE;
	pHTuner->bAbrTaskFinished = TRUE;
}

/**
 * @b Function @b Description <br>
 * Create preconnector task <br>
 *
 * @param[in] pHSession	: Session handle
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_CreatePreconnectorTask (SESSION_HANDLE pHSession)
{
	MEDIA_DASH_HANDLE_t pMDASH;
	DASH_MASTER_HANDLE_t pDash;
	VK_ERR_CODE nVKRet = VK_OK;
	HINT8 task_name[128]={0};
	DI_IPTUNER_t* pHTuner = NULL;
	PrintEnter;
	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		return DASH_ERR_ERROR;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return DASH_ERR_ERROR;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDash = pMDASH->hDASH;

	if(pDash->handle.hController == NULL)
	{
		PrintError("%s(%d) Error> DASH Controller is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pDash->handle.hPreconnector== NULL)
	{
		PrintError("%s(%d) Error> DASH Preconnector is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pDash->handle.hBuffermgr == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_snprintf((char*)task_name,VK_strlen("DASH_Preconnector_")+10, "DASH_ProeonnectorTask_%p", (void*)pHSession);
	nVKRet = VK_TASK_Create ( P_STREAM_DASH_Preconnector_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, TUNER_ADAPTIVE_STACK_SIZE
									, (char*)task_name
									, (void*)pHSession
									, &pDash->unPreconnectorTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return DASH_ERR_ERROR;
	}
	pDash->bPreconnectorTaskInit = TRUE;
	VK_TASK_Start(pDash->unPreconnectorTaskId);
	PrintExit;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Create buffering task <br>
 *
 * @param[in] pHSession	: Session handle
 *
 * @return  #  Error code <br>
 */
static DASH_ERROR_e P_STREAM_DASH_CreateBufferingTask (SESSION_HANDLE pHSession)
{
	MEDIA_DASH_HANDLE_t pMDASH;
	DASH_MASTER_HANDLE_t pDash;
	VK_ERR_CODE nVKRet = VK_OK;
	HINT8 task_name[128]={0};
	DI_IPTUNER_t* pHTuner = NULL;
	PrintEnter;
	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		return DASH_ERR_ERROR;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return DASH_ERR_ERROR;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDash = pMDASH->hDASH;

	if(pDash->handle.hController == NULL)
	{
		PrintError("%s(%d) Error> DASH Controller is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pDash->handle.hPreconnector== NULL)
	{
		PrintError("%s(%d) Error> DASH Preconnector is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pDash->handle.hBuffermgr == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_snprintf((char*)task_name,VK_strlen("DASH_Buffering_")+10, "DASH_Buffering_%p", (void*)pHSession);
	nVKRet = VK_TASK_Create ( P_STREAM_DASH_Buffering_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, TUNER_ADAPTIVE_STACK_SIZE
									, (char*)task_name
									, (void*)pHSession
									, &pDash->unBufferingTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return DASH_ERR_ERROR;
	}
	pDash->bBufferingTaskInit = TRUE;
	VK_TASK_Start(pDash->unBufferingTaskId);
	PrintExit;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Scheduler task <br>
 * It controll DASH flow. <br>
 * - check user event : if user event is occured, call user event processor <br>
 * - get segment information from MPD : if exists, register to preconnector structure <br>
 *
 * @param[in] pParam	: Session handle
 *
 * @return  #  void <br>
 */
static void P_STREAM_DASH_Scheduler_Task(void *pParam)
{
	DI_IPTUNER_t* pHTuner = NULL;
	SESSION_HANDLE pHSession = (SESSION_HANDLE)pParam;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_PRECONNECTOR_HANDLE_t pPrecon = NULL;
	DASH_HEURISTICS_HANDLE_t pHeuristics = NULL;
	DASH_HEURISTICS_CONFIG_t tHeuristicsConf;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	DASH_MPDPARSER_HANDLE_t pMPDParser = NULL;
	DASH_SEGMENTINFO_t tSegInfo[NR_DASH_ADAPTATIONSET_TYPE];
	DASH_ADAPTATIONSET_TYPE_e eAVType;
	DASH_PRECONNECTOR_ITEM_t *pItem = NULL;
	DASH_PRECONNECTOR_POOL_t pPool = NULL;
	DASH_MPD_STATE_e mpdstate = DASH_MPD_STATE_NOTREADY;
	DASH_PROCESS_STATE_t state = DASH_STATE_INITIALIZING;
	DASH_EVENT_RESULT_e evt = DASH_EVENT_RESULT_NONE;
	DASH_ERROR_e nRet = DASH_ERR_OK;
	DASH_ERROR_e nGetSegResult = DASH_ERR_OK;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	HUINT32 curBandwidth = 0;
	HUINT32 minBuffer = 0;
	HUINT32 nrTryMPDUpdate = 0;
	HUINT32 nrSegment = 0;
	HINT64 	llAvailableStart = 0;
	HINT64	llAvailableEnd = 0;
	HUINT64	llPrevVideoConnectingTime = 0ULL;
	HUINT64 llPrevAudioConnectingTime = 0ULL;
	HUINT64 szDownload = 0ULL;
	HUINT64 szSegment = 0ULL;
	HBOOL 	bGotNRSegment = FALSE;
	HBOOL	bDoLiveStart = FALSE;
	HBOOL	bIsAVStreamToggled = FALSE;
	HLONG 	lTime = 0;
	struct timeval tv;
	struct tm* currentTime = NULL;

	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		goto __scheduler_error;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		goto __scheduler_error;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		goto __scheduler_error;
	}
	/* get handles */
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(pController == NULL)
	{
		PrintError("%s(%d) Cannot get Controller handle\n", __FUNCTION__, __LINE__);
		goto __scheduler_error;
	}
	nRet = DASH_MAIN_GetPreconnectorHandle(pDASH, &pPrecon);
	if(nRet != DASH_ERR_OK)
	{
		PrintError("%s(%d) Cannot get Preconnector handle\n", __FUNCTION__, __LINE__);
		goto __scheduler_error;
	}
	nRet = DASH_MAIN_GetMPDParserHandle(pDASH, &pMPDParser);
	if(nRet != DASH_ERR_OK)
	{
		PrintError("%s(%d) Cannot get MPD handle\n", __FUNCTION__, __LINE__);
		goto __scheduler_error;
	}
	while(TRUE)
	{
		state = DI_STREAM_DASH_GetState(pMDASH);

		/*********************************************
		 * Error state : DASH Scheduling 중 error를 처리
		*********************************************/
		if(state == DASH_STATE_ERROR)
		{
			PrintDebug("%s(%d) [Scheduler:ErrorState] kill preconnector and buffering task\n", __FUNCTION__, __LINE__);
			P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_QUIT);
			while(P_STREAM_DASH_GetPreconnectorCommand(pMDASH) != DASH_PRECONCMD_QUITOK)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}
			P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_QUIT);
			while(P_STREAM_DASH_GetBufferCommand(pMDASH) != DASH_BUFFERCMD_QUITOK)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}

			DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_FINALIZEOK);
			break;
		}
		/********************************************
		 * Finalize state : DASH Task 정리(preconnector, buffering task)
		*********************************************/
		else if(state == DASH_STATE_FINALIZE || pHTuner->bExitDownload == TRUE)
		{
			PrintDebug("%s(%d) [Scheduler:FinalizeState]\n", __FUNCTION__, __LINE__);
			P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_QUIT);
			while(P_STREAM_DASH_GetPreconnectorCommand(pMDASH) != DASH_PRECONCMD_QUITOK)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}
			P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_QUIT);
			while(P_STREAM_DASH_GetBufferCommand(pMDASH) != DASH_BUFFERCMD_QUITOK)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}

			DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_FINALIZEOK);
			break;
		}
		/********************************************
		 * Reset state : DASH Task Flush 및 time 리셋
		 (Adaptive Close 시 리소스 정리 및 connecting time reset)
		*********************************************/
		else if(state == DASH_STATE_RESET)
		{
			PrintDebug("%s(%d) [Scheduler:ResetState]\n", __FUNCTION__, __LINE__);
			P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_FLUSH);
			while(P_STREAM_DASH_GetPreconnectorCommand(pMDASH) != DASH_PRECONCMD_PAUSE)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}
			P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_FLUSH);
			while(P_STREAM_DASH_GetBufferCommand(pMDASH) != DASH_BUFFERCMD_PAUSE)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}
			DASH_CTRL_SetCurrentBufferingTime(pController, 0);
			DASH_CTRL_SetCurrentConnectingTime(pController, 0, DASH_CONNECTTIME_VIDEO);
			DASH_CTRL_SetCurrentConnectingTime(pController, 0, DASH_CONNECTTIME_AUDIO);
			DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_READY);
			continue;
		}
		/*********************************************
		 * Finish state : 버퍼링 완료 후 대기 상태
		*********************************************/
		else if(state == DASH_STATE_FINISH)
		{
			//PrintDebug("%s(%d) [Scheduler:FinishState]\n", __FUNCTION__, __LINE__);
			/* User event process */
			nRet = P_STREAM_DASH_UserEventProcess(pMDASH, pController);
			if(nRet == DASH_ERR_ERROR)
			{
				PrintDebug("%s(%d) Error occured while user event processing\n", __FUNCTION__, __LINE__);
				goto __scheduler_error;
			}
			evt = P_STREAM_DASH_GetEventResult(pMDASH);
			if(evt == DASH_EVENT_RESULT_SEEK_OUTBUF)
			{
				DASH_CTRL_SetIsEndOfStream(pController, FALSE);
				pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
				continue;
			}
			else if(evt == DASH_EVENT_RESULT_SEEK_BUFREGION)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
				continue;
			}
			else if(evt == DASH_EVENT_RESULT_EOF)
			{
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
				continue;
			}
			else if(evt == DASH_EVENT_RESULT_ERROR)
			{
				PrintError("%s(%d) Error occured while user event processing\n", __FUNCTION__, __LINE__);
				goto __scheduler_error;
			}
			VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
		}
		/*********************************************
		 * Live stanby state : Live에서 available start time 대기 상태
		*********************************************/
		else if(state == DASH_STATE_LIVESTANDBY)
		{
			PrintDebug("%s(%d) [Scheduler:LiveStanbyState : Tuner(%x)]\n", __FUNCTION__, __LINE__, pHTuner);
			bDoLiveStart = FALSE;
			while(bDoLiveStart == FALSE)
			{
				llAvailableStart = DASH_MPD_GetAvailableStartTime(pMPDParser);
				gettimeofday(&tv, 0);
				currentTime = localtime(&tv.tv_sec);
				lTime = mktime(currentTime);
				if(llAvailableStart <= lTime)
				{
					bDoLiveStart = TRUE;
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_READY);
					break;
				}
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
			}
		}
		/********************************************
		 * Initializing state : DASH 모듈 초기화 상태
		*********************************************/
		else if(state == DASH_STATE_INITIALIZING)
		{
			//PrintDebug("%s(%d) [Scheduler:InitState]\n", __FUNCTION__, __LINE__);

			/* waiting for MPD download */
			merr = DASH_MPD_WaitState(pMPDParser, DASH_MPD_STATE_READY);
			if(merr == DASH_MPD_ERR_ERROR)
			{
				PrintDebug("%s(%d) [Scheduler:InitState : Tuner(%x)] MPD construction failed\n", __FUNCTION__, __LINE__, pHTuner);
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			/* initialize heuristics */
			nRet = DASH_MAIN_GetHeuristicsHandle(pDASH, &pHeuristics);
			if(nRet != DASH_ERR_OK)
			{
				PrintError("%s(%d) Cannot get Heuristics handle\n", __FUNCTION__, __LINE__);
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			if(ADP_HEURISTICS_GetDefaultConfig(pHeuristics, ADP_HEUR_CONFIG_PRESET_DASH, &tHeuristicsConf))
			{
				PrintError ("%s(%d) Fail! GetDefaultConfig was failed. \n",__FUNCTION__,__LINE__);
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			nRet = DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
			if(nRet != DASH_ERR_OK)
			{
				PrintError("%s(%d) Cannot get buffermgr handle\n", __FUNCTION__, __LINE__);
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			tHeuristicsConf.hCtrl = pController;
			tHeuristicsConf.hBufferMgr = pBufferMgr;
			tHeuristicsConf.eStartup = DI_IPTUNER_ADAPTIVE_STARTUP_LOWEST;
			tHeuristicsConf.eDecision= DI_IPTUNER_ADAPTIVE_DECISION_AUTO;
			tHeuristicsConf.cbCurDuration = _CB_DASH_CTRL_GetCurDuration;
			tHeuristicsConf.cbIsProtected = _CB_DASH_CTRL_GetIsProtected;
			tHeuristicsConf.cbCurBitrate = _CB_DASH_CTRL_GetCurBitrate;
			tHeuristicsConf.cbGearLowest = _CB_DASH_CTRL_SetLowestBandwidth;
			tHeuristicsConf.cbGearHighest = _CB_DASH_CTRL_SetHighestBandwidth;
			tHeuristicsConf.cbGearUp = _CB_DASH_CTRL_SetBandwidthStepUp;
			tHeuristicsConf.cbGearDown = _CB_DASH_CTRL_SetBandwidthStepDown;
			tHeuristicsConf.cbGearSet = _CB_DASH_CTRL_SetManualBandwidth;
			if(ADP_HEURISTICS_Init(pHeuristics, &tHeuristicsConf))
			{
				PrintError ("%s(%d) Fail! Heuristics Init was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			/* Task Create */
			nRet = P_STREAM_DASH_CreatePreconnectorTask(pHSession);
			if(nRet != DASH_ERR_OK)
			{
				PrintError ("%s(%d) Error> Preconnector create failed !!\n",__FUNCTION__,__LINE__);
				pHTuner->hAbr = NULL;
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			nRet = P_STREAM_DASH_CreateBufferingTask(pHSession);
			if(nRet != DASH_ERR_OK)
			{
				PrintError ("%s(%d) Error> Buffering task create failed !!\n",__FUNCTION__,__LINE__);
				pHTuner->hAbr = NULL;
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
			/* Change the size of buffer
			   After MPD processing, we finally know the required buffer size */
			//unMinBuffer = DASH_CTRL_GetMinBufferTime(pController);
			//unBandwidth = DASH_CTRL_GetMaxBandwidth(pController, DASH_TIME_BASE_CONNECTING);
			//nRet = ADP_BUFFERMGR_Resize(pBufferMgr, unMinBuffer/1000 * unBandwidth/8 * 2);
			nRet = ADP_BUFFERMGR_Resize(pBufferMgr, DASH_DEFAULT_TIMESHIFTBUFFER_SIZE);
			PrintDebug("%s(%d) :Buffer size changed (%d)\n", __FUNCTION__, __LINE__, nRet);

			if(DASH_MPD_Get_IsLiveUpdate(pMPDParser) == TRUE)
			{
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_LIVESTANDBY);
			}
			else
			{
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_READY);
			}

			/* default AV type is Video only */
			nRet = DASH_CTRL_SetCurrentAVType(pController, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY);
			if(nRet != DASH_ERR_OK)
			{
				PrintError ("%s(%d) Error> failed to change AV type !!\n",__FUNCTION__,__LINE__);
				pHTuner->hAbr = NULL;
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
			}
		}
		/********************************************
		 * Ready state : DASH 대기 상태 (for sync)
		*********************************************/
		else if(state == DASH_STATE_READY)
		{
			;//PrintDebug("%s(%d) [Scheduler:ReadyState]\n", __FUNCTION__, __LINE__);
		}
		/********************************************
		 * Pause state : DASH 스케쥴링 정지 (related to seek sync)
		*********************************************/
		else if(state == DASH_STATE_PAUSE)
		{
			;//PrintDebug("%s(%d) [Scheduler:PauseState]\n", __FUNCTION__, __LINE__);
		}
		/********************************************
		 * Prebuffering state : DASH Prebuffering 상태
		*********************************************/
		else if(state == DASH_STATE_PREBUFFERING)
		{
			if(pController->bIsPrebufferd != TRUE)
			{
				PrintDebug("%s(%d) [Scheduler:PrebufferingState(B:%d) : Tuner(%x), Connecting time(%"PRId64")]\n",
					__FUNCTION__, __LINE__, DASH_CTRL_GetCurrentBandwidth(pController), pHTuner, DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_VIDEO));
				minBuffer = DASH_CTRL_GetMinBufferTime(pController);
				if(minBuffer == 0)
				{
					PrintDebug("%s(%d) No need to prebuffering, minbuffer(%d)\n", __FUNCTION__,__LINE__,minBuffer);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_READY);
					continue;
				}
				curBandwidth = DASH_CTRL_GetCurrentBandwidth(pController);
				szDownload = (HUINT64)minBuffer * (HUINT64)curBandwidth / 1000ULL / 8ULL;	// don't make 0
				PrintDebug("szDownload : %"PRId64", minBuffer : %d, curBandwidth : %d\n", szDownload, minBuffer, curBandwidth);
				do
				{
					PrintDebug("%s(%d) [Scheduler:PrebufferingState] do prebuffering\n", __FUNCTION__, __LINE__);
					/* how many segment need to be downloaded? */
					if(szDownload != 0)
					{
						eAVType = DASH_CTRL_GetCurrentAVType(pController);
						VK_MEM_Memset(&tSegInfo[eAVType], 0x0, sizeof(DASH_SEGMENTINFO_t));
						nGetSegResult = DASH_CTRL_GetSegmentInfo(
										pController
										, DASH_SEGMENT_MEDIA
										, eAVType
										, &tSegInfo[eAVType] );
						if(bGotNRSegment == FALSE)
						{
							szSegment = tSegInfo[eAVType].llSegmentDuration * pController->CurrentBandwidth / 1000ULL / 8ULL;
							nrSegment = szDownload / szSegment;
#if 0
							VK_printf("minBuffer : %d\n", minBuffer);
							VK_printf("CurrentSelectedBandwidth : %d\n", pController->CurrentBandwidth);
							VK_printf("szDownload : %"PRId64"\n", szDownload);
							VK_printf("llsegmentduration : %"PRId64"\n", tSegInfo[eAVType].llSegmentDuration);
							VK_printf("szSegment : %"PRId64"\n",szSegment);
#endif
							bGotNRSegment = TRUE;
						}
#if 0
						VK_printf("current connecting time : %"PRId64"\n", DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_VIDEO));
						VK_printf("scheduled segment url : %s\n", tSegInfo[eAVType].pucSegmentURL);
						VK_printf("Remaining Segments : %d\n",nrSegment);
#endif

						if(nGetSegResult == DASH_ERR_ERROR)
						{
							PrintError("%s(%d) DASH_ERR_ERROR, failed to get segment url\n", __FUNCTION__, __LINE__);
							DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
						}
						else
						{
							/* if EOF */
							if(nGetSegResult == DASH_ERR_EOF)
							{
								PrintError("%s(%d) DASH_ERR_EOF, there is no more media segments\n", __FUNCTION__, __LINE__);
								tSegInfo[eAVType].bIsLastSegment = TRUE;
								DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_FINISH);
							}
							else
							{
								tSegInfo[eAVType].bIsLastSegment = FALSE;
								if(evt == DASH_EVENT_RESULT_SEEK_OUTBUF)
								{
									tSegInfo[eAVType].llSegmentSeektime = DASH_MAIN_GetSeekPosition(pDASH);
								}
								else
								{
									tSegInfo[eAVType].llSegmentSeektime = tSegInfo[eAVType].llSegmentStarttime;
								}
							}
							/* try to get pool element */
							do
							{
								nRet = DASH_PRECON_GetPool(pPrecon, &pPool);
								if(nRet == DASH_ERR_POOL_EMPTY)
								{
									VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
								}
							}
							while(nRet == DASH_ERR_POOL_EMPTY);
							/* Fill item that we just obtain from pool using dash_segmentinfo */
							pItem = pPool->pData;
							VK_MEM_Memset(pItem, 0, sizeof(DASH_PRECONNECTOR_ITEM_t));
							nRet = P_STREAM_DASH_ConvertSegmentInfoToItem(&tSegInfo[eAVType], pItem);
							if(nRet != DASH_ERR_OK)
							{
								PrintError("%s(%d) DASH_ERR_ERROR, failed to fill preconnector item\n", __FUNCTION__, __LINE__);
								DASH_PRECON_Free(pPrecon, pPool);
								DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
								continue;
							}
							nRet = DASH_PRECON_MovetoNotConnected(pPrecon, pPool);
							if(nRet != DASH_ERR_OK)
							{
								PrintError("%s(%d) DASH_ERR_ERROR, failed to move pool item\n", __FUNCTION__, __LINE__);
								DASH_PRECON_Free(pPrecon, pPool);
								DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
								continue;
							}
							P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_RUN);
						}
					}
					else
					{
						break;
					}
				}
				while(nrSegment-- > 0);

				/* waiting for prebuffering completion */
				if(szDownload != 0)
				{
					while(P_STREAM_DASH_GetBufferCommand(pMDASH) != DASH_BUFFERCMD_PAUSE);
				}
			}
			pController->bIsPrebufferd = TRUE;
			DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_READY);
		}
		/********************************************
		 * Buffering state : DASH buffering 상태
		*********************************************/
		else if(state == DASH_STATE_BUFFERING)
		{
			PrintDebug("%s(%d) [Scheduler:BufferingState(B:%d): Tuner(%x), Connecting time(V:%"PRId64", A:%"PRId64")]\n",
				__FUNCTION__,
				__LINE__,
				DASH_CTRL_GetCurrentBandwidth(pController),
				pHTuner,
				DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_VIDEO),
				DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_AUDIO));

			/* MPD State check */
			mpdstate = DASH_MPD_GetState(pMPDParser);
			if(mpdstate == DASH_MPD_STATE_UPDATING)
			{
				PrintDebug("%s(%d) [Scheduler:BufferingState] MPD is now updating\n", __FUNCTION__, __LINE__);
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
				continue;
			}
			else if (mpdstate == DASH_MPD_STATE_NOTREADY)
			{
				PrintDebug("%s(%d) [Scheduler:BufferingState] MPD is not ready\n", __FUNCTION__, __LINE__);
				VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
				continue;
			}
			else if (mpdstate == DASH_MPD_STATE_ERROR)
			{
				PrintDebug("%s(%d) [Scheduler:BufferingState] MPD construction failed during MPD updating\n",
					__FUNCTION__, __LINE__);
				DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
				continue;
			}
			/* UserEvent check */
			nRet = P_STREAM_DASH_UserEventProcess(pMDASH, pController);
			if(nRet == DASH_ERR_ERROR)
			{
				PrintError("%s(%d) Error occured while user event processing\n", __FUNCTION__, __LINE__);
				goto __scheduler_error;
			}
			evt = P_STREAM_DASH_GetEventResult(pMDASH);

			/* Get SegmentInfo from MPD */
			llPrevVideoConnectingTime = DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_VIDEO);
			llPrevAudioConnectingTime = DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_AUDIO);

			/* If this condition is true, we need to download audio segment */
			if(llPrevVideoConnectingTime > llPrevAudioConnectingTime)
			{
				if(DASH_CTRL_GetCurrentAVType(pController) != DASH_ADAPTATIONSET_TYPE_AV)
				{
					DASH_CTRL_ToggleCurrentAVType(pController);
					bIsAVStreamToggled = TRUE;
				}
			}
			eAVType = DASH_CTRL_GetCurrentAVType(pController);
			/* Common segment download mechanism : Try to find segment based on current selected AV type */
			PrintDebug("Now DASH is going to looking for a segment of eAVType(%d)\n", eAVType);
			VK_MEM_Memset(&tSegInfo[eAVType], 0x0, sizeof(DASH_SEGMENTINFO_t));
			nGetSegResult = DASH_CTRL_GetSegmentInfo(
							pController
							, DASH_SEGMENT_MEDIA
							, eAVType
							, &tSegInfo[eAVType] );

			/* If avstream is toggled, rollback it */
			if(bIsAVStreamToggled == TRUE)
			{
				bIsAVStreamToggled = FALSE;
				/* if Audio segment does not exist, consider as AV Stream */
				if(nGetSegResult == DASH_ERR_ERROR)
				{
					/*
						실제 DASH의 adaptation set들은 Audio 혹은 Video 둘 중 하나만 가진다.
						AV타입은 Scheduler에서 운용하기 위한 요소이며, DASH에서 AV로 운용하더라도
						실제 획득한 Segment 정보들은 Video 혹은 Audio 정보를 가진다.
						VIDEO track만 존재할 경우 AV로 간주하도록 전환한다.

						AV - Video only or Video+Audio intergrated
						Video - Video only
						Audio - Audio only
					*/
					PrintDebug(" *** Current Stream is Video only, but there are no Audio track."
						"so consider video stream as AV stream ***\n");
					DASH_CTRL_SetCurrentAVType(pController, DASH_ADAPTATIONSET_TYPE_AV);
					continue;
				}
				DASH_CTRL_ToggleCurrentAVType(pController);
			}

			if(nGetSegResult == DASH_ERR_ERROR)
			{
				if(DASH_MPD_Get_IsLiveUpdate(pMPDParser) == TRUE)
				{
					llAvailableEnd = DASH_MPD_GetAvailableEndTime(pMPDParser);
					gettimeofday(&tv, 0);
					currentTime = localtime(&tv.tv_sec);
					lTime = mktime(currentTime);
					/* if there is remainging running time, request to update MPD */
					if(llAvailableEnd > lTime)
					{
						DASH_MAIN_SetOccuredUserEvent(pDASH, DASH_USER_EVENT_MPDUPDATE);
						nrTryMPDUpdate = 0;
					}
					else
					{
						/* if available end time is unknown */
						if(llAvailableEnd == DASH_WALLCLK_NOT_EXIST
							&& nrTryMPDUpdate < DASH_MAXTRY_MPDUPDATE_FOR_UNKNOWNENDTIME)
						{
							DASH_MAIN_SetOccuredUserEvent(pDASH, DASH_USER_EVENT_MPDUPDATE);
							nrTryMPDUpdate++;
						}
						/* available end time reached */
						else
						{
							PrintError("%s(%d) Availability end time reached\n", __FUNCTION__, __LINE__);
							DASH_CTRL_SetIsEndOfStream(pController, TRUE);
							DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_FINISH);
						}
					}
				}
				/* if not live case, this result in error */
				else
				{
					PrintError("%s(%d) DASH_ERR_ERROR, failed to get segment url (requested time : %"PRId64")\n",
						__FUNCTION__, __LINE__, DASH_CTRL_GetCurrentConnectingTime(pController,DASH_CONNECTTIME_VIDEO));
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
				}
			}
			else	/* if(nGetSegResult == DASH_ERR_ERROR) */
			{
				nrTryMPDUpdate = 0;
				/* EOF processing (for static mpd) - live mpd does not enter this case */
				if(nGetSegResult == DASH_ERR_EOF)
				{
					PrintError("%s(%d) DASH_ERR_EOF, there are no more media segments (requested time : %"PRId64")\n",
						__FUNCTION__, __LINE__, DASH_CTRL_GetCurrentConnectingTime(pController,DASH_CONNECTTIME_VIDEO));
					tSegInfo[eAVType].bIsLastSegment = TRUE;
					DASH_CTRL_SetIsEndOfStream(pController, TRUE);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_FINISH);
				}
				else
				{
					/* check result of user event processing */
					if(evt == DASH_EVENT_RESULT_SEEK_OUTBUF)
					{
						tSegInfo[eAVType].llSegmentSeektime = DASH_MAIN_GetSeekPosition(pDASH);
						P_STREAM_DASH_ClearEventResult(pMDASH);
					}
					else
					{
						tSegInfo[eAVType].llSegmentSeektime = tSegInfo[eAVType].llSegmentStarttime;
					}
				}

				/**************************************
				 * Finally, we pruned exceptional cases.
				 **************************************/

				/* try to get pool element */
				do
				{
					if(DASH_MAIN_GetOccuredUserEvent(pDASH) != DASH_USER_EVENT_NONE)
					{
						if(DASH_MAIN_GetOccuredUserEvent(pDASH) == DASH_USER_EVENT_SEEK)
						{
							/* rollback : connecting time */
							DASH_CTRL_SetCurrentConnectingTime(pController, llPrevVideoConnectingTime, DASH_CONNECTTIME_VIDEO);
							DASH_CTRL_SetCurrentConnectingTime(pController, llPrevAudioConnectingTime, DASH_CONNECTTIME_AUDIO);
							nRet = DASH_ERR_STOP_BY_USEREVENT;
							break;
						}
					}
					nRet = DASH_PRECON_GetPool(pPrecon, &pPool);
					if(nRet == DASH_ERR_POOL_EMPTY)
					{
						VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
					}
				}
				while(nRet == DASH_ERR_POOL_EMPTY);
				/* if stop by user event */
				if(nRet == DASH_ERR_STOP_BY_USEREVENT)
				{
					continue;
				}
				/* fill item that we obtain from pool using dash_segmentinfo */
				pItem = pPool->pData;
				VK_MEM_Memset(pItem, 0, sizeof(DASH_PRECONNECTOR_ITEM_t));
				nRet = P_STREAM_DASH_ConvertSegmentInfoToItem(&tSegInfo[eAVType], pItem);
				if(nRet != DASH_ERR_OK)
				{
					PrintError("%s(%d) DASH_ERR_ERROR, failed to make preconnector item\n", __FUNCTION__, __LINE__);
					VK_MEM_Free(pItem);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
					continue;
				}
				/* add pool element to not connected */
				nRet = DASH_PRECON_MovetoNotConnected(pPrecon, pPool);
				if(nRet != DASH_ERR_OK)
				{
					PrintError("%s(%d) DASH_ERR_ERROR, failed to move pool item\n", __FUNCTION__, __LINE__);
					DASH_PRECON_Free(pPrecon, pPool);
					DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_ERROR);
					continue;
				}
				P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_RUN);
			}
		}
		VK_TASK_Sleep(DASH_SCHEDULER_TASK_SLEEP);
	}
__scheduler_error:
	PrintDebug("%s(%d) : TASK TERMINATED <SCHEDULER> : Tuner(%x)\n",__FUNCTION__,__LINE__,pHTuner);
	P_STREAM_DASH_SetPreconnectorCommand(pMDASH, DASH_PRECONCMD_QUIT);
	P_STREAM_DASH_SetBufferCommand(pMDASH, DASH_BUFFERCMD_QUIT);
}

/**
 * @b Function @b Description <br>
 * Create DASH data structures, and register handles <br>
 *
 * @param[in] pUrl	: DASH MPD Url
 *
 * @return  #  MEDIA DASH handle <br>
 */
MEDIA_DASH_HANDLE_t DI_STREAM_DASH_OpenController(HINT8 * pUrl)
{
	DASH_ERROR_e derr = DASH_ERR_OK;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_PRECONNECTOR_HANDLE_t pPrecon = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_MPDPARSER_HANDLE_t pMPDParser = NULL;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	DASH_BUFFERMGR_CONFIG_t	tBufferMgrCfg;
	DASH_HEURISTICS_HANDLE_t pHeuristics = NULL;
	HINT8 sem_name[128] = {0};
	HINT32 nRet = 0;

	PrintEnter;

	if(pUrl == NULL)
	{
		PrintError("%s(%d) : Error> invalid pUrl\n",__FUNCTION__,__LINE__);
		goto open_error;
	}

	pMDASH = VK_MEM_Alloc(sizeof(DASH_t));
	VK_MEM_Memset(pMDASH, 0x0, sizeof(DASH_t));

	/* create handles */
	pDASH = DASH_MAIN_Create();
	if(pDASH == NULL)
	{
		PrintError("%s(%d) : Error> DASH data create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	pPrecon = DASH_PRECON_Create();
	if(pPrecon == NULL)
	{
		PrintError("%s(%d) : Error> Preconnector data create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	pController = DASH_CTRL_Create();
	if(pController == NULL)
	{
		PrintError("%s(%d) : Error> Controller data create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	pMPDParser = DASH_MPD_Create();
	if(pMPDParser == NULL)
	{
		PrintError("%s(%d) : Error> MPD parser data create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	/* heuristics will be initialized in scheduler task */
	pHeuristics = VK_MEM_Alloc(sizeof(ADP_HEURISTICS_t));
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) : Error> heuristics create failed\n", __FUNCTION__, __LINE__);
		goto open_error;
	}
	VK_MEM_Memset(pHeuristics, 0x0, sizeof(ADP_HEURISTICS_t));

	DASH_MPD_REGISTER_XLINKDOWN_CB(pMPDParser, (DASH_XLINK_DOWN_CB_t)P_STREAM_DASH_XLINK_Download);
	tBufferMgrCfg.eType = 0;
	tBufferMgrCfg.eMode = 0;
	tBufferMgrCfg.ullBufferSize = DASH_BUFFERMANAGER_TEMPORAL_BUFFERSIZE;
	tBufferMgrCfg.ulBlockSize = DASH_BUFFERMANAGER_BLOCKSIZE;

	pBufferMgr = (DASH_BUFFERMGR_HANDLE_t)ADP_BUFFERMGR_Create(&tBufferMgrCfg);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) : Error> Buffer manager create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}

	/* configuration */
	derr = DASH_PRECON_InitPool(pPrecon, DASH_PRECONNECTOR_MAXPOOL);
	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) : Error> (Preconnector) Initialize pool failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	derr = DASH_CTRL_SetMPDUrl(pController, pUrl);
	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) : Error> MPDUrl Setting failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	derr = DASH_CTRL_SetMPDParser(pController, pMPDParser);
	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) : Error> MPDparser Setting failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}

	DASH_CTRL_SetHeuristicsSwitch(pController, DASH_DEFAULT_HEURISTICS_MODE);	/* heuristics on */

	/* attach handles */
	DASH_MAIN_SetPreconnectorHandle(pDASH, pPrecon);
	DASH_MAIN_SetControllerHandle(pDASH, pController);
	DASH_MAIN_SetMPDParserHandle(pDASH, pMPDParser);
	DASH_MAIN_SetBufferMgrHandle(pDASH, pBufferMgr);
	DASH_MAIN_SetHeuristicsHandle(pDASH, pHeuristics);

	DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_INITIALIZING);


	pMDASH->hDASH = pDASH;
	/* minor configuration */
	pMDASH->mp4box = mem_file_open(DASH_MP4BOX_BUFFERSIZE);	/* mp4 box buffer initialize */
	if(pMDASH->mp4box == NULL)
	{
		PrintError("%s(%d) : Error> MP4 box memfile create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	/* create mp4 cache */
	pMDASH->InitData = P_STREAM_DASH_MP4CACHE_Create(DASH_MP4CACHE_DEFAULT_ELEMENT_COUNT);
	if(pMDASH->InitData == NULL)
	{
		PrintError("%s(%d) : Error> MP4 cache create failed\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	nRet = VK_SEM_Create((unsigned long*)&pMDASH->ulSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if(nRet != VK_OK)
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		VK_MEM_Free(pMDASH);
		pMDASH = NULL;
	}
	PrintExit;
	return pMDASH;
open_error:
	if(pController != NULL)
	{
		DASH_CTRL_Destroy(pController);
		pController = NULL;
	}
	if(pPrecon != NULL)
	{
		DASH_PRECON_Destroy(pPrecon);
		pPrecon = NULL;
	}
	if(pMPDParser != NULL)
	{
		DASH_MPD_Destroy(pMPDParser);
		pMPDParser = NULL;
	}
	if(pHeuristics != NULL)
	{
		VK_MEM_Free(pHeuristics);
		pHeuristics = NULL;
	}
	if(pBufferMgr != NULL)
	{
		ADP_BUFFERMGR_Destroy(pBufferMgr);
		pBufferMgr = NULL;
	}
	if(pDASH != NULL)
	{
		DASH_MAIN_Destroy(pDASH);
		pDASH = NULL;
	}
	if(pMDASH != NULL)
	{
		if(pMDASH->InitData != NULL)
		{
			P_STREAM_DASH_MP4CACHE_Destroy(pMDASH->InitData);
		}
		pMDASH->InitData = NULL;
		VK_MEM_Free(pMDASH);
		pMDASH = NULL;
	}

	return NULL;
}

/**
 * @b Function @b Description <br>
 * Destroy DASH data structures <br>
 *
 * @param[in] pMDASH	: MEDIA DASH handle
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_CloseController(MEDIA_DASH_HANDLE_t pMDASH)
{
	DASH_MASTER_HANDLE_t pDASH;
	DASH_PRECONNECTOR_HANDLE_t pPrecon;
	DASH_CONTROLLER_HANDLE_t pController;
	DASH_MPDPARSER_HANDLE_t pMPDParser;
	DASH_BUFFERMGR_HANDLE_t	pBufferMgr;
	DASH_HEURISTICS_HANDLE_t pHeuristics;
	DASH_ERROR_e derr = DASH_ERR_OK;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	ADP_BUFFER_ERR aerr = ADP_BUFFER_OK;

	PrintEnter;

	/* get handles */
	pDASH = pMDASH->hDASH;
	DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid controller handle\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	DASH_MAIN_GetPreconnectorHandle(pDASH, &pPrecon);
	if(pPrecon == NULL)
	{
		PrintError("%s(%d) Invalid preconnector handle\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	DASH_MAIN_GetMPDParserHandle(pDASH, &pMPDParser);
	if(pMPDParser == NULL)
	{
		PrintError("%s(%d) Invalid MPD parser handle\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Invalid buffer manager handle\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	DASH_MAIN_GetHeuristicsHandle(pDASH, &pHeuristics);
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Invalid Heuristics handle\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	// DASH가 종료를 위해 작업하는 상태가 아닐 경우 상태 변경
	if(DI_STREAM_DASH_GetState(pMDASH) != DASH_STATE_ERROR &&
		DI_STREAM_DASH_GetState(pMDASH) != DASH_STATE_FINALIZE &&
		DI_STREAM_DASH_GetState(pMDASH) != DASH_STATE_FINALIZEOK)
	{
		DI_STREAM_DASH_SetState(pMDASH,DASH_STATE_FINALIZE);
	}

	/* quit tasks and waiting */
	DI_STREAM_DASH_WaitState(pMDASH, DASH_STATE_FINALIZEOK);

	/* destroy handles */
	derr = DASH_CTRL_Destroy(pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) Failed to destroy(controller)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	derr = DASH_PRECON_Destroy(pPrecon);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) Failed to destroy(preconnector)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	merr = DASH_MPD_Destroy(pMPDParser);
	if(merr != DASH_MPD_ERR_OK)
	{
		PrintError("%s(%d) Failed to destroy(mpd_parser)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	aerr = ADP_BUFFERMGR_Destroy(pBufferMgr);
	if(aerr != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) Failed to destroy(buffer manager)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pHeuristics != NULL)
	{
		VK_MEM_Free(pHeuristics);
	}
	mem_file_close(pMDASH->mp4box);

	/* DASH_MAIN should be destroyed last */
	if(pDASH->unMPDTaskId != 0)
	{
		VK_TASK_Stop(pDASH->unMPDTaskId);
		VK_TASK_Destroy(pDASH->unMPDTaskId);
		pDASH->unMPDTaskId = 0;
	}
	if(pDASH->unSchedulerTaskId != 0)
	{
		VK_TASK_Stop(pDASH->unSchedulerTaskId);
		VK_TASK_Destroy(pDASH->unSchedulerTaskId);
		pDASH->unSchedulerTaskId = 0;
	}
	if(pDASH->unBufferingTaskId != 0)
	{
		VK_TASK_Stop(pDASH->unBufferingTaskId);
		VK_TASK_Destroy(pDASH->unBufferingTaskId);
		pDASH->unBufferingTaskId = 0;
	}
	if(pDASH->unPreconnectorTaskId != 0)
	{
		VK_TASK_Stop(pDASH->unPreconnectorTaskId);
		VK_TASK_Destroy(pDASH->unPreconnectorTaskId);
		pDASH->unPreconnectorTaskId = 0;
	}

	derr = DASH_MAIN_Destroy(pDASH);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) Failed to destroy(DASH data)\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	/* cache destroy */
	if(pMDASH->InitData != NULL)
	{
		P_STREAM_DASH_MP4CACHE_Destroy(pMDASH->InitData);
		pMDASH->InitData = NULL;
	}

	VK_SEM_Destroy(pMDASH->ulSema);

	/* MEDIA DASH HANDLE destroy */
	VK_MEM_Free(pMDASH);
	pMDASH = NULL;

	PrintExit;
	return DI_ERR_OK;

}

/**
 * @b Function @b Description <br>
 * Create scheduler task <br>
 *
 * @param[in] pHSession	: session handle
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_CreateSchedulerTask (SESSION_HANDLE pHSession)
{
	MEDIA_DASH_HANDLE_t pMDASH;
	DASH_MASTER_HANDLE_t pDash;
	VK_ERR_CODE nVKRet = VK_OK;
	HINT8 task_name[128]={0};
	DI_IPTUNER_t* pHTuner = NULL;
	PrintEnter;
	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDash = pMDASH->hDASH;
	if(pDash->handle.hController == NULL)
	{
		PrintError("%s(%d) Error> DASH Controller is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pDash->handle.hPreconnector== NULL)
	{
		PrintError("%s(%d) Error> DASH Preconnector is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pDash->handle.hBuffermgr == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_snprintf((char*)task_name,VK_strlen("DASH_Scheduler_")+10, "DASH_Scheduler_%p", (void*)pHSession);
	nVKRet = VK_TASK_Create ( P_STREAM_DASH_Scheduler_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, TUNER_ADAPTIVE_STACK_SIZE
									, (char*)task_name
									, (void*)pHSession
									, &pDash->unSchedulerTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return DI_ERR_ERROR;
	}
	pDash->bSchedulerTaskInit = TRUE;
	VK_TASK_Start(pDash->unSchedulerTaskId);
	PrintExit;
	return DI_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Create MPD task <br>
 *
 * @param[in] pHSession	: session handle
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_CreateMPDTask (SESSION_HANDLE pHSession)
{
	MEDIA_DASH_HANDLE_t pMDASH;
	DASH_MASTER_HANDLE_t pDash;
	VK_ERR_CODE nVKRet = VK_OK;
	HINT8 task_name[128]={0};
	DI_IPTUNER_t* pHTuner = NULL;
	PrintEnter;
	if(pHSession == NULL)
	{
		PrintError("%s(%d) : Error> Session Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDash = pMDASH->hDASH;
	if(pDash->handle.hController == NULL)
	{
		PrintError("%s(%d) Error> DASH Controller is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pDash->handle.hPreconnector== NULL)
	{
		PrintError("%s(%d) Error> DASH Preconnector is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	if(pDash->handle.hBuffermgr == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	VK_snprintf((char*)task_name,VK_strlen("DASH_MPD_")+10, "DASH_MPD_%p", (void*)pHSession);
	nVKRet = VK_TASK_Create ( P_STREAM_DASH_MPD_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, 512*1024
									, (char*)task_name
									, (void*)pHSession
									, &pDash->unMPDTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return DI_ERR_ERROR;
	}
	pDash->bMPDTaskInit = TRUE;
	VK_TASK_Start(pDash->unMPDTaskId);
	PrintExit;
	return DI_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get Media format information from DASH to determine DASH works as TS or MP4 <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[out] format : segment format value
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_GetMediaFormat (MEDIA_DASH_HANDLE_t pMDASH, DASH_SEGMENT_FORMAT_e *format)
{
	DASH_MASTER_HANDLE_t	pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_ERROR_e derr;

	PrintEnter;
	if( pMDASH == NULL )
	{
		PrintError("%s(%d) : Error> MEDIADASH HANDLE is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pDASH = pMDASH->hDASH;
	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	*format = DASH_CTRL_GetSegmentFormat(pController);

	return DI_ERR_OK;

}

/**
 * @b Function @b Description <br>
 * construct media information(pstMediaInfo) manually <br>
 * If MPD consists TS segments, just added duration information
 * If not(MP4), retrieves media information from MPD and segments
 *
 * @param[in] pDASH	: DASH master handle
 * @param[in] pstMediaInfo : Media information structure
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_GetMediaInfo (MEDIA_DASH_HANDLE_t pMDASH, DI_MEDIA_INFO_t *pstMediaInfo)
{
	HINT32 nVideoIndex = 0;
	HINT32 nAudioIndex = 0;
	HUINT32 loopcnt = 0;
	DASH_STRING_t tSTR;
	DASH_MASTER_HANDLE_t	pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_ERROR_e derr;

	PrintEnter;
	if( pMDASH == NULL )
	{
		PrintError("%s(%d) : Error> MEDIADASH HANDLE is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pDASH = pMDASH->hDASH;
	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* common config for TS and MP4F */
	pstMediaInfo->max_bitrate = DASH_CTRL_GetMaxBandwidth(pController, DASH_TIME_BASE_CONNECTING);
	pstMediaInfo->duration	= DASH_CTRL_GetMediaDuration(pController);
	if(pstMediaInfo->duration == 0 || DASH_CTRL_GetIsLiveUpdate(pController) == TRUE)
	{
		pstMediaInfo->duration = 0xFFFFFFFF;
	}

	/* config for MP4F */
	if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_MP4)
	{
		pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_MP4FRAGMENT;
		pstMediaInfo->pcr_pid	= 0x1fff; /* NULL_PID;*/
		pstMediaInfo->nprograms = 1;  /* Check */
		pstMediaInfo->ntracks = DASH_CTRL_GetTotalTracks(pController);
		for(loopcnt = 0 ; loopcnt < pstMediaInfo->ntracks ; loopcnt++)
		{
			switch(DASH_CTRL_GetTrackType(pController, loopcnt))
			{
				case DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY :
					pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_STREAM_DASH_DASHCodecToDICodec(DASH_CTRL_GetTrackCodec(pController, loopcnt), DASH_VIDEO_CODECS);
					if(pstMediaInfo->stVideoTrack[nVideoIndex].video_codec != 0)
					{
						pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = DASH_VIDEO_STREAM_ID;
						pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
						nVideoIndex++;
					}
					break;
				case DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY :
					tSTR = DASH_CTRL_GetTrackLang(pController, loopcnt);
					if(tSTR.str != NULL && tSTR.length> 0)
					{
						VK_strncpy(pstMediaInfo->stAudioTrack[nAudioIndex].uacAudioLang,tSTR.str,tSTR.length);
					}

					pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec =  P_STREAM_DASH_DASHCodecToDICodec(DASH_CTRL_GetTrackCodec(pController, loopcnt), DASH_AUDIO_CODECS);
					if(pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec != 0)
					{
						pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = DASH_AUDIO_STREAM_ID;
						pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
						nAudioIndex++;
					}
					break;
				case DASH_ADAPTATIONSET_TYPE_AV:
					pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_STREAM_DASH_DASHCodecToDICodec(DASH_CTRL_GetTrackCodec(pController, loopcnt), DASH_VIDEO_CODECS);
					if(pstMediaInfo->stVideoTrack[nVideoIndex].video_codec != 0)
					{
						pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = DASH_VIDEO_STREAM_ID;
						pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
						nVideoIndex++;
					}
					tSTR = DASH_CTRL_GetTrackLang(pController, loopcnt);
					if(tSTR.str != NULL && tSTR.length> 0)
					{
						VK_strncpy(pstMediaInfo->stAudioTrack[nAudioIndex].uacAudioLang,tSTR.str,tSTR.length);
					}
					pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec =  P_STREAM_DASH_DASHCodecToDICodec(DASH_CTRL_GetTrackCodec(pController, loopcnt), DASH_AUDIO_CODECS);
					if(pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec != 0)
					{
						pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = DASH_AUDIO_STREAM_ID;
						pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
						nAudioIndex++;
					}
					break;
				default :
					break;
			}
		}
		pstMediaInfo->ucTotalAudioTrack = nAudioIndex;
		pstMediaInfo->ucTotalVideoTrack = nVideoIndex;
		pstMediaInfo->ntracks = nVideoIndex + nAudioIndex;
	}

#if 0
	pstMediaInfo->nWrmHeaderLen = SSTR_CONTROLLER_GetDecodedProtectionHeaderLen(pSstr->hCtrl);
	pstMediaInfo->pucWrmHeader = SSTR_CONTROLLER_GetDecodedProtectionHeader(pSstr->hCtrl);
//	PrintDebug ("pstMediaInfo->nWrmHeaderLen %d\n",pstMediaInfo->nWrmHeaderLen);
	if(pstMediaInfo->nWrmHeaderLen > 0 && pstMediaInfo->pucWrmHeader != NULL)
	{
#if 0
		pstMediaInfo->nWrmHeaderLen = nProtectionHeaderLen+1;
		pstMediaInfo->pucWrmHeader = VK_MEM_Alloc(nProtectionHeaderLen+1);
		if(pstMediaInfo->pucWrmHeader != NULL)
		{
			VK_MEM_Memset(pstMediaInfo->pucWrmHeader,0x00,nProtectionHeaderLen+1);
			VK_MEM_Memcpy(pstMediaInfo->pucWrmHeader, pProtectionHeader, nProtectionHeaderLen);
			pstMediaInfo->nWrmHeaderLen = nProtectionHeaderLen;
			pstMediaInfo->is_drm = TRUE;
		}
#else
		pstMediaInfo->is_drm = TRUE;
		pstMediaInfo->eDrmType = DI_MEDIA_DRM_TYPE_PLAYREADY;
#endif
	}
#endif

#if 0
	for(int i = 0; i < pMedia->nStreamCnt ; i++)
	{
		if((pMedia->streamSet + i)->eType == SSTR_TYPE_VIDEO)
		{
			if(usMaxWidth < (pMedia->streamSet + i)->ulMaxWidth)
			{
				usMaxWidth = (pMedia->streamSet + i)->ulMaxWidth;
			}
			if(usMaxHeight < (pMedia->streamSet + i)->ulMaxHeight)
			{
				usMaxHeight = (pMedia->streamSet + i)->ulMaxHeight;
			}
			if(nVideoIndex < DI_MEDIA_DIVX_MAX_VIDEO_TRACK )
			{
				pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = SSTR_CONST_VIDEO_STREAM_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet)->FourCC);
				nVideoIndex++;
			}
/*
			for(int j = 0; j < (pMedia->streamSet + i)->ulQualityLevels || ; j++)
			{
				pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = SSTR_CONST_VIDEO_STREAM_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet + j)->FourCC);
				nVideoIndex++;
			}
*/
		}
		else if((pMedia->streamSet + i)->eType == SSTR_TYPE_AUDIO)
		{
			if(nAudioIndex < DI_MEDIA_DIVX_MAX_AUDIO_TRACK )
			{
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = SSTR_CONST_AUDIO_STREAM_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet)->FourCC);
				nAudioIndex++;
			}
/*
			for(int j = 0; j < (pMedia->streamSet + i)->ulQualityLevels ; j++)
			{
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = SSTR_CONST_AUDIO_STREAM_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet + j)->FourCC);
				nAudioIndex++;
			}
*/
		}
	}
#endif
	//pstMediaInfo->usVideoWidth = SSTR_CONTROLLER_GetMaxWidth(pSstr->hCtrl);
	//pstMediaInfo->usVideoHeight = SSTR_CONTROLLER_GetMaxHeight(pSstr->hCtrl);
	pstMediaInfo->usVideoWidth = 320;
	pstMediaInfo->usVideoHeight = 240;

	pstMediaInfo->audio_codec = pstMediaInfo->stAudioTrack[0].audio_codec;
	pstMediaInfo->video_codec = pstMediaInfo->stVideoTrack[0].video_codec;
	pstMediaInfo->audiodecodertype = DI_MEDIA_AUDIO_DECODER_MAIN;
	pstMediaInfo->videodecodertype = DI_MEDIA_VIDEO_DECODER_MAIN;
	pstMediaInfo->audio_stream_id = pstMediaInfo->stAudioTrack[0].audio_stream_id;
	pstMediaInfo->video_stream_id = pstMediaInfo->stVideoTrack[0].video_stream_id;
	pstMediaInfo->audio_pes_id = pstMediaInfo->stAudioTrack[0].audio_pes_id;
	pstMediaInfo->video_pes_id = pstMediaInfo->stVideoTrack[0].video_pes_id;
	pstMediaInfo->support_seek = 1;
	pstMediaInfo->support_trick = 0; /* not supported */
	PrintExit;

	return DI_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Download MP4 header(initialisation segment or partial media segment) and parsing then cache it
 *
 * @param[in] pDASH	: DASH master handle
 * @param[in] eStreamType : adaptationset type
 * @param[in] nBandwidth : base bandwidth
 * @param[in] tTime : scaled time (related to MPD)
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_MP4Preprocess (MEDIA_DASH_HANDLE_t pMDASH, DASH_ADAPTATIONSET_TYPE_e eStreamType, DASH_BANDWIDTH_t nBandwidth, DASH_TIME_t tTime)
{
	DASH_SEGMENTINFO_t tSegInfo;
	DASH_MASTER_HANDLE_t	pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_ERROR_e derr = DASH_ERR_OK;
	HUINT8 *pBuf = NULL;
	HUINT64 nContentLength = 0;
	HUINT32 unRetry = 0;
	UTIL_MP4_PARSER_t tInitSegment;
	HUINT32 szWritten = 0;

	if(pMDASH == NULL)
	{
		return DI_ERR_ERROR;
	}

	pDASH = pMDASH->hDASH;
	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Cannot get Controller handle\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(&tSegInfo, 0, sizeof(DASH_SEGMENTINFO_t));
	VK_MEM_Memset(&tInitSegment, 0x0, sizeof(UTIL_MP4_PARSER_t));
	/* find initialisation segment */
	derr = DASH_CTRL_FindSegmentInfo(pController, DASH_SEGMENT_INITIALISATION, eStreamType, nBandwidth, tTime, &tSegInfo);
	/* no initialisation segment */
	if(derr == DASH_ERR_INITSEGMENT_NONE)
	{
		PrintDebug("%s(%d) : Initialisation segment was not found - attempt to prove using real segment\n", __FUNCTION__, __LINE__);
		VK_MEM_Memset(&tSegInfo, 0, sizeof(DASH_SEGMENTINFO_t));
		derr = DASH_CTRL_FindSegmentInfo(pController, DASH_SEGMENT_MEDIA, eStreamType, nBandwidth, tTime, &tSegInfo);
		if(derr == DASH_ERR_ERROR)
		{
			PrintError("%s(%d) : Error> getting segment information was failed\n",__FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}
		/* partial segment download */
		PrintDebug("%s(%d) : media segment (%s)\n", __FUNCTION__, __LINE__, tSegInfo.pucSegmentURL);
		while((nContentLength = P_STREAM_DASH_Download((void *)tSegInfo.pucSegmentURL, (HINT8**)&pBuf, 0, DASH_DOWNLOAD_SIZE_FOR_PROBING)) <= 0)
		{
			PrintError("%s(%d) : Error> P_STREAM_Download Fail Retrying...\n",__FUNCTION__,__LINE__);
			if(unRetry > DASH_MAXIMUM_CONNECT_TRY)
			{
				PrintError("%s(%d) : Error> P_STREAM_Download Fail Exit...\n",__FUNCTION__,__LINE__);
				return DI_ERR_ERROR;
			}
			unRetry++;
		}
	}
	/* find initialisation segment */
	else if(derr == DASH_ERR_OK)
	{
		PrintDebug("%s(%d) : Initialisation segment (%s)\n", __FUNCTION__, __LINE__, tSegInfo.pucSegmentURL);
		while((nContentLength = P_STREAM_DASH_Download((void *)tSegInfo.pucSegmentURL, (HINT8**)&pBuf, tSegInfo.startoffset, tSegInfo.endoffset)) <= 0)
		{
			PrintError("%s(%d) : Error> P_STREAM_Download Fail Retrying...\n",__FUNCTION__,__LINE__);
			if(unRetry > DASH_MAXIMUM_CONNECT_TRY)
			{
				PrintError("%s(%d) : Error> P_STREAM_Download Fail Exit...\n",__FUNCTION__,__LINE__);
				return DI_ERR_ERROR;
			}
			unRetry++;
		}
	}
	else 	/* this conitition is not error case actually, just dash could not find segment */
	{
		PrintError("%s(%d) : Error> Can not process to get segment information\n",__FUNCTION__,__LINE__);
		return DI_ERR_OK;
	}
	/* Parsing MP4 header */
	UTIL_MP4_PARSER_Parse(&tInitSegment, (HINT8*)pBuf, nContentLength);
	PrintDebug("%s(%d) Attempt to write cache (ID : %d, Type : %d)\n",
		__FUNCTION__, __LINE__, tSegInfo.nBandwidth, tSegInfo.eTrackType);
	/* Write parsed mp4 header to cache */
	szWritten = P_STREAM_DASH_MP4CACHE_Write(pMDASH->InitData, &tInitSegment, sizeof(UTIL_MP4_PARSER_t), tSegInfo.nBandwidth, tSegInfo.eTrackType);
	while(szWritten == 0)
	{
		P_STREAM_DASH_MP4CACHE_Reclaim(pMDASH->InitData, tSegInfo.eTrackType);
		szWritten = P_STREAM_DASH_MP4CACHE_Write(pMDASH->InitData, &tInitSegment, sizeof(UTIL_MP4_PARSER_t), tSegInfo.nBandwidth, tSegInfo.eTrackType);
	}

	if(pBuf != NULL)
	{
		VK_MEM_Free(pBuf);
		pBuf = NULL;
	}

	return DI_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * External user event receiver (such as seek) <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] cmd : command value (in DASH, we use control function when cmd is only 'seek' command)
 * @param[in] val : seek position
 * @param[in] val2 : unused param
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_DASH_Control (MEDIA_DASH_HANDLE_t pMDASH, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	UNUSED_PARAM(val2);
	DI_ERR_CODE nRet = DI_ERR_OK;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_ERROR_e derr = DASH_ERR_OK;

	PrintEnter;

	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error> MEDIA DASH handle is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pDASH = pMDASH->hDASH;
	/*************************************************
	*  DASH Seek process
	*  DASH의 seek은 controller에 position을 설정 후
	*  User event를 발생시기면 내부적으로 seek pos의
	*  segment를 획득하여 buffering을 시작한다.
	**************************************************/
	if ( cmd == CMD_CONTROL_SEEK )
	{
		derr = DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error> cannot get pBufferMgr\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
		derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error> cannot get pController\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
		derr = DASH_MAIN_SetSeekPosition(pDASH, DASH_CTRL_ConvertMStoNonScaledTime(pController, val));
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error> cannot set seek pos\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
		/* send seek event to DASH */
		ADP_BUFFERMGR_SetReadLock(pBufferMgr, TRUE);
		derr = 	DASH_MAIN_SetOccuredUserEvent(pDASH, DASH_USER_EVENT_SEEK);
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error> cannot set seek event\n", __FUNCTION__, __LINE__);
			ADP_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
			DASH_MAIN_ClearUserEventForRead(pDASH);
			return DI_ERR_ERROR;
		}
		PrintDebug("Control function is going to wait for DASH_STATE_PAUSE\n");
		/* at this point, dash scheduler stops scheduling and get ready to buffer new seg */
		derr = DI_STREAM_DASH_WaitState(pMDASH, DASH_STATE_PAUSE);
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error> Some error has been occured during waiting DASH_STATE_PAUSE\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
		PrintDebug("DASH_STATE_PAUSE trigger!\n");
		/*
		   버퍼링 안된 영역으로 seek 혹은 EOF 에 도달한 경우 해당 이벤트는
		   scheduler의 Buffering state에서 처리한다.
		   :OUTBUF는 buffering state에서 갱신된 connecting time으로 새로운 segment를 스케쥴링 시작
		   :EOF는 buffering state에서 EOF segment를 생성하여 preconnector에 전달
		*/
		if(P_STREAM_DASH_GetEventResult(pMDASH) == DASH_EVENT_RESULT_SEEK_OUTBUF
			|| P_STREAM_DASH_GetEventResult(pMDASH) == DASH_EVENT_RESULT_EOF)
		{
			/* User event가 설정되면 scheduler은 user event prcess를 시작하고,
			   이 process가 완료되면 pause 상태로 진입한다. scheduler은 아래 호출을 통해 buffering 상태로 재진입한다 */
			derr = DI_STREAM_DASH_SetState(pMDASH, DASH_STATE_BUFFERING);
		}
		else
		{
			/* OUTBUF와 EOF와 같이 buffering 상태에서 추가 처리가 필요하지 않은 경우라면 이전 상태를 복구한다 */
			derr = DI_STREAM_DASH_RestoreState(pMDASH);
			P_STREAM_DASH_ClearEventResult(pMDASH);
		}
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error> Cannot change DASH state\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
	}
	else if ( cmd == CMD_CONTROL_SPEED )
	{
		/* not supported yet */
	}
	PrintExit;

	return nRet;

}

/**
 * @b Function @b Description <br>
 * Set main state of DASH <br>
 *
 * @param[in] pMDASH : Media DASH handle
 * @param[in] eState : DASH state
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DI_STREAM_DASH_SetState(MEDIA_DASH_HANDLE_t pMDASH, DASH_PROCESS_STATE_t eState)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	//VK_SEM_Get(e->ulSema);
	PrintDebug("%s(%d) DASH_PROCESS_STATE was changed (%d -> %d)\n", __FUNCTION__, __LINE__,pMDASH->state, eState);
	pMDASH->state = eState;
	//VK_SEM_Release(e->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get main state of DASH <br>
 *
 * @param[in] pMDASH : Media DASH handle
 *
 * @return	#  DASH state <br>
 */
DASH_PROCESS_STATE_t DI_STREAM_DASH_GetState(MEDIA_DASH_HANDLE_t pMDASH)
{
	DASH_PROCESS_STATE_t state;
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		return -1;
	}
	//VK_SEM_Get(e->ulSema);
	state = pMDASH->state;
	//VK_SEM_Release(e->ulSema);

	return state;
}

/**
 * @b Function @b Description <br>
 * Waiting for given state <br>
 *
 * @param[in] pMDASH: Media DASH handle
 * @param[in] eState : state which caller will be waiting for
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DI_STREAM_DASH_WaitState(MEDIA_DASH_HANDLE_t pMDASH, DASH_PROCESS_STATE_t eState)
{
	DASH_PROCESS_STATE_t d_state;

	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	PrintDebug("%s(%d) WaitState (%d)\n", __FUNCTION__, __LINE__,eState);
	while(DI_STREAM_DASH_GetState(pMDASH) != eState)
	{
		d_state = DI_STREAM_DASH_GetState(pMDASH);
		if( d_state == DASH_STATE_ERROR)
		{
			return DASH_ERR_ERROR;
		}
		VK_TASK_Sleep(DASH_MINIMUM_TASK_SLEEP);
	}
	PrintDebug("%s(%d) WaitState (%d)...OK\n", __FUNCTION__, __LINE__,eState);
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Save current state <br>
 *
 * @param[in] pMDASH : Media DASH handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DI_STREAM_DASH_SaveState(MEDIA_DASH_HANDLE_t pMDASH)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pMDASH->ulSema);
	pMDASH->prevstate = pMDASH->state;
	PrintDebug("%s(%d) DASH_PROCESS_STATE was saved (%d)\n", __FUNCTION__, __LINE__,pMDASH->prevstate);
	VK_SEM_Release(pMDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Restore previous state of DASH <br>
 *
 * @param[in] pMDASH : Media DASH handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DI_STREAM_DASH_RestoreState(MEDIA_DASH_HANDLE_t pMDASH)
{
	if(pMDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pMDASH->ulSema);
	pMDASH->state = pMDASH->prevstate;
	PrintDebug("%s(%d) DASH_PROCESS_STATE was restored (%d)\n", __FUNCTION__, __LINE__,pMDASH->prevstate);
	VK_SEM_Release(pMDASH->ulSema);

	return DASH_ERR_OK;
}

/* DASH VFIOs */

/* this will be called by TS Probing, in case of MP4, no need to probe */
DI_ERR_CODE P_STREAM_DASH_Open(const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle)
{
	HINT32  vRet = DASH_ERR_OK;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_SEGMENTINFO_t tSegInfo;
	DASH_ERROR_e derr = DASH_ERR_OK;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	HUINT8 *pUrl= NULL;
	HUINT64 llPrevVideoConnectingTime = 0ULL;
	HUINT64 llPrevAudioConnectingTime = 0ULL;
	HTTP_ACCESS_SYS_t *pstHttpSys = NULL;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif

	PrintEnter;
	UNUSED_PARAM(path);
	UNUSED_PARAM(mode);
	UNUSED_PARAM(ptHandle);

	/* Searching IPTUNER handle using saved file name */
	pHTuner = DI_IPTUNER_POOL_FindByUrl ((HINT8*)path);
	/*
	 * IPTuner Handle must be tied up with VFIO Handle
	 * because VFIO Functions need to be refer to each other.
	 */
	ptHandle->pTunerHandle = pHTuner;

	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		goto __vfio_error_in_device;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> DASH Main handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pMDASH->IOmode = DASH_IO_TYPE_PROBE;
	pDASH = pMDASH->hDASH;


	/* Waiting for downloading first MPD.
	   To be honest, no need to wait for this. It is just assuerance */
	PrintDebug("%s(%d) Waiting for MPD parsing completion\n", __FUNCTION__, __LINE__);
	derr = DI_STREAM_DASH_WaitState(pMDASH, DASH_STATE_READY);
	if(derr == DASH_ERR_ERROR)
	{
		PrintError ("%s(%d) Error> There are some trouble while task running !!\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Cannot get Controller handle\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
	derr = DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Cannot get BufferMgr handle\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
#if 1
	derr = DASH_CTRL_SetCurrentConnectingTime(pController, 0, DASH_CONNECTTIME_VIDEO);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Set connecting pos is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
	derr = DASH_CTRL_SetCurrentConnectingTime(pController, 0, DASH_CONNECTTIME_AUDIO);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Set connecting pos is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
	derr = DASH_CTRL_SetCurrentBufferingTime(pController, 0);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Set buffering pos is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}
#endif
	derr = DASH_CTRL_SetCurrentLocale(pController, DASH_LOCALE_TYPE_NONE);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Set Locale is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		goto __open_error;
	}

	/* Find initialisation segment or get first real segment if initialisation segment is not exist */
	VK_MEM_Memset(&tSegInfo, 0, sizeof(DASH_SEGMENTINFO_t));
	derr = DASH_CTRL_GetSegmentInfo(pController, DASH_SEGMENT_INITIALISATION, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, &tSegInfo);
	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) : Error> Can not process to get segment information\n",__FUNCTION__,__LINE__);
	}
	else //if(derr == DASH_ERR_INITSEGMENT_NONE)
	{
		PrintDebug("%s(%d) : attempt to prove using real segment\n", __FUNCTION__, __LINE__);
		llPrevVideoConnectingTime = DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_VIDEO);
		llPrevAudioConnectingTime = DASH_CTRL_GetCurrentConnectingTime(pController, DASH_CONNECTTIME_AUDIO);
		VK_MEM_Memset(&tSegInfo, 0, sizeof(DASH_SEGMENTINFO_t));
		derr = DASH_CTRL_GetSegmentInfo(pController, DASH_SEGMENT_MEDIA, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, &tSegInfo);
		if(derr == DASH_ERR_ERROR)
		{
			PrintError("%s(%d) : Error> getting segment information was failed\n",__FUNCTION__,__LINE__);
			goto __open_error;
		}
		PrintDebug("%s(%d) : 1st media segment (%s)\n", tSegInfo.pucSegmentURL);
		DASH_CTRL_SetCurrentConnectingTime(pController, llPrevVideoConnectingTime, DASH_CONNECTTIME_VIDEO);
		DASH_CTRL_SetCurrentConnectingTime(pController, llPrevAudioConnectingTime, DASH_CONNECTTIME_AUDIO);

		/* Make Stream Core Handler */
		pAccess = (STREAM_ACCESS_t*)VK_MEM_Alloc(sizeof(STREAM_ACCESS_t));
		if(pAccess == NULL)
		{
			PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
			goto __vfio_error_in_device;
		}
		VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));

		/*
		 * We must store a STREAM Handler in iptuner handle .
		 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
		 *
		 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
		 *				   --  STREAM Handler ( include protocol controller)
		 */
		ptHandle->pTunerHandle = (void*) pHTuner;
		pHTuner->pHStream = (void*) pAccess;

#if SUPPORT_HUMAX_COOKIE
		VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
		UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
		if(tUrl.psz_cookie != NULL)
		{
			psCookie = VK_StrDup(tUrl.psz_cookie);
		}
		UTIL_UrlClean(&tUrl);
#endif

		pAccess->psz_path = VK_StrDup ((void *)tSegInfo.pucSegmentURL );
#if SUPPORT_HUMAX_COOKIE
		if(psCookie != NULL)
		{
			pAccess->psz_cookie = psCookie;
		}
#endif
		if(pAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;
			goto __vfio_error_in_device;
		}
		/* support YouView Specific UserAgent String */
		if(pHTuner->pUserAgentString != NULL)
		{
			pAccess->psz_user_agent = VK_StrDup ((void *)pHTuner->pUserAgentString);
			if(pAccess->psz_user_agent == NULL)
			{
				PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
				ptHandle->ucAvailable = 0;
				goto __vfio_error_in_device;
			}
		}
		else /* error case : using default HUMAX UserAgent */
		{
			pAccess->psz_user_agent = VK_StrDup ((void *)DI_IPTUNER_USER_AGENT_STRING);
		}
#if defined (SUPPORT_TLS_V1_PROTOCOL)
		if(pHTuner->pTls_ca_name != NULL)
		{
			pAccess->psz_ca_name = VK_StrDup ( pHTuner->pTls_ca_name);
		}

		if(pHTuner->pTls_ca_path != NULL)
		{
			pAccess->psz_ca_path = VK_StrDup ( pHTuner->pTls_ca_path);
		}

		if(pHTuner->pTls_certi_path != NULL)
		{
			pAccess->psz_certi_path = VK_StrDup ( pHTuner->pTls_certi_path);
		}

		if(pHTuner->pTls_prikey_path != NULL)
		{
			pAccess->psz_prikey_path = VK_StrDup ( pHTuner->pTls_prikey_path);
		}

		if(pHTuner->pTls_pripw != NULL)
		{
			pAccess->psz_prikey_passwd = VK_StrDup ( pHTuner->pTls_pripw);
		}
#endif
		if ( DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess))
		{
			/***********************
			 * STREAM Open fails
			 ***********************/
			pAccess->b_alive = TRUE;
			ptHandle->ucAvailable = 0;

			if(pAccess->psz_path != NULL)
			{
				VK_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			if(pAccess->psz_user_agent != NULL)
			{
				VK_MEM_Free(pAccess->psz_user_agent);
				pAccess->psz_user_agent =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				VK_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
#if defined (SUPPORT_TLS_V1_PROTOCOL)
			if(pAccess->psz_ca_name != NULL)
			{
				VK_MEM_Free(pAccess->psz_ca_name);
				pAccess->psz_ca_name = NULL;
			}

			if(pAccess->psz_ca_path != NULL)
			{
				VK_MEM_Free(pAccess->psz_ca_path);
				pAccess->psz_ca_path = NULL;
			}

			if(pAccess->psz_certi_path != NULL)
			{
				VK_MEM_Free(pAccess->psz_certi_path);
				pAccess->psz_certi_path = NULL;
			}

			if(pAccess->psz_prikey_path != NULL)
			{
				VK_MEM_Free(pAccess->psz_prikey_path);
				pAccess->psz_prikey_path = NULL;
			}

			if(pAccess->psz_prikey_passwd != NULL)
			{
				VK_MEM_Free(pAccess->psz_prikey_passwd);
				pAccess->psz_prikey_passwd = NULL;
			}
#endif
			PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			vRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			/*************************
			 * Succesful Open
			 *************************/
			PrintData ("%s(%d) : i_update		= %d \n",__FUNCTION__,__LINE__,pAccess->info.i_update);
			PrintData ("%s(%d) : i_size 		= %"PRId64"\n",__FUNCTION__,__LINE__,pAccess->info.i_size);
			PrintData ("%s(%d) : i_pos			= %"PRId64"\n",__FUNCTION__,__LINE__,pAccess->info.i_pos);
			PrintData ("%s(%d) : b_eof			= %d \n",__FUNCTION__,__LINE__,pAccess->info.b_eof);
			PrintData ("%s(%d) : i_title		= %d \n",__FUNCTION__,__LINE__,pAccess->info.i_title);
			PrintData ("%s(%d) : i_seekpoint	= %d \n",__FUNCTION__,__LINE__,pAccess->info.i_seekpoint);
			PrintData ("%s(%d) : b_prebuffered	= %d \n",__FUNCTION__,__LINE__,pAccess->info.b_prebuffered);

			if((pAccess->info.i_size > 0) && (pAccess->info.i_size < 4*1024))
			{
				/******************
				 * Abnormal Case
				 ******************/
				PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				PrintError("%s(%d) : Error> Invalid File Size(File Size = %"PRId64".\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
				pAccess->b_alive = TRUE;
				ptHandle->ucAvailable = 0;

				(void)STREAM_HTTP_Close (pAccess);

				if(pAccess->psz_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_path);
					pAccess->psz_path =NULL;
				}

				if(pAccess->psz_user_agent != NULL)
				{
					VK_MEM_Free(pAccess->psz_user_agent);
					pAccess->psz_user_agent =NULL;
				}

				/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
				if(pAccess->psz_cookie != NULL)
				{
					VK_MEM_Free(pAccess->psz_cookie);
					pAccess->psz_cookie =NULL;
				}
				/* else { continue; } */
#endif

#if defined (SUPPORT_TLS_V1_PROTOCOL)
				if(pAccess->psz_ca_name != NULL)
				{
					VK_MEM_Free(pAccess->psz_ca_name);
					pAccess->psz_ca_name = NULL;
				}

				if(pAccess->psz_ca_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_ca_path);
					pAccess->psz_ca_path = NULL;
				}

				if(pAccess->psz_certi_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_certi_path);
					pAccess->psz_certi_path = NULL;
				}

				if(pAccess->psz_prikey_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_prikey_path);
					pAccess->psz_prikey_path = NULL;
				}

				if(pAccess->psz_prikey_passwd != NULL)
				{
					VK_MEM_Free(pAccess->psz_prikey_passwd);
					pAccess->psz_prikey_passwd = NULL;
				}
#endif
				goto __vfio_error_in_device;
			}
			else
			{
				/******************
				 * Normal Case
				 ******************/
				pstHttpSys = (HTTP_ACCESS_SYS_t*) pAccess->p_sys;
				ptHandle->ucAvailable = 1;
				ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
				pHTuner->ulDNPFileLength = pAccess->info.i_size;
				PrintData ("%s(%d) : callback unhandle=%d\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

				/* Create Semaphore to control a vfio functions exclusively.*/
				if ( VK_OK != VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE))
				{
					/**************************
					 * Semaphore creation fail
					 **************************/
					(void)STREAM_HTTP_Close (pAccess);

					if(pAccess->psz_path != NULL)
					{
						VK_MEM_Free(pAccess->psz_path);
						pAccess->psz_path =NULL;
					}

					if(pAccess->psz_user_agent != NULL)
					{
						VK_MEM_Free(pAccess->psz_user_agent);
						pAccess->psz_user_agent =NULL;
					}
					/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
					if(pAccess->psz_cookie != NULL)
					{
						VK_MEM_Free(pAccess->psz_cookie);
						pAccess->psz_cookie =NULL;
					}
					/* else { continue; } */
#endif

#if defined (SUPPORT_TLS_V1_PROTOCOL)
					if(pAccess->psz_ca_name != NULL)
					{
						VK_MEM_Free(pAccess->psz_ca_name);
						pAccess->psz_ca_name = NULL;
					}

					if(pAccess->psz_ca_path != NULL)
					{
						VK_MEM_Free(pAccess->psz_ca_path);
						pAccess->psz_ca_path = NULL;
					}

					if(pAccess->psz_certi_path != NULL)
					{
						VK_MEM_Free(pAccess->psz_certi_path);
						pAccess->psz_certi_path = NULL;
					}

					if(pAccess->psz_prikey_path != NULL)
					{
						VK_MEM_Free(pAccess->psz_prikey_path);
						pAccess->psz_prikey_path = NULL;
					}

					if(pAccess->psz_prikey_passwd != NULL)
					{
						VK_MEM_Free(pAccess->psz_prikey_passwd);
						pAccess->psz_prikey_passwd = NULL;
					}
#endif
					PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
					goto __vfio_error_in_device;
				}

				/*************************************************************
				 * A decision of TIMELINE methods from YouView Content Server
				 * 주의 !!
				 *	   timeline 결정은 probing time 에만 시도해야한다.
				 *	   본 vfio open function 은 어디서든 항상 불리는 function 이므로
				 *	   status 를 비교하지 않으면 호출될때마다 timeline 이 바뀌게된다.
				 *************************************************************/
				if (pHTuner->eDownloadType != DI_IPTUNER_DOWNLOADING)
				{
					if(pHTuner->pIndexTimeLineUrl )
					{
						VK_MEM_Free (pHTuner->pIndexTimeLineUrl);
						pHTuner->pIndexTimeLineUrl = NULL;
					}

					if(pHTuner->pIndexRoom)
					{
						VK_MEM_Free (pHTuner->pIndexRoom);
						pHTuner->pIndexRoom = NULL;
						pHTuner->nIndexRoomSize=0;
					}

					if(pAccess->psz_index_file != NULL)
					{
						pHTuner->eTimeLine = DI_IPTUNER_TIMELINE_INDEX_FILE;
						pHTuner->pIndexTimeLineUrl = VK_StrDup (pAccess->psz_index_file);
						PrintError ("%s(%d) Info> Using X-IndexFileHeader(%s) for iptuner-timeline \n"
								,__FUNCTION__
								,__LINE__,
								pAccess->psz_index_file);
					}
					else if(pAccess->psz_bps_value != NULL)
					{
						pHTuner->eTimeLine = DI_IPTUNER_TIMELINE_BYTESPERSECOND;
						pHTuner->nTimeLineBPS = (HUINT32) VK_atoi (pAccess->psz_bps_value);
						PrintError ("%s(%d) Info> Using X-BytesPerSecond Value (%d) for iptuner-timeline \n"
								,__FUNCTION__
								,__LINE__
								,pHTuner->nTimeLineBPS);
					}
					else
					{
						pHTuner->eTimeLine = DI_IPTUNER_TIMELINE_STREAM;
						PrintError ("%s(%d) Info> Using StreamItself for iptuner-timeline \n",__FUNCTION__,__LINE__);
					}
				}
			} /* normal end */
		}
		DASH_CTRL_SetProbeMethod(pController, DASH_PROBE_REALSEGMENT);
	}
	return DI_ERR_OK;

__vfio_error_in_device:
#if 0
	if(pController->InitBuf != NULL)
	{
		mem_file_close(pController->InitBuf);
		pController->InitBuf = NULL;
	}
#endif
	if(pAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pAccess);
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}

		if(pAccess->psz_user_agent != NULL)
		{
			VK_MEM_Free(pAccess->psz_user_agent);
			pAccess->psz_user_agent =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			VK_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

#if defined (SUPPORT_TLS_V1_PROTOCOL)
		if(pAccess->psz_ca_name != NULL)
		{
			VK_MEM_Free(pAccess->psz_ca_name);
			pAccess->psz_ca_name = NULL;
		}

		if(pAccess->psz_ca_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_ca_path);
			pAccess->psz_ca_path = NULL;
		}

		if(pAccess->psz_certi_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_certi_path);
			pAccess->psz_certi_path = NULL;
		}

		if(pAccess->psz_prikey_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_prikey_path);
			pAccess->psz_prikey_path = NULL;
		}

		if(pAccess->psz_prikey_passwd != NULL)
		{
			VK_MEM_Free(pAccess->psz_prikey_passwd);
			pAccess->psz_prikey_passwd = NULL;
		}
#endif
		VK_MEM_Free(pAccess);
		pAccess = NULL;
	}
	return VFIO_ERROR_IN_DEVICE;

__open_error:
#if 0
	if(pController->InitBuf != NULL)
	{
		mem_file_close(pController->InitBuf);
		pController->InitBuf = NULL;
	}
#endif
	return DI_ERR_ERROR;
}

DI_ERR_CODE P_STREAM_DASH_Close(VFIO_Handle_t tHandle)
{
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_MPDPARSER_HANDLE_t pMPD = NULL;
	DASH_PROBE_METHOD_e method;
	DASH_SEGMENT_FORMAT_e format;
	DASH_ERROR_e derr = DASH_ERR_OK;
	int ret = VFIO_ERROR_SUCCESS;

	pHTuner = tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr== NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(pMDASH->IOmode == DASH_IO_TYPE_PROBE)
	{
		//Probe mode에서 read 처리
		method = DASH_CTRL_GetProbeMethod(pController);
		if(method == DASH_PROBE_INITIALISATION)
		{
#if 0
			(void)mem_file_close(pController->InitBuf);
#endif
		}
		else if(method == DASH_PROBE_REALSEGMENT)
		{
			derr = DASH_MAIN_GetMPDParserHandle(pDASH, &pMPD);
			if(derr != DASH_ERR_OK)
			{
				PrintError("%s(%d) : Error> MPD handle is NULL.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return DI_ERR_ERROR;
			}
			format = pMPD->format;
			if(format == DASH_SEGMENT_TS)
			{
				/* Now we just get the Stream handler */
				pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
				if (NULL == pAccess)
				{
					PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
						, __FUNCTION__
						, __LINE__);
					return VFIO_ERROR_FAIL;
				}

				/* In multi-open case, the http session should be closed only once. */
				if(pAccess->b_alive == FALSE)
				{
					PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
					goto success;
				}

				if(VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
				{
					PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d), Maybe this tuner wasn't opened.\n"
							,__FUNCTION__
							,__LINE__
							,(HUINT32)tHandle.pTunerHandle
							,tHandle.tFile.callback.unHandle);
					return VFIO_ERROR_FAIL;
				}

				(void) STREAM_HTTP_Close ( pAccess );

				pAccess->b_alive = FALSE;

				if(pAccess->psz_path != NULL)
				{
					VK_MEM_Free (pAccess->psz_path);
					pAccess->psz_path =NULL;
				}

#ifdef SUPPORT_ERR_EVENT /* Error Event Callback */
				if(pAccess->callback != NULL)
				{
					VK_MEM_Free (pAccess->callback);
					pAccess->callback = NULL;
				}
#endif

				if(pAccess->psz_user_agent != NULL)
				{
					VK_MEM_Free(pAccess->psz_user_agent);
					pAccess->psz_user_agent =NULL;
				}
				/* else { continue; } */

#if SUPPORT_HUMAX_COOKIE
				if(pAccess->psz_cookie != NULL)
				{
					VK_MEM_Free(pAccess->psz_cookie);
					pAccess->psz_cookie =NULL;
				}
				/* else { continue; } */
#endif

#if defined (SUPPORT_TLS_V1_PROTOCOL)
				if(pAccess->psz_ca_name != NULL)
				{
					VK_MEM_Free(pAccess->psz_ca_name);
					pAccess->psz_ca_name = NULL;
				}

				if(pAccess->psz_ca_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_ca_path);
					pAccess->psz_ca_path = NULL;
				}

				if(pAccess->psz_certi_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_certi_path);
					pAccess->psz_certi_path = NULL;
				}

				if(pAccess->psz_prikey_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_prikey_path);
					pAccess->psz_prikey_path = NULL;
				}

				if(pAccess->psz_prikey_passwd != NULL)
				{
					VK_MEM_Free(pAccess->psz_prikey_passwd);
					pAccess->psz_prikey_passwd = NULL;
				}
#endif
				(void)VK_SEM_Release (pAccess->ulStreamSema);
				(void)VK_SEM_Destroy (pAccess->ulStreamSema);
				pAccess->ulStreamSema = 0;
	success:
				if(pHTuner->pHStream != NULL)
				{
					VK_MEM_Free (pHTuner->pHStream);
					pHTuner->pHStream = NULL;
				}
			}
		}
	}
	else if(pMDASH->IOmode == DASH_IO_TYPE_PLAYBACK)
	{
		DI_STREAM_DASH_SetState(pHTuner->hAbr, DASH_STATE_PAUSE);
	}
	return ret;
}

DI_ERR_CODE P_STREAM_DASH_Read(VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	UNUSED_PARAM(tHandle);
	HBOOL bIsEOS = FALSE;
	size_t sz_Try = size*nmemb;
	size_t cur=0;
	size_t sz_Read = 0;
	size_t sz_Temp = 0;
	size_t sz_Real = 0;
	int nRead=0;
	int ret = VFIO_ERROR_SUCCESS;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_MPDPARSER_HANDLE_t pMPD = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	ADP_BUFFERMGR_STATUS_t pStatus;
	DASH_PROBE_METHOD_e	method;
	DASH_SEGMENT_FORMAT_e format;
	DASH_ERROR_e derr = DASH_ERR_OK;
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	HINT32 n_ReturnedValue = 0;
	HINT32 nRetryStreamReadCount =0;

	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(pMDASH->IOmode == DASH_IO_TYPE_PROBE)
	{
		method = DASH_CTRL_GetProbeMethod(pController);
		if(method == DASH_PROBE_INITIALISATION)
		{
#if 0
			if(pController->InitBuf != NULL)
			{
				*ptReadSize = mem_file_read(pController->InitBuf, ptr, sz_Try);
				if(*ptReadSize == -1)
				{
					*ptReadSize = 0;
				}
				return ret;
			}
#endif
		}
		else if(method == DASH_PROBE_REALSEGMENT)
		{
			derr = DASH_MAIN_GetMPDParserHandle(pDASH, &pMPD);
			if(derr != DASH_ERR_OK)
			{
				PrintError("%s(%d) : Error> MPD handle is NULL.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return DI_ERR_ERROR;
			}
			format = pMPD->format;
			if(format == DASH_SEGMENT_MP4)
			{
				/* this condition does not used */ ;
			}
			/* Now we just get the Stream handler */
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
			if (NULL == pAccess)
			{
				PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
				, __FUNCTION__, __LINE__);
				return VFIO_ERROR_FAIL;
			}

			if (VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
			{
				PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d)\n"
				,__FUNCTION__
				,__LINE__
				,(HUINT32)tHandle.pTunerHandle
				,tHandle.tFile.callback.unHandle);
				return VFIO_ERROR_FAIL;
			}

			if(pAccess->info.i_size < pAccess->info.i_pos + sz_Try)
			{
				sz_Try = pAccess->info.i_size - pAccess->info.i_pos;
			}

			if(sz_Try > size*nmemb)
			{
				PrintDebug("######### [%s(%d)] Error> Size Invalid(%u, %"PRId64",%"PRId64") #######\n", __FUNCTION__,__LINE__, sz_Try, pAccess->info.i_size, pAccess->info.i_pos);
				sz_Try = size*nmemb;
			}
			do
			{
				nRetryStreamReadCount = 0;
retry_read:
				n_ReturnedValue = STREAM_HTTP_Read ( pAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
				/* Read Error : should be exit immediately */
				if (n_ReturnedValue == 0 || n_ReturnedValue == DI_IPT_ETIMEOUT)
				{
					/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
					 *	this function should be returned immediately.
					 * If not so, the session may be blocked by retrying logic. Be careful.
					 */
					if(pAccess->b_immediately_exit == TRUE)
					{
						pAccess->b_retrying = FALSE;
						pAccess->b_alive = FALSE;
						ret = VFIO_ERROR_IN_DEVICE;

						PrintError("%s(%d) Error> STREAM_HTTP_Read Fail!!\n", __FUNCTION__, __LINE__);
						goto stream_read_error;
					}

					if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
					{
						PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
						break;
					}
					/* Something happened in STREAM Controller , Let's try again */
					else
					{
						nRetryStreamReadCount ++;
						if(nRetryStreamReadCount > 100)
						{
							*ptReadSize = sz_Real;
							nRetryStreamReadCount = 0;
							PrintError("%s(%d) Error> Retry Time Over (%d)!!\n", __FUNCTION__, __LINE__, nRetryStreamReadCount );
							goto stream_read_error;
						}

						PrintError("%s(%d) Retry> (RetryCount=%d)...\n", __FUNCTION__, __LINE__, nRetryStreamReadCount );
						goto retry_read;
					}

				}
				else if (n_ReturnedValue < 0)
				{
					*ptReadSize = sz_Real;
					ret = VFIO_ERROR_IN_DEVICE;
					PrintError("%s(%d) Error> STREAM_HTTP_Read Fail!!\n", __FUNCTION__, __LINE__);
					goto stream_read_error;
				}
				/* EOF or Error */
				sz_Real += n_ReturnedValue;
			}
			while (sz_Real < sz_Try);

			*ptReadSize = sz_Real;

stream_read_error:
			(void)VK_SEM_Release (pAccess->ulStreamSema);
		}
		else
		{
			PrintError("%s(%d) : Error> Probe method is invalid\n",__FUNCTION__,__LINE__);
			PrintExit;
			return DI_ERR_ERROR;
		}

		return ret;
	}
	else if(pMDASH->IOmode == DASH_IO_TYPE_PLAYBACK)
	{
		derr = DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) : Error> BufferMgr handle is NULL.\n",__FUNCTION__,__LINE__);
			PrintExit;
			return DI_ERR_ERROR;
		}
		/* buffer read in normal case*/
		sz_Read = sz_Try;
		VK_MEM_Memset(ptr,0x00,sz_Read);
		while(sz_Read > 0)
		{
			if(pHTuner->bExitRead)
			{
				PrintDebug("%s(%d) : Debug> Forced Exit.\n",__FUNCTION__,__LINE__);
				break;
			}
#if 1
			if(DASH_MAIN_GetOccuredUserEventForRead(pDASH) == DASH_USER_EVENT_SEEK)
			{
				if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_TS)
				{
					/* TS 일 경우 Waiting IO를 피하기 위해 요청한 사이즈에 대해 0xff 를 채워서 리턴한다 */
					cur = sz_Try;
					VK_MEM_Memset(ptr,0xff,sz_Try);
					break;
				}
				else if(DASH_CTRL_GetSegmentFormat(pController) == DASH_SEGMENT_MP4)
				{
					cur = 0;
				}
			}
#endif

			ADP_BUFFERMGR_GetStatus(pBufferMgr, &pStatus);
			if(pStatus.ullReadableSize < sz_Read)
			{
				if(pHTuner->eBufferingStatus > DI_IPTUNER_BUFFERING_START)
				{
					sz_Temp = pStatus.ullReadableSize;
					if(sz_Temp == 0)
					{
						if(pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_PAUSE)
						{
							if(DASH_CTRL_GetIsEndOfStream(pController) == TRUE)
							{
								DASH_CTRL_SetIsReadCompletion(pController, TRUE);
								break;
							}
							else
							{
								VK_TASK_Sleep(10);
								continue;
							}
						}
						else if(pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_STOP||
						pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_COMPLETE)
						{
							DASH_CTRL_SetIsReadCompletion(pController, TRUE);
							break;
						}
					}
					if(sz_Temp < sz_Read)
					{
						sz_Read = sz_Temp;
						bIsEOS = TRUE;
					}
				}
				else
				{
					VK_TASK_Sleep(10);
					continue;
				}
			}

			nRead = ADP_BUFFERMGR_Read(pBufferMgr, (HUINT8*)ptr + cur, sz_Read);
			ADP_BUFFERMGR_GetStatus(pBufferMgr, &pStatus);
			if(nRead == 0)
			{
				if(pHTuner->eBufferingStatus > DI_IPTUNER_BUFFERING_START)
				{
					break;
				}
				else
				{
					VK_TASK_Sleep(10);
					continue;
				}
			}
			if(pDASH->bDump == TRUE)
			{
				FILE *fp = fopen("/read_dump.bin","a+");
				if(fp==NULL) exit(1);
				fwrite((void*)((HINT8*)ptr+cur), sizeof(HINT8), nRead, fp);
				VK_fflush(fp);
				fclose(fp);
			}
			cur+=nRead;
			sz_Read-=nRead;
		}
		if(!bIsEOS)
		{
			*ptReadSize = cur;
		}
		else
		{
			*ptReadSize = sz_Try;
		}
	}
	return DI_ERR_OK;

	return ret;
}

DI_ERR_CODE P_STREAM_DASH_Seek(VFIO_Handle_t tHandle, off_t offset, int whence)
{
	UNUSED_PARAM(tHandle);
	UNUSED_PARAM(offset);
	UNUSED_PARAM(whence);
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_MPDPARSER_HANDLE_t pMPD = NULL;
	DASH_PROBE_METHOD_e	method;
	DASH_SEGMENT_FORMAT_e format;
	DASH_ERROR_e derr;
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	HINT32 nSeekRetryCount = 0;

	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> DASH Controller is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(pMDASH->IOmode == DASH_IO_TYPE_PROBE)
	{
		method = DASH_CTRL_GetProbeMethod(pController);
		if(method == DASH_PROBE_INITIALISATION)
		{
#if 0
			if(pController->InitBuf != NULL)
			{
				mem_file_seek(pController->InitBuf, offset);
			}
#endif
		}
		else if(method == DASH_PROBE_REALSEGMENT)
		{
			derr = DASH_MAIN_GetMPDParserHandle(pDASH, &pMPD);
			if(derr != DASH_ERR_OK)
			{
				PrintError("%s(%d) : Error> MPD handle is NULL.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return DI_ERR_ERROR;
			}
			format = pMPD->format;
			if(format == DASH_SEGMENT_MP4)
			{
				// TODO: implement
			}
			else if(format == DASH_SEGMENT_TS)
			{
				/* Now we just get the Stream handler */
				pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
				if (NULL == pAccess)
				{
					PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
					, __FUNCTION__, __LINE__);
					return VFIO_ERROR_FAIL;
				}

				if ( VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
				{
					PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d)\n"
					,__FUNCTION__
					,__LINE__
					,(HUINT32)tHandle.pTunerHandle
					,tHandle.tFile.callback.unHandle);
					return VFIO_ERROR_FAIL;
				}

				PrintData ("%s(%d) : Tuner(%u) offset=%"PRId64", whence=%d\n"
						,__FUNCTION__
						,__LINE__
						, (unsigned int)pHTuner
						, offset
						, whence);


				/* POSIX Seek operation */
				if(whence == SEEK_END)
					offset = (off_t)pAccess->info.i_size-offset;

				PrintDebug("== Probe Seek\n");

				/***********************************
				*	If the connection is lost,
				*	i_pos has advanced but the actual pointer didn't move yet.
				***********************************/
				/* YouView Back-Off */
				/* Retrying ... */
				while( DI_IPT_SUCCESS != STREAM_HTTP_Seek ( pAccess , offset ))
				{
					nSeekRetryCount++;
					PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail... Retrying seek request (%d) times..\n"
					,__FUNCTION__
					,__LINE__
					,nSeekRetryCount);

					if ( FALSE == UTIL_SYS_GetLinkStatus () )
					{
						PrintError ("%s (%d) Debug> Network Cable unplugged !!\n",__FUNCTION__,__LINE__);
						(void)VK_SEM_Release (pAccess->ulStreamSema);
						return VFIO_ERROR_IN_DEVICE;
					}

					if(nSeekRetryCount > 20)
					{
						PrintError ("%s(%d) Error> STREAM_HTTP_Seek Retrying fail. Last Retrying count(%d) ...\n"
							,__FUNCTION__
							,__LINE__
							,nSeekRetryCount);
						(void)VK_SEM_Release (pAccess->ulStreamSema);
						return VFIO_ERROR_FAIL;
					}
				}

				(void)VK_SEM_Release (pAccess->ulStreamSema);
				return VFIO_ERROR_SUCCESS;
			}
		}
	}
	return nRet;
}

DI_ERR_CODE P_STREAM_DASH_Tell(VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_MPDPARSER_HANDLE_t pMPD = NULL;
	DASH_ERROR_e derr = DASH_ERR_OK;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	DASH_PROBE_METHOD_e method;
	DASH_SEGMENT_FORMAT_e format;

	PrintEnter;
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(pMDASH->IOmode == DASH_IO_TYPE_PROBE)
	{
		method = DASH_CTRL_GetProbeMethod(pController);
		if(method == DASH_PROBE_INITIALISATION)
		{
#if 0
			if(pController->InitBuf != NULL)
			{
				*plCurOffset = mem_file_tell(pController->InitBuf);
			}
#endif
		}
		else if(method == DASH_PROBE_REALSEGMENT)
		{
			derr = DASH_MAIN_GetMPDParserHandle(pDASH, &pMPD);
			if(derr != DASH_ERR_OK)
			{
				PrintError("%s(%d) : Error> MPD handle is NULL.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return DI_ERR_ERROR;
			}
			format = pMPD->format;
			if(format == DASH_SEGMENT_MP4)
			{
				// TODO: implement
			}
			else if(format == DASH_SEGMENT_TS)
			{
				pHTuner = tHandle.pTunerHandle;
				if (pHTuner == NULL)
				{
					tHandle.ucAvailable = 0;
					PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
					return VFIO_ERROR_IN_DEVICE;
				}
				/* Now we just get the Stream handler */
				pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
				if (NULL == pAccess)
				{
					PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
					, __FUNCTION__, __LINE__);
					return VFIO_ERROR_FAIL;
				}
				*plCurOffset = pAccess->info.i_pos;
			}
		}
	}
	else
	{
		*plCurOffset = pMDASH->ullVirtureOffset;
	}

	PrintExit;

	return VFIO_ERROR_SUCCESS;
}

DI_ERR_CODE P_STREAM_DASH_IsEOF(VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_MPDPARSER_HANDLE_t pMPD = NULL;
	DASH_ERROR_e derr = DASH_ERR_OK;
	DASH_PROBE_METHOD_e method;
	DASH_SEGMENT_FORMAT_e format;

	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(pMDASH->IOmode == DASH_IO_TYPE_PROBE)
	{
		method = DASH_CTRL_GetProbeMethod(pController);
		if(method == DASH_PROBE_INITIALISATION)
		{

		}
		else if(method == DASH_PROBE_REALSEGMENT)
		{
			derr = DASH_MAIN_GetMPDParserHandle(pDASH, &pMPD);
			if(derr != DASH_ERR_OK)
			{
				PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return DI_ERR_ERROR;
			}
			format = pMPD->format;
			if(format == DASH_SEGMENT_TS)
			{
				/* Now we just get the Stream handler */
				pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
				if (NULL == pAccess)
				{
					PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
					, __FUNCTION__, __LINE__);
					nRet = 0;
				}

			    if(TRUE == pAccess->info.b_eof)
				{
					nRet = -1; /* must be -1 like feof*/
				}
				return nRet;
			}
		}
	}
	else if(pMDASH->IOmode == DASH_IO_TYPE_PLAYBACK)
	{
		return nRet;
	}
	return nRet;
}

DI_ERR_CODE P_STREAM_DASH_OpenFromNexus(const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	UNUSED_PARAM (flags);
	UNUSED_PARAM (pUrl);
	UNUSED_PARAM (ptHandle);
	HINT32 nRet = DI_ERR_OK;
	DI_IPTUNER_t *pHTuner=NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;

	PrintEnter;
	pHTuner = DI_IPTUNER_POOL_FindByUrl ((HINT8*)pUrl);
	/*
	 * IPTuner Handle must be tied up with VFIO Handle
	 * because VFIO Functions need to be refer to each other.
	 */
	ptHandle->pTunerHandle = pHTuner;
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> DASH Handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	PrintData ("%s (%d) Buffering Start from OpenNexusCallback\n",__FUNCTION__,__LINE__);
	pHTuner->bExitRead = FALSE;
	if( pHTuner->eBufferingStatus < DI_IPTUNER_BUFFERING_STOP )
	{
		PrintExit;
		return nRet;
	}

	//if IO type is already playback, skip it
	if( pMDASH->IOmode == DASH_IO_TYPE_PLAYBACK)
	{
		PrintExit;
		return nRet;
	}
	pMDASH->IOmode = DASH_IO_TYPE_PLAYBACK;

	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
	/* waiting for DASH ready (prebuffering completion) */
	nRet = DI_STREAM_DASH_WaitState(pHTuner->hAbr, DASH_STATE_READY);
	if(nRet == DASH_ERR_ERROR)
	{
		PrintError ("%s(%d) Error> There are some trouble while task running !!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		PrintExit;
		return nRet;
	}

	/* Buffering start */
	DI_STREAM_DASH_SetState(pHTuner->hAbr, DASH_STATE_BUFFERING);

	PrintExit;
	return nRet;
}

off_t P_STREAM_DASH_SeekForNexus(VFIO_Handle_t tHandle, off_t offset, int whence)
{
	UNUSED_PARAM(whence);
	DI_IPTUNER_t *pHTuner=NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_BUFFERMGR_HANDLE_t pBufferMgr = NULL;
	DASH_ERROR_e derr;

	PrintEnter;

	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	if(offset == 0)
	{
		derr = DASH_MAIN_GetBufferMgrHandle(pDASH, &pBufferMgr);
		if(derr != DASH_ERR_OK)
		{
			PrintError("%s(%d) : Error> BufferMgr handle is NULL.\n",__FUNCTION__,__LINE__);
			PrintExit;
			return DI_ERR_ERROR;
		}
		/* sync DI SetPosition(seek) with DASH Module */
		ADP_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
		DASH_MAIN_ClearUserEventForRead(pDASH);
	}
	pMDASH->ullVirtureOffset = offset;
	return offset;
}

DI_ERR_CODE P_STREAM_DASH_BoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner=NULL;
	MEDIA_DASH_HANDLE_t pMDASH = NULL;
	DASH_MASTER_HANDLE_t pDASH = NULL;
	DASH_CONTROLLER_HANDLE_t pController = NULL;
	DASH_ERROR_e derr;

	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->hAbr== NULL)
	{
		PrintError("%s(%d) : Error> DASH Handler is NULL.\n",__FUNCTION__,__LINE__);
	}
	pMDASH = (MEDIA_DASH_HANDLE_t)pHTuner->hAbr;
	pDASH = pMDASH->hDASH;

	derr = DASH_MAIN_GetControllerHandle(pDASH, &pController);
	if(derr != DASH_ERR_OK)
	{
		PrintError("%s(%d) : Error> Controller handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(pMDASH->IOmode == DASH_IO_TYPE_PROBE)
	{
#if 0
		if(pController->InitBuf != NULL)
		{
			*file_size = mem_file_bound(pController->InitBuf);
		}
#endif
	}
	else
	{
		*file_size = 0xFFFFFFFFFFFFFFLL;
	}
	return nRet;
}

void DI_STREAM_DASH_RegisterCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;
	PrintEnter;

	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));
	stCBFunction.Open 			= (VFIOOpenCB)	P_STREAM_DASH_Open;
	stCBFunction.Close			= (VFIOCloseCB)	P_STREAM_DASH_Close;
	stCBFunction.Read			= (VFIOReadCB)	P_STREAM_DASH_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	P_STREAM_DASH_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	P_STREAM_DASH_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) P_STREAM_DASH_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 P_STREAM_DASH_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 P_STREAM_DASH_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) P_STREAM_DASH_BoundsForNexus;
	(void) VFIO_RegisterCallback (pUrl, stCBFunction);

	PrintExit;
	return;
}

void DI_STREAM_DASH_UnregisterCallback (const char *pUrl)
{
	HINT32 nRet=0;
	PrintEnter;
	nRet = VFIO_UnregisterCallback (pUrl);
	if(nRet<0)
	{
		PrintError("%s(%d) Error> DI_STREAM_DASH_UnregisterCallback Fail (%s)\n",__FUNCTION__,__LINE__,pUrl);
	}
	PrintExit;
	return;
}


