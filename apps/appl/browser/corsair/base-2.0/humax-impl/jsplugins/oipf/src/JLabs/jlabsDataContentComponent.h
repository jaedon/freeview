// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

 #ifndef __JLABSDATACONTENTCOMPONENT__
#define __JLABSDATACONTENTCOMPONENT__

#if defined OIPF && defined JLABS

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "AVComponent.h"
#include "NativeJlabsDataContentComponent.h"

namespace Ooif
{
	class jlabsDataContentComponent : public AVComponent
	{
	private:
		void init();
	public:
		jlabsDataContentComponent(AVComponentHandle i, ClassType owner_class_type);
		jlabsDataContentComponent(const jlabsDataContentComponent &original);
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static void releaseNative(AVComponentHandle id);
		static const ClassType class_name = CLASS_JLABS_DATA_CONTENT_COMPONENT;
		// JS functions
	};
} /* namespace Ooif */

#endif

#endif // __JLABSDATACONTENTCOMPONENT__
