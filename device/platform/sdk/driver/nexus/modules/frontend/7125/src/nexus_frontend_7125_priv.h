/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_frontend_7125_priv.h $
* $brcm_Revision: 2 $
* $brcm_Date: 5/27/11 4:08p $
*
* API Description:
*   API name: BCM7125 Frontend Private
*    Private interfaces between 7125 frontend modules
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/7125/src/nexus_frontend_7125_priv.h $
* 
* 2   5/27/11 4:08p mward
* SW7125-926:  Use NEXUS_Amplifier for LNA, rather than direct control in
*  tnr/7125.
* 
* SW7125-926/1   4/28/11 5:30p mward
* SW7125-926:  Add update (LNA) gain callback, LNA gain configuration
*  setting for 7125 tuner/DS.
* 
* 1   7/12/10 3:31p mward
* SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
*  adapt 7550 implementation.
* 
* 
***************************************************************************/
#include "nexus_frontend_module.h"
#include "btnr_7125.h"

#ifndef NEXUS_FRONTEND_7125_PRIV_H__
#define NEXUS_FRONTEND_7125_PRIV_H__

NEXUS_Error NEXUS_Tuner_P_Install7125AdsInterrupt(
    NEXUS_TunerHandle tuner,
    BTNR_7125_AdsInterruptCallback callback,
    void *pParam1, 
    int param2
    );

void NEXUS_Tuner_P_Remove7125AdsInterrupt(
    NEXUS_TunerHandle tuner
    );

NEXUS_Error NEXUS_Tuner_SetTunerConfigSettings7125(
    NEXUS_TunerHandle handle,
    const NEXUS_7125ConfigSettings *pConfigSettings
    );

#endif /* #ifndef NEXUS_FRONTEND_7125_PRIV_H__ */

