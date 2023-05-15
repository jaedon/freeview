var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    var cPgVoiceAssistant = (function (_super) {
        __extends(cPgVoiceAssistant, _super);
        function cPgVoiceAssistant() {
            _super.apply(this, arguments);
            this.TMenuList = {
                ALEXA_DEVICE: 0,
            };
            this.EnableMenuList = {
                ENABLE: 0,
                DISABLE: 1
            };
            this._bAlexaEnabled = false;
            this._isChangePageInSetting = false;
            this._contextListMap = (hx.config.settings.hdmicec) ? hx.config.settings.hdmicec : {
                title: ['LOC_VOICE_ASSISTANT_ID'],
                itemId: ['voiceassistant'],
                itemStr: ['LOC_ALEXA_DEVICE_ID'],
                describe: ['LOC_ALEXA_DEVICE_HELP_TEXT_ID'],
                itemSub01Str: ['LOC_ENABLE_ID', 'LOC_DISABLE_ID']
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
        cPgVoiceAssistant.prototype.create = function () {
            var that = this;
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _voiceAssistantMainMenu: that.voiceAssistantMainMenu(that._contextListMap.itemStr),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': hx.l(that._contextListMap.describe[that.TMenuList.ALEXA_DEVICE])
                })
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l(that._contextListMap.title).toUpperCase();
            }
            var dsrvWebStorage = hx.svc.WEB_STORAGE;
            that._bAlexaEnabled = dsrvWebStorage.getAlexaDeviceEnable();
            if (that._bAlexaEnabled === undefined) {
                that._bAlexaEnabled = false;
            }
            that._voiceAssistantMainMenu.getItem(that.TMenuList.ALEXA_DEVICE).find('em').html(hx.l(that._contextListMap.itemSub01Str[((that._bAlexaEnabled === true) ? that.EnableMenuList.ENABLE : that.EnableMenuList.DISABLE)]));
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._voiceAssistantMainMenu.$)).after(that._$noti));
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
                    that.append(that._voiceAssistantMainMenu);
                    that._voiceAssistantMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cPgVoiceAssistant.prototype.destroy = function () {
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
            that._voiceAssistantMainMenu = undefined;
            that._bAlexaEnabled = undefined;
            that._isChangePageInSetting = undefined;
            that._$noti = undefined;
        };
        cPgVoiceAssistant.prototype._on_dlMinute = function (param) {
            var that = this, now = param.alTime, $dateTime = $('#set hgroup .dateTime dd');
            if ($dateTime.eq(0)[0]) {
                $dateTime.eq(0)[0].innerHTML = dateFormat(now, "dd.mm.yyyy");
            }
            if ($dateTime.eq(1)[0]) {
                $dateTime.eq(1)[0].innerHTML = dateFormat(now, "ddd HH:MM").toUpperCase();
            }
        };
        cPgVoiceAssistant.prototype._on_alChangeFocus = function (param) {
            var that = this;
            var focusIdx = param.alData.index, target = param.alData.me;
            if (target === that._voiceAssistantMainMenu) {
                that._$noti.html(hx.l(that._contextListMap.describe[focusIdx]));
            }
        };
        cPgVoiceAssistant.prototype._on_alKey = function (param) {
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
                                    that._voiceAssistantMainMenu.getItem(that._currentSubMenuIdx).removeClass('his');
                                    that._voiceAssistantMainMenu.setItemFocus(that._currentSubMenuIdx);
                                    that._voiceAssistantMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cPgVoiceAssistant.prototype._on_alClicked = function (param) {
            var that = this, data = param.alData, target = data.me, bConsumed = false;
            switch (target) {
                case that._voiceAssistantMainMenu:
                    bConsumed = true;
                    that._voiceAssistantMainMenu.off();
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
        cPgVoiceAssistant.prototype._on_alExit = function (param) {
            var data = param.alData, that = this, bConsumed = false;
            if (data) {
                switch (data.me) {
                    case that._voiceAssistantMainMenu:
                        bConsumed = true;
                        that._gotoSettingPage();
                        break;
                }
            }
            return bConsumed;
        };
        cPgVoiceAssistant.prototype._setValueProcess = function (data) {
            var that = this;
            var dsrvWebStorage = hx.svc.WEB_STORAGE;
            var dsrvVA = hx.svc.VOICE_ASSISTANCE;
            var subItemIdx = data.index;
            var __backToMain = function () {
                that._$contextMenu.removeClass('active');
                var $currentMenu = that._voiceAssistantMainMenu.getItem(that._currentSubMenuIdx);
                $currentMenu.find('em').text(hx.l(that._contextListMap.itemSub01Str[subItemIdx]));
                $currentMenu.removeClass('his');
                that._voiceAssistantMainMenu.setItemFocus(that._currentSubMenuIdx);
                that._voiceAssistantMainMenu.on();
                that._subList.$.remove();
            };
            if (that._currentSubMenuIdx === that.TMenuList.ALEXA_DEVICE) {
                if (that._bAlexaEnabled === false && subItemIdx === 0) {
                    var options = dsrvVA.getMQTTOptions();
                    var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
                    var lanMacAddress = systemInfo.wiredMacAddress;
                    options.userId = lanMacAddress;
                    dsrvVA.createMQTTClient(options);
                    dsrvVA.connectMQTTClient(options);
                }
                else if (that._bAlexaEnabled === true && subItemIdx !== 0) {
                    dsrvVA.removeMQTTClient();
                }
                that._bAlexaEnabled = (subItemIdx === 0) ? true : false;
                dsrvWebStorage.setAlexaDeviceEnable(that._bAlexaEnabled);
            }
            __backToMain();
        };
        cPgVoiceAssistant.prototype._gotoSettingPage = function () {
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
        cPgVoiceAssistant.prototype.voiceAssistantMainMenu = function (param) {
            var voiceAssistantMainMenu = hx.al.component('settingsList', {
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
            return voiceAssistantMainMenu;
        };
        cPgVoiceAssistant.prototype._makesubList = function (param) {
            var that = this, index, subFocusIndex, i;
            var dataList = that._contextListMap.itemSub01Str;
            if (param === that.TMenuList.ALEXA_DEVICE) {
                subFocusIndex = (that._bAlexaEnabled) ? 0 : 1;
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
            that._voiceAssistantMainMenu.getItem(param).addClass('his');
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
        return cPgVoiceAssistant;
    })(__KERNEL_SETTINGS__);
    ;
    return cPgVoiceAssistant;
});
//# sourceMappingURL=page_voice_assistant.js.map