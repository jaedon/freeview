/**
	@file	hxtoken.c
	@brief	\n

	Description: 					\n
	Module:						\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
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

extern HERROR	INT_HxToken_URLNEW_Parse (HCHAR *szString, HxTOKEN_Handle_t *phHandle);
extern void		INT_HxToken_URLNEW_Free (HxTOKEN_Handle_t hHandle);
extern HERROR	INT_HxToken_URLNEW_ResetHandleCursor (HxTOKEN_Handle_t hHandle);
extern HERROR	INT_HxToken_URLNEW_CountMainKey (HxTOKEN_Handle_t hArgHandle);
extern HERROR	INT_HxToken_URLNEW_GetKeyAndValue (HxTOKEN_Handle_t hHandle, HUINT32 ulIndex, HCHAR *szKey, HCHAR *szValue);
extern HUINT32	INT_HxToken_URLNEW_CountSubKey (HxTOKEN_Handle_t hHandle, HUINT32 ulIndex);
extern HERROR	INT_HxToken_URLNEW_GetSubKeyAndValue (HxTOKEN_Handle_t hHandle, HUINT32 ulIndex, HUINT32 ulSubIndex, HCHAR *szKey, HCHAR *szValue);
extern HERROR	INT_HxToken_URLNEW_FindKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR **pszValue);
extern HERROR	INT_HxToken_URLNEW_FindSubItemKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR *szSubKey, HCHAR **pszValue);

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HxTOKEN_StringType_e	 eParseType;
	HUINT8					 aucReserved[8];
} _hxtoken_Templete_t;


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

STATIC HxTOKEN_StringType_e _hxtoken_GetStringType (HxTOKEN_Handle_t hArgHandle)
{
	HxTOKEN_StringType_e	 eStrType = eParseStrType_NONE;
	_hxtoken_Templete_t		*pstTemplete = (_hxtoken_Templete_t *)hArgHandle;

	if (NULL != pstTemplete)
	{
		eStrType = pstTemplete->eParseType;
	}

	return eStrType;
}


#define _____GLOBAL_FUNCTIONS_____

HERROR HLIB_TOKEN_Str2Int (const HCHAR *s, HINT32 *pnValue)
{
	if ((NULL == s) || (NULL == pnValue))					{ return ERR_FAIL; }

	return INT_HxToken_Str2Int (s, pnValue);
}

HERROR HLIB_TOKEN_Str2IntArray (const HCHAR *s, HINT32 *pnValueArray, HUINT32 ulArrayLen, HUINT32 *pulIntNum)
{
	HBOOL			 bNumFound = FALSE, bHexFound = FALSE;
	HINT32			 nValue;
	HUINT32			 ulIntCnt = 0, ulStrPos=0;
	HCHAR			 szNumbStr[16];
	HERROR			 hErr;

	if ((NULL == s) || (NULL == pnValueArray) || (0 == ulArrayLen))
	{
		return ERR_FAIL;
	}

	while ('\0' != *s)
	{
		if (TRUE == bNumFound)
		{
			if (TRUE == bHexFound)
			{
				bNumFound = (isxdigit (*s)) ? TRUE : FALSE;
			}
			else
			{
				bNumFound = (isdigit (*s)) ? TRUE : FALSE;
			}

			if (TRUE == bNumFound)
			{
				szNumbStr[ulStrPos] = *s;
				s++;
				ulStrPos++;
			}
			else
			{
				hErr = INT_HxToken_Str2Int (szNumbStr, &nValue);
				if ((ERR_OK == hErr) && (ulIntCnt < ulArrayLen))
				{
					pnValueArray[ulIntCnt] = nValue;
					ulIntCnt ++;
				}

				s++;
			}
		}
		else
		{
			if (('0' == s[0]) && (('x' == s[1]) || ('X' == s[1])))
			{
				HxSTD_MemSet (szNumbStr, 0, 16);

				szNumbStr[0] = '0';
				szNumbStr[1] = 'x';
				ulStrPos = 2;

				bNumFound = TRUE;
				bHexFound = TRUE;

				s += 2;
			}
			else if (isdigit (*s))
			{
				HxSTD_MemSet (szNumbStr, 0, 16);

				szNumbStr[0] = *s;
				ulStrPos = 1;

				bNumFound = TRUE;
				bHexFound = FALSE;

				s++;
			}
			else
			{
				s++;
			}
		}
	}

	if (TRUE == bNumFound)
	{
		hErr = INT_HxToken_Str2Int (szNumbStr, &nValue);
		if ((ERR_OK == hErr) && (ulIntCnt < ulArrayLen))
		{
			pnValueArray[ulIntCnt] = nValue;
			ulIntCnt ++;
		}
	}

	if (NULL != pulIntNum)				{ *pulIntNum = ulIntCnt; }

	return (ulIntCnt > 0) ? ERR_OK : ERR_FAIL;
}


HERROR HLIB_TOKEN_Str2Float (const HCHAR *s, float *pfValue)
{
	if ((NULL == s) || (NULL == pfValue))					{ return ERR_FAIL; }

	return INT_HxToken_Str2Float (s, pfValue);
}

HERROR HLIB_TOKEN_Parse (HxTOKEN_StringType_e eParseType, HCHAR *szString, HxTOKEN_Handle_t *phArgHandle)
{
	HERROR			 hErr;

	if ((NULL == szString) || (NULL == phArgHandle))
	{
		if (NULL != phArgHandle)			{ *phArgHandle = NULL; }
		return ERR_FAIL;
	}

	*phArgHandle = NULL;

	switch (eParseType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_Parse (szString, phArgHandle);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_Parse (szString, phArgHandle);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_Parse (szString, phArgHandle);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR HLIB_TOKEN_FreeHandle (HxTOKEN_Handle_t hArgHandle)
{
	HxTOKEN_StringType_e	 eType;

	if (NULL == hArgHandle)
	{
		return ERR_FAIL;
	}

	eType = _hxtoken_GetStringType (hArgHandle);

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		INT_HxToken_ARG_Free (hArgHandle);
		break;

	case eTokenStrType_URL:
		INT_HxToken_URL_Free (hArgHandle);
		break;

	case eTokenStrType_URL_NEW:
		INT_HxToken_URLNEW_Free (hArgHandle);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR HLIB_TOKEN_ResetHandleCursor (HxTOKEN_Handle_t hArgHandle)
{
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_ResetHandleCursor (hArgHandle);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_ResetHandleCursor (hArgHandle);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_ResetHandleCursor (hArgHandle);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HUINT32 HLIB_TOKEN_CountMainKey (HxTOKEN_Handle_t hArgHandle)
{
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HUINT32					 ulCount;

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		ulCount = INT_HxToken_ARG_CountMainKey (hArgHandle);
		break;

	case eTokenStrType_URL_NEW:
		ulCount = INT_HxToken_URLNEW_CountMainKey (hArgHandle);
		break;

	default:
		ulCount = 0;
		break;
	}

	return ulCount;
}

HERROR HLIB_TOKEN_GetMainKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HCHAR *szKey, HCHAR *szValue)
{
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || (NULL == szValue))			{ return ERR_FAIL; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_GetKeyAndValue (hArgHandle, ulIndex, szKey, szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_GetKeyAndValue (hArgHandle, ulIndex, szKey, szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HUINT32 HLIB_TOKEN_CountSubKey (HxTOKEN_Handle_t hArgHandle, HUINT32 ulMainIdx)
{
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HUINT32					 ulCount;

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		ulCount = INT_HxToken_ARG_CountSubKey (hArgHandle, ulMainIdx);
		break;

	case eTokenStrType_URL_NEW:
		ulCount = INT_HxToken_URLNEW_CountSubKey (hArgHandle, ulMainIdx);
		break;

	default:
		ulCount = 0;
		break;
	}

	return ulCount;
}

HERROR HLIB_TOKEN_GetSubKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HUINT32 ulSubIndex, HCHAR *szKey, HCHAR *szValue)
{
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || (NULL == szValue))			{ return ERR_FAIL; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_GetSubKeyAndValue (hArgHandle, ulIndex, ulSubIndex, szKey, szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_GetSubKeyAndValue (hArgHandle, ulIndex, ulSubIndex, szKey, szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HBOOL HLIB_TOKEN_FindKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HBOOL bValueExist)
{
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || ('\0' == szKey[0]))			{ return FALSE; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindKey (hArgHandle, szKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return FALSE;
	}

	if ((TRUE == bValueExist) && ('\0' == szValue[0]))
	{
		return FALSE;
	}

	return TRUE;
}

HCHAR *HLIB_TOKEN_GetValueString (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey)
{
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || ('\0' == szKey[0]))			{ return NULL; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindKey (hArgHandle, szKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return NULL;
	}

	return szValue;
}

HINT32 HLIB_TOKEN_GetValueInt (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey)
{
	HINT32					 nValue = 0;
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || ('\0' == szKey[0]))			{ return 0; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindKey (hArgHandle, szKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return 0;
	}

	INT_HxToken_Str2Int (szValue, &nValue);
	return nValue;
}

HINT32 HLIB_TOKEN_GetValueIntEx (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, const HxTOKEN_ConvRule_t *astLawList)
{
	HINT32					 nCount, nValue = 0;
	HINT32					 nRuleNum, nDefaultVal = 0;
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	// No Rule Table
	if (NULL == astLawList)
	{
		return HLIB_TOKEN_GetValueInt (hArgHandle, szKey);
	}

	for (nRuleNum = 0; nRuleNum < HxTOKEN_MAX_RULE; nRuleNum ++)
	{
		if ('\0' == astLawList[nRuleNum].szConvLaw[0])
		{
			break;
		}
	}

	// No Rule Table
	if (0 == nRuleNum)
	{
		return HLIB_TOKEN_GetValueInt (hArgHandle, szKey);
	}

	// Default Setting Value :
	for (nCount = 0; nCount < nRuleNum; nCount++)
	{
		if (0 == HxSTD_StrCmp (astLawList[nCount].szConvLaw, HxTOKEN_CONVRULE_DEFAULT))
		{
			nDefaultVal = astLawList[nCount].nValue;
			break;
		}
	}

	if ((NULL == szKey) || ('\0' == szKey[0]))			{ return nDefaultVal; }

	// Find the Main Key
	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindKey (hArgHandle, szKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindKey (hArgHandle, szKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return nDefaultVal;
	}

	for (nCount = 0; nCount < nRuleNum; nCount++)
	{
		// 1. Law의 String과 동일한 szValue면 Law에서 지정한 nValue로 보내준다.
		if (0 == HxSTD_StrCmp (szValue, astLawList[nCount].szConvLaw))
		{
			return astLawList[nCount].nValue;
		}
		// 2. Law가 HxTOKEN_CONVRULE_INTEGER (atoi) : 면 숫자로 변환시켜서 보낸다
		// 다만 이 Law의 경우, 숫자가 아닌 경우엔 적용되지 않는다.
		else if (0 == HxSTD_StrCmp (astLawList[nCount].szConvLaw, HxTOKEN_CONVRULE_INTEGER))
		{
			hErr = INT_HxToken_Str2Int (szValue, &nValue);
			if (ERR_OK == hErr)
			{
				return nValue;
			}
		}
		// ...차후 Special Law가 추가되면 여기에 넣도록 한다.
	}

	// Rule에 아무런 조건에 맞지 않았다 : Default Value
	return nDefaultVal;
}

HBOOL HLIB_TOKEN_FindSubItemKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey, HBOOL bValueExist)
{
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || ('\0' == szKey[0]))				{ return FALSE; }
	if ((NULL == szSubKey) || ('\0' == szSubKey[0]))		{ return FALSE; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return FALSE;
	}

	if ((TRUE == bValueExist) && ('\0' == szValue[0]))
	{
		return FALSE;
	}

	return TRUE;
}

HCHAR *HLIB_TOKEN_GetSubItemString (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey)
{
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || ('\0' == szKey[0]))				{ return NULL; }
	if ((NULL == szSubKey) || ('\0' == szSubKey[0]))		{ return NULL; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return NULL;
	}

	return szValue;
}

HINT32 HLIB_TOKEN_GetSubItemInt (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey)
{
	HINT32					 nValue = 0;
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	if ((NULL == szKey) || ('\0' == szKey[0]))				{ return 0; }
	if ((NULL == szSubKey) || ('\0' == szSubKey[0]))		{ return 0; }

	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return 0;
	}

	INT_HxToken_Str2Int (szValue, &nValue);
	return nValue;
}

HINT32 HLIB_TOKEN_GetSubItemIntEx (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey, const HxTOKEN_ConvRule_t *astLawList)
{
	HINT32					 nCount, nValue = 0;
	HINT32					 nRuleNum, nDefaultVal = 0;
	HCHAR					*szValue = NULL;
	HxTOKEN_StringType_e	 eType = _hxtoken_GetStringType (hArgHandle);
	HERROR					 hErr;

	// No Rule Table
	if (NULL == astLawList)
	{
		return HLIB_TOKEN_GetSubItemInt (hArgHandle, szKey, szSubKey);
	}

	for (nRuleNum = 0; nRuleNum < HxTOKEN_MAX_RULE; nRuleNum ++)
	{
		if ('\0' == astLawList[nRuleNum].szConvLaw[0])
		{
			break;
		}
	}

	// No Rule Table
	if (0 == nRuleNum)
	{
		return HLIB_TOKEN_GetSubItemInt (hArgHandle, szKey, szSubKey);
	}

	// Default Setting Value :
	for (nCount = 0; nCount < nRuleNum; nCount++)
	{
		if (0 == HxSTD_StrCmp (astLawList[nCount].szConvLaw, HxTOKEN_CONVRULE_DEFAULT))
		{
			nDefaultVal = astLawList[nCount].nValue;
			break;
		}
	}

	if ((NULL == szKey) || ('\0' == szKey[0]))				{ return nDefaultVal; }
	if ((NULL == szSubKey) || ('\0' == szSubKey[0]))		{ return nDefaultVal; }

	// Find the Main Key
	switch (eType)
	{
	case eTokenStrType_ARGUMENT:
		hErr = INT_HxToken_ARG_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL:
		hErr = INT_HxToken_URL_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;

	case eTokenStrType_URL_NEW:
		hErr = INT_HxToken_URLNEW_FindSubItemKey (hArgHandle, szKey, szSubKey, &szValue);
		break;


	default:
		hErr = ERR_FAIL;
		break;
	}

	if ((ERR_OK != hErr) || (NULL == szValue))
	{
		return nDefaultVal;
	}

	for (nCount = 0; nCount < nRuleNum; nCount++)
	{
		// 1. Law의 String과 동일한 szValue면 Law에서 지정한 nValue로 보내준다.
		if (0 == HxSTD_StrCmp (szValue, astLawList[nCount].szConvLaw))
		{
			return astLawList[nCount].nValue;
		}
		// 2. Law가 HxTOKEN_CONVRULE_INTEGER (atoi) : 면 숫자로 변환시켜서 보낸다
		// 다만 이 Law의 경우, 숫자가 아닌 경우엔 적용되지 않는다.
		else if (0 == HxSTD_StrCmp (astLawList[nCount].szConvLaw, HxTOKEN_CONVRULE_INTEGER))
		{
			hErr = INT_HxToken_Str2Int (szValue, &nValue);
			if (ERR_OK == hErr)
			{
				return nValue;
			}
		}
		// ...차후 Special Law가 추가되면 여기에 넣도록 한다.
	}

	// Rule에 아무런 조건에 맞지 않았다 : Default Value
	return nDefaultVal;
}

