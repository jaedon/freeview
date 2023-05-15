// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __EXCEPTION
#define __EXCEPTION

#include "jsplugin.h"

#include "ObjectInstance.h"

namespace Lovefilm
{
class Exception : public ObjectInstance
{
protected:
	char *name;
	char *message;

public:
	Exception(const char *_name, const char *_message);
	virtual ~Exception();
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	// Javascript functions
};
} /* namespace Lovefilm */

#endif // __EXCEPTION
