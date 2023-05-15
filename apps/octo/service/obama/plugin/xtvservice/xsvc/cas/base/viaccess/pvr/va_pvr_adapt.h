/*
	Viaccess PVR API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.123
*/

#ifndef VA_PVR_ADAPT_H
#define VA_PVR_ADAPT_H

#include "va_pvr.h"
#include "va_pvr_api.h"

#include "va_ui_adapt.h"

HERROR VA_PVR_Adapt_SetRecordInfo(HUINT32 ulAcsId, VA_UI_RECORD_INFO *pstInfo);
HERROR VA_PVR_Adapt_SetPlaybackInfo(HUINT32 ulAcsId, VA_UI_PLAYBACK_INFO *pstInfo);

HERROR VA_PVR_Adapt_Init();

HERROR VA_SvcPb_SendEcmByPid(HUINT32 ulAcsId, HUINT16 usPid);

HUINT32 VA_PVR_Rec_GetTimecode(HUINT32 ulAcsId);
void VA_PVR_Rec_TimecodeChange(Handle_t hAction, HUINT32 ulCurrentTime);

HERROR VA_SvcPb_OpenFile(Handle_t hVaAction);
HERROR VA_SvcPb_CloseFile(Handle_t hVaAction);
void VA_SvcPb_ForceSetTimecode(HUINT32 ulAcsId);
void VA_SvcPb_TimecodeChange(Handle_t hAction, HUINT32 ulCurrentTime);
void VA_SvcPb_SendEcm(Handle_t hVaAction, HUINT32 ulCurrentTime);

HERROR VA_PVR_Rec_OpenFile(VA_PVR_FILE_t *pFile, HUINT32 ulRetryCnt);
HERROR VA_PVR_Rec_CloseFile(VA_PVR_FILE_t *pFile);
HERROR VA_PVR_Rec_RecordEcm(VA_ECM_CHANNEL_t *pEcmChannel, HUINT32 ulEcmLength, HUINT8 *pucEcm, HUINT32 ulTimestamp);
HERROR VA_PVR_Rec_DeleteEcm(HUINT32 ulAcsId, HBOOL bBlock);

HERROR VA_PVR_Adapt_WriteMetaData(HUINT32 ulAcsId, HUINT32 ulMetaLength, HUINT8 *pMeta);
HERROR VA_PVR_Adapt_ReadMetaData(HUINT32 ulAcsId, HUINT32 ulMetaLength, HUINT8 *pMeta);

#endif //VA_PVR_ADAPT_H


