
#include <dlib.h>
#include "_dlib.h"

#include "port/_dlib_port.h"

#define	DLIB_RECYCLE_BIN_CAP	100
#define	DLIB_MAX_TAG_COUNT		20

typedef struct
{
	HxHASH_t	*map;
	HxHASH_t	*enum2str;
	HxHASH_t	*str2enum;
	//HxHASH_t	*enum2str_dep;			//	for keeping compatibility db value table.
	HxHASH_t	*str2enum_dep;			//	for keeping compatibility db value table.
	HxHASH_t	*decmodule;
	HxHASH_t	*encmodule;
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	HxQueue_t		*recycleBin;
#endif
	HCHAR			tagBuf[DxLIB_MAX_NAME];
	HCHAR 			*tags[DLIB_MAX_TAG_COUNT + 1];
} DxObjectMgr_t;

static DxObjectMgr_t	*s_pObjMgr;


#ifdef	CONFIG_DEBUG
static HBOOL dlib_debug_objects(const HCHAR * objectName, const HCHAR * tags[], void * userdata)
{
	HINT32	i;

	(void)userdata;
	HxLOG_Debug("DLIB_Init DONE(%s)\n", objectName);
	for (i = 0 ; tags[i] ; i++)
		HxLOG_Debug("\tTAG[%d]: \"%s\"\n", i, tags[i]);
	return FALSE;
}

static void	dlib_cmd_init(void)
{
}
#endif

#define	DLIB_PORT_Init()	(void)dlib_GetManager()
static DxObjectMgr_t *	dlib_GetManager (void)
{
	if (s_pObjMgr == NULL)
	{
		s_pObjMgr = (DxObjectMgr_t *)DLIB_Calloc(sizeof(DxObjectMgr_t));
		if (s_pObjMgr)
		{
			HLIB_CP_Init();
			s_pObjMgr->map = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, (_HxDestroyNotify)DLIB_Free_CB);
			HxLOG_Assert(s_pObjMgr->map);

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
			s_pObjMgr->recycleBin = HLIB_QUEUE_NewEx(DLIB_RECYCLE_BIN_CAP, TRUE, (HxQueue_RemoveCallback_t)DLIB_Free_CB);
#endif

			s_pObjMgr->enum2str = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, NULL);
			s_pObjMgr->str2enum = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, NULL);

			//s_pObjMgr->enum2str_dep = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, (_HxDestroyNotify)DLIB_Free_CB);
			s_pObjMgr->str2enum_dep = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, NULL);

			s_pObjMgr->decmodule = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, NULL);
			s_pObjMgr->encmodule = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, NULL);

			DLIB_BASE_Init();
			// 아마도 DLIB_Init()을 귀찮아서 호출하지 않을 것임...
			// 그래서 그냥 초기화 시점에 여기서 PORT Init을 호출하도록...
			(void) DxLIB_PORT_Init();

			#ifdef	CONFIG_DEBUG
				DLIB_ForEachObject(dlib_debug_objects, NULL);
				dlib_cmd_init();
			#endif
		}
	}
	return s_pObjMgr;
}


static DxObject_t *		dlib_GetObject (const HCHAR *typeName)
{
	return (DxObject_t *)HLIB_HASH_Lookup(dlib_GetManager()->map, (const void *)typeName);
}

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
static void			dlib_FlushRecycleBin (void)
{
	HxQueue_t	*recycleBin = dlib_GetManager()->recycleBin;

	while (HLIB_QUEUE_Length(recycleBin) > DLIB_RECYCLE_BIN_CAP)
	{
		void *trash = HLIB_QUEUE_Pop(recycleBin);
		if (trash)
			DLIB_Free(trash);
	}
}

static void	*		dlib_PutTrashToRecycleBin (void *trash)
{
	if (trash)
	{
		dlib_FlushRecycleBin();

		HLIB_QUEUE_Push(dlib_GetManager()->recycleBin, trash);
	}
	return trash;
}

static DxField_t *	dlib_PutFieldToRecycleBin (DxField_t *field)
{
	HxQueue_t	*recycleBin = dlib_GetManager()->recycleBin;

	dlib_FlushRecycleBin();

	switch (field->type)
	{
	case eDxTYPE_STRING:
		HLIB_QUEUE_Push(recycleBin, field->u.string);
		break;

	case eDxTYPE_BINARY:
		HLIB_QUEUE_Push(recycleBin, field->u.binary.data);
		break;

	default:
		break;
	}
	HLIB_QUEUE_Push(recycleBin, field);

	return field;
}
#endif

#ifdef CONFIG_DEBUG
// sorted alphabetically!!
static const HCHAR *s_sqliteKeyword[] = {
	"ABORT", "ACTION", "ADD", "AFTER", "ALL", "ALTER", "ANALYZE", "AND", "AS", "ASC", "ATTACH",
	"AUTOINCREMENT", "BEFORE", "BEGIN", "BETWEEN", "BY", "CASCADE", "CASE", "CAST", "CHECK",
	"COLLATE", "COLUMN", "COMMIT", "CONFLICT", "CONSTRAINT", "CREATE", "CROSS", "CURRENT_DATE",
	"CURRENT_TIME", "CURRENT_TIMESTAMP", "DATABASE", "DEFAULT", "DEFERRABLE", "DEFERRED", "DELETE",
	"DESC", "DETACH", "DISTINCT", "DROP", "EACH", "ELSE", "END", "ESCAPE", "EXCEPT", "EXCLUSIVE",
	"EXISTS", "EXPLAIN", "FAIL", "FOR", "FOREIGN", "FROM", "FULL", "GLOB", "GROUP", "HAVING",
	"IF", "IGNORE", "IMMEDIATE", "IN", "INDEX", "INDEXED", "INITIALLY", "INNER", "INSERT",
	"INSTEAD", "INTERSECT", "INTO", "IS", "ISNULL", "JOIN", "KEY", "LEFT", "LIKE", "LIMIT",
	"MATCH", "NATURAL", "NO", "NOT", "NOTNULL", "NULL", "OF", "OFFSET", "ON", "OR", "ORDER",
	"OUTER", "PLAN", "PRAGMA", "PRIMARY", "QUERY", "RAISE", "REFERENCES", "REGEXP", "REINDEX",
	"RELEASE", "RENAME", "REPLACE", "RESTRICT", "RIGHT", "ROLLBACK", "ROW", "SAVEPOINT", "SELECT",
	"SET", "TABLE", "TEMP", "TEMPORARY", "THEN", "TO", "TRANSACTION", "TRIGGER", "UNION", "UNIQUE",
	"UPDATE", "USING", "VACUUM", "VALUES", "VIEW", "VIRTUAL", "WHEN", "WHERE"
};
static HINT32	dlib_CompareFieldName (const void *arg1, const void *arg2)
{
	return HLIB_STD_StrCaseCmp(*(const HCHAR **)arg1, *(const HCHAR **)arg2);
}
static HBOOL	dlib_VerifyFieldName (const HCHAR *name)
{
	if (bsearch(&name, s_sqliteKeyword, sizeof(s_sqliteKeyword) / sizeof(s_sqliteKeyword[0]), sizeof(HCHAR **), dlib_CompareFieldName) != NULL)
	{
		return FALSE;
	}
	return TRUE;
}
#endif

static void	dlib_AddEnumTable(DxObjectMgr_t	*mgr, const DxEnumStringTable_t *pstEnumTable, HBOOL bDeprecated)
{
	HCHAR			szNameValue[128];
	HCHAR			szKeyValue [128];
	const DxEnumItem_t	*pstEnumItem;

	if (pstEnumTable == NULL)
	{
		HxLOG_Warning("Input Table is NULL\n");
		return;
	}

	HxLOG_Debug("Add ENUM Type : [%s]\n", pstEnumTable->enumType);

	pstEnumItem = pstEnumTable->pstEnumItems;
	while (pstEnumItem->enumName)
	{
		snprintf(szKeyValue , 127, "%s.%d", pstEnumTable->enumType, pstEnumItem->enumValue);
		snprintf(szNameValue, 127, "%s.%s", pstEnumTable->enumType, pstEnumItem->enumName);

		HxLOG_Info("enumName [%s] value [%d] szKeyValue [%s]\n", pstEnumItem->enumName, pstEnumItem->enumValue, szKeyValue);
		if (bDeprecated == TRUE)
		{
			//HLIB_HASH_Insert(mgr->enum2str_dep, (void*)DLIB_StrDup(szKeyValue) , (void*)pstEnumItem->enumName);
			HLIB_HASH_Insert(mgr->str2enum_dep, (void*)DLIB_StrDup(szNameValue), (void*)pstEnumItem->enumValue);
		} else
		{
			HLIB_HASH_Insert(mgr->enum2str, (void*)DLIB_StrDup(szKeyValue) , (void*)pstEnumItem->enumName);
			HLIB_HASH_Insert(mgr->str2enum, (void*)DLIB_StrDup(szNameValue), (void*)pstEnumItem->enumValue);
		}
		pstEnumItem++;
	}
}

static HINT32	dlib_GetTags (const HCHAR *tags, HCHAR *tagBuf, HCHAR **tagArray)
{
	HCHAR	*token, *save = NULL;
	HINT32	n = 0;

	HLIB_STD_StrNCpySafe(tagBuf, tags, DxLIB_MAX_NAME);

	token = HxSTD_StrTok(tagBuf, ";", &save);
	while (token && n < DLIB_MAX_TAG_COUNT)
	{
		tagArray[n++] = token;
		token = HxSTD_StrTok(NULL, ";", &save);
	}

	tagArray[n] = NULL;
	return n;
}


HERROR	DLIB_Init (void)
{
	HxLOG_Trace();

	DLIB_PORT_Init();

#ifdef	CONFIG_DEBUG
	DLIB_ForEachObject(dlib_debug_objects, NULL);
#endif
	return ERR_OK;
}

const HCHAR *	DLIB_GetDBPath (const HCHAR *typeName, HINT32 dupIndex)
{
	DxObject_t	*object;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->getDBFullName == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return NULL;
	}
	return object->getDBFullName(dupIndex);
}

HUINT32			DLIB_GetSyncTime (const HCHAR *typeName)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return 0;
	}
	if (object->getSyncTime == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return 0;		//	manual sync
	}
	return object->getSyncTime();
}

HUINT32			DLIB_GetThreshold (const HCHAR *typeName)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return 0;
	}
	if (object->getThreshold == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return 0;		// infinite
	}
	return object->getThreshold();
}

HUINT64			DLIB_GetThresholdSize (const HCHAR *typeName)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return 0;
	}
	if (object->getThresholdSize == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return 0;		// infinite
	}
	return object->getThresholdSize();
}

HUINT32			DLIB_GetPushCycle (const HCHAR *typeName)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return 0;
	}
	if (object->getPushCycle == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return 0;
	}
	return object->getPushCycle();
}


HINT32			DLIB_GetFieldCount (const HCHAR *typeName)
{
	DxObject_t	*object;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return 0;
	}
	if (object->getFieldCount == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return 0;
	}
	return object->getFieldCount();
}


#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
const DxField_t *DLIB_GetField (const HCHAR *typeName, HINT32 index, const void *src, HUINT32 srcSize)
{
	DxObject_t	*object;
	DxField_t	*field;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);
	HxLOG_Assert(index >= 0 && index < DLIB_GetFieldCount(typeName));

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->getField == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return NULL;
	}

	field = object->getField(index, src, srcSize);
	if (field == NULL)
	{
		HxLOG_Debug("%s(%s) getField fail!\n", __FUNCTION__, typeName);
		return NULL;
	}
	return dlib_PutFieldToRecycleBin(field);
}
#else
const DxField_t *DLIB_CreateGetFieldData (const HCHAR *typeName, HINT32 index, const void *src, HUINT32 srcSize)
{
	DxObject_t	*object;
	DxField_t	*field;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);
	HxLOG_Assert(index >= 0 && index < DLIB_GetFieldCount(typeName));

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->getField == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return NULL;
	}

	field = object->getField(index, src, srcSize);
	if (field == NULL)
	{
		HxLOG_Debug("%s(%s) getField fail!\n", __FUNCTION__, typeName);
		return NULL;
	}
	return field;
}

HERROR DLIB_DestroyGetFieldData (const DxField_t *field)
{
	if (NULL == field)
	{
		return ERR_FAIL;
	}

	switch (field->type)
	{
	case eDxTYPE_STRING:
		if (NULL != field->u.string)
		{
			DLIB_Free(field->u.string);
		}

		break;

	case eDxTYPE_BINARY:
		if (NULL != field->u.binary.data)
		{
			DLIB_Free(field->u.binary.data);
		}
		break;

	default:
		break;
	}

	DLIB_Free((void *)field);
	return ERR_OK;
}
#endif

const HCHAR*	DLIB_GetDataPath(const HCHAR *typeName)

{
	DxObject_t	*object;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->getDataPath == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return NULL;
	}
	return object->getDataPath();
}

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
HERROR			DLIB_Encode (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize)
{
	DxObject_t	*object;
	HUINT32		tempSize;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0 && dst);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->encode == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (dstSize == NULL)
		dstSize = &tempSize;

	err = object->encode(uid, src, srcSize, &ret, dstSize);
	if (err == ERR_OK)
	{
		*dst = dlib_PutTrashToRecycleBin(ret);
	}
	return err;
}
#else
HERROR			DLIB_CreateEncodedData (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize)
{
	DxObject_t	*object;
	HUINT32		tempSize;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0 && dst);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->encode == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (dstSize == NULL)
		dstSize = &tempSize;

	err = object->encode(uid, src, srcSize, &ret, dstSize);
	if (err == ERR_OK)
	{
		*dst = ret;
	}
	return err;
}

HERROR			DLIB_DestroyEncodedData (const void *dst)
{
	if (NULL == dst)
	{
		return ERR_FAIL;
	}

	DLIB_Free((void *)dst);
	return ERR_OK;
}
#endif

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
HERROR			DLIB_Decode (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize)
{
	DxObject_t	*object;
	HUINT32 	tempSize;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0 && dst);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->decode == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (dstSize == NULL)
		dstSize = &tempSize;

	err = object->decode(uid, src, srcSize, &ret, dstSize);
	if (err == ERR_OK)
	{
		*dst = dlib_PutTrashToRecycleBin(ret);
	}
	return err;
}
#else
HERROR			DLIB_CreateDecodedData (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize)
{
	DxObject_t	*object;
	HUINT32 	tempSize;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0 && dst);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->decode == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (dstSize == NULL)
		dstSize = &tempSize;

	err = object->decode(uid, src, srcSize, &ret, dstSize);
	if (err == ERR_OK)
	{
		*dst = ret;
	}
	return err;
}

HERROR			DLIB_DestroyDecodedData (const HCHAR *typeName, const void *dst)
{
	if (HxSTD_StrEmpty(typeName) || (NULL == dst))
	{
		return ERR_FAIL;
	}

	DLIB_FreeType(typeName, (void *)dst);
	return ERR_OK;
}

#endif

HERROR			DLIB_Serialize (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize)
{
	DxObject_t	*object;
	HUINT32		tempSize;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0 && dst);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->serialize == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}

	if (dstSize == NULL)
		dstSize = &tempSize;

	err = object->serialize(uid, src, srcSize, &ret, dstSize);
	if (err == ERR_OK)
	{
		*dst = ret;
	}

	return err;
}

HERROR			DLIB_Deserialize (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize)
{
	DxObject_t	*object;
	HUINT32 	tempSize;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0 && dst);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->deserialize == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (dstSize == NULL)
		dstSize = &tempSize;

	err = object->deserialize(uid, src, srcSize, &ret, dstSize);
	if (err == ERR_OK)
	{
		*dst = ret;
	}

	return err;
}

HBOOL			DLIB_HasTag (const HCHAR *typeName, const HCHAR *tag)
{
	DxObject_t		*object;
	HINT32			i, n;
	HCHAR			*tagBuf = NULL;
	HCHAR 			*tagArray[DLIB_MAX_TAG_COUNT + 1];
	HBOOL			 bRet = FALSE;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && tag);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		goto END_FUNC;
	}

	tagBuf = (HCHAR *)HLIB_STD_MemAlloc(DxLIB_MAX_NAME);
	if (NULL == tagBuf)
	{
		HxLOG_Warning("memory allocation failed:\n");
		goto END_FUNC;
	}

	n = dlib_GetTags(object->tags, tagBuf, tagArray);
	for (i = 0 ; i < n ; i++)
	{
		if (HLIB_STD_StrCaseCmp(tagArray[i], tag) == 0)
		{
			bRet = TRUE;
			goto END_FUNC;
		}
	}

END_FUNC:
	if (NULL != tagBuf)					{ HLIB_STD_MemFree(tagBuf); }
	return bRet;
}

HERROR			DLIB_GetLinkName(const HCHAR *typeName, HUID uid, const void *pvData, HUINT32 ulSize, void *pvName, HUINT32 ulNameSize)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->getLinkName == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	return object->getLinkName(uid, pvData, ulSize, pvName, ulNameSize);

}


HERROR			DLIB_WriteData(const HCHAR *typeName, HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 ulSize)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->writeData == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	return object->writeData(uid, pszLink, pvData, ulSize);
}

HERROR			DLIB_ReadData(const HCHAR *typeName, HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pulSize)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->readData == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	return object->readData(uid, pszLink, ppvData, pulSize);
}

HERROR			DLIB_SyncData(const HCHAR *typeName)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	if (object->syncData == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return ERR_FAIL;
	}
	return object->syncData();
}

void *			DLIB_CloneType (const HCHAR *typeName, void *src, HINT32 srcSize)
{
	DxObject_t	*object;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->cloneType == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return DLIB_MemDup(src, srcSize);
	}

	ret = object->cloneType(src, srcSize);
	if (ret == NULL)
	{
		HxLOG_Warning("%s(%s) Cannot make type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	return ret;
}

void *			DLIB_NewType (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize)
{
	DxObject_t	*object;
	HERROR		err;
	void		*ret;

//	HxLOG_Trace();
	HxLOG_Assert(typeName && src && srcSize > 0);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->makeType == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return DLIB_MemDup(src, srcSize);
	}

	err = object->makeType(uid, src, srcSize, &ret);
	if (err != ERR_OK)
	{
		HxLOG_Warning("%s(%s) Cannot make type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	return ret;
}

void			DLIB_FreeType (const HCHAR *typeName, void *type)
{
	DxObject_t	*object;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);

	if (type)
	{
		object = dlib_GetObject(typeName);
		if (object == NULL)
		{
			HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
			return;
		}
		if (object->freeType == NULL)
		{
			HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
			DLIB_Free(type);
			return;
		}

		object->freeType(type);
	}
}

DxFreeFunc_t	DLIB_GetFreeFunc (const HCHAR *typeName)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}
	if (object->freeType == NULL)
	{
		return (DxFreeFunc_t)DLIB_Free_CB;
	}
	return (DxFreeFunc_t)object->freeType;
}

void			DLIB_ForEachObject(HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata)
{
	DxObject_t		*object;
	DxObjectMgr_t	*mgr;
	HxHASH_TableIter_t	iter;
	HCHAR			*tagBuf = NULL;
	HCHAR 			*tagArray[DLIB_MAX_TAG_COUNT + 1];

	HxLOG_Trace();
	HxLOG_Assert(func);

	tagBuf = (HCHAR *)HLIB_STD_MemAlloc(DxLIB_MAX_NAME);
	if (NULL == tagBuf)
	{
		HxLOG_Warning("memory allocation failed:\n");
		return;
	}

	mgr = dlib_GetManager();
	HLIB_HASH_IterInit(&iter, mgr->map);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&object))
	{
		HxSTD_MemSet(tagArray, 0, sizeof(HCHAR *) * (DLIB_MAX_TAG_COUNT + 1));
		(void)dlib_GetTags(object->tags, tagBuf, tagArray);
		if (func(object->name, (const HCHAR **)tagArray, userdata) == TRUE)
			break;
	}

	HLIB_STD_MemFree(tagBuf);
}

const HCHAR*	DLIB_EncodeJSONBin(const HCHAR *typeName, HxJSONBIN_t enc, const void *pstData, HINT32 nSize)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}

	if (object->encodeJsonBin == NULL)
	{
		HLIB_JSONBIN_ENCODER_Clear(enc);

		if (HLIB_JSONBIN_ENCODER_SetBinary(enc, pstData, nSize) == FALSE)
			return NULL;

		return HLIB_JSONBIN_ENCODER_Encode(enc);
	}

	return object->encodeJsonBin(enc, pstData, nSize);
}

const void*			DLIB_DecodeJSONBin(const HCHAR *typeName, void *type, HxJSON_t json, HINT32 nBinSize)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return NULL;
	}

	if (object->decodeJsonBin == NULL)
		return HLIB_JSONBIN_DECODER_Decode(NULL, type, nBinSize, json, NULL, NULL, NULL);

	return object->decodeJsonBin(type, json, nBinSize);
}

HERROR			DLIB_OnStorageAdded (const HCHAR *typeName, DxStorage_Info_t *pstStorage)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName && pstStorage);

	object = dlib_GetObject(typeName);
	if (NULL == object)
	{
		HxLOG_Warning("(%s) Unknown type!\n", typeName);
		return ERR_FAIL;
	}

	if (NULL == object->onStorageAdded)
	{
		HxLOG_Debug("(%s) Not supported!\n", typeName);
		return ERR_FAIL;
	}

	return object->onStorageAdded(pstStorage);
}

HERROR			DLIB_OnStorageRemoved (const HCHAR *typeName, DxStorage_Info_t *pstStorage)
{
	DxObject_t	*object;

	HxLOG_Trace();
	HxLOG_Assert(typeName && pstStorage);

	object = dlib_GetObject(typeName);
	if (NULL == object)
	{
		HxLOG_Warning("(%s) Unknown type!\n", typeName);
		return ERR_FAIL;
	}

	if (NULL == object->onStorageRemoved)
	{
		HxLOG_Debug("(%s) Not supported!\n", typeName);
		return ERR_FAIL;
	}

	return object->onStorageRemoved(pstStorage);
}

const HCHAR*	DLIB_ConvertEnum2Str(const HCHAR *enumType, HINT32 enumValue)
{
	return dlib_ConvertEnum2Str(enumType, enumValue);
}

HINT32	DLIB_ConvertStr2Enum(const HCHAR *enumType, const HCHAR *enumName)
{
	return dlib_ConvertStr2Enum(enumType, enumName);
}

HINT32			DLIB_Print (const HCHAR *typeName, FILE *fp, const void *data)
{
	DxObject_t	*object;

//	HxLOG_Trace();
	HxLOG_Assert(typeName);

	object = dlib_GetObject(typeName);
	if (object == NULL)
	{
		HxLOG_Warning("%s(%s) Unknown type!\n", __FUNCTION__, typeName);
		return -1;
	}
	if (object->print == NULL)
	{
		HxLOG_Debug("%s(%s) Not supported!\n", __FUNCTION__, typeName);
		return -1;
	}
	if (fp == NULL)
		fp = stdout;
	object->print(fp, data);
	return 0;
}

/**************************************************************************************************
 * INTERNAL FUNCTION
 */
HERROR	dlib_RegisterObject (const DxObject_t *object)
{
	DxObjectMgr_t	*mgr;
	DxObject_t		*clone;

	HxLOG_Trace();
	HxLOG_Assert(object);

	mgr = dlib_GetManager();

#ifdef	CONFIG_DEBUG
	if (object->getFieldCount && object->getField)
	{
		HINT32	i, n, sd;

		n = object->getFieldCount();
		for (i = 0 ; i < n ; i++)
		{

			DxField_t *field = object->getField(i, NULL, 0);
			if (field == NULL)
				continue;

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
			dlib_PutFieldToRecycleBin(field);
#endif

			if (!dlib_VerifyFieldName(field->name))
			{
				n  = sizeof(s_sqliteKeyword) / sizeof(s_sqliteKeyword[0]);
				sd = HLIB_RWSTREAM_Open(NULL, 0);
				for (i = 0 ; i < n ; i++)
				{
					if (i % 5 == 0)
						HLIB_RWSTREAM_Print(sd, "\n\t");
					HLIB_RWSTREAM_Print(sd, "%18s", s_sqliteKeyword[i]);
				}
				HLIB_RWSTREAM_Print(sd, "\n");

				HxLOG_Error("%s(%s)\n"
					HxANSI_CODE_FULL(
						"\tDO NOT USE!!!! sqlite3 keyword as field name(%s)"
						, HxANSI_CODE_UNDERLINED
						, HxANSI_COLOR_FG_WHITE
						, HxANSI_COLOR_BG_RED
					)HxANSI_COLOR_YELLOW("%s")"\n", __FUNCTION__, object->name, field->name, (HCHAR *)HLIB_RWSTREAM_GetBuf(sd));

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
				(void)DLIB_DestroyGetFieldData((const DxField_t *)field);
#endif
				HLIB_RWSTREAM_Close(sd);
				return ERR_FAIL;
			}
		}
	}
#endif

	clone = (DxObject_t *)DLIB_MemDup(object, sizeof(DxObject_t));
	if (clone == NULL)
	{
		HxLOG_Error("%s[%d] Out of memory!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Debug(
		"%s(%s\n"
		"  , getFieldCount : %c\n"
		"  , getField      : %c\n"
		"  , encode        : %c\n"
		"  , decode        : %c\n"
		"  , makeType      : %c\n"
		"  , freeType      : %c\n"
		");\n"
		, __FUNCTION__
		, object->name
		, object->getFieldCount ? 'O' : 'X'
		, object->getField ? 'O' : 'X'
		, object->encode ? 'O' : 'X'
		, object->decode ? 'O' : 'X'
		, object->makeType ? 'O' : 'X'
		, object->freeType ? 'O' : 'X'
	);

	HLIB_HASH_Replace(mgr->map, (void *)clone->name, clone);
	if (clone->init)
		clone->init();
	return ERR_OK;
}

HERROR	dlib_RegisterEnumTable(const DxEnumStringTable_t *pstEnumTable)
{
	DxObjectMgr_t	*mgr;

	mgr = dlib_GetManager();
	HxLOG_Assert(mgr->enum2str && mgr->str2enum);

	if (pstEnumTable == NULL)
	{
		HxLOG_Warning("Input table is NULL\n");
		return ERR_OK;
	}

	dlib_AddEnumTable(mgr, pstEnumTable, FALSE);

	return ERR_OK;
}

HERROR	dlib_RegisterEnumTable_deprecated(const DxEnumStringTable_t *pstEnumTable)
{
	DxObjectMgr_t	*mgr;

	mgr = dlib_GetManager();
	HxLOG_Assert(mgr->str2enum_dep);

	if (pstEnumTable == NULL)
	{
		HxLOG_Warning("Input table is NULL\n");
		return ERR_OK;
	}

	dlib_AddEnumTable(mgr, pstEnumTable, TRUE);

	return ERR_OK;
}

const HCHAR*	dlib_ConvertEnum2Str(const HCHAR *enumType, HINT32 enumValue)
{
	HCHAR	szEnumStr[128];
	HCHAR	*pszReturn;

	DxObjectMgr_t	*mgr;

	mgr = dlib_GetManager();
	HxSTD_snprintf(szEnumStr, 127, "%s.%d", enumType, enumValue);
	HxLOG_Debug("[%s]\n", szEnumStr);

	pszReturn = (HCHAR*)HLIB_HASH_Lookup(mgr->enum2str, (void*)szEnumStr);
	if (pszReturn == NULL)
	{
		HxLOG_Critical("Can't find enumValue [%s](%d)\n", enumType, enumValue);
		return "error";
	}

	return pszReturn;
}

HINT32	dlib_ConvertStr2Enum(const HCHAR *enumType, const HCHAR *enumName)
{
	HINT32	nValue;
	HCHAR	szEnumStr[128];
	DxObjectMgr_t	*mgr;

	mgr = dlib_GetManager();

	HxSTD_snprintf(szEnumStr, 127, "%s.%s", enumType, enumName);

	if (HLIB_HASH_LookupExtended(mgr->str2enum, (void*)szEnumStr, NULL, (void**)&nValue) == TRUE)
		return nValue;

	//HxLOG_Warning("test old enum value table with [%s]\n", szEnumStr);
	return (HINT32)HLIB_HASH_Lookup(mgr->str2enum_dep, (void*)szEnumStr);
}

HERROR	dlib_RegisterDECModule(const HCHAR *decType, DxDECFunc encoder, DxDECFunc decoder)
{
	DxObjectMgr_t	*mgr;

	mgr = dlib_GetManager();

	HxLOG_Assert(mgr->encmodule && mgr->decmodule);
	HLIB_HASH_Insert(mgr->encmodule, (void*)DLIB_StrDup(decType), (void*)encoder);
	HLIB_HASH_Insert(mgr->decmodule, (void*)DLIB_StrDup(decType), (void*)decoder);

	return ERR_OK;
}

DxDECFunc	dlib_GetDECModule(const HCHAR *decType, HBOOL bEncoder)
{
	DxObjectMgr_t	*mgr;
	DxDECFunc		func;

	mgr = dlib_GetManager();
	HxLOG_Assert(mgr->encmodule && mgr->decmodule);

	if (bEncoder)
		func = (DxDECFunc)HLIB_HASH_Lookup(mgr->encmodule, decType);
	else
		func = (DxDECFunc)HLIB_HASH_Lookup(mgr->decmodule, decType);

	if (func == NULL)
		HxLOG_Critical("GetDECModule FAILED [%s]\n", decType);

	return func;
}

void		DLIB_MemFree(void *p)
{
	DLIB_Free(p);
}



