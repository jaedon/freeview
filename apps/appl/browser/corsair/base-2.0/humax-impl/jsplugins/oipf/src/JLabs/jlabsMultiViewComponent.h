// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __JLABSMULTIVIEWCOMPONENT
#define __JLABSMULTIVIEWCOMPONENT

#include "NativeJlabsMultiViewComponent.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

namespace Ooif
{
	class jlabsMultiViewComponentDataWrapper
	{
	public:
		jlabsMultiViewComponentData data;
		void init(void *id);
		~jlabsMultiViewComponentDataWrapper();
	};

	class jlabsMultiViewComponent : public ObjectInstance,
						public DataObject<jlabsMultiViewComponent, jlabsMultiViewComponentDataWrapper>
	{

	/*
	  The jlabsMultiViewComponent object caches the property data from the time of
	  creation.
	*/

	private:
		void init(ClassType owner_class_type);
		AVComponentCollection *components(int count, AVComponentHandle *handles);

    protected:
		/** since HbbTV access permissions are different for Video
		  Broadcast objects and VideoOnDemand objects
		  (HBBTV_SECURITY), we need to keep track of the owner class
		  type */
		ClassType ownerClassType;

	public:
		jlabsMultiViewComponent(jlabsMultiViewComponentHandle i, ClassType owner_class_type);
		jlabsMultiViewComponent(const jlabsMultiViewComponent &original);
		~jlabsMultiViewComponent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative(jlabsMultiViewComponentHandle id);
		static const ClassType class_name = CLASS_JLABS_MULTIVIEW_COMPONENT;
	};
} /* namespace Ooif */

#endif // __JLABSMULTIVIEWCOMPONENT

