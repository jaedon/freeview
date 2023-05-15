/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrSambaclient extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public StartScan(aCb: () => void): void;
    public StopScan(aCb: () => void): void;
    public Mount(aSharedFolderInfo: number, aCb: () => void): void;
    public UnMount(aSharedFolderInfo: number, aCb: () => void): void;
    public UnmountAll(aCb: () => void): void;
}
export = CFilemgrSambaclient;
