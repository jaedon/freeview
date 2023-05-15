var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __NETFLIX__) {
    var cCPgInstall09 = (function (_super) {
        __extends(cCPgInstall09, _super);
        function cCPgInstall09() {
            _super.call(this);
            this._status = 'result_1'; //result_1, result_2, result_3
            /*
             _result : {
             '_one' : 'English',
             '_two' : 'Unknown',
             '_three' : '0 (TV: 0, Radio: 0)',
             '_four' : 'Not Connected'
             },   */
            this.TLanguageList = {
                LANGUAGECODE: 0,
                LANGUAGESTRID: 1
            };
            this._languageListMap = [
                ['eng', 'ger', 'tur', 'rus', 'fre', 'ara', 'per', 'gla', 'wel', 'gle'],
                ['LOC_SET_ENGLISH_ID', 'LOC_SET_GERMAN_ID', 'LOC_SET_TURKISH_ID', 'LOC_SET_RUSSIAN_ID', 'LOC_SET_FRENCH_ID', 'LOC_SET_ARABIC_ID', 'LOC_SET_PERSIAN_ID', 'LOC_LANG_GARLIC_ID', 'LOC_LANG_WELSH_ID', 'LOC_LANG_IRISH_ID']
            ];
            this._otaSource = null;
            this._state = 'normal';
            this._softwareDetect = null;
            this._softwareDownload = null;
            this._usbCopyWaiting = null;
            this._ssuDialog = null;
            this._usbOTAConfirmMsg = null;
            this._enumSource = hx.svc.SW_UPDATE.TSWUSource;
        }
        cCPgInstall09.prototype.create = function (aEntity, aPageUrl) {
            hx.logadd('CPgInstall09');
            var i = 0; //, _result;
            var tvLength = 0, radioLength = 0;
            var noSearchResultString;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(false);
            $.extend(this, {
                _containerEl: document.querySelector('#wiz'),
                _$article: $('<article>', {
                    'class': 'cont'
                }),
                _boxWrapEl: document.createElement('dl'),
                _langTextEl: document.createElement('dt'),
                _languageEl: document.createElement('dd'),
                _antennaTextEl: document.createElement('dt'),
                _antennaEl: document.createElement('dd'),
                _channelTextEl: document.createElement('dt'),
                _channelEl: document.createElement('dd'),
                _connectionTextEl: document.createElement('dt'),
                _connectionEl: document.createElement('dd'),
                _resultSetEl: document.createElement('p'),
                _resultSetEl2: document.createElement('p'),
                _btnWrapEl: document.createElement('div'),
                _$hArea: $('<h1>', {
                    'class': 'hArea'
                }),
                _$title: $('<span>', {
                    'class': 'tit',
                    'html': "SETUP COMPLETE"
                }),
                _pre_btn: this.__make_btn('LOC_PREVIOUS_ID'),
                _exit_btn: null
            });
            this._$article.css('left', '50px');
            this._resultSetEl.style.width = '1030px';
            this._resultSetEl.style.margin = '0px 0px 0px 66px';
            this._resultSetEl2.style.width = '1030px';
            this._resultSetEl2.style.margin = '12px 0px 0px 66px';
            this._btnWrapEl.classList.add('btnBo');
            this._boxWrapEl.classList.add('rest');
            this._boxWrapEl.classList.add('box');
            this._softwareDetect = new DetectNewSoftware(this);
            hx.svc.SW_UPDATE.addEventListener({
                'event': 'UpdateEvent',
                'objTarget': this
            });
            if (ENV.op === ENV.listOP.AUSDTT) {
                this._langTextEl.innerHTML = hx.l('LOC_SELECTED_REGION_ID');
            }
            else if (ENV.listModel.FVP4000T === ENV.model) {
                this._langTextEl.innerHTML = hx.l('LOC_PREFERRED_LANGUAGE_ID');
            }
            else {
                this._langTextEl.innerHTML = hx.l('LOC_SELECTED_LANGUAGE_ID');
            }
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                this._antennaTextEl.innerHTML = hx.l('LOC_SATELLITE_ANTENNA_ID');
            }
            this._channelTextEl.innerHTML = hx.l('LOC_CHANNELS_FOUND_ID');
            this._connectionTextEl.innerHTML = hx.l('LOC_NETWORK_CONNECTION_ID');
            var retObj = hx.svc.CH_LIST.getChlist({
                group: 'TV'
            });
            if (retObj.chList) {
                tvLength = retObj.chList.length;
            }
            retObj = hx.svc.CH_LIST.getChlist({
                group: 'RADIO'
            });
            if (retObj.chList) {
                radioLength = retObj.chList.length;
            }
            if (tvLength === 0 && radioLength === 0) {
                this._status = 'result_2';
                this._channelEl.innerHTML = '0';
            }
            else {
                var TotalCnt = tvLength + radioLength;
                this._channelEl.innerHTML = TotalCnt + ' (' + hx.l('LOC_TV_ID') + ': ' + tvLength + ', ' + hx.l('LOC_RADIO_ID') + ': ' + radioLength + ')';
            }
            if (ENV.op === ENV.listOP.AUSDTT) {
                var regionId;
                var RegionObj = hx.svc.SETTING_UTIL.getRegionId();
                for (i = 0; i < hx.config.wzd.page03.regionId.length; i += 1) {
                    if (RegionObj === hx.config.wzd.page03.regionId[i]) {
                        regionId = i;
                    }
                }
                this._drawRegion(regionId);
            }
            else {
                var LangObj = hx.svc.SETTING_UTIL.getLanguage();
                this._drawMenuLanguage(LangObj);
            }
            var conType = hx.svc.SETTING_UTIL.getSignalConnectionType();
            var state = hx.svc.SETTING_UTIL.getConnectionState();
            // TODO : change to satellite
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                if (conType < 0) {
                    this._antennaEl.innerHTML = hx.l('LOC_NO_SIG_ID');
                }
                else if (conType === 0) {
                    this._antennaEl.innerHTML = hx.l('LOC_SINGLE_CABLE_ID');
                }
                else if (conType === 1 || conType === 2) {
                    this._antennaEl.innerHTML = hx.l('LOC_DUAL_CABLE_ID');
                }
                else {
                    this._antennaEl.innerHTML = hx.l('LOC_UNKNOWN_ID');
                }
            }
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                if (state === 'not') {
                    if (this._status === 'result_2') {
                        noSearchResultString = '<strong>' + "Unfortunately no channels were found during setup." + '</strong>' + "Please check your aerial. Select PREVIOUS and press OK to try searching for channels again.";
                        this._resultSetEl.innerHTML = noSearchResultString;
                        this._resultSetEl2.innerHTML = "Alternatively, select NEXT and press OK to search for channels manually later.";
                    }
                    else {
                        this._status = 'result_3';
                        this._resultSetEl.innerHTML = '<strong>' + "We recommend connecting your box to the internet to make full use of our services." + '</strong>' + "Select FINISH and press OK to start watching Live TV.";
                        this._resultSetEl2.innerHTML = "Alternatively, select PREVIOUS and press OK to try connecting to the internet again.";
                    }
                    this._connectionEl.innerHTML = hx.l('LOC_NOT_CONNECTED_ID');
                }
                else if (state === 'dhcp' || state === 'wifi') {
                    //this._connectionEl.innerHTML = hx.l('LOC_CONNECTED_DHCP_ID');
                    if (state == 'dhcp') {
                        this._connectionEl.innerHTML = hx.l('LOC_CONNECTED_BY_LAN_ID');
                    }
                    else if (state == 'wifi') {
                        this._connectionEl.innerHTML = hx.l('LOC_CONNECTED_BY_WLAN_ID');
                    }
                    if (this._status === 'result_2') {
                        noSearchResultString = '<strong>' + "Unfortunately no channels were found during setup." + '</strong>' + "Please check your aerial. Select PREVIOUS and press OK to try searching for channels again.";
                        this._resultSetEl.innerHTML = noSearchResultString;
                        this._resultSetEl2.innerHTML = "Alternatively, select NEXT and press OK to search for channels manually later.";
                    }
                    else {
                        this._resultSetEl.style.margin = '0px 0px 0px 130px';
                        this._resultSetEl.style.textAlign = 'center';
                        this._resultSetEl.style.width = '900px';
                        this._resultSetEl2.style.margin = '20px 0px 0px 130px';
                        this._resultSetEl2.style.textAlign = 'center';
                        this._resultSetEl2.style.width = '900px';
                        this._resultSetEl.innerHTML = '<strong>' + "You have now successfully completed the setup process." + '</strong>';
                        this._resultSetEl2.innerHTML = "Select FINISH and press OK to start watching Live TV.";
                    }
                }
            }
            else {
                if (state === 'not') {
                    if (this._status === 'result_2') {
                        noSearchResultString = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_WITHOUT_MOTOR_02_ID');
                        this._resultSetEl.innerHTML = noSearchResultString;
                    }
                    else {
                        this._status = 'result_3';
                        this._resultSetEl.innerHTML = '<strong>' + hx.l('LOC_CONNECT_INTERNET_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_WITHOUT_MOTOR_02_ID');
                    }
                    this._connectionEl.innerHTML = hx.l('LOC_NOT_CONNECTED_ID');
                }
                else if (state === 'dhcp' || state === 'wifi') {
                    this._connectionEl.innerHTML = hx.l('LOC_CONNECTED_DHCP_ID');
                    if (this._status === 'result_2') {
                        noSearchResultString = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_WITHOUT_MOTOR_02_ID');
                        this._resultSetEl.innerHTML = noSearchResultString;
                    }
                    else {
                        this._resultSetEl.style.margin = '0px 0px 0px 130px';
                        this._resultSetEl.style.textAlign = 'center';
                        this._resultSetEl.style.width = '900px';
                        this._resultSetEl.innerHTML = "You have now successfully completed the setup process." + "<br>" + "Select FINISH and press OK to start watching Live TV.";
                    }
                }
            }
            /////////////////append To DOM
            if (!this._containerEl) {
                this._containerEl = document.createElement('section');
                this._containerEl.id = 'wiz';
                document.querySelector('#wrap').appendChild(this._containerEl);
            }
            this._boxWrapEl.appendChild(this._langTextEl);
            this._boxWrapEl.appendChild(this._languageEl);
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                this._boxWrapEl.appendChild(this._antennaTextEl);
                this._boxWrapEl.appendChild(this._antennaEl);
            }
            this._boxWrapEl.appendChild(this._channelTextEl);
            this._boxWrapEl.appendChild(this._channelEl);
            this._boxWrapEl.appendChild(this._connectionTextEl);
            this._boxWrapEl.appendChild(this._connectionEl);
            this._btnWrapEl.appendChild(this._pre_btn.$[0]);
            this._$article[0].appendChild(this._resultSetEl);
            this._$article[0].appendChild(this._resultSetEl2);
            this._$article[0].appendChild(this._boxWrapEl);
            this._$article[0].appendChild(this._btnWrapEl);
            this._$hArea[0].appendChild(this._$title[0]);
            this._containerEl.style.display = 'none';
            this._containerEl.appendChild(this._$hArea[0]);
            this._containerEl.appendChild(this._$article[0]);
            this._containerEl.style.display = 'block';
            /////////////Do Initial Script
            hx.al.compose(this).append(this._pre_btn);
            this._set_exit_button();
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
            if (hx.config.netflix) {
                hx.svc.NETFLIX.start({
                    type: __NETFLIX__.START_TYPE.SUSPENDED_AT_POWER_ON
                });
            }
        };
        cCPgInstall09.prototype.destroy = function () {
            if (this._containerEl) {
                if (!this._isGoWizardPage) {
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
            if (this._softwareDownload) {
                this._softwareDownload.stopDownload();
                delete this._softwareDownload;
                this._softwareDownload = null;
            }
            if (this._softwareDetect) {
                this._softwareDetect.cancel();
                delete this._softwareDetect;
                this._softwareDetect = null;
            }
            this._$article = undefined;
            this._boxWrapEl = undefined;
            this._langTextEl = undefined;
            this._languageEl = undefined;
            this._antennaTextEl = undefined;
            this._antennaEl = undefined;
            this._channelTextEl = undefined;
            this._channelEl = undefined;
            this._connectionEl = undefined;
            this._resultSetEl = undefined;
            this._resultSetEl2 = undefined;
            this._btnWrapEl = undefined;
            this._$hArea = undefined;
            this._$title = undefined;
            this._pre_btn = undefined;
            this._exit_btn = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
            this._updateCurrentOTATasking(false);
        };
        cCPgInstall09.prototype._show = function () {
            var that = this;
            that._$article[0].style.display = 'block';
        };
        cCPgInstall09.prototype._hide = function () {
            var that = this;
            that._$article[0].style.display = 'none';
        };
        cCPgInstall09.prototype._drawMenuLanguage = function (aLangObj) {
            if (ENV.listModel.FVP4000T === ENV.model) {
                var diplayTxt = this._conv_iso639_to_DispText(aLangObj.LangAudio);
                this._languageEl.innerHTML = hx.l(diplayTxt);
            }
            else {
                var diplayTxt = this._conv_iso639_to_DispText(aLangObj.LangMenu);
                this._languageEl.innerHTML = hx.l(diplayTxt);
            }
        };
        cCPgInstall09.prototype._drawRegion = function (aRegionObj) {
            var diplayTxt = hx.l(hx.al.CPgInstall03.items.itemStr[aRegionObj]);
            if (aRegionObj !== 0) {
                this._boxWrapEl.classList.add('rest2');
            }
            //that._languageEl.style.height = '100px';
            this._languageEl.innerHTML = diplayTxt;
        };
        /*********** Direct SVC Event End ****************/
        cCPgInstall09.prototype._set_exit_button = function () {
            if (this._status === 'result_2') {
                this._exit_btn = this.__make_btn('LOC_NEXT_ID');
            }
            else {
                this._exit_btn = this.__make_btn('LOC_FINISH_ID');
            }
            this._btnWrapEl.appendChild(this._exit_btn.$[0]);
            hx.al.compose(this).append(this._exit_btn);
            this._exit_btn.on();
            this._exit_btn.$.addClass('on');
        };
        cCPgInstall09.prototype._on_alScrollLeft = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                    bConsumed = true;
                    this._pre_btn.$.removeClass('on');
                    this._exit_btn.on();
                    this._exit_btn.$.addClass('on');
                    break;
                case this._exit_btn:
                    bConsumed = true;
                    this._exit_btn.$.removeClass('on');
                    this._pre_btn.on();
                    this._pre_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall09.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                    bConsumed = true;
                    this._pre_btn.$.removeClass('on');
                    this._exit_btn.on();
                    this._exit_btn.$.addClass('on');
                    break;
                case this._exit_btn:
                    bConsumed = true;
                    this._exit_btn.$.removeClass('on');
                    this._pre_btn.on();
                    this._pre_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall09.prototype._on_alExit = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                case this._exit_btn:
                    bConsumed = true;
                    this._go_pre_page();
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgInstall09.prototype._on_alClicked = function (aParam) {
            var bConsumed = false;
            var _key = aParam.alKey, name = aParam.alData.me.name;
            if (aParam.alData.me === this._pre_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._go_pre_page();
            }
            else if (aParam.alData.me === this._exit_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                hx.svc.SETTING_UTIL.offFirstTimeBoot();
                hx.il.isInWizardPage = false;
                hx.svc.FRONT_PANEL.clearFrontPanelText();
                this._checkNewSoftware();
            }
            else if (aParam.alData.me === this._softwareDownload.downloadBtn && _key === hx.key.KEY_OK) {
                function clickedFunc(name) {
                    return {
                        'newSoftware': 'getNewSoftware',
                        'download': 'stopDownload'
                    }[name];
                }
                hx.log("CPgInstall09", hx.util.format('++++++ ######## _on_alClicked : _isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
                var func = '_' + clickedFunc(name);
                hx.log("CPgInstall09", hx.util.format('name : {0}, func : {1}', 'newSoftware', func));
                if (this[func]) {
                    this[func]();
                    bConsumed = true;
                }
            }
            return bConsumed;
        };
        cCPgInstall09.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        cCPgInstall09.prototype._on_UpdateEvent = function (param) {
            var that = this;
            var SwUpdateEventInfo = param.SwUpdateEventInfo;
            var detectSW = that._softwareDetect;
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID'), start = hx.l('LOC_START_THE_UPDATE_ID').toUpperCase();
            var enumSWUEvent = hx.svc.SW_UPDATE.TSWUEvent;
            var eventType = SwUpdateEventInfo.getInfo('event');
            var source = SwUpdateEventInfo.getInfo('source_type');
            if (that._state === 'complete') {
                hx.log('warning', '\n *********** Completed OTA. will be restart system ***********\n');
                return true;
            }
            // auto update without user input
            if (!!hx.config.useWakeUpOTA || !!hx.config.useLiveIPOTA) {
                if (!!that._pageUrl && that._pageUrl.param === "startBootupHumaxOta") {
                    that._pageUrl = null;
                    // clear state
                    if (that._bBootupOtaInProgress && (eventType === enumSWUEvent.EEventDetectSuccess)) {
                        that._otaSource = source;
                        that._closeDetectWaitingProgress();
                        that._startDownload(source);
                        // Bootup OTA may use IP, event if ota type is RF at the bootup time.
                        that._bBootupOtaInProgress = false;
                        return;
                    }
                }
            }
            function callback_fn(status) {
                switch (status) {
                    case 'key_back':
                    case 'key_esc':
                        hx.msg.close('ssuDialog', that._ssuDialog);
                        that._ssuDialog = null;
                        that._cancelDetect();
                        hx.al.compose(that).append(that._exit_btn);
                        break;
                    case 'key_power':
                    case 'auto_close':
                    case no:
                        hx.msg.close('ssuDialog', that._ssuDialog);
                        that._ssuDialog = null;
                        that._cancelDetect();
                        that._go_next_page();
                        break;
                    case yes:
                    case start:
                        hx.msg.close('ssuDialog', that._ssuDialog);
                        that._ssuDialog = null;
                        that._hide();
                        that._startDownload(source);
                        break;
                }
            }
            if (eventType === enumSWUEvent.EEventDetectSuccess) {
                that._state = 'showDialog';
                that._otaSource = source;
                var curver = SwUpdateEventInfo.getInfo('current_version');
                var foundver = SwUpdateEventInfo.getInfo('found_version');
                if (source === that._enumSource.ESourceRf || source === that._enumSource.ESourceIp) {
                    if (hx.config.op === 'SES') {
                        that._ssuDialog = detectSW.showSuccessMessageForSES(param, callback_fn);
                    }
                    else {
                        that._ssuDialog = detectSW.showSuccessMessage(curver, foundver, callback_fn);
                    }
                }
                else if (source === that._enumSource.ESourceUsb) {
                    that._usbOTAConfirmMsg = detectSW.showSuccessUSBMessage(that, function (waitingHandle) {
                        that._usbCopyWaiting = waitingHandle;
                        that._startDownload(source);
                    });
                }
            }
            else if (eventType === enumSWUEvent.EEventDetectFail) {
                that._state = 'normal';
                that._updateCurrentOTATasking(false);
                that._closeDetectWaitingProgress();
                that._exit_btn.off();
                detectSW.showFailMessage(that);
                hx.log("CPgInstall09", hx.util.format('ssuDialog handle : {0}, usbOTAConfirmMsg handle : {1}', that._ssuDialog, that._usbOTAConfirmMsg));
            }
            else if (that._softwareDownload) {
                var progress = SwUpdateEventInfo.getInfo('progress');
                var progressMax = SwUpdateEventInfo.getInfo('progress_max');
                if ((eventType === enumSWUEvent.EEventDownloadFail) || (eventType === enumSWUEvent.EEventInstallFail)) {
                    var text = hx.l('LOC_DOWNLOAD_FAIL_ID');
                    var ok_btn = hx.l('LOC_OK_ID');
                    var confirmMsg = hx.msg('confirm', {
                        'text': text,
                        'auto_close': false,
                        'btn_title_arr': [ok_btn],
                        'callback_fn': function (_status) {
                            // hx.log("CPgInstall09", hx.util.format('[_on_UpdateProgress] _status : {0}', _status));
                            if (_status === ok_btn || _status === 'key_back' || _status === 'key_esc') {
                                that._state = 'normal';
                                that._show();
                                that._exit_btn.on();
                                hx.msg.close('confirm', confirmMsg);
                            }
                        }
                    });
                    if (that._usbCopyWaiting) {
                        hx.log("CPgInstall09", 'hx.msg.close[usbCopyWaiting - progress]');
                        hx.msg.close('progress', that._usbCopyWaiting);
                        that._usbCopyWaiting = null;
                    }
                    that._updateCurrentOTATasking(false);
                }
                that._softwareDownload.updateEvent(param);
            }
        };
        cCPgInstall09.prototype._go_pre_page = function () {
            var pgInstall09; // = {};
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            if (ENV.listModel.IR4000HD === ENV.model) {
                pgInstall09 = hx.al.CPgInstall09Time;
            }
            else {
                pgInstall09 = hx.al.CPgInstall09;
            }
            this._fadeOutAni(pgInstall09.items.prevPage + '?prev', true);
        };
        cCPgInstall09.prototype._go_next_page = function () {
            var that = this;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            var pgInstall09; // = {};
            hx.svc.SETTING_UTIL.offFirstTimeBoot();
            hx.il.isInWizardPage = false;
            dsrvFrontPanel.clearFrontPanelText();
            if (ENV.listModel.IR4000HD === ENV.model) {
                pgInstall09 = hx.al.CPgInstall09Time;
            }
            else {
                pgInstall09 = hx.al.CPgInstall09;
            }
            that.sendEventToEntity({
                alEvent: 'alChangePage',
                //target : pgInstall09.items.nextPage+'?startService',
                target: 'CPgLiveController?startIntroVideo',
                me: that
            });
        };
        cCPgInstall09.prototype.__make_btn = function (aId) {
            return hx.al.component('button', {
                attr: {
                    'class': 'button',
                    'data-langID': aId
                },
                usePointerKey: true
            });
        };
        cCPgInstall09.prototype._fadeOutAni = function (aTargetPage, aIsGoPrev) {
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
                        alEvent: 'alChangeInstallPage',
                        target: aTargetPage,
                        me: _this
                    });
                }
            });
        };
        cCPgInstall09.prototype._conv_iso639_to_DispText = function (aIso639) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var DispText = 'LOC_UNKNOWN_ID';
            var i = 0;
            for (i = 0; i < languageListCode.length; i += 1) {
                if (aIso639 === languageListCode[i]) {
                    DispText = languageListStrID[i];
                    break;
                }
            }
            return DispText;
        };
        cCPgInstall09.prototype._cancelDetect = function () {
            var that = this;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _cancelDetect : isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            that._state = 'normal';
            that._softwareDetect.cancel();
            that._exit_btn.on();
            that._updateCurrentOTATasking(false);
        };
        cCPgInstall09.prototype._closeDetectWaitingProgress = function () {
            var that = this;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _closeDetectWaitingProgress : isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            if (that._softwareDetect) {
                that._softwareDetect.closeWaitingProgress();
            }
        };
        cCPgInstall09.prototype._startDownload = function (source) {
            var that = this;
            var currentRecordings, length;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _startDownload : isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            var has_HotScheduledItem, scheduleTimerId = null;
            that._softwareDownload = new cSoftwareDownload(that, source);
            that._state = 'downloading';
        };
        cCPgInstall09.prototype._stopDownload = function () {
            var that = this;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _stopDownload : isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            that._state = 'normal';
            that._otaSource = null;
            if (that._softwareDownload) {
                that._softwareDownload.stopDownload();
            }
            that._show();
            that._exit_btn.on();
            that._updateCurrentOTATasking(false);
        };
        cCPgInstall09.prototype._isOtaWorking = function () {
            return hx.il.criticalTasking.crOTATasking;
        };
        cCPgInstall09.prototype._updateCurrentOTATasking = function (currentState) {
            hx.il.criticalTasking.crOTATasking = currentState;
            if (currentState) {
                // DMR uri set null ('bring in client' do not have to work)
                hx.svc.DMR.setMedia({
                    uri: null
                });
            }
        };
        cCPgInstall09.prototype._showDetectWaitingProgress = function () {
            var that = this;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _showDetectWaitingProgress : isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            if (that._softwareDetect) {
                that._softwareDetect.showWaitingProgress();
            }
        };
        cCPgInstall09.prototype._tryGetNewSoftwareDetect = function () {
            var that = this;
            var enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
            var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _tryGetNewSoftwareDetect : isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            if (hx.config.useOTA === 'DVBSSU') {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
            }
            else if (hx.config.useOTA === 'ASTRA') {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
            }
            else {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
            }
            if (that._softwareDetect) {
                that._state = 'swdetect';
                that._softwareDetect.getNewSoftwareWithType(ulOtaSpec);
                that._updateCurrentOTATasking(true);
            }
        };
        cCPgInstall09.prototype._checkNewSoftware = function () {
            var _this = this;
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var dsrvTVPortal = hx.svc.TV_PORTAL;
            var state = "";
            var networkAvailable = false;
            var internetAvailable = false;
            var fn = function (xml) {
                var version;
                try {
                    version = $(xml).find('version').text();
                }
                catch (e) {
                    hx.log("CPgInstall09", "_checkNewSoftware exception: " + e);
                }
                if (!!version) {
                    state = dsrvSettingUtil.getConnectionState();
                    networkAvailable = dsrvSettingUtil.getNetworkAvailable();
                    internetAvailable = dsrvSettingUtil.getInternetAvailable();
                    if (state === 'dhcp' || state === 'wifi') {
                        if ((networkAvailable === true) && (internetAvailable === true)) {
                            _this._getNewSoftware();
                        }
                    }
                    else {
                        hx.log("CPgInstall09", "_____no internet_____");
                        _this._go_next_page();
                    }
                }
                else {
                    hx.log("CPgInstall09", "_____not found ip ota new version_____");
                    _this._go_next_page();
                }
            };
            var err = function () {
                hx.log("CPgInstall09", "_____not found ip ota new version_____");
                _this._go_next_page();
            };
            try {
                dsrvTVPortal.checkNewSoftware({
                    fn: fn,
                    fnError: err
                });
            }
            catch (e) {
                hx.log("CPgInstall09", "_____exception: " + e + "_____");
                this._go_next_page();
            }
        };
        cCPgInstall09.prototype._getNewSoftware = function () {
            var that = this;
            hx.log("CPgInstall09", hx.util.format('++++++ ######## _getNewSoftware : _isOtaWorking {0}, state : {1} ######## ++++++', that._isOtaWorking(), that._state));
            if (that._state !== 'normal') {
                return;
            }
            var fnAutoUpdate = function () {
                that._showDetectWaitingProgress();
                //show Progress : 'Searching for new software...'
                that._tryGetNewSoftwareDetect();
            };
            fnAutoUpdate();
        };
        return cCPgInstall09;
    })(__PAGE_KERNEL__);
    var DetectNewSoftware = (function (_super) {
        __extends(DetectNewSoftware, _super);
        function DetectNewSoftware(aParentObject) {
            _super.call(this);
            this._parent = null;
            this._waiting = null;
            this._ssuDialog = null;
            this._copyWaiting = null;
            this._parent = aParentObject;
        }
        DetectNewSoftware.prototype.showWaitingProgress = function () {
            var that = this;
            if (that._waiting === null) {
                that._waiting = hx.msg('progress', {
                    'text': hx.l('LOC_CHECKING_NEW_SOFTWARE_UPDATE_MSG_ID'),
                    'auto_close': false,
                    'dia_class': '',
                    'callback_fn': function (status) {
                        hx.log("CPgInstall09", hx.util.format('++++++ ######## showWaitingProgress : status {0} ######## ++++++', status));
                        if (status === 'key_back') {
                            that.closeWaitingProgress();
                            that.cancel();
                        }
                    }
                });
            }
            return that._waiting;
        };
        DetectNewSoftware.prototype.closeWaitingProgress = function () {
            var that = this;
            if (that._waiting) {
                hx.msg.close('progress', that._waiting);
                that._waiting = null;
            }
        };
        DetectNewSoftware.prototype.closeUsbCopyWaitingProgress = function () {
            var that = this;
            if (that._copyWaiting) {
                hx.msg.close('progress', that._copyWaiting);
                that._copyWaiting = null;
            }
        };
        DetectNewSoftware.prototype.getNewSoftwareWithType = function (aParamOtaType) {
            var that = this;
            hx.svc.AV_CTRL.stopCurChannel({
                'stopAV': 1,
                'stopTsr': 1
            });
            hx.svc.SW_UPDATE.startDetectWithType({
                'otaType': aParamOtaType
            });
        };
        DetectNewSoftware.prototype.cancel = function () {
            var that = this;
            var a = that._parent;
            if (that._waiting) {
                hx.msg.close('progress', that._waiting);
                that._waiting = null;
            }
            hx.svc.SW_UPDATE.cancelDetect();
            a._state = 'normal';
        };
        DetectNewSoftware.prototype.showSuccessMessageForSES = function (param, callback) {
            var that = this;
            var dsrvSWUpdate = hx.svc.SW_UPDATE;
            var SwUpdateEventInfo = param.SwUpdateEventInfo;
            var curVer = SwUpdateEventInfo.getInfo('current_version');
            var newVer = SwUpdateEventInfo.getInfo('found_version');
            var otaType = SwUpdateEventInfo.getInfo('ota_type');
            var source = SwUpdateEventInfo.getInfo('source_type');
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID'), start = hx.l('LOC_START_THE_UPDATE_ID').toUpperCase();
            var isForcedOTA = function (type) {
                return (type === dsrvSWUpdate.TSWUType.EFORCED) || false;
            };
            if (isForcedOTA(otaType)) {
                that._ssuDialog = hx.msg('ssuDialog', {
                    title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                    text: hx.l('LOC_OTA_WILLBE_DESC_ID'),
                    first_line_arr: [hx.l('LOC_CUR_VER_ID'), curVer],
                    second_line_arr: [hx.l('LOC_NEW_VER_ID'), newVer],
                    auto_close: true,
                    timeout: 30000,
                    btn_title_arr: [start],
                    dia_class: 'osd',
                    callback_fn: callback
                });
            }
            else {
                that._ssuDialog = hx.msg('ssuDialog', {
                    title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                    text: hx.l('LOC_OTA_USERSELECT_DESC_ID'),
                    first_line_arr: [hx.l('LOC_CUR_VER_ID'), curVer],
                    second_line_arr: [hx.l('LOC_NEW_VER_ID'), newVer],
                    auto_close: true,
                    timeout: 30000,
                    btn_title_arr: [yes, no],
                    dia_class: 'osd',
                    callback_fn: callback
                });
            }
            that.closeWaitingProgress();
            return that._ssuDialog;
        };
        DetectNewSoftware.prototype.showSuccessMessage = function (currentVersion, newVersion, cb) {
            var that = this;
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
            that._ssuDialog = hx.msg('ssuDialog', {
                title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                text: hx.l('LOC_NEWSWDETECTED_ID'),
                text1: hx.l('LOC_DOYOUWANTUPDATE_ID'),
                first_line_arr: [hx.l('LOC_CURSWVERSION_ID'), currentVersion],
                second_line_arr: [hx.l('LOC_NEWSWVERSION_ID'), newVersion],
                auto_close: true,
                timeout: 30000,
                btn_title_arr: [yes, no],
                dia_class: 'osd',
                callback_fn: cb
            });
            that.closeWaitingProgress();
            return that._ssuDialog;
        };
        DetectNewSoftware.prototype.showSuccessUSBMessage = function (a, successCB) {
            var that = this;
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
            var selectMsg = hx.msg('ssuDialog', {
                title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                text: hx.l('LOC_USB_NEWSWDETECTED_ID'),
                text1: hx.l('LOC_DOYOUWANTUPDATE_NEW_ID'),
                auto_close: true,
                timeout: 30000,
                btn_title_arr: [yes, no],
                dia_class: 'osd',
                callback_fn: function (_status) {
                    if (_status === yes) {
                        that._copyWaiting = hx.msg('progress', {
                            text: hx.l('LOC_COPYING_USB_SOFTWARE_ID'),
                            auto_close: false,
                            dia_class: '',
                            callback_fn: function (status) {
                                if (status === 'key_back') {
                                    hx.msg.close('progress', that._copyWaiting);
                                    that._copyWaiting = null;
                                    that.cancel();
                                }
                            }
                        });
                        if (successCB) {
                            successCB(that._copyWaiting);
                        }
                    }
                    else if (_status === no) {
                        that.cancel();
                        hx.msg.close('ssuDialog', selectMsg);
                        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
                        var pgInstall09; // = {};
                        hx.svc.SETTING_UTIL.offFirstTimeBoot();
                        hx.il.isInWizardPage = false;
                        dsrvFrontPanel.clearFrontPanelText();
                        if (ENV.listModel.IR4000HD === ENV.model) {
                            pgInstall09 = hx.al.CPgInstall09Time;
                        }
                        else {
                            pgInstall09 = hx.al.CPgInstall09;
                        }
                        a.sendEventToEntity({
                            alEvent: 'alChangePage',
                            //target : pgInstall09.items.nextPage+'?startService',
                            target: 'CPgLiveController?startIntroVideo',
                            me: a
                        });
                    }
                    else if (_status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                        that.cancel();
                        hx.msg.close('ssuDialog', selectMsg);
                    }
                }
            });
            that.closeWaitingProgress();
            return selectMsg;
        };
        DetectNewSoftware.prototype.showFailMessage = function (a) {
            var _this = this;
            var that = this;
            hx.msg('response', {
                text: hx.l('LOC_NEW_SOFTWARE_NOT_DETECTED_ID'),
                auto_close: true,
                close_time: 3000,
                callback_fn: function (status) {
                    if (status === 'auto_close') {
                        var that = _this;
                        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
                        var pgInstall09; // = {};
                        hx.svc.SETTING_UTIL.offFirstTimeBoot();
                        hx.il.isInWizardPage = false;
                        dsrvFrontPanel.clearFrontPanelText();
                        if (ENV.listModel.IR4000HD === ENV.model) {
                            pgInstall09 = hx.al.CPgInstall09Time;
                        }
                        else {
                            pgInstall09 = hx.al.CPgInstall09;
                        }
                        a.sendEventToEntity({
                            alEvent: 'alChangePage',
                            //target : pgInstall09.items.nextPage+'?startService',
                            target: 'CPgLiveController?startIntroVideo',
                            me: a
                        });
                    }
                }
            });
            that.closeWaitingProgress();
            that.cancel();
        };
        return DetectNewSoftware;
    })(__PAGE_KERNEL__);
    var cSoftwareDownload = (function () {
        function cSoftwareDownload(aTargetObject, aSource) {
            this._parent = null;
            this._enumSource = hx.svc.SW_UPDATE.TSWUSource;
            this._SUCCESSED = 1;
            this._FAIL = 0;
            this._create(aTargetObject, aSource);
        }
        cSoftwareDownload.prototype._create = function (targetObject, source) {
            var that = this;
            that._parent = targetObject;
            that._bStandbyOta = that._parent._bStandbyOtaInstallInProgress || false;
            if ((source === that._enumSource.ESourceRf || source === that._enumSource.ESourceIp || that._bStandbyOta) && (hx.config.op !== 'SES')) {
                $.extend(that, {
                    $: $('<article>', {
                        'class': 'wrap alignC colum2'
                    }),
                    _$title: $('<h2>', {
                        'class': 'infoTitle',
                        text: hx.l('LOC_DOWNLOAD_PROGRESS_ID')
                    }),
                    _$info: $('<p>', {
                        'class': 'infoTxt',
                        html: hx.l('LOC_DOWNLOAD_IS_BEING_DOWNLOAD_ID') + '<br><em>' + hx.l('LOC_DO_NOT_TURN_OFF_ID') + '</em>'
                    }),
                    _progress: hx.al.component('searchProgress', {
                        tag: 'dl',
                        data: [' ', '0%', 'width:0%'],
                        attr: {
                            'class': 'progress type3 infoProgress'
                        }
                    }),
                    downloadBtn: hx.al.CPgInstall09.OTAButton('LOC_CANCEL_ID', 'download')
                });
                that.$.append(that._$title).append(that._$info).append(that._progress.$).append(that.downloadBtn.$);
                $('.cont').after(that.$);
                that._parent.append(that.downloadBtn);
                that.downloadBtn.on();
            }
            // Standby OTA Installation Check
            var enumMultiOtaSpec;
            var enumOtaStep;
            if (that._bStandbyOta === true) {
                enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
                enumOtaStep = hx.svc.SW_UPDATE.TSWUStep;
                hx.svc.SW_UPDATE.startSwupdateState({
                    otaSpec: enumMultiOtaSpec.ESPEC_HUMAX,
                    otaStep: enumOtaStep.EStepInstall
                });
            }
            else {
                enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
                var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;
                if (hx.config.useOTA === 'DVBSSU') {
                    ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
                }
                else if (hx.config.useOTA === 'ASTRA') {
                    ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
                }
                else {
                    ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
                }
                hx.svc.SW_UPDATE.startDownload({
                    'otaSpec': ulOtaSpec
                });
            }
        };
        cSoftwareDownload.prototype.destroy = function () {
            var $ = this.$;
            if (this.$ !== null) {
                this.$.remove();
            }
            if (this._parent._bStandbyOtaInstallInProgress) {
                this._parent._bStandbyOtaInstallInProgress = false;
            }
        };
        cSoftwareDownload.prototype.stopDownload = function () {
            var that = this;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            hx.svc.SW_UPDATE.cancelDownload();
            dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
            that.destroy();
        };
        cSoftwareDownload.prototype.updateEvent = function (param) {
            var that = this, cmptProgress;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            var SwUpdateEventInfo = param.SwUpdateEventInfo;
            var eventType = SwUpdateEventInfo.getInfo('event');
            var progress = SwUpdateEventInfo.getInfo('progress');
            var progressMax = SwUpdateEventInfo.getInfo('progress_max');
            var enumSWUEvent = hx.svc.SW_UPDATE.TSWUEvent;
            if ((eventType === enumSWUEvent.EEventDownloadFail) || (eventType === enumSWUEvent.EEventInstallFail)) {
                that.stopDownload();
            }
            else if ((eventType === enumSWUEvent.EEventDownloadProgress) || (eventType === enumSWUEvent.EEventInstallProgress)) {
                if (that._parent._otaSource === that._enumSource.ESourceRf || that._parent._otaSource === that._enumSource.ESourceIp || (that._bStandbyOta === true)) {
                    cmptProgress = that._progress;
                    cmptProgress.$.find('dd').eq(0).html(progress + '%');
                    cmptProgress.$.find('dd').eq(1).find('span').css('width', progress + '%');
                }
                hx.log("CPgInstall09", "[updateEvent] progress : " + progress + ' / ' + progressMax);
                if (progress === progressMax) {
                    that._parent._state = 'complete';
                    dsrvFrontPanel.clearFrontPanelText();
                    setTimeout(function () {
                        hx.svc.SW_UPDATE.restartSystem();
                    }, 1000 * 2);
                }
                hx.svc.SETTING_UTIL.setFrontPanelString({
                    'text': hx.util.format('D{0}{1}{2}%', parseInt(progress) / 100, parseInt(progress) % 100 / 10, parseInt(progress) % 10)
                });
            }
            else {
                hx.log("CPgInstall09", "[updateEvent]Ignore Other messages: " + eventType);
                dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
            }
        };
        return cSoftwareDownload;
    })();
    hx.al.CPgInstall09 = cCPgInstall09;
    hx.al.CPgInstall09.OTAButton = function (langID, name) {
        return hx.al.component('button', {
            attr: {
                'class': 'button on',
                'data-langID': langID
            },
            usePointerKey: true,
            notSelfDestroy: true,
            name: name
        });
    };
    /********************      Static Variables     ********************/
    if (hx.config.wzd.page09) {
        hx.al.CPgInstall09.items = hx.config.wzd.page09;
    }
    else {
        hx.al.CPgInstall09.items = {
            itemStr: [''],
            nextPage: 'CPgLiveController',
            prevPage: 'CPgInstall07',
            step: 'step4'
        };
    }
    return cCPgInstall09;
});
