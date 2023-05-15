
#define ________CA_SESSION_MGR_Private_Include_____________________________________________________
/**************************************************************************************************/
#include "na_glob.h"
#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_SESSION_MGR_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_SESSION_MGR_Private_Define_______________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________CA_SESSION_MGR_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef struct
{
	Handle_t			hAction;
	TTransportSessionId xTsId;		// mapped with demux_id
	TUnsignedInt32		ulNumOfPid;
	TUnsignedInt16		*pusPids;
}CaSessionMgr_Context_t;

/**************************************************************************************************/
#define ________CA_SESSION_MGR_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static CaSessionMgr_Context_t	s_astCaSessionContext[NUM_MAX_ACTION];

#if defined(MULTI_DIRECT_TSID)
typedef struct
{
	TTransportSessionId xTsId;		// mapped with demux_id
	HBOOL 				bSeTsIdtDirect;
} CaSessionMgr_SetTsidDirect_t;

static CaSessionMgr_SetTsidDirect_t stSetTsidDirect[NUM_MAX_ACTION];
#else
static HBOOL					s_bSetTsIdDirect = FALSE;
#endif
/**************************************************************************************************/
#define ________CA_SESSION_MGR_Private_Static_Prototype______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_SESSION_MGR_Externel_Functions_Prototype_________________________________________
/**************************************************************************************************/
extern TSessionManagerStatus brcmCaAddTransportSessionContext(TTransportSessionId xTSid, NEXUS_ParserBand parserBand, NEXUS_PlaypumpHandle playpumpHandle, BrcmCaStreamSessionKeyHandleCallback streamSessionKeyHandleCallback, BrcmCaCipherSessionKeyLoadedCallback cipherSessionKeyLoadedCallback);
extern TSessionManagerStatus brcmCaRemoveTransportSessionContext(TTransportSessionId xTSid);
extern TSessionManagerStatus brcmCaAddPidToTransportSessionId(TTransportSessionId xTSid, TUnsignedInt16 xPid);
extern TSessionManagerStatus brcmCaRemovePidFromTransportSessionId(TTransportSessionId xTSid, TUnsignedInt16 xPid);
extern TSessionManagerStatus brcmCaAssociateIvToTransportSessionId(TTransportSessionId xTSid, const TUnsignedInt8 *pxIv, size_t xIvSize);
extern TSessionManagerStatus brcmCaGetM2MKeyslotHandle(TTransportSessionId xTSid, NEXUS_KeySlotHandle *pM2mKeyHandle);


/**************************************************************************************************/
#define ________CA_SESSION_MGR_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/
#if defined(MULTI_DIRECT_TSID)
static CaSessionMgr_SetTsidDirect_t * _caSessionMgr_GetTsidDirectByTsid(TTransportSessionId xTSid) 
{
	int i;

	HxLOG_Print("[%s:%d] \n", __FUNCTION__, __LINE__);
	for(i=0; i<NUM_MAX_ACTION; i++)
	{
		HxLOG_Print("\t\t [%d] - tsid(0x%08x)\n", i, stSetTsidDirect[i].xTsId);
		if(stSetTsidDirect[i].xTsId == xTSid)
		{
			HxLOG_Print("\t\t [%d] - Get OK\n", i);
			return &stSetTsidDirect[i];
		}
	}

	return NULL;
}

static CaSessionMgr_SetTsidDirect_t *_caSessionMgr_GetNewTsidDirect(void)
{
	int i;

	HxLOG_Print("[%s:%d] \n", __FUNCTION__, __LINE__);

	for(i=0; i<NUM_MAX_ACTION; i++)
	{
		HxLOG_Print("\t\t [%d] - tsid(0x%08x)\n", i, stSetTsidDirect[i].xTsId);
		if(stSetTsidDirect[i].xTsId == TRANSPORT_SESSION_ID_INVALID)
		{
			HxLOG_Print("\t\t [%d] - Create\n", i);
			return &stSetTsidDirect[i];
		}
	}

	HxLOG_Print("[%s:%d]  ---------------------------------- error\n", __FUNCTION__, __LINE__);
	return NULL;	
}
#endif

static CaSessionMgr_Context_t *_caSessionMgr_GetTransportSessionContext(Handle_t hAction)
{
	HUINT32 ulActionId;

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId < NUM_MAX_ACTION)
	{
		return &s_astCaSessionContext[ulActionId];
	}

	return NULL;
}

#if defined(CONFIG_SUPPORT_NOVA_APP)
static HERROR  _caSessionMgr_SendDrmHandleToNova(Handle_t hAction, Handle_t hSvcHdl)
{
	HERROR hErr;
	NaPrmKeyInfo_t stKeyInfo;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);


	hErr = svc_cas_MgrSubUpdateCasEtcEventData((Handle_t)hAction, (Handle_t)NULL, eDxCAS_GROUPID_NA, eSEVT_CAS_UPDATE_NOVA_DRM_HANDLE, hSvcHdl, 0, NULL, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");

		return ERR_FAIL;;
	}

	return ERR_OK;
}
#endif

static HERROR  _caSessionMgr_AddTransportSessionId(Handle_t hAction)
{
	CaSessionMgr_Context_t	*pstContext;
	TTransportSessionId		xTsId;
	int nReturn;
#if defined(MULTI_DIRECT_TSID)
	CaSessionMgr_SetTsidDirect_t * pstSetTsidDirect = NULL;
#endif	

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	pstContext = _caSessionMgr_GetTransportSessionContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("_caSessionMgr_GetTransportSessionContext() error\n");
		return ERR_FAIL;
	}

	if(pstContext->hAction != HANDLE_NULL)
	{
		HxLOG_Warning("pstContext->hAction is not HANDLE_NULL\n");
	}

	if(svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, (HUINT32 *)&xTsId) != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId() error\n");
		return ERR_FAIL;
	}

#if !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	HxLOG_Print("\tx -- BrcmAdd_TsId(0x%08x)\n", xTsId);

// SES & TARA DEMO위해 임시로 추가함. SATIP MENU로 진입후 CAS 채널로 동작시키면 SATIP Menu로 진입하면서 brcmCaRemoveTransportSessionContext가 호출이 안된다.
// 그래서 임시로 여기에 테스트용으로 remove를 해줌. gcan
#if 0//defined(TARA_SES_DEMO) // TARA & SES Demo only
	if(xTsId==0)
	{
		nReturn = brcmCaRemoveTransportSessionContext(xTsId);
		if(nReturn)
		{
			HxLOG_Error("brcmCaRemoveTransportSessionContext() error(%d), xTsId(%x)\n", nReturn, xTsId);
		}
	}
#endif

#if defined(CAS_ZAP_LOG)
	{
		static int addCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SESSION [%s:%d] ----- (%d) ----- hAction(0x%08x) Add Tsid[%d] ----------\n", __FUNCTION__, __LINE__, addCnt++, hAction, xTsId);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}	
#endif

	nReturn = brcmCaAddTransportSessionContext(xTsId, xTsId, 0, NULL, NULL);
	if(nReturn)
	{
		HxLOG_Error("brcmCaAddTransportSessionContext() error(%d), hAction(%x), xTsId(%x)\n", nReturn, hAction, xTsId);
		return ERR_FAIL;
	}
#endif

	pstContext->hAction = hAction;
	pstContext->xTsId = xTsId;

#if defined(MULTI_DIRECT_TSID)
	HxLOG_Print("[%s:%d] add transportSessionId\n", __FUNCTION__, __LINE__);
	pstSetTsidDirect = _caSessionMgr_GetTsidDirectByTsid(xTsId);
	if(pstSetTsidDirect == NULL)
	{
		HxLOG_Error("add  :  xTransportSessionId(%d)\n", xTsId);
		pstSetTsidDirect = _caSessionMgr_GetNewTsidDirect();
	}

	if(pstSetTsidDirect == NULL)
	{
		HxLOG_Print("### humandks_satip_tsid [%s:%d] add transportSessionId ------------- error\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

	pstSetTsidDirect->xTsId = xTsId;
	pstSetTsidDirect->bSeTsIdtDirect = FALSE;

	HxLOG_Print("[%s:%d] add transportSessionId No Direct - tsid(%d) TsIdDirect(%d)\n", __FUNCTION__, __LINE__, pstSetTsidDirect->xTsId, pstSetTsidDirect->bSeTsIdtDirect);
#else
	s_bSetTsIdDirect = FALSE;
#endif

	HxLOG_Print("[%s:%d] hAction(%x), xTsId(%d)\n", __FUNCTION__, __HxLINE__, pstContext->hAction, pstContext->xTsId);

	return ERR_OK;
}

static HERROR _caSessionMgr_RemoveTransportSessionId(Handle_t hAction)
{
	int nReturn;
	CaSessionMgr_Context_t *pstContext;
#if defined(MULTI_DIRECT_TSID)
	CaSessionMgr_SetTsidDirect_t * pstSetTsidDirect = NULL;
#endif

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	pstContext = _caSessionMgr_GetTransportSessionContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("_caSessionMgr_GetTransportSessionContext() error\n");
		return ERR_FAIL;
	}

#if defined(CAS_ZAP_LOG)
{
	static int rmCnt  = 0;
	HxLOG_Print("===================================================================== \n");
	HxLOG_Print("=\n");
	HxLOG_Print("\t CAS_ZAP : CA SESSION [%s:%d] ----- (%d) ----- Remove TransportSessionId  hAction(0x%08x), tsId[%d] ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, pstContext->xTsId);
	HxLOG_Print("=\n");
	HxLOG_Print("===================================================================== \n");
}
#endif


	if(pstContext->hAction == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hAction is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	if(pstContext->hAction != hAction)
	{
		// 실제 발생하면 안되는 경우겠지?
		HxLOG_Warning("Action is not valid, pstContext->hAction(%x), hAction(%x)\n", pstContext->hAction, hAction);
	}

#if !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	nReturn = brcmCaRemoveTransportSessionContext(pstContext->xTsId);
	if(nReturn)
	{
		HxLOG_Error("brcmCaRemoveTransportSessionContext() error(%d), hAction(%x), xTsId(%x)\n", nReturn, hAction, pstContext->xTsId);
		return ERR_FAIL;
	}
#endif

	HxLOG_Print("[%s:%d] hAction(%x), xTsId(%d)\n", __FUNCTION__, __HxLINE__, pstContext->hAction, pstContext->xTsId);

#if defined(MULTI_DIRECT_TSID)
	HxLOG_Print("[%s:%d] remove transportSessionId -> \n", __FUNCTION__, __LINE__);
	pstSetTsidDirect = _caSessionMgr_GetTsidDirectByTsid(pstContext->xTsId);
	if(pstSetTsidDirect)
	{
		HxLOG_Print("[%s:%d] remove ok, xTsId(%d)\n", __FUNCTION__, __LINE__, pstSetTsidDirect->xTsId);
		pstSetTsidDirect->xTsId = TRANSPORT_SESSION_ID_INVALID;
		pstSetTsidDirect->bSeTsIdtDirect = FALSE;
	}
#endif

	pstContext->hAction = HANDLE_NULL;
	pstContext->xTsId = TRANSPORT_SESSION_ID_INVALID;

	return ERR_OK;
}

static HERROR _caSessionMgr_AssociatePids(Handle_t hAction, HUINT32 ulNumOfPid, HUINT16 *pusPids)
{
	CaSessionMgr_Context_t *pstContext;
	int i, j;
	HBOOL bFound;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	if( (ulNumOfPid && pusPids == NULL) || (ulNumOfPid == 0 && pusPids != NULL) )
	{
		HxLOG_Error("Parameter error, ulNumOfPid(%d), pusPids(0x%x)\n", ulNumOfPid, pusPids);
		return ERR_FAIL;
	}

	pstContext = _caSessionMgr_GetTransportSessionContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("_caSessionMgr_GetTransportSessionContext() error\n");
		return ERR_FAIL;
	}

	if(pstContext->hAction == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hAction is HANDLE_NULL\n");
		return ERR_FAIL;
	}

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SESSION [%s:%d] ----- (%d) ----- Add or Remove Pid - hAction(0x%08x), tsId[%d] ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, pstContext->xTsId);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif

	if(pstContext->hAction != hAction)
	{
		// 실제 발생하면 안되는 경우겠지?
		HxLOG_Warning("Action is not valid, pstContext->hAction(%x), hAction(%x)\n", pstContext->hAction, hAction);
	}

	// 기존에 가진 PID list에서 Remove 되어야 하는 PID를 찾는다.
	for(i = 0 ; i < pstContext->ulNumOfPid ; i++)
	{
		bFound = FALSE;
		for(j = 0 ; j < ulNumOfPid ; j++)
		{
			if(pstContext->pusPids[i] == pusPids[j])
			{
				bFound = TRUE;
				break;
			}
		}

		if(bFound == FALSE && VALID_PID(pstContext->pusPids[i]))
		{
			HxLOG_Print("[%s:%d] xTsId[%x], RemovePid[%x]\n", __FUNCTION__, __HxLINE__, pstContext->xTsId, pstContext->pusPids[i]);
			if(brcmCaRemovePidFromTransportSessionId(pstContext->xTsId, pstContext->pusPids[i]))
				HxLOG_Error("xTsId[%x], RemovePid[%x] error\n", pstContext->xTsId, pstContext->pusPids[i]);
		}
	}

	// 새로운 PID list에서 Add 되어야 하는 PID를 찾는다.
	for(i = 0 ; i < ulNumOfPid ; i++)
	{
		bFound = FALSE;
		for(j = 0 ; j < pstContext->ulNumOfPid ; j++)
		{
			if(pusPids[i] == pstContext->pusPids[j])
			{
				bFound = TRUE;
				break;
			}
		}

		if(bFound == FALSE && VALID_PID(pusPids[i]))
		{
			HxLOG_Print("[%s:%d] xTsId[%x], AddPid[%x]\n", __FUNCTION__, __HxLINE__, pstContext->xTsId, pusPids[i]);
			if(brcmCaAddPidToTransportSessionId(pstContext->xTsId, pusPids[i]))
				HxLOG_Error("xTsId[%x], AddPid[%x] error\n", pstContext->xTsId, pusPids[i]);
		}
	}

	// 기존에 관리하고 있는 PID 정보를 update 한다.
	if(pstContext->pusPids)
	{
		OxCAS_Free(pstContext->pusPids);
	}

	pstContext->ulNumOfPid	= ulNumOfPid;
	pstContext->pusPids		= pusPids;

	return ERR_OK;
}

static HERROR _caSessionMgr_AssociateIv(Handle_t hAction, const TUnsignedInt8 *pxIv, size_t xIvSize)
{
	CaSessionMgr_Context_t *pstContext;
	int nReturn;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	pstContext = _caSessionMgr_GetTransportSessionContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("_caSessionMgr_GetTransportSessionContext() error\n");
		return ERR_FAIL;
	}

	if(pstContext->hAction == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hAction is HANDLE_NULL\n");
		return ERR_FAIL;
	}

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SESSION [%s:%d] ----- (%d) ----- Associate Iv hAction(0x%08x), tsId[%d] ----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, pstContext->xTsId);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif


	if(pstContext->hAction != hAction)
	{
		// 실제 발생하면 안되는 경우겠지?
		HxLOG_Warning("Action is not valid, pstContext->hAction(%x), hAction(%x)\n", pstContext->hAction, hAction);
	}

	nReturn = brcmCaAssociateIvToTransportSessionId(pstContext->xTsId, pxIv, xIvSize);
	if(nReturn)
	{
		HxLOG_Error("brcmCaAssociateIvToTransportSessionId() error(%d), hAction(%x), xTsId(%x)\n", nReturn, hAction, pstContext->xTsId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR _caSessionMgr_GetKeyslotHandle(Handle_t hAction, Handle_t *phKeySlot)
{
	CaSessionMgr_Context_t *pstContext;
	TUnsignedInt32 ulKeySlot;
	int nReturn;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	if(phKeySlot == NULL)
	{
		HxLOG_Error("phKeySlot is NULL\n");
		return ERR_FAIL;
	}

	pstContext = _caSessionMgr_GetTransportSessionContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("_caSessionMgr_GetTransportSessionContext() error\n");
		return ERR_FAIL;
	}

	if(pstContext->hAction == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hAction is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	if(pstContext->hAction != hAction)
	{
		// 실제 발생하면 안되는 경우겠지?
		HxLOG_Warning("Action is not valid, pstContext->hAction(%x), hAction(%x)\n", pstContext->hAction, hAction);
	}

	nReturn = brcmCaGetM2MKeyslotHandle(pstContext->xTsId, &ulKeySlot);
	if(nReturn)
	{
		HxLOG_Error("brcmCaGetM2MKeyslotHandle() error(%d), hAction(%x), xTsId(%x)\n", nReturn, hAction, pstContext->xTsId);
		return ERR_FAIL;
	}

	*phKeySlot = (Handle_t)ulKeySlot;

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SESSION [%s:%d] ----- (%d) ----- Get Key Slot hAction(0x%08x), tsId[%d] SlotHandle(0x%08x)----------\n", __FUNCTION__, __LINE__, rmCnt++, hAction, pstContext->xTsId, ulKeySlot);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif


	return ERR_OK;
}

#if defined (CONFIG_MW_CAS_NAGRA_PRM_3_1)
#if defined (_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
static HINT32 _caSessionMgr_NotifyKeyLoaded(TTransportSessionId xTSid, /*NEXUS_KeySlotHandle*/TUnsignedInt32 ulKeySlotHandle /* handle of keyslot */)
{
	HERROR hErr;
	NaPrmKeyInfo_t stKeyInfo;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	stKeyInfo.usEmi = 0x4023;  /* HLS (EMI : 0x4023), 일단 HLS만 고려 */
	stKeyInfo.ulPlaybackHandle = 0xFFFFFFFF; /* 의미 없는 값. 위에서 xTSid를 가지고 확인 하자. */
	stKeyInfo.xTSid = xTSid;
	stKeyInfo.ulKeyHandler = ulKeySlotHandle;

	hErr = svc_cas_MgrSubUpdateCasEtcEventData((Handle_t)NULL, (Handle_t)NULL, eDxCAS_GROUPID_NA, eSEVT_CAS_UPDATE_KEYINFO, 0, eDxCAS_GROUPID_NA, (void*)&stKeyInfo, sizeof(NaPrmKeyInfo_t));
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");

		return -1;
	}

	return 0;
}
#endif

#if defined(_NA_PRM_PVR_KEY_NOTIFY_FRM_NEXUS_)
static HINT32 _caSessionMgr_NotifyPvrKeyLoaded(TTransportSessionId xTSid, /*NEXUS_KeySlotHandle*/TUnsignedInt32 ulKeySlotHandle /* handle of keyslot */)
{
	HERROR hErr;
	NaPrmPvrKeyInfo_t stKeyInfo;

	stKeyInfo.xTSid = xTSid;
	stKeyInfo.ulKeyHandler = ulKeySlotHandle;

	HxLOG_Print("[%s:%d] called...xTSid(%d), ulKeySlotHandle(0x%08x)\n", __FUNCTION__, __HxLINE__, xTSid, ulKeySlotHandle);

	hErr = svc_cas_MgrSubUpdateCasEtcEventData((Handle_t)NULL, (Handle_t)NULL, eDxCAS_GROUPID_NA, eSEVT_CAS_PRM_PVR_UPDATE_KEYINFO, 0, eDxCAS_GROUPID_NA, (void*)&stKeyInfo, sizeof(NaPrmPvrKeyInfo_t));
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");

		return -1;
	}

	return 0;
}
#endif

static HERROR  _caSessionMgr_AddTransportSessionIdDirect(TTransportSessionId xTsId)
{
	int nReturn;
#if defined(MULTI_DIRECT_TSID)
	CaSessionMgr_SetTsidDirect_t * pstSetTsidDirect = NULL;
#endif

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

#if !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#if defined (_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	nReturn = brcmCaAddTransportSessionContext(xTsId, NULL, NULL, _caSessionMgr_NotifyKeyLoaded);
#else
	#if defined(_NA_PRM_PVR_KEY_NOTIFY_FRM_NEXUS_) // TODO :  _NA_PRM_KEY_NOTIFY_FROM_NEXUS_ 와 같이 사용될수 있으므로 정리 필요.
	nReturn = brcmCaAddTransportSessionContext(xTsId, xTsId, 0, NULL, _caSessionMgr_NotifyPvrKeyLoaded);
	#else
	nReturn = brcmCaAddTransportSessionContext(xTsId, xTsId, 0, NULL, NULL);
#endif
#endif
	if(nReturn)
	{
		HxLOG_Error("brcmCaAddTransportSessionContext() error(%d), xTsId(%x)\n", nReturn, xTsId);
		return ERR_FAIL;
	}
#endif

#if defined(MULTI_DIRECT_TSID)
	HxLOG_Print("[%s:%d] ADD Direct xTsId(%d)\n", __FUNCTION__, __LINE__, xTsId);
	pstSetTsidDirect = _caSessionMgr_GetTsidDirectByTsid(xTsId);
	if(pstSetTsidDirect == NULL)
	{
		HxLOG_Print("add  :  xTransportSessionId(%d)\n", xTsId);
		pstSetTsidDirect = _caSessionMgr_GetNewTsidDirect();
	}

	if(pstSetTsidDirect == NULL)
	{
		HxLOG_Print("### humandks_satip_tsid [%s:%d] -------------------- Error xTsId(%d)\n", __FUNCTION__, __LINE__, xTsId);
		return ERR_FAIL;
	}

	pstSetTsidDirect->xTsId = xTsId;
	pstSetTsidDirect->bSeTsIdtDirect = TRUE;

	HxLOG_Print("[%s:%d] add transportSessionId Direct - tsid(%d) TsIdDirect(%d)\n", __FUNCTION__, __LINE__, pstSetTsidDirect->xTsId, pstSetTsidDirect->bSeTsIdtDirect);
#else
	s_bSetTsIdDirect = TRUE;
#endif

	HxLOG_Print("[%s:%d] xTsId(%d)\n", __FUNCTION__, __HxLINE__, xTsId);

	return ERR_OK;
}

static HERROR _caSessionMgr_RemoveTransportSessionIdDirect(TTransportSessionId xTsId)
{
	int nReturn;
#if defined(MULTI_DIRECT_TSID)
	CaSessionMgr_SetTsidDirect_t * pstSetTsidDirect = NULL;
#endif

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

#if defined(CAS_ZAP_LOG)
	{
		static int rmCnt  = 0;
		HxLOG_Print("===================================================================== \n");
		HxLOG_Print("=\n");
		HxLOG_Print("\t CAS_ZAP : CA SESSION[%s:%d] ----- (%d) ----- direct Remove tsId[%d] ----------\n", __FUNCTION__, __LINE__, rmCnt++, xTsId);
		HxLOG_Print("=\n");
		HxLOG_Print("===================================================================== \n");
	}
#endif


#if !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	nReturn = brcmCaRemoveTransportSessionContext(xTsId);
	if(nReturn)
	{
		HxLOG_Error("brcmCaRemoveTransportSessionContext() error(%d), xTsId(%x)\n", nReturn, xTsId);
		return ERR_FAIL;
	}
#endif

#if defined(MULTI_DIRECT_TSID)
	HxLOG_Print("[%s:%d] Remove tsid(%d)\n", __FUNCTION__, __LINE__, xTsId);
	pstSetTsidDirect = _caSessionMgr_GetTsidDirectByTsid(xTsId);
	if(pstSetTsidDirect == NULL)
	{
		HxLOG_Print("[%s:%d] Remove -------------------------------------- Error\n", __FUNCTION__, __LINE__, xTsId);
		return ERR_OK;
	}

	pstSetTsidDirect->xTsId = TRANSPORT_SESSION_ID_INVALID;
	pstSetTsidDirect->bSeTsIdtDirect = FALSE;
#else
	s_bSetTsIdDirect = FALSE;
#endif

	HxLOG_Print("[%s:%d] xTsId(%d)\n", __FUNCTION__, __HxLINE__, xTsId);

	return ERR_OK;
}
#endif

/**************************************************************************************************/
#define ________CA_SESSION_MGR_Public_Functions_Body________________________________________________
/**************************************************************************************************/
#if defined (CONFIG_MW_CAS_NAGRA_PRM_3_1)
HERROR  caSessionMgr_AddTransportSessionIdDirect(TTransportSessionId xTsId)
{
	return _caSessionMgr_AddTransportSessionIdDirect(xTsId);
}

HERROR caSessionMgr_RemoveTransportSessionIdDirect(TTransportSessionId xTsId)
{
	return _caSessionMgr_RemoveTransportSessionIdDirect(xTsId);
}
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR	 caSessionMgr_SendDrmHandleToNova(Handle_t hAction, Handle_t hSvcHdl)
{
	return _caSessionMgr_SendDrmHandleToNova(hAction, hSvcHdl);
}
#endif

HERROR  caSessionMgr_AddTransportSessionId(Handle_t hAction)
{
	return _caSessionMgr_AddTransportSessionId(hAction);
}

HERROR caSessionMgr_RemoveTransportSessionId(Handle_t hAction)
{
	if(_caSessionMgr_AssociatePids(hAction, 0, NULL) != ERR_OK)
		HxLOG_Error("_caSessionMgr_AssociatePids() error, hAction(%x)\n", hAction);

	return _caSessionMgr_RemoveTransportSessionId(hAction);
}

HERROR caSessionMgr_AssociatePids(Handle_t hAction, HUINT32 ulNumOfPid, HUINT16 *pusPids)
{
	return _caSessionMgr_AssociatePids(hAction, ulNumOfPid, pusPids);
}

HERROR caSessionMgr_AssociateIv(Handle_t hAction, const TUnsignedInt8 *pxIv, size_t xIvSize)
{
	return _caSessionMgr_AssociateIv(hAction, pxIv, xIvSize);
}

HERROR caSessionMgr_GetKeyslotHandle(Handle_t hAction, Handle_t *phKeySlot)
{
	return _caSessionMgr_GetKeyslotHandle(hAction, phKeySlot);
}

#if defined(MULTI_DIRECT_TSID)
HBOOL	CA_NA_CaSessionMgrIsTsIdSetDirect(TTransportSessionId xTransportSessionId)
{
	CaSessionMgr_SetTsidDirect_t * pstSetTsidDirect = NULL;

	HxLOG_Print("[%s:%d]  IS DIRECT? - xTransportSessionId(%d)\n", __FUNCTION__, __LINE__, xTransportSessionId);

	pstSetTsidDirect = _caSessionMgr_GetTsidDirectByTsid(xTransportSessionId);
	if(pstSetTsidDirect == NULL)
	{
		HxLOG_Print("[%s:%d]  IS DIRECT? -  ------------ tsId is not exist\n", __FUNCTION__, __LINE__);
		// default로 false로 하자
		return FALSE;
	}

	HxLOG_Print("[%s:%d] bSeTsIdtDirect(%d)\n", __FUNCTION__, __HxLINE__, pstSetTsidDirect->bSeTsIdtDirect);

	return pstSetTsidDirect->bSeTsIdtDirect;
}
#else
HBOOL CA_NA_CaSessionMgrIsTsIdSetDirect(void)
{
	return s_bSetTsIdDirect;
}
#endif

HERROR CA_NA_CaSessionMgrInitialize(void)
{
	int i;

	for(i = 0 ; i < NUM_MAX_ACTION ; i++)
	{
		s_astCaSessionContext[i].hAction = HANDLE_NULL;
		s_astCaSessionContext[i].xTsId = TRANSPORT_SESSION_ID_INVALID;
		s_astCaSessionContext[i].ulNumOfPid = 0;
		s_astCaSessionContext[i].pusPids = NULL;
	}

#if defined(MULTI_DIRECT_TSID)
	for(i = 0 ; i < NUM_MAX_ACTION ; i++)
	{
		stSetTsidDirect[i].bSeTsIdtDirect = FALSE;
		stSetTsidDirect[i].xTsId = TRANSPORT_SESSION_ID_INVALID;
	}
#endif	

	return ERR_OK;
}

/*** End of File ***/
