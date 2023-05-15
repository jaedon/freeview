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

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <assert.h>
#include <sys/types.h>

#include "vkernel.h"
#include "humax_bsu.h"

#include <linden_trace.h>

#define _______________________________________________________________________
#define ____MACRO_DEFINITION____

#define DEBUG_MODULE			TRACE_MODULE_VK_MEM

#define	ALLOW_MEMORY_ALLOC_FAIL
#ifdef ALLOW_MEMORY_ALLOC_FAIL
#define VK_Panic()
#else
#define VK_Panic    ((void (*)(void))0x0)
#endif

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____

#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____

int VK_MEM_Init(void)
{

	return VK_OK;
}

//8bit, 16bit, 32bit address = dest, 8bit data = c, 8bit = count*1, 16bit = count*2, 32bit = count*4
void *VK_MEM_Memset(void *dest, unsigned char c, unsigned long count )
{
	return BSU_memset(dest, c, count);
}

//32bit address =- dest, 32bit valut = c, count
void *memset32by32(void *dest, unsigned long c, unsigned long count )
{
	HAPPY(dest);
	HAPPY(c);
	HAPPY(count);

	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return NULL;

}

//8bit, 16bit, 32bit address = dest, count : 8bit = count*1, 16bit = count*2, 32bit = count*4
void *VK_MEM_Memcpy(void *dst0, void *src0, unsigned long len0)
{
	return BSU_memcpy(dst0, src0, len0);
}

void *VK_MEM_Alloc(unsigned long size)
{
	return BSU_malloc(size);
}

int VK_MEM_Free(void *ap)
{
	if( NULL != ap )
		BSU_free(ap);
		
	return VK_OK;
}

void *VK_MEM_Calloc(unsigned long size)
{
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	HAPPY(size);
	return NULL;
}

void *VK_MEM_Realloc(void *p, unsigned long size)
{
	HAPPY(p);
	HAPPY(size);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return NULL;
}

int VK_MEM_GetInfo(VK_MEM_INFO_t *pstMemInfo)
{
	HAPPY(pstMemInfo);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return NULL;

}


