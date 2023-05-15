#include <stdio.h>
#include <stdlib.h>
#include <rxlib.h>
#include <rxdir.h>

#include "ImgGenTool.h"



#if defined(WIN32)
#define ImgGen_SRCFILE_PATH			"../../image" //"../../image"
#define ImgGen_DSTFILE_PATH			"main/usr/image"
#else
#define ImgGen_SRCFILE_PATH			"../../image"
#define ImgGen_DSTFILE_PATH			"/usr/image"
#endif

#define ImgGen_FILECOPY_SIZE	 		(1024*128)

typedef struct tagmMgr
{
	RxHashTable_t	*pImageFolderHash;
	RxHashTable_t	*pUsingList;
} mMgr_t;


static mMgr_t* s_Mgr;
static HCHAR	*s_pszDstPath = NULL;




static HCHAR *_imgGen_util_StrToUpper(HCHAR *arg);


/*************************************************************************************************************/
#define _____imgGen_MANAGER___________________________________________________________________________
/*************************************************************************************************************/

static HERROR	_imgGen_Manager_New(void)
{
	HERROR hErr = ERR_OK;
	HUINT32 i=0;

	s_Mgr = (mMgr_t *)RxSTD_Alloc(sizeof(mMgr_t));
	ImgGenAssert(s_Mgr);
	RxSTD_MemSet(s_Mgr, 0, sizeof(mMgr_t));

	s_Mgr->pUsingList 		= RxHashTable_NewFull(RxHash_Table_String_Key, RxHash_Table_String_Equal, RxSTD_Free, RxSTD_Free);
	s_Mgr->pImageFolderHash	= RxHashTable_NewFull(RxHash_Table_String_Key, RxHash_Table_String_Equal, RxSTD_Free, RxSTD_Free);


	return hErr;

}


/*************************************************************************************************************/
#define _____imgGen_UTIL___________________________________________________________________________
/*************************************************************************************************************/
static HCHAR *_imgGen_util_StrToUpper(HCHAR *arg)
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

static HCHAR *_imgGen_util_StrToLower(HCHAR *arg)
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

static HCHAR *_imgGen_util_StrNCpy(HUINT8 *dest, HUINT8 *src, HINT32 cnt)
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


/*************************************************************************************************************/
#define _____imgGen_BUFFER___________________________________________________________________________
/*************************************************************************************************************/
#define MAX_CHARS_BUFFER_SIZE		8192

/*************************************************************************************************************/
#define ___________FUNCTION_CONVERT__________________________________________________________________________
/*************************************************************************************************************/



/*************************************************************************************************************/
#define ___________FUNCTION_STR_PARSE__________________________________________________________________________
/*************************************************************************************************************/




/*************************************************************************************************************/
#define ___________FUNCTION_Set_Data______________________________________________________________________
/*************************************************************************************************************/

static HINT32 _imgGen_Init(void)
{
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

	hErr = _imgGen_Manager_New();
	if(hErr != ERR_OK)
	{
		Rx_Error("[%s:%d] _imgGen_Manager_New error!!\n", __FUNCTION__, __LINE__);
		nRet = -300;
		goto ERROR_BREAK;
	}


ERROR_BREAK:
	RxCP_Destroy();

	return nRet;
}

static HBOOL  _imgGen_New_ImageFolderHash_DirFindCallback(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata)
{
	RxHashTable_t 	*pHash = (RxHashTable_t *)userdata;
	HCHAR			*pFolderName = NULL;
	HCHAR			pszFullPath[512] = {0, };

	if(FALSE == bIsDir)
	{
		return FALSE;
	}

	pFolderName = RxSTD_StrDup(pszName);
//	_imgGen_util_StrToUpper(pFolderName);

	if(RxHashTable_Lookup(pHash, pFolderName) == NULL)
	{
		RxSTD_sprintf(pszFullPath, "%s/%s", pszPath, pFolderName);
		RxHashTable_Insert(pHash, pFolderName, RxSTD_StrDup(pszFullPath));
	}
	else
	{
		RxSTD_Free(pFolderName);
	}
#if 0
	if(0 != pszFullPath[0])
	{
		Rx_Print("\t%s\n", pszName);
	}
#endif
	return FALSE;
}

static HINT32 _imgGen_New_ImageFolderHash(HCHAR	*pSrcPath)
{
	HINT32	ret = 0;

	ret = RxDIR_Find(pSrcPath, "*", "D", _imgGen_New_ImageFolderHash_DirFindCallback, s_Mgr->pImageFolderHash);
	if(0 > ret)
	{
		Rx_Error("[%s:%d] can't find pSrcPath(%s)\r\n", __FUNCTION__, __LINE__, pSrcPath);

		return ERR_FAIL;
	}

	return ERR_OK;
}

static void	_imgGen_New_UsingList_FolderListForeach(void* key , void* value , void* user_data)
{
	HCHAR 			imgConfig[256] = {0, };
	HCHAR 			copyKey[256] = {0, };
	HCHAR			*pValue = NULL;
	HCHAR			v = 0;
	RxHashTable_t 	*pHash = (RxHashTable_t *)user_data;

	RxSTD_StrCpy(copyKey, key);

	RxSTD_sprintf(imgConfig, "%s%s", "RESOURCE_IMGCFG_", copyKey);


	pValue = getenv(imgConfig);
	if(NULL == pValue)
	{
		_imgGen_util_StrToUpper(copyKey);
		RxSTD_sprintf(imgConfig, "%s%s", "RESOURCE_IMGCFG_", copyKey);
		pValue = getenv(imgConfig);
	}
	Rx_Print("\tcheck CFG - %-30s", imgConfig);

	if(NULL != pValue)
	{
		Rx_Print("= %s\n", pValue);
		v = pValue[0];
		if('\"' == v)
		{
			v = pValue[1];
		}

		switch(v)
		{
		case 'n':
		case '0':
		case '\"':
		case ' ':
		case 0:
			break;

		default:
			if(RxHashTable_Lookup(pHash, key) == NULL)
			{
				RxHashTable_Insert(pHash, key, value);
			}
			break;
		}
		return;
	}

	RxSTD_sprintf(imgConfig, "%s%s", "CONFIG_OP_", key);
	pValue = getenv(imgConfig);
	if(NULL != pValue)
	{
		Rx_Print("= %s\n", pValue);
		if(RxHashTable_Lookup(pHash, key) == NULL)
		{
			RxHashTable_Insert(pHash, key, value);
		}
		return;
	}

	Rx_Print("= NULL\n");
}

static HINT32 _imgGen_New_UsingList(void)
{
	RxHashTable_Foreach(s_Mgr->pImageFolderHash, _imgGen_New_UsingList_FolderListForeach, s_Mgr->pUsingList);
}

static HINT32 _imgGen_New_FileCopy(const HCHAR* src, const HCHAR* dst)
{
	RxFile_t in, out;
	HCHAR* buf = NULL;
	HUINT32 len = 0;

	if(0 == RxSTD_StrCmp(src, dst))
	{
		return 0;
	}

	buf = (char *) RxSTD_Alloc(ImgGen_FILECOPY_SIZE);
	if(NULL == buf)
	{
		Rx_Error("[%s:%d] out of memory\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	in	= RxFile_Open (src, "rb");
	if (NULL == in)
	{
		Rx_Error("[%s:%d] can not open the src_file (%s)\n", __FUNCTION__, __LINE__, src);
		return ERR_FAIL;
	}

	out = RxFile_Open(dst, "wb");
	if (NULL == out)
	{
		Rx_Error("[%s:%d] can not create the dst_file (%s)\n", __FUNCTION__, __LINE__, dst);
		return ERR_FAIL;
	}

	while ( (len = RxFile_Read(in, buf, sizeof(HCHAR), ImgGen_FILECOPY_SIZE)) != 0 )
	{
		if (RxFile_Write(out, buf, sizeof(HCHAR), len) == 0)
		{
			RxFile_Close(in);
			RxFile_Close(out);
			RxSTD_Free(buf);
			return ERR_FAIL;
		}
	}

	RxFile_Close(in);
	RxFile_Close(out);
	RxSTD_Free(buf);

	return 0;
}

static void _imgGen_Copy_Files_FoeachImgFileCallback(void* key , void* value , void* user_data)
{
	RxFile_t		pTargetFile = (RxFile_t)user_data;
	HCHAR			szFullDefine[2048] = {0, };
	HCHAR			szDefine[512] = {0, };
	HCHAR			szRealTarget[512] = {0, };
	HCHAR			*pFileName = NULL;

	Rx_Print(".");
	_imgGen_New_FileCopy(value, key);

	pFileName = RxSTD_StrRChar(value, '/');
	if(NULL == pFileName)
	{
		return;
	}

	pFileName++;
	RxSTD_StrCpy(szDefine, "RES_");
	RxSTD_StrCat(szDefine, pFileName);
	_imgGen_util_StrToUpper(szDefine);
	RxSTD_StrReplaceChar(szDefine, szDefine, '.', '_');

#if defined(WIN32)
	RxSTD_snprintf(szRealTarget, 512, "/usr/image/%s", pFileName);
#else
	RxSTD_snprintf(szRealTarget, 512, "%s/%s", ImgGen_DSTFILE_PATH, pFileName);
#endif
	RxSTD_snprintf(szFullDefine, 2048, "#define %-50s \t\"%s\"\r\n", szDefine, szRealTarget);
	RxFile_Write(pTargetFile, szFullDefine, sizeof (HUINT8), RxSTD_StrLen(szFullDefine));
}

static void _imgGen_Copy_Files_FoeachImgFileCallbackWOSvn(void* key , void* value , void* user_data)
{
	RxFile_t		pTargetFile = (RxFile_t)user_data;
	HCHAR			szFullDefine[2048] = {0, };
	HCHAR			szDefine[512] = {0, };
	HCHAR			szRealTarget[512] = {0, };
	HCHAR			*pFileName = NULL;

	Rx_Print(".");
	_imgGen_New_FileCopy(value, key);

	pFileName = RxSTD_StrRChar(value, '/');
	if(NULL == pFileName)
	{
		return;
	}

	// remove SVN files & none png files
	{
		HCHAR *pPos = RxSTD_StrStr(pFileName, ".png");
		if(pPos == NULL)
		{
			return;
		}

		pPos = RxSTD_StrStr(pFileName, ".svn");
		if(pPos != NULL)
		{
			return;
		}
	}

	pFileName++;
	RxSTD_StrCpy(szDefine, "RES_");
	RxSTD_StrCat(szDefine, pFileName);
	_imgGen_util_StrToUpper(szDefine);
	RxSTD_StrReplaceChar(szDefine, szDefine, '.', '_');

#if defined(WIN32)
	RxSTD_snprintf(szRealTarget, 512, "/usr/image/%s", pFileName);
#else
	RxSTD_snprintf(szRealTarget, 512, "%s/%s", ImgGen_DSTFILE_PATH, pFileName);
#endif
	RxSTD_snprintf(szFullDefine, 2048, "#define %-50s \t\"%s\"\r\n", szDefine, szRealTarget);
	RxFile_Write(pTargetFile, szFullDefine, sizeof (HUINT8), RxSTD_StrLen(szFullDefine));
}


static HBOOL  _imgGen_Copy_Files_FindCallback(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata)
{
	HCHAR			szSrcFull[512] = {0, };
	HCHAR			szDstFull[512] = {0, };

	RxHashTable_t 	*pHash = (RxHashTable_t *)userdata;
	if(TRUE == bIsDir)
	{
		return FALSE;
	}
	RxSTD_sprintf(szSrcFull, "%s/%s", pszPath, pszName);
	RxSTD_sprintf(szDstFull, "%s/%s", s_pszDstPath, pszName);

	RxHashTable_Insert(pHash, RxSTD_StrDup(szDstFull), RxSTD_StrDup(szSrcFull));

	return FALSE;
}


static void	_imgGen_Copy_Files_ForeachRootCallback(void* key , void* value , void* user_data)
{
	RxFile_t		pTargetFile = (RxFile_t)user_data;
	RxHashTable_t	*pImgFiles = NULL;
	HCHAR			szImgPath[512] = {0, };

	Rx_Print("\t copy directory - %s\n", value);

	pImgFiles = RxHashTable_NewFull(RxHash_Table_String_Key, RxHash_Table_String_Equal, RxSTD_Free, RxSTD_Free);
	RxDIR_Find(value, "*.*", "A", _imgGen_Copy_Files_FindCallback, pImgFiles);

	Rx_Print("\t\t [");
//	RxHashTable_Foreach(pImgFiles, _imgGen_Copy_Files_FoeachImgFileCallback, pTargetFile);
	RxHashTable_Foreach(pImgFiles, _imgGen_Copy_Files_FoeachImgFileCallbackWOSvn, pTargetFile);
	Rx_Print("]\n");

	RxHashTable_Destroy(pImgFiles);
}


static HINT32 _imgGen_Copy_Files(HCHAR *headerFullName)
{
	RxFile_t	pTargetFile = NULL;
	HCHAR		*pStrHeader = NULL;

	pTargetFile	= RxFile_Open (headerFullName, "wb");
	if (NULL == pTargetFile)
	{
		Rx_Error("[%s:%d] can not open the src_file (%s)\n", __FUNCTION__, __LINE__, headerFullName);
		return ERR_FAIL;
	}

	pStrHeader = "/******************************************************************************************/ \r\n";	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	pStrHeader = "/* This file should be generated by Humax ONDK Imggen (Ver. 2.0.0)                        */ \r\n";	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	pStrHeader = "/******************************************************************************************/ \r\n";	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
	pStrHeader = "#ifndef __IMGGEN_DEFINE_H__\r\n#define __IMGGEN_DEFINE_H__ \r\n";
	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));

	RxHashTable_Foreach(s_Mgr->pUsingList, _imgGen_Copy_Files_ForeachRootCallback, pTargetFile);

	pStrHeader = "#endif /* __IMGGEN_DEFINE_H__ */\r\n";
	RxFile_Write(pTargetFile, pStrHeader, sizeof (HUINT8), RxSTD_StrLen(pStrHeader));
}



/*************************************************************************************************************/
#define ___________FUNCTION_API__________________________________________________________________________
/*************************************************************************************************************/


/*******************************************************************/
#define ___________MAIN_______________________________________________________________________
/*************************************************************************************************************/

HINT32 main(HINT32 argc, HINT8 *argv[])
{

	HCHAR *temp_product = NULL;
	HCHAR *temp_resname = NULL;
	HCHAR *temp_headerPath = NULL;
	HCHAR *temp_pSrcPath = NULL;

	HINT32 ret = 0;
	HCHAR product[256] = {0, };
	HCHAR headerFullName[256] = {0, };
	HCHAR srcPath[256] = {0, };

	Rx_Print("********************************************************************************\r\n");
	Rx_Print("*                              ONDK Image Tools                               *\r\n");
	Rx_Print("********************************************************************************\r\n");

	temp_product  = getenv("CONFIG_PRODUCT_NAME");
	if(temp_product  == NULL)
	{
		_imgGen_util_StrNCpy(product, "hms1000sph2", 256);
	}
	else
	{
		_imgGen_util_StrNCpy(product, temp_product, 256);
	}
	Rx_Print("product name : %s\n", product);


	s_pszDstPath = getenv("RESOURCE_IMGTOOL_CFG_DSTPATH");
	if(NULL == s_pszDstPath)
	{
		s_pszDstPath = ImgGen_DSTFILE_PATH;
	}

	ret = _imgGen_Init();
	if(0 != ret)
	{
		Rx_Error("init...fail\n");
		return -1;
	}
	Rx_Print("init...Ok\n");


	RxDIR_Make(s_pszDstPath);
	Rx_Print("make folder...Ok\n");

	temp_pSrcPath = getenv("RESOURCE_IMGTOOL_CFG_SRCPATH");
	if(NULL == temp_pSrcPath)
	{
		temp_pSrcPath = ImgGen_SRCFILE_PATH;
	}
	RxSTD_StrCpy(srcPath, temp_pSrcPath);

	Rx_Print("Get Folder info...\n");
	ret = _imgGen_New_ImageFolderHash(temp_pSrcPath);
	if(0 != ret)
	{
		Rx_Error("Image pool create...fail\n");
		return -1;
	}

	_imgGen_New_UsingList();


	temp_headerPath = getenv("RESOURCE_IMGTOOL_HEADERPATH");
	if(NULL != temp_headerPath)
	{
		RxSTD_snprintf(headerFullName, 256, "%s", temp_headerPath);
	}
	else
	{
		RxSTD_snprintf(headerFullName, 256, "../../../include/res_imgid.h");
	}


	Rx_Print("Copy files...\n");
	_imgGen_Copy_Files(headerFullName);
	Rx_Print("\r\nCopy done\n");

	return 0;
}


