// file name : pages/standby/standby.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../def/sugar.d.ts"/>
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __DEF__ = require("def/oipf.def");
declare var hx: ifSugar;

class cPgStandby extends __PAGE_KERNEL__ {
    constructor() {
        super();
    }
    create() {
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
    }
    destroy() {
        hx.log('warning', ' -------------- DESTROY STANDBY PAGE --------------------------');
        hx.log('warning', ' -- ');
        hx.log('warning', ' -----------------------------------------------------------------');

        hx.log('warning', 'call createHbbTVApplication(), in standby.js');
        hx.svc.APP_MGR.createHbbTVApplication();

        this.fnRemoveCallback();

        hx.svc.AV_CTRL.unBlockAV();

        // fling do not have to work when standby
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_alKey(aParam: {alKey: number}) {
        hx.log('CPgStandby', 'key : ' + aParam.alKey);
        switch( aParam.alKey ) {
            case hx.key.KEY_POWER:
                return true;
            default :
                break;
        }
        return true;
    }
    fnGoStandby() {
        hx.log("CPgStandby", "[fnGoStandby] 1");
        // statistics log
        if (hx.svc.STATISTICS) {
            hx.svc.STATISTICS.statisticsLog({
                "CODE" : "SSTAND", //STATUS_STANDBY
                "TRS" : "I"
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
                    'stopAV' : 1,
                    'stopTsr' : 1
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

    }
    fnRestart() {
        // power off timer - start
        hx.svc.SETTING_UTIL.startPowerOffTimer();
        // goto installation wizard or live
        var bFirstTimeBoot: boolean = hx.svc.SETTING_UTIL.getFirstTimeBoot();
        hx.il.isInWizardPage = bFirstTimeBoot;
        hx.log("CPgStandby", "bFirstTimeBoot : " + bFirstTimeBoot);
        if (bFirstTimeBoot === true) {
            var LangObj: {LangMenu: string; LanAudio: string; LangSubTtl: string} = hx.svc.SETTING_UTIL.getLanguage();

            hx.svc.SETTING_UTIL.setLanguage({
                    'LangMenu' : LangObj.LangMenu
            });
            this.sendEventToEntity({
                alEvent : 'alChangeInstallPage',
                target : hx.config.installwzd,
                me : this
            });
        } else {
            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : 'CPgLiveController?startService',
                me : this
            });
            // Boot OTA Check
            if (hx.config.useOTA === "HUMAX") {
                hx.log("CPgStandby", "check the standby humax ota version after 2 seconds. ");
                setTimeout(() => {
                    this.fnCheckStandbyOta(this);
                }, 1000 * 2);
                // after 2 seconds, check the standby OTA
            }
        }
    }
    fnRegisterCallback() {
        // register power on event
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        dsrvSettingUtil.addEventCb('PowerStateChanged', this, (aPowerState) => {
            hx.log('warning', 'onPowerStateChange -- szWakeUpReason is');
            if (aPowerState === __DEF__.POWER_ON) {// it going to normal mode... (POWER_STATE.POWER_ON)
                hx.log('warning', 'onPowerStateChange -- powerState is 1, going to Normal Mode');
                hx.log('debug', '[STANDBY] hx.il.isGoingOperationFromStandby = true;');
                hx.il.isGoingOperationFromStandby = true;
                this.fnRestart();

                hx.svc.SETTING_UTIL.setPowerOn();

                // link s/w update event listener
                var dsrvSWUpdate = hx.svc.SW_UPDATE;
                dsrvSWUpdate.swupdateInit();
            } else {
                hx.log('warning', 'onPowerStateChange -- powerState is ' + aPowerState);
            }
        });
        hx.log("CPgStandby", "[fnRegisterCallback] added PowerStateChanged");
        
        hx.log("CPgStandby", "[fnRegisterCallback] added ChannelChangeSuccess");
        hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, (aParam) => {
            this._on_ChannelChangeSucceeded(aParam);
        });
        hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, (aParam) => {
            this._on_ChannelChangeError(aParam);
        });
        hx.log("CPgStandby", "[fnRegisterCallback] added ChannelChangeError");

        // when received power on timer & remind event
        hx.svc.RECORDINGS.addEventCb('ScheduledWillBeStart', this, (aParam) => {
            var item = aParam.item;

            if (item.isPowerOnTimer()) {
                var retPowerOnInfo = hx.svc.SETTING_UTIL.getPowerOnInfo();
                var retGrp = hx.svc.WEB_STORAGE.getPowerTimerLastGroup();

                hx.svc.WEB_STORAGE.updateLastService({
                        'ccid' : retPowerOnInfo.channel.ccid,
                        'group' : retGrp.grpName
                });
                hx.svc.SETTING_UTIL.setAudioVolume(retPowerOnInfo.volume);
            } else if (item.isReminder()) {
                var scheduleGroup = null;

                // schedule item
                var scheduleCCID = item.getChannelId();
                var o2 = hx.svc.CH_LIST.getAvailableGroupForCcid({
                        'ccid' : scheduleCCID
                });
                if (o2.group) {
                    scheduleGroup = o2.group;
                }

                hx.svc.WEB_STORAGE.updateLastService({
                        'ccid' : scheduleCCID,
                        'group' : scheduleGroup
                });
            }
        });
        hx.log("CPgStandby", "[fnRegisterCallback] added ScheduledWillBeStart");

        hx.svc.RECORDINGS.addEventCb('NotiRecordingStarted', this, (aParam) => {
            var item = aParam.item;
            if((item.isPowerOnTimer && item.isPowerOnTimer()) || (item.isReminder && item.isReminder()))
            {
                hx.log('warning', 'Received NotiRecordingStarted Event. Going to Normal Mode\n');
                    // going to normal
                hx.svc.SETTING_UTIL.standbyState(false);
            }
        });
        hx.log("CPgStandby", "[fnRegisterCallback] added NotiRecordingStarted");
    }
    fnRemoveCallback() {
        hx.svc.SETTING_UTIL.clearEventCb(this);
        hx.svc.RECORDINGS.clearEventCb(this);
        hx.svc.AV_CTRL.clearEventCb(this);
    }
    _on_ChannelChangeSucceeded(o) {
        hx.log('CPgStandby', '_on_ChannelChangeSucceeded !!! ');
        setTimeout(function() {
            hx.svc.AV_CTRL.stopCurChannel({
                    'stopAV' : 1,
                    'stopTsr' : 1
            });
        }, 1);
    }
    _on_ChannelChangeError(o) {
        hx.log('CPgStandby', '_on_ChannelChangeError !!! ');
        setTimeout(function() {
            hx.svc.AV_CTRL.stopCurChannel({
                    'stopAV' : 1,
                    'stopTsr' : 1
            });
        }, 1);
    }

    // PRIVATE
    fnCheckStandbyOta(aParam) {
        var that = aParam;
        var btnOK: string = hx.l('LOC_OK_ID');
        var STR_PREFIX_OTA_VER: string = 'MESIBE';
        var NO_NEW_OTA_VERSION: string = '0.00.00';
        var fHasNewOtaVersion: boolean = false;
        var curVersion: string = "";
        var newVersion: string = "";
        var strQuery: string = "";

        var versionStandbyHumaxOTA = hx.svc.SW_UPDATE.getVerBootupHumaxOta();

        if ( typeof versionStandbyHumaxOTA === "undefined") {
            //        	hx.log('CPgStandby', '[versionStandbyHumaxOTA]: undefined');
            return;
        }

        //        hx.log('CPgStandby', '[versionStandbyHumaxOTA]: ' + versionStandbyHumaxOTA);
        var strVersionArray = versionStandbyHumaxOTA.split(STR_PREFIX_OTA_VER);

        if (strVersionArray.length === 3) {
            /* strVersionArray[0] identifies standby or bootup ota. */
            if (strVersionArray[0] === "NORMAL") {
                strQuery = 'startBootupHumaxOta';
            } else if (strVersionArray[0] === "STANDBY") {
                strQuery = 'startInstallStandbyOta';
            }
            curVersion = STR_PREFIX_OTA_VER + " " + strVersionArray[1].trim();
            newVersion = STR_PREFIX_OTA_VER + " " + strVersionArray[2].trim();
            if (strVersionArray[2].trim() !== NO_NEW_OTA_VERSION && strQuery !== "") {
                fHasNewOtaVersion = true;
            }
        }

        if (fHasNewOtaVersion) {// there is ota flag
            var yes: string = hx.l('LOC_YES_ID'), no: string = hx.l('LOC_NO_ID');
            var forcedOtaConfirm = hx.msg('ssuDialog', {
                title : hx.l('LOC_SOFTWAREUPDATE_ID'),
                text : hx.l('LOC_NEWSWDETECTED_ID'),
                text1 : hx.l('LOC_DOYOUWANTUPDATE_ID'),
                first_line_arr : [hx.l('LOC_CURSWVERSION_ID'), curVersion],
                second_line_arr : [hx.l('LOC_NEWSWVERSION_ID'), newVersion],
                auto_close : true,
                timeout : 30000, // 30 seconds time out, automatically ota starts according to scenario.
                btn_title_arr : [yes, no],
                dia_class : 'osd',
                callback_fn : function(_status) {
                    if (_status === yes || _status === 'auto_close') {
                        hx.msg.close('confirm', forcedOtaConfirm);
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings?set=' + strQuery,
                            me : that
                        });
                    }
                }
            });
        }
    }
}
export = cPgStandby;