/*
 * Copyright (c) 2006 HUMAX Co., Ltd. All rights reserved.
 *
 * This is very confidential.
 * This code must not distributed outside.
 */

#include "stdio.h"
#include "vkernel.h"
#include "htype.h"

#if defined(CONFIG_VK_STDLIB)
#else
	#include 	<stdio.h>
	#include 	<stdlib.h>
	#include 	<string.h>
	#include	<ctype.h>
#endif

#include <stdarg.h>

#define DEBUG_MODULE			TRACE_MODULE_VK

#ifdef KERNEL_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* KERNEL_DEBUG */

#if defined(CONFIG_DEBUG) || !defined(VK_DBG_Print)
static HUINT32	s_ulRefTick = 0;
static HUINT32	s_ulRefTick2 = 0;
#endif

static int	s_vk_initialized;

extern int VK_MEM_Init(void);
extern int VK_MSG_Init(void);
extern int VK_SEM_Init(void);
extern int VK_TASK_Init(void);
extern int VK_TIMER_Init(void);
extern int VK_CLOCK_Init (void);
extern int VK_IPC_MSG_Init(void);
extern int VK_INTR_Init(void);

int VK_Init(void)
{
	int	vk_res;

	if (s_vk_initialized)
	{
		PrintError("%s : vkernel was already initalized!\n", __FUNCTION__);
		return VK_OK;
	}

	vk_res = VK_SEM_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_SEM_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_MEM_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_MEM_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_MSG_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_MSG_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_TASK_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_TASK_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_INTR_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_INTR_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_TIMER_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_TIMER_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_CLOCK_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_CLOCK_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_IPC_MSG_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_MSG_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_SIG_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_SIG_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_SOCKET_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_SOCKET_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

	vk_res = VK_RPC_Init();
	if (vk_res != VK_OK)
	{
		PrintError("%s : VK_RPC_Init() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR;
	}

#if defined(MEMORY_DEBUG)
	{
		extern void VK_DBGMEM_InitCheckTask(void);

		VK_DBGMEM_InitCheckTask();
	}
#endif

	s_vk_initialized = 1;
	return VK_OK;
}

void VK_Panic( void )
{
	*((unsigned long *)NULL) = (unsigned long)0;
}

#if defined(CONFIG_DEBUG)
void VK_DBG_SetRefTick(unsigned char *Str)
{
	s_ulRefTick = VK_TIMER_GetSystemTick();

	VK_DBG_Print("%-30s\t==> %10u\r\n", Str, s_ulRefTick);

	s_ulRefTick2 = s_ulRefTick;
}
#endif

#if !defined(VK_DBG_Print)
void	VK_DBG_Print(const char *fmt, ...)
{
	unsigned long	ticks;
	char			buf[512+24];
	char			*pbuf;
	va_list			v;

	pbuf = buf;

	ticks = VK_TIMER_GetSystemTick();

	snprintf(pbuf, 512, "[%04d:%02d, %5lu, %5lu] ",
			(int)(( ticks / VK_TIMER_GetTickFreq() ) % 10000),
			(int)(( ticks % VK_TIMER_GetTickFreq() ) / 10), (ticks-s_ulRefTick), (ticks - s_ulRefTick2));

	pbuf += 24;	// strlen("[%04d:%02d, %5u, %5u] ")=24

	s_ulRefTick2 = ticks;

    va_start(v, fmt);
    vsnprintf(pbuf, 512, fmt, v);
    va_end(v);

	VK_printf("%s", buf);
}
#endif



HCHAR *		VK_strreplacechar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to)
{
	HCHAR *ret = dst;

	while (*src != '\0')
	{
		if (*src == from)
			*dst++ = to;
		else
			*dst++ = *src;
		src++;
	}
	*dst = *src;
	return ret;
}

HCHAR *		VK_strreplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to)
{
	HCHAR  *ret = dst;
	HUINT32 len = VK_strlen(from);
	HUINT32	adv = VK_strlen(to);

	if (len == 0 || adv == 0)
	{
		VK_strcpy(dst, src);
		return ret;
	}

	while (*src != '\0')
	{
		if (VK_strncmp(src, from, len) == 0)
		{
			VK_strncpy(dst, to, adv);
			src += len;
			dst += adv;
		}
		else
		{
			*dst++ = *src++;
		}
	}
	*dst++ = *src++;

	return ret;
}

HCHAR*		VK_strcasestr(const HCHAR *s1, const HCHAR *s2)
{
	HINT32	wanted;
	HUINT32	len;

	len = VK_strlen(s2);
	if (len == 0)
		return (HCHAR *)s1;

	wanted = VK_toupper(*s2);
	while (VK_toupper(*s1) != wanted || VK_strncasecmp(s1, s2, len))
	{
		if (*s1++ == '\0')
			return NULL;
	}
	return (HCHAR *)s1;
}

HCHAR *	VK_strdupex (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32))
{
	HUINT32 ulLen;
	HCHAR *	szNew = NULL;

	if (szOld)
	{
		ulLen = VK_strlen(szOld);
		if (n < ulLen)
			ulLen = n;

		szNew = _alloc(ulLen + 1);
		if (szNew)
		{
			VK_MEM_Memcpy((void *)szNew, (void *)szOld, ulLen);
			szNew[ulLen] = '\0';
		}
	}
	return szNew;
}


void *	VK_memdupex(const void *data, HUINT32 size, void *(_alloc)(HUINT32))
{
	void *	dup;

	dup = _alloc(size);
	if (dup)
	{
		VK_MEM_Memcpy((void *)dup, (void *)data, size);
	}
	return dup;
}

void *	VK_memdup(const void *data, HUINT32 size)
{
	void *	dup;

	dup = VK_MEM_Alloc(size);
	if (dup)
	{
		VK_MEM_Memcpy((void *)dup, (void *)data, size);
	}
	return dup;
}

HUINT32		VK_atoh(const HCHAR *s)
{
	HUINT32		val = 0;
	HINT32		x = 0;

	if(s[x] == '0' && (s[x+1]=='x' || s[x+1]=='X')) x+=2;

	while(s[x]!='\0')
	{
		if((s[x] == 'X') || (s[x] == 'x'))
		{
			x++;
			continue;
		}
		else if(s[x] >= '0' && s[x] <='9')
		{
		  val = (val << 4) + s[x] - '0';
		}
		else if(s[x]>='A' && s[x] <='F')
		{
		  val = (val << 4) + s[x] - 'A' + 10;
		}
		else if(s[x]>='a' && s[x] <='f')
		{
		  val = (val << 4) + s[x] - 'a' + 10;
		}
		else return val;

	   x++;
	}
	return val;
}

HUINT32		VK_atoui (const HCHAR *s)
{
	HUINT32	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return VK_atoh(s);
	}

	for (n = 0 ; isdigit(*s) ; s++)
	{
		n = n * 10 + (*s - '0');
	}
	return n;
}


HBOOL		VK_strisnumber (const HCHAR *s)
{
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		s += 2;
		while (*s != '\0')
		{
			if (!VK_isdigit(*s))
				return FALSE;
			s++;
		}
		return TRUE;
	}

	if (*s == '+' || *s == '-')
		s++;

	while (*s != '\0')
	{
		if (!VK_isdigit(*s))
			return FALSE;
		s++;
	}
	return TRUE;
}

HBOOL VK_ishexdigit(const HUINT8 ch)
{
	if('0' <= ch && ch <= '9')	return TRUE;
	if('a' <= ch && ch <= 'f')	return TRUE;
	if('A' <= ch && ch <= 'F')	return TRUE;

	return FALSE;
}

HCHAR	*VK_strlower (HCHAR *dest, const HCHAR *src)
{
	HCHAR	ch;
	HCHAR 	*p = dest;

	while ((ch = *src++)) {
		*p++ = VK_tolower(ch);
	}

	return dest;
}


HCHAR	*VK_strupper (HCHAR *dest, const HCHAR *src)
{
	HCHAR	ch;
	HCHAR 	*p = dest;

	while ((ch = *src++)) {
		*p++ = VK_toupper(ch);
	}

	return dest;
}



