/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrSambaserver extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public Start(aCb: () => void): void;
    public Stop(aCb: () => void): void;
    public SetConfig(aConfigInfo: number, aCb: () => void): void;
    public GetServerInfo(aCb: (aConfigInfo: number) => void): void;
    public SetPassword(aUserName: string, aCurrentPassword: string, aNewPassword: string, aCb: () => void): void;
    public CheckPassword(aUserName: string, aPassword: string, aCb: (aIsVaild: number) => void): void;
    public SetPermission(aUserName: string, aSharedInfo: number, aCb: () => void): void;
    public GetSharedList(aUserName: string, aCb: (aSharedInfo: number) => void): void;
    public SetPublicAccess(aUserName: string, aAccess: number, aCb: () => void): void;
    public GetPublicAccess(aUserName: string, aCb: (aAccess: number) => void): void;
}
export = CFilemgrSambaserver;
