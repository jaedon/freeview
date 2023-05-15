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
 * $brcm_Workfile: graphics2.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/graphics2.c $
 * 
 * 3   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings create_settings;
	bsurface_settings surface_settings;
	bsurface_t surface;
	bsettop_rect r = {0,0,300,200};

	if (rc) goto error;

	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.width = 300;
	create_settings.height = 200;
	create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	/* on magnum systems, an implicit call to bgraphics_create_framebuffer will be made when we
	first call bsurface_create. It will be rgb565, not double buffered. */
	surface = bsurface_create(graphics, &create_settings);

	bsurface_get(surface, &surface_settings);
	surface_settings.position.x = 50;
	surface_settings.position.y = 50;
	surface_settings.visible = true;
	bsurface_set(surface, &surface_settings);

	bsurface_fill(surface, &r, 0xFFFFFFFF); /* RGB565 white */
	
	/* This will sync surface and on magnum systems, it will automatically blit the visible surface to
	the implicitly created framebuffer. */
	bgraphics_sync(graphics);

	getchar(); /* press enter to stop decode */

	bsurface_destroy(surface);
	bgraphics_close(graphics);
	bdisplay_close(display);	
	bsettop_uninit();
	return 0;
error:
	return 1;
}
