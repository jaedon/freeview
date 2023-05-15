/*************************************************************
 * @file		hxudb.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB UserDB API

 * http://www.humaxdigital.com
 *************************************************************/

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<errno.h>

#include 	<htype.h>
#include	<vkernel.h>

#include	"_hlib_types.h"
#include	"_hlib_stdlib.h"
#include	"_hlib_log.h"
#include	"_hlib_sem.h"
#include	"_hlib_mem.h"
#include	"_hlib_math.h"
#include	"_hlib_json.h"
#include	"_hlib_list.h"
#include	"_hlib_file.h"
#include	"_hlib_udb.h"

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN		"HxUDB"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef	struct _HxUDBInfo
{
	HCHAR		*baseFullFileName;
	HCHAR		*backupFullFileName;
	HBOOL		update;
	HUINT32		ulVersion;

	HINT32		semaphore;
	HxJSON_t	root;
	HxJSON_t	startNode;
} _HxUDBInfo_t;

#define	ENTER_CRITICAL(info)		VK_SEM_Get(info->semaphore)
#define	LEAVE_CRITICAL(info)		VK_SEM_Release(info->semaphore)

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________
/* *************************************************************************************************** */
static	HxJSON_t	_hxudb_find_section(_HxUDBInfo_t *udbInfo, const HCHAR *key, HBOOL bMakeSection, HCHAR **ppKey);

static	HERROR	_hxudb_get_value(HxJSON_t section, const HCHAR *key, HUINT32 *pulValue);
static	HERROR	_hxudb_get_text(HxJSON_t section, const HCHAR *key, HCHAR *text, HINT32 n);
static	HERROR	_hxudb_get_binary(HxJSON_t section, const HCHAR *key, void *obj, HINT32 *pnSize);
static	HBOOL	_hxudb_load_data_from_file(_HxUDBInfo_t *udbInfo);
static	void	_hxudb_write_data_to_file(HCHAR *fileName, HUINT8 *pucBuffer, HINT32 size);
static	void	_hxudb_save_data_to_file(_HxUDBInfo_t *udbInfo);

static HBOOL	_hxudb_get_keyLists(void *key, void *value, void *user);
static	void	printUDBList(_HxUDBInfo_t *udbInfo);

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
HxUDBHandle	HLIB_UDB_CreateInstance(HCHAR *filePath, HCHAR *fileName, HBOOL *pbNewFile, HCHAR *backupPath, HUINT32 ulVersion)
{
	HINT32			len;
	HBOOL			needNewFile;
	_HxUDBInfo_t	*udbInfo;
	HINT32			nRet;
	HULONG			ulSemId;

	HxLOG_Assert(filePath && fileName);

	udbInfo = (_HxUDBInfo_t*)HLIB_MEM_Calloc(sizeof(_HxUDBInfo_t));
	if (NULL == udbInfo)
	{
		return NULL;
	}

	nRet = VK_SEM_Create((unsigned long *)&ulSemId, "udbSem", VK_SUSPEND_FIFO);
	if (VK_OK != nRet)
	{
		if(udbInfo != NULL)
			HLIB_MEM_Free(udbInfo);
		return NULL;
	}

	len = HxSTD_StrLen(filePath) + HxSTD_StrLen(fileName) + 4;
	udbInfo->baseFullFileName = (HCHAR*)HLIB_MEM_Calloc(len);
	HxSTD_snprintf(udbInfo->baseFullFileName, len - 1, "%s/%s", filePath, fileName);
	if (backupPath)
	{
		len = HxSTD_StrLen(backupPath) + HxSTD_StrLen(fileName) + 4;
		udbInfo->backupFullFileName = (HCHAR*)HLIB_MEM_Calloc(len);
		HxSTD_snprintf(udbInfo->backupFullFileName, len - 1, "%s/%s", backupPath, fileName);
	}

	udbInfo->update = FALSE;
	udbInfo->ulVersion = ulVersion;
	udbInfo->semaphore = (HINT32)ulSemId;

	needNewFile = _hxudb_load_data_from_file(udbInfo);

	if (pbNewFile)
		*pbNewFile = needNewFile;

	return (HxUDBHandle)udbInfo;
}

void		HLIB_UDB_DestroyInstance(HxUDBHandle handle, HBOOL bSync)
{
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);

	if (bSync)
		_hxudb_save_data_to_file(udbInfo);

	if (udbInfo->baseFullFileName)
		HLIB_MEM_Free(udbInfo->baseFullFileName);
	if (udbInfo->backupFullFileName)
		HLIB_MEM_Free(udbInfo->backupFullFileName);

	if (udbInfo->root)
		HLIB_JSON_Delete(udbInfo->root);

	VK_SEM_Destroy(udbInfo->semaphore);

	HLIB_MEM_Free(udbInfo);

	(void)printUDBList;	// prevent compile warning!
}

void		HLIB_UDB_Sync(HxUDBHandle handle)
{
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);

	ENTER_CRITICAL(udbInfo);
	if (udbInfo->update)
		_hxudb_save_data_to_file(udbInfo);

	udbInfo->update = FALSE;

	LEAVE_CRITICAL(udbInfo);
}

void		HLIB_UDB_Remove(HxUDBHandle handle, const HCHAR *key)
{
	HCHAR			*_key;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, key, FALSE, &_key);
	if (section)
		HxJSON_Object_Remove(udbInfo->root, _key);

	udbInfo->update = TRUE;

	LEAVE_CRITICAL(udbInfo);
}

void		HLIB_UDB_RemoveAll(HxUDBHandle handle)
{
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	HLIB_JSON_Delete(udbInfo->root);
	udbInfo->root = HxJSON_Object_New();

	udbInfo->update = TRUE;

	LEAVE_CRITICAL(udbInfo);
}

HERROR			HLIB_UDB_SetStartNode(HxUDBHandle handle, const HCHAR *root_key)
{
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxJSON_t		section;
	HCHAR			*_objectName;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, root_key, FALSE, &_objectName);
	if (HxSTD_StrLen(_objectName) > 0)
		section = HxJSON_Object_Get(section, _objectName);

	if (section)
	{
		udbInfo->startNode = section;

		LEAVE_CRITICAL(udbInfo);

		return ERR_OK;
	}

	udbInfo->startNode = NULL;

	LEAVE_CRITICAL(udbInfo);

	return ERR_FAIL;
}

HERROR			HLIB_UDB_ClearStartNode(HxUDBHandle handle)
{
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	if (udbInfo->startNode)
		udbInfo->startNode = NULL;

	LEAVE_CRITICAL(udbInfo);

	return ERR_OK;
}

HINT32		HLIB_UDB_GetKeyCount(HxUDBHandle handle, const HCHAR *root_key)
{
	HCHAR			*_objectName;
	HINT32			keyCount = 0;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, root_key, FALSE, &_objectName);
	if (HxSTD_StrLen(_objectName) > 0)
		section = HxJSON_Object_Get(section, _objectName);

	if (section)
		keyCount = HLIB_JSON_GetSize(section);

	LEAVE_CRITICAL(udbInfo);

	return keyCount;
}

HxList_t*	HLIB_UDB_CreateKeyInfos(HxUDBHandle handle, const HCHAR *root_key, HINT32 *pnKeyCount)
{
	HCHAR			*_objectName;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, root_key, FALSE, &_objectName);
	if (HxSTD_StrLen(_objectName) > 0)
		section = HxJSON_Object_Get(section, _objectName);

	if (section)
	{
		HxList_t	*keyLists = NULL;

		HLIB_JSON_ForEach(section, _hxudb_get_keyLists, (void*)&keyLists);
		*pnKeyCount = HLIB_JSON_GetSize(section);

		LEAVE_CRITICAL(udbInfo);

		return keyLists;
	}

	LEAVE_CRITICAL(udbInfo);

	return NULL;
}

void		HLIB_UDB_DestroyKeyInfos(HxUDBHandle handle, HxList_t *subList)
{
	HxLOG_Assert(handle);

	HLIB_LIST_RemoveAll(subList);
}

void		HLIB_UDB_SetValue(HxUDBHandle handle, const HCHAR *key, HUINT32 ulValue)
{
	HCHAR		*_key;
	HxJSON_t	section;
	HUINT32		_ulValue;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);
	section = _hxudb_find_section(udbInfo, key, TRUE, &_key);
	if (_hxudb_get_value(section, _key, &_ulValue) == ERR_OK)
	{
		if (_ulValue == ulValue)
		{
			//	Same Data.
			LEAVE_CRITICAL(udbInfo);

			return;
		}
	}

	HxJSON_Object_Set(section, _key, HxJSON_Integer_New(ulValue));
	udbInfo->update = TRUE;
	LEAVE_CRITICAL(udbInfo);
}

HERROR		HLIB_UDB_GetValue(HxUDBHandle handle, const HCHAR *key, HUINT32 *pulValue)
{
	HCHAR			*_key;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, key, FALSE, &_key);
	if (section == NULL)
	{
		LEAVE_CRITICAL(udbInfo);

		return ERR_FAIL;
	}

	if (_hxudb_get_value(section, _key, pulValue) != ERR_OK)
	{
		LEAVE_CRITICAL(udbInfo);

		return ERR_FAIL;
	}

	LEAVE_CRITICAL(udbInfo);

	return ERR_OK;
}

void		HLIB_UDB_SetText(HxUDBHandle handle, const HCHAR *key, HCHAR *text)
{
	HCHAR			*_key;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, key, TRUE, &_key);
	HxJSON_Object_Set(section, _key, HxJSON_String_New(text));
	udbInfo->update = TRUE;
	LEAVE_CRITICAL(udbInfo);
}

HERROR		HLIB_UDB_GetText(HxUDBHandle handle, const HCHAR *key, HCHAR *text, HINT32 n)
{
	HCHAR			*_key;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, key, FALSE, &_key);
	if (section == NULL)
	{
		LEAVE_CRITICAL(udbInfo);

		return ERR_FAIL;
	}

	if (_hxudb_get_text(section, _key, text, n) != ERR_OK)
	{
		LEAVE_CRITICAL(udbInfo);

		return ERR_FAIL;
	}

	LEAVE_CRITICAL(udbInfo);

	return ERR_OK;
}

#define	ENCODE_BUFFER_SIZE		1024
static	HCHAR	__encode_buffer[ENCODE_BUFFER_SIZE];

void		HLIB_UDB_SetBinary(HxUDBHandle handle, const HCHAR *key, const void *obj, HINT32 size)
{
	HCHAR		*_key;
	HxJSON_t	section;
	HCHAR		*buf;
	HUINT32		need;

	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	need = HLIB_MATH_EncodeBase64(NULL, obj, size);
	if (need < ENCODE_BUFFER_SIZE)
		buf = __encode_buffer;
	else
		buf = (HCHAR *)HLIB_MEM_Malloc(need + 1);

	HLIB_MATH_EncodeBase64(buf, obj, size);
	buf[need] = 0;

	section = _hxudb_find_section(udbInfo, key, TRUE, &_key);
	HxJSON_Object_Set(section, _key, HxJSON_String_New(buf));

	if (buf != __encode_buffer)
		HLIB_MEM_Free(buf);

	udbInfo->update = TRUE;

	LEAVE_CRITICAL(udbInfo);
}

HERROR		HLIB_UDB_GetBinary(HxUDBHandle handle, const HCHAR *key, void *obj, HINT32 *pnSize)
{
	HCHAR			*_key;
	HxJSON_t		section;
	_HxUDBInfo_t	*udbInfo = (_HxUDBInfo_t*)handle;

	HxLOG_Assert(udbInfo);
	HxLOG_Assert(udbInfo->root);

	ENTER_CRITICAL(udbInfo);

	section = _hxudb_find_section(udbInfo, key, FALSE, &_key);
	if (section == NULL)
	{
		LEAVE_CRITICAL(udbInfo);

		return ERR_FAIL;
	}

	if (_hxudb_get_binary(section, _key, obj, pnSize) != ERR_OK)
	{
		LEAVE_CRITICAL(udbInfo);

		return ERR_FAIL;
	}

	LEAVE_CRITICAL(udbInfo);

	return ERR_OK;
}

/* *************************************************************************************************** */
#define ___PRIVATE_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
//	key     : MENU/AUDIO/LANGUAGE
//	SECTION : MENU/AUDIO
//	ITEM    : LANGUAGE
static	HxJSON_t	_hxudb_find_section(_HxUDBInfo_t *udbInfo, const HCHAR *key, HBOOL bMakeSection, HCHAR **ppKey)
{
	const	HCHAR	*p, *tok;
	HxJSON_t	root = udbInfo->root;
	HxJSON_t	object;

	HINT32		len;
	HCHAR		buf[128];

	if (udbInfo->startNode)
		root = udbInfo->startNode;

	p = key;

	while (*p == '/')
		p++;
	tok = p;

	len = 0;
	while (*p != 0)
	{
		len++;
		if (*p == '/')
		{
			HxSTD_StrNCpy(buf, tok, len);
			buf[len - 1] = 0;

			//*p = 0;
			//	do something with tok...
			if (*(p + 1) == 0)		//	end of string
			{
				return root;
			} else
			{
				//	make section or something.
				object = HxJSON_Object_Get(root, buf);
				if (object == NULL)
				{
					if (bMakeSection)
					{
						object = HxJSON_Object_New();
						HxJSON_Object_Set(root, buf, object);
					} else
						return NULL;
				} else
				if (!HxJSON_IsObject(object))
				{
					HxLOG_Critical("TOKEN[%s] is not JSON OBJECT!!! (JuSt VaLuE!!!)\n", buf);
					root = udbInfo->root;
					if (ppKey)
						*ppKey = (HCHAR*)key;

					return root;
				}

				root = object;
			}
			len = 0;
			tok = p + 1;
		}
		p++;
	}

	if (ppKey)
		*ppKey = (HCHAR*)tok;

	return root;
}

static	HERROR		_hxudb_get_value(HxJSON_t section, const HCHAR *key, HUINT32 *pulValue)
{
	HxJSON_t		object;

	object = HxJSON_Object_Get(section, key);
	if (object == NULL)
		return ERR_FAIL;

	if (!HxJSON_IsInteger(object))
		return ERR_FAIL;

	*pulValue = HLIB_JSON_Object_GetInteger(section, key);

	return ERR_OK;
}

static	HERROR		_hxudb_get_text(HxJSON_t section, const HCHAR *key, HCHAR *text, HINT32 n)
{
	const	HCHAR	*data;

	data = HLIB_JSON_Object_GetString(section, key);
	if (data == NULL)
		return ERR_FAIL;

	if (n == 0)
		HxSTD_StrCpy(text, data);
	else
	{
		HxSTD_StrNCpy(text, data, n);
		text[n - 1] = 0;
	}

	return ERR_OK;
}

static	HERROR		_hxudb_get_binary(HxJSON_t section, const HCHAR *key, void *obj, HINT32 *pnSize)
{
	const	HCHAR	*buf;
	HUINT32	need;

	buf = HLIB_JSON_Object_GetString(section, key);
	if (buf == NULL)
		return ERR_FAIL;

	need = HLIB_MATH_DecodeBase64(NULL, buf, HxSTD_StrLen(buf));
	*pnSize = need;

	HLIB_MATH_DecodeBase64(obj, buf, HxSTD_StrLen(buf));

	return ERR_OK;
}

static	HERROR	_hxudb_load_json_text(HCHAR *pszFileName, HCHAR **pphcJsonText)
{
	HCHAR		*phcData = NULL;
	HUINT32		ulLen = 0;
	HxFILE_t	file;

	file = HLIB_FILE_Open(pszFileName, "rb");

	if (file == NULL)
		return ERR_FAIL;

	if (HLIB_FILE_Size32(file, &ulLen) == ERR_OK)
	{
		if (ulLen > 0)
		{
			phcData = (HCHAR*)HLIB_MEM_Calloc(ulLen + 1);
			(void)HLIB_FILE_Read(file, phcData, 1, ulLen);
		}
	}

	HLIB_FILE_Close(file);

	*pphcJsonText = phcData;

	return ERR_OK;
}

static	HBOOL	_hxudb_load_data_from_file(_HxUDBInfo_t *udbInfo)
{
	HBOOL	bNewFile = FALSE;
	HCHAR	*aszFileName[2] = {udbInfo->baseFullFileName, udbInfo->backupFullFileName};
	HINT32	i;

	for (i = 0; i < 2; i++)
	{
#if 1		//	File Loading후에 JSON Decoder를 돌리자. (decode file상의 exception에 대한 대처)
		HCHAR	*phcJsonText = NULL;

		_hxudb_load_json_text(aszFileName[i], &phcJsonText);
		if (phcJsonText == NULL)
			continue;

		udbInfo->root = HLIB_JSON_Decode(phcJsonText);
		HLIB_MEM_Free(phcJsonText);
#else
		udbInfo->root = HLIB_JSON_DecodeFile(aszFileName[i]);
#endif
		if (udbInfo->root)
		{
			HUINT32	ulVersion;

			if (udbInfo->ulVersion == UDB_NOTUSE_VERSION)
			{
				HxJSON_Object_Remove(udbInfo->root, "version");
#if	0
				HxJSON_t	removeRoot;

				removeRoot = HxJSON_Object_Get(udbInfo->root, "version");
				if (removeRoot)
					HLIB_JSON_Remove(udbInfo->root, "version");
#endif
				HLIB_FILE_Copy(aszFileName[i % 2], aszFileName[(i + 1) % 2]);

				goto err_ok;
			} else
			{
				ulVersion = HLIB_JSON_Object_GetInteger(udbInfo->root, "version");
				if (ulVersion == udbInfo->ulVersion)
				{
					HLIB_FILE_Copy(aszFileName[i % 2], aszFileName[(i + 1) % 2]);
					goto err_ok;
				}
				HLIB_JSON_Delete(udbInfo->root);
				udbInfo->root = NULL;
			}
		}
	}

	bNewFile = TRUE;
	udbInfo->root = HxJSON_Object_New();

	if (udbInfo->ulVersion != UDB_NOTUSE_VERSION)
		HxJSON_Object_Set(udbInfo->root, "version", HxJSON_Integer_New(udbInfo->ulVersion));

err_ok:
	return bNewFile;
}

static	void	_hxudb_write_data_to_file(HCHAR *fileName, HUINT8 *pucBuffer, HINT32 size)
{
	FILE	*fp;
	HINT32	written;
	HUINT8	*p;

	//	Open한상태에서 fflush을 사용하면 더 좋을것 같으나... 그건 나중에..
	p = pucBuffer;
	fp = fopen(fileName, "wb");
	if (fp)
	{
		while (size)
		{
			written = fwrite(p, 1, size, fp);
			size = size - written;
			p += written;
		}
		fclose(fp);
	}
}

static	void	_hxudb_save_data_to_file(_HxUDBInfo_t *udbInfo)
{
	static	HCHAR	*_null_string_buffer = "{}";
	HCHAR	*buffer;

	if (access(udbInfo->baseFullFileName, R_OK | W_OK) == 0)
		HLIB_FILE_Copy(udbInfo->baseFullFileName, udbInfo->backupFullFileName);

	//	Write..
	buffer = HLIB_JSON_Encode(udbInfo->root, NULL, 0);
	if (buffer == NULL)
		buffer = _null_string_buffer;

	_hxudb_write_data_to_file(udbInfo->baseFullFileName, buffer, HxSTD_StrLen(buffer));
	if (access(udbInfo->backupFullFileName, R_OK | W_OK) != 0)
		_hxudb_write_data_to_file(udbInfo->backupFullFileName, buffer, HxSTD_StrLen(buffer));

	if (buffer != _null_string_buffer)
		HLIB_MEM_Free(buffer);
}

static HBOOL	_hxudb_get_keyLists(void *key, void *value, void *user)
{
	HxList_t	**keyAddr = (HxList_t**)user;
	HxList_t	*keyLists = *keyAddr;

	keyLists = HLIB_LIST_Append(keyLists, key);
	*keyAddr = keyLists;

	return FALSE;
}

static	void	printUDBList(_HxUDBInfo_t *udbInfo)
{
	HCHAR	*buf;

	buf = HLIB_JSON_Encode(udbInfo->root, NULL, 0);
	HxLOG_Print("buf : [%s]\n", buf);
	HLIB_MEM_Free(buf);
}


