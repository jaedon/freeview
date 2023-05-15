// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __RCT_NOTIFY_BROADCAST_EVENT
#define __RCT_NOTIFY_BROADCAST_EVENT

#include "Event.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"
#include <dlib.h>

#include <string>



namespace Ooif
{
	class RctNotifyBroadcastEvent : public Event
	{
	private:
		//OOIFBoolean display;
		//char *rctList;
		DxService_RctInfo_t *mRctInfo;
		int mLinkCount;

	public:
		//RctNotifyBroadcastEvent(jsplugin_obj *c, OOIFBoolean _display, const char *_rctList);
		RctNotifyBroadcastEvent(jsplugin_obj *c, void *aRctInfo);
		~RctNotifyBroadcastEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class RctImageIconInfo : public ObjectInstance
	{
		private:
			DxService_RctImageIconInfo_t mRctImageIconInfo;

		public:
			RctImageIconInfo(DxService_RctImageIconInfo_t* aRctImageIconInfo);
			~RctImageIconInfo();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static const ClassType class_name = CLASS_RCT_IMAGE_ICON_INFO;
	};

	class RctDvbBinaryLocator : public ObjectInstance
	{
		public:
			RctDvbBinaryLocator(DxService_RctDvbBinaryLocator_t aRctDvbBinaryLocator);
			~RctDvbBinaryLocator();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static const ClassType class_name = CLASS_RCT_DVB_BINARY_LOCATOR;
        private:
            DxService_RctDvbBinaryLocator_t mRctDvbBinaryLocator;
            std::string m_buff;


	};

	class RctLinkInfo : public ObjectInstance
	{
		private:
		DxService_RctLinkInfo_t mRctLinkInfo;

		public:
			RctLinkInfo(DxService_RctLinkInfo_t* aRctLinkInfo);
			~RctLinkInfo();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static const ClassType class_name = CLASS_RCT_INFO;
	};

	class RctInfoCollection : public Collection<RctLinkInfo>
	{
		private:
		DxService_RctInfo_t *mRctInfo;

		public:
			RctInfoCollection(DxService_RctInfo_t *aRctInfo);
			~RctInfoCollection();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			static const ClassType class_name = CLASS_RCT_INFO_COLLECTION;
	};

} /* namespace Ooif */

#endif // __RCT_NOTIFY_BROADCAST_EVENT

/* EOF */
