// file name : pages/start/start.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../def/sugar.d.ts"/>
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: ifSugar;
declare var prism;

class cPgStart extends __PAGE_KERNEL__ {
    _message: any;
    _article: any;
    $wrap: JQuery;
    constructor() {
        super();
    }
    create() {
        hx.logadd('CPgStart');
        hx.log('CPgStart', 'create');

        //hx.log('CPgStart', '----------- image preloading start -----------');
        /**
         *	image preloading
         */
        $.imgPreload({
            images : [//
                './images/105_BG_COM_Context_L01.png'//
                , './images/105_BG_COM_Context_L02.png'//
                , './images/105_BG_COM_Context_R01.png'//
                , './images/105_BG_EPG_Quick.png'//
                , './images/105_BG_EPG_Title.png'//
                , './images/106_Arrow_Left.png'//
                , './images/106_Arrow_Right.png'//
                , './images/107_Line_Context_L.png'//
                , './images/107_Line_Context_R.png'//
                , './images/107_Line_EPG_Qnav_C.png'//
                , './images/107_Line_EPG_Qnav_N.png'//
                , './images/614_Thumb_TV.jpg'
                , './images/614_Thumb_REC.jpg'
                , './images/614_Thumb_REC_Fail.jpg'
                , './images/614_Thumb_TV.jpg'
                , './images/614_Thumb_Radio.jpg'
                , './images/614_OD_Default01.png'
                , './images/614_OD_Default02.png'
                , './images/614_Thumb_Movies.jpg'
                , './images/614_Thumb_News.jpg'
                , './images/614_Thumb_Entertainment.jpg'
                , './images/614_Thumb_Sport1.jpg'
                , './images/614_Thumb_Children.jpg'
                , './images/614_Thumb_Entertainment.jpg'
                , './images/614_Thumb_News.jpg'
                , './images/614_Thumb_Education.jpg'
                , './images/614_Thumb_Lifestyle.jpg'
                , './images/614_Thumb_Drama.jpg'
                , './images/614_Thumb_Genre.jpg'
            ]
        });
        //hx.log('CPgStart', '----------- image preloading end ------------');

        this.fnMakeDom();

        hx.al.compose(this);
        hx.svc.APP_MGR.registerGlobalKeyCapture([hx.key.KEY_POWER, hx.key.KEY_ESCAPE, hx.key.KEY_MENU, hx.key.KEY_LIST],
            function(keyCode) {
                var eventObj;
                if (document.createEventObject) {
                    eventObj = document.createEventObject();
                    eventObj.keyCode = keyCode;
                    document.body.fireEvent("onkeydown", eventObj);
                } else if (document.createEvent) {
                    eventObj = document.createEvent("Events");
                    eventObj.initEvent("keydown", true, true);
                    eventObj.which = keyCode;
                    document.body.dispatchEvent(eventObj);
                }
            }
        );

        /* To add listners */
        this._addListeners();

        /**************temporary code ********/
        var dsrvRecordings = hx.svc.RECORDINGS;
        dsrvRecordings.setTargetStorage({
            //INTHDD
            storage : 1
        });
        /*************************************/

        var webReadyTimeout: number = 0;

        // power mode
        var powerValueObj = hx.svc.SETTING_UTIL.getPowerStateValues();
        hx.log('warning', ' -----------------------------------------------------------------');
        hx.log('warning', ' -- curPowerState : [' + powerValueObj.curPowerState + ']');
        hx.log('warning', ' -- previousPowerState : [' + powerValueObj.previousPowerState + ']');
        hx.log('warning', ' -- timeCurrentPowerState : [' + powerValueObj.timeCurrentPowerState + ']');
        hx.log('warning', ' -- wakeupReason : [' + powerValueObj.wakeupReason + ']');
        hx.log('warning', ' -----------------------------------------------------------------');
        if (1 !== powerValueObj.curPowerState) {
            // goto standby
            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : 'CPgStandby',
                me : this
            });
            webReadyTimeout = 2000;
            // prevent standby key issue when boot of active standby on.
        } else {
            // normal boot
            this._start();
            webReadyTimeout = 0;
        }

        // web ready - clear start logo
        setTimeout(function() {
            hx.svc.SETTING_UTIL.setApplicationReady();
            hx.log('CPgStart', '!!!set web ready!!!');
        }, webReadyTimeout);
    }
    destroy() {
        this.fnRemoveDom();
    }
    _addListeners() {
    }
    _start() {
        hx.svc.SETTING_UTIL.setPowerOn();
        // auto power down - start
        /*
         hx.svc.SETTING_UTIL.startAutoPowerDown();
         */
        // power off timer - start
        hx.svc.SETTING_UTIL.startPowerOffTimer();

        // link s/w update event listener
        var dsrvSWUpdate = hx.svc.SW_UPDATE;
        dsrvSWUpdate.swupdateInit();

        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        dsrvSettingUtil.setNtpServer('pool.ntp.org');

        var bFirstTimeBoot: boolean = hx.svc.SETTING_UTIL.getFirstTimeBoot();
        hx.il.isInWizardPage = bFirstTimeBoot;
        hx.log("CPgStart", "bFirstTimeBoot : " + bFirstTimeBoot);
        if (bFirstTimeBoot === true) {
            if (ENV.listModel.HMS1000T === ENV.model) {
                hx.svc.SETTING_UTIL.setRegionId(hx.config.region);
            } else {
                hx.svc.SETTING_UTIL.setLanguage({
                        LangMenu : hx.config.lang
                });
            }
            hx.svc.SETTING_UTIL.setStandbyPowerMode(hx.config.standbyPowerMode);
            this.sendEventToEntity({
                alEvent : 'alChangeInstallPage',
                target : hx.config.installwzd,
                me : this
            });
        } else {
            this.fnWaitingBoot();
        }
    }
    // EVENT HANDLER
    _on_dlAppMgr(param) {
        hx.log('CPgStart', 'show application !!' + param.appMgrData);
    }
    // PRIVATE
    _checkBootuptimeOta(aParam) {
        var STR_PREFIX_OTA_VER: string = 'MESIBE';
        var NO_NEW_OTA_VERSION: string = '0.00.00';
        var fHasNewOtaVersion: boolean = false;
        var curVersion: string = "";
        var newVersion: string = "";
        var strQuery: string = "";

        var versionBootupHumaxOTA = hx.svc.SW_UPDATE.getVerBootupHumaxOta();

        if ( typeof versionBootupHumaxOTA === "undefined") {
            //       hx.log('CPgStart', '[versionBootupHumaxOTA]: undefined');
            return;
        }

        //        hx.log('CPgStart', '[versionBootupHumaxOTA]: ' + versionBootupHumaxOTA);
        var strVersionArray = versionBootupHumaxOTA.split(STR_PREFIX_OTA_VER);

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

        if (fHasNewOtaVersion) {
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
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
                callback_fn : (_status) => {
                    if (_status === yes || _status === 'auto_close') {
                        hx.msg.close('confirm', forcedOtaConfirm);
                        aParam.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings?set=' + strQuery,
                            me : aParam
                        });
                    }
                    $('#DIA.osd').removeClass('osd');
                    // when closing ssuDialog box, remove osd class to make screen brighter
                }
            });
        }
    }
    _startDownload() {
        var enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
        var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;

        if (hx.config.useOTA === 'DVBSSU') {
            ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
        } else if (hx.config.useOTA === 'ASTRA') {
            ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
        } else {
            ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
        }

        hx.svc.SW_UPDATE.startDownload({
                'otaSpec' : ulOtaSpec
        });
    }
    fnStartLiveService() {
        if (this._message) {
            this._message.$.remove();
            this._article.$.remove();
        }

        this.sendEventToEntity({
            alEvent : 'alChangePage',
            target : 'CPgLiveController?startService',
            me : this
        });
        // Boot OTA Check
        if (hx.config.useWakeUpOTA === 1) {
            hx.log("CPgStart", "check the bootup humax ota version after 2 seconds. ");
            setTimeout(() => {
                this._checkBootuptimeOta(this);
            }, 1000 * 2);
            // after 2 seconds, check the bootuptime OTA
        }
    }
    /************** cold boot functions ********/
    fnMakeDom() {
        if (!hx.config.useWaitBooting) {
            return;
        }
        this._article = hx.al.component('frame', {
            'tag' : 'article',
            'attr' : {
                'id' : 'DIA'
            }
        });
        this.$wrap = $('#wrap');

        /* The following object is for Long Cold Boot to show a Message. */
        this.$wrap.append(this._article.$);
    }
    fnRemoveDom() {
        if (!hx.config.useWaitBooting) {
            return;
        }

        if (this._message) {
            hx.al.decomponent(this._message);
            this._message = undefined;
        }
        if (this._article) {
            hx.al.decomponent(this._article);
            this._article = undefined;
        }
    }
    fnDrawSystemTimeUpdate() {
        var $target: JQuery, i: number, $selectedTarget: JQuery;

        this._message = hx.al.component('response', {
            '_msg_type' : 'progress',
            'text' : hx.l('LOC_INSTALL_SW_UPDATE_SYSTEM_TIME_ITEM_ID')
        });
        this._article.$.append(this._message.$);

        $target = $('#DIA .vaTop');
        for ( i = 0; i < $target.length; i += 1) {
            if ($target.eq(i).hasClass('aniActive') === false) {
                $selectedTarget = $target.eq(i);
                break;
            }
        }
        hx.il.ani.action({
            type : 'in',
            targets : $selectedTarget
        });
    }
    fnWaitingBoot() {
        var that = this;
        var tick = 0;
        var timeout = hx.config.useWaitBooting;

        if (!hx.config.useWaitBooting) {
            this.fnStartLiveService();
            return;
        }

        var result = hx.svc.CH_LIST.getChannelCount({});
        if (result.chCnt === 0) {
            this.fnStartLiveService();
            return;
        }

        var bColdBootDone = hx.svc.SETTING_UTIL.getColdBootDone();
        if (bColdBootDone) {
            that.fnStartLiveService();
            if (hx.svc.WEB_STORAGE.getAlexaDeviceEnable() === true) {
                var options = hx.svc.VOICE_ASSISTANCE.getMQTTOptions();
                var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
                var lanMacAddress = systemInfo.wiredMacAddress;

                options.userId = lanMacAddress;
                hx.svc.VOICE_ASSISTANCE.createMQTTClient(options);
                hx.svc.VOICE_ASSISTANCE.connectMQTTClient(options);
            }
            return;
        }

        that.fnDrawSystemTimeUpdate();
        var timer = setInterval(() => {
            hx.log("warning", "Check Cold Boot Done. tick : " + tick);
            tick += 1;
            bColdBootDone = hx.svc.SETTING_UTIL.getColdBootDone();
            if (bColdBootDone || tick >= timeout) {
                clearTimeout(timer);
                setTimeout( () => {
                    that.fnStartLiveService();
                    if (hx.svc.WEB_STORAGE.getAlexaDeviceEnable() === true) {
                        var options = hx.svc.VOICE_ASSISTANCE.getMQTTOptions();
                        var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
                        var lanMacAddress = systemInfo.wiredMacAddress;
                        options.userId = lanMacAddress;
                        hx.svc.VOICE_ASSISTANCE.createMQTTClient(options);
                        hx.svc.VOICE_ASSISTANCE.connectMQTTClient(options);
                    }
                }, 0);
            }
        }, 1000);
    }
}
hx.al.CPgStart = cPgStart;
export = cPgStart;
