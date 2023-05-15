///<reference path="../def/require.d.ts" />
///<reference path="../def/sugar.d.ts" />
// file name : fw/common/domain/dlEntity.js
/*jslint sloppy:true, nomen:true, vars:true */
/* domain layer initializer */

declare var hx: ifSugar;

module mDL {
    class cDLServiceReciever implements ifSVC {
        private _madeObjects: string[] = [];
        AIR_PLAY: ifSVCAirPlay = null;
        APP_MGR: ifSVCAppMgr = null;
        AV_CTRL: ifSVCAvCtrl = null;
        BRIDGE: ifSVCBridge = null;
        CAS_CI: ifSVCCasCI = null;
        CAS_IR_EMM_FORCED_OTA: ifSVCCasActionIrEmmForcedOta = null;
        CHANNEL_SEARCH: ifSVCChannelSearch = null;
        CHANNEL_SEARCH_T: ifSVCChannelSearchT = null;
        CHANNEL_SEARCH_S: ifSVCChannelSearchS = null;
        CH_LIST: ifSVCChlist = null;
        DCN: ifSVCDCN = null;
        DETAIL_INFO: ifSVCDetailInfo = null;
        DIAL: ifSVCDial = null;
        DLNA: ifSVCDlna = null;
        DMR: ifSVCDmr = null;
        EPG: ifSVCEPG = null;
        FRONT_PANEL: ifSVCFrontPanel = null;
        HISTORY: ifSVCHistory = null;
        LOGICAL_STORAGE: ifSVCLogicalStorage = null;
        MEDIA_ITEM_MANAGER: ifSVCMediaItemManager = null;
        MEDIA_SERVER_MANAGER: ifSVCMediaServerManager = null;
        PARENTAL_CTRL: ifSVCParentalCtrl = null;
        PLAYBACK: ifSVCPlayback = null;
        PLAYLIST: ifSVCPlaylist = null;
        RECORDINGS: ifSVCRecordings = null;
        SAMBA: ifSVCSamba = null;
        SETTING_UTIL: ifSVCSettingUtil = null;
        SMART_SEARCH: ifSVCSmartSearch = null;
        STATISTICS: ifSVCStatistics = null;
        STORAGE_DLNA: ifSVCStorageDlna = null;
        STORAGE_MASS: ifSVCStorageMass = null;
        STORAGE_SAMBA: ifSVCStorageSamba = null;
        STORAGE_UTIL: ifSVCStorageUtil = null;
        SW_UPDATE: ifSVCSWUpdate = null;
        TSR: ifSVCTsr = null;
        TV_PORTAL: ifSVCTVPortal = null;
        WEB_STORAGE: ifSVCWebStorage = null;
        KEY_EVENT_BINDER: ifSVCKeyEventBinder = null;
        CLOCK: ifSVCClock = null;
        CHANNEL_IMPORT_EXPORT: ifSVCChannelImportExport = null;
        CLOCK_RECOVERY: ifSVCClockRecovery = null;
        IP_EPG = null;
        NETFLIX  : ifSVCNetflix= null;
        VOICE_ASSISTANCE : ifSVCVoiceAssistance = null;

        addService(aServiceName: string, aConstructor: any, aForceReload?: boolean) {
            var svc = this[aServiceName];
            if (svc === undefined) {
                throw "cannot find [" + aServiceName + "] in cDLServiceReceiver";
            }
            if (svc === null || aForceReload) {
                this[aServiceName] = new aConstructor(aServiceName);
                this._madeObjects.push(aServiceName);
            } else {
                throw aServiceName + " already added.";
            }
        }
        isMade(aServiceName) {
            return !!this._madeObjects[aServiceName];
        }
    }
    export class cDLEntity {
        private name: string;

        service: any;
        serviceoff: any;
        dsvc: any;
        svc: ifSVC;

        constructor(aServices: { [key: string]: string[] }, aOnLoad?) {
            this.name = "dlEntity";

            /* TODO
             * aServices may not need to type { [key: string]: string[] }, just string[] OK
             * bglee@humaxdigital.com, 15:39, 2014. 3. 24.
             */
            function fnMakeSVC(aServiceName: string, aConstructor) {
                svcReceiver.addService(aServiceName, aConstructor)
            }
            function _fnLoad(aDoesNotUse: (...aModules: any[]) => void) {
                function __fnLoadFinal(aDoesNotUse: (...aModules: any[]) => void) {
                    var key = keys.shift();
                    fnMakeSVC(key, aDoesNotUse);
                    callback(callback);
                }
                function __fnLoadNext(aDoesNotUse: (...aModules: any[]) => void) {
                    var key = keys.shift();
                    fnMakeSVC(key, aDoesNotUse);

                    var file = scripts.splice(0, 1);
                    console.log('file name - ' + file);
                    require(file, scripts.length ? __fnLoadNext : __fnLoadFinal);
                }
                var file = scripts.splice(0, 1);
                console.log('file name - ' + file);
                require(file, scripts.length ? __fnLoadNext: callback);
            }
            var svcReceiver: ifSVC = new cDLServiceReciever();
            var dservices: ifOBJKeyStringArray = aServices || {};
            var keys = Object.keys(dservices);
            var len = keys.length;
            var scripts: string[] = [];
            var callback = (aDoesNotUse: (...aModules: any[]) => void) => {
                aOnLoad();
            };
            for (var i = 0; i < len; i++) {
                /* COMMENT
                 * it suppose one direct service matching one file. -- keys[i]][0]
                 * * bglee@humaxdigital.com, 16:12, 2014. 3. 24.
                 */
                scripts = scripts.concat(dservices[keys[i]][0]);
            }
            this.svc = svcReceiver;
            //TODO: will be remove hx.service ...
            hx.service = this.service;
            hx.serviceoff = this.serviceoff;
            hx.dsvc = this.dsvc;
            hx.svc = this.svc;
            _fnLoad(callback);
        }
    }
}
export = mDL;