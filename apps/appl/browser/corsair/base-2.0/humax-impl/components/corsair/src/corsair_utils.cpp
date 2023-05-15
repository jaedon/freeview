/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include <iostream>
#include <sstream>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "corsair_utils.h"
#include "gogi_opera_api.h"
#include "corsair_product_env.h"

using namespace std;

/**
 * Sets the fonts for Opera to use. Note that the set of fonts used in
 * this reference example are not compliant with the HbbTV
 * specification. See HbbTV 1.1.1 section 10.2.1 for more details on
 * the requirements for fonts on the platform
 */
void CorsairUtils::set_default_fonts(GOGI_GenericFonts &fonts)
{
#if 1
	Font_t *font = CorsairProductEnv::getFont();

	fonts.default_font = font->default_font.c_str();
	fonts.cursive_font = font->cursive_font.c_str();
	fonts.fantasy_font = font->fantasy_font.c_str();
	fonts.monospace_font = font->monospace_font.c_str();
	fonts.sans_serif_font = font->sans_serif_font.c_str();
	fonts.serif_font = font->serif_font.c_str();
#else
	fonts.default_font = (char *) "Cumberland AMT";
	fonts.cursive_font = (char *) "Albany AMT";
	fonts.fantasy_font = (char *) "Albany AMT";
	fonts.monospace_font = (char *) "Cumberland AMT";
	fonts.sans_serif_font = (char *) "Albany AMT";
	fonts.serif_font = (char *) "Thorndale AMT";
#endif
}


bool CorsairUtils::timed_out(const struct timeval &timeout)
{
   struct timeval now;
   gettimeofday(&now, NULL);

	static struct timeval past = now;
	if (past.tv_sec > now.tv_sec)
	{
	    past = now;
	    return true;
	}
	past = now;

   return ((now.tv_sec > timeout.tv_sec) || ((now.tv_sec == timeout.tv_sec) && (now.tv_usec > timeout.tv_usec)));
}


void CorsairUtils::usec_increase(struct timeval &time, int usecs)
{
   time.tv_sec  += (time.tv_usec + usecs) / 1000000;
   time.tv_usec  = (time.tv_usec + usecs) % 1000000;
   return;
}



int CorsairUtils::get_bpp(GOGI_FORMAT format)
{
	switch (format)
	{
		case GOGI_FORMAT_VEGA_BGRA32:
		case GOGI_FORMAT_VEGA_RGBA32:
		case GOGI_FORMAT_VEGA_ABGR32:
			return 4;
		case GOGI_FORMAT_VEGA_RGBA16:
		case GOGI_FORMAT_VEGA_ABGR16:
		case GOGI_FORMAT_VEGA_RGB16:
		case GOGI_FORMAT_VEGA_BGR16:
			return 2;
	};

	return -1;
}
