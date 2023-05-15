/** **********************************************************************************************************
	@file 		PlayerEvents.cpp

	@date		2013/10/28
	@author		STB-Component
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include "PlayerEvents.h"
#include "StreamTypes.h"
#include "FragmentCollection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Lovefilm;

PlayerStreamStartedDownloadingEvent::~PlayerStreamStartedDownloadingEvent()
{
}

PlayerStreamStartedDownloadingEvent::PlayerStreamStartedDownloadingEvent(jsplugin_obj *context, StreamTypesHandle avstream)
: Event(context, "player_stream_started_downloading"), m_stream(avstream)
{
}

int PlayerStreamStartedDownloadingEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	
	GET_OBJECT("stream", obj, new StreamTypes(m_stream), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PlayerStreamStartedDownloadingEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new StreamTypes(m_stream), 0);

	return 1;
}

PlayerStreamStoppedDownloadingEvent::~PlayerStreamStoppedDownloadingEvent()
{
}

PlayerStreamStoppedDownloadingEvent::PlayerStreamStoppedDownloadingEvent(jsplugin_obj *context, StreamTypesHandle avstream)
: Event(context, "player_stream_stopped_downloading"), m_stream(avstream)
{
}

int PlayerStreamStoppedDownloadingEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	
	GET_OBJECT("stream", obj, new StreamTypes(m_stream), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PlayerStreamStoppedDownloadingEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new StreamTypes(m_stream), 0);

	return 1;
}

PlayerStreamBecameActiveEvent::~PlayerStreamBecameActiveEvent()
{
}

PlayerStreamBecameActiveEvent::PlayerStreamBecameActiveEvent(jsplugin_obj *context, StreamTypesHandle avstream)
: Event(context, "player_stream_became_active"), m_stream(avstream)
{
}

int PlayerStreamBecameActiveEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	
	GET_OBJECT("stream", obj, new StreamTypes(m_stream), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PlayerStreamBecameActiveEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new StreamTypes(m_stream), 0);

	return 1;
}

PlayerBufferStartedEvent::~PlayerBufferStartedEvent()
{
}

PlayerBufferStartedEvent::PlayerBufferStartedEvent(jsplugin_obj *context, char *type, StreamTypesHandle avstream)
: Event(context, "player_buffer_started"), m_bufferingtype(type), m_stream(avstream)
{
}

int PlayerBufferStartedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	
	GET_STRING("type", m_bufferingtype, JSP_GET_VALUE);
	GET_OBJECT("stream", obj, new StreamTypes(m_stream), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PlayerBufferStartedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = m_bufferingtype;

	SET_SERIALIZED_OBJECT(new StreamTypes(m_stream), 1);

	return 2;
}

PlayerBufferCompletedEvent::~PlayerBufferCompletedEvent()
{
}

PlayerBufferCompletedEvent::PlayerBufferCompletedEvent(jsplugin_obj *context, char *type, StreamTypesHandle avstream)
: Event(context, "player_buffer_completed"), m_bufferingtype(type), m_stream(avstream)
{
}

int PlayerBufferCompletedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	
	GET_STRING("type", m_bufferingtype, JSP_GET_VALUE);
	GET_OBJECT("stream", obj, new StreamTypes(m_stream), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int PlayerBufferCompletedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = m_bufferingtype;

	SET_SERIALIZED_OBJECT(new StreamTypes(m_stream), 1);

	return 2;
}

PlayerFragmentsDownloadedEvent::~PlayerFragmentsDownloadedEvent()
{
}

PlayerFragmentsDownloadedEvent::PlayerFragmentsDownloadedEvent(jsplugin_obj *context)
: Event(context, "player_fragments_downloaded")
{
}

int PlayerFragmentsDownloadedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	
	GET_OBJECT("fragments", obj, new FragmentCollection(), JSP_GET_VALUE);	

	return JSP_GET_NOTFOUND;
}

int PlayerFragmentsDownloadedEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new FragmentCollection(), 1);

	return 1;

}

