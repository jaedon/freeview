// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __APPLICATIONPRIVATEDATA
#define __APPLICATIONPRIVATEDATA

#include "ObjectInstance.h"
#include "Application.h"

namespace Ooif
{
class ApplicationPrivateData : public ObjectInstance
{
private:
		ApplicationIdentifier _appId;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

#ifdef HMX_WEBUI	
private:
	OOIFBoolean	retvalPrepareWakeupOITF;
	OOIFBoolean	retvalPrepareWakeupTvtvUpdate;
	static void	getDateResults (int status, jsplugin_value *return_value, void *user_data);
	static void getDateFunctionResultObj(int status, jsplugin_value *return_value, void *user_data);
	static void getTvtvDateResults(int status, jsplugin_value *return_value, void *user_data);
	static void getTvtvDateFunctionResultObj(int status, jsplugin_value *return_value, void *user_data);
	int returnWakeupTime(jsplugin_obj *obj, jsplugin_value *result);
	int returnTvtvWakeupTime(jsplugin_obj *obj, jsplugin_value *result);
#endif

public:
		ApplicationPrivateData(ApplicationIdentifier appId);
		~ApplicationPrivateData();

	// Specific functions for this interface
	static int getFreeMem(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int prepareWakeupApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int prepareWakeupOITF(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef HMX_WEBUI
	static int prepareWakeupTvtvUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
	static int clearWakeupToken(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	static const ClassType class_name = CLASS_APPLICATIONPRIVATEDATA;
};
} /* namespace Ooif */

#endif // __APPLICATIONPRIVATEDATA

