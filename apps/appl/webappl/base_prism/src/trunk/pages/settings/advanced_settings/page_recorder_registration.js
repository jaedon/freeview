var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    var cPgRecorderRegistration = (function (_super) {
        __extends(cPgRecorderRegistration, _super);
        function cPgRecorderRegistration() {
            _super.call(this);
            this.TMenuList = {
                REGISTRATIONCODE: 0
            };
            this.dsrvTVPortal = hx.svc.TV_PORTAL;
            this._isChangePageInSetting = false;
            this._contextListMap = [
                ['LOC_RETRIEVE_REGICODE_ID'],
                ['', '', '', '']
            ];
        }
        cPgRecorderRegistration.prototype.create = function () {
            _super.prototype.create.call(this);
            var that = this;
            $.extend(this, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _remoteRecordMainMenu: hx.al.CPgRecorderRegistration.remoteRecordMainMenu(that._contextListMap[0]),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': 'To use Live TV app or remote recording from other devices, you need to register your product in the website. Follow the instructions below:<br><br>1. Agree to the Privacy Policy first to register your product.<br>2. Go to http://www.myhumax.net and create an account and register your receiver.<br>3. Press OK to retrieve the registration code.<br>3. Enter the code in the website.'
                })
            });
            that._$noti.addClass('smallFont');
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_RECORDER_REGISTRATION_UPPER_ID');
            }
            $('#set').append(that._$menu.append(that._remoteRecordMainMenu.$).after(that._$noti));
            hx.al.compose(that);
            $title.ani({
                style: that.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            that._$noti.ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                }
            });
            that._$menu.ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    that.append(that._remoteRecordMainMenu);
                    that._remoteRecordMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cPgRecorderRegistration.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
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
            this._remoteRecordMainMenu = undefined;
            this._$noti = undefined;
        };
        cPgRecorderRegistration.prototype._on_alClicked = function (aParam) {
            var that = this;
            var data = aParam.alData, target = data.me, bConsumed = false;
            var tmp = null, netConnectedInfo = null;
            switch (target) {
                case that._remoteRecordMainMenu:
                    bConsumed = true;
                    if (data.index === that.TMenuList.REGISTRATIONCODE) {
                        netConnectedInfo = hx.svc.SETTING_UTIL.getNetConnectedInfo();
                        if (!netConnectedInfo) {
                            break;
                        }
                        if (netConnectedInfo.wireDevState === 'Connected' || netConnectedInfo.wirelessDevState === 'Connected') {
                            that.dsrvTVPortal.init({
                                fn: that._tvPortalInitCb.bind(that)
                            });
                        }
                        else {
                            tmp = hx.msg('confirm', {
                                text: hx.l('LOC_CONNECTION_HAS_FAILED_ID'),
                                auto_close: false,
                                btn_title_arr: [hx.l('LOC_OK_ID')],
                                callback_fn: fn,
                                dia_class: 'osd'
                            });
                            function fn(_status) {
                                if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                                    that._remoteRecordMainMenu.on();
                                    that._remoteRecordMainMenu.setItemFocus(that.TMenuList.REGISTRATIONCODE);
                                }
                                else if (_status === 'key_esc') {
                                    hx.svc.SETTING_UTIL.setFrontPanelString({
                                        text: ''
                                    });
                                    that.sendEventToEntity({
                                        alEvent: 'alChangePage',
                                        target: 'CPgLiveController',
                                        me: that
                                    });
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cPgRecorderRegistration.prototype._tvPortalInitCb = function (aResult) {
            var that = this, tmp;
            if (aResult) {
                that.dsrvTVPortal.getRegistrationCode({
                    fn: that._getRegiCodeCb.bind(that)
                });
            }
            else {
                tmp = hx.msg('confirm', {
                    text: hx.l('LOC_CONNECTION_HAS_FAILED_ID'),
                    auto_close: false,
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: fn,
                    dia_class: 'osd'
                });
                function fn(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        that._remoteRecordMainMenu.on();
                        that._remoteRecordMainMenu.setItemFocus(3);
                    }
                    else if (_status === 'key_esc') {
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    }
                }
            }
        };
        cPgRecorderRegistration.prototype._getRegiCodeCb = function (aParam) {
            var that = this, tmp;
            if (aParam.status) {
                if (aParam.regcode) {
                    that._registrationCode = aParam.regcode;
                    tmp = hx.msg('codeConfirm', {
                        first_line: hx.l('LOC_REGISTRATION_CODE_ID') + ' : ',
                        regcode: aParam.regcode,
                        showRegi: true,
                        nolinejump: true,
                        second_line: hx.l('LOC_CODE_VALID_THREE_ID'),
                        auto_close: false,
                        btn_title_arr: [hx.l('LOC_OK_ID')],
                        callback_fn: ff,
                        dia_class: 'osd'
                    });
                    function ff(_status) {
                        if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                            that._remoteRecordMainMenu.on();
                            that._remoteRecordMainMenu.setItemFocus(3);
                        }
                        else if (_status === 'key_esc') {
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text: ''
                            });
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgLiveController',
                                me: that
                            });
                        }
                    }
                }
                else {
                    tmp = hx.msg('confirm', {
                        text: hx.l('LOC_CONNECTION_HAS_FAILED_ID'),
                        auto_close: false,
                        btn_title_arr: [hx.l('LOC_OK_ID')],
                        callback_fn: fn,
                        dia_class: 'osd'
                    });
                    function fn(_status) {
                        if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                            that._remoteRecordMainMenu.on();
                            that._remoteRecordMainMenu.setItemFocus(3);
                        }
                        else if (_status === 'key_esc') {
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text: ''
                            });
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgLiveController',
                                me: that
                            });
                        }
                    }
                }
            }
            else {
                tmp = hx.msg('confirm', {
                    text: hx.l('LOC_CONNECTION_HAS_FAILED_ID'),
                    auto_close: false,
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: fn1,
                    dia_class: 'osd'
                });
                function fn1(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        that._remoteRecordMainMenu.on();
                        that._remoteRecordMainMenu.setItemFocus(3);
                    }
                    else if (_status === 'key_esc') {
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    }
                }
            }
        };
        cPgRecorderRegistration.prototype._on_alChangeFocus = function (aParam) {
            var that = this;
            if (aParam.alData.index === that.TMenuList.REGISTRATIONCODE && aParam.alData.me === this._remoteRecordMainMenu) {
                this._$noti.html(hx.l('LOC_REMOTE_RECORDING_ALLOWS_ID'));
            }
        };
        cPgRecorderRegistration.prototype._on_alExit = function (aParam) {
            var bConsumed = false;
            if (aParam.alData.me === this._remoteRecordMainMenu) {
                bConsumed = true;
                this._gotoSettingPage();
            }
            return bConsumed;
        };
        cPgRecorderRegistration.prototype._on_alKey = function (aParam) {
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
            }
            return bConsumed;
        };
        cPgRecorderRegistration.prototype._gotoSettingPage = function () {
            var that = this;
            that._isChangePageInSetting = true;
            hx.al.decompose(that);
            that._$noti.ani({
                style: that.aniConfig.menu.changeOut3,
                trans: { dur: 200, delay: 100 }
            });
            that._$menu.ani({
                style: that.aniConfig.menu.changeOut3,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgSettings',
                        me: that
                    });
                }
            });
        };
        return cPgRecorderRegistration;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgRecorderRegistration = cPgRecorderRegistration;
    hx.al.CPgRecorderRegistration.remoteRecordMainMenu = function (aParam) {
        return hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep1'
            },
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            data: aParam,
            count: aParam.length,
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
    };
    hx.al.CPgRecorderRegistration.prototype.aniConfig = {
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
    return cPgRecorderRegistration;
});
