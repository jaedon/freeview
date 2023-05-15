
/**************************************************************
 *	@file		meta_search.c
 *
 *	http://www.humaxdigital.com
 *
 **************************************************************/

#include	<hlib.h>
#include	<dlib.h>
#include	<oapi.h>
#include	<dapi.h>
#include <apk.h>

#include	"_apk_int.h"


#ifdef	CONFIG_DEBUG
#define APK_DEBUG(expr)	do {expr;}while(0)
#else
#define APK_DEBUG(expr)
#endif


typedef struct
{
	APK_META_Getter_t	getter;
	void				*userdata;
	HUINT32				id;
#ifdef	CONFIG_DEBUG
	HCHAR				query[4];
#endif
} apkMetaSearch_Ctx_t;

static HxTREE_t *s_queryMap;
static HUINT32   s_semaphore;


static HxTREE_t	*metasearch_GetMap (void)
{
	if (s_queryMap == NULL)
	{
		s_queryMap = HLIB_TREE_NewFull(NULL, NULL, NULL, (HxDestroyNotify)APK_Free_CB);
		HLIB_TREE_NodeCache(s_queryMap, TRUE);
		HxSEMT_Create(&s_semaphore, "metasearch_sem", HxSEMT_FIFO);
	}
	HxSEMT_Get(s_semaphore);
	return s_queryMap;
}

static void		metasearch_ReleaseMap (HxTREE_t *map)
{
	HxSEMT_Release(s_semaphore);
	(void)map;
}


static HINT32	metasearch_Register(apkMetaSearch_Ctx_t *search)
{
	HxTREE_t *map = metasearch_GetMap();
	if (map)
	{
		HLIB_TREE_Insert(map, (void *)search, (void *)search);
		metasearch_ReleaseMap(map);
	}
	return (HINT32)search;
}

static apkMetaSearch_Ctx_t *	metasearch_Unregister (HINT32 id)
{
	HxTREE_t *map = metasearch_GetMap();
	apkMetaSearch_Ctx_t *ins = NULL;
	if (map)
	{
		ins = (apkMetaSearch_Ctx_t *)HLIB_TREE_Lookup(map, (void *)id);
		if (ins)
			HLIB_TREE_Steal(map, (void *)id);
		metasearch_ReleaseMap(map);
	}
	return ins;
}

STATIC void	metasearch_freeOnDataReceived(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;
	HCHAR	*pszName = NULL;

	HxLOG_Assert(nArgc == 6);

	pvData = apArgV[1];
	if (pvData)
		APK_Free(pvData);

	pszName = apArgV[2];
	if (pszName)
		APK_Free(pszName);
}

static void	metasearch_OnDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t	iter;
	apkMetaSearch_Ctx_t	*search;
	HINT32			i, n, c;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

#define	METASEARCH_MAX_TYPES	15
	struct result_list {
		const HCHAR	*name;
		HxVector_t	*list;
	} results[METASEARCH_MAX_TYPES];
	void			*item;

	HxLOG_Trace();
	search = metasearch_Unregister((HINT32)userdata);
	if (search == NULL)
	{
		HxLOG_Debug("%s() There is no search instance matched with id(0x%X) (maybe aborted)\n", __FUNCTION__, (int)userdata);
		return;
	}

	if (size == 0 || data == NULL)
	{
		APK_DEBUG(HxLOG_Debug("%s() Failed to search(%s)\n", __FUNCTION__, search->query));

		HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
		stApkEvtArgs.pfnFreeFunc = metasearch_freeOnDataReceived;
		stApkEvtArgs.ulArgCount = 6;
		stApkEvtArgs.apArgV[0] = (void *)search->getter;
		stApkEvtArgs.apArgV[1] = (void *)APK_MemDup(search, sizeof(apkMetaSearch_Ctx_t));
		stApkEvtArgs.apArgV[2] = (void *)0;
		stApkEvtArgs.apArgV[3] = (void *)APK_MemDup(name, HxSTD_StrLen(name)+1);
		stApkEvtArgs.apArgV[4] = (void *)NULL;
		stApkEvtArgs.apArgV[5] = (void *)search->userdata;
		APK_EVENT_Emit(&stApkEvtArgs);

		APK_Free(search);
		return;
	}

	i = c = 0;
	if (size && data)
	{
		memset(results, 0, sizeof(struct result_list) * METASEARCH_MAX_TYPES);
		DAPI_InitIterator(&iter, data, size, 0);
		while (DAPI_NextIterator(&iter))
		{
			item = DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
			if (!item)
				continue;

			for (n = c ; n-- && strcmp(results[i].name, iter.name) != 0 ; i = (i + 1) % c) ;
			if (n < 0)
			{
				if (c == METASEARCH_MAX_TYPES)
				{
					HxLOG_Critical("%s() METASEARCH_MAX_TYPES(%d) overflow!!!!\n", __FUNCTION__, METASEARCH_MAX_TYPES);
					DLIB_FreeType(iter.name, item);
					continue;
				}
				i = c;
				results[i].name = iter.name;
				results[i].list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_GetFreeFunc(iter.name), NULL);
				if (results[i].list == NULL)
				{
					DLIB_FreeType(iter.name, item);
					continue;
				}
				c++;
			}
                        if ( results[i].list )
                        {
			        HLIB_VECTOR_Add(results[i].list, item);
                        }
		}
	}
	if (c == 0)
	{
		APK_DEBUG( HxLOG_Debug("%s() Failed to search(%s)\n", __FUNCTION__, search->query) );

		HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
		stApkEvtArgs.pfnFreeFunc = metasearch_freeOnDataReceived;
		stApkEvtArgs.ulArgCount = 6;
		stApkEvtArgs.apArgV[0] = (void *)search->getter;
		stApkEvtArgs.apArgV[1] = (void *)APK_MemDup(search, sizeof(apkMetaSearch_Ctx_t));
		stApkEvtArgs.apArgV[2] = (void *)0;
		stApkEvtArgs.apArgV[3] = (void *)APK_MemDup(name, HxSTD_StrLen(name)+1);
		stApkEvtArgs.apArgV[4] = (void *)NULL;
		stApkEvtArgs.apArgV[5] = (void *)search->userdata;
		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		APK_DEBUG( HxLOG_Debug("%s() Search success(%s)\n", __FUNCTION__, search->query) );
		for (i = 0 ; c-- ; i++)
		{
			HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));

			stApkEvtArgs.pfnFreeFunc = metasearch_freeOnDataReceived;
			stApkEvtArgs.ulArgCount = 6;
			stApkEvtArgs.apArgV[0] = (void *)search->getter;
			stApkEvtArgs.apArgV[1] = (void *)APK_MemDup(search, sizeof(apkMetaSearch_Ctx_t));
			stApkEvtArgs.apArgV[2] = (void *)c;
			stApkEvtArgs.apArgV[3] = (void *)APK_MemDup(results[i].name, HxSTD_StrLen(results[i].name)+1);
			stApkEvtArgs.apArgV[4] = (void *)results[i].list;
			stApkEvtArgs.apArgV[5] = (void *)search->userdata;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}
	APK_Free(search);
}

/**
 * @see meta_search.h
 */
HINT32	APK_META_SEARCH_Exec (const HCHAR *query, APK_META_Getter_t getter, void *userdata)
{
	apkMetaSearch_Ctx_t	*search;

	HxLOG_Trace();
	HxLOG_Assert(query);
	HxLOG_Debug("%s(%s)\n", __FUNCTION__, query);

#ifdef	CONFIG_DEBUG
	search = (apkMetaSearch_Ctx_t *)APK_Malloc(sizeof(apkMetaSearch_Ctx_t)+ strlen(query));
#else
	search = (apkMetaSearch_Ctx_t *)APK_Malloc(sizeof(apkMetaSearch_Ctx_t));
#endif
	if (search == NULL)
	{
		HxLOG_Error("%s() Out of memory!!\n", __FUNCTION__);
		return 0;
	}
	search->getter  = getter;
	search->userdata = userdata;
	APK_DEBUG(memcpy(search->query, query, strlen(query) + 1));
	search->id = DAPI_Query(query, metasearch_OnDataReceived, (void *)search);
	if (search->id == 0)
	{
		APK_Free(search);
		HxLOG_Error("%s() DAPI_Query(%s) fail!\n", __FUNCTION__, query);
		return 0;
	}
	return metasearch_Register(search);
}

/**
 * @see meta_search.h
 */
void	APK_META_SEARCH_Abort (HINT32 id)
{
	HxLOG_Trace();

	if (id)
	{
		apkMetaSearch_Ctx_t *search = metasearch_Unregister(id);
		if (search)
			APK_Free(search);
	}
}


HBOOL APK_META_SEARCH_Join (HINT32 nId, HUINT32 timeout)
{
	apkMetaSearch_Ctx_t *search;

	HxLOG_Trace();

	search = (apkMetaSearch_Ctx_t *)nId;

	return DAPI_Wait(timeout, search->id) == DERR_TIMEOUT ? FALSE : TRUE;
}

#ifdef	CONFIG_DEBUG

static HBOOL metasearch_CmdGetter (HINT32 nId, HINT32 nRemainingJob, const HCHAR *szTypeName, HxVector_t *pList, void *pvUserData)
{
	void *item;

	HLIB_CMD_Printf(
		"%s(\n"
		"\t          nId: 0x%X\n"
		"\tnRemainingJob: %d\n"
		"\t   szTypeName: %s\n"
		"\t        pList: 0x%X\n"
		"\t   pvUserData: 0x%X(%s)\n"
		")\n"
		, __FUNCTION__
		, nId
		, nRemainingJob
		, szTypeName
		, (unsigned int)pList
		, (unsigned int)pvUserData
		, (const char *)pvUserData
	);

	if (pList == NULL)	// if pList == NULL, 'SEARCH FAILED' for 'szTypeName'
	{
		return FALSE;
	}

	HxVECTOR_EACH(pList, void *, item, {
		DLIB_Print(szTypeName, stdout, item);
	});
	HLIB_VECTOR_Delete(pList);

	if (nRemainingJob == 0)	// search finished
	{
		return TRUE; // if return TRUE, 'STOP SEARCH'
	}
	return FALSE; // if return FALSE, 'KEEP GOING'
}

static HINT32	metasearch_CmdSearch(void *arg)
{
	HCHAR	*query = (HCHAR *)arg;
	HCHAR	*buf   = NULL;
	HINT32	id;

	if (HxSTD_StrEmpty(query))
		return -1;

	if (HLIB_STD_StrCaseEndWith(query, ".query"))
	{
		FILE *fp = fopen(query, "r");

		if (!fp)
			return -1;

		buf   = (HCHAR *)APK_Malloc(4096);
		(void)fread(buf, 1, 4096, fp);
		fclose(fp);
		query = buf;
	}
	HLIB_CMD_Printf("%s(%s)\n", __FUNCTION__, query);

	id = APK_META_SEARCH_Exec(query, metasearch_CmdGetter, query);
	if (id)
		APK_META_SEARCH_Join(id, 5000);

	if (buf)
		APK_Free(buf);
	return 0;
}

void	APK_META_SEARCH_InitCommand(void)
{
#define	hCmdHandle		"APPKIT"
	HLIB_CMD_RegisterWord (hCmdHandle
		, metasearch_CmdSearch
		, "meta_search"
		, "metadata search"
		, "metadata search"
	);
}
#endif
