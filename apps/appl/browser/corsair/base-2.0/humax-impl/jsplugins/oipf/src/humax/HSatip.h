/*************************************************************************************************************
	File 		: HSatip.h
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

#ifndef __HSATIP_H__
#define __HSATIP_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHSatip.h"

namespace Ooif
{
	class HSatip: public ObjectInstance
	{
		private:
			static HSatip *m_pSatipInstance;

		public:
			HSatip();
			virtual ~HSatip();
			static const ClassType class_name = CLASS_SATIP;

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	};

	class HSatipClient: public EventTarget
	{
		private:
			static HSatipClient *m_pHSatipClientInstance;
			static list<HSatipClient*> instances;

		public:
			HSatipClient();
			virtual ~HSatipClient();
			static const ClassType class_name = CLASS_SATIPCLIENT;

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	};

	class HSatipServer: public EventTarget
	{
		private:
			static HSatipServer *m_pHSatipServerInstance;
			static list<HSatipServer*> instances;

		public:
			HSatipServer();
			virtual ~HSatipServer();
			static const ClassType class_name = CLASS_SATIPSERVER;

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

			static int	start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int	stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		

			static void callbackEventHandler(unsigned int event, char *app, void *data);
	};

	class HSatipServerResultEvent : public Event
	{
		private :
			int 	m_nEvent;
			char 	*m_szApplicationName;
			void	*m_szUrl;

		public :
			HSatipServerResultEvent( jsplugin_obj *c, const char *eventName, int event, void *data = NULL);
			virtual ~HSatipServerResultEvent();	

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			virtual int serialize(jsplugin_value **argv);

			int getEvent(void);
			char *getApplicationName(void);
			char *getUrl(void);
	};
} /* namespace Ooif */

#endif // __HSATIP_H__
#endif
