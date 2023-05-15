/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_pb.h
	@brief

	Description:  									\n
	Module: SVC / Playback							\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/


#ifndef __SVC_PB__
#define __SVC_PB__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_NUM_OF_PLAY					(NUM_VIEW_ACTION + NUM_BG_ACTION)

#define MAX_FILE_LEN					256 // FS_MAX_FILE_NAME_LEN
#define MAX_TITLE_LEN					256

/*******************************************************************/
/********************      Data Type (enum)    *********************/
/*******************************************************************/
typedef enum
{
	eSEVT_PB_				= eSEVT_PB_START,

	/* events comes from Player */
	eSEVT_PB_SETUP,
	eSEVT_PB_TIMECODE_CHANGE,
	eSEVT_PB_BOF,
	eSEVT_PB_EOF,
	eSEVT_PB_DRM_AV_BLANK,
	eSEVT_PB_SPEED_CHANGED
}SvcPb_EvtMsg_e;

typedef enum
{
	ePb_ActType_NONE = 0,

	ePb_ActType_Playback,
	ePb_ActType_Tsr_Playback,
	ePb_ActType_Chase_Playback,
	ePb_ActType_Transcoding_Playback,
	ePb_ActType_Transcoding_TsrPlayback,

	ePb_ActType_EndOfType
} SvcPb_ActType_e;


typedef enum
{
	ePb_StartPosition_Begin,
	ePb_StartPosition_LastView,
	ePb_StartPosition_End
} SvcPb_PlayStartPosType_e;


#define __MEMBER__
typedef enum
{
	eSvcPb_CtxtState_None = 0,
	eSvcPb_CtxtState_Waiting,
	eSvcPb_CtxtState_Running
} svcPb_ContextState_e;

typedef enum
{
	ePBEVT_TIMECODE,
	ePBEVT_EOF,
	ePBEVT_BOF,

	ePBEVT_END
}svcPb_EvtType_e;

typedef enum
{
	/* Session */
	ePBMSG_CMD_SETUP,
	ePBMSG_CMD_START,
	ePBMSG_CMD_STOP,

	ePBMSG_CMD_CLOSE,
	ePBMSG_CMD_PAUSE,
	ePBMSG_CMD_RESUME,

	ePBMSG_CMD_SETDECRYPT,

	/** DD -> SVC */
	ePBMSG_CMD_EVT_PLAYBACK,

	ePBMSG_CMD_END

}svcPb_MsgType_e;

typedef enum
{
	ePBMSG_ARGTYPE_None,
	ePBMSG_ARGTYPE_Setup,
	ePBMSG_ARGTYPE_SetDecryption,
	ePBMSG_ARGTYPE_ApiResultPtr,
	ePBMSG_ARGTYPE_SyncMode,
	ePBMSG_ARGTYPE_PlugIn,

	ePBMSG_ARGTYPE_PbEvtParam,

	ePBMSG_ARGTYPE_Unknown
} svcPb_MsgArgType_e;

typedef enum
{
	ePbResType_PlayerId,

	ePbResType_Unknown,
}svcPb_ResType_e;

typedef enum
{
	ePB_STATE_Init		= 0,
	ePB_STATE_Setup,
	ePB_STATE_Start,
	ePB_STATE_Stop,
	ePB_STATE_Pause,
	ePB_STATE_Trick,

	ePB_STATE_EndOfCase
} svcPb_State_e;

#define __STRUCT__
typedef struct
{
	SvcPb_ActType_e	ePbActType;

	DxSvcType_e			eSvcType;
	HBOOL				bEncryption;
	HBOOL				bUseUserEncKey;
	HUINT8			 	aucKey[PAL_PVR_CRYPTO_KEY_LEN];
	UNIXTIME			ulStartTime;

	SvcPb_PlayStartPosType_e	eStartPosType;

	HUINT8		aucPath[MAX_FILE_LEN];
	HUINT8		aucFileName[MAX_FILE_LEN];

	HBOOL		bDescramblePlay;
	HUINT32		ulRecordId; //chase PB
	HBOOL		bMakeThumbnail;

	HBOOL		bIsUsbFlashRecord;
	HBOOL		bStartPaused;		/* Playback start with pause */

} SvcPb_Setup_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;

	SvcPb_Setup_t	 stSetup;

} SvcPb_Arg_Setup_t;

typedef struct
{
	svcPb_EvtType_e	eType;
	HUINT32				ulParam1;
	HUINT32				ulParam2;
	HUINT32				ulParam3;

} SvcPb_EvtParam_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;

} SvcPb_Arg_Base_t;

typedef struct
{
	SvcPb_EvtParam_t	stEvtParam;

} SvcPb_Arg_EvtParam_t;

typedef	struct
{
	svcPb_MsgType_e eMsgClass;
	Handle_t		 hAction;
	HUINT32			 ulDemuxId;

	union
	{
		SvcPb_Arg_Setup_t		stSetup;
		SvcPb_Arg_Base_t		stBase;

		SvcPb_Arg_EvtParam_t	stEvtParam;

	} unArg;
} SvcPb_Msg_t;

typedef void (*SvcPb_ActFunc) (SvcPb_Msg_t *pstMsg, void *pvData);


HERROR SVC_PB_Init( PostMsgToMgrCb_t fnPostMsg );
HERROR SVC_PB_Setup( Handle_t hAction, SvcPb_Setup_t *pstSetup, HBOOL bSync );
HERROR SVC_PB_Start( Handle_t hAction, HBOOL bSync );
HERROR SVC_PB_Stop( Handle_t hAction, HBOOL bSync );
HERROR SVC_PB_Close( Handle_t hAction, HBOOL bSync );
HERROR SVC_PB_Pause( Handle_t hAction, HBOOL bSync );
HERROR SVC_PB_Resume( Handle_t hAction, HBOOL bSync );
HERROR SVC_PB_FrameByFrame(Handle_t hAction, HBOOL bFoward);
HERROR SVC_PB_SetSpeed (Handle_t hAction, HINT32 nSpeed );
HERROR SVC_PB_GetSpeed (Handle_t hAction, HINT32 *pnSpeed );
HERROR SVC_PB_GetSupportSpeeds (Handle_t hAction, HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray);
HERROR SVC_PB_SetPlayTime(Handle_t hAction, HUINT32 ulTime );
HERROR SVC_PB_GetPlayTime(Handle_t hAction, HUINT32 *pulTime );
HERROR SVC_PB_GetTimeInfo(Handle_t hAction, HUINT32 *pulCurrentTime, HUINT32 *pulEndTime );
HERROR SVC_PB_GetState (Handle_t hAction, svcPb_State_e *pePbState);
HERROR SVC_PB_SetDecrypt (Handle_t hAction, SvcPb_Setup_t *pstSetup, HBOOL bSync );

HUINT8*	 SVC_PB_ExtractThumbnailFromFile(HCHAR *pszFileName, HUINT32 ulTime, HINT32 nWidth, HINT32 nHeight, HUINT16 usVideoPid, HUINT32 ulVideoCodec, HBOOL bEncryption);

#define __PLUGIN_MEMBER__
HERROR svc_pb_GetMsgArg (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void *pvData);

#endif	// __SVC_PB__
/* end of file */
