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
 * $brcm_Workfile: setup.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/utils/settopapi/setup.c $
 * 
 * 4   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "setup.h"
#include <assert.h>

bdisplay_t display;
bgraphics_t graphics;
bsurface_t surface;
bool surface_is_fb;

int settopapi_init(const bsettopapi_graphics *settings, bsettopapi_graphics_status *status)
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bsurface_create_settings surface_create_settings;
	bsurface_settings surface_settings;
	bsurface_t framebuffer;
	bsurface_memory surfmem;
	bvideo_format_settings format_settings;
	bdisplay_settings display_settings;

	assert(!rc);
	
	bvideo_get_format_settings(settings->display_format, &format_settings);

	display = bdisplay_open(B_ID(0));
	assert(display);
	bdisplay_get(display, &display_settings);
	display_settings.format = settings->display_format;
	if (format_settings.height > 576 || !format_settings.interlaced) {
		display_settings.svideo = NULL;
		display_settings.composite = NULL;
		display_settings.rf = NULL;
		bdisplay_set(display, &display_settings);
	}
	
	graphics = bgraphics_open(B_ID(0),display);
	assert(graphics);

	bsurface_create_settings_init(&surface_create_settings, graphics);
	surface_create_settings.pixel_format = settings->pixel_format;
	surface_create_settings.width = format_settings.width;
	surface_create_settings.height = format_settings.height;

	/* explicit framebuffer creation */
	framebuffer = bgraphics_create_framebuffer(graphics, &surface_create_settings, 
		settings->double_buffered);
	assert(framebuffer);

	surface_is_fb = !settings->use_offscreen_buffer;
	if (settings->use_offscreen_buffer) {
		/* offscreen surface */
		surface = bsurface_create(graphics, &surface_create_settings);
		assert(surface);
		bsurface_get(surface, &surface_settings);
		surface_settings.visible = true;
		bsurface_set(surface, &surface_settings);
	}
	else {
		/* draw directly to framebuffer */
		surface = framebuffer;
	}

	{
	bsettop_rect rect = {0,0,0,0};
	rect.width = format_settings.width;
	rect.height = format_settings.height;
	bsurface_fill(surface, &rect, 0);
	}
	
	bsurface_get_memory(surface, &surfmem);
	status->buffer = surfmem.buffer;
	status->pitch = surfmem.pitch;
	status->width = format_settings.width;
	status->height = format_settings.height;
	status->framebuffer = framebuffer;
	status->graphics = graphics;

	return 0;
}

void settopapi_sync()
{
	bgraphics_sync(graphics);
}

void settopapi_uninit()
{
	if (!surface_is_fb) {
		bsurface_destroy(surface);
	}
	bgraphics_close(graphics);
	bdisplay_close(display);
	bsettop_uninit();
}
