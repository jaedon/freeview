// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeApplicationManager.h"
#include "NativeChannel.h"
#include "macros.h"
#include "ApplicationManager.h"

#include "oplappmgr.h"

#if defined(HMX_WEBUI)
#include "hapi.h"
#include "hlib.h"
#endif

#include "oplscheduler.h"

using namespace Ooif;

OOIFReturnCode native_ApplicationManagerSetListener(applicationManagerListener *listener)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_ApplicationManagerSetAppDestroyedListener(applicationManagerAppDestroyedListener *listener)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_ApplicationManagerSetKeyListener(applicationManagerOipfKeyEventListener *listener)
{
  OOIF_LOG_DEBUG("not implemented");
  return OOIF_RETURN_OK;
}

OOIFReturnCode native_ApplicationSetListener(applicationListener *listener)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationManagerSetWidgetListener(applicationManagerWidgetListener *listener)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

/**
   This function corresponds to the OIPF function
   getApplicationVisualizationMode() as defined in DAE 7.2.1.2.
   \param retval Return value as defined in DAE 7.2.1.2
   \return OOIF return code
 */
OOIFReturnCode native_AppManGetApplicationVisualizationMode(OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_AppManGetChildApplications(ApplicationIdentifier parent, int *count, ApplicationIdentifier **children)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_AppManInstallWidget(const char* uri)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_AppManUninstallWidget(WidgetDescriptorHandle identifier)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_AppManGetWidgets(int *count, WidgetDescriptorHandle **handles)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WidgetDescriptorGetLocalUri(WidgetDescriptorHandle identifier, const char** retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WidgetDescriptorGetDefaultIcon(WidgetDescriptorHandle identifier, int* count, char*** retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}


OOIFReturnCode native_WidgetDescriptorGetCustomIcons(WidgetDescriptorHandle identifier, int* count, char*** retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WidgetDescriptorGetRunning(WidgetDescriptorHandle identifier, OOIFBoolean* retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WidgetDescriptorReleaseHandle(WidgetDescriptorHandle id)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}
#endif /* OIPF */

#if defined OIPF || (defined HBBTV && defined KEY_HBBTV_TO_OIPF)
/**
   This function corresponds indirectly to the OIPF functionality of activated applications
   defined in DAE 4.3.8.
   \param activatedApp Application identifier of the application to get the
   next activated application from activation stack.
 * When this param is 0 it means that we want the first activated application from the stack.
   \param nextActivatedApp application identifier of the next activated application from activated stack
   \return OOIF return code
 */
OOIFReturnCode native_AppManGetNextActivatedApplication(ApplicationIdentifier activatedApp, ApplicationIdentifier *nextActivatedApp)
{
  OOIF_LOG_DEBUG("not implemented");
  return OOIF_RETURN_NOT_SUPPORTED;
}

/**
	Propagates the certain event over all NJSE plugins regarding HbbTV and OIPF.
	As a result of this function the synchronously call of onApplicationManagerEvent() is expected.

   \param event to be propagated over all NJSE plugins
   \param appId Identifier of application which caused such event
   \return OOIF return code
*/
OOIFReturnCode native_AppManPropagateEvent(ApplicationManagerEvent event, ApplicationIdentifier appId)
{
	// trigger ApplicationLoaded event (add event to queue and dispatch immediately)
	ApplicationManager::onApplicationManagerEvent(MANAGER_EVENT_APPLICATION_LOADED, appId);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppManPropagateKeyEvent(const char* event_name, int _keyCode)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_OK;
}
#endif /* OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)*/

/**
   Gets the window identifier of the specified application.

   \param appId Identifier of the application to find out the window of
   \param retval The window identifier that hosts the application
   \return OOIF return code
*/
OOIFReturnCode native_AppManGetWindowOfApplication(ApplicationIdentifier appId, long int* retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

/**
   Gets the application identifier of the application in the specified
   window. If the window is not part of an application the method
   shall return null.
   \param window_identifier Identifier of the window to find the application in
   \param retval The application identifier that is part of the window
   \return OOIF return code
*/
OOIFReturnCode native_AppManGetApplicationId(long int window_identifier, const char* host, ApplicationIdentifier *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

#ifdef OIPF
/**
	Check if specified application is OIPF/DAE application

   \param appId Identifier of the application
   \param isOipfApp is 1 if specified application is OIPF/DAE one, 0 otherwise
   \return OOIF return code
*/
OOIFReturnCode native_AppManIsOipfApplication(ApplicationIdentifier appId, OOIFBoolean *isOipfApp)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}
#endif /* OIPF */

OOIFReturnCode native_AppManNotifySetChannelCalled(long int window_indentifier, const char * host, ChannelHandle channel)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

#ifdef HBBTV
OOIFReturnCode native_AppManNotifySetChannelChanged(long int window_identifier, const char * host, ChannelHandle channel)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}
#endif /* HBBTV */

/*------------------------------------------
  Application
  ------------------------------------------*/
#ifdef OIPF

/**
   Gets the OIPF property permissions as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param retval Return value as defiend in DAE 7.2.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetPermissions(ApplicationIdentifier appId, int *count, char ***retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

/**
   Gets the OIPF property visible as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param retval Return value as defiend in DAE 7.2.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetVisible(ApplicationIdentifier appId, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

/**
   Registers new listener on Application related events to be
   forwarded to the running OIPF application(s).
   \param listener The listener
   \return OOIF_RETURN_OK
   @see keyListener
 */
OOIFReturnCode native_KeySetListener(keyListener *listener)
{
  OOIF_LOG_DEBUG("not implemented");
  return OOIF_RETURN_OK;
}

/**
   This function is called by Application when key event processing is finished.
   \param event The event info
   \param appId The instance identifier of the application.
   @see native_ApplicationKeyEventHandled
 */
OOIFReturnCode native_ApplicationKeyEventHandled(ApplicationIdentifier appId, void *event)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_OK;
}

/**
   Gets the OIPF property active as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param retval Return value as defiend in DAE 7.2.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetActive(ApplicationIdentifier appId, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationGetIsPrimaryReceiver(ApplicationIdentifier appId, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationStartWidget(WidgetDescriptorHandle wid, OOIFBoolean createChild, ApplicationIdentifier parent, ApplicationIdentifier *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationStopWidget(WidgetDescriptorHandle wid, ApplicationIdentifier appId)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}
#endif /* OIPF */

OOIFReturnCode native_ApplicationShow(ApplicationIdentifier appId)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationHide(ApplicationIdentifier appId)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

#ifdef OIPF
OOIFReturnCode native_ApplicationActivateInput(ApplicationIdentifier appId, OOIFBoolean gainInput)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationDeactivateInput(ApplicationIdentifier appId)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}
#endif /* OIPF */

OOIFReturnCode native_ApplicationCreateApplication(const char *uri, OOIFBoolean createChild, ApplicationIdentifier parent, ApplicationIdentifier *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ApplicationDestroyApplication(ApplicationIdentifier appId)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

/*------------------------------------------
  ApplicationPrivateData
  ------------------------------------------*/
OOIFReturnCode native_AppPrivateDataGetFreeMem(ApplicationIdentifier appId, OOIFNumber *retval)
{
	*retval = 1000;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetCurrentChannel(ApplicationIdentifier appId, ChannelHandle *retval)
{
	*retval = (void*)1;
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_AppPrivateDataGetWakeupApplication(ApplicationIdentifier appId, OOIFBoolean *retval)
{
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetWakeupOITF(ApplicationIdentifier appId, OOIFBoolean *retval)
{
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataPrepareWakeupApplication(ApplicationIdentifier appId, const char *p_URI, const char *p_token, const char *p_time, OOIFBoolean *retval)
{
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataPrepareWakeupOITF(ApplicationIdentifier appId, const char *p_time, OOIFBoolean *retval)
{
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataClearWakeupToken(ApplicationIdentifier appId, const char **retval)
{
	*retval = "TEST TOKEN SET";
	return OOIF_RETURN_OK;
}
#endif /* OIPF */

/*------------------------------------------
  KeySet
  ------------------------------------------*/
OOIFReturnCode native_KeySetGetValue(ApplicationIdentifier appId, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_KeySetGetMaximumValue(ApplicationIdentifier appId, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

#ifdef OIPF
OOIFReturnCode native_KeySetOtherKeys(ApplicationIdentifier appId, int *count, OOIFNumber **retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_KeySetMaximumOtherKeys(ApplicationIdentifier appId, int *count, OOIFNumber **retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

#endif // OIPF

OOIFReturnCode native_KeySetSetValue(ApplicationIdentifier appId, OOIFNumber value, int otherKeysCount, OOIFNumber otherKeys[], OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("not implemented");
	return OOIF_RETURN_NOT_SUPPORTED;
}

#if defined(HMX_WEBUI)
OOIFReturnCode native_AppPrivateDataPrepareWakeupOITFExtended(ApplicationIdentifier appId, OOIFNumber time, OOIFNumber repeat, const char *channelID, OOIFNumber volume, OOIFBoolean *retval)
{
	if (!time)	// cancel
	{
		*retval = OPL_Scheduler_CancelWakeup(OPL_Scheduler_GetInstance()) ? TRUE : FALSE;
		return OOIF_RETURN_OK;
	}

	*retval = FALSE;
	if (OPL_Scheduler_Wakeup(OPL_Scheduler_GetInstance(), (unsigned long)time, (int)repeat, channelID, (int)volume))
	{
		*retval = TRUE;
	}
	(void)appId;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetWakeupTime(ApplicationIdentifier appId, const char **retval)
{
	static char 	timestr[64];
	unsigned long	unixtime;

	if (!OPL_Scheduler_GetWakeupParam(OPL_Scheduler_GetInstance(), &unixtime, NULL, NULL, NULL))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (char *)HLIB_DATETIME_UnixTimeToStringWithOutOffset((UNIXTIME)unixtime, timestr, 64);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetWakeupChannelID(ApplicationIdentifier appId, const char **retval)
{
	if (!OPL_Scheduler_GetWakeupParam(OPL_Scheduler_GetInstance(), NULL, NULL, retval, NULL)
		|| *retval == NULL)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetWakeupRepeatDays(ApplicationIdentifier appId, OOIFNumber *retval)
{
	int	repeatDays;

	if (!OPL_Scheduler_GetWakeupParam(OPL_Scheduler_GetInstance(), NULL, &repeatDays, NULL, NULL))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	*retval = (OOIFNumber)repeatDays;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetWakeupVolume(ApplicationIdentifier appId, OOIFNumber *retval)
{
	int	volume;

	if (!OPL_Scheduler_GetWakeupParam(OPL_Scheduler_GetInstance(), NULL, NULL, NULL, &volume))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	*retval = (OOIFNumber)volume;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataPrepareWakeupTvtvUpdate(ApplicationIdentifier appId, OOIFNumber time, OOIFBoolean isTvtvStandbyEnable, OOIFBoolean *retval)
{
	if (!OPL_Scheduler_SetTVTVEPGDownload_StandbyEnable(isTvtvStandbyEnable))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	if (!time || !isTvtvStandbyEnable)	// cancel
	{
		*retval = OPL_Scheduler_CancelTVTVEPG_Download(OPL_Scheduler_GetInstance()) ? TRUE : FALSE;
		return OOIF_RETURN_OK;
	}
	else
	{
		*retval = FALSE;
		if (OPL_Scheduler_TVTVEPG_Download(OPL_Scheduler_GetInstance(), (unsigned long)time, (HBOOL)isTvtvStandbyEnable))
		{
			*retval = TRUE;
		}
		(void)appId;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetTvtvWakeupTime(ApplicationIdentifier appId, const char **retval)
{
	static char 	timestr[64];
	unsigned long	unixtime;

	if (!OPL_Scheduler_GetTVTVEPG_DownloadParam(OPL_Scheduler_GetInstance(), &unixtime, NULL))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (char *)HLIB_DATETIME_UnixTimeToStringWithOutOffset((UNIXTIME)unixtime, timestr, 64);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppPrivateDataGetTvtvIsStandbyEnable(ApplicationIdentifier appId, OOIFBoolean *retval)
{
	HBOOL	isTvtvStandbyEnable;

	if (!OPL_Scheduler_GetTVTVEPG_DownloadParam(OPL_Scheduler_GetInstance(), NULL, &isTvtvStandbyEnable))
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	*retval = (OOIFBoolean)isTvtvStandbyEnable;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_Debug(const char *log)
{
	Hx_Print("\033[;44mWEBAPP\033[;49m| %s\n", log);
	
	//OplApplicationManager::debug(NULL, log);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_CreatePortalApplication()
{
	HAPI_BroadcastSignal("signal:onStartPortal", 0, "i", 0);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DestroyPortalApplication()
{
	HAPI_BroadcastSignal("signal:onStopPortal", 0, "i", 0);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AddOIPFApplication(const char *url, OOIFBoolean *retval)
{
	*retval = ( OplOIPFApplicationManager::addTrustedApplication(url) == true ) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RemoveOIPFApplication(const char *url, OOIFBoolean *retval)
{
	*retval = ( OplOIPFApplicationManager::removeTrustedApplication(url) == true ) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_RemoveAllOIPFApplications()
{
	OplOIPFApplicationManager::removeAllApplications();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_GetHbbTVApplicationType(const char *type, OOIFNumber *ret_type)
{
	OOIFReturnCode ret = OOIF_RETURN_OK;
	if( strcmp( type, "HBBTV_APP_TYPE_AUTOSTART" ) == 0 )
	{
		*ret_type = (OOIFNumber)OPLHBBTVAPPLICATIONTYPE_AUTOSTART;
	}
	else if( strcmp( type, "HBBTV_APP_TYPE_INDEPENDENT" ) == 0 )
	{
		*ret_type = (OOIFNumber)OPLHBBTVAPPLICATIONTYPE_INDEPENDENT;
	}
	else
	{
		*ret_type = (OOIFNumber)OPLHBBTVAPPLICATIONTYPE_UNDEFINED;
		ret = OOIF_RETURN_ERROR;
	}
	return ret;
}

OOIFReturnCode native_CreateHbbTVApplication(const int application_type )
{
	OplHbbTVApplicationManager::createHbbTVApplication( (eOplHbbTVApplicationType) application_type );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DestroyHbbTVApplication()
{
	OplHbbTVApplicationManager::destroyHbbTVApplication();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DoTeletext()
{
	OplOIPFApplicationManager::doTeletext();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppManGetHumaxApplications(int *count, char ***app_name)
{
	OplOIPFApplicationManager::getHumaxApplications(count, app_name);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppManSetHumaxApplicationListener(applicationManagerHumaxAppListener *listener)
{
	OplOIPFApplicationManager::setHumaxApplicationListener(listener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppManCreateHumaxApplication(const char* name, const char* url)
{
	OplOIPFApplicationManager::createHumaxApplication(name, url);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AppManDestroyHumaxApplication(const char* name)
{
	OplOIPFApplicationManager::destroyHumaxApplication(name);
	return OOIF_RETURN_OK;
}
#endif

#if 1 // OIPF 2.3 added
OOIFReturnCode native_KeySetGetKeyIcon(ApplicationIdentifier appId, OOIFNumber code, const char **retval)
{
	*retval = "";
	return OOIF_RETURN_OK;
}
#endif

