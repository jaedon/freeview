#if 0
// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  This is the object corresponding to the oipfConfiguration embedded
  object. Due to naming collision with the Configuration object
  (returned from this object) - it is named MasterConfiguration.
 */

#ifndef __HMX_MEDIA_H__
#define __HMX_MEDIA_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

namespace Ooif
{

	class HMX_Media : public ObjectInstance
	{
	private:


	public:
		HMX_Media() {}
		~HMX_Media() {}

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	};

} /* namespace Ooif */

#endif // __HMX_MEDIA_H__
#endif
