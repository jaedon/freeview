/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrBrowse extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (handleId: number, event: number, result: number) => void): void;
    public Open(aPath: string, aCategory: number, aExtFilter: string, aOptFlags: number, aCb: (aHandle: number) => void): void;
    public Close(aHandle: number, aCb: () => void): void;
    public GetItems(aHandle: number, aIndex: number, aReqCount: number, aOption: number, aCb: () => void): void;
    public ScanItems(aHandle: number, aReqCount: number, aOption: number, aCb: () => void): void;
    public Sort(aHandle: number, aSort: number, aCb: () => void): void;
}
export = CFilemgrBrowse;
