/**************************************************************
*	http://www.humaxdigital.com
*   @title		Contents Categorized Library
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

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Include_________________________________________________
/**************************************************************************************************/
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <hlib.h>
#include <dlib.h>

#include "clib_int.h"

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct
{
	HINT32		nCurrentLogicalNumber;
} CLibInt_ORF_t;

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Value_________________________________________________
/**************************************************************************************************/
#define	CHECK_AND_RETURN(a, b)				if ((a) && !(b)) return FALSE
#define	CLIB_PORT_ORF_LOGICAL_NUMBER_TV			1
#define	CLIB_PORT_ORF_LOGICAL_NUMBER_RADIO		5000

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Prototype_________________________________________________
/**************************************************************************************************/

static HUINT8*	clib_port_orf_GetFirstAlphabetCharacter(HUINT8* pSvcName)
{
	HUINT8	*p = NULL, *pcStr = pSvcName;

	if ((pcStr == NULL) || (*pcStr == 0x00))		{ return NULL; }

	if (((*pcStr >= 0x01) && (*pcStr <= 0x0B)) || ((*pcStr >= 0x11) && (*pcStr <= 0x15)))
	{
		p = pcStr + 1;
		if ((p != NULL) && (*p == 0x86 || *p == 0x87 || *p == 0x8a)) // control code 인 경우 처리함.
		{
			p = p + 1;
		}
	}
	else if (*pcStr == 0x86 || *pcStr == 0x87 || *pcStr == 0x8a)	// control code 인 경우 처리함.
	{
		p = pcStr + 1;
	}
	else if (*pcStr == 0x10)
	{
		/* ETSI EN 300 468 spec */
		if (*(pcStr+1) == 0x00)
		{
			if ((*(pcStr+2) > 0x00) && (*(pcStr+2) <= 0x0f))
			{
				p = pcStr + 3;
			}
		}
		/* 6937 */
		else if (*(pcStr+1) == 0x06 && *(pcStr+2) == 0x09)
		{
			p = pcStr + 3;
		}
	}
	else
	{
		p = pcStr;
	}

	return p;
}

static HINT32	clib_port_orf_MatchSvcNameAnyAlphabet(HUINT8 ucAlphabetIdx, HUINT8 *pSvcName)
{
	HUINT8		*pChar = NULL;

	if (ucAlphabetIdx == eDxAlphabetGroup_AlphabetAll)
	{
		return TRUE;
	}

	pChar = clib_port_orf_GetFirstAlphabetCharacter(pSvcName);
	if ((ucAlphabetIdx >= eDxAlphabetGroup_AlphabetA) && (ucAlphabetIdx <= eDxAlphabetGroup_AlphabetZ))
	{
		if (pChar != NULL)
		{
			if ( (*pChar >= 'A' && *pChar <= 'Z') ||
				(*pChar >= 'a' && *pChar <= 'z') )
			{
				if(*pChar == ucAlphabetIdx || *pChar == (ucAlphabetIdx+32)) // 'A' 와  'a' 는 32 차이
				{
					return TRUE;
				}
			}
		}
	}
	else if (ucAlphabetIdx == eDxAlphabetGroup_AlphabetDigit)
	{
		if (pChar != NULL)
		{
			if ( *pChar >= '0' && *pChar <= '9' )
			{
				return TRUE;
			}
		}
	}
	else
	{
		if (pChar != NULL)
		{
			if ( !(*pChar >= 'A' && *pChar <= 'Z') &&
				!(*pChar >= 'a' && *pChar <= 'z') &&
				!(*pChar >= '0' && *pChar <= '9') )
				return TRUE;
		}
	}

	return FALSE;
}



/**************************************************************************************************/
#define _________CLIB_PORT_Public_Functions__________________________________________________________
/**************************************************************************************************/
HBOOL			CLIB_PORT_FilterInOrf(CListAttr_t *pstAttr, DxService_t *pstSvcInfo)
{
	CHECK_AND_RETURN(pstAttr->ulContentsType & CLIB_TYPE_TV, pstSvcInfo->svcType & eDxSVC_TYPE_TV);
	CHECK_AND_RETURN(pstAttr->ulContentsType & CLIB_TYPE_RADIO, pstSvcInfo->svcType & eDxSVC_TYPE_RADIO);
	CHECK_AND_RETURN(pstAttr->ulContentsType & CLIB_TYPE_HD, pstSvcInfo->highDefinition == eDxVIDEO_TYPE_HD);
	CHECK_AND_RETURN(pstAttr->ulContentsType & CLIB_TYPE_CAS, (pstSvcInfo->casType & pstAttr->ulExt) || (pstAttr->ulExt == 0));

	if (pstAttr->ulContentsType & CLIB_TYPE_ALPABET)
	{
		return	clib_port_orf_MatchSvcNameAnyAlphabet(pstAttr->ulExt, pstSvcInfo->name);
	}


	return TRUE;
}

HINT32			CLIB_PORT_GetServiceNumberOrf(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx)
{
	HINT32	ulLogicalNumber;
	CLibInt_ORF_t	*pstIntInfo;

	if (pstSvcInfo->lcnFlag == TRUE)
	{
		ulLogicalNumber = pstSvcInfo->lcn;
	} else
	{
		pstIntInfo = (CLibInt_ORF_t*)pstAttr->aucData;
		ulLogicalNumber = pstIntInfo->nCurrentLogicalNumber++;
	}
	return ulLogicalNumber;
}

CListAttr_t*	CLIB_PORT_InitSessionOrf(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt)
{
	CListAttr_t	*pstAttr;
	CLibInt_ORF_t	*pstIntInfo;

	pstAttr = (CListAttr_t*)HLIB_STD_MemCalloc(sizeof(CListAttr_t) + sizeof(CLibInt_ORF_t));
	pstIntInfo = (CLibInt_ORF_t*)pstAttr->aucData;
	if (ulContentsType == CLIB_TYPE_RADIO)
	{
		pstIntInfo->nCurrentLogicalNumber = CLIB_PORT_ORF_LOGICAL_NUMBER_RADIO;
	}else
	{
		pstIntInfo->nCurrentLogicalNumber = CLIB_PORT_ORF_LOGICAL_NUMBER_TV;
	}

	return pstAttr;
}

/**************************************************************************************************/
#define _________CLIB_PORT_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_End_Of_File____________________________________________________
/**************************************************************************************************/


