/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_spdif_output.h $
* $brcm_Revision: 10 $
* $brcm_Date: 10/19/12 2:19p $
*
* API Description:
*   API name: SpdifOutput
*    Specific APIs related to SPDIF audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_spdif_output.h $
* 
* 10   10/19/12 2:19p jgarrett
* SW7358-417: Merge to main branch
* 
* SW7358-417/1   10/3/12 12:21p mpeteri
* SW7358-417: Changes to allow NEXUS to configure HDMI audio burst
*  settings.
* 
* 9   4/22/11 10:43a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 8   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 7   12/3/10 5:50p jgarrett
* SW35230-2472: Adding CGMS-A settings for SPDIF
* 
* 6   2/19/09 11:13a jgarrett
* PR 52181: Adding dither option
* 
* 5   2/7/08 12:11p vsilyaev
* PR 38692: Added attributes for shutdown functions
* 
* 4   2/6/08 4:28p vsilyaev
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
* Nexus_Devel/9   1/9/08 5:56p jgarrett
* PR 38535: Adding option for raw channel status
* 
* Nexus_Devel/8   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
* Nexus_Devel/7   11/6/07 4:25p jgarrett
* PR 34954: Fixing include merge bug
*
* Nexus_Devel/6   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/5   11/5/07 3:38p erickson
* PR36725: added #include
*
* Nexus_Devel/5   11/5/07 2:13p erickson
* PR36725: added required #include
*
* Nexus_Devel/4   10/4/07 2:25p erickson
* PR34594: removed audio descriptors
*
* Nexus_Devel/3   9/27/07 9:18p jgarrett
* PR 34954: Adding SPDIF
*
* Nexus_Devel/2   9/27/07 7:52p jgarrett
* PR 34954: Changing some prototypes
*
* Nexus_Devel/1   9/17/07 1:33p jgarrett
* PR 34954: Splitting I2S, SPDIF, and PCM inputs and outputs
*
***************************************************************************/

#ifndef NEXUS_SPDIF_OUTPUT_H__
#define NEXUS_SPDIF_OUTPUT_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: SpdifOutput

Header file: nexus_spdif_output.h

Module: Audio

Description: Route PCM or compressed audio to a SPDIF output

**************************************/

/**
Summary:
Handle for SPDIF output
**/
typedef struct NEXUS_SpdifOutput *NEXUS_SpdifOutputHandle;

/***************************************************************************
Summary:
SPDIF Output Settings
***************************************************************************/
typedef struct NEXUS_SpdifOutputSettings
{
    bool    limitTo16Bits;  /* If true, the output will be limited to 16 bits */

    bool    dither;         /* If true, a dither signal will be sent out when
                               there is no data on this output in PCM mode. */

    NEXUS_SpdifOutputBurstType burstType; /* Burst type insertion for compressed data */

    /* The information below can be changed while a decode is in progress */
    /* These values can be overridden with NEXUS_SpdifOutput_SetRawChannelStatus */
    struct
    {
        bool           professionalMode;    /* [0:0] The professional mode flag.
                                                TRUE: Professional mode. Other user
                                                options will not be considered.
                                                FALSE: Consumer mode.*/
        bool           swCopyRight;         /* [2:2] Software CopyRight assert.
                                                TRUE: CopyRight is asserted
                                                FALSE: CopyRight is not asserted */
        uint16_t       categoryCode;        /* [8:15] Category Code */
        uint16_t       clockAccuracy;       /* [28:29] Clock Accuracy */
        bool           separateLRChanNum;   /* TRUE:  Left channel num = 0000
                                                      Right Channel Num = 0000
                                               FALSE: Left channel num = 1000
                                                      Right Channel Num = 0100 */
        uint8_t        cgmsA;               /* CGMS-A copy bits.  Typical values are 
                                               0=Copy Freely 1=Copy Once 
                                               2=Reserved 3=Copy Never */
    } channelStatusInfo;
} NEXUS_SpdifOutputSettings;

/***************************************************************************
Summary:
Get default settings for a SPDIF Output
***************************************************************************/
void NEXUS_SpdifOutput_GetDefaultSettings(
    NEXUS_SpdifOutputSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open a SPDIF Output
***************************************************************************/
NEXUS_SpdifOutputHandle NEXUS_SpdifOutput_Open( /* attr{destructor=NEXUS_SpdifOutput_Close}  */
    unsigned index,
    const NEXUS_SpdifOutputSettings *pSettings     /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close a SPDIF Output

Description:
Input to the SPDIF output must be removed prior to closing.
***************************************************************************/
void NEXUS_SpdifOutput_Close(
    NEXUS_SpdifOutputHandle handle
    );

/***************************************************************************
Summary:
Get Settings for a SPDIF Output
***************************************************************************/
void NEXUS_SpdifOutput_GetSettings(
    NEXUS_SpdifOutputHandle handle,
    NEXUS_SpdifOutputSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
Set Settings for a SPDIF Output
***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_SetSettings(
    NEXUS_SpdifOutputHandle handle,
    const NEXUS_SpdifOutputSettings *pSettings
    );

/***************************************************************************
Summary:
Get the audio connector for a SPDIF output
***************************************************************************/
NEXUS_AudioOutput NEXUS_SpdifOutput_GetConnector(
    NEXUS_SpdifOutputHandle handle
    );

/***************************************************************************
Summary:
Program the transmitted channel status in raw format.

Description:
This API will set the SPDIF channel status bits using raw 64-bit values
per channel.  Not available on all platforms.  Once set, the values in
NEXUS_SpdifOutputSettings will be ignored.  To clear these settings, pass
NULL for pChannelStatus.
***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_SetRawChannelStatus(
    NEXUS_SpdifOutputHandle handle,
    NEXUS_AudioRawChannelStatus *pChannelStatus
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_SPDIF_OUTPUT_H__ */

