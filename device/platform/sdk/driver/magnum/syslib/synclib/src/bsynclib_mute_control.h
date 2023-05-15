/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_mute_control.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 3/18/10 10:03p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_mute_control.h $
* 
* Hydra_Software_Devel/2   3/18/10 10:03p bandrews
* SW7405-3774: fixed case where video may unmute even with pending
* adjustment
* 
* Hydra_Software_Devel/1   1/26/10 9:00p bandrews
* SW7405-3774: added mute control support
***************************************************************************/

#include "bsynclib.h"

#ifndef BSYNCLIB_MUTE_CONTROL_H__
#define BSYNCLIB_MUTE_CONTROL_H__

BERR_Code BSYNClib_MuteControl_ScheduleTask_isr(BSYNClib_Channel_Handle hChn);
BERR_Code BSYNClib_MuteControl_StartUnmuteTimers(BSYNClib_Channel_Handle hChn);
BERR_Code BSYNClib_MuteControl_CancelUnmuteTimers_isr(BSYNClib_Channel_Handle hChn);

#endif /*BSYNCLIB_MUTE_CONTROL_H__ */

