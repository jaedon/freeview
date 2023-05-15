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
* $brcm_Workfile: nexus_spdif_input.h $
* $brcm_Revision: 1 $
* $brcm_Date: 8/18/11 5:51p $
*
* API Description:
*   API name: SpdifInput
*    Specific APIs related to SPDIF audio inputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/include/nexus_spdif_input.h $
* 
* 1   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   7/1/11 5:51p jgarrett
* SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
*  directory structure
* 
* 5   5/30/11 5:52p jhaberf
* SWDTV-7253: removed shutdown attr as this is no longer used
* 
* 4   2/7/08 11:44p vsilyaev
* PR 38682: Added attributes for the linux kernel/user proxy mode
* 
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
* 
* 2   1/23/08 2:39p erickson
* PR35457: update docs
* 
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/2   11/29/07 10:33a jgarrett
* PR 34741: Changing settings
* 
* Nexus_Devel/1   11/27/07 5:26p jgarrett
* PR 37471: Adding I2sInput and SpdifInput
* 
* Nexus_Devel/1   9/17/07 1:33p jgarrett
* PR 34954: Splitting I2S, SPDIF, and PCM inputs and outputs
* 
***************************************************************************/

#ifndef NEXUS_SPDIF_INPUT_H__
#define NEXUS_SPDIF_INPUT_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_timebase.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: SpdifInput

Header file: nexus_spdif_input.h

Module: Audio

Description:

**************************************/

/**
Summary:
Handle for SPDIF input
**/
typedef struct NEXUS_SpdifInput *NEXUS_SpdifInputHandle;

/***************************************************************************
Summary:
SPDIF Input Settings
***************************************************************************/
typedef struct NEXUS_SpdifInputSettings
{
    bool muted;
} NEXUS_SpdifInputSettings;

/***************************************************************************
Summary:
    Get default settings for a SPDIF Input
***************************************************************************/
void NEXUS_SpdifInput_GetDefaultSettings(
    NEXUS_SpdifInputSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open a SPDIF input device
***************************************************************************/
NEXUS_SpdifInputHandle NEXUS_SpdifInput_Open(  /* attr{destructor=NEXUS_SpdifInput_Close}  */
    unsigned index,
    const NEXUS_SpdifInputSettings *pSettings
    );

/***************************************************************************
Summary:
Close a SPDIF input device
***************************************************************************/
void NEXUS_SpdifInput_Close(
    NEXUS_SpdifInputHandle handle
    );

/***************************************************************************
Summary:
Get Settings for a SPDIF Input
***************************************************************************/
void NEXUS_SpdifInput_GetSettings(
    NEXUS_SpdifInputHandle handle,
    NEXUS_SpdifInputSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
Set Settings for a SPDIF Input
***************************************************************************/
NEXUS_Error NEXUS_SpdifInput_SetSettings(
    NEXUS_SpdifInputHandle handle,
    const NEXUS_SpdifInputSettings *pSettings
    );

/***************************************************************************
Summary:
Get an audio connector for use with downstream components

Description:
Currently, SPDIF Input devices must always connect to a NEXUS_AudioDecoder downstream.
***************************************************************************/
NEXUS_AudioInput NEXUS_SpdifInput_GetConnector( 
    NEXUS_SpdifInputHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_SPDIF_INPUT_H__ */

