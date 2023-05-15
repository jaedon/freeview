// file name : pages/settings/system_settings/power_timer.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/prism.d.ts"/>
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class cPgPowerTimer extends __KERNEL_SETTINGS__ {
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
    onTimerMenuList = {
        CHANNELLIST : 5
    };
    TMenuItemValue_OffSetTimer = {
        VALUE_OFF : 0,
        VALUE_SETTIMER : 1
    };
    TMenuItemValue_OnOff = {
        VALUE_ON : 0,
        VALUE_OFF : 1
    };
    _selectChannelIdx: number = 0;
    _oldSelectChannelIdx: number = 0;
    _wakeupTime: string = '00:00';
    _wakeupVolume: string = '10';
    _wakeupRepeatDays: string = hx.l('LOC_DAILY_ID');
    _powerOffHour: string = null;
    _powerOffMin: string = null;
    _ispowerOnTimerOn: boolean = false;
    _ispowerOffTimerOn: string = 'Off';
    _currentState: string = 'main';
    _svcList = []; //for showList (name + number)
    currentList = []; //for realList
    settedChGrp: string = 'TV';
    _isChangePageInSetting: boolean = false;
    _contextListMap: any;
    _settimerListMap: any;
    _$menu: JQuery;
    _$contextMenu: JQuery;
    _powerTimerMainMenu: any;
    _onTimerMainMenu: any;
    _subList: any;
    _onTimersubList: any;
    _title: any;
    _currentSubMenu: number;
    _currentOnTimerSubMenu: number;
    _$setStge: JQuery;
    _$onTimerMenu: JQuery;
    _$onTimerSubMenu: JQuery;
    _$btnWrap: JQuery;
    _okBtn: any;
    _cancelBtn: any;
    _tvtvUpdateTime: any;
    currentGrp: any;
    _tvList: any;
    _radioList: any;
    _hdtvList: any;
    _fav1List: any;
    _fav2List: any;
    _fav3List: any;
    _fav4List: any;
    _fav5List: any;
    grpList: string[];
    constructor() {
        super();
    }
    create() {
        super.create();
        var that = this;
        if (hx.config.settings.powertimer) {
            this._contextListMap = hx.config.settings.powertimer;
        } else {
            this._contextListMap = {
                title : ['LOC_POWER_TIMER_ID'],
                itemId : ['powerontimer', 'powerofftimer'],
                itemStr : ['LOC_POWERONTIMER_ID', 'LOC_POWEROFFTIMER_ID'],
                describe : ['', ''],
                itemSub01Str : ['LOC_OFF_ID', 'LOC_SETTIMER_ID'],
                itemSub02Str : ['LOC_OFF_ID', 'LOC_SETTIMER_ID']
            };
        }
        if (hx.config.use2ndPowerOnOffTimer !== true) {
            this._settimerListMap = {
                powerOnItemId : ['subpowertimer', 'subtime', 'subchannel', 'subvolume', 'subrepeat'],
                powerOnItem : ['LOC_POWERONTIMER_ID', 'LOC_TIME_ID', 'LOC_CHANNEL_ID', 'LOC_VOLUME_ID', 'LOC_REPEAT_ID'],
                powerOffItemId : ['subpowertimer', 'subtime'],
                powerOffItem : ['LOC_POWEROFFTIMER_ID', 'LOC_TIME_ID'],
                itemSub01Str : ['LOC_ON_ID', 'LOC_OFF_ID'],
                itemSub02Str : ['LOC_1_ID', 'LOC_2_ID', 'LOC_3_ID', 'LOC_4_ID', 'LOC_5_ID', 'LOC_6_ID', 'LOC_7_ID', 'LOC_8_ID', 'LOC_9_ID', 'LOC_10_ID', 'LOC_11_ID', 'LOC_12_ID', 'LOC_13_ID', 'LOC_14_ID', 'LOC_15_ID', 'LOC_16_ID', 'LOC_17_ID', 'LOC_18_ID', 'LOC_19_ID', 'LOC_20_ID'],
                itemSub03Str : ['LOC_ONCE_ID', 'LOC_DAILY_ID', 'LOC_WEEKLY_ID']
            };
        } else {
            this._settimerListMap = {
                powerOnItemId : ['subtime', 'subchannel', 'subvolume', 'subrepeat'],
                powerOnItem : ['LOC_TIME_ID', 'LOC_CHANNEL_ID', 'LOC_VOLUME_ID', 'LOC_REPEAT_ID'],
                powerOffItemId : ['subtime'],
                powerOffItem : ['LOC_TIME_ID'],
                itemSub01Str : ['LOC_ON_ID', 'LOC_OFF_ID'],
                itemSub02Str : ['LOC_1_ID', 'LOC_2_ID', 'LOC_3_ID', 'LOC_4_ID', 'LOC_5_ID', 'LOC_6_ID', 'LOC_7_ID', 'LOC_8_ID', 'LOC_9_ID', 'LOC_10_ID', 'LOC_11_ID', 'LOC_12_ID', 'LOC_13_ID', 'LOC_14_ID', 'LOC_15_ID', 'LOC_16_ID', 'LOC_17_ID', 'LOC_18_ID', 'LOC_19_ID', 'LOC_20_ID'],
                itemSub03Str : ['LOC_ONCE_ID', 'LOC_DAILY_ID', 'LOC_WEEKLY_ID']
            };
        }
        this._$menu = $('<article>', {
            'class' : 'menu'
        });
        this._$contextMenu = $('<div>', {
            'class' : 'dep2'
        });
        this._powerTimerMainMenu = this._makePowerTimerMainMenu(this._contextListMap.itemStr, 'dep1', 'roll');
        /* power on timer menu */
        this._$setStge = $('<section>', {
            'class' : 'setStge'
        });
        this._$onTimerMenu = $('<article>', {
            'class' : 'menu'
        });
        this._$onTimerSubMenu = $('<div>', {
            'class' : 'dep2'
        });
        this._$btnWrap = $('<div>', {
            'class' : 'wrap_btn'
        });
        this._okBtn = hx.al.component('button', {
            attr : {
                'class' : 'button',
                'data-langID' : 'LOC_OK_ID'
            },
            usePointerKey : true
        });
        this._cancelBtn = hx.al.component('button', {
            attr : {
                'class' : 'button',
                'data-langID' : 'LOC_CANCEL_ID'
            },
            usePointerKey : true
        });
        var $title: any = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = hx.l(this._contextListMap.title);
        }
        $('#set').append(this._$menu.append(this._$contextMenu.after(this._powerTimerMainMenu.$)).after(this._$setStge.append(this._$onTimerMenu.append(this._$onTimerSubMenu)).append(this._$btnWrap.append(this._okBtn.$).append(this._cancelBtn.$))));
    
        hx.al.compose(this);
    
        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this._$menu.ani({
            style : that.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : () => {
                that.append(that._powerTimerMainMenu).append(that._okBtn).append(that._cancelBtn);
                that._powerTimerMainMenu.on();

                that.getChlist();

                that._createItem();
                that._isChangePageInSetting = false;
            }
        });
    }
    destroy() {
        super.destroy();
        this._$menu.remove();
        this._$setStge.remove();
        this._selectChannelIdx = 0;
        this._oldSelectChannelIdx = 0;
        this._wakeupTime = '00:00';
        this._wakeupVolume = '10';
        this._wakeupRepeatDays = hx.l('LOC_DAILY_ID');
        this._powerOffHour = null;
        this._powerOffMin = null;
        this._ispowerOnTimerOn = false;
        this._ispowerOffTimerOn = 'Off';
        this._currentState = 'main';
        this._svcList.length = 0;
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this._$menu = undefined;
        this._$contextMenu = undefined;
        this._powerTimerMainMenu = undefined;
        this._$setStge = undefined;
        this._$onTimerMenu = undefined;
        this._$onTimerSubMenu = undefined;
        this._$btnWrap = undefined;
        this._okBtn = undefined;
        this._cancelBtn = undefined;
        this._svcList = [];
        this.currentList = [];
        if (hx.config.useGracenote) {
            this._tvtvUpdateTime = null;
        }
    }
    _createItem() {
        var i = 0, szItemId: string = '';
        for (i = 0; i<this._contextListMap.itemId.length; i+=1) {
            szItemId = this._conv_mainIndex_to_itemID(i);
            if (szItemId === 'powerontimer') {
                this._display_powerontimer(i);
            }
            if (szItemId === 'powerofftimer') {
                this._display_powerofftimer(i);
            }
        }
    }
     _display_powerontimer(aParam) {
        this.getPowerOnInfoData();
    }
    _display_powerofftimer(aParam) {
        var retObjOff = hx.svc.WEB_STORAGE.getPowerOffTimer();
        this._ispowerOffTimerOn = retObjOff.isOn;
        this._powerOffHour = retObjOff.hour;
        this._powerOffMin = retObjOff.min;
        if (this._ispowerOffTimerOn === 'Off' || !this._ispowerOffTimerOn) {
            this._ispowerOffTimerOn = 'Off';
            this._powerTimerMainMenu.getItem(aParam).find('em').html(hx.l(this._contextListMap.itemSub02Str[this.TMenuItemValue_OffSetTimer.VALUE_OFF]));
        } else {
            this._ispowerOffTimerOn = 'On';
            this._powerTimerMainMenu.getItem(aParam).find('em').html(this._powerOffHour + ':' + this._powerOffMin);
        }
    }
    _on_alClicked(aParam): boolean {
        var that = this, data = aParam.alData, target = data.me, bConsumed: boolean = false;
        var szItemId: string = 'undefined', ulItemIdx: number = 0, szSubItemId: string = 'undefined';
        var retObjOff;
        var powerOffTimeStr: string;

        switch (target) {
            case that._powerTimerMainMenu:
                bConsumed = true;
                that._powerTimerMainMenu.off();
                that._makesubList(data.index);
                that._currentSubMenu = data.index;
                break;
            case that._onTimerMainMenu:
                bConsumed = true;
                that._onTimerMainMenu.off();
                szItemId = that._conv_mainIndex_to_itemID(that._currentSubMenu);
                if (szItemId === 'powerontimer') {
                    szSubItemId = that._conv_subIndex_to_itemID('powerontimer', data.index);
                    if (szSubItemId === 'subtime') {
                        that._inputOnTime();
                    } else if (szSubItemId === 'subchannel') {
                        that._makeOnTimersubList(data.index);
                        that._currentOnTimerSubMenu = data.index;
                    } else if (szSubItemId === 'subvolume') {
                        that._makeOnTimersubList(data.index);
                        that._currentOnTimerSubMenu = data.index;
                    } else {
                        that._makeOnTimersubList(data.index);
                        that._currentOnTimerSubMenu = data.index;
                    }
                } else if (szItemId === 'powerofftimer') {
                    szSubItemId = that._conv_subIndex_to_itemID('powerofftimer', data.index);
                    if (szSubItemId === 'subpowertimer') {
                        that._makeOffTimersubList(data.index);
                        that._currentOnTimerSubMenu = data.index;
                    } else if (szSubItemId === 'subtime') {
                        that._inputOffTime();
                    }
                }
                break;
            case that._subList:
                bConsumed = true;
                that._subList.off();
                that._$contextMenu.ani({
                    style: that.aniConfig.dep2.fadeOut,
                    trans: {dur: 150, delay: 75},
                    cb: function() {
                        that._$contextMenu.removeClass('active');
                        szItemId = that._contextListMap.itemId[that._currentSubMenu];
                        if (szItemId === 'powerontimer') {
                            that._setPowerOnTimer(data.index);
                            that._currentState = 'ontimer';
                        } else if (szItemId === 'powerofftimer') {
                            that._setPowerOffTimer(data.index);
                            that._currentState = 'offtimer';
                        }
                    }
                });
                break;
            case that._onTimersubList:
                bConsumed = true;
                that._onTimersubList.off();
                that._$onTimerSubMenu.ani({
                    style: that.aniConfig.dep2.fadeOut,
                    trans: {dur: 150, delay: 75},
                    cb: function() {
                        szItemId = that._conv_mainIndex_to_itemID(that._currentSubMenu);
                        if (szItemId === 'powerontimer') {
                            szSubItemId = that._conv_subIndex_to_itemID('powerontimer', that._currentOnTimerSubMenu);
                            if (szSubItemId === 'subpowertimer') {
                                that._setSubPowerOnTimer(data.index);
                            } else if (szSubItemId === 'subchannel') {
                                that._setSubPowerOnChGroup(data.index);
                                return;
                            } else if (szSubItemId === 'subvolume') {
                                that._setSubOnTimerVolume(data.index, data.getFirstItem);
                            } else if (szSubItemId === 'subrepeat') {
                                that._setSubOnTimerRepeat(data.index);
                            }

                            if (that._currentOnTimerSubMenu === that.onTimerMenuList.CHANNELLIST) {
                                that._setSubOnTimerChannel(data.index, data.getFirstItem);
                                ulItemIdx = that._conv_subItemID_to_index('powerontimer', 'subchannel');
                                that._currentOnTimerSubMenu = ulItemIdx;
                            }
                            that._onTimerMainMenu.getItem(that._currentOnTimerSubMenu).removeClass('his');
                        } else if (szItemId === 'powerofftimer') {
                            that._setSubPowerOffTimer(data.index);
                            that._onTimerMainMenu.getItem(that._currentOnTimerSubMenu).removeClass('his');
                        }
                        that._onTimersubList.$.remove();
                        that._onTimerMainMenu.on();
                        that._$onTimerSubMenu.removeClass('active');
                    }
                });
                break;
            case that._okBtn:
                bConsumed = true;
                if (that._currentState === 'offtimer') {
                    if (hx.config.use2ndPowerOnOffTimer === true) {
                        that.getPowerOnInfoData();
                        if (that._wakeupTime === (that._powerOffHour + ':' + that._powerOffMin) && (that._ispowerOnTimerOn === true)) {//conflict
                            hx.msg('response', {
                                text : hx.l('LOC_POWEROFFTIME_CANNOT_SAME_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        } else {
                            hx.svc.WEB_STORAGE.setPowerOffTimer({
                                    hour : that._powerOffHour,
                                    min : that._powerOffMin,
                                    isOn : 'On'
                            });
                            that._ispowerOffTimerOn = 'On';
                            ulItemIdx = that._conv_mainItemID_to_index('powerofftimer');
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._powerOffHour + ':' + that._powerOffMin);
                            hx.msg('response', {
                                text : hx.l('LOC_POWEROFFTIMER_SET_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        }
                        that._$setStge.removeClass('active');
                        that._onTimerMainMenu.$.remove();
                        that._title.$.remove();
                        that._powerTimerMainMenu.on();
                        that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                        that._currentState = 'main';
                    } else {
                        if (that._ispowerOffTimerOn === 'On') {
                            hx.svc.WEB_STORAGE.setPowerOffTimer({
                                    hour : that._powerOffHour,
                                    min : that._powerOffMin,
                                    isOn : 'Off'
                            });
                            ulItemIdx = that._conv_mainItemID_to_index('powerofftimer');
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub02Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
                            that._$setStge.removeClass('active');
                            that._onTimerMainMenu.$.remove();
                            that._title.$.remove();
                            that._powerTimerMainMenu.on();
                            that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                        } else {
                            that.getPowerOnInfoData();
                            if (that._wakeupTime === (that._powerOffHour + ':' + that._powerOffMin) && (that._ispowerOnTimerOn === true)) {//conflict
                                hx.msg('response', {
                                    text : hx.l('LOC_POWEROFFTIME_CANNOT_SAME_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            } else {
                                hx.svc.WEB_STORAGE.setPowerOffTimer({
                                        hour : that._powerOffHour,
                                        min : that._powerOffMin,
                                        isOn : 'On'
                                });
                                ulItemIdx = that._conv_mainItemID_to_index('powerofftimer');
                                that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._powerOffHour + ':' + that._powerOffMin);
                                hx.msg('response', {
                                    text : hx.l('LOC_POWEROFFTIMER_SET_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            }
                            that._$setStge.removeClass('active');
                            that._onTimerMainMenu.$.remove();
                            that._title.$.remove();
                            that._powerTimerMainMenu.on();
                            that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                        }
                        that._currentState = 'main';
                    }
                } else if (that._currentState === 'ontimer') {
                    if (hx.config.use2ndPowerOnOffTimer === true) {
                        retObjOff = hx.svc.WEB_STORAGE.getPowerOffTimer();
                        powerOffTimeStr = retObjOff.hour + ':' + retObjOff.min;
                        if ((retObjOff.isOn === 'On') && (powerOffTimeStr === that._wakeupTime)) {
                            hx.msg('response', {
                                text : hx.l('LOC_POWERONTIME_CANNOT_SAME_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        } else {
                            that.setPowerOnInfoData(that._wakeupTime, that._wakeupRepeatDays, that.currentList[that._selectChannelIdx].ccid, that._wakeupVolume);
                            ulItemIdx = that._conv_mainItemID_to_index('powerontimer');
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._wakeupTime);
                            hx.msg('response', {
                                text : hx.l('LOC_POWERONTIMER_SET_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        }
                        that._$setStge.removeClass('active');
                        that._onTimerMainMenu.$.remove();
                        that._title.$.remove();
                        that._powerTimerMainMenu.on();
                        that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                        that._currentState = 'main';
                    } else {
                        if (that._ispowerOnTimerOn === false || that.currentList.length === 0) {
                            var recordings, scheduleList,i;
                            recordings = hx.svc.RECORDINGS;
                            scheduleList = recordings.getScheduledList();

                            for(i=0;i<scheduleList.length; i+=1){
                                var item = scheduleList[i];
                                if(item.isPowerOnTimer()) {
                                    recordings.removeScheduledRecording({
                                        items : [item],
                                        finished : () => {
                                            prism.log("power on timer schedule deleted!!");
                                        }
                                    });
                                }
                            }
                            /*
                             hx.svc.SETTING_UTIL.clearPowerOnInfo();*/
                            ulItemIdx = that._conv_mainItemID_to_index('powerontimer');
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub01Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
                            that._$setStge.removeClass('active');
                            that._onTimerMainMenu.$.remove();
                            that._title.$.remove();
                            that._powerTimerMainMenu.on();
                            that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                        } else {
                            retObjOff = hx.svc.WEB_STORAGE.getPowerOffTimer();
                            powerOffTimeStr = retObjOff.hour + ':' + retObjOff.min;
                            if (powerOffTimeStr === that._wakeupTime) {
                                hx.msg('response', {
                                    text : hx.l('LOC_POWERONTIME_CANNOT_SAME_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            } else if (hx.config.useGracenote && (that._wakeupTime === that._tvtvUpdateTime)) {
                                hx.msg('response', {
                                    text : hx.l('LOC_ONTIME_UPDATETIME_CONFLICT_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            } else {
                                that.setPowerOnInfoData(that._wakeupTime, that._wakeupRepeatDays, that.currentList[that._selectChannelIdx].ccid, that._wakeupVolume);
                                ulItemIdx = that._conv_mainItemID_to_index('powerontimer');
                                that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._wakeupTime);
                                hx.msg('response', {
                                    text : hx.l('LOC_POWERONTIMER_SET_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            }
                            that._$setStge.removeClass('active');
                            that._onTimerMainMenu.$.remove();
                            that._title.$.remove();
                            that._powerTimerMainMenu.on();
                            that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                        }
                        that._currentState = 'main';
                    }
                }
                that._okBtn.$.removeClass('on');
                that._cancelBtn.$.removeClass('on');
                break;
            case that._cancelBtn:
                bConsumed = true;
                if (that._currentState === 'offtimer') {
                    var retObj = hx.svc.WEB_STORAGE.getPowerOffTimer();
                    that._powerOffHour = retObj.hour;
                    that._powerOffMin = retObj.min;
                    ulItemIdx = that._conv_mainItemID_to_index('powerofftimer');
                    if (that._ispowerOffTimerOn === 'Off') {
                        that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub02Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
                    } else {
                        if(!retObj.isOn) {
                            that._ispowerOffTimerOn = 'Off';
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub02Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
                        } else {
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._powerOffHour + ':' + that._powerOffMin);
                        }
                    }
                    that._currentState = 'main';
                } else if (that._currentState === 'ontimer') {
                    that.getPowerOnInfoData();
                    that._currentState = 'main';
                }

                that._$setStge.removeClass('active');
                that._onTimerMainMenu.$.remove();
                that._title.$.remove();
                that._powerTimerMainMenu.on();
                that._powerTimerMainMenu.setItemFocus(that._currentSubMenu);
                that._okBtn.$.removeClass('on');
                that._cancelBtn.$.removeClass('on');
                break;
        }
        return bConsumed;
    }
    _on_alExit(aParam): boolean {
        var that = this, bConsumed: boolean = false, ulItemIdx: number = 0;
        if (aParam.alData) {
            if (aParam.alData.me === that._powerTimerMainMenu) {
                bConsumed = true;
                that._isChangePageInSetting = true;
                hx.al.decompose(that);
                that._$menu.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : that
                        });
                    }
                });
            } else if (aParam.alData.me === that._onTimerMainMenu || aParam.alData.me === that._okBtn || aParam.alData.me === that._cancelBtn) {
                bConsumed = true;
                if (that._currentState === 'offtimer') {
                    var retObj = hx.svc.WEB_STORAGE.getPowerOffTimer();
                    that._powerOffHour = retObj.hour;
                    that._powerOffMin = retObj.min;
                    ulItemIdx = that._conv_mainItemID_to_index('powerofftimer');
                    if (that._ispowerOffTimerOn === 'Off') {
                        that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub02Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
                    } else {
                        if (!retObj.isOn) {
                            that._ispowerOffTimerOn = 'Off';
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub02Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
                        } else {
                            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._powerOffHour + ':' + that._powerOffMin);
                        }
                    }
                    that._currentState = 'main';
                } else if (that._currentState === 'ontimer') {
                    that.getPowerOnInfoData();
                    that._currentState = 'main';
                }

                that._okBtn.$.removeClass('on');
                that._cancelBtn.$.removeClass('on');
                that._$setStge.removeClass('active');
                that._onTimerMainMenu.$.remove();
                that._title.$.remove();
                that._powerTimerMainMenu.on();
            }
        }
        return bConsumed;
    }
    _on_alKey(aParam): boolean {
        var that = this, bConsumed: boolean = false, subItdmIdx: number = 0;
        switch (aParam.alKey) {
            case hx.key.KEY_ESCAPE:
                if (aParam.alData) {
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
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_BACK:
            case hx.key.KEY_LEFT:
                if (aParam.alData) {
                    if (aParam.alData.me === that._onTimersubList) {
                        bConsumed = true;
                        that._onTimersubList.off();
                        that._$onTimerSubMenu.ani({
                            style : that.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : () => {
                                if (that._currentOnTimerSubMenu === that.onTimerMenuList.CHANNELLIST) {
                                    subItdmIdx = that._conv_subItemID_to_index('powerontimer', 'subchannel');
                                    that._currentOnTimerSubMenu = subItdmIdx;
                                    that._selectChannelIdx = that._oldSelectChannelIdx;
                                }
                                that._onTimersubList.$.remove();
                                that._$onTimerSubMenu.removeClass('active');
                                that._onTimerMainMenu.on();
                                that._onTimerMainMenu.setItemFocus(that._currentOnTimerSubMenu);
                                that._onTimerMainMenu.getItem(that._currentOnTimerSubMenu).removeClass('his');
                            }
                        });
                    } else if (aParam.alData.me === that._subList) {
                        bConsumed = true;
                        that._subList.off();
                        that._$contextMenu.ani({
                            style : that.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : () => {
                                var menuFocusIndex = that._currentSubMenu;
                                that._subList.$.remove();
                                that._$contextMenu.removeClass('active');
                                that._powerTimerMainMenu.getItem(menuFocusIndex).removeClass('his');
                                that._powerTimerMainMenu.setItemFocus(menuFocusIndex);
                                that._powerTimerMainMenu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _on_alScrollLeft(aParam): boolean {
        var data = aParam.alData, target = data.me, bConsumed: boolean = false;
        if (target === this._okBtn) {
            bConsumed = true;
            this._okBtn.$.removeClass('on');
            this._cancelBtn.on();
            this._cancelBtn.$.addClass('on');
        } else if (target === this._cancelBtn) {
            bConsumed = true;
            this._cancelBtn.$.removeClass('on');
            this._okBtn.on();
            this._okBtn.$.addClass('on');
        }
        return bConsumed;
    }
    _on_alScrollRight(aParam): boolean {
        var data = aParam.alData, target = data.me, bConsumed: boolean = false;
        if (target === this._okBtn) {
            bConsumed = true;
            this._okBtn.$.removeClass('on');
            this._cancelBtn.on();
            this._cancelBtn.$.addClass('on');
        } else if (target === this._cancelBtn) {
            bConsumed = true;
            this._cancelBtn.$.removeClass('on');
            this._okBtn.on();
            this._okBtn.$.addClass('on');
        }
        return bConsumed;
    }
    _on_alScrollup(aParam): boolean {
        var bConsumed: boolean = false, subItdmIdx: number = 0;
        if (aParam.alData.me === this._okBtn || aParam.alData.me === this._cancelBtn) {
            bConsumed = true;
            if (aParam.alData.me === this._okBtn) {
                this._okBtn.$.removeClass('on');
            } else if (aParam.alData.me === this._cancelBtn) {
                this._cancelBtn.$.removeClass('on');
            }
            this._onTimerMainMenu.on();
            subItdmIdx = this._conv_subItemID_to_index('powerontimer', 'subrepeat');
            this._onTimerMainMenu.setItemFocus(subItdmIdx);
        } else if (aParam.alData.me === this._onTimerMainMenu) {
            bConsumed = true;
            this._okBtn.on();
            this._okBtn.$.addClass('on');
        } else if (aParam.alData.me === this._onTimersubList) {
            bConsumed = true;
            var cmpt = this._onTimersubList;
            // this._subMenuScrollup(cmpt);  // TODO remove _subMenuScrolldown is not exist.
        }
        return bConsumed;
    }
    _on_alScrolldown(aParam) {
        var bConsumed = false;
        if (aParam.alData.me === this._onTimerMainMenu) {
            bConsumed = true;
            this._okBtn.on();
            this._okBtn.$.addClass('on');
        } else if (aParam.alData.me === this._okBtn || aParam.alData.me === this._cancelBtn) {
            bConsumed = true;
            if (aParam.alData.me === this._okBtn) {
                this._okBtn.$.removeClass('on');
            } else if (aParam.alData.me === this._cancelBtn) {
                this._cancelBtn.$.removeClass('on');
            }
            this._onTimerMainMenu.on();
        } else if (aParam.alData.me === this._onTimersubList) {
            bConsumed = true;
            var cmpt = this._onTimersubList;
            // this._subMenuScrolldown(cmpt); // TODO remove _subMenuScrolldown is not exist.
        }
        return bConsumed;
    }
    _conv_mainItemID_to_index(aItemID: string): number {
        var i: number = 0, ulIndex: number = -1;
        for (i = 0; i < this._contextListMap.itemId.length; i+=1) {
            if (aItemID === this._contextListMap.itemId[i]) {
                ulIndex = i;
                return	ulIndex;
            }
        }
        return ulIndex;
    }
    _conv_mainIndex_to_itemID(aItemIdx: number): string {
        var i: number = 0, itemID: string = 'undefined';
        for (i = 0; i < this._contextListMap.itemId.length; i+=1) {
            if (i === aItemIdx) {
                itemID = this._contextListMap.itemId[i];
                return	itemID;
            }
        }
        return itemID;
    }
    _conv_subItemID_to_index(aMainItemID: string, aItemID: string): number {
        var i: number = 0, ulIndex: number = -1;
        if (aMainItemID === 'powerontimer') {
            for (i = 0; i < this._settimerListMap.powerOnItemId.length; i+=1) {
                if (aItemID === this._settimerListMap.powerOnItemId[i]) {
                    ulIndex = i;
                    return	ulIndex;
                }
            }
        } else if (aMainItemID === 'powerofftimer') {
            for (i = 0; i < this._settimerListMap.powerOffItemId.length; i+=1) {
                if (aItemID === this._settimerListMap.powerOffItemId[i]) {
                    ulIndex = i;
                    return	ulIndex;
                }
            }
        }
        return ulIndex;
    }
    _conv_subIndex_to_itemID(aMainItemID: string, aItemIdx: number): string {
        var i: number = 0, itemID: string = 'undefined';
        if (aMainItemID === 'powerontimer') {
            for (i = 0; i < this._settimerListMap.powerOnItemId.length; i+=1) {
                if (i === aItemIdx) {
                    itemID = this._settimerListMap.powerOnItemId[i];
                    return	itemID;
                }
            }
        } else if (aMainItemID === 'powerofftimer') {
            for (i = 0; i < this._settimerListMap.powerOffItemId.length; i+=1) {
                if (i === aItemIdx) {
                    itemID = this._settimerListMap.powerOffItemId[i];
                    return	itemID;
                }
            }
        }
        return itemID;
    }
    setPowerOnInfoData(aCtime: string, aRepeat: string, aCcid, aVolume: string) {
        var key;
        var TRepeatDays = {
            ESUNDAY : 0x01,
            EMONDAY : 0x02,
            ETUESDAY : 0x04,
            EWEDNESDAY : 0x08,
            ETHURSDAY : 0x10,
            EFRIDAY : 0x20,
            ESATURDAY : 0x40
        };
        var days = [];
        var _time = aCtime.split(':');
        var baseTime = new Date();
        var time = new Date();
        var repeatDays: number = 0x00;
        var unixtime: number = 0;
        var volume: number = parseInt(aVolume, 10);
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var scheduleList = dsrvRecordings2.getScheduledList();

        for (key in TRepeatDays) {
            days.push(TRepeatDays[key]);
        }
        time.setMilliseconds(0);
        time.setSeconds(0);
        time.setHours(parseInt(_time[0], 10));
        time.setMinutes(parseInt(_time[1], 10));
        time["addDays"] = function(days) {
            this.addTime(days * 24 * 60 * 60 * 1000);
        };
        if (time.getTime() < baseTime.getTime()) {
            time["addDays"](1);
        }
        unixtime = time.getTime() /1000;

        var i, loop;
        switch (aRepeat) {
            case hx.l('LOC_ONCE_ID'):
                //repeatDays = days[time.getDay()];
                repeatDays = 0;
                break;
            case hx.l('LOC_DAILY_ID'):
                for ( i = 0, loop = days.length; i < loop; i += 1) {
                    repeatDays += days[i];
                }
                break;
            case hx.l('LOC_WEEKLY_ID'):
                repeatDays = days[baseTime.getDay()];
                break;
        }
        for ( i=0; i<scheduleList.length; i+=1) {
            var item = scheduleList[i];
            if (item.isPowerOnTimer()) {
                dsrvRecordings2.removeScheduledRecording({
                    items : [item],
                    finished : () => {
                        prism.log("power on timer schedule deleted!!");
                    }
                });
                break;
            }
        }
        hx.svc.WEB_STORAGE.setPowerTimerLastGroup({
                grpName : this.settedChGrp
        });
        dsrvRecordings2.recordAtPowerOnTimer({
            startTime : unixtime,
            duration : 0,
            repeatDays : repeatDays,
            ccid : aCcid,
            volume: volume
        }); // no need "factoryType"
    }

    getPowerTimerLastGroupList() {
        var retGrp = hx.svc.WEB_STORAGE.getPowerTimerLastGroup();

        this.currentGrp = retGrp.grpName;
        if (this.currentGrp === undefined || this.currentGrp === null) {
            hx.svc.WEB_STORAGE.setPowerTimerLastGroup({
                    grpName : 'TV'
            });
            this.currentGrp = 'TV';
        }
        this.settedChGrp = this.currentGrp;
        this.getSrvlist(this.settedChGrp);
    }
    getPowerOnInfoData() {
        var that = this, retObjOn = hx.svc.SETTING_UTIL.getPowerOnInfo();
        var ulItemIdx: number = that._conv_mainItemID_to_index('powerontimer');

        if (retObjOn.channel) {
            var channelIndex = function(aCcid): number {
                var i: number, loop: number;
                for ( i = 0, loop = that.currentList.length; i < loop; i += 1) {
                    if (that.currentList[i].ccid === aCcid) {
                        return i;
                    }
                }
                return 0;
            }, repeatDay = function(aRepeat): string {
                var ret;
                switch (aRepeat) {
                    case 0x7f:
                        ret = hx.l('LOC_DAILY_ID');
                        break;
                    case 0x00:
                        ret = hx.l('LOC_ONCE_ID');
                        break;
                    case 0x01:
                    case 0x02:
                    case 0x04:
                    case 0x08:
                    case 0x10:
                    case 0x20:
                    case 0x40:
                        ret = hx.l('LOC_WEEKLY_ID');
                        break;
                    default:
                        ret = hx.l('LOC_UNKNOWN_ID');
                        break;
                }
                return ret;
            };
            that._selectChannelIdx = channelIndex(retObjOn.channel.ccid);
            that._oldSelectChannelIdx = that._selectChannelIdx;
            that._wakeupTime = retObjOn.time.format('HH:MM');
            that._wakeupVolume = retObjOn.volume;
            that._wakeupRepeatDays = repeatDay(retObjOn.repeatDays);
            that._ispowerOnTimerOn = true;
            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(that._wakeupTime);
        } else {
            that._ispowerOnTimerOn = false;
            that._wakeupRepeatDays = hx.l('LOC_DAILY_ID');
            that._powerTimerMainMenu.getItem(ulItemIdx).find('em').html(hx.l(that._contextListMap.itemSub01Str[that.TMenuItemValue_OffSetTimer.VALUE_OFF]));
        }
    }
    getChlist() {
        var retObj, retGrp;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'TV'
        });
        this._tvList = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'RADIO'
        });
        this._radioList = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'HDTV'
        });
        this._hdtvList = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'Favourite1'
        });
        this._fav1List = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'Favourite2'
        });
        this._fav2List = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'Favourite3'
        });
        this._fav3List = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'Favourite4'
        });
        this._fav4List = retObj.chList;

        retObj = hx.svc.CH_LIST.getChlist({
                group : 'Favourite5'
        });
        this._fav5List = retObj.chList;

        retGrp = hx.svc.WEB_STORAGE.getPowerTimerLastGroup();

        this.currentGrp = retGrp.grpName;
        if (this.currentGrp === undefined || this.currentGrp === null) {
            hx.svc.WEB_STORAGE.setPowerTimerLastGroup({
                    grpName : 'TV'
            });
            this.currentGrp = 'TV';
        }
        retObj = undefined;
        this.settedChGrp = this.currentGrp;
        this.getSrvlist(this.settedChGrp);
    }
    getSrvlist(aChType: string) {
        switch(aChType) {
            case 'TV':
                this.currentList = this._tvList;
                break;
            case 'RADIO':
                this.currentList = this._radioList;
                break;
            case 'HDTV':
                this.currentList = this._hdtvList;
                break;
            case 'Favourite1':
                this.currentList = this._fav1List;
                break;
            case 'Favourite2':
                this.currentList = this._fav2List;
                break;
            case 'Favourite3':
                this.currentList = this._fav3List;
                break;
            case 'Favourite4':
                this.currentList = this._fav4List;
                break;
            case 'Favourite5':
                this.currentList = this._fav5List;
                break;
            default:
                this.currentList = this._tvList;
                break;
        }
        var i: number;
        this._svcList.length = 0;
        for ( i = 0; i < this.currentList.length; i += 1) {
            this._svcList.push(this.currentList[i].majorChannel + ' ' + this.currentList[i].name);
        }
    }
    _makesubList(aParam) {
        var that = this, index: number = 0, subFocusIndex: number = 0, szItemId: string = 'undefined', dataList: string[];
        szItemId = this._contextListMap.itemId[aParam];
        if (szItemId === 'powerontimer') {
            subFocusIndex = (this._ispowerOnTimerOn === true) ? 1 : 0;
            dataList = this._contextListMap.itemSub01Str;
        } else if (szItemId === 'powerofftimer') {
            subFocusIndex = (this._ispowerOffTimerOn === 'On') ? 1 : 0;
            dataList = this._contextListMap.itemSub02Str;
        }

        this._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,

            displayCount : dataList.length
        });

        this._$contextMenu.addClass('active');
        this._powerTimerMainMenu.getItem(aParam).addClass('his');
        this._$contextMenu.append(this._subList.$);
        this._subList.bind(dataList);
        this._subList.draw();
        this._$contextMenu.ani({
            style: that.aniConfig.dep2.fadeIn,
            trans: {dur: 200, delay: 100},
            cb: () => {
                that.append(that._subList);
                that._subList.on();
            }
        });
    }
    _makeOnTimersubList(aParam) {
        var that = this, index: number, subFocusIndex: number, dataList: string[], findGrpName: string, i: number;
        var szSubItemId: string = 'undefined';
        szSubItemId = this._conv_subIndex_to_itemID('powerontimer', aParam);
        if (szSubItemId === 'subpowertimer') {
            dataList = this._settimerListMap.itemSub01Str;
            subFocusIndex = (this._ispowerOnTimerOn === true) ? 0 : 1;
        } else if (szSubItemId === 'subchannel') {
            this.grpList = [];
            this.grpList.length = 0;
            if (this._tvList.length > 0) {
                this.grpList.push(hx.l('LOC_TV_ID'));
            }
            if (this._radioList.length > 0) {
                this.grpList.push(hx.l('LOC_RADIO_ID'));
            }
            if (this._hdtvList.length > 0) {
                this.grpList.push(hx.l('LOC_HDTV_ID'));
            }
            if (this._fav1List.length > 0) {
                this.grpList.push(hx.svc.CH_LIST.getFavListName({
                        favtype : 'Favourite1'
                }));
            }
            if (this._fav2List.length > 0) {
                this.grpList.push(hx.svc.CH_LIST.getFavListName({
                        favtype : 'Favourite2'
                }));
            }
            if (this._fav3List.length > 0) {
                this.grpList.push(hx.svc.CH_LIST.getFavListName({
                        favtype : 'Favourite3'
                }));
            }
            if (this._fav4List.length > 0) {
                this.grpList.push(hx.svc.CH_LIST.getFavListName({
                        favtype : 'Favourite4'
                }));
            }
            if (this._fav5List.length > 0) {
                this.grpList.push(hx.svc.CH_LIST.getFavListName({
                        favtype : 'Favourite5'
                }));
            }
            //dataList = this._svcList;
            dataList = this.grpList;
            subFocusIndex = 0;
            switch(this.settedChGrp) {
                case 'TV':
                    findGrpName = hx.l('LOC_TV_ID');
                    break;
                case 'RADIO':
                    findGrpName = hx.l('LOC_RADIO_ID');
                    break;
                case 'HDTV':
                    findGrpName = hx.l('LOC_HDTV_ID');
                    break;
                case 'Favourite1':
                    findGrpName = hx.svc.CH_LIST.getFavListName({
                            favtype : 'Favourite1'
                    });
                    break;
                case 'Favourite2':
                    findGrpName = hx.svc.CH_LIST.getFavListName({
                            favtype : 'Favourite2'
                    });
                    break;
                case 'Favourite3':
                    findGrpName = hx.svc.CH_LIST.getFavListName({
                            favtype : 'Favourite3'
                    });
                    break;
                case 'Favourite4':
                    findGrpName = hx.svc.CH_LIST.getFavListName({
                            favtype : 'Favourite4'
                    });
                    break;
                case 'Favourite5':
                    findGrpName = hx.svc.CH_LIST.getFavListName({
                            favtype : 'Favourite5'
                    });
                    break;
                default:
                    findGrpName = hx.l('LOC_TV_ID');
                    break;
            }
            for ( i = 0; i < this.grpList.length; i += 1) {
                if (findGrpName === this.grpList[i]) {
                    subFocusIndex = i;
                    break;
                }
            }
        } else if (szSubItemId === 'subvolume') {
            dataList = this._settimerListMap.itemSub02Str;
            subFocusIndex = parseInt(this._wakeupVolume, 10) - 1;
        } else if (szSubItemId === 'subrepeat') {
            dataList = this._settimerListMap.itemSub03Str;
            subFocusIndex = 0;

            for ( i = 0; i < this._settimerListMap.itemSub03Str.length; i+=1) {
                if (this._wakeupRepeatDays === hx.l(this._settimerListMap.itemSub03Str[i])) {
                    subFocusIndex = i;
                    break;
                }
            }
        }

        if (aParam === this.onTimerMenuList.CHANNELLIST) {
            dataList = this._svcList;
            if (this.settedChGrp === this.currentGrp) {
                subFocusIndex = this._selectChannelIdx;
            } else {
                this._oldSelectChannelIdx = this._selectChannelIdx;
                this._selectChannelIdx = 0;
                subFocusIndex = this._selectChannelIdx;
            }
        }

        var _isNotId: boolean = false;

        szSubItemId = this._conv_subIndex_to_itemID('powerontimer', aParam);
        if ((szSubItemId === 'subchannel') || (aParam === this.onTimerMenuList.CHANNELLIST)) {
            _isNotId = true;
        }
        this._onTimersubList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length,
            _isNotId : _isNotId
        });
        var hisIndex: number = aParam;
        if (hisIndex === 5) {
            if (hx.config.use2ndPowerOnOffTimer !== true) {
                hisIndex = 2;
            } else {
                hisIndex = 1;
            }
        }
        this._$onTimerSubMenu.addClass('active');
        this._onTimerMainMenu.getItem(hisIndex).addClass('his');
        this._$onTimerSubMenu.append(this._onTimersubList.$);
        this._onTimersubList.bind(dataList);
        this._onTimersubList.draw();
        this._$onTimerSubMenu.ani({
            style : that.aniConfig.dep2.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : () => {
                that.append(that._onTimersubList);
                that._onTimersubList.on();
            }
        });
    }
    _makeOffTimersubList(aParam) {
        var that = this, index: number, subFocusIndex: number, dataList: string[];
        var szSubItemId: string = 'undefined';

        szSubItemId = this._conv_subIndex_to_itemID('powerofftimer', aParam);
        if (szSubItemId === 'subpowertimer') {
            dataList = this._settimerListMap.itemSub01Str;
            subFocusIndex = (this._ispowerOffTimerOn === 'On') ? 0 : 1;
        }

        this._onTimersubList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length
        });
        this._$onTimerSubMenu.addClass('active');
        this._onTimerMainMenu.getItem(aParam).addClass('his');
        this._$onTimerSubMenu.append(this._onTimersubList.$);
        this._onTimersubList.bind(dataList);
        this._onTimersubList.draw();
        this._$onTimerSubMenu.ani({
            style: that.aniConfig.dep2.fadeIn,
            trans: {dur: 200, delay: 100},
            cb: () => {
                that.append(that._onTimersubList);
                that._onTimersubList.on();
            }
        });
    }
    _setPowerOnTimer(aIndex: number) {
        var ulItemIdx: number = this._conv_mainItemID_to_index('powerontimer');
        var $powerOnTimerSub = this._powerTimerMainMenu.getItem(ulItemIdx).find('em');
        var powerOnTimerKey: string = hx.l(this._contextListMap.itemSub01Str[aIndex]);
        var ulsubItemIdx: number = 0;
        var recordings, scheduleList,i;

        recordings = hx.svc.RECORDINGS;
        scheduleList = recordings.getScheduledList();

        if (aIndex === this.TMenuItemValue_OffSetTimer.VALUE_OFF) {
            $powerOnTimerSub.html(powerOnTimerKey);
        }
        this._powerTimerMainMenu.getItem(ulItemIdx).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');

        if (aIndex === this.TMenuItemValue_OffSetTimer.VALUE_OFF) {//off
            this._ispowerOnTimerOn = false;
            for(i=0;i<scheduleList.length; i+=1)
            {
                var item = scheduleList[i];
                if(item.isPowerOnTimer())
                {
                    recordings.removeScheduledRecording({
                        items : [item],
                        finished : () => {
                            prism.log("power on timer schedule deleted!!");
                        }
                    });
                    break;
                }
            }
            /*
             hx.svc.SETTING_UTIL.clearPowerOnInfo();
             */
            this._powerTimerMainMenu.on();
            this._powerTimerMainMenu.setItemFocus(ulItemIdx);
        } else if (aIndex === this.TMenuItemValue_OffSetTimer.VALUE_SETTIMER) {//set Timer
            this.getPowerTimerLastGroupList();
            this.getPowerOnInfoData();

            this._$setStge.addClass('active');
            this._onTimerMainMenu = this._makePowerTimerMainMenu(this._settimerListMap.powerOnItem, 'dep1', 'notroll');
            if (hx.config.use2ndPowerOnOffTimer !== true) {
                ulsubItemIdx = this._conv_subItemID_to_index('powerontimer', 'subpowertimer');
                this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html((this._ispowerOnTimerOn === true) ? hx.l(this._settimerListMap.itemSub01Str[this.TMenuItemValue_OnOff.VALUE_ON]) : hx.l(this._settimerListMap.itemSub01Str[this.TMenuItemValue_OnOff.VALUE_OFF]));
            }
            ulsubItemIdx = this._conv_subItemID_to_index('powerontimer', 'subtime');
            this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(this._wakeupTime);
            ulsubItemIdx = this._conv_subItemID_to_index('powerontimer', 'subchannel');
            if (this._svcList.length === 0) {
                this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(hx.l('LOC_NO_CHANNEL_ID'));
            } else {
                this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(this._svcList[this._selectChannelIdx]);
            }
            ulsubItemIdx = this._conv_subItemID_to_index('powerontimer', 'subvolume');
            this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(this._wakeupVolume);
            ulsubItemIdx = this._conv_subItemID_to_index('powerontimer', 'subrepeat');
            this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(this._wakeupRepeatDays);

            this._title = this._makeTitle('LOC_POWERONTIMER_ID');
            this._$onTimerSubMenu.after(this._onTimerMainMenu.$);
            this._$setStge.prepend(this._title.$);
            this.append(this._onTimerMainMenu);
            this._okBtn.$.blur();
            this._cancelBtn.$.blur();
            this._onTimerMainMenu.on();
        }
    }
    _setPowerOffTimer(aIndex: number) {
        var ulItemIdx: number = this._conv_mainItemID_to_index('powerofftimer');
        var $powerOffTimerSub = this._powerTimerMainMenu.getItem(ulItemIdx).find('em');
        var powerOffTimerKey: string = hx.l(this._contextListMap.itemSub02Str[aIndex]);
        var ulsubItemIdx: number = 0;
        if (aIndex === this.TMenuItemValue_OffSetTimer.VALUE_OFF) {
            $powerOffTimerSub.html(powerOffTimerKey);
        }

        this._powerTimerMainMenu.getItem(ulItemIdx).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        if (aIndex === this.TMenuItemValue_OffSetTimer.VALUE_OFF) {//off
            hx.svc.WEB_STORAGE.setPowerOffTimer({
                    hour : this._powerOffHour,
                    min : this._powerOffMin,
                    isOn : 'Off'
            });
            this._ispowerOffTimerOn = 'Off';
            this._powerTimerMainMenu.on();
            this._powerTimerMainMenu.setItemFocus(ulItemIdx);
        } else if (aIndex === this.TMenuItemValue_OffSetTimer.VALUE_SETTIMER) {//set Timer
            var retObj = hx.svc.WEB_STORAGE.getPowerOffTimer();
            this._powerOffHour = retObj.hour;
            this._powerOffMin = retObj.min;

            this._$setStge.addClass('active');
            this._onTimerMainMenu = this._makePowerTimerMainMenu(this._settimerListMap.powerOffItem, 'dep1', 'notroll');
            if (hx.config.use2ndPowerOnOffTimer !== true) {
                ulsubItemIdx = this._conv_subItemID_to_index('powerofftimer', 'subpowertimer');
                this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(this._ispowerOffTimerOn === 'On' ? hx.l(this._settimerListMap.itemSub01Str[this.TMenuItemValue_OnOff.VALUE_ON]) : hx.l(this._settimerListMap.itemSub01Str[this.TMenuItemValue_OnOff.VALUE_OFF]));
            }
            ulsubItemIdx = this._conv_subItemID_to_index('powerofftimer', 'subtime');
            if (this._powerOffHour !== null && this._powerOffMin !== null) {
                this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html(this._powerOffHour + ':' + this._powerOffMin);
            } else {
                this._onTimerMainMenu.getItem(ulsubItemIdx).find('em').html('00' + ':' + '00');
            }
            this._title = this._makeTitle('LOC_POWEROFFTIMER_ID');
            this._$onTimerSubMenu.after(this._onTimerMainMenu.$);
            this._$setStge.prepend(this._title.$);
            this.append(this._onTimerMainMenu);
            this._okBtn.$.blur();
            this._cancelBtn.$.blur();
            this._onTimerMainMenu.on();
        }
    }
    _setSubPowerOffTimer(aIndex: number) {
        var ulsubItemIdx: number = this._conv_subItemID_to_index('powerofftimer', 'subpowertimer');
        var $offTimerSub = this._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
        var offTimerKey: string = hx.l(this._settimerListMap.itemSub01Str[aIndex]);
        $offTimerSub.html(offTimerKey);

        //this._powerTimerMainMenu.getItem(0).removeClass('his');
        this._onTimersubList.$.remove();
        if (aIndex === this.TMenuItemValue_OnOff.VALUE_OFF) {//off
            this._ispowerOffTimerOn = 'Off';
            this._onTimerMainMenu.on();
        } else if (aIndex === this.TMenuItemValue_OnOff.VALUE_ON) {//on
            this._ispowerOffTimerOn = 'On';
            if ((this._powerOffHour === null || this._powerOffMin === null)) {
                this._powerOffHour = '00';
                this._powerOffMin = '00';
            }
            this._onTimerMainMenu.on();
        }
    }
    _setSubPowerOnTimer(aIndex: number) {
        var ulsubItemIdx: number = this._conv_subItemID_to_index('powerontimer', 'subpowertimer');
        var $onTimerSub: JQuery = this._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
        var onTimerKey: string = hx.l(this._settimerListMap.itemSub01Str[aIndex]);
        $onTimerSub.html(onTimerKey);

        //this._powerTimerMainMenu.getItem(0).removeClass('his');
        this._onTimersubList.$.remove();
        if (aIndex === this.TMenuItemValue_OnOff.VALUE_OFF) {//off
            this._ispowerOnTimerOn = false;
            this._onTimerMainMenu.on();
        } else if (aIndex === this.TMenuItemValue_OnOff.VALUE_ON) {//on
            this._ispowerOnTimerOn = true;
            if (this._wakeupTime) {
                var noWork;
                //TODO : set wakeupTime 00:00 ?
            }
            this._onTimerMainMenu.on();
        }
    }
    _setSubPowerOnChGroup(aIndex: number) {
        var ulsubItemIdx: number = this._conv_subItemID_to_index('powerontimer', 'subchannel');
        var $onTimerChannelSub: JQuery = this._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
        var onGrpKey: string = this.grpList[aIndex];

        switch(onGrpKey) {
            case hx.l('LOC_TV_ID'):
                this.currentGrp = 'TV';
                break;
            case hx.l('LOC_RADIO_ID'):
                this.currentGrp = 'RADIO';
                break;
            case hx.l('LOC_HDTV_ID'):
                this.currentGrp = 'HDTV';
                break;
            case hx.svc.CH_LIST.getFavListName({
                    favtype : 'Favourite1'
            }):
                this.currentGrp = 'Favourite1';
                break;
            case hx.svc.CH_LIST.getFavListName({
                    favtype : 'Favourite2'
            }):
                this.currentGrp = 'Favourite2';
                break;
            case hx.svc.CH_LIST.getFavListName({
                    favtype : 'Favourite3'
            }):
                this.currentGrp = 'Favourite3';
                break;
            case hx.svc.CH_LIST.getFavListName({
                    favtype : 'Favourite4'
            }):
                this.currentGrp = 'Favourite4';
                break;
            case hx.svc.CH_LIST.getFavListName({
                    favtype : 'Favourite5'
            }):
                this.currentGrp = 'Favourite5';
                break;
            default:
                this.currentGrp = 'TV';
                break;
        }

        this._onTimersubList.$.remove();

        this._currentOnTimerSubMenu = this.onTimerMenuList.CHANNELLIST;
        if (undefined !== this.currentGrp) {
            this.getSrvlist(this.currentGrp);
        }
        this._makeOnTimersubList(this.onTimerMenuList.CHANNELLIST);
    }
    _setSubOnTimerChannel(aIndex: number, aGetFirstItem: string) {
        var firstItemIdx: number, i: number;
        var ulsubItemIdx: number = this._conv_subItemID_to_index('powerontimer', 'subchannel');
        this.settedChGrp = this.currentGrp;

        for ( i = 0; i < this._svcList.length; i += 1) {
            if (this._svcList[i] === aGetFirstItem) {
                firstItemIdx = i;
                break;
            }
        }
        var realIndex = (aIndex + firstItemIdx > this._svcList.length) ? (aIndex + firstItemIdx - this._svcList.length) : aIndex + firstItemIdx;
        if (realIndex === this._svcList.length) {
            realIndex = 0;
        }

        var $onTimerChannelSub: JQuery = this._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
        var onTimerChannelKey: string = this._svcList[realIndex];
        $onTimerChannelSub.html(onTimerChannelKey);

        //this._powerTimerMainMenu.getItem(0).removeClass('his')
        this._onTimersubList.$.remove();
        this._selectChannelIdx = realIndex;
        this._oldSelectChannelIdx = this._selectChannelIdx;
        this._onTimerMainMenu.on();
        this._onTimerMainMenu.setItemFocus(ulsubItemIdx);
    }
    _setSubOnTimerVolume(aIndex: number, aGetFirstItem: string) {
        var firstItemIdx: number, i: number;
        var ulsubItemIdx: number = this._conv_subItemID_to_index('powerontimer', 'subvolume');

        for ( i = 0; i < this._settimerListMap.itemSub02Str.length; i += 1) {
            if (hx.l(this._settimerListMap.itemSub02Str[i]) === aGetFirstItem) {
                firstItemIdx = i;
                break;
            }
        }
        var realIndex: number = (aIndex + firstItemIdx > this._settimerListMap.itemSub02Str.length) ? (aIndex + firstItemIdx - this._settimerListMap.itemSub02Str.length) : aIndex + firstItemIdx;
        if (realIndex === this._settimerListMap.itemSub02Str.length) {
            realIndex = 0;
        }

        var $onTimerVolumeSub: JQuery = this._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
        var onTimerVolumeKey: string = hx.l(this._settimerListMap.itemSub02Str[realIndex]);
        $onTimerVolumeSub.html(onTimerVolumeKey);

        //this._powerTimerMainMenu.getItem(0).removeClass('his');
        this._onTimersubList.$.remove();
        this._wakeupVolume = onTimerVolumeKey;

        this._onTimerMainMenu.on();
        this._onTimerMainMenu.setItemFocus(ulsubItemIdx);
    }
    _setSubOnTimerRepeat(aIndex: number) {
        var ulsubItemIdx: number = this._conv_subItemID_to_index('powerontimer', 'subrepeat');

        var $onTimerRepeatSub: JQuery = this._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
        var onTimerRepeatKey: string = hx.l(this._settimerListMap.itemSub03Str[aIndex]);
        $onTimerRepeatSub.html(onTimerRepeatKey);

        //this._powerTimerMainMenu.getItem(0).removeClass('his');
        this._onTimersubList.$.remove();

        //TODO : set wakeupVolume
        this._wakeupRepeatDays = onTimerRepeatKey;
        this._onTimerMainMenu.on();
        this._onTimerMainMenu.setItemFocus(ulsubItemIdx);
    }
    _inputOffTime() {
        var that = this, ulsubItemIdx: number = that._conv_subItemID_to_index('powerofftimer', 'subtime');

        var inputfn = (aStatus: string, aInput: string) => {
            if (aStatus === 'stop') {
                var hour: string = aInput.substring(0, 2), min: string = aInput.substring(2, 4), periodString: string = hour + ':' + min;
                that._powerOffHour = hour;
                that._powerOffMin = min;
                that._onTimerMainMenu.on();
                that._onTimerMainMenu.setItemFocus(ulsubItemIdx);
                var $offTimeSub = that._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
                $offTimeSub.html(periodString);
                hx.msg.close('inputonetime', inputTime);
            } else if (aStatus === 'key_back') {
                hx.msg.close('inputonetime', inputTime);
                that._onTimerMainMenu.on();
                that._onTimerMainMenu.setItemFocus(ulsubItemIdx);
            } else if (aStatus === 'key_esc') {
                hx.msg.close('inputonetime', inputTime);
                that._isChangePageInSetting = true;
                hx.al.decompose(that);
                that._$menu.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : that
                        });
                    }
                });
            }
        };

        var inputTime = hx.msg('inputonetime', {
            text : hx.l('LOC_ENTER_POWEROFF_TIME_ID'),
            auto_close : false,
            dia_class : 'osd',
            enable : true,
            callback_fn : inputfn
        });
    }
    _inputOnTime() {
        var that = this, ulsubItemIdx: number = that._conv_subItemID_to_index('powerontimer', 'subtime');

        var inputfn = (aStatus: string, aInput: string) => {
            if (aStatus === 'stop') {
                var hour: string = aInput.substring(0, 2), min: string = aInput.substring(2, 4), periodString: string = hour + ':' + min;
                that._wakeupTime = periodString;
                that._onTimerMainMenu.on();
                that._onTimerMainMenu.setItemFocus(ulsubItemIdx);
                var $onTimeSub = that._onTimerMainMenu.getItem(ulsubItemIdx).find('em');
                $onTimeSub.html(periodString);
                hx.msg.close('inputonetime', inputTime);
            } else if (aStatus === 'key_back') {
                hx.msg.close('inputonetime', inputTime);
                that._onTimerMainMenu.on();
                that._onTimerMainMenu.setItemFocus(ulsubItemIdx);
            } else if (aStatus === 'key_esc') {
                hx.msg.close('inputonetime', inputTime);
                that._isChangePageInSetting = true;
                hx.al.decompose(that);
                that._$menu.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : that
                        });
                    }
                });
            }
        };

        var inputTime = hx.msg('inputonetime', {
            text : hx.l('LOC_ENTER_POWERON_TIME_ID'),
            auto_close : false,
            dia_class : 'osd',
            enable : true,
            callback_fn : inputfn
        });
        //TODO : this._wakeupTime = retObjOn.time;
    }
    _getTvtvUpdateTime() {
        var timeObj = hx.svc.SETTING_UTIL.getTvtvUpdateTime();
        var isTvtvStandbyEnable: boolean = hx.svc.SETTING_UTIL.getIsTvtvStandbyEnable();
        if (timeObj && isTvtvStandbyEnable === true) {
            this._tvtvUpdateTime =  timeObj.format('HH:MM');
        } else {
            this._tvtvUpdateTime = null;
        }
    }
    _makePowerTimerMainMenu(aParam1: any, aParam2: any, aParam3: any): any {
        var powerTimermainmenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : aParam2
            },
            roll : aParam3,
            data : aParam1,
            count : aParam1.length,
            fx : function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return powerTimermainmenu;
    }
    _makePowerTimerSubMenu(aParam1: any, aParam2: any, aParam3: any, aParam4: any): any {
        var powerTimersubmenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : aParam3
            },
            roll : aParam2,
            data : aParam1,
            count : aParam1.length,
            fx : function (i, o) {
                if (aParam4 === 'channelList') {
                    o.html(this.data[i]);
                } else {
                    o.html(hx.l(this.data[i]));
                }
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
        });
        return powerTimersubmenu;
    }
    _makeTitle(aParam: any): any {
        return hx.al.component('text', {
            tag : 'h1',
            attr : {
                'data-langID' : aParam
            }
        });
    }
}
export = cPgPowerTimer;