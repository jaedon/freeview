// file name : pages/settings/system_settings/power_management.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class cPgPowerManagement extends __KERNEL_SETTINGS__ {
    TMenuItemValue_OffOn = {
        VALUE_OFF : 0,
        VALUE_ON : 1
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
    };
    _autoPowerDown: boolean = true;
    _powerSaving: number = 1;
    _isChangePageInSetting:boolean = false;
    _contextListMap: any;
    _$menu: JQuery;
    _$contextMenu: JQuery;
    _powerManagementMainMenu: any;
    _$noti: JQuery;
    _subList: any;
    _currentSubMenu: number;
    _hdmiCec : boolean = true;

    constructor() {
        super();
    }
    create() {
        super.create();
        if (hx.config.settings.powermanager) {
            this._contextListMap = hx.config.settings.powermanager;
        } else {
            this._contextListMap = {
                title : ['LOC_POWER_MANAGEMENT_ID'],
                itemId : ['autopowerdown', 'powersavingstandby'],
                itemStr : ['LOC_AUTOPOWERDOWN_ID', 'LOC_POWER_SAVING_MODE_MENU_ID'],
                describe : ['', ''],
                itemSub01Str : ['LOC_OFF_ID', 'LOC_ON_ID'],
                itemSub02Str : ['LOC_OFF_ID', 'LOC_ON_ID']
            };
        }
        this._$menu = $('<article>', {
            'class' : 'menu'
        });
        this._$contextMenu = $('<div>', {
            'class' : 'dep2'
        }),
        this._powerManagementMainMenu = hx.al.CPgPowerManagement.powerManagementMainMenu(this._contextListMap.itemStr, this._contextListMap.describe, 'dep1');
        this._$noti = $('<p>', {
            'class' : 'noti',
            'html' : hx.l('LOC_AUTO_POWER_MSG_ID')
        });
        var $title: any = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = hx.l(this._contextListMap.title);
        }

        $('#set').append(this._$menu.append(this._$contextMenu.after(this._powerManagementMainMenu.$)).after(this._$noti));

        hx.al.compose(this);

        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this._$noti.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            }
        });
        this._$menu.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : () => {
                this.append(this._powerManagementMainMenu);
                this._powerManagementMainMenu.on();

                this._createItem();
                this._isChangePageInSetting = false;
            }
        });
    }
    destroy() {
        super.destroy();
        this._$menu.remove();
        this._$noti.remove();
        this._autoPowerDown = true;
        this._powerSaving = 1;
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this._$menu = undefined;
        this._$contextMenu = undefined;
        this._powerManagementMainMenu = undefined;
        this._$noti = undefined;
    }
    _createItem() {
        var i = 0, szItemId: string = '';

        for ( i = 0; i < this._contextListMap.itemId.length; i+=1) {
            szItemId = this._conv_index_to_itemID(i);

            if (szItemId === 'autopowerdown') {
                this._display_autoPowerDown(i);
            }
            if (szItemId === 'powersavingstandby') {
                this._display_powerSavingStandby(i);
            }
            if (szItemId === 'hdmicec') {
                this._display_hdmiCec(i);
            }
        }
    }
    _display_autoPowerDown(aParam) {
        var retObjAPD = hx.svc.WEB_STORAGE.getAutoPowerDown();
        if (retObjAPD.isOn === null || retObjAPD.isOn === undefined) {
            if (hx.config.useAutoPowerDown === 0) {
                hx.svc.WEB_STORAGE.setAutoPowerDown({
                        bIsOn : false
                });
            } else {
                hx.svc.WEB_STORAGE.setAutoPowerDown({
                        bIsOn : true
                });
            }
            retObjAPD = hx.svc.WEB_STORAGE.getAutoPowerDown();
        }
        this._autoPowerDown = (retObjAPD.isOn === "true") ? true : false;
        if (this._autoPowerDown) {
            this._powerManagementMainMenu.getItem(aParam).find('em').html(hx.l(this._contextListMap.itemSub01Str[this.TMenuItemValue_OffOn.VALUE_ON]));
        } else {
            this._powerManagementMainMenu.getItem(aParam).find('em').html(hx.l(this._contextListMap.itemSub01Str[this.TMenuItemValue_OffOn.VALUE_OFF]));
        }
    }
    _display_powerSavingStandby(aParam) {
		// normal : 0,    saving : 1
        var retObjPowerMode = hx.svc.SETTING_UTIL.getStandbyPowerMode();
        this._powerSaving = retObjPowerMode;
        if (retObjPowerMode === 0) {//normal
            this._powerManagementMainMenu.getItem(aParam).find('em').html(hx.l(this._contextListMap.itemSub02Str[this.TMenuItemValue_OffOn.VALUE_OFF]));
        } else if (retObjPowerMode === 1) {//saving
            this._powerManagementMainMenu.getItem(aParam).find('em').html(hx.l(this._contextListMap.itemSub02Str[this.TMenuItemValue_OffOn.VALUE_ON]));
        }
        this._isChangePageInSetting = false;
    }
    _display_hdmiCec(param) {
        var retObjHdmiCec = hx.svc.SETTING_UTIL.getHdmiCecMode();
        this._hdmiCec = retObjHdmiCec;
        if (retObjHdmiCec === false) {
            this._powerManagementMainMenu.getItem(param).find('em').html(hx.l(this._contextListMap.itemSub02Str[this.TMenuItemValue_OffOn.VALUE_OFF]));
        } else if (retObjHdmiCec === true) {
            this._powerManagementMainMenu.getItem(param).find('em').html(hx.l(this._contextListMap.itemSub02Str[this.TMenuItemValue_OffOn.VALUE_ON]));
        }
        this._isChangePageInSetting = false;
    }
    _on_alClicked(aParam): boolean {
        var data = aParam.alData, target = data.me, bConsumed: boolean = false;
        var szItemId;

        switch (target) {
            case this._powerManagementMainMenu:
                bConsumed = true;
                this._powerManagementMainMenu.off();
                this._makesubList(data.index);
                this._currentSubMenu = data.index;
                break;
            case this._subList:
                bConsumed = true;
                this._subList.off();
                this._$contextMenu.ani({
                    style : this.aniConfig.dep2.fadeOut,
                    trans : {
                        dur : 150,
                        delay : 75
                    },
                    cb : () => {
                        this._$contextMenu.removeClass('active');
                        szItemId = this._conv_index_to_itemID(this._currentSubMenu);
                        if (szItemId === 'autopowerdown') {
                            this._setAutoPowerDown(data.index);
                        } else if (szItemId === 'powersavingstandby') {
                            this._setPowerSaving(data.index);
                        } else if (szItemId === 'hdmicec') {
                            this._setHdmiCec(data.index);
                        }
                    }
                });
                break;
        }
        return bConsumed;
    }
    _on_alChangeFocus(aParam) {
        if(aParam.alData.index === 0 && aParam.alData.me === this._powerManagementMainMenu) {
            this._$noti.html(hx.l('LOC_AUTO_POWER_MSG_ID'));
        } else if(aParam.alData.index === 1 && aParam.alData.me === this._powerManagementMainMenu) {
            this._$noti.html("Setting power saving mode to On will consume less power in standby (0.5W) but increase system start times. If you wish to pair other devices or use loop-through in standby, set the mode to Off.");
        } else if(aParam.alData.index === 2 && aParam.alData.me === this._powerManagementMainMenu) {
            this._$noti.html(hx.l('LOC_HDMI_CEC_NOTI_ID'));
        }
    }
    _on_alExit(aParam): boolean {
        var bConsumed: boolean = false;
        if (aParam.alData) {
            if (aParam.alData.me === this._powerManagementMainMenu) {
                bConsumed = true;
                this._isChangePageInSetting = true;
                hx.al.decompose(this);
                this._$menu.ani({
                    style : this.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        this.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : this
                        });
                    }
                });
            }
        }
        return bConsumed;
    }
    _on_alKey(aParam): boolean {
        var bConsumed: boolean = false;
        switch (aParam.alKey) {
            case hx.key.KEY_ESCAPE:
                if (aParam.alData) {
                    bConsumed = true;
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                            text : ''
                    });
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : this
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
                        var menuFocusIndex = this._currentSubMenu;
                        this._subList.off();
                        this._$contextMenu.ani({
                            style : this.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : () => {
                                this._subList.$.remove();
                                this._$contextMenu.removeClass('active');
                                this._powerManagementMainMenu.getItem(menuFocusIndex).removeClass('his');
                                this._powerManagementMainMenu.setItemFocus(menuFocusIndex);
                                this._powerManagementMainMenu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _conv_itemID_to_index(aItemId: string): number {
        var i = 0, ulIndex: number = -1;
        for ( i = 0; i < this._contextListMap.itemId.length; i+=1) {
            if (aItemId === this._contextListMap.itemId[i]) {
                ulIndex = i;
                return ulIndex;
            }
        }
        return ulIndex;
    }
    _conv_index_to_itemID(aItemIdx: number): string {
        var i = 0, itemID: string = 'undefined';
        for ( i = 0; i < this._contextListMap.itemId.length; i+=1) {
            if (i === aItemIdx) {
                itemID = this._contextListMap.itemId[i];
                return itemID;
            }
        }
        return itemID;
    }
    _makesubList(aParam) {
        var index, subFocusIndex, dataList, szItemId: string = 'undefined', topPx: string;
        szItemId = this._contextListMap.itemId[aParam];
        if (szItemId === 'autopowerdown') {
            dataList = this._contextListMap.itemSub01Str;
            subFocusIndex = (this._autoPowerDown === true) ? this.TMenuItemValue_OffOn.VALUE_ON : this.TMenuItemValue_OffOn.VALUE_OFF;
        } else if (szItemId === 'powersavingstandby') {
            dataList = this._contextListMap.itemSub02Str;
            subFocusIndex = (this._powerSaving === 0) ? this.TMenuItemValue_OffOn.VALUE_OFF : this.TMenuItemValue_OffOn.VALUE_ON;
        } else if (szItemId === 'hdmicec') {
            dataList = this._contextListMap.itemSub02Str;
            subFocusIndex = (this._hdmiCec === false) ? this.TMenuItemValue_OffOn.VALUE_OFF : this.TMenuItemValue_OffOn.VALUE_ON;
            topPx = '185px';
        }

        this._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length,
            top : topPx
        });
        this._$contextMenu.addClass('active');
        this._powerManagementMainMenu.getItem(aParam).addClass('his');
        this._$contextMenu.append(this._subList.$);
        this._subList.bind(dataList);
        this._subList.draw();
        this._$contextMenu.ani({
            style : this.aniConfig.dep2.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : () => {
                this.append(this._subList);
                this._subList.on();
            }
        });
    }
    _setAutoPowerDown(aIndex: number) {
        var ulItemIdx: number = this._conv_itemID_to_index('autopowerdown');
        var $autoPowerDownSub = this._powerManagementMainMenu.getItem(ulItemIdx).find('em');
        var autoPowerDownKey: string = hx.l(this._contextListMap.itemSub01Str[aIndex]);
        $autoPowerDownSub.html(autoPowerDownKey);

        this._autoPowerDown = (aIndex === 0) ? false : true;
        hx.svc.WEB_STORAGE.setAutoPowerDown({
                bIsOn : this._autoPowerDown
        });
        this._powerManagementMainMenu.getItem(ulItemIdx).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._powerManagementMainMenu.on();
        this._powerManagementMainMenu.setItemFocus(ulItemIdx);
    }
    _setPowerSaving(aIndex: number) {
        var ulItemIdx:  number = this._conv_itemID_to_index('powersavingstandby');
        var $powerSavingSub = this._powerManagementMainMenu.getItem(ulItemIdx).find('em');
        var powerSavingKey: string = hx.l(this._contextListMap.itemSub02Str[aIndex]);
        $powerSavingSub.html(powerSavingKey);

        this._powerSaving = (aIndex === 0) ? 0 : 1;
         // normal : 0,    saving : 1
        hx.svc.SETTING_UTIL.setStandbyPowerMode(this._powerSaving);
        this._powerManagementMainMenu.getItem(ulItemIdx).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._powerManagementMainMenu.on();
        this._powerManagementMainMenu.setItemFocus(ulItemIdx);
    }
    _setHdmiCec(aIndex: number) {
        var ulItemIdx = this._conv_itemID_to_index('hdmicec');
        var $powerSavingSub = this._powerManagementMainMenu.getItem(ulItemIdx).find('em');
        var powerSavingKey = hx.l(this._contextListMap.itemSub02Str[aIndex]);
        $powerSavingSub.html(powerSavingKey);

        this._hdmiCec = (aIndex === 1) ? true : false;
        hx.svc.SETTING_UTIL.setHdmiCecMode(this._hdmiCec);
        this._powerManagementMainMenu.getItem(ulItemIdx).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._powerManagementMainMenu.on();
        this._powerManagementMainMenu.setItemFocus(ulItemIdx);
    }
}
hx.al.CPgPowerManagement = cPgPowerManagement;
hx.al.CPgPowerManagement.powerManagementMainMenu = function(param, param2, param3) {
    var powerManagementmainmenu = hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : param3
        },
        data : param,
        data2 : param2,
        count : param.length,
        fx : function (i, o) {
            o.html(hx.l(this.data[i])).append($('<em>').html(this.data2[i]));
        },
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
    });
    return powerManagementmainmenu;
};
hx.al.CPgPowerManagement.powerManagementSubMenu = function(param) {
    var powerManagementsubmenu = hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : 'dep2'
        },
        data : param,
        count : param.length,
        fx : function (i, o) {
            o.html(hx.l(this.data[i]));
        },
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
    });
    return powerManagementsubmenu;
};
export = cPgPowerManagement;
