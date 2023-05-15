#ifndef __MEMORY_DEBUG_H__
#define __MEMORY_DEBUG_H__

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
extern void	MEMDBG_Init (
			  	  HINT32  (* log) (const HCHAR *fmt, ...)
				, HINT32  (* errlog) (const HCHAR *fmt, ...)
				, HUINT32 (* sem_create) (const HCHAR *name)
				, void    (* sem_destroy) (HUINT32 id)
				, HINT32  (* sem_lock) (HUINT32 id)
				, HINT32  (* sem_unlock) (HUINT32 id)
			);
extern void *	MEMDBG_Malloc (HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line);
extern void		MEMDBG_Free (void *mem, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_Calloc (HUINT32 nmemb, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_Realloc (void *mem, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line);
extern HCHAR *	MEMDBG_StrDup (const HCHAR *str, HINT32 group, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_MemDup (const void *mem, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line);
extern void		MEMDBG_Summary (HINT32 group, HUINT32 fromSec, HUINT32 durSec, HCHAR *szThreadName, HUINT32 ulMinSize, HUINT32 ulMaxSize, const HCHAR *filter, FILE *fp);
extern void		MEMDBG_GetBackTrace (HINT32 group, void *allocPtr, void *pvPrintCbFunc);
extern void		MEMDBG_LogTaskMemUsage (HCHAR *szTaskName, FILE *fp);
extern HUINT32	MEMDBG_GetUsedMemory(void);
extern void		MEMDBG_CheckMem(HINT32 group, FILE *fp);
extern void *	MEMDBG_MemCpy (void *dest, const void *src, HUINT32 size, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_MemSet (void *dest, HINT32 c, HUINT32 size, const HCHAR *file, HINT32 line);
extern HCHAR *	MEMDBG_StrCpy (HCHAR *dest, const HCHAR *src, const HCHAR *file, HINT32 line);
extern HCHAR *	MEMDBG_StrNCpy (HCHAR *dest, const HCHAR *src, HUINT32 size, const HCHAR *file, HINT32 line);
extern HCHAR *	MEMDBG_StrCat (HCHAR *dest, const HCHAR *src, const HCHAR *file, HINT32 line);
extern HCHAR *	MEMDBG_StrNCat (HCHAR *dest, const HCHAR *src, HUINT32 size, const HCHAR *file, HINT32 line);
extern HINT32	MEMDBG_Snprintf (const HCHAR *file, HINT32 line, HCHAR *dest, HUINT32 size, const HCHAR *format, ...);
extern void		MEMDBG_PrintBacktrace (void *pfPrintCb);


extern	void	MEMDBG_HLIB_CONF(HINT32 nStartSec, HINT32 nDurationSec, HERROR(* memdbg_print)(const HCHAR *format, ...));
extern HERROR	MEMDBG_HLIB_Print(const HCHAR *opt, const HCHAR *opt2, void * num);
extern void *	MEMDBG_HLIB_Malloc (HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line);
extern void		MEMDBG_HLIB_Free (void *mem, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_HLIB_Calloc (HUINT32 nmemb, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_HLIB_Realloc (void *mem, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line);
extern HCHAR *	MEMDBG_HLIB_StrDup (const HCHAR *str, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line);
extern void *	MEMDBG_HLIB_MemDup (const void *mem, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line);


#endif /* __MEMORY_DEBUG_H__ */


