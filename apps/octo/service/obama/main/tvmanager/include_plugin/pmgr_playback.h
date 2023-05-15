#ifndef	___PMGR_PLAYBACK_H___
#define	___PMGR_PLAYBACK_H___


#include <octo_common.h>
#include <mgr_playback.h>

HERROR			pmgr_playback_Init(void);
HERROR			pmgr_playback_SetPlaySetup (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup);
BUS_Callback_t	pmgr_playback_GetProcFunc(HUINT32 ulActionId);

HERROR			pmgr_playback_SetPlaySpeed (HUINT32 ulActionId, HINT32 nPlaySpeed);
HERROR			pmgr_playback_SetJumpPlay(HUINT32 ulActionId, HUINT32 ulTimeToPlay);
HERROR			pmgr_playback_GetPlaybackInfo(HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			pmgr_playback_GetPlaybackTimeInfo(HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			pmgr_playback_StopPlayback (HUINT32 ulActionId);
HERROR			pmgr_playback_SetAudioInfo (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e eSoundMode);
HERROR			pmgr_playback_SetSubTitleDisplay (HUINT32 ulActionId, HBOOL bDisplay);
HERROR			pmgr_playback_SetSubtitleLanguage (HUINT32 ulActionId, HUINT32 ulIndex);
HERROR			pmgr_playback_GetSubtitleDisplay (HUINT32 ulActionId, HBOOL *pbDisplay);
HERROR			pmgr_playback_GetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService);
HERROR			pmgr_playback_SetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService, HBOOL bUpdate);
HUINT32			pmgr_playback_GetPlaybackParam (MgrPb_ParamType_e eType);
HBOOL			pmgr_playback_IsPlaybackId(HUINT32 ulContentId, MgrPb_Info_t *pstInfo);

HERROR 			pmgr_playback_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSubtitleList, HINT32 *pnIndex);


#endif /* ___PMGR_PLAYBACK_H___ */
