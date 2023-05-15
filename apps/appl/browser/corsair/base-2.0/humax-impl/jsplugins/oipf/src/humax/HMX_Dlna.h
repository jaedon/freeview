// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __HMX_DLNA__
#define __HMX_DLNA__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHmxDLNA.h"
namespace Ooif
{
	class HMX_DMSSearcher : public EventTarget
	{
	private : 
		static void callbackEventHandler( int event, int size, DMSDeviceHandle dmsList );
		static list<HMX_DMSSearcher *> instances;
	public :
		HMX_DMSSearcher();
		virtual ~HMX_DMSSearcher();

		int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );

		static int	search(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	getAvailableDMSList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	getCurrentDMS(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static DMSDeviceHandle GetCurrentDMSDevice() {		return m_sCurrentDMSDevice;		}
	private :
		static HMX_DMSSearcher *m_sDMSSearcher; 
		static DMSDeviceHandle m_sCurrentDMSDevice;
	public :
		static const ClassType class_name = CLASS_DMSSEARCHER; 
	};

	class HMX_DMS : public EventTarget, public WrapperObject<HMX_DMS>
	{
	private :
		static void callbackEventHandler( int event, char *udn, int reqId, int size, DLNAContentHandle *contentList, int totla );
	public :
		HMX_DMS( DMSDeviceHandle id );
		virtual ~HMX_DMS();

		int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );
		static void releaseNative( DMSDeviceHandle id);
		static int	browseDirectChildren(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	private :
		static HMX_DMS *m_sDMSDevice;
	public :
		static const ClassType class_name = CLASS_DMSDEVICE; 
	};

	class HMX_DLNAContent : public ObjectInstance, public WrapperObject<HMX_DLNAContent>
	{
	private :
	public :
		HMX_DLNAContent( DLNAContentHandle id, DLNAContentMetaHandle metaId );
		HMX_DLNAContent(const HMX_DLNAContent &original );
		virtual ~HMX_DLNAContent();
		int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );
		DLNAContentMetaHandle m_metaDataHandle;
		static void releaseNative( DLNAContentHandle id);
	};

	class HMX_DLNAContentMetaData : public ObjectInstance, public WrapperObject<HMX_DLNAContentMetaData>
	{
	public :
		HMX_DLNAContentMetaData( DLNAContentMetaHandle id );
		virtual ~HMX_DLNAContentMetaData();
		int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );
		static void releaseNative( DLNAContentMetaHandle id);
	};

	class HMX_DMSSearcherResultEvent : public Event
	{
	private :
		void	*m_handleId;
		int 	m_nState;
		int		m_nHandleCount;
	public :
		HMX_DMSSearcherResultEvent( jsplugin_obj *c, const char *eventName, int size, DMSDeviceHandle handleId );
		virtual ~HMX_DMSSearcherResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HMX_DLNAContentSearchResultEvent : public Event
	{
	private :
		DLNAContentHandle		*m_handleId;
		int 	m_nState;
		int		m_nHandleCount;
	public :
		HMX_DLNAContentSearchResultEvent( jsplugin_obj *c, int event, int size, DLNAContentHandle *handleId );
		virtual ~HMX_DLNAContentSearchResultEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	private :
//		HMX_DLNAContentCollection *getContentCollection( int count, DLNAContentHandle * hContents, DLNAContentMetaHandle *hMetaHandles );
	};

} /* namespace Ooif */

#endif // __HMX_DLNA__
#endif // OIPF
