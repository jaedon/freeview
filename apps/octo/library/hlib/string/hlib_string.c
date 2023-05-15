/**************************************************************
*	@file		hxm3u8.c
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxSTR"

#define	LOG				printf//HxLOG_Print
#define	ERR_LOG			HxLOG_Error




/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <hlib.h>
#include <_hlib_string.h>


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

#define HxSTR_FORMAT_BUFFER_SIZE 2048


struct _HxSTR
{
	eHxCP_CodeTable_e	eCT;

	HINT32			nUserTag;

	HINT32			nNumOfBytes;
	HINT32			nNumOfChars;

	HUINT8			*pszASCII;
	HUINT8			*pszUTF8;
	HUINT16			*pszUCS2;

	HUINT8			szCharBuffer[16];
	HUINT8			*pszAllocPtr;

 };



/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
static	void	_HxSTR_ConvertDataFree(HxSTR_t *pSelf)
{

	if(pSelf->pszUCS2 != NULL)
	{
		HxSTR_MemFree(pSelf->pszUCS2);
		pSelf->pszUCS2 = NULL;
	}

	if(pSelf->pszASCII != NULL)
	{
		HxSTR_MemFree(pSelf->pszASCII);
		pSelf->pszASCII = NULL;
	}

}

static	void	_HxSTR_DataFree(HxSTR_t *pSelf)
{
	_HxSTR_ConvertDataFree(pSelf);

	if(pSelf->pszUTF8 != NULL)
	{
		HxLOG_Assert(pSelf->pszAllocPtr);
		HxSTR_MemFree(pSelf->pszAllocPtr);
		pSelf->pszAllocPtr = NULL;
		pSelf->pszUTF8 = NULL;
	}

}

static	void	_HxSTR_HxSTRFree(HxSTR_t *pSelf)
{
	HxSTR_MemFree(pSelf);
	pSelf = NULL;
}


static	void	_HxSTR_Delete(HxSTR_t *pSelf)
{
	_HxSTR_DataFree(pSelf);
	_HxSTR_HxSTRFree(pSelf);
	return;
}


static INLINE	HINT32	_HxSTR_CompareUCS2Char(HUINT16 usChar1, HUINT16 usChar2)
{
	if(usChar1 < usChar2)
	{
		return 1;
	}
	else if(usChar1 > usChar2)
	{
		return -1;
	}

	return 0;
}


static void		_HxSTR_ModifyHxSTR(HxSTR_t *pSelf)
{
	_HxSTR_ConvertDataFree(pSelf);
}



static	HxSTR_t	*_HxSTR_NewUTF8(const HUINT8 *pszUTF8, HINT32 nUTF8StrBytes)
{
	HxSTR_t		*pNew = NULL;

	pNew = (HxSTR_t	*)HxSTR_MemAlloc(sizeof(HxSTR_t));
	HxLOG_Assert(pNew);

	HxSTR_Memset(pNew, 0, sizeof(HxSTR_t));

	pNew->pszUTF8 = (HUINT8 *)HxSTR_MemAlloc((nUTF8StrBytes+sizeof(HUINT8)));
	HxLOG_Assert(pNew->pszUTF8);

	HxSTR_Memcpy(pNew->pszUTF8, pszUTF8, nUTF8StrBytes);
	pNew->pszUTF8[nUTF8StrBytes] = 0;

	pNew->nNumOfBytes	= nUTF8StrBytes;
	pNew->nNumOfChars	= HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, pszUTF8);
	pNew->pszAllocPtr	= pNew->pszUTF8;
	pNew->eCT			= eHxCP_CODE_TABLE_Auto;
	pNew->nUserTag		= 0;

	return pNew;
}

static HxSTR_t	*_HxSTR_Clone(HxSTR_t *pSelf, HINT32 nLeft, HINT32 byte)
{
	return _HxSTR_NewUTF8((pSelf->pszUTF8+nLeft), byte);
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
*
* HxSTR 형태의 String을 만들어 준다. 입력받는 인자는 utf8 format이다.
*
* @param	(HUINT8 *) UTF8 String
* @return	(HxSTR_t *) HxSTR_t를 만들어 준다.
*/
HxSTR_t	*HLIB_STR_New(const HUINT8 *pszUTF8 )
{
	HINT32		nUTF8StrBytes = 0;

	nUTF8StrBytes = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,  pszUTF8);

	if(HxCP_IsErrorCheck(nUTF8StrBytes) == FALSE)
	{
		ERR_LOG("HLIB_CP_NumOfChars error!!\n");
		HxLOG_Assert(0);
		return NULL;
	}

	return _HxSTR_NewUTF8(pszUTF8, nUTF8StrBytes);
}


/**
*
* HxSTR 형태의 String을 만들어 준다. 입력받는 인자는 utf8 format이다.
*
* @param	(HUINT8 *) UTF8 String
* @param	(HUINT8 *) UTF8 Length
* @return	(HxSTR_t *) HxSTR_t를 만들어 준다.
*/
HxSTR_t	*HLIB_STR_NewLength(const HUINT8 *pszUTF8, const HINT32 nUTF8StrBytes)
{
	return _HxSTR_NewUTF8(pszUTF8, nUTF8StrBytes);
}



/**
*
* HxSTR 형태의 String을 만들어 준다. 입력받는 인자는 utf8형태의 printf format이다.
*
* @param	(HUINT8 *) File Name
* @return	(HUINT8 *)  utf8 string, printf 형태
*/
HxSTR_t	*HLIB_STR_NewFormat(const HUINT8 *pszUTF8Format, ...)
{
	HUINT8		*pszUTF8 = NULL;
	HxSTR_t		*pStr = NULL;
	HINT32		nUTF8StrBytes = 0;
	va_list		va;

	pszUTF8 = HLIB_STD_MemCalloc(HxSTR_FORMAT_BUFFER_SIZE);
	HxLOG_Assert(pszUTF8);

 	va_start(va, pszUTF8Format);
	nUTF8StrBytes = vsnprintf(pszUTF8, (HxSTR_FORMAT_BUFFER_SIZE-1), pszUTF8Format, va);
	va_end(va);

	pStr = HLIB_STR_New(pszUTF8);

	HLIB_STD_MemFree(pszUTF8);

	return pStr;
}


/**
*
* file에서 String을 읽어온다. 읽어온 String은 바로 내부에 보관된다.
*
* @param	(HUINT8 *) File Name
* @return	(HxSTR_t *)
*/
HxSTR_t	*HLIB_STR_NewFile(const HUINT8 *pszFileName)
{
	HxLOG_Assert(0);
	// ::TODO 만들어야 함.

	return HLIB_STR_New(pszFileName);
}


/**
*
* HxSTR을 복사하여 새로운 객체를 만든다.
*
* @param	(HxSTR_t *) self
* @return	HxSTR_t	* => New + Copy 역활
*/
HxSTR_t	*HLIB_STR_Clone(HxSTR_t *pSelf)
{
	return _HxSTR_Clone(pSelf, 0, pSelf->nNumOfBytes);
}



/**
*
* HxSTR 을 삭제한다.
*
* @param	(HxSTR_t *) self
* @return	void
*/
void HLIB_STR_Delete(HxSTR_t *pSelf)
{
	if(pSelf)
	{
		_HxSTR_Delete(pSelf);
	}
}


/**
*
* HxSTR에 연결된 String을 만든다.
* 주의!! self가 삭제됨. return을 self로 다시 받을 것!!
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @return	(HxSTR_t *) self + utf8이 된 HxSTR (self가 변화됨 - 그래도 return 받아서 쓸 것! )
*/
HxSTR_t	*HLIB_STR_Append(HxSTR_t *pSelf, const HUINT8 *pszUTF8)
{
	HxSTR_t *pstNew = NULL;
	HUINT8	*tmp = NULL;
	HINT32	utf8Len = 0;

	HxLOG_Assert(pSelf);

	if(pszUTF8 == NULL)
	{
		return pSelf;
	}

	utf8Len = HxSTD_StrLen(pszUTF8);
	tmp = HLIB_MEM_Malloc(utf8Len+pSelf->nNumOfBytes+1);
	HxLOG_Assert(tmp);

	HxSTD_MemCopy(tmp, pSelf->pszUTF8, pSelf->nNumOfBytes);
	HxSTD_MemCopy((tmp+pSelf->nNumOfBytes), pszUTF8, utf8Len);
	tmp[utf8Len+pSelf->nNumOfBytes] = 0;

	pstNew = HLIB_STR_New(tmp);

	HLIB_MEM_Free(tmp);

	HLIB_STR_Delete(pSelf);

	return pstNew;
}


/**
*
* HxSTR에 연결된 String을 만든다.
* 주의!! self가 삭제됨. return을 self로 다시 받을 것!!
*
* @param	(HxSTR_t *) str1
* @param	(HxSTR_t *) str2
* @return	(HxSTR_t *) self + utf8이 된 HxSTR (self가 변화됨 - 그래도 return 받아서 쓸 것! )
*/
HxSTR_t	*HLIB_STR_AppendHxSTR(HxSTR_t *pSelf, const HxSTR_t *pstAppendStr)
{
	return HLIB_STR_Append(pSelf, pstAppendStr->pszUTF8);
}


/**
*
* HxSTR에 연결된 String을 만든다.
* UTF8 String을 넣을 때 printf와 같은 형태로 넣음.
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string, printf 형태
* @return	(HxSTR_t *) self + utf8이 된 HxSTR (self가 변화됨 - 그래도 return 받아서 쓸 것! )
*/
HxSTR_t	*HLIB_STR_AppendFormat(HxSTR_t *pSelf, const HUINT8 *pszUTF8Format, ...)
{
	HUINT8	*pszUTF8 = NULL;
	HxSTR_t	*pStr = NULL;

	va_list va;

	pszUTF8 = HLIB_STD_MemCalloc(HxSTR_FORMAT_BUFFER_SIZE);
	HxLOG_Assert(pszUTF8);

	va_start(va, pszUTF8Format);
	vsnprintf(pszUTF8, (HxSTR_FORMAT_BUFFER_SIZE-1), pszUTF8Format, va);
	va_end(va);

	pStr = HLIB_STR_Append(pSelf, pszUTF8);

	HLIB_STD_MemFree(pszUTF8);

	return pStr;
}



/**
*
* 두 개의 HxSTR 비교
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_Compare(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8)
{
	return HxSTD_StrCmp(pHxSTR1->pszUTF8, pszUTF8);
}


/**
*
* 두 개의 HxSTR 비교 (개수만큼)
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @param	(HINT32) number
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareN(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8, HINT32 number)
{
	return HxSTD_StrNCmp(pHxSTR1->pszUTF8, pszUTF8, number);
}


/**
*
* 두 개의 HxSTR 비교 (대소문자 무시)
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareCase(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8)
{
	return HLIB_STD_StrCaseCmp(pHxSTR1->pszUTF8, pszUTF8);
}


/**
*
* 두 개의 HxSTR 비교 (개수만큼, 대소문자 무시)
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @param	(HINT32) number
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareNCase(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8, HINT32 number)
{
	return HLIB_STD_StrNCaseCmp(pHxSTR1->pszUTF8, pszUTF8, number);
}


/**
*
* 두 개의 HxSTR 비교
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2)
{
	return HLIB_STR_Compare(pHxSTR1, pHxSTR2->pszUTF8);
}


/**
*
* 두 개의 HxSTR 비교 (개수만큼)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @param	(HINT32) number
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareNHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2, HINT32 number)
{
	return HLIB_STR_CompareN(pHxSTR1, pHxSTR2->pszUTF8, number);
}


/**
*
* 두 개의 HxSTR 비교 (대소문자 무시)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @param	(HINT32) number
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareCaseHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2, HINT32 number)
{
	return HLIB_STR_CompareCase(pHxSTR1, pHxSTR2->pszUTF8);
}


/**
*
* 두 개의 HxSTR 비교 (개수만큼, 대소문자 무시)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @param	(HINT32) number
* @return	앞의 인자가 크면 -1, 같으면 0, 뒤에 인자가 크면 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareNCaseHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2, HINT32 number)
{
	return HLIB_STR_CompareNCase(pHxSTR1, pHxSTR2->pszUTF8, number);
}


/**
*
* HxSTR에서 Index에 해당하는 문자 다음에 String을 삽입한다. (Byte단위)
* 주의!! self가 삭제됨. return을 self로 다시 받을 것!!
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *)연결할 UTF8 String
* @param	char index(Byte단위가 아닌, Char순서이다.)
* @return	(HxSTR_t *) self + utf8이 된 HxSTR (self가 변화됨 - 그래도 return 받아서 쓸 것! )
*/
HxSTR_t	*HLIB_STR_Insert(HxSTR_t *pSelf, const HUINT8 *pszUTF8, const HINT32 number)
{
	HxSTR_t *pstNew = NULL;
	HUINT8	*tmp = NULL;
	HINT32	utf8Len = 0;

	HxLOG_Assert(pSelf);

	if(pszUTF8 == NULL)
	{
		return pSelf;
	}

	utf8Len = HxSTD_StrLen(pszUTF8);
	tmp = HLIB_MEM_Malloc(utf8Len+pSelf->nNumOfBytes+1);
	HxLOG_Assert(tmp);

	HxSTD_MemCopy(tmp, pSelf->pszUTF8, number);
	HxSTD_MemCopy((tmp+number), pszUTF8, utf8Len);
	HxSTD_MemCopy((tmp+number+utf8Len), (pSelf->pszUTF8+number), pSelf->nNumOfBytes-number);
	tmp[utf8Len+pSelf->nNumOfBytes] = 0;

	pstNew = HLIB_STR_New(tmp);

	HLIB_MEM_Free(tmp);

	HLIB_STR_Delete(pSelf);

	return pstNew;
}


/**
*
* HxSTR Buffer에서 UTF8에 해당하는 String을 찾아 준다.
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) utf8 string
* @return	해당 Char가 있는 Index => Return 받은 HxSTR_GetAt를 통해 알수 있다. / error : -1
*/
HINT32	HLIB_STR_Find(HxSTR_t *pSelf, const HUINT8 *pszStrUTF8)
{
	HUINT8 *pszFind = HxSTD_StrStr(pSelf->pszUTF8, pszStrUTF8);

	if(pszFind == NULL)
	{
		return -1;
	}

	return (pszFind - pSelf->pszUTF8);
}


/**
*
* HxSTR Buffer에서 UTF8에 해당하는 String을 찾아 준다. (대소문자 무시)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) utf8 string
* @return	해당 Char가 있는 Char Index => Return 받은 HxSTR_GetAt를 통해 알수 있다. / error : -1
*/
HINT32	HLIB_STR_FindCase(HxSTR_t *pSelf, const HUINT8 *pszStrUTF8)
{
	HUINT8 *pszFind =  HLIB_STD_StrCaseStr(pSelf->pszUTF8, pszStrUTF8);

	if(pszFind == NULL)
	{
		return -1;
	}

	return (pszFind - pSelf->pszUTF8);
}


/**
*
* HxSTR Self Buffer에서 HxSTR에 해당하는 String위치를 찾아 준다.
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) find string
* @return	해당 Char가 있는 Char Index => Return 받은 HxSTR_GetAt를 통해 알수 있다. / error : -1
*/
HINT32	HLIB_STR_FindHxSTR(HxSTR_t *pSelf, HxSTR_t *pstHxSTRFind)
{
	return HLIB_STR_Find(pSelf, pstHxSTRFind->pszUTF8);
}



/**
*
* HxSTR Self Buffer에서 HxSTR에 해당하는 String위치를 찾아 준다.
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) find string
* @return	해당 Char가 있는 Char Index => Return 받은 HxSTR_GetAt를 통해 알수 있다. / error : -1
*/
HINT32	HLIB_STR_FindCaseHxSTR(HxSTR_t *pSelf, HxSTR_t *pstHxSTRFind)
{
	return HLIB_STR_FindCase(pSelf, pstHxSTRFind->pszUTF8);
}

/**
*
* Data를 Free하고 Null로 만든다.
*
* @param	(HxSTR_t *) self
* @return	HERROR
*/
HERROR	HLIB_STR_Empty(HxSTR_t *pSelf)
{
	_HxSTR_DataFree(pSelf);
	HxSTR_Memset(pSelf, 0, sizeof(HxSTR_t));

	return ERR_OK;
}


/**
*
* 차지하고 있는 Byte 수
*
* @param	(HxSTR_t *) self
* @return	Byte - HxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32		HLIB_STR_GetLength(HxSTR_t *pSelf)
{
	return HLIB_STR_GetNumOfBytes(pSelf);
}


/**
*
* 차지하고 있는 Byte 수
*
* @param	(HxSTR_t *) self
* @return	Byte - HxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32		HLIB_STR_GetNumOfBytes(HxSTR_t *pSelf)
{
	return pSelf->nNumOfBytes;
}

/**
*
* 실제 문자의 개수
*
* @param	(HxSTR_t *) self
* @return	Byte - HxCP_IsValidReturnValue(return byte) 확인 할 것.
*/
HINT32		HLIB_STR_GetNumOfChars(HxSTR_t *pSelf)
{
	return pSelf->nNumOfChars;
}


/**
*
* HxSTR Self에 들어있는 Buffer pointer를 가져온다.
*
* @param	(HxSTR_t *) self
* @return	Buffer Pointer
*/
HUINT8		*HLIB_STR_GetBuffer(HxSTR_t *pSelf)
{
	return pSelf->pszUTF8;
}


/**
*
* 해당 Index에 해당하는 Buffer의 index를 가져온다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) buffer Index
* @return	UTF8 문자
*/
HUINT8		HLIB_STR_GetAtBuffer(HxSTR_t *pSelf, const HINT32 index)
{
	return pSelf->pszUTF8[index];
}



/**
*
* 해당 Index에 해당하는 buffer에서 index를 지운다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) buffer Index
* @return	UTF8 문자
*/
HUINT8		HLIB_STR_RemoveAtBuffer(HxSTR_t *pSelf, const HINT32 index)
{
	HINT32 	i = 0;
	HCHAR	ch = 0;

	if(pSelf->nNumOfBytes > index)
	{
		pSelf->nNumOfBytes--;
		ch = pSelf->pszUTF8[index];

		for(i=index; i < pSelf->nNumOfBytes; i++)
		{
			pSelf->pszUTF8[i] = pSelf->pszUTF8[i+1];
		}

		pSelf->nNumOfChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);

	}

	return ch;
}


/**
*
* 해당 Index에 해당하는 buffer에서 셋팅한다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Index
* @param	(HUINT16) UTF8 char
* @return	UTF8 문자(기존 문자)
*/
HUINT8		HLIB_STR_SetAtBuffer(HxSTR_t *pSelf, const HINT32 index, const HUINT8 oneByte)
{
	HUINT8 ch = 0;

	ch = pSelf->pszUTF8[index];
	pSelf->pszUTF8[index] = oneByte;

	return ch;
}



/**
*
* 해당 Index에 해당하는 Char를 가져온다. (Char의 index이다. Byte의 Index가 아니다.)
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Char Index
* @return	UCS2 문자
*/
HUINT8		*HLIB_STR_GetAt(HxSTR_t *pSelf, const HINT32 nChIdx)
{
	HINT32 nLen = 0;

	nLen = HLIB_CP_CharAt(eHxCP_UNICODE_UTF8, pSelf->pszUTF8, nChIdx, pSelf->szCharBuffer);
	if(nLen == eHCodepage_MAX || nLen == eHxCP_ERROR_FAIL)
	{
		return "";
	}
	pSelf->szCharBuffer[nLen] = 0;

	return pSelf->szCharBuffer;
}


/**
*
* 해당 Index에 해당하는 Char를 지운다. (Char의 index이다. Byte의 Index가 아니다.)
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Char Index
* @return	UTF8 문자
*/
HUINT8		*HLIB_STR_RemoveAt(HxSTR_t *pSelf, const HINT32 nChIdx)
{

	HxLOG_Assert(0);
	// TODO :: 구현해야함.
	_HxSTR_ModifyHxSTR(pSelf);

	return NULL;
}

/**
*
* 해당 Index에 해당하는 Char를 셋팅한다. (Char의 index이다. Byte의 Index가 아니다.)
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Char Index
* @param	(HUINT16) UTF8 char
* @return	UTF8 문자
*/
HUINT8		*HLIB_STR_SetAt(HxSTR_t *pSelf, const HINT32 nChIdx, const HUINT8 *utf8Char)
{
	HxLOG_Assert(0);
	// TODO :: 구현해야함.

	return NULL;
}


/**
*
* 왼쪽과 오른쪽의 문자를 없애준다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Left/Right Trim Count
* @return	HERROR
*/
HERROR	HLIB_STR_trim(HxSTR_t *pSelf, const HINT32 byte) //Left And Right
{
	HERROR 	hErr 		= ERR_OK;

	if(pSelf->nNumOfBytes > (byte*2))
	{
		pSelf->pszUTF8[pSelf->nNumOfBytes-byte] = 0;
		pSelf->pszUTF8 += byte;

		pSelf->nNumOfBytes = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);
		pSelf->nNumOfChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);

		_HxSTR_ModifyHxSTR(pSelf);
	}

	return hErr;
}


/**
*
* 왼쪽의 문자를 없애준다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Left Trim Count
* @return	HERROR
*/
HERROR	HLIB_STR_trimLeft(HxSTR_t *pSelf, const HINT32 byte)
{
	HERROR hErr = ERR_OK;


	if(pSelf->nNumOfBytes > (byte))
	{
		pSelf->pszUTF8 += byte;

		pSelf->nNumOfBytes = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);
		pSelf->nNumOfChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);

		_HxSTR_ModifyHxSTR(pSelf);
	}


	return hErr;
}


/**
*
* 오른쪽의 문자를 없애준다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Right Trim Count
* @return	HERROR
*/
HERROR	HLIB_STR_trimRight(HxSTR_t *pSelf, const HINT32 byte)
{
	HERROR hErr = ERR_OK;


	if(pSelf->nNumOfBytes > (byte))
	{
		pSelf->pszUTF8[pSelf->nNumOfBytes-byte] = 0;

		pSelf->nNumOfBytes = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);
		pSelf->nNumOfChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, pSelf->pszUTF8);

		_HxSTR_ModifyHxSTR(pSelf);
	}


	return hErr;
}

/**
*
* HxSTR Self에 들어있는 Buffer pointer를 ASCII로 변환하여 가져온다.
*
* @param	(HxSTR_t *) self
* @return	ascii alloc Buffer -> 받은 Pointer는 복사하여 사용할 것!!!!!!!
*/
HCHAR		*HLIB_STR_toASCII(HxSTR_t *pSelf)
{
	HINT32		nAsciiByte = 0;

	if(pSelf->pszASCII != NULL)
	{
		return pSelf->pszASCII;
	}

	if(pSelf->nNumOfBytes == 0)
	{
		return NULL;
	}

	pSelf->pszASCII = (HCHAR *)HxSTR_MemAlloc(sizeof(HCHAR) * (pSelf->nNumOfChars+1));
	HxLOG_Assert(pSelf->pszASCII);

	if(pSelf->pszUCS2 == NULL)
	{
		HLIB_STR_toUCS2(pSelf);
	}

	nAsciiByte = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_ISO_ASCII, pSelf->pszUCS2, pSelf->pszASCII, (pSelf->nNumOfChars+1));
	if(HxCP_IsErrorCheck(nAsciiByte) == FALSE)
	{
		ERR_LOG("HLIB_CP_NumOfChars error!!\n");
		HxLOG_Assert(0);
		return NULL;
	}

	pSelf->pszASCII[nAsciiByte] = 0;

	return pSelf->pszASCII;
}


/**
*
* HxSTR Self에 들어있는 Buffer pointer를 UTF8로 변환하여 가져온다.
*
* @param	(HxSTR_t *) self
* @return	utf8 Buffer
*/
HUINT8		*HLIB_STR_toUTF8(HxSTR_t *pSelf)
{
	return pSelf->pszUTF8;
}


/**
*
* HxSTR Self에 들어있는 Buffer pointer를 UCS2로 변환하여 가져온다.
*
* @param	(HxSTR_t *) self
* @return	ucs2 Buffer -> 받은 Pointer는 복사하여 사용할 것!!!!!!!
*/
HUINT16		*HLIB_STR_toUCS2(HxSTR_t *pSelf)
{
	HINT32		nUsc2Byte = 0;

	if(pSelf->pszUCS2 != NULL)
	{
		return pSelf->pszUCS2;
	}

	if(pSelf->nNumOfBytes == 0)
	{
		return NULL;
	}

	pSelf->pszUCS2 = (HUINT16 *)HxSTR_MemAlloc(sizeof(HUINT16) * (pSelf->nNumOfChars+1));
	HxLOG_Assert(pSelf->pszUCS2);


	nUsc2Byte = HLIB_CP_ConvertCodepageToUCS2(eHxCP_UNICODE_UTF8, pSelf->pszUTF8, pSelf->pszUCS2, (sizeof(HUINT16) * (pSelf->nNumOfChars+1)));
	if(HxCP_IsErrorCheck(nUsc2Byte) == FALSE)
	{
		ERR_LOG("HLIB_CP_NumOfChars error!!\n");
		HxLOG_Assert(0);
		return NULL;
	}

	pSelf->pszUCS2[(nUsc2Byte/2)] = 0;

	return pSelf->pszUCS2;

}

/**
*
* HxSTR Self에 들어있는 Buffer pointer를 10진수로 변환하여 가져온다.
*
* @param	(HxSTR_t *) self
* @return	HINT32
*/
HINT32		HLIB_STR_toDec(HxSTR_t *pSelf)
{
	return HLIB_STD_StrToINT32(pSelf->pszUTF8);
}


/**
*
* HxSTR Self에 들어있는 User tag를 가져온다.
*
* @param	(HxSTR_t *) self
* @return	HINT32
*/
HINT32		HLIB_STR_GetUserTag(HxSTR_t *pSelf)
{
	return pSelf->nUserTag;
}



/**
*
* HxSTR Self에 user tag를 셋팅한다.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) nUserTag
* @return	void
*/
void		HLIB_STR_SetUserTag(HxSTR_t *pSelf, HINT32 nUserTag)
{
	pSelf->nUserTag = nUserTag;
}




#if defined(CONFIG_DEBUG)
void	HLIB_STR_Dump(HxSTR_t *pSelf)
{
	HINT32 i = 0;

	LOG("+ HxSTR(0x%x)\n", (HUINT32)pSelf);
	LOG("  | Ascii : %s\n", HLIB_STR_toASCII(pSelf));
	LOG("  | Utf8  : %s\n", HLIB_STR_toUTF8(pSelf));
	LOG("  | UCS2  : ");

	for(i=0; i < pSelf->nNumOfChars; i++)
	{
		LOG("%04x ", pSelf->pszUCS2[i]);
	}
	LOG("\n");
	LOG("  | NumChar : %d\n", pSelf->nNumOfBytes);
	LOG("  | NumChar : %d\n", pSelf->nNumOfChars);

}

HINT32 HLIB_STR_Test(void *pszArgs)
{
	HxSTR_t *pStr = NULL;

	HLIB_CP_Init();

	LOG("\n\n\n");

	pStr = HLIB_STR_New("Test String Tool");
	HxLOG_Assert(pStr);
	LOG("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	HLIB_STR_trimLeft(pStr, 5);
	LOG("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	pStr = HLIB_STR_Append(pStr, " Test OK");
	LOG("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	HLIB_STR_Delete(pStr);
	pStr = NULL;

	pStr = HLIB_STR_NewFormat("Test Format %s-%d", "string", 10);
	HxLOG_Assert(pStr);
	LOG("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	HLIB_STR_Delete(pStr);
	pStr = NULL;

	pStr = HLIB_STR_New("Test UTF8 to UCS2");
	HxLOG_Assert(pStr);
	HLIB_STR_Dump(pStr);
	HLIB_STR_Delete(pStr);
	pStr = NULL;


	pStr = HLIB_STR_New("");
	HxLOG_Assert(pStr);
	HLIB_STR_Dump(pStr);
	HLIB_STR_Delete(pStr);
	pStr = NULL;

	return 0;
}

#endif



