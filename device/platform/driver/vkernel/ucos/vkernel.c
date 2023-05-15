/*
 * Copyright (c) 2006 HUMAX Co., Ltd. All rights reserved.
 *
 * This is very confidential.
 * This code must not distributed outside.
 */

#if defined(CONFIG_VK_STDLIB)
#else
	#include 	<stdio.h>
	#include 	<stdlib.h>
	#include 	<string.h>
#endif

#include <linden_trace.h>
#include <stdarg.h>

#include "vkernel.h"
#include "os.h"

#define DEBUG_MODULE			TRACE_MODULE_VK

#if defined(CONFIG_DEBUG) || !defined(VK_DBG_Print)
static HUINT32	s_ulRefTick = 0;
static HUINT32	s_ulRefTick2 = 0;
#endif

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
	VK_SEM_Init();
	VK_INTR_Init();
	VK_TIMER_Init();
	return VK_OK;
}

void VK_Panic( void )
{
	return;
}

#if defined(CONFIG_DEBUG)
void VK_DBG_SetRefTick(unsigned char *Str)
{
	HAPPY(Str);
}
#endif


#undef ENUM_TO_STR
#define ENUM_TO_STR(enumVal) 			case enumVal: return (HINT8 *)#enumVal;

HINT8 *VK_DBG_OSErr2Str(HINT32 ucosErr/* OS_ERR */)
{
	switch((OS_ERR)ucosErr)
	{
	ENUM_TO_STR(OS_ERR_A)
	ENUM_TO_STR(OS_ERR_ACCEPT_ISR)
	ENUM_TO_STR(OS_ERR_B)
	ENUM_TO_STR(OS_ERR_C)
	ENUM_TO_STR(OS_ERR_CREATE_ISR)
	ENUM_TO_STR(OS_ERR_D)
	ENUM_TO_STR(OS_ERR_DEL_ISR)
	ENUM_TO_STR(OS_ERR_E)
	ENUM_TO_STR(OS_ERR_F)
	ENUM_TO_STR(OS_ERR_FATAL_RETURN)
	ENUM_TO_STR(OS_ERR_FLAG_GRP_DEPLETED)
	ENUM_TO_STR(OS_ERR_FLAG_NOT_RDY)
	ENUM_TO_STR(OS_ERR_FLAG_PEND_OPT)
	ENUM_TO_STR(OS_ERR_FLUSH_ISR)
	ENUM_TO_STR(OS_ERR_G)
	ENUM_TO_STR(OS_ERR_H)
	ENUM_TO_STR(OS_ERR_I)
	ENUM_TO_STR(OS_ERR_ILLEGAL_CREATE_RUN_TIME)
	ENUM_TO_STR(OS_ERR_INT_Q)
	ENUM_TO_STR(OS_ERR_INT_Q_FULL)
	ENUM_TO_STR(OS_ERR_INT_Q_SIZE)
	ENUM_TO_STR(OS_ERR_INT_Q_STK_INVALID)
	ENUM_TO_STR(OS_ERR_INT_Q_STK_SIZE_INVALID)
	ENUM_TO_STR(OS_ERR_J)
	ENUM_TO_STR(OS_ERR_K)
	ENUM_TO_STR(OS_ERR_L)
	ENUM_TO_STR(OS_ERR_LOCK_NESTING_OVF)
	ENUM_TO_STR(OS_ERR_M)
	ENUM_TO_STR(OS_ERR_MEM_CREATE_ISR)
	ENUM_TO_STR(OS_ERR_MEM_FULL)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_BLKS)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_P_ADDR)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_P_BLK)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_P_DATA)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_P_MEM)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_PART)
	ENUM_TO_STR(OS_ERR_MEM_INVALID_SIZE)
	ENUM_TO_STR(OS_ERR_MEM_NO_FREE_BLKS)
	ENUM_TO_STR(OS_ERR_MSG_POOL_EMPTY)
	ENUM_TO_STR(OS_ERR_MSG_POOL_NULL_PTR)
	ENUM_TO_STR(OS_ERR_MUTEX_NESTING)
	ENUM_TO_STR(OS_ERR_MUTEX_NOT_OWNER)
	ENUM_TO_STR(OS_ERR_MUTEX_OWNER)
	ENUM_TO_STR(OS_ERR_N)
	ENUM_TO_STR(OS_ERR_NAME)
	ENUM_TO_STR(OS_ERR_NO_MORE_ID_AVAIL)
	ENUM_TO_STR(OS_ERR_NONE)
	ENUM_TO_STR(OS_ERR_O)
	ENUM_TO_STR(OS_ERR_OBJ_CREATED)
	ENUM_TO_STR(OS_ERR_OBJ_DEL)
	ENUM_TO_STR(OS_ERR_OBJ_PTR_NULL)
	ENUM_TO_STR(OS_ERR_OBJ_TYPE)
	ENUM_TO_STR(OS_ERR_OPT_INVALID)
	ENUM_TO_STR(OS_ERR_OS_NOT_RUNNING)
	ENUM_TO_STR(OS_ERR_OS_RUNNING)
	ENUM_TO_STR(OS_ERR_P)
	ENUM_TO_STR(OS_ERR_PEND_ABORT)
	ENUM_TO_STR(OS_ERR_PEND_ABORT_ISR)
	ENUM_TO_STR(OS_ERR_PEND_ABORT_NONE)
	ENUM_TO_STR(OS_ERR_PEND_ABORT_SELF)
	ENUM_TO_STR(OS_ERR_PEND_DEL)
	ENUM_TO_STR(OS_ERR_PEND_ISR)
	ENUM_TO_STR(OS_ERR_PEND_LOCKED)
	ENUM_TO_STR(OS_ERR_PEND_WOULD_BLOCK)
	ENUM_TO_STR(OS_ERR_POST_ISR)
	ENUM_TO_STR(OS_ERR_POST_NULL_PTR)
	ENUM_TO_STR(OS_ERR_PRIO)
	ENUM_TO_STR(OS_ERR_PRIO_EXIST)
	ENUM_TO_STR(OS_ERR_PRIO_INVALID)
	ENUM_TO_STR(OS_ERR_PTR_INVALID)
	ENUM_TO_STR(OS_ERR_Q)
	ENUM_TO_STR(OS_ERR_Q_EMPTY)
	ENUM_TO_STR(OS_ERR_Q_FULL)
	ENUM_TO_STR(OS_ERR_Q_MAX)
	ENUM_TO_STR(OS_ERR_Q_SIZE)
	ENUM_TO_STR(OS_ERR_R)
	ENUM_TO_STR(OS_ERR_REG_ID_INVALID)
	ENUM_TO_STR(OS_ERR_ROUND_ROBIN_1)
	ENUM_TO_STR(OS_ERR_ROUND_ROBIN_DISABLED)
	ENUM_TO_STR(OS_ERR_S)
	ENUM_TO_STR(OS_ERR_SCHED_INVALID_TIME_SLICE)
	ENUM_TO_STR(OS_ERR_SCHED_LOCK_ISR)
	ENUM_TO_STR(OS_ERR_SCHED_LOCKED)
	ENUM_TO_STR(OS_ERR_SCHED_NOT_LOCKED)
	ENUM_TO_STR(OS_ERR_SCHED_UNLOCK_ISR)
	ENUM_TO_STR(OS_ERR_SEM_OVF)
	ENUM_TO_STR(OS_ERR_SET_ISR)
	ENUM_TO_STR(OS_ERR_STAT_PRIO_INVALID)
	ENUM_TO_STR(OS_ERR_STAT_RESET_ISR)
	ENUM_TO_STR(OS_ERR_STAT_STK_INVALID)
	ENUM_TO_STR(OS_ERR_STAT_STK_SIZE_INVALID)
	ENUM_TO_STR(OS_ERR_STATE_INVALID)
	ENUM_TO_STR(OS_ERR_STATUS_INVALID)
	ENUM_TO_STR(OS_ERR_STK_INVALID)
	ENUM_TO_STR(OS_ERR_STK_LIMIT_INVALID)
	ENUM_TO_STR(OS_ERR_STK_SIZE_INVALID)
	ENUM_TO_STR(OS_ERR_T)
	ENUM_TO_STR(OS_ERR_TASK_CHANGE_PRIO_ISR)
	ENUM_TO_STR(OS_ERR_TASK_CREATE_ISR)
	ENUM_TO_STR(OS_ERR_TASK_DEL)
	ENUM_TO_STR(OS_ERR_TASK_DEL_IDLE)
	ENUM_TO_STR(OS_ERR_TASK_DEL_INVALID)
	ENUM_TO_STR(OS_ERR_TASK_DEL_ISR)
	ENUM_TO_STR(OS_ERR_TASK_INVALID)
	ENUM_TO_STR(OS_ERR_TASK_NO_MORE_TCB)
	ENUM_TO_STR(OS_ERR_TASK_NOT_DLY)
	ENUM_TO_STR(OS_ERR_TASK_NOT_EXIST)
	ENUM_TO_STR(OS_ERR_TASK_NOT_SUSPENDED)
	ENUM_TO_STR(OS_ERR_TASK_OPT)
	ENUM_TO_STR(OS_ERR_TASK_RESUME_ISR)
	ENUM_TO_STR(OS_ERR_TASK_RESUME_PRIO)
	ENUM_TO_STR(OS_ERR_TASK_RESUME_SELF)
	ENUM_TO_STR(OS_ERR_TASK_RUNNING)
	ENUM_TO_STR(OS_ERR_TASK_STK_CHK_ISR)
	ENUM_TO_STR(OS_ERR_TASK_SUSPEND_IDLE)
	ENUM_TO_STR(OS_ERR_TASK_SUSPEND_INT_HANDLER)
	ENUM_TO_STR(OS_ERR_TASK_SUSPEND_ISR)
	ENUM_TO_STR(OS_ERR_TASK_SUSPEND_PRIO)
	ENUM_TO_STR(OS_ERR_TASK_SUSPENDED)
	ENUM_TO_STR(OS_ERR_TASK_WAITING)
	ENUM_TO_STR(OS_ERR_TCB_INVALID)
	ENUM_TO_STR(OS_ERR_TICK_PRIO_INVALID)
	ENUM_TO_STR(OS_ERR_TICK_STK_INVALID)
	ENUM_TO_STR(OS_ERR_TICK_STK_SIZE_INVALID)
	ENUM_TO_STR(OS_ERR_TICK_WHEEL_SIZE)
	ENUM_TO_STR(OS_ERR_TIME_DLY_ISR)
	ENUM_TO_STR(OS_ERR_TIME_DLY_RESUME_ISR)
	ENUM_TO_STR(OS_ERR_TIME_GET_ISR)
	ENUM_TO_STR(OS_ERR_TIME_INVALID_HOURS)
	ENUM_TO_STR(OS_ERR_TIME_INVALID_MILLISECONDS)
	ENUM_TO_STR(OS_ERR_TIME_INVALID_MINUTES)
	ENUM_TO_STR(OS_ERR_TIME_INVALID_SECONDS)
	ENUM_TO_STR(OS_ERR_TIME_NOT_DLY)
	ENUM_TO_STR(OS_ERR_TIME_SET_ISR)
	ENUM_TO_STR(OS_ERR_TIME_ZERO_DLY)
	ENUM_TO_STR(OS_ERR_TIMEOUT)
	ENUM_TO_STR(OS_ERR_TLS_DESTRUCT_ASSIGNED)
	ENUM_TO_STR(OS_ERR_TLS_ID_INVALID)
	ENUM_TO_STR(OS_ERR_TLS_ISR)
	ENUM_TO_STR(OS_ERR_TLS_NO_MORE_AVAIL)
	ENUM_TO_STR(OS_ERR_TLS_NOT_EN)
	ENUM_TO_STR(OS_ERR_TMR_INACTIVE)
	ENUM_TO_STR(OS_ERR_TMR_INVALID)
	ENUM_TO_STR(OS_ERR_TMR_INVALID_DEST)
	ENUM_TO_STR(OS_ERR_TMR_INVALID_DLY)
	ENUM_TO_STR(OS_ERR_TMR_INVALID_PERIOD)
	ENUM_TO_STR(OS_ERR_TMR_INVALID_STATE)
	ENUM_TO_STR(OS_ERR_TMR_ISR)
	ENUM_TO_STR(OS_ERR_TMR_NO_CALLBACK)
	ENUM_TO_STR(OS_ERR_TMR_NON_AVAIL)
	ENUM_TO_STR(OS_ERR_TMR_PRIO_INVALID)
	ENUM_TO_STR(OS_ERR_TMR_STK_INVALID)
	ENUM_TO_STR(OS_ERR_TMR_STK_SIZE_INVALID)
	ENUM_TO_STR(OS_ERR_TMR_STOPPED)
	ENUM_TO_STR(OS_ERR_U)
	ENUM_TO_STR(OS_ERR_V)
	ENUM_TO_STR(OS_ERR_W)
	ENUM_TO_STR(OS_ERR_X)
	ENUM_TO_STR(OS_ERR_Y)
	ENUM_TO_STR(OS_ERR_YIELD_ISR)
	ENUM_TO_STR(OS_ERR_Z)
	default:
		break;
	}
	return (HINT8*)"";
}

#define DEF_PRINTF_MAX_BUF 1024

HCHAR *VK_strcpy(HCHAR *dest, const HCHAR *src)
{
	HCHAR *ptr = dest;

	if( NULL == dest || NULL == src )
		return dest;

	while (*src)
	{
		*ptr++ = *src++;
	}
	*ptr = '\0';

	return dest;
}

HCHAR *VK_strncpy(HCHAR *dest, const HCHAR *src, HUINT32 cnt)
{
	HCHAR *ptr = dest;

	if( NULL == dest || NULL == src )
		return dest;

	while (*src && (cnt > 0))
	{
		*ptr++ = *src++;
		cnt--;
	}
	if (cnt > 0)
		*ptr = '\0';

	return dest;
}

HCHAR*		VK_strcasestr(const HCHAR *s1, const HCHAR *s2)
{
	HINT32	wanted;
	HUINT32	len;

	if( NULL == s1 || NULL == s2 )
		return NULL;

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

HINT32		VK_strncasecmp (const HCHAR *s1, const HCHAR *s2, HUINT32 n)
{
	HINT32	ch1;
	HINT32	ch2;

	if( NULL == s1 && NULL == s2 )
		return 0;
	else if ( NULL == s1 )
		return -1;
	else if ( NULL == s2 )
		return 1;

	while (n)
	{
		ch1	= VK_toupper(*s1++);
		ch2	= VK_toupper(*s2++);

		if (ch1 != ch2)
			return ch1 - ch2;

		if (ch1 == 0)
			break;

		n--;
	}
	return 0;
}

HINT32 VK_strncmp(const HCHAR *s1, const HCHAR *s2, HUINT32 n)
{
	HINT32	ch1;
	HINT32	ch2;

	if( NULL == s1 && NULL == s2 )
		return 0;
	else if ( NULL == s1 )
		return -1;
	else if ( NULL == s2 )
		return 1;

	while (n)
	{
		ch1	= *s1++;
		ch2	= *s2++;

		if (ch1 != ch2)
			return ch1 - ch2;

		if (ch1 == 0)
			break;

		n--;
	}
	return 0;
}

HCHAR 	VK_toupper(HCHAR c)
{
	if ((c >= 'a') && (c <= 'z'))
		c -= 32;
	return c;
}

HCHAR 	VK_tolower(HCHAR c)
{
	if ((c >= 'A') && (c <= 'Z'))
		c += 32;
	return c;
}

HCHAR	*VK_strlower (HCHAR *dest, const HCHAR *src)
{
	HCHAR 	*p = dest;
	HCHAR 	*s = (HCHAR *)src;

	if( NULL == dest || NULL == src )
		return NULL;

	while (*s)
	{
		*p++ = VK_tolower(*s++);
	}

	return dest;
}


HCHAR	*VK_strupper (HCHAR *dest, const HCHAR *src)
{
 	HCHAR 	*p = dest;
	HCHAR 	*s = (HCHAR *)src;

	if( NULL == dest || NULL == src )
		return NULL;

	while (*s)
	{
		*p++ = VK_toupper(*s++);
	}

	return dest;
}


HINT32		VK_strcmp (const HCHAR *s1, const HCHAR *s2)
{
	HINT32	ch1;
	HINT32	ch2;

	if( NULL == s1 && NULL == s2 )
		return 0;
	else if ( NULL == s1 )
		return -1;
	else if ( NULL == s2 )
		return 1;

	do
	{
		ch1	= *s1++;
		ch2	= *s2++;
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}


HINT32		VK_strcasecmp (const HCHAR *s1, const HCHAR *s2)
{
	HINT32	ch1;
	HINT32	ch2;

	if( NULL == s1 && NULL == s2 )
		return 0;
	else if ( NULL == s1 )
		return -1;
	else if ( NULL == s2 )
		return 1;

	do
	{
		ch1	= VK_toupper(*s1++);
		ch2	= VK_toupper(*s2++);
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}



HCHAR *VK_strchr(const HCHAR *dest, HINT32 c)
{
	if(NULL == dest)
		return NULL;

	while (*dest) {
		if (*dest == c)
			return (HCHAR *)dest;
		dest++;
	}
	return NULL;
}


HUINT32 VK_strlen(const HCHAR *str)
{
	HUINT32 cnt = 0;

	if(NULL == str)
		return 0;

	while (*str) {
		str++;
		cnt++;
	}

	return cnt;
}

HCHAR *VK_strstr(const HCHAR *dest, const HCHAR *find)
{
	HCHAR c, sc;
	HUINT32 len;
	HCHAR *s = (HCHAR *)dest;

	if( NULL == dest || NULL == find)
		return NULL;

	c = *find++;
	if (c != 0) {
		len = VK_strlen(find);
		do {
			do {
				sc = *s++;
				if (sc == 0)
					return NULL;
			} while (sc != c);
		} while (VK_strncmp(s, find, len) != 0);
		s--;
	}
	return s;
}

HCHAR *VK_strcat(HCHAR *s1, const HCHAR *s2)
{
	HCHAR *p = NULL;

	if( NULL == s1 || NULL == s2 )
		return s1;

	p = s1 + VK_strlen(s1);

	VK_strcpy(p, s2);

	return s1;
}

HCHAR *VK_strncat(HCHAR *s1, const HCHAR *s2, HINT32 cnt)
{
	HINT32 len = 0;
	HCHAR *p = NULL;

	if( NULL == s1 || NULL == s2 )
		return s1;

	len = VK_strlen(s1);
	p = s1 + len;

	cnt -= len;
	if(cnt > 0)
	{
		VK_strncpy(p, s2, cnt);
	}

	return s1;
}


HCHAR *		VK_strreplacechar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to)
{
	HCHAR *ret = dst;

	if( NULL == dst || NULL == src )
		return dst;

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

	if( NULL == dst || NULL == src )
		return dst;

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



HCHAR *	VK_strdup (const HCHAR *szOld)
{
	HUINT32 ulLen;
	HCHAR *	szNew = NULL;

	if( NULL == szOld )
		return NULL;

	if (szOld)
	{
		ulLen = VK_strlen(szOld);

		szNew = VK_MEM_Alloc(ulLen + 1);
		if (szNew)
		{
			VK_MEM_Memcpy(szNew, (void *)szOld, ulLen);
			szNew[ulLen] = '\0';
		}
	}
	return szNew;
}

HCHAR *	VK_strdupex (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32))
{
	HUINT32 ulLen;
	HCHAR *	szNew = NULL;

	if( NULL == szOld )
		return NULL;

	if (szOld)
	{
		ulLen = VK_strlen(szOld);
		if (n < ulLen)
			ulLen = n;

		szNew = _alloc(ulLen + 1);
		if (szNew)
		{
			VK_MEM_Memcpy(szNew, (void *)szOld, ulLen);
			szNew[ulLen] = '\0';
		}
	}
	return szNew;
}

void *	VK_memdupex(const void *data, HUINT32 size, void *(_alloc)(HUINT32))
{
	void *	dup;

	if( NULL == data )
		return NULL;

	dup = _alloc(size);
	if (dup)
	{
		VK_MEM_Memcpy(dup, (void *)data, size);
	}
	return dup;
}

void *	VK_memdup(const void *data, HUINT32 size)
{
	void *	dup;

	if( NULL == data )
		return NULL;

	dup = VK_MEM_Alloc(size);
	if (dup)
	{
		VK_MEM_Memcpy(dup, (void *)data, (unsigned long)size);
	}
	return dup;
}

void *	VK_memset(const void *data, HUINT8 c, HUINT32 size)
{
	HUINT8 *p = (HUINT8 *)data;

	if( NULL == data )
		return NULL;

	while(size--)
	{
		*p++ = c;
	}

	return (void *)data;
}

void *	VK_memcpy(const void *dst, const void *src, HUINT32 size)
{
	HUINT8 *d = (HUINT8 *)dst;
	HUINT8 *s = (HUINT8 *)src;

	if( NULL == dst || NULL == src )
		return (void*)dst;

	while(size--)
	{
		*d++ = *s++;
	}

	return (void *)dst;
}

HINT32	VK_memcmp(const void *dst, const void *src, HUINT32 size)
{
	HUINT8 *d = (HUINT8 *)dst;
	HUINT8 *s = (HUINT8 *)src;
	HUINT32 c = 0;


	if( NULL == dst && NULL == src )
		return 0;
	else if ( NULL == dst )
		return -1;
	else if ( NULL == src )
		return 1;

	while(size--)
	{
		c = *d++ - *s++;
		if(0 != c)
		{
			return c;
		}
	}

	return 0;
}

HUINT32		VK_atoh(const HCHAR *s)
{
	HUINT32		val = 0;
	HINT32		x = 0;

	if( NULL == s )
		return 0;

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

HINT32		VK_atoi (const HCHAR *s)
{
	HINT32	ne = 1;
	HINT32	n;

	if( NULL == s )
		return 0;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return VK_atoh(s);
	}

	if (*s == '+' || *s == '-')
	{
		ne = (*s++ == '+') ? 1 : -1;
	}

	for (n = 0 ; VK_isdigit(*s) ; s++)
	{
		n = n * 10 + (*s - '0');
	}
	return n * ne;
}

HUINT32		VK_atoui (const HCHAR *s)
{
	HUINT32	n;
	HCHAR	*p = (HCHAR *)s;

	if( NULL == s )
		return 0;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return VK_atoh(p);
	}

	for (n = 0 ; VK_isdigit(*p) ; p++)
	{
		n = n * 10 + (*p - '0');
	}
	return n;
}

HBOOL VK_isdigit(const HCHAR ch)
{
	return (HBOOL)(((ch) >= '0') && ((ch) <= '9'));
}

HBOOL VK_isprint(const HCHAR ch)
{
	return ((const HUINT8)((ch)-' ') < 95);
}

HBOOL VK_isxdigit(const HUINT8 ch)
{
	if('0' <= ch && ch <= '9')	return TRUE;
	if('a' <= ch && ch <= 'f')	return TRUE;
	if('A' <= ch && ch <= 'F')	return TRUE;

	return FALSE;
}

HBOOL VK_ishexdigit(const HUINT8 ch)
{
	return VK_isxdigit(ch);
}


HBOOL		VK_strisnumber  (const HCHAR *s)
{
	if( NULL == s )
		return 0;

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




#if defined(CONFIG_PLATFORM_BCM_APP_BSU)
	#include <humax_bsu.h>

	#define DEF_OS_vsprintf		BSU_vsprintf
	#define DEF_OS_vsnprintf	BSU_vsnprintf
	#define DEF_OS_vprintf		BSU_vprintf
	#define DEF_OS_puts			BSU_puts
	#define DEF_OS_putchar		BSU_putchar
	#define DEF_OS_getchar		BSU_getchar
	#define DEF_OS_getch		BSU_getch
	#define DEF_OS_gets			BSU_gets
#else
	#error "Check Platform & OS"
#endif


#if !defined(VK_DBG_Print)
void	VK_DBG_Print(const char *fmt, ...)
{
	unsigned long	ticks;
	char			buf[DEF_PRINTF_MAX_BUF];
	char			*pbuf;
	va_list			v;

	pbuf = buf;

	ticks = VK_TIMER_GetSystemTick();

	VK_sprintf(pbuf, "[%04d:%02d, %5lu, %5lu] ",
			(int)(( ticks / VK_TIMER_GetTickFreq() ) % 10000),
			(int)(( ticks % VK_TIMER_GetTickFreq() ) / 10), (ticks-s_ulRefTick), (ticks - s_ulRefTick2));

	pbuf += 24;	// VK_strlen("[%04d:%02d, %5u, %5u] ")=24

	s_ulRefTick2 = ticks;

    va_start(v, fmt);
	DEF_OS_vsnprintf(pbuf, DEF_PRINTF_MAX_BUF, fmt, v);
    va_end(v);

	DEF_OS_puts(buf);
}
#endif

HINT32 VK_printf(const HCHAR *templat, ...)
{
	va_list		marker;
	HINT32 		ret;

	va_start(marker,templat);
	ret = DEF_OS_vprintf(templat,marker);
	va_end(marker);

	return ret;
}


HINT32 VK_sprintf(HCHAR *buf, const HCHAR *templat, ...)
{
    va_list marker;
    HINT32 		ret;

    va_start(marker,templat);
    ret = DEF_OS_vsprintf(buf, templat,marker);
    va_end(marker);

    return ret;
}

HINT32 VK_snprintf(HCHAR *buf, HINT32 len, const HCHAR *templat, ...)
{
    va_list marker;
    HINT32 		ret;

	(void)len;

    va_start(marker,templat);
    ret = DEF_OS_vsnprintf(buf,len, templat,marker);
    va_end(marker);

    return ret;
}

HINT32 VK_fprintf(void *fp, const HCHAR *templat, ...)
{
    va_list		marker;
    HINT32 		ret = 0;
	HCHAR		buf[DEF_PRINTF_MAX_BUF] = {0, };

	switch((HUINT32)fp)
	{
	case 0: /* VK_STDOUT */
	case 2: /* VK_STDERR */
		va_start(marker,templat);
		ret = DEF_OS_vsnprintf(buf, DEF_PRINTF_MAX_BUF, templat,marker);
		va_end(marker);

		VK_puts(buf);

		break;

	default:
		/*
		 * not implement
		 */
		 break;
	}

	return ret;
}

STATIC HCHAR *_vk_sscanf_GetDigit (const HCHAR *str, HINT32 base)
{
	HCHAR *ptr = (HCHAR*) str;

	if( NULL == str )
		return NULL;

	switch (base)
	{
	case 0:
		while (!(*ptr == 0x00))
		{
			ptr++;
		}
		break;

	case 10:
		while (!(VK_isdigit(*ptr) || *ptr == '-' || *ptr == 0x00))
		{
			ptr++;
		}
		break;

	case 16:
		while (!(VK_isxdigit(*ptr) || *ptr == 0x00))
		{
			ptr++;
		}
		break;

	default:
		break;
	}

	return ptr;
}

STATIC HINT32 _vk_sscanf(HCHAR *str, const HCHAR *templat,  va_list marker)
{
	HINT32 cnt;
	HCHAR *format_ptr = (HCHAR*)templat;
	HCHAR *ptr = str;

	HINT32 	*pInt;
	HCHAR	*pStr;

	cnt = 0;

	while ((*format_ptr != 0x00) && (*ptr != 0x00))
	{
		if (*format_ptr != '%')
		{
			format_ptr++;
			continue;
		}

		format_ptr++;
		if (*format_ptr == 0x00)
		{
			continue;
		}

		switch (*format_ptr)
		{
		case 'd':		case 'D':
		case 'i':		case 'I':
			pInt = va_arg( marker, HINT32 *);
			ptr = _vk_sscanf_GetDigit(ptr, 10);
			if (*ptr)
			{
				*pInt = (HINT32)VK_atoi (ptr);
				cnt ++;
			}
			break;

		case 'x':       // expect an HINT32 in hexadecimal format
		case 'X':
			pInt = va_arg( marker, HINT32 *);
			ptr=_vk_sscanf_GetDigit(ptr, 16);
			if (*ptr)
			{
				*pInt = (HINT32)VK_atoh(ptr);
				cnt ++;
			}
			break;

		case 's':
		case 'S':
			pStr = va_arg( marker, HCHAR *);
			ptr = _vk_sscanf_GetDigit(ptr, 0);
			if (*ptr)
			{
				VK_strcpy(pStr, ptr);
				cnt ++;
			}
			break;

		default:
			break;
		}
	}

	return cnt;
}

HCHAR	*VK_fgets (HCHAR *buf, HINT32 size, void *fp)
{
	HCHAR *ret = NULL;

	HAPPY(size);

	switch((HUINT32)fp)
	{
	case 0:	/* VK_STDIN */
		ret = DEF_OS_gets(buf, DEF_PRINTF_MAX_BUF);
		break;

	default:
		/*
		 * not implement
		 */
		break;
	}

	return ret;
}

HINT32	VK_scanf (const HCHAR *templat, ...)
{
	HCHAR	buf[DEF_PRINTF_MAX_BUF] = {0, };
    HINT32	count;
    va_list marker;

	DEF_OS_gets(buf, DEF_PRINTF_MAX_BUF);

    va_start (marker, templat);
	count = _vk_sscanf(buf, templat, marker);
    va_end (marker);

    return (count);
}

HINT32	VK_sscanf (const HCHAR *buf, const HCHAR *templat, ...)
{
    HINT32	count;
    va_list marker;

    va_start (marker, templat);
	count = _vk_sscanf((HCHAR *)buf, templat, marker);
    va_end (marker);

    return (count);
}

HINT32		VK_fflush (void *fp)
{
	HINT32 len = 0;

	switch((HUINT32)fp)
	{
	case 0:	/* VK_STDIN */
	case 1:	/* VK_STDOUT */
	case 2:	/* VK_STDERR */
		return 0;

	default:
		break;
	}

	/*
	 * not implement
	 */

	return len;
}

HINT32 		VK_putc(HINT32 c, void *fp)
{
	HINT32 ret = 0;

	(void)fp;
	ret = VK_putchar(c);

	return ret;
}

HINT32 VK_puts(const HCHAR *str)
{
	HINT32 ret = 0;
	ret = DEF_OS_puts(str);
	return ret;
}

HINT32 VK_putchar(HINT32 ch)
{
	HINT32 ret = 0;
	ret = DEF_OS_putchar(ch);
	return ret;
}

HINT32 VK_getchar(void)
{
	HINT32 ret = 0;
	ret = DEF_OS_getchar();
	return ret;
}

HINT32 VK_getch(void)
{
	HINT32 ret = 0;
	ret = DEF_OS_getch();
	return ret;
}

HINT32 VK_vsprintf(HCHAR *outbuf,const HCHAR *templat, va_list marker)
{
	return DEF_OS_vsprintf(outbuf, templat, marker);
}

HINT32 VK_vsnprintf(HCHAR *outbuf, HINT32 n, const HCHAR *templat, va_list marker)
{
	return DEF_OS_vsnprintf(outbuf, n, templat, marker);
}

HINT32 	VK_vprintf(const HCHAR *templat, va_list marker)
{
	return DEF_OS_vprintf(templat, marker);
}


HCHAR 		*VK_strtok_r(HCHAR *s, const HCHAR *delim, HCHAR **lasts)
{
	HCHAR *strdata = NULL;
	HCHAR *tok = NULL;
	const HCHAR *spanp  = NULL;

	if ((NULL == s && NULL == *lasts) || (NULL == delim))
	{
		return NULL;
	}

	if(NULL != s)
	{
		if(*s == 0)
		{
			return NULL;
		}
		*lasts = s;
	}

	tok = *lasts;
	for(strdata = tok; '\0' != *strdata; strdata++)
	{
		for(spanp = delim; '\0' != *spanp ; spanp++)
		{
			if(*spanp  == *strdata)
			{
				*lasts = strdata+1;
				*strdata = 0;
				spanp  = NULL;
				break;
			}
		}

		if(NULL == spanp )
		{
			break;
		}
	}

	if(NULL != spanp )
	{
		*lasts = NULL;
	}

	return tok;
}

HCHAR *VK_strtok(HCHAR *s, HCHAR *delim)
{
	STATIC HCHAR *last;

	return VK_strtok_r(s, delim, &last);
}
