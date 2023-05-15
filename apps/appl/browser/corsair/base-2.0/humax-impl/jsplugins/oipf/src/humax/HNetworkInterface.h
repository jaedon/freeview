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
	class HNetworkInterface : public EventTarget, public WrapperObject<HNetworkInterface>
	{
	private:
		static list<HNetworkInterface *> instances;
		static HNetworkInterface * GetInstanceById( HNetworkInterfaceHandle id );

	protected:
		virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
	public:
		HNetworkInterface(HNetworkInterfaceHandle i);
		HNetworkInterface(const HNetworkInterface &original);
		virtual ~HNetworkInterface();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative(HNetworkInterfaceHandle id);
		static int applyConfigure(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stopConnecting(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int checkGatewayAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int checkDnsAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int checkInternetConnection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static const ClassType class_name = CLASS_NETWORKINTERFACE;
		static void callbackEventHandler( int state, HNetworkHandle id, int nDevIndex );
	};
	/* HMX_NetworkManager Class */
	class HMX_NetworkManager : public EventTarget
	{
	public:
		HMX_NetworkManager();
		virtual ~HMX_NetworkManager();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( HNetworkHandle id );
		static int getLinkStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getWifiConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getFTPConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getDMSConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPPPoEConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static const ClassType class_name = CLASS_NETWORKMANAGER;
		static void callbackEventHandler( int state, HNetworkHandle id, int nDevIndex );
	private:
		static list<HMX_NetworkManager *> instances;
	};

	/* HMX_NetworFTPConfigClass */
	class HMX_NetworkFTPConfig: public ObjectInstance
	{
	public:
		HMX_NetworkFTPConfig();
		virtual ~HMX_NetworkFTPConfig();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( void * id );

		static int FTPServerOn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int FTPServerOff(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_NETWORKFTPCONFIG;
	};

	/* HMX_NetworDMSConfigClass */
	class HMX_NetworkDMSConfig: public ObjectInstance
	{
	public:
		HMX_NetworkDMSConfig();
		virtual ~HMX_NetworkDMSConfig();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_NETWORKDMSCONFIG;
	};

	/* HMX_NetworPPPoEConfigClass */
	class HMX_NetworkPPPoEConfig: public EventTarget
	{
	public:
		HMX_NetworkPPPoEConfig(int devIndex);
		virtual ~HMX_NetworkPPPoEConfig();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int getOn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setOn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int connect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int disconnect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_NETWORKPPPOECONFIG;
		static void callbackEventHandler(int event, HNetworkHandle id, int nDevIndex);
	private:
		static list<HMX_NetworkPPPoEConfig *> instances;
		static int	m_nPPPoEDevIndex;
		static char *m_szID;
		static char *m_szPassword;
	};

	/* HMX_NetworkWiFiConfig Class */
	class HMX_NetworkWiFiConfig : public EventTarget
	{
	private:
		int m_bStopScan;
	public:
		HMX_NetworkWiFiConfig( int devIndex );
		virtual ~HMX_NetworkWiFiConfig();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( void * id );
		static int isConnectedAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int apScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stopApScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setKey(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int applyConnectToAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int applyConnectToAPbyWPS(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getWaveStrength(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getConnectedAPInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getUserAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_NETWORKWIFICONFIG;
	private:
		static list<HMX_NetworkWiFiConfig *> instances;
		static HMX_NetworkWiFiConfig *m_sScanningInstance;
		static HMX_NetworkWiFiConfig *m_sConnectInstance;
		static int	m_nWifiDevIndex;
		static char *m_szKey;

		static void callbackEventHandler( int event, HNetworkHandle id, int nDevIndex);
	};

	/* HMX_NetworkAccessPoint Class */
	class HMX_NetworkAccessPoint : public ObjectInstance , public WrapperObject<HMX_NetworkAccessPoint>
	{
	public:
		HMX_NetworkAccessPoint( NetworkApHandle id );
		HMX_NetworkAccessPoint( const HMX_NetworkAccessPoint &original);
		virtual ~HMX_NetworkAccessPoint();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		//static int setKey(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void releaseNative( void * id );
		static const ClassType class_name = CLASS_NETWORKACCESSPOINT;
	};

	class HMX_NetworkLinkStatusEvent : public Event
	{
	public :
		HMX_NetworkLinkStatusEvent( jsplugin_obj *c, const char *eventName, int state );
		~HMX_NetworkLinkStatusEvent();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	private :
		int	m_nState;

	};
	class HMX_NetworkAPScanningResultEvent : public Event
	{

	private :
		HMX_NetworkAccessPoint ap;
		int	m_nState;
	public :
		HMX_NetworkAPScanningResultEvent( jsplugin_obj *c,  NetworkApHandle handleId, int state );
		~HMX_NetworkAPScanningResultEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

} /* namespace Ooif */

#endif // __H_NETWORKINTERFACE
#endif // OIPF
