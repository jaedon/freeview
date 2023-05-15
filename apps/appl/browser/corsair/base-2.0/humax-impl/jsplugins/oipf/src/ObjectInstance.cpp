// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ObjectInstance.h"
#include "AsyncHandler.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>

#include "jsplugin.h"

#include "macros.h"

using namespace std;
using namespace Ooif;

#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
std::list<void*> ObjectInstance::instances;

#define CASE_OBJTYPE(x) case x: return string(#x).data();
__inline const char* ObjectInstance::__getType__(int _type)
{
    ClassType type = static_cast<ClassType>(_type);

    switch ( type )
    {
        CASE_OBJTYPE( CLASS_INVALID );
        CASE_OBJTYPE( CLASS_DEFAULT );
        CASE_OBJTYPE( CLASS_PROGRAMME );
        CASE_OBJTYPE( CLASS_BOOKMARK );
        CASE_OBJTYPE( CLASS_QUERY );
        CASE_OBJTYPE( CLASS_CHANNEL );
        CASE_OBJTYPE( CLASS_CHANNELLIST );
        CASE_OBJTYPE( CLASS_PARENTALRATINGSCHEME );
        CASE_OBJTYPE( CLASS_DOWNLOAD );
        CASE_OBJTYPE( CLASS_AVCOMPONENT );
        CASE_OBJTYPE( CLASS_PARENTALRATINGCOLLECTION );
#if defined(OIPF) || defined(HBBTV_1_2_1)
        CASE_OBJTYPE( CLASS_METADATASEARCH );
        CASE_OBJTYPE( CLASS_SEARCHRESULT );
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
        CASE_OBJTYPE( CLASS_BOOKMARKCOLLECTION );
#endif // OIPF
        CASE_OBJTYPE( CLASS_APPLICATIONPRIVATEDATA );
        CASE_OBJTYPE( CLASS_EVENT );
        CASE_OBJTYPE( CLASS_KEYSET );
        CASE_OBJTYPE( CLASS_FAVOURITELISTCOLLECTION );
#if defined(HBBTV_PVR) || defined(OIPF)
           //CLASS_PVR );
        CASE_OBJTYPE( CLASS_RECORDING );
        CASE_OBJTYPE( CLASS_SCHEDULEDRECORDING );
#endif // HBBTV_PVR || OIPF
        CASE_OBJTYPE( CLASS_PARENTAL_RATING_SCHEME_COLLECTION );
        CASE_OBJTYPE( CLASS_PARENTAL_RATING );
#ifdef OIPF
        CASE_OBJTYPE( CLASS_SIGNAL_INFO );
        CASE_OBJTYPE( CLASS_TUNER );
#ifdef HMX_WEBUI
        CASE_OBJTYPE( CLASS_CHANNELSCANPARAMS );
        CASE_OBJTYPE( CLASS_CHANNELSCANPARAMSCOLLECTION );
#endif
        CASE_OBJTYPE( CLASS_CHANNELSCANOPTIONS );
        CASE_OBJTYPE( CLASS_CHANNELSCANPARAMETERS );
        CASE_OBJTYPE( CLASS_WIDGETDESCRIPTOR );
        CASE_OBJTYPE( CLASS_WIDGETDESCRIPTORCOLLECTION );
#endif // OIPF
        CASE_OBJTYPE( CLASSTYPE_EVENTTARGET ); // All classes after this point should be subclasses of EventTarget
        CASE_OBJTYPE( CLASS_DRMAGENT );
        CASE_OBJTYPE( CLASS_APPLICATIONMANAGER );
        CASE_OBJTYPE( CLASS_DOWNLOADMANAGER );
        CASE_OBJTYPE( CLASS_RECORDINGSCHEDULER );
        CASE_OBJTYPE( CLASS_APPLICATION );
#if defined(OIPF) || defined(HBBTV_1_2_1)
        CASE_OBJTYPE( CLASS_SEARCHMANAGER );
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
        CASE_OBJTYPE( CLASS_LOCAL_SYSTEM );
        CASE_OBJTYPE( CLASS_CHANNEL_CONFIG );
#endif // OIPF
        CASE_OBJTYPE( CLASSTYPE_VISUAL ); //ALL classes after this point should be subclasses of VisualObject
        CASE_OBJTYPE( CLASS_VIDEO_BROADCAST );
        CASE_OBJTYPE( CLASS_VIDEO_ON_DEMAND );
        CASE_OBJTYPE( CLASS_DISC_INFO );
#if defined(HMX_WEBUI) || defined(JLABS)
        CASE_OBJTYPE( CLASS_HDISC_INFO );
#endif
#ifdef HMX_WEBUI
        CASE_OBJTYPE( CLASS_NETWORKINTERFACE );
        CASE_OBJTYPE( CLASS_NETWORKMANAGER );
        CASE_OBJTYPE( CLASS_NETWORKWIFICONFIG );
        CASE_OBJTYPE( CLASS_NETWORKACCESSPOINT );
        CASE_OBJTYPE( CLASS_NETWORKFTPCONFIG );
        CASE_OBJTYPE( CLASS_NETWORKDMSCONFIG );
        CASE_OBJTYPE( CLASS_NETWORKPPPOECONFIG );
        CASE_OBJTYPE( CLASS_HMX_MEDIASCAN );
        CASE_OBJTYPE( CLASS_HMX_MEDIACONTENT );
        CASE_OBJTYPE( CLASS_HMX_MEDIADOFILE );
        CASE_OBJTYPE( CLASS_HMX_MEDIAFILEUTIL );
        CASE_OBJTYPE( CLASS_HMX_STORAGEUTIL );
        CASE_OBJTYPE( CLASS_HMX_PHYSICALSTORAGE );
        CASE_OBJTYPE( CLASS_HMX_LOGICALSTORAGE );
        CASE_OBJTYPE( CLASS_HMX_OPERATORFEATURE );
        CASE_OBJTYPE( CLASS_HMX_OPERATOR_FREESAT );
        CASE_OBJTYPE( CLASS_HMX_BLUETOOTHMANAGER );
        CASE_OBJTYPE( CLASS_HMX_BLUETOOTHDEVICE );
        CASE_OBJTYPE( CLASS_HMX_BLUETOOTH );
        CASE_OBJTYPE( CLASS_ANTENNAINFOCOLLECTION );
        CASE_OBJTYPE( CLASS_ANTENNAINFO );
        CASE_OBJTYPE( CLASS_SEARCHEDINFO );
        CASE_OBJTYPE( CLASS_LCNUPDATESVCLIST );
        CASE_OBJTYPE( CLASS_LCNUPDATESVC );
        CASE_OBJTYPE( CLASS_DMSSEARCHER );
        CASE_OBJTYPE( CLASS_DMSDEVICE );
       
        CASE_OBJTYPE( CLASS_DLNA );
        CASE_OBJTYPE( CLASS_DLNADMP );
        CASE_OBJTYPE( CLASS_DLNADMS );
        CASE_OBJTYPE( CLASS_DLNADMR );
        CASE_OBJTYPE( CLASS_DLNACDS );
        CASE_OBJTYPE( CLASS_DLNACDS_CONTENT );
        CASE_OBJTYPE( CLASS_CONTENTMANAGER );
        CASE_OBJTYPE( CLASS_CONTENTMGR_OPERTOKEN );
        CASE_OBJTYPE( CLASS_CONTENT );
        CASE_OBJTYPE( CLASS_CONTENTCOLLECTION );
        CASE_OBJTYPE( CLASS_STORAGE_CONTENT );
        CASE_OBJTYPE( CLASS_AIRPLAY );
       
        CASE_OBJTYPE( CLASS_WOON );
        CASE_OBJTYPE( CLASS_WOONCLIENT );
        CASE_OBJTYPE( CLASS_WOONFRIEND );
        CASE_OBJTYPE( CLASS_WOONSERVER );
        CASE_OBJTYPE( CLASS_WOONSERVERINFO );
        CASE_OBJTYPE( CLASS_WOONCONTENT );
       
        CASE_OBJTYPE( CLASS_DIAL );
        CASE_OBJTYPE( CLASS_DIALCLIENT );
        CASE_OBJTYPE( CLASS_DIALSERVER );
       
        CASE_OBJTYPE( CLASS_SATIP );
        CASE_OBJTYPE( CLASS_SATIPCLIENT );
        CASE_OBJTYPE( CLASS_SATIPSERVER );
       
        CASE_OBJTYPE( CLASS_SAMBA_MANAGER );
        CASE_OBJTYPE( CLASS_SAMBA_CLIENT );
        CASE_OBJTYPE( CLASS_SAMBA_SEARCHED_SERVER );
        CASE_OBJTYPE( CLASS_SAMBA_SERVER );
        CASE_OBJTYPE( CLASS_SAMBA_SERVER_SHARE_FOLDER );
        CASE_OBJTYPE( CLASS_FTP_MANAGER );
        CASE_OBJTYPE( CLASS_FTP_SERVER );
        CASE_OBJTYPE( CLASS_FTP_SERVER_SHARE_FOLDER );
        CASE_OBJTYPE( CLASS_NETSERVERCONFIGURATION );
        CASE_OBJTYPE( CLASS_NETSERVER_TESTCONNINFO );
       
        CASE_OBJTYPE( CLASS_SOFTWARE_UPDATE );
        CASE_OBJTYPE( CLASS_SOFTWARE_UPDATEINFO );
        CASE_OBJTYPE( CLASS_HCASUI );
        CASE_OBJTYPE( CLASS_CHANNEL_IMPORT_EXPORT );
        CASE_OBJTYPE( CLASS_COLD_BOOT_DONE );
        CASE_OBJTYPE( CLASS_CAS_ACTION_IR_EMM_FORCED_OTA );
        CASE_OBJTYPE( CLASS_LCN_UPDATE );
        CASE_OBJTYPE( CLASS_CLOCK_RECOVERY );
        
        CASE_OBJTYPE( CLASS_RCT_IMAGE_ICON_INFO );
        CASE_OBJTYPE( CLASS_RCT_DVB_BINARY_LOCATOR );
        CASE_OBJTYPE( CLASS_RCT_INFO );
        CASE_OBJTYPE( CLASS_RCT_INFO_COLLECTION );
        CASE_OBJTYPE( CLASS_TRDCONFLICT );
        CASE_OBJTYPE( CLASS_HMX_ALEXA_HELPER );
#endif
#ifdef JLABS
        CASE_OBJTYPE( CLASS_JLABS_DATA_CONTENT_COMPONENT );
        CASE_OBJTYPE( CLASS_JLABS_EMM_AUTO_DISPLAY_MESSAGE );
        CASE_OBJTYPE( CLASS_JLABS_EMM_MESSAGE );
        CASE_OBJTYPE( CLASS_JLABS_BOARD_MESSAGE );
        CASE_OBJTYPE( CLASS_JLABS_EMM_MESSAGE_COLLECTION );
        CASE_OBJTYPE( CLASS_JLABS_EMM_MESSAGE_MANAGER );
        CASE_OBJTYPE( CLASS_JLABS_CDSRECORDING );   /* jlabsCDSRecording */
        CASE_OBJTYPE( CLASS_JLABS_CDSMETADATA );        /* jlabsCDSMetadata */
        CASE_OBJTYPE( CLASS_JLABS_RECORDDESTINATION );
        CASE_OBJTYPE( CLASS_JLABS_LOCAL_SYSTEM );
        CASE_OBJTYPE( CLASS_JLABS_SCANNING );
        CASE_OBJTYPE( CLASS_JLABS_SCANNING_COLLECTION );
        CASE_OBJTYPE( CLASS_JLABS_PPV_HISTORY );
        CASE_OBJTYPE( CLASS_JLABS_PPV_HISTORY_COLLECTION );
       
        CASE_OBJTYPE( CLASS_JLABS_NETWORK_INTERFACE );
        CASE_OBJTYPE( CLASS_JLABS_NETWORK_INTERFACECOLLECTION );
        CASE_OBJTYPE( CLASS_JLABS_NETWORK_SERVICE );
        CASE_OBJTYPE( CLASS_JLABS_NETWORK_SERVICECOLLECTION );
        CASE_OBJTYPE( CLASS_JLABS_NETWORK_ACCESSPOINT );
        CASE_OBJTYPE( CLASS_JLABS_NETWORK_ACCESSPOINTCOLLECTION );
       
        CASE_OBJTYPE( CLASS_JLABS_DMSDEVICE );
        CASE_OBJTYPE( CLASS_JLABS_CONTENTRESULTS );
        CASE_OBJTYPE( CLASS_JLABS_CONTENTSEARCH );
        CASE_OBJTYPE( CLASS_JLABS_CDSSEARCHER );
        CASE_OBJTYPE( CLASS_JLABS_DMR_CONTROLLER );
           //CLASS_JLABS_DMS );
        CASE_OBJTYPE( CLASS_JLABS_SEARCHRESULT );
       
        CASE_OBJTYPE( CLASS_JLABS_REMOTECONTROL_FUNCTION );
       
        CASE_OBJTYPE( CLASS_JLABS_DTT_CHANNEL );
        CASE_OBJTYPE( CLASS_JLABS_DTT_CHANNEL_LIST );
        CASE_OBJTYPE( CLASS_JLABS_BS_CATV_CHANNEL );
        CASE_OBJTYPE( CLASS_JLABS_BS_CATV_CHANNEL_LIST );
        CASE_OBJTYPE( CLASS_JLABS_CHANNEL_LIST_COLLECTION );
        CASE_OBJTYPE( CLASS_JLABS_DMSCOLLECTION );
       
        CASE_OBJTYPE( CLASS_JLABS_MULTIVIEW_COMPONENT );
        CASE_OBJTYPE( CLASS_JLABS_PBPROGRAMME );
        CASE_OBJTYPE( CLASS_JLABS_PPV_HISTORY_COLLECTION_EVENT_TARGET );
        CASE_OBJTYPE( CLASS_JLABS_SETUPINFO_INSERT_FINISHED_EVENT );
#endif // JLABS
        CASE_OBJTYPE( CLASS_HMX_APPLICATIONMANAGER );
        default:
            OOIF_LOG_INFO("Unknown ObjectInstance (type: %d) \n", type);
            break;
    }
    return "";
}

void ObjectInstance::debugPrintObjectInstanceCount()
{
    map<int, unsigned int> types;
    map<string, unsigned int> events;
    for ( list<void*>::iterator it = instances.begin(); \
            it != instances.end(); ++it )
    {
        ObjectInstance *instance = static_cast<ObjectInstance*>(*it);
        types[instance->getType()]++;

        if ( CLASS_EVENT == instance->getType() )
        {
            Event *evt = static_cast<Event*>(instance);
            events[string(evt->eventName)]++;
        }
    }

    OOIF_LOG_INFO("== List of existing Oipf ObjectInstances WebApplication is having at this moment ==\n");
    for( map<int, unsigned int>::iterator it = types.begin(); \
            it != types.end(); ++it )
    {
        OOIF_LOG_INFO(" ObjectInstance: %40s [ref count : %4d] \n", __getType__(it->first), it->second);
    }
    OOIF_LOG_INFO("==============================================================================\n\n");

    if ( 0 < events.size() )
    {
        OOIF_LOG_INFO("====================== List of detail for CLASS_EVENT ========================\n");
        for ( map<string, unsigned int>::iterator it = events.begin(); \
                it != events.end(); ++it )
        {
            string event_name = string(it->first);
            OOIF_LOG_INFO(" Event : %48s [ref count : %4d] \n", event_name.data(), it->second);
        }
        OOIF_LOG_INFO("==============================================================================\n\n");
    }
}
#endif
ObjectInstance::ObjectInstance() : classType(CLASS_DEFAULT), host(NULL)
{
//	ENTRY;
#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
    instances.push_back(this);
#endif
}

ObjectInstance::~ObjectInstance()
{
//	ENTRY;
	if (host)
	{
		host->plugin_private = NULL;
	}
#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
    instances.remove(this);
#endif
}

void ObjectInstance::addProtege(jsplugin_obj *obj)
{
#ifdef DEBUG
	if (find(proteges.begin(), proteges.end(), obj) != proteges.end())
		assert(false);
#endif
    if ( obj != NULL )
    {
        proteges.push_back(obj);
    }
}

void ObjectInstance::removeProtege(jsplugin_obj *obj)
{
#ifdef DEBUG
	if (find(proteges.begin(), proteges.end(), obj) == proteges.end())
		assert(false);
#endif
    if ( obj != NULL )
    {
        proteges.remove(obj);
    }
}

//static
int ObjectInstance::getter_proxy(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	return instance ? instance->getter(obj, name, result) : JSP_GET_NOTFOUND;
}

//static
int ObjectInstance::setter_proxy(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	return instance ? instance->setter(obj, name, value) : JSP_PUT_NOTFOUND;
}

//static
int ObjectInstance::destructor_proxy(jsplugin_obj *obj)
{
    ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
    if (instance)
    {
    	instance->destructor(obj);
    	delete instance;
        obj->plugin_private = NULL;
    }
    return JSP_DESTROY_OK;
}

//static
void ObjectInstance::gc_proxy(jsplugin_obj *obj)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
		instance->gc(obj);
}

//static
void ObjectInstance::unload_proxy(jsplugin_obj *obj)
{
	ENTRY;
#ifdef OOIF_ASYNC
	AsyncHandler::untrackToken(obj);
#endif // OOIF_ASYNC
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
	{
		delete instance;
		obj->plugin_private = NULL;
	}
}

//static
void ObjectInstance::attr_change_proxy(jsplugin_obj *obj, const char *name, const char *value)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
	{
		instance->attr_change(obj, name, value);
	}
}

//static
void ObjectInstance::param_set_proxy(jsplugin_obj *obj, const char *name, const char *value)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
	{
		instance->param_set(obj, name, value);
	}
}

int ObjectInstance::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int ObjectInstance::destructor(jsplugin_obj *obj)
{
//	OOIF_LOG_DEBUG("I have been destroyed: %p", this);
	return JSP_DESTROY_OK;
}

void ObjectInstance::gc(jsplugin_obj *obj)
{
//	OOIF_LOG_DEBUG("GC TRACE for me %p", this);
	for (list<jsplugin_obj*>::iterator it=proteges.begin(); it!=proteges.end(); it++)
	{
		jsplugin_obj *protected_obj = *it;
//		OOIF_LOG_DEBUG("  Protecting me %p", protected_obj);
		g_opera_callbacks->gcmark(protected_obj);
	}
}

void ObjectInstance::attr_change(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
}

void ObjectInstance::param_set(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
}

void ObjectInstance::setAttrs(int attrs_count, jsplugin_attr *attrs)
{
	ENTRY;
	for (int i = 0; i < attrs_count; ++i)
		attr_change(getHost(), attrs[i].name, attrs[i].value);
}
