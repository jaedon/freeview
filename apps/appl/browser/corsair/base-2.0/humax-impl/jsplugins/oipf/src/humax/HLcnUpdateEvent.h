// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __CHANNEL_LCN_UPDATE_EVENT
#define __CHANNEL_LCN_UPDATE_EVENT

#include "Event.h"
#include "HLcnUpdate.h"

namespace Ooif
{
	class HLcnUpdateEvent : public Event
	{
	private:
		humax::LcnUpdateEvent	m_eventType;
		OOIFNumber				m_numOfSvcList;
		OOIFNumber				m_numOfNewSvc;
		OOIFNumber				m_numOfReplacedSvc;

		char*					m_svcListName;
		OOIFNumber				m_tvNum;
		OOIFNumber				m_radioNum;
		char*					m_lastUpdatedTime;
		char*					m_lastCheckedTime;
		bool					m_displayInfo;
		OOIFNumber				m_coldbootResult;

		humax::LcnUpdateSvcList_Data *svcList;
		humax::LcnUpdateSvc_Data *newSvc;
		humax::LcnUpdateSvc_Data *replacedSvc;

	public:
		HLcnUpdateEvent(jsplugin_obj *context, humax::LcnUpdateEvent _eventType,
								int _numOfSvcList, humax::LcnUpdateSvcList_Data *_svcList,
								int _numOfNewSvc, humax::LcnUpdateSvc_Data *_newSvc,
								int _numOfReplacedSvc, humax::LcnUpdateSvc_Data *_replacedSvc);

		HLcnUpdateEvent(jsplugin_obj *context, humax::LcnUpdateEvent _eventType,
							char* _svcListName, int _tvNum,
							int _radioNum, char* _lastUpdatedTime,
							char* _lastCheckedTime, bool _displayInfo,
							int _coldbootResult);

		~HLcnUpdateEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __CHANNEL_LCN_UPDATE_EVENT
#endif // OIPF


