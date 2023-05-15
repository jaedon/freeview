/*************************************************************************************************************
	File 		: HSamba.h
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

#ifndef __HSAMBA_H__
#define __HSAMBA_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHSamba.h"

namespace Ooif
{
	class HSambaManager : public EventTarget
	{
	private:
		static HSambaManager *m_pSambaInstance;
		static list<HSambaManager*> instances;

	public:
		HSambaManager(); 
		~HSambaManager();

		static const ClassType class_name = CLASS_SAMBA_MANAGER; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		
		static int startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int unmountAll(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int mount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int unmount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getAvailableList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
		static void callbackEventHandler( int event, SambaServerHandle handle);

	};

	class HSambaServer : public EventTarget
	{
	public:
		HSambaServer( SambaServerHandle handle ); 
		~HSambaServer();

		static const ClassType class_name = CLASS_SAMBA_SERVER; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		StringCollection * getUsers();
		static int start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int isValidPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setShareFolder(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getShareFolder(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPublicAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPublicAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void callbackEventHandler( int event, int result );
	private:
		enum { eSERVER_STOP = 0, eSERVER_START, eCONFIG_CHANGE };
		static list<HSambaServer*> instances;
		static SambaServerHandle		identifier;
	};

	class HSmbsShareFolder : public ObjectInstance, public WrapperObject<HSmbsShareFolder>
	{
	public :
		HSmbsShareFolder(void *id);
		HSmbsShareFolder(const HSmbsShareFolder &original);
		virtual ~HSmbsShareFolder();

		static const ClassType class_name = CLASS_SAMBA_SERVER_SHARE_FOLDER; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( void* id);
	};

	class HSambaClient : public EventTarget
	{
	private:
		static HSambaClient *m_pSambaInstance;
		static list<HSambaClient*> instances;

	public:
		HSambaClient(); 
		~HSambaClient();

		static const ClassType class_name = CLASS_SAMBA_CLIENT; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		
		static int startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int unmountAll(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int mount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int unmount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getAvailableList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
		static void callbackEventHandler( int event, SambaServerHandle handle);

	};

	class HSambaResultEvent : public Event
	{
	private :
		SambaServerHandle		m_handleId;
		int 					m_nState;
		int						m_nHandleCount;
	public :
		HSambaResultEvent( jsplugin_obj *c, const char *eventName, int state, SambaServerHandle handleId = NULL);
		virtual ~HSambaResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};


	class HSambaSearchedServer : public ObjectInstance,  public WrapperObject<HSambaSearchedServer> 
	{
	public:
		HSambaSearchedServer(SambaServerHandle id);
		HSambaSearchedServer(const HSambaSearchedServer &original);
		virtual ~HSambaSearchedServer();
		
		static const ClassType class_name = CLASS_SAMBA_SEARCHED_SERVER; 
		int 		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int 		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( SambaServerHandle id);
	};


} /* namespace Ooif */

#endif // __HMX_MEDIA_H__
#endif
