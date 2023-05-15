/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/prism.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvNetflix.js
/*jslint nomen:true, vars:true */
//    NETFLIX: ['layer_domain/service/dl_dsrvNetflix.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLNetflix = (function (_super) {
        __extends(cDLNetflix, _super);
        function cDLNetflix(aServiceName) {
            _super.call(this, aServiceName);
            this.runningState = cDLNetflix.RUNNING_STATE.STOPPED;
            this.requestLock = false;
            this.bOpenPW = false;
            this.recoveryTimerId = null;
            if (hx.config.netflix === true) {
                this._loadNetflixService();
            }
        }
        cDLNetflix.prototype.start = function (param) {
            var type = param.type;
            var url = param.url;
            if (this.runningState === cDLNetflix.RUNNING_STATE.RUNNING || this.requestLock) {
                hx.log('info', 'netflix is already running(' + this.runningState + ') or requested(' + this.requestLock + ')!');
                return;
            }
            if (hx.config.useNetflixPW) {
                var isPassedPW = param.passedPW; //from Netflix password
                var guidancePolicy = hx.svc.SETTING_UTIL.getGuidancePolicy();
                hx.log("info", "[dsrvNetflix] guidancePolicy ::" + guidancePolicy + ", type : " + type);
                if (guidancePolicy !== 0) {
                    if (isPassedPW === undefined) {
                        //not from PW
                        if (this.bOpenPW === false) {
                            hx.log("info", "[dsrvNetflix][start] ShowNetflixPW !");
                            this.fireEvent('ShowNetflixPW', type);
                            this.bOpenPW = true;
                        }
                        else {
                            hx.log("info", "[dsrvNetflix][start] return due to opened password !");
                        }
                        return;
                    }
                    else {
                        hx.log("info", "[dsrvNetflix][start] passedPW !");
                    }
                }
            }
            var needChangingLC = this.needChangingLiveController(type);
            hx.log('info', '[Netflix start] needChangingLC ::' + needChangingLC);
            if (needChangingLC === true) {
                this.fireEvent('ChangeCPgLiveController', cDLNetflix.CTRL_TYPE.START);
            }
            if (url) {
                hx.log('info', 'startWithTypeUrl(' + type + ', ' + url + ')');
                this.netflixPlugin.startWithTypeUrl(type, url);
            }
            else {
                hx.log('info', 'startWithType(' + type + ')');
                hx.log('info', 'netflixPlugin exist: ' + !!this.netflixPlugin + ", startWithType exist: " + !!this.netflixPlugin.startWithType);
                this.netflixPlugin.startWithType(type);
            }
            if (this.bOpenPW) {
                this.bOpenPW = false;
            }
            this._requestLock();
        };
        cDLNetflix.prototype.stop = function (reasonCode) {
            if (this.runningState === cDLNetflix.RUNNING_STATE.STOPPED || this.requestLock) {
                hx.log('info', 'netflix is already stopped(' + this.runningState + ') or requested(' + this.requestLock + ')!');
                if (reasonCode !== cDLNetflix.STOP_REASON.Reason_USER_Kill) {
                    hx.log('info', 'but.. force to terminate netflix process!! by USER_Kill');
                    // force to terminate netflix process!! by USER_Kill
                    return;
                }
            }
            hx.log('info', 'stopWithReason(' + reasonCode + ')');
            this.netflixPlugin.stopWithReason(reasonCode);
            this._requestLock();
        };
        cDLNetflix.prototype.notifyOverlayState = function (state) {
            if (this.runningState === cDLNetflix.RUNNING_STATE.RUNNING) {
                hx.log('info', 'notifyOverlayState(' + state + ')');
                this.netflixPlugin.notifyOverlayState(state);
            }
        };
        cDLNetflix.prototype.isRunning = function () {
            return this.runningState === cDLNetflix.RUNNING_STATE.RUNNING;
        };
        cDLNetflix.prototype.getThumbnail = function () {
            return cDLNetflix.CHANNEL_THUMB;
        };
        cDLNetflix.prototype.setPwStatus = function (bOn) {
            if (!hx.config.useNetflixPW) {
                return;
            }
            hx.log("dsrcNetflix", "[setPwStatus] bOn : " + bOn);
            this.bOpenPW = bOn;
        };
        cDLNetflix.prototype.getPwStatus = function () {
            if (!hx.config.useNetflixPW) {
                return;
            }
            hx.log("dsrcNetflix", "[getPwStatus] isOpenedBarkerPW : " + this.bOpenPW);
            return this.bOpenPW;
        };
        cDLNetflix.prototype.needChangingLiveController = function (type) {
            switch (type) {
                case cDLNetflix.START_TYPE.NETFLIX_BUTTON:
                case cDLNetflix.START_TYPE.DIAL:
                case cDLNetflix.START_TYPE.DEDICATED_ON_SCREEN_ICON:
                    return true;
                default:
                    return false;
            }
        };
        //
        // private functions
        //
        cDLNetflix.prototype._requestLock = function () {
            var _this = this;
            this.requestLock = true;
            if (this.recoveryTimerId) {
                clearTimeout(this.recoveryTimerId);
                this.recoveryTimerId = null;
            }
            this.recoveryTimerId = setTimeout(function () {
                _this.recoveryTimerId = null;
                hx.log('info', "[Netflix] request Timeout! recover your request!");
                _this.requestLock = false;
            }, cDLNetflix.REQUEST_LOCK_RECOVERY_TIME);
        };
        cDLNetflix.prototype._preProcess = function (status) {
            hx.log('info', "[netflix] preProcess for netflix!");
            this.runningState = status;
            hx.svc.APP_MGR.destroyHbbTVApplication();
            hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
            hx.svc.BRIDGE.requestStopPlayback();
            this.fireEvent('HideChInfo');
            hx.svc.AV_CTRL.blockAV({
                'cb': function () {
                }
            });
            //Dial Update
            hx.svc.DIAL.updateApplicationState("Netflix", cDLNetflix.DIAL_STATE.RUNNING);
        };
        cDLNetflix.prototype._postProcess = function (status) {
            hx.log('info', "[netflix] postProcess for netflix! status: " + status);
            hx.svc.AV_CTRL.unBlockAV();
            if (status !== cDLNetflix.RUNNING_STATE.STOPPED_BY_DIAL_OTHER) {
                //for no sdb
                this.fireEvent('ChangeCPgLiveController', cDLNetflix.CTRL_TYPE.EXIT);
            }
            hx.svc.DIAL.updateApplicationState("Netflix", cDLNetflix.DIAL_STATE.STOPPED);
            switch (status) {
                case cDLNetflix.RUNNING_STATE.STOPPED_BY_CH_UP:
                    break;
                case cDLNetflix.RUNNING_STATE.STOPPED_BY_CH_DOWN:
                    break;
                case cDLNetflix.RUNNING_STATE.STOPPED_BY_DIAL_OTHER:
                    hx.log("info", "[nexflix][postProcess] STOPPED_BY_DIAL_OTHER !!!!!!!!!!!!!!!! ");
                    return;
                default:
                    var lastChannel = hx.svc.AV_CTRL.lastChannel();
                    if (!lastChannel) {
                        hx.svc.AV_CTRL.availableChannel();
                    }
                    break;
            }
            hx.svc.APP_MGR.createHbbTVApplication(true);
        };
        cDLNetflix.prototype._loadNetflixService = function () {
            var _this = this;
            /*
             local cache
             */
            this.netflixPlugin = this.getPluginInstance('Netflix');
            /*
             interface
             */
            this.defineEvents(['NetflixStatusChanged', 'HideChInfo', 'ChangeCPgLiveController', 'ShowNetflixPW']);
            var goToStart = function (status) {
                return _this.runningState !== cDLNetflix.RUNNING_STATE.RUNNING && status === cDLNetflix.RUNNING_STATE.RUNNING;
            };
            var goToStop = function (status) {
                return _this.runningState === cDLNetflix.RUNNING_STATE.RUNNING && status !== cDLNetflix.RUNNING_STATE.RUNNING;
            };
            var onRegisterStatusChanged = function (status) {
                hx.log('info', 'netflix registerStatusChanged ::: ' + status);
                if (goToStart(status)) {
                    _this._preProcess(status);
                }
                else if (goToStop(status)) {
                    _this._postProcess(status);
                }
                hx.log('info', 'netflix registerStatusChanged almost!!!! ' + status);
                _this.runningState = status;
                _this.requestLock = false;
                _this.fireEvent('NetflixStatusChanged', status);
            };
            this.netflixPlugin.registerStatusChanged(onRegisterStatusChanged);
        };
        cDLNetflix.MODULE_NAME = 'dsrvNetflix';
        cDLNetflix.BARKER_CHANNEL_NAME = 'Netflix';
        cDLNetflix.BARKER_CHANNEL_LCN = 366;
        cDLNetflix.BARKER_CHANNEL_CCID = "ccid://netflix";
        cDLNetflix.TV_GROUP = "TV";
        cDLNetflix.REQUEST_LOCK_RECOVERY_TIME = 10 * 1000; // 10 seconds
        cDLNetflix.CHANNEL_THUMB = "images/616_Netflix_Thumb01.png";
        cDLNetflix.CHANNEL_LOGO = "images/616_Netflix_Thumb03.png";
        cDLNetflix.START_TYPE = {
            NETFLIX_BUTTON: 1,
            DEDICATED_ON_SCREEN_ICON: 2,
            ITEM_IN_APPLICATION_LIST: 3,
            SEARCH_RESULT: 4,
            SEARCH_CONTINUATION: 5,
            WEB_BROWSER: 6,
            LIVE_FOLDER: 7,
            EXTERNAL_APP: 8,
            iBANNER_AD: 9,
            EXTERNAL_CONTROL_PROTOCOL: 10,
            META_DISCOVERY: 11,
            DIAL: 12,
            VOICE_CONTROL: 13,
            VISUAL_GESTURE: 14,
            TOUCH_GESTURE: 15,
            VIRTUAL_REMOTE: 16,
            EPG_GRID: 17,
            CHANNEL_NUMBER: 18,
            POWER_ON_FROM_NETFLIX_BUTTON: 19,
            DIAL_3RD_PARTY: 20,
            TITLE_RECOMMEND: 21,
            SUSPENDED_AT_POWER_ON: 22,
            SUSPENDED_AFTER_APP_RESTART: 23,
            CHANNEL_SURF: 25,
            CHANNEL_INFO_BAR: 26,
            TESTING: 99
        };
        cDLNetflix.RUNNING_TYPE = {
            STOPPED: 0,
            RUNNING: 1,
            STOPPED_BY_STANDBY: 7,
            STOPPED_BY_CH_UP: 8,
            STOPPED_BY_CH_DOWN: 9,
            STOPPED_BY_DIAL: 10,
            STOPPED_BY_DIAL_OTHER: 11
        };
        cDLNetflix.STOP_REASON_TYPE = {
            Reason_USER_Kill: 1,
            Reason_USER_Nav: 2,
            Reason_DIAL_Stop: 3,
            Reason_AM_LowResource: 4,
            Reason_DIAL_Other_Launch: 7,
            Reason_Unknown: 98,
            Reason_Testing: 99
        };
        cDLNetflix.OVERLAY_POPUP_STATE = {
            OPEN_WITH_KEY: 0,
            OPEN_WITHOUT_KEY: 1,
            CLOSE_WITH_NETFLIX_EXIT: 2,
            CLOSE: 3
        };
        cDLNetflix.CATEGORY_TYPE = {
            BASIC_SD: 0,
            BASIC_HD: 1,
            MOVIES_SD: 2,
            MOVIES_HD: 3,
            CHILDREN_SD: 4,
            CHILDREN_HD: 5,
            LEARNING_SD: 6,
            LEARNING_HD: 7,
            INDIE_SD: 8,
            INDIE_HD: 9,
            OTHER: 10
        };
        cDLNetflix.CTRL_TYPE = {
            START: 0,
            EXIT: 1
        };
        cDLNetflix.STOP_REASON = cDLNetflix.STOP_REASON_TYPE;
        cDLNetflix.RUNNING_STATE = cDLNetflix.RUNNING_TYPE;
        cDLNetflix.DIAL_STATE = {
            STOPPED: 0,
            RUNNING: 1
        };
        return cDLNetflix;
    })(__KERNEL__);
    return cDLNetflix;
});
