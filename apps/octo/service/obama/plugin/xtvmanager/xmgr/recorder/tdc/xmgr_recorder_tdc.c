/**
	@file     	xmgr_recorder_tdc.c
	@brief    	recording control application for Tdc.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#if defined(CONFIG_OP_TDC)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>

#include <octo_common.h>

#include <db_svc.h>
#include <xdb_svc.h>
#include <isosvc.h>

#include <svc_pipe.h>
#include <svc_rec.h>
#include <xsvc_pvr.h>


#include <bus.h>

#include <mgr_action.h>
#include <mgr_storage.h>
#include <mgr_live.h>
#include "../local_include/_xmgr_recorder.h"
#include "../base/local_include/_xmgr_recorder_base.h"
#include "../../schedule/local_include/_xmgr_schedule.h"
#include <xmgr_cas.h>
#include <mgr_copyrightcontrol.h>
#include <svc_sys.h>
#include <svc_cas.h>

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
#define _________LOCAL_FUNC__________
STATIC HERROR xmgr_rec_ConvertUtf8ToByte_Tdc(HUINT8 *pszEvtName, HUINT8 *pszFileName)
{
	HINT32 i=0;
	const HINT32 BUFF_LEN = 256;
	HUINT8	szChangedString[BUFF_LEN];

	HxSTD_MemSet(&szChangedString, 0x00, sizeof(HUINT8)*BUFF_LEN);
	HLIB_STD_StrUtf8NCpy ( szChangedString,
						pszEvtName,
						MAX_REC_FILE_NAME_LEN-1);

	/* delete space in first character */
	for( i=0  ; i<BUFF_LEN ; i++ )
	{
		if( HxSTD_IsWhiteSpace( szChangedString[i] ) )
			continue;
		else break;
	}


	HLIB_STD_StrUtf8NCpy(pszFileName, &szChangedString[i], MAX_REC_FILE_NAME_LEN-1);
	return ERR_OK;
}

#define _____MSG_PROCESSING_FUNCTIONS_____

#define _____PROTO_PROCEDURE_____

#define _________PLUGIN_FUNC__________

HERROR xmgr_recorder_CreateRecordFailReason_Tdc (MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule)
{
	HUINT32			 ulContentId;
	UNIXTIME		 utStartTime;
	HCHAR			 szFileName[FS_MAX_FILE_NAME_LEN];
//	DxSchedule_t	 stSchedule;
	SvcMeta_Record_t	 stMetaInfo;
	HERROR			 hErr;
	DxRSV_TvAnyTimeRecord_t	*pstTvaRecord = NULL;

	if (pstRecSetup == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("invalid param. setup(0x%x) schedule(0x%x) \n",pstRecSetup, pstSchedule);
		return ERR_FAIL;
	}

	if(MAX_CONNECTABLE_PVR_STORAGE_NUM == 0 || 
		SVC_FS_GetDefaultPvrIdx() >= MAX_CONNECTABLE_PVR_STORAGE_NUM)
	{
		// YS-1000과 같이 HDD가 없는 경우. 기록하지 않음.
		HxLOG_Error("No HDD or invalid PvrIdx\n");
		return ERR_FAIL;
	}
	pstTvaRecord = &pstSchedule->uExtInfo.stRec.ex.tva;

	HxLOG_Warning("schedule slot(%d), fail reason(%d) \n",pstSchedule->ulSlot,eEndReason);

	/* Recording 이 시작하기 전에 hErr 가 발생한 것을 meta 파일로 만든다.*/
	HxLOG_Debug("eEndReason: %d \n", eEndReason);
	HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcMeta_Record_t));

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSchedule->stStartTime), &utStartTime);

	stMetaInfo.ulStartTime		= utStartTime - pstSchedule->uExtInfo.stRec.ulPreOffsetTime;
	stMetaInfo.ulEndTime			= utStartTime + 1;
	stMetaInfo.ucEndCondition	= (HUINT8)eEndReason;
	stMetaInfo.ucRecordStatus	= eMgrRec_STS_INCOMPLETED;
	stMetaInfo.nEventId			= pstSchedule->uExtInfo.stRec.nEvtId;
	if ((DxRSVTYPE_RECORD_TBR != pstSchedule->eRsvType) && (DxRSVTYPE_RECORD_SATRECORD_TBR != pstSchedule->eRsvType))
	{
		stMetaInfo.u2cEventBase	= TRUE;
	}

	HxLOG_Info ("evtName(%s) evtId(0x%04x)\n", pstSchedule->uExtInfo.stRec.szEventName, pstSchedule->uExtInfo.stRec.nEvtId);
	snprintf(stMetaInfo.aucPath, FS_MAX_FILE_NAME_LEN, "%s", (char *)xmgr_recorder_GetRecordPath_Base(pstRecSetup->ulPvrIdx));

	xmgr_rec_ConvertUtf8ToByte_Tdc (pstSchedule->uExtInfo.stRec.szEventName, szFileName);
	HLIB_STD_StrUtf8NCpy (stMetaInfo.aucDisplayEvtName, szFileName, PVR_MAX_TITLE_LEN - 1);

	if (pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR || pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
	{
		if (pstTvaRecord->ucCRIDType == DxCRID_TYPE_PROG)
		{
			stMetaInfo.stCrid[0].ucType = DxCRID_TYPE_PROG;
			HxSTD_snprintf(stMetaInfo.stCrid[0].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s", pstTvaRecord->szCRID);
		}
		else if (pstTvaRecord->ucCRIDType == DxCRID_TYPE_SERIES)
		{
			stMetaInfo.bSeriesRec = TRUE;

			stMetaInfo.stCrid[0].ucType = DxCRID_TYPE_SERIES;
			HxSTD_snprintf(stMetaInfo.stCrid[0].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s", pstTvaRecord->szCRID);
			if (!(HxSTD_StrEmpty(pstTvaRecord->szProgCRID)))
			{
				stMetaInfo.stCrid[1].ucType = DxCRID_TYPE_PROG;
				HxSTD_snprintf(stMetaInfo.stCrid[1].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s",pstTvaRecord->szProgCRID);
			}

			if (!(HxSTD_StrEmpty(pstTvaRecord->szSeriesTitle)))
			{
				HxSTD_snprintf(stMetaInfo.aucSeriesTitle, DxMAX_SERIES_TITLE_LEN, "%s",pstTvaRecord->szSeriesTitle);
			}
		}
	}

	hErr = SVC_META_MakeFileName (stMetaInfo.ulStartTime, szFileName, stMetaInfo.aucFileName, HxPATH_MAX);
	hErr = SVC_META_AddContentsInfo (pstRecSetup->hCurrSvc, &stMetaInfo, &ulContentId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PVR_AddContentsInfo err: SVCHandle:(0x%08x)\n", pstRecSetup->hCurrSvc);
		return ERR_FAIL;
	}

	BUS_PostMessage(NULL, eSEVT_META_UPDATED, (Handle_t)HANDLE_NULL, ulContentId, 1, 0);
	return ERR_OK;
}
#endif
