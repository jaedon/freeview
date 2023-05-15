///<reference path="../../../def/sugar.d.ts" />
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx;
//file name : pages/settings/pvr_settings/remote_record.js
/*jslint sloppy:true, nomen:true, vars:true */

class cCPgRemoteRecord extends __KERNEL_SETTINGS__{
    TMenuList = {
        REMOTERECORDING : 0,
        UPDATEPERIOD : 1,
        UPDATEINTERVAL : 2
    };
    dsrvTVPortal = hx.svc.TV_PORTAL;
    aniConfig : any;

    _isRemoteRecordingOn : boolean = false;
    _updatePeriod : string = 'All Day';
    _updateInterval : number = 30;
    _isIpEpgOn : boolean = false;
    _currentSubMenu : number = 0;
    _isChangePageInSetting : boolean = false;
    _subList : any;
    _remoteRecordMainMenu : any;
    _$noti : any;
    _$contextMenu : any;
    _$menu : any;

    constructor() {
        super();
    }

    create () {
        super.create();
        var that = this;
        var $DrawItem : any, value : number, tmp : string;

        $.extend(this, {
            _$menu : $('<article>', {
                'class' : 'menu'
            }),
            _$contextMenu : $('<div>', {
                'class' : 'dep2'
            }),
            _remoteRecordMainMenu : hx.al.CPgRemoteRecord.remoteRecordMainMenu(that._contextListMap[0]),
            _$noti : $('<p>', {
				'class' : 'noti',
				'html' : 'Agree to the Privacy Policy first to use Remote Recording.' + '<br>' + 'When the Remote Recording menu is set to On, the receiver can receive the scheduled data that you have scheduled the programmes remotely by using tablet or PC.'
            })
        });

        var $title : any = $('#set .title');
		if ($title[0]) {
			$title[0].children[0].innerHTML = hx.l('LOC_REMOTE_RECORD_ID');
        }
         that._isRemoteRecordingOn = hx.svc.SETTING_UTIL.getEnableRemoteRec();
        if (that._isRemoteRecordingOn === false) {
			that._remoteRecordMainMenu.getItem(1).addClass('dis');
			that._remoteRecordMainMenu.getItem(2).addClass('dis');
        }

        value = (that._isRemoteRecordingOn === true) ? 0 : 1;
        $DrawItem = that._remoteRecordMainMenu.getItem(0).find('em');
        $DrawItem.html(hx.l(that._contextListMap[1][value]));

        that._updatePeriod = hx.svc.SETTING_UTIL.getUpdatePeriodRemoteRec();
        tmp = that._conv_updatePeriod_to_string(that._updatePeriod);
        $DrawItem = that._remoteRecordMainMenu.getItem(1).find('em');
        $DrawItem.html(tmp);

        that._updateInterval = hx.svc.SETTING_UTIL.getUpdateIntervalRemoteRec();
        that._isIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
        value = that._conv_updateInterval_to_index(that._updateInterval);
        $DrawItem = that._remoteRecordMainMenu.getItem(2).find('em');
        $DrawItem.html(hx.l(that._contextListMap[3][value]));

        $('#set').append(that._$menu.append(that._$contextMenu.after(that._remoteRecordMainMenu.$)).after(that._$noti));

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
                that.append(that._remoteRecordMainMenu);
                that._remoteRecordMainMenu.on();
                that._isChangePageInSetting = false;
            }
        });
    }
    destroy () {
        super.destroy();
        this._$menu.remove();
        this._$noti.remove();
        if(!this._isChangePageInSetting) {
			$('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this._$menu = undefined;
        this._$contextMenu = undefined;
        this._remoteRecordMainMenu = undefined;
        this._$noti = undefined;
    }
    _conv_updateInterval_to_index (aTime : number) : number {
        var index : number;
        switch(aTime) {
            case 20:
                index = 1;
                break;
            case 30:
                index = 2;
                break;
            case 40:
                index = 3;
                break;
            case 50:
                index = 4;
                break;
            case 10:
                index = 0;
                break;
            default:
                index = 0;
                break;
        }
        return index;
    }
    _conv_index_to_updateInterval (aIndex : number) : number {
        var time : number;
        switch(aIndex) {
            case 1:
                time = 20;
                break;
            case 2:
                time = 30;
                break;
            case 3:
                time = 40;
                break;
            case 4:
                time = 50;
                break;
            case 0:
                time = 10;
                break;
            default:
                time = 10;
                break;
        }
        return time;
    }
    _conv_updatePeriod_to_string (aUpdatePeriod) {
        var that = this, value : any = aUpdatePeriod;
        var startTimeHour : any, startTimeMinute : any, endTimeHour : any, endTimeMinute : any, str_value : string;
			//startDate = new Date(1970, 1, 1, (value.start_time/3600), (value.start_time%3600), 0 ),
			//endDate = new Date(1970, 1, 1, (value.end_time/3600), (value.end_time%3600), 0 ),
			//str_value = startDate.format("HH:MM") + ' - ' + endDate.format("HH:MM");
		startTimeHour = Math.floor(value.start_time/3600);
		if(startTimeHour < 10) {
			startTimeHour = '0' + (value.start_time/3600);
		}

		startTimeMinute = Math.floor(((value.start_time%3600)/60));
		if(startTimeMinute < 10) {
			startTimeMinute = '0' + (value.start_time%3600)/60;
		}

		endTimeHour = Math.floor(value.end_time/3600);
		if(endTimeHour < 10) {
			endTimeHour = '0' + (value.end_time/3600);
		}

		endTimeMinute = Math.floor((value.end_time%3600)/60);
		if(endTimeMinute < 10) {
			endTimeMinute = '0' + (value.end_time%3600)/60;
		}

        str_value = startTimeHour + ':' + startTimeMinute + ' - ' + endTimeHour + ':' + endTimeMinute;

        if (str_value === '00:00 - 24:00')
        {
            str_value = "All Day";
        }
        that._updatePeriod = str_value;
        return str_value;
    }
    _conv_updatePeriod_to_secondValueString (aUpdatePeriod) {
		var startHour : number = aUpdatePeriod.substring(0, 2) * 60 * 60,
		    startMinute : number = aUpdatePeriod.substring(3, 5) * 60,
			endHour : number = aUpdatePeriod.substring(8, 10) * 60 * 60,
			endMinute : number = aUpdatePeriod.substring(11, 13) * 60;
        //var that = this, value = updatePeriod,
		var startTime : number = startHour + startMinute,
			endTime : number = endHour + endMinute;

        return {
			start_time : startTime,
			end_time : endTime
        };
    }
    _on_alClicked (aParam) {
        var that = this, data = aParam.alData, target : any = data.me, index : number, bConsumed : boolean = false;
        var tmp : any, netConnectedInfo : any;
        switch (target) {
            case that._remoteRecordMainMenu:
				bConsumed = true;
                if (data.index === that.TMenuList.REMOTERECORDING || data.index === that.TMenuList.UPDATEPERIOD || data.index === that.TMenuList.UPDATEINTERVAL) {
                    that._remoteRecordMainMenu.off();
					that._makesubList(data.index);
                }
                that._currentSubMenu = data.index;
                break;
            case that._subList:
				bConsumed = true;
				that._subList.off();
				that._$contextMenu.ani({
					style: that.aniConfig.dep2.fadeOut,
					trans: {dur: 150, delay: 75},
					cb: function() {
						that._$contextMenu.removeClass('active');
						if(that._currentSubMenu === that.TMenuList.REMOTERECORDING) {
							that._setRemoteRecording(data.index);
		                } else if (that._currentSubMenu === that.TMenuList.UPDATEPERIOD) {
							that._setUpdatePeriod(data.index);
		                } else if (that._currentSubMenu === that.TMenuList.UPDATEINTERVAL) {
							that._setUpdateInterval(data.index);
		                }
					}
				});
			    break;
        }
        return bConsumed;
    }
    _on_alChangeFocus (aParam) {
		if(aParam.alData.index === 0 && aParam.alData.me === this._remoteRecordMainMenu) {
			this._$noti.html('When the Remote Recording menu is set to On, the receiver can receive the scheduled data that you have scheduled the programmes remotely by using tablet or PC.');
		} else if(aParam.alData.index === 1 && aParam.alData.me === this._remoteRecordMainMenu) {
			this._$noti.html('');
		} else if(aParam.alData.index === 2 && aParam.alData.me === this._remoteRecordMainMenu) {
			this._$noti.html('');
		}
    }
    _on_alExit (aParam) : boolean {
        var bConsumed : boolean = false;
        if (aParam.alData) {
            if (aParam.alData.me === this._remoteRecordMainMenu) {
                bConsumed = true;
                this._gotoSettingPage();
            }
        }
        return bConsumed;
    }
    _on_alKey (aParam) : boolean {
        var that = this, bConsumed : boolean = false;
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
                    if (aParam.alData.me === that._subList) {
                        bConsumed = true;
                        var menuFocusIndex = that._currentSubMenu;
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
                                that._remoteRecordMainMenu.getItem(menuFocusIndex).removeClass('his');
                                that._remoteRecordMainMenu.setItemFocus(menuFocusIndex);
                                that._remoteRecordMainMenu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _contextListMap = //
    [//
        ['LOC_REMOTERECORDING_ID', 'LOC_UPDATE_PERIOD_ID', 'LOC_UPDATE_INTERVAL_ID'],//
        ['LOC_ON_ID', 'LOC_OFF_ID'], //
        ['LOC_SETPERIOD_ID', 'LOC_ALLDAY_ID'],//
        ['LOC_10_MINS_ID', 'LOC_20_MINS_ID', 'LOC_30_MINS_ID', 'LOC_40_MINS_ID', 'LOC_50_MINS_ID']//
    ];

    _makesubList (aParam) {
        var that = this, index : number, subFocusIndex : number;
        var dataList : any;

        if (aParam === that.TMenuList.REMOTERECORDING) {
            index = 1;
            subFocusIndex = (that._isRemoteRecordingOn === true) ? 0 : 1;
        } else if (aParam === that.TMenuList.UPDATEPERIOD) {
            index = 2;
            subFocusIndex = (that._updatePeriod === 'All Day') ? 1 : 0;
        } else if (aParam === that.TMenuList.UPDATEINTERVAL) {
            index = 3;
            subFocusIndex =  that._conv_updateInterval_to_index(that._updateInterval);
        }
        dataList = that._contextListMap[index];

        that._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length
        });
        that._$contextMenu.addClass('active');
        that._remoteRecordMainMenu.getItem(aParam).addClass('his');
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
    _setRemoteRecording (aIndex : number) {
        var that = this, _onConfirm : any;
        if(aIndex === 0) {  //set RemoteRecording on
            if(that._isIpEpgOn === true && that._isRemoteRecordingOn === false) {
                that._isRemoteRecordingOn = true;
                hx.svc.SETTING_UTIL.setEnableRemoteRec(true);
                that._remoteRecordMainMenu.getItem(1).removeClass('dis');
                that._remoteRecordMainMenu.getItem(2).removeClass('dis');
                _showRemoteRecordingSet(that._isRemoteRecordingOn);
            } else if(that._isIpEpgOn === false && that._isRemoteRecordingOn === false) {
                _onConfirm = hx.msg('confirm', {
                    text : hx.l('LOC_SET_REMOTE_RECORDING_ON_ID'),
                    auto_close : false,
                    dia_class : 'osd',
                    btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                    callback_fn : _onCallback
                });
            } else {
                _showRemoteRecordingSet(that._isRemoteRecordingOn);
            }
        } else if(aIndex === 1) { //set RemoteRecording off
            that._isRemoteRecordingOn = false;
            hx.svc.SETTING_UTIL.setEnableRemoteRec(false);
            that._remoteRecordMainMenu.getItem(1).addClass('dis');
            that._remoteRecordMainMenu.getItem(2).addClass('dis');
            _showRemoteRecordingSet(that._isRemoteRecordingOn);
        }

        function _onCallback(_status) {
            if (_status === hx.l('LOC_YES_ID')) {
                that._isIpEpgOn = true;
                hx.svc.SETTING_UTIL.setEnableIPEPG(true);
                that._isRemoteRecordingOn = true;
                hx.svc.SETTING_UTIL.setEnableRemoteRec(true);
                that._remoteRecordMainMenu.getItem(1).removeClass('dis');
                that._remoteRecordMainMenu.getItem(2).removeClass('dis');
                _showRemoteRecordingSet(that._isRemoteRecordingOn);
            } else if (_status === hx.l('LOC_NO_ID') || _status === 'key_back') {
                _showRemoteRecordingSet(that._isRemoteRecordingOn);
            } else if (_status === 'key_esc') {
                that._gotoSettingPage();
            }
        }

        function _showRemoteRecordingSet(isRemoteRecordingOn) {
            var index = (isRemoteRecordingOn) ? 0 : 1;
            var $remoteRecordingSub = that._remoteRecordMainMenu.getItem(0).find('em');
            var remoteRecordingKey : string = hx.l(that._contextListMap[1][index]);
            $remoteRecordingSub.html(remoteRecordingKey);
            that._remoteRecordMainMenu.getItem(0).removeClass('his');
            that._subList.$.remove();
            that._$contextMenu.removeClass('active');
            that._remoteRecordMainMenu.on();
        }
    }
    _setUpdatePeriod (aIndex : number) {
        var that = this, convUpdatePeriod : any = {}, inputPeriod : any;

        if(aIndex === 0) {   //set period
            inputPeriod = hx.msg('inputtime', {
                text : hx.l('LOC_ENTER_START_END_TIME_ID'),
                auto_close : false,
                dia_class : 'osd',
                enable : true,
                callback_fn : periodfn
            });

            function periodfn(_status, _input) {
                if (_status === 'stop') {
                    var periodString : string = _input.substring(0, 2) + ':' + _input.substring(2, 4) + ' - ' + _input.substring(4, 6) + ':' + _input.substring(6, 8);
                    if (_input.substring(0, 2).length !== 2 || _input.substring(2, 4).length !== 2//
                        || _input.substring(4, 6).length !== 2 || _input.substring(6, 8).length !== 2) {
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                        hx.msg('response', {
                            text : "Invalid time values",
                            auto_close : true,
                            close_time : 3000,
                            dia_class : 'res vaTop'
                        });
                    } else if (_input.substring(0, 4) === _input.substring(4, 8)) {
                        // it is same time case..
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                        hx.msg('response', {
                            text : "Invalid time values",
                            auto_close : true,
                            close_time : 3000,
                            dia_class : 'res vaTop'
                        });
                    } else {//if(parseInt(_input.substring(0, 2) + _input.substring(2, 4), 10) < parseInt(_input.substring(4, 6) +
                        // _input.substring(6, 8), 10))
                        var $updatePeriodSub = that._remoteRecordMainMenu.getItem(1).find('em');
                        $updatePeriodSub.html(periodString);
                        convUpdatePeriod = that._conv_updatePeriod_to_secondValueString(periodString);
                        hx.svc.SETTING_UTIL.setUpdatePeriodRemoteRec(convUpdatePeriod);
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                    }
                } else if (_status === 'key_back') {
                    hx.msg.close('pin', inputPeriod);
                    _removeUpdatePeriodList();
                } else if (_status === 'key_esc') {
                    hx.msg.close('pin', inputPeriod);
                    that._gotoSettingPage();
                }
            }
        } else if(aIndex === 1) {
            var $updatePeriodSub = that._remoteRecordMainMenu.getItem(1).find('em');
            var updatePeriodKey : string = hx.l(that._contextListMap[2][1]);
            that._updatePeriod = updatePeriodKey;
            $updatePeriodSub.html(updatePeriodKey);
            hx.svc.SETTING_UTIL.setUpdatePeriodRemoteRec({
                    start_time : 0,
                    end_time : 86400
            });
            _removeUpdatePeriodList();
        }

        function _removeUpdatePeriodList() {
            that._remoteRecordMainMenu.getItem(1).removeClass('his');
            that._subList.$.remove();
            that._$contextMenu.removeClass('active');
            that._remoteRecordMainMenu.on();
            that._remoteRecordMainMenu.setItemFocus(1);
        }
    }
    _setUpdateInterval (aIndex : number) {
        var that = this;
        var $updateIntervalSub = that._remoteRecordMainMenu.getItem(2).find('em');
        var updateIntervalKey : string = hx.l(that._contextListMap[3][aIndex]);
        $updateIntervalSub.html(updateIntervalKey);

        that._updateInterval = that._conv_index_to_updateInterval(aIndex);
        hx.svc.SETTING_UTIL.setUpdateIntervalRemoteRec(that._updateInterval);

        that._remoteRecordMainMenu.getItem(2).removeClass('his');
        that._subList.$.remove();
        that._$contextMenu.removeClass('active');
        that._remoteRecordMainMenu.on();
        that._remoteRecordMainMenu.setItemFocus(2);
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
}
hx.al.CPgRemoteRecord =  cCPgRemoteRecord;

hx.al.CPgRemoteRecord.remoteRecordMainMenu = function(aParam) {
    return hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : 'dep1'
        },
        fx : function(i, o) {
            o.html(hx.l(this.data[i])).append($('<em>'));
        },
        data : aParam,
        count : aParam.length,
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
    });
};

hx.al.CPgRemoteRecord.remoteRecordSubMenu = function(aParam) {
    var subMenu : any = hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : 'dep2'
        },
        data : aParam,
        count : aParam.length,
        fx : function(i, o) {
            o.html(hx.l(this.data[i]));
        },
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
    });
    return subMenu;
};

hx.al.CPgRemoteRecord.prototype.aniConfig = {
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
export = cCPgRemoteRecord;
