/*************************************************************
 * @file		_token_internal.h
 * @date		2011/04/28
 * @author	jinh@humaxdigital.com
 * @brief		HUMAX common library header, related with command debugger
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef ___TOKEN_INTERNAL_H___
#define ___TOKEN_INTERNAL_H___

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef              ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions  ********************/
/*******************************************************************/

// _token_util.c
extern HERROR	INT_HxToken_Str2Hex (const HCHAR *s, HINT32 *pnValue);
extern HERROR	INT_HxToken_Str2Int (const HCHAR *s, HINT32 *pnValue);
extern HERROR	INT_HxToken_Str2Float (const HCHAR *s, float *pfValue);
extern HCHAR*	INT_HxToken_SkipTokens (HCHAR *szString, HCHAR **pszToken, HUINT32 ulTokenNum);
extern HCHAR*	INT_HxToken_FindNextTokenPos (HCHAR *szString, HCHAR *szEndString, const HCHAR **pszToken, HUINT32 ulTokenNum, HCHAR **pszTokenFound);
extern HCHAR*	INT_HxToken_FindLastTokenPos (HCHAR *szString, HCHAR *szEndString, const HCHAR **pszToken, HUINT32 ulTokenNum, HCHAR **pszTokenFound);
extern HERROR	INT_HxToken_RemoveTokens (HCHAR *szString, const HCHAR **pszToken, HUINT32 ulTokenNum);
extern HERROR	INT_HxToken_CopyString (HCHAR *szStart, HCHAR *szEnd, HCHAR *szDst, HUINT32 ulDstMax);
extern HERROR	INT_HxToken_GetToken (HCHAR *szString, HCHAR *szToken, HUINT32 ulTokenSize, HCHAR **pszNextString, const HCHAR **pszDelim, const HCHAR **pszTokenTable);


// _token_arg.c
extern HERROR	INT_HxToken_ARG_Parse (HCHAR *szString, HxTOKEN_Handle_t *phArgHandle);
extern void		INT_HxToken_ARG_Free (HxTOKEN_Handle_t hArgHandle);
extern HERROR	INT_HxToken_ARG_ResetHandleCursor (HxTOKEN_Handle_t hArgHandle);
extern HUINT32	INT_HxToken_ARG_CountMainKey (HxTOKEN_Handle_t hArgHandle);
extern HERROR	INT_HxToken_ARG_GetKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HCHAR *szKey, HCHAR *szValue);
extern HUINT32	INT_HxToken_ARG_CountSubKey (HxTOKEN_Handle_t hArgHandle, HUINT32 ulMainIdx);
extern HERROR	INT_HxToken_ARG_GetSubKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HUINT32 ulSubIndex, HCHAR *szKey, HCHAR *szValue);
extern HERROR	INT_HxToken_ARG_FindKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR **pszValue);
extern HERROR	INT_HxToken_ARG_FindSubItemKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey, HCHAR **pszValue);

// _token_url.c
extern HERROR	INT_HxToken_URL_Parse (HCHAR *szString, HxTOKEN_Handle_t *phHandle);
extern void		INT_HxToken_URL_Free (HxTOKEN_Handle_t hHandle);
extern HERROR	INT_HxToken_URL_ResetHandleCursor (HxTOKEN_Handle_t hHandle);
extern HERROR	INT_HxToken_URL_GetKeyAndValue (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR *szValue);
extern HERROR	INT_HxToken_URL_FindKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR **pszValue);
extern HERROR	INT_HxToken_URL_FindSubItemKey (HxTOKEN_Handle_t hHandle, HCHAR *szKey, HCHAR *szSubKey, HCHAR **pszValue);

#endif /* ___TOKEN_INTERNAL_H___ */

