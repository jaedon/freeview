// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __KEYSET
#define __KEYSET

#include "ObjectInstance.h"
#include "Application.h"
#include "Collection.h"

namespace Ooif
{
	class KeySet : public ObjectInstance
	{
	private:
		ApplicationIdentifier _appId;
		NumberCollection *getNumberCollection(int count, OOIFNumber *speeds);

	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		KeySet(ApplicationIdentifier appId);
		~KeySet();

		// Specific functions for this interface
		static int setValue(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if 1 // OIPF 2.3 added
		static int getKeyIcon(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
		static const ClassType class_name = CLASS_KEYSET;
	};
} /* namespace Ooif */

#endif // __KEYSET
