/// <reference path="config.d.ts" />
/// <reference path="dateformat.d.ts" />
/// <reference path="jquery.d.ts" />
/// <reference path="misc.d.ts" />
/// <reference path="oipf.humax.d.ts" />
/// <reference path="hx.d.ts" />
/// <reference path="data.d.ts" />
/// <reference path="domain_layer.d.ts" />
/// <reference path="lodash.d.ts" />
/// <reference path="tv_portal.d.ts" />
/// <reference path="q.d.ts" />
/// <reference path="mqtt.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 3. 14..
 */
interface ifSugar {
    start(): void;
    log(cls, msg): void;
    logadd(cls): void;
    logremove(cls): void;
    parse: {
        url: RegExp;
        url_param: RegExp;
        wrap: any;
    };
    parsePageUrl(aURL): any;
    l: ifL;
    msg: any;
    service: any;
    serviceoff: any
    config: any;
    dsvc: any;
    key: any;
    emulMode: boolean;
    il: any;
    dl: any;
    al: any;
    ui: any;
    cmpt: any;
    svc: ifSVC;
    util: ifUtil;
    stbObject: any;
}
interface ifUtil {
    format(aString: string, ...aArgs: any[]): string;
}
interface ifL {
    (aString: string): string;
    load(aString: string, aCallback: Function);
    reload(aString: string, aCallback: Function);
}
interface ifDirectServiceKernel {
    addEventListeners(aParam): void;
    addEventListener(aParam): void;
    removeEventListener(aParam): void;
    getPluginInstance(pluginName): any;
    getPluginInstance(pluginName: "OperatorFeature"): HMXOperatorFeature;
    defineEvents(eventList): void;
    addEventCb(eventName, receiver, handler): void;

    clearEventCb(receiver, eventName?): void;
    fireEvent(...aArgs: any[]): void;
    fireEvent_Legacy(...aArgs: any[]): void;
    saveData(key, param): void;
    readData(key): any;
    require(...aArgs: any[]): void;
}
interface ifSVCKeyEventBinder extends ifDirectServiceKernel {
    bind(aElement: Document, aTarget: any, aUserListener?: (aEvent: KeyboardEvent) => void): void
    unBind(): void;
    reqDeliverMainApp(aKeyCode: number): void;
    destroy(): void;
}
interface ifSVCClock extends ifDirectServiceKernel {
    startClock(): void;
    stopClock(): void;
}
interface ifSVCBridge extends ifDirectServiceKernel {
    requestPlay(aItems: any[], aStartIndex: number): void;
    requestMusicPlayback(aItems: any[], aStartIndex: number): void;
    requestVideoPlayback(aItems: any[], aStartIndex: number, aKeyCode?: number): void;
    requestPhotoPlayback(aItem, aStartIndex: number): void;
    requestInstanceRecording(aChannel): void;
    requestStopPlayback(): void;
    enablePlayback(aEnabled: boolean): void;
    isPlayEnabled(): boolean;
    setTicket(aTicket): void;
    getTicket(): any;
}
interface ifSVCDial extends ifDirectServiceKernel {
    start(): void;
    stop(): void;
    isStarted(): boolean;
    registApplication(aName): void;
    unregistApplication(aName): void;
    updateApplicationState(aName, aState): void;

    onDialStatusEvent(status): void;
    onReqLaunch(name, url?): void;
    onReqStop(name): void;
}
interface ifSVCWebStorage extends ifDirectServiceKernel {
    deleteWholeItems(): void;
    getLastService(param?): any;
    updateLastService(p): void;
    getLastGroup(param?): string;
    updateLastGroup(p): void;
    getLastDeliveryType(): number;
    setPowerTimerLastGroup(param): void;
    getPowerTimerLastGroup(): any;
    getBackService(): any;
    clearBackService(): void;
    updateLastDeliveryType(p): void;
    getFavListID(): any;
    setFavListID(p): void;
    defaultZapInfo(): void;
    updateRecentItems(p): void;
    getRecentItems(p): any;
    getLastTvRadioService(data): any;
    saveAntIdToLocal(param): any;
    getAntIdListFromLocal(): any;
    saveSearchOptionsToLocal(param): void;
    getSearchOptionsFromLocal(): any;
    saveTpIdxToLocal(param): void;
    getTpIdxListFromLocal(): any;
    saveUserTpParamToLocal(param): void;
    saveUserTpParamToLocalT(param): void;
    saveUserTpParamToLocalT2(param): void;
    getUserTpParamFromLocalT(): any;
    getUserTpParamFromLocalT2(): any;
    getUserTpParam1FromLocal(): any;
    getUserTpParam2FromLocal(): any;
    getUserTpParam3FromLocal(): any;
    getUserTpParam4FromLocal(): any;
    getUserTpParam5FromLocal(): any;
    getUserTpParam6FromLocal(): any;
    getUserTpParam7FromLocal(): any;
    getUserTpParam8FromLocal(): any;
    setLastSearchType(param): void;
    getLastSearchType(): any;
    setAutoPowerDown(param): void;
    getAutoPowerDown(): any;
    setPowerOffTimer(param): void;
    getPowerOffTimer(): any;
    setServerActivationTime(param): void;
    getServerActivationTime(): any;
    setTVTVEPGUpdateTimer(param): void;
    getTVTVEPGUpdateTimer(): any;
    getHelp(): any;
    setHelp(p): void;
    getOtaNoti(): any;
    setOtaNoti(p): void;
    getStorageUUID(): any;
    setStorageUUID(p): void;
    isPreInstalledFinished(): boolean;
    setPreInstalledFinished(param): void;
    setVideoOption(param): void;
    getVideoOption(): any;
    setMusicOption(param): void;
    getMusicOption(): any;
    setPhotoOption(param): void;
    getPhotoOption(): any;
    getSambaUserInfo(param): any;
    setSambaUserInfo(param): void;
    getStatistics(): any;
    setStatistics(param): void;
    setFileSharePasswd(param): void;
    getFileSharePasswd(): any;
    getPrivacyPolicy(): any;
    setPrivacyPolicyState(aState): void;
    setPrivacyPolicyText(aText): void;
    setPrivacyPolicyVersion(aVersion): void;
    getTermsAndConditions(): any;
    setTermsAndConditionsState(aState): void;
    setTermsAndConditionsText(aText): void;
    setTermsAndConditionsVersion(aVersion): void;
    readTerms();
    isNeedTerms(): boolean;
    setItem(aKey: string, aData: string): void;
    getItem(aKey: string): any;
    setStatisticsIpEpgDownloadStartEnable(param: any): void;
    getStatisticsIpEpgDownloadStartEnable(): any;
    setStatisticsLastUpdateIpEpgTime(param: any): void;
    getStatisticsLastUpdateIpEpgTime(): any;
    setStatisticsLastSendingTimeOfSettingInfo(param: any): any;
    getStatisticsLastSendingTimeOfSettingInfo(): any;
    setStatisticsLastSendingTimeOfCheckUpload(param: any): void;
    getStatisticsLastSendingTimeOfCheckUpload(): any;
    getHideAppList(param?): any;
    getSortTypeInRecordings(): any;
    setSortTypeInRecordings(aParam): void;
    getSearchHistory(): any;
    setSearchHistory(aList?): void;
    getBackgroundTransparency(): number;
    setBackgroundTransparency(aOpacity): void;
    getAlexaDeviceEnable(): boolean;
    setAlexaDeviceEnable(enable): void;
}
interface ifSVCStorageMass extends ifDirectServiceKernel {
    doDetach(aParam): boolean;
    doCleanUp(aParam): boolean;
    doFormat(aParam): boolean;
    getDetailInfo(aParam): any;
    getPairedStorage(type): any;
    getRecPVRSizeInfo(aParam): any;
    getRecPVRHDD(): boolean;
    getRecPVRList(aSync?): any[];
    getRecStorageList(aPhysicalStorages?): any[];
    getWholeStorageInfo(aParam?): any[];
    getDevice(): any[];
    setStorageName(aParam): boolean;
    doSwitchExternalHdd(aParam: any): void;
}
interface ifSVCSettingUtil extends ifDirectServiceKernel {
    getSystemInfo(): ifSystemInfo;
    getHiddenInfo();
    getNandBadBlocks();
    getBoxFirstBootStatus: () => boolean;
    setBoxFirstBootStatus: (value: boolean) => void;
    setAudioVolume(value);
    getAudioInfo(param);
    setAudioInfo(param);
    getPictureInfo();
    setPictureInfo(data);
    getOutputOption();
    setOutputOption(data);
    getLanguage();
    setLanguage(values);
    controlNetServer(bOnOff);
    IsNetServerWork();
    getPvrEndPadding();
    setPvrEndPadding(value);
    getPvrStartPadding();
    setPvrStartPadding(value);
    getInstantReplayTime();
    setInstantReplayTime(value);
    getSkipForwardTime();
    setSkipForwardTime(value);
    InitializeLanguage(cb);
    getEnableSeriesPlayback();
    setEnableSeriesPlayback(param);
    getEnableIPEPG();
    getIpepgLastUpdateTime();
    getIpepgDownloadProgress();
    getTvtvEpgLastUpdateTime();
    setEnableIPEPG(param);
    getTvtvUpdateTime();
    getIsTvtvStandbyEnable();
    setTvtvUpdateTime(param);
    getEnableRemoteRec();
    setEnableRemoteRec(param);
    getEnableSatIpNotification();
    setEnableSatIpNotification(param);
    getSatRecordUpdateInterval();
    setSatRecordUpdateInterval(param);
    getSatRecordConfirmCode(param);
    getSatRecordPriority();
    setSatRecordPriority(param);
    getUpdatePeriodRemoteRec();
    setUpdatePeriodRemoteRec(param);
    getUpdateIntervalRemoteRec();
    setUpdateIntervalRemoteRec(param);
    doFactoryDefault();
    getFirstTimeBoot();
    offFirstTimeBoot();
    getHdmiCecMode();
    setHdmiCecMode(value);
    getHdmiCecVolumeCtrlMode();
    setHdmiCecVolumeCtrlMode(value);
    setPowerOff();
    setPowerOn();
    standbyState(state);
    setStandbyPowerMode(mode);
    getStandbyPowerMode();
    getPowerOnInfo();
    setPowerOnInfo(param);
    clearPowerOnInfo();
    getNetConnectedInfo();
    stopGetSignalInfo();
    getSignalInfo();
    getSignalConnectionType();
    setInfoDisplayTimeout(param);
    getInfoDisplayTimeout();
    setSubtitleFont(param);
    getSubtitleFont();
    setSubtitleEnabled(aParam: boolean);
    getSubtitleEnabled(): boolean;
    setHardofHearing(aParam: boolean);
    getHardofHearing(): boolean;
    renameServer(param);
    getServerName();
    testConnection();
    setFrontPanelString(param);
    setFrontPanelLed(param);
    getTVPortalUrl();
    getHbbTvUrl();
    getRpUrl();
    getIpOtaUrl();
    getBluetoothStatus();
    setBluetoothStatus(param);
    bluetoothStartScan();
    getBluetoothDevList();
    bluetoothConnect(param);
    bluetoothDisconnect(param);
    bluetoothSendPincode(param);
    bluetoothRemove(param);
    getNetworkAvailable();
    getInternetAvailable();
    setIpAddress(data);
    setNetmask(data);
    setGateway(data);
    setDns1st(data);
    setDns2nd(data);
    getIpAddress();
    getNetmask();
    getGateway();
    getDns1st();
    getDns2nd();
    getallAddress();
    apScanning();
    ConnectionTry();
    stopConnecting();
    stopApScanning();
    getPPPoEId();
    getPPPoEPassword();
    setPPPoEId(param);
    setPPPoEPassword(param);
    connectPPPoE();
    disConnectPPPoE();
    wirelessStopConnecting();
    manualConfig();
    applyConnectToAP(param);
    checkWireConnection();
    checkWirelessConnection();
    getConnectionState();
    doingKeyGrab(param);
    getFrontPanelText();
    getRCUCustomCode();
    getPowerStateValues();
    startPowerOffTimer();
    stopPowerOffTimer();
    startReminderPowerOff();
    stopReminderPowerOff();
    getReminderPowerOff(): boolean;
    doReboot();
    setApplicationReady();
    checkDetachableStorage();
    setServerActivation(param);
    getIrdetoCasInfo();
    getpvrTsrOnOff();
    getDescrambleOnOff();
    setDescrambleOnOff(value);
    getAutoDeleteOnOff();
    setAutoDeleteOnOff(value);
    getNagraCasSn();
    getNagraNuid();
    setpvrTsrOnOff(value);
    getoffsetGMT();
    setoffsetGMT(param);
    getdaylightsaving();
    setdaylightsaving(param);
    getVirtualSCW();
    setVirtualSCW(param);
    getUpdateTime();
    setUpdateTime(param);
    getIRFusinInfo();
    getIRControlByte();
    getRegionId();
    setRegionId(value);
    getCountryId();
    setCountryId(countryId);
    getEnableDynamicUpdate();
    setEnableDynamicUpdate(value);
    getEnableAntennaPower();
    setEnableAntennaPower(value);
    checkFileExistence(uri);
    getPromotionVideo();
    getUpdatedList();
    getEnableSambaServer();
    setEnableSambaServer(value?);
    getEnableFtpServer();
    setEnableFtpServer(param);
    getPasswordSet();
    setPasswordSet(value);
    getEnableDownloadFolder();
    setEnableDownloadFolder(param);
    getColdBootDone(): boolean;
    showMsgbox();
    getNoOtaAction();
    setNoOtaAction(param);
    getNoOtaSWVer();
    setNoOtaSWVer(param);
    getLaterOtaCount();
    setLaterOtaCount(param);
    getPrivacyPolicy();
    setPrivacyPolicy(value);
    getPrivacyPolicyVersion();
    setPrivacyPolicyVersion(value);
    checkGatewayStatus();
    checkDnsStatus();
    checkInternetConnection();
    setGuidancePolicy(aGuidancePolicy: number): void;
    getGuidancePolicy(): number;
    setNCDNotifyData(aNeedToShowAgain: number): void;
    getNCDNetworkMsg(): string;
    checkTvPortalServer(success, fail): void;
    isHbbTVActivated(): boolean;
    getHbbTvEnable(): boolean;
    getPrefixUrl(): string;
    setNtpServer(param): any;
    getIpEpgSvcCount(): any;
    getIpEpgEvtCount(): any;
    getHdmiEdidInfo(): any;
    getNetworkId(): number[];
    deleteCookies() : any;
    getInitHumaxTvPortal(): boolean;
    setInitHumaxTvPortal(aValue: boolean): void;
    disconnectNetworkForTest(): void;
    connectNetworkForTest(): void;
    isMute(): boolean;
}
interface ifSVCDmr extends ifDirectServiceKernel {
    start(): void;
    stop(): void;
    isStarted(): boolean;
    isActivated(): boolean;
    setMedia(param): void;
    getCurrentUri(): string;
}
interface ifSVCChlist extends ifDirectServiceKernel {
    checkChannelChangable(data);
    checkChannelsChangable(channels);
    getChlist(param: any, aIsHiddenChannelOnly?: boolean);
    getCurrentGrpInfo(group);
    getAvailableFirstChannel(param?);
    getAvailableChannelWithGroup(group);
    getAvailableGroupForChannel(data);
    getAvailableGroupForCcid(data);
    getChannelCount(aParam?);
    getWholeChDB(param);
    forceChListUpdate();
    getChannel(param);
    getChannels(param);
    getNextChannel(param);
    getPrevChannel(param);
    getBackChannel(param);
    getCurrentChannel(param?);
    removeWholeChannelList();
    removeChannel(param);
    updateChannel(param);
// Favourite List area...;
    updateFavList(p);
    addToFavList(params): any;
    getFavListName(param);
    removeFromFavList(params);
    renewFavList(params);
    renewPreferList(params);
    moveChListItem(params): any;
    moveChListItemWithTargetIdx(params): any;
    editChListDoCommit(param);
    outFromEditChannels();
    getChannelByTriplet(params);
    getChannelByTripletExceptALL(params);
    getChannelByCcidExceptALL(param);
    removeChannels(params);
    updateChannels(params);
}
interface ifSVCRecordings extends ifDirectServiceKernel {
    isRecordingListLoaded(): boolean;
    refreshRecordingList(): void;
    refreshScheduleList(): void;
    getScheduledList(): any;
    getRecordingList(): any;
    getMatchedIdList(): any;
    getRecording(param): any;
    getCurrentRecordings(): any;
    getSeriesList(seriesID): any;
    arrangeSeriesList(list): any;
    getAllStreamingList(): any;
    getFileOperationInfo(): any;
    getErrorText(recError): string;
    isDelayedRecStarted(): boolean;
    setManualLocked(param): boolean;
    updateScheduledRecording(param): boolean;
    stopRecord(param): void;
    stopRecordingByID(param): void;
    removeScheduledRecording(param): void;
    removeWholeSeriesSchedule(param): void;
    removeRecording(param): void;
    recordConfirm(param): void;
    setDuration(param): boolean;
    record(param): void;
    recordAt(param): void;
    recordAtPowerOnTimer(param): void;
    recordAtReminder(param): void;
    recordAtRecording(param): void;
    recordAtRegularOTA(param): void;
    requestInstanceRecording(param): void;
    requestStopRecording(param?): void;
    setTargetStorage(param): void;
    getTargetStorage(param): any;
    copyRecording(param): void;
    moveRecording(param): void;
    stopFileOperation(param): void;
    createScheduledItem(oipfScheduledObj): any;
    createRecordingItem(oipfRecordingObj): any;
    checkRemainedRequestedRemove(): any;
    getRecordingById(param): any;
    startSatRecord(): void;
    getFileOpState(): string;
    setKeep(param): void;
    getCurrentRecordingByCcid(aCcid: string): any;
}
interface ifSVCEPG extends ifDirectServiceKernel {
    findProgrammes(param): any;
    getProgrammes(param): any;
    retrieveLiveProgrammes(param?): void;
    getLiveProgrammes(p): any;
    executeQuery(param): void;
    _deleteQueue(searchOipfObj): void;
    onMetadataSearch(search, state): void;
    onMetadataUpdate(action, info, object, type): void;
    _doSearch(param): any;
    _wrap(searchTarget, result, param): any;
    _createQuery(search, clause): any;
    _lazyExecute(param, programmes): void;
    getAllProgrammesOrderByTime(view, param, range): any;
}
interface ifSVCAvCtrl extends ifDirectServiceKernel {
    setID(data): void;
    channelChange(param): void;
    stopCurChannel(param): void;
    isLiveOnGoing(): boolean;
    isMpbOnGoing(): boolean;
    blockAV(param): void;
    unBlockAV(): void;
    availableChannel(): boolean;
    lastChannel(): boolean;
    naviChannel(param): boolean;
    getVideoBroadcastCurrentChannel(): any;
    isChannelPlaying(): boolean;
    isIdle(): boolean;
    getAVComponent(): any;
    getAudioComponent(): any;
    getSubtitleComponent(): any;
    setAudioComponent(param): void;
    setSubtitleComponent(param): void;
    offSubtitleComponent(param?): void;
    getCurAudio(): any;
    getCurSubtitle(): any;
    getCurSubtitleonoff(): any;
    getCurAudiodescription(): any;
    toggleAudio(): any;
    toggleAudiodescription(): any;
    toggleSubtitle(): any;
    toggleSubtitleonoff(): any;
    toggleMediaSubtitle(): any;
    getMediaHasSubtitle(): any;
    getMediaSubtitleDisplay(): any;
    getMediaSubtitleLanguage(): any;
    getMediaSubtitleSyncTime(): any;
    getMediaSubtitleFontSize(): any;
    getMediaSubtitleTextPosition(): any;
    getMediaSubtitleSupportLanguages(): any;
    setMediaSubtitleDisplay(data): void;
    setMediaSubtitleLanguage(data): void;
    setMediaSubtitleSyncTime(data): void;
    setMediaSubtitleFontSize(data): void;
    setMediaSubtitleTextPosition(data): void;
    getSupportedSpeedsArray(): any[];
    isLiveStopping() : boolean;
    setRequestPincodeResult(aResult: boolean): void;
    getProgrammePresent(): any;
    getChannelChanging(): boolean;
    setChannelChanging(param: boolean): void;
    bindToChannel():any;
    isMhegIcsOnGoing() : boolean;
    requestThumbnail(param): any;
}
interface ifSVCParentalCtrl extends ifDirectServiceKernel {
    TNaScPinState: {
        ECORRECT;
        EINCORRECT_1st;
        EINCORRECT_2nd;
        EBLOCKED;
    }
    getParentalRatingValue(): any;
    setParentalRatingValue(value): any;
    verifyParentalControlPIN(params): any;
    setParentalControlPIN(data): any;
    setDefaultParentalControlPIN(data): any;
    setParentalControlStatus(data): any;
    getParentalControlStatus(data): any;
    compareTwoPassword(params): any;
    setChannelsetupPin(value): any;
    getChannelsetupPin(): any;
    setReSetParentalControlPIN(data): boolean;
    isNagraSmartCardInserted(): boolean;
    setNargaSmartCardPIN(param): any;
    getNargaSmartCardPINRetryCount(): any;
    getNargaSmartCardPINRemainTime(): any;
    verifyNargaSmartCardPIN(param): any;
}
interface ifSVCCurrentStatus extends ifDirectServiceKernel {
    getCurrentStatus(): number;
    setCurrentStatus(p): boolean;
    isPbGoing(): boolean;
    isRecGoing(): boolean;
    isTsrGoing(): boolean;
    isMedGoing(): boolean;
    isUsrGoing(): boolean;
    isTsrPlayGoing(): boolean;
}
interface ifSVCAppMgr extends ifDirectServiceKernel {
    registerGlobalKeyCapture(aKeyList: number[], aFn: (aKeyCode: number) => void): any;
    createApplication(param): void;
    showApplication(param): void;
    hideApplication(param): void;
    sendMessage(param): void;
    destroyApplication(param): void;
    destroyAllApps(): void;
    destroyAllChildApps(param): void;
    getChildApps(param): ApplicationCollection;
    isAppOpened(param): boolean;
    installWidget(param): void;
    uninstallWidget(param): void;
    getWidgetList(): WidgetDescriptor[];
    getWidgetDescriptor(param): WidgetDescriptor;
    createWidget(param): void;
    destroyWidget(param): void;
    isWidgetInstalled(param): boolean;
    createPortalApp(): void;
    destroyPortalApp(): void;
    createHbbTVApplication(bForced?): void;
    createHbbTvDeeplinkApplication(param): void;
    destroyHbbTvDeeplinkApplication():void;
    destroyMhegApplication():void;
    teletextToggle(): void;
    getHumaxApps(): any[];
    destroyHbbTVApplication():void;
}
interface ifSVCHistory extends ifDirectServiceKernel {
    addChannelWatched(param): void;
    addRecordingPlayed(param): void;
    addRecordingStarted(param): void;
    addAppLaunched(param): void;
    addMediaPlayed(param): void;
    removeHistoryItem(param): void;
    removeHistoryList(param): void;
    getHistoryList(): any;
    getMediaList(): any;
    getPlaybackList(): any;
    getRecordingList(): any;
    getContinueWatchingList(): any;
    getOnNowChannels(): any;
    findMediaItem(param): any;
    reconcileHistory(): void;
    removeHistoryItemByStorage(): void;
    updateHistory2(newChList): void;
    deleteHistory(deleteChList): void;
    updateOnNowChannel(param): void;
}
interface ifSVCPlaylist extends ifDirectServiceKernel {
    add(param): void;
    remove(param): void;
    getPlaylist(contentType): any;
}
interface ifSVCPlayback extends ifDirectServiceKernel {
    mIsBgmOngoing: boolean;
    play(param): any;
    playBGM(param): any;
    makeBGM(): any;
    createBGMList(mediaItemList): void;
    pause(param): boolean;
    resume(): boolean;
    seek(param): boolean;
    stop(): boolean;
    stopBGM(): boolean;
    setData(param): void;
    getData(): any;
    getPosition(): any;
    getDuration(): any;
    getSpeed(): any;
    getThumbnail(param): any;
    getSupportedSpeedsArray(): any;
    getPlayState(): any;
    setSpeed(param): any;
    isMpbOnGoing(): boolean;
    clearTimer(): any;
}
interface ifSVCTsr extends ifDirectServiceKernel {
    play (): any;
    pause (): any;
    resume (): any;
    seek (aParam): any;
    trick (aParam): any;
    forward (aParam): any;
    rewind (aParam): any;
    stop (): any;
    getData (): any;
    getPlaybackOffset (): any;
    getMaxOffset (): any;
    getStartOffset (): any;
    getPlayState (): any;
    getSpeed (): any;
    setSpeed (aParam): void;
    initStartTime (): void;
    getStartTime (): any;
    isTsrOngoing (): any;
    isRecordingOngoing (): any;
    isChaseOngoing (): any;
    isAvailableTrick (): any;
    isTrickEnabled (): any;
    clearTimer (): void;
    setTimer (time?): void;
    getTrickRestrictMode (): any;
    getCurrentTimeShiftMode (): any;
}
interface ifSVCFrontPanel extends ifDirectServiceKernel {
    setFrontPanelText (aText, aAlign?): void;
    clearFrontPanelText (): void;
    setFrontPanelLed (aLedId): void;
    clearFrontPanelLed (): void;
    setFrontPanelLedForPlayback (aLedId): void;
}
interface ifSVCStorageSamba extends ifDirectServiceKernel {
    startScan(): void;
    stopScan(): void;
    unmountAll(): void;
    mount(aServer): any;
    unmount(aServer): any;
}
interface ifSVCStorageDlna extends ifDirectServiceKernel {
    startScan(): void;
    stopScan(): void;
    refreshDms(aParam): void;
    browseContent(aParam): void;
}
interface ifSVCLogicalStorage extends ifDirectServiceKernel {
    updateDevice(param): void;
    unmountAll(param): void;
    updateDeviceStop(param): void;
    updateDlna(data?): void;
    updateSamba(): void;
    userMount(param): void;
    mount(server): void;
    setStorageName(param): any;
}
interface ifSVCSmartSearch extends ifDirectServiceKernel {
    setPlayContent (aData): void;
    getPlayContent (aData): any;
    setKeyword (aKeyword): void;
    getKeyword (): any;
}
interface ifSVCChannelSearch extends ifDirectServiceKernel {
    setSearchOptionParam(param):void;
    getSearchOptionParam(): any;
    StartChannelScan(): void;
    startDiseqcScan(): void;
    getstartDiseqcScanAntennaListCount(): number;
    getstartDiseqcScanAntennaInfo(data): any;
    startAntennaConnection(data): void;
    antennaConnectionComplete(data): void;
    StartTune(data): void;
    StartTuneWithNotTestAntenna(data): void;
    loadChannelSearchResult(): void;
    loadChannelSearchResultWithDefCh(): void;
    saveChannelSearchResult(): void;
    stopScan(): void;
    pauseScan(): void;
    resumeScan(): void;
    updateLcnChannelList(): void;
    changeLcnChannelList();
    stopLcnUpdate(): void;
    saveLcnUpdate(param): void;
    getCurrentLcnListInfo(): void;
    removeCurrentLcnListInfo(): void;
    /////////////////////////*   Channel Scan Control END *////////////////////////////////////;
    ////////////////////////*  Manage Satellite START  *///////////////////////////////////////;
    getSearchTypeFromAnt(): any;
    getAntIdListFromLocal(): void;
    getSatelliteNameById(param): string;
    getAntennaIdByName(name): number;
    setTestSatelliteName(param): void;
    getTestSatelliteName(): any;
    setAntennaInfo(param): number;
    saveAntennaInfo(param): void;
    saveTestAntennaInfo(param): void;
    removeAntennaList(): void;
    removeAnt(): void;
    updateNumSatelSetting(param): void;
    updateNumTpSetting(param): void;
    setSatelliteSetting(param): void;
    getSatelliteSetting(): any;
    getAllSatelliteSetting(): any;
    antennaSetting(): number;
    getConvertSatName(data: any): any;
    ////////////////////////*  Manage Satellite END  *///////////////////////////////////////;
    /////////////////////* Manage Transponder List START *//////////////////////////////////////////;
    getTransponderSelectorAll(data): any;
    getTpUserSelector(): any;
    getUserSettingTestTp(): any;
    getTransponderSelector(): any;
    getTestTransponderSelector(): any;
    setUserlnbFreq(param): void;
    setUserTPParam(param): void;
    setUserTestTPParam(param): void;
    setUserTempTPParam(param): void;
    getUserTPParam(): any;
    setTpIndex(param): void;
    setTestTpIndex(param): void;
    /////////////////////* Manage Transponder List END *//////////////////////////////////////////;
    ///////////////////////////* TODO : Motor ChannelSearch Start *//////////////////////////;
    motorGetMyPosition(): any;
    motorSetMyPosition(data): void;
    setActivationList(data): void;
    getActivationList(): any;
    setMotorLimit(data): void;
    getMotorLimit(): any;
    setMotorMoving(data): void;
    getMotorMoving(): any;
    setMotorSetting(data): void;
    getMotorType(): any;
    motorDrive(data): void;
    motorStop(): void;
    motorGotoPosition(): void;
    motorStorePosition(): void;
    motorSetLimit(): void;
    motorGotoSatellite(): void;
    motorRecalculate(): void;
    ///////////////////////////* Motor ChannelSearch END *//////////////////////////;
    ///////////////////////////*  SCD Channel Search START */////////////////////////////;
    setlastSettingTuner(param): void;
    getlastSettingTuner(): any;
    setScdSetting(param): void;
    getScdSetting(): any;
    ///////////////////////////*  SCD Channel Search END *//////////////////////;
    // ##################################################################################################;
    // Private Metho;
    /////////////////////////// Initialize START /////////////////////////////////;
    _initializeAntData(): void;
    _loadTpData(urlPrefix?): void;
    _loadSatPosData(urlPrefix?): void;
    _remakeNotPreDefinedMotorAntennaPos(): void;
    /////////////////////////// Initialize END! /////////////////////////////////////;
    //////////////////////  Save to WebStorage & get from WebStorage START //////////////////////////;
    _saveAntIdToLocal(curUsingSatelliteIdx): void;
    _saveSearchOptionsToLocal(): void;
    _getSearchOptionsFromLocal(): void;
    _getTpIdxListFromLocal(): void;
    _saveTpIdxToLocal(curSettingTpIdx): void;
    _getTpParamFromLocal(): void;
    _saveTpParamToLocal(curSettingTpIdx): void;
    //////////////////////  Save to WebStorage & get from WebStorage END! //////////////////////////;
    ////////////////////// Channel Scan Control START //////////////////////////////////////////////;
    _startScan(param): void;
    _startAntennaConnection(param): void;
    _startTune(param): boolean;
    _disecScanCompleted(): any;
    _save_disecScanCompletedAntenna(scanData): void;
    _updateChannelList(): void;
    _changeChannelList(): void;
    _saveLcnUpdate(param): void;
    _getLcnUpdateInfo(): void;
    _removeLcnUpdateInfo(): void;
    //////////////////////////// Channel Scan Control END! ////////////////////////////////////////;
    /////////////////////////// Manage Channel Scan Parameter START /////////////////////////////////////;
    _createChannelScanParamCollectionObject(): any;
    _addScanParameters(freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, scanParamCollection): void;
    _settingChannelScanParameters(freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, channelParams): any;
    _createChannelScanOptionsObject(): any;
    _setScanOptionsChannelType(channelType, channelScanOptions): void;
    _setScanOptionsCasType(casType, channelScanOptions): void;
    _setScanOptionsNetwork(network, channelScanOptions): void;
    _getTuningInfo(satName, type): any;
    /////////////////////////// Manage Channel Scan Parameter END! /////////////////////////////////////;
    /////////////////////////// Manage Satellite START ////////////////////////////////////////////;
    _antennaSetting(): any;
    _getAllAntennaInfo(): any;
    _getAntennaInfo(): any;
    _doGetCurrentAntInfo(): any;
    _setAntennaId(id, curUsingSatelliteIdx): void;
    _setTestAntennaId(id): void;
    _setAntennaInfolnbFreq(antInfo, lnbFreq): void;
    _setAntennaInfoantennaType(antInfo, antennaType): void;
    _setAntennaInfoswitchInput(antInfo, switchInput): void;
    _setAntennaInfodiseqcVersion(antInfo, diseqcVersion): void;
    _setAntennaInfoenable22KHzTone(antInfo, enable22KHzTone): void;
    _setAntennaInfoscdBandNo0(antInfo, scdBandNo0): void;
    _setAntennaInfoscdBandNo1(antInfo, scdBandNo1): void;
    _setAntennaInfoscdBandNo2(antInfo, scdBandNo2): void;
    _setAntennaInfoscdBandNo3(antInfo, scdBandNo3): void;
    _setAntennaInfoscdBandFreq0(antInfo, scdBandFreq0): void;
    _setAntennaInfoscdBandFreq1(antInfo, scdBandFreq1): void;
    _setAntennaInfoscdBandFreq2(antInfo, scdBandFreq2): void;
    _setAntennaInfoscdBandFreq3(antInfo, scdBandFreq3): void;
    _setAntennaInfosatName(antInfo, satName): void;
    _setAntennaInfoposition(antInfo, position): void;
    _setAntennaInfolongitude(antInfo, longitude): void;
    _setAntennaInfolnbVoltage(antInfo, lnbVoltage): void;
    _antInfoCommit(antInfo): void;
    _antInfoListsCommit(): void;
    _removeAntennaList(): void;
    _removeAnt(curUsingSatelliteIdx): void;
    _moveDataFromTo(fromSatelliteIdx, toSatelliteIdx): void;
    _calcAntlength(): void;
    _makeAntennaWithDefault(satName): number;
    _createAntennaInfo(): number;
    _getAntennaInfoFromId(antId): any;
    _getAntennaInfoWithId(antId): any;
    _makeAntennaDefault(satName, antInfo): void;
    _setAntennaInfoWithMenu(antInfo, antennaSetJson, satName, userlnbFreq, tuner1Param, tuner2Param, tuner3Param, tuner4Param, motorParam): void;
    _convertAntennaData(antInfo): any;
    _SatNameConverter(name): any;
    _initAllAntennaInfo(): void;
    /////////////////////////// Manage Satellite END! ////////////////////////////////////////////;
    ////////////////////////// Motor Channel Search START! //////////////////////////////////////;
    _motorDisableLimit(): void;
    _motorEnableLimit(): void;
    _motorDrive(param): void;
    _motorStop(): void;
    _motorGotoPosition(param): void;
    _motorStorePosition(param): void;
    _motorSetLimit(param): void;
    _motorGotoSatellite(param): void;
    _motorRecalculate(param): void;
    _motorGetMyPosition(): any;
    _motorSetMyPosition(param): void;
}
interface ifSVCChannelSearchT extends ifDirectServiceKernel {
    StartTerChannelScan (aParam);
    StartTerTune (): void;
    loadChannelSearchResult (): void;
    saveChannelSearchResult (): void;
    stopScan (): void;
    pauseScan (): void;
    resumeScan (): void;
    setUserTPParam (aParam): void;
    setUserTestTPParam (aParam): void;
    setUserTempTPParam (aParam): void;
    getUserTPParam (): any;

    getTrdConflict(aDepth: number, aCallback: (aCount, aList) => void);
    setTrdConflict(aDepth: number, aIndex: number);
    isStartScan(): boolean;
}
interface ifSVCChannelSearchS extends ifDirectServiceKernel {
    setSearchOptionParam (aParam): void;
    getSearchOptionParam (): any;
    StartChannelScan (): void;
    startDiseqcScan (): void;
    getstartDiseqcScanAntennaListCount (): number;
    getstartDiseqcScanAntennaInfo (aParam): any;
    startAntennaConnection (aParam): void;
    antennaConnectionComplete (aParam): void;
    StartTune (aParam): void;
    StartTuneWithNotTestAntenna (aParam): void;
    loadChannelSearchResult (): void;
    loadChannelSearchResultWithDefCh (): void;
    saveChannelSearchResult (): void;
    stopScan (): void;
    pauseScan (): void;
    resumeScan (): void;
    updateLcnChannelList (): void;
    changeLcnChannelList (): void;
    stopLcnUpdate (): void;
    saveLcnUpdate (aParam): void;
    getCurrentLcnListInfo (): void;
    removeCurrentLcnListInfo (): void;
    getSearchTypeFromAnt (): any;
    getAntIdListFromLocal (): void;
    getSatelliteNameById (aParam): string;
    getAntennaIdByName (aParam): any;
    setTestSatelliteName (aParam): void;
    getTestSatelliteName (): any;
    setAntennaInfo (aParam): any;
    saveAntennaInfo (aParam): void;
    saveTestAntennaInfo (aParam): void;
    removeAntennaList (): void;
    removeAnt (): void;
    updateNumSatelSetting (aParam): void;
    updateNumTpSetting (aParam): void;
    setSatelliteSetting (aParam): void;
    getSatelliteSetting (): any;
    getAllSatelliteSetting (): any;
    antennaSetting (): number;
    getTransponderSelectorAll (aParam): any;
    getTpUserSelector (): any;
    getUserSettingTestTp (): any;
    getTransponderSelector (): any;
    getTestTransponderSelector (): any;
    setUserlnbFreq (aParam): void;
    setUserTPParam (aParam): void;
    setUserTestTPParam (aParam): void;
    setUserTempTPParam (aParam): void;
    getUserTPParam (): any;
    setTpIndex (aParam): void;
    setTestTpIndex (aParam): void;
    setlastSettingTuner (aParam): void;
    getlastSettingTuner (): void;
    setScdSetting (aParam): void;
    getScdSetting (): any;
}
interface ifSVCCasCI extends ifDirectServiceKernel {
    getCISlotInfo (): void;
    requestMmi (aParam): void;
    setInput (aData): void;
    closeMmi (aData): void;
    requestMmiforCI (aParam): void;
    requestMmiforCas (aParam): void;
    requestCIBannerSession (aParam): void;
    requestBannerSession (aParam): void;
    getSessionHandle (aParam): any;
    isSCInserted (): any;
    verifySCPincode (aParam): any;
    setSCPincode (aParam): any;
    requestMmiforIrdeto (aParam): void;
}
interface ifSVCCasActionIrEmmForcedOta extends ifDirectServiceKernel {
    setListener (): void;
    releaseListener (): void;
}

interface ifSVCSWUpdate extends ifDirectServiceKernel {
    TSWUSpec : {// MUST be same with DxSwUpdate_Spec_e @ dlib_swupdate.h
        ESPEC_NONE;
        ESPEC_ASTRA;
        ESPEC_SATDVBSSU;
        ESPEC_DTT;
        ESPEC_SKYD;
        ESPEC_APS;
        ESPEC_IRDETO;
        ESPEC_HUMAX;
        ESPEC_FSATSSU;
        ESPEC_UPC;
        ESPEC_KDG;
    };
    TSWUEvent : {// MUST be same with DxSwUpdate_Event_e @ dlib_swupdate.h
        EEventConnectSuccess;
        EEventConnectFail;
        EEventDetectSuccess;
        EEventDetectSuccessInLive;
        EEventDetectFail;
        EEventDownloadSuccess;
        EEventDownloadFail;
        EEventDownloadProgress;
        EEventInstallSuccess;
        EEventInstallFail;
        EEventInstallProgress;
        EEventErorr;
        EEventFinish;
    };
    TSWUStep : {// MUST be same with DxSwUpdate_Step_e @ dlib_swupdate.h
        EStepUnknown;
        EStepConfig;
        EStepConnect;
        EStepDetect;
        EStepDownload;
        EStepInstall;
    };
    TSWUSource : {// MUST be same with DxSwUpdate_Source_e @ dlib_swupdate.h
        ESourceNone;
        ESourceRf;
        ESourceIp;
        ESourceFile;
        ESourceUsb;
    };
    TSatCodeRate : {// MUST be same with DxSat_CodeRate_e @ dlib_tuneparam.h
        EAuto;
        ENone;
        E_1_2;
        E_2_3;
        E_3_4;
        E_5_6;
        E_7_8;
        E_3_5;
        E_4_5;
        E_5_11;
        E_6_7;
        E_8_9;
        E_9_10;
    };
    TSatPolarization : {// MUST be same with DxSat_Polarization_e @ dlib_tuneparam.h
        EAuto;
        EHorizontal;
        EVertical;
        ELeft;
        ERight;
    };
    TSWUType : {
        EUSERSELECT;
        EFORCED;
        ECONDITIONAL;
        EBURSTUPDATE;
    };

    swupdateInit (): void;
    startDetectWithType (aParam): void;
    startDetect (): void;
    startManualDetect (aInfo): void;
    cancelDetect (): void;
    startDownload (aParam): void;
    startSwupdateState (aParam): void;
    cancelDownload (): void;
    restartSystem (): void;
    getVerBootupHumaxOta(): any;
}
interface ifSVCDetailInfo extends ifDirectServiceKernel {
    createProgrammeItem(programmeItem): any;
    createRecordingItem(recordingItem): any;
    createScheduledItem(scheduledItem): any;
    createOnDemandItem(ondemandItem): any;
    createProgrammeScheduledItem(param): any;
    getProgrammeDetailList(param): void;
    sortProgrammeDetailList(list): any;
}
interface ifSVCDCN extends ifDirectServiceKernel {
    create (aParam): any;
    putNumber (aParam): any;
    setchannel (aParam) : boolean;
}
interface ifSVCTVPortal extends ifDirectServiceKernel {
    init (aParam): void;
    login (aParam): void;
    setLanguage (aParam): void;
    isInitialized (): any;
    isLogged (): any;
    getPrivacyPolicyText (aFn, aFnError): any;
    getTermsAndConditionsText (aFn, aFnError): any;
    getProfileList (aParam): void;
    getActiveProfile (aParam): void;
    setActiveProfile (aParam): void;
    getRegistrationCode (aParam): void;
    getAvailableAppList (): any;
    getOttAppList (): any;
    getFeaturedAppList(aParam): any;
    getDefaultAppList (aParam): any;
    getPreInstalledAppList (): any;
    getInstalledAppList (aParam): void;
    getApplicationUrl (aParam): void;
    deleteApplication (aParam): void;
    updateApplicationList (aParam): void;
    acceptPrivacyPolicy (aParam): void;
    acceptTermsAndConditions (aParam): void;
    acceptPrivacyTerms (aParam): void;
    checkPrivacyVersion (aFn?, aFnError?): void;
    getSmartSearchAppList (aParam): any;
    setStatisticsLog (aParam): any;
    findProgrammes (aParam): void;
    getLastErrorCode (): any;
    launchApp (param): void;
    destroyActiveApp (): void;
    isPrivacyPolicyAccepted (): any;
    isTermsAndConditionsAccepted (): any;
    putCrashLogData (): any;
    checkNewSoftware (aParam): void;
    getCurrentRegionInfo(param): void;
    getExternalIp(): any;
    getCheckUploadStatistics(param): void;
    putExceptionLogData(param): void;
    getAuthTicket(): any;
    getPortalUrl(): any;
    deviceReset(param): void;
    enableHumaxServerForTest(): void;
    disableHumaxServerForTest(): void;
    getRecommendations(aParam): void;
}
interface ifSVCDlna extends ifDirectServiceKernel {
    startScan(): void;
    stopScan(): void;
    browseContent(param): void;
    getState(): any;
    startKeywordSearch(param): void;
    stopKeywordSearch(): void;
    getCdsList(): any;
}
interface ifSVCAirPlay extends ifDirectServiceKernel {
    start(param): void;
    stop(param): void;
    setDuration(param): void;
    setPosition(param): void;
    setRate(param): void;
    setPlaybackStatus(param): void;
}
interface ifSVCSamba extends ifDirectServiceKernel {
    startScan(param?): void;
    stopScan(param?): void;
    mount(param): void;
    unmount(param): void;
}
interface ifSVCStorageUtil extends ifDirectServiceKernel {
    getStorages (): any;
    detach (aParam): void;
    rename (aParam): void;
}
interface ifSVCMediaServerManager extends ifDirectServiceKernel {
    startScan(): void;
    stopScan(): void;
    getAvailableServerList(param): any;
    getAvailableLocalServerList(param): any;
    getAvailableNetworkServerList(param): any;
    isStarted(): boolean;
}
interface ifSVCMediaItemManager extends ifDirectServiceKernel {
    autoCopy(aParam): any;
    copyContent(aParam): any;
    deleteContent(aParam): any;
    makeDir(aParam): any;
    hasFile(aParam): boolean;
    isExist(aParam): boolean;
    stopFileOperation(aParam): any;
    getOperationState(aParam): any;
    getOperationFileList(aParam): any;
    browseContent(path, type, count, ext, sort, rootPath, cb?): any;
    reScan(aParam): any;
    searchContent(aParam): any;
    getState(): any;
    requestScanNotify(aParam): void;
    mount(aParam): void;
    findMediaItems(aParam, aPath?, cb?): void;
    recoveryOperations(): void;
    isFileOp(): boolean;
}
interface ifSVCStatistics extends ifDirectServiceKernel {
    statisticsLog(param: {
        CODE: string;
        TRS: string;
        KEY?: number;
        OSD?: string;
        ONID?: string;
        TSID?: string;
        SID?: string;
        EID?: string;
    }): any;
    startLog(): any;
    setStatisticsMode(param: boolean): any;
    setStatisticsPolicy(param: {
        CODE: string;
        TRS: string;
        OSD?: string;
        ONID?: string;
        TSID?: string;
        SID?: string;
        EID?: string;
    }): any;
    transmitLog(): any;
    getLog(): any;
    getSysInfo(): any;
    getUrl(): string;
}
interface ifSVCChannelImportExport extends ifDirectServiceKernel {
    TErrorType: {// refer DxImportExportErrorType_e @ xmgr_setting_import_export_mesat.c
        EErrorNone;
        EErrorFailure;
        EErrorNoUsb;
        EErrorNoFile;
    };
    startChannelExport(): void;
    startChannelImport(): void;
}
interface ifSVCClockRecovery extends ifDirectServiceKernel {
    startClockRecovery(): void;
}

interface ifSVCNetflix extends ifDirectServiceKernel {
    start(param : any) : void;
    stop(reasonCode : Number) : void;
    notifyOverlayState(state : Number) : void;
    isRunning() : boolean;
    setPwStatus(bOn : boolean) : void;
    getPwStatus() : boolean;
}

interface ifSVCVoiceAssistance extends ifDirectServiceKernel {
    createMQTTClient(options) : void;
    removeMQTTClient() : void;
    connectMQTTClient(options) : void;
    disconnectMQTTClient() : void;
    getMQTTOptions(type?) : any;
    publish(responseValue?): void;
    processData(param?): void;
    isSubscribed() : boolean;
    getStatus() : string;
}

interface ifSVC {
    CLOCK: ifSVCClock;
    KEY_EVENT_BINDER: ifSVCKeyEventBinder;
    AIR_PLAY: ifSVCAirPlay;
    APP_MGR: ifSVCAppMgr;
    AV_CTRL: ifSVCAvCtrl;
    BRIDGE: ifSVCBridge;
    CAS_CI: ifSVCCasCI;
    CAS_IR_EMM_FORCED_OTA: ifSVCCasActionIrEmmForcedOta;
    CHANNEL_SEARCH: ifSVCChannelSearch;
    CHANNEL_SEARCH_T: ifSVCChannelSearchT;
    CHANNEL_SEARCH_S: ifSVCChannelSearchS;
    CH_LIST: ifSVCChlist;
    DCN: ifSVCDCN;
    DETAIL_INFO: ifSVCDetailInfo;
    DIAL: ifSVCDial;
    DLNA: ifSVCDlna;
    DMR: ifSVCDmr;
    EPG: ifSVCEPG;
    FRONT_PANEL: ifSVCFrontPanel;
    HISTORY: ifSVCHistory;
    LOGICAL_STORAGE: ifSVCLogicalStorage;
    MEDIA_ITEM_MANAGER: ifSVCMediaItemManager;
    MEDIA_SERVER_MANAGER: ifSVCMediaServerManager;
    PARENTAL_CTRL: ifSVCParentalCtrl;
    PLAYBACK: ifSVCPlayback;
    PLAYLIST: ifSVCPlaylist;
    RECORDINGS: ifSVCRecordings;
    SAMBA: ifSVCSamba;
    SETTING_UTIL: ifSVCSettingUtil;
    SMART_SEARCH: ifSVCSmartSearch;
    STATISTICS: ifSVCStatistics;
    STORAGE_DLNA: ifSVCStorageDlna;
    STORAGE_MASS: ifSVCStorageMass;
    STORAGE_SAMBA: ifSVCStorageSamba;
    STORAGE_UTIL: ifSVCStorageUtil;
    SW_UPDATE: ifSVCSWUpdate;
    TSR: ifSVCTsr;
    TV_PORTAL: ifSVCTVPortal;
    WEB_STORAGE: ifSVCWebStorage;
    CHANNEL_IMPORT_EXPORT: ifSVCChannelImportExport;
    CLOCK_RECOVERY: ifSVCClockRecovery;
    IP_EPG;
    NETFLIX : ifSVCNetflix;
    VOICE_ASSISTANCE: ifSVCVoiceAssistance;

    addService(aServiceName: string, aConstructor: any, aForceReload?: boolean): void;
    isMade(aServiceName: string): boolean;
}
interface ifCAppRpcUtil {
    _call(aArguments): void;
    _notify(fname, fnCallback): void;
    show(): void;
    hide(): void;
    notifyHidden(fnHidden): void;
    notifyShown(fnShown): void;
}
//jQuery extended
interface JQueryStatic {
    hxGetScript(aURL: string, aCallback: Function, aDomain: string): void;
    hxGetScript(aURLs: string[], aCallback: Function, aDomain: string): void;

    hxGetMultiScript(aURL: string, aCallback: Function, aDomain: string): void;
    hxGetMultiScript(aURLs: string[], aCallback: Function, aDomain: string): void;

    hxUpgrade(aDestination: any, aSource: {}): void;
    hxLogErr(aLog: string): void;
    isObject(aObject): boolean;
    imgPreload(aParam: any);

    (selector: any, context?: any): JQuery; // TODO FIX TYPE  (cKeynavigator build)
    getUrlParams(aURL): any;
}
interface JQuery {
    hxTrigger(aDest, aData?): void;
    hxBind(aName: string, aCallback: Function): void;
    hxUnbind(aName: string, aHandler: Function): void;

    hxKeep(aName: string, aCallback: Function): void;
    hxRun(aName: string, aData: any): void;
    ani(aPram: any) : number;
    anim(aArg: any, aCallback?): void;

    afterTransitionOneShot(aHandler: (aArg) => void, aWithEvent?: boolean): void;
}
interface Window {
    hx: ifSugar;
}
interface Date {
    addTime(aTime): void;
    toSeconds(): number;
    toDays(): number;
    dateAfter(): Date;
}
//external library
interface Window {
    crossBrowser_initKeyboardEvent(aEventName: string, aArgs?: any): KeyboardEvent;
}
declare var TAFFY: any;
declare var ENV: ifENV;
