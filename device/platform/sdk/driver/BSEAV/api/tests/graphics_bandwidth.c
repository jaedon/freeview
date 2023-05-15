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
 * $brcm_Workfile: graphics_bandwidth.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/graphics_bandwidth.c $
 * 
 * 3   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

#define USE_RGB565	1
//#define USE_RGB8888 1

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings cfg;
	bsurface_t framebuffer;
	unsigned long color = 0xFF550055ul;
	bsurface_memory mem;
	unsigned i, l;
#if USE_RGB565
	unsigned short *ptr;
	bgraphics_pixel_format pixel_format=bgraphics_pixel_format_r5_g6_b5;
#elif USE_RGB8888
	unsigned long *ptr;
	bgraphics_pixel_format pixel_format=bgraphics_pixel_format_a8_r8_g8_b8;
#else
#error "Unknown color format"
#endif
	bool doublebuffered = true;

	if (rc) goto error;

	bsurface_create_settings_init(&cfg, graphics);
	cfg.pixel_format = pixel_format;
	framebuffer = bgraphics_create_framebuffer(graphics, &cfg, doublebuffered);

	/* use the bash time command to measure the length.
	to test uncached memory, modify bsettop_graphics.c, in the
	bsurface_p_set_defaults function so that it doesn't use cached
	memory.
	*/
#define TOTALLOOPS  1000

	for (l=0;l<TOTALLOOPS;l++) {
		bsurface_get_memory(framebuffer, &mem);
		ptr = mem.buffer;
		for(i=0;i<mem.pitch*cfg.height/sizeof(*ptr);i++) {
			*ptr++ = color;
		}
		if (doublebuffered)
			bgraphics_flip(graphics);
		color += 100;
	}

	return 0;
error:
	return 1;
}
