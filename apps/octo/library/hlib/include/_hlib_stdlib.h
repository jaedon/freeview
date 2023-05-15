/*************************************************************
 * @file		hxstdlib.h
 * @date		2011/05/18
 * @author		Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HxSTDLIB_README___________________

/******************************************************************************
 [Example Code]

 [Expected Result]
 ************************************************************************************/

#ifndef _HxSTDLIB_H_
#define _HxSTDLIB_H_

#include <htype.h>
#include <sys/types.h>

#include "_hlib_types.h"
#include "_hlib_mem.h"
#define	____HxSTDLIB_TYPE___________________



#define	____HxSTDLIB_APIs___________________

#define		HxSTD_MemCmp		memcmp
#define		HxSTD_MemSet		memset

/* va_copy is a C99 feature. In C89 implementations, it's sometimes
   available as __va_copy. If not, memcpy() should do the trick. */
#ifndef va_copy
#ifdef __va_copy
#define va_copy __va_copy
#else
#define va_copy(a, b)  memcpy(&(a), &(b), sizeof(va_list))
#endif
#endif

#if defined(CONFIG_DEBUG)
void *		HLIB_STD_MemCopy_Debug(void *dest, void *src, HUINT32 size, const HCHAR *funcname, HUINT32 line);
#define		HxSTD_MemCopy(dest, src, size)		HLIB_STD_MemCopy_Debug((void *)(dest), (void *)(src), (HUINT32)(size), __FUNCTION__, __LINE__)
void *		HLIB_STD_MemMove_Debug(void *dest, void *src, HUINT32 size, const HCHAR *funcname, HUINT32 line);
#define		HxSTD_MemMove(dest, src, size)		HLIB_STD_MemMove_Debug((void *)(dest), (void *)(src), (HUINT32)(size), __FUNCTION__, __LINE__)
#else
#define		HxSTD_MemCopy		memcpy
#define		HxSTD_MemMove		memmove
#endif

#define		HxSTD_memcmp		HxSTD_MemCmp
#define		HxSTD_memset		HxSTD_MemSet
#define		HxSTD_memcpy		HxSTD_MemCopy

	// 아래 함수보다 죠~~~ 아래 코딩 컨벤션 맞춘 함수를 사용하길 권장~
	#define	HxSTD_printf		printf
	#define	HxSTD_sprintf		sprintf
	#define	HxSTD_snprintf		snprintf	/* int snprintf ( char * s, size_t n, const char * format, ... ); */
	#define	HxSTD_vsnprintf		vsnprintf
	#define	HxSTD_strtok_r		strtok_r

#define		HxSTD_Print				HxSTD_printf	/* int printf(const char*, ...) */
#define		HxSTD_PrintToStr		HxSTD_sprintf	/* int sprintf(char*, const char*, ...) */
#define		HxSTD_PrintToStrN		HxSTD_snprintf	/* int snprintf(char*, int, const char*, ...) */
#define		HxSTD_PrintToStrV		HxSTD_vsnprintf	/* int vsnprintf(char*, int, const char*, ...),  (ext: va_list ap; va_start(ap, fmt); vsnprintf(...) va_end(ap);) */

#define		HxSTD_StrCpy		strcpy		/* size_t strcpy(char*, char*) */
#define		HxSTD_StrNCpy		strncpy		/* size_t strcpy(char*, char*, int) */
#define		HxSTD_StrTok		HxSTD_strtok_r	// do not use strtok()!
#define		HxSTD_StrLen		strlen		/* size_t strlen(char*) */
#define		HxSTD_StrCat		strcat		/* int strcat(char*, char*) */
#define		HxSTD_StrNCat		strncat		/* int strncat(char*, char*, int) */

#define		HxSTD_StrCmp		strcmp		/* int strcmp(char*, char*) */
#define		HxSTD_StrNCmp		strncmp		/* int strncmp(char*, char*, int) */
HINT32		HLIB_STD_StrCaseCmp (const HCHAR *str1, const HCHAR *str2);
HINT32		HLIB_STD_StrNCaseCmp (const HCHAR *str1, const HCHAR *str2, HUINT32 n);
HINT32		HLIB_STD_StrCmpV (HINT32 (* compare)(const HCHAR *, const HCHAR *), const HCHAR *s1, ...);

/**
 * example 1)
 * HCHAR  **chars = {"A", "B", "C", "D", "E"};
 * HUINT32  flag  = HLIB_STD_MakeStringFlag("BBC HD1", chars, 5);
 * -> flag = 0x0000 000E
 *
 * example 2)
 * HCHAR  **chars = {"가", "나", "다", NULL, "B", "K", ..."};
 * HUINT32  flag  = HLIB_STD_MakeStringFlag("나는 가수다 BBK", chars, 50);
 * -> flag = 0x0000 0037
 *
 * 위의 예에서 보듯, chars의 index는 BIT position 이다.
 * 알고리즘: bit_pos = (1 << (index % 32))
 *
 * 만약 chars를 안넣어 주면, 그냥 alphabet 26글자 flag를 만듦
 *
 * @param	str		UTF-8 string
 * @param	chars	map of UTF-8 char
 * @param	n		length of param 'chars'
 * @return	32bit flag
 */
HUINT32		HLIB_STD_MakeStringFlag (const HCHAR *str, const HCHAR **chars, HUINT32 n);


HINT32		HLIB_STD_StrLine (HCHAR *dst, const HCHAR *src);
HINT32		HLIB_STD_StrNLine (HCHAR *dst, const HCHAR *src, HINT32 len);

HINT32		HLIB_STD_StrCountChar (const HCHAR *str, const HCHAR ch);


#define		HxSTD_StrEmpty(s)	((s) == NULL  || (s)[0] == '\0')
#define		HxSTD_StrStr		strstr		/* int strcmp(char*, char*) */
HCHAR*		HLIB_STD_StrCaseStr (const HCHAR *s1, const HCHAR *s2);

#define		HxSTD_StrChar		strchr		/* char* strchr(char *, char) */
#define		HxSTD_StrRChar		strrchr		/* char* strrchr(char *, char) */

HUINT32		HLIB_STD_StrNLen (const HCHAR *str, HUINT32 n);
HCHAR *		HLIB_STD_StrNCpySafe (HCHAR *dst, const HCHAR *src, HUINT32 n);
HCHAR *		HLIB_STD_StrUtf8NCpy (HCHAR *dst, const HCHAR *src, size_t maxncpy);
HCHAR *		HLIB_STD_StrAppend (HCHAR *dst, const HCHAR *src, HUINT32 n);

HUINT32		HLIB_STD_URLStrLen (const HCHAR *src);
HCHAR *		HLIB_STD_URLEncode (HCHAR *dst, const HCHAR *src, HUINT32 n);
HCHAR *		HLIB_STD_URLDecode (HCHAR *dst, const HCHAR *src, HUINT32 n);
HCHAR *		HLIB_STD_URLEncodeExceptX (HCHAR *dst, const HCHAR *src, HUINT32 n, const HCHAR exceptX);
HCHAR *		HLIB_STD_URLEncodeExceptTokens (HCHAR *dst, const HCHAR *src, HUINT32 n, const HCHAR *exceptToken);


HCHAR *		HLIB_STD_FmtString (HCHAR *buf, HUINT32 size, const HCHAR *fmt, ...);
void		HLIB_STD_FreeFmtString(HCHAR *p);

HCHAR *		HLIB_STD_StrReplaceChar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to);
HCHAR *		HLIB_STD_StrReplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to);
HCHAR *		HLIB_STD_StrReplaceAll (HCHAR *buf, HUINT32 n, const HCHAR *src, ...);
HBOOL		HLIB_STD_StrStartWith (const HCHAR *s, const HCHAR *key);
HBOOL		HLIB_STD_StrEndWith (const HCHAR *s, const HCHAR *key);
HBOOL		HLIB_STD_StrCaseStartWith (const HCHAR *s, const HCHAR *key);
HBOOL		HLIB_STD_StrCaseEndWith (const HCHAR *s, const HCHAR *key);
HCHAR *		HLIB_STD_StrStrip (HCHAR *s, const HCHAR *delim);
HCHAR *		HLIB_STD_StrLStrip (HCHAR *s, const HCHAR *delim);
HCHAR *		HLIB_STD_StrRStrip (HCHAR *s, const HCHAR *delim);
HCHAR *		HLIB_STD_StrUpper (HCHAR *src);
HCHAR *		HLIB_STD_StrLower (HCHAR *src);
HCHAR *		HLIB_STD_StrCaseCpy (HCHAR *dst, const HCHAR *src, HBOOL toLower);
HCHAR *		HLIB_STD_StrNCaseCpy (HCHAR *dst, const HCHAR *src, HUINT32 n, HBOOL toLower);

HINT32		HLIB_STD_StrToINT32 (const HCHAR *s);
HUINT32		HLIB_STD_StrToUINT32 (const HCHAR *s);
HUINT64		HLIB_STD_StrToUINT64 (const HCHAR *s);
HFLOAT32	HLIB_STD_StrToFloat32 (const HCHAR *s);
HFLOAT64	HLIB_STD_StrToFloat64 (const HCHAR *s);

HCHAR *		HLIB_STD_StrDupEx (const HCHAR *str, HUINT32 n, void *(*_alloc)(HUINT32));
void *		HLIB_STD_MemDupEx(const void *data, HUINT32 size, void *(*_alloc)(HUINT32));

#define		HLIB_STD_MemCpySafe(dst,dsize,src,ssize)		HLIB_STD_MemCpySafeEx(__FUNCTION__,__LINE__,dst,dsize,src,ssize);
void *		HLIB_STD_MemCpySafeEx (const char* func, int line, void *dst, HUINT32 dsize, const void *src, HUINT32 ssize);
HUINT32		HLIB_STD_HexToInt(const HCHAR *s);
HUINT64		HLIB_STD_HexToInt64(const HCHAR *s);
HBOOL		HLIB_STD_IsInteger (const HCHAR *s);
#define 	HxSTD_IsDigit(c)	isdigit(c)
#define 	HxSTD_IsSpace(c)			((0x09 <= (c) && (c) <= 0x0d) || (c) == ' ')
#define 	HxSTD_IsWhiteSpace(c)		(c == 0x20 || c == '\t' || c == '\n')
#define 	HxSTD_IsReturn(c)			('\r' == (c) || '\n' == (c))


HERROR		HLIB_STD_GetChar(HUINT8 *data);
HINT32		HLIB_STD_Map (HINT32 keyIndex, HINT32 value, const HINT32 (* map)[2], HUINT32 n, HINT32 err);
HCHAR *		HLIB_STD_GetFileNameFromUrl (HCHAR *szFullUrl);

HUINT32		HLIB_STD_GetTID (void);
HUINT32		HLIB_STD_GetPID (void);
HUINT32		HLIB_STD_GetSystemTick (void);
HUINT32		HLIB_STD_GetSystemTime(void);
HERROR		HLIB_STD_TaskSleep(HUINT32 ulTime);

void		HLIB_STD_QuickSort (void *base, HUINT32 nmemb, HUINT32 size, HINT32 (* compare)(const void *arg1, const void *arg2, void *userdata), void *userdata);

HCHAR		*HLIB_STD_GetCurProcessName(void);
HCHAR		*HLIB_STD_GetProcessName(pid_t pid);
HCHAR 		*HLIB_STD_GetCurLxcName(void);
HERROR 		HLIB_STD_GetProcessPidWithLXC(HCHAR *pszProcessName, HCHAR *pszLXCName, HINT32 *pnProgPid);
HINT32 		HLIB_STD_GetPID_ByProcessname(const HCHAR* processname);
/**
 * LSOF clone (list open file)
 *
 * @param	pid	(if 0, current process)
 * @param	filter (path filter, ex. "/mnt/hd2")
 * @param	callback
 * @param	userdata
 */
HINT32		HLIB_STD_ListOpenFile (HUINT32 pid
				, const HCHAR *filter
				, void (* callback)(const HCHAR *path, HINT32 fd, void *userdata)
				, void *userdata);

HINT32		HLIB_STD_ListOpenFile_ByProcessName(HCHAR* processname
				, const HCHAR *filter
				, void (* callback)(const HCHAR *path, HINT32 fd, void *userdata)
				, void *userdata);

HUINT32	HLIB_STD_WriteCircularBuffer(HUINT8 *pucBuffer, HINT32 bufferSize, HINT32 startOffset, HINT32 length, HUINT8 *srcBuffer);
HUINT32	HLIB_STD_ReadCircularBuffer(HUINT8 *pucBuffer, HINT32 bufferSize, HINT32 startOffset, HINT32 length, HUINT8 *dstBuffer);

#endif /* _HxSTDLIB_H_ */

