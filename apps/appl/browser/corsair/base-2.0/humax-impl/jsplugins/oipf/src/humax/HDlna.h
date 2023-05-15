/*************************************************************************************************************
	File 		: HDlna.h
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

#ifndef __HDLNA_H__
#define __HDLNA_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHDlna.h"

namespace Ooif
{

	class HDlna : public ObjectInstance
	{
	private:
		static HDlna *m_pDlnaInstance;

	public:
		HDlna(); 
		virtual ~HDlna();			
		static const ClassType class_name = CLASS_DLNA; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		

	};
	class HDlnaDms : public EventTarget
	{
	private:
		static HDlnaDms 	*m_pHDlnaDmsInstance;

	public:
		HDlnaDms();
		virtual ~HDlnaDms();
		static const ClassType class_name = CLASS_DLNADMS;

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		
		static int	start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		
		
		static void callbackEventHandler( int event);
	};
	
	class HDlnaDmr : public EventTarget
	{
	private:
		static HDlnaDmr 	*m_pHDlnaDmrInstance;
		static int 			m_nDlnaDmrScanState;	
		static list<HDlnaDmr*> instances;
	public:
		HDlnaDmr();
		virtual ~HDlnaDmr();
		static const ClassType class_name = CLASS_DLNADMR; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int	start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		
		static int	setMediaChange(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void callbackEventHandler( unsigned int event, unsigned int result, CdsContentHandle cdsItem );
	};

	class HDlnaDmrResultEvent : public Event
	{
	private :
		void	*m_handleId;
		int 	m_nResult;
		int 	m_nEvent;
	public :
		HDlnaDmrResultEvent( jsplugin_obj *c, const char *eventName, int state, int result, CdsContentHandle handleId = NULL);
		virtual ~HDlnaDmrResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};	

	
	class HDlnaDmp : public EventTarget
	{
	private:
		static HDlnaDmp 	*m_pHDlnaDmpInstance;
		static int 			m_nDlnaDmpScanState;	
		static list<HDlnaDmp*> instances;
		static int			m_nState;
		
	public:
		HDlnaDmp();
		virtual ~HDlnaDmp();
		static const ClassType class_name = CLASS_DLNADMP; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int	startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		
		static int	refreshDms(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	getAvailableCdsList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void callbackEventHandler( int event, int size, CdsDeviceHandle cdsList );


	};
	class HDlnaDmpScanResultEvent : public Event
	{
	private :
		void	*m_handleId;
		int 	m_nState;
		int		m_nHandleCount;
	public :
		HDlnaDmpScanResultEvent( jsplugin_obj *c, const char *eventName, int state, int size, CdsDeviceHandle handleId );
		virtual ~HDlnaDmpScanResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};	

#define CDS_SEARCH (1)
	class HDlnaCds : public EventTarget, public WrapperObject<HDlnaCds> 
	{
	private:
		static list<HDlnaCds*> instances;
		static HDlnaCds *m_pHDlnaCdsInstance;

	public:
		HDlnaCds(CdsDeviceHandle id);
		HDlnaCds(const HDlnaCds &original );
		virtual ~HDlnaCds();

		int	m_nLastReqId;
		static const ClassType class_name = CLASS_DLNACDS; 
		
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( CdsDeviceHandle id);
		
		static int browseContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if CDS_SEARCH
		static int searchContent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

#endif
		//static void callbackEventHandler( int event, int size, CdsContentHandle *cdsItem, char *parentId);
		static void callbackEventHandler( int event, int reqId, int size, CdsContentHandle *contentList, char *parentId, int total );
	};
	
	class HDlnaCdsScanResultEvent : public Event
	{
	private :
		void	**m_handleId;
		int 	m_nState;
		int		m_nHandleCount;
		char	*m_pszParentId;
	public :
		HDlnaCdsScanResultEvent( jsplugin_obj *c, const char *eventName, int state, int size, CdsContentHandle *handleId, char *parentId);
		virtual ~HDlnaCdsScanResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		
		HDlnaCdsContentCollection* GetContentCollection();
		virtual int serialize(jsplugin_value **argv);
	};
	
	class HDlnaCdsSearchResultEvent : public Event
	{
	private :
		void	**m_handleId;
		int 	m_nState;
		int		m_nHandleCount;
		int		m_nTotalCount; // Match Count
		int		m_nRequestId;
	public :
		HDlnaCdsSearchResultEvent( jsplugin_obj *c, const char *eventName, int state, int reqId, int size, CdsContentHandle *handleId, char *parentId, int total );
		virtual ~HDlnaCdsSearchResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		
		HDlnaCdsContentCollection* GetContentCollection();
		virtual int serialize(jsplugin_value **argv);
	};

	class HDlnaCdsContent : public ObjectInstance, public WrapperObject<HDlnaCdsContent> 
	{
	private:
				
	public:
		HDlnaCdsContent(CdsContentHandle id);
		HDlnaCdsContent(const HDlnaCdsContent &original);
		virtual ~HDlnaCdsContent();	

		static const ClassType class_name = CLASS_DLNACDS_CONTENT; 
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( CdsContentHandle id);

	};
} /* namespace Ooif */

#endif // __HDLNA_H__
#endif
