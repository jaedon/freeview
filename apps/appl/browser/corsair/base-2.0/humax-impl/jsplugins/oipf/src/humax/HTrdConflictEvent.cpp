// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HTrdConflictEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;


#if 0
HTrdConflictEvent::HTrdConflictEvent(
	jsplugin_obj *context, humax::TrdConflictEventType_e _eventType,
				int _numOfList, humax::TrdConflictList_t *_TrdConflictList)
		: Event(context,"LcnUpdate")
		, m_eventType(_eventType)
		, m_numOfConflictList(_numOfList)
{
	ENTRY;

	trdconflictlist = NULL;


	if(_eventType == humax::humax::eTRDCONFLICT_GET_INFO)
	{
		if(_numOfList > 0)
		{
			trdconflictlist = (humax::TrdConflictList_t*)HLIB_STD_MemAlloc(sizeof(humax::TrdConflictList_t) * _numOfList);

			memcpy(trdconflictlist, _TrdConflictList, (sizeof(humax::TrdConflictList_t) * _numOfList));
		}
	}
}
#else
HTrdConflictEvent::HTrdConflictEvent(
	jsplugin_obj *context, humax::TrdConflictEventType_e _eventType, int _count,
				char *_trdconflictstr)
		: Event(context,"TrdConflict")
		, m_eventType(_eventType)
		, m_count(_count)
		, m_trdconflictstr(HLIB_STD_StrDup(_trdconflictstr))
{
	ENTRY;


}

#endif
HTrdConflictEvent::~HTrdConflictEvent()
{


	if(m_trdconflictstr)
	{
		HLIB_STD_MemFree (m_trdconflictstr);
	}


}

int HTrdConflictEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("eventtype", m_eventType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("count", m_count, JSP_GET_VALUE_CACHE);
	GET_STRING("trdCondlict", m_trdconflictstr, JSP_GET_VALUE_CACHE);


	return JSP_GET_NOTFOUND;
}

int HTrdConflictEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HTrdConflictEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[3];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_eventType;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_count;
	

	result[2].type = JSP_TYPE_STRING;
	result[2].u.string = m_trdconflictstr;

	return 3;
}
#endif // OIPF


