/**
 * vk_dbgmem.c
*/

/**
 * @defgroup		VK_DBGMEM
 * @author		Tae-Hyuk Kang
 * @note			Virsual Debug Memory APIs
 * @file 			vk_dbgmem.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

/********************************************************************
Header Files
********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "vkernel.h"
#include "taskdef.h"

#include "htype.h"

#if defined(MEMORY_DEBUG)

#define DEBUG_MODULE			TRACE_MODULE_VK_MEM

// definition
#define SHORTFILENAME_LEN			20

// debug alloc header & tail magic code
#define DBG_ALLOC_HEAD_CODE		0xAEAEAEAE
#define DBG_ALLOC_TAIL_CODE		0xEDEDEDED

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define MSIZE(x)		(*((unsigned long*)x - 1))
//#define VK_Panic		((void (*)(void))0x0)
#define MAX_DBGMEM_GROUP	MAX_MEMDBG_GROUP

#ifdef MEMORY_DEBUG
#define VK_DBGMEM_PRINT	VK_DBG_Print
#define VK_DBGMEM_ERROR	VK_DBG_Print
#else
#define VK_DBGMEM_PRINT
#define VK_DBGMEM_ERROR	VK_DBG_Print
#endif

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */
char* groupStr[] =
{
	"AP",
	"API",
	"MD",
	"MW",
	"PL",
	"SI",
	"CA",
	"EX",
	"DB",
	"DD",
	"FS",
	"ZZ",
	"DSMCC",
	"EPG",
	"NONE",
	"NONE",
	"NONE",
	"NONE"
};

#define HEAD_RESERVED_SIZE		4
#define TAIL_RESERVED_SIZE		12

// debug memory header
typedef struct tagDBG_ALLOC_HEADER
{
	unsigned long 	group;						// memory data group
	unsigned char 	reserved[HEAD_RESERVED_SIZE];
	unsigned long	size;						// memory data size
	unsigned long 	code;						// head magic code. must be DBG_ALLOC_HEAD_CODE(0xAE)
} DBGALLOCHEADER;

// debug memory tail
typedef struct tagDBG_ALLOC_TAILER
{
	unsigned long code;		// tail magic code. must be. must be DBG_ALLOC_HEAD_CODE(0xAE)
	unsigned char reserved[TAIL_RESERVED_SIZE];
} DBGALLOCTAILER;

// memory list
typedef struct ALLOC_MEMLISTENT
{
	unsigned char 			*addr;
	int	 					size;
	char 					file[SHORTFILENAME_LEN];		// file name
	int	 					line;
	unsigned long			tick;

	struct ALLOC_MEMLISTENT	*next;
	struct ALLOC_MEMLISTENT	*prev;
}MEMLIST;

typedef struct DBGMEMGroup_t
{
	unsigned char traceFlag;
	unsigned long usedSize;
	unsigned long allocCnt;
}DBGMEMGROUP;

static MEMLIST *s_base[MAX_DBGMEM_GROUP] = {0,};
static MEMLIST *s_end[MAX_DBGMEM_GROUP] ={0,};
static unsigned long s_dbgMemSemId;
static DBGMEMGROUP s_dbgMemGroupInfo[MAX_DBGMEM_GROUP];


/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */


/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */
// memlist functions
static void INT_AddToMemList( void *addr, int lSize, char *file, int line, unsigned short group );
MEMLIST* INT_FindFromMemList(void* addr, unsigned short group);
void INT_RemoveElementFromMemList(MEMLIST* elem, unsigned short group, unsigned long *size);
static void INT_DelFromMemList( void *addr, char* file, int line, unsigned short group, unsigned long *size );
static void INT_PrintMemList(unsigned short group);

// group size funtions
static void INT_AddUsedSize(unsigned short group, int size);
static void INT_DeleteUsedSize(unsigned short group, int size);

static int INT_CheckTrace(unsigned short group);
static void INT_RemovePathName(char *pszDest, char *pszSrc);

// debug alloc
static int INT_GetDbgAllocSize(int size);
static void* INT_GetRealPointer(void* p);
void* INT_GetDataPointer(void* pReal);
static void INT_MakeDbgInfo(void* pReal, unsigned long size, unsigned short group, char* file, int line);
static void INT_GetDbgInfo(void* pData, unsigned long* pnDataSize, unsigned short* pnGroup);
static int INT_CheckDbgHeaderInfo(void* pData);
static int INT_CheckDbgTailInfo(void* pData, unsigned long size);

/******************************************************************************
* function : VK_DBGMEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_Init(void)
{

	return VK_OK;
}


/******************************************************************************
* function : VK_DBGMEM_Alloc
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void	*VK_DBGMEM_Alloc(unsigned long size, char* file, int line, unsigned short group)
{

	HAPPY(size);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);

}

/******************************************************************************
* function : VK_DBGMEM_Calloc
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void	*VK_DBGMEM_Calloc(unsigned long size, char* file, int line, unsigned short group)
{
	HAPPY(size);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);
}

/******************************************************************************
* function : VK_DBGMEM_Realloc
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void	*VK_DBGMEM_Realloc(void *p, unsigned long size, char* file, int line, unsigned short group)
{
	HAPPY(p);
	HAPPY(size);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);
}

/******************************************************************************
* function : VK_DBGMEM_Free
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_Free(void *p, char* file, int line, unsigned short group)
{

	HAPPY(p);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);

	return VK_OK;
}

/******************************************************************************
* function : VK_DBGMEM_Free
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_Check(void *p, char* file, int line, unsigned short group)
{

	HAPPY(p);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);

	return VK_OK;
}

/******************************************************************************
* function : VK_DBGMEM_GroupInit
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_GroupInit(unsigned short group)
{

	HAPPY(group);

	return VK_OK;
}

/******************************************************************************
* function : VK_DBGMEM_EnableTrace
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_EnableTrace(unsigned short group)
{

	HAPPY(group);

	return VK_OK;

}

/******************************************************************************
* function : VK_DBGMEM_DisableTrace
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_DisableTrace(unsigned short group)
{

	HAPPY(group);

	return VK_OK;

}

/******************************************************************************
* function : VK_DBGMEM_Group_PrintList
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_Group_PrintList(unsigned short group)
{

	HAPPY(group);

	return VK_OK;

}

/******************************************************************************
* function : VK_DBGMEM_Group_DisplayUsage
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int VK_DBGMEM_Group_DisplayUsage(unsigned short group)
{

	HAPPY(group);

	return VK_OK;

}

int VK_DBGMEM_DumpMemory(unsigned char* pBase, unsigned long ulSize, int nBytes)
{

	HAPPY(pBase);
	HAPPY(ulSize);
	HAPPY(nBytes);

	return VK_OK;
}

//
// static functions
//
void INT_AddToMemList( void *addr, int lSize, char *file, int line, unsigned short group )
{

	HAPPY(addr);
	HAPPY(lSize);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);

}

MEMLIST* INT_FindFromMemList(void* addr, unsigned short group)
{
	HAPPY(addr);
	HAPPY(group);

	return NULL;
}

void INT_RemoveElementFromMemList(MEMLIST* elem, unsigned short group, unsigned long *size)
{

	HAPPY(elem);
	HAPPY(group);
	HAPPY(size);

}

void INT_DelFromMemList( void *addr, char* file, int line, unsigned short group, unsigned long *size)
{
	HAPPY(addr);
	HAPPY(size);
	HAPPY(file);
	HAPPY(line);
	HAPPY(group);

}

void INT_AddUsedSize(unsigned short group, int size)
{
	HAPPY(size);
	HAPPY(group);
}

void INT_DeleteUsedSize(unsigned short group, int size)
{
	HAPPY(size);
	HAPPY(group);
}

int INT_CheckTrace(unsigned short group)
{
	HAPPY(group);

	return VK_OK;
}

static void INT_RemovePathName(char *pszDest, char *pszSrc)
{
	HAPPY(pszDest);
	HAPPY(pszSrc);

}

void INT_PrintMemList(unsigned short group)
{
	HAPPY(group);
}

int INT_GetDbgAllocSize(int size)
{

	HAPPY(size);

	return VK_OK;
}

void* INT_GetRealPointer(void* pData)
{
	HAPPY(pData);

	return VK_OK;
}

void* INT_GetDataPointer(void* pReal)
{
	HAPPY(pReal);

	return VK_OK;
}

void INT_MakeDbgInfo(void* pReal, unsigned long size, unsigned short group, char* file, int line)
{
	HAPPY(pReal);
	HAPPY(size);
	HAPPY(group);
	HAPPY(file);
	HAPPY(line);

}

void INT_GetDbgInfo(void* pData, unsigned long* pnDataSize, unsigned short* pnGroup)
{
	HAPPY(pData);
	HAPPY(pnDataSize);
	HAPPY(pnGroup);
}

int INT_CheckDbgHeaderInfo(void* pData)
{
	HAPPY(pData);

	return VK_OK;
}

unsigned long INT_PrintDbgInfo(int n, void* pData)
{
	HAPPY(n);
	HAPPY(pData);

	return VK_OK;

}

int INT_CheckDbgTailInfo(void* pData, unsigned long size)
{
	HAPPY(pData);
	HAPPY(size);

	return VK_OK;
}

int VK_DBGMEM_CheckAll(void)
{

	return VK_OK;
}

int VK_DBGMEM_Group_DisplayUsageVK(void)
{

	return VK_OK;
}

static void P_DBGMEM_SelfMemCheckTask(void *param)
{
	HAPPY(param);

}

void VK_DBGMEM_InitCheckTask(void)
{

}

void TEST_VK_MEM(void)
{

}
#endif

