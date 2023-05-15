/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CCasAction extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public GetSlotId(aCb: (aSlotId: number) => void): void;
    public GetCasId(aCb: (aCasId: number) => void): void;
    public GetData(aCb: (aData: number[]) => void): void;
}
export = CCasAction;
