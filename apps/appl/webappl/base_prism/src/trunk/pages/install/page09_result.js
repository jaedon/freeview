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
        }
        cCPgInstall09.prototype.create = function (aEntity, aPageUrl) {
            hx.logadd('CPgInstall09');
            var i = 0; //, _result;
            var nowStep;
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
                _btnWrapEl: document.createElement('div'),
                _$hArea: $('<h1>', {
                    'class': 'hArea'
                }),
                _$title: $('<span>', {
                    'class': 'tit',
                    'html': hx.l('LOC_RESULT_ID')
                }),
                _stepEl: document.createElement('span'),
                _pre_btn: this.__make_btn('LOC_PREVIOUS_ID'),
                _exit_btn: null
            });
            //////////////////set Element attribute
            if (ENV.op === ENV.listOP.SES) {
                nowStep = 'step2';
            }
            else if (ENV.op === ENV.listOP.MIDDLE_EAST || ENV.listModel.HMS1000S === ENV.model || ENV.op === ENV.listOP.AUSDTT) {
                nowStep = 'step4';
            }
            else {
                nowStep = hx.al.CPgInstall09.items.step ? hx.al.CPgInstall09.items.step : 'step3';
            }
            this._stepEl.classList.add(nowStep);
            this._$article.css('left', '50px');
            this._resultSetEl.style.width = '1030px';
            this._resultSetEl.style.margin = '0px 0px 0px 66px';
            this._btnWrapEl.classList.add('btnBo');
            this._boxWrapEl.classList.add('rest');
            this._boxWrapEl.classList.add('box');
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
            this._channelTextEl.innerHTML = hx.l('LOC_SEARCHED_CHANNEL_ID');
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
                        noSearchResultString = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_WITHOUT_MOTOR_ID');
                        this._resultSetEl.innerHTML = noSearchResultString;
                    }
                    else {
                        this._status = 'result_3';
                        this._resultSetEl.innerHTML = '<strong>' + hx.l('LOC_CONNECT_INTERNET_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_WITHOUT_MOTOR_ID');
                    }
                    this._connectionEl.innerHTML = hx.l('LOC_NOT_CONNECTED_ID');
                }
                else if (state === 'dhcp' || state === 'wifi') {
                    if (state == 'dhcp') {
                        this._connectionEl.innerHTML = hx.l('LOC_CONNECTED_BY_LAN_ID');
                    }
                    else if (state == 'wifi') {
                        this._connectionEl.innerHTML = hx.l('LOC_CONNECTED_BY_WLAN_ID');
                    }
                    if (this._status === 'result_2') {
                        noSearchResultString = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_WITHOUT_MOTOR_ID');
                        this._resultSetEl.innerHTML = noSearchResultString;
                    }
                    else {
                        this._resultSetEl.style.margin = '0px 0px 0px 130px';
                        this._resultSetEl.style.textAlign = 'center';
                        this._resultSetEl.style.width = '900px';
                        this._resultSetEl.innerHTML = hx.l('LOC_COMPLETED_PRESS_EXIT_ID');
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
                        this._resultSetEl.innerHTML = hx.l('LOC_COMPLETED_PRESS_EXIT_ID');
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
            this._$article[0].appendChild(this._boxWrapEl);
            this._$article[0].appendChild(this._btnWrapEl);
            this._$hArea[0].appendChild(this._$title[0]);
            this._$hArea[0].appendChild(this._stepEl);
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
            this._btnWrapEl = undefined;
            this._$hArea = undefined;
            this._$title = undefined;
            this._stepEl = undefined;
            this._pre_btn = undefined;
            this._exit_btn = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
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
                this._exit_btn = this.__make_btn('LOC_EXIT_UPPER_ID');
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
            var _key = aParam.alKey;
            if (aParam.alData.me === this._pre_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._go_pre_page();
            }
            else if (aParam.alData.me === this._exit_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._go_next_page();
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
        return cCPgInstall09;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall09 = cCPgInstall09;
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
