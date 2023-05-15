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
 * $brcm_Workfile: graphics_async_fill.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/graphics_async_fill.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <unistd.h>
#include <bstd.h>
#include <bkni.h>

void prompt() {
	getchar();
}

bgraphics_t graphics;

/* CPU rectangle draw */
void draw(bsurface_memory *mem, bsettop_rect *r, int color)
{
	unsigned y, x;
	for (y=r->y; y<r->y + r->height; y++) {
		unsigned long *ptr = mem->buffer + mem->pitch * y;
		ptr += r->x;
		for (x=0; x<5; x++)
			*ptr++ = color;
	}
}

void callback(void *event)
{
	BKNI_SetEvent((BKNI_EventHandle)event);
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bsurface_create_settings create_settings;
	bsurface_t fb;
	bsurface_settings surface_settings;
	bgraphics_settings g_settings;
	bsurface_memory mem;
	bdisplay_t display;
	BKNI_EventHandle event;

	if (rc) return rc;

	display = bdisplay_open(B_ID(0));
	graphics = bgraphics_open(B_ID(0),display);
	BKNI_CreateEvent(&event);

	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
	fb = bgraphics_create_framebuffer(graphics, &create_settings, false);

	bgraphics_get(graphics, &g_settings);
	g_settings.async_blits = true; /* force it to async for this test */
	g_settings.operation_complete_callback = callback;
	g_settings.callback_context = (void *)event;
	bgraphics_set(graphics, &g_settings);

	bsurface_get(fb, &surface_settings);
	bsurface_get_memory(fb, &mem);

	bsettop_rect r2 = {0,0,720,480};
	bsurface_fill(fb, &r2, 0xFF000000);
	bgraphics_sync(graphics);

	bsettop_rect r1 = {100,100,400,200};
	int i;

	for (i=0;i<1000;i++)
		bsurface_fill(fb, &r1, 0xFFFF0000);
	/* missing sync here */
	r1.x += 25; r1.y += 25;
	draw(&mem, &r1, 0xFF888888);
	bgraphics_sync(graphics);
	prompt("improper sync, so gray bar appears behind");

	for (i=0;i<1000;i++)
		bsurface_fill(fb, &r1, 0xFFFF0000);
	bsurface_sync(fb);
	r1.x += 25; r1.y += 25;
	draw(&mem, &r1, 0xFF888888);
	bgraphics_sync(graphics);
	prompt("proper sync using bsurface_sync");

	for (i=0;i<1000;i++)
		bsurface_fill(fb, &r1, 0xFFFF0000);
	bgraphics_sync(graphics);
	r1.x += 25; r1.y += 25;
	draw(&mem, &r1, 0xFF888888);
	bgraphics_sync(graphics);
	prompt("proper sync using bgraphics_sync");

	BKNI_ResetEvent(event);
	for (i=0;i<100;i++) {
		bsurface_fill(fb, &r1, 0xFFFF0000); /* can only do one per wait */
		BKNI_WaitForEvent(event, 0xFFFFFFFF);
		draw(&mem, &r1, 0xFF888888);
		bgraphics_sync(graphics);
	}
	/* this is a bit of a bogus test, but we're testing that we keep getting
	one event for every fill, and the final gray bar should be on top of the red box */
	r1.x += 25; r1.y += 25;
	prompt("proper sync using event");

	bgraphics_close(graphics);
	return 0;
}
