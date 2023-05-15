
#include	<otl.h>
#include	<octo_common.h>
#include	<dapi.h>
#include	<uapi.h>

#include	"otl_hjm.h"

//#define	OTL_HJM_INTERNAL_SCANNER
#define OTL_HJM_SCAN_THREAD

typedef struct
{
	HCHAR			path[HxPATH_MAX];
	HxIoctlFunc_t	scanDone;
	void *			userdata;
} OtlScan_t;

typedef struct
{
	HxHASH_t	*idMap;
	HxHASH_t	*urlMap;
	HxTREE_t		*scanned;
	HxCBUF_t			textBuf;
	HINT32			tempStream;
	HUINT32			nextId;
	OtlHjmPostProcessor_t postProcessor;
} OtlHJM_t;

#if defined(OTL_HJM_SCAN_THREAD)
typedef struct
{
	OtlScan_t		*pstScan;
	HxVector_t		*pstFileNameVectors;
} OtlHjmScanMsg_t;
#endif

static HUINT32	s_hjmSem;
#define	OTL_HJM_ENTER	HxSEMT_Get(s_hjmSem)
#define	OTL_HJM_LEAVE	HxSEMT_Release(s_hjmSem)

#if defined(OTL_HJM_SCAN_THREAD)
STATIC unsigned long		 s_ulOtlHjm_ScanTaskId = 0;
STATIC unsigned long		 s_ulOtlHjm_ScanTaskQId = 0;
#endif

static OtlHJM_t *	otl_hjm_GetInstance (void);
static void		otl_hjm_ReleaseInstance(OtlHJM_t *ctx);

static HCHAR *	otl_hjm_GetBuf (OtlHJM_t *ctx, HUINT32 size)
{
	return HLIB_CBUF_GetBuffer(ctx->textBuf, (HINT32)size);
}

static HINT32	otl_hjm_GetRWStream (OtlHJM_t *ctx)
{
	HLIB_RWSTREAM_Rewind(ctx->tempStream);
	return ctx->tempStream;
}

static HBOOL	otl_hjm_ComparePath (const DxRecListData_t *rec, void *path)
{
	return HLIB_STD_StrStartWith(rec->stMeta.szPath, path);
}

static HBOOL	otl_hjm_IsScannable (OtlHJM_t *ctx, const DxRecListData_t *rec)
{
	void	*iter;
	HCHAR	*path;

	for ( iter = HLIB_TREE_Iterator(ctx->scanned)
		; iter
		; iter = HLIB_TREE_NextIterator(iter) )
	{
		path = HLIB_TREE_IteratorValue(iter);

		if (HLIB_STD_StrStartWith(rec->stMeta.szPath, path))
			return TRUE;
	}
	return FALSE;
}

static HCHAR *	otl_hjm_GetFullpath (OtlHJM_t *ctx, const DxRecListData_t *rec, const HCHAR *ext)
{
	HCHAR	*path;

	path = otl_hjm_GetBuf(ctx, 512);
	HLIB_PATH_Join(rec->stMeta.szPath, rec->stMeta.szFileName, path, 512);
	if (ext)
	{
		HLIB_STD_StrAppend(path, ext, 512);
	}
	return path;
}

static void *	otl_hjm_Pack (OtlHJM_t *ctx, const DxRecListData_t *rec, HUINT32 *size)
{
	HINT32	sd;

	sd = otl_hjm_GetRWStream(ctx);

	HLIB_RWSTREAM_Write(sd, (void *)rec, sizeof(DxRecListData_t));
	if (rec->ulExtEvtCnt && rec->pstExtEvts)
		HLIB_RWSTREAM_Write(sd, (void *)rec->pstExtEvts, sizeof(DxRecList_ExtEvt_t) * rec->ulExtEvtCnt);
	if (size)
		*size = HLIB_RWSTREAM_GetOffset(sd);

	return HLIB_RWSTREAM_GetBuf(sd);
}

static HBOOL	otl_hjm_UpdateHJM (OtlHJM_t *ctx, const DxRecListData_t *rec)
{
	FILE	*fp;
	HCHAR	*path;
	HCHAR	*encoded;

	encoded = DxRECLISTDATA_Encode(rec, NULL);
	if (encoded == NULL)
	{
		HxLOG_Error("%s(%s,%s) Failed to encode!\n", __FUNCTION__, rec->szUrl, rec->szName);
		return FALSE;
	}

	path = otl_hjm_GetFullpath(ctx, rec, ".hjm");
	fp   = fopen(path, "w");
	if (fp == NULL)
	{
		DLIB_MemFree(encoded);
		HxLOG_Error("%s() Failed to open file(%s)\n", __FUNCTION__, path);
		return FALSE;
	}
	fprintf(fp, "%s", encoded);
	fclose(fp);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	if (HLIB_EXTRA_ChownWithParam(path, UID_obama, GID_pvr) != ERR_OK)
	{
		HxLOG_Error("Fail to chown path = %s \n", path);
	}

	if (HLIB_EXTRA_Chmod(path, 0660) != ERR_OK)
	{
		HxLOG_Error("	Fail to chmod path = %s \n", path);
	}
#endif

	DLIB_MemFree(encoded);
	HxLOG_Message("%s(): \'%s\' updated\n", __FUNCTION__, path);
	return TRUE;
}

static HBOOL	otl_hjm_RemoveHJM (OtlHJM_t *ctx, const DxRecListData_t *rec)
{
	HCHAR	*path;

	path = otl_hjm_GetFullpath(ctx, rec, ".hjm");

	if (HLIB_FILE_Exist(path))
	{
		UAPI_FM_Remove(path, NULL, NULL, NULL);
		HxLOG_Message("%s(): \'%s\' removed\n", __FUNCTION__, path);
		return TRUE;
	}
	return FALSE;
}


static DxRecListData_t *	otl_hjm_AddRecording (OtlHJM_t *ctx, const DxRecListData_t *rec, HBOOL IsSavedItem)
{
	DxRecListData_t	*clone;

	clone = DxRECLISTDATA_Clone(rec);
	if (clone == NULL)
		return 0;

	if(IsSavedItem == TRUE)
	{
		// 녹화물 불러오기 전에 녹화가 먼저 되어, 해쉬에 등록되어있다면 해쉬 등록을 제외 한다.
		if(HLIB_HASH_Lookup(ctx->urlMap,(const void*)clone->szUrl) != NULL)
		{
			HxLOG_Warning("*********************************************\n");
			HxLOG_Warning("[%s:%d]Already exist Hash Table[ID %d,URI: %s] - [%s]\n",__FUNCTION__,__LINE__,clone->ulContentId,clone->szUrl, clone->szName);
			HxLOG_Warning("*********************************************\n");
			if(clone)
				HLIB_STD_MemFree(clone);
			return NULL;
		}
	}

	clone->ulContentId = ctx->nextId++;

	HLIB_HASH_Insert(ctx->urlMap, (void *)clone->szUrl, clone);
	HLIB_HASH_Insert(ctx->idMap , (void *)clone->ulContentId, clone);

	HxLOG_Message("%s(): %d(%s) \'%s\' added\n", __FUNCTION__, clone->ulContentId, clone->szUrl, clone->szName);

	if (otl_hjm_IsScannable(ctx, rec))
	{
		void *	pack;
		HUINT32	size;

		pack = otl_hjm_Pack(ctx, clone, &size);
		DAPI_Set(DxNAMEOF(DxRecListData_t), (HUID)clone->ulContentId, pack, size);
	}
	return clone;
}

static DxRecListData_t *	otl_hjm_UpdateRecording (OtlHJM_t *ctx, const DxRecListData_t *rec)
{
	DxRecListData_t	*clone;

	clone = DxRECLISTDATA_Clone(rec);
	if (clone == NULL)
		return 0;

	HxLOG_Message("%s(): %d(%s) \'%s\' updated\n", __FUNCTION__, clone->ulContentId, clone->szUrl, clone->szName);

	HLIB_HASH_Replace(ctx->urlMap, (void *)clone->szUrl, clone);
	HLIB_HASH_Replace(ctx->idMap , (void *)clone->ulContentId, clone);

	if (otl_hjm_IsScannable(ctx, rec))
	{
		void *	pack;
		HUINT32	size;
		pack = otl_hjm_Pack(ctx, clone, &size);
		DAPI_Set(DxNAMEOF(DxRecListData_t), (HUID)clone->ulContentId, pack, size);
	}
	return clone;
}

static HUINT32	otl_hjm_RemoveRecording (OtlHJM_t *ctx, const DxRecListData_t *rec)
{
	HUINT32	cid;

	HxLOG_Message("%s(): %d(%s) \'%s\' removed\n", __FUNCTION__, rec->ulContentId, rec->szUrl, rec->szName);

	if (otl_hjm_IsScannable(ctx, rec))
		DAPI_Reset(DxNAMEOF(DxRecListData_t), (HUID)rec->ulContentId);

	cid = rec->ulContentId;
	HLIB_HASH_Remove(ctx->urlMap, (void *)rec->szUrl);
	HLIB_HASH_Remove(ctx->idMap , (void *)rec->ulContentId);
	return cid;
}

static HxVector_t *	otl_hjm_Filtering (OtlHJM_t *ctx, OtlHjmCompare_t equals, void *key)
{
	HxHASH_TableIter_t	iter;
	DxRecListData_t		*rec;
	HxVector_t			*list;

	list = HLIB_VECTOR_New(NULL, NULL);
	if (list == NULL)
		return NULL;

	HLIB_HASH_IterInit(&iter, ctx->idMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&rec))
	{
		if (equals == NULL || equals(rec, key))
		{
			HLIB_VECTOR_Add(list, rec);
		}
	}
	HxVECTOR_DELETE_IF_EMPTY(list);
	return list;
}

static HINT32	otl_hjm_RemoveList (OtlHJM_t *ctx, HxVector_t *list, HBOOL withHJM)
{
	HINT32			n;
	DxRecListData_t	*rec;

	if (list == NULL)
		return 0;

	DAPI_BatchBegin(DxNAMEOF(DxRecListData_t));
	HxVECTOR_EACH(list, DxRecListData_t *, rec, {
		if (withHJM)
			otl_hjm_RemoveHJM(ctx, rec);
		otl_hjm_RemoveRecording(ctx, rec);
	});
	DAPI_BatchEnd(DxNAMEOF(DxRecListData_t));

	n = HLIB_VECTOR_Length(list);
	HLIB_VECTOR_Delete(list);
	return n;
}

static HBOOL	otl_hjm_CheckContents (OtlHJM_t *ctx, const DxRecListData_t *rec)
{
	HCHAR	*path;

	path = otl_hjm_GetBuf(ctx, DxRECLIST_URL);
	HLIB_PATH_Join(rec->stMeta.szPath, rec->stMeta.szFileName, path, DxRECLIST_URL);
	HLIB_STD_StrAppend(path, ".ts", DxRECLIST_URL);

	if (!HLIB_FILE_Exist(path))
	{
		HxLOG_Debug("%s() invalid contents: no .ts file(%s)\n", __FUNCTION__, path);
		return FALSE;
	}
	return TRUE;
}

static HBOOL	otl_hjm_DoCorrection (OtlHJM_t *ctx, DxRecListData_t *rec)
{
	if (!otl_hjm_CheckContents(ctx, rec) && rec->eRecStatus != eDxREC_STS_INCOMPLETED)
	{
		rec->eRecStatus    = eDxREC_STS_INCOMPLETED;
		rec->eRecEndReason = eDxREC_FAILED_UNKNOWN;
		return TRUE;
	}

	// CORRECTION!
	if (rec->eRecStatus == eDxREC_STS_RECORD_START || rec->eRecStatus == eDxREC_STS_RECORDING)
	{
		rec->eRecStatus    = eDxREC_STS_INCOMPLETED;
		rec->eRecEndReason = eDxREC_FAILED_POWER_OFF;
		HxLOG_Debug("%s() invalid contents: recording maybe stopped by power-off!\n", __FUNCTION__);
		return TRUE;
	}
	return FALSE;
}

static DxRecListData_t *otl_hjm_Load (OtlHJM_t *ctx, const HCHAR *path)
{
	HxJSON_t		json;
	DxRecListData_t	*rec;

	json = HLIB_JSON_DecodeFile(path);
	if (!json)
	{
		HxLOG_Debug("%s(%s) failed to decode hjm!\n", __FUNCTION__, path);
		return NULL;
	}
	rec  = DxRECLISTDATA_Decode(json, NULL);
	HLIB_JSON_Delete(json);

	if (rec)
	{
		HLIB_PATH_DirName(path, rec->stMeta.szPath, DxRECLIST_PATH);
		HLIB_STD_StrAppend(rec->stMeta.szPath, "/", DxRECLIST_PATH);

		otl_hjm_DoCorrection(ctx, rec);

		if (rec && ctx->postProcessor)
			ctx->postProcessor(rec);
	}
	return rec;
}

// Recording 이 진행중인 아이템은 제외해야 하는 경우가 존재함 .... File Info DB에서 로드한 아이템 아님...
static void	otl_hjm_RemoveListExceptRunningItem (OtlHJM_t *ctx, HxVector_t *list, HBOOL withHJM)
{
	DxRecListData_t	*rec;

	if (list == NULL)
		return;

	DAPI_BatchBegin(DxNAMEOF(DxRecListData_t));
	HxVECTOR_EACH(list, DxRecListData_t *, rec, {
		switch(rec->eRecStatus)
		{
			case eDxREC_STS_RECORDED:
			case eDxREC_STS_INCOMPLETED:
				if (withHJM)
					otl_hjm_RemoveHJM(ctx, rec);
				HxLOG_Debug("Clean Item ...(%s) \n", rec->stMeta.szFileName);
				otl_hjm_RemoveRecording(ctx, rec);
				break;
			case eDxREC_STS_RECORD_START:
			case eDxREC_STS_RECORDING:
			default:
				break;
		}
	});
	DAPI_BatchEnd(DxNAMEOF(DxRecListData_t));

	HxVECTOR_DELETE_IF_EMPTY(list);
}

static void		otl_hjm_OnScanDone (OtlScan_t *scan, HxVector_t *result)
{
	HINT32			i, n;
	DxRecListData_t	*rec;
	OtlHJM_t 		*ctx;
	HxVector_t		*list;
	HBOOL 			bChangeHJM = FALSE;

	ctx  = otl_hjm_GetInstance();
	HxLOG_Debug(HxANSI_COLOR_GREEN("(+)%s(%d)\n"), __FUNCTION__, HLIB_STD_GetSystemTick());

	list = otl_hjm_Filtering(ctx, otl_hjm_ComparePath, scan->path);
	if (list)
	{
		// 레코딩 중인 아이템조차 지워버리면...문제가 발생함...(Scan 전에 이미 시작한 레코딩...)
		otl_hjm_RemoveListExceptRunningItem(ctx, list, FALSE);
	}

	if (result == NULL)	// scan failed
	{
		HxLOG_Debug("%s(%s) scan done with empty result\n", __FUNCTION__, scan->path);
		otl_hjm_ReleaseInstance(ctx);

		if (scan->scanDone)
			scan->scanDone(scan->userdata, 0, scan->path, NULL);
		HLIB_STD_MemFree(scan);
		return;
	}

	HxLOG_Debug("%s(%s) scan done (count:%d)\n", __FUNCTION__, scan->path, HLIB_VECTOR_Length(result));

	HLIB_STD_StrRStrip(scan->path, "/");

	DAPI_BatchBegin(DxNAMEOF(DxRecListData_t));
	n   = HLIB_VECTOR_Length(result);

	otl_hjm_ReleaseInstance(ctx);

	for (i = 0 ; i < n ; i++)
	{
		ctx  = otl_hjm_GetInstance();
		rec = (DxRecListData_t *)HLIB_VECTOR_ItemAt(result, i);
		HxSTD_PrintToStrN(rec->stMeta.szPath, DxRECLIST_PATH, "%s/", scan->path);

		bChangeHJM = otl_hjm_DoCorrection(ctx, rec);

		if (ctx->postProcessor)
			ctx->postProcessor(rec);

		if(bChangeHJM == TRUE)
		{
			DxRecListData_t	*clone;
			// DB만 보정하는 것이 아니라 HJM파일도 업데이트 하도록 수정...
			clone = otl_hjm_AddRecording(ctx, rec, TRUE);
			if (clone)
			{
				otl_hjm_UpdateHJM(ctx, clone);
			}
		}
		else
		{
			otl_hjm_AddRecording(ctx, rec, TRUE);
		}
        otl_hjm_ReleaseInstance(ctx);
	}
	DAPI_BatchEnd(DxNAMEOF(DxRecListData_t));

	HxLOG_Debug(HxANSI_COLOR_GREEN("(-)%s(%d)\n"), __FUNCTION__, HLIB_STD_GetSystemTick());

	if (scan->scanDone)
		scan->scanDone(scan->userdata, n, scan->path, result);
	HLIB_VECTOR_Delete(result);
	HLIB_STD_MemFree(scan);
}




#if !defined(OTL_HJM_INTERNAL_SCANNER)
#if defined(CONFIG_FILEINFO_IFACE_NO_RECORDINGS)
static HERROR otl_hjm_GetRecDataFromFile (const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return ERR_FAIL;
#else
	HCHAR	*pszHJMBuf = NULL;
	struct stat	fstat;

	if (stat(pszFileName, &fstat) == 0)
	{
		FILE	*file = NULL;
		HUINT32	ulSize;

		pszHJMBuf = HLIB_STD_MemCalloc(fstat.st_size + 1);
		file = fopen(pszFileName, "rb");
		if (file)
		{
			HERROR	hErr = ERR_FAIL;

			if (fread(pszHJMBuf, 1, fstat.st_size, file) == fstat.st_size)
			{
				pszHJMBuf[fstat.st_size] = '\0';
				if ((ulSize = HxSTD_StrLen(pszHJMBuf)) > 0)
				{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
					hErr = DLIB_Decode(DxNAMEOF(DxRecListData_t), 0, (const void*)pszHJMBuf
						, ulSize, (const void**)ppvData, pnSize);
#else
					hErr = DLIB_CreateDecodedData(DxNAMEOF(DxRecListData_t), 0, (const void*)pszHJMBuf
						, ulSize, (const void**)ppvData, pnSize);
#endif
					if (ERR_OK != hErr)
					{
						HxLOG_Error("Error HJM (%s), Content (%s) \n", pszFileName, pszHJMBuf);
						*pnSize = 0;
						*ppvData = NULL;
					}
				}
			}
			else
			{
				HxLOG_Error("Read File failed...\n");
			}

			HLIB_STD_MemFree(pszHJMBuf);
			fclose(file);
			return hErr;
		}
		else
		{
			HxLOG_Error("fopen error: File Name [%s]\n", pszFileName);
		}

		if (pszHJMBuf)
			HLIB_STD_MemFree(pszHJMBuf);
	}
	else
	{
		HxLOG_Error("File [%s] not exist\n", pszFileName);
	}

	return ERR_FAIL;
#endif /* CONFIG_PROD_OS_EMULATOR */
}

static void		otl_hjm_OnDataReceivedFromDaMa (const HCHAR *name, HUID uid, const void *data, HUINT32 size, OtlScan_t *scan)
{
#if !defined(OTL_HJM_SCAN_THREAD)
{
	HINT32			recSize;
	DAxIterator_t	iter;
	HxVector_t		*result;
	DxFileInfo_t	*file;
	DxRecListData_t	*rec;
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	DxRecListData_t	*clone;
#endif
	HERROR			 hErr;

	HxLOG_Info("%s("HxANSI_COLOR_YELLOW("data:0x%X, size:%u, path:%s)")"\n", __FUNCTION__, data, size, scan->path);
	if (data && size)
	{
		result = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);
		if (result)
		{
			DAPI_InitIterator(&iter, data, size, 0);
			while (DAPI_NextIterator(&iter))
			{
				file = (DxFileInfo_t *)iter.data;

				rec = NULL;
				recSize = 0;

 				hErr = otl_hjm_GetRecDataFromFile((const HCHAR *)file->szFileName, (void **)&rec, &recSize);
				if ((ERR_OK == hErr) && (NULL != rec) && (0 != recSize))
				{
					if (rec->ulExtEvtCnt)
						rec->pstExtEvts = (DxRecList_ExtEvt_t *)(rec + 1);
					else
						rec->pstExtEvts = NULL;

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
					clone = DxRECLISTDATA_Clone(rec);
					if (clone)
						HLIB_VECTOR_Add(result, clone);
#else
					HLIB_VECTOR_Add(result, rec);
#endif
				}
			}

			HxVECTOR_DELETE_IF_EMPTY(result);
		}
	}
	else
	{
		result = NULL;
	}

	otl_hjm_OnScanDone(scan, result);
}
#else
{
	DAxIterator_t	 iter;
	HBOOL			 bMsgSent = FALSE;
	HxVector_t		*pstVector = NULL;
	
	HxLOG_Info("%s("HxANSI_COLOR_YELLOW("data:0x%X, size:%u, path:%s)")"\n", __FUNCTION__, data, size, scan->path);
	if (data && size)
	{
		HCHAR			*pszFileName;
		DxFileInfo_t	*pstFileInfo;
		
		pstVector = HLIB_VECTOR_NewEx(NULL, 200, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
		if (NULL != pstVector)
		{
			DAPI_InitIterator(&iter, data, size, 0);
			while (DAPI_NextIterator(&iter))
			{
				pstFileInfo = (DxFileInfo_t *)iter.data;

				if (HxSTD_StrLen(pstFileInfo->szFileName) != 0)
				{
					pszFileName = HLIB_STD_StrDup(pstFileInfo->szFileName);
					if (NULL != pszFileName)
					{
						HLIB_VECTOR_Add(pstVector, (void *)pszFileName);
					}
				}
			}
			
			if (HLIB_VECTOR_Length(pstVector) > 0)
			{
				OtlHjmScanMsg_t 	 stMsg;
				HERROR				 hErr;
			
				stMsg.pstFileNameVectors = pstVector;
				stMsg.pstScan = scan;
				
				hErr = VK_MSG_SendTimeout(s_ulOtlHjm_ScanTaskQId, &stMsg, sizeof(OtlHjmScanMsg_t), 0);
				if (VK_OK == hErr)
				{
					bMsgSent = TRUE;
				}
			}
		}
	}

	if (TRUE != bMsgSent)
	{
		if(NULL != scan)
			otl_hjm_OnScanDone(scan, NULL);

		if (NULL != pstVector)
		{
			HLIB_VECTOR_Delete(pstVector);
		}
	}
}
#endif
}
#else
static void		otl_hjm_OnDataReceivedFromDaMa (const HCHAR *name, HUID uid, const void *data, HUINT32 size, OtlScan_t *scan)
{
	DAxIterator_t	iter;
	HxVector_t		*result;
	DxFileInfo_t	*file;
	DxRecListData_t	*rec;
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	DxRecListData_t	*clone;
#endif

	HxLOG_Info("%s("HxANSI_COLOR_YELLOW("data:0x%X, size:%u, path:%s)")"\n", __FUNCTION__, data, size, scan->path);
	if (data && size)
	{
		result = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);
		if (result)
		{
			DAPI_InitIterator(&iter, data, size, 0);
			while (DAPI_NextIterator(&iter))
			{
				file = (DxFileInfo_t *)iter.data;
				if (file->eDataType != eDxFILETYPE_RECORDINGS)
					continue;
				if (file->size < sizeof(DxRecListData_t))
					continue;
				rec  = (DxRecListData_t *)file->data;

				if (rec->ulExtEvtCnt)
					rec->pstExtEvts = (DxRecList_ExtEvt_t *)(rec + 1);
				else
					rec->pstExtEvts = NULL;

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
				clone = DxRECLISTDATA_Clone(rec);
				if (clone)
					HLIB_VECTOR_Add(result, clone);
#else
				HLIB_VECTOR_Add(result, rec);
#endif

			}
			HxVECTOR_DELETE_IF_EMPTY(result);
		}
	}
	else
	{
		result = NULL;
	}
	otl_hjm_OnScanDone(scan, result);
}
#endif
//static void		otl_hjm_OnUmmaScanDone (const UAPI_NotiInfo_t *info, OtlScan_t *scan)
static HERROR		otl_hjm_OnUmmaScanDone (void *userdata, HINT32 nRequestId, UAPI_STATUS_t *pstInfo)
{
	HCHAR	*query;
	HCHAR	path[HxPATH_MAX];
	OtlScan_t	*scan = (OtlScan_t*)userdata;

	HxLOG_Info("%s(reqId:%u, %s)\n", __FUNCTION__, nRequestId, scan->path);
	if (pstInfo->nDoneOperationNum < pstInfo->nTotalOperationNum)
	{
		HxLOG_Debug("%s(In scaning... (%d/%d)\n", __FUNCTION__, pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
		return ERR_OK;
	}

	HLIB_STD_StrNCpySafe(path, scan->path, HxPATH_MAX);

	query = HLIB_STD_FmtString(NULL, 0,
		"SELECT * FROM %s WHERE path LIKE '%s%%' AND ext IN('hjm')"
		, DxNAMEOF(DxFileInfo_t)
		, HLIB_STD_StrRStrip(path, "/")
	);

	if (query == NULL)
	{
		HxLOG_Error("%s() Out of memory!\n", __FUNCTION__);
		HLIB_STD_MemFree(scan);
		return ERR_OK;
	}
	HxLOG_Debug("Query : [%s]\n", query);
	DAPI_Query(query, (DAPI_Getter)otl_hjm_OnDataReceivedFromDaMa, (void *)scan);
	HLIB_STD_FreeFmtString(query);

	return ERR_OK;
}
#else
static HINT32	otl_hjm_OnScannerScanDone (OtlScan_t *scan, HINT32 request, ...)
{
	otl_hjm_OnScanDone(scan, (HxVector_t *)request);
	return 0;
}
extern void		otl_hjm_scanner_Scan (const HCHAR *path, HxIoctlFunc_t callback, void *userdata);
#endif

#if defined(OTL_HJM_SCAN_THREAD)
STATIC void otl_hjm_ScanTask (void *pvArg)
{
	OtlHjmScanMsg_t	 stMsg;
	HERROR			 hErr;
	
	while (1)
	{
		HxSTD_MemSet(&stMsg, 0, sizeof(OtlHjmScanMsg_t));
		
		hErr = VK_MSG_Receive(s_ulOtlHjm_ScanTaskQId, &stMsg, sizeof(OtlHjmScanMsg_t));
		if (VK_OK == hErr)
		{
			HxVector_t		*pstRecResult = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);

			if (stMsg.pstFileNameVectors)
			{
				HINT32			 nRecSize;
				HUINT32			 ulCnt;
				HUINT32 		 ulFileNameNum = HLIB_VECTOR_Length(stMsg.pstFileNameVectors);
				DxRecListData_t	*pstRec, *pstCloneRec;

				for (ulCnt = 0; ulCnt < ulFileNameNum; ulCnt++)
				{
					HCHAR		*pszFileName = (HCHAR *)HLIB_VECTOR_ItemAt(stMsg.pstFileNameVectors, ulCnt);

					if ((NULL != pszFileName) && ('\0' != *pszFileName))
					{
						nRecSize = 0;
						pstRec = NULL;
						
						hErr = otl_hjm_GetRecDataFromFile((const HCHAR *)pszFileName, (void **)&pstRec, &nRecSize);
						if ((ERR_OK == hErr) && (NULL != pstRec) && (nRecSize > 0))
						{
							if (pstRec->ulExtEvtCnt)
							{
								pstRec->pstExtEvts = (DxRecList_ExtEvt_t *)(pstRec + 1);
							}
							else
							{
								pstRec->pstExtEvts = NULL;
							}

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
							pstCloneRec = DxRECLISTDATA_Clone(pstRec);
							if (NULL != pstCloneRec)
							{
								HLIB_VECTOR_Add(pstRecResult, pstCloneRec);
							}
#else
							HLIB_VECTOR_Add(pstRecResult, pstRec);
#endif
						}
					}
				}

				HLIB_VECTOR_Delete(stMsg.pstFileNameVectors);
			}

			HxVECTOR_DELETE_IF_EMPTY(pstRecResult);

			otl_hjm_OnScanDone(stMsg.pstScan, pstRecResult);
		}
	}
}

STATIC HERROR otl_hjm_InitScanThread (void)
{
	unsigned long		 ulTaskId = 0;
	unsigned long		 ulQueueId = 0;
	HERROR				 hErr;
	
	if (0 != s_ulOtlHjm_ScanTaskId)
	{
		return ERR_OK;
	}

	hErr = VK_TASK_Create(otl_hjm_ScanTask, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "otl_hjm_ScanTask", NULL, &ulTaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error("VK_TASK_Create err:\n");
		goto ERROR;
	}

	hErr = VK_MSG_Create(OCTO_MSGQ_LEVEL01, sizeof(OtlHjmScanMsg_t), "otl_hjm_msgQ", &ulQueueId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Error("VK_MSG_Create err:\n");
		goto ERROR;
	}

	(void)VK_TASK_Start(ulTaskId);

	s_ulOtlHjm_ScanTaskId = ulTaskId;
	s_ulOtlHjm_ScanTaskQId = ulQueueId;

	return ERR_OK;
	
ERROR:	
	if (0 != ulTaskId)
	{
		VK_TASK_Destroy(ulTaskId);
	}

	if (0 != ulQueueId)
	{
		VK_MSG_Destroy(ulQueueId);
	}

	return ERR_FAIL;
}
#endif

static void		otl_hjm_Scan (OtlHJM_t *ctx, const HCHAR *path, HxIoctlFunc_t scanDone, void *userdata)
{
	OtlScan_t	scan;
	HCHAR		*save;

	HLIB_STD_StrNCpySafe(scan.path, path, HxPATH_MAX);
	scan.scanDone = scanDone;
	scan.userdata = userdata;

	save = HLIB_STD_StrDup(path);
	if (save)
		HLIB_TREE_Replace(ctx->scanned, save, save);

#if !defined(OTL_HJM_INTERNAL_SCANNER)
{
#if defined(OTL_HJM_SCAN_THREAD)
	(void)otl_hjm_InitScanThread();
#endif
	UAPI_FM_Scan(path, "hjm", FALSE, HLIB_STD_MemDup(&scan, sizeof(OtlScan_t)), otl_hjm_OnUmmaScanDone);
}
#else
{
	otl_hjm_scanner_Scan(path, (HxIoctlFunc_t)otl_hjm_OnScannerScanDone, HLIB_STD_MemDup(&scan, sizeof(OtlScan_t)));
}
#endif
}

static OtlHJM_t *	otl_hjm_GetInstance (void)
{
	static OtlHJM_t	*s_ctx;

	if (s_ctx == NULL)
	{
		s_ctx = (OtlHJM_t *)HLIB_STD_MemCalloc(sizeof(OtlHJM_t));
		HxLOG_Assert(s_ctx);
		s_ctx->idMap  = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL
					, (_HxDestroyNotify)DxRECLISTDATA_Delete);
		s_ctx->urlMap = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, NULL);
		s_ctx->scanned = HLIB_TREE_NewFull((HxCompareDataFunc)HxSTD_StrCmp, NULL, NULL, (HxDestroyNotify)HLIB_STD_MemFree_CB);
		HxLOG_Assert(s_ctx->idMap && s_ctx->urlMap && s_ctx->scanned);
		s_ctx->textBuf = HLIB_CBUF_New(1024 * 40);
		HxLOG_Assert(s_ctx->textBuf);
		s_ctx->tempStream = HLIB_RWSTREAM_Open(NULL, sizeof(DxRecListData_t) * 2);
		HxLOG_Assert(s_ctx->tempStream);
		s_ctx->nextId = 2;
		HxSEMT_Create(&s_hjmSem, "OtlHjmSem", HxSEMT_FIFO);
	}
	OTL_HJM_ENTER;
	return s_ctx;
}

static void	otl_hjm_ReleaseInstance(OtlHJM_t *ctx)
{
	OTL_HJM_LEAVE;
	(void)ctx;
}


#define	__API_______________________________________________________________________________________

void	OTL_HJM_SetPostProcessor (OtlHjmPostProcessor_t postProcessor)
{
	OtlHJM_t	*ctx;

	HxLOG_Trace();

	ctx = otl_hjm_GetInstance();
	ctx->postProcessor = postProcessor;
	otl_hjm_ReleaseInstance(ctx);
}

DxRecListData_t *	OTL_HJM_Load (const HCHAR *path)
{
	OtlHJM_t		*ctx;
	DxRecListData_t	*rec;

	HxLOG_Trace();
	HxLOG_Assert(path);

	ctx = otl_hjm_GetInstance();
	rec = otl_hjm_Load(ctx, path);
	otl_hjm_ReleaseInstance(ctx);

	return rec;
}

void	OTL_HJM_Scan (const HCHAR *path, HxIoctlFunc_t scanDone, void *userdata)
{
	OtlHJM_t	*ctx;

	HxLOG_Trace();
	HxLOG_Assert(path && scanDone);

	ctx = otl_hjm_GetInstance();
	otl_hjm_Scan(ctx, path, scanDone, userdata);
	otl_hjm_ReleaseInstance(ctx);
}

HCHAR *				OTL_HJM_GetURL (HUINT32 contentId, HCHAR *buf, HUINT32 n)
{
	OtlHJM_t		*ctx;
	DxRecListData_t *rec;
	HCHAR			*ret = NULL;

	HxLOG_Trace();

	ctx = otl_hjm_GetInstance();
	rec = HLIB_HASH_Lookup(ctx->idMap, (const void *)contentId);
	if (rec)
	{
		if (buf && n)
		{
			ret = HLIB_STD_StrNCpySafe(buf, rec->szUrl, n);
		}
		else
		{
			ret = HLIB_STD_StrDup(rec->szUrl);
		}
	}
	otl_hjm_ReleaseInstance(ctx);
	return ret;
}


DxRecListData_t *	OTL_HJM_GetRecording (const HCHAR *url)
{
	OtlHJM_t		*ctx;
	DxRecListData_t	*rec;

	HxLOG_Trace();
	HxLOG_Assert(url);

	ctx = otl_hjm_GetInstance();
	rec = HLIB_HASH_Lookup(ctx->urlMap, (const void *)url);
	if (rec)
		rec = DxRECLISTDATA_Clone(rec);
	otl_hjm_ReleaseInstance(ctx);

	return rec;
}

HxVector_t *	OTL_HJM_GetRecordingList (OtlHjmCompare_t filter, void *key)
{
	OtlHJM_t	*ctx;
	HxVector_t	*list;
	HxVector_t	*clone;

	HxLOG_Trace();

	ctx  = otl_hjm_GetInstance();
	list = otl_hjm_Filtering(ctx, filter, key);
	if (list)
	{
		clone = HLIB_VECTOR_NewEasy(HLIB_VECTOR_Length(list), (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);
		if (clone)
		{
			DxRecListData_t *rec, *c;
			HxVECTOR_EACH(list, DxRecListData_t *, rec, {
				c = DxRECLISTDATA_Clone(rec);
				if (c)
					HLIB_VECTOR_Add(clone, c);
			});
			HxVECTOR_DELETE_IF_EMPTY(clone);
		}

		HLIB_VECTOR_Delete(list);
	}
	else
	{
		clone = NULL;
	}
	otl_hjm_ReleaseInstance(ctx);

	return clone;
}

HUINT32				OTL_HJM_AddRecording (const DxRecListData_t *rec, HBOOL withHJM)
{
	OtlHJM_t		*ctx;
	DxRecListData_t	*clone;
	HUINT32			contentId = 0;

	HxLOG_Trace();
	HxLOG_Assert(rec);

	ctx   = otl_hjm_GetInstance();
	clone = otl_hjm_AddRecording(ctx, rec, FALSE);
	if (clone)
	{
		contentId = clone->ulContentId;
		if (withHJM)
			otl_hjm_UpdateHJM(ctx, clone);
	}
	otl_hjm_ReleaseInstance(ctx);

	return contentId;
}

HUINT32				OTL_HJM_UpdateRecording (const DxRecListData_t *rec, HBOOL withHJM)
{
	OtlHJM_t		*ctx;
	DxRecListData_t	*clone;
	HUINT32			contentId = 0;

	HxLOG_Trace();
	HxLOG_Assert(rec);

	ctx   = otl_hjm_GetInstance();
	clone = otl_hjm_UpdateRecording(ctx, rec);
	if (clone)
	{
		contentId = clone->ulContentId;
		if (withHJM)
			otl_hjm_UpdateHJM(ctx, clone);
	}
	otl_hjm_ReleaseInstance(ctx);

	return contentId;
}

HUINT32			OTL_HJM_RemoveRecording (const HCHAR *url, HBOOL withHJM)
{
	OtlHJM_t		*ctx;
	DxRecListData_t	*rec;
	HUINT32			cid = 0;

	HxLOG_Trace();
	HxLOG_Assert(url);

	ctx = otl_hjm_GetInstance();
	rec = HLIB_HASH_Lookup(ctx->urlMap, (const void *)url);
	if (rec)
	{
		cid = rec->ulContentId;
		if (withHJM)
			otl_hjm_RemoveHJM(ctx, rec);
		otl_hjm_RemoveRecording(ctx, rec);
	}
	otl_hjm_ReleaseInstance(ctx);

	return cid;
}

HUINT32			OTL_HJM_RemoveRecordingList (OtlHjmCompare_t filter, void *key, HBOOL withHJM)
{
	OtlHJM_t	*ctx;
	HxVector_t	*list;
	HUINT32		removed = 0;

	HxLOG_Trace();

	ctx  = otl_hjm_GetInstance();
	list = otl_hjm_Filtering(ctx, filter, key);
	if (list)
	{
		removed = otl_hjm_RemoveList(ctx, list, withHJM);
		if(filter == NULL)
			ctx->nextId = 2;
	}
	otl_hjm_ReleaseInstance(ctx);

	return removed;
}



