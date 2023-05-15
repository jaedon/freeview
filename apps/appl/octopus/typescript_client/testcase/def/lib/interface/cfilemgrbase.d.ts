/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrBase extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public BrowserFiles(aPath: string, aFilter: string, aKeyword: string, aBrowsingMode: number, aCb: (aRequestId: number) => void): void;
    public ScanDir(aSource: string, aExtFilter: string, aRecursive: number, aCb: (aRequestId: number) => void): void;
    public Remove(aSource: string, aExtFilter: string, aRecursive: number, aCb: (aRequestId: number) => void): void;
    public Copy(aSource: string, aDest: string, aExtFilter: string, aDecryption: number, aCb: (aRequestId: number) => void): void;
    public Move(aSource: string, aDest: string, aExtFilter: string, aDecryption: number, aCb: (aRequestId: number) => void): void;
    public MakeDir(aSource: string, aCb: (aRequestId: number) => void): void;
    public RemoveDir(aSource: string, aCb: (aRequestId: number) => void): void;
    public Cancel(aCancleRequestId: number, aCb: (aRequestId: number) => void): void;
    public GetSizeOfPath(aPath: string, aExtFilter: string, aCb: (aSize: number) => void): void;
}
export = CFilemgrBase;
