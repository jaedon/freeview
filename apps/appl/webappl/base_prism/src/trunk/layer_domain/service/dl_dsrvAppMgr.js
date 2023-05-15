/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/prism.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAppMgr.js
/*jslint nomen:true, vars:true */
//    APP_MGR: ['layer_domain/service/dl_dsrvAppMgr.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "def/oipf.def", "lib/local/app_rpc_util"], function (require, exports, __KERNEL__, __DEF__, __APP_RPC_UTIL__) {
    //var WIDGET_INSTALLATION_STARTED = 0;
    //var WIDGET_INSTALLATION_COMPLETED = 1;
    //var WIDGET_INSTALLATION_FAILED = 2;
    //var WIDGET_UNINSTALLATION_STARTED = 3;
    //var WIDGET_UNINSTALLATION_COMPLETED = 4;
    //var WIDGET_UNINSTALLATION_FAILED = 5;
    //var WIDGET_ERROR_STORAGE_AREA_FULL = 10;
    //var WIDGET_ERROR_DOWNLOAD = 11;
    //var WIDGET_ERROR_INVALID_ZIP_ARCHIVE = 12;
    //var WIDGET_ERROR_INVALID_SIGNATURE = 13;
    //var WIDGET_ERROR_GENERIC = 14;
    //var WIDGET_ERROR_SIZE_EXCEEDED = 15;
    var dbgStr = {
        0: 'INSTALL STARTED',
        1: 'INSTALL COMPLETED',
        2: 'INSTALL FAILED',
        3: 'UNINSTALL STARTED',
        4: 'UNINSTALL COMPLETED',
        5: 'UNINSTALL FAILED',
        10: 'ERR: STORAGE AREA FULL',
        11: 'ERR: DOWNLOAD',
        12: 'ERR: INVALID ZIP ARCHIVE',
        13: 'ERR: INVALID SIGNATURE',
        14: 'ERR: GENERIC',
        15: 'ERR: SIZE EXCEEDED'
    };
    var fnLog = function (msg) {
        hx.log('dsrvAppMgr', msg);
    };
    var mClientList = [];
    window.addEventListener('message', function (messageEvent) {
        var source = messageEvent.source;
        var message = messageEvent.data;
        if (message.indexOf('iwc.') === 0) {
            var msgIwc = JSON.parse(message.slice(4, message.length));
            var type = msgIwc.type;
            var fname = msgIwc.fname;
            var args = msgIwc.args;
            var i, len, client;
            for (i = 0, len = mClientList.length; i < len; i += 1) {
                client = mClientList[i];
                if (client._window === source) {
                    var cbList = client._notifyList[fname];
                    var j, len2;
                    for (j = 0, len2 = cbList.length; j < len2; j += 1) {
                        cbList[j].apply(client, args);
                    }
                }
            }
        }
    });
    var cDLAppMgr = (function (_super) {
        __extends(cDLAppMgr, _super);
        function cDLAppMgr(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mAppMgr = this.getPluginInstance("ApplicationManager");
            this.mAppMain = this.mAppMgr.getOwnerApplication(document);
            this.mAppInfoMain = {
                name: 'MAIN',
                param: {},
                app: this.mAppMain
            };
            this.mAppInfoList = [];
            this.mBuiltInApps = [];
            this.doNotHandleBuiltInApp = false;
            this.mGlobalKeyCaptureList = [];
            this.mOnGoingInstallation = [];
            this.mParentUrl = location.href;
            this.mBaseUrl = this.mParentUrl.substr(0, location.href.lastIndexOf("/") + 1);
            this.mMessageSuffix = ",url:" + this.mParentUrl;
            this.mAppZOrder = [];
            this.mPrefixForHbbTV = "HbbTV";
            this.defineEvents(['WidgetCreated', 'WidgetDestroyed', 'WidgetInstalled', 'ApplicationLoadError', 'ApplicationStart', 'ApplicationEnd', 'ApplicationHide', 'HbbTVAppUnloaded', 'HbbTvAppEnter', 'HbbTVAppLoaded']);
            hx.logadd('dsrvAppMgr');
            this.mAppMgr.addEventListener('ApplicationLoaded', function (e) {
                _this.onApplicationLoaded(e.appl);
            });
            this.mAppMgr.addEventListener('ApplicationLoadError', function (e) {
                _this.onApplicationLoadError(e.appl);
            });
            this.mAppMgr.addEventListener('ApplicationUnloaded', function (e) {
                _this.onApplicationUnloaded(e.appl);
            });
            this.mAppMgr.addEventListener('HumaxApplicationLoaded', function (e) {
                _this.onHumaxApplicationLoaded(e.name);
            });
            this.mAppMgr.addEventListener('HumaxApplicationUnloaded', function (e) {
                _this.onHumaxApplicationUnloaded(e.name);
            });
            this.mAppMgr.addEventListener('WidgetInstallation', function (e) {
                _this.onWidgetInstallation(e.wd, e.state, e.reason);
            });
            this.mAppMgr.addEventListener('WidgetUnInstallation', function (e) {
                _this.onWidgetUnInstallation(e.wd, e.state);
            });
            this.mAppInfoList.push(this.mAppInfoMain);
            this.fnActivateApp(this.mAppInfoMain);
            this.createApplication({
                name: 'GLOBAL_DLG',
                url: 'builtin_apps/global_dialog/global_dialog.html',
                builtIn: true,
                onLoaded: function (appRpcUtil) {
                    prism.setGlobalDialogApi(appRpcUtil);
                }
            });
        }
        cDLAppMgr.prototype.fnFindAppInfo = function (key) {
            var app = null, name = null, type = null;
            //fnLog('fnFindAppInfo: ' + key);
            if (typeof key === 'string') {
                name = key;
                if (key.indexOf(this.mPrefixForHbbTV) >= 0) {
                    type = this.mPrefixForHbbTV;
                }
            }
            else {
                app = key;
            }
            var i;
            for (i = 0; i < this.mAppInfoList.length; i += 1) {
                var appInfo = this.mAppInfoList[i];
                if ((app && app === appInfo.app) || (name && name === appInfo.name) || (type && type === appInfo.type)) {
                    return i;
                }
            }
            return -1;
        };
        cDLAppMgr.prototype.fnGetAppInfo = function (key) {
            var index = this.fnFindAppInfo(key);
            return index === -1 ? null : this.mAppInfoList[index];
        };
        cDLAppMgr.prototype.fnRemoveAppInfo = function (key) {
            var index = this.fnFindAppInfo(key);
            if (index !== -1) {
                this.mAppInfoList.splice(index, 1);
                return true;
            }
            return false;
        };
        cDLAppMgr.prototype.fnUnRegisterGlobalKeyCapture = function (handle) {
            throw "Array does not have remove method";
            this.mGlobalKeyCaptureList['remove'](handle);
        };
        cDLAppMgr.prototype.fnGetActiveWidget = function () {
            var i;
            for (i = 0; i < this.mAppInfoList.length; i += 1) {
                var appInfo = this.mAppInfoList[i];
                if (appInfo.wd) {
                    return appInfo;
                }
            }
            return null;
        };
        cDLAppMgr.prototype.fnFindOngoingInstallRequest = function (downloadURI) {
            var i;
            for (i = 0; i < this.mOnGoingInstallation.length; i += 1) {
                var info = this.mOnGoingInstallation[i];
                if (info.url === downloadURI) {
                    return info;
                }
            }
            return null;
        };
        //  Event handlers
        cDLAppMgr.prototype.fnRemoveAppZOrder = function (appl) {
            var i = this.mAppZOrder.indexOf(appl);
            if (i !== -1) {
                this.mAppZOrder.splice(i, 1);
                return true;
            }
            return false;
        };
        cDLAppMgr.prototype.fnRecoverPrevReceiver = function () {
            try {
                var appl = this.mAppZOrder.pop();
                if (appl) {
                    fnLog('fnRecoverPrevReceiver PREV');
                    appl.activateInput(true);
                }
                else {
                    fnLog('fnRecoverPrevReceiver MAIN');
                    this.mAppMain.activateInput(true);
                }
            }
            catch (e) {
                fnLog('fnRecoverPrevReceiver MAIN due to EXCEPTION');
                this.mAppMain.activateInput(true);
            }
        };
        cDLAppMgr.prototype.onApplicationLoaded = function (appl) {
            var _this = this;
            var appInfo = this.fnGetAppInfo(appl), appName;
            if (appInfo === null) {
                appName = 'App';
            }
            else {
                appName = appInfo.name;
            }
            this.fireEvent('ApplicationStart', appName);
            appl.onApplicationPrimaryReceiver = (function (appl, appInfo) {
                return function () {
                    if (appInfo) {
                        fnLog('onApplicationPrimaryReceiver [' + appInfo.name + ']');
                    }
                    else {
                        fnLog('onApplicationPrimaryReceiver UNKNOWN');
                    }
                    _this.fnRemoveAppZOrder(appl);
                    _this.mAppZOrder.push(appl);
                };
            })(appl, appInfo);
            appl.onApplicationDeactivated = (function (appl, appInfo) {
                return function () {
                    if (appInfo) {
                        fnLog('onApplicationDeactivated [' + appInfo.name + ']');
                    }
                    else {
                        fnLog('onApplicationDeactivated UNKNOWN');
                    }
                    _this.fnRemoveAppZOrder(appl);
                };
            })(appl, appInfo);
            if (appInfo) {
                fnLog('onApplicationLoaded: [' + appInfo.name + ']');
                hx.log("info", 'onApplicationLoaded: [' + appInfo.name + ']');
                setTimeout(function () {
                    hx.log("info", 'call onLoaded after setTimeout 1000 : [' + appInfo.name + ']');
                    if (appInfo.param.onLoaded) {
                        if (appInfo.param.builtIn) {
                            var buildinAppInfo = new __APP_RPC_UTIL__(appInfo);
                            appInfo.param.onLoaded(buildinAppInfo);
                            mClientList.push(buildinAppInfo);
                        }
                        else {
                            appInfo.param.onLoaded(appInfo);
                        }
                        appInfo.param.onLoaded = null;
                    }
                }, 1000);
                this.fnActivateApp(appInfo);
                if (appInfo.wd) {
                    fnLog('WidgetCreated [' + appInfo.name + ']');
                    this.fireEvent('WidgetCreated', appInfo.wd);
                }
                appl.onKeyDown = (function (appInfo) {
                    return function (e) {
                        _this.onApplicationKeyDown(appInfo, e);
                    };
                })(appInfo);
            }
            else {
                fnLog('onApplicationLoaded: UNKNOWN');
                appl.onKeyDown = (function (appInfo) {
                    return function (e) {
                        _this.onApplicationKeyDown(appInfo, e);
                    };
                })({
                    name: 'UNKOWN',
                    app: appl
                });
            }
        };
        cDLAppMgr.prototype.onApplicationLoadError = function (app) {
            fnLog('onApplicationLoadError');
            var infoIdx = this.fnFindAppInfo(app);
            if (this.mAppInfoList) {
                var appInfo = this.mAppInfoList[infoIdx];
                if (appInfo) {
                    this.mAppInfoList.splice(infoIdx, 1);
                }
            }
            this.mAppMain.activateInput(true);
            this.fireEvent('ApplicationLoadError', appInfo);
            hx.svc.SETTING_UTIL.doingKeyGrab(false);
        };
        cDLAppMgr.prototype.onApplicationUnloaded = function (app) {
            var appInfo = this.fnGetAppInfo(app), i, builtInApps, appTemp, appName;
            if (appInfo === null) {
                appName = 'App';
            }
            else {
                appName = appInfo.name;
            }
            this.fireEvent('ApplicationEnd', appName);
            if (appInfo) {
                fnLog('onApplicationUnloaded [' + appInfo.name + ']');
                this.mAppMgr.removeOIPFApplication(appInfo.param.url);
                if (appInfo.param.onUnloaded) {
                    appInfo.param.onUnloaded(appInfo);
                }
                if (appInfo.wd) {
                    fnLog('WidgetDestroyed [' + appInfo.name + ']');
                    this.fireEvent('WidgetDestroyed', appInfo.wd);
                }
                this.fnRemoveAppInfo(app);
            }
            else {
                fnLog('onApplicationUnloaded UNKNOWN');
            }
            if (this.fnRemoveAppZOrder(app)) {
                this.fnRecoverPrevReceiver();
            }
            hx.svc.SETTING_UTIL.doingKeyGrab(false);
        };
        cDLAppMgr.prototype.onHumaxApplicationLoaded = function (name) {
            hx.log('info', '[onHumaxApplicationLoaded] name :' + name);
            var appInfo = this.fnGetAppInfo(name), appName;
            if (!appInfo) {
                appName = 'App';
            }
            else {
                appName = appInfo.name;
            }
            if (appInfo) {
                if (name === 'HbbTV Application') {
                    this.fireEvent('HbbTvAppLoaded');
                    var dsrvAvCtrl = hx.svc.AV_CTRL;
                    dsrvAvCtrl.naviChannel({
                        'direction': 'block'
                    });
                    if (appInfo.param && appInfo.param.onHumaxApplicationLoaded) {
                        appInfo.param.onHumaxApplicationLoaded(appInfo);
                    }
                }
                else {
                    fnLog('onHumaxApplicationLoaded : Not HbbTV Application');
                }
            }
            else {
                fnLog('onHumaxApplicationLoaded UNKNOWN');
            }
        };
        cDLAppMgr.prototype.onHumaxApplicationUnloaded = function (name) {
            hx.log('info', '[onHumaxApplicationUnloaded] name :' + name);
            var appInfo = this.fnGetAppInfo(name), appName;
            if (!appInfo) {
                appName = 'App';
            }
            else {
                appName = appInfo.name;
            }
            if (appInfo) {
                fnLog('[[onHumaxApplicationUnloaded]] [' + appInfo.name + ']');
                if (name === 'HbbTV Application') {
                    this.fireEvent('HbbTVAppUnloaded');
                    if (appInfo.param && appInfo.param.onHumaxApplicationUnloaded) {
                        appInfo.param.onHumaxApplicationUnloaded(appInfo);
                    }
                    this.fnRemoveAppInfo(name);
                    hx.svc.AV_CTRL.naviChannel({
                        'direction': 'unBlock'
                    });
                }
                else {
                    fnLog('onHumaxApplicationUnloaded : Not HbbTV Application');
                }
            }
            else {
                fnLog('onHumaxApplicationUnloaded UNKNOWN');
            }
        };
        cDLAppMgr.prototype.fnCheckAndDeliverGlobalKey = function (keyCode) {
            var i, len;
            for (i = 0, len = this.mGlobalKeyCaptureList.length; i < len; i += 1) {
                var info = this.mGlobalKeyCaptureList[i];
                if (info.keyList.indexOf(keyCode) !== -1) {
                    try {
                        fnLog('fnCheckAndDeliverGlobalKey: KEY=' + keyCode + ' GLOBAL CATCH');
                        info.fn(keyCode);
                    }
                    catch (e) {
                        fnLog('fnCheckAndDeliverGlobalKey: KEY=' + keyCode + ' GLOBAL CATCH EXCEPTION: ' + e.message);
                    }
                }
            }
        };
        cDLAppMgr.prototype.onApplicationKeyDown = function (appInfo, evt) {
            var keyCode = evt.keyCode;
            var name = appInfo.name;
            var primaryReceiver = appInfo.app.isPrimaryReceiver;
            var bDoSendGlobalKey = false;
            fnLog('onApplicationKeyDown: [' + name + '] KEY=' + keyCode + ' primaryReceiver=' + primaryReceiver);
            if (appInfo.param && appInfo.param.onKeyDown) {
                appInfo.param.onKeyDown(appInfo, keyCode);
                if (appInfo.bKeyDontSendGlobal && appInfo.bKeyDontSendGlobal === false) {
                    bDoSendGlobalKey = true;
                }
            }

            if (name !== 'MAIN' && primaryReceiver && bDoSendGlobalKey === true) {
                this.fnCheckAndDeliverGlobalKey(keyCode);
            }
            else if (name === 'GLOBAL_DLG' && primaryReceiver) {
                this.fnCheckAndDeliverGlobalKey(keyCode);
            }
        };
        cDLAppMgr.prototype.onWidgetInstallation = function (wd, state, reason) {
            fnLog('onWidgetInstallation [' + wd.name + ', ' + dbgStr[state] + ', ' + dbgStr[reason] + ']');
            var ongoingRequest = this.fnFindOngoingInstallRequest(wd.downloadURI);
            if (ongoingRequest) {
                if (ongoingRequest.onStarted) {
                    if (state === __DEF__.WIDGET_INSTALLATION_STARTED) {
                        ongoingRequest.onStarted();
                    }
                }
                if (ongoingRequest.onCompleted) {
                    if (state === __DEF__.WIDGET_INSTALLATION_COMPLETED) {
                        ongoingRequest.onCompleted(true);
                    }
                    else if (state === __DEF__.WIDGET_INSTALLATION_FAILED) {
                        ongoingRequest.onCompleted(false);
                    }
                }
            }
            else {
                if (state === __DEF__.WIDGET_INSTALLATION_COMPLETED) {
                    this.fireEvent('WidgetInstalled', wd);
                }
            }
        };
        cDLAppMgr.prototype.onWidgetUnInstallation = function (wd, state) {
            fnLog('onWidgetUnInstallation [' + wd.name + ', ' + dbgStr[state] + ']');
        };
        cDLAppMgr.prototype.fnActivateApp = function (appInfo) {
            fnLog('fnActivateApp: [' + appInfo.name + ']');
            if (appInfo.param.builtIn) {
                this.hideApplication({
                    appInfo: appInfo
                });
            }
            else {
                this.showApplication({
                    appInfo: appInfo
                });
            }
        };
        cDLAppMgr.prototype.registerGlobalKeyCapture = function (aKeyList, aFn) {
            var globalKeyCaptureInfo = {
                fn: aFn,
                keyList: aKeyList
            };
            this.mGlobalKeyCaptureList.push(globalKeyCaptureInfo);
            return globalKeyCaptureInfo;
        };
        cDLAppMgr.prototype.createApplication = function (param) {
            if (param.name === undefined) {
                throw '"name" is required';
            }
            if (param.url === undefined) {
                throw '"url" is required';
            }
            var urlParam = param.url;
            if (param.builtIn) {
                param.url = this.mBaseUrl + param.url;
                urlParam = param.url + '?appName=' + param.name + '&parentUrl=' + this.mParentUrl;
            }
            else {
                this.mAppMgr.addOIPFApplication(param.url);
            }
            var app = this.mAppMain.createApplication(urlParam, true);
            this.mAppInfoList.push({
                name: param.name,
                param: param,
                app: app
            });
        };
        cDLAppMgr.prototype.showApplication = function (param) {
            if (param.appInfo === undefined) {
                throw '"appInfo" is required';
            }
            var appInfo;
            var name = param.appInfo.name;
            var app = param.appInfo.app;
            fnLog('fnShowApplication: [' + name + ']');
            if (!app) {
                appInfo = this.fnGetAppInfo(name);
                if (appInfo) {
                    app = appInfo.app;
                }
            }
            if (app) {
                hx.svc.SETTING_UTIL.doingKeyGrab(true);
                app.activateInput(true);
                app.show();
            }
        };
        cDLAppMgr.prototype.hideApplication = function (param) {
            if (param.appInfo === undefined) {
                throw '"appInfo" is required';
            }
            var appInfo;
            var name = param.appInfo.name;
            var app = param.appInfo.app;
            fnLog('fnHideApplication: [' + name + ']');
            if (!app) {
                appInfo = this.fnGetAppInfo(name);
                if (appInfo) {
                    app = appInfo.app;
                }
            }
            if (app) {
                this.fireEvent('ApplicationHide');
                this.fnRemoveAppZOrder(app);
                app.deactivateInput();
                app.hide();
            }
            if (param.appInfo.param && param.appInfo.param.builtIn) {
                this.fnRecoverPrevReceiver();
            }
        };
        cDLAppMgr.prototype.sendMessage = function (param) {
            if (param.appInfo === undefined) {
                throw '"appInfo" is required';
            }
            if (param.message === undefined) {
                throw '"message" is required';
            }
            var app = param.appInfo.app;
            var message = param.message + this.mMessageSuffix;
            fnLog('SEND MSG[' + param.appInfo.name + ']: ' + message);
            app.window.postMessage(param.message + this.mMessageSuffix, param.appInfo.param.url);
        };
        cDLAppMgr.prototype.destroyApplication = function (param) {
            if (param.name === undefined) {
                throw '"name" is required';
            }
            var appInfo = this.fnGetAppInfo(param.name);
            if (appInfo) {
                appInfo.app.onKeyDown = null;
                appInfo.app.destroyApplication();
            }
        };
        cDLAppMgr.prototype.destroyAllApps = function () {
            var i, length, app, name;
            var appInfo;
            /// var apps = this.mAppMgr.getChildApplications(this.mAppMain);
            length = this.mAppInfoList.length;
            if (length) {
                for (i = length - 1; i >= 0; i -= 1) {
                    appInfo = this.mAppInfoList[i];
                    name = appInfo ? appInfo.name : null;
                    app = appInfo ? appInfo.app : null;
                    if (app && name !== 'HUB' && name !== 'MAIN' && name !== 'GLOBAL_DLG') {
                        app.destroyApplication();
                        this.mAppInfoList.splice(i, 1);
                    }
                }
            }
        };
        cDLAppMgr.prototype.destroyAllChildApps = function (param) {
            var i, length, child, childApps;
            childApps = this.mAppMgr.getChildApplications(param.app);
            length = childApps.length;
            for (i = 0; i < length; i += 1) {
                child = childApps[i];
                child.destroyApplication();
            }
        };
        cDLAppMgr.prototype.getChildApps = function (param) {
            return this.mAppMgr.getChildApplications(param.app);
        };
        cDLAppMgr.prototype.isAppOpened = function (param) {
            if (param.name === undefined) {
                throw '"name" is required';
            }
            var appInfo = this.fnGetAppInfo(param.name);
            if (appInfo) {
                return true;
            }
            return false;
        };
        cDLAppMgr.prototype.installWidget = function (param) {
            if (param.url === undefined) {
                throw '"url" is required';
            }
            fnLog('fnInstallWidget [' + param.url + ']');
            this.mOnGoingInstallation.push(param);
            this.mAppMgr.installWidget(param.url);
        };
        cDLAppMgr.prototype.uninstallWidget = function (param) {
            if (param.wd === undefined) {
                throw '"wd" is required';
            }
            fnLog('fnUninstallWidget [' + param.wd.name + ']');
            this.mAppMgr.uninstallWidget(param.wd);
        };
        cDLAppMgr.prototype.getWidgetList = function () {
            var widgets = this.mAppMgr.widgets, ret = [], i;
            for (i = 0; i < widgets.length; i += 1) {
                ret.push(widgets.item(i));
            }
            return ret;
        };
        cDLAppMgr.prototype.getWidgetDescriptor = function (param) {
            var widgetId = param.widgetId;
            var widgets = this.mAppMgr.widgets, i, len, wd;
            for (i = 0, len = widgets.length; i < len; i += 1) {
                wd = widgets.item(i);
                if (wd.id === widgetId) {
                    return wd;
                }
            }
            return null;
        };
        cDLAppMgr.prototype.createWidget = function (param) {
            if (param.name === undefined) {
                throw '"name" is required';
            }
            if (param.wd === undefined) {
                throw '"wd" is required';
            }
            fnLog('fnCreateWidget [param:' + param.url + ']');
            this.mAppMgr.addOIPFApplication(param.url);
            var app = this.mAppMain.startWidget(param.wd, true, param.url);
            this.mAppInfoList.push({
                name: param.name,
                param: param,
                app: app,
                wd: param.wd
            });
        };
        cDLAppMgr.prototype.destroyWidget = function (param) {
            if (param.name === undefined) {
                throw '"name" is required';
            }
            var appInfo = this.fnGetAppInfo(param.name);
            if (appInfo) {
                appInfo.app.onKeyDown = null;
                appInfo.app.stopWidget(appInfo.wd);
            }
        };
        cDLAppMgr.prototype.isWidgetInstalled = function (param) {
            if (param.id === undefined) {
                throw '"id" is required';
            }
            this.doNotHandleBuiltInApp = true;
            var widgetList = this.getWidgetList(), i, wd;
            for (i = 0; i < widgetList.length; i += 1) {
                wd = widgetList[i];
                if (wd.id.indexOf(param.id) !== -1) {
                    return true;
                }
            }
            return false;
        };
        cDLAppMgr.prototype.createPortalApp = function () {
            this.mAppMgr.createPortalApplication();
        };
        cDLAppMgr.prototype.destroyPortalApp = function () {
            this.mAppMgr.destroyPortalApplication();
        };
        cDLAppMgr.prototype.createHbbTVApplication = function (bForced) {
            if (hx.svc.AV_CTRL.isLiveOnGoing() || bForced) {
                this.mAppMgr.createHbbTVApplication(this.mAppMgr.HBBTV_APP_TYPE_AUTOSTART);
            }
        };
        cDLAppMgr.prototype.destroyHbbTVApplication = function () {
            this.mAppMgr.destroyHbbTVApplication();
        };
        cDLAppMgr.prototype.teletextToggle = function () {
            this.mAppMgr.doTeletext();
        };
        cDLAppMgr.prototype.getHumaxApps = function () {
            var hmxApps = this.mAppMgr.getHumaxApplications();
            var i, ret = [];
            for (i = 0; i < hmxApps.length; i += 1) {
                ret.push(hmxApps.item(i));
            }
            return ret;
        };
        cDLAppMgr.prototype.createHbbTvDeeplinkApplication = function (param) {
            this.fireEvent('HbbTvAppEnter');
            if (param.name === undefined) {
                throw '"name" is required';
            }
            if (param.url === undefined) {
                throw '"url" is required';
            }
            var app = this.mAppMgr.createHumaxApplication('HbbTV Application', param.url);
            if (!param.name) {
                param.name = this.mPrefixForHbbTV;
            }
            this.mAppInfoList.push({
                name: param.name,
                param: param,
                app: app,
                type: 'HbbTV'
            });
        };
        cDLAppMgr.prototype.destroyHbbTvDeeplinkApplication = function () {
            if (this.mAppMgr) {
                fnLog('[destroyHbbTvDeeplinkApplication]');
                this.mAppMgr.destroyHumaxApplication("HbbTV Application");
            }
        };
        cDLAppMgr.prototype.destroyMhegApplication = function () {
            if (this.mAppMgr) {
                fnLog('[destroyMhegApplication]');
                this.mAppMgr.destroyHumaxApplication("Mheg Application");
            }
        };
        return cDLAppMgr;
    })(__KERNEL__);
    return cDLAppMgr;
});
