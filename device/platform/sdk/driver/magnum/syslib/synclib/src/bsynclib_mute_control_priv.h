/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_mute_control_priv.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 3/22/10 5:40p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_mute_control_priv.h $
* 
* Hydra_Software_Devel/4   3/22/10 5:40p bandrews
* SW7408-83: add unconditional video unmute
* 
* Hydra_Software_Devel/3   3/19/10 2:50p bandrews
* SW7405-3774: merge mute control
* 
* Hydra_Software_Devel/SW7405-3774/1   2/17/10 6:17p bandrews
* SW7405-3774: attempt to fix early audio unmute then mute
* 
* Hydra_Software_Devel/2   2/4/10 3:43p bandrews
* SW7405-3774: fix PIG behavior
* 
* Hydra_Software_Devel/1   1/27/10 12:57p bandrews
* SW7405-3774: adding mute control support
***************************************************************************/

#include "bsynclib.h"
#include "bsyslib.h"
#include "bsynclib_audio_source.h"
#include "bsynclib_video_source.h"

#ifndef BSYNCLIB_MUTE_CONTROL_PRIV_H__
#define BSYNCLIB_MUTE_CONTROL_PRIV_H__

bool BSYNClib_MuteControl_P_FullScreenCheck(BSYNClib_Channel_Handle hChn);
BERR_Code BSYNClib_MuteControl_P_UnmuteAll(BSYNClib_Channel_Handle hChn);

BERR_Code BSYNClib_MuteControl_P_HandleVideoSourceMutePending(BSYNClib_Channel_Handle hChn, BSYNClib_VideoSource * psSource);
BERR_Code BSYNClib_MuteControl_P_HandleAudioSourceMutePending(BSYNClib_Channel_Handle hChn, BSYNClib_AudioSource * psSource);

BERR_Code BSYNClib_MuteControl_P_TaskTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
BERR_Code BSYNClib_MuteControl_P_Process(void * pvParm1, int iParm2);

BERR_Code BSYNClib_MuteControl_P_VideoSourceUnmuteTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
BERR_Code BSYNClib_MuteControl_P_AudioSourceUnmuteTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
#if BSYNCLIB_UNCONDITIONAL_AUDIO_UNMUTE_SUPPORT
BERR_Code BSYNClib_MuteControl_P_AudioSourceUnconditionalUnmuteTimerExpired(void *pvParm1,int iParm2,BSYSlib_Timer_Handle hTimer);
#endif
#if BSYNCLIB_UNCONDITIONAL_VIDEO_UNMUTE_SUPPORT
BERR_Code BSYNClib_MuteControl_P_VideoSourceUnconditionalUnmuteTimerExpired(void *pvParm1,int iParm2,BSYSlib_Timer_Handle hTimer);
#endif

#endif /*BSYNCLIB_MUTE_CONTROL_PRIV_H__ */

