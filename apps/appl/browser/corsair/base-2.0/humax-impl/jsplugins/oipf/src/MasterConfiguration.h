// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  This is the object corresponding to the oipfConfiguration embedded
  object. Due to naming collision with the Configuration object
  (returned from this object) - it is named MasterConfiguration.
 */

#ifndef __MASTERCONFIGURATION
#define __MASTERCONFIGURATION

#include "jsplugin.h"
#include "ObjectInstance.h"

namespace Ooif
{
class MasterConfiguration : public ObjectInstance
{
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
};
} /* namespace Ooif */

#endif // __MASTERCONFIGURATION
