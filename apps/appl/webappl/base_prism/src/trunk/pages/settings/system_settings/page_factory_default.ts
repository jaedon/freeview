//file name : modules/settings/system_settings/page_factory_default.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class cPgFactoryDefault extends __KERNEL_SETTINGS__ {
    TMenuList = {
        AllSettings : 0,
        AllSettingsAndFormatHDD : 1,
    };
    _isChangePageInSetting = false;
    _factoryDefaultMainMenu : any;
    _$menu : JQuery;
    _$contextMenu : JQuery;
    _$noti : JQuery;
    _progressMsg = null;
    _pinMsg = null;
    _settedPassword = null;
    create () {
        var that = this;
        $.extend(that, {
            _$menu : $('<article>', {
                'class' : 'menu'
            }),
            _factoryDefaultMainMenu : that.factoryDefaultMainMenu(that._contextListMap, that._hasIndicator),
            _$noti : $('<p>', {
                'class' : 'noti',
                'html' : hx.l('LOC_NOTI_RESET_ALL_SETTINGS_02_ID')
            })
        });
        var $title = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = hx.l('LOC_FACDEFAULT_ID').toUpperCase();
        }
        $('#set').append(that._$menu.append(that._factoryDefaultMainMenu.$).after(that._$noti));
        hx.al.compose(that);
        hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, (aState, aProgress, aProgressMax, aErrMessage) => {
            that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
        });
        $title.ani({
            style : that.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        that._$noti.ani({
            style : that.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            }
        });
        that._$menu.ani({
            style : that.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : function() {
                that.append(that._factoryDefaultMainMenu);
                that._factoryDefaultMainMenu.on();
                that._isChangePageInSetting = false;
            }
        });
    }
    destroy () {
        var that = this;
        super.destroy();
        hx.svc.STORAGE_MASS.clearEventCb(that);
        that._$menu.remove();
        that._$noti.remove();
        if (!that._isChangePageInSetting) {
            $('#set').remove();
        } else {
            that._isChangePageInSetting = false;
        }
        that._$menu = undefined;
        that._$contextMenu = undefined;
        that._factoryDefaultMainMenu = undefined;
        that._$noti = undefined;
    }
    _on_alChangeFocus (param) {
        var that = this;
        var index = param.alData.index;
        if (param.alData.me === that._factoryDefaultMainMenu) {
            if (index === that.TMenuList.AllSettings) {
                that._$noti.html(hx.l(that._notiMessage[index]));
            } else if (index === that.TMenuList.AllSettingsAndFormatHDD) {
                that._$noti.html(hx.l(that._notiMessage[index]));
            }
        }
    }
    _on_alKey (param) {
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
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : that
                    });
                }
                break;
        }
        return bConsumed;
    }
    _on_alClicked (param) {
        var that = this, data = param.alData, target = data.me, bConsumed = false;
        var index = param.alData.index;
        switch (target) {
            case that._factoryDefaultMainMenu:
                var _onoffCallback = (_status) => {
                    if (_status === hx.l('LOC_YES_ID')) {
                        function _pinfn(_status, _input) {
                            if (_status === 'stop') {
                                that._settedPassword = _input;
                                hx.svc.PARENTAL_CTRL .verifyParentalControlPIN({
                                    password : _input,
                                    cb : function(retObj) {
                                        that._on_dlVerifyPassword(retObj, index);
                                    }
                                });
                            } else if (_status === 'key_back') {
                                hx.msg.close('pin', that._pinMsg);
                                that._pinMsg = null;
                                that._factoryDefaultMainMenu.setItemFocus(index);
                            } else if (_status === 'key_esc') {
                                hx.msg.close('pin', that._pinMsg);
                                that._pinMsg = null;
                                that._gotoSettingPage();
                            }
                        }
                        that._pinMsg = hx.msg('pin', {
                            text : hx.l('LOC_ENTER_PASSWORD_ID'),
                            auto_close : false,
                            dia_class : 'osd',
                            enable : true,
                            callback_fn : _pinfn
                        });
                    } else if (_status === hx.l('LOC_NO_ID') || _status === 'key_back') {
                        that._factoryDefaultMainMenu.setItemFocus(index);
                    } else if (_status === 'key_esc') {
                        that._gotoSettingPage();
                    }
                }
                var confirmMsgStinrg = that._notiMessage[index];
                var text = hx.l(confirmMsgStinrg) + ' ' + hx.l('LOC_CONTINUE_ID') + '?';
                var _offConfirm = hx.msg('confirm', {
                    text : text,
                    auto_close : false,
                    dia_class : 'osd',
                    btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                    callback_fn : _onoffCallback
                });
                _offConfirm.setFocus(1);

                bConsumed = true;
                break;
        }
        return bConsumed;
    }
    _on_alExit (param) {
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
    }
    _on_dlVerifyPassword (param, index) {
        var that = this, ulItemIdx = 0, typeObj, searchType, targetPage, ulFocus;
        var dsrvTVPortal = hx.svc.TV_PORTAL;
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        function _pinfn(_status, _input) {
            if (_status === 'stop') {
                that._settedPassword = _input;
                hx.svc.PARENTAL_CTRL .verifyParentalControlPIN({
                    password : _input,
                    cb : function(retObj) {
                        that._on_dlVerifyPassword(retObj, index);
                    }
                });
            } else if (_status === 'key_back') {
                hx.msg.close('pin', that._pinMsg);
                that._pinMsg = null;
                that._factoryDefaultMainMenu.setItemFocus(index);
            } else if (_status === 'key_esc') {
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
                text : hx.l('LOC_INITIALISING_ID') + '...',
                auto_close : false,
                //timeout : 5000,
                dia_class : 'osd'
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
                    } else {
                        hx.svc.SETTING_UTIL.doFactoryDefault();
                    }
                    break;
            }
        } else if (param.result === 1) {
            hx.msg.close('pin', that._pinMsg);
            that._pinMsg = hx.msg('pin', {
                text : hx.l('LOC_ENTER_PASSWORD_ID'),
                auto_close : false,
                dia_class : 'osd',
                enable : true,
                callback_fn : _pinfn
            });
        }
    }
    _on_FormatProgress (state, progress, progressMax, errMessage) {
        var that = this;

        switch(state) {
            case 0:
                //EFormatProgressNone
                break;
            case 1:
                //EFormatProgressOnGoing
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
                    text : hx.l('LOC_STORAGE_NOT_FORMATTED_ID'),
                    auto_close : false,
                    dia_class : 'osd',
                    btn_title_arr : [hx.l('LOC_OK_ID')],
                    callback_fn : function() {
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
                text : hx.l('LOC_STORAGE_NOT_FORMATTED_ID'),
                auto_close : false,
                dia_class : 'osd',
                btn_title_arr : [hx.l('LOC_OK_ID')],
                callback_fn : function() {
                    // MessageBox will be closed.
                    that._factoryDefaultMainMenu.setItemFocus(that.TMenuList.AllSettingsAndFormatHDD);
                }
            });
        }4
    }
    _gotoSettingPage () {
        var that = this;
        that._isChangePageInSetting = true;
        hx.al.decompose(that);
        that._$noti.ani({
            style: that.aniConfig.menu.changeOut3,
            trans: {dur: 200, delay: 100}
        });
        that._$menu.ani({
            style: that.aniConfig.menu.changeOut3,
            trans: {dur: 200, delay: 100},
            cb : function() {
                that.sendEventToEntity({
                    alEvent : 'alChangePage',
                    target : 'CPgSettings',
                    me : that
                });
            }
        });
    }
    _hasIndicator = false;
    _contextListMap = ['LOC_RESET_ALL_SETTINGS_ID', 'LOC_RESET_ALL_SETTINGS_FORMAT_HDD_ID'];
    _notiMessage = ['LOC_NOTI_RESET_ALL_SETTINGS_02_ID', 'LOC_NOTI_RESET_ALL_SETTINGS_FORMAT_HDD_02_ID'];
    aniConfig = {
        fadeIn : {
            start : {
                'opacity' : 0
            },
            end : {
                'opacity' : 1
            }
        },
        menu : {
            changeIn : {
                start : {
                    'opacity' : 0,
                    'left' : '20px'
                },
                end : {
                    'opacity' : 1,
                    'left' : 0
                }
            },
            changeOut3 : {
                start : {
                    'opacity' : 1,
                    'left' : 0
                },
                end : {
                    'opacity' : 0,
                    'left' : '20px'
                }
            }
        },
        dep2 : {
            fadeIn : {
                start : {
                    'opacity' : 0,
                    'left' : '795px'
                },
                end : {
                    'opacity' : 1,
                    'left' : '775px'
                }
            },
            fadeOut : {
                start : {
                    'opacity' : 1,
                    'left' : '775px'
                },
                end : {
                    'opacity' : 0,
                    'left' : '795px'
                }
            }
        }
    }
    _changeToLastChannel () {
        var that = this;
        // tune to available channel
        var result = hx.svc.AV_CTRL.lastChannel();
        if (result === false) {
            hx.svc.AV_CTRL.availableChannel();
        }
    }
    factoryDefaultMainMenu (param, param2) {
        var factoryDefaultMainMenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : 'dep1'
            },
            data : param,
            count : param.length,
            noInd : param2,
            fx : function(i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return factoryDefaultMainMenu;
    }
};
export = cPgFactoryDefault;
