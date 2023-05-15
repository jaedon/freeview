/**
	@file     	mgr_media_video_playlist.c
	@brief    	Media Video Play List Manager

	Description: 	Play List\n
	Module: 		app/ap_core/action/media			\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <isosvc.h>
#include <linkedlist.h>

#include <svc_fs.h>
#include <svc_pb.h>


#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_recorder.h>

#include <mgr_media.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	POINTER_LIST_T 		*pstThumbnailList;					// file item entry list video playlist
	HUINT16				 usTotalNum;						// file item total number
	HUINT16				 usCurIdx;							// current file item
} Thumbnail_Contents_t;

/*******************************************************************/
/********************      Variables     ********************/
/*******************************************************************/

STATIC Thumbnail_Contents_t		 s_stMakeThumbnail_Contents;


/*******************************************************************/
/********************      Proto Type define     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC Thumbnail_Contents_t *local_makethumbnail_GetContents(void)
{
	return &s_stMakeThumbnail_Contents;
}

#define _____INTERNAL_MANAGING_THUMBNAIL_LIST_FUNCTIONS_____


#define _____INTERNAL_MESSAGE_PROCESSING_FUNCTIONS_____

STATIC BUS_Result_t local_makethumbnail_MsgGwmCreate (Thumbnail_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrMedia_Item_t		*pFileItem = NULL;
	POINTER_LIST_T		*pList = NULL;
	HUINT8				*pucStartupPath = NULL;
	HBOOL				bHasDir = FALSE;
	HUINT32				ulContentNum = 0, idx = 0;
	HUINT32				*pulContentId = NULL;
	SvcMeta_Record_t		stRecInfo;
	SvcMeta_Service_t 	stSvcInfo;
	HERROR 				 hErr;
	HUINT32			ulPvrIdx;
	HxSTD_memset(pstContents, 0, sizeof(Thumbnail_Contents_t));

	/* browser internal storage */

	ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
	if(0xffff == ulPvrIdx)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	pucStartupPath = MGR_RECORDER_GetRecordPath(ulPvrIdx); // 추후 수정필요
//	bHasDir = MWC_FILE_IsDirectoryExist (pucStartupPath);
	bHasDir = HLIB_DIR_IsExist((const HCHAR *)pucStartupPath);
	// Video 파일이 존재하는 HDD 가 존재하는지 체크
	if(bHasDir == FALSE)
	{
		HxLOG_Info("[local_makethumbnail_MsgGwmCreate] if(bHasDir == FALSE) MESSAGE_BREAK \n");
		// BUS_SendMessage(NULL, MSG_APP_STANDBY_THUMBNAIL_PROCESS_DONE, 0, 0, 0, 0);
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	SVC_META_UnloadContentsInfo();
	SVC_META_LoadContentsInfo(pucStartupPath);
	MGR_MEDIA_SetVideoLoadedPath(pucStartupPath);
	hErr = SVC_META_GetContentsIdList(&ulContentNum, &pulContentId);
	if(hErr == ERR_OK && ulContentNum > 0)
	{
		for(idx = 0; idx < ulContentNum; idx++)
		{
			hErr = SVC_META_GetRecordInfo(pulContentId[idx], &stRecInfo);
			if(hErr == ERR_OK)
			{
				HUINT32 *pulThumbImage = NULL;

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
				if(stRecInfo.ulRecTime < MGR_RECORDER_GetRecorderParam(eMgrRec_MIN_REQUIRED_RECORD_TIME))
#else
				if((stRecInfo.u1cScrambled == 1) ||(stRecInfo.ulRecTime < MGR_RECORDER_GetRecorderParam(eMgrRec_MIN_REQUIRED_RECORD_TIME)))
#endif
				{
					HxLOG_Info("[local_makethumbnail_MsgGwmCreate] SVC_META_GetRecordInfo() Continue ulContentId=%d %s u1cScrambled=%d ulRecTime=%d\n", pulContentId[idx], stRecInfo.aucFileName, stRecInfo.u1cScrambled, stRecInfo.ulRecTime);
					continue;
				}

				hErr = SVC_META_GetServiceInfo(pulContentId[idx], &stSvcInfo);
				if(hErr == ERR_OK && stSvcInfo.eSvcType == eDxSVC_TYPE_RADIO)
				{
					HxLOG_Info("[local_makethumbnail_MsgGwmCreate] SVC_META_GetRecordInfo() Continue ulContentId=%d %s eSvcType=%d\n", pulContentId[idx], stRecInfo.aucFileName, stSvcInfo.eSvcType);
					continue;
				}

				hErr = SVC_META_GetThumbnail(pulContentId[idx], &pulThumbImage);
				if(hErr == ERR_OK)
				{
					SVC_META_ReleaseThumbnail(pulThumbImage);
					HxLOG_Info("[local_makethumbnail_MsgGwmCreate] SVC_META_GetRecordInfo() Continue ulContentId=%d %s Already Thumbnail file\n", pulContentId[idx], stRecInfo.aucFileName);
					continue;
				}

				if(SVC_META_IsExistTSFileContents(pulContentId[idx]) == FALSE)
				{
					continue;
				}

				// Add the item
				pFileItem = OxAP_Malloc(sizeof(MgrMedia_Item_t));
				HxSTD_memset(pFileItem, 0, sizeof(MgrMedia_Item_t));
				pFileItem->ulContentID = pulContentId[idx];
#if defined(CONFIG_MW_MM_PVR)
				pFileItem->ucValidThumbImage = 0;
				pFileItem->pulThumbImage = NULL;
#endif
				pstContents->pstThumbnailList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstThumbnailList, (void *)pFileItem);
			}
			else
			{
				HxLOG_Info("[local_makethumbnail_MsgGwmCreate] SVC_META_GetRecordInfo() Failed! ulContentNum=%d MESSAGE_BREAK \n", pulContentId[idx]);
			}
		}

		if(pulContentId != NULL)
		{
			SVC_META_FreeContentsIdList(pulContentId);
		}
	}
	else
	{
		HxLOG_Info("[local_makethumbnail_MsgGwmCreate] Finish !!! ulContentNum=%d MESSAGE_BREAK \n", ulContentNum);
		// BUS_SendMessage(NULL, MSG_APP_STANDBY_THUMBNAIL_PROCESS_DONE, 0, 0, 0, 0);
		if(pulContentId != NULL)
		{
			SVC_META_FreeContentsIdList(pulContentId);
		}
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}
#if  defined(CONFIG_MW_MM_PVR)
	pstContents->usTotalNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstContents->pstThumbnailList, NULL, NULL);

	if (pstContents->usTotalNum > 0)
	{
		pList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pstThumbnailList, pstContents->usCurIdx, NULL, NULL);
		if( (pList != NULL)&& (pList->pvContents != NULL) )
		{
			pFileItem = (MgrMedia_Item_t *)pList->pvContents;
			if(pFileItem != NULL)
			{
				hErr = MGR_PVR_MakeThumbnail (pFileItem->ulContentID);
				#if 0
				hErr = MGR_ACTION_PlayRecordedFile(pFileItem->ulContentID, 0, TRUE, TRUE, TRUE, TRUE);
				#endif
				if(hErr != ERR_OK)
				{
					// BUS_SendMessage(NULL, MSG_APP_STANDBY_THUMBNAIL_PROCESS_DONE, 0, 0, 0, 0);
					BUS_MGR_Destroy (NULL);
				}
			}
		}
	}
	else
	{
		HxLOG_Info("[local_makethumbnail_MsgGwmCreate] Finish !!! usTotalNum=%d MESSAGE_BREAK \n", pstContents->usTotalNum);
		// BUS_SendMessage(NULL, MSG_APP_STANDBY_THUMBNAIL_PROCESS_DONE, 0, 0, 0, 0);
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}
#endif

	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}


// eMEVT_PVR_MEDIA_NEXT_FILE : 다음 파일 재생
STATIC BUS_Result_t local_makethumbnail_MsgAppMediaNextFile(Thumbnail_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 				err;
	MgrMedia_Item_t		*pFileItem = NULL;
	POINTER_LIST_T		*pList = NULL;

	pstContents->usCurIdx++;
	if(pstContents->usCurIdx >= pstContents->usTotalNum)
	{
		BUS_MGR_Destroy (NULL);
		// BUS_SendMessage(NULL, MSG_APP_STANDBY_THUMBNAIL_PROCESS_DONE, 0, 0, 0, 0);
		return MESSAGE_BREAK;
	}

#if  defined(CONFIG_MW_MM_PVR)
	pList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pstThumbnailList, pstContents->usCurIdx, NULL, NULL);
	if( (pList != NULL) && (pList->pvContents != NULL) )
	{
		pFileItem = (MgrMedia_Item_t *)pList->pvContents;
		if(pFileItem != NULL)
		{
			err = MGR_PVR_MakeThumbnail (pFileItem->ulContentID);
			#if 0
			err = MGR_ACTION_PlayRecordedFile(pFileItem->ulContentID, 0, TRUE, TRUE, TRUE, TRUE);
			#endif
			if(err != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
//				BUS_SendMessage(NULL, MSG_APP_STANDBY_THUMBNAIL_PROCESS_DONE, 0, 0, 0, 0);
			}
		}
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

// eMEVT_BUS_DESTROY : destroy
STATIC BUS_Result_t	local_makethumbnail_MsgGwmDestroy(Thumbnail_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	POINTER_LIST_T		*pRoot = NULL;

	if(pstContents != NULL)
	{
		BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, HANDLE_NULL, 0, 0, 0);
		if(pstContents->pstThumbnailList != NULL)
		{
			// clean up the previous file items
			pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstContents->pstThumbnailList, MGR_MEDIA_FreeMediaItem);
			if (pRoot != NULL)
			{
				HxLOG_Info("[local_makethumbnail_MsgGwmDestroy]fail to free the file list.\n");
			}
		}

		pstContents->pstThumbnailList = NULL;
		pstContents->usTotalNum = 0;
		pstContents->usCurIdx = 0;
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}


#define _____GLOBAL_FUNCTIONS_____

STATIC BUS_Result_t proc_MakeThumbnail (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eResult = ERR_BUS_NO_ERROR;
	Thumbnail_Contents_t *pstContents = local_makethumbnail_GetContents();

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			eResult = local_makethumbnail_MsgGwmCreate(pstContents, hAction, p1, p2, p3);
			break;

		// 다음 파일을 재생해 달라는 메시지
		case eMEVT_PVR_MEDIA_NEXT_FILE:
			eResult = local_makethumbnail_MsgAppMediaNextFile(pstContents, p1, p2, p3);
			break;

#if 0	// this code needs to remove in octo2
		case MSG_GWM_QUIT_UI:
			return MESSAGE_PASS;
#endif

		case eMEVT_BUS_DESTROY:
			eResult = local_makethumbnail_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		default :
			break;
	}

	if(eResult != ERR_BUS_NO_ERROR)
	{
		return eResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


BUS_Callback_t	MGR_MakeThumbnail_GetProc (void)
{
	return proc_MakeThumbnail;
}

HBOOL			MGR_MakeThumbnail_IsAvailable (void)
{
	return (BUS_MGR_Get(proc_MakeThumbnail) == NULL) ? FALSE : TRUE;
}

HERROR			MGR_MakeThumbnail_Start (void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_MakeThumbnail",	APP_ACTION_PRIORITY,
									proc_MakeThumbnail,
									HANDLE_NULL,
									0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR			MGR_MakeThumbnail_Stop (void)
{
	BUS_Result_t	eRes = ERR_BUS_NO_OBJECT;

	if (BUS_MGR_Get(proc_MakeThumbnail) != NULL)
	{
		eRes = BUS_MGR_Destroy (proc_MakeThumbnail);
	}

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}


