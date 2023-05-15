///<reference path="../../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : product/fvp4000t/pages/settings/channel/manualChannelSearchT.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgManualSearch = (function (_super) {
        __extends(cCPgManualSearch, _super);
        function cCPgManualSearch() {
            _super.call(this);
            this.TMenuList = {
                CHANNEL: 0,
                FREQUENCY: 1,
                BANDWIDTH: 2,
                NETWORKSEARCH: 3
            };
            //sendEventToEntity : any;
            //append : any;
            /* local variable */
            this._userChannel = '21';
            this._userFreq = 474000;
            this._userBand = '8MHz';
            this._userTransmission = 'DVB-T';
            this._userNetworkSearch = 'Off';
            this._isChangePageInSetting = false;
            this._contextListMap = [
                ['LOC_CHANNEL_ID', 'LOC_FREQ_ID', 'LOC_TRANSMISSION_ID', 'LOC_BANDWIDTH_ID', 'LOC_NETWORKSCH_ID'],
                [],
                ['DVB-T', 'DVB-T2'],
                ['LOC_7MHZ_ID', 'LOC_8MHZ_ID'],
                ['LOC_ON_ID', 'LOC_OFF_ID'],
                ['', '', '', '', ''],
                [hx.l('LOC_STRENGTH_ID'), '0%', 'width:0%'],
                [hx.l('LOC_QUALITY_ID'), '0%', 'width:0%']
            ];
        }
        cCPgManualSearch.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            _super.prototype.create.call(this);
            this._userChannel = '21';
            this._userFreq = 474000;
            this._userTransmission = 'DVB-T';
            this._userBand = '8MHz';
            this._userNetworkSearch = 'Off';
            $.extend(this, {
                _bPageActive: true,
                _$menuWrap: $('<article>', {
                    'class': 'menu'
                }),
                _mainMenu: hx.al.CPgManualSearch.satmainMenu(this._contextListMap[0], this._contextListMap[5], 'dep1', 'notroll'),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _$menu2: $('<article>', {
                    'class': 'wrap sigTest icoRec'
                }),
                _$box1: $('<div>', {
                    'class': 'box',
                    'style': 'margin-top:370px'
                }),
                _userTpStrProgress: hx.al.CPgManualSearch.progress(this._contextListMap[6], 'progress type1'),
                _userTpQltProgress: hx.al.CPgManualSearch.progress(this._contextListMap[7], 'progress type2'),
                _searchBtn: hx.al.CPgManualSearch.button('LOC_SEARCH_ID'),
                /* INPUT FREQUENCY RATE */
                _$wrapAticle: $('<article>', {
                    'id': 'DIA',
                    'class': 'osd',
                    'style': "display:none;"
                }),
                _$frline: $('<div>', {
                    'class': 'pas multi',
                    'style': 'top:253px;'
                }),
                _txt4: hx.al.CPgManualSearch.txt(hx.l('LOC_ENTER_FREQUENCY_NUMBER_ID'), 'p'),
                _$wrappw: $('<p>', {
                    'class': 'wrap_pw'
                }),
                _numbers: hx.al.CPgManualSearch.numbers(6)
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_MANUAL_CHANNEL_SEARCH_ID').toUpperCase();
            }
            $('#set').append(this._$menuWrap.append(this._mainMenu.$.after(this._$contextMenu)).after(this._$menu2.append(this._$box1.append(this._userTpStrProgress.$.after(this._userTpQltProgress.$)).after(this._searchBtn.$))));
            $('#set').after(this._$wrapAticle.append(this._$frline.append(this._txt4.$.after(this._$wrappw.append(this._numbers.$)))));
            hx.al.compose(this).append(this._mainMenu).append(this._searchBtn).append(this._numbers);
            hx.il.criticalTasking.crChannelSearchTasking = true;
            // DMR uri set null ('bring in client' do not have to work)
            hx.svc.DMR.setMedia(null);
            hx.svc.AV_CTRL.stopCurChannel({
                stopAV: 1,
                stopTsr: 1
            });
            this._isChangePageInSetting = false;
            var tpParam = hx.svc.CHANNEL_SEARCH_T.getUserTPParam();
            this._getUserTpParam(tpParam);
            $title.ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            this._$menuWrap.ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    _this.append(_this._mainMenu);
                    _this._mainMenu.on();
                    _this._isChangePageInSetting = false;
                }
            });
            $('#set .wrap.sigTest').ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                }
            });
        };
        cCPgManualSearch.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.SETTING_UTIL.clearEventCb(this);
            hx.il.criticalTasking.crChannelSearchTasking = false;
            //TODO : lastChannel
            this._$menuWrap.remove();
            this._$contextMenu.remove();
            this._$wrapAticle.remove();
            this._$menu2.remove();
            this._userChannel = '';
            this._userFreq = 0;
            this._userBand = '';
            this._userNetworkSearch = '';
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this._$menuWrap = undefined;
            this._$contextMenu = undefined;
            this._mainMenu = undefined;
        };
        /* dl event */
        cCPgManualSearch.prototype._on_NotiSignalInfo = function (aParam) {
            var i;
            //not consider 2 conn
            var strength = 0;
            var quality = 0;
            if (aParam.signal) {
                for (i = 0; i < aParam.signal.length; i += 1) {
                    if (aParam.signal[i]) {
                        var freq = aParam.signal[i].tuningParams.split(',')[0];
                        if (aParam.signal[i].lock && freq === this._userFreq.toString()) {
                            strength = aParam.signal[i].strength;
                            quality = aParam.signal[i].quality;
                            break;
                        }
                    }
                }
            }
            if (strength > 100) {
                strength = 100;
            }
            if (quality > 100) {
                quality = 100;
            }
            this._userTpStrProgress.$.find('dd').eq(0).html(strength + '%');
            this._userTpStrProgress.$.find('dd span').css('width', strength + '%');
            this._userTpQltProgress.$.find('dd').eq(0).html(quality + '%');
            this._userTpQltProgress.$.find('dd span').css('width', quality + '%');
        };
        /* al event */
        cCPgManualSearch.prototype._on_alScrollup = function (aParam) {
            var target = aParam.alData.me, bConsumed = false;
            if (target === this._searchBtn) {
                bConsumed = true;
                this._searchBtn.$.removeClass('on');
                this._mainMenu.on();
                this._mainMenu.setItemFocus(4);
            }
            else if (target === this._mainMenu) {
                bConsumed = true;
                this._searchBtn.on();
                this._searchBtn.$.addClass('on');
            }
            else if (target === this._contextList) {
                bConsumed = true;
                this._subMenuScrollup(this._contextList);
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._on_alScrolldown = function (aParam) {
            var target = aParam.alData.me, bConsumed = false;
            if (target === this._searchBtn) {
                bConsumed = true;
                this._searchBtn.$.removeClass('on');
                this._mainMenu.on();
            }
            else if (target === this._mainMenu) {
                bConsumed = true;
                this._searchBtn.$.addClass('on');
                this._searchBtn.on();
            }
            else if (target === this._contextList) {
                bConsumed = true;
                this._subMenuScrolldown(this._contextList);
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            var $target;
            if (target === this._mainMenu) {
                bConsumed = true;
                this._mIndex = data.index;
                if (data.index === 1) {
                    this._$wrapAticle.show();
                    $target = $('#DIA .pas').eq(0);
                    hx.il.ani.action({
                        type: 'in',
                        targets: $target
                    });
                    this._numbers.on();
                }
                else {
                    this._mainMenu.getItem(data.index).addClass('his');
                    var focusIndex = this._getFocusIndex(data.index);
                    this._mainMenu.off();
                    this._make_editSubMenu(this._mIndex, focusIndex);
                }
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._on_alScrollLeft = function (aParam) {
            var _this = this;
            var target = aParam.alData.me, bConsumed = false;
            if (target === this._contextList) {
                bConsumed = true;
                this._contextList.off();
                this._$contextMenu.ani({
                    style: this.aniConfig.dep2.fadeOut,
                    trans: {
                        dur: 150,
                        delay: 75
                    },
                    cb: function () {
                        var subIndex = _this._mIndex;
                        if (_this._mIndex === 8) {
                            subIndex = 4;
                        }
                        _this._mainMenu.getItem(subIndex).removeClass('his');
                        _this._$contextMenu.removeClass('active');
                        _this._contextList.$.remove();
                        _this._mainMenu.on();
                        _this._mainMenu.setItemFocus(subIndex);
                    }
                });
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, target = data.me, bConsumed = false;
            var $target;
            if (target === that._mainMenu) {
                bConsumed = true;
                that._mIndex = data.index;
                if (data.index === 1) {
                    that._$wrapAticle.show();
                    $target = $('#DIA .pas').eq(0);
                    hx.il.ani.action({
                        type: 'in',
                        targets: $target
                    });
                    that._numbers.on();
                    that._numbers.position(0);
                }
                else {
                    that._mainMenu.getItem(data.index).addClass('his');
                    var focusIndex = that._getFocusIndex(data.index);
                    that._mainMenu.off();
                    that._make_editSubMenu(that._mIndex, focusIndex);
                }
            }
            else if (target === that._contextList) {
                bConsumed = true;
                that._contextList.off();
                that._$contextMenu.ani({
                    style: that.aniConfig.dep2.fadeOut,
                    trans: {
                        dur: 150,
                        delay: 75
                    },
                    cb: function () {
                        if (that._mIndex === 0) {
                            that._setChannel(data.index, data.getFirstItem);
                        }
                        else if (that._mIndex === 2) {
                            // TODO: transmission set
                            that._setTransmission(data.index);
                        }
                        else if (that._mIndex === 3) {
                            that._setBandwidth(data.index);
                        }
                        else if (that._mIndex === 4) {
                            that._setNetworkSearch(data.index);
                        }
                    }
                });
            }
            else if (target === that._numbers) {
                bConsumed = true;
                var $tpsubFreq = that._mainMenu.getItem(1).find('em');
                var freqkey = that._numbers.getNum();
                freqkey = parseInt(freqkey, 10);
                if (that._checkFreq(freqkey)) {
                    that._userFreq = freqkey;
                    that._setFreq(freqkey);
                    $tpsubFreq.html(freqkey);
                    hx.svc.SETTING_UTIL.clearEventCb(this);
                    hx.svc.SETTING_UTIL.stopGetSignalInfo();
                    hx.svc.CHANNEL_SEARCH_T.setUserTempTPParam({
                        userChannel: that._userChannel,
                        userFreq: that._userFreq,
                        userTransmission: that._userTransmission,
                        userBand: that._userBand,
                        userNetworkSearch: that._userNetworkSearch
                    });
                    that._doStartTune();
                }
                else {
                    hx.msg('response', {
                        text: hx.l('LOC_FREQUENCY_NOT_VALID_ID'),
                        auto_close: true,
                        close_time: 3000,
                        dia_class: 'res vaTop',
                        callback_fn: function () {
                            that._$wrapAticle.show();
                            $target = $('#DIA .pas').eq(0);
                            hx.il.ani.action({
                                type: 'in',
                                targets: $target
                            });
                            that._numbers.resetNum();
                            that._numbers.on();
                            that._numbers.position(0);
                        }
                    });
                }
                that._hideNumber(1);
            }
            else if (data.me === that._searchBtn) {
                bConsumed = true;
                /* Transponder Setting OK Button */
                hx.svc.CHANNEL_SEARCH_T.setUserTestTPParam({
                    userChannel: that._userChannel,
                    userFreq: that._userFreq,
                    userTransmission: that._userTransmission,
                    userBand: that._userBand,
                    userNetworkSearch: that._userNetworkSearch
                });
                hx.svc.CHANNEL_SEARCH_T.setUserTPParam({
                    userChannel: that._userChannel,
                    userFreq: that._userFreq,
                    userTransmission: that._userTransmission,
                    userBand: that._userBand,
                    userNetworkSearch: that._userNetworkSearch
                });
                that._isChangePageInSetting = true;
                that._stopSignal();
                that.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgSearchStart',
                    me: that
                });
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._on_alExit = function (aParam) {
            var that = this, bConsumed = false;
            if (aParam.alData) {
                if ((aParam.alData.me === that._mainMenu) || (aParam.alData.me === that._searchBtn)) {
                    bConsumed = true;
                    that._stopSignal();
                    hx.al.decompose(that);
                    that._changeToLastChannel();
                    that._$menu2.ani({
                        style: that.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        }
                    });
                    that._$menuWrap.ani({
                        style: that.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: that
                            });
                        }
                    });
                    that._isChangePageInSetting = true;
                }
                else if (aParam.alData.me === that._contextList) {
                    bConsumed = true;
                    that._contextList.off();
                    that._$contextMenu.ani({
                        style: that.aniConfig.dep2.fadeOut,
                        trans: {
                            dur: 150,
                            delay: 75
                        },
                        cb: function () {
                            var subIndex = that._mIndex;
                            if (that._mIndex === 8) {
                                subIndex = 4;
                            }
                            that._mainMenu.getItem(subIndex).removeClass('his');
                            that._$contextMenu.removeClass('active');
                            that._contextList.$.remove();
                            that._mainMenu.on();
                            that._mainMenu.setItemFocus(subIndex);
                        }
                    });
                }
                else if (aParam.alData.me === that._numbers) {
                    bConsumed = true;
                    that._hideNumber(1);
                }
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._on_alKey = function (param) {
            var _this = this;
            var bConsumed = false;
            switch (param.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    bConsumed = true;
                    this._stopSignal();
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                        text: ''
                    });
                    this._changeToLastChannel();
                    this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgLiveController',
                        me: this
                    });
                    break;
                case hx.key.KEY_POWER:
                    this._stopSignal();
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                        text: ''
                    });
                    break;
                case hx.key.KEY_BACK:
                    if (param.alData) {
                        bConsumed = true;
                        if (param.alData.me === this._contextList) {
                            this._contextList.off();
                            this._$contextMenu.ani({
                                style: this.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    var subIndex = _this._mIndex;
                                    if (_this._mIndex === 8) {
                                        subIndex = 4;
                                    }
                                    _this._mainMenu.getItem(subIndex).removeClass('his');
                                    _this._$contextMenu.removeClass('active');
                                    _this._contextList.$.remove();
                                    _this._mainMenu.on();
                                    _this._mainMenu.setItemFocus(subIndex);
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgManualSearch.prototype._hideNumber = function (param, param2) {
            var that = this, $hideTarget, $target;
            that._numbers.off();
            if (param === 1) {
                $hideTarget = that._$wrapAticle;
                $target = $('#DIA .pas').eq(0);
            }
            hx.il.ani.action({
                type: 'out',
                targets: $target,
                cb: function () {
                    $hideTarget.hide();
                    if (param === 1) {
                        that._mainMenu.on();
                        that._mainMenu.setItemFocus(1);
                    }
                    else if (param === 2) {
                        if (param2 === 'select') {
                            that._mainMenu.getItem(3).removeClass('his');
                            that._$contextMenu.removeClass('active');
                            that._contextList.$.remove();
                            that._mainMenu.on();
                            that._mainMenu.setItemFocus(3);
                        }
                        else if (param2 === 'back') {
                            that._$contextMenu.ani({
                                style: that.aniConfig.dep2.fadeIn,
                                trans: {
                                    dur: 200,
                                    delay: 100
                                },
                                cb: function () {
                                    that._contextList.on();
                                    that._contextList.getItem(4).addClass('on');
                                }
                            });
                        }
                    }
                }
            });
        };
        cCPgManualSearch.prototype._getSignalInfo = function () {
            var _this = this;
            hx.svc.SETTING_UTIL.addEventCb('NotiSignalInfo', this, function (aParam) {
                _this._on_NotiSignalInfo(aParam);
            });
            hx.svc.SETTING_UTIL.getSignalInfo();
        };
        cCPgManualSearch.prototype._changeToLastChannel = function () {
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        cCPgManualSearch.prototype._stopSignal = function () {
            hx.svc.SETTING_UTIL.stopGetSignalInfo();
            hx.svc.CHANNEL_SEARCH_T.stopScan();
        };
        cCPgManualSearch.prototype._doStartTune = function () {
            hx.svc.CHANNEL_SEARCH_T.StartTerTune();
            this._getSignalInfo();
        };
        cCPgManualSearch.prototype._tpSetAfterDoStartTune = function () {
            hx.svc.SETTING_UTIL.clearEventCb(this);
            hx.svc.SETTING_UTIL.stopGetSignalInfo();
            hx.svc.CHANNEL_SEARCH_T.setUserTempTPParam({
                userChannel: this._userChannel,
                userFreq: this._userFreq,
                userTransmission: this._userTransmission,
                userBand: this._userBand,
                userNetworkSearch: this._userNetworkSearch
            });
            this._doStartTune();
        };
        cCPgManualSearch.prototype._getUserTpParam = function (aParam) {
            this._userChannel = aParam.tpParam.userChannel;
            this._userFreq = parseInt(aParam.tpParam.userFreq, 10);
            this._userTransmission = aParam.tpParam.userTransmission;
            this._userBand = aParam.tpParam.userBand;
            this._userNetworkSearch = aParam.tpParam.userNetworkSearch;
            hx.svc.CHANNEL_SEARCH_T.setUserTempTPParam({
                userChannel: this._userChannel,
                userFreq: this._userFreq,
                userTransmission: this._userTransmission,
                userBand: this._userBand,
                userNetworkSearch: this._userNetworkSearch
            });
            this._doStartTune();
            if (this._userChannel === undefined || this._userChannel === 'undefined') {
                this._mainMenu.getItem(0).find('em').html('-');
            }
            else {
                this._mainMenu.getItem(0).find('em').html(this._userChannel);
            }
            this._mainMenu.getItem(1).find('em').html(this._userFreq);
            this._mainMenu.getItem(2).find('em').html(this._userTransmission);
            this._mainMenu.getItem(3).find('em').html(this._userBand);
            this._mainMenu.getItem(4).find('em').html(this._userNetworkSearch);
        };
        cCPgManualSearch.prototype._getFocusIndex = function (aIndex) {
            var focusIndex = 0, i;
            switch (aIndex) {
                case 0:
                    if (this._userBand === '7MHz') {
                        length = hx.config.settings.channelgroupT.bandwidth7MHz.channel.length;
                        for (i = 0; i < length; i += 1) {
                            if (this._userChannel === hx.config.settings.channelgroupT.bandwidth7MHz.channel[i]) {
                                break;
                            }
                        }
                    }
                    else if (this._userBand === '8MHz') {
                        length = hx.config.settings.channelgroupT.bandwidth8MHz.channel.length;
                        for (i = 0; i < length; i += 1) {
                            if (this._userChannel === hx.config.settings.channelgroupT.bandwidth8MHz.channel[i]) {
                                break;
                            }
                        }
                    }
                    focusIndex = i;
                    break;
                case 2:
                    //Transmission
                    if (this._userTransmission === 'DVB-T') {
                        focusIndex = 0;
                    }
                    else if (this._userTransmission === 'DVB-T2') {
                        focusIndex = 1;
                    }
                    break;
                case 3:
                    //Bandwidth
                    if (this._userBand === '7MHz') {
                        focusIndex = 0;
                    }
                    else if (this._userBand === '8MHz') {
                        focusIndex = 1;
                    }
                    break;
                case 4:
                    //Network Search
                    if (this._userNetworkSearch === 'On') {
                        focusIndex = 0;
                    }
                    else if (this._userNetworkSearch === 'Off') {
                        focusIndex = 1;
                    }
                    break;
                default:
                    break;
            }
            return focusIndex;
        };
        cCPgManualSearch.prototype._setTransmission = function (aIndex) {
            var $value = this._mainMenu.getItem(2).find('em');
            var value = this._contextListMap[this._mIndex][aIndex];
            this._userTransmission = value;
            $value.html(value);
            //TODO: userTransmission will be add
            this._tpSetAfterDoStartTune();
            this._mainMenu.getItem(2).removeClass('his');
            this._$contextMenu.removeClass('active');
            this._contextList.$.remove();
            this._mainMenu.on();
            this._mainMenu.setItemFocus(2);
        };
        cCPgManualSearch.prototype._setNetworkSearch = function (aIndex) {
            var $tpsubNetwork = this._mainMenu.getItem(4).find('em');
            var networkSearchKey = hx.l(this._contextListMap[this._mIndex][aIndex]);
            if (networkSearchKey === hx.l('LOC_ON_ID')) {
                this._userNetworkSearch = 'On';
            }
            else if (networkSearchKey === hx.l('LOC_OFF_ID')) {
                this._userNetworkSearch = 'Off';
            }
            $tpsubNetwork.html(networkSearchKey);
            this._tpSetAfterDoStartTune();
            this._mainMenu.getItem(4).removeClass('his');
            this._$contextMenu.removeClass('active');
            this._contextList.$.remove();
            this._mainMenu.on();
            this._mainMenu.setItemFocus(4);
        };
        cCPgManualSearch.prototype._setFreq = function (aFreq) {
            var $tpsubChannel = this._mainMenu.getItem(0).find('em');
            var length, tempFreq, i;
            if (this._userBand === '7MHz') {
                length = hx.config.settings.channelgroupT.bandwidth7MHz.limitFrequency.length;
                for (i = 0; i < length; i += 1) {
                    if (aFreq > hx.config.settings.channelgroupT.bandwidth7MHz.limitFrequency[i][0] && aFreq <= hx.config.settings.channelgroupT.bandwidth7MHz.limitFrequency[i][1]) {
                        break;
                    }
                }
                this._userChannel = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
            }
            else if (this._userBand === '8MHz') {
                length = hx.config.settings.channelgroupT.bandwidth8MHz.limitFrequency.length;
                for (i = 0; i < length; i += 1) {
                    if (aFreq > hx.config.settings.channelgroupT.bandwidth8MHz.limitFrequency[i][0] && aFreq <= hx.config.settings.channelgroupT.bandwidth8MHz.limitFrequency[i][1]) {
                        break;
                    }
                }
                this._userChannel = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
            }
            if (this._userChannel === undefined) {
                $tpsubChannel.html('-');
            }
            else {
                $tpsubChannel.html(this._userChannel);
            }
        };
        cCPgManualSearch.prototype._checkFreq = function (aFreq) {
            var result;
            /*
             if (that._userBand === '7MHz') {
             if ( (freq < 174000 ) || (freq > 230000 && freq < 519000) || (freq > 820000) ) {
             result = false;
             } else {
             result = true;
             }
             } else if (that._userBand === '8MHz') {
             if ( (freq < 174000 ) || (freq > 230000 && freq < 518000) || (freq > 822000) ) {
             result = false;
             } else {
             result = true;
             }
             }
             */
            result = true;
            return result;
        };
        cCPgManualSearch.prototype._setChannel = function (aIndex, aFirstItem) {
            var $tpsubChannel = this._mainMenu.getItem(0).find('em');
            var $tpsubFreq = this._mainMenu.getItem(1).find('em');
            var channelKey, freqKey, i, length;
            if (this._userBand === '7MHz') {
                length = hx.config.settings.channelgroupT.bandwidth7MHz.channel.length;
                for (i = 0; i < length; i += 1) {
                    if (aFirstItem === hx.config.settings.channelgroupT.bandwidth7MHz.channel[i]) {
                        break;
                    }
                }
                if ((aIndex += i) >= length) {
                    aIndex -= length;
                }
                this._userChannel = hx.config.settings.channelgroupT.bandwidth7MHz.channel[aIndex];
                channelKey = this._userChannel;
                this._userFreq = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[aIndex];
                freqKey = this._userFreq;
            }
            else if (this._userBand === '8MHz') {
                length = hx.config.settings.channelgroupT.bandwidth8MHz.channel.length;
                for (i = 0; i < length; i += 1) {
                    if (aFirstItem === hx.config.settings.channelgroupT.bandwidth8MHz.channel[i]) {
                        break;
                    }
                }
                if ((aIndex += i) >= length) {
                    aIndex -= length;
                }
                this._userChannel = hx.config.settings.channelgroupT.bandwidth8MHz.channel[aIndex];
                channelKey = this._userChannel;
                this._userFreq = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[aIndex];
                freqKey = this._userFreq;
            }
            if (channelKey === undefined) {
                $tpsubChannel.html('-');
            }
            else {
                $tpsubChannel.html(channelKey);
            }
            $tpsubFreq.html(freqKey);
            this._tpSetAfterDoStartTune();
            this._mainMenu.getItem(0).removeClass('his');
            this._$contextMenu.removeClass('active');
            this._contextList.$.remove();
            this._mainMenu.on();
            this._mainMenu.setItemFocus(0);
        };
        cCPgManualSearch.prototype._setBandwidth = function (aIndex) {
            var $tpsubChannel = this._mainMenu.getItem(0).find('em');
            var $tpsubFreq = this._mainMenu.getItem(1).find('em');
            var $tpsubBand = this._mainMenu.getItem(3).find('em');
            var channelKey, freqKey;
            var bandwidthKey = hx.l(this._contextListMap[this._mIndex][aIndex]);
            switch (bandwidthKey) {
                case hx.l('LOC_7MHZ_ID'):
                    if (this._userBand !== '7MHz') {
                        this._userBand = '7MHz';
                        this._userChannel = hx.config.settings.channelgroupT.bandwidth7MHz.channel[0];
                        this._userFreq = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[0];
                        channelKey = this._userChannel;
                        freqKey = this._userFreq;
                    }
                    break;
                case hx.l('LOC_8MHZ_ID'):
                    if (this._userBand !== '8MHz') {
                        this._userBand = '8MHz';
                        this._userChannel = hx.config.settings.channelgroupT.bandwidth8MHz.channel[0];
                        this._userFreq = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[0];
                        channelKey = this._userChannel;
                        freqKey = this._userFreq;
                    }
                    break;
                default:
                    this._userBand = '7MHz';
                    break;
            }
            if (channelKey === undefined) {
                $tpsubChannel.html('-');
            }
            else {
                $tpsubChannel.html(channelKey);
            }
            $tpsubFreq.html(freqKey);
            $tpsubBand.html(bandwidthKey);
            this._tpSetAfterDoStartTune();
            this._mainMenu.getItem(3).removeClass('his');
            this._$contextMenu.removeClass('active');
            this._contextList.$.remove();
            this._mainMenu.on();
            this._mainMenu.setItemFocus(3);
        };
        cCPgManualSearch.prototype._make_editSubMenu = function (aParam, aSubFocusIndex) {
            var _this = this;
            var dataList;
            var mainFocus;
            var isNotId = false;
            var topPx, i;
            var length;
            if (aParam === 0) {
                mainFocus = 0;
                isNotId = true;
                if (this._userBand === '7MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth7MHz.channel.length;
                    this._contextListMap[1] = [];
                    for (i = 0; i < length; i += 1) {
                        this._contextListMap[1][i] = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
                    }
                }
                else if (this._userBand === '8MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth8MHz.channel.length;
                    this._contextListMap[1] = [];
                    for (i = 0; i < length; i += 1) {
                        this._contextListMap[1][i] = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
                    }
                }
                dataList = this._contextListMap[1];
            }
            else if (aParam === 2) {
                isNotId = true;
                dataList = this._contextListMap[aParam];
                topPx = '182px';
            }
            else {
                dataList = this._contextListMap[aParam];
                if (aParam === this.TMenuList.BANDWIDTH)
                    topPx = '182px';
                else if (aParam === this.TMenuList.NETWORKSEARCH)
                    topPx = '255px';
            }
            this._contextList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: aSubFocusIndex,
                displayCount: dataList.length,
                _isNotId: isNotId,
                top: topPx
            });
            this._$contextMenu.addClass('active');
            mainFocus = aParam;
            if (aParam === 8) {
                mainFocus = 4;
            }
            this._mainMenu.getItem(mainFocus).addClass('his');
            this._$contextMenu.append(this._contextList.$);
            this._contextList.bind(dataList);
            this._contextList.draw();
            this._$contextMenu.ani({
                style: this.aniConfig.dep2.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    _this.append(_this._contextList);
                    _this._contextList.on();
                }
            });
        };
        return cCPgManualSearch;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgManualSearch = cCPgManualSearch;
    hx.al.CPgManualSearch.prototype.aniConfig = {
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
    hx.al.CPgManualSearch.title = function () {
        return hx.al.component('text', {
            tag: 'h1',
            attr: {
                'data-langID': 'LOC_MANUAL_CHANNEL_SEARCH_ID'
            }
        });
    };
    hx.al.CPgManualSearch.satmainMenu = function (aParam, aParam2, aParam3, aParam4) {
        var satmainmenu = hx.al.component('Search_list', {
            itemAttr: {},
            attr: {
                'class': aParam3
            },
            roll: aParam4,
            data: aParam,
            data2: aParam2,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>').html(this.data2[i]));
            }
        });
        return satmainmenu;
    };
    hx.al.CPgManualSearch.progress = function (aParam, aParam2) {
        return hx.al.component('searchProgress', {
            tag: 'dl',
            data: aParam,
            attr: {
                'class': aParam2
            }
        });
    };
    hx.al.CPgManualSearch.button = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button search',
                'data-langID': aParam
            },
            usePointerKey: true
        }));
    };
    hx.al.CPgManualSearch.txt = function (aParam, aParam2) {
        return hx.al.component('text', {
            tag: aParam2,
            text: aParam,
            attr: {
                'data-langID': false,
                'class': 'txt'
            }
        });
    };
    hx.al.CPgManualSearch.numbers = function (aParam) {
        var numbers = hx.al.component('scnumbers', {
            baseText: '',
            count: aParam,
            attr: {
                'class': 'wrap_num'
            }
        });
        return numbers;
    };
    return cCPgManualSearch;
});
