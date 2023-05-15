/// <reference path="../def/require.d.ts" />
/// <reference path="../def/config.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 3. 19..
 */
import __DEF__ = require("def/config.def");
var _DEFAULT_CONFIG: ifCONFIG = {
    ux: "PRISM",
    lang: 'eng',
    newChList: 1,
    antConTpList: 'searched',
    useDetachableStorage: 1,
    useAnimation: 1,
    useFavouriteCollection: 1,
    useHMXVideoBroadcast: 1,
    useIntHDDFormatReboot: 1,
    hxcomm: true,
    installwzd: 'CPgInstall02',
    region: 2,
    standbyPowerMode: 1,
    model: __DEF__.eProductName.FVP4000T,
    op: __DEF__.eOP.UKDTT,
    useCAS: __DEF__.eCAS.NONE,
    useOTA: 'DVBSSU',
    useCasOTA: __DEF__.eCASOTA.NONE,
    useTsrOnOff: 0,
    maxSetAntennaSlot: 4,
    useUserDefineSatellite: 1,
    useImportExportChannels: 0,
    maxTuner: 2,
    useInternalPvr: 1,
    useHiddenFunction: 1,
    useHiddenPWReset: 0,
    useDefaultChannels: 0,
    useAlphabetGroup: 0,
    useWakeUpOTA: 0,
    useLiveIPOTA: 0,
    useAutoPowerDown: 1,
    usefrontPanel: false,//'lcdVfd16',
    useMaxScheduleDuration: 1440,
    useDelayedRec: true,
    useWaitBooting: 20,
    use2ndPowerOnOffTimer: true,
    useBackwardEpg: true,
    useTVAppsV3: false,
    apiReleaseForTvAppsV3: false,
    maxBackwardEpgDuration: 1209600,
    useRfCRID: true,
//    usePrismLiteS: false,
    showSeriesInfo: false,
    durationTSR: 7200,
    isFieldSearch: false,
    useOTANotification: 0,
    CONFIG_FCC_NUM: 0,
    useSlideText : true,        // Support Slide Text Animation in infoHub and EPG
    useUSBWireless : true,		// Support USB Wireless dongle
    tvGuide2ndScenario : true,
    useTVStreaming : true,
    CONFIG_HD_AUDIO : true, // support HD Audio - flac
    netflix : true,
    useNetflixPW : true,
    emulOnBox : false,
    testWifiOnDebug : false,
    recStorageStatusType2: true,
    useDLNAonSearch : false,
    useHelpPage : false
};
var _DEFAULT_PAGE: ifOBJKeyString = {
    CPgStart: 'pages/start/page_start.js',
    CPgMain: 'pages/main/page_main.js',
    CPgSettings: 'pages/settings/page_settings.js',
    CPgParental: 'pages/settings/general_settings/page_parental_control.js',
    CPgLanguage: 'pages/settings/general_settings/page_language.js',
    CPgSettingsVideo: 'pages/settings/general_settings/page_video.js',
    CPgAudio: 'pages/settings/general_settings/page_audio.js',
    CPgScreenDisplay: 'pages/settings/general_settings/page_screen_display.js',
    CPgPrivacyPolicy: 'pages/settings/general_settings/page_new_privacy_policy.js',
    CPgConfigureLAN: 'pages/settings/network_settings/page_configure_lan.js',
    CPgConfigureWiFi: 'pages/settings/network_settings/page_configure_wifi.js',
    CPgNetworkConnectionStatus: 'pages/settings/network_settings/page_network_connection_status.js',
    CPgNetworkServer: 'pages/settings/network_settings/page_network_server.js',
    CPgFileShare: 'pages/settings/network_settings/page_fileshare.js',
    CPgRemoteRecord: 'pages/settings/pvr_settings/page_remote_record.js',
    CPgRecording: 'pages/settings/pvr_settings/page_recording.js',
    CPgPlaybackOptions: 'pages/settings/pvr_settings/page_playback_options.js',
    CPgStorage: 'pages/settings/pvr_settings/page_storage.js',
    CPgExternalStorage: 'pages/settings/pvr_settings/page_external_storage.js',
    CPgInternalStorage: 'pages/settings/pvr_settings/page_internal_storage.js',
    CPgSystem: 'pages/settings/system_settings/page_system_information.js',
    CPgPowerTimer: 'pages/settings/system_settings/page_power_timer.js',
    CPgPowerManagement: 'pages/settings/system_settings/page_power_management.js',
    CPgHdmiCec: 'pages/settings/system_settings/page_hdmi_cec.js',
    CPgFactoryDefault: 'pages/settings/system_settings/page_factory_default.js',
    CPgFavoriteChannels: 'pages/settings/channel/page_fav_channels.js',
    CPgEditChannels: 'pages/settings/channel/page_edit_channels.js',
    CPgSignalTest: 'pages/settings/channel/page_signal_test.js',
    CPgSearchStart: 'pages/settings/channel/page_searchstart.js',
    CPgManualSearch: 'pages/settings/channel/page_manualchannelsearcht.js',
    CPgAutomaticSearch: 'pages/settings/channel/page_automaticchannelsearcht.js',
    CPgMessage: 'pages/msgbox/page_messagebox.js',
    CPgNotifier: 'pages/msgbox/page_notifier.js',
    CPgLive: 'pages/live/page_live.js',
    CPgHub: 'pages/live/page_hub.js',
    CPgHubProxy: 'pages/live/page_proxyhomemenu.js',
    CPgHotkey: 'pages/live/page_hotkey.js',
    CPgNotAvailable: 'pages/live/page_notavailable.js',
    CPgLiveController: 'pages/live/page_livecontroller.js',
    CPgChInfo: 'pages/live/page_chinfo.js',
    CPgDcn: 'pages/live/page_dcn.js',
    CPgWide: 'pages/live/page_wide.js',
    CPgInfoHub: 'pages/live/page_infohub2.js',
    CPgIPlate: 'pages/live/page_iplate.js',
    CPgTopPicks: 'pages/toppicks/page_toppicks.js',
    CPgMedia: 'pages/media/media.js',
    CPgMediaCenter: 'pages/media/page_mediacenter.js',
    CPgVideo: 'pages/media/page_video2.js',
    CPgPhoto: 'pages/media/page_photo2.js',
    CPgMusic: 'pages/media/page_music2.js',
    CPgGuide: 'pages/tvguide/page_pgguide.js',
    CPgDetailInfo: 'pages/prdetail/page_cpgprogrammedetail.js',
    CPgRecordings: 'pages/recordings/page_reclist.js',
    CPgRecControl: 'pages/recordings/page_reccontrol.js',
    CPgInstall02: 'pages/install/page02_start.js',
    CPgInstall03: 'pages/install/page03_select_lang.js',
    CPgInstall04: 'pages/install/page04_start_net.js',
    CPgInstall05: 'pages/install/page05_wifi.js',
    CPgInstall06: 'pages/install/page06_net_ip_result.js',
    CPgInstall06_1: 'pages/install/page06_1_new_privacy_policy.js',
    CPgInstall07: 'pages/install/page07_start_search.js',
    CPgInstall08: 'pages/install/page08_search_action.js',
    CPgInstall08_1: 'pages/install/page08_1_quickboot.js',
    //CPgInstall09: 'pages/install/page09_result.js',
    CPgInstall09: 'pages/install/page09_result_ip_ota.js',
    CPgSmartSearch: 'pages/smartsearch/page_cpgsmartsearch2.js',
    CPgStandby: 'pages/standby/page_standby.js',
    //CPgLiveTVStreaming: 'pages/settings/network_settings/live_tv_streaming.js'
    CPgRecorderRegistration: 'pages/settings/advanced_settings/page_recorder_registration.js',
    CPgVoiceAssistant: 'pages/settings/advanced_settings/page_voice_assistant.js'
};
var _PAGE_TOP_: string[] = ['CPgMain', 'CPgMessage', 'CPgHubProxy'];
var _PAGE_CENTER_: ifOBJKeyStringArray = {
    CPgLiveController: ['CPgInfoHub', 'CPgIPlate', 'CPgDcn', 'CPgWide', 'CPgChInfo', 'CPgDetailInfo', 'CPgSmartSearch'],
    CPgSettings: ['CPgDetailInfo'],
    CPgGuide: ['CPgDetailInfo', 'CPgSmartSearch', 'CPgWide'],
    CPgRecordings: ['CPgDetailInfo', 'CPgSmartSearch', 'CPgWide'],
    CPgMediaCenter: ['CPgSmartSearch', 'CPgDetailInfo'],
    CPgVideo: ['CPgSmartSearch', 'CPgDetailInfo', 'CPgWide'],
    CPgPhoto: ['CPgSmartSearch', 'CPgDetailInfo'],
    CPgMusic: ['CPgSmartSearch', 'CPgDetailInfo'],
    CPgPhotoPlayback: ['CPgSmartSearch', 'CPgDetailInfo'],
    CPgMusicPlayback: ['CPgSmartSearch', 'CPgDetailInfo'],
    CPgOnDemand: ['CPgDetailInfo'],
    CPgTopPicks: ['CPgDetailInfo'],
    CPgStandby: []
    // others ...
};
var _PAGE_BOTTOM_: string[] = [/*'CPgDmrProxy',*/ 'CPgNotAvailable', 'CPgHotkey', 'CPgLive', /*'CPgPlayback',*/ 'CPgNotifier', 'CPgRecControl'];
var _PAGE_INSTALL_WIZARD_: string[] = ['CPgMessage', 'CPgHotkey'];
var _PAGE_ENTRY_: string = 'CPgStart';
var _EMULATOR_: string[] = ['emul/emul.js',
    'emul/humax.ext/emul.js',
    'emul/emulUtil.js',
    'emul/humax.ext/emulUtil.js'
];
var _EMULATION_DATA_: string[] = [
    //'emul/humax.ext/json/service.hmx.js',
    //'emul/humax.ext/json/service.freesat.js',
    'emul/humax.ext/json/service.freeview.js',
    'emul/humax.ext/json/antinfo.js',
//    TV eventlist datas
    'emul/humax.ext/json/event_07_238.js',
    'emul/humax.ext/json/event_08_240.js',
    'emul/humax.ext/json/event_09_237.js',
    'emul/humax.ext/json/event_10_220.js',
    'emul/humax.ext/json/event_11_284.js',
    'emul/humax.ext/json/event_12_243.js',
    'emul/humax.ext/json/event_13_261.js',
    'emul/humax.ext/json/event_14_274.js',
    'emul/humax.ext/json/event_15_273.js',
    'emul/humax.ext/json/event_16_270.js',
    'emul/humax.ext/json/event_17_234.js',
    'emul/humax.ext/json/event_18_275.js',
//     Radio eventlist datas
    'emul/humax.ext/json/event_35_206.js',
    'emul/humax.ext/json/event_36_207.js',
    'emul/humax.ext/json/event_37_202.js',
    'emul/humax.ext/json/event_38_198.js',
    'emul/humax.ext/json/schedulelist.js',
    'emul/humax.ext/json/reclist.js',
    'emul/humax.ext/json/mediaVideoList.js',
    'emul/humax.ext/json/casuidata.js',
    'emul/humax.ext/json/AVComponentInfo.js',
    'emul/humax.ext/json/media/directory.js',
    'emul/humax.ext/json/media/video.js',
    'emul/humax.ext/json/media/music.js',
    'emul/humax.ext/json/media/photo.js'
];
var _RCU_: string = "emul/rcu/oipf_emul_console_hms.html"
var _SERVICE_: ifOBJKeyStringArray = {
    BRIDGE: ['layer_domain/service/dl_dsrvBridge.js'],
    DIAL: ['layer_domain/service/dl_dsrvDial.js'],
    WEB_STORAGE: ['layer_domain/service/dl_dsrvWebStorage.js'],
    STORAGE_MASS: ['layer_domain/service/dl_dsrvStorageMass.js'],
    SETTING_UTIL: ['layer_domain/service/dl_dsrvSettingUtil.js'],
    DMR: ['layer_domain/service/dl_dsrvDmr2.js'],
    CH_LIST: ['layer_domain/service/dl_dsrvChlist.js'],
    RECORDINGS: ['layer_domain/service/dl_dsrvRecordings2.js'],
    EPG: ['layer_domain/service/dl_dsrvEpg.js'],
    AV_CTRL: ['layer_domain/service/dl_dsrvAvCtrl.js'],
    PARENTAL_CTRL: ['layer_domain/service/dl_dsrvParentalCtrl.js'],
    APP_MGR: ['layer_domain/service/dl_dsrvAppMgr.js'],
    HISTORY: ['layer_domain/service/dl_dsrvHistory.js'],
    PLAYLIST: ['layer_domain/service/dl_dsrvPlaylist.js'],
    PLAYBACK: ['layer_domain/service/dl_dsrvPlayback2.js'],
    TSR: ['layer_domain/service/dl_dsrvTsr2.js'],
    STORAGE_SAMBA: ['layer_domain/service/dl_dsrvStorageSamba.js'],
    STORAGE_DLNA: ['layer_domain/service/dl_dsrvStorageDlna.js'],
    LOGICAL_STORAGE: ['layer_domain/service/dl_dsrvLogicalStorage.js'],
    SMART_SEARCH: ['layer_domain/service/dl_dsrvSmartSearch.js'],
    CHANNEL_SEARCH_T: ['layer_domain/service/dl_dsrvChannelSearchT.js'],
    CHANNEL_SEARCH_S: ['layer_domain/service/dl_dsrvChannelSearchS.js'],
    FRONT_PANEL: ['layer_domain/service/dl_dsrvFrontPanel.js'],
    CAS_CI: ['layer_domain/service/dl_dsrvCasCI.js'],
    CAS_IR_EMM_FORCED_OTA: ['layer_domain/service/dl_dsrvCasActionIrEmmForcedOta.js'],
    SW_UPDATE: ['layer_domain/service/dl_dsrvSWUpdate.js'],
    DETAIL_INFO: ['layer_domain/service/dl_dsrvDetailInfo2.js'],
    DCN: ['layer_domain/service/dl_dsrvDCN.js'],
    TV_PORTAL: ['layer_domain/service/dl_dsrvTVPortal.js'],
    DLNA: ['layer_domain/service/dl_dsrvDlna.js'],
    //AIR_PLAY: ['layer_domain/service/dl_dsrvAirPlay.js'],
    SAMBA: ['layer_domain/service/dl_dsrvSamba.js'],
    STORAGE_UTIL: ['layer_domain/service/dl_dsrvStorageUtil.js'],
    MEDIA_SERVER_MANAGER: ['layer_domain/service/dl_dsrvMediaServerManager.js'],
    MEDIA_ITEM_MANAGER: ['layer_domain/service/dl_dsrvMediaItemManager.js'],
    NETFLIX: ['layer_domain/service/dl_dsrvNetflix.js'],
    STATISTICS: ['layer_domain/service/dl_dsrvStatistics.js'],
    /* TODO
     * clockrecoverty, channel search, channel import export, content manager
     * bglee@humaxdigital.com, 17:24, 2014. 5. 14.
     */

    CLOCK_RECOVERY: ['layer_domain/service/dl_dsrvClockRecovery.js'],
    CHANNEL_SEARCH: ['layer_domain/service/dl_dsrvChannelSearch.js'],
    CHANNEL_IMPORT_EXPORT: ['layer_domain/service/dl_dsrvChannelImportExport.js'],
//    CONTENT_MANAGER : ['layer_domain/service/dl_dsrvStatistics.js'],
    KEY_EVENT_BINDER: ['layer_domain/service/dl_key_event_binder.js'],
    CLOCK: ['layer_domain/service/dl_clock.js'],
    IP_EPG: ['layer_domain/service/dl_ip_epg.js'],
    VOICE_ASSISTANCE: ['layer_domain/service/dl_dsrvVoiceAssistance.js']
};
var _KEY_TABLE_ = ["env/keytable"];
var _KEY_TABLE_FOR_EMUL_ = ["def/keytable_for_emul.def.js"];
var _UTILITY_ = [
    "lib/local/ilUtil.js",
    "pages/util/iUiComponent.js",
    "lib/local/languageTranslator.js"
//    "lib/local/illang.js"
];
var _COMPONENT_KEY_ = {
    "pages/component/cmpt_primitive.js": ["button", "text", "input", "frame", "numbers"],
    'pages/component/cmpt_dcn.js': ["dcn"],
    'pages/component/cmpt_settingscmpt.js': ["settingsList", "table", "menuList", "hgroup"],
    'pages/component/cmpt_channellist.js': ["ChannelList", "editChannelOptList"],
    //'pages/component/cmpt_preferredchannellist.js': ["PreferredChannelList", "editPreferredChannelOptList"],
    //'pages/component/cmpt_newpreferredchannellist.js': ["NewPreferredChannelList", "editNewPreferredChannelOptList"],
    'pages/component/cmpt_list.js': ["list", "tab"],
    //'pages/component/cmpt_datetime.js': ["dateTime"],
    'pages/component/cmpt_searchchannels.js': ["Search_list", "Search_result", "txt2", "searchProgress", "scnumbers"],
    'pages/component/cmpt_networksetting.js': ["NetworkAddressType", "networknumbers"],
    'pages/component/cmpt_storage.js': ["storageInfo", "storageGraph",  "strPincode"],
    //'pages/component/cmpt_playbar.js': ["playbar", "pb_button"],
    'pages/msgbox/cmpt_response.js': ["response"],
     'pages/msgbox/cmpt_confirm.js': ["confirm"],
    'pages/msgbox/cmpt_progressbar.js': ["progressBar"],
    //'pages/msgbox/cmpt_usbwizard.js': ["usbWizard"],
    'pages/msgbox/cmpt_pin.js': ["pin"],
    'pages/msgbox/cmpt_pinpc.js': ["pinPC"],
    'pages/msgbox/cmpt_twopin.js': ["twopin"],
    'pages/msgbox/cmpt_inputtime.js': ["inputtime", "inputonetime"],
    //'pages/msgbox/cmpt_inputonetime.js': ["inputonetime"],
    'pages/msgbox/cmpt_inputtext.js': ["inputtext"],
    'pages/msgbox/cmpt_passwd.js': ["passwd"],
    'pages/msgbox/cmpt_lipsync.js': ["lipsync", "subtitleSync"],
    'pages/msgbox/cmpt_notify.js': ["Notify"],
    'pages/msgbox/cmpt_message.js': ["cMessage"],
    'pages/msgbox/cmpt_responseavcomponent.js': ["responseAVC"],
    'pages/msgbox/cmpt_chupdate.js': ["chupdate", "selectchlist"],
    //'pages/msgbox/cmpt_greenbutton.js': ["greenButton"],
    //'pages/msgbox/cmpt_watchinhdbutton.js': ["watchinHDButton"],
    //'pages/msgbox/cmpt_cimmi.js': ["cimmi"],
    'pages/msgbox/cmpt_recordduration.js': ["recordDuration"],
    'pages/msgbox/cmpt_cancelrecording.js': ["cancelRecording"],
    'pages/msgbox/cmpt_recsubconflict.js': ["recordingSubConflict"],
    //'pages/component/cmpt_tvoptionmainmenu.js': ["tvOptionMainmenu"],
    //'pages/component/cmpt_tvoptionmainbtn.js': ["tvOptionMainbtn"],
    //'pages/component/cmpt_tvoptionsubmenu.js': ["tvOptionSubmenu"],
    'pages/component/cmpt_installationwizard.js': ["wizList"],
    //'pages/component/cmpt_installationwizarddis.js': ["wizList2"],
    //'pages/component/cmpt_simplelist.js': ["simpleList"],
    'pages/component/cmpt_simplemenu.js': ["simpleMenu"],
    'pages/component/cmpt_dynamicmenu.js': ["dynamicMenu"],

    'pages/component/cmpt_contextmenu.js': ["ContextMenu"],
    'pages/component/cmpt_help.js': ["HelpView"],
    'pages/component/cmpt_ota.js': ["OtaNoti"]
};
/*
var _COMPONENT_KEY_ = {
     'pages/component/component_util.js': ["button", "text", "input", "frame", "numbers", "dcn", "settingsList", "table", "menuList", "hgroup", "ChannelList",
         "editChannelOptList", "PreferredChannelList", "editPreferredChannelOptList", "NewPreferredChannelList", "editNewPreferredChannelOptList",
         "list", "tab", "Search_list", "Search_result", "txt2", "searchProgress", "scnumbers", "NetworkAddressType", "networknumbers", "storageInfo", "storageGraph",
         "strPincode", "response", "confirm", "progressBar", "pin", "pinPC", "twopin", "inputtime", "inputonetime", "inputtext", "passwd", "Notify", "cMessage",
         "responseAVC", "chupdate", "selectchlist", "recordDuration", "cancelRecording", "recordingSubConflict", "wizList", "simpleMenu", "dynamicMenu",
         "ContextMenu", "HelpView", "OtaNoti"]
};
 */
var _EVENT_: any = {
    alKey: {},
    alForwardKey: {},
    alNextKey: {},
    alNextpage: {},
    alClicked: {},
    alScrollup: {},
    alScrolldown: {},
    alScrollRight: {},
    alScrollLeft: {},
    alChange: {},
    alUpdate: {},
    alChangeFocus: {},
    alExit: {},
    alBack: {},
    alChangeItem: {},
    alNumberOverflow: {},
    alNumberUnderflow: {},
    algetIpAddr: {},
    alEndOfRepeatKey: {},
    alMessageToPage: {},
    alPagedown: {},
    alPageup: {},
    alNumber: {},
    alMostTopPage: {},
    alResetOrdering: {},
    alChangeInstallPage: {},
    alChangeServiceGroup: {},
    //for grab
    alCheckPageVisible: {},
    alPagesVisible: {}
//    alLaunchPage : {}
};
//TODO: _PAGE_UTILITY_ may not need
var _PAGE_UTILITY_: string[] = [
    //XXX: just String, Date object extension, it may move to _UTILITY_
//    "pages/util/utility.js",
    //XXX: json?
//    "pages/util/satTpData.json",
    //XXX: is controls.js ext libs?
    "pages/util/controls.js"
    //XXX: file removed by bglee
//    "pages/util/manager.js",
    //XXX: provider is module type non-global utility
//    "pages/util/provider.js"
];
var _PRISM_MODEL_: string[] = [
    'model/model.js'
];
//XXX: prism ux built on controls.js : initializing time il -> al
var _PRISM_UX_: string[] = [
    'prismux/prism.js',
    'prismux/prism.dlg.js',
    'prismux/prism.dlg.contextmenu.js',
    'prismux/prism.dlg.selection.js',
    'prismux/prism.dlg.password.js',
    'prismux/prism.dlg.button.js',
    'prismux/prism.dlg.image.js',
//    'prismux/prism.dlg.help.js',
    'prismux/prism.dlg.notification.js',
//    'prismux/prism.dlg.dialogWindow.js',
    'prismux/prism.dlg.view.js',
    'prismux/prism.dlg.programmedetail.js',
    'prismux/prism.dlg.login2.js',
    'prismux/prism.dlg.rename.js',
    'prismux/prism.dlg.playback.js',
    'prismux/prism.dlg.photoplayback.js',
    'prismux/prism.dlg.musicplayback.js',
    'prismux/prism.dlg.videoplayback.js',
    'prismux/prism.dlg.sync.js',
    'prismux/prism.dlg.list.js',
    'prismux/prism.dlg.list.combo.js',
    'prismux/prism.control.player.js',
    'prismux/prism.dlg.setting.js',
    'prismux/prism.dlg.query.js',
    'prismux/prism.page.js',
    'prismux/prism.page.main.js',
    'prismux/prism.page.recordings.js',
    'prismux/prism.category.js',
    'prismux/prism.page.media.js',
    'prismux/prism.page.toppicks.js'
//    'pages/media/media.js',
//    'prismux/prism.view.js',
//    'prismux/prism.page.settings.js'
];
var _EXTERNAL_LIBS_ = {
    DEBUG: ["lib/ext/jquery-1.7.1.js", "lib/ext/lodash.min.js", "lib/ext/crossBrowser_initKeyboardEvent.js", "lib/ext/dateformat.js", "lib/ext/taffy.js", "lib/ext/q.min.js"],
    RELEASE: ["lib/ext/jquery-1.7.1.min.js", "lib/ext/lodash.min.js", "lib/ext/dateformat.js", "lib/ext/taffy.js", "lib/ext/q.min.js"]
};
var __UNCLASSIFIED_DATA__: any = {
    settings: {
        systeminfo: {
            title: ['LOC_SYSTEM_INFORMATION_ID'],
            itemId: ['modelname', 'softver', 'loaderver', 'systemid', 'updatedate', 'serialnumber', 'macaddlen', 'macaddwlen', 'macaddbluetooth'],
            itemStr: ['LOC_MODELNAME_ID', 'LOC_SOFTVER_ID', 'LOC_LOADERVER_ID', 'LOC_SYSTEMID_ID', 'LOC_UPDATEDATE_ID', 'LOC_MICOMVER_ID', 'LOC_MACADDR_ID', 'LOC_MACADDR_ID', 'LOC_MACADDR_ID'],
            describe: ['', '', '', '', '', '', '', '', '', '']
        },
        motorsatellite: {
            title: ['LOC_SETSATEL_FOR_SEARCH_ID'],
            itemId: ['satellite', 'activation'],
            itemStr: ['LOC_SATELLITE_ID', 'LOC_ACTIVATION_ID'],
            itemValue0: ['AMOS 2&amp;3 (4.0W) ', 'APSTAR 2R (76.5E) ', 'ARABSAT 5A (30.5E) ', 'ASIASAT 3S (105.5E) ', 'ASIASAT 5 (100.5E) ', 'ASTRA 1 (19.2E) ', 'ASTRA 2 (28.2E) ', 'ASTRA 3A&amp;3B (23.5E) ', 'ASTRA 4A (5.0E) ', 'ATLANTIC BIRD 1 (12.5W) ', 'ATLANTIC BIRD 2 (8.0W) ', 'ATLANTIC BIRD 3 (5.0W) ', 'BADR 4&amp;6 (26.0E) ', 'EUROBIRD 1 (28.2E) ', 'EUROBIRD 3 (33.0E) ', 'EUROBIRD 9A (9.0E) ', 'EUTELSAT W2A (10.0E) ', 'EUTELSAT W3A (7.0E) ', 'EUTELSAT W4&amp;W7 (36.0E) ', 'EUTELSAT W6 (21.6E) ', 'EXPRESS AM1 (40.0E) ', 'EXPRESS AM22 (53.0E) ', 'HELLAS SAT2 (39.0E) ', 'HISPASAT 1C&amp;1D (30.0W) ', 'HOTBIRD (13.0E) ', 'INSAT 3A&amp;4B (93.5E) ', 'INTELSAT 10-02 (1.0W) ', 'INTELSAT 12 (45.0E) ', 'INTELSAT 7&amp;10 (68.5E) ', 'INTELSAT 901 (18.0W) ', 'INTELSAT 902 (62.0E) ', 'INTELSAT 904 (60.0E) ', 'INTELSAT 906 (64.2E) ', 'INTELSAT 907 (27.5W) ', 'NILESAT 101&amp;102 (7.0W) ', 'NSS 12 (57.0E) ', 'NSS 7 (22.0W) ', 'TELSTAR 12 (15.0W) ', 'THOR 5&amp;6 (1.0W) ', 'TURKSAT 2A&amp;3A (42.0E) ', 'YAMAL 201 (90.0E) '],
            itemValue1: ['LOC_ON_ID', 'LOC_OFF_ID']
        },
        language: {
            title: ['LOC_BIGLANGUAGE_ID'],
            itemId: ['systemlang', 'audiolang', 'subtitlelang'],
            itemStr: ['LOC_SYSTEM_LANGUGE_ID', 'LOC_AUDIO_LANGUAGE_ID', 'LOC_SUBTITLE_LANGUAGE_ID'],
            describe: ['', '', ''],
            itemSub01Str: ['LOC_SET_ENGLISH_ID'],
            itemSub02Str: ['LOC_OFF_ID', 'LOC_SET_ENGLISH_ID', 'LOC_LANG_GARLIC_ID', 'LOC_LANG_WELSH_ID', 'LOC_LANG_IRISH_ID'],
            itemSub03Str: ['LOC_OFF_ID', 'LOC_SET_ENGLISH_ID', 'LOC_LANG_GARLIC_ID', 'LOC_LANG_WELSH_ID', 'LOC_LANG_IRISH_ID']
        },
        powermanager: {
            title: ['LOC_POWER_MANAGEMENT_ID'],
            itemId: ['autopowerdown', 'powersavingstandby'],
            itemStr: ['LOC_AUTOPOWERDOWN_ID', 'LOC_POWER_SAVING_MODE_MENU_ID'],
            describe: ['', ''],
            itemSub01Str: ['LOC_OFF_ID', 'LOC_ON_ID'],
            itemSub02Str: ['LOC_OFF_ID', 'LOC_ON_ID']
        },
        recording: {
            title: ['LOC_RECORDINGOPTIONS_ID'],
            itemId: ['startpadding', 'endpadding'],
            itemStr: ['LOC_START_PADDING_ID', 'LOC_END_PADDING_ID'],
            describe: ['', ''],
            itemSub01Str: ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID'],
            itemSub02Str: ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID']
        },
        extstorage: {
            title: [''],
            itemId: ['removestorage', 'formatstorage'],
            itemStr: ['LOC_REMOVE_STORAGESAFELY_ID', 'LOC_FORMATSTR_ID'],
            describe: ['', ''],
            itemSub01Str: [],
            itemSub02Str: []
        },
        playbackoption: {
            title: ['LOC_PLAYBACK_OPTIONS_ID'],
            itemId: ['seriesplay', 'instantreplaytime', 'skipforwardtime'],
            itemStr: ['LOC_SERIESPLAY_ID', 'LOC_INSTANT_REPLAY_TIME_ID', 'LOC_SKIP_FORWARD_TIME_ID'],
            describe: ['', '', ''],
            itemSub01Str: ['LOC_ON_ID', 'LOC_OFF_ID'],
            itemSub02Str: ['LOC_7_SEC_ID', 'LOC_15_SEC_ID', 'LOC_30_SEC_ID'],
            itemSub03Str: ['LOC_30_SEC_ID', 'LOC_60_SEC_ID', 'LOC_120_SEC_ID', 'LOC_240_SEC_ID']
        },
        channelgroupT: {
            bandwidth7MHz: {
                channel: ['6', '7', '8', '9', '9A', '10', '11', '12', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '43', '44', '45',
                    '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63', '64', '65', '66', '67', '68', '69'],
                centreFrequency: [177500, 184500, 191500, 198500, 205500, 212500, 219500, 226500, 522500, 529500, 536500, 543500, 550500, 557500, 564500, 571500,
                    578500, 585500, 592500, 599500, 606500, 613500, 620500, 627500, 634500, 641500, 648500, 655500, 662500, 669500, 676500, 683500, 690500, 697500,
                    704500, 711500, 718500, 725500, 732500, 739500, 746500, 753500, 760500, 767500, 774500, 781500, 788500, 795500, 802500, 809500, 816500],
                limitFrequency: [
                    [174000, 181000],
                    [181000, 188000],
                    [188000, 195000],
                    [195000, 202000],
                    [202000, 209000],
                    [209000, 216000],
                    [216000, 223000],
                    [223000, 230000],
                    [519000, 526000],
                    [526000, 533000],
                    [533000, 540000],
                    [540000, 547000],
                    [547000, 554000],
                    [554000, 561000],
                    [561000, 568000],
                    [568000, 575000],
                    [575000, 582000],
                    [582000, 589000],
                    [589000, 596000],
                    [596000, 603000],
                    [603000, 610000],
                    [610000, 617000],
                    [617000, 624000],
                    [624000, 631000],
                    [631000, 638000],
                    [638000, 645000],
                    [645000, 652000],
                    [652000, 659000],
                    [659000, 666000],
                    [666000, 673000],
                    [673000, 680000],
                    [680000, 687000],
                    [687000, 694000],
                    [694000, 701000],
                    [701000, 708000],
                    [708000, 715000],
                    [715000, 722000],
                    [722000, 729000],
                    [729000, 736000],
                    [736000, 743000],
                    [743000, 750000],
                    [750000, 757000],
                    [757000, 764000],
                    [764000, 771000],
                    [771000, 778000],
                    [778000, 785000],
                    [785000, 792000],
                    [792000, 799000],
                    [799000, 806000],
                    [806000, 813000],
                    [813000, 820000]
                ],
                bandwith: "7MHz"
            },
            bandwidth8MHz: {
                channel: ['21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '43', '44', '45',
                    '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63', '64', '65', '66', '67', '68', '69'],
                centreFrequency: [474000, 482000, 490000, 498000, 506000, 514000, 522000, 530000, 538000, 546000, 554000, 562000, 570000, 578000,
                    586000, 594000, 602000, 610000, 618000, 626000, 634000, 642000, 650000, 658000, 666000, 674000, 682000, 690000, 698000, 706000, 714000, 722000,
                    730000, 738000, 746000, 754000, 762000, 770000, 778000, 786000, 794000, 802000, 810000, 818000, 826000, 834000, 842000, 850000, 858000],
                limitFrequency: [
                    [470000, 478000],
                    [478000, 486000],
                    [486000, 494000],
                    [494000, 502000],
                    [502000, 510000],
                    [510000, 518000],
                    [518000, 526000],
                    [526000, 534000],
                    [534000, 542000],
                    [542000, 550000],
                    [550000, 558000],
                    [558000, 566000],
                    [566000, 574000],
                    [574000, 582000],
                    [582000, 590000],
                    [590000, 598000],
                    [598000, 606000],
                    [606000, 614000],
                    [614000, 622000],
                    [622000, 630000],
                    [630000, 638000],
                    [638000, 646000],
                    [646000, 654000],
                    [654000, 662000],
                    [662000, 670000],
                    [670000, 678000],
                    [678000, 686000],
                    [686000, 694000],
                    [694000, 702000],
                    [702000, 710000],
                    [710000, 718000],
                    [718000, 726000],
                    [726000, 734000],
                    [734000, 742000],
                    [742000, 750000],
                    [750000, 758000],
                    [758000, 766000],
                    [766000, 774000],
                    [774000, 782000],
                    [782000, 790000],
                    [790000, 798000],
                    [798000, 806000],
                    [806000, 814000],
                    [814000, 822000],
                    [822000, 830000],
                    [830000, 838000],
                    [838000, 846000],
                    [846000, 854000],
                    [854000, 862000]
                ],
                bandwith: "8MHz"
            }
        },
        parentalcontrol: [
            ['LOC_RESTRICTION_OF_ACCESS_ID', 'LOC_CHANGE_PASSWORD_ID', 'LOC_BLOCK_SETTINGS_CHANNEL_ID'],
            ['LOC_NO_BLOCK_ID', 'LOC_BLOCK_G_ID', 'LOC_BLOCK_PG_ID', 'LOC_BLOCK_M_ID', 'LOC_BLOCK_MA_ID', 'LOC_BLOCK_AV_ID', 'LOC_BLOCK_R_ID', 'LOC_BLOCK_ALL_ID'], //Maturity Rating
            ['LOC_OFF_ID', 'LOC_ON_ID'] //Channel Settings PIN
        ]
    },
    wzd: {
        page01: {	// welcome
            itemStr: ['LOC_ENJOY_HUMAXWORLD_ID'],
            nextPage: 'CPgInstall02',
            prevPage: '',
            fronttext: '-WIZARD-'
        },
        page02: {	// Start
//            itemStr: ['LOC_BIG_PREFERRED_ID', 'LOC_INTERNET_ID', 'LOC_BIGCHANNEL_ID', 'LOC_POWER_27_ID'],
//            stepType: ['LOC_BIGLANGUAGE_ID', 'LOC_CONNECTION_ID', 'LOC_SEARCH_ID', 'LOC_SAVING_MODE_ID'],
            itemStr: ['LOC_BIG_PREFERRED_ID', 'LOC_BIGCHANNEL_ID', 'LOC_INTERNET_ID', 'LOC_POWER_27_ID'],
            stepType: ['LOC_BIGLANGUAGE_ID', 'LOC_SEARCH_ID', 'LOC_CONNECTION_ID', 'LOC_SAVING_MODE_ID'],
            nextPage: 'CPgInstall03',
//		nextPage : 'CPgInstall08_1',
            prevPage: ''
        },
        page03: {// Language
            itemStr: ['LOC_SET_ENGLISH_ID', 'LOC_LANG_GARLIC_ID', 'LOC_LANG_WELSH_ID', 'LOC_LANG_IRISH_ID'],
            langIsoCode: ['eng', 'gla', 'wel', 'gle'],
            //nextPage: 'CPgInstall04',//network
            nextPage: 'CPgInstall07',//Search
            prevPage: '',
            step : 'step1'
        },
        page04: {	// Start network
            itemStr: ['LOC_CABLE_ID', 'LOC_USBDONGLE_ID'],
            nextPage: 'CPgInstall06', //Network Reuslt
            //prevPage: 'CPgInstall03', //Language
            prevPage: 'CPgInstall07', // Search
            step : 'step3'
        },
        page05: {	// Wifi
            itemStr: [],
            nextPage: 'CPgInstall06', //Internet
            prevPage: 'CPgInstall04', //Network
            step : 'step3'
        },
        page06: {	// Network Result
            itemStr: [],
            nextPage: 'CPgInstall06_1',
            //prevPage: 'CPgInstall04'
            prevPage: 'CPgInstall04', //Network
            step : 'step3'
        },
        page06_1: {	// Privacy Policy
            itemStr: [],
            //nextPage: 'CPgInstall07', //Search
            nextPage: 'CPgInstall08_1', //Quick Boot
            prevPage: 'CPgInstall06', // Network Result
            step : 'step3'
        },
        page07: {	// Start Search
            itemStr: [],
            nextPage: 'CPgInstall08',
            //prevPage: 'CPgInstall04',// Network
            prevPage: 'CPgInstall03',// Language
            //skipPage: 'CPgInstall08_1'
            skipPage: 'CPgInstall06',
            step : 'step2'
        },
        page08: {	// Search Action
            itemStr : ["See more information about TV programmes and what's on Catch-up with the Freeview Play guide.",
                "Find your favourite programmes quicker from Live TV, Catch-up, Recordings and On-Demand by presssing the search button.",
                "Catch up on your favourite programmes courtesy of Freeview Play.<br>Find shows within the Catch-up and On-Demand section.",
                "Watch live, playback recordings, set recordings and more from our Live TV app. Download the Humax Live TV app from the App Store or Google Play Store.",
                "With three tuners, you can record four channels simultaneously while watching a fifth channel."],
            images: ['./images/614_WIZ_Promotion01.png', './images/614_WIZ_Promotion03.png', './images/614_WIZ_Promotion04.png', './images/614_WIZ_Promotion02.png', './images/614_WIZ_Promotion05.png'],
            //nextPage: 'CPgInstall08_1',//Quick Boot
            nextPage: 'CPgInstall04',//Network
            prevPage: '',
            step : 'step2'
        },
        page08_1: {	// Quick Boot Mode
            itemStr: ['LOC_POWER_SAVING_MODE_MENU_ID'],
            itemSubStr: ['LOC_OFF_ID', 'LOC_ON_ID'],
            nextPage: 'CPgInstall09',
            //prevPage: 'CPgInstall07' //Search
            prevPage: 'CPgInstall04', //Internet
            step : 'step4'
        },
        page09: {	// Result
            itemStr: [],
            nextPage: 'CPgLiveController',
            prevPage: 'CPgInstall08_1',
            step : 'step4'
        },
        lockingDataList: [
            {
                antId: 1,
                tpData: [10832000, 22000000, 1, "auto", 1, 1, 1]
            },
            {
                antId: 1,
                tpData: [12552000, 22000000, 2, "auto", 1, 0, 1]
            },
            {
                antId: 1,
                tpData: [12604000, 22000000, 1, "auto", 1, 0, 1]
            }
        ]
    },
    CPgInfoHub: {
        chGroup: {
            strCasID: ['CAS_FTA', 'CAS_NAGRA', 'CAS_IRDETO', 'CAS_NDS', 'CAS_VIACCESS', 'CAS_CONAX', 'CAS_MEDIAGUARD', 'CAS_CRYPTOWORKS'],
            // groupDB name is same with dl_dsrvChlist.js
            groupDB: ['TV', 'RADIO', 'HDTV', 'Favourite1', 'Favourite2', 'Favourite3', 'Favourite4', 'Favourite5'],
            favGroupDB: ['Favourite1', 'Favourite2', 'Favourite3', 'Favourite4', 'Favourite5'],
            alphabetGroupsDB: [],
            groupName: ['LOC_TV_ID', 'LOC_RADIO_ID', 'LOC_HDTV_ID', 'LOC_FAVOURITE1_ID', 'LOC_FAVOURITE2_ID', 'LOC_FAVOURITE3_ID', 'LOC_FAVOURITE4_ID', 'LOC_FAVOURITE5_ID'],
            favlistName: ['LOC_FAVOURITE1_ID', 'LOC_FAVOURITE2_ID', 'LOC_FAVOURITE3_ID', 'LOC_FAVOURITE4_ID', 'LOC_FAVOURITE5_ID'],
            alphabetGroupName: []
        }
    },
    frontPanel: {
        lcdVfd16: ['-NO CHANNEL-', '-WIZARD-', '-TV GUIDE-', '-GUIDE-', '-MENU-', '-SETTINGS-', '-VIDEO-', '-MUSIC-', '-PHOTO-', '-MUTE-', 'HUMAX TV PORTAL', '-CHANNEL LIST-', 'START RECORDING', 'MEDIA PLAY', 'PAUSE', '-TV APPS-', 'FRX', 'FFX'],
        lcdVfd10: ['-NO CHANNEL-', '-WIZARD-', '-TV GUIDE-', '-GUIDE-', '-MENU-', '-SETTINGS-', '-VIDEO-', '-MUSIC-', '-PHOTO-', '-MUTE-', 'TV PORTAL', '-CH LIST-', 'RECORDING', 'MEDIA PLAY', 'PAUSE', '-TV APPS-', 'FRX', 'FFX'],
        segment7: ['----', 'INST', 'EPG', 'EPG', 'MENU', 'MENU', 'FILE', 'FILE', 'FILE', '', '', '', 'REC', 'PLAY', 'PAUS', 'APPS', 'FBXX', 'FFXX']
    },
    help2: {
        'tvapps': [
            {
                'index': '0',
                'no': '01',
                'title': 'LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID',
                'scene': '../../images/614_Help_Promotion_TA01.png',
                'caption': 'LOC_CATCH_UP_FREEVIEW_ID'
            },
            {
                'index': '1',
                'no': '02',
                'title': 'LOC_CREATE_AN_ACCOUNT_FIRST_ID',
                'scene': '../../images/614_Help_Promotion_TA02.png',
                'caption': 'LOC_HELP_TV_APPS_STEP_2_SCENE_1_TXT_1_ID'
            },
            {
                'index': '2',
                'no': '02',
                'title': 'LOC_CREATE_AN_ACCOUNT_FIRST_ID',
                'scene': '../../images/312_Help_Promotion_TA03.png',
                'caption': 'LOC_HELP_TV_APPS_STEP_2_SCENE_2_TXT_1_ID'
            },
            {
                'index': '3',
                'no': '03',
                'title': 'LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID',
                'scene': '../../images/312_Help_Promotion_TA04.png',
                'caption': 'LOC_HELP_TV_APPS_STEP_3_SCENE_1_TXT_1_ID'
            },
            {
                'index': '4',
                'no': '03',
                'title': 'LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID',
                'scene': '../../images/312_Help_Promotion_TA05.png',
                'caption': 'LOC_HELP_TV_APPS_STEP_3_SCENE_2_TXT_1_ID'
            }
        ]
    },
    help: {
        'ltv': {
            'txt': [
                ['LOC_HELP_LIVE_TV_STEP_1_TITLE_1_ID'],
                ['LOC_HELP_LIVE_TV_STEP_2_TITLE_1_ID'],
                ['LOC_HELP_LIVE_TV_STEP_3_TITLE_1_ID']
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_LIVE_TV_STEP_1_TITLE_1_ID'],
                        'scene': 'images/312_Help_Promotion_LTV01.png',
                        'caption': ['LOC_HELP_LIVE_TV_STEP_1_SCENE_1_TXT_1_ID', '<br>', 'LOC_HELP_LIVE_TV_STEP_1_SCENE_1_TXT_2_ID']
                    }
                ],
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_LIVE_TV_STEP_1_TITLE_1_ID'],
                        'scene': 'images/312_Help_Promotion_LTV02.png',
                        'caption': ['LOC_HELP_LIVE_TV_STEP_1_SCENE_2_TXT_1_ID']
                    }
                ],
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_LIVE_TV_STEP_1_TITLE_1_ID'],
                        'scene': 'images/312_Help_Promotion_LTV03.png',
                        'caption': ['LOC_FVC_HELP_LIVE_TV_STEP_1_SCENE_3_TXT_1_ID']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': ['LOC_HELP_LIVE_TV_STEP_2_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_LTV04.png',
                        'caption': ['LOC_HELP_LIVE_TV_STEP_2_SCENE_1_TXT_1_ID']
                    }
                ],
                [
                    {
                        'txtnum': 2,
                        'txt': ['LOC_HELP_LIVE_TV_STEP_3_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_LTV05.png',
                        'caption': ['Search more episodes with search suggestions or your entered keywords. Just press SEARCH.']
                    }
                ]
            ]
        },
        'tvg': {
            'txt': [
                ['LOC_HELP_TV_GUIDE_STEP_1_TITLE_1_ID'],
                ['LOC_HELP_TV_GUIDE_STEP_2_TITLE_1_ID', 'LOC_HELP_TV_GUIDE_STEP_2_TITLE_2_ID', 'LOC_HELP_TV_GUIDE_STEP_2_TITLE_3_ID']
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_TV_GUIDE_STEP_1_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_EPG01.png',
                        'caption': ['LOC_HELP_TV_GUIDE_STEP_1_SCENE_1_TXT_1_ID', '<br>', 'LOC_HELP_TV_GUIDE_STEP_1_SCENE_1_TXT_2_ME_ID']
                    }
                ],
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_TV_GUIDE_STEP_1_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_EPG02.png',
                        'caption': ['LOC_HELP_TV_GUIDE_STEP_1_SCENE_2_TXT_1_ID']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': ['LOC_FVC_HELP_TV_GUIDE_STEP_2_TITLE_ID'],
                        'scene': 'images/614_Help_Promotion_EPG03.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_FVC_PRESS_LEFT_TO_CATCH_UP_ID',
                                param: ['<em class="img_pressLeft"></em>']
                            }
                        ]
                    }
                ]
                //,
                //[
                //    {
                //        'txtnum': 1,
                //        'txt': ['LOC_FVC_HELP_TV_GUIDE_STEP_2_TITLE_ID'],
                //        'scene': 'product/fvp4000t/images/614_Help_Promotion_EPG04.png',
                //        'caption': ['LOC_HELP_TV_GUIDE_STEP_2_SCENE_2_TXT_1_ID']
                //    }
                //]
            ]
        },
        'tvg_update1': {
            'txt': [
                ['LOC_HELP_TV_GUIDE_STEP_1_TITLE_1_ID'],
                ['LOC_HELP_TV_GUIDE_STEP_2_TITLE_1_ID', 'LOC_HELP_TV_GUIDE_STEP_2_TITLE_2_ID', 'LOC_HELP_TV_GUIDE_STEP_2_TITLE_3_ID']
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_TV_GUIDE_STEP_1_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_EPG01.png',
                        'caption': ['LOC_GUIDE_RENEWAL_DESC_ID']
                    }
                ],
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_TV_GUIDE_STEP_1_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_EPG03.png',
                        'caption': ['LOC_HELP_TV_GUIDE_STEP_1_SCENE_2_TXT_1_ID']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': ['LOC_FVC_HELP_TV_GUIDE_STEP_2_TITLE_ID'],
                        'scene': 'images/614_Help_Promotion_EPG02.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_FVC_PRESS_LEFT_TO_CATCH_UP_ID',
                                param: ['<em class="img_pressLeft"></em>']
                            }
                        ]
                    }
                ],
                [
                    {
                        'txtnum': 2,
                        'txt': ['LOC_DETAILED_RECORDING_OPTION_01_TITLE_ID'],
                        'scene': 'images/614_Help_Promotion_EPG04.png',
                        'caption': ['LOC_DETAILED_RECORDING_OPTION_01_DESC_ID']
                    }
                ]
            ]
        },
        'rec': {
            'txt': [
                ['LOC_HELP_RECORDINGS_STEP_1_TITLE_1_ID'],
                ['LOC_HELP_RECORDINGS_STEP_2_TITLE_1_ID']
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': ['LOC_HELP_RECORDINGS_STEP_1_TITLE_1_ID'],
                        'scene': 'images/614_Help_Promotion_REC01.png',
                        'caption': ['LOC_HELP_RECORDINGS_STEP_1_SCENE_1_TXT_1_ID', ' ', {
                            stringFormat: 'LOC_HELP_RECORDINGS_STEP_1_SCENE_1_TXT_2_ID',
                            param: ['']
                        }]
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': ['LOC_HELP_RECORDINGS_STEP_2_TITLE_1_ID'],
                        'scene': 'images/312_Help_Promotion_REC02.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_HELP_RECORDINGS_STEP_2_SCENE_1_TXT_1_ID',
                                param: ['<br>']
                            }
                        ]
                    }
                ]
            ]
        },
        'tvapps': {
            'txt': [
                [
                    {
                        stringFormat: 'LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID',
                        param: ['']
                    }
                ],
                [
                    {
                        stringFormat: 'LOC_HELP_TV_APPS_STEP_2_TITLE_1_ID',
                        param: ['']
                    }
                ],
                ['LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID']
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': [
                            {
                                stringFormat: 'LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID',
                                param: ['']
                            }
                        ],
                        'scene': 'images/614_Help_Promotion_TA01.png',
                        'caption': ['LOC_INSTALL_IR_STR4_ID']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': [
                            {
                                stringFormat: 'LOC_HELP_TV_APPS_STEP_2_TITLE_1_ID',
                                param: ['']
                            }
                        ],
                        'scene': 'images/614_Help_Promotion_TA02.png',
                        'caption': ['LOC_HELP_TV_APPS_STEP_2_SCENE_1_TXT_1_ID']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': [
                            {
                                stringFormat: 'LOC_HELP_TV_APPS_STEP_2_TITLE_1_ID',
                                param: ['']
                            }
                        ],
                        'scene': 'images/312_Help_Promotion_TA03.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_HELP_TV_APPS_STEP_2_SCENE_2_TXT_1_ID',
                                param: ['']
                            }
                        ]
                    }
                ],
                [
                    {
                        'txtnum': 2,
                        'txt': ['LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID'],
                        'scene': 'images/312_Help_Promotion_TA04.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_HELP_TV_APPS_STEP_3_SCENE_1_TXT_1_ID',
                                param: ['']
                            }
                        ]
                    }
                ],
                [
                    {
                        'txtnum': 2,
                        'txt': ['LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID'],
                        'scene': 'images/312_Help_Promotion_TA05.png',
                        'caption': ['LOC_HELP_TV_APPS_STEP_3_SCENE_2_TXT_1_ID']
                    }
                ]
            ]
        },
        'media': {
            'txt': [
                [
                    {
                        stringFormat: 'LOC_HELP_MEDIACENTER_STEP_1_TITLE_1_ID',
                        param: ['']
                    }
                ],
                [
                    {
                        stringFormat: 'LOC_HELP_MEDIACENTER_STEP_2_TITLE_1_ID',
                        param: ['']
                    }
                ],
                [
                    {
                        stringFormat: 'LOC_HELP_MEDIACENTER_STEP_3_TITLE_1_ID',
                        param: ['']
                    }
                ]
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': [
                            {
                                stringFormat: 'LOC_HELP_MEDIACENTER_STEP_1_TITLE_1_ID',
                                param: ['']
                            }
                        ],
                        'scene': 'images/312_Help_Promotion_ME01.png',
                        'caption': ['LOC_HELP_MEDIACENTER_STEP_1_SCENE_1_TXT_1_ID', '<br>', 'LOC_HELP_MEDIACENTER_STEP_1_SCENE_1_TXT_2_ID']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': [
                            {
                                stringFormat: 'LOC_HELP_MEDIACENTER_STEP_2_TITLE_1_ID',
                                param: ['']
                            }
                        ],
                        'scene': 'images/312_Help_Promotion_ME02.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_HELP_MEDIACENTER_STEP_2_SCENE_1_TXT_1_ID',
                                param: ['<br>']
                            }
                        ]
                    }
                ],
                [
                    {
                        'txtnum': 2,
                        'txt': [
                            {
                                stringFormat: 'LOC_HELP_MEDIACENTER_STEP_3_TITLE_1_ID',
                                param: ['']
                            }
                        ],
                        'scene': 'images/312_Help_Promotion_ME03.png',
                        'caption': [
                            {
                                stringFormat: 'LOC_HELP_MEDIACENTER_STEP_3_SCENE_1_TXT_1_ID',
                                param: ['<br>']
                            }
                        ]
                    }
                ]
            ]
        },
        'home': {
            'txt': [
                [
                    {
                        stringFormat: 'SMART SEARCH. A SIMPLE BUTTON PRESS',
                        param: ['']
                    }
                ],
                [
                    {
                        stringFormat: 'SMART SEARCH. A SIMPLE BUTTON PRESS',
                        param: ['']
                    }
                ]
            ],
            'step': [
                [
                    {
                        'txtnum': 0,
                        'txt': [
                            {
                                stringFormat: 'SMART SEARCH. A SIMPLE BUTTON PRESS',
                                param: ['']
                            }
                        ],
                        'scene': './images/614_Help_Promotion_update01.png',
                        'caption': ['Search more content with search suggestions.<br>Just press SEARCH via TV remote to enter text.']
                    }
                ],
                [
                    {
                        'txtnum': 1,
                        'txt': [
                            {
                                stringFormat: 'SMART SEARCH. A SIMPLE BUTTON PRESS',
                                param: ['']
                            }
                        ],
                        'scene': './images/614_Help_Promotion_update02.png',
                        'caption': ['Search results are classified based on content type such as On Demand, broadcast, recordings and more.']
                    }
                ]
            ]
        }
    }
};
var _LANGUAGE_TABLE_ = {
    eng: __DEF__.eISO639.eng,
    ger: __DEF__.eISO639.ger,
    tur: __DEF__.eISO639.tur,
    rus: __DEF__.eISO639.rus,
    fre: __DEF__.eISO639.fre,
    ara: __DEF__.eISO639.ara,
    per: __DEF__.eISO639.per,
    gre: __DEF__.eISO639.gre,
    ita: __DEF__.eISO639.ita,
    spa: __DEF__.eISO639.spa,
    gla: __DEF__.eISO639.gla,
    wel: __DEF__.eISO639.wel,
    gle: __DEF__.eISO639.gle,
    ces: __DEF__.eISO639.ces,
    slo: __DEF__.eISO639.slo
};
var _PRISM_UI_FRAMEWORK_SET_ = [
    "pages/util/controls.js",
    'prismux/prism.js'
];
interface ifCacheConfigurationInfo {
    config: any;
    pages: any;
    pageLayout: any;
    emulator: any;
    keyTable: any;
    emulationData: any;
    utility: any;
    pageUtility: any;
    extLibs: any;
    prismModel: any;
    prismUX: any;
    service: any;
    componenets: any;
    events: any;
    prismUIFramework: any;
    languageTable: any;
    settings: any;
    wzd: any;
    CPgInfoHub: any;
    frontPanel: any;
    help2: any;
    help: any;
}
class cProductConfiguration implements ifProductConfiguration {
    private _codeProduct: number = __DEF__.eProductName.DEFAULT;
    private _dataProductSetting: ifProductSetting;
    private _modeDistribution: number = __DEF__.eModeDistribution.DEBUG;
    private _isEmul: boolean = !window['oipfObjectFactory'];
    private _cache: ifCacheConfigurationInfo = {
        config: null,
        pages: null,
        pageLayout: null,
        emulator: null,
        keyTable: null,
        emulationData: null,
        utility: null,
        pageUtility: null,
        extLibs: null,
        prismModel: null,
        prismUX: null,
        service: null,
        componenets: null,
        events: null,
        prismUIFramework: null,
        languageTable: null,
        settings: null,
        wzd: null,
        CPgInfoHub: null,
        frontPanel: null,
        help2: null,
        help: null
    };

    constructor(aProductName: string, aProductSettingData?: any) {
        var productNameCode = __DEF__.eProductName[aProductName.trim().toUpperCase()];

        if (!productNameCode) {
            throw "product[" + aProductName + "] does not support. do extend enum eProductName in config.d.ts file";
        }

        this._codeProduct = productNameCode;
        this._dataProductSetting = aProductSettingData || {};
        this._modeDistribution = this._dataProductSetting.release ? __DEF__.eModeDistribution.RELEASE : __DEF__.eModeDistribution.DEBUG;
    }

    private _fnOverwriteUserData(aData: any, aUserData: any): any {
        if (aUserData) {
            var keys = Object.keys(aUserData);
            var len = keys.length;
            try {
                for (var i = 0; i < len; i++) {
                    aData[keys[i]] = aUserData[keys[i]]
                }
            }
            catch (aEx) {
                throw aEx;
            }
        }
        return aData;
    }

    private _fnMakePathWithPrefix(aStringArray: string[], aPrefix: string, aCutExt: boolean) {
        var ret = aStringArray;
        var fnCutExt = aCutExt ? this._fnCutExt : (aDummy: string) => {
            return aDummy;
        };
        if (aPrefix || aCutExt) {
            for (var i = 0, len = ret.length; i < len; i++) {
                ret[i] = fnCutExt(aPrefix + ret[i]);
            }
        }
        return ret;
    }

    private _fnMakePathWithPrefixForObjectStringArray(aObjectStringArray: ifOBJKeyStringArray, aPrefix: string, aCutExt: boolean) {
        var ret = aObjectStringArray;
        if (aPrefix || aCutExt) {
            var arrayString: string[];
            for (var key in ret) {
                arrayString = ret[key];
                this._fnMakePathWithPrefix(ret[key], aPrefix, aCutExt);
            }
        }
        return ret;
    }

    private _fnCutExt(aFileName: string) {
        var index = aFileName.lastIndexOf('.');
        if (index === -1) {
            return aFileName;
        }
        return aFileName.substring(0, index);
    }

    //XXX: to manage pages
    getConfig(): ifCONFIG {
        return this._cache.config || (this._cache.config = this._fnOverwriteUserData(_DEFAULT_CONFIG, this._dataProductSetting.config));
    }

    getPages(): ifPAGE {
        return this._cache.pages || (this._cache.pages = this._fnOverwriteUserData(_DEFAULT_PAGE, this._dataProductSetting.page));
    }

    getPageLayout(): ifPageLayout {
        if (!this._cache.pageLayout) {
            var userData = this._dataProductSetting.pageLayout;
            var top = _PAGE_TOP_;
            var center = _PAGE_CENTER_;
            var bottom = (this._dataProductSetting.bottom_page) ? this._dataProductSetting.bottom_page : _PAGE_BOTTOM_;
            var wizard = _PAGE_INSTALL_WIZARD_;
            if (userData) {
                top = userData.pages_top || top;
                this._fnOverwriteUserData(center, userData.pages_center);
                bottom = userData.pages_bottom || top;
                wizard = userData.pages_install_wizard || _PAGE_INSTALL_WIZARD_;
            }
        }
        return this._cache.pageLayout || (this._cache.pageLayout = {
            pages_top: top,
            pages_center: center,
            pages_bottom: bottom,
            pages_install_wizard: wizard
        });
    }

    getPageEntry(): string {
        return this._dataProductSetting.pageEntry || _PAGE_ENTRY_;
    }

    getEmulator(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.emulator || (this._cache.emulator = this._fnMakePathWithPrefix(_EMULATOR_, aPrefix, aCutExt));
    }

    getKeyTable(aPrefix: string = "", aCutExt: boolean = true): string[] {
        var ret = _KEY_TABLE_;
        if (!this._cache.keyTable) {
            if (this._dataProductSetting.keyTable) {
                ret = ret.concat(this._dataProductSetting.keyTable);
            }
            if (this._isEmul && !_DEFAULT_CONFIG.emulOnBox) {
                ret = ret.concat(_KEY_TABLE_FOR_EMUL_);
            }
        }
        return this._cache.keyTable || (this._cache.keyTable = this._fnMakePathWithPrefix(ret, aPrefix, aCutExt));
    }

    getEmulationData(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.emulationData || (this._cache.emulationData = this._fnMakePathWithPrefix(_EMULATION_DATA_, aPrefix, aCutExt));
    }

    getRCU(): string {
        return _RCU_;
    }

    getModeDistribution(): string {
        return __DEF__.eModeDistribution[this._modeDistribution];
    }

    getUtility(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.utility || (this._cache.utility = this._fnMakePathWithPrefix(_UTILITY_, aPrefix, aCutExt));
    }

    getPageUtility(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.pageUtility || (this._cache.pageUtility = this._fnMakePathWithPrefix(_PAGE_UTILITY_, aPrefix, aCutExt));
    }

    getExtLibs(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.extLibs || (this._cache.extLibs = this._fnMakePathWithPrefix(<string[]>_EXTERNAL_LIBS_[this.getModeDistribution()], aPrefix, aCutExt));
    }

    getPrismModel(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.prismModel || (this._cache.prismModel = this._fnMakePathWithPrefix(_PRISM_MODEL_, aPrefix, aCutExt));
    }

    getPrismUX(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.prismUX || (this._cache.prismUX = this._fnMakePathWithPrefix(_PRISM_UX_, aPrefix, aCutExt));
    }

    getService(aPrefix: string = "", aCutExt: boolean = true): ifOBJKeyStringArray {
        var disable = this._dataProductSetting.disableService;
        var services = _SERVICE_;
        if (disable && disable.length) {
            for (var i = 0, len = disable.length; i < len; i++) {
                delete services[disable[i]];
            }
        }
        return this._cache.service || (this._cache.service = this._fnMakePathWithPrefixForObjectStringArray(services, aPrefix, aCutExt));
    }

    getComponents(aPrefix: string = "", aCutExt: boolean = true): ifOBJKeyString {
        return this._cache.componenets || (this._cache.componenets = this._fnOverwriteUserData(_COMPONENT_KEY_, this._dataProductSetting.components));
    }

    getEvents(): any {
        return this._cache.events || (this._cache.events = this._fnOverwriteUserData(_EVENT_, this._dataProductSetting.events));
    }

    getLanguageTable(): any {
        return this._cache.languageTable || (this._cache.languageTable = this._fnOverwriteUserData(_LANGUAGE_TABLE_, this._dataProductSetting.languageTable));
    }

    //TODO: have to classify
    getUnclassifiedData(): any {
        var ret = {
            settings: null,
            wzd: null,
            CPgInfoHub: null,
            frontPanel: null,
            help2: null,
            help: null
        };
        ret.settings = this._cache.settings || (this._cache.settings = (this._dataProductSetting.settings || __UNCLASSIFIED_DATA__.settings));
        ret.wzd = this._cache.wzd || (this._cache.wzd = (this._dataProductSetting.wzd || __UNCLASSIFIED_DATA__.wzd));
        ret.CPgInfoHub = this._cache.CPgInfoHub || (this._cache.CPgInfoHub = (this._dataProductSetting.CPgInfoHub || __UNCLASSIFIED_DATA__.CPgInfoHub));
        ret.frontPanel = this._cache.frontPanel || (this._cache.frontPanel = (this._dataProductSetting.frontPanel || __UNCLASSIFIED_DATA__.frontPanel));
        ret.help2 = this._cache.help2 || (this._cache.help2 = (this._dataProductSetting.help2 || __UNCLASSIFIED_DATA__.help2));
        ret.help = this._cache.help || (this._cache.help = (this._dataProductSetting.help || __UNCLASSIFIED_DATA__.help));
        return ret;
    }

    isEmul(): any {
        return _DEFAULT_CONFIG.emulOnBox || this._isEmul;
    }

    getPrismUIFramework(aPrefix: string = "", aCutExt: boolean = true): string[] {
        return this._cache.prismUIFramework || (this._cache.prismUIFramework = this._fnMakePathWithPrefix(_PRISM_UI_FRAMEWORK_SET_, aPrefix, aCutExt));
    }
}
export = cProductConfiguration;
