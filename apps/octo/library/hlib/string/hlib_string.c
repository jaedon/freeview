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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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
* HxSTR ������ String�� ����� �ش�. �Է¹޴� ���ڴ� utf8 format�̴�.
*
* @param	(HUINT8 *) UTF8 String
* @return	(HxSTR_t *) HxSTR_t�� ����� �ش�.
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
* HxSTR ������ String�� ����� �ش�. �Է¹޴� ���ڴ� utf8 format�̴�.
*
* @param	(HUINT8 *) UTF8 String
* @param	(HUINT8 *) UTF8 Length
* @return	(HxSTR_t *) HxSTR_t�� ����� �ش�.
*/
HxSTR_t	*HLIB_STR_NewLength(const HUINT8 *pszUTF8, const HINT32 nUTF8StrBytes)
{
	return _HxSTR_NewUTF8(pszUTF8, nUTF8StrBytes);
}



/**
*
* HxSTR ������ String�� ����� �ش�. �Է¹޴� ���ڴ� utf8������ printf format�̴�.
*
* @param	(HUINT8 *) File Name
* @return	(HUINT8 *)  utf8 string, printf ����
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
* file���� String�� �о�´�. �о�� String�� �ٷ� ���ο� �����ȴ�.
*
* @param	(HUINT8 *) File Name
* @return	(HxSTR_t *)
*/
HxSTR_t	*HLIB_STR_NewFile(const HUINT8 *pszFileName)
{
	HxLOG_Assert(0);
	// ::TODO ������ ��.

	return HLIB_STR_New(pszFileName);
}


/**
*
* HxSTR�� �����Ͽ� ���ο� ��ü�� �����.
*
* @param	(HxSTR_t *) self
* @return	HxSTR_t	* => New + Copy ��Ȱ
*/
HxSTR_t	*HLIB_STR_Clone(HxSTR_t *pSelf)
{
	return _HxSTR_Clone(pSelf, 0, pSelf->nNumOfBytes);
}



/**
*
* HxSTR �� �����Ѵ�.
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
* HxSTR�� ����� String�� �����.
* ����!! self�� ������. return�� self�� �ٽ� ���� ��!!
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @return	(HxSTR_t *) self + utf8�� �� HxSTR (self�� ��ȭ�� - �׷��� return �޾Ƽ� �� ��! )
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
* HxSTR�� ����� String�� �����.
* ����!! self�� ������. return�� self�� �ٽ� ���� ��!!
*
* @param	(HxSTR_t *) str1
* @param	(HxSTR_t *) str2
* @return	(HxSTR_t *) self + utf8�� �� HxSTR (self�� ��ȭ�� - �׷��� return �޾Ƽ� �� ��! )
*/
HxSTR_t	*HLIB_STR_AppendHxSTR(HxSTR_t *pSelf, const HxSTR_t *pstAppendStr)
{
	return HLIB_STR_Append(pSelf, pstAppendStr->pszUTF8);
}


/**
*
* HxSTR�� ����� String�� �����.
* UTF8 String�� ���� �� printf�� ���� ���·� ����.
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string, printf ����
* @return	(HxSTR_t *) self + utf8�� �� HxSTR (self�� ��ȭ�� - �׷��� return �޾Ƽ� �� ��! )
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
* �� ���� HxSTR ��
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_Compare(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8)
{
	return HxSTD_StrCmp(pHxSTR1->pszUTF8, pszUTF8);
}


/**
*
* �� ���� HxSTR �� (������ŭ)
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @param	(HINT32) number
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareN(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8, HINT32 number)
{
	return HxSTD_StrNCmp(pHxSTR1->pszUTF8, pszUTF8, number);
}


/**
*
* �� ���� HxSTR �� (��ҹ��� ����)
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareCase(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8)
{
	return HLIB_STD_StrCaseCmp(pHxSTR1->pszUTF8, pszUTF8);
}


/**
*
* �� ���� HxSTR �� (������ŭ, ��ҹ��� ����)
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *) utf8 string
* @param	(HINT32) number
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareNCase(HxSTR_t *pHxSTR1, const HUINT8 *pszUTF8, HINT32 number)
{
	return HLIB_STD_StrNCaseCmp(pHxSTR1->pszUTF8, pszUTF8, number);
}


/**
*
* �� ���� HxSTR ��
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2)
{
	return HLIB_STR_Compare(pHxSTR1, pHxSTR2->pszUTF8);
}


/**
*
* �� ���� HxSTR �� (������ŭ)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @param	(HINT32) number
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareNHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2, HINT32 number)
{
	return HLIB_STR_CompareN(pHxSTR1, pHxSTR2->pszUTF8, number);
}


/**
*
* �� ���� HxSTR �� (��ҹ��� ����)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @param	(HINT32) number
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareCaseHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2, HINT32 number)
{
	return HLIB_STR_CompareCase(pHxSTR1, pHxSTR2->pszUTF8);
}


/**
*
* �� ���� HxSTR �� (������ŭ, ��ҹ��� ����)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) dest
* @param	(HINT32) number
* @return	���� ���ڰ� ũ�� -1, ������ 0, �ڿ� ���ڰ� ũ�� 1 / error : HxSTR_COMP_ERROR
*/
HINT32		HLIB_STR_CompareNCaseHxSTR(HxSTR_t *pHxSTR1, HxSTR_t *pHxSTR2, HINT32 number)
{
	return HLIB_STR_CompareNCase(pHxSTR1, pHxSTR2->pszUTF8, number);
}


/**
*
* HxSTR���� Index�� �ش��ϴ� ���� ������ String�� �����Ѵ�. (Byte����)
* ����!! self�� ������. return�� self�� �ٽ� ���� ��!!
*
* @param	(HxSTR_t *) self
* @param	(HUINT8 *)������ UTF8 String
* @param	char index(Byte������ �ƴ�, Char�����̴�.)
* @return	(HxSTR_t *) self + utf8�� �� HxSTR (self�� ��ȭ�� - �׷��� return �޾Ƽ� �� ��! )
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
* HxSTR Buffer���� UTF8�� �ش��ϴ� String�� ã�� �ش�.
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) utf8 string
* @return	�ش� Char�� �ִ� Index => Return ���� HxSTR_GetAt�� ���� �˼� �ִ�. / error : -1
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
* HxSTR Buffer���� UTF8�� �ش��ϴ� String�� ã�� �ش�. (��ҹ��� ����)
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) utf8 string
* @return	�ش� Char�� �ִ� Char Index => Return ���� HxSTR_GetAt�� ���� �˼� �ִ�. / error : -1
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
* HxSTR Self Buffer���� HxSTR�� �ش��ϴ� String��ġ�� ã�� �ش�.
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) find string
* @return	�ش� Char�� �ִ� Char Index => Return ���� HxSTR_GetAt�� ���� �˼� �ִ�. / error : -1
*/
HINT32	HLIB_STR_FindHxSTR(HxSTR_t *pSelf, HxSTR_t *pstHxSTRFind)
{
	return HLIB_STR_Find(pSelf, pstHxSTRFind->pszUTF8);
}



/**
*
* HxSTR Self Buffer���� HxSTR�� �ش��ϴ� String��ġ�� ã�� �ش�.
*
* @param	(HxSTR_t *) self
* @param	(HxSTR_t *) find string
* @return	�ش� Char�� �ִ� Char Index => Return ���� HxSTR_GetAt�� ���� �˼� �ִ�. / error : -1
*/
HINT32	HLIB_STR_FindCaseHxSTR(HxSTR_t *pSelf, HxSTR_t *pstHxSTRFind)
{
	return HLIB_STR_FindCase(pSelf, pstHxSTRFind->pszUTF8);
}

/**
*
* Data�� Free�ϰ� Null�� �����.
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
* �����ϰ� �ִ� Byte ��
*
* @param	(HxSTR_t *) self
* @return	Byte - HxCP_IsValidReturnValue(return byte) Ȯ�� �� ��.
*/
HINT32		HLIB_STR_GetLength(HxSTR_t *pSelf)
{
	return HLIB_STR_GetNumOfBytes(pSelf);
}


/**
*
* �����ϰ� �ִ� Byte ��
*
* @param	(HxSTR_t *) self
* @return	Byte - HxCP_IsValidReturnValue(return byte) Ȯ�� �� ��.
*/
HINT32		HLIB_STR_GetNumOfBytes(HxSTR_t *pSelf)
{
	return pSelf->nNumOfBytes;
}

/**
*
* ���� ������ ����
*
* @param	(HxSTR_t *) self
* @return	Byte - HxCP_IsValidReturnValue(return byte) Ȯ�� �� ��.
*/
HINT32		HLIB_STR_GetNumOfChars(HxSTR_t *pSelf)
{
	return pSelf->nNumOfChars;
}


/**
*
* HxSTR Self�� ����ִ� Buffer pointer�� �����´�.
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
* �ش� Index�� �ش��ϴ� Buffer�� index�� �����´�.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) buffer Index
* @return	UTF8 ����
*/
HUINT8		HLIB_STR_GetAtBuffer(HxSTR_t *pSelf, const HINT32 index)
{
	return pSelf->pszUTF8[index];
}



/**
*
* �ش� Index�� �ش��ϴ� buffer���� index�� �����.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) buffer Index
* @return	UTF8 ����
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
* �ش� Index�� �ش��ϴ� buffer���� �����Ѵ�.
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Index
* @param	(HUINT16) UTF8 char
* @return	UTF8 ����(���� ����)
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
* �ش� Index�� �ش��ϴ� Char�� �����´�. (Char�� index�̴�. Byte�� Index�� �ƴϴ�.)
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Char Index
* @return	UCS2 ����
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
* �ش� Index�� �ش��ϴ� Char�� �����. (Char�� index�̴�. Byte�� Index�� �ƴϴ�.)
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Char Index
* @return	UTF8 ����
*/
HUINT8		*HLIB_STR_RemoveAt(HxSTR_t *pSelf, const HINT32 nChIdx)
{

	HxLOG_Assert(0);
	// TODO :: �����ؾ���.
	_HxSTR_ModifyHxSTR(pSelf);

	return NULL;
}

/**
*
* �ش� Index�� �ش��ϴ� Char�� �����Ѵ�. (Char�� index�̴�. Byte�� Index�� �ƴϴ�.)
*
* @param	(HxSTR_t *) self
* @param	(HINT32) Char Index
* @param	(HUINT16) UTF8 char
* @return	UTF8 ����
*/
HUINT8		*HLIB_STR_SetAt(HxSTR_t *pSelf, const HINT32 nChIdx, const HUINT8 *utf8Char)
{
	HxLOG_Assert(0);
	// TODO :: �����ؾ���.

	return NULL;
}


/**
*
* ���ʰ� �������� ���ڸ� �����ش�.
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
* ������ ���ڸ� �����ش�.
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
* �������� ���ڸ� �����ش�.
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
* HxSTR Self�� ����ִ� Buffer pointer�� ASCII�� ��ȯ�Ͽ� �����´�.
*
* @param	(HxSTR_t *) self
* @return	ascii alloc Buffer -> ���� Pointer�� �����Ͽ� ����� ��!!!!!!!
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
* HxSTR Self�� ����ִ� Buffer pointer�� UTF8�� ��ȯ�Ͽ� �����´�.
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
* HxSTR Self�� ����ִ� Buffer pointer�� UCS2�� ��ȯ�Ͽ� �����´�.
*
* @param	(HxSTR_t *) self
* @return	ucs2 Buffer -> ���� Pointer�� �����Ͽ� ����� ��!!!!!!!
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
* HxSTR Self�� ����ִ� Buffer pointer�� 10������ ��ȯ�Ͽ� �����´�.
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
* HxSTR Self�� ����ִ� User tag�� �����´�.
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
* HxSTR Self�� user tag�� �����Ѵ�.
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



