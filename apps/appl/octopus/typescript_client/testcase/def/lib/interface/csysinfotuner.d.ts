/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CSysinfoTuner extends dbusConn.CDBusInterface {
    constructor();
    public GetDeviceNum(aCb: (aDeviceNumber: number) => void): void;
    public GetDevType(aIndex: number, aCb: (aDeliveryType: number) => void): void;
    public IsLocked(aIndex: number, aCb: (aLocked: number) => void): void;
    public GetSignalLevel(aIndex: number, aCb: (aSignalLevel: number) => void): void;
    public GetSignalQuality(aIndex: number, aCb: (aSignalQuality: number) => void): void;
    public GetAgc(aIndex: number, aCb: (aAgc: number) => void): void;
    public GetBer(aIndex: number, aCb: (aBer: number) => void): void;
    public GetSnr(aIndex: number, aCb: (aSnr: number) => void): void;
    public GetInputPower(aIndex: number, aCb: (aInputPower: number) => void): void;
    public GetTuneParam(aIndex: number, aCb: (aTunerParam: number) => void): void;
    public GetTunerIdByTripleId(aOnId: number, aTsId: number, aSvcId: number, aCb: (aTunerId: number) => void): void;
}
export = CSysinfoTuner;
