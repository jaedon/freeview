// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __AVCOMPONENT
#define __AVCOMPONENT

#include "NativeAVComponent.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{

class AVComponentDataWrapper
{
public:
	AVComponentData data;
	void init(void *id);
	~AVComponentDataWrapper();
};

class AVComponent : public ObjectInstance,
	public DataObject<AVComponent, AVComponentDataWrapper>
{
	/*
	  The AVComponent object caches the property data from the time of
	  creation.
	*/
private:
	void init(ClassType owner_class_type);

	/** since HbbTV access permissions are different for Video
	  Broadcast objects and VideoOnDemand objects
	  (HBBTV_SECURITY), we need to keep track of the owner class
	  type */
	ClassType ownerClassType;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	AVComponent(AVComponentHandle i, ClassType owner_class_type);
	AVComponent(const AVComponent &original);
	~AVComponent();

	static void releaseNative(AVComponentHandle id);
	static const ClassType class_name = CLASS_AVCOMPONENT;
};
} /* namespace Ooif */

#endif // __AVCOMPONENT
