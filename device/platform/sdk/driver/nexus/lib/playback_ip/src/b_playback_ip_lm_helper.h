/***************************************************************************
*     (c)2008-2010 Broadcom Corporation
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
* $brcm_Workfile: b_playback_ip_lm_helper.h $
* $brcm_Revision: 6 $
* $brcm_Date: 12/14/10 12:07p $
*
* Module Description:
*  Helper file for Live Media library 
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_lm_helper.h $
* 
* 6   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 5   4/2/10 5:56p ssood
* SW7468-198: Coverity fixes + RTSP related changes (requested by
*  Philips)
* 
* 4   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/1   2/16/10 10:11a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 3   10/3/08 3:35p lwhite
* PR42739: Added RTSP timeline support
* 
* 2   7/22/08 10:13a ssood
* PR42739: fixing warnings
* 
* 1   7/21/08 4:17p ssood
* PR42739: Moving LiveMedia wrapper functionality part of IP Applib
* 
****************************************************************************/

/**
Summary:
	lm_helper is a C wrapper around the LiveMedia library.
Description:
	It provides C callable functions for opening, closing and related 
    operations on a liveMedia session. 
	Each LiveMedia session supports a single RTP stream.
**/

#ifndef B_PLAYBACK_IP_LM_HELPER_H__
#define B_PLAYBACK_IP_LM_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

void *B_PlaybackIp_liveMediaOpen(void);
void B_PlaybackIp_liveMediaClose(void);

/* open a livemedia RTP or RTSP session */
B_PlaybackIpError B_PlaybackIp_liveMediaSessionOpen(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, void **context, int *fd);

/* setup livemedia RTP or RTSP session */
B_PlaybackIpError B_PlaybackIp_liveMediaSessionSetup( void *context);

/* stop a livemedia RTP or RTSP session */
void B_PlaybackIp_liveMediaSessionStop(void *context);

/* close a livemedia RTP or RTSP session */
void B_PlaybackIp_liveMediaSessionClose(void *context);

/* start a livemedia RTP or RTSP session */
B_PlaybackIpError B_PlaybackIp_liveMediaSessionStart(void *context);

/* control an RTSP session (eg FFW, REW, etc) */
void B_PlaybackIp_liveMediaSessionRTSPControl(void *context, B_PlaybackIp_RtspCtrlCmd *cmd);

/* get data from an RTP or RTSP session */
void B_PlaybackIp_liveMediaSessionGetData(void *context, void *buffer, unsigned int *buffer_size);

/* get range and position from an RTP or RTSP session */
void B_PlaybackIp_liveMediaSessionRange(void *context , unsigned long *current,  unsigned long *end);

/* get supported scale list */
void B_PlaybackIp_liveMediaGetScaleList(void *context, int *scaleListEntries, float *scaleList);
#ifdef __cplusplus
}
#endif

#endif /* B_PLAYBACK_IP_LM_HELPER_H__ */


