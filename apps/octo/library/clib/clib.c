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

/**************************************************************************************************/
#define _________CLIB_Private_Include_________________________________________________
/**************************************************************************************************/
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <hlib.h>
#include <dlib.h>

#include "clib_int.h"

/**************************************************************************************************/
#define _________CLIB_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_Private_Struct_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_Private_Prototype_________________________________________________
/**************************************************************************************************/
static HINT32	clib_compareLogicalNumber(const void *arg1, const void *arg2)
{
	const CListItem_t *lhs = *(const CListItem_t **)arg1;
	const CListItem_t *rhs = *(const CListItem_t **)arg2;

	return (lhs->nLogicalNo - rhs->nLogicalNo);
}

static HINT32	clib_compareUid(const void *arg1, const void *arg2)
{
	const CListItem_t *lhs = *(const CListItem_t **)arg1;
	const CListItem_t *rhs = *(const CListItem_t **)arg2;

	return (lhs->uid - rhs->uid);
}

/**************************************************************************************************/
#define _________CLIB_Public_Functions__________________________________________________________
/**************************************************************************************************/
HxVector_t*	CLIB_GenerateList(HxVector_t *pstSvcList, HxVector_t *pstOrgList, HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt, HBOOL bSort)
{
	HINT32		i, len;
	HUINT32		ulSession;
	CListItem_t		*pstItem;
	DxService_t		*pstSvcInfo;
	HxVector_t		*pstNewList = NULL;
	HxVector_t		*pstRefList = NULL;

	if (pstOrgList)
	{
		pstRefList = HLIB_VECTOR_NewEasy(HLIB_VECTOR_Length(pstOrgList), NULL, NULL);
		for (i = 0; i < HLIB_VECTOR_Length(pstOrgList); i++)
			HLIB_VECTOR_Add(pstRefList, HLIB_VECTOR_ItemAt(pstOrgList, i));
		HLIB_VECTOR_Sort(pstRefList, clib_compareUid);
	}

	ulSession = CLIB_PORT_StartSession(pstSvcList, ulContentsType, ulOperatorType, ulExt);

	len = HLIB_VECTOR_Length(pstSvcList);
	pstNewList = HLIB_VECTOR_NewEasy(HLIB_VECTOR_Length(pstSvcList), HLIB_STD_MemFree_CB, NULL);

	if (len > 0)
	{
		for (i = 0; i < len; i++)
		{
			pstSvcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, i);

			if (CLIB_PORT_FilterIn(ulSession, pstSvcInfo) == TRUE)
			{
				HINT32	nLogicalNo;

				if (pstRefList)
				{
					HINT32	index;
					CListItem_t	stKeyItemInfo, *pstRefItemInfo;

					stKeyItemInfo.uid = pstSvcInfo->uid;
					index = HLIB_VECTOR_BSearch(pstRefList, &stKeyItemInfo, clib_compareUid);
					if (index < 0)
						continue;
					pstRefItemInfo = (CListItem_t*)HLIB_VECTOR_ItemAt(pstRefList, index);
					nLogicalNo = pstRefItemInfo->nLogicalNo;
				} else
					nLogicalNo = CLIB_PORT_GetServiceNumber(ulSession, pstSvcInfo, i);

				pstItem = (CListItem_t*)HLIB_STD_MemAlloc(sizeof(CListItem_t));

				pstItem->bLCNControl = pstSvcInfo->lcnFlag;
				pstItem->uid         = pstSvcInfo->uid;
				pstItem->nLogicalNo  = nLogicalNo;

				HxLOG_Debug("In Filter : [%08lld] [%04d] [%s]\n", pstItem->uid, pstItem->nLogicalNo, pstSvcInfo->name);
				HLIB_VECTOR_Add(pstNewList, (void*)pstItem);
			}
		}
	}

	CLIB_PORT_EndSession(ulSession);

	if (bSort)
		HLIB_VECTOR_Sort(pstNewList, clib_compareLogicalNumber);

	return pstNewList;
}

HINT32		CLIB_GetListSize(CList_t *pstList)
{
	HxLOG_Assert(pstList);

	return pstList->ulCount * sizeof(CListItem_t) + sizeof(CList_t);
}

CList_t*	CLIB_Serialize(HxVector_t *pstList, HINT32 *pnSize)
{

	HINT32		i, nSize;
	CList_t		*pstCList;
	CListItem_t	*pstItemInfo;

	HxLOG_Assert(pstList && pnSize);

	nSize = sizeof(CList_t) + HLIB_VECTOR_Length(pstList) * sizeof(CListItem_t);
	pstCList = (CList_t*)HLIB_STD_MemAlloc(nSize);
	pstItemInfo = (CListItem_t*)pstCList->data;
	for (i = 0; i < HLIB_VECTOR_Length(pstList); i++)
		HxSTD_MemCopy(pstItemInfo++, HLIB_VECTOR_ItemAt(pstList, i), sizeof(CListItem_t));

	pstCList->ulMagicCode = CLIB_MAGIC_CODE;
	pstCList->ulCount = HLIB_VECTOR_Length(pstList);
	pstCList->ulCrc = HLIB_MATH_GetCrc32(pstCList->data, pstCList->ulCount * sizeof(CListItem_t));

	if (pnSize)
		*pnSize = nSize;

	return pstCList;
}

HxVector_t*	CLIB_DeSerialize(CList_t *pstCList)
{
	HINT32		i;
	HxVector_t	*pstResultList = NULL;
	CListItem_t	*pstItemInfo;

	HxLOG_Assert(pstCList);

	if (pstCList->ulMagicCode != CLIB_MAGIC_CODE)
	{
		HxLOG_Error("CList is not valid!!!\n");
		return NULL;
	}

	if (pstCList->ulCrc != HLIB_MATH_GetCrc32(pstCList->data, pstCList->ulCount * sizeof(CListItem_t)))
	{
		HxLOG_Error("CList is not valid!!!\n");
		return NULL;
	}

	pstResultList = HLIB_VECTOR_NewEasy(pstCList->ulCount, HLIB_STD_MemFree_CB, NULL);

	pstItemInfo = (CListItem_t*)pstCList->data;
	for (i = 0; i < pstCList->ulCount; i++)
		HLIB_VECTOR_Add(pstResultList, HLIB_STD_MemDup(pstItemInfo++, sizeof(CListItem_t)));

	return pstResultList;
}

void		CLIB_RemoveCList(CList_t *pstCList)
{
	HLIB_STD_MemFree(pstCList);
}

void		CLIB_RemoveItemList(HxVector_t *pstList)
{
	HLIB_VECTOR_Delete(pstList);
}

/**************************************************************************************************/
#define _________CLIB_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/
#ifdef	CONFIG_DEBUG
void	CLIB_TEST_PrintList(HCHAR *pszName, HxVector_t *pstList)
{
	HINT32	i;
	CListItem_t	*pstItemInfo;

	HxLOG_Debug("LIST : [%s]\n", pszName);
	for (i = 0; i < HLIB_VECTOR_Length(pstList); i++)
	{
		pstItemInfo = (CListItem_t*)HLIB_VECTOR_ItemAt(pstList, i);
		HxLOG_Debug("LCN List : [%d] - [%lld]\n", pstItemInfo->nLogicalNo, pstItemInfo->uid);
	}
}

void	CLIB_TEST_GenerateDefaultList(HxVector_t *pstSvcList, void (*pfnWriteCallback)(int, char *, HxVector_t*))
{
	HINT32	i;
	HxVector_t	*pstTVList, *pstRadioList, *pstHDList, *pstFreeList, *pstCasList;
	HxVector_t	*pstFullList;

	pstTVList    = CLIB_GenerateList(pstSvcList, NULL,         CLIB_TYPE_TV,    CLIB_OP_NULL, 0, TRUE);
	if (pfnWriteCallback)
		pfnWriteCallback(1, "TVList", pstTVList);

	CLIB_TEST_PrintList("pstTVList", pstTVList);

	pstRadioList = CLIB_GenerateList(pstSvcList, NULL,         CLIB_TYPE_RADIO, CLIB_OP_NULL, 0, TRUE);
	if (pfnWriteCallback)
		pfnWriteCallback(2, "RadioList", pstRadioList);
	CLIB_TEST_PrintList("pstRadioList", pstRadioList);

	pstHDList    = CLIB_GenerateList(pstSvcList, pstTVList,    CLIB_TYPE_HD,    CLIB_OP_NULL, 0, TRUE);
	if (pfnWriteCallback)
		pfnWriteCallback(3, "HDList", pstHDList);
	CLIB_TEST_PrintList("pstHDList", pstHDList);

	pstFullList = HLIB_VECTOR_NewEasy(HLIB_VECTOR_Length(pstTVList) + HLIB_VECTOR_Length(pstRadioList), NULL, NULL);
	for (i = 0; i < HLIB_VECTOR_Length(pstTVList); i++)
		HLIB_VECTOR_Add(pstFullList, (void*)HLIB_VECTOR_ItemAt(pstTVList, i));
	for (i = 0; i < HLIB_VECTOR_Length(pstRadioList); i++)
		HLIB_VECTOR_Add(pstFullList, (void*)HLIB_VECTOR_ItemAt(pstRadioList, i));
	HLIB_VECTOR_Sort(pstFullList, clib_compareLogicalNumber);
	if (pfnWriteCallback)
		pfnWriteCallback(4, "ALL", pstFullList);

	pstFreeList  = CLIB_GenerateList(pstSvcList, pstFullList,    CLIB_TYPE_CAS,   CLIB_OP_NULL, eDxCAS_TYPE_FTA, TRUE);
	if (pfnWriteCallback)
		pfnWriteCallback(5, "FTA", pstFreeList);
	CLIB_TEST_PrintList("pstFreeList", pstFreeList);

	pstCasList   = CLIB_GenerateList(pstSvcList, pstFullList,    CLIB_TYPE_CAS,   CLIB_OP_NULL, 0xFFFE, TRUE);
	if (pfnWriteCallback)
		pfnWriteCallback(6, "CAS", pstCasList);
	CLIB_TEST_PrintList("pstCasList", pstCasList);
}

void	CLIB_TEST_GenerateAllCategory(HxVector_t *pstSvcList)
{
	HINT32		i, nSize;
	HCHAR		szName[32];
	HxVector_t	*pstNewList, *pstHDList;
	CListItem_t	*pstItemInfo;
	CList_t		*pstCList = NULL;

//	pstSvcList = NULL;
	pstNewList = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_TV, CLIB_OP_NULL, 0, TRUE);
	if (pstNewList)
	{
		for (i = 0; i < HLIB_VECTOR_Length(pstNewList); i++)
		{
			pstItemInfo = (CListItem_t*)HLIB_VECTOR_ItemAt(pstNewList, i);
			HxLOG_Debug("LCN List : [%d] - [%lld]\n", pstItemInfo->nLogicalNo, pstItemInfo->uid);
		}
		pstCList = CLIB_Serialize(pstNewList, &nSize);
		if (nSize && pstCList)
		{
			HxLOG_Debug("Serialized Data...\n");
			HLIB_LOG_Dump((HUINT8*)pstCList, nSize, 0, 0);
		}

		CLIB_RemoveItemList(pstNewList);
	}

	pstNewList = CLIB_DeSerialize(pstCList);
	if (pstNewList)
	{
		HxLOG_Debug("DeSerialized OK - [%s]\n", szName);
		for (i = 0; i < HLIB_VECTOR_Length(pstNewList); i++)
		{
			pstItemInfo = (CListItem_t*)HLIB_VECTOR_ItemAt(pstNewList, i);
			HxLOG_Debug("   - LCN [%04d] --- uid [%lld]\n", pstItemInfo->nLogicalNo, pstItemInfo->uid);
		}
	}

	pstHDList = CLIB_GenerateList(pstSvcList, pstNewList, CLIB_TYPE_HD, CLIB_OP_NULL, 0, TRUE);
	for (i = 0; i < HLIB_VECTOR_Length(pstHDList); i++)
	{
		pstItemInfo = (CListItem_t*)HLIB_VECTOR_ItemAt(pstHDList, i);
		HxLOG_Debug("   - LCN [%04d] --- uid [%lld]\n", pstItemInfo->nLogicalNo, pstItemInfo->uid);
	}

	CLIB_RemoveItemList(pstNewList);
	CLIB_RemoveCList(pstCList);
}

void	CLIB_TEST_MakeUserFavorite(void)
{
	HINT32		i, nSize;
	HCHAR		szName[32];
	CList_t		*pstCList;
	CListItem_t	stItemInfo, *pstItemInfo;
	HxVector_t	*pstUserList, *pstNewList;

	pstUserList = HLIB_VECTOR_NewEasy(1024, HLIB_STD_MemFree_CB, NULL);
	for (i = 0; i < 100; i++)
	{
		stItemInfo.uid = i + 3;
		stItemInfo.nLogicalNo = i + 3;
		stItemInfo.bLCNControl = FALSE;
		HLIB_VECTOR_Add(pstUserList, HLIB_STD_MemDup(&stItemInfo, sizeof(CListItem_t)));
	}
	pstCList = CLIB_Serialize(pstUserList, &nSize);
	if (pstCList && nSize)
	{
		HxLOG_Debug("Serialized Data...\n");
		HLIB_LOG_Dump((HUINT8*)pstCList, nSize, 0, 0);
	}

	HLIB_VECTOR_Delete(pstUserList);

	pstNewList = CLIB_DeSerialize(pstCList);
	if (pstNewList)
	{
		HxLOG_Debug("DeSerialized OK [%s]\n", szName);
		for (i = 0; i < HLIB_VECTOR_Length(pstNewList); i++)
		{
			pstItemInfo = (CListItem_t*)HLIB_VECTOR_ItemAt(pstNewList, i);
			HxLOG_Debug("   - LCN(%d) [%04d] --- uid [%lld]\n", pstItemInfo->bLCNControl, pstItemInfo->nLogicalNo, pstItemInfo->uid);
		}
	}

	CLIB_RemoveItemList(pstNewList);
	CLIB_RemoveCList(pstCList);
}
#endif

/**************************************************************************************************/
#define _________CLIB_End_Of_File____________________________________________________
/**************************************************************************************************/

