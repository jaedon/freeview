/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CUtilDrmagent extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgStr: string, msgInt: number, resultCode: number) => void): void;
    public SendDrmMessage(aMsgType: string, aMessage: string, aDrmSystemId: string, aMsgId: number, aCb: () => void): void;
    public SendDrmSystemStatusRequest(aDrmSystemId: string, aCb: () => void): void;
}
export = CUtilDrmagent;
