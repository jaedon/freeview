// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#ifndef __DISCINFO
#define __DISCINFO

#include "jsplugin.h"

#include "ObjectInstance.h"

namespace Ooif
{
class DiscInfo : public ObjectInstance
{
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	// Javascript functions
};
} /* namespace Ooif */

#endif // __DISCINFO
#endif // OIPF
