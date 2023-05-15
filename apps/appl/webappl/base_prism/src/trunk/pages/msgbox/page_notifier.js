///<reference path="../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __NETFLIX__) {
    /* file name : pages/msgbox/notifier.js*/
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgNotifier = (function (_super) {
        __extends(cCPgNotifier, _super);
        function cCPgNotifier(aPageName) {
            _super.call(this, aPageName);
            this.HMXPhysicalStorageInterface = {
                EInterfaceUnknown: 0,
                EInterfaceSATA: 1,
                EInterfaceUSB: 2
            };
            this.HMXPhysicalStorageType = {
                ETypeUnknown: 0,
                ETypeInternal: 1,
                ETypeExternal: 2
            };
            this.HMXPhysicalStorageKind = {
                EKindUnknown: 0,
                EKindHDD: 1,
                EKindUSBMemory: 2,
                EKindSDMemory: 3
            };
            this.HMXPhysicalStorageUsage = {
                EUsageUnknown: 0,
                EUsagePVR: 1,
                EUsageSubPVR: 2,
                EUsageStorage: 3
            };
            this.PAIRINGSTATE = {
                EPairingUnkown: 0,
                EPairingOK: 1,
                EPairingFail: 2,
                EPairingNoInfo: 3,
                EPairingNow: 4
            };
            this.SESSION_EVENT_TYPE = {
                NONE: 0,
                REQUESTSESSIONSTART: 1,
                REQUESTSESSIONSTOP: 2,
                SESSIONSTARTED: 3,
                SESSIONSTOPPED: 4,
                DISPLAYUI: 5,
                MAX: 6
            };
            this._powerDownDlg = null;
            this._powerOffDlg = null;
            this._isVisible = false;
            this._bDraw = true;
            this._opName = null;
            this._bStatusUpdate = false;
            this._casEventType = ["YES", "NO", "FAIL"];
            this._OvercurrentEventTimer = null;
            this._Overcurrentmsg = -1;
            this._CISessionKey = 'CI_MENU';
            this._CIBannerSessionKey = 'CI_BANNER';
            this.createKeyMap();
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            dsrvSettingUtil.addEventCb('NetworkAvailableChanged', this, function (networkAvailable) {
                var notificationDlg, closedParam = {};
                hx.log('info', 'NetworkAvailableChanged ::' + networkAvailable);
                if (networkAvailable) {
                    notificationDlg = prism.createNotificationDlg(null, hx.l('LOC_NET_CONNECTED_ID'), { validOnNetflix: true });
                }
                else {
                    notificationDlg = prism.createNotificationDlg(null, hx.l('LOC_NET_DISCONNECTED_ID'), { validOnNetflix: true });
                }
                closedParam.onClosed = function () {
                    if (hx.config.netflix) {
                        if (hx.svc.NETFLIX.isRunning()) {
                            hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                        }
                    }
                };
                if (!notificationDlg) {
                    hx.log('error', 'Can not create dialog. ');
                    return;
                }
                notificationDlg.open(closedParam);
            });
        }
        cCPgNotifier.prototype.create = function () {
            hx.al.compose(this);
            this._initService();
            hx.logadd('CPgNotifier');
            this._registerPowerOffTimer();
        };
        cCPgNotifier.prototype.createKeyMap = function () {
            this._keyTable = {};
            this._keyRTable = {};
        };
        cCPgNotifier.prototype.destroy = function () {
            hx.svc.SETTING_UTIL.clearEventCb(this);
            hx.svc.STORAGE_MASS.clearEventCb(this);
            hx.svc.CH_LIST.clearEventCb(this);
            this._bStatusUpdate = false;
            hx.svc.SETTING_UTIL.clearEventCb(this);
            hx.svc.RECORDINGS.clearEventCb(this);
            hx.svc.APP_MGR.clearEventCb(this, 'WidgetCreated');
            hx.svc.APP_MGR.clearEventCb(this, 'WidgetDestroyed');
        };
        cCPgNotifier.prototype.visible = function () {
            return this._isVisible;
        };
        cCPgNotifier.prototype.isActive = function () {
            //becuz background service
            return false;
        };
        // EVENT HANDLER
        cCPgNotifier.prototype._on_alMessageToPage = function (aData) {
            switch (aData.query) {
                case 'alChangePage':
                    if (this._cimmi && this._cimmi !== -1) {
                        hx.msg.close('selectMmi', this._cimmi);
                    }
                    if (this._cinative !== undefined) {
                        hx.msg.close(this._cinative.msgType, this._cinative.msgKey);
                    }
                    break;
            }
        };
        // from dl_dsrvCasCI.js
        cCPgNotifier.prototype._on_CiSystemMessage = function (aParam) {
            this._ci_draw_systemMsg(aParam);
        };
        // from dl_dsrvCasCI.js
        cCPgNotifier.prototype._on_CiNativeUiEvent = function (aParam) {
            this._ci_draw_nativeui(aParam);
        };
        // from dl_dsrvCasCI.js
        cCPgNotifier.prototype._on_CiMMIEvent = function (aParam) {
            this._ci_draw_mmi(aParam);
        };
        // PRIVATE FUNCTION
        cCPgNotifier.prototype._initService = function () {
            var _this = this;
            var that = this;
            if (hx.config.useCI) {
                //for common interface ui
                hx.svc.CAS_CI.addEventCb('CiMMIEvent', this, function (aParam) {
                    _this._on_CiMMIEvent(aParam);
                });
                hx.svc.CAS_CI.addEventCb('CiNativeUiEvent', this, function (aParam) {
                    _this._on_CiNativeUiEvent(aParam);
                });
                hx.svc.CAS_CI.addEventCb('CiSystemMessage', this, function (aParam) {
                    _this._on_CiSystemMessage(aParam);
                });
                /*
                //TODO apply for CI Plus 1.3
                 hx.svc.CAS_CI.addEventCb('UpdateCiProfileInfo', this, (aParam) => {
                    this._on_UpdateCiProfileInfo(aParam);
                });
                */
                hx.svc.CAS_CI.requestCIBannerSession({
                    'slot': 0,
                    'sessionHandleKey': that._CIBannerSessionKey
                });
            }
            //for usb wizard
            hx.logadd('sh');
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageAttached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageAttached(aPhysicalStorage);
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageDetached(aPhysicalStorage);
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageStatus', that, function (aId, aStatus) {
                that._on_PhysicalStorageStatus(aId, aStatus);
            });
            hx.svc.SETTING_UTIL.addEventCb('NotifyPinCode', that, function (aParam) {
                that._on_NotifyPinCode(aParam);
            });
            hx.svc.SETTING_UTIL.addEventCb('checkDetachableStorage', that, function (aParam) {
                that._on_checkDetachableStorage(aParam);
            });
            /*
            //TODO apply for CI Plus 1.3
            //for AppMgr event
            hx.svc.APP_MGR.addEventCb('WidgetCreated', that, (widgetDescriptor) => {
                hx.log('dmrProxy', 'WidgetCreated');
                that._bDraw = false;
            });
    
            hx.svc.APP_MGR.addEventCb('WidgetDestroyed', that, (widgetDescriptor) => {
                hx.log('dmrProxy', 'WidgetDestroyed');
                that._bDraw = true;
            });
    
            //for ChannelSearch event
            hx.svc.CHANNEL_SEARCH.addEventListener({
                    event : 'CIProfileChannelScan',
                    objTarget : that
            });
            */
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', that, function () {
                that._on_ChannelListUpdate();
            });
            hx.svc.SETTING_UTIL.addEventCb('PowerStateChanged', that, function (aPowerState) {
                hx.log('CPgNotifier', '#### PowerStateChanged!! aPowerState : ' + aPowerState);
                if (aPowerState === 2) {
                    hx.log('CPgNotifier', '#### PowerStateChanged!! Go to CPgStandby');
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStandby',
                        me: that
                    });
                }
            });
        };
        cCPgNotifier.prototype._registerPowerOffTimer = function () {
            var that = this;
            // 1. auto power down
            hx.svc.SETTING_UTIL.addEventCb('autoPowerDownEvent', that, function () {
                if (that.getCenterPageName() !== 'CPgStandby') {
                    that._powerDownDlgCreate(hx.l('LOC_AUTOPOWERDOWN_WATCHING_TV_ID'), 120000);
                }
            });
            // 2. power off timer
            hx.svc.SETTING_UTIL.addEventCb('powerOffTimerEvent', that, function () {
                if (that.getCenterPageName() !== 'CPgStandby') {
                    if (hx.il.isCriticalTasking() !== true) {
                        that._powerOffDlgCreate(hx.l('LOC_POWEROFF_MSG_ID'), 30000);
                    }
                }
            });
            // 3. reminder power off
            hx.svc.RECORDINGS.addEventCb('ScheduledItemRemoved', that, function (param) {
                var item = param.item;
                if (hx.svc.SETTING_UTIL.getReminderPowerOff() === true) {
                    if (item.isReminder()) {
                        hx.log('CPgNotifier', '#### canceled!! reminder power off - scheduel event : ' + param.state);
                        hx.svc.SETTING_UTIL.stopReminderPowerOff();
                        // cancel
                        if (that.getCenterPageName() !== 'CPgStandby') {
                            // end of reminder, power off
                            hx.log('CPgNotifier', 'reminder power off : GO GO GO reminder power off !!!');
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgStandby',
                                me: that
                            });
                        }
                    }
                }
            });
        };
        cCPgNotifier.prototype._ci_draw_systemMsg = function (param) {
            var that = this;
            if (param.message !== undefined) {
                if (that._cisysmsg === undefined) {
                    that._cisysmsg = -1;
                }
                if (that._cisysmsg !== -1) {
                    hx.msg.close('progress', that._cisysmsg);
                    that._cisysmsg = -1;
                }
                if (param.message.length !== 0) {
                    that._cisysmsg = hx.msg('progress', {
                        text: that.__parse_casui_string(param.message),
                        enable_key: true,
                        auto_close: true,
                        timeout: 10000
                    });
                }
            }
        };
        cCPgNotifier.prototype._ci_draw_nativeui = function (param) {
            'use strict';
            var that = this //
            , uidata, msgKey, userInput;
            var dispInfo = param.data;
            if (!dispInfo || !dispInfo.castype || !dispInfo.uitype) {
                return;
            }
            var msgParam;
            switch (dispInfo.uitype) {
                case 'banner':
                    uidata = dispInfo.uidata;
                    msgParam = {
                        auto_close: true
                    };
                    if (uidata.attribute.indexOf('timeout') !== -1) {
                        msgParam.timeout = hx.il.castNumber(uidata.timeoutsecond) * 1000;
                    }
                    if (uidata && uidata.string && uidata.string.length) {
                        msgParam.text = that.__parse_casui_string(uidata.string);
                        msgKey = hx.msg('response', msgParam);
                    }
                    break;
                case 'inputdialog':
                    if (that._cinative !== undefined) {
                        hx.msg.close(that._cinative.msgType, that._cinative.msgKey);
                    }
                    uidata = dispInfo.uidata;
                    msgParam = {
                        auto_close: false
                    };
                    if (uidata.attribute.indexOf('timeout') !== -1) {
                        msgParam.auto_close = true;
                        msgParam.timeout = hx.il.castNumber(uidata.timeoutsecond) * 1000;
                    }
                    if (uidata.attribute.indexOf('title') !== -1) {
                        msgParam.first_line = that.__parse_casui_string(uidata.title);
                    }
                    if (uidata.attribute.indexOf('subtitle') !== -1) {
                        msgParam.second_line = that.__parse_casui_string(uidata.subtitle);
                    }
                    switch (uidata.inputtype) {
                        case 'button':
                            msgParam.btn_title_arr = [];
                            msgParam.btn_type_arr = [];
                            $.each(uidata.inputdata.attribute, function (i, v) {
                                if (v) {
                                    var key;
                                    msgParam.btn_title_arr.push();
                                    for (key in v) {
                                        if (v.hasOwnProperty(key)) {
                                            msgParam.btn_title_arr.push(that.__parse_casui_string(v[key]));
                                            msgParam.btn_type_arr.push(key);
                                        }
                                    }
                                }
                            });
                            msgParam.callback_fn = function (_status) {
                                var type;
                                hx.log('CPgNotifier', 'inputdialog callback_fn _status ' + _status);
                                if ((_status === 'close') || (_status === 'auto_close')) {
                                    if (that._cinative) {
                                        that._cinative = undefined;
                                    }
                                }
                                else {
                                    switch (_status) {
                                        case 'key_back':
                                        case 'key_esc':
                                            type = 'etc';
                                            userInput = 'previous';
                                            break;
                                        default:
                                            type = 'button';
                                            userInput = msgParam.btn_type_arr[msgParam.btn_title_arr.indexOf(_status)];
                                            break;
                                    }
                                    hx.svc.CAS_CI.setInput({
                                        'target': param.from,
                                        'module': param.module,
                                        'slot': param.slot,
                                        'sessionHandleKey': that._CISessionKey,
                                        'listener': $(that),
                                        'type': type,
                                        'input': userInput
                                    });
                                    hx.msg.close('codeConfirm', that._cinative.msgKey);
                                }
                            };
                            msgKey = hx.msg('codeConfirm', msgParam);
                            that._cinative = {
                                msgKey: msgKey,
                                msgType: 'codeConfirm'
                            };
                            break;
                        case 'number':
                            msgParam.callback_fn = function (_status, _input) {
                                var type;
                                if ((_status === 'close') || (_status === 'auto_close')) {
                                    if (that._cinative) {
                                        that._cinative = undefined;
                                    }
                                }
                                else {
                                    switch (_status) {
                                        case 'stop':
                                            type = 'number';
                                            userInput = _input;
                                            break;
                                        case 'key_back':
                                        case 'key_esc':
                                            type = 'etc';
                                            userInput = 'previous';
                                            break;
                                    }
                                    hx.svc.CAS_CI.setInput({
                                        'target': param.from,
                                        'module': param.module,
                                        'slot': param.slot,
                                        'sessionHandleKey': that._CISessionKey,
                                        'listener': $(that),
                                        'type': type,
                                        'input': userInput
                                    });
                                    hx.msg.close('pin', that._cinative.msgKey);
                                }
                            };
                            msgKey = hx.msg('pin', msgParam);
                            that._cinative = {
                                msgKey: msgKey,
                                msgType: 'pin'
                            };
                            break;
                    }
                    break;
                case 'undefine':
                    if (that._cinative !== undefined) {
                        hx.msg.close(that._cinative.msgType, that._cinative.msgKey);
                    }
                    break;
            }
        };
        cCPgNotifier.prototype._ci_draw_mmi = function (param) {
            'use strict';
            var that = this, msgKey, userInput;
            if (that._cimmi === undefined) {
                that._cimmi = -1;
            }
            else if (that._cimmi !== -1) {
                hx.msg.close('selectMmi', that._cimmi);
                that._cimmi = -1;
            }
            if (param.eventType === that.SESSION_EVENT_TYPE.REQUESTSESSIONSTOP) {
                if (that._cimmi !== -1) {
                    hx.msg.close('selectMmi', that._cimmi);
                    that._cimmi = -1;
                }
            }
            var dispInfo = param.data;
            if (!dispInfo || !dispInfo.castype || !dispInfo.uitype) {
                //undefine ui type
                return;
            }
            var bDraw = false;
            switch (param.from) {
                case 'MENU':
                    if (that.getCenterPageName() === 'CPgCommonInterface') {
                        bDraw = true;
                    }
                    break;
                case 'EXTENSION':
                    switch (that.getCenterPageName()) {
                        case 'CPgConfigureLAN':
                        case 'CPgConfigureWiFi':
                        case 'CPgInstall01':
                        case 'CPgInstall02':
                        case 'CPgInstall03':
                        case 'CPgInstall04':
                        case 'CPgInstall05':
                        case 'CPgInstall06':
                        case 'CPgInstall07':
                        case 'CPgInstall08':
                        case 'CPgInstall09':
                        case 'CPgEditChannels':
                        case 'CPgSystem':
                        case 'CPgInstall09Time':
                            bDraw = false;
                            break;
                        default:
                            bDraw = true;
                            break;
                    }
                    break;
            }
            if (!bDraw) {
                hx.svc.CAS_CI.closeMmi({
                    'target': param.from,
                    'module': param.module,
                    'slot': param.slot,
                    'sessionHandleKey': that._CISessionKey
                });
            }
            else {
                switch (dispInfo.castype) {
                    case 'ci':
                        var msgParam;
                        switch (dispInfo.uitype) {
                            case 'banner':
                                uidata = dispInfo.uidata;
                                msgParam = {
                                    auto_close: true
                                };
                                if (uidata.attribute.indexOf('timeout') !== -1) {
                                    msgParam.timeout = hx.il.castNumber(uidata.timeoutsecond) * 1000;
                                }
                                if (uidata && uidata.string && uidata.string.length) {
                                    msgParam.text = that.__parse_casui_string(uidata.string);
                                    msgKey = hx.msg('response', msgParam);
                                }
                                break;
                            case 'inputdialogex':
                                uidata = dispInfo.uidata;
                                msgParam = {
                                    auto_close: false
                                };
                                if (uidata.attribute.indexOf('timeout') !== -1) {
                                    msgParam.auto_close = true;
                                    msgParam.timeout = hx.il.castNumber(uidata.timeoutsecond) * 1000;
                                }
                                if (uidata.attribute.indexOf('title') !== -1) {
                                    msgParam.first_line = that.__parse_casui_string(uidata.title);
                                }
                                if (uidata.attribute.indexOf('subtitle') !== -1) {
                                    msgParam.second_line = that.__parse_casui_string(uidata.subtitle);
                                }
                                switch (uidata.inputtype) {
                                    case 'button':
                                        msgParam.btn_title_arr = [];
                                        msgParam.btn_type_arr = [];
                                        $.each(uidata.inputdata.attribute, function (i, v) {
                                            if (v) {
                                                var key;
                                                msgParam.btn_title_arr.push();
                                                for (key in v) {
                                                    if (v.hasOwnProperty(key)) {
                                                        msgParam.btn_title_arr.push(that.__parse_casui_string(v[key]));
                                                        msgParam.btn_type_arr.push(key);
                                                    }
                                                }
                                            }
                                        });
                                        msgParam.callback_fn = function (_status) {
                                            var type;
                                            if ((_status === 'close') || (_status === 'auto_close')) {
                                                if (that._cinative) {
                                                    that._cinative = undefined;
                                                }
                                            }
                                            else {
                                                switch (_status) {
                                                    case 'key_back':
                                                    case 'key_esc':
                                                        type = 'etc';
                                                        userInput = 'previous';
                                                        break;
                                                    default:
                                                        type = 'button';
                                                        userInput = msgParam.btn_type_arr[msgParam.btn_title_arr.indexOf(_status)];
                                                        break;
                                                }
                                                hx.svc.CAS_CI.setInput({
                                                    'target': param.from,
                                                    'module': param.module,
                                                    'slot': param.slot,
                                                    'sessionHandleKey': that._CISessionKey,
                                                    'listener': $(that),
                                                    'type': type,
                                                    'input': userInput
                                                });
                                                hx.msg.close('codeConfirm', that._cinative.msgKey);
                                            }
                                        };
                                        msgKey = hx.msg('codeConfirm', msgParam);
                                        that._cinative = {
                                            msgKey: msgKey,
                                            msgType: 'codeConfirm'
                                        };
                                        break;
                                    case 'number':
                                        msgParam.callback_fn = function (_status, _input) {
                                            var type;
                                            if ((_status === 'close') || (_status === 'auto_close')) {
                                                if (that._cinative) {
                                                    that._cinative = undefined;
                                                }
                                            }
                                            else {
                                                switch (_status) {
                                                    case 'stop':
                                                        type = 'number';
                                                        userInput = _input;
                                                        break;
                                                    case 'key_back':
                                                    case 'key_esc':
                                                        type = 'etc';
                                                        userInput = 'previous';
                                                        break;
                                                }
                                                hx.svc.CAS_CI.setInput({
                                                    'target': param.from,
                                                    'module': param.module,
                                                    'slot': param.slot,
                                                    'sessionHandleKey': that._CISessionKey,
                                                    'listener': $(that),
                                                    'type': type,
                                                    'input': userInput
                                                });
                                                hx.msg.close('pin', that._cinative.msgKey);
                                            }
                                        };
                                        msgKey = hx.msg('pin', msgParam);
                                        that._cinative = {
                                            msgKey: msgKey,
                                            msgType: 'pin'
                                        };
                                        break;
                                }
                                break;
                            case 'menulist':
                            case 'inputdialog':
                                var uidata = dispInfo.uidata;
                                hx.log('CPgNotifier', 'Draw Input dialog... type: ' + dispInfo.uitype);
                                that._cimmi = hx.msg('selectMmi', {
                                    'title': param.name,
                                    'type': dispInfo.uitype,
                                    'uidata': uidata,
                                    'module': param.module,
                                    'slot': param.slot,
                                    'auto_close': false,
                                    'callback_fn': function (_msg) {
                                    },
                                    'fxInput': function (type, input) {
                                        var inputData;
                                        if (that._cimmi !== -1) {
                                            hx.msg.close('selectMmi', that._cimmi);
                                            that._cimmi = -1;
                                        }
                                        switch (type) {
                                            case 'exit':
                                                hx.svc.CAS_CI.closeMmi({
                                                    'target': param.from,
                                                    'module': param.module,
                                                    'slot': param.slot,
                                                    'sessionHandleKey': param.sessionKey
                                                });
                                                break;
                                            default:
                                                hx.svc.CAS_CI.setInput({
                                                    'target': param.from,
                                                    'module': param.module,
                                                    'slot': param.slot,
                                                    'sessionHandleKey': param.sessionKey,
                                                    'listener': $(that),
                                                    'type': type,
                                                    'input': input
                                                });
                                                break;
                                        }
                                    }
                                });
                                break;
                        }
                        break;
                }
            }
        };
        // due to dynamic update - Check CCID Change
        cCPgNotifier.prototype._on_ChannelListUpdate = function () {
            var that = this;
            that._checkUpdateServiceForLastChannel();
        };
        cCPgNotifier.prototype._checkUpdateServiceForLastChannel = function () {
            var that = this;
            var currentVBChannel = null, stlastSvc = null, stFindSvc = null;
            var bIsChannelPlaying = false, bAvailable = false, bFindCurSvc = false;
            var channelInfo = null;
            stlastSvc = hx.svc.WEB_STORAGE.getLastService();
            var fnLastServiceAvaiable = function () {
                var bChangeable = false;
                if ((stlastSvc.ccid !== null) && (stlastSvc.group !== null)) {
                    channelInfo = hx.svc.CH_LIST.getChannel({
                        ccid: stlastSvc.ccid,
                        group: stlastSvc.group
                    });
                    if (channelInfo && channelInfo.channel) {
                        bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel: channelInfo.channel
                        });
                    }
                    else {
                        bChangeable = false;
                    }
                }
                return bChangeable;
            };
            var fnSetLastService = function (pCCid, pGroup) {
                hx.svc.WEB_STORAGE.updateLastService({
                    'ccid': pCCid,
                    'group': pGroup
                });
            };
            var fnChangeServiceWithFindCurrnetService = function (pbSvcChange) {
                var bReturn = false;
                currentVBChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                if (currentVBChannel === null) {
                    return false;
                }
                channelInfo = hx.svc.CH_LIST.getChannel({
                    ccid: currentVBChannel.ccid,
                    group: stlastSvc.group
                });
                if (channelInfo && channelInfo.channel) {
                    fnSetLastService(channelInfo.channel.ccid, channelInfo.group);
                    bReturn = true;
                }
                else {
                    channelInfo = hx.svc.CH_LIST.getAvailableGroupForChannel({
                        'channel': currentVBChannel
                    });
                    if (channelInfo && channelInfo.channel) {
                        if (pbSvcChange === true) {
                            hx.svc.AV_CTRL.channelChange({
                                'channel': channelInfo.channel,
                                'group': channelInfo.group
                            });
                        }
                        else {
                            fnSetLastService(channelInfo.channel.ccid, channelInfo.group);
                        }
                        bReturn = true;
                    }
                }
                return bReturn;
            };
            var fnChangeServiceWithAvailableService = function () {
                hx.log("error", '#### _checkUpdateServiceForLastChannel :  Current channel is not valid!! change due to current is update!');
                hx.svc.AV_CTRL.availableChannel();
            };
            bIsChannelPlaying = hx.svc.AV_CTRL.isChannelPlaying();
            // 1. Live View
            // 1-1 View Channel Check
            if (bIsChannelPlaying === true) {
                currentVBChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                bAvailable = fnLastServiceAvaiable();
                if ((stlastSvc.ccid !== null) && (stlastSvc.group !== null) && currentVBChannel) {
                    if (!!currentVBChannel && (currentVBChannel.ccid === stlastSvc.ccid) && (bAvailable === true)) {
                        hx.log("error", '#### _checkUpdateServiceForLastChannel :  Do not channel change due to current and last was same channel');
                        return;
                    }
                }
                if (bAvailable === false) {
                    hx.msg('response', {
                        text: hx.l('LOC_DYNAMIC_CHANNEL_UPDATE_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                    bFindCurSvc = fnChangeServiceWithFindCurrnetService(true);
                    if (bFindCurSvc === false) {
                        fnChangeServiceWithAvailableService();
                    }
                }
            }
            else {
                bAvailable = fnLastServiceAvaiable();
                if (bAvailable === false) {
                    bFindCurSvc = fnChangeServiceWithFindCurrnetService(false);
                    if (bFindCurSvc === false) {
                        stFindSvc = hx.svc.CH_LIST.getAvailableFirstChannel();
                        if (stFindSvc && stFindSvc.channel) {
                            hx.log("error", '#### _checkUpdateServiceForLastChannel :  last channel is not valid!! change due to last channel info change!');
                            fnSetLastService(stFindSvc.channel.ccid, stFindSvc.group);
                        }
                    }
                }
            }
        };
        cCPgNotifier.prototype._on_checkDetachableStorage = function (aParam) {
            var dsrvWebStorage = hx.svc.WEB_STORAGE;
            hx.logadd('DetachableHDD - MSG');
            var storageUuid = -1;
            if (aParam) {
                storageUuid = aParam.data;
            }
            var _fnGetStorageUUID = function () {
                return dsrvWebStorage.getStorageUUID();
            };
            var _fnSetStorageUUID = function (p) {
                dsrvWebStorage.setStorageUUID({
                    detachableHdd: p
                });
            };
            function _checkFormatType(strObj) {
                var enter = true;
                var repo = hx.al.entity.getRepository();
                var pgTvOption = repo.get("CPgTvOption");
                if (pgTvOption) {
                    if (pgTvOption.isActive()) {
                        enter = false;
                    }
                }
                if (!hx.msg.isOpend() && enter === true && ($('#wrap #hep').length === 0)) {
                    // msg
                    // && option && help
                    if (!strObj.isNTFS) {
                        var yesBtn = hx.l('LOC_YES_ID');
                        var noBtn = hx.l('LOC_NO_ID');
                        that._formatConfirm = hx.msg('confirm', {
                            text: hx.l('LOC_FORMAT_DETACH_STORAGE_ID'),
                            auto_close: true,
                            timeout: 30000,
                            btn_title_arr: [yesBtn, noBtn],
                            callback_fn: function (_status) {
                                if (_status === yesBtn) {
                                    hx.il.criticalTasking.crHDDTasking = true;
                                    // DMR uri set null ('bring in client' do not have to work)
                                    hx.svc.DMR.setMedia(null);
                                    that.progressMsg = hx.msg('progress', {
                                        text: hx.l('LOC_FORMATTING_ID'),
                                        _use_backkey: false,
                                        auto_close: false
                                    });
                                    hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                                        that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                                    });
                                    hx.svc.STORAGE_MASS.doFormat({
                                        id: strObj.id
                                    });
                                }
                                that._formatConfirm = undefined;
                            }
                        });
                    }
                    else {
                        var currentUuid = _fnGetStorageUUID;
                        if (currentUuid().detachableHdd !== storageUuid) {
                            var okBtn = hx.l('LOC_OK_ID');
                            that._formatConfirm = hx.msg('confirm', {
                                text: hx.l('LOC_DETACHABLE_HDD_ASSIGN_RECORDING_PURPOSE'),
                                auto_close: true,
                                timeout: 30000,
                                btn_title_arr: [okBtn],
                                callback_fn: function (_status) {
                                    _fnSetStorageUUID(storageUuid);
                                }
                            });
                            hx.log('DetachableHDD - MSG', 'fnSetConnectedDetachableHdd : ' + storageUuid);
                        }
                    }
                }
            }
            if ($('#wrap #hep').length > 0) {
                return;
            }
            var that = this, i, strObj, retArray = hx.svc.STORAGE_MASS.getWholeStorageInfo({
                cb: undefined
            });
            for (i = 0; i < retArray.length; i += 1) {
                strObj = retArray[i];
                if (strObj.type === 'externalPvrHDD') {
                    hx.logadd('storage');
                    hx.log('storage', '!!!!!!!! strObj.pairing : ' + strObj.pairing);
                    if (strObj.pairing === that.PAIRINGSTATE.EPairingOK) {
                        that.progressMsg = hx.msg('progress', {
                            text: hx.l('LOC_EXTERNAL_DEVICE_LOADING_ID'),
                            _use_backkey: false,
                            auto_close: true,
                            timeout: 1500,
                            callback_fn: function (_status) {
                                if (_status === 'auto_close') {
                                    _checkFormatType(strObj);
                                }
                            }
                        });
                    }
                    else {
                        _checkFormatType(strObj);
                    }
                    break;
                }
            }
        };
        cCPgNotifier.prototype._on_FormatProgress = function (aState, aProgress, aProgressMax, aErrMessage) {
            var that = this;
            switch (aState) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    //EFormatProgressFinished
                    hx.msg.close('progress', that.progressMsg);
                    hx.il.criticalTasking.crHDDTasking = false;
                    hx.msg('response', {
                        text: hx.l('LOC_HDD_ASSIGNED_FOR_RECORDING_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                    var dsrvRecordings = hx.svc.RECORDINGS;
                    dsrvRecordings.refreshRecordingList();
                    break;
                case 3:
                    hx.msg.close('progress', that.progressMsg);
                    hx.il.criticalTasking.crHDDTasking = false;
                    that._failFormatMsg = hx.msg('codeConfirm', {
                        first_line: '',
                        regcode: hx.l('LOC_FAILED_HDD_FORMAT_ID'),
                        nolinejump: true,
                        second_line: hx.l('LOC_CANNOT_FORMAT_STORAGE_ID'),
                        auto_close: false,
                        btn_title_arr: [hx.l('LOC_OK_ID')],
                        callback_fn: failformat_fn,
                        dia_class: 'osd'
                    });
                    function failformat_fn(_status) {
                        if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                            hx.msg.close('codeConfirm', that._failFormatMsg);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('codeConfirm', that._failFormatMsg);
                        }
                    }
                    break;
            }
            if (aErrMessage > 0) {
                hx.msg.close('progress', that.progressMsg);
                hx.il.criticalTasking.crHDDTasking = false;
                that._failFormatMsg = hx.msg('codeConfirm', {
                    first_line: '',
                    regcode: hx.l('LOC_FAILED_HDD_FORMAT_ID'),
                    nolinejump: true,
                    second_line: hx.l('LOC_CANNOT_FORMAT_STORAGE_ID'),
                    auto_close: false,
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: failformat_fn2,
                    dia_class: 'osd'
                });
                function failformat_fn2(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        hx.msg.close('codeConfirm', that._failFormatMsg);
                    }
                    else if (_status === 'key_esc') {
                        hx.msg.close('codeConfirm', that._failFormatMsg);
                    }
                }
            }
        };
        cCPgNotifier.prototype._on_PhysicalStorageStatus = function (aId, aStatus) {
            var that = this;
            var stString = '', bTasking = false, bMenu = false;
            hx.logadd('PhysicalStorageStatus');
            hx.log('PhysicalStorageStatus', 'CPgNotifier id : ' + aId);
            hx.log('PhysicalStorageStatus', 'CPgNotifier status : ' + aStatus);
            bTasking = hx.il.isCriticalTasking();
            bMenu = that._isInStoragePage();
            if ((bTasking === true) || (bMenu === true)) {
                hx.log('PhysicalStorageStatus', 'Error!! isCriticalTasking & isInStoragePage');
                return;
            }
            // Check Message statues, because some time duplicate delivery
            if (that._bStatusUpdate === true) {
                hx.log('PhysicalStorageStatus', 'Error!! Already check prev message');
                return;
            }
            if ((that._formatConfirm === undefined) && (aStatus === 5)) {
                hx.log('PhysicalStorageStatus', 'Error!! Sorry it is not my order');
                return;
            }
            switch (aStatus) {
                case 5:
                    //eOPL_STORAGE_EVENT_PairingDone
                    stString = hx.l('LOC_STORAGE_ASSIGN_PVR_COMPLETE_MSG_ID');
                    that._bStatusUpdate = true;
                    break;
                case 6:
                    //eOPL_STORAGE_EVENT_UnPairingDone
                    stString = hx.l('LOC_STORAGE_DEASSIGN_PVR_COMPLETE_MSG_ID');
                    that._bStatusUpdate = true;
                    break;
                case 7:
                    // eOPL_STORAGE_EVENT_PairingFail
                    stString = hx.l('LOC_USB_LOADING_ERROR_ID');
                    that._bStatusUpdate = true;
                    break;
                case 12:
                    // eOPL_STORAGE_EVENT_CheckDiskStarted
                    stString = hx.l('LOC_CHECK_DISK_USB_ID');
                    that._bStatusUpdate = true;
                    break;
                case 16:
                    // eOPL_STORAGE_EVENT_OverCurrent
                    if (that._Overcurrentmsg === -1) {
                        stString = hx.l('LOC_USB_SHORT_DESC_ID');
                        that._Overcurrentmsg = hx.msg('status', {
                            text: stString,
                            auto_close: true,
                            close_time: 3000
                        });
                        that._bStatusUpdate = true;
                    }
                    return;
                default:
                    hx.log('PhysicalStorageStatus', 'Skip this status Msg');
                    return;
            }
            if (aStatus !== 15) {
                hx.msg('response', {
                    text: stString,
                    auto_close: true,
                    close_time: 3000
                });
            }
            that._bStatusUpdate = false;
        };
        cCPgNotifier.prototype._on_PhysicalStorageAttached = function (aPhysicalStorage) {
            var that = this;
            hx.log('sh', '_on_PhysicalStorageAttached###################################');
            if (that.getCenterPageName() === 'CPgStandby') {
                return;
            }
            if (!hx.il.isCriticalTasking() && !that._isInStoragePage()) {
                if ((aPhysicalStorage.kind == that.HMXPhysicalStorageKind.EKindUSBMemory) || (aPhysicalStorage.kind == that.HMXPhysicalStorageKind.EKindSDMemory) || (aPhysicalStorage.kind == that.HMXPhysicalStorageKind.EKindHDD)) {
                    if (hx.config.useInternalPvr === 0) {
                        that._action_ExternalHDDProcess(aPhysicalStorage);
                    }
                }
                else {
                }
            }
        };
        cCPgNotifier.prototype._action_ExternalHDDProcess = function (aPhysicalStorage) {
            var that = this;
            hx.log('sh', '_action_ExternalHDDProcess###################################');
            switch (aPhysicalStorage.kind) {
                case that.HMXPhysicalStorageKind.EKindHDD:
                    if (aPhysicalStorage.usage !== that.HMXPhysicalStorageUsage.EUsagePVR) {
                        that._on_checkExternalHDDStorage(aPhysicalStorage);
                    }
                    else {
                        hx.log('sh', '_action_ExternalHDDProcess - This HDD already used for PVR');
                    }
                    break;
                case that.HMXPhysicalStorageKind.EKindUSBMemory:
                case that.HMXPhysicalStorageKind.EKindSDMemory:
                    break;
                default:
                    hx.log('sh', '_action_ExternalHDDProcess - Error Unknow Storage Type : ' + aPhysicalStorage.kin);
                    break;
            }
        };
        cCPgNotifier.prototype._on_checkExternalHDDStorage = function (aPhysicalStorage) {
            var that = this, strObj, strDevObj, retArray, retDeviceArray;
            var j = 0, i = 0, ulPartitionLen = 0, bHasParingHDD = false;
            hx.log('sh', '_on_checkExternalHDDStorage ###################################');
            function _checkFormatType(strObj, ulPartition) {
                // Step 2 : HDD Format & Partition Info
                if ((!strObj.isNTFS) || (ulPartition > 1)) {
                    that._action_FormatUSBPhysicalStorage(aPhysicalStorage, strObj);
                }
                else {
                    that._action_AssignPhysicalStorage(aPhysicalStorage, strObj);
                }
            }
            retArray = hx.svc.STORAGE_MASS.getWholeStorageInfo({
                cb: undefined
            });
            // Step 1 : already has paring HDD
            bHasParingHDD = false;
            for (i = 0; i < retArray.length; i += 1) {
                if (retArray[i].type === 'internalPvrHDD') {
                    bHasParingHDD = true;
                    break;
                }
            }
            // Step 2 : HDD Format & Partition Info
            retDeviceArray = hx.svc.STORAGE_MASS.getDevice();
            for (i = 0; i < retArray.length; i += 1) {
                strObj = retArray[i];
                if (strObj.id === aPhysicalStorage.id) {
                    hx.logadd('storage');
                    hx.log('storage', '!!!!!!!! strObj.id : ' + strObj.id);
                    for (j = 0; j < retDeviceArray.length; i++) {
                        strDevObj = retDeviceArray[i];
                        if (strObj.id === strDevObj.id) {
                            hx.log('storage', '!!!!!!!! ulPartitionLen : ' + strDevObj.partitions.length);
                            ulPartitionLen = strDevObj.partitions.length;
                            break;
                        }
                    }
                    _checkFormatType(strObj, ulPartitionLen);
                    break;
                }
            }
        };
        cCPgNotifier.prototype._action_FormatUSBPhysicalStorage = function (aPhysicalStorage, aStrObj) {
            var that = this, yesBtn, noBtn;
            hx.log('sh', '_action_FormatUSBPhysicalStorage###################################');
            yesBtn = hx.l('LOC_YES_ID');
            noBtn = hx.l('LOC_NO_ID');
            that._formatConfirm = hx.msg('confirm', {
                text: hx.l('LOC_FORMAT_USB_STORAGE_ID'),
                auto_close: true,
                timeout: 30000,
                btn_title_arr: [yesBtn, noBtn],
                callback_fn: function (_status) {
                    if (_status === yesBtn) {
                        hx.il.criticalTasking.crHDDTasking = true;
                        // DMR uri set null ('bring in client' do not have to work)
                        hx.svc.DMR.setMedia(null);
                        that.progressMsg = hx.msg('progress', {
                            text: hx.l('LOC_FORMATTING_ID'),
                            _use_backkey: false,
                            auto_close: false
                        });
                        hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                            that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                        });
                        hx.svc.STORAGE_MASS.doFormat({
                            id: aStrObj.id
                        });
                    }
                    that._formatConfirm = undefined;
                }
            });
        };
        cCPgNotifier.prototype._action_AssignPhysicalStorage = function (aPhysicalStorage, aStrObj) {
            var that = this, yesBtn, noBtn, ulResult;
            hx.log('sh', '_action_AssignPhysicalStorage###################################');
            yesBtn = hx.l('LOC_YES_ID');
            noBtn = hx.l('LOC_NO_ID');
            that._formatConfirm = hx.msg('confirm', {
                text: hx.l('LOC_STORAGE_ASSIGN_PVR_MSG_ID'),
                auto_close: true,
                timeout: 30000,
                btn_title_arr: [yesBtn, noBtn],
                callback_fn: function (_status) {
                    if (_status === yesBtn) {
                        ulResult = hx.svc.STORAGE_MASS.doSwitchExternalHdd({
                            id: aStrObj.id,
                            assignPvr: true
                        });
                    }
                    that._formatConfirm = undefined;
                }
            });
        };
        cCPgNotifier.prototype._on_PhysicalStorageDetached = function (aPhysicalStorage) {
            hx.log('sh', '_on_PhysicalStorageDetached###################################');
            var that = this;
            if (aPhysicalStorage.kind == that.HMXPhysicalStorageKind.EKindHDD) {
                if (aPhysicalStorage.usage == that.HMXPhysicalStorageUsage.EUsageSubPVR) {
                    if (!hx.il.isCriticalTasking() && !that._isInStoragePage()) {
                        if (!that._detachPullOut) {
                            that._detachPullOut = hx.msg('confirm', {
                                text: hx.l('LOC_DETACH_PULLED_OUT_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                btn_title_arr: [hx.l('LOC_OK_ID')],
                                callback_fn: function () {
                                    that._detachPullOut = undefined;
                                }
                            });
                        }
                    }
                }
                else if ((hx.config.useInternalPvr === 0) && (aPhysicalStorage.usage == that.HMXPhysicalStorageUsage.EUsagePVR)) {
                    if (!hx.il.isCriticalTasking() && !that._isInStoragePage()) {
                        if (that._formatConfirm) {
                            hx.msg.close('confirm', that._formatConfirm);
                            that._formatConfirm = undefined;
                        }
                        // TODO : - JHLEE : Check Other pairing HDD
                        if (!that._detachPullOut) {
                            that._detachPullOut = hx.msg('confirm', {
                                text: hx.l('LOC_STORAGE_ASSIGN_PVR_CANCELLATION_MSG_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                btn_title_arr: [hx.l('LOC_OK_ID')],
                                callback_fn: function () {
                                    that._detachPullOut = undefined;
                                }
                            });
                        }
                    }
                }
                else {
                    if (!that._isInStoragePage() && !that._isInMediaPage()) {
                        if (that._formatConfirm) {
                            hx.msg.close('confirm', that._formatConfirm);
                            that._formatConfirm = undefined;
                        }
                        hx.msg('response', {
                            text: hx.l('LOC_USB_REMOVED_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                }
            }
            else if (aPhysicalStorage.kind == that.HMXPhysicalStorageKind.EKindUSBMemory) {
                if (!that._isInStoragePage()) {
                    hx.msg('response', {
                        text: hx.l('LOC_USB_REMOVED_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                }
            }
            else if (aPhysicalStorage.kind == that.HMXPhysicalStorageKind.EKindSDMemory) {
            }
            else {
            }
        };
        cCPgNotifier.prototype._on_NotifyPinCode = function (aParam) {
            var that = this;
            var state = aParam.state;
            var pin = aParam.pin;
            var buttonDlgOpened = prism.CButtonDlg.isOpened();
            var queryPasswordDlgOpened = prism.CQueryPasswordDlg.isOpened();
            var selectionDlgOpened = prism.CSelectionDlg.isOpened();
            var globalDialogOpened = prism.isGlobalDialogOpened();
            var messageOpened = hx.msg.isOpend();
            var isOpened = buttonDlgOpened || queryPasswordDlgOpened || selectionDlgOpened || globalDialogOpened || messageOpened;
            hx.log('sh', '_on_NotifyPinCode : state : ' + state + ', pincode : ' + pin);
            hx.log('sh', '_on_NotifyPinCode : opend : ' + isOpened + ', critical : ' + hx.il.isCriticalTasking());
            if (state) {
                if (!isOpened && !hx.il.isCriticalTasking()) {
                    that._cacheDlgHandle = hx.msg('codeConfirm', {
                        first_line: hx.l('LOC_STR_PIN_CODE_ID') + ' : ',
                        regcode: pin,
                        second_line: hx.l('LOC_MSG_PAIRING_ID'),
                        auto_close: true,
                        timeout: 30 * 1000,
                        btn_title_arr: [hx.l('LOC_OK_ID')],
                        callback_fn: function (choice) {
                            that._cacheDlgHandle = null;
                        }
                    });
                }
            }
            else {
                if (that._cacheDlgHandle) {
                    hx.msg.close('codeConfirm', that._cacheDlgHandle);
                }
            }
        };
        cCPgNotifier.prototype._isInMediaPage = function () {
            var bShowVideo = false, bShowPhoto = false, bShowMusic = false;
            var repo = hx.al.entity.getRepository();
            var pgVideo = repo.get("CPgVideo");
            var pgPhoto = repo.get("CPgPhoto");
            var pgMusic = repo.get("CPgMusic");
            if (pgVideo) {
                if (pgVideo.isActive()) {
                    bShowVideo = true;
                }
            }
            if (pgPhoto) {
                if (pgPhoto.isActive()) {
                    bShowPhoto = true;
                }
            }
            if (pgMusic) {
                if (pgMusic.isActive()) {
                    bShowMusic = true;
                }
            }
            if (bShowVideo || bShowPhoto || bShowMusic) {
                return true;
            }
            else {
                return false;
            }
        };
        cCPgNotifier.prototype._isInStoragePage = function () {
            var storage = false, intStorage = false, exStorage = false;
            var repo = hx.al.entity.getRepository();
            var pgStorage = repo.get("CPgStorage");
            var pgInternalStorage = repo.get("CPgInternalStorage");
            var pgExternalStorage = repo.get("CPgExternalStorage");
            if (pgStorage) {
                if (pgStorage.isActive()) {
                    storage = true;
                }
            }
            if (pgInternalStorage) {
                if (pgInternalStorage.isActive()) {
                    intStorage = true;
                }
            }
            if (pgExternalStorage) {
                if (pgExternalStorage.isActive()) {
                    exStorage = true;
                }
            }
            if (storage || intStorage || exStorage) {
                return true;
            }
            else {
                return false;
            }
        };
        cCPgNotifier.prototype.__parse_casui_string = function (aData) {
            if (!aData) {
                return '';
            }
            var resString = '', i, ti, j, tj;
            switch (typeof aData) {
                case 'object':
                    if (!$.isArray(aData)) {
                        aData = [aData];
                    }
                    for (i = 0, ti = aData.length; i < ti; i += 1) {
                        var item = aData[i];
                        switch (typeof item) {
                            case 'object':
                                if (item.id) {
                                    var strParam = item['in'], locStr = hx.l(item.id);
                                    if (strParam) {
                                        if (!$.isArray(aData)) {
                                            strParam = [strParam];
                                        }
                                        for (j = 0, tj = strParam.length; j < tj; j += 1) {
                                            if (strParam[j]) {
                                                var inputParam = strParam[j];
                                                if (typeof inputParam === 'string') {
                                                    inputParam = decodeURIComponent(inputParam);
                                                }
                                                locStr = locStr.replace(/%d|%s/i, inputParam);
                                            }
                                        }
                                    }
                                    resString += locStr;
                                }
                                break;
                            default:
                                resString += decodeURIComponent(item);
                                break;
                        }
                    }
                    break;
                default:
                    resString = decodeURIComponent(aData);
                    break;
            }
            return resString;
        };
        cCPgNotifier.prototype._powerDownDlgCreate = function (aText, aTimeout) {
            var that = this;
            var btnOK = hx.l('LOC_OK_ID');
            if (that._powerDownDlg !== null) {
                return;
            }
            var powerOff = function () {
                that.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgStandby',
                    me: that
                });
            };
            that._powerDownDlg = prism.createButtonDlg(null, {
                textHtml: aText,
                buttonList: [btnOK],
                timeOutValue: aTimeout,
                validOnNetflix: true
            }, function (index) {
            });
            that._powerDownDlg.open({
                onClosed: function (reason) {
                    if (hx.config.netflix) {
                        if (hx.svc.NETFLIX.isRunning()) {
                            hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                        }
                    }
                    that._setVisible(false);
                    that._powerDownDlg = null;
                    if (reason === 'timedOut') {
                        setTimeout(function () {
                            powerOff();
                        }, 0);
                    }
                }
            });
            that._setVisible(true);
        };
        cCPgNotifier.prototype._powerOffDlgCreate = function (aText, aTimeout) {
            var that = this;
            var btnYES = hx.l('LOC_YES_ID');
            var btnNO = hx.l('LOC_NO_ID');
            if (that._powerOffDlg !== null) {
                return;
            }
            var powerOff = function () {
                that.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgStandby',
                    me: that
                });
            };
            that._powerOffDlg = prism.createButtonDlg(null, {
                textHtml: aText,
                buttonList: [btnYES, btnNO],
                timeOutValue: aTimeout,
                validOnNetflix: true
            }, function (index) {
                if (index === 0) {
                    if (hx.config.netflix) {
                        if (hx.svc.NETFLIX.isRunning()) {
                            hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                        }
                    }
                    setTimeout(function () {
                        powerOff();
                    }, 0);
                }
            });
            that._powerOffDlg.open({
                onClosed: function (reason) {
                    if (hx.config.netflix) {
                        if (hx.svc.NETFLIX.isRunning()) {
                            hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                        }
                    }
                    that._setVisible(false);
                    that._powerOffDlg = null;
                    if (reason === 'timedOut') {
                        setTimeout(function () {
                            powerOff();
                        }, 0);
                    }
                }
            });
        };
        cCPgNotifier.prototype._setVisible = function (aIsVisible) {
            hx.log('error', '[setVisible] previous setVisible ' + this._isVisible + ', set ' + aIsVisible);
            if (aIsVisible !== this._isVisible) {
                this._isVisible = aIsVisible;
            }
        };
        return cCPgNotifier;
    })(__PAGE_KERNEL__);
    return cCPgNotifier;
});
