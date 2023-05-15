/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_live_comp_base.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_LIVE_COMP_BASE_H___
#define	___XMGR_LIVE_COMP_BASE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <xmgr_live.h>
#include <svc_epg.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	// common
	eComp_EpgContents,
	eComp_EpgPf,			//<< PF_ALL, PF_ACT, PF_OTH
	eComp_EpgSch,			//<< SCH_ALL, SCH_ACT, SCH_OTH

	//  base
	eComp_Subtitle,
	eComp_Teletext,

	// freesat
	eComp_EpgPfExt,
	eComp_EpgSchExt,

	// japan
	eComp_CloseCaption,
	eComp_AribDownload,
	eComp_BML,


	eComp_MAX
} xMGR_Complementary_e;	// complementary type

typedef struct
{
	Handle_t		hSvc;
	SvcEpg_EpgKind_e 	eEpgKind;		//<< ����� ������� ����
	SvcEpg_EventKind_e 	eEvtKind;

	HUINT32		ulParam1;			//<< param�� ���� freesat������ �����
	HUINT32 		ulParam2;
	HUINT32		ulParam3;
}xMGR_Comp_EpgParam_T;

typedef struct
{
	XmgrLive_MwSvcState_e		aeServiceState[eComp_MAX];

	HUINT32						ulActionId;					/* �� context�� �Ҵ� ���� action ID */
	Handle_t						hAction;						/* �� context�� ���� zapping session version�� ���Ե� action handle */
	Handle_t						hSvc; 						/* Service handle of this session */

	HBOOL						bPmtValid;					/* PMT�� ���޵�. */
	DbSvc_Info_t					stSvcInfo;					/* ���� ���� information */

	/* Live module�̳� PB���� start��Ű�� TRUE, stop�ϸ� FALSE : mwSvc�� start�Ǳ� ���� �������� �ϳ���.. */
	HBOOL						bStartSubtitle;

	/* +++++ EPG ���� +++++ */
	xMGR_Comp_EpgParam_T		stEpgParam;					//<< Start EPG or Stop EPG�� �� ����ϴ� ����

#if defined(CONFIG_MW_SUBTITLE)
	/* +++++ Subtitle ���� ���� +++++ */
	HBOOL						bSubtitleAllowed;				/* Subtitle �����ص� �ȴٴ� flag. MENU���� AUTO�̰ų� subtitle list popup�� start�� ��� TRUE */
	HBOOL						bTtxPageEnabled;				/* TELETEXT page�� �����ְ� ����. Subtitle�� enable�� �� ����. */
	SvcSi_SubtitleInfo_t				subtitleInfo;					/* svcInfo�� �ִ� index�� PMT���� ��� subtitle info */
	HUINT32						nDVBSbtCPageID;				/* subtitle���� page id �� ���Ƿ� ������ ��� �����. */
	HUINT32						nDVBSbtAPageID;
	HUINT32						nEBUSbtMagNum;
	HUINT32						nEBUSbtPageNum;
#endif

} xMGR_LiveComp_Context_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern BUS_Callback_t 	XMGR_LiveComp_BASE_GetProc(void);
extern HERROR 		XMGR_LiveComp_BASE_Init (void);

extern HERROR 		XMGR_LiveComp_BASE_StartEpgContent(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
extern HERROR 		XMGR_LiveComp_BASE_StartEpg(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
extern HERROR 		XMGR_LiveComp_BASE_StopEpg(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction);

extern HERROR 		XMGR_LiveComp_BASE_StartSubtitle(Handle_t hAction);
extern HERROR 		XMGR_LiveComp_BASE_StopSubtitle(Handle_t hAction, ApiSyncMode_t eSync);

extern HERROR 		XMGR_LiveComp_BASE_StartCloseCaption(Handle_t hAction);
extern HERROR 		XMGR_LiveComp_BASE_StopCloseCaption(Handle_t hAction, ApiSyncMode_t eSync);

extern HERROR 		XMGR_LiveComp_BASE_StartTeletext(Handle_t hAction, HUINT16 usTtxPid);
extern HERROR 		XMGR_LiveComp_BASE_StopTeletext(Handle_t hAction, ApiSyncMode_t eSync);

extern HERROR 		XMGR_LiveComp_BASE_StartAribDownload(Handle_t hAction, Handle_t hSvc);
extern HERROR 		XMGR_LiveComp_BASE_StopAribDownload(Handle_t hAction, ApiSyncMode_t eSync);

extern HERROR 		XMGR_LiveComp_BASE_StartBML(Handle_t hAction);
extern HERROR 		XMGR_LiveComp_BASE_StopBML(Handle_t hAction, ApiSyncMode_t eSync);

#endif /* !___XMGR_LIVE_COMP_BASE_H___ */

/* End Of File */
