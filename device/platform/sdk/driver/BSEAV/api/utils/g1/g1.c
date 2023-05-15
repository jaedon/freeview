/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: g1.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/g1/g1.c $
 * 
 * 1   2/7/05 7:37p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   6/2/04 2:44p erickson
 * PR11204: changed/removed deprecated settop api functions
 * 
 * Irvine_BSEAVSW_Devel/8   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/7   9/11/03 10:29a erickson
 * added some output, format and pos options
 * 
 * Irvine_BSEAVSW_Devel/6   9/2/03 2:18p erickson
 * fixed warnings
 * 
 * Irvine_BSEAVSW_Devel/5   8/18/03 12:09p vsilyaev
 * Fixed compliation iisues.
 * 
 * Irvine_BSEAVSW_Devel/4   8/12/03 9:43a erickson
 * changes after pre-review meeting
 * 
 * Irvine_BSEAVSW_Devel/3   7/28/03 3:50p erickson
 * set width when it matters
 * 
 * Irvine_BSEAVSW_Devel/2   7/25/03 8:15p vsilyaev
 * Fixed for new api.
 * 
 * Irvine_BSEAVSW_Devel/1   7/8/03 3:20p erickson
 * added graphics api
 *
 ****************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "bsettop.h"
#include "pixmap.h"
#include "bargs_set.h"

#define B_ASSERT(COND) \
	do { if (!(COND)) { \
		fprintf(stderr, "g1.c failed at line %d\n", __LINE__); exit(1); }} while (0)

extern PIXMAP test1PixMap;

void LoadPixMap(unsigned char *destbuf, int pitch, const PIXMAP *pm)
{
    uint32_t i,j;
	unsigned char *srcbuf = pm->buf;

    for (i=0; i<(uint32_t)pm->height; i++)
    {
        for(j=0; j<(uint32_t)pm->pitch; j++)
			destbuf[j] = *(srcbuf++);
		destbuf += pitch;
    }
}

static void printUsage(int more)
{
	printf(
	"Options:\n"
	"  -output {S,C,RGB,YPRPB}      Chose many\n"
	"  -format {NTSC|PAL|SECAM}     Chose one. Default is NTSC.\n"
	"  -pos x,y                     Default is 100,100\n"
	"  --morehelp                   Extended options\n"
	);
	if (more) {
		printf("\n");
		bargs_print_options();
	}
}


int main(int argc, char **argv)
{
	bdisplay_t d;
	bgraphics_t g;
	bsurface_t s;
	bsurface_create_settings create_settings;
	bsurface_settings surface_settings;
	bsurface_memory mem;
	bresult rc = bsettop_init(BSETTOP_VERSION);
	int curarg = 1;
	bool changesettings = false;
	bdisplay_settings display_settings;
	int x = 100, y = 100;

	B_ASSERT(!rc);

	printf("g1 graphics test. --help for options.\n");

	d = bdisplay_open(B_ID(0));
	B_ASSERT(d);
	bdisplay_get(d, &display_settings);
	g = bgraphics_open(B_ID(0), d);
	B_ASSERT(g);

	while (curarg < argc) {
		if (!strcmp(argv[curarg], "--help")) {
			printUsage(0);
			exit(0);
		}
		else if (!strcmp(argv[curarg], "--morehelp")) {
			printUsage(1);
			exit(0);
		}
		else if (!strcmp(argv[curarg], "-output") && curarg+1 < argc) {
			bargs_set_outputs(argv[++curarg], &display_settings);
			changesettings = true;
		}
		else if (!strcmp(argv[curarg], "-format") && curarg+1 < argc) {
			bargs_set_format(argv[++curarg], &display_settings);
			changesettings = true;
		}
		else if (!strcmp(argv[curarg], "-pos") && curarg+1 < argc) {
			sscanf(argv[++curarg], "%d,%d", &x, &y);
		}
		else {
			printf("### Bad parameter: %s\n\n", argv[curarg]);
			printUsage(0);
			exit(1);
		}
		curarg++;
	}
	if (changesettings)
		bdisplay_set(d, &display_settings);

	bsurface_create_settings_init(&create_settings, g);
	create_settings.width = test1PixMap.width;
	create_settings.height = test1PixMap.height;
	s = bsurface_create(g, &create_settings);
	B_ASSERT(s);

	bsurface_get_memory(s, &mem);
	LoadPixMap(mem.buffer, mem.pitch, &test1PixMap);

	bsurface_get(s, &surface_settings);
	surface_settings.visible = true;
	surface_settings.position.x = x;
	surface_settings.position.y = y;
	bsurface_set(s, &surface_settings);

	printf("Press return to exit:");
	fflush(stdout);
	getchar();

    return 0;
}
