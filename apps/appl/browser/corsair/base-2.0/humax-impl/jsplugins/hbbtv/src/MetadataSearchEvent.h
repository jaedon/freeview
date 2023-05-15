// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __MDSEARCHEVENT
#define __MDSEARCHEVENT

#include "Event.h"
#include "MetadataSearch.h"

namespace Ooif
{
class MetadataSearchEvent : public Event
{
protected:
	int state;
	MetadataSearchHandle search;

public:
	MetadataSearchEvent(jsplugin_obj *c, int _state, MetadataSearchHandle _search);
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif //__MDSEARCHEVENT
#endif // OIPF || HBBTV_1_2_1
