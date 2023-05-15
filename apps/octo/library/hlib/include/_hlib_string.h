
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#ifndef	__HXSTRING_H__
#define	__HXSTRING_H__

#include <_hlib_codepage.h>
/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

/* memory control */
#define HxSTR_MemAlloc				HLIB_MEM_Malloc
#define HxSTR_MemFree				HLIB_MEM_Free
#define HxSTR_Memset				HxSTD_MemSet
#define HxSTR_Memcpy				HxSTD_MemCopy

#define HxSTR_COMP_ERROR			0x12345678



#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef NULL
#define NULL					0
#endif

#ifndef FALSE
#define FALSE					0
#endif

#ifndef TRUE
#define TRUE					1
#endif

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

typedef struct _HxSTR HxSTR_t;


typedef HINT32	(*HxSTR_CompareUCS2Char)(const HUINT16 usChar1, const HUINT16 usChar2);


/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */

extern HxSTR_t *HLIB_STR_New(const HUINT8 *pszUTF8 );
extern HxSTR_t *HLIB_STR_NewLength(const HUINT8 *pszUTF8, const HINT32 nUTF8StrBytes);
extern HxSTR_t *HLIB_STR_NewFormat(const HUINT8 *pszUTF8Format, ...);
extern HxSTR_t *HLIB_STR_NewFile(const HUINT8 *pszFileName);

extern HxSTR_t *HLIB_STR_Clone(HxSTR_t *pSelf);

extern void HLIB_STR_Delete(HxSTR_t *pSelf);

extern HxSTR_t *HLIB_STR_Append(HxSTR_t *pSelf, const HUINT8 *pszUTF8);
extern HxSTR_t *HLIB_STR_AppendHxSTR(HxSTR_t *pSelf, const HxSTR_t *pstAppendStr);
extern HxSTR_t *HLIB_STR_AppendFormat(HxSTR_t *pSelf, const HUINT8 *pszUTF8Format, ...);

extern HINT32	HLIB_STR_Compare(HxSTR_t *pMxStr1, const HUINT8 *pszUTF8);
extern HINT32	HLIB_STR_CompareN(HxSTR_t *pMxStr1, const HUINT8 *pszUTF8, HINT32 number);
extern HINT32	HLIB_STR_CompareCase(HxSTR_t *pMxStr1, const HUINT8 *pszUTF8);
extern HINT32	HLIB_STR_CompareNCase(HxSTR_t *pMxStr1, const HUINT8 *pszUTF8, HINT32 number);
extern HINT32	HLIB_STR_CompareHxSTR(HxSTR_t *pMxStr1, HxSTR_t *pMxStr2);
extern HINT32	HLIB_STR_CompareNHxSTR(HxSTR_t *pMxStr1, HxSTR_t *pMxStr2, HINT32 number);
extern HINT32	HLIB_STR_CompareCaseHxSTR(HxSTR_t *pMxStr1, HxSTR_t *pMxStr2, HINT32 number);
extern HINT32	HLIB_STR_CompareNCaseHxSTR(HxSTR_t *pMxStr1, HxSTR_t *pMxStr2, HINT32 number);

extern HxSTR_t	*HLIB_STR_Insert(HxSTR_t *pSelf, const HUINT8 *pszUTF8, const HINT32 number);

extern HINT32	HLIB_STR_Find(HxSTR_t *pSelf, const HUINT8 *pszStrUTF8);
extern HINT32	HLIB_STR_FindCase(HxSTR_t *pSelf, const HUINT8 *pszStrUTF8);
extern HINT32	HLIB_STR_FindHxSTR(HxSTR_t *pSelf, HxSTR_t *pstMxStrFind);
extern HINT32	HLIB_STR_FindCaseHxSTR(HxSTR_t *pSelf, HxSTR_t *pstMxStrFind);

extern HERROR	HLIB_STR_Empty(HxSTR_t *pSelf);

extern HINT32	HLIB_STR_GetLength(HxSTR_t *pSelf);
extern HINT32	HLIB_STR_GetNumOfBytes(HxSTR_t *pSelf);
extern HINT32	HLIB_STR_GetNumOfChars(HxSTR_t *pSelf);

extern HUINT8	*HLIB_STR_GetBuffer(HxSTR_t *pSelf);

extern HUINT8	HLIB_STR_GetAtBuffer(HxSTR_t *pSelf, const HINT32 index);
extern HUINT8	HLIB_STR_RemoveAtBuffer(HxSTR_t *pSelf, const HINT32 index);
extern HUINT8	HLIB_STR_SetAtBuffer(HxSTR_t *pSelf, const HINT32 index, const HUINT8 oneByte);

extern HUINT8	*HLIB_STR_GetAt(HxSTR_t *pSelf, const HINT32 nChIdx);
extern HUINT8	*HLIB_STR_RemoveAt(HxSTR_t *pSelf, const HINT32 nChIdx);
extern HUINT8	*HLIB_STR_SetAt(HxSTR_t *pSelf, const HINT32 nChIdx, const HUINT8 *utf8Char);

extern HERROR	HLIB_STR_trim(HxSTR_t *pSelf, const HINT32 byte); //Left And Right
extern HERROR	HLIB_STR_trimLeft(HxSTR_t *pSelf, const HINT32 byte);
extern HERROR	HLIB_STR_trimRight(HxSTR_t *pSelf, const HINT32 byte);

extern HCHAR	*HLIB_STR_toASCII(HxSTR_t *pSelf);
extern HUINT8	*HLIB_STR_toUTF8(HxSTR_t *pSelf);
extern HUINT16 	*HLIB_STR_toUCS2(HxSTR_t *pSelf);

extern HINT32	HLIB_STR_toDec(HxSTR_t *pSelf);

extern HINT32	HLIB_STR_GetUserTag(HxSTR_t *pSelf);
extern void		HLIB_STR_SetUserTag(HxSTR_t *pSelf, HINT32 nUserTag);


/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */


#if defined(CONFIG_DEBUG)
void	HLIB_STR_Dump(HxSTR_t *pSelf);
HINT32 	HLIB_STR_Test(void *pszArgs);
#endif


#endif /* __HXSTRING_H__ */

