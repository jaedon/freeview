/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CCasIrdeto extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public SetVirtualSecureCwMode(aVirtualSecureCwMode: number, aCb: () => void): void;
    public GetVirtualSecureCwMode(aCb: (aVirtualSecureCwMode: number) => void): void;
    public GetIrdetoClientVersionName(aCb: (aVersionName: string) => void): void;
    public IsCWE(aCb: (aCWE: number) => void): void;
    public IsJTAG(aCb: (aJTAG: number) => void): void;
    public IsSBOOT(aCb: (aSboot: number) => void): void;
    public IsOTP(aCb: (aSOTP: number) => void): void;
    public GetIrdetoManCode(aCb: (aManCode: string) => void): void;
    public GetIrdetoHwCode(aCb: (aHwcode: string) => void): void;
    public GetIrdetoVariant(aCb: (aVariant: string) => void): void;
    public GetIrdetoSysId(aCb: (aSysId: string) => void): void;
    public GetIrdetoKeyVer(aCb: (aKeyVer: string) => void): void;
    public GetIrdetoSigVer(aCb: (aSigVer: string) => void): void;
    public GetControlByte(aCb: (aControlByte: number) => void): void;
}
export = CCasIrdeto;
