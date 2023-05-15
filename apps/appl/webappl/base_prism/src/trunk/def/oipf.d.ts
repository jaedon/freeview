/**
 * Created by bglee@humaxdigital.com on 2014. 3. 15..
 */
interface OipfObjectFactory {
    // 7.1.1 Methods

    /*
     * This method SHALL return trueif and only if an object of the specified type is supported
     * by the OITF. The method SHALL return falseif the MIME type passed as a parameter is
     * not supported by the client.
     */
    isObjectSupported: (mimeType: string) => boolean;

    // 7.1.1.1 Visual objects
    createVideoBroadcastObject: () => VideoBroadcast;
    createVideoMpegObject: () => HTMLObjectElement;
    createStatusViewObject: () => HTMLObjectElement;

    // 7.1.1.2 Non-Visual objects
    createApplicationManagerObject: () => ApplicationManager;
    /*
     createCapabilitiesObject: () => any;
     createChannelConfig: () => any;
     createCodManagerObject: () => any;
     createConfigurationObject: () => any;
     createDownloadManagerObject: () => any;
     createDownloadTriggerObject: () => any;
     createDrmAgentObject: () => any;
     createGatewayInfoObject: () => any;
     createIMSObject: () => any;
     createMDTFObject: () => any;
     createNotifSocketObject: () => any;
     */
    createParentalControlManagerObject: () => ParentalControlManager;
    createRecordingSchedulerObject: () => any;
    /*
     createRemoteControlFunctionObject: () => any;
     createRemoteManagementObject: () => any;
     */
    createSearchManagerObject: () => any;

    /*
     createHumaxConfigurationObject: function () {
     // TODO: object instance management requirend
     return new Configuration();
     },
     createMediaSearchManagerObject: function () {
     gOipfEmul._mediaObject = new HMX_Media();
     return gOipfEmul._mediaObject;
     },
     createFileSystemManagerObject: function () {
     gOipfEmul._fileSystemObject = new HMX_FileSystem();
     return gOipfEmul._fileSystemObject;
     },
     createHumaxStorageUtilObject: function () {
     gOipfEmul._storageUtil = new HMXStorageUtil();
     return gOipfEmul._storageUtil;
     },
     createHumaxCasUtil: function () {
     gOipfEmul._casUtil = new HMXCasUtil();
     return gOipfEmul._casUtil;
     },
     createHumaxOperatorFeatureObject: function () {
     gOipfEmul._operatorFeature = new HMXOperatorFeature();
     return gOipfEmul._operatorFeature;
     },
     createHumaxOperatorFreesatObject: function () {
     gOipfEmul._operatorFreesat = new HMXOperatorFreesat();
     return gOipfEmul._operatorFreesat;
     }
     */

}

interface DOM3EventSource {
    addEventListener(eventName: string, listener: Function, useCapture?: boolean): void;
    removeEventListener(eventName: string, listener: Function): void;
}

interface Collection {

    /*
     The number of items in the collection
     */
    length: number;

    /*
     Return the item at position index in the collection, or undefinedif no item is present at that
     position.
     */
    item(index: number): any;
}

interface ApplicationManager extends DOM3EventSource {
    /*
     The function that is called when the OITF is running low on available memory for running DAE
     applications. The exact criteria determining when to generate such an event is implementation specific.
     */
    onLowMemory: { (): void; };

    /*
     The function that is called immediately prior to a loadevent being generated in the affected
     application. The specified function is called with one argument appl, which provides a reference to the
     affected application.
     */
    onApplicationLoaded: { (appl: Application): void; };

    /*
     The function that is called immediately prior to an unloadevent being generated in the affected
     application. The specified function is called with one argument appl, which provides a reference to the
     affected application.
     */
    onApplicationUnloaded: { (appl: Application): void; };

    /*
     The function that is called when the OITF fails to load either the file containing the initial HTML
     document of an application or an XML AIT file (e.g. due to an HTTP 404 error, an HTTP timeout, being
     unable to load the file from a DSM-CC object carousel or due to the file not being either an HTML file or
     a XML AIT file as appropriate), All properties of the Applicationobject referred to by applSHALL
     have the value undefinedand calling any methods on that object SHALL fail.
     */
    onApplicationLoadError: { (appl: Application): void; };

    /*
     The callback function that is called during the installation process of a Widget.
     */
    onWidgetInstallation: { (wd: WidgetDescriptor, state: number, reason: number): void; };

    /*
     The function that is called during the uninstallation process of a Widget.
     */
    onWidgetUninstallation: { (wd: WidgetDescriptor, state: number): void; };

    /*
     A collection of WidgetDescriptorobjects for the Widgets currently installed on the OITF.
     */
    wigets: WidgetDescriptorCollection;

    /*
     Returns the current mode used by the OITF to visualize applications, whereby a return value:
     */
    getApplicationVisualizationMode: () => number;

    /*
     Get the application that the specified document is part of. If the document is not part of
     an application, or the calling application does not have permission to access that
     application, this method will return null.
     */
    getOwnerApplication: (document: Document) => Application;

    /*
     Get the applications that are children of the specified application.
     */
    getChildApplications: (application: Application) => ApplicationCollection;

    /*
     Provide a hint to the execution environment that a garbag
     initiated. The OITF is not required to act upon this hint.
     */
    gc: () => void;

    /*
     Attempts to install on the OITF a Widget located at the URI passed. If the Widget is
     stored on a remote server it SHALL first be downloaded. This specification does not
     specify where the OITF stores the Widgetpackage, nor does it define what happens
     to the original package after the installation process has finished (regardless of
     whether it succeeded or failed).
     */
    installWidget: (uri: String) => void;

    /*
     Uninstalls a Widget. If this Widget is running it will be stopped. Any storage areas
     associated with the uninstalled Widget SHALL be deleted.
     */
    uninstallWidget: (wd: WidgetDescriptor) => void;
}

interface WidgetDescriptor {
    localURI: string;
    downloadURI: string;
    running: boolean;
    author: string;
    description: string;
    name: string;
    shortName: string;
    version: string;
    id: string;
    authorEmail: string;
    authorHref: string;
    width: number;
    height: number;
}

interface WidgetDescriptorCollection extends Collection {
    [index: number]: WidgetDescriptor;
    item: (index: number) => WidgetDescriptor;
}

interface Application extends DOM3EventSource {
    /*
     trueif the application is visible, falseotherwise. The value of this property is not affected by the
     application's Z-index or position relative to other applications. Only calls to the show()and hide()
     methods will affect its value.
     */
    visible: boolean;

    /*
     trueif the application is in the list of currently active applications, falseotherwise (as defined in
     Section 4.3.8).
     */
    active: boolean;

    /*
     StringCollectionobject containing the names of the permissions granted to this application.
     */
    permissions: string[];

    /*
     trueif the application receives cross application events before any other application, falseotherwise.
     */
    isPrimaryReceiver: boolean;

    /*
     A strict subset of the DOM Windowobject representing the application.
     */
    window: Window;

    /*
     Access the current application’s private data object.
     If the application accessing the privateDataproperty is not the currentapplication, the OITF SHALL
     throw an error as defined in section 10.1.1.
     */
    privateData: ApplicationPrivateData;

    onApplicationActivated: { (): void; };
    onApplicationDeactivated: { (): void; };
    onApplicationShown: { (): void; };
    onApplicationHidden: { (): void; };
    onApplicationPrimaryReceiver: { (): void; };
    onApplicationNotPrimaryReceiver: { (): void; };
    onApplicationTopmost: { (): void; };
    onApplicationNotTopmost: { (): void; };
    onApplicationDestroyRequest: { (): void; };
    onApplicationHibernateRequest: { (): void; };
    onKeyUp: { (): void; };
    onKeyDown: { (): void; };

    /*
     If the application visualization mode as defined by method
     getApplicationVisualizationMode()in Section 7.2.1.3 is:
     1 : Make the application visible.
     2 : Make the application visible. Calling this method from the application itself may
     have no effect.
     3 : Request to make the application visible.
     This method only affects the visibility of an application. In the case where more than
     one application is visible, calls to this method will not affect the z-index of the
     application with respect to any other visible applications.
     */
    show: () => void;

    /*
     If the application visualization mode as defined by method
     getApplicationVisualizationMode()in Section 7.2.1.3 is:
     1 : Make the application invisible.
     2 : Make the application invisible. Calling this method from the application itself may
     have no effect.
     3 : Request to make the application invisible.
     Calling this method has no effect on the lifecycle of the application.
     */
    hide: () => void;

    /*
     Move the application to the front of the active applications list. If the application has
     been hidden using Application. Hide(), this method does not cause the
     application to be shown.
     If the application visualization mode as defined by method
     getApplicationVisualizationMode()in Section 7.2.1.3 is:
     1 : The application’s Windowobject SHALL be moved to the top of the stack of visible
     applications. In addition, the application’s Windowobject SHALL gain input focus if
     argument gainFocushas value true.
     2 : The application’s Windowobject SHALL be moved to the top of the stack of visible
     Page 75 (369)
     Copyright 2011 © Open IPTV Forum
     applications. In addition, the application’s Windowobject SHALL gain input focus if
     argument gainFocushas value true.Calling this method from the application itself
     MAY have no effect.
     3 : Request to make the application’s Windowobject visible. Once visible, the
     application SHALL be given input focus, irrespective of the value for argument
     gainFocus.
     */
    activateInput: (gainFocus: boolean) => void;

    /*
     Remove the application from the active applications list. This has no effect on the
     lifecycle of the application and MAY have no effect on the resources it uses.
     Applications which are not active will receive no cross-application events, unless their
     Applicationobject is the target of the event (as for the events defined in Section
     7.2.6). Applications may still be manipulated via their Applicationobject or their
     DOM tree.
     */
    deactivateInput: () => void;

    /*
     Create a new application and add it tothe application tree. Calling this method does
     not automatically show the newly-created application.
     This call is asynchronous and may return before the new application is fully loaded.
     An ApplicationLoadedevent will be targeted at the Applicationobject when the
     new application has fully loaded.
     If the application cannot be created, this method SHALL return null.
     */
    createApplication: (uri: string, createChild: boolean) => Application;

    /*
     Terminate the application, detach it from the application tree, and make any
     resources used available to other applications. When an application is terminated,
     any child applications shall also be terminated.
     */
    destroyApplication: () => void;

    /*
     Starts a Widget installed on the OITF. The behaviour of this method is equivalent to
     that of Application.createApplication().
     The Widget is identified by its WidgetDescriptor. To get a list of the
     WidgetDescriptorobjects for the installed Widgets one can check
     ApplicationManager.widgetsproperty. If the Widget is already running or fails to
     start this call will return null.
     */
    startWidget: (wd: WidgetDescriptor, createChild: boolean, aUrl: string) => Application;

    /*
     Terminate a running Widget. The behaviourof this method is equivalent to that of
     Application.destroyApplication().
     Calling this method will detach the Widget from the application tree, and make any
     resources used available to other applications. When a Widget is terminated, any
     child applications shall also be terminated.
     */
    stopWidget: (wd: WidgetDescriptor) => void;

    // humax extension
    createHumaxApplication: (name: string, url: string) => Application;
    destroyHumaxApplication: (command : string) => void;
}

interface ApplicationPrivateData {
}

interface ApplicationCollection extends Collection {
}

// 7.3.2 The Configuration class

interface Configuration {
    /*
     A comma-separated set of languages to be used for audio playback, in order of preference.
     Each language SHALL be indicated by its ISO 639 language code.
     */
    preferredAudioLanguage: string;

    /*
     A comma-separated set of languages to be used for subtitle playback, inorder of preference.
     Each language SHALL be indicated by its ISO 639 language code.
     */
    preferredSubtitleLanguage: string;

    /*
     An ISO-3166 three character country code identifying the country in which the receiver is deployed.
     */
    countryId: string;

    /*
     An integer indicating the time zone within a country in which the receiver is deployed. A value of 0
     SHALL represent the eastern-most time zone in the country, a value of 1 SHALL represent the next
     time zone to the west, and so on.
     Valid values are in the range 0 – 60.
     */
    regionId: number;

    /*
     The policy dictates what mechanism the systemshould use when storage space is exceeded.
     Valid values are shown in the table below.
     */
    pvrPolicy: number;

    /*
     When the pvrPolicyproperty is set to the value 2, this property indicates the minimum number of
     episodes that SHALL be saved for series-link recordings.
     */
    pvrSaveEpisodes: number;

    /*
     When the pvrPolicyproperty is set to the value 2, this property indicates the minimum save time (in
     days) for individual recordings. Only recordings older than the save time MAY be deleted.
     */
    pvrSaveDays: number;

    /*
     The default padding (measured in seconds) to be added at the start of a recording.
     */
    pvrStartPadding: number;

    /*
     The default padding (measured in seconds) to be added at the end of a recording.
     */
    pvrEndPadding: number;

    /*
     The time shift mode indicates the preferred mode of operation for support of timeshift playback in the
     video/broadcast object. Valid values are defined inthe timeShiftMode property in Section7.13.2.2.
     The default value is 0, timeshift is turned off.
     */
    preferredTimeShiftMode: number;

    /*
     Get the system text string that has been set for the specified key.
     */
    getText: (key: string) => string;

    /*
     Set the system text string that has been set for the specified key. System text strings
     are used for automatically-generated messages in certain cases, e.g. parental control
     messages.
     */
    setText: (key: string, value: string) => void;

}

// 7.3.3 The LocalSystem class

interface LocalSystem extends DOM3EventSource {

    /*
     Private OITF Identifier. Unique identifier which SHALL be the same as X-HNI-IGI-OITF-DeviceIDin
     [OIPF_PROT2]. This property SHALL take the value undefinedexcept when accessed by
     applications meeting either of the following criteria:
     */
    deviceID: string;

    /*
     Indicates whether the system has finished initialising. A value of trueindicates that the system is
     ready.
     */
    systemReady: boolean;

    /*
     String identifying the vendor name of the device.
     */
    vendorName: string;

    /*
     String identifying the model name of the device.
     */
    modelName: string;

    /*
     String identifying the version number of the platform firmware.
     */
    softwareVersion: string;

    /*
     String identifying the version number of the platform hardware.
     */
    hardwareVersion: string;

    /*
     String containing the serial number of the platform hardware.
     */
    serialNumber: string;

    /*
     Release version of the OIPF specification implemented by the OITF.
     For instance, if the OITF implements release 2 version “1.0”, this property should be set to 2.
     */
    releaseVersion: number;

    /*
     Major version of the OIPF specification implemented by the OITF.
     For instance, if the OITF implements release 2 version “2.0”, this property should be set to 2.
     */
    majorVersion: number;

    /*
     Minor version of the OIPF specification implemented by the OITF.
     For instance, if the OITF implements release 2 version “2.0”, this property should be set to 0.
     */
    minorVersion: number;

    /*
     Profile of the OIPF specification implemented by the OITF. Values of this field are not defined in this
     specification.
     */
    oipfProfile: string;

    /*
     Flag indicating whether the platform has PVR capability (local PVR).
     Note: This property is deprecated in favour of the pvrSupportproperty.
     */
    pvrEnabled: boolean;

    /*
     Flag indicating whether the platform has CI+ capability.
     */
    ciplusEnabled: boolean;

    /*
     Get or set the standby state of the receiver. A value of trueindicates that the receiver is in standby
     mode.
     Note - the property is deprecated in favour of the powerStateproperty.
     */
    standbyState: boolean;

    /*
     The powerStateproperty provides the DAE application the ability to determine the current state of the
     OITF. The property is limited to the ACTIVE_STANDBYor ONstates.
     Note this state deprecates the standbyStateproperty.
     */
    powerState: number;

    /*
     The previousPowerStateproperty provides the DAE application the ability to retrieve the previous
     state.
     */
    previousPowerState: number;

    /*
     The time that the OITF entered the current power state. The time is represented in seconds since
     midnight (GMT) on 1/1/1970.
     */
    timeCurrentPowerState: number;

    onThermalProtectionOccured: { (): void; };

    /*
     The function that is called when the power state has changed.
     */
    onPowerStateChange: { (powerState: number): void; };

    /*
     Get or set the overall system volume. Valid values for this property are in the range 0 - 100.
     */
    volume: number;

    /*
     Get or set the mute status of the default audio output(s). A value of trueindicates that the default
     output(s) are currently muted.
     */
    mute: boolean;

    /*
     A collection of AVOutputobjects representing the audio and video outputs of the platform.
     Applications MAY use these objects to configure and control the available outputs.
     */
    outputs: AVOutputCollection;

    /*
     A collection of NetworkInterfaceobjects representing the available network interfaces.
     */
    networkInterfaces: NetworkInterfaceCollection;

    /*
     A collection of Tunerobjects representing the physical tuners available in the OITF.
     */
    tuners: TunerCollection;

    /*
     Get the TV standard(s) for which the system is configured. This enables the user interface to only
     display those options relevant to the available TV standard(s).
     */
    tvStandard: number;

    /*
     Flag indicating the type of PVR support used by the application. This property may take zero or more of
     the following values:
     */
    pvrSupport: number;

    /*
     Set the resolution of the graphics plane. If the specified resolution is not supported by the
     OITF, this method SHALL return false. Otherwise, this method SHALL return true.
     */
    setScreenSize: (width: number, height: number) => boolean;

    /*
     Set the type of PVR support used by the application. The types of PVR supported by the
     receiver MAY not be supported by the application; in this case, the return value indicates
     the pvr support that has been set.
     */
    setPvrSupport: (state: number) => number;

    /*
     The setPowerState()method allows the DAE application to modify the OITF state.
     The power state change may be restricted for some values of type, for example OFFand
     PASSIVE_STANDBY. A call to setPowerStatewith a restricted value of type SHALL return
     false.
     */
    setPowerState: (type: number) => boolean;

    /*
     Set the credentials for the specified protocol to use for digest authentication negotiation for
     all subsequent requests to the specified domain. The credentials are persistently stored
     overwriting any previous set credentials. If domain is null the provided credentials SHALL
     apply for all domains. Returns trueif credentials are successfully set, falseotherwise.
     */
    setDigestCredentials: (protocol: string, domain: string, username: string, password: string) => boolean;

    /*
     Clear any previously set digest credentialsfor the specified domain. If domain is null all set
     credentials are cleared.
     */
    clearDigestCredentials: (protocol: string, domain: string) => boolean;
}

// 7.3.4 The NetworkInterface class
interface NetworkInterface {

    /*
     The IP address of the network interface, in dotted-quad notation for IPv4 or colon-hexadecimal notation
     for IPv6.
     */
    ipAddress: string;

    /*
     The colon-separated MAC address of the network interface.
     */
    macAddress: string;

    /*
     Flag indicating whether the network interface is currently connected.
     */
    connected: boolean;

    /*
     Flag indicating whether the network interface isenabled. Setting this property SHALL enable or
     disable the network interface.
     */
    enabled: boolean;
}

/*
 7.3.5 The AVOutput class
 The AVOutputclass represents an audio or video output on the local platform.
 */

interface AVOutput {

    /*
     The name of the output. Each output SHALL have a name that is unique on the local system. At least
     one of the outputs SHALL have the name "all" and SHALL represent all available outputs on the
     platform.
     */
    name: string;

    /*
     The type of the output. Valid values are “audio”, “video”, or “both”.
     */
    type: string;

    /*
     Flag indicating whether the output is enabled. Setting this property SHALL enable or disable the
     output.
     */
    enabled: boolean;

    /*
     Flag indicating whether the subtitles are enabled.The language of the displayed subtitles is
     determined by a combination of the value of the Configuration.preferredSubtitleLanguage
     property (see Section 7.3.2.1) and the subtitles available in the stream. For audio outputs, setting this
     property will have no effect.
     */
    subtitleEnabled: boolean;

    hardOfHearingEnabled: boolean;
    /*
     Read or set the video format conversion mode, for which hardware support MAY be available on the
     device, used when displaying a 4:3 signal on a 16:9 display. Valid values are:
     •  normal
     •  stretch
     •  zoom
     The actual effect on the display, for example how bars are introduced when stretching an input video,
     depends on the value of this property, the aspect ratio of the display device and the aspect ratio of the
     input video (represented by the aspectRatioproperty of the appropriate instance of the
     AVVideoComponentclass).
     An OITF that does not support its own display (e.g. STB) may also signal over the interface (e.g. HDMI
     and SCART) to the TV set which may also have effect on the actual display. This specification remains
     silent on the actual effect.
     The DAE application graphical layer is unaffected by the videoMode.
     For audio-only outputs, setting this property SHALL have no effect.
     */
    videoMode: string;

    /*
     Set the output mode for digital audio outputs for which hardware support MAY be available on the
     device. Valid values are shown below.
     Value Behaviour
     ac3  Output AC-3 audio.
     uncompressed  Output uncompressed PCM audio.
     For video-only outputs, setting this property SHALL have no effect.
     */
    digitalAudioMode: string;

    /*
     Set the range for digital audio outputs for which hardware support MAY be available on the device.
     Valid values are shown below
     Value Behaviour
     normal  Use the normal audio range.
     narrow  Use a narrow audio range.
     wide  Use a wide audio range.
     For video-only outputs, setting this property SHALL have no effect.
     */
    audioRange: string;

    /*
     Set the video format for HD video outputs for which hardware support MAY be available on the device.
     Valid values are:
     •  480i
     •  480p
     •  576i
     •  576p
     •  720p
     •  1080i
     •  1080p
     For audio-only or standard-definition outputs,setting this property SHALL have no effect.
     */
    hdVideoFormat: string;

    /*
     Indicates the display aspect ratio of the display device connected to this output for which hardware
     support MAY be available on the device. Valid values are:
     •  4:3
     •  16:9
     Other values may be indicated but are not listed.
     For audio-only outputs, setting this property SHALL have no effect.
     */
    tvAspectRatio: string;

    /*
     Read the video format conversion modes thatmay be used. See the definition of the videoModes
     property for valid values.
     For audio outputs, this property will have the value null.
     */
    supportedVideoModes: string[];

    /*
     Read the supported ouput modes for digital audio outputs. See the definition of the
     digitalAudioModeproperty for valid values.
     For video outputs, this property will have the value null.
     */
    supportedDigitalAudioModes: string[];

    /*
     Read the supported ranges for digital audio outputs. See the definition of the audioRangeproperty for
     valid values.
     For video outputs, this property will have the value null.
     */
    supportedAudioRanges: string[];

    /*
     Read the supported HD video formats. See the definition of the hdVideoFormatproperty for valid
     values.
     For audio outputs, this property will have the value null.
     */
    supportedHdVideoFormats: string[];

    /*
     Read the supported TV aspect ratios. See the definition of the tvAspectRatioproperty for valid
     values.
     For audio outputs, this property will have the value null.
     */
    supportedAspectRatios: string[];
}

/*
 7.3.6 The NetworkInterfaceCollection class
 */

interface NetworkInterfaceCollection extends Collection {
}

/*
 7.3.7 The AVOutputCollection class
 */
interface AVOutputCollection extends Collection {
}

/*
 7.3.8 The TunerCollection class
 */
interface TunerCollection extends Collection {
}

/*
 7.3.9 The Tuner class
 */

interface Tuner {

    /*
     A unique identifier of the tuner.
     */
    id: number;

    /*
     The name of the tuner as designated in OITF.
     */
    name: string;

    /*
     Returns a collection of the types supported by the tuner. The types are according to the ID types in
     section 7.13.12.1 under Channelobject.
     */
    idTypes: string[];

    /*
     The property enables (true) and disables (false) the tuner. Reading the property provides the current
     state, enabled or disabled. Attempting to disable the tuner while the resource is in use has no effect
     and the tuner SHALL continue to be enabled. While disabled:
     •any external power feed (if applicable) SHALL be turned off;
     •the value of the signalInfo property is not defined;
     •the tuner SHALL NOT be available for use by any JavaScript object (e.g. the video/broadcast
     object) or by the underlying OITF system (e.g. to perform a scheduled recording). Note the
     property enableTuneris available in order to re-enable the tuner and get access to the tuner
     again.
     The set value of the property SHALL persist after OITF restarts.
     */
    enableTuner: boolean;

    /*
     The property returns a SignalInfoobject with signal information for example signal strength.
     */
    signalInfo: SignalInfo;

    /*
     The property turns on (true) and off (false) the power applied to the external interface of the tuner
     unless the tuner is disabled. Reading the property provides the current value, on or off. Attempting to
     modify the property while the resource is in use has no effect. The value of the property SHALL persist
     after OITF restarts.
     For DVB-S/S2 power is supplied to the LNB(s) and if present the DiSEqC switch.
     For DVB-T/T2 a supply +5V is supplied to the antenna with built in amplifier. Note that applying power
     may have adverse effects to the external equipment ifit has its own power supply. It is a strong
     recommendation to indicate to the end user a possible adverse effect before using this method.
     For DVB-C/C2 there is no effect.
     Reading the property provides the current value.
     */
    powerOnExternal: boolean;
}

/*
 7.3.10 The SignalInfo class
 */
interface SignalInfo {

    /*
     Signal strength measured in dBm, for example -31.5dBm.
     */
    strength: number;

    /*
     Signal quality with range from 0 to 100. Calculation of quality is a function of berand snr. The
     specification remains silent as to how the calculation is made.
     */
    quality: number;

    /*
     Bit error rate.
     */
    ber: number;

    /*
     Signal to noise ratio (dB), for example 22.3dB.
     */
    snr: number;

    /*
     Trueif the tuner is locked to current transponder.
     */
    lock: boolean;
}
/*
 7.9.1 ParentalControlManager
 */
interface ParentalControlManager {
    parentalRatingSchemes: ParentalRatingSchemeCollection;

    /*
     A reference to the collection of rating schemes known by the OITF.
     */
    aParentalRatingScheme: ParentalRatingScheme;

    /*
     The lockout status of the parental control PIN. If the incorrect PIN has been entered too many times
     in the configured timeout period, parental control PIN entry SHALL be locked out for period of time
     determined by the OITF.
     */
    isPINEntryLocked: () => boolean;

    /*
     As defined in [OIPF_CSP2], the OITF shall prevent the consumption of a programme
     when its parental rating doesn't meet the parental rating criterion currently defined in
     the OITF. Calling this method with enable set to false will temporarily allow the
     consumption of any blocked programme.
     Setting the parental control status using this method SHALL set the status until the
     consumption of any of all the blocked programmes terminates (e.g. until the content
     item being played is changed), or another call to the
     setParentalControlStatus() method is made.
     Setting the parental control status using this method has the following effect :for the
     Programme and Channel objects as defined in Sections 7.16.2 and 7.13.12, the
     blocked property of a programme or channel SHALL be set to true for programmes
     whose parental rating does not meet the applicable parental rating criterion, but the
     locked property SHALL be set to false.
     This operation to temporarily disable parental rating control SHALL be protected by
     the parental control PIN (i.e. through the pcPIN argument). The return value indicates
     the success of the operation, and SHALL take one of the following values:
     0 The PIN is correct.
     1 The PIN is incorrect.
     2 PIN entry is locked because an invalid PIN has been entered too many
     times. The number of invalid PIN attempts before PIN entry is locked is
     outside the scope of this specification.
     pcPIN The parental control PIN.
     enable Flag indicating whether parental control should be enabled.
     */
    setParentalControlStatus: (pcPIN: string, enable: boolean) => number;

    /*
     Returns a flag indicating the temporary parental control status set by
     setParentalControlStatus(). Note that the returned status covers parental
     control functionality related to all rating schemes.
     */
    getParentalControlStatus: () => boolean;

    /*
     Returns a flag indicating whether or not the OITF has been configured by the user to
     block content for which a parental rating is absent.
     */
    getBlockUnrated: () => boolean;

    /*
     Set whether programmes for which no parental rating has been retrieved from the
     metadata client nor defined by the service provider should be blocked automatically
     by the terminal.
     This operation SHALL be protected by the parental control PIN (if PIN entry is
     enabled). The return value indicates the success of the operation, and SHALL take
     one of the following values:
     0 The PIN is correct.
     1 The PIN is incorrect.
     2 PIN entry is locked because an invalid PIN has been entered too many
     times. The number of invalid PIN attempts before PIN entry is locked is
     outside the scope of this specification.
     */
    setBlockUnrated: (pcPIN: string, block: boolean) => number;

    /*
     Set the parental control PIN.
     This operation SHALL be protected by the parental control PIN (if PIN entry is
     enabled). The return value indicates the success of the operation, and SHALL take
     one of the following values:
     Value Description
     0 The PIN is correct.
     1 The PIN is incorrect.
     2 PIN entry is locked because an invalid PIN has been entered too many
     times. The number of invalid PIN attempts before PIN entry is locked is
     outside the scope of this specification.
     oldPcPIN The current parental control PIN.
     newPcPIN The new value for the parental control PIN.
     pcPIN The parental control PIN.
     block Flag indicating whether programmes SHALL be blocked.
     */
    setParentalControlPIN: (oldPcPIN: string, newPcPIN: string) => number;

    /*
     Unlock the object specified by target for viewing if pcPIN contains the correct
     parental control PIN.
     The return value indicates the success of the operation, and SHALL take one of the
     following values:
     Value Description
     0 The PIN is correct.
     1 The PIN is incorrect.
     2 PIN entry is locked because an invalid PIN has been entered too many
     times. The number of invalid PIN attempts before PIN entry is locked is
     outside the scope of this specification.
     3 Invalid object
     The object type of target can be one of the following:
     • Channel object, in which case the broadcast channel currently being presented
     SHALL be unlocked as long as the ccid of the object matches the broadcast
     channel. If the channel object does not match the broadcast channel, an Invalid
     Object error SHALL be returned. The channel SHALL remain unlocked until the
     broadcast video channel is changed to a different one or has stopped being
     presented (e.g. the OITF being powered off or put in standby).
     • Video/broadcast object, in which case the content being presented through this
     object SHALL be unlocked until a new channel is selected.
     • A/V control object, in which case the content being presented through this
     object SHALL be unlocked until a new item of content is played using this
     object.
     Otherwise an Invalid Object error SHALL be returned.
     pcPIN The parental control PIN.
     target The object to be unlocked.
     duration The length of time (in seconds) for which the item SHALL be
     unlocked.
     */
    unlockWithParentalControlPIN: (pcPIN: string, target: any) => number;

    /*
     Verify that the PIN specified by pcPIN is the correct parental control PIN.
     This method will return one of the following values:
     0 The PIN is correct.
     1 The PIN is incorrect.
     2 PIN entry is locked because an invalid PIN has been entered too
     many times. The number of invalid PIN attempts before PIN entry is
     locked is outside the scope of this specification
     pcPIN The parental control PIN to be verified.
     */
    verifyParentalControlPIN: (pcPIN: string) => number;

    /*
    HUMAX EXTENSION
     */
    resetParentalControlPIN: (param: string) => number;
    isNagraSmartCardInserted: () => boolean;
    setNargaSmartCardPIN: (param1: string, param2: string) => boolean;
    getNargaSmartCardPINRetryCount: () => number;
    getNargaSmartCardPINRemainTime: () => number;
    verifyNargaSmartCardPIN: (param1: string) => number;
}

/*
 7.9.2 The ParentalRatingScheme class
 typedef Collection<String> ParentalRatingScheme
 A ParentalRatingSchemedescribes a single parental rating scheme that may be in use for rating content, e.g. the
 MPAA or BBFC rating schemes. It is a collection of strings representing rating values, which next to the properties and
 methods defined below SHALL support the array notation to access the rating values in this collection. For the natively
 OITF supported parental rating systems the values SHALL be ordered by the OITF to allow the rating values to be
 compared in the manner as defined for property threshold for the respective parental rating system. Using a threshold as
 defined in this API may not necessarily be the proper way in which parental rating filtering is applied on the OITF, e.g.
 the US FCC requirements take precedence for device to be imported to the US.
 The parental rating schemes supported by a receiver MAY vary between deployments.
 See annex K for the definition of the collection template. In addition to the methods and properties defined for generic
 collections, the ParentalRatingScheme class supports the additional properties and methods defined below.
 */

interface ParentalRatingScheme {

    /*
     The unique name that identifies the parental rating scheme. Valid strings include:
     •the URI of one of the MPEG-7 classification schemes representing a parental rating scheme as
     defined by the uriattribute of one of the parental rating <ClassificationScheme>elements in
     [MPEG-7].
     •the string value “urn:oipf:GermanyFSKCS” to represent the GermanyFSK rating scheme as
     defined in [META].
     •the string value “dvb-si”: this means that the scheme of a minimum recommended age encoded
     as per [EN 300 468], is used to represent the parental rating values.
     If the value of nameis “dvb-si”, the ParentalRatingSchemeremains empty (i.e.
     ParentalRatingScheme.length == 0).
     */
    name: string;

    /*
     The parental rating threshold that is currently in use by the OITF’s parental control system for this rating
     scheme, which is encoded as a ParentalRatingobject in the following manner:
     If the value of the nameproperty of the ParentalRatingSchemeobject is unequal to “dvb-si”, then:
     •the valueproperty of the threshold object represents the value for which items with a
     ParentalRating. Valuegreater or equal to the valueproperty of the threshold object have
     been configured by the OITF’s parental control subsystem to be blocked.
     •the labelsproperty of the threshold object represents the bit map of zero or more flags for which
     items with a ParentalRating. Labelsproperty with any of the same flags set have been
     configured by the OITF’s parental control subsystem to be blocked.
     If the value of the nameproperty of the ParentalRatingSchemeobject is “dvb-si”, the threshold
     indicates a minimum recommended age encoded as per [EN 300 468] at which or above which the
     content is being blocked by the OITF’s parental control subsystem
     Note that the valueproperty as an index into the ParentalRatingobject that defines the threshold
     can be 1 larger than the value of ParentalRatingScheme.lengthto convey that no content is being
     blocked by the parental control subsystem.
     */
    threshold: ParentalRating;

    /*
     Return the index of the rating represented by attribute ratingValueinside the
     parental rating scheme string collection, or -1if the rating value cannot be found in
     the collection.
     */
    indexOf(ratingValue: string): number;

    /*
     Return the URI of the icon representing the rating at indexin the rating scheme, or
     undefinedif no item is present at that position.If no icon is available, this method
     SHALL return null.
     */
    iconUri(index: number): string;
}

/*
 7.9.3 The ParentalRatingSchemeCollection class
 typedef Collection<ParentalRatingScheme> ParentalRatingSchemeCollection
 A ParentalRatingSchemeCollectionrepresents a collection of parental rating schemes, e.g. as returned by
 property parentalRatingSchemesof the “application/oipfParentalControlManager” object as defined
 in Section 7.9.1. Next to the properties and methods defined below a ParentalRatingSchemeCollectionobject
 SHALL support the array notation to access the parental rating scheme objects in this collection.
 See annex K for the definition of the collection template. In addition to the methods and properties defined for generic
 collections, the ParentalRatingSchemeCollectionclass supports the additional properties and methods defined
 below.
 */

interface ParentalRatingSchemeCollection extends Collection {

    /*
     Create a new ParentalRatingSchemeobject and adds it to the
     ParentalRatingSchemeCollection. Applications MAY use this method to register
     additional parental rating schemes with the platform. When registered, the new
     parental rating scheme SHALL (temporarily) be accessible through the
     parentalRatingSchemesproperty of the
     “application/oipfParentalControlmanager” object as defined in Section 7.9.1.
     The application SHALL make sure that the values are ordered in such a way to allow
     the rating values to be compared in the manner as defined for the threshold
     property for the respective parental rating system.
     This method returns a reference to the ParentalRatingSchemeobject representing
     the added scheme. If the value of the nameparameter corresponds to an already-registered rating scheme, this method returns a reference to the existing
     ParentalRatingSchemeobject. If the newly defined rating scheme was not known
     to the OITF, the scheme MAY be stored persistently, and the OITF may offer a UI to
     set the parental rating blocking criteria for the newly added parental rating scheme.
     If the OITF has successfully stored (persistently or not persistently) the additional
     parental rating scheme, the method SHALL return a non-null
     ParentalRatingSchemeobject.
     */
    addParentalRatingScheme(name: string, values: string): ParentalRatingScheme;

    /*
     This method returns a reference to the ParentalRatingSchemeobject that is
     associated with the given scheme as specified through parameter name. If the value
     of name does not correspond to the nameproperty of any of the
     ParentalRatingSchemeobjects in the ParentalRatingSchemeCollection, the
     method SHALL return undefined.
     */
    getParentalRatingScheme(name: string): ParentalRatingScheme;
}

/*
 7.9.4 The ParentalRating class
 A ParentalRatingobject describes a parental rating value for a programme or channel. The ParentalRating
 object identifies both the rating scheme in use, and the parental rating value within that scheme.
 */
interface ParentalRating {

    /*
     The case-insensitive string representation of the parental rating value for the respective rating scheme
     denoted by property scheme.
     Valid strings include:
     •if the value of property schemerepresents one of the parental rating classification scheme names
     identified by [MPEG-7]: the string representation of one of the parental rating values as defined
     by one of the <Name>elements.
     •if the value of property schemeis ”urn:oipf:GermanyFSKCS” , the string representation of one
     the values for the GermanyFSK rating scheme as defined in [OIPF_META2].
     •if the value of property schemeis equal to “dvb-si”, the string representation of the minimum
     recommended age encoded as per [EN 300 468], which corresponds to rating_type 0 in
     [IEC62455].
     An example of a valid parental rating value is “PG-13”.
     */
    name: string;

    /*
     Unique case-insensitive name identifying the parental rating guidance scheme to which this parental
     rating value refers. Valid strings include:
     •the URI of one of the MPEG-7 classification schemes representing a parental rating scheme as
     defined by the uriattribute of one of the parental rating <ClassificationScheme>elements in
     [MPEG-7]
     •the string value “urn:oipf:GermanyFSKCS” to represent the GermanyFSK rating scheme as
     defined in [OIPF_META2].
     •the string value “dvb-si”: this means that the scheme of a minimum recommended age encoded
     as per [EN 300 468], is used to represent the parental rating values.
     */
    scheme: string;

    /*
     The parental rating value represented as an index into the set of values defined as part of the
     ParentalRatingSchemeidentified through property scheme.
     If an associated ParentalRatingSchemeobject can be found by calling method
     getParentalRatingScheme()on property parentalRatingSchemesof the
     application/oipfParentalControlManagerobject and the value of property schemeis not equal
     to “dvb-si”, then the valueproperty SHALL represent the index of the parental rating value inside the
     ParentalRatingSchemeobject, or -1 if the value cannot be found. If the value of property schemeis
     equal to “dvb-si”, then this property SHALL be the integerrepresentation of the string value of
     ParentalRatingproperty name.
     If no associated ParentalRatingSchemeobject can be found by calling method
     getParentalRatingSchemeon property parentalRatingSchemesof the
     application/oipfParentalControlManagerobject, then the valueproperty SHALL have value
     undefined.
     */
    value: number;

    /*
     The labels property represents a set of parental advisory flags that may provide additional information
     about the rating.
     The value of this field is a 32 bit integer value that represents a binary mask corresponding to the sum
     of zero or more label values defined in the table below. If no labels have been explicitly set, the value
     for the labelsproperty SHALL be 0.
     */
    labels: number;

    /*
     The region to which the parental rating value applies as case-insensitive alpha-2 region code as
     defined in ISO 3166-1. Returns undefinedif no specific region has been defined.
     */
    region: string;
}

/*
 7.9.5 The ParentalRatingCollection class
 typedef Collection<ParentalRating> ParentalRatingCollection
 A ParentalRatingCollectionrepresents a collection of parental rating values. See annex K for the definition of
 the collection template.
 In addition to the methods and properties defined for generic collections, the ParentalRatingCollectionclass
 supports the additional properties and methods defined below.
 */
interface ParentalRatingCollection extends Collection {

    /*
     Creates a ParentalRatingobject instance for a given parental rating scheme and
     parental rating value, and adds it to the ParentalRatingCollectionfor a
     programme or channel.
     */
    addParentalRating(scheme: string, name: string, value: number, labels: number, region: string): void;
}

/*
 7.10.1 The application/oipfRecordingScheduler embedded object
 */

/*
 7.13 Scheduled content and hybrid tuner APIs
 The OITF SHALL support the scheduling of recordings of broadcasts through the use of the following non-visual
 embedded object:
 <object type=“application/oipfRecordingScheduler”/>
 Note that the functionality in this section SHALL adhereto the security model as specified in Section 10.1.
 Which channels can be recorded SHALL be indicated by the ipBroadcastand HASattributes in the PVR capability
 indication (see Section 9.3.3). Within the channels indicated by these attributes, recording of both channels stored in the
 channel list and locally defined channels SHALL be supported.
 */

interface RecordingScheduler extends DOM3EventSource {

    /*
     Requests the scheduler to schedule the recording of the programme identified by the
     programmeIDproperty of the programme.
     If the programmeIDTypeof the programme has the value ID_TVA_GROUP_CRIDthen
     the ScheduledRecordingobject returned by this method SHALL be a “parent”
     scheduled recording object that conceptually represents the recording. Each
     individual programme in the SHALL be represented by a separate
     ScheduledRecordingobject. Note that ScheduledRecordingobjects for individual
     programmes may not be created until the CRID has been partially or completely
     resolved. The start time, duration and other properties of the programme SHALL NOT
     be used for scheduling any recording.
     Individual programmes SHALL be recorded if any entries in a programme’s
     associated groupCRIDscollection matches the group CRID specified in the
     programmeIDproperty of any “parent” recording.
     The other data contained in the programme object is used solely for annotation of the
     (scheduled) recording. If such programme metadata is provided, it SHALL be retained
     in the ScheduledRecordingobject that is returned if the recording of the programme
     was scheduled successfully, reflecting the possibility that not all relevant metadata
     might be available to the scheduler. When the programme is recorded, the metadata in
     the associated Recordingobject SHALL be updated with the metadata from the
     broadcast stream if such metadata is available..
     Note that the actual implementation of this method should enable the scheduler to
     identify the domain of the service that issues the scheduling request in order to support
     future retrieval of the scheduled recording through the getScheduledRecordings
     method.
     */
    record(programme: Programme): ScheduledRecording;

    /*
     Requests the scheduler to schedule the recording of the broadcast to be received over
     the channel identified by channelID, starting at startTimeand stopping at
     startTime + duration.
     The OITF SHOULD associate metadata with recordings scheduled using this method.
     This metadata MAY be obtained from the broadcast being recorded (for example DVB-SI in an MPEG-2 transport stream) or fromother sources of metadata. If an application
     anticipates that the OITF may not be able toobtain any metadata, it SHOULD instead
     of using this method;
     •create a Programmeobject (using the createProgramme()method) containing
     the channelID, startTimeand duration
     •populate that Programmeobject with metadata
     •pass that Programmeobject to the record( Programme )method.
     Note that the actual implementation of this method should enable the scheduler to
     identify the domain of the service that issues the scheduling request in order to support
     future retrieval of the scheduled recording through the getScheduledRecordings
     method.
     */
    recordAt(startTime: number, duration: number, repeatDays: number, channelID: string): ScheduledRecording;

    /*
     Returns a subset of all the recordings that are scheduled but which have not yet
     started. The subset SHALL include only scheduled recordings that were scheduled
     using a service from the same FQDN as the domain of the service that calls the
     method.
     */
    getScheduledRecordings(): ScheduledRecordingCollection;

    /*
     Returns the channel line-upof the tuner in the form of a ChannelConfigobject as
     defined in Section.7.13.9. This includes the favourite lists. The ChannelConfigobject
     returned from this function SHALL be identical to the ChannelConfigobject returned
     from the getChannelConfig()method on the video/broadcast object as defined in
     7.13.3.
     */
    getChannelConfig(): ChannelConfig;

    /*
     Remove a recording (either scheduled, in-progress or completed).
     For non-privileged applications, recordings SHALL only be removed when they are
     scheduled but not yet started and the recording was scheduled by the current service.
     As with the record method, only the programmeIDproperty of the scheduled recording
     SHALL be used to identify the scheduled recording to remove where this property is
     available. The other data contained in the scheduled recording SHALL NOT be used
     when removing a recording scheduled using methods other than recordAt(). For
     recordings scheduled using recordAt(), the data used to identify the recording to
     remove is implementation dependent..
     If the programmeIDTypeproperty has the value ID_TVA_GROUP_CRIDthen the OITF
     SHALL cancel the recording of the specified group.
     */
    remove(recording: ScheduledRecording): void;

    /*
     Factory method to create an instance of Programme
     */
    createProgrammeObject(): Programme;

}

/*
 7.10.2 The ScheduledRecording class
 The ScheduledRecordingobject represents a scheduled programme in the system, i.e. a recording that is scheduled
 but which has not yet started. For group recordings (e.g. recording an entire series), a ScheduledRecordingobject is
 also used to represent a “parent” recording that enables management of the group recording without representing any of
 the actual recordings in the group. The values of the properties of a ScheduledRecording(except for
 startPaddingand endPadding) are provided when the object is created using one of the record()methods in
 Section 7.10.1, for example by using a corresponding Programmeobject as argument for the record()method, and
 cannot be changed for this scheduled recording object (except for startPaddingand endPadding).
 */
interface ScheduledRecording {

    /*
     The amount of padding to add at the start of a scheduled recording, in seconds. This property is
     initialised to the value of the Configuration. PvrStartPaddingproperty. The default OITF defined
     start padding MAY be changed through property pvrStartPaddingof the Configurationclass as
     defined in Section 7.3.2. When a recording is due to start, the OITF MAY use a smaller amount of
     padding in order to avoid conflicts with other recordings.
     Positive values of this property SHALL cause the recording to start earlier than the specified start time
     (i.e. the actual duration of the recording shall be increased); negative values SHALL cause the
     recording to start later than the specified start time(i.e. the actual duration of the recording shall be
     decreased).
     */
    startPadding: number;

    /*
     The amount of padding to add at the end of a scheduled recording, in seconds. This property is
     initialised to the value of the Configuration. PvrEndPaddingproperty. The default OITF defined
     end padding MAY be changed through property pvrEndPaddingof the Configurationclass as
     defined in Section 7.3.2. When a recording is in progress, the OITF MAY use a smaller amount of
     padding in order to avoid conflicts with other recordings.
     Positive values of this property SHALL cause the recording to end later than the specified end time (i.e.
     the actual duration of the recording shall be increased); negative values SHALL cause the recording to
     end earlier than the specified end time (i.e. the actual duration of the recording shall be decreased).
     */
    endPadding: number;

    /*
     Bitfield indicating on which days of the week the recording SHOULD be repeated.
     */
    repeatDays: number;

    /*
     The short name of the scheduled recording, e.g. 'Star Trek: DS9'. For recordings scheduled using the
     oipfRecordingScheduler.recordAt()method, OITFs SHALL set this to an implementation-dependent value (e.g. “Manual Recording”).
     */
    name: string;

    /*
     The long name of the scheduled recording, e.g. 'Star Trek: Deep Space Nine'. If the long name is not
     available, this property will be undefined.
     */
    longName: string;

    /*
     The description of the scheduled recording, e.g. an episode synopsis. If no description is available, this
     property will be undefined.
     */
    description: string;

    /*
     The long description of the programme. If no description is available, this property will be undefined.
     */
    longDescription: string;

    /*
     The start time of the scheduled recording, measured in seconds since midnight (GMT) on 1/1/1970.
     The value for the startPaddingproperty can be used to indicate if the recording has to be started
     before the startTime(as defined by the Programmeclass).
     */
    startTime: number;

    /*
     The duration of the scheduled recording (in seconds). The value for the endPaddingproperty can be
     used to indicate how long the recording has to be continued after the specified duration of the
     recording.
     */
    duration: number;

    /*
     Reference to the broadcast channel where the scheduled programme is available.
     */
    channel: Channel;

    /*
     The unique identifier of the scheduled programme or series, e.g. a TV-Anytime CRID (Content
     Reference Identifier). For recordings scheduled using the oipfRecordingScheduler.recordAt()
     method, the value of this property MAY be undefined.
     */
    programmeID: string;

    /*
     The type of identification used to reference the programme, as indicated by one of the ID_* constants
     defined in Section 7.10.2.1. For recordings scheduled using the
     oipfRecordingScheduler.recordAt()method, the value of this property MAY be undefined.
     */
    programmeIDType: number;

    /*
     The episode number for the programme if itis part of a series. This property is undefinedwhen the
     programme is not part of a series or the information is not available.
     */
    episode: number;

    /*
     If the programme is part of a series, the total number of episodes in the series. This property is
     undefinedwhen the programme is not part of a series or the information is not available.
     */
    totalEpisodes: number;

    /*
     A collection of parental rating values for the programme for zero or more parental rating schemes
     supported by the OITF. The value of this property is typically provided by a corresponding
     “Programme” object that is used to schedule the recording and cannot be changed for this scheduled
     recording object. If no parental rating information is available for this scheduled recording, this property
     is a ParentalRatingCollectionobject (as defined in Section 7.9.5) with length 0.
     Note that if the parentalRatingproperty contains a certain parental rating (e.g. PG-13) and the
     broadcast channel associated with this scheduled recording has metadata that says that the content is
     rated PG-16, then the conflict resolution is implementation dependent.
     Note that this property was formerly called “parentalRating” (singular not plural).
     */
    parentalRatings: ParentalRatingCollection;

    /*
     7.10.4 Extension to application/oipfRecordingScheduler for control of recordings
     */

    /*
     Provides a list of scheduled and recorded programmesin the system. This property may only provide
     access to a subset of the full list of recordings, as determined by the value of the manageRecordings
     attribute of the <recording>element in the client capability description (see Section 9.3.3).
     */
    recordings: ScheduledRecordingCollection;

    /*
     Get information about the status of the local storage device. The DiscInfoclass is defined in Section
     7.16.4.
     */
    discInfo: DiscInfo;

    /*
     This function is the DOM 0 event handler for notification of changes in the state of recordings.
     The specified function is called with the following arguments:
     •  Integer state– The current state of the recording. The states that are applicable are listed in
     section 7.13.2.2 in the definition of the recordingStateproperty ScheduledRecording
     recording– The recording to which this event refers.
     */
    onPVREvent: { (state: number, recording: Recording): void; };

    /*
     Returns the Recordingobject for which the value of the Recording.idproperty
     corresponds to the given idparameter. If such a Recordingdoes not exist, the
     method returns null.
     */
    getRecording(id: string): Recording;

    /*
     Stop an in-progress recording.The recording SHALL NOT be deleted.
     */
    stop(recording: Recording): void;

    /*
     Update the recordingsproperty to show the current status of all recordings.
     */
    refresh(): void;

}

// 7.10.3 The ScheduledRecordingCollection class

interface ScheduledRecordingCollection extends Collection {
}

/*
 7.10.5 The Recording class
 TheRecording class represents an in-progress or completed recording being made available through the extended
 PVR management functionality as defined in Section 7.10.4. This class implements the ScheduledRecording
 interface (see Section 7.10.2), with the following changes:
 •  The startPaddingproperty is read only.
 •  For in-progress recordings, changes to the value of the endPaddingproperty SHALL modify the actual duration of
 the recording. If the value of the endPaddingproperty is changed so that the current actual duration of the
 recording exceeds the new actual duration specified by the sum of the startPadding, durationand
 endPaddingproperties, the recording SHALL bestopped immediately. Changing the value of this property for a
 completed recording SHALL have no effect.
 Recordings MAY be “manual” in that they simply record a channel at a certain time, for a period - analogous to a
 traditional VCR - or alternatively recordings can be programme based.
 If an in-progress recording is interrupted and automatically resumed, e.g. due to a temporary power failure, all sections of
 the recording SHALL be represented by a single Recordingobject.
 Values of properties in the Recordingobject SHALL be obtained from metadata about the recorded programme and
 are typically copied from the Programme used for scheduling a recording by the record(Programme programme)
 method of the application/oipfRecordingSchedulerobject. In the event of a conflict between the metadata in
 the Programme and that in the broadcast channel, the conflict resolution is implementation dependent.
 See Section 7.10.4 for more information about the mapping between the properties of a Programmeand the BCG
 metadata.
 NOTE: The property parentalRatingsformerly defined as part of this class is now redundant following the
 renaming of the parentalRatingproperty in the ScheduledRecordingclass to parentalRatings.
 */

interface Recording extends ScheduledRecording {

    /*
     The state of the recording. Valid values are:
     •  RECORDING_REC_STARTED
     •  RECORDING_REC_COMPLETED
     •  RECORDING_REC_PARTIALLY_COMPLETED
     •  RECORDING ERROR
     */
    state: number;

    /*
     If the state of the recording has changed due to an error, this field contains an error code detailing the
     type of error. This is only valid if the value of the stateargument isRECORDING ERROR or
     RECORDING_REC_PARTIALLY_COMPLETEDotherwise this property SHALL be null. Valid values are:
     •  ERROR_REC_RESOURCE_LIMITATION
     •  ERROR_INSUFFICIENT_STORAGE
     •  ERROR_REC_UNKNOWN
     */
    error: number;

    /*
     An identifier for this recording. This value SHALL be unique to this recording and so can be used to
     compare two recording objects to see if they refer to the same recording. The OITF SHALL guarantee
     that recording identifiers are unique in relation to download identifiers and CODAssetidentifiers.
     */
    id: string;

    /*
     trueif the recording was scheduled using oipfRecordingScheduler.recordAt()or using a
     terminal-specific approach that does not use guide data to determine what to record, falseotherwise.
     If false, then any fields whose name matches a field in the Programmeobject contains details from the
     programme guide on the programme that has been recorded.
     If true, only the channel, start time and duration of the recording are valid.
     */
    isManual: boolean;

    /*
     If true, then this recording should not be automatically deleted by the system.
     */
    doNotDelete: boolean;

    /*
     The number of days for which an individual or manual recording SHOULD be saved. Recordings older
     than this value MAY be deleted. This property is initialised to the value of the
     Configuration.pvrSaveDaysproperty.
     */
    saveDays: boolean;

    /*
     The number of episodes of a series-link that SHOULD be saved. Older episodes MAY be deleted. This
     is only valid when set on the latest scheduled recording in the series. This property is initialised to the
     value of the Configuration.pvrSaveEpisodesproperty
     */
    saveEpisodes: number;

    /*
     Flag indicating whether the programme is blocked due to parental control settings or conditional access
     restrictions.
     The blockedand lockedproperties work together to provide a tri-state flag describing the status of a
     programme. This can best be described by the following table:
     Description blocked locked
     No parental control applies.  false false
     Item is above the parental rating threshold (or manually blocked); no PIN has
     been entered to view it and so the item cannot currently be viewed.
     true true
     Item is above the parental rating threshold (or manually blocked); the PIN has
     been entered and so the item can be viewed.
     true false
     Invalid combination – OITFs SHALL NOT support this combination  false true
     */
    blocked: boolean;

    /*
     Flag indicating the type of show. This field SHALL take one of the following values:
     Value Description
     0 The show is live.
     1 The show is a first-run show.
     2 The show is a rerun.
     */
    showType: number;

    /*
     Flag indicating whether subtitles or closed-caption information is available.
     */
    subtitles: boolean;

    /*
     Supported subtitle languages, indicated by iso639 language codes.
     */
    subtitleLanguages: string[];

    /*
     Flag indicating whether the programme has high-definition video.
     */
    isHD: boolean;

    /*
     Flag indicating whether the recording has encrypt.
     */
    isEncrypted: boolean;

    /*
     Flag indicating whether the programme has Audio Description.
     */
    isAD: boolean;

    /*
     Flag indicating whether the programme is broadcast in widescreen.
     */
    isWidescreen: boolean;

    /*
     Bitfield indicating the type of audio that is available for the programme. Since more than one type of
     audio may be available for a given programme, the value of this field SHALL consist of one or more of
     the following values ORed together:
     Value Description
     1 Mono audio
     2 Stereo audio
     4 Multi-channel audio
     */
    audioType: number;

    /*
     Flag indicating whether more than one audio language is available for this recording.
     */
    isMultilingual: boolean;

    /*
     Supported audio languages, indicated by iso639 language codes.
     */
    audioLanguages: string[];

    /*
     A collection of genres that describe this programme.
     */
    genres: string[];

    /*
     The actual start time of the recording, including any padding. This MAY not be the same as the
     scheduled start time of the recorded programme (e.g. due to a recording starting late, or due to
     start/end padding).
     */
    recordingStartTime: number;

    /*
     The actual duration of the recording, including any padding. This MAY not be the same as the
     scheduled duration of the recording (e.g. due to a recording finishing early, or due to start/end padding).
     */
    recordingDuration: number;

    /*
     A collection of the bookmarks set in a recording. If no bookmarks are set, the collection SHALL be
     empty.
     */
    bookmarks: BookmarkCollection;

    /*
     Flag indicating whether the current state of the parental control system prevents the recording from
     being viewed (e.g. a correct parental control PIN has not been entered to allow the recording to be
     viewed).
     */
    locked: boolean;
}

interface RecordingCollection extends Collection {
}

/*
 * 7.12.1 The application/oipfSearchManager embedded object
 *
 * OITFs SHALL implement the “application/oipfSearchManager” embedded object. This object provides a
 * mechanism for applications to create and manage metadata searches.
 */
interface MetadataSearchEvent {
    search: MetadataSearch;
    state: number;
    /*
     * HUMAX EXTENSION
     */
    id: number;
}

interface SearchManager extends DOM3EventSource {

    /*
     * The number of days for which guide data is available.A value of -1 means that the amount of guide
     * data available is unknown.
     */
    guideDaysAvailable: number;

    /*
     This function is the DOM 0 event handler for events indicating changes in metadata. This SHALL be
     raised when changes to the parental control settings change the lock status of an item, or when a new
     version of the metadata becomes available. The specified function is called with the arguments action,
     info and object. These arguments are defined as follows:
     •  Integer action– the type of update that has taken place. This field will take one of the
     following values:
     Value Description
     1 A new version of metadata is available (see clause 4.1.2.1.2 of
     [META]) and applications SHOULD discard all references to
     Programme objects immediately and re-acquire them.
     2 A change to the parental control flags for a content item has
     occurred (e.g. the user has unlocked the parental control features
     of the receiver, allowing a blocked item to be played).
     3 A flag affecting the filtering criteria of a channel has changed.
     Applications MAY listen for events with this action code to update
     lists of favourite channels, for instance.
     •  Integer info– extended information about the type ofupdate that has taken place. If the
     action argument is set to the value 3, the value of this field SHALL be one or more of the
     following:
     Value Description
     1 The list of blocked channels has changed.
     2 A list of favourite channels has changed.
     4 The list of hidden channels has changed.
     If the action argument is set to the value 2, the value of this field SHALL be one or more of:
     Value  Description
     1 The block status of a content item has changed.
     2 The lock status of a content item has changed.
     This field is treated as a bitfield, so values MAY be combined to allow multiple reasons to be passed.
     •  Object object– the affected channel, programme, or CoD asset. If more than one is affected,
     then this argument SHALL take the value null.
     */
    onMetadataUpdate: { (action: number, info: number, object: any): void; };

    /*
     This function is the DOM 0 event handler for events relating to metadata searches. The specified
     function is called with the arguments search and state. These arguments are defined as follows:
     •  MetadataSearch search– the affected search
     •  Number state– the new state of the search
     Value Description
     0 Search has finished. This event SHALL be generated when a
     search has completed or been aborted.
     1 More search results are available. Calling update()on the
     SearchResultsobject SHALL update the list of results to
     include the newly-retrieved data.
     2 The data returned by the search is no longer valid, e.g. because
     of a change in the metadata. Applications that still require the
     data SHALL repeat the search.
     */
    onMetadataSearch: { (search: MetadataSearch, state: number): void; };

    /*
     Create a MetadataSearchobject that can be used to search the metadata.
     */
    createSearch(searchTarget: number): MetadataSearch;

    /*
     Returns the channel line-upof the tuner in the form of a ChannelConfigobject as
     defined in Section 7.13.9. This includes the favourite lists. The ChannelConfig
     object returned from this function SHALL be identical to the ChannelConfigobject
     returned from the getChannelConfig()method on the video/broadcastobject
     as defined in 7.13.3.
     */
    getChannelConfig(): ChannelConfig;

}

/*
 7.12.2 The MetadataSearch class
 A MetadataSearchobject represents a query of the BCG and SD&S metadata about available programmes.
 Applications can create MetadataSearchobjects using the createSearch()method on the
 application/oipfSearchManagerobject. When metadata queries are performed on a remote server, the protocol
 used is defined in section 4.1.2.2 of [OIPF_META2].
 Changes to constraints or the ordering of search results SHALL be applied when the getResults()method on the
 corresponding SearchResultsobject is called.
 Due to the nature of metadata queries, searches are asynchronous and events are used to notify the application that results
 are available. MetadataSearchEventsSHALL be targeted at the application/oipfSearchManagerobject.
 To minimise race conditions, results are updated on request rather thandynamically. Upon receipt of a
 MetadataSearchEventindicating that more results are available, applications SHALL call update()on the
 corresponding SearchResultsobject to get the new results.
 */

interface MetadataSearch {

    /*
     * The target(s) of the search. Valid values of the searchTargetparameter are:
     * Value Description
     * 1 Metadata relating to scheduled content SHALL be searched.
     * 2 Metadata relating to on-demand content SHALL be searched.
     * These values SHALL be treated as a bitfield, allowing searches to be carried out across multiple search
     * targets.
     */
    searchTarget: number;

    /*
     * The query that will be carried out by this search.
     */
    query: Query;

    /*
     * The results found so far.
     * The values within result MAY change after subsequent calls toits update()method.
     */
    result: SearchResults;

    /*
     * Set the query terms to be used for thissearch, discarding any previously-set query
     * terms. Calling this method when a search is in progress SHALL:
     * •Abort any outstanding requests for results (equivalent to calling
     * results.abort()).
     * •Invalidate any existing search results and dispatch a MetadataSearchevent
     * with a value of 2 for the state argument.
     */
    setQuery(query: Query): void;

    /*
     * Constrain the search to only include results whose parental rating value is below the
     * specified threshold.
     */
    addRatingConstraint(scheme: ParentalRatingScheme, threshold: number): void;

    /*
     * Constrain the search to only include results whose parental rating value is below the
     * threshold currently set by the user.
     */
    addCurrentRatingConstraint(): void;

    /*
     * Constrain the search to only include results from the specified channels. If a channel
     * constraint has already been set, subsequent calls to
     * addChannelConstraint()SHALL add the specified channels to the list of channels
     * from which results should be returned.
     * For CoD searches, adding a channel constraint SHALL have no effect.
     */
    //addChannelConstraint(channels: ChannelList): void;

    /*
     * Constrain the search to only include results from the specified channel. If a channel
     * constraint has already been set, subsequent calls to addChannelConstraint()
     * SHALL add the specified channel to the list of channels from which results should be
     * returned.
     * For CoD searches, adding a channel constraint SHALL have no effect.
     */
    addChannelConstraint(channel: Channel): void;

    /*
     * Set the order in which results SHOULDbe returned in future. Any existing search
     * results SHALL not be re-ordered. Subsequent calls to orderBy()will apply further
     * levels of ordering within the order defined by previous calls. For example:
     * orderBy(“ServiceName”, true);
     * orderBy(“PublishedStart”, true);
     * will cause results to be ordered by service name and then by start time for results
     * with the same channel number.
     */
    orderBy(field: string, ascending: boolean): void;

    /*
     * Create a metadata query for a specific value in a specific field within the metadata.
     * Simple queries MAY be combined to create more complex queries. Applications
     * SHALL follow the ECMAScript type conversion rules to convert non-string values into
     * their string representation, if necessary.
     */
    createQuery(field: string, comparison: number, value: string): Query;

    /*
     * Retrieve guide data for a specified number of programmes from a given channel from
     * metadata contained in the stream as defined in section 4.1.3 of [OIPF_META2].
     * Searches made using this method will implicitly remove any existing constraints,
     * ordering or queries created by prior calls to methods on this object.
     */
    findProgrammesFromStream(channel: Channel, startTime: number, count: number): void;

    /*
     * NOTE: HUMAX EXTENSION
     */
    id: number;

    addConstraint(type: string, param: string): void;
}

/*
 7.12.3 The Query class
 The Queryclass represents a metadata query that the user wants tocarry out. This may be a simple search, or a complex
 search involving Boolean logic. Queries are immutable; an operation on a query SHALL return a new Queryobject,
 allowing applications to continue referring to the original query.
 */

interface Query {

    /*
     Create a query based on the logical AND of the predicates represented by the current
     query and the argument query.
     */
    and(query: Query): Query;

    /*
     Create a query based on the logical ORof the predicates represented by the current
     query and the argument query.
     */
    or(query: Query): Query;

    /*
     Create a new query that is the logical negation of the current query.
     */
    not(): Query;
}

/*
 7.12.4 The SearchResults class
 The SearchResultsclass represents the results of a metadata search. Since the result set may contain a large number
 of items, applications request a ‘window’ on to the result set, similar to the functionality provided by the OFFSET and
 LIMIT clauses in SQL.
 Applications MAY request the contents of the result in groups of an arbitrary size, based on an offset from the beginning
 of the result set. The data SHALL be fetched from the appropriate source, and application SHALL be notified when the
 data is available.
 Next to the properties and methods defined below a SearchResultsobject SHALL support the array notation to
 access the results in this collection.
 */

interface SearchResults {

    /*
     The number of items in the currently available results. If results are fetched asynchronously, the value
     of this property SHALL be zero until after update()has been called.
     */
    length: number;

    /*
     The current offset into the total result set.
     */
    offset: number;

    /*
     The total number of items in the result set. If results are fetched asynchronously, the value of this
     property SHALL be undefined until getResults()has been called and a MetadataSearchEvent
     notifying the application that results are available has been dispatched.
     */
    totalSize: number;

    /*
     Return the item at position index in the collection of currentlyavailable results, or
     undefinedif no item is present at that position. This function SHALL only return
     objects that are instances of Programmewhen searching metadata for scheduled
     content, or CODAsset, CODFolder, or CODServicewhen searching CoD metadata
     */
    item(index: number): any;

    /*
     Perform the search and retrieve a subset of the items that match the query.
     Results MAY be returned both synchronously and asynchronously, depending on
     whether data is available from the cache. If getResults()returns false, results
     are not available until the notification events have been returned and update()has
     been called. If getResults()returns true, results are available immediately, and
     the application need not wait for MetadataSearchEvents indicating that results are
     available or call update()to obtain the results.
     For results returned as a result of the same call to getResults(), the full result set
     may build up over time – the availability of new entries in the result set will be
     indicated by notification events. Subsequent calls to getResults()will clear the
     result set, so only results fetched for the most recent call to getResults()will be
     available to applications.
     */
    getResults(offset: number, count: number): boolean;

    /*
     Abort any outstanding request for results. Items currently in the collection SHALL be
     removed (i.e. the value of the lengthproperty SHALL be 0 and any calls to
     item()SHALL return undefined).
     */
    abort(): void;

    /*
     Through the update method new results are made available to applications. When a
     call to getResults()has returned false and results are fetched asynchronously, this
     method must be called after an application has received a notification event informing
     it that new results are available. The results may be delivered over multiple
     notification events.
     Until this method is called, results returned by asynchronous requests SHALL NOT
     be available to applications. This ensures that applications have a consistent view of
     the available results, without the result set changing asynchronously. This enables
     applications to (for example) iterate overthe current result set and update their UI
     before retrieving the new results which have been returned to the OITF but are not
     yet available to applications.
     */
    update(): void;
}

/*
 7.13.1 The video/broadcast embedded object
 The OITF SHALL support the video/broadcastembedded object with the following properties and methods, which
 SHALL adhere to the tuner related security requirements in Section 10.1.3.1. The MIME type of this object SHALL be
 “video/broadcast”.
 */
interface VideoBroadcast extends HTMLObjectElement {
    // from DOM3EventSource
    addEventListener(eventName: string, listener: Function, useCapture?: boolean): void;
    removeEventListener(eventName: string, listener: Function): void;

    /*
     The width of the area used for rendering the video object. This property is only writable if property
     fullScreenhas value false. Changing the widthproperty corresponds to changing the widthproperty
     through the HTMLObjectElementinterface, and must have the same effect as changing the width through
     the DOM Level 2 Style interfaces (i.e. CSS2Propertiesinterface style.width), at least for values
     specified in pixels.
     */
    //width: number;

    /*
     * The height of the area used for rendering the video object. This property is only writable if property
     * fullScreenhas value false. Changing the heightproperty corresponds to changing the height
     * property through the HTMLObjectElementinterface, and must have the same effect as changing the
     * height through the DOM Level 2 Style interfaces (i.e. CSS2Propertiesinterface style.height), at least
     * for values specified in pixels
     */
    //height: number;

    /*
     * Returns trueif this video object is in full-screen mode, falseotherwise. The default value is false.
     */
    fullScreen: boolean;

    /*
     Setting the value of the dataproperty SHALL have no effect on the video/broadcastobject. If this
     property is read, the value returned SHALL always be the empty string.
     */
    data: string;

    /*
     The function that is called when a request to switch a tuner to another channel resulted in an error
     preventing the broadcasted content from being rendered. This function may be called either in response to
     a channel change initiated by the application, or a channel change initiated by the OITF (see section
     7.13.1.1).
     */
    onChannelChangeError: { (channel: Channel, errorState: number): void; };

    /*
     The current play state of the video/broadcastobject. Valid values are:
     */
    playState: number;

    /*
     The function that is called when the play state of the video/broadcastobject changes. This function
     may be called either in response to an initiated by the application, an action initiated by the OITF or an
     error (see section 7.13.1.1).
     */
    onPlayStateChange: { (state: number, error: number): void; };

    /*
     The function that is called when a request to switch a tuner to another channel has successfully
     completed. This function may be called either in response to a channel change initiated by the application,
     or a channel change initiated by the OITF (see section 7.13.1.1).
     */
    onChannelChangeSucceeded: { (channel: Channel): void; };

    /*
     The function that is called when the value of fullScreenchanges. The default value is null.
     The specified function is called with no arguments.
     */
    onFullScreenChange: { (): void; };

    /*
     The function that is called when the video object gains focus. The specified function is called with no
     arguments.
     */
    onfocus: { (): void; };

    /*
     The function that is called when the video object loses focus. The specified function is called with no
     arguments.
     */
    onblur: { (): void; };

    /*
     Returns the channel line-upof the tuner in the form of a ChannelConfigobject as defined
     in Section 7.13.9. The method SHALL return the value nullif the channel list is not
     (partially) managed by the OITF (i.e., if the channel list information is managed entirely in
     the network).
     */
    getChannelConfig: () => ChannelConfig;

    /*
     If a broadcast channel is being presented under the control of the OITF (i.e. the video was
     being presented by the OITF before the application started) then move the control of the
     broadcast channel presentation to the application and present the currently playing
     broadcast channel in the video/broadcastobject. If the broadcast channel that is being
     presented is already under the control by an application instead of the OITF (either by the
     current application or by another running application) it cannot be bound using
     bindToCurrentChannel(). If video from exactly one channel is currently being presented
     by the OITF then this binds the video/broadcastobject to that video.
     */
    bindToCurrentChannel: () => Channel;

    /*
     Creates a Channelobject of the specified idType. This method is typically used to create
     a Channelobject of type ID_DVB_SI_DIRECT. The Channelobject can subsequently be
     used by the setChannel()method to switch a tuner to a channel that is not part of the
     channel list which was conveyed by the OITF to the server. The resulting Channelobject
     represents a locally-defined channel which does not get added to the channel list accessed
     through the ChannelConfigclass (see 7.13.10).
     */
    createChannelObject: (idType: number, dsd: string, sid: number) => Channel;
    //createChannelObject: (idType: number, onid: number, tsid: number, sid: number, sourceID: number, ipBroadcastID: string) => Channel;

    /*
     Requests the OITF to switch a (logicalor physical) tuner to the channel specified by
     channeland render the received broadcast content in the area of the browser allocated for
     the video/broadcastobject.
     */
    setChannel: (channel: Channel, trickplay: boolean, contentAccessDescriptorURL: string) => void;

    /*
     Requests the OITF to switch the tuner that is currently in use by the video/broadcast
     object to the channel that precedes the current channel in the active favourite list, or, if no
     favourite list is currently selected, to the previous channel in the channel list. If it has
     reached the start of the favourite/channel list, itSHALL cycle to the last channel in the list.
     If the current channel is not part of the channellist, the result of calling this method is
     implementation dependent.
     */
    prevChannel: () => void;

    /*
     Requests the OITF to switch the tuner that is currently in use by the video/broadcast
     object to the channel that succeeds the current channel in the active favourites list, or, if no
     favourite list is currently selected, to the next channel in the channel list. If it has reached
     the end of the favourite/channel list, it SHALL cycle to the first channel in the list. If the
     current channel is not part of the channel list, the result of calling this method is
     implementation dependent.
     */
    nextChannel: () => void;

    /*
     Stop presenting broadcast video. If the video/broadcast object is in any state other than the
     unrealized state, it SHALL transition tothe stopped state. and stop video and audio
     presentation. This SHALL have no effect on access to non-media broadcast resources
     such as EIT information.
     */
    stop: () => void;

    /*
     Sets the rendering of the video content to full-screen (fullscreen = true) or windowed
     (fullscreen = false) mode (as per [Req. 5.7.1.c] of [CEA-2014-A]). If this indicates a
     change in mode, this SHALL result in a change of the value of property fullScreen.
     Changing the mode SHALL NOT affect the z-index of the video object.
     */
    setFullScreen: (fullscreen: boolean) => void;

    /*
     Adjusts the volume of the currently playing media to the volume as indicated by volume.
     Allowed values for the volume argument are all the integer values starting with 0 up to and
     including 100. A value of 0 means the sound will be muted. A value of 100 means that the
     volume will become equal to current “master”volume of the device, whereby the “master”
     volume of the device is the volume currently set for the main audio output mixer of the
     device. All values between 0 and 100 define a linear increase of the volume as a
     percentage of the current master volume, whereby the OITF SHALL map it to the closest
     volume level supported by the platform.
     */
    setVolume: (volume: number) => boolean;

    /*
     Returns the actual volume level set; for systems that do not support individual volume
     control of players, this method will have no effect and will always return 100.
     */
    getVolume: () => number;

    /*
     Releases the decoder/tuner used for displaying the video broadcast inside the
     video/broadcast object, stopping any form of visualization of the video inside the
     video/broadcastobject and releasing any other associated resources.
     */
    release: () => void;

    // 7.13.2 Extensions to video / broadcast for recording and time - shift

    /*
     The function that is called when the playback speed of a channel changes.
     */
    onPlaySpeedChanged: { (speed: number): void; };

    /*
     The function that is called when change occurs in the play position of a channel due to the use of trick
     play functions.
     */
    onPlayPositionChanged: { (position: number): void; };

    /*
     Returns the playback position, specified as the positiveoffset of the live broadcast in seconds, in the
     currently rendered (timeshifted) broadcast.
     */
    playbackOffset: number;

    /*
     Returns the maximum playback offset, in seconds ofthe live broadcast, which is supported for the
     currently rendered (timeshifted) broadcast. If the maximum offset is unknown, the value of this property
     SHALL be undefined
     */
    maxOffset: number;

    /*
     Returns the state of the OITF’s timeshift and recordNowfunctionality for the channel shown in the
     video/broadcastobject. Valid values are:
     •  RECORDING_UNREALIZED
     •  RECORDING_SCHEDULED
     •  RECORDING_REC_PRESTART
     •  RECORDING_REC_ACQUIRING_RESOURCES
     •  RECORDING_REC_STARTED
     •  RECORDING_REC_UPDATED
     •  RECORDING_REC_COMPLETED
     •  RECORDING_TS_ACQUIRING_RESOURCES
     •  RECORDING_TS_STARTED
     •  RECORDING ERROR
     When the currentTimeShiftModeproperty has the value 1, the value of this property is undefined.
     */
    recordingState: number;

    /*
     This function is the DOM 0 event handler for notification of state changes of the recording functionality.
     */
    onRecordingEvent: { (state: number, error: number, recordingId: string): void; };

    /*
     If the value of the currentTimeShiftModeproperty is 1, the current playback position of the media,
     measured in milliseconds from the start of the timeshift buffer.
     */
    playPosition: number;

    /*
     The current play speed of the media.
     */
    playSpeed: number;

    /*
     Returns the ordered list of playback speeds, expressed as values relative to the normal playback
     speed (1.0), at which the currently specified A/V content can be played (as a time-shifted broadcast in
     the video/broadcastobject), or undefinedif the supported playback speeds are not known or the
     video/broadcastobject is not in timeshift mode.
     */
    playSpeeds: number[];

    /*
     The function that is called when the playSpeedsarray values have changed. An application that
     makes use of the playSpeedsarray needs to read the values of the playSpeedsproperty again.
     */
    onPlaySpeedsArrayChanged: { (): void; };

    /*
     The time shift mode indicates the mode of operation for support of timeshift playback in the
     video/broadcast object.
     */
    timeShiftMode: number;

    /*
     When timeshift is in operation the property indicates which resources are currently being used.
     */
    currentTimeShiftMode: number;

    /*
     Starts recording the broadcast currently rendered in the video/broadcastobject. If
     the OITF has buffered the broadcasted content, the recording starts from the current
     playback position in the buffer, otherwise start recording the broadcast stream as soon
     as possible after the recording resources have been acquired. The specified duration
     is used by the OITF to determine the minimum duration of the recording in seconds
     from the current starting point.
     */
    recordNow: (duration: number) => string;

    /*
     Stops the current recording started by recordNow().
     */
    stopRecording: () => void;

    /*
     Pause playback of the broadcast.
     This operation may be asynchronous, and presentation of the video may not pause
     until after this method returns. For this reason, a PlaySpeedChangedevent will be
     generated when the operation has completed,regardless of the success of the
     operation. If the operation fails, the argument of the event SHALL be set to the
     previous play speed.
     */
    pause: () => boolean;

    /*
     Resumes playback of the time-shifted broadcast channel that is currently being
     rendered in the video/broadcast object at the speed specified by setSpeed(). If the
     desired speed was not set via setSpeed(), playback is resumed at normal speed (i.e.
     speed 1.0).
     */
    resume: () => boolean;

    /*
     Sets the playback speed of the time-shifted broadcast to the value speed. If the time-shifted broadcast cannot be played at the desired speed (specified as a value relative
     to the normal playback speed), the playback speed will be set to the best
     approximation of speed. Applications are not required to pause playback of the
     broadcast or take any other action before calling setSpeed().
     */
    setSpeed: (speed: number) => boolean;

    /*
     Sets the playback position of the time-shifted broadcast that is being rendered in the
     video/broadcast object to the position specified by the offset and the reference point as
     specified by one of the constants defined in Section 7.13.2.1. Returns trueif the
     playback position is a valid position to seek to, falseotherwise.
     */
    seek: (offset: number, reference: number) => boolean;

    /*
     Stops rendering in time-shifted mode of the broadcast channel in the
     video/broadcastobject and, if applicable, plays the current broadcast from the live
     point and stops time-shifting the broadcast. The OITF SHALL release all resources
     that were used to support time-shifted rendering of the broadcast. This operation
     SHALL NOT affect recording of a channel if recordNow()was used.
     */
    stopTimeshift: () => boolean;

    /*
     Requests the OITF to switch a (logical or physical) tuner to the specified  channeland
     render the received broadcast content in the area of the browser allocated for the
     video/broadcastobject, as specified by the setChannel(Channel channel,
     Boolean trickPlay, String contentAccessDescriptorURL)method in
     Section 7.13.1.3.
     */
    //setChannel: (channel: Channel, trickplay: boolean, contentAccessDescriptorURL: string, offset: number) => void;

    // 7.13.3 Extensions to video/broadcast for access to EIT p/f

    /*
     The collection of programmes available on the currently tuned channel. This list is a
     ProgrammeCollectionas defined in Section 7.16.3 and is ordered by start time, so index 0 will
     always refer to the present programme (if this information is available).
     */
    programmes: ProgrammeCollection;

    /*
     The function that is called when the programmesproperty has been updated with new programme
     information, e.g. when the current broadcast programme is finished and a new one has started. The
     specified function is called with no arguments.
     */
    onProgrammesChanged: { (): void; };

    // 7.13.4 Extensions to video / broadcast for playback of selected components
    // 7.13.5 Extensions to video/broadcast for parental ratings errors
    // 7.13.6 Extensions to video/broadcast for DRM rights errors

    // 7.13.8 Extensions to video / broadcast for current channel information

    /*
     The channel currently being presented by this embedded object if the user has given permission to
     share this information, possibly through a mechanism outside the scope of this specification. If no
     channel is being presented, or if this information is not visible to the caller, the value of this property
     SHALL be null.
     */
    currentChannel: Channel;
}

/*
 7.13.10 The ChannelConfig class
 The ChannelConfigobject provides the entry point for applications to get information about available channels. It can
 be obtained in two ways:
 •  By calling the method getChannelConfig()of the video/broadcastembedded object as defined in Section
 7.13.1.3.
 •  By calling the method createChannelConfig()of the object factory API as defined in Section 7.1.1.
 The availability of the properties and methods are dependent on the capabilities description as specified in section 9.3.
 The following table provides a list of the capabilities and the associated properties and methods. If the capability is false
 the properties and methods SHALL NOT be available to the application. Properties and methods not listed in the
 following table SHALL be available to all applications as longas the OITF has indicated support for tuner control (i.e.
 <video_broadcast>true</video_broadcast>as defined in Section 9.3.1) in their capability.
 */
interface ChannelConfig extends DOM3EventSource {

    /*
     The list of all available channels. The order of the channels in the list corresponds to the channel
     ordering as managed by the OITF.
     SHALL return the value nullif the channel list is not (partially) managed by the OITF (i.e., if the
     channel list information is managed entirely in the network).
     */
    channelList: ChannelList;

    /*
     A list of favourite lists. SHALL return the value nullif the favourite lists are not (partially) managed by
     the OITF (i.e., if the favourite lists information is managed entirely in the network).
     */
    favouriteLists: FavouriteListCollection;

    /*
     Currently active Favourite channel list object. If currentFavouriteListis undefined, no favourite
     filter list is currently applied.
     The OITF SHALL return the value nullif the favourite lists are not(partially) managed by the OITF
     (i.e. if the favourite lists information is managed entirely in the network).
     */
    currentFavouriteList: FavouriteList;

    /*
     This function is the DOM 0 event handler for events relating to channel scanning. On IP-only receivers,
     setting this property SHALL have no effect.
     */
    onChannelScan: { (type: number, progress: number, frequency: number, signalStrength: number, channelNumber: number, channelType: number, channelCount: number, transponderCount: number): void; };

    /*
     Create a filtered list ofchannels. Returns a subset of ChannelConfig.channelList.
     The blocked, favouriteand hiddenflags indicate whether a channel is included in
     the returned list. These flags correspond to the properties on Channelwith the same
     names. Each flag MAY be set to one of three values:
     Value Meaning
     true  The channel is added if and only if the corresponding property has
     the value true.
     false  The channel is added if and only if the corresponding property has
     the value false.
     undefined  The channel is added regardless ofthe state of the corresponding
     property.
     A channel will only be added to the list if the values of all three flags allow it to be
     added.
     The favouriteListIDattribute is used to select a particular favouriteListthat
     the createFilteredListmethod uses as a basis ofthe filtering process. If
     favouriteListIDis the empty string (i.e. “”), then the filtering is performed on all
     available channels as defined by ChannelConfig.channelList.
     */
    createFilteredList(blocked: boolean, favourite: boolean, hidden: boolean, favouriteListID: string): ChannelList;

    /*
     Start a scan for new channels on all available sources. When each source
     finishes scanning, an UpdateEventSHALL be raised with the type
     CHANNELS_INVALIDATEDand any channel lists for that source SHALL have been
     updated.
     On IP-only receivers, this method SHALL have no effect.
     */
    startScan(options: ChannelScanOptions, scanParameters: ChannelScanParameters): void;
    controlScan(aCommand: string): void;

    /*
     Stop a channel scan, if one is in progress. Any sources that have not finished
     scanning SHALL have their scans aborted and channel line-ups for SHALL NOT be
     changed.
     On IP-only receivers, this method SHALL have no effect.
     */
    stopScan(): void;

    /*
     Creates a ChannelListobject from the specified SD&S Broadcast Discovery Record.
     Channels in the returned channel list will not be included in the channel list that can be
     retrieved via calls to getChannelConfig()
     */
    createChannelList(bdr: string): ChannelList;

    /*
     Creates a Channelobject of the specified idType. The Channelobject can
     subsequently be used by the setChannelmethod to switch a tuner to a channel that is
     not part of the channel list which was conveyed by the OITF to the server. The scope
     of the resulting Channelobject is limited to the Javascript environment (incl.
     video/broadcastobject) to which the Channelobject is returned, i.e. it does not get
     added to the channellist available through method getChannelConfig.
     If the channel of the given idTypecannot be created or the given (combination of)
     arguments are not considered valid or complete, the method SHALL return null.
     If the channel of the given type can be created and arguments are considered valid
     and complete, the method SHALL return a Channelobject whereby at a minimum the
     properties with the same names are given the same value as the given arguments of
     the createChannelObjectmethod. The values specified for the remaining properties
     of the Channelobject are set to undefined.
     */
    createChannelObject(idType: number, onid: number, tsid: number, sid: number, sourceID: number, ipBroadcastID: string): Channel;

    /*
     Create an instance of one of the subclasses of the ChannelScanParametersclass
     (or one of its subclasses). The exact subclass that will be returned SHALL be
     determined by the value of the idTypeparameter.
     Initial values of all properties on the returned object SHALL be undefined.
     */
    createChannelScanParametersObject(idType: number): ChannelScanParameters;
}

/*
 7.13.11 The ChannelList class
 A ChannelListrepresents a collection of Channelobjects. See annex K for the definition of the collection template.
 In addition to the methods and properties defined for generic collections, the ChannelListclass supports the additional
 properties and methods defined below.
 */

interface ChannelList extends Collection {

    /*
     Return the first channel in the list with the specified channel identifier. Returns nullif
     no corresponding channel can be found.
     */
    getChannel(channelID: string): Channel;

    /*
     Return the first (IPTV or non-IPTV) channel in the list that matches the specified DVB
     or ISDB triplet (original network ID, transport stream ID, service ID).
     Where no channels of type ID_ISDB_*or ID_DVB_*are available, or no channel
     identified by this triplet are found, this method SHALL return null.
     */
    getChannelByTriplet(onid: number, tsid: number, sid: number): Channel;

    /*
     Return the first (IPTV or non-IPTV) channel in the list with the specified ATSC source
     ID.
     Where no channels of type ID_ATSC_*are available, or no channel with the specified
     source ID is found in the channel list, this method SHALL return null.
     */
    getChannelBySourceID(sourceID: number): Channel;
}

/*
 7.13.12 The Channel class
 The Channelobject represents a broadcast stream or service.
 Channelobjects typically represent channels stored in the channel list (see 7.13.10). Channelobjects may also
 represent locally defined channels created by an application using the createChannelObject()methods on the
 video/broadcast embedded object or the ChannelConfigclass or the createChannelList()method on the
 ChannelConfigclass. Accessing the channelproperty of a ScheduledRecordingobject or Recordingobject
 which is scheduled on a locally defined channel SHALL return a Channelobject representing that locally defined
 channel.
 Except for the hiddenproperty, writing to the writable properties on a Channel objectSHALL have no effect for
 Channelobjects representing channels stored in the channel list. Applications SHOULD only change these writable
 properties of a locally defined channel before the Channelobject is referenced by another object or passed to an API
 call as an input parameter. The effects of writing to these properties at any other time is implementation dependent.
 */
interface Channel {

    /*
     The type of channel, as indicated by one of the TYPE_*constants defined above.
     */
    channelType: number;

    /*
     The type of identification for the channel, as indicated by one of the ID_*constants defined above.
     */
    idType: number;

    /*
     Unique identifier of a channel within the scope of the OITF. The ccid is defined by the OITF and SHALL
     have prefix ‘ccid: e.g., ‘ccid:{tunerID.}majorChannel{.minorChannel}’.
     Note: the format of this string is platform-dependent.
     */
    ccid: string;

    /*
     Optional unique identifier of the tuner within the scope of the OITF that is able to receive the given
     channel.
     */
    tunerID: string;

    /*
     DVB or ISDB original network ID (for channels of type ID_DVB_*and ID_ISDB_*); can be undefinedif
     stream does not contain an SDT Actual.
     */
    onid: number;

    /*
     DVB or ISDB transport stream ID (for channels of type ID_DVB_*and ID_ISDB_*).
     */
    tsid: number;

    /*
     DVB or ISDB service ID (for channels of type ID_DVB_*and ID_ISDB_*)
     */
    sid: number;

    /*
     ATSC source_ID value.
     */
    sourceID: number;

    /*
     For analogue channels, the frequency of the video carrier in kHz.
     */
    freq: number;

    /*
     For analogue channels, the VPS/PDC confirmed network identifier.
     */
    cni: number;

    /*
     The name of the channel. Can be used for linking analog channels without CNI. Typically, it will contain the
     call sign of the station (e.g. 'HBO'). For channels of type ID_DVB_*the service name is to be used.
     */
    name: string;

    /*
     The major channel number, if assigned. Value undefinedotherwise. Typically used for channels of type
     ID_ATSC_*.
     */
    majorChannel: number;

    /*
     The minor channel number, if assigned. Value undefinedotherwise. Typically used for channels of type
     ID_ATSC_*.
     */
    minorChannel: number;

    /*
     For channels of type ID_DVB_SI_DIRECTcreated through createChannelObject(), this property
     defines the delivery system descriptor (tuning parameters) as defined by DVB-SI [EN 300 468] section
     6.2.13.
     */
    dsd: string;

    /*
     Flag indicating whether the channel is marked as a favourite channel or not in one of the favourite lists as
     defined by the property favIDs.
     */
    favourite: boolean;

    /*
     The names of the favourite lists to which this channel belongs (see the favouriteListsproperty on the
     ChannelConfigclass).
     */
    favIDs: string[];

    /*
     Flag indicating whether the current state of the parental control system prevents the channel from being
     viewed (e.g. a correct parental control pin has not been entered).
     */
    locked: boolean;

    /*
     Flag indicating whether the user has manually blocked viewing of this channel. Manual blocking of a
     channel will treat the channel as if its parental rating value always exceeded the system threshold.
     */
    manualBlock: boolean;

    /*
     If the Channel has idType ID_IPTV_SDS, this element denotes the DVB textual service identifier of the IP
     broadcast service, specified in the format “ServiceName.DomainName” with the ServiceName and
     DomainName as defined in [DVB-IPTV].
     */
    ipBroadcastID: string;

    /*
     The MaxBitRate associated to the channel is returnedthrough this property. The MaxBitRate is provided
     through SD&S as defined in section 3.2.2 of [OIPF_META2]. The property is only related to IP based
     broadcast of type ID_IPTV_SDS.
     If the field does not exist, this method SHALL return undefined.
     */
    channelMaxBitRate: number;

    /*
     The TTR (TimeToRenegotiate) associated to the channel is returned through this property. The MBR is
     provided through SD&S as defined in section 3.2.2 of [OIPF_META2]. The property is only related to IP
     based broadcast of type ID_IPTV_SDS.
     If the field does not exist, this method SHALL return undefined.
     */
    channelTTR: number;

    // 7.13.12.3 Metadata extensions to Channel

    /*
     The long name of the channel. If both short and long names are being transmitted, this property SHALL
     contain the long name of the station (e.g. 'Home Box Office'). If the long name is not available, this
     property SHALL be undefined.
     The value of this property is derived from the Nameelement that is a child of the BCG
     ServiceInformationelement describing the channel, where the lengthattribute of the Name
     element has the value ‘long’.
     */
    longName: string;

    /*
     The description of the channel. If no description is available, this property SHALL be undefined.
     The value of this field is taken from the ServiceDescriptionelement that is a child of the BCG
     ServiceInformationelement describing this channel.
     */
    description: string;

    /*
     Flag indicating whether the receiver is currently authorised to view the channel. This describes the
     conditional access restrictions that may be imposed on the channel, rather than parental control
     restrictions.
     */
    authorised: boolean;

    /*
     A collection of genres that describe the channel.
     This field contains the values of. any ServiceGenreelements that are children of the BCG
     ServiceInformationelement describing the channel.
     */
    genre: string[];

    /*
     Flag indicating whether the channel is included in the default channel list. A value of true means that
     the OITF SHALL exclude this channel from the default channel list.
     */
    hidden: boolean;

    /*
     The URL for the default logo image for this channel.
     The value of this field is derived from the value of the first Logoelement that is a child of the BCG
     ServiceInformationelement describing the channel. If thiselement specifies anything other than
     the URL of an image, the value of this filed SHALL be undefined.
     */
    logoURL: string;

    /*
     Get the value of the field referred to by fieldIdthat is contained in the BCG
     metadata for this channel. If the field doesnot exist, this method SHALL return
     undefined.
     */
    getField(fieldId: string): string;

    /*
     Get the URI for the logo image for this channel. The width and height parameters
     specify the desired width and height of the image; if an image of that size is not
     available, the URI of the logo with the closest available size not exceeding the
     specified dimensions SHALL be returned. If no image matches these criteria, this
     method SHALL return null.
     */
    getLogo(width: number, height: number): string;

}

/*
 7.13.13 The FavouriteListCollection class
 The FavouriteListCollectionclass represents a collection ofFavouriteList objects.
 */

interface FavouriteListCollection extends Collection {

    /*
     Return the first favourite list in the collection with the given favListID.
     */
    getFavouriteList(favID: string): FavouriteList;

    // 7.13.13.2 Extensions to FavouriteListCollection

    /*
     Create a new favourite list and add it tothe collection. The ID of the new favourite list
     SHALL be returned.
     */
    createFavouriteList(name: string): FavouriteList;

    /*
     Remove the list at the specified indexfrom the collection. This method SHALL return
     trueof the operation succeeded, or falseif an invalid index was specified.
     */
    remove(index: number): boolean;

    /*
     Commit any changes to the collection to persistent storage. This method SHALL
     return trueof the operation succeeded, or falseif it failed (e.g. due to insufficient
     space to store the collection).
     If a server has indicated that it requires control of the tuner functionality of an OITF in
     the server capability description for a particular service, then the OITF SHOULD send
     an updated Client Channel Listing to the server using HTTP POST over TLS as
     described in section 4.8.1.1.
     */
    commit(): boolean;

    /*
     Active the favourite list from the collection. This method SHALL return trueif the
     operation succeeded, or falseif an invalid index was specified. A newly created
     favourite list has to be committed before it can be activated.
     */
    activateFavouriteList(favID: string): boolean;

}

/*
 7.13.14 The FavouriteList class
 The FavouriteListclass represents a list of favourite channels. See annex K for the definition of the collection
 template. In addition to the methods and properties defined for generic collections, the FavouriteListclass supports
 the additional properties and methods defined below.
 In order to preserve backwards compatibility with already existing DAE content the ECMAScript toString()method
 SHALL return the FavouriteList.idfor FavouriteListobjects.
 */
interface FavouriteList extends Collection {

    /*
     A unique identifier by which the favourite list can be identified.
     */
    favID: string;

    /*
     A descriptive name given to the favourite list.
     */
    name: string;

    /*
     Return the first channel in the favourite list with the specified channel identifier.
     Returns nullif no corresponding channel can be found.
     */
    getChannel(channelID: string): Channel;

    /*
     Return the first (IPTV or non-IPTV) channel in the list that matches the specified DVB
     or ISDB triplet (original network ID, transport stream ID, service ID).
     Where no channels of type ID_ISDB_*or ID_DVB_*are available, or no channel
     identified by this triplet are found, this method SHALL return null.
     */
    getChannelByTriplet(onid: number, tsid: number, sid: number): Channel;

    /*
     Return the first (IPTV or non-IPTV) channel in the list with the specified ATSC source
     ID.
     Where no channels of type ID_ATSC_*are available, or no channel with the specified
     source ID is found in the channel list, this method SHALL return null.
     */
    getChannelBySourceID(sourceID: number): Channel;

    // 7.13.14.3 Extensions to FavouriteList

    /*
     Insert a new favourite into the favourites list at the specified index. In order to add a
     ccid at the end of the favourite list the index shall be equal to length. This method
     SHALL return trueof the operation succeeded, or falseif an invalid index was
     specified (e.g. index> length).
     */
    insertBefore(index: number, ccid: string): boolean;

    /*
     Remove the item at the specified index from the favourites list. Returns trueof the
     operation succeeded, or falseif an invalid index was specified.
     */
    remove(index: number): boolean;

    /*
     Commit any changes to the favouriteslist to persistent storage. This method SHALL
     return trueof the operation succeeded, or falseif it failed (e.g. due to insufficient
     space to store the list on the OITF).
     If a server has indicated that it requires control of the tuner functionality of an OITF in
     the server capability description for a particular service, then the OITF SHOULD send
     an updated Client Channel Listing to the server using HTTP POST over TLS as
     described in section 4.8.1.1.
     */
    commit(): boolean;
}

// 7.13.16 The ChannelScanOptions class

interface ChannelScanOptions {

    /*
     The types of channel that should be discovered during the scan. Valid values are TYPE_RADIO,
     TYPE_TVor TYPE_OTHERas defined in section 7.13.12.1.
     */
    channelType: number;

    /*
     If true, any existing channels in the channel list managed by the OITF SHALL be removed and the
     new channel list SHALL consist only of channels found during the channel scan operation. If false,
     any channels discovered during the channel scanSHALL be added to the existing channel list.
     */
    replaceExisting: boolean;

}

// 7.13.17 The ChannelScanParameters class

interface ChannelScanParameters {
    addChannelScanParam(aParam: any);
}

// 7.13.18 The DVBTChannelScanParameters class

interface DVBTChannelScanParameters extends ChannelScanParameters {

    /*
     The start frequency of the scan, in kHz.
     */
    startFrequency: number;

    /*
     The end frequency of the scan, in kHz.
     */
    endFrequency: number;

    /*
     The Raster size represented inkHz (typically 7000 or 8000).
     */
    raster: number;

    /*
     The Orthogonal Frequency Division Multiplexing (OFDM) for the indicating frequency.
     */
    ofdm: string;

    /*
     The modulation modes to be scanned.
     */
    modulationModes: number;

    /*
     The expected bandwidth.
     */
    bandwidth: string;

}

// 7.13.19 The DVBSChannelScanParameters class

interface DVBSChannelScanParameters extends ChannelScanParameters {

    /*
     The start frequency of the scan, in kHz.
     */
    startFrequency: number;

    /*
     The end frequency of the scan, in kHz.
     */
    endFrequency: number;

    /*
     The modulation modes to be scanned.
     */
    modulationModes: number;

    /*
     A comma-separated list of the symbol rates to be scanned, in symbols/sec.
     */
    symbolRate: string;

    /*
     The polarisation to be scanned.
     */
    polarisation: number;

    /*
     The code rate, e.g. “3/4” or “5/6”.
     */
    codeRate: string;

    /*
     The orbitalPositionproperty is used to resolve DiSEqC switch/motor. The value is the orbital
     position of the satellite, negative value for west, positive value for east. For example, Astra 19.2 East
     would have orbitalPosition 19.2. Thor 0.8 West would have orbitalPosition -0.8.
     */
    orbitalPosition: number;

    /*
     The network ID of the network to be scanned, or undefinedif all networks should be scanned.
     */
    networkId: number;
}

// 7.13.20 The DVBCChannelScanParameters class

interface DVBCChannelScanParameters extends ChannelScanParameters {

    /*
     The start frequency of the scan, in kHz.
     */
    startFrequency: number;

    /*
     The end frequency of the scan, in kHz.
     */
    endFrequency: number;

    /*
     The Raster size represented inkHz (typically 7000 or 8000).
     */
    Raster: number;

    /*
     The scan mode for scanning. A falsevalue indicates to scan complete range, a truevalue indicates
     scan terminates when a valid NIT is found. The frequency scan is replaced by a scan based on NIT. If
     networkIdis set and the value of this property is set to truethe scan continues until there is a match
     on both.
     */
    startNoetworkScanOnNIT: boolean;

    /*
     The modulation modes to be scanned.
     */
    modulationModes: number;

    /*
     A comma-separated list of the symbol rates to be scanned, in symbols/sec.
     */
    symbolRate: string;

    /*
     The network ID of the network to be scanned, or undefinedif all networks should be scanned.
     */
    networkId: number;

    // 7.13.21 Extensions to video/broadcast for synchronization
    /*
     Add a listener for the specified DSM-CC stream event.
     Event triggers are carried in the streamas MPEG private data sections. For
     robustness, the section describing a particular trigger may be repeated several times.
     Each section has a version number which is used to disambiguate a new trigger for the
     same event (which will have a different version number) from a repeated instance of a
     previous trigger (which will have the same version number).
     When OITF detects a trigger corresponding to an event for which a listener has been
     registered, a DOM StreamEvent SHALL be dispatched.
     An event shall also be dispatched in case of error.
     */
    addStreamEventListener(targetURL: string, eventName: string, listener: Function): void;

    /*
     Remove a streamevent listener for the specified stream event name.
     */
    removeStreamEventListener(eventURL: string, eventName: string, listener: Function): void;
}

// 7.13.21.1 The StreamEvent class

interface StreamEvent {

    /*
     The name of the stream event.
     */
    eventName: string;

    /*
     Data of the DSM-CC StreamEvent’s event encoded inhexadecimal. For example: “0A10B81033” (for a
     message 5 bytes long).
     */
    data: string;

    /*
     Text data of the DSM-CC StreamEvent’s event as a string, assuming UTF-8 as the encoding for the
     DSM-CC StreamEvent’s event. Characters that cannot be transcoded SHALL be skipped.
     */
    text: string;

    /*
     The status of the event. Equal to “trigger” when the event is dispatched in response to a trigger in
     the stream or “error” when an error occurred (e.g. attempting to add a listener for an event that does
     not exist, or when a StreamEvent object with registered listeners is removed from the carousel).
     */
    status: string;

}

// VideoOnDemand

interface VideoOnDemand extends HTMLObjectElement {
    // from DOM3EventSource
    addEventListener(eventName: string, listener: Function, useCapture?: boolean): void;
    removeEventListener(eventName: string, listener: Function): void;

    // 7.14.3 Extensions to A / V Control object for trickmodes

    /*
     The function that is called when the playback speed of the media changes.
     The specified function is called with one argument, speed, which is defined as follows:
     •  Number speed– the playback speed of the media atthe time the event was dispatched.
     The behaviour of the A/V Control object when the end of media (or the end of the currently-available
     media) is reached is defined in Section 7.14.1.
     */
    onPlaySpeedChanged: { (speed: number): void; };

    /*
     The function that is called when change occurs in the play position of the media due to the use of trick
     play functions.
     The specified function is called with one argument, position, which is defined as follows:
     •  position – the playback position of the media at the time the event was dispatched, measured
     in milliseconds since the beginning of the referenced media as denoted by the server.
     The behaviour of the A/V Control object when the end of media (or the end of the currently-available
     media) is reached is defined in section 7.14.1.
     */
    onPlayPositionChanged: { (position: number); void; };

    /*
     Returns an ordered list of playback speeds, expressed as values relative to the normal playback speed
     (1.0), at which the currently specified A/V content can be played (either through an CEA-2014 audio or
     video object), or undefinedif the supported playback speeds are not (yet) known.
     */
    playSpeeds: number[];

    /*
     The function that is called when the playSpeedsarray values have changed. An application that
     makes use of the playSpeedsarray needs to read the values of the playSpeedsproperty again.
     */
    onplaySpeedsArrayChanged: { (): void; };

    /*
     The OITF source IP address for RTSP or HTTP signalling, as well as, the address where the RTSP
     stream is expected to arrive. The information shall be available in “buffering”, “paused” or
     “playing” states.
     */
    oitfSourceIPAddress: string;

    /*
     The OITF Port Address where the RTSP stream isexpected to arrive. The information shall be
     available in “buffering”, “paused” or “playing” states.
     */
    oitfSourcePortAddress: string;

    /*
     When the oitfNoRTSPSessionControlis set to truethen the OITF SHALL NOT signal the RTSP
     messages DESCRIBE, SETUPor TEARDOWN.
     */
    oitfNoRTSPSessionControl: boolean;

    /*
     The sessionId to be used by the A/V Control Object when signalling RTSP. This property is only
     applicable when property oitfNoRTSPSessionControlis set to true.
     */
    oitfRTSPSessionId: string;

    // 7.14.4 Extensions to A/V Control object for playback of selected components

    /*
     This function is called when there is a change inthe set of components being presented. This may
     occur if one of the currently selected components is no longer available and an alternative is chosen
     based on user preferences, or when presentation has changed due to a different component or set of
     components being selected.
     */
    onSelectedComponentChanged: { (componentType: number): void; };

    /*
     If the set of components is known, returns a collection of AVComponentvalues
     representing the components of the specified type in the current stream. If
     componentType is set to null or undefined then all the currently active components are
     returned if the set of active components is known.
     For a video/broadcast object, the set ofcomponents SHALL be known if the
     video/broadcast object is in the presenting state and MAY be known if the object is in
     other states. For an AV Control object, the set of components SHALL be known if the
     AV Control object is in the playing state and MAY be known if the object is in other
     states.
     One or more of the components returned MAY be passed back to one of the other
     methods unchanged (e.g. selectComponent()).
     If property preferredAudioLanguagein the Configurationobject (refer to section
     7.3.2.1) is set then a component is by default selected and is considered as an active
     component.
     If property preferredSubtitleLanguagein the Configurationobject (refer to section
     7.3.2.1) is set and property subtitleEnabledin AVOutputclass (refer to section
     7.3.5.1) is enabled then a component is bydefault selected and is considered as an
     active component.
     */
    getComponents(componentType: number): AVComponentCollection;

    /*
     If the set of components is known, returns a collection of AVComponentvalues
     representing the currently active components of the specified type that are being
     rendered.
     For a video/broadcast object, the set ofcomponents SHALL be known if the
     video/broadcast object is in the presenting state and MAY be known if the object is in
     other states. For an AV Control object, the set of components SHALL be known if the
     AV Control object is in the playing state and MAY be known if the object is in other
     states.
     One or more of the components returned MAY be passed back to one of the other
     methods unchanged (e.g. selectComponent()).
     */
    getCurrentActiveComponents(componentType: number): AVComponentCollection;

    /*
     Select the component that will be subsequently rendered when A/V playback starts or
     select the component for rendering if A/V playback has already started.
     If playback has started, this SHALL replace any other components of the same type
     that are currently playing.
     If property preferredAudioLanguagein the Configurationobject (refer to section
     7.3.2.1) is set then a component is by default selected and it is not necessary to
     perform selectComponent().
     If property preferredSubtitleLanguagein the Configurationobject (refer to section
     7.3.2.1) is set and property subtitleEnabledin AVOutputclass (refer to section
     7.3.5.1) is enabled then a component is by default selected and it is not necessary to
     perform selectComponent().
     */
    selectComponent(component: AVComponent): void;

    /*
     Stop rendering of the specified component of the stream.
     If property preferredAudioLanguagein the Configurationobject (see section 7.3.2.1)
     is set then unselecting a specific componentreturns to the default preferred audio
     language.
     If property preferredSubtitleLanguagein the Configurationobject (see section
     7.3.2.1) is set and property subtitleEnabledin AVOutputclass (see section 7.3.5.1) is
     enabled then unselecting a specific component returns to the default preferred subtitle
     language. In order to stop rendering subtitles completely it is necessary to disable
     subtitles with property subtitleEnabledin AVOutputclass.
     */
    unselectComponent(component: AVComponent): void;

    /*
     If A/V playback has already started,start rendering the default component of the
     specified type in the current stream. ThisSHALL replace any other components of the
     same type that are currently playing.
     If A/V playback has not started, the default component of the specified type will be
     subsequently rendered after calling the setChannelmethod on the
     video/broadcastobject.
     */
    selectComponent(componentType: number): void;

    /*
     If A/V playback has already started, stop rendering of the specified type of component.
     If A/V playback has not started, no components of the specified type will be
     subsequently rendered after calling the setChannelmethod on the
     video/broadcastobject.
     */
    unselectComponent(componentType: number): void;

    // 7.14.5 Extensions to A/V Control object for parental rating errors

    /*
     The function that is called whenever the parental rating of the content being played inside the A/V Control
     object changes.
     These events may occur at the start of a new content item, or during playback of a content item (e.g.
     during playback of linear TV content).
     */
    onParentalRatingChange: { (contentID: string, rating: ParentalRating, DRMSystemID: string, blocked: boolean): void; };

    /*
     The function that is called when a parental rating error occurs during playback of A/V content inside the
     A/V Control object, and is triggered whenever one ormore parental ratings are discovered and none of
     them are valid. A valid parental rating is defined as one which uses a parental rating scheme that is
     supported by the OITF and which has a parental rating value that is supported by the OITF.
     */
    onParentalRatingError: { (contentID: string, ratings: ParentalRatingCollection, DRMSystemID: string): void; };

    // 7.14.6 Extensions to A/V Control object for DRM rights errors

    // 7.14.7 Extensions to A / V Control object for playing media objects

    /*
     Change the content item to be played by the A/V control object to the content item
     represented by id.
     */
    setSource(id: string): string;

    // 7.14.8 Extensions to A/V Control object for UI feedback of buffering A/V content
}


// 7.16.2 The Programme class
interface Programme {

    /*
     The short name of the programme, e.g. 'Star Trek: DS9'.
     */
    name: string;

    /*
     The long name of the programme, e.g. 'Star Trek: DeepSpace Nine'. If the long name is not available,
     this property will be undefined.
     */
    longName: string;

    /*
     The description of the programme, e.g. an episode synopsis. If no descriptionis available, this property
     will be undefined.
     */
    description: string;

    /*
     The long description of the programme. If no description is available, this property will be undefined.
     */
    longDescription: string;

    /*
     The start time of the programme, measured inseconds since midnight (GMT) on 1/1/1970.
     */
    startTime: number;

    /*
     The duration of the programme (in seconds).
     */
    duration: number;

    /*
     The identifier of the channel from which the broadcasted content is to be recorded. Specifies either a
     ccid or ipBroadcastID (as defined by the Channelobject in Section 7.13.12)
     */
    channelID: string;

    /*
     The episode number for the programme if it is part of a series. This property is undefinedwhen the
     programme is not part of a series or the information is not available.
     */
    episode: number;

    /*
     If the programme is part of a series, the total numberof episodes in the series. This property is
     undefinedwhen the programme is not part of a series or the information is not available.
     */
    totalEpisodes: number;

    /*
     The unique identifier of the programme or series, e.g., a TV-Anytime CRID (Content Reference
     Identifier).
     */
    programmeID: string;

    /*
     The type of identification used to reference the programme, as indicated by one of the ID_*constants
     defined above.
     */
    programmeIDType: number;

    /*
     The TV-Anytime Instance Metadata ID for this programme.
     */
    IMI: string;

    /*
     The group CRIDs associated with this programme.
     */
    groupCRIDs: string[];

    /*
     A collection of parental rating values for the programme for zero or more parental rating schemes
     supported by the OITF. For instances of the Programmeclass created by the createProgramme()
     method defined in section 7.10.1.1, the initial value of this property (upon creation of the Programme
     object) is an instance of the ParentalRatingCollection object (as defined in Section 7.9.5) with
     length 0. Parental rating values can be added to thisempty readonly parental rating collection by using
     the addParentalRating()method of the ParentalRatingCollection object. The
     ParentalRatingCollection is defined in Section 7.9.5. The related ParentalRating and
     ParentalRatingScheme objects are defined in Section 7.9.4 and 7.9.2 respectively.
     For instances of the Programmeclass returned through the metadata APIs defined in section 7.12 or
     through the programmesproperty of the video/broadcast object defined in section7.13.3, the initial
     value of this property SHALL include the parental rating value(s) carried in the metadata or DVB-SI
     entry describing the programme, if this information is included.
     Note that if the service provider specifies a certain parental rating (e.g. PG-13) through this property
     and the actual parental rating extracted from the streamsays that the content is rated PG-16, then the
     conflict resolution is implementation dependent.
     */
    parentalRating: ParentalRatingCollection;

    // 7.16.2.3 Metadata extensions to Programme

    /*
     Reference to the broadcast channel where the programme is available.
     The value of this field is derived from the serviceIDrefattribute of the Scheduleelement that refers
     to this programme.
     */
    channel: Channel;

    /*
     Flag indicating whether the programme is blocked due to parental control settings or conditional access
     restrictions.
     */
    blocked: boolean;

    /*
     Flag indicating the type of show (live, first run, rerun, etc,).
     */
    showType: number;

    /*
     Flag indicating whether subtitles or closed-caption information is available.
     This flag SHALL be trueif one or more BCG CaptionLanguageelements are present in this
     programme’s description, falseotherwise.
     */
    subtitles: boolean;

    /*
     Flag indicating whether the programme has high-definition video.
     This flag SHALL be trueif a VerticalSizeelement is present in the programme’s description and
     has a value greater than 576, falseotherwise.
     */
    isHD: boolean;

    /*
     Flag indicating whether the programme has Audio Description.
     */
    isAD: boolean;

    /*
     Bitfield indicating the type of audio thatis available for the programme.
     */
    audioType: number;

    /*
     Flag indicating whether more than one audio language is available for the programme.
     This flag SHALL be trueif more than one BCG Languageelement is present in the programme’s
     description, falseotherwise.
     */
    isMultilingual: boolean;

    /*
     A collection of genres that describe this programme.
     The value of this field is the concatenation of the values of any Nameelements that are children of
     Genreelements in the programme’s description.
     */
    genre: string[];

    /*
     Flag indicating whether the Programmehas a recording associated with it (either scheduled, in
     progress, or completed).
     */
    hasRecording: boolean;

    /*
     Supported audio languages, indicated by iso639 language codes.
     */
    audioLanguages: string[];

    /*
     Supported subtitle languages, indicated by iso639 language codes.
     */
    subtitleLanguages: string[];

    /*
     Flag indicating whether the current state of the parental control system prevents the programme from
     being viewed (e.g. a correct parental control PIN has not been entered to allow the programme to be
     viewed).
     */
    locked: boolean;

    /*
     Get the value of the field referred to by fieldIdthat is contained in the metadata for
     this programme. If the field does not exist, this method SHALL return undefined.
     */
    getField(fieldId: string): string;

    // 7.16.2.4 DVB-SI extensions to Programme

    // 7.16.2.5 Recording extensions to Programme
    /*
     If available, this property represents the recording associated with this programme (either scheduled, in-progress or completed). Has value undefinedif this programme has no scheduled recording
     associated with it.
     */
    recording: ScheduledRecording;
}

/*
 7.16.3 The ProgrammeCollection class
 */
interface ProgrammeCollection extends Collection {
}

/*
 7.16.4 The DiscInfo class
 */

interface DiscInfo {

    /*
     The space (in megabytes) available on the storage device for recordings.
     */
    free: number;

    /*
     The total capacity (in megabytes) of the storage device. Depending upon the system, free MAY be less
     than total even with no recordings as some of the disc space MAY be used for management purposes.
     */
    total: number;

    /*
     The space (in megabytes) reserved for scheduled or ongoing recordings and downloads.
     */
    reserved: number;
}

/*
 7.16.5.2 The AVComponent class
 */

interface AVComponent {

    /*
     The component tag identifies a component. The component tag identifier corresponds to the
     component_tag in the component descriptor in the ES loop of the stream in the PMT [EN 300 468], or
     undefinedif the component is not carried in an MPEG-2 TS .
     */
    componentTag: number;

    /*
     The MPEG Program ID (PID) of the component in the MPEG2-TS in which it is carried, or undefined
     if the component is not carried in an MPEG-2 TS.
     */
    pid: number;

    /*
     Type of the component stream. Valid values for this field are given by the constants listed in section
     7.16.5.1.1.
     */
    type: number;

    /*
     The encoding of the stream. The value of video format or audio format defined in section 3 of
     [OIPF_MEDIA2] SHALL be used.
     */
    encoding: string;

    /*
     Flag indicating whether the component is encrypted or not.
     */
    encrypted: boolean;
}

interface AVVideoComponent extends AVComponent {

    /*
     Indicates the aspect ratio of the video or undefinedif the aspect ratio is not known. Values SHALL be
     equal to width divided by height, rounded to a float value with two decimals, e.g. 1.78 to indicate 16:9
     and 1.33 to indicate 4:3.
     */
    aspectRatio: number;
}

interface AVAudioComponent extends AVComponent {

    /*
     An ISO 639 language code representing the language of the stream.
     */
    language: string;

    /*
     Has value true if the stream contains an audio description intended for people with a visual impairment,
     false otherwise.
     */
    audioDescription: boolean;

    /*
     Indicates the number of channels present in this stream (e.g. 2 for stereo, 5 for 5.1, 7 for 7.1).
     */
    audioChannels: number;
}

interface AVSubtitleComponent extends AVComponent {

    /*
     An ISO 639 language code representing the language of the stream.
     */
    language: string;

    /*
     Has value trueif the stream is intended for the hearing-impaired (e.g. contains a written description of
     the sound effects), falseotherwise.
     */
    hearingImpaired: boolean;

}

interface AVComponentCollection extends Collection {
}

