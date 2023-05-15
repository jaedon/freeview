//file name : modules/settings/system_settings/page_hdmi_cec.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class cPgVoiceAssistant extends __KERNEL_SETTINGS__ {
    TMenuList = {
        ALEXA_DEVICE : 0,
    };
    EnableMenuList = {
        ENABLE : 0,
        DISABLE : 1
    };
    _bAlexaEnabled = false;
    _isChangePageInSetting = false;
    _voiceAssistantMainMenu : any;
    _subList : any;
    _confirmMsg : any;
    _currentSubMenuIdx : number;
    _$menu : JQuery;
    _$contextMenu : JQuery;
    _$noti : JQuery;
    create () {
        var that = this;
        $.extend(that, {
            _$menu : $('<article>', {
                'class' : 'menu'
            }),
            _$contextMenu : $('<div>', {
                'class' : 'dep2'
            }),
            _voiceAssistantMainMenu : that.voiceAssistantMainMenu(that._contextListMap.itemStr),
            _$noti : $('<p>', {
                'class' : 'noti',
                'html' : hx.l(that._contextListMap.describe[that.TMenuList.ALEXA_DEVICE])
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
                that.append(that._voiceAssistantMainMenu);
                that._voiceAssistantMainMenu.on();
                that._isChangePageInSetting = false;
            }
        });
    }
    destroy () {
        var that = this;
        super.destroy();
        that._$menu.remove();
        that._$noti.remove();
        if (!that._isChangePageInSetting) {
            $('#set').remove();
        } else {
            that._isChangePageInSetting = false;
        }
        that._$menu = undefined;
        that._$contextMenu = undefined;
        that._voiceAssistantMainMenu = undefined;
        that._bAlexaEnabled = undefined;
        that._isChangePageInSetting = undefined;
        that._$noti = undefined;
    }
    _on_dlMinute (param) {
        var that = this, now = param.alTime, $dateTime = $('#set hgroup .dateTime dd');

        if ($dateTime.eq(0)[0]) {
            $dateTime.eq(0)[0].innerHTML = dateFormat(now, "dd.mm.yyyy");
        }
        if ($dateTime.eq(1)[0]) {
            $dateTime.eq(1)[0].innerHTML = dateFormat(now, "ddd HH:MM").toUpperCase();
        }
    }
    _on_alChangeFocus (param) {
        var that = this;
        var focusIdx = param.alData.index, target = param.alData.me;
        if (target === that._voiceAssistantMainMenu) {
            that._$noti.html(hx.l(that._contextListMap.describe[focusIdx]));
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
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                        text : ''
                    });
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : that
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
                            style : that.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : function() {
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
    }
    _on_alClicked (param) {
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
                    trans: {dur: 150, delay: 75},
                    cb: function() {
                        that._setValueProcess(data);
                    }
                });
                break;
        }
        return bConsumed;
    }
    _on_alExit (param) {
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
    }
    _setValueProcess (data) {
        var that = this;
        var dsrvWebStorage = hx.svc.WEB_STORAGE;
        var dsrvVA = hx.svc.VOICE_ASSISTANCE;
        var subItemIdx = data.index;
        var __backToMain = function() {
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
            } else if (that._bAlexaEnabled === true && subItemIdx !== 0) {
                dsrvVA.removeMQTTClient();
            }
            that._bAlexaEnabled = (subItemIdx === 0) ? true : false;
            dsrvWebStorage.setAlexaDeviceEnable(that._bAlexaEnabled);
        }
        __backToMain();
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
    _contextListMap = (hx.config.settings.hdmicec) ? hx.config.settings.hdmicec : {
        title : ['LOC_VOICE_ASSISTANT_ID'],
        itemId : ['voiceassistant'],
        itemStr : ['LOC_ALEXA_DEVICE_ID'],
        describe : ['LOC_ALEXA_DEVICE_HELP_TEXT_ID'],
        itemSub01Str : ['LOC_ENABLE_ID', 'LOC_DISABLE_ID']
    };
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
    voiceAssistantMainMenu (param) {
        var voiceAssistantMainMenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : 'dep1'
            },
            data : param,
            count : param.length,
            fx : function(i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return voiceAssistantMainMenu;
    }
    _makesubList (param) {
        var that = this, index, subFocusIndex, i;
        var dataList = that._contextListMap.itemSub01Str;
        if (param === that.TMenuList.ALEXA_DEVICE) {
            subFocusIndex = (that._bAlexaEnabled) ? 0 : 1;
        }
        that._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length,
        });
        that._$contextMenu.addClass('active');
        that._voiceAssistantMainMenu.getItem(param).addClass('his');
        that._$contextMenu.append(that._subList.$);
        that._subList.bind(dataList);
        that._subList.draw();
        that._$contextMenu.ani({
            style: that.aniConfig.dep2.fadeIn,
            trans: {dur: 200, delay: 100},
            cb: function() {
                that.append(that._subList);
                that._subList.on();
            }
        });
    }
};
export = cPgVoiceAssistant;
