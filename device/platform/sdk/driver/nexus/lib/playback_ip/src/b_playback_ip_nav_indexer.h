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
* $brcm_Workfile: b_playback_ip_nav_indexer.h
* $brcm_Revision: 3 $
* $brcm_Date: 1/11/12 5:54p $
*
* Description: Header 
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_nav_indexer.h $
* 
* 3   1/11/12 5:54p ssood
* SW7425-906: fix for guard violations during fast rewind trickplays
* 
* 2   1/18/10 5:00p ssood
* SW7420-454: Add support to create index w/o using the createindex
*  utility
* 
* 1   12/8/09 1:46p ssood
* SW7420-454: Add initial support for server based trick modes (not fully
*  DLNA compliant as DLNA requires server to send MPEG compliant streams:
*  correct frame rate & w/ timing information)
* 
***************************************************************************/
#ifndef __B_PLAYBACK_IP_NAV_INDEXER__ 
#define __B_PLAYBACK_IP_NAV_INDEXER__ 
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "bstd.h"
#if 0
#include "bcmplayer.h"
#include "bcmplayer_version.h"
#endif

#define MPEG2_FRAME_TIME	60
#define AVC_FRAME_TIME		150

typedef struct trick_mode_t
{
	int modifier;
	int speed;
	int videoType;
	int minISpeed;
	int direction;
} trick_mode_t;

typedef struct index_entry_t {
	uint64_t 		offset;
	unsigned long	size;
	unsigned long pcr;
	unsigned long pts;
	int 		insertPkt;
	unsigned long 	index;
	unsigned long	type;
	unsigned char    pkt[224];
} index_entry_t;

void nav_indexer_close(void *context);
int nav_indexer_next(void *context, index_entry_t *index);
int nav_indexer_rewind(void *context);
int nav_indexer_seek(void *context, unsigned long pts, unsigned long *duration, unsigned long *first_pts);
bool nav_indexer_mode(void *context, trick_mode_t *tm);
int nav_indexer_pts(void *context, 	index_entry_t *index);
int nav_indexer_open(void **context, FILE *fp, B_PlaybackIpPsiInfoHandle psi);
void nav_indexer_setIndexByByteOffset(void *context, uint64_t byteOffset);
int nav_indexer_setIndexByTimeOffset(void *context, double timeOffset);
int nav_indexer_create( char *mediaFileName, char *indexFileName, B_PlaybackIpPsiInfoHandle psi);
#endif /* __B_PLAYBACK_IP_NAV_INDEXER__ */
