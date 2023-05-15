/***************************************************************************
 *     (c)2007-2009 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_record_init.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 4/22/09 9:44a $
 *
 * Module Description:
 *  PVR Record
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/record/include/nexus_record_init.h $
 * 
 * 3   4/22/09 9:44a erickson
 * PR35457: clarified module handle comment
 *
 * 2   4/21/09 3:32p erickson
 * PR45902: playback module is optional. only required for timeshifting.
 *  add NEXUS_RecordModule_SetPlaybackModule for dynamic changes. add
 *  runtime check.
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/1   11/7/07 5:09p vsilyaev
 * PR 36788: PVR record module
 *
 **************************************************************************/
#ifndef NEXUS_RECORD_INIT_H__
#define NEXUS_RECORD_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_RecordModuleSettings
{
    struct {
        NEXUS_ModuleHandle file;
        NEXUS_ModuleHandle recpump; /* transport module */
        NEXUS_ModuleHandle playback; /* only required for timeshifting. */
    } modules;
} NEXUS_RecordModuleSettings;

void NEXUS_RecordModule_GetDefaultSettings(
    NEXUS_RecordModuleSettings *pSettings /* [out] */
    );

NEXUS_ModuleHandle NEXUS_RecordModule_Init(
    const NEXUS_RecordModuleSettings *pSettings
    );

void NEXUS_RecordModule_Uninit(void);

/**
Summary:
For active standby state, the Playback module is uninitialized while the Record module
stays active.
No change can be made while a timeshift session is in process.
**/
void NEXUS_RecordModule_SetPlaybackModule(
    NEXUS_ModuleHandle playback /* Set to NULL or to the Playback module */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_RECORD_INIT_H__ */

