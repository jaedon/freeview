/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CCasCi extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public StartMMIEvent(aCb: (msgType: number, userData: number[]) => void): void;
    public IsSupportedCI(aCb: (aSupported: number) => void): void;
    public IsSupportedCIPlus(aCb: (aSupported: number) => void): void;
    public GetCIPlusVersionName(aCb: (aVersionName: string) => void): void;
    public GetSlotCount(aCb: (aSlotCount: number) => void): void;
    public GetCamNumberBySlot(aSlot: number, aCb: (aCamNumber: number) => void): void;
    public GetCamStatusByIndex(aSlot: number, aCb: (aCamStatus: number) => void): void;
    public GetCamNameByIndex(aSlot: number, aCb: (aCamName: string) => void): void;
    public RequestFileorData(aSessionId: number, aReqType: number, aReqName: string, aCb: () => void): void;
    public RequestAppAbort(aSessionId: number, aAbortReqCode: number, aCb: () => void): void;
}
export = CCasCi;
