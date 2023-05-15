var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "def/oipf.def"], function (require, exports, __PAGE_KERNEL__, __DEF__) {
    var cPgStandby = (function (_super) {
        __extends(cPgStandby, _super);
        function cPgStandby() {
            _super.call(this);
        }
        cPgStandby.prototype.create = function () {
            hx.al.compose(this);
            hx.logadd('CPgStandby');
            hx.log('warning', ' -------------- CREATE STANDBY PAGE --------------------------');
            hx.log('warning', ' -- ');
            hx.log('warning', ' -----------------------------------------------------------------');
            hx.svc.SETTING_UTIL.setPowerOff();
            hx.log('warning', ' [CPgStandby]-dsrvSettingUtil setPowerOff !!-----------------------');
            // fling do not have to work when standby
            hx.svc.BRIDGE.enablePlayback(false);
            hx.svc.DMR.setMedia(null);
            hx.log('warning', ' [CPgStandby]-go stand by');
            this.fnGoStandby();
            // register event callback
            hx.log('warning', ' [CPgStandby]-fn register callback');
            this.fnRegisterCallback();
        };
        cPgStandby.prototype.destroy = function () {
            hx.log('warning', ' -------------- DESTROY STANDBY PAGE --------------------------');
            hx.log('warning', ' -- ');
            hx.log('warning', ' -----------------------------------------------------------------');
            hx.log('warning', 'call createHbbTVApplication(), in standby.js');
            hx.svc.APP_MGR.createHbbTVApplication();
            this.fnRemoveCallback();
            hx.svc.AV_CTRL.unBlockAV();
            // fling do not have to work when standby
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cPgStandby.prototype._on_alKey = function (aParam) {
            hx.log('CPgStandby', 'key : ' + aParam.alKey);
            switch (aParam.alKey) {
                case hx.key.KEY_POWER:
                    return true;
                default:
                    break;
            }
            return true;
        };
        cPgStandby.prototype.fnGoStandby = function () {
            hx.log("CPgStandby", "[fnGoStandby] 1");
            // statistics log
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE": "SSTAND",
                    "TRS": "I"
                });
                hx.svc.STATISTICS.transmitLog();
            }
            hx.log("CPgStandby", "[fnGoStandby] 2");
            // close active child app
            hx.svc.TV_PORTAL.destroyActiveApp();
            hx.log("CPgStandby", "[fnGoStandby] 3");
            // close HBBTV
            hx.svc.APP_MGR.destroyHbbTVApplication();
            // close Apps
            hx.svc.APP_MGR.destroyAllApps();
            hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
            hx.log("CPgStandby", "[fnGoStandby] 4");
            // stop AV
            var o = hx.svc.AV_CTRL.isChannelPlaying();
            if (o === true) {
                hx.svc.AV_CTRL.stopCurChannel({
                    'stopAV': 1,
                    'stopTsr': 1
                });
            }
            hx.log("CPgStandby", "[fnGoStandby] 5");
            // stop PB
            hx.svc.BRIDGE.requestStopPlayback();
            hx.log("CPgStandby", "[fnGoStandby] 6");
            // clear back key service
            hx.svc.WEB_STORAGE.clearBackService();
            hx.log("CPgStandby", "[fnGoStandby] 7");
            // power off timer - stop
            hx.svc.SETTING_UTIL.stopPowerOffTimer();
        };
        cPgStandby.prototype.fnRestart = function () {
            var _this = this;
            // power off timer - start
            hx.svc.SETTING_UTIL.startPowerOffTimer();
            // goto installation wizard or live
            var bFirstTimeBoot = hx.svc.SETTING_UTIL.getFirstTimeBoot();
            hx.il.isInWizardPage = bFirstTimeBoot;
            hx.log("CPgStandby", "bFirstTimeBoot : " + bFirstTimeBoot);
            if (bFirstTimeBoot === true) {
                var LangObj = hx.svc.SETTING_UTIL.getLanguage();
                hx.svc.SETTING_UTIL.setLanguage({
                    'LangMenu': LangObj.LangMenu
                });
                this.sendEventToEntity({
                    alEvent: 'alChangeInstallPage',
                    target: hx.config.installwzd,
                    me: this
                });
            }
            else {
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgLiveController?startService',
                    me: this
                });
                // Boot OTA Check
                if (hx.config.useOTA === "HUMAX") {
                    hx.log("CPgStandby", "check the standby humax ota version after 2 seconds. ");
                    setTimeout(function () {
                        _this.fnCheckStandbyOta(_this);
                    }, 1000 * 2);
                }
            }
        };
        cPgStandby.prototype.fnRegisterCallback = function () {
            var _this = this;
            // register power on event
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            dsrvSettingUtil.addEventCb('PowerStateChanged', this, function (aPowerState) {
                hx.log('warning', 'onPowerStateChange -- szWakeUpReason is');
                if (aPowerState === __DEF__.POWER_ON) {
                    hx.log('warning', 'onPowerStateChange -- powerState is 1, going to Normal Mode');
                    hx.log('debug', '[STANDBY] hx.il.isGoingOperationFromStandby = true;');
                    hx.il.isGoingOperationFromStandby = true;
                    _this.fnRestart();
                    hx.svc.SETTING_UTIL.setPowerOn();
                    // link s/w update event listener
                    var dsrvSWUpdate = hx.svc.SW_UPDATE;
                    dsrvSWUpdate.swupdateInit();
                }
                else {
                    hx.log('warning', 'onPowerStateChange -- powerState is ' + aPowerState);
                }
            });
            hx.log("CPgStandby", "[fnRegisterCallback] added PowerStateChanged");
            hx.log("CPgStandby", "[fnRegisterCallback] added ChannelChangeSuccess");
            hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, function (aParam) {
                _this._on_ChannelChangeSucceeded(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, function (aParam) {
                _this._on_ChannelChangeError(aParam);
            });
            hx.log("CPgStandby", "[fnRegisterCallback] added ChannelChangeError");
            // when received power on timer & remind event
            hx.svc.RECORDINGS.addEventCb('ScheduledWillBeStart', this, function (aParam) {
                var item = aParam.item;
                if (item.isPowerOnTimer()) {
                    var retPowerOnInfo = hx.svc.SETTING_UTIL.getPowerOnInfo();
                    var retGrp = hx.svc.WEB_STORAGE.getPowerTimerLastGroup();
                    hx.svc.WEB_STORAGE.updateLastService({
                        'ccid': retPowerOnInfo.channel.ccid,
                        'group': retGrp.grpName
                    });
                    hx.svc.SETTING_UTIL.setAudioVolume(retPowerOnInfo.volume);
                }
                else if (item.isReminder()) {
                    var scheduleGroup = null;
                    // schedule item
                    var scheduleCCID = item.getChannelId();
                    var o2 = hx.svc.CH_LIST.getAvailableGroupForCcid({
                        'ccid': scheduleCCID
                    });
                    if (o2.group) {
                        scheduleGroup = o2.group;
                    }
                    hx.svc.WEB_STORAGE.updateLastService({
                        'ccid': scheduleCCID,
                        'group': scheduleGroup
                    });
                }
            });
            hx.log("CPgStandby", "[fnRegisterCallback] added ScheduledWillBeStart");
            hx.svc.RECORDINGS.addEventCb('NotiRecordingStarted', this, function (aParam) {
                var item = aParam.item;
                if ((item.isPowerOnTimer && item.isPowerOnTimer()) || (item.isReminder && item.isReminder())) {
                    hx.log('warning', 'Received NotiRecordingStarted Event. Going to Normal Mode\n');
                    // going to normal
                    hx.svc.SETTING_UTIL.standbyState(false);
                }
            });
            hx.log("CPgStandby", "[fnRegisterCallback] added NotiRecordingStarted");
        };
        cPgStandby.prototype.fnRemoveCallback = function () {
            hx.svc.SETTING_UTIL.clearEventCb(this);
            hx.svc.RECORDINGS.clearEventCb(this);
            hx.svc.AV_CTRL.clearEventCb(this);
        };
        cPgStandby.prototype._on_ChannelChangeSucceeded = function (o) {
            hx.log('CPgStandby', '_on_ChannelChangeSucceeded !!! ');
            setTimeout(function () {
                hx.svc.AV_CTRL.stopCurChannel({
                    'stopAV': 1,
                    'stopTsr': 1
                });
            }, 1);
        };
        cPgStandby.prototype._on_ChannelChangeError = function (o) {
            hx.log('CPgStandby', '_on_ChannelChangeError !!! ');
            setTimeout(function () {
                hx.svc.AV_CTRL.stopCurChannel({
                    'stopAV': 1,
                    'stopTsr': 1
                });
            }, 1);
        };
        // PRIVATE
        cPgStandby.prototype.fnCheckStandbyOta = function (aParam) {
            var that = aParam;
            var btnOK = hx.l('LOC_OK_ID');
            var STR_PREFIX_OTA_VER = 'MESIBE';
            var NO_NEW_OTA_VERSION = '0.00.00';
            var fHasNewOtaVersion = false;
            var curVersion = "";
            var newVersion = "";
            var strQuery = "";
            var versionStandbyHumaxOTA = hx.svc.SW_UPDATE.getVerBootupHumaxOta();
            if (typeof versionStandbyHumaxOTA === "undefined") {
                //        	hx.log('CPgStandby', '[versionStandbyHumaxOTA]: undefined');
                return;
            }
            //        hx.log('CPgStandby', '[versionStandbyHumaxOTA]: ' + versionStandbyHumaxOTA);
            var strVersionArray = versionStandbyHumaxOTA.split(STR_PREFIX_OTA_VER);
            if (strVersionArray.length === 3) {
                /* strVersionArray[0] identifies standby or bootup ota. */
                if (strVersionArray[0] === "NORMAL") {
                    strQuery = 'startBootupHumaxOta';
                }
                else if (strVersionArray[0] === "STANDBY") {
                    strQuery = 'startInstallStandbyOta';
                }
                curVersion = STR_PREFIX_OTA_VER + " " + strVersionArray[1].trim();
                newVersion = STR_PREFIX_OTA_VER + " " + strVersionArray[2].trim();
                if (strVersionArray[2].trim() !== NO_NEW_OTA_VERSION && strQuery !== "") {
                    fHasNewOtaVersion = true;
                }
            }
            if (fHasNewOtaVersion) {
                var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
                var forcedOtaConfirm = hx.msg('ssuDialog', {
                    title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                    text: hx.l('LOC_NEWSWDETECTED_ID'),
                    text1: hx.l('LOC_DOYOUWANTUPDATE_ID'),
                    first_line_arr: [hx.l('LOC_CURSWVERSION_ID'), curVersion],
                    second_line_arr: [hx.l('LOC_NEWSWVERSION_ID'), newVersion],
                    auto_close: true,
                    timeout: 30000,
                    btn_title_arr: [yes, no],
                    dia_class: 'osd',
                    callback_fn: function (_status) {
                        if (_status === yes || _status === 'auto_close') {
                            hx.msg.close('confirm', forcedOtaConfirm);
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings?set=' + strQuery,
                                me: that
                            });
                        }
                    }
                });
            }
        };
        return cPgStandby;
    })(__PAGE_KERNEL__);
    return cPgStandby;
});
