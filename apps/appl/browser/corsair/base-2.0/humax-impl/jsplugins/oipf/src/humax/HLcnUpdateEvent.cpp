// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HLcnUpdateEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

HLcnUpdateEvent::HLcnUpdateEvent(
	jsplugin_obj *context, humax::LcnUpdateEvent _eventType,
				int _numOfSvcList, humax::LcnUpdateSvcList_Data *_svcList,
				int _numOfNewSvc, humax::LcnUpdateSvc_Data *_newSvc,
				int _numOfReplacedSvc, humax::LcnUpdateSvc_Data *_replacedSvc)
	: Event(context,"LcnUpdate")
		, m_eventType(_eventType)
		, m_numOfSvcList(_numOfSvcList)
		, m_numOfNewSvc(_numOfNewSvc)
		, m_numOfReplacedSvc(_numOfReplacedSvc)
{
	ENTRY;

	svcList = NULL;
	newSvc = NULL;
	replacedSvc = NULL;
	m_svcListName = NULL;
	m_lastUpdatedTime = NULL;
	m_lastCheckedTime = NULL;

	if(_eventType == humax::LcnUpdateEvent_Start_OK)
	{
		if(_numOfSvcList > 0)
		{
			svcList = (humax::LcnUpdateSvcList_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvcList_Data) * _numOfSvcList);

			memcpy(svcList, _svcList, (sizeof(humax::LcnUpdateSvcList_Data) * _numOfSvcList));

			// 일단.. test code...
			for (int i =0; i<_numOfSvcList; i++)
			{
				OOIF_LOG_PRINT("\n\n\n[%s,%d] [%d] index(%d), svclistName(%s)\n\n\n",__FUNCTION__,__LINE__,i,
					svcList[i].index, svcList[i].name);
			}
		}
	}
	else if(_eventType == humax::LcnUpdateEvent_Save_OK)
	{
		if(_numOfNewSvc > 0)
		{
			newSvc = (humax::LcnUpdateSvc_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvc_Data) * _numOfNewSvc);

			memcpy(newSvc, _newSvc, (sizeof(humax::LcnUpdateSvc_Data) * _numOfNewSvc));
		}

		if(_numOfReplacedSvc > 0)
		{
			replacedSvc = (humax::LcnUpdateSvc_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvc_Data) * _numOfReplacedSvc);

			memcpy(replacedSvc, _replacedSvc, (sizeof(humax::LcnUpdateSvc_Data) * _numOfReplacedSvc));
		}
	}
}

HLcnUpdateEvent::HLcnUpdateEvent(
	jsplugin_obj *context, humax::LcnUpdateEvent _eventType,
					char* _svcListName, int _tvNum,
					int _radioNum, char* _lastUpdatedTime,
					char* _lastCheckedTime, bool _displayInfo,
					int _coldbootResult)
	: Event(context,"LcnUpdate")
		, m_eventType(_eventType)
		, m_svcListName(HLIB_STD_StrDup(_svcListName))
		, m_tvNum(_tvNum)
		, m_radioNum(_radioNum)
		, m_lastUpdatedTime(HLIB_STD_StrDup(_lastUpdatedTime))
		, m_lastCheckedTime(HLIB_STD_StrDup(_lastCheckedTime))
		, m_displayInfo(_displayInfo)
		, m_coldbootResult(_coldbootResult)
{
	svcList = NULL;
	newSvc = NULL;
	replacedSvc = NULL;
	ENTRY;
}

HLcnUpdateEvent::~HLcnUpdateEvent()
{
	if (svcList)
	{
		// 일단 테스트..확인용.. 나중에 지워죵...
		HLIB_STD_MemFree (svcList);
	}

	if(newSvc)
	{
		HLIB_STD_MemFree (newSvc);
	}

	if(replacedSvc)
	{
		HLIB_STD_MemFree (replacedSvc);
	}

	if(m_svcListName)
	{
		HLIB_STD_MemFree (m_svcListName);
	}

	if(m_lastUpdatedTime)
	{
		HLIB_STD_MemFree (m_lastUpdatedTime);
	}

	if(m_lastCheckedTime)
	{
		HLIB_STD_MemFree (m_lastCheckedTime);
	}
}


int HLcnUpdateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("eventtype", m_eventType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("numofsvclist", m_numOfSvcList, JSP_GET_VALUE_CACHE);
	GET_NUMBER("numofnewsvc", m_numOfNewSvc, JSP_GET_VALUE_CACHE);
	GET_NUMBER("numofreplacedsvc", m_numOfReplacedSvc, JSP_GET_VALUE_CACHE);
	GET_NUMBER("tvNum", m_tvNum, JSP_GET_VALUE_CACHE);
	GET_NUMBER("radioNum", m_radioNum, JSP_GET_VALUE_CACHE);
	GET_NUMBER("coldbootResult", m_coldbootResult, JSP_GET_VALUE_CACHE);

	GET_OBJECT("svclist", obj, new CLcnUpdateSvcList(m_numOfSvcList, svcList), JSP_GET_VALUE);
	GET_OBJECT("newsvc", obj, new CLcnUpdateSvc(m_numOfNewSvc, newSvc), JSP_GET_VALUE);
	GET_OBJECT("replacedsvc", obj, new CLcnUpdateSvc(m_numOfReplacedSvc, replacedSvc), JSP_GET_VALUE);

	GET_STRING("svcListName", m_svcListName, JSP_GET_VALUE_CACHE);
	GET_STRING("lastUpdatedTime", m_lastUpdatedTime, JSP_GET_VALUE_CACHE);
	GET_STRING("lastCheckedTime", m_lastCheckedTime, JSP_GET_VALUE_CACHE);

	GET_BOOLEAN("displayInfo", m_displayInfo, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HLcnUpdateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

#define NUM_OF_LCNUPDATE_EVENTPARAM	7
#define NUM_OF_LCNUPDATE_GETINFO_EVENTPARAM	8

//virtual
int HLcnUpdateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;

	if(m_eventType == humax::LcnUpdateEvent_Get_Info)
	{
		*argv = new jsplugin_value[NUM_OF_LCNUPDATE_GETINFO_EVENTPARAM];
		jsplugin_value *result = *argv;

		result[0].type = JSP_TYPE_NUMBER;
		result[0].u.number = m_eventType;

		result[1].type = JSP_TYPE_STRING;
		result[1].u.string = m_svcListName;

		result[2].type = JSP_TYPE_NUMBER;
		result[2].u.number = m_tvNum;

		result[3].type = JSP_TYPE_NUMBER;
		result[3].u.number = m_radioNum;

		result[4].type = JSP_TYPE_STRING;
		result[4].u.string = m_lastUpdatedTime;

		result[5].type = JSP_TYPE_STRING;
		result[5].u.string = m_lastCheckedTime;

		result[6].type = JSP_TYPE_BOOLEAN;
		result[6].u.boolean = m_displayInfo;

		result[7].type = JSP_TYPE_NUMBER;
		result[7].u.number = m_coldbootResult;

		return NUM_OF_LCNUPDATE_GETINFO_EVENTPARAM;
	}
	else
	{
		*argv = new jsplugin_value[NUM_OF_LCNUPDATE_EVENTPARAM];
		jsplugin_value *result = *argv;

		result[0].type = JSP_TYPE_NUMBER;
		result[0].u.number = m_eventType;

		result[1].type = JSP_TYPE_NUMBER;
		result[1].u.number = m_numOfSvcList;

		SET_SERIALIZED_OBJECT(new CLcnUpdateSvcList(m_numOfSvcList, svcList), 2);

		result[3].type = JSP_TYPE_NUMBER;
		result[3].u.number = m_numOfNewSvc;

		SET_SERIALIZED_OBJECT(new CLcnUpdateSvc(m_numOfNewSvc, newSvc), 4);

		result[5].type = JSP_TYPE_NUMBER;
		result[5].u.number = m_numOfReplacedSvc;

		SET_SERIALIZED_OBJECT(new CLcnUpdateSvc(m_numOfReplacedSvc, replacedSvc), 6);

		return NUM_OF_LCNUPDATE_EVENTPARAM;
	}

}
#endif // OIPF


