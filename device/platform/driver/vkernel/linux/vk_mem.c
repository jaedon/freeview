/**
 * vk_mem.c
*/

/**
 * @defgroup		VK_MEM
 * @author		Tae-Hyuk Kang
 * @note			Virsual Memory APIs
 * @file 			vk_mem.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>

#include "vkernel.h"

#define _______________________________________________________________________
#define ____MACRO_DEFINITION____

#define DEBUG_MODULE			TRACE_MODULE_VK_MEM

#ifdef MEM_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)				
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)			
#endif /* MEM_DEBUG */

#define	ALLOW_MEMORY_ALLOC_FAIL
#ifdef ALLOW_MEMORY_ALLOC_FAIL
#define VK_Panic()
#else
#define VK_Panic    ((void (*)(void))0x0)
#endif

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____
static int P_GetNumber(unsigned char *pucData, int *pnNum);

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____
static int P_GetNumber(unsigned char *pucData, int *pnNum)
{
	int nIndex;
	
	*pnNum = 0;
	nIndex = 0;

	while (pucData[nIndex])
	{
		if (pucData[nIndex] < '0' || pucData[nIndex] > '9')
		{
			return VK_ERROR;
		}

		*pnNum *= 10;
		*pnNum += pucData[nIndex] - '0';
		
		nIndex++;
	}

	return VK_OK;
}

#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____

int VK_MEM_Init(void)
{
#if defined(MEMORY_DEBUG)
	if (VK_DBGMEM_Init())
		return -VK_ERROR;
#endif

	return VK_OK;
}

#define UNALIGNED32(X, Y) (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#define BIGBLOCKSIZE    (sizeof (long) << 2)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

//8bit, 16bit, 32bit address = dest, 8bit data = c, 8bit = count*1, 16bit = count*2, 32bit = count*4
void *VK_MEM_Memset(void *dest, unsigned char c, unsigned long count )
{
#if 1
	return memset(dest, (int)c, count);
#else
	register unsigned long	i;
	unsigned char	*p8;
	unsigned long	*p32;
	unsigned long	val32;
	unsigned long	count32;

	// phase 1
	p8 = (unsigned char*)dest;
	for( i=0; i<count; i++)
	{
		if( (((int)p8) & 3) == 0 )
		{
			break;
		}
		*p8++ = c;
	}
	count -= i;

	// phase 2
	p32 = (unsigned long*)p8;
	count32 = count>>2;

	val32 = ((c) | (c<<8) | (c<<16) | (c<<24));
	i = count32;
	while(i >= 4)
	{
		*p32++ = val32;
		*p32++ = val32;
		*p32++ = val32;
		*p32++ = val32;
		i-=4;
	}
	while(i >= 1)
	{
		*p32++ = val32;
		i--;
	}

	// phase 3
	count -= (count32<<2);
	p8 = (unsigned char*)p32;
	for( i=0; i<count; i++)
	{
		*p8++ = c;
	}
	return dest;
#endif
}

//32bit address =- dest, 32bit valut = c, count
void *memset32by32(void *dest, unsigned long c, unsigned long count )
{
	register unsigned long	i;
	unsigned long	*p32;
	unsigned long	count32;

	// phase 1
	if( (((int)dest) & 3) && count & 3 )
	{
		PrintError("[memset32by32] error = %d\n", -1);
		return NULL;
	}

	// phase 2
	p32 = (unsigned long*)dest;
	count32 = count>>2;

	i = count32;
	while(i >= 4)
	{
		*p32++ = c;
		*p32++ = c;
		*p32++ = c;
		*p32++ = c;
		i-=4;
	}
	while(i >= 1)
	{
		*p32++ = c;
		i--;
	}

	return dest;
}

//8bit, 16bit, 32bit address = dest, count : 8bit = count*1, 16bit = count*2, 32bit = count*4
void *VK_MEM_Memcpy(void *dst0, void *src0, unsigned long len0)
{
#if 1
	return memcpy(dst0, src0, len0);
#else
	char *dst = dst0;
	const	char *src = src0;
	long *aligned_dst;
	const	long *aligned_src;
	unsigned int   len =  len0;

	if (!TOO_SMALL(len) && !UNALIGNED32 (src, dst))
	{
		aligned_dst = (long*)dst;
		aligned_src = (const long*)src;

		/* Copy 4X long words at a time if possible.  */
		while (len >= BIGBLOCKSIZE)
		{
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			len -= BIGBLOCKSIZE;
		}

		/* Copy one long word at a time if possible.  */
		while (len >= LITTLEBLOCKSIZE)
		{
			*aligned_dst++ = *aligned_src++;
			len -= LITTLEBLOCKSIZE;
		}

		/* Pick up any residual with a byte copier.  */
		dst = (char*)aligned_dst;
		src = (const char*)aligned_src;
	}

	while (len--)
		*dst++ = *src++;

	return dst0;
#endif
}

void *VK_MEM_Alloc(unsigned long size)
{
	return malloc(size);
}

int VK_MEM_Free(void *ap)
{
	free(ap);
	return 0;
}

void *VK_MEM_Calloc(unsigned long size)
{
	return calloc(size, sizeof(char));
}

void *VK_MEM_Realloc(void *p, unsigned long size)
{
	return realloc(p, size);
}

char *VK_StrDup(const char *p)
{
	char *pNewStr;
	unsigned long	strSize;

	if (p == NULL)
		return NULL;

	strSize = strlen(p);
	pNewStr = DD_MEM_Alloc(strSize+1);
	if (pNewStr == NULL)
		return NULL;
	strncpy(pNewStr, p, strSize);
	pNewStr[strSize] = 0;
	return pNewStr;
}

int VK_MEM_GetInfo(VK_MEM_INFO_t *pstMemInfo)
{
	int nRet = 0, nVkErr = VK_OK, nLineNum = 0;
	unsigned int ulMemTotal = 0, ulMemFree = 0, ulBuffers = 0, ulCached = 0, ulCommitted_AS = 0;

	unsigned char aucParam1[20];	/* name */
	unsigned char aucParam2[20];	/* size */
	unsigned char aucMemBuf[64]; 

	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL )
	{
		PrintError("%s:%d File open failed(%d)\n", __FUNCTION__, __LINE__, nRet);
		return VK_ERROR;
	}

	VK_memset(aucMemBuf, 0x0, 64);	
	
	while (fgets(aucMemBuf, 63, fp) != NULL)
	{
		VK_memset(aucParam1, 0x0, 20);
		VK_memset(aucParam2, 0x0, 20);
		
		nRet = sscanf(aucMemBuf, "%19s%19s", aucParam1, aucParam2);
		if(nRet != 2)
		{
			PrintError("%s:%d File read failed(%d)\n", __FUNCTION__, __LINE__, nRet);
			nVkErr = VK_ERROR;
			break;
		}
		
		if (aucMemBuf[strlen(aucMemBuf)-1] == '\n')
		{			
			nLineNum++;
		}

		/* Get MemTotal */
		if(nLineNum == 1 && (strncmp(aucParam1, "MemTotal", 8) == 0))
		{
			nVkErr = P_GetNumber(&(aucParam2[0]), &ulMemTotal);
			if(nVkErr != VK_OK)
			{
				PrintError("%s:%d Get number failed(%d)\n", __FUNCTION__, __LINE__, nVkErr);
				break;
			}
		}
		/* Get MemFree */
		else if(nLineNum == 2 && (strncmp(aucParam1, "MemFree", 7) == 0))
		{
			nVkErr = P_GetNumber(&(aucParam2[0]), &ulMemFree);
			if(nVkErr != VK_OK)
			{
				PrintError("%s:%d Get number failed(%d)\n", __FUNCTION__, __LINE__, nVkErr);
				break;
			}
		}
		/* Get Buffers */
		else if(nLineNum == 3 && (strncmp(aucParam1, "Buffers", 7) == 0))
		{
			nVkErr = P_GetNumber(&(aucParam2[0]), &ulBuffers);
			if(nVkErr != VK_OK)
			{
				PrintError("%s:%d Get number failed(%d)\n", __FUNCTION__, __LINE__, nVkErr);
				break;
			}
		}
		/* Get Cached */
		else if(nLineNum == 4 && (strncmp(aucParam1, "Cached", 6) == 0))
		{
			nVkErr = P_GetNumber(&(aucParam2[0]), &ulCached);
			if(nVkErr != VK_OK)
			{
				PrintError("%s:%d Get number failed(%d)\n", __FUNCTION__, __LINE__, nVkErr);
				break;
			}
		}
		/* Get Committed_AS */
		else if(nLineNum == 34 && (strncmp(aucParam1, "Committed_AS", 12) == 0))
		{
			nVkErr = P_GetNumber(&(aucParam2[0]), &ulCommitted_AS);
			if(nVkErr != VK_OK)
			{
				PrintError("%s:%d Get number failed(%d)\n", __FUNCTION__, __LINE__, nVkErr);
				break;
			}
		}

		VK_memset(aucMemBuf, 0x0, 64);
	}

	fclose(fp);

	pstMemInfo->ulMemTotal = ulMemTotal;
	pstMemInfo->ulMemFree = ulMemFree + ulBuffers + ulCached;
	pstMemInfo->ulCommitted_AS = ulCommitted_AS;

	return nVkErr;
}
