// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __CHANNELSCANOPTIONS
#define __CHANNELSCANOPTIONS

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "NativeChannel.h"

#if defined JLABS
#include "NativeJlabsScanning.h"
#endif

namespace Ooif
{
	class ChannelScanOptions : public ObjectInstance
	{
		public:

			ChannelScanOptions();
#if defined JLABS
			ChannelScanOptions(jlabsScanningHandle handle);
#endif
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			const ScanOptions* getOpts();
			static const ClassType class_name = CLASS_CHANNELSCANOPTIONS;
		private:
			ScanOptions m_options;
	};
} /* namespace Ooif */

#endif // __CHANNELSCANOPTIONS
#endif // OIPF
