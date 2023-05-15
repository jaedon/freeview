

#include <hlib.h>
#include <dlib.h>
#include <otl.h>
#include <uapi.h>

#include <octo_common.h>

#include <pal_thumbnail.h>
#include <pal_crypt.h>
#include <pal_cipher.h>
#include <pal_pb.h>

#include <svc_fs.h>
#include <svc_sys.h>
#include <isosvc.h>
#include <util.h>

#include <svc_meta.h>

#define	SVC_META_TODO	do{ HxLOG_Info("%s:%d: NOT IMPLEMENTED YET!\n", __FUNCTION__, __LINE__); return ERR_FAIL; }while(0)
#define	SVC_META_ENTER	HxSEMT_Get(s_svcPvrMetaSem);
#define	SVC_META_LEAVE	HxSEMT_Release(s_svcPvrMetaSem);
#define	EPG_CONVERT_STRING_BUFSIZE		(1024*10)

#define	META_MAX_DATA_LENGTH 	256
#define SVC_META_MAX_URL_CORRECT_TIME 10


#if defined(CONFIG_OP_FREESAT_GEN2)
#define	META_MAX_EXT_TEXT_LENGTH 	(META_MAX_DATA_LENGTH*16)
#else
#define	META_MAX_EXT_TEXT_LENGTH 	META_MAX_DATA_LENGTH
#endif

#define META_MAX_LICENSE_DRM_LENGTH 1024
#define SVC_META_CAMID_LEN	8
#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
#define SVC_META_MAX_CONTENTS_IN_CACHE		16
#endif


#define	SVC_META_FOREACH(mgr,content,expr)	\
	do {\
		HxHASH_TableIter_t	__iter;\
		HLIB_HASH_IterInit(&__iter, mgr->urlMap);\
		while (HLIB_HASH_IterNext(&__iter, NULL, (void **)&(content))) {\
			expr;\
		}\
	} while (0)

//#define USE_LICENSE_DATA_JSON_STRING

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32	ulMetaVersion;
	HINT8	acModelName[20];
	HINT8	acSystemID[20];
	HINT8	acsAppVersion[20];

	HUINT32	ulPmtNumber;
	HUINT32	ulEitNumber;
	HUINT32	ulStartPosition;

	HUINT8	aucReserved[52];
} svcMetaInfoHeader_t;


typedef struct
{
	HUINT8	ucDescriptionLength;
	HUINT8	aucDescription[META_MAX_DATA_LENGTH + 1];
	HUINT8	ucItemLength;
	HUINT8	aucItem[META_MAX_DATA_LENGTH + 1];
} svcMetaExtEventItem_t;

// Freesat Gen 2 에서는 Extended Event Info 를 enhanced EIT 라는 다른 EIT 로 부터 얻어 오므로
// 처리의 용의성의 위해 svcMetaExtEvent_t 하나에 모두 정리해 저장.
// 그러므로 Text char 의 size 가 maximum size 가 되어야 함.
typedef struct
{
	HUINT8	ucDesNum;
	HUINT8	ucLastDesNum;
	HUINT8	szIso639LangCode[LANG_639_CODE_MAX+1];
	HUINT8	ucItemNum;
	svcMetaExtEventItem_t	astItem[PVR_EXT_ITEM_MAX];

	HUINT16	ucCharLength;
	HUINT8	aucChar[META_MAX_EXT_TEXT_LENGTH + 1];
} svcMetaExtEvent_t;

typedef struct
{
	UNIXTIME		ulStartTime;
	UNIXTIME		ulDuration;
//------------------ 32 bit Align -------------------------------------------
	HBIT			u3RunnigStatus			: 3;
	HBIT			u1FreeCaMode			: 1;
	HBIT			ucReserved				: 28;
//-----------------------------------------------------------------------

	HUINT8			ucParentalRatingNum;
	SvcMeta_Rating_t	astParentalRating[PVR_PARENTAL_RATING_MAX];
	HUINT8			ucContentNibble;
	HUINT8			ucUserNibble;

	HUINT8			aucEventName[PVR_SHORT_EVENT_NAME_LEN + 1];
	HUINT8			aucEventText[PVR_EVENT_TEXT+ 1];
	HINT32			nEvtId;

	SvcMeta_Component_t astComponent[EPG_COMPONENT_DESC_MAX];
	HUINT8			aucReserved[32];

#if defined(CONFIG_OP_FREESAT)
	SvcMeta_FtaCntMgmt_t	ftaCntMgmt;
	SvcMeta_Guidance_t		guidance;
#endif

	HUINT32			ulExtEvtNum;
}svcMetaEvent_t;

typedef struct
{
	DxCopyrightControl_t stDrm;
	HUINT32 ulCrc;
	HUINT8 aucRerseve[60];
} svcMetaDrmInfo_t;

typedef struct
{
	HUINT16	usProgramNum;
	HUINT8 	aucLicenseData[META_MAX_LICENSE_DRM_LENGTH];			// 1024 is the max length of CI+ License information
	HUINT16	usLicenseLength;
	HUINT8	aucBindedCamId[SVC_META_CAMID_LEN];
} svcMetaLicenseDrmInfo_t;

typedef struct
{
	svcMetaInfoHeader_t			stHeader; 		// Header Info
	SvcMeta_Record_t				stRecordInfo;  	// Record Info
	SvcMeta_Service_t				stServiceInfo; 	// Service Info
	svcMetaEvent_t			stEventInfo;	// Event Info
	svcMetaDrmInfo_t			stFileDrm; 		// DRM info
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	svcMetaLicenseDrmInfo_t	stLicenseDrm;
#endif
} svcMetaContents_t;

static HUINT32 s_ulMetaSem = 0;

static  HULONG local_Meta_GetSem( void )
{
	return s_ulMetaSem;
}

typedef struct
{
	HUINT32			cid;
	HINT32			reqId;
	HCHAR			path[DxRECLIST_URL];
	HxReturnFunc_t	onDone;
	void			*userdata;
} svcMetaFOP_t;

typedef struct
{
	HxHASH_t	*urlMap;
	HxTREE_t		*trashUrl;
	HxHASH_t	*fopMap;
	HxCBUF_t			textBuf;
	DxRecListData_t	tempRec;
	HBOOL			muteEvent;

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	HxList_t		*fullContentCache;
#endif
} svcMetaMgr_t;


typedef struct
{
	Handle_t	hAction;
	HUINT32		ulContentId;

	SvcMeta_UsingActionType_e	ePvrActionType;

} svcMetaResAction_t;



#define __VAR__
STATIC HUINT32	s_svcPvrMetaSem;
STATIC HUINT8	s_aucDes_Key[] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6};
STATIC HxList_t	*s_pstMetaResAction_List = NULL;
STATIC PostMsgToMgrCb_t		s_fnSvcMetaPostMsgToMgr = NULL;


#define __PROTOTYPE__
STATIC void		svc_meta_EmitEvent(svcMetaMgr_t *mgr, HINT32 evt, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC HINT32 svc_meta_PostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
STATIC svcMetaMgr_t *	svc_meta_GetManager (void);
STATIC void 	svc_meta_ReleaseManager (svcMetaMgr_t *mgr);
//STATIC void		svc_meta_OnCopyDone (const UAPI_NotiInfo_t *info, void *pvUserData);
STATIC HERROR	svc_meta_OnCopyDone (void *userdata, HINT32 nRequestId, UAPI_STATUS_t *pstInfo);
//STATIC void		svc_meta_OnMoveDone (const UAPI_NotiInfo_t *info, void *pvUserData);
STATIC HERROR	svc_meta_OnMoveDone (void *userdata, HINT32 nRequestId, UAPI_STATUS_t *pstInfo);

STATIC SvcMeta_Contents_t *	svc_meta_ConvertToSvc (SvcMeta_Contents_t *content, const DxRecListData_t *rec);


#define __LOCAL_FUNC__
STATIC HINT32 svc_meta_PostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcMetaPostMsgToMgr)
	{
		return (* s_fnSvcMetaPostMsgToMgr)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return ERR_OK;
}

STATIC HCHAR *	svc_meta_GetBuf (svcMetaMgr_t *mgr, HUINT32 size)
{
	return HLIB_CBUF_GetBuffer(mgr->textBuf, (HINT32)size);
}

STATIC DxRecListData_t *	svc_meta_GetTempRec (svcMetaMgr_t *mgr)
{
	DxRecListData_t	*rec = &(mgr->tempRec);
	memset(rec, 0, sizeof(DxRecListData_t));
	return rec;
}

STATIC HBOOL	svc_meta_IsPvrPath (const HCHAR *path)
{
	SvcFs_DevIdx_e	devIdx;
	HUINT32			prtIdx;

	if (SVC_FS_GetDevPartIndexByPath((HUINT8 *)path, &devIdx, &prtIdx) != ERR_OK)
		return FALSE;
	return SVC_FS_IsDevicePvrUsed(devIdx);
}

STATIC HBOOL	svc_meta_IsPlaybackPath (const HCHAR *path)
{
	SvcFs_DevIdx_e	devIdx;
	HUINT32			prtIdx;
	HBOOL			bPvr;

	if (SVC_FS_GetDevPartIndexByPath((HUINT8 *)path, &devIdx, &prtIdx) != ERR_OK)
		return FALSE;

	bPvr = SVC_FS_IsDevicePvrUsed(devIdx);
	if (bPvr == TRUE)
	{
		return TRUE;
	}

	return SVC_FS_IsDeviceRecStorageUsed(devIdx);
}

STATIC HCHAR *	svc_meta_MakePath (svcMetaMgr_t *mgr, SvcMeta_Contents_t *content, const HCHAR *ext)
{
	HCHAR	*path;

	path = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	HLIB_PATH_Join(content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName, path, DxRECLIST_URL);
	if (ext)
		HLIB_STD_StrAppend(path, ext, DxRECLIST_URL);
	return path;
}

STATIC HBOOL	svc_meta_ComparePath (const DxRecListData_t *rec, void *path)
{
	return HLIB_STD_StrStartWith(rec->stMeta.szPath, path);
}

STATIC HINT32	svc_meta_CompareBookmark (const SvcMeta_Bookmark_t *lhs, const SvcMeta_Bookmark_t *rhs)
{
	if (lhs->ulPos < rhs->ulPos)
		return -1;
	if (lhs->ulPos > rhs->ulPos)
		return +1;
	return strcmp(lhs->szName, rhs->szName);
}

STATIC HINT32	svc_meta_ComparePvrEvent (const SvcMeta_Event_t *lhs, const SvcMeta_Event_t *rhs)
{
	SvcMeta_Event_t	levt;
	SvcMeta_Event_t	revt;
	HINT32			diff;

	HxSTD_memcpy(&levt, lhs, sizeof(SvcMeta_Event_t));
	HxSTD_memcpy(&revt, rhs, sizeof(SvcMeta_Event_t));

	levt.pstExtEvt = NULL;
	revt.pstExtEvt = NULL;

	diff = HxSTD_memcmp(&levt, &revt, sizeof(SvcMeta_Event_t));
	if (diff != 0)
		return diff;

	if (lhs->pstExtEvt == rhs->pstExtEvt)
		return 0;

	if (lhs->pstExtEvt == NULL)
		return -1;
	if (rhs->pstExtEvt == NULL)
		return +1;
	return +1;
}

STATIC HINT32	svc_meta_GetServiceUID (DxRecListData_t *rec)
{
	HINT32			i, n;
	Handle_t		*list;
	HINT32			uid;
	DbSvc_TripleId_t 	triplet;
	HINT32			lcn;

	triplet.usOnId  = rec->stMeta.usOnId;
	triplet.usTsId  = rec->stMeta.usTsId;
	triplet.usSvcId = rec->stMeta.usSvcId;
	lcn          = rec->nSvcNo;

#if defined(CONFIG_OP_FREESAT)
	if(!(rec->extension.freesat.usMasterOnId == 0 &&
		rec->extension.freesat.usMasterTsId == 0 &&
		rec->extension.freesat.usMasterSvcId == 0 &&
		rec->extension.freesat.nMasterChNumber == 0))
	{
		triplet.usOnId 	= rec->extension.freesat.usMasterOnId;
		triplet.usTsId	= rec->extension.freesat.usMasterTsId;
		triplet.usSvcId 	= rec->extension.freesat.usMasterSvcId;
		lcn		= rec->extension.freesat.nMasterChNumber;
	}
#endif

	if (DB_SVC_FindServiceListByTripleId(eDxDELIVERY_TYPE_ALL, triplet, &n, &list) != ERR_OK)
	{
		HxLOG_Debug("%s(%s) Cannot get service UID\n", __FUNCTION__, rec->szName);
		return 0;
	}

	for (i = 0, uid = 0 ; i < n ; i++)
	{
		DbSvc_Info_t	svcInfo;

		if (DB_SVC_GetServiceInfo (list[i], &svcInfo) != ERR_OK)
			continue;

		if (lcn == DbSvc_GetLcn(&svcInfo))
		{
			HxLOG_Debug("%s(%s) found it!! (%d.%s)\n", __FUNCTION__, rec->szName, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo));
			uid = DB_SVC_GetServiceUID(list[i]);
			break;
		}
	}
	DB_SVC_FreeServiceList(list);
	return uid;
}


STATIC void		svc_meta_ValidateField (DxRecListData_t *rec)
{
	HCHAR	uuid[64];

	DxRECLISTDATA_ValidateName(rec);

	HLIB_PATH_Join(rec->stMeta.szPath, rec->stMeta.szFileName, rec->szThumbnailPath, DxRECLIST_URL);
	HLIB_STD_StrAppend(rec->szThumbnailPath, ".png", DxRECLIST_URL);

	// THUMBNAIL PATH
	if (!HLIB_FILE_Exist(rec->szThumbnailPath))
	{
		rec->szThumbnailPath[0] = '\0';
	}

	// SVC UID
	rec->svcUId = svc_meta_GetServiceUID(rec);

	// UUID
	if (SVC_FS_GetUuidByPath(rec->stMeta.szPath, uuid) != ERR_OK)
		uuid[0] = '\0';
	HLIB_STD_StrNCpySafe(rec->szStorageId, uuid, DxRECLIST_STORAGE_ID_LEN);

	if (HxSTD_StrEmpty(rec->szUrl)
		|| !HLIB_STD_StrStartWith(rec->szUrl, "HPVR://")
		|| !HLIB_STD_StrStartWith(rec->szUrl+7, uuid))
	{
		// URL
		DxRECORDING_MakeURL(rec->szUrl, DxRECLIST_URL
			, rec->szStorageId
			, rec->stMeta.usOnId
			, rec->stMeta.usTsId
			, rec->stMeta.usSvcId
			, rec->svcUId
			, rec->ulRecStartTime == 0 ? HLIB_STD_GetSystemTime() : rec->ulRecStartTime
		);
	}
}

STATIC void 	svc_meta_HjmPostProcessor (DxRecListData_t *rec)
{
	svc_meta_ValidateField(rec);

	if (rec->ulRecDuration == 0)
	{
		HCHAR	path[HxPATH_MAX];
		HUINT32	duration;

		HLIB_PATH_Join(rec->stMeta.szPath, rec->stMeta.szFileName, path, HxPATH_MAX);
		if (PAL_PB_GetDurationByIndex(path, (HULONG *)&duration) == ERR_OK)
		{
			rec->ulRecDuration = duration;
		}
	}
}


STATIC void		svc_meta_FreeRecordingMember (DxRecListData_t *rec)
{
	if (rec->pstExtEvts)
	{
		HLIB_STD_MemFree(rec->pstExtEvts);
		rec->pstExtEvts = NULL;
	}

}


STATIC SvcMeta_ExtEvt_t * svc_meta_CloneExtEvt( const SvcMeta_ExtEvt_t *org )
{
	HUINT8 		i;
	SvcMeta_ExtEvt_t *src= NULL, *dst = NULL;
	SvcMeta_ExtEvt_t *clone;

	if(org == NULL)
		return NULL;

	clone  = (SvcMeta_ExtEvt_t *)HLIB_STD_MemCalloc(sizeof(SvcMeta_ExtEvt_t));
	if (clone == NULL)
		return NULL;

	src = (SvcMeta_ExtEvt_t *)org;
	dst = clone;

	while( src != NULL )
	{
		dst->ucDesNum = src->ucDesNum;
		dst->ucLastDesNum = src->ucLastDesNum;
		HxSTD_MemCopy( dst->szIso639LangCode, src->szIso639LangCode, 4 );
		dst->ucItemNum = src->ucItemNum;

		if( src->pszChar != NULL )
			dst->pszChar = HLIB_STD_StrDup(src->pszChar);

		for( i=0; i<src->ucItemNum; i++ )
		{
			if( src->astItem[i].pszDescription != NULL )
				dst->astItem[i].pszDescription = HLIB_STD_StrDup(src->astItem[i].pszDescription);

			if( src->astItem[i].pszItem != NULL )
				dst->astItem[i].pszItem = HLIB_STD_StrDup(src->astItem[i].pszItem);
		}

		src = src->pNext;
		if( src != NULL )
		{
			dst->pNext = (SvcMeta_ExtEvt_t *)HLIB_STD_MemCalloc( sizeof(SvcMeta_ExtEvt_t) );
			if( dst->pNext == NULL )
			{

				HxLOG_Error("HLIB_STD_MemCalloc Error,  dst->pNext\n");

				break;
			}
			dst = dst->pNext;
		} else {
			dst->pNext = NULL;
		}
	}
	return clone;
}

STATIC void		svc_meta_FreeExtEvt (SvcMeta_ExtEvt_t *pstExtEvt)
{
	HUINT32 i;
	SvcMeta_ExtEvt_t *pstOrgEvt= NULL;

	if( pstExtEvt == NULL ) return;

	while( pstExtEvt != NULL )
	{
		if( pstExtEvt->pszChar != NULL )
		{
			HLIB_STD_MemFree( pstExtEvt->pszChar );
			pstExtEvt->pszChar = NULL;
		}

		for( i=0; i<pstExtEvt->ucItemNum; i++ )
		{
			if( pstExtEvt->astItem[i].pszDescription != NULL )
			{
				HLIB_STD_MemFree( pstExtEvt->astItem[i].pszDescription );
				pstExtEvt->astItem[i].pszDescription = NULL;
			}

			if( pstExtEvt->astItem[i].pszItem != NULL )
			{
				HLIB_STD_MemFree( pstExtEvt->astItem[i].pszItem );
				pstExtEvt->astItem[i].pszItem = NULL;
			}
		}

		pstOrgEvt = pstExtEvt;
		pstExtEvt = pstExtEvt->pNext;
		if( pstOrgEvt != NULL )
		{
			HLIB_STD_MemFree( pstOrgEvt );
		}
	}
}

STATIC void		svc_meta_FreeContents (void *pvContents)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	SvcMeta_Contents_t		*pstContents = (SvcMeta_Contents_t *)pvContents;

	if (pstContents)
	{
		if (pstContents->stEventInfo.pstExtEvt)
			svc_meta_FreeExtEvt(pstContents->stEventInfo.pstExtEvt);

		HLIB_STD_MemFree(pstContents);
	}
}
#else
{
	SvcMeta_BriefContents_t	*pstBriefContents = (SvcMeta_BriefContents_t *)pvContents;

	if (pstBriefContents)
	{
		switch (pstBriefContents->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
			{
				SvcMeta_Contents_t		*pstFullContents = (SvcMeta_Contents_t *)pvContents;

				if (pstFullContents->stEventInfo.pstExtEvt)
				{
					svc_meta_FreeExtEvt(pstFullContents->stEventInfo.pstExtEvt);
				}
			}

			break;

		case eSVC_META_CONTENTS_OBJTYPE_Brief:
			if (NULL != pstBriefContents->pszUrl)
			{
				HLIB_STD_MemFree(pstBriefContents->pszUrl);
				pstBriefContents->pszUrl = NULL;
			}

			if (NULL != pstBriefContents->pszHjmPath)
			{
				HLIB_STD_MemFree(pstBriefContents->pszHjmPath);
				pstBriefContents->pszHjmPath = NULL;
			}

			break;

		default:
			break;
		}

		HLIB_STD_MemFree(pstBriefContents);
	}
}
#endif
}

STATIC SvcMeta_Service_t *	svc_meta_MakePvrServiceBySvcHandle (SvcMeta_Service_t *svc, Handle_t svcHandle)
{
	DbSvc_Info_t 	svcInfo;

	HxSTD_MemSet(&svcInfo,0,sizeof(DbSvc_Info_t));

	if (DB_SVC_GetServiceInfo(svcHandle, &svcInfo) != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo svcHandle:0x%X\n", svcHandle);
		//return NULL;
	}

	svc->usSvcId = DbSvc_GetSvcId(&svcInfo);
	svc->usTsId = DbSvc_GetTsId(&svcInfo);
	svc->usOnId = DbSvc_GetOnId(&svcInfo);
	svc->usPmtPid = DbSvc_GetPmtPid(&svcInfo);
	svc->usPcrPid = DbSvc_GetPcrPid(&svcInfo);
	svc->usTtxPid = DbSvc_GetTtxPid(&svcInfo);
	svc->usVideoPid = DbSvc_GetVideoPid(&svcInfo);
	svc->eVideoCodec = DbSvc_GetVideoCodec(&svcInfo);
	svc->usAudioPid = DbSvc_GetAudioPid(&svcInfo);
	svc->eAudioCodec = DbSvc_GetAudioCodec(&svcInfo);
	svc->usAudioAuxPid = DbSvc_GetAudioAuxPid(&svcInfo);
	svc->eAudioAuxCodec = DbSvc_GetAudioAuxCodec(&svcInfo);
	svc->usDolbyPid = DbSvc_GetDolbyPid(&svcInfo);
	svc->eDolbyCodec = DbSvc_GetDolbyCodec(&svcInfo);
	svc->bDolbyFlag = DbSvc_GetDolbyFlag(&svcInfo);
	svc->eSvcType = DbSvc_GetSvcType(&svcInfo);
	svc->eVideoType = DbSvc_GetVideoType(&svcInfo);
	svc->eCasType = DbSvc_GetCasType(&svcInfo);
	svc->ucSubttlIdx = DbSvc_GetSubttlIdx(&svcInfo);
	svc->bSubttlUserFlag = DbSvc_GetSubttlFlag(&svcInfo);
	svc->eSoundMode = DbSvc_GetStereoSelect(&svcInfo);
//	svc->eDualMono = eDxDUALMONO_SELECT_BOTH; /* DualMono 설정의 기본값은 BOTH이다. */
	svc->eDualMono = DbSvc_GetDualmonoSelect(&svcInfo);
	svc->bAudUserSetFlag = DbSvc_GetAudUserSelected(&svcInfo);
	svc->nChNumber = DbSvc_GetLcn(&svcInfo);
	HxSTD_MemCopy(svc->aucServiceName, DbSvc_GetName(&svcInfo), /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength());

	return svc;
}

STATIC DbSvc_Info_t * svc_meta_MakeSvcInfoByMetaSvc(SvcMeta_Service_t *	MetaSvc, DbSvc_Info_t * svc)
{
	DbSvc_SetSvcId(svc, MetaSvc->usSvcId);
	DbSvc_SetTsId(svc, MetaSvc->usTsId);
	DbSvc_SetOnId(svc, MetaSvc->usOnId);
	DbSvc_SetPmtPid(svc, MetaSvc->usPmtPid);
	DbSvc_SetPcrPid(svc, MetaSvc->usPcrPid);
	DbSvc_SetTtxPid(svc, MetaSvc->usTtxPid);
	DbSvc_SetVideoPid(svc, MetaSvc->usVideoPid);
	DbSvc_SetVideoCodec(svc, MetaSvc->eVideoCodec);
	DbSvc_SetAudioPid(svc, MetaSvc->usAudioPid);
	DbSvc_SetAudioCodec(svc, MetaSvc->eAudioCodec);
	DbSvc_SetAudioAuxPid(svc, MetaSvc->usAudioAuxPid);
	DbSvc_SetAudioAuxCodec(svc, MetaSvc->eAudioAuxCodec);
	DbSvc_SetDolbyPid(svc, MetaSvc->usDolbyPid);
	DbSvc_SetDolbyCodec(svc, MetaSvc->eDolbyCodec);
	DbSvc_SetDolbyFlag(svc, MetaSvc->bDolbyFlag);
	DbSvc_SetSvcType(svc, MetaSvc->eSvcType);
	DbSvc_SetVideoType(svc, MetaSvc->eVideoType);
	DbSvc_SetCasType(svc, MetaSvc->eCasType);
	DbSvc_SetSubttlIdx(svc, MetaSvc->ucSubttlIdx);
	DbSvc_SetSubttlFlag(svc, MetaSvc->bSubttlUserFlag);
	DbSvc_SetStereoSelect(svc, MetaSvc->eSoundMode);
	DbSvc_SetDualmonoSelect(svc, MetaSvc->eDualMono);
	DbSvc_SetAudUserSelected(svc, MetaSvc->bAudUserSetFlag);
	DbSvc_SetLcn(svc, MetaSvc->nChNumber);
	DbSvc_SetName(svc, MetaSvc->aucServiceName);

	return svc;
}

STATIC SvcMeta_Service_t * svc_meta_MakeMetaSvcByMetaSvcInfo(DbSvc_Info_t * svcInfo, SvcMeta_Service_t *	svc)
{
	svc->usSvcId = DbSvc_GetSvcId(svcInfo);
	svc->usTsId = DbSvc_GetTsId(svcInfo);
	svc->usOnId = DbSvc_GetOnId(svcInfo);
	svc->usPmtPid = DbSvc_GetPmtPid(svcInfo);
	svc->usPcrPid = DbSvc_GetPcrPid(svcInfo);
	svc->usTtxPid = DbSvc_GetTtxPid(svcInfo);
	svc->usVideoPid = DbSvc_GetVideoPid(svcInfo);
	svc->eVideoCodec = DbSvc_GetVideoCodec(svcInfo);
	svc->usAudioPid = DbSvc_GetAudioPid(svcInfo);
	svc->eAudioCodec = DbSvc_GetAudioCodec(svcInfo);
	svc->usAudioAuxPid = DbSvc_GetAudioAuxPid(svcInfo);
	svc->eAudioAuxCodec = DbSvc_GetAudioAuxCodec(svcInfo);
	svc->usDolbyPid = DbSvc_GetDolbyPid(svcInfo);
	svc->eDolbyCodec = DbSvc_GetDolbyCodec(svcInfo);
	svc->bDolbyFlag = DbSvc_GetDolbyFlag(svcInfo);
	svc->eSvcType = DbSvc_GetSvcType(svcInfo);
	svc->eVideoType = DbSvc_GetVideoType(svcInfo);
	svc->eCasType = DbSvc_GetCasType(svcInfo);
	svc->ucSubttlIdx = DbSvc_GetSubttlIdx(svcInfo);
	svc->bSubttlUserFlag = DbSvc_GetSubttlFlag(svcInfo);
	svc->eSoundMode = DbSvc_GetStereoSelect(svcInfo);
//	svc->eDualMono = eDxDUALMONO_SELECT_BOTH; /* DualMono 설정의 기본값은 BOTH이다. */
	svc->eDualMono = DbSvc_GetDualmonoSelect(svcInfo);
	svc->bAudUserSetFlag = DbSvc_GetAudUserSelected(svcInfo);
	svc->nChNumber = DbSvc_GetLcn(svcInfo);
#if defined(SVC_AUDIO_LANGUAGE_CODE)
	HxSTD_StrNCpy(svc->audioLanguage, DbSvc_GetAudioLanguage(svcInfo), DxLANG_CODE_LEN);
	svc->audioLanguage[DxLANG_CODE_LEN-1] = 0;
	svc->audioDescription = DbSvc_GetAdFlag(svcInfo);
#endif
	HxSTD_MemCopy(svc->aucServiceName, DbSvc_GetName(svcInfo), /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength());

	return svc;
}

STATIC void	svc_meta_PrintList (svcMetaMgr_t *mgr)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	HxHASH_TableIter_t	iter;
	SvcMeta_Contents_t	*content;

	HLIB_HASH_IterInit(&iter, mgr->urlMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&content))
	{
		HxLOG_Debug("\t%d.%s \'%s%s\'\n"
			, content->ulContentsId, content->szURL, content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName);
	}
}
#else
{
	HxHASH_TableIter_t		 iter;
	SvcMeta_Contents_t		*pstContent;
	SvcMeta_BriefContents_t	*pstBriefContent;

	HLIB_HASH_IterInit(&iter, mgr->urlMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&pstBriefContent))
	{
		switch(pstBriefContent->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
			pstContent = (SvcMeta_Contents_t *)pstBriefContent;
			HxLOG_Debug("\t(FULL Data)  %d.%s \'%s%s\'\n"
				, pstContent->ulContentsId, pstContent->szURL, pstContent->stRecordInfo.aucPath, pstContent->stRecordInfo.aucFileName);
			break;

		case eSVC_META_CONTENTS_OBJTYPE_Brief:
			HxLOG_Debug("\t(Brief Data) %d.%s \'%s\'\n"
				, pstBriefContent->ulContentsId, pstBriefContent->pszUrl, pstBriefContent->pszHjmPath);
			break;

		default:
			HxLOG_Debug("\tUnknown Type\n");
			break;
		}

	}
}
#endif
}

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
STATIC HERROR svc_meta_InsertFullContentToCache (svcMetaMgr_t *mgr, SvcMeta_Contents_t *content)
{
	HUINT32					 ulFullContentsLen;
	HUINT32					 ulUrlLen;
	HUINT32					 ulHjmPathLen;
	HxList_t				*pstListItem, *pstNextItem;
	SvcMeta_Contents_t		*pstFullContent = NULL;
	SvcMeta_BriefContents_t *pstBriefContent = NULL;

	mgr->fullContentCache = HLIB_LIST_Remove(mgr->fullContentCache, (const void *)content);
	mgr->fullContentCache = HLIB_LIST_Append(mgr->fullContentCache, (void *)content);

	ulFullContentsLen = HLIB_LIST_Length(mgr->fullContentCache);

	if (ulFullContentsLen > SVC_META_MAX_CONTENTS_IN_CACHE)
	{
		//  You cannot run the for loop with pstListItem = HLIB_LIST_Next(pstListItem)
		//  Because the item may deleted in the loop: then it will corrupt:
		for (pstListItem = mgr->fullContentCache; NULL != pstListItem; pstListItem = pstNextItem)
		{
			pstNextItem = HLIB_LIST_Next(pstListItem);
			pstFullContent = (SvcMeta_Contents_t *)HLIB_LIST_Data(pstListItem);
			if (NULL == pstFullContent)					{ continue; }
			if ((eDxREC_STS_RECORD_START == pstFullContent->stRecordInfo.ucRecordStatus) ||
				(eDxREC_STS_RECORDING == pstFullContent->stRecordInfo.ucRecordStatus))
			{
				continue;
			}
			
			pstBriefContent = (SvcMeta_BriefContents_t *)HLIB_STD_MemCalloc(sizeof(SvcMeta_BriefContents_t));
			if (NULL != pstBriefContent)
			{
				pstBriefContent->eObjectType = eSVC_META_CONTENTS_OBJTYPE_Brief;
				pstBriefContent->ulContentsId = pstFullContent->ulContentsId;
			
				ulUrlLen = HxSTD_StrLen(pstFullContent->szURL);
				ulHjmPathLen = HxSTD_StrLen(pstFullContent->stRecordInfo.aucPath) + HxSTD_StrLen(pstFullContent->stRecordInfo.aucFileName) + 4 + 4;
			
				pstBriefContent->pszUrl = (HCHAR *)OxMW_Calloc(ulUrlLen + 1);
				pstBriefContent->pszHjmPath = (HCHAR *)OxMW_Calloc(ulHjmPathLen + 1);
			
				if ((NULL != pstBriefContent->pszUrl) && (NULL != pstBriefContent->pszHjmPath))
				{
					HxSTD_StrNCpy(pstBriefContent->pszUrl, pstFullContent->szURL, ulUrlLen);
					pstBriefContent->pszUrl[ulUrlLen] = '\0';
			
					HLIB_PATH_Join(pstFullContent->stRecordInfo.aucPath, pstFullContent->stRecordInfo.aucFileName, pstBriefContent->pszHjmPath, ulHjmPathLen);
					HLIB_STD_StrAppend(pstBriefContent->pszHjmPath, EXT_META_FILE, ulHjmPathLen);
					pstBriefContent->pszHjmPath[ulHjmPathLen] = '\0';
			
					mgr->fullContentCache = HLIB_LIST_Remove(mgr->fullContentCache, (const void *)pstFullContent);
					HLIB_HASH_Replace(mgr->urlMap, (void *)pstBriefContent->pszUrl, (void *)pstBriefContent);

					ulFullContentsLen--;
					if (ulFullContentsLen <= SVC_META_MAX_CONTENTS_IN_CACHE)
					{
						break;
					}
				}
				else
				{
					if (NULL != pstBriefContent->pszUrl)
					{
						OxMW_Free(pstBriefContent->pszUrl);
					}
					
					if (NULL != pstBriefContent->pszHjmPath)
					{
						OxMW_Free(pstBriefContent->pszHjmPath);
					}

					OxMW_Free(pstBriefContent);
				}
			}
		}	
	}

	return ERR_OK;
}
#endif

STATIC SvcMeta_Contents_t *	svc_meta_GetContents (svcMetaMgr_t *mgr, const HCHAR *url)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	SvcMeta_Contents_t	*content;

	content = (SvcMeta_Contents_t *)HLIB_HASH_Lookup(mgr->urlMap, (void *)url);
	if (content == NULL)
	{
		HxLOG_Error("%s(url:%s) Cannot find contents by URL\n", __FUNCTION__, url);
		svc_meta_PrintList(mgr);
	}
	return content;
}
#else
{
	SvcMeta_BriefContents_t		*pstBriefContent;
	SvcMeta_Contents_t			*pstContent = NULL;

	pstBriefContent = (SvcMeta_BriefContents_t *)HLIB_HASH_Lookup(mgr->urlMap, (void *)url);
	if (pstBriefContent == NULL)
	{
		HxLOG_Error("%s(url:%s) Cannot find contents by URL\n", __FUNCTION__, url);
		svc_meta_PrintList(mgr);
		return NULL;
	}

	switch (pstBriefContent->eObjectType)
	{
	case eSVC_META_CONTENTS_OBJTYPE_FullSize:
		pstContent = (SvcMeta_Contents_t *)pstBriefContent;
		break;

	case eSVC_META_CONTENTS_OBJTYPE_Brief:
	{
		DxRecListData_t		*pstItem;

		pstItem = OTL_HJM_GetRecording(url);
		if (NULL != pstItem)
		{
			pstContent = svc_meta_ConvertToSvc(NULL, pstItem);
			if (NULL != pstContent)
			{
				HLIB_HASH_Replace(mgr->urlMap, (void *)pstContent->szURL, (void *)pstContent);
			}

			DxRECLISTDATA_Delete(pstItem);
		}

		break;
	}

	default:
		break;
	}

	if (NULL != pstContent)
	{
		(void)svc_meta_InsertFullContentToCache(mgr, pstContent);
	}

	return pstContent;
}
#endif
}

STATIC SvcMeta_Contents_t *	svc_meta_GetContentsByID (svcMetaMgr_t *mgr, HUINT32 id)
{
	HCHAR	*url;
	SvcMeta_Contents_t *content;

	url = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	url = OTL_HJM_GetURL(id, url, DxRECLIST_URL);
	if (url == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find url by ID\n", __FUNCTION__, id);
		return NULL;
	}

	content = svc_meta_GetContents(mgr, url);
	if (content == NULL)
	{
		HxLOG_Error("(id:%d) cannot find content. url=[%s]\n ",id, url);
	}
	return content;
}

STATIC SvcMeta_Contents_t *	svc_meta_GetContentsByPath (svcMetaMgr_t 	*mgr, const HCHAR *inpath)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	SvcMeta_Contents_t	*content;
	HCHAR				*path;
	HCHAR				*key;

	key  = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	path = svc_meta_GetBuf(mgr, DxRECLIST_URL);

	HLIB_PATH_SplitExt(inpath, key, DxRECLIST_URL);	// without file extension

	SVC_META_FOREACH(mgr, content, {
		HLIB_PATH_Join(content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName, path, DxRECLIST_URL);
		if (strcmp(path, key) == 0)
		{
			return content;
		}
	});
	return NULL;
}
#else
{
	SvcMeta_Contents_t		*content;
	SvcMeta_BriefContents_t	*briefContent;
	HCHAR					*path, *extPtr;
	HCHAR					*key;

	key  = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	path = svc_meta_GetBuf(mgr, DxRECLIST_URL);

	HLIB_PATH_SplitExt(inpath, key, DxRECLIST_URL);	// without file extension

	SVC_META_FOREACH(mgr, content, {
		switch (content->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
			HLIB_PATH_Join(content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName, path, DxRECLIST_URL);
			if (strcmp(path, key) == 0)
			{
				return content;
			}
			break;

		case eSVC_META_CONTENTS_OBJTYPE_Brief:
			briefContent = (SvcMeta_BriefContents_t *)content;
			HxSTD_StrNCpy(path, briefContent->pszHjmPath, DxRECLIST_URL);
			path[DxRECLIST_URL - 1] = '\0';

			extPtr = HxSTD_StrStr(path, EXT_META_FILE);
			if (NULL != extPtr)
			{
				*extPtr = '\0';
			}

			if (strcmp(path, key) == 0)
			{
				return svc_meta_GetContents(mgr, briefContent->pszUrl);
			}

			break;

		default:
			break;
		}
	});

	return NULL;
}
#endif
}

STATIC const HCHAR *	svc_meta_GetFilename (svcMetaMgr_t *mgr, const HCHAR *name, UNIXTIME time, HBOOL last)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	HCHAR				*s;
	HCHAR				*src;
	HCHAR				*dst;
	HUINT32				n;
	HxDATETIME_t		dt;
	SvcMeta_Contents_t	*content;

	if (HLIB_DATETIME_ConvertUnixTimeToDateTime(time, &dt) != ERR_OK)
		return NULL;

	n   = strlen(name);
	src = svc_meta_GetBuf(mgr, n + 1);
	HxSTD_memcpy(src, name, n); src[n] = '\0';

	// remove reserved characters, @see http://en.wikipedia.org/wiki/Filename
	for (s = src ; *s != '\0' ; s++)
	{
		if (strchr("/\\?%*:|\">.", *s))
			*s = '_';
	}

	dst = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	HxSTD_PrintToStrN(dst, DxRECLIST_URL, "%s_%04d%02d%02d_%02d%02d"
			, src
			, dt.stDate.usYear, dt.stDate.ucMonth, dt.stDate.ucDay
			, dt.stTime.ucHour, dt.stTime.ucMinute
		);

	n = 0;
	SVC_META_FOREACH(mgr, content, {
		if (HLIB_STD_StrStartWith(content->stRecordInfo.aucFileName, dst))
			n++;	// dup!
	});

	if (last && n > 0)
		n--;

	if (n > 0)
	{
		HxSTD_PrintToStrN(dst, DxRECLIST_URL, "%s_%04d%02d%02d_%02d%02d(%u)"
			, src
			, dt.stDate.usYear, dt.stDate.ucMonth, dt.stDate.ucDay
			, dt.stTime.ucHour, dt.stTime.ucMinute
			, n
		);
	}
	HxLOG_Debug("%s(%s) -> \'%s'\n", __FUNCTION__, name, dst);
	return dst;
}
#else
{
	HCHAR						*s;
	HCHAR						*src;
	HCHAR						*dst;
	HUINT32						n;
	HxDATETIME_t				dt;
	SvcMeta_Contents_t			*content;
	SvcMeta_BriefContents_t 	*briefContent;

	if (HLIB_DATETIME_ConvertUnixTimeToDateTime(time, &dt) != ERR_OK)
		return NULL;

	n   = strlen(name);
	src = svc_meta_GetBuf(mgr, n + 1);
	HxSTD_memcpy(src, name, n); src[n] = '\0';

	// remove reserved characters, @see http://en.wikipedia.org/wiki/Filename
	for (s = src ; *s != '\0' ; s++)
	{
		if (strchr("/\\?%*:|\">.", *s))
			*s = '_';
	}

	dst = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	HxSTD_PrintToStrN(dst, DxRECLIST_URL, "%s_%04d%02d%02d_%02d%02d"
			, src
			, dt.stDate.usYear, dt.stDate.ucMonth, dt.stDate.ucDay
			, dt.stTime.ucHour, dt.stTime.ucMinute
		);

	n = 0;
	SVC_META_FOREACH(mgr, content, {
		switch (content->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
			if (HLIB_STD_StrStartWith(content->stRecordInfo.aucFileName, dst))
			{
				n++;	// dup!
			}
			break;

		case eSVC_META_CONTENTS_OBJTYPE_Brief:
		{
			briefContent = (SvcMeta_BriefContents_t *)content;
			if (HLIB_STD_StrStartWith(HLIB_PATH_FileName(briefContent->pszHjmPath), dst))
			{
				n++;	// dup!
			}
			break;
		}

		default:
			break;
		}
	});

	if (last && n > 0)
		n--;

	if (n > 0)
	{
		HxSTD_PrintToStrN(dst, DxRECLIST_URL, "%s_%04d%02d%02d_%02d%02d(%u)"
			, src
			, dt.stDate.usYear, dt.stDate.ucMonth, dt.stDate.ucDay
			, dt.stTime.ucHour, dt.stTime.ucMinute
			, n
		);
	}
	HxLOG_Debug("%s(%s) -> \'%s'\n", __FUNCTION__, name, dst);
	return dst;
}
#endif
}

#if defined(CONFIG_MW_CI_PLUS)
STATIC HBOOL 	svc_meta_IsExpiredForCiPlus( UNIXTIME ulCreateTime, DxCopyrightControl_t *pstDrmInfo, UNIXTIME *pulExpiredTime )
{
	HERROR ulRet = 0;
	HULONG ulSysTime = 0;
	UNIXTIME ulExpireDuration = 0, ulExpireDay = 0;
//	HULONG	ulRlCopyCtlInfo = 0;

	if( pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_CiPlus )
	{
		HxLOG_Info("DRM is not CI Plus  !!! \n");
		return FALSE;
	}

	ulExpireDuration = pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL * 60;
/*
	if( ulRlCopyCtlInfo == 0x00 )
	{
		ulExpireDuration = 60 *  90;  // 90 minutes
	}
	else if( ulRlCopyCtlInfo == 0x01 )
	{
		ulExpireDuration = 60 * 60 * 6;  // 6 hours
	}
	else if( ulRlCopyCtlInfo == 0x02 )
	{
		ulExpireDuration = 60 * 60 * 12;  // 12 hours
	}
	else if( (ulRlCopyCtlInfo >= 0x03) && (ulRlCopyCtlInfo <= 0x03F) )
	{
		ulExpireDuration = 60 * 60 * 24 * (ulRlCopyCtlInfo - 0x02);  // 24 hours * (ulRlCopyCtlInfo - 2)
	} else {  // In case  undefined, apply to default value 0
		ulExpireDuration = 60 *  90;  // 90 minutes
	}
*/
	ulRet = VK_CLOCK_GetTime( &ulSysTime );
	if( ulRet != ERR_OK )
	{
		HxLOG_Error("VK_CLOCK_GetTime Error:0x%X\n", ulRet);
		return FALSE;
	}

	ulExpireDay = ulCreateTime + ulExpireDuration;
	*pulExpiredTime = ulExpireDay;
	if( ulSysTime > ulExpireDay )
	{
		return TRUE;
	}
	return FALSE;
}
#endif

#if defined(CONFIG_MW_CAS_CONAX) && defined(CONFIG_MW_CAS_CONAX_URI)
STATIC HERROR svc_meta_IsExpiredForConax(UNIXTIME ulCreateTime, DxCopyrightControl_t* pstDrmInfo, HBOOL* pbExpired, UNIXTIME* pulExpiredTime)
{
	HERROR										ulRet				= 0;
	HULONG										ulSysTime			= 0;
	eDxCopyrightControl_ConaxRetentionLimit_e	eCxRetLimit			= eDxCopyrightControl_ConaxRetentionLimit_NoLimit;
	UNIXTIME									ulExpireDuration	= 0, ulExpiredTime = 0;

	if (pbExpired == NULL)
	{
		return ERR_FAIL;
	}

	if ((pstDrmInfo == NULL) || (pulExpiredTime == NULL))
	{
		*pbExpired = TRUE;
		return ERR_FAIL;
	}

	if (pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Conax)
	{
		HxLOG_Print("[svc_meta_IsExpiredForConax] DRM is not Conax  !!! \n");
		*pbExpired = TRUE;
		return ERR_OK;
	}

	eCxRetLimit = pstDrmInfo->stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit;

	if ((eCxRetLimit == eDxCopyrightControl_ConaxRetentionLimit_CopyFree_NoLimit) 	// ----- EMI == [ Copy Freely ] -----
		|| (eCxRetLimit == eDxCopyrightControl_ConaxRetentionLimit_NoLimit)		// ----- EMI == [ Copy Never ] or [ Copy No More ] -----
		|| (eCxRetLimit == eDxCopyrightControl_ConaxRetentionLimit_NoExpLimit))	// ----- EMI == [ Copy Once ] -----
	{
		HxLOG_Print("[svc_meta_IsExpiredForConax] Unlimited retention ! (%d)  !!! \n", eCxRetLimit);
		*pbExpired = FALSE;
		return ERR_OK;
	}

	switch (eCxRetLimit)
	{
		// ----- EMI == [ Copy Never ] or [ Copy No More ] -----
	case eDxCopyrightControl_ConaxRetentionLimit_1Week:
		ulExpireDuration = SECONDS_PER_WEEK;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_2Days:
		ulExpireDuration = SECONDS_PER_DAY * 2;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_1Day:
		ulExpireDuration = SECONDS_PER_DAY;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_12Hours:
		ulExpireDuration = SECONDS_PER_HOUR * 12;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_6Hours:
		ulExpireDuration = SECONDS_PER_HOUR * 6;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_3Hours:
		ulExpireDuration = SECONDS_PER_HOUR * 3;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_90Min:
		ulExpireDuration = SECONDS_PER_MIN * 90;
		break;

		// ----- EMI == [ Copy Once ] -----
	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_360:
		ulExpireDuration = SECONDS_PER_DAY * 360;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_90:
		ulExpireDuration = SECONDS_PER_DAY * 90;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_30:
		ulExpireDuration = SECONDS_PER_DAY * 30;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_14:
		ulExpireDuration = SECONDS_PER_DAY * 14;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_7:
		ulExpireDuration = SECONDS_PER_DAY * 7;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_2:
		ulExpireDuration = SECONDS_PER_DAY * 2;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_1:
		ulExpireDuration = SECONDS_PER_DAY;
		break;

		// ----- EMI == Exception Case...
	default:
		HxLOG_Print("[svc_meta_IsExpiredForConax]VK_CLOCK_GetTime Error:0x%X\n", ulRet);
		*pbExpired = TRUE;
		return ERR_FAIL;
	}

	ulRet = VK_CLOCK_GetTime(&ulSysTime);
	if (ulRet != ERR_OK)
	{
		HxLOG_Print("[svc_meta_IsExpiredForConax]VK_CLOCK_GetTime Error:0x%X\n", ulRet);
		*pbExpired = TRUE;
		return ERR_FAIL;
	}

	ulExpiredTime = ulCreateTime + ulExpireDuration;
	*pulExpiredTime = ulExpiredTime;
	if (ulSysTime > ulExpiredTime)
	{
		*pbExpired = TRUE;
		return ERR_OK;
	}

	*pbExpired = FALSE;
	return ERR_OK;
}
#endif


STATIC HBOOL svc_meta_IsExpired (SvcMeta_Record_t *pstRecInfo, UNIXTIME *pulExpiredTime )
{
	*pulExpiredTime = 0;

	switch( pstRecInfo->stDrmInfo.stCasCcInfo.eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_CiPlus:
#if defined(CONFIG_MW_CI_PLUS)
			return svc_meta_IsExpiredForCiPlus( pstRecInfo->ulStartTime, &pstRecInfo->stDrmInfo, pulExpiredTime );
#endif

		default:
			HxLOG_Error("unknow DRM Type :0x%X\n", pstRecInfo->stDrmInfo.stCasCcInfo.eCasCcType);
			break;
	}

	return FALSE;
}

STATIC HERROR svc_meta_ConvertToDx (DxRecListData_t *rec, const SvcMeta_Contents_t *content)
{
	const SvcMeta_Record_t	*mwrec = NULL;
	const SvcMeta_Service_t	*mwsvc = NULL;
	const SvcMeta_Event_t	*mwevt = NULL;
	HERROR					hErr = ERR_FAIL;

	if (rec == NULL)
	{
		HxLOG_Error("rec is NULL !\n");
		return ERR_FAIL;
	}

	if (content->bHasRecord)
		mwrec = &content->stRecordInfo;
	if (content->bHasService)
		mwsvc = &content->stServiceInfo;
	if (content->bHasEvent)
		mwevt = &content->stEventInfo;

	hErr = OTL_CONV_MW_PvrMeta2DxRecListData(rec, mwrec, mwsvc, mwevt);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("Cannot convert mw pvr meta to dxmeta\n");
		return ERR_FAIL;
	}
	svc_meta_ValidateField(rec);

	if (!HxSTD_StrEmpty(content->szURL))
		HLIB_STD_StrNCpySafe(rec->szUrl, content->szURL, DxRECLIST_URL);

	if (content->ulContentsId)
		rec->ulContentId = content->ulContentsId;
	rec->ulChipId = content->ulChipId;

	return ERR_OK;
}

STATIC SvcMeta_Contents_t *	svc_meta_ConvertToSvc (SvcMeta_Contents_t *content, const DxRecListData_t *rec)
{
	if (content == NULL)
	{
		content = (SvcMeta_Contents_t *)HLIB_STD_MemCalloc(sizeof(SvcMeta_Contents_t));
		if (content == NULL)
			return NULL;
	}

	if (OTL_CONV_DxRecListData2MW_PvrMeta(&content->stRecordInfo, &content->stServiceInfo, &content->stEventInfo, rec) != ERR_OK)
	{
		HxLOG_Error("Cannot convert dxmeta to mw meta\n");
		return NULL;
	}

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	content->eObjectType = eSVC_META_CONTENTS_OBJTYPE_FullSize;
#endif
	content->ulContentsId = rec->ulContentId;
	HLIB_STD_StrNCpySafe(content->szURL, rec->szUrl, DxRECLIST_URL);
	content->bHasRecord = TRUE;
	if (content->stServiceInfo.nChNumber || !HxSTD_StrEmpty(content->stServiceInfo.aucServiceName))
		content->bHasService= TRUE;
	if (content->stEventInfo.ulStartTime || content->stEventInfo.ulDuration)
		content->bHasEvent  = TRUE;

	content->ulChipId = rec->ulChipId;

	return content;
}

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
STATIC SvcMeta_BriefContents_t *	svc_meta_ConvertToBrief (SvcMeta_BriefContents_t *pstSrcDstBrief, const DxRecListData_t *rec)
{
	HUINT32					 ulUrlLen, ulHjmPathLen;
	HCHAR					*pszUrl = NULL;
	HCHAR					*pszHjmPath = NULL;
	SvcMeta_BriefContents_t	*pstBriefContent = NULL, *pstResult = NULL;


	if (NULL == pstSrcDstBrief)
	{
		pstBriefContent = (SvcMeta_BriefContents_t *)HLIB_STD_MemCalloc(sizeof(SvcMeta_BriefContents_t));
		if (NULL == pstBriefContent)
		{
			goto END_FUNC;
		}

		pstSrcDstBrief = pstBriefContent;
	}

	ulUrlLen = HxSTD_StrLen(rec->szUrl);
	ulHjmPathLen = HxSTD_StrLen(rec->stMeta.szPath) + HxSTD_StrLen(rec->stMeta.szFileName) + 4 + 4;
	pszUrl = (HCHAR *)OxMW_Calloc(ulUrlLen + 1);
	pszHjmPath = (HCHAR *)OxMW_Calloc(ulHjmPathLen + 1);

	if ((NULL == pszUrl) || (NULL == pszHjmPath))
	{
		goto END_FUNC;
	}

	HxSTD_StrNCpy(pszUrl, rec->szUrl, ulUrlLen);
	pszUrl[ulUrlLen] = '\0';
	HLIB_PATH_Join(rec->stMeta.szPath, rec->stMeta.szFileName, pszHjmPath, ulHjmPathLen);
	HLIB_STD_StrAppend(pszHjmPath, EXT_META_FILE, 512);
	pszHjmPath[ulHjmPathLen] = '\0';

	pstSrcDstBrief->eObjectType = eSVC_META_CONTENTS_OBJTYPE_Brief;
	pstSrcDstBrief->ulContentsId = rec->ulContentId;
	pstSrcDstBrief->pszUrl = pszUrl;
	pstSrcDstBrief->pszHjmPath = pszHjmPath;

	pstResult = pstSrcDstBrief;
	pstBriefContent = NULL;
	pszUrl = NULL;
	pszHjmPath = NULL;

END_FUNC:
	if (NULL != pstBriefContent)
	{
		OxMW_Free(pstBriefContent);
	}

	if (NULL != pszUrl)
	{
		OxMW_Free(pszUrl);
	}

	if (NULL != pszHjmPath)
	{
		OxMW_Free(pszHjmPath);
	}

	return pstResult;
}
#endif

STATIC HBOOL		svc_meta_HandleEachFile (HxDIR_t *dir, HCHAR *path, HCHAR *name, HBOOL isDir, void * userdata)
{
	HxPair_t	*pair;
	HCHAR		*fullpath,szFileName[1024];

	pair = (HxPair_t *)userdata;
	HLIB_PATH_SplitExt(name, szFileName, 1024);

	if (pair->right && HxSTD_StrCmp(szFileName, pair->right) != 0)
	{
		return FALSE;
	}

	fullpath = HLIB_PATH_JoinPathes(path, name, NULL);
	if (fullpath)
		HLIB_VECTOR_Add(pair->left, fullpath);
	return FALSE;
}

STATIC HBOOL		svc_meta_CountFile (HxDIR_t *dir, HCHAR *path, HCHAR *name, HBOOL isDir, void * userdata)
{
	HxPair_t	*pair;

	pair = (HxPair_t *)userdata;

	if (HLIB_STD_StrEndWith(name, pair->right))
		pair->left = (void *)(((HINT32)(pair->left)) + 1);
	return FALSE;
}

STATIC HERROR		svc_meta_DoCorrectUrl (svcMetaMgr_t *mgr, DxRecListData_t *rec)
{
	HUINT32	i;

	for (i=0; i<SVC_META_MAX_URL_CORRECT_TIME; i++)
	{
		// URL
		DxRECORDING_MakeURL(rec->szUrl, DxRECLIST_URL
			, rec->szStorageId
			, rec->stMeta.usOnId
			, rec->stMeta.usTsId
			, rec->stMeta.usSvcId
			, rec->svcUId
			, rec->ulRecStartTime == 0 ? HLIB_STD_GetSystemTime()+i : rec->ulRecStartTime+i
		);

		HxLOG_Error("URL crash! TRY url [%s] \n",rec->szUrl);
		if (HLIB_HASH_Lookup(mgr->urlMap, (void *)rec->szUrl) != NULL)
		{
			continue;
		}

		HxLOG_Error("URL crash resolved. use URL [%s]. \n", rec->szUrl);
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HxVector_t *	svc_meta_FindFiles (svcMetaMgr_t *mgr, const HCHAR *path, const HCHAR *filter, HBOOL recursive)
{
	HCHAR 		*dirname;
	HxVector_t	*list;
	HxPair_t	pair;

	dirname = svc_meta_GetBuf(mgr, HxPATH_MAX);
	dirname = HLIB_PATH_DirName(path, dirname, HxPATH_MAX);
	if (dirname == NULL)
		return NULL;

	list = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)HLIB_PATH_MemFree, NULL);
	if (list == NULL)
		return NULL;

	pair.left  = (void *)list;
	pair.right = (void *)filter;

	(void) HLIB_DIR_Find((HUINT8 *)dirname, NULL, recursive ? "R" : "", svc_meta_HandleEachFile, &pair);
	HxVECTOR_DELETE_IF_EMPTY(list);
	return list;
}

STATIC HERROR		svc_meta_UpdateContents (svcMetaMgr_t *mgr, SvcMeta_Contents_t *content)
{
	DxRecListData_t *rec;
	HUINT32          ret;
	HERROR			 hErr = ERR_FAIL;

	rec = svc_meta_GetTempRec(mgr);
	hErr = svc_meta_ConvertToDx(rec, content);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("Cannot convert mw pvr meta to dxmeta\n");
		return ERR_FAIL;
	}

	ret = OTL_HJM_UpdateRecording(rec, TRUE);
	svc_meta_FreeRecordingMember(rec);
	return ret == 0 ? ERR_FAIL : ERR_OK;
}

STATIC HERROR		svc_meta_RemoveContents (svcMetaMgr_t *mgr, SvcMeta_Contents_t *content)
{
	DxRecListData_t *rec;
	HUINT32          ret;
	HERROR			 hErr = ERR_FAIL;

	rec = svc_meta_GetTempRec(mgr);
	hErr = svc_meta_ConvertToDx(rec, content);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("Cannot convert mw pvr meta to dxmeta\n");
		return ERR_FAIL;
	}

	ret = OTL_HJM_RemoveRecording(rec->szUrl, FALSE);
	svc_meta_FreeRecordingMember(rec);

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	mgr->fullContentCache = HLIB_LIST_Remove(mgr->fullContentCache, (const void *)content);
#endif
	HLIB_HASH_Remove(mgr->urlMap, content->szURL);

	return ret == 0 ? ERR_FAIL : ERR_OK;
}

STATIC HERROR		svc_meta_AddContentsInfo(svcMetaMgr_t *mgr, Handle_t svcHandle, SvcMeta_Record_t *mwrec, HUINT32 *contentId)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	SvcMeta_Service_t 	*mwsvc;
	SvcMeta_Contents_t	*content;
	DxRecListData_t		*rec;
	HERROR				hErr = ERR_FAIL;

	SVC_META_FOREACH(mgr, content, {
		if (strcmp(content->stRecordInfo.aucFileName, mwrec->aucFileName) == 0
			&& strcmp(content->stRecordInfo.aucPath, mwrec->aucPath) == 0)
		{
			HxLOG_Error("already exist in meta db : %s\n", mwrec->aucFileName);
			return ERR_FAIL;
		}
	});

	content = (SvcMeta_Contents_t *)HLIB_STD_MemCalloc(sizeof(SvcMeta_Contents_t));
	if (content == NULL)
	{
		HxLOG_Error("Out of memory!\n");
		return ERR_FAIL;
	}
	HxSTD_memcpy(&content->stRecordInfo, mwrec, sizeof(SvcMeta_Record_t));
	mwsvc = svc_meta_MakePvrServiceBySvcHandle(&content->stServiceInfo, svcHandle);
	if (mwsvc == NULL)
	{
		HLIB_STD_MemFree(content);
		HxLOG_Error("Cannot make pvr service by svc handle:0x%X\n", svcHandle);
		return ERR_FAIL;
	}

	SVC_SYS_GetChipId(&(content->ulChipId));
	content->bHasRecord  = TRUE;
	content->bHasService = TRUE;

	rec = svc_meta_GetTempRec(mgr);
	hErr = svc_meta_ConvertToDx(rec, content);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("Cannot convert mw pvr meta to dxmeta\n");
		HLIB_STD_MemFree(content);
		return ERR_FAIL;
	}

	if (HLIB_HASH_Lookup(mgr->urlMap, (void *)rec->szUrl) != NULL)
	{
		HERROR hErr;
		hErr = svc_meta_DoCorrectUrl(mgr, rec);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Can't resolve URL crash. url=[%s] \n",rec->szUrl);
			HLIB_STD_MemFree(content);

			return ERR_FAIL;
		}
	}

	content->ulContentsId = OTL_HJM_AddRecording(rec, TRUE);
	if (content->ulContentsId == 0)
	{
		HxLOG_Error("Cannot add recording to OTL HJM\n");
		HLIB_STD_MemFree(content);
		return ERR_FAIL;
	}
	HLIB_STD_StrNCpySafe(content->szURL, rec->szUrl, DxRECLIST_URL);

	HLIB_HASH_Insert(mgr->urlMap, content->szURL, content);
	if (contentId)
		*contentId = content->ulContentsId;

	HxLOG_Message("Recording added:%u(%s) \'%s\'\n"
			, content->ulContentsId
			, content->szURL
			, content->stRecordInfo.aucFileName
		);
	return ERR_OK;
}
#else
{
	SvcMeta_Service_t 			*mwsvc;
	SvcMeta_Contents_t			*content;
	SvcMeta_BriefContents_t		*briefContent;
	DxRecListData_t				*rec;
	HCHAR						*path;
	HERROR						hErr = ERR_FAIL;

	SVC_META_FOREACH(mgr, content, {
		switch (content->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
			if (strcmp(content->stRecordInfo.aucFileName, mwrec->aucFileName) == 0
				&& strcmp(content->stRecordInfo.aucPath, mwrec->aucPath) == 0)
			{
				HxLOG_Error("already exist in meta db : %s\n", mwrec->aucFileName);
				return ERR_FAIL;
			}

			break;

		case eSVC_META_CONTENTS_OBJTYPE_Brief:
			briefContent = (SvcMeta_BriefContents_t *)content;
			path = svc_meta_GetBuf(mgr, DxRECLIST_URL);

			HLIB_PATH_Join(mwrec->aucPath, mwrec->aucFileName, path, DxRECLIST_URL);
			HLIB_STD_StrAppend(path, EXT_META_FILE, DxRECLIST_URL);

			if (strcmp(path, briefContent->pszHjmPath) == 0)
			{
				HxLOG_Error("already exist in meta db : %s\n", mwrec->aucFileName);
				return ERR_FAIL;
			}

			break;

		default:
			break;

		}
	});

	content = (SvcMeta_Contents_t *)HLIB_STD_MemCalloc(sizeof(SvcMeta_Contents_t));
	if (content == NULL)
	{
		HxLOG_Error("Out of memory!\n");
		return ERR_FAIL;
	}
	HxSTD_memcpy(&content->stRecordInfo, mwrec, sizeof(SvcMeta_Record_t));
	mwsvc = svc_meta_MakePvrServiceBySvcHandle(&content->stServiceInfo, svcHandle);
	if (mwsvc == NULL)
	{
		HLIB_STD_MemFree(content);
		HxLOG_Error("Cannot make pvr service by svc handle:0x%X\n", svcHandle);
		return ERR_FAIL;
	}

	SVC_SYS_GetChipId(&(content->ulChipId));
	content->eObjectType = eSVC_META_CONTENTS_OBJTYPE_FullSize;
	content->bHasRecord  = TRUE;
	content->bHasService = TRUE;

	rec = svc_meta_GetTempRec(mgr);
	hErr = svc_meta_ConvertToDx(rec, content);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("Cannot convert mw pvr meta to dxmeta\n");
		HLIB_STD_MemFree(content);
		return ERR_FAIL;
	}

	if (HLIB_HASH_Lookup(mgr->urlMap, (void *)rec->szUrl) != NULL)
	{
		HERROR hErr;
		hErr = svc_meta_DoCorrectUrl(mgr, rec);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Can't resolve URL crash. url=[%s] \n",rec->szUrl);
			HLIB_STD_MemFree(content);

			return ERR_FAIL;
		}
	}

	content->ulContentsId = OTL_HJM_AddRecording(rec, TRUE);
	if (content->ulContentsId == 0)
	{
		HxLOG_Error("Cannot add recording to OTL HJM\n");
		HLIB_STD_MemFree(content);
		return ERR_FAIL;
	}
	HLIB_STD_StrNCpySafe(content->szURL, rec->szUrl, DxRECLIST_URL);

	HLIB_HASH_Insert(mgr->urlMap, content->szURL, content);
	if (contentId)
		*contentId = content->ulContentsId;

	// Append to the full Content Cache
	(void)svc_meta_InsertFullContentToCache(mgr, content);

	HxLOG_Message("Recording added:%u(%s) \'%s\'\n"
			, content->ulContentsId
			, content->szURL
			, content->stRecordInfo.aucFileName
		);
	return ERR_OK;
}
#endif
}


static HCHAR* svc_meta_getRecycleBinFolder(const HCHAR* pPath)
{
#define	RECYCLEBIN_NAME	".recycleBin"
	HCHAR		MountPath[HxPATH_MAX]={0,};

	if(HLIB_DIR_GetMountPath(pPath,MountPath,HxPATH_MAX) == ERR_FAIL)
	{
		HxLOG_Print("[%s:%d]HLIB_DIR_GetMountPath ERROR![%s]\n",__FUNCTION__,__LINE__,pPath);
		return NULL;
	}

	return  HLIB_PATH_JoinPathes(MountPath, RECYCLEBIN_NAME, NULL);
}

STATIC HERROR svc_meta_SyncRemove(HCHAR* pPath)
{
	HxDIR_t		*pDir;
	HCHAR		*recycleBinPath = NULL, *buf = NULL, *szBuf = NULL;
	HCHAR		*szDir;
	HCHAR 		*szName,*szPath;
	HCHAR 		*szFull,*szFulldup;
	HCHAR 		*szConvertPath;
	HERROR 		hErr;
	HxList_t	*pList = NULL;
	HxList_t	*pListBackup = NULL;
	HCHAR		*szInputFileName = NULL;
	HCHAR		*szFileName = NULL;
	HBOOL		bIsInputDirPath = FALSE;
	HERROR		hResult = ERR_FAIL;

	szFileName = (HCHAR *)HLIB_STD_MemAlloc(HxPATH_MAX);
	buf = (HCHAR *)HLIB_STD_MemAlloc(HxPATH_MAX);
	szBuf = (HCHAR *)HLIB_STD_MemAlloc(HxPATH_MAX*2);
	if (szFileName == NULL || buf == NULL || szBuf == NULL)
	{
		HxLOG_Error("malloc failed. \n");
		goto END_FUNC;
	}

	szDir = HLIB_PATH_DirName(pPath, buf, HxPATH_MAX);
	szInputFileName = HLIB_PATH_FileName(pPath);

	if (HLIB_DIR_IsDirPath(pPath) == TRUE)
	{
		bIsInputDirPath= TRUE;
	}

	HxLOG_Print("[%s:%d]pPath[%s]\n",__FUNCTION__,__LINE__,pPath);

	if( (recycleBinPath= svc_meta_getRecycleBinFolder(pPath))== NULL)
	{
		HxLOG_Error("[%s:%d]svc_meta_getRecycleBinFolder ERROR![%s]\n",__FUNCTION__,__LINE__,pPath);
		goto END_FUNC;
	}

	if(HLIB_FILE_Exist(recycleBinPath) == FALSE)
	{
		HLIB_DIR_Make(recycleBinPath);
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(recycleBinPath, UID_obama, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to HLIB_EXTRA_ChownWithParam (%s)\n", recycleBinPath);
		}

		if (HLIB_DIR_Chmod(recycleBinPath, 0770) != ERR_OK)
		{
			HxLOG_Error ("Fail to HLIB_DIR_Chmod (%s)\n", recycleBinPath);
		}
#endif
	}

	pDir = HLIB_DIR_Open(szDir);
	if (pDir == NULL)
	{
		HxLOG_Error("[%s:%d]HLIB_DIR_Open ERROR![%s]\n",__FUNCTION__,__LINE__,pPath);
		goto END_FUNC;
	}

	do
	{
		szName = HLIB_DIR_GetName(pDir);
		szPath = HLIB_DIR_GetPath(pDir);

		if (HLIB_STD_StrCmpV(HxSTD_StrCmp, szName, ".", "..", NULL) >= 0)
			continue;

		szFull = HLIB_PATH_JoinPathes(szPath, szName, NULL);
		if(szFull)
		{
			if ((HLIB_DIR_IsValid(szFull) == ERR_OK)
				&&(HLIB_STD_StrStartWith(szFull,pPath) == TRUE))
			{
				// if input is dir then remove whole dir (case for xxx.ts folder)
				if (bIsInputDirPath == TRUE)
				{
					pList = HLIB_LIST_Append(pList, (void*)szFull);
				}
				// if input is filename, then remove ones with matching filename
				else if ((HLIB_PATH_SplitExt(szName,szFileName,HxPATH_MAX) != NULL)
							&&(HxSTD_StrCmp(szInputFileName, szFileName) == 0))
				{
					pList = HLIB_LIST_Append(pList, (void*)szFull);
				}
				else
				{
					HLIB_STD_MemFree(szFull);
				}
			}
			else
			{
				HLIB_STD_MemFree(szFull);
			}
		}
	}while(HLIB_DIR_Next(pDir) == ERR_OK);

	HLIB_DIR_Close(pDir);
	pList = HLIB_LIST_First(pList);
	pListBackup = pList;
	while (pList)
	{
		szFull = (HCHAR*)HLIB_LIST_Data(pList);
		if(HLIB_DIR_IsDirPath(szFull) == TRUE)
		{
			svc_meta_SyncRemove(szFull);
			if(HLIB_DIR_IsDirectoryEmpty(szFull)== TRUE)
				HLIB_DIR_Delete(szFull);
		}
		else
		{
			szFulldup = HLIB_STD_StrDup(szFull);
			HxSTD_MemSet(szBuf,0,HxPATH_MAX * 2);
			HLIB_STD_StrReplaceChar(szBuf,szFulldup,'/','_');
			szConvertPath = HLIB_PATH_JoinPathes(recycleBinPath, szBuf, NULL);
			szPath = HLIB_PATH_DirName(szFull, buf, HxPATH_MAX);
			szName = HLIB_PATH_FileName(szFull);
#if defined(CONFIG_MW_CAS_CONAX)
			if(HxSTD_StrStr(szName, ".cxm") != NULL)
			{
				CXAPI_PVR_Remove(szPath, szName);
			}
			else
#endif
			{
				hErr = HLIB_FILE_Move(szFulldup,szConvertPath);
				HxLOG_Warning("Sync Move[%s]->[%s]---[%s]\n",szFulldup,szConvertPath,(hErr== ERR_OK)? "Success..." :"Fail..");
			}
			HLIB_STD_MemFree(szConvertPath);
			HLIB_STD_MemFree(szFulldup);
		}
		HLIB_STD_MemFree(szFull);
		pList = HLIB_LIST_Next(pList);
	}

	hResult = ERR_OK;

END_FUNC:
	if (szFileName != NULL)
	{
		HLIB_STD_MemFree(szFileName);
	}
	if (buf != NULL)
	{
		HLIB_STD_MemFree(buf);
	}
	if (szBuf != NULL)
	{
		HLIB_STD_MemFree(szBuf);
	}
	if (pListBackup != NULL)
	{
		HLIB_LIST_RemoveAll(pListBackup);
	}
	if (recycleBinPath != NULL)
	{
		HLIB_STD_MemFree(recycleBinPath);
	}
	return hResult;
}

STATIC HERROR	svc_meta_RemoveContentsInfo (svcMetaMgr_t *mgr, HUINT32 contentId)
{
	SvcMeta_Contents_t	*content;
	HCHAR				*path;
	//HUINT32				reqId;
	HERROR				err;
	content = svc_meta_GetContentsByID(mgr, contentId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, contentId);
		return 0;
	}
	path = svc_meta_MakePath(mgr, content, NULL);
	err = svc_meta_SyncRemove(path);

	if(err == ERR_FAIL)
		HxLOG_Error("%s(id:%d) svc_meta_SyncRemove() fail!\n", __FUNCTION__, contentId);

	svc_meta_RemoveContents(mgr, content);

	UAPI_FM_Flush();
/*
	HxSTD_StrNCat(path, "*", HxSTD_StrLen("*"));
	if ((reqId = UAPI_FM_Remove(path, NULL, NULL, NULL)) > 0)
	{
		svc_meta_RemoveContents(mgr, content);
		return reqId;
	}

	HxLOG_Error("%s(id:%d) UAPI_FM_Remove() fail!\n", __FUNCTION__, contentId);
*/
	return err;
}

STATIC HERROR	svc_meta_GetEventList (SvcMeta_Contents_t *content, SvcMeta_Event_t **list, SvcMeta_AllocFunc _alloc)
{
	SvcMeta_Event_t *evt;
	SvcMeta_Event_t *ret;

	evt = &content->stEventInfo;
	if(evt == NULL)
	{
		return ERR_FAIL;
	}

	ret = _alloc(sizeof(SvcMeta_Event_t));
	if(ret == NULL)
	{
		return ERR_FAIL;
	}
	*list = ret;
	HxSTD_memcpy(ret, evt, sizeof(SvcMeta_Event_t));
	ret->pNext = NULL;

	while(evt)
	{
		ret->pNext = _alloc(sizeof(SvcMeta_Event_t));
		if(ret->pNext == NULL)
		{
			return ERR_FAIL;
		}

		HxSTD_memcpy(ret->pNext, evt, sizeof(SvcMeta_Event_t));

		evt = evt->pNext;
		ret = ret->pNext;
		ret->pNext = NULL;
	}
	return ERR_OK;
}

STATIC HERROR svc_meta_GetThumbnail (HUINT8 *aucFileName, HUINT32 **pulData )
{
	HERROR	hErr = ERR_FAIL;

	// for the png case check only file exist (due to PAL_THUMB_ReadFile() can't handle png not yet)
	if (0 == HxSTD_StrCmp(".png", EXT_THUMBNAIL_FILE))
	{	// currently the PAL_THUMB_ReadFile() not for the PNG type (PNG thumbnail will decoded by browser & HUMAX native uses ARGB thumbnail)
		HxFILE_t hFile = HLIB_FILE_Open(aucFileName, "r");
		if (hFile)
		{
			hErr = ERR_OK;
			HLIB_FILE_Close(hFile);
		}
		else
		{
			hErr = ERR_FAIL;
			HxLOG_Error("HLIB_FILE_Open() FAIL ! \n");
		}
	}
	else
	{
		// for the ePAL_THUMB_IMAGETYPE_ARGB32
		HUINT32 *pulBuf = NULL;
		HUINT32	ulDataSize = 0;
		HUINT32 ulWidth = 0, ulHeight = 0;

		ulDataSize = (DxPVR_THUMBNAIL_WIDTH * DxPVR_THUMBNAIL_HEIGHT * sizeof(HUINT32) );
		pulBuf = (HUINT32 *) HLIB_STD_MemCalloc( ulDataSize );

		if( pulBuf == NULL )
		{

			HxLOG_Error("HLIB_STD_MemCalloc is NULL (%d)\n", ulDataSize);
			return ERR_FAIL;
		}

		hErr = PAL_THUMB_ReadFile(aucFileName, &ulWidth, &ulHeight, (HUINT8*)pulBuf);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("PAL_THUMB_ReadFile() FAIL ! \n");
		}

		if (pulBuf && ERR_OK == hErr)
		{
			if (pulData)
			{
				*pulData = pulBuf;
			}
		}

		if (pulBuf)
			HLIB_STD_MemFree(pulBuf);

	}
	return hErr;

}

STATIC HINT32	svc_meta_Load (svcMetaMgr_t *mgr, const HCHAR *path)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	HxVector_t	*list;
	HINT32		i, n, c;
	DxRecListData_t		*src;
	SvcMeta_Contents_t	*dst;

	list = OTL_HJM_GetRecordingList(svc_meta_ComparePath, (void *)path);
	if (list == NULL)
	{
		HxLOG_Debug("GetRecordingList by path:\'%s\' fail!\n", path);
		return 0;
	}

	n = HLIB_VECTOR_Length(list);
	for (i = 0, c = 0 ; i < n ; i++)
	{
		src = (DxRecListData_t *)HLIB_VECTOR_ItemAt(list, i);
		dst = svc_meta_ConvertToSvc(NULL, src);
		if (dst)
		{
			// 녹화물 불러오기 전에 녹화가 먼저 되어, 해쉬에 등록되어있다면 해쉬 등록을 제외 한다.
			if(HLIB_HASH_Lookup(mgr->urlMap,dst->szURL))
			{
				HxLOG_Warning(HxANSI_COLOR_GREEN("[%s] Already add table.. [%s]\n" ),__FUNCTION__,dst->szURL);
				HLIB_STD_MemFree(dst);
			}
			else
			{
				HxLOG_Debug("%s(%s) Loading... %u(%s).\'%s\'\n", __FUNCTION__, path, dst->ulContentsId, dst->szURL, dst->stRecordInfo.aucFileName);
				HLIB_HASH_Insert(mgr->urlMap, dst->szURL, dst);
				c++;
			}
		}
	}
	HLIB_VECTOR_Delete(list);
	HxLOG_Message("%s(%s) Loading done: count:%d\n", __FUNCTION__, path, c);
	return c;
}
#else
{
	HxVector_t	*list;
	HINT32		i, n, c;
	DxRecListData_t			*src;
	SvcMeta_BriefContents_t	*dst;

	list = OTL_HJM_GetRecordingList(svc_meta_ComparePath, (void *)path);
	if (list == NULL)
	{
		HxLOG_Debug("GetRecordingList by path:\'%s\' fail!\n", path);
		return 0;
	}

	n = HLIB_VECTOR_Length(list);
	for (i = 0, c = 0 ; i < n ; i++)
	{
		src = (DxRecListData_t *)HLIB_VECTOR_ItemAt(list, i);
		dst = svc_meta_ConvertToBrief(NULL, src);
		if (dst)
		{
			// 녹화물 불러오기 전에 녹화가 먼저 되어, 해쉬에 등록되어있다면 해쉬 등록을 제외 한다.
			if(HLIB_HASH_Lookup(mgr->urlMap,dst->pszUrl))
			{
				HxLOG_Warning(HxANSI_COLOR_GREEN("[%s] Already add table.. [%s]\n" ),__FUNCTION__,dst->pszUrl);
				svc_meta_FreeContents((void *)dst);
			}
			else
			{
				HxLOG_Debug("%s(%s) Loading... %u(%s).\'%s\'\n", __FUNCTION__, path, dst->ulContentsId, dst->pszUrl, dst->pszHjmPath);
				HLIB_HASH_Insert(mgr->urlMap, dst->pszUrl, dst);
				c++;
			}
		}
	}
	HLIB_VECTOR_Delete(list);
	HxLOG_Message("%s(%s) Loading done: count:%d\n", __FUNCTION__, path, c);
	return c;
}
#endif
}


STATIC HERROR	svc_meta_UpdateCopyControl(svcMetaMgr_t *mgr, SvcMeta_Contents_t *content, HBOOL *shallDecrypt, const HCHAR *pucDestFullPath)
{
#ifndef CONFIG_OP_UK_DTT
	return ERR_OK;
#else
	SvcMeta_Record_t	*pstRecInfo = NULL;

	/* If the content ruled by its FTA content management, update its copy count just for 1-copy content */
	pstRecInfo = &(content->stRecordInfo);
	if (pstRecInfo != NULL)
	{
		switch (pstRecInfo->ulFtaCmVal)
		{
			case eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc:
			case eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc_NoEnc:
				*shallDecrypt = FALSE;
				return ERR_OK;

			case eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy:
				if (pstRecInfo->ucCopyTime < DxUKDTT_PVR_AVAILABLE_CONTENT_COPY_NUMBER)
				{
					HxLOG_Error("managed copy, but no right to copy - %s\n", pstRecInfo->aucFileName);

					if (svc_meta_IsPvrPath(pucDestFullPath))
					{
						/* Copy될 Target Path가 Internal Storage일 경우이다. */
						*shallDecrypt = FALSE;
						return ERR_OK;
					}
					else
					{
						*shallDecrypt = FALSE;
						return ERR_FAIL;
					}
				}
				else
				{
					HxLOG_Print("managed copy, allow to copy this time - %s\n", pstRecInfo->aucFileName);
					pstRecInfo->ucCopyTime--;
					svc_meta_UpdateContents(mgr, content);
					svc_meta_EmitEvent(mgr, eSEVT_META_UPDATED, 0, (HINT32)content->ulContentsId, 1, 0);
					*shallDecrypt = FALSE;
					return ERR_OK;
				}
				break;

			case eDxUkdtt_CONTENT_MGMT_STATE_Reserved:
				HxLOG_Error("prohibit to copy - %s\n", pstRecInfo->aucFileName);
				return ERR_FAIL;

			case eDxUkdtt_CONTENT_MGMT_STATE_None:
				if (pstRecInfo->u1cEncryption == 1)
					*shallDecrypt = TRUE;
				else
					*shallDecrypt = FALSE;
				return ERR_OK;
		}
	}

	return ERR_OK;
#endif
}

STATIC HERROR	svc_meta_CopyContents (svcMetaMgr_t *mgr, HUINT32 id, const HCHAR *path, HxReturnFunc_t onDone, void *userdata)
{
	SvcMeta_Contents_t	*content;
	HCHAR				*src;
	HINT32				reqId;
	svcMetaFOP_t		*fop;
	HxVector_t			*dups;
	HBOOL				bDecryptCopy	= FALSE;
	HBOOL				bRecordingCopy	= FALSE;
	HBOOL				bChunkForMedia	= FALSE;
	HUINT32				ulUapiFlag		= 0x00000000;
	DxRecStatus_e		eRecStatus;

	content = svc_meta_GetContentsByID(mgr, id);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%u) Cannot find contents!\n", __FUNCTION__, id);
		onDone(ERR_FAIL,userdata);
		return ERR_FAIL;
	}

	eRecStatus = content->stRecordInfo.ucRecordStatus;
	if(eRecStatus == eDxREC_STS_RECORD_START || eRecStatus == eDxREC_STS_RECORDING)
	{
		HxLOG_Error("%s(id:%u) Cannot Copy contents!\n", __FUNCTION__, id);
		onDone(UMERR_REASON_FAIL_TO_READ,userdata);
		return ERR_FAIL;
	}

	if ((dups = svc_meta_FindFiles(mgr, path, content->stRecordInfo.aucFileName, FALSE)) != NULL)
	{
		SvcMeta_Contents_t	*forDeleteContent = NULL;
		HCHAR				*cmpPath = NULL;
		HUINT32 			ulContentID = 0;

		HxVECTOR_EACH(dups, HCHAR *, cmpPath, {
			forDeleteContent = svc_meta_GetContentsByPath(mgr, cmpPath);
			if(forDeleteContent)
			{
				ulContentID = forDeleteContent->ulContentsId;
				if (svc_meta_RemoveContentsInfo(mgr, ulContentID) == ERR_OK)
				{
					HxLOG_Debug("[%s:%d] Notify Removed Event ...\n", __FUNCTION__, __LINE__);
					svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, (HINT32)ulContentID, 1, 0);
					break;
				}
			}
		});
		HLIB_VECTOR_Delete(dups);
	}

	src = svc_meta_MakePath(mgr, content, NULL);
	bRecordingCopy = svc_meta_IsPlaybackPath(path);
	if (bRecordingCopy)
	{
		// recording (ts+meta) copy
		HxSTD_StrNCat(src, ".*", HxSTD_StrLen(".*"));
		bDecryptCopy = FALSE;
	}
	else
	{
		HCHAR *pszCheckPath = HLIB_STD_MemCalloc(HxSTD_StrLen(src) + HxSTD_StrLen(".ts") + 1);
		// video (ts) copy
		// src: only copy ts (no meta)
		if(pszCheckPath)
		{
			HxSTD_MemCopy(pszCheckPath, src, HxSTD_StrLen(src));
			HxSTD_StrNCat(pszCheckPath, ".ts", HxSTD_StrLen(".ts"));
			if(HLIB_DIR_IsDirPath(pszCheckPath) == TRUE)
			{
				HxLOG_Debug("Copy Delayed Recording File To Non Pvr Path.\n");
				HxSTD_StrNCat(src, ".*", HxSTD_StrLen(".*"));
				// For Delayed Recording To Non PVR Path
				bChunkForMedia = TRUE;
			}
			else
			{
				HxSTD_StrNCat(src, ".ts", HxSTD_StrLen(".ts"));
			}
			HLIB_STD_MemFree(pszCheckPath);
		}

		if (content->stRecordInfo.u1cEncryption == TRUE)
		{
			bDecryptCopy = TRUE;
		}
		else
		{
			bDecryptCopy = FALSE;
		}
	}

	if (svc_meta_UpdateCopyControl(mgr, content, &bDecryptCopy, path) != ERR_OK)
	{
		HxLOG_Error("cannot proceed to copy for copy-protection.\n");
		onDone(ERR_FAIL,userdata);
		return ERR_FAIL;
	}

	if (bRecordingCopy == FALSE
		&& bDecryptCopy == FALSE
		&& (content->stRecordInfo.u1cEncryption== TRUE))
	{
		HxLOG_Warning("copied video probably are not playable because it's encrypted. \n");
	}

	fop = (svcMetaFOP_t *)HLIB_STD_MemCalloc(sizeof(svcMetaFOP_t));
	if (fop == NULL)
	{
		HxLOG_Error("%s(id:%u) Out of memory\n", __FUNCTION__, id);
		onDone(ERR_FAIL,userdata);
		return ERR_FAIL;
	}

	ulUapiFlag |= bRecordingCopy	? UAPI_COPY_FLAG_RECORDINGS		: 0;
	ulUapiFlag |= bDecryptCopy		? UAPI_COPY_FLAG_DECRYPT		: 0;
	ulUapiFlag |= bChunkForMedia	? UAPI_COPY_FLAG_CHUNKFORMEDIA	: 0;

	HxLOG_Debug("src[%s],dest[%s],uapi flag[0x%x]\n", src, path, ulUapiFlag);

	if ((reqId = UAPI_FM_Copy(src, path, NULL, ulUapiFlag, (void*)id, svc_meta_OnCopyDone)) >= 0)
	{
		fop->cid      = id;
		fop->reqId    = reqId;
		HLIB_PATH_Join(path, content->stRecordInfo.aucFileName, fop->path, DxRECLIST_URL);
		HLIB_STD_StrAppend(fop->path, EXT_META_FILE, DxRECLIST_URL);
		fop->onDone   = onDone;
		fop->userdata = userdata;
		HLIB_HASH_Insert(mgr->fopMap, (void *)id, (void *)fop);

		return ERR_OK;
	}

	HLIB_STD_MemFree(fop);
	HxLOG_Error("%s(id:%u) UAPI_FM_Copy(%s.*, %s) fail!!\n", __FUNCTION__, id, src, path);
	onDone(ERR_FAIL,userdata);
	return ERR_FAIL;
}

STATIC HERROR	svc_meta_MoveContents (svcMetaMgr_t *mgr, HUINT32 id, const HCHAR *path, HxReturnFunc_t onDone, void *userdata)
{
	SvcMeta_Contents_t	*content;
	HCHAR				*src;
	HINT32				reqId;
	svcMetaFOP_t		*fop;
	HxVector_t			*dups;
	HBOOL				bDecryptCopy	= FALSE;
	HBOOL				bRecordingCopy	= FALSE;
	HBOOL				bChunkForMedia	= FALSE;
	HUINT32				ulUapiFlag		= 0x00000000;
	DxRecStatus_e		eRecStatus;

	content = svc_meta_GetContentsByID(mgr, id);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%u) Cannot find contents!\n", __FUNCTION__, id);
		onDone(ERR_FAIL,userdata);
		return ERR_FAIL;
	}

	eRecStatus = content->stRecordInfo.ucRecordStatus;
	if(eRecStatus == eDxREC_STS_RECORD_START || eRecStatus == eDxREC_STS_RECORDING)
	{
		HxLOG_Error("%s(id:%u) Cannot Move contents!\n", __FUNCTION__, id);
		onDone(UMERR_REASON_FAIL_TO_READ,userdata);
		return ERR_FAIL;
	}

	if ((dups = svc_meta_FindFiles(mgr, path, content->stRecordInfo.aucFileName, FALSE)) != NULL)
	{
		SvcMeta_Contents_t	*forDeleteContent = NULL;
	   	HCHAR				*cmpPath = NULL;
		HUINT32 			ulContentID = 0;

		HxVECTOR_EACH(dups, HCHAR *, cmpPath, {
			forDeleteContent = svc_meta_GetContentsByPath(mgr, cmpPath);
			if(forDeleteContent)
			{
				ulContentID = forDeleteContent->ulContentsId;
				if (svc_meta_RemoveContentsInfo(mgr, ulContentID) == ERR_OK)
				{
					HxLOG_Debug("[%s:%d] Notify Removed Event ...\n", __FUNCTION__, __LINE__);
					svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, (HINT32)ulContentID, 1, 0);
					break;
				}
			}
		});
		HLIB_VECTOR_Delete(dups);
	}

	src = svc_meta_MakePath(mgr, content, NULL);
	bRecordingCopy = svc_meta_IsPlaybackPath(path);
	if (bRecordingCopy)
	{
		// recording (ts+meta) copy
		HxSTD_StrNCat(src, ".*", HxSTD_StrLen(".*"));
		bDecryptCopy = FALSE;
	}
	else
	{
		HCHAR *pszCheckPath = HLIB_STD_MemCalloc(HxSTD_StrLen(src) + HxSTD_StrLen(".ts") + 1);
		// video (ts) copy
		// src: only copy ts (no meta)
		if(pszCheckPath)
		{
			HxSTD_MemCopy(pszCheckPath, src, HxSTD_StrLen(src));
			HxSTD_StrNCat(pszCheckPath, ".ts", HxSTD_StrLen(".ts"));
			if(HLIB_DIR_IsDirPath(pszCheckPath) == TRUE)
			{
				HxLOG_Debug("Move Delayed Recording File To Non Pvr Path.\n");
				HxSTD_StrNCat(src, ".*", HxSTD_StrLen(".*"));
				// For Delayed Recording To Non PVR Path
				bChunkForMedia = TRUE;
			}
			else
			{
				HxSTD_StrNCat(src, ".ts", HxSTD_StrLen(".ts"));
			}
			HLIB_STD_MemFree(pszCheckPath);
		}

		if (content->stRecordInfo.u1cEncryption == TRUE)
		{
			bDecryptCopy = TRUE;
		}
		else
		{
			bDecryptCopy = FALSE;
		}
	}

	if (svc_meta_UpdateCopyControl(mgr, content, &bDecryptCopy, path) != ERR_OK)
	{
		HxLOG_Error("cannot proceed to copy for copy-protection.\n");
		onDone(ERR_FAIL,userdata);
		return ERR_FAIL;
	}

	if (bRecordingCopy == FALSE
		&& bDecryptCopy == FALSE
		&& (content->stRecordInfo.u1cEncryption== TRUE))
	{
		HxLOG_Warning("copied video probably are not playable because it's encrypted. \n");
	}

	fop = (svcMetaFOP_t *)HLIB_STD_MemCalloc(sizeof(svcMetaFOP_t));
	if (fop == NULL)
	{
		HxLOG_Error("%s(id:%u) Out of memory\n", __FUNCTION__, id);
		onDone(ERR_FAIL,userdata);
		return ERR_FAIL;
	}

	ulUapiFlag |= bRecordingCopy	? UAPI_COPY_FLAG_RECORDINGS		: 0;
	ulUapiFlag |= bDecryptCopy		? UAPI_COPY_FLAG_DECRYPT		: 0;
	ulUapiFlag |= bChunkForMedia	? UAPI_COPY_FLAG_CHUNKFORMEDIA	: 0;
	HxLOG_Debug("src[%s],dest[%s],uapi flag[0x%x]\n",src,path,ulUapiFlag);

	//녹화물은 hjm .ts /chunk. nts 등의 여러 파일이 같이동작한다.
	//umma-move 동작은 이 파일 하나씩 전송 완료되면 삭제가 된다.
	//이때 cancel 에 대한 동작이 불가하기에, copy 이후 완료 이벤트에 remove 명령을 하도록 변경함
	if ((reqId = UAPI_FM_Copy(src, path, NULL, ulUapiFlag, (void*)id, svc_meta_OnMoveDone)) > 0)
	{
		fop->cid      = id;
		fop->reqId    = reqId;

		HLIB_PATH_Join(path, content->stRecordInfo.aucFileName, fop->path, DxRECLIST_URL);
		HLIB_STD_StrAppend(fop->path, EXT_META_FILE, DxRECLIST_URL - 1);
		fop->onDone   = onDone;
		fop->userdata = userdata;
		HLIB_HASH_Insert(mgr->fopMap, (void *)id, (void *)fop);

		return ERR_OK;
	}

	HLIB_STD_MemFree(fop);
	HxLOG_Error("%s(id:%u) UAPI_FM_Copy(%s.*, %s) fail!!\n", __FUNCTION__, id, src, path);
	onDone(ERR_FAIL,userdata);
	return ERR_FAIL;
}

STATIC HERROR	svc_meta_RenameContents (svcMetaMgr_t *mgr, HUINT32 id, const HCHAR *path, const HCHAR *file)
{
	SvcMeta_Contents_t	*content;
	HxVector_t			*list;
	HCHAR				*src;
	HCHAR				*dst;
	HCHAR				*ext;

	content = svc_meta_GetContentsByID(mgr, id);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%u) Cannot find contents!\n", __FUNCTION__, id);
		return ERR_FAIL;
	}

	list = svc_meta_FindFiles(mgr, content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName, FALSE);
	if (list == NULL)
	{
		HxLOG_Error("%s(id:%u) There is no files (%s)!\n", __FUNCTION__, id, content->stRecordInfo.aucFileName);
		return ERR_FAIL;
	}

	dst = svc_meta_GetBuf(mgr, DxRECLIST_URL);
	HxVECTOR_EACH(list, HCHAR *, src, {
		HLIB_PATH_Join(path, file, dst, DxRECLIST_URL);
		ext = strrchr(src, '.');
		if (ext)
			HLIB_STD_StrAppend(dst, ext, DxRECLIST_URL);
		HLIB_FILE_Move(src, dst);


		HxLOG_Debug("%s() mv \"%s\" \"%s\"\n", __FUNCTION__, src, dst);
	});
	HLIB_VECTOR_Delete(list);
	return ERR_OK;
}

STATIC svcMetaFOP_t * svc_meta_CancelFileOp (svcMetaMgr_t *mgr, HUINT32 cid)
{
	svcMetaFOP_t	*fop;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	fop = (svcMetaFOP_t *)HLIB_HASH_Lookup(mgr->fopMap, (void *)cid);
	if (fop == NULL)
	{
		HxLOG_Error("%s(cid:%u) There is no file operation!\n", __FUNCTION__, cid);
		return NULL;
	}

	UAPI_FM_Cancel(fop->reqId, NULL, NULL);
	return fop;
}

STATIC svcMetaMgr_t *	svc_meta_GetManager (void)
{
	STATIC svcMetaMgr_t	*s_mgr;

	if (s_mgr == NULL)
	{
		s_mgr = (svcMetaMgr_t *)HLIB_STD_MemCalloc(sizeof(svcMetaMgr_t));
		HxLOG_Assert(s_mgr);
		s_mgr->textBuf = HLIB_CBUF_New(1024 * 10);
		HxLOG_Assert(s_mgr->textBuf);
		s_mgr->urlMap = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual
						, NULL, (_HxDestroyNotify)svc_meta_FreeContents);
		HxLOG_Assert(s_mgr->urlMap);
		s_mgr->trashUrl = HLIB_TREE_NewFull(NULL, NULL, NULL, (HxDestroyNotify)HLIB_STD_MemFree_CB);
		HxLOG_Assert(s_mgr->trashUrl);
		s_mgr->fopMap = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual
						, NULL, (_HxDestroyNotify)HLIB_STD_MemFree_CB);
		HxLOG_Assert(s_mgr->fopMap);

		OTL_HJM_SetPostProcessor(svc_meta_HjmPostProcessor);

		HxSEMT_Create(&s_svcPvrMetaSem, "SvcMetaSem", HxSEMT_FIFO);
	}
	SVC_META_ENTER;
	return s_mgr;
}

STATIC void		svc_meta_ReleaseManager (svcMetaMgr_t *mgr)
{
	(void)mgr;
	SVC_META_LEAVE;
}


STATIC void		svc_meta_EmitEvent(svcMetaMgr_t *mgr, HINT32 evt, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (!mgr->muteEvent)
		svc_meta_PostMsgToMgr(evt, hAct, p1, p2, p3);
}

STATIC HERROR	svc_meta_DoCorrection (DxRecListData_t *rec)
{
#ifndef CONFIG_OP_UK_DTT
	return ERR_FAIL;
#else

	switch (rec->extension.ukdtt.ulFtaCmVal)
	{
		case eDxUkdtt_CONTENT_MGMT_STATE_None:
			if (rec->bEncrypted == 1)
			{
				HxLOG_Debug("decrypted copied encrypted content. set encryption false \n");
				rec->bEncrypted = 0;
				return ERR_OK;
			}
			break;
		default:
			break;
	}
	return ERR_FAIL;
#endif
}

//STATIC void		svc_meta_OnCopyDone (const UAPI_NotiInfo_t *info, void *userdata)
STATIC HERROR	svc_meta_OnCopyDone (void *userdata, HINT32 nRequestId, UAPI_STATUS_t *pstInfo)
{
	svcMetaMgr_t *mgr;
	svcMetaFOP_t	*fop;
	DxRecListData_t	*rec;
	SvcMeta_Contents_t	*content;
	HBOOL			bIsUpdateNeeded = FALSE;
	HERROR			hErr;

	HxLOG_Trace();

	if (pstInfo->nDoneOperationNum != pstInfo->nTotalOperationNum)
	{
		HxLOG_Debug("in copy operation ....\n");
		return ERR_OK;
	}

	rec = NULL;
	mgr = svc_meta_GetManager();

	fop = (svcMetaFOP_t *)HLIB_HASH_Lookup(mgr->fopMap, userdata);
	if (fop == NULL)
	{
		HxLOG_Error("%s(cid:%d) Cannot find copy operation (maybe canceled)\n", __FUNCTION__, (int)userdata);
		svc_meta_ReleaseManager(mgr);
		return ERR_OK;
	}

	if(fop->reqId != nRequestId)
	{
		HxLOG_Error("%s(nRequestId:%d) not my operation \n", __FUNCTION__, nRequestId);
		svc_meta_ReleaseManager(mgr);
		return ERR_OK;
	}
	fop = HLIB_HASH_Detach(mgr->fopMap, userdata);

	if (pstInfo->ulErr)
	{
		svc_meta_ReleaseManager(mgr);

		HxLOG_Warning("%s() copy failed! Err[0x%x]\n", __FUNCTION__,pstInfo->ulErr);
		if (fop->onDone)
			fop->onDone(pstInfo->ulErr, fop->userdata);
		HLIB_STD_MemFree(fop);
		return ERR_OK;
	}

	if (svc_meta_IsPlaybackPath(fop->path))
	{
		HxLOG_Debug("%s(path:%s) pvr path! copied item will be added to recording list\n", __FUNCTION__, fop->path);
		rec = OTL_HJM_Load(fop->path);
		if (rec)
		{
			hErr = svc_meta_DoCorrection(rec);
			if (hErr == ERR_OK)
			{
				HxLOG_Debug("rec updated. \n");
				bIsUpdateNeeded = TRUE;
			}

			rec->ulContentId = OTL_HJM_AddRecording(rec, bIsUpdateNeeded);
			content = svc_meta_ConvertToSvc(NULL, rec);

			if (content)
			{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
				HLIB_HASH_Insert(mgr->urlMap, (void *)content->szURL, (void *)content);
#else
				HLIB_HASH_Replace(mgr->urlMap, (void *)content->szURL, (void *)content);
				svc_meta_InsertFullContentToCache(mgr, content);
#endif
				svc_meta_EmitEvent(mgr, eSEVT_META_ADDED, 0, (HINT32)content->ulContentsId, (HINT32)HLIB_HASH_Size(mgr->urlMap), 1);
			}
		}
		else
		{
			HxLOG_Debug("%s() Failed to load hjm(%s)\n", __FUNCTION__, fop->path);
		}
	}
	svc_meta_ReleaseManager(mgr);
	if (fop->onDone)
	{
		if (rec)
			fop->onDone(ERR_OK, fop->userdata, rec->ulContentId);
		else
			fop->onDone(ERR_OK, fop->userdata);
	}
	HLIB_STD_MemFree(fop);
	if (rec)
		DxRECLISTDATA_Delete(rec);

	return ERR_OK;
}

//STATIC void		svc_meta_OnMoveDone (const UAPI_NotiInfo_t *info, void *userdata)
STATIC HERROR	svc_meta_OnMoveDone (void *userdata, HINT32 nRequestId, UAPI_STATUS_t *pstInfo)
{
	svcMetaMgr_t *mgr;
	svcMetaFOP_t	*fop;
	DxRecListData_t	*rec;
	SvcMeta_Contents_t	*content;
	HBOOL			bIsUpdateNeeded = FALSE;
	HERROR			hErr;

	HxLOG_Trace();
	if (pstInfo->nDoneOperationNum != pstInfo->nTotalOperationNum)
	{
		HxLOG_Debug("in move operation ....\n");
		return ERR_OK;
	}

	rec = NULL;
	mgr = svc_meta_GetManager();

	fop = (svcMetaFOP_t *)HLIB_HASH_Lookup(mgr->fopMap, userdata);
	if (fop == NULL)
	{
		HxLOG_Error("%s(cid:%d) Cannot find move operation (maybe canceled)\n", __FUNCTION__, (int)userdata);
		svc_meta_ReleaseManager(mgr);
		return ERR_OK;
	}

	if(fop->reqId != nRequestId)
	{
		HxLOG_Error("%s(nRequestId:%d) not my operation \n", __FUNCTION__, nRequestId);
		svc_meta_ReleaseManager(mgr);
		return ERR_OK;
	}
	fop = HLIB_HASH_Detach(mgr->fopMap, userdata);

	if (pstInfo->ulErr)
	{
		svc_meta_ReleaseManager(mgr);

		HxLOG_Warning("%s() move failed!  reason[0x%x]\n", __FUNCTION__,pstInfo->ulErr);
		if (fop->onDone)
			fop->onDone(pstInfo->ulErr, fop->userdata);
		HLIB_STD_MemFree(fop);
		return ERR_OK;
	}

	if (svc_meta_IsPlaybackPath(fop->path))
	{
		HxLOG_Debug("%s(path:%s) pvr path! moved item will be added to recording list\n", __FUNCTION__, fop->path);
		rec = OTL_HJM_Load(fop->path);
		if (rec)
		{
			hErr = svc_meta_DoCorrection(rec);
			if (hErr == ERR_OK)
			{
				HxLOG_Debug("rec updated. \n");
				bIsUpdateNeeded = TRUE;
			}

			rec->ulContentId = OTL_HJM_AddRecording(rec, bIsUpdateNeeded);
			content = svc_meta_ConvertToSvc(NULL, rec);

			if (content)
			{
				HxLOG_Debug("content[%d], fop-cid[%d]\n",content->ulContentsId,fop->cid);
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
				HLIB_HASH_Insert(mgr->urlMap, (void *)content->szURL, (void *)content);
#else
				HLIB_HASH_Replace(mgr->urlMap, (void *)content->szURL, (void *)content);
				svc_meta_InsertFullContentToCache(mgr, content);
#endif
				svc_meta_EmitEvent(mgr, eSEVT_META_ADDED, 0, (HINT32)content->ulContentsId, (HINT32)HLIB_HASH_Size(mgr->urlMap), 1);
			}
		}
		else
		{
			HxLOG_Debug("%s() Failed to load hjm(%s)\n", __FUNCTION__, fop->path);
		}
	}
	svc_meta_RemoveContentsInfo(mgr,fop->cid);
	svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, (HINT32)fop->cid, 1, 0);
	svc_meta_ReleaseManager(mgr);



	if (fop->onDone)
	{
		if (rec)
			fop->onDone(ERR_OK, fop->userdata, rec->ulContentId);
		else
			fop->onDone(ERR_OK, fop->userdata);
	}

	HLIB_STD_MemFree(fop);
	if (rec)
		DxRECLISTDATA_Delete(rec);

	return ERR_OK;
}


#define __V2__
STATIC HxJSON_t	svc_meta_ReadTimestamp(const HCHAR *szPath)
{
	HxJSON_t	hJson;

	hJson = HLIB_JSON_DecodeFile(szPath);
#ifdef	DxREC_ENCRYPT_HJM
	if (hJson)
	{
		const HCHAR	*szEnc;
		HCHAR		*szDec;

		szEnc = HLIB_JSON_Object_GetString(hJson, "hjtsm");
		if (szEnc)
		{
			szDec = DxRECORDING_Decrypt(szEnc, DxRECLISTDATA_GetCryptKey());
			HLIB_JSON_Delete(hJson);

			if (szDec == NULL)
			{
				HxLOG_Error("decrypt fail!!\n");
				return NULL;
			}
			hJson = HLIB_JSON_Decode(szDec);
			DLIB_MemFree(szDec);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("%s(%s) %s\n"), __FUNCTION__, szPath, hJson ? "Success" : "Failed");
		}
	}
#endif
	return hJson;
}

STATIC HERROR	svc_meta_WriteTimestamp(const HCHAR *szPath, const HCHAR *szJson)
{
	HUINT32	ulWritten;
#ifdef	DxREC_ENCRYPT_HJM
	HINT32	nJsonWriter = 0;
#endif
	HxFILE_t		hFile;

	if (szJson == NULL)
	{
		return ERR_FAIL;
	}

#ifdef	DxREC_ENCRYPT_HJM
	{
		HCHAR *szEnc;

		szEnc = DxRECORDING_Encrypt(szJson, DxRECLISTDATA_GetCryptKey());
		if (szEnc == NULL)
		{
			HxLOG_Error("encrypt fail!!\n");
			return ERR_FAIL;
		}
		nJsonWriter = HLIB_JSONWRITER_Open();
		if (!nJsonWriter)
		{
			DxRECORDING_FreeData(szEnc);
			HxLOG_Error("out of memory!!\n");
			return ERR_FAIL;
		}
		HLIB_JSONWRITER_Batch(nJsonWriter,"{ss}", "hjtsm", szEnc);
		DxRECORDING_FreeData(szEnc);

		szJson = HLIB_JSONWRITER_GetJSON(nJsonWriter);
		if (szJson == NULL)
		{
			HxLOG_Error("json encoding error:%s\n", HLIB_JSONWRITER_Error(nJsonWriter));
			HLIB_JSONWRITER_Close(nJsonWriter);
			return ERR_FAIL;
		}
	}
#endif
	// write the "TimeStampList"
	hFile = HLIB_FILE_Open((const HUINT8 *)szPath, "wb");
	if (NULL == hFile)
	{
		HxLOG_Error("Open Error :  %s\n", szPath);
		//HLIB_FILE_Close(hFile); // kill되는 문제로 인해 삭제.
		return ERR_FAIL;
	}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	if (HLIB_EXTRA_ChownWithParam(szPath, UID_obama, GID_pvr) != ERR_OK)
	{
		HxLOG_Error("Fail to HLIB_EXTRA_ChownWithParam (%s)\n", szPath);
	}

	if (HLIB_EXTRA_Chmod(szPath, 0660) != ERR_OK)
	{
		HxLOG_Error("Fail to HLIB_EXTRA_Chmod (%s)\n", szPath);
	}
#endif

	ulWritten = HLIB_FILE_Write(hFile, (void *)szJson, 1, HxSTD_StrLen(szJson));

#ifdef	DxREC_ENCRYPT_HJM
	if (nJsonWriter)
		HLIB_JSONWRITER_Close(nJsonWriter);
#endif
	HLIB_FILE_Close(hFile);

	NOT_USED_PARAM(ulWritten);
	return ERR_OK;
}

STATIC const HCHAR* svc_meta_EncryptAsBase64 (HxJSONBIN_t	encoder, HUINT8 *pucData, HUINT32 ulDataSize)
{
	HUINT8	*pszEncryptBuf;

	// encrypt
	pszEncryptBuf		= HLIB_STD_MemCalloc(ulDataSize);
	HLIB_CRYPT_DES_EncryptData(s_aucDes_Key, pucData, pszEncryptBuf, ulDataSize);

	// encode as base 64
	HLIB_JSONBIN_ENCODER_Clear(encoder);
	if (FALSE == HLIB_JSONBIN_ENCODER_SetBinary(encoder, pszEncryptBuf, ulDataSize))
	{	// fail
		HxLOG_Error("HLIB_JSONBIN_ENCODER_SetBinary() Fail! \n");
		goto ERROR;
	}

	if (pszEncryptBuf)
		HLIB_STD_MemFree(pszEncryptBuf);
	return HLIB_JSONBIN_ENCODER_Encode(encoder);

ERROR:
	if (pszEncryptBuf)
		HLIB_STD_MemFree(pszEncryptBuf);
	return (HCHAR *)NULL;
}

STATIC HERROR svc_meta_DecryptFromBase64Json (HxJSON_t json, HUINT8 *pucData, HUINT32 ulDataSize)
{
	// decode base 64 json
	HERROR		hErr 			= ERR_FAIL;
	HUINT8		*pszBase64DecodeBuf;
	HxJSONBIN_t	decoder			= HLIB_JSONBIN_DECODER_Open();

	pszBase64DecodeBuf = HLIB_JSONBIN_DECODER_Decode(decoder, NULL, ulDataSize, json, NULL, NULL, NULL);
	HLIB_JSONBIN_DECODER_Close(decoder);

	if (NULL == pszBase64DecodeBuf)
	{
		HxLOG_Error("HLIB_JSONBIN_DECODER_Decode() Fail! \n");
		goto ERROR;
	}

	HLIB_CRYPT_DES_DecryptData(s_aucDes_Key, pszBase64DecodeBuf, pucData, ulDataSize);
	hErr = ERR_OK;

ERROR:
	if (pszBase64DecodeBuf)
		HLIB_STD_MemFree(pszBase64DecodeBuf);
	return hErr;
}

STATIC HERROR svc_meta_ConvertEventToFileEvent( SvcMeta_Event_t *pstEvent,  svcMetaEvent_t *pstFileEvent,  svcMetaExtEvent_t **pstFileExtEvt )
{
	HUINT32 ulExtEvtNum = 0, i, j;
	SvcMeta_ExtEvt_t *pstExtEvt = NULL;
	svcMetaExtEvent_t *astFileExtEvt = NULL;

	if( (pstEvent == NULL) || (pstFileEvent == NULL ) )
		return ERR_FAIL;

	pstFileEvent->ulStartTime = pstEvent->ulStartTime;
	pstFileEvent->ulDuration = pstEvent->ulDuration;
	pstFileEvent->u3RunnigStatus = pstEvent->u3RunnigStatus;
	pstFileEvent->u1FreeCaMode = pstEvent->u1FreeCaMode;
	pstFileEvent->nEvtId = pstEvent->nEventId;

	pstFileEvent->ucParentalRatingNum = pstEvent->ucParentalRatingNum;
	HxSTD_MemCopy( pstFileEvent->astParentalRating, pstEvent->astParentalRating, PVR_PARENTAL_RATING_MAX * sizeof(SvcMeta_Rating_t) );
	pstFileEvent->ucContentNibble = pstEvent->ucContentNibble;
	pstFileEvent->ucUserNibble = pstEvent->ucUserNibble;
	HxSTD_MemCopy( pstFileEvent->aucEventName, pstEvent->aucEventName, PVR_SHORT_EVENT_NAME_LEN );
	HxSTD_MemCopy( pstFileEvent->aucEventText, pstEvent->aucEventText, PVR_EVENT_TEXT );

	HxSTD_MemCopy( pstFileEvent->astComponent, pstEvent->stComponent, sizeof(SvcMeta_Component_t) * EPG_COMPONENT_DESC_MAX);
	//HxSTD_MemCopy( pstFileEvent->aucReserved, pstEvent->aucReserved, 32 );

#if defined(CONFIG_OP_FREESAT)
	if (pstEvent->bUseFtaCntMgmt)
	{
		HxSTD_MemCopy(&pstFileEvent->ftaCntMgmt, &pstEvent->ftaCntMgmt, sizeof(SvcMeta_FtaCntMgmt_t));
	}
	if (pstEvent->bUseGuidance)
	{
		HxSTD_MemCopy(&pstFileEvent->guidance, &pstEvent->guidance, sizeof(SvcMeta_Guidance_t));
	}
#endif

	// count ext event number
	pstExtEvt = pstEvent->pstExtEvt;
	while( pstExtEvt != NULL )
	{
		ulExtEvtNum++;
		pstExtEvt = pstExtEvt->pNext;
	}
	pstFileEvent->ulExtEvtNum = ulExtEvtNum;

	if( ulExtEvtNum == 0 )
	{
		return ERR_OK;
	}

	// convert ext event  to file ext event
	astFileExtEvt = (svcMetaExtEvent_t*) HLIB_STD_MemCalloc( ulExtEvtNum * sizeof(svcMetaExtEvent_t) );
	if( astFileExtEvt == NULL )
	{
		pstFileEvent->ulExtEvtNum = 0;

		HxLOG_Error("HLIB_STD_MemCalloc fail : %d\n", ulExtEvtNum * sizeof(svcMetaExtEvent_t));

		return ERR_OK;
	}
	HxSTD_MemSet( astFileExtEvt, 0, ulExtEvtNum * sizeof(svcMetaExtEvent_t) );

	pstExtEvt = pstEvent->pstExtEvt;
	for( i=0; i<ulExtEvtNum; i++ )
	{
		if( pstExtEvt == NULL ) break;

		astFileExtEvt[i].ucDesNum = pstExtEvt->ucDesNum;
		astFileExtEvt[i].ucLastDesNum = pstExtEvt->ucLastDesNum;
		HxSTD_MemCopy( astFileExtEvt[i].szIso639LangCode, pstExtEvt->szIso639LangCode, 4 );

		astFileExtEvt[i].ucItemNum = pstExtEvt->ucItemNum;
		for( j=0; j<pstExtEvt->ucItemNum; j++ )
		{
			if (pstExtEvt->astItem[j].pszDescription)
			{
				astFileExtEvt[i].astItem[j].ucDescriptionLength = MWC_UTIL_DvbStrlen(pstExtEvt->astItem[j].pszDescription);
				HxSTD_MemCopy( astFileExtEvt[i].astItem[j].aucDescription, pstExtEvt->astItem[j].pszDescription, astFileExtEvt[i].astItem[j].ucDescriptionLength );
			}
			else
			{
				astFileExtEvt[i].astItem[j].ucDescriptionLength = 0;
				astFileExtEvt[i].astItem[j].aucDescription[0]	= '\0';
			}

			if (pstExtEvt->astItem[j].pszItem)
			{
				astFileExtEvt[i].astItem[j].ucItemLength = MWC_UTIL_DvbStrlen(pstExtEvt->astItem[j].pszItem);
				HxSTD_MemCopy( astFileExtEvt[i].astItem[j].aucItem, pstExtEvt->astItem[j].pszItem, astFileExtEvt[i].astItem[j].ucItemLength );
			}
			else
			{
				astFileExtEvt[i].astItem[j].ucItemLength= 0;
				astFileExtEvt[i].astItem[j].aucItem[0]	= '\0';
			}
		}

		astFileExtEvt[i].ucCharLength = MWC_UTIL_DvbStrlen(pstExtEvt->pszChar);
		HxSTD_MemCopy( astFileExtEvt[i].aucChar, pstExtEvt->pszChar, astFileExtEvt[i].ucCharLength );

		pstExtEvt = pstExtEvt->pNext;
	}

	*pstFileExtEvt = astFileExtEvt;

	return ERR_OK;
}

STATIC HERROR svc_meta_encodeStrAsUrlEncoding(HINT32	sd, HCHAR	*pszFieldName, HCHAR	*pszStr)
{
	HUINT32		len = 0;
	HCHAR		*buf= NULL;

	len = HLIB_STD_URLStrLen(pszStr) + 1;
    buf = HLIB_STD_MemCalloc(len+1);
    HLIB_STD_URLEncode(buf, pszStr, len);

	HLIB_RWSTREAM_Print(sd, "\"%s\":\"%s\"", 	pszFieldName, buf);

	HLIB_STD_MemFree(buf);

	return ERR_OK;
}


HERROR		svc_meta_encodePvrTimeStamp (HINT32		sd, SvcMeta_Timestamp_t 	*pstTimeStamp, const HCHAR *objname)
{
	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body
	HLIB_RWSTREAM_Print(sd, "\"%s\":%d", 		"eType", 		pstTimeStamp->eType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 	"ulTime", 		pstTimeStamp->ulTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 	"ulPosition", 	pstTimeStamp->ulPosition);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 	"ulSize", 		pstTimeStamp->ulSize);
//	HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 	"aucReserved", 	pstTimeStamp->aucReserved);
	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}

#if 0
HERROR		svc_meta_encodePVR_MetaContent_EventInfo (HINT32	sd, svcMetaEvent_t *pstEventInfo, const HCHAR *objname)
{
	HUINT32	i;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body

	HLIB_RWSTREAM_Print(sd, "\"%s\":%d", 			"ulStartTime", 		pstEventInfo->ulStartTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulDuration", 		pstEventInfo->ulDuration);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u3RunnigStatus", 	pstEventInfo->u3RunnigStatus);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1FreeCaMode", 	pstEventInfo->u1FreeCaMode);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucReserved", 		pstEventInfo->ucReserved);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 			"astParentalRating");
	for (i = 0; i < pstEventInfo->ucParentalRatingNum ; i++)
	{
		SvcMeta_Rating_t	*pstParentalRating = &pstEventInfo->astParentalRating[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":\"%s\"", 		"aucIsoCountryCode",pstParentalRating->aucIsoCountryCode);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucParentalRating", pstParentalRating->ucParentalRating);
		HLIB_RWSTREAM_Print(sd, "}");	// pstParentalRating
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astParentalRating

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucContentNibble", 	pstEventInfo->ucContentNibble);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucUserNibble", 	pstEventInfo->ucUserNibble);
	HLIB_RWSTREAM_Print(sd, ",");
	svc_meta_encodeStrAsUrlEncoding(sd, 		"aucEventName", 	pstEventInfo->aucEventName);
	HLIB_RWSTREAM_Print(sd, ",");
	svc_meta_encodeStrAsUrlEncoding(sd, 		"aucEventText", 	pstEventInfo->aucEventText);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"nEvtId", 	pstEventInfo->nEvtId);


	HLIB_RWSTREAM_Print(sd, ", \"%s\":[",			"astComponent");
	for (i = 0; i < EPG_COMPONENT_DESC_MAX; i++)
	{
		SvcMeta_Component_t		*pstEvtComp		= &pstEventInfo->astComponent[i];

		if (0 == pstEvtComp->ucStreamContent)
		{
			HxLOG_Print("[%s:%d] (%d) component encoded \n", __FUNCTION__, __LINE__, i);
			break;
		}

        if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ucStreamContent", 	pstEvtComp->ucStreamContent);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucComponentType", 	pstEvtComp->ucComponentType);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"",		"aucIsoCountryCode",pstEvtComp->aucIsoCountryCode);
		HLIB_RWSTREAM_Print(sd, "}");	// pstEvtComp
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astComponent

	//HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 	"aucReserved",		pstEventInfo->aucReserved);

#if defined(CONFIG_OP_FREESAT)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":", 			"ftaCntMgmt");
	HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 				"ucDoNotScramble", 	pstEventInfo->ftaCntMgmt.ucDoNotScramble);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 			"ucCtrlRAccOverINet", 	pstEventInfo->ftaCntMgmt.ucCtrlRAccOverINet);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d}", 			"ucDoNotApplyRevocation", 	pstEventInfo->ftaCntMgmt.ucDoNotApplyRevocation);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":",			"guidance");
	HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",				"eType",		pstEventInfo->guidance.eType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 		"stLangCode",	pstEventInfo->guidance.stLangCode);
	HLIB_RWSTREAM_Print(sd, ",");
	svc_meta_encodeStrAsUrlEncoding(sd, 			"stData", 		pstEventInfo->guidance.stData);
	HLIB_RWSTREAM_Print(sd, "}");
#endif

	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}
#endif

HERROR		svc_meta_encodePVR_MetaContent_ExtEventInfo (HINT32	sd, HUINT32 ulExtEvtNum, svcMetaExtEvent_t	*pstFileExtEvt, const HCHAR *objname)
{
	HUINT32	i, k;


	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "[");		// array
	for (i = 0; i < ulExtEvtNum; ++i)
	{
		svcMetaExtEvent_t	*pstExtEvt = &pstFileExtEvt[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ucDesNum", 		pstExtEvt->ucDesNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucLastDesNum", 	pstExtEvt->ucLastDesNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 	"szIso639LangCode",	pstExtEvt->szIso639LangCode);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 		"astItem");

		for (k = 0; k < pstExtEvt->ucItemNum; ++k)
		{
			svcMetaExtEventItem_t	*pstItem = &pstExtEvt->astItem[k];

			if (0 < k)
				HLIB_RWSTREAM_Print(sd, ",");

			HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",			"ucDescriptionLength",	pstItem->ucDescriptionLength);
			HLIB_RWSTREAM_Print(sd, ",");
			svc_meta_encodeStrAsUrlEncoding(sd, 		"aucDescription", 	pstItem->aucDescription);
			HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"ucItemLength",	pstItem->ucItemLength);
			HLIB_RWSTREAM_Print(sd, ",");
			svc_meta_encodeStrAsUrlEncoding(sd, 		"aucItem", 	pstItem->aucItem);
			HLIB_RWSTREAM_Print(sd, "}");
		}
		HLIB_RWSTREAM_Print(sd, "]"); // astItem

		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucCharLength", 		pstExtEvt->ucCharLength);
		HLIB_RWSTREAM_Print(sd, ",");
		svc_meta_encodeStrAsUrlEncoding(sd, 		"aucChar", 				pstExtEvt->aucChar);
		HLIB_RWSTREAM_Print(sd, "}");
	}
	HLIB_RWSTREAM_Print(sd, "]");	// array

	return ERR_OK;
}
#if 0 // N/A
STATIC HERROR	svc_meta_SetEventTimeStampToJMeta(HUINT32 ulContentsId, HUINT32 ulTime, SvcMeta_Event_t *pstInfo)
{

	HERROR				ulRet = ERR_OK;
	SvcMeta_Timestamp_t 	stTimeStamp;
	svcMetaEvent_t	stFileEvtInfo;
	svcMetaExtEvent_t 	*pstFileExtEvt= NULL;
	SvcMeta_Record_t		stRec;
	HCHAR				*timeStampBuf = NULL;
	HUINT8	aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];

	HxJSON_t	json, jsonTimeStampList;
	HINT32		sd;
	HBOOL		bExist = FALSE;


	// 1. check the content (in the list & file open)

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		HxLOG_Error("don't exist in meta list : %d\n", ulContentsId);
		return ERR_FAIL;
	}

	// data set
	HxSTD_MemSet( &stFileEvtInfo, 0, sizeof(svcMetaEvent_t) );
	svc_meta_ConvertEventToFileEvent( pstInfo, &stFileEvtInfo, &pstFileExtEvt );

	HxSTD_MemSet( &stTimeStamp, 0, sizeof(SvcMeta_Timestamp_t) );
	stTimeStamp.ulTime = ulTime;
	stTimeStamp.eType = eSvcMeta_Timestamp_EIT;

	HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				stRec.aucPath,
				stRec.aucFileName, EXT_JMETA_TSMPFILE);

	HxLOG_Info("[%s:%d] ulContentsId:%d, ulTime:%d, nEvtId:0x%X, ulExtEvtNum:%d\n"
		, __FUNCTION__, __LINE__, ulContentsId, ulTime, stFileEvtInfo.nEvtId, stFileEvtInfo.ulExtEvtNum);


	// 2. check the "TimeStampList"
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc, HLIB_STD_MemFree);
	if (!sd)
	{
		HxLOG_Warning("[%s:%d] R/W Stream open fail!\n", __HxFILE__, __HxLINE__);
		if( pstFileExtEvt != NULL )
			HLIB_STD_MemFree( pstFileExtEvt );
		return ERR_FAIL;
	}
	bExist = HLIB_FILE_Exist( aucFileName );
	if (FALSE == bExist)
	{	// 	a. non exist : add the "TimeStampList" with the time stamp info ("SvcMeta_Timestamp_t" + "stEventInfo", "stExtEventInfo")
		HxLOG_Info("(%s) non exist case\n", aucFileName);

		HLIB_RWSTREAM_Print(sd, "{\"%s\":[{", "TimeStampList");
		svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
		HLIB_RWSTREAM_Print(sd, ", ");
		svc_meta_encodePVR_MetaContent_EventInfo(sd, &stFileEvtInfo, "stEventInfo");
		HLIB_RWSTREAM_Print(sd, ", ");
		svc_meta_encodePVR_MetaContent_ExtEventInfo(sd, stFileEvtInfo.ulExtEvtNum, pstFileExtEvt, "stExtEventInfo");
		HLIB_RWSTREAM_Print(sd, "}]}");

		timeStampBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else
	{	//	b. exist     : append the time stamp info (("SvcMeta_Timestamp_t" + "stEventInfo", "stExtEventInfo") to the "TimeStampList"
		HxLOG_Info("(%s) exist case\n", aucFileName);

		//json = ;
		json = svc_meta_ReadTimestamp(aucFileName);
		if (json == NULL)
		{
			HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n",aucFileName);
			if( pstFileExtEvt != NULL )
				HLIB_STD_MemFree( pstFileExtEvt );
			return ERR_FAIL;
		}

		jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");
		if (jsonTimeStampList)
		{
			HCHAR		*newJsonBuf 	= NULL;
			HxJSON_t	jsonNewTimeStamp= NULL;

			HLIB_RWSTREAM_Print(sd, "{");
			svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
			HLIB_RWSTREAM_Print(sd, ", ");
			svc_meta_encodePVR_MetaContent_EventInfo(sd, &stFileEvtInfo, "stEventInfo");
			HLIB_RWSTREAM_Print(sd, ", ");
			svc_meta_encodePVR_MetaContent_ExtEventInfo(sd, stFileEvtInfo.ulExtEvtNum, pstFileExtEvt, "stExtEventInfo");
			HLIB_RWSTREAM_Print(sd, "}");
			newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
			jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
			HLIB_STD_MemFree(newJsonBuf);


			if (NULL == jsonNewTimeStamp)
			{   // 죽도록 놔두자 (정확히 decoding 되지 않는 원인을 잡아야 하므로)

				HxLOG_Error("fail to decode new eventTimeStamp with (%s)\n", newJsonBuf);

				if( pstFileExtEvt != NULL )
					HLIB_STD_MemFree( pstFileExtEvt );

				HLIB_JSON_Delete(json);
				HxLOG_Assert(jsonNewTimeStamp);
				return ERR_FAIL;
			}

			HLIB_JSON_Array_Append(jsonTimeStampList, jsonNewTimeStamp);
			timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
		}

		HLIB_JSON_Delete(json);
	}
	HxLOG_Info("timestamp =(%s)\n", timeStampBuf);

	if( pstFileExtEvt != NULL )
		HLIB_STD_MemFree( pstFileExtEvt );

	ulRet = svc_meta_WriteTimestamp(aucFileName, timeStampBuf);
	if (timeStampBuf)
		HLIB_RWSTREAM_MemFree(timeStampBuf);

	if(ulRet != ERR_OK)
	{
		HxLOG_Error("save Error : ulRet:0x%X, %s\n", ulRet, aucFileName);
	}
	return ulRet;
}
#endif
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
HERROR		svc_meta_encodePVR_MetaContent_LicenseDrmInfo (HINT32		sd, svcMetaLicenseDrmInfo_t *pstLicenseDrm, const HCHAR *objname)
{
#if 1 // License Data를 encode 해서 저장해야 하면 아래 루틴을 탈 것(동작 테스트 필요! 검증안된 code)! But, License Data는 이미 암호화 되어있기 때문에 encode 해서 저장할 필요는 없을듯.
	svcMetaLicenseDrmInfo_t		stLicenseDrm = {0};
	svcMetaLicenseDrmInfo_t 	*pstLicenseDrmTmp = &stLicenseDrm;

	const	HCHAR	*pszEncodedStr;
	HxJSONBIN_t	encoder = HLIB_JSONBIN_ENCODER_Open();

	HxSTD_MemCopy(pstLicenseDrmTmp, pstLicenseDrm, sizeof(svcMetaLicenseDrmInfo_t));
	pszEncodedStr = svc_meta_EncryptAsBase64(encoder, (HUINT8*)pstLicenseDrmTmp, sizeof(svcMetaLicenseDrmInfo_t));
	if (NULL == pszEncodedStr)
	{
		HxLOG_Error("svc_meta_EncryptAsBase64() Fail! \n");
		HLIB_JSONBIN_ENCODER_Close(encoder);
		return ERR_FAIL;
	}

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_PutString(sd, pszEncodedStr);
	HLIB_JSONBIN_ENCODER_Close(encoder);

#else
		if (objname)
			HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

		HLIB_RWSTREAM_Print(sd, "{");				// start body

		HLIB_RWSTREAM_Print(sd, "\"%s\":\"%s\"", 	"aucLicenseData", 	pstLicenseDrm->aucLicenseData);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 	"usLicenseLength", 	pstLicenseDrm->usLicenseLength);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 	"usProgramNum", 	pstLicenseDrm->usProgramNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", "aucBindedCamId", 	pstLicenseDrm->aucBindedCamId);
		HLIB_RWSTREAM_Print(sd, "}");				// end body
#endif

	return ERR_OK;
}

#if 0
STATIC HERROR	svc_meta_SetCiPlusLicenseDrmTimeStampToJMeta(HUINT32 ulContentsId, HUINT32 ulTime, DxCopyrightControl_CiPlus_t *pstCiPlusDrmInfo, SvcMeta_Record_t *pstRecordInfo)
{
	DxCopyrightControl_CiPlusLicenseInfo_t 		*pstLicenseInfo = &pstCiPlusDrmInfo->ciplusDrmInfo.stCiPlusLicenseInfo;
	HERROR						hErr = ERR_OK;
	SvcMeta_Timestamp_t 			stTimeStamp;
	svcMetaLicenseDrmInfo_t 	stLicenseDrm;
	HCHAR						*timeStampBuf = NULL;
	HUINT8						aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];

	HxJSON_t					json, jsonTimeStampList;
	HINT32						sd;
	HBOOL						bExist = FALSE;

	// data set
	HxSTD_MemSet(&stLicenseDrm, 0x00, sizeof(svcMetaLicenseDrmInfo_t));
	stLicenseDrm.usProgramNum = pstLicenseInfo->usProgramNum;
	stLicenseDrm.usLicenseLength = pstLicenseInfo->usLicenseLength;
	HxSTD_MemCopy(stLicenseDrm.aucLicenseData, pstLicenseInfo->pucLicenseData, sizeof(HUINT8) * pstLicenseInfo->usLicenseLength);
	HxSTD_MemCopy(stLicenseDrm.aucBindedCamId, pstLicenseInfo->aucBindedCamId, sizeof(HUINT8) * SVC_META_CAMID_LEN);

	HxSTD_MemSet( &stTimeStamp, 0, sizeof(SvcMeta_Timestamp_t) );
	stTimeStamp.ulTime = ulTime;
	stTimeStamp.eType = eSvcMeta_Timestamp_LICENSE_DRM;

	HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				pstRecordInfo->aucPath,
				pstRecordInfo->aucFileName, EXT_JMETA_TSMPFILE);

	HxLOG_Info("ulContentsId:%d, ulTime:%d, eDrmType: CI+ License \n", ulContentsId, ulTime);

	// check the "TimeStampList"
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
	{
		HxLOG_Warning("[%s:%d] R/W Stream open fail!\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}
	bExist = HLIB_FILE_Exist( aucFileName );
	if (FALSE == bExist)
	{	// 	a. non exist : add the "TimeStampList" with the time stamp info ("SvcMeta_Timestamp_t" + "stFileDrm")
		HxLOG_Info("(%s) non exist case\n", aucFileName);

		HLIB_RWSTREAM_Print(sd, "{\"%s\":[{", "TimeStampList");
		svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
		HLIB_RWSTREAM_Print(sd, ", ");
		svc_meta_encodePVR_MetaContent_LicenseDrmInfo(sd, &stLicenseDrm, "stLicenseDrm");
		HLIB_RWSTREAM_Print(sd, "}]}");

		timeStampBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else
	{	//	b. exist     : append the time stamp info (("SvcMeta_Timestamp_t" + "stFileDrm") to the "TimeStampList"
		HxLOG_Info("(%s) exist case\n", aucFileName);

		json = svc_meta_ReadTimestamp(aucFileName);

		if (json == NULL)
		{
			HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n",aucFileName);
			return ERR_FAIL;
		}
		jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");
		if (jsonTimeStampList)
		{
			HCHAR		*newJsonBuf 	= NULL;
			HxJSON_t	jsonNewTimeStamp= NULL;

			HLIB_RWSTREAM_Print(sd, "{");
			svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
			HLIB_RWSTREAM_Print(sd, ", ");
			svc_meta_encodePVR_MetaContent_LicenseDrmInfo(sd, &stLicenseDrm, "stLicenseDrm");
			HLIB_RWSTREAM_Print(sd, "}");
			newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
			jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
			HLIB_STD_MemFree(newJsonBuf);

			if (NULL == jsonNewTimeStamp)
			{
				HLIB_JSON_Delete(json);
				HxLOG_Assert(jsonNewTimeStamp);
				return ERR_FAIL;
			}

			HLIB_JSON_Array_Append(jsonTimeStampList, jsonNewTimeStamp);
			timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
		}

		HLIB_JSON_Delete(json);
	}
	HxLOG_Info("timestamp =(%s)\n", timeStampBuf);

	hErr = svc_meta_WriteTimestamp(aucFileName, timeStampBuf);
	if (timeStampBuf)
		HLIB_STD_MemFree(timeStampBuf);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("save Error : ulRet:0x%X, %s\n", hErr, aucFileName);
		return hErr;
	}
	return ERR_OK;
}
#endif
STATIC HERROR svc_meta_loadJMeta_LicenseDrm(svcMetaLicenseDrmInfo_t *pstLicenseDrm, HxJSON_t jsonLicenseDrmInfo)
{
	if (NULL == jsonLicenseDrmInfo || NULL == pstLicenseDrm)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	if (ERR_FAIL == svc_meta_DecryptFromBase64Json(jsonLicenseDrmInfo, (HUINT8*)pstLicenseDrm, sizeof(svcMetaLicenseDrmInfo_t)))
	{
		HxLOG_Error("Fail... svc_meta_DecryptFromBase64Json()... \n");
		return ERR_FAIL;
	}

	HxLOG_Info("License Length(%d)\n", pstLicenseDrm->usLicenseLength);

#if defined (CONFIG_DEBUG)
	{
		int i ;
		HxLOG_Info("(");

		for (i = 0; i < pstLicenseDrm->usLicenseLength; i++)
		{
			HxLOG_Print("%02x ", pstLicenseDrm->aucLicenseData[i]);
		}
		HxLOG_Info(")\n");
	}
#endif
	return ERR_OK;
}
#endif


HERROR		svc_meta_encodePVR_MetaContent_DrmInfo (HINT32		sd, svcMetaDrmInfo_t *pstFileDrm, const HCHAR *objname)
{
	DxCopyrightControl_t			stDrm = {0};
	DxCopyrightControl_t 			*pstDrm = &stDrm;


	HxSTD_MemCopy(pstDrm, &pstFileDrm->stDrm, sizeof(DxCopyrightControl_t));

    {
		HxJSONBIN_t	encoder = HLIB_JSONBIN_ENCODER_Open();
		const	HCHAR	*pszDrmEncodedStr = svc_meta_EncryptAsBase64(encoder, (HUINT8*)pstDrm, sizeof(DxCopyrightControl_t));
		if (NULL == pszDrmEncodedStr)
		{
			HxLOG_Error("svc_meta_EncryptAsBase64() Fail! \n");
			HLIB_JSONBIN_ENCODER_Close(encoder);
			return ERR_FAIL;
		}

		if (objname)
			HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

		HLIB_RWSTREAM_Print(sd, "{");				// start body
		HLIB_RWSTREAM_Print(sd, "\"%s\":", 			"stDrmInfo");
		HLIB_RWSTREAM_PutString(sd, pszDrmEncodedStr);
		HLIB_JSONBIN_ENCODER_Close(encoder);

		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 			"ulCrc", pstFileDrm->ulCrc);
		HLIB_RWSTREAM_Print(sd, "}");				// end body
	}

	return ERR_OK;
}

STATIC HERROR	svc_meta_SetDrmTimeStampToJMeta(HUINT32 ulContentsId, HUINT32 ulTime, DxCopyrightControl_t *pstInfo)
{

	HERROR					 ulRet = ERR_OK;
	SvcMeta_Timestamp_t 	 stTimeStamp;
	svcMetaDrmInfo_t 		*pstFileDrm = NULL;
	SvcMeta_Record_t		*pstRecordInfo = NULL;
	HCHAR					*timeStampBuf = NULL;
	HCHAR					*szFileName = NULL;

	HxJSON_t	json, jsonTimeStampList;
	HINT32		sd;
	HBOOL		bExist = FALSE;
#ifdef USE_LICENSE_DATA_JSON_STRING
	svcMetaLicenseDrmInfo_t	stLicenseDrm;
#endif
	HERROR				 hResult = ERR_FAIL;

	pstFileDrm = (svcMetaDrmInfo_t *)OxMW_Malloc(sizeof(svcMetaDrmInfo_t));
	pstRecordInfo = (SvcMeta_Record_t *)OxMW_Malloc(sizeof(SvcMeta_Record_t));
	szFileName = (HCHAR *)OxMW_Malloc(PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	if ((NULL == pstFileDrm) || (NULL == pstRecordInfo) || (NULL == szFileName))
	{
		HxLOG_Error("Memory Allocation Failed:\n");
		goto END_FUNC;
	}

	// 1. check the content (in the list & file open)
	if (SVC_META_GetRecordInfo(ulContentsId, pstRecordInfo) != ERR_OK)
	{
		HxLOG_Error("don't exist in meta list : %d\n", ulContentsId);
		goto END_FUNC;
	}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	if (pstInfo->stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		switch (pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
		{
			case eDxCopyrightControl_CiPlusDrmType_License:
#ifdef USE_LICENSE_DATA_JSON_STRING
				// data set
				HxSTD_MemSet(&stLicenseDrm, 0x00, sizeof(svcMetaLicenseDrmInfo_t));
				stLicenseDrm.usProgramNum = pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum;
				stLicenseDrm.usLicenseLength = pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength;
				HxSTD_MemCopy(stLicenseDrm.aucLicenseData, pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucLicenseData, sizeof(HUINT8) * pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength);
				HxSTD_MemCopy(stLicenseDrm.aucBindedCamId, pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, sizeof(HUINT8) * SVC_META_CAMID_LEN);
				if (pstRecordInfo->bCamNeedContent == FALSE)
				{
					pstRecordInfo->bCamNeedContent = TRUE;
					HxSTD_MemCopy(pstRecordInfo->aucBindedCamId,
						pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, SVC_META_CAMID_LEN);
					SVC_META_SetRecordInfo(ulContentsId, pstRecordInfo);
				}
#endif
				break;
			case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:	// 저장 자체는 기존 저장 방법 그대로 사용 함. Playback 할 때 확인
			{
				HxLOG_Info("Time from DI timestamp[%08x], from PIN_event APDU[%08x]\n", ulTime, pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.utTimeStamp);
				ulTime = pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.utTimeStamp;	// CI+ CAM으로 실제 APDU로 올라오는 시간 확인 해봐야 함!(kkkim)
				if (pstRecordInfo->bCamNeedContent == FALSE)
				{
					pstRecordInfo->bCamNeedContent = TRUE;
					HxSTD_MemCopy(pstRecordInfo->aucBindedCamId,
						pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucBindedCamId, SVC_META_CAMID_LEN);
					SVC_META_SetRecordInfo(ulContentsId, pstRecordInfo);
				}
				break;
			}
			case eDxCopyrightControl_CiPlusDrmType_Uri:			// URI의 경우는 기존 저장 방법 그대로 사용.
				break;
			case eDxCopyrightControl_CiPlusDrmType_RecCtrl:
				HxLOG_Warning("eDxCopyrightControl_CiPlusDrmType_RecCtrl information should not be here!!\n");
				break;
			default:
				HxLOG_Warning("DRM Type[%02x]!!\n", pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType);
				break;
		}
	}
#endif

	// data set
	HxSTD_MemSet(pstFileDrm, 0x00, sizeof(svcMetaDrmInfo_t));
	HxSTD_MemCopy(&(pstFileDrm->stDrm), pstInfo, sizeof(DxCopyrightControl_t));

	HxSTD_MemSet( &stTimeStamp, 0, sizeof(SvcMeta_Timestamp_t) );
	stTimeStamp.ulTime = ulTime;
	stTimeStamp.eType = eSvcMeta_Timestamp_DRM;

	HxSTD_MemSet(szFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(szFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				pstRecordInfo->aucPath,
				pstRecordInfo->aucFileName, EXT_JMETA_TSMPFILE);

	HxLOG_Info("[%s:%d] ulContentsId:%d, ulTime:%d, eDrmType:%d \n"
		, __FUNCTION__, __LINE__, ulContentsId, ulTime, pstFileDrm->stDrm.stCasCcInfo.eCasCcType);


	// 2. check the "TimeStampList"
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
	{
		HxLOG_Warning("[%s:%d] R/W Stream open fail!\n", __HxFILE__, __HxLINE__);
		goto END_FUNC;
	}
	bExist = HLIB_FILE_Exist(szFileName);
	if (FALSE == bExist)
	{	// 	a. non exist : add the "TimeStampList" with the time stamp info ("SvcMeta_Timestamp_t" + "stFileDrm")
		HxLOG_Info("(%s) non exist case\n", szFileName);

		HLIB_RWSTREAM_Print(sd, "{\"%s\":[{", "TimeStampList");
		svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
		HLIB_RWSTREAM_Print(sd, ", ");
		svc_meta_encodePVR_MetaContent_DrmInfo(sd, pstFileDrm, "stFileDrm");

#ifdef USE_LICENSE_DATA_JSON_STRING
		if (pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_License)
		{
			HLIB_RWSTREAM_Print(sd, ", ");
			svc_meta_encodePVR_MetaContent_LicenseDrmInfo(sd, &stLicenseDrm, "stLicenseDrm");
		}
#endif
		HLIB_RWSTREAM_Print(sd, "}]}");

		timeStampBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else
	{	//	b. exist     : append the time stamp info (("SvcMeta_Timestamp_t" + "stFileDrm") to the "TimeStampList"
		HxLOG_Info("(%s) exist case\n", szFileName);

		json = svc_meta_ReadTimestamp(szFileName);

		if (json == NULL)
		{
			HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n", szFileName);
			goto END_FUNC;
		}
		jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");
		if (jsonTimeStampList)
		{
			HCHAR		*newJsonBuf 	= NULL;
			HxJSON_t	jsonNewTimeStamp= NULL;

			HLIB_RWSTREAM_Print(sd, "{");
			svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
			HLIB_RWSTREAM_Print(sd, ", ");
			svc_meta_encodePVR_MetaContent_DrmInfo(sd, pstFileDrm, "stFileDrm");
#ifdef USE_LICENSE_DATA_JSON_STRING
			if (pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_License)
			{
				HLIB_RWSTREAM_Print(sd, ", ");
				svc_meta_encodePVR_MetaContent_LicenseDrmInfo(sd, &stLicenseDrm, "stLicenseDrm");
			}
#endif
			HLIB_RWSTREAM_Print(sd, "}");
			newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
			jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
			HLIB_STD_MemFree(newJsonBuf);

			if (NULL == jsonNewTimeStamp)
			{
				HLIB_JSON_Delete(json);
				HxLOG_Assert(jsonNewTimeStamp);
				goto END_FUNC;
			}

			HLIB_JSON_Array_Append(jsonTimeStampList, jsonNewTimeStamp);
			timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
		}

		HLIB_JSON_Delete(json);
	}

	if (timeStampBuf)
	{
		HxLOG_Info("timestamp =(%s)\n", timeStampBuf);

		ulRet = svc_meta_WriteTimestamp(szFileName, timeStampBuf);
		if (timeStampBuf)
			HLIB_RWSTREAM_MemFree(timeStampBuf);
		if(ulRet != ERR_OK)
		{
			HxLOG_Error("save Error : ulRet:0x%X, %s\n", ulRet, szFileName);
		}
	}


	hResult = ulRet;

END_FUNC:
	if (NULL != pstFileDrm)						{ OxMW_Free(pstFileDrm); }
	if (NULL != pstRecordInfo)					{ OxMW_Free(pstRecordInfo); }
	if (NULL != szFileName)						{ OxMW_Free(szFileName); }

	return hResult;
}

HERROR		svc_meta_encodePVR_MetaContent_ServiceAudio (HINT32		sd, SvcMeta_Service_t *pstServiceInfo, const HCHAR *objname)
{
	HUINT32			i, j;
	SvcMeta_AudioInfoList_t	*pstAudioInfoList = &pstServiceInfo->stAudioInfoList;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);
	HLIB_RWSTREAM_Print(sd, "{");				// start body

	HLIB_RWSTREAM_Print(sd, "\"%s\":[", 			"astAudioInfo");
	for (i = 0; i < pstAudioInfoList->ulItemNum ; i++)
	{
		HUINT32	ulLangCount = 0;
		SvcMeta_PvrAudioInfo_t	*pstAudioInfo = &pstAudioInfoList->astAudioInfo[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ulLangNum", pstAudioInfo->ulLangNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 			"stLangInfo");
		for (j = 0; j < pstAudioInfo->ulLangNum; j++)
		{
			SvcMeta_Iso639LangInfo_t	*pstLangInfo = &pstAudioInfo->stLangInfo[j];

			if (0 < ulLangCount)
				HLIB_RWSTREAM_Print(sd, ",");

			if (HxSTD_StrLen(pstLangInfo->szIso639LangCode) <= 0)	continue;

			HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"eAudType",			pstLangInfo->eAudType);
			HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"",		"szIso639LangCode",	pstLangInfo->szIso639LangCode);
			HLIB_RWSTREAM_Print(sd, "}");	// pstIsoLangInfo

			ulLangCount++;
		}
		HLIB_RWSTREAM_Print(sd, "]");	// stLangInfo
		HLIB_RWSTREAM_Print(sd, "}");	// pstAudioInfo
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astAudioInfo
	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}



HERROR		svc_meta_encodePVR_MetaContent_ServiceSbtl (HINT32		sd, SvcMeta_Service_t *pstServiceInfo, const HCHAR *objname)
{
	HUINT32 		i;
	SvcMeta_SbtlInfoList_t *pstSbtlInfoList = &pstServiceInfo->stSbtlInfoList;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);
	HLIB_RWSTREAM_Print(sd, "{");				// start body

	HLIB_RWSTREAM_Print(sd, "\"%s\":[", 			"astSbtlInfo");
	for (i = 0; i < pstSbtlInfoList->ulItemNum ; i++)
	{
		SvcMeta_SbtlInfo_t	*pstSbtlInfo = &pstSbtlInfoList->astSbtlInfo[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ucComponentType",	pstSbtlInfo->ucComponentType);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 	"aucLangCode", 		pstSbtlInfo->aucLangCode);
		HLIB_RWSTREAM_Print(sd, "}");	// pstSbtlInfo
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astSbtlInfo
	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}



HERROR		svc_meta_encodePVR_MetaContent_ServiceInfo (HINT32		sd, SvcMeta_Service_t *pstServiceInfo, const HCHAR *objname)
{
	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body
	HLIB_RWSTREAM_Print(sd, "\"%s\":%d", 			"nChNumber", 		pstServiceInfo->nChNumber);
	HLIB_RWSTREAM_Print(sd, ",");
	svc_meta_encodeStrAsUrlEncoding(sd, 		"aucServiceName", 	pstServiceInfo->aucServiceName);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usSvcId",			pstServiceInfo->usSvcId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usTsId",			pstServiceInfo->usTsId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usOnId",			pstServiceInfo->usOnId);
#if defined(CONFIG_OP_FREESAT)
	HLIB_RWSTREAM_Print(sd, ",\"%s\":%d", 			"nMasterChNumber", 		pstServiceInfo->nMasterChNumber);
	HLIB_RWSTREAM_Print(sd, ",");
	svc_meta_encodeStrAsUrlEncoding(sd, 		"aucMasterServiceName", 	pstServiceInfo->aucMasterServiceName);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usMasterSvcId",			pstServiceInfo->usMasterSvcId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usMasterTsId",			pstServiceInfo->usMasterTsId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usMasterOnId",			pstServiceInfo->usMasterOnId);
#endif
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usPmtPid",			pstServiceInfo->usPmtPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usPcrPid",			pstServiceInfo->usPcrPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usTtxPid",			pstServiceInfo->usTtxPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usCaptionPid",		pstServiceInfo->usCaptionPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usVideoPid",		pstServiceInfo->usVideoPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eVideoCodec",		pstServiceInfo->eVideoCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usAudioPid",		pstServiceInfo->usAudioPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eAudioCodec",		pstServiceInfo->eAudioCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usAudioAuxPid",	pstServiceInfo->usAudioAuxPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eAudioAuxCodec",	pstServiceInfo->eAudioAuxCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usDolbyPid",		pstServiceInfo->usDolbyPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eDolbyCodec",		pstServiceInfo->eDolbyCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"bDolbyFlag",		pstServiceInfo->bDolbyFlag);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eSvcType",			pstServiceInfo->eSvcType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eVideoType",		pstServiceInfo->eVideoType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eCasType",			pstServiceInfo->eCasType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"ucSubttlIdx",		pstServiceInfo->ucSubttlIdx);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"bSubttlUserFlag",	pstServiceInfo->bSubttlUserFlag);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eSoundMode",		pstServiceInfo->eSoundMode);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eDualMono",		pstServiceInfo->eDualMono);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"bAudUserSetFlag",	pstServiceInfo->bAudUserSetFlag);

	HLIB_RWSTREAM_Print(sd, ", ");
	svc_meta_encodePVR_MetaContent_ServiceAudio(sd, pstServiceInfo, 	"stAudioInfoList");

	HLIB_RWSTREAM_Print(sd, ", ");
	svc_meta_encodePVR_MetaContent_ServiceSbtl(sd, pstServiceInfo, 	"stSbtlInfoList");

	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}

STATIC HERROR	svc_meta_SetServiceTimeStampToJMeta( HUINT32 ulContentsId, HUINT32 ulTime, SvcMeta_Service_t *pstInfo )
{

	SvcMeta_Record_t		stRecordInfo;
	HERROR				ulRet = ERR_OK;
	SvcMeta_Timestamp_t 	stTimeStamp;
	HCHAR				*timeStampBuf = NULL;
	HUINT8	aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];

	HxJSON_t	json, jsonTimeStampList;
	HINT32		sd;
	HBOOL		bExist = FALSE;


	// 1. check the content (in the list & file open)
	if (SVC_META_GetRecordInfo(ulContentsId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("don't exist in meta list : %d\n", ulContentsId);
		return ERR_FAIL;
	}

	// data set
	HxSTD_MemSet( &stTimeStamp, 0, sizeof(SvcMeta_Timestamp_t) );
	stTimeStamp.ulTime = ulTime;
	stTimeStamp.eType = eSvcMeta_Timestamp_PMT;

	HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				stRecordInfo.aucPath,
				stRecordInfo.aucFileName, EXT_JMETA_TSMPFILE);

	HxLOG_Info("ulContentsId:%d, ulTime:%d", ulContentsId, ulTime);


	// 2. check the "TimeStampList"
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
	{
		HxLOG_Warning("[%s:%d] R/W Stream open fail!\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}
	bExist = HLIB_FILE_Exist( aucFileName );
	if (FALSE == bExist)
	{	// 	a. non exist : add the "TimeStampList" with the time stamp info ("SvcMeta_Timestamp_t" + "stServiceInfo")
		HxLOG_Info("(%s) non exist case\n", aucFileName);

		HLIB_RWSTREAM_Print(sd, "{\"%s\":[{", "TimeStampList");
		svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
		HLIB_RWSTREAM_Print(sd, ", ");
		svc_meta_encodePVR_MetaContent_ServiceInfo(sd, pstInfo, "stServiceInfo");
		HLIB_RWSTREAM_Print(sd, "}]}");

		timeStampBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else
	{	//	b. exist     : append the time stamp info (("SvcMeta_Timestamp_t" + "stServiceInfo") to the "TimeStampList"
		HxLOG_Info("(%s) exist case\n", aucFileName);

		json = svc_meta_ReadTimestamp(aucFileName);

		if (json == NULL)
		{
			HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n",aucFileName);
			return ERR_FAIL;
		}
		jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");
		if (jsonTimeStampList)
		{
			HCHAR		*newJsonBuf 	= NULL;
			HxJSON_t	jsonNewTimeStamp= NULL;

			HLIB_RWSTREAM_Print(sd, "{");
			svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "SvcMeta_Timestamp_t");
			HLIB_RWSTREAM_Print(sd, ", ");
			svc_meta_encodePVR_MetaContent_ServiceInfo(sd, pstInfo, "stServiceInfo");
			HLIB_RWSTREAM_Print(sd, "}");
			newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
			jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
			HLIB_STD_MemFree(newJsonBuf);

			if (NULL == jsonNewTimeStamp)
			{	// decoding fail -> Assert
				HLIB_JSON_Delete(json);
				HxLOG_Assert(jsonNewTimeStamp);
				return ERR_FAIL;
			}

			HLIB_JSON_Array_Append(jsonTimeStampList, jsonNewTimeStamp);
			timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
		}

		HLIB_JSON_Delete(json);
	}
	HxLOG_Info("timestamp =(%s)\n", timeStampBuf);

	ulRet = svc_meta_WriteTimestamp(aucFileName, timeStampBuf);
	if (timeStampBuf)
		HLIB_RWSTREAM_MemFree(timeStampBuf);
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("save Error : ulRet:0x%X, %s\n", ulRet, aucFileName);
	}
	return ulRet;
}


STATIC HERROR svc_meta_loadJMeta_ServiceAudio(SvcMeta_Service_t *pstServiceInfo, HxJSON_t jAudioInfoList)
{
	const HCHAR				*pszStr;
	SvcMeta_AudioInfoList_t	*pstAudioInfoList;
	HxJSON_t		array, item;
	HINT32			i, j;

	if (NULL == jAudioInfoList || NULL == pstServiceInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	pstAudioInfoList = &pstServiceInfo->stAudioInfoList;

	array		= HxJSON_Object_Get (jAudioInfoList, "astAudioInfo");
	if (array)
	{
		pstAudioInfoList->ulItemNum = HLIB_JSON_GetSize(array);
		for (i = 0; i < pstAudioInfoList->ulItemNum; ++i)
		{
			item = HxJSON_Array_Get (array, i);
			if (item)
			{
				SvcMeta_PvrAudioInfo_t	*pstAudioInfo = &pstAudioInfoList->astAudioInfo[i];
				HxJSON_t			subArray, subItem;

				subArray = HxJSON_Object_Get(item, "stLangInfo");
				if (NULL == subArray)		continue;
				pstAudioInfo->ulLangNum = HLIB_JSON_GetSize(subArray);
				for (j = 0; j < pstAudioInfo->ulLangNum; ++j)
				{
					SvcMeta_Iso639LangInfo_t	*pstLangInfo = &pstAudioInfo->stLangInfo[j];
					subItem = HxJSON_Array_Get (subArray, j);

					pstLangInfo->eAudType	= HLIB_JSON_Object_GetInteger(subItem, "eAudType");
					pszStr					= HLIB_JSON_Object_GetString(subItem, 	"szIso639LangCode");
					if (pszStr)
						HxSTD_StrNCpy(pstLangInfo->szIso639LangCode, pszStr, LANG_639_CODE_MAX);
				}
			}
		}	// end for
	}
	return ERR_OK;
}



STATIC HERROR svc_meta_loadJMeta_ServiceSbtl(SvcMeta_Service_t *pstServiceInfo, HxJSON_t jSbtlInfoList)
{
	const HCHAR				*pszStr;
	SvcMeta_SbtlInfoList_t	*pstSbtlInfoList;
	HxJSON_t		array, item;
	HINT32			i;

	if (NULL == jSbtlInfoList || NULL == pstServiceInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	pstSbtlInfoList = &pstServiceInfo->stSbtlInfoList;
	array			= HxJSON_Object_Get (jSbtlInfoList, "astSbtlInfo");
	if (array)
	{
		pstSbtlInfoList->ulItemNum = HLIB_JSON_GetSize(array);
		for (i = 0; i < pstSbtlInfoList->ulItemNum; ++i)
		{
			item = HxJSON_Array_Get (array, i);
			if (item)
			{
				SvcMeta_SbtlInfo_t	*pstSbtlInfo = &pstSbtlInfoList->astSbtlInfo[i];

				pstSbtlInfo->ucComponentType= HLIB_JSON_Object_GetInteger(item, "ucComponentType");
				pszStr						= HLIB_JSON_Object_GetString(item, "aucLangCode");
				if (pszStr)
					HxSTD_StrNCpy(pstSbtlInfo->aucLangCode, pszStr, LANG_639_CODE_MAX);
			}
		}	// end for
	}
	return ERR_OK;
}


STATIC HERROR svc_meta_ConvertEventFileToEvent( svcMetaEvent_t *pstFileEvent, svcMetaExtEvent_t *astFileExtEvt, SvcMeta_Event_t *pstEvent )
{
	HUINT32 ulLength = 0;
	HUINT32 ulExtEvtNum = 0, i, j;
	SvcMeta_ExtEvt_t *pstExtEvt = NULL;
	svcMetaExtEvent_t *pstFileExtEvt = NULL;
	SvcMeta_ExtEvt_t	*pstPvrExtHead = NULL;

	if( (pstEvent == NULL) || (pstFileEvent == NULL ) )
		return ERR_FAIL;

	pstEvent->ulStartTime = pstFileEvent->ulStartTime;
	pstEvent->ulDuration = pstFileEvent->ulDuration;
	pstEvent->u3RunnigStatus = pstFileEvent->u3RunnigStatus;
	pstEvent->u1FreeCaMode = pstFileEvent->u1FreeCaMode;
	pstEvent->nEventId = pstFileEvent->nEvtId;
	pstEvent->ucParentalRatingNum = pstFileEvent->ucParentalRatingNum;
	HxSTD_MemCopy( pstEvent->astParentalRating, pstFileEvent->astParentalRating,
				pstFileEvent->ucParentalRatingNum * sizeof(SvcMeta_Rating_t) );
	pstEvent->ucContentNibble = pstFileEvent->ucContentNibble;
	pstEvent->ucUserNibble = pstFileEvent->ucUserNibble;
	HxSTD_MemCopy( pstEvent->aucEventName, pstFileEvent->aucEventName, PVR_SHORT_EVENT_NAME_LEN );
	HxSTD_MemCopy( pstEvent->aucEventText, pstFileEvent->aucEventText, PVR_EVENT_TEXT );

	HxSTD_MemCopy( pstEvent->stComponent, pstFileEvent->astComponent, sizeof(SvcMeta_Component_t) * EPG_COMPONENT_DESC_MAX);

	pstEvent->bUseFtaCntMgmt = FALSE;
	pstEvent->bUseGuidance = FALSE;
#if defined(CONFIG_OP_FREESAT)
	HxSTD_MemCopy(&pstEvent->ftaCntMgmt, &pstFileEvent->ftaCntMgmt, sizeof(SvcMeta_FtaCntMgmt_t));
	HxSTD_MemCopy(&pstEvent->guidance, &pstFileEvent->guidance, sizeof(SvcMeta_Guidance_t));
	pstEvent->bUseFtaCntMgmt = TRUE;
	pstEvent->bUseGuidance = TRUE;
#endif
	ulExtEvtNum = pstFileEvent->ulExtEvtNum;
	if( (ulExtEvtNum == 0) || (astFileExtEvt == NULL) )
	{
		pstEvent->pstExtEvt = NULL;
		return ERR_OK;
	}

	pstPvrExtHead = NULL;
	for( i=0; i<ulExtEvtNum; i++ )
	{
		if( pstPvrExtHead == NULL )
		{
			pstExtEvt = (SvcMeta_ExtEvt_t *)HLIB_STD_MemCalloc( sizeof(SvcMeta_ExtEvt_t) );
			if( pstExtEvt == NULL ) break;
			HxSTD_MemSet( pstExtEvt, 0, sizeof(SvcMeta_ExtEvt_t ) );

			pstPvrExtHead = pstExtEvt;
		} else {
			pstExtEvt->pNext = (SvcMeta_ExtEvt_t *)HLIB_STD_MemCalloc( sizeof(SvcMeta_ExtEvt_t) );
			pstExtEvt = pstExtEvt->pNext;
			if( pstExtEvt == NULL ) break;
			HxSTD_MemSet( pstExtEvt, 0, sizeof(SvcMeta_ExtEvt_t ) );
		}

		pstFileExtEvt = &astFileExtEvt[i];

		pstExtEvt->ucDesNum = pstFileExtEvt->ucDesNum;
		pstExtEvt->ucLastDesNum = pstFileExtEvt->ucLastDesNum;
		HxSTD_MemCopy( pstExtEvt->szIso639LangCode, pstFileExtEvt->szIso639LangCode, 4 );
		pstExtEvt->ucItemNum = pstFileExtEvt->ucItemNum;
		for( j=0; j<pstExtEvt->ucItemNum; j++ )
		{
			ulLength = pstFileExtEvt->astItem[j].ucDescriptionLength;
			if (ulLength)
			{
				pstExtEvt->astItem[j].pszDescription = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength + 1));
				if( pstExtEvt->astItem[j].pszDescription != NULL )
				{
					HxSTD_MemCopy( pstExtEvt->astItem[j].pszDescription, pstFileExtEvt->astItem[j].aucDescription, ulLength );
					pstExtEvt->astItem[j].pszDescription[ulLength] = '\0';
				}
			}
            else
            {
                pstExtEvt->astItem[j].pszDescription = NULL;
            }

			ulLength = pstFileExtEvt->astItem[j].ucItemLength;
			if (ulLength)
			{
				pstExtEvt->astItem[j].pszItem = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength + 1));
				if( pstExtEvt->astItem[j].pszItem != NULL )
				{
					HxSTD_MemCopy( pstExtEvt->astItem[j].pszItem, pstFileExtEvt->astItem[j].aucItem, ulLength );
					pstExtEvt->astItem[j].pszItem[ulLength] = '\0';
				}
			}
            else
            {
                pstExtEvt->astItem[j].pszItem = NULL;
            }
		}

		ulLength = pstFileExtEvt->ucCharLength;
		if( ulLength > 0 )
		{
			pstExtEvt->pszChar = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength + 1));
			if( pstExtEvt->pszChar != NULL )
			{
				HxSTD_MemCopy( pstExtEvt->pszChar, pstFileExtEvt->aucChar, ulLength );
				pstExtEvt->pszChar[ulLength] = '\0';
			}
		}

	}
	pstEvent->pstExtEvt = pstPvrExtHead;

	return ERR_OK;
}


STATIC HERROR svc_meta_loadJMeta_FileDrm(svcMetaDrmInfo_t *pstFileDrm, HxJSON_t jsonFileDrmInfo)
{
	HxJSON_t	jsonDrmInfo;

	if (NULL == jsonFileDrmInfo || NULL == pstFileDrm)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	jsonDrmInfo = HxJSON_Object_Get(jsonFileDrmInfo, "stDrmInfo");
	if (NULL == jsonDrmInfo)
	{
		HxLOG_Error(": can't get the stDrmInfo json \n");
		return ERR_FAIL;
	}

	if (ERR_FAIL == svc_meta_DecryptFromBase64Json(jsonDrmInfo, (HUINT8*)&pstFileDrm->stDrm, sizeof(DxCopyrightControl_t)))
	{
		HxLOG_Error(": Fail in svc_meta_DecryptFromBase64Json() \n");
		return ERR_FAIL;
	}
	pstFileDrm->ulCrc			= HLIB_JSON_Object_GetInteger(jsonFileDrmInfo, "ulCrc");

	HxLOG_Print("[%s:%d] meta Crc=(%d)\n", __FUNCTION__, __LINE__, pstFileDrm->ulCrc);

	return ERR_OK;
}
STATIC HERROR svc_meta_loadJMeta_ExtEventInfo(svcMetaExtEvent_t *pstExtEventInfo, HxJSON_t jExtEvtList)
{
	const HCHAR		*pszStr;
	HUINT32	ulExtEvtCnt, i;

	if (NULL == jExtEvtList || NULL == pstExtEventInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}


	ulExtEvtCnt = HLIB_JSON_GetSize(jExtEvtList);

	HxLOG_Print("[%s:%d] ext evt cnt (%d)\n", __FUNCTION__, __LINE__, ulExtEvtCnt);
	for (i = 0; i < ulExtEvtCnt; ++i)
	{
		svcMetaExtEvent_t *pstExtEvtUnit= &pstExtEventInfo[i];
		HxJSON_t			jExtEvt 	= HxJSON_Array_Get (jExtEvtList, i);

		if (jExtEvt)
		{
			HxSTD_MemSet(pstExtEvtUnit, 0, sizeof(svcMetaExtEvent_t));

			pstExtEvtUnit->ucDesNum = HLIB_JSON_Object_GetInteger(jExtEvt, "ucDesNum");
			pstExtEvtUnit->ucLastDesNum = HLIB_JSON_Object_GetInteger(jExtEvt, "ucLastDesNum");
			pszStr	= HLIB_JSON_Object_GetString(jExtEvt, "szIso639LangCode");
			if (pszStr)
				HLIB_STD_StrNCpySafe(pstExtEvtUnit->szIso639LangCode, pszStr, 4);

			HxLOG_Print("[%s:%d]	(%d) idx ext evt, des num(%d/%d), lngcode(%s)\n", __FUNCTION__, __LINE__
				, i, pstExtEvtUnit->ucDesNum, pstExtEvtUnit->ucLastDesNum, pstExtEvtUnit->szIso639LangCode);

			pstExtEvtUnit->ucItemNum = 0;
			{
				HINT32		k;
				HxJSON_t	jExtItemArray 	= HxJSON_Object_Get (jExtEvt, "astItem");
				if (jExtItemArray)
				{
					pstExtEvtUnit->ucItemNum	= HLIB_JSON_GetSize(jExtItemArray);


					for (k = 0; k < pstExtEvtUnit->ucItemNum; ++k)
					{
						HxJSON_t	jExtEvtItem 	= HxJSON_Array_Get (jExtItemArray, k);
						pstExtEvtUnit->astItem[k].ucDescriptionLength = HLIB_JSON_Object_GetInteger(jExtEvtItem, "ucDescriptionLength");
						pszStr	= HLIB_JSON_Object_GetString(jExtEvtItem, "aucDescription");
						if (pszStr)
						{
							HxSTD_StrNCpy(pstExtEvtUnit->astItem[k].aucDescription, pszStr, pstExtEvtUnit->astItem[k].ucDescriptionLength);
						}

						pstExtEvtUnit->astItem[k].ucItemLength = HLIB_JSON_Object_GetInteger(jExtEvtItem, "ucItemLength");
						pszStr	= HLIB_JSON_Object_GetString(jExtEvtItem, "aucItem");
						if (pszStr)
						{
							HxSTD_StrNCpy(pstExtEvtUnit->astItem[k].aucItem, pszStr, pstExtEvtUnit->astItem[k].ucItemLength);
						}

						HxLOG_Print("[%s:%d] 	(%d) item idx descLen(%d), desc (%s), itemLen(%d), item (%s)\n", __FUNCTION__, __LINE__
							, k, pstExtEvtUnit->astItem[k].ucDescriptionLength, pstExtEvtUnit->astItem[k].aucDescription
							, pstExtEvtUnit->astItem[k].ucItemLength, pstExtEvtUnit->astItem[k].aucItem);
					}	// end for k
				}
			}	// end if (pstExtEvtUnit->ucItemNum)

			pstExtEvtUnit->ucCharLength = HLIB_JSON_Object_GetInteger(jExtEvt, "ucCharLength");
			pszStr	= HLIB_JSON_Object_GetString(jExtEvt, "aucChar");
			if (pszStr)
			{
				HxSTD_StrNCpy(pstExtEvtUnit->aucChar, pszStr, pstExtEvtUnit->ucCharLength);
			}

			HxLOG_Print("[%s:%d] (%d) idx char len (%d), char (%s)\n", __FUNCTION__, __LINE__
				, i, pstExtEvtUnit->ucCharLength, pstExtEvtUnit->aucChar);
		}	// if (jExtEvt)
	}	// end for (i = 0; i < ulExtEvtCnt; ++i)


	return ERR_OK;
}
STATIC HERROR svc_meta_loadJMeta_EventInfo(svcMetaEvent_t	*pstEventInfo, HxJSON_t jEvt)
{
	const HCHAR		*pszStr;
	HxJSON_t		jPRArray, jCompArray;

	if (NULL == jEvt || NULL == pstEventInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}


	pstEventInfo->ulStartTime	= HLIB_JSON_Object_GetInteger(jEvt, "ulStartTime");
	pstEventInfo->ulDuration	= HLIB_JSON_Object_GetInteger(jEvt, "ulDuration");
	pstEventInfo->u3RunnigStatus= HLIB_JSON_Object_GetInteger(jEvt, "u3RunnigStatus");
	pstEventInfo->u1FreeCaMode	= HLIB_JSON_Object_GetInteger(jEvt, "u1FreeCaMode");
	pstEventInfo->ucReserved	= HLIB_JSON_Object_GetInteger(jEvt, "ucReserved");

	HxLOG_Print("[%s:%d] ulStartTime (%d) ulDuration (%d) u3RunnigStatus(%d) u1FreeCaMode(%d) \n", __FUNCTION__, __LINE__,
		pstEventInfo->ulStartTime, pstEventInfo->ulDuration, pstEventInfo->u3RunnigStatus, pstEventInfo->u1FreeCaMode);

	jPRArray	= HxJSON_Object_Get (jEvt, "astParentalRating");
	if (jPRArray)
	{
		HINT32	i;

		pstEventInfo->ucParentalRatingNum = HLIB_JSON_GetSize(jPRArray);
		for (i = 0; i < pstEventInfo->ucParentalRatingNum; ++i)
		{
			HxJSON_t	jPR = HxJSON_Array_Get (jPRArray, i);
			if (jPR)
			{
				pszStr	= HLIB_JSON_Object_GetString(jPR, "aucIsoCountryCode");
				if (pszStr)
					HxSTD_StrNCpy(pstEventInfo->astParentalRating[i].aucIsoCountryCode, pszStr, LANG_639_CODE_MAX);
				pstEventInfo->astParentalRating[i].ucParentalRating	= HLIB_JSON_Object_GetInteger(jPR, "ucParentalRating");

				HxLOG_Print("[%s:%d] (%d) idx aucIsoCountryCode (%s) ucParentalRating(%d) \n", __FUNCTION__, __LINE__,
					i, pstEventInfo->astParentalRating[i].aucIsoCountryCode, pstEventInfo->astParentalRating[i].ucParentalRating);
			}
		}	// end for
	}

	pstEventInfo->ucContentNibble	= HLIB_JSON_Object_GetInteger(jEvt, "ucContentNibble");
	pstEventInfo->ucUserNibble		= HLIB_JSON_Object_GetInteger(jEvt, "ucUserNibble");
	pszStr	= HLIB_JSON_Object_GetString(jEvt, "aucEventName");
	if (pszStr)
		HxSTD_StrNCpy(pstEventInfo->aucEventName, pszStr, PVR_SHORT_EVENT_NAME_LEN);

	pszStr	= HLIB_JSON_Object_GetString(jEvt, "aucEventText");
	if (pszStr)
		HxSTD_StrNCpy(pstEventInfo->aucEventText, pszStr, PVR_EVENT_TEXT);
	pstEventInfo->nEvtId			= HLIB_JSON_Object_GetInteger(jEvt, "nEvtId");

	HxLOG_Print("[%s:%d] ucContentNibble(%d) ucUserNibble(%d) evtName (%s) txt (%s) id(%d)\n", __FUNCTION__, __LINE__
		, pstEventInfo->ucContentNibble, pstEventInfo->ucUserNibble, pstEventInfo->aucEventName, pstEventInfo->aucEventText, pstEventInfo->nEvtId);


	jCompArray	= HxJSON_Object_Get (jEvt, "astComponent");
	if (jCompArray)
	{
		HUINT32	i;
		HUINT32	ulCompCnt = HLIB_JSON_GetSize(jCompArray);

		for (i = 0; i < ulCompCnt; ++i)
		{
			HxJSON_t	jComp = HxJSON_Array_Get (jCompArray, i);
			if (jComp)
			{
				pstEventInfo->astComponent[i].ucStreamContent	= HLIB_JSON_Object_GetInteger(jComp, "ucStreamContent");
				pstEventInfo->astComponent[i].ucComponentType	= HLIB_JSON_Object_GetInteger(jComp, "ucComponentType");
				pszStr	= HLIB_JSON_Object_GetString(jComp, "aucIsoCountryCode");
				if (pszStr)
					HxSTD_StrNCpy(pstEventInfo->astComponent[i].aucIsoCountryCode, pszStr, LANG_639_CODE_MAX);

				HxLOG_Print("[%s:%d] (%d) idx aucIsoCountryCode (%s) content (%d) type (%d)\n", __FUNCTION__, __LINE__
					, i, pstEventInfo->astComponent[i].aucIsoCountryCode
					, pstEventInfo->astComponent[i].ucStreamContent
					, pstEventInfo->astComponent[i].ucComponentType
					);
			}
		}	// end for
	}

#if defined(CONFIG_OP_FREESAT)
{
	HxJSON_t	jFtaCntMgm, jGuidance;

	jFtaCntMgm = HxJSON_Object_Get(jEvt, "ftaCntMgmt");
	if (jFtaCntMgm)
	{
		pstEventInfo->ftaCntMgmt.ucDoNotScramble 		= HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucDoNotScramble");
		pstEventInfo->ftaCntMgmt.ucCtrlRAccOverINet 	= HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucCtrlRAccOverINet");
		pstEventInfo->ftaCntMgmt.ucDoNotApplyRevocation = HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucDoNotApplyRevocation");
	}

	jGuidance = HxJSON_Object_Get(jEvt, "guidance");
	if (jGuidance)
	{
		pstEventInfo->guidance.eType 		= HLIB_JSON_Object_GetInteger(jGuidance, "eType");
		pszStr	= HLIB_JSON_Object_GetString(jGuidance, "stLangCode");
		if (pszStr)
			HxSTD_StrNCpy(pstEventInfo->guidance.stLangCode, pszStr, LANG_639_CODE_MAX );
		pszStr	= HLIB_JSON_Object_GetString(jGuidance, "stData");
		if (pszStr)
			HxSTD_StrNCpy(pstEventInfo->guidance.stData, pszStr, DxMAX_GUIDANCE_DATA_LEN );
	}
}
#endif // CONFIG_OP_FREESAT

	return ERR_OK;
}

STATIC HERROR svc_meta_loadJMeta_ServiceInfo(SvcMeta_Service_t *pstServiceInfo, HxJSON_t jsonServiceInfo)
{
	const HCHAR		*pszStr;

	if (NULL == jsonServiceInfo || NULL == pstServiceInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}


	pstServiceInfo->nChNumber	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "nChNumber");
	pszStr	= HLIB_JSON_Object_GetString(jsonServiceInfo, "aucServiceName");
	if (pszStr)
		HLIB_STD_StrNCpySafe(pstServiceInfo->aucServiceName, pszStr, PVR_MAX_SERVICE_NAME_LEN);

	pstServiceInfo->usSvcId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usSvcId");
	pstServiceInfo->usTsId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usTsId");
	pstServiceInfo->usOnId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usOnId");
#if defined(CONFIG_OP_FREESAT)
	pstServiceInfo->nMasterChNumber	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "nMasterChNumber");
	pszStr	= HLIB_JSON_Object_GetString(jsonServiceInfo, "aucMasterServiceName");
	if (pszStr)
		HxSTD_StrNCpy(pstServiceInfo->aucMasterServiceName, pszStr, PVR_MAX_SERVICE_NAME_LEN);

	pstServiceInfo->usMasterSvcId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usMasterSvcId");
	pstServiceInfo->usMasterTsId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usMasterTsId");
	pstServiceInfo->usMasterOnId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usMasterOnId");
#endif
	pstServiceInfo->usPmtPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usPmtPid");
	pstServiceInfo->usPcrPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usPcrPid");
	pstServiceInfo->usTtxPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usTtxPid");
	pstServiceInfo->usCaptionPid= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usCaptionPid");
	pstServiceInfo->usVideoPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usVideoPid");
	pstServiceInfo->eVideoCodec	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eVideoCodec");

	pstServiceInfo->usAudioPid		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usAudioPid");
	pstServiceInfo->eAudioCodec		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eAudioCodec");
	pstServiceInfo->usAudioAuxPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usAudioAuxPid");
	pstServiceInfo->eAudioAuxCodec	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eAudioAuxCodec");
	pstServiceInfo->usDolbyPid		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usDolbyPid");
	pstServiceInfo->eDolbyCodec		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eDolbyCodec");
	pstServiceInfo->bDolbyFlag		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "bDolbyFlag");

	pstServiceInfo->eSvcType		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eSvcType");
	pstServiceInfo->eVideoType		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eVideoType");
	pstServiceInfo->eCasType		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eCasType");
	pstServiceInfo->ucSubttlIdx		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "ucSubttlIdx");
	pstServiceInfo->bSubttlUserFlag	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "bSubttlUserFlag");
	pstServiceInfo->eSoundMode		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eSoundMode");
	pstServiceInfo->eDualMono		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eDualMono");
	pstServiceInfo->bAudUserSetFlag	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "bAudUserSetFlag");

	{
		HxJSON_t	jAudioInfoList 	= HxJSON_Object_Get(jsonServiceInfo, "stAudioInfoList");
		HxJSON_t	jSbtlInfoList 	= HxJSON_Object_Get(jsonServiceInfo, "stSbtlInfoList");

		svc_meta_loadJMeta_ServiceAudio(pstServiceInfo, jAudioInfoList);
		svc_meta_loadJMeta_ServiceSbtl(pstServiceInfo, jSbtlInfoList);
	}


	HxLOG_Print("[%s:%d] meta ch num(%d), svcName(%s) tripleid(%d/%d/%d)\n", __FUNCTION__, __LINE__,
		pstServiceInfo->nChNumber, pstServiceInfo->aucServiceName, pstServiceInfo->usOnId, pstServiceInfo->usTsId, pstServiceInfo->usSvcId);

	return ERR_OK;
}

STATIC HERROR	svc_meta_LoadTimeStampListFromJMeta(HUINT32 ulContentsId, SvcMeta_TimestampList_t *pstTimeStampList)
{

	SvcMeta_Record_t		stRecordInfo;
	HUINT8	aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	SvcMeta_TimestampList_t *pstTimeStampNaviList = NULL;
	HxJSON_t	json;

	if (SVC_META_GetRecordInfo(ulContentsId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("don't exist in meta db : %d\n", ulContentsId);
		return ERR_FAIL;
	}

	HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				stRecordInfo.aucPath,
				stRecordInfo.aucFileName,  EXT_JMETA_TSMPFILE);

	// load the data - encrypted -> read & decrypt, normal text -> decode directly
	json = svc_meta_ReadTimestamp(aucFileName);
	if (json == NULL)
	{
		HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n",aucFileName);
		return ERR_FAIL;
	}
	else
	{
		HxJSON_t	jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");

		if (jsonTimeStampList)
		{
			HINT32	i;
			HUINT32	ulTimeStmpCnt = HLIB_JSON_GetSize(jsonTimeStampList);

			pstTimeStampNaviList 			= pstTimeStampList;
			pstTimeStampNaviList->ulNumber 	= ulTimeStmpCnt;
			pstTimeStampNaviList->pstData = (SvcMeta_TimestampData_t *) HLIB_STD_MemCalloc( (ulTimeStmpCnt)* sizeof(SvcMeta_TimestampData_t) );

			HxLOG_Info(": ulTimeStmpCnt (%d) \n", ulTimeStmpCnt);

			for (i = 0; i < ulTimeStmpCnt; ++i)
			{
				HxJSON_t				jTimeSmp 	= HxJSON_Array_Get (jsonTimeStampList, i);

				if (jTimeSmp)
				{
					HxJSON_t	jTimeSmpPvrTimeStamp = HxJSON_Object_Get(jTimeSmp, "SvcMeta_Timestamp_t");
					if (jTimeSmpPvrTimeStamp)
					{
						svcMetaContents_t	*pstFileMeta = NULL;
						HxJSON_t	jDataInfo = NULL;
						SvcMeta_Timestamp_t *pstTimeStamp 	= &pstTimeStampNaviList->pstData[i].stTimeStamp;
						pstTimeStamp->eType				= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "eType");
						pstTimeStamp->ulTime			= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulTime");

						pstFileMeta = (svcMetaContents_t *)HLIB_STD_MemCalloc(sizeof(svcMetaContents_t));
						if (pstFileMeta == NULL)
						{
							HxLOG_Error("Memory Alloc Fail!!\n");
							return ERR_FAIL;
						}

						switch (pstTimeStamp->eType)
						{
							case eSvcMeta_Timestamp_PMT:
								{
									SvcMeta_Service_t		*pstService = NULL;
									jDataInfo = HxJSON_Object_Get(jTimeSmp, "stServiceInfo");
									svc_meta_loadJMeta_ServiceInfo(&pstFileMeta->stServiceInfo, jDataInfo);

									pstService = (SvcMeta_Service_t *) HLIB_STD_MemCalloc( sizeof(SvcMeta_Service_t) );
									HxSTD_MemCopy(pstService, &pstFileMeta->stServiceInfo, sizeof(SvcMeta_Service_t) );
									pstTimeStampNaviList->pstData[i].pData = (void *)pstService;
									break;
								}

							case eSvcMeta_Timestamp_EIT:
								{
									SvcMeta_Event_t		*pstEvt			= NULL;
									HxJSON_t			jExtEventInfo	= NULL;
									svcMetaExtEvent_t *pstFileExtEvt 	= NULL;

									jDataInfo = HxJSON_Object_Get(jTimeSmp, "stEventInfo");
									if (ERR_FAIL == svc_meta_loadJMeta_EventInfo(&pstFileMeta->stEventInfo, jDataInfo))
									{
										HLIB_JSON_Delete(json);
										if(pstFileMeta)		{	HLIB_STD_MemFree(pstFileMeta);	}
										return ERR_FAIL;
									}

									jExtEventInfo = HxJSON_Object_Get(jTimeSmp, "stExtEventInfo");
									if (jExtEventInfo)
									{
										pstFileMeta->stEventInfo.ulExtEvtNum = HLIB_JSON_GetSize(jExtEventInfo);
										if (pstFileMeta->stEventInfo.ulExtEvtNum)
										{
											pstFileExtEvt = (svcMetaExtEvent_t *) HLIB_STD_MemCalloc( sizeof(svcMetaExtEvent_t) * pstFileMeta->stEventInfo.ulExtEvtNum );
											if (pstFileExtEvt)
											{
												if (ERR_FAIL == svc_meta_loadJMeta_ExtEventInfo(pstFileExtEvt, jExtEventInfo))
												{
													HLIB_STD_MemFree(pstFileExtEvt);
													HLIB_JSON_Delete(json);
													if(pstFileMeta) 	{	HLIB_STD_MemFree(pstFileMeta);	}
													return ERR_FAIL;
												}
											}
										}
									}

									pstEvt = (SvcMeta_Event_t *) HLIB_STD_MemCalloc( sizeof(SvcMeta_Event_t) );
									svc_meta_ConvertEventFileToEvent(&pstFileMeta->stEventInfo, pstFileExtEvt, pstEvt );
									if (pstFileExtEvt)
										HLIB_STD_MemFree(pstFileExtEvt);
									pstTimeStampNaviList->pstData[i].pData = (void *)pstEvt;
									break;
								}

							case eSvcMeta_Timestamp_DRM:
								{
									DxCopyrightControl_t		*pstDrm = NULL;
									jDataInfo = HxJSON_Object_Get(jTimeSmp, "stFileDrm");
									svc_meta_loadJMeta_FileDrm(&pstFileMeta->stFileDrm, jDataInfo);

									pstDrm = (DxCopyrightControl_t *) HLIB_STD_MemCalloc( sizeof(DxCopyrightControl_t) );
									HxSTD_MemCopy(pstDrm, &pstFileMeta->stFileDrm.stDrm, sizeof(DxCopyrightControl_t) );

#ifdef USE_LICENSE_DATA_JSON_STRING
									if (pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_License)
									{
										HERROR			hErr;

										jDataInfo = HxJSON_Object_Get(jTimeSmp, "stLicenseDrm");
										if (jDataInfo)
										{
											hErr = svc_meta_loadJMeta_LicenseDrm(&pstFileMeta->stLicenseDrm, jDataInfo);
											if(hErr == ERR_OK)
											{
												pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum = pstFileMeta->stLicenseDrm.usProgramNum;
												pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength = pstFileMeta->stLicenseDrm.usLicenseLength;
												HxSTD_MemCopy(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucLicenseData,
													pstFileMeta->stLicenseDrm.aucLicenseData, sizeof(HUINT8) * pstFileMeta->stLicenseDrm.usLicenseLength);
												HxSTD_MemCopy(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, pstFileMeta->stLicenseDrm.aucBindedCamId, SVC_META_CAMID_LEN);
											}
											else
											{
												HxLOG_Error("svc_meta_loadJMeta_LicenseDrm() ERROR!!\n");
											}
										}
										else
										{
											HxLOG_Error("HxJSON_Object_Get() : stLicenseDrm ERROR!!\n");
										}
									}
#endif

									if(pstDrm->stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
									{
										pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ucCiPlusDrmInfoVer = 0;
									}
									pstTimeStampNaviList->pstData[i].pData = (void *)pstDrm;
									break;
								}

							#if 0 //defined(CONFIG_MW_CI_PLUS_CC_V_2)
							case eSvcMeta_Timestamp_LICENSE_DRM:
								{
									DxCopyrightControl_t		*pstDrm = NULL;
									HERROR			hErr;
									jDataInfo = HxJSON_Object_Get(jTimeSmp, "stLicenseDrm");
									hErr = svc_meta_loadJMeta_LicenseDrm(&stFileMeta.stLicenseDrm, jDataInfo);

									if(hErr == ERR_OK)
									{
										pstDrm = (DxCopyrightControl_t *) HLIB_STD_MemCalloc( sizeof(DxCopyrightControl_t) );
										pstDrm->stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_CiPlus;
										pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType = eDxCopyrightControl_CiPlusDrmType_License;
										pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ucCiPlusDrmInfoVer = 0;	// Metadata load시 version은 아무 의미 없음.
										pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum = stFileMeta.stLicenseDrm.usProgramNum;
										pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength = stFileMeta.stLicenseDrm.usLicenseLength;
										pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData = (HUINT8 *)HLIB_STD_MemCalloc(stFileMeta.stLicenseDrm.usLicenseLength);
										HxSTD_MemCopy(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData,
											stFileMeta.stLicenseDrm.aucLicenseData, sizeof(HUINT8) * stFileMeta.stLicenseDrm.usLicenseLength);
										HxSTD_MemCopy(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, stFileMeta.stLicenseDrm.aucBindedCamId, SVC_META_CAMID_LEN);
										pstTimeStampNaviList->pstData[i].pData = (void *)pstDrm;
									}
									else
									{
										HxLOG_Error("svc_meta_loadJMeta_LicenseDrm() ERROR!!\n");
									}
									break;
								}
							#endif

							default:
								HxLOG_Info(": TimeStamp type (%d) Error \n", pstTimeStamp->eType);
								break;
						}

						if(pstFileMeta)	{	HLIB_STD_MemFree(pstFileMeta);	}

					}
				}
			}	// end for
		}	// end if (jsonTimeStampList)
	}

	HLIB_JSON_Delete(json);

	return ERR_OK;
}

static HERROR	 svc_meta_UpdateDrmTimeStampToJMetaByIndex( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo, HUINT32 ulMetaIndex )
{
	HERROR					ulRet = ERR_OK;
	SvcMeta_Record_t		stRecordInfo;
	HCHAR					*timeStampBuf = NULL;
	HCHAR					*szFileName = NULL;

	HxJSON_t	json = NULL, jsonTimeStampList = NULL;
	HINT32		sd;
	HUINT32 	ulTimeStmpCnt;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	SvcMeta_Timestamp_t 	stTimeStamp;
	svcMetaDrmInfo_t		*pstFileDrm = NULL;
	HxJSON_t	jDataInfo = NULL;
	HxJSON_t	jTimeSmp = NULL;
	HINT32		i;
	HxJSON_t	jsonNewTimeStamp= NULL;
	HCHAR		*newJsonBuf 	= NULL;
#endif
#ifdef USE_LICENSE_DATA_JSON_STRING
	svcMetaLicenseDrmInfo_t	*pstLicenseDrm = NULL;
#endif
	HERROR		 hResult = ERR_FAIL;

	szFileName = (HCHAR *)OxMW_Malloc(PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	if (NULL == szFileName)
	{
		HxLOG_Error("Memory Allocation Failed:\n");
		goto END_FUNC;
	}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	pstFileDrm = (svcMetaDrmInfo_t *)OxMW_Malloc(sizeof(svcMetaDrmInfo_t));
	if (NULL == pstFileDrm)
	{
		HxLOG_Error("Memory Allocation Failed:\n");
		goto END_FUNC;
	}
#endif
#ifdef USE_LICENSE_DATA_JSON_STRING
	pstLicenseDrm = (svcMetaLicenseDrmInfo_t *)OxMW_Malloc(sizeof(svcMetaLicenseDrmInfo_t));
	if (NULL == pstLicenseDrm)
	{
		HxLOG_Error("Memory Allocation Failed:\n");
		goto END_FUNC;
	}
#endif

	// 1. check the content (in the list & file open)
	if (SVC_META_GetRecordInfo(ulContentsId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("don't exist in meta list : %d\n", ulContentsId);
		goto END_FUNC;
	}

	HxSTD_MemSet(szFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(szFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				stRecordInfo.aucPath,
				stRecordInfo.aucFileName, EXT_JMETA_TSMPFILE);

	json = svc_meta_ReadTimestamp(szFileName);

	if (json == NULL)
	{
		HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n", szFileName);
		goto END_FUNC;
	}

	// 2. check the "TimeStampList"
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
	{
		HxLOG_Warning("R/W Stream open fail!\n");
		goto END_FUNC;
	}

	jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");

	if (jsonTimeStampList == NULL)
	{
		HxLOG_Error("HxJSON_Object_Get() : TimeStampList ERROR!!\n");
		goto END_FUNC;
	}

	ulTimeStmpCnt = HLIB_JSON_GetSize(jsonTimeStampList);

					HxLOG_Info("ulTimeStmpCnt (%d), ulMetaIndex (%d)\n", ulTimeStmpCnt, ulMetaIndex);

	if (ulMetaIndex >= ulTimeStmpCnt)
	{
		HxLOG_Error("check ulMetaIndex (%d/%d) ERROR!!\n", ulMetaIndex, ulTimeStmpCnt);
		goto END_FUNC;
	}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	if (pstInfo->stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		switch (pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
		{
		case eDxCopyrightControl_CiPlusDrmType_Uri:
			{
				HUINT32 ulLicenseTime = 0;
				// ulMetaIndex 부터 1씩  빼며서 동일한 URI를 찾는다.
				for (i = ulMetaIndex ; i >= 0; i--)
				{
					jTimeSmp = HxJSON_Array_Get (jsonTimeStampList, i);

					if (jTimeSmp)
					{
						HxJSON_t	jTimeSmpPvrTimeStamp = HxJSON_Object_Get(jTimeSmp, "Pvr_TimeStamp_t");
						if (jTimeSmpPvrTimeStamp)
						{
							HxSTD_MemSet( &stTimeStamp, 0, sizeof(SvcMeta_Timestamp_t) );
							stTimeStamp.eType		= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "eType");
							stTimeStamp.ulTime		= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulTime");
							stTimeStamp.ulPosition	= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulPosition");
							stTimeStamp.ulSize		= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulSize");

							if (stTimeStamp.eType == eSvcMeta_Timestamp_DRM)
							{
								HERROR					hErr;

#ifndef USE_LICENSE_DATA_JSON_STRING
								jDataInfo = HxJSON_Object_Get(jTimeSmp, "stFileDrm");

								HxSTD_MemSet(pstFileDrm, 0x00, sizeof(svcMetaDrmInfo_t));
								hErr = svc_meta_loadJMeta_FileDrm(pstFileDrm, jDataInfo);
								if(hErr == ERR_OK)
								{
									if (pstFileDrm->stDrm.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_License)
									{
										ulLicenseTime = stTimeStamp.ulTime;
										continue; // next for statement
									}
									else if (pstFileDrm->stDrm.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_Uri)
									{
										if (ulLicenseTime == stTimeStamp.ulTime)
										{
											HxSTD_MemCopy(&(pstFileDrm->stDrm), pstInfo, sizeof(DxCopyrightControl_t));

											HLIB_RWSTREAM_Print(sd, "{");
											svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "Pvr_TimeStamp_t");
											HLIB_RWSTREAM_Print(sd, ", ");
											svc_meta_encodePVR_MetaContent_DrmInfo(sd, pstFileDrm, "stFileDrm");
											HLIB_RWSTREAM_Print(sd, "}");

											newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
											jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
											HLIB_STD_MemFree(newJsonBuf);

											if (NULL == jsonNewTimeStamp)
											{
												HLIB_JSON_Delete(json);
												HxLOG_Assert(jsonNewTimeStamp);
												goto END_FUNC;
											}

											HxJSON_Array_Set(jsonTimeStampList, i, jsonNewTimeStamp);
											timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);

											break; // exit for statement
										}
									}
								}
#else
								jDataInfo = HxJSON_Object_Get(jTimeSmp, "stLicenseDrm");
								if (jDataInfo)
								{
									HxSTD_MemSet(pstLicenseDrm, 0x00, sizeof(svcMetaLicenseDrmInfo_t));
									hErr = svc_meta_loadJMeta_LicenseDrm(pstLicenseDrm, jDataInfo);
									if(hErr == ERR_OK)
									{
										HxSTD_MemCopy(pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, pstLicenseDrm->aucBindedCamId, SVC_META_CAMID_LEN);
										if (pstLicenseDrm->usProgramNum == pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum)
										{
											pstLicenseDrm->usLicenseLength = pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength;
											HxSTD_MemSet(pstLicenseDrm->aucLicenseData, 0x00, META_MAX_LICENSE_DRM_LENGTH);
											HxSTD_MemCopy(pstLicenseDrm->aucLicenseData, pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucLicenseData, sizeof(HUINT8) * pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength);
											HxSTD_MemSet(pstFileDrm, 0x00, sizeof(svcMetaDrmInfo_t));
											HxSTD_MemCopy(pstFileDrm->stDrm, pstInfo, sizeof(DxCopyrightControl_t));

											HLIB_RWSTREAM_Print(sd, "{");
											svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "Pvr_TimeStamp_t");
											HLIB_RWSTREAM_Print(sd, ", ");
											svc_meta_encodePVR_MetaContent_DrmInfo(sd, pstFileDrm, "stFileDrm");

											HLIB_RWSTREAM_Print(sd, ", ");
											svc_meta_encodePVR_MetaContent_LicenseDrmInfo(sd, pstLicenseDrm, "stLicenseDrm");

											HLIB_RWSTREAM_Print(sd, "}");
											newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
											jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
											HLIB_STD_MemFree(newJsonBuf);

											if (NULL == jsonNewTimeStamp)
											{
												HLIB_JSON_Delete(json);
												HxLOG_Assert(jsonNewTimeStamp);
												goto END_FUNC;
											}

											HxJSON_Array_Set(jsonTimeStampList, ulMetaIndex, jsonNewTimeStamp);
											timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
										}
									}
									else
									{
										HxLOG_Error("HxJSON_Object_Get() : stLicenseDrm ERROR!!\n");
									}
								}
#endif
							}
						}
					}
				}
			}
			break;

		case eDxCopyrightControl_CiPlusDrmType_License:
			// data set
			jTimeSmp = HxJSON_Array_Get (jsonTimeStampList, ulMetaIndex);

			if (jTimeSmp)
			{
				HxJSON_t	jTimeSmpPvrTimeStamp = HxJSON_Object_Get(jTimeSmp, "Pvr_TimeStamp_t");
				if (jTimeSmpPvrTimeStamp)
				{
					HxSTD_MemSet( &stTimeStamp, 0, sizeof(SvcMeta_Timestamp_t) );
					stTimeStamp.eType		= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "eType");
					stTimeStamp.ulTime		= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulTime");
					stTimeStamp.ulPosition	= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulPosition");
					stTimeStamp.ulSize		= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulSize");

					if (stTimeStamp.eType == eSvcMeta_Timestamp_DRM)
					{
						HERROR					hErr;

#ifndef USE_LICENSE_DATA_JSON_STRING
						jDataInfo = HxJSON_Object_Get(jTimeSmp, "stFileDrm");

						HxSTD_MemSet(pstFileDrm, 0x00, sizeof(svcMetaDrmInfo_t));
						hErr = svc_meta_loadJMeta_FileDrm(pstFileDrm, jDataInfo);
						if(hErr == ERR_OK)
						{
							if (pstFileDrm->stDrm.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum == pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum)
							{
								HxSTD_MemCopy(pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, pstFileDrm->stDrm.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, SVC_META_CAMID_LEN);
								HxSTD_MemCopy(&(pstFileDrm->stDrm), pstInfo, sizeof(DxCopyrightControl_t));

								HLIB_RWSTREAM_Print(sd, "{");
								svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "Pvr_TimeStamp_t");
								HLIB_RWSTREAM_Print(sd, ", ");
								svc_meta_encodePVR_MetaContent_DrmInfo(sd, pstFileDrm, "stFileDrm");
								HLIB_RWSTREAM_Print(sd, "}");

								newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
								jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
								HLIB_STD_MemFree(newJsonBuf);

								if (NULL == jsonNewTimeStamp)
								{
									HLIB_JSON_Delete(json);
									HxLOG_Assert(jsonNewTimeStamp);
									goto END_FUNC;
								}

								HxJSON_Array_Set(jsonTimeStampList, ulMetaIndex, jsonNewTimeStamp);
								timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
							}
						}
#else
						jDataInfo = HxJSON_Object_Get(jTimeSmp, "stLicenseDrm");
						if (jDataInfo)
						{
							HxSTD_MemSet(pstLicenseDrm, 0x00, sizeof(PVR_File_LicenseDrmInfo_t));
							hErr = svc_meta_loadJMeta_LicenseDrm(pstLicenseDrm, jDataInfo);
							if(hErr == ERR_OK)
							{
								HxSTD_MemCopy(pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, pstLicenseDrm->aucBindedCamId, MW_PVR_CAMID_LEN);
								if (pstLicenseDrm->usProgramNum == pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum)
								{
									pstLicenseDrm->usLicenseLength = pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength;
									HxSTD_MemSet(pstLicenseDrm->aucLicenseData, 0x00, META_MAX_LICENSE_DRM_LENGTH);
									HxSTD_MemCopy(pstLicenseDrm->aucLicenseData, pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData, sizeof(HUINT8) * pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength);
									HxSTD_MemSet(pstFileDrm, 0x00, sizeof(svcMetaDrmInfo_t));
									HxSTD_MemCopy(pstFileDrm.stDrm, pstInfo, sizeof(DxCopyrightControl_t));

									HLIB_RWSTREAM_Print(sd, "{");
									svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "Pvr_TimeStamp_t");
									HLIB_RWSTREAM_Print(sd, ", ");
									svc_meta_encodePVR_MetaContent_DrmInfo(sd, pstFileDrm, "stFileDrm");
									HLIB_RWSTREAM_Print(sd, ", ");
									local_Meta_encodePVR_MetaContent_LicenseDrmInfo(sd, pstLicenseDrm, "stLicenseDrm");

									HLIB_RWSTREAM_Print(sd, "}");
									newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
									jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
									HLIB_STD_MemFree(newJsonBuf);

									if (NULL == jsonNewTimeStamp)
									{
										HLIB_JSON_Delete(json);
										HxLOG_Assert(jsonNewTimeStamp);
										goto END_FUNC;
									}

									HxJSON_Array_Set(jsonTimeStampList, ulMetaIndex, jsonNewTimeStamp);
									timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);
								}
							}
							else
							{
								HxLOG_Error("HxJSON_Object_Get() : stLicenseDrm ERROR!!\n");
							}
						}
#endif
					}
				}
			}
			break;

		case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
			break;
		case eDxCopyrightControl_CiPlusDrmType_RecCtrl:
			break;
		default:
				HxLOG_Warning("DRM Type[%02x]!!\n", pstInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType);
			break;
		}
	}
#endif

	HLIB_JSON_Delete(json);

	HxLOG_Info("timestamp =(%s)\n", timeStampBuf);

	if (timeStampBuf)
	{
		ulRet = svc_meta_WriteTimestamp(szFileName, timeStampBuf);
		HLIB_RWSTREAM_MemFree(timeStampBuf);
	}

	if(ulRet != ERR_OK)
	{
		HxLOG_Error("Update Drm Error : ulRet:0x%X, %s\n", ulRet, szFileName);
	}

	hResult = ulRet;

END_FUNC:
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	if (NULL != pstFileDrm)					{ OxMW_Free(pstFileDrm); }
#endif
#ifdef USE_LICENSE_DATA_JSON_STRING
	if (NULL != pstLicenseDrm)				{ OxMW_Free(pstLicenseDrm); }
#endif
	if (NULL != szFileName)					{ OxMW_Free(szFileName); }

	return hResult;
}


STATIC void	svc_meta_DecryptFile (const HCHAR *szFile)
{
	HxJSON_t	json;
	HCHAR		fname[512];
	HCHAR		*ext;
	FILE		*fp;
	HCHAR		*enc;

	HLIB_CMD_Printf("\tdecrypting \"%s\"...\n", szFile);

	json = HLIB_JSON_DecodeFile(szFile);
	if (!json)
	{
		HLIB_CMD_Printf(HxANSI_COLOR_RED("\t\tFail")": Cannot decode json file!\n");
		return;
	}
	ext = strrchr(szFile, '.');
	if (ext)
		ext++;
	else
		ext = "hjm";

	enc = DxRECLISTDATA_Decrypt(json, ext);
	HLIB_JSON_Delete(json);

	if (enc == NULL)
	{
		HLIB_CMD_Printf(HxANSI_COLOR_RED("\t\tFail")": decrypt fail!\n");
		return;
	}
	HxSTD_PrintToStrN(fname, 512, "%s.decrypted.json", szFile);
	fp = fopen(fname, "w");
	if (fp == NULL)
	{
		HLIB_CMD_Printf(HxANSI_COLOR_RED("\t\tFail")": failed to open file(%s)\n", fname);
		HLIB_STD_MemFree(enc);
		return;
	}
	fprintf(fp, "%s", enc);
	fclose(fp);
	HLIB_STD_MemFree(enc);
	HLIB_CMD_Printf(HxANSI_COLOR_GREEN("\t\tOK")": \"%s\"\n", fname);
}

STATIC void	svc_meta_DecryptDir (const HCHAR *szPath)
{
	HCHAR	path[512];
	HxDIR_t *dir;

	HLIB_CMD_Printf(HxANSI_COLOR_YELLOW("    decrypt dir: \"%s\"\n"), szPath);
	dir = HLIB_DIR_Open(szPath);
	if (dir)
	{
		do
		{
			HCHAR *name = HLIB_DIR_GetName(dir);
			HCHAR *ext	= strrchr(name, '.');
			if (ext)
				ext++;
			if (HLIB_DIR_IsFile(dir)
				&& ext
				&& (HLIB_STD_StrCaseCmp(ext, "hjm") == 0 || HLIB_STD_StrCaseCmp(ext, "hjtsm") == 0 || HLIB_STD_StrCaseCmp(ext, "hmt") == 0)
			) {
				HxSTD_PrintToStrN(path, 512, "%s/%s", HLIB_DIR_GetPath(dir), name);
				svc_meta_DecryptFile(path);
			}
		} while (HLIB_DIR_Next(dir) == ERR_OK);
		HLIB_DIR_Close(dir);
	}
}

STATIC HUINT32	svc_meta_GetBookmarkMaxNum(void)
{
#if (!defined(CONFIG_MW_MM_AUTO_BOOKMARK) )
	return 4;
#else
	return 32;
#endif
}

#define __RES__
STATIC svcMetaResAction_t *svc_meta_FindPvrAction (Handle_t hAction, HUINT32 ulContentId, SvcMeta_UsingActionType_e ePvrActionType)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstMetaResAction_List; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcMetaResAction_t		*pstAct = (svcMetaResAction_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstAct) &&
			(pstAct->hAction == hAction) &&
			(pstAct->ulContentId == ulContentId) &&
			(pstAct->ePvrActionType == ePvrActionType))
		{
			return pstAct;
		}
	}

	return NULL;
}

STATIC svcMetaResAction_t *svc_meta_FindPvrActionByContentId (HUINT32 ulContentId, SvcMeta_UsingActionType_e ePvrActionType)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstMetaResAction_List; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcMetaResAction_t		*pstAct = (svcMetaResAction_t *)HLIB_LIST_Data (pstListItem);
#if 0
		if( eSvcMeta_UsingActType_None == ePvrActionType )
			ePvrActionType = pstAct->ePvrActionType; // Find condition 'ignore ActionType'
#endif
		if ((NULL != pstAct) &&
			(pstAct->ulContentId == ulContentId) &&
			((pstAct->ePvrActionType == ePvrActionType) || (eSvcMeta_UsingActType_None == ePvrActionType)) )
		{
			return pstAct;
		}
	}

	return NULL;
}


STATIC svcMetaResAction_t *svc_meta_FindPvrActionByhAction (Handle_t hAction, SvcMeta_UsingActionType_e ePvrActionType)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstMetaResAction_List; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		svcMetaResAction_t		*pstAct = (svcMetaResAction_t *)HLIB_LIST_Data (pstListItem);
#if 0
		if( eSvcMeta_UsingActType_None == ePvrActionType )
			ePvrActionType = pstAct->ePvrActionType; // Find condition 'ignore ActionType'
#endif
		if ((NULL != pstAct) &&
			(pstAct->hAction == hAction) &&
			((pstAct->ePvrActionType == ePvrActionType) || (eSvcMeta_UsingActType_None == ePvrActionType)) )
		{
			return pstAct;
		}
	}

	return NULL;
}

#define __DEBUG_CMD__
STATIC int	cmd_meta_PrintList (void *szArgs)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	svcMetaMgr_t		*mgr = svc_meta_GetManager();

	if (NULL != mgr)
	{
		HxHASH_TableIter_t	iter;
		SvcMeta_Contents_t	*content;

		HLIB_HASH_IterInit(&iter, mgr->urlMap);
		while (HLIB_HASH_IterNext(&iter, NULL, (void **)&content))
		{
			HLIB_CMD_Printf("\t%d.%s \'%s%s\'\n"
				, content->ulContentsId, content->szURL, content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName);
		}

		svc_meta_ReleaseManager(mgr);
	}

	return HxCMD_OK;
}
#else
{
	svcMetaMgr_t			*mgr = svc_meta_GetManager();

	if (NULL != mgr)
	{
		HxHASH_TableIter_t		 iter;
		SvcMeta_Contents_t		*pstContent;
		SvcMeta_BriefContents_t *pstBriefContent;

		HLIB_HASH_IterInit(&iter, mgr->urlMap);
		while (HLIB_HASH_IterNext(&iter, NULL, (void **)&pstBriefContent))
		{
			switch(pstBriefContent->eObjectType)
			{
			case eSVC_META_CONTENTS_OBJTYPE_FullSize:
				pstContent = (SvcMeta_Contents_t *)pstBriefContent;
				HLIB_CMD_Printf("\t(FULL Data)	%d.%s \'%s%s\'\n"
					, pstContent->ulContentsId, pstContent->szURL, pstContent->stRecordInfo.aucPath, pstContent->stRecordInfo.aucFileName);
				break;

			case eSVC_META_CONTENTS_OBJTYPE_Brief:
				HLIB_CMD_Printf("\t(Brief Data) %d.%s \'%s\'\n"
					, pstBriefContent->ulContentsId, pstBriefContent->pszUrl, pstBriefContent->pszHjmPath);
				break;

			default:
				HLIB_CMD_Printf("\tUnknown Type\n");
				break;
			}

		}

		svc_meta_ReleaseManager(mgr);
	}

	return HxCMD_OK;
}
#endif
}

#define	____SVC_META_API_________________________________________________________________________

void		SVC_META_MuteEvent (HBOOL mute)
{
	svcMetaMgr_t *mgr;

	HxLOG_Trace();
	HxLOG_Debug("%s(%s)\n", __FUNCTION__, mute ? "MUTE" : "UNMUTE");

	mgr = svc_meta_GetManager();
	mgr->muteEvent = mute;
	svc_meta_ReleaseManager(mgr);
}

HBOOL		SVC_META_IsTsrContents (HUINT32 ulContentsId)
{
	SvcMeta_Record_t	stRec;

	HxLOG_Trace();
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, ulContentsId);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) == ERR_OK)
	{
		HUINT32	i;
		HCHAR	szPath[HxPATH_MAX];

		if( SVC_FS_UseTsrOnlyDevice() == TRUE )
		{
			if( SVC_FS_GetTsrOnlyPath(szPath) == ERR_OK )
			{
				if (HLIB_STD_StrStartWith(stRec.aucPath, szPath))
					return TRUE;
			}
		}

		for(i = 0 ; i < MAX_CONNECTABLE_PVR_STORAGE_NUM ; i++)
		{
			if (SVC_FS_GetPvrPath (i, eSVC_FS_UsageType_Tsr, szPath) != ERR_OK)
				continue;
			if (HLIB_STD_StrStartWith(stRec.aucPath, szPath))
				return TRUE;
		}
	}
	return FALSE;
}

HERROR		SVC_META_AddContentsInfo(Handle_t hSvc, SvcMeta_Record_t *pstRecordInfo, HUINT32 *pulContentsId)
{
	svcMetaMgr_t *mgr;
	HERROR			err;
	HUINT32			id = 0;

	HxLOG_Trace();
	HxLOG_Assert(hSvc && pstRecordInfo);

	mgr = svc_meta_GetManager();
	err = svc_meta_AddContentsInfo(mgr, hSvc, pstRecordInfo, &id);
	if (err == ERR_OK)
	{
		if (pulContentsId)
		{
			*pulContentsId = id;
		}

		svc_meta_EmitEvent(mgr, eSEVT_META_ADDED, 0, (HINT32)id, (HINT32)HLIB_HASH_Size(mgr->urlMap), 1);
	}

	svc_meta_ReleaseManager(mgr);

	return err;
}


HERROR		SVC_META_GetServiceInfo( HUINT32 ulContentsId, SvcMeta_Service_t *pstInfo )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	if(ulContentsId == 0 || pstInfo == NULL)
	{
		HxLOG_Error("Invalid Arguments!!  ulContentsId( %d ), pstInfo( %x )\n", ulContentsId, pstInfo);
		return ERR_FAIL;
	}

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	HxSTD_memcpy(pstInfo, &content->stServiceInfo, sizeof(SvcMeta_Service_t));
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_SetServiceInfo( HUINT32 ulContentsId, SvcMeta_Service_t *pstInfo, HBOOL bSave )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	if(ulContentsId == 0 || pstInfo == NULL)
	{
		HxLOG_Error("Invalid Arguments!!  ulContentsId( %d ), pstInfo( %x )\n", ulContentsId, pstInfo);
		return ERR_FAIL;
	}

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	content->bHasService = TRUE;
	if (HxSTD_memcmp(&content->stServiceInfo, pstInfo, sizeof(SvcMeta_Service_t)) != 0)
	{
		HxSTD_memcpy(&content->stServiceInfo, pstInfo, sizeof(SvcMeta_Service_t));
		if (bSave)
		{
			svc_meta_UpdateContents(mgr, content);
		}
		svc_meta_EmitEvent(mgr, eSEVT_META_UPDATED, 0, (HINT32)content->ulContentsId, 1, 0);
	}
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_GetEventList( HUINT32 ulContentsId, SvcMeta_Event_t **ppstInfo, SvcMeta_AllocFunc cbAlloc )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;
	HERROR				err;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && ppstInfo && cbAlloc);

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	err = svc_meta_GetEventList(content, ppstInfo, cbAlloc);
	svc_meta_ReleaseManager(mgr);

	return err;
}


HERROR		SVC_META_GetEventInfo( HUINT32 ulContentsId, SvcMeta_Event_t *pstInfo )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstInfo);

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	HxSTD_memcpy(pstInfo, &content->stEventInfo, sizeof(SvcMeta_Event_t));
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_SetEventInfo( HUINT32 ulContentsId, SvcMeta_Event_t *pstInfo, HBOOL bSave)
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstInfo);

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}

	content->bHasEvent = TRUE;
	if (svc_meta_ComparePvrEvent(&content->stEventInfo, pstInfo) != 0)
	{
		if (content->stEventInfo.pstExtEvt)
		{
			svc_meta_FreeExtEvt(content->stEventInfo.pstExtEvt);
		}
		HxSTD_memcpy(&content->stEventInfo, pstInfo, sizeof(SvcMeta_Event_t));
		if (pstInfo->pstExtEvt)
		{
			content->stEventInfo.pstExtEvt = svc_meta_CloneExtEvt(pstInfo->pstExtEvt);
		}
		if (bSave)
		{
			svc_meta_UpdateContents(mgr, content);
		}
		svc_meta_EmitEvent(mgr, eSEVT_META_UPDATED, 0, (HINT32)content->ulContentsId, 1, 0);
	}
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}

HERROR		SVC_META_IsEncrypt( HUINT8 *pucFileName, HBOOL  *pbEncrypt )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(pucFileName && pbEncrypt);

	*pbEncrypt = FALSE;

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByPath(mgr, (HCHAR *)pucFileName);
	if (content == NULL)
	{
		HxLOG_Error("%s(path:%s) Cannot find contents!\n", __FUNCTION__, pucFileName);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}

	if(content->stRecordInfo.stDrmInfo.bDrmCc == TRUE)
	{

	}
	else if(content->stRecordInfo.stDrmInfo.bCasCc == TRUE)
	{
		switch( content->stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType)
		{
			case eDxCopyrightControl_CasCcType_None:
				break;

			case eDxCopyrightControl_CasCcType_NagraSes:
				// 이 함수를 call 하는 mgr이 없다. 만약 필요하게 된다면 mgr 단에서 control byte를 가지고 get 할수 있다. xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes() 참고
//				*pbEncrypt = content->stRecordInfo.stDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.bStorageEncrypt;
				break;

			default:
				*pbEncrypt = FALSE;
				HxLOG_Error("unknow DRM Type :0x%X\n", content->stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType);
				break;
		}

	}
	else if(content->stRecordInfo.stDrmInfo.bSiCc == TRUE)
	{
		switch( content->stRecordInfo.stDrmInfo.stSiCcInfo.eSiCcType)
		{
			case eDxCopyrightControl_SiCcType_None:
				break;

			case eDxCopyrightControl_SiCcType_Ses:
				// 이 함수를 call 하는 mgr이 없다. 만약 필요하게 된다면 mgr 단에서 control byte를 가지고 get 할수 있다. xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes() 참고
//				*pbEncrypt = content->stRecordInfo.stDrmInfo.stSiCcInfo.info.stSesDrmInfo.bStorageEncrypt;
				break;

			default:
				*pbEncrypt = FALSE;
				HxLOG_Error("unknow DRM Type :0x%X\n", content->stRecordInfo.stDrmInfo.stSiCcInfo.eSiCcType);
				break;
		}

	}
#if defined(CONFIG_PROD_FVP4000T)
	/* Usually UK DTT doesn't have any DRM, but consider one of CI+'s in near future */
	if (content->stRecordInfo.stDrmInfo.stDrmCcInfo.eDrmCcType == eDxCopyrightControl_DrmCcType_None)
	{
		*pbEncrypt = (HBOOL)content->stRecordInfo.u1cEncryption;
		HxLOG_Error(" NO DRM, encryption:%d\n", content->stRecordInfo.u1cEncryption);
	}
	else
	{
		*pbEncrypt = FALSE;
		HxLOG_Error(" unknow DRM Type :0x%X\n", content->stRecordInfo.stDrmInfo.stDrmCcInfo.eDrmCcType);
	}
#endif


	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_GetDrmInfoByName( HUINT8 *pucFileName, HUINT8  *pucDrm, HUINT32 *pulExpired )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;
	UNIXTIME			time;

	HxLOG_Trace();
	HxLOG_Assert(pucFileName && pucDrm && pulExpired);

	*pulExpired = FALSE;

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByPath(mgr, (HCHAR *)pucFileName);
	if (content == NULL)
	{
		HxLOG_Error("%s(path:%s) Cannot find contents!\n", __FUNCTION__, pucFileName);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}

	if (svc_meta_IsExpired(&content->stRecordInfo, &time))
	{
		*pulExpired = TRUE;
	}
	HxSTD_memcpy( pucDrm, &content->stRecordInfo.stDrmInfo, sizeof(DxCopyrightControl_t) );

	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_ReadRecordInfo( HUINT8 *pucFileName, SvcMeta_Record_t *pstInfo )
{
	HUINT32	contentId;

	HxLOG_Trace();
	HxLOG_Assert(pucFileName && pstInfo);

	if (SVC_META_GetContentsIdFromFullname(&contentId, (HCHAR *)pucFileName) != ERR_OK)
		return ERR_FAIL;
	return SVC_META_GetRecordInfo(contentId, pstInfo);
}


HERROR          SVC_META_ReadContentInfo(HUINT8 *pucFileName, SvcMeta_Record_t *pstInfo, SvcMeta_Service_t *pstSvcInfo, SvcMeta_Event_t *pstEvtInfo )
{
	HUINT32	contentId;
	HERROR	err;

	HxLOG_Trace();
	HxLOG_Assert(pucFileName);

	if (SVC_META_GetContentsIdFromFullname(&contentId, (HCHAR *)pucFileName) != ERR_OK)
		return ERR_FAIL;

	err = ERR_OK;
	if (pstInfo)
		err |= SVC_META_GetRecordInfo(contentId, pstInfo);
	if (pstSvcInfo)
		err |= SVC_META_GetServiceInfo(contentId, pstSvcInfo);
	if (pstEvtInfo)
		err |= SVC_META_GetEventInfo(contentId, pstEvtInfo);
	return err;
}


HERROR 		SVC_META_WriteRecordInfo( HUINT8 *pucFileName, SvcMeta_Record_t *pstInfo )
{
	HUINT32	contentId;

	HxLOG_Trace();
	HxLOG_Assert(pucFileName);

	if (SVC_META_GetContentsIdFromFullname(&contentId, (HCHAR *)pucFileName) != ERR_OK)
		return ERR_FAIL;

	return SVC_META_SetRecordInfo(contentId, pstInfo);
}


HERROR		SVC_META_GetRecordUrl( HUINT32 ulContentsId, HCHAR *pszUrl, HUINT32	ulBufLen)
{
	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pszUrl && ulBufLen);

	if (OTL_HJM_GetURL(ulContentsId, pszUrl, ulBufLen) == NULL)
	{
		HxLOG_Error("%s(id:%u) cannot get URL by id!\n", __FUNCTION__, ulContentsId);
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR		SVC_META_GetRecordInfo( HUINT32 ulContentsId, SvcMeta_Record_t *pstInfo )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstInfo);

	mgr 	= svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	HxSTD_memcpy(pstInfo, &content->stRecordInfo, sizeof(SvcMeta_Record_t));
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_SetRecordInfo( HUINT32 ulContentsId, SvcMeta_Record_t *pstInfo)
{
	return SVC_META_SetRecordInfoWithOption(ulContentsId, pstInfo, TRUE, TRUE);
}


HERROR		SVC_META_SetRecordInfoWithOption( HUINT32 ulContentsId, SvcMeta_Record_t *pstInfo, HBOOL bSave, HBOOL bEmitEvent)
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstInfo);

	mgr 	= svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	content->bHasRecord = TRUE;
	if (HxSTD_memcmp(&content->stRecordInfo, pstInfo, sizeof(SvcMeta_Record_t)) != 0)
	{
		HxSTD_memcpy(&content->stRecordInfo, pstInfo, sizeof(SvcMeta_Record_t));
		if (bSave)
		{
			svc_meta_UpdateContents(mgr, content);
		}
		if (bEmitEvent)
		{
			svc_meta_EmitEvent(mgr, eSEVT_META_UPDATED, 0, (HINT32)content->ulContentsId, 1, 0);
		}
	}
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}

HERROR SVC_META_GetThumbnail( HUINT32 ulContentsId, HUINT32 **pulData )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;
	HCHAR				*path;
	HERROR				err;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	mgr 	= svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	path = svc_meta_MakePath(mgr, content, EXT_THUMBNAIL_FILE);
	err  = svc_meta_GetThumbnail(path, pulData);
	svc_meta_ReleaseManager(mgr);

	return err;
}


HERROR SVC_META_ReleaseThumbnail( HUINT32 *pulData )
{
	if (pulData)
		HLIB_STD_MemFree( pulData );
	return ERR_OK;
}


HERROR		SVC_META_GetDRMInfo( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo )
{
	SvcMeta_Record_t	stRec;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstInfo);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}
	HxSTD_memcpy(pstInfo, &stRec.stDrmInfo, sizeof(DxCopyrightControl_t));
	return ERR_OK;
}


HERROR		SVC_META_SetDRMInfo( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo, HBOOL bSave )
{
	SvcMeta_Record_t	stRec;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstInfo);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}
	HxSTD_memcpy(&stRec.stDrmInfo, pstInfo, sizeof(DxCopyrightControl_t));
	return SVC_META_SetRecordInfoWithOption(ulContentsId, &stRec, bSave, TRUE);
}



HERROR 		SVC_META_CheckExpiredByRecordInfo( SvcMeta_Record_t *pstRecInfo, HBOOL *pbExpired, UNIXTIME *pulExpiredTime )
{
	HxLOG_Trace();

	*pbExpired = svc_meta_IsExpired(pstRecInfo, pulExpiredTime);

	return ERR_OK;
}



HERROR		SVC_META_DeleteContents( HUINT32 ulContentsId )
{
	svcMetaMgr_t 	*mgr;
	HERROR			err;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	mgr     = svc_meta_GetManager();
	err = svc_meta_RemoveContentsInfo(mgr, ulContentsId);
	if (err == ERR_OK)
		svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, (HINT32)ulContentsId, 1, 0);
	svc_meta_ReleaseManager(mgr);

	return err;
}


HBOOL		SVC_META_IsExistTSFileContents( HUINT32 ulContentsId )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;
	HxVector_t			*list;
	HINT32				count = 0;
	HINT32				i, n;
	HCHAR				*path;
	STATIC const HCHAR	*s_checkExt[] = {EXT_TS_FILE, EXT_INDEX_FILE, EXT_META_FILE};

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	mgr 	= svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%d) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return FALSE;
	}

	list    = svc_meta_FindFiles(mgr, content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName, FALSE);
	if (list == NULL)
	{
		HxLOG_Error("%s(id:%d) There is no file(%s, %s)!!\n", __FUNCTION__, ulContentsId
						, content->stRecordInfo.aucPath, content->stRecordInfo.aucFileName);
		svc_meta_ReleaseManager(mgr);
		return FALSE;
	}
	n = sizeof(s_checkExt) / sizeof(s_checkExt[0]);
	HxVECTOR_EACH(list, HCHAR *, path, {
		for (i = 0 ; i < n ; i++)
		{
			if (HLIB_STD_StrEndWith(path, s_checkExt[i]))
			{
				count++;
				break;
			}
		}
	});
	HLIB_VECTOR_Delete(list);
	svc_meta_ReleaseManager(mgr);

	return count == n ? TRUE : FALSE;
}


HERROR		SVC_META_GetLastPlayTime( HUINT32 ulContentsId, HUINT32 *pulLastPlayTime	)
{
	SvcMeta_Record_t	stRec;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pulLastPlayTime);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}
	*pulLastPlayTime = stRec.ulLastWatchtime;
	return ERR_OK;
}

HERROR		SVC_META_SetLastPlayTime( HUINT32 ulContentsId, HUINT32 ulLastPlayTime )
{
	SvcMeta_Record_t	stRec;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}
	stRec.ulLastWatchtime = ulLastPlayTime;
	return SVC_META_SetRecordInfo(ulContentsId, &stRec);
}

HERROR		SVC_META_AddBookmark( HUINT32 ulContentsId, HCHAR *szName, HUINT32 ulPosition, HBOOL bUseUpdateEvent)
{
	SvcMeta_Record_t		stRec;
	SvcMeta_Bookmark_t	*bookmark;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && szName);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (stRec.usNumOfBookmark == svc_meta_GetBookmarkMaxNum())
	{
		HxLOG_Error("bookmark overflow!\n");
		return ERR_FAIL;
	}

	bookmark = stRec.astBookmarks + stRec.usNumOfBookmark++;
	HLIB_STD_StrNCpySafe(bookmark->szName, szName, SVC_PVR_BOOKMARK_NAME_LEN);
	bookmark->ulPos = ulPosition;

	qsort(stRec.astBookmarks, stRec.usNumOfBookmark, sizeof(SvcMeta_Bookmark_t), (void *)svc_meta_CompareBookmark);

	return SVC_META_SetRecordInfoWithOption(ulContentsId, &stRec, TRUE, bUseUpdateEvent);
}

HERROR		SVC_META_AddPausedPoint( HUINT32 ulContentsId, HCHAR *szName, HUINT32 ulPosition, HBOOL bUseUpdateEvent)
{
	SvcMeta_Record_t		stRec;
	SvcMeta_Bookmark_t	*bookmark;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && szName);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (stRec.usNumOfPausedPoints == svc_meta_GetBookmarkMaxNum())
	{
		HxLOG_Error("bookmark overflow!\n");
		return ERR_FAIL;
	}

	bookmark = stRec.astPausedPoints + stRec.usNumOfPausedPoints++;
	HLIB_STD_StrNCpySafe(bookmark->szName, szName, SVC_PVR_BOOKMARK_NAME_LEN);
	bookmark->ulPos = ulPosition;

	qsort(stRec.astPausedPoints, stRec.usNumOfPausedPoints, sizeof(SvcMeta_Bookmark_t), (void *)svc_meta_CompareBookmark);

	return SVC_META_SetRecordInfoWithOption(ulContentsId, &stRec, TRUE, bUseUpdateEvent);
}


HERROR		SVC_META_GetBookmarks( HUINT32 ulContentsId, HUINT16 *pulNumOfBookmark, SvcMeta_Bookmark_t **ppstBookmarks)
{
	SvcMeta_Record_t		stRec;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (pulNumOfBookmark)
	{
		*pulNumOfBookmark = stRec.usNumOfBookmark;
	}

	if (ppstBookmarks && stRec.usNumOfBookmark)
	{
		*ppstBookmarks = (SvcMeta_Bookmark_t *)HLIB_STD_MemDup(stRec.astBookmarks, sizeof(SvcMeta_Bookmark_t) * stRec.usNumOfBookmark);
		if (*ppstBookmarks == NULL)
		{
			if (pulNumOfBookmark)
				*pulNumOfBookmark = 0;
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}


HERROR		SVC_META_GetPausedPoints( HUINT32 ulContentsId, HUINT16 *pulNumOfPausedPoints, SvcMeta_Bookmark_t **ppstPausedPoints)
{
	SvcMeta_Record_t		stRec;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (pulNumOfPausedPoints)
	{
		*pulNumOfPausedPoints = stRec.usNumOfPausedPoints;
	}

	if (ppstPausedPoints && stRec.usNumOfPausedPoints)
	{
		*ppstPausedPoints = (SvcMeta_Bookmark_t *)HLIB_STD_MemDup(stRec.astPausedPoints, sizeof(SvcMeta_Bookmark_t) * stRec.usNumOfPausedPoints);
		if (*ppstPausedPoints == NULL)
		{
			if (pulNumOfPausedPoints)
				*pulNumOfPausedPoints = 0;
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}

void		SVC_META_FreeBookmarks (SvcMeta_Bookmark_t *pstBookmarks)
{
	if (pstBookmarks)
		HLIB_STD_MemFree(pstBookmarks);
}

HERROR		SVC_META_RemoveBookmark( HUINT32 ulContentsId, SvcMeta_Bookmark_t	*pstBookmark, HBOOL bUseUpdateEvent)
{
	SvcMeta_Record_t		stRec;
	SvcMeta_Bookmark_t	*bookmark;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && pstBookmark);

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (stRec.usNumOfBookmark == 0)
	{
		HxLOG_Warning("id:%u There is no bookmark!\n", ulContentsId);
		return ERR_FAIL;
	}

	bookmark = (SvcMeta_Bookmark_t *)bsearch(pstBookmark, stRec.astBookmarks, stRec.usNumOfBookmark
									, sizeof(SvcMeta_Bookmark_t), (void *)svc_meta_CompareBookmark);
	if (bookmark == NULL)
	{
		HxLOG_Error("[%s:%d] There is no bookmark matched with (pos:%u, name:%s)!\n"
						, __FUNCTION__, __LINE__, pstBookmark->ulPos, pstBookmark->szName);
		return ERR_FAIL;
	}
	bookmark->ulPos     = 0xFFFFFFFF;
	bookmark->szName[0] = '\0';

	if (stRec.usNumOfBookmark > 1)
		qsort(stRec.astBookmarks, stRec.usNumOfBookmark, sizeof(SvcMeta_Bookmark_t), (void *)svc_meta_CompareBookmark);
	stRec.usNumOfBookmark--;

	return SVC_META_SetRecordInfoWithOption(ulContentsId, &stRec, TRUE, bUseUpdateEvent);
}

SvcMeta_ExtEvt_t *	SVC_META_CloneExtEvt(const SvcMeta_ExtEvt_t *pstSrc)
{
	return svc_meta_CloneExtEvt(pstSrc);
}

void		SVC_META_FreeExtEvt(SvcMeta_ExtEvt_t *pstExtEvt)
{
	svc_meta_FreeExtEvt(pstExtEvt);
}


HERROR		SVC_META_LoadContentsInfo(const HCHAR *szPath)
{
	svcMetaMgr_t *mgr;
	HINT32			ret;

	HxLOG_Trace();
	HxLOG_Assert(szPath);

	mgr = svc_meta_GetManager();
	ret = svc_meta_Load(mgr, szPath);
	if (ret > 0)
	{
		svc_meta_EmitEvent(mgr, eSEVT_META_ADDED, 0, 0, HLIB_HASH_Size(mgr->urlMap), ret);
	}
	svc_meta_ReleaseManager(mgr);
	return ret > 0 ? ERR_OK : ERR_FAIL;
}


HERROR		SVC_META_UnloadContentsInfo( void )
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	svcMetaMgr_t 	*mgr;
	HINT32				removed;
	HxHASH_TableIter_t	iter;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();

	removed = 0;
	mgr = svc_meta_GetManager();
	HLIB_HASH_IterInit(&iter, mgr->urlMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&content))
	{
		if (content->bDeleteSkip)
			continue;
		HLIB_HASH_IterRemove(&iter);
		removed++;
	}
	if (removed > 0)
		svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, 0, removed, 0);
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}
#else
{
	svcMetaMgr_t 	*mgr;
	HINT32				removed;
	HxHASH_TableIter_t	iter;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();

	removed = 0;
	mgr = svc_meta_GetManager();
	HLIB_HASH_IterInit(&iter, mgr->urlMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&content))
	{
		if (eSVC_META_CONTENTS_OBJTYPE_FullSize == content->eObjectType)
		{
			if (content->bDeleteSkip)
				continue;
			mgr->fullContentCache = HLIB_LIST_Remove(mgr->fullContentCache, (const void *)content);
		}

		HLIB_HASH_IterRemove(&iter);
		removed++;
	}
	if (removed > 0)
		svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, 0, removed, 0);
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}
#endif
}

HERROR		SVC_META_UnloadAllContentsInfo( void )
{
	svcMetaMgr_t *mgr;
	HINT32			removed = 0;

	HxLOG_Trace();

	mgr = svc_meta_GetManager();
	removed = HLIB_HASH_Size(mgr->urlMap);
	HLIB_HASH_RemoveAll(mgr->urlMap);
#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	mgr->fullContentCache = HLIB_LIST_RemoveAll(mgr->fullContentCache);
#endif
	if (removed > 0)
		svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, 0, removed, 0);
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_UnloadContentsInfoByPath(const HCHAR *szPath)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	svcMetaMgr_t 	*mgr;
	HINT32				removed;
	HxHASH_TableIter_t	iter;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(szPath);
	HxLOG_Debug("unload path (%s) \n", szPath);

	removed = 0;
	mgr = svc_meta_GetManager();
	HLIB_HASH_IterInit(&iter, mgr->urlMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&content))
	{
		if (!HLIB_STD_StrStartWith(content->stRecordInfo.aucPath, szPath))
			continue;
		HLIB_HASH_IterRemove(&iter);
		removed++;
	}
	if (removed > 0)
		svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, 0, removed, 0);
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}
#else
{
	svcMetaMgr_t 			*mgr;
	HINT32					removed;
	HxHASH_TableIter_t		iter;
	HCHAR					*pszRecPath;
	SvcMeta_Contents_t		*content;
	SvcMeta_BriefContents_t	*briefContent;

	HxLOG_Trace();
	HxLOG_Assert(szPath);
	HxLOG_Debug("unload path (%s) \n", szPath);

	removed = 0;
	mgr = svc_meta_GetManager();
	HLIB_HASH_IterInit(&iter, mgr->urlMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&content))
	{
		switch (content->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
			mgr->fullContentCache = HLIB_LIST_Remove(mgr->fullContentCache, (const void *)content);
			pszRecPath = content->stRecordInfo.aucPath;
			break;

		case eSVC_META_CONTENTS_OBJTYPE_Brief:
			briefContent = (SvcMeta_BriefContents_t *)content;
			pszRecPath = briefContent->pszHjmPath;
			break;

		default:
			pszRecPath = NULL;
			break;
		}

		if ((NULL != pszRecPath) && (!HLIB_STD_StrStartWith(pszRecPath, szPath)))
		{
			continue;
		}

		HLIB_HASH_IterRemove(&iter);
		removed++;
	}
	if (removed > 0)
		svc_meta_EmitEvent(mgr, eSEVT_META_REMOVED, 0, 0, removed, 0);
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}
#endif
}

HUINT64		SVC_META_GetRecycleBinSize (const HCHAR *szMountPath)
{
	HUINT64		ullRecycleBinSize = 0;
	HERROR		hError;
	HCHAR		*recycleBinPath = NULL;

	recycleBinPath= svc_meta_getRecycleBinFolder(szMountPath);
	if ( recycleBinPath== NULL)
	{
		HxLOG_Error("svc_meta_getRecycleBinFolder ERROR! mountPath[%s]\n",__FUNCTION__,__LINE__,szMountPath);
		goto END_FUNC;
	}

	if (HLIB_FILE_Exist(recycleBinPath) == FALSE)
	{
		HxLOG_Debug("recycle bin not exists. \n");
		goto END_FUNC;
	}

	hError = HLIB_STORAGE_GetPathSize(recycleBinPath, &ullRecycleBinSize);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Get Recycle Bin size failed. recycle bin path=[%s] \n",recycleBinPath);
		goto END_FUNC;
	}

	HxLOG_Debug("Recycle Bin Path[%s] size(%lld) \n",recycleBinPath,ullRecycleBinSize);

END_FUNC:
	if (recycleBinPath != NULL)
	{
		HLIB_STD_MemFree(recycleBinPath);
	}
	return ullRecycleBinSize;
}
HERROR		SVC_META_GetContentsNumber( const HCHAR *szPath, HUINT32 *pulNumber )
{
	HxPair_t	pair;

	HxLOG_Trace();
	HxLOG_Assert(szPath && pulNumber);

	pair.left  = NULL;
	pair.right = (void *)EXT_META_FILE;

	(void) HLIB_DIR_Find((HUINT8 *)szPath, NULL, "", svc_meta_CountFile, &pair);

	if (pair.left == NULL)
	{
		HxLOG_Error("%s(%s) no contents!\n", __FUNCTION__, szPath);
		return ERR_FAIL;
	}
	*pulNumber = (HUINT32)pair.left;
	return ERR_OK;
}

HERROR		SVC_META_CountContentsIdList (HUINT32 *pulNumber)
{
	svcMetaMgr_t 				*mgr;
	SvcMeta_Contents_t			*content;
	HUINT32						n;

	HxLOG_Trace();
	HxLOG_Assert(pulNumber);

	mgr = svc_meta_GetManager();
	if ((n = HLIB_HASH_Size(mgr->urlMap)) == 0)
	{
		svc_meta_ReleaseManager(mgr);
		HxLOG_Debug("%s() no contents!\n", __FUNCTION__);
		*pulNumber = 0;
		return ERR_OK;
	}

	n = 0;
	SVC_META_FOREACH(mgr, content, {
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
		n++;
#else
		switch (content->eObjectType)
		{
		case eSVC_META_CONTENTS_OBJTYPE_FullSize:
		case eSVC_META_CONTENTS_OBJTYPE_Brief:
			n++;
		default:
			break;
		}
#endif
	});

	*pulNumber = n;
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}



HERROR		SVC_META_GetContentsIdList( HUINT32 *pulNumber, HUINT32 **pulContentsList	)
{
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;
	HUINT32				n;

	HxLOG_Trace();

	if (NULL == pulNumber)
	{
		HxLOG_Error("pulNumber NULL:\n");
		return ERR_FAIL;
	}

	mgr = svc_meta_GetManager();
	if ((n = HLIB_HASH_Size(mgr->urlMap)) == 0)
	{
		svc_meta_ReleaseManager(mgr);
		HxLOG_Debug("%s() no contents!\n", __FUNCTION__);
		*pulNumber = 0;
		return ERR_OK;
	}
	if (pulContentsList)
	{
		*pulContentsList = (HUINT32 *)HLIB_STD_MemCalloc(sizeof(HUINT32) * n);
		if (*pulContentsList == NULL)
		{
			svc_meta_ReleaseManager(mgr);
			HxLOG_Error("%s() out of memory!!\n", __FUNCTION__);
			return ERR_FAIL;
		}

		n = 0;
		SVC_META_FOREACH(mgr, content, {
			(*pulContentsList)[n++] = content->ulContentsId;
		});
		*pulNumber = n;
	}
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}
#else
{
	svcMetaMgr_t 				*mgr;
	SvcMeta_Contents_t			*content;
	SvcMeta_BriefContents_t		*breifContent;
	HUINT32						n;

	HxLOG_Trace();

	if (NULL == pulNumber)
	{
		HxLOG_Error("pulNumber NULL:\n");
		return ERR_FAIL;
	}

	mgr = svc_meta_GetManager();
	if ((n = HLIB_HASH_Size(mgr->urlMap)) == 0)
	{
		svc_meta_ReleaseManager(mgr);
		HxLOG_Debug("%s() no contents!\n", __FUNCTION__);
		*pulNumber = 0;
		return ERR_OK;
	}

	if (pulContentsList)
	{
		*pulContentsList = (HUINT32 *)HLIB_STD_MemCalloc(sizeof(HUINT32) * n);
		if (*pulContentsList == NULL)
		{
			svc_meta_ReleaseManager(mgr);
			HxLOG_Error("%s() out of memory!!\n", __FUNCTION__);
			return ERR_FAIL;
		}

		n = 0;
		SVC_META_FOREACH(mgr, content, {
			switch (content->eObjectType)
			{
			case eSVC_META_CONTENTS_OBJTYPE_FullSize:
				(*pulContentsList)[n++] = content->ulContentsId;
				break;

			case eSVC_META_CONTENTS_OBJTYPE_Brief:
				breifContent = (SvcMeta_BriefContents_t *)content;
				(*pulContentsList)[n++] = breifContent->ulContentsId;
				break;

			default:
				break;
			}
		});
		*pulNumber = n;
	}
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}
#endif
}

HERROR		SVC_META_FreeContentsIdList( HUINT32 *pulContentsList )
{
	if( pulContentsList != NULL )
		HLIB_STD_MemFree( pulContentsList );

	return ERR_OK;
}

HERROR		SVC_META_GetContentsIdFromUrl( HUINT32 *pulContentsId, const HCHAR *pszUrl)
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(pulContentsId && pszUrl);

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContents(mgr, pszUrl);
	if (content == NULL)
	{
		HxLOG_Error("%s(url:%s) Cannot find contents!\n", __FUNCTION__, pszUrl);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	*pulContentsId = content->ulContentsId;
	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}


HERROR		SVC_META_GetContentsIdFromFullname( HUINT32 *pulContentsId, HCHAR *pszFullName)
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(pulContentsId && pszFullName);

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByPath(mgr, (HCHAR *)pszFullName);
	if (content == NULL)
	{
		HxLOG_Error("%s(path:%s) Cannot find contents!\n", __FUNCTION__, pszFullName);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	*pulContentsId = content->ulContentsId;

	svc_meta_ReleaseManager(mgr);

	return ERR_OK;
}



// 새로운 파일을 생성하기 위해서 기존 파일명과 겹치지 않는 파일명을 생성해낸다.
HERROR		SVC_META_MakeFileName( UNIXTIME ulTime, const HCHAR *pucSourceName, HCHAR *pucTargetName, HUINT32 n)
{
	svcMetaMgr_t *mgr;
	const HCHAR		*name;

	HxLOG_Trace();
	HxLOG_Assert(pucSourceName && pucTargetName && n);

	mgr  = svc_meta_GetManager();
	name = svc_meta_GetFilename(mgr, pucSourceName, ulTime, FALSE);
	if (name == NULL)
	{
		HxLOG_Error("%s(%s) Cannot get filename!\n", __FUNCTION__, pucSourceName);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	HLIB_STD_StrNCpySafe(pucTargetName, name, n);
	svc_meta_ReleaseManager(mgr);
	return ERR_OK;
}


HERROR		SVC_META_SetAsDeleteSkipContent ( HUINT32 ulContentsId, HBOOL bDeleteSkip )
{
	svcMetaMgr_t 	*mgr;
	SvcMeta_Contents_t	*content;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	mgr     = svc_meta_GetManager();
	content = svc_meta_GetContentsByID(mgr, ulContentsId);
	if (content == NULL)
	{
		HxLOG_Error("%s(id:%u) Cannot find contents!\n", __FUNCTION__, ulContentsId);
		svc_meta_ReleaseManager(mgr);
		return ERR_FAIL;
	}
	content->bDeleteSkip = bDeleteSkip;
	svc_meta_ReleaseManager(mgr);
	return ERR_OK;
}


HERROR		SVC_META_GetService( Handle_t hAct, SvcMeta_Service_t *pstInfo )
{
	HERROR		ulRet = ERR_OK;
	HUINT32		ulContentsId = 0;

	ulRet = SVC_META_GetContentIdByhAction( hAct, &ulContentsId );
	if( ulRet != ERR_OK )
		return ERR_FAIL;

	ulRet = SVC_META_GetServiceInfo( ulContentsId, pstInfo );

	return ulRet;
}

HERROR		SVC_META_MakeSvcInfoByMetaSvc(SvcMeta_Service_t * MetaSvc, DbSvc_Info_t * svc)
{
	if(MetaSvc == NULL || svc == NULL)
		return ERR_FAIL;

	if(svc_meta_MakeSvcInfoByMetaSvc(MetaSvc, svc) != NULL)
		return ERR_OK;
	else
		return ERR_FAIL;
}

HERROR		SVC_META_MakeMetaSvcBySvcInfo(DbSvc_Info_t * svc, SvcMeta_Service_t * MetaSvc)
{
	if(MetaSvc == NULL || svc == NULL)
		return ERR_FAIL;

	if(svc_meta_MakeMetaSvcByMetaSvcInfo(svc, MetaSvc) != NULL)
		return ERR_OK;
	else
		return ERR_FAIL;
}

HERROR		SVC_META_GetEvent( Handle_t hAct, SvcMeta_Event_t *pstInfo )
{
	HERROR		ulRet = ERR_OK;
	HUINT32		ulContentsId = 0;

	ulRet = SVC_META_GetContentIdByhAction( hAct, &ulContentsId );
	if( ulRet != ERR_OK )
		return ERR_FAIL;

	ulRet = SVC_META_GetEventInfo( ulContentsId, pstInfo );

	return ulRet;
}

HERROR		SVC_META_GetMediaExtEvent( HUINT32 ulContentsId, HINT32	nEvtId, SvcMeta_ExtEvt_t **ppstExtEvt )
{
	HERROR		ulRet = ERR_OK;
	SvcMeta_Event_t stPvrEvt;

	if( ppstExtEvt == NULL )
		return ERR_FAIL;

	ulRet = SVC_META_GetEventInfo( ulContentsId, &stPvrEvt );
	if( ulRet != ERR_OK )
		return ERR_FAIL;

	*ppstExtEvt = SVC_META_CloneExtEvt(stPvrEvt.pstExtEvt);
	if (*ppstExtEvt)
		return ERR_OK;
	return ERR_FAIL;
}


HERROR		SVC_META_CopyContents( HUINT32 ulContentsId, const HCHAR *szPath, HxReturnFunc_t cbDone, void *pvUserdata )
{
	svcMetaMgr_t	*mgr;
	HERROR			err;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && szPath);

	mgr = svc_meta_GetManager();
	err = svc_meta_CopyContents(mgr, ulContentsId, szPath, cbDone, pvUserdata);
	svc_meta_ReleaseManager(mgr);

	return err;
}

HERROR		SVC_META_MoveContents( HUINT32 ulContentsId, const HCHAR *szPath, HxReturnFunc_t cbDone, void *pvUserdata )
{
	svcMetaMgr_t	*mgr;
	HERROR			err;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId && szPath);

	mgr = svc_meta_GetManager();
	err = svc_meta_MoveContents(mgr, ulContentsId, szPath, cbDone, pvUserdata);
	svc_meta_ReleaseManager(mgr);

	return err;
}

HERROR		SVC_META_RenameContents( HUINT32 ulContentsId, const HCHAR *szPath, const HCHAR *szNewFileName, const HCHAR *szNewEventName)
{
	svcMetaMgr_t	*mgr;
	HERROR			err;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	mgr = svc_meta_GetManager();

	if (szPath && szNewFileName)
		err = svc_meta_RenameContents(mgr, ulContentsId, szPath, szNewFileName);
	else
		err = ERR_OK;

	svc_meta_ReleaseManager(mgr);

	if (err == ERR_OK)
	{
		SvcMeta_Record_t	stRec;

		err = SVC_META_GetRecordInfo(ulContentsId, &stRec);
		if (err == ERR_OK)
		{
			if (szPath && szNewFileName)
			{
				HLIB_STD_StrNCpySafe(stRec.aucPath, szPath, FS_MAX_FILE_NAME_LEN);
				HLIB_STD_StrNCpySafe(stRec.aucFileName, szNewFileName, FS_MAX_FILE_NAME_LEN);
			}

			if (szNewEventName)
			{
				HLIB_STD_StrNCpySafe(stRec.aucDisplayEvtName, szNewEventName, PVR_MAX_TITLE_LEN);
			}
			err = SVC_META_SetRecordInfo(ulContentsId, &stRec);
		}
	}
	return err;
}

HERROR		SVC_META_CancelCopyOperation (HUINT32 ulContentsId)
{
	svcMetaMgr_t	*mgr;
	svcMetaFOP_t	*fop;

	HxLOG_Trace();
	HxLOG_Assert(ulContentsId);

	mgr = svc_meta_GetManager();
	fop = svc_meta_CancelFileOp(mgr, ulContentsId);
	svc_meta_ReleaseManager(mgr);

	(void)(fop);

	return ERR_OK;
}

HERROR		SVC_META_CancelMoveOperation (HUINT32 ulContentsId)
{
	return SVC_META_CancelCopyOperation(ulContentsId);
}

STATIC HERROR svc_meta_ConvertExtEvtItemTextToUtf8(SvcMeta_ExtEvt_t *pstExtEvt)
{
	HUINT32		i, strIndex;
	HINT32		numItemBytes;
	HINT32		mergedStrLen;
	HUINT8		*pszTmpItemsText = NULL, *pszTmpUtfString;
	HINT32		copylen;

	/* UTF8으로 변환하기전 Item Num에 해당하는 Text를 합친다. */

	/* item의 Text Size 파악 */
	numItemBytes = 0;
	for (i = 0; i < pstExtEvt->ucItemNum; i++)
	{
		numItemBytes += HxSTD_StrLen(pstExtEvt->astItem[i].pszItem);
	}
	pszTmpItemsText = HLIB_STD_MemAlloc(numItemBytes + 1);	/* add 1 byte to insert NULL character */
	if (pszTmpItemsText == NULL)
	{
		return ERR_FAIL;
	}

	/* item Text를 하나의 String으로 합친다 */
	strIndex = 0;
	for (i = 0; i < pstExtEvt->ucItemNum; i++)
	{
		copylen = HxSTD_StrLen(pstExtEvt->astItem[i].pszItem);
		if( copylen != 0)
		{
			HxSTD_MemCopy(pszTmpItemsText + strIndex, pstExtEvt->astItem[i].pszItem, copylen);
			strIndex += copylen;
		}
	}
	pszTmpItemsText[strIndex] = '\0';

	if (strIndex != numItemBytes)
	{
		HxLOG_Critical("============= Error strIndex=%d numItemBytes=%d==========\n", strIndex, numItemBytes);
		HLIB_STD_MemFree(pszTmpItemsText);
		pszTmpItemsText = NULL;

		return ERR_FAIL;
	}

	/* 합쳐진 문자열을 UTF8으로 변환 한다. */
	pszTmpUtfString = NULL;
	mergedStrLen = MWC_UTIL_ConvSiStringToUtf8(pstExtEvt->szIso639LangCode, strIndex, pszTmpItemsText, &pszTmpUtfString);
	if (mergedStrLen == 0)
	{
		HLIB_STD_MemFree(pszTmpItemsText);
		pszTmpItemsText = NULL;

		if (pszTmpUtfString != NULL)
		{
			MW_MEM_Free(pszTmpUtfString);
			pszTmpUtfString = NULL;
		}

		return ERR_FAIL;
	}
	HLIB_STD_MemFree(pszTmpItemsText);
	pszTmpItemsText = NULL;

	HxLOG_Print("======================================\n");
	HxLOG_Print("Merged Text[%d][%d]=%s\n",numItemBytes, mergedStrLen, pszTmpUtfString);
	HxLOG_Print("======================================\n\n");

	/* 변환된 UTF8문자를 Item에 다시 넣는다. */
	pstExtEvt->ucItemNum = 0;
	for (i = 0; i < PVR_EXT_ITEM_MAX; i++)
	{
		SvcMeta_ExtEvtItem_t *pstItem = &pstExtEvt->astItem[i];

		if (pstItem->pszItem != NULL)
		{
			HLIB_STD_MemFree(pstItem->pszItem);
			pstItem->pszItem = NULL;
		}

		if (mergedStrLen > 0)
		{
			pstItem->pszItem = HLIB_STD_MemAlloc(PVR_EVENT_TEXT);
			HxSTD_MemSet(pstItem->pszItem, 0, PVR_EVENT_TEXT);
			HxSTD_MemCopy(pstItem->pszItem, pszTmpUtfString + (PVR_EVENT_TEXT - 1)*i, PVR_EVENT_TEXT - 1);
			pstItem->pszItem[(PVR_EVENT_TEXT - 1)] = '\0';
			mergedStrLen -= (PVR_EVENT_TEXT - 1);
			pstExtEvt->ucItemNum++;

			HxLOG_Print("Split Text[%d]=%s\n",i, pstItem->pszItem);
		}
	}

	if (pszTmpUtfString != NULL)
	{
		MW_MEM_Free(pszTmpUtfString);
		pszTmpUtfString = NULL;
	}

	return ERR_OK;
}

/* OCTO 2.0에서는 Descriptor Parsing시 문자열에 대해서 UTF8으로 변환 한다.
   하지만 일본향의 Extended Event Info Descriptor는 문자 코드 처리상 여러 Extended Event Info의 Descriptor가
   합쳐진 다음에 UTF8으로 변환 해야 올바른 변환이 수행된다. */
SvcMeta_ExtEvt_t *SVC_META_GetMergedExtEvtInfo(SvcMeta_ExtEvt_t *pstExtEvt)
{
	SvcMeta_ExtEvt_t	*pstTempExtEvt = NULL, *pstPrevExtEvt = NULL, *pstFreeExtEvt = NULL;
	HUINT32 		k, ulExtEvtCnt = 0;
	SvcMeta_ExtEvtItem_t *pstItem = NULL;
	HUINT8 *pstItemText = NULL;

	if (NULL == pstExtEvt)
	{
		HxLOG_Error("[%s, %d line parameter NULL !\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	/* Extended Event Descriptor의 갯수를 파악 */
	pstTempExtEvt = pstExtEvt;
	while (pstTempExtEvt)
	{
		ulExtEvtCnt++;
		pstTempExtEvt = pstTempExtEvt->pNext;
	}

	if (ulExtEvtCnt == 0)
	{
		HxLOG_Critical("[%s, %d line] none Ext event !\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	/* Parsing된 item에 대해 Description Text가 없는 item은 이전 ExtEvt Descriptor의 item에 연결 */
	pstTempExtEvt = pstExtEvt;
	while (pstTempExtEvt)
	{
		pstItem = &pstTempExtEvt->astItem[0];

		/* Arib에서 ucItemNum은 항상 1이다. */
		if (HxSTD_StrLen(pstItem->pszDescription) != 0)
		{
			pstPrevExtEvt = pstTempExtEvt;
		}
		else
		{
			if ((HxSTD_StrLen(pstItem->pszItem) != 0) && (pstPrevExtEvt != NULL) && (pstPrevExtEvt->ucItemNum < (PVR_EXT_ITEM_MAX - 1)))
			{
				pstItemText = HLIB_STD_StrDup(pstItem->pszItem);
				if (pstItemText != NULL)
				{
					if (pstPrevExtEvt->astItem[pstPrevExtEvt->ucItemNum].pszItem != NULL)
					{
						MW_MEM_Free(pstPrevExtEvt->astItem[pstPrevExtEvt->ucItemNum].pszItem);
					}
					pstPrevExtEvt->astItem[pstPrevExtEvt->ucItemNum].pszItem = pstItemText;
					pstPrevExtEvt->ucItemNum += 1;
					pstPrevExtEvt->pNext = pstTempExtEvt->pNext;
					pstFreeExtEvt = pstTempExtEvt;
				}
			}
		}

		pstTempExtEvt = pstTempExtEvt->pNext;

		/* 이전 ExtEvt에 item을 복사한 ExtEvt의 메모리 초기화 */
		if (pstFreeExtEvt != NULL)
		{
			if (pstFreeExtEvt->pszChar != NULL)
			{
				MW_MEM_Free(pstFreeExtEvt->pszChar);
				pstFreeExtEvt->pszChar = NULL;
			}

			for (k=0; k<pstFreeExtEvt->ucItemNum; k++)
			{
				if (pstFreeExtEvt->astItem[k].pszDescription != NULL)
				{
					MW_MEM_Free(pstFreeExtEvt->astItem[k].pszDescription);
					pstFreeExtEvt->astItem[k].pszDescription = NULL;
				}

				if (pstFreeExtEvt->astItem[k].pszItem != NULL)
				{
					MW_MEM_Free(pstFreeExtEvt->astItem[k].pszItem);
					pstFreeExtEvt->astItem[k].pszItem = NULL;
				}
			}

			MW_MEM_Free(pstFreeExtEvt);
			pstFreeExtEvt = NULL;
		}
	}

	/* ExtEvt Descriptor별로 item항목에 대해서 UTF8변환 */
	pstTempExtEvt = pstExtEvt;
	while (pstTempExtEvt)
	{
		svc_meta_ConvertExtEvtItemTextToUtf8(pstTempExtEvt);
		pstTempExtEvt = pstTempExtEvt->pNext;
	}

	return pstExtEvt;
}


void		SVC_META_PrintServiceInfo( const SvcMeta_Service_t *pstSvc )
{
	if( pstSvc == NULL ) return;

	HLIB_CMD_Printf("    <<Service Info>>\n", __FUNCTION__, __LINE__);
	HLIB_CMD_Printf("\tServiceName:%s\n", pstSvc->aucServiceName);
	HLIB_CMD_Printf("\tnChNum:%d, eSvcType:0x%X, eCasType:0x%X, \n\tusSvcId:0x%X, usTsId:0x%X, usOnId:0x%X, usPmtPid:0x%X\n",
				pstSvc->nChNumber, pstSvc->eSvcType, pstSvc->eCasType, pstSvc->usSvcId, pstSvc->usTsId, pstSvc->usOnId, pstSvc->usPmtPid );
	HLIB_CMD_Printf("\tusVideoPid:0x%X, eVideoCodec:0x%X, eVideoType:0x%X\n",
				pstSvc->usVideoPid, pstSvc->eVideoCodec, pstSvc->eVideoType );
	HLIB_CMD_Printf("\tusAudioPid:0x%X, eAudioCodec:0x%X, usAudioAuxPid:0x%X, eAudioAuxCodec:0x%X\n",
				pstSvc->usAudioPid, pstSvc->eAudioCodec, pstSvc->usAudioAuxPid, pstSvc->eAudioAuxCodec );
	HLIB_CMD_Printf("\tusDolbyPid:0x%X, eDolbyCodec:0x%X\n",
				pstSvc->usDolbyPid, pstSvc->eDolbyCodec );
}


#define __V2__
HERROR		SVC_META_SetEventTimeStamp( HUINT32 ulContentsId, HUINT32 ulTime, SvcMeta_Event_t *pstInfo )
{
	if( pstInfo == NULL ) return ERR_FAIL;
#ifdef DONT_USE_TIMESTAMP
	return ERR_FAIL;
#endif // DONT_USE_TIMESTAMP
	return ERR_OK;
}

HERROR		SVC_META_SetDrmTimeStamp( HUINT32 ulContentsId, HUINT32 ulTime, DxCopyrightControl_t *pstInfo )
{
#ifdef DONT_USE_TIMESTAMP
	return ERR_FAIL;
#else

	HERROR		ulRet = ERR_OK;

	if( pstInfo == NULL ) return ERR_FAIL;

	HxSEMT_Get( local_Meta_GetSem() );

	ulRet = svc_meta_SetDrmTimeStampToJMeta(ulContentsId, ulTime, pstInfo);

	HxSEMT_Release( local_Meta_GetSem() );

	return ulRet;
#endif // DONT_USE_TIMESTAMP
}

HERROR		SVC_META_UpdateDrmTimeStampByIndex( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo, HUINT32 ulMetaIndex )
{
#ifdef DONT_USE_TIMESTAMP
	return ERR_FAIL;
#else

	HERROR		ulRet = ERR_OK;

	if( pstInfo == NULL ) return ERR_FAIL;

	HxSEMT_Get( local_Meta_GetSem() );

	ulRet = svc_meta_UpdateDrmTimeStampToJMetaByIndex(ulContentsId, pstInfo, ulMetaIndex);

	HxSEMT_Release( local_Meta_GetSem() );

	return ulRet;
#endif // DONT_USE_TIMESTAMP
}

HERROR		SVC_META_SetServiceTimeStamp( HUINT32 ulContentsId, HUINT32 ulTime, SvcMeta_Service_t *pstInfo )
{
#ifdef DONT_USE_TIMESTAMP
	return ERR_FAIL;
#else

	HERROR		ulRet = ERR_OK;

	if( pstInfo == NULL ) return ERR_FAIL;

	HxSEMT_Get( local_Meta_GetSem() );

	ulRet = svc_meta_SetServiceTimeStampToJMeta(ulContentsId, ulTime, pstInfo);

	HxSEMT_Release( local_Meta_GetSem() );

	return ulRet;
#endif // DONT_USE_TIMESTAMP
}

HERROR		SVC_META_LoadTimeStamp( HUINT32 ulContentsId, SvcMeta_TimestampList_t *pstTimeStampList )
{
#ifdef DONT_USE_TIMESTAMP
	return ERR_FAIL;
#else

	HERROR		ulRet = ERR_OK;

	if( pstTimeStampList == NULL ) return ERR_FAIL;

	HxSEMT_Get( local_Meta_GetSem() );

	ulRet = svc_meta_LoadTimeStampListFromJMeta(ulContentsId, pstTimeStampList);

	HxSEMT_Release( local_Meta_GetSem() );

	return ulRet;
#endif // DONT_USE_TIMESTAMP
}

HERROR		SVC_META_UnloadTimeStamp( SvcMeta_TimestampList_t *pstList )
{
	HUINT32 i, ulNumber=0;
	SvcMeta_Event_t 		*pstEvt = NULL;
	SvcMeta_Service_t 		*pstService = NULL;
	DxCopyrightControl_t	*pstDrm = NULL;

#ifdef DONT_USE_TIMESTAMP
	return ERR_FAIL;
#endif // DONT_USE_TIMESTAMP

	if( pstList == NULL ) return ERR_FAIL;

	if( pstList->pstData == NULL )
	{
		pstList->ulNumber = 0;
		goto END_FUNC;
	}

	ulNumber = pstList->ulNumber;
	for( i=0; i<ulNumber; i++ )
	{
		switch( pstList->pstData[i].stTimeStamp.eType )
		{
			case eSvcMeta_Timestamp_PMT:
				pstService = pstList->pstData[i].pData;
				if(pstService != NULL)
				{
					HLIB_STD_MemFree( pstService );
					pstService = NULL;
				}
				break;

			case eSvcMeta_Timestamp_EIT:
				pstEvt = (SvcMeta_Event_t * )pstList->pstData[i].pData;
				if(pstEvt != NULL)
				{
					if (pstEvt->pstExtEvt != NULL)
					{
						SVC_META_FreeExtEvt( pstEvt->pstExtEvt );
						pstEvt->pstExtEvt = NULL;
					}
					HLIB_STD_MemFree( pstEvt );
					pstEvt = NULL;
				}
				break;

			case eSvcMeta_Timestamp_DRM:
				pstDrm = (DxCopyrightControl_t *) pstList->pstData[i].pData;

				if( pstDrm != NULL)
				{
					HLIB_STD_MemFree( pstDrm );
					pstDrm = NULL;
				}

				break;

#if 0 // defined(CONFIG_MW_CI_PLUS_CC_V_2)
			case eSvcMeta_Timestamp_LICENSE_DRM:
			{
				DxCopyrightControl_t *pstDrm;
				HxLOG_Info("Unload eSvcMeta_Timestamp_LICENSE_DRM\n");

				if( pstList->pstData[i].pData == NULL ) break;

				pstDrm = (DxCopyrightControl_t *) pstList->pstData[i].pData;
				if(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType != eDxCopyrightControl_CiPlusDrmType_License)
				{
					HxLOG_Critical("Critical Error... It's not License DRM data! Check it!!\n");
					break;
				}
				if(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData == NULL)
				{
					HxLOG_Critical("Critical Error... It's not License DRM data! Check it!!\n");
					break;
				}

				HLIB_STD_MemFree(pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData);
				HLIB_STD_MemFree(pstDrm);
				break;
			}
#endif
			default:
				break;
		}
	}

	HLIB_STD_MemFree( pstList->pstData );

END_FUNC:
	pstList->pstData = NULL;
	pstList->ulNumber = 0;
	pstList->ulRefTime = 0;
	pstList->ulPmtRefTime = PVR_INVALID_REF_TIME;
	pstList->ulEitRefTime = PVR_INVALID_REF_TIME;
	pstList->ulDrmRefTime = 0;
	pstList->ulDrmUriRefTime = 0;
	pstList->ulDrmLicenseRefTime = 0;

	return ERR_OK;
}

HERROR		SVC_META_SaveTimeStampTsrToDelayedRec( HUINT32 ulContentsId, UNIXTIME ulTimeoffset, HUINT8 *pucPathName)
{
	HERROR				ulRet = ERR_FAIL;
	SvcMeta_Record_t	stRec;
	HUINT8				aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	HBOOL				bExist = FALSE;

	if (SVC_META_GetRecordInfo(ulContentsId, &stRec) != ERR_OK)
	{
		HxLOG_Error("don't exist in meta list : %d\n", ulContentsId);
		return ERR_FAIL;
	}

	HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
	HxSTD_PrintToStrN(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
				stRec.aucPath,
				stRec.aucFileName, EXT_JMETA_TSMPFILE);

	HxSEMT_Get( local_Meta_GetSem() );

	bExist = HLIB_FILE_Exist( pucPathName );
	if (TRUE == bExist)
	{
		if (ulTimeoffset == 0) // offset 이 없으면 tsr timestamp 파일을 그대로 복사
		{
			ulRet = HLIB_FILE_Copy(pucPathName, aucFileName);
		}
		else // offset 이  있다면 보정해서 저장함.
		{
			HxJSON_t	json;
			HCHAR		*timeStampBuf = NULL;

			// read tsr timestamp
			json = svc_meta_ReadTimestamp(pucPathName);
			if (json == NULL)
			{
				HxLOG_Critical("(%s) HLIB_JSON_DecodeFile Fail!!!!!!\n", pucPathName);
				goto EXIT_SAVE_TS;
			}
			else
			{
				HxJSON_t	jsonTimeStampList = HxJSON_Object_Get(json, "TimeStampList");

				if (jsonTimeStampList)
				{
					HINT32	i;
					HUINT32	ulTimeStmpCnt = HLIB_JSON_GetSize(jsonTimeStampList);

					HxLOG_Info("ulTimeStmpCnt (%d) \n", ulTimeStmpCnt);

					for (i = 0; i < ulTimeStmpCnt; ++i)
					{
						HxJSON_t	jTimeSmp 	= HxJSON_Array_Get (jsonTimeStampList, i);
						HINT32		sd;

						sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
						if (!sd)
						{
							HxLOG_Warning("R/W Stream open fail!\n");
							goto EXIT_SAVE_TS;
						}

						if (jTimeSmp)
						{
							HxJSON_t	jTimeSmpPvrTimeStamp = HxJSON_Object_Get(jTimeSmp, "Pvr_TimeStamp_t");

							if (jTimeSmpPvrTimeStamp)
							{
								SvcMeta_Timestamp_t stTimeStamp;
								HCHAR				*newJsonBuf 	= NULL;
								HxJSON_t			jsonNewTimeStamp= NULL;

								stTimeStamp.eType	= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "eType");
								stTimeStamp.ulTime	= HLIB_JSON_Object_GetInteger(jTimeSmpPvrTimeStamp, "ulTime");

								// apply offset time
								stTimeStamp.ulTime = (stTimeStamp.ulTime > ulTimeoffset)? stTimeStamp.ulTime - ulTimeoffset : 1;

								HLIB_RWSTREAM_Print(sd, "{");
								svc_meta_encodePvrTimeStamp(sd, &stTimeStamp, "Pvr_TimeStamp_t");
								HLIB_RWSTREAM_Print(sd, "}");
								newJsonBuf = HLIB_RWSTREAM_CloseWithoutBuf(sd);

								if (newJsonBuf)
								{
									jsonNewTimeStamp = HLIB_JSON_Decode(newJsonBuf);
									HLIB_STD_MemFree(newJsonBuf);

									if (NULL == jsonNewTimeStamp)
									{
										HLIB_JSON_Delete(json);
										HxLOG_Assert(jsonNewTimeStamp);
										goto  EXIT_SAVE_TS;
									}

									HLIB_JSON_Object_Update(jTimeSmp, jsonNewTimeStamp);
								}
							}
						}
					}
				}	// end for
			}

			timeStampBuf = HLIB_JSON_Encode(json, NULL, 0);

			//HxLOG_Info("timestamp =(%s)\n", timeStampBuf);

			HLIB_JSON_Delete(json);

			ulRet = svc_meta_WriteTimestamp(aucFileName, timeStampBuf);
			if(ulRet != ERR_OK)
			{
				HxLOG_Error("save Error : ulRet:0x%X, %s\n", ulRet, aucFileName);
			}

			if (timeStampBuf)
				HLIB_RWSTREAM_MemFree(timeStampBuf);
		}
	}

EXIT_SAVE_TS:
	HxSEMT_Release( local_Meta_GetSem() );

	return ulRet;
}


HERROR		SVC_META_CheckDrmExpired(DxCopyrightControl_t *pstDrmInfo, UNIXTIME ulStartTime, HBOOL *pbExpired, UNIXTIME *pulExpiredTime )
{

	HERROR ulRet = ERR_OK;
	HxLOG_Trace();

	if( (pbExpired == NULL) || (pulExpiredTime == NULL) )
		return ERR_FAIL;

	*pbExpired = FALSE;
	*pulExpiredTime = 0;

	switch( pstDrmInfo->stCasCcInfo.eCasCcType )
	{
#if 0
		case eDxCopyrightControl_CasCcType_NagraSes:
			break;
#endif
		case eDxCopyrightControl_CasCcType_Conax:
#if defined(CONFIG_MW_CAS_CONAX) && defined(CONFIG_MW_CAS_CONAX_URI)
			ulRet = svc_meta_IsExpiredForConax(ulStartTime, pstDrmInfo, pbExpired, pulExpiredTime );
#endif
			break;
		default:
			HxLOG_Error("unknow DRM Type :0x%X\n", pstDrmInfo->stCasCcInfo.eCasCcType);
			return ERR_FAIL;
	}

	return ulRet;
}

void		SVC_META_Decrypt (const HCHAR *szPath)
{
#if !defined(WIN32)
	struct stat fstat;

	HLIB_CMD_Printf(HxANSI_COLOR_YELLOW("CMD: %s(%s)\n"), __FUNCTION__, szPath);
	if (HxSTD_StrEmpty(szPath) || HLIB_STD_StrCaseCmp(szPath, "help") == 0)
	{
		HLIB_CMD_Printf(
			HxANSI_COLOR_YELLOW("USAGE:\n")
			"\t1. $meta decrypt *\n"
			"\t\t- decrypt all meta file in attached pvr storage.\n"
			"\t2. $meta decrypt /mnt/hd2/Recordings\n"
			"\t\t- decrypt all meta file in dir.\n"
			"\t3. $meta decrypt /mnt/hd2/Recordings/Simpson.hjm\n"
			"\t\t- decrypt a meta file.\n"
		);
		return;
	}
	if (strcmp(szPath, "*") == 0)
	{
		HINT32		i;
		HCHAR		path[512];

		for (i = 0 ; i < MAX_CONNECTABLE_PVR_STORAGE_NUM; i++)
		{
			if (SVC_FS_GetPvrPath (i, eSVC_FS_UsageType_Record, path) != ERR_OK)
				continue;

			svc_meta_DecryptDir(path);
		}
	}
	else if (stat(szPath, &fstat) == 0 && S_ISDIR(fstat.st_mode))
	{
		svc_meta_DecryptDir(szPath);
	}
	else
	{
		svc_meta_DecryptFile(szPath);
	}
#endif
}


HERROR		SVC_META_GetDRMInfoByTime( HUINT32 ulContentsId, eDxCopyrightControl_CiPlusDrmType_e eDrmType, HUINT32 ulSourceTime, DxCopyrightControl_t *pstInfo )
{
	HERROR		ulRet = ERR_OK;
	SvcMeta_TimestampList_t stTimeStampList;
	SvcMeta_Timestamp_t *pstTimeStamp = NULL;
	HUINT32 ulTimeStampNum = 0;
	SvcMeta_TimestampList_t *pstList = NULL;
	HUINT32 i, ulMaxIndex=0, ulMaxTime = 0, ulCount=0;
	DxCopyrightControl_t *pstDrmInfo;

	if( pstInfo == NULL )
		return ERR_FAIL;

	ulRet = SVC_META_LoadTimeStamp( ulContentsId, &stTimeStampList );
	if( ulRet != ERR_OK )
	{
		HxLOG_Error("SVC_META_LoadTimeStamp, ulRet:0x%X, ulContentsId:%d\n", ulRet, ulContentsId);
		return ERR_FAIL;
	}

	pstList = &stTimeStampList;
	ulTimeStampNum = pstList->ulNumber;

	for( i=0; i<ulTimeStampNum; i++ )
	{
		pstTimeStamp = &pstList->pstData[i].stTimeStamp;

		if( pstTimeStamp->eType != eSvcMeta_Timestamp_DRM ) continue;

		pstDrmInfo = (DxCopyrightControl_t*)pstList->pstData[i].pData;

		if(pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType != eDrmType) continue;

		if( pstTimeStamp->ulTime > ulSourceTime )
		{
			continue;
		}
		ulCount++;

		if( ulMaxTime <= pstTimeStamp->ulTime )
		{
			ulMaxIndex = i;
			ulMaxTime =  pstTimeStamp->ulTime;
		}
	}

	if( ulCount == 0 )
	{
		SVC_META_UnloadTimeStamp(&stTimeStampList);
		HxLOG_Error("DRM Info not found, ulContentsId:%d\n", ulContentsId);
		return ERR_FAIL;
	}

	HxSTD_MemCopy( pstInfo, (DxCopyrightControl_t *)pstList->pstData[ulMaxIndex].pData, sizeof(DxCopyrightControl_t) );

	SVC_META_UnloadTimeStamp(&stTimeStampList);

	return ERR_OK;
}


HERROR		SVC_META_ConvertEpgExtendedTextToString(SvcMeta_ExtEvt_t* pstExtEvt, HUINT8 * * ppucString)
{
	HERROR				hError = ERR_FAIL;
	HUINT16				usLength = 0;
	SvcMeta_ExtEvt_t		*pTempText = NULL;
	HUINT32				ulCount = 0;
	HBOOL				bUCS16 = FALSE;
	STATIC HUINT8		aucTextBuffer[EPG_CONVERT_STRING_BUFSIZE + 1];


	if ((pstExtEvt == NULL) || (ppucString == NULL))
	{
		HxLOG_Print("[%s:%d] param fail!!\n", __FUNCTION__, __LINE__);
		goto EXIT_PVREPG_FUNC;
	}
	*ppucString = NULL;

	aucTextBuffer[0] = '\0';
	pTempText = pstExtEvt;

	/* multibyte: 2byte language */
	if (pTempText->pszChar[0] == 0x11)
	{
		bUCS16 = TRUE;
	}
	if (bUCS16 == TRUE)
	{
		while(pTempText)
		{
			//usLength = MWC_UTIL_DvbStrlen(DxEPG_EXT_TEXT_LEN);
			usLength = DxEPG_EXT_TEXT_LEN;
			if ((ulCount + usLength) >= EPG_CONVERT_STRING_BUFSIZE)
			{
				break;
			}
			HxSTD_MemCopy(&aucTextBuffer[ulCount], pTempText->pszChar, sizeof(HUINT8)*usLength);
			ulCount += usLength;

			pTempText = pTempText->pNext;
		}
	}
	else
	{
		while (pTempText)
		{
			usLength = MWC_UTIL_DvbStrlen(pTempText->pszChar);
			if ((ulCount + usLength) >= EPG_CONVERT_STRING_BUFSIZE)
			{
				break;
			}
			HxSTD_MemCopy(&aucTextBuffer[ulCount], pTempText->pszChar, sizeof(HUINT8)*usLength);
			ulCount += usLength;

			pTempText = pTempText->pNext;
		}
	}

	if (ulCount < EPG_CONVERT_STRING_BUFSIZE - 4)
	{
		aucTextBuffer[ulCount] = 0;
		aucTextBuffer[ulCount + 1] = 0;
	}
	else
	{
		aucTextBuffer[ulCount - 2] = 0;
		aucTextBuffer[ulCount - 1] = 0;
	}


	if (MWC_UTIL_DvbStrlen(aucTextBuffer) == 0)
	{
		goto EXIT_PVREPG_FUNC;
	}

	*ppucString = aucTextBuffer;

	hError = ERR_OK;

EXIT_PVREPG_FUNC :

	return hError;

}


HERROR		SVC_META_ConvertEpgExtEvtItemTextToString(SvcMeta_ExtEvt_t* pstExtEvt, HUINT8 ** ppucDescString, HUINT8 ** ppucItemString)
{
	HERROR				hError = ERR_FAIL;
	HUINT16				usLength = 0;
	SvcMeta_ExtEvt_t		*pTempText = NULL;
	HUINT32				ulCount = 0, i = 0;
	HBOOL				bUCS16 = FALSE;
	STATIC HUINT8		aucDescTextBuffer[EPG_CONVERT_STRING_BUFSIZE + 1];
	STATIC HUINT8		aucItemTextBuffer[EPG_CONVERT_STRING_BUFSIZE + 1];

	/* prevent defect assign이 아니고 compare로 수정함. */
	if ((pstExtEvt == NULL) || (ppucItemString == NULL) || (ppucDescString == NULL))
	{
		HxLOG_Print("[%s:%d] param fail!!\n", __FUNCTION__, __LINE__);
		goto EXIT_PVREPG_FUNC;
	}

	*ppucDescString = NULL;
	*ppucItemString = NULL;

	aucDescTextBuffer[0] = '\0';
	aucItemTextBuffer[0] = '\0';

	pTempText = pstExtEvt;

	// Extended Info Description Text
	for (i = 0; i < PVR_EXT_ITEM_MAX; i++)
	{
		usLength = MWC_UTIL_DvbStrlen(pTempText->astItem[i].pszDescription);
		if (usLength > 0)
		{
			/* multibyte: 2byte language */
			if (pTempText->astItem[i].pszDescription[0] == 0x11)
			{
				bUCS16 = TRUE;
			}
		}
		else
		{
			continue;
		}

		if (bUCS16 == TRUE)
		{
			//usLength = MWC_UTIL_DvbStrlen(DxEPG_EXT_TEXT_LEN);
			usLength = DxEPG_EXT_TEXT_LEN;
			if ((ulCount + usLength) >= EPG_CONVERT_STRING_BUFSIZE)
			{
				break;
			}
			HxSTD_MemCopy(&aucDescTextBuffer[ulCount], pTempText->astItem[i].pszDescription, sizeof(HUINT8)*usLength);
			ulCount += usLength;
		}
		else
		{
			usLength = MWC_UTIL_DvbStrlen(pTempText->astItem[i].pszDescription);
			if ((ulCount + usLength) >= EPG_CONVERT_STRING_BUFSIZE)
			{
				break;
			}
			HxSTD_MemCopy(&aucDescTextBuffer[ulCount], pTempText->astItem[i].pszDescription, sizeof(HUINT8)*usLength);
			ulCount += usLength;
		}
	}

	// Extended Info Item Text
	for (i = 0; i < PVR_EXT_ITEM_MAX; i++)
	{
		usLength = MWC_UTIL_DvbStrlen(pTempText->astItem[i].pszItem);
		if (usLength > 0)
		{
			/* multibyte: 2byte language */
			if (pTempText->astItem[i].pszItem[0] == 0x11)
			{
				bUCS16 = TRUE;
			}
		}
		else
		{
			continue;
		}

		if (bUCS16 == TRUE)
		{
			//usLength = MWC_UTIL_DvbStrlen(DxEPG_EXT_TEXT_LEN);
			usLength = DxEPG_EXT_TEXT_LEN;
			if ((ulCount + usLength) >= EPG_CONVERT_STRING_BUFSIZE)
			{
				break;
			}
			HxSTD_MemCopy(&aucItemTextBuffer[ulCount], pTempText->astItem[i].pszItem, sizeof(HUINT8)*usLength);
			ulCount += usLength;
		}
		else
		{
			usLength = MWC_UTIL_DvbStrlen(pTempText->astItem[i].pszItem);
			if ((ulCount + usLength) >= EPG_CONVERT_STRING_BUFSIZE)
			{
				break;
			}
			HxSTD_MemCopy(&aucItemTextBuffer[ulCount], pTempText->astItem[i].pszItem, sizeof(HUINT8)*usLength);
			ulCount += usLength;
		}
	}

	if (ulCount < EPG_CONVERT_STRING_BUFSIZE - 4)
	{
		aucDescTextBuffer[ulCount] = 0;
		aucDescTextBuffer[ulCount + 1] = 0;

		aucItemTextBuffer[ulCount] = 0;
		aucItemTextBuffer[ulCount + 1] = 0;
	}
	else
	{
		aucDescTextBuffer[ulCount - 2] = 0;
		aucDescTextBuffer[ulCount - 1] = 0;

		aucItemTextBuffer[ulCount - 2] = 0;
		aucItemTextBuffer[ulCount - 1] = 0;
	}

	if (MWC_UTIL_DvbStrlen(aucDescTextBuffer) != 0)
	{
		*ppucDescString = aucDescTextBuffer;
	}

	if (MWC_UTIL_DvbStrlen(aucItemTextBuffer) != 0)
	{
		*ppucItemString = aucItemTextBuffer;
	}

	hError = ERR_OK;

EXIT_PVREPG_FUNC :

	return hError;

}

#define __RES__
HERROR SVC_META_RegisterPvrAction(Handle_t hAction, HUINT32 ulContentId, SvcMeta_UsingActionType_e	ePvrActionType)
{
	svcMetaResAction_t	*pstPvrAction;

	HxLOG_Trace();

	pstPvrAction = svc_meta_FindPvrAction(hAction, ulContentId, ePvrActionType);
	if (NULL != pstPvrAction)
	{
		HxLOG_Error("Already Registered:\n");
		return ERR_OK;
	}

	pstPvrAction = (svcMetaResAction_t *)HLIB_STD_MemCalloc(sizeof(svcMetaResAction_t));
	if (NULL == pstPvrAction)
	{
		HxLOG_Error("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	pstPvrAction->hAction			= hAction;
	pstPvrAction->ulContentId		= ulContentId;
	pstPvrAction->ePvrActionType	= ePvrActionType;

	s_pstMetaResAction_List = HLIB_LIST_Append (s_pstMetaResAction_List, (void *)pstPvrAction);
	return ERR_OK;

}

HERROR SVC_META_UnRegisterPvrAction(Handle_t hAction)
{
	svcMetaResAction_t	*pstPvrAction;

	HxLOG_Trace();

	pstPvrAction = svc_meta_FindPvrActionByhAction(hAction, eSvcMeta_UsingActType_None);
	if (NULL == pstPvrAction)
	{
		HxLOG_Error("Not Registered:\n");
		return ERR_OK;
	}

	s_pstMetaResAction_List = HLIB_LIST_Remove (s_pstMetaResAction_List, (const void *)pstPvrAction);
	HLIB_STD_MemFree (pstPvrAction);

	return ERR_OK;
}

HERROR SVC_META_GetPlaybackActionByContentId( HUINT32 ulContentId, Handle_t *phAction )
{
	svcMetaResAction_t	*pstPvrAction;

	if (NULL == phAction)
		return ERR_FAIL;

	pstPvrAction = svc_meta_FindPvrActionByContentId(ulContentId, eSvcMeta_UsingActType_Playback);
	if( NULL == pstPvrAction )
	{
		HxLOG_Error ("can't find playback action : ulContentId(%d)\n", ulContentId);
		return ERR_FAIL;
	}

	*phAction= pstPvrAction->hAction;

	return ERR_OK;
}

HERROR SVC_META_GetRecActionByContentId( HUINT32 ulContentId, Handle_t *phAction )
{
	svcMetaResAction_t	*pstPvrAction;

	if (NULL == phAction)
		return ERR_FAIL;

	pstPvrAction = svc_meta_FindPvrActionByContentId(ulContentId, eSvcMeta_UsingActType_Record);
	if( NULL == pstPvrAction )
	{
		HxLOG_Debug ("can't find record action : ulContentId(%d)\n", ulContentId);
		return ERR_FAIL;
	}

	*phAction= pstPvrAction->hAction;

	return ERR_OK;
}

HERROR SVC_META_GetContentIdByhAction( Handle_t hAction, HUINT32 *pulContentId )
{
	svcMetaResAction_t	*pstPvrAction;

	pstPvrAction = svc_meta_FindPvrActionByhAction(hAction, eSvcMeta_UsingActType_None); // nonetype is all
	if( NULL == pstPvrAction )
	{
		HxLOG_Error("can't find pvr action : hAction(%d)\n", hAction);
		return ERR_FAIL;
	}

	*pulContentId= pstPvrAction->ulContentId;

	return ERR_OK;
}

#define __INIT__
HERROR SVC_META_Init(PostMsgToMgrCb_t fnPostMsgToMgr)
{
	HERROR ulRet = ERR_OK;
	HxLOG_Trace();

	s_fnSvcMetaPostMsgToMgr = fnPostMsgToMgr;

	ulRet = HxSEMT_Create (&s_ulMetaSem, "PvrMeta", HxSEMT_FIFO);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("HxSEMT_Create fail, ulRet:0x%X\n", ulRet);
		return ulRet;
	}

	return ERR_OK;
}

void CMD_Register_SVC_META(void)
{
#if defined(CONFIG_DEBUG)
#define	hCmdHandle		"svc_meta"
	HLIB_CMD_RegisterWord(hCmdHandle, cmd_meta_PrintList,			"list" ,
												"list all the contents" ,
												"list");
#endif
}

