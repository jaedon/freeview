/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: nexus_playback_module.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 1/18/08 2:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/playback/src/nexus_playback_module.h $
 * 
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/5   11/9/07 6:02p vsilyaev
 * PR 36788: Added timeshifting
 * 
 * Nexus_Devel/4   10/15/07 5:12p vsilyaev
 * PR 35824: Added module initialization
 * 
 * Nexus_Devel/3   10/15/07 2:55p vsilyaev
 * PR 35824: Added synchronization thunk layer
 * 
 * Nexus_Devel/2   10/11/07 6:25p vsilyaev
 * PR 35824: Added more playback code
 * 
 * Nexus_Devel/1   10/10/07 4:54p vsilyaev
 * PR 35824: Playback module
 * 
 **************************************************************************/
#ifndef NEXUS_PLAYBACK_MODULE_H__
#define NEXUS_PLAYBACK_MODULE_H__

#include "nexus_base.h"
#include "nexus_playback_thunks.h"
#include "nexus_playback.h"
#include "priv/nexus_playback_notify.h"
#include "nexus_playback_init.h"

#ifdef __cplusplus
extern "C" {
#endif

extern NEXUS_ModuleHandle NEXUS_PlaybackModule;

#define NEXUS_MODULE_SELF NEXUS_PlaybackModule

void NEXUS_Playback_RecordProgress_priv(NEXUS_PlaybackHandle p);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_PLAYBACK_MODULE_H__ */

