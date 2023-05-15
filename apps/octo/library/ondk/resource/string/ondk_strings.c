#include <hlib.h>

#include <ondk.h>


#define DUMY_PROC_ID	0x00000991

#define ONDK_STRINGFILE_HEADERNAME "/usr/string/str_"
#define ONDK_STRINGFILE_PATH "/usr/string"

#define ONDK_DEFAULT_ORANIZE_STRING 	"strgen_organize_str_id.txt"
#define ONDK_STRINGFILE_EXTENTION ".txt"

#define STRGEN_DEFAULT_LANGUAGE "eng"

#ifndef UNUSED
#define UNUSED(_x)		((void)(_x))
#endif

/*************************************************************************************************************/
#define _____STRGEN_BUFFER___________________________________________________________________________
/*************************************************************************************************************/
#define MAX_CHARS_BUFFER_SIZE		8192
#define MAX_STRID_LENGTH			128 // same StrGenTool

typedef struct _StringItem
{
	HUINT8 	strid[MAX_STRID_LENGTH];
	HUINT8 	*strvalue;
	HINT32	nRefCnt;
	HBOOL	unlinkedData;
} StringItem_t;

typedef struct _StringProcItem
{
	void			*procId;
	HxList_t		*pUsingStrItemList; /* StringItem_t */
} StringProcItem_t;


typedef struct _StringsMgr
{
	HINT8	fileNameHeader[ONDK_FILENAME_MAX_LENGTH];
	HINT8	fileNameExt[ONDK_FILEEXT_MAX_LENGTH];
	HINT8	language[ONDK_LANGUAGE_STRING_MAX_LENGTH];
	HINT8	organizeStringFile[ONDK_FILENAME_MAX_LENGTH];

	HxHASH_t 			*stringHash; /* StringItem_t */
	StringProcItem_t	*pstArrProcItem;
	ONDKSTR_Settings_t 	stSettings;
} StringsMgr_t;



typedef struct _StringBuffer
{
	HULONG 	taskId;
	HUINT8	*buffer;
	HINT32 	dummyId;
} StringBuffer_t;



STATIC StringsMgr_t *s_pStrMgr = NULL;
STATIC HULONG		s_ulStrSem = 0;

/*************************************************************************************************************/
#define ___________FUNCTION_PRIVATE______________________________________________________________________
/*************************************************************************************************************/


STATIC ONDK_Result_t _strings_GetDB(StringsMgr_t *pMgr, HCHAR *pszLang639)
{
	ONDK_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(pszLang639)
	{
		HLIB_STD_StrNCpySafe(pMgr->language, pszLang639, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	}

	ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}

STATIC StringItem_t *_strings_New_StrItem(HUINT8 *pStrFileBuf, HUINT8 *pValFileBuf)
{
	StringItem_t *pStrItem = NULL;

	pStrItem = (StringItem_t *)ONDK_Calloc(sizeof(StringItem_t));
	if(NULL == pStrItem)
	{
		HxLOG_Assert(pStrItem);
		return NULL;
	}

#if 0
	pStrItem->strid = ONDK_StrDup(pStrFileBuf);
	if(NULL == pStrItem->strid)
	{
		HxLOG_Assert(pStrItem->strid);
		return NULL;
	}
#else
	HxSTD_StrNCpy(pStrItem->strid, pStrFileBuf, MAX_STRID_LENGTH);
#endif
	pStrItem->strvalue = ONDK_StrDup(pValFileBuf);
	if(NULL == pStrItem->strvalue)
	{
		HxLOG_Assert(pStrItem->strvalue);

		ONDK_Free(pStrItem);
		return NULL;
	}
	pStrItem->unlinkedData = FALSE;
	pStrItem->nRefCnt = 0;

	return pStrItem;
}

STATIC void	_strings_Delete_StrItem(void* data)
{
	StringItem_t *pStrItem = (StringItem_t *)data;

	if(pStrItem == NULL)
	{
		return;
	}

	if(pStrItem->nRefCnt > 0)
	{
		pStrItem->unlinkedData = TRUE;
	}
	else
	{
#if 0
		if(pStrItem->strid)
		{
			ONDK_Free(pStrItem->strid);
		}
#endif
		if(pStrItem->strvalue)
		{
			ONDK_Free(pStrItem->strvalue);
		}
		ONDK_Free(pStrItem);
	}

}

STATIC HERROR	_strings_GetStringByFile(HxFILE_t fp, HUINT8 *pBuf, HINT32 nBufSize)
{
	HUINT8 		*pRet = 0;
	HINT32		nLen = 0;

	HxSTD_MemSet(pBuf, 0, nBufSize);

	pRet = HLIB_FILE_GetString(fp, pBuf, nBufSize);
	if(pRet == NULL)
	{
		return ERR_FAIL;
	}

	nLen = HxSTD_StrLen(pBuf);
	if(nLen == 0)
	{
		return ERR_FAIL;
	}

	nLen--;
	while(pBuf[nLen])
	{
		if(HxSTD_IsSpace(pBuf[nLen]))
		{
			pBuf[nLen--] = 0;
			continue;
		}
		break;
	}

	return ERR_OK;
}


STATIC void _strings_Append_UsingItem(StringItem_t *pStrItem,  void *procId)
{
	StringsMgr_t 		*pStrMgr = s_pStrMgr;

	HINT32				i = 0;
	HxList_t			*pFindList = NULL;
	StringProcItem_t 	*pProcItem = NULL;
	StringProcItem_t 	*pVoidProcItem = NULL;

	if(NULL == pStrMgr || NULL == pStrMgr->pstArrProcItem)
	{
		return;
	}

	if(NULL == procId)
	{
		procId = pStrMgr->stSettings.cbGetProcId();
		if(NULL == procId)
		{
			return;
		}
	}

	for(i=0; i < pStrMgr->stSettings.nProcMaxCount; i++)
	{
		if(pStrMgr->pstArrProcItem[i].procId == procId)
		{
			pProcItem = &pStrMgr->pstArrProcItem[i];
			break;
		}
		if(NULL == pVoidProcItem && NULL == pStrMgr->pstArrProcItem[i].procId)
		{
			pVoidProcItem = &pStrMgr->pstArrProcItem[i];
		}
	}

	if(NULL == pProcItem)
	{
		if(NULL == pVoidProcItem)
		{
			ONDK_Error("[%s:%d] Out of ProcArray\r\n", __FUNCTION__, __LINE__);
			return;
		}
		pVoidProcItem->procId = procId;
		pVoidProcItem->pUsingStrItemList = NULL;
		pProcItem = pVoidProcItem;
	}

	if(pStrItem->nRefCnt > 0)
	{
		pFindList = HLIB_LIST_Find(pProcItem->pUsingStrItemList, pStrItem);
	}

	if(NULL == pFindList)
	{
		ONDK_Print("-[%s:%d] String Append(%s) Proc(0x%x) ref(%d)\r\n", __FUNCTION__, __LINE__, pStrItem->strid, pProcItem->procId, pStrItem->nRefCnt);
		pStrItem->nRefCnt++;
		pProcItem->pUsingStrItemList = HLIB_LIST_Append(pProcItem->pUsingStrItemList, pStrItem);
	}
}

STATIC void _strings_Remove_UsingItem(HINT8 *str_id, void *procId)
{
	StringsMgr_t 		*pStrMgr = s_pStrMgr;

	HINT32				i = 0;
	HxList_t			*pList = NULL;
	StringItem_t		*pStrItem = NULL;
	StringProcItem_t 	*pProcItem = NULL;

	if(NULL == pStrMgr || NULL == pStrMgr->pstArrProcItem)
	{
		return;
	}

	if(NULL == procId)
	{
		procId = pStrMgr->stSettings.cbGetProcId();
		if(NULL == procId)
		{
			return;
		}
	}

	for(i=0; i < pStrMgr->stSettings.nProcMaxCount; i++)
	{
		if(pStrMgr->pstArrProcItem[i].procId == procId)
		{
			pProcItem = &pStrMgr->pstArrProcItem[i];
			break;
		}
	}

	if(NULL == pProcItem)
	{
		return;
	}

	pList = pProcItem->pUsingStrItemList;
	while(pList)
	{
		pStrItem = HLIB_LIST_Data(pList);
		HxLOG_Assert(pStrItem);

		if(NULL == str_id || HxSTD_StrCmp(str_id, pStrItem->strid) == 0)
		{
			if(pStrItem->nRefCnt > 0)
			{
				pStrItem->nRefCnt--;

				if(pStrItem->nRefCnt <= 0 && pStrItem->unlinkedData == TRUE)
				{
					ONDK_Print("-[%s:%d] String Delete (%s) Proc(0x%x) ref(%d)\r\n", __FUNCTION__, __LINE__, pStrItem->strid, pProcItem->procId, pStrItem->nRefCnt);
					_strings_Delete_StrItem(pStrItem);
				}
				else
				{
					ONDK_Print("-[%s:%d] String ref down(%s) Proc(0x%x) ref(%d)\r\n", __FUNCTION__, __LINE__, pStrItem->strid, pProcItem->procId, pStrItem->nRefCnt);
				}
			}
			else
			{
				#if defined(CONFIG_PROD_OS_EMULATOR)
					HxLOG_Assert(0);
				#endif
				ONDK_Error("[%s:%d] error pair string resource, strid(%s)\r\n", __FUNCTION__, __LINE__, pStrItem->strid);
			}
		}

		pList = HLIB_LIST_Next(pList);
	}

	if(NULL == str_id)
	{
		ONDK_Print("-[%s:%d] Remove Proc(0x%x)\r\n", __FUNCTION__, __LINE__, pProcItem->procId);

		pProcItem->pUsingStrItemList = HLIB_LIST_RemoveAll (pProcItem->pUsingStrItemList);
		HxSTD_memset(pProcItem, 0, sizeof(StringProcItem_t));
	}
}

STATIC void _strings_Dump_UsingItem(void *procId)
{
	StringsMgr_t 		*pStrMgr = s_pStrMgr;

	HINT32				i = 0;
	HxList_t			*pList = NULL;
	StringItem_t		*pStrItem = NULL;


	for(i=0; i < pStrMgr->stSettings.nProcMaxCount; i++)
	{
		if(NULL != procId
			&& pStrMgr->pstArrProcItem[i].procId != procId)
		{
			continue;
		}

		pList = pStrMgr->pstArrProcItem[i].pUsingStrItemList;
		if(NULL == pList)
		{
			continue;
		}

		while(pList)
		{
			pStrItem = HLIB_LIST_Data(pList);
			if(NULL == pStrItem)
			{
				pList = HLIB_LIST_Next(pList);
				continue;
			}
			if(NULL == procId)
			{
				HLIB_CMD_Printf("__str__ Proc(0x%x) ref(%d) ID(%s) Value(%s)\r\n", pStrMgr->pstArrProcItem[i].procId, pStrItem->nRefCnt, pStrItem->strid, pStrItem->strvalue);
			}
			else
			{
				HLIB_CMD_Printf("__str__ ref(%d) ID(%s) \r\n", pStrItem->nRefCnt, pStrItem->strid);
			}
			pList = HLIB_LIST_Next(pList);
		}
	}

	return;
}



/*************************************************************************************************************/
#define ___________FUNCTION_API__________________________________________________________________________
/*************************************************************************************************************/

ONDK_Result_t ONDK_SetResourceFile(HUINT8 *valuelistFile)
{
	StringsMgr_t 	*pStrMgr = s_pStrMgr;

	HERROR 		hErr = ERR_OK;

	HxFILE_t 	pStrListFile = NULL;
	HxFILE_t 	pValueListFile = NULL;

	HUINT8		*pStrFileBuf = NULL;
	HUINT8		*pValFileBuf = NULL;

	StringItem_t *pStrItem = NULL;

	VK_SEM_Get(s_ulStrSem);

	pStrFileBuf = ONDK_Malloc(sizeof(HCHAR) * MAX_CHARS_BUFFER_SIZE);
	pValFileBuf = ONDK_Malloc(sizeof(HCHAR) * MAX_CHARS_BUFFER_SIZE);

	if(pStrFileBuf == NULL || pValFileBuf == NULL)
	{
		ONDK_Error("[%s:%d] Allocation error!! \n", __FUNCTION__, __LINE__);
		hErr = ERR_FAIL;
		goto ERROR_OUT;
	}

	pStrListFile = HLIB_FILE_Open(pStrMgr->organizeStringFile, "rb");
	if(pStrListFile == NULL)
	{
		ONDK_Error("[%s:%d] strlistFile(%s) error!! \n", __FUNCTION__, __LINE__, pStrMgr->organizeStringFile);
		hErr = ERR_FAIL;
		goto ERROR_OUT;
	}
	pValueListFile = HLIB_FILE_Open(valuelistFile, "rb");
	if(pValueListFile == NULL)
	{
		ONDK_Error("[%s:%d] valuelistFile error!! \n", __FUNCTION__, __LINE__);
		hErr = ERR_FAIL;
		goto ERROR_OUT;
	}

	if(pStrMgr->stringHash != NULL)
	{
		HLIB_HASH_Destroy(pStrMgr->stringHash);
		pStrMgr->stringHash = NULL;
	}
	pStrMgr->stringHash	=  HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, _strings_Delete_StrItem);
	ONDK_Assert(pStrMgr->stringHash);

	while(HLIB_FILE_EOF(pStrListFile) == 0 && HLIB_FILE_EOF(pValueListFile) == 0)
	{
		hErr = _strings_GetStringByFile(pStrListFile, pStrFileBuf, MAX_CHARS_BUFFER_SIZE);
		if(hErr != ERR_OK)
		{
			hErr = ERR_OK;
			goto ERROR_OUT;
		}

		hErr = _strings_GetStringByFile(pValueListFile, pValFileBuf, MAX_CHARS_BUFFER_SIZE);
		if(hErr != ERR_OK)
		{
			hErr = ERR_OK;
			goto ERROR_OUT;
		}

		pStrItem = _strings_New_StrItem(pStrFileBuf, pValFileBuf);
		if(NULL == pStrItem)
		{
			ONDK_Error("[%s:%d] out of mem!! \n", __FUNCTION__, __LINE__);
			hErr = ERR_FAIL;
			goto ERROR_OUT;
		}

		ONDK_Print("String-Load) ID[%s],Value[%s]\n",pStrItem->strid,pStrItem->strvalue);
		HLIB_HASH_Insert(pStrMgr->stringHash, pStrItem->strid, pStrItem);
	}

	hErr = ERR_OK;

ERROR_OUT:
	if(pStrListFile) 	HLIB_FILE_Close(pStrListFile);
	if(pValueListFile)	HLIB_FILE_Close(pValueListFile);

	if(pStrFileBuf)		ONDK_Free(pStrFileBuf);
	if(pValFileBuf)		ONDK_Free(pValFileBuf);

	VK_SEM_Release(s_ulStrSem);

	if(hErr == ERR_FAIL)
	{
		ONDK_Error("[%s:%d] error string!!\r\n", __FUNCTION__, __LINE__);
	}

	return hErr;
}


ONDK_Result_t ONDK_SetLanguage(HINT8 *language)
{
	ONDK_Result_t	oRet = ONDK_RET_OK;
	HCHAR			szPath[HxPATH_MAX] ={0,};
	StringsMgr_t 	*pStrMgr = s_pStrMgr;

	ONDK_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if(language == NULL)
	{
		ONDK_Error("[%s:%d] Input text is Null.\n", __FUNCTION__, __LINE__);
		return ONDK_RET_FAIL;
	}

	VK_SEM_Get(s_ulStrSem);

	if(s_pStrMgr != NULL)
	{
		HLIB_STD_MemCpySafe(pStrMgr->language, 4, language, sizeof(HCHAR) * 4);
		HxSTD_snprintf(szPath,HxPATH_MAX,"%s%s%s", ONDK_STRINGFILE_HEADERNAME, language, ONDK_STRINGFILE_EXTENTION);
		if(HLIB_FILE_Exist(szPath) == FALSE)
		{
			HLIB_STD_MemCpySafe(pStrMgr->language, 4, STRGEN_DEFAULT_LANGUAGE, sizeof(HCHAR) * 4);
			HxSTD_MemSet(szPath, 0, sizeof(szPath));
			HxSTD_snprintf(szPath,HxPATH_MAX,"%s%s%s", ONDK_STRINGFILE_HEADERNAME, STRGEN_DEFAULT_LANGUAGE, ONDK_STRINGFILE_EXTENTION);
		}
	}

	VK_SEM_Release(s_ulStrSem);
	oRet = ONDK_SetResourceFile(szPath);
	return oRet;
}

void ONDK_ReleaseString(HINT8 *str_id)
{
	VK_SEM_Get(s_ulStrSem);
	_strings_Remove_UsingItem(str_id, NULL);
	VK_SEM_Release(s_ulStrSem);
}

void ONDK_ReleaseStringByProcId(HUINT32 procId)
{
	VK_SEM_Get(s_ulStrSem);
	_strings_Remove_UsingItem(NULL, (void *)procId);
	VK_SEM_Release(s_ulStrSem);
}

HUINT8 *ONDK_GetString(HINT8 *str_id)
{
	HCHAR				*pStrBuf = NULL;
	StringsMgr_t 		*pStrMgr = s_pStrMgr;
	StringItem_t 		*pStrItem = NULL;

	ONDK_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if(NULL == pStrMgr)
	{
		ONDK_Error("[%s:%d] pStrMgr == NULL\n", __FUNCTION__, __LINE__);
		return str_id;
	}

	VK_SEM_Get(s_ulStrSem);

	pStrItem = (StringItem_t *) HLIB_HASH_Lookup(pStrMgr->stringHash, str_id);
	if(pStrItem != NULL)
	{
		_strings_Append_UsingItem(pStrItem, NULL);
		pStrBuf = pStrItem->strvalue;
	}
	VK_SEM_Release(s_ulStrSem);

	ONDK_Info("[%s:%d] pStrBuf(%s) (-)\n", __FUNCTION__, __LINE__, pStrBuf);

	if(NULL == pStrBuf)
	{
		ONDK_Error("================================================\n");
		ONDK_Error("[%s:%d] Can't find Str id(%s)\n", __FUNCTION__, __LINE__, str_id);
		ONDK_Error("================================================\n");
	}

	return pStrBuf;
}

ONDK_Result_t  ONDK_STRINGS_Settings(ONDKSTR_Settings_t *pSettings)
{
	StringsMgr_t 		*pStrMgr = s_pStrMgr;

	if( NULL == pStrMgr )
	{
		ONDK_Error("[%s:%d] non-init error!!\n", __FUNCTION__, __LINE__);
		return ONDK_RET_INITFAIL;
	}

	HxSTD_MemCopy(&pStrMgr->stSettings, pSettings, sizeof(ONDKSTR_Settings_t));
	pStrMgr->pstArrProcItem	= ONDK_Calloc(sizeof(StringProcItem_t) * pSettings->nProcMaxCount);
	ONDK_Assert (pStrMgr->pstArrProcItem);

	return ONDK_RET_OK;
}

/*
 * String을 사용하기 위해서는 eSQC_SCENARIO init을 해야한다.
 */
ONDK_Result_t  ONDK_STRINGS_Init(void)
{
	HERROR 			hErr = ERR_OK;
	ONDK_Result_t	oRet = ONDK_RET_OK;
	ONDK_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);


	s_pStrMgr = (StringsMgr_t *)ONDK_Malloc(sizeof(StringsMgr_t));
	ONDK_Assert(s_pStrMgr);

	HxSTD_MemSet(s_pStrMgr, 0, sizeof(StringsMgr_t));

	hErr = VK_SEM_Create(&s_ulStrSem,"ONDK_STR", VK_SUSPENDTYPE_FIFO);
	if(hErr != ERR_OK)
	{
		ONDK_Error("[%s:%d] init error!!\n", __FUNCTION__, __LINE__);
		return ONDK_RET_INITFAIL;
	}

	HxSTD_StrNCpy(s_pStrMgr->fileNameHeader, ONDK_STRINGFILE_HEADERNAME, ONDK_FILENAME_MAX_LENGTH);
	HxSTD_StrNCpy(s_pStrMgr->fileNameExt, ONDK_STRINGFILE_EXTENTION, ONDK_FILEEXT_MAX_LENGTH);
	HxSTD_snprintf(s_pStrMgr->organizeStringFile, ONDK_FILENAME_MAX_LENGTH, "%sstrid.%s", ONDK_STRINGFILE_HEADERNAME, ONDK_STRINGFILE_EXTENTION);

	oRet = _strings_GetDB(s_pStrMgr, STRGEN_DEFAULT_LANGUAGE);
	if(oRet != ONDK_RET_OK)
	{
		ONDK_Error("[%s:%d] init error!!\n", __FUNCTION__, __LINE__);
		return ONDK_RET_INITFAIL;
	}
	ONDK_Print("[%s:%d] selected language (%s)\n", __FUNCTION__, __LINE__, s_pStrMgr->language);

	oRet = ONDK_SetLanguage(s_pStrMgr->language);
	if(oRet != ONDK_RET_OK)
	{
		ONDK_Error("[%s:%d] init error!!\n", __FUNCTION__, __LINE__);
		return ONDK_RET_INITFAIL;
	}

	ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}

void DBG_ONDK_STRINGS_Dump(HUINT32 procId)
{
	StringsMgr_t 		*pStrMgr = s_pStrMgr;

	if( NULL == pStrMgr )
	{
		return;
	}

	VK_SEM_Get(s_ulStrSem);
	_strings_Dump_UsingItem((void *)procId);
	VK_SEM_Release(s_ulStrSem);
}

