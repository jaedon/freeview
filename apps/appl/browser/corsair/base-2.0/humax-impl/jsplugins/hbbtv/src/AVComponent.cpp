// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "AVComponent.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Exception.h"
#include "macros.h"

using namespace Ooif;

void AVComponentDataWrapper::init(void *id)
{
	memset(&data, 0, sizeof(AVComponentData));
	NATIVE_WARN_NOK(native_AVComponentInitialize(id, &data));
}

AVComponentDataWrapper::~AVComponentDataWrapper()
{
	ENTRY;

	DATA_FREE_STRING(encoding);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	if (data.type == COMPONENT_TYPE_AUDIO)
		DATA_FREE_STRING(audio_language);
	else if (data.type == COMPONENT_TYPE_SUBTITLE)
		DATA_FREE_STRING(subtitle_language);
#else
	DATA_FREE_STRING(language);
#endif
}

void AVComponent::init(ClassType owner_class_type)
{
	classType = CLASS_AVCOMPONENT;
	ownerClassType = owner_class_type;
}


AVComponent::AVComponent(AVComponentHandle i, ClassType owner_class_type)
	: DataObject<AVComponent, AVComponentDataWrapper>(i)
{
	init(owner_class_type);
}

AVComponent::AVComponent(const AVComponent &original)
	: DataObject<AVComponent, AVComponentDataWrapper>(original)
{
	init(original.ownerClassType);
}

AVComponent::~AVComponent()
{
	ENTRY;

}

int AVComponent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	if (ownerClassType == CLASS_VIDEO_BROADCAST)
	{
		GET_SECURITY_CHECK(CHECK_BROADCAST);
	}
#endif // HBBTV_SECURITY

#ifdef OIPF
	DATA_GET_NUMBER(componentTag);
	DATA_GET_NUMBER(pid);
#endif /* OIPF */
	DATA_GET_NUMBER(type);
	DATA_GET_STRING(encoding);
	DATA_GET_BOOLEAN(encrypted);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	DATA_GET_NUMBER(componentTag);
	DATA_GET_NUMBER(pid);

	if (data_wrapper)
	{
		switch (data_wrapper->data.type)
		{
			case COMPONENT_TYPE_VIDEO:
				DATA_GET_NUMBER(aspectRatio);
				break;
			case COMPONENT_TYPE_AUDIO:
				DATA_GET_RENAMED_STRING(language, audio_language);
				DATA_GET_BOOLEAN(audioDescription);
				DATA_GET_NUMBER(audioChannels);
				break;
			case COMPONENT_TYPE_SUBTITLE:
				DATA_GET_RENAMED_STRING(language, subtitle_language);
				DATA_GET_BOOLEAN(hearingImpaired);
				break;
		}
	}
#else
	DATA_GET_NUMBER_OR_UNDEFINED(aspectRatio);
	DATA_GET_STRING(language);
	DATA_GET_BOOLEAN(audioDescription);
	DATA_GET_NUMBER(audioChannels);
	DATA_GET_BOOLEAN(hearingImpaired);
#endif
	return JSP_GET_NOTFOUND;
}

void AVComponent::releaseNative(AVComponentHandle id)
{
	NATIVE_WARN_NOK(native_AVComponentReleaseHandle(id));
}
