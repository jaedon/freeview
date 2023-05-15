/*******************************************************************************
*
* File Name    : ap_mplayer.h
*
* Description  : new media player header for OCTO 2.0
*
*
* Copyright (C) 2012, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                  Name
* ----        ------------                                 --------
* 2012/01/09	createded									ultracat,jwkim
* 2012/05/05 	adding interfaces about video/audio track
*
*******************************************************************************/

#ifndef ____INSIDE_MGR_MEDIA_H____
#error "Only <mgr_media.h> can include directly."
#endif

#ifndef __MGR_MPLAYER_INT_H__
#define __MGR_MPLAYER_INT_H__

#include <octo_common.h>

typedef void MGR_MPlayer_t;

typedef enum
{
	eMPLAYER_PLAYTYPE_NONE,
	eMPLAYER_PLAYTYPE_AUTO,
	eMPLAYER_PLAYTYPE_PROGRESSIVE_DOWNLOAD,
	eMPLAYER_PLAYTYPE_REAL_STREAMING
} MgrMplayer_Type_e;

typedef struct
{
	DxVideoCodec_e		eVideoCodec;
	HUINT32				ulVideoPesId;
	HUINT32				ulVideoStreamId;
} MgrMplayer_VideoTrack_t;

typedef struct
{
	DxAudioCodec_e		eAudioCodec;
	HUINT32				ulAudioPesId;
	HUINT32				ulAudioStreamId;
	HINT8				szAudioLanguage[64];
} MgrMplayer_AudioTrack_t;

typedef struct
{
	HINT32			width;
	HINT32			height;
} MgrMplayer_VideoWindow_t;

#if 0
typedef struct
{
	HUINT32         ulChunks;
	HUINT32         *ullStartTime;
	HUINT32         *ullEndTime;
	HUINT32			ulBufferRemainedTime;
	HUINT32			ulBufferPercent;
} MgrMplayer_BufferingInfo_t;

typedef struct
{
	HBOOL		bHdcp;
	HBOOL		bScms;
	HBOOL		bCgms;
	HBOOL		bMacrovision;
} MgrMplayer_OutputControl_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef BUS_Result_t (* MgrMplayer_Proc_t)(MGR_MPlayer_t *pstContenxt, HINT32 message, Handle_t handle, HINT32 param1, HINT32 param2, HINT32 param3);

typedef void (*MgrMplayer_VerifyExternalCb)(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result);

typedef struct
{
	MgrMplayer_VerifyExternalCb			verifycallback;
	void 								*userdata;
} MgrMplayer_VerifyExternalCallback_t;

MGR_MPlayer_t *MGR_MPlayer_Create(HUINT32 hActionId);
void MGR_MPlayer_Destroy(MGR_MPlayer_t *hPlayer);
MGR_MPlayer_t *MGR_MPlayer_GetActivePlayer(HUINT32 hActionId);

HERROR MGR_MPlayer_Probe(MGR_MPlayer_t *hPlayer, const HINT8 *szUrl, MgrMplayer_Type_e ePlayType);
HERROR MGR_MPlayer_Play(MGR_MPlayer_t *hPlayer, HUINT32 ulStartTime);
HERROR MGR_MPlayer_Reset(MGR_MPlayer_t *hPlayer);
HERROR MGR_MPlayer_Stop(MGR_MPlayer_t *hPlayer);
HERROR MGR_MPlayer_SetSaveThumbnail(MGR_MPlayer_t *hPlayer, HINT8 *szFileName, HINT32 nWidth, HINT32 nHeight);
HERROR MGR_MPlayer_SetSubtitleUsage(MGR_MPlayer_t *hPlayer, HBOOL bUse);
BUS_Callback_t	MGR_MPlayer_GetProc(HUINT32 ulActionId);

HERROR MGR_MPlayer_SI_Start(MGR_MPlayer_t *hPlayer);
HERROR MGR_MPlayer_SI_Stop(MGR_MPlayer_t *hPlayer);
HERROR MGR_MPlayer_DVB_Sbtl_Start(MGR_MPlayer_t *hPlayer, HUINT16 usPid, HUINT16 usCompPageId, HUINT16 usAncPageId);
HERROR MGR_MPlayer_DVB_Sbtl_Stop(MGR_MPlayer_t *hPlayer);

#ifdef __cplusplus
};
#endif


#endif	/* !__MGR_MPLAYER_INT_H__ */
