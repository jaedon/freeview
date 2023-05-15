/**
	@file     	xmgr_recorder_freesat.c
	@brief    	recording control application for UK DTT.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#if defined(CONFIG_OP_FREESAT)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>

#include <octo_common.h>

#include <db_svc.h>

#include <isosvc.h>

#include <svc_si.h>
#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_pipe.h>
#include <svc_epg.h>
#include <svc_sys.h>

#include <bus.h>

#include <mgr_action.h>
#include <mgr_live.h>
#include "../local_include/_xmgr_recorder.h"

//#include "pvr_int_common.h"
//#include "xsvc_rec_fsat.h"

#include <xsvc_pvr.h>


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

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

BUS_Result_t		local_record_fsat_preSetup(xmgrRec_Context_t *context, HUINT32 actionId, HUINT8 recIdx)
{
	BUS_Result_t	ret = ERR_BUS_NO_ERROR;
/*
	HERROR			err = ERR_OK;
	MgrRec_RecSetup_t	*setupInfo = NULL;
	RSV_Item_t		rsvItem;
	char			*recPath = xmgr_pvr_GetRecordPath();
	HUINT8			*pszChangedString = NULL;
	HINT32			nSrcLen = 0, nUsedByte = 0, nUtf8Len = 0;
	eHxCP_CodePage_e	eEncoding = eHCodepage_MAX;

	(void)context;

	// refer to ap_recorder's setup info directly since this process done on app's creation
	setupInfo = xmgr_pvr_GetRecSetup(recIdx);
	if (setupInfo != NULL)
	{
		HxLOG_Info("[FSAT] recording type : %d\n", setupInfo->eRecType);

		// check recording type if this is series recording or not.
		// Typical EBR or TBR would be setup by base procedure.
		if (setupInfo->eRecType == eMgrRecType_SBR)
		{
			err = MW_RSV_ReadReservedItem(setupInfo->ulSlot, &rsvItem);
			if (err != ERR_OK)
			{
				HxLOG_Info("[FSAT] no reservation info : slot(%d)\n", setupInfo->ulSlot);
				return ret;
			}
			HxLOG_Info("[FSAT] Series recording preprocess - rsvSlot(%d) event(%s)\n",
						setupInfo->ulSlot, rsvItem.ext.stRec.szFPBRecPath);

			// pre-process for series recording
			pszChangedString = local_record_fsat_GetTextBuffer();

			nSrcLen = (HINT32)MWC_UTIL_DvbStrlen(rsvItem.ext.stRec.szFPBRecPath);
			eEncoding = MWC_UTIL_GetEncodingByLocaleCode((const CH_1BYTE *)rsvItem.ext.stRec.szFPBRecPath);
			MWC_UTIL_ConvertEncodingToUtf8(eEncoding, rsvItem.ext.stRec.szFPBRecPath, nSrcLen,
								256, &nUsedByte, pszChangedString, &nUtf8Len);

			SVC_PVR_SetSeriesMetaInfo(context->ulActionId, recIdx, recPath, pszChangedString);
		}
	}
*/
	return ret;
}

HERROR			xmgr_rec_UpdateCopyControl_FreeSat(xmgrRec_Context_t *pContext, HINT32	evtId, XsvcPvr_FtaContentMgmtState_e copyState)
{
	HERROR			ret = ERR_OK;
	SvcMeta_Record_t	recInfo;
	HBOOL			updateFtaValue = FALSE;

	if (pContext != NULL)
	{
		if (pContext->stRecSetup.eRecType == eMgrRecType_EBR || pContext->stRecSetup.eRecType == eMgrRecType_SBR)
		{	/* compare the base event id and given one of FTA changes */
			if (pContext->usBaseEventId == evtId)
			{
				updateFtaValue = TRUE;
				HxLOG_Info("EIT(%d) that FTA changes is same to the base one.\n", evtId);
			}
			else
			{
				updateFtaValue = FALSE;
				HxLOG_Info("EIT(%d)'s FTA changes, but ignored.\n", evtId);
			}
		}
		else
		{	/* override FTA state */
			updateFtaValue = TRUE;
		}

		if (updateFtaValue)
		{
			switch (copyState)
			{
				/* free copy */
				case eCONTENT_MGMT_STATE_FreeAcc:
				case eCONTENT_MGMT_STATE_FreeAcc_NoEnc:
					break;

				/* 1 copy possible */
				case eCONTENT_MGMT_STATE_ManagedCopy:
				/* no more copy */
				case eCONTENT_MGMT_STATE_Reserved:
					ret = SVC_META_GetRecordInfo(pContext->ulContentId, &recInfo);
					if (ret != ERR_OK)
					{
						HxLOG_Info("error to get event(contentId:%d) info.\n", pContext->ulContentId);
						return ret;
					}
					if (copyState == eCONTENT_MGMT_STATE_ManagedCopy)
					{
						HxLOG_Info("set the content(%d) as COPY ONCE.\n", pContext->ulContentId);
						recInfo.ucCopyTime = AVAILABLE_CONTENT_COPY_NUMBER;
					}
					else
					{
						HxLOG_Info("set the content(%d) as NO COPY.\n", pContext->ulContentId);
						recInfo.ucCopyTime = 0;
					}
					SVC_META_SetRecordInfo(pContext->ulContentId, &recInfo);
					break;

				case eCONTENT_MGMT_STATE_None:
					break;
			}
		}
	}

	return ERR_OK;
}

#if 0 //defined but not used
STATIC HUINT8 *local_record_fsat_GetTextBuffer (void)
{
	HxSTD_memset (s_aucRecFsat_TextBuffer, 0, 256);
	return s_aucRecFsat_TextBuffer;
}
#endif


BUS_Result_t 	xproc_pvr_RecordProc_Freesat(xmgrRec_Context_t *context,
									HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			ret = ERR_BUS_NO_ERROR;
	XsvcPvr_FtaContentMgmtState_e	copyControl;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			//ret = local_record_fsat_preSetup(context, ulActionId, ucRecIdx);
			break;

		case XEVT_PVR_NOTIFY_FTA_CONTENT_MANAGEMENT_CHANGED:
			copyControl = (XsvcPvr_FtaContentMgmtState_e)p2;
			HxLOG_Info("FTA_CONTENT_MANAGEMENT : evtid : (%d), copy_control(%d)\n", p1, copyControl);
			xmgr_rec_UpdateCopyControl_FreeSat(context, (HUINT16)p1, copyControl);
			return MESSAGE_BREAK;

		default:
			break;
	}

	if (ret != ERR_BUS_NO_ERROR)
		return ret;

	return xproc_pvr_RecordProtoAction(context, message, hAction, p1, p2, p3);
}
#endif
