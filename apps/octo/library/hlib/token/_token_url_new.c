/**
	@file	_token_url_new.c
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

#define _URLTYPE_NONE			0
#define _URLTYPE_HTTP			1
#define _URLTYPE_FILE			2
#define _URLTYPE_HDVB			3


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HCHAR		 szKey[HxTOKEN_KEY_SIZE];
	HCHAR		 szValue[HxTOKEN_VALUE_SIZE];
} _hxtoken_URLNEW_SubItem_t;

typedef struct
{
	HCHAR		 szKey[HxTOKEN_KEY_SIZE];
	HCHAR		*szValue;
} _hxtoken_URLNEW_MainValue_t;

typedef struct
{
	_hxtoken_URLNEW_MainValue_t		 stMainItem;

	HxList_t	*pstHeadSub;			// SubItems, _hxtoken_URLNEW_SubItem_t
	HxList_t	*pstCurrSub;			// SubItems, _hxtoken_URLNEW_SubItem_t
} _hxtoken_URLNEW_MainItem_t;

typedef struct
{
	HxTOKEN_StringType_e	 eParseType;

	HxList_t			*pstHeadItem;	// _hxtoken_URLNEW_MainItem_t
	HxList_t			*pstCurrItem;	// _hxtoken_URLNEW_MainItem_t
} _hxtoken_URLNEW_List_t;


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

STATIC HBOOL _hxtoken_urlnew_compKey (HCHAR *szKey1, HCHAR *szKey2)
{
	while (1)
	{
		if (('\0' == *szKey1) && ('\0' == *szKey2))
		{
			return TRUE;
		}

		if (tolower (*szKey1) != tolower (*szKey2))
		{
			return FALSE;
		}

		szKey1 ++;
		szKey2 ++;
	}

	return FALSE;
}


STATIC HBOOL _hxtoken_urlnew_compMainItemByKey (void *pvData, void *pvListItem)
{
	HCHAR						*szString = (HCHAR *)pvData;
	_hxtoken_URLNEW_MainItem_t	*pstListItem = (_hxtoken_URLNEW_MainItem_t *)pvListItem;

	if ((szString != NULL) && (pstListItem != NULL))
	{
		return _hxtoken_urlnew_compKey (szString, pstListItem->stMainItem.szKey);
	}

	return FALSE;
}

STATIC HBOOL _hxtoken_urlnew_compSubItemByKey (void *pvData, void *pvListItem)
{
	HCHAR					*szString = (HCHAR *)pvData;
	_hxtoken_URLNEW_SubItem_t	*pstListItem = (_hxtoken_URLNEW_SubItem_t *)pvListItem;

	if ((szString != NULL) && (pstListItem != NULL))
	{
		return _hxtoken_urlnew_compKey (szString, pstListItem->szKey);
	}

	return FALSE;
}

STATIC _hxtoken_URLNEW_MainItem_t *_hxtoken_urlnew_allocMainItem (HUINT32 ulValueLen)
{
	HCHAR		*szValue;
	_hxtoken_URLNEW_MainItem_t		*pstItem;

	szValue = (HCHAR *)HLIB_MEM_Malloc(sizeof(HCHAR) * ulValueLen);
	if (NULL == szValue)
	{
		return NULL;
	}

	pstItem = (_hxtoken_URLNEW_MainItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_MainItem_t));
	if (NULL == pstItem)
	{
		HLIB_MEM_Free(szValue);
		return NULL;
	}

	HxSTD_MemSet (szValue, 0, sizeof(HCHAR) * ulValueLen);
	HxSTD_MemSet (pstItem, 0, sizeof(_hxtoken_URLNEW_MainItem_t));

	pstItem->stMainItem.szValue = szValue;
	return pstItem;
}

STATIC void _hxtoken_urlnew_freeSubItem (_hxtoken_URLNEW_SubItem_t *pstItem)
{
	if (NULL != pstItem)
	{
		HLIB_MEM_Free(pstItem);
	}
}

STATIC void _hxtoken_urlnew_freeMainItem (_hxtoken_URLNEW_MainItem_t *pstItem)
{
	_hxtoken_URLNEW_SubItem_t		*pstSubItem;

	if (NULL != pstItem)
	{
		while (NULL != pstItem->pstHeadSub)
		{
			pstSubItem = (_hxtoken_URLNEW_SubItem_t *)HLIB_LIST_Data (pstItem->pstHeadSub);
			if (NULL != pstSubItem)
			{
				pstItem->pstHeadSub = HLIB_LIST_Remove (pstItem->pstHeadSub, pstSubItem);
				_hxtoken_urlnew_freeSubItem (pstSubItem);
			}
		}

		if (NULL != pstItem->stMainItem.szValue)
		{
			HLIB_MEM_Free(pstItem->stMainItem.szValue);
		}

		HLIB_MEM_Free(pstItem);
	}
}

STATIC void _hxtoken_urlnew_freeList (_hxtoken_URLNEW_List_t *pstList)
{
	_hxtoken_URLNEW_MainItem_t		*pstItem;

	if (NULL != pstList)
	{
		while (NULL != pstList->pstHeadItem)
		{
			pstItem = (_hxtoken_URLNEW_MainItem_t *)HLIB_LIST_Data (pstList->pstHeadItem);
			if (NULL != pstItem)
			{
				pstList->pstHeadItem = HLIB_LIST_Remove (pstList->pstHeadItem, pstItem);
				_hxtoken_urlnew_freeMainItem (pstItem);
			}
		}

		HLIB_MEM_Free(pstList);
	}
}

STATIC HERROR _hxtoken_urlnew_parseSubItem (_hxtoken_URLNEW_MainItem_t *pstMainItem, HCHAR *szStart, HCHAR *szEnd)
{
	const STATIC HCHAR			*s_aszKeyDelim[]		= {"="};
	HCHAR						*szTokenFound, *szNextString;
	_hxtoken_URLNEW_SubItem_t		*pstItem;
	HERROR						 hResult = ERR_FAIL;

	if (szStart > szEnd)
	{
		return ERR_FAIL;
	}

	pstItem = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
	if (NULL == pstItem)				{ return ERR_FAIL; }
	HxSTD_MemSet (pstItem, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

	szTokenFound = NULL;
 	szNextString = INT_HxToken_FindNextTokenPos (szStart, szEnd, s_aszKeyDelim, 1, &szTokenFound);
	if (NULL == szTokenFound)
	{
		INT_HxToken_CopyString (szStart, szEnd, pstItem->szKey, HxTOKEN_KEY_SIZE);
	}
	else if (_hxtoken_urlnew_compKey (szTokenFound, "="))
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

STATIC HERROR _hxtoken_urlnew_parseSubList (_hxtoken_URLNEW_MainItem_t *pstItem, HCHAR *szStart, HCHAR *szEnd)
{
	const STATIC HCHAR			*s_aszKeyDelim[]		= { "&" };
	HCHAR						*szTokenFound, *szNextString;

	if (NULL == szEnd)
	{
		szEnd = szStart + HxSTD_StrLen (szStart);
	}

	while (szStart <= szEnd)
	{
		szTokenFound = NULL;
		szNextString = INT_HxToken_FindNextTokenPos (szStart, szEnd, s_aszKeyDelim, 1, &szTokenFound);

		_hxtoken_urlnew_parseSubItem (pstItem, szStart, szNextString);
		szStart = szNextString + 1;
	}

	pstItem->pstCurrSub = pstItem->pstHeadSub;

	return ERR_OK;
}

STATIC _hxtoken_URLNEW_MainItem_t *_hxtoken_urlnew_getMainItem (_hxtoken_URLNEW_List_t *pstList, HUINT32 ulIndex)
{
	HUINT32						 ulCount;
	HxList_t					*pstListItem;

	if (NULL == pstList)
	{
		return NULL;
	}

	pstListItem = pstList->pstHeadItem;
	for (ulCount = 0; (ulCount < ulIndex) && (NULL != pstListItem) ; ulCount ++)
	{
		pstListItem = pstListItem->next;
	}

	return (_hxtoken_URLNEW_MainItem_t *)HLIB_LIST_Data (pstListItem);
}

STATIC HERROR _hxtoken_urlnew_parseHdvbDomain (_hxtoken_URLNEW_MainItem_t *pstUrlItem, HCHAR *szDomain)
{
	const STATIC HCHAR			*s_aszDot[]			= { "." };

	HCHAR						*szCurr, *szNext, *szEnd;
	_hxtoken_URLNEW_SubItem_t	*pstSubItem_Type = NULL;
	_hxtoken_URLNEW_SubItem_t	*pstSubItem_onid = NULL, *pstSubItem_tsid = NULL, *pstSubItem_svcid = NULL;
	_hxtoken_URLNEW_SubItem_t	*pstSubItem_svcno = NULL;

	if ((NULL == pstUrlItem) || (NULL == szDomain) || ('\0' == szDomain[0]))
	{
		goto FUNC_ERROR;
	}

	szEnd = szDomain + HxSTD_StrLen (szDomain);
	szCurr = szDomain;

	// Type (svcno/svcid)
	pstSubItem_Type = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
	if (NULL == pstSubItem_Type)		{ goto FUNC_ERROR; }
	HxSTD_MemSet (pstSubItem_Type, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

	szNext = INT_HxToken_FindNextTokenPos (szCurr, NULL, s_aszDot, 1, NULL);
	HxSTD_StrNCpy (pstSubItem_Type->szKey, HxTOKEN_HDVB_TOKEN_TUNETYPE,(HxTOKEN_KEY_SIZE-1));
	INT_HxToken_CopyString (szCurr, szNext - 1, pstSubItem_Type->szValue, HxTOKEN_KEY_SIZE);

	if (_hxtoken_urlnew_compKey (pstSubItem_Type->szValue, HxTOKEN_HDVB_TOKEN_SVCNO))
	{
		// Svc No
		szCurr = szNext + 1;
		if (szCurr >= szEnd)			{ goto FUNC_ERROR; }

		pstSubItem_svcno = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
		if (NULL == pstSubItem_svcno)		{ goto FUNC_ERROR; }
		HxSTD_MemSet (pstSubItem_svcno, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

		szNext = INT_HxToken_FindNextTokenPos (szCurr, NULL, s_aszDot, 1, NULL);
		HxSTD_StrNCpy (pstSubItem_svcno->szKey, HxTOKEN_HDVB_TOKEN_SVCNO,(HxTOKEN_KEY_SIZE-1));
		INT_HxToken_CopyString (szCurr, szNext - 1, pstSubItem_svcno->szValue, HxTOKEN_VALUE_SIZE);
	}
	else if (_hxtoken_urlnew_compKey (pstSubItem_Type->szValue, HxTOKEN_HDVB_TOKEN_SVCUID))
	{
		// Svc UID
		szCurr = szNext + 1;
		if (szCurr >= szEnd)			{ goto FUNC_ERROR; }

		pstSubItem_svcno = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
		if (NULL == pstSubItem_svcno)		{ goto FUNC_ERROR; }
		HxSTD_MemSet (pstSubItem_svcno, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

		szNext = INT_HxToken_FindNextTokenPos (szCurr, NULL, s_aszDot, 1, NULL);
		HxSTD_StrNCpy (pstSubItem_svcno->szKey, HxTOKEN_HDVB_TOKEN_SVCUID,(HxTOKEN_KEY_SIZE-1));
		INT_HxToken_CopyString (szCurr, szNext - 1, pstSubItem_svcno->szValue, HxTOKEN_VALUE_SIZE);
	}
	else if (_hxtoken_urlnew_compKey (pstSubItem_Type->szValue, HxTOKEN_HDVB_TOKEN_TRIPLEID))
	{
		// SVC ID
		szCurr = szNext + 1;
		if (szCurr >= szEnd)			{ goto FUNC_ERROR; }

		pstSubItem_svcid = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
		if (NULL == pstSubItem_svcid)		{ goto FUNC_ERROR; }
		HxSTD_MemSet (pstSubItem_svcid, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

		szNext = INT_HxToken_FindNextTokenPos (szCurr, NULL, s_aszDot, 1, NULL);
		HxSTD_StrNCpy (pstSubItem_svcid->szKey, HxTOKEN_HDVB_TOKEN_SVCID,(HxTOKEN_KEY_SIZE-1));
		INT_HxToken_CopyString (szCurr, szNext - 1, pstSubItem_svcid->szValue, HxTOKEN_VALUE_SIZE);


		// TS ID
		szCurr = szNext + 1;
		if (szCurr >= szEnd)			{ goto FUNC_ERROR; }

		pstSubItem_tsid = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
		if (NULL == pstSubItem_tsid)		{ goto FUNC_ERROR; }
		HxSTD_MemSet (pstSubItem_tsid, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

		szNext = INT_HxToken_FindNextTokenPos (szCurr, NULL, s_aszDot, 1, NULL);
		HxSTD_StrNCpy (pstSubItem_tsid->szKey, HxTOKEN_HDVB_TOKEN_TSID,(HxTOKEN_KEY_SIZE-1));
		INT_HxToken_CopyString (szCurr, szNext - 1, pstSubItem_tsid->szValue, HxTOKEN_VALUE_SIZE);

		// OrgNetID
		szCurr = szNext + 1;
		if (szCurr >= szEnd)			{ goto FUNC_ERROR; }

		pstSubItem_onid = (_hxtoken_URLNEW_SubItem_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_SubItem_t));
		if (NULL == pstSubItem_onid)		{ goto FUNC_ERROR; }
		HxSTD_MemSet (pstSubItem_onid, 0, sizeof(_hxtoken_URLNEW_SubItem_t));

		szNext = INT_HxToken_FindNextTokenPos (szCurr, NULL, s_aszDot, 1, NULL);
		HxSTD_StrNCpy (pstSubItem_onid->szKey, HxTOKEN_HDVB_TOKEN_ONID,(HxTOKEN_KEY_SIZE-1));
		INT_HxToken_CopyString (szCurr, szNext - 1, pstSubItem_onid->szValue, HxTOKEN_VALUE_SIZE);
	}
	else
	{
		goto FUNC_ERROR;
	}

	if (NULL != pstSubItem_Type)
	{
		pstUrlItem->pstHeadSub = HLIB_LIST_Append (pstUrlItem->pstHeadSub, (void *)pstSubItem_Type);
	}

	if (NULL != pstSubItem_svcno)
	{
		pstUrlItem->pstHeadSub = HLIB_LIST_Append (pstUrlItem->pstHeadSub, (void *)pstSubItem_svcno);
	}

	if (NULL != pstSubItem_onid)
	{
		pstUrlItem->pstHeadSub = HLIB_LIST_Append (pstUrlItem->pstHeadSub, (void *)pstSubItem_onid);
	}

	if (NULL != pstSubItem_tsid)
	{
		pstUrlItem->pstHeadSub = HLIB_LIST_Append (pstUrlItem->pstHeadSub, (void *)pstSubItem_tsid);
	}

	if (NULL != pstSubItem_svcid)
	{
		pstUrlItem->pstHeadSub = HLIB_LIST_Append (pstUrlItem->pstHeadSub, (void *)pstSubItem_svcid);
	}

	return ERR_OK;

FUNC_ERROR:
	if (NULL != pstSubItem_Type)
	{
		HLIB_MEM_Free(pstSubItem_Type);
	}

	if (NULL != pstSubItem_svcno)
	{
		HLIB_MEM_Free(pstSubItem_svcno);
	}

	if (NULL != pstSubItem_onid)
	{
		HLIB_MEM_Free(pstSubItem_onid);
	}

	if (NULL != pstSubItem_tsid)
	{
		HLIB_MEM_Free(pstSubItem_tsid);
	}

	if (NULL != pstSubItem_svcid)
	{
		HLIB_MEM_Free(pstSubItem_svcid);
	}

	return ERR_FAIL;
}


#define _____MEMBER_FUNCTIONS_____

HERROR INT_HxToken_URLNEW_Parse (HCHAR *szString, HxTOKEN_Handle_t *phHandle)
{
	const STATIC HCHAR			*s_aszSlash[]			= { "/" };
	const STATIC HCHAR			*s_aszFileDelim[]		= { "?" };
	const STATIC HCHAR			*s_aszKeyBlank[]		= { " ", "\t", "\n" };
	const STATIC HCHAR			*s_aszUrlHeadDelim[]	= { "://" };

	HBOOL						 bDirExist = FALSE;
	HUINT32						 ulUrlType = _URLTYPE_NONE;
	HCHAR						*szTokenFound;
	HCHAR						*szNextString, *szNextUrl;
	_hxtoken_URLNEW_MainItem_t		*pstUrlItem;
	_hxtoken_URLNEW_List_t			*pstUrlList = NULL;
	HxList_t					*pstList = NULL;
	HERROR						 hErr;


	if ((NULL == szString) || (NULL == phHandle))
	{
		return ERR_FAIL;
	}

	hErr = ERR_OK;
	pstList = NULL;

	// Remove the blanks
	INT_HxToken_RemoveTokens (szString, s_aszKeyBlank, 3);

	// 1: URL Header (before ://)
	szTokenFound = NULL;
	szNextString = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszUrlHeadDelim, 1, &szTokenFound);
	if ((NULL == szTokenFound) || (szNextString == szString))
	{
		return ERR_FAIL;
	}

	pstUrlItem = (_hxtoken_URLNEW_MainItem_t *)_hxtoken_urlnew_allocMainItem((HUINT32)(szNextString - szString + 2));
	if (pstUrlItem == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_StrNCpy (pstUrlItem->stMainItem.szKey, HxTOKEN_URL_URLTYPE,(HxTOKEN_KEY_SIZE-1));
	INT_HxToken_CopyString (szString, szNextString - 1, pstUrlItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
	pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);

	if (_hxtoken_urlnew_compKey (pstUrlItem->stMainItem.szValue, HxTOKEN_URLTYPE_HDVB))
	{
		ulUrlType = _URLTYPE_HDVB;
	}

	szString = (szNextString + 3);			// Skip the URL Head and ://

	// 2: Domain (192.168.0.1, svcid.128.1.13, etc)
	szTokenFound = NULL;
	szNextUrl = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszSlash, 1, &szTokenFound);
	if (('/' == szNextUrl[0]) && ('\0' != szNextUrl[1]))
	{
		bDirExist = TRUE;
	}

	pstUrlItem = (_hxtoken_URLNEW_MainItem_t *)_hxtoken_urlnew_allocMainItem ((HUINT32)(szNextUrl - szString + 2));
	if (NULL == pstUrlItem)
	{
		goto END_FUNC;
	}

	HxSTD_StrNCpy (pstUrlItem->stMainItem.szKey, HxTOKEN_URL_DOMAIN,(HxTOKEN_KEY_SIZE-1));
	INT_HxToken_CopyString (szString, szNextUrl - 1, pstUrlItem->stMainItem.szValue, HxTOKEN_KEY_SIZE);

	switch (ulUrlType)
	{
	case _URLTYPE_HDVB:
		_hxtoken_urlnew_parseHdvbDomain (pstUrlItem, pstUrlItem->stMainItem.szValue);
		break;

	default:
		break;
	}

	pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);

	if (TRUE == bDirExist)
	{
		szString	= szNextUrl + 1;

		szTokenFound = NULL;
		szNextUrl = INT_HxToken_FindLastTokenPos (szString, NULL, s_aszSlash, 1, &szTokenFound);
		if ('/' == *szNextUrl)
		{
			// Path :
			pstUrlItem = (_hxtoken_URLNEW_MainItem_t *)_hxtoken_urlnew_allocMainItem ((HUINT32)(szNextUrl - szString + 2));
			if (NULL == pstUrlItem)
			{
				goto END_FUNC;
			}

			HxSTD_StrNCpy (pstUrlItem->stMainItem.szKey, HxTOKEN_URL_PATH, (HxTOKEN_KEY_SIZE-1));
			INT_HxToken_CopyString (szString, szNextUrl - 1, pstUrlItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
			pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);

			szString = szNextUrl + 1;
		}
		else
		{
			// Path : NULL
			pstUrlItem = (_hxtoken_URLNEW_MainItem_t *)_hxtoken_urlnew_allocMainItem ((HUINT32)(szNextUrl - szString + 2));
			if (NULL == pstUrlItem)
			{
				goto END_FUNC;
			}

			HxSTD_StrNCpy (pstUrlItem->stMainItem.szKey, HxTOKEN_URL_PATH,(HxTOKEN_KEY_SIZE-1));
			pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);
		}

		if ('\0' != szString)
		{
			// File :
			pstUrlItem = (_hxtoken_URLNEW_MainItem_t *)_hxtoken_urlnew_allocMainItem ((HUINT32)HxSTD_StrLen (szString) + 2);
			if (NULL == pstUrlItem)
			{
				goto END_FUNC;
			}

			HxSTD_StrNCpy (pstUrlItem->stMainItem.szKey, HxTOKEN_URL_FILE,(HxTOKEN_KEY_SIZE-1));

			szNextUrl = INT_HxToken_FindNextTokenPos (szString, NULL, s_aszFileDelim, 1, &szTokenFound);

			// File
			INT_HxToken_CopyString (szString, szNextUrl - 1, pstUrlItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
			if (('\0' != szNextUrl[0]) && ('\0' != szNextUrl[1]))
			{
				// Argument
				hErr = _hxtoken_urlnew_parseSubList (pstUrlItem, szNextUrl + 1, NULL);
			}

			pstList = HLIB_LIST_Append (pstList, (void *)pstUrlItem);
		}
	}


	if (pstList == NULL)
	{
		goto END_FUNC;
	}

	pstUrlList = (_hxtoken_URLNEW_List_t *)HLIB_MEM_Malloc(sizeof(_hxtoken_URLNEW_List_t));
	if (pstUrlList == NULL)
	{
		goto END_FUNC;
	}

	pstUrlList->eParseType	= eTokenStrType_URL_NEW;
	pstUrlList->pstHeadItem	= pstList;
	pstUrlList->pstCurrItem = pstList;

	*phHandle = (HxTOKEN_Handle_t)pstUrlList;
	return ERR_OK;

END_FUNC:
	while (NULL != pstList)
	{
		pstUrlItem = (_hxtoken_URLNEW_MainItem_t *)HLIB_LIST_Data (pstList);
		if (NULL != pstUrlItem)
		{
			pstList = HLIB_LIST_Remove (pstList, (void *)pstUrlItem);
			_hxtoken_urlnew_freeMainItem (pstUrlItem);
		}
	}

	return ERR_FAIL;
}

void INT_HxToken_URLNEW_Free (HxTOKEN_Handle_t hHandle)
{
	_hxtoken_urlnew_freeList ((_hxtoken_URLNEW_List_t *)hHandle);
}

HERROR INT_HxToken_URLNEW_ResetHandleCursor (HxTOKEN_Handle_t hHandle)
{
	HxList_t				*pstItem;
	_hxtoken_URLNEW_List_t 	*pstList = (_hxtoken_URLNEW_List_t *)hHandle;
	_hxtoken_URLNEW_MainItem_t	*pstMainItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	// 1. Main Item 내를 다 Reset 시킨다.
	for (pstItem = pstList->pstHeadItem; NULL != pstItem; pstItem = pstItem->next)
	{
		pstMainItem = (_hxtoken_URLNEW_MainItem_t *)HLIB_LIST_Data (pstItem);
		if (NULL != pstMainItem)
		{
			pstMainItem->pstCurrSub = pstMainItem->pstHeadSub;
		}
	}

	pstList->pstCurrItem = pstList->pstHeadItem;
	return ERR_OK;
}

HERROR INT_HxToken_URLNEW_CountMainKey (HxTOKEN_Handle_t hArgHandle)
{
	_hxtoken_URLNEW_List_t		*pstList = (_hxtoken_URLNEW_List_t *)hArgHandle;

	if (NULL == pstList)
	{
		return 0;
	}

	return HLIB_LIST_Length (pstList->pstHeadItem);
}

HERROR INT_HxToken_URLNEW_GetKeyAndValue (HxTOKEN_Handle_t hHandle, HUINT32 ulIndex, HCHAR *szKey, HCHAR *szValue)
{
	_hxtoken_URLNEW_List_t		*pstList = (_hxtoken_URLNEW_List_t *)hHandle;
	_hxtoken_URLNEW_MainItem_t	*pstItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstItem = _hxtoken_urlnew_getMainItem (pstList, ulIndex);
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	if (NULL != szKey)
	{
		HxSTD_StrNCpy (szKey, pstItem->stMainItem.szKey, HxTOKEN_KEY_SIZE);
	}

	if (NULL != szValue)
	{
		HxSTD_StrNCpy (szValue, pstItem->stMainItem.szValue, HxTOKEN_VALUE_SIZE);
	}

	return ERR_OK;
}

HUINT32 INT_HxToken_URLNEW_CountSubKey (HxTOKEN_Handle_t hHandle, HUINT32 ulIndex)
{
	_hxtoken_URLNEW_List_t		*pstList = (_hxtoken_URLNEW_List_t *)hHandle;
	_hxtoken_URLNEW_MainItem_t	*pstItem;

	if (NULL == pstList)
	{
		return 0;
	}

	pstItem = _hxtoken_urlnew_getMainItem (pstList, ulIndex);
	if (NULL == pstItem)
	{
		return 0;
	}

	return HLIB_LIST_Length (pstItem->pstHeadSub);
}

HERROR INT_HxToken_URLNEW_GetSubKeyAndValue (HxTOKEN_Handle_t hHandle, HUINT32 ulIndex, HUINT32 ulSubIndex, HCHAR *szKey, HCHAR *szValue)
{
	HUINT32						 ulCount;
	HxList_t					*pstListItem;
	_hxtoken_URLNEW_List_t		*pstList = (_hxtoken_URLNEW_List_t *)hHandle;
	_hxtoken_URLNEW_MainItem_t	*pstItem;
	_hxtoken_URLNEW_SubItem_t	*pstSubItem;

	if (NULL != szKey)					{ *szKey = '\0'; }
	if (NULL != szValue)				{ *szValue = '\0'; }

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstItem = _hxtoken_urlnew_getMainItem (pstList, ulIndex);
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	pstListItem = pstItem->pstHeadSub;

	for (ulCount = 0; (ulCount < ulIndex) && (NULL != pstListItem); ulCount++)
	{
		pstListItem = pstListItem->next;
	}

	pstSubItem = (_hxtoken_URLNEW_SubItem_t *)HLIB_LIST_Data (pstListItem);
	if (NULL == pstSubItem)
	{
		return ERR_FAIL;
	}

	if (NULL != szKey)					{ HxSTD_StrNCpy (szKey, pstSubItem->szKey, (HxTOKEN_KEY_SIZE-1)); }
	if (NULL != szValue)				{ HxSTD_StrNCpy (szValue, pstSubItem->szValue, (HxTOKEN_KEY_SIZE-1)); }

	return ERR_OK;
}

HERROR INT_HxToken_URLNEW_FindKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR **pszValue)
{
	_hxtoken_URLNEW_List_t 	*pstList = (_hxtoken_URLNEW_List_t *)hHandle;
	_hxtoken_URLNEW_MainItem_t	*pstItem;
	HxList_t				*pstListItem;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstListItem = HLIB_LIST_FindEx (pstList->pstHeadItem, (void *)szKey, _hxtoken_urlnew_compMainItemByKey);
	pstItem = (_hxtoken_URLNEW_MainItem_t *)HLIB_LIST_Data (pstListItem);
	if ((NULL == pstItem) || (NULL == pstItem->stMainItem.szValue))
	{
		return ERR_FAIL;
	}

	*pszValue = pstItem->stMainItem.szValue;
	return ERR_OK;
}

HERROR INT_HxToken_URLNEW_FindSubItemKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR *szSubKey, HCHAR **pszValue)
{
	_hxtoken_URLNEW_List_t 	*pstList = (_hxtoken_URLNEW_List_t *)hHandle;
	_hxtoken_URLNEW_MainItem_t	*pstMainItem;
	_hxtoken_URLNEW_SubItem_t	*pstSubItem;
	HxList_t				*pstMainList, *pstSubList;

	if (NULL == pstList)
	{
		return ERR_FAIL;
	}

	pstMainList = HLIB_LIST_FindEx (pstList->pstHeadItem, (void *)szKey, _hxtoken_urlnew_compMainItemByKey);
	pstMainItem = (_hxtoken_URLNEW_MainItem_t *)HLIB_LIST_Data (pstMainList);
	if (NULL == pstMainItem)
	{
		return ERR_FAIL;
	}

	pstSubList = HLIB_LIST_FindEx (pstMainItem->pstHeadSub, (void *)szSubKey, _hxtoken_urlnew_compSubItemByKey);
	pstSubItem = (_hxtoken_URLNEW_SubItem_t *)HLIB_LIST_Data (pstSubList);
	if (NULL == pstSubItem)
	{
		return ERR_FAIL;
	}

	*pszValue = pstSubItem->szValue;
	return ERR_OK;
}

