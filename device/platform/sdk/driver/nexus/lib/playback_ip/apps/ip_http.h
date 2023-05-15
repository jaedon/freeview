/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
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
 * $brcm_Workfile: ip_http.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 1/23/10 11:16a $
 *
 * Module Description: http server module declarations
 *  
 * Revision History:
 *    
 * $brcm_Log: /nexus/lib/playback_ip/apps/ip_http.h $
 * 
 * 6   1/23/10 11:16a ssood
 * SW7420-454: added support to extract 1st PTS from TS files, store in
 *  info file & send to clients in HTTP resp
 * 
 * 5   1/19/10 10:17a ssood
 * SW7420-454: enhanced streaming test app to generate info & nav files
 *  during startup and publish channel map for brutus via port 5002
 * 
 * 4   12/17/09 2:07p ssood
 * SW7420-454: Add option to just open IP Streamer & IP library contexts
 *  for Media Probe purposes
 * 
 * 3   11/17/09 2:58p ssood
 * SW7420-454: Enhance IP Streamer to stream files from local disk
 * 
 * 2   10/11/09 8:52p ssood
 * SW7420-340: added support to bind ip_streamer to a particular network
 *  interface
 * 
 * 1   9/16/09 10:50a ssood
 * SW7420-340: Adding example apps for demonstrating IP Streaming & Client
 *  features of IP Applib
 * 
 *************************************************************/ 
#ifndef __IP_HTTP_HDR__
#define __IP_HTTP_HDR__
extern char *acceptNewHttpRequest(int listeningFd, char *requestUrl, int requestUrlLen, int *streamingFd);
extern int getRequestInfoFromUrl(IpStreamerOpenSettings *openSettings, IpStreamerConfig *cfg, IpStreamerGlobalCfg *ipStreamerGlobalCfg);
extern void closeHttpSession(int streamingFd);
extern int initTcpServer(IpStreamerGlobalCfg *cfg);
extern void unInitTcpServer(int listeningFd);
extern int sendHttpResponse(IpStreamerConfig *ipStreamerCfg, int sd, B_PlaybackIpPsiInfo *psi);
extern int waitForNetworkEvent(int sd);

/* default port to receive requests for streaming live channels */
#define LIVE_STREAMER_DEFAULT_SERVER_PORT 5000
#endif /*__IP_HTTP_HDR__*/
