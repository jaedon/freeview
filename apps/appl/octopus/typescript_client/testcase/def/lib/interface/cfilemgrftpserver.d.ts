/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrFtpserver extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (handleId: number, result: number) => void): void;
    public Start(aCb: (aReval: number) => void): void;
    public Stop(aCb: (aReval: number) => void): void;
    public SetConfig(aServerName: string, aPort: number, aCb: (aReval: number) => void): void;
    public GetServerInfo(aCb: (aFtpInfo: tdata.TFTPServerInfo) => void): void;
    public SetPassword(aUserName: string, aCurrentPassword: string, aNewPassword: string, aCb: (aReval: number) => void): void;
    public CheckPassword(aUserName: string, aPassword: string, aCb: (aIsVaild: number) => void): void;
    public SetPermission(aUserName: string, aName: string, aPath: string, aState: number, aWritable: number, aMounted: number, aCb: (aIsVaild: number) => void): void;
    public GetSharedList(aUserName: string, aCb: (aSharedInfoList: tdata.TFTPShared[]) => void): void;
    public SetPublicAccess(aUserName: string, aAccess: number, aCb: (aReval: number) => void): void;
    public GetPublicAccess(aUserName: string, aCb: (aAccess: number) => void): void;
}
export = CFilemgrFtpserver;
