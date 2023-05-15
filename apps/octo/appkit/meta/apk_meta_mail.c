/**************************************************************
 *	@file		apk_meta_mail.c
 *	appkit for meta mail
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/09/04
 *	@author			humax
 *	@brief			appkit for META mail
 **************************************************************/

 //2 [Todo] Appkit 2.0으로 새로 구현해야함


#if 1

// OPL 호환을 위해 dummy만 남겨둠
#include <apk.h>

void	APK_META_MAIL_Delete (void *object)
{
	return;
}

void *	APK_META_MAIL_Clone (const void *object)
{
	return NULL;
}

HERROR		APK_META_MAIL_Remove( HINT32 id )
{
	return ERR_FAIL;
}

HERROR		APK_META_MAIL_Update( DxMail_t *updateMail )
{
	return ERR_FAIL;
}

DxMail_t *	APK_META_MAIL_GetMail (HINT32 id)
{
	return NULL;
}

HxVector_t *	APK_META_MAIL_GetMailList (void)
{
	return NULL;
}

HERROR	APK_META_MAIL_LoadFromOcto (void)
{
	return ERR_FAIL;
}

void			APK_META_MAIL_RegisterNotifier ( APK_META_MAIL_Notifier_t fnNotifier, void *userdata)
{
	return;
}

void			APK_META_MAIL_UnRegisterNotifier ( void )
{
	return;
}

#else
#include	<oapi.h>

#include	<dapi.h>

#include	"meta_int.h"

#define	RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)


#define	____CODEC_API___________________________________________________________________________

#define	____PRIVATE_TYPE___________________________________________________________________________

typedef struct
{
	HCHAR	*name;
	HUINT32	size;
	HUINT32	offset;

	void  *	(* decode) (void *, HxJSON_t);
	HCHAR *	(* encode) (const void *, HxJSONBIN_t enc);

#if defined(CONFIG_DEBUG)
	void	(* print) (FILE *, const void *);
#endif

	APKS_Extension_t	*extension;
} METAMAIL_Class_t;

typedef struct
{
	HUINT32				accessCode;
	HUINT32				notifierId;

	APK_MetaMailListener_t	onUpdate;
	void *				userdata;
} METAMAIL_Listener_t;

typedef struct
{
	HxTREE_t			*classTree;	///< <METAMAIL_Class_t *>
	HxTREE_t			*listTree;	///< <HxVector_t *>
	HCHAR				*path;
	HINT32				version;	///< last version
	HxJSONBIN_t			encoder;
	METAMAIL_Listener_t	listener;
} METAMAIL_Manager_t;

static	HUINT32		_ulModuleSemaphoreId = 0;

static METAMAIL_Manager_t * metamail_GetMailManager (void);
static HUINT32		metamail_oapi_Notifier (HUINT32 accessCode, HxJSON_t param);
static HERROR		metamail_oapi_Read (METAMAIL_Manager_t *mgr, HxTREE_t **updated, HxTREE_t **removed);

static INLINE const METAMAIL_Class_t *
				metamail_ClassOfObject (void *self)
{
	return *(const METAMAIL_Class_t **)((HUINT8 *)self - sizeof(METAMAIL_Class_t *));
}

static void *	metamail_NewObject (const METAMAIL_Class_t *clazz)
{
	void *self;

	self = APK_Calloc(sizeof(METAMAIL_Class_t *) + clazz->size);
	if (self)
	{
		*(const METAMAIL_Class_t **)self = clazz;
	}
	return (void *)((HUINT8 *)self + sizeof(METAMAIL_Class_t *));
}

static void		metamail_DeleteObject (void *self)
{
	if (self)
	{
		APK_Free(((HUINT8 *)self - sizeof(METAMAIL_Class_t *)));
	}
}

static void	*	metamail_CloneObject (const void *self)
{
	const METAMAIL_Class_t *clazz;
	void		*clone;

	RETURN_IF(self == NULL, NULL);

	clazz = metamail_ClassOfObject((void *)self);
	clone = metamail_NewObject(clazz);
	if (clone)
	{
		memcpy(clone, self, clazz->size);
	}
	return clone;
}

static void *	metamail_DecodeObject (const METAMAIL_Class_t *clazz, HxJSON_t json)
{
	void *	self;

	self = metamail_NewObject(clazz);
	RETURN_IF(self == NULL, NULL);

	if (clazz->decode(self, json) == NULL)
	{
		metamail_DeleteObject(self);
		return NULL;
	}
	return self;
}

static HCHAR *	metamail_EncodeObject (HxJSONBIN_t enc, const void *self)
{
	const METAMAIL_Class_t *clazz;
	HCHAR	*json;

	RETURN_IF(self == NULL, NULL);

	clazz = metamail_ClassOfObject((void *)self);

	json = clazz->encode(self, enc);
	return json;
}

static HINT32	metamail_CompareVectorObject (const void *arg1, const void *arg2)
{
	const void *lhs = *(const void **)arg1;
	const void *rhs = *(const void **)arg2;

	return (HINT32)memcmp(lhs, rhs, sizeof(HINT32)/* ulid field */);
}

static void *	metamail_DecodeMail (void *self, HxJSON_t json)
{
	return (void *)DxMail_DecodeBin((DxMail_t *)self, json);
}

static HCHAR *	metamail_EncodeMail (const void *self, HxJSONBIN_t enc)
{
	return (HCHAR *)DxMail_EncodeBin(enc,(const DxMail_t *)self);
}

static HxVector_t *	metamail_DecodeList (const METAMAIL_Class_t *clazz, HxJSON_t array, HINT32 version)
{
	HxVector_t		*list;
	DxMail_t	*item;
	HxJSON_t		value;

	HxLOG_Trace();

	RETURN_IF(!array, NULL);

	list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))metamail_DeleteObject, metamail_CompareVectorObject);
	RETURN_IF(list == NULL, NULL);

	HxLOG_Print("[%s,%d] json type : %d \n", __FUNCTION__ , __LINE__, HLIB_JSON_TypeOf( array )  );

	HxJSON_ARRAY_EACH(array, value, {
		item = (DxMail_t *)metamail_DecodeObject(clazz, value);
		if (item == NULL)
		{
			continue;
		}

		HLIB_VECTOR_Add(list, item);

	});

	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		return NULL;
	}

	HLIB_VECTOR_Sort(list, NULL);

	return list;
}

#if 0 // for warning -> sbkim, 나중에 사용할지 몰라 남겨둔다.
static HCHAR *		metamail_EncodeList (HxJSONBIN_t enc, const METAMAIL_Class_t *clazz, HxVector_t *list)
{
	HINT32		sd, c = 0;
	void		*item;
	HCHAR		*json;

	HxLOG_Trace();

	sd  = HLIB_RWSTREAM_OpenEx(NULL, 0, APK_Malloc, APK_Free);
	RETURN_IF(!sd, NULL);

	HLIB_RWSTREAM_Print(sd, "\"%s\":[", clazz->name);

	HxVECTOR_EACH(list, void *, item, {
		json = metamail_EncodeObject(enc, item);
		if (json == NULL)
			continue;

		if (c > 0)
			HLIB_RWSTREAM_PutChar(sd, ',');

		HLIB_RWSTREAM_PutString(sd, json);
		c++;
	});

	HLIB_RWSTREAM_Print(sd, "]");
	if (c == 0)
	{
		HxLOG_Warning("[%s] Failed!!\n", __FUNCTION__);
		HLIB_RWSTREAM_Close(sd);
		return NULL;
	}
	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}
#endif

static HxTREE_t *	metamail_DecodeAll (METAMAIL_Manager_t *mgr, HxJSON_t json, HINT32 version)
{
	METAMAIL_Class_t	*clazz;
	const HCHAR		*key;
	HxJSON_t		value;
	HxVector_t		*list;
	HxTREE_t		*tree;

	HxLOG_Trace();
	RETURN_IF(!json || !HxJSON_IsObject(json), NULL);

	tree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)HLIB_VECTOR_Delete
			);
	RETURN_IF(tree == NULL, NULL);

	HxJSON_OBJECT_EACH(json, key, value, {
		if (!value)
			continue;

		clazz = (METAMAIL_Class_t *)HLIB_TREE_Lookup(mgr->classTree, (const void * )key);
		if (clazz == NULL)
			continue;

		list = metamail_DecodeList(clazz, value, version);
		if (list == NULL)
		{
			continue;
		}
		HLIB_TREE_Insert(tree, (void *)clazz->name, list);
	});

	if (HLIB_TREE_Nnodes(tree) == 0)
	{
		HLIB_TREE_Destroy(tree);
		return NULL;
	}
	return tree;
}

#if 0 // for warning -> sbkim, 나중에 사용할지 몰라 남겨둔다.
static HBOOL	metamail_EncodeEach (const HCHAR *name , HxVector_t *list, void **argv)
{
	HINT32	sd;
	HCHAR	*json;
	METAMAIL_Manager_t	*mgr;
	METAMAIL_Class_t		*clazz;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, name);
	RETURN_IF(name == NULL, FALSE);
	RETURN_IF(list == NULL, FALSE);

	mgr = (METAMAIL_Manager_t *)argv[0];
	sd  = (HINT32)argv[1];

	clazz = (METAMAIL_Class_t *)HLIB_TREE_Lookup(mgr->classTree, (const void *)name);
	RETURN_IF(clazz == NULL, FALSE);

	json = metamail_EncodeList(mgr->encoder, clazz, list);
	RETURN_IF(json == NULL, FALSE);

	if ((HINT32)argv[2] > 0)
		HLIB_RWSTREAM_PutChar(sd, ',');
	HLIB_RWSTREAM_PutString(sd, json);

	argv[2] = (void *)((HINT32)argv[2] + 1);
	APK_Free(json);
	return FALSE;
}

static HCHAR *	metamail_EncodeAll (METAMAIL_Manager_t *mgr, HxTREE_t *tree)
{
	HINT32		sd;
	void		*argv[3];

	HxLOG_Trace();

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, APK_Malloc, APK_Free);
	RETURN_IF(!sd, NULL);

	argv[0] = (void *)mgr;
	argv[1] = (void *)sd;
	argv[2] = (void *)0;

	HLIB_RWSTREAM_Print(sd, "{");
	HLIB_TREE_Foreach(tree, (HxTraverseFunc)metamail_EncodeEach, (void *)argv);
	HLIB_RWSTREAM_Print(sd, "}");

	if (argv[2] == (void *)0)
	{
		HxLOG_Warning("[%s] Failed!!\n", __FUNCTION__);
		HLIB_RWSTREAM_Close(sd);
		return NULL;
	}
	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}
#endif

static void		metamail_ReleaseClass (METAMAIL_Class_t *clazz)
{
	if (clazz)
	{
		if (clazz->name)
			APK_Free(clazz->name);
		if (clazz->extension)
			APK_Free(clazz->extension);
		APK_Free(clazz);
	}
}

static HERROR	metamail_AddClass (
							  METAMAIL_Manager_t *mgr
							, const HCHAR	*name
							, HUINT32 size
							, HUINT32 offset
							, void  * (* decode) (void *, HxJSON_t)
							, HCHAR * (* encode) (const void *, HxJSONBIN_t)
							, const APKS_Extension_t *extension
						#if defined(CONFIG_DEBUG)
							, void	(* print) (FILE *, const void *)
						#endif
						)
{
	METAMAIL_Class_t	*clazz;

	clazz = (METAMAIL_Class_t *)APK_Calloc(sizeof(METAMAIL_Class_t));
	RETURN_IF(clazz == NULL, ERR_FAIL);

	clazz->name   = APK_StrDup(name);
	clazz->size   = size;
	clazz->offset = offset;
	clazz->decode = decode;
	clazz->encode = encode;
	if (extension)
		clazz->extension = (APKS_Extension_t *)APK_MemDup(extension, sizeof(APKS_Extension_t));

#if defined(CONFIG_DEBUG)
	clazz->print  = print;
#endif
	HLIB_TREE_Insert(mgr->classTree, (void *)clazz->name, (void *)clazz);
	return ERR_OK;
}

static void *	metamail_FindObject (METAMAIL_Manager_t *mgr, const HCHAR *type, const void *key)
{
	HxVector_t	*list;

	list = (HxVector_t *)HLIB_TREE_Lookup(mgr->listTree, type);
	RETURN_IF(list == NULL, NULL);

	return HLIB_VECTOR_ItemAt(list, HLIB_VECTOR_BSearch(list, key, NULL));
}

static METAMAIL_Manager_t *
				metamail_GetMailManager (void)
{
	static METAMAIL_Manager_t	s_manager;
	if (s_manager.classTree == NULL)
	{
		s_manager.classTree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)metamail_ReleaseClass
			);
		HxLOG_Assert(s_manager.classTree);

		s_manager.listTree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)HLIB_VECTOR_Delete
			);
		HxLOG_Assert(s_manager.listTree);
	}
	return &s_manager;
}

#define	____OAPI____________________________________________________________________________________

static HUINT32		metamail_oapi_Handshake (METAMAIL_Manager_t *mgr)
{
	HxLOG_Trace();

	return APKI_META_Query("mail", "{\"handshake\":true}", NULL, NULL);
}

static HBOOL 		metamail_oapi_ReadResult (HxJSON_t json, void **argv)
{
	METAMAIL_Manager_t *mgr;
	HxJSON_t	root;

	HxLOG_Trace();

	mgr  = (METAMAIL_Manager_t *)argv[0];
	root = HLIB_JSON_Object_GetObject(json, "mail");
	RETURN_IF(!root, FALSE);

	mgr->version++;

	argv[2] = (void *)metamail_DecodeAll(mgr, root, mgr->version);

	if (argv[2] == NULL)
		mgr->version--;

	return TRUE;
}

static HERROR		metamail_oapi_Read (METAMAIL_Manager_t *mgr, HxTREE_t **updated, HxTREE_t **removed)
{
	void	*argv[4];
	HCHAR	*cmd;
	HUINT32	ret;

	HxLOG_Trace();

	argv[0] = (void *)mgr;
	argv[1] = NULL;
	argv[2] = NULL;
	argv[3] = NULL;

	cmd = HLIB_STD_FmtString(NULL, 0, "{\"read\":null}");
	RETURN_IF(cmd == NULL, ERR_FAIL);

	ret = APKI_META_Query("mail", cmd, (void *)argv, (HBOOL (*)(HxJSON_t, void *))metamail_oapi_ReadResult);
	HLIB_STD_FreeFmtString(cmd);
	if (ret == 0)
	{
		HxLOG_Error("%s() fail!!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	if (updated)
	{
		*updated = (HxTREE_t *)argv[2];
	}

	if (removed)
	{
		*removed = (HxTREE_t *)argv[3];
	}
	return ERR_OK;
}

#if 0 // for warning -> sbkim, 나중에 사용할지 몰라 남겨둔다.
static HERROR		metamail_oapi_Write (METAMAIL_Manager_t *mgr, HxTREE_t *tree)
{
	HCHAR	*json, *cmd;

	HxLOG_Trace();

	json = metamail_EncodeAll(mgr, tree);
	RETURN_IF(json == NULL, ERR_FAIL);

	cmd  = HLIB_STD_FmtString(NULL, 0, "{\"write\":%s}", json);
	APK_Free(json);
	RETURN_IF(cmd == NULL, ERR_FAIL);

	APKI_META_Call("mail", cmd);
	APK_Free(cmd);
	return ERR_OK;
}

static HBOOL 		metamail_oapi_GetResult (HxJSON_t json, void **argv)
{
	METAMAIL_Manager_t	*mgr;
	METAMAIL_Class_t		*clazz;
	HxJSON_t	root;
	HxJSON_t	value;
	const HCHAR *key;

	HxLOG_Trace();

	mgr  = (METAMAIL_Manager_t *)argv[0];
	root = HLIB_JSON_Object_GetObject(json, "mail");
	RETURN_IF(!root, FALSE);

	HxJSON_OBJECT_EACH(json, key, value, {
		clazz = (METAMAIL_Class_t *)HLIB_TREE_Lookup(mgr->classTree, (const void *)key);
		if (clazz == NULL)
			continue;

		argv[1] = metamail_DecodeObject(clazz, value);
		break;
	});
	return TRUE;
}

static HxVector_t *	metamail_oapi_Get (METAMAIL_Manager_t *mgr, const HCHAR *type, HINT32 uid)
{
	HCHAR	json[256];
	void	*argv[2];

	HxLOG_Trace();
	RETURN_IF(HLIB_TREE_Lookup(mgr->classTree, (const void *)type) == NULL, NULL);

	argv[0] = (void *)mgr;
	argv[1] = NULL;

	HxSTD_PrintToStrN(json, 256, "{\"get\":{\"type\":\"%s\",\"uid\":%d}}", type, (int)uid);

	if (APKI_META_Query(
					"mail"
					, json
					, (void *)argv
					, (HBOOL (*)(HxJSON_t, void *))metamail_oapi_GetResult
				) == 0)
	{
		HxLOG_Debug("%s(%s) fail!!\n", __FUNCTION__), json;
		return NULL;
	}
	return (HxVector_t *)argv[1];
}
#endif

static HERROR		metamail_oapi_Set (METAMAIL_Manager_t *mgr, const HCHAR *typeName, const void *data)
{
	HCHAR	*json, *item;
	METAMAIL_Class_t	*clazz;

	HxLOG_Trace();

	clazz = (METAMAIL_Class_t *)HLIB_TREE_Lookup(mgr->classTree, (const void *)typeName);
	RETURN_IF(clazz == NULL, ERR_FAIL);

	item = metamail_EncodeObject(mgr->encoder, data);
	RETURN_IF(item == NULL, ERR_FAIL);

	json = HLIB_STD_FmtString(NULL, 0, "{\"set\":{\"type\":\"%s\",\"item\":%s}}", clazz->name, item);
	RETURN_IF(json == NULL, ERR_FAIL);

	APKI_META_Call("mail", json);
	HLIB_STD_FreeFmtString(json);
	return ERR_OK;
}


static HERROR		metamail_oapi_Remove (METAMAIL_Manager_t *mgr, const HCHAR *typeName, HINT32 ulId)
{
	HCHAR	*json;
	METAMAIL_Class_t	*clazz;

	HxLOG_Trace();

	clazz = (METAMAIL_Class_t *)HLIB_TREE_Lookup(mgr->classTree, (const void *)typeName);
	RETURN_IF(clazz == NULL, ERR_FAIL);

	json = HLIB_STD_FmtString(NULL, 0, "{\"remove\":{\"type\":\"%s\",\"id\":%d}}", clazz->name, ulId);
	RETURN_IF(json == NULL, ERR_FAIL);

	APKI_META_Call("mail", json);
	HLIB_STD_FreeFmtString(json);

	return ERR_OK;
}

static HUINT32		metamail_oapi_Notifier (HUINT32 accessCode, HxJSON_t param)
{
	METAMAIL_Manager_t	*mgr;
	APK_MetaMailListener_t	onUpdate;
	void *				userdata;
	HINT32				args[2];
	METAMAIL_Class_t		*clazz;
	DxMail_t				*pstMail = NULL;

	HxLOG_Trace();

	onUpdate = NULL;
	userdata = NULL;
	args[0]  = args[1] = 0;

	APPKIT_ENTER;
	mgr = metamail_GetMailManager();
	if (mgr->listener.accessCode == accessCode
		&& param)
	{
		onUpdate = mgr->listener.onUpdate;
		userdata = mgr->listener.userdata;
	}

	if( onUpdate )
	{
		clazz = (METAMAIL_Class_t *)HLIB_TREE_Lookup(mgr->classTree, DxNAMEOF(DxMail_t));
		if (clazz != NULL)
		{
			pstMail = (DxMail_t *)metamail_DecodeObject(clazz, HLIB_JSON_Object_GetObject(param, "on_update"));

			HxLOG_Print("[%s,%d] id : %d \n", __FUNCTION__ , __LINE__, pstMail->ulId);
			HxLOG_Print("[%s,%d] type : %d \n", __FUNCTION__ , __LINE__, (HINT32)pstMail->eMailType);
			onUpdate( NULL, 0, (const HINT32 *)pstMail, userdata );
		}
	}

	APPKIT_LEAVENO;
	return 0;
}

#define	____PROTECTED_API___________________________________________________________________________

HBOOL	APKI_METAMAIL_Init (HINT32 argc, ...)
{
	METAMAIL_Manager_t	*mgr;
	HERROR				err;
	HINT32				i;
	APKS_Extension_t	*extension[10];


	HxLOG_Trace();

	i = 0;
	memset(extension, 0, sizeof(APKS_Extension_t *) * 10);

	HxSEMT_Create(&_ulModuleSemaphoreId, "sametamail", HxSEMT_FIFO);

	mgr  = metamail_GetMailManager();
	if (argc == 6)
	{
		va_list	ap;
		va_start(ap, argc);
		while (argc--)
		{
			extension[i++] = va_arg(ap, APKS_Extension_t *);
		}
		va_end(ap);
		i = 0;
	}
	else
	{
		HxLOG_Critical("%s(argc:%d, ...) called, META Mail will be initialized as default!\n", __FUNCTION__, argc);
	}
	mgr->encoder = HLIB_JSONBIN_ENCODER_Open();
	HxLOG_Assert(mgr->encoder);

	err  = metamail_AddClass(mgr
				, DxNAMEOF(DxMail_t)
				, sizeof(DxMail_t)
				, offsetof(DxMail_t, ucMailText)
				, metamail_DecodeMail
				, metamail_EncodeMail
				, extension[i++]
		#if defined(CONFIG_DEBUG)
				, (void *)DxMail_Print
		#endif
			);

	RETURN_IF(err != ERR_OK, FALSE);

	APPKIT_ENTER;
	{
		mgr->listener.accessCode = metamail_oapi_Handshake(mgr);
		HxLOG_Assert(mgr->listener.accessCode);
		mgr->listener.notifierId = APKI_ConnectNotifier(mgr->listener.accessCode, (APK_OapiNotifier)metamail_oapi_Notifier);
		HxLOG_Assert(mgr->listener.notifierId);
	}
	APPKIT_LEAVENO;

	return TRUE;
}

HBOOL	APKI_METAMAIL_DeInit (HINT32 argc, ...)
{
	METAMAIL_Manager_t	*mgr;

	mgr  = metamail_GetMailManager();
	APPKIT_ENTER;
	if (mgr->listener.accessCode && mgr->listener.notifierId)
		APKI_DisconnectNotifier(mgr->listener.accessCode, mgr->listener.notifierId);

	if (mgr->classTree)
		HLIB_TREE_Destroy(mgr->classTree);
	if (mgr->listTree)
		HLIB_TREE_Destroy(mgr->listTree);
	if (mgr->path)
		APK_Free(mgr->path);
	APPKIT_LEAVENO;
	HxSEMT_Destroy(_ulModuleSemaphoreId);

	return TRUE;
}

#define	____PUBLIC_API___________________________________________________________________________

/* 메모리 해제 */
void	APK_METAMAIL_Delete (void *object)
{
	metamail_DeleteObject((void *)object);
}

void *	APK_METAMAIL_Clone (const void *object)
{
	return metamail_CloneObject(object);
}

/* 메일 db에서 삭제 */
HERROR		APK_METAMAIL_Remove( HINT32 id )
{
	HERROR				hErr = ERR_FAIL;

	APPKIT_ENTER;
	hErr = metamail_oapi_Remove( metamail_GetMailManager(), DxNAMEOF(DxMail_t), id );
	APPKIT_LEAVENO;

	return hErr;
}

HERROR		APK_METAMAIL_Update( DxMail_t *updateMail )
{
	HERROR				hErr = ERR_FAIL;
	DxMail_t 				*self = NULL;

	APPKIT_ENTER;
	/* 검색 기준은 ulId다 .. 이건 변경하면 안되므로 다른것만 변경되야함 그래서
	 key 가 들어오는 ulId를 사용하면 된다. */
	self = (DxMail_t *)metamail_FindObject(metamail_GetMailManager(), DxNAMEOF(DxMail_t), updateMail);
	if( self != NULL )
	{
		updateMail->bIsOpen = TRUE;
		memcpy( self , updateMail , sizeof( DxMail_t ));

		/* db update 요청 */
		hErr = metamail_oapi_Set( metamail_GetMailManager(), DxNAMEOF(DxMail_t), (const void *)self );
	}
	APPKIT_LEAVENO;

	return hErr;
}

DxMail_t *	APK_METAMAIL_GetMail (HINT32 id)
{
	DxMail_t	key;
	void *self;

	HxLOG_Trace();

	APPKIT_ENTER;
	key.ulId = id;
	self = metamail_FindObject(metamail_GetMailManager(), DxNAMEOF(DxMail_t), &key);
	self = metamail_CloneObject(self);
	APPKIT_LEAVENO;

	return (DxMail_t *)self;
}

HxVector_t *	APK_METAMAIL_GetMailList (void)
{
	HxVector_t	*list;
	HxVector_t	*result;
	METAMAIL_Manager_t *mgr;

	APPKIT_ENTER;
	mgr  = metamail_GetMailManager();
	list = (HxVector_t *)HLIB_TREE_Lookup(mgr->listTree, DxNAMEOF(DxMail_t));
	if (list)
	{
		result = HLIB_VECTOR_CloneEx(list, (void *(*)(void *))metamail_CloneObject);
	}
	else
	{
		result = NULL;
	}
	APPKIT_LEAVENO;

	RETURN_IF(result == NULL, NULL);
	if (HLIB_VECTOR_Length(result) == 0)
	{
		HLIB_VECTOR_Delete(result);
		return NULL;
	}
	return result;
}

HERROR	APK_METAMAIL_LoadFromOcto (void)
{
	METAMAIL_Manager_t 	*mgr;
	HxTREE_t				*tree = NULL;
	APK_MetaMailListener_t	onUpdate = NULL;
	void *				userdata = NULL;
	HINT32				args[2];

	HxLOG_Trace();

	APPKIT_ENTER;
	mgr  = metamail_GetMailManager();

	if (metamail_oapi_Read(mgr, &tree, NULL) == ERR_OK && tree)
	{
		if (mgr->listTree)
		{
			HLIB_TREE_Destroy(mgr->listTree);
		}
		mgr->listTree = tree;

		onUpdate = mgr->listener.onUpdate;
		userdata = mgr->listener.userdata;

		args[0] = mgr->version;
		args[1] = (HINT32)TRUE;	// means, 'ALL'
	}
	else
	{
		args[0] = args[1]= 0;
	}
	APPKIT_LEAVENO;

	// emit 'updated'
	if (onUpdate)
	{
		//onUpdate(NULL, 1, args, userdata);
	}

	if( tree )
	{
		HxLOG_Print("[%s,%d] Tree valid \n", __FUNCTION__ , __LINE__ );
	}
	return (tree) ? ERR_OK : ERR_FAIL;
}

void			APK_METAMAIL_RegisterNotifier ( APK_MetaMailListener_t listener, void *userdata)
{
	METAMAIL_Manager_t	*mgr;
	HxLOG_Trace();

	APPKIT_ENTER;

	mgr = metamail_GetMailManager();

	if( mgr != NULL )
	{
		mgr->listener.onUpdate = listener;
		mgr->listener.userdata = userdata;
	}

	APPKIT_LEAVENO;
}

void			APK_METAMAIL_UnRegisterNotifier ( void )
{
	METAMAIL_Manager_t	*mgr;
	HxLOG_Trace();

	APPKIT_ENTER;

	mgr = metamail_GetMailManager();

	if( mgr != NULL )
	{
		mgr->listener.onUpdate = NULL;
		mgr->listener.userdata = NULL;
	}

	APPKIT_LEAVENO;
}

#endif
