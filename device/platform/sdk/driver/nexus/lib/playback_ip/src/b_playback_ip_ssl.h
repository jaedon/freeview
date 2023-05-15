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
 * $brcm_Workfile: b_playback_ip_ssl.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 3/4/10 12:17p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_ssl.h $
 * 
 * 7   3/4/10 12:17p ssood
 * SW7420-561: merge from branch to mainline
 * 
 * SW7420-561/1   2/16/10 10:11a ssood
 * SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
 *  Philips RTSP requirements
 * 
 * 6   6/10/09 8:36p lwhite
 * PR53773: Add RadEa time/byte offset
 * 
 * 5   4/16/09 5:19p lwhite
 * PR53373: Clone session support
 * 
 * 4   4/10/09 3:48p lwhite
 * PR53773: Suspend support added
 * 
 * 3   4/7/09 5:16p lwhite
 * PR53773: Enable certificate checking
 * 
 * 2   4/6/09 3:35p lwhite
 * PR53773: Added SSL suspend support
 * 
 * 1   4/3/09 3:17p lwhite
 * PR53773: Added SSL support
*
****************************************************************************/
#ifndef B_PLAYBACK_IP_SSL_H__
#define B_PLAYBACK_IP_SSL_H__


void B_PlaybackIp_SslUnInit(void *);

int B_PlaybackIp_SslSessionOpen(
    B_PlaybackIpSessionOpenSettings *socketOpenSettings,         /* input: provides server ip, port info */
    int sd,                                                     /* input: socket descriptor */
    B_PlaybackIpSecurityOpenOutputParams *securityOpenOutputSettings);/* output: security return params */

#endif /* B_PLAYBACK_IP_SSL_H__ */
