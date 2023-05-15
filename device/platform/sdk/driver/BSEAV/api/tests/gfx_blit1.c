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
 * $brcm_Workfile: gfx_blit1.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/gfx_blit1.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void prompt(const char *msg)
{
	printf("%s\n", msg);
	getchar();
}

int fill(void *buffer, int mode) 
{
	int i, j;
	for (i=0;i<256;i++) {
		unsigned char *ptr = buffer + i*256*4;
		for (j=0;j<256;j++) {
			switch (mode) {
			case 0:
				ptr[j*4] = 0x00;
				ptr[j*4+1] = 0xFF; /* green */
				ptr[j*4+2] = 0x00;
				ptr[j*4+3] = 255-j; /* alpha */
				break;
			case 1:
				ptr[j*4] = 0x00;
				ptr[j*4+1] = 0x00;
				ptr[j*4+2] = 0xFF; /* red */
				ptr[j*4+3] = j; /* alpha */
				break;
			}
		}
	}
	return 0;
}


int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings cfg;
	bsurface_t framebuffer;
	bsurface_t surface1;
	bsurface_memory mem, fbmem;
	bsettop_rect fbrect = {0,0,720,480};
	bsettop_rect srcrect = {0,0,256,256};

	if (rc) return -1;

	bsurface_create_settings_init(&cfg, graphics);
	cfg.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
	framebuffer = bgraphics_create_framebuffer(graphics, &cfg, false);
	
	cfg.width = srcrect.width;
	cfg.height = srcrect.height;
	surface1 = bsurface_create(graphics, &cfg);
	bsurface_get_memory(surface1, &mem);
	bsurface_get_memory(framebuffer, &fbmem);
	fill(mem.buffer, 1);

#define test_pixel(VALUE) assert(*(unsigned long*)fbmem.buffer == (VALUE))

	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		NULL, NULL,
		0xFF, 0xFF);
	test_pixel(0xFFFF0000);
	prompt("bright, solid (no gradient) red box");
	
	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		framebuffer, &srcrect,
		0xFF, 0xFF);
	test_pixel(0xFFFF0000);
	prompt("bright, solid (no gradient) red box");

	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		NULL, NULL,
		0xFF, 0xA0);
	test_pixel(0xA0FF0000);
	prompt("dim, solid (no gradient) red box");

	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		NULL, NULL,
		0xFF, 0xA0);
	test_pixel(0xA0FF0000);
	prompt("dim, solid red box"); 
	
	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		framebuffer, &srcrect,
		0xA0, 0xA0);
	test_pixel(0xA0A00000);
	prompt("more dim, solid red box"); /* alpha is applied to blit and 
		final alpha per pixel */
	
	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		framebuffer, &srcrect,
		0, 0xFF);
	test_pixel(0xFF000000);
	prompt("bright, gradient red box");
	
	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		surface1, &srcrect,
		framebuffer, &srcrect,
		0, 0xA0);
	test_pixel(0xA0000000);
	prompt("dim, gradient red box");
	
	bsurface_fill(framebuffer, &fbrect, 0xFF000000);
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
		surface1, &srcrect,
		framebuffer, &srcrect,
		0, 0);
	test_pixel(0x00000000);
	prompt("bright->verydim, gradient red box");
	
	/* test failure cases */
	BKNI_Printf("Error expected...\n");
	rc = bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
		surface1, &srcrect,
		NULL, NULL, /* NOTE: if null, must use BSURFACE_BLEND_WITH_PIXEL1 */
		0, 0);
	assert(rc);
	
	BKNI_Printf("Error expected...\n");
	rc = bsurface_blit(NULL, &srcrect, /* dest required */
		BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
		surface1, &srcrect,
		NULL, NULL,
		0, 0);
	assert(rc);
	
	BKNI_Printf("Error expected...\n");
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
		NULL, &srcrect, /* src1 require if specified */
		framebuffer, &srcrect,
		0, 0);
	assert(rc);
	
	BKNI_Printf("Error expected...\n");
	bsurface_blit(framebuffer, &srcrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC2_ALPHA,
		surface1, &srcrect,
		NULL, &srcrect, /* src2 require if specified */
		0, 0);
	assert(rc);
	
	return 0;
}
