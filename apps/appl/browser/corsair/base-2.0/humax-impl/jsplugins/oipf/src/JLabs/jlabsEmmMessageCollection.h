// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */


#if defined OIPF && defined JLABS
#ifndef __JLABS_EMMMESSAGECOLLECTION
#define __JLABS_EMMMESSAGECOLLECTION

#include "jsplugin.h"


#include "macros.h"
#include "Collection.h"
#include "jlabsEmmMessage.h"

namespace Ooif
{
	class jlabsEmmMessageCollection : public Collection<jlabsEmmMessage>
	{
	public:
		jlabsEmmMessageCollection();
		jlabsEmmMessageCollection(int count, jlabsEmmMessageHandle*handles);
		~jlabsEmmMessageCollection();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		// Javascript functions
		static int deleteMsg (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int readDone (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static const ClassType class_name = CLASS_JLABS_EMM_MESSAGE_COLLECTION;
	};
} /* namespace Ooif */

#endif // __JLABS_EMMMESSAGECOLLECTION
#endif // OIPF && JLABS
