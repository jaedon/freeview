/***************************************************************************
*	  Copyright (c) 2004-2012, Broadcom Corporation
*	  All Rights Reserved
*	  Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_channel_priv.h $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 2/9/12 9:17p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_channel_priv.h $
* 
* Hydra_Software_Devel/12   2/9/12 9:17p bandrews
* SW7358-219: really apply zero delay to sinks with mismatched rates
* 
* Hydra_Software_Devel/11   11/15/11 7:07p bandrews
* SW7425-1235: merge to main
* 
* Hydra_Software_Devel/SW7425-1235/1   10/31/11 3:18p bandrews
* SW7425-1235: need more timers for 6 audio channels
* 
* Hydra_Software_Devel/10   7/8/11 2:40p bandrews
* SW7405-3884: punch RMD settings through properly, hParent wasn't set
* during create
* 
* Hydra_Software_Devel/9   7/2/10 6:04p bandrews
* SW7405-4436: added channel index to algo debug messages
* 
* Hydra_Software_Devel/8   6/15/10 6:42p bandrews
* SW7405-4471: only perform lipsync adjustments under correct conditions
* 
* Hydra_Software_Devel/7   5/28/10 6:42p bandrews
* SW7405-4436: print channel index in debug messages
* 
* Hydra_Software_Devel/6   3/22/10 5:41p bandrews
* sw7408-83: bump timer count; add summary comments to functions
* 
* Hydra_Software_Devel/5   3/19/10 2:48p bandrews
* SW7405-3774: merge mute control
* 
* Hydra_Software_Devel/SW7405-3774/1   2/17/10 6:17p bandrews
* SW7405-3774: attempt to fix early audio unmute then mute
* 
* Hydra_Software_Devel/4   1/26/10 9:00p bandrews
* SW7405-3774: added mute control support
* 
* Hydra_Software_Devel/3   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/2   11/26/08 5:10p bandrews
* PR49294: Added precision lipsync flag
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/6   2/26/08 10:21p bandrews
* PR37951: Fixed units.  Implemented static rate mismatch detection.
* 
* Hydra_Software_Devel/5   2/26/08 3:03p bandrews
* PR37951: Added video delay requantization
* 
* Hydra_Software_Devel/4   2/21/08 6:02p bandrews
* PR37951: Default video source numbers to 45KHz domain.  Improved
* handling of unsynchronized elements
* 
* Hydra_Software_Devel/3   2/20/08 10:03p bandrews
* PR37951: Updated based on feedback from usage
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:53p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsyslib_list.h"
#include "bsynclib.h"
#include "bsynclib_audio_sink.h"
#include "bsynclib_audio_source.h"
#include "bsynclib_rate_mismatch_detector.h"
#include "bsynclib_resource_pool.h"
#include "bsynclib_state_machine.h"
#include "bsynclib_timer.h"
#include "bsynclib_video_sink.h"
#include "bsynclib_video_source.h"

#ifndef BSYNCLIB_CHANNEL_PRIV_H__
#define BSYNCLIB_CHANNEL_PRIV_H__

#define BSYNCLIB_TIMER_COUNT 18

extern const unsigned int auiAudioSamplingRates[];

typedef struct
{
	int TBD;
} BSYNClib_Channel_Path_Data;

typedef struct
{
	unsigned int uiMaxSourceDelay;
	void * pvMaxDelaySource;

    bool bInconsistentSinkDomains;
	unsigned int uiMaxSinkDelay;
	void * pvMaxDelaySink;

	unsigned int uiMaxPathDelay;
} BSYNClib_Channel_Path_Results;

typedef struct
{
	BSYNClib_Channel_Handle hChn;
	BSYSlib_List_Handle hSources;
	unsigned int uiSourceCount;

	BSYSlib_List_Handle hSinks;
	unsigned int uiSinkCount;

	BSYNClib_Channel_Path_Data sData;
	BSYNClib_Channel_Path_Results sResults;
} BSYNClib_Channel_Path;

/*
Summary:
*/
typedef struct
{
	bool bEnabled; 
} BSYNClib_Channel_Data;

/*
Summary:
*/
typedef struct
{
	bool bSyncTaskScheduled;
	bool bMuteTaskScheduled;

	unsigned int uiMaxPathDelay;
	BSYNClib_Channel_Path * psMaxDelayPath;
} BSYNClib_Channel_Results;

/*
Summary:
*/
struct BSYNClib_Channel_Impl
{
	bool bEnabled;

	bool bPrecisionLipsyncEnabled;

	BSYNClib_Handle hParent;
	BSYNClib_Channel_Settings sSettings;
	BSYNClib_Channel_Config sConfig;
	BSYNClib_Channel_Status sStatus;
	
	BSYNClib_Channel_Path sVideo;
	BSYNClib_Channel_Path sAudio;

	BSYNClib_Channel_Results sResults;

	BSYNClib_ResourcePool * psTimers;

	BSYNClib_Timer * psTaskTimer;
	BSYNClib_Timer * psMuteControlTaskTimer;

	BSYNClib_StateMachine_Handle hMachine;
	BSYNClib_RateMismatchDetector_Handle hDetector;
	int iIndex;
};

/*
Summary:
Opens an SYNC lib channel
Description:
*/
BERR_Code BSYNClib_Channel_P_Create(
	const BSYNClib_Settings * psLibSettings,
	const BSYNClib_Channel_Settings * psSettings,
	BSYNClib_Channel_Handle * phChn /* [out] */
);

/*
Summary:
Closes an SYNC lib channel
Description:
*/
void BSYNClib_Channel_P_Destroy(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
Returns the default configuration of a sync channel
*/
void BSYNClib_Channel_P_GetDefaultConfig(
	BSYNClib_Channel_Config * psConfig
);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_StartTimer_isr(
	BSYNClib_Channel_Handle hChn,
	BSYNClib_Timer * psTimer, 
	unsigned long ulTimeout,
	BSYSlib_Timer_ExpiryHandler pfTimerExpired,
	void * pvParm1,
	int iParm2
);

BERR_Code BSYNClib_Channel_P_CancelTimer_isr(
	BSYNClib_Channel_Handle hChn,
	BSYNClib_Timer * psTimer
);

/*
Summary:
Handles expiry of a sync timer
*/
BERR_Code BSYNClib_Channel_P_TimerExpired(
	BSYNClib_Channel_Handle hChn,
	BSYSlib_Timer_Handle hTimer
);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_ProcessConfig(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
*/
bool BSYNClib_Channel_P_SyncCheck(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
*/
void BSYNClib_Channel_P_Snapshot_isr(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_Process(void * pvParm1, int iParm2);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_ApplyDelays(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
bool BSYNClib_Channel_P_AudioVideoSyncCheck(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
bool BSYNClib_Channel_P_AudioAudioSyncCheck(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
bool BSYNClib_Channel_P_VideoVideoSyncCheck(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_Synchronize(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_GenerateCallbacks(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_GenerateDelayCallback(BSYNClib_Channel_SetDelay pfSetDelay, void * pvParm1, int iParm2, unsigned int uiDeviceIndex, BSYNClib_DelayElement * psData);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_GenerateDelayNotificationCallback(BSYNClib_Channel_SetDelayNotification pfSetDelayNotification, void * pvParm1, int iParm2, unsigned int uiDeviceIndex, BSYNClib_DelayElement * psData);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_ScheduleTask_isr(BSYNClib_Channel_Handle hChn);

/*
Summary:
*/
BERR_Code BSYNClib_Channel_P_TaskTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);

/*
Summary:
*/
bool BSYNClib_Channel_P_Enabled(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
*/
bool BSYNClib_Channel_P_Enabled_isr(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
*/
bool BSYNClib_Channel_P_PredictMadStateChange_isr(
	BSYNClib_Channel_Handle hChn, 
	BSYNClib_VideoSink * psSink
);

/*
Summary:
*/
void BSYNClib_Channel_P_ResetVideoSourceJtiFactor_isr(
	BSYNClib_Channel_Handle hChn
);

/*
Summary:
*/
void BSYNClib_Channel_P_GetDefaultStatus(
	BSYNClib_Channel_Status * psStatus
);

/*
Summary:
*/
void BSYNClib_Channel_P_Stop(
	BSYNClib_Channel_Handle hChn
);

#if 0
void bsync_p_init(void);
void bsync_p_uninit(void);
void bsync_p_init_channel(bsync_t sync);
void bsync_p_init_settings(bsync_settings * settings);
void bsync_p_get(bsync_settings * settings);
void bsync_p_set(bsync_settings * settings);

#if B_N_DISPLAYS > 1
void bsync_p_sync_simul_windows(bsync_t sync);
#endif

/* delay calculators */
void bsync_p_calculate_max_pts_to_vsync_delay(bsync_t sync);
void bsync_p_requantize_delay(bsync_t sync, unsigned int delay, unsigned int * p_requantized_delay);
#if 0
void bsync_p_calculate_max_vec_delay(bsync_t sync);
#endif
void bsync_p_calculate_current_post_proc_delay(bsync_t sync);
void bsync_p_calculate_max_afb_delay(bsync_t sync);
void bsync_p_calculate_current_rap_audio_path_delay(bsync_t sync, unsigned * p_audio_path_delay);
void bsync_p_calculate_current_pts_to_vsync_delay(bsync_t sync);
#if BSYNC_PASS_WINDOW_CORRECTION_SUPPORT
void bsync_p_calculate_video_pass_window_correction_factor(bsync_t sync, unsigned int current_video_delay, int * p_correction_factor, unsigned int * p_additional_video_delay);
#endif
void bsync_p_calculate_current_xvd_video_path_delay(bsync_t sync, unsigned * p_video_path_delay);

/* av offset mutators */
void bsync_p_apply_rap_delay(bsync_t sync, long lRAPDelay);
void bsync_p_apply_xvd_delay(bsync_t sync, long lXVDDelay);

/* interrupt and event handlers */
void bsync_p_config_changed_event_handler(void * sync_);
void bsync_p_video_post_proc_delay_changed_event_handler(void * sync_);

void bsync_p_video_post_proc_delay_notifier_isr(void * sync_, int iParm2, void * pvVdcData);
#if BSYNC_XVD_PTS_STC_OFFSET_SUPPORT
void bsync_p_xvd_phase_notifier_isr(void * sync_, int iParm1, void * pvParm2);
void bsync_p_xvd_phase_accept(bsync_t sync);
void bsync_p_xvd_tsm_lock_timer_event_handler(void * sync_);
void bsync_p_xvd_phase_event_handler(void * sync_);
void bsync_p_audio_unmute_timer_event_handler(void * sync_);
void bsync_p_video_unmute_timer_event_handler(void * sync_);
void bsync_p_source_to_display_rate_mismatch_detector(void * sync_);
void bsync_p_rate_mismatch_detection_timer_event_handler(void * sync_);
void bsync_p_video_rate_mismatch_detection_reset(bsync_t sync);
#endif

void bsync_p_idle_sync_task(void * sync_);

bool bsync_p_sync_simul_check(bsync_t sync);

#if BSYNC_UNCONDITIONAL_AUDIO_UNMUTE_SUPPORT
void bsync_p_unconditional_audio_unmute_timer_event_handler(void * sync_);
#endif

void bsync_p_get_picture_info_isr(bvideo_format_settings * format, unsigned long height, unsigned long width, BAVC_FrameRateCode eFrameRateCode, bool bStreamProgressive);

int bsync_p_find_window_index(bsync_t sync, bdecode_window_t window);
bsync_t bsync_p_find_sync_by_window(bdecode_window_t window);
int bsync_p_find_display_index(bsync_t sync, bdisplay_t display);
bsync_t bsync_p_find_sync_by_display(bdisplay_t display);
bsync_t bsync_p_find_sync_by_xvd_channel(struct b_base_xvd_channel * xvd_channel);
bool bsync_p_predict_vdc_mad_state_change(bsync_t sync);
bsync_t bsync_p_alloc_sync_channel();
void bsync_p_free_sync_channel(bsync_t sync);
void bsync_p_acquire(bsync_t sync, bdecode_t decode);
void bsync_p_release(bdecode_t decode);
void bsync_p_video_unmute(bsync_t sync);
void bsync_p_audio_unmute(bsync_t sync);
void bsync_p_video_picture_params_handler_isr(void * sync_, BXVD_PictureParameterInfo * info);
void bsync_p_video_channel_started_handler(bsync_t sync);
void bsync_p_video_channel_stopped_handler(bsync_t sync);
void bsync_p_audio_channel_started_handler(bsync_t sync, bool primary);
void bsync_p_audio_channel_stopped_handler(bsync_t sync, bool primary);

#endif

#endif /* BSYNCLIB_CHANNEL_PRIV_H__ */

