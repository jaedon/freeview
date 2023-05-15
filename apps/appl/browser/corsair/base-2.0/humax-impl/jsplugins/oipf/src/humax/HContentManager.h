/*************************************************************************************************************
	File 		: HContentManager.cpp
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

#ifndef __HMX_CONTENT_MANAGER_H__
#define __HMX_CONTENT_MANAGER_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHContentManager.h"

namespace Ooif
{

	class HContentManager : public EventTarget
	{
	private:
		static HContentManager *m_pHContentManager;
		static void cbBrowseEventHandler( int event, unsigned int requestId, int size, BrowserContentHandle *contentList);
		static void cbSearchEventHandler( int event, unsigned int requestId, int size, BrowserContentHandle *contentList);
		static void cbOperationEventHandler(int ticket, int result);
		static void cbExtMonitorEventHandler(int state, void *pszTargetSource);
		static void cbScanSurveyEventHandler(int state, void *pszTargetSource, int nRequestId);

	public:
		HContentManager(); 
		virtual ~HContentManager();

		static const ClassType class_name = CLASS_CONTENTMANAGER; 
		
		int 		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int 		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int 	reScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int 	startAutoScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	stopAutoScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int 	browseContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	searchContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static int 	autoCopy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	copyContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	copyContentArray(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	deleteContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	moveContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	renameContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	makeDir(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	removeDir(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	getSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	stopFileOperation(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		
		static int 	isExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	hasFile(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	extensionMonitor(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	getOperationState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);	
		static int  isScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int  requestScanNotify(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int  createOperationToken(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
	};

	class HContentManagerOperationToken : public ObjectInstance
	{
	public:
		HContentManagerOperationToken( void * id ); 
		HContentManagerOperationToken( const HContentManagerOperationToken &original);
		virtual ~HContentManagerOperationToken();

		static const ClassType class_name = CLASS_CONTENTMGR_OPERTOKEN; 
		static void cbEventHandler(void *identifier, int event, void * resultInfo);

		int 		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int 		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		void *		getIdentifier() { return m_identifier;} ;
		static int 	addPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int 	removePath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		//static void releaseNative( void * id );
	private :
		void *m_identifier;
		
	};

	class HContentManagerOperateResultEvent : public Event
	{
	private :
		int 	m_nTicket;
		int		m_nResult;
	public :
		HContentManagerOperateResultEvent( jsplugin_obj *c, const char *eventName, int ticket, int result);
		virtual ~HContentManagerOperateResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HContentManagerScanSurveyResultEvent : public Event
	{
	private :
		int 	m_nState;
		char	*m_handleId;
		int 	m_nRequestId;
	public :
		HContentManagerScanSurveyResultEvent( jsplugin_obj *c, const char *eventName, int state, char *pszTargetSource, int requestId);
		virtual ~HContentManagerScanSurveyResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HContentManagerMonitorResultEvent : public Event
	{
	private :
		int 	m_nState;
		char	*m_handleId;
	public :
		HContentManagerMonitorResultEvent( jsplugin_obj *c, const char *eventName, int state, char *pszTargetSource);
		virtual ~HContentManagerMonitorResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
	
	class HContentManagerBrowseResultEvent : public Event
	{
	protected :
		void	**m_handleId;
		int 	m_nState;
		int		m_nHandleCount;
		unsigned int 	m_nTicketId;
	public :
		HContentManagerBrowseResultEvent( jsplugin_obj *c, const char *eventName, int state, unsigned int requestId, int size, BrowserContentHandle *handleId );
		virtual ~HContentManagerBrowseResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		HContentCollection* GetContentCollection();
		virtual int serialize(jsplugin_value **argv);
	};

	class HContentManagerSearchResultEvent : public HContentManagerBrowseResultEvent
	{
	public :
		HContentManagerSearchResultEvent( jsplugin_obj *c, const char *eventName, int state, unsigned int requestId, int size, BrowserContentHandle *handleId );
		//~HContentManagerSearchResultEvent();	
		int serialize(jsplugin_value **argv);
	};

	class HContent : public ObjectInstance, public WrapperObject<HContent> 
	{
	public:
		HContent(BrowserContentHandle id);
		HContent(const HContent &original);
		virtual ~HContent();
	
		static const ClassType class_name = CLASS_CONTENT; 
		int 		getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int 		setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( BrowserContentHandle id);
	};

	class ContentCollection : public Collection<HContent>
	{
		public:
			ContentCollection();
			~ContentCollection();
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static const ClassType class_name = CLASS_CONTENTCOLLECTION;
	};
	
} /* namespace Ooif */

#endif // __HMX_MEDIA_H__
#endif
