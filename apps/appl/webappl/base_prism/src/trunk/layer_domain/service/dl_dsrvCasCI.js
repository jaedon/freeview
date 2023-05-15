/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvCasCI.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CAS_CI: ['layer_domain/service/dl_dsrvCasCI.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLCasCI = (function (_super) {
        __extends(cDLCasCI, _super);
        function cDLCasCI(aServiceName) {
            _super.call(this, aServiceName);
            this.oipfCasUtilObj = hx.stbObject.casUtil;
            this.localSystemOipfObj = this.getPluginInstance('LocalSystem');
            this.innerConfigOipfObj = this.getPluginInstance('Configuration');
            this._CAModule = [
                'NONE',
                'MG',
                'VA',
                'IR',
                'CX',
                'CR',
                'NA',
                'ND',
                'SATSA',
                'BC',
                'COMMON',
                'CI',
                'IRUC'
            ];
            this._CAGroupId = [
                0x0000,
                0x0100,
                0x0500,
                0x0600,
                0x0B00,
                0x0D00,
                0x1800,
                0x0900,
                0x4400,
                0xfffd,
                0xfffe,
                0xffff,
                0x0600
            ];
            this._CASessionType = [
                'NONE',
                'CONTROL',
                'BANNER',
                'MENU',
                'CUSTOM',
                'NATIVE'
            ];
            this._CANewSessionType = {
                'NONE': 0,
                'BANNER': 2,
                'MENU': 3,
                'EXTENSION': 4,
                'MAX': 5
            };
            this._CASMenuGroup = ["{\"menugroup\":\"menugroup1\"}", "{\"menugroup\":\"menugroup2\"}", "{\"menugroup\":\"menugroup3\"}", "{\"menugroup\":\"menugroup4\"}", "{\"menugroup\":\"menugroup5\"}"];
            this._camStatus = [
                'EXTRACTED',
                'INSERTED',
                'INITIALIZING',
                'INITIALIZED'
            ];
            this._state = 'idle';
            this._pActList = [];
            this._slotInfo = {
                'enable': false,
                'slot': []
            };
            this._casCiSessionHandle = {};
            this._typeOfSessionHandle = {
                handle: 0,
                module: ''
            };
            // Event handler.
            this.eventHandlers = [];
            this.defineEvents(['CiMMIEvent', 'CiNativeUiEvent', 'CiSystemMessage', 'UpdateCiSlotInfo', 'SmartcardStateChanged', 'UpdateCiProfileInfo', 'CasIrSystemMessage', 'CasIrCustomUiEvent', 'CasIrNativeUiEvent', 'CasIrMenuUiEvent', 'CasNaMenuUiEvent', 'CasNaCustomUiEvent', 'CasNaSystemMessage']);
            hx.log("dsvc", "initializer service - hx.svc.CAS_CI");
            hx.logadd("dsrvCasCI");
            if (ENV.listCAS.IR === ENV.cas) {
                this._moduleName = 'IR';
            }
            else if (ENV.listCAS.NA === ENV.cas) {
                this._moduleName = 'NA';
            }
            else if (ENV.listCAS.CI === ENV.cas) {
                this._moduleName = 'CI';
            }
            else if (ENV.listCAS.IRUC === ENV.cas) {
                this._moduleName = 'IRUC';
            }
            else {
                var pleaseAddOther;
            }
            ;
            this._init();
        }
        /*********** Internal Function Start ****************/
        cDLCasCI.prototype._getActualCISlotInfo = function (data) {
            var ciEnable, retObj = {}, i, ti;
            hx.log('dsrvCasCI', '[dsrvCasCI] _getActualCISlotInfo() called!');
            ciEnable = this.localSystemOipfObj.getField('ciplus_enable');
            switch (ciEnable) {
                case 'disable':
                    retObj.enable = false;
                    break;
                case 'enable':
                    retObj.enable = true;
                    retObj.version = this.localSystemOipfObj.getField('ciplus_version');
                    retObj.slotCnt = this.localSystemOipfObj.getField('cislot_count');
                    retObj.slot = [];
                    for (i = 0, ti = retObj.slotCnt; i < ti; i += 1) {
                        retObj.slot.push({
                            'num': i,
                            'name': this.innerConfigOipfObj.getCamNameBySlot(i)
                        });
                    }
                    break;
            }
            return retObj;
        };
        cDLCasCI.prototype.__close_mmi = function (target, module, num, sessionHandleKey) {
            var that = this;
            hx.log('dsrvCasCI', '[dsrvCasCI] __close_mmi() called!');
            that._close_session(module, num, target, sessionHandleKey);
            that._clearMmiTimeout();
            that.fireEvent('CiMMIEvent', {
                'from': target,
                'slot': num,
                'data': {
                    'uitype': 'undefine'
                }
            });
        };
        cDLCasCI.prototype._setMmiTimeout = function (target, module, num) {
            var that = this;
            hx.log('dsrvCasCI', '[dsrvCasCI] _setMmiTimeout() called!');
            that._mmiReqTimer = setTimeout(function () {
                that.__close_mmi(target, module, num);
            }, 10000);
        };
        cDLCasCI.prototype._clearMmiTimeout = function () {
            var that = this;
            hx.log('dsrvCasCI', '[dsrvCasCI] _clearMmiTimeout() called!');
            if (that._mmiReqTimer !== -1) {
                clearTimeout(that._mmiReqTimer);
                that._mmiReqTimer = -1;
            }
        };
        cDLCasCI.prototype.__get_slotInfo = function (num) {
            var that = this, i, ti, retInfo, slotList = that._slotInfo.slot;
            hx.log('dsrvCasCI', '[dsrvCasCI] __get_slotInfo() called!');
            if (!slotList) {
                hx.log('dsrvCasCI', '[dsrvCasCI] slotList is invalid');
                return;
            }
            for (i = 0, ti = slotList.length; i < ti; i += 1) {
                if (slotList[i].num === num) {
                    retInfo = slotList[i];
                    break;
                }
            }
            return retInfo;
        };
        cDLCasCI.prototype._execute_action = function (data) {
            var that = this;
            var action = '_action_' + data.action;
            hx.log('dsrvCasCI', '[dsrvCasCI] _execute_action() called!');
            if (that[action] !== undefined) {
                that[action](data);
            }
        };
        cDLCasCI.prototype._refresh_slotInfo = function (cb) {
            var that = this, slotInfo;
            hx.log('dsrvCasCI', '[dsrvCasCI] _refresh_slotInfo() called!');
            slotInfo = that._getActualCISlotInfo();
            var i, ti;
            if (slotInfo.enable) {
                that._slotInfo.enable = true;
                if (that._slotInfo.slot === undefined) {
                    that._slotInfo.slot = [];
                }
                var list = that._slotInfo.slot;
                list.length = 0;
                for (i = slotInfo.slot.length - 1; i >= 0; i -= 1) {
                    var slot = slotInfo.slot[i];
                    list.push({
                        'num': slot.num,
                        'name': slot.name,
                        'status': (slot.name.length === 0) ? 'EXTRACTED' : 'INITIALIZED'
                    });
                }
                if (cb) {
                    cb(slotInfo.slot);
                }
            }
        };
        cDLCasCI.prototype._open_session = function (module, slot, type, menu, sessionHandleKey) {
            var that = this //
            , moduleIndex = that._CAModule.indexOf(module) //
            , moduleEnum = that._CAGroupId[moduleIndex], sessionEnum, sessionHandle;
            sessionEnum = that._CANewSessionType[type];
            hx.log('dsrvCasCI', '[dsrvCasCI] _open_session() called!');
            if ((moduleEnum === -1) || (sessionEnum === -1)) {
                hx.log('error', '[dsrvCasCI] _open_session error');
                return;
            }
            sessionHandle = that.oipfCasUtilObj.openSession(moduleEnum, slot, sessionEnum, menu);
            this._casCiSessionHandle[sessionHandleKey] = this._typeOfSessionHandle;
            this._casCiSessionHandle[sessionHandleKey].handle = sessionHandle;
            this._casCiSessionHandle[sessionHandleKey].module = module;
        };
        cDLCasCI.prototype._close_session = function (module, slot, type, sessionHandleKey) {
            var that = this //
            , moduleIndex = that._CAModule.indexOf(module) //
            , moduleEnum = that._CAGroupId[moduleIndex], sessionEnum, sessionHandle;
            sessionEnum = that._CANewSessionType[type];
            hx.log('dsrvCasCI', '[dsrvCasCI] _close_session() called!');
            if ((moduleEnum === -1) || (sessionEnum === -1)) {
                hx.log('error', '[dsrvCasCI] _close_session error');
                return;
            }
            if (this._casCiSessionHandle[sessionHandleKey]) {
                sessionHandle = this._casCiSessionHandle[sessionHandleKey].handle;
            }
            if (sessionHandle) {
                hx.log("dsrvCasCI", hx.util.format("closeSession({0},{1},{2},{3})", moduleEnum, slot, sessionEnum, sessionHandle));
                that.oipfCasUtilObj.closeSession(moduleEnum, slot, sessionEnum, sessionHandle);
                this._casCiSessionHandle[sessionHandleKey] = null;
            }
            else {
                hx.log('error', '[dsrvCasCI] _close_session error');
                return;
            }
        };
        cDLCasCI.prototype._set_response = function (module, slot, type, resData, sessionHandleKey) {
            var that = this //
            , moduleIndex = that._CAModule.indexOf(module) //
            , moduleEnum = that._CAGroupId[moduleIndex] //
            , sessionEnum, sessionHandle, resString;
            sessionEnum = that._CANewSessionType[type];
            hx.log('dsrvCasCI', '[dsrvCasCI] _set_response() called!');
            resString = JSON.stringify(resData);
            if ((moduleEnum === -1) || (sessionEnum === -1) || !resString) {
                hx.log('error', '[dsrvCasCI] _set_response error');
                return;
            }
            if (this._casCiSessionHandle[sessionHandleKey]) {
                sessionHandle = this._casCiSessionHandle[sessionHandleKey].handle;
            }
            if (sessionHandle) {
                that.oipfCasUtilObj.sendUiResponse(moduleEnum, slot, sessionEnum, sessionHandle, resString);
            }
            else {
                hx.log('error', '[dsrvCasCI] _set_response error');
                return;
            }
        };
        cDLCasCI.prototype._init = function () {
            var _this = this;
            hx.log("dsvc", "hx.svc.CAS_CI's _init() called");
            //        hx.svc.CAS_CI._initEventHandler();
            var a = this;
            a._state = 'preparing';
            hx.svc.AV_CTRL.addEventCb('CasMessageNotify', this, function (aParam) {
                _this._on_CasMessageNotify(aParam);
            });
            // MMI Trigger
            hx.svc.AV_CTRL.addEventCb('CasUiNotify', this, function (aParam) {
                _this._on_CasUiNotify(aParam);
            });
            a.oipfCasUtilObj.onCasUiEvent = function (module, slot, target, session, eventType, uiData) {
                hx.log("dsrvCasCI", "raw uiData string " + uiData);
                var uidataObj;
                try {
                    uidataObj = JSON.parse(uiData);
                }
                catch (e) {
                    hx.log('error', '[dsrvCasCI] onCasUiEvent json parse error');
                }
                if (uidataObj) {
                    var moduleIndex = a._CAGroupId.indexOf(module);
                    var moduleStr = a._CAModule[moduleIndex];
                    var fromStr;
                    var key;
                    for (key in a._CANewSessionType) {
                        if (a._CANewSessionType[key] === target) {
                            fromStr = key;
                            break;
                        }
                    }
                    var param = {
                        'module': moduleStr,
                        'slot': slot,
                        'from': fromStr,
                        'session': session,
                        'eventType': eventType,
                        'uiData': uidataObj
                    };
                    hx.log("dsrvCasCI", "module " + moduleStr + " slot " + slot + " target - " + fromStr + " session " + session + " eventType " + eventType);
                    hx.log("dsrvCasCI", "uiData - \n" + uiData);
                    //                    a.fireEvent_Legacy('CasUiEvent', {
                    //                        'param' : param
                    //                    });
                    //setTimeout(function(){
                    a._on_CasUiEvent(param);
                }
                else {
                    hx.log('error', '[dsrvCasCI] there is no uidataObj');
                }
            };
            a.localSystemOipfObj.onCamStateChange = function (slotNum, camStatus) {
                hx.log("dsrvCasCI", '[dsrvCasCI] onCamStateChange evemt called slotNum : [' + slotNum + ']');
                $.each(a._slotInfo.slot, function (i, v) {
                    if (v.num === slotNum) {
                        var slot = v;
                        slot.status = a._camStatus[camStatus];
                        if (slot.status === 'INITIALIZED') {
                            a._refresh_slotInfo(function () {
                                a.fireEvent('UpdateCiSlotInfo', a._slotInfo);
                            });
                        }
                        else {
                            a.fireEvent('UpdateCiSlotInfo', a._slotInfo);
                        }
                        return false;
                    }
                });
            };
            a.localSystemOipfObj.onSmartcardStateChanged = function (slotNum, scState) {
                hx.log("dsrvCasCI", '[dsrvCasCI] onSmartcardStateChanged evemt called slotNum : [' + slotNum + '] / scStatus : [' + scState + ']');
                a.fireEvent('SmartcardStateChanged', {
                    slot: slotNum,
                    state: scState
                });
            };
            a.localSystemOipfObj.onCasProfileRequest = function (actionType, slotId, casId, data) {
                hx.log("dsrvCasCI", '[dsrvCasCI] actionType : [' + actionType + '],' + '  slotId : [' + slotId + '],' + '  casId : [' + casId + '],' + '  data : [' + data + ']');
                var actionTypeNum = actionType;
                var slotNum = slotId;
                var casNum = casId;
                var dataStr = JSON.parse(data);
                var param = {
                    'actionType': actionTypeNum,
                    'slotId': slotNum,
                    'casId': casNum,
                    'data': dataStr
                };
                a.fireEvent('UpdateCiProfileInfo', param);
            };
            a._refresh_slotInfo(function (slotList) {
                a._state = 'ready';
                $.each(slotList, function (i, v) {
                    a._open_session('CI', v.num, 'NATIVE');
                });
                var list = a._pActList;
                var i, ti;
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    a._execute_action(list.shift());
                }
            });
        };
        /*********** Internal Function End ****************/
        /*********** Event Listener Start ****************/
        cDLCasCI.prototype._on_CasMessageNotify = function (o) {
            var that = this;
            var caUiDataObj = o.param.uiData;
            hx.log('dsrvCasCI', '[dsrvCasCI] _on_CasMessageNotify() called! with ' + JSON.stringify(caUiDataObj));
            if (caUiDataObj.castype === "ir") {
                that.fireEvent('CasIrSystemMessage', {
                    'message': caUiDataObj.uidata.string,
                    'sessionHandle': o.param.seesionHandle
                });
            }
            else if (caUiDataObj.castype === "nagra") {
                that.fireEvent('CasNaSystemMessage', {
                    'message': caUiDataObj.uidata.string,
                    'sessionHandle': o.param.seesionHandle
                });
            }
            else {
                that.fireEvent('CiSystemMessage', {
                    'message': caUiDataObj.uidata.string
                });
            }
        };
        cDLCasCI.prototype._on_CasUiNotify = function (o) {
            var that = this;
            //hx.log('dsrvCasCI', '[dsrvCasCI] _on_CasUiNotify() called!');
            //that._open_session(that._moduleName, o.param.slot, 'CUSTOM');
        };
        cDLCasCI.prototype._on_CasUiEvent = function (o) {
            var that = this;
            var extKey;
            hx.log('dsrvCasCI', '[dsrvCasCI] _on_CasUiEvent() called! o.module : ' + o.module);
            that._clearMmiTimeout();
            if (hx.il.isCriticalTasking()) {
                if (o.from === 'EXTENSION') {
                    hx.log('dsrvCasCI', '[dsrvCasCI] _on_CasUiEvent() Extension Session Type should be closed on M/W');
                }
                else {
                    var key, isFindSessionToClose;
                    for (key in that._casCiSessionHandle) {
                        if (that._casCiSessionHandle[key]) {
                            if (that._casCiSessionHandle[key].handle === o.session) {
                                isFindSessionToClose = true;
                                that._close_session(o.module, o.slot, o.from, key);
                                break;
                            }
                        }
                    }
                    if (isFindSessionToClose !== true) {
                        hx.log('dsrvCasCI', '[dsrvCasCI] _on_CasUiEvent() fail To findSessionHandle To Close');
                    }
                }
            }
            else {
                var slotInfo, slotName;
                switch (o.module) {
                    case 'CI':
                        slotInfo = that.__get_slotInfo(o.slot);
                        if (slotInfo) {
                            slotName = slotInfo.name;
                        }
                        if (o.from === 'NATIVE') {
                            that.fireEvent('CiNativeUiEvent', {
                                'from': o.from,
                                'module': o.module,
                                'slot': o.slot,
                                'name': slotName,
                                'data': o.uiData
                            });
                        }
                        else {
                            extKey = "CI_EXTENSION_" + o.session;
                            if (!that._casCiSessionHandle[extKey]) {
                                that._casCiSessionHandle[extKey] = that._typeOfSessionHandle;
                                that._casCiSessionHandle[extKey].handle = o.session;
                                that._casCiSessionHandle[extKey].module = o.module;
                            }
                            that.fireEvent('CiMMIEvent', {
                                'from': o.from,
                                'module': o.module,
                                'slot': o.slot,
                                'name': slotName,
                                'eventType': o.eventType,
                                'data': o.uiData,
                                'sessionKey': extKey
                            });
                        }
                        break;
                    case 'IR':
                    case 'IRUC':
                        slotInfo = that.__get_slotInfo(o.slot);
                        if (slotInfo) {
                            slotName = slotInfo.name;
                        }
                        if (o.from === 'MENU') {
                            hx.log('dsrvCasCI', '[dsrvCasCI] o.from === MENU');
                            that.fireEvent('CasIrMenuUiEvent', {
                                'from': o.from,
                                'module': o.module,
                                'data': o.uiData,
                                'sessionKey': sessionKey,
                                'eventType': o.eventType
                            });
                        }
                        else if (o.from === 'EXTENSION') {
                            extKey = "IRUC_EXTENSION_" + o.session;
                            if (!that._casCiSessionHandle[extKey]) {
                                that._casCiSessionHandle[extKey] = that._typeOfSessionHandle;
                                that._casCiSessionHandle[extKey].handle = o.session;
                                that._casCiSessionHandle[extKey].module = o.module;
                            }
                            that.fireEvent('CasIrNativeUiEvent', {
                                'from': o.from,
                                'module': o.module,
                                'slot': o.slot,
                                'name': slotName,
                                'data': o.uiData,
                                'sessionKey': sessionKey,
                                'eventType': o.eventType
                            });
                        }
                        else {
                            that.fireEvent('CiMMIEvent', {
                                'from': o.from,
                                'module': o.module,
                                'slot': o.slot,
                                'name': slotName,
                                'data': o.uiData
                            });
                        }
                        break;
                    case 'NA':
                        if (o.from === 'MENU') {
                            that.fireEvent('CasNaMenuUiEvent', {
                                'from': o.from,
                                'module': o.module,
                                'data': o.uiData,
                                'eventType': o.eventType
                            });
                        }
                        else if ((o.from === 'CUSTOM') || (o.from === 'EXTENSION')) {
                            extKey = "NAGRA_EXTENSION_" + o.session;
                            if (!that._casCiSessionHandle[extKey]) {
                                that._casCiSessionHandle[extKey] = that._typeOfSessionHandle;
                                that._casCiSessionHandle[extKey].handle = o.session;
                                that._casCiSessionHandle[extKey].module = o.module;
                            }
                            that.fireEvent('CasNaCustomUiEvent', {
                                'message': o.uiData.uidata.string,
                                'subtitle': o.uiData.uidata.subtitle,
                                'uidata': o.uiData.uidata,
                                'uitype': o.uiData.uitype,
                                'sessionKey': extKey,
                                'eventType': o.eventType
                            });
                        }
                        break;
                    default:
                        var sessionKey;
                        for (sessionKey in that._casCiSessionHandle) {
                            if (that._casCiSessionHandle[sessionKey]) {
                                if (that._casCiSessionHandle[sessionKey].handle === o.session) {
                                    if (that._casCiSessionHandle[sessionKey].module === 'NA') {
                                        that.fireEvent('CasNaCustomUiEvent', {
                                            'message': o.uiData.uidata.string,
                                            'subtitle': o.uiData.uidata.subtitle,
                                            'uidata': o.uiData.uidata,
                                            'sessionKey': sessionKey,
                                            'eventType': o.eventType
                                        });
                                    }
                                    else if (that._casCiSessionHandle[sessionKey].module === 'IRUC') {
                                        that.fireEvent('CasIrNativeUiEvent', {
                                            'message': o.uiData.uidata.string,
                                            'subtitle': o.uiData.uidata.subtitle,
                                            'uidata': o.uiData.uidata,
                                            'sessionKey': sessionKey,
                                            'eventType': o.eventType
                                        });
                                    }
                                    else if (that._casCiSessionHandle[sessionKey].module === 'CI') {
                                        that.fireEvent('CiMMIEvent', {
                                            'eventType': o.eventType,
                                            'data': o.uiData
                                        });
                                    }
                                }
                            }
                        }
                        hx.log('dsrvCasCI', '[_on_CasUiEvent] Error! Invalid module');
                        break;
                }
            }
        };
        /*********** Event Listener End ****************/
        /*********** External API Start ****************/
        //hx.logadd('dsrvCasCI');
        cDLCasCI.prototype.getCISlotInfo = function () {
            var that = this;
            hx.log('dsrvCasCI', '[dsrvCasCI] getCISlotInfo() called!');
            this.localSystemOipfObj.registerEventListener();
            that._refresh_slotInfo(function () {
                that.fireEvent('UpdateCiSlotInfo', that._slotInfo);
            });
        };
        cDLCasCI.prototype.requestMmi = function (param) {
            var that = this;
            var idx = param.slot;
            var i, ti, list = that._slotInfo.slot;
            hx.log('dsrvCasCI', '[dsrvCasCI] requestMmi() called!');
            for (i = 0, ti = list.length; i < ti; i += 1) {
                if (list[i].num === idx) {
                    if (list[i].status === 'INITIALIZED') {
                        that._open_session(that._moduleName, idx, 'MENU');
                    }
                    that._setMmiTimeout('MENU', 'CI', idx);
                    break;
                }
            }
        };
        cDLCasCI.prototype.setInput = function (data) {
            var that = this;
            var resData = {};
            hx.log('dsrvCasCI', '[dsrvCasCI] setInput() called!');
            resData.inputtype = data.type;
            var inputdata;
            var sessionHandleKey = data.sessionHandleKey;
            switch (resData.inputtype) {
                case 'button':
                    inputdata = {
                        'buttontype': data.input
                    };
                    break;
                case 'selection':
                    inputdata = {
                        'itemidx': data.input
                    };
                    break;
                case 'etc':
                    inputdata = {
                        'etctype': data.input
                    };
                    break;
                case 'number':
                    inputdata = {
                        'number': 0,
                        'numberlength': data.input.length,
                        'numberstring': data.input
                    };
                    break;
            }
            resData.inputdata = inputdata;
            that._set_response(data.module, data.slot, data.target, resData, sessionHandleKey);
            //that._setMmiTimeout(data.target, data.module, data.slot);
        };
        cDLCasCI.prototype.closeMmi = function (data) {
            var that = this;
            hx.log('dsrvCasCI', '[dsrvCasCI] closeMmi() called!');
            that.__close_mmi(data.target, data.module, data.slot, data.sessionHandleKey);
        };
        cDLCasCI.prototype.requestMmiforCI = function (param) {
            var that = this;
            var idx = param.slot;
            var i, ti, list = that._slotInfo.slot;
            var sessionHandleKey = param.sessionHandleKey;
            hx.log('dsrvCasCI', '[dsrvCasCI] requestMmiforCI() called!');
            for (i = 0, ti = list.length; i < ti; i += 1) {
                if (list[i].num === idx) {
                    if (list[i].status === 'INITIALIZED') {
                        that._open_session('CI', idx, 'MENU', null, sessionHandleKey);
                    }
                    that._setMmiTimeout('MENU', 'CI', idx);
                    break;
                }
            }
        };
        cDLCasCI.prototype.requestMmiforCas = function (param) {
            var that = this;
            var idx = param.slot;
            var menugroup = param.menu;
            var sessionHandleKey = param.sessionHandleKey;
            hx.log('dsrvCasCI', '[dsrvCasCI] requestMmiforCas() called!');
            //hx.log('dsrvCasCI', '[dsrvCasCI] that._moduleName : ' + that._moduleName + 'idx : ' + idx + 'menugroup : ' + menugroup + 'sessionHandleKey : ' + sessionHandleKey);
            that._open_session(that._moduleName, idx, 'MENU', menugroup, sessionHandleKey);
        };
        cDLCasCI.prototype.requestCIBannerSession = function (param) {
            var that = this;
            var idx = param.slot;
            var sessionHandleKey = param.sessionHandleKey;
            hx.log('dsrvCasCI', '[dsrvCasCI] requestCIBannerSession() called!');
            that._open_session('CI', idx, 'BANNER', null, sessionHandleKey);
        };
        cDLCasCI.prototype.requestBannerSession = function (param) {
            var that = this;
            var idx = param.slot;
            var sessionHandleKey = param.sessionHandleKey;
            hx.log('dsrvCasCI', '[dsrvCasCI] requestBannerSessionforCas() called!');
            that._open_session(that._moduleName, idx, 'BANNER', null, sessionHandleKey);
        };
        // CAS - NAGRA ============================
        cDLCasCI.prototype.getSessionHandle = function (param) {
            var that = this;
            var handle;
            hx.log('dsrvCasCI', hx.util.format('[getSessionHandle] Session Key : {0}', param.sessionKey));
            if (param && param.sessionKey) {
                if (that._casCiSessionHandle[param.sessionKey]) {
                    handle = that._casCiSessionHandle[param.sessionKey].handle;
                    hx.log('dsrvCasCI', hx.util.format('[getSessionHandle] Session Handle : {0}', handle));
                }
                else {
                    hx.log('dsrvCasCI', '[getSessionHandle] fail to get Session Handle');
                }
            }
            return handle;
        };
        cDLCasCI.prototype.isSCInserted = function () {
            var that = this;
            //var bSCInserted = that.oipfCasUtilObj.isSCInserted();
            var bSCInserted = true;
            hx.log('dsrvCasCI', hx.util.format('[isSCInserted] bSCInserted : {0}', bSCInserted));
            return bSCInserted;
        };
        cDLCasCI.prototype.verifySCPincode = function (param) {
            var that = this;
            //var accessibleInfo = that.oipfCasUtilObj.veritySCPincode(param);
            var accessibleInfo = {
                accessState: 0,
                remainTime: 0,
                retryCount: 0
            };
            // temporary
            hx.log('dsrvCasCI', hx.util.format('[veritySCPincode] accessibleInfo(state/time/cnt) : {0}/{1}/{2}', accessibleInfo.accessState, accessibleInfo.remainTime, accessibleInfo.retryCount));
            return accessibleInfo;
        };
        cDLCasCI.prototype.setSCPincode = function (param) {
            var that = this;
            hx.log('dsrvCasCI', hx.util.format('[setSCPincode] new SCPincode : {0}', param));
            //return that.oipfCasUtilObj.setSCPincode(param);
            return true;
        };
        // CAS - IRDETO ===========================
        cDLCasCI.prototype.requestMmiforIrdeto = function (param) {
            var that = this;
            var idx = param.slot;
            var menugroup = param.menu;
            hx.log('dsrvCasCI', '[dsrvCasCI] requestMmiforCas() called!');
            that._open_session(that._moduleName, idx, 'MENU', menugroup);
        };
        return cDLCasCI;
    })(__KERNEL__);
    return cDLCasCI;
});
