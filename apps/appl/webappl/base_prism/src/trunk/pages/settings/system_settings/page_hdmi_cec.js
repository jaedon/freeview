var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    var cPgHdmiCec = (function (_super) {
        __extends(cPgHdmiCec, _super);
        function cPgHdmiCec() {
            _super.apply(this, arguments);
            this.TMenuList = {
                TVPWRCTRL: 0,
            };
            this.OnOffMenuList = {
                ON: 0,
                OFF: 1
            };
            this._bPowerCtrl = true;
            this._isChangePageInSetting = false;
            this._contextListMap = (hx.config.settings.hdmicec) ? hx.config.settings.hdmicec : {
                title: ['LOC_HDMI_CEC_ID'],
                itemId: ['powerctl'],
                itemStr: ['LOC_HDMI_CEC_PWR_ID'],
                describe: ['LOC_HDMI_CEC_PWR_DESC_ID'],
                itemSub01Str: ['LOC_ON_ID', 'LOC_OFF_ID']
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
        }
        cPgHdmiCec.prototype.create = function () {
            var that = this;
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _hdmiCecMainMenu: that.hdmiCecMainMenu(that._contextListMap.itemStr),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': hx.l(that._contextListMap.describe[that.TMenuList.TVPWRCTRL])
                })
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l(that._contextListMap.title).toUpperCase();
            }
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            that._bPowerCtrl = dsrvSettingUtil.getHdmiCecMode();
            if (that._bPowerCtrl === undefined) {
                that._bPowerCtrl = false;
            }
            that._hdmiCecMainMenu.getItem(that.TMenuList.TVPWRCTRL).find('em').html(hx.l(that._contextListMap.itemSub01Str[((that._bPowerCtrl === true) ? that.OnOffMenuList.ON : that.OnOffMenuList.OFF)]));
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._hdmiCecMainMenu.$)).after(that._$noti));
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
                    that.append(that._hdmiCecMainMenu);
                    that._hdmiCecMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cPgHdmiCec.prototype.destroy = function () {
            var that = this;
            _super.prototype.destroy.call(this);
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
            that._hdmiCecMainMenu = undefined;
            that._bPowerCtrl = undefined;
            that._isChangePageInSetting = undefined;
            that._$noti = undefined;
        };
        cPgHdmiCec.prototype._on_dlMinute = function (param) {
            var that = this, now = param.alTime, $dateTime = $('#set hgroup .dateTime dd');
            if ($dateTime.eq(0)[0]) {
                $dateTime.eq(0)[0].innerHTML = dateFormat(now, "dd.mm.yyyy");
            }
            if ($dateTime.eq(1)[0]) {
                $dateTime.eq(1)[0].innerHTML = dateFormat(now, "ddd HH:MM").toUpperCase();
            }
        };
        cPgHdmiCec.prototype._on_alChangeFocus = function (param) {
            var that = this;
            var focusIdx = param.alData.index, target = param.alData.me;
            if (target === that._hdmiCecMainMenu) {
                that._$noti.html(hx.l(that._contextListMap.describe[focusIdx]));
            }
        };
        cPgHdmiCec.prototype._on_alKey = function (param) {
            var that = this, bConsumed = false;
            switch (param.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (param.alData) {
                        bConsumed = true;
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    }
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    if (param.alData) {
                        bConsumed = true;
                        if (param.alData.me === that._subList) {
                            that._subList.off();
                            that._$contextMenu.ani({
                                style: that.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    that._subList.$.remove();
                                    that._$contextMenu.removeClass('active');
                                    that._hdmiCecMainMenu.getItem(that._currentSubMenuIdx).removeClass('his');
                                    that._hdmiCecMainMenu.setItemFocus(that._currentSubMenuIdx);
                                    that._hdmiCecMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cPgHdmiCec.prototype._on_alClicked = function (param) {
            var that = this, data = param.alData, target = data.me, bConsumed = false;
            switch (target) {
                case that._hdmiCecMainMenu:
                    bConsumed = true;
                    //that._hdmiCecMainMenu.getItem(data.index).addClass('his');
                    //that._$contextMenu.addClass('active');
                    that._hdmiCecMainMenu.off();
                    that._makesubList(data.index);
                    that._currentSubMenuIdx = data.index;
                    break;
                case that._subList:
                    bConsumed = true;
                    that._subList.off();
                    that._$contextMenu.ani({
                        style: that.aniConfig.dep2.fadeOut,
                        trans: { dur: 150, delay: 75 },
                        cb: function () {
                            that._setValueProcess(data);
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cPgHdmiCec.prototype._on_alExit = function (param) {
            var data = param.alData, that = this, bConsumed = false;
            if (data) {
                switch (data.me) {
                    case that._hdmiCecMainMenu:
                        bConsumed = true;
                        that._gotoSettingPage();
                        break;
                }
            }
            return bConsumed;
        };
        cPgHdmiCec.prototype._setValueProcess = function (data) {
            var that = this;
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var subItemIdx = data.index;
            var __backToMain = function () {
                that._$contextMenu.removeClass('active');
                var $currentMenu = that._hdmiCecMainMenu.getItem(that._currentSubMenuIdx);
                $currentMenu.find('em').text(hx.l(that._contextListMap.itemSub01Str[subItemIdx]));
                $currentMenu.removeClass('his');
                that._hdmiCecMainMenu.setItemFocus(that._currentSubMenuIdx);
                that._hdmiCecMainMenu.on();
                that._subList.$.remove();
            };
            if (that._currentSubMenuIdx === that.TMenuList.TVPWRCTRL) {
                that._bPowerCtrl = (subItemIdx === 0) ? true : false;
                dsrvSettingUtil.setHdmiCecMode(that._bPowerCtrl);
            }
            __backToMain();
        };
        cPgHdmiCec.prototype._gotoSettingPage = function () {
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
        cPgHdmiCec.prototype.hdmiCecMainMenu = function (param) {
            var hdmiCecMainMenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep1'
                },
                data: param,
                count: param.length,
                fx: function (i, o) {
                    o.html(hx.l(this.data[i])).append($('<em>'));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return hdmiCecMainMenu;
        };
        cPgHdmiCec.prototype.hdmiCecSubMenu = function (param) {
        };
        cPgHdmiCec.prototype._makesubList = function (param) {
            var that = this, index, subFocusIndex, i;
            var dataList = that._contextListMap.itemSub01Str;
            if (param === that.TMenuList.TVPWRCTRL) {
                subFocusIndex = (that._bPowerCtrl) ? 0 : 1;
            }
            that._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length,
            });
            that._$contextMenu.addClass('active');
            that._hdmiCecMainMenu.getItem(param).addClass('his');
            that._$contextMenu.append(that._subList.$);
            that._subList.bind(dataList);
            that._subList.draw();
            that._$contextMenu.ani({
                style: that.aniConfig.dep2.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.append(that._subList);
                    that._subList.on();
                }
            });
        };
        return cPgHdmiCec;
    })(__KERNEL_SETTINGS__);
    ;
    return cPgHdmiCec;
});
