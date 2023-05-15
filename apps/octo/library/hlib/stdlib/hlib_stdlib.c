/*
 * hxstdlib.c
 *
 *  Created on: 2011. 5. 18.
 */

#include <hlib.h>
#include <_hlib_stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <ctype.h>
#include <time.h>
#if defined(OS_LINUX)
#include <termio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/times.h>
#include <dirent.h>
#else
#include "vkernel.h"
#endif

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxSTDLIB"


#define	____HxSTDLIB_Private_________________________________________________


#define	____HxSTDLIB_APIs____________________________________________________

HUINT32		HLIB_STD_GetSystemTick (void)
{
#if defined(OS_LINUX)
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
#else
	return VK_TIMER_GetSystemTick();
#endif
}

HUINT32		HLIB_STD_GetTID (void)
{
#if defined(OS_LINUX)
	return (HUINT32)syscall(SYS_gettid);
#else
	return (HUINT32)syscall(224);
#endif
}

HUINT32		HLIB_STD_GetPID (void)
{
	return (HUINT32)getpid();
}


HERROR		HLIB_STD_TaskSleep(HUINT32 ulTime)
{
#if	defined(OS_LINUX)
	struct timespec	delay;
	struct timespec	rem;
	HINT32	rc;

	delay.tv_sec = ulTime / 1000;
	delay.tv_nsec = (ulTime % 1000) * 1000000;

	for (;;)
	{

		rc = nanosleep(&delay, &rem);
		if (0 != rc)
		{
			if (EINTR == errno)
			{
				delay = rem;
				continue;
			}

			return ERR_FAIL;
		}
		break;
	}
#else
	VK_TASK_Sleep(ulTime);
#endif
	return ERR_OK;

}

HUINT32		HLIB_STD_GetSystemTime(void)
{
	HUINT32	unixTime = 0;

	unixTime = (HUINT32)time(NULL);

	if(unixTime < 0)
	{
		unixTime = 0;
	}

	return unixTime;
}

HINT32		HLIB_STD_StrCmpV (HINT32 (* compare)(const HCHAR *, const HCHAR *), const HCHAR *s1, ...)
{
	const HCHAR	*s2;
	va_list		ap;
	HINT32		i = 0;

	if (compare == NULL)
		compare = strcmp;

	va_start(ap, s1);
	while ((s2 = va_arg(ap, const HCHAR *)) != NULL)
	{
		if (compare(s1, s2) == 0)
		{
			va_end(ap);
			return i;
		}
		i++;
	}
	va_end(ap);

	return -1;
}

HINT32		HLIB_STD_StrCaseCmp (const HCHAR *s1, const HCHAR *s2)
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


HINT32		HLIB_STD_StrNCaseCmp (const HCHAR *s1, const HCHAR *s2, HUINT32 n)
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


HCHAR *		HLIB_STD_StrCaseStr(const HCHAR *haystack, const HCHAR *needle)
{
	const HCHAR *p;
	const HCHAR *startn = NULL;
	const HCHAR *np = NULL;

	for (p = haystack ; *p ; p++)
	{
		if (np)
		{
			if (toupper(*p) == toupper(*np))
			{
				if (!*++np)
					return (HCHAR *)startn;
			}
			else
			{
				np = NULL;
			}
		}
		else if (toupper(*p) == toupper(*needle))
		{
			np = needle + 1;
			startn = p;
			if( strlen( needle ) == 1 )
			{/* only 1 byte ch not found - a ~ z , 1 ~ 0 */
				return (HCHAR *)startn;
			}
		}
	}
	return NULL;
}
/*
HCHAR*		HLIB_STD_StrCaseStr(const HCHAR *s1, const HCHAR *s2)
{
	HINT32	wanted;
	HUINT32	len;

	len = strlen(s2);
	if (len == 0)
		return (HCHAR *)s1;

	wanted = toupper(*s2);
	while (toupper(*s1) != wanted || HLIB_STD_StrNCaseCmp(s1, s2, len))
	{
		if (*s1++ == '\0')
			return NULL;
	}
	return (HCHAR *)s1;
}
*/
HUINT32		HLIB_STD_StrNLen (const HCHAR *str, HUINT32 n)
{
	HUINT32	c = 0;
	while (*str++ != '\0' && n--)
		c++;
	return c;
}


HCHAR *		HLIB_STD_StrNCpySafe (HCHAR *dst, const HCHAR *src, HUINT32 n)
{
	HCHAR *ret = HxSTD_StrNCpy(dst, src, n);
	if (ret)
	{
		ret[n - 1] = '\0';
	}
	return ret;
}

static const size_t utf8_skip_data[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

HCHAR *		HLIB_STD_StrUtf8NCpy (HCHAR *dst, const HCHAR *src, size_t maxncpy)
{
    HCHAR 	*dst_r 	= dst;
	HUINT8 	*pucDst	= (HUINT8 *)dst;
	HUINT8	*pucSrc	= (HUINT8 *)src;
    size_t utf8_size;

	if (src == NULL)
	{
		return NULL;
	}

	if(maxncpy < 0)
	{
		return NULL;
	}

    while(*pucSrc != '\0' && (utf8_size = utf8_skip_data[(int)*pucSrc]) < maxncpy) {
		maxncpy -= utf8_size;
        switch(utf8_size) {
            case 6: *pucDst ++ = *pucSrc ++;
            case 5: *pucDst ++ = *pucSrc ++;
            case 4: *pucDst ++ = *pucSrc ++;
            case 3: *pucDst ++ = *pucSrc ++;
            case 2: *pucDst ++ = *pucSrc ++;
            case 1: *pucDst ++ = *pucSrc ++;	break;
			default: *pucDst ++ = *pucSrc ++;	break;
        }
    }
    *pucDst	= '\0';
    return dst_r;
}

HCHAR *		HLIB_STD_StrAppend (HCHAR *dst, const HCHAR *src, HUINT32 n)
{
	HUINT32	c;

	c = strlen(dst);
	if (c >= n)
		return dst;

	while (c < n && *src != '\0')
	{
		dst[c++] = *src++;
	}
	if (c == n)
		dst[c-1] = '\0';
	else
		dst[c] = '\0';

	return dst;
}

HUINT32		HLIB_STD_URLStrLen (const HCHAR *src)
{
	static const HUINT32	s_flag[] = {
		0xFFFFFFFF, 0xFC00987D, 0x78000001, 0xF8000001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
	};
	const HUINT8 *s;
	HUINT32	len = 0;

	s = (const HUINT8 *)src;
	while (*s != '\0')
	{
		if (s_flag[*s / 32] & (1 << (*s % 32)))
			len += 3;
		else
			len++;
		s++;
	}
	return len;
}

HCHAR *		HLIB_STD_URLEncode (HCHAR *dst, const HCHAR *src, HUINT32 n)
{
	static const HCHAR *	s_hex = "0123456789ABCDEF";
	static const HUINT32	s_flag[] = {
		0xFFFFFFFF, 0xFC00987D, 0x78000001, 0xF8000001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
	};

	const HUINT8 *s;
	HCHAR        *d;

	s = (const HUINT8 *)src;
	d = dst;

	if (n--/*for '\0' termination*/ == 0)
		return NULL;

	while (*s != '\0')
	{
		if (s_flag[*s / 32] & (1 << (*s % 32)))
		{
			if (n < 3)
				break;
			n -= 3;
			*d++ = '%';
			*d++ = s_hex[(*s) >> 4];
			*d++ = s_hex[(*s) & 0xF];
			s++;
		}
		else
		{
			if (n-- == 0)
				break;
			*d++ = *s++;
		}
	}
	*d = '\0';
	return dst;
}

HCHAR *		HLIB_STD_URLEncodeExceptX (HCHAR *dst, const HCHAR *src, HUINT32 n, const HCHAR exceptX)
{
	static const HCHAR *	s_hex = "0123456789ABCDEF";
	static const HUINT32	s_flag[] = {
		0xFFFFFFFF, 0xFC00987D, 0x78000001, 0xF8000001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
	};

	const HUINT8 *s;
	HCHAR        *d;

	s = (const HUINT8 *)src;
	d = dst;

	if (n--/*for '\0' termination*/ == 0)
		return NULL;

	while (*s != '\0')
	{
		if ( (*s !=exceptX) && (s_flag[*s / 32] & (1 << (*s % 32))))
		{
			if (n < 3)
				break;
			n -= 3;
			*d++ = '%';
			*d++ = s_hex[(*s) >> 4];
			*d++ = s_hex[(*s) & 0xF];
			s++;
		}
		else
		{
			if (n-- == 0)
				break;
			*d++ = *s++;
		}
	}
	*d = '\0';
	return dst;
}

HCHAR *		HLIB_STD_URLEncodeExceptTokens (HCHAR *dst, const HCHAR *src, HUINT32 n, const HCHAR *exceptToken)
{
	static const HCHAR *	s_hex = "0123456789ABCDEF";
	static const HUINT32	s_flag[] = {
		0xFFFFFFFF, 0xFC00987D, 0x78000001, 0xF8000001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
	};

	const HUINT8 *s;
	HCHAR        *d;
	HINT32		  nCnt, nTokenNum = 0;
	HBOOL		  bExcept = FALSE;

	if ((NULL != exceptToken) && ('\0' != exceptToken[0]))
	{
		nTokenNum = HxSTD_StrLen(exceptToken);
	}

	s = (const HUINT8 *)src;
	d = dst;

	if (n--/*for '\0' termination*/ == 0)
		return NULL;

	while (*s != '\0')
	{
		bExcept = FALSE;

		for (nCnt = 0; nCnt < nTokenNum; nCnt++)
		{
			if (*s == exceptToken[nCnt])
			{
				bExcept = TRUE;
				break;
			}
		}


		if ( (TRUE != bExcept) && (s_flag[*s / 32] & (1 << (*s % 32))))
		{
			if (n < 3)
				break;
			n -= 3;
			*d++ = '%';
			*d++ = s_hex[(*s) >> 4];
			*d++ = s_hex[(*s) & 0xF];
			s++;
		}
		else
		{
			if (n-- == 0)
				break;
			*d++ = *s++;
		}
	}
	*d = '\0';
	return dst;
}

HCHAR *		HLIB_STD_URLDecode (HCHAR *dst, const HCHAR *src, HUINT32 n)
{
	static const HUINT8 s_hex[] = {
		/* '0' ~ '9' */0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
		/* :;<=>?@   */0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		/* 'A' ~ 'F' */0xA, 0xB, 0xC, 0xD, 0xE, 0xF
	};
	const HUINT8 *s;
	HCHAR        *d;

	s = (const HUINT8 *)src;
	d = dst;

	if (n--/*for '\0' termination*/ == 0)
		return NULL;

	while (*s != '\0' /*&& isascii(*s)*/ && n--)
	{
		if (s[0] == '%' && isxdigit(s[1]) && isxdigit(s[2]))
		{
			*d++ = (HCHAR)((s_hex[toupper(s[1]) - '0'] << 4) | s_hex[toupper(s[2]) - '0']);
			s += 3;
		}
		else *d++ = *s++;
	}
	*d = '\0';
	return dst;
}


HUINT32		HLIB_STD_MakeStringFlag (const HCHAR *str, const HCHAR **chars, HUINT32 n)
{
	HUINT32	flag;
	size_t	skip;
	const HUINT8 *s;

	if (str == NULL)
		return 0;

	s    = (const HUINT8 *)str;
	flag = 0;

	if (chars && n)
	{
		HUINT32	i;
		const HUINT8 **chMap = (const HUINT8 **)chars;

		for ( ; *s != '\0' ; s += skip)
		{
			skip = utf8_skip_data[*s];
			for (i = 0 ; i < n ; i++)
			{
				if (chMap[i] == NULL)
					continue;
				if (skip != utf8_skip_data[chMap[i][0]])
					continue;
				if (memcmp(s, chMap[i], skip) == 0)
					flag |= (1 << (i % 32));
			}
		}
	}
	else
	{
		for ( ; *s != '\0' ; s += skip)
		{
			skip = utf8_skip_data[*s];
			if (skip == 1 && isalpha(*s))
				flag |= (1 << (toupper(*s) - 'A'));
		}
	}
	return flag;
}

HINT32		HLIB_STD_StrLine (HCHAR *dst, const HCHAR *src)
{
	HCHAR 	*org = dst;

	while (*src != '\0')
	{
		if(HxSTD_IsReturn(*src))
		{
			break;
		}
		*dst++ = *src++;
	}
	*dst = 0;
	return (dst-org);
}

HINT32		HLIB_STD_StrNLine (HCHAR *dst, const HCHAR *src, HINT32 len)
{
	HCHAR 	*org = dst;

	len--;
	while (*src != '\0' && len > 0)
	{
		if(HxSTD_IsReturn(*src))
		{
			break;
		}
		*dst++ = *src++;
		len--;
	}
	*dst = 0;
	return (dst-org);
}


HINT32		HLIB_STD_StrCountChar (const HCHAR *str, const HCHAR ch)
{
	HINT32 	cnt = 0;

	while (*str != '\0')
	{
		if(*str == ch)
		{
			cnt++;
		}
		str++;
	}
	return cnt;
}


HCHAR *		HLIB_STD_StrReplaceChar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to)
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


HCHAR *		HLIB_STD_StrReplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to)
{
	HCHAR  *ret = dst;
	HUINT32 len = HxSTD_StrLen(from);
	HUINT32	adv = HxSTD_StrLen(to);

	if (len == 0 || adv == 0)
	{
		HUINT32 ulDstLen = (strlen(dst)-1);

		HxSTD_StrNCpy(dst, src, ulDstLen);
		dst[ulDstLen] = '\0';
		return ret;
	}

	while (*src != '\0')
	{
		if (HxSTD_StrNCmp(src, from, len) == 0)
		{
			HxSTD_StrNCpy(dst, to, adv);
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

HCHAR *	HLIB_STD_FmtString (HCHAR *buf, HUINT32 size, const HCHAR *fmt, ...)
{
	HINT32	sd;
	HINT32	ret;
	va_list	ap;

	sd = HLIB_RWSTREAM_OpenEx(buf, size, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
		return NULL;

	va_start(ap, fmt);
	ret = HLIB_RWSTREAM_VPrint(sd, fmt, ap);
	va_end(ap);

	if (ret < 0)
	{
		HLIB_RWSTREAM_Close(sd);
		return NULL;
	}
	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

void	HLIB_STD_FreeFmtString(HCHAR *p)
{
	HLIB_STD_MemFree(p);
}

HCHAR *	HLIB_STD_StrReplaceAll (HCHAR *buf, HUINT32 n, const HCHAR *src, ...)
{
#define	__SR_ITEM	20
	struct strrep_item {
		const HCHAR	*from;
		const HCHAR *to;
		HUINT32		len;
	} item[__SR_ITEM];
	HUINT32		i, count;
	HINT32		sd;
	HINT32		ret;
	va_list		ap;

	if (buf == NULL)
		n = HxSTD_StrLen(src) + 1;

	sd = HLIB_RWSTREAM_OpenEx(buf, n, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (sd == 0)
		return NULL;

	// prepare
	va_start(ap, src);
	for (count = 0 ; count < __SR_ITEM ; count++)
	{
		item[count].from = va_arg(ap, const HCHAR *);
		if (item[count].from == NULL) break;
		item[count].to   = va_arg(ap, const HCHAR *);
		if (item[count].to   == NULL) break;

		item[count].len = HxSTD_StrLen(item[count].from);
	}
	va_end(ap);

	// replace & copy
	while (*src != '\0')
	{
		for (i = 0 ; i < count ; i++)
		{
			if (*src == item[i].from[0] && HxSTD_StrNCmp(src, item[i].from, item[i].len) == 0)
				break;
		}
		if (i < count)
		{
			ret = HLIB_RWSTREAM_Print(sd, "%s", item[i].to);
			src += item[i].len;
		}
		else
		{
			ret = HLIB_RWSTREAM_PutChar(sd, (HINT32)*src++);
		}
		if (ret < 0)
		{
			HLIB_RWSTREAM_Close(sd);
			return NULL;
		}
	}
	return (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

HBOOL		HLIB_STD_StrStartWith (const HCHAR *s, const HCHAR *key)
{
	HUINT32	len;

	len = HxSTD_StrLen(key);
	if (len == 0)
		return FALSE;

	if (HxSTD_StrNCmp(s, key, len) == 0)
		return TRUE;
	return FALSE;
}

HBOOL		HLIB_STD_StrEndWith (const HCHAR *s, const HCHAR *key)
{
	HINT32	len1 = HxSTD_StrLen(s);
	HINT32	len2 = HxSTD_StrLen(key);

	if (len1 < len2)
		return FALSE;

	if (HxSTD_StrNCmp(s + len1 - len2, key, len2) == 0)
		return TRUE;
	return FALSE;
}



HBOOL		HLIB_STD_StrCaseStartWith (const HCHAR *s, const HCHAR *key)
{
	HUINT32	len;

	len = HxSTD_StrLen(key);
	if (len == 0)
		return FALSE;

	if (HLIB_STD_StrNCaseCmp(s, key, len) == 0)
		return TRUE;
	return FALSE;
}

HBOOL		HLIB_STD_StrCaseEndWith (const HCHAR *s, const HCHAR *key)
{
	HINT32	len1 = HxSTD_StrLen(s);
	HINT32	len2 = HxSTD_StrLen(key);

	if (len1 < len2)
		return FALSE;

	if (HLIB_STD_StrNCaseCmp(s + len1 - len2, key, len2) == 0)
		return TRUE;
	return FALSE;
}


HCHAR *		HLIB_STD_StrStrip (HCHAR *s, const HCHAR *delim)
{
	HCHAR	*ret;

	ret = HLIB_STD_StrLStrip(s, delim);
	if (ret)
		ret = HLIB_STD_StrRStrip(ret, delim);
	return ret;
}

HCHAR *		HLIB_STD_StrLStrip (HCHAR *s, const HCHAR *delim)
{
	if (delim == NULL)
		delim = "\n\r\t\v\f ";

	while (*s != '\0' && strchr(delim, *s))
		s++;

	if (HxSTD_StrEmpty(s))
		return NULL;
	return s;
}

HCHAR *		HLIB_STD_StrRStrip (HCHAR *s, const HCHAR *delim)
{
	HINT32	n;

	if (delim == NULL)
		delim = "\n\r\t\v\f ";

	n = strlen(s);
	while (n-- && strchr(delim, s[n]))
		s[n] = '\0';

	if (HxSTD_StrEmpty(s))
		return NULL;
	return s;
}

HCHAR *	HLIB_STD_StrUpper (HCHAR *src)
{
	HCHAR	*ret = src;

	while (*src != '\0')
	{
		if (islower(*src))
			*src = toupper(*src);
		src++;
	}
	return ret;
}

HCHAR *	HLIB_STD_StrLower (HCHAR *src)
{
	HCHAR	*ret = src;

	while (*src != '\0')
	{
		if (isupper(*src))
			*src = tolower(*src);
		src++;
	}
	return ret;
}

HCHAR *	HLIB_STD_StrCaseCpy (HCHAR *dst, const HCHAR *src, HBOOL toLower)
{
	HCHAR	*ret = dst;

	if (toLower)
	{
		while (*src != '\0')
		{
			if (isupper(*src))
				*dst++ = tolower(*src++);
			else
				*dst++ = *src++;
		}
	}
	else
	{
		while (*src != '\0')
		{
			if (islower(*src))
				*dst++ = toupper(*src++);
			else
				*dst++ = *src++;
		}
	}
	*dst = '\0';
	return ret;
}

HCHAR *	HLIB_STD_StrNCaseCpy (HCHAR *dst, const HCHAR *src, HUINT32 n, HBOOL toLower)
{
	HCHAR	*ret = dst;

	if (n == 0)
		return ret;

	if (toLower)
	{
		do
		{
			if ((*dst++ = tolower(*src++)) == '\0')
			{
				while (--n != 0)
					*dst++ = '\0';
				break;
			}
		} while (--n != 0);
	}
	else
	{
		do
		{
			if ((*dst++ = toupper(*src++)) == '\0')
			{
				while (--n != 0)
					*dst++ = '\0';
				break;
			}
		} while (--n != 0);
	}
	return ret;
}


HCHAR *	HLIB_STD_StrDupEx (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32))
{
	HUINT32 ulLen;
	HCHAR *	szNew = NULL;

	if (szOld)
	{
		ulLen = HxSTD_StrLen(szOld);
		if (n < ulLen)
			ulLen = n;

		szNew = _alloc(ulLen + 1);
		if (szNew)
		{
			HxSTD_MemCopy(szNew, szOld, ulLen);
			szNew[ulLen] = '\0';
		}
	}
	return szNew;
}

void *	HLIB_STD_MemDupEx(const void *data, HUINT32 size, void *(*_alloc)(HUINT32))
{
	void *	dup;

	dup = _alloc(size);
	if (dup)
	{
		HxSTD_MemCopy(dup, data, size);
	}
	return dup;
}

void *		HLIB_STD_MemCpySafeEx (const char* func, int line,void *dst, HUINT32 dsize, const void *src, HUINT32 ssize)
{
	if (dst == src)
	{
		HxLOG_Warning("*****************************************\n");
		HxLOG_Warning("[%s:%d] src == dest..\n",func,line);
		HxLOG_Warning("*****************************************\n");
		return dst;
	}
	return memcpy(dst, src, HxMACRO_MIN(dsize, ssize));
}

HINT32		HLIB_STD_StrToINT32 (const HCHAR *s)
{
	HINT32	ne = 1;
	HINT32	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return HLIB_STD_HexToInt(s);
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

HUINT32		HLIB_STD_StrToUINT32 (const HCHAR *s)
{
	HUINT32	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return HLIB_STD_HexToInt(s);
	}

	for (n = 0 ; isdigit(*s) ; s++)
	{
		n = n * 10 + (*s - '0');
	}
	return n;
}

HUINT64		HLIB_STD_StrToUINT64 (const HCHAR *s)
{
	HUINT64	n;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return HLIB_STD_HexToInt64(s);
	}

	for (n = 0 ; isdigit(*s) ; s++)
	{
		n = n * 10ULL + (*s - '0');
	}
	return n;
}


HFLOAT32	HLIB_STD_StrToFloat32 (const HCHAR *s)
{
	return (HFLOAT32)HLIB_STD_StrToFloat64(s);
}


HFLOAT64	HLIB_STD_StrToFloat64 (const HCHAR *s)
{
	HFLOAT64	ne = 1.0;
	HFLOAT64	f = 0.0;
	HFLOAT64	so = 0.1;

	if (*s == '+' || *s == '-')
	{
		ne = (*s++ == '+') ? 1.0 : -1.0;
	}

	for ( ; isdigit(*s) ; s++)
	{
		f = f * 10.0 + (*s - '0');
	}

	if(*s == '.')
	{
		s++;
		for ( ; isdigit(*s) ; s++)
		{
			f = f  + ((*s - '0') * so);
			so *= 0.1;
		}
	}

	return f * ne;
}




HUINT32		HLIB_STD_HexToInt(const HCHAR *s)
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


HUINT64		HLIB_STD_HexToInt64(const HCHAR *s)
{
	HUINT64		val = 0;
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
		  val = (val << 4) + s[x] - 'A' + 10ULL;
		}
		else if(s[x]>='a' && s[x] <='f')
		{
		  val = (val << 4) + s[x] - 'a' + 10ULL;
		}
		else return val;

	   x++;
	}
	return val;
}


HBOOL		HLIB_STD_IsInteger (const HCHAR *s)
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


HERROR HLIB_STD_GetChar(HUINT8 *data)
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

HINT32		HLIB_STD_Map (HINT32 keyIndex, HINT32 value, const HINT32 (* map)[2], HUINT32 n, HINT32 err)
{
	HUINT32	i;
	HINT32	key;

	key = (keyIndex == 0) ? 0 : 1;

	for (i = 0 ; i < n ; i++)
	{
		if (map[i][key] == value)
			return map[i][!key];
	}
	return err;
}

HCHAR	*HLIB_STD_GetFileNameFromUrl (HCHAR *szFullUrl)
{
	HCHAR	*szToken = NULL, *szPrevToken = NULL;

	if ((NULL == szFullUrl) || ('\0' == *szFullUrl))			{ return NULL; }

	szToken = HxSTD_StrStr(szFullUrl, "://");
	if (NULL != szToken)
	{
		szToken += 3;
	}
	else
	{
		szToken = szFullUrl;
	}

	szToken = HxSTD_StrStr(szToken, "/");
	while (szToken)
	{
		szPrevToken = szToken + 1;
		szToken = HxSTD_StrStr(szToken + 1, "/");
	}

	return szPrevToken;
}

#if defined(CONFIG_DEBUG)
void *HLIB_STD_MemCopy_Debug(void *dest, void *src, HUINT32 size, const HCHAR *funcname, HUINT32 line)
{
	if (dest != src)
	{
		return memcpy(dest, src, size);
	}

/*
	The memcpy() function copies n bytes from memory area src to memory area dest.  The memory
       areas must not overlap.  Use memmove(3) if the memory areas do overlap.
*/

	HxSTD_Print("***** Critical Error! The memory areas must not overlap!! check here! (%s, %d) :  dest(%p) src(%p) size(%d) \n",
					funcname, line, dest, src, size);
	return (void *)NULL;
}

void *HLIB_STD_MemMove_Debug(void *dest, void *src, HUINT32 size, const HCHAR *funcname, HUINT32 line)
{
	return memmove(dest, src, size);
}
#endif

#define	____HxSTD_QuickSort________________________________


void	HLIB_STD_QuickSort (void *base, HUINT32 nmemb, HUINT32 size, HINT32 (* compare)(const void *arg1, const void *arg2, void *userdata), void *userdata)
{
#define __SW_CODE(t, lhs, rhs, n)	\
	do {\
		HINT32 i = (n) / sizeof (t);	\
		t *lp = (t *) (lhs);		\
		t *rp = (t *) (rhs);		\
		do {						\
			t temp = *lp;			\
			*lp++ = *rp;			\
			*rp++ = temp;			\
		} while (--i > 0);			\
	} while (0)
#define __SW_FUNC(a,b,n,type)		\
	do {\
		if (type <= 1)				\
			__SW_CODE(HINT32,a,b,n);	\
		else __SW_CODE(HINT8,a,b,n); \
	} while (0)
#define __SW(l,r,type,size) \
	do {\
		if (type == 0) {					\
			HINT32 temp = *(HINT32 *)(l);		\
			*(HINT32 *)(l) = *(HINT32 *)(r);	\
			*(HINT32 *)(r) = temp;			\
		} else __SW_FUNC(l,r,size,type);	\
	} while (0)
#define __MED3(ret,a,b,c,cmp,thunk) \
	do {\
		char *_ret = (cmp(a, b, thunk) < 0) 	\
			? (cmp(b, c, thunk) < 0 ? b : (cmp(a, c, thunk) < 0 ? c : a ))	\
			: (cmp(b, c, thunk) > 0 ? b : (cmp(a, c, thunk) < 0 ? a : c )); \
		ret = _ret;\
	} while (0)

	HINT32	i, j;
	HINT32	swtype;
	HINT32	swcount;
	HINT8	*var[7];

	for (;;)
	{
		if (((HINT8 *)base - (HINT8 *)0) % sizeof(HINT32) || size % sizeof(HINT32) ? 2 : size == sizeof(HINT32))
			swtype = 0;
		else swtype = 1;

		swcount = 0;
		if (nmemb < 7)
		{
			for (var[5] = (HINT8 *)base + size; var[5] < (HINT8 *)base + nmemb * size; var[5] += size)
			{
				for (var[4] = var[5]; var[4] > (HINT8 *)base && compare(var[4] - size, var[4], userdata) > 0; var[4] -= size)
					__SW(var[4], var[4] - size, swtype, size);
			}
			return;
		}
		var[5] = (HINT8 *)base + (nmemb / 2) * size;
		if (nmemb > 7)
		{
			var[4] = base;
			var[6] = (HINT8 *)base + (nmemb - 1) * size;
			if (nmemb > 40)
			{
				i = (nmemb / 8) * size;
				__MED3(var[4], var[4], var[4] + i, var[4] + 2 * i, compare, userdata);
				__MED3(var[5], var[5] - i, var[5], var[5] + i, compare, userdata);
				__MED3(var[6], var[6] - 2 * i, var[6] - i, var[6], compare, userdata);
			}
			__MED3(var[5], var[4], var[5], var[6], compare, userdata);
		}
		__SW(base, var[5], swtype, size);
		var[0] = var[1] = (HINT8 *)base + size;
		var[2] = var[3] = (HINT8 *)base + (nmemb - 1) * size;
		for (;;)
		{
			while (var[1] <= var[2] && (j = compare(var[1], base, userdata)) <= 0)
			{
				if (j == 0)
				{
					swcount = 1;
					__SW(var[0], var[1], swtype, size);
					var[0] += size;
				}
				var[1] += size;
			}
			while (var[1] <= var[2] && (j = compare(var[2], base, userdata)) >= 0)
			{
				if (j == 0)
				{
					swcount = 1;
					__SW(var[2], var[3], swtype, size);
					var[3] -= size;
				}
				var[2] -= size;
			}
			if (var[1] > var[2])
				break;
			__SW(var[1], var[2], swtype, size);
			swcount = 1;
			var[1] += size;
			var[2] -= size;
		}
		if (swcount == 0)
		{
			for (var[5] = (HINT8 *)base + size; var[5] < (HINT8 *)base + nmemb * size; var[5] += size)
			{
				for (var[4] = var[5]; var[4] > (HINT8 *)base && compare(var[4] - size, var[4], userdata) > 0; var[4] -= size)
					__SW(var[4], var[4] - size, swtype, size);
			}
			return;
		}

		var[6] = (HINT8 *)base + nmemb * size;
		j = HxMACRO_MIN(var[0] - (HINT8 *)base, var[1] - var[0]);
		if (j > 0)
			__SW_FUNC(base, var[1]-j, j, swtype);
		j = HxMACRO_MIN(var[3] - var[2], var[6] - var[3] - size);
		if (j > 0)
			__SW_FUNC(var[1], var[6]-j, j, swtype);
		if ((j = var[1] - var[0]) > size)
			HLIB_STD_QuickSort(base, j / size, size, compare, userdata);
		if ((j = var[3] - var[2]) > size)
		{
			base = var[6] - j;
			nmemb = j / size;
			continue;
		}
		break;
	}
}

#define	____HxSTD_PROCESS________________________________

HCHAR *stdlib_get_process_name(pid_t pid, HCHAR *pszName)
{
	FILE *fd;
	int nlen;
	static char szProcessName[128];
	char szProcessNameLoc[128];
	int nReadSize = 0;

	HxSTD_MemSet(szProcessName,0,128);

	snprintf(szProcessNameLoc,128,"/proc/%d/%s",pid,pszName);
	fd = fopen(szProcessNameLoc,"r");
	if (fd)
	{
		nReadSize = fread(szProcessName,127,1,fd);
		if (nReadSize == 0)
		{
			// TODO: 여기에 hlib log 함수 쓰면 안되요.  부팅시 hlib init이 안 되어 있는데, 이 아이를 부르면 안되요.
			;//HxLOG_Print("[stdlib_get_process_name] reading fail !!\n");
		}
		szProcessName[127] = '\0';

		// /proc/[processid]/comm file's name has carriage return value
		nlen = strlen(szProcessName);
		if (nlen > 0)
			nlen--;

		szProcessName[nlen] = '\0';
#if 0
		fflush(NULL);
		printf("\n\n\n[%s:%d]  szProcessNameLoc:[%s] szProcessName:[%s] - len :%d\n",__FUNCTION__,__LINE__,
					szProcessNameLoc,szProcessName,nlen);
		fflush(NULL);
#endif
		fclose(fd);
	}
	return szProcessName;
}

HCHAR *HLIB_STD_GetCurProcessName(void)
{
	static HCHAR  s_name[128];

	if (HxSTD_StrEmpty(s_name))
	{
		HCHAR *name = stdlib_get_process_name(getpid(), "comm");
		HLIB_STD_StrNCpySafe(s_name, name, 128);
	}
	return s_name;
}

HCHAR *HLIB_STD_GetCurLxcName(void)
{
	static HCHAR s_lxcname[128];

	if (HxSTD_StrEmpty(s_lxcname))
	{
		HCHAR *lxcname = stdlib_get_process_name(getpid(), "cpuset");
		HLIB_STD_StrNCpySafe(s_lxcname, lxcname, 128);
	}

	return s_lxcname;
}

HCHAR *HLIB_STD_GetProcessName(pid_t pid)
{
	return stdlib_get_process_name(pid, "comm");
}

HERROR HLIB_STD_GetProcessPidWithLXC(HCHAR *pszProcessName, HCHAR *pszLXCName, HINT32 *pnProgPid)
{
#if defined(OS_WIN32)
	//TODO
#else
	DIR 	*dp;
	struct dirent *dir;
	HINT32	pid;
	HCHAR	*pszName;


	dp = opendir("/proc");
	if(!dp)
		return -1;

	while( (dir = readdir(dp)))
	{
		pid = atoi(dir->d_name);
		if(pid == -1)
			continue;

		pszName = stdlib_get_process_name(pid, "comm");
		if (HxSTD_StrCmp(pszName, pszProcessName) == 0)
		{
			pszName = stdlib_get_process_name(pid, "cpuset");
			if (HxSTD_StrCmp(pszName, pszLXCName) == 0)
			{
				closedir(dp);
				*pnProgPid = pid;

				return ERR_OK;
			}
		}
	}

	closedir(dp);
#endif
	return ERR_FAIL;
}

HINT32 HLIB_STD_GetPID_ByProcessname(const HCHAR* processname)
{
#if defined(OS_LINUX)
	DIR 	*dp;
	struct dirent *dir;
	HINT32 pid;
	HCHAR  buf[100],line[1024],tag[100],name[100];
	FILE *fp;

	dp = opendir("/proc");
	if(!dp)
		return -1;

	while( (dir = readdir(dp)))
	{
		pid = atoi(dir->d_name);
		if(pid == -1)
			continue;

		snprintf(buf,100,"/proc/%d/status",pid);

		fp= fopen(buf,"r");
		if(fp == NULL)
			continue;
		fgets(line,1024,fp);

		fclose(fp);

		sscanf(line,"%s %s",tag,name);
		if(!strcmp(name,processname))
		{
			closedir(dp);
			return pid;
		}

	}
	closedir(dp);
	return -1;
#elif defined(CONFIG_PROD_OS_EMULATOR)
	return (HINT32)MAIN_GetProcessId(processname);
#endif
}





HINT32	HLIB_STD_ListOpenFile (HUINT32 pid
				, const HCHAR *filter
				, void (* callback)(const HCHAR *path, HINT32 fd, void *userdata)
				, void *userdata)
{
#if defined(OS_WIN32)
	// TODO
	return 0;
#else
	HCHAR	buf[HxPATH_MAX];
	HCHAR	path[HxPATH_MAX];
	HCHAR	abspath[HxPATH_MAX];
	HINT32	count = 0;
	HINT32	i, n;
	struct dirent	**namelist;

	if (pid == 0)
		pid = HLIB_STD_GetPID();

	if (filter == NULL)
		filter = "/";

	HxSTD_PrintToStrN(path, HxPATH_MAX, "/proc/%u/fd", pid);
	n = scandir(path, &namelist, NULL, alphasort);
	for (i = 0 ; i < n ; i++)
	{
		if (strcmp(namelist[i]->d_name, ".") == 0 || strcmp(namelist[i]->d_name, "..") == 0)
		{
			free(namelist[i]);
			continue;
		}

		HLIB_PATH_Join(path, namelist[i]->d_name, buf, HxPATH_MAX);
		if (HLIB_PATH_AbsPath(buf, abspath, HxPATH_MAX) != NULL && HLIB_STD_StrStartWith(abspath, filter))
		{
			if (callback)
				callback(abspath, HLIB_STD_StrToINT32(namelist[i]->d_name), userdata);
			count++;
		}
		free(namelist[i]);
	}
	if (n > 0)
		free(namelist);
	return count;
#endif
}


HINT32	HLIB_STD_ListOpenFile_ByProcessName(HCHAR* processname
				, const HCHAR *filter
				, void (* callback)(const HCHAR *path, HINT32 fd, void *userdata)
				, void *userdata)
{
	HINT32 pid;
	pid = HLIB_STD_GetPID_ByProcessname(processname);
	return HLIB_STD_ListOpenFile(pid,filter,callback,userdata);
}


#define	____HxSTD_CIRCULAR_BUFFER___________________________
HUINT32	HLIB_STD_WriteCircularBuffer(HUINT8 *pucBuffer, HINT32 bufferSize, HINT32 startOffset, HINT32 length, HUINT8 *srcBuffer)
{
	HINT32	last_offset, wlength;

	wlength = 0;
	last_offset = startOffset + length;
	if (bufferSize < last_offset)
	{
		wlength = bufferSize - startOffset;
		HxSTD_MemCopy((HUINT8*)(pucBuffer + startOffset), srcBuffer, wlength);
		startOffset = 0;
		length = length - wlength;
	}

	HxSTD_MemCopy((HUINT8*)(pucBuffer + startOffset), (HUINT8*)(srcBuffer + wlength), length);

	return startOffset + length;
}

HUINT32	HLIB_STD_ReadCircularBuffer(HUINT8 *pucBuffer, HINT32 bufferSize, HINT32 startOffset, HINT32 length, HUINT8 *dstBuffer)
{
	HUINT8	*p;
	HINT32	last_offset, wlength;

	p = dstBuffer;
	last_offset = startOffset + length;
	wlength = 0;
	if (bufferSize < last_offset)
	{
		wlength = bufferSize - startOffset;
		HxSTD_MemCopy(p, (HUINT8*)(pucBuffer + startOffset), wlength);
		startOffset = 0;
		length = length - wlength;
	}

	HxSTD_MemCopy((HUINT8*)(p + wlength), (HUINT8*)(pucBuffer + startOffset), length);

	return startOffset + length;
}

/* end of file */
