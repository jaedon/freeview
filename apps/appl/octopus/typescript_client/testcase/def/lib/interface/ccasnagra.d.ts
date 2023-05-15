/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CCasNagra extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public IsCWE(aCb: (aCWE: number) => void): void;
    public IsJTAG(aCb: (aJTAG: number) => void): void;
    public IsSBOOT(aCb: (aSboot: number) => void): void;
    public IsOTP(aCb: (aSOTP: number) => void): void;
    public GetNUID(aCb: (aNuid: string) => void): void;
    public GetCASN(aCb: (aCasn: string) => void): void;
    public GetFusingInfo(aCb: (aScs: number, aJtag: number, aOtp: number, aCwe: number) => void): void;
    public IsSCInserted(aCb: (aScinserted: number) => void): void;
    public VerifyPincode(aPinStr: string, aCb: (aIsOk: number) => void): void;
    public GetAccessibleState(aCb: (aAccessState: number, aRemainTime: number, aRetryCount: number, aIsOk: number) => void): void;
    public VerifyPincodeGetAccessibleState(aPinStr: string, aCb: (aAccessState: number, aRemainTime: number, aRetryCount: number, aIsOk: number) => void): void;
    public SetPincode(aOldPinStr: string, aNewPinStr: string, aCb: (aIsOk: number) => void): void;
}
export = CCasNagra;
