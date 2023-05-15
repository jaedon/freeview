/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvSettingUtil.js
/*jslint sloppy:true, nomen:true, vars:true */
//    SETTING_UTIL: ['layer_domain/service/dl_dsrvSettingUtil.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var eNET_TYPE;
    (function (eNET_TYPE) {
        eNET_TYPE[eNET_TYPE["TYPE_LAN"] = 0] = "TYPE_LAN";
        eNET_TYPE[eNET_TYPE["TYPE_WLAN"] = 1] = "TYPE_WLAN";
        eNET_TYPE[eNET_TYPE["TYPE_PPPOE"] = 2] = "TYPE_PPPOE";
        eNET_TYPE[eNET_TYPE["TYPE_NONE"] = 3] = "TYPE_NONE";
    })(eNET_TYPE || (eNET_TYPE = {}));
    var eNET_STATE;
    (function (eNET_STATE) {
        eNET_STATE[eNET_STATE["NET_WIRE"] = 0] = "NET_WIRE";
        eNET_STATE[eNET_STATE["NET_WIRELESS"] = 1] = "NET_WIRELESS";
        eNET_STATE[eNET_STATE["CON_FAIL"] = 0] = "CON_FAIL";
        eNET_STATE[eNET_STATE["CON_SUCCESS"] = 1] = "CON_SUCCESS";
        eNET_STATE[eNET_STATE["CABLE_LINK_DISCON"] = 2] = "CABLE_LINK_DISCON";
        eNET_STATE[eNET_STATE["CABLE_LINK_CON"] = 3] = "CABLE_LINK_CON";
        eNET_STATE[eNET_STATE["WIFI_SCAN_AP"] = 0] = "WIFI_SCAN_AP";
        eNET_STATE[eNET_STATE["WIFI_SCAN_SUCCESS"] = 1] = "WIFI_SCAN_SUCCESS";
        eNET_STATE[eNET_STATE["WIFI_SCAN_FAIL"] = 2] = "WIFI_SCAN_FAIL";
        eNET_STATE[eNET_STATE["WIFI_CON_SUCCESS"] = 3] = "WIFI_CON_SUCCESS";
        eNET_STATE[eNET_STATE["WIFI_CON_FAIL"] = 4] = "WIFI_CON_FAIL";
        eNET_STATE[eNET_STATE["WIFI_KEY_INVALID"] = 5] = "WIFI_KEY_INVALID";
        eNET_STATE[eNET_STATE["WIFI_USB_EXTRACTED"] = 4] = "WIFI_USB_EXTRACTED";
        eNET_STATE[eNET_STATE["WIFI_USB_INSERTED"] = 5] = "WIFI_USB_INSERTED";
        eNET_STATE[eNET_STATE["GATEWAY_ACCESS_FAIL"] = 0] = "GATEWAY_ACCESS_FAIL";
        eNET_STATE[eNET_STATE["GATEWAY_ACCESS_SUCCESS"] = 1] = "GATEWAY_ACCESS_SUCCESS";
        eNET_STATE[eNET_STATE["DNS_ACCESS_FAIL"] = 2] = "DNS_ACCESS_FAIL";
        eNET_STATE[eNET_STATE["DNS_ACCESS_SUCCESS"] = 3] = "DNS_ACCESS_SUCCESS";
        eNET_STATE[eNET_STATE["INTERNET_ACCESS_FAIL"] = 4] = "INTERNET_ACCESS_FAIL";
        eNET_STATE[eNET_STATE["INTERNET_ACCESS_SUCCESS"] = 5] = "INTERNET_ACCESS_SUCCESS";
    })(eNET_STATE || (eNET_STATE = {}));
    var ePOWER_STATE;
    (function (ePOWER_STATE) {
        ePOWER_STATE[ePOWER_STATE["POWER_OFF"] = 0] = "POWER_OFF";
        ePOWER_STATE[ePOWER_STATE["POWER_ON"] = 1] = "POWER_ON";
        ePOWER_STATE[ePOWER_STATE["POWER_PASSIVE_STANDBY"] = 2] = "POWER_PASSIVE_STANDBY";
        ePOWER_STATE[ePOWER_STATE["POWER_ACTIVE_STANDBY"] = 3] = "POWER_ACTIVE_STANDBY";
    })(ePOWER_STATE || (ePOWER_STATE = {}));
    function logNetworkInterface(aDev, aTitle, aLogLevel) {
        if (aTitle === void 0) { aTitle = "unnamed"; }
        if (aLogLevel === void 0) { aLogLevel = "netInfo"; }
        return;
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.networkType : ' + eNET_TYPE[aDev.networkType]);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.ipAddress : ' + aDev.ipAddress);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.netmask : ' + aDev.netmask);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.gateway : ' + aDev.gateway);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.dns1st : ' + aDev.dns1st);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.dns2nd : ' + aDev.dns2nd);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.macAddress : ' + aDev.macAddress);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.enabled : ' + aDev.enabled);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.connected : ' + aDev.connected);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.onLinkStatusChanged : ' + aDev.onLinkStatusChanged);
        hx.log(aLogLevel, '###### NetworkInfo ####### ' + aTitle + '.onAccessStatusUpdated : ' + aDev.onAccessStatusUpdated);
    }
    var CNetworkDeivceModel = (function () {
        function CNetworkDeivceModel() {
            this._listDevWired = [];
            this._listDevWireless = [];
            this._listDevPPPoE = [];
        }
        CNetworkDeivceModel.prototype.addWiredDevice = function (aDev) {
            //hx.log("debug", "[CNetworkDeivceModel][addWiredDevice]");
            CNetworkDeivceModel.__addDevice(aDev, this._listDevWired);
        };
        CNetworkDeivceModel.prototype.addWirelessDevice = function (aDev) {
            //hx.log("debug", "[CNetworkDeivceModel][addWirelessDevice]");
            CNetworkDeivceModel.__addDevice(aDev, this._listDevWireless);
        };
        CNetworkDeivceModel.prototype.addPPPoEDevice = function (aDev) {
            //hx.log("debug", "[CNetworkDeivceModel][addPPPoEDevice]");
            CNetworkDeivceModel.__addDevice(aDev, this._listDevPPPoE);
        };
        CNetworkDeivceModel.prototype.getWiredDevice = function () {
            //hx.log("debug", "[CNetworkDeivceModel][getWiredDevice]: " + this._listDevWired[0] || null);
            return this._listDevWired[0] || null;
        };
        CNetworkDeivceModel.prototype.getWirelessDevice = function () {
            //hx.log("debug", "[CNetworkDeivceModel][getWirelessDevice]: " + this._listDevWireless[0] || null);
            return this._listDevWireless[0] || null;
        };
        CNetworkDeivceModel.prototype.getPPPoEDevice = function () {
            //hx.log("debug", "[CNetworkDeivceModel][getPPPoEDevice]: " + this._listDevPPPoE[0] || null);
            return this._listDevPPPoE[0] || null;
        };
        CNetworkDeivceModel.prototype.truncate = function (aType) {
            //hx.log("debug", "[CNetworkDeivceModel][truncate]: " + eNET_STATE[aType]);
            if (aType) {
                if (aType === 0 /* TYPE_LAN */) {
                    this._listDevWired = [];
                }
                else if (aType === 1 /* TYPE_WLAN */) {
                    this._listDevWireless = [];
                }
                else if (aType === 2 /* TYPE_PPPOE */) {
                    this._listDevPPPoE = [];
                }
            }
            else {
                this._listDevWired = [];
                this._listDevWireless = [];
                this._listDevPPPoE = [];
            }
        };
        CNetworkDeivceModel.__addDevice = function (aDev, aListDev) {
            if (aListDev.push(aDev) !== 1) {
                for (var i = 0; i < aListDev.length; i++) {
                    logNetworkInterface(aListDev[i], undefined, "debug");
                }
                throw "we support only one device, check above log";
                ;
            }
        };
        return CNetworkDeivceModel;
    })();
    var CNetworkDeviceManager = (function () {
        function CNetworkDeviceManager(aOnReqNetworkInterfaces, aOnReqOnAccessStatusUpdated, aOnReqOnLinkStatusChanged) {
            this._modelDevs = new CNetworkDeivceModel();
            this.onReqNetworkInterfaces = function () {
                throw "must override via construcctor";
                return [];
            };
            /* author: bglee@humaxdigital.com
             * date: 2015-02-13 ?�전 12:19
             * comments:
                - dev code, pair with debug
             */
            //(<any>window).d = (<any>window).d || {};
            //(<any>window).d.network = (<any>window).d.network || {
            //    onReqOnAccessStatusUpdated: [],
            //    onReqOnLinkStatusChanged: []
            //};
            this.onReqNetworkInterfaces = aOnReqNetworkInterfaces;
            this.onReqOnAccessStatusUpdated = aOnReqOnAccessStatusUpdated;
            this.onReqOnLinkStatusChanged = aOnReqOnLinkStatusChanged;
            this.refresh();
        }
        CNetworkDeviceManager.prototype.isNetworkAvailable = function () {
            var model = this._modelDevs;
            var ret = false;
            var device = model.getWiredDevice();
            if (device) {
                ret = device.enabled;
            }
            if (!ret) {
                device = model.getWirelessDevice();
                if (device) {
                    ret = device.enabled;
                }
            }
            //hx.log("debug", "[CNetworkDeviceManager][isNetworkAvailable]: " + ret);
            return ret;
        };
        CNetworkDeviceManager.prototype._doGetNetworkInterface = function () {
            //hx.log("debug", "[CNetworkDeviceManager][_doGetNetworkInterface]");
            return this.onReqNetworkInterfaces();
        };
        CNetworkDeviceManager.prototype._doGetEventCBAccessStatusUpdated = function (aNetworkType) {
            //hx.log("debug", "[CNetworkDeviceManager][_doGetEventCBAccessStatusUpdated]");
            return this.onReqOnAccessStatusUpdated(aNetworkType);
        };
        CNetworkDeviceManager.prototype._doGetEventCBLinkStatusChanged = function (aNetworkType) {
            //hx.log("debug", "[CNetworkDeviceManager][_doGetEventCBLinkStatusChanged]");
            return this.onReqOnLinkStatusChanged(aNetworkType);
        };
        CNetworkDeviceManager.prototype._setDevInfo = function (aDev) {
            //hx.log("debug", "[CNetworkDeviceManager][_setDevInfo]");
            var model = this._modelDevs;
            aDev.onAccessStatusUpdated = this._doGetEventCBAccessStatusUpdated(aDev.networkType);
            aDev.onLinkStatusChanged = this._doGetEventCBLinkStatusChanged(aDev.networkType);
            logNetworkInterface(aDev);
            switch (aDev.networkType) {
                case 0 /* TYPE_LAN */:
                    model.addWiredDevice(aDev);
                    break;
                case 1 /* TYPE_WLAN */:
                    model.addWirelessDevice(aDev);
                    break;
                case 2 /* TYPE_PPPOE */:
                    model.addPPPoEDevice(aDev);
                    break;
                default:
                    throw "??";
                    break;
            }
        };
        CNetworkDeviceManager.prototype.refresh = function (aNetworkType) {
            //hx.log("debug", "[CNetworkDeviceManager][refresh]");
            var listDev = this._doGetNetworkInterface();
            var dev;
            var fnCheck = function (aType) {
                if (typeof aType === "undefined") {
                    fnCheck = function () {
                        return true;
                    };
                }
                else {
                    fnCheck = function (aDev) {
                        return aDev.networkType === aType;
                    };
                }
                return fnCheck;
            }(aNetworkType);
            this._modelDevs.truncate(aNetworkType);
            for (var i = 0; i < listDev.length; i++) {
                dev = listDev.item(i);
                if (fnCheck(dev)) {
                    this._setDevInfo(listDev.item(i));
                }
            }
        };
        CNetworkDeviceManager.prototype.getDevWired = function () {
            //hx.log("debug", "[CNetworkDeviceManager][getDevWired]");
            return this._modelDevs.getWiredDevice();
        };
        CNetworkDeviceManager.prototype.getDevWireless = function () {
            //hx.log("debug", "[CNetworkDeviceManager][getDevWireless]");
            return this._modelDevs.getWirelessDevice();
        };
        CNetworkDeviceManager.prototype.getDevPPPoE = function () {
            //hx.log("debug", "[CNetworkDeviceManager][getDevPPPoE]");
            return this._modelDevs.getPPPoEDevice();
        };
        CNetworkDeviceManager.prototype._updateDevice = function (aCB, aDev) {
            if (aDev && aCB(aDev)) {
                aDev.applyConfigure();
            }
        };
        CNetworkDeviceManager.prototype.updateDevWired = function (aCB) {
            //hx.log("debug", "[CNetworkDeviceManager][updateDevWired]");
            this._updateDevice(aCB, this.getDevWired());
        };
        CNetworkDeviceManager.prototype.updateDevWireless = function (aCB) {
            //hx.log("debug", "[CNetworkDeviceManager][updateDevWireless]");
            this._updateDevice(aCB, this.getDevWireless());
        };
        CNetworkDeviceManager.prototype.updateDevPPPoE = function (aCB) {
            //hx.log("debug", "[CNetworkDeviceManager][updateDevPPPoE]");
            this._updateDevice(aCB, this.getDevPPPoE());
        };
        return CNetworkDeviceManager;
    })();
    var cDLSettingUtil = (function (_super) {
        __extends(cDLSettingUtil, _super);
        function cDLSettingUtil(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mLocalSystemOipfObj = this.getPluginInstance('LocalSystem');
            //    private mNetManager: HMX_NetworkManager = this.mLocalSystemOipfObj.networkManager;
            //    private mNetDevList: NetworkInterfaceCollection = this.mLocalSystemOipfObj.networkInterfaces;
            this.mWifiConfig = null;
            this.mPPPoEConfig = null;
            this.mConfigurationOipfObj = this.getPluginInstance('Configuration');
            //private mWoon = this.getPluginInstance('Woon');
            //private mWoonServer = this.mWoon.server;
            //private mWoonClient = this.mWoon.client;
            this.mDial = this.getPluginInstance('Dial');
            this.mDialServer = this.mDial.server;
            this.mDialClient = this.mDial.client;
            //private mSatip = this.getPluginInstance('Satip');
            //private mSatipServer = this.mSatip.server;
            //private mSatipClient = this.mSatip.client;
            this.mBluetoothManager = this.getPluginInstance('BluetoothManager');
            this.mDmsObj = this.getPluginInstance('DLNA.DMS');
            this.mDmrObj = this.getPluginInstance('DLNA.DMR');
            this.mAppMain = this.getPluginInstance('ApplicationManager').getOwnerApplication(window.document);
            this.mSamba = this.getPluginInstance('Samba');
            this.mSambaServer = this.getPluginInstance('SambaServer');
            this.mFtp = this.getPluginInstance('Ftp');
            this.mFtpServer = this.getPluginInstance('FtpServer');
            this.mOperatorFeature = this.getPluginInstance('OperatorFeature');
            this.mConnectionState = 'not';
            this.mIpAddress = "0.0.0.0";
            this.mNetmask = "0.0.0.0";
            this.mGateway = "0.0.0.0";
            this.mDns1st = "0.0.0.0";
            this.mDns2nd = "0.0.0.0";
            this.mOriginApList = [];
            this.mApCacheList = [];
            this.mTimerSignalInform = null;
            this.mNetworkAvailable = false;
            this.mInternetAvailable = false;
            this.mReminderPowerOff = false;
            this.mNetworkManager = new CNetworkDeviceManager(function () {
                return _this.mLocalSystemOipfObj.networkInterfaces;
            }, function (aNetworkType) {
                var ret;
                switch (aNetworkType) {
                    case 0 /* TYPE_LAN */:
                        ret = function (state) {
                            var type = '', result = "";
                            switch (state) {
                                case 0 /* GATEWAY_ACCESS_FAIL */:
                                    type = 'gateway';
                                    result = 'fail';
                                    break;
                                case 1 /* GATEWAY_ACCESS_SUCCESS */:
                                    type = 'gateway';
                                    result = 'success';
                                    break;
                                case 4 /* INTERNET_ACCESS_FAIL */:
                                case 2 /* DNS_ACCESS_FAIL */:
                                    type = 'dns';
                                    result = 'fail';
                                    break;
                                case 5 /* INTERNET_ACCESS_SUCCESS */:
                                case 3 /* DNS_ACCESS_SUCCESS */:
                                    type = 'dns';
                                    result = 'success';
                                    break;
                                default:
                                    result = 'fail';
                                    break;
                            }
                            hx.log("SettingUtil", "[onAccessStatusUpdated()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result + ' ,[type : ' + type);
                            if (state === 3 /* DNS_ACCESS_SUCCESS */ || state == 5 /* INTERNET_ACCESS_SUCCESS */) {
                                _this.mInternetAvailable = true;
                                _this.fireEvent('InternetAvailableChanged', true);
                                _this.fireEvent('networkStatus', type, 'success');
                            }
                            else {
                                _this.mInternetAvailable = false;
                                _this.fireEvent('networkStatus', type, result);
                            }
                            //(function() {
                            //    var debug = (<any>window).d.network;
                            //    if (debug) {
                            //        debug.onReqOnAccessStatusUpdated.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state])
                            //    }
                            //})();
                        };
                        break;
                    case 1 /* TYPE_WLAN */:
                        ret = function (state) {
                            var type = '', result = "";
                            switch (state) {
                                case 0 /* GATEWAY_ACCESS_FAIL */:
                                    type = 'gateway';
                                    result = 'fail';
                                    break;
                                case 1 /* GATEWAY_ACCESS_SUCCESS */:
                                    type = 'gateway';
                                    result = 'success';
                                    break;
                                case 4 /* INTERNET_ACCESS_FAIL */:
                                case 2 /* DNS_ACCESS_FAIL */:
                                    type = 'dns';
                                    result = 'fail';
                                    break;
                                case 5 /* INTERNET_ACCESS_SUCCESS */:
                                case 3 /* DNS_ACCESS_SUCCESS */:
                                    type = 'dns';
                                    result = 'success';
                                    break;
                                default:
                                    result = 'fail';
                                    break;
                            }
                            hx.log("SettingUtil", "[onAccessStatusUpdated()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result);
                            if (state === 3 /* DNS_ACCESS_SUCCESS */ || state == 5 /* INTERNET_ACCESS_SUCCESS */) {
                                _this.mInternetAvailable = true;
                                _this.fireEvent('InternetAvailableChanged', true);
                                _this.fireEvent('networkStatus', type, 'success');
                            }
                            else {
                                _this.mInternetAvailable = false;
                                _this.fireEvent('networkStatus', type, result);
                            }
                            //(function() {
                            //    var debug = (<any>window).d.network;
                            //    if (debug) {
                            //        debug.onReqOnAccessStatusUpdated.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                            //    }
                            //})();
                        };
                        break;
                }
                return ret;
            }, function (aNetworkType) {
                var ret;
                switch (aNetworkType) {
                    case 0 /* TYPE_LAN */:
                        ret = function (state) {
                            var result = "";
                            switch (state) {
                                case 0 /* CON_FAIL */:
                                    result = 'fail';
                                    break;
                                case 1 /* CON_SUCCESS */:
                                    result = 'success';
                                    break;
                                case 2 /* CABLE_LINK_DISCON */:
                                    result = 'discon';
                                    break;
                                case 3 /* CABLE_LINK_CON */:
                                    result = 'con';
                                    break;
                                default:
                                    result = 'fail';
                                    break;
                            }
                            hx.log("SettingUtil", "[onLinkStatusChanged()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result);
                            if (result === 'success') {
                                _this.mConnectionState = 'dhcp';
                                _this.checkInternetConnection();
                            }
                            _this.fireEvent('tryConnection', result);
                            _this.updateNetworkAvailabilty();
                            //(function debug() {
                            //    var debug = (<any>window).d.network;
                            //    if (debug) {
                            //        debug.onReqOnLinkStatusChanged.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                            //    }
                            //})();
                        };
                        break;
                    case 1 /* TYPE_WLAN */:
                        ret = function (state) {
                            var result = "";
                            switch (state) {
                                case 1 /* CON_SUCCESS */:
                                    _this.mNetworkManager.updateDevWireless(function (aDev) {
                                        aDev.dhcpOn = true;
                                        return false;
                                    });
                                    _this.mConnectionState = 'wifi';
                                    result = 'success';
                                    _this.fireEvent('wirelessLinkStatus', result);
                                    _this.updateNetworkAvailabilty();
                                    break;
                                case 0 /* CON_FAIL */:
                                    result = 'fail';
                                    _this.fireEvent('wirelessLinkStatus', result);
                                    _this.updateNetworkAvailabilty();
                                    break;
                                case 4 /* WIFI_CON_FAIL */:
                                    result = 'fail';
                                    _this.updateNetworkAvailabilty();
                                    break;
                                case 5 /* WIFI_USB_INSERTED */:
                                    if (hx.config.useUSBWireless) {
                                        var networkManager = _this.mNetworkManager;
                                        networkManager.refresh(1 /* TYPE_WLAN */);
                                        networkManager.updateDevWireless(function (aDev) {
                                            aDev.enabled = state !== 0;
                                            return false;
                                        });
                                    }
                                    break;
                                case 4 /* WIFI_USB_EXTRACTED */:
                                    if (hx.config.useUSBWireless) {
                                        var networkManager = _this.mNetworkManager;
                                        networkManager.refresh(1 /* TYPE_WLAN */);
                                    }
                                    break;
                                default:
                                    result = 'fail';
                                    break;
                            }
                            hx.log("debug", "[onLinkStatusChanged()][networkType: " + eNET_TYPE[aNetworkType] + "][ - state : " + state + "][ - result : " + result);
                            if (result === 'success') {
                                _this.checkInternetConnection();
                            }
                            //(function debug() {
                            //    var debug = (<any>window).d.network;
                            //    if (debug) {
                            //        debug.onReqOnLinkStatusChanged.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                            //    }
                            //})();
                        };
                        break;
                    case 2 /* TYPE_PPPOE */:
                        ret = function (state) {
                            var result = "";
                            switch (state) {
                                case 0 /* CON_FAIL */:
                                    result = 'fail';
                                    _this.fireEvent('pppoeLinkStatus', result);
                                    break;
                                case 1 /* CON_SUCCESS */:
                                    result = 'success';
                                    _this.fireEvent('pppoeLinkStatus', result);
                                    break;
                                case 2 /* CABLE_LINK_DISCON */:
                                    result = 'discon';
                                    _this.fireEvent('pppoeLinkStatus', result);
                                    break;
                                case 3 /* CABLE_LINK_CON */:
                                    _this.fireEvent('pppoeLinkStatus', result);
                                    result = 'con';
                                    break;
                                default:
                                    result = 'fail';
                                    _this.fireEvent('pppoeLinkStatus', result);
                                    break;
                            }
                            hx.log("SettingUtil", "[onLinkStatusChanged()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result);
                            if (result === 'success') {
                                _this.mConnectionState = 'dhcp';
                                _this.checkInternetConnection();
                            }
                            _this.fireEvent('tryConnection', result);
                            _this.updateNetworkAvailabilty();
                            //(function debug() {
                            //    var debug = (<any>window).d.network;
                            //    if (debug) {
                            //        debug.onReqOnLinkStatusChanged.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                            //    }
                            //})();
                        };
                }
                return ret;
            });
            hx.logadd('netInfo');
            hx.logadd('grab');
            var netManager = this.mLocalSystemOipfObj.networkManager;
            this.mWifiConfig = netManager.getWifiConfig(1);
            this.mPPPoEConfig = netManager.getPPPoEConfig(2);
            hx.logadd("SettingUtil");
            //        hx.logadd('eventWifiScanningResult');
            //        hx.logadd('conNetServer');
            hx.log("SettingUtil", "hx.svc.SETTING_UTILEvent's _init() called");
            /* Cached Buffer Data Area End */
            this.defineEvents(['NetworkAvailableChanged', 'PowerStateChanged', 'autoPowerDownEvent', 'powerOffTimerEvent', 'LanguageChanged', 'privacyPolicyChange', 'PlaybackOptionChange', 'NotifyPinCode', 'checkDetachableStorage', 'ResultEvent', 'StateUpdate', 'tryConnection', 'networkStatus', 'InternetAvailableChanged', 'wirelessLinkStatus', 'eventWifiScanningResult', 'NotiSignalInfo', 'pppoeLinkStatus', 'applyConnectToAP', 'ThermalProtectionOccured', 'wirelessStopConnect']);
            this.mLocalSystemOipfObj.onThermalProtectionOccured = function () {
                hx.log('warning', 'trigger ThermalProtectionOccured event to Page...');
                _this.fireEvent('ThermalProtectionOccured');
            };
            this.mLocalSystemOipfObj.onPowerStateChange = function (powerState) {
                hx.log('warning', 'trigger PowerStateChanged event to Page... powerState : [' + powerState + ']');
                _this.fireEvent('PowerStateChanged', powerState);
            };
            this.mLocalSystemOipfObj.onUserActionIdle = function () {
                hx.log('warning', 'trigger onUserActionIdle event to Page... onUserActionIdle');
                var res = hx.svc.WEB_STORAGE.getAutoPowerDown();
                if (res.isOn === 'true') {
                    _this.fireEvent('autoPowerDownEvent', null);
                }
            };
            this.mBluetoothManager.onStateUpdate = function (evnt, param1, param2, param3) {
                _this.fireEvent('StateUpdate', evnt, param1, param2, param3);
            };
            this.mPPPoEConfig.onPPPoEStatusChanged = function (state) {
                _this.fireEvent('pppoeLinkStatus', state);
            };
            this.mWifiConfig.onAccessPointUpdated = function (state, ap) {
                var waveStrength, secureAttr, isConnected, apName, apInfo = {}, result = '';
                switch (state) {
                    case 0 /* WIFI_SCAN_AP */:
                        _this.mOriginApList.push(ap);
                        waveStrength = ap.waveStrength;
                        if (99 < waveStrength) {
                            waveStrength = 99;
                        }
                        waveStrength = Math.floor(waveStrength / 20);
                        secureAttr = 'false';
                        if (ap.secure || ap.authen || ap.encrypt) {
                            secureAttr = 'true';
                        }
                        isConnected = ap.isConnected;
                        apName = ap.essid;
                        hx.log('eventWifiScanningResult', 'secureAttr : ' + secureAttr);
                        hx.log('eventWifiScanningResult', 'essid : ' + apName);
                        hx.log('eventWifiScanningResult', 'waveStrength : ' + waveStrength);
                        apInfo = {
                            secure: secureAttr,
                            apname: apName,
                            strength: waveStrength,
                            isConnected: isConnected
                        };
                        _this.mApCacheList.push(apInfo);
                        break;
                    case 1 /* WIFI_SCAN_SUCCESS */:
                        _this.fireEvent('eventWifiScanningResult', {
                            apList: _this.mApCacheList
                        });
                        break;
                    case 2 /* WIFI_SCAN_FAIL */:
                        _this.mApCacheList.length = 0;
                        _this.fireEvent('eventWifiScanningResult', {
                            apList: _this.mApCacheList
                        });
                        break;
                    case 3 /* WIFI_CON_SUCCESS */:
                        _this.mNetworkManager.updateDevWireless(function (aDev) {
                            aDev.dhcpOn = true;
                            aDev.dnsAuto = true;
                            return true;
                        });
                        _this.mConnectionState = 'wifi';
                        result = 'success';
                        _this.fireEvent('applyConnectToAP', result);
                        _this.updateNetworkAvailabilty();
                        break;
                    case 4 /* WIFI_CON_FAIL */:
                        result = 'fail';
                        _this.fireEvent('applyConnectToAP', result);
                        _this.updateNetworkAvailabilty();
                        break;
                    case 5 /* WIFI_KEY_INVALID */:
                        result = 'invalid';
                        _this.fireEvent('applyConnectToAP', result);
                        break;
                    default:
                        result = 'fail';
                        _this.fireEvent('applyConnectToAP', result);
                }
            };
            var retServerActivation = hx.svc.WEB_STORAGE.getServerActivationTime();
            if (retServerActivation.isOn === null || retServerActivation.startHour === null || retServerActivation.startMin === null || retServerActivation.endHour === null || retServerActivation.endMin === null || retServerActivation.durationMin === null) {
                retServerActivation.isOn = 'Off';
                retServerActivation.startHour = '12';
                retServerActivation.startMin = '00';
                retServerActivation.endHour = '24';
                retServerActivation.endMin = '00';
                retServerActivation.durationMin = '720';
                hx.svc.WEB_STORAGE.setServerActivationTime({
                    startHour: retServerActivation.startHour,
                    startMin: retServerActivation.startMin,
                    endHour: retServerActivation.endHour,
                    endMin: retServerActivation.endMin,
                    durationMin: retServerActivation.durationMin,
                    isOn: retServerActivation.isOn
                });
                this.mConfigurationOipfObj.setServerActivationTime(retServerActivation.startHour, retServerActivation.startMin, retServerActivation.durationMin, false);
            }
            this.mInternetAvailable = false;
            this.mNetworkAvailable = this.isNetworkConnected();
            if (this.mNetworkAvailable) {
                this.checkInternetConnection();
            }
        }
        cDLSettingUtil.prototype.checkTvPortalServer = function (success, error) {
            $.ajax({
                url: 'http://www.humaxtvportal.com/api/GetCurrentRegionInfo',
                timeout: 3000,
                success: function (data, str, res) {
                    hx.log('error', 'checkTvPortalServer SUCCESS');
                    hx.log('error', 'checkTvPortalServer SUCCESS status(' + res.status + ')');
                    success();
                },
                error: function (data) {
                    hx.log('error', 'checkTvPortalServer ERROR');
                    hx.log('error', 'checkTvPortalServer ERROR status(' + data.status + ')');
                    console.log('checkTvPortalServer_error_res : ' + data);
                    error();
                }
            });
        };
        cDLSettingUtil.prototype.updateNetworkAvailabilty = function () {
            var state = this.isNetworkConnected();
            if (this.mNetworkAvailable !== state) {
                this.mNetworkAvailable = state;
                hx.log("SettingUtil", "[this.updateNetworkAvailabilty()] - trigger (NetworkAvailableChanged) event,  this.mNetworkAvailable : " + this.mNetworkAvailable);
                this.fireEvent('NetworkAvailableChanged', this.mNetworkAvailable);
            }
            if (!state) {
                this.mInternetAvailable = false;
            }
        };
        cDLSettingUtil.prototype.isNetworkConnected = function () {
            var devWired = this.mNetworkManager.getDevWired();
            var devWireless = this.mNetworkManager.getDevWireless();
            var ret = false;
            if (devWired && devWired.enabled) {
                ret = true;
            }
            else if (devWireless && devWireless.enabled) {
                var apInfo = this.mWifiConfig.getConnectedAPInfo();
                if (apInfo.isConnected) {
                    ret = true;
                }
            }
            return ret;
        };
        //    updateUSBWirelessDevice(state) {
        //        var i = 0, netDevListLength = 0;
        //
        //        hx.log("netInfo", "####### NetworkInfo - Wireless ######## updateUSBWirelessDevice");
        //
        //        this.mNetDevList = this.mLocalSystemOipfObj.networkInterfaces;
        //        netDevListLength = this.mNetDevList.length;
        //        hx.log('netInfo', '####### NetworkInfo - Wireless ######## netDevListLength : ' + netDevListLength);
        //        for ( i = 0; i < netDevListLength; i += 1) {
        //            if (this.mNetDevList.item(i).networkType === eNET_TYPE.TYPE_WLAN) {
        //                this._setWirelessDevInfo(this.mNetDevList.item(i), state);
        //            }
        //        }
        //    }
        cDLSettingUtil.prototype.getSystemInfo = function () {
            var mWireDevmacAddress = '00:00:00:00:00:00';
            var mWirelessDevmacAddress = '00:00:00:00:00:00';
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            var devWireless = networkManager.getDevWireless();
            var netflix_ESN = "";
            //        if (this.mWireDev !== null) {
            //            mWireDevmacAddress = this.mWireDev.macAddress;
            //        }
            //        if (this.mWirelessDev !== null) {
            //            mWirelessDevmacAddress = this.mWirelessDev.macAddress;
            //        }
            if (devWired) {
                mWireDevmacAddress = devWired.macAddress;
            }
            if (devWireless) {
                mWirelessDevmacAddress = devWireless.macAddress;
            }
            if (hx.config.netflix === true) {
                netflix_ESN = this.mLocalSystemOipfObj.getField('nflix_esn');
                hx.log("info", "NETFLIX ~~~~~~~~~~~~ esn ::" + netflix_ESN);
            }
            // do not have to convert model name cos it has been done by MW(OBAMA)
            return {
                modelName: this.mLocalSystemOipfObj.modelName,
                softwareVersion: this.mLocalSystemOipfObj.softwareVersion,
                loaderVersion: this.mLocalSystemOipfObj.getField('loader_version'),
                ciplusVersion: this.mLocalSystemOipfObj.getField('ciplus_version'),
                system_id: this.mLocalSystemOipfObj.getField('system_id'),
                update_date: this.mLocalSystemOipfObj.getField('update_date'),
                micom_version: this.mLocalSystemOipfObj.getField('micom_version'),
                serialNumber: this.mLocalSystemOipfObj.serialNumber,
                chipId: this.mLocalSystemOipfObj.chipId,
                wiredMacAddress: mWireDevmacAddress,
                wirelessMacAddress: mWirelessDevmacAddress,
                bluetoothMacAddress: this.mBluetoothManager.getMacAddress(),
                esn: netflix_ESN
            };
        };
        cDLSettingUtil.prototype.getHiddenInfo = function () {
            var i, length, fusingInfo;
            var scs = 0;
            var jtag = 0;
            var otp = 0;
            var cwe = 0;
            var temp;
            var modelName = ENV.model;
            switch (modelName) {
                case ENV.listModel.HD9000i:
                case ENV.listModel.ICORDPRO:
                    temp = $.parseJSON(this.mLocalSystemOipfObj.getField('nagra_fusing_info'));
                    break;
                default:
                    temp = $.parseJSON(this.mLocalSystemOipfObj.getField('fusing_info'));
                    break;
            }
            if (temp) {
                fusingInfo = temp.FUSINGINFO;
            }
            length = fusingInfo.length;
            for (i = 0; i < length; i += 1) {
                if (fusingInfo[i].NAME === 'SCS') {
                    scs = fusingInfo[i].VALUE;
                }
                else if (fusingInfo[i].NAME === 'JTAG') {
                    jtag = fusingInfo[i].VALUE;
                }
                else if (fusingInfo[i].NAME === 'OTP') {
                    otp = fusingInfo[i].VALUE;
                }
                else if (fusingInfo[i].NAME === 'CWE') {
                    cwe = fusingInfo[i].VALUE;
                }
            }
            return {
                serialNumber: this.mLocalSystemOipfObj.serialNumber,
                language: this.mConfigurationOipfObj.preferredMenuLanguage,
                scs: scs,
                jtag: jtag,
                otp: otp,
                cwe: cwe,
                nagraCasn: this.mLocalSystemOipfObj.getField('nagra_casn')
            };
        };
        cDLSettingUtil.prototype.getNandBadBlocks = function () {
            var i, length, badblocks;
            var loader = 0;
            var ubi = 0;
            var ubifs = 0;
            var temp = $.parseJSON(this.mLocalSystemOipfObj.getField('nand_badblocks'));
            if (temp) {
                badblocks = temp.NANDBADBLOCKS;
            }
            length = badblocks.length;
            for (i = 0; i < length; i += 1) {
                if (badblocks[i].NAME === 'LOADER') {
                    loader = badblocks[i].VALUE;
                }
                else if (badblocks[i].NAME === 'UBI') {
                    ubi = badblocks[i].VALUE;
                }
                else if (badblocks[i].NAME === 'UBIFS') {
                    ubifs = badblocks[i].VALUE;
                }
            }
            return {
                'loader': loader,
                'ubi': ubi,
                'ubifs': ubifs
            };
        };
        cDLSettingUtil.prototype.getBoxFirstBootStatus = function () {
            return this.mLocalSystemOipfObj.boxFirstBootStatus;
        };
        cDLSettingUtil.prototype.setBoxFirstBootStatus = function (value) {
            this.mLocalSystemOipfObj.boxFirstBootStatus = value;
        };
        cDLSettingUtil.prototype.setAudioVolume = function (value) {
            value = 5 * value;
            /* (100(max) - 0(min)) / 20(volume step) * value */
            this.mLocalSystemOipfObj.volume = value;
        };
        cDLSettingUtil.prototype.getAudioInfo = function (param) {
            var cb = param.cb;
            var retObj = {};
            var i, len;
            var oipfAudios = this.mLocalSystemOipfObj.hmx_audios;
            var oipfOutput = this.mLocalSystemOipfObj.hmx_outputs[0];
            len = oipfAudios.length;
            for (i = 0; i < len; i += 1) {
                if (oipfAudios[i].name === "audio_hdmi") {
                    retObj.audioModeHdmi = oipfAudios[i].modeDigitalAudioOutput;
                    retObj.audioType = oipfAudios[i].soundMode;
                    retObj.lipSyncDelay = oipfAudios[i].lipSyncDelay;
                }
                else if (oipfAudios[i].name === "audio_spdif") {
                    retObj.audioModePcm = oipfAudios[i].modeDigitalAudioOutput;
                }
                else if (oipfAudios[i].name === "audio_" + i) {
                    retObj.audioModeHdmi = oipfAudios[i].modeDigitalAudioOutput;
                    retObj.audioType = oipfAudios[i].soundMode;
                    retObj.lipSyncDelay = oipfAudios[i].lipSyncDelay;
                }
            }
            retObj.audioDescription = oipfOutput.audioDescriptionEnabled;
            if (cb) {
                setTimeout(function () {
                    cb(retObj);
                }, 1);
            }
            return retObj;
        };
        cDLSettingUtil.prototype.setAudioInfo = function (param) {
            var retObj = {}, i, len, oipfAudios = this.mLocalSystemOipfObj.hmx_audios, oipfOutput = this.mLocalSystemOipfObj.hmx_outputs[0];
            hx.log("SettingUtil", 'setAudioInfo - audioDescriptionEnabled : ' + param.audioDescription);
            oipfOutput.audioDescriptionEnabled = param.audioDescription;
            len = oipfAudios.length;
            for (i = 0; i < len; i += 1) {
                if (oipfAudios[i].name === "audio_hdmi") {
                    if (param.audioModeHdmi) {
                        hx.log("SettingUtil", 'setAudioInfo - HDMI modeDigitalAudioOutput : ' + param.audioModeHdmi);
                        oipfAudios[i].modeDigitalAudioOutput = param.audioModeHdmi;
                    }
                    if (param.audioType) {
                        hx.log("SettingUtil", 'setAudioInfo - soundMode : ' + param.audioType);
                        oipfAudios[i].soundMode = param.audioType;
                    }
                    if (!!param.lipSyncDelay || param.lipSyncDelay === 0) {
                        hx.log("SettingUtil", 'setAudioInfo - lipSyncDelay : ' + param.lipSyncDelay);
                        oipfAudios[i].lipSyncDelay = param.lipSyncDelay;
                    }
                }
                else if (oipfAudios[i].name === "audio_spdif") {
                    if (param.audioModePcm) {
                        hx.log("SettingUtil", 'setAudioInfo - PCM modeDigitalAudioOutput : ' + param.audioModePcm);
                        oipfAudios[i].modeDigitalAudioOutput = param.audioModePcm;
                    }
                }
            }
        };
        cDLSettingUtil.prototype.getPictureInfo = function () {
            var supportedResolutions = [];
            var i, len, curResolution;
            var output = this.mLocalSystemOipfObj.hmx_outputs[0];
            len = output.supportedResolutions.length;
            for (i = 0; i < len; i += 1) {
                supportedResolutions[i] = output.supportedResolutions.item(i);
            }
            curResolution = output.curResolution;
            return {
                tvAspectRatio: output.tvAspectRatio,
                videoDisplayFormat: output.videoDisplayFormat,
                curResolution: curResolution,
                supportedResolutions: supportedResolutions
            };
        };
        cDLSettingUtil.prototype.setPictureInfo = function (data) {
            var output = this.mLocalSystemOipfObj.hmx_outputs[0];
            hx.log("SettingUtil", 'setPictureInfo - data : ' + data);
            for (var prop in data) {
                hx.log("SettingUtil", 'setPictureInfo - [' + prop + ' : ' + data[prop] + ']');
                if (data[prop]) {
                    output[prop] = data[prop];
                }
            }
        };
        cDLSettingUtil.prototype.getOutputOption = function () {
            var priorityOutputs = this.mLocalSystemOipfObj.priorityOutputs;
            hx.log("SettingUtil", 'getOutputOption - priorityOutputs : ' + priorityOutputs);
            return priorityOutputs;
        };
        cDLSettingUtil.prototype.setOutputOption = function (data) {
            hx.log("SettingUtil", 'setOutputOption - priorityOutputs : ' + data);
            this.mLocalSystemOipfObj.priorityOutputs = data;
        };
        cDLSettingUtil.prototype.getLanguage = function () {
            var retObj = {};
            if (undefined === this.cache_langMenu) {
                this.cache_langMenu = this.mConfigurationOipfObj.preferredMenuLanguage;
                retObj.LangMenu = this.cache_langMenu;
            }
            else {
                retObj.LangMenu = this.cache_langMenu;
            }
            if (undefined === this.cache_langAudio) {
                this.cache_langAudio = this.mConfigurationOipfObj.preferredAudioLanguage;
                retObj.LangAudio = this.cache_langAudio;
            }
            else {
                retObj.LangAudio = this.cache_langAudio;
            }
            if (undefined === this.cache_langSubTtl) {
                this.cache_langSubTtl = this.mConfigurationOipfObj.preferredSubtitleLanguage;
                retObj.LangSubTtl = this.cache_langSubTtl;
            }
            else {
                retObj.LangSubTtl = this.cache_langSubTtl;
            }
            //hx.log("SettingUtil", 'getLanguage - [MenuLang : ' + retObj.LangMenu + '] [SubTtlLang : ' + retObj.LangSubTtl + '] [AudioLang : ' + retObj.LangAudio + ']');
            return retObj;
        };
        cDLSettingUtil.prototype.setLanguage = function (values) {
            hx.log("SettingUtil", 'setLanguage - [LangMenu : ' + values.LangMenu + '] [LangSubTtl : ' + values.LangSubTtl + '] [LangAudio : ' + values.LangAudio + ']');
            if (values.LangMenu !== undefined) {
                this.cache_langMenu = values.LangMenu;
                this.mConfigurationOipfObj.preferredMenuLanguage = this.cache_langMenu;
            }
            if (values.LangAudio !== undefined) {
                this.cache_langAudio = values.LangAudio;
                this.mConfigurationOipfObj.preferredAudioLanguage = this.cache_langAudio;
            }
            if (values.LangSubTtl !== undefined) {
                this.cache_langSubTtl = values.LangSubTtl;
                this.mConfigurationOipfObj.preferredSubtitleLanguage = this.cache_langSubTtl;
            }
            this.fireEvent('LanguageChanged', this.getLanguage());
        };
        cDLSettingUtil.prototype.controlNetServer = function (bOnOff) {
            var dsrvDmr = hx.svc.DMR;
            var dsrvPlayback = hx.svc.PLAYBACK;
            var dmrIsStarted = dsrvDmr.isStarted();
            if (bOnOff) {
                if (!this.mDmsObj.started) {
                    hx.log('conNetServer', 'DMS start');
                    this.mDmsObj.start();
                }
                //if (!this.mDialServer.started) {
                //    hx.log('conNetServer', 'Dial start');
                //    this.mDialServer.start();
                //}
                if (!dmrIsStarted) {
                    hx.log('conNetServer', 'DMR start');
                    dsrvDmr.start();
                }
            }
            else {
                if (this.mDmsObj.started) {
                    hx.log('conNetServer', 'DMS stop');
                    this.mDmsObj.stop();
                }
                if (dmrIsStarted) {
                    hx.log('conNetServer', 'DMR stop');
                    dsrvDmr.stop();
                }
            }
        };
        cDLSettingUtil.prototype.IsNetServerWork = function () {
            return this.mDmsObj.started;
        };
        cDLSettingUtil.prototype.getPvrEndPadding = function () {
            var pvrEndPadding;
            if (undefined === this.cache_endPadding) {
                pvrEndPadding = this.mConfigurationOipfObj.pvrEndPadding;
                this.cache_endPadding = pvrEndPadding;
            }
            else {
                pvrEndPadding = this.cache_endPadding;
            }
            hx.log("SettingUtil", 'fnGetPvrEndPadding() - pvrEndPadding : ' + pvrEndPadding);
            return pvrEndPadding;
        };
        cDLSettingUtil.prototype.setPvrEndPadding = function (value) {
            this.cache_endPadding = value;
            this.mConfigurationOipfObj.pvrEndPadding = this.cache_endPadding;
            hx.log("SettingUtil", '() - this.cache_endPadding : ' + this.cache_endPadding);
        };
        cDLSettingUtil.prototype.getPvrStartPadding = function () {
            var pvrStartPadding = this.mConfigurationOipfObj.pvrStartPadding;
            if (undefined === this.cache_startPadding) {
                pvrStartPadding = this.mConfigurationOipfObj.pvrStartPadding;
                this.cache_startPadding = pvrStartPadding;
            }
            else {
                pvrStartPadding = this.cache_startPadding;
            }
            hx.log("SettingUtil", 'fnGetPvrStartPadding() - pvrStartPadding : ' + pvrStartPadding);
            return pvrStartPadding;
        };
        cDLSettingUtil.prototype.setPvrStartPadding = function (value) {
            this.cache_startPadding = value;
            this.mConfigurationOipfObj.pvrStartPadding = this.cache_startPadding;
            hx.log("SettingUtil", 'fnSetPvrStartPadding() - this.cache_startPadding : ' + this.cache_startPadding);
        };
        cDLSettingUtil.prototype.getInstantReplayTime = function () {
            var instantReplayTime;
            if (undefined === this.cache_instantReplayTime) {
                instantReplayTime = this.mConfigurationOipfObj.instantReplayTime || 15;
                this.cache_instantReplayTime = instantReplayTime;
            }
            else {
                instantReplayTime = this.cache_instantReplayTime;
            }
            hx.log("SettingUtil", 'fnGetInstantReplayTime() - instantReplayTime : ' + instantReplayTime);
            return instantReplayTime;
        };
        cDLSettingUtil.prototype.setInstantReplayTime = function (value) {
            this.cache_instantReplayTime = value;
            this.mConfigurationOipfObj.instantReplayTime = this.cache_instantReplayTime;
            hx.log("SettingUtil", 'fnSetInstantReplayTime() - this.cache_instantReplayTime : ' + this.cache_instantReplayTime);
            var instantReplayTime = this.getInstantReplayTime();
            this.fireEvent('PlaybackOptionChange', {
                state: 'ReplayTime',
                time: instantReplayTime
            });
        };
        cDLSettingUtil.prototype.getSkipForwardTime = function () {
            var skipForwardTime;
            if (undefined === this.cache_instantSkipTime) {
                skipForwardTime = this.mConfigurationOipfObj.skipForwardTime || 120;
                this.cache_instantSkipTime = skipForwardTime;
            }
            else {
                skipForwardTime = this.cache_instantSkipTime;
            }
            hx.log("SettingUtil", 'fnGetSkipForwardTime() - value : ' + skipForwardTime);
            return skipForwardTime;
        };
        cDLSettingUtil.prototype.setSkipForwardTime = function (value) {
            this.cache_instantSkipTime = value;
            this.mConfigurationOipfObj.skipForwardTime = this.cache_instantSkipTime;
            hx.log("SettingUtil", 'fnSetSkipForwardTime() - this.cache_instantSkipTime : ' + this.cache_instantSkipTime);
        };
        cDLSettingUtil.prototype.InitializeLanguage = function (cb) {
            var LangObj = this.getLanguage();
            hx.l.load(LangObj.LangMenu, function () {
                dateFormat.set(undefined, [hx.l('LOC_SUN_ID'), hx.l('LOC_MON_ID'), hx.l('LOC_TUE_ID'), hx.l('LOC_WED_ID'), hx.l('LOC_THU_ID'), hx.l('LOC_FRI_ID'), hx.l('LOC_SAT_ID'), hx.l('LOC_SUNDAY_ID'), hx.l('LOC_MONDAY_ID'), hx.l('LOC_TUESDAY_ID'), hx.l('LOC_WEDNESDAY_ID'), hx.l('LOC_THURSDAY_ID'), hx.l('LOC_FRIDAY_ID'), hx.l('LOC_SATURDAY_ID')], [hx.l('LOC_JAN_ID'), hx.l('LOC_FEB_ID'), hx.l('LOC_MAR_ID'), hx.l('LOC_APR_ID'), hx.l('LOC_MAY_01_ID'), hx.l('LOC_JUN_ID'), hx.l('LOC_JUL_ID'), hx.l('LOC_AUG_ID'), hx.l('LOC_SEP_ID'), hx.l('LOC_OCT_ID'), hx.l('LOC_NOV_ID'), hx.l('LOC_DEC_ID'), hx.l('LOC_JANUARY_ID'), hx.l('LOC_FEBRUARY_ID'), hx.l('LOC_MARCH_ID'), hx.l('LOC_APRIL_ID'), hx.l('LOC_MAY_ID'), hx.l('LOC_JUNE_ID'), hx.l('LOC_JULY_ID'), hx.l('LOC_AUGUST_ID'), hx.l('LOC_SEPTEMBER_ID'), hx.l('LOC_OCTOBER_ID'), hx.l('LOC_NOVEMBER_ID'), hx.l('LOC_DECEMBER_ID')]);
                cb();
            });
        };
        cDLSettingUtil.prototype.getEnableSeriesPlayback = function () {
            var retObj;
            switch (this.mConfigurationOipfObj.getField('enableSeriesPlay')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
            }
            hx.log("SettingUtil", 'getEnableSeriesPlayback - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableSeriesPlayback = function (param) {
            hx.log("SettingUtil", 'setEnableSeriesPlayback - param : ' + param);
            return this.mConfigurationOipfObj.setField('enableSeriesPlay', param ? "true" : "false");
        };
        cDLSettingUtil.prototype.getEnableIPEPG = function () {
            var retObj;
            switch (this.mConfigurationOipfObj.getField('enableIPEPG')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
            }
            hx.log("SettingUtil", 'getEnableIPEPG - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.getIpepgLastUpdateTime = function () {
            var retObj, value;
            value = this.mConfigurationOipfObj.getField('ipepgLastUpdateTime');
            retObj = Number(value);
            hx.log("SettingUtil", 'getIpepgLastUpdateTime - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.getIpepgDownloadProgress = function () {
            var retObj, value;
            value = this.mConfigurationOipfObj.getField('ipepgDownloadProgress');
            retObj = Number(value);
            hx.log("SettingUtil", 'fnGetIpepgDownloadProgress - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.getTvtvEpgLastUpdateTime = function () {
            var retObj, value;
            value = this.mConfigurationOipfObj.getField('tvtvEpgLastUpdateTime');
            retObj = Number(value);
            hx.log("SettingUtil", 'getTvtvEpgLastUpdateTime - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableIPEPG = function (param) {
            hx.log("SettingUtil", 'setEnableIPEPG - param : ' + param);
            return this.mConfigurationOipfObj.setField('enableIPEPG', param ? "true" : "false");
        };
        cDLSettingUtil.prototype.getTvtvUpdateTime = function () {
            var privateData = this.mAppMain.privateData, tvtvWakeupTime = null;
            if (privateData) {
                tvtvWakeupTime = privateData.tvtvWakeupTime;
            }
            return tvtvWakeupTime;
        };
        cDLSettingUtil.prototype.getIsTvtvStandbyEnable = function () {
            var privateData = this.mAppMain.privateData, isTvtvStandbyEnable = false;
            if (privateData) {
                isTvtvStandbyEnable = privateData.isTvtvStandbyEnable;
            }
            return isTvtvStandbyEnable;
        };
        cDLSettingUtil.prototype.setTvtvUpdateTime = function (param) {
            this.mAppMain.privateData.prepareWakeupTvtvUpdate(param.timeObj, param.isTvtvStandbyEnable);
        };
        cDLSettingUtil.prototype.getEnableRemoteRec = function () {
            var retObj;
            switch (this.mConfigurationOipfObj.getField('enableRemoteRecord')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
            }
            hx.log("SettingUtil", 'getEnableRemoteRec - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableRemoteRec = function (param) {
            var retObj;
            hx.log("SettingUtil", 'setEnableRemoteRec - param : ' + param);
            return (retObj = this.mConfigurationOipfObj.setField('enableRemoteRecord', param ? "true" : "false"));
        };
        cDLSettingUtil.prototype.getEnableSatIpNotification = function () {
            var retObj;
            switch (this.mConfigurationOipfObj.getField('enableSatIpNotification')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
            }
            hx.log("SettingUtil", 'GetEnableSatIpNotification - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableSatIpNotification = function (param) {
            var retObj;
            hx.log("SettingUtil", 'SetEnableSatIpNotification - param : ' + param);
            return (retObj = this.mConfigurationOipfObj.setField('enableSatIpNotification', param ? "true" : "false"));
        };
        cDLSettingUtil.prototype.getSatRecordUpdateInterval = function () {
            var value, retObj;
            retObj = (Number(this.mConfigurationOipfObj.getField('satRecordUpdateInterval')));
            hx.log("SettingUtil", 'getSatRecordUpdateInterval - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setSatRecordUpdateInterval = function (param) {
            var retObj;
            hx.log("SettingUtil", 'setSatRecordUpdateInterval - param : ' + param);
            return (retObj = this.mConfigurationOipfObj.setField('satRecordUpdateInterval', param));
        };
        cDLSettingUtil.prototype.getSatRecordConfirmCode = function (param) {
            var retObj;
            retObj = this.mConfigurationOipfObj.getSatRecordConfirmCode(param.registrationCode);
            hx.log("SettingUtil", 'getSatRecordConfirmCode - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.getSatRecordPriority = function () {
            var retObj;
            retObj = this.mConfigurationOipfObj.getField('satRecordPriority');
            hx.log("SettingUtil", 'getSatRecordPriority - retObj : ' + retObj);
            return (retObj === "true") ? 1 : 0;
        };
        cDLSettingUtil.prototype.setSatRecordPriority = function (param) {
            var retObj;
            hx.log("SettingUtil", 'setSatRecordPriority - param : ' + param);
            return this.mConfigurationOipfObj.setField('satRecordPriority', param);
        };
        cDLSettingUtil.prototype.getUpdatePeriodRemoteRec = function () {
            var values, retObj = {};
            values = this.mConfigurationOipfObj.getField('updatePeriodRemoteRecord').split(':');
            retObj.start_time = (Number(values[0]));
            retObj.end_time = (Number(values[1]));
            hx.log("SettingUtil", 'getUpdatePeriodRemoteRec - ' + retObj.start_time + ':' + retObj.end_time);
            return retObj;
        };
        cDLSettingUtil.prototype.setUpdatePeriodRemoteRec = function (param) {
            var retObj;
            hx.log("SettingUtil", 'setUpdatePeriodRemoteRec - param : ' + param);
            return (retObj = this.mConfigurationOipfObj.setField('updatePeriodRemoteRecord', ((param.start_time) + ':' + (param.end_time))));
        };
        cDLSettingUtil.prototype.getUpdateIntervalRemoteRec = function () {
            var retObj;
            retObj = (Number(this.mConfigurationOipfObj.getField('updateIntervalRemoteRecord')) / 60);
            hx.log("SettingUtil", 'getUpdateIntervalRemoteRec - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setUpdateIntervalRemoteRec = function (param) {
            var retObj;
            hx.log("SettingUtil", 'setUpdateIntervalRemoteRec - param : ' + param);
            return this.mConfigurationOipfObj.setField('updateIntervalRemoteRecord', (param * 60).toString());
        };
        cDLSettingUtil.prototype.doFactoryDefault = function () {
            hx.log("SettingUtil", 'doFactoryDefault');
            hx.svc.WEB_STORAGE.deleteWholeItems();
            hx.svc.AV_CTRL.stopCurChannel({
                stopAV: 1,
                stopTsr: 1
            });
            this.mConfigurationOipfObj.doFactoryDefault('all');
        };
        cDLSettingUtil.prototype.getFirstTimeBoot = function () {
            var firstTimeBoot = this.mConfigurationOipfObj.firstTimeBoot;
            hx.log("SettingUtil", 'getFirstTimeBoot : ' + firstTimeBoot);
            return firstTimeBoot;
        };
        cDLSettingUtil.prototype.offFirstTimeBoot = function () {
            hx.log("SettingUtil", 'offFirstTimeBoot ');
            this.mConfigurationOipfObj.firstTimeBoot = false;
        };
        cDLSettingUtil.prototype.getHdmiCecMode = function () {
            var hdmiCecMode = this.mConfigurationOipfObj.hdmiCecMode;
            hx.log("SettingUtil", 'fnGetHdmiCecModes : ' + hdmiCecMode);
            return hdmiCecMode;
        };
        cDLSettingUtil.prototype.setHdmiCecMode = function (value) {
            hx.log("SettingUtil", 'fnSetHdmiCecMode : ' + value);
            this.mConfigurationOipfObj.hdmiCecMode = value;
        };
        cDLSettingUtil.prototype.getHdmiCecVolumeCtrlMode = function () {
            var hdmiCecVolumeCtrlMode = this.mConfigurationOipfObj.hdmiCecVolumeCtrlMode;
            hx.log("SettingUtil", 'fnGetHdmiCecVolumeCtrlMode : ' + hdmiCecVolumeCtrlMode);
            return hdmiCecVolumeCtrlMode;
        };
        cDLSettingUtil.prototype.setHdmiCecVolumeCtrlMode = function (value) {
            hx.log("SettingUtil", 'fnSetHdmiCecVolumeCtrlMode : ' + value);
            this.mConfigurationOipfObj.hdmiCecVolumeCtrlMode = value;
        };
        cDLSettingUtil.prototype.setPowerOff = function () {
            hx.log("SettingUtil", 'setPowerOff() from web application');
            this.mLocalSystemOipfObj.setPowerState(2 /* POWER_PASSIVE_STANDBY */);
        };
        cDLSettingUtil.prototype.setPowerOn = function () {
            hx.log("SettingUtil", 'SetPowerOn() from web application');
            this.mLocalSystemOipfObj.setPowerState(1 /* POWER_ON */);
        };
        cDLSettingUtil.prototype.standbyState = function (state) {
            hx.log("SettingUtil", 'standbyState. going to state mode..');
            this.mLocalSystemOipfObj.standbyState = state;
        };
        cDLSettingUtil.prototype.setStandbyPowerMode = function (mode) {
            hx.log("SettingUtil", 'setStandbyPowerMode() from web application');
            this.mConfigurationOipfObj.standbyPowerMode = mode;
            //['Normal', 'Power save']
        };
        cDLSettingUtil.prototype.getStandbyPowerMode = function () {
            hx.log("SettingUtil", 'getStandbyPowerMode() from web application');
            return this.mConfigurationOipfObj.standbyPowerMode;
        };
        cDLSettingUtil.prototype.getPowerOnInfo = function () {
            var privateData = this.mAppMain.privateData, retObj = false;
            if (privateData) {
                retObj = {
                    channel: privateData.wakeupChannel,
                    time: privateData.wakeupTime,
                    volume: privateData.wakeupVolume,
                    repeatDays: privateData.wakeupRepeatDays
                };
            }
            if (!retObj.channel) {
                retObj = false;
            }
            return retObj;
        };
        cDLSettingUtil.prototype.setPowerOnInfo = function (param) {
            hx.log("SettingUtil", '--------------------------------------------------------------');
            hx.log("SettingUtil", hx.util.format('time: {0}, repeatDays: {1} - {2}, channel: {3}, volume: {4}', param.timeObj.format('yyyy-mm-dd HH:MM:ss.L'), param.timeObj.getTime(), param.repeatDays, param.channel.ccid, param.volume));
            hx.log("SettingUtil", '--------------------------------------------------------------');
            this.mAppMain.privateData.prepareWakeupOITF(param.timeObj, param.repeatDays, param.channel, param.volume);
        };
        cDLSettingUtil.prototype.clearPowerOnInfo = function () {
            this.mAppMain.privateData.prepareWakeupOITF(null);
        };
        cDLSettingUtil.prototype.getNetConnectedInfo = function () {
            var retObj = {};
            var wireDevState = "Disconnected";
            var wirelessDevState = "Disconnected";
            var pppoeDevState = "Disconnected";
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            var devWireless = networkManager.getDevWireless();
            var devPPPoE = networkManager.getDevPPPoE();
            var waveStrength = "-";
            if (devWired && devWired.enabled) {
                wireDevState = 'Connected';
            }
            else {
                wireDevState = 'Disconnected';
            }
            if (devWireless && devWireless.enabled) {
                var apInfo = this.mWifiConfig.getConnectedAPInfo();
                if (apInfo !== null) {
                    wirelessDevState = 'Connected';
                    var strength = apInfo.waveStrength;
                    if (99 < strength) {
                        strength = 99;
                    }
                    else if (strength == 0 && apInfo.essid == '') {
                        wirelessDevState = 'Disconnected';
                    }
                    waveStrength = Math.floor(strength / 20).toString();
                    var secureAttr = 'false';
                    if (apInfo.secure || apInfo.authen || apInfo.encrypt) {
                        secureAttr = 'true';
                    }
                    retObj.wirelessDevInfo = {
                        secure: secureAttr,
                        apname: apInfo.essid,
                        strength: waveStrength,
                        isConnected: 1
                    };
                }
                else {
                    wirelessDevState = 'Disconnected';
                }
            }
            else {
                wirelessDevState = 'Disconnected';
            }
            if (devPPPoE && devPPPoE.enabled) {
                pppoeDevState = 'Connected';
            }
            else {
                pppoeDevState = 'Disconnected';
            }
            retObj.pppoeDevState = pppoeDevState;
            retObj.wireDevState = wireDevState;
            retObj.wirelessDevState = wirelessDevState;
            retObj.waveStrength = waveStrength;
            hx.log("SettingUtil", "[fnGetNetConnectedInfo()] - wireDevState : [" + wireDevState + "] waveStrength : [" + waveStrength + "]");
            return retObj;
        };
        cDLSettingUtil.prototype.stopGetSignalInfo = function () {
            if (this.mTimerSignalInform) {
                clearInterval(this.mTimerSignalInform);
                this.mTimerSignalInform = null;
                return true;
            }
            return false;
        };
        cDLSettingUtil.prototype._fnGetSignalInfo = function () {
            var signalInfo, tunerList = this.mLocalSystemOipfObj.tuners, i, len = tunerList.length, signal = [{
                strength: 0,
                quality: 0,
                tuningParams: '',
                lock: false
            }, {
                strength: 0,
                quality: 0,
                tuningParams: '',
                lock: false
            }, {
                strength: 0,
                quality: 0,
                tuningParams: '',
                lock: false
            }, {
                strength: 0,
                quality: 0,
                tuningParams: '',
                lock: false
            }];
            if (tunerList) {
                for (i = 0; i < len; i += 1) {
                    signalInfo = tunerList.item(i).signalInfo;
                    if (signalInfo.lock) {
                        signal[i].strength = signalInfo.strength;
                        signal[i].quality = signalInfo.quality;
                        signal[i].tuningParams = signalInfo.tuningParams;
                        signal[i].lock = true;
                    }
                    else {
                        signal[i].strength = 0;
                        signal[i].quality = 0;
                        signal[i].tuningParams = signalInfo.tuningParams;
                        signal[i].lock = false;
                    }
                }
            }
            return signal;
        };
        cDLSettingUtil.prototype.getSignalInfo = function () {
            var _this = this;
            var CheckSignalInfoTimer = function () {
                var retObj;
                retObj = _this._fnGetSignalInfo();
                _this.fireEvent('NotiSignalInfo', {
                    signal: retObj
                });
            };
            setTimeout(CheckSignalInfoTimer, 0);
            this.mTimerSignalInform = setInterval(CheckSignalInfoTimer, 2000);
        };
        cDLSettingUtil.prototype.getSignalConnectionType = function () {
            var antInfoList = this.mLocalSystemOipfObj.antennaInfoLists, connectionType;
            hx.log("SettingUtil", 'getSignalConnectionType');
            connectionType = antInfoList.getConnectionType();
            return connectionType;
        };
        cDLSettingUtil.prototype.setInfoDisplayTimeout = function (param) {
            this.cache_infoDisplayTimeout = param.timeout;
            this.mConfigurationOipfObj.infoDisplayTimeout = this.cache_infoDisplayTimeout;
            hx.log("SettingUtil", 'fnSetInfoDisplayTimeout() this.cache_infoDisplayTimeout : ' + this.cache_infoDisplayTimeout);
        };
        cDLSettingUtil.prototype.getInfoDisplayTimeout = function () {
            var UiDisplayTimeOut;
            if (undefined === this.cache_infoDisplayTimeout) {
                UiDisplayTimeOut = this.mConfigurationOipfObj.infoDisplayTimeout;
                this.cache_infoDisplayTimeout = UiDisplayTimeOut;
            }
            else {
                UiDisplayTimeOut = this.cache_infoDisplayTimeout;
            }
            hx.log("SettingUtil", 'getInfoDisplayTimeout() UiDisplayTimeOut : ' + UiDisplayTimeOut);
            return UiDisplayTimeOut;
        };
        cDLSettingUtil.prototype.setSubtitleFont = function (aValue) {
            hx.log("SettingUtil", 'setSubtitleFont() from web application');
            //        if (param.subtitleFont === 'variable') {
            //            this.mConfigurationOipfObj.subtitleFontType = this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_VARIABLE;
            //        } else if (param.subtitleFont === 'fixed') {
            //            this.mConfigurationOipfObj.subtitleFontType = this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_FIXED;
            //        }
            if (!hx.emulMode && !(aValue in [this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_VARIABLE, this.mConfigurationOipfObj.subtitleFontType = this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_FIXED])) {
                throw 'undefined value input';
            }
            this.mConfigurationOipfObj.subtitleFontType = aValue;
        };
        cDLSettingUtil.prototype.getSubtitleFont = function () {
            hx.log("SettingUtil", 'getSubtitleFont() from web application');
            return this.mConfigurationOipfObj.subtitleFontType;
        };
        cDLSettingUtil.prototype.setSubtitleEnabled = function (aParam) {
            var outputs = this.mLocalSystemOipfObj.hmx_outputs;
            for (var i = 0; i < outputs.length; i++) {
                outputs[i].subtitleEnabled = aParam;
            }
        };
        cDLSettingUtil.prototype.getSubtitleEnabled = function () {
            return this.mLocalSystemOipfObj.hmx_outputs[0].subtitleEnabled;
        };
        cDLSettingUtil.prototype.setHardofHearing = function (aParam) {
            var outputs = this.mLocalSystemOipfObj.hmx_outputs;
            for (var i = 0; i < outputs.length; i++) {
                outputs[i].hardOfHearingEnabled = aParam;
            }
        };
        cDLSettingUtil.prototype.getHardofHearing = function () {
            return this.mLocalSystemOipfObj.hmx_outputs[0].hardOfHearingEnabled;
        };
        cDLSettingUtil.prototype.renameServer = function (param) {
            this.mDmsObj.friendlyName = param.serverName;
            this.mDmrObj.friendlyName = param.serverName;
            this.mDialServer.friendlyName = param.serverName;
        };
        cDLSettingUtil.prototype.getServerName = function () {
            var serverName = this.mDmsObj.friendlyName;
            if (!serverName) {
                // handled an exceptional case
                // set default friendlyName by using modelName
                hx.log("debug", '[getServerName] set default friendlyName by using modelName');
                serverName = this.getSystemInfo().modelName;
                this.mDmsObj.friendlyName = serverName;
                this.mDmrObj.friendlyName = serverName;
                this.mDialServer.friendlyName = serverName;
            }
            return serverName;
        };
        cDLSettingUtil.prototype.testConnection = function () {
            //this.mWoonServer.testConnection();
        };
        cDLSettingUtil.prototype.setFrontPanelString = function (param) {
            var text = param.text;
            var bStr = (text === '') ? true : false;
            var i = 0, chgText, bfind = false, subValue, iValue;
            hx.log("SettingUtil", 'fnSetFrontPanelString : ' + param.text);
            if (hx.config.frontPanel) {
                if (bStr === false) {
                    for (i = 0; i < hx.config.frontPanel.lcdVfd16.length; i += 1) {
                        if (text === hx.config.frontPanel.lcdVfd16[i]) {
                            if (hx.config.usefrontPanel === 'segment7') {
                                chgText = hx.config.frontPanel.segment7[i];
                                text = chgText;
                            }
                            else if (hx.config.usefrontPanel === 'lcdVfd10') {
                                chgText = hx.config.frontPanel.lcdVfd10[i];
                                text = chgText;
                            }
                            bfind = true;
                            hx.log("SettingUtil", 'fnSetFrontPanelString chgText: ' + text);
                            break;
                        }
                    }
                    if ((bfind === false) && (hx.config.usefrontPanel === 'segment7')) {
                        subValue = 'FRX';
                        iValue = text.indexOf(subValue);
                        if (iValue !== -1) {
                            text = 'FRXX';
                            hx.log("SettingUtil", 'fnSetFrontPanelString chgText: ' + text);
                        }
                        subValue = 'FFX';
                        iValue = text.indexOf(subValue);
                        if (iValue !== -1) {
                            text = 'FFXX';
                            hx.log("SettingUtil", 'fnSetFrontPanelString chgText: ' + text);
                        }
                    }
                }
                this.mLocalSystemOipfObj.frontPanel.setString(text, 0, 100, bStr);
            }
            //panelString, align, dimlevel, bAuto
        };
        cDLSettingUtil.prototype.setFrontPanelLed = function (param) {
            var ledId = param.ledId, bEnable = param.bEnable, bClear = (param.bClear === undefined) ? false : param.bClear;
            this.mLocalSystemOipfObj.frontPanel.setLed(ledId, bEnable, false, bClear);
            //ledId, bOnOff, bAuto, bAllClear
        };
        cDLSettingUtil.prototype.getHbbTvUrl = function () {
            return this.mLocalSystemOipfObj.getField('url_hbbtv');
        };
        cDLSettingUtil.prototype.getRpUrl = function () {
            return this.mLocalSystemOipfObj.getField('url_rp');
        };
        cDLSettingUtil.prototype.getIpOtaUrl = function () {
            return this.mLocalSystemOipfObj.getField('url_ipota');
        };
        cDLSettingUtil.prototype.getBluetoothStatus = function () {
            return this.mBluetoothManager.enabled;
        };
        cDLSettingUtil.prototype.setBluetoothStatus = function (param) {
            this.mBluetoothManager.enabled = param.turnOnOff;
        };
        cDLSettingUtil.prototype.bluetoothStartScan = function () {
            this.mBluetoothManager.startScan();
        };
        cDLSettingUtil.prototype.getBluetoothDevList = function () {
            return this.mBluetoothManager.getDeviceList();
        };
        cDLSettingUtil.prototype.bluetoothConnect = function (param) {
            this.mBluetoothManager.connect(param.addr);
        };
        cDLSettingUtil.prototype.bluetoothDisconnect = function (param) {
            this.mBluetoothManager.disconnect(param.addr);
        };
        cDLSettingUtil.prototype.bluetoothSendPincode = function (param) {
            this.mBluetoothManager.sendPincode(param.addr, param.pincode);
        };
        cDLSettingUtil.prototype.bluetoothRemove = function (param) {
            this.mBluetoothManager.remove(param.addr);
        };
        cDLSettingUtil.prototype.getNetworkAvailable = function () {
            this.updateNetworkAvailabilty();
            hx.log("SettingUtil", 'getNetworkAvailable : ' + this.mNetworkAvailable);
            return this.mNetworkAvailable;
        };
        cDLSettingUtil.prototype.getInternetAvailable = function () {
            if (hx.emulMode) {
                return true;
            }
            this.updateNetworkAvailabilty();
            hx.log("SettingUtil", 'getInternetAvailable : ' + this.mInternetAvailable);
            return this.mInternetAvailable;
        };
        cDLSettingUtil.prototype.setIpAddress = function (data) {
            this.mIpAddress = data.address;
        };
        cDLSettingUtil.prototype.setNetmask = function (data) {
            this.mNetmask = data.address;
        };
        cDLSettingUtil.prototype.setGateway = function (data) {
            this.mGateway = data.address;
        };
        cDLSettingUtil.prototype.setDns1st = function (data) {
            this.mDns1st = data.address;
        };
        cDLSettingUtil.prototype.setDns2nd = function (data) {
            this.mDns2nd = data.address;
        };
        cDLSettingUtil.prototype.getIpAddress = function () {
            return this.mIpAddress;
        };
        cDLSettingUtil.prototype.getNetmask = function () {
            return this.mNetmask;
        };
        cDLSettingUtil.prototype.getGateway = function () {
            return this.mGateway;
        };
        cDLSettingUtil.prototype.getDns1st = function () {
            return this.mDns1st;
        };
        cDLSettingUtil.prototype.getDns2nd = function () {
            return this.mDns2nd;
        };
        cDLSettingUtil.prototype.getallAddress = function () {
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            var devWireless = networkManager.getDevWireless();
            var netDev = devWired;
            if (!devWired && !devWireless) {
                hx.log("SettingUtil", "[fnGetallAddress()] - this.mWireDev is not exist");
                return null;
            }
            if (!devWired.enabled && devWireless.enabled) {
                netDev = devWireless;
            }
            var address = {};
            address.type = netDev.dhcpOn ? hx.l('LOC_DHCP_ID') : hx.l('LOC_MANUAL_ID');
            this.mIpAddress = address.ipAddress = netDev.ipAddress;
            this.mNetmask = address.netmask = netDev.netmask;
            this.mGateway = address.gateway = netDev.gateway;
            this.mDns1st = address.dns1st = netDev.dns1st;
            this.mDns2nd = address.dns2nd = netDev.dns2nd;
            hx.log("SettingUtil", "[fnGetallAddress()] - ipAddress : " + address.ipAddress);
            return address;
        };
        cDLSettingUtil.prototype.apScanning = function () {
            this.mApCacheList = [];
            this.mOriginApList = [];
            this.mWifiConfig.apScanning();
        };
        cDLSettingUtil.prototype.ConnectionTry = function () {
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            if (!devWired || devWired.connected == false) {
                hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev is not exist");
                this.fireEvent('tryConnection', "fail");
                return;
            }
            hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev.applyConfigure(); call");
            networkManager.updateDevWired(function (aDev) {
                aDev.dhcpOn = true;
                aDev.dnsAuto = true;
                return true;
            });
        };
        cDLSettingUtil.prototype.stopConnecting = function () {
            var ret = false;
            var devWired = this.mNetworkManager.getDevWired();
            if (!devWired) {
                hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev is not exist");
            }
            else {
                hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev.stopConnecting(); call");
                devWired.stopConnecting();
                ret = true;
            }
            return ret;
        };
        cDLSettingUtil.prototype.stopApScanning = function () {
            this.mApCacheList = [];
            this.mOriginApList = [];
            this.mWifiConfig.stopApScanning();
        };
        cDLSettingUtil.prototype.getPPPoEId = function () {
            return this.mPPPoEConfig.getID();
        };
        cDLSettingUtil.prototype.getPPPoEPassword = function () {
            return this.mPPPoEConfig.getPassword();
        };
        cDLSettingUtil.prototype.setPPPoEId = function (param) {
            this.mPPPoEConfig.setID(param.newid);
        };
        cDLSettingUtil.prototype.setPPPoEPassword = function (param) {
            this.mPPPoEConfig.setPassword(param.newpw);
        };
        cDLSettingUtil.prototype.connectPPPoE = function () {
            this.mPPPoEConfig.connect();
        };
        cDLSettingUtil.prototype.disConnectPPPoE = function () {
            this.mPPPoEConfig.disconnect();
        };
        cDLSettingUtil.prototype.wirelessStopConnecting = function () {
            var ret = false;
            var devWireless = this.mNetworkManager.getDevWireless();
            if (devWireless) {
                this.fireEvent('wirelessStopConnect');
                devWireless.stopConnecting();
                ret = true;
            }
            return ret;
        };
        cDLSettingUtil.prototype.manualConfig = function () {
            var _this = this;
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            if (!devWired || devWired.connected == false) {
                hx.log("SettingUtil", "[fnManualConfig()] - this.mWireDev is not exist");
                this.fireEvent('tryConnection', "fail");
                return;
            }
            networkManager.updateDevWired(function (aDev) {
                devWired.dhcpOn = false;
                devWired.dnsAuto = false;
                devWired.ipAddress = _this.mIpAddress;
                devWired.netmask = _this.mNetmask;
                devWired.gateway = _this.mGateway;
                devWired.dns1st = _this.mDns1st;
                devWired.dns2nd = _this.mDns2nd;
                return true;
            });
            hx.log("SettingUtil", "[fnManualConfig()] - this.mWireDev.applyConfigure(); called!!!");
        };
        cDLSettingUtil.prototype.applyConnectToAP = function (param) {
            var ap = this.mOriginApList[param.index];
            var userAp;
            if (!this.mNetworkManager.getDevWireless()) {
                this.fireEvent('wirelessLinkStatus', "fail");
                return;
            }
            if (param.ap) {
                userAp = this.mWifiConfig.getUserAP();
                userAp.essid = param.ap.essid;
                userAp.secure = param.ap.secure;
                userAp.authen = param.ap.authen;
                userAp.encrypt = param.ap.encrypt;
                this.mWifiConfig.applyConnectToAP(userAp, param.password);
            }
            else if (param.password) {
                this.mWifiConfig.applyConnectToAP(ap, param.password);
            }
            else {
                this.mWifiConfig.applyConnectToAP(ap, " ");
            }
        };
        cDLSettingUtil.prototype.checkWireConnection = function () {
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            if (!devWired) {
                hx.log("SettingUtil", "[fnCheckWireConnection()] - this.mWireDev is not exist");
                return false;
            }
            hx.log("SettingUtil", "[fnCheckWireConnection()] - this.mWireDev.connected : " + devWired.connected);
            return devWired.connected;
        };
        cDLSettingUtil.prototype.checkWirelessConnection = function () {
            var devWireless = this.mNetworkManager.getDevWireless();
            hx.log('netInfo', '####### fnCheckWirelessConnection ######## mWirelessDev.enabled : ' + devWireless.enabled);
            if (devWireless && devWireless.enabled) {
                return true;
            }
            return false;
        };
        cDLSettingUtil.prototype.getConnectionState = function () {
            return this.mConnectionState;
        };
        cDLSettingUtil.prototype.doingKeyGrab = function (aGrab) {
            //hx.log("warning", "###############  [fnDoingKeyGrab]  ###############");
            if (aGrab) {
                this.mConfigurationOipfObj.setField("grabkey", "true");
                hx.log("debug", "###############  KEY GRAB = TRUE   ###############");
            }
            else {
                this.mConfigurationOipfObj.setField("ungrabkey", "true");
                hx.log("debug", "###############  KEY GRAB = FALSE  ###############");
            }
            //hx.log("warning", "###############  [fnDoingKeyGrab]  ###############");
        };
        cDLSettingUtil.prototype.getFrontPanelText = function () {
            return this.mPrevFrontPanelText;
        };
        cDLSettingUtil.prototype.getRCUCustomCode = function () {
            return {
                rcuCustomCode: this.mLocalSystemOipfObj.getField('rcu_customcode')
            };
        };
        cDLSettingUtil.prototype.getPowerStateValues = function () {
            return {
                curPowerState: this.mLocalSystemOipfObj.powerState,
                previousPowerState: this.mLocalSystemOipfObj.previousPowerState,
                timeCurrentPowerState: this.mLocalSystemOipfObj.timeCurrentPowerState,
                wakeupReason: this.mLocalSystemOipfObj.getField('wakeupReason')
            };
        };
        cDLSettingUtil.prototype.startPowerOffTimer = function () {
            var _this = this;
            // this working takes 2msec every 1min
            var secPOT = (60 * 1000);
            var triggered = function () {
                var time = new Date();
                var hour = time.getHours();
                var min = time.getMinutes();
                var res = hx.svc.WEB_STORAGE.getPowerOffTimer();
                if (res.hour !== null && res.min !== null && res.isOn && res.isOn === 'On') {
                    if (parseInt(res.hour, 10) === hour && parseInt(res.min, 10) === min) {
                        _this.fireEvent('powerOffTimerEvent', null);
                    }
                }
            };
            this.mPowerOffTimerID = setInterval(triggered, secPOT);
        };
        cDLSettingUtil.prototype.stopPowerOffTimer = function () {
            clearInterval(this.mPowerOffTimerID);
        };
        cDLSettingUtil.prototype.startReminderPowerOff = function () {
            var _this = this;
            this.mReminderPowerOff = true;
            $('body').keydown(function (e) {
                _this.mReminderPowerOff = false;
            });
        };
        cDLSettingUtil.prototype.stopReminderPowerOff = function () {
            this.mReminderPowerOff = false;
        };
        cDLSettingUtil.prototype.getReminderPowerOff = function () {
            return this.mReminderPowerOff;
        };
        cDLSettingUtil.prototype.doReboot = function () {
            this.mConfigurationOipfObj.setField("doReboot", "true");
        };
        cDLSettingUtil.prototype.setApplicationReady = function () {
            this.mLocalSystemOipfObj.setApplicationReady();
        };
        cDLSettingUtil.prototype.checkDetachableStorage = function () {
            this.fireEvent('checkDetachableStorage');
        };
        cDLSettingUtil.prototype.setServerActivation = function (param) {
            var onoff;
            if (param.isOn === 'On') {
                onoff = true;
            }
            else {
                onoff = false;
            }
            if (param.startHour === '24' && param.startMin === '00') {
                param.startHour = '00';
            }
            this.mConfigurationOipfObj.setServerActivationTime(param.startHour, param.startMin, param.durationMin, onoff);
        };
        cDLSettingUtil.prototype.getIrdetoCasInfo = function () {
            return {
                clientinfo: this.mLocalSystemOipfObj.getField('irdetoclient_version')
            };
        };
        cDLSettingUtil.prototype.getpvrTsrOnOff = function () {
            var pvrTsrOnOff = this.mConfigurationOipfObj.timeShiftEnabled;
            hx.log('dsvc', 'getpvrTsrOnOff - value : ' + pvrTsrOnOff);
            return pvrTsrOnOff;
        };
        cDLSettingUtil.prototype.getDescrambleOnOff = function () {
            var DescrambleOnOff = this.mConfigurationOipfObj.descrambleOnOff;
            hx.log('dsvc', 'fnGetDescrambleOnOff - value : ' + DescrambleOnOff);
            return DescrambleOnOff;
        };
        cDLSettingUtil.prototype.setDescrambleOnOff = function (value) {
            hx.log('dsvc', 'fnSetDescrambleOnOff - value : ' + value);
            this.mConfigurationOipfObj.descrambleOnOff = value;
        };
        cDLSettingUtil.prototype.getAutoDeleteOnOff = function () {
            var AutoDeleteOnOff = this.mConfigurationOipfObj.autoDelete;
            hx.log('dsvc', 'fnGetAutoDeleteOnOff - value : ' + AutoDeleteOnOff);
            return AutoDeleteOnOff;
        };
        cDLSettingUtil.prototype.setAutoDeleteOnOff = function (value) {
            hx.log('dsvc', 'fnSetAutoDeleteOnOff - value : ' + value);
            this.mConfigurationOipfObj.autoDelete = value;
        };
        cDLSettingUtil.prototype.getNagraCasSn = function () {
            var casn = this.mLocalSystemOipfObj.getField('nagra_casn');
            hx.log('SettingUtil', "type :: " + typeof casn + " :: value : " + casn);
            return casn;
        };
        cDLSettingUtil.prototype.getNagraNuid = function () {
            var nuid = this.mLocalSystemOipfObj.getField('nagra_nuid');
            hx.log('SettingUtil', "type :: " + typeof nuid + " :: value : " + nuid);
            return nuid;
        };
        cDLSettingUtil.prototype.setpvrTsrOnOff = function (value) {
            this.mConfigurationOipfObj.timeShiftEnabled = value;
        };
        cDLSettingUtil.prototype.getoffsetGMT = function () {
            var retObj;
            retObj = Number(this.mConfigurationOipfObj.getField('offsetGMT'));
            hx.log('dsvc', 'fnGetoffsetGMT - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setoffsetGMT = function (param) {
            hx.log('dsvc', 'fnSetoffsetGMT - param : ' + param);
            return this.mConfigurationOipfObj.setField('offsetGMT', param.toString());
        };
        cDLSettingUtil.prototype.getdaylightsaving = function () {
            var retObj;
            switch (this.mConfigurationOipfObj.getField('daylightsaving')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
                default:
                    retObj = false;
                    break;
            }
            hx.log('dsvc', 'fnGetdaylightsaving - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setdaylightsaving = function (param) {
            hx.log('dsvc', 'fnSetdaylightsaving - param : ' + param);
            return this.mConfigurationOipfObj.setField('daylightsaving', param ? "true" : "false");
        };
        cDLSettingUtil.prototype.getVirtualSCW = function () {
            var retObj;
            switch (this.mLocalSystemOipfObj.getField('irdeto_virtualscw')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
                default:
                    retObj = false;
                    break;
            }
            hx.log('dsvc', 'fnGetVirtualSCW - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setVirtualSCW = function (param) {
            hx.log('dsvc', 'fnSetVirtualSCW - param : ' + param);
            return this.mLocalSystemOipfObj.setField('irdeto_virtualscw', param ? "true" : "false");
        };
        cDLSettingUtil.prototype.getUpdateTime = function () {
            var retObj;
            switch (this.mConfigurationOipfObj.getField('timeupdateonalltp')) {
                case 'false':
                    retObj = false;
                    break;
                case 'true':
                    retObj = true;
                    break;
                default:
                    retObj = false;
                    break;
            }
            hx.log('dsvc', 'fnGetUpdateTime - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setUpdateTime = function (param) {
            hx.log('dsvc', 'fnSetUpdateTime - param : ' + param);
            return this.mConfigurationOipfObj.setField('timeupdateonalltp', param ? "true" : "false");
        };
        cDLSettingUtil.prototype.getIRFusinInfo = function () {
            return {
                serialNumber: this.mLocalSystemOipfObj.serialNumber,
                language: this.mConfigurationOipfObj.preferredMenuLanguage,
                jtag: this.mLocalSystemOipfObj.getField('irdeto_jtag'),
                scw: this.mLocalSystemOipfObj.getField('irdeto_cwe'),
                sboot: this.mLocalSystemOipfObj.getField('irdeto_sboot'),
                otp: this.mLocalSystemOipfObj.getField('irdeto_otp'),
                ir_mancode: this.mLocalSystemOipfObj.getField('irdeto_mancode'),
                ir_hwcode: this.mLocalSystemOipfObj.getField('irdeto_hwcode'),
                ir_variant: this.mLocalSystemOipfObj.getField('irdeto_variant'),
                ir_sysid: this.mLocalSystemOipfObj.getField('irdeto_sysid'),
                ir_sigver: this.mLocalSystemOipfObj.getField('irdeto_sigver'),
                ir_keyver: this.mLocalSystemOipfObj.getField('irdeto_keyver')
            };
        };
        cDLSettingUtil.prototype.getIRControlByte = function () {
            var strControlByte = this.mLocalSystemOipfObj.getField('irdeto_control_byte');
            return {
                numControlByte: Number(strControlByte)
            };
        };
        cDLSettingUtil.prototype.getRegionId = function () {
            var regionId = this.mConfigurationOipfObj.regionId;
            if (this.cache_regionId === undefined) {
                this.cache_regionId = regionId;
            }
            hx.logadd('SettingUtil');
            hx.log("SettingUtil", 'fnGetRegionId() - regionId : ' + regionId);
            return regionId;
        };
        cDLSettingUtil.prototype.setRegionId = function (value) {
            this.cache_regionId = value;
            this.mConfigurationOipfObj.regionId = this.cache_regionId;
            hx.logadd('SettingUtil');
            hx.log("SettingUtil", 'fnSetRegionId() - this.cache_regionId : ' + this.cache_regionId);
        };
        cDLSettingUtil.prototype.getCountryId = function () {
            var countryId = this.mConfigurationOipfObj.countryId;
            if (this.cache_countryId === undefined) {
                this.cache_countryId = countryId;
            }
            hx.log("SettingUtil", 'fnGetCountryId() - this.cache_countryId : ' + countryId);
            return countryId;
        };
        cDLSettingUtil.prototype.setCountryId = function (countryId) {
            this.cache_countryId = countryId;
            this.mConfigurationOipfObj.countryId = countryId;
            hx.log("SettingUtil", 'fnSetCountryId() - this.cache_countryId : ' + this.cache_countryId);
        };
        cDLSettingUtil.prototype.getEnableDynamicUpdate = function () {
            var retObj;
            retObj = this.mLocalSystemOipfObj.boxServiceUpdateFlag;
            hx.log("SettingUtil", 'fnGetEnableDynamicUpdate - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableDynamicUpdate = function (value) {
            this.mLocalSystemOipfObj.boxServiceUpdateFlag = value;
            hx.log("SettingUtil", 'fnSetEnableDynamicUpdate - value : ' + value);
        };
        cDLSettingUtil.prototype.getEnableAntennaPower = function () {
            var retObj;
            retObj = this.mLocalSystemOipfObj.boxAntennaPowerStatus;
            hx.log("SettingUtil", 'fnGetEnableAntennaPower - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableAntennaPower = function (value) {
            this.mLocalSystemOipfObj.boxAntennaPowerStatus = value;
            hx.log("SettingUtil", 'fnSetEnableAntennaPower - value : ' + value);
        };
        cDLSettingUtil.prototype.checkFileExistence = function (uri) {
            return this.mLocalSystemOipfObj.checkFileExistence(uri);
        };
        cDLSettingUtil.prototype.getPromotionVideo = function () {
            var countryId = hx.svc.SETTING_UTIL.getCountryId();
            var path = null;
            var fileName = null;
            var uri = null;
            var duration = 0;
            switch (countryId) {
                case 'DEU':
                case 'GER':
                    path = '/mnt/hd2/Media/Video/';
                    fileName = 'Introduction Movie_DE.avi';
                    duration = 173000;
                    uri = path + fileName;
                    break;
                case 'AUS':
                    path = '/mnt/hd2/Media/Video/';
                    fileName = 'Humax_HMS_Promotion_Eng.mp4';
                    duration = 173000;
                    uri = path + fileName;
                    break;
                default:
                    path = '/mnt/hd2/Media/Video/';
                    fileName = 'Humax_HMS_Promotion_Eng.mp4';
                    duration = 173000;
                    uri = path + fileName;
                    break;
            }
            if (!!uri) {
                var o = hx.svc.SETTING_UTIL.checkFileExistence(uri);
                if (o === true) {
                    uri = 'file://' + uri;
                }
                else {
                    uri = null;
                }
            }
            return {
                'uri': uri,
                'title': fileName,
                'duration': duration
            };
        };
        cDLSettingUtil.prototype.getUpdatedList = function () {
            return {
                updatedList: this.mLocalSystemOipfObj.updatedList
            };
        };
        cDLSettingUtil.prototype.getEnableSambaServer = function () {
            var retObj = false;
            if (!this.mSambaServer) {
                return retObj;
            }
            retObj = this.mSambaServer.started;
            hx.log("SettingUtil", 'fnGetEnableSambaServer - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableSambaServer = function (value) {
            if (value) {
                this.mSambaServer.start();
            }
            else {
                this.mSambaServer.stop();
            }
            hx.log("SettingUtil", 'fnSetEnableSambaServer - value : ' + this.mSambaServer.started);
        };
        cDLSettingUtil.prototype.getEnableFtpServer = function () {
            var retObj = false;
            if (!this.mFtpServer) {
                return retObj;
            }
            retObj = this.mFtpServer.started;
            hx.log("SettingUtil", 'fnGetEnableFtpServer - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setEnableFtpServer = function (param) {
            if (param) {
                this.mFtpServer.start();
            }
            else {
                this.mFtpServer.stop();
            }
            hx.log("SettingUtil", 'fnSetEnableFtpServer - value : ' + this.mFtpServer.started);
        };
        cDLSettingUtil.prototype.getPasswordSet = function () {
            var retObj, isAccess;
            isAccess = this.mSambaServer.getPublicAccess('admin');
            isAccess = this.mFtpServer.getPublicAccess('admin');
            if (isAccess) {
                retObj = false;
                return retObj;
            }
            retObj = hx.svc.WEB_STORAGE.getFileSharePasswd();
            hx.log("SettingUtil", 'fnGetPasswordSet - retObj.passwd : ' + retObj.passwd);
            return retObj.passwd;
        };
        cDLSettingUtil.prototype.setPasswordSet = function (value) {
            if (value) {
                this.mSambaServer.setPassword('admin', '', value);
                this.mFtpServer.setPassword('admin', '', value);
                this.mSambaServer.setPublicAccess('admin', false);
                this.mFtpServer.setPublicAccess('admin', false);
            }
            else {
                this.mSambaServer.setPublicAccess('admin', true);
                this.mFtpServer.setPublicAccess('admin', true);
            }
            hx.svc.WEB_STORAGE.setFileSharePasswd(value);
            hx.log("SettingUtil", 'fnSetPasswordSet - value : ' + value);
        };
        cDLSettingUtil.prototype.getEnableDownloadFolder = function () {
            var ret = false;
            var mShareFolderCollection = this.mSambaServer.getShareFolder('admin');
            var DOWNLOAD = "Download";
            var item;
            for (var i = 0, len = mShareFolderCollection.length; i < len; i++) {
                item = mShareFolderCollection[i];
                if (item.name === DOWNLOAD) {
                    ret = item.writable;
                    break;
                }
            }
            hx.log("SettingUtil", 'fnGetEnableDownloadFolder - ret : ' + ret);
            return ret;
        };
        cDLSettingUtil.prototype.setEnableDownloadFolder = function (param) {
            if (param.enable) {
                this.mSambaServer.setShareFolder('admin', param.name, 'rw');
                this.mFtpServer.setShareFolder('admin', param.name, 'rw');
            }
            else {
                this.mSambaServer.setShareFolder('admin', param.name, 'ro');
                this.mFtpServer.setShareFolder('admin', param.name, 'ro');
            }
            hx.log("SettingUtil", 'fnSetEnableDownloadFolder - value : ' + param);
        };
        cDLSettingUtil.prototype.getColdBootDone = function () {
            if (!!this.mLocalSystemOipfObj) {
                if (!!this.mLocalSystemOipfObj.coldBootDone) {
                    if (!!this.mLocalSystemOipfObj.coldBootDone.getColdBootDone) {
                        return this.mLocalSystemOipfObj.coldBootDone.getColdBootDone();
                    }
                }
            }
        };
        cDLSettingUtil.prototype.showMsgbox = function () {
            if (!!this.mLocalSystemOipfObj) {
                var status = this.mLocalSystemOipfObj.getStatusOfTVBrowser();
                if (status) {
                    this.mLocalSystemOipfObj.showMsgbox();
                }
            }
            return;
        };
        cDLSettingUtil.prototype.getNoOtaAction = function () {
            var retObj;
            retObj = this.mConfigurationOipfObj.getField('no_ota_action');
            hx.log("SettingUtil", 'fnGetNoOtaAction - retObj : ' + retObj);
            return (retObj === "true") ? true : false;
        };
        cDLSettingUtil.prototype.setNoOtaAction = function (param) {
            var retObj;
            hx.log("SettingUtil", 'fnSetNoOtaAction - param : ' + param);
            return this.mConfigurationOipfObj.setField('no_ota_action', param);
        };
        cDLSettingUtil.prototype.getNoOtaSWVer = function () {
            var retObj;
            retObj = this.mConfigurationOipfObj.getField('no_ota_swversion');
            hx.log("SettingUtil", 'fnGetNoOtaSWVer - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setNoOtaSWVer = function (param) {
            var retObj;
            hx.log("SettingUtil", 'fnSetNoOtaSWVer - param : ' + param);
            return this.mConfigurationOipfObj.setField('no_ota_swversion', param);
        };
        cDLSettingUtil.prototype.getLaterOtaCount = function () {
            var retObj;
            retObj = this.mConfigurationOipfObj.getField('later_ota_count');
            hx.log("SettingUtil", 'fnGetLaterOtaCount - retObj : ' + retObj);
            return Number(retObj);
        };
        cDLSettingUtil.prototype.setLaterOtaCount = function (param) {
            var retObj;
            hx.log("SettingUtil", 'fnSetLaterOtaCount - param : ' + param);
            return this.mConfigurationOipfObj.setField('later_ota_count', param);
        };
        cDLSettingUtil.prototype.getPrivacyPolicy = function () {
            var retObj, isEnable, mShareFolderCollection;
            retObj = hx.svc.WEB_STORAGE.getPrivacyPolicy();
            hx.log("SettingUtil", 'fnGetPrivacyPolicy - retObj : ' + retObj.state);
            if (retObj.state === 'false') {
                return false;
            }
            return true;
        };
        cDLSettingUtil.prototype.setPrivacyPolicy = function (value) {
            var privacyPolicyState = value;
            hx.svc.WEB_STORAGE.setPrivacyPolicyState(privacyPolicyState);
            this.fireEvent('privacyPolicyChange', privacyPolicyState);
            hx.log("SettingUtil", 'fnSetPrivacyPolicy - value : ' + privacyPolicyState);
        };
        cDLSettingUtil.prototype.getPrivacyPolicyVersion = function () {
            var retObj = this.mConfigurationOipfObj.getField('privacy_policy_ver');
            hx.log("SettingUtil", 'getPrivacyPolicyVersion - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.setPrivacyPolicyVersion = function (aValue) {
            hx.log("SettingUtil", 'setPrivacyPolicyVersion - value : ' + aValue);
            this.mConfigurationOipfObj.setField('privacy_policy_ver', aValue || "disagree");
        };
        cDLSettingUtil.prototype.checkGatewayStatus = function () {
            if (hx.emulMode) {
                return;
            }
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            var devWireless = networkManager.getDevWireless();
            var netDev = devWired;
            if (!devWired.connected && devWireless !== null && devWireless.enabled) {
                netDev = devWireless;
            }
            return netDev.checkGatewayAccess();
        };
        cDLSettingUtil.prototype.checkDnsStatus = function () {
            if (hx.emulMode) {
                return;
            }
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            var devWireless = networkManager.getDevWireless();
            var netDev = devWired;
            if (!devWired.connected && devWireless !== null && devWireless.enabled) {
                netDev = devWireless;
            }
            return netDev.checkDnsAccess();
        };
        cDLSettingUtil.prototype.checkInternetConnection = function () {
            var networkManager = this.mNetworkManager;
            var devWired = networkManager.getDevWired();
            var devWireless = networkManager.getDevWireless();
            var netDev = devWired;
            if (!devWired.connected && devWireless != null && devWireless.enabled) {
                netDev = devWireless;
                if (!devWireless.enabled) {
                    return false;
                }
            }
            if (netDev.checkInternetConnection) {
                hx.log("SettingUtil", 'Call checkInternetConnection!!');
                return netDev.checkInternetConnection();
            }
            return false;
        };
        cDLSettingUtil.prototype.setNtpServer = function (param) {
            hx.log("SettingUtil", 'fnSetNtpServer - ntp_server_url : ' + param);
            return this.mLocalSystemOipfObj.setField('ntp_server_url', param);
        };
        cDLSettingUtil.prototype.setGuidancePolicy = function (aGuidancePolicy) {
            this.mConfigurationOipfObj.guidancepolicy = aGuidancePolicy;
        };
        cDLSettingUtil.prototype.getGuidancePolicy = function () {
            return this.mConfigurationOipfObj.guidancepolicy;
        };
        cDLSettingUtil.prototype.setNCDNotifyData = function (aNeedToShowAgain) {
            hx.log("SettingUtil", 'setNCDNotifyData - value : ' + aNeedToShowAgain);
            this.mOperatorFeature.setNcdNotifyData(aNeedToShowAgain);
        };
        cDLSettingUtil.prototype.getNCDNetworkMsg = function () {
            var ret = this.mOperatorFeature.getNcdNetworkMsg(0);
            hx.log("SettingUtil", 'getNCDNetworkMsg - : ' + ret);
            return ret;
        };
        cDLSettingUtil.prototype.isHbbTVActivated = function () {
            var ret = false;
            if (this.mLocalSystemOipfObj.isHbbTVActive !== undefined) {
                ret = !!this.mLocalSystemOipfObj.isHbbTVActive;
            }
            return ret;
        };
        cDLSettingUtil.prototype.getHbbTvEnable = function () {
            var bEnabled = this.mConfigurationOipfObj.hbbTvEnbled;
            hx.log("SettingUtil", 'fnGetHbbTvEnable - value : ' + bEnabled);
            return bEnabled;
        };
        cDLSettingUtil.prototype.getTVPortalUrl = function () {
            return this.mLocalSystemOipfObj.getField('url_portal');
        };
        cDLSettingUtil.prototype.getPrefixUrl = function () {
            if (new Date().getFullYear() < 2015) {
                return null;
            }
            return this.mLocalSystemOipfObj.getField("fvc_networkuriprefix");
        };
        cDLSettingUtil.prototype.getIpEpgSvcCount = function () {
            var retObj;
            retObj = this.mConfigurationOipfObj.getField('ipepgDownloadSvcCount');
            hx.log("SettingUtil", 'ipepgDownloadSvcCount - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.getIpEpgEvtCount = function () {
            var retObj;
            retObj = this.mConfigurationOipfObj.getField('ipepgDownloadEvtCount');
            hx.log("SettingUtil", 'ipepgDownloadEvtCount - retObj : ' + retObj);
            return retObj;
        };
        cDLSettingUtil.prototype.getHdmiEdidInfo = function () {
            var val, retObj;
            if (!hx.emulMode) {
                val = $.parseJSON(this.mLocalSystemOipfObj.getField('hdmi_edidinfo'));
                retObj = val.HDMI_EDID_INFO;
            }
            return retObj;
        };
        cDLSettingUtil.prototype.getNetworkId = function () {
            hx.log("info", "[getNetworkId]: " + JSON.parse(this.mLocalSystemOipfObj.getField("network_ids")).NETWORK_ID);
            return JSON.parse(this.mLocalSystemOipfObj.getField("network_ids")).NETWORK_ID;
        };
        cDLSettingUtil.prototype.deleteCookies = function () {
            if (this.mLocalSystemOipfObj.deleteCookies) {
                this.mLocalSystemOipfObj.deleteCookies();
            }
        };
        cDLSettingUtil.prototype.getInitHumaxTvPortal = function () {
            return (this.mConfigurationOipfObj.getField("init_humaxtvportal") == "true") ? true : false;
        };
        cDLSettingUtil.prototype.setInitHumaxTvPortal = function (aValue) {
            this.mConfigurationOipfObj.setField('init_humaxtvportal', aValue ? "true" : "false");
        };
        cDLSettingUtil.prototype.disconnectNetworkForTest = function () {
            this.mNetworkAvailable = false;
            this.mInternetAvailable = false;
            this.fireEvent('NetworkAvailableChanged', this.mNetworkAvailable);
            this.fireEvent('networkStatus', 'dns', 'fail');
        };
        cDLSettingUtil.prototype.connectNetworkForTest = function () {
            this.mNetworkAvailable = true;
            this.mInternetAvailable = true;
            this.fireEvent('NetworkAvailableChanged', this.mNetworkAvailable);
            this.fireEvent('InternetAvailableChanged', this.mInternetAvailable);
            this.fireEvent('networkStatus', 'dns', 'success');
        };
        cDLSettingUtil.prototype.isMute = function () {
            return this.mLocalSystemOipfObj.mute;
        };
        return cDLSettingUtil;
    })(__KERNEL__);
    return cDLSettingUtil;
});
