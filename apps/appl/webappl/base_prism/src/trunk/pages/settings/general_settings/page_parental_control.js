var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    function fnParentalMainMenu(param, param2) {
        return hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': param2
            },
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            data: param,
            count: param.length,
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
    }
    var _eGuidancePolicy;
    (function (_eGuidancePolicy) {
        _eGuidancePolicy[_eGuidancePolicy["LOC_OFF_ID"] = 0] = "LOC_OFF_ID";
        _eGuidancePolicy[_eGuidancePolicy["LOC_ALL_GUIDANCE_MARKED_CONTENT_ID"] = 1] = "LOC_ALL_GUIDANCE_MARKED_CONTENT_ID";
        _eGuidancePolicy[_eGuidancePolicy["LOC_INTERNET_CONTENT_ONLY_ID"] = 2] = "LOC_INTERNET_CONTENT_ONLY_ID";
        _eGuidancePolicy[_eGuidancePolicy["LOC_ALL_CONTENT_ID"] = 3] = "LOC_ALL_CONTENT_ID";
    })(_eGuidancePolicy || (_eGuidancePolicy = {}));
    var CPgParental = (function (_super) {
        __extends(CPgParental, _super);
        function CPgParental() {
            var _this = this;
            _super.call(this);
            this.TMenuList = {
                MATURITYRATING: 0,
                CHANGEPIN: 1,
                SETTINGPIN: 2
            };
            this.TSESMenuList = {
                CHANGESMARTCARDPINCODE: 0,
                CHANGEPIN: 1,
                SETTINGPIN: 2
            };
            this.TNAAccessState = {
                EOK: 0,
                ENOACCESS: 1,
                ENORIGHT: 2,
                ENOTIME: 3,
            };
            this.aniConfig = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
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
                }
            };
            this._currentSubMenu = 0;
            this._pinSetting = '';
            this._settedPassword = '';
            this._pinSettingValue = false;
            this._maturityRating = '';
            this._maturityRatingValue = 0;
            this._isChangePageInSetting = false;
            this._retryCnt = 0;
            this._accessState = 0;
            this._remainTime = 0;
            this._handlers = [
                function (aIndex) {
                    //guidance Policy
                    var dataHandlers = _this._dataHandler;
                    var stringID = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[0])[aIndex];
                    hx.svc.SETTING_UTIL.setGuidancePolicy(_eGuidancePolicy[stringID]);
                },
                function (aIndex) {
                    //change password
                    var pinSettingKey = hx.l(_this._contextListMap[2][aIndex]);
                    _this._pinSetting = pinSettingKey;
                    hx.svc.PARENTAL_CTRL.setChannelsetupPin(((pinSettingKey === hx.l('LOC_ON_ID')) ? true : false));
                },
                function (aIndex) {
                    //block channel settings
                    _this._pinSettingValue = hx.svc.PARENTAL_CTRL.setChannelsetupPin(!!aIndex);
                    _this._pinSettingValue = !!aIndex;
                }
            ];
            this.isUKDTT = ENV.op === ENV.listOP.UKDTT;
        }
        CPgParental.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            hx.logadd('CPgParental');
            hx.log('error', 'page_parental_control create');
            _super.prototype.create.call(this);
            this.setDataCutted(this.productData.general_settings.parental_control);
            this._dataHandler = this.getHandlerSelectionMenu();
            if (ENV.op === ENV.listOP.UKDTT) {
                this._contextListMap = this.productData.general_settings.parental_control;
            }
            else if (hx.config.settings.parentalcontrol) {
                this._contextListMap = hx.config.settings.parentalcontrol;
            }
            else {
                this._contextListMap = [['LOC_MATURITY_RATING_ID', 'LOC_CHANGE_PASSWORD_ID', 'LOC_BLOCK_SETTINGS_CHANNEL_ID'], ['LOC_VIEWALL_ID', 'LOC_7_ID', 'LOC_12_ID', 'LOC_15_ID', 'LOC_18_ID', 'LOC_LOCKALL_ID'], ['LOC_OFF_ID', 'LOC_ON_ID']];
            }
            if (ENV.op === ENV.listOP.AUSDTT) {
                this._ratingDB = [hx.l('LOC_NO_BLOCK_ID'), "", "", "", "", "", hx.l('LOC_BLOCK_G_ID'), "", hx.l('LOC_BLOCK_PG_ID'), "", hx.l('LOC_BLOCK_M_ID'), "", hx.l('LOC_BLOCK_MA_ID'), "", hx.l('LOC_BLOCK_AV_ID'), hx.l('LOC_BLOCK_R_ID'), hx.l('LOC_BLOCK_ALL_ID')];
            }
            else if (ENV.op === ENV.listOP.UKDTT) {
                this._ratingDB = [hx.l('LOC_OFF_ID'), hx.l('LOC_ALL_GUIDANCE_MARKED_CONTENT_ID'), hx.l('LOC_INTERNET_CONTENT_ONLY_ID'), "", "", "", "", "", "", "", "", "", "", "", "", "", hx.l('LOC_ALL_CONTENT_ID')];
            }
            else {
                this._ratingDB = [hx.l('LOC_VIEWALL_ID'), "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", hx.l('LOC_LOCKALL_ID')];
            }
            this._$menu = $('<article>', {
                'class': 'menu'
            });
            this._$contextMenu = $('<div>', {
                'class': 'dep2'
            });
            var html = hx.l('LOC_BLOCK_ACCESS_CHANNEL_ID');
            if (ENV.op === ENV.listOP.UKDTT) {
                this._parentalMainMenu = fnParentalMainMenu(this._dataHandler.getItemTitles(), 'dep1');
            }
            else if (ENV.op !== ENV.listOP.SES) {
                this._parentalMainMenu = fnParentalMainMenu(this._contextListMap[0], 'dep1');
            }
            else {
                this._parentalMainMenu = fnParentalMainMenu(this._contextListMap[0], 'dep1');
                html = "";
            }
            this._$noti = $('<p>', {
                'class': 'noti',
                'html': html
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_PARENTAL_CONTROL_01_ID');
            }
            if (this.isUKDTT) {
                var titles = this._dataHandler.getItemTitles();
                var i, len = titles.length;
                for (i = 0; i < len; i++) {
                    var $el = this._parentalMainMenu.getItem(i).find('em');
                    var text = this._getEntryStringID(i);
                    if (text) {
                        $el.html(hx.l(text));
                    }
                }
            }
            else {
                this._pinSettingValue = hx.svc.PARENTAL_CTRL.getChannelsetupPin();
                this.__disp_ChannelsetupPin();
                if (ENV.op !== ENV.listOP.SES) {
                    this._maturityRatingValue = hx.svc.PARENTAL_CTRL.getParentalRatingValue();
                    this._maturityRating = this._conv_rValue_to_rStr(this._maturityRatingValue);
                    this.__disp_ParentalRatingValue();
                }
            }
            $('#set').append(this._$menu.append(this._$contextMenu.after(this._parentalMainMenu.$)).after(this._$noti));
            hx.al.compose(this);
            $title.ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            this._$noti.ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                }
            });
            this._$menu.ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    _this.append(_this._parentalMainMenu);
                    _this._parentalMainMenu.on();
                    _this._settedPassword = aPageUrl.query;
                    _this._isChangePageInSetting = false;
                }
            });
            // event listener
            hx.svc.CAS_CI.addEventCb('SmartcardStateChanged', this, function (aParam) {
                _this._on_SmartcardStateChanged(aParam);
            });
        };
        CPgParental.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._currentSubMenu = 0;
            this._pinSetting = '';
            this._maturityRating = '';
            this._settedPassword = '';
            this._retryCnt = 0;
            this._$menu.remove();
            this._$noti.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this._$menu = undefined;
            this._$contextMenu = undefined;
            this._parentalMainMenu = undefined;
            this._$noti = undefined;
        };
        /* EN 300 468 - Parental rating descriptor (value = rating + 3 years) */
        CPgParental.prototype._conv_rValue_to_rStr = function (aValue) {
            if (aValue < 16) {
                return this._ratingDB[aValue];
            }
            return this._ratingDB[16];
        };
        CPgParental.prototype._conv_rStr_to_rValue = function (aString) {
            var rValue;
            if (ENV.op === ENV.listOP.AUSDTT) {
                switch (aString) {
                    case hx.l('LOC_NO_BLOCK_ID'):
                        rValue = 0;
                        break;
                    case hx.l('LOC_BLOCK_ALL_ID'):
                        rValue = 127;
                        break;
                    case hx.l('LOC_BLOCK_G_ID'):
                        rValue = 6;
                        break;
                    case hx.l('LOC_BLOCK_PG_ID'):
                        rValue = 8;
                        break;
                    case hx.l('LOC_BLOCK_M_ID'):
                        rValue = 10;
                        break;
                    case hx.l('LOC_BLOCK_MA_ID'):
                        rValue = 12;
                        break;
                    case hx.l('LOC_BLOCK_AV_ID'):
                        rValue = 14;
                        break;
                    case hx.l('LOC_BLOCK_R_ID'):
                        rValue = 15;
                        break;
                }
            }
            else if (ENV.op === ENV.listOP.UKDTT) {
                switch (aString) {
                    case hx.l('LOC_OFF_ID'):
                        rValue = 0;
                        break;
                    case hx.l('LOC_ALL_CONTENT_ID'):
                        rValue = 127;
                        break;
                    case hx.l('LOC_ALL_GUIDANCE_MARKED_CONTENT_ID'):
                        rValue = 1;
                        break;
                    case hx.l('LOC_INTERNET_CONTENT_ONLY_ID'):
                        rValue = 2;
                        break;
                    default:
                        rValue = 0;
                        break;
                }
            }
            else {
                switch (aString) {
                    case hx.l('LOC_VIEWALL_ID'):
                        rValue = 0;
                        break;
                    case hx.l('LOC_LOCKALL_ID'):
                        rValue = 127;
                        break;
                    default:
                        rValue = Number(aString) - 3;
                        break;
                }
            }
            return rValue;
        };
        CPgParental.prototype.__disp_ChannelsetupPin = function () {
            var $pinSettingSub = this._parentalMainMenu.getItem(2).find('em');
            if (this._pinSettingValue === false) {
                this._pinSetting = hx.l('LOC_OFF_ID');
            }
            else if (this._pinSettingValue === true) {
                this._pinSetting = hx.l('LOC_ON_ID');
            }
            $pinSettingSub.html(this._pinSetting);
        };
        CPgParental.prototype.__disp_ParentalRatingValue = function () {
            var $ParentalValueSub = this._parentalMainMenu.getItem(0).find('em');
            $ParentalValueSub.html(this._conv_rValue_to_rStr(this._maturityRatingValue));
        };
        CPgParental.prototype._on_alClicked = function (aParam) {
            var _this = this;
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._parentalMainMenu:
                    bConsumed = true;
                    if (data.index === this.TMenuList.CHANGEPIN) {
                        this._pinChange();
                    }
                    else if (((data.index === this.TMenuList.MATURITYRATING || data.index === this.TMenuList.SETTINGPIN) && ENV.op !== ENV.listOP.SES) || (data.index === this.TSESMenuList.SETTINGPIN && ENV.op === ENV.listOP.SES)) {
                        this._parentalMainMenu.off();
                        this._makesubList(data.index);
                    }
                    else if (data.index === this.TSESMenuList.CHANGESMARTCARDPINCODE && ENV.op === ENV.listOP.SES) {
                        this._processChangeSCPincode();
                    }
                    this._currentSubMenu = data.index;
                    break;
                case this._subList:
                    bConsumed = true;
                    this._subList.off();
                    this._$contextMenu.ani({
                        style: this.aniConfig.dep2.fadeOut,
                        trans: { dur: 150, delay: 75 },
                        cb: function () {
                            if (ENV.op === ENV.listOP.SES) {
                                if (_this._currentSubMenu === _this.TSESMenuList.SETTINGPIN) {
                                    _this._setPinSetting(data.index);
                                }
                            }
                            else if (ENV.op === ENV.listOP.UKDTT) {
                                var selectedMenuIndex = _this._currentSubMenu;
                                _this._setSubListFocusedIndex(selectedMenuIndex, data.index);
                                var $el = _this._parentalMainMenu.getItem(selectedMenuIndex).find('em');
                                var text = _this._getEntryStringID(selectedMenuIndex);
                                $el.html(hx.l(text));
                            }
                            else {
                                if (_this._currentSubMenu === _this.TMenuList.MATURITYRATING) {
                                    _this._setMaturityRating(data.index);
                                }
                                else if (_this._currentSubMenu === _this.TMenuList.SETTINGPIN) {
                                    _this._setPinSetting(data.index);
                                }
                            }
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        CPgParental.prototype._on_alExit = function (aParam) {
            var _this = this;
            var bConsumed = false;
            if (aParam.alData) {
                if (aParam.alData.me === this._parentalMainMenu) {
                    bConsumed = true;
                    this._isChangePageInSetting = true;
                    hx.al.decompose(this);
                    this._$noti.ani({
                        style: this.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        }
                    });
                    this._$menu.ani({
                        style: this.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            _this.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: _this
                            });
                        }
                    });
                }
            }
            return bConsumed;
        };
        CPgParental.prototype._on_alKey = function (aParam) {
            var _this = this;
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        bConsumed = true;
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: this
                        });
                    }
                    break;
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me === this._subList) {
                            this._subList.off();
                            this._$contextMenu.ani({
                                style: this.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    var menuFocusIndex = _this._currentSubMenu;
                                    _this._subList.$.remove();
                                    _this._$contextMenu.removeClass('active');
                                    _this._parentalMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    _this._parentalMainMenu.setItemFocus(menuFocusIndex);
                                    _this._parentalMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        CPgParental.prototype._on_alChangeFocus = function (aParam) {
            if (aParam.alData.index === 0 && aParam.alData.me === this._parentalMainMenu && ENV.op !== ENV.listOP.SES) {
                this._$noti.html(hx.l('LOC_BLOCK_ACCESS_CHANNEL_ID'));
            }
            else if (aParam.alData.index === 0 && aParam.alData.me === this._parentalMainMenu && ENV.op === ENV.listOP.SES) {
                this._$noti.html(hx.l(''));
            }
            else if (aParam.alData.index === 1 && aParam.alData.me === this._parentalMainMenu) {
                this._$noti.html('');
            }
            else if (aParam.alData.index === 2 && aParam.alData.me === this._parentalMainMenu) {
                this._$noti.html(hx.l('LOC_BLOCK_ACCESS_SETTINGS_ID'));
            }
        };
        CPgParental.prototype._on_CasNaMenuUiEvent = function (aParam) {
            var casNaMenuData = aParam.data.uidata.listitem;
            if (casNaMenuData) {
                var i;
                for (i = 0; i < casNaMenuData.length; i += 1) {
                    var strData = casNaMenuData[i];
                    hx.log('CPgParental', hx.util.format('[_on_CasNaMenuUiEvent] strData[{0}].string = {1}', i, strData.string));
                }
            }
            else {
                hx.log('CPgParental', hx.util.format('[_on_CasNaMenuUiEvent] casNaMenuData is invalid'));
            }
        };
        CPgParental.prototype._processChangeSCPincode = function () {
            var dsrvParentalCtrl = hx.svc.PARENTAL_CTRL;
            var bSCInserted = dsrvParentalCtrl.isNagraSmartCardInserted();
            if (bSCInserted) {
                var retryCnt = dsrvParentalCtrl.getNargaSmartCardPINRetryCount();
                this.__makeVerifyPincodeMsg(retryCnt);
            }
            else {
                this._parentalMainMenu.setItemFocus(0);
                this._parentalMainMenu.on();
            }
        };
        CPgParental.prototype.__makeVerifyPincodeMsg = function (aRetryCnt) {
            var _this = this;
            var dsrvParentalCtrl = hx.svc.PARENTAL_CTRL;
            var isPinBlocked = function (cnt) {
                return (cnt === dsrvParentalCtrl.TNaScPinState.EBLOCKED) || false;
            };
            var isAccessible = function (cnt) {
                return (cnt === dsrvParentalCtrl.TNaScPinState.ECORRECT) || false;
            };
            var getStringByRetryCnt = function (cnt) {
                var str = '';
                switch (cnt) {
                    case 0:
                        str = hx.l('LOC_NA_CAS_MSG_1_ID');
                        break;
                    case 1:
                        str = hx.l('LOC_NA_CAS_MSG_2_ID');
                        break;
                    case 2:
                        str = hx.l('LOC_NA_CAS_MSG_3_ID');
                        break;
                    case 3:
                        str = hx.l('LOC_NA_CAS_MSG_4_ID');
                        break;
                    default:
                        break;
                }
                hx.log('CPgParental', hx.util.format('[__makeVerifyPincodeMsg] getStringByRetryCnt - retryCnt : {0} / string : {1}', aRetryCnt, str));
                return str;
            };
            var string = getStringByRetryCnt(aRetryCnt);
            if (isPinBlocked(aRetryCnt)) {
                var remainTime = dsrvParentalCtrl.getNargaSmartCardPINRemainTime();
                hx.log('CPgParental', hx.util.format('[__makeVerifyPincodeMsg] remainTime - remainTime : {0}', remainTime));
                var ok = hx.l('LOC_OK_ID');
                this._confirmMsg = hx.msg('confirm', {
                    //'text' : message,
                    'html': hx.util.format(string, Math.ceil(remainTime / 60)),
                    'auto_close': true,
                    'timeout': remainTime * 1000,
                    'btn_title_arr': [ok],
                    'dia_class': 'osd',
                    'callback_fn': function (_status) {
                        if (_status === ok || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                            _this._parentalMainMenu.getItem(0).removeClass('his');
                            _this._parentalMainMenu.setItemFocus(0);
                            _this._parentalMainMenu.on();
                            hx.msg.close('confirm', _this._confirmMsg);
                            _this._confirmMsg = null;
                        }
                    }
                });
            }
            else {
                this._pinMsg = hx.msg('pin', {
                    html: string,
                    auto_close: false,
                    dia_class: 'osd',
                    enable: true,
                    callback_fn: function (_status, _input) {
                        if (_status === 'stop') {
                            hx.msg.close('pin', _this._pinMsg);
                            _this._pinMsg = null;
                            var retCnt = dsrvParentalCtrl.verifyNargaSmartCardPIN(_input);
                            if (isAccessible(retCnt)) {
                                _this.__makeChangePincodeMsg(_input);
                            }
                            else {
                                _this.__makeVerifyPincodeMsg(retCnt);
                            }
                        }
                        else if (_status === 'key_back') {
                            hx.msg.close('pin', _this._pinMsg);
                            _this._pinMsg = null;
                            _this._parentalMainMenu.on();
                            _this._parentalMainMenu.setItemFocus(0);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('pin', _this._pinMsg);
                            _this._pinMsg = null;
                            _this._isChangePageInSetting = true;
                            hx.al.decompose(_this);
                            _this._$noti.ani({
                                style: _this.aniConfig.menu.changeOut3,
                                trans: {
                                    dur: 200,
                                    delay: 100
                                }
                            });
                            _this._$menu.ani({
                                style: _this.aniConfig.menu.changeOut3,
                                trans: {
                                    dur: 200,
                                    delay: 100
                                },
                                cb: function () {
                                    _this.sendEventToEntity({
                                        alEvent: 'alChangePage',
                                        target: 'CPgSettings',
                                        me: _this
                                    });
                                }
                            });
                        }
                    }
                });
            }
        };
        CPgParental.prototype.__makeChangePincodeMsg = function (aOldPin) {
            var dsrvParentalCtrl = hx.svc.PARENTAL_CTRL;
            // create change Pincode
            this._changeMsg = hx.msg('twopin', {
                text: hx.l('LOC_CHANGE_SMARTCARD_PINCODE_ID'),
                auto_close: false,
                dia_class: 'osd',
                enable: true,
                callback_fn: pinfn
            });
            function pinfn(_status, _input) {
                var _this = this;
                if (_status === 'stop') {
                    var firstInput = _input.substring(0, 4);
                    var secondInput = _input.substring(4, 8);
                    if (firstInput === secondInput) {
                        dsrvParentalCtrl.setNargaSmartCardPIN({
                            oldPin: aOldPin,
                            newPin: secondInput
                        });
                        hx.msg('response', {
                            text: hx.l('LOC_PASSWORD_CHANGED_ID'),
                            auto_close: true,
                            close_time: 2000,
                            dia_class: 'res vaTop'
                        });
                        hx.msg.close('twopin', this._changeMsg);
                        this._changeMsg = null;
                        this._parentalMainMenu.on();
                        this._parentalMainMenu.setItemFocus(0);
                    }
                    else {
                        hx.msg.close('twopin', this._changeMsg);
                        this._changeMsg = null;
                        this.__makeChangePincodeMsg(aOldPin);
                        hx.msg('response', {
                            text: hx.l('LOC_PASSWROD_NOT_MATCH_ID'),
                            auto_close: true,
                            close_time: 2000,
                            dia_class: 'res vaTop'
                        });
                    }
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', this._changeMsg);
                    this._changeMsg = null;
                    this._parentalMainMenu.on();
                    this._parentalMainMenu.setItemFocus(0);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', this._changeMsg);
                    this._changeMsg = null;
                    this._isChangePageInSetting = true;
                    hx.al.decompose(this);
                    this._$noti.ani({
                        style: this.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        }
                    });
                    this._$menu.ani({
                        style: this.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            _this.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: _this
                            });
                        }
                    });
                }
            }
        };
        CPgParental.prototype._on_SmartcardStateChanged = function (aParam) {
            var slot = aParam.slot, state = aParam.state;
            var bClosedMsg = false;
            hx.log('CPgParental', hx.util.format('[_on_SmartcardStateChanged] slot : {0} / state : {1}', slot, state));
            if (state) {
                if (this._confirmMsg) {
                    hx.msg.close('confirm', this._confirmMsg);
                    bClosedMsg = true;
                }
                if (this._pinMsg) {
                    hx.msg.close('pin', this._pinMsg);
                    bClosedMsg = true;
                }
                if (this._changeMsg) {
                    hx.msg.close('twopin', this._changeMsg);
                    bClosedMsg = true;
                }
                if (bClosedMsg) {
                    this._parentalMainMenu.setItemFocus(0);
                    this._parentalMainMenu.on();
                }
            }
        };
        CPgParental.prototype._makesubList = function (aParam) {
            var that = this, index, subFocusIndex, topPx;
            if (aParam === this.TMenuList.MATURITYRATING && ENV.op !== ENV.listOP.SES) {
                index = 1;
                if (ENV.op === ENV.listOP.AUSDTT) {
                    switch (this._maturityRating) {
                        case hx.l('LOC_NO_BLOCK_ID'):
                            subFocusIndex = 0;
                            break;
                        case hx.l('LOC_BLOCK_G_ID'):
                            subFocusIndex = 1;
                            break;
                        case hx.l('LOC_BLOCK_PG_ID'):
                            subFocusIndex = 2;
                            break;
                        case hx.l('LOC_BLOCK_M_ID'):
                            subFocusIndex = 3;
                            break;
                        case hx.l('LOC_BLOCK_MA_ID'):
                            subFocusIndex = 4;
                            break;
                        case hx.l('LOC_BLOCK_AV_ID'):
                            subFocusIndex = 5;
                            break;
                        case hx.l('LOC_BLOCK_R_ID'):
                            subFocusIndex = 6;
                            break;
                        case hx.l('LOC_BLOCK_ALL_ID'):
                            subFocusIndex = 7;
                            break;
                        default:
                            subFocusIndex = 0;
                            break;
                    }
                }
                else if (ENV.op === ENV.listOP.UKDTT) {
                    switch (this._maturityRating) {
                        case hx.l('LOC_ALL_CONTENT_ID'):
                            subFocusIndex = 0;
                            break;
                        case hx.l('LOC_ALL_GUIDANCE_MARKED_CONTENT_ID'):
                            subFocusIndex = 1;
                            break;
                        case hx.l('LOC_INTERNET_CONTENT_ONLY_ID'):
                            subFocusIndex = 2;
                            break;
                        case hx.l('LOC_OFF_ID'):
                            subFocusIndex = 3;
                            break;
                        default:
                            subFocusIndex = 0;
                            break;
                    }
                }
                else {
                    switch (this._maturityRating) {
                        case hx.l('LOC_VIEWALL_ID'):
                            subFocusIndex = 0;
                            break;
                        case '7':
                            subFocusIndex = 1;
                            break;
                        case '12':
                            subFocusIndex = 2;
                            break;
                        case '15':
                            subFocusIndex = 3;
                            break;
                        case '18':
                            subFocusIndex = 4;
                            break;
                        case hx.l('LOC_LOCKALL_ID'):
                            subFocusIndex = 5;
                            break;
                        default:
                            subFocusIndex = 0;
                            break;
                    }
                }
            }
            else if ((aParam === this.TMenuList.SETTINGPIN) || (aParam === this.TSESMenuList.SETTINGPIN && ENV.op === ENV.listOP.SES)) {
                index = 2;
                switch (this._pinSetting) {
                    case hx.l('LOC_OFF_ID'):
                        subFocusIndex = 0;
                        break;
                    case hx.l('LOC_ON_ID'):
                        subFocusIndex = 1;
                        break;
                    default:
                        subFocusIndex = 0;
                        break;
                }
                topPx = '185px';
            }
            var dataHandlers = this._dataHandler;
            var dataList = this._contextListMap[index];
            if (ENV.op === ENV.listOP.UKDTT) {
                dataList = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[aParam]);
                subFocusIndex = this._getSubListFocusedIndex(aParam, dataHandlers.getItemOptions(dataHandlers.getItemTitles()[aParam]));
            }
            this._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length,
                top: topPx
            });
            this._$contextMenu.addClass('active');
            this._parentalMainMenu.getItem(aParam).addClass('his');
            this._$contextMenu.append(this._subList.$);
            this._subList.bind(dataList);
            this._subList.draw();
            this._$contextMenu.ani({
                style: that.aniConfig.dep2.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.append(that._subList);
                    that._subList.on();
                }
            });
        };
        CPgParental.prototype._pinChange = function () {
            var that = this;
            var ptemp = hx.msg('twopin', {
                text: hx.l('LOC_CHANGE_YOUR_PASSWORD_ID') + '.',
                auto_close: false,
                dia_class: 'osd',
                enable: true,
                callback_fn: pinfn
            });
            function pinfn(_status, _input) {
                if (_status === 'stop') {
                    var result = hx.svc.PARENTAL_CTRL.compareTwoPassword({
                        firstInputtedPassword: _input.substring(0, 4),
                        secondInputtedPassword: _input.substring(4, 8),
                        settedPassword: that._settedPassword
                    });
                    hx.msg.close('twopin', ptemp);
                    that._parentalMainMenu.on();
                    that._parentalMainMenu.setItemFocus(1);
                    if (result === true) {
                        that._settedPassword = _input.substring(0, 4);
                        hx.msg('response', {
                            text: hx.l('LOC_PASSWORD_CHANGED_ID'),
                            auto_close: true,
                            close_time: 2000,
                            dia_class: 'res vaTop'
                        });
                    }
                    else if (result === false) {
                        hx.msg('response', {
                            text: hx.l('LOC_PASSWROD_NOT_MATCH_ID'),
                            auto_close: true,
                            close_time: 2000,
                            dia_class: 'res vaTop'
                        });
                        that._pinChange();
                    }
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', ptemp);
                    that._parentalMainMenu.on();
                    that._parentalMainMenu.setItemFocus(1);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', ptemp);
                    that._isChangePageInSetting = true;
                    hx.al.decompose(that);
                    that._$noti.ani({
                        style: that.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        }
                    });
                    that._$menu.ani({
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
                }
            }
        };
        CPgParental.prototype._setMaturityRating = function (aIndex) {
            var $ratingSub = this._parentalMainMenu.getItem(0).find('em');
            $ratingSub.html(ratingKey);
            var ratingKey = hx.l(this._contextListMap[1][aIndex]);
            this._maturityRating = ratingKey;
            this._maturityRatingValue = this._conv_rStr_to_rValue(this._maturityRating);
            hx.svc.PARENTAL_CTRL.setParentalRatingValue(this._maturityRatingValue);
            //this._maturityRatingValue =
            this._parentalMainMenu.getItem(0).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._parentalMainMenu.on();
        };
        CPgParental.prototype._setPinSetting = function (aIndex) {
            var $pinSettingSub = this._parentalMainMenu.getItem(2).find('em');
            $pinSettingSub.html(pinSettingKey);
            var pinSettingKey = hx.l(this._contextListMap[2][aIndex]);
            this._pinSetting = pinSettingKey;
            //this._maturityRatingValue =
            hx.svc.PARENTAL_CTRL.setChannelsetupPin(((pinSettingKey === hx.l('LOC_ON_ID')) ? true : false));
            this._parentalMainMenu.getItem(2).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._parentalMainMenu.on();
            this._parentalMainMenu.setItemFocus(2);
        };
        CPgParental.prototype._setSubListFocusedIndex = function (aIndexMain, aIndexSub) {
            this.doSetSubListFocusedIndex(aIndexMain, aIndexSub);
            this._handlers[aIndexMain](aIndexSub);
            this._parentalMainMenu.getItem(aIndexMain).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._parentalMainMenu.on();
            this._parentalMainMenu.setItemFocus(aIndexMain);
        };
        CPgParental.prototype._getSubListFocusedIndex = function (aIndex, aArray) {
            return this.doGetSubListFocusedIndex(aIndex, aArray);
        };
        /* COMMENT
         * doXetSubListFocusedIndex functions exist for inherited classes, but now use for UKDTT
         * * bglee@humaxdigital.com, 0:58, 2014. 7. 26.
         */
        CPgParental.prototype._getEntryStringID = function (aIndex) {
            return this.doGetEntryStringID(aIndex);
        };
        CPgParental.prototype.doGetEntryStringID = function (aIndex) {
            var dataHandlers = this._dataHandler;
            return [
                function () {
                    return _eGuidancePolicy[hx.svc.SETTING_UTIL.getGuidancePolicy()];
                },
                function () {
                    return "";
                },
                function () {
                    return dataHandlers.getItemOptions(dataHandlers.getItemTitles()[2])[hx.svc.PARENTAL_CTRL.getChannelsetupPin() ? 1 : 0];
                }
            ][aIndex]();
        };
        CPgParental.prototype.doSetSubListFocusedIndex = function (aIndexMain, aIndexSub) {
            //throw 'do implement in inherited class';
        };
        CPgParental.prototype.doGetSubListFocusedIndex = function (aIndex, aArray) {
            //throw 'do implement in inherited class';
            return [
                function () {
                    return aArray.indexOf(_eGuidancePolicy[hx.svc.SETTING_UTIL.getGuidancePolicy()]);
                },
                function () {
                    return -1;
                },
                function () {
                    return hx.svc.PARENTAL_CTRL.getChannelsetupPin() ? 1 : 0;
                }
            ][aIndex]();
        };
        return CPgParental;
    })(__KERNEL_SETTINGS__);
    return CPgParental;
});
