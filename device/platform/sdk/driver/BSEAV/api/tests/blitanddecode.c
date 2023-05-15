/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: blitanddecode.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/blitanddecode.c $
 * 
 * 4   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>

unsigned gettime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings cfg;
	bsurface_t framebuffer;
	bsurface_memory mem;
	bsurface_settings s;
	bsettop_rect rect = {0,0,720,480};
	int i;
	bdecode_t decode = bdecode_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	unsigned diff;

	if (rc) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x1322;
	mpeg.pcr_pid = 0x1322;
	band = bstreamer_attach( B_ID(0), bstream_mpeg_type_ts );
	if (!band) goto error;
	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;
	if (bdecode_start(decode, stream, window))
		goto error;
		
	bsurface_create_settings_init(&cfg, graphics);

	framebuffer = bsurface_create(graphics, &cfg);
	bsurface_get(framebuffer, &s);
	s.visible = true;
	bsurface_set(framebuffer, &s);
	bsurface_fill(framebuffer, &rect, 0xFF000000);

	diff = gettime();
	bsurface_get_memory(framebuffer, &mem);
	for(i=0;i<10000;i++) {
		unsigned x = rand() % cfg.width;
		unsigned y = rand() % cfg.height;
		unsigned short *buf = (unsigned short *)((unsigned)mem.buffer+y*mem.pitch);
		buf[x] = rand();
		if (i%10000==0) printf(".");

		/* because we're drawing to the framebuffer and not double-buffered,
		this only serves to flush cached memory. In general, you should always
		use it. */
		bgraphics_sync(graphics);
	}
	diff = gettime() - diff;
	
	printf("TIME %d ms\n", diff);
	
	bdecode_stop(decode);
	bstream_close(stream);

	return 0;
error:
	printf("error\n");
	return 1;
}
