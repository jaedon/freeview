/*************************************************************
 * @file		hxtoken.h
 * @date		2011/04/28
 * @author	jinh@humaxdigital.com
 * @brief		HUMAX common library header, related with command debugger
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef ___HLIB_TOKEN_H___
#define ___HLIB_TOKEN_H___

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define HxTOKEN_KEY_SIZE							32
#define HxTOKEN_VALUE_SIZE							256
#define HxTOKEN_RULE_SIZE							256

#define HxTOKEN_MAX_RULE							256

#define HxTOKEN_CONVRULE_INTEGER					"_+###ATOI###+_"
#define HxTOKEN_CONVRULE_DEFAULT					"_+###DEFAULT###+_"

#define HxTOKEN_CONVRULE_END						{ "", -1 }				// End Of Rule


/*
	URL Parsing ½Ã ±âº» Token
*/
#define HxTOKEN_URL_URLTYPE							"url_type"
#define HxTOKEN_URL_DOMAIN							"domain"
#define HxTOKEN_URL_PATH							"path"
#define HxTOKEN_URL_FILE							"file"

#define HxTOKEN_URLTYPE_HTTP						"http"
#define HxTOKEN_URLTYPE_FILE						"file"
#define HxTOKEN_URLTYPE_HDVB						"HDVB"

#define HxTOKEN_HDVB_TOKEN_TUNETYPE					"type"
#define HxTOKEN_HDVB_TOKEN_TRIPLEID					"tripleid"
#define HxTOKEN_HDVB_TOKEN_SVCNO					"svcno"
#define HxTOKEN_HDVB_TOKEN_SVCUID					"uid"
#define HxTOKEN_HDVB_TOKEN_ONID						"onid"
#define HxTOKEN_HDVB_TOKEN_TSID						"tsid"
#define HxTOKEN_HDVB_TOKEN_SVCID					"svcid"



/*******************************************************************/
/********************      Typedef              ********************/
/*******************************************************************/

typedef struct
{
	HCHAR		 szConvLaw[HxTOKEN_KEY_SIZE];
	HINT32		 nValue;
} HxTOKEN_ConvRule_t;

typedef enum
{
	eTokenStrType_PLANE,
	eTokenStrType_ARGUMENT,
	eTokenStrType_URL,
	eTokenStrType_URL_NEW,

	eParseStrType_NONE,
	eParseStrType_EndOfType
} HxTOKEN_StringType_e;


typedef void *HxTOKEN_Handle_t;



/*******************************************************************/
/********************      Functions  ********************/
/*******************************************************************/

extern HERROR	HLIB_TOKEN_Str2Int (const HCHAR *s, HINT32 *pnValue);
extern HERROR	HLIB_TOKEN_Str2IntArray (const HCHAR *s, HINT32 *pnValueArray, HUINT32 ulArrayLen, HUINT32 *pulIntNum);
extern HERROR	HLIB_TOKEN_Str2Float (const HCHAR *s, float *pfValue);
extern HERROR	HLIB_TOKEN_Parse (HxTOKEN_StringType_e eParseType, HCHAR *szString, HxTOKEN_Handle_t *phArgHandle);
extern HERROR	HLIB_TOKEN_FreeHandle (HxTOKEN_Handle_t hArgHandle);
extern HERROR	HLIB_TOKEN_ResetHandleCursor (HxTOKEN_Handle_t hArgHandle);
extern HUINT32	HLIB_TOKEN_CountMainKey (HxTOKEN_Handle_t hArgHandle);
extern HERROR	HLIB_TOKEN_GetMainKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HCHAR *szKey, HCHAR *szValue);
extern HUINT32	HLIB_TOKEN_CountSubKey (HxTOKEN_Handle_t hArgHandle, HUINT32 uIndex);
extern HERROR	HLIB_TOKEN_GetSubKeyAndValue (HxTOKEN_Handle_t hArgHandle, HUINT32 ulIndex, HUINT32 ulSubIndex, HCHAR *szKey, HCHAR *szValue);
extern HBOOL	HLIB_TOKEN_FindKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HBOOL bValueExist);
extern HCHAR*	HLIB_TOKEN_GetValueString (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey);
extern HINT32	HLIB_TOKEN_GetValueInt (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey);
extern HINT32	HLIB_TOKEN_GetValueIntEx (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, const HxTOKEN_ConvRule_t *astLawList);
extern HBOOL	HLIB_TOKEN_FindSubItemKey (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey, HBOOL bValueExist);
extern HCHAR*	HLIB_TOKEN_GetSubItemString (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey);
extern HINT32	HLIB_TOKEN_GetSubItemInt (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey);
extern HINT32	HLIB_TOKEN_GetSubItemIntEx (HxTOKEN_Handle_t hArgHandle, HCHAR *szKey, HCHAR *szSubKey, const HxTOKEN_ConvRule_t *astLawList);

#endif /* ___HLIB_TOKEN_H___ */

