/*
*		file : oapi_manager_parser.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <htype.h>
#include <hlib.h>

#include "_token_internal.h"




HERROR INT_HxToken_Str2Hex (const HCHAR *s, HINT32 *pnValue)
{
	HINT32		 nVal = 0;

	if ('\0' == *s)					{ return ERR_FAIL; }

	while ('\0' != *s)
	{
		if (('0' <= *s) && ('9' >= *s))
		{
			nVal = (nVal << 4) + (HINT32)(*s - '0');
		}
		else if (('A' <= *s) && ('F' >= *s))
		{
			nVal = (nVal << 4) + (HINT32)(*s - 'A' + 10);
		}
		else if (('a' <= *s) && ('f' >= *s))
		{
			nVal = (nVal << 4) + (HINT32)(*s - 'a' + 10);
		}
		else
		{
			return ERR_FAIL;
		}

		s ++;
	}

	*pnValue = nVal;
	return ERR_OK;
}

HERROR INT_HxToken_Str2Int (const HCHAR *s, HINT32 *pnValue)
{
	HINT32	ne = 1;
	HINT32	n = 0;

	if (('0' == s[0]) && (('x' == s[1]) || ('X' == s[1])))
	{
		return INT_HxToken_Str2Hex (s + 2, pnValue);
	}

	if ('+' == *s)
	{
		ne = 1;
		s ++;
	}
	else if ('-' == *s)
	{
		ne = -1;
		s ++;
	}

	if ('\0' == *s)
	{
		return ERR_FAIL;
	}

	while ('\0' != *s)
	{
		if (('0' <= *s) && ('9' >= *s))
		{
			n = (n * 10) + (HINT32)(*s - '0');
		}
		else
		{
			return ERR_FAIL;
		}

		s ++;
	}

	*pnValue = (n * ne);
	return ERR_OK;
}

HERROR INT_HxToken_Str2Float (const HCHAR *s, float *pfValue)
{
	HBOOL			 bPointFound = FALSE;
	HINT32			 nPlusMinus = 1;
	HINT32			 nValue = 0, nPointUpper = 0, nPointLower = 0, nPointDigit = 1;

	if ('+' == *s)
	{
		nPlusMinus = 1;
		s ++;
	}
	else if ('-' == *s)
	{
		nPlusMinus = -1;
		s ++;
	}

	if ('\0' == *s)
	{
		return ERR_FAIL;
	}

	nValue = 0;
	while ('\0' != *s)
	{
		if ('.' == *s)
		{
			if (TRUE == bPointFound)
			{
				return ERR_FAIL;
			}

			bPointFound = TRUE;
			nPointUpper = nValue;
			nValue = 0;
		}
		else if (('0' <= *s) && ('9' >= *s))
		{
			nValue = (nValue * 10) + (HINT32)(*s - '0');
			if (TRUE == bPointFound)
			{
				nPointDigit *= 10;
			}
		}
		else
		{
			return ERR_FAIL;
		}

		s ++;
	}

	if (TRUE == bPointFound)
	{
		nPointLower = nValue;
	}
	else
	{
		nPointUpper = nValue;
	}

	*pfValue  = (float)nPointUpper + ((float)nPointLower / (float)nPointDigit);
	*pfValue *= (float)nPlusMinus;

	return ERR_OK;
}

HCHAR *INT_HxToken_SkipTokens (HCHAR *szString, HCHAR **pszToken, HUINT32 ulTokenNum)
{
	HUINT32			 ulCnt;

	while (1)
	{
		if ('\0' == *szString)
		{
			return szString;
		}

		for (ulCnt = 0; ulCnt < ulTokenNum; ulCnt++)
		{
			if (HxSTD_StrNCmp (szString, pszToken[ulCnt], HxSTD_StrLen (pszToken[ulCnt])) == 0)
			{
				break;
			}
		}

		if (ulCnt == ulTokenNum)
		{
			return szString;
		}

		szString ++;
	}

	return szString;
}

HCHAR *INT_HxToken_FindNextTokenPos (HCHAR *szString, HCHAR *szEndString, const HCHAR **pszToken, HUINT32 ulTokenNum, HCHAR **pszTokenFound)
{
	HUINT32			 ulCnt;

	while (1)
	{
		if ('\0' == *szString)
		{
			if (pszTokenFound != NULL)			{ *pszTokenFound = NULL; };
			return szString;
		}
		else if ((NULL != szEndString) && (szString > szEndString))
		{
			if (pszTokenFound != NULL)			{ *pszTokenFound = NULL; };
			return szString;
		}
		else if ('"' == *szString)		// "" 이 붙으면 그 안에 있는 것은 하나의 Token으로 간주해야 한다.
		{
			HCHAR			*szQuotaStart;
			HUINT32			 ulQuotaLen;

			szQuotaStart = szString;

			szString ++;
			ulQuotaLen = 1;

			while (1)
			{
				if ('\0' == *szString)
				{
					if (pszTokenFound != NULL)			{ *pszTokenFound = NULL; };
					return szString;
				}
				else if ((NULL != szEndString) && (szString > szEndString))
				{
					if (pszTokenFound != NULL)			{ *pszTokenFound = NULL; };
					return szString;
				}
				else if ('"' == *szString)
				{
					szString ++;
					ulQuotaLen ++;

					if (ulQuotaLen > 2)
					{
						for (ulCnt = 0; ulCnt < ulTokenNum; ulCnt++)
						{
							if ((HxSTD_StrLen (pszToken[ulCnt]) == (ulQuotaLen - 2)) &&
								(HxSTD_StrNCmp (szQuotaStart + 1, pszToken[ulCnt], HxSTD_StrLen (pszToken[ulCnt])) == 0))
							{
								if (pszTokenFound != NULL)			{ *pszTokenFound = (HCHAR *)pszToken[ulCnt]; }
								return szString;
							}
						}
					}

					break;
				}
				else
				{
					szString ++;
					ulQuotaLen ++;
				}
			}
		}
		else
		{
			for (ulCnt = 0; ulCnt < ulTokenNum; ulCnt++)
			{
				if (HxSTD_StrNCmp (szString, pszToken[ulCnt], HxSTD_StrLen (pszToken[ulCnt])) == 0)
				{
					if (pszTokenFound != NULL)			{ *pszTokenFound = (HCHAR *)pszToken[ulCnt]; }
					return szString;
				}
			}

			szString ++;
		}
	}

	return szString;
}

HCHAR *INT_HxToken_FindLastTokenPos (HCHAR *szString, HCHAR *szEndString, const HCHAR **pszToken, HUINT32 ulTokenNum, HCHAR **pszTokenFound)
{
	HCHAR			*szCurrChar;
	HUINT32			 ulCnt;

	if (NULL == szEndString)
	{
		szEndString = szString + HxSTD_StrLen (szString);
	}

	szCurrChar = szEndString;

	while (szCurrChar >= szString)
	{
		if ('"' == *szCurrChar)		// "" 이 붙으면 그 안에 있는 것은 하나의 Token으로 간주해야 한다.
		{
			HUINT32			 ulQuotaLen;

			szCurrChar --;
			ulQuotaLen = 1;

			while (1)
			{
				if (szCurrChar < szString)
				{
					if (pszTokenFound != NULL)			{ *pszTokenFound = NULL; };
					return szEndString;
				}
				else if ('"' == *szCurrChar)
				{
					HCHAR			*szQuotaStart = szCurrChar;
					szCurrChar --;
					ulQuotaLen ++;

					if (ulQuotaLen > 2)
					{
						for (ulCnt = 0; ulCnt < ulTokenNum; ulCnt++)
						{
							if ((HxSTD_StrLen (pszToken[ulCnt]) == (ulQuotaLen - 2)) &&
								(HxSTD_StrNCmp (szQuotaStart + 1, pszToken[ulCnt], HxSTD_StrLen (pszToken[ulCnt])) == 0))
							{
								if (pszTokenFound != NULL)			{ *pszTokenFound = (HCHAR *)pszToken[ulCnt]; }
								return szQuotaStart + 1;
							}
						}
					}

					break;
				}
				else
				{
					ulQuotaLen ++;
				}
			}
		}
		else
		{
			for (ulCnt = 0; ulCnt < ulTokenNum; ulCnt++)
			{
				if (HxSTD_StrNCmp (szCurrChar, pszToken[ulCnt], HxSTD_StrLen (pszToken[ulCnt])) == 0)
				{
					if (pszTokenFound != NULL)			{ *pszTokenFound = (HCHAR *)pszToken[ulCnt]; }
					return szCurrChar;
				}
			}

			szCurrChar --;
		}
	}

	return szEndString;
}


HERROR INT_HxToken_RemoveTokens (HCHAR *szString, const HCHAR **pszToken, HUINT32 ulTokenNum)
{
	HUINT32			 ulCount;
	HINT32			 nSrcPos, nDstPos;
	HINT32			 nSrcLength;
	HBOOL			 bQuota = FALSE;

	nSrcPos		= 0;
	nDstPos		= 0;
	nSrcLength	= HxSTD_StrLen (szString);

	while (nSrcPos < nSrcLength)
	{
		if ('"' == szString[nSrcPos])
		{
			bQuota = (TRUE != bQuota) ? TRUE : FALSE;
		}
		else if (FALSE == bQuota)
		{
			for (ulCount = 0; ulCount < ulTokenNum; ulCount ++)
			{
				if (HxSTD_StrNCmp (szString + nSrcPos, pszToken[ulCount], HxSTD_StrLen (pszToken[ulCount])) == 0)
				{
					break;
				}
			}

			if (ulCount < ulTokenNum)
			{
				nSrcPos += HxSTD_StrLen (pszToken[ulCount]);
				continue;
			}
		}
		// bQuota == TRUE 일 때에는 구분 없이 무조건 넣어야 한다.

		if (nDstPos != nSrcPos)
		{
			szString[nDstPos] = szString[nSrcPos];
		}

		nDstPos ++;
		nSrcPos ++;
	}

	szString[nDstPos] = '\0';
	return ERR_OK;
}

HERROR INT_HxToken_CopyString (HCHAR *szStart, HCHAR *szEnd, HCHAR *szDst, HUINT32 ulDstMax)
{
	HUINT32			 ulCount;

	if ((NULL == szStart) || (NULL == szEnd) || (NULL == szDst) || (0 == ulDstMax) ||
		(szEnd < szStart))
	{
		return ERR_FAIL;
	}

	// Quotation으로 묶인 것은 떼어내서 저장한다.
	if (('"' == *szStart) && ('"' == *szEnd))
	{
		szStart ++;
		szEnd --;
	}

	for (ulCount = 0; ('\0' != szStart) && (szStart <= szEnd) && (ulCount < ulDstMax); ulCount++, szStart++, szDst++)
	{
		*szDst = *szStart;
	}

	if (ulCount < ulDstMax)
	{
		*szDst = '\0';
	}

	return ERR_OK;
}

#if 0
HERROR INT_HxToken_GetToken (HCHAR *szString, HCHAR *szToken, HUINT32 ulTokenSize, HCHAR **pszNextString, const HCHAR **pszDelim, const HCHAR **pszTokenTable)
{
	HUINT32			 ulTokenLen;
	HUINT32			 ulDelimNum = 0, ulTokenNum = 0;
	HCHAR			*szCurr;
	HCHAR			*szNextPos_Delim = NULL, *szNextPos_SpTok = NULL, *szNextToken = NULL;

	if ((NULL == szString) || ('\0' == szString[0]) || (NULL == szToken) || (0 == ulTokenSize) || (NULL == pszDelim))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (szToken, 0, ulTokenSize);

	// Check Delimiter
	for (ulDelimNum = 0; pszDelim[ulDelimNum] != NULL; ulDelimNum++)
	{
		if (pszDelim[ulDelimNum][0] == '\0')
		{
			break;
		}
	}

	if (ulDelimNum == 0)
	{
		return ERR_FAIL;
	}

	// Token Table (Delimiter로 나뉘지 않아도 이 Token들은 우선적으로 string match가 되면 처리되어야 한다.
	if (pszTokenTable != NULL)
	{
		for (ulTokenNum = 0; pszTokenTable[ulTokenNum] != NULL; ulTokenNum++)
		{
			if (pszTokenTable[ulTokenNum][0] == '\0')
			{
				break;
			}
		}
	}

	// 1. 앞의 delimiter들이 있으면 제거
	szCurr = INT_HxToken_SkipTokens (szString, pszDelim, ulDelimNum);
	if (*szCurr == '\0')
	{
		return ERR_FAIL;
	}

	// 2. 다음 Delimitor나 다음 Token을 만나는 위치까지를 Token으로 잡는다.
	szNextPos_Delim = INT_HxToken_FindNextTokenPos (szCurr, pszDelim, ulDelimNum, NULL);
	szNextPos_SpTok = INT_HxToken_FindNextTokenPos (szCurr, pszTokenTable, ulTokenNum, &szNextToken);

	// 맨 앞이 Token Table의 Token에 해당하는 string이 있다
	if ((szCurr == szNextPos_SpTok) && (szNextToken != NULL))
	{
		VK_strncpy (szToken, szNextToken, ulTokenSize);
		szToken[ulTokenSize - 1] = '\0';

		szCurr += HxSTD_StrLen (szNextToken);
	}
	// 다음 Delimitor나 다음 Token을 만나는 위치까지를 Token으로 잡는다.
	else if ((szCurr < szNextPos_Delim) && (szCurr < szNextPos_SpTok))
	{
		if ('"' == *szCurr)
		{
			HUINT32			 ulRealTokenLen;		// 실제의 Token Size : "xxxx" 라고 하면 ulTokenLen은 6이고 ulRealTokenLen 은 4이다.

			ulTokenLen = 1;
			while ('"' != szCurr[ulTokenLen])
			{
				if ('\0' == szCurr[ulTokenLen])
				{
					return ERR_FAIL;
				}

				ulTokenLen ++;
			}

			ulTokenLen ++;

			ulRealTokenLen = ulTokenLen - 2;
			if (ulRealTokenLen > 0)
			{
				if (ulRealTokenLen >= ulTokenSize)
				{
					VK_memcpy (szToken, szCurr + 1, ulTokenSize);
					szToken[ulTokenSize - 1] = '\0';
				}
				else
				{
					VK_memcpy (szToken, szCurr + 1, ulRealTokenLen);
					szToken[ulRealTokenLen] = '\0';
				}
			}
			else
			{
				szToken[0] = '\0';
			}
		}
		else
		{
			ulTokenLen = (szNextPos_Delim > szNextPos_SpTok) ? (szNextPos_SpTok - szCurr) : (szNextPos_Delim - szCurr);
			if (ulTokenLen >= ulTokenSize)
			{
				VK_memcpy (szToken, szCurr, ulTokenSize);
				szToken[ulTokenSize - 1] = '\0';
			}
			else
			{
				VK_memcpy (szToken, szCurr, ulTokenLen);
				szToken[ulTokenLen] = '\0';
			}
		}

		szCurr += ulTokenLen;
	}
	else
	{
		return ERR_FAIL;
	}

	if (pszNextString != NULL)
	{
		*pszNextString = szCurr;
	}

	return ERR_OK;
}
#endif

