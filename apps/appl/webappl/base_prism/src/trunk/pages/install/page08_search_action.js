var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "pages/components_based_controls/trd_conflict_dialog"], function (require, exports, __PAGE_KERNEL__, __CONTROL_TRD_CONFLICT_DLG__) {
    //declare var prism;
    var cCPgInstall08 = (function (_super) {
        __extends(cCPgInstall08, _super);
        function cCPgInstall08() {
            _super.call(this);
            this._timeset = 0;
            this.popup = null;
            this._allTPList = [];
            this._tvCount = 0;
            this._tempTvCount = 0;
            this._radioCount = 0;
            this._tempRadioCount = 0;
            this._channelCount = 0;
            this._lockingDataList = [];
            this._isTRDConflictDlgOpened = false;
        }
        cCPgInstall08.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            hx.logadd('CPgInstall08');
            var i;
            var nowStep;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(false);
            if (hx.config.antConTpList === 'searched') {
                this._lockingDataList.length = 0;
            }
            else if (hx.config.antConTpList === 'hard') {
                this._lockingDataList = hx.config.wzd.lockingDataList;
            }
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                this.dsrvChannelSearch = hx.svc.CHANNEL_SEARCH_T;
            }
            else {
                this.dsrvChannelSearch = hx.svc.CHANNEL_SEARCH_S;
            }
            $.extend(this, {
                _containerEl: document.querySelector('#wiz'),
                _$article: $('<article>', {
                    'class': 'cont'
                }),
                _chSearchEl: document.createElement('article'),
                _searchListEl: document.createElement('ul'),
                _searchListLiEl: document.createElement('li'),
                _promotionWrapEl: document.createElement('figure'),
                _promotionImgEl: document.createElement('img'),
                _promotionTextEl: document.createElement('p'),
                _barWrapEl: document.createElement('div'),
                _progressTypeEl: document.createElement('dl'),
                _progressTextEl: document.createElement('dt'),
                _progressPercentEl: document.createElement('dd'),
                _graphEl: document.createElement('dd'),
                _progressBarEl: document.createElement('span'),
                _searchChannelEl: document.createElement('p'),
                _channelTextEl: document.createElement('em'),
                _btnWrapEl: document.createElement('div'),
                _$hArea: $('<h1>', {
                    'class': 'hArea'
                }),
                _$title: $('<span>', {
                    'class': 'tit',
                    'html': hx.l('LOC_CHANNELSEARCH_ID')
                }),
                _stepEl: document.createElement('span'),
                _stop_btn: this.__make_btn('LOC_STOP_ID'),
                _save_btn: this.__make_btn('LOC_SAVE_ID'),
                _exit_btn: this.__make_btn('LOC_EXIT_ID')
            });
            //////////////////set Element attribute
            if (ENV.op === ENV.listOP.SES) {
                nowStep = 'step2';
            }
            else if (ENV.op === ENV.listOP.MIDDLE_EAST || ENV.listModel.HMS1000S === ENV.model) {
                nowStep = 'step4';
            }
            else {
                nowStep = hx.al.CPgInstall08.items.step ? hx.al.CPgInstall08.items.step : 'step3';
            }
            this._stepEl.classList.add(nowStep);
            this._$article.css('left', '50px');
            this._chSearchEl.classList.add('chSearch');
            this._searchListEl.classList.add('searchList');
            this._promotionImgEl.setAttribute('src', hx.al.CPgInstall08.items.images[0]);
            this._promotionImgEl.classList.add('searchImg');
            this._promotionTextEl.innerHTML = '<em>' + "The all new TV Guide." + '</em>' + hx.l(hx.al.CPgInstall08.items.itemStr[0]);
            this._btnWrapEl.classList.add('btnBo');
            this._barWrapEl.classList.add('bar_wrap');
            this._progressTypeEl.classList.add('progress');
            this._progressTypeEl.classList.add('type3');
            this._graphEl.classList.add('graph');
            this._searchChannelEl.innerHTML = hx.l('LOC_CHANNELS_FOUND_ID') + ' ';
            this._searchChannelEl.classList.add('sechan');
            //set promotion~!!!!
            this._set_promotion_section('<em>' + "The all new TV Guide." + '</em>' + "See more information about TV programmes and what's on Catch-up with the Freeview Play guide.", hx.al.CPgInstall08.items.images[0]);
            //set progress~!!!
            this._set_progress_status('', '0');
            //set channel count~!!!
            this._set_channel_count('0');
            /////////////////append To DOM
            if (!this._containerEl) {
                this._containerEl = document.createElement('section');
                this._containerEl.id = 'wiz';
                document.querySelector('#wrap').appendChild(this._containerEl);
            }
            this._$hArea[0].appendChild(this._$title[0]);
            this._$hArea[0].appendChild(this._stepEl);
            this._graphEl.appendChild(this._progressBarEl);
            this._progressTypeEl.appendChild(this._progressTextEl);
            this._progressTypeEl.appendChild(this._progressPercentEl);
            this._progressTypeEl.appendChild(this._graphEl);
            this._searchChannelEl.appendChild(this._channelTextEl);
            this._barWrapEl.appendChild(this._progressTypeEl);
            this._barWrapEl.appendChild(this._searchChannelEl);
            this._promotionWrapEl.appendChild(this._promotionImgEl);
            this._searchListLiEl.appendChild(this._promotionWrapEl);
            this._searchListLiEl.appendChild(this._promotionTextEl);
            this._searchListEl.appendChild(this._searchListLiEl);
            this._chSearchEl.appendChild(this._searchListEl);
            this._btnWrapEl.appendChild(this._stop_btn.$[0]);
            this._$article[0].appendChild(this._barWrapEl);
            this._$article[0].appendChild(this._chSearchEl);
            this._$article[0].appendChild(this._btnWrapEl);
            this._containerEl.style.display = 'none';
            this._containerEl.appendChild(this._$hArea[0]);
            this._containerEl.appendChild(this._$article[0]);
            this._containerEl.style.display = 'block';
            /////////////Do Initial Script
            hx.al.compose(this).append(this._stop_btn).append(this._save_btn).append(this._exit_btn);
            this._stop_btn.on();
            this._stop_btn.$.addClass('on');
            this._isGoWizardPage = false;
            hx.il.ani.action({
                targets: this._$hArea,
                cls: 'aniFadeIn'
            });
            hx.il.ani.action({
                targets: [this._$article],
                cls: 'aniFadeIn',
                style: 'left:0'
            });
            this._getChupdateEvt = false;
            //promotion change~~!!!
            this._set_promotion_change([{
                '_html': '<em>' + "The all new TV Guide." + '</em>' + hx.al.CPgInstall08.items.itemStr[0],
                '_src': hx.al.CPgInstall08.items.images[0]
            }, {
                '_html': '<em>' + "Start a Smart Search with one button." + '</em>' + hx.al.CPgInstall08.items.itemStr[1],
                '_src': hx.al.CPgInstall08.items.images[1]
            }, {
                '_html': '<em>' + "Explore a new world of entertainment, for free." + '</em>' + hx.al.CPgInstall08.items.itemStr[2],
                '_src': hx.al.CPgInstall08.items.images[2]
            }, {
                '_html': '<em>' + "Stream Live TV directly to your smart devices." + '</em>' + hx.al.CPgInstall08.items.itemStr[3],
                '_src': hx.al.CPgInstall08.items.images[3]
            }, {
                '_html': '<em>' + "Record 4 programmes at once." + '</em>' + hx.al.CPgInstall08.items.itemStr[4],
                '_src': hx.al.CPgInstall08.items.images[4]
            }]);
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, function () {
                _this._on_ChannelListUpdate();
            });
            this.dsrvChannelSearch.addEventCb('channelScan', this, function (aParam) {
                _this._on_channelScan(aParam);
            });
            if (hx.config.useClockRecovery === true) {
                hx.svc.CLOCK_RECOVERY.addEventListener({
                    'event': 'ClockRecoveryEvent',
                    'objTarget': this
                });
            }
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                // DMR uri set null ('bring in client' do not have to work)
                hx.svc.DMR.setMedia(null);
                hx.svc.AV_CTRL.stopCurChannel({
                    stopAV: 1,
                    stopTsr: 1
                });
                this.dsrvChannelSearch.StartTerChannelScan({
                    isAuto: true
                });
            }
            else {
                this.dsrvChannelSearch.addEventCb('antennaConnection', this, function (aParam) {
                    _this._on_antennaConnection(aParam);
                });
                var tpSelectorObj = this.dsrvChannelSearch.getTransponderSelectorAll();
                /* Initialize */
                this._allTPList = tpSelectorObj.transponderSelector;
                //that._$progress_text.text(that._allTPList[0]);
                if (this._allTPList.length !== 0) {
                    this.dsrvChannelSearch.StartChannelScan();
                }
                else {
                    this._progressVariable = 100;
                    this._progressPercentEl.innerHTML = this._progressVariable + '%';
                    this._progressBarEl.style.width = this._progressVariable + '%';
                    this._progressTextEl.innerHTML = hx.l('LOC_NONE_ID');
                    this._btn_status(hx.l('LOC_EXIT_UPPER_ID'));
                }
            }
        };
        cCPgInstall08.prototype.destroy = function () {
            hx.svc.CH_LIST.clearEventCb(this);
            this.dsrvChannelSearch.clearEventCb(this);
            if (hx.config.useClockRecovery === true) {
                hx.svc.CLOCK_RECOVERY.removeEventListener(this);
            }
            this._tvCount = 0;
            this._tempTvCount = 0;
            this._radioCount = 0;
            this._tempRadioCount = 0;
            this._channelCount = 0;
            this._lockingDataList.length = 0;
            clearInterval(this._timeset);
            if (this._containerEl) {
                if (!this._isGoWizardPage) {
                    this.dsrvChannelSearch.stopScan();
                    if (this._savingProgress) {
                        hx.msg.close('progress', this._savingProgress);
                        delete this._savingProgress;
                    }
                    if (this.progress) {
                        hx.msg.close('progress', this.progress);
                    }
                    this._containerEl.parentElement.removeChild(this._containerEl);
                }
                else {
                    this._containerEl.style.display = 'none';
                    this._containerEl.removeChild(this._$hArea[0]);
                    this._containerEl.removeChild(this._$article[0]);
                    this._containerEl.style.display = 'block';
                }
                this._containerEl = undefined;
            }
            if (this._savingProgress !== undefined) {
                hx.msg.close('progress', this._savingProgress);
                delete this._savingProgress;
            }
            this._$article = undefined;
            this._chSearchEl = undefined;
            this._searchListEl = undefined;
            this._searchListLiEl = undefined;
            this._promotionWrapEl = undefined;
            this._promotionImgEl = undefined;
            this._promotionTextEl = undefined;
            this._barWrapEl = undefined;
            this._progressTypeEl = undefined;
            this._progressTextEl = undefined;
            this._progressPercentEl = undefined;
            this._graphEl = undefined;
            this._progressBarEl = undefined;
            this._searchChannelEl = undefined;
            this._channelTextEl = undefined;
            this._btnWrapEl = undefined;
            this._$hArea = undefined;
            this._$title = undefined;
            this._stepEl = undefined;
            this._stop_btn = undefined;
            this._save_btn = undefined;
            this._exit_btn = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cCPgInstall08.prototype.__make_btn = function (aId) {
            return hx.al.component('button', {
                attr: {
                    'class': 'button',
                    'data-langID': aId
                },
                usePointerKey: true
            });
        };
        cCPgInstall08.prototype._set_promotion_section = function (aHtml, aImg) {
            this._promotionImgEl.setAttribute('src', aImg);
            this._promotionTextEl.innerHTML = aHtml;
        };
        cCPgInstall08.prototype._set_progress_status = function (aText, aPercent) {
            var tmp;
            tmp = aPercent;
            if (tmp < 0) {
                aPercent = '0';
            }
            else if (tmp > 100) {
                aPercent = '100';
            }
            this._progressTextEl.innerHTML = aText;
            this._progressPercentEl.innerHTML = aPercent + '%';
            this._progressBarEl.style.width = aPercent + '%';
        };
        cCPgInstall08.prototype._set_channel_count = function (aCnt) {
            this._channelTextEl.innerHTML = aCnt;
        };
        cCPgInstall08.prototype._set_promotion_change = function (aArr) {
            var _this = this;
            var i = 1;
            clearInterval(this._timeset);
            this._timeset = setInterval(function () {
                if (i === aArr.length) {
                    i = 0;
                }
                var tmp = aArr[i];
                _this._set_promotion_section(tmp._html, tmp._src);
                i += 1;
            }, 10000);
        };
        cCPgInstall08.prototype._on_channelScan = function (aParam) {
            var antennaId = aParam.tpInfo.antennaId, frequency = aParam.tpInfo.startFrequency;
            var symbolRate = aParam.tpInfo.symbolRate, polarisation = aParam.tpInfo.polarisation;
            var codeRate = aParam.tpInfo.codeRate, modulationModes = aParam.tpInfo.modulationModes;
            var satName;
            var searchedTvListLength = 0, searchedRadioListLength = 0, transpec = 0, i = 0;
            var showString = '', pola = '', showCodeRate = '';
            /* Show search progress */
            this._progressVariable = aParam.progress;
            if (aParam.progress < 0) {
                this._progressVariable = 0;
            }
            //progressVariable will be used to check search completion
            this._progressPercentEl.innerHTML = this._progressVariable + '%';
            this._progressBarEl.style.width = this._progressVariable + '%';
            for (i = 0; i < aParam.owSearchedInfos.length; i += 1) {
                if (aParam.owSearchedInfos.getInfo(i, 'channelType') === 1) {
                    searchedTvListLength += 1;
                }
                else if (aParam.owSearchedInfos.getInfo(i, 'channelType') === 2) {
                    searchedRadioListLength += 1;
                }
            }
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                if (hx.config.antConTpList === 'searched') {
                    hx.logadd('ChannelScanEvent');
                    hx.log('ChannelScanEvent', 'tpInfo.AntId : ' + antennaId);
                    hx.log('ChannelScanEvent', 'tpInfo.startFrequency : ' + frequency);
                    hx.log('ChannelScanEvent', 'tpInfo.symbolRate : ' + symbolRate);
                    hx.log('ChannelScanEvent', 'tpInfo.polarisation : ' + polarisation);
                    hx.log('ChannelScanEvent', 'tpInfo.codeRate : ' + codeRate);
                    hx.log('ChannelScanEvent', 'tpInfo.modulationModes : ' + modulationModes);
                    //ms modifying
                    if (aParam.signalStrength >= 10) {
                        if (codeRate === 'AUTO') {
                            codeRate = 'auto';
                        }
                        if (modulationModes !== 0) {
                            transpec = 1;
                        }
                        this._lockingDataList.push({
                            antId: antennaId,
                            tpData: [parseInt(frequency, 10) * 1000, parseInt(symbolRate, 10) * 1000, polarisation, codeRate, modulationModes, transpec, 0]
                        });
                    }
                }
                else if (hx.config.antConTpList === 'hard') {
                    this._lockingDataList[0].antId = antennaId;
                    this._lockingDataList[1].antId = antennaId;
                    this._lockingDataList[2].antId = antennaId;
                }
            }
            this._tvCount = searchedTvListLength;
            this._radioCount = searchedRadioListLength;
            this._channelCount = this._tvCount + this._radioCount;
            /* Show All searched channel count */
            this._set_channel_count(this._channelCount);
            switch (polarisation) {
                case 0:
                    pola = 'A';
                    break;
                case 1:
                    pola = 'H';
                    break;
                case 2:
                    pola = 'V';
                    break;
            }
            if (aParam.tpInfo.codeRate === 'AUTO') {
                showCodeRate = 'A';
            }
            else {
                showCodeRate = aParam.tpInfo.codeRate;
            }
            this._freq = frequency;
            this._pola = pola;
            this._symbol = symbolRate;
            this._codeRate = showCodeRate;
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                satName = this.dsrvChannelSearch.getSatelliteNameById({
                    antId: antennaId
                });
            }
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
            else {
                showString = satName + this._freq + ', ' + this._pola + ', ' + this._symbol + ', ' + this._codeRate;
            }
            if (this._freq) {
                this._progressTextEl.innerHTML = showString;
            }
            else {
                this._progressTextEl.innerHTML = '';
            }
            if (aParam.progress === 100) {
                if (this._channelCount > 0) {
                    this._btn_status(hx.l('LOC_SAVE_ID'));
                }
                else {
                    this._btn_status(hx.l('LOC_EXIT_UPPER_ID'));
                }
            }
        };
        cCPgInstall08.prototype._on_ChannelListUpdate = function () {
            var that = this;
            function setSearchType(type) {
                hx.svc.WEB_STORAGE.setLastSearchType({
                    lastSearchType: type
                });
                hx.log('warning', '');
                hx.log('warning', '');
                hx.log('warning', '######################## Set Antenna Type Start #######################');
                hx.log('warning', 'Set Antenna Type in install wizard: ' + type);
                hx.log('warning', '######################## Set Antenna Type Result End #######################');
                hx.log('warning', '');
                hx.log('warning', '');
            }
            // tune to available channel
            if (that._getChupdateEvt === true) {
                return;
            }
            that._getChupdateEvt = true;
            hx.msg.close('progress', this._savingProgress);
            delete this._savingProgress;
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                this.dsrvChannelSearch.saveChannelSearchResult();
                hx.msg.close('progress', that._savingProgress);
                if (that._lockingDataList.length > 0) {
                    that.progress = hx.msg('progress', {
                        text: hx.l('LOC_CHECKING_ANTENNATYPE_ID'),
                        auto_close: false
                    });
                    this.dsrvChannelSearch.startAntennaConnection({
                        lockingDataList: that._lockingDataList
                    });
                }
                else {
                    var searchType = this.dsrvChannelSearch.getSearchTypeFromAnt();
                    if ((searchType === 0) || (searchType === null)) {
                        setSearchType('lnb');
                    }
                    else if (searchType === 1) {
                        setSearchType('diseqc');
                    }
                    else if (searchType === 2) {
                        setSearchType('scd');
                    }
                    else if (searchType === 3) {
                        setSearchType('motor');
                    }
                    that._isGoWizardPage = true;
                    hx.al.decompose(that);
                    that._fadeOutAni(hx.al.CPgInstall08.items.nextPage, false);
                }
            }
            else {
                hx.msg.close('progress', that._savingProgress);
                this._checkTRD();
            }
        };
        cCPgInstall08.prototype._checkTRD = function () {
            var _this = this;
            new __CONTROL_TRD_CONFLICT_DLG__(function (aDlg) {
                _this._stop_btn.off();
                _this._save_btn.off();
                _this._exit_btn.off();
                hx.al.CPgInstall08.popup = aDlg;
                _this._isTRDConflictDlgOpened = true;
            }, function (aDlg) {
                hx.al.CPgInstall08.popup = null;
                _this._stop_btn.on();
                _this._save_btn.on();
                _this._exit_btn.on();
            }, function () {
                _this._isGoWizardPage = true;
                _this._isTRDConflictDlgOpened = false;
                _this.dsrvChannelSearch.saveChannelSearchResult();
                hx.al.decompose(_this);
                _this._fadeOutAni(hx.al.CPgInstall08.items.nextPage, false);
            });
        };
        cCPgInstall08.prototype._on_antennaConnection = function (param) {
            var that = this, searchType;
            function setSearchType(type) {
                hx.svc.WEB_STORAGE.setLastSearchType({
                    lastSearchType: type
                });
                hx.log('warning', '');
                hx.log('warning', '');
                hx.log('warning', '######################## Set Antenna Type Start #######################');
                hx.log('warning', 'Set Antenna Type in install wizard: ' + type);
                hx.log('warning', '######################## Set Antenna Type Result End #######################');
                hx.log('warning', '');
                hx.log('warning', '');
            }
            if (param.progress === 100) {
                hx.msg.close('progress', that.progress);
                hx.log('warning', '');
                hx.log('warning', '');
                hx.log('warning', '######################## Antenna Connection Result Start #######################');
                hx.log('warning', 'Antenna Connection Type : ' + param.connectionType);
                hx.log('warning', '######################## Antenna Connection Result End #######################');
                hx.log('warning', '');
                hx.log('warning', '');
                this.dsrvChannelSearch.antennaConnectionComplete({
                    connectionType: param.connectionType
                });
                searchType = this.dsrvChannelSearch.getSearchTypeFromAnt();
                if ((searchType === 0) || (searchType === null)) {
                    setSearchType('lnb');
                }
                else if (searchType === 1) {
                    setSearchType('diseqc');
                }
                else if (searchType === 2) {
                    setSearchType('scd');
                }
                else if (searchType === 3) {
                    setSearchType('motor');
                }
                if (hx.config.useClockRecovery === true) {
                    that._actonTimeRecovery();
                }
                else {
                    that._isGoWizardPage = true;
                    hx.al.decompose(that);
                    that._fadeOutAni(hx.al.CPgInstall08.items.nextPage, false);
                }
            }
        };
        cCPgInstall08.prototype._on_alClicked = function (aParam) {
            var bConsumed = false;
            var _key = aParam.alKey;
            if (aParam.alData.me === this._stop_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this.dsrvChannelSearch.pauseScan();
                if (ENV.listModel.HMS1000T === ENV.model) {
                    this._makeMsgbox();
                }
                else {
                    this.__channel_search_stop();
                }
            }
            else if (aParam.alData.me === this._save_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._save_channel(hx.l('LOC_SAVE_ID'));
            }
            else if (aParam.alData.me === this._exit_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                if (this._isTRDConflictDlgOpened) {
                    this._checkTRD();
                }
                else {
                    this._exit_channel();
                }
            }
            return bConsumed;
        };
        cCPgInstall08.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var dlg = hx.al.CPgInstall08.popup;
            if (dlg) {
                dlg.doKey(aParam.alKey);
                bConsumed = true;
            }
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        cCPgInstall08.prototype.__channel_search_stop = function () {
            var that = this;
            var tmp = hx.msg('confirm', {
                text: hx.l('LOC_STOP_THE_CHANNEL_SEARCH_ID'),
                auto_close: false,
                //timeout : 5000,
                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                dia_class: 'osd',
                callback_fn: fn
            });
            function fn(_status) {
                if (_status === 'key_back' || _status === 'key_power' || _status === 'key_esc' || _status === hx.l('LOC_NO_ID')) {
                    that._tempTvCount = that._tvCount;
                    that._tempRadioCount = that._radioCount;
                    that.dsrvChannelSearch.resumeScan();
                    that._btn_status(hx.l('LOC_STOP_ID'));
                }
                else if (_status === hx.l('LOC_YES_ID')) {
                    if (that._channelCount > 0) {
                        that._save_channel(hx.l('LOC_STOP_ID'));
                    }
                    else {
                        that._exit_channel();
                    }
                }
            }
        };
        cCPgInstall08.prototype._btn_status = function (aStatus) {
            var i, len;
            for (i = 0, len = this._btnWrapEl.children.length; i < len; i += 1) {
                this._btnWrapEl.removeChild(this._btnWrapEl.children[i]);
            }
            if (aStatus === hx.l('LOC_STOP_ID')) {
                this._btnWrapEl.appendChild(this._stop_btn.$[0]);
                this._stop_btn.on();
                this._stop_btn.$.addClass('on');
            }
            else if (aStatus === hx.l('LOC_SAVE_ID')) {
                this._btnWrapEl.appendChild(this._save_btn.$[0]);
                this._save_btn.on();
                this._save_btn.$.addClass('on');
            }
            else if (aStatus === hx.l('LOC_EXIT_UPPER_ID')) {
                this._btnWrapEl.appendChild(this._exit_btn.$[0]);
                this._exit_btn.on();
                this._exit_btn.$.addClass('on');
            }
        };
        cCPgInstall08.prototype._save_channel = function (aStatus) {
            if (aStatus === hx.l('LOC_STOP_ID')) {
                this._stop_btn.$.attr('class', 'button on');
            }
            else if (aStatus === hx.l('LOC_SAVE_ID')) {
                this._save_btn.$.attr('class', 'button on');
            }
            this._savingProgress = hx.msg('progress', {
                text: hx.l('LOC_SAVING_CHANNELS_ID'),
                auto_close: false
            });
            if (hx.config.useDefaultChannels === 1) {
                this.dsrvChannelSearch.loadChannelSearchResultWithDefCh();
            }
            else {
                this.dsrvChannelSearch.loadChannelSearchResult();
            }
        };
        cCPgInstall08.prototype._exit_channel = function () {
            if (!this._exit_btn) {
                return;
            }
            this._exit_btn.$.attr('class', 'button on');
            this.dsrvChannelSearch.stopScan();
            //go next page~!!!
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall08.items.nextPage, false);
        };
        cCPgInstall08.prototype._fadeOutAni = function (aTargetPage, aIsGoPrev) {
            var _this = this;
            var left;
            if (aIsGoPrev) {
                left = '20px';
            }
            else {
                left = '-20px';
            }
            hx.il.ani.action({
                targets: this._$article,
                cls: 'aniFadeOut',
                style: 'left:' + left
            });
            hx.il.ani.action({
                targets: this._$title,
                cls: 'aniFadeOut',
                cb: function () {
                    _this.sendEventToEntity({
                        'alEvent': 'alChangeInstallPage',
                        'target': aTargetPage,
                        'me': _this
                    });
                }
            });
        };
        cCPgInstall08.prototype._actonTimeRecovery = function () {
            hx.log('CPgInstall08', '_actonTimeRecovery()');
            this.progress = hx.msg('progress', {
                text: hx.l('LOC_INSTALL_SW_UPDATE_SYSTEM_TIME_ITEM_ID'),
                auto_close: false
            });
            hx.svc.CLOCK_RECOVERY.startClockRecovery();
        };
        cCPgInstall08.prototype._on_ClockRecoveryEvent = function (aParam) {
            hx.log('CPgInstall08', '_on_ClockRecoveryEvent()');
            if (this.progress) {
                hx.msg.close('progress', this.progress);
                this.progress = undefined;
            }
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall08.items.nextPage, false);
        };
        cCPgInstall08.prototype._makeMsgbox = function () {
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
                        that._stop_btn.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        that.dsrvChannelSearch.resumeScan();
                    }
                    else if (p === 'key_esc' || p === 'key_back') {
                        hx.msg.close('confirm', msgbox);
                        that._stop_btn.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        that.dsrvChannelSearch.resumeScan();
                    }
                }
            });
        };
        cCPgInstall08.prototype._makeMsgbox2 = function () {
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
                        that._savingProgress = hx.msg('progress', {
                            text: hx.l('LOC_SAVING_CHANNELS_ID'),
                            auto_close: false
                        });
                        that.dsrvChannelSearch.loadChannelSearchResult();
                    }
                    else if (p === noBtn || (p === yesBtn && that._channelCount <= 0)) {
                        hx.msg.close('confirm', msgbox2);
                        that.dsrvChannelSearch.stopScan();
                        that._exit_channel();
                    }
                    else if (p === 'key_esc' || p === 'key_back') {
                        hx.msg.close('confirm', msgbox2);
                        that._stop_btn.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        that.dsrvChannelSearch.resumeScan();
                    }
                }
            });
        };
        return cCPgInstall08;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall08 = cCPgInstall08;
    /********************      Static Variables     ********************/
    if (hx.config.wzd.page08) {
        hx.al.CPgInstall08.items = hx.config.wzd.page08;
    }
    else {
        hx.al.CPgInstall08.items = {
            //itemStr : ['LOC_INSTALL_STR1_ID', 'LOC_INSTALL_STR2_ID', 'LOC_CATCH_UP_FREEVIEW_ID', 'LOC_INSTALL_STR5_ID'],
            itemStr: ["See more information about TV programmes and what's on Catch-up with the Freeview Play guide.", "Find your favourite programmes quicker from Live TV, Catch-up, Recordings and On-Demand by presssing the search button.", "Catch up on your favourite programmes courtesy of Freeview Play.<br>Find shows within the Catch-up and On-Demand section.", "Watch live, playback recordings, set recordings and more from our Live TV app. Download the Humax Live TV app from the App Store or Google Play Store.", "With three tuners, you can record four channels simultaneously while watching a fifth channel."],
            images: ['./images/614_WIZ_Promotion01.png', './images/614_WIZ_Promotion03.png', './images/614_WIZ_Promotion04.png', './images/614_WIZ_Promotion02.png', './images/614_WIZ_Promotion05.png'],
            nextPage: 'CPgInstall09',
            prevPage: '',
            step: 'step3'
        };
    }
    return cCPgInstall08;
});
