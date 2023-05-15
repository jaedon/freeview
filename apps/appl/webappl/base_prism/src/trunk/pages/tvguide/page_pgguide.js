var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "pages/tvguide/cmpt_guideentry", "pages/tvguide/cmpt_guidedcn"], function (require, exports, __PAGE_KERNEL__, __CMPT_GUIDE_ENTRY__, __CMPT_GUIDE_DCN__) {
    var cPgGuide = (function (_super) {
        __extends(cPgGuide, _super);
        function cPgGuide() {
            _super.call(this);
            this._bActive = false;
            this._pgTimerId = null;
            this._3MIN_TIMEOUT = 3 * 60 * 1000;
            this._FRONT_TITLE = '-TV GUIDE-';
        }
        cPgGuide.prototype.create = function (entity, pageUrl) {
            var _this = this;
            this._hidePlaybar();
            // display TV GUIDE title on the front panel
            this._display_FrontTitle(this._FRONT_TITLE);
            // set 3min timeout
            this._setTimeout();
            this.dcn = new __CMPT_GUIDE_DCN__();
            this._guideEntry = new __CMPT_GUIDE_ENTRY__({
                'sugarPage': this,
                'guideDcn': this.dcn
            });
            hx.svc.CLOCK.startClock();
            var $dateTime = this.showDateTime(); // dl_clock addEventCb
            this._guideEntry._$tvg.append($dateTime.addClass('on'));
            hx.al.compose(this);
            // if enter on this page for the first time, launch a help page (promotion page).
            this._launch_HelpView(this);
            this._bActive = true;
            hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, function (aParam) {
                _this._on_ChannelChangeSucceeded(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, function (aParam) {
                _this._on_ChannelChangeError(aParam);
            });
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, function () {
                _this._on_ChannelListUpdate();
            });
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var freeViewDiv = $('<div>', {
                'id': 'tvg',
                'class': 'termsAndConditions'
            });
            var dsrvTVPortal = hx.svc.TV_PORTAL;
            if (dsrvTVPortal.isLogged() && !dsrvTVPortal.isTermsAndConditionsAccepted()) {
                //TODO: msg창 출력
                var dsrvIpEpg = hx.svc.IP_EPG;
                var okBtn = hx.l('LOC_AGREE_ID'), cancelBtn = hx.l('LOC_DISAGREE_ID');
                var msgbox = hx.msg('confirm', {
                    text: hx.l('LOC_FOR_ADVANCED_EPG_SELECTIVE_MSG'),
                    auto_close: false,
                    btn_title_arr: [okBtn, cancelBtn],
                    dia_class: 'osd',
                    callback_fn: function (p) {
                        if (p === okBtn) {
                            hx.msg.close('confirm', msgbox);
                            dsrvTVPortal.acceptTermsAndConditions({
                                accept: true
                            });
                            freeViewDiv.insertBefore($('#tvg').children().first());
                        }
                        else if (p === cancelBtn) {
                            hx.msg.close('confirm', msgbox);
                            dsrvTVPortal.acceptTermsAndConditions({
                                accept: false
                            });
                            freeViewDiv.remove();
                        }
                        else if (p === 'key_esc' || p === 'key_back') {
                            hx.msg.close('confirm', msgbox);
                            dsrvTVPortal.acceptTermsAndConditions({
                                accept: false
                            });
                            freeViewDiv.remove();
                        }
                    }
                });
            }
            else {
                freeViewDiv.insertBefore($('#tvg').children().first());
            }
            //terminate all apps
            var dsrvAppMgr = hx.svc.APP_MGR;
            dsrvAppMgr.destroyHbbTvDeeplinkApplication();
        };
        cPgGuide.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._bActive = false;
            hx.svc.SETTING_UTIL.setFrontPanelString({
                'text': ''
            });
            hx.svc.AV_CTRL.clearEventCb(this);
            hx.svc.CH_LIST.clearEventCb(this);
            hx.svc.CLOCK.stopClock();
            if (this._guideEntry) {
                this._guideEntry.destroy();
                delete this._guideEntry;
                this._guideEntry = null;
            }
            if (this._pgTimerId) {
                clearTimeout(this._pgTimerId);
                this._pgTimerId = null;
            }
            // dcn component
            if (this.dcn) {
                if (this.dcn._timerId !== null) {
                    clearTimeout(this.dcn._timerId);
                }
                if (this.dcn.isActive) {
                    this.dcn.clearDcn();
                }
                delete this.dcn;
                this.dcn = null;
            }
            this.closeAllHookingControls();
        };
        cPgGuide.prototype.isActive = function () {
            return this._bActive;
        };
        cPgGuide.prototype.isNumberKey = function (param) {
            var numTable = [hx.key.KEY_0, hx.key.KEY_1, hx.key.KEY_2, hx.key.KEY_3, hx.key.KEY_4, hx.key.KEY_5, hx.key.KEY_6, hx.key.KEY_7, hx.key.KEY_8, hx.key.KEY_9];
            var guideEntry = this._guideEntry;
            if (numTable.indexOf(param.alKey) > -1) {
                var group, num, actViewName, actViewState, guideDcn;
                actViewName = guideEntry.getCurrentView().name;
                actViewState = guideEntry.getCurrentView().state;
                guideDcn = this.dcn;
                num = (param.alKey - hx.key.KEY_0).toString();
                group = guideEntry.guideChannelList.channelGroup;
                if ((actViewName === 'guideOnAir' || actViewName === 'guideNext' || actViewName === 'guideCatchUp' || actViewName === 'guideBackward') && actViewState === 'idle') {
                    guideEntry.setHistoryFocus();
                    guideDcn.show(num, group);
                    this.append(guideDcn.cmptDcn);
                    guideDcn.cmptDcn.on();
                    this.dcnRefresh();
                }
                return true;
            }
            return false;
        };
        cPgGuide.prototype.hasStackedPage = function (param) {
            var mainLayer = this._guideEntry._mainLayer, hasPage = false;
            if (mainLayer._layerStack.length) {
                hasPage = mainLayer.doKey(param.alKey);
                switch (param.alKey) {
                    case hx.key.KEY_LEFT:
                    case hx.key.KEY_RIGHT:
                    case hx.key.KEY_UP:
                    case hx.key.KEY_DOWN:
                    case hx.key.KEY_CHANNEL_UP:
                    case hx.key.KEY_CHANNEL_DOWN:
                        hasPage = true;
                        break;
                }
            }
            return hasPage;
        };
        cPgGuide.prototype._on_alKey = function (param) {
            var isConsumed, guideEntry = this._guideEntry, focusInfo, keyword, channel, viewState = guideEntry.getCurrentView().state;
            var dsrvRecordings = hx.svc.RECORDINGS;
            if (!this.isActive() || this.hasStackedPage(param) || this.isNumberKey(param)) {
                return true;
            }
            // 3min set timeout
            this._setTimeout();
            switch (param.alKey) {
                case hx.key.KEY_GUIDE:
                    if (this.dcn.isActive) {
                        this.dcn.clearDcn(param);
                    }
                    this._close();
                    isConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (this.dcn.isActive) {
                        this.dcn.clearDcn(param);
                        //guideEntry.setFocus();
                        isConsumed = true;
                    }
                    else if (viewState === 'idle') {
                        this._close();
                        isConsumed = true;
                    }
                    break;
                case hx.key.KEY_BACK:
                    if (this.dcn.isActive) {
                        this.dcn.clearDcn(param);
                        //guideEntry.setFocus();
                        isConsumed = true;
                    }
                    else if (guideEntry.getCurrentView().name === 'guideNext' && guideEntry.checkNextViewTimeline() && viewState === 'idle') {
                        this._close();
                        isConsumed = true;
                    }
                    break;
                case hx.key.KEY_SEARCH:
                    // clear 3min Timer
                    clearTimeout(this._pgTimerId);
                    /*focusInfo = guideEntry.getCurrentView().focusInfo;
    
                     if (focusInfo && focusInfo.programme) {
                     keyword = focusInfo.programme.name || '';
                     }
                     hx.svc.SMART_SEARCH.setKeyword({keyword
                     });*/
                    this.sendEventToEntity({
                        'alEvent': 'alMessageToPage',
                        // 'target' : 'CPgSmartSearch?type=guide',
                        'target': 'CPgSmartSearch',
                        'me': this
                    });
                    isConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    if (guideEntry.getCurrentView().name === 'guideOnAir' && viewState === 'idle') {
                        if (this.dcn.isActive) {
                            this.dcn.clearDcn();
                            //guideEntry.setFocus();
                            isConsumed = true;
                        }
                        else {
                            var isMedGoing = hx.svc.PLAYBACK.isMpbOnGoing();
                            if (isMedGoing) {
                                hx.svc.BRIDGE.requestStopPlayback();
                            }
                            else {
                                if (guideEntry && !guideEntry.isLiveChannelClicked(true)) {
                                    if (guideEntry.guideOnAir && guideEntry.guideOnAir.changeChannel()) {
                                        this._close();
                                    }
                                }
                                isConsumed = true;
                            }
                        }
                    }
                    else if (guideEntry.getCurrentView().name === 'guideNext' && viewState === 'idle') {
                        if (this.dcn.isActive) {
                            this.dcn.clearDcn();
                            //guideEntry.setFocus();
                            isConsumed = true;
                        }
                    }
                    break;
                case hx.key.KEY_RECORD:
                    if (guideEntry.getCurrentView().name === 'guideOnAir' && viewState === 'idle') {
                        if (!this._guideEntry.isLiveChannelClicked(false)) {
                            channel = guideEntry.getCurrentChannel();
                        }
                        var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                        if (bIsGetPvrHDD === true) {
                            hx.svc.BRIDGE.requestInstanceRecording(channel);
                        }
                        else {
                            this.sendEventToEntity({
                                alEvent: 'alMessageToPage',
                                target: 'CPgNotAvailable?show'
                            });
                        }
                        isConsumed = true;
                    }
                    else if (guideEntry.getCurrentView().name === 'guideNext' && viewState === 'idle') {
                        if (guideEntry.guideNext) {
                            if (guideEntry.guideNext.isLive(guideEntry.getCurrentFocusProgramme())) {
                                if (!guideEntry.isLiveChannelClicked(false)) {
                                    channel = guideEntry.getCurrentChannel();
                                }
                                var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                                if (bIsGetPvrHDD === true) {
                                    hx.svc.BRIDGE.requestInstanceRecording(channel);
                                }
                                else {
                                    this.sendEventToEntity({
                                        alEvent: 'alMessageToPage',
                                        target: 'CPgNotAvailable?show'
                                    });
                                }
                                isConsumed = true;
                            }
                        }
                    }
                    break;
                case hx.key.KEY_STOP:
                    if (guideEntry.getCurrentView().name === 'guideOnAir' && viewState === 'idle') {
                        channel = guideEntry.getCurrentChannel();
                        var currentRecordingList = dsrvRecordings.getCurrentRecordings();
                        if (currentRecordingList && currentRecordingList.length) {
                            var ccid = channel.ccid;
                            var i, ti, recordingItem;
                            for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                                recordingItem = currentRecordingList[i];
                                if (recordingItem.getChannelId() === ccid) {
                                    dsrvRecordings.stopRecord({
                                        'item': recordingItem
                                    });
                                    break;
                                }
                            }
                        }
                        isConsumed = true;
                    }
                    else if (guideEntry.getCurrentView().name === 'guideNext' && viewState === 'idle') {
                        if (guideEntry.guideNext) {
                            if (guideEntry.guideNext.isLive(guideEntry.getCurrentFocusProgramme())) {
                                channel = guideEntry.getCurrentChannel();
                                var currentRecordingList = dsrvRecordings.getCurrentRecordings();
                                if (currentRecordingList && currentRecordingList.length) {
                                    var ccid = channel.ccid;
                                    var i, ti, recordingItem;
                                    for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                                        recordingItem = currentRecordingList[i];
                                        if (recordingItem.getChannelId() === ccid) {
                                            dsrvRecordings.stopRecord({
                                                'item': recordingItem
                                            });
                                            break;
                                        }
                                    }
                                }
                                isConsumed = true;
                            }
                        }
                    }
                    break;
                case hx.key.KEY_OPTION:
                    if (this.dcn.isActive) {
                        isConsumed = true;
                    }
                    break;
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                    if (this.dcn.isActive) {
                        isConsumed = true;
                    }
                    break;
                default:
                    break;
            }
            return (isConsumed || guideEntry._on_alKey(param));
        };
        cPgGuide.prototype._on_alEndOfRepeatKey = function (aParam) {
            return this._guideEntry._on_alEndOfRepeatKey(aParam);
        };
        cPgGuide.prototype._on_ChannelChangeSucceeded = function (param) {
            hx.log('pgGuide', '==== _on_ChannelChangeSucceeded ====');
            this._display_FrontTitle(this._FRONT_TITLE);
        };
        cPgGuide.prototype._on_ChannelChangeError = function (param) {
            hx.log('pgGuide', '==== _on_ChannelChangeError ====');
            this._display_FrontTitle(this._FRONT_TITLE);
        };
        cPgGuide.prototype._on_alUpdate = function (param) {
            this.dcnRefresh();
        };
        cPgGuide.prototype._on_alMessageToPage = function (param) {
        };
        cPgGuide.prototype._on_alDialogRequested = function (param) {
            var _this = this;
            var data = param.alData;
            var hdProgramme = data.hdProgramme;
            var sdProgramme = data.sdProgramme;
            var fnRecorder = data.fnRecorder;
            var dlg = prism.createHDAlternativeButtonDlg(null, [
                {
                    text: hdProgramme.getName(),
                    fn: function () {
                        hx.log("debug", hdProgramme.getName() + " HD Programme recording");
                        fnRecorder(hdProgramme.getPluginObject());
                    }
                },
                {
                    text: sdProgramme.getName(),
                    fn: function () {
                        hx.log("debug", sdProgramme.getName() + " SD Programme recording");
                        fnRecorder(sdProgramme.getPluginObject());
                    }
                }
            ]);
            this.setKeyHookingControl(dlg, true);
            dlg.open({
                onClosed: function () {
                    _this.setKeyHookingControl(dlg);
                }
            });
        };
        // due to dynamic update - Check CCID Change
        cPgGuide.prototype._on_ChannelListUpdate = function () {
            if (this.dcn && this.dcn.isActive) {
                this.dcn.clearDcn();
            }
            //TODO: why guide close?
            //        this._close();
        };
        cPgGuide.prototype.dcnRefresh = function () {
            var dcn = this.dcn, guideEntry = this._guideEntry;
            if (dcn._timerId !== null) {
                clearTimeout(dcn._timerId);
            }
            dcn._timerId = setTimeout(function () {
                var param = dcn.getFocusedChannelInfo();
                if (param.alData && param.alData.ccid) {
                    guideEntry.dcnClicked(param);
                }
                dcn.clearDcn();
            }, 3000);
        };
        cPgGuide.prototype._on_alClicked = function (param) {
            var dcn = this.dcn, guideEntry = this._guideEntry;
            if (param.alData && param.alData.ccid) {
                guideEntry.dcnClicked(param);
            }
            else {
            }
            dcn.clearDcn();
        };
        cPgGuide.prototype._close = function () {
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController',
                me: this
            });
        };
        cPgGuide.prototype._hidePlaybar = function () {
            $('#').css('opacity', 0);
        };
        cPgGuide.prototype._display_FrontTitle = function (title) {
            hx.svc.SETTING_UTIL.setFrontPanelString({
                'text': title
            });
        };
        cPgGuide.prototype._setTimeout = function () {
            var _this = this;
            clearTimeout(this._pgTimerId);
            this._pgTimerId = setTimeout(function () {
                hx.log("debug", "timeout called by TVGUIDE");
                hx.log("debug", "timeout called by TVGUIDE");
                hx.log("debug", "timeout called by TVGUIDE");
                hx.log("debug", "timeout called by TVGUIDE");
                _this._close();
            }, this._3MIN_TIMEOUT);
        };
        cPgGuide.prototype._launch_HelpView = function (targetObject) {
            if (!hx.config.useHelpPage) {
                hx.log('debug', '[startHelp] !hx.config.useHelpPage');
                return;
            }
            var helpViewObj = hx.svc.WEB_STORAGE.getHelp();
            //if (!helpViewObj.tvg) {
            if (!helpViewObj.tvg_update1) {
                this.$wrap = $(document.getElementById('wrap'));
                this.help = hx.al.component('HelpView', {
                    'parent': targetObject,
                    //'_type': 'tvg'
                    '_type': 'tvg_update1'
                });
                this.$wrap[0].appendChild(this.help.$[0]);
                hx.al.compose(this).append(this.help);
                this.help.on();
            }
        };
        return cPgGuide;
    })(__PAGE_KERNEL__);
    return cPgGuide;
});
