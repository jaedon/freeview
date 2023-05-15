var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "model/model", "prismux/layout/prism.layout.button.promotion", "prismux/by_controls/prism.button", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __MODEL__, __BUTTON_PROMOTION__, __BUTTON__, __NETFLIX__) {
    ///<reference path="../../def/sugar.d.ts" />
    var SCHEDULE_TERM_ID;
    (function (SCHEDULE_TERM_ID) {
        SCHEDULE_TERM_ID[SCHEDULE_TERM_ID["SINGLE"] = 2] = "SINGLE";
        SCHEDULE_TERM_ID[SCHEDULE_TERM_ID["SERIES"] = 5] = "SERIES";
    })(SCHEDULE_TERM_ID || (SCHEDULE_TERM_ID = {}));
    var eMHEGIsInterruptible;
    (function (eMHEGIsInterruptible) {
        eMHEGIsInterruptible[eMHEGIsInterruptible["UNKNOWN"] = -1] = "UNKNOWN";
        eMHEGIsInterruptible[eMHEGIsInterruptible["NOT_INTERRUPTIBLE"] = 0] = "NOT_INTERRUPTIBLE";
        eMHEGIsInterruptible[eMHEGIsInterruptible["INTERRUPTIBLE"] = 1] = "INTERRUPTIBLE";
    })(eMHEGIsInterruptible || (eMHEGIsInterruptible = {}));
    var cCPgLiveController = (function (_super) {
        __extends(cCPgLiveController, _super);
        function cCPgLiveController(aPageName) {
            var _this = this;
            _super.call(this);
            this.mliveOtaChecked = false;
            this._lastBackTime = 0;
            this._audToggleObj = null;
            this._subToggleObj = null;
            this._dolbyIconObj = null;
            this._dolbyPlusIconObj = null;
            this._statusObj = null;
            this._pinObj = null;
            this._noSvcObj = null;
            this._bActivate = false;
            this._bNeedUpdateChInfo = false;
            this._lastNetflixPWStatus = null;
            this._allVisible = true;
            this._dolbyTimeOutID = -1;
            this._dolbyIconTimer = -1;
            this._dolbyPlusIconTimer = -1;
            this.mHelpLiveShown = false;
            this.mOtaNotiShown = false;
            this._curErrorState = 0;
            //_closeWatchinHDBtnTimer : any;
            this._lastErrorState = {
                command: 'clear',
                text: '',
                timeout: false
            };
            this._errorState = {
                CHANNEL_NOT_SUPPORTED: 0,
                CANNOT_TUNE: 1,
                TUNER_LOCK: 2,
                PARENTAL_LOCK: 3,
                ENCRYPTED_CHANNEL: 4,
                UNKNOWN_CHANNEL: 5,
                CHANNEL_SWITCH_INTERRUPTED: 6,
                CANNOT_TUNE_DUETO_RECORD: 7,
                CANNOT_RESOLVE_IP_CHANNEL: 8,
                BANDWIDTH: 9,
                CANNOT_SUPPORT_CHANNEL_LIST: 10,
                INSUFFICIENT_RESOURCE: 11,
                NOT_FOUND_IN_TRANSPORT: 12,
                MOTOR_MOVING: 13,
                CAS_BLOCK: 14,
                PIN_DENIED: 15,
                AV_UNDERRUN: 16,
                HDMI_BLOCK: 17,
                TIMER_7DAY_BLOCK: 18,
                NO_VIDEO: 19,
                NO_AV: 20,
                NOT_RUNNING: 21,
                NO_EIT: 22,
                PIN_5_TIMES_FAIL: 23,
                FTA_60DAY_LIMIT: 24,
                EMPTY_CHANNEL: 25,
                NO_ERROR: 26
            };
            this._STATE = {
                STATE_CONNECT: 1,
                STATE_PRESENT: 2,
                STATE_STOPPED: 3
            };
            this._PLAYSTATE = {
                PLAYSTATE_STOPPED: 0,
                PLAYSTATE_PLAYING: 1
            };
            this.TScheduledFactoryType = {
                ERECORDING: 0,
                EREMIND: 1,
                EHTTP_LIVE_STREAMING: 4,
                ESERIES_RECORDING: 10
            };
            this._promotionGreenButton = new __BUTTON_PROMOTION__.CGreenButtonPromotion();
            this._preChannel = null;
            this._promotionWatchInHD = new __BUTTON__.CButtonPromotionControl($("#DIA"), hx.l("LOC_WATCH_IN_HD_ID"), function (aData) {
                var linkages = aData.linkage;
                var ExtendedLinkage;
                hx.log('CPgLiveController', 'debug _moveToTheHDChannel');
                if (linkages.length === 1) {
                    if (linkages[0].simul) {
                        var ChannelInfo = hx.svc.CH_LIST.getChannel({
                            ccid: linkages[0].channel,
                            group: 'TV'
                        });
                        var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel: ChannelInfo.channel
                        });
                        if (bChangeable === false) {
                            if (hx.config.useTVStreaming === false) {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                        }
                        else {
                            hx.svc.AV_CTRL.channelChange({
                                channel: ChannelInfo.channel,
                                group: 'TV'
                            });
                        }
                    }
                }
                else if (linkages.length === 2) {
                    if ((linkages[0].simul && !linkages[1].simul) || (!linkages[0].simul && linkages[1].simul)) {
                        if (linkages[0].simul) {
                            ExtendedLinkage = linkages[0];
                        }
                        else {
                            ExtendedLinkage = linkages[1];
                        }
                        var ChannelInfo = hx.svc.CH_LIST.getChannel({
                            ccid: ExtendedLinkage.channel,
                            group: 'TV'
                        });
                        hx.svc.AV_CTRL.channelChange({
                            channel: ChannelInfo.channel,
                            group: 'TV'
                        });
                    }
                    else if (linkages[0].simul && linkages[1].simul) {
                        var ChannelInfoA = hx.svc.CH_LIST.getChannel({
                            ccid: linkages[0].channel,
                            group: 'TV'
                        });
                        var ChannelInfoB = hx.svc.CH_LIST.getChannel({
                            ccid: linkages[1].channel,
                            group: 'TV'
                        });
                        var dlg = prism.createExtendedEventLinkageDescriptorButtonDlg(null, [
                            {
                                text: ChannelInfoA.channel.name,
                                fn: function () {
                                    hx.log("debug", ChannelInfoA.channel.name + " ChannelInfoA Move");
                                    hx.svc.AV_CTRL.channelChange({
                                        channel: ChannelInfoA.channel,
                                        group: 'TV'
                                    });
                                }
                            },
                            {
                                text: ChannelInfoB.channel.name,
                                fn: function () {
                                    hx.log("debug", ChannelInfoB.channel.name + " ChannelInfoB Move");
                                    hx.svc.AV_CTRL.channelChange({
                                        channel: ChannelInfoB.channel,
                                        group: 'TV'
                                    });
                                }
                            }
                        ]);
                        dlg.open({
                            onClosed: function () {
                                _this.setKeyHookingControl(dlg);
                            }
                        });
                        _this.setKeyHookingControl(dlg);
                    }
                }
            });
            this._RCTButtonDlg = null;
            this._stateMhegInterruptible = -1 /* UNKNOWN */;
            this._prevRctInfo = null;
            this._prevStateMhegInterruptible = null;
            this._dlgNCD = null;
            hx.logadd('CPgLiveController');
            hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, function (aParam) {
                _this._on_ChannelChangeSucceeded(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, function (aParam) {
                _this._on_ChannelChangeError(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('PlayStateChange', this, function (aParam) {
                _this._on_PlayStateChange(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('ParentalRatingChange', this, function (aParam) {
                _this._on_ParentalRatingChange(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('evtSetChannel', this, function (aParam) {
                _this._on_evtSetChannel(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('audioChange', this, function (aParam) {
                _this._on_audioChange(aParam);
            });
            if (ENV.op === ENV.listOP.UKDTT) {
                hx.svc.AV_CTRL.addEventCb('RctInfoChanged', this, this.onRctInfoChanged);
                hx.svc.AV_CTRL.addEventCb('ProgrammesChanged', this, this.onProgrammesChanged);
                hx.svc.AV_CTRL.addEventCb('RequestPincode', this, this.onRequestPincode);
                hx.svc.AV_CTRL.addEventCb('NCDChanged', this, this.onNCDChanged);
                hx.svc.AV_CTRL.addEventCb('RequestMhegTune', this, function (aChannel, aServiceID, aTuneMode) {
                    hx.log('CPgLiveController', '_');
                    hx.log("debug", "RequestMhegTune received: aServiceID: " + aServiceID);
                    hx.log("debug", "RequestMhegTune received: tuneInfo: " + aTuneMode);
                    hx.svc.AV_CTRL.channelChange({ channel: aChannel }, aTuneMode);
                });
                hx.svc.AV_CTRL.addEventCb('RequestMhegInterrupt', this, function (aIsInterruptible) {
                    hx.log("debug", "RequestMhegInteerrupt: " + aIsInterruptible);
                    if (_this._stateMhegInterruptible !== -1 /* UNKNOWN */) {
                        _this._prevStateMhegInterruptible = _this._stateMhegInterruptible;
                    }
                    _this._stateMhegInterruptible = aIsInterruptible;
                    if (!aIsInterruptible) {
                        _this._closeGreenButton();
                    }
                    hx.svc.SETTING_UTIL.doingKeyGrab(_this.visible());
                    _this.DEV();
                });
                hx.svc.RECORDINGS.addEventCb('NCDChanged', this, this.onNCDChanged);
                hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, function (aButtonInfo) {
                    if (_this.getCenterPageName() === _this.name) {
                        if (!_this.getPageVisible("CPgInfoHub") && !_this.getPageVisible("CPgDetailInfo")) {
                            var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                            dlg.open({
                                onClosed: function () {
                                    _this.setKeyHookingControl(dlg);
                                }
                            });
                            _this.setKeyHookingControl(dlg, true);
                        }
                    }
                });
            }
            var self = this;
            hx.svc.APP_MGR.addEventCb('HbbTvAppEnter', this, function () {
                self._promotionGreenButton.hideButton(false);
                //hx.svc.SETTING_UTIL.doingKeyGrab(false);
            });
            if (hx.config.netflix) {
                hx.svc.NETFLIX.addEventCb('NetflixStatusChanged', this, function () {
                    if (hx.svc.NETFLIX.isRunning() === true) {
                        self._promotionGreenButton.hideButton(true);
                        self._promotionWatchInHD.hideButton();
                    }
                });
            }
        }
        cCPgLiveController.prototype.DEV = function () {
            var devBoard = $("#debugger-green-button");
            if (devBoard) {
                var elPlus = devBoard.find("#debugger-plus");
                if (elPlus && elPlus.length === 0) {
                    elPlus = $("<div>", { id: "debugger-plus" });
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
        };
        cCPgLiveController.prototype.create = function (aEntity, aPageUrl) {
            // icon
            var that = this;
            $.extend(that, {
                $ac3: $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + '<p id="dolby" class="ico"></p>' + '</div>'),
                $eac3: $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + '<p id="dolbyPlus" class="ico"></p>' + '</div>')
            });
            hx.al.compose(that);
            /////////////////////////////////
            // check no service
            var result = hx.svc.CH_LIST.getChannelCount();
            if (result.chCnt === 0) {
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
                }
                else {
                    // normal booting
                    hx.log('CPgLiveController', '### normal wake up');
                }
                that._changeToLastService();
                hx.log('debug', '[LIVE] hx.il.isGoingOperationFromStandby = false;');
                hx.il.isGoingOperationFromStandby = false;
                // update chinfo manually
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgChInfo?update'
                });
                // check P/P & login
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgMain/checkPrivacyVersion',
                    me: that
                });
            }
            else if (aPageUrl.query === 'startIntroVideo') {
                if (result.chCnt > 0) {
                    hx.log('CPgLiveController', '### try to play promotion video');
                    var proRet = that.playPromotionVideo();
                    if (proRet === false) {
                        hx.log('CPgLiveController', '### no promotion video found');
                        that._changeToLastService();
                    }
                }
                //checkDetachableStorage
                if (hx.config.useDetachableStorage) {
                    hx.svc.SETTING_UTIL.checkDetachableStorage();
                }
            }
            else {
                if (result.chCnt > 0) {
                    that.startHelp();
                    that.mHelpLiveShown = true;
                }
            }
            if (that._bNeedUpdateChInfo) {
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgChInfo?update'
                });
                that._bNeedUpdateChInfo = false;
            }
            /////////////////////////////////
            // recovery last error state
            that._refreshLastErrorState();
            // refresh irdeto error message, if stb use the irdeo CA system.
            that._refreshLastSystemErrorMessage();
            that.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
            if (aPageUrl.query !== 'refresh') {
                that._checkCurrentStatus();
            }
            that.fnLiveIPOTACheck();
            that._showGreenButton();
            that.setKeyHookingControl(that._promotionWatchInHD, false, true);
            if (that._dlgNCD) {
                that.setKeyHookingControl(that._dlgNCD);
                that.setKeyHookingControl(that._dlgNCD);
            }
        };
        cCPgLiveController.prototype._checkShowButton = function () {
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
        };
        cCPgLiveController.prototype._showGreenButton = function (aForce) {
            if (this._checkShowButton()) {
                if (this._stateMhegInterruptible !== 0 /* NOT_INTERRUPTIBLE */) {
                    this.setVisible(this._promotionGreenButton.showButton(aForce));
                }
            }
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        cCPgLiveController.prototype._showWatchInHD = function () {
            var a = this;
            if (this._checkShowButton()) {
                this._promotionWatchInHD.showButton();
            }
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        cCPgLiveController.prototype.visible = function () {
            var ret = this._promotionGreenButton.isShowing() || _super.prototype.visible.call(this);
            if (!ret) {
                if (this._stateMhegInterruptible !== 0 /* NOT_INTERRUPTIBLE */) {
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
        };
        cCPgLiveController.prototype.reqOpenRCTDialog = function () {
            var _this = this;
            var ret = false;
            var greenButton = this._promotionGreenButton;
            var rctInfos = greenButton.getData();
            /* TODO
             * load RCT dialog
             * bglee@humaxdigital.com, 17:31, 2014. 8. 6.
             */
            if (rctInfos) {
                greenButton.hideButton(true);
                var dlg = prism.createRCTButtonDlg(null, rctInfos, function (aRctInfo, aIsReminder) {
                    if (aIsReminder) {
                        /* TODO
                         * how to find programme from RctLinkInfo
                         * bglee@humaxdigital.com, 10:40, 2014. 8. 8.
                         */
                        var programme = null;
                        hx.svc.RECORDINGS.record({
                            programme: programme,
                            factoryType: 1 /* EREMIND */,
                            finished: function () {
                                hx.log('CPgLiveController', "reminder");
                            }
                        });
                    }
                    else {
                        /* TODO
                         * if current channel is not available
                         * bglee@humaxdigital.com, 10:24, 2014. 8. 8.
                         */
                        var termIdToFactoryType = {
                            2: 0 /* ERECORDING */,
                            5: 10 /* ESERIES_RECORDING */
                        };
                        var currentChannelInfo = hx.svc.CH_LIST.getCurrentChannel();
                        var channelID = currentChannelInfo.result ? currentChannelInfo.channel.ccid : null;
                        hx.log("debug", "hx.svc.RECORDINGS.recordCRID(" + [termIdToFactoryType[aRctInfo.termId], aRctInfo.mediaUri, aRctInfo.eventName, channelID].join(", ") + ")");
                        hx.svc.RECORDINGS.recordCRID(termIdToFactoryType[aRctInfo.termId], aRctInfo.mediaUri, aRctInfo.eventName, channelID);
                    }
                }, function () {
                    hx.log("debug", "roll back key routine");
                    _this.setKeyHookingControl(dlg);
                    _this._showGreenButton(true);
                });
                dlg.setOnCheckSchedule(function (rctInfo) {
                    var scheduledList = hx.svc.RECORDINGS.getScheduledList();
                    var crids = [];
                    var termId;
                    dlg.hideRecSingle();
                    dlg.hideRecSeries();
                    var i, len = scheduledList.length;
                    for (i = 0; i < len; i++) {
                        if (rctInfo.termId == 2 /* SINGLE */) {
                            crids.push(scheduledList[i].getProgrammeCRID());
                            termId = 2 /* SINGLE */;
                        }
                        else if (rctInfo.termId == 5 /* SERIES */) {
                            var cridsLen = scheduledList[i].getGroupCRIDs().length;
                            var cridIdx;
                            for (cridIdx = 0; cridIdx < cridsLen; cridIdx++) {
                                crids.push(scheduledList[i].getGroupCRIDs()[cridIdx]);
                            }
                            termId = 5 /* SERIES */;
                        }
                    }
                    var j, cridsLen = crids.length;
                    for (j = 0; j < cridsLen; j++) {
                        if (crids[j] == rctInfo.mediaUri) {
                            if (termId === 2 /* SINGLE */) {
                                dlg.showRecSingle();
                                break;
                            }
                            else if (termId === 5 /* SERIES */) {
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
        };
        cCPgLiveController.prototype.destroy = function () {
            //this._hideGreenBtn();
            //TODO: close watch in hd
            if (hx.config.useNetflixPW) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgMain/CloseNetflixPW',
                    me: this
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
            this.setKeyHookingControl(this._promotionWatchInHD);
            //<any>window.watchInHD = this._promotionWatchInHD;
            hx.svc.SETTING_UTIL.doingKeyGrab(true);
        };
        cCPgLiveController.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var kc = aParam.alKey, k = hx.key;
            var ct;
            var diff, bPrismLiteSMode = false;
            var bRet = true;
            hx.log('CPgLiveController', '######  KEY : ' + kc);
            if (this._bActivate === false) {
                return;
            }
            var bisMpbOnGoing = hx.svc.PLAYBACK.isMpbOnGoing();
            if (bisMpbOnGoing && (kc !== k.KEY_SEARCH)) {
                return;
            }
            switch (kc) {
                case k.KEY_CHANNEL_UP:
                    hx.svc.AV_CTRL.naviChannel({
                        'direction': 'next'
                    });
                    bConsumed = true;
                    break;
                case k.KEY_CHANNEL_DOWN:
                    hx.svc.AV_CTRL.naviChannel({
                        'direction': 'prev'
                    });
                    bConsumed = true;
                    break;
                case k.KEY_BACK:
                    // prevent too fast chaning betwin two
                    ct = new Date().getTime();
                    diff = Math.abs(ct - this._lastBackTime);
                    if (diff > 1000) {
                        bRet = hx.svc.AV_CTRL.naviChannel({
                            'direction': 'back'
                        });
                        if (bRet === false) {
                            if (hx.config.useTVStreaming === false) {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                        }
                        else {
                            this._lastBackTime = ct;
                        }
                    }
                    bConsumed = true;
                    break;
                case k.KEY_SCREEN_MODE_NEXT:
                    this.sendEventToEntity({
                        alEvent: 'alMessageToPage',
                        target: 'CPgWide?show=toggle'
                    });
                    bConsumed = true;
                    break;
                case k.KEY_OK:
                    //this._hideGreenBtn();
                    this._promotionGreenButton.hideButton(true);
                    this._promotionWatchInHD.hideButton();
                    this.sendEventToEntity({
                        alEvent: 'alMessageToPage',
                        target: 'CPgInfoHub?iPlate=toggle'
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
                        alEvent: 'alMessageToPage',
                        target: 'CPgInfoHub?channelList=toggle'
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
                            alEvent: 'alMessageToPage',
                            target: 'CPgDcn?active=' + (kc - k.KEY_0).toString()
                        });
                        bConsumed = true;
                    }
                    break;
                case k.KEY_SEARCH:
                    this._promotionGreenButton.hideButton(true);
                    this._promotionWatchInHD.hideButton();
                    this.sendEventToEntity({
                        alEvent: 'alMessageToPage',
                        target: 'CPgSmartSearch?type=live'
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
                switch (kc) {
                    case k.KEY_NETFLIX:
                        this._pinMsgClose();
                        break;
                    default:
                        break;
                }
            }
            return bConsumed;
        };
        cCPgLiveController.prototype._on_alMessageToPage = function (aParam) {
            var result;
            if (aParam.query === 'active') {
                this._bActivate = true;
            }
            else if (aParam.query === 'showpin') {
                if (hx.config.useNetflixPW) {
                    var srvNetflix = hx.svc.NETFLIX;
                    if (this._lastErrorState.command !== "clear") {
                        if (srvNetflix.getPwStatus()) {
                            return;
                        }
                        else {
                            if (this._lastNetflixPWStatus === 'hide') {
                                srvNetflix.start({
                                    type: __NETFLIX__.START_TYPE.NETFLIX_BUTTON
                                });
                                this._lastNetflixPWStatus = null;
                                return;
                            }
                        }
                    }
                    this._lastNetflixPWStatus = null;
                }
                this._refreshLastErrorState();
            }
            else if (aParam.query === 'hidepin') {
                if (hx.config.useNetflixPW) {
                    if (aParam.data.hidepin === 'lunchApp') {
                        this._lastNetflixPWStatus = null;
                    }
                    var srvNetflix = hx.svc.NETFLIX;
                    if (srvNetflix.getPwStatus() && !this._pinObj) {
                        if (aParam.data.hidepin !== 'lunchApp') {
                            hx.log('CPgLiveController', 'alMessageToPage hidepin _lastNetflixPWStatus : hide');
                            this._lastNetflixPWStatus = 'hide';
                        }
                        this.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgMain/CloseNetflixPW',
                            me: this
                        });
                        return;
                    }
                }
                this._pinMsgClose();
            }
            else if (aParam.query === 'nochannel') {
                this._noServiceDlgCreate();
            }
            else if (aParam.query === 'hideerrorstate') {
                result = this._errorStatus(null, null, 'clear');
                this._showServiceStatus(result);
            }
            else if (aParam.query === 'hideNoChannelConfirm') {
                this._noServiceDlgClose();
            }
            else if (aParam.query === 'restoreNoChannelConfirm') {
                var result = hx.svc.CH_LIST.getChannelCount();
                if (result.chCnt === 0) {
                    this._noServiceDlgCreate();
                }
            }
            else if (aParam.query === 'hideInfoHub') {
                this._showGreenButton(true);
            }
        };
        cCPgLiveController.prototype._on_alPagesVisible = function (aParam) {
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        cCPgLiveController.prototype._on_evtSetChannel = function (aParam) {
            var channel = aParam.channel;
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
            var lastSvc = hx.svc.WEB_STORAGE.getLastService();
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
                var helpO = hx.svc.WEB_STORAGE.getHelp();
                if (!helpO.ltv) {
                    this.startHelp();
                }
                else {
                    this.mHelpLiveShown = true;
                }
            }
            //hide promotionWatchInHD
            var promotionWatchInHD = this._promotionWatchInHD;
            if (promotionWatchInHD.isShowing()) {
                promotionWatchInHD.hideButton();
            }
        };
        /*
         * ERROR State --------------------- start
         * */
        cCPgLiveController.prototype._showServiceStatus = function (aParam) {
            var _this = this;
            var bPbGoing;
            var cmd = (aParam.command !== undefined) ? aParam.command : '';
            var text = (aParam.text !== undefined) ? aParam.text : '';
            hx.log('CPgLiveController', '_showServiceStatus - cmd : ' + cmd);
            // it first 4 character is 'LOC_', it is string id!!!!
            if (text.substr(0, 4) === 'LOC_') {
                text = hx.l(text);
            }
            setTimeout(function () {
                switch (cmd) {
                    case 'clear':
                        // remove status message
                        _this._statusMsgClose();
                        _this._pinMsgClose();
                        break;
                    case 'status':
                        bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                        if ((_this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                            _this._statusMsgCreate(text);
                        }
                        break;
                    case 'ratingBlock':
                        if (_this.getPageVisible('CPgInfoHub') === true) {
                            break;
                        }
                        bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                        if ((_this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                            _this._pinMsgCreate('ratingBlock');
                        }
                        break;
                    case 'pin':
                        if (_this.getPageVisible('CPgInfoHub') === true) {
                            break;
                        }
                        bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                        if ((_this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                            _this._pinMsgCreate('pin');
                        }
                        break;
                    case 'pinNoEit':
                        if (_this.getPageVisible('CPgInfoHub') === true) {
                            break;
                        }
                        bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
                        if ((_this.getCenterPageName() === 'CPgLiveController') && !bPbGoing) {
                            _this._pinMsgCreate("noEit");
                        }
                        break;
                    case 'pinClose':
                        _this._pinMsgClose();
                        break;
                    case 'response':
                        if (_this._bActivate === true) {
                            _this._responseMsgCreate(text);
                        }
                        break;
                    default:
                        break;
                }
            }, 100);
        };
        cCPgLiveController.prototype._showOrHideWatchInHD = function () {
            var promotionWatchInHD = this._promotionWatchInHD;
            if (promotionWatchInHD.setFromVolatileData()) {
                var data = promotionWatchInHD.getData();
                var current = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                if (current && data.ccid === current.ccid) {
                    this._showWatchInHD();
                }
                else {
                    promotionWatchInHD.setData();
                }
            }
            else {
                promotionWatchInHD.setData();
            }
        };
        cCPgLiveController.prototype._on_ChannelChangeSucceeded = function (aParam) {
            var channel = (aParam) ? aParam.channel : undefined;
            var promotionWatchInHD = this._promotionWatchInHD;
            hx.log('CPgLiveController', '### ChannelChangeSucceeded ' + ((channel) ? channel.name : 'undefined'));
            hx.svc.AV_CTRL.setChannelChanging(false);
            //destroy greenButton
            this._promotionGreenButton.setData();
            this._promotionGreenButton.hideButton();
            if (this._preChannel === null) {
                this._preChannel = channel.majorChannel;
                this._showOrHideWatchInHD();
            }
            else {
                var volatileData = promotionWatchInHD.getVolatileData();
                //if (this._preChannel !== channel.majorChannel) {
                if (volatileData && volatileData.ccid == channel.ccid && !promotionWatchInHD.isShowing()) {
                    this._showOrHideWatchInHD();
                    this._preChannel = channel.majorChannel;
                }
                else {
                    if (volatileData && volatileData.ccid != channel.ccid && promotionWatchInHD.isShowing()) {
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
        };
        cCPgLiveController.prototype._on_ChannelChangeError = function (aParam) {
            var channel = (aParam) ? aParam.channel : undefined;
            var errorState = aParam.errorState;
            var param = null;
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
        };
        cCPgLiveController.prototype._on_PlayStateChange = function (aParam) {
            var param, state, error;
            if (aParam.from === 'VideoBroadcast') {
                state = aParam.state;
                error = aParam.error;
                hx.log('CPgLiveController', '### PlayStateChange : VB state: ' + state + ' error : ' + error);
                if (state === this._STATE.STATE_CONNECT) {
                    if (error !== undefined) {
                        param = this._errorStatus(null, error);
                        this._showServiceStatus(param);
                    }
                }
                else if (state === this._STATE.STATE_PRESENT || state === this._STATE.STATE_STOPPED) {
                    param = this._errorStatus(null, null, 'clear');
                    this._showServiceStatus(param);
                }
            }
            else if (aParam.from === 'VideoOnDemand') {
                state = aParam.playState;
                error = aParam.error;
                hx.log('CPgLiveController', '### PlayStateChange : VOD state: ' + state + ' error : ' + error);
                if (state === this._PLAYSTATE.PLAYSTATE_PLAYING) {
                    param = this._errorStatus(null, null, 'clear');
                    this._showServiceStatus(param);
                    // close irdeto error message, if it uses the irdeo CA system.
                    this._hideLastSystemErrorMessage();
                }
            }
        };
        cCPgLiveController.prototype._on_ParentalRatingChange = function (aParam) {
            var bblocked = aParam.blocked;
            var param;
            hx.log('CPgLiveController', '### ParentalRatingChange rating : ' + aParam.rating + 'bblockedd : ' + aParam.blocked);
            if (bblocked === true) {
                param = this._errorStatus(null, 3, 'ratingBlock');
            }
            else {
                param = this._errorStatus(null, 3, 'close');
            }
            this._showServiceStatus(param);
        };
        cCPgLiveController.prototype._on_audioChange = function (aParam) {
            this._checkDolby();
        };
        cCPgLiveController.prototype._errorStatus = function (aChannel, aErrorState, aOpt) {
            // must be syncronized with eOplPlaybackError in oplvideobroadcasthandle_def.h
            var errtext = [
                ["Channel not supported by tuner.", false],
                ['LOC_NO_SIGNAL_ID', false],
                ["Tuner locked by other object.", false],
                ['LOC_LIVE_PC_BLOCK_ID', false],
                ["Encrypted channel, key/module missing.", false],
                ["Unknown channel.", false],
                ["Channel switch interrupted.", false],
                ['LOC_CANNOTCHANGEWHILEREC_ID', false],
                ["Cannot resolve URI of referenced IP channel.", false],
                ["Insufficient bandwidth.", false],
                ["Channel cannot be changed.", false],
                ["Insufficient resources are available to present the given channel.", false],
                ["Specified channel not found in transport stream.", false],
                ["DiSEqC Motor is now moving.", false],
                ["Waiting CAS admission", false],
                ["Pin code is denied", false],
                ['LOC_CHANNEL_NOT_AVAILABLE_ID', false],
                ['LOC_HDCP_MSG_ID', false],
                ["7DayTime Block (??)", false],
                ['LOC_VIDEO_NOTAVAILABLE_ID', false],
                ['LOC_AV_NOTAVAILABLE_ID', false],
                ["Service is not running regarding running flag in SDT", false],
                ["No EIT (Irdeto Specific)", false],
                ["Pin code is denied at 5 times.", false],
                ["Encounter limitation of watching FTA in 60 days.", false],
                ['LOC_CHANNEL_NOT_AVAILABLE_ID', false],
                ['LOC_TERRESTRIAL_SHORT_CIRCUIT_ID', false]
            ];
            var text = '';
            var btimeout = true;
            var dlg;
            if (aOpt === 'clear') {
                dlg = 'clear';
            }
            else if (aErrorState < errtext.length) {
                switch (aErrorState) {
                    case this._errorState.PARENTAL_LOCK:
                        dlg = 'pin';
                        if (aOpt === 'close') {
                            dlg = 'pinClose';
                        }
                        else if (aOpt === 'ratingBlock') {
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
                        }
                        else {
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
            }
            else {
                dlg = 'status';
                text = 'Unknown error';
                btimeout = false;
            }
            var param = {
                'command': dlg,
                'text': text,
                'timeout': btimeout
            };
            if (dlg !== 'response') {
                this._lastErrorState.command = param.command;
                this._lastErrorState.text = param.text;
                this._lastErrorState.timeout = param.timeout;
            }
            return param;
        };
        cCPgLiveController.prototype._refreshLastErrorState = function (fnValid) {
            var param = {
                'command': this._lastErrorState.command,
                'text': this._lastErrorState.text,
                'timeout': this._lastErrorState.timeout
            };
            if (fnValid && fnValid(this._lastErrorState.command)) {
                this._showServiceStatus(param);
            }
            else {
                this._showServiceStatus(param);
            }
        };
        cCPgLiveController.prototype._refreshLastSystemErrorMessage = function () {
            if (ENV.listCAS.IR === ENV.cas) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgCasIrUiNotifier?refreshIrdetoSystemErrorMsg'
                });
            }
            else if (ENV.listCAS.IRUC === ENV.cas) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgCasIrUcUiNotifier?refreshIrdetoSystemErrorMsg'
                });
            }
            else if (ENV.listCAS.NA === ENV.cas) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgCasNaUiNotifier?refreshNagraSystemErrorMsg'
                });
            }
        };
        cCPgLiveController.prototype._isEmpty = function (value) {
            return (typeof value === undefined || value === null || value === undefined || value.length === 0);
        };
        cCPgLiveController.prototype.onRctInfoChanged = function (aLinkCount, aRctInfoCollection) {
            var _this = this;
            var greenButton = this._promotionGreenButton;
            if (this._stateMhegInterruptible === 0 /* NOT_INTERRUPTIBLE */) {
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
                var fnCheckValidInfo = function () {
                    var ret = true;
                    if (_this._stateMhegInterruptible === 0 /* NOT_INTERRUPTIBLE */) {
                        ret = false;
                        hx.log("debug", "mheg not interruptible, unset data");
                    }
                    else if (_this._stateMhegInterruptible === 1 /* INTERRUPTIBLE */ && _this._prevStateMhegInterruptible === 0 /* NOT_INTERRUPTIBLE */) {
                        ret = !_.isEqual(_this._prevRctInfo, aRctInfoCollection);
                        hx.log("debug", "rct info comparison result: " + ret);
                    }
                    hx.log("debug", "ret: " + ret);
                    return ret;
                };
                if (fnCheckValidInfo()) {
                    greenButton.setData(aRctInfoCollection);
                }
                else {
                    greenButton.setData();
                }
                if (hx.al.entity.getCenterPageName() === "CPgLiveController") {
                    this._showGreenButton();
                }
            }
        };
        cCPgLiveController.prototype._closeGreenButton = function () {
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
        };
        cCPgLiveController.prototype.onNCDChanged = function (aStartTime, aDuration, aMessage) {
            var _this = this;
            if (hx.svc.CHANNEL_SEARCH_T.isStartScan()) {
                return;
            }
            var currentTime = Date.now();
            var isCurrentValid = aStartTime === 0 ? true : (aStartTime + aDuration) <= currentTime;
            var aCallback = function (res) {
                switch (res) {
                    case "NOW":
                        if (hx.config.netflix && hx.svc.NETFLIX.isRunning()) {
                            hx.log('debug', 'Stop(suspend) Netflix & Search channel again by NCD changed');
                            hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE_WITH_NETFLIX_EXIT);
                        }
                        hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
                        hx.svc.BRIDGE.requestStopPlayback();
                        hx.svc.SETTING_UTIL.setNCDNotifyData(0);
                        _this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgSettings?set=startAutomaticSearch',
                            me: _this
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
                            factoryType: 9 /* ESCHEDULED_RECORDING_AOS_NOTIFY */,
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
                    aMessage: aMessage,
                    aExpectedTime: new Date(aStartTime).format("HH:MM:ss, m/d/yyyy"),
                    isValid: !isCurrentValid,
                    validOnNetflix: true
                }, aCallback);
                this._dlgNCD.open({
                    onClosed: function () {
                        _this.setKeyHookingControl(_this._dlgNCD);
                        _this._dlgNCD = null;
                    }
                });
                this.setKeyHookingControl(this._dlgNCD);
            }
        };
        cCPgLiveController.prototype.onProgrammesChanged = function (aProgramme) {
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
                                }
                                else {
                                    promotionWatchInHD.hideButton();
                                }
                            }
                            else {
                                promotionWatchInHD.setData({
                                    ccid: chCurrent.ccid,
                                    linkage: linkage
                                });
                                this._showWatchInHD();
                            }
                            break;
                        }
                    }
                }
                else {
                    if (promotionWatchInHD.isShowing()) {
                        promotionWatchInHD.hideButton();
                    }
                }
            }
        };
        cCPgLiveController.prototype.onRequestPincode = function (aMessage) {
            var fnPincodeDialog = function (_status, _input) {
                if (_status === 'stop') {
                    hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                        password: _input,
                        cb: function (aParam) {
                            // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
                            hx.msg.close('pin', compPincode);
                            hx.svc.AV_CTRL.setRequestPincodeResult(!aParam.result);
                        }
                    });
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', compPincode);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', compPincode);
                }
            };
            var compPincode = hx.msg('pin', {
                text: "STRING_" + aMessage,
                callback_fn: fnPincodeDialog,
                auto_close: false,
                dia_class: 'osd',
                enable: true
            });
        };
        /*
         * ERROR State --------------------- end
         * */
        cCPgLiveController.prototype.__makeDispTime = function (aData) {
            var startTime, endTime;
            if (aData.startTime === undefined) {
                startTime = new Date();
            }
            else {
                startTime = new Date(aData.startTime * 1000);
            }
            endTime = new Date(startTime.valueOf() + (aData.duration * 1000));
            return aData.name + ', ' + startTime.format("HH:MM") + '-' + endTime.format("HH:MM") + ', ' + startTime.format("mm.dd");
        };
        cCPgLiveController.prototype._checkDolby = function () {
            var _this = this;
            var f = function () {
                //hx.log('CPgLiveController', '[_checkDolby] : getAudioComponent');
                var stCuraudioInfo = hx.svc.AV_CTRL.getCurAudio();
                var bIsAC3 = false;
                var bIsEAC3 = false;
                if (stCuraudioInfo) {
                    //hx.log('CPgLiveController', '[_checkDolby] : ' + ':' + stCuraudioInfo.encoding);
                    if (stCuraudioInfo.encoding === "AC3") {
                        bIsAC3 = true;
                    }
                    else if (stCuraudioInfo.encoding === "E-AC3") {
                        bIsEAC3 = true;
                    }
                }
                if (bIsEAC3 === true) {
                    _this._dolbyPlusIconShow();
                }
                else if (bIsAC3 === true) {
                    _this._dolbyIconShow();
                }
            };
            clearTimeout(this._dolbyTimeOutID);
            this._dolbyTimeOutID = setTimeout(f, 2000);
        };
        cCPgLiveController.prototype._statusMsgClose = function () {
            //hx.log('error', '_statusMsgClose - this._statusObj : ' + this._statusObj);
            if (this._statusObj !== null) {
                hx.msg.close('status', this._statusObj);
                this._statusObj = null;
            }
        };
        cCPgLiveController.prototype._statusMsgCreate = function (aText) {
            this._statusMsgClose();
            this._statusObj = hx.msg('status', {
                text: aText,
                auto_close: false,
                callback_fn: function (p) {
                    var nowork;
                }
            });
            //hx.log('error', '_statusMsgCreate - that._statusObj : ' + that._statusObj);
        };
        cCPgLiveController.prototype._noServiceDlgClose = function () {
            if (this._noSvcObj !== null) {
                hx.msg.close('confirm', this._noSvcObj);
                this._noSvcObj = null;
            }
        };
        cCPgLiveController.prototype._noServiceDlgCreate = function () {
            var _this = this;
            var text = hx.l('LOC_NOCHANNEL_ID');
            var btn = hx.l('LOC_OK_ID');
            // close previous dialog
            this._noServiceDlgClose();
            var dlgAgain = function () {
                _this._noServiceDlgCreate();
            };
            var cb = function (param) {
                if (param === btn) {
                    _this._noServiceDlgClose();
                    // jump to settings > installation
                    _this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgSettings?set=directSch',
                        me: _this
                    });
                }
                else if (param === 'key_esc' || param === 'key_back') {
                    _this._noServiceDlgClose();
                    setTimeout(dlgAgain, 0);
                }
            };
            // create dialog
            this._noSvcObj = hx.msg('confirm', {
                text: text,
                auto_close: false,
                btn_title_arr: [btn],
                //	timeout : 5000,
                //dia_class : 'osd',
                callback_fn: cb
            });
            // set front panel
            hx.svc.SETTING_UTIL.setFrontPanelString({
                'text': '-NO CHANNEL-'
            });
        };
        cCPgLiveController.prototype._responseMsgCreate = function (aText) {
            hx.msg('response', {
                text: aText,
                auto_close: true,
                close_time: 3000
            });
        };
        cCPgLiveController.prototype._pinMsgClose = function () {
            if (this._pinObj !== null) {
                hx.msg.close('pinPC', this._pinObj);
                this._pinObj = null;
            }
        };
        cCPgLiveController.prototype._pinMsgCreate = function (opt) {
            var that = this, text = hx.l('LOC_LIVE_PC_BLOCK_ID');
            that._pinMsgClose();
            if (opt === 'pin') {
                var curChObj = hx.svc.CH_LIST.getCurrentChannel();
                if (curChObj.channel) {
                    if (curChObj.channel.manualBlock) {
                        text = hx.l('LOC_STR_MESG_806_ID');
                    }
                }
            }
            else {
                // parental control lock
                if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                    var irErrCode = '';
                    if (opt === 'noEit') {
                        if (ENV.listCAS.IR === ENV.cas) {
                            irErrCode = "E44-32";
                        }
                        else {
                            irErrCode = "E044-32";
                        }
                    }
                    else {
                        if (ENV.listCAS.IR === ENV.cas) {
                            irErrCode = "E42-32";
                        }
                        else {
                            irErrCode = "E042-32";
                        }
                    }
                    var message;
                    if (ENV.listCAS.IR === ENV.cas) {
                        message = hx.al.component('cIrErrorMessages');
                    }
                    else {
                        message = hx.al.component('cIrUcErrorMessages');
                    }
                    var cas_ir_errMsgString = message._cas_ir_get_errorMsgString(irErrCode);
                    text = irErrCode + " " + cas_ir_errMsgString;
                }
            }
            that._pinObj = hx.msg('pinPC', {
                text: text,
                auto_close: false,
                //			dia_class : 'osd',
                enable: true,
                callback_fn: function (_status, _input) {
                    if (_status === 'stop') {
                        hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                            password: _input,
                            cb: function (retObj) {
                                if (retObj.result === 0) {
                                    that._pinMsgClose();
                                    setPCStatus(_input, false);
                                }
                                else {
                                    that._pinMsgClose();
                                    that._pinObjWrong = hx.msg('pinPC', {
                                        text: text,
                                        auto_close: true,
                                        enable: false,
                                        timeout: 500,
                                        callback_fn: function (_status, _input) {
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
            var setPCStatus = function (pcPIN, enable) {
                hx.svc.PARENTAL_CTRL.setParentalControlStatus({
                    pcPIN: pcPIN,
                    enable: enable
                });
            };
        };
        cCPgLiveController.prototype._dolbyIconShow = function () {
            var _this = this;
            this._dolbyIconHide();
            clearTimeout(this._dolbyIconTimer);
            $('#wrap').append(this.$ac3);
            this._dolbyIconTimer = setTimeout(function () {
                _this._dolbyIconHide();
            }, 5000);
        };
        cCPgLiveController.prototype._dolbyIconHide = function () {
            clearTimeout(this._dolbyIconTimer);
            if (this.$ac3) {
                this.$ac3.remove();
            }
        };
        cCPgLiveController.prototype._dolbyPlusIconShow = function () {
            var _this = this;
            this._dolbyPlusIconHide();
            clearTimeout(this._dolbyPlusIconTimer);
            $('#wrap').append(this.$eac3);
            this._dolbyPlusIconTimer = setTimeout(function () {
                _this._dolbyPlusIconHide();
            }, 5000);
        };
        cCPgLiveController.prototype._dolbyPlusIconHide = function () {
            clearTimeout(this._dolbyPlusIconTimer);
            if (this.$eac3) {
                this.$eac3.remove();
            }
        };
        cCPgLiveController.prototype.startHelp = function () {
            if (!hx.config.useHelpPage) {
                hx.log('debug', '[startHelp] !hx.config.useHelpPage');
                return;
            }
            var helpViewObj = hx.svc.WEB_STORAGE.getHelp();
            var target = hx.al.entity._repos.get("CPgMain") || this;
            this.$wrap = $(document.getElementById('wrap'));
            if (!helpViewObj.ltv) {
                this.help = hx.al.component('HelpView', {
                    'parent': target,
                    '_type': 'ltv'
                });
                this.$wrap[0].appendChild(this.help.$[0]);
                hx.al.compose(target).append(this.help);
                this.help.on();
            }
        };
        cCPgLiveController.prototype._checkstartOtaNoti = function () {
            var updatedList, datas = [];
            var OtaNoti = hx.svc.WEB_STORAGE.getOtaNoti();
            var NewOtaVersion, uidataObj;
            updatedList = hx.svc.SETTING_UTIL.getUpdatedList();
            datas.push(updatedList.updatedList);
            uidataObj = JSON.parse(datas);
            NewOtaVersion = uidataObj.uidata.otaversion;
            // check live ota notification message
            if (this.mOtaNotiShown === false) {
                if ((OtaNoti.otacheck === undefined) || (OtaNoti.otacheck === null)) {
                    hx.svc.WEB_STORAGE.setOtaNoti({
                        'otacheck': NewOtaVersion
                    });
                }
                if ((OtaNoti.otanoti === undefined) || (OtaNoti.otanoti === null)) {
                    hx.svc.WEB_STORAGE.setOtaNoti({
                        'otanoti': "false"
                    });
                }
                this.startOtaNoti(this);
                this.mOtaNotiShown = true;
            }
        };
        cCPgLiveController.prototype.startOtaNoti = function (aTargetObject) {
            var updatedList, datas = [];
            var OtaNoti = hx.svc.WEB_STORAGE.getOtaNoti();
            var NewOtaVersion, uidataObj;
            updatedList = hx.svc.SETTING_UTIL.getUpdatedList();
            datas.push(updatedList.updatedList);
            uidataObj = JSON.parse(datas);
            NewOtaVersion = uidataObj.uidata.otaversion;
            if ((OtaNoti.otacheck < NewOtaVersion) && (NewOtaVersion !== 0)) {
                //hx.svc.WEB_STORAGE.setOtaNoti(NewOtaVersion);
                hx.svc.WEB_STORAGE.setOtaNoti({
                    'otacheck': NewOtaVersion
                });
                this.$wrap = $(document.getElementById('wrap'));
                this.otanoti = hx.al.component('OtaNoti', {
                    'parent': aTargetObject,
                    '_type': 'otanoti'
                });
                this.$wrap[0].appendChild(this.otanoti.$[0]);
                hx.al.compose(this).append(this.otanoti);
                this.otanoti.on();
            }
        };
        cCPgLiveController.prototype.playPromotionVideo = function () {
            if (!hx.config.useHelpPage) {
                hx.log('debug', '[startHelp] !hx.config.useHelpPage');
                return false;
            }
            var helpViewObj = hx.svc.WEB_STORAGE.getHelp();
            var dsrvBridge = hx.svc.BRIDGE;
            var bret = false;
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
        };
        cCPgLiveController.prototype._checkCurrentStatus = function () {
            var bMedGoing = hx.svc.PLAYBACK.isMpbOnGoing();
            hx.log('CPgLiveController', 'checkCurrentStatus => bMedGoing : ' + bMedGoing);
            var bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing() || hx.svc.TSR.isTsrOngoing();
            hx.log('CPgLiveController', 'checkCurrentStatus => bPbGoing : ' + bPbGoing);
        };
        cCPgLiveController.prototype._changeToLastService = function () {
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        cCPgLiveController.prototype._hideLastSystemErrorMessage = function () {
            if (ENV.listCAS.IR === ENV.cas) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgCasIrUiNotifier?hideIrdetoSystemErrorMsg'
                });
            }
            else if (ENV.listCAS.IRUC === ENV.cas) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgCasIrUcUiNotifier?hideIrdetoSystemErrorMsg'
                });
            }
            else if (ENV.listCAS.NA === ENV.cas) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgCasNaUiNotifier?hideNagraSystemErrorMsg'
                });
            }
        };
        cCPgLiveController.prototype._showNotAvailableIcon = function () {
            this.sendEventToEntity({
                alEvent: 'alMessageToPage',
                target: 'CPgNotAvailable?show'
            });
        };
        cCPgLiveController.prototype._isPrismLiteSModeOn = function () {
            var bIpEpgOn = false;
            var bret = false;
            if ((hx.config.usePrismLiteS === undefined) || (hx.config.usePrismLiteS === false)) {
                bret = false;
            }
            else if (hx.config.usePrismLiteS === true) {
                if (ENV.listModel.IR4000HD === ENV.model) {
                    bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                    if (bIpEpgOn === true) {
                        bret = false;
                    }
                    else {
                        bret = true;
                    }
                }
                else if (ENV.listModel.HMS1000S === ENV.model) {
                    var countryId;
                    countryId = hx.svc.SETTING_UTIL.getCountryId();
                    bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                    switch (countryId) {
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                            bret = false;
                            break;
                        default:
                            bret = false;
                    }
                }
                else {
                    bret = true;
                }
            }
            else {
                bret = false;
            }
            return bret;
        };
        cCPgLiveController.prototype._action_TvRadioKey = function () {
            var ulTvSvcCount = 0, ulRadioSvcCount = 0;
            var retObjCurSvc, curSvcType, curSvcCCID, lastSvc, lastSvcCCID, lastSvcGroup, retObjLastSvc;
            var retObjLastTvRadioSvcCcid, bChangeable = false;
            var chlist;
            chlist = hx.svc.CH_LIST.getChannelCount();
            ulTvSvcCount = hx.svc.CH_LIST.getChannelCount({
                group: 'TV'
            }).chCnt;
            ulRadioSvcCount = hx.svc.CH_LIST.getChannelCount({
                group: 'RADIO'
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
                        ccid: lastSvcCCID,
                        group: lastSvcGroup
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
                retObjLastTvRadioSvcCcid = hx.svc.WEB_STORAGE.getLastTvRadioService((curSvcType === 0) ? 'RADIO' : 'TV');
                if ((retObjLastTvRadioSvcCcid !== undefined) && (retObjLastTvRadioSvcCcid !== null)) {
                    // Check tune available
                    lastSvc = hx.svc.CH_LIST.getChannel({
                        ccid: retObjLastTvRadioSvcCcid,
                        group: (curSvcType === 0) ? 'RADIO' : 'TV'
                    });
                    if (true === lastSvc.result) {
                        bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel: lastSvc.channel
                        });
                    }
                    else {
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
                            channel: lastSvc.channel
                        });
                    }
                    else {
                        bChangeable = false;
                    }
                }
                if (bChangeable === false) {
                    if (hx.config.useTVStreaming === false) {
                        hx.msg('response', {
                            text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    else {
                        hx.msg('response', {
                            text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    return;
                }
                curSvcType = lastSvc.channel.channelType;
                hx.svc.AV_CTRL.channelChange({
                    'channel': lastSvc.channel,
                    'group': (lastSvc.channel.channelType === 0) ? 'TV' : 'RADIO'
                });
            }
        };
        cCPgLiveController.prototype.fnLiveIPOTACheck = function () {
            var that = this;
            if (!!hx.config.useLiveIPOTA) {
                if (that.mliveOtaChecked === false) {
                    that.mliveOtaChecked = true;
                    var fn = function (xml) {
                        var version;
                        try {
                            version = $(xml).find('version').text();
                        }
                        catch (e) {
                            hx.log('error', 'fnLiveIPOTACheck' + 'EXCEPT: ' + e);
                        }
                        if (!!version) {
                            hx.log('CPgLiveController', '_____found ip ota new version_____');
                            var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
                            var currentVersion = systemInfo.softwareVersion;
                            var newVersion = systemInfo.softwareVersion.substr(0, 6) + version;
                            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID'), start = hx.l('LOC_START_THE_UPDATE_ID').toUpperCase();
                            var strQuery = 'startBootupHumaxOta';
                            var handleOTADlg = hx.msg('ssuDialog', {
                                title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                                text: hx.l('LOC_NEWSWDETECTED_ID'),
                                text1: hx.l('LOC_DOYOUWANTUPDATE_ID'),
                                first_line_arr: [hx.l('LOC_CURSWVERSION_ID'), currentVersion],
                                second_line_arr: [hx.l('LOC_NEWSWVERSION_ID'), newVersion],
                                auto_close: true,
                                timeout: 30000,
                                btn_title_arr: [yes, no],
                                dia_class: 'osd',
                                callback_fn: function (_status) {
                                    if (_status === yes) {
                                        hx.msg.close('confirm', handleOTADlg);
                                        that.sendEventToEntity({
                                            alEvent: 'alChangePage',
                                            target: 'CPgSettings?set=' + strQuery,
                                            me: that
                                        });
                                    }
                                    $("#DIA").removeClass("osd");
                                    // when closing ssuDialog box, remove osd class to make screen brighter
                                }
                            });
                            if (hx.svc.STATISTICS) {
                                hx.svc.STATISTICS.statisticsLog({
                                    "CODE": "CKOTA",
                                    "TRS": "I",
                                    "TYPE": "IP",
                                    "DEV_VER": currentVersion,
                                    "NEW_VER": newVersion
                                });
                            }
                        }
                    };
                    var err = function () {
                        hx.log('CPgLiveController', '_____not found ip ota new version_____');
                    };
                    setTimeout(function () {
                        if (that.getCenterPageName() !== 'CPgStandby') {
                            hx.svc.TV_PORTAL.checkNewSoftware({ fn: fn, fnError: err });
                        }
                    }, 20000);
                }
            }
        };
        return cCPgLiveController;
    })(__PAGE_KERNEL__);
    return cCPgLiveController;
});
