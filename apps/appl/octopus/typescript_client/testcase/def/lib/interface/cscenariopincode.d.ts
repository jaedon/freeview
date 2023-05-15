/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CScenarioPincode extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public Check(aPinString: string, aCb: (aReval: number) => void): void;
    public Verify(aPinString: string, aCb: (aReval: number) => void): void;
    public Set(aOldPinString: string, aNewPinString: string, aCb: (aReval: number) => void): void;
    public Reset(aNewPinString: string, aCb: (aReval: number) => void): void;
    public SetTemporaryBlock(aPinString: string, aBlocked: number, aCb: (aReval: number) => void): void;
    public GetTemporaryBlock(aCb: (aBlocked: number) => void): void;
}
export = CScenarioPincode;
