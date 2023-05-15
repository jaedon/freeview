/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_APPLICATION_MANAGER
#define __NATIVE_APPLICATION_MANAGER

#include "NativeOOIFTypes.h"
#include "NativeChannel.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup Application Application Management
   @{

   This is the native interface for the Applications Management APIs
   as defined in DAE 7.2.
 */

/**
	Application Manager event types.
*/
typedef enum {
	/** This event corresponds to the onLowMemory event defined in DAE 7.2.1 */
	MANAGER_EVENT_LOW_MEMORY = 0
#if defined OIPF || (defined HBBTV && defined KEY_HBBTV_TO_OIPF)
	,
	/** This event corresponds to the onApplicationLoaded event as defined in DAE 7.2.1 */
	MANAGER_EVENT_APPLICATION_LOADED = 1,
	/** This event corresponds to the onApplicationUnloaded event as defined in DAE 7.2.1 */
	MANAGER_EVENT_APPLICATION_UNLOADED = 2
#endif /* OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)*/
	,
	/** This event corresponds to the onApplicationLoadError event as defined in HbbTV Errata #1 6.1.7 / DAE2.1 7.2.1.2 */
	MANAGER_EVENT_APPLICATION_LOAD_ERROR = 3,
#ifdef OIPF
	/** This event corresponds to the onApplicationActivated event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_ACTIVATED = 4,
	/** This event corresponds to the onApplicationDeativated event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_DEACTIVATED = 5,
	/** This event corresponds to the onApplicationShown event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_SHOWN = 6,
	/** This event corresponds to the onApplicationHidden event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_HIDDEN = 7,
	/** This event corresponds to the onApplicationPrimaryReceiver event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER = 8,
	/** This event corresponds to the onApplicationNotPrimaryReceiver event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER = 9,
	/** This event corresponds to the onApplicationTopmost event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_TOPMOST = 10,
	/** This event corresponds to the onApplicationNotTopmost event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_NOT_TOPMOST = 11,
	/** This event corresponds to the onApplicationDestroyRequest event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_DESTROY_REQUEST = 12,
	/** This event corresponds to the onApplicationHibarnateRequest event as defined in DAE 7.2.6 */
	MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST = 13,
	/** This event corresponds to the nagraPRM event  */
	MANAGER_EVENT_APPLICATION_NAGRA_PRM_START = 15,
	/** This event corresponds to the nagraPRM event  */
	MANAGER_EVENT_APPLICATION_NAGRA_PRM_STOP = 16,
#endif
} ApplicationManagerEvent;

#ifdef OIPF
/**
	Application event types.
*/
typedef enum {
	/** This event corresponds to the onApplicationActivated event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_ACTIVATED = 0,
	/** This event corresponds to the onApplicationDeativated event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_DEACTIVATED = 1,
	/** This event corresponds to the onApplicationShown event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_SHOWN = 2,
	/** This event corresponds to the onApplicationHidden event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_HIDDEN = 3,
	/** This event corresponds to the onApplicationPrimaryReceiver event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_PRIMARY_RECEIVER = 4,
	/** This event corresponds to the onApplicationNotPrimaryReceiver event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_NOT_PRIMARY_RECEIVER = 5,
	/** This event corresponds to the onApplicationTopmost event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_TOPMOST = 6,
	/** This event corresponds to the onApplicationNotTopmost event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_NOT_TOPMOST = 7,
	/** This event corresponds to the onApplicationDestroyRequest event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_DESTROY_REQUEST = 8,
	/** This event corresponds to the onApplicationHibarnateRequest event as defined in DAE 7.2.6 */
	APPLICATION_EVENT_HIBERNATE_REQUEST = 9,
} ApplicationEvent;

/**
	Application Manager event types (for widgets).
*/
typedef enum
{
	/** This event corresponds to the onWidgetInstallation event as defined in DAE 7.2.1.4 */
	MANAGER_EVENT_WIDGET_INSTALLATION = 1,
	/** This event corresponds to the onWidgetUninstallation event as defined in DAE 7.2.1.4 */
	MANAGER_EVENT_WIDGET_UNINSTALLATION = 2
} ApplicationManagerWidgetEvent;

/**
    Application Manager event states related to Widgets installation and uninstallation as defined in DAE 7.2.1.1
*/
typedef enum
{
	WIDGET_INSTALLATION_STARTED = 0,
	WIDGET_INSTALLATION_COMPLETED = 1,
	WIDGET_INSTALLATION_FAILED = 2,
	WIDGET_UNINSTALLATION_STARTED = 3,
	WIDGET_UNINSTALLATION_COMPLETED = 4,
	WIDGET_UNINSTALLATION_FAILED = 5
} ApplicationManagerWidgetEventState;

/**
    Application Manager reason of event states related to Widgets installation as defined in DAE 7.2.1.1
*/
typedef enum
{
	WIDGET_NO_ERROR = 0,
	WIDGET_ERROR_STORAGE_AREA_FULL = 10,
	WIDGET_ERROR_DOWNLOAD = 11,
	WIDGET_ERROR_INVALID_ZIP_ARCHIVE = 12,
	WIDGET_ERROR_INVALID_SIGNATURE = 13,
	WIDGET_ERROR_GENERIC = 14,
	WIDGET_ERROR_SIZE_EXCEEDED = 15
} ApplicationManagerWidgetEventStateReason;

#endif /* OIPF */

/**
   Opaque reference to an Application in the native platform.
   @warning 0 value for ApplicationIdentifier is treated by OOIF plugin as invalid.
   Please use mapping algorithm which takes this into account.
 */
typedef void* ApplicationIdentifier;

#ifdef OIPF
/**
	Opaque reference to a Widget in the native platform.
 */
typedef void* WidgetDescriptorHandle;
#endif /* OIPF */

/**
   The callback function that the native platform should call to
   dispatch an Application related event. The OOIF plugin will call
   the registered listeners on all current application instances.
   \param event The event type
   \param appId The instance identifier of the application an event
   occurred on. Currently not used for the onLowMemory event.
   @see native_ApplicationManagerSetListener
 */
typedef void (applicationManagerListener)(ApplicationManagerEvent event, ApplicationIdentifier appId);

#ifdef OIPF
/**
   The callback function that the native platform should call to
   dispatch an Application related event. The OOIF plugin will call
   the registered listeners on all current application instances.
   \param event_name The name of key event
   \param _keyCode The code of keyEvent
   @see applicationManagerOipfKeyEventListener
 */
typedef void (applicationManagerOipfKeyEventListener)(const char* event_name, int _keyCode);
#endif // OIPF

#ifdef OIPF
/**
   The callback function that the native platform should call to
   dispatch an Application related events. The OOIF plugin will call
   the registered listeners on all active applications.
   \param event The event type
   \param appId The instance identifier of the application an event occurred on.
   @see native_ApplicationSetListener
 */
typedef void (applicationListener)(ApplicationEvent event, ApplicationIdentifier appId);

/**
   The callback function that the native platform should call to
   dispatch an Widget related event. The OOIF plugin will call
   the registered listeners on all active applications.
   \param event The event type
   \param handle The WidgetDescriptor for the installed Widget.
   \param state as described in DAE 7.2.1.2.
   \param reason as described in DAE 7.2.1.2. This is only valid if the
   value of the state argument is WIDGET_INSTALLATION_FAILED
   @see native_ApplicationManagerSetWidgetListener
 */
typedef void (applicationManagerWidgetListener)(ApplicationManagerWidgetEvent event, WidgetDescriptorHandle handle, ApplicationManagerWidgetEventState state, ApplicationManagerWidgetEventStateReason reason);
#endif /* OIPF */

/**
   A callback function that the native platform should call to
   inform OOIF that an application has been removed to allow OOIF to
   clean up internal data associated with the application.
   \param appId The instance identifier of the removed application
   @see native_ApplicationManagerSetAppDestroyedListener
 */
typedef void (applicationManagerAppDestroyedListener)(ApplicationIdentifier appId);

/**
   Registers new listener on Application related events to be
   forwarded to the running OIPF/HbbTV application(s).
   \param listener The listener
   \return OOIF_RETURN_OK
   @see applicationManagerListener
 */
OOIFReturnCode native_ApplicationManagerSetListener(applicationManagerListener *listener);

/**
   Registers new listener on Application related events to be
   forwarded to the running OIPF/HbbTV application(s).
   \param listener The listener
   \return OOIF_RETURN_OK
   @see applicationManagerAppDestroyedListener
 */
OOIFReturnCode native_ApplicationManagerSetAppDestroyedListener(applicationManagerAppDestroyedListener *listener);

#ifdef OIPF
/**
   Registers new listener on Application related key events to be
  forwarded from HBBTV to OIPF applications.
   \param listener The listener
   \return OOIF_RETURN_OK
   @see native_ApplicationManagerSetKeyListener
 */

OOIFReturnCode native_ApplicationManagerSetKeyListener(applicationManagerOipfKeyEventListener *listener);
#endif // OIPF

#ifdef OIPF
/**
   Regsiters new listener on Application related events to be forwarded to the OOIF.
   \param listener The listener
   \return OOIF_RETURN_OK
   @see applicationManagerWidgetListener
 */
OOIFReturnCode native_ApplicationSetListener(applicationListener *listener);

/**
   Regsiters new listener on Widget related events to be
   forwarded to the running OOIF application(s).
   \param listener The listener
   \return OOIF_RETURN_OK
   @see applicationManagerWidgetListener
 */
OOIFReturnCode native_ApplicationManagerSetWidgetListener(applicationManagerWidgetListener *listener);

/**
   This function corresponds to the OIPF function
   getApplicationVisualizationMode() as defined in DAE 7.2.1.3.
   \param retval Return value as defined in DAE 7.2.1.3
   \return OOIF return code
 */
OOIFReturnCode native_AppManGetApplicationVisualizationMode(OOIFNumber *retval);

/**
   This function corresponds to the OIPF function GetChildApplications() as
   defined in DAE 7.2.1.3.
   \param parent Application identifier of the application to get the
   children of
   \param count Number of returned children application identifiers
   \param children Use this to return the application identifiers of
   the child applications. The OOIF plugin will take over ownership of the
   array and will later call free() to de-allocate the memory.
   \return OOIF return code
 */
OOIFReturnCode native_AppManGetChildApplications(ApplicationIdentifier parent, int *count, ApplicationIdentifier **children);
#endif /* OIPF */

/**
   Gets the window identifier of the specified application.

   \param appId Identifier of the application to find out the window of
   \param retval The window identifier that hosts the application
   \return OOIF return code
*/
OOIFReturnCode native_AppManGetWindowOfApplication(ApplicationIdentifier appId, long int *retval);

/**
   Gets the application identifier of the application in the specified
   window. If the window is not part of an application the method
   shall return null. This is related to DAE1.1 7.2.1.2 / DAE2.1 7.2.1.3
   \param window_identifier Identifier of the window to find the application in
   \param host The context host URI
   \param retval The application identifier that is part of the window
   \return OOIF return code
*/
OOIFReturnCode native_AppManGetApplicationId(long int window_identifier, const char *host, ApplicationIdentifier *retval);

#ifdef OIPF
/**
	Check if specified application is OIPF/DAE application

   \param appId Identifier of the application
   \param isOipfApp is 1 if specified application is OIPF/DAE one, 0 otherwise
   \return OOIF return code
*/
OOIFReturnCode native_AppManIsOipfApplication(ApplicationIdentifier appId, OOIFBoolean *isOipfApp);
#endif /* OIPF */

#ifdef HBBTV
/**
   This is called when an application calls setChannel, to notify
   the Application manager that it should check the criteria defined in
   described in HBBTV 6.2.2.6, as the application may need to transition from
   broadcast related and broadcast independent. This is called imediately after
   native_BroadcastSetChannel if it was called and it returned OOIF_RETURN_OK.
   This native function allow the integration code of the Application manager
   to be intependent of integration code for VideoBroadcast.
   \param window_identifier The identifier of the window the context
   is running in
   \param host The context host URI
   \param channel The handle of the channel that setChannel was called with an argument of.
   channel=0 is used for 'NULL'.
   \return OOIF_RETURN_OK if context is trusted
*/
OOIFReturnCode native_AppManNotifySetChannelCalled(long int window_identifier, const char *host, ChannelHandle channel);
#endif /* HBBTV */

/*------------------------------------------
  Application
  ------------------------------------------*/
/** @name Application
	The Application class as defined in DAE 7.2.2.
	@{*/


#ifdef OIPF

/**
   This function is called only once at OOIF startup and it's responsible for initializing the native widget API.
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ApplicationManagerInitializeWidgets();

/**
   This function corresponds to the OIPF function installWidget() as
   defined in DAE 7.2.1.3.
   \param uri The resource locator in form of a URI, which points to a Widget
   package to be installed
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AppManInstallWidget(const char* uri);

/**
   This function corresponds to the OIPF function uninstallWidget() as
   defined in DAE 7.2.1.3.
   \param identifier Denotes native handle of the widget to be uninstalled
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AppManUninstallWidget(WidgetDescriptorHandle identifier);

/**
   Humax extension
   \param url URL from which widget was downloaded from
*/
OOIFReturnCode native_AppManGetWidgetInstallProgress(const char *url, OOIFNumber *retval);

/**
   Gets the OIPF property widgets as defined in DAE 7.2.1.2.
   \param count The number of returned handles
   \param handles Return the widget list handles here. The OOIF
   plugin will take over ownership of the array and will later call
   free() to de-allocate the memory
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AppManGetWidgets(int *count, WidgetDescriptorHandle **handles);

/*------------------------------------------
  WidgetDescriptor
  ------------------------------------------*/
/**
   @name WidgetDescriptor
   The WidgetDescriptor class as defined in DAE 7.2.8.1
   @{
	The WidgetDescriptor class is used to implement the characteristics of a DAE Widget.
	It is defined in DAE 7.2.8 and extends the Widget interface from DAE 11.3.
 */

/**
   Gets the OIPF property running as defined in DAE 7.2.8.1.1
   \param identifier WidgetDescriptor instance identifier
   \param retval Property value as defiend in DAE 7.2.8.1.1
   \return OOIF return code
*/
OOIFReturnCode native_WidgetDescriptorGetRunning(WidgetDescriptorHandle identifier, OOIFBoolean* retval);

/**
   Called when the WidgetDescriptor instance is destroyed in the OOIF plugin.
   \param id WidgetDescriptor instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_WidgetDescriptorReleaseHandle(WidgetDescriptorHandle id);


/**
   Widget Interface fields. WidgetDescriptor implements the Widget Interface as descibed in DAE 7.2.8.1
 */

typedef enum {
	WIDGET_LOCAL_URL = 0,
	WIDGET_DOWNLOAD_URL,
	WIDGET_DEFAULT_ICON,
	WIDGET_CUSTOM_ICONS,
	WIDGET_AUTHOR,
	WIDGET_DESCRIPTION,
	WIDGET_NAME,
	WIDGET_SHORT_NAME,
	WIDGET_VERSION,
	WIDGET_ID,
	WIDGET_AUTHOR_EMAIL,
	WIDGET_AUTHOR_HREF,
	WIDGET_WIDTH,
	WIDGET_HEIGHT,
	WIDGET_LAST
} WidgetProperty;

OOIFReturnCode native_WidgetDescriptorGetString(WidgetProperty p, WidgetDescriptorHandle h, const char** retval);
OOIFReturnCode native_WidgetDescriptorGetStringVector(WidgetProperty p, WidgetDescriptorHandle h, int* count, char*** retval);
OOIFReturnCode native_WidgetDescriptorGetNumber(WidgetProperty p, WidgetDescriptorHandle h, OOIFNumber* retval);


/*@}*/
#endif /* OIPF */

/*------------------------------------------
  Application
  ------------------------------------------*/
/** @name Application
	The Application class as defined in DAE 7.2.2.
	@{*/
#ifdef OIPF

/**
	Key event types.
*/
typedef enum {
	/** This event corresponds to the onKeyUp event */
	EVENT_KEY_UP = 0,
	/** This event corresponds to the onKeyDown event */
	EVENT_KEY_DOWN = 1
}KeyEventType;

/**
	Struct used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#KeyEventType.
*/
typedef struct
{
	KeyEventType type;
	OOIFBoolean ctrl_key;
	OOIFBoolean alt_key;
	OOIFBoolean shift_key;
	OOIFNumber keycode;
}KeyEvent;

/**
   The callback function that the native platform should call to
   dispatch an Application related event. The OOIF plugin will call
   the registered listeners on all current application instances.
   \param event The event data
   \param appId The instance identifier of the application an event occurred on.
   @see native_KeySetListener
 */
typedef void (keyListener)(KeyEvent *event, ApplicationIdentifier appId);

/**
   Registers new listener on Application related events to be
   forwarded to the running OIPF application(s).
   \param listener The listener
   \return OOIF_RETURN_OK
   @see keyListener
 */
OOIFReturnCode native_KeySetListener(keyListener *listener);

/**
   This function is called by Application when key event processing is finished.
   \param event The event data
   \param appId The instance identifier of the application.
   @see native_ApplicationKeyEventHandled
 */
OOIFReturnCode native_ApplicationKeyEventHandled(ApplicationIdentifier appId, void *event);


/**
   Gets the OIPF property permissions as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param count Size of the array returned in retval
   \param retval Return value as defiend in DAE 7.2.2.1. The OOIF
   plugin will take over ownership of the array and will later call
   free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetPermissions(ApplicationIdentifier appId, int *count, char ***retval);

/**
   Gets the OIPF property visible as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param retval Return value as defiend in DAE 7.2.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetVisible(ApplicationIdentifier appId, OOIFBoolean *retval);

/**
   Gets the OIPF property active as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param retval Return value as defiend in DAE 7.2.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetActive(ApplicationIdentifier appId, OOIFBoolean *retval);

/**
   Gets the OIPF property isPromaryReceiver as defined in DAE 7.2.2.1.
   \param appId Application identifier
   \param retval Return value as defiend in DAE 7.2.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationGetIsPrimaryReceiver(ApplicationIdentifier appId, OOIFBoolean *retval);
#endif /* OIPF */

/**
   This function corresponds to the OIPF function show() as
   defined in DAE 7.2.2.2.
   \param appId Application identifier
   \return OOIF return code
 */
OOIFReturnCode native_ApplicationShow(ApplicationIdentifier appId);

/**
   This function corresponds to the OIPF function hide() as
   defined in DAE 7.2.2.2.
   \param appId Application identifier
   \return OOIF return code
 */
OOIFReturnCode native_ApplicationHide(ApplicationIdentifier appId);

#ifdef OIPF
/**
   This function corresponds to the OIPF function activateInput() as
   defined in DAE 7.2.2.2.
   \param appId Application identifier
   \param gainFocus Boolean parameter as defined in DAE 7.2.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationActivateInput(ApplicationIdentifier appId, OOIFBoolean gainFocus);

/**
   This function corresponds to the OIPF function deactivateInput() as
   defined in DAE 7.2.2.2.
   \param appId Application identifier
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationDeactivateInput(ApplicationIdentifier appId);
#endif /* OIPF */
/**
   This function corresponds to the OIPF function createApplication() as
   defined in DAE 7.2.2.2.
   \param uri As defined in DAE 7.2.2.2
   \param createChild As defined in DAE 7.2.2.2
   \param parent Instance identifier of the parent/sibling application
   of the created application
   \param retval Return value as defiend in DAE 7.2.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationCreateApplication(const char *uri, OOIFBoolean createChild, ApplicationIdentifier parent, ApplicationIdentifier *retval);

/**
   This function corresponds to the OIPF function destroyApplication() as
   defined in DAE 7.2.2.2.
   \param appId The instance identifier of the application being destroyed
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationDestroyApplication(ApplicationIdentifier appId);

#ifdef OIPF
/**
   This function corresponds to the OIPF function startWidget() as
   defined in DAE 7.2.2.2.
   \param wid a WidgetDescriptor object handle As defined in DAE 7.2.2.2
   \param createChild As defined in DAE 7.2.2.2
   \param parent Instance identifier of the parent/sibling application
   of the created application
   \param retval Return value as defiend in DAE 7.2.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationStartWidget(WidgetDescriptorHandle wid, OOIFBoolean createChild, ApplicationIdentifier parent, ApplicationIdentifier *retval);

/**
   This function corresponds to the OIPF function stopWidget() as
   defined in DAE 7.2.2.2.
   \param wid a WidgetDescriptor object handle As defined in DAE 7.2.2.2
   \param appId Application instance identifier which requested widget to be stopped
   \return OOIF return code
*/
OOIFReturnCode native_ApplicationStopWidget(WidgetDescriptorHandle wid, ApplicationIdentifier appId);
#endif /* OIPF */

#if defined OIPF || (defined HBBTV && defined KEY_HBBTV_TO_OIPF)
/**
   This function corresponds to the OIPF Cross-application event handling as
   defined in DAE 4.4.7
   \param prevAppId application to which the key event was sent previously
   \param appId application which is next in Activation Stack.
    If *prevAppId* is NULL than the first application from the activation stack should be chosen.
   \return OOIF return code
*/
OOIFReturnCode native_AppManGetNextActivatedApplication(ApplicationIdentifier prevAppId, ApplicationIdentifier* appId);


/**
	Propagates the certain event over all NJSE plugins regarding HbbTV and OIPF.
	As a result of this function the synchronously call of onApplicationManagerEvent() is expected.

   \param event to be propagated over all NJSE plugins
   \param appId Identifier of application which caused such event
   \return OOIF return code
*/
OOIFReturnCode native_AppManPropagateEvent(ApplicationManagerEvent event, ApplicationIdentifier appId);
#endif // OIPF || (HBBTV && KEY_HBBTV_TO_OIPF)

/**
	Propagates the key event from HBBTV to OIPF application manager.
	As a result of this function the synchronously call of receiveKeyFromHbbtv() is expected.

   \param event_name to be propagated to OIPF applications
   \param _keyCode to be proipagated to OIPF applications
   \return OOIF return code
*/
#if (defined HBBTV && defined KEY_HBBTV_TO_OIPF) || (defined OIPF && defined KEY_HBBTV_TO_OIPF)
	OOIFReturnCode native_AppManPropagateKeyEvent(const char* event_name, int _keyCode);
#endif //(HBBTV && KEY_HBBTV_TO_OIPF) || (OIPF && KEY_HBBTV_TO_OIPF)



/*@}*/
/*------------------------------------------
  ApplicationPrivateData
  ------------------------------------------*/
/** @name ApplicationPrivateData
	The ApplicationPrivateData class as defined in DAE 7.2.4.
	@{*/
/**
   This function corresponds to the OIPF function getFreeMem()
   as defined in DAE 7.2.4.2.
   \param appId The instance identifier of the application the data
   belongs to
   \param retval Return value as defiend in DAE 7.2.4.2
   \return OOIF return code
 */
OOIFReturnCode native_AppPrivateDataGetFreeMem(ApplicationIdentifier appId, OOIFNumber *retval);

/**
   This function corresponds to the HbbTV function
   getCurrentChannel() as defined in HBBTV A.2.2 / DAE2.1 7.2.4.1. If no channel is
   being presented the return value shall be NULL.
   \param appId The instance identifier of the application the data
   belongs to
   \param retval Return value as defiend in HBBTV A.2.2 / DAE2.1 7.2.4.1
   \return OOIF return code
*/
OOIFReturnCode native_AppPrivateDataGetCurrentChannel(ApplicationIdentifier appId, ChannelHandle *retval);

#ifdef OIPF
/**
   This function corresponds to the OIPF property wakeupApplication
   as defined in DAE 7.2.4.1.
   \param appId The instance identifier of the application the data
   belongs to
   \param retval Return value as defiend in DAE 7.2.4.1
   \return OOIF return code
 */
OOIFReturnCode native_AppPrivateDataGetWakeupApplication(ApplicationIdentifier appId, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF property wakeupOITF
   as defined in DAE 7.2.4.1.
   \param appId The instance identifier of the application the data
   belongs to
   \param retval Return value as defiend in DAE 7.2.4.1
   \return OOIF return code
 */
OOIFReturnCode native_AppPrivateDataGetWakeupOITF(ApplicationIdentifier appId, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function prepareWakeupApplication
   as defined in DAE 7.2.4.2.
   \param appId The instance identifier of the application the data
   belongs to
   \param p_URI As defined in 7.2.4.2
   \param p_token As defined in 7.2.4.2
   \param p_time As defined in 7.2.4.2
   \param retval Return value as defiend in DAE 7.2.4.2
   \return OOIF return code
 */
OOIFReturnCode native_AppPrivateDataPrepareWakeupApplication(ApplicationIdentifier appId, const char *p_URI, const char *p_token, const char *p_time, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function prepareWakeupOITF
   as defined in DAE 7.2.4.2.
   \param appId The instance identifier of the application the data
   belongs to
   \param p_time As defined in 7.2.4.2
   \param retval Return value as defiend in DAE 7.2.4.2
   \return OOIF return code
 */
OOIFReturnCode native_AppPrivateDataPrepareWakeupOITF(ApplicationIdentifier appId, const char *p_time, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function clearWakeupToken
   as defined in DAE 7.2.4.2.
   \param appId The instance identifier of the application the data
   belongs to
   \param retval Return value as defiend in DAE 7.2.4.2
   \return OOIF return code
 */
OOIFReturnCode native_AppPrivateDataClearWakeupToken(ApplicationIdentifier appId, const char **retval);
#endif /* OIPF */

/*@}*/
/*------------------------------------------
  KeySet
  ------------------------------------------*/
/** @name KeySet
	The KeySet class as defined in DAE 7.2.5.
	@{*/
/**
  Constants defined in DAE 7.2.5
*/
typedef enum {
	KEY_RED = 0x1,
	KEY_GREEN = 0x2,
	KEY_YELLOW = 0x4,
	KEY_BLUE = 0x8,
	KEY_NAVIGATION = 0x10,
	KEY_VCR = 0x20,
	KEY_SCROLL = 0x40,
	KEY_INFO = 0x80,
	KEY_NUMERIC = 0x100,
	KEY_ALPHA = 0x200
#if defined(OIPF) || defined(HBBTV_1_2_1)
	, KEY_OTHER = 0x400
#endif /* OIPF || HBBTV_1_2_1 */

} ApplicationKey;
/**
   Gets the OIPF property value as defined in DAE 7.2.5.2.
   \param appId The instance identifier of the application the
   key set belongs to
   \param retval Return value as defiend in DAE 7.2.5.2
   \return OOIF return code
*/
OOIFReturnCode native_KeySetGetValue(ApplicationIdentifier appId, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getMaximumValue()
   as defined in DAE 7.2.5.3.
   \param appId The instance identifier of the application the
	key set belongs to
   \param retval Return value as defiend in DAE 7.2.5.3
   \return OOIF return code
 */
OOIFReturnCode native_KeySetGetMaximumValue(ApplicationIdentifier appId, OOIFNumber *retval);

#ifdef OIPF
/**
   Gets the OIPF property otherKeys[] as defined in DAE 7.2.5.2
   \param appId The instance identifier of the application the
    key set belongs to
   \param count Size of the otherKeys array
   \param retval Array with otherKeys values
   \return OOIF return code
 */
OOIFReturnCode native_KeySetOtherKeys(ApplicationIdentifier appId, int *count, OOIFNumber **retval);

/**
   Gets the OIPF property maximuOtherKeys[] as defined in DAE 7.2.5.2
   \param appId The instance identifier of the application the
    key set belongs to
   \param count Size of the maximumOtherKeys array
   \param retval Array with maximumOtherKeys values
   \return OOIF return code
 */
OOIFReturnCode native_KeySetMaximumOtherKeys(ApplicationIdentifier appId, int *count, OOIFNumber **retval);
#endif /* OIPF */

/**
   This function corresponds to the OIPF function setValue()
   as defined in DAE 7.2.5.3.
   \param appId The instance identifier of the application the
   key set belongs to
   \param value As defiend in DAE 7.2.5.3
   \param otherKeysCount size of the otherKeys array
   \param otherKeys As defined in DAE 7.2.5.3
   \param retval Return value as defiend in DAE 7.2.5.3
   \return OOIF return code
 */
OOIFReturnCode native_KeySetSetValue(ApplicationIdentifier appId, OOIFNumber value, int otherKeysCount, OOIFNumber otherKeys[], OOIFNumber *retval);

#if defined(HMX_WEBUI)
typedef void (applicationManagerHumaxAppListener)(int event, const char *app_name);

OOIFReturnCode native_Debug(const char *log);
/* Create The Portal application like TV Portal. */
OOIFReturnCode native_CreatePortalApplication();
/* Destroy The latest loaded Portal application. */
OOIFReturnCode native_DestroyPortalApplication();
/* The OIPF Application has to be managed to load the correct plugin. */
OOIFReturnCode native_AddOIPFApplication(const char *url, OOIFBoolean *retval);
OOIFReturnCode native_RemoveOIPFApplication(const char *url, OOIFBoolean *retval);
OOIFReturnCode native_RemoveAllOIPFApplications();
OOIFReturnCode native_GetHbbTVApplicationType(const char *type, OOIFNumber *ret);
OOIFReturnCode native_CreateHbbTVApplication(const int application_type );
OOIFReturnCode native_DestroyHbbTVApplication(void);
OOIFReturnCode native_DoTeletext(void);
OOIFReturnCode native_AppManGetHumaxApplications(int *count, char ***app_name);
OOIFReturnCode native_AppManCreateHumaxApplication(const char* app_name, const char *url);
OOIFReturnCode native_AppManDestroyHumaxApplication(const char* app_name);
OOIFReturnCode native_AppManSetHumaxApplicationListener(applicationManagerHumaxAppListener *listener);
OOIFReturnCode native_AppPrivateDataPrepareWakeupOITFWithUnixtime(ApplicationIdentifier appId, OOIFNumber time, OOIFBoolean *retval);
OOIFReturnCode native_AppPrivateDataPrepareWakeupOITFExtended(ApplicationIdentifier appId, OOIFNumber time, OOIFNumber repeat, const char *channelID, OOIFNumber volume, OOIFBoolean *retval);
OOIFReturnCode native_AppPrivateDataGetWakeupTime(ApplicationIdentifier appId, const char **retval);
OOIFReturnCode native_AppPrivateDataGetWakeupChannelID(ApplicationIdentifier appId, const char **retval);
OOIFReturnCode native_AppPrivateDataGetWakeupRepeatDays(ApplicationIdentifier appId, OOIFNumber *retval);
OOIFReturnCode native_AppPrivateDataGetWakeupVolume(ApplicationIdentifier appId, OOIFNumber *retval);
OOIFReturnCode native_AppPrivateDataPrepareWakeupTvtvUpdate(ApplicationIdentifier appId, OOIFNumber time, OOIFBoolean isTvtvStandbyEnable, OOIFBoolean *retval);
OOIFReturnCode native_AppPrivateDataGetTvtvWakeupTime(ApplicationIdentifier appId, const char **retval);
OOIFReturnCode native_AppPrivateDataGetTvtvIsStandbyEnable(ApplicationIdentifier appId, OOIFBoolean *retval);
#endif

#if 1 // OIPF 2.3 added
/**
   This function corresponds to the OIPF function getKeyIcon(Integer code)
   as defined in DAE 7.2.5.3.
   \param appId The instance identifier of the application the keyset belongs to
   \param code As defiend in DAE 7.2.5.3
   \param retval Return value as defiend in DAE 7.2.5.3
   \return OOIF return code
 */
OOIFReturnCode native_KeySetGetKeyIcon(ApplicationIdentifier appId, OOIFNumber code, const char **retval);
#endif

/*@}*/
/*@}*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_APPLICATION_MANAGER */
