/// <reference path="../def/config.d.ts" />
/**
 * Created by mskang2 on 2014-05-09.
 */
define(["require", "exports", "../def/config.def"], function (require, exports, __DEF__) {
    var config = {
        config: {
            model: 2 /* HDFOXIRCONNECT */,
            op: 4 /* ORF */,
            useCAS: 1 /* IR */,
            useCasOTA: 1 /* IRDETO */,
            useDetachableStorage: 0,
            useTsrOnOff: 1,
            maxSetAntennaSlot: 4,
            useUserDefineSatellite: 1,
            useImportExportChannels: 1,
            maxTuner: 1,
            useInternalPvr: 0,
            useHiddenFunction: 1,
            useHiddenPWReset: 1,
            useDefaultChannels: 0,
            useAlphabetGroup: 1,
            useWakeUpOTA: 1,
            useLiveIPOTA: 0,
            useAutoPowerDown: 1,
            usefrontPanel: 'segment7',
            useMaxScheduleDuration: 1440,
            useDelayedRec: false,
            useWaitBooting: 120,
            use2ndPowerOnOffTimer: true,
            useBackwardEpg: false,
            maxBackwardEpgDuration: 0,
            useRfCRID: false,
            usePrismLiteS: true,
            showSeriesInfo: false,
            durationTSR: 5400,
            isFieldSearch: false,
            useOTANotification: 1,
            useWidget: false,
            useTVStreaming: false,
            standbyPowerMode: 1,
            useUSBWireless: true,
            CONFIG_FCC_NUM: 0,
            useUCRecordingLimit: true
        },
        page: {
            CPgScreenDisplay: 'product/hdfoxirconnect/pages/settings/general_settings/page_screen_display.js',
            CPgLnbSearch: 'pages/settings/channel/page_lnbsearch.js',
            CPgDiseqcSearch: 'pages/settings/channel/page_diseqcsearch.js',
            CPgScdSearch: 'pages/settings/channel/page_scdsearch.js',
            CPgScdSetting: 'pages/settings/channel/page_scdsetting.js',
            CPgSatelliteSetting: 'pages/settings/channel/page_satellitesetting.js',
            CPgSettings: 'product/hdfoxirconnect/pages/settings/page_settings.js',
            CPgHiddenSetting: 'product/hdfoxirconnect/pages/page_hidden.js',
            CPgLanguage: 'product/hdfoxirconnect/pages/settings/general_settings/page_language.js',
            CPgSystem: 'product/hdfoxirconnect/pages/settings/system_settings/page_system_information.js',
            // Softcell - IRDETO
            CPgConditionalAccess: 'product/cas_irdeto/page_conditional_access.js',
            CPgSmartcardStatus: 'product/cas_irdeto/page_smartcard_status.js',
            CPgSoftcellServiceStatus: 'product/cas_irdeto/page_softcell_service_status.js',
            CPgLoaderStatus: 'product/cas_irdeto/page_loader_status.js',
            CPgComponents: 'product/cas_irdeto/page_components.js',
            /*
                    // CCA - IRDETO
                    CPgIrCaStatus : 'product/cas_irdeto/page_cca_irdeto_ca_status.js',
                    CPgServiceStatus : 'product/cas_irdeto/page_cca_service_status.js',
                    CPgCaLoaderStatus : 'product/cas_irdeto/page_cca_loader_status.js',
                    CPgProductStatus : 'product/cas_irdeto/page_cca_product_status.js',
                    CPgClientStatus : 'product/cas_irdeto/page_cca_client_status.js',
            */
            // Mail Messages
            CPgMailMessages: 'product/cas_irdeto/page_mail_messages.js',
            CPgCasIrUiNotifier: 'product/cas_irdeto/page_casIrUiNotifier.js',
            //CPgCasIrUcUiNotifier :'product/cas_irdeto/page_casIrUcUiNotifier.js', // for UCCA
            CPgCasIrMailIcon: 'product/cas_irdeto/page_cas_ir_mail_icon.js'
        },
        components: {
            irErrorMessages: 'pages/msgbox/cmpt_irerrormessages.js'
        },
        settings: {
            systeminfo: {
                title: ['LOC_SYSTEM_INFORMATION_ID'],
                itemId: ['modelname', 'softver', 'loaderver', 'systemid', 'updatedate', 'serialnumber', 'macaddlen', 'macaddwlen', 'macaddbluetooth'],
                itemStr: ['LOC_MODELNAME_ID', 'LOC_SOFTVER_ID', 'LOC_LOADERVER_ID', 'LOC_SYSTEMID_ID', 'LOC_UPDATEDATE_ID', 'LOC_MICOMVER_ID', 'LOC_MACADDR_ID', 'LOC_MACADDR_ID', 'LOC_MACADDR_ID'],
                describe: ['', '', '', '', '', '', '', '', '', '']
            },
            language: {
                title: ['LOC_BIGLANGUAGE_ID'],
                itemId: ['systemlang', 'audiolang', 'subtitlelang'],
                itemStr: ['LOC_SYSTEM_LANGUGE_ID', 'LOC_AUDIO_LANGUAGE_ID', 'LOC_SUBTITLE_LANGUAGE_ID'],
                describe: ['', '', ''],
                itemSub01Str: ['LOC_SET_ENGLISH_ID'],
                itemSub02Str: ['LOC_OFF_ID', 'LOC_SET_ENGLISH_ID', 'LOC_SET_GERMAN_ID', 'LOC_SET_CZECH_ID', 'LOC_SET_SLOVAK_ID'],
                itemSub03Str: ['LOC_OFF_ID', 'LOC_SET_ENGLISH_ID', 'LOC_SET_GERMAN_ID', 'LOC_SET_CZECH_ID', 'LOC_SET_SLOVAK_ID']
            },
            powermanager: {
                title: ['LOC_POWER_MANAGEMENT_ID'],
                itemId: ['autopowerdown', 'powersavingstandby'],
                itemStr: ['LOC_AUTOPOWERDOWN_ID', 'LOC_POWERSAVING_STANDBY_ID'],
                describe: ['', ''],
                itemSub01Str: ['LOC_OFF_ID', 'LOC_ON_ID'],
                itemSub02Str: ['LOC_OFF_ID', 'LOC_ON_ID']
            },
            recording: {
                title: ['LOC_RECORDINGOPTIONS_ID'],
                itemId: ['startpadding', 'endpadding', 'tsr'],
                itemStr: ['LOC_START_PADDING_ID', 'LOC_END_PADDING_ID', 'LOC_TIME_SHIFT_RECORDING_ID'],
                describe: ['', '', ''],
                itemSub01Str: ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID'],
                itemSub02Str: ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID'],
                itemSub03Str: ['LOC_ON_ID', 'LOC_OFF_ID']
            },
            extstorage: {
                "title": [''],
                "itemId": ['removestorage', 'formatstorage', 'selecthddforpvr', 'deselecthddforpvr'],
                "itemStr": ['LOC_REMOVE_STORAGESAFELY_ID', 'LOC_FORMATSTR_ID', 'LOC_STORAGE_SELECT_HDD_FOR_PVR_ID', 'LOC_STORAGE_DESELECT_HDD_FOR_MEDIA_ID'],
                "describe": ['', '', '', ''],
                "itemSub01Str": [],
                "itemSub02Str": [],
                "itemSub03Str": [],
                "itemSub04Str": []
            },
            playbackoption: {
                title: ['LOC_PLAYBACK_OPTIONS_ID'],
                itemId: ['instantreplaytime', 'skipforwardtime'],
                itemStr: ['LOC_INSTANT_REPLAY_TIME_ID', 'LOC_SKIP_FORWARD_TIME_ID'],
                describe: ['', '', ''],
                itemSub01Str: ['LOC_ON_ID', 'LOC_OFF_ID'],
                itemSub02Str: ['LOC_7_SEC_ID', 'LOC_15_SEC_ID', 'LOC_30_SEC_ID'],
                itemSub03Str: ['LOC_30_SEC_ID', 'LOC_60_SEC_ID', 'LOC_120_SEC_ID', 'LOC_240_SEC_ID']
            },
            parentalcontrol: [['LOC_PARENTALCONTROL_ID', 'LOC_CHANGE_PASSWORD_ID', 'LOC_BLOCK_SETTINGS_CHANNEL_ID'], ['LOC_VIEWALL_ID', 'LOC_7_ID', 'LOC_12_ID', 'LOC_15_ID', 'LOC_18_ID', 'LOC_LOCKALL_ID'], ['LOC_OFF_ID', 'LOC_ON_ID']]
        },
        languageTable: {
            eng: __DEF__.eISO639.eng,
            ger: __DEF__.eISO639.ger,
            ces: __DEF__.eISO639.ces,
            slo: __DEF__.eISO639.slo
        },
        bottom_page: ['CPgNotAvailable', 'CPgHotkey', 'CPgLive', 'CPgNotifier', 'CPgRecControl', 'CPgCasIrUiNotifier'],
        keyTable: ["product/hdfoxirconnect/fw/service/keytable/keytable.js"],
        disableService: []
    };
    return config;
});
