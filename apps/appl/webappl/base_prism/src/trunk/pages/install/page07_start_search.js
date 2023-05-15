var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgInstall07 = (function (_super) {
        __extends(cCPgInstall07, _super);
        function cCPgInstall07() {
            _super.call(this);
            this._selectValue = false;
            this._indexValue = 0;
            if (hx.config.wzd.page07) {
                this.items = hx.config.wzd.page07;
            }
            else {
                this.items = {
                    itemStr: null,
                    nextPage: 'CPgInstall08',
                    prevPage: 'CPgInstall04',
                    skipPage: 'CPgInstall09'
                };
            }
        }
        cCPgInstall07.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            //hx.logadd('CPgInstall07');
            if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                this.dsrvChannelSearch = hx.svc.CHANNEL_SEARCH_T;
            }
            else {
                this.dsrvChannelSearch = hx.svc.CHANNEL_SEARCH_S;
            }
            var i, _result;
            var nowStep;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(false);
            this._containerEl = document.querySelector('#wiz');
            this._$article = $('<article>', {
                'class': 'cont'
            });
            this._ul = null;
            this._WzdSearchMainMenu = this.WzdSearchMainMenu(this.items.itemStr, 'notroll');
            this._pre_btn = this.__make_btn('LOC_PREVIOUS_ID');
            this._next_btn = this.__make_btn('LOC_NEXT_ID');
            this._$hArea = $('<h1>', {
                'class': 'hArea'
            });
            this._$title = $('<span>', {
                'class': 'tit',
                'html': hx.l('LOC_CHANNELSEARCH_ID')
            });
            this._stepEl = document.createElement('span');
            this._menuWrapEl = document.createElement('article');
            this._contextMenuEl = document.createElement('div');
            this._wzdsearchInfoEl = document.createElement('div');
            this._wzdsearchIconEl = document.createElement('p');
            this._wzdsearchImgEl = document.createElement('img');
            this._wzdsearchTextEl = document.createElement('p');
            this._descriptEl = document.createElement('p');
            this._descriptEl2 = document.createElement('p');
            this._btnWrapEl = document.createElement('div');
            this._incoEl = document.createElement('article');
            this._$WzdsearchMenu = null;
            //////////////////set Element attribute
            if (aPageUrl.query === 'prev') {
                this._$article.css('left', '-50px');
            }
            else {
                this._$article.css('left', '50px');
            }
            if (ENV.op === ENV.listOP.MIDDLE_EAST || ENV.listModel.HMS1000S === ENV.model) {
                nowStep = 'step4';
            }
            else {
                //nowStep = 'step3';
                nowStep = hx.al.CPgInstall07.items.step ? hx.al.CPgInstall07.items.step : 'step3';
            }
            this._stepEl.classList.add(nowStep);
            this._menuWrapEl.classList.add('menu');
            this._menuWrapEl.classList.add('wrap');
            this._contextMenuEl.classList.add('dep2');
            this._wzdsearchInfoEl.classList.add('chInfo');
            this._wzdsearchIconEl.classList.add('chIcon');
            this._wzdsearchImgEl.setAttribute('src', './product/ir4000hd/image/611_IR_WIZ_Ch.png');
            this._wzdsearchTextEl.innerHTML = hx.l('LOC_WATCH_RECORD_CHANNELS2_ID');
            this._wzdsearchTextEl.classList.add('ch');
            this._descriptEl.style.width = '1030px';
            this._descriptEl2.style.width = '1030px';
            this._descriptEl.style.margin = '0px 0px 0px 66px';
            this._descriptEl2.style.margin = '12px 0px 0px 66px';
            this._btnWrapEl.classList.add('btnBo');
            /////////////////append To DOM
            if (!this._containerEl) {
                this._containerEl = document.createElement('section');
                this._containerEl.id = 'wiz';
                document.querySelector('#wrap').appendChild(this._containerEl);
            }
            this._$hArea[0].appendChild(this._$title[0]);
            this._$hArea[0].appendChild(this._stepEl);
            this._descriptEl.innerHTML = "To be able to watch and record TV, your aerial needs to be connected. Please ensure your main aerial cable is connected to the recorder.";
            this._descriptEl2.innerHTML = "The search will begin automatically once you select NEXT and press OK.";
            this._incoEl.innerHTML = '<div class="box cab2">' + '</div>';
            this._incoEl.classList.add('inco');
            this._btnWrapEl.appendChild(this._pre_btn.$[0]);
            this._btnWrapEl.appendChild(this._next_btn.$[0]);
            this._$article[0].appendChild(this._descriptEl);
            this._$article[0].appendChild(this._descriptEl2);
            this._$article[0].appendChild(this._incoEl);
            this._$article[0].appendChild(this._btnWrapEl);
            this._containerEl.style.display = 'none';
            this._containerEl.appendChild(this._$hArea[0]);
            this._containerEl.appendChild(this._$article[0]);
            this._containerEl.style.display = 'block';
            /////////////Do Initial Script
            hx.al.compose(this).append(this._pre_btn).append(this._next_btn);
            this._next_btn.on();
            this._next_btn.$.addClass('on');
            this._isGoWizardPage = false;
            this._isDoingDiseqcScan = false;
            hx.il.ani.action({
                targets: this._$hArea,
                cls: 'aniFadeIn'
            });
            hx.il.ani.action({
                targets: [this._$article],
                cls: 'aniFadeIn',
                style: 'left:0'
            });
            if ((ENV.op !== ENV.listOP.AUSDTT) && (ENV.op !== ENV.listOP.UKDTT)) {
                this.dsrvChannelSearch.addEventCb('diseqcScanResult', this, function (aParam) {
                    _this._on_diseqcScanResult(aParam);
                });
                this.dsrvChannelSearch.removeAntennaList();
            }
            hx.svc.CH_LIST.removeWholeChannelList();
        };
        cCPgInstall07.prototype.destroy = function () {
            this.dsrvChannelSearch.clearEventCb(this);
            if (this._containerEl) {
                if (!this._isGoWizardPage) {
                    if (this.progress) {
                        hx.msg.close('progress', this.progress);
                    }
                    if (this._isDoingDiseqcScan) {
                        this.dsrvChannelSearch.stopScan();
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
            this._$article = undefined;
            this._ul = undefined;
            this._WzdSearchMainMenu = undefined;
            this._pre_btn = undefined;
            this._next_btn = undefined;
            this._$hArea = undefined;
            this._$title = undefined;
            this._stepEl = undefined;
            this._menuWrapEl = undefined;
            this._contextMenuEl = undefined;
            this._wzdsearchInfoEl = undefined;
            this._wzdsearchIconEl = undefined;
            this._wzdsearchImgEl = undefined;
            this._wzdsearchTextEl = undefined;
            this._descriptEl = undefined;
            this._descriptEl2 = undefined;
            this._btnWrapEl = undefined;
            this._incoEl = undefined;
            this._$WzdsearchMenu = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cCPgInstall07.prototype.__make_btn = function (aId) {
            return hx.al.component('button', {
                attr: {
                    'class': 'button',
                    'data-langID': aId
                },
                usePointerKey: true
            });
        };
        cCPgInstall07.prototype._on_diseqcScanResult = function (aParam) {
            var i, antInfo, antInfoCount = 0, ulMax = 0;
            var stAntennaList = [];
            var countryId;
            var satName = 'ASTRA 1 (19.2E) ';
            if (aParam.scanSuccess) {
                hx.msg.close('progress', this.progress);
                if (ENV.listModel.IR4000HD === ENV.model) {
                    antInfoCount = this.dsrvChannelSearch.getstartDiseqcScanAntennaListCount();
                    hx.log('CPgInstall07', 'getstartDiseqcScanAntennaListCount : ' + antInfoCount);
                    for (i = 0; i < antInfoCount; i += 1) {
                        antInfo = this.dsrvChannelSearch.getstartDiseqcScanAntennaInfo({
                            index: i
                        });
                        hx.log('CPgInstall07', 'getstartDiseqcScanAntennaInfo get Antenna name : ' + antInfo.satelliteName);
                        stAntennaList.push(antInfo);
                    }
                    // Delete current Antenna list
                    this.dsrvChannelSearch.removeAntennaList();
                    for (i = 0; i < stAntennaList.length; i += 1) {
                        hx.log('CPgInstall07', 'getstartDiseqcScanAntennaInfo reset Antenna name : ' + antInfo.satelliteName);
                        this._set_autoDisecScanCompletedAntenna(i, stAntennaList[i]);
                    }
                }
                this._isDoingDiseqcScan = false;
                this._go_next_page();
            }
            else {
                hx.msg.close('progress', this.progress);
                this._isDoingDiseqcScan = false;
                if (ENV.listModel.IR4000HD === ENV.model) {
                    this.dsrvChannelSearch.setSatelliteSetting({
                        satName: 'HOTBIRD (13.0E) ',
                        switchInput: 0,
                        lnbFreq: 0,
                        toneEnable: 0
                    });
                }
                else if (ENV.listModel.HMS1000S === ENV.model) {
                    countryId = hx.svc.SETTING_UTIL.getCountryId();
                    switch (countryId) {
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                            satName = 'ASTRA 1 (19.2E) ';
                            break;
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                            satName = 'HOTBIRD (13.0E) ';
                            break;
                        default:
                            break;
                    }
                    this.dsrvChannelSearch.setSatelliteSetting({
                        satName: satName,
                        switchInput: 0,
                        lnbFreq: 0,
                        toneEnable: 0
                    });
                }
                else {
                    this.dsrvChannelSearch.setSatelliteSetting({
                        satName: 'ASTRA 1 (19.2E) ',
                        switchInput: 0,
                        lnbFreq: 0,
                        toneEnable: 0
                    });
                }
                this.dsrvChannelSearch.saveAntennaInfo({
                    searchType: 0 //lnb
                });
                this._go_skip_next_page();
            }
        };
        cCPgInstall07.prototype._on_alExit = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._WzdSearchMainMenu:
                case this._WzdSearchsubList:
                case this._pre_btn:
                case this._next_btn:
                    bConsumed = true;
                    this._go_pre_page();
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._on_alScrollup = function (aParam) {
            var cmpt;
            var bConsumed = false;
            if (ENV.listModel.IR4000HD === ENV.model) {
                if (aParam.alData.me === this._pre_btn || aParam.alData.me === this._next_btn) {
                    bConsumed = true;
                    if (aParam.alData.me === this._pre_btn) {
                        this._pre_btn.$.removeClass('on');
                    }
                    else if (aParam.alData.me === this._next_btn) {
                        this._next_btn.$.removeClass('on');
                    }
                    this._WzdSearchMainMenu.setItemFocus(0);
                    this._WzdSearchMainMenu.on();
                }
                else if (aParam.alData.me === this._WzdSearchMainMenu) {
                    bConsumed = true;
                    this._next_btn.on();
                    this._next_btn.$.addClass('on');
                }
                else if (aParam.alData.me === this._WzdSearchsubList) {
                    bConsumed = true;
                    cmpt = this._WzdSearchsubList;
                    this._subMenuScrollup(cmpt);
                }
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._on_alScrolldown = function (aParam) {
            var cmpt;
            var bConsumed = false;
            if (ENV.listModel.IR4000HD === ENV.model) {
                if (aParam.alData.me === this._pre_btn || aParam.alData.me === this._next_btn) {
                    bConsumed = true;
                    if (aParam.alData.me === this._pre_btn) {
                        this._pre_btn.$.removeClass('on');
                    }
                    else if (aParam.alData.me === this._next_btn) {
                        this._next_btn.$.removeClass('on');
                    }
                    this._WzdSearchMainMenu.on();
                }
                else if (aParam.alData.me === this._WzdSearchMainMenu) {
                    bConsumed = true;
                    this._next_btn.on();
                    this._next_btn.$.addClass('on');
                }
                else if (aParam.alData.me === this._WzdSearchsubList) {
                    bConsumed = true;
                    cmpt = this._WzdSearchsubList;
                    this._subMenuScrolldown(cmpt);
                }
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._on_alScrollLeft = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                    bConsumed = true;
                    this._pre_btn.$.removeClass('on');
                    this._next_btn.on();
                    this._next_btn.$.addClass('on');
                    break;
                case this._next_btn:
                    bConsumed = true;
                    this._next_btn.$.removeClass('on');
                    this._pre_btn.on();
                    this._pre_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                    bConsumed = true;
                    this._pre_btn.$.removeClass('on');
                    this._next_btn.on();
                    this._next_btn.$.addClass('on');
                    break;
                case this._next_btn:
                    bConsumed = true;
                    this._next_btn.$.removeClass('on');
                    this._pre_btn.on();
                    this._pre_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._on_alClicked = function (aParam) {
            var bConsumed = false;
            var _key = aParam.alKey;
            var data = aParam.alData, target = data.me, index;
            if (aParam.alData.me === this._pre_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._go_pre_page();
            }
            if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                if ((aParam.alData.me === this._next_btn) && (_key === hx.key.KEY_OK) && (this._selectValue === false)) {
                    // on
                    bConsumed = true;
                    this.dsrvChannelSearch.antennaSetting();
                    this.dsrvChannelSearch.removeAntennaList();
                    //that._set_autoSearchAntennaInfo();
                    this.progress = hx.msg('progress', {
                        text: hx.l('LOC_SEARCHING_SIGNAL_ID'),
                        auto_close: false
                    });
                    this.dsrvChannelSearch.startDiseqcScan();
                    this._isDoingDiseqcScan = true;
                }
                else if ((aParam.alData.me === this._next_btn) && (_key === hx.key.KEY_OK) && (this._selectValue === true)) {
                    // search skip
                    bConsumed = true;
                    this._go_skip_next_page();
                }
            }
            else if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                if (aParam.alData.me === this._next_btn && _key === hx.key.KEY_OK) {
                    bConsumed = true;
                    this._go_next_page();
                }
            }
            else {
                if (aParam.alData.me === this._next_btn && _key === hx.key.KEY_OK) {
                    bConsumed = true;
                    this.dsrvChannelSearch.antennaSetting();
                    this.dsrvChannelSearch.removeAntennaList();
                    this._set_autoSearchAntennaInfo();
                    this.progress = hx.msg('progress', {
                        text: hx.l('LOC_SEARCHING_SIGNAL_ID'),
                        auto_close: false
                    });
                    this.dsrvChannelSearch.startDiseqcScan();
                    this._isDoingDiseqcScan = true;
                }
            }
            if (target === this._WzdSearchMainMenu) {
                bConsumed = true;
                this._makesubList(data.index);
                this._currentSubMenu = data.index;
            }
            else if (target === this._WzdSearchsubList) {
                bConsumed = true;
                this._contextMenuEl.classList.remove('active');
                this._setAutoSearch(data.index);
                if (data.index === 0) {
                    this._selectValue = false;
                    this._indexValue = 0;
                }
                else if (data.index === 1) {
                    this._selectValue = true;
                    this._indexValue = 1;
                }
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    bConsumed = true;
                    if (aParam.alData.me === this._WzdSearchsubList) {
                        var menuFocusIndex = this._currentSubMenu;
                        this._WzdSearchsubList.$.remove();
                        this._contextMenuEl.classList.remove('active');
                        this._WzdSearchMainMenu.getItem(menuFocusIndex).removeClass('his');
                        this._WzdSearchMainMenu.setItemFocus(menuFocusIndex);
                        this._WzdSearchMainMenu.on();
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgInstall07.prototype._go_pre_page = function () {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(this.items.prevPage + '?prev', true);
        };
        cCPgInstall07.prototype._go_next_page = function () {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(this.items.nextPage, false);
        };
        cCPgInstall07.prototype._go_skip_next_page = function () {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(this.items.skipPage, false);
        };
        cCPgInstall07.prototype._fadeOutAni = function (aTargetPage, aIsGoPrev) {
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
        cCPgInstall07.prototype._set_autoSearchAntennaInfo = function () {
            if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                this._set_autoSearchAntennaInfo_IR4000HD();
            }
            else if (ENV.listModel.HMS1000S === ENV.model) {
                this._set_autoSearchAntennaInfo_HMS1000S();
            }
            else {
                this._set_autoSearchAntennaInfo_Default();
            }
        };
        cCPgInstall07.prototype._set_autoSearchAntennaInfo_Default = function () {
            this.dsrvChannelSearch.updateNumSatelSetting({
                numSat: 0
            });
            this.dsrvChannelSearch.setSatelliteSetting({
                satName: 'ASTRA 1 (19.2E) ',
                switchInput: 0,
                lnbFreq: 0,
                toneEnable: 0
            });
            this.dsrvChannelSearch.saveAntennaInfo({
                searchType: 0 //LNB
            });
        };
        cCPgInstall07.prototype._set_autoSearchAntennaInfo_HMS1000S = function () {
            var countryId;
            countryId = hx.svc.SETTING_UTIL.getCountryId();
            switch (countryId) {
                case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                    this.dsrvChannelSearch.updateNumSatelSetting({
                        numSat: 0
                    });
                    this.dsrvChannelSearch.setSatelliteSetting({
                        satName: 'ASTRA 1 (19.2E) ',
                        switchInput: 0,
                        lnbFreq: 0,
                        toneEnable: 0
                    });
                    this.dsrvChannelSearch.saveAntennaInfo({
                        searchType: 0 //LNB
                    });
                    break;
                case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                    // 'HOTBIRD (13.0E) '
                    this.dsrvChannelSearch.updateNumSatelSetting({
                        numSat: 2
                    });
                    this.dsrvChannelSearch.setSatelliteSetting({
                        satName: 'HOTBIRD (13.0E) ',
                        switchInput: 2,
                        lnbFreq: 0,
                        toneEnable: 0
                    });
                    this.dsrvChannelSearch.saveAntennaInfo({
                        searchType: 1 //DISEQC
                    });
                    break;
            }
        };
        cCPgInstall07.prototype._set_autoSearchAntennaInfo_IR4000HD = function () {
            // 'NILESAT 101&amp;102 (7.0W) '
            this.dsrvChannelSearch.updateNumSatelSetting({
                numSat: 0
            });
            this.dsrvChannelSearch.setSatelliteSetting({
                satName: 'NILESAT 101&amp;102 (7.0W) ',
                switchInput: 0,
                lnbFreq: 0,
                toneEnable: 0
            });
            this.dsrvChannelSearch.saveAntennaInfo({
                searchType: 1 //DISEQC
            });
            // 'BADR 4&amp;6 (26.0E) '
            this.dsrvChannelSearch.updateNumSatelSetting({
                numSat: 1
            });
            this.dsrvChannelSearch.setSatelliteSetting({
                satName: 'BADR 4&amp;6 (26.0E) ',
                switchInput: 1,
                lnbFreq: 0,
                toneEnable: 0
            });
            this.dsrvChannelSearch.saveAntennaInfo({
                searchType: 1 //DISEQC
            });
            // 'HOTBIRD (13.0E) '
            this.dsrvChannelSearch.updateNumSatelSetting({
                numSat: 2
            });
            this.dsrvChannelSearch.setSatelliteSetting({
                satName: 'HOTBIRD (13.0E) ',
                switchInput: 2,
                lnbFreq: 0,
                toneEnable: 0
            });
            this.dsrvChannelSearch.saveAntennaInfo({
                searchType: 1 //DISEQC
            });
            // 'YAHLIVE (52.5E) '
            this.dsrvChannelSearch.updateNumSatelSetting({
                numSat: 3
            });
            this.dsrvChannelSearch.setSatelliteSetting({
                satName: 'YAHLIVE (52.5E) ',
                switchInput: 3,
                lnbFreq: 0,
                toneEnable: 0
            });
            this.dsrvChannelSearch.saveAntennaInfo({
                searchType: 1 //DISEQC
            });
        };
        cCPgInstall07.prototype._set_autoDisecScanCompletedAntenna = function (aIndex, aAntinfo) {
            var anteName = '', satelliteName = aAntinfo.satelliteName.toUpperCase();
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna idx : ' + aIndex);
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna satelliteName : ' + aAntinfo.satelliteName);
            if (satelliteName.match('NILESAT')) {
                anteName = 'NILESAT 101&amp;102 (7.0W) ';
            }
            else if (satelliteName.match('BADR')) {
                anteName = 'BADR 4&amp;6 (26.0E) ';
            }
            else if (satelliteName.match('HOTBIRD')) {
                anteName = 'HOTBIRD (13.0E) ';
            }
            else if (satelliteName.match('HOT')) {
                anteName = 'HOTBIRD (13.0E) ';
            }
            else if (satelliteName.match('YAHLIVE')) {
                anteName = 'YAHLIVE (52.5E) ';
            }
            else {
                anteName = 'NONE';
            }
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna conver Antenna Name : ' + anteName);
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna conver antinfo.lnbFrequency : ' + aAntinfo.lnbFrequency);
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna conver antinfo.switchInput : ' + aAntinfo.switchInput);
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna conver antinfo.enable22KHzTone : ' + aAntinfo.enable22KHzTone);
            hx.log('CPgInstall07', '_set_autoDisecScanCompletedAntenna conver antinfo.antennaType : ' + aAntinfo.antennaType);
            this.dsrvChannelSearch.updateNumSatelSetting({
                numSat: aIndex
            });
            this.dsrvChannelSearch.setSatelliteSetting({
                satName: anteName,
                switchInput: aAntinfo.switchInput,
                lnbFreq: aAntinfo.lnbFrequency,
                toneEnable: aAntinfo.enable22KHzTone
            });
            this.dsrvChannelSearch.saveAntennaInfo({
                searchType: aAntinfo.antennaType //DISEQC
            });
        };
        cCPgInstall07.prototype._set_deleteautoDisecScanCompletedAntenna = function (aAntInfoCount) {
            var i = 0, ulMax = 0;
            hx.log('CPgInstall07', '_set_deleteautoDisecScanCompletedAntenna antInfoCount : ' + aAntInfoCount);
            if (hx.config.maxdiseqcScanAntennaSlot === undefined) {
                ulMax = 0;
            }
            else {
                ulMax = hx.config.maxdiseqcScanAntennaSlot;
            }
            hx.log('CPgInstall07', '_set_deleteautoDisecScanCompletedAntenna ulMax : ' + ulMax);
            for (i = aAntInfoCount; i < ulMax; i += 1) {
                this.dsrvChannelSearch.updateNumSatelSetting({
                    numSat: i
                });
                this.dsrvChannelSearch.setSatelliteSetting({
                    satName: hx.l('LOC_NONE_ID'),
                    switchInput: 0,
                    lnbFreq: 0,
                    toneEnable: 0
                });
                this.dsrvChannelSearch.removeAnt();
            }
        };
        cCPgInstall07.prototype._makesubList = function (aParam) {
            var index = 0, subFocusIndex = 0, dataList;
            dataList = this.items.itemSub01Str;
            this._WzdSearchsubList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            this._contextMenuEl.style.display = 'none';
            this._contextMenuEl.classList.add('active');
            this._WzdSearchMainMenu.getItem(aParam).addClass('his');
            this.append(this._WzdSearchsubList);
            this._contextMenuEl.appendChild(this._WzdSearchsubList.$[0]);
            this._WzdSearchsubList.on();
            this._WzdSearchsubList.bind(dataList);
            this._WzdSearchsubList.draw();
            this._contextMenuEl.style.display = 'block';
        };
        cCPgInstall07.prototype._setAutoSearch = function (aIndex) {
            var $autosearchSub = this._WzdSearchMainMenu.getItem(0).find('em');
            var autosearchKey = hx.l(this.items.itemSub01Str[aIndex]);
            $autosearchSub.html(autosearchKey);
            this._WzdSearchMainMenu.getItem(0).removeClass('his');
            this._WzdSearchsubList.$.remove();
            this._contextMenuEl.classList.remove('active');
            this._WzdSearchMainMenu.on();
            this._WzdSearchMainMenu.setItemFocus(0);
        };
        cCPgInstall07.prototype.WzdSearchMainMenu = function (aParam, aParam2) {
            var WzdSearchMainMenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep1'
                },
                data: aParam,
                roll: aParam2,
                count: aParam.length,
                fx: function (i, o) {
                    var dataString = '';
                    if (this.data[i]) {
                        dataString = hx.l(this.data[i]);
                    }
                    o.html(dataString).append($('<em>'));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return WzdSearchMainMenu;
        };
        cCPgInstall07.prototype.WzdSearchSubMenu = function (aParam) {
            var WzdSearchSubMenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep2'
                },
                data: aParam,
                count: aParam.length,
                fx: function (i, o) {
                    var dataString = '';
                    if (this.data[i]) {
                        dataString = hx.l(this.data[i]);
                    }
                    o.html(dataString);
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
            });
            return WzdSearchSubMenu;
        };
        return cCPgInstall07;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall07 = cCPgInstall07;
    if (hx.config.wzd.page07) {
        hx.al.CPgInstall07.items = hx.config.wzd.page07;
    }
    else {
        hx.al.CPgInstall07.items = {
            itemStr: [],
            nextPage: 'CPgInstall08',
            prevPage: 'CPgInstall04',
            skipPage: 'CPgInstall08_1',
            step: 'step3'
        };
    }
    return cCPgInstall07;
});
