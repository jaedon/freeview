/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_si.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include <db_svc.h>
#include <pal_pipe.h>
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"
#include "ir_svc.h"
#include "ir_msg_scell.h"
#include "ir_api.h"
#include "ir_evt.h"
#include "ir_util.h"

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
#include "ir_sol.h"
#endif
#include "ir_fta_block.h"

#include "util.h" //for api :get 12 bit."


/********************************************************************
*	Definitions (for this source file)
********************************************************************/






#if 0
#define	PrintCAT					HxLOG_Print
#define	PrintPMT					HxLOG_Print
#define	PrintNewEMM				HxLOG_Print
#else
#define	PrintCAT(...)
#define	PrintPMT(...)
#define	PrintNewEMM(...)
#endif


#ifdef	_____ECM_EMM_MONITORING_____
	#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
		extern HUINT32	s_EmmMonitorCount[CAS_MAX_NUM_OF_SC];
	#endif
#endif

/********************************************************************
*	Global variables
********************************************************************/
extern	IR_SERVICE_INFO		g_IrSvcInfo[];
extern	IR_TP_INFO			g_IrTpInfo[];
extern	IR_DRV_SC_INFO		g_IrDrvScInfo[];


extern	HUINT32	CAS_IR_SVC_AddIrTpInfo(Handle_t hAction, HUINT16 usUniqueId, HUINT32 ulLength, HUINT8 *pucRawData);
extern	void	CAS_IR_SVC_UpdateIrTpInfo(void);

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
extern HVD_Result  HUMAX_VD_SC_GetCardPhysicalStatus(HUINT8 ucSlotID, HBOOL *pSCInserted);
#endif

#define _______local_Functions___________________________________________________________

#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
/********************************************************************************
********************************************************************************/
static	void	local_cas_ir_Drv_PMT_NotifySCStatusToApp()
{
	HMSG		msg={0,};
	ia_status_st status;
	HUINT8	CharSeverity, ucService;

	// Card Slot 별로 던져 주는 처리 필요
	status.eSeverity = g_IrDrvScInfo[0].stIrScApiInfo.stStatus.eSeverity;
	status.eSource = g_IrDrvScInfo[0].stIrScApiInfo.stStatus.eSource;
	status.wStatus = g_IrDrvScInfo[0].stIrScApiInfo.stStatus.usStatus;

	if(status.eSource != 4 ) // source 4 == card status
		return;

	msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_GLOBAL;
	msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
	CharSeverity = SEVERITY_TO_CHAR(status.eSeverity);
	msg.ulParam = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
	msg.unInfo.aulL[0] = 0;

	for(ucService = 0; ucService < IR_HUMAX_MAX_SERVICE; ucService++ )
	{
		if( g_IrSvcInfo[ucService].hIrSvc != HANDLE_NULL )
		{
			CAS_IR_EVT_SendEvent(g_IrSvcInfo[ucService].hAction, &msg);
		}
	}
}
#endif


#define _______Parse_Functions___________________________________________________________

/********************************************************************************
********************************************************************************/
HUINT8	*CAS_IR_SI_ParseCAT(HUINT8	*pCatRaw, HINT32 nDmxId, HUINT16 *Length)
{
	msg_emm_source_st	MsgEmmSource;
	HUINT8				*pMsgEmmSource;
	msg_resource_st		MsgRsc;
	HUINT8				*pTemp;
	HUINT8				*p;
	HUINT8				*pEMMRscData = NULL;
	HUINT16				TotalLength = 0;

	HUINT8				ucTableId, ucSecSyntaxIndicator;
	HUINT16				wSecLen;
	HUINT8				ucTemp;
	HUINT8				ucVerNum, ucCurNextIndicator, ucSecNum, ucLastSecNum;
	HUINT8				ucGlobalDescCount;
	HUINT8				ucDesTag, ucDesLen;

	HxLOG_Info("\n");

	pEMMRscData = (HUINT8 *)OxCAS_Malloc(1024 + sizeof(msg_resource_st));	// Max로 잡아놓고 시작.
	if (pEMMRscData == NULL)
	{
		HxLOG_Critical("\n\n");
		return (HUINT8 *)NULL;
	}
	pTemp = pEMMRscData;

	MsgRsc.wResourceType = RESOURCE_EMM_SOURCE;
	MsgRsc.wResourceId = (vd_rsc_id)nDmxId;
	HxLOG_Print("MsgRsc.wResourceId = %d\n", (HINT32) MsgRsc.wResourceId);

	MsgRsc.wResourceMgnt = MANAGE_UPDATE;
	HxSTD_memcpy(pTemp,(HUINT8 *)&MsgRsc, sizeof(msg_resource_st));
	pTemp += sizeof(msg_resource_st);
	TotalLength += sizeof(msg_resource_st);

	p = pCatRaw;


	// table id(1byte)
	ucTableId = *p++;
	PrintCAT("\t[CAT] table id = 0x%x\n", ucTableId);

	// section syntax indicator & section length(2byte)
	ucSecSyntaxIndicator =(*p & 0x80) >> 7;
	wSecLen =(HUINT16)get12bit(p);
	p += 2;
	PrintCAT("\t[CAT] section syntax indicator = 0x%x\n", ucSecSyntaxIndicator);
	PrintCAT("\t[CAT] section length = %d\n", wSecLen);
	if (wSecLen > 1024)
	{
		HxLOG_Critical("\n\n");
		if( pEMMRscData != NULL )
			OxCAS_Free(pEMMRscData);

		return (HUINT8 *)NULL;
	}

	wSecLen -= 4;	// CRC 4byte를 미리 뺀다.

	// reserved & version number & current next indicator(3byte)
	p += 2;
	wSecLen -= 2;

	ucTemp = *p++;
	wSecLen--;
	ucVerNum =(ucTemp & 0x3e) >> 1;
	ucCurNextIndicator = ucTemp & 0x01;
	PrintCAT("\t[CAT] version number = 0x%x\n", ucVerNum);
	PrintCAT("\t[CAT] current next indicator = 0x%x\n", ucCurNextIndicator);

	// section number(1byte)
	ucSecNum = *p++;
	wSecLen--;
	PrintCAT("\t[CAT] section number = 0x%x\n", ucSecNum);

	// last section number(1byte)
	ucLastSecNum = *p++;
	wSecLen--;
	PrintCAT("\t[CAT] last section number = 0x%x\n", ucLastSecNum);

	// pTemp가 msg_emm_source_st를 써야하는 단계지만, 아직 bDescrCount를 모르므로.. 일단 pointer는 pMsgEmmSource에 적어두고.
	pMsgEmmSource = pTemp;
	pTemp += sizeof(msg_emm_source_st);
	TotalLength += sizeof(msg_emm_source_st);

	HxSTD_memcpy(pTemp, p, wSecLen);
	pTemp += wSecLen;
	TotalLength += wSecLen;

	ucGlobalDescCount = 0;
	while(wSecLen > 0 && wSecLen < 1024)
	{
		ucDesTag = p[0];
		ucDesLen = p[1];
		PrintCAT("\t\t[CAT] ucDesTag = 0x%x\n", ucDesTag);
		PrintCAT("\t\t[CAT] ucDesLen = %d\n", ucDesLen);
		ucGlobalDescCount++;

		p +=(ucDesLen+2);
		wSecLen -=(ucDesLen+2);
	}

	MsgEmmSource.bDescrCount = ucGlobalDescCount;
	HxSTD_memcpy(pMsgEmmSource, &MsgEmmSource, sizeof(msg_emm_source_st));

#ifdef IR_DEBUG_SI
{
	HUINT32	i;

	IrSiDump("[pEMMRscData : Length(0x%X)]\n", TotalLength);
	for( i = 0; i < TotalLength; i++ )
	{
		IrSiDump("%02X ", pEMMRscData[i]);
		if( i % 16 == 15 )
			IrSiDump("\n");
	}
	IrSiDump("\n");
}
#endif

	*Length = TotalLength;

	return pEMMRscData;
}

/********************************************************************************
********************************************************************************/
HUINT8	*CAS_IR_SI_ParsePMT(HUINT8 index, HUINT8* pPmtRaw, Handle_t hAction, HUINT16 *Length)
{
	msg_resource_st MsgRsc;
	HUINT8			*p;
	HUINT16 		TotalLength = 0;
	HUINT8			*pECMRscData = NULL;
	HUINT8			*pTemp;

	HUINT8				ucTableId, ucSecSyntaxIndicator, ucTemp;
	HUINT16 			wSecLen, wProgNum;
	HUINT8				ucVerNum, ucCurNextIndicator, ucSecNum, ucLastSecNum;
	HUINT16 			wPcrPid, wProgInfoLen;
	HUINT8				ucDesTag, ucDesLen, ucGlobalDescCount;
	msg_ecm_source_st	MsgEcmSource;
	HUINT8				*pMsgEcmSource;
	HUINT8				ucStreamType, ucStreamCount, ucStreamDescCount;
	HUINT16 			wElemPid, wEsInfoLen;
	msg_dvb_stream_st	MsgDvbStream;
	msg_dvb_stream_st	*pMsgDvbStream;
	HUINT8				IsGlobalCADescExist = 0;
	HUINT8				IsStreamCADescExist = 0;
	HUINT8				IsIrdetoCADescExist = 0;
	HUINT16 			usCaSystemID = 0;
	HMSG				msg;
	HUINT8				i;

	HxLOG_Info("index(0x%X)\n", index);

	HxSTD_memset(&msg, 0, sizeof(HMSG));

	if (index >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Error("SvcIndex >= IR_HUMAX_MAX_SERVICE\n");
		return (HUINT8 *)NULL;
	}

	if(  TRUE != g_IrSvcInfo[index].ucIsValidPMT)
	{
		HxLOG_Error("local_IrDrv_ParsePMT : PMT is not valid yet.\n");
		return (HUINT8 *)NULL;
	}

	pECMRscData = (HUINT8 *)OxCAS_Calloc(1024 + sizeof(msg_resource_st));	// Max로 잡아놓고 시작.
	pTemp = pECMRscData;

	if( pECMRscData == NULL )
	{
		HxLOG_Critical("\n\n");
		return (HUINT8 *)NULL;
	}

	HxSTD_memset(&MsgRsc, 0, sizeof(msg_resource_st));
	HxSTD_memset(&MsgEcmSource, 0, sizeof(msg_ecm_source_st));
	HxSTD_memset(&MsgDvbStream, 0, sizeof(msg_dvb_stream_st));

	MsgRsc.wResourceType = RESOURCE_ECM_SOURCE;
	// TODO:check the type. jichoi
	MsgRsc.wResourceId = (vd_rsc_id) g_IrSvcInfo[index].ucDemuxId;
	MsgRsc.wResourceMgnt = MANAGE_UPDATE;

	HxSTD_memcpy(pTemp, &MsgRsc, sizeof(msg_resource_st));
	pTemp += sizeof(msg_resource_st);
	TotalLength += sizeof(msg_resource_st);

	p = pPmtRaw;


	// table id(1byte)
	ucTableId = *p++;
	PrintPMT("\t[PMT] table id = 0x%x\n", ucTableId);

	// section syntax indicator & section length(2byte)
	ucSecSyntaxIndicator =(*p & 0x80) >> 7;
	wSecLen =(HUINT16)get12bit(p);
	p += 2;
	PrintPMT("\t[PMT] section syntax indicator = 0x%x\n", ucSecSyntaxIndicator);
	PrintPMT("\t[PMT] section length = %d\n", wSecLen);
	if (wSecLen > 1024)
	{
		HxLOG_Critical("\n\n");

		if( pECMRscData != NULL )
			OxCAS_Free(pECMRscData);
		return (HUINT8 *)NULL;
	}

	wSecLen -= 4;	// CRC 4byte를 미리 뺀다.

	// program number(2byte)
	wProgNum =(HUINT16)get16bit(p);
	p += 2;
	wSecLen -= 2;
	PrintPMT("\t[PMT] program number = 0x%x\n", wProgNum);

	// version number & current next indicator(1byte)
	ucTemp = *p++;
	wSecLen--;
	ucVerNum =(ucTemp & 0x3e) >> 1;
	ucCurNextIndicator = ucTemp & 0x01;
	PrintPMT("\t[PMT] version number = 0x%x\n", ucVerNum);
	PrintPMT("\t[PMT] current next indicator = 0x%x\n", ucCurNextIndicator);

	// section number(1byte)
	ucSecNum = *p++;
	wSecLen--;
	PrintPMT("\t[PMT] section number = 0x%x\n", ucSecNum);

	// last section number(1byte)
	ucLastSecNum = *p++;
	wSecLen--;
	PrintPMT("\t[PMT] last section number = 0x%x\n", ucLastSecNum);

	// PCR pid(2byte)
	wPcrPid =(HUINT16)get13bit(p);
	p += 2;
	wSecLen -= 2;
	PrintPMT("\t[PMT] pcr pid = 0x%x\n", wPcrPid);

	// program info length(2byte)
	wProgInfoLen =(HUINT16)get12bit(p);
	p += 2;
	wSecLen -= 2;
	PrintPMT("\t[PMT] program info length = %d\n", wProgInfoLen);

	// pTemp가 msg_ecm_source_st를 써야하는 단계지만, 아직 bStreamCount를 모르므로.. 일단 pointer는 pMsgEcmSource에 적어두고.
	pMsgEcmSource = pTemp;
	pTemp += sizeof(msg_ecm_source_st);
	TotalLength += sizeof(msg_ecm_source_st);

	HxSTD_memcpy(pTemp, p, wProgInfoLen);
	pTemp += wProgInfoLen;
	TotalLength += wProgInfoLen;

	ucGlobalDescCount = 0;
	while(wProgInfoLen > 0 && wProgInfoLen < 4096)
	{
		ucDesTag = p[0];
		ucDesLen = p[1];
		PrintPMT("\t\t[PMT] ucDesTag = 0x%X\n", ucDesTag);
		PrintPMT("\t\t[PMT] ucDesLen = %d\n", ucDesLen);
		ucGlobalDescCount++;

		if( ucDesTag == 0x09/*TAG_CA*/)
		{
			if(IsIrdetoCADescExist ==0 ) // protection code for irdeto descriptor upper located.
			{
				IsGlobalCADescExist = 1;
				usCaSystemID = get16bit(p+2);
				PrintPMT("\t\t[PMT] usCaSystemID = 0x%X\n", usCaSystemID);
				if( (IR_CA_SYSTEM_ID_MIN <= usCaSystemID) && (usCaSystemID <= IR_CA_SYSTEM_ID_MAX) )
				{
					g_IrSvcInfo[index].eSvcCasKind = eIrDrvCaType_irdeto;
					IsIrdetoCADescExist = 1;
				}
				else
				{
					if(g_IrSvcInfo[index].eSvcCasKind != eIrDrvCaType_irdeto)
						g_IrSvcInfo[index].eSvcCasKind = eIrDrvCaType_Other;
				}
			}
		}

		p +=(ucDesLen+2);
		wProgInfoLen -=(ucDesLen+2);
		wSecLen -=(ucDesLen+2);
	}

	MsgEcmSource.bGlobalCount = ucGlobalDescCount;

	// ucTypeFromPlay가 CASMGR_NONE이 아닌 것이 하나라도 있으면 set pid가 되었다는 말이므로 service define을 보내고, 그렇지 않으면 set pid할 때 다시 보낸다.
	for( i = 0; i < IR_SVC_MAX_PID; i++ )
	{
		if( g_IrSvcInfo[index].astPidList[i].ucTypeFromPlay != (HUINT8)eEsType_None )
			break;
	}

	if( i == IR_SVC_MAX_PID )
	{
		HxLOG_Error("[PMT(ERROR)] No PID was started!!\n");
		if( pECMRscData != NULL )
			OxCAS_Free(pECMRscData);
		return (HUINT8 *)NULL;
	}

	ucStreamCount = 0;
	while(wSecLen > 0 && wSecLen < 4096)
	{
		// stream type(1byte)
		ucStreamType = *p++;
		wSecLen--;
		PrintPMT("\t\t[PMT] stream type = 0x%x\n", ucStreamType);

		// elementary pid(2byte)
		wElemPid =(HUINT16)get13bit(p);
		p += 2;
		wSecLen -= 2;
		PrintPMT("\t\t[PMT] elementary pid = 0x%x\n", wElemPid);
		MsgDvbStream.wPid = wElemPid;

		for( i = 0; i < IR_SVC_MAX_PID; i++ )
		{
			if( g_IrSvcInfo[index].astPidList[i].usPID == wElemPid )
				break;
		}

		if( i == IR_SVC_MAX_PID )
		{
			// PMT의 wElemPid가 PidList에 없으므로 skip!!

			// ES info length(2byte)
			wEsInfoLen =(HUINT16)get12bit(p);
			p += 2;
			wSecLen -= 2;
			PrintPMT("\t\t[PMT] ES info length = %d\n", wEsInfoLen);

			while(wEsInfoLen > 0 && wEsInfoLen < 4096)
			{
				ucDesTag = p[0];
				ucDesLen = p[1];
				PrintPMT("\t\t\t[PMT] ucDesTag = 0x%x\n", ucDesTag);
				PrintPMT("\t\t\t[PMT] ucDesLength = %d\n", ucDesLen);

				p +=(ucDesLen+2);
				wEsInfoLen -=(ucDesLen+2);
				wSecLen -=(ucDesLen+2);
			}
		}
		else
		{
			// PMT의 wElemPid가 PidList에 있다.
			switch(ucStreamType)
			{
				case ISO_VIDEO_1 :
				case ISO_VIDEO_2 :
				case ISO_VIDEO_4 :
					g_IrSvcInfo[index].astPidList[i].ucTypeFromPMT = eEsType_Video;
					break;

				case ISO_AUDIO_1 :
				case ISO_AUDIO_2 :
				case ISO_AUDIO_AAC :
				case ISO_AUDIO_4 :
					g_IrSvcInfo[index].astPidList[i].ucTypeFromPMT = eEsType_Audio;
					break;

				case ISO_PRIVDATA_1 :
				case ISO_PRIVDATA_2 :
					g_IrSvcInfo[index].astPidList[i].ucTypeFromPMT = eEsType_Data;
					break;

				default:
					HxLOG_Print(": ITU-T, ISO/IEC Reserved Or, User Priveate	type(0x%X).\n", ucStreamType);
					g_IrSvcInfo[index].astPidList[i].ucTypeFromPMT = eEsType_Data;
					break;
			}

			ucStreamCount++;

			// ES info length(2byte)
			wEsInfoLen =(HUINT16)get12bit(p);
			p += 2;
			wSecLen -= 2;
			PrintPMT("\t\t[PMT] ES info length = %d\n", wEsInfoLen);

			// pTemp가 msg_dvb_stream_st를 써야하는 단계지만, 아직 bDescCount를 모르므로.. 일단 pointer는 pMsgDvbStream에 적어두고.
			pMsgDvbStream = (msg_dvb_stream_st *)pTemp;
			pTemp += sizeof(msg_dvb_stream_st);
			TotalLength += sizeof(msg_dvb_stream_st);

			HxSTD_memcpy(pTemp, p, wEsInfoLen);
			pTemp += wEsInfoLen;
			TotalLength += wEsInfoLen;

			ucStreamDescCount = 0;
			usCaSystemID = 0;
			IsIrdetoCADescExist =0;
			while(wEsInfoLen > 0 && wEsInfoLen < 4096)
			{
				ucDesTag = p[0];
				ucDesLen = p[1];
				PrintPMT("\t\t\t[PMT] ucDesTag = 0x%x\n", ucDesTag);
				PrintPMT("\t\t\t[PMT] ucDesLength = %d\n", ucDesLen);
				ucStreamDescCount++;

				if( ucDesTag == 0x09/*TAG_CA*/)
				{
					if(IsIrdetoCADescExist ==0 ) // protection code for irdeto descriptor upper located.
					{
						IsStreamCADescExist = 1;
						usCaSystemID = get16bit(p+2);
						PrintPMT("\t\t\t[PMT] usCaSystemID = %d\n", usCaSystemID);
						if( (IR_CA_SYSTEM_ID_MIN <= usCaSystemID) && (usCaSystemID <= IR_CA_SYSTEM_ID_MAX) )
						{
							g_IrSvcInfo[index].eSvcCasKind = eIrDrvCaType_irdeto;
							IsIrdetoCADescExist = 1;
						}
						else
						{
							if(g_IrSvcInfo[index].eSvcCasKind != eIrDrvCaType_irdeto)
								g_IrSvcInfo[index].eSvcCasKind = eIrDrvCaType_Other;
						}
					}
				}
				p +=(ucDesLen+2);
				wEsInfoLen -=(ucDesLen+2);
				wSecLen -=(ucDesLen+2);
			}
			MsgDvbStream.bDescrCount = ucStreamDescCount;
			HxSTD_memcpy(pMsgDvbStream, &MsgDvbStream, sizeof(msg_dvb_stream_st));
		}
	}
	MsgEcmSource.bStreamCount = ucStreamCount;
	HxSTD_memcpy(pMsgEcmSource, &MsgEcmSource, sizeof(msg_ecm_source_st));

	if( (IsGlobalCADescExist == 0) && (IsStreamCADescExist == 0) )
	{
		g_IrSvcInfo[index].eSvcCasKind = eIrDrvCaType_Fta;
		msg.usMsgClass = IREVT_CLASS_CAS_FTA_SERVICE;
		msg.ulParam = MAKE_IR_STATUSCODE(SEVERITY_TO_CHAR(STATUS_INFO_DND), 100, 8);
		msg.usParam = AXI_STATUSCODE_IDX_D100_8;

		CAS_IR_EVT_SendEvent(g_IrSvcInfo[index].hAction, &msg);
	}
	*Length = TotalLength;
	return pECMRscData;
}

/********************************************************************************
********************************************************************************/
HUINT8 *CAS_IR_SI_ParseNullPMT(HUINT8	index, Handle_t hAction, HUINT16 *Length)
{
	msg_resource_st	MsgRsc;
	HUINT16			TotalLength = 0;
	HUINT8			*pECMRscData = NULL;
	HUINT8			*pTemp;

	msg_ecm_source_st	MsgEcmSource;
	HUINT8				*pMsgEcmSource;
	msg_dvb_stream_st	MsgDvbStream;


	HxLOG_Info("index(0x%X)\n", index);

	if (index >= IR_HUMAX_MAX_SERVICE)
	{
		HxLOG_Error("SvcIndex >= IR_HUMAX_MAX_SERVICE\n");
		return (HUINT8 *)NULL;
	}
	pECMRscData = (HUINT8 *)OxCAS_Calloc(1024 + sizeof(msg_resource_st));	// Max로 잡아놓고 시작.
	if (pECMRscData == NULL)
	{
		HxLOG_Critical("\n\n");
		return (HUINT8 *)NULL;
	}
	pTemp = pECMRscData;

	HxSTD_memset(&MsgRsc, 0, sizeof(msg_resource_st));
	HxSTD_memset(&MsgEcmSource, 0, sizeof(msg_ecm_source_st));
	HxSTD_memset(&MsgDvbStream, 0, sizeof(msg_dvb_stream_st));

	MsgRsc.wResourceType = RESOURCE_ECM_SOURCE;
	// TODO:check the type. jichoi
	MsgRsc.wResourceId = (vd_rsc_id) g_IrSvcInfo[index].ucDemuxId;
	MsgRsc.wResourceMgnt = MANAGE_UPDATE;

	HxSTD_memcpy(pTemp, &MsgRsc, sizeof(msg_resource_st));
	pTemp += sizeof(msg_resource_st);
	TotalLength += sizeof(msg_resource_st);

	// pTemp가 msg_ecm_source_st를 써야하는 단계지만, 아직 bStreamCount를 모르므로.. 일단 pointer는 pMsgEcmSource에 적어두고.
	pMsgEcmSource = pTemp;
	pTemp += sizeof(msg_ecm_source_st);
	TotalLength += sizeof(msg_ecm_source_st);

	MsgEcmSource.bGlobalCount = 0;
	MsgEcmSource.bStreamCount = 0;
	HxSTD_memcpy(pMsgEcmSource, &MsgEcmSource, sizeof(msg_ecm_source_st));

	*Length = TotalLength;

	return pECMRscData;
}

/********************************************************************************
********************************************************************************/
HIR_ERR	CAS_IR_SI_ParseSDT(Handle_t hAction, HUINT8* pSdtRaw, HUINT16 Length, HUINT8 *pucRunningStatus)
{
	HxLOG_Critical("\n\n");
	return HIR_NOT_OK;
}


#define _______Update_Functions___________________________________________________________

#if	defined(IRPLAT_SYS_EMM_BASED_ORGID)

void	CAS_IR_SI_UpdateCAT(CAS_CAI_IF_msgCAT *pMsgSI)
{
	PrintNewEMM("[%s][%d] - CAT Update With action [0x%08x]\n", __FUNCTION__, __LINE__, pMsgSI->hAction);

	//	add new cat info
	CAS_IR_SVC_AddIrTpInfo(pMsgSI->hAction, pMsgSI->usTsUniqueId, pMsgSI->usLength, pMsgSI->pRawData);

	//	update EMM open if needs
	CAS_IR_SVC_UpdateIrTpInfo();
}

#else

void	CAS_IR_SI_UpdateCAT(CAS_CAI_IF_msgCAT *pMsgSI)
{
	IR_TP_INFO		*pstIrTpInfo;
	HUINT32			i;
	ia_result		result;
	msg_service_open_request_data_st	*pstSvcOpenReqData;

	HxLOG_Info("[CAS_IR_SI_UpdateCAT] hAction(%x) UniqueId(%x)\n", pMsgSI->hAction, pMsgSI->usTsUniqueId);
	if (pMsgSI == NULL)
		return;
	if (pMsgSI->pRawData == NULL)
		return;

	pstIrTpInfo = CAS_IR_DRV_FindIrTpInfo(pMsgSI->hAction);
	if (pstIrTpInfo == NULL)
	{
		CAS_CAI_IF_msgTS	msgTs;

		HxLOG_Print("Try to StartTS\n");
		msgTs.usTsUniqueId	= pMsgSI->usTsUniqueId;
		msgTs.hAction		= pMsgSI->hAction;
		CAS_IR_SVC_StartTS(&msgTs);

		pstIrTpInfo = CAS_IR_DRV_FindIrTpInfo(pMsgSI->hAction);
	}
	else
	{
		HxLOG_Print("CAT is Changed!!!\n");
	}

	if (pstIrTpInfo == NULL)
	{
		HxLOG_Error("StartTS failed!!!\n");
		return;
	}
	if(pMsgSI->pRawData)
	{
		HxLOG_Print("(pMsgSI->pRawData) exists \n");
	}
	if(pstIrTpInfo->pCatRaw)
	{
		HxLOG_Print("(pstIrTpInfo->pCatRaw) exists \n");
	}
	//	if new CAT is same with previous CAT in Softcell, we ignore new CAT.
	if ((pMsgSI->pRawData) && (pstIrTpInfo->pCatRaw))
	{

		HxLOG_Print("pMsgSI->usLength: %d, pstIrTpInfo->nCatLen: %d \n", pMsgSI->usLength, pstIrTpInfo->nCatLen);
		if (pMsgSI->usLength == pstIrTpInfo->nCatLen)
		{
			if (memcmp(pstIrTpInfo->pCatRaw, pMsgSI->pRawData, pMsgSI->usLength) == 0)
			{
				HxLOG_Print("pstIrTpInfo->ucIsSCELL_Ready: %d \n", pstIrTpInfo->ucIsSCELL_Ready);
				if (pstIrTpInfo->ucIsSCELL_Ready)
				{
					//	EMM started
					HxLOG_Print("Same CAT is encountered, ignore CAT\n");
					OxCAS_Free(pMsgSI->pRawData);
					return;
				}
			}
			else
			{
				HxLOG_Print("it's different CAT \n");
			}
		}

		if (pstIrTpInfo->ucIsSCELL_Ready)
		{
			HxLOG_Print("EMM SCELL is already ready!! - close and restart!!\n");

			CAS_IR_SVC_CLOSE_EMM(pstIrTpInfo);

			for( i = 0; i < CAS_MAX_NUM_OF_SC; i++ )
			{
				#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
					s_EmmMonitorCount[i] = pstIrTpInfo->EmmMonitoring[i].wEmmCount;
				#endif

				pstIrTpInfo->usSCELL_SvcHandle[i]	= INVALID_SOFTCELL_SVCHANDLE;
				pstIrTpInfo->usSCELL_SvcType[i]		= SERVICE_INVALID;
				pstIrTpInfo->ucIsSCELL_Ready		= 0;
			}
		}
	}

	HxLOG_Print("New CAT Process!!!\n");
	//	New CAT process
	if (pstIrTpInfo->pCatRaw)
		OxCAS_Free(pstIrTpInfo->pCatRaw);

	pstIrTpInfo->pCatRaw = OxCAS_Malloc(pMsgSI->usLength);
	if (pstIrTpInfo->pCatRaw == NULL)
	{
		HxLOG_Error("CAT memory allocation error [%d]\n", pMsgSI->usLength);
		pstIrTpInfo->IsValidCat = FALSE;
		return;
	}
	pstIrTpInfo->nCatLen = pMsgSI->usLength;
	HxSTD_memcpy(pstIrTpInfo->pCatRaw, pMsgSI->pRawData, pMsgSI->usLength);
	OxCAS_Free(pMsgSI->pRawData);
	pMsgSI->pRawData = NULL;
	pstIrTpInfo->IsValidCat = TRUE;

	HxLOG_Info("\n");
	for( i = 0; i < CAS_MAX_NUM_OF_SC; i++ ) //EMM open request
	{
		pstIrTpInfo->usSCELL_SvcType[i] = SERVICE_DVB_EMM;
		pstSvcOpenReqData = (msg_service_open_request_data_st*)OxCAS_Malloc(sizeof(msg_service_open_request_data_st));
		if (pstSvcOpenReqData == NULL)
		{
			HxLOG_Error("Memory allocation error!!\n");
			return;
		}

		HxLOG_Print("CAT_UPDATE --> SEND_SVC_OPEN --> EMM_OPEN\n");
		HxLOG_Print("SCELL_Message --> MSG_SERVICE_OPEN_REQUEST : SERVICE_DVB_EMM\n");

		pstSvcOpenReqData->wServiceType = SERVICE_DVB_EMM;
		result = SCELL_Message(MSG_SERVICE_OPEN_REQUEST, 0, sizeof(msg_service_open_request_data_st), (void*)pstSvcOpenReqData);
		if (result != IA_SUCCESS)
		{
			HxLOG_Print("Error : SCELL_Message (%x)", result);
		}
	}

}

#endif



void	CAS_IR_SI_UpdatePMT(CAS_CAI_IF_msgPMT *pMsgSI)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	HVD_Result			hVdResult;
	HBOOL 				bSCInserted = FALSE, bNoSCInseted = TRUE;
	HUINT32				ulSlot = 0;
#endif
	HBOOL				bHumaxPvrRecording = FALSE;

	HxLOG_Info("hIrSvc(%x) UniqueId(%x)\n", pMsgSI->hIrSvc, pMsgSI->usTsUniqueId);
	if (pMsgSI == NULL)
	{
		HxLOG_Error("Error : pMsgSI is NULL!!!\n");
		return;
	}
	if (pMsgSI->pRawData == NULL)
	{
		HxLOG_Error("Error : pMsgSI->pRawData is NULL!!!\n");
		return;
	}

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(pMsgSI->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Error("Error : hIrSvc(%x) is not allocated yet!!\n", pMsgSI->hIrSvc);
		OxCAS_Free(pMsgSI->pRawData);
		return;
	}

#if	(CONFIG_MW_CAS_NUM_EMBEDDED > 1)
	// Multicas 의 경우, PMT update 시 마다 카드 상태를 알린다.
	local_cas_ir_Drv_PMT_NotifySCStatusToApp();
#endif

	if (pstIrSvcInfo->pucPMTRaw)
	{
		HxLOG_Print("PMT changed!!\n");
		OxCAS_Free(pstIrSvcInfo->pucPMTRaw);
	}
	pstIrSvcInfo->pucPMTRaw = OxCAS_Malloc(pMsgSI->usLength);
	if (pstIrSvcInfo->pucPMTRaw == NULL)
	{
		HxLOG_Error("Error to allocated PMT raw memory!!\n");
		pstIrSvcInfo->ucIsValidPMT = FALSE;
		return;
	}
	HxSTD_memcpy(pstIrSvcInfo->pucPMTRaw, pMsgSI->pRawData, pMsgSI->usLength);
	OxCAS_Free(pMsgSI->pRawData);
	pMsgSI->pRawData = NULL;
	pstIrSvcInfo->ucIsValidPMT = TRUE;

#if defined(CONFIG_IRDETO_HMX_PVR)
	if( (pstIrSvcInfo->eIrSvcActionType == IR_SVC_LIVE)
		&& (pstIrSvcInfo->eCasActionType == eCAS_ActionType_Recording || pstIrSvcInfo->eCasActionType == eCAS_ActionType_Tsr_Rec))
	{
		bHumaxPvrRecording = TRUE;
	}
#endif

	HxLOG_Info("bHumaxPvrRecording: %d\n", bHumaxPvrRecording);

	//	record base mode : application에서 pid설정을 하지 않는다.
	//	pmt update시에 record base모드이면 강제default pid를 설정해 주어야 함.
	if ((pstIrSvcInfo->eIrSvcActionType == IR_SVC_RECORD_BASE || bHumaxPvrRecording == TRUE)  && pstIrSvcInfo->ucStreamCount == 0)
	{
		HxLOG_Info("PMT UPDATE with add default pid for recording descramble!!!\n");
		CAS_IR_SVC_SetPIDforRecording(pstIrSvcInfo);
		if (pstIrSvcInfo->ucIsSCELLReady)
			return;
	}

	if (pstIrSvcInfo->ucIsSCELLReady)
	{
		CAS_IR_SVC_DEFINE_DVB(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo));
	}
	else
	{
		HxLOG_Print("Open Reply is not received yet!!!\n");
	}

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	for(ulSlot=0; ulSlot < CONFIG_MW_CAS_NUM_SC_SLOT;ulSlot++)
	{
		hVdResult = HUMAX_VD_SC_GetCardPhysicalStatus (ulSlot, &bSCInserted);
		if((hVdResult == eHVD_OK)&&(bSCInserted == TRUE))
		{
			bNoSCInseted = FALSE;
#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
			HxLOG_Info("PMT UPDATE with ExtendedProductListQuery(%d)!!!\n",ulSlot);
			CAS_IR_DRV_QueryExtendedProductList(ulSlot, IA_TRUE, 0, NULL);
#else
			HxLOG_Info("PMT UPDATE with AnyProductQuery(%d)!!!\n",ulSlot);
			CAS_IR_DRV_AnyProductQuery(ulSlot);
#endif
		}
	}

	if(bNoSCInseted)
	{
		CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT);
	}
#endif
}


#if defined(IRPLAT_FN_PROCESS_SDT)
void CAS_IR_SI_UpdateSDT(CAS_CAI_IF_msgSDT * pMsgSI)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	eIR_SDT_RUNNING_STATUS	eIrRunStatus;

	HxLOG_Info("hIrSvc(%X), usTsUniqueId(%X)\n", pMsgSI->hIrSvc, pMsgSI->usTsUniqueId) );

	if (pMsgSI == NULL)
	{
		HxLOG_Error("pMsgSI is NULL!!!\n");
		return;
	}
	if (pMsgSI->pRawData == NULL)
	{
		HxLOG_Error("pMsgSI->pRawData is NULL!!!\n");
		return;
	}

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(pMsgSI->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Error("hIrSvc[%x] is not existed in service pool!!!\n", pMsgSI->hIrSvc);
		OxCAS_Free(pMsgSI->pRawData);
		return;
	}

	CAS_IR_SI_ParseSDT(pMsgSI->hAction, pMsgSI->pRawData, pMsgSI->usLength, &eIrRunStatus);

	if(pMsgSI->pRawData != NULL)
	{
		OxCAS_Free(pMsgSI->pRawData);
		pMsgSI->pRawData = NULL;
	}
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
void	CAS_IR_SI_UpdateNIT(CAS_CAI_IF_msgNIT *pMsgSI)
{
	HUINT8		*p;
	HUINT16 	usLen1, usLen2, usLen3, usONID, usTSID;
	HUINT8		*pucMuxTransportDesc= NULL, *pucSignatureDesc = NULL;
	HUINT8		descLen=0;
	HBOOL		bIsChanged;
	HUINT32 	ulCRC;
	HUINT16 	tLength=0;
	HUINT8		ucLastSecNum = 0;
	HUINT32 	ulCount=0, i=0;
	HUINT8		*pucTemp;

	HxLOG_Info("hAction(%x) UniqueId(%x)\n", pMsgSI->hAction, pMsgSI->usTsUniqueId);
	if (pMsgSI == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	if (pMsgSI->pRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pucTemp = pMsgSI->pRawData;

	ucLastSecNum = get8bit(pucTemp + 7);
	ucLastSecNum++;

	for(i = 0 ; i < ucLastSecNum; i++)
	{
		tLength 	= get12bit(pucTemp + 1) + 3;
		ulCRC		= get32bit(pucTemp + tLength -4);
		bIsChanged	= CAS_IR_SOL_Is_NIT_Changed(i, ulCRC);

		if(bIsChanged == TRUE)
		{
			break;
		}

		pucTemp += tLength;
	}

	if(bIsChanged != TRUE)
	{
		return;
	}

	CAS_IR_SOL_Clear_NIT_Info();

	pucTemp = pMsgSI->pRawData;

	for(i = 0 ; i < ucLastSecNum; i++)
	{
		tLength = get12bit(pucTemp + 1) + 3;

		if(tLength == 0)
		{
			OxCAS_Free(pMsgSI->pRawData);
			return;
		}

		ulCRC = get32bit(pucTemp + tLength -4);

		CAS_IR_SOL_NIT_UpdateCRC(i, ulCRC);

		p = pucTemp;
		ulCount = 0;

		p+=8; /*skip until find network descriptors_length */
		ulCount+=8;

		/*Caluclate the length of network descriptors */
		usLen1 = get12bit(p);
		p+=2;
		ulCount+=2;

		/*jump to item of ts loop length */
		p+= usLen1;
		ulCount+=usLen1;

		/*Caluclate the length of transport_stream_loop_length*/
		usLen1 = get12bit(p);
		p+=2;
		ulCount+=2;

		/*Transport stream loops */
		while(usLen1 > 0)
		{
			usTSID = get16bit(p);			p+=2;	ulCount+=2;
			usONID = get16bit(p);			p+=2;	ulCount+=2;
			usLen3 = usLen2 = get12bit(p);	p+=2;	ulCount+=2;

			while(usLen3 > 0)
			{
				if( (*p == 0x43)   /*Tag of satellite delivery descriptor */
				||	(*p == 0x44)   /*Tag of cable delivery descriptor */
				||	(*p == 0x5A) ) /*Tag of terrestial delivery descriptor */
				{
					HxLOG_Info("Found: Delivery descriptor (%s)\n ", (*p==0x43)?"SATELLITE": ((*p==0x44)? "CABLE" : "TERTESTIAL" ));

					/*Update each delivery descriptor */
					if ( CAS_IR_SOL_update_DeliveryDesc(usONID, usTSID, p) )
					{
						OxCAS_Free(pMsgSI->pRawData);
						HxLOG_Info("sol update failed\n");
						return;
					}
				}
				usLen3 -= (*(p+1) + 2 );
				p+= (*(p+1) + 2 );
				ulCount+= (*(p+1) + 2 );

			}
			usLen1 -= (6 + usLen2 );
		}

		pucTemp += tLength;
	}

	HxLOG_Info("COMPLETE ==> Go to check validation\n");
	CAS_IR_SOL_Complete_NITInfo(TRUE);
	OxCAS_Free(pMsgSI->pRawData);
	return;
}

void	CAS_IR_SI_UpdateBAT(CAS_CAI_IF_msgBAT *pMsgSI)
{
	HUINT8		*p;
	HUINT8		descLen=0;
	HUINT8		bFoundMuxTP = FALSE;
	HUINT8		bFoundMuxSign = FALSE;
	HUINT8		*pucSignatureDesc = NULL;
	HUINT8		*pucMuxTransportDesc= NULL;
	HUINT16 	tLength=0;
	HUINT16 	bat_exid;
	HUINT32 	ret;
	HUINT32 	ulCRC;

	HxLOG_Info("[SOL]INT_IR_DRV_BAT_Update\n");

	/*Check condition */
	if( (pMsgSI->usLength == 0) || (pMsgSI->pRawData == NULL))
	{
		HxLOG_Error("illegal BAT notified!\n");
	}

	p = pMsgSI->pRawData;
	p+=3;

	/*Check whether ex-ID is a BAT for SOL*/
	bat_exid = get16bit(p);
	if(bat_exid != eBOUQEUT_ID_IRDETO_SOL)
	{
		if(pMsgSI->usLength != 0)
		{
			OxCAS_Free(pMsgSI->pRawData);
		}
		return;
	}

	//if CRC is the same, BAT is not changed
	tLength = get12bit(pMsgSI->pRawData + 1) + 3;
	ulCRC = get32bit(pMsgSI->pRawData + tLength -4);

	if(CAS_IR_SOL_Is_BAT_Changed(ulCRC) != TRUE)
	{
		HxLOG_Info("SAME BAT\n");

		p+=5;
		tLength = get12bit(p);

		p+=2;

		while(tLength > 0)
		{
			if((*p == TAG_MUX_TRANSPORT_LIST_DESCRIPTOR) && !bFoundMuxTP )
			{
				bFoundMuxTP = TRUE;
			}
			else if((*p == TAG_MUX_SIGNATURE_DESCRIPTOR) && !bFoundMuxSign ) // signature descriptor fixed length= 131 byte
			{
				bFoundMuxSign = TRUE;
			}

			if(bFoundMuxTP == TRUE && bFoundMuxSign == TRUE)
			{
				break;
			}
			tLength -=	(HUINT16)( *(p+1) +2 );
			p +=  *(p+1) + 2;
		}
		OxCAS_Free(pMsgSI->pRawData);

		if( bFoundMuxTP && bFoundMuxSign )
		{
			CAS_IR_SOL_Complete_BATInfo(FALSE);
		}
		else
		{
			CAS_IR_SOL_Complete_BATInfoFail();
		}
		return;
	}

	p+=5;
	tLength = get12bit(p);

	p+=2;
	while(tLength > 0)
	{
		if((*p == TAG_MUX_TRANSPORT_LIST_DESCRIPTOR) && !bFoundMuxTP )
		{
			HxLOG_Info("Found: Mux-Transport descriptor\n");

			CAS_IR_SOL_Clear_BAT_Info();
			CAS_IR_SOL_BAT_UpdateCRC(ulCRC);

			pucMuxTransportDesc = p;

			/*pass Mux-transport descritpor to SOL */
			ret= CAS_IR_SOL_update_MuxTPDesc(pucMuxTransportDesc);
			if(ret == ERR_OK)
				bFoundMuxTP= TRUE;
		}
		else if((*p == TAG_MUX_SIGNATURE_DESCRIPTOR) && !bFoundMuxSign ) // signature descriptor fixed length= 131 byte
		{
			HxLOG_Info("Found: Mux-signature descriptor\n");
			pucSignatureDesc = p;

			/*upate signature */
			ret = CAS_IR_SOL_update_SignatureDesc(pucSignatureDesc);
			if(ret == ERR_OK)
				bFoundMuxSign= TRUE;
		}

		tLength -=	(HUINT16)( *(p+1) +2 );
		p +=  *(p+1) + 2;

	}

	if( bFoundMuxTP && bFoundMuxSign  )
	{
		CAS_IR_SOL_Complete_BATInfo(TRUE);
	}
	else
	{
		CAS_IR_SOL_Complete_BATInfoFail();
	}

	OxCAS_Free(pMsgSI->pRawData);

}

#endif


