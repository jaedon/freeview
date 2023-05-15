/**
	@file	_token_url.c
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
} _hxtoken_URL_BaseItem_t;

typedef struct
{
	_hxtoken_URL_BaseItem_t		 stMainItem;

	HxList_t	*pstHeadSub;			// SubItems, _hxtoken_URL_BaseItem_t
	HxList_t	*pstCurrSub;			// SubItems, _hxtoken_URL_BaseItem_t
} _hxtoken_URL_MainItem_t;

typedef struct
{
	HxTOKEN_StringType_e	 eParseType;

	HxList_t			*pstHeadItem;	// _hxtoken_URL_MainItem_t
	HxList_t			*pstCurrItem;	// _hxtoken_URL_MainItem_t
} _hxtoken_URL_List_t;


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


STATIC HBOOL _hxtoken_url_compMainItemByKey (void *pvData, void *pvListItem)
{
	HCHAR						*szString = (HCHAR *)pvData;
	_hxtoken_URL_MainItem_t	*pstListItem = (_hxtoken_URL_MainItem_t *)pvListItem;

	if ((szString != NULL) && (pstListItem != NULL))
	{
		return (HxSTD_StrCmp (szString, pstListItem->stMainItem.szKey) == 0) ? TRUE : FALSE;
	}

	return FALSE;
}

STATIC HBOOL _hxtoken_url_compSubItemByKey (void *pvData, void *pvListItem)
{
	HCHAR					*szString = (HCHAR *)pvData;
	_hxtoken_URL_BaseItem_t	*pstListItem = (_hxtoken_URL_BaseItem_t *)pvListItem;

	if ((szString != NULL) && (pstListItem != NULL))
	{
		return (HxSTD_StrCmp (szString, pstListItem->szKey) == 0) ? TRUE : FALSE;
	}

	return FALSE;
}

STATIC void _hxtoken_url_freeSubItem (_hxtoken_URL_BaseItem_t *pstItem)
{
	if (NULL != pstItem)
	{
		HLIB_MEM_Free(pstItem);
	}
}

STATIC void _hxtoken_url_freeMainItem (_hxtoken_URL_MainItem_t *pstItem)
{
	_hxtoken_URL_BaseItem_t		*pstSubItem;

	if (NULL != pstItem)
	{
		while (NULL != pstItem->pstHeadSub)
		{
			pstSubItem = (_hxtoken_URL_BaseItem_t *)HLIB_LIST_Data (pstItem->pstHeadSub);
			if (NULL != pstSubItem)
			{
				pstItem->pstHeadSub = HLIB_LIST_Remove (pstItem->pstHeadSub, pstSubItem);
				_hxtoken_url_freeSubItem (pstSubItem);
			}
		}

		HLIB_MEM_Free(pstItem);
	}
}

STATIC void _hxtoken_url_freeList (_hxtoken_URL_List_t *pstList)
{
	_hxtoken_URL_MainItem_t		*pstItem;

	if (NULL != pstList)
	{
		while (NULL != pstList->pstHeadItem)
		{
			pstItem = (_hxtoken_URL_MainItem_t *)HLIB_LIST_Data (pstList->pstHeadItem);
			if (NULL != pstItem)
			{
				pstList->pstHeadItem = HLIB_LIST_Remove (pstList->pstHeadItem, pstItem);
				_hxtoken_url_freeMainItem (pstItem);
			}
		}

		HLIB_MEM_Free(pstList);
	}
}

STATIC HERROR _hxtoken_url_parseSubItem (_hxtoken_URL_MainItem_t *pstMainItem, HCHAR *szStart, HCHAR *szEnd)
{
	const STATIC HCHAR			*s_aszKeyDelim[]		= {"=", ".", "/"};
	HCHAR						*szTokenFound, *szNextString;
	_hxtoken_URL_BaseItem_t		*pstItem;
	HERROR						 hResult = ERR_FAIL;

	if (szStart > szEnd)
	{
		return ERR_FAIL;
	}

	pstItem = (_hxtoken_URL_BaseItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URL_BaseItem_t));
	if (NULL == pstItem)				{ return ERR_FAIL; }
	HxSTD_MemSet (pstItem, 0, sizeof(_hxtoken_URL_BaseItem_t));

	szTokenFound = NULL;
	szNextString = INT_HxToken_FindNextTokenPos (szStart, szEnd, s_aszKeyDelim,
		sizeof(s_aszKeyDelim) / sizeof(s_aszKeyDelim[0]), &szTokenFound);

	if (NULL == szTokenFound)
	{
		INT_HxToken_CopyString (szStart, szEnd, pstItem->szKey, HxTOKEN_KEY_SIZE);
	}
	else if (0 == HxSTD_StrCmp (szTokenFound, ".") || 0 == HxSTD_StrCmp (szTokenFound, "/") )
	{
		HUINT32	ulNth	= 0;
		if (szStart > (szNextString - 1))						{ goto END_FUNC; }

		ulNth	= HLIB_LIST_Length(pstMainItem->pstHeadSub);
		snprintf(pstItem->szKey, HxTOKEN_KEY_SIZE, "domain%dth", ulNth);
		INT_HxToken_CopyString (szStart, szNextString - 1, pstItem->szValue, HxTOKEN_VALUE_SIZE);
	}
	else if (HxSTD_StrCmp (szTokenFound, "=") == 0)
	{
		if (szStart > (szNextString - 1))						{ goto END_FUNC; }

		INT_HxToken_CopyString (szStart, szNextString - 1, pstItem->szKey, HxTOKEN_KEY_SIZE);
		if ((szNextString + 1) <= szEnd)
		{
			INT_HxToken_CopyString (szNextString + 1, szEnd - 1, pstItem->szValue, HxTOKEN_VALUE_SIZE);
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstItem)
	{
		if (ERR_OK == hResult)
		{
			pstMainItem->pstHeadSub = HLIB_LIST_Append (pstMainItem->pstHeadSub, (void *)pstItem);
		}
		else
		{
			HLIB_MEM_Free(pstItem);
		}
	}

	return hResult;
}

STATIC HERROR _hxtoken_url_parseSubList (_hxtoken_URL_MainItem_t *pstItem, HCHAR *szStart, HCHAR *szEnd)
{
	const STATIC HCHAR			*s_aszKeyDelim[]		= {"/", "&", "." };
	HCHAR						*szTokenFound, *szNextString;

	while (szStart <= szEnd)
	{
		szTokenFound = NULL;
		szNextString = INT_HxToken_FindNextTokenPos (szStart, szEnd, s_aszKeyDelim,
			sizeof(s_aszKeyDelim) / sizeof(s_aszKeyDelim[0]), &szTokenFound);

		_hxtoken_url_parseSubItem (pstItem, szStart, szNextString);
		szStart = szNextString + 1;
	}

	pstItem->pstCurrSub = pstItem->pstHeadSub;

	return ERR_OK;
}

STATIC HERROR _hxtoken_url_parseMainItem (_hxtoken_URL_MainItem_t *pstItem, HCHAR *szStart, HCHAR *szEnd)
{
	const STATIC HCHAR			*s_aszKeyDelim[]		= {"@", ".", "="};
	HCHAR						*szTokenFound, *szNextString;

	if (szStart > szEnd)
	{
		return ERR_FAIL;
	}

	szTokenFound = NULL;
	szNextString = INT_HxToken_FindNextTokenPos (szStart, szEnd, s_aszKeyDelim,
		sizeof(s_aszKeyDelim) / sizeof(s_aszKeyDelim[0]), &szTokenFound);
	if (NULL == szTokenFound)
	{
		INT_HxToken_CopyString (szStart, szEnd, pstItem->stMainItem.szKey, HxTOKEN_KEY_SIZE);
	}
	else if (HxSTD_StrCmp (szTokenFound, ".") == 0)
	{
		if (szStart > (szNextString - 1))
		{
			return ERR_FAIL;
		}

		INT_HxToken_CopyString (szStart, szNextString - 1, pstItem->stMainItem.szKey, HxTOKEN_KEY_SIZE);
		if ((szNextString + 1) <= szEnd)
		{
			INT_HxToken_CopyString (szNextString + 1, szEnd, pstItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
		}
	}
	else if (HxSTD_StrCmp (szTokenFound, "@") == 0)
	{
		if (szStart > (szNextString - 1))
		{
			return ERR_FAIL;
		}

		INT_HxToken_CopyString (szStart, szNextString - 1, pstItem->stMainItem.szKey, HxTOKEN_KEY_SIZE);
		if ((szNextString + 1) <= szEnd)
		{
			INT_HxToken_CopyString (szNextString + 1, szEnd, pstItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
		}
	}
	else if (HxSTD_StrCmp (szTokenFound, "=") == 0)
	{
		if (szStart > (szNextString - 1))
		{
			return ERR_FAIL;
		}

		INT_HxToken_CopyString (szStart, szNextString - 1, pstItem->stMainItem.szKey, HxTOKEN_KEY_SIZE);
		if ((szNextString + 1) <= szEnd)
		{
			INT_HxToken_CopyString (szNextString + 1, szEnd, pstItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
		}
	}

	return ERR_OK;
}


#define _____MEMBER_FUNCTIONS_____

HERROR INT_HxToken_URL_Parse (HCHAR *szString, HxTOKEN_Handle_t *phHandle)
{
	const STATIC HCHAR			*s_aszSlash[]			= { "/", "&" };
	const STATIC HCHAR			*s_aszKeyDelim[]		= { "/", "?", "=" };
	const STATIC HCHAR			*s_aszKeyBlank[]		= { " ", "\t", "\n" };
	const STATIC HCHAR			*s_aszUrlHeadDelim[]	= { "://" };

	HINT32						 nLeft;
	HCHAR						*szTokenFound;
	HCHAR						*szNextString, *szDelimPos, *szNextUrl, *szEndStr;
	_hxtoken_URL_MainItem_t		*pstUrlItem;
	_hxtoken_URL_List_t			*pstUrlList = NULL;
	HxList_t					*pstList = NULL;
	HERROR						 hErr;


	if ((NULL == szString) || (NULL == phHandle))
	{
		if (NULL != phHandle)
		{
			*phHandle = NULL;
		}

		return ERR_FAIL;
	}

	hErr = ERR_OK;
	pstList = NULL;
	*phHandle = NULL;

	// Remove the blanks
	INT_HxToken_RemoveTokens (szString, s_aszKeyBlank, 3);

	// 1: URL Header (before ://)
	szTokenFound = NULL;
	szNextString = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszUrlHeadDelim, 1, &szTokenFound);
	if ((NULL == szTokenFound) || (szNextString == szString))
	{
		return ERR_FAIL;
	}

	pstUrlItem = (_hxtoken_URL_MainItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URL_MainItem_t));
	if (pstUrlItem == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstUrlItem, 0, sizeof(_hxtoken_URL_MainItem_t));
	HxSTD_StrNCpy (pstUrlItem->stMainItem.szKey, "url_type", (HxTOKEN_KEY_SIZE-1));
	INT_HxToken_CopyString (szString, szNextString - 1, pstUrlItem->stMainItem.szValue, HxTOKEN_KEY_SIZE);
	pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);

	// Skip the URL Head and ://
	szString = (szNextString + 3);

	for (nLeft = (HINT32)HxSTD_StrLen (szString); nLeft > 0; szString = szNextUrl)
	{
		szTokenFound = NULL;
		szNextUrl = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszSlash, 2, &szTokenFound);
		szEndStr = szNextUrl + 1;

		if (szTokenFound != NULL)
		{
			szNextUrl += HxSTD_StrLen (szTokenFound);
		}

		nLeft -= (HINT32)(szNextUrl - szString);

		pstUrlItem = (_hxtoken_URL_MainItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URL_MainItem_t));
		if (NULL != pstUrlItem)
		{
			HxSTD_MemSet (pstUrlItem, 0, sizeof(_hxtoken_URL_MainItem_t));

			szTokenFound = NULL;
			szDelimPos = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszKeyDelim, 3, &szTokenFound);
			while ((NULL != szTokenFound) &&  ((szDelimPos-1) <= szString))
			{
				nLeft--;
				szString++;
				szDelimPos = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszKeyDelim, 3, &szTokenFound);
			}

			if (NULL == szTokenFound)
			{
				hErr  = _hxtoken_url_parseMainItem (pstUrlItem, szString, szDelimPos - 1);
			}
			else if (HxSTD_StrCmp (szTokenFound, "/") == 0)
			{
				hErr  = _hxtoken_url_parseMainItem (pstUrlItem, szString, szDelimPos - 1);
				hErr |= _hxtoken_url_parseSubList (pstUrlItem, szString, szEndStr);
			}
			else if (HxSTD_StrCmp (szTokenFound, "?") == 0)
			{
				hErr  = _hxtoken_url_parseMainItem (pstUrlItem, szString, szDelimPos - 1);
				hErr |= _hxtoken_url_parseSubList (pstUrlItem, szDelimPos + 1, szEndStr);
			}
			else if (HxSTD_StrCmp (szTokenFound, "=") == 0)
			{
				hErr  = _hxtoken_url_parseMainItem (pstUrlItem, szString, (szEndStr-2));
			}

			if (ERR_OK == hErr)
			{
				HxLOG_Print("[%s:%d] main(%s) value(%s)\r\n", __FUNCTION__, __LINE__, pstUrlItem->stMainItem.szKey, pstUrlItem->stMainItem.szValue);
				pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);
			}
			else
			{
				_hxtoken_url_freeMainItem (pstUrlItem);
			}
		}
	}

	if (pstList == NULL)
	{
		goto END_FUNC;
	}

	pstUrlList = (_hxtoken_URL_List_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URL_List_t));
	if (pstUrlList == NULL)
	{
		goto END_FUNC;
	}

	pstUrlList->eParseType	= eTokenStrType_URL;
	pstUrlList->pstHeadItem	= pstList;
	pstUrlList->pstCurrItem = pstList;

	*phHandle = (HxTOKEN_Handle_t)pstUrlList;
	return ERR_OK;

END_FUNC:
	while (NULL != pstList)
	{
		pstUrlItem = (_hxtoken_URL_MainItem_t *)HLIB_LIST_Data (pstList);
		if (NULL != pstUrlItem)
		{
			pstList = HLIB_LIST_Remove (pstList, (void *)pstUrlItem);
			_hxtoken_url_freeMainItem (pstUrlItem);
		}
	}

	return ERR_FAIL;
}

void INT_HxToken_URL_Free (HxTOKEN_Handle_t hHandle)
{
	_hxtoken_url_freeList ((_hxtoken_URL_List_t *)hHandle);
}

HERROR INT_HxToken_URL_ResetHandleCursor (HxTOKEN_Handle_t hHandle)
{
	HxList_t				*pstItem;
	_hxtoken_URL_List_t 	*pstList = (_hxtoken_URL_List_t *)hHandle;
	_hxtoken_URL_MainItem_t	*pstMainItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	// 1. Main Item 내를 다 Reset 시킨다.
	for (pstItem = pstList->pstHeadItem; NULL != pstItem; pstItem = pstItem->next)
	{
		pstMainItem = (_hxtoken_URL_MainItem_t *)HLIB_LIST_Data (pstItem);
		if (NULL != pstMainItem)
		{
			pstMainItem->pstCurrSub = pstMainItem->pstHeadSub;
		}
	}

	pstList->pstCurrItem = pstList->pstHeadItem;
	return ERR_OK;
}


HERROR INT_HxToken_URL_GetKeyAndValue (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR *szValue)
{
	_hxtoken_URL_List_t 	*pstList = (_hxtoken_URL_List_t *)hHandle;
	_hxtoken_URL_MainItem_t	*pstItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstItem = (_hxtoken_URL_MainItem_t *)HLIB_LIST_Data (pstList->pstCurrItem);
	if (NULL == pstItem)					{ return ERR_FAIL; }

	HxSTD_StrNCpy (szKey, pstItem->stMainItem.szKey, HxTOKEN_KEY_SIZE);
	HxSTD_StrNCpy (szValue, pstItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);

	pstList->pstCurrItem = pstList->pstCurrItem->next;
	return ERR_OK;
}

HERROR INT_HxToken_URL_FindKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR **pszValue)
{
	_hxtoken_URL_List_t 	*pstList = (_hxtoken_URL_List_t *)hHandle;
	_hxtoken_URL_MainItem_t	*pstItem;
	HxList_t				*pstListItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstListItem = HLIB_LIST_FindEx (pstList->pstHeadItem, (void *)szKey, _hxtoken_url_compMainItemByKey);
	pstItem = (_hxtoken_URL_MainItem_t *)HLIB_LIST_Data (pstListItem);
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	*pszValue = pstItem->stMainItem.szValue;
	return ERR_OK;
}

HERROR INT_HxToken_URL_FindSubItemKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR *szSubKey, HCHAR **pszValue)
{
	_hxtoken_URL_List_t 	*pstList = (_hxtoken_URL_List_t *)hHandle;
	_hxtoken_URL_MainItem_t	*pstMainItem;
	_hxtoken_URL_BaseItem_t	*pstSubItem;
	HxList_t				*pstMainList, *pstSubList;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstMainList = HLIB_LIST_FindEx (pstList->pstHeadItem, (void *)szKey, _hxtoken_url_compMainItemByKey);
	pstMainItem = (_hxtoken_URL_MainItem_t *)HLIB_LIST_Data (pstMainList);
	if (NULL == pstMainItem)
	{
		return ERR_FAIL;
	}

	pstSubList = HLIB_LIST_FindEx (pstMainItem->pstHeadSub, (void *)szSubKey, _hxtoken_url_compSubItemByKey);
	pstSubItem = (_hxtoken_URL_BaseItem_t *)HLIB_LIST_Data (pstSubList);
	if (NULL == pstSubItem)
	{
		return ERR_FAIL;
	}

	*pszValue = pstSubItem->szValue;
	return ERR_OK;
}

