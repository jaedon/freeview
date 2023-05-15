///<reference path="../def/require.d.ts" />
///<reference path="../def/sugar.d.ts" />
// file name : fw/common/domain/dlEntity.js
/*jslint sloppy:true, nomen:true, vars:true */
/* domain layer initializer */
define(["require", "exports"], function (require, exports) {
    var mDL;
    (function (mDL) {
        var cDLServiceReciever = (function () {
            function cDLServiceReciever() {
                this._madeObjects = [];
                this.AIR_PLAY = null;
                this.APP_MGR = null;
                this.AV_CTRL = null;
                this.BRIDGE = null;
                this.CAS_CI = null;
                this.CAS_IR_EMM_FORCED_OTA = null;
                this.CHANNEL_SEARCH = null;
                this.CHANNEL_SEARCH_T = null;
                this.CHANNEL_SEARCH_S = null;
                this.CH_LIST = null;
                this.DCN = null;
                this.DETAIL_INFO = null;
                this.DIAL = null;
                this.DLNA = null;
                this.DMR = null;
                this.EPG = null;
                this.FRONT_PANEL = null;
                this.HISTORY = null;
                this.LOGICAL_STORAGE = null;
                this.MEDIA_ITEM_MANAGER = null;
                this.MEDIA_SERVER_MANAGER = null;
                this.PARENTAL_CTRL = null;
                this.PLAYBACK = null;
                this.PLAYLIST = null;
                this.RECORDINGS = null;
                this.SAMBA = null;
                this.SETTING_UTIL = null;
                this.SMART_SEARCH = null;
                this.STATISTICS = null;
                this.STORAGE_DLNA = null;
                this.STORAGE_MASS = null;
                this.STORAGE_SAMBA = null;
                this.STORAGE_UTIL = null;
                this.SW_UPDATE = null;
                this.TSR = null;
                this.TV_PORTAL = null;
                this.WEB_STORAGE = null;
                this.KEY_EVENT_BINDER = null;
                this.CLOCK = null;
                this.CHANNEL_IMPORT_EXPORT = null;
                this.CLOCK_RECOVERY = null;
                this.IP_EPG = null;
                this.NETFLIX = null;
                this.VOICE_ASSISTANCE = null;
            }
            cDLServiceReciever.prototype.addService = function (aServiceName, aConstructor, aForceReload) {
                var svc = this[aServiceName];
                if (svc === undefined) {
                    throw "cannot find [" + aServiceName + "] in cDLServiceReceiver";
                }
                if (svc === null || aForceReload) {
                    this[aServiceName] = new aConstructor(aServiceName);
                    this._madeObjects.push(aServiceName);
                }
                else {
                    throw aServiceName + " already added.";
                }
            };
            cDLServiceReciever.prototype.isMade = function (aServiceName) {
                return !!this._madeObjects[aServiceName];
            };
            return cDLServiceReciever;
        })();
        var cDLEntity = (function () {
            function cDLEntity(aServices, aOnLoad) {
                this.name = "dlEntity";
                /* TODO
                 * aServices may not need to type { [key: string]: string[] }, just string[] OK
                 * bglee@humaxdigital.com, 15:39, 2014. 3. 24.
                 */
                function fnMakeSVC(aServiceName, aConstructor) {
                    svcReceiver.addService(aServiceName, aConstructor);
                }
                function _fnLoad(aDoesNotUse) {
                    function __fnLoadFinal(aDoesNotUse) {
                        var key = keys.shift();
                        fnMakeSVC(key, aDoesNotUse);
                        callback(callback);
                    }
                    function __fnLoadNext(aDoesNotUse) {
                        var key = keys.shift();
                        fnMakeSVC(key, aDoesNotUse);
                        var file = scripts.splice(0, 1);
                        console.log('file name - ' + file);
                        require(file, scripts.length ? __fnLoadNext : __fnLoadFinal);
                    }
                    var file = scripts.splice(0, 1);
                    console.log('file name - ' + file);
                    require(file, scripts.length ? __fnLoadNext : callback);
                }
                var svcReceiver = new cDLServiceReciever();
                var dservices = aServices || {};
                var keys = Object.keys(dservices);
                var len = keys.length;
                var scripts = [];
                var callback = function (aDoesNotUse) {
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
            return cDLEntity;
        })();
        mDL.cDLEntity = cDLEntity;
    })(mDL || (mDL = {}));
    return mDL;
});
