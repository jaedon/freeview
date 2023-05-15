/** **********************************************************************************************************
	@file 		PlayerEvents.h

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
#ifndef _PLAYER_EVENTS_H_
#define _PLAYER_EVENTS_H_
	
#include "Event.h"
#include "NativeStreamTypes.h"

namespace Lovefilm
{
	class PlayerStreamStartedDownloadingEvent : public Event
	{
	private:
		StreamTypesHandle m_stream;
	
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	
	public:
		PlayerStreamStartedDownloadingEvent(jsplugin_obj *context, StreamTypesHandle avstream);
		virtual ~PlayerStreamStartedDownloadingEvent();
		virtual int serialize(jsplugin_value **argv);
	};

	class PlayerStreamStoppedDownloadingEvent : public Event
	{
	private:
		StreamTypesHandle m_stream;
	
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	
	public:
		PlayerStreamStoppedDownloadingEvent(jsplugin_obj *context, StreamTypesHandle avstream);
		virtual ~PlayerStreamStoppedDownloadingEvent();
		virtual int serialize(jsplugin_value **argv);
	};

	class PlayerStreamBecameActiveEvent : public Event
	{
	private:
		StreamTypesHandle m_stream;

	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		PlayerStreamBecameActiveEvent(jsplugin_obj *context, StreamTypesHandle avstream);
		virtual ~PlayerStreamBecameActiveEvent();
		virtual int serialize(jsplugin_value **argv);
	};

	class PlayerBufferStartedEvent : public Event
	{
	private:
		char * m_bufferingtype;
		StreamTypesHandle m_stream;

	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		PlayerBufferStartedEvent(jsplugin_obj *context, char* type, StreamTypesHandle avstream);
		virtual ~PlayerBufferStartedEvent();
		virtual int serialize(jsplugin_value **argv);
	};

	class PlayerBufferCompletedEvent : public Event
	{
	private:
		char * m_bufferingtype;
		StreamTypesHandle m_stream;

	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		PlayerBufferCompletedEvent(jsplugin_obj *context, char* type, StreamTypesHandle avstream);
		virtual ~PlayerBufferCompletedEvent();
		virtual int serialize(jsplugin_value **argv);
	};

	class PlayerFragmentsDownloadedEvent : public Event
	{
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

	public:
		PlayerFragmentsDownloadedEvent(jsplugin_obj *context);
		virtual ~PlayerFragmentsDownloadedEvent();
		virtual int serialize(jsplugin_value **argv);
	};
	
}

#endif //_PLAYER_EVENTS_H_

