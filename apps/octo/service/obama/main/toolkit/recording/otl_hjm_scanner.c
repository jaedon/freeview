
#include	<otl.h>
#include	<octo_common.h>
#include	<dapi.h>
#include	<uapi.h>

static unsigned long	s_scannerMsgQ;

typedef void	(* OtlScannerJob_t) (HINT32 argc, void **argv);

typedef struct
{
	OtlScannerJob_t	job;
	HINT32		argc;
	void *		argv[10];
	HINT32		gbgc;
	void *		gbgv[10];
} OtlScannerMsg_t;

typedef struct
{
	HCHAR	path[HxPATH_MAX];
	time_t	ctime;
	HUINT32	size;
	HUINT8	data[4];
} OtlScannerItem_t;

typedef struct
{
	HCHAR			path[HxPATH_MAX];
	HINT32			change;
	HxHASH_t	*hash;
} OtlScannerCache_t;

static void	otl_hjm_scanner_Thread (void *arg)
{
	OtlScannerMsg_t	msg;

	for (;;)
	{
		if (VK_MSG_Receive(s_scannerMsgQ, &msg, sizeof(OtlScannerMsg_t)) == VK_OK)
		{
			msg.job(msg.argc, msg.argv);
			while (msg.gbgc--)
				HLIB_STD_MemFree(msg.gbgv[msg.gbgc]);
		}
	}
	(void)arg;
}

static void	otl_hjm_scanner_FreeCache (OtlScannerCache_t *cache)
{
	if (cache)
	{
		if (cache->hash)
			HLIB_HASH_Destroy(cache->hash);
		HLIB_STD_MemFree(cache);
	}
}
static OtlScannerCache_t	*otl_hjm_scanner_LoadCache (const HCHAR *path)
{
	HCHAR	*name;
	FILE	*fp;
	OtlScannerCache_t	*cache;
	OtlScannerItem_t	*rec;
	OtlScannerItem_t	*clone;

	name = HLIB_PATH_JoinPathes(path, ".scan_hjm", NULL);
	if (name == NULL)
		return NULL;

	cache = (OtlScannerCache_t *)HLIB_STD_MemCalloc(sizeof(OtlScannerCache_t));
	cache->hash = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, (_HxDestroyNotify)HLIB_STD_MemFree_CB);
	HLIB_STD_StrNCpySafe(cache->path, name, HxPATH_MAX);

	fp = fopen(name, "rb");
	HLIB_STD_MemFree(name);
	if (fp == NULL)
	{
		HxLOG_Print(HxANSI_COLOR_RED("%s(%s)")": failed to open file!\n", __FUNCTION__, path);
		return cache;
	}
	rec   = (OtlScannerItem_t *)HLIB_STD_MemAlloc(sizeof(OtlScannerItem_t) + sizeof(DxRecListData_t) * 10);
	for (;;)
	{
		if (fread(rec, 1, offsetof(OtlScannerItem_t, data), fp) != offsetof(OtlScannerItem_t, data))
			break;
		if (rec->size > (sizeof(DxRecListData_t) * 10))
		{
			HxLOG_Error("Size(%d) is overflow(%d)\n",rec->size,(sizeof(DxRecListData_t)*10));
			break;
		}
		if (fread(rec->data, 1, rec->size, fp) != rec->size)
			break;
		clone = (OtlScannerItem_t *)HLIB_STD_MemDup(rec, rec->size + offsetof(OtlScannerItem_t, data));
		if (clone)
		{
			HLIB_HASH_Insert(cache->hash, clone->path, clone);
		}
	}
	HLIB_STD_MemFree(rec);
	fclose(fp);

	if (HLIB_HASH_Size(cache->hash) == 0)
	{
		HxLOG_Print(HxANSI_COLOR_RED("%s(%s)")": failed to load cache!\n", __FUNCTION__, cache->path);
	}
	return cache;
}

static void	otl_hjm_scanner_WriteCache(OtlScannerCache_t *cache)
{
	FILE	*fp;
	OtlScannerItem_t		*rec;
	HxHASH_TableIter_t	iter;

	fp = fopen(cache->path, "wb");
	if (fp == NULL)
	{
		HxLOG_Print(HxANSI_COLOR_RED("%s(%s)")": failed to open file!\n", __FUNCTION__, cache->path);
		return;
	}


	HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")": write... %u!\n", __FUNCTION__, cache->path, HLIB_STD_GetSystemTick());

	HLIB_HASH_IterInit(&iter, cache->hash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
	{
		fwrite(rec, 1, rec->size + offsetof(OtlScannerItem_t, data), fp);
	}
	fclose(fp);

	HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")": write... done %u!\n", __FUNCTION__, cache->path, HLIB_STD_GetSystemTick());

}


static HxVector_t *	otl_hjm_scanner_MakeResult (OtlScannerCache_t *cache, const HCHAR *path)
{
	HxHASH_TableIter_t	iter;
	HxVector_t			*result;
	DxRecListData_t		*rec, *clone;
	OtlScannerItem_t	*item;

	HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")"\n", __FUNCTION__, path);

	result = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);
	if (result == NULL)
		return NULL;

	HLIB_HASH_IterInit(&iter, cache->hash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&item))
	{
		rec = (DxRecListData_t *)item->data;
		if (rec->ulExtEvtCnt)
			rec->pstExtEvts = (DxRecList_ExtEvt_t *)(rec + 1);
		else
			rec->pstExtEvts = NULL;

		clone = DxRECLISTDATA_Clone(rec);
		if (clone)
			HLIB_VECTOR_Add(result, clone);
	}
	HxVECTOR_DELETE_IF_EMPTY(result);
	return result;
}

static void	otl_hjm_scanner_Load (HINT32 argc, void *argv[])
{
#if !defined(WIN32)
	const HCHAR		*path;
	HxIoctlFunc_t	callback;
	void			*userdata;
	HxDIR_t 		*dir;
	HCHAR			*name;
	struct stat		fstat;
	OtlScannerCache_t	*cache;
	OtlScannerItem_t	*rec;
	OtlScannerItem_t	*temp;
	HCHAR			buf[HxPATH_MAX];
	HxVector_t		*result;

	path     = argv[0];
	callback = argv[1];
	userdata = argv[2];

	HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")"\n", __FUNCTION__, path);
	cache = otl_hjm_scanner_LoadCache(path);
	if (cache == NULL)
	{
		HxLOG_Print(HxANSI_COLOR_RED("%s(%s)")": failed to load cache!\n", __FUNCTION__, path);
		return;
	}

	// check cache
	if (HLIB_HASH_Size(cache->hash) > 0)
	{
		HxHASH_TableIter_t	iter;

		HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")": check hash... %u!\n", __FUNCTION__, cache->path, HLIB_STD_GetSystemTick());
		HLIB_HASH_IterInit(&iter, cache->hash);
		while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
		{
			if (lstat(rec->path, &fstat) != 0)
			{
				HxLOG_Print(HxANSI_COLOR_GREEN("%s()")": \'%s\' maybe removed\n", __FUNCTION__, rec->path);
				HLIB_HASH_IterRemove(&iter);
				cache->change++;
				continue;
			}
			if (rec->ctime != fstat.st_ctime)
			{
				HxLOG_Print(HxANSI_COLOR_GREEN("%s()")": \'%s\' maybe changed\n", __FUNCTION__, rec->path);
				HLIB_HASH_IterRemove(&iter);
				cache->change++;
				continue;
			}
		}
		HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")": check hash... done %u!\n", __FUNCTION__, cache->path, HLIB_STD_GetSystemTick());
	}

	// scan dir
	dir = HLIB_DIR_Open(path);
	if (dir == NULL)
	{
		otl_hjm_scanner_FreeCache(cache);
		HxLOG_Print(HxANSI_COLOR_RED("%s(%s)")": failed to open dir!\n", __FUNCTION__, path);
		return;
	}
	temp = (OtlScannerItem_t *)HLIB_STD_MemAlloc(sizeof(OtlScannerItem_t) + sizeof(DxRecListData_t) * 10);
	do
	{
		name = HLIB_DIR_GetName(dir);
		if (name && HLIB_STD_StrEndWith(name, ".hjm"))
		{
			HLIB_PATH_Join(HLIB_DIR_GetPath(dir), name, buf, HxPATH_MAX);

			rec = (OtlScannerItem_t *)HLIB_HASH_Lookup(cache->hash, buf);
			if (rec == NULL)
			{
				HxJSON_t json = HLIB_JSON_DecodeFile(buf);
				if (json)
				{
					DxRecListData_t *shit = DxRECLISTDATA_Decode(json, NULL);
					if (shit)
					{
						int lstat_result;

						HLIB_STD_StrNCpySafe(temp->path, buf, HxPATH_MAX);
						lstat_result = lstat(buf, &fstat);
						if (lstat_result >= 0)
						{
							temp->ctime = fstat.st_ctime;
							temp->size  = sizeof(DxRecListData_t) + sizeof(DxRecList_ExtEvt_t) * shit->ulExtEvtCnt;
							memcpy(temp->data, shit, temp->size);

							rec = (OtlScannerItem_t *)HLIB_STD_MemDup(temp, temp->size + offsetof(OtlScannerItem_t, data));
							if (rec)
							{
								HLIB_HASH_Insert(cache->hash, rec->path, rec);
								HxLOG_Print(HxANSI_COLOR_YELLOW("%s()")": \'%s\' caching...\n", __FUNCTION__, buf);
								cache->change++;
							}
						}
						DLIB_FreeType(DxNAMEOF(DxRecListData_t), shit);
					}
					HLIB_JSON_Delete(json);
				}
			}
			else
			{
				HxLOG_Print(HxANSI_COLOR_GREEN("%s()")": \'%s\' cache hit!\n", __FUNCTION__, buf);
			}
		}
	} while (HLIB_DIR_Next(dir) == ERR_OK);
	HLIB_STD_MemFree(temp);
	HLIB_DIR_Close(dir);

	// send to dapi
	result = otl_hjm_scanner_MakeResult(cache, path);

	callback(userdata, (HINT32)result);

	if (cache->change > 0)
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("%s(%s)")": cache changed (%d)\n", __FUNCTION__, cache->path, cache->change);
		otl_hjm_scanner_WriteCache(cache);
	}
	otl_hjm_scanner_FreeCache(cache);
#endif
}

static void		otl_hjm_scanner_Init (void)
{
	HERROR hErr;
	if (s_scannerMsgQ == 0)
	{
		unsigned long	tid;

		hErr=VK_MSG_Create(
			OCTO_MSGQ_LEVEL05
			, sizeof(OtlScannerMsg_t)
			, "HjmScannerQ"
			, (unsigned long *)&s_scannerMsgQ
			, VK_SUSPENDTYPE_FIFO
		);

		if(hErr != VK_OK)
		{
			HxLOG_Error(HxANSI_COLOR_RED("%s")": failed to create VK_MSG!\n", __FUNCTION__);
		}

		hErr=VK_TASK_Create(
			otl_hjm_scanner_Thread
			, VK_TASK_PRIORITY_MW_REF
			, SIZE_32K
			, "HjmScanner"
			, NULL
			, &tid
			, VK_SUSPENDTYPE_FIFO
		);

		if(hErr != VK_OK)
		{
			HxLOG_Error(HxANSI_COLOR_RED("%s")": failed to create VK_TASK!\n", __FUNCTION__);
		}
		else
		{
			VK_TASK_Start(tid);
		}
	}
}

// INTERNAL API
void		otl_hjm_scanner_Scan (const HCHAR *path, HxIoctlFunc_t callback, void *userdata)
{
	OtlScannerMsg_t	msg;

	otl_hjm_scanner_Init();

	memset(&msg, 0, sizeof(OtlScannerMsg_t));
	msg.job  = otl_hjm_scanner_Load;

	msg.argv[msg.argc++] = HLIB_STD_StrDup(path);
	msg.argv[msg.argc++] = callback;
	msg.argv[msg.argc++] = userdata;
	msg.gbgv[msg.gbgc++] = msg.argv[0];

	VK_MSG_Send(s_scannerMsgQ, &msg, sizeof(OtlScannerMsg_t));
}


