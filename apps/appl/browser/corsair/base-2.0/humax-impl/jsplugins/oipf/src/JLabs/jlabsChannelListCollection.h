// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __JLABS_CHANNELLISTCOLLECTION
#define __JLABS_CHANNELLISTCOLLECTION

#if defined OIPF && defined JLABS

#include "jsplugin.h"

#include "macros.h"
#include "Collection.h"
#include "ChannelList.h"

namespace Ooif
{
	class jlabsChannelListCollection : public Collection<ChannelList>
	{
	public:
		int networkTypeName;
		jlabsChannelListCollection(int networkType);
		jlabsChannelListCollection();
		~jlabsChannelListCollection();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

		int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type);
		static const ClassType class_name = CLASS_JLABS_CHANNEL_LIST_COLLECTION;
	};
} /* namespace Ooif */

#endif /* OIPF && JLABS */
#endif /* __JLABS_CHANNELLISTCOLLECTION*/

