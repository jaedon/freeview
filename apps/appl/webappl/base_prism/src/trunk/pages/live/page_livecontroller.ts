///<reference path="../../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 16..
 */
declare var hx;
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __MODEL__ = require("model/model");
import __DEF__ = require("def/oipf.def");
import __MSG_DEF__ = require("def/message.def");
import __PROVIDER__ = require("pages/util/provider");
import __CONTROLS__ = require("pages/util/controls");
import __BUTTON_PROMOTION__ = require("prismux/layout/prism.layout.button.promotion");
import __BUTTON__ = require("prismux/by_controls/prism.button");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");
enum SCHEDULE_TERM_ID {
    SINGLE = 2,
    SERIES = 5
}
enum eMHEGIsInterruptible {
    UNKNOWN = -1,
    NOT_INTERRUPTIBLE,
    INTERRUPTIBLE
}
class cCPgLiveController extends __PAGE_KERNEL__ {
    mliveOtaChecked : boolean = false;
    otanoti : any;
    help : any;
    $eac3 : any;
    $ac3 : any;
    $wrap : any;

    _lastBackTime : number = 0;
    _audToggleObj : any = null;
    _subToggleObj : any = null;
    _dolbyIconObj : any = null;
    _dolbyPlusIconObj : any = null;
    _statusObj : any = null;
    _pinObj : any = null;

    _noSvcObj : any = null;
    _bActivate : boolean = false;
    _bNeedUpdateChInfo : boolean = false;
    _lastNetflixPWStatus : any = null;
    _allVisible : boolean = true;
    _dolbyTimeOutID : number = -1;
    _dolbyIconTimer : number = -1;
    _dolbyPlusIconTimer : number = -1;
    mHelpLiveShown : boolean = false;
    mOtaNotiShown : boolean = false;
    _curErrorState : number = 0;
    _pinObjWrong : any;
    //_closeWatchinHDBtnTimer : any;
    _lastErrorState : any = {
        command : 'clear',
        text : '',
        timeout : false
    };
    _errorState : any = {
        CHANNEL_NOT_SUPPORTED : 0,
        CANNOT_TUNE : 1,
        TUNER_LOCK : 2,
        PARENTAL_LOCK : 3,
        ENCRYPTED_CHANNEL : 4,
        UNKNOWN_CHANNEL : 5,
        CHANNEL_SWITCH_INTERRUPTED : 6,
        CANNOT_TUNE_DUETO_RECORD : 7,
        CANNOT_RESOLVE_IP_CHANNEL : 8,
        BANDWIDTH : 9,
        CANNOT_SUPPORT_CHANNEL_LIST : 10,
        INSUFFICIENT_RESOURCE : 11,
        NOT_FOUND_IN_TRANSPORT : 12,
        MOTOR_MOVING : 13,
        CAS_BLOCK : 14,
        PIN_DENIED : 15,
        AV_UNDERRUN : 16,
        HDMI_BLOCK : 17,
        TIMER_7DAY_BLOCK : 18,
        NO_VIDEO : 19,
        NO_AV : 20,
        NOT_RUNNING : 21,
        NO_EIT : 22,
        PIN_5_TIMES_FAIL : 23,
        FTA_60DAY_LIMIT : 24,
        EMPTY_CHANNEL : 25,
        NO_ERROR : 26
    };
    _STATE : any = {
        STATE_CONNECT : 1,
        STATE_PRESENT : 2,
        STATE_STOPPED : 3
    };
    _PLAYSTATE : any = {
        PLAYSTATE_STOPPED : 0,
        PLAYSTATE_PLAYING : 1
    };
    TScheduledFactoryType : any = {
        ERECORDING : 0,
        EREMIND : 1,
        EHTTP_LIVE_STREAMING : 4,
        ESERIES_RECORDING : 10
    };
    private _promotionGreenButton = new __BUTTON_PROMOTION__.CGreenButtonPromotion();
    private _preChannel: any = null;
    private _promotionWatchInHD = new __BUTTON__.CButtonPromotionControl($("#DIA"), hx.l("LOC_WATCH_IN_HD_ID"),
        (aData) => {
            var linkages: ifFreeviewLinkage[] = aData.linkage;
            var ExtendedLinkage : any;
            hx.log('CPgLiveController', 'debug _moveToTheHDChannel');

            if (linkages.length === 1) {
                if(linkages[0].simul) {
                    var ChannelInfo = hx.svc.CH_LIST.getChannel({
                        ccid: linkages[0].channel,
                        group: 'TV'
                    });
                    var bChangeable:any = hx.svc.CH_LIST.checkChannelChangable({
                        channel: ChannelInfo.channel
                    });
                    if (bChangeable === false) {
                        if (hx.config.useTVStreaming === false) {
                            hx.msg('response', {
                                text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        } else {
                            hx.msg('response', {
                                text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                    } else {
                        hx.svc.AV_CTRL.channelChange({
                            channel: ChannelInfo.channel,
                            group: 'TV'
                        });
                    }
                }
            } else if (linkages.length === 2) {
                if((linkages[0].simul && !linkages[1].simul) || (!linkages[0].simul && linkages[1].simul)){
                    if(linkages[0].simul) {
                        ExtendedLinkage = linkages[0];
                    } else {
                        ExtendedLinkage = linkages[1];
                    }
                    var ChannelInfo = hx.svc.CH_LIST.getChannel({
                        ccid : ExtendedLinkage.channel,
                        group : 'TV'
                    });
                    hx.svc.AV_CTRL.channelChange({
                        channel : ChannelInfo.channel,
                        group : 'TV'
                    });
                } else if (linkages[0].simul && linkages[1].simul) {
                    var ChannelInfoA = hx.svc.CH_LIST.getChannel({
                        ccid : linkages[0].channel,
                        group : 'TV'
                    });
                    var ChannelInfoB = hx.svc.CH_LIST.getChannel({
                        ccid : linkages[1].channel,
                        group : 'TV'
                    });

                    var dlg = prism.createExtendedEventLinkageDescriptorButtonDlg(null, [
                        {
                            text: ChannelInfoA.channel.name,
                            fn: function() {
                                hx.log("debug", ChannelInfoA.channel.name + " ChannelInfoA Move");
                                hx.svc.AV_CTRL.channelChange({
                                    channel : ChannelInfoA.channel,
                                    group : 'TV'
                                });
                            }
                        },
                        {
                            text: ChannelInfoB.channel.name,
                            fn: function () {
                                hx.log("debug", ChannelInfoB.channel.name + " ChannelInfoB Move");
                                hx.svc.AV_CTRL.channelChange({
                                    channel : ChannelInfoB.channel,
                                    group : 'TV'
                                });
                            }
                        }
                    ]);
                    dlg.open({
                        onClosed: () => {
                            this.setKeyHookingControl(dlg);
                        }
                    });
                    this.setKeyHookingControl(dlg);
                }
            }
        }
    );
    private _RCTButtonDlg = null;
    private _stateMhegInterruptible: eMHEGIsInterruptible = eMHEGIsInterruptible.UNKNOWN;
    private _prevRctInfo: RctLinkInfoCollection = null;
    private _prevStateMhegInterruptible: eMHEGIsInterruptible = null;

    constructor(aPageName: string) {
        super();
        hx.logadd('CPgLiveController');
        hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, (aParam) => {
            this._on_ChannelChangeSucceeded(aParam);
        });
        hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, (aParam) => {
            this._on_ChannelChangeError(aParam);
        });
        hx.svc.AV_CTRL.addEventCb('PlayStateChange', this, (aParam) => {
            this._on_PlayStateChange(aParam);
        });
        hx.svc.AV_CTRL.addEventCb('ParentalRatingChange', this, (aParam) => {
            this._on_ParentalRatingChange(aParam);
        });
        hx.svc.AV_CTRL.addEventCb('evtSetChannel', this, (aParam) => {
            this._on_evtSetChannel(aParam);
        });
		hx.svc.AV_CTRL.addEventCb('audioChange', this, (aParam) => {
            this._on_audioChange(aParam);
		});
        if (ENV.op === ENV.listOP.UKDTT) {
            hx.svc.AV_CTRL.addEventCb('RctInfoChanged', this, this.onRctInfoChanged);
            hx.svc.AV_CTRL.addEventCb('ProgrammesChanged', this, this.onProgrammesChanged);
            hx.svc.AV_CTRL.addEventCb('RequestPincode', this, this.onRequestPincode);
            hx.svc.AV_CTRL.addEventCb('NCDChanged', this, this.onNCDChanged);
            hx.svc.AV_CTRL.addEventCb('RequestMhegTune', this, (aChannel: Channel, aServiceID: number, aTuneMode: __DEF__.eMhegTuneInfo) => {
                hx.log('CPgLiveController', '_');
                hx.log("debug", "RequestMhegTune received: aServiceID: " + aServiceID);
                hx.log("debug", "RequestMhegTune received: tuneInfo: " + aTuneMode);
                hx.svc.AV_CTRL.channelChange({channel: aChannel}, aTuneMode);
            });
            hx.svc.AV_CTRL.addEventCb('RequestMhegInterrupt', this, (aIsInterruptible: eMHEGIsInterruptible) => {
                hx.log("debug", "RequestMhegInteerrupt: "+ aIsInterruptible);
                if (this._stateMhegInterruptible !== eMHEGIsInterruptible.UNKNOWN) {
                    this._prevStateMhegInterruptible = this._stateMhegInterruptible;
                }
                this._stateMhegInterruptible = aIsInterruptible;
                if (!aIsInterruptible) {
                    this._closeGreenButton();
                }
                hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
                this.DEV();
            });
            hx.svc.RECORDINGS.addEventCb('NCDChanged', this, this.onNCDChanged);
            hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, (aButtonInfo) => {
                if (this.getCenterPageName() === this.name) {
                    if (!this.getPageVisible("CPgInfoHub") && !this.getPageVisible("CPgDetailInfo")) {
                        var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                        dlg.open({
                            onClosed: () => {
                                this.setKeyHookingControl(dlg);
                            }
                        });
                        this.setKeyHookingControl(dlg, true);
                    }
                }
            });
        }

        var self = this;
        hx.svc.APP_MGR.addEventCb('HbbTvAppEnter',this,()=>{
            self._promotionGreenButton.hideButton(false);
            //hx.svc.SETTING_UTIL.doingKeyGrab(false);
        });

        if(hx.config.netflix) {
            hx.svc.NETFLIX.addEventCb('NetflixStatusChanged',this,()=>{
                if(hx.svc.NETFLIX.isRunning() === true) {
                    self._promotionGreenButton.hideButton(true);
                    self._promotionWatchInHD.hideButton();
                }
            });
        }
    }
    DEV() {
        var devBoard = $("#debugger-green-button");
        if (devBoard) {
            var elPlus = devBoard.find("#debugger-plus");
            if (elPlus && elPlus.length === 0) {
                elPlus = $("<div>", {id: "debugger-plus"});
            }
            var textContent = elPlus.html();
            if (textContent !== null) {
                var line = textContent.split("<br>");
                if (line.length > 7) {
                    line.pop();
                }
                line.unshift(dateFormat(Date.now(), "HH:MM:ss") + "] MHEG is Interruptible: " + eMHEGIsInterruptible[this._stateMhegInterruptible] + " :: keygrab : " + this.visible());
                elPlus.html(line.join("<br>"));
            }
            devBoard.append(elPlus);
        }
    }
    create (aEntity : any, aPageUrl : any) : void {
        // icon
        var that = this;
        $.extend(that, {
            $ac3 : $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + '<p id="dolby" class="ico"></p>' + '</div>'),
            $eac3 : $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + '<p id="dolbyPlus" class="ico"></p>' + '</div>')
        });

        hx.al.compose(that);

        /////////////////////////////////
        // check no service

        var result : any = hx.svc.CH_LIST.getChannelCount();
        if (result.chCnt === 0) {// show no service
            this._noServiceDlgCreate();
        }

        that._bActivate = true;
        //this._lastNetflixPWStatus = null;
        hx.log('CPgLiveController', '######CREATE######');

        /////////////////////////////////
        // start first service
        if (aPageUrl.query === 'startService') {
            // clear reminder power off
            hx.svc.SETTING_UTIL.stopReminderPowerOff();

            // is reminder from standby off
            var powerValueObj = hx.svc.SETTING_UTIL.getPowerStateValues();
            if (powerValueObj && powerValueObj.wakeupReason === 'BY_TIMER') {
                hx.log('CPgLiveController', '### schedule wake up : power state BY_TIMER');
                hx.svc.SETTING_UTIL.startReminderPowerOff();
            } else {
                // normal booting
                hx.log('CPgLiveController', '### normal wake up');
            }
            that._changeToLastService();
            hx.log('debug', '[LIVE] hx.il.isGoingOperationFromStandby = false;');
            hx.il.isGoingOperationFromStandby = false;

            // update chinfo manually
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgChInfo?update'
            });

            // check P/P & login
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgMain/checkPrivacyVersion',
                me : that
            });
        } else if (aPageUrl.query === 'startIntroVideo') {
            if (result.chCnt > 0) {
                hx.log('CPgLiveController', '### try to play promotion video');
                var proRet : boolean = that.playPromotionVideo();
                if (proRet === false) {
                    hx.log('CPgLiveController', '### no promotion video found');
                    that._changeToLastService();
                }
            }
            //checkDetachableStorage
            if (hx.config.useDetachableStorage) {
                hx.svc.SETTING_UTIL.checkDetachableStorage();
            }
        } else {
            if (result.chCnt > 0) {
                that.startHelp();
                that.mHelpLiveShown = true;
            }
        }

        if (that._bNeedUpdateChInfo) {
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgChInfo?update'
            });
            that._bNeedUpdateChInfo = false;
        }
        /////////////////////////////////
        // recovery last error state
        that._refreshLastErrorState();
        // refresh irdeto error message, if stb use the irdeo CA system.
        that._refreshLastSystemErrorMessage();

        that.sendEventToEntity({
            alEvent : 'alCheckPageVisible'
        });
        if (aPageUrl.query !== 'refresh') {
            that._checkCurrentStatus();
        }

        that.fnLiveIPOTACheck();

        that._showGreenButton();

        that.setKeyHookingControl(<any>that._promotionWatchInHD, false, true);
        if (that._dlgNCD) {
            that.setKeyHookingControl(that._dlgNCD);
            that.setKeyHookingControl(that._dlgNCD);
        }
    }
    private _checkShowButton() {
        var ret = true;
        var pages = ["CPgInfoHub", "CPgDetailInfo", "CPgSmartSearch"];
        if (this.getCenterPageName() !== this.name) {
            ret = false;
        }
        if (ret) {
            ret = !hx.svc.SETTING_UTIL.isHbbTVActivated();
        }
        if (ret) {
            for (var i = 0; i < pages.length; i++) {
                if (this.getPageVisible(pages[i])) {
                    ret = false;
                    break;
                }
            }
        }
        return ret;
    }
    private _showGreenButton(aForce?: boolean) {
        if (this._checkShowButton()) {
            if (this._stateMhegInterruptible !== eMHEGIsInterruptible.NOT_INTERRUPTIBLE) {
                this.setVisible(this._promotionGreenButton.showButton(aForce));
            }
        }
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    private _showWatchInHD() {
        var a = this;
        if (this._checkShowButton()) {
            this._promotionWatchInHD.showButton();
        }
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    visible() {
        var ret = this._promotionGreenButton.isShowing() || super.visible();
        if (!ret) {
            if (this._stateMhegInterruptible !== eMHEGIsInterruptible.NOT_INTERRUPTIBLE) {
                ret = this._promotionGreenButton.getRctInfoCount() > 0;
            }
            if (!ret) {
                var pgMessage = hx.al.entity.getRepository().get("CPgMessage");
                if (pgMessage) {
                    ret = __PAGE_KERNEL__.prototype.visible.call(pgMessage);
                }
                if (!ret) {
                    var pgMain = hx.al.entity.getRepository().get("CPgMain");
                    if (pgMain) {
                        ret = __PAGE_KERNEL__.prototype.visible.call(pgMain);
                    }
                }
            }
        }
        return ret;
    }
    reqOpenRCTDialog() {
        var ret = false;
        var greenButton = this._promotionGreenButton;
        var rctInfos = greenButton.getData();
        /* TODO
         * load RCT dialog
         * bglee@humaxdigital.com, 17:31, 2014. 8. 6.
         */
        if (rctInfos) {
            greenButton.hideButton(true);
            var dlg = prism.createRCTButtonDlg(null, rctInfos, (aRctInfo: RctLinkInfo, aIsReminder: boolean) => {
                if (aIsReminder) {
                    /* TODO
                     * how to find programme from RctLinkInfo
                     * bglee@humaxdigital.com, 10:40, 2014. 8. 8.
                     */
                    var programme: Programme = null;
                    hx.svc.RECORDINGS.record({
                        programme: programme,
                        factoryType: __MODEL__.eTScheduledFactoryType.EREMIND,
                        finished: function () {
                            hx.log('CPgLiveController', "reminder");
                        }
                    });
                } else {
                    /* TODO
                     * if current channel is not available
                     * bglee@humaxdigital.com, 10:24, 2014. 8. 8.
                     */
                    var termIdToFactoryType = {
                        2: __MODEL__.eTScheduledFactoryType.ERECORDING,
                        5: __MODEL__.eTScheduledFactoryType.ESERIES_RECORDING
                    };
                    var currentChannelInfo = hx.svc.CH_LIST.getCurrentChannel();
                    var channelID = currentChannelInfo.result ? currentChannelInfo.channel.ccid : null;
                    hx.log("debug", "hx.svc.RECORDINGS.recordCRID(" + [termIdToFactoryType[aRctInfo.termId], aRctInfo.mediaUri, aRctInfo.eventName, channelID].join(", ") + ")");
                    hx.svc.RECORDINGS.recordCRID(termIdToFactoryType[aRctInfo.termId], aRctInfo.mediaUri, aRctInfo.eventName, channelID);
                }
            }, () => {
                hx.log("debug", "roll back key routine");
                this.setKeyHookingControl(dlg);
                this._showGreenButton(true);
            });
            dlg.setOnCheckSchedule((rctInfo: any) => {
                var scheduledList: any = hx.svc.RECORDINGS.getScheduledList();
                var crids: Array<any> = [];
                var termId: number;
                dlg.hideRecSingle();
                dlg.hideRecSeries();

                var i: number, len: number = scheduledList.length;
                for(i = 0 ; i < len ; i++) {
                    if(rctInfo.termId == SCHEDULE_TERM_ID.SINGLE) {
                        crids.push(scheduledList[i].getProgrammeCRID());
                        termId = SCHEDULE_TERM_ID.SINGLE;
                    }
                    else if(rctInfo.termId == SCHEDULE_TERM_ID.SERIES) {
                        var cridsLen: number = scheduledList[i].getGroupCRIDs().length;
                        var cridIdx: number;
                        for(cridIdx = 0 ; cridIdx < cridsLen ; cridIdx++) {
                            crids.push(scheduledList[i].getGroupCRIDs()[cridIdx]);
                        }
                        termId = SCHEDULE_TERM_ID.SERIES;
                    }
                }
                var j: number, cridsLen: number = crids.length;
                for(j = 0 ; j < cridsLen ; j++) {
                    if(crids[j] == rctInfo.mediaUri)
                    {
                        if(termId === SCHEDULE_TERM_ID.SINGLE) {
                            dlg.showRecSingle();
                            break;
                        }
                        else if(termId === SCHEDULE_TERM_ID.SERIES) {
                            dlg.showRecSeries();
                            break;
                        }
                    }
                }
            });
            dlg.doCheckSchedule();
            this.setKeyHookingControl(dlg);
            this._RCTButtonDlg = dlg;
        }
        ret = true;
        return ret;
    }
    destroy() {
        //this._hideGreenBtn();
        //TODO: close watch in hd
        if(hx.config.useNetflixPW){
            this.sendEventToEntity({
                alEvent: 'alMessageToPage',
                target: 'CPgMain/CloseNetflixPW',
                me : this
            });
        }
        this._statusMsgClose();
        this._noServiceDlgClose();
        this._pinMsgClose();
        // close irdeto error message, if stb use the irdeto CA system.
        this._hideLastSystemErrorMessage();
        // dolby
        clearTimeout(this._dolbyTimeOutID);
        this._dolbyPlusIconHide();
        this._dolbyIconHide();
        this._promotionGreenButton.hideButton();
        this._promotionWatchInHD.hideButton();

        this._bActivate = false;
        this._allVisible = true;

        this.setKeyHookingControl(<any>this._promotionWatchInHD);

        //<any>window.watchInHD = this._promotionWatchInHD;
        hx.svc.SETTING_UTIL.doingKeyGrab(true);
    }
    _on_alKey (aParam : any) : any {
        var bConsumed : boolean = false;
        var kc : number = aParam.alKey, k : any = hx.key;
        var ct : number;
        var diff : number, bPrismLiteSMode : boolean = false;
        var bRet : boolean = true;
        hx.log('CPgLiveController', '######  KEY : ' + kc);
        if (this._bActivate === false) {
            return;
        }
        var bisMpbOnGoing : boolean = hx.svc.PLAYBACK.isMpbOnGoing();
        if (bisMpbOnGoing && (kc !== k.KEY_SEARCH)) {    // except smart search KEY
            return;
        }
        switch (kc) {
            case k.KEY_CHANNEL_UP:
                hx.svc.AV_CTRL.naviChannel({
                    'direction' : 'next'
                });
                bConsumed = true;
                break;
            case k.KEY_CHANNEL_DOWN:
                hx.svc.AV_CTRL.naviChannel({
                    'direction' : 'prev'
                });
                bConsumed = true;
                break;
            case k.KEY_BACK:
                // prevent too fast chaning betwin two
                ct = new Date().getTime();
                diff = Math.abs(ct - this._lastBackTime);
                if (diff > 1000) {// 1000 msec
                    bRet = hx.svc.AV_CTRL.naviChannel({
                        'direction' : 'back'
                    });
                    if (bRet === false) {
                        if (hx.config.useTVStreaming === false) {
                            hx.msg('response', {
                                text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        } else {
                            hx.msg('response', {
                                text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        }
                    } else {
                        this._lastBackTime = ct;
                    }
                }
                bConsumed = true;
                break;
            case k.KEY_SCREEN_MODE_NEXT:
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgWide?show=toggle'
                });
                bConsumed = true;
                break;
            case k.KEY_OK:
                //this._hideGreenBtn();
                this._promotionGreenButton.hideButton(true);
                this._promotionWatchInHD.hideButton();
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgInfoHub?iPlate=toggle'
                });
                bConsumed = true;
                break;
            case k.KEY_UP:
            case k.KEY_DOWN:
            case k.KEY_LEFT:
            case k.KEY_RIGHT:
                //this._hideGreenBtn();
                this._promotionGreenButton.hideButton(true);
                this._promotionWatchInHD.hideButton();
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgInfoHub?channelList=toggle'
                });
                bConsumed = true;
                break;
            case k.KEY_0:
            case k.KEY_1:
            case k.KEY_2:
            case k.KEY_3:
            case k.KEY_4:
            case k.KEY_5:
            case k.KEY_6:
            case k.KEY_7:
            case k.KEY_8:
            case k.KEY_9:
                // hide dolby
                this._dolbyPlusIconHide();
                this._dolbyIconHide();

                if (this._pinObj === null) {
                    this.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgDcn?active=' + (kc - k.KEY_0).toString()
                    });
                    bConsumed = true;
                }
                break;
            case k.KEY_SEARCH:
                this._promotionGreenButton.hideButton(true);
                this._promotionWatchInHD.hideButton();
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgSmartSearch?type=live'
                });
                break;
            case k.KEY_TVRADIO_TOGGLE:
                this._action_TvRadioKey();
                bConsumed = true;
                break;
            case k.KEY_GREEN:
                if (this._promotionGreenButton.isShowing() || this._promotionGreenButton.getRctInfoCount() > 0) {
                    bConsumed = this.reqOpenRCTDialog();
                }
                break;
            default:
                break;
        }
        if (hx.config.netflix && hx.config.useNetflixPW) {
            switch(kc) {
                case k.KEY_NETFLIX:
                    this._pinMsgClose();
                    break;
                default:
                    break;
            }
        }
        return bConsumed;
    }
    _on_alMessageToPage (aParam : any) : void {
        var result : any;

        if (aParam.query === 'active') {
            this._bActivate = true;
        } else if (aParam.query === 'showpin') {
            if(hx.config.useNetflixPW) {
                var srvNetflix = hx.svc.NETFLIX;
                if(this._lastErrorState.command !== "clear"){
                    if(srvNetflix.getPwStatus()){
                        return;
                    }else{
                        if(this._lastNetflixPWStatus === 'hide'){
                            srvNetflix.start({
                                type:__NETFLIX__.START_TYPE.NETFLIX_BUTTON
                            });
                            this._lastNetflixPWStatus = null;
                            return;
                        }
                    }
                }
                this._lastNetflixPWStatus = null;
            }
            this._refreshLastErrorState();
        } else if (aParam.query === 'hidepin') {
            if(hx.config.useNetflixPW){
                if(aParam.data.hidepin === 'lunchApp'){
                    this._lastNetflixPWStatus = null;
                }
                var srvNetflix = hx.svc.NETFLIX;
                if(srvNetflix.getPwStatus() && !this._pinObj){
                    if(aParam.data.hidepin !== 'lunchApp'){
                        hx.log('CPgLiveController', 'alMessageToPage hidepin _lastNetflixPWStatus : hide');
                        this._lastNetflixPWStatus = 'hide';
                    }
                    this.sendEventToEntity({
                        alEvent: 'alMessageToPage',
                        target: 'CPgMain/CloseNetflixPW',
                        me : this
                    });
                    return;
                }
            }
            this._pinMsgClose();
        } else if (aParam.query === 'nochannel') {
            this._noServiceDlgCreate();
        } else if (aParam.query === 'hideerrorstate') {
            result = this._errorStatus(null, null, 'clear');
            this._showServiceStatus(result);
        } else if (aParam.query === 'hideNoChannelConfirm') {
            this._noServiceDlgClose();
        } else if (aParam.query === 'restoreNoChannelConfirm') {
            var result = hx.svc.CH_LIST.getChannelCount();
            if (result.chCnt === 0) {// show no service
                this._noServiceDlgCreate();
            }
        } else if (aParam.query === 'hideInfoHub') {
            this._showGreenButton(true);
        }
    }
    _on_alPagesVisible (aParam : any) : void {
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    _on_evtSetChannel (aParam : any) : void {
        var channel : any = aParam.channel;
        if (this._bActivate === false) {
            this._bNeedUpdateChInfo = true;
            return;
        }
        if (!aParam || aParam === null) {
            return;
        }

        // clear dolby
        clearTimeout(this._dolbyTimeOutID);
        this._dolbyPlusIconHide();
        this._dolbyIconHide();

        // clear error
        var lastSvc : any = hx.svc.WEB_STORAGE.getLastService();
        if (hx.svc.AV_CTRL.isLiveOnGoing() !== true || lastSvc.ccid !== channel.ccid) {
            var obj = this._errorStatus(channel, null, 'clear');
            this._showServiceStatus(obj);
        }

        if (hx.config.useOTANotification === 1) {
            // check live ota notification message
			this._checkstartOtaNoti();
        }

        // check live help message
        if (this.mHelpLiveShown === false) {
            var helpO : any = hx.svc.WEB_STORAGE.getHelp();
            if (!helpO.ltv) {
                this.startHelp();
            } else {
                this.mHelpLiveShown = true;
            }
        }

        //hide promotionWatchInHD
        var promotionWatchInHD = this._promotionWatchInHD;
        if(promotionWatchInHD.isShowing()){
            promotionWatchInHD.hideButton();
        }
    }
    /*
     * ERROR State --------------------- start
     * */
    _showServiceStatus (aParam : any) : void {
         var bPbGoing : boolean;
        var cmd : string = (aParam.command !== undefined) ? aParam.command : '';
        var text : string = (aParam.text !== undefined) ? aParam.text : '';

        hx.log('CPgLiveController', '_showServiceStatus - cmd : ' + cmd);

        // it first 4 character is 'LOC_', it is string id!!!!
        if (text.substr(0, 4) === 'LOC_') {
            text = hx.l(text);
        }

        setTimeout(() => {
            switch (cmd) {
                case 'clear':
                    // remove status message
                    this._statusMsgClose();
                    this._pinMsgClose();
                    break;
                case 'status':
                    bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                    if ((this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                        this._statusMsgCreate(text);
                    }
                    break;
                case 'ratingBlock' :
                    if (this.getPageVisible('CPgInfoHub') === true) {
                        // not allow with in info hub
                        break;
                    }
                    bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                    if ((this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                        this._pinMsgCreate('ratingBlock');
                    }
                    break;
                case 'pin':
                    if (this.getPageVisible('CPgInfoHub') === true) {
                        // not allow with in info hub
                        break;
                    }
                    bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                    if ((this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                        this._pinMsgCreate('pin');
                    }
                    break;
                case 'pinNoEit':
                    if (this.getPageVisible('CPgInfoHub') === true) {
                        // not allow with in info hub
                        break;
                    }
                    bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                    if ((this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                        this._pinMsgCreate("noEit");
                    }
                    break;
                case 'pinClose':
                    this._pinMsgClose();
                    break;
                case 'response' :
                    if (this._bActivate === true) {
                        this._responseMsgCreate(text);
                    }
                    break;
                default:
                    break;
            }
        }, 100);
    }
    _showOrHideWatchInHD (): void {
        var promotionWatchInHD = this._promotionWatchInHD;
        if (promotionWatchInHD.setFromVolatileData()) {
            var data = promotionWatchInHD.getData();
            var current = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
            if (current && data.ccid === current.ccid) {
                this._showWatchInHD();
            } else {
                promotionWatchInHD.setData();
            }
        } else {
            promotionWatchInHD.setData();
        }
    }
    _on_ChannelChangeSucceeded (aParam : any) : void {
        var channel : any = (aParam) ? aParam.channel : undefined;
        var promotionWatchInHD = this._promotionWatchInHD;
        hx.log('CPgLiveController', '### ChannelChangeSucceeded ' + ((channel) ? channel.name : 'undefined'));
        hx.svc.AV_CTRL.setChannelChanging(false);

        //destroy greenButton
        this._promotionGreenButton.setData();
        this._promotionGreenButton.hideButton();

        if (this._preChannel === null) {
            this._preChannel = channel.majorChannel;
            this._showOrHideWatchInHD();
        } else {
            var volatileData = promotionWatchInHD.getVolatileData();
            //if (this._preChannel !== channel.majorChannel) {
            if (volatileData && volatileData.ccid == channel.ccid && !promotionWatchInHD.isShowing()) {
                this._showOrHideWatchInHD();
                this._preChannel = channel.majorChannel;
            }else{
                if(volatileData && volatileData.ccid != channel.ccid && promotionWatchInHD.isShowing()){
                    promotionWatchInHD.hideButton();
                }
            }
        }

        //TODO: close watch in hd
        // check dolby
        this._dolbyPlusIconHide();
        this._dolbyIconHide();
        if (this._bActivate === true) {
            this._checkDolby();
        }
        // set error state
        var param = this._errorStatus(channel, null, 'clear');
        this._showServiceStatus(param);
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    _on_ChannelChangeError (aParam) {
        var channel : any = (aParam) ? aParam.channel : undefined;
        var errorState : number = aParam.errorState;
        var param : any = null;
        hx.log('CPgLiveController', '### ChannelChangeError ' + ((channel) ? channel.name : 'undefined') + ' errorState : ' + errorState);
        //destroy greenButton
        this._promotionGreenButton.setData();
        this._promotionGreenButton.hideButton();
        // check dolby
        this._dolbyPlusIconHide();
        this._dolbyIconHide();
        if (this._bActivate === true) {
            this._checkDolby();
        }
        param = this._errorStatus(channel, errorState);
        this._showServiceStatus(param);
        this._promotionWatchInHD.setData();
        this._promotionWatchInHD.setVolatileData();
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    _on_PlayStateChange (aParam) {
        var param : any, state : number, error : number;
        if (aParam.from === 'VideoBroadcast') {
            state = aParam.state;
            error = aParam.error;
            hx.log('CPgLiveController', '### PlayStateChange : VB state: ' + state + ' error : ' + error);
            if (state === this._STATE.STATE_CONNECT) {
                if (error !== undefined) {
                    param = this._errorStatus(null, error);
                    this._showServiceStatus(param);
                }
            } else if (state === this._STATE.STATE_PRESENT || state === this._STATE.STATE_STOPPED) {
                param = this._errorStatus(null, null, 'clear');
                this._showServiceStatus(param);
            }
        } else if (aParam.from === 'VideoOnDemand') {
            state = aParam.playState;
            error = aParam.error;
            hx.log('CPgLiveController', '### PlayStateChange : VOD state: ' + state + ' error : ' + error);
            if (state === this._PLAYSTATE.PLAYSTATE_PLAYING) {// close message when start PB
                param = this._errorStatus(null, null, 'clear');
                this._showServiceStatus(param);
                // close irdeto error message, if it uses the irdeo CA system.
                this._hideLastSystemErrorMessage();
            }
        }
    }
    _on_ParentalRatingChange (aParam : any) {
        var bblocked : boolean = aParam.blocked;
        var param : any;

        hx.log('CPgLiveController', '### ParentalRatingChange rating : ' + aParam.rating + 'bblockedd : ' + aParam.blocked);
        if (bblocked === true) {
            param = this._errorStatus(null, 3, 'ratingBlock');
        } else {
            param = this._errorStatus(null, 3, 'close');
        }
        this._showServiceStatus(param);
    }
	_on_audioChange (aParam : any) {
        this._checkDolby();
	}
    _errorStatus (aChannel : any, aErrorState : number, aOpt? : string ) {
        // must be syncronized with eOplPlaybackError in oplvideobroadcasthandle_def.h
        var errtext: any[][] = [
        // text, timeout
        ["Channel not supported by tuner.", false], // 0
        ['LOC_NO_SIGNAL_ID', false], // 1
        ["Tuner locked by other object.", false], // 2
        ['LOC_LIVE_PC_BLOCK_ID', false], // 3
        ["Encrypted channel, key/module missing.", false], // 4
        ["Unknown channel.", false], // 5
        ["Channel switch interrupted.", false], // 6
        ['LOC_CANNOTCHANGEWHILEREC_ID', false], // 7
        ["Cannot resolve URI of referenced IP channel.", false], // 8
        ["Insufficient bandwidth.", false], // 9
        ["Channel cannot be changed.", false], // 10
        ["Insufficient resources are available to present the given channel.", false], // 11
        ["Specified channel not found in transport stream.", false], // 12
        ["DiSEqC Motor is now moving.", false], // 13
        ["Waiting CAS admission", false], // 14
        ["Pin code is denied", false], // 15
        ['LOC_CHANNEL_NOT_AVAILABLE_ID', false], // 15
        ['LOC_HDCP_MSG_ID', false], // 17
        ["7DayTime Block (??)", false], // 18
        ['LOC_VIDEO_NOTAVAILABLE_ID', false], // 19
        ['LOC_AV_NOTAVAILABLE_ID', false], // 20
        ["Service is not running regarding running flag in SDT", false], // 21
        ["No EIT (Irdeto Specific)", false], // 22
        ["Pin code is denied at 5 times.", false], // 23
        ["Encounter limitation of watching FTA in 60 days.", false], // 24
        ['LOC_CHANNEL_NOT_AVAILABLE_ID', false], // 25
        ['LOC_TERRESTRIAL_SHORT_CIRCUIT_ID', false] // 26
        ];
        var text : string = '';
        var btimeout : boolean = true;
        var dlg : string;

        if (aOpt === 'clear') {// clear all
            dlg = 'clear';
        } else if (aErrorState < errtext.length) {
            switch(aErrorState) {
                case this._errorState.PARENTAL_LOCK:
                    dlg = 'pin';
                    if (aOpt === 'close') {
                        dlg = 'pinClose';
                    } else if (aOpt === 'ratingBlock') {
                        dlg = 'ratingBlock';
                    }
                    break;
                case this._errorState.CANNOT_TUNE_DUETO_RECORD:
                    dlg = 'response';
                    break;
                case this._errorState.NO_EIT:
                    if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                        dlg = 'pinNoEit';
                        if (aOpt === 'close') {
                            dlg = 'pinClose';
                        }
                    } else {
                        // follow the default case;
                        dlg = 'status';
                    }
                    break;
                default:
                    dlg = 'status';
                    break;
            }

            text = errtext[aErrorState][0];
            btimeout = errtext[aErrorState][1];

            // it first 4 character is 'LOC_', it is string id!!!!
            if (text.substr(0, 4) !== 'LOC_') {
                // This is not translate String ID....
                hx.log('error', 'we have to use String ID !!!!!! erro text : ' + text);
            }
        } else {
            dlg = 'status';
            text = 'Unknown error';
            btimeout = false;
        }
        var param : any = {
            'command' : dlg,
            'text' : text,
            'timeout' : btimeout
        };

        if (dlg !== 'response') {
            this._lastErrorState.command = param.command;
            this._lastErrorState.text = param.text;
            this._lastErrorState.timeout = param.timeout;
        }
        return param;
    }
    _refreshLastErrorState (fnValid?:(command: string) => boolean) : void {
        var param : any = {
            'command' : this._lastErrorState.command,
            'text' : this._lastErrorState.text,
            'timeout' : this._lastErrorState.timeout
        };
        if (fnValid && fnValid(this._lastErrorState.command)) {
            this._showServiceStatus(param);
        } else {
            this._showServiceStatus(param);
        }
    }
    _refreshLastSystemErrorMessage () : void {
        if (ENV.listCAS.IR === ENV.cas) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgCasIrUiNotifier?refreshIrdetoSystemErrorMsg'
            });
        } else if (ENV.listCAS.IRUC === ENV.cas) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgCasIrUcUiNotifier?refreshIrdetoSystemErrorMsg'
            });
        } else if (ENV.listCAS.NA === ENV.cas) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgCasNaUiNotifier?refreshNagraSystemErrorMsg'
            });
        }
    }

    _isEmpty(value: any) : boolean {
        return ( typeof value === undefined || value === null || value === undefined || value.length === 0);
    }
    onRctInfoChanged(aLinkCount: number, aRctInfoCollection: RctLinkInfoCollection): void {
        var greenButton = this._promotionGreenButton;
        if (this._stateMhegInterruptible === eMHEGIsInterruptible.NOT_INTERRUPTIBLE) {
            hx.log("debug", "keep!!");
            this._prevRctInfo = aRctInfoCollection;
        }
        this._closeGreenButton();
        hx.log("warning", "prev interruptible state: " + this._prevStateMhegInterruptible);
        hx.log("warning", "current interruptible state: " + this._stateMhegInterruptible);
        if (aRctInfoCollection && aRctInfoCollection.length) {
            if (aLinkCount !== aRctInfoCollection.length) {
                hx.log("debug", "check why aLinkCount !== aRctInfoCollection.length");
            }
            var fnCheckValidInfo = () => {
                var ret = true;
                if (this._stateMhegInterruptible === eMHEGIsInterruptible.NOT_INTERRUPTIBLE) {
                    ret = false;
                    hx.log("debug", "mheg not interruptible, unset data");
                } else if (this._stateMhegInterruptible === eMHEGIsInterruptible.INTERRUPTIBLE
                    && this._prevStateMhegInterruptible === eMHEGIsInterruptible.NOT_INTERRUPTIBLE) {
                    ret = !_.isEqual(this._prevRctInfo, aRctInfoCollection);
                    hx.log("debug", "rct info comparison result: " + ret);
                }
                hx.log("debug", "ret: " + ret);
                return ret;
            };
            if (fnCheckValidInfo()) {
                greenButton.setData(aRctInfoCollection);
            } else {
                greenButton.setData();
            }
            if (hx.al.entity.getCenterPageName() === "CPgLiveController") {
                this._showGreenButton();
            }
        }
    }
    private _closeGreenButton() {
        var greenButton = this._promotionGreenButton;
        if (this._RCTButtonDlg) {
            this._RCTButtonDlg.close();
            this._RCTButtonDlg = null;
        }
        greenButton.setData();
        greenButton.hideButton();
        this.setVisible(false);
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        this.DEV();
    }
    onNCDChanged(aStartTime, aDuration, aMessage): void {
        if (hx.svc.CHANNEL_SEARCH_T.isStartScan()) {
            return;
        }
        var currentTime = Date.now();
        var isCurrentValid = aStartTime === 0 ? true : (aStartTime + aDuration) <= currentTime;
        var aCallback = (res : String) => {
            switch (res) {
                case "NOW":
                    if (hx.config.netflix && hx.svc.NETFLIX.isRunning()) {
                        hx.log('debug', 'Stop(suspend) Netflix & Search channel again by NCD changed');
                        hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE_WITH_NETFLIX_EXIT);
                    }
                    hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
                    hx.svc.BRIDGE.requestStopPlayback();
                    hx.svc.SETTING_UTIL.setNCDNotifyData(0);
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgSettings?set=startAutomaticSearch',
                        me : this
                    });
                    break;
                case "LATER":
                    if (hx.config.netflix && hx.svc.NETFLIX.isRunning()) {
                        hx.log('debug', 'Close NCD popup for Netflix');
                        hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                    }
                    hx.svc.SETTING_UTIL.setNCDNotifyData(0);
                    hx.svc.RECORDINGS.recordAt({
                        startTime: (currentTime / 1000) + 12 * 60 * 60,
                        repeatDays: 0,
                        duration: 0,
                        factoryType: __MODEL__.eTScheduledFactoryType.ESCHEDULED_RECORDING_AOS_NOTIFY,
                        ccid: "ccid://do.not.use"
                    });
                    break;
                case "OK":
                    if (hx.config.netflix && hx.svc.NETFLIX.isRunning()) {
                        hx.log('debug', 'Close NCD popup for Netflix');
                        hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                    }
                    break;
            }
        };
        if (!this._dlgNCD) {
            this._dlgNCD = prism.createNCDButtonDlg(null, {
                aMessage : aMessage,
                aExpectedTime : new Date(aStartTime).format("HH:MM:ss, m/d/yyyy"),
                isValid : !isCurrentValid,
                validOnNetflix : true
            }, aCallback);
            this._dlgNCD.open({
                onClosed : () => {
                    this.setKeyHookingControl(this._dlgNCD);
                    this._dlgNCD = null;
                }
            });
            this.setKeyHookingControl(this._dlgNCD);
        }
    }
    private _dlgNCD = null;

    onProgrammesChanged(aProgramme: __PROVIDER__.cPdeProgrammeItem) {
        var chCurrent = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
        var prevCCID = hx.svc.WEB_STORAGE.getBackService().ccid;
        var promotionWatchInHD = this._promotionWatchInHD;
        if (chCurrent && !chCurrent.isHD) {
            var linkage = aProgramme.getHDLinkage();
            var isChannelChanged = chCurrent.ccid !== prevCCID;
            if (!(linkage && linkage.length)) {
                var programme = hx.svc.EPG.getLiveProgrammes();
                if (!(programme && programme[0][0])) {
                    return;
                }
                hx.log("debug", "is channel changed: " + isChannelChanged);
                linkage = programme[0][0].pluginObject.getField("freeview/linkage");
            }
            if (linkage && linkage.length > 0) {
                for (var i = 0, len = linkage.length; i < len; i++) {
                    if (linkage[i].simul) {
                        //TODO show watch in hd button
                        promotionWatchInHD.setData();
                        promotionWatchInHD.setVolatileData({
                            ccid: chCurrent.ccid,
                            linkage: linkage
                        });
                        if (isChannelChanged) {
                            //TODO: wait;
                            hx.log("debug", "dat setted but we wait channel changed succceed event");
                            if (hx.svc.AV_CTRL.getChannelChanging() === false) {
                                this._showOrHideWatchInHD();
                            } else {
                                promotionWatchInHD.hideButton();
                            }
                        } else {
                            promotionWatchInHD.setData({
                                ccid: chCurrent.ccid,
                                linkage: linkage
                            });
                            this._showWatchInHD();
                        }
                        break;
                    }
                }
            }else{
                if(promotionWatchInHD.isShowing()){
                    promotionWatchInHD.hideButton();
                }
            }
        }
    }

    onRequestPincode(aMessage: string): void {
        var fnPincodeDialog = (_status, _input) => {
            if (_status === 'stop') {
                hx.svc.PARENTAL_CTRL .verifyParentalControlPIN({
                    password : _input,
                    cb : (aParam) => {
                        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
                        hx.msg.close('pin', compPincode);
                        hx.svc.AV_CTRL.setRequestPincodeResult(!aParam.result);
                    }
                });
            } else if (_status === 'key_back') {
                hx.msg.close('pin', compPincode);
                //that._strMenu.on();
                //that._strMenu.setItemFocus(1);
            } else if (_status === 'key_esc') {
                hx.msg.close('pin', compPincode);
            }
        };
        var compPincode = hx.msg('pin', {
            text : "STRING_" + aMessage,
            callback_fn : fnPincodeDialog,
            auto_close : false,
            dia_class : 'osd',
            enable : true
        });
    }
    /*
     * ERROR State --------------------- end
     * */
    __makeDispTime (aData : any) : string {
        var startTime : any, endTime : any;
        if (aData.startTime === undefined) {
            startTime = new Date();
        } else {
            startTime = new Date(aData.startTime * 1000);
        }
        endTime = new Date(startTime.valueOf() + (aData.duration * 1000));
        return aData.name + ', ' + startTime.format("HH:MM") + '-' + endTime.format("HH:MM") + ', ' + startTime.format("mm.dd");
    }
	_checkDolby () : void {
		var f = ()=> {
			//hx.log('CPgLiveController', '[_checkDolby] : getAudioComponent');
			var stCuraudioInfo : any = hx.svc.AV_CTRL.getCurAudio();
			var bIsAC3 : boolean = false;
			var bIsEAC3 : boolean = false;

			if (stCuraudioInfo) {
				//hx.log('CPgLiveController', '[_checkDolby] : ' + ':' + stCuraudioInfo.encoding);
				if (stCuraudioInfo.encoding === "AC3") {
					bIsAC3 = true;
				} else if (stCuraudioInfo.encoding === "E-AC3") {
					bIsEAC3 = true;
				}
			}

			if (bIsEAC3 === true) {
				this._dolbyPlusIconShow();
			} else if (bIsAC3 === true) {
				this._dolbyIconShow();
			}
		};

		clearTimeout(this._dolbyTimeOutID);
		this._dolbyTimeOutID = setTimeout(f, 2000);
	}
    _statusMsgClose () : void {
        //hx.log('error', '_statusMsgClose - this._statusObj : ' + this._statusObj);

        if (this._statusObj !== null) {
            hx.msg.close('status', this._statusObj);
            this._statusObj = null;
        }
    }
    _statusMsgCreate (aText : string) : void {
        this._statusMsgClose();
        this._statusObj = hx.msg('status', {
            text : aText,
            auto_close : false,
            callback_fn : function(p) {
                var nowork;
            }
        });
        //hx.log('error', '_statusMsgCreate - that._statusObj : ' + that._statusObj);
    }
    _noServiceDlgClose () : void {
        if (this._noSvcObj !== null) {
            hx.msg.close('confirm', this._noSvcObj);
            this._noSvcObj = null;
        }
    }
    _noServiceDlgCreate () : void {
        var text : string = hx.l('LOC_NOCHANNEL_ID');
        var btn : string = hx.l('LOC_OK_ID');

        // close previous dialog
        this._noServiceDlgClose();

        var dlgAgain : Function = ()=> {
            this._noServiceDlgCreate();
        };

        var cb : Function = (param)=> {
            if (param === btn) {
                this._noServiceDlgClose();
                // jump to settings > installation
                this.sendEventToEntity({
                    alEvent : 'alChangePage',
                    target : 'CPgSettings?set=directSch',
                    me : this
                });
            } else if (param === 'key_esc' || param === 'key_back') {
                this._noServiceDlgClose();
                setTimeout(dlgAgain, 0);
            }
        };

        // create dialog
        this._noSvcObj = hx.msg('confirm', {
            text : text,
            auto_close : false,
            btn_title_arr : [btn],
            //	timeout : 5000,
            //dia_class : 'osd',
            callback_fn : cb
        });

        // set front panel
        hx.svc.SETTING_UTIL.setFrontPanelString({
                'text' : '-NO CHANNEL-'
        });
    }
    _responseMsgCreate (aText : string) : void {
        hx.msg('response', {
            text : aText,
            auto_close : true,
            close_time : 3000
        });
    }
    _pinMsgClose () : void {
        if (this._pinObj !== null) {
            hx.msg.close('pinPC', this._pinObj);
            this._pinObj = null;
        }
    }
    _pinMsgCreate (opt : string) : void {
        var that = this, text = hx.l('LOC_LIVE_PC_BLOCK_ID');

        that._pinMsgClose();

        if (opt === 'pin') {// channel lock
            var curChObj = hx.svc.CH_LIST.getCurrentChannel();
            if (curChObj.channel) {
                if (curChObj.channel.manualBlock) {
                    text = hx.l('LOC_STR_MESG_806_ID');
                }
            }
        } else {
            // parental control lock
            if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {// for Irdeto case, use Irdeto error message.
                var irErrCode : string = '';
                if (opt === 'noEit') {
                    if (ENV.listCAS.IR === ENV.cas) {
                        irErrCode = "E44-32";
                    } else {
                        irErrCode = "E044-32";
                    }
                } else {
                    if (ENV.listCAS.IR === ENV.cas) {
                        irErrCode = "E42-32";
                    } else {
                        irErrCode = "E042-32";
                    }
                }
                var message;
                if (ENV.listCAS.IR === ENV.cas) {
                    message = hx.al.component('cIrErrorMessages');
                } else {
                    message = hx.al.component('cIrUcErrorMessages');
                }
                var cas_ir_errMsgString = message._cas_ir_get_errorMsgString(irErrCode);
                text = irErrCode + " " + cas_ir_errMsgString;
            }
        }

        that._pinObj = hx.msg('pinPC', {
            text : text,
            auto_close : false,
            //			dia_class : 'osd',
            enable : true,
            callback_fn : function(_status, _input) {
                if (_status === 'stop') {

                    hx.svc.PARENTAL_CTRL .verifyParentalControlPIN({
                            password : _input,
                            cb : function(retObj) {
                                if (retObj.result === 0) {
                                    that._pinMsgClose();
                                    setPCStatus(_input, false);
                                } else {
                                    that._pinMsgClose();
                                    that._pinObjWrong = hx.msg('pinPC', {
                                        text : text,
                                        auto_close : true,
                                        enable : false,
                                        timeout : 500,
                                        callback_fn : function(_status, _input) {
                                            if (_status === 'auto_close') {
                                                that._pinMsgCreate(opt);
                                            }
                                        }
                                    });
                                }
                            }
                    });
                }
            }
        });

        var setPCStatus = function(pcPIN : any, enable : boolean) : void {
            hx.svc.PARENTAL_CTRL .setParentalControlStatus({
                    pcPIN : pcPIN,
                    enable : enable
            });
        };
    }
    _dolbyIconShow () : void {
        this._dolbyIconHide();
        clearTimeout(this._dolbyIconTimer);
        $('#wrap').append(this.$ac3);
        this._dolbyIconTimer = setTimeout(()=> {
            this._dolbyIconHide();
        }, 5000);
    }
    _dolbyIconHide () : void {
        clearTimeout(this._dolbyIconTimer);
        if (this.$ac3) {
            this.$ac3.remove();
        }
    }
    _dolbyPlusIconShow () : void {
        this._dolbyPlusIconHide();
        clearTimeout(this._dolbyPlusIconTimer);
        $('#wrap').append(this.$eac3);
        this._dolbyPlusIconTimer = setTimeout(()=> {
            this._dolbyPlusIconHide();
        }, 5000);
    }
    _dolbyPlusIconHide () : void {
        clearTimeout(this._dolbyPlusIconTimer);
        if (this.$eac3) {
            this.$eac3.remove();
        }
    }
    startHelp () : void {
        if (!hx.config.useHelpPage) {
            hx.log('debug', '[startHelp] !hx.config.useHelpPage');
            return;
        }
        var helpViewObj : any = hx.svc.WEB_STORAGE.getHelp();
        var target = hx.al.entity._repos.get("CPgMain") || this;
        this.$wrap = $(document.getElementById('wrap'));
        if (!helpViewObj.ltv) {// && that.mediaType === 'Video') {
            this.help = hx.al.component('HelpView', {
                'parent' : target,
                '_type' : 'ltv'
            });
            this.$wrap[0].appendChild(this.help.$[0]);
            hx.al.compose(target).append(this.help);
            this.help.on();
        }
    }
	_checkstartOtaNoti () : void {
		var updatedList : any, datas : any = [];
		var OtaNoti : any = hx.svc.WEB_STORAGE.getOtaNoti();
        var NewOtaVersion : number,  uidataObj : any;

		updatedList = hx.svc.SETTING_UTIL.getUpdatedList();
		datas.push(updatedList.updatedList);

		uidataObj = JSON.parse(datas);
		NewOtaVersion = uidataObj.uidata.otaversion;

		// check live ota notification message
		if (this.mOtaNotiShown === false) {
			if ((OtaNoti.otacheck === undefined) || (OtaNoti.otacheck === null)) {
				hx.svc.WEB_STORAGE.setOtaNoti({
						'otacheck' : NewOtaVersion
				});
			}
			if ((OtaNoti.otanoti === undefined) || (OtaNoti.otanoti === null)) {
				hx.svc.WEB_STORAGE.setOtaNoti({
						'otanoti' : "false"
				});
			}
			this.startOtaNoti(this);
			this.mOtaNotiShown = true;
		}
	}
    startOtaNoti (aTargetObject) {
		var updatedList : any, datas : any = [];
		var OtaNoti : any = hx.svc.WEB_STORAGE.getOtaNoti();
        var NewOtaVersion : number,  uidataObj : any;

		updatedList = hx.svc.SETTING_UTIL.getUpdatedList();
		datas.push(updatedList.updatedList);

		uidataObj = JSON.parse(datas);
		NewOtaVersion = uidataObj.uidata.otaversion;

		if ((OtaNoti.otacheck < NewOtaVersion) && (NewOtaVersion !== 0)/* || (OtaNoti.otanoti === "false")*/) {
			//hx.svc.WEB_STORAGE.setOtaNoti(NewOtaVersion);
			hx.svc.WEB_STORAGE.setOtaNoti({
					'otacheck' : NewOtaVersion
			});
			this.$wrap = $(document.getElementById('wrap'));
			this.otanoti = hx.al.component('OtaNoti', {
				'parent' : aTargetObject,
				'_type' : 'otanoti'
			});
			this.$wrap[0].appendChild(this.otanoti.$[0]);
			hx.al.compose(this).append(this.otanoti);
			this.otanoti.on();
		}
    }
    playPromotionVideo () : boolean {
        if (!hx.config.useHelpPage) {
            hx.log('debug', '[startHelp] !hx.config.useHelpPage');
            return false;
        }
        var helpViewObj : any = hx.svc.WEB_STORAGE.getHelp();
        var dsrvBridge : any = hx.svc.BRIDGE;
        var bret : boolean = false;
        if (!helpViewObj.ltv) {
            var o = hx.svc.SETTING_UTIL.getPromotionVideo();
            if (!!o.uri) {
                var item = new __MODEL__.CMediaItemByUrl(o.title, o.uri);
                item.setDuration(o.duration);
                dsrvBridge.requestVideoPlayback([item], 0);
                bret = true;
            }
        }
        return bret;
    }
    _checkCurrentStatus () : void {
        var bMedGoing : boolean = hx.svc.PLAYBACK.isMpbOnGoing();
        hx.log('CPgLiveController', 'checkCurrentStatus => bMedGoing : ' + bMedGoing);
        var bPbGoing : boolean = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
        hx.log('CPgLiveController', 'checkCurrentStatus => bPbGoing : ' + bPbGoing);
    }
    _changeToLastService () : void {
        // tune to available channel
        var result : boolean = hx.svc.AV_CTRL.lastChannel();
        if (result === false) {
            hx.svc.AV_CTRL.availableChannel();
        }
    }
    _hideLastSystemErrorMessage () : void {
        if (ENV.listCAS.IR === ENV.cas) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgCasIrUiNotifier?hideIrdetoSystemErrorMsg'
            });
        } else if (ENV.listCAS.IRUC === ENV.cas) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgCasIrUcUiNotifier?hideIrdetoSystemErrorMsg'
            });
        } else if (ENV.listCAS.NA === ENV.cas) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgCasNaUiNotifier?hideNagraSystemErrorMsg'
            });
        }
    }
    _showNotAvailableIcon () : void {
        this.sendEventToEntity({
            alEvent : 'alMessageToPage',
            target : 'CPgNotAvailable?show'
        });
    }
    _isPrismLiteSModeOn () : boolean {
        var bIpEpgOn : boolean = false;
        var bret : boolean = false;

        if ((hx.config.usePrismLiteS === undefined) || (hx.config.usePrismLiteS === false)) {
            bret = false;
        } else if (hx.config.usePrismLiteS === true) {
            if (ENV.listModel.IR4000HD === ENV.model) {
                bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                if (bIpEpgOn === true) {
                    bret = false;
                } else {
                    bret = true;
                }
            } else if (ENV.listModel.HMS1000S === ENV.model) {
                var countryId;

                countryId = hx.svc.SETTING_UTIL.getCountryId();

                bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();

                switch(countryId) {
                    //germany
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                    //austria
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                    //switzerland
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                    //italy
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                        bret = false;
                        break;
                    default :
                        bret = false;
                }
            } else {
                bret = true;
            }
        } else {
            bret = false;
        }
        return bret;
    }
    _action_TvRadioKey () : void {
        var ulTvSvcCount : number = 0, ulRadioSvcCount : number = 0;
        var retObjCurSvc : any, curSvcType : number, curSvcCCID : any, lastSvc : any, lastSvcCCID : any, lastSvcGroup : any, retObjLastSvc : any;
        var retObjLastTvRadioSvcCcid : any, bChangeable = false;
        var chlist : any;

        chlist = hx.svc.CH_LIST.getChannelCount();

        ulTvSvcCount = hx.svc.CH_LIST.getChannelCount({
                group : 'TV'
        }).chCnt;

        ulRadioSvcCount = hx.svc.CH_LIST.getChannelCount({
                group : 'RADIO'
        }).chCnt;

        // 1. Check Total Service Count
        if ((chlist.chCnt === 0) || (ulTvSvcCount < 1) || (ulRadioSvcCount < 1)) {
            this._showNotAvailableIcon();
            return;
        }

        // 2. check current service type
        retObjCurSvc = hx.svc.CH_LIST.getCurrentChannel();
        if (true === retObjCurSvc.result) {
            if (retObjCurSvc.channel !== undefined) {
                curSvcType = retObjCurSvc.channel.channelType;
                curSvcCCID = retObjCurSvc.channel.ccid;
            }
        }

        // 3. Check Last service
        if ((retObjCurSvc.result === false) || (curSvcType === undefined)) {
            lastSvc = hx.svc.WEB_STORAGE.getLastService();
            if (lastSvc !== undefined) {
                lastSvcCCID = lastSvc.ccid;
                lastSvcGroup = lastSvc.group;
                retObjLastSvc = hx.svc.CH_LIST.getChannel({
                        ccid : lastSvcCCID,
                        group : lastSvcGroup
                });
                if (true === retObjLastSvc.result) {
                    if (retObjLastSvc.channel !== undefined) {
                        curSvcType = retObjLastSvc.channel.channelType;
                        curSvcCCID = retObjLastSvc.channel.ccid;
                    }
                }
            }
        }

        // 3. Check First service
        if (curSvcType === undefined || curSvcCCID === undefined) {
            lastSvc = hx.svc.CH_LIST.getAvailableFirstChannel();
            curSvcCCID = lastSvc.channel.ccid;
            curSvcType = lastSvc.channel.channelType;
        }

        // change Channel
        if ((undefined !== curSvcCCID) && (undefined !== curSvcType)) {
            //type : (channel.channelType === 0) ? 'TV' : 'RADIO',
            // Get Last TV / Radio Service
            retObjLastTvRadioSvcCcid = hx.svc.WEB_STORAGE.getLastTvRadioService((curSvcType === 0) ? 'RADIO' : 'TV' );

            if ((retObjLastTvRadioSvcCcid !== undefined) && (retObjLastTvRadioSvcCcid !== null)) {
                // Check tune available
                lastSvc = hx.svc.CH_LIST.getChannel({
                        ccid : retObjLastTvRadioSvcCcid,
                        group : (curSvcType === 0) ? 'RADIO' : 'TV'
                });

                if (true === lastSvc.result) {
                    bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel : lastSvc.channel
                    });
                } else {
                    bChangeable = false;
                }

                if (bChangeable === false) {
                    retObjLastTvRadioSvcCcid = null;
                }
            }
            if (bChangeable === false) {
                if ((retObjLastTvRadioSvcCcid === undefined) || (retObjLastTvRadioSvcCcid === null)) {
                    lastSvc = hx.svc.CH_LIST.getAvailableChannelWithGroup((curSvcType === 0) ? 'RADIO' : 'TV');
                }
                if (lastSvc !== undefined) {
                    bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel : lastSvc.channel
                    });
                } else {
                    bChangeable = false;
                }
            }
            if (bChangeable === false) {
                if (hx.config.useTVStreaming === false) {
                    hx.msg('response', {
                        text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                        auto_close : true,
                        close_time : 3000
                    });
                } else {
                    hx.msg('response', {
                        text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                        auto_close : true,
                        close_time : 3000
                    });
                }
                return;
            }
            curSvcType = lastSvc.channel.channelType;
            hx.svc.AV_CTRL.channelChange({
                    'channel' : lastSvc.channel,
                    'group' : (lastSvc.channel.channelType === 0) ? 'TV' : 'RADIO'
            });
        }
    }
    fnLiveIPOTACheck () : void {
        var that = this;
        if(!!hx.config.useLiveIPOTA) {
            if(that.mliveOtaChecked === false) {
                that.mliveOtaChecked = true;

                var fn = function(xml) {
                    var version : string;
                    try{
                        version = $(xml).find('version').text();
                    } catch (e) {
                        hx.log('error', 'fnLiveIPOTACheck' + 'EXCEPT: ' + e);
                    }

                    if(!!version) {
                        hx.log('CPgLiveController', '_____found ip ota new version_____');
                        var systemInfo : any = hx.svc.SETTING_UTIL.getSystemInfo();
                        var currentVersion : string = systemInfo.softwareVersion;
                        var newVersion : string = systemInfo.softwareVersion.substr(0, 6) + version;
                        var yes : string = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID'), start : string = hx.l('LOC_START_THE_UPDATE_ID').toUpperCase();
                        var strQuery : string = 'startBootupHumaxOta';
                        var handleOTADlg : any = hx.msg('ssuDialog', {
                            title : hx.l('LOC_SOFTWAREUPDATE_ID'),
                            text : hx.l('LOC_NEWSWDETECTED_ID'),
                            text1 : hx.l('LOC_DOYOUWANTUPDATE_ID'),
                            first_line_arr : [hx.l('LOC_CURSWVERSION_ID'), currentVersion],
                            second_line_arr : [hx.l('LOC_NEWSWVERSION_ID'), newVersion],
                            auto_close : true,
                            timeout : 30000, // 30 seconds time out, automatically ota starts according to scenario.
                            btn_title_arr : [yes, no],
                            dia_class : 'osd',
                            callback_fn : function(_status) {
                                if (_status === yes) {
                                    hx.msg.close('confirm', handleOTADlg);
                                    that.sendEventToEntity({
                                        alEvent : 'alChangePage',
                                        target : 'CPgSettings?set=' + strQuery,
                                        me : that
                                    });
                                }
                                $("#DIA").removeClass("osd");
                                // when closing ssuDialog box, remove osd class to make screen brighter
                            }
                        });
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE" : "CKOTA",
                                "TRS" : "I",
                                "TYPE" : "IP",
                                "DEV_VER" : currentVersion,
                                "NEW_VER" : newVersion
                            });
                        }
                    }
                };
                var err = function() {
                    hx.log('CPgLiveController', '_____not found ip ota new version_____');
                };

                setTimeout(() => {
                    if (that.getCenterPageName() !== 'CPgStandby') {
                        hx.svc.TV_PORTAL.checkNewSoftware({fn:fn, fnError:err});
                    }
                }, 20000);
            }
        }
    }
}

export = cCPgLiveController;
