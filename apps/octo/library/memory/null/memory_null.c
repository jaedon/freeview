
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>

#include <htype.h>
#include <vkernel.h>

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
}

void *	MEMDBG_Malloc (HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	return malloc(size);
}

void	MEMDBG_Free (void *mem, const HCHAR *file, HINT32 line)
{
	free(mem);
}

void *	MEMDBG_Calloc (HUINT32 nmemb, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	void *mem;

	mem = malloc(size * nmemb);
	if (mem)
		memset(mem, 0, nmemb * size);

	return mem;
}


void *	MEMDBG_Realloc (void *mem, HUINT32 size, HINT32 group, const HCHAR *file, HINT32 line)
{
	return realloc(mem, size);
}



HCHAR *	MEMDBG_StrDup (const HCHAR *str, HINT32 group, const HCHAR *file, HINT32 line)
{
	HUINT32	len;
	HCHAR *	dup;

	if (str)
	{
		len = strlen(str);
		dup = (HCHAR *)malloc(len + 1);
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
	void *dup;

	if (mem && size)
	{
		dup = malloc(size);
		if (dup)
		{
			memcpy(dup, mem, size);
		}
		return dup;
	}

	return NULL;
}

void *	MEMDBG_HLIB_Malloc (HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	return malloc(size);
}
void	MEMDBG_HLIB_Free (void *mem, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	free(mem);
}

void *	MEMDBG_HLIB_Calloc (HUINT32 nmemb, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void *mem;

	mem = malloc(size * nmemb);
	if (mem)
		memset(mem, 0, nmemb * size);

	return mem;
}

void *	MEMDBG_HLIB_Realloc (void *mem, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	return realloc(mem, size);
}
HCHAR *	MEMDBG_HLIB_StrDup (const HCHAR *str, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	HUINT32 len;
	HCHAR * dup;

	if (str)
	{
		len = strlen(str);
		dup = (HCHAR *)malloc(len + 1);
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

void *	MEMDBG_HLIB_MemDup (const void *mem, HUINT32 size, void *handle, const HCHAR *orgFile, HINT32 orgLine, const HCHAR *file, HINT32 line)
{
	void *dup;

	if (mem && size)
	{
		dup = malloc(size);
		if (dup)
		{
			memcpy(dup, mem, size);
		}
		return dup;
	}

	return NULL;
}

void	MEMDBG_HLIB_CONF(HINT32 nStartSec, HINT32 nDurationSec, HERROR (* memdbg_print)(const HCHAR *format, ...))
{
}

HERROR	MEMDBG_HLIB_Print(const HCHAR *opt, const HCHAR *opt2, void * num)
{
	return ERR_OK;
}

void	MEMDBG_Summary (HINT32 group, HUINT32 fromSec, HUINT32 durSec, HCHAR *szThreadName, HUINT32 ulMinSize, HUINT32 ulMaxSize, const HCHAR *filter, FILE *fp)
{
}

void	MEMDBG_GetBackTrace (HINT32 group, void *allocPtr, void *pvPrintCbFunc)
{
}

void	MEMDBG_LogTaskMemUsage (HCHAR *szTaskName, FILE *fp)
{
}

HUINT32	MEMDBG_GetUsedMemory(void)
{
	return 0;
}

void	MEMDBG_CheckMem(HINT32 group, FILE *fp)
{
}

void *	MEMDBG_MemCpy (void *dest, const void *src, HUINT32 size, const HCHAR *file, HINT32 line)
{
	return memcpy(dest, src, size);
}

void *	MEMDBG_MemSet (void *dest, HINT32 c, HUINT32 size, const HCHAR *file, HINT32 line)
{
	return memset(dest, c, size);
}

HCHAR *	MEMDBG_StrCpy (HCHAR *dest, const HCHAR *src, const HCHAR *file, HINT32 line)
{
	return strcpy(dest, src);
}

HCHAR *	MEMDBG_StrNCpy (HCHAR *dest, const HCHAR *src, HUINT32 size, const HCHAR *file, HINT32 line)
{
	return strncpy(dest, src, size);
}

HCHAR *	MEMDBG_StrCat (HCHAR *dest, const HCHAR *src, const HCHAR *file, HINT32 line)
{
	return strcat(dest, src);
}

HCHAR *	MEMDBG_StrNCat (HCHAR *dest, const HCHAR *src, HUINT32 size, const HCHAR *file, HINT32 line)
{
	return strncat(dest, src, size);
}

HINT32	MEMDBG_Snprintf (const HCHAR *file, HINT32 line, HCHAR *dest, HUINT32 size, const HCHAR *format, ...)
{
	va_list		ap;
	HINT32		ret;

	va_start(ap, format);
	ret = vsnprintf(dest, size, format, ap);
	va_end(ap);

	return ret;
}
void MEMDBG_PrintBacktrace (void *pvPrintCbFunc)
{
	return;
}

