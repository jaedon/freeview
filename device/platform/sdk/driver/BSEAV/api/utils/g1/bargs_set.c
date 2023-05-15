/***************************************************************************
 *     Copyright (c) 1997-2001, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bargs_set.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/g1/bargs_set.c $
 * 
 * 1   2/7/05 7:36p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   6/2/04 2:44p erickson
 * PR11204: changed/removed deprecated settop api functions
 * 
 * Irvine_BSEAVSW_Devel/5   11/19/03 4:57p erickson
 * PR8583: resolved some naming issues, especially bvideo_format
 * 
 * Irvine_BSEAVSW_Devel/4   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/3   10/23/03 2:59p erickson
 * no longer use old enum
 * 
 * Irvine_BSEAVSW_Devel/2   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/1   9/11/03 10:29a erickson
 * added some output, format and pos options
 *
 ***************************************************************************/
#include "bargs_set.h"
#include <stdio.h>
#include <string.h>

struct {
	const char *name;
	bvideo_format fmt;
} g_formats[] = {
	{"NTSC",bvideo_format_ntsc},
	{"480P",bvideo_format_480p},
	{"PAL",bvideo_format_pal},
	{"1080I",bvideo_format_1080i},
	{"720P",bvideo_format_720p},
	{"1080I_50HZ",bvideo_format_1080i_50hz},
	{"PAL_M",bvideo_format_pal_m},
	{"PAL_N",bvideo_format_pal_n},
	{"NTSC_JAPAN",bvideo_format_ntsc_japan},
	{"720P_50HZ",bvideo_format_720p_50hz},
	{"576P",bvideo_format_576p},
	{"SECAM",bvideo_format_secam},
	{0,bvideo_format_ntsc} /* default */
};

void bargs_print_options()
{
	int i;
	printf(
	"Outputs: S,S2,C,C2,RGB,YPRPB,RFMOD,DVI\n"
	"Formats: "
	);
	for (i=0;g_formats[i].name;i++) {
		printf(g_formats[i].name);
		if (g_formats[i+1].name)
			printf(",");
	}
	printf("\n");
}

void bargs_set_outputs(const char *origoutput, bdisplay_settings *settings)
{
	char output[256];
	char *s;
	
	strncpy(output, origoutput, 256);

	settings->svideo = NULL;
	settings->composite = NULL;
	settings->component = NULL;
	settings->dvi = NULL;
	settings->rf = NULL;

	s = strtok(output, ",");
	while (s) {
		if (!strcasecmp(s, "S"))
			settings->svideo = boutput_svideo_open(B_ID(0));
		else if (!strcasecmp(s, "S2"))
			settings->svideo = boutput_svideo_open(B_ID(1));
		else if (!strcasecmp(s, "C"))
			settings->composite = boutput_composite_open(B_ID(0));
		else if (!strcasecmp(s, "C2"))
			settings->composite = boutput_composite_open(B_ID(1));
		else if (!strcasecmp(s, "RFMOD"))
			settings->rf = boutput_rf_open(B_ID(0));
		else if (!strcasecmp(s, "DVI"))
			settings->dvi = boutput_dvi_open(B_ID(0));
		else if (!strcasecmp(s, "RGB") || !strcasecmp(s, "YPRPB")) {
			settings->component = boutput_component_open(B_ID(0));
			if (settings->component) {
				boutput_component_settings component_settings;
				boutput_component_get(settings->component, &component_settings);
				component_settings.type = !strcasecmp(s, "RGB") ? boutput_component_type_rgb : boutput_component_type_yprpb;
				boutput_component_set(settings->component, &component_settings);
			}
		}
		s = strtok(NULL, ",");
	}
}

void bargs_set_format(const char *format, bdisplay_settings *settings)
{
	int i;
	for (i=0;g_formats[i].name;i++)
		if (!strcasecmp(g_formats[i].name, format)) {
			settings->format = g_formats[i].fmt;
			break;
		}
}
