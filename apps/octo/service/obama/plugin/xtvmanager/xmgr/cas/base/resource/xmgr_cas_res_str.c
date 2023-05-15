/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */
#include <octo_common.h>

#include <xmgr_cas_res_str.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




STATIC	AP_CasResStr_t	_stCasResStrInfo;
STATIC	HCHAR			_casResourceDataPath[128] = "/usr/data/cas_string.bin";

/* *************************************************************************************************** */
#define ___Private_FUNCTION_PROTO________________________________________________________________________
/* *************************************************************************************************** */
STATIC	AP_CasResStr_t *local_casresstr_GetResourceInfo(void);
STATIC	HERROR		local_casresstr_FindLanguageIndex(HUINT8 *szLang639Code, HUINT32 *pulSubItem);
STATIC	HERROR		local_casresstr_getResourceDataById(AP_CasResStr_t *self, HUINT32 ulId, void **ppV, HUINT32 *pulSize);
STATIC	HERROR		local_casresstr_prepareGroupResource(AP_CasResStr_t *self, HUINT32 ulSelSubItem);
STATIC	HERROR		local_casresstr_getGroupDataById(AP_CasResStr_t *self, HUINT32 ulId, void **ppVPtr, HUINT32 *pulSize);

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC	AP_CasResStr_t *local_casresstr_GetResourceInfo(void)
{
	return &_stCasResStrInfo;
}

STATIC	HERROR		local_casresstr_FindLanguageIndex(HUINT8 *szLang639Code, HUINT32 *pulSubItem)
{
	HUINT32 			ulIndex;

	if(szLang639Code == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	for(ulIndex = 0; ulIndex < CAS_STRING_LANGUAGE_MAX; ulIndex++)
	{
		if(strcmp(szLang639Code, s_stStrList[ulIndex].ucLang639Code) == 0)
		{
			*pulSubItem = s_stStrList[ulIndex].nLangIndex;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC	HERROR		local_casresstr_getResourceDataById(AP_CasResStr_t *self, HUINT32 ulId, void **ppV, HUINT32 *pulSize)
{
	return local_casresstr_getGroupDataById(self, ulId, ppV, pulSize);
}

STATIC	HERROR		local_casresstr_prepareGroupResource(AP_CasResStr_t *self, HUINT32 ulSelSubItem)
{
	HINT32		i;
	HUINT8		*pucRawData = NULL, *p;
	HUINT16		usNumOfItems, usDataSize;
	HUINT32		ulNumOfSubItem, ulNumOfItems;
	HUINT32		ulId, ulPos = 0, ulDataSize, ulReturndSize, ulItemsSize;
	HUINT32		*pulGroupData;
	HINT32		nReadSize;
	HINT32		nRet;

	FILE		*fp;

	if(self == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	self->ulSelectedSubItem = ulSelSubItem;

	if (self->rawData)
	{
		OxAP_Free(self->rawData);
		self->rawData = NULL;
	}
	if (self->dataFileHandle)
	{
		OxAP_Free(self->dataFileHandle);
		self->dataFileHandle = NULL;
	}

	fp = fopen(self->dataFileName, "rb");
	if(fp == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	nRet = fseek(fp, 16, SEEK_SET);
	if(nRet != 0)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}

	nReadSize = fread(&ulNumOfSubItem, 1, sizeof(HUINT32), fp);
	if (nReadSize < 1)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}
	nReadSize = fread(&ulNumOfItems, 1, sizeof(HUINT32), fp);
	if (nReadSize < 1)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}

	usNumOfItems = (HUINT16)ulNumOfItems;

	if(ulSelSubItem > ulNumOfSubItem)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}

	for (i = 0; i < ulSelSubItem + 1; i++)
	{
		nReadSize = fread(&ulId, 1, sizeof(HUINT32), fp);
		if (nReadSize < 1)
		{
			HxLOG_Error("\n");
			goto EXIT_FUNC;
		}
		nReadSize = fread(&ulPos, 1, sizeof(HUINT32), fp);
		if (nReadSize < 1)
		{
			HxLOG_Error("\n");
			goto EXIT_FUNC;
		}
	}

	nRet = fseek(fp, ulPos, SEEK_SET);
	if(nRet != 0)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}

	nReadSize = fread(&ulId, 1, sizeof(HUINT32), fp);
	if (nReadSize < 1)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}
	nReadSize = fread(&ulDataSize, 1, sizeof(HUINT32), fp);
	if (nReadSize < 1)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}
	usDataSize = (HUINT16)ulDataSize;

	ulPos = ftell(fp);

	pucRawData = (HUINT8*)OxAP_Malloc(usDataSize+2);
	if(pucRawData == NULL)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}
	HxSTD_memset(pucRawData, 0, usDataSize);

	ulItemsSize = (HUINT32)(sizeof(HUINT32) * usNumOfItems);
	nRet = fseek(fp, ulItemsSize, SEEK_CUR);
	if(nRet != 0)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}

	ulReturndSize = fread(pucRawData, 1, usDataSize, fp);
	if(ulReturndSize == 0)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}
	pucRawData[ulReturndSize + 1] = '\0';

	p = pucRawData;

	pulGroupData = (HUINT32*)OxAP_Calloc(ulItemsSize);

	if(pulGroupData == NULL)
	{
		HxLOG_Error("\n");
		goto EXIT_FUNC;
	}

	for (i = 0; i < usNumOfItems; i++)
	{
		pulGroupData[i] = (HUINT32)p;
		HxLOG_Print("[%d][%s]\n", i, (char *)p);
		p += (strlen(p) + 1);
	}

	fclose(fp);

	self->dataFileHandle = pucRawData;
	self->rawData = pulGroupData;

	return ERR_OK;

EXIT_FUNC:
	if(pucRawData != NULL)
	{
		OxAP_Free(pucRawData);
	}

	fclose(fp);

	return ERR_FAIL;
}

STATIC	HERROR		local_casresstr_getGroupDataById(AP_CasResStr_t *self, HUINT32 ulId, void **ppVPtr, HUINT32 *pulSize)
{
	HUINT32			*pulPtr;

	if(self == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if(self->rawData == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if(self->dataFileHandle == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	pulPtr = self->rawData;

	*ppVPtr = (void*)pulPtr[ulId];
	*pulSize = strlen((HCHAR*)pulPtr[ulId]);

	return ERR_OK;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
HERROR	AP_CasResStr_Init(/*HCHAR *resourceDataPath*/void)
{
	AP_CasResStr_t	*pstCasResStrInfo;

	pstCasResStrInfo = local_casresstr_GetResourceInfo();

	//strcpy(_casResourceDataPath, resourceDataPath);
	memset(pstCasResStrInfo, 0, sizeof(AP_CasResStr_t));

	pstCasResStrInfo->dataFileName = _casResourceDataPath;

	return ERR_OK;
}

HERROR		AP_CasResStr_PrepareResource(HUINT8 *szLang639Code)
{
	HERROR			result = ERR_FAIL;
	HUINT32			ulSubItem;
	AP_CasResStr_t	*pstCasResStrInfo;

	pstCasResStrInfo = local_casresstr_GetResourceInfo();

	if(local_casresstr_FindLanguageIndex(szLang639Code, &ulSubItem) != ERR_OK)
	{
		HxLOG_Error("\n");
		ulSubItem = 0; /* 언어가 지원 되지 않는 것이면 eng를 선택해 주자.*/
	}

	result = local_casresstr_prepareGroupResource(pstCasResStrInfo, ulSubItem);

	return result;
}

HERROR		AP_CasResStr_GetResourceDataById(HUINT32 ulId, HUINT8* pszStr, HUINT32 *pulSize)
{
	AP_CasResStr_t	*pstCasResStrInfo;

	pstCasResStrInfo = local_casresstr_GetResourceInfo();

	return local_casresstr_getResourceDataById(pstCasResStrInfo, ulId,  (void**)&pszStr, pulSize);
}

HCHAR	*AP_CasResStr_GetStrRsc(HUINT32 ulId)
{
	AP_CasResStr_t	*pstCasResStrInfo;
	HUINT8			*pszStr;
	HUINT32 			ulSize;
	HERROR			hErr;

	pstCasResStrInfo = local_casresstr_GetResourceInfo();

	hErr = local_casresstr_getResourceDataById(pstCasResStrInfo, ulId,  (void**)&pszStr, &ulSize);

	if((hErr != ERR_OK) || (pszStr == NULL) || (ulSize == 0))
	{
		HxLOG_Error("\n");
	}

	return pszStr;
}

