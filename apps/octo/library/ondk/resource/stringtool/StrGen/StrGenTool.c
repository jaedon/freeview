#include <stdio.h>
#include <stdlib.h>
#include <rxlib.h>
#include <rxdir.h>

#include "StrGenTool.h"

#define STRGEN_DEFAULT_INPUT_UCS2_STRING 	"strgen_str_ucs2.txt"
#define STRGEN_DEFAULT_OUTPUT_UTF8_STRING 	"strgen_str_utf8.txt"
#define STRGEN_DEFAULT_USING_STRING 	"strgen_using_str_id.txt"
#define STRGEN_DEFAULT_ORGANIZE_STRING 	"strgen_organize_str_id.txt"
#define STRGEN_DEFAULT_LANGUAGE_SETTING "strgen_language_setting.txt"

#if defined(WIN32)
#define STRGEN_SRCFILE_PATH			"main/usr/string"
#define STRGEN_DSTFILE_PATH			"main/usr/string"
#define STRGEN_FILENAME_HEADER		"main/usr/string/str_"
#else
#define STRGEN_SRCFILE_PATH			"../../string/model"
#define STRGEN_DSTFILE_PATH			"../../string/strings"
#define STRGEN_FILENAME_HEADER		"../../string/strings/str_"
#endif

#define STRGEN_FILENAME_EXTENTION	".txt"

#define MAX_FILENAME_LENGTH 		512
#define MAX_STRID_LENGTH 			128
#define MAX_STRID_COUNT				1024
#define MAX_LANGUAGE_COUNT			48

typedef struct tagmStrIdItem_t
{
	HBOOL		bIsComment;
	HCHAR		szStrId[MAX_STRID_LENGTH];	/* hash key */
	RxVector_t	*pVectorText;
} mStrIdItem_t;


typedef struct tagmMgr
{
	RxHashTable_t	*pLanguageNumHash;			// mLanguageNum_t
	RxHashTable_t	*pStridHash;					// mStrIdItem_t
	RxHashTable_t	*pUsingStrIdHash;
	//	MHHashTable_t *pGetUsingStrId;

	HUINT8 pSelectLan[MAX_LANGUAGE_COUNT][MAX_STRID_LENGTH];
	HINT32 pSetLanCount;

	RxHashTable_t	*pNoHaveStrId;
	RxHashTable_t	*pNoTransStrId;
} mMgr_t;


typedef struct _mStrValue
{
	HCHAR 		*pszLang;
	RxFile_t	pTargetFile;
} mStrValue_t;


static mMgr_t* s_Mgr;


static HUINT8 *s_strgen_pre_InputUCS2File = NULL;
static HUINT8 *s_strgen_pre_OutputUTF8File = NULL;



static HUINT8 *_strgen_Get_LangValue(HINT8 *str_id, HINT8 *lan);
static HCHAR *_strgen_String_ToLower(HCHAR *arg);
static HCHAR *_strgen_String_ToUpper(HCHAR *arg);
static void	_strgen_StrReplaceChar_Define2StrId(HCHAR *dst, const HCHAR *src);


/*************************************************************************************************************/
#define _____STRGEN_MANAGER___________________________________________________________________________
/*************************************************************************************************************/

static HERROR	_strgen_Manager_New(void)
{

	HERROR hErr = ERR_OK;	
	
	if(NULL == s_Mgr)
	{
		s_Mgr = (mMgr_t *)RxSTD_Alloc(sizeof(mMgr_t));
		StrGenAssert(s_Mgr);
		RxSTD_MemSet(s_Mgr, 0, sizeof(mMgr_t));

		s_Mgr->pSetLanCount = 0;
	}

	return hErr;

}


/*************************************************************************************************************/
#define _____STRGEN_UTIL___________________________________________________________________________
/*************************************************************************************************************/

static HCHAR *_strgen_util_StrNCpy(HUINT8 *dest, HUINT8 *src, HINT32 cnt)
{
	HCHAR *ptr = dest;

	while (*src && (cnt > 0))
	{
		switch(*src)
		{
		case '\"':
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			src++;
			break;

		default:
			*ptr++ = *src++;
			break;
		}

		cnt--;
	}
	*ptr = '\0';

	return dest;
}

static	HINT32	_strgen_util_Cmp(const void *vpSrc1, const void *vpSrc2)
{
	mStrIdItem_t	**ppSrc = (mStrIdItem_t**)vpSrc1;
	mStrIdItem_t	**ppDst = (mStrIdItem_t**)vpSrc2;

	return RxSTD_StrCmp((*ppSrc)->szStrId, (*ppDst)->szStrId);
}


static HUINT8 *	_strgen_util_ToLower(HUINT8 *str, HUINT8 *pucTarget)
{
	HINT32 i=0, length=0;
	HCHAR ch = 0;

	if( str  == NULL || pucTarget == NULL )
		return NULL;

	length = RxSTD_StrLen( (char *)str );

	for( i = 0; i < length; i++ )
	{
		ch = str[i];
		if(('A' <= ch && ch <= 'Z') ||
		   ('0' <= ch && ch <= '9') ||
		   ('a' <= ch && ch <= 'z'))
		{
			pucTarget[i] = tolower(ch);
		}
		else
		{
			pucTarget[i] = ch;
		}
	}

	return str;
}

static HUINT8 *	_strgen_util_ToUpper(HUINT8 *str, HUINT8 *pucTarget)
{
	HINT32 i=0, length=0;
	HCHAR ch = 0;

	if( str  == NULL || pucTarget == NULL )
		return NULL;

	length = RxSTD_StrLen( (char *)str );

	for( i = 0; i < length; i++ )
	{
		ch = str[i];
		if(('A' <= ch && ch <= 'Z') ||
		   ('0' <= ch && ch <= '9') ||
		   ('a' <= ch && ch <= 'z'))
		{
			pucTarget[i] = toupper(ch);
		}
		else
		{
			pucTarget[i] = ch;
		}
	}

	return str;
}


static HUINT8 *	_strgen_skip_Space(HUINT8 *str)
{
	while(*str)
	{
		switch(*str)
		{
		case '\r':
		case '\n':
		case ' ':
		case '\t':
			break;

		default:
			return str;
		}
		str++;
	}

	return str;
}


/*************************************************************************************************************/
#define _____STRGEN_BUFFER___________________________________________________________________________
/*************************************************************************************************************/
#define MAX_CHARS_BUFFER_SIZE		8192


/*************************************************************************************************************/
#define ___________FUNCTION_CONVERT__________________________________________________________________________
/*************************************************************************************************************/


static HERROR _strgen_Convert_UCStoUTF8 (HUINT8 *srcFile, HUINT8 *dstFile)
{
	HINT32 		readRet = 0;
	HINT32		wcount = 0;
	RxFile_t 	pInput = NULL, pOutput = NULL;

	HUINT8 		utf8Buf[MAX_CHARS_BUFFER_SIZE*4] = {0,};
	HUINT8		ucs2Buf[MAX_CHARS_BUFFER_SIZE*2] = {0,};


	pInput = RxFile_Open(srcFile, "rb");
	StrGenAssert(pInput);

	pOutput = RxFile_Open(dstFile, "wb");
	StrGenAssert(pOutput);

	while(RxFile_EOF(pInput) == 0)
	{
		RxSTD_MemSet (ucs2Buf, 0, (MAX_CHARS_BUFFER_SIZE*2));

		readRet = RxFile_Read(pInput, ucs2Buf, 1, MAX_CHARS_BUFFER_SIZE);
		if(readRet <= 0)
		{
			break;
		}

		if(ucs2Buf[0] == 255 && ucs2Buf[1] == 254)	// bom char
		{
			ucs2Buf[0] = 0x20;
			ucs2Buf[1] = 0x00;
		}

		if(readRet < MAX_CHARS_BUFFER_SIZE)
		{
			RxSTD_MemSet (ucs2Buf+readRet, 0,  (MAX_CHARS_BUFFER_SIZE - readRet));
		}

		RxSTD_MemSet (utf8Buf, 0,  (MAX_CHARS_BUFFER_SIZE * 4));
		wcount = RxCP_ConvertUCS2ToCodepage(eRxCP_UNICODE_UTF8, (const HUINT16 *)ucs2Buf, (void *)utf8Buf, (MAX_CHARS_BUFFER_SIZE * 4));

		RxFile_Write(pOutput, utf8Buf, 1, wcount);
	}

	RxFile_Close(pInput);
	RxFile_Close(pOutput);

	return ERR_OK;
}

/*************************************************************************************************************/
#define ___________FUNCTION_STR_PARSE__________________________________________________________________________
/*************************************************************************************************************/

static HINT32 _strgen_StrVector_StrChar(HUINT8 *srcBuf, HCHAR ch)
{
	HINT32		i = 0;

	for(i=0; i < (MAX_CHARS_BUFFER_SIZE * 4); i++)
	{
		if(srcBuf[i] == 0)
		{
			break;
		}
		if (srcBuf[i] < 0x80)
		{
			;
		}
		else if (srcBuf[i] < 0xc2)
		{
			i+=1;
			continue;
		}
		else if (srcBuf[i] < 0xe0)
		{
			i+=1;
			continue;
		}
		else if (srcBuf[i] < 0xf0)
		{
			i+=2;
			continue;
		}
		else if (srcBuf[i] < 0xff)
		{
			i+=3;
			continue;
		}
		else
		{
			continue;
		}

		if(ch == srcBuf[i])
		{
			return i;
		}

	}

	return -1;
}

static HUINT8 *_strgen_StrVector_GetStrId(HUINT8 *dst, HUINT8 *src)
{
	HINT32 index = 0;

	index = _strgen_StrVector_StrChar(src, '\t');
	if(index < 0)
	{
		return NULL;
	}

	RxSTD_StrNCpy(dst, src, index);
	_strgen_util_ToLower(dst, dst);

	return src + index +1;
}

static RxVector_t *_strgen_StrVector_MakeList(RxVector_t	*pVectorText, HUINT8 *string)
{
	HINT32	index = 0;
	HUINT8	*str = string;

	while(1)
	{
		index = _strgen_StrVector_StrChar(str, '\t');
		if(index < 0)
		{
			RxVector_Add(pVectorText, RxSTD_StrDupEx( str, RxSTD_StrLen(str), (void *)RxSTD_Alloc));
			break;
		}
		RxVector_Add(pVectorText, RxSTD_StrDupEx( str, index, (void *)RxSTD_Alloc));

		str+=(index+1);
	}

	return pVectorText;
}

static mStrIdItem_t *_strgen_StrVector_New(HUINT8 *string)
{
//	HCHAR 				*strid;
	void 				*ret = NULL;
	mStrIdItem_t	*pItem = NULL;

	pItem = (mStrIdItem_t *)RxSTD_Alloc(sizeof(mStrIdItem_t));
	StrGenAssert(pItem);

	RxSTD_MemSet(pItem, 0, sizeof(mStrIdItem_t));

	string = _strgen_StrVector_GetStrId(pItem->szStrId, string);
	if(string == NULL)
	{
		RxSTD_Free(pItem);
		return NULL;
	}

	pItem->pVectorText = RxVector_NewEx(NULL, MAX_LANGUAGE_COUNT, NULL, NULL, _strgen_util_Cmp);
	StrGenAssert(pItem->pVectorText);

	pItem->pVectorText = _strgen_StrVector_MakeList(pItem->pVectorText, string);

	return pItem;
}


static void _strgen_StrVector_Delete(mStrIdItem_t *pItem)
{
	StrGenAssert(pItem);

	RxVector_Delete(pItem->pVectorText);
	RxSTD_Free(pItem);
}


static HINT32 _strgen_Parse_GetLine(HUINT8 *srcBuf, HUINT8 *dstBuf)
{
	HINT32		i = 0, j = 0, src_using_len = 0, dst_using_len = 0;
	HINT32		arg_count = 0;
	HCHAR		ch = 0;
	HUINT8		*pStr = NULL;
	HCHAR		pTmp[MAX_CHARS_BUFFER_SIZE*4] = {0,};

	pStr = strstr(srcBuf, "\r\n");
	if(pStr == NULL)
	{
		return 0;
	}

	src_using_len = pStr - srcBuf;

	for(i=0; i < src_using_len; i++)
	{
		if (srcBuf[i] < 0x80)
		{
			pTmp[j++] = srcBuf[i];
		}
		else if (srcBuf[i] < 0xc2)
		{
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i];
			continue;
		}
		else if (srcBuf[i] < 0xe0)
		{
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i];
			continue;
		}
		else if (srcBuf[i] < 0xf0)
		{
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i];
			continue;
		}
		else if (srcBuf[i] < 0xff)
		{
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i++];
			pTmp[j++] = srcBuf[i];
			continue;
		}
		else
		{
			continue;
		}
	}
	pTmp[j] = 0;
	dst_using_len = j;

	for(i=0,j=0; i < dst_using_len; i++)
	{
		switch(pTmp[i])
		{
#if 1
		case '#':
			if(pTmp[i+1] == '#' &&
				pTmp[i+3] == '#' &&
				pTmp[i+4] == '#')
			{
				if(pTmp[i+2] == '#')
				{
					arg_count++;
					ch = '0' + arg_count;
				}
				else if('0' <= pTmp[i+2] && pTmp[i+2] <= '9')
				{
					ch = pTmp[i+2];
				}
				else
				{
					dstBuf[j++] = pTmp[i];
					break;
				}

				i += 4;
				dstBuf[j++] = '%';
				dstBuf[j++] = ch;
				dstBuf[j++] = '$';
				dstBuf[j++] = 's';

			}
			else
			{
				dstBuf[j++] = pTmp[i];
			}
			break;
#endif
		case '\t':
			arg_count = 0;
			dstBuf[j++] = pTmp[i];
			break;

		case '\\':
			dstBuf[j++] = pTmp[i++];
			dstBuf[j++] = pTmp[i];
			break;

		case '\'':
			if(pTmp[i+1] == '\'')
			{
				i++;
			}
			dstBuf[j++] = '\\';
			dstBuf[j++] = '\'';
			break;

		case '\"':
			if(pTmp[i+1] == '"')
			{
				i++;
				dstBuf[j++] = '\\';
				dstBuf[j++] = '"';
			}
			break;

		case '&':
			dstBuf[j++] = '&';
			dstBuf[j++] = 'a';
			dstBuf[j++] = 'm';
			dstBuf[j++] = 'p';
			dstBuf[j++] = ';';
			break;

		case '\r':
			dstBuf[j++] = '\\';
			dstBuf[j++] = 'n';
			break;

		case '\n':
			break;

		default:
			dstBuf[j++] = pTmp[i];
			break;
		}
	}


	dstBuf[j] = 0;
	return (src_using_len+1);
}




static HERROR	_strgen_Parse_StrTable (HUINT8 *utf8File)
{
	HERROR hErr = ERR_OK;

	mStrIdItem_t *pStrIdItem = NULL;

	RxFile_t 	srcFile = NULL;

	HUINT8		buf[MAX_CHARS_BUFFER_SIZE * 4] = {0,};
	HUINT8		*bufTemp = NULL;
	HUINT8		utf8Buf[MAX_CHARS_BUFFER_SIZE * 4] = {0,};

	HINT32		i = 0;
	HINT32		ret = 0;
	HINT32		addPoint = 0;
	HINT32		strCurPos = 0;

	StrGenAssert(s_Mgr);

	s_Mgr->pLanguageNumHash =  RxHashTable_New(RxHash_Table_String_Key, RxHash_Table_String_Equal);
	s_Mgr->pStridHash =  RxHashTable_New(RxHash_Table_String_Key, RxHash_Table_String_Equal);
	s_Mgr->pNoHaveStrId = RxHashTable_New(RxHash_Table_String_Key, RxHash_Table_String_Equal);
	s_Mgr->pNoTransStrId = RxHashTable_New(RxHash_Table_String_Key, RxHash_Table_String_Equal);

	StrGenAssert(s_Mgr->pLanguageNumHash);

	srcFile = RxFile_Open(utf8File, "rb");

	if(srcFile == NULL)
	{
		Rx_Error("[%s:%d] srcFile error!! \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	while(RxFile_EOF(srcFile) == 0)
	{
		RxSTD_MemSet(utf8Buf, 0, (MAX_CHARS_BUFFER_SIZE * 4));
		ret = RxFile_Read(srcFile, utf8Buf, 1, (MAX_CHARS_BUFFER_SIZE * 4));
		if(ret <= 0)
		{
			break;
		}

		if(ret < (MAX_CHARS_BUFFER_SIZE * 4))
		{
			RxSTD_MemSet (utf8Buf+ret, 0, ((MAX_CHARS_BUFFER_SIZE * 4) - ret));
		}

		bufTemp = utf8Buf;
		while(*bufTemp != 0)
		{
			RxSTD_MemSet(buf, 0, (MAX_CHARS_BUFFER_SIZE * 4));
			addPoint  = _strgen_Parse_GetLine(bufTemp, buf);
			if(addPoint == 0)
			{
				for(i=0; i < 8; i++)
				{
					switch(bufTemp[i])
					{
					case '\r':
					case '\n':
					case 0:
						strCurPos++;
						break;

					default:
						i = 8;
						break;
					}
				}
				break;
			}

			strCurPos += addPoint;
			bufTemp += addPoint;
			if(addPoint < 8)
			{
				continue;
			}


			pStrIdItem = _strgen_StrVector_New(buf);
			if(pStrIdItem != NULL)
			{
				RxHashTable_Insert(s_Mgr->pStridHash, pStrIdItem->szStrId, pStrIdItem);

			}

			buf[80] = 0;
			Rx_Trace("%s\n", buf);

		}

		RxFile_Seek(srcFile, 0, strCurPos, eRxFILE_SEEK_SET, NULL, NULL);
	}

	RxFile_Close(srcFile);

	return hErr;

}

static HERROR _strgen_Set_LanguageNum(){

	HERROR hErr = ERR_OK;

	HINT8 *pLang = 0;

	HINT32 i = 0, max = 0;

	mStrIdItem_t	*temp = NULL;

	temp = (mStrIdItem_t *)RxHashTable_Lookup(s_Mgr->pStridHash, " actualid");
	if(temp == NULL)
	{
		temp = (mStrIdItem_t *)RxHashTable_Lookup(s_Mgr->pStridHash, "actual id");
	}
	if(temp == NULL)
	{
		temp = (mStrIdItem_t *)RxHashTable_Lookup(s_Mgr->pStridHash, "actualid");
	}
	StrGenAssert(temp);

	max = RxVector_Length(temp->pVectorText);

	for(i=0; i < max; i++)
	{
		pLang = RxVector_ItemAt(temp->pVectorText, i);

		if(pLang != NULL)
		{
			RxHashTable_Insert(s_Mgr->pLanguageNumHash,(void *) _strgen_String_ToLower(pLang), (void *)i);
		}
	}

	return hErr;
}



/*************************************************************************************************************/
#define ___________FUNCTION_Set_Data______________________________________________________________________
/*************************************************************************************************************/

static HINT32 _strgen_Init(){

	HINT32 nRet = 0;
	HERROR hErr = ERR_OK;
	HCHAR	*filename = NULL;

	hErr = RxCP_Init();
	if(hErr != ERR_OK)
	{
		Rx_Error("[%s:%d] RxCP_Init error!!\n", __FUNCTION__, __LINE__);
		nRet = -100;
		goto ERROR_BREAK;
	}

	hErr = _strgen_Convert_UCStoUTF8(s_strgen_pre_InputUCS2File, s_strgen_pre_OutputUTF8File);
	if(hErr != ERR_OK)
	{
		Rx_Error("[%s:%d] _strgen_Convert_UCStoUTF8 error!!\n", __FUNCTION__, __LINE__);
		nRet = -200;
		goto ERROR_BREAK;
	}

	hErr = _strgen_Manager_New();
	if(hErr != ERR_OK)
	{
		Rx_Error("[%s:%d] _strgen_Manager_New error!!\n", __FUNCTION__, __LINE__);
		nRet = -300;
		goto ERROR_BREAK;
	}

	hErr = _strgen_Parse_StrTable(s_strgen_pre_OutputUTF8File);
	if(hErr != ERR_OK)
	{
		Rx_Error("[%s:%d] _strgen_Parse_StrTable error!!\n", __FUNCTION__, __LINE__);
		nRet = -400;
		goto ERROR_BREAK;
	}

	hErr = _strgen_Set_LanguageNum();
	if(hErr != ERR_OK)
	{
		Rx_Error("[%s:%d] _strgen_Set_LanguageNum error!!\n", __FUNCTION__, __LINE__);
		nRet = -500;
		goto ERROR_BREAK;
	}


ERROR_BREAK:
	RxCP_Destroy();

	return nRet;
}

static HINT32 _strgen_Read_LanguageSetting(HUINT8 *srcFile){

	HERROR hErr = ERR_OK;

	RxFile_t 	pLanSettingFile = NULL;

	HUINT8	buf[MAX_CHARS_BUFFER_SIZE * 4] = {0};
	HUINT8	bufLan[128] = {0};
//	HUINT8	bufSetVal;

	HUINT32 ret = 0;
	HUINT32 strpos = 0;
	HUINT32 i = 0,j= 0;
	HUINT32 num = 0;

	pLanSettingFile = RxFile_Open(srcFile, "rb");
	if(NULL == pLanSettingFile)
	{
		return ERR_FAIL;
	}
	StrGenAssert(pLanSettingFile);

	ret = RxFile_Read(pLanSettingFile, buf, 1, (MAX_CHARS_BUFFER_SIZE * 4));
	if(ret <= 0)
		return ERR_FAIL;
	RxSTD_MemSet(bufLan, 0, 128);
	while(1)
	{

		if(buf[i] == '\n')
		{
			_strgen_util_StrNCpy(s_Mgr->pSelectLan[s_Mgr->pSetLanCount], bufLan, 128);
			s_Mgr->pSetLanCount++;
			RxSTD_MemSet(bufLan, 0, 128);
			j =0;
		}
		else if (buf[i] == 0)
		{
			break;
		}
		else
		{
			if( buf[i] >= 'A' && buf[i] <= 'Z')
			{
				bufLan[j++] = _tolower(buf[i]);
			}
			else
			{
				bufLan[j++] = (buf[i]);
			}
		}

		i++;
	}

#if 0
	while(RxFile_EOF(pLanSettingFile) == 0)
	{
		RxSTD_MemSet(buf, 0, (MAX_CHARS_BUFFER_SIZE * 4));

		ret = RxFile_Read(pLanSettingFile, buf, 1, (MAX_CHARS_BUFFER_SIZE * 4));
		if(ret <= 0)
		{
			break;
		}


		if(ret < (MAX_CHARS_BUFFER_SIZE * 4))
		{
			RxSTD_MemSet (buf+ret, 0, (MAX_CHARS_BUFFER_SIZE - ret));
		}

		while(1)
		{
			if(buf[i] == ' ')
			{
				RxSTD_StrNCpy(bufLan,buf,i);
				bufLan[i] = '\0';
				bufSetVal = buf[++i];
				break;
			}
			else if(buf[i] == '\r' || buf[i] == '\n')
			{
				bufSetVal = 'n';
				break;
			}
			i++;
		}

		strpos += i;

		if(bufSetVal == 'Y' || bufSetVal == 'y')
		{
			RxSTD_StrCpy(s_Mgr->pSelectLan[s_Mgr->pSetLanCount], bufLan);
			s_Mgr->pSetLanCount++;
			strpos  = strpos + 5;
		}
		else
		{
			//MHSTD_StrCpy(s_Mgr->pSelectLan[s_Mgr->pSetLanCount], bufLan);
			//s_Mgr->pSetLanCount++;
			strpos  = strpos + 4;
		}

		i=0;

		RxFile_Seek(pLanSettingFile, 0, strpos, eRxFILE_SEEK_SET, NULL, NULL);

	}
#endif
	RxFile_Close(pLanSettingFile);

	return hErr;
}

static void	_strgen_Read_UsingList_HashForeachCallback(void* key , void* value , void* user_data)
{
	RxFile_t 	pOrgStrFile = (RxFile_t)user_data;
	HINT32		strLen = 0;

 	strLen = RxSTD_StrLen(value);

	RxFile_Write( pOrgStrFile, value, sizeof (HUINT8), strLen);
	RxFile_Write( pOrgStrFile, "\r\n", sizeof (HUINT8), 2);
	Rx_Print("\t%s\r\n", value);
}

static HINT32 _strgen_Read_UsingList(HUINT8 *srcFile, HUINT8 *desFile)
{
	HERROR hErr = ERR_OK;

	RxFile_t 	pUsingStrFile = NULL;
	RxFile_t 	pOrgStrFile = NULL;

	HUINT8		buf[MAX_CHARS_BUFFER_SIZE] = {0,};
	HUINT8		bufTemp[MAX_CHARS_BUFFER_SIZE] = {0,};

	HUINT32 ret = 0;
	HUINT32 strpos = 0;
	HUINT32 i = 0;
	HINT32	strLen = 0;

	StrGenAssert(s_Mgr);

	s_Mgr->pUsingStrIdHash=  RxHashTable_New(RxHash_Table_String_Key, RxHash_Table_String_Equal);
	//s_Mgr->pGetUsingStrId=  MHHashTable_New(HxHash_Table_String_Key, HxHash_Table_String_Equal);

	StrGenAssert(s_Mgr->pUsingStrIdHash);

	pUsingStrFile = RxFile_Open(srcFile, "rb");
	StrGenAssert(pUsingStrFile);
	

	while(RxFile_EOF(pUsingStrFile) == 0)
	{
		RxSTD_MemSet(buf, 0, (MAX_CHARS_BUFFER_SIZE));

		ret = RxFile_Read(pUsingStrFile, buf, 1, (MAX_CHARS_BUFFER_SIZE));
		if(ret <= 0)
		{
			break;
		}

		if(ret < (MAX_CHARS_BUFFER_SIZE))
		{
			RxSTD_MemSet (buf+ret, 0,  ((MAX_CHARS_BUFFER_SIZE) - ret));
		}

		while(1)
		{
			if(buf[i] == '\n')
			{
				_strgen_util_StrNCpy(bufTemp,buf,i);
				bufTemp[i] = '\0';
				strpos = strpos + i + 1;
				break;
			}
			else if(buf[i] == '\0')
			{
				if(i > 1)
				{
					_strgen_util_StrNCpy(bufTemp,buf,i);
					bufTemp[i] = '\0';
					strpos = strpos + i;
				}
				else
				{
					bufTemp[0] = 0;
					strpos = strpos + i;
				}
				break;
			}
			i++;
		}

		strLen = RxSTD_StrLen(bufTemp);
		if(strLen > 0)
		{
			_strgen_util_ToLower(bufTemp, buf);
			buf[strLen] = 0;

			if((HUINT8*)RxHashTable_Lookup(s_Mgr->pUsingStrIdHash, buf) == NULL)
			{
				RxHashTable_Insert(s_Mgr->pUsingStrIdHash, (void *)RxSTD_StrDup(buf), (void *)RxSTD_StrDup(bufTemp));
			}

			i=0;
		}
		RxFile_Seek(pUsingStrFile, 0, strpos, eRxFILE_SEEK_SET, NULL, NULL);

	}

	RxFile_Close(pUsingStrFile);

	RxSTD_MemSet(buf, 0, (MAX_CHARS_BUFFER_SIZE));
	RxSTD_sprintf(buf, "%s%s.%s", STRGEN_FILENAME_HEADER, desFile, STRGEN_FILENAME_EXTENTION);
	pOrgStrFile = RxFile_Open(buf, "wb");
	RxHashTable_Foreach(s_Mgr->pUsingStrIdHash, _strgen_Read_UsingList_HashForeachCallback, pOrgStrFile);
	RxFile_Close(pOrgStrFile);

	return hErr;

}

static HINT32 _strgen_Make_UsingList(HUINT8 *srcFile, HUINT8 *desFile)
{
	HERROR hErr = ERR_OK;

	RxFile_t 	pSrcFile = NULL;
	RxFile_t 	pDstFile = NULL;

	HUINT8		*temp = NULL;
	HUINT8		*strId = NULL;
	HUINT8		*strIdEnd = NULL;
	HUINT8		*strIdEndTemp = NULL;
	HUINT8		rbuf[MAX_CHARS_BUFFER_SIZE] = {0,};
	HUINT8		wbuf[MAX_CHARS_BUFFER_SIZE] = {0,};

	RxFile_t 	pCpHeaderFile = NULL;
	HUINT8		*pStrHeader = NULL;
	HUINT8		*pOutFileName = NULL;

	pSrcFile = RxFile_Open(srcFile, "rb");
	StrGenAssert(pSrcFile);

	pDstFile = RxFile_Open(desFile, "wb");
	StrGenAssert(pDstFile);

	temp = getenv("CONFIG_RES_STRING_OUTPUT_FILE");
	if(temp)
	{
		pOutFileName = RxSTD_StrDup(temp);
		StrGenAssert(pOutFileName);
			
		Rx_Print("copy file ... %s\n", pOutFileName);

		pCpHeaderFile = RxFile_Open(pOutFileName, "wb");
		StrGenAssert(pCpHeaderFile);		
	}

	if(NULL != pOutFileName)
	{
		pStrHeader = "/******************************************************************************************/ \r\n";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		pStrHeader = "/* This file should be generated by Humax ONDK Strgen (Ver. 2.991.0)                      */ \r\n";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		pStrHeader = "/******************************************************************************************/ \r\n";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		pStrHeader = "/* 자동생성 파일입니다. 수정하시려면 아래 파일을 수정하세요.                              */ \r\n";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		pStrHeader = "/* The auto-generated files. To modify the file, please modify the                        */ \r\n";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		pStrHeader = "/* ";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		RxFile_Write(pCpHeaderFile, srcFile, sizeof (HUINT8), RxSTD_StrLen(srcFile));
		pStrHeader = " */\r\n";	RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));

		pStrHeader = "#ifndef __NXRES_STRID_H__\r\n#define __NXRES_STRID_H__ \r\n";
		RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	}

	while(RxFile_EOF(pSrcFile) == 0)
	{
		RxSTD_MemSet(rbuf, 0, (MAX_CHARS_BUFFER_SIZE));
		RxSTD_MemSet(wbuf, 0, (MAX_CHARS_BUFFER_SIZE));

		temp = RxFile_GetString(pSrcFile, rbuf, MAX_CHARS_BUFFER_SIZE);
		if(NULL == temp) break;

		temp = RxSTD_StrStr(rbuf, "#define");
		if(NULL == temp) continue;
		
		strId = RxSTD_StrStr(rbuf, "STR_");
		if(NULL == strId)
		{
			strId = RxSTD_StrStr(rbuf, "SJP_");
		}
		if(NULL == strId) continue;

		RxFile_Write(pCpHeaderFile, temp, sizeof (HUINT8), RxSTD_StrLen(temp));

		strIdEnd = RxSTD_StrStr(strId, "_ID");
		if(NULL == strIdEnd)
		{
			continue;
		}
		
		strIdEndTemp = strIdEnd;
		while(strIdEndTemp)
		{
			strIdEnd = strIdEndTemp;
			strIdEndTemp += 1;
			strIdEndTemp = RxSTD_StrStr(strIdEndTemp, "_ID");
		}

		strIdEnd+=3;
		*strIdEnd = 0;

		RxSTD_snprintf(wbuf, MAX_CHARS_BUFFER_SIZE, "%s\r\n", strId);
		RxFile_Write(pDstFile, wbuf, RxSTD_StrLen(wbuf), 1);
	}

	RxFile_Close(pSrcFile);
	RxFile_Close(pDstFile);	

	if(NULL != pOutFileName)
	{
		RxSTD_Free(pOutFileName);

		pStrHeader = "#endif /* __NXRES_STRID_H__ */\r\n";
		RxFile_Write(pCpHeaderFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
		RxFile_Close(pCpHeaderFile);
	}

	return ERR_OK;
}

static void	_strgen_Write_StrValue_HashForeachCallback(void* key , void* value , void* user_data)
{
	mStrValue_t *pstrValue = (mStrValue_t *)user_data;

	HUINT8		*mStrVal =NULL;
	HUINT32		strLen =0;
	HUINT8		szStr[MAX_STRID_LENGTH] = {0, };

	_strgen_StrReplaceChar_Define2StrId(szStr, key);
	mStrVal = _strgen_Get_LangValue(szStr, pstrValue->pszLang);
	strLen = RxSTD_StrLen(mStrVal);

	RxFile_Write( pstrValue->pTargetFile, mStrVal, sizeof (HUINT8), strLen);
	RxFile_Write( pstrValue->pTargetFile, "\r\n", sizeof (HUINT8), 2);
}


static HINT32 _strgen_Write_StrValue(){

	HERROR hErr = ERR_OK;

	RxFile_t	pTargetFile = NULL;

	HUINT8	filename[MAX_FILENAME_LENGTH] = {0, };
	HUINT8	strID[MAX_STRID_LENGTH] = {0,};
	const HUINT8 	*strLangName = NULL;

	HINT32	i=0, j=0;
	HUINT32	lanNum=0;
	HUINT8	szNoTransStrId[MAX_STRID_LENGTH*2] = {0, };

	mStrValue_t stStrValue;

	RxSTD_MemSet(&stStrValue, 0, sizeof(mStrValue_t));

	for(i=0;i<s_Mgr->pSetLanCount;i++)
	{
		RxSTD_MemSet(filename, 0, sizeof(HUINT8) * MAX_FILENAME_LENGTH);

		strLangName = RxLANG_FullNameTo639(s_Mgr->pSelectLan[i]);
		if(NULL == strLangName)
		{
			strLangName = s_Mgr->pSelectLan[i];
			RxSTD_StrCat(filename, "error_");
		}


		RxSTD_StrCat(filename, STRGEN_FILENAME_HEADER);
		RxSTD_StrCat(filename, strLangName);
		RxSTD_StrCat(filename, STRGEN_FILENAME_EXTENTION);

		pTargetFile = RxFile_Open(filename, "wb");
		if(pTargetFile == NULL)
		{
			StrGenAssert(0);
			return 0;
		}

		stStrValue.pszLang = s_Mgr->pSelectLan[i];
		stStrValue.pTargetFile	= pTargetFile;

		RxHashTable_Foreach(s_Mgr->pUsingStrIdHash, _strgen_Write_StrValue_HashForeachCallback, &stStrValue);
		RxFile_Close(pTargetFile);
	}

	return hErr;

}

static void	_strgen_StrReplaceChar_Define2StrId(HCHAR *dst, const HCHAR *src)
{
	HCHAR *ret = dst;
	HUINT8 ch = 0, ch2 = 0;
	HUINT8 lbit = 0;
	HUINT8 rbit = 0;

	while (*src != '\0')
	{
		ch = (HUINT8)*src++;
		ch2 = (HUINT8)*src;
		if('0' == ch
			&& 'x' == ch2)
		{
			lbit = *(src+1);
			rbit = *(src+2);

			if((('0' <= lbit && lbit <= '9') || ('a' <= lbit && lbit <= 'f'))
				&& (('0' <= rbit && rbit <= '9') || ('a' <= rbit && rbit <= 'f')))
			{
				if('0' <= lbit && lbit <= '9')
				{
					*dst  |= ((lbit << 4) & 0xF0);
				}
				else
				{
					*dst |= (((lbit-'a'+10) << 4) & 0xF0);
				}

				if('0' <= rbit && rbit <= '9')
				{
					*dst  |= (rbit & 0x0F);
				}
				else
				{
					*dst |= ((rbit-'a'+10) & 0x0F);
				}
				dst++;
				src += 3;
			}
			else
			{
				*dst++ = ch;
			}
		}
		else
		{
			*dst++ = ch;
		}
	}
}

static void	_strgen_StrReplaceChar_StrId2Define(HCHAR *dst, const HCHAR *src)
{
	HCHAR *ret = dst;
	HUINT8 ch = 0;
	HUINT8 lbit = 0;
	HUINT8 rbit = 0;

	while (*src != '\0')
	{
		ch = (HUINT8)*src;
		if(('A' <= ch && ch <= 'Z') ||
		   ('0' <= ch && ch <= '9') ||
		   ('a' <= ch && ch <= 'z'))
		{
			*dst++ = ch;
		}
		else
		{
			if(ret == dst)
			{
				*dst = 0;
				break;
			}

			switch(ch)
			{
			case '_':
				*dst++ = '_';
				break;

			default:
				lbit = (ch & 0xF0) >> 4;
				rbit = (ch & 0x0F);

				*dst++ = '0';
				*dst++ = 'x';
				if(0x00 <= lbit && lbit <= 0x09)
				{
					*dst++ = 0x30 + lbit;
				}
				else
				{
					*dst++ = 'a' + (lbit-10);
				}

				if(0x00 <= rbit && rbit <= 0x09)
				{
					*dst++ = 0x30 + rbit;
				}
				else
				{
					*dst++ = 'a' + (rbit-10);
				}
			}
		}
		src++;
	}
}


static void	_strgen_Write_StrIdHeader_foreachCallback(void* key , void* value , void* user_data)
{
	RxFile_t	pTargetFile = (RxFile_t)user_data;
	HCHAR		szStrId[MAX_STRID_LENGTH] = {0, };
	HCHAR		szDefineStrId[MAX_STRID_LENGTH*3] = {0, };
	HCHAR		szReplStrId[MAX_STRID_LENGTH] = {0, };

	if(key)
	{
		RxSTD_StrCpy(szStrId, (char *)key);

		_strgen_String_ToUpper(szStrId);
		_strgen_StrReplaceChar_StrId2Define(szReplStrId, szStrId);

		if(0 != szReplStrId[0])
		{
			RxSTD_snprintf(szDefineStrId, MAX_STRID_LENGTH*3, "#define %-50s \t\"%s\"\r\n", szReplStrId, szStrId);
			RxFile_Write(pTargetFile, szDefineStrId, sizeof (HUINT8), RxSTD_StrLen(szDefineStrId));
		}
	}
}

static HINT32 _strgen_Write_StrIdHeader(HCHAR *filename)
{
	HERROR hErr = ERR_OK;

	RxFile_t	pTargetFile = NULL;

	HUINT8	headerFile[MAX_STRID_LENGTH] = {0, };
	HUINT8 	*pStrHeader = NULL;

	RxSTD_snprintf(headerFile, MAX_STRID_LENGTH, "%sstrid.h", filename);
	pTargetFile = RxFile_Open(headerFile, "wb");
	if(pTargetFile == NULL)
	{
		Rx_Error("[%s:%d] Error filename (%s)\r\n", __FUNCTION__, __LINE__, headerFile);
		StrGenAssert(0);
		return 0;
	}

	pStrHeader = "/******************************************************************************************/ \r\n";	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	pStrHeader = "/* This file should be generated by Humax ONDK Strgen (Ver. 2.991.0)                      */ \r\n";	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	pStrHeader = "/******************************************************************************************/ \r\n";	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	pStrHeader = "#ifndef __STRGEN_DEFINE_H__\r\n#define __STRGEN_DEFINE_H__ \r\n";
	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));

	RxHashTable_Foreach(s_Mgr->pStridHash, _strgen_Write_StrIdHeader_foreachCallback, pTargetFile);

	pStrHeader = "#endif /* __STRGEN_DEFINE_H__ */\r\n";
	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));

	RxFile_Close(pTargetFile);

	return hErr;
}

static void	_strgen_Write_NoID_foreachCallback(void* key , void* value , void* user_data)
{
	RxFile_t	pTargetFile = (RxFile_t)user_data;

	if(value)
	{
		RxFile_Write(pTargetFile, value, sizeof (HUINT8), RxSTD_StrLen(value));
		Rx_Print("* \t%s", value);
	}
}

static HINT32 _strgen_Write_NoHaveStrID()
{
	RxFile_t 	file;
	HCHAR		*filename = NULL;
	HUINT8 		*pStrHeader = NULL;

	filename = STRGEN_SRCFILE_PATH"/notrans_strid.txt";
	file = RxFile_Open(filename, "wb");
	if(NULL == file)
	{
		Rx_Error("[%s:%d] Open error (%s)\r\n", __FUNCTION__, __LINE__, filename);
		return -600;
	}
	Rx_Print("\r\nCan't find STR_ID list...\r\n");
	Rx_Print("********************************************************************************\r\n");
	RxHashTable_Foreach(s_Mgr->pNoHaveStrId, _strgen_Write_NoID_foreachCallback, file);
	Rx_Print("********************************************************************************\r\n\r\n");
	RxFile_Close(file);
}

static HINT32 _strgen_Write_NoTransStrID()
{
	RxFile_t 	file;
	HCHAR		*filename = NULL;
	HUINT8 		*pStrHeader = NULL;

	filename = STRGEN_SRCFILE_PATH"/nohave_strid.txt";
	file = RxFile_Open(filename, "wb");
	if(NULL == file)
	{
		Rx_Error("[%s:%d] Open error (%s)\r\n", __FUNCTION__, __LINE__, filename);
		return -600;
	}

	Rx_Print("\r\nCan't find translate list...\r\n");
	Rx_Print("********************************************************************************\r\n");
	RxHashTable_Foreach(s_Mgr->pNoTransStrId, _strgen_Write_NoID_foreachCallback, file);
	Rx_Print("********************************************************************************\r\n\r\n");

	RxFile_Close(file);
}


/*************************************************************************************************************/
#define ___________FUNCTION_API__________________________________________________________________________
/*************************************************************************************************************/

static HUINT8 *_strgen_Get_LangValue(HINT8 *str_id, HINT8 *lan)
{
	HERROR hErr = ERR_OK;
	HUINT8 *temp = 0;
	HUINT8 *dupStr = 0;
	HINT32	pLanguageNum=0;
	static HINT8 s_haveStrId[1024] = {0, };
	HINT8 noStrId[1024] = {0, };

	mStrIdItem_t	*Select_Str_Vector = NULL;

	Select_Str_Vector = (mStrIdItem_t *)RxHashTable_Lookup(s_Mgr->pStridHash, str_id);
	if(Select_Str_Vector == NULL)
	{
		RxSTD_sprintf(s_haveStrId, "## No StrID : %s ##", str_id);
		RxSTD_sprintf(noStrId, "%s\r\n", str_id);
		RxHashTable_Insert(s_Mgr->pNoHaveStrId, RxSTD_StrDup(str_id), RxSTD_StrDup(noStrId));
		return s_haveStrId;
	}

	pLanguageNum = (HINT32) RxHashTable_Lookup(s_Mgr->pLanguageNumHash, _strgen_String_ToLower(lan));
	temp = RxVector_ItemAt(Select_Str_Vector->pVectorText, pLanguageNum);
	if(NULL == temp || 0 == RxSTD_StrLen(temp))
	{
		RxSTD_sprintf(noStrId, "%s::%s\r\n", str_id, lan);
		dupStr = RxSTD_StrDup(noStrId);
		RxHashTable_Insert(s_Mgr->pNoTransStrId, dupStr, dupStr);

		RxSTD_sprintf(s_haveStrId, "## No Tans: %s ##", str_id);
		temp = s_haveStrId;
	}

	return temp;
}

static HCHAR *_strgen_String_ToLower(HCHAR *arg)
{
	HCHAR *pszString = arg;

	while(*pszString)
	{
		if( *pszString >= 'A' && *pszString <= 'Z')
		{
			*pszString = _tolower(*pszString);
		}
		pszString++;
	}
	return arg;
}

static HCHAR *_strgen_String_ToUpper(HCHAR *arg)
{
	HCHAR *pszString = arg;

	while(*pszString)
	{
		if( *pszString >= 'a' && *pszString <= 'z')
		{
			*pszString = _toupper(*pszString);
		}
		pszString++;
	}
	return arg;
}


/*******************************************************************/
#define ___________MAIN_______________________________________________________________________
/*************************************************************************************************************/

HINT32 main(HINT32 argc, HINT8 *argv[])
{
	HERROR hErr = ERR_OK;
	HINT32 ret = 0;
	HCHAR *temp_product = NULL;
	HCHAR *temp_resname = NULL;
	HCHAR *read_usingfile_header = NULL;

	HCHAR product[256] = {0, };
	HCHAR usingstringid[256] = {0, };
	HCHAR organizeStr[256] = {0, };
	HCHAR languagesettigns[256] = {0, };

	Rx_Print("********************************************************************************\r\n");
	Rx_Print("*                              ONDK String Tools                               *\r\n");
	Rx_Print("********************************************************************************\r\n");

	temp_product  = getenv("CONFIG_PRODUCT_NAME");
	if(temp_product  == NULL)
	{
		_strgen_util_StrNCpy(product, "hms1000sph2", 256);
	}
	else
	{
		_strgen_util_StrNCpy(product, temp_product, 256);
	}
	Rx_Print("product name : %s\n", product);

	RxSTD_sprintf(usingstringid, "%s/%s_using.txt", STRGEN_SRCFILE_PATH, product);
	if(getenv("CONFIG_APUI_NATIVE_APP") != NULL)
	{
		hErr = _strgen_Manager_New();
		StrGenAssert(hErr == ERR_OK);

		Rx_Print("Make using file (%s)\r\n", usingstringid);
		read_usingfile_header = getenv("CONFIG_RES_STRING_FILE");
		if(NULL != read_usingfile_header)
		{
			read_usingfile_header = RxSTD_StrDup(read_usingfile_header);
			_strgen_Make_UsingList(read_usingfile_header, usingstringid);
			RxSTD_Free(read_usingfile_header);
			read_usingfile_header = NULL;
		}
	}

	s_strgen_pre_InputUCS2File = getenv("STRTOOL_PROD_COUNTRY");
	if(s_strgen_pre_InputUCS2File == NULL)
	{
		s_strgen_pre_InputUCS2File = STRGEN_DEFAULT_INPUT_UCS2_STRING;
	}
	Rx_Print("Input UCS2 File name : %s\n", s_strgen_pre_InputUCS2File);


	s_strgen_pre_OutputUTF8File = getenv("STRTOOL_PROD_COUNTRY_OUTPUT");
	if(s_strgen_pre_OutputUTF8File == NULL)
	{
		s_strgen_pre_OutputUTF8File = STRGEN_DEFAULT_OUTPUT_UTF8_STRING;
	}
	Rx_Print("Output UTF8 File name : %s\n", s_strgen_pre_OutputUTF8File);

	Rx_Print("init...Ok\n");
	ret = _strgen_Init();
	StrGenAssert(0 == ret)

	RxDIR_Make(STRGEN_DSTFILE_PATH);

	Rx_Print("make folder...Ok\n");

	Rx_Print("Gen info...\n");

	Rx_Print("\tUsing    File Name : %s\n", usingstringid);

	RxSTD_sprintf(organizeStr, "strid");
	Rx_Print("\tOrganize File Name : %s\n", organizeStr);

	RxSTD_sprintf(languagesettigns, "%s/%s_language.txt", STRGEN_SRCFILE_PATH, product);
	Rx_Print("\tLanguage File Name : %s\n", languagesettigns);

	Rx_Print("make using file ...\n");
	_strgen_Read_UsingList(usingstringid, organizeStr);

	Rx_Print("read language setting...\n");
	_strgen_Read_LanguageSetting(languagesettigns);

	temp_resname  = getenv("CONFIG_RESOURCE_HEADER_NAME");
	if(temp_resname  == NULL)
	{
		temp_resname = "../../../include/res_";
	}

	Rx_Print("write strings value...\n");
	_strgen_Write_StrValue();

	Rx_Print("make STR_xxx_ID header ...\n");
	_strgen_Write_StrIdHeader(temp_resname);

	_strgen_Write_NoHaveStrID();
	_strgen_Write_NoTransStrID();

	return 0;
}


