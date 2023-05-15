/*
 * rxstdlib.c
 *
 *  Created on: 2011. 5. 18.
 */

#include <rxlib.h>
#include <rxstdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <ctype.h>
#if defined(OS_LINUX)
#include <termio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#endif

#undef	RxLOG_DOMAIN
#define	RxLOG_DOMAIN	"RxSTD_LIB"


#define	____RxSTD_LIB_Private_________________________________________________


#define	____RxSTD_LIB_APIs____________________________________________________

HUINT32		RxSTD_GetSystemTick (void)
{
#if 0//defined(OS_LINUX)
	static int s_bFirst = 0;
	static unsigned long s_ulFirstTick;

	int rc;
	struct timespec humax_time;
	unsigned long	msTick;

	rc = clock_gettime (CLOCK_MONOTONIC, &humax_time);
    if (rc!=0)
	{
		return 0;
    }

	msTick = (humax_time.tv_sec*1000 + humax_time.tv_nsec/1000000);

	if (0 == s_bFirst)
	{
		s_ulFirstTick = msTick;
		s_bFirst = 1;
	}

	return (unsigned long)(msTick - s_ulFirstTick);
#else
	return 0;
#endif
}

HUINT32		RxSTD_GetSystemTime(void)
{
	return time(NULL);
}

HINT32		RxSTD_StrCaseCmp (const HCHAR *s1, const HCHAR *s2)
{
	HINT32	ch1;
	HINT32	ch2;

	do
	{
		ch1	= toupper(*s1++);
		ch2	= toupper(*s2++);
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}


HINT32		RxSTD_StrNCaseCmp (const HCHAR *s1, const HCHAR *s2, HUINT32 n)
{
	HINT32	ch1;
	HINT32	ch2;

	while (n)
	{
		ch1	= toupper(*s1++);
		ch2	= toupper(*s2++);

		if (ch1 != ch2)
			return ch1 - ch2;

		if (ch1 == 0)
			break;

		n--;
	}
	return 0;
}

HCHAR*		RxSTD_StrCaseStr(const HCHAR *s1, const HCHAR *s2)
{
	HINT32	wanted;
	HUINT32	len;

	len = strlen(s2);
	if (len == 0)
		return (HCHAR *)s1;

	wanted = toupper(*s2);
	while (toupper(*s1) != wanted || RxSTD_StrNCaseCmp(s1, s2, len))
	{
		if (*s1++ == '\0')
			return NULL;
	}
	return (HCHAR *)s1;
}


HCHAR *		RxSTD_StrReplaceChar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to)
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


HCHAR *		RxSTD_StrReplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to)
{
	HCHAR  *ret = dst;
	HUINT32 len = RxSTD_StrLen(from);
	HUINT32	adv = RxSTD_StrLen(to);

	if (len == 0 || adv == 0)
	{
		RxSTD_StrCpy(dst, src);
		return ret;
	}

	while (*src != '\0')
	{
		if (RxSTD_StrNCmp(src, from, len) == 0)
		{
			RxSTD_StrNCpy(dst, to, adv);
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

HBOOL		RxSTD_StrStartWith (const HCHAR *s, const HCHAR *key)
{
	HUINT32	len;

	len = RxSTD_StrLen(key);
	if (len == 0)
		return FALSE;

	if (RxSTD_StrNCmp(s, key, len) == 0)
		return TRUE;
	return FALSE;
}

HBOOL		RxSTD_StrEndWith (const HCHAR *s, const HCHAR *key)
{
	HINT32	len1 = RxSTD_StrLen(s);
	HINT32	len2 = RxSTD_StrLen(key);

	if (len1 < len2)
		return FALSE;

	if (RxSTD_StrNCmp(s + len1 - len2, key, len2) == 0)
		return TRUE;
	return FALSE;
}


HBOOL		RxSTD_StrCaseStartWith (const HCHAR *s, const HCHAR *key)
{
	HUINT32	len;

	len = RxSTD_StrLen(key);
	if (len == 0)
		return FALSE;

	if (RxSTD_StrNCaseCmp(s, key, len) == 0)
		return TRUE;
	return FALSE;
}

HBOOL		RxSTD_StrCaseEndWith (const HCHAR *s, const HCHAR *key)
{
	HINT32	len1 = RxSTD_StrLen(s);
	HINT32	len2 = RxSTD_StrLen(key);

	if (len1 < len2)
		return FALSE;

	if (RxSTD_StrNCaseCmp(s + len1 - len2, key, len2) == 0)
		return TRUE;
	return FALSE;
}

HCHAR *	RxSTD_StrDupEx (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32))
{
	HUINT32 ulLen;
	HCHAR *	szNew = NULL;

	if (szOld)
	{
		ulLen = RxSTD_StrLen(szOld);
		if (n < ulLen)
			ulLen = n;

		szNew = _alloc(ulLen + 1);
		if (szNew)
		{
			RxSTD_MemCopy(szNew, szOld, ulLen);
			szNew[ulLen] = '\0';
		}
	}
	return szNew;
}

void *	RxSTD_MemDupEx(const void *data, HUINT32 size, void *(_alloc)(HUINT32))
{
	void *	dup;

	dup = _alloc(size);
	if (dup)
	{
		RxSTD_MemCopy(dup, data, size);
	}
	return dup;
}


HINT32		RxSTD_StrToINT32 (const HCHAR *s)
{
	HINT32	ne = 1;
	HINT32	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return RxSTD_HexToInt(s);
	}

	if (*s == '+' || *s == '-')
	{
		ne = (*s++ == '+') ? 1 : -1;
	}

	for (n = 0 ; isdigit(*s) ; s++)
	{
		n = n * 10 + (*s - '0');
	}
	return n * ne;
}

HUINT32		RxSTD_StrToUINT32 (const HCHAR *s)
{
	HUINT32	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return RxSTD_HexToInt(s);
	}

	for (n = 0 ; isdigit(*s) ; s++)
	{
		n = n * 10 + (*s - '0');
	}
	return n;
}


HUINT32		RxSTD_HexToInt(const HCHAR *s)
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

HBOOL		RxSTD_IsInteger (const HCHAR *s)
{
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		s += 2;
		while (*s != '\0')
		{
			if (!isxdigit(*s))
				return FALSE;
			s++;
		}
		return TRUE;
	}

	if (*s == '+' || *s == '-')
		s++;

	while (*s != '\0')
	{
		if (!isdigit(*s))
			return FALSE;
		s++;
	}
	return TRUE;
}


HERROR RxSTD_GetChar(HUINT8 *data)
{
	int ch;
#if defined(OS_LINUX)
	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);
#endif
	ch = getchar();

#if defined(OS_LINUX)
	tcsetattr(0, TCSAFLUSH, &save);
#endif
	*data = (unsigned char)ch;

	return ERR_OK;
}

/* end of file */
