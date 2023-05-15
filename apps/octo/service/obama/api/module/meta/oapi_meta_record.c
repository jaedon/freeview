
#include	<hlib.h>
#include	<dlib.h>
#include	<oapi.h>

#include "../../oapi_mem.h"


typedef struct
{
	OxMetaRecord_Notifier_t	listener;
	void 					*userdata;
	HUINT32					semaphore;
} OxRecordingsMgr_t;


static const HCHAR *	oapi_recordings_EventToString (OxMetaRecord_Event_e evt)
{
	switch (evt)
	{
	case eOxMETA_RECORD_ADDED:   return HxNAMEOF(eOxMETA_RECORD_ADDED);
	case eOxMETA_RECORD_UPDATED: return HxNAMEOF(eOxMETA_RECORD_UPDATED);
	case eOxMETA_RECORD_REMOVED: return HxNAMEOF(eOxMETA_RECORD_REMOVED);
	default:
		break;
	}
	return "Unknown";
}

static OxRecordingsMgr_t *	oapi_recordings_GetManager (void)
{
	static OxRecordingsMgr_t	*s_mgr;

	if (s_mgr == NULL)
	{
		s_mgr = (OxRecordingsMgr_t *)OxMEM_Calloc(sizeof(OxRecordingsMgr_t));
		HxLOG_Assert(s_mgr);
		HxSEMT_Create(&s_mgr->semaphore, "OxRecordingsMgr", 0);
	}
	HxSEMT_Get(s_mgr->semaphore);

	return s_mgr;
}

static void		oapi_recordings_ReleaseManager (OxRecordingsMgr_t *mgr)
{
	HxSEMT_Release(mgr->semaphore);
}

static void *	oapi_recordings_Pack (const DxRecListData_t *rec, HUINT32 *size)
{
	DxRecListData_t *	pack;

	*size = sizeof(DxRecListData_t) + sizeof(DxRecList_ExtEvt_t) * rec->ulExtEvtCnt;
	 pack = OxMEM_Calloc(*size);
	if (pack)
	{
		memcpy(pack, rec, sizeof(DxRecListData_t));
		if (rec->ulExtEvtCnt)
		{
			HxSTD_memcpy(pack + 1, rec->pstExtEvts, sizeof(DxRecList_ExtEvt_t) * rec->ulExtEvtCnt);
		}

	}
	return (void *)pack;
}

static DxRecListData_t *oapi_recordings_Unpack (DxRecListData_t *rec)
{
	if (rec->ulExtEvtCnt)
	{
		rec->pstExtEvts = (DxRecList_ExtEvt_t *)(rec + 1);
	}
	return rec;
}

static void		oapi_recordings_EmitEvent (OxMetaRecord_Event_e evt, HINT32 count, HUINT32 cid, const DxRecListData_t *rec)
{
	OxRecordingsMgr_t		*mgr;
	OxMetaRecord_Notifier_t	listener;
	void 					*userdata;

	HxLOG_Trace();

	mgr = oapi_recordings_GetManager();
	listener = mgr->listener;
	userdata = mgr->userdata;
	oapi_recordings_ReleaseManager(mgr);

	if (listener)
	{
		HxLOG_Debug("%s(evt:%s, count:%d, cid:%d, rec:%s<\'%s\'>)\n", __FUNCTION__
				, oapi_recordings_EventToString(evt), count, cid, rec ? rec->szUrl : "NULL", rec ? rec->szName : "NULL");
		listener(evt, count, cid, rec, userdata);
	}
}

static void		oapi_recordings_OnUpdate (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HINT32			variation;
	DxRecListData_t	*rec;

	HxLOG_Trace();

	variation = HxOBJECT_INT(argv[0]);
	// OnADD
	if (variation > 0)
	{
		if (argc == 1)	// 'i' [added recording count]
		{
			oapi_recordings_EmitEvent(eOxMETA_RECORD_ADDED, variation, 0, NULL);
		}
		else if (argc == 2)	// 'ib' [1][added recording:DxRecListData_t]
		{
			rec = oapi_recordings_Unpack(HxOBJECT_BIN_DATA(argv[1]));
			oapi_recordings_EmitEvent(eOxMETA_RECORD_ADDED, variation, rec->ulContentId, rec);
		}
		else
		{
			HxLOG_Error("%s(OnADD:%d, argc:%d) Invalid param(s)\n", __FUNCTION__, variation, argc);
		}
	}
	// OnREMOVE
	else if (variation < 0)
	{
		if (argc == 1)	// 'i' [removed recording count (negative value)]
		{
			oapi_recordings_EmitEvent(eOxMETA_RECORD_REMOVED, -variation, 0, NULL);
		}
		else if (argc == 2)	// 'ii[-1][recording contents id]
		{
			oapi_recordings_EmitEvent(eOxMETA_RECORD_REMOVED, -variation, HxOBJECT_INT(argv[1]), NULL);
		}
		else
		{
			HxLOG_Error("%s(OnREMOVE:%d, argc:%d) Invalid param(s)\n", __FUNCTION__, variation, argc);
		}
	}
	// OnUPDATE
	else // if (variation == 0)
	{
		if (argc == 2)	// 'ii' [0][updated recording count]
		{
			oapi_recordings_EmitEvent(eOxMETA_RECORD_UPDATED, HxOBJECT_INT(argv[1]), 0, NULL);
		}
		else if (argc == 3)	// 'iib' [0][1][recording:DxRecListData_t]
		{
			rec = oapi_recordings_Unpack(HxOBJECT_BIN_DATA(argv[2]));
			oapi_recordings_EmitEvent(eOxMETA_RECORD_UPDATED, HxOBJECT_INT(argv[1]), rec->ulContentId, rec);
		}
		else
		{
			HxLOG_Error("%s(OnUPDATE:%d, argc:%d) Invalid param(s)\n", __FUNCTION__, variation, argc);
		}
	}
}

static void		oapi_recordings_OnScanDone (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HINT32			nCount = 0;

	HxLOG_Trace();
	nCount = HxOBJECT_INT(argv[0]);
	oapi_recordings_EmitEvent(eOxMETA_RECORD_SCANDONE, nCount, 0, NULL);
}

static void		oapi_recordings_OnResponse (HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HxPair_t		*pair;
	HxReturnFunc_t	ret;
	HINT32			err;
	HxLOG_Debug("%s(argc:%d, userdata:0x%X) -hErr[%d]\n", __FUNCTION__, argc, userdata ,hErr);

	pair = (HxPair_t *)userdata;
	if (pair == NULL)
		return;

	ret  = (HxReturnFunc_t)pair->left;
	if (ret)
	{
		// 기존 OAPI RPC도 무조건 err가 ERR_OK로 오게 되어 있었음.
		// 임시로 ERR_OK로 두고 추후 손본다.
		if(hErr == ERR_OK)
		{
			ret(hErr, pair->right);
		}
		else
		{
			//Fileoperation 실패...
			err = HxOBJECT_INT(argv[0]);
			ret(err, pair->right);
		}
	}
	OxMEM_Free(pair);
}

void	OAPI_META_RECORD_Update (const DxRecListData_t *rec)
{
	void	*data;
	HUINT32	size;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Assert(rec);
	HxLOG_Debug("%s(%u.%s.\'%s\')\n", __FUNCTION__, rec->ulContentId, rec->szUrl, rec->szName);

	data = oapi_recordings_Pack(rec, &size);
	if (data)
	{
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_Update, "ib", rec->ulContentId, data, size);
		OxMEM_Free(data);
	}
}

void	OAPI_META_RECORD_Rename (HUINT32 cid, const HCHAR *name)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Assert(cid && name);
	HxLOG_Debug("%s(cid:%u, name:\'%s\')\n", __FUNCTION__, cid, name);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_Rename, "is", cid, name);
}

HINT32	OAPI_META_RECORD_Remove (HUINT32 cid, HxReturnFunc_t ret, void *userdata)
{
	HxPair_t	*pair = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HINT32	nRequestId = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Assert(cid);
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	if (ret)
	{
		pair = (HxPair_t *)OxMEM_Malloc(sizeof(HxPair_t));
		if (pair)
		{
			pair->left  = (void *)ret;
			pair->right = userdata;
		}
	}
	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_recordings_OnResponse, pair, RPC_OAPI_META_RECORD_Remove, "i", cid);

	return nRequestId;
}

HINT32	OAPI_META_RECORD_Move (HUINT32 cid, const HCHAR *path, HxReturnFunc_t ret, void *userdata)
{
	HxPair_t	*pair = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HINT32	nRequestId = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Assert(cid && path);
	HxLOG_Debug("%s(cid:%u, path:\'%s\')\n", __FUNCTION__, cid, path);

	if (ret)
	{
		pair = (HxPair_t *)OxMEM_Malloc(sizeof(HxPair_t));
		if (pair)
		{
			pair->left  = (void *)ret;
			pair->right = userdata;
		}
	}
	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_recordings_OnResponse, pair, RPC_OAPI_META_RECORD_Move, "is", cid, path);

	return nRequestId;
}

HINT32	OAPI_META_RECORD_Copy (HUINT32 cid, const HCHAR *path, HxReturnFunc_t ret, void *userdata)
{
	HxPair_t	*pair = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HINT32	nRequestId = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Assert(cid && path);
	HxLOG_Debug("%s(cid:%u, path:\'%s\')\n", __FUNCTION__, cid, path);

	if (ret)
	{
		pair = (HxPair_t *)OxMEM_Malloc(sizeof(HxPair_t));
		if (pair)
		{
			pair->left  = (void *)ret;
			pair->right = userdata;
		}
	}
	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_recordings_OnResponse, pair, RPC_OAPI_META_RECORD_Copy, "is", cid, path);

	return nRequestId;
}

void	OAPI_META_RECORD_CancelFileOperation (HUINT32 cid)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Assert(cid);
	HxLOG_Debug("%s(cid:%u)\n", __FUNCTION__, cid);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_CancelFileOperation, "i", cid);
}

void	OAPI_META_RECORD_Lock (HUINT32 cid, HBOOL lockOrNot)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Assert(cid);
	HxLOG_Debug("%s(cid:%u, lock:\'%s\')\n", __FUNCTION__, cid, lockOrNot ? "LOCK" : "UNLOCK");

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_Lock, "ii", cid, lockOrNot);
}

void	OAPI_META_RECORD_AddBookmark (HUINT32 cid, HUINT32 pos, const HCHAR *name)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Assert(cid && name);
	HxLOG_Debug("%s(cid:%u, pos:%u, name:\'%s\')\n", __FUNCTION__, cid, pos, name);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_AddBookmark, "iis", cid, pos, name);
}

void	OAPI_META_RECORD_RemoveBookmark (HUINT32 cid, HUINT32 pos, const HCHAR *name)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Assert(cid && name);
	HxLOG_Debug("%s(cid:%u, pos:%u, name:\'%s\')\n", __FUNCTION__, cid, pos, name);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_RemoveBookmark, "iis", cid, pos, name);
}

void	OAPI_META_RECORD_RemoveAll (void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Debug("%s()\n", __FUNCTION__);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_RECORD_RemoveAll, NULL);
}

void	OAPI_META_RECORD_SetListener(OxMetaRecord_Notifier_t listener, void *userdata)
{
	OxRecordingsMgr_t	*mgr;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();

	mgr = oapi_recordings_GetManager();
	if (listener)
	{
		if (mgr->listener == NULL)
		{
			mgr->listener = listener;
			mgr->userdata = userdata;

			HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_META_RECORD_OnUpdate, NULL, NULL, (HxRPC_Noti_t)oapi_recordings_OnUpdate);
			HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_META_RECORD_OnScanDone, "i", NULL, (HxRPC_Noti_t)oapi_recordings_OnScanDone);
		}
	}
	else
	{
		if (mgr->listener)
		{
			mgr->listener = NULL;
			mgr->userdata = NULL;
			// TODO:
			//OAPI_UnregisterCallback(RPC_OBAMA_META_RECORD_OnUpdate, oapi_recordings_OnUpdate);
		}
	}

	oapi_recordings_ReleaseManager(mgr);
}

#ifdef	CONFIG_DEBUG

static void test_oapi_recordings_Listener (
						OxMetaRecord_Event_e evt
						, HINT32 count
						, HUINT32 cid
						, const DxRecListData_t *rec
						, void *userdata
					)
{
	HxLOG_Debug("%s(%s, count:%d, cid:%u)\n", __FUNCTION__, oapi_recordings_EventToString(evt), cid);

}

void	OAPI_META_RECORD_Test (void)
{
	OAPI_META_RECORD_SetListener(test_oapi_recordings_Listener, NULL);

}
#endif

