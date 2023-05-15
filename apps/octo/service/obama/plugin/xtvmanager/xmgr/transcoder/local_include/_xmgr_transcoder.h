/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_transcoder.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_TRANSCODER_INT_H___
#define	___XMGR_TRANSCODER_INT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_transcoder.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define MAX_TRANSCODE_ACTION		(NUM_VIEW_ACTION + NUM_BG_ACTION)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eTrcdState_Idle			= 0,

	eTrcdState_ReqStart,
	eTrcdState_Running,
	eTrcdState_Stopped,

	eTrcdState_EndOfCase
} xMgrTrcdState_e;

typedef struct
{
	Handle_t					 hSvcHandle;
} xMgrTrcdSrcLive_t;

typedef struct
{
	HUINT32						 ulContentId;
	SvcMeta_Service_t				 stPvrSvcInfo;
} xMgrTrcdSrcPb_t;

typedef struct
{
	Handle_t					 hRingBuf;
} xMgrTrcdOutputRingBuf_t;

typedef struct
{
	HCHAR						 szFileUrl[TRANSCODING_FILE_LEN];
} xMgrTrcdOutputFile_t;

typedef struct
{
	//
	// Input Arguments:
	//
	Handle_t					 hAction;

	// Input Arguments:
	SvcTranscoder_SourceType_e	 eSourceType;
	union
	{
		xMgrTrcdSrcLive_t		 stLive;
		xMgrTrcdSrcPb_t			 stPb;
	} unSrc;

	SvcTranscoder_OutputType_e	 eOutputType;
	union
	{
		xMgrTrcdOutputRingBuf_t	 stRingBuf;
		xMgrTrcdOutputFile_t	 stFile;
	} unOutput;

	// Transcoding Profile:
	SvcTranscoder_Profile_t	 	 stTranscoderProfile;
	MgrTrcd_StreamProfile		 stProfile;
	HBOOL						 bPcrTsPacketInsert;

	// Transcoding Status:
	BUS_Callback_t				 pfActionProc;
	xMgrTrcdState_e				 eState;
	HBOOL						 bTranscodingStarted;
} xMgrTrcdContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
// xmgr_transcoder.c
extern xMgrTrcdContext_t*	XMGR_TRCD_GetContext (HUINT32 ulActionId);
extern HERROR				XMGR_TRCD_SetStartArguments (xMgrTrcdContext_t *pstContext, Handle_t hAction, MgrTrcd_Start_t *pstStart);

// xmgr_transcoder_pb_base.c
extern BUS_Result_t			MGR_TRCD_PB_BASE_MsgBusCreate (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t			MGR_TRCD_PB_BASE_MsgBusDestroy (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t			MGR_TRCD_PB_BASE_MsgAppPlaybackNotifyStopped (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t			MGR_TRCD_PB_BASE_EvtPbEof (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t			MGR_TRCD_PB_BASE_EvtPvrSitChanged (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t			MGR_TRCD_PB_BASE_EvtPvrPmtChanged (xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t 		MGR_TRCD_PB_BASE_EvtStopTranscoding(xMgrTrcdContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

// xmgr_transcoder_pb_base.c
extern BUS_Callback_t		XMGR_TRCD_PB_BASE_GetActionProc (HUINT32 ulActionId);

// xmgr_transcoder_pb_japan.c
extern BUS_Callback_t		XMGR_TRCD_PB_JAPAN_GetActionProc (HUINT32 ulActionId);


HERROR			xmgr_transcoder_Start (HUINT32 ulActionId, MgrTrcd_Start_t *pstStartArg);
HERROR			xmgr_transcoder_Stop (HUINT32 ulActionId);


#endif /* !___XMGR_TRANSCODER_INT_H___ */

