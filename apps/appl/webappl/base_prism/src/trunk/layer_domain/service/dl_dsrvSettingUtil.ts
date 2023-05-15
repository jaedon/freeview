/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvSettingUtil.js
/*jslint sloppy:true, nomen:true, vars:true */
//    SETTING_UTIL: ['layer_domain/service/dl_dsrvSettingUtil.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

enum eNET_TYPE {
    TYPE_LAN,
    TYPE_WLAN,
    TYPE_PPPOE,
    TYPE_NONE
}
enum eNET_STATE {
    NET_WIRE = 0,
    NET_WIRELESS = 1,
    CON_FAIL = 0,
    CON_SUCCESS = 1,
    CABLE_LINK_DISCON = 2,
    CABLE_LINK_CON = 3,
    WIFI_SCAN_AP = 0,
    WIFI_SCAN_SUCCESS = 1,
    WIFI_SCAN_FAIL = 2,
    WIFI_CON_SUCCESS = 3,
    WIFI_CON_FAIL = 4,
    WIFI_KEY_INVALID = 5,
    WIFI_USB_EXTRACTED = 4,
    WIFI_USB_INSERTED = 5,
    GATEWAY_ACCESS_FAIL = 0,
    GATEWAY_ACCESS_SUCCESS = 1,
    DNS_ACCESS_FAIL = 2,
    DNS_ACCESS_SUCCESS = 3,
    INTERNET_ACCESS_FAIL = 4,
    INTERNET_ACCESS_SUCCESS =5
}
enum ePOWER_STATE {
    POWER_OFF,
    POWER_ON,
    POWER_PASSIVE_STANDBY,
    POWER_ACTIVE_STANDBY
}

function logNetworkInterface(aDev: NetworkInterface, aTitle = "unnamed", aLogLevel = "netInfo"): void {
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
interface _ifEventForNetworkInterface {
    (aNetworkType: number): (aState: number) => void;
}

class CNetworkDeivceModel {
    static __addDevice = function(aDev: NetworkInterface, aListDev: NetworkInterface[]) {
        if (aListDev.push(aDev) !== 1) {
            for (var i = 0; i < aListDev.length; i++) {
                logNetworkInterface(aListDev[i], undefined, "debug");
            }
            throw "we support only one device, check above log";;
        }
    }
    private _listDevWired: NetworkInterface[] = [];
    private _listDevWireless: NetworkInterface[] = [];
    private _listDevPPPoE: NetworkInterface[] = [];
    constructor() {
    }
    addWiredDevice(aDev: NetworkInterface) {
        //hx.log("debug", "[CNetworkDeivceModel][addWiredDevice]");
        CNetworkDeivceModel.__addDevice(aDev, this._listDevWired);
    }
    addWirelessDevice(aDev: NetworkInterface) {
        //hx.log("debug", "[CNetworkDeivceModel][addWirelessDevice]");
        CNetworkDeivceModel.__addDevice(aDev, this._listDevWireless);
    }
    addPPPoEDevice(aDev: NetworkInterface) {
        //hx.log("debug", "[CNetworkDeivceModel][addPPPoEDevice]");
        CNetworkDeivceModel.__addDevice(aDev, this._listDevPPPoE);
    }
    getWiredDevice(): NetworkInterface {
        //hx.log("debug", "[CNetworkDeivceModel][getWiredDevice]: " + this._listDevWired[0] || null);
        return this._listDevWired[0] || null;
    }
    getWirelessDevice(): NetworkInterface {
        //hx.log("debug", "[CNetworkDeivceModel][getWirelessDevice]: " + this._listDevWireless[0] || null);
        return this._listDevWireless[0] || null;
    }
    getPPPoEDevice(): NetworkInterface {
        //hx.log("debug", "[CNetworkDeivceModel][getPPPoEDevice]: " + this._listDevPPPoE[0] || null);
        return this._listDevPPPoE[0] || null;
    }
    truncate(aType?: eNET_TYPE) {
        //hx.log("debug", "[CNetworkDeivceModel][truncate]: " + eNET_STATE[aType]);
        if (aType) {
            if (aType === eNET_TYPE.TYPE_LAN) {
                this._listDevWired = [];
            } else if (aType === eNET_TYPE.TYPE_WLAN) {
                this._listDevWireless = [];
            } else if (aType === eNET_TYPE.TYPE_PPPOE) {
                this._listDevPPPoE = [];
            }
        } else {
            this._listDevWired = [];
            this._listDevWireless = [];
            this._listDevPPPoE = [];
        }
    }
}
class CNetworkDeviceManager {
    private _modelDevs: CNetworkDeivceModel = new CNetworkDeivceModel();

    constructor(
        aOnReqNetworkInterfaces: () => NetworkInterfaceCollection,
        aOnReqOnAccessStatusUpdated: _ifEventForNetworkInterface,
        aOnReqOnLinkStatusChanged: _ifEventForNetworkInterface
    ) {
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
    isNetworkAvailable() {
        var model = this._modelDevs;
        var ret = false;
        var device = model.getWiredDevice()
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
    }
    private _doGetNetworkInterface() {
        //hx.log("debug", "[CNetworkDeviceManager][_doGetNetworkInterface]");
        return this.onReqNetworkInterfaces();
    }
    private _doGetEventCBAccessStatusUpdated(aNetworkType: eNET_TYPE) {
        //hx.log("debug", "[CNetworkDeviceManager][_doGetEventCBAccessStatusUpdated]");
        return this.onReqOnAccessStatusUpdated(aNetworkType);
    }
    private _doGetEventCBLinkStatusChanged(aNetworkType: eNET_TYPE) {
        //hx.log("debug", "[CNetworkDeviceManager][_doGetEventCBLinkStatusChanged]");
        return this.onReqOnLinkStatusChanged(aNetworkType);
    }

    private _setDevInfo(aDev: NetworkInterface) {
        //hx.log("debug", "[CNetworkDeviceManager][_setDevInfo]");
        var model = this._modelDevs;
        aDev.onAccessStatusUpdated = this._doGetEventCBAccessStatusUpdated(aDev.networkType);
        aDev.onLinkStatusChanged = this._doGetEventCBLinkStatusChanged(aDev.networkType);
        logNetworkInterface(aDev);
        switch (aDev.networkType) {
            case eNET_TYPE.TYPE_LAN:
                model.addWiredDevice(aDev);
                break;
            case eNET_TYPE.TYPE_WLAN:
                model.addWirelessDevice(aDev);
                break;
            case eNET_TYPE.TYPE_PPPOE:
                model.addPPPoEDevice(aDev);
                break;
            default:
                throw "??";
                break;
        }
    }
    onReqNetworkInterfaces: () => NetworkInterfaceCollection = () => {
        throw "must override via construcctor";
        return <NetworkInterfaceCollection><any>[];
    }
    onReqOnAccessStatusUpdated: _ifEventForNetworkInterface;
    onReqOnLinkStatusChanged: _ifEventForNetworkInterface;

    refresh(aNetworkType?: eNET_TYPE) {
        //hx.log("debug", "[CNetworkDeviceManager][refresh]");
        var listDev = this._doGetNetworkInterface();
        var dev: NetworkInterface;
        var fnCheck = function(aType) {
            if (typeof aType === "undefined") {
                fnCheck =  function() {
                    return true;
                }
            } else {
                fnCheck = function(aDev: NetworkInterface) {
                    return aDev.networkType === aType;
                }

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
    }
    getDevWired(): NetworkInterface {
        //hx.log("debug", "[CNetworkDeviceManager][getDevWired]");
        return this._modelDevs.getWiredDevice();
    }
    getDevWireless(): NetworkInterface {
        //hx.log("debug", "[CNetworkDeviceManager][getDevWireless]");
        return this._modelDevs.getWirelessDevice();
    }
    getDevPPPoE(): NetworkInterface {
        //hx.log("debug", "[CNetworkDeviceManager][getDevPPPoE]");
        return this._modelDevs.getPPPoEDevice();
    }
    _updateDevice(aCB: (aDev: NetworkInterface) => void, aDev: NetworkInterface) {
        if (aDev && aCB(aDev)) {
            aDev.applyConfigure();
        }
    }
    updateDevWired(aCB: (aDev: NetworkInterface) => boolean) {
        //hx.log("debug", "[CNetworkDeviceManager][updateDevWired]");
        this._updateDevice(aCB, this.getDevWired());
    }
    updateDevWireless(aCB: (aDev: NetworkInterface) => boolean) {
        //hx.log("debug", "[CNetworkDeviceManager][updateDevWireless]");
        this._updateDevice(aCB, this.getDevWireless());
    }
    updateDevPPPoE(aCB: (aDev: NetworkInterface) => boolean) {
        //hx.log("debug", "[CNetworkDeviceManager][updateDevPPPoE]");
        this._updateDevice(aCB, this.getDevPPPoE());
    }
}

class cDLSettingUtil extends __KERNEL__ implements ifSVCSettingUtil {
    private mLocalSystemOipfObj: HLocalSystem = this.getPluginInstance('LocalSystem');
//    private mNetManager: HMX_NetworkManager = this.mLocalSystemOipfObj.networkManager;
//    private mNetDevList: NetworkInterfaceCollection = this.mLocalSystemOipfObj.networkInterfaces;
    private mWifiConfig = null;
    private mPPPoEConfig = null;
    private mConfigurationOipfObj = this.getPluginInstance('Configuration');
    //private mWoon = this.getPluginInstance('Woon');
    //private mWoonServer = this.mWoon.server;
    //private mWoonClient = this.mWoon.client;
    private mDial = this.getPluginInstance('Dial');
    private mDialServer = this.mDial.server;
    private mDialClient = this.mDial.client;
    //private mSatip = this.getPluginInstance('Satip');
    //private mSatipServer = this.mSatip.server;
    //private mSatipClient = this.mSatip.client;
    private mBluetoothManager = this.getPluginInstance('BluetoothManager');
    private mDmsObj = this.getPluginInstance('DLNA.DMS');
    private mDmrObj = this.getPluginInstance('DLNA.DMR');
    private mAppMain = this.getPluginInstance('ApplicationManager').getOwnerApplication(window.document);
    private mSamba = this.getPluginInstance('Samba');
    private mSambaServer = this.getPluginInstance('SambaServer');
    private mFtp = this.getPluginInstance('Ftp');
    private mFtpServer = this.getPluginInstance('FtpServer');
    private mOperatorFeature: HMXOperatorFeature = this.getPluginInstance('OperatorFeature');
    private mConnectionState: string = 'not';
    private mIpAddress = "0.0.0.0";
    private mNetmask = "0.0.0.0";
    private mGateway = "0.0.0.0";
    private mDns1st = "0.0.0.0";
    private mDns2nd = "0.0.0.0";
    private mOriginApList = [];
    private mApCacheList = [];

    /* Cached Buffer Data Area Start */
    private cache_instantReplayTime;
    private cache_instantSkipTime;
    private cache_startPadding;
    private cache_endPadding;
    private cache_langMenu;
    private cache_langSubTtl;
    private cache_langAudio;
    private cache_infoDisplayTimeout;
    private cache_regionId;
    private cache_countryId;

    private mTimerSignalInform = null;
    private mPrevFrontPanelText;
    private mNetworkAvailable = false;
    private mInternetAvailable = false;

    private mPowerOffTimerID;
    private mReminderPowerOff = false;

    private mNetworkManager = new CNetworkDeviceManager(
        () => {
            return this.mLocalSystemOipfObj.networkInterfaces;
        },
        (aNetworkType: eNET_TYPE) => {
            var ret;
            switch (aNetworkType) {
                case eNET_TYPE.TYPE_LAN:
                    ret = (state) => {
                        var type: string = '' , result: string = "";
                        switch (state) {
                            case eNET_STATE.GATEWAY_ACCESS_FAIL:
                                type = 'gateway';
                                result = 'fail';
                                break;
                            case eNET_STATE.GATEWAY_ACCESS_SUCCESS:
                                type = 'gateway';
                                result = 'success';
                                break;
                            case eNET_STATE.INTERNET_ACCESS_FAIL:
                            case eNET_STATE.DNS_ACCESS_FAIL:
                                type = 'dns';
                                result = 'fail';
                                break;
                            case eNET_STATE.INTERNET_ACCESS_SUCCESS:
                            case eNET_STATE.DNS_ACCESS_SUCCESS:
                                type = 'dns';
                                result = 'success';
                                break;
                            default:
                                result = 'fail';
                                break;
                        }
                        hx.log("SettingUtil", "[onAccessStatusUpdated()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result+' ,[type : '+type);
                        if (state === eNET_STATE.DNS_ACCESS_SUCCESS || state == eNET_STATE.INTERNET_ACCESS_SUCCESS) {
                            this.mInternetAvailable = true;
                            this.fireEvent('InternetAvailableChanged', true);
                            this.fireEvent('networkStatus', type, 'success');
                        } else {
                            this.mInternetAvailable = false;
                            this.fireEvent('networkStatus', type, result);
                        }
                        //(function() {
                        //    var debug = (<any>window).d.network;
                        //    if (debug) {
                        //        debug.onReqOnAccessStatusUpdated.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state])
                        //    }
                        //})();
                    };
                    break;
                case eNET_TYPE.TYPE_WLAN:
                    ret = (state) => {
                        var type : string = '' , result : string = "";
                        switch (state) {
                            case eNET_STATE.GATEWAY_ACCESS_FAIL:
                                type = 'gateway';
                                result = 'fail';
                                break;
                            case eNET_STATE.GATEWAY_ACCESS_SUCCESS:
                                type = 'gateway';
                                result = 'success';
                                break;
                            case eNET_STATE.INTERNET_ACCESS_FAIL:
                            case eNET_STATE.DNS_ACCESS_FAIL:
                                type = 'dns';
                                result = 'fail';
                                break;
                            case eNET_STATE.INTERNET_ACCESS_SUCCESS:
                            case eNET_STATE.DNS_ACCESS_SUCCESS:
                                type = 'dns';
                                result = 'success';
                                break;
                            default:
                                result = 'fail';
                                break;
                        }
                        hx.log("SettingUtil", "[onAccessStatusUpdated()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result);
                        if (state === eNET_STATE.DNS_ACCESS_SUCCESS || state == eNET_STATE.INTERNET_ACCESS_SUCCESS) {
                            this.mInternetAvailable = true;
                            this.fireEvent('InternetAvailableChanged', true);
                            this.fireEvent('networkStatus', type, 'success');
                        } else {
                            this.mInternetAvailable = false;
                            this.fireEvent('networkStatus', type, result);
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
        },
        (aNetworkType: eNET_TYPE) => {
            var ret;
            switch (aNetworkType) {
                case eNET_TYPE.TYPE_LAN:
                    ret = (state) => {
                        var result: string = "";
                        switch (state) {
                            case eNET_STATE.CON_FAIL:
                                result = 'fail';
                                break;
                            case eNET_STATE.CON_SUCCESS:
                                result = 'success';
                                break;
                            case eNET_STATE.CABLE_LINK_DISCON:
                                result = 'discon';
                                break;
                            case eNET_STATE.CABLE_LINK_CON:
                                result = 'con';
                                break;
                            default:
                                result = 'fail';
                                break;
                        }
                        hx.log("SettingUtil", "[onLinkStatusChanged()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result);
                        if (result === 'success') {
                            this.mConnectionState = 'dhcp';
                            this.checkInternetConnection();
                        }
                        this.fireEvent('tryConnection', result);
                        this.updateNetworkAvailabilty();
                        //(function debug() {
                        //    var debug = (<any>window).d.network;
                        //    if (debug) {
                        //        debug.onReqOnLinkStatusChanged.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                        //    }
                        //})();
                    }
                    break;
                case eNET_TYPE.TYPE_WLAN:
                    ret = (state) => {
                        var result: string = "";
                        switch (state) {
                            case eNET_STATE.CON_SUCCESS:
                                this.mNetworkManager.updateDevWireless((aDev) => {
                                    aDev.dhcpOn = true;
                                    return false;
                                })
                                this.mConnectionState = 'wifi';
                                result = 'success';
                                this.fireEvent('wirelessLinkStatus', result);
                                this.updateNetworkAvailabilty();
                                break;
                            case eNET_STATE.CON_FAIL:
                                result = 'fail';
                                this.fireEvent('wirelessLinkStatus', result);
                                this.updateNetworkAvailabilty();
                                break;
                            case eNET_STATE.WIFI_CON_FAIL:
                                result = 'fail';
                                this.updateNetworkAvailabilty();
                                break;
                            case eNET_STATE.WIFI_USB_INSERTED:
                                if (hx.config.useUSBWireless) {
                                    var networkManager = this.mNetworkManager;
                                    networkManager.refresh(eNET_TYPE.TYPE_WLAN);
                                    networkManager.updateDevWireless((aDev) => {
                                        aDev.enabled = state !== 0;
                                        return false;
                                    });
                                }
                                break;
                            case eNET_STATE.WIFI_USB_EXTRACTED:
                                if (hx.config.useUSBWireless) {
                                    var networkManager = this.mNetworkManager;
                                    networkManager.refresh(eNET_TYPE.TYPE_WLAN);
                                }
                                break;
                            default:
                                result = 'fail';
                                break;
                        }
                        hx.log("debug", "[onLinkStatusChanged()][networkType: " + eNET_TYPE[aNetworkType] + "][ - state : " + state + "][ - result : " + result);
                        if (result === 'success') {
                            this.checkInternetConnection();
                        }
                        //(function debug() {
                        //    var debug = (<any>window).d.network;
                        //    if (debug) {
                        //        debug.onReqOnLinkStatusChanged.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                        //    }
                        //})();
                    }
                    break;
                case eNET_TYPE.TYPE_PPPOE:
                    ret = (state) => {
                        var result : string = "";
                        switch (state) {
                            case eNET_STATE.CON_FAIL:
                                result = 'fail';
                                this.fireEvent('pppoeLinkStatus', result);
                                break;
                            case eNET_STATE.CON_SUCCESS:
                                result = 'success';
                                this.fireEvent('pppoeLinkStatus', result);
                                break;
                            case eNET_STATE.CABLE_LINK_DISCON:
                                result = 'discon';
                                this.fireEvent('pppoeLinkStatus', result);
                                break;
                            case eNET_STATE.CABLE_LINK_CON:
                                this.fireEvent('pppoeLinkStatus', result);
                                result = 'con';
                                break;
                            default:
                                result = 'fail';
                                this.fireEvent('pppoeLinkStatus', result);
                                break;
                        }
                        hx.log("SettingUtil", "[onLinkStatusChanged()][networkType: " + eNET_TYPE[aNetworkType] + "][ - result : " + result);
                        if (result === 'success') {
                            this.mConnectionState = 'dhcp';
                            this.checkInternetConnection();
                        }
                        this.fireEvent('tryConnection', result);
                        this.updateNetworkAvailabilty();
                        //(function debug() {
                        //    var debug = (<any>window).d.network;
                        //    if (debug) {
                        //        debug.onReqOnLinkStatusChanged.push(eNET_TYPE[aNetworkType] + ": " + eNET_STATE[state]);
                        //    }
                        //})();
                    }
            }

            return ret;
        }
    );

    checkTvPortalServer(success, error) {
        $.ajax({
            url: 'http://www.humaxtvportal.com/api/GetCurrentRegionInfo',
            timeout: 3000,
            success: (data,str, res) => {
                hx.log('error','checkTvPortalServer SUCCESS');
                hx.log('error','checkTvPortalServer SUCCESS status('+res.status+')');
                success();
            },
            error: (data) => {
                hx.log('error','checkTvPortalServer ERROR');
                hx.log('error','checkTvPortalServer ERROR status('+data.status+')');
                console.log('checkTvPortalServer_error_res : ' + data);
                error();
            }
        });
    }

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('netInfo');
        hx.logadd('grab');

        var netManager: HMX_NetworkManager = this.mLocalSystemOipfObj.networkManager;
        this.mWifiConfig = netManager.getWifiConfig(1);
        this.mPPPoEConfig = netManager.getPPPoEConfig(2);

        hx.logadd("SettingUtil");
//        hx.logadd('eventWifiScanningResult');
//        hx.logadd('conNetServer');
        hx.log("SettingUtil", "hx.svc.SETTING_UTILEvent's _init() called");

        /* Cached Buffer Data Area End */

        this.defineEvents(['NetworkAvailableChanged', 'PowerStateChanged', 'autoPowerDownEvent', 'powerOffTimerEvent', 'LanguageChanged', 'privacyPolicyChange', 'PlaybackOptionChange',
            'NotifyPinCode', 'checkDetachableStorage', 'ResultEvent', 'StateUpdate', 'tryConnection', 'networkStatus', 'InternetAvailableChanged', 'wirelessLinkStatus', 'eventWifiScanningResult', 'NotiSignalInfo', 'pppoeLinkStatus', 'applyConnectToAP', 'ThermalProtectionOccured','wirelessStopConnect'
        ]);

        this.mLocalSystemOipfObj.onThermalProtectionOccured = () => {
            hx.log('warning', 'trigger ThermalProtectionOccured event to Page...');
            this.fireEvent('ThermalProtectionOccured');
        };
        this.mLocalSystemOipfObj.onPowerStateChange = (powerState: number) => {
            hx.log('warning', 'trigger PowerStateChanged event to Page... powerState : [' + powerState + ']');
            this.fireEvent('PowerStateChanged', powerState);
        };
        this.mLocalSystemOipfObj.onUserActionIdle = () => {
            hx.log('warning', 'trigger onUserActionIdle event to Page... onUserActionIdle');
            var res = hx.svc.WEB_STORAGE.getAutoPowerDown();

            if (res.isOn === 'true') {
                this.fireEvent('autoPowerDownEvent', null);
            }
        };

        this.mBluetoothManager.onStateUpdate = (evnt, param1, param2, param3) => {
            this.fireEvent('StateUpdate', evnt, param1, param2, param3);
        };

        this.mPPPoEConfig.onPPPoEStatusChanged = (state) => {
            this.fireEvent('pppoeLinkStatus', state);
        };
        this.mWifiConfig.onAccessPointUpdated = (state, ap) => {
            var waveStrength, secureAttr, isConnected, apName, apInfo = {}, result : string= '';

            switch (state) {
                case eNET_STATE.WIFI_SCAN_AP:
                    this.mOriginApList.push(ap);
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
                        secure : secureAttr,
                        apname : apName,
                        strength : waveStrength,
                        isConnected : isConnected
                    };
                    this.mApCacheList.push(apInfo);
                    break;
                case eNET_STATE.WIFI_SCAN_SUCCESS:
                    this.fireEvent('eventWifiScanningResult', {
                        apList : this.mApCacheList
                    });
                    break;
                case eNET_STATE.WIFI_SCAN_FAIL :
                    this.mApCacheList.length = 0;
                    this.fireEvent('eventWifiScanningResult', {
                        apList : this.mApCacheList
                    });
                    break;
                case eNET_STATE.WIFI_CON_SUCCESS:
                    this.mNetworkManager.updateDevWireless((aDev) => {
                        aDev.dhcpOn = true;
                        aDev.dnsAuto = true;
                        return true;
                    })
                    this.mConnectionState = 'wifi';
                    result = 'success';
                    this.fireEvent('applyConnectToAP', result);
                    this.updateNetworkAvailabilty();
                    break;
                case eNET_STATE.WIFI_CON_FAIL:
                    result = 'fail';
                    this.fireEvent('applyConnectToAP', result);
                    this.updateNetworkAvailabilty();
                    break;
                case eNET_STATE.WIFI_KEY_INVALID:
                    result = 'invalid';
                    this.fireEvent('applyConnectToAP', result);
                    break;
                default:
                    result = 'fail';
                    this.fireEvent('applyConnectToAP', result);
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
                startHour : retServerActivation.startHour,
                startMin : retServerActivation.startMin,
                endHour : retServerActivation.endHour,
                endMin : retServerActivation.endMin,
                durationMin : retServerActivation.durationMin,
                isOn : retServerActivation.isOn
            });

            this.mConfigurationOipfObj.setServerActivationTime(retServerActivation.startHour, retServerActivation.startMin, retServerActivation.durationMin, false);
        }
        this.mInternetAvailable = false;
        this.mNetworkAvailable = this.isNetworkConnected();
        if (this.mNetworkAvailable) {
            this.checkInternetConnection();
        }
    }
    private updateNetworkAvailabilty() {
        var state = this.isNetworkConnected();
        if (this.mNetworkAvailable !== state) {
            this.mNetworkAvailable = state;
            hx.log("SettingUtil", "[this.updateNetworkAvailabilty()] - trigger (NetworkAvailableChanged) event,  this.mNetworkAvailable : " + this.mNetworkAvailable);
            this.fireEvent('NetworkAvailableChanged', this.mNetworkAvailable);
        }
        if (!state) {
            this.mInternetAvailable = false;
        }
    }
    private isNetworkConnected(): boolean {
        var devWired = this.mNetworkManager.getDevWired();
        var devWireless = this.mNetworkManager.getDevWireless();
        var ret = false;
        if (devWired && devWired.enabled) {
            ret = true;
        } else if (devWireless && devWireless.enabled) {
            var apInfo = this.mWifiConfig.getConnectedAPInfo();
            if (apInfo.isConnected) {
                ret = true;
            }
        }
        return ret;
    }

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

    getSystemInfo(): ifSystemInfo {
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
            hx.log("info","NETFLIX ~~~~~~~~~~~~ esn ::" + netflix_ESN);
        }
        // do not have to convert model name cos it has been done by MW(OBAMA)
        return {
            modelName : this.mLocalSystemOipfObj.modelName,
            softwareVersion : this.mLocalSystemOipfObj.softwareVersion,
            loaderVersion : this.mLocalSystemOipfObj.getField('loader_version'),
            ciplusVersion : this.mLocalSystemOipfObj.getField('ciplus_version'),
            system_id : this.mLocalSystemOipfObj.getField('system_id'),
            update_date : this.mLocalSystemOipfObj.getField('update_date'),
            micom_version : this.mLocalSystemOipfObj.getField('micom_version'),
            serialNumber : this.mLocalSystemOipfObj.serialNumber,
            chipId : this.mLocalSystemOipfObj.chipId,
            wiredMacAddress : mWireDevmacAddress,
            wirelessMacAddress : mWirelessDevmacAddress,
            bluetoothMacAddress : this.mBluetoothManager.getMacAddress(),
            esn : netflix_ESN
        };
    }

    getHiddenInfo() {
        var i, length, fusingInfo;
        var scs = 0;
        var jtag = 0;
        var otp = 0;
        var cwe = 0;
        var temp;
        var modelName = ENV.model;

        switch(modelName) {
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
        for ( i = 0; i < length; i += 1) {
            if (fusingInfo[i].NAME === 'SCS') {
                scs = fusingInfo[i].VALUE;
            } else if (fusingInfo[i].NAME === 'JTAG') {
                jtag = fusingInfo[i].VALUE;
            } else if (fusingInfo[i].NAME === 'OTP') {
                otp = fusingInfo[i].VALUE;
            } else if (fusingInfo[i].NAME === 'CWE') {
                cwe = fusingInfo[i].VALUE;
            }
        }
        return {
            serialNumber : this.mLocalSystemOipfObj.serialNumber,
            language : this.mConfigurationOipfObj.preferredMenuLanguage,
            scs : scs,
            jtag : jtag,
            otp : otp,
            cwe : cwe,
            nagraCasn : this.mLocalSystemOipfObj.getField('nagra_casn')
        };
    }
    getNandBadBlocks() {
        var i, length, badblocks;
        var loader = 0;
        var ubi = 0;
        var ubifs = 0;
        var temp: any = $.parseJSON(this.mLocalSystemOipfObj.getField('nand_badblocks'));
        if (temp) {
            badblocks = temp.NANDBADBLOCKS;
        }
        length = badblocks.length;
        for ( i = 0; i < length; i += 1) {
            if (badblocks[i].NAME === 'LOADER') {
                loader = badblocks[i].VALUE;
            } else if (badblocks[i].NAME === 'UBI') {
                ubi = badblocks[i].VALUE;
            } else if (badblocks[i].NAME === 'UBIFS') {
                ubifs = badblocks[i].VALUE;
            }
        }
        return {
            'loader' : loader,
            'ubi' : ubi,
            'ubifs' : ubifs
        };
    }
    getBoxFirstBootStatus(): boolean {
        return this.mLocalSystemOipfObj.boxFirstBootStatus;
    }
    setBoxFirstBootStatus(value: boolean) {
        this.mLocalSystemOipfObj.boxFirstBootStatus = value;
    }
    setAudioVolume(value) {
        value = 5 * value;
        /* (100(max) - 0(min)) / 20(volume step) * value */

        this.mLocalSystemOipfObj.volume = value;
    }
    getAudioInfo(param) {
        var cb = param.cb;
        var retObj: any = {};
        var i, len;
        var oipfAudios = this.mLocalSystemOipfObj.hmx_audios;
        var oipfOutput = this.mLocalSystemOipfObj.hmx_outputs[0];

        len = oipfAudios.length;

        for ( i = 0; i < len; i += 1) {
            if (oipfAudios[i].name === "audio_hdmi") {
                retObj.audioModeHdmi = oipfAudios[i].modeDigitalAudioOutput;
                retObj.audioType = oipfAudios[i].soundMode;
                retObj.lipSyncDelay = oipfAudios[i].lipSyncDelay;
            } else if (oipfAudios[i].name === "audio_spdif") {
                retObj.audioModePcm = oipfAudios[i].modeDigitalAudioOutput;
            } else if (oipfAudios[i].name === "audio_" + i) {
                retObj.audioModeHdmi = oipfAudios[i].modeDigitalAudioOutput;
                retObj.audioType = oipfAudios[i].soundMode;
                retObj.lipSyncDelay = oipfAudios[i].lipSyncDelay;
            }
        }

        retObj.audioDescription = oipfOutput.audioDescriptionEnabled;

        if (cb) {
            setTimeout(function() {
                cb(retObj);
            }, 1);
        }

        return retObj;
    }
    setAudioInfo(param) {
        var retObj = {}, //
            i, len, //
            oipfAudios = this.mLocalSystemOipfObj.hmx_audios, //
            oipfOutput = this.mLocalSystemOipfObj.hmx_outputs[0];

        hx.log("SettingUtil", 'setAudioInfo - audioDescriptionEnabled : ' + param.audioDescription);
        oipfOutput.audioDescriptionEnabled = param.audioDescription;

        len = oipfAudios.length;
        for ( i = 0; i < len; i += 1) {
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
            } else if (oipfAudios[i].name === "audio_spdif") {
                if (param.audioModePcm) {
                    hx.log("SettingUtil", 'setAudioInfo - PCM modeDigitalAudioOutput : ' + param.audioModePcm);
                    oipfAudios[i].modeDigitalAudioOutput = param.audioModePcm;

                }
            }
        }
    }
    getPictureInfo() {
        var supportedResolutions = [];
        var i, len, curResolution;
        var output = this.mLocalSystemOipfObj.hmx_outputs[0];
        len = output.supportedResolutions.length;
        for ( i = 0; i < len; i += 1) {
            supportedResolutions[i] = output.supportedResolutions.item(i);
        }
        curResolution = output.curResolution;

        return {
            tvAspectRatio : output.tvAspectRatio,
            videoDisplayFormat : output.videoDisplayFormat,
            curResolution : curResolution,
            supportedResolutions : supportedResolutions
        };
    }
    setPictureInfo(data) {
        var output = this.mLocalSystemOipfObj.hmx_outputs[0];

        hx.log("SettingUtil", 'setPictureInfo - data : ' + data);
        for (var prop in data) {
            hx.log("SettingUtil", 'setPictureInfo - [' + prop + ' : ' + data[prop] + ']');
            if (data[prop]) {
                output[prop] = data[prop];
            }
        }
    }
    getOutputOption() {
        var priorityOutputs = this.mLocalSystemOipfObj.priorityOutputs;
        hx.log("SettingUtil", 'getOutputOption - priorityOutputs : ' + priorityOutputs);
        return priorityOutputs;
    }
    setOutputOption(data) {
        hx.log("SettingUtil", 'setOutputOption - priorityOutputs : ' + data);
        this.mLocalSystemOipfObj.priorityOutputs = data;
    }
    getLanguage() {
        var retObj: any = {};

        if (undefined === this.cache_langMenu) {
            this.cache_langMenu = this.mConfigurationOipfObj.preferredMenuLanguage;
            retObj.LangMenu = this.cache_langMenu;
        } else {
            retObj.LangMenu = this.cache_langMenu;
        }
        if (undefined === this.cache_langAudio) {
            this.cache_langAudio = this.mConfigurationOipfObj.preferredAudioLanguage;
            retObj.LangAudio = this.cache_langAudio;
        } else {
            retObj.LangAudio = this.cache_langAudio;
        }
        if (undefined === this.cache_langSubTtl) {
            this.cache_langSubTtl = this.mConfigurationOipfObj.preferredSubtitleLanguage;
            retObj.LangSubTtl = this.cache_langSubTtl;
        } else {
            retObj.LangSubTtl = this.cache_langSubTtl;
        }
        //hx.log("SettingUtil", 'getLanguage - [MenuLang : ' + retObj.LangMenu + '] [SubTtlLang : ' + retObj.LangSubTtl + '] [AudioLang : ' + retObj.LangAudio + ']');
        return retObj;
    }
    setLanguage(values) {
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
    }
    controlNetServer(bOnOff) {
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
        } else {
            if (this.mDmsObj.started) {
                hx.log('conNetServer', 'DMS stop');
                this.mDmsObj.stop();
            }
            if (dmrIsStarted) {
                hx.log('conNetServer', 'DMR stop');
                dsrvDmr.stop();
            }
            //if (this.mDialServer.started) {
            //    hx.log('conNetServer', 'Dial stop');
            //    this.mDialServer.stop();
            //}
        }
    }
    IsNetServerWork() {
        return this.mDmsObj.started;
    }
    getPvrEndPadding() {
        var pvrEndPadding;
        if (undefined === this.cache_endPadding) {
            pvrEndPadding = this.mConfigurationOipfObj.pvrEndPadding;
            this.cache_endPadding = pvrEndPadding;
        } else {
            pvrEndPadding = this.cache_endPadding;
        }
        hx.log("SettingUtil", 'fnGetPvrEndPadding() - pvrEndPadding : ' + pvrEndPadding);
        return pvrEndPadding;
    }
    setPvrEndPadding(value) {
        this.cache_endPadding = value;
        this.mConfigurationOipfObj.pvrEndPadding = this.cache_endPadding;
        hx.log("SettingUtil", '() - this.cache_endPadding : ' + this.cache_endPadding);
    }
    getPvrStartPadding() {
        var pvrStartPadding = this.mConfigurationOipfObj.pvrStartPadding;
        if (undefined === this.cache_startPadding) {
            pvrStartPadding = this.mConfigurationOipfObj.pvrStartPadding;
            this.cache_startPadding = pvrStartPadding;
        } else {
            pvrStartPadding = this.cache_startPadding;
        }
        hx.log("SettingUtil", 'fnGetPvrStartPadding() - pvrStartPadding : ' + pvrStartPadding);
        return pvrStartPadding;
    }
    setPvrStartPadding(value) {
        this.cache_startPadding = value;
        this.mConfigurationOipfObj.pvrStartPadding = this.cache_startPadding;
        hx.log("SettingUtil", 'fnSetPvrStartPadding() - this.cache_startPadding : ' + this.cache_startPadding);
    }
    getInstantReplayTime() {
        var instantReplayTime;

        if (undefined === this.cache_instantReplayTime) {
            instantReplayTime = this.mConfigurationOipfObj.instantReplayTime || 15;
            this.cache_instantReplayTime = instantReplayTime;
        } else {
            instantReplayTime = this.cache_instantReplayTime;
        }
        hx.log("SettingUtil", 'fnGetInstantReplayTime() - instantReplayTime : ' + instantReplayTime);

        return instantReplayTime;
    }
    setInstantReplayTime(value) {
        this.cache_instantReplayTime = value;
        this.mConfigurationOipfObj.instantReplayTime = this.cache_instantReplayTime;
        hx.log("SettingUtil", 'fnSetInstantReplayTime() - this.cache_instantReplayTime : ' + this.cache_instantReplayTime);

        var instantReplayTime = this.getInstantReplayTime();
        this.fireEvent('PlaybackOptionChange', {
            state : 'ReplayTime',
            time : instantReplayTime
        });
    }
    getSkipForwardTime() {
        var skipForwardTime;

        if (undefined === this.cache_instantSkipTime) {
            skipForwardTime = this.mConfigurationOipfObj.skipForwardTime || 120;
            this.cache_instantSkipTime = skipForwardTime;
        } else {
            skipForwardTime = this.cache_instantSkipTime;
        }

        hx.log("SettingUtil", 'fnGetSkipForwardTime() - value : ' + skipForwardTime);
        return skipForwardTime;
    }
    setSkipForwardTime(value) {
        this.cache_instantSkipTime = value;
        this.mConfigurationOipfObj.skipForwardTime = this.cache_instantSkipTime;
        hx.log("SettingUtil", 'fnSetSkipForwardTime() - this.cache_instantSkipTime : ' + this.cache_instantSkipTime);
    }
    InitializeLanguage(cb) {
        var LangObj = this.getLanguage();
        hx.l.load(LangObj.LangMenu, function() {
            dateFormat.set(undefined, [hx.l('LOC_SUN_ID'), hx.l('LOC_MON_ID'), //
                    hx.l('LOC_TUE_ID'), hx.l('LOC_WED_ID'), hx.l('LOC_THU_ID'), //
                    hx.l('LOC_FRI_ID'), hx.l('LOC_SAT_ID'), hx.l('LOC_SUNDAY_ID'), //
                    hx.l('LOC_MONDAY_ID'), hx.l('LOC_TUESDAY_ID'), hx.l('LOC_WEDNESDAY_ID'), //
                    hx.l('LOC_THURSDAY_ID'), hx.l('LOC_FRIDAY_ID'), hx.l('LOC_SATURDAY_ID')], //
                [hx.l('LOC_JAN_ID'), hx.l('LOC_FEB_ID'), hx.l('LOC_MAR_ID'), //
                    hx.l('LOC_APR_ID'), hx.l('LOC_MAY_01_ID'), hx.l('LOC_JUN_ID'), //
                    hx.l('LOC_JUL_ID'), hx.l('LOC_AUG_ID'), hx.l('LOC_SEP_ID'), //
                    hx.l('LOC_OCT_ID'), hx.l('LOC_NOV_ID'), hx.l('LOC_DEC_ID'), //
                    hx.l('LOC_JANUARY_ID'), hx.l('LOC_FEBRUARY_ID'), hx.l('LOC_MARCH_ID'), //
                    hx.l('LOC_APRIL_ID'), hx.l('LOC_MAY_ID'), hx.l('LOC_JUNE_ID'), //
                    hx.l('LOC_JULY_ID'), hx.l('LOC_AUGUST_ID'), hx.l('LOC_SEPTEMBER_ID'), //
                    hx.l('LOC_OCTOBER_ID'), hx.l('LOC_NOVEMBER_ID'), hx.l('LOC_DECEMBER_ID')]);
            cb();
        });
    }
    getEnableSeriesPlayback() {
        var retObj;
        switch(this.mConfigurationOipfObj.getField('enableSeriesPlay')) {
            case 'false':
                retObj = false;
                break;
            case 'true':
                retObj = true;
                break;
        }
        hx.log("SettingUtil", 'getEnableSeriesPlayback - retObj : ' + retObj);
        return retObj;
    }
    setEnableSeriesPlayback(param) {
        hx.log("SettingUtil", 'setEnableSeriesPlayback - param : ' + param);
        return this.mConfigurationOipfObj.setField('enableSeriesPlay', param ? "true" : "false");
    }
    getEnableIPEPG() {
        var retObj;
        switch(this.mConfigurationOipfObj.getField('enableIPEPG')) {
            case 'false':
                retObj = false;
                break;
            case 'true':
                retObj = true;
                break;
        }
        hx.log("SettingUtil", 'getEnableIPEPG - retObj : ' + retObj);
        return retObj;
    }
    getIpepgLastUpdateTime() {
        var retObj, value;

        value = this.mConfigurationOipfObj.getField('ipepgLastUpdateTime');

        retObj = Number(value);

        hx.log("SettingUtil", 'getIpepgLastUpdateTime - retObj : ' + retObj);

        return retObj;
    }
    getIpepgDownloadProgress() {
        var retObj, value;
        value = this.mConfigurationOipfObj.getField('ipepgDownloadProgress');
        retObj = Number(value);
        hx.log("SettingUtil", 'fnGetIpepgDownloadProgress - retObj : ' + retObj);
        return retObj;
    }
    getTvtvEpgLastUpdateTime() {
        var retObj, value;

        value = this.mConfigurationOipfObj.getField('tvtvEpgLastUpdateTime');

        retObj = Number(value);

        hx.log("SettingUtil", 'getTvtvEpgLastUpdateTime - retObj : ' + retObj);

        return retObj;
    }
    setEnableIPEPG(param) {
        hx.log("SettingUtil", 'setEnableIPEPG - param : ' + param);
        return this.mConfigurationOipfObj.setField('enableIPEPG', param ? "true" : "false");
    }
    getTvtvUpdateTime() {
        var privateData = this.mAppMain.privateData, tvtvWakeupTime = null;
        if (privateData) {
            tvtvWakeupTime = privateData.tvtvWakeupTime;
        }
        return tvtvWakeupTime;
    }
    getIsTvtvStandbyEnable() {
        var privateData = this.mAppMain.privateData, isTvtvStandbyEnable = false;
        if (privateData) {
            isTvtvStandbyEnable = privateData.isTvtvStandbyEnable;
        }
        return isTvtvStandbyEnable;
    }
    setTvtvUpdateTime(param) {
        this.mAppMain.privateData.prepareWakeupTvtvUpdate(param.timeObj, param.isTvtvStandbyEnable);
    }
    getEnableRemoteRec() {
        var retObj;

        switch(this.mConfigurationOipfObj.getField('enableRemoteRecord')) {
            case 'false':
                retObj = false;
                break;
            case 'true':
                retObj = true;
                break;
        }

        hx.log("SettingUtil", 'getEnableRemoteRec - retObj : ' + retObj);

        return retObj;
    }
    setEnableRemoteRec(param) {
        var retObj;

        hx.log("SettingUtil", 'setEnableRemoteRec - param : ' + param);

        return ( retObj = this.mConfigurationOipfObj.setField('enableRemoteRecord', param ? "true" : "false"));
    }
    getEnableSatIpNotification() {
        var retObj;

        switch(this.mConfigurationOipfObj.getField('enableSatIpNotification')) {
            case 'false':
                retObj = false;
                break;
            case 'true':
                retObj = true;
                break;
        }

        hx.log("SettingUtil", 'GetEnableSatIpNotification - retObj : ' + retObj);

        return retObj;
    }
    setEnableSatIpNotification(param) {
        var retObj;

        hx.log("SettingUtil", 'SetEnableSatIpNotification - param : ' + param);

        return ( retObj = this.mConfigurationOipfObj.setField('enableSatIpNotification', param ? "true" : "false"));
    }
    getSatRecordUpdateInterval() {
        var value, retObj;

        retObj = (Number(this.mConfigurationOipfObj.getField('satRecordUpdateInterval')));

        hx.log("SettingUtil", 'getSatRecordUpdateInterval - retObj : ' + retObj);

        return retObj;
    }
    setSatRecordUpdateInterval(param) {
        var retObj;

        hx.log("SettingUtil", 'setSatRecordUpdateInterval - param : ' + param);

        return ( retObj = this.mConfigurationOipfObj.setField('satRecordUpdateInterval', param));
    }
    getSatRecordConfirmCode(param) {
        var retObj;

        retObj = this.mConfigurationOipfObj.getSatRecordConfirmCode(param.registrationCode);

        hx.log("SettingUtil", 'getSatRecordConfirmCode - retObj : ' + retObj);

        return retObj;
    }
    getSatRecordPriority() {
        var retObj;

        retObj = this.mConfigurationOipfObj.getField('satRecordPriority');

        hx.log("SettingUtil", 'getSatRecordPriority - retObj : ' + retObj);

        return (retObj === "true") ? 1 : 0;
    }
    setSatRecordPriority(param) {
        var retObj;

        hx.log("SettingUtil", 'setSatRecordPriority - param : ' + param);

        return this.mConfigurationOipfObj.setField('satRecordPriority', param);
    }
    getUpdatePeriodRemoteRec() {
        var values, retObj: any = {};

        values = this.mConfigurationOipfObj.getField('updatePeriodRemoteRecord').split(':');

        retObj.start_time = (Number(values[0]));
        retObj.end_time = (Number(values[1]));

        hx.log("SettingUtil", 'getUpdatePeriodRemoteRec - ' + retObj.start_time + ':' + retObj.end_time);

        return retObj;
    }
    setUpdatePeriodRemoteRec(param) {
        var retObj;

        hx.log("SettingUtil", 'setUpdatePeriodRemoteRec - param : ' + param);

        return ( retObj = this.mConfigurationOipfObj.setField('updatePeriodRemoteRecord', ((param.start_time) + ':' + (param.end_time))));
    }
    getUpdateIntervalRemoteRec() {
        var retObj;

        retObj = (Number(this.mConfigurationOipfObj.getField('updateIntervalRemoteRecord')) / 60);

        hx.log("SettingUtil", 'getUpdateIntervalRemoteRec - retObj : ' + retObj);

        return retObj;
    }
    setUpdateIntervalRemoteRec(param) {
        var retObj;

        hx.log("SettingUtil", 'setUpdateIntervalRemoteRec - param : ' + param);

        return this.mConfigurationOipfObj.setField('updateIntervalRemoteRecord', (param * 60).toString());
    }
    doFactoryDefault() {
        hx.log("SettingUtil", 'doFactoryDefault');
        hx.svc.WEB_STORAGE.deleteWholeItems();
        hx.svc.AV_CTRL.stopCurChannel({
            stopAV : 1,
            stopTsr : 1
        });
        this.mConfigurationOipfObj.doFactoryDefault('all');
    }
    getFirstTimeBoot() {
        var firstTimeBoot = this.mConfigurationOipfObj.firstTimeBoot;
        hx.log("SettingUtil", 'getFirstTimeBoot : ' + firstTimeBoot);
        return firstTimeBoot;
    }
    offFirstTimeBoot() {
        hx.log("SettingUtil", 'offFirstTimeBoot ');
        this.mConfigurationOipfObj.firstTimeBoot = false;
    }

    getHdmiCecMode() {
        var hdmiCecMode = this.mConfigurationOipfObj.hdmiCecMode;
        hx.log("SettingUtil", 'fnGetHdmiCecModes : ' + hdmiCecMode);
        return hdmiCecMode;
    }
    setHdmiCecMode(value) {
        hx.log("SettingUtil", 'fnSetHdmiCecMode : ' + value);
        this.mConfigurationOipfObj.hdmiCecMode = value;
    }
    getHdmiCecVolumeCtrlMode() {
        var hdmiCecVolumeCtrlMode = this.mConfigurationOipfObj.hdmiCecVolumeCtrlMode;
        hx.log("SettingUtil", 'fnGetHdmiCecVolumeCtrlMode : ' + hdmiCecVolumeCtrlMode);
        return hdmiCecVolumeCtrlMode;
    }
    setHdmiCecVolumeCtrlMode(value) {
        hx.log("SettingUtil", 'fnSetHdmiCecVolumeCtrlMode : ' + value);
        this.mConfigurationOipfObj.hdmiCecVolumeCtrlMode = value;
    }

    setPowerOff() {
        hx.log("SettingUtil", 'setPowerOff() from web application');
        this.mLocalSystemOipfObj.setPowerState(ePOWER_STATE.POWER_PASSIVE_STANDBY);
    }
    setPowerOn() {
        hx.log("SettingUtil", 'SetPowerOn() from web application');
        this.mLocalSystemOipfObj.setPowerState(ePOWER_STATE.POWER_ON);
    }
    standbyState(state) {
        hx.log("SettingUtil", 'standbyState. going to state mode..');
        this.mLocalSystemOipfObj.standbyState = state;
    }
    setStandbyPowerMode(mode) {
        hx.log("SettingUtil", 'setStandbyPowerMode() from web application');
        this.mConfigurationOipfObj.standbyPowerMode = mode;
        //['Normal', 'Power save']
    }
    getStandbyPowerMode() {
        hx.log("SettingUtil", 'getStandbyPowerMode() from web application');
        return this.mConfigurationOipfObj.standbyPowerMode;
    }
    getPowerOnInfo() {
        var privateData = this.mAppMain.privateData, retObj: any = false;
        if (privateData) {
            retObj = {
                channel : privateData.wakeupChannel,
                time : privateData.wakeupTime,
                volume : privateData.wakeupVolume,
                repeatDays : privateData.wakeupRepeatDays
            };
        }

        if (!retObj.channel) {
            retObj = false;
        }
        return retObj;
    }
    setPowerOnInfo(param) {
        hx.log("SettingUtil", '--------------------------------------------------------------');
        hx.log("SettingUtil", hx.util.format('time: {0}, repeatDays: {1} - {2}, channel: {3}, volume: {4}', param.timeObj.format('yyyy-mm-dd HH:MM:ss.L'), param.timeObj.getTime(), param.repeatDays, param.channel.ccid, param.volume));
        hx.log("SettingUtil", '--------------------------------------------------------------');
        this.mAppMain.privateData.prepareWakeupOITF(param.timeObj, param.repeatDays, param.channel, param.volume);
    }
    clearPowerOnInfo() {
        this.mAppMain.privateData.prepareWakeupOITF(null);
    }
    getNetConnectedInfo() {
        var retObj: any = {};
        var wireDevState : string = "Disconnected";
        var wirelessDevState : string = "Disconnected";
        var pppoeDevState : string = "Disconnected";
        var networkManager = this.mNetworkManager;
        var devWired = networkManager.getDevWired();
        var devWireless = networkManager.getDevWireless();
        var devPPPoE = networkManager.getDevPPPoE();
        var waveStrength : string = "-";

        if (devWired && devWired.enabled) {
            wireDevState = 'Connected';
        } else {
            wireDevState = 'Disconnected';
        }

        if (devWireless && devWireless.enabled) {
            var apInfo = this.mWifiConfig.getConnectedAPInfo();
            if (apInfo !== null) {
                wirelessDevState = 'Connected';
                var strength = apInfo.waveStrength;
                if (99 < strength) {
                    strength = 99;
                }else if(strength == 0 && apInfo.essid == ''){
                    wirelessDevState = 'Disconnected';
                }
                waveStrength = Math.floor(strength / 20).toString();

                var secureAttr : string = 'false';
                if (apInfo.secure || apInfo.authen || apInfo.encrypt) {
                    secureAttr = 'true';
                }

                retObj.wirelessDevInfo = {
                    secure : secureAttr,
                    apname : apInfo.essid,
                    strength : waveStrength,
                    isConnected : 1
                };
            } else {
                wirelessDevState = 'Disconnected';
            }
        } else {
            wirelessDevState = 'Disconnected';
        }
        if (devPPPoE && devPPPoE.enabled) {
            pppoeDevState = 'Connected';
        } else {
            pppoeDevState = 'Disconnected';
        }
        retObj.pppoeDevState = pppoeDevState;
        retObj.wireDevState = wireDevState;
        retObj.wirelessDevState = wirelessDevState;
        retObj.waveStrength = waveStrength;

        hx.log("SettingUtil", "[fnGetNetConnectedInfo()] - wireDevState : [" + wireDevState + "] waveStrength : [" + waveStrength + "]");

        return retObj;
    }
    stopGetSignalInfo() {
        if (this.mTimerSignalInform) {
            clearInterval(this.mTimerSignalInform);
            this.mTimerSignalInform = null;
            return true;
        }
        return false;
    }
    private _fnGetSignalInfo() {
        var signalInfo, //
            tunerList = this.mLocalSystemOipfObj.tuners, //
            i, //
            len = tunerList.length, //
            signal = [{
                strength : 0,
                quality : 0,
                tuningParams : '',
                lock : false
            }, {
                strength : 0,
                quality : 0,
                tuningParams : '',
                lock : false
            }, {
                strength : 0,
                quality : 0,
                tuningParams : '',
                lock : false
            }, {
                strength : 0,
                quality : 0,
                tuningParams : '',
                lock : false
            }];

        if (tunerList) {
            for ( i = 0; i < len; i += 1) {
                signalInfo = tunerList.item(i).signalInfo;
                if (signalInfo.lock) {
                    signal[i].strength = signalInfo.strength;
                    signal[i].quality = signalInfo.quality;
                    signal[i].tuningParams = signalInfo.tuningParams;
                    signal[i].lock = true;
                } else {
                    signal[i].strength = 0;
                    signal[i].quality = 0;
                    signal[i].tuningParams = signalInfo.tuningParams;
                    signal[i].lock = false;
                }
            }
        }
        return signal;
    }
    getSignalInfo() {
        var CheckSignalInfoTimer = () => {
            var retObj;
            retObj = this._fnGetSignalInfo();

            this.fireEvent('NotiSignalInfo', {
                signal : retObj
            });
        };

        setTimeout(CheckSignalInfoTimer, 0);
        this.mTimerSignalInform = setInterval(CheckSignalInfoTimer, 2000);
    }
    getSignalConnectionType() {
        var antInfoList = this.mLocalSystemOipfObj.antennaInfoLists, connectionType;

        hx.log("SettingUtil", 'getSignalConnectionType');
        connectionType = antInfoList.getConnectionType();

        return connectionType;
    }
    setInfoDisplayTimeout(param) {

        this.cache_infoDisplayTimeout = param.timeout;
        this.mConfigurationOipfObj.infoDisplayTimeout = this.cache_infoDisplayTimeout;

        hx.log("SettingUtil", 'fnSetInfoDisplayTimeout() this.cache_infoDisplayTimeout : ' + this.cache_infoDisplayTimeout);
    }
    getInfoDisplayTimeout() {
        var UiDisplayTimeOut;

        if (undefined === this.cache_infoDisplayTimeout) {
            UiDisplayTimeOut = this.mConfigurationOipfObj.infoDisplayTimeout;
            this.cache_infoDisplayTimeout = UiDisplayTimeOut;
        } else {
            UiDisplayTimeOut = this.cache_infoDisplayTimeout;
        }

        hx.log("SettingUtil", 'getInfoDisplayTimeout() UiDisplayTimeOut : ' + UiDisplayTimeOut);

        return UiDisplayTimeOut;
    }
    setSubtitleFont(aValue: number) {
        hx.log("SettingUtil", 'setSubtitleFont() from web application');
//        if (param.subtitleFont === 'variable') {
//            this.mConfigurationOipfObj.subtitleFontType = this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_VARIABLE;
//        } else if (param.subtitleFont === 'fixed') {
//            this.mConfigurationOipfObj.subtitleFontType = this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_FIXED;
//        }

        if (!hx.emulMode && !(aValue in [this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_VARIABLE, this.mConfigurationOipfObj.subtitleFontType = this.mConfigurationOipfObj.TYPE_SUBTITLE_FONT_FIXED])) {
            throw 'undefined value input'
        }
        this.mConfigurationOipfObj.subtitleFontType = aValue;
    }
    getSubtitleFont() {
        hx.log("SettingUtil", 'getSubtitleFont() from web application');
        return this.mConfigurationOipfObj.subtitleFontType;
    }
    setSubtitleEnabled(aParam: boolean) {
        var outputs = this.mLocalSystemOipfObj.hmx_outputs;
        for (var i = 0; i < outputs.length; i++) {
            outputs[i].subtitleEnabled = aParam;
        }
    }
    getSubtitleEnabled(): boolean {
        return this.mLocalSystemOipfObj.hmx_outputs[0].subtitleEnabled;
    }
    setHardofHearing(aParam: boolean) {
        var outputs = this.mLocalSystemOipfObj.hmx_outputs;
        for (var i = 0; i < outputs.length; i++) {
            outputs[i].hardOfHearingEnabled = aParam;
        }
    }
    getHardofHearing(): boolean {
        return this.mLocalSystemOipfObj.hmx_outputs[0].hardOfHearingEnabled;
    }
    renameServer(param) {
        this.mDmsObj.friendlyName = param.serverName;
        this.mDmrObj.friendlyName = param.serverName;
        this.mDialServer.friendlyName = param.serverName;
    }
    getServerName() {
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
    }
    testConnection() {
        //this.mWoonServer.testConnection();
    }
    setFrontPanelString(param) {
        var text = param.text;
        var bStr = (text === '') ? true : false;
        var i = 0, chgText, bfind = false, subValue, iValue;

        hx.log("SettingUtil", 'fnSetFrontPanelString : ' + param.text);
        if (hx.config.frontPanel) {
            if (bStr === false) {
                for ( i = 0; i < hx.config.frontPanel.lcdVfd16.length; i += 1) {
                    if (text === hx.config.frontPanel.lcdVfd16[i]) {
                        if (hx.config.usefrontPanel === 'segment7') {
                            chgText = hx.config.frontPanel.segment7[i];
                            text = chgText;
                        } else if (hx.config.usefrontPanel === 'lcdVfd10') {
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
    }
    setFrontPanelLed(param) {
        var ledId = param.ledId, bEnable = param.bEnable, bClear = (param.bClear === undefined) ? false : param.bClear;
        this.mLocalSystemOipfObj.frontPanel.setLed(ledId, bEnable, false, bClear);
        //ledId, bOnOff, bAuto, bAllClear
    }
    getHbbTvUrl() {
        return this.mLocalSystemOipfObj.getField('url_hbbtv');
    }
    getRpUrl() {
        return this.mLocalSystemOipfObj.getField('url_rp');
    }
    getIpOtaUrl() {
        return this.mLocalSystemOipfObj.getField('url_ipota');
    }
    getBluetoothStatus() {
        return this.mBluetoothManager.enabled;
    }
    setBluetoothStatus(param) {
        this.mBluetoothManager.enabled = param.turnOnOff;
    }
    bluetoothStartScan() {
        this.mBluetoothManager.startScan();
    }
    getBluetoothDevList() {
        return this.mBluetoothManager.getDeviceList();
    }
    bluetoothConnect(param) {
        this.mBluetoothManager.connect(param.addr);
    }
    bluetoothDisconnect(param) {
        this.mBluetoothManager.disconnect(param.addr);
    }
    bluetoothSendPincode(param) {
        this.mBluetoothManager.sendPincode(param.addr, param.pincode);
    }
    bluetoothRemove(param) {
        this.mBluetoothManager.remove(param.addr);
    }
    getNetworkAvailable() {
        this.updateNetworkAvailabilty();
        hx.log("SettingUtil", 'getNetworkAvailable : ' + this.mNetworkAvailable);
        return this.mNetworkAvailable;
    }
    getInternetAvailable() {
        if (hx.emulMode) {
            return true;
        }
        this.updateNetworkAvailabilty();
        hx.log("SettingUtil", 'getInternetAvailable : ' + this.mInternetAvailable);
        return this.mInternetAvailable;
    }
    setIpAddress(data) {
        this.mIpAddress = data.address;
    }
    setNetmask(data) {
        this.mNetmask = data.address;
    }
    setGateway(data) {
        this.mGateway = data.address;
    }
    setDns1st(data) {
        this.mDns1st = data.address;
    }
    setDns2nd(data) {
        this.mDns2nd = data.address;
    }
    getIpAddress() {
        return this.mIpAddress;
    }
    getNetmask() {
        return this.mNetmask;
    }
    getGateway() {
        return this.mGateway;
    }
    getDns1st() {
        return this.mDns1st;
    }
    getDns2nd() {
        return this.mDns2nd;
    }
    getallAddress() {
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
        var address: any = {};
        address.type = netDev.dhcpOn ? hx.l('LOC_DHCP_ID') : hx.l('LOC_MANUAL_ID');
        this.mIpAddress = address.ipAddress = netDev.ipAddress;
        this.mNetmask = address.netmask = netDev.netmask;
        this.mGateway = address.gateway = netDev.gateway;
        this.mDns1st = address.dns1st = netDev.dns1st;
        this.mDns2nd = address.dns2nd = netDev.dns2nd;
        hx.log("SettingUtil", "[fnGetallAddress()] - ipAddress : " + address.ipAddress);
        return address;
    }
    apScanning() {
        this.mApCacheList = [];
        this.mOriginApList = [];
        this.mWifiConfig.apScanning();
    }
    ConnectionTry() {
        var networkManager = this.mNetworkManager;
        var devWired = networkManager.getDevWired();

        if (!devWired || devWired.connected == false) {
            hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev is not exist");
            this.fireEvent('tryConnection', "fail");
            return;
        }
        hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev.applyConfigure(); call");
        networkManager.updateDevWired((aDev) => {
            aDev.dhcpOn = true;
            aDev.dnsAuto = true;
            return true;
        })
    }
    stopConnecting() {
        var ret = false;
        var devWired = this.mNetworkManager.getDevWired();

        if (!devWired) {
            hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev is not exist");
        } else {
            hx.log("SettingUtil", "[fnConnectionTry()] - this.mWireDev.stopConnecting(); call");
            devWired.stopConnecting();
            ret = true;
        }
        return ret;
    }
    stopApScanning() {
        this.mApCacheList = [];
        this.mOriginApList = [];
        this.mWifiConfig.stopApScanning();
    }
    getPPPoEId() {
        return this.mPPPoEConfig.getID();
    }
    getPPPoEPassword() {
        return this.mPPPoEConfig.getPassword();
    }
    setPPPoEId(param) {
        this.mPPPoEConfig.setID(param.newid);
    }
    setPPPoEPassword(param) {
        this.mPPPoEConfig.setPassword(param.newpw);
    }
    connectPPPoE() {
        this.mPPPoEConfig.connect();
    }
    disConnectPPPoE() {
        this.mPPPoEConfig.disconnect();
    }
    wirelessStopConnecting() {
        var ret = false;
        var devWireless = this.mNetworkManager.getDevWireless();
        if (devWireless) {
            this.fireEvent('wirelessStopConnect');
            devWireless.stopConnecting();
            ret = true;
        }
        return ret;
    }
    manualConfig() {
        var networkManager = this.mNetworkManager;
        var devWired = networkManager.getDevWired();
        if (!devWired || devWired.connected == false) {
            hx.log("SettingUtil", "[fnManualConfig()] - this.mWireDev is not exist");
            this.fireEvent('tryConnection', "fail");
            return;
        }

        networkManager.updateDevWired((aDev) => {
            devWired.dhcpOn = false;
            devWired.dnsAuto = false;
            devWired.ipAddress = this.mIpAddress;
            devWired.netmask = this.mNetmask;
            devWired.gateway = this.mGateway;
            devWired.dns1st = this.mDns1st;
            devWired.dns2nd = this.mDns2nd;
            return true;
        });
        hx.log("SettingUtil", "[fnManualConfig()] - this.mWireDev.applyConfigure(); called!!!");
    }
    applyConnectToAP(param) {
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
        } else if (param.password) {
            this.mWifiConfig.applyConnectToAP(ap, param.password);
        } else {
            this.mWifiConfig.applyConnectToAP(ap, " ");
        }
    }
    checkWireConnection() {
        var networkManager = this.mNetworkManager;
        var devWired = networkManager.getDevWired();

        if (!devWired) {
            hx.log("SettingUtil", "[fnCheckWireConnection()] - this.mWireDev is not exist");
            return false;
        }
        hx.log("SettingUtil", "[fnCheckWireConnection()] - this.mWireDev.connected : " + devWired.connected);
        return devWired.connected;
    }

    checkWirelessConnection() {
        var devWireless = this.mNetworkManager.getDevWireless();
        hx.log('netInfo', '####### fnCheckWirelessConnection ######## mWirelessDev.enabled : ' + devWireless.enabled);
        if (devWireless && devWireless.enabled) {
            return true;
        }
        return false;
    }

    getConnectionState() {
        return this.mConnectionState;
    }
    doingKeyGrab(aGrab: boolean) {
        //hx.log("warning", "###############  [fnDoingKeyGrab]  ###############");
        if (aGrab) {
            this.mConfigurationOipfObj.setField("grabkey", "true");
            hx.log("debug", "###############  KEY GRAB = TRUE   ###############");
        } else {
            this.mConfigurationOipfObj.setField("ungrabkey", "true");
            hx.log("debug", "###############  KEY GRAB = FALSE  ###############");
        }
        //hx.log("warning", "###############  [fnDoingKeyGrab]  ###############");
    }
    getFrontPanelText() {
        return this.mPrevFrontPanelText;
    }
    getRCUCustomCode() {
        return {
            rcuCustomCode : this.mLocalSystemOipfObj.getField('rcu_customcode')
        };
    }
    getPowerStateValues() {
        return {
            curPowerState : this.mLocalSystemOipfObj.powerState,
            previousPowerState : this.mLocalSystemOipfObj.previousPowerState,
            timeCurrentPowerState : this.mLocalSystemOipfObj.timeCurrentPowerState,
            wakeupReason : this.mLocalSystemOipfObj.getField('wakeupReason')
        };
    }
    startPowerOffTimer() {
        // this working takes 2msec every 1min
        var secPOT = (60 * 1000);
        var triggered = () => {
            var time = new Date();
            var hour = time.getHours();
            var min = time.getMinutes();
            var res = hx.svc.WEB_STORAGE.getPowerOffTimer();
            if (res.hour !== null && res.min !== null && res.isOn && res.isOn === 'On') {
                if (parseInt(res.hour, 10) === hour && parseInt(res.min, 10) === min) {
                    this.fireEvent('powerOffTimerEvent', null);
                }
            }
        };
        this.mPowerOffTimerID = setInterval(triggered, secPOT);
    }
    stopPowerOffTimer() {
        clearInterval(this.mPowerOffTimerID);
    }
    startReminderPowerOff() {
        this.mReminderPowerOff = true;
        $('body').keydown((e) => {
            this.mReminderPowerOff = false;
        });
    }
    stopReminderPowerOff() {
        this.mReminderPowerOff = false;
    }
    getReminderPowerOff(): boolean {
        return this.mReminderPowerOff;
    }
    doReboot() {
        this.mConfigurationOipfObj.setField("doReboot", "true");
    }
    setApplicationReady() {
        this.mLocalSystemOipfObj.setApplicationReady();
    }
    checkDetachableStorage() {
        this.fireEvent('checkDetachableStorage');
    }
    setServerActivation(param) {
        var onoff;
        if (param.isOn === 'On') {
            onoff = true;
        } else {
            onoff = false;
        }
        if (param.startHour === '24' && param.startMin === '00') {
            param.startHour = '00';
        }
        this.mConfigurationOipfObj.setServerActivationTime(param.startHour, param.startMin, param.durationMin, onoff);
    }

    getIrdetoCasInfo() {
        return {
            clientinfo : this.mLocalSystemOipfObj.getField('irdetoclient_version')
        };
    }
    getpvrTsrOnOff() {
        var pvrTsrOnOff = this.mConfigurationOipfObj.timeShiftEnabled;
        hx.log('dsvc', 'getpvrTsrOnOff - value : ' + pvrTsrOnOff);
        return pvrTsrOnOff;
    }
    getDescrambleOnOff() {
        var DescrambleOnOff = this.mConfigurationOipfObj.descrambleOnOff;
        hx.log('dsvc', 'fnGetDescrambleOnOff - value : ' + DescrambleOnOff);
        return DescrambleOnOff;
    }
    setDescrambleOnOff(value) {
        hx.log('dsvc', 'fnSetDescrambleOnOff - value : ' + value);
        this.mConfigurationOipfObj.descrambleOnOff = value;
    }
    getAutoDeleteOnOff() {
        var AutoDeleteOnOff= this.mConfigurationOipfObj.autoDelete;
        hx.log('dsvc', 'fnGetAutoDeleteOnOff - value : ' + AutoDeleteOnOff);
        return AutoDeleteOnOff;
    }
    setAutoDeleteOnOff(value) {
        hx.log('dsvc', 'fnSetAutoDeleteOnOff - value : ' + value);
        this.mConfigurationOipfObj.autoDelete = value;
    }
    getNagraCasSn() {
        var casn = this.mLocalSystemOipfObj.getField('nagra_casn');
        hx.log('SettingUtil', "type :: " + typeof casn + " :: value : " + casn);
        return casn;
    }
    getNagraNuid() {
        var nuid = this.mLocalSystemOipfObj.getField('nagra_nuid');
        hx.log('SettingUtil', "type :: " + typeof nuid + " :: value : " + nuid);
        return nuid;
    }
    setpvrTsrOnOff(value) {
        this.mConfigurationOipfObj.timeShiftEnabled = value;
    }
    getoffsetGMT() {
        var retObj;

        retObj = Number(this.mConfigurationOipfObj.getField('offsetGMT'));
        hx.log('dsvc', 'fnGetoffsetGMT - retObj : ' + retObj);

        return retObj;
    }
    setoffsetGMT(param) {
        hx.log('dsvc', 'fnSetoffsetGMT - param : ' + param);
        return this.mConfigurationOipfObj.setField('offsetGMT', param.toString());
    }
    getdaylightsaving() {
        var retObj;

        switch(this.mConfigurationOipfObj.getField('daylightsaving')) {
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
    }
    setdaylightsaving(param) {
        hx.log('dsvc', 'fnSetdaylightsaving - param : ' + param);
        return this.mConfigurationOipfObj.setField('daylightsaving', param ? "true" : "false");
    }
    getVirtualSCW() {
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
    }
    setVirtualSCW(param) {
        hx.log('dsvc', 'fnSetVirtualSCW - param : ' + param);
        return this.mLocalSystemOipfObj.setField('irdeto_virtualscw', param ? "true" : "false");
    }
    getUpdateTime() {
        var retObj;

        switch(this.mConfigurationOipfObj.getField('timeupdateonalltp')) {
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
    }
    setUpdateTime(param) {
        hx.log('dsvc', 'fnSetUpdateTime - param : ' + param);
        return this.mConfigurationOipfObj.setField('timeupdateonalltp', param ? "true" : "false");
    }
    getIRFusinInfo() {
        return {
            serialNumber : this.mLocalSystemOipfObj.serialNumber,
            language : this.mConfigurationOipfObj.preferredMenuLanguage,
            jtag : this.mLocalSystemOipfObj.getField('irdeto_jtag'),
            scw : this.mLocalSystemOipfObj.getField('irdeto_cwe'),
            sboot : this.mLocalSystemOipfObj.getField('irdeto_sboot'),
            otp : this.mLocalSystemOipfObj.getField('irdeto_otp'),
            ir_mancode : this.mLocalSystemOipfObj.getField('irdeto_mancode'),
            ir_hwcode : this.mLocalSystemOipfObj.getField('irdeto_hwcode'),
            ir_variant : this.mLocalSystemOipfObj.getField('irdeto_variant'),
            ir_sysid : this.mLocalSystemOipfObj.getField('irdeto_sysid'),
            ir_sigver : this.mLocalSystemOipfObj.getField('irdeto_sigver'),
            ir_keyver : this.mLocalSystemOipfObj.getField('irdeto_keyver')
        };
    }
    getIRControlByte() {
        var strControlByte = this.mLocalSystemOipfObj.getField('irdeto_control_byte');
        return {
            numControlByte : Number(strControlByte)
        };
    }
    getRegionId() {
        var regionId = this.mConfigurationOipfObj.regionId;
        if (this.cache_regionId === undefined) {
            this.cache_regionId = regionId;
        }
        hx.logadd('SettingUtil');
        hx.log("SettingUtil", 'fnGetRegionId() - regionId : ' + regionId);
        return regionId;
    }
    setRegionId(value) {
        this.cache_regionId = value;
        this.mConfigurationOipfObj.regionId = this.cache_regionId;
        hx.logadd('SettingUtil');
        hx.log("SettingUtil", 'fnSetRegionId() - this.cache_regionId : ' + this.cache_regionId);
    }
    getCountryId() {
        var countryId = this.mConfigurationOipfObj.countryId;

        if (this.cache_countryId === undefined) {
            this.cache_countryId = countryId;
        }

        hx.log("SettingUtil", 'fnGetCountryId() - this.cache_countryId : ' + countryId);
        return countryId;
    }
    setCountryId(countryId) {
        this.cache_countryId = countryId;
        this.mConfigurationOipfObj.countryId = countryId;
        hx.log("SettingUtil", 'fnSetCountryId() - this.cache_countryId : ' + this.cache_countryId);
    }
    getEnableDynamicUpdate() {
        var retObj;
        retObj = this.mLocalSystemOipfObj.boxServiceUpdateFlag;
        hx.log("SettingUtil", 'fnGetEnableDynamicUpdate - retObj : ' + retObj);
        return retObj;
    }
    setEnableDynamicUpdate(value) {
        this.mLocalSystemOipfObj.boxServiceUpdateFlag = value;
        hx.log("SettingUtil", 'fnSetEnableDynamicUpdate - value : ' + value);
    }
    getEnableAntennaPower() {
        var retObj;
        retObj = this.mLocalSystemOipfObj.boxAntennaPowerStatus;
        hx.log("SettingUtil", 'fnGetEnableAntennaPower - retObj : ' + retObj);
        return retObj;
    }
    setEnableAntennaPower(value) {
        this.mLocalSystemOipfObj.boxAntennaPowerStatus = value;
        hx.log("SettingUtil", 'fnSetEnableAntennaPower - value : ' + value);
    }
    checkFileExistence(uri) {
        return this.mLocalSystemOipfObj.checkFileExistence(uri);
    }
    getPromotionVideo() {
        var countryId = hx.svc.SETTING_UTIL.getCountryId();
        var path : string = null;
        var fileName : string = null;
        var uri : string = null;
        var duration = 0;
        switch(countryId) {
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
            } else {
                uri = null;
            }
        }

        return {
            'uri' : uri,
            'title' : fileName,
            'duration' : duration
        };
    }
    getUpdatedList() {
        return {
            updatedList : this.mLocalSystemOipfObj.updatedList
        };
    }
    getEnableSambaServer() {
        var retObj = false;
        if (!this.mSambaServer) {
            return retObj;
        }
        retObj = this.mSambaServer.started;
        hx.log("SettingUtil", 'fnGetEnableSambaServer - retObj : ' + retObj);
        return retObj;
    }
    setEnableSambaServer(value?) {
        if (value) {
            this.mSambaServer.start();
        } else {
            this.mSambaServer.stop();
        }

        hx.log("SettingUtil", 'fnSetEnableSambaServer - value : ' + this.mSambaServer.started);
    }
    getEnableFtpServer() {
        var retObj = false;
        if (!this.mFtpServer) {
            return retObj;
        }
        retObj = this.mFtpServer.started;

        hx.log("SettingUtil", 'fnGetEnableFtpServer - retObj : ' + retObj);
        return retObj;
    }
    setEnableFtpServer(param) {
        if (param) {
            this.mFtpServer.start();
        } else {
            this.mFtpServer.stop();
        }

        hx.log("SettingUtil", 'fnSetEnableFtpServer - value : ' + this.mFtpServer.started);
    }
    getPasswordSet() {
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
    }
    setPasswordSet(value) {
        if (value) {
            this.mSambaServer.setPassword('admin', '', value);
            this.mFtpServer.setPassword('admin', '', value);
            this.mSambaServer.setPublicAccess('admin', false);
            this.mFtpServer.setPublicAccess('admin', false);
        } else {
            this.mSambaServer.setPublicAccess('admin', true);
            this.mFtpServer.setPublicAccess('admin', true);
        }

        hx.svc.WEB_STORAGE.setFileSharePasswd(value);

        hx.log("SettingUtil", 'fnSetPasswordSet - value : ' + value);
    }
    getEnableDownloadFolder(): boolean {
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
    }
    setEnableDownloadFolder(param) {
        if (param.enable) {
            this.mSambaServer.setShareFolder('admin', param.name, 'rw');
            this.mFtpServer.setShareFolder('admin', param.name, 'rw');
        } else {
            this.mSambaServer.setShareFolder('admin', param.name, 'ro');
            this.mFtpServer.setShareFolder('admin', param.name, 'ro');
        }

        hx.log("SettingUtil", 'fnSetEnableDownloadFolder - value : ' + param);
    }
    getColdBootDone(): boolean {
        if (!!this.mLocalSystemOipfObj) {
            if (!!this.mLocalSystemOipfObj.coldBootDone) {
                if (!!this.mLocalSystemOipfObj.coldBootDone.getColdBootDone) {
                    return this.mLocalSystemOipfObj.coldBootDone.getColdBootDone();
                }
            }
        }
    }
    showMsgbox() {
        if (!!this.mLocalSystemOipfObj) {
            var status = this.mLocalSystemOipfObj.getStatusOfTVBrowser();
            if (status) {
                this.mLocalSystemOipfObj.showMsgbox();
            }
        }

        return;
    }
    getNoOtaAction() {
        var retObj;
        retObj = this.mConfigurationOipfObj.getField('no_ota_action');
        hx.log("SettingUtil", 'fnGetNoOtaAction - retObj : ' + retObj);
        return (retObj === "true") ? true : false;
    }
    setNoOtaAction(param) {
        var retObj;
        hx.log("SettingUtil", 'fnSetNoOtaAction - param : ' + param);
        return this.mConfigurationOipfObj.setField('no_ota_action', param);
    }
    getNoOtaSWVer() {
        var retObj;
        retObj = this.mConfigurationOipfObj.getField('no_ota_swversion');
        hx.log("SettingUtil", 'fnGetNoOtaSWVer - retObj : ' + retObj);
        return retObj;
    }
    setNoOtaSWVer(param) {
        var retObj;
        hx.log("SettingUtil", 'fnSetNoOtaSWVer - param : ' + param);
        return this.mConfigurationOipfObj.setField('no_ota_swversion', param);
    }
    getLaterOtaCount() {
        var retObj;
        retObj = this.mConfigurationOipfObj.getField('later_ota_count');
        hx.log("SettingUtil", 'fnGetLaterOtaCount - retObj : ' + retObj);
        return Number(retObj);
    }
    setLaterOtaCount(param) {
        var retObj;
        hx.log("SettingUtil", 'fnSetLaterOtaCount - param : ' + param);
        return this.mConfigurationOipfObj.setField('later_ota_count', param);
    }
    getPrivacyPolicy() {
        var retObj, isEnable, mShareFolderCollection;

        retObj = hx.svc.WEB_STORAGE.getPrivacyPolicy();

        hx.log("SettingUtil", 'fnGetPrivacyPolicy - retObj : ' + retObj.state);

        if (retObj.state === 'false') {
            return false;
        }
        return true;
    }
    setPrivacyPolicy(value) {
        var privacyPolicyState = value;

        hx.svc.WEB_STORAGE.setPrivacyPolicyState(privacyPolicyState);
        this.fireEvent('privacyPolicyChange', privacyPolicyState);

        hx.log("SettingUtil", 'fnSetPrivacyPolicy - value : ' + privacyPolicyState);
    }
    getPrivacyPolicyVersion() {
        var retObj = this.mConfigurationOipfObj.getField('privacy_policy_ver');
        hx.log("SettingUtil", 'getPrivacyPolicyVersion - retObj : ' + retObj);
        return retObj;
    }
    setPrivacyPolicyVersion(aValue) {
        hx.log("SettingUtil", 'setPrivacyPolicyVersion - value : ' + aValue);
        this.mConfigurationOipfObj.setField('privacy_policy_ver', aValue || "disagree");
    }
    checkGatewayStatus() {
        if (hx.emulMode) {
            return;
        }
        var networkManager = this.mNetworkManager;
        var devWired = networkManager.getDevWired();
        var devWireless = networkManager.getDevWireless();
        var netDev = devWired;

        if(!devWired.connected && devWireless !== null && devWireless.enabled){
            netDev = devWireless;
        }
        return netDev.checkGatewayAccess();
    }
    checkDnsStatus() {
        if (hx.emulMode) {
            return;
        }
        var networkManager = this.mNetworkManager;
        var devWired = networkManager.getDevWired();
        var devWireless = networkManager.getDevWireless();
        var netDev = devWired;

        if(!devWired.connected && devWireless !== null && devWireless.enabled){
            netDev = devWireless;
        }
        return netDev.checkDnsAccess();
    }
    checkInternetConnection() {
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
    }
    setNtpServer(param): any {
        hx.log("SettingUtil", 'fnSetNtpServer - ntp_server_url : ' + param);
        return this.mLocalSystemOipfObj.setField('ntp_server_url', param);
    }
    setGuidancePolicy(aGuidancePolicy: number): void {
        this.mConfigurationOipfObj.guidancepolicy = aGuidancePolicy;
    }
    getGuidancePolicy(): number {
        return this.mConfigurationOipfObj.guidancepolicy;
    }
    setNCDNotifyData(aNeedToShowAgain: number): void {
        hx.log("SettingUtil", 'setNCDNotifyData - value : ' + aNeedToShowAgain);
        this.mOperatorFeature.setNcdNotifyData(aNeedToShowAgain);
    }
    getNCDNetworkMsg(): string {
        var ret = this.mOperatorFeature.getNcdNetworkMsg(0);
        hx.log("SettingUtil", 'getNCDNetworkMsg - : ' + ret);
        return ret;
    }
    isHbbTVActivated(): boolean {
        var ret = false;
        if (this.mLocalSystemOipfObj.isHbbTVActive !== undefined) {
            ret = !!this.mLocalSystemOipfObj.isHbbTVActive;
        }
        return ret;
    }
    getHbbTvEnable(): boolean {
        var bEnabled = this.mConfigurationOipfObj.hbbTvEnbled;
        hx.log("SettingUtil", 'fnGetHbbTvEnable - value : ' + bEnabled);
        return bEnabled;
    }
    getTVPortalUrl(): string {
        return this.mLocalSystemOipfObj.getField('url_portal');
    }
    getPrefixUrl() {
        if (new Date().getFullYear() < 2015) {
            return null;
        }
        return this.mLocalSystemOipfObj.getField("fvc_networkuriprefix");
    }
    getIpEpgSvcCount() {
        var retObj;
        retObj = this.mConfigurationOipfObj.getField('ipepgDownloadSvcCount');
        hx.log("SettingUtil", 'ipepgDownloadSvcCount - retObj : ' + retObj);

        return retObj;
    }
    getIpEpgEvtCount() {
        var retObj;
        retObj = this.mConfigurationOipfObj.getField('ipepgDownloadEvtCount');
        hx.log("SettingUtil", 'ipepgDownloadEvtCount - retObj : ' + retObj);

        return retObj;
    }
    getHdmiEdidInfo() {
        var val, retObj;

        if (!hx.emulMode) {
            val = $.parseJSON(this.mLocalSystemOipfObj.getField('hdmi_edidinfo'));
            retObj = val.HDMI_EDID_INFO;
        }

        return retObj;
    }
    getNetworkId(): number[] {
        hx.log("info", "[getNetworkId]: " + JSON.parse(this.mLocalSystemOipfObj.getField("network_ids")).NETWORK_ID);
        return JSON.parse(this.mLocalSystemOipfObj.getField("network_ids")).NETWORK_ID;
    }

    deleteCookies() {
        if(this.mLocalSystemOipfObj.deleteCookies){
            this.mLocalSystemOipfObj.deleteCookies();
        }
    }
    getInitHumaxTvPortal() {
        return (this.mConfigurationOipfObj.getField("init_humaxtvportal") == "true") ? true : false;
    }
    setInitHumaxTvPortal(aValue) {
        this.mConfigurationOipfObj.setField('init_humaxtvportal', aValue ? "true" : "false");
    }
    disconnectNetworkForTest() {
        this.mNetworkAvailable = false;
        this.mInternetAvailable = false;
        this.fireEvent('NetworkAvailableChanged', this.mNetworkAvailable);
        this.fireEvent('networkStatus', 'dns', 'fail');
    }
    connectNetworkForTest() {
        this.mNetworkAvailable = true;
        this.mInternetAvailable = true;
        this.fireEvent('NetworkAvailableChanged', this.mNetworkAvailable);
        this.fireEvent('InternetAvailableChanged', this.mInternetAvailable);
        this.fireEvent('networkStatus', 'dns', 'success');
    }
    isMute(): boolean {
        return this.mLocalSystemOipfObj.mute;
    }
}

export = cDLSettingUtil;
