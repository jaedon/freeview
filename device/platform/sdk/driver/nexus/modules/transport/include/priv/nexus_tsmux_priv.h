/***************************************************************************
 *     (c)2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_tsmux_priv.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 12/30/10 4:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/include/priv/nexus_tsmux_priv.h $
 * 
 * 3   12/30/10 4:21p vsilyaev
 * SW7425-39: Remove playback from mux
 * 
 * 2   12/28/10 6:38p vsilyaev
 * SW7425-39: Updated to match latest TsMux
 * 
 * 1   12/28/10 5:38p vsilyaev
 * SW7425-39: Added Interface to control XPT_TsMux
 * 
 **************************************************************************/
#ifndef NEXUS_TSMUX_PRIV_H__
#define NEXUS_TSMUX_PRIV_H__

#include "nexus_types.h"
#include "bxpt_tsmux.h"


#ifdef __cplusplus
extern "C" {
#endif
typedef struct BXPT_P_TsMuxHandle *NEXUS_TsMuxHandle;
typedef struct BXPT_TsMux_Settings NEXUS_TsMuxSettings;
typedef struct BXPT_TsMux_Status NEXUS_TsMuxStatus;

void NEXUS_TsMux_GetDefaultSettings_priv(NEXUS_TsMuxSettings  *pSettings);
NEXUS_TsMuxHandle NEXUS_TsMux_Create_priv(void);
void NEXUS_TsMux_Destroy_priv(NEXUS_TsMuxHandle tsMux);
NEXUS_Error NEXUS_TsMux_AddPlaypump_priv(NEXUS_TsMuxHandle tsMux, NEXUS_PlaypumpHandle playpump);
void NEXUS_TsMux_RemovePlaypump_priv(NEXUS_TsMuxHandle tsMux, NEXUS_PlaypumpHandle playpump);
void NEXUS_TsMux_GetSettings_priv(NEXUS_TsMuxHandle tsMux, NEXUS_TsMuxSettings *pSettings); 
NEXUS_Error NEXUS_TsMux_SetSettings_priv(NEXUS_TsMuxHandle tsMux, NEXUS_TsMuxSettings *pSettings, NEXUS_StcChannelHandle stcChannel); 
void NEXUS_TsMux_GetStatus_priv(NEXUS_TsMuxHandle tsMux, NEXUS_TsMuxStatus *pStatus);


#ifdef __cplusplus
}
#endif

#endif /* NEXUS_TSMUX_PRIV_H__ */
