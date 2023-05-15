var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    //file name : pages/settings/network_settings/network_server.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgNetworkServer = (function (_super) {
        __extends(cCPgNetworkServer, _super);
        function cCPgNetworkServer() {
            _super.call(this);
            this.TMenuList = {
                NETWORKSERVER: 0,
                RENAMESERVER: 1,
                SERVERSTATUS: 2,
                SERVERACTIVATION: 3
            };
            this.TSubMenuList = {
                SERVERACTIVATION: 0,
                ACTIVATIONTIMEPERIOD: 1
            };
            this.EVENT = {
                START_RESULT: 0,
                STOP_RESULT: 1,
                RENAME_RESULT: 2,
                TEST_CONNECTION_RESULT: 3
            };
            this.RESULTSTATE = {
                FALSE: 0,
                TRUE: 1
            };
            this.TEST_CONNECTION_STATE = {
                NETWORK_UNKNOWN: 100,
                NETWORK_SUCCESS: 200,
                NETWORK_FAIL: 300,
                NETWORK_FAIL_FIREWALL: 400,
                NETWORK_FAIL_NEED_MANUAL_FORWARDING: 500,
                NETWORK_FAIL_AUTO_MANUAL_FORWARDING: 501,
                NETWORK_ERROR: 600,
                NETWORK_ERROR_PUBLIC_IP: 601,
                NETWORK_CHECKING: 900
            };
            this._updateTimer = undefined;
            this._isNetServerOn = false;
            this._serverIpAddress = '';
            this._isChangePageInSetting = false;
            this._isServerRunning = false;
            this._testConResult = '';
            this._contextListMap = [['LOC_NETWORKSERVER_ID', 'LOC_RENAME_SERVER_ID', 'LOC_SERVER_STATUS_ID', 'LOC_SERVER_ACTIVATION_TIME_ID'], ['LOC_ON_ID', 'LOC_OFF_ID'], ['LOC_OFF_ID', 'LOC_SETTIMER_ID'], ['LOC_SERVER_ACTIVATION_TIME_ID', 'LOC_ACTIVATION_TIME_PERIOD_ID']];
        }
        cCPgNetworkServer.prototype.create = function () {
            _super.prototype.create.call(this);
            var that = this;
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _networkMainMenu: hx.al.CPgNetworkServer.networkMainMenu(that._contextListMap[0]),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': hx.l('LOC_NETWORK_SERVER_ON_ID')
                }),
                //server status
                _$setStge: $('<section>', {
                    'id': 'statusStage',
                    'class': 'setStge'
                }),
                _$networkStage: $('<section>', {
                    'id': 'networkStage',
                    'class': 'setStge'
                }),
                _$networkMenu: $('<article>', {
                    'class': 'menu'
                }),
                _$networkSubMenu: $('<section>', {
                    'class': 'dep2'
                }),
                _$serverStatus: $('<h1>', {
                    'html': hx.l('LOC_SERVER_STATUS_ID')
                }),
                _$titleArea: $('<p>', {
                    'class': 'tit area1 point',
                    'html': ''
                }),
                _$box: $('<div>', {
                    'class': 'box type1'
                }),
                _$boxInfo: $('<dl>').append($('<dt>', {
                    'html': hx.l('LOC_DIGITAL_MEDIA_SERVER_ID')
                })).append($('<dd>', {
                    'html': hx.l('LOC_NOT_RUNNING_ID')
                })).append($('<dt>', {
                    'html': hx.l('LOC_DIGITAL_MEDIA_RENDERER_ID')
                })).append($('<dd>', {
                    'html': hx.l('LOC_NOT_RUNNING_ID')
                })),
                _$wrapBtn: $('<div>', {
                    'class': 'wrap_btn'
                }),
                _$btnWrap: $('<div>', {
                    'class': 'wrap_btn'
                }),
                _restartBtn: hx.al.CPgNetworkServer.button('LOC_RESTART_ID'),
                _closeBtn: hx.al.CPgNetworkServer.button('LOC_CLOSE_ID'),
                _okBtn: hx.al.component('button', {
                    attr: {
                        'class': 'button',
                        'data-langID': 'LOC_OK_ID'
                    },
                    usePointerKey: true
                }),
                _cancelBtn: hx.al.component('button', {
                    attr: {
                        'class': 'button',
                        'data-langID': 'LOC_CANCEL_ID'
                    },
                    usePointerKey: true
                })
            });
            that._$setStge.append(that._$serverStatus).append(that._$titleArea).append(that._$box.append(that._$boxInfo)).append(that._$wrapBtn.append(that._restartBtn.$).append(that._closeBtn.$));
            that._$networkStage.append(that._$btnWrap.append(that._okBtn.$).append(that._cancelBtn.$));
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_NETWORK_SERVER_ID');
            }
            that._isNetServerOn = hx.svc.SETTING_UTIL.IsNetServerWork();
            that._serverName = hx.svc.SETTING_UTIL.getServerName();
            that._retObj = hx.svc.WEB_STORAGE.getServerActivationTime();
            if (that._retObj.isOn === null || that._retObj.startHour === null || that._retObj.startMin === null || that._retObj.endHour === null || that._retObj.endMin === null || that._retObj.durationMin === null) {
                that._isServerActivationTime = 'Off';
                that._startHour = '12';
                that._startMin = '00';
                that._endHour = '24';
                that._endMin = '00';
                that._durationMin = '720';
                hx.svc.WEB_STORAGE.setServerActivationTime({
                    startHour: that._startHour,
                    startMin: that._startMin,
                    endHour: that._endHour,
                    endMin: that._endMin,
                    durationMin: that._durationMin,
                    isOn: that._isServerActivationTime
                });
                hx.svc.SETTING_UTIL.setServerActivation({
                    startHour: that._startHour,
                    startMin: that._startMin,
                    endHour: that._endHour,
                    endMin: that._endMin,
                    durationMin: that._durationMin,
                    isOn: that._isServerActivationTime
                });
            }
            else {
                that._isServerActivationTime = that._retObj.isOn;
                that._startHour = that._retObj.startHour;
                that._startMin = that._retObj.startMin;
                that._endHour = that._retObj.endHour;
                that._endMin = that._retObj.endMin;
                that._durationMin = that._retObj.durationMin;
            }
            that._serverActivation = that._isServerActivationTime;
            that._activationTimePeriod = that._startHour + ':' + that._startMin + ' - ' + that._endHour + ':' + that._endMin;
            that._isChangePageInSetting = false;
            //TODO : have to get Server IPAddress
            that._serverIpAddress = '00.000.00.00';
            /*
             that._serverIpAddress = hx.dsvc('dsrvSettingUtil', {
             fname : 'get~~~~'
             });
             */
            that._updateTimer = undefined;
            var $netServerOnOffMenu = that._networkMainMenu.getItem(0).find('em');
            $netServerOnOffMenu.html(hx.l(that._contextListMap[1][((that._isNetServerOn === true) ? 0 : 1)]));
            var $renameServerMenu = that._networkMainMenu.getItem(1).find('em');
            $renameServerMenu.html(that._serverName);
            var $serverActivation = that._networkMainMenu.getItem(3).find('em');
            if (that._serverActivation === 'Off') {
                $serverActivation.html(hx.l(that._contextListMap[2][0]));
            }
            else {
                $serverActivation.html(that._activationTimePeriod);
            }
            that._updateServerStatus();
            that._updateSubMenu();
            hx.svc.SETTING_UTIL.addEventCb('ResultEvent', that, function (aEvt, aState, aServerInfo) {
                that._on_ResultEvent(aEvt, aState, aServerInfo);
            });
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._networkMainMenu.$)).after(that._$noti)).append(that._$setStge).append(that._$networkStage.append(that._$networkMenu.append(that._$networkSubMenu)));
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
                    that.append(that._networkMainMenu).append(that._restartBtn).append(that._closeBtn).append(that._okBtn).append(that._cancelBtn);
                    that._networkMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cCPgNetworkServer.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.SETTING_UTIL.clearEventCb(this);
            this._$menu.remove();
            this._$noti.remove();
            this._$setStge.remove();
            this._$networkStage.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            clearInterval(this._updateTimer);
            this._updateTimer = undefined;
            this._$menu = undefined;
            this._$contextMenu = undefined;
            this._networkMainMenu = undefined;
            this._$noti = undefined;
            this._$setStge = undefined;
            this._$networkStage = undefined;
            this._$serverStatus = undefined;
            this._$titleArea = undefined;
            this._$box = undefined;
            this._$boxInfo = undefined;
            this._$wrapBtn = undefined;
            this._restartBtn = undefined;
            this._closeBtn = undefined;
        };
        cCPgNetworkServer.prototype._on_ResultEvent = function (aEvt, aState, aServerInfo) {
            hx.logadd('ResultEvent');
            hx.log('ResultEvent', 'event state : ' + aState);
            hx.log('ResultEvent', 'event event : ' + aEvt);
            if (aEvt === this.EVENT.START_RESULT) {
                if (aState === this.RESULTSTATE.TRUE) {
                    this._isServerRunning = true;
                }
            }
            else if (aEvt === this.EVENT.STOP_RESULT) {
                if (aState === this.RESULTSTATE.TRUE) {
                    this._isServerRunning = false;
                }
            }
            else if (aEvt === this.EVENT.TEST_CONNECTION_RESULT) {
                this._testConState = aState;
                if (aState === this.TEST_CONNECTION_STATE.NETWORK_UNKNOWN || aState === this.TEST_CONNECTION_STATE.NETWORK_FAIL || aState === this.TEST_CONNECTION_STATE.NETWORK_FAIL_FIREWALL || aState === this.TEST_CONNECTION_STATE.NETWORK_FAIL_NEED_MANUAL_FORWARDING || aState === this.TEST_CONNECTION_STATE.NETWORK_FAIL_AUTO_MANUAL_FORWARDING || aState === this.TEST_CONNECTION_STATE.NETWORK_ERROR || aState === this.TEST_CONNECTION_STATE.NETWORK_ERROR_PUBLIC_IP) {
                    this._testConResult = 'notworking';
                }
                else if (aState === this.TEST_CONNECTION_STATE.NETWORK_SUCCESS || aState === this.TEST_CONNECTION_STATE.NETWORK_CHECKING) {
                    this._testConResult = 'working';
                }
            }
            if (!this._updateTimer) {
                this._updateServerStatus();
                this._updateTimer = setInterval(this._updateServerStatus, 5000);
            }
            else {
                this._updateServerStatus();
            }
        };
        cCPgNetworkServer.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, target = data.me, bConsumed = false;
            var yesBtn, noBtn;
            var msgbox;
            var retObj = null;
            var menuFocusIndex = 0;
            switch (target) {
                case that._networkMainMenu:
                    bConsumed = true;
                    if (data.index === that.TMenuList.NETWORKSERVER) {
                        that._networkMainMenu.off();
                        that._makesubList(data.index);
                    }
                    else if (data.index === that.TMenuList.RENAMESERVER) {
                        yesBtn = hx.l('LOC_YES_ID');
                        noBtn = hx.l('LOC_NO_ID');
                        msgbox = hx.msg('confirm', {
                            text: hx.l('LOC_SERVER_RESTART_CONTINUE_ID'),
                            auto_close: false,
                            // timeout : 5000,
                            btn_title_arr: [yesBtn, noBtn],
                            dia_class: 'osd',
                            btn_last_focus: 1,
                            callback_fn: function (p) {
                                if (p === yesBtn) {
                                    hx.msg.close('confirm', msgbox);
                                    var passwdTmp = hx.msg('passwd', {
                                        text: hx.l('LOC_ENTER_SERVER_NAME_ID'),
                                        responseText: hx.l('LOC_TYPE_NAME_PRESS_ENTER_ID'),
                                        wifi: 'Name',
                                        string: hx.l('LOC_STR_SERVER_NAME_01_ID'),
                                        auto_close: false,
                                        dia_class: 'osd',
                                        callback_fn: fn
                                    });
                                    function fn(_status, _input) {
                                        if (_status === 'key_back' || _status === 'key_esc') {
                                            hx.msg.close('passwd', passwdTmp);
                                            that._networkMainMenu.setItemFocus(1);
                                        }
                                        else if (_status === hx.l('LOC_OK_ID')) {
                                            if (_input) {
                                                if (_input.password !== '') {
                                                    that._status = "connecting";
                                                    hx.svc.SETTING_UTIL.renameServer({
                                                        serverName: _input.password
                                                    });
                                                    that._networkMainMenu.getItem(1).find('em').html(_input.password);
                                                }
                                            }
                                            hx.msg.close('passwd', passwdTmp);
                                            that._networkMainMenu.setItemFocus(1);
                                        }
                                        else if (_status === 'close') {
                                            that._networkMainMenu.setItemFocus(1);
                                        }
                                    }
                                }
                                else if (p === noBtn) {
                                    hx.msg.close('confirm', msgbox);
                                    that._networkMainMenu.setItemFocus(1);
                                }
                                else if (p === 'key_back') {
                                    hx.msg.close('confirm', msgbox);
                                    that._networkMainMenu.setItemFocus(1);
                                }
                                else if (p === 'key_esc') {
                                    hx.msg.close('confirm', msgbox);
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
                        });
                    }
                    else if (data.index === that.TMenuList.SERVERSTATUS) {
                        that._updateServerStatus();
                        that._networkMainMenu.off();
                        that._$setStge.addClass('active');
                        that._restartBtn.$.addClass('on');
                        that._$setStge.ani({
                            style: that.aniConfig.channelMenu.fadeIn,
                            trans: {
                                dur: 200,
                                delay: 100
                            },
                            cb: function () {
                                that._restartBtn.on();
                            }
                        });
                    }
                    else if (data.index === that.TMenuList.SERVERACTIVATION) {
                        that._networkMainMenu.off();
                        that._makesubList(data.index);
                    }
                    that._currentSubMenu = data.index;
                    break;
                case that._onTimerMainMenu:
                    if (data.index === that.TSubMenuList.SERVERACTIVATION) {
                        that._onTimerMainMenu.off();
                        that._makeonoffsubList(data.index);
                        that._currentServerSubMenu = data.index;
                    }
                    else if (data.index === that.TSubMenuList.ACTIVATIONTIMEPERIOD) {
                        that._onTimerMainMenu.off();
                        that._setUpdatePeriod();
                        that._currentServerSubMenu = data.index;
                    }
                    break;
                case that._subList:
                    bConsumed = true;
                    that._subList.off();
                    that._$contextMenu.ani({
                        style: that.aniConfig.dep2.fadeOut,
                        trans: {
                            dur: 150,
                            delay: 75
                        },
                        cb: function () {
                            if (that._currentSubMenu === that.TMenuList.NETWORKSERVER) {
                                that._setNetServer(data.index);
                            }
                            else if (that._currentSubMenu === that.TMenuList.SERVERACTIVATION) {
                                that._setActivationTime(data.index);
                            }
                        }
                    });
                    break;
                case that._activationSubList:
                    that._setSubPowerOffTimer(data.index);
                    that._activationSubList.off();
                    $('#set #networkStage .menu .dep2').ani({
                        style: that.aniConfig.dep2.fadeOut,
                        trans: {
                            dur: 150,
                            delay: 75
                        },
                        cb: function () {
                            menuFocusIndex = that._currentServerSubMenu;
                            that._activationSubList.$.remove();
                            that._$networkSubMenu.removeClass('active');
                            that._onTimerMainMenu.getItem(menuFocusIndex).removeClass('his');
                            that._onTimerMainMenu.setItemFocus(menuFocusIndex);
                            that._onTimerMainMenu.on();
                        }
                    });
                    break;
                case that._restartBtn:
                    bConsumed = true;
                    that._isNetServerOn = true;
                    that.ProgressMsg = hx.msg('progress', {
                        text: hx.l('LOC_RESTARTING_SERVER_ID'),
                        auto_close: true,
                        timeout: 8000
                    });
                    hx.svc.SETTING_UTIL.controlNetServer(false);
                    hx.svc.SETTING_UTIL.controlNetServer(true);
                    //hx.svc.SETTING_UTIL.testConnection();
                    that._updateServerStatus();
                    that._networkMainMenu.getItem(0).find('em').html(hx.l(that._contextListMap[1][0]));
                    break;
                case that._closeBtn:
                    bConsumed = true;
                    that._closeBtn.off();
                    that._closeBtn.$.removeClass('on');
                    that._networkMainMenu.setItemFocus(2);
                    that._$setStge.ani({
                        style: that.aniConfig.channelMenu.fadeOut,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            that._$setStge.removeClass('active');
                            that._networkMainMenu.on();
                        }
                    });
                    break;
                case that._okBtn:
                    that._updateServerActivationTime();
                    menuFocusIndex = that._currentSubMenu;
                    that._okBtn.$.removeClass('on');
                    that._cancelBtn.$.removeClass('on');
                    that._$networkStage.removeClass('active');
                    that._onTimerMainMenu.$.remove();
                    that._title.$.remove();
                    that._networkMainMenu.getItem(menuFocusIndex).removeClass('his');
                    that._networkMainMenu.setItemFocus(menuFocusIndex);
                    that._networkMainMenu.on();
                    bConsumed = true;
                    break;
                case that._cancelBtn:
                    bConsumed = true;
                    retObj = hx.svc.WEB_STORAGE.getServerActivationTime();
                    that._startHour = retObj.startHour;
                    that._startMin = retObj.startMin;
                    that._endHour = retObj.endHour;
                    that._endMin = retObj.endMin;
                    that._isServerActivationTime = retObj.isOn;
                    menuFocusIndex = that._currentSubMenu;
                    that._okBtn.$.removeClass('on');
                    that._cancelBtn.$.removeClass('on');
                    that._$networkStage.removeClass('active');
                    that._onTimerMainMenu.$.remove();
                    that._title.$.remove();
                    if (that._isServerActivationTime === 'On') {
                        that._networkMainMenu.getItem(3).find('em').html(that._startHour + ':' + that._startMin + ' - ' + that._endHour + ':' + that._endMin);
                    }
                    else {
                        that._networkMainMenu.getItem(3).find('em').html(hx.l(that._contextListMap[2][0]));
                    }
                    that._networkMainMenu.getItem(menuFocusIndex).removeClass('his');
                    that._networkMainMenu.setItemFocus(menuFocusIndex);
                    that._networkMainMenu.on();
                    break;
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._on_alChangeFocus = function (aParam) {
            if (aParam.alData.index === 0 && aParam.alData.me === this._networkMainMenu) {
                this._$noti.html(hx.l('LOC_NETWORK_SERVER_ON_ID'));
            }
            else if (aParam.alData.index === 1 && aParam.alData.me === this._networkMainMenu) {
                this._$noti.html('');
            }
            else if (aParam.alData.index === 2 && aParam.alData.me === this._networkMainMenu) {
                this._$noti.html('');
            }
            else if (aParam.alData.index === 3 && aParam.alData.me === this._networkMainMenu) {
                this._$noti.html(hx.l('LOC_SERVER_ON_REMAINS_STANDBY_MODE_ID'));
            }
        };
        cCPgNetworkServer.prototype._conv_updatePeriod_to_secondValueString = function (aUpdatePeriod) {
            var startHour = parseInt(aUpdatePeriod.substring(0, 2), 10);
            var startMinute = parseInt(aUpdatePeriod.substring(3, 5), 10);
            var endHour = parseInt(aUpdatePeriod.substring(8, 10), 10);
            var endMinute = parseInt(aUpdatePeriod.substring(11, 13), 10);
            var durationMin;
            if ((endHour * 60 + endMinute) - (startHour * 60 + startMinute) < 0) {
                durationMin = ((endHour + 24) * 60 + endMinute) - (startHour * 60 + startMinute);
            }
            else if ((endHour * 60 + endMinute) - (startHour * 60 + startMinute) > 0) {
                durationMin = (endHour * 60 + endMinute) - (startHour * 60 + startMinute);
            }
            this._startHour = aUpdatePeriod.substring(0, 2);
            this._startMin = aUpdatePeriod.substring(3, 5);
            this._endHour = aUpdatePeriod.substring(8, 10);
            this._endMin = aUpdatePeriod.substring(11, 13);
            this._durationMin = durationMin.toString();
        };
        cCPgNetworkServer.prototype._on_alExit = function (aParam) {
            var that = this, bConsumed = false;
            var menuFocusIndex;
            var retObj = null;
            if (aParam.alData) {
                if (aParam.alData.me === that._networkMainMenu) {
                    bConsumed = true;
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
                else if (aParam.alData.me === that._restartBtn) {
                    bConsumed = true;
                    that._restartBtn.off();
                    that._restartBtn.$.removeClass('on');
                    that._networkMainMenu.setItemFocus(2);
                    that._$setStge.ani({
                        style: that.aniConfig.channelMenu.fadeOut,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            that._$setStge.removeClass('active');
                            that._networkMainMenu.on();
                        }
                    });
                }
                else if (aParam.alData.me === that._closeBtn) {
                    bConsumed = true;
                    that._closeBtn.off();
                    that._closeBtn.$.removeClass('on');
                    that._networkMainMenu.setItemFocus(2);
                    that._$setStge.ani({
                        style: that.aniConfig.channelMenu.fadeOut,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            that._$setStge.removeClass('active');
                            that._networkMainMenu.on();
                        }
                    });
                }
                else if (aParam.alData.me === that._onTimerMainMenu || aParam.alData.me === that._okBtn || aParam.alData.me === that._cancelBtn) {
                    menuFocusIndex = that._currentSubMenu;
                    that._okBtn.$.removeClass('on');
                    that._cancelBtn.$.removeClass('on');
                    that._$networkStage.removeClass('active');
                    that._onTimerMainMenu.$.remove();
                    that._title.$.remove();
                    retObj = hx.svc.WEB_STORAGE.getServerActivationTime();
                    that._startHour = retObj.startHour;
                    that._startMin = retObj.startMin;
                    that._endHour = retObj.endHour;
                    that._endMin = retObj.endMin;
                    that._durationMin = retObj.durationMin;
                    that._isServerActivationTime = retObj.isOn;
                    if (that._isServerActivationTime === 'On') {
                        that._networkMainMenu.getItem(3).find('em').html(that._startHour + ':' + that._startMin + ' - ' + that._endHour + ':' + that._endMin);
                    }
                    else {
                        that._networkMainMenu.getItem(3).find('em').html(hx.l(that._contextListMap[2][0]));
                    }
                    that._networkMainMenu.getItem(menuFocusIndex).removeClass('his');
                    that._networkMainMenu.setItemFocus(menuFocusIndex);
                    that._networkMainMenu.on();
                }
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._on_alKey = function (aParam) {
            var that = this, bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
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
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me === that._subList) {
                            that._subList.off();
                            that._$contextMenu.ani({
                                style: that.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    var menuFocusIndex = that._currentSubMenu;
                                    that._subList.$.remove();
                                    that._$contextMenu.removeClass('active');
                                    that._networkMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    that._networkMainMenu.setItemFocus(menuFocusIndex);
                                    that._networkMainMenu.on();
                                }
                            });
                        }
                        else if (aParam.alData.me === that._activationSubList) {
                            that._activationSubList.off();
                            $('#set #networkStage .menu .dep2').ani({
                                style: that.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    var menuFocusIndex = that._currentServerSubMenu;
                                    that._activationSubList.$.remove();
                                    that._$networkSubMenu.removeClass('active');
                                    that._onTimerMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    that._onTimerMainMenu.setItemFocus(menuFocusIndex);
                                    that._onTimerMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._restartBtn:
                    bConsumed = true;
                    this._closeBtn.on();
                    this._restartBtn.$.removeClass('on');
                    this._closeBtn.$.addClass('on');
                    break;
                case this._closeBtn:
                    bConsumed = true;
                    this._restartBtn.on();
                    this._restartBtn.$.addClass('on');
                    this._closeBtn.$.removeClass('on');
                    break;
            }
            if (target === this._okBtn) {
                bConsumed = true;
                this._okBtn.$.removeClass('on');
                this._cancelBtn.on();
                this._cancelBtn.$.addClass('on');
            }
            else if (target === this._cancelBtn) {
                bConsumed = true;
                this._cancelBtn.$.removeClass('on');
                this._okBtn.on();
                this._okBtn.$.addClass('on');
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._on_alScrollLeft = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._restartBtn:
                    bConsumed = true;
                    this._closeBtn.on();
                    this._restartBtn.$.removeClass('on');
                    this._closeBtn.$.addClass('on');
                    break;
                case this._closeBtn:
                    bConsumed = true;
                    this._restartBtn.on();
                    this._restartBtn.$.addClass('on');
                    this._closeBtn.$.removeClass('on');
                    break;
            }
            if (target === this._okBtn) {
                bConsumed = true;
                this._okBtn.$.removeClass('on');
                this._cancelBtn.on();
                this._cancelBtn.$.addClass('on');
            }
            else if (target === this._cancelBtn) {
                bConsumed = true;
                this._cancelBtn.$.removeClass('on');
                this._okBtn.on();
                this._okBtn.$.addClass('on');
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._on_alScrollup = function (aParam) {
            var that = this, bConsumed = false;
            var cmpt;
            if (aParam.alData.me === that._okBtn || aParam.alData.me === that._cancelBtn) {
                bConsumed = true;
                if (aParam.alData.me === that._okBtn) {
                    that._okBtn.$.removeClass('on');
                }
                else if (aParam.alData.me === that._cancelBtn) {
                    that._cancelBtn.$.removeClass('on');
                }
                if (that._isServerActivationTime === 'On') {
                    that._onTimerMainMenu.setItemFocus(1);
                }
                else {
                    that._onTimerMainMenu.setItemFocus(0);
                }
                that._onTimerMainMenu.on();
            }
            else if (aParam.alData.me === that._onTimerMainMenu) {
                bConsumed = true;
                that._okBtn.on();
                that._okBtn.$.addClass('on');
            }
            else if (aParam.alData.me === that._subList) {
                bConsumed = true;
                cmpt = that._subList;
                that._subMenuScrollup(cmpt);
            }
            else if (aParam.alData.me === that._activationSubList) {
                bConsumed = true;
                cmpt = that._activationSubList;
                that._subMenuScrollup(cmpt);
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._on_alScrolldown = function (aParam) {
            var that = this, bConsumed = false;
            var cmpt;
            if (aParam.alData.me === that._onTimerMainMenu) {
                bConsumed = true;
                that._okBtn.on();
                that._okBtn.$.addClass('on');
            }
            else if (aParam.alData.me === that._okBtn || aParam.alData.me === that._cancelBtn) {
                bConsumed = true;
                if (aParam.alData.me === that._okBtn) {
                    that._okBtn.$.removeClass('on');
                }
                else if (aParam.alData.me === that._cancelBtn) {
                    that._cancelBtn.$.removeClass('on');
                }
                that._onTimerMainMenu.on();
            }
            else if (aParam.alData.me === that._subList) {
                bConsumed = true;
                cmpt = that._subList;
                that._subMenuScrolldown(cmpt);
            }
            else if (aParam.alData.me === that._activationSubList) {
                bConsumed = true;
                cmpt = that._activationSubList;
                that._subMenuScrollup(cmpt);
            }
            return bConsumed;
        };
        cCPgNetworkServer.prototype._temporyCode = function (aParam) {
            var that = this, tmp = hx.msg('confirm', {
                text: 'Not yet supported',
                auto_close: false,
                btn_title_arr: [hx.l('LOC_OK_ID')],
                callback_fn: fn,
                dia_class: 'osd'
            });
            function fn(_status) {
                if (_status === hx.l('LOC_OK_ID')) {
                    that._networkMainMenu.on();
                    that._networkMainMenu.setItemFocus(that._currentSubMenu);
                }
                else if (_status === 'key_back') {
                    that._networkMainMenu.on();
                    that._networkMainMenu.setItemFocus(that._currentSubMenu);
                }
                else if (_status === 'key_esc') {
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
        cCPgNetworkServer.prototype._makesubList = function (aParam) {
            var _this = this;
            var index, subFocusIndex;
            var topPx;
            var dataList;
            if (aParam === this.TMenuList.NETWORKSERVER) {
                index = 1;
                subFocusIndex = (this._isNetServerOn === true) ? 0 : 1;
                topPx = undefined;
            }
            else if (aParam === this.TMenuList.SERVERACTIVATION) {
                index = 2;
                subFocusIndex = 0;
                topPx = '255px';
            }
            dataList = this._contextListMap[index];
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
            this._networkMainMenu.getItem(aParam).addClass('his');
            this._$contextMenu.append(this._subList.$);
            this._subList.bind(dataList);
            this._subList.draw();
            this._$contextMenu.ani({
                style: this.aniConfig.dep2.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    _this.append(_this._subList);
                    _this._subList.on();
                }
            });
        };
        cCPgNetworkServer.prototype._makeonoffsubList = function (aParam) {
            var _this = this;
            var index, subFocusIndex;
            var dataList;
            if (aParam === this.TSubMenuList.SERVERACTIVATION) {
                index = 1;
                subFocusIndex = (this._isServerActivationTime === 'On') ? 0 : 1;
            }
            dataList = this._contextListMap[index];
            this._activationSubList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            this._$networkSubMenu.addClass('active');
            this._onTimerMainMenu.getItem(aParam).addClass('his');
            this._$networkSubMenu.append(this._activationSubList.$);
            this._activationSubList.bind(dataList);
            this._activationSubList.draw();
            $('#set #networkStage .menu .dep2').ani({
                style: this.aniConfig.dep2.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    _this.append(_this._activationSubList);
                    _this._activationSubList.on();
                }
            });
        };
        cCPgNetworkServer.prototype._setUpdatePeriod = function () {
            var that = this, convUpdatePeriod = {};
            var MinsOf24Hours = 60 * 24;
            var MinsOf12Hours = 60 * 12;
            var inputPeriod = hx.msg('inputtime', {
                text: hx.l('LOC_ENTER_START_END_TIME_ID'),
                auto_close: false,
                dia_class: 'osd',
                enable: true,
                callback_fn: periodfn
            });
            function periodfn(_status, _input) {
                var strInvalidValues = hx.l('LOC_INVALID_VALUE_ID');
                // String Not Exist.
                if (_status === 'stop') {
                    var periodString = _input.substring(0, 2) + ':' + _input.substring(2, 4) + ' - ' + _input.substring(4, 6) + ':' + _input.substring(6, 8);
                    if (_input.substring(0, 2).length !== 2 || _input.substring(2, 4).length !== 2 || _input.substring(4, 6).length !== 2 || _input.substring(6, 8).length !== 2) {
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                        hx.msg('response', {
                            text: strInvalidValues,
                            auto_close: true,
                            close_time: 3000,
                            dia_class: 'res vaTop'
                        });
                    }
                    else if (_calculateTime(_input) === 0) {
                        // it is same time case..
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                        hx.msg('response', {
                            text: strInvalidValues,
                            auto_close: true,
                            close_time: 3000,
                            dia_class: 'res vaTop'
                        });
                    }
                    else if (_calculateTime(_input) > MinsOf24Hours) {
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                        hx.msg.close('response', that.responseDlg);
                        that.responseDlg = hx.msg('response', {
                            text: hx.l('LOC_INVALID_INPUT_LESS_24_HOURS_ID'),
                            auto_close: true,
                            close_time: 3000,
                            dia_class: 'res vaTop'
                        });
                    }
                    else {
                        var $updatePeriodSub = that._onTimerMainMenu.getItem(1).find('em');
                        $updatePeriodSub.html(periodString);
                        convUpdatePeriod = that._conv_updatePeriod_to_secondValueString(periodString);
                        hx.msg.close('inputtime', inputPeriod);
                        _removeUpdatePeriodList();
                    }
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', inputPeriod);
                    _removeUpdatePeriodList();
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', inputPeriod);
                    that._isChangePageInSetting = true;
                    hx.al.decompose(that);
                    $('#set .noti').ani({
                        style: that.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        }
                    });
                    $('#set article.menu').ani({
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
            function _removeUpdatePeriodList() {
                that._onTimerMainMenu.getItem(1).removeClass('his');
                that._subList.$.remove();
                that._$contextMenu.removeClass('active');
                that._onTimerMainMenu.on();
                that._onTimerMainMenu.setItemFocus(1);
            }
            function _calculateTime(_input) {
                var calStartMin = parseInt(_input.substring(0, 2), 10) * 60 + parseInt(_input.substring(2, 4), 10);
                var calEndMin = parseInt(_input.substring(4, 6), 10) * 60 + parseInt(_input.substring(6, 8), 10);
                if ((parseInt(calEndMin, 10) - parseInt(calStartMin, 10)) < 0) {
                    calEndMin = (parseInt(_input.substring(4, 6), 10) + 24) * 60 + parseInt(_input.substring(6, 8), 10);
                }
                //if ((parseInt(calEndMin, 10) - parseInt(calStartMin, 10)) === 1440) {
                //    return 0;
                //}
                return parseInt(calEndMin, 10) - parseInt(calStartMin, 10);
            }
        };
        cCPgNetworkServer.prototype._setNetServer = function (aIndex) {
            var that = this;
            var $netServerSub = this._networkMainMenu.getItem(0).find('em');
            var netServerKey = hx.l(this._contextListMap[1][aIndex]);
            var bCommand;
            $netServerSub.html(netServerKey);
            bCommand = (aIndex === 0) ? true : false;
            if (that._isNetServerOn !== bCommand) {
                that._isNetServerOn = (aIndex === 0) ? true : false;
                hx.svc.SETTING_UTIL.controlNetServer(bCommand);
                that.ProgressMsg = hx.msg('progress', {
                    text: hx.l('LOC_PLEASE_WAIT_ID'),
                    auto_close: true,
                    timeout: 5000
                });
                that._updateServerStatus();
                that._updateSubMenu();
                that._updateServerActivationTime(bCommand);
            }
            that._networkMainMenu.getItem(0).removeClass('his');
            that._subList.$.remove();
            that._$contextMenu.removeClass('active');
            that._networkMainMenu.on();
        };
        cCPgNetworkServer.prototype._updateServerStatus = function () {
            var that = this;
            hx.log('ResultEvent', 'Update Server Status - isServerRunning : ' + that._isServerRunning);
            hx.log('ResultEvent', 'Update Server Status - testConResult : ' + that._testConResult);
            that._isNetServerOn = hx.svc.SETTING_UTIL.IsNetServerWork();
            if (that._isNetServerOn) {
                that._isServerRunning = true;
                that._$titleArea.html(hx.l('LOC_SERVER_WORKING_CORRECTLY_ID'));
                that._$boxInfo.find('dd').eq(0).html(hx.l('LOC_RUNNING_ID'));
                that._$boxInfo.find('dd').eq(1).html(hx.l('LOC_RUNNING_ID'));
            }
            else {
                that._isServerRunning = false;
                that._$titleArea.html('[N300] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                that._$boxInfo.find('dd').eq(0).html(hx.l('LOC_NOT_RUNNING_ID'));
                that._$boxInfo.find('dd').eq(1).html(hx.l('LOC_NOT_RUNNING_ID'));
            }
            //Test Connection Result
            if (that._testConResult === 'notworking') {
                switch (that._testConState) {
                    case that.TEST_CONNECTION_STATE.NETWORK_UNKNOWN:
                        that._$titleArea.html('[N100] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                        break;
                    case that.TEST_CONNECTION_STATE.NETWORK_FAIL:
                        that._$titleArea.html('[N300] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                        break;
                    case that.TEST_CONNECTION_STATE.NETWORK_FAIL_FIREWALL:
                        that._$titleArea.html('[N400] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                        break;
                    case that.TEST_CONNECTION_STATE.NETWORK_FAIL_NEED_MANUAL_FORWARDING:
                        that._$titleArea.html('[N500] ' + hx.l('LOC_SERVER_NOT_ACCESSED_ID'));
                        break;
                    case that.TEST_CONNECTION_STATE.NETWORK_FAIL_AUTO_MANUAL_FORWARDING:
                        that._$titleArea.html('[N501] ' + hx.l('LOC_SERVER_NOT_ACCESSED_ID'));
                        break;
                    case that.TEST_CONNECTION_STATE.NETWORK_ERROR:
                        that._$titleArea.html('[N600] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                        break;
                    case that.TEST_CONNECTION_STATE.NETWORK_ERROR_PUBLIC_IP:
                        that._$titleArea.html('[N601] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                        break;
                    default:
                        that._$titleArea.html('[' + hx.l('LOC_UNKNOWN_ID') + '] ' + hx.l('LOC_SOME_NOT_WORKING_ID'));
                }
            }
            else if (that._testConResult === 'working') {
                that._$titleArea.html(hx.l('LOC_SERVER_WORKING_CORRECTLY_ID'));
            }
        };
        cCPgNetworkServer.prototype._setActivationTime = function (aIndex) {
            var that = this;
            var $serverAtivationTimeSub = that._networkMainMenu.getItem(3).find('em');
            var activationOffTimerKey = hx.l(that._contextListMap[2][aIndex]);
            if (aIndex === 0) {
                $serverAtivationTimeSub.html(activationOffTimerKey);
            }
            that._networkMainMenu.getItem(3).removeClass('his');
            if (that._subList) {
                that._subList.$.remove();
            }
            that._$contextMenu.removeClass('active');
            if (aIndex === 0) {
                that._isServerActivationTime = 'Off';
                hx.svc.WEB_STORAGE.setServerActivationTime({
                    startHour: that._startHour,
                    startMin: that._startMin,
                    endHour: that._endHour,
                    endMin: that._endMin,
                    durationMin: that._durationMin,
                    isOn: that._isServerActivationTime
                });
                hx.svc.SETTING_UTIL.setServerActivation({
                    startHour: that._startHour,
                    startMin: that._startMin,
                    endHour: that._endHour,
                    endMin: that._endMin,
                    durationMin: that._durationMin,
                    isOn: that._isServerActivationTime
                });
                that._networkMainMenu.on();
                that._networkMainMenu.setItemFocus(3);
            }
            else if (aIndex === 1) {
                var retObj = hx.svc.WEB_STORAGE.getServerActivationTime();
                that._startHour = retObj.startHour;
                that._startMin = retObj.startMin;
                that._endHour = retObj.endHour;
                that._endMin = retObj.endMin;
                that._isServerActivationTime = retObj.isOn;
                that._$networkStage.addClass('active');
                that._onTimerMainMenu = hx.al.CPgNetworkServer.powerTimerMainMenu(that._contextListMap[3], 'dep1', 'notroll');
                that._onTimerMainMenu.getItem(0).find('em').html(that._isServerActivationTime === 'On' ? hx.l(that._contextListMap[1][0]) : hx.l(that._contextListMap[1][1]));
                if (that._isServerActivationTime === 'On') {
                    that._onTimerMainMenu.getItem(1).removeClass('dis');
                }
                else {
                    that._onTimerMainMenu.getItem(1).addClass('dis');
                }
                if (that._startHour !== null && that._startMin !== null && that._endHour !== null && that._endMin !== null) {
                    that._onTimerMainMenu.getItem(1).find('em').html(that._startHour + ':' + that._startMin + ' - ' + that._endHour + ':' + that._endMin);
                }
                else {
                    that._onTimerMainMenu.getItem(1).find('em').html(that._activationTimePeriod);
                }
                that._title = hx.al.CPgNetworkServer.title('LOC_SERVER_ACTIVATION_TIME_ID');
                that._$networkSubMenu.after(that._onTimerMainMenu.$);
                that._$networkStage.prepend(that._title.$);
                that.append(that._onTimerMainMenu);
                that._okBtn.$.blur();
                that._cancelBtn.$.blur();
                that._onTimerMainMenu.on();
            }
        };
        cCPgNetworkServer.prototype._setSubPowerOffTimer = function (aIndex) {
            var that = this;
            var $offTimerSub = that._onTimerMainMenu.getItem(0).find('em');
            var offTimerKey = hx.l(that._contextListMap[1][aIndex]);
            $offTimerSub.html(offTimerKey);
            that._activationSubList.$.remove();
            if (aIndex === 1) {
                that._isServerActivationTime = 'Off';
                that._onTimerMainMenu.getItem(1).addClass('dis');
                that._onTimerMainMenu.on();
            }
            else if (aIndex === 0) {
                that._isServerActivationTime = 'On';
                that._onTimerMainMenu.getItem(1).find('em').html(that._startHour + ':' + that._startMin + ' - ' + that._endHour + ':' + that._endMin);
                that._onTimerMainMenu.getItem(1).removeClass('dis');
                that._onTimerMainMenu.on();
            }
        };
        cCPgNetworkServer.prototype._updateServerActivationTime = function (bEnable) {
            var that = this;
            if (bEnable === true) {
                that._isServerActivationTime = 'On';
            }
            else if (bEnable === false) {
                that._isServerActivationTime = 'Off';
            }
            hx.svc.WEB_STORAGE.setServerActivationTime({
                startHour: that._startHour,
                startMin: that._startMin,
                endHour: that._endHour,
                endMin: that._endMin,
                durationMin: that._durationMin,
                isOn: that._isServerActivationTime
            });
            hx.svc.SETTING_UTIL.setServerActivation({
                startHour: that._startHour,
                startMin: that._startMin,
                endHour: that._endHour,
                endMin: that._endMin,
                durationMin: that._durationMin,
                isOn: that._isServerActivationTime
            });
            if (that._isServerActivationTime === 'On') {
                that._networkMainMenu.getItem(3).find('em').html(that._startHour + ':' + that._startMin + ' - ' + that._endHour + ':' + that._endMin);
            }
            else {
                that._networkMainMenu.getItem(3).find('em').html(hx.l(that._contextListMap[2][0]));
            }
        };
        cCPgNetworkServer.prototype._updateSubMenu = function () {
            var that = this;
            var i = 0;
            for (i = that.TMenuList.RENAMESERVER; i <= that.TMenuList.SERVERACTIVATION; i += 1) {
                if (that._isNetServerOn) {
                    this._networkMainMenu.getItem(i).removeClass('dis');
                }
                else {
                    this._networkMainMenu.getItem(i).addClass('dis');
                }
            }
        };
        return cCPgNetworkServer;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgNetworkServer = cCPgNetworkServer;
    hx.al.CPgNetworkServer.powerTimerMainMenu = function (aParam, aParam2, aParam3) {
        var that = this;
        var powerTimermainmenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': aParam2
            },
            roll: aParam3,
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return powerTimermainmenu;
    };
    hx.al.CPgNetworkServer.title = function (aParam) {
        return hx.al.component('text', {
            tag: 'h1',
            attr: {
                'data-langID': aParam
            }
        });
    };
    hx.al.CPgNetworkServer.networkMainMenu = function (aParam) {
        var networkMainMenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep1'
            },
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.append($('<b>', {
                    'class': 'item',
                    'html': hx.l(this.data[i])
                })).append($('<em>', {
                    'class': 'value',
                    'html': ''
                }));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return networkMainMenu;
    };
    hx.al.CPgNetworkServer.networkSubMenu = function (aParam) {
        var networkSubMenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep2'
            },
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i]));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
        });
        return networkSubMenu;
    };
    hx.al.CPgNetworkServer.button = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': aParam
            },
            usePointerKey: true
        }));
    };
    hx.al.CPgNetworkServer.prototype.aniConfig = {
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
        channelMenu: {
            fadeIn: {
                start: {
                    'opacity': 0,
                    'top': '20px'
                },
                end: {
                    'opacity': 1,
                    'top': 0
                }
            },
            fadeOut: {
                start: {
                    'opacity': 1,
                    'top': 0
                },
                end: {
                    'opacity': 0,
                    'top': '20px'
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
    return cCPgNetworkServer;
});
