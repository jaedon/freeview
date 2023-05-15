// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_NETWORKINTERFACE
#define __H_NETWORKINTERFACE

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

namespace Ooif
{
	/* jlabsNetworkManager Class */
	class jlabsNetworkManager : public EventTarget
	{
	public:
		jlabsNetworkManager();
		virtual ~jlabsNetworkManager();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static int getWifiConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getLinkStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static const ClassType class_name = CLASS_NETWORKMANAGER;
		static void callbackEventHandler( int state, HNetworkHandle id, int nDevIndex );
	private:
		static void * m_sNetManagerIdentifier;
		static list<jlabsNetworkManager *> instances;
	};

	/* jlabsNetworkWiFiConfig Class */
	class jlabsNetworkWiFiConfig : public EventTarget
	{
	public:
		jlabsNetworkWiFiConfig( );
		jlabsNetworkWiFiConfig( int devIndex );
		virtual ~jlabsNetworkWiFiConfig();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( void * id );
		static int isConnectedAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int apScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setKey(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int applyConnectToAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getWaveStrength(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getConnectedAPInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_NETWORKWIFICONFIG;
	private:
		static list<jlabsNetworkWiFiConfig *> instances;
		static int	m_nWifiDevIndex;
		static void * m_sApScanningIdentifier;
		static char *m_szKey;

		static void callbackEventHandler( int event, HNetworkHandle id, int nDevIndex);
	};

	/* jlabsNetworkAccessPoint Class */
	class jlabsNetworkAccessPoint : public ObjectInstance , public WrapperObject<jlabsNetworkAccessPoint>
	{
	public:
		jlabsNetworkAccessPoint( NetworkApHandle id );
		jlabsNetworkAccessPoint( const jlabsNetworkAccessPoint &original);
		virtual ~jlabsNetworkAccessPoint();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		//static int setKey(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void releaseNative( void * id );
		static const ClassType class_name = CLASS_NETWORKACCESSPOINT;
	};

	class jlabsNetworkLinkStatusEvent : public Event
	{
	public :
		jlabsNetworkLinkStatusEvent( jsplugin_obj *c, const char *eventName, int state );
		~jlabsNetworkLinkStatusEvent();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	private :
		int	m_nState;

	};

	class jlabsNetworkWiFiResultEvent : public Event
	{

	private :
		jlabsNetworkAccessPoint ap;
		int	m_nState;
	public :
		jlabsNetworkWiFiResultEvent( jsplugin_obj *c,  NetworkApHandle handleId, int state );
		~jlabsNetworkWiFiResultEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

} /* namespace Ooif */

#endif // __H_NETWORKINTERFACE
#endif // OIPF
