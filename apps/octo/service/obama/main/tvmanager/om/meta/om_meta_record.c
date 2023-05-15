
#include	<hlib.h>
#include	<dlib.h>
#include <obama_rpcprotocol.h>
#include	<otl.h>
#include	<oapi.h>

#include	<octo_common.h>

#include	<db_param.h>
#include	<svc_fs.h>

#include	<svc_meta.h>

#include	<bus.h>

#include	<mgr_common.h>
#include	<mgr_storage.h>
#include	<mgr_action.h>

#include	<oapi_meta_record.h>
#include	<om_common.h>

#if defined(CONFIG_PROD_FVP4000T) || defined(CONFIG_PROD_CDIG2000C) || defined(CONFIG_PROD_HGS1000S)
#define CONFIG_APCORE_AUTO_DELETE
#endif

#define	RECJOB_COPY	1

#define CONFIG_TEST_AUTO_DEL_LIST_RUN_FREE_SPACE_PERCENT		5
#define CONFIG_TEST_AUTO_DEL_LIST_CHECK_FREE_SPACE_PERCENT		20
#define	RUN_FREESPACE_PERCENT									CONFIG_TEST_AUTO_DEL_LIST_RUN_FREE_SPACE_PERCENT
#define	CHECK_FREESPACE_PERCENT								CONFIG_TEST_AUTO_DEL_LIST_CHECK_FREE_SPACE_PERCENT
#define MAX_AUTO_DELETE_REQUEST			(100LL*1024LL*1024LL)	/* 100G - KByte 단위 */

#define OM_AUTO_DELETE_REFRESH_TIMER_ID_TYPE_MASK		(0xFFFFFF00)
#define OM_AUTO_DELETE_REFRESH_TIMER_ID_ARG_MASK	(0x000000FF)
#define OM_AUTO_DELETE_REFRESH_TIMER_ID			(0xe62300)
#define OM_AUTO_DELETE_REFRESH_TIME_OUT			(5000)		// 5초

typedef HINT32	(* OM_REC_CMP_t) (void *,void *);

typedef	enum
{
	eMETA_AutoDelete_DoNothing,
	eMETA_AutoDelete_MakeList,
	eMETA_AutoDelete_Delete,
	eMETA_AutoDelete_Max,
} OmMediaRec_AutoDelRun_e;

typedef struct
{
	OmMediaRec_AutoDelRun_e		eAutoDelRunType;
	HBOOL						bAutoDeleteProcessing;
	HUINT64						ullToDeleteSpaceKB;
	HUINT64						ullToMakeListSpaceKB;
	HUINT64						ullDeletedItemsSpaceKB;
} _OmMediaRecAutoDelContext_t;
STATIC _OmMediaRecAutoDelContext_t	s_stMetaRecAutoDel_Context = { 0, };

typedef struct
{
	HINT32	job;
	HINT32	requestId;
	HINT32	cid;		///< contents id
	HCHAR	*detail;
} OmMediaRecJob_t;

typedef struct
{
	HINT32	sd;
	HINT32	srzr;
	HCHAR	url[DxRECLIST_URL];

	HUINT32	tsrCID;

	HCHAR	szPvrMountPathList[MAX_CONNECTABLE_PVR_STORAGE_NUM][DxSTORAGE_NAME_LEN];
	HCHAR	szPlaybackMountPathList[MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM][DxSTORAGE_NAME_LEN];
} OmMediaRecContext_t;

static OmMediaRecContext_t	s_context;

static HBOOL	om_meta_rec_ComparePath (const DxRecListData_t *rec, void *path)
{
	return HLIB_STD_StrStartWith(rec->stMeta.szPath, path);
}

static HINT32	om_meta_rec_GetSerializer (OmMediaRecContext_t *ctx)
{
	HLIB_SERIALIZER_Clear(ctx->srzr);
	return ctx->srzr;
}

static HINT32	om_meta_rec_GetRWStream (OmMediaRecContext_t *ctx)
{
	HLIB_RWSTREAM_Rewind(ctx->sd);
	return ctx->sd;
}

static void *	om_meta_rec_Pack (OmMediaRecContext_t *ctx, const DxRecListData_t *rec, HUINT32 *size)
{
	HINT32	sd;

	sd = om_meta_rec_GetRWStream(ctx);

	HLIB_RWSTREAM_Write(sd, (void *)rec, sizeof(DxRecListData_t));
	if (rec->ulExtEvtCnt)
		HLIB_RWSTREAM_Write(sd, (void *)rec->pstExtEvts, sizeof(DxRecList_ExtEvt_t) * rec->ulExtEvtCnt);

	if (size)
		*size = HLIB_RWSTREAM_GetOffset(sd);
	return HLIB_RWSTREAM_GetBuf(sd);
}

static void		om_meta_rec_Unpack (DxRecListData_t *rec)
{
	if (rec->ulExtEvtCnt)
	{
		rec->pstExtEvts = (DxRecList_ExtEvt_t *)(rec + 1);
	}
}


static DxRecListData_t *	om_meta_rec_GetRecording (OmMediaRecContext_t *ctx, HINT32 cid)
{
	HCHAR		*pszUrl;
	DxRecListData_t *	pstItem;

	pszUrl = OTL_HJM_GetURL(cid, ctx->url, DxRECLIST_URL);
	if (pszUrl == NULL)
		return NULL;

	pstItem = OTL_HJM_GetRecording(pszUrl);

	if (pszUrl != NULL && pszUrl != ctx->url)
	{
		HLIB_STD_MemFree((void *)pszUrl);
	}

	return pstItem;
}

static void			om_meta_rec_OnFileOpDone (HERROR err,  void *userdata, ...)
{
	va_list	ap;
	HUINT32	cid;
	HERROR	hErr;
	HINT32	nRpcHandle = 0;
	HxPair_t	*pstPair = (HxPair_t *)userdata;
	HINT32 nRequestId;
	HINT32 nClientHandle;


	nRequestId = (HINT32)pstPair->left;
	nClientHandle = (HINT32)pstPair->right;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed");
	}

	va_start(ap, userdata);
	cid = va_arg(ap, HUINT32);
	va_end(ap);

	HxLOG_Debug("%s(err:%d, requestId:0x%X, cid:%u)\n", __FUNCTION__, err, nRequestId, cid);

	if (err == ERR_OK)
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	else
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, "i",err);
	}
}


static HERROR om_meta_rec_InitAutoDelete (HUINT64	ullToDeleteSpaceKB, HUINT64 ullToMakeListSpaceKB, OmMediaRec_AutoDelRun_e	eAutoDelRunType)
{
	s_stMetaRecAutoDel_Context.eAutoDelRunType		= eAutoDelRunType;
	s_stMetaRecAutoDel_Context.ullToDeleteSpaceKB	= ullToDeleteSpaceKB;
	s_stMetaRecAutoDel_Context.ullToMakeListSpaceKB	= ullToMakeListSpaceKB;
	s_stMetaRecAutoDel_Context.ullDeletedItemsSpaceKB	= 0;

	return ERR_OK;
}

static HUINT64	om_meta_rec_CalcRecDiskSpace(HUINT32 ulDuration, HBOOL bIsHD)
{
	HUINT64	ullRecSpace;

	if(bIsHD)
	{
		/* HD : 1시간에 10G 기준. 초당 KByte */
		ullRecSpace = (STORAGE_SIZE_FOR_HD_PER_HOUR * 1024 * 1024) / (SECONDS_PER_HOUR);
	}
	else
	{
		/* SD : 1시간에 4G 기준. 초당  KByte */
		ullRecSpace = (STORAGE_SIZE_FOR_SD_PER_HOUR * 1024 * 1024) / (SECONDS_PER_HOUR);
	}
	ullRecSpace *= ulDuration;

	return ullRecSpace;
}

static HINT32 om_meta_rec_ItemSortCallback( void *S1, void *S2 )
{
	DxRecListData_t	*pstListRecData= ( DxRecListData_t * )S1;
	DxRecListData_t	*pstNewRecData= ( DxRecListData_t * )S2;
	HUINT32			s1StartTime;
	HUINT32			s2StartTime;
	HINT32			nRet = 0;

	if(pstListRecData == NULL || pstNewRecData == NULL)
		return nRet;

	nRet = pstNewRecData->bDoNotDelete - pstListRecData->bDoNotDelete;
	if (0 != nRet)
	{
		return nRet;
	}

	nRet = pstListRecData->bPlayed - pstNewRecData->bPlayed;
	if (0 != nRet)
	{
		return nRet;
	}

	// keep을 한 recording(bDoNotDelete=True)은 keep을 한 시점을 start time으로 생각해서 to be deleted list에 올리는 시나리오를 반영
	s1StartTime=(pstListRecData->bDoNotDelete==TRUE)?pstListRecData->ulDoNotDeleteSetTime : pstListRecData->ulRecStartTime;
	s2StartTime=(pstNewRecData->bDoNotDelete==TRUE)?pstNewRecData->ulDoNotDeleteSetTime : pstNewRecData->ulRecStartTime;
	nRet = s2StartTime - s1StartTime;

	return nRet;
}

static void om_meta_rec_FreeItem (void *item)
{
	if (item)
	{
		DxRECLISTDATA_Delete((DxRecListData_t *)item);
	}
}

static HxList_t*	om_meta_rec_GetSortedRecList (OmMediaRecContext_t *ctx)
{
	HUINT32 		ulContentNum = 0;
	HUINT32			*pulContentId = NULL;
	HUINT32			i;
	HxList_t		*pstSortedList = NULL;

	(void)SVC_META_GetContentsIdList(&ulContentNum, &pulContentId);
	if (pulContentId == NULL)
	{
		HxLOG_Error("SVC_META_GetContentsIdList failed. \n");
		(void)SVC_META_FreeContentsIdList (pulContentId);
		return NULL;
	}

	for (i = 0; i < ulContentNum; ++i)
	{	// order the recording items
		DxRecListData_t	*pstRecData = om_meta_rec_GetRecording(ctx, pulContentId[i]);
		if ( pstRecData == NULL )
		{
			HxLOG_Error("om_meta_rec_GetRecording failed. cid=(%d) \n", pulContentId[i]);
			continue;
		}

		pstSortedList = HLIB_LIST_AppendSortEx(pstSortedList, pstRecData, om_meta_rec_ItemSortCallback);
	}

	(void)SVC_META_FreeContentsIdList (pulContentId);
	return pstSortedList;
}

static HERROR		om_meta_rec_SetWillbedeleted(DxRecListData_t *pstRecData, HBOOL bWillBeDeleted)
{
	SvcMeta_Record_t	stRec;
	HUINT32				cid;

	if (pstRecData==NULL)
	{
		return ERR_FAIL;
	}

	cid = pstRecData->ulContentId;
	pstRecData->bWillBeDeleted = bWillBeDeleted;
	if (OTL_CONV_DxRecListData2MW_PvrMeta(&stRec, NULL, NULL, pstRecData) == ERR_OK)
	{
		HxLOG_Debug("%s(cid:%u, %s|\'%s\')\n", __FUNCTION__, cid, pstRecData->szUrl, pstRecData->szName);

		SVC_META_SetRecordInfo(cid, &stRec);
	}

	return ERR_OK;
}
STATIC HERROR om_meta_rec_UpdateAutoDeleteList (HxList_t *pstSortedRecList)
{
	HUINT32					i, nListLen = 0;
	HxList_t				*pSortedListNavi = NULL;
	HBOOL					bNeedAutoDeleteCheck = TRUE;
	HUINT32					ulWillBeDeletedItems = 0;
	HUINT32					ulDefaultPvrIdx = DxSTORAGE_INVALID_PVR_INDEX;
	HCHAR					szDefaultPvrPath[DxSTORAGE_NAME_LEN] = {0,};
	HERROR					hErr;

	if (s_stMetaRecAutoDel_Context.bAutoDeleteProcessing)
	{
		HxLOG_Error("[%s, %d line] Already Auto Delete Process working ! \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	ulDefaultPvrIdx = SVC_FS_GetDefaultPvrIdx();
	if (ulDefaultPvrIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM)
	{
		hErr = SVC_FS_GetPvrPath(ulDefaultPvrIdx, eSVC_FS_UsageType_Record, szDefaultPvrPath);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("fail to get pvrpath. pvrIdx(%d) \n",ulDefaultPvrIdx);
		}
	}

	HxLOG_Debug("[%s, %d] Start! \n", __FUNCTION__, __LINE__);
	s_stMetaRecAutoDel_Context.bAutoDeleteProcessing = TRUE;	// get the 	// sort the list as scenario

	// should has same length ...
	nListLen = HLIB_LIST_Length(pstSortedRecList);
	pSortedListNavi = pstSortedRecList;

	if (0 == s_stMetaRecAutoDel_Context.ullToMakeListSpaceKB)
	{
		bNeedAutoDeleteCheck = FALSE;
	}

	for (i = 0; i < nListLen; ++i)
	{
		DxRecListData_t		*pstRecData = (DxRecListData_t *)HLIB_LIST_Data(pSortedListNavi);
		if (pstRecData)
		{	// check the space
			if (FALSE == bNeedAutoDeleteCheck)
			{	// already done ! -> off the checked items
				if (pstRecData->bWillBeDeleted)
				{	// unflag case
					HxLOG_Debug("[%s, %d] (%d) idx item check for the willBeDeleted Off cid=(%d) \n", __FUNCTION__, __LINE__, i, pstRecData->ulContentId);
					om_meta_rec_SetWillbedeleted(pstRecData, FALSE);
				}
			}
			else
			{	// check auto delete candidate
				// skip contents -> lock || Running
				//                        or contents that is not on default hdd
				if (pstRecData->bUserLock
					|| eDxREC_STS_RECORD_START== pstRecData->eRecStatus
					|| eDxREC_STS_RECORDING 	== pstRecData->eRecStatus
					|| ((ulDefaultPvrIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM)
					    &&(HLIB_STD_StrStartWith(pstRecData->stMeta.szPath,szDefaultPvrPath) == FALSE)))
				{
					pSortedListNavi = pSortedListNavi->next;
					if (pstRecData->bWillBeDeleted)
					{	// unflag case
						om_meta_rec_SetWillbedeleted(pstRecData, FALSE);
					}
					continue;
				}
				s_stMetaRecAutoDel_Context.ullDeletedItemsSpaceKB
					+= om_meta_rec_CalcRecDiskSpace(pstRecData->ulRecDuration, pstRecData->eVideoType == eDxVIDEO_TYPE_HD);

				if (FALSE == pstRecData->bWillBeDeleted)
				{
					om_meta_rec_SetWillbedeleted(pstRecData, TRUE);
				}
				ulWillBeDeletedItems++;

				HxLOG_Debug("[%s, %d] (%d) idx item checked as willBeDeleted cid=(%d) \n", __FUNCTION__, __LINE__, i, pstRecData->ulContentId);
				HxLOG_Debug("[%s, %d] req space (%lld), checked space (%lld) checkedItemNum (%d)\n", __FUNCTION__, __LINE__
					, s_stMetaRecAutoDel_Context.ullToMakeListSpaceKB, s_stMetaRecAutoDel_Context.ullDeletedItemsSpaceKB
					, ulWillBeDeletedItems);

				if (s_stMetaRecAutoDel_Context.ullToMakeListSpaceKB <= s_stMetaRecAutoDel_Context.ullDeletedItemsSpaceKB)
				{	// needed checking done !
					HxLOG_Debug("[%s, %d] req space (%lld) satisfied\n", __FUNCTION__, __LINE__
						, s_stMetaRecAutoDel_Context.ullToMakeListSpaceKB);
					bNeedAutoDeleteCheck = FALSE;
				}
			}
		}
		pSortedListNavi = pSortedListNavi->next;
	}

	s_stMetaRecAutoDel_Context.bAutoDeleteProcessing = FALSE;
	HxLOG_Debug("[%s, %d] Finished! \n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

STATIC HERROR om_meta_rec_GetPvrSizeInfo (HUINT64 *pullFreeKbSize, HUINT64 *pullUserTotalKbSize)
{
	HUINT32				ulPvrIdx;
	SvcFs_DevIdx_e		eDevIdx;
	HERROR				hErr;

	HUINT64		ullUserTotalKbSize;
	HUINT64		ullAvailableKbSize;
	HUINT64		ullUsedKbSize;
	HUINT64		ullReservedKbSize;
	HUINT64		ullAvailablePvrKbSize;
	HUINT64		ullPvrReservedKbSize;
	HUINT64		ullRecycleBinSize;		// in Byte
	HUINT64 	ullRecycleBinKbSize;	// in KByte

	HUINT64		ullTsrKbSize;

	SvcFs_DevInfo_t	stDevInfo;

	ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
	if (ulPvrIdx == 0xffff)
	{
		HxLOG_Error("SVC_FS_GetDefaultPvrIdx failed. \n");
		return ERR_FAIL;
	}

	hErr = SVC_FS_GetPvrDevIdPartIdx(ulPvrIdx, &eDevIdx, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_FS_GetPvrDevIdPartIdx failed. pvr idx=(%d) \n", ulPvrIdx);
		return ERR_FAIL;
	}

	hErr = SVC_FS_GetSize(eDevIdx, &ullUserTotalKbSize, &ullAvailableKbSize, &ullUsedKbSize, &ullReservedKbSize,&ullAvailablePvrKbSize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_FS_GetSize failed. dev idx=(%d) \n", eDevIdx);
		return ERR_FAIL;
	}

	hErr = SVC_FS_GetPvrReservedSize(eDevIdx, &ullPvrReservedKbSize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_FS_GetPvrReservedSize failed. dev idx=(%d) \n", eDevIdx);
		return ERR_FAIL;
	}

	hErr = SVC_FS_GetDeviceInfo(eDevIdx, &stDevInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_FS_GetDeviceInfo failed. dev idx=(%d) \n", eDevIdx);
		return ERR_FAIL;
	}

	ullRecycleBinSize = SVC_META_GetRecycleBinSize(stDevInfo.szMountPath);
	ullRecycleBinKbSize = ullRecycleBinSize/1024;

	HxLOG_Debug("used(%lld) pvr(%lld) recycle bin size (%lld) \n",ullUsedKbSize,ullAvailablePvrKbSize,ullRecycleBinKbSize);

	// recycle bin에 있는 데이터가 실제로 삭제되지 않는 경우에
	// auto delete 요청시마다 계속 recording를 지우는 이슈 vs auto delete를 하지 않고 recording를 hdd full로 인해 실패하는 이슈 중 hdd full로 인해 실패하는 이슈를 가져간다.

	// 사용할 수 있는 용량이므로 pvr available에 추가한다.
	ullAvailablePvrKbSize += ullRecycleBinKbSize;

	// available에 recycleBin을 더하고 used에 recycleBin을 빼면 userTotal은 어짜피 동일하므로 available과 used에는 처리는 하지 않는다.

	ullTsrKbSize = ullAvailableKbSize + ullUsedKbSize + ullReservedKbSize + ullAvailablePvrKbSize - ullUserTotalKbSize;

	*pullFreeKbSize = (ullAvailablePvrKbSize);
	*pullUserTotalKbSize = (ullAvailableKbSize + ullUsedKbSize - ullPvrReservedKbSize);

	return ERR_OK;
}
STATIC HERROR om_meta_rec_ProcessAutoDelete (HxList_t *pstSortedRecList)
{
	HUINT32					i, nListLen = 0;
	HxList_t				*pstRecList;
	HUINT64				ullToDeleteSpaceKB = 0;
	HUINT64				ullDeletedItemsSpaceKB = 0;

	if (s_stMetaRecAutoDel_Context.bAutoDeleteProcessing)
	{
		HxLOG_Error(" Already Auto Delete Process woring ! \n");
		return ERR_FAIL;
	}

	if (MAX_AUTO_DELETE_REQUEST <= s_stMetaRecAutoDel_Context.ullToDeleteSpaceKB)
	{
		HxLOG_Error("Skip Auto Delete. MAX REQUEST SIZE : (%llu Kbyte).\n", MAX_AUTO_DELETE_REQUEST);
		return ERR_FAIL;
	}

	HxLOG_Debug(" Start! \n");
	s_stMetaRecAutoDel_Context.bAutoDeleteProcessing = TRUE;

	ullToDeleteSpaceKB = s_stMetaRecAutoDel_Context.ullToDeleteSpaceKB;
	HxLOG_Debug("\033[30;46m  Delete Request Size :  %lld \033[0m\n", ullToDeleteSpaceKB);

	// sort the list as scenario
	nListLen = HLIB_LIST_Length(pstSortedRecList);
	pstRecList = pstSortedRecList;

	for (i = 0; i < nListLen; ++i)
	{
		DxRecListData_t		*pstRecData = (DxRecListData_t *)HLIB_LIST_Data(pstRecList);
		if (pstRecData)
		{	// check the space
			if (pstRecData->bWillBeDeleted && FALSE == pstRecData->bUserLock)
			{	// delete
				BUS_PostMessage(NULL, eOEVT_METARECLIST_PROCESS_AUTO_DELETE, HANDLE_NULL, (HINT32)pstRecData->ulContentId, 0, 0);
				ullDeletedItemsSpaceKB += om_meta_rec_CalcRecDiskSpace(pstRecData->ulRecDuration, pstRecData->eVideoType == eDxVIDEO_TYPE_HD);
				HxLOG_Debug("\033[30;46m  Deleted Item :  %lld \033[0m\n", ullDeletedItemsSpaceKB);
			}

			if (ullToDeleteSpaceKB <= ullDeletedItemsSpaceKB)
			{	// needed checking done !
				HxLOG_Debug(" req space (%lld) satisfied\n", ullToDeleteSpaceKB);
				break;
			}
		}
		pstRecList = pstRecList->next;
	}

	s_stMetaRecAutoDel_Context.bAutoDeleteProcessing = FALSE;
	HxLOG_Debug(" Finished! \n");
	return ERR_OK;
}

static OmMediaRec_AutoDelRun_e	om_meta_rec_GetAutoDeleteState(HUINT64 ullReqSize, HUINT64 *pullToDeleteSize, HUINT64 *pullToMakeListSize)
{
	OmMediaRec_AutoDelRun_e	eAutoDelRun = eMETA_AutoDelete_DoNothing;
	HERROR					hErr;

	HUINT64					ullUserTotalKbSize = 0;
	HINT64					nlNeedToMakeListSpace;
	HINT64					nlNeedToRunSpace;

	HUINT64					ullUserUsageBeforeAD, ullPvrAvailableBeforeAD, ullFreeBeforeAD;	// AD: auto delete
	HINT64					nlUserUsageAfterAD, nlPvrAvailableAfterAD, nlFreeAfterAD;		// AD: auto delete

	if (pullToDeleteSize == NULL)
	{
		return eMETA_AutoDelete_DoNothing;
	}

	// init
	*pullToDeleteSize = 0;
	HxLOG_Debug("\033[30;46m  Request space for recording:  %lld \033[0m\n", ullReqSize);

	hErr = om_meta_rec_GetPvrSizeInfo(&ullFreeBeforeAD, &ullUserTotalKbSize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("om_meta_rec_GetPvrSizeInfo failed. \n");
		return eMETA_AutoDelete_DoNothing;
	}

	if (ullUserTotalKbSize == 0)
	{
		HxLOG_Error(" can't get the total size \n");
		return eMETA_AutoDelete_DoNothing;
	}

	// before
	ullPvrAvailableBeforeAD =ullFreeBeforeAD;
	ullUserUsageBeforeAD = ullUserTotalKbSize - ullPvrAvailableBeforeAD;
	HxLOG_Debug("\033[30;46m  Before Auto Delete, UserUsage is :  %lld (%lld MB) (%lld GB) (%lld %%)\033[0m\n", ullUserUsageBeforeAD, (ullUserUsageBeforeAD)/1000L, (ullUserUsageBeforeAD)/1000L/1000L, ullUserUsageBeforeAD*100/ullUserTotalKbSize);

	// request
	HxLOG_Debug("\033[30;46m  Request space for recording:  %lld \033[0m\n", ullReqSize);

	// after
	/*****************************************************************************************************************************************************************
	*  목표는 PvrAvailable Size를 (사용자가 recording 할 수 있는 total)*Run% 크기 만큼 여유 있게 유지 시키는 일이다.
	*
	*  ex ) 500GB 하드 (GB는 10^3 MB, ...)
	*   System Reserved 40GB
	*   total = 460GB =~ 438.69 GiB =~ 440 GiB
	*   PVR Reserved (TSR) = 40GiB
	*   PVR Available = Total - PVR Reserved - Used(user Usage)
	*   UI에서 Recording 관련 storage 그래프를 보여줄때는
	*       User가 가능한 size = total - pvr reserved  400GiB
	*       현재 size  = used + pvr reserved
	*       남은 size  = pvr available
	*   위와 같은 의미를 담도록 해야 한다.
	*   또한 이 경우에는 400GiB * (1-RUN%) ~= 380GiB이므로, 사용자의 Recording이 380GiB를 넘기려는 경우에 삭제 해준다.
	******************************************************************************************************************************************************************/
	// BeforePvrAvailable 보다 많은 용량을 request할 수 있으므로, signed 를 사용해야 한다.
	nlFreeAfterAD = ullPvrAvailableBeforeAD - ullReqSize;
	nlNeedToMakeListSpace = (HINT64)(ullUserTotalKbSize*CHECK_FREESPACE_PERCENT)/100;
	nlNeedToRunSpace = (HINT64)(ullUserTotalKbSize*RUN_FREESPACE_PERCENT)/100;
	if (nlFreeAfterAD < nlNeedToMakeListSpace)
	{
		if (nlFreeAfterAD < nlNeedToRunSpace)
		{
			HxLOG_Debug(" over %d. delete \n",(100-RUN_FREESPACE_PERCENT));
			eAutoDelRun = eMETA_AutoDelete_Delete;

			// AfterPvrAvailable이 Total*Run% 보다 크도록 DeleteSize를 맞춰준다.
			*pullToDeleteSize = nlNeedToRunSpace - nlFreeAfterAD;
			*pullToMakeListSize = nlNeedToMakeListSpace - nlFreeAfterAD;
		}
		else
		{
			HxLOG_Debug(" between %d ~ %d. only make list \n",(100-CHECK_FREESPACE_PERCENT),(100-RUN_FREESPACE_PERCENT));
			eAutoDelRun = eMETA_AutoDelete_MakeList;

			*pullToDeleteSize = 0;
			*pullToMakeListSize = nlNeedToMakeListSpace - nlFreeAfterAD;
		}

	}
	else
	{
		HxLOG_Debug("Still enogh PvrAvailable. Don't delete. \n");
		*pullToDeleteSize = 0;
	}


	nlPvrAvailableAfterAD = nlFreeAfterAD + *pullToDeleteSize;
	nlUserUsageAfterAD = (HINT64)ullUserTotalKbSize - nlPvrAvailableAfterAD;

	HxLOG_Debug("\033[30;46m  To Delete Size:  %lld (%lld MB) (%lld GB) \033[0m\n", *pullToDeleteSize,(*pullToDeleteSize)/1000L, (*pullToDeleteSize)/1000L/1000L);
	HxLOG_Debug("\033[30;46m  To MakeList Size:  %lld (%lld MB) (%lld GB) \033[0m\n", *pullToMakeListSize,(*pullToMakeListSize)/1000L, (*pullToMakeListSize)/1000L/1000L);
	HxLOG_Debug("\033[30;46m  After Auto Delete, UserUsage Will be :  %lld (%lld MB) (%lld GB) (%lld %%)\033[0m\n", nlUserUsageAfterAD, (nlUserUsageAfterAD)/1000L, (nlUserUsageAfterAD)/1000L/1000L, nlUserUsageAfterAD*100/ullUserTotalKbSize);

	return eAutoDelRun;
}

static	void	om_meta_rec_RefreshAutoDeleteList (void)
{
#if defined(CONFIG_APCORE_AUTO_DELETE)
	HERROR			hErr;
	HBOOL			bIsAutoDeleteOn;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUTO_DELETE, (HUINT32 *)&bIsAutoDeleteOn, 0);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_PARAM_GetItem(eDB_PARAM_ITEM_AUTO_DELETE) Error!!\n");
		bIsAutoDeleteOn = FALSE;
	}

	if (bIsAutoDeleteOn == TRUE)
	{
		BUS_PostMessage(NULL, eOEVT_METARECLIST_REFRESH_AUTO_DELETE_LIST, HANDLE_NULL, 0, 0, 0);
	}
#endif
}


#define	____OAPI_RECORDINGS_RPC_____________________________________________________________________
static HERROR	om_meta_rec_Update (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32			cid;
	DxRecListData_t	*rec;
	SvcMeta_Record_t	mwrec;
	SvcMeta_Service_t	mwsvc;


	HxLOG_Trace();

	cid = HxOBJECT_INT(argv[0]);
	rec = HxOBJECT_BIN_DATA(argv[1]);
	om_meta_rec_Unpack(rec);

	// 일단 이벤트 정보는 업데이트 하지 않는다고 가정하자
	if (OTL_CONV_DxRecListData2MW_PvrMeta(&mwrec, &mwsvc, NULL, rec) == ERR_OK)
	{
		HxLOG_Debug("%s(cid:%u, %s|\'%s\')\n", __FUNCTION__, cid, rec->szUrl, rec->szName);

		SVC_META_MuteEvent(TRUE);
			SVC_META_SetRecordInfo(cid, &mwrec);
			SVC_META_SetServiceInfo(cid, &mwsvc, TRUE);
		SVC_META_MuteEvent(FALSE);
	}

	om_meta_rec_RefreshAutoDeleteList();

	return ERR_OK;
}

static HERROR	om_meta_rec_Rename (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32			cid;
	const HCHAR *	name;
	SvcMeta_Record_t	rec;

	HxLOG_Trace();

	cid  = HxOBJECT_INT(argv[0]);
	name = HxOBJECT_STR(argv[1]);

	if (SVC_META_GetRecordInfo(cid, &rec) == ERR_OK)
	{
		HxLOG_Debug("%s(cid:%u, name:%s)\n", __FUNCTION__, cid, name);
		SVC_META_MuteEvent(TRUE);
			SVC_META_RenameContents(cid, (HUINT8 *)rec.aucPath, (HUINT8 *)rec.aucFileName, (HUINT8 *)name);
		SVC_META_MuteEvent(FALSE);
	}
	return ERR_OK;
}

static HERROR	om_meta_rec_Remove (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32		cid;
	HERROR		err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed");
	}

	HxLOG_Trace();
	cid = HxOBJECT_INT(argv[0]);

	HxLOG_Debug("%s(cid:%d)\n", __FUNCTION__, cid);

	SVC_META_MuteEvent(TRUE);
	err = SVC_META_DeleteContents(cid);
	SVC_META_MuteEvent(FALSE);
	if (err == ERR_OK)
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_OK, NULL);
	else
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);

	om_meta_rec_RefreshAutoDeleteList();

	return ERR_OK;
}

static HERROR	om_meta_rec_Move (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32			cid;
	const HCHAR *	path;
	HERROR			err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HxPair_t	*pstPair = NULL;


	pstPair = (HxPair_t *)HLIB_MEM_Calloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)requestId;
		pstPair->right = (void *)nClientHandle;
	}

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed");
	}

	HxLOG_Trace();

	cid  = HxOBJECT_INT(argv[0]);
	path = HxOBJECT_STR(argv[1]);
	HxLOG_Debug("%s(cid:%d, path:%s )\n", __FUNCTION__, cid, path);

	err  = SVC_META_MoveContents(cid, path, (HxReturnFunc_t)om_meta_rec_OnFileOpDone, (void *)pstPair);
	if (err != ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		return ERR_OK;
	}
	return ERR_OK;
}

static HERROR	om_meta_rec_Copy (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32			cid;
	const HCHAR *	path;
	HERROR			err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HxPair_t	*pstPair = NULL;


	pstPair = (HxPair_t *)HLIB_MEM_Calloc(sizeof(HxPair_t));
	if (pstPair)
	{
		pstPair->left  = (void *)requestId;
		pstPair->right = (void *)nClientHandle;
	}

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed");
	}

	HxLOG_Trace();

	cid  = HxOBJECT_INT(argv[0]);
	path = HxOBJECT_STR(argv[1]);
	HxLOG_Debug("%s(cid:%d, path:%s)\n", __FUNCTION__, cid, path);


	err  = SVC_META_CopyContents(cid, (HUINT8 *)path, (HxReturnFunc_t)om_meta_rec_OnFileOpDone, (void*)pstPair);
	if (err != ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		HxLOG_Error("Cannot copy recording\n");
		return ERR_OK;
	}
	return ERR_OK;
}

static HERROR	om_meta_rec_CancelFileOperation (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32	cid;
	HERROR	err;

	HxLOG_Trace();
	cid = HxOBJECT_INT(argv[0]);

	HxLOG_Debug("%s(cid:%d)\n", __FUNCTION__, cid);

	err = SVC_META_CancelCopyOperation(cid);
	if (err != ERR_OK)
	{
		HxLOG_Error("%s() Cannot cancel file operation!\n", __FUNCTION__);
		return ERR_OK;
	}
	return ERR_OK;
}

static HERROR	om_meta_rec_Lock (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	SvcMeta_Record_t	stRec;
	HINT32			cid;
	HBOOL			lock;

	HxLOG_Trace();
	cid  = HxOBJECT_INT(argv[0]);
	lock = HxOBJECT_INT(argv[1]) ? TRUE : FALSE;
	HxLOG_Debug("%s(cid:%d, %s)\n", __FUNCTION__, cid, lock ? "LOCK" : "UNLOCK");

	if (SVC_META_GetRecordInfo(cid, &stRec) != ERR_OK)
	{
		HxLOG_Error("%s() Cannot get record info\n", __FUNCTION__);
		return ERR_OK;
	}

	if (stRec.bUserLocked != lock || stRec.u1cLocked != lock)
	{
		stRec.bUserLocked = lock;
		stRec.u1cLocked = lock;
		SVC_META_SetRecordInfoWithOption(cid, &stRec, TRUE, FALSE);
	}
	return ERR_OK;
}

static HERROR	om_meta_rec_AddBookmark (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HERROR	err;
	HINT32	cid;
	HINT32	pos;
	const HCHAR	*name;

	HxLOG_Trace();

	cid  = HxOBJECT_INT(argv[0]);
	pos  = HxOBJECT_INT(argv[1]);
	name = HxOBJECT_STR(argv[2]);

	err = SVC_META_AddBookmark(cid, (HCHAR *)name, (HUINT32)pos, FALSE);
	if (err == ERR_OK)
		HxLOG_Debug("%s(cid:%d, pos:%d, name:%s) "HxANSI_COLOR_GREEN("SUCCESS")"\n", __FUNCTION__, cid, pos, name);
	else
		HxLOG_Debug("%s(cid:%d, pos:%d, name:%s) "HxANSI_COLOR_RED("FAIL")"\n", __FUNCTION__, cid, pos, name);
	return ERR_OK;
}

static HERROR	om_meta_rec_RemoveBookmark (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HERROR	err;
	HINT32	cid;
	HINT32	pos;
	const HCHAR	*name;
	SvcMeta_Bookmark_t	bookmark;

	HxLOG_Trace();

	cid  = HxOBJECT_INT(argv[0]);
	pos  = HxOBJECT_INT(argv[1]);
	name = HxOBJECT_STR(argv[2]);

	bookmark.ulPos = (HUINT32)pos;
	HLIB_STD_StrNCpySafe(bookmark.szName, name, SVC_PVR_BOOKMARK_NAME_LEN);
	err = SVC_META_RemoveBookmark(cid, &bookmark, FALSE);
	if (err == ERR_OK)
		HxLOG_Debug("%s(cid:%d, pos:%d, name:%s) "HxANSI_COLOR_GREEN("SUCCESS")"\n", __FUNCTION__, cid, pos, name);
	else
		HxLOG_Debug("%s(cid:%d, pos:%d, name:%s) "HxANSI_COLOR_RED("FAIL")"\n", __FUNCTION__, cid, pos, name);
	return ERR_OK;
}

static HERROR	om_meta_rec_RemoveAll (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OmMediaRecContext_t *ctx)
{
	HUINT32	removed;

	HxLOG_Trace();

	removed = OTL_HJM_RemoveRecordingList(NULL, NULL, TRUE);

	SVC_META_MuteEvent(TRUE);
		SVC_META_UnloadAllContentsInfo();
	SVC_META_MuteEvent(FALSE);

	HxLOG_Debug("%s() removed:%u\n", __FUNCTION__, removed);
	return ERR_OK;
}

#define	____OAPI_RECORDINGS_________________________________________________________________________


static void		om_meta_rec_Init (OmMediaRecContext_t *ctx)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	// set proc Name
	BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);

	// INIT context
	ctx->sd   = HLIB_RWSTREAM_Open(NULL, 0);
	ctx->srzr = HLIB_SERIALIZER_Open();

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		// REGISTER RPC Method & Callback
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_Update, "ib", (HxRPC_Func_t)om_meta_rec_Update, ctx
			, "update recording: (int contentsId, DxRecListData_t *rec)"
		);
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_Rename, "is", (HxRPC_Func_t)om_meta_rec_Rename, ctx
			, "rename recording: (int contentsId, const HCHAR *newName)"
		);
		HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_META_RECORD_Remove,"i", (HxRPC_Func_t)om_meta_rec_Remove, ctx
			, "remove recording: (int contentsId)"
		);
		HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_META_RECORD_Move, "is", (HxRPC_Func_t)om_meta_rec_Move, ctx
			, "move recording: (int contentsId, const HCHAR *destination)"
		);
		HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_META_RECORD_Copy, "is", (HxRPC_Func_t)om_meta_rec_Copy, ctx
			, "copy recording: (int contentsId, const HCHAR *destination)"
		);
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_CancelFileOperation, "i", (HxRPC_Func_t)om_meta_rec_CancelFileOperation, ctx
			, "cancel recording file operation(copy/move): (int contentsId)"
		);
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_Lock, "ii", (HxRPC_Func_t)om_meta_rec_Lock, ctx
			, "lock recording: (int contentsId, HBOOL lockIfTRUE)"
		);
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_AddBookmark, "iis", (HxRPC_Func_t)om_meta_rec_AddBookmark, ctx
			, "add bookmark: (int contentsId, int pos, const HCHAR *name)"
		);
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_RemoveBookmark, "iis", (HxRPC_Func_t)om_meta_rec_RemoveBookmark, ctx
			, "remove bookmark: (int contentsId, int pos, const HCHAR *name)"
		);
		HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_RECORD_RemoveAll, NULL, (HxRPC_Func_t)om_meta_rec_RemoveAll, ctx
			, "remove all: (void)"
		);

		HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, NULL,
			"Trigger when the recording content be updated!\n"
			"\t1. added\n"
			"\t   - \'i\'  : [added recording count]\n"
			"\t   - \'ib\' : [1][added recording:DxRecListData_t]\n"
			"\t2. updated\n"
			"\t   - \'ii\'  : [0][updated recording count]\n"
			"\t   - \'iib\' : [0][1][recording:DxRecListData_t]\n"
			"\t3. removed\n"
			"\t   - \'i\'  : [removed recording count (negative value)]\n"
			"\t   - \'ii\' : [-1][recording contents id]\n"
		);

		HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_META_RECORD_OnScanDone, "i", "Trigger when the recording scan done!\n");
	}



	(void)om_meta_rec_ComparePath;
	(void)om_meta_rec_GetSerializer;
}

static void		om_meta_rec_OnEVT_META_ADDED (OmMediaRecContext_t *ctx, HINT32 cid, HINT32 total, HINT32 added)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;




	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%d, total:%d)\n", __FUNCTION__, cid, total);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		if (cid && added == 1)
		{
			DxRecListData_t	*rec;
			void *	data;
			HUINT32	size;

			if (!SVC_META_IsTsrContents(cid))
			{
				rec = om_meta_rec_GetRecording(ctx, cid);
				if (rec == NULL)
				{
					HxLOG_Error("%s() Cannot get recording (cid:%u)\n", __FUNCTION__, cid);
					return;
				}
				data = om_meta_rec_Pack(ctx, rec, &size);

				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "ib", +1, data, size);
				HxLOG_Debug("%s() on_update(%d, %d.%s.%s)\n", __FUNCTION__, added, rec->ulContentId, rec->szUrl, rec->szName);
				DxRECLISTDATA_Delete(rec);
			}
			else
			{
				ctx->tsrCID = cid;
			}
		}
		else
		{
			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "i", added);
			HxLOG_Debug("%s() on_update(%d)\n", __FUNCTION__, added);
		}
	}
}

static void		om_meta_rec_OnEVT_META_UPDATED (OmMediaRecContext_t *ctx, HINT32 cid, HINT32 changes)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;


	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%d, changes:%d)\n", __FUNCTION__, cid, changes);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		if (cid && changes == 1)
		{
			DxRecListData_t	*rec;
			void *	data;
			HUINT32	size;

			if (!SVC_META_IsTsrContents(cid))
			{
				rec = om_meta_rec_GetRecording(ctx, cid);
				if (rec == NULL)
				{
					HxLOG_Error("%s() Cannot get recording (cid:%u)\n", __FUNCTION__, cid);
					return;
				}

				data = om_meta_rec_Pack(ctx, rec, &size);

				if (ctx->tsrCID == cid)
				{
					ctx->tsrCID = 0;
					HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "ib", +1, data, size);
					HxLOG_Debug("%s() on_update(%d, %d.%s.%s): "HxANSI_COLOR_CYAN("Delayed recording added\n"), __FUNCTION__, changes, rec->ulContentId, rec->szUrl, rec->szName);
				}
				else
				{
					HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "iib", 0, 1, data, size);
					HxLOG_Debug("%s() on_update(%d, %d.%s.%s)\n", __FUNCTION__, changes, rec->ulContentId, rec->szUrl, rec->szName);
				}
				DxRECLISTDATA_Delete(rec);
			}
		}
		else
		{
			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "ii", 0, changes);
			HxLOG_Debug("%s() on_update(%d)\n", __FUNCTION__, changes);
		}
	}
}

static void		om_meta_rec_OnEVT_META_REMOVED (OmMediaRecContext_t *ctx, HINT32 cid, HINT32 removed)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%d, removed:%d)\n", __FUNCTION__, cid, removed);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		if (cid && removed == 1)
		{
			if (SVC_META_IsTsrContents(cid))
				return;
			if (ctx->tsrCID == cid)
			{
				ctx->tsrCID = 0;
				return;
			}

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "ii", -removed, cid);
			HxLOG_Debug("%s() on_update(%d, cid:%d)\n", __FUNCTION__, -removed, cid);
		}
		else
		{
			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, "i", -removed);
			HxLOG_Debug("%s() on_update(%d)\n", __FUNCTION__, -removed);
		}
	}
}

static void		om_meta_rec_OnMSG_APP_FS_XXX (OmMediaRecContext_t *ctx, SvcFs_DevIdx_e devIdx, HINT32 pvrIdx)
{
	HxLOG_Trace();
	HxLOG_Debug("%s(devIdx:%d, pvrIdx:%d)\n", __FUNCTION__, devIdx, pvrIdx);

	if (pvrIdx < 0)
	{
		if (!SVC_FS_IsDevicePvrUsed(devIdx) || SVC_FS_GetPvrIdxByDevIdx((HUINT32 *)&pvrIdx, devIdx) != ERR_OK)
		{
			HxLOG_Error("%s(devIdx:%d) Cannot get pvr index by device index!\n", __FUNCTION__, devIdx);
			return;
		}
	}

	if (SVC_FS_GetPvrPath((HUINT32)pvrIdx, eSVC_FS_UsageType_Record, ctx->url) != ERR_OK)
	{
		HxLOG_Error("%s(pvrIdx:%d) Cannot get pvr path by pvr index!\n", __FUNCTION__, pvrIdx);
		return;
	}

	OTL_HJM_RemoveRecordingList(om_meta_rec_ComparePath, (void *)ctx->url, FALSE);
	SVC_META_UnloadContentsInfoByPath(ctx->url);

	HxLOG_Debug("%s(path:\"%s\") Unload all contents!!\n", __FUNCTION__, ctx->url);
}

static HINT32	om_meta_rec_OnScanDone (void *userdata, HINT32 count, ...)
{
	va_list		ap;
	const HCHAR	*path;

	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	va_start(ap, count);
	path   = va_arg(ap, const HCHAR *);
	va_end(ap);

	HxLOG_Trace();
	HxLOG_Debug("%s(%s, count:%d)\n", __FUNCTION__, path, count);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(ERR_OK == hErr)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_META_RECORD_OnScanDone, "i", count);
	}

	if (count > 0)
		SVC_META_LoadContentsInfo((HUINT8 *)path);
	return 0;
}

static void		om_meta_rec_OnEVT_META_RECLIST_HDD_SCANABLE (OmMediaRecContext_t *ctx, HINT32 p1, HINT32 p2)
{
	HCHAR	path[HxPATH_MAX] = {0, };
	HUINT32	ulPvrIndex = (HUINT32)p1;
	HUINT32	ulPlaybackIndex = (HUINT32)p2;

	HxLOG_Trace();
	HxLOG_Debug("(ulPvrIndex:%d, ulPlaybackIndex:%d)\n", ulPvrIndex, ulPlaybackIndex);

	// TODO: should only use ulPlaybackIndex (not ulPvrIndex)
	if (SVC_FS_GetPvrPath((HUINT32)ulPvrIndex, eSVC_FS_UsageType_Record, path) == ERR_OK)
	{
		HxLOG_Debug("add path. ulPvrIndex(%d) path[%s] \n", ulPvrIndex, path);
		if (ulPvrIndex < MAX_CONNECTABLE_PVR_STORAGE_NUM)
		{
			HxSTD_snprintf(ctx->szPvrMountPathList[ulPvrIndex],DxSTORAGE_NAME_LEN,"%s",path);
		}
	}
	else
	{
		if (SVC_FS_GetPlaybackPath((HUINT32)ulPlaybackIndex, eSVC_FS_UsageType_Record, path) == ERR_OK)
		{
			HxLOG_Debug("add path. ulPlaybackIndex(%d) path[%s] \n", ulPlaybackIndex, path);
			if (ulPlaybackIndex < MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM)
			{
				HxSTD_snprintf(ctx->szPlaybackMountPathList[ulPlaybackIndex],DxSTORAGE_NAME_LEN,"%s",path);
			}
		}
	}

	if (HxSTD_StrEmpty(path))
	{
		HxLOG_Error("Fail to find path. ulPvrIndex(%d) ulPlaybackIndex(%d) \n",ulPvrIndex, ulPlaybackIndex);
		return;
	}

	SVC_META_UnloadContentsInfoByPath(path);
	OTL_HJM_Scan(path, om_meta_rec_OnScanDone, NULL);
}

static void		om_meta_rec_OnEVT_META_RECLIST_HDD_REMOVED (OmMediaRecContext_t *ctx, HINT32 p1, HINT32 p2)
{
	HCHAR *szPath = NULL;
	HUINT32	ulPvrIndex = (HUINT32)p1;
	HUINT32	ulPlaybackIndex = (HUINT32)p2;

	HxLOG_Trace();
	HxLOG_Debug("(ulPvrIndex:%d, ulPlaybackIndex:%d)\n", ulPvrIndex, ulPlaybackIndex);

	if (ulPvrIndex < MAX_CONNECTABLE_PVR_STORAGE_NUM)
	{
		szPath = ctx->szPvrMountPathList[ulPvrIndex];
	}
	else if (ulPlaybackIndex < MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM)
	{
		szPath = ctx->szPlaybackMountPathList[ulPlaybackIndex];
	}
	else
	{
		HxLOG_Error("Fail to find path. ulPvrIndex(%d) ulPlaybackIndex(%d) \n",ulPvrIndex, ulPlaybackIndex);
		return;
	}

	HxLOG_Debug("unload path [%s] \n",szPath);

	SVC_META_UnloadContentsInfoByPath((HCHAR *)szPath);
	OTL_HJM_RemoveRecordingList(om_meta_rec_ComparePath, (void *)szPath, FALSE);
}

static void		om_meta_rec_OnEVT_META_REQUEST_AUTO_DELETE (OmMediaRecContext_t *ctx, HINT32 p1, HINT32 p2)
{
#if defined CONFIG_APCORE_AUTO_DELETE
	// check the requested space & free space
	HUINT64		ullReqSize = 0;
	HUINT64		ullToDeleteSize = 0;
	HUINT64		ullToMakeListSize = 0;
	HUINT32		ulReqSizeUpper = (HUINT32)p1;
	HUINT32		ulReqSizeLower = (HUINT32)p2;

	OmMediaRec_AutoDelRun_e	eAutoDelRun;
	HBOOL		bIsAutoDeleteOn = FALSE;
	HxList_t	*pstSortedRecList = NULL;

	(void)DB_PARAM_GetItem(eDB_PARAM_ITEM_AUTO_DELETE, (HUINT32 *)&bIsAutoDeleteOn, 0);
	if (bIsAutoDeleteOn == FALSE )
	{
		HxLOG_Debug("eDB_PARAM_ITEM_AUTO_DELETE is false. do nothing \n");
		return;
	}

	ullReqSize = (HUINT64)((HUINT64)ulReqSizeUpper << 32) | (HUINT64)ulReqSizeLower;

	HxLOG_Debug("\033[30;46m[%s : %d]  Request Size :  %lld \033[0m\n", __FUNCTION__, __LINE__, ullReqSize);
	eAutoDelRun = om_meta_rec_GetAutoDeleteState(ullReqSize,&ullToDeleteSize,&ullToMakeListSize);

	switch(eAutoDelRun)
	{
		case eMETA_AutoDelete_DoNothing:
			HxLOG_Debug("\033[30;46m  Do nothing \033[0m\n", ullReqSize);
			break;
		case eMETA_AutoDelete_MakeList:
		case eMETA_AutoDelete_Delete:
			HxLOG_Debug("\033[30;46m  run(%d) (req: %lld)(del: %lld) \033[0m\n", eAutoDelRun, ullReqSize,ullToDeleteSize);
			HxLOG_Debug("\033[30;46m  run(%d) (req: %lld)(list: %lld) \033[0m\n", eAutoDelRun, ullReqSize,ullToMakeListSize);
			om_meta_rec_InitAutoDelete(ullToDeleteSize,ullToMakeListSize, eAutoDelRun);
			pstSortedRecList = om_meta_rec_GetSortedRecList(ctx);
			if (pstSortedRecList == NULL)
			{
				HxLOG_Debug("om_meta_rec_GetSortedRecList failed. \n");
				break;
			}
			om_meta_rec_UpdateAutoDeleteList(pstSortedRecList);
			if (eAutoDelRun == eMETA_AutoDelete_Delete
				&& TRUE == bIsAutoDeleteOn)
			{
				om_meta_rec_ProcessAutoDelete(pstSortedRecList);
			}
			break;

		default:
			break;
	}

	HLIB_LIST_RemoveAllFunc(pstSortedRecList,om_meta_rec_FreeItem);

	return;
#endif
}

static void		om_meta_rec_OnEVT_META_PROCESS_AUTO_DELETE (OmMediaRecContext_t *ctx, HINT32 p1)
{
#if defined CONFIG_APCORE_AUTO_DELETE
	HUINT32		cid = (HUINT32)p1;

	HxLOG_Warning("Auto delete: delete recording. cid=(%d) \n", cid);
	SVC_META_DeleteContents(cid);
#endif
}
static void		om_meta_rec_OnEVT_META_UPDATE_AUTO_DELETE_LIST (OmMediaRecContext_t *ctx, HINT32 p1)
{
#if defined CONFIG_APCORE_AUTO_DELETE
	HxList_t	*pstSortedRecList = NULL;
	HUINT64 ullToDeleteSize = 0;
	HUINT64 ullToMakeListSize = 0;
	HBOOL	bIsToGoOff = (HBOOL)p1;	// auto delete on->off: unflag all recording's will be deleted

	if (bIsToGoOff == TRUE)
	{
		ullToDeleteSize = 0;
		ullToMakeListSize = 0;
	}
	else
	{
		(void)om_meta_rec_GetAutoDeleteState(0,&ullToDeleteSize,&ullToMakeListSize);
	}

	pstSortedRecList = om_meta_rec_GetSortedRecList(ctx);
	if (pstSortedRecList == NULL)
	{
		HxLOG_Debug("om_meta_rec_GetSortedRecList failed. \n");
		return;
	}

	om_meta_rec_InitAutoDelete(ullToDeleteSize,ullToMakeListSize, eMETA_AutoDelete_MakeList);
	om_meta_rec_UpdateAutoDeleteList(pstSortedRecList);

	HLIB_LIST_RemoveAllFunc(pstSortedRecList,om_meta_rec_FreeItem);

#endif
}

static void		om_meta_rec_OnEVT_DB_PARAM_UPDATED (OmMediaRecContext_t *ctx, HINT32 p1)
{
#if defined CONFIG_APCORE_AUTO_DELETE
	DbParam_Item_e eItem = p1;
	switch(eItem)
	{
		case eDB_PARAM_ITEM_AUTO_DELETE:
			{
				HBOOL	bIsAutoDeleteOn = FALSE;
				HERROR	hErr;
				HxLOG_Info("eDB_PARAM_ITEM_AUTO_DELETE\n");
				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUTO_DELETE, (HUINT32 *)&bIsAutoDeleteOn, 0);

				if (hErr == ERR_FAIL)
				{
					HxLOG_Error("get auto delete db param error \n");
					return;
				}

				if (bIsAutoDeleteOn == TRUE)
				{
					BUS_PostMessage(NULL, eOEVT_METARECLIST_REQUEST_AUTO_DELETE, HANDLE_NULL, 0, 0, 0);
				}
				else if (bIsAutoDeleteOn == FALSE)
				{
					BUS_PostMessage(NULL, eOEVT_METARECLIST_REFRESH_AUTO_DELETE_LIST, HANDLE_NULL, TRUE, 0, 0);
				}
				break;
			}
		default:
			break;

	}
#endif
}

static void		om_meta_rec_OnEVT_META_SHUTDOWN_DONE (OmMediaRecContext_t *ctx)
{
	HxLOG_Trace();

	HxLOG_Warning("Received shutdown Done message \n");
	OTL_HJM_RemoveRecordingList(NULL,NULL, FALSE);
}

BUS_Result_t	proc_om_meta_rec(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
	case eMEVT_BUS_CREATE:
		om_meta_rec_Init(&s_context);
		break;

	case eMEVT_BUS_DESTROY:
		break;

	case eMEVT_BUS_TIMER:
		{
			HUINT32 ulTimerId	= (HUINT32)p1;
			HUINT32 ulTimerType	= ulTimerId&OM_AUTO_DELETE_REFRESH_TIMER_ID_TYPE_MASK;
			HINT32	nGoOff	  = ulTimerId&OM_AUTO_DELETE_REFRESH_TIMER_ID_ARG_MASK;

			switch (ulTimerType)
			{
				case OM_AUTO_DELETE_REFRESH_TIMER_ID:
					BUS_KillTimer(ulTimerId);
					BUS_PostMessage(NULL, eOEVT_METARECLIST_UPDATE_AUTO_DELETE_LIST, HANDLE_NULL, nGoOff, 0, 0);
					break;
				default:
					break;
			}
		}
		break;

	case eSEVT_META_ADDED:
		om_meta_rec_OnEVT_META_ADDED(&s_context, p1, p2, p3);
		break;

	case eSEVT_META_UPDATED:
		om_meta_rec_OnEVT_META_UPDATED(&s_context, p1, p2);
		break;

	case eSEVT_META_REMOVED:
		om_meta_rec_OnEVT_META_REMOVED(&s_context, p1, p2);
		break;

	case eMEVT_META_RECLIST_HDD_REMOVED:
		om_meta_rec_OnEVT_META_RECLIST_HDD_REMOVED(&s_context, p1, p2);
		break;

	case eMEVT_META_RECLIST_HDD_SCANABLE:
		om_meta_rec_OnEVT_META_RECLIST_HDD_SCANABLE(&s_context, p1, p2);
		break;

	case eOEVT_METARECLIST_REQUEST_AUTO_DELETE:
		HxLOG_Info("eOEVT_METARECLIST_REQUEST_AUTO_DELETE \n");
		om_meta_rec_OnEVT_META_REQUEST_AUTO_DELETE(&s_context, p1, p2);
		break;

	case eOEVT_METARECLIST_PROCESS_AUTO_DELETE:
		HxLOG_Info("eOEVT_METARECLIST_PROCESS_AUTO_DELETE \n");
		om_meta_rec_OnEVT_META_PROCESS_AUTO_DELETE(&s_context, p1);
		break;

	case eOEVT_METARECLIST_UPDATE_AUTO_DELETE_LIST:
		HxLOG_Info("eSEVT_META_REQUEST_AUTO_DELETE_LIST_UPDATE \n");
		om_meta_rec_OnEVT_META_UPDATE_AUTO_DELETE_LIST(&s_context, p1);
		break;

	case eOEVT_METARECLIST_REFRESH_AUTO_DELETE_LIST:
		{
			HUINT32 ulTimerId = 0;
			HUINT32 ulGoOff = (HUINT32)p1;
			ulTimerId = OM_AUTO_DELETE_REFRESH_TIMER_ID;
			ulTimerId |= (ulGoOff&OM_AUTO_DELETE_REFRESH_TIMER_ID_ARG_MASK);
			BUS_SetTimer(ulTimerId, OM_AUTO_DELETE_REFRESH_TIME_OUT);
		}
		break;

	case eSEVT_DB_PARAM_UPDATED:
		HxLOG_Info("eSEVT_DB_PARAM_UPDATED \n");
		om_meta_rec_OnEVT_DB_PARAM_UPDATED(&s_context, p1);
		break;

	case eMEVT_BUS_SHUTDOWN_DONE:
		om_meta_rec_OnEVT_META_SHUTDOWN_DONE(&s_context);
		break;

	default:
		break;
	}
	return MESSAGE_PASS;
}

HERROR OM_META_REC_Init(void)
{
	BUS_Result_t eRes;

	eRes = BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, proc_om_meta_rec, 0, 0, 0, 0);

	return (eRes == ERR_BUS_NO_ERROR) ? ERR_OK : ERR_FAIL;
}


/* end of file */
