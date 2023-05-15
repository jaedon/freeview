/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#ifndef __CORSAIR_UTILS_H_
#define __CORSAIR_UTILS_H_

#include <queue>
#include <stdio.h>
#include "hbbtv_api.h"
#include "gogi_opera_api.h"

#include "corsair_log.h"

// Check the return value of directfb-functions to see if an error
// occurred.  Also prints a description of the error, if one occurs.
#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = ((DFBResult)x);                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
		LOG_ERR("\n");\
        DirectFBErrorFatal( #x, err );                         \
      }                                                        \
  }

class CorsairUtils
{

public:
	static void set_default_fonts(GOGI_GenericFonts &fonts);
	static bool timed_out(const struct timeval &timeout);
	static void usec_increase(struct timeval &time, int usecs);
	static int get_bpp(GOGI_FORMAT format);
};


#endif //__CORSAIR_UTILS_H_
