var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    var cPgFactoryDefault = (function (_super) {
        __extends(cPgFactoryDefault, _super);
        function cPgFactoryDefault() {
            _super.apply(this, arguments);
            this.TMenuList = {
                AllSettings: 0,
                AllSettingsAndFormatHDD: 1,
            };
            this._isChangePageInSetting = false;
            this._progressMsg = null;
            this._pinMsg = null;
            this._settedPassword = null;
            this._hasIndicator = false;
            this._contextListMap = ['LOC_RESET_ALL_SETTINGS_ID', 'LOC_RESET_ALL_SETTINGS_FORMAT_HDD_ID'];
            this._notiMessage = ['LOC_NOTI_RESET_ALL_SETTINGS_02_ID', 'LOC_NOTI_RESET_ALL_SETTINGS_FORMAT_HDD_02_ID'];
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
        }
        cPgFactoryDefault.prototype.create = function () {
            var that = this;
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _factoryDefaultMainMenu: that.factoryDefaultMainMenu(that._contextListMap, that._hasIndicator),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': hx.l('LOC_NOTI_RESET_ALL_SETTINGS_02_ID')
                })
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_FACDEFAULT_ID').toUpperCase();
            }
            $('#set').append(that._$menu.append(that._factoryDefaultMainMenu.$).after(that._$noti));
            hx.al.compose(that);
            hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
            });
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
                    that.append(that._factoryDefaultMainMenu);
                    that._factoryDefaultMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cPgFactoryDefault.prototype.destroy = function () {
            var that = this;
            _super.prototype.destroy.call(this);
            hx.svc.STORAGE_MASS.clearEventCb(that);
            that._$menu.remove();
            that._$noti.remove();
            if (!that._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                that._isChangePageInSetting = false;
            }
            that._$menu = undefined;
            that._$contextMenu = undefined;
            that._factoryDefaultMainMenu = undefined;
            that._$noti = undefined;
        };
        cPgFactoryDefault.prototype._on_alChangeFocus = function (param) {
            var that = this;
            var index = param.alData.index;
            if (param.alData.me === that._factoryDefaultMainMenu) {
                if (index === that.TMenuList.AllSettings) {
                    that._$noti.html(hx.l(that._notiMessage[index]));
                }
                else if (index === that.TMenuList.AllSettingsAndFormatHDD) {
                    that._$noti.html(hx.l(that._notiMessage[index]));
                }
            }
        };
        cPgFactoryDefault.prototype._on_alKey = function (param) {
            var that = this, bConsumed = false;
            switch (param.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (param.alData) {
                        bConsumed = true;
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    }
                    break;
            }
            return bConsumed;
        };
        cPgFactoryDefault.prototype._on_alClicked = function (param) {
            var that = this, data = param.alData, target = data.me, bConsumed = false;
            var index = param.alData.index;
            switch (target) {
                case that._factoryDefaultMainMenu:
                    var _onoffCallback = function (_status) {
                        if (_status === hx.l('LOC_YES_ID')) {
                            function _pinfn(_status, _input) {
                                if (_status === 'stop') {
                                    that._settedPassword = _input;
                                    hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                                        password: _input,
                                        cb: function (retObj) {
                                            that._on_dlVerifyPassword(retObj, index);
                                        }
                                    });
                                }
                                else if (_status === 'key_back') {
                                    hx.msg.close('pin', that._pinMsg);
                                    that._pinMsg = null;
                                    that._factoryDefaultMainMenu.setItemFocus(index);
                                }
                                else if (_status === 'key_esc') {
                                    hx.msg.close('pin', that._pinMsg);
                                    that._pinMsg = null;
                                    that._gotoSettingPage();
                                }
                            }
                            that._pinMsg = hx.msg('pin', {
                                text: hx.l('LOC_ENTER_PASSWORD_ID'),
                                auto_close: false,
                                dia_class: 'osd',
                                enable: true,
                                callback_fn: _pinfn
                            });
                        }
                        else if (_status === hx.l('LOC_NO_ID') || _status === 'key_back') {
                            that._factoryDefaultMainMenu.setItemFocus(index);
                        }
                        else if (_status === 'key_esc') {
                            that._gotoSettingPage();
                        }
                    };
                    var confirmMsgStinrg = that._notiMessage[index];
                    var text = hx.l(confirmMsgStinrg) + ' ' + hx.l('LOC_CONTINUE_ID') + '?';
                    var _offConfirm = hx.msg('confirm', {
                        text: text,
                        auto_close: false,
                        dia_class: 'osd',
                        btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                        callback_fn: _onoffCallback
                    });
                    _offConfirm.setFocus(1);
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        cPgFactoryDefault.prototype._on_alExit = function (param) {
            var data = param.alData, that = this, bConsumed = false;
            if (data) {
                switch (data.me) {
                    case that._factoryDefaultMainMenu:
                        bConsumed = true;
                        that._gotoSettingPage();
                        break;
                }
            }
            return bConsumed;
        };
        cPgFactoryDefault.prototype._on_dlVerifyPassword = function (param, index) {
            var that = this, ulItemIdx = 0, typeObj, searchType, targetPage, ulFocus;
            var dsrvTVPortal = hx.svc.TV_PORTAL;
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            function _pinfn(_status, _input) {
                if (_status === 'stop') {
                    that._settedPassword = _input;
                    hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                        password: _input,
                        cb: function (retObj) {
                            that._on_dlVerifyPassword(retObj, index);
                        }
                    });
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', that._pinMsg);
                    that._pinMsg = null;
                    that._factoryDefaultMainMenu.setItemFocus(index);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', that._pinMsg);
                    that._pinMsg = null;
                    that._gotoSettingPage();
                }
            }
            // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
            if (param.result === 0) {
                hx.msg.close('pin', that._pinMsg);
                that._pinMsg = null;
                that._progressMsg = hx.msg('progress', {
                    text: hx.l('LOC_INITIALISING_ID') + '...',
                    auto_close: false,
                    //timeout : 5000,
                    dia_class: 'osd'
                });
                switch (index) {
                    case that.TMenuList.AllSettings:
                        hx.svc.SETTING_UTIL.doFactoryDefault();
                        break;
                    case that.TMenuList.AllSettingsAndFormatHDD:
                        hx.il.criticalTasking.crHDDTasking = true;
                        var bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing();
                        if (bPbGoing) {
                            hx.svc.PLAYBACK.stop();
                        }
                        hx.svc.AV_CTRL.stopCurChannel({
                            stopAV: 1,
                            stopTsr: 1
                        });
                        // DMR uri set null ('bring in client' do not have to work)
                        hx.svc.DMR.setMedia(null);
                        var pairedStorage = hx.svc.STORAGE_MASS.getPairedStorage();
                        if (pairedStorage) {
                            hx.svc.STORAGE_MASS.doFormat({
                                id: pairedStorage.id
                            });
                        }
                        else {
                            hx.svc.SETTING_UTIL.doFactoryDefault();
                        }
                        break;
                }
            }
            else if (param.result === 1) {
                hx.msg.close('pin', that._pinMsg);
                that._pinMsg = hx.msg('pin', {
                    text: hx.l('LOC_ENTER_PASSWORD_ID'),
                    auto_close: false,
                    dia_class: 'osd',
                    enable: true,
                    callback_fn: _pinfn
                });
            }
        };
        cPgFactoryDefault.prototype._on_FormatProgress = function (state, progress, progressMax, errMessage) {
            var that = this;
            switch (state) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    //EFormatProgressFinished
                    hx.log("CPgFactoryDefault", 'Format : Success');
                    hx.il.criticalTasking.crHDDTasking = false;
                    hx.svc.SETTING_UTIL.doFactoryDefault();
                    break;
                case 3:
                    //EFormatProgressError
                    hx.log("CPgFactoryDefault", 'Format : Fail');
                    hx.msg.close('progress', that._progressMsg);
                    that._changeToLastChannel();
                    hx.il.criticalTasking.crHDDTasking = false;
                    hx.msg('confirm', {
                        text: hx.l('LOC_STORAGE_NOT_FORMATTED_ID'),
                        auto_close: false,
                        dia_class: 'osd',
                        btn_title_arr: [hx.l('LOC_OK_ID')],
                        callback_fn: function () {
                            // MessageBox will be closed.
                            that._factoryDefaultMainMenu.setItemFocus(that.TMenuList.AllSettingsAndFormatHDD);
                        }
                    });
                    break;
            }
            if (errMessage > 0) {
                hx.log("CPgFactoryDefault", 'Format : Fail');
                hx.msg.close('progress', that._progressMsg);
                that._changeToLastChannel();
                hx.il.criticalTasking.crHDDTasking = false;
                hx.msg('confirm', {
                    text: hx.l('LOC_STORAGE_NOT_FORMATTED_ID'),
                    auto_close: false,
                    dia_class: 'osd',
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: function () {
                        // MessageBox will be closed.
                        that._factoryDefaultMainMenu.setItemFocus(that.TMenuList.AllSettingsAndFormatHDD);
                    }
                });
            }
            4;
        };
        cPgFactoryDefault.prototype._gotoSettingPage = function () {
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
        cPgFactoryDefault.prototype._changeToLastChannel = function () {
            var that = this;
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        cPgFactoryDefault.prototype.factoryDefaultMainMenu = function (param, param2) {
            var factoryDefaultMainMenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep1'
                },
                data: param,
                count: param.length,
                noInd: param2,
                fx: function (i, o) {
                    o.html(hx.l(this.data[i])).append($('<em>'));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return factoryDefaultMainMenu;
        };
        return cPgFactoryDefault;
    })(__KERNEL_SETTINGS__);
    ;
    return cPgFactoryDefault;
});
