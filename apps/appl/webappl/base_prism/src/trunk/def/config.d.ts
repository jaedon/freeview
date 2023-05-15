/// <reference path="common.d.ts"/>
/// <reference path="data.d.ts"/>

/**
 * Created by bglee@humaxdigital.com on 2014. 3. 19..
 */

interface ifCONFIG {
    ux?: string;
    lang?: string;
    newChList?: number;
    antConTpList?: string;           //'searched' or 'hard'; - antenna connection check
    useDetachableStorage?: number;
    useAnimation?: number;
    useFavouriteCollection?: number;
    useHMXVideoBroadcast?: number;
    useIntHDDFormatReboot?: number;
    hxcomm?: boolean; 				// key input duration equalizing
    installwzd?: string;
    region?: number;       		    // default value 2, New South Wales / Australian Capital Territory
    standbyPowerMode?: number;  	    // default value 1, normal number; saving ?: 1
    model?: number;			        // Support Model Function
    op?: number;			            // Support Operator Function
    useCAS?: number; 				// Support CAS Function
    useOTA?: string; 			    // Support OTA TYPE
    useCasOTA?: number;	            // Support CAS OTA TYPE
    useTsrOnOff?: number;			// Support TSR Please change _contextListMap in recording.js
    maxSetAntennaSlot?: number;		// Support Number of antenna slot
    useUserDefineSatellite?: number;	// Support User Define Antenna with Satellite list.
    useImportExportChannels?: number;// Support Import/Export Channel
    maxTuner?: number;				// Support Number of tuner
    useInternalPvr?: number;			// Support Internal PVR
    useHiddenFunction?: number;		// Support Hidden Full Menu
    useHiddenPWReset?: number;		// Support Hidden Menu - Password Reset
    useDefaultChannels?: number;		// Support Default Channel list
    useAlphabetGroup?: number;		// Support Alphabet Group List
    useWakeUpOTA?: number;			// Support wake up OTA
    useLiveIPOTA?: number;           // New SW IP OTA check in live mode
    useAutoPowerDown?: number;		// Support automatic power down
    usefrontPanel?: string;	        // Support used front panel
    useMaxScheduleDuration?: number; // Support Max schedule duration value
    useDelayedRec?: boolean;	    	// Support the Delayed Recording functionality.
    useWaitBooting?: number;   		// Cold Boot Process takes long time, since it update system time and OTA information. - sec
    use2ndPowerOnOffTimer?: boolean; // Support Power on/off timer 2nd scenario
    useBackwardEpg?: boolean;        // Support BackwardEPG.
    maxBackwardEpgDuration?: number; // 2weeks to seconds = 2*7*24*60*60
    useRfCRID?: boolean;             // Support RF-CRID
    usePrismLiteS?: boolean;       // Support Simple Prism lite
    showSeriesInfo?: boolean;		// Support Show Series Info where Detail Info
    durationTSR?: number;            // duration of TSR (minutes)
    isFieldSearch?: boolean;         // Support Search Type
    useOTANotification?: number;		// Support OTA Notification(updated List menu)
    CONFIG_FCC_NUM?: number;         // support Fast Channel Change
    CONFIG_HD_AUDIO? : boolean;  //flac HD Audio
    netflix? : boolean; //support for Netflix UI & functions
    emulOnBox? : boolean; // emul on settop box for rendering test
    recStorageStatusType2? : boolean;
}
interface ifPAGE {
    CPgStart?: string;
    CPgMain?: string;
    CpgVolumeControl?: string;
    // settings
    CPgSettings?: string;
    CPgParental?: string;
    CPgLanguage?: string;
    CPgSettingsVideo?: string;
    CPgAudio?: string;
    CPgScreenDisplay?: string;
    CPgPrivacyPolicy?: string;
    CPgConfigureLAN?: string;
    CPgConfigureWiFi?: string;
    CPgNetworkConnectionStatus?: string;
    CPgNetworkServer?: string;
    CPgFileShare?: string;
    CPgIpEPG?: string;
    CPgRemoteRecord?: string;
    CPgRecording?: string;
    CPgPlaybackOptions?: string;
    CPgStorage?: string;
    CPgExternalStorage?: string;
    CPgInternalStorage?: string;
    CPgSystem?: string;
    CPgPowerTimer?: string;
    CPgPowerManagement?: string;
    CPgHdmiCec?: string;
    CPgFactoryDefault?: string;
    CPgCommonInterface?: string;
    CPgFavoriteChannels?: string;
    CPgEditChannels?: string;
    CPgSignalTest?: string;
    CPgLnbSearch?: string;
    CPgDiseqcSearch?: string;
    CPgScdSearch?: string;
    CPgSatelliteSetting?: string;
    CPgTpSetting?: string;
    CPgSearchStart?: string;
    CPgScdSetting?: string;
    CPgMotorSearch?: string;
    CPgMotorSetSatellite?: string;
    CPgMotorSetSatelPosition?: string;
    CPgSetMotor?: string;
    CPgSetMotorLimit?: string;
    CPgManualSearch?: string;
    CPgAutomaticSearch?: string;
    CPgDynamicChannelUpdate?: string;
    CPgAntennaPower?: string;
    CPgHiddenSetting?: string;
    // messages
    CPgMessage?: string;
    CPgNotifier?: string;
    // Live
    CPgLive?: string;
    CPgHub?: string;
    CPgHubProxy?: string;
    CPgHotkey?: string;
    CPgNotAvailable?: string;
    CPgLiveController?: string;
    CPgChInfo?: string;
    CPgDcn?: string;
    CPgWide?: string;
    CPgInfoHub?: string;
    CPgIPlate?: string;
    CPgDmrProxy?: string;
    // media
    CPgMediaCenter?: string;
    CPgVideo?: string;
    CPgPhoto?: string;
    CPgMusic?: string;
    // This is prism ux's new video list. it will not used in HMS-1000S!!
    CPgPrismUX_Video?: string;
    // TVGuide
    CPgGuide?: string;
    CPgGuide_temp?: string;
    // programme detail
    CPgProgrammeDetail?: string;
    CPgDetailInfo?: string;
    // Recordings
    CPgRecordings?: string;
    CPgRecControl?: string;
    //installation wizard
    CPgInstall01?: string;
    CPgInstall02?: string;
    CPgInstall03?: string;
    CPgInstall04?: string;
    CPgInstall05?: string;
    CPgInstall06?: string;
    CPgInstall06_1?: string;
    CPgInstall07?: string;
    CPgInstall08?: string;
    CPgInstall08_1?: string;
    CPgInstall09?: string;
    // Smart Search
    CPgSmartSearch?: string;
    // Top Picks
    CPgTopPicks?: string;
    // standby
    CPgStandby?: string;
    // advanced
    CPgRecorderRegistration? : string;
    CPgVoiceAssistant? : string;
}
interface ifPageLayout {
    pages_top?: string[];
    pages_center?: { [key: string]: string[] };
    pages_bottom?: string[];
    pages_install_wizard?: string[];
}
interface ifProductSetting {
    config?: ifCONFIG;
    page?: ifPAGE;
    pageLayout?: ifPageLayout;
    pageEntry?: string;
    components?: ifOBJKeyString;
    events?: any;
    keyTable?: string[];
    release?: boolean;
    languageTable?: ifOBJKeyString;
    bottom_page?: string[];
    disableService?: string[];
    settings?: any;
    wzd?: any;
    CPgInfoHub?: any;
    CPgIPlate?: any;
    frontPanel?: any;
    help2?: any;
    help?: any;
}
interface ifProductConfiguration {
    getConfig(): ifCONFIG;
    getPages(): ifPAGE;
    getPageLayout(): ifPageLayout;
    getPageEntry(): string;
    getKeyTable(): string[];
    getEmulator(): string[];
    getEmulationData(): string[];
    getRCU(): string;
    getModeDistribution(): string;
    getUtility(): string[];
    getPageUtility(): string[];
    getExtLibs(aPrefix?: string): string[];
    getPrismModel(): string[];
    getPrismUX(): string[];
    getUnclassifiedData(): any;
    getService(): ifOBJKeyStringArray;
    getComponents(): ifOBJKeyString;
    getEvents(): any;
    isEmul(): boolean;
    getLanguageTable(): ifOBJKeyString;
    //XXX: for built-in apps
    getPrismUIFramework(): string[];
}
//ENV
interface ifENVProductName {
    DEFAULT: number;
    FVP4000T: number;
    HDFOXIRCONNECT: number;
    IR4000HD: number;
    HMS1000S: number;
    HMS1000T: number;
    ICORDPRO: number;
    HD9000i: number;
}
interface ifENVOP {
    MIDDLE_EAST: number;
    AUSDTT: number;
    UKDTT: number;
    SES: number;
    ORF: number;
}
interface ifENVOIPF {
    OIPF: number;
    HUMAX: number;
}
interface ifENVCAS {
    NONE: number;
    IR: number;
    NA: number;
    CI: number;
    IRUC: number;
}
interface ifENVCASOTA {
    NONE: number;
    IRDETO: number;
}
interface ifENV {
    productData: ifDataProductDataInterface;
    op: number;
    model: number;
    cas: number;
    casOTA: number;
    listOP: ifENVOP;
    listModel: ifENVProductName;
    listCAS: ifENVCAS;
    listCasOTA: ifENVCASOTA;
    isModeRelease: boolean;
}
interface Window {
    ENV: ifENV;
}
interface ifRequestBuiltinAppsInfo {
    ansi: string;
    deps: string[];
    url: string[];
}
