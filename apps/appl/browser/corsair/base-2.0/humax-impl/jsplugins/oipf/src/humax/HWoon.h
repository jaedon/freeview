/*************************************************************************************************************
	File 		: HWoon.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/


#ifdef OIPF

#ifndef __HWOON_H__
#define __HWOON_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHWoon.h"

namespace Ooif
{

	class HWoon : public EventTarget
	{
	private:
		static HWoon *g_pWoonInstance;
	public:
		HWoon();
		virtual ~HWoon();

		static const ClassType class_name = CLASS_WOON;

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	};

	class HWoonClient : public EventTarget
	{
	private:
		static HWoonClient *g_pWoonClientInstance;
		static list<HWoonClient*> instances;

	public:
		HWoonClient();
		virtual ~HWoonClient();

		static const ClassType class_name = CLASS_WOONCLIENT;

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);


		static int	startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int	addFriend(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	removeFriend(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	getWoonFriendList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void callbackEventHandler(unsigned int event, unsigned int state, WoonFriendHandle stFriendInfo);

	};

	class HWoonClientResultEvent : public Event
	{
	private :
		void	*m_handleId;
		int 	m_nEvent;
		int 	m_nState;
	public :
		HWoonClientResultEvent( jsplugin_obj *c, const char *eventName, int event, int state, WoonFriendHandle handleId );
		virtual ~HWoonClientResultEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HWoonFriend : public EventTarget, public WrapperObject<HWoonFriend>
	{
	public:
		HWoonFriend(WoonFriendHandle id);
		HWoonFriend(const HWoonFriend &original );
		virtual ~HWoonFriend();

		static const ClassType class_name = CLASS_WOONFRIEND;


		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int browseContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void callbackEventHandler( int event, int size, WoonContentHandle stContentList );

		static void releaseNative( WoonFriendHandle id);
	};

	class HWoonContent : public EventTarget, public WrapperObject<HWoonContent>
	{
	public:
		HWoonContent(WoonContentHandle id);
		HWoonContent(const HWoonContent &original );
		virtual ~HWoonContent();


		static const ClassType class_name = CLASS_WOONCONTENT;


		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);


		static void releaseNative( WoonContentHandle id);
	};

	class HWoonServer : public EventTarget
	{
	private:
		static HWoonServer *g_pWoonServerInstance;
		static list<HWoonServer*> instances;

	public:
		HWoonServer();
		virtual ~HWoonServer();

		static const ClassType class_name = CLASS_WOONSERVER;

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

#if 0
		static int 	addEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
		static int	start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	testConnection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void callbackEventHandler(unsigned int event, unsigned int state, void* pstInfo );
	};

	class HWoonServerResultEvent : public Event
	{
	private :
		void	*m_handleId;
		int 	m_nEvent;
		int 	m_nState;
	public :
		HWoonServerResultEvent( jsplugin_obj *c, const char *eventName, int event, int state, WoonServerInfoHandle handleId );
		virtual ~HWoonServerResultEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HWoonServerConnectInfo : public EventTarget, public WrapperObject<HWoonServerConnectInfo>
	{
	public:
		HWoonServerConnectInfo(WoonServerInfoHandle id);
		HWoonServerConnectInfo(const HWoonServerConnectInfo &original);
		virtual ~HWoonServerConnectInfo();

		static const ClassType class_name = CLASS_WOONSERVERINFO;

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static void releaseNative( WoonServerInfoHandle id);
	};


} /* namespace Ooif */

#endif // __HMX_MEDIA_H__
#endif
