/*
 * utils.c
 */

#if !defined(WIN32)
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "utils.h"

/*
 * returns a PIX_FMT_xxx type that matches the given bits per pixel and RGB bit mask values
 * returns PIX_FMT_NONE if none match
 */
#if 0
enum PixelFormat
find_av_pix_fmt(int bpp, unsigned long rmask, unsigned long gmask, unsigned long bmask)
{
	enum PixelFormat fmt;

	fmt = PIX_FMT_NONE;
	switch(bpp)
	{
	case 32:
		if(rmask == 0xff0000 && gmask == 0xff00 && bmask == 0xff)
			fmt = PIX_FMT_RGBA32;
		break;

	case 24:
		if(rmask == 0xff0000 && gmask == 0xff00 && bmask == 0xff)
			fmt = PIX_FMT_RGB24;
		else if(rmask == 0xff && gmask == 0xff00 && bmask == 0xff0000)
			fmt = PIX_FMT_BGR24;
		break;

	case 16:
		if(rmask == 0xf800 && gmask == 0x07e0 && bmask == 0x001f)
			fmt = PIX_FMT_RGB565;
		else if(rmask == 0x7c00 && gmask == 0x03e0 && bmask == 0x001f)
			fmt = PIX_FMT_RGB555;
		break;

	default:
		break;
	}

	return fmt;
}
#endif

/*
 * returns 15 for 'f' etc
 */

unsigned int
char2hex(unsigned char c)
{
	if(!isxdigit(c))
		return 0;
	else if(c >= '0' && c <= '9')
		return c - '0';
	else
		return 10 + (tolower(c) - 'a');
}

/*
 * returns the next UTF8 character in the given text
 * size should be the amount of data available in text
 * sets *used to the number of bytes in the UTF8 character
 * gives up if the char is more than 4 bytes long
 */

int
next_utf8(unsigned char *text, int size, int *used)
{
	if(size >= 1 && (text[0] & 0x80) == 0)
	{
		*used = 1;
		return text[0];
	}
	else if(size >= 2 && (text[0] & 0xe0) == 0xc0)
	{
		*used = 2;
		return ((text[0] & 0x1f) << 6) + (text[1] & 0x3f);
	}
	else if(size >= 3 && (text[0] & 0xf0) == 0xe0)
	{
		*used = 3;
		return ((text[0] & 0x0f) << 12) + ((text[1] & 0x3f) << 6) + (text[2] & 0x3f);
	}
	else if(size >= 4 && (text[0] & 0xf8) == 0xf0)
	{
		*used = 4;
		return ((text[0] & 0x07) << 18) + ((text[1] & 0x3f) << 12) + ((text[2] & 0x3f) << 6) + (text[3] & 0x3f);
	}
	else if(size > 0)
	{
		/* error, return the next char */
		*used = 1;
		return text[0];
	}
	else
	{
		*used = 0;
		return 0;
	}
}

/*
 * I don't want to double the size of my code just to deal with malloc failures
 * if you've run out of memory you're fscked anyway, me trying to recover is not gonna help...
 */

#ifdef DEBUG_MALLOC

static int __malloc_count = 0;

void safe_print_malloc_count(void)
{
	Hx_Print("* Memory Allocation Count : %d\n", __malloc_count);
}

#endif

#ifdef DEBUG_MALLOC_LIST

#define MAX_MALLOC_LIST 1000

typedef struct
{
	void *ptr;
	size_t size;
	char caller[29+1];
	int line;
} malloc_info;

static malloc_info __malloc_list[MAX_MALLOC_LIST] = {{NULL, 0},};

void safe_print_malloc_list(void)
{
	int i, count= 0;
	size_t sum = 0;

	Hx_Print("* Memory Allocation List\n");

	for (i = 0; i < MAX_MALLOC_LIST; i++)
	{
		if (__malloc_list[i].ptr)
		{
			Hx_Print("\t%d : %p, %d - %s %d lines\n", i, __malloc_list[i].ptr, __malloc_list[i].size,  __malloc_list[i].caller,  __malloc_list[i].line);
			count++;
			sum += __malloc_list[i].size;
		}
	}

	Hx_Print("* Memory Allocation Size : %d bytes by %d alloc\n", sum, count);
}

#endif

void *
#ifdef DEBUG_MALLOC_LIST
safe_malloc_(size_t nbytes, char *caller, int line)
#else
safe_malloc(size_t nbytes)
#endif
{
	void *buf;

	#ifdef DEBUG_MALLOC_LIST
	if (nbytes == 0) fatal("nbytes is zero - %s %d", caller, line);
	#else
	if (nbytes == 0) fatal("nbytes is zero");
	#endif

	if((buf = HLIB_STD_MemAlloc(nbytes)) == NULL)
		fatal("Out of memory");

	#ifdef DEBUG_MALLOC
	__malloc_count ++;
//	fprintf(stderr, "safe_malloc: %d\n", __malloc_count);
	#endif

	#ifdef DEBUG_MALLOC_LIST
	{
		int i;
		for (i = 0; i < MAX_MALLOC_LIST; i++)
		{
			if (__malloc_list[i].ptr == NULL)
			{
				__malloc_list[i].ptr = buf;
				__malloc_list[i].size = nbytes;
				strncpy(__malloc_list[i].caller, caller, 29);
				__malloc_list[i].line = line;
				break;
			}
		}
		if (i == MAX_MALLOC_LIST) HxLOG_Error("malloc_list full !!!\n");
	}
	#endif

	return buf;
}

void *
#ifdef DEBUG_MALLOC_LIST
safe_mallocz_(size_t nbytes, char *caller, int line)
#else
safe_mallocz(size_t nbytes)
#endif
{
	#ifdef DEBUG_MALLOC_LIST
	void *buf = safe_malloc_(nbytes, caller, line);
	#else
	void *buf = safe_malloc(nbytes);
	#endif

	VK_memset(buf, 0x00, nbytes);

	return buf;
}

/*
 * safe_realloc(NULL, n) == safe_malloc(n)
 * safe_realloc(x, 0) == safe_free(x) and returns NULL
 */

void *
#ifdef DEBUG_MALLOC_LIST
safe_realloc_(void *oldbuf, size_t nbytes, char *caller, int line)
#else
safe_realloc(void *oldbuf, size_t nbytes)
#endif
{
	void *newbuf;

	if(nbytes == 0)
	{
		safe_free(oldbuf);
		return NULL;
	}

	#ifdef DEBUG_MALLOC_LIST
	if(oldbuf == NULL)
		return safe_malloc_(nbytes, caller, line);
	#else
	if(oldbuf == NULL)
		return safe_malloc(nbytes);
	#endif

	if((newbuf = HLIB_STD_MemRealloc(oldbuf, nbytes)) == NULL)
		fatal("Out of memory");

	#ifdef DEBUG_MALLOC_LIST
	{
		int i;
		for (i = 0; i < MAX_MALLOC_LIST; i++)
		{
			if (__malloc_list[i].ptr == oldbuf)
			{
				__malloc_list[i].ptr = newbuf;
				__malloc_list[i].size = nbytes;
				strncpy(__malloc_list[i].caller, caller, 29);
				__malloc_list[i].line = line;
				break;
			}
		}
		if (i == MAX_MALLOC_LIST) HxLOG_Error("Not found %p at the malloc_list (safe_realloc) !!!\n", oldbuf);
	}
	#endif

	return newbuf;
}

/*
 * only calls safe_realloc if nbytes > *oldsize
 * updates *oldsize if it calls safe_realloc
 */

void *
#ifdef DEBUG_MALLOC_LIST
safe_fast_realloc_(void *oldbuf, size_t *oldsize, size_t nbytes, char *caller, int line)
#else
safe_fast_realloc(void *oldbuf, size_t *oldsize, size_t nbytes)
#endif
{
	if(nbytes > *oldsize)
	{
		#ifdef DEBUG_MALLOC_LIST
		oldbuf = safe_realloc_(oldbuf, nbytes, caller, line);
		#else
		oldbuf = safe_realloc(oldbuf, nbytes);
		#endif
		*oldsize = nbytes;
	}

	return oldbuf;
}

/*
 * safe_strdup(NULL) == NULL
 */

char *
#ifdef DEBUG_MALLOC_LIST
safe_strdup_(const char *src, char *caller, int line)
#else
safe_strdup(const char *src)
#endif
{
	char *dst;

	if(src == NULL)
		return NULL;

	#ifdef DEBUG_MALLOC_LIST
	dst = (char *) safe_malloc_(strlen(src) + 1, caller, line);
	#else
	dst = (char *) safe_malloc(strlen(src) + 1);
	#endif
	strcpy(dst, src);

	return dst;
}

/*
 * safe_free(NULL) is okay
 */

void
safe_free(void *buf)
{
	char bfree = 0;

	if(buf != NULL)
	{
#ifdef DEBUG_MALLOC
		__malloc_count --;
//		fprintf(stderr, "safe_free: %d\n", __malloc_count);
#endif

#ifdef DEBUG_MALLOC_LIST
		{
			int i;
			for (i = 0; i < MAX_MALLOC_LIST; i++)
			{
				if (__malloc_list[i].ptr == buf)
				{
					__malloc_list[i].ptr = NULL;
					__malloc_list[i].size = 0;
					break;
				}
			}
			if (i == MAX_MALLOC_LIST)
			{
				HxLOG_Error("Not found %p at the malloc_list (safe_free)!!!\n", buf);
				//safe_print_malloc_list();
				bfree = 1;
			}
		}
#endif
		if (bfree == 0)
			HLIB_STD_MemFree(buf);
	}

	return;
}

void
error_at(char *fn, int line, char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	Hx_Print("[IB:ERROR] ");
	vprintf(message, ap);
	Hx_Print(" at %s() %d !!!\n", fn, line);
	va_end(ap);

	return;
}

void
fatal_at(char *fn, int line, char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	Hx_Print("[IB:ERROR] ");
	vprintf(message, ap);
	Hx_Print(" at %s() %d (fatal) !!!\n", fn, line);
	va_end(ap);
}

int g_MhegIbDbgFlag = (MHEG_IB_DBG_MASK_TODO | MHEG_IB_DBG_MASK_TODO_PORTING);

//int g_MhegIbDbgFlag = MHEG_IB_DBG_MASK_ALL;//(MHEG_IB_DBG_MASK_AMMI | MHEG_IB_DBG_MASK_TODO | MHEG_IB_DBG_MASK_TODO_PORTING);

#ifdef CONFIG_DEBUG
static char _szMhegIbPrintBuffer[1024];
void
verbose_(int flag, char *message, ...)
{
	va_list ap;

	if (g_MhegIbDbgFlag & flag)
	{
		va_start(ap, message);
		if (flag == MHEG_IB_DBG_MASK_MAIN)
			HxLOG_Print("[IB:MAIN]  ");
		else if (flag == MHEG_IB_DBG_MASK_FILE)
			HxLOG_Print("[IB:FILE]  ");
		else if (flag == MHEG_IB_DBG_MASK_CLS)
			HxLOG_Print("[IB:CLASS] ");
		else if (flag == MHEG_IB_DBG_MASK_EVT)
			HxLOG_Print("[IB:EVENT] ");
		else if (flag == MHEG_IB_DBG_MASK_OSD)
			HxLOG_Print("[IB:OSD]   ");
		else if (flag == MHEG_IB_DBG_MASK_AVSTR)
			HxLOG_Print("[IB:AVSTR] ");
		else if (flag == MHEG_IB_DBG_MASK_AMMI)
			HxLOG_Print("[IB:AMMI]  ");
		else if (flag == MHEG_IB_DBG_MASK_DSM)
			HxLOG_Print("[IB:DSM]   ");
		else if (flag == MHEG_IB_DBG_MASK_TODO)
			HxLOG_Print("[IB:TODO]  ");
		else if (flag == MHEG_IB_DBG_MASK_TODO_PORTING)
			HxLOG_Print("[IB:PORT]  ");
		else if (flag == MHEG_IB_DBG_MASK_MISC)
			HxLOG_Print("[IB:MISC]  ");
		vsprintf(_szMhegIbPrintBuffer, message, ap);
		HxLOG_Print("%s\n", _szMhegIbPrintBuffer);
		va_end(ap);
	}

	return;
}

#endif

