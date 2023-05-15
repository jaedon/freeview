/*************************************************************
 * @file		rxstdlib.h
 * @date		2011/05/18
 * @author		Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header

 * http://www.humaxdigital.com
 *************************************************************/

#define	____RxSTD_LIB_README___________________

/******************************************************************************
 [Example Code]

 [Expected Result]
 ************************************************************************************/

#ifndef _RxSTD_LIB_H_
#define _RxSTD_LIB_H_

#include <rxtype.h>

#define	____RxSTD_LIB_TYPE___________________


#define	____RxSTD_LIB_APIs___________________

#define		RxSTD_Alloc			malloc
#define		RxSTD_Free			free
#define		RxSTD_Realloc		realloc

#define		RxSTD_MemSet		memset
#define		RxSTD_MemCopy		memcpy

	// 아래 함수보다 죠~~~ 아래 코딩 컨벤션 맞춘 함수를 사용하길 권장~
	#define	RxSTD_printf		printf
	#define	RxSTD_sprintf		sprintf
	#if defined(WIN32)
		#define	RxSTD_snprintf		_snprintf
		#define	RxSTD_vsnprintf		_vsnprintf
		#define	RxSTD_strtok_r		strtok_s
	#else
		#define	RxSTD_snprintf		snprintf
		#define	RxSTD_vsnprintf		vsnprintf
		#define	RxSTD_strtok_r		strtok_r
	#endif

#define		RxSTD_Print				RxSTD_printf
#define		RxSTD_PrintToStr		RxSTD_sprintf
#define		RxSTD_PrintToStrN		RxSTD_snprintf
#define		RxSTD_PrintToStrV		RxSTD_vsnprintf

#define		RxSTD_StrCpy		strcpy
#define		RxSTD_StrNCpy		strncpy
#define		RxSTD_StrTok		RxSTD_strtok_r	// do not use strtok()!
#define		RxSTD_StrLen		strlen
#define		RxSTD_StrCat		strcat

#define		RxSTD_StrCmp		strcmp
#define		RxSTD_StrNCmp		strncmp
HINT32		RxSTD_StrCaseCmp (const HCHAR *str1, const HCHAR *str2);
HINT32		RxSTD_StrNCaseCmp (const HCHAR *str1, const HCHAR *str2, HUINT32 n);

#define		RxSTD_StrStr		strstr
HCHAR*		RxSTD_StrCaseStr (const HCHAR *s1, const HCHAR *s2);

#define		RxSTD_StrChar		strchr
#define		RxSTD_StrRChar		strrchr

HCHAR *		RxSTD_StrReplaceChar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to);
HCHAR *		RxSTD_StrReplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to);
HBOOL		RxSTD_StrStartWith (const HCHAR *s, const HCHAR *key);
HBOOL		RxSTD_StrEndWith (const HCHAR *s, const HCHAR *key);
HBOOL		RxSTD_StrCaseStartWith (const HCHAR *s, const HCHAR *key);
HBOOL		RxSTD_StrCaseEndWith (const HCHAR *s, const HCHAR *key);

HCHAR *		RxSTD_StrDupEx (const HCHAR *str, HUINT32 n, void *(*_alloc)(HUINT32));
#define 	RxSTD_StrDup(str) RxSTD_StrDupEx(str, RxSTD_StrLen(str), (void *)RxSTD_Alloc)

void *		RxSTD_MemDupEx(const void *data, HUINT32 size, void *(_alloc)(HUINT32));

HINT32		RxSTD_StrToINT32 (const HCHAR *s);
HUINT32		RxSTD_StrToUINT32 (const HCHAR *s);
HUINT32		RxSTD_HexToInt(const HCHAR *s);
HBOOL		RxSTD_IsInteger (const HCHAR *s);

HERROR		RxSTD_GetChar(HUINT8 *data);

HUINT32		RxSTD_GetSystemTick (void);
HUINT32		RxSTD_GetSystemTime(void);

#endif /* _MHSTDLIB_H_ */

