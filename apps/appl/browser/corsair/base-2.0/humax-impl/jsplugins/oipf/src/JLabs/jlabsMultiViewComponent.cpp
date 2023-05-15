// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "jlabsMultiViewComponent.h"
#include "jlabsDataContentComponent.h"
#include "AVComponent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"

using namespace Ooif;

void jlabsMultiViewComponentDataWrapper::init(void *id)
{
	memset(&data, 0, sizeof(jlabsMultiViewComponentData));
	NATIVE_WARN_NOK(native_jlabsMultiViewComponentInitialize(id, &data));
}

jlabsMultiViewComponentDataWrapper::~jlabsMultiViewComponentDataWrapper()
{
	ENTRY;
	DATA_FREE_STRING(name);
	if (data.avcomponents)
	{
		for( int i =0; i<data.compNum; i++ )
		{
			if( data.avcomponents[i] )
			{
				HLIB_STD_MemFree( (void *)data.avcomponents[i] );
			}
		}
		HLIB_STD_MemFree(data.avcomponents);
	}
}

void jlabsMultiViewComponent::init(ClassType owner_class_type)
{
	classType = CLASS_JLABS_MULTIVIEW_COMPONENT;
	ownerClassType = owner_class_type;
}


jlabsMultiViewComponent::jlabsMultiViewComponent(jlabsMultiViewComponentHandle i, ClassType owner_class_type)
  : DataObject<jlabsMultiViewComponent, jlabsMultiViewComponentDataWrapper>(i)
{
	init(owner_class_type);
}

jlabsMultiViewComponent::jlabsMultiViewComponent(const jlabsMultiViewComponent &original)
  : DataObject<jlabsMultiViewComponent, jlabsMultiViewComponentDataWrapper>(original)
{
	init(original.ownerClassType);
}

jlabsMultiViewComponent::~jlabsMultiViewComponent()
{
}

int jlabsMultiViewComponent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	DATA_GET_STRING(name);
	NATIVE_GET_VOIDP_COLLECTION("components", obj,
								native_jlabsMultiViewComponentGetComponents( &data_wrapper->data ,&SIZE, &NATIVE_ARRAY),
								components(SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int jlabsMultiViewComponent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

AVComponentCollection *jlabsMultiViewComponent::components(int count, AVComponentHandle *handles)
{
	AVComponentCollection *collection = new AVComponentCollection();
	AVComponentData *pstData = NULL;

	for (int i=0; i<count; i++)
	{
		pstData = NULL;

		/* 메모리 해제 타이밍은 class가 종료 될 때 release 되면서 해제 된다. */
		pstData = (AVComponentData*)HLIB_STD_MemAlloc(sizeof(AVComponentData));
		memset( pstData , 0x00, sizeof( AVComponentData ));

		if( pstData != NULL && handles != NULL)
		{
			native_AVComponentCopy( pstData, handles[i] );

			if( pstData->type == 0 )
			{ // VIDEO
				collection->addItem(new VideoComponent((AVComponentHandle)pstData, CLASS_JLABS_MULTIVIEW_COMPONENT));
			}
			else if( pstData->type == 1)
			{// AUDIO
				collection->addItem(new AudioComponent((AVComponentHandle)pstData, CLASS_JLABS_MULTIVIEW_COMPONENT));
			}
			else if( pstData->type == 2)
			{// SUBTITLE
				collection->addItem(new SubtitleComponent((AVComponentHandle)pstData, CLASS_JLABS_MULTIVIEW_COMPONENT));
			}
			else if( pstData->type == 4)
			{// DATA
				collection->addItem(new jlabsDataContentComponent((AVComponentHandle)pstData, CLASS_JLABS_MULTIVIEW_COMPONENT));
			}
			else{ /* do nothing */ }
		}
	}

	HLIB_STD_MemFree( handles );

	return collection;
}

void jlabsMultiViewComponent::releaseNative(jlabsMultiViewComponentHandle id)
{
	NATIVE_WARN_NOK(native_jlabsMultiViewComponentReleaseHandle(id));
}

