#include <execinfo.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>

#include <htype.h>
#include <vkernel.h>


#include "_memory_debug_int.h"

#define	_DEF_MEMDBG_snprintf		snprintf	/* int snprintf ( char * s, size_t n, const char * format, ... ); */

#define	MEMDBG_MAX_GROUP	1
#define	MEMDBG_ALLOC	malloc
#define	MEMDBG_FREE		free
#define	MEMDBG_SLICE_BLOCK	100
#define	MEMDBG_ASSERT	assert

#define	MEMDBG_FINGERPRINT_ON_FREE

#define	_____memdbg_type______________________________________________________________

typedef struct memmeta
{
	HINT32			group;		///< group index
	HINT32			slot;		///< slot index

	const HCHAR		*file;		///< call file
	HINT32			line;		///< call line
	HUINT32			size;		///< memory size (NOT real size)
	HUINT32			time;		///< alloc tick (sec)

#if defined(MEMDBG_BACKTRACE)
	int				numFrame;
	void			*framePtrs[MEMDBG_BACKTRACE_FRAME_NUM];
#endif
	unsigned long	vkTaskId;
	HUINT32			osThreadId;

	HUINT32			magicno;	///< magic number
	HCHAR			memory[4];	///< allocated memory (last 4 byte == magic number)
} MemMeta_t;

#define	MEMDBG_SIZE(s)	((s) + sizeof(MemMeta_t))
#define	MEMDBG_MAGICNO	((HUINT32)0x4A41434B)

typedef struct memgroup
{
	HINT32			group;			///< group index
	HUINT32			usedsize;		///< total used size

	HUINT32			allocated;		///< allocated count
	HUINT32			used;			///< used count

	HINT32			nextslot;		///< next slot
	MemMeta_t		**allocslot;	///< memory allocslot address array

	HUINT32			freecount;		///< free slot count
	HINT32			*freeslot;		///< free slot array
} MemGroup_t;

typedef struct memdebugger
{
	HBOOL		initialized;
	HUINT32		semId;		///< semaphore
	HUINT32		addrBegin;
	HUINT32		addrEnd;
	MemGroup_t	group[MEMDBG_MAX_GROUP];
} MemDebug_t;

typedef struct memextiface
{
	HINT32		(* Log) (const HCHAR *fmt, ...);
	HINT32		(* Error) (const HCHAR *fmt, ...);
	HUINT32		(* SemCreate) (const HCHAR *);
	void		(* SemDestroy) (HUINT32);
	HINT32		(* SemLock) (HUINT32);
	HINT32		(* SemUnlock) (HUINT32);
} MemIFace_t;

typedef struct
{
	unsigned long		vkTaskId;
	HUINT32				osThreadId;
	char				taskName[32];
	unsigned long		memorySize;
	unsigned long		memoryCount;
} TaskMemInfo_t;

typedef struct
{
	HUINT32				 ulNumTask;
	HUINT32				 ulArrayLen;

	TaskMemInfo_t		*pstTaskArray;
} TaskMemArray_t;


#define	_____memdbg_private______________________________________________________________
#include <vkernel.h>
static HINT32 _memdbg_Log (const HCHAR * fmt,...)
{
	va_list	ap;
	HINT32	ret;

	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);

	abort();

	return ret;
}

static HUINT32	_memdbg_SemCreate (const HCHAR *name)
{
	HUINT32	id;

	if (VK_SEM_Create((unsigned long *)&id, name, VK_SUSPENDTYPE_FIFO) != VK_OK)
		return 0;
	return id;
//	return (HUINT32)name;
}

static void		_memdbg_SemDestroy (HUINT32 id)
{
	(void)id;
	VK_SEM_Destroy((unsigned long)id);
}

static HINT32	_memdbg_SemLock (HUINT32 id)
{
	return VK_SEM_Get(id);
}
static HINT32	_memdbg_SemUnlock (HUINT32 id)
{
	return VK_SEM_Release(id);
}


const HCHAR *	_memdbg_AscTime (HUINT32 unixtime, HCHAR *timebuf, HUINT32 n)
{
	static HCHAR	buf[26];

	if (timebuf == NULL || n == 0)
	{
		timebuf = buf;
		n = 26;
	}

	_DEF_MEMDBG_snprintf(timebuf, n, "%03d:%02d:%02d", unixtime / 3600, (unixtime % 3600) / 60, (unixtime % 60));

	return timebuf;
}


const HCHAR *	_memdbg_RmPath(const HCHAR *file)
{
	HCHAR *ch;

	if (file == NULL)
		return file;

	ch = strrchr(file, (HINT32)'/');
	if (ch == NULL)
	{
		return file;
	}
	return ++ch;
}
#define	_RMDIR(f)	_memdbg_RmPath(f)

static MemIFace_t *	_memdbg_IFace (void)
{
	static MemIFace_t	s_iface;

	if (s_iface.Log == NULL)
	{
		s_iface.Log        = _memdbg_Log;
		s_iface.Error      = _memdbg_Log;
		s_iface.SemCreate  = _memdbg_SemCreate;
		s_iface.SemDestroy = _memdbg_SemDestroy;
		s_iface.SemLock    = _memdbg_SemLock;
		s_iface.SemUnlock  = _memdbg_SemUnlock;
	}
	return &s_iface;
}

static MemDebug_t *	_memdbg_Get(void)
{
	static MemDebug_t	s_dbgr;

	if (!s_dbgr.initialized)
	{
		HINT32	i;
		MemGroup_t	*group;

		for (i = 0 ; i < MEMDBG_MAX_GROUP ; i++)
		{
			group = s_dbgr.group + i;

			group->group = i;
			group->allocated = MEMDBG_SLICE_BLOCK;
			group->allocslot = (MemMeta_t **)MEMDBG_ALLOC(sizeof(void *) * group->allocated);
			group->freeslot  = (HINT32     *)MEMDBG_ALLOC(sizeof(HINT32) * group->allocated);

			if (group->allocslot == NULL || group->freeslot == NULL)
			{
				if (group->allocslot) MEMDBG_FREE(group->allocslot);
				if (group->freeslot ) MEMDBG_FREE(group->freeslot);
				return NULL;
			}
			memset(group->allocslot, 0x00, sizeof(void *) * group->allocated);
			memset(group->freeslot , 0xFF, sizeof(HINT32) * group->allocated);
		}
		s_dbgr.semId = _memdbg_IFace()->SemCreate("MEMDBG");
		s_dbgr.addrBegin = 0xFFFFFFFF;
		s_dbgr.addrEnd   = 0;
		s_dbgr.initialized = TRUE;
	}
	_memdbg_IFace()->SemLock(s_dbgr.semId);

	return &s_dbgr;
}

static void		_memdbg_Release (MemDebug_t *dbg)
{
	if(dbg == NULL)
	{
		_memdbg_IFace()->Error("[%4d] dbg is NULL!\n", __LINE__);
		return;
	}

	_memdbg_IFace()->SemUnlock(dbg->semId);
}


static MemGroup_t *	_memdbg_Grow (MemGroup_t *group)
{
	HUINT32	oldsize;
	HUINT32	newsize;
	void *	grown[2];

	oldsize  = group->allocated * sizeof(void *);
	newsize  = oldsize * 2;

	grown[0] = MEMDBG_ALLOC(newsize);
	if (grown[0] == NULL)
		return NULL;

	grown[1] = MEMDBG_ALLOC(newsize);
	if (grown[1] == NULL)
	{
		MEMDBG_FREE(grown[0]);
		return NULL;
	}

	memset((HCHAR *)grown[0] + oldsize, 0x00, newsize - oldsize);
	memset((HCHAR *)grown[1] + oldsize, 0xFF, newsize - oldsize);

	memcpy(grown[0], group->allocslot, oldsize);
	memcpy(grown[1], group->freeslot,  oldsize);

	MEMDBG_FREE(group->allocslot);
	MEMDBG_FREE(group->freeslot);

	group->allocslot = (MemMeta_t **)grown[0];
	group->freeslot  = (   HINT32 * )grown[1];
	group->allocated *= 2;

	return group;
}


static HINT32	_memdbg_FindSlot (MemGroup_t *group)
{
	if (group->freecount > 0)
	{
		HINT32 slot;

		slot  = group->freeslot[--(group->freecount)];

		MEMDBG_ASSERT(slot >= 0);
		MEMDBG_ASSERT(group->allocslot[slot] == NULL);

		group->freeslot[group->freecount] = -1;
		return slot;
	}

	if ((HUINT32)group->nextslot >= group->allocated)
		return -1;

	return group->nextslot++;
}

HUINT32		_memdbg_GetSystemTick(void)
{
	int rc;
	struct timespec humax_time;
	unsigned long	msTick;

	rc = clock_gettime (CLOCK_MONOTONIC, &humax_time);
    if (rc!=0)
	{
		return 0;
    }

	msTick = (humax_time.tv_sec*1000 + humax_time.tv_nsec/1000000);

	return (unsigned long)msTick;
}

static MemMeta_t *	_memdbg_New (MemGroup_t *group, HUINT32 size, const HCHAR *file, HINT32 line)
{
	MemMeta_t	*meta;
	HINT32		slot;

	// grow slot list, if full
	if (group->allocated <= group->used)
	{
		if (_memdbg_Grow(group) == NULL)
		{
			_memdbg_IFace()->Error("[%4d] out of memory!\n", __LINE__);
			return NULL;
		}
	}

	meta = (MemMeta_t *)MEMDBG_ALLOC(MEMDBG_SIZE(size));
	if (meta == NULL)
	{
		_memdbg_IFace()->Error("[%4d] out of memory!\n", __LINE__);
		return NULL;
	}

	// find empty slot
	slot = _memdbg_FindSlot(group);
	if (slot < 0)
	{
		_memdbg_IFace()->Error("[%4d] cannot find free slot!\n", __LINE__);
		MEMDBG_FREE(meta);
		return NULL;
	}

	group->allocslot[slot] = meta;
	group->usedsize += size;
	group->used++;

	meta->group = group->group;
	meta->slot  = slot;
	meta->file  = file;
	meta->line  = line;
	meta->size  = size;
	meta->time  = _memdbg_GetSystemTick() / 1000;
#if defined(MEMDBG_BACKTRACE)
	meta->numFrame = 0;
#endif
	meta->vkTaskId = 0;
	meta->osThreadId = 0;

	(void)VK_TASK_GetCurrentId(&(meta->vkTaskId));

	if (meta->vkTaskId == 0)
	{
#if defined(OS_LINUX)
		meta->osThreadId = (HUINT32)syscall(4222);
#endif
	}

	meta->magicno = MEMDBG_MAGICNO;
	*(HUINT32 *)(meta->memory + meta->size) = MEMDBG_MAGICNO;

	return meta;
}

static void		_memdbg_Delete (MemGroup_t *group, MemMeta_t *meta, const HCHAR *file, HINT32 line)
{
	if (meta->magicno != MEMDBG_MAGICNO
		|| *(HUINT32 *)(meta->memory + meta->size) != MEMDBG_MAGICNO)
	{
		_memdbg_IFace()->Error("[%4d] <G%d> memory corruption [%s:%d]!!!\n", __LINE__, group->group, _RMDIR(file), line);
		_memdbg_IFace()->Error("            ==> meta[%s:%d]!!!\n", _RMDIR(meta->file), meta->line);
		return;
	}

	if (group->allocated <= (HUINT32)meta->slot || group->allocslot[meta->slot] != meta)
	{
		_memdbg_IFace()->Error("[%4d] <G%d> invalid address(0x%X) [%s:%d]!!!\n", __LINE__, group->group, (int)meta, _RMDIR(file), line);
		_memdbg_IFace()->Error("            ==> meta[%s:%d]!!!\n", _RMDIR(meta->file), meta->line);
		return;
	}

	group->allocslot[meta->slot] = NULL;
	group->freeslot[group->freecount++] = meta->slot;
	group->used--;
	group->usedsize -= meta->size;

#ifdef	MEMDBG_FINGERPRINT_ON_FREE
	meta->magicno = 0x58585858;//"XXXX"
	_DEF_MEMDBG_snprintf((char *)meta, MEMDBG_SIZE(meta->size), "%s:%d", _RMDIR(file), line);
#endif

	MEMDBG_FREE(meta);
}

static HBOOL	_memdbg_CheckCorruption (MemDebug_t *dbg, void *mem, HUINT32 size)
{
	HUINT8		*byte;
	MemMeta_t	*meta;
	HUINT32		temp;
	HINT32		limit;
	HUINT8		*magicno;

	if ((HUINT32)mem < dbg->addrBegin)
		return FALSE;
	if (dbg->addrEnd < (HUINT32)mem)
		return FALSE;
	// heap!

	temp  = MEMDBG_MAGICNO;
	magicno = (HUINT8 *)&temp;

	byte  = (HUINT8 *)mem;
	limit = (HINT32)size * 10;
	while (limit--)	// find magic no
	{
		byte--;
		if (*byte == magicno[3]
			&& *(byte - 1) == magicno[2]
			&& *(byte - 2) == magicno[1]
			&& *(byte - 3) == magicno[0]
		) {
			byte++;
			break;
		}
	}
	if (limit < 0)	// 이거 무서운 코드니, 일단 이정도로만 해보자
		return FALSE;

	meta = (MemMeta_t *)(byte - offsetof(MemMeta_t, memory));
	if (meta->size - ((HUINT32)mem - (HUINT32)meta->memory) < size)
		return TRUE;
	return FALSE;
}

static void	_memdbg_SummaryGroup (MemGroup_t *group, HUINT32 curr, HUINT32 fromSec, HUINT32 durSec, HBOOL allThread, unsigned long taskId, HUINT32 ulMinSize, HUINT32 ulMaxSize, const HCHAR *filter, FILE *fp)
{
//	HBOOL		bAllThread = FALSE, bNoVkThread = FALSE;
//	HUINT32		ulThreadId = 0;
	HUINT32	   i;
	HCHAR	   timebuf[26];
	MemMeta_t *meta;

	if (group->allocslot == NULL || group->used == 0)
	{
		fprintf(fp, "\tG(%2d) empty group.\n", group->group);
		return;
	}


	for (i = 0 ; i < group->allocated ; i++)
	{
		meta = group->allocslot[i];
		if (meta == NULL)
			continue;

		if (meta->time < fromSec)
			continue;
		if (durSec > 0 && curr - meta->time < durSec)
			continue;
		if (meta->size < ulMinSize)
			continue;
		if (meta->size > ulMaxSize)
			continue;
		if ((!allThread) && (meta->vkTaskId != taskId))
			continue;
		if (filter && strstr(meta->file, filter) == NULL)
			continue;

		fprintf(fp,
			"\tG(%2d)%15s %25s[%4d] size:%u ptr:%p\n"
			, group->group
			, _memdbg_AscTime(curr - meta->time, timebuf, 26)
			, _RMDIR(meta->file)
			, meta->line
			, meta->size
			, meta->memory
		);
	}
	fprintf(fp, "\tG(%2d) used(%u) count(%u)\n", group->group, group->usedsize, group->used);
}

static void _memdbg_ClearTaskArray (TaskMemArray_t *pstArray)
{
	if (NULL != pstArray)
	{
		if (NULL != pstArray->pstTaskArray)				{ free(pstArray->pstTaskArray); }
		memset(pstArray, 0, sizeof(TaskMemArray_t));
	}
}

static TaskMemInfo_t *_memdbg_AppendTaskInfoToArray (TaskMemArray_t *pstArray, unsigned long ulVkTaskId, HUINT32 ulOsThreadId)
{
	HUINT32				 ulCnt;
	TaskMemInfo_t		*pstInfo;

	if ((pstArray->ulNumTask > 0) && (pstArray->pstTaskArray != NULL))
	{
		for (ulCnt = 0; ulCnt < pstArray->ulNumTask; ulCnt++)
		{
			pstInfo = &(pstArray->pstTaskArray[ulCnt]);

			if (((0 != ulVkTaskId) && (pstInfo->vkTaskId == ulVkTaskId)) ||
				((0 == ulVkTaskId) && (0 == pstInfo->vkTaskId) && (pstInfo->osThreadId == ulOsThreadId)))
			{
				return pstInfo;
			}
		}
	}

	if (pstArray->ulNumTask >= pstArray->ulArrayLen)
	{
		TaskMemInfo_t		*pstNewArray;

		pstNewArray = (TaskMemInfo_t *)malloc(sizeof(TaskMemInfo_t) * (pstArray->ulNumTask + 32));
		if (NULL == pstNewArray)
		{
			return NULL;
		}

		memset(pstNewArray, 0, sizeof(TaskMemInfo_t) * (pstArray->ulNumTask + 32));

		if ((pstArray->ulNumTask > 0) && (NULL != pstArray->pstTaskArray))
		{
			memcpy(pstNewArray, pstArray->pstTaskArray, sizeof(TaskMemInfo_t) * pstArray->ulNumTask);
			free(pstArray->pstTaskArray);
		}

		pstArray->ulArrayLen = pstArray->ulNumTask + 32;
		pstArray->pstTaskArray = pstNewArray;
	}

	if ((pstArray->ulNumTask == 0) || (pstArray->pstTaskArray == NULL))
	{
		return NULL;
	}

	pstArray->pstTaskArray[pstArray->ulNumTask].vkTaskId = ulVkTaskId;
	if (0 != ulVkTaskId)
	{
		VK_TASK_GetTaskName(ulVkTaskId, pstArray->pstTaskArray[pstArray->ulNumTask].taskName);
	}
	else
	{
		pstArray->pstTaskArray[pstArray->ulNumTask].osThreadId = ulOsThreadId;
	}

	pstInfo = &(pstArray->pstTaskArray[pstArray->ulNumTask]);
	pstArray->ulNumTask ++;

	return pstInfo;
}

static void	_memdbg_LogTaskMemGroup (MemGroup_t *group, TaskMemArray_t *pstArray)
{
	HUINT32			 i;
	MemMeta_t		*meta;
	TaskMemInfo_t	*pstInfo;

	if (group->allocslot == NULL || group->used == 0)
	{
		return;
	}

	for (i = 0 ; i < group->allocated ; i++)
	{
		meta = group->allocslot[i];
		if (meta == NULL)
			continue;

		pstInfo = _memdbg_AppendTaskInfoToArray(pstArray, meta->vkTaskId, meta->osThreadId);
		if (NULL != pstInfo)
		{
			pstInfo->memorySize += (unsigned long)meta->size;
			pstInfo->memoryCount ++;
		}
	}
}

#if defined(MEMDBG_BACKTRACE)
static HBOOL	_memdbg_getBackTrace (MemGroup_t *group, void *allocPtr, MemPrintCb_t pfPrintCb)
{
	HUINT32		i;
#if !defined(MEMDBG_BACKTRACE_USE_HUMAX_FIX)
	HINT32		cnt;
	char		**btfuncnames = NULL;
#endif
	MemMeta_t	*meta;

	if (group->allocslot == NULL || group->used == 0)
	{
		return FALSE;
	}

	for (i = 0 ; i < group->allocated ; i++)
	{
		meta = group->allocslot[i];
		if (meta == NULL)
			continue;

		if ((void *)meta->memory != allocPtr)
			continue;



#if defined(MEMDBG_BACKTRACE_USE_HUMAX_FIX)
		if (meta->numFrame > 0)
		{
			pfPrintCb("pointer (0x%08x) found in group (%d):\n", (HUINT32)allocPtr, i);
			(void)_memdbg_BacktraceSymbols(meta->framePtrs, meta->numFrame, pfPrintCb);

			return TRUE;
		}

		pfPrintCb("pointer (0x%08x) found in group (%d):\n", (HUINT32)allocPtr, i);
		pfPrintCb("but something wrong... frameNum(%d):\n", meta->numFrame);
#if defined(CONFIG_PROD_CPU_BCM_ARM)
#if !defined (CONFIG_DEBUG_ARM_BACKTRACE)
		pfPrintCb("CONFIG_DEBUG_ARM_BACKTRACE is not set. turn on Menuconfig -> Debug -> DEBUG_ARM_BACKTRACE\n");
#endif
#endif
#else

		if (meta->numFrame > 0)
		{
			// get symbol info.
			btfuncnames = backtrace_symbols(meta->framePtrs, meta->numFrame);
			if (NULL != btfuncnames)
			{
				pfPrintCb("pointer (0x%08x) found in group (%d):\n", (HUINT32)allocPtr, i);
				for (cnt = 0; cnt < meta->numFrame; cnt++)
				{
					pfPrintCb("- Frame (%02d) : ptr(0x%08x) with [%s]\n", cnt, meta->framePtrs[cnt], btfuncnames[cnt]);
				}

				free(btfuncnames);

				return TRUE;
			}
		}

		pfPrintCb("pointer (0x%08x) found in group (%d):\n", (HUINT32)allocPtr, i);
		pfPrintCb("but something wrong... frameNum(%d), btfuncnames(%p):\n", meta->numFrame, btfuncnames);
#if defined(CONFIG_PROD_CPU_BCM_ARM)
#if !defined (CONFIG_DEBUG_ARM_BACKTRACE)
		pfPrintCb("CONFIG_DEBUG_ARM_BACKTRACE is not set. turn on Menuconfig -> Debug -> DEBUG_ARM_BACKTRACE\n");
#endif
#endif

#endif
		return TRUE;
	}

	return FALSE;
}
#endif

static void	_memdbg_CheckMemGroup(MemGroup_t *group, FILE *fp)
{
	HUINT32		i;
	MemMeta_t	*meta;

	if (group->allocslot == NULL || group->used == 0)
	{
		fprintf(fp, "\tG(%2d) empty group.\n", group->group);
		return;
	}

//	fprintf(fp, "start check [%d]\n", _memdbg_GetSystemTick());
	for (i = 0; i < group->allocated; i++)
	{
		meta = group->allocslot[i];
		if (meta == NULL)
			continue;

		if (meta->magicno != MEMDBG_MAGICNO
			|| *(HUINT32 *)(meta->memory + meta->size) != MEMDBG_MAGICNO)
		{
			fprintf(fp,
				"\tG(%2d) %25s[%4d] size:%u ptr:%p\n"
				, group->group
				, _RMDIR(meta->file)
				, meta->line
				, meta->size
				, meta->memory);
			abort();
		}
	}
//	fprintf(fp, "end check [%d]\n", _memdbg_GetSystemTick());
}

#define	_____memdbg_public______________________________________________________________


void	MEMDBG_Init (
			  	  HINT32  (* log) (const HCHAR *fmt, ...)
				, HINT32  (* errlog) (const HCHAR *fmt, ...)
				, HUINT32 (* sem_create) (const HCHAR *name)
				, void    (* sem_destroy) (HUINT32 id)
				, HINT32  (* sem_lock) (HUINT32 id)
				, HINT32  (* sem_unlock) (HUINT32 id)
			)
{
	MemIFace_t *iface = _memdbg_IFace();

	if (log)         iface->Log        = log;
	if (errlog)      iface->Error      = errlog;
	if (sem_create)  iface->SemCreate  = sem_create;
	if (sem_destroy) iface->SemDestroy = sem_destroy;
	if (sem_lock)    iface->SemLock    = sem_lock;
	if (sem_unlock)  iface->SemUnlock  = sem_unlock;

	_memdbg_Release(_memdbg_Get());

	//iface->Log("[%4d] memdbg initialized!\n", __LINE__);
}

void *	MEMDBG_Malloc (HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;
	MemMeta_t  *meta = NULL;

	dbg  = _memdbg_Get();
	if (dbg)
	{
		meta = _memdbg_New(dbg->group + group % MEMDBG_MAX_GROUP, size, file, line);
		if (meta)
		{
			if ((HUINT32)meta < dbg->addrBegin)
				dbg->addrBegin = (HUINT32)meta;
			if (dbg->addrEnd < (HUINT32)meta + MEMDBG_SIZE(size))
				dbg->addrEnd = (HUINT32)meta + MEMDBG_SIZE(size);

#if defined(MEMDBG_BACKTRACE)
			meta->numFrame = _memdbg_GetCallStack(meta->framePtrs, MEMDBG_BACKTRACE_FRAME_NUM);
#endif
		}
		_memdbg_Release(dbg);
	}

	if (meta)
	{
		return (void *)meta->memory;
	}
	return NULL;
}

void	MEMDBG_Free (void *mem, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;
	MemMeta_t  *meta;

	if (mem == NULL)
	{
		_memdbg_IFace()->Log("NULL free !![%s:%d]\n", _RMDIR(file), line);
		return;
	}

	dbg  = _memdbg_Get();
	if (dbg)
	{
#ifdef	MEMDBG_CHECK_DETAIL
		if (!_memdbg_HasMem(dbg, mem))
		{
			_memdbg_IFace()->Error("The memory(0x%X) is NOT mine~![%s:%d]\n", mem, _RMDIR(file), line);
			return;
		}
#endif

		meta = (MemMeta_t *)((HCHAR *)mem - offsetof(MemMeta_t, memory));
		_memdbg_Delete(dbg->group + meta->group % MEMDBG_MAX_GROUP, meta, file, line);
		_memdbg_Release(dbg);
	}
}

void *	MEMDBG_Calloc (HUINT32 nmemb, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	void *mem;

	mem = MEMDBG_Malloc(nmemb * size, group, file, line);
	if (mem)
	{
		memset(mem, 0, nmemb * size);
	}
	return mem;
}


void *	MEMDBG_Realloc (void *mem, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	void      *newmem;
	MemMeta_t *meta;

	if (mem == NULL)
	{
		//@see manpage, If 'mem' is NULL, then the call is equivalent to 'malloc(size)'.
		return MEMDBG_Malloc(size, group, file, line);
	}

	if (size == 0)
	{
		//@see manpage, If 'size' is equal to zero, then the call is equivalent to 'free(mem)'.
		MEMDBG_Free(mem, file, line);
		return NULL;
	}

	meta = (MemMeta_t *)((HCHAR *)mem - offsetof(MemMeta_t, memory));
	if (meta->size == size)
	{
		return mem;
	}

	newmem = MEMDBG_Malloc(size, group, file, line);
	if (newmem)
	{
		if (meta->size < size)
			size = meta->size;
		memcpy(newmem, mem, size);

		MEMDBG_Free(mem, file, line);
	}
	return newmem;
}



HCHAR *	MEMDBG_StrDup (const HCHAR *str, HINT32 group, const HCHAR *file, HINT32 line)
{
	HUINT32	len;
	HCHAR *	dup;

	if (str)
	{
		len = strlen(str);
		dup = MEMDBG_Malloc(len + 1, group, file, line);
		if (dup)
		{
			if (len)
				memcpy(dup, str, len);
			dup[len] = '\0';
		}
		return dup;
	}

	return NULL;
}

void *	MEMDBG_MemDup (const void *mem, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	void *	dup;

	if (size && mem)
	{
		dup = MEMDBG_Malloc(size, group, file, line);
		if (dup)
		{
			memcpy(dup, mem, size);
		}
		return dup;
	}

	return NULL;
}

void	MEMDBG_Summary (HINT32 group, HUINT32 fromSec, HUINT32 durSec, HCHAR *szThreadName, HUINT32 ulMinSize, HUINT32 ulMaxSize, const HCHAR *filter, FILE *fp)
{
	HINT32		i;
	HBOOL		allThread = FALSE;
	unsigned long	taskId = 0;
	HUINT32		curr;
	HUINT32	    total;
	MemDebug_t *dbg;
	MemGroup_t *grp;

	dbg   = _memdbg_Get();
	if (dbg)
	{
		curr  = _memdbg_GetSystemTick() / 1000;
		total = 0;

		if (strcmp(szThreadName, "all thread") == 0)
		{
			allThread = TRUE;
		}
		else if (strcmp(szThreadName, "no VK thread") == 0)
		{
			taskId = 0;
		}
		else
		{
			HINT32			 nTaskNum = 0;

			VK_TASK_GetCountTasks(&nTaskNum);
			if (nTaskNum > 0)
			{
				unsigned long		*pulTaskIdArray;

				pulTaskIdArray = (unsigned long *)malloc(sizeof(unsigned long) * nTaskNum);
				if (NULL != pulTaskIdArray)
				{
					char		 szTaskName[64];

					memset(pulTaskIdArray, 0, sizeof(unsigned long) * nTaskNum);
					VK_TASK_GetAllTaskIds(nTaskNum, &nTaskNum, pulTaskIdArray);

					for (i = 0; i < nTaskNum; i++)
					{
						memset(szTaskName, 0, 64);
						VK_TASK_GetTaskName(pulTaskIdArray[i], szTaskName);
						if (('\0' != szTaskName[0]) && (strcmp(szTaskName, szThreadName) == 0))
						{
							taskId = pulTaskIdArray[i];
							break;
						}
					}
					free(pulTaskIdArray);
				}

			}
		}

		fprintf(fp, "\n<MEMDBG::SUMMARY(group:%d, from:%usec duration:%usec, namefilter:%s)>\n", group, fromSec, durSec, filter);

		for (i = 0 ; i < MEMDBG_MAX_GROUP ; i++)
		{
			grp = dbg->group + i;

			if (group < 0 || group == i)
			{
				_memdbg_SummaryGroup(grp, curr, fromSec, durSec, allThread, taskId, ulMinSize, ulMaxSize, filter, fp);
			}
			total += grp->usedsize;
		}

		fprintf(fp, "</MEMDBG(total:%u)>\n\n", total);
		_memdbg_Release(dbg);
	}
}

void	MEMDBG_GetBackTrace (HINT32 group, void *allocPtr, void *pvPrintCbFunc)
{
#if defined(MEMDBG_BACKTRACE)
	HBOOL			 found = FALSE;
	HINT32			 i;
	MemDebug_t		*dbg;
	MemGroup_t		*grp;
	MemPrintCb_t	 pfPrintCb = (MemPrintCb_t)pvPrintCbFunc;

	if (NULL == pfPrintCb)			{ return; }

	dbg = _memdbg_Get();
	if (dbg)
	{
		for (i = 0 ; i < MEMDBG_MAX_GROUP ; i++)
		{
			grp = dbg->group + i;

			if (group < 0 || group == i)
			{
				found = _memdbg_getBackTrace(grp, allocPtr, pfPrintCb);
			}

			if (TRUE == found)
			{
				break;
			}
		}

		_memdbg_Release(dbg);
	}

	if (TRUE != found)
	{
		pfPrintCb(">> !! cannot found the memory (0x%08x):\n", (HUINT32)allocPtr);
	}
#else
	MemPrintCb_t	 pfPrintCb = (MemPrintCb_t)pvPrintCbFunc;

	if (NULL == pfPrintCb)			{ return; }

	pfPrintCb(">> !! Memory Backtrace not supported:\n");
#endif
}

void	MEMDBG_LogTaskMemUsage (HCHAR *szTaskName, FILE *fp)
{
	int				 i, logAllTask;
	MemDebug_t		*dbg = NULL;
	MemGroup_t		*grp;
	TaskMemArray_t	 taskArray;

	memset(&taskArray, 0, sizeof(TaskMemArray_t));

	dbg   = _memdbg_Get();
	if (dbg)
	{
		if ((NULL == szTaskName) || ('\0' == *szTaskName))
		{
			szTaskName = "all";
		}

		logAllTask = (strcmp(szTaskName, "all") == 0) ? 1 : 0;

		for (i = 0 ; i < MEMDBG_MAX_GROUP ; i++)
		{
			grp = dbg->group + i;
			_memdbg_LogTaskMemGroup(grp, &taskArray);
		}

		fprintf(fp, "\n<MEMDBG::MEMORY ALLOCATION FOR TASK>\n");
		fprintf(fp, "\n\tin VK TASK:\n");

		for (i = 0; i < taskArray.ulNumTask; i++)
		{
			TaskMemInfo_t			*pstInfo = &(taskArray.pstTaskArray[i]);

			if ((1 == logAllTask) || (strcmp(szTaskName, pstInfo->taskName) == 0))
			{
				if ((pstInfo->vkTaskId != 0) && (pstInfo->memorySize > 0))
				{
					fprintf(fp, "\t\tTask(%s), OS used(%lu), count(%lu)\n", pstInfo->taskName, pstInfo->memorySize, pstInfo->memoryCount);
				}
			}
		}

		fprintf(fp, "\n\tin NON VK TASK:\n");

		for (i = 0; i < taskArray.ulNumTask; i++)
		{
			TaskMemInfo_t			*pstInfo = &(taskArray.pstTaskArray[i]);

			if (1 == logAllTask)
			{
				if ((pstInfo->vkTaskId == 0) && (pstInfo->memorySize > 0))
				{
					fprintf(fp, "\t\tOS ThreadId(%u), OS used(%lu), count(%lu)\n", pstInfo->osThreadId, pstInfo->memorySize, pstInfo->memoryCount);
				}
			}
		}

		fprintf(fp, "</MEMDBG>\n\n");
	}

	if (NULL != dbg)
	{
		_memdbg_Release(dbg);
	}

	_memdbg_ClearTaskArray(&taskArray);
}

HUINT32	MEMDBG_GetUsedMemory(void)
{
	HINT32		i;
	HUINT32		total = 0;
	MemDebug_t	*dbg;
	MemGroup_t	*grp;

	dbg = _memdbg_Get();
	if (dbg)
	{
		for (i = 0; i < MEMDBG_MAX_GROUP; i++)
		{
			grp = dbg->group + i;
			total += grp->usedsize;
		}
		_memdbg_Release(dbg);
	}

	return total;
}

void	MEMDBG_CheckMem(HINT32 group, FILE *fp)
{
	HINT32		i;
	MemDebug_t	*dbg;
	MemGroup_t	*grp;

	dbg = _memdbg_Get();
	if (dbg)
	{
		for (i = 0; i < MEMDBG_MAX_GROUP; i++)
		{
			if (i == -1 || i == group)
			{
				grp = dbg->group + group;
				_memdbg_CheckMemGroup(grp, fp);
			}
		}

		_memdbg_Release(dbg);
	}
}

void *	MEMDBG_MemCpy (void *dest, const void *src, HUINT32 size, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;

	dbg = _memdbg_Get();
	if (dbg)
	{
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);
		return memcpy(dest, src, size);
	}
	return NULL;
}

void *	MEMDBG_MemSet (void *dest, HINT32 c, HUINT32 size, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;

	dbg = _memdbg_Get();
	if (dbg)
	{
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);
		return memset(dest, c, size);
	}
	return NULL;
}

HCHAR *	MEMDBG_StrCpy (HCHAR *dest, const HCHAR *src, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;
	HUINT32		size;

	dbg  = _memdbg_Get();
	if (dbg)
	{
		size = strlen(src) + 1;
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);
		return memcpy(dest, src, size);
	}
	return NULL;
}

HCHAR *	MEMDBG_StrNCpy (HCHAR *dest, const HCHAR *src, HUINT32 size, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;

	dbg  = _memdbg_Get();
	if (dbg)
	{
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);
		return strncpy(dest, src, size);
	}
	return NULL;
}

HCHAR *	MEMDBG_StrCat (HCHAR *dest, const HCHAR *src, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;
	HUINT32		size;

	dbg  = _memdbg_Get();
	if (dbg)
	{
		size  = strlen(dest);
		size += strlen(src) + 1;
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);
		return strcat(dest, src);
	}
	return NULL;
}

HCHAR *	MEMDBG_StrNCat (HCHAR *dest, const HCHAR *src, HUINT32 size, const HCHAR *file, HINT32 line)
{
	MemDebug_t *dbg;

	dbg  = _memdbg_Get();
	if (dbg)
	{
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);
		return strncat(dest, src, size);
	}
	return NULL;
}

HINT32	MEMDBG_Snprintf (const HCHAR *file, HINT32 line, HCHAR *dest, HUINT32 size, const HCHAR *format, ...)
{
	MemDebug_t *dbg;
	va_list		ap;
	HINT32		ret;

	dbg  = _memdbg_Get();
	if (dbg)
	{
		if (_memdbg_CheckCorruption(dbg, dest, size))
		{
			_memdbg_IFace()->Error("[%s] memory corruption [%s:%d]!!!\n", __FUNCTION__, _RMDIR(file), line);
		}
		_memdbg_Release(dbg);

		va_start(ap, format);
		ret = vsnprintf(dest, size, format, ap);
		va_end(ap);
		return ret;
	}
	return 0;
}

void MEMDBG_PrintBacktrace (void *pvPrintCbFunc)
{
	void			*framePtrs[MEMDBG_BACKTRACE_FRAME_NUM];
	int				num;
	MemPrintCb_t	 pfPrintCb = (MemPrintCb_t)pvPrintCbFunc;
#if !defined(MEMDBG_BACKTRACE_USE_HUMAX_FIX)
	HINT32		cnt;
	char		**btfuncnames = NULL;
#endif

	num = _memdbg_GetCallStack(framePtrs, MEMDBG_BACKTRACE_FRAME_NUM);
#if defined(MEMDBG_BACKTRACE_USE_HUMAX_FIX)
	if (num > 0)
	{
		(void)_memdbg_BacktraceSymbols(framePtrs, num, pfPrintCb);

		return;
	}

	pfPrintCb("something wrong... frameNum(%d):\n", num);
#if defined(CONFIG_PROD_CPU_BCM_ARM)
#if !defined (CONFIG_DEBUG_ARM_BACKTRACE)
	pfPrintCb("CONFIG_DEBUG_ARM_BACKTRACE is not set. turn on Menuconfig -> Debug -> DEBUG_ARM_BACKTRACE\n");
#endif
#endif
#else

	if (num > 0)
	{
		// get symbol info.
		btfuncnames = backtrace_symbols(framePtrs, num);
		if (NULL != btfuncnames)
		{
			for (cnt = 0; cnt < num; cnt++)
			{
				pfPrintCb("- Frame (%02d) : ptr(0x%08x) with [%s]\n", cnt, framePtrs[cnt], btfuncnames[cnt]);
			}

			free(btfuncnames);

			return;
		}
	}

	pfPrintCb("something wrong... frameNum(%d), btfuncnames(%p):\n", num, btfuncnames);
#if defined(CONFIG_PROD_CPU_BCM_ARM)
#if !defined (CONFIG_DEBUG_ARM_BACKTRACE)
	pfPrintCb("CONFIG_DEBUG_ARM_BACKTRACE is not set. turn on Menuconfig -> Debug -> DEBUG_ARM_BACKTRACE\n");
#endif
#endif

#endif
}
