/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_fs.h>
#include <svc_pb.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_sys.h>
#include <get_enum_str.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>
#include <mgr_pg.h>
#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <mgr_svclist.h>
#include <mgr_live.h>
#include <mgr_playback.h>
#include <_xmgr_playback.h>
#include "../local_include/_xmgr_playback_base.h"

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <svc_output.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define _____LOCAL_FUNCTIONS_____

STATIC BUS_Result_t xmgr_playback_VerimatrixMsgBusCreate_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	SvcMeta_Record_t	stRecordInfo;
	HERROR				hErr = ERR_FAIL;

	// call default base function
	eBusResult = xmgr_playback_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);

	// set vmx drm
	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));
	hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecordInfo);
	if (hErr == ERR_OK)
	{
		if(stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_Verimatrix)
		{
			HxSTD_MemCopy(&pstContext->stDrmInfo, &stRecordInfo.stDrmInfo, sizeof(DxCopyrightControl_t));

			// xmgr_playback_SetCopyProtection()에서 bCasCc값을 bDrmValid 값으로 덮어쓰기 때문에 여기서 이렇게 설정해 주어야 함
			pstContext->bDrmValid = pstContext->stDrmInfo.bCasCc;
		}
	}

	return eBusResult;
}

STATIC BUS_Result_t xmgr_playback_VerimatrixMsgBusDestroy_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->hAction != hAction)
	{
		HxLOG_Warning("pstContext->hAction (%x), hAction (%x)\n", pstContext->hAction, hAction);
		hAction = pstContext->hAction;
	}

	return xmgr_playback_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_playback_VerimatrixMsgEvtCasDrmSignalled_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t	stDrmInfo;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if ((void *)p3 == NULL)
	{
		HxLOG_Error("Invalid parameter! \n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (const void *)p3, sizeof(DxCopyrightControl_t));

	if ((stDrmInfo.bCasCc != TRUE) || (stDrmInfo.stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Verimatrix))
	{
		HxLOG_Print("No verimatrix drm info! bCasCc(%d) bCasCc(%d) \n", stDrmInfo.bCasCc, stDrmInfo.stCasCcInfo.eCasCcType);
		return MESSAGE_PASS;
	}

	pstContext->bDrmValid = TRUE;
	HxSTD_MemCopy(&pstContext->stDrmInfo.stCasCcInfo, &stDrmInfo.stCasCcInfo, sizeof(DxCopyrightControl_CasCcInfo_t));
	xmgr_playback_SetCopyProtection(pstContext);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

#define _____PROTO_PROCEDURE_____

STATIC BUS_Result_t xproc_playback_VerimatrixProto_Base(xmgrPbContext_t* pstContext, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult	= ERR_BUS_NO_ERROR;
	HUINT32			ulActionId	= SVC_PIPE_GetActionId(hAction);
	BUS_Callback_t	pfBase;

	// 특별히 처리할 것들만 여기서 처리하고 나머지는 base proc 에 맡긴다
	switch (nMessage)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Warning ("***********************************************************\n");
		HxLOG_Warning ("*  XMGR : VERIMATRIX : PLAYBACK : CREATE (%d)\n", ulActionId);
		HxLOG_Warning ("***********************************************************\n");

		HxLOG_Info("[MGR:PbVmx(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
		eBusResult = xmgr_playback_VerimatrixMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Info("[MGR:PbVmx(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
		eBusResult = xmgr_playback_VerimatrixMsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

		HxLOG_Warning ("***********************************************************\n");
		HxLOG_Warning ("*  XMGR : VERIMATRIX : PLAYBACK : DESTROY (%d)\n", ulActionId);
		HxLOG_Warning ("***********************************************************\n");
		break;

	case eSEVT_CAS_DRM_SIGNALLED:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbVmx(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
		eBusResult = xmgr_playback_VerimatrixMsgEvtCasDrmSignalled_Base(pstContext, hAction, p1, p2, p3);
		break;

	default :
		break;
	}

	if (eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	// 여기서 처리하지 않은 것은 base proc에게 보내자
	pfBase = xmgr_playback_GetProcFunc_Base(pstContext->ulActionId);

	return pfBase(nMessage, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(0);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(1);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction2_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(2);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction3_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(3);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction4_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(4);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction5_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(5);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction6_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(6);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_VerimatrixAction7_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(7);
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xproc_playback_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____

BUS_Callback_t xmgr_playback_VerimatrixGetProcFunc_Base(HUINT32 ulActionId)
{
	HUINT32	ulPbIndex	= 0;

	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		ulPbIndex = (ulActionId - eActionId_VIEW_FIRST);
	}
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
	{
		ulPbIndex = (ulActionId - eActionId_BG_FIRST) + NUM_VIEW_ACTION;
	}

	if (ulPbIndex >= MAX_NUM_OF_PLAY)
	{
		HxLOG_Error("Wrong Action ID:(%d)\n", ulActionId);
		return NULL;
	}

	switch (ulPbIndex)
	{
	case 0:
		return xproc_playback_VerimatrixAction0_Base;

	case 1:
		return xproc_playback_VerimatrixAction1_Base;

	case 2:
		return xproc_playback_VerimatrixAction2_Base;

	case 3:
		return xproc_playback_VerimatrixAction3_Base;

	case 4:
		return xproc_playback_VerimatrixAction4_Base;

	case 5:
		return xproc_playback_VerimatrixAction5_Base;

	case 6:
		return xproc_playback_VerimatrixAction6_Base;

	case 7:
		return xproc_playback_VerimatrixAction7_Base;

	default:
		break;
	}

	return NULL;
}

/* end of file */
