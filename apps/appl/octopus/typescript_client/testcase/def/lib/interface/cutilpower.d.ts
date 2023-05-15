/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CUtilPower extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public GetWakeupReason(aCb: (aWakeupReason: number) => void): void;
    public GetWakeupSecondReason(aCb: (aWakeupSecondReason: number) => void): void;
    public SetStandbyPowerMode(aPowerMode: number, aCb: () => void): void;
    public GetStandbyPowerMode(aCb: (aPowerMode: number) => void): void;
    public IsStandbyMode(aCb: (aReval: number) => void): void;
    public GoingToReboot(aCb: (aReval: number) => void): void;
    public GoingToShutdown(aCb: (aReval: number) => void): void;
    public GoingToOperate(aCb: (aReval: number) => void): void;
    public RequestShutDown(aCb: () => void): void;
    public RequestReboot(aCb: () => void): void;
    public RequestOperation(aCb: () => void): void;
    public WaitShutDown(aWaitTime: number, aCb: () => void): void;
}
export = CUtilPower;
