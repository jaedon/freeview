/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrStorage extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public Format(aDeviceId: number, aVolume: string, aCb: () => void): void;
    public StartDST(aDeviceId: number, aCb: () => void): void;
    public SetDefaultHdd(aStorageId: string, aCb: () => void): void;
    public DisablePVRHdd(aStorageId: string, aCb: () => void): void;
    public PairingHdd(aStorageId: string, aCb: () => void): void;
    public UnPairingHdd(aStorageId: string, aCb: () => void): void;
    public SetName(aStorageId: string, aName: string, aCb: () => void): void;
    public SafeRemove(aDeviceId: number, aForced: number, aCb: () => void): void;
    public GetList(aFilter: number, aCb: (aInfoList: number[]) => void): void;
    public GetSize(aDeviceId: number, aCb: (aTotalKb: number, aAvailableKb: number, aUsedKb: number, aReservedKb: number, aAvailablePvrKb: number) => void): void;
    public GetPvrReservedSize(aDeviceId: number, aCb: (aReservedKb: number) => void): void;
    public CheckUsbSpeed(aStorageId: string, aCb: () => void): void;
    public GetDevTypeString(aDeviceType: number, aCb: (aTypeStr: string) => void): void;
    public GetFsString(aFsType: number, aCb: (aTypeStr: string) => void): void;
}
export = CFilemgrStorage;
