/**
 * Created by shlee7 on 5/7/14.
 */

/// <reference path="oipf.d.ts" />
interface Window {
    oipfObjectFactory: OipfObjectFactory;
    Netflix : Netflix;
}

interface OipfObjectFactory {
    createHumaxVideoBroadcastObject: () => VideoBroadcast;
    createHumaxVideoOnDemandObject: () => VideoOnDemand;

    createHumaxConfigurationObject: () => HMasterConfiguration;
    createHumaxStorageUtilObject: () => HStorageUtil;
    createHumaxOperatorFreesatObject: () => HOperatorFreesat;
    createContentManagerObject: () => HContentManager;
    createDlnaManagerObject: () => HDlna;
    createWoonManagerObject: () => HWoon;
    createSambaManagerObject: () => HSamba;
    createFtpManagerObject: () => HFtpManager;
    createBluetoothManagerObject: () => HBluetoothManager;
    createDialManagerObject: () => HDial;
    createSatipManagerObject: () => HSatip;
    createAirplayManagerObject: () => HAirplay;

    createHumaxCasUtil: () => HCasUi;
    createHumaxOperatorFeatureObject: () => HMXOperatorFeature;
    createAlexaHelperManagerObject: () => HAlexaHelper;
}
/*
 * Media Local
 */
interface HContentManager {
    onContentBrowse: (contents: HContentCollection, state: number, ticket: number) => void;
    onContentScanSurvey: (state: number, path: string, ticket: number) => void;
    onContentOperate: (ticket: number, result: number) => void;
    onContentMonitor: (state: number, filepath: number) => void;
    reScan: (scanPath: string, bRecursive: boolean, extension: string) => number;
    startAutoScan: (cycleTime: number) => number;
    stopAutoScan: () => boolean;
    extensionMonitor: (extension: string) => boolean;
    autoCopy: (source: string, destination: string, bRecursive: boolean, extension: string) => number;
    copyContent: (source: string, destination: string, bRecursive: boolean, extension: string) => number;
    deleteContent: (source: string, destination: string, extension: string) => number;
    renameContent: (source: string, destination: string, extension: string) => number;
    moveContent: (source: string, destination: string, bRecursive: boolean, extension: string) => number;
    makeDir: (foldername: string) => number;
    removeDir: (foldername: string) => number;
    hasFile: (path: string) => boolean;
    isExist: (path: string) => boolean;
    requestScanNotify: (path: string) => number;
    isScanning: (path: string) => void;
    stopFileOperation: (ticket: number) => number;
    getOperationState: (ticket: number) => number;
    browseContent: (browsePath: string, browseType: number, ReqItemCount: number, fileExt: string, sortType: number, rootPath: string) => number;
    createOperationToken: (ext: string) => any;
}
interface HContent {
    contentType: number;
    fileType: number;
    fullPath: string;
    date: string;
    thumbResolution: string;
    duration: number;
    lastViewTime: number;
    thumbnailUrl: string;
    resolution: string;
    pictureDate: string;
    title: string;
    genre: string;
    album: string;
    artist: string;
    watched: number;
    storageCategory: number;
}
interface HContentCollection extends Collection {
}
/*
 * Media dLNA
 */
interface HDlna {
    dmp: HDlnaDmp;
    dmr: HDlnaDmr;
    dms: HDlnaDms;
}
interface HDlnaDmp {
    onCDSScan: (cds: HDlnaCds, state: number) => void;
    startScan: () => void;
    stopScan: () => void;
    refreshDms: (udn: string) => void;
    getAvailableCdsList: () => HDlnaCdsCollection;
}
interface HDlnaCdsCollection extends Collection {
}
interface HDlnaCds {
    udn: string;
    friendlyName: string;
    ipAddress: string;
    onContentBrowse: (contents: HDlnaCdsContentCollection, state: number, parentID: string) => void;
    browseContent: (cid: string, sort: string, StartIdx: number, RequestItemCount: number) => void;
}
interface HDlnaCdsContentCollection extends Collection {
}
interface HDlnaCdsContent {
    objectID: string;
    parentID: string;
    class: string;
    restricted: boolean;
    objectType: string;
    importURI: string;
    thumbnailUrl: string;
    byteSeekAble: number;
    timeSeekAble: number;
    title: string;
    date: string;
    duration: string;
    resolution: string;
    digitalCopyControl: string;
    genre: string;
    childCount: number;
    album: string;
    artist: string;
}
interface HDlnaDmr {
    activated: boolean;
    started: boolean;
    friendlyName: string;
    onReqPlay: (item: HDlnaCdsContent, playSpeed: number) => void;
    onReqStop: () => void;
    onReqPause: () => void;
    onReqSeek: (position: number) => void;
    onReqSetVolume: (volume: number) => void;
    onReqMute: (mute: boolean) => void;
    start: () => void;
    stop: () => void;
    setMediaChange: (currentURI: string) => void;
}
interface HDlnaDms {
    onEventResult: (state: number) => void;
    started: boolean;
    friendlyName: string;
    start: () => void;
    stop: () => void;
}
/*
 * Media SAMBA
 */
interface HSamba {
    onScanResult: (server: HSambaServer, state: number) => void;
    onMountResult: (server: HSambaServer, state: number) => void;
    startScan: () => void;
    stopScan: () => void;
    mount: (server: HSambaServer) => void;
    unmount: (server: HSambaServer) => void;
    unmountAll: () => void;
}
interface HSambaServer {
    serverName: string;
    sharedName: string;
    ip: string;
    isMounted: boolean;
    log: string;
    mountPath: string;
    id: string;
    password: string;
}

interface HMasterConfiguration {
    configuration: HConfiguration;
    localSystem: HLocalSystem;
}

interface HConfiguration extends Configuration {

    /*
     Get opeartor ID.
     */
    operatorId: number;

    /*
     UI language
     */
    preferredMenuLanguage: string;

    /*

     */
    standbyPowerMode: number;

    automaticStandby: boolean;

    autoDelete: boolean;

    audioBleepEnabled: boolean;

    subtitleFontType: number;

    infoDisplayTimeout: number;

    timeShiftEnabled: boolean;

    skipForwardTime: number;

    instantReplayTime: number;

    firstTimeBoot: boolean;

    channelsetupPin: boolean;

    guidancepolicy: number;

    getField: (fieldName: string) => string;

    setField: (fieldName: string) => void;

    doFactoryDefault: (param?: string) => void;

    doDBRestore: (param1: string, param: number) => number;

    getCamNameBySlot: (param1: number) => string;

    setServerActivationTime: (param) => void;
}

interface HLocalSystem extends LocalSystem {

    pairingDeviceUUID: string;

    priorityOutputs: any;

    hmx_outputs: HOutput[];

    hmx_audios: HAudio[];

    networkManager: any;

    frontPanel: any;

    swupdate: any;

    onUsbWirelessStateChanged: any;

    getField(fieldName: string): string;

    setField(fieldName: string, value: string): void;
    isHbbTVActive: number;

    deleteCookies() : any;
}

interface HOutput {

    name: string;

    type: string;

    enabled: boolean;

    scartEnabled: boolean;

    compositeEnabled: boolean;

    componentEnabled: boolean;

    rfEnabled: boolean;

    hdmiEnabled: boolean;

    spdifEnabled: boolean;

    tvscartEnabled: boolean;

    tvscartFormat: string;

    supportedTvScartFormats: string[];

    vcrscartEnabled: boolean;

    vcrscartFormat: string;

    supportedVcrScartFormats: string[];

    videoStandard: string;

    avAdditionalSignal: string;

    tvAspectRatio: string;

    wss2hdmiMode: string;

    videoDisplayFormat: string;

    curResolution: string;

    //string collection
    supportedResolutions: Collection;

    audioDescriptionEnabled: boolean;

    subtitleEnabled: boolean;

    hardOfHearingEnabled: boolean;
}

interface HAudio {

    name: string;

    type: string;

    enabled: boolean;

    maxVolume: number;

    minVolume: number;

    lipSyncDelay: number;

    soundMode: string;

    transcodingEnabled: boolean;

    modeDigitalAudioOutput: string;

    codecDigitalAudioOutput: string;

    modeTranscoding: string;

    supportedSoundModes: string[];

    supportedDigitalAudioOutputCodecs: string[];

    supportedDigitalAudioOutputModes: string[];

    supportedTranscodingModes: string[];

}

interface HNetworkManager {

    defaultDevIndex: number;

    linkStatus: number;

    getWifiConfig(): HNetworkWiFiConfig;

    getDMSConfig(): HNetworkDMSConfig;

    getFTPConfig(): HNetworkFTPConfig;

    onNetworkStatusChanged: { (networkStatus: number): void; };
}

interface HNetworkWiFiConfig {

    isConnectedAP(): number;

    apScanning(): number;

    setKey(key: string): void;

    applyConnectToAP(ap: HNetworkAccessPoint): number;

    getWaveStrength(): number;

    getConnectedAPInfo(): HNetworkAccessPoint;

    onAccessPointUpdated: { (state: number, ap: HNetworkAccessPoint): void; };
}

interface HNetworkDMSConfig {

    start(): number;

    stop(): number;

    actionStatus(); number;
}

interface HNetworkFTPConfig {

    FTPServerOn(): number;

    FTPServerOff(): number;
}

interface NetworkInterface {
    netmask: string;
    gateway: string;
    dns1st: string;
    dns2nd: string;
    networkType: number;
    dhcpOn: boolean;
    dnsAuto: boolean;
    ipv6Address: string;
    ipv6Prefix: number;
    ipv6Gateway: string;
    ipv6dns1st: string;
    ipv6dns2nd: string;
    ipv6dhcpOn: boolean;
    ipv6dnsAuto: boolean;

    applyConfigure();
    stopConnecting();
    checkGatewayAccess();
    checkDnsAccess();
    checkInternetConnection();

    onLinkStatusChanged: { (linkStatus: number): void; };
    onAccessStatusUpdated: Function;
}

interface HNetworkAccessPoint {

    essid: string;

    secure: number;

    authen: number;

    encrypt: number;

    waveStrength: number;

    connectSpeed: number;


}

interface HRecordingScheduler extends RecordingScheduler {
    recordings: RecordingCollection;
    record(programme: Programme, factoryType?: number): ScheduledRecording;
    recordAt(startTime: number, duration: number, repeatDays: number, channelID: string, factoryType?: number): ScheduledRecording;
    recordConfirm(scheduledRecording: ScheduledRecording, type: number, check: number);
    recordAlternative(scheduledRecording: ScheduledRecording);
    stop(recording: HRecording);
    onPVREvent: { (state: number, recording: HRecording, error: number, conflicts: HRecording[], alternatives: HRecording[]): void; };
    recordCrid(factory: number, crid: string, title: string, channelId: string);
    setDoNotDelete(item, keep);
}
interface Programme {
    subtitleType: number;
    subtitleTypes: string[];
    scheduledRecording: ScheduledRecording;
    recordings: any[];
    mediaDeeplinkID: string;
    mediaDeeplinkUrl: string;
    mediaDeeplinkExpireDate: number;
}
interface HRecording extends Recording {
    thumbnail: string;

    isScheduledAsSeries: boolean;

    willBeDeleted: boolean;

    factoryType: number;

    seriesID: string;

    lastPlayPosition: string;
}
interface ScheduledRecording {
    factoryType: number;
    isScheduledAsSeries: boolean;
    seriesID: string;
    groupCRIDs: string[];
    volume: number;
    programmeCRID: string;
    recommendationCRID: string;
    diseqcInput: number;
    frequency: number;
    polarisation: number;
    symbolRate: number;
    satType: string;
}
/*
 interface HMetadataSearch extends MetadataSearch {

 addConstraint(type: string, param: string): void;
 }
 */

/****************************************************************************
 * Storage utilties
 * @class
 */
interface HStorageUtil extends DOM3EventSource {

    /**
     * Deliever newly attached physical storage
     * @event
     * @param {HMXPhysicalStorage} physicalStorage Newly attached physical storage
     */
        onPhysicalStorageAttached: { (physicalStorage: HPhysicalStorage): void; };

    /**
     * Deliever detached physical storage
     * @event
     * @param {HMXPhysicalStorage} physicalStorage Detached physical storage
     */
        onPhysicalStorageDetached: { (physicalStorage: HPhysicalStorage): void; };

    /**
     * Get physical storage instances
     * @function
     * @returns {HMXPhysicalStorageCollection}
     */
    getPhysicalStorages(): HPhysicalStorage[];

    /**
     * Get supported file system type
     * @function
     * @returns {HMXPhysicalStorageCollection}
     */
    supportedFormatTypes(): string[];
}

interface HPhysicalStorage {

    /*
     * @property {number} id Unique ID for physical storage
     */
    id: number;

    /*
     * @property {number} interface Interface, 0:EInterfaceUnknown, 1:EInterfaceSATA, 2:EInterfaceUSB
     */
    interface: number;

    /*
     * @property {number} type Type, 0:ETypeUnknown, 1:ETypeInternal, 2:ETypeExternal
     */
    type: number;

    /*
     * @property {number} kind Kind, 0:EKindUnknown, 1:EKindHDD, 2:EKindUSBMemory, 3:
     */
    kind: number;

    /*
     * @property {number} usage Usage, 0:EUsageUnkown, 1:EUsagePVR, 2:EUsageStorage
     */
    usage: number;

    /*
     * @property {string} uuid UUID for the physical storage
     */
    uuid: string;

    /*
     * @property {number} availableSize Total space
     */
    availableSize: number;

    /*
     * @property {number} usedSize Total space
     */
    usedSize: number;

    /*
     * @property {number} reservedSize Total space
     */
    reservedSize: number;

    /*
     * @property {number} totalSize Total space
     */
    totalSize: number;

    /**
     * Get logical storage instances
     * @function
     * @returns {HMXLogicalStorageCollection}
     */
    getLogicalStorages(): HLogicalStorage[];

    /**
     * Format all logical storages with the new label
     * @function
     */
    formatAll();

    /**
     * Detach physical storage from STB device
     * @function
     */
    detach();

    /**
     * Check disk and recover if required
     * @function
     */
    recovery();

    pairing: boolean;

    /**
     * Deliver format progress information
     * @event
     * @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
     * @param {number} progress Number indicating format progress
     * @param {number} progressMax Maximum number for format progress
     * @param {string} errMessage Error message only valid if status is 3 (Error)
     */
        onFormatProgress: { (status: number, progress: number, progressMax: number, numbererrMessage: string): void; };

    /**
     * Deliver detach progress information
     * @event
     * @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
     * @param {string} errMessage Error message only valid if status is 3 (Error)
     */
        onDetachProgress: { (status: number, errMessage: string): void; };

    /**
     * Deliver recovery progress information
     * @event
     * @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
     * @param {number} progress Number indicating format progress
     * @param {number} progressMax Maximum number for format progress
     * @param {string} errMessage Error message only valid if status is 3 (Error)
     */
        onRecoveryProgress: { (status: number, progress: number, progressMax: number, errMessage: string): void; };



}

interface HLogicalStorage {
    id: number;
    label: string;
    path: string;
    isPVR: boolean;
    isAvailable: boolean;
}

interface HOperatorFreesat {
    name: string;
    postcode: string;
    bouquetname: string;
    simode: number;
    bouquetid: number;
    regioncode: number;
    adultchannels: boolean;
    //FreesatMetaFileUpdate: Function;
    //FreesatMhegEvent: Function;
    //FreesatMhegAppState: Function;
    //FreesatMetaEpgEvent: Function;
    //listenHomeTp: Function;
    //startMetaDownNow: Function;
    //setDeepLinkForITV: Function;
}

/*
 HCasUi
 */
interface HCasUi {
    openSession: (param1: number, param2: number, param3: number, param4: string) =>  number;
    closeSession: (param1: number, param2: number, param3: number, param4: string) =>  void;
    sendUiResponse: (param1: number, param2: number, param3: number, param4: number, param5: string) => void;
}

/*
 HFtpManager
 */
interface HFtpManager {
    server: HFtpServer;
}
interface HFtpServer {
    serverName: string;
    started: boolean;
    port: number;
    users: any;
    start: () => number;
    stop: () => number;
    setPassword: (param1: string, param2: string, param3: string) => number;
    isValidPassword: (param1: string, param2: string) => boolean;
    setShareFolder: (param1: string, param2: string, param3: string) => number;
    getShareFolder: (param1: string) => any;
    setPublicAccess: (param1: string, param2: boolean) => number;
    getPublicAccess: (param1: string) => boolean;
}

/*
 HWoon
 */
interface HWoon {
    server: HWoonServer;
    client: HWoonClient;
}
interface HWoonServer {
    start: (aParam1: string) => boolean;
    stop: () => boolean;
    testConnection: () => boolean;
    started: boolean;
    friendlyName: string;
}
interface HWoonClient {
    startScan: () => boolean;
    stopScan: () => boolean;
    addFriend: (aParam1: string) => number;
    removeFriend: (aParam1: string) => number;
    getWoonFriendList: () => any;
}

/*
 HBluetoothManager
 */
interface HBluetoothManager {
    enabled: boolean;
    startScan: () => void;
    stopScan: () => void;
    connect: (aParam1: string) => void;
    disconnect: (aParam1: string) => void;
    remove: (aParam1: string) => void;
    getDeviceList: () => any;
    sendPincode: (aParam1: string, aParam2: string) => void;
    getMacAddress: () => void
}

/*
 HDial
 */
interface HDial {
    server: HDialServer;
    client: HDialClient;
}
interface HDialServer {
    start: () => boolean;
    stop: () => boolean;
    registApplication: (aParam1: string) => boolean;
    unregistApplication: (aParam1: string) => boolean;
    updateApplicationState: (aParam1: string, aParam2: number) => boolean;
    started: boolean;
    friendlyName: string;
}
interface HDialClient {
}

/*
 HSatip
 */
interface HSatip {
    server: HSatipServer;
    client: HSatipClient;
}
interface HSatipServer {
    start: () => boolean;
    stop: () => boolean;
    started: boolean;
    friendlyName: string;
}
interface HSatipClient {
}

/*
 HAirplay
 */
interface HAirplay {
    start: () => boolean;
    stop: () => boolean;
    started: boolean;
    activated: boolean;
}

/**************************************
 * oipf.def.ts HUMAX EXTENSION
 **************************************/
interface ApplicationManager extends DOM3EventSource {
    addOIPFApplication: (aParam: string) => boolean;
    removeOIPFApplication: (aParam: string) => boolean;
    widgets: WidgetDescriptorCollection;
    createPortalApplication: () => void;
    destroyPortalApplication: () => void;
    doTeletext: () => void;
    createHbbTVApplication: (aParam: number) => void;
    destroyHbbTVApplication: () => void;
    HBBTV_APP_TYPE_AUTOSTART: number;
    HBBTV_APP_TYPE_INDEPENDENT: number;
    HBBTV_APP_TYPE_DIGITALTELETEXT: number;
    getHumaxApplications: () => any;
    createHumaxApplication:(applicationType: string, url: string) => Application;
    destroyHumaxApplication(command : string) : void;
}
interface LocalSystem extends DOM3EventSource {
    getField: (aParam: string) => string;
    setField: (aParam1: string, aParam2: string) => void;
    networkManager: HMX_NetworkManager;
    antennaInfoLists: AntennaInfoCollection;
    frontPanel: HFrontPanel;
    coldBootDone: HColdBootDone;
    hmx_audios: any;
    hmx_outputs: any;
    chipId: string;
    boxFirstBootStatus: boolean;
    setApplicationReady: () => boolean;
    boxServiceUpdateFlag: boolean;
    boxAntennaPowerStatus: boolean;
    updatedList: string;
    checkFileExistence: (aParam: string) => boolean;
    getStatusOfTVBrowser: () => boolean;
    showMsgbox: () => boolean;
    onUserActionIdle: () => void; // EVENT
    lcnupdate?: any;
}
interface HFrontPanel {
    setString: (aParam1: string, aParam2: number, aParam3: number, aParam4: boolean) => number;
    setLed: (aParam1: number, aParam2: boolean, aParam3: boolean, aParam4: boolean) => number;
    setAnimation: (aParam1: number, aParam2: number, aParam3: number, aParam4: number) => number;
    setCurrentMediaInform: () => number;
}
interface HColdBootDone {
    getColdBootDone: () => boolean;
    releaseColdBootDoneListener: () => void;
}
interface HMX_NetworkManager {
    defaultDevIndex: number;
    linkStatus: number;
    getWifiConfig: (aParam: number) => any;
    getDMSConfig: () => any;
    getFTPConfig:() => any;
    getPPPoEConfig: (aParam?: number) => any;
}
interface AntennaInfo {

}
interface AntennaInfoCollection extends Collection {
    [index: number]: AntennaInfo;
    commit: () => boolean;
    refresh: () => boolean;
    createAntennaInfo: () => number;
    getAntennaInfo: (aIndex: number) => any;
    removeAll: () => boolean;
    getConnectionType: () => number;
    setConnectionType: (aParam: number) => void;
    gotoPosition: (aParam: number) => void;
    storePosition: (aParam: number) => void;
    recalculate: (aParam: number) => void;
    stop: () => void;
    setLimit: (aParam: number) => void;
    enableLimit: () => void;
    disableLimit: () => void;
    gotoDegree: (aParam: number) => void;
    gotoSatellite: (aParam: number) => void;
    drive: (aParam1: number, aParam2: number, aParam3: number) => void;
    getMyLatitude: () => number;
    getMyLongitude: () => number;
    setMyLatitude: (aParam: number) => void;
    setMyLongitude: (aParam: number) => void;
    UNDEFINED: number; // = 0;
    EAST: number; // = 1;
    WEST: number; // = 2;
    STEP: number; // = 1;
    TIMEOUT: number; // = 2;
    CONTINUOUS: number; // = 3;
}
interface ChannelConfig {
    startScan(options: ChannelScanOptions, scanParameters: ChannelScanParameters, aSearchType: string): void;
    createChannelScanParametersObject(): ChannelScanParameters;
    createChannelScanOptionsObject(): any;
}

interface RctLinkInfo {
    linkType: number;
    howRelatedSchemId: number;
    termId: number;
    groupId: number;
    precedence: number;
    mediaUri: string;
    dvbBinLocator: DVBBinLocator;
    proTextNum: number;
    promotionalText: string;
    promotionalTextLangCode: string;
    isDefaultIcon: boolean;
    iconId: number;
    eventName: string;
    eventText: string;
    eventTextLang: string;
}

interface DVBBinLocator {
    identifierType: number;
    scheTimeReliability: number;
    inlineSvc: number
    startDate: string;
    dvbSvcTripletId: string;
    tsId: string;
    onId: string;
    svcId: string;
    startTime: string;
    duration: string;
    evtId: string;
    tvaId: string;
    component: number;
    earlyStartWin: number;
    lateEndWin: number;
}

interface RctImageIconInfo {
    iconId: number;
    transPortMode: number;
    positionFlag: number;
    coordinateSystem: number;
    horizontalOrigin: number;
    verticalOrigin: number;
    iconType: string;
    dataByteSize: number;
    dataByte: string;
    iconUrl: string;
}

interface RctLinkInfoCollection extends Collection {
    item(index: number): RctLinkInfo;
    imageIconInfo: RctImageIconInfo;
    totalLinkCnt: number;
    serviceId: number;
    yearOffset: number;
}

interface VideoBroadcast {
        // FOR UK RCT
    onRctInfoChanged: (aLinkCount, aRctInfo) => void;

    // FOR UK Watch in HD
    onHDLinkageChanged: (aLinkCount, aRctInfo) => void;
    onRequestMhegTune: (aServiceUID: number, aTuneMode: number) => void;
    setChannelWithTuneInfo(aChannel: Channel, aTuneInfo: number): void;
    requestThumbnail(aPath: string, lastPlayPosition): string;
}

interface HMXOperatorFeature {
    getNcdNetworkMsg(aDummy: number);
    setNcdNotifyData(aNeedToShowAgain: number);
}

interface HAlexaHelper {
    processData(data: string);
}

interface Netflix {
    registerStatusChanged : (status: number)=> void;
    startWithTypeUrl : ( type:number, url : string) => void;
    startWithType : (type : number) => void;
    stopWithReason : (reson: number) => void;
    notifyOverlayState : (state : number) => void;
}

declare var Netflix: {
    prototype: Netflix;
    new (): Netflix;
}