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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	int i;
	int	vkErr;

	vkErr = VK_SEM_Create(&s_dbgMemSemId,"dbgmem", DEFAULT_SUSPEND_TYPE);
	if ( vkErr != VK_OK )
	{
		return VK_CREATE_ERROR;
	}
	memset(s_dbgMemGroupInfo, 0, sizeof(s_dbgMemGroupInfo));

	for ( i=0; i<MAX_DBGMEM_GROUP;i++)
		VK_DBGMEM_GroupInit( i );

	for (i=0; i<MAX_MEMDBG_GROUP; i++)
		VK_DBGMEM_EnableTrace(i);

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
	void* p;
	int vkErr;
	unsigned long realsize;

	if(size == 0)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Alloc[%s(%d)] - alloc size zero\n", file, line);
	}

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if ( vkErr != VK_OK )
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Alloc[%s(%d)] - semaphore error\n", file, line);
		return NULL;
	}
	realsize = INT_GetDbgAllocSize(size);
	p = VK_MEM_Alloc(realsize);
	if(p != NULL)
	{
		// add debug info and make data pointer
		INT_MakeDbgInfo(p, size, group, file, line);
		p = INT_GetDataPointer(p);

		INT_AddUsedSize(group, size);
		if(INT_CheckTrace(group))
		{
			INT_AddToMemList(p, size, file, line, group);
		}
	}
	else
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Alloc[%s(%d)] size=%ld\n", file, line, size);
	}

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if ( vkErr != VK_OK )
	{
		return NULL;
	}
	return p;

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
	void* p;
	int	vkErr;
	unsigned long realsize;

	if(size == 0)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Calloc[%s(%d)] - alloc size zero\n", file, line);
	}

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if ( vkErr != VK_OK )
	{
		return NULL;
	}

	realsize = INT_GetDbgAllocSize(size);
	p = VK_MEM_Calloc(realsize);
	if(p != NULL)
	{
		// add debug info and make data pointer
		INT_MakeDbgInfo(p, size, group, file, line);
		p = INT_GetDataPointer(p);

		INT_AddUsedSize(group,size);
		if(INT_CheckTrace(group))
		{
			INT_AddToMemList(p, size, file, line, group);
		}
	}
	else
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Calloc[%s(%d)] size=%d\n", file, line, size);

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if ( vkErr != VK_OK )
	{
		return NULL;
	}

	return p;
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
	void* pNew;

	if(size == 0)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Realloc[%s(%d)] - alloc size zero\n", file, line);
	}

	pNew = VK_DBGMEM_Alloc(size, file, line, group);
	if (!pNew)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Realloc[%s(%d)] error VK_DBGMEM_Alloc\n", file, line);
		if(p != NULL )
			VK_DBGMEM_Free(p, file, line, group);
		return NULL;
	}

	if( p != NULL )
	{
		unsigned long 		freeSize;
		unsigned short		freeGroup;
		INT_GetDbgInfo(p, &freeSize, &freeGroup);
		VK_memcpy32(pNew, p, freeSize);
		VK_DBGMEM_Free(p, file, line, group);
	}

	return pNew;
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
	unsigned long freeSize = 0;
	int	vkErr;
	unsigned short freeGroup;

	if(p == NULL)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Free[%s(%d)]ptr == NULL\n", file, line);
	}

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if ( vkErr != VK_OK )
	{
		return VK_LOCK_ERROR;
	}

	// check data pointer
	if (INT_CheckDbgHeaderInfo(p) != VK_OK)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Free[%s(%d)] invalid header\n", file, line);
	}

	// get size & group
	INT_GetDbgInfo(p, &freeSize, &freeGroup);
	// check tail pointer
	if (INT_CheckDbgTailInfo(p, freeSize) != VK_OK)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Free[%s(%d)] invalid tail\n", file, line);
	}
	if (freeGroup != group)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Free[%s(%d)] invalid group(%d), recommanded group(%d) \n", file, line, group, freeGroup);
		INT_PrintMemList(group);
		INT_PrintMemList(freeGroup);
	}

	INT_DeleteUsedSize(group, freeSize);
	if (INT_CheckTrace(group))
	{
		INT_DelFromMemList(p, file, line, group, &freeSize);
	}

	VK_MEM_Free(INT_GetRealPointer(p));

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if ( vkErr != VK_OK )
	{
		return VK_UNLOCK_ERROR;
	}

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
	unsigned long freeSize = 0;
	unsigned short freeGroup;

	if(p == NULL)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Check[%s(%d)]ptr == NULL\n", file, line);
	}

	// check data pointer
	if (INT_CheckDbgHeaderInfo(p) != VK_OK)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Check[%s(%d)] invalid header\n", file, line);
	}

	// get size & group
	INT_GetDbgInfo(p, &freeSize, &freeGroup);
	// check tail pointer
	if (INT_CheckDbgTailInfo(p, freeSize) != VK_OK)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Check[%s(%d)] invalid tail\n", file, line);
	}
	if (freeGroup != group)
	{
		VK_DBGMEM_ERROR("Error:: VK_DBGMEM_Check[%s(%d)] invalid group(%d), recommanded group(%d)\n", file, line, group, freeGroup);
	}

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
	int	vkErr;

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_LOCK_ERROR;
	}

	s_dbgMemGroupInfo[group].traceFlag = 0;
	s_dbgMemGroupInfo[group].usedSize = 0;
	s_dbgMemGroupInfo[group].allocCnt = 0;

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_UNLOCK_ERROR;
	}

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
	int	vkErr;

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_LOCK_ERROR;
	}

	s_dbgMemGroupInfo[group].traceFlag = 1;

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_UNLOCK_ERROR;
	}

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
	int	vkErr;

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_LOCK_ERROR;
	}

	s_dbgMemGroupInfo[group].traceFlag = 0;

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_UNLOCK_ERROR;
	}

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
	int	vkErr;

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_LOCK_ERROR;
	}
	INT_PrintMemList(group);

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_UNLOCK_ERROR;
	}

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
	int	vkErr;

	vkErr = VK_SEM_Get(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_LOCK_ERROR;
	}

	VK_DBGMEM_PRINT("[%s MEM] ", groupStr[group]) ;
	VK_DBGMEM_PRINT("\t0x%08X (%12u Byte, %10u KByte). AllocCnt: %d\n\r",
		s_dbgMemGroupInfo[group].usedSize,
		s_dbgMemGroupInfo[group].usedSize,
		s_dbgMemGroupInfo[group].usedSize/1024, s_dbgMemGroupInfo[group].allocCnt) ;

	vkErr = VK_SEM_Release(s_dbgMemSemId);
	if (vkErr != VK_OK )
	{
		return VK_UNLOCK_ERROR;
	}

	return VK_OK;
}

int VK_DBGMEM_DumpMemory(unsigned char* pBase, unsigned long ulSize, int nBytes)
{
	int i, j;
	unsigned long nAddr = (unsigned long)pBase / 16 * 16;
	unsigned long nEndAddr = ((unsigned long)pBase + ulSize + 15) / 16 * 16;

	VK_DBGMEM_PRINT("---------------<<DumpMemory>>---------------\n");
	VK_DBGMEM_PRINT("base: 0x%08x, size: 0x%x\n", pBase, ulSize);

	for (i = 0; nAddr < nEndAddr; i++)
	{
		if (i % (16 / nBytes) == 0)
		{
			VK_DBGMEM_PRINT("%08x: ", nAddr);
		}

		if ((unsigned long)pBase <= nAddr && nAddr < ((unsigned long)pBase + ulSize))
		{
			switch (nBytes)
			{
			case 1:
				VK_DBGMEM_PRINT("%02x ", *((unsigned char*)nAddr));
				break;
			case 2:
				VK_DBGMEM_PRINT("%04x ", *((unsigned short*)nAddr));
				break;
			case 4:
				VK_DBGMEM_PRINT("%08x ", *((unsigned long*)nAddr));
				break;
			}
		}
		else
		{
			for (j = 0; j < nBytes; j++)
			{
				VK_DBGMEM_PRINT("  ");
			}
			VK_DBGMEM_PRINT(" ");
		}

		if (i % (16 / nBytes) == 16 / nBytes - 1)
		{
			VK_DBGMEM_PRINT("\n");
		}
		nAddr += nBytes;
	}
	VK_DBGMEM_PRINT("-------------<<End DumpMemory>>-------------\n\n");
	return VK_OK;
}

//
// static functions
//
void INT_AddToMemList( void *addr, int lSize, char *file, int line, unsigned short group )
{
	MEMLIST *elem;
	char szFileName[SHORTFILENAME_LEN];

	elem = (MEMLIST *)VK_MEM_Alloc( sizeof(MEMLIST) );
	elem->addr = (unsigned char*)addr;
	elem->size = lSize;
	elem->tick = VK_TIMER_GetSystemTick();

	INT_RemovePathName(szFileName, file);
	strncpy(elem->file, szFileName, SHORTFILENAME_LEN);
	elem->file[SHORTFILENAME_LEN - 1] = '\0';
	elem->line = line;

	elem->prev = s_end[group];
	elem->next = NULL;
	if( s_end[group] != NULL )
		s_end[group]->next = elem;
	if( s_base[group] == NULL )
		s_base[group] = elem;
	s_end[group] = elem;
}

MEMLIST* INT_FindFromMemList(void* addr, unsigned short group)
{
	MEMLIST *elem;
	elem = s_base[group];

	while(elem != NULL)
	{
		if( elem->addr == addr )
		{
			return elem;
		}
		elem = elem->next;
	}

	return NULL;
}

void INT_RemoveElementFromMemList(MEMLIST* elem, unsigned short group, unsigned long *size)
{
	if( elem->prev != NULL )
		elem->prev->next = elem->next;
	if( elem->next != NULL )
		elem->next->prev = elem->prev;
	if( elem == s_base[group] )
		s_base[group] = elem->next;
	if( elem == s_end[group] )
		s_end[group] = elem->prev;

	*size = elem->size;
}

void INT_DelFromMemList( void *addr, char* file, int line, unsigned short group, unsigned long *size)
{
	MEMLIST *elem;
	elem = s_base[group];

	while(elem != NULL)
	{
		if( elem->addr == addr )
		{
			if( elem->prev != NULL )
				elem->prev->next = elem->next;
			if( elem->next != NULL )
				elem->next->prev = elem->prev;
			if( elem == s_base[group] )
				s_base[group] = elem->next;
			if( elem == s_end[group] )
				s_end[group] = elem->prev;

			*size = elem->size;

			VK_MEM_Free( elem );

			return;
		}
		elem = elem->next;
	}

	VK_DBGMEM_ERROR("Error:%s(%d), invalid address=0x%x\n\r", file, line, addr);
	INT_PrintMemList(group);
	VK_Panic();
}

void INT_AddUsedSize(unsigned short group, int size)
{
	s_dbgMemGroupInfo[group].usedSize += size;
	s_dbgMemGroupInfo[group].allocCnt++;
	return;
}

void INT_DeleteUsedSize(unsigned short group, int size)
{
	s_dbgMemGroupInfo[group].usedSize -= size;
	s_dbgMemGroupInfo[group].allocCnt--;
	return;
}

int INT_CheckTrace(unsigned short group)
{
	if(s_dbgMemGroupInfo[group].traceFlag == 1)
		return 1;
	else
		return 0;
}

static void INT_RemovePathName(char *pszDest, char *pszSrc)
{
	int i;
	char	*pLastPosition = NULL;
	int nStrLen;

	nStrLen = strlen(pszSrc);
	for (i=0; i<nStrLen; i++)
	{
		if (pszSrc[i] == '\\' || pszSrc[i] == '/')
		{
			pLastPosition = &pszSrc[i];
		}
	}
	if (pLastPosition)
		strncpy(pszDest, pLastPosition+1, SHORTFILENAME_LEN);
	else
		strncpy(pszDest, pszSrc, SHORTFILENAME_LEN);
}

void INT_PrintMemList(unsigned short group)
{
	int		num=0;
	int		total=0;
	HUINT8	*pucPointer;
	MEMLIST *elem = s_base[group];

	VK_DBGMEM_PRINT("[DIAGNOSTIC] %s MEMORY USAGE\n\r", groupStr[group]);

	while(elem != NULL)
	{
		pucPointer = elem->addr;
		if (pucPointer)
		{
			VK_DBGMEM_PRINT("%03d) 0x%08X[%02X %02X %02X %02X %02X %02X %02X %02X](%d)\t%s(%d)\t[T:%ld]\n", num+1, pucPointer, pucPointer[0], pucPointer[1], pucPointer[2], pucPointer[3], pucPointer[4], pucPointer[5], pucPointer[6], pucPointer[7], elem->size, elem->file, elem->line, elem->tick);
		}
		total += elem->size;
		num++;

		elem = elem->next;
		VK_TASK_Sleep(1);
	}
	VK_DBGMEM_PRINT("\t[%s]%d bytes allocated. (%dKB)\n\r", groupStr[group], total, total/1024);
	return;
}

int INT_GetDbgAllocSize(int size)
{
	return size + sizeof(DBGALLOCHEADER) + sizeof(DBGALLOCTAILER);
}

void* INT_GetRealPointer(void* pData)
{
	return (void*)((char*)pData - sizeof(DBGALLOCHEADER));
}

void* INT_GetDataPointer(void* pReal)
{
	return (void*)((char*)pReal + sizeof(DBGALLOCHEADER));
}

void INT_MakeDbgInfo(void* pReal, unsigned long size, unsigned short group, char* file, int line)
{
	DBGALLOCHEADER header;
	DBGALLOCTAILER tailer;
	unsigned long dbgAllocSize;
	unsigned char* pAlloc = (unsigned char*)pReal;
	HAPPY(file);
	HAPPY(line);

	dbgAllocSize = size + sizeof(DBGALLOCHEADER) + sizeof(DBGALLOCTAILER);

	// make header
	header.code = DBG_ALLOC_HEAD_CODE;
	header.group = group;
	header.size = size;

	// make tail
	tailer.code = DBG_ALLOC_TAIL_CODE;

	memcpy(pAlloc, &header, sizeof(DBGALLOCHEADER));
	memcpy(&pAlloc[dbgAllocSize - sizeof(DBGALLOCTAILER)], &tailer, sizeof(DBGALLOCTAILER));
}

void INT_GetDbgInfo(void* pData, unsigned long* pnDataSize, unsigned short* pnGroup)
{
	DBGALLOCHEADER* pHeader = (DBGALLOCHEADER*)INT_GetRealPointer(pData);

	*pnDataSize = pHeader->size;
	*pnGroup = (unsigned short)pHeader->group;
}

int INT_CheckDbgHeaderInfo(void* pData)
{
	DBGALLOCHEADER* pHeader = (DBGALLOCHEADER*)INT_GetRealPointer(pData);

	if (pHeader->code != DBG_ALLOC_HEAD_CODE)
		return VK_ERROR;
	return VK_OK;
}

unsigned long INT_PrintDbgInfo(int n, void* pData)
{
	DBGALLOCHEADER* pHeader = (DBGALLOCHEADER*)INT_GetRealPointer(pData);
	VK_DBGMEM_PRINT("\t[%d] s:%ld\n", n, pHeader->size);
	return pHeader->size;
}

int INT_CheckDbgTailInfo(void* pData, unsigned long size)
{
	DBGALLOCTAILER* pTailer = (DBGALLOCTAILER*)((char*)pData + size);

	if (pTailer->code != DBG_ALLOC_TAIL_CODE)
		return VK_ERROR;
	return VK_OK;
}

int VK_DBGMEM_CheckAll(void)
{
	HUINT32	i;
	MEMLIST *memlist, *curList;

	VK_SEM_Get(s_dbgMemSemId);
	for (i=0; i<MAX_DBGMEM_GROUP; i++)
	{
		memlist = s_base[i];
		while (memlist)
		{
			curList = memlist;
			memlist = memlist->next;
			if (curList->addr != NULL)
			{
				VK_DBGMEM_Check(curList->addr, curList->file, curList->line, i);
			}
		}
	}
	VK_SEM_Release(s_dbgMemSemId);
	return VK_OK;
}

int VK_DBGMEM_Group_DisplayUsageVK(void)
{
	int				group;
	unsigned long 	ulVkMemAllocSize;
	unsigned long 	ulVkMemAllocCount;

	ulVkMemAllocSize = 0;
	ulVkMemAllocCount = 0;
	VK_SEM_Get(s_dbgMemSemId);
	for(group=0; group<MAX_DBGMEM_GROUP; group++)
	{
		ulVkMemAllocSize += s_dbgMemGroupInfo[group].usedSize;
		ulVkMemAllocCount += s_dbgMemGroupInfo[group].allocCnt;
	}
	VK_SEM_Release(s_dbgMemSemId);

	VK_DBGMEM_PRINT("[*TOTAL] ");
	VK_DBGMEM_PRINT("\t0x%08X (%12u Byte, %10u KByte). AllocCnt: %d\n\r",
		ulVkMemAllocSize,
		ulVkMemAllocSize,
		ulVkMemAllocSize/1024, ulVkMemAllocCount);

	return VK_OK;
}

static void P_DBGMEM_SelfMemCheckTask(void *param)
{
	HAPPY(param);
	
	while (1)
	{
		VK_TASK_Sleep(1000);
		VK_DBGMEM_CheckAll();
	}
}

void VK_DBGMEM_InitCheckTask(void)
{
	int				vkResult;
	unsigned long	s_taskSelfMemCheckTaskId;
	
	vkResult = VK_TASK_Create(P_DBGMEM_SelfMemCheckTask,
						68,
						1024*4,
						"vk_self_memcheck_t",
						NULL,
						&s_taskSelfMemCheckTaskId, FALSE);
	if( vkResult != VK_OK )
	{
		return;
	}

	vkResult = VK_TASK_Start(s_taskSelfMemCheckTaskId);
	if( vkResult != VK_OK )
	{
		s_taskSelfMemCheckTaskId = 0;
		return;
	}
}

void TEST_VK_MEM(void)
{
	unsigned char *p1, *p2, *p3, *p4, *p5;

	p1 = (unsigned char *)DD_MEM_Alloc(100);
	if(p1 == NULL)
	{
		VK_DBGMEM_PRINT("DD_MEM_Alloc Error!!\n");
	}
	p1[100] = 0;	// violation : exceed ponter accessed
	
	p2 = (unsigned char *)DD_MEM_Alloc(10);
	if(p2 == NULL)
	{
		VK_DBGMEM_PRINT("DD_MEM_Alloc Error!!\n");
	}
	p3 = (unsigned char *)DD_MEM_Alloc(300);
	if(p3 == NULL)
	{
		VK_DBGMEM_PRINT("DD_MEM_Alloc Error!!\n");
	}
	p4 = (unsigned char *)DD_MEM_Alloc(500);
	if(p4 == NULL)
	{
		VK_DBGMEM_PRINT("DD_MEM_Alloc Error!!\n");
	}
	p5 = (unsigned char *)DD_MEM_Alloc(900);
	if(p5 == NULL)
	{
		VK_DBGMEM_PRINT("DD_MEM_Alloc Error!!\n");
	}

	DD_MEM_PrintList();
	DD_MEM_DisplayUsage();
}
#endif

