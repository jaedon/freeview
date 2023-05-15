/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software ASA.  All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Opera's
 * separate written consent.
 */

#ifndef __VIDEOBACKEND_LIST_H__
#define __VIDEOBACKEND_LIST_H__

#include <stdlib.h>

#include "include/operavideobackend_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void vblist_add_instance(OVB instance);

void vblist_remove_instance(OVB instance);

OVB vblist_get_latest_instance(void);

bool vblist_empty();

OVB vblist_get_playing_instance(void);

#ifdef __cplusplus
};
#endif

#endif // __VIDEOBACKEND_LIST_H__

