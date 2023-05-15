/***************************************************************************
 *     (c)2003-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_message_swfilter_priv.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/8/12 1:22p $
 *
 * Module Description:
 * 
 * 
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/src/nexus_message_swfilter_priv.h $
 * 
 * 3   2/8/12 1:22p erickson
 * SW7335-1216: minor cleanup
 * 
 * 2   2/15/10 1:20p rjain
 * SW7550-243: Increasing MAX_FILTERs to 64
 * 
 * 1   12/11/09 3:32p erickson
 * SW7550-112: merge 7550 code. add support for sw message filtering.
 *  allow for no HW message filtering using standard XPT define.
 * 
 * Refsw_7550/4   12/2/09 3:46p rjain
 * SW7550-95: Adding PES SW filter. This works by capturing a TS stream
 *  and then extracting the PES by SW
 * 
 * Refsw_7550/3   11/13/09 2:54p rjain
 * SW7550-63: changing namespace
 * 
 * Refsw_7550/2   11/4/09 3:03p rjain
 * SW7550-63: making SW fitler nexus compliant + some clean up
 * 
 * Refsw_7550/1   11/3/09 4:13p rjain
 * SW7550-63: first draft of SW filter
 * 
 *
 ***************************************************************************/
#ifndef __NEXUS_SWFILTER_PRIV_H__
#define __NEXUS_SWFILTER_PRIV_H__

/***************************************************************************
Summary:
Callback, called when complete message is received.
***************************************************************************/
typedef void * (*NEXUS_SwFilter_Msg_P_callback_t)(void * context, size_t NEXUS_SwFilter_Msg_P_size);

struct NEXUS_SwFilter_FilterState;

/***************************************************************************
Summary:
Parameters for starting message capture.
***************************************************************************/
typedef struct NEXUS_SwFilter_MsgParams 
{
    uint16_t pid;
    NEXUS_MessageFilter filt;
    bool capture_ts;
    uint8_t * buffer;
    size_t buffer_size;
    bool disable_crc_check;
    NEXUS_SwFilter_Msg_P_callback_t callback; 
    void * context;
} NEXUS_SwFilter_MsgParams_t;

/***************************************************************************
Summary:
Initialize message filter.
***************************************************************************/
void NEXUS_SwFilter_Msg_P_Init(void);
void NEXUS_SwFilter_Msg_P_InitPes(void);

/***************************************************************************
Summary:
Process buffer with transport packets. Buffer must be aligned on start of
transport packet.
***************************************************************************/
size_t NEXUS_SwFilter_Msg_P_Feed(uint8_t * buffer, size_t size);
size_t NEXUS_SwFilter_Msg_P_FeedPes(uint8_t * buffer, size_t size);

/***************************************************************************
Summary:
Create and start filter.
***************************************************************************/
struct NEXUS_SwFilter_FilterState * NEXUS_SwFilter_Msg_P_SetFilter(NEXUS_SwFilter_MsgParams_t * params);
struct NEXUS_SwFilter_FilterState * NEXUS_SwFilter_Msg_P_SetPesFilter(NEXUS_SwFilter_MsgParams_t * params);

/***************************************************************************
Summary:
Disable and remove filter.
***************************************************************************/
void NEXUS_SwFilter_Msg_P_RemoveFilter(struct NEXUS_SwFilter_FilterState *filter);
void NEXUS_SwFilter_Msg_P_RemovePesFilter(struct NEXUS_SwFilter_FilterState *filter);

/***************************************************************************
Summary:
Reset pid state when hardware was reset.
***************************************************************************/
void NEXUS_SwFilter_Msg_P_ResetPids(void);

#endif /*__NEXUS_SWFILTER_PRIV_H__*/
