/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilNetwork extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Network', '/Octopus/Appkit/Util/Network');
    }

    StartEvent(aCb: (msgInt: number, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, param1: number, param2: number, param3: number) {
                aCb(msgInt, param1, param2, param3);
            });
        });
    }

    BT_SetEnable(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.BT_SetEnable['finish'] = function() {
                aCb();
            };
            iface.BT_SetEnable(aEnable);
        });
    }

    BT_GetEnable(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.BT_GetEnable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.BT_GetEnable();
        });
    }

    BT_Scan(aCb: () => void) {
        this._call( function (iface){
            iface.BT_Scan['finish'] = function() {
                aCb();
            };
            iface.BT_Scan();
        });
    }

    BT_Connect(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.BT_Connect['finish'] = function() {
                aCb();
            };
            iface.BT_Connect(aIndex);
        });
    }

    BT_ConnectByBdAddr(aBdAddress: string, aCb: () => void) {
        this._call( function (iface){
            iface.BT_ConnectByBdAddr['finish'] = function() {
                aCb();
            };
            iface.BT_ConnectByBdAddr(aBdAddress);
        });
    }

    BT_Disconnect(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.BT_Disconnect['finish'] = function() {
                aCb();
            };
            iface.BT_Disconnect(aIndex);
        });
    }

    BT_DisconnectByBdAddr(aBdAddress: string, aCb: () => void) {
        this._call( function (iface){
            iface.BT_DisconnectByBdAddr['finish'] = function() {
                aCb();
            };
            iface.BT_DisconnectByBdAddr(aBdAddress);
        });
    }

    BT_Remove(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.BT_Remove['finish'] = function() {
                aCb();
            };
            iface.BT_Remove(aIndex);
        });
    }

    BT_RemoveByBdAddr(aBdAddress: string, aCb: () => void) {
        this._call( function (iface){
            iface.BT_RemoveByBdAddr['finish'] = function() {
                aCb();
            };
            iface.BT_RemoveByBdAddr(aBdAddress);
        });
    }

    BT_SendPinCode(aIndex: number, aPincode: number[], aCb: () => void) {
        this._call( function (iface){
            iface.BT_SendPinCode['finish'] = function() {
                aCb();
            };
            iface.BT_SendPinCode(aIndex, aPincode);
        });
    }

    BT_SendPinCodeByBdAddr(aBdAddress: string, aPincode: number[], aCb: () => void) {
        this._call( function (iface){
            iface.BT_SendPinCodeByBdAddr['finish'] = function() {
                aCb();
            };
            iface.BT_SendPinCodeByBdAddr(aBdAddress, aPincode);
        });
    }

    BT_GetDeviceNum(aCb: (aDeviceNum: number) => void) {
        this._call( function (iface){
            iface.BT_GetDeviceNum['finish'] = function(aDeviceNum: number) {
                aCb(aDeviceNum);
            };
            iface.BT_GetDeviceNum();
        });
    }

    BT_GetDeviceInfo(aDeviceId: number, aCb: (aDeviceInfo: tdata.TBTDeviceinfo) => void) {
        this._call( function (iface){
            iface.BT_GetDeviceInfo['finish'] = function(data) {
                aCb(tdata.convert_btdeviceinfo(data));
            };
            iface.BT_GetDeviceInfo(aDeviceId);
        });
    }

    BT_GetMacAddrInfo(aCb: (aMacAddress: string) => void) {
        this._call( function (iface){
            iface.BT_GetMacAddrInfo['finish'] = function(aMacAddress: string) {
                aCb(aMacAddress);
            };
            iface.BT_GetMacAddrInfo();
        });
    }

    BT_GetMacAddrInfoSize(aCb: (aSize: number) => void) {
        this._call( function (iface){
            iface.BT_GetMacAddrInfoSize['finish'] = function(aSize: number) {
                aCb(aSize);
            };
            iface.BT_GetMacAddrInfoSize();
        });
    }

    BT_GetDevInfoList(aListSize: number, aCb: (aDeviceInfoList: tdata.TBTDeviceinfo[]) => void) {
        this._call( function (iface){
            iface.BT_GetDevInfoList['finish'] = function(data) {
                var deviceInfoList = [];
                data.forEach((s) => {
                    deviceInfoList.push(tdata.convert_btdeviceinfo(s));
                });
                aCb(deviceInfoList);
            };
            iface.BT_GetDevInfoList(aListSize);
        });
    }

    CM_GetStatus(aCb: (aStatus: number) => void) {
        this._call( function (iface){
            iface.CM_GetStatus['finish'] = function(aStatus: number) {
                aCb(aStatus);
            };
            iface.CM_GetStatus();
        });
    }

    CM_GetDownStreamInfo(aDownInfo: number, aCb: (aValue: number) => void) {
        this._call( function (iface){
            iface.CM_GetDownStreamInfo['finish'] = function(aValue: number) {
                aCb(aValue);
            };
            iface.CM_GetDownStreamInfo(aDownInfo);
        });
    }

    CM_SetDownStreamInfo(aDownInfo: number, aValue: number, aCb: () => void) {
        this._call( function (iface){
            iface.CM_SetDownStreamInfo['finish'] = function() {
                aCb();
            };
            iface.CM_SetDownStreamInfo(aDownInfo, aValue);
        });
    }

    CM_GetUpStreamInfo(aUpInfo: number, aCb: (aValue: number) => void) {
        this._call( function (iface){
            iface.CM_GetUpStreamInfo['finish'] = function(aValue: number) {
                aCb(aValue);
            };
            iface.CM_GetUpStreamInfo(aUpInfo);
        });
    }

    DHCPSVR_UpdateInfo(aCb: () => void) {
        this._call( function (iface){
            iface.DHCPSVR_UpdateInfo['finish'] = function() {
                aCb();
            };
            iface.DHCPSVR_UpdateInfo();
        });
    }

    DHCPSVR_IsRunning(aCb: (aRunning: number) => void) {
        this._call( function (iface){
            iface.DHCPSVR_IsRunning['finish'] = function(aRunning: number) {
                aCb(aRunning);
            };
            iface.DHCPSVR_IsRunning();
        });
    }

    DHCPSVR_GetDevIndex(aCb: (aDeviceIndex: number) => void) {
        this._call( function (iface){
            iface.DHCPSVR_GetDevIndex['finish'] = function(aDeviceIndex: number) {
                aCb(aDeviceIndex);
            };
            iface.DHCPSVR_GetDevIndex();
        });
    }

    DHCPSVR_Start(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.DHCPSVR_Start['finish'] = function() {
                aCb();
            };
            iface.DHCPSVR_Start(aIndex);
        });
    }

    DHCPSVR_Stop(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.DHCPSVR_Stop['finish'] = function() {
                aCb();
            };
            iface.DHCPSVR_Stop(aIndex);
        });
    }

    CONF_GetNumOfNetDev(aCb: (aCount: number) => void) {
        this._call( function (iface){
            iface.CONF_GetNumOfNetDev['finish'] = function(aCount: number) {
                aCb(aCount);
            };
            iface.CONF_GetNumOfNetDev();
        });
    }

    CONF_GetDefaultDev(aCb: (aIndex: number) => void) {
        this._call( function (iface){
            iface.CONF_GetDefaultDev['finish'] = function(aIndex: number) {
                aCb(aIndex);
            };
            iface.CONF_GetDefaultDev();
        });
    }

    CONF_GetDevInfo(aIndex: number, aCb: (aDeviceInfo: tdata.TNetDevinfo) => void) {
        this._call( function (iface){
            iface.CONF_GetDevInfo['finish'] = function(data) {
                aCb(tdata.convert_netdevinfo(data));
            };
            iface.CONF_GetDevInfo(aIndex);
        });
    }

    CONF_GetMacAddr(aIndex: number, aCb: (aMacAddr: string) => void) {
        this._call( function (iface){
            iface.CONF_GetMacAddr['finish'] = function(aMacAddr: string) {
                aCb(aMacAddr);
            };
            iface.CONF_GetMacAddr(aIndex);
        });
    }

    //CONF_GetAddrInfo
    //CONF_SetAddrInfo

    CONF_StopConnecting(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.CONF_StopConnecting['finish'] = function() {
                aCb();
            };
            iface.CONF_StopConnecting(aIndex);
        });
    }

    CONF_SaveEnableStatus(aIndex: number, aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.CONF_SaveEnableStatus['finish'] = function() {
                aCb();
            };
            iface.CONF_SaveEnableStatus(aIndex, aEnable);
        });
    }

    CONF_AddrN2A(aAddress: number, aCb: (aAddressStr: string) => void) {
        this._call( function (iface){
            iface.CONF_AddrN2A['finish'] = function(aAddressStr: string) {
                aCb(aAddressStr);
            };
            iface.CONF_AddrN2A(aAddress);
        });
    }

    CONF_AddrA2N(aAddressStr: string, aCb: (aAddress: number) => void) {
        this._call( function (iface){
            iface.CONF_AddrA2N['finish'] = function(aAddress: number) {
                aCb(aAddress);
            };
            iface.CONF_AddrA2N(aAddressStr);
        });
    }

    CONF_IsSupportNetwork(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.CONF_IsSupportNetwork['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.CONF_IsSupportNetwork();
        });
    }

    CONF_IsNetworkAvailable(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.CONF_IsNetworkAvailable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.CONF_IsNetworkAvailable();
        });
    }

    WIFI_StartScan(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.WIFI_StartScan['finish'] = function() {
                aCb();
            };
            iface.WIFI_StartScan(aIndex);
        });
    }

    //WIFI_ConnectAP
    //WIFI_GetConnectedInfo

    WIFI_ConnectAPByWPS(aIndex: number, aWpsType: number, aPinCode: string, aCb: () => void) {
        this._call( function (iface){
            iface.WIFI_ConnectAPByWPS['finish'] = function() {
                aCb();
            };
            iface.WIFI_ConnectAPByWPS(aIndex, aWpsType, aPinCode);
        });
    }

    WIFI_DisconnectAP(aIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.WIFI_DisconnectAP['finish'] = function() {
                aCb();
            };
            iface.WIFI_DisconnectAP(aIndex);
        });
    }
}

export = CUtilNetwork