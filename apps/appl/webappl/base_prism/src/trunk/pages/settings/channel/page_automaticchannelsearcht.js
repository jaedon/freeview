var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings", "pages/components_based_controls/trd_conflict_dialog"], function (require, exports, __KERNEL_SETTINGS__, __CONTROL_TRD_CONFLICT_DLG__) {
    var cCPgAutomaticSearch = (function (_super) {
        __extends(cCPgAutomaticSearch, _super);
        function cCPgAutomaticSearch() {
            _super.call(this);
            this._progressVariable = 0;
            this._isSearching = 'true';
            this._getChupdateEvt = false;
            this._tvListindex = [];
            this._radioListindex = [];
            this._channelCount = 0;
            this._tempTvCount = 0;
            this._tempTvList = [];
            this._tvCount = 0;
            this._tempRadioCount = 0;
            this._radioCount = 0;
            this._tempRadioList = [];
            this._isChangePageInSetting = false;
            this._isTRDConflictDlgOpened = false;
            this._popup = [];
            /* DATAS IN THIS PAGE */
            this.tvList = [];
            this.radioList = [];
            this.aniConfig = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                dep2: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            'left': '795px'
                        },
                        end: {
                            'opacity': 1,
                            'left': '775px'
                        }
                    },
                    fadeOut: {
                        start: {
                            'opacity': 1,
                            'left': '775px'
                        },
                        end: {
                            'opacity': 0,
                            'left': '795px'
                        }
                    }
                },
                menu: {
                    changeIn: {
                        start: {
                            'opacity': 0,
                            'left': '20px'
                        },
                        end: {
                            'opacity': 1,
                            'left': 0
                        }
                    },
                    changeOut3: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '20px'
                        }
                    }
                }
            };
        }
        cCPgAutomaticSearch.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            _super.prototype.create.call(this);
            this.$colum = $('<article>', { 'class': 'alignC colum2' });
            this.$tv = $('<article>', { 'class': 'tv' });
            this._tvcount = this.string(hx.l('LOC_TV_ID') + ' : 0');
            this._tvlist = this.Search_result(this.tvList);
            this.$radio = $('<article>', { 'class': 'radio' });
            this._radiocount = this.string(hx.l('LOC_RADIO_ID') + ' : 0');
            this._radiolist = this.Search_result(this.radioList);
            this._progress = this.progress3();
            this._allResult = this.allResult();
            this._button = this.button('LOC_STOP_ID');
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_AUTOMATIC_CHANNEL_SEARCH_ID').toUpperCase();
            }
            $('#set').append(this.$colum.append(this.$tv.append(this._tvcount.$.after(this._tvlist.$)).after(this.$radio.append(this._radiocount.$.after(this._radiolist.$)).after(this._progress.$.after(this._allResult.$.after(this._button.$))))));
            hx.al.compose(this).append(this._button);
            /* initialize because page change can be a problem*/
            this._isChangePageInSetting = false;
            this._progressVariable = 0;
            this._tvlist.count = 0;
            this._radiolist.count = 0;
            this._getChupdateEvt = false;
            /* request service */
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, function () {
                _this._on_ChannelListUpdate();
            });
            hx.svc.CHANNEL_SEARCH_T.addEventCb('channelScan', this, function (aParam) {
                _this._on_channelScan(aParam);
            });
            $title.ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            this.$colum.ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    hx.log("info", "button on!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    _this._button.on();
                    _this._button.$.addClass('on');
                    _this._isChangePageInSetting = false;
                },
                noEvent: true
            });
            /* Initialize */
            this._progress.$.find('dt').html('');
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                hx.il.criticalTasking.crChannelSearchTasking = true;
                // DMR uri set null ('bring in client' do not have to work)
                hx.svc.DMR.setMedia(null);
                hx.svc.AV_CTRL.stopCurChannel({
                    stopAV: 1,
                    stopTsr: 1
                });
                hx.svc.WEB_STORAGE.defaultZapInfo();
                if (aPageUrl && aPageUrl.param === 'smartRetune') {
                }
                else {
                    hx.svc.CH_LIST.removeWholeChannelList();
                }
                hx.svc.CHANNEL_SEARCH_T.StartTerChannelScan({
                    isAuto: true
                });
            }
            else {
                this._progressVariable = 100;
                this._progress.$.find('dd').eq(0).html(100 + '%');
                this._progress.$.find('dd').eq(1).find('span').css('width', 100 + '%');
                this._progress.$.find('dt').html(hx.l('LOC_NONE_ID'));
                this._button.$.html(hx.l('LOC_EXIT_ID'));
            }
            var retObj = hx.svc.CH_LIST.getChlist({
                group: 'ALL'
            });
            var _chList = retObj.chList;
            if (_chList) {
                if (_chList.length === 0) {
                    this._isNoChannel = true;
                }
                else {
                    this._isNoChannel = false;
                }
            }
            else {
                this._isNoChannel = true;
            }
        };
        cCPgAutomaticSearch.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.CH_LIST.clearEventCb(this);
            hx.svc.CHANNEL_SEARCH_T.clearEventCb(this);
            this._isSearching = 'true';
            this.$colum.remove();
            this._channelCount = 0;
            this._tempTvCount = 0;
            this._tvCount = 0;
            this._tempRadioCount = 0;
            this._radioCount = 0;
            if (!this._isChangePageInSetting) {
                $('#set').remove();
                if ($('#opt').length > 0) {
                    $('#opt').remove();
                }
            }
            else {
                this._isChangePageInSetting = false;
            }
            this.$colum = undefined;
            this.$tv = undefined;
            this._tvcount = undefined;
            this._tvlist = undefined;
            this.$radio = undefined;
            this._radiocount = undefined;
            this._radiolist = undefined;
            this._progress = undefined;
            this._allResult = undefined;
            this._button = undefined;
            this._tvListindex = [];
            this._radioListindex = [];
            this._tempTvList = [];
            this._tempRadioList = [];
        };
        /*********** Direct SVC Event Start ****************/
        /*
         *  param is structured
         *     retObj.group = group;
         *     retObj.chList = chList;
         */
        cCPgAutomaticSearch.prototype._on_ChListEvt = function (aRetObj) {
            var $chList = this._$channel;
            var o, tvLength, radioLength;
            var TotalCnt;
            var noSearchResultString = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_ID');
            if ((aRetObj.chList === undefined) || (aRetObj.chList.length === 0)) {
                this._status = 'result_2';
                $chList.html('0' + ' (' + hx.l('LOC_TV_ID') + ': ' + '0' + ', ' + hx.l('LOC_RADIO_ID') + ': ' + '0' + ')');
                this._resultSet.html(noSearchResultString);
            }
            else {
                o = hx.svc.CH_LIST.getChannelCount({
                    group: 'TV'
                });
                tvLength = o.chCnt;
                o = hx.svc.CH_LIST.getChannelCount({
                    group: 'RADIO'
                });
                radioLength = o.chCnt;
                TotalCnt = tvLength + radioLength;
                $chList.html(TotalCnt + ' (' + hx.l('LOC_TV_ID') + ': ' + tvLength + ', ' + hx.l('LOC_RADIO_ID') + ': ' + radioLength + ')');
            }
        };
        /*********** Direct SVC Event End ****************/
        cCPgAutomaticSearch.prototype._on_channelScan = function (aParam) {
            var j, k, p, u, showString;
            var useTvLength;
            var useRadioLength;
            var searchedTvListLength = 0;
            var searchedRadioListLength = 0;
            var i, len, lchannelType;
            var showtvListindex = 0;
            var showtvList = [];
            var showradioListindex = 0;
            var showradioList = [];
            var tvListLength;
            var radioListLength;
            var transpec = 0;
            var showtvLength;
            var showradioLength;
            var tvindex = 0;
            var radioindex = 0;
            var tvname = '';
            var radioname = '';
            /* initialize */
            this._tvListindex = [];
            this._radioListindex = [];
            useTvLength = this._tvlist._$items.length;
            useRadioLength = this._radiolist._$items.length;
            this._tempTvList.length = 0;
            this._tempRadioList.length = 0;
            for (j = 0; j < useTvLength; j += 1) {
                this._tempTvList.push(this._tvlist.getItem(j).text());
                this._tvlist.getItem(j).remove();
            }
            for (k = 0; k < useTvLength; k += 1) {
                this._tvlist._$items.pop();
            }
            for (p = 0; p < useRadioLength; p += 1) {
                this._tempRadioList.push(this._radiolist.getItem(p).text());
                this._radiolist.getItem(p).remove();
            }
            for (u = 0; u < useRadioLength; u += 1) {
                this._radiolist._$items.pop();
            }
            /* save index about tvList, radioList from searchedInfos */
            this._radiolist.count = 0;
            len = aParam.owSearchedInfos.length;
            for (i = 0; i < len; i += 1) {
                lchannelType = aParam.owSearchedInfos.getInfo(i, "channelType");
                if (lchannelType === 1) {
                    searchedTvListLength += 1;
                    this._tvListindex.push(i);
                }
                else if (lchannelType === 2) {
                    searchedRadioListLength += 1;
                    this._radioListindex.push(i);
                }
            }
            /* push show item from end to maximum 6*/
            tvListLength = this._tvListindex.length;
            radioListLength = this._radioListindex.length;
            for (i = 0; i < tvListLength; i += 1) {
                showtvListindex = this._tvListindex.pop();
                showtvList.push(showtvListindex);
                if (i === 5) {
                    break;
                }
            }
            for (i = 0; i < radioListLength; i += 1) {
                showradioListindex = this._radioListindex.pop();
                showradioList.push(showradioListindex);
                if (i === 5) {
                    break;
                }
            }
            /* Show searching TP */
            //that._progress.$.find('dt').html(that._allTPList[param.transponderCount - 1]);
            showString = '';
            this._freq = aParam.tpInfo.startFrequency;
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                if (aParam.tpInfo.bandwidth === '7Mhz') {
                    aParam.tpInfo.bandwidth = '7MHz';
                }
                else if (aParam.tpInfo.bandwidth === '8Mhz') {
                    aParam.tpInfo.bandwidth = '8MHz';
                }
                if (aParam.tpInfo.bandwidth === '7MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency.length;
                    for (i = 0; i < length; i += 1) {
                        if (this._freq === hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[i]) {
                            break;
                        }
                    }
                    if (hx.config.settings.channelgroupT.bandwidth7MHz.channel[i]) {
                        aParam.channelNumber = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
                    }
                }
                else if (aParam.tpInfo.bandwidth === '8MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency.length;
                    for (i = 0; i < length; i += 1) {
                        if (this._freq === hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[i]) {
                            break;
                        }
                    }
                    if (hx.config.settings.channelgroupT.bandwidth8MHz.channel[i]) {
                        aParam.channelNumber = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
                    }
                }
                if (aParam.channelNumber) {
                    showString = 'CH' + aParam.channelNumber + ', ' + this._freq + 'kHz, ' + aParam.tpInfo.bandwidth;
                }
                else {
                    showString = this._freq + 'kHz, ' + aParam.tpInfo.bandwidth;
                }
            }
            if (this._freq) {
                this._progress.$.find('dt').html(showString);
            }
            else {
                this._progress.$.find('dt').html('');
            }
            this._tvCount = searchedTvListLength;
            this._radioCount = searchedRadioListLength;
            this._channelCount = aParam.owSearchedInfos.length;
            /* show tvList and radioList in shearchedInfos maximum 6 item */
            showtvLength = showtvList.length;
            showradioLength = showradioList.length;
            for (i = 0; i < showtvLength; i += 1) {
                tvindex = showtvList.pop();
                tvname = aParam.owSearchedInfos.getInfo(tvindex, "name");
                this._tvlist.addItem(tvname);
            }
            for (i = 0; i < showradioLength; i += 1) {
                radioindex = showradioList.pop();
                radioname = aParam.owSearchedInfos.getInfo(radioindex, "name");
                this._radiolist.addItem(radioname);
            }
            if (this._tvlist._$items.length === 0) {
                for (i = 0; i < this._tempTvList.length; i += 1) {
                    this._tvlist.addItem(this._tempTvList[i]);
                }
            }
            if (this._radiolist._$items.length === 0) {
                for (i = 0; i < this._tempRadioList.length; i += 1) {
                    this._radiolist.addItem(this._tempRadioList[i]);
                }
            }
            this._tvcount.$.html(hx.l('LOC_TV_ID') + ' : ' + this._tvCount);
            this._radiocount.$.html(hx.l('LOC_RADIO_ID') + ' : ' + this._radioCount);
            /* Show search progress */
            this._progressVariable = aParam.progress;
            if (aParam.progress < 0) {
                this._progressVariable = 0;
            }
            //progressVariable will be used to check search completion
            this._progress.$.find('dd').eq(0).html(this._progressVariable + '%');
            this._progress.$.find('dd').eq(1).find('span').css('width', this._progressVariable + '%');
            /* Show All searched channel count */
            this._allResult.$.find('em').html(' ' + this._channelCount);
            if (aParam.progress === 100) {
                if (this._channelCount > 0) {
                    this._button.$.html(hx.l('LOC_SAVE_ID'));
                }
                else {
                    this._button.$.html(hx.l('LOC_EXIT_ID'));
                }
            }
        };
        cCPgAutomaticSearch.prototype._checkTRD = function () {
            var _this = this;
            new __CONTROL_TRD_CONFLICT_DLG__(function (aDlg) {
                _this._button.off();
                _this._popup.push(aDlg);
                _this._isTRDConflictDlgOpened = true;
            }, function (aDlg) {
                var index = _this._popup.indexOf(aDlg);
                if (index !== -1) {
                    _this._popup.splice(index, 1);
                }
                _this._button.on();
            }, function () {
                _this._isTRDConflictDlgOpened = false;
                _this._savingProgress = hx.msg('progress', {
                    text: hx.l('LOC_SAVING_CHANNELS_ID'),
                    auto_close: false
                });
                hx.svc.CHANNEL_SEARCH_T.saveChannelSearchResult();
                hx.msg.close('progress', _this._savingProgress);
                _this._changeToLastChannel();
                _this._goExit();
            });
        };
        cCPgAutomaticSearch.prototype._on_ChannelListUpdate = function () {
            // tune to available channel
            if (this._getChupdateEvt === true) {
                return;
            }
            this._getChupdateEvt = true;
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                this._checkTRD();
            }
            else {
                hx.svc.CHANNEL_SEARCH_T.saveChannelSearchResult();
                hx.msg.close('progress', this._savingProgress);
                this._changeToLastChannel();
                this._goExit();
            }
        };
        cCPgAutomaticSearch.prototype._changeToLastChannel = function () {
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        cCPgAutomaticSearch.prototype._goExit = function () {
            var targetPage;
            hx.il.criticalTasking.crChannelSearchTasking = false;
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                targetPage = 'CPgSettings';
            }
            this._isChangePageInSetting = true;
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: targetPage,
                me: this
            });
        };
        cCPgAutomaticSearch.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var popup = this._popup;
            if (popup.length) {
                for (var i = 0; i < popup.length; i++) {
                    bConsumed = popup[i].doKey(aParam.alKey);
                    if (bConsumed) {
                        return bConsumed;
                    }
                }
            }
            switch (aParam.alKey) {
                case hx.key.KEY_ESCAPE:
                    bConsumed = true;
                    if (this._isSearching === 'true') {
                        /*  WHEN STOP BUTTON */
                        if (this._progressVariable !== 100) {
                            this._makeMsgbox();
                            hx.svc.CHANNEL_SEARCH_T.pauseScan();
                            this._isSearching = 'false';
                        }
                        else {
                            this._makeMsgbox2();
                            hx.svc.CHANNEL_SEARCH_T.pauseScan();
                            this._isSearching = 'false';
                        }
                    }
                    break;
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_POWER:
                    hx.svc.CHANNEL_SEARCH_T.stopScan();
                    bConsumed = false;
                    break;
            }
            return bConsumed;
        };
        cCPgAutomaticSearch.prototype._on_alClicked = function (aParam) {
            var data = aParam.alData, targetPage = null, bConsumed = false;
            if (data.me === this._button) {
                bConsumed = true;
                /*STOP & SAVE BUTTON */
                if (this._progressVariable !== 100) {
                    this._makeMsgbox();
                    hx.svc.CHANNEL_SEARCH_T.pauseScan();
                    this._isSearching = 'false';
                }
                else {
                    if (this._channelCount > 0) {
                        if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                            if (this._isTRDConflictDlgOpened) {
                                this._checkTRD();
                                return bConsumed;
                            }
                        }
                        else {
                            this._savingProgress = hx.msg('progress', {
                                text: hx.l('LOC_SAVING_CHANNELS_ID'),
                                auto_close: false
                            });
                        }
                        hx.svc.CHANNEL_SEARCH_T.loadChannelSearchResult();
                    }
                    else {
                        hx.svc.CHANNEL_SEARCH_T.stopScan();
                        this._changeToLastChannel();
                        this._goExit();
                    }
                }
            }
            return bConsumed;
        };
        cCPgAutomaticSearch.prototype._on_alExit = function (aParam) {
            var data = aParam.alData, bConsumed = false;
            if (data.me === this._button) {
                bConsumed = true;
                /*  WHEN STOP BUTTON */
                if (this._progressVariable !== 100) {
                    var noWork;
                }
                else {
                    var noWork2;
                }
            }
            return bConsumed;
        };
        cCPgAutomaticSearch.prototype._makeMsgbox = function () {
            var that = this;
            var yesBtn = hx.l('LOC_YES_ID'), noBtn = hx.l('LOC_NO_ID');
            var msgbox = hx.msg('confirm', {
                text: hx.l('LOC_STOP_THE_CHANNEL_SEARCH_ID'),
                auto_close: false,
                btn_title_arr: [yesBtn, noBtn],
                dia_class: 'osd',
                callback_fn: function (p) {
                    if (p === yesBtn) {
                        hx.msg.close('confirm', msgbox);
                        that._makeMsgbox2();
                    }
                    else if (p === noBtn) {
                        hx.msg.close('confirm', msgbox);
                        that._button.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        hx.svc.CHANNEL_SEARCH_T.resumeScan();
                        that._isSearching = 'true';
                    }
                    else if (p === 'key_esc' || p === 'key_back') {
                        hx.msg.close('confirm', msgbox);
                        that._button.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        hx.svc.CHANNEL_SEARCH_T.resumeScan();
                        that._isSearching = 'true';
                    }
                }
            });
        };
        cCPgAutomaticSearch.prototype._makeMsgbox2 = function () {
            var that = this;
            var yesBtn = hx.l('LOC_YES_ID'), noBtn = hx.l('LOC_NO_ID');
            var msgbox2 = hx.msg('confirm', {
                text: hx.l('LOC_SAVE_CHANNEL_SEARCH_ID'),
                auto_close: false,
                btn_title_arr: [yesBtn, noBtn],
                dia_class: 'osd',
                callback_fn: function (p) {
                    if (p === yesBtn && that._channelCount > 0) {
                        hx.msg.close('confirm', msgbox2);
                        if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                            if (that._isTRDConflictDlgOpened) {
                                that._checkTRD();
                                return;
                            }
                        }
                        else {
                            that._savingProgress = hx.msg('progress', {
                                text: hx.l('LOC_SAVING_CHANNELS_ID'),
                                auto_close: false
                            });
                        }
                        hx.svc.CHANNEL_SEARCH_T.loadChannelSearchResult();
                    }
                    else if (p === noBtn || (p === yesBtn && that._channelCount <= 0)) {
                        hx.msg.close('confirm', msgbox2);
                        hx.svc.CHANNEL_SEARCH_T.stopScan();
                        that._changeToLastChannel();
                        that._goExit();
                    }
                    else if (p === 'key_esc' || p === 'key_back') {
                        hx.msg.close('confirm', msgbox2);
                        that._button.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        hx.svc.CHANNEL_SEARCH_T.resumeScan();
                        that._isSearching = 'true';
                    }
                }
            });
        };
        /* COMPONENTS IN THIS PAGE */
        cCPgAutomaticSearch.prototype.string = function (aParam) {
            return hx.al.component('text', {
                tag: 'h2',
                text: aParam,
                attr: {
                    'data-langID': false,
                    'class': ''
                }
            });
        };
        cCPgAutomaticSearch.prototype.button = function (aParam) {
            return (new hx.al.component('button', {
                attr: {
                    'class': 'button',
                    'data-langID': aParam
                },
                usePointerKey: true
            }));
        };
        cCPgAutomaticSearch.prototype.Search_result = function (aParam) {
            var searchresult = hx.al.component('Search_result', {
                itemAttr: {},
                attr: {
                    'style': 'height:208px'
                },
                data: aParam,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(this.data[i]);
                }
            });
            return searchresult;
        };
        cCPgAutomaticSearch.prototype.progress3 = function () {
            return hx.al.component('searchProgress', {
                tag: 'dl',
                data: [' ', '0%', 'width:0%'],
                attr: {
                    'class': 'progress type3'
                }
            });
        };
        cCPgAutomaticSearch.prototype.allResult = function () {
            return hx.al.component('txt2', {
                tag: 'p',
                attr: {
                    'class': 'srcResult'
                },
                data: [hx.l('LOC_CHANNELS_FOUND_ID'), '0']
            });
        };
        return cCPgAutomaticSearch;
    })(__KERNEL_SETTINGS__);
    return cCPgAutomaticSearch;
});
