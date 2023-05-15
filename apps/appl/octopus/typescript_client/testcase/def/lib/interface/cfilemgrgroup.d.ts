/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CFilemgrGroup extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public Create(aExtFilter: string, aCb: (aHandle: number) => void): void;
    public Release(aCb: (aHandle: number) => void): void;
    public AddPath(aHandle: number, aPath: string, aCb: () => void): void;
    public RemovePath(aHandle: number, aPath: string, aCb: () => void): void;
    public ExcludePath(aHandle: number, aPath: string, aCb: () => void): void;
    public Copy(aHandle: number, aPath: string, aCb: (aRequestId: number) => void): void;
    public Move(aHandle: number, aPath: string, aCb: (aRequestId: number) => void): void;
    public Remove(aHandle: number, aPath: string, aCb: (aRequestId: number) => void): void;
    public Cancel(aHandle: number, aCb: (aRequestId: number) => void): void;
}
export = CFilemgrGroup;
