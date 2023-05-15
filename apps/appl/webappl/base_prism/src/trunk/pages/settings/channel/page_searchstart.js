var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings", "pages/components_based_controls/trd_conflict_dialog"], function (require, exports, __KERNEL_SETTINGS__, __CONTROL_TRD_CONFLICT_DLG__) {
    var cCPgSearchStart = (function (_super) {
        __extends(cCPgSearchStart, _super);
        function cCPgSearchStart() {
            _super.call(this);
            this._progressVariable = 0;
            this._allTPList = {};
            this._tsIndex = '0';
            this._searchType = null;
            this._transponderCount = 0;
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
            this._lockingDataList = [];
            this._isLoadPreferList = false;
            this._isChangePageInSetting = false;
            this._popup = [];
            this._isTRDConflictDlgOpened = false;
        }
        cCPgSearchStart.prototype.create = function () {
            var _this = this;
            _super.prototype.create.call(this);
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                this.dsrvChannelSearch = hx.svc.CHANNEL_SEARCH_T;
            }
            else {
                this.dsrvChannelSearch = hx.svc.CHANNEL_SEARCH_S;
            }
            $.extend(this, {
                $colum: $('<article>', {
                    'class': 'alignC colum2'
                }),
                $tv: $('<article>', {
                    'class': 'tv'
                }),
                _tvcount: hx.al.CPgSearchStart.string(hx.l('LOC_TV_ID') + ' : 0'),
                _tvlist: hx.al.CPgSearchStart.Search_result(hx.al.CPgSearchStart.tvList),
                $radio: $('<article>', {
                    'class': 'radio'
                }),
                _radiocount: hx.al.CPgSearchStart.string(hx.l('LOC_RADIO_ID') + ' : 0'),
                _radiolist: hx.al.CPgSearchStart.Search_result(hx.al.CPgSearchStart.radioList),
                _progress: hx.al.CPgSearchStart.progress3(),
                _allResult: hx.al.CPgSearchStart.allResult(),
                _button: hx.al.CPgSearchStart.button('LOC_STOP_ID')
            });
            if ($('#set h1')) {
                $('#set h1').html(hx.l('LOC_SEARCH_CHANNELS_ID'));
            }
            $('#set').append(this.$colum.append(this.$tv.append(this._tvcount.$.after(this._tvlist.$)).after(this.$radio.append(this._radiocount.$.after(this._radiolist.$)).after(this._progress.$.after(this._allResult.$.after(this._button.$))))));
            hx.al.compose(this).append(this._button);
            this._button.on();
            this._button.$.addClass('on');
            /* initialize because page change can be a problem*/
            this._isChangePageInSetting = false;
            this._progressVariable = 0;
            this._allTPList = null;
            this._tsIndex = '0';
            this._tvlist.count = 0;
            this._radiolist.count = 0;
            this._isLoadPreferList = false;
            this._getChupdateEvt = false;
            if (hx.config.antConTpList === 'searched') {
                this._lockingDataList.length = 0;
            }
            else if (hx.config.antConTpList === 'hard') {
                this._lockingDataList = [{
                    antId: 1,
                    tpData: [10832000, 22000000, 1, "auto", 1, 1, 1]
                }, {
                    antId: 1,
                    tpData: [12552000, 22000000, 2, "auto", 1, 0, 1]
                }, {
                    antId: 1,
                    tpData: [12604000, 22000000, 1, "auto", 1, 0, 1]
                }];
            }
            /* request service */
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
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                this.dsrvChannelSearch.addEventCb('antennaConnection', this, function (aParam) {
                    _this._on_antennaConnection(aParam);
                });
                var typeObj = hx.svc.WEB_STORAGE.getLastSearchType();
                var searchType = typeObj.searchType;
                this._searchType = searchType;
                var tpSelectorObj = this.dsrvChannelSearch.getTransponderSelectorAll();
                this._allTPList = tpSelectorObj.transponderSelector;
            }
            /* Initialize */
            this._progress.$.find('dt').html('');
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                if (this._allTPList.length !== 0) {
                    hx.il.criticalTasking.crChannelSearchTasking = true;
                    // DMR uri set null ('bring in client' do not have to work)
                    hx.svc.DMR.setMedia(null);
                    hx.svc.AV_CTRL.stopCurChannel({
                        stopAV: 1,
                        stopTsr: 1
                    });
                    this.dsrvChannelSearch.StartChannelScan();
                }
                else {
                    this._progressVariable = 100;
                    this._progress.$.find('dd').eq(0).html(100 + '%');
                    this._progress.$.find('dd').eq(1).find('span').css('width', 100 + '%');
                    this._progress.$.find('dt').html(hx.l('LOC_NONE_ID'));
                    this._button.$.html(hx.l('LOC_EXIT_ID'));
                }
            }
            else if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                hx.il.criticalTasking.crChannelSearchTasking = true;
                // DMR uri set null ('bring in client' do not have to work)
                hx.svc.DMR.setMedia(null);
                hx.svc.AV_CTRL.stopCurChannel({
                    stopAV: 1,
                    stopTsr: 1
                });
                this.dsrvChannelSearch.StartTerChannelScan({
                    isAuto: false
                });
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
        cCPgSearchStart.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.CH_LIST.clearEventCb(this);
            this.dsrvChannelSearch.clearEventCb(this);
            if (hx.config.useClockRecovery === true) {
                hx.svc.CLOCK_RECOVERY.removeEventListener(this);
            }
            this._isSearching = 'true';
            this._isLoadPreferList = false;
            this.$colum.remove();
            this._channelCount = 0;
            this._tempTvCount = 0;
            this._tvCount = 0;
            this._tempRadioCount = 0;
            this._radioCount = 0;
            this._lockingDataList.length = 0;
            if (!this._isChangePageInSetting) {
                $('#set').remove();
                if ($('#opt').length > 0) {
                    $('#opt').remove();
                }
                if (hx.al.CPgLnbSearch) {
                    hx.al.CPgLnbSearch.deActive();
                }
                if (hx.al.CPgDiseqcSearch) {
                    hx.al.CPgDiseqcSearch.deActive();
                }
                if (hx.al.CPgScdSearch) {
                    hx.al.CPgScdSearch.deActive();
                }
                if (hx.al.CPgMotorSearch) {
                    hx.al.CPgMotorSearch.deActive();
                }
            }
            else {
                this._isChangePageInSetting = false;
            }
            if (this._savingProgress !== undefined) {
                hx.msg.close('progress', this._savingProgress);
                delete this._savingProgress;
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
            this._allTPList = {};
            this._tvListindex = [];
            this._radioListindex = [];
            this._tempTvList = [];
            this._tempRadioList = [];
            this._lockingDataList = [];
        };
        /*********** Direct SVC Event Start ****************/
        /*
         *  param is structured
         *     retObj.group = group;
         *     retObj.chList = chList;
         */
        cCPgSearchStart.prototype._on_ChListEvt = function (aRetObj) {
            var $chList = this._$channel;
            var tvLength, radioLength, TotalCnt;
            var noSearchResultString = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_ID');
            if ((aRetObj.chList === undefined) || (aRetObj.chList.length === 0)) {
                this._status = 'result_2';
                $chList.html('0' + ' (' + hx.l('LOC_TV_ID') + ': ' + '0' + ', ' + hx.l('LOC_RADIO_ID') + ': ' + '0' + ')');
                this._resultSet.html(noSearchResultString);
            }
            else {
                aRetObj = hx.svc.CH_LIST.getChannelCount({
                    group: 'TV'
                });
                tvLength = aRetObj.chCnt;
                aRetObj = hx.svc.CH_LIST.getChannelCount({
                    group: 'RADIO'
                });
                radioLength = aRetObj.chCnt;
                TotalCnt = tvLength + radioLength;
                $chList.html(TotalCnt + ' (' + hx.l('LOC_TV_ID') + ': ' + tvLength + ', ' + hx.l('LOC_RADIO_ID') + ': ' + radioLength + ')');
            }
        };
        /*********** Direct SVC Event End ****************/
        cCPgSearchStart.prototype._on_channelScan = function (aParam) {
            var j, k, p, u, satName, showString;
            var l_antennaId;
            var useTvLength, useRadioLength;
            var searchedTvListLength = 0;
            var searchedRadioListLength = 0;
            var i, len, lchannelType;
            var showtvListindex = 0;
            var showtvList = [];
            var showradioListindex = 0;
            var showradioList = [];
            var tvListLength;
            var radioListLength;
            var pola = '', codeRate = '', transpec = 0;
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
            switch (aParam.tpInfo.polarisation) {
                case 0:
                    pola = 'A';
                    break;
                case 1:
                    pola = 'H';
                    break;
                case 2:
                    pola = 'V';
                    break;
                default:
                    pola = 'A';
                    break;
            }
            if (aParam.tpInfo.codeRate === 'AUTO') {
                codeRate = 'A';
            }
            else {
                codeRate = aParam.tpInfo.codeRate;
            }
            this._freq = aParam.tpInfo.startFrequency;
            this._pola = pola;
            this._symbol = aParam.tpInfo.symbolRate;
            this._codeRate = codeRate;
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                var defaultSatName = 'ASTRA 1 (19.2E) ';
                satName = this.dsrvChannelSearch.getSatelliteNameById({
                    antId: aParam.tpInfo.antennaId
                });
                if (ENV.listModel.HMS1000S === ENV.model) {
                    var countryId;
                    countryId = hx.svc.SETTING_UTIL.getCountryId();
                    switch (countryId) {
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                            defaultSatName = 'ASTRA 1 (19.2E) ';
                            break;
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                            defaultSatName = 'HOTBIRD (13.0E) ';
                            break;
                        default:
                            defaultSatName = 'ASTRA 1 (19.2E) ';
                            break;
                    }
                }
                if (satName === defaultSatName && this._isNoChannel === true) {
                    this._isLoadPreferList = true;
                }
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
                    aParam.channelNumber = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
                }
                else if (aParam.tpInfo.bandwidth === '8MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency.length;
                    for (i = 0; i < length; i += 1) {
                        if (this._freq === hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[i]) {
                            break;
                        }
                    }
                    aParam.channelNumber = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
                }
                if (aParam.channelNumber === undefined) {
                    showString = 'CH -, ' + this._freq + 'kHz, ' + aParam.tpInfo.bandwidth;
                }
                else {
                    showString = 'CH' + aParam.channelNumber + ', ' + this._freq + 'kHz, ' + aParam.tpInfo.bandwidth;
                }
            }
            else {
                showString = satName + this._freq + ', ' + this._pola + ', ' + this._symbol + ', ' + this._codeRate;
            }
            if (this._freq) {
                this._progress.$.find('dt').html(showString);
            }
            else {
                this._progress.$.find('dt').html('');
            }
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                // case terrestrial model.
                if (hx.config.antConTpList === 'searched') {
                    //ms modifying
                    hx.logadd('searchParam');
                    hx.log('searchParam', 'param.tpInfo.startFrequency : ' + aParam.tpInfo.startFrequency);
                    hx.log('searchParam', 'signalStrength : ' + aParam.signalStrength);
                    if (aParam.signalStrength >= 10) {
                        codeRate = aParam.tpInfo.codeRate;
                        if (aParam.tpInfo.codeRate === 'AUTO') {
                            codeRate = 'auto';
                        }
                        if (aParam.tpInfo.modulationModes !== 0) {
                            transpec = 1;
                        }
                        this._lockingDataList.push({
                            antId: aParam.tpInfo.antennaId,
                            tpData: [parseInt(aParam.tpInfo.startFrequency, 10) * 1000, parseInt(aParam.tpInfo.symbolRate, 10) * 1000, aParam.tpInfo.polarisation, codeRate, aParam.tpInfo.modulationModes, transpec, 0]
                        });
                    }
                }
                else if (hx.config.antConTpList === 'hard') {
                    l_antennaId = aParam.tpInfo.antennaId;
                    this._lockingDataList[0].antId = l_antennaId;
                    this._lockingDataList[1].antId = l_antennaId;
                    this._lockingDataList[2].antId = l_antennaId;
                }
            }
            this._tvCount = searchedTvListLength;
            this._radioCount = searchedRadioListLength;
            this._channelCount = this._tvCount + this._radioCount;
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
        cCPgSearchStart.prototype._on_antennaConnection = function (param) {
            if (param.progress === 100) {
                hx.msg.close('progress', this.progress);
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
                if (hx.config.useClockRecovery === true) {
                    this._actonTimeRecovery();
                }
                else {
                    this._changeToLastChannel();
                    this._goExit();
                }
            }
        };
        cCPgSearchStart.prototype._on_ChannelListUpdate = function () {
            // tune to available channel
            var that = this;
            var svcChannelSearch = this.dsrvChannelSearch;
            if (this._getChupdateEvt === true) {
                return;
            }
            this._getChupdateEvt = true;
            hx.msg.close('progress', this._savingProgress);
            delete this._savingProgress;
            if ((this._searchType === 'lnb' || this._searchType === 'diseqc') && this._lockingDataList.length > 0) {
                svcChannelSearch.saveChannelSearchResult();
                this.progress = hx.msg('progress', {
                    text: hx.l('LOC_PROCESS_ANT_CONNECTION_ID'),
                    auto_close: false
                });
                svcChannelSearch.startAntennaConnection({
                    lockingDataList: this._lockingDataList
                });
            }
            else if (this._searchType === 'scd' || this._searchType === 'motor' || (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT && this._lockingDataList.length <= 0)) {
                svcChannelSearch.saveChannelSearchResult();
                this._changeToLastChannel();
                this._goExit();
            }
            else if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                this._checkTRD();
            }
        };
        cCPgSearchStart.prototype._checkTRD = function () {
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
                hx.svc.CHANNEL_SEARCH_T.saveChannelSearchResult();
                _this._goExit();
            });
        };
        cCPgSearchStart.prototype._changeToLastChannel = function () {
            var that = this;
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        cCPgSearchStart.prototype._goExit = function () {
            var targetPage;
            hx.il.criticalTasking.crChannelSearchTasking = false;
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                targetPage = 'CPgManualSearch';
            }
            else {
                if (this._searchType === 'lnb') {
                    targetPage = hx.al.CPgSearchStart.targetPage[0];
                }
                else if (this._searchType === 'diseqc') {
                    targetPage = hx.al.CPgSearchStart.targetPage[1];
                }
                else if (this._searchType === 'scd') {
                    targetPage = hx.al.CPgSearchStart.targetPage[2];
                }
                else if (this._searchType === 'motor') {
                    targetPage = hx.al.CPgSearchStart.targetPage[3];
                }
            }
            this._isChangePageInSetting = true;
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: targetPage,
                me: this
            });
        };
        cCPgSearchStart.prototype._on_alKey = function (aParam) {
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
                            this.dsrvChannelSearch.pauseScan();
                            this._isSearching = 'false';
                        }
                        else {
                            this._makeMsgbox2();
                            this.dsrvChannelSearch.pauseScan();
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
                    this.dsrvChannelSearch.stopScan();
                    bConsumed = false;
                    break;
            }
            return bConsumed;
        };
        cCPgSearchStart.prototype._on_alClicked = function (aParam) {
            var data = aParam.alData, targetPage = null, bConsumed = false;
            if (data.me === this._button) {
                bConsumed = true;
                /*STOP & SAVE BUTTON */
                if (this._progressVariable !== 100) {
                    this._makeMsgbox();
                    this.dsrvChannelSearch.pauseScan();
                    this._isSearching = 'false';
                }
                else {
                    if (this._channelCount > 0) {
                        if (this._isTRDConflictDlgOpened) {
                            this._checkTRD();
                        }
                        else {
                            this._savingProgress = hx.msg('progress', {
                                text: hx.l('LOC_SAVING_CHANNELS_ID'),
                                auto_close: false
                            });
                            if (!this._isLoadPreferList) {
                                this.dsrvChannelSearch.loadChannelSearchResult();
                            }
                            else {
                                if (hx.config.useDefaultChannels === 1) {
                                    this.dsrvChannelSearch.loadChannelSearchResultWithDefCh();
                                }
                                else {
                                    this.dsrvChannelSearch.loadChannelSearchResult();
                                }
                            }
                        }
                    }
                    else {
                        this.dsrvChannelSearch.stopScan();
                        if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                            this._changeToLastChannel();
                        }
                        this._goExit();
                    }
                }
            }
            return bConsumed;
        };
        cCPgSearchStart.prototype._on_alExit = function (aParam) {
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
        cCPgSearchStart.prototype._actonTimeRecovery = function () {
            hx.log('searchParam', '_actonTimeRecovery()');
            this.progress = hx.msg('progress', {
                text: hx.l('LOC_INSTALL_SW_UPDATE_SYSTEM_TIME_ITEM_ID'),
                auto_close: false
            });
            hx.svc.CLOCK_RECOVERY.startClockRecovery();
        };
        cCPgSearchStart.prototype._on_ClockRecoveryEvent = function (aParam) {
            hx.log('searchParam', '_on_ClockRecoveryEvent()');
            if (this.progress) {
                hx.msg.close('progress', this.progress);
                this.progress = undefined;
            }
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                this._changeToLastChannel();
            }
            this._goExit();
        };
        cCPgSearchStart.prototype._makeMsgbox = function () {
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
                        that.dsrvChannelSearch.resumeScan();
                        that._isSearching = 'true';
                    }
                    else if (p === 'key_esc' || p === 'key_back') {
                        hx.msg.close('confirm', msgbox);
                        that._button.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        that.dsrvChannelSearch.resumeScan();
                        that._isSearching = 'true';
                    }
                }
            });
        };
        cCPgSearchStart.prototype._makeMsgbox2 = function () {
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
                        if (!that._isLoadPreferList) {
                            that.dsrvChannelSearch.loadChannelSearchResult();
                        }
                        else {
                            if (hx.config.useDefaultChannels === 1) {
                                that.dsrvChannelSearch.loadChannelSearchResultWithDefCh();
                            }
                            else {
                                that.dsrvChannelSearch.loadChannelSearchResult();
                            }
                        }
                    }
                    else if (p === noBtn || (p === yesBtn && that._channelCount <= 0)) {
                        hx.msg.close('confirm', msgbox2);
                        that.dsrvChannelSearch.stopScan();
                        if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                            that._changeToLastChannel();
                        }
                        that._goExit();
                    }
                    else if (p === 'key_esc' || p === 'key_back') {
                        hx.msg.close('confirm', msgbox2);
                        that._button.on();
                        that._tempTvCount = that._tvCount;
                        that._tempRadioCount = that._radioCount;
                        that.dsrvChannelSearch.resumeScan();
                        that._isSearching = 'true';
                    }
                }
            });
        };
        return cCPgSearchStart;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgSearchStart = cCPgSearchStart;
    /* DATAS IN THIS PAGE */
    hx.al.CPgSearchStart.targetPage = ['CPgLnbSearch', 'CPgDiseqcSearch', 'CPgScdSearch', 'CPgMotorSearch'];
    hx.al.CPgSearchStart.tvList = [];
    hx.al.CPgSearchStart.radioList = [];
    /* COMPONENTS IN THIS PAGE */
    hx.al.CPgSearchStart.string = function (param) {
        return hx.al.component('text', {
            tag: 'h2',
            text: param,
            attr: {
                'data-langID': false,
                'class': ''
            }
        });
    };
    hx.al.CPgSearchStart.button = function (param) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': param
            },
            usePointerKey: true
        }));
    };
    /*
     hx.al.CPgSearchStart.Search_result = function(param, param2) {
     var Search_result = hx.al.component('Search_result', {
     itemAttr : {},
     attr : {
     'class' : param2
     },
     data : param,
     count : param.length,
     fx : function(i, o) {
     o.html(this.data[i]);
     }
     });
     return Search_result;
     };
     */
    hx.al.CPgSearchStart.Search_result = function (param) {
        var searchresult = hx.al.component('Search_result', {
            itemAttr: {},
            attr: {
                'style': 'height:208px'
            },
            data: param,
            count: param.length,
            fx: function (i, o) {
                o.html(this.data[i]);
            }
        });
        return searchresult;
    };
    hx.al.CPgSearchStart.progress3 = function () {
        return hx.al.component('searchProgress', {
            tag: 'dl',
            data: [' ', '0%', 'width:0%'],
            attr: {
                'class': 'progress type3'
            }
        });
    };
    hx.al.CPgSearchStart.allResult = function () {
        return hx.al.component('txt2', {
            tag: 'p',
            attr: {
                'class': 'srcResult'
            },
            data: [hx.l('LOC_CHANNELS_FOUND_ID'), '0']
        });
    };
    return cCPgSearchStart;
});
