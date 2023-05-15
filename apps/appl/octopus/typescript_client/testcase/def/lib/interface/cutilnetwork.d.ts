/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CUtilNetwork extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, param1: number, param2: number, param3: number) => void): void;
    public BT_SetEnable(aEnable: number, aCb: () => void): void;
    public BT_GetEnable(aCb: (aEnable: number) => void): void;
    public BT_Scan(aCb: () => void): void;
    public BT_Connect(aIndex: number, aCb: () => void): void;
    public BT_ConnectByBdAddr(aBdAddress: string, aCb: () => void): void;
    public BT_Disconnect(aIndex: number, aCb: () => void): void;
    public BT_DisconnectByBdAddr(aBdAddress: string, aCb: () => void): void;
    public BT_Remove(aIndex: number, aCb: () => void): void;
    public BT_RemoveByBdAddr(aBdAddress: string, aCb: () => void): void;
    public BT_SendPinCode(aIndex: number, aPincode: number[], aCb: () => void): void;
    public BT_SendPinCodeByBdAddr(aBdAddress: string, aPincode: number[], aCb: () => void): void;
    public BT_GetDeviceNum(aCb: (aDeviceNum: number) => void): void;
    public BT_GetDeviceInfo(aDeviceId: number, aCb: (aDeviceInfo: tdata.TBTDeviceinfo) => void): void;
    public BT_GetMacAddrInfo(aCb: (aMacAddress: string) => void): void;
    public BT_GetMacAddrInfoSize(aCb: (aSize: number) => void): void;
    public BT_GetDevInfoList(aListSize: number, aCb: (aDeviceInfoList: tdata.TBTDeviceinfo[]) => void): void;
    public CM_GetStatus(aCb: (aStatus: number) => void): void;
    public CM_GetDownStreamInfo(aDownInfo: number, aCb: (aValue: number) => void): void;
    public CM_SetDownStreamInfo(aDownInfo: number, aValue: number, aCb: () => void): void;
    public CM_GetUpStreamInfo(aUpInfo: number, aCb: (aValue: number) => void): void;
    public DHCPSVR_UpdateInfo(aCb: () => void): void;
    public DHCPSVR_IsRunning(aCb: (aRunning: number) => void): void;
    public DHCPSVR_GetDevIndex(aCb: (aDeviceIndex: number) => void): void;
    public DHCPSVR_Start(aIndex: number, aCb: () => void): void;
    public DHCPSVR_Stop(aIndex: number, aCb: () => void): void;
    public CONF_GetNumOfNetDev(aCb: (aCount: number) => void): void;
    public CONF_GetDefaultDev(aCb: (aIndex: number) => void): void;
    public CONF_GetDevInfo(aIndex: number, aCb: (aDeviceInfo: tdata.TNetDevinfo) => void): void;
    public CONF_GetMacAddr(aIndex: number, aCb: (aMacAddr: string) => void): void;
    public CONF_StopConnecting(aIndex: number, aCb: () => void): void;
    public CONF_SaveEnableStatus(aIndex: number, aEnable: number, aCb: () => void): void;
    public CONF_AddrN2A(aAddress: number, aCb: (aAddressStr: string) => void): void;
    public CONF_AddrA2N(aAddressStr: string, aCb: (aAddress: number) => void): void;
    public CONF_IsSupportNetwork(aCb: (aSupport: number) => void): void;
    public CONF_IsNetworkAvailable(aCb: (aEnable: number) => void): void;
    public WIFI_StartScan(aIndex: number, aCb: () => void): void;
    public WIFI_ConnectAPByWPS(aIndex: number, aWpsType: number, aPinCode: string, aCb: () => void): void;
    public WIFI_DisconnectAP(aIndex: number, aCb: () => void): void;
}
export = CUtilNetwork;