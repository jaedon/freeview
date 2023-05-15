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
* $brcm_Workfile: nexus_audio_input.h $
* $brcm_Revision: 6 $
* $brcm_Date: 6/5/12 5:22p $
*
* API Description:
*   API name: AudioInput
*   Generic API for inputs to an audio mixer.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio_input.h $
* 
* 6   6/5/12 5:22p jgarrett
* SW7425-3071: Adding NEXUS_AudioInput_HasConnectedOutputs
* 
* 5   6/4/12 2:11p jgarrett
* SW7425-3071: Adding routines to verify connections
* 
* 4   2/7/08 11:57a vsilyaev
* PR 38682: Changed return type of XXX_Shutdown routines to void
* 
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
* 
* 2   1/23/08 2:39p erickson
* PR35457: update docs
* 
* 1   1/18/08 2:15p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/11   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
***************************************************************************/
#ifndef NEXUS_AUDIO_INPUT_H__
#define NEXUS_AUDIO_INPUT_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Shutdown the connector for an audio input/processing object.

Description:
This must be called before the corresponding object is closed.
 ***************************************************************************/
void NEXUS_AudioInput_Shutdown(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Determine if two audio nodes are connected to one another.

Description:
This routine will return true if the two nodes are linked in a filter 
graph.  Order is not important, both directions will be scanned.  It will 
also return true even if there are nodes between the two provided in this 
call. 
 ***************************************************************************/
void NEXUS_AudioInput_IsConnectedToInput(
    NEXUS_AudioInput input1,
    NEXUS_AudioInput input2,
    bool *pConnected            /* [out] True if the nodes are connected in a filter graph */
    );

/***************************************************************************
Summary:
Determine if an output is connected to a node.

Description:
This routine will return true if the input and output provided are linked 
in a filter graph.  It will also return true even if there are nodes between 
input and output.
 ***************************************************************************/
void NEXUS_AudioInput_IsConnectedToOutput(
    NEXUS_AudioInput input,
    NEXUS_AudioOutput output,
    bool *pConnected            /* [out] True if the nodes are connected in a filter graph */
    );

/***************************************************************************
Summary:
Determine if any outputs are connected to a node.

Description:
This routine will return true if the input has one or more outputs connected  
in a filter graph.  It will also return true even if there are nodes between 
input and output.
 ***************************************************************************/
void NEXUS_AudioInput_HasConnectedOutputs(
    NEXUS_AudioInput input,
    bool *pConnected            /* [out] True if one or more output is connected to this node */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_INPUT_H__ */

