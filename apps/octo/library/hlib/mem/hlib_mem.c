

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>

#include <hlib.h>

#include "_hlib_mem.h"

HCHAR *	__HLIB_MEM_StrDup (const HCHAR *str)
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

void *	__HLIB_MEM_MemDup (const void *mem, HUINT32 size)
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

void *	__HLIB_MEM_Calloc (HUINT32 nmemb, HUINT32 size)
{
	void *mem;

	mem = malloc(size * nmemb);
	if (mem)
		memset(mem, 0, nmemb * size);

	return mem;
}

