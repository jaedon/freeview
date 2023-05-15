/**
	@file	_token_arg.c
	@brief	\n

	Description: 					\n
	Module:						\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <htype.h>
#include <hlib.h>

#include "_token_internal.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxToken"

#define	LOG				HxLOG_Print
#define	ERR_LOG			HxLOG_Error
#define MSG_LOG			HxLOG_Message


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HCHAR		 szKey[HxTOKEN_KEY_SIZE];
	HCHAR		 szValue[HxTOKEN_VALUE_SIZE];
} _hxtoken_ARG_BaseItem_t;

typedef struct
{
	HxTOKEN_StringType_e	 eParseType;

	HxList_t			*pstHeadItem;	// _hxtoken_ARG_BaseItem_t
	HxList_t			*pstCurrItem;	// _hxtoken_ARG_BaseItem_t
} _hxtoken_ARG_List_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/************************** Function *********************************/
/*******************************************************************/

#define _____LOCAL_FUNCTIONS_____

STATIC HBOOL _hxtoken_arg_compItemByKey (void *pvData, void *pvListItem)
{
	HCHAR					*szString = (HCHAR *)pvData;
	_hxtoken_ARG_BaseItem_t *pstListItem = (_hxtoken_ARG_BaseItem_t *)pvListItem;

	if ((szString != NULL) && (pstListItem != NULL))
	{
		return (HxSTD_StrCmp (szString, pstListItem->szKey) == 0) ? TRUE : FALSE;
	}

	return FALSE;
}

STATIC void _hxtoken_arg_freeItem (_hxtoken_ARG_BaseItem_t *pstItem)
{
	if (NULL != pstItem)
	{
		HLIB_MEM_Free(pstItem);
	}
}

STATIC void _hxtoken_arg_freeList (_hxtoken_ARG_List_t *pstList)
{
	_hxtoken_ARG_BaseItem_t 	*pstItem;

	if (NULL != pstList)
	{
		while (pstList->pstHeadItem != NULL)
		{
			pstItem = (_hxtoken_ARG_BaseItem_t *)HLIB_LIST_Data (pstList->pstHeadItem);
			if (NULL != pstItem)
			{
				pstList->pstHeadItem = HLIB_LIST_Remove (pstList->pstHeadItem, (void *)pstItem);
				_hxtoken_arg_freeItem (pstItem);
			}
		}

		HLIB_MEM_Free(pstList);
	}
}

STATIC HERROR _hxtoken_arg_parseItem  (_hxtoken_ARG_BaseItem_t *pstItem, HCHAR *szStart, HCHAR *szEnd)
{
	const STATIC HCHAR			*s_aszKeyDelim[]		= { "=" };
	HCHAR						*szTokenFound, *szNextString;

	if (szStart > szEnd)
	{
		return ERR_FAIL;
	}

	szTokenFound = NULL;
	szNextString = INT_HxToken_FindNextTokenPos (szStart, szEnd, s_aszKeyDelim, 1, &szTokenFound);
	if (NULL == szTokenFound)
	{
		INT_HxToken_CopyString (szStart, szEnd, pstItem->szKey, HxTOKEN_KEY_SIZE);
	}
	else if (HxSTD_StrCmp (szTokenFound, "=") == 0)
	{
		if (szStart > (szNextString - 1))
		{
			return ERR_FAIL;
		}

		INT_HxToken_CopyString (szStart, szNextString - 1, pstItem->szKey, HxTOKEN_KEY_SIZE);
		if ((szNextString + 1) <= szEnd)
		{
			INT_HxToken_CopyString (szNextString + 1, szEnd, pstItem->szValue, HxTOKEN_KEY_SIZE);
		}
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____MEMBER_FUNCTIONS_____

HERROR INT_HxToken_ARG_Parse (HCHAR *szString, HxTOKEN_Handle_t *phArgHandle)
{
	const STATIC HCHAR *s_aszKeyBlank[] = { " ", "\t", "\n" };
	const STATIC HCHAR *s_aszKeyDelim[] = { ",", "&" };

	HINT32						 nLeftStr;
	HCHAR						*szNextString;
	HxList_t					*pstList = NULL;
	_hxtoken_ARG_BaseItem_t 	*pstArgItem;
	_hxtoken_ARG_List_t 	*pstArgList = NULL;
	HERROR						 hErr;

	if ((NULL == szString) || (NULL == phArgHandle))
	{
		if (NULL != phArgHandle)
			*phArgHandle = NULL;
		return ERR_FAIL;
	}

	hErr = ERR_OK;
	pstList = NULL;
	*phArgHandle = NULL;

	INT_HxToken_RemoveTokens (szString, s_aszKeyBlank, 3);

	nLeftStr = HxSTD_StrLen (szString);
	while (nLeftStr > 0)
	{
		// 다음 delimiter를 찾는다.
		szNextString = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszKeyDelim, 2, NULL);

		pstArgItem = (_hxtoken_ARG_BaseItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_ARG_BaseItem_t));
		if (pstArgItem != NULL)
		{
			HxSTD_MemSet (pstArgItem, 0, sizeof(_hxtoken_ARG_BaseItem_t));

			hErr = _hxtoken_arg_parseItem (pstArgItem, szString, szNextString - 1);
			if (ERR_OK == hErr)
			{
				pstList = HLIB_LIST_Append (pstList, (void *)pstArgItem);
			}
			else
			{
				HLIB_MEM_Free(pstArgItem);
			}
		}

		nLeftStr -= (HINT32)(szNextString + 1 - szString);
		szString = szNextString + 1;
	}

	if (pstList == NULL)
	{
		goto END_FUNC;
	}

	pstArgList = (_hxtoken_ARG_List_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_ARG_List_t));
	if (pstArgList == NULL)
	{
		goto END_FUNC;
	}

	pstArgList->eParseType	= eTokenStrType_ARGUMENT;
	pstArgList->pstHeadItem = pstList;
	pstArgList->pstCurrItem = pstList;

	*phArgHandle = (HxTOKEN_Handle_t)pstArgList;
	return ERR_OK;

END_FUNC:
	while (pstList != NULL)
	{
		pstArgItem = (_hxtoken_ARG_BaseItem_t *)HLIB_LIST_Data (pstList);
		if (pstArgItem != NULL)
		{
			pstList = HLIB_LIST_Remove (pstList, (void *)pstArgItem);
			HLIB_MEM_Free(pstArgItem);
		}
	}

	return ERR_FAIL;
}

void INT_HxToken_ARG_Free (HxTOKEN_Handle_t hArgHandle)
{
	_hxtoken_arg_freeList ((_hxtoken_ARG_List_t *)hArgHandle);
}


HERROR INT_HxToken_ARG_ResetHandleCursor (HxTOKEN_Handle_t hArgHandle)
{
	_hxtoken_ARG_List_t		*pstList = (_hxtoken_ARG_List_t *)hArgHandle;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstList->pstCurrItem = pstList->pstHeadItem;
	return ERR_OK;
}

HUINT32 INT_HxToken_ARG_CountMainKey (HxTOKEN_Handle_t hArgHandle)
{
	_hxtoken_ARG_List_t		*pstList = (_hxtoken_ARG_List_t *)hArgHandle;

	if (NULL == pstList)
	{
		return 0;
	}

	return HLIB_LIST_Length (pstList->pstHeadItem);
}

HERROR INT_HxToken_ARG_GetKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HCHAR *szKey, HCHAR *szValue)
{
	HUINT32					 ulCount;
	HxList_t				*pstListItem;
	_hxtoken_ARG_List_t		*pstList = (_hxtoken_ARG_List_t *)hArgHandle;
	_hxtoken_ARG_BaseItem_t *pstItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstListItem = pstList->pstHeadItem;
	for (ulCount = 0; (ulCount < ulIndex) && (pstListItem); ulCount ++)
	{
		pstListItem = pstListItem->next;
	}

	pstItem = (_hxtoken_ARG_BaseItem_t *)HLIB_LIST_Data (pstListItem);
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	if (NULL != szKey)
	{
		HxSTD_StrNCpy (szKey, pstItem->szKey, HxTOKEN_KEY_SIZE);
	}

	if (NULL != szValue)
	{
		HxSTD_StrNCpy (szValue, pstItem->szValue, HxTOKEN_VALUE_SIZE);
	}

	return ERR_OK;
}

HUINT32 INT_HxToken_ARG_CountSubKey (HxTOKEN_Handle_t hArgHandle, HUINT32 ulMainIdx)
{
	return 0;
}

HERROR INT_HxToken_ARG_GetSubKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HUINT32 ulSubIndex, HCHAR *szKey, HCHAR *szValue)
{
	if (NULL != szKey)					{ *szKey = '\0'; }
	if (NULL != szValue)				{ *szValue = '\0'; }

	return ERR_FAIL;
}

HERROR INT_HxToken_ARG_FindKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR **pszValue)
{
	_hxtoken_ARG_List_t		*pstList = (_hxtoken_ARG_List_t *)hArgHandle;
	_hxtoken_ARG_BaseItem_t *pstItem;
	HxList_t				*pstListItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstListItem = HLIB_LIST_FindEx (pstList->pstHeadItem, (void *)szKey, _hxtoken_arg_compItemByKey);
	pstItem = (_hxtoken_ARG_BaseItem_t *)HLIB_LIST_Data (pstListItem);
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	*pszValue = pstItem->szValue;
	return ERR_OK;
}

HERROR INT_HxToken_ARG_FindSubItemKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey, HCHAR **pszValue)
{
	NOT_USED_PARAM (hArgHandle);		NOT_USED_PARAM (szKey);
	NOT_USED_PARAM (szSubKey);			NOT_USED_PARAM (pszValue);

	return ERR_FAIL;
}

