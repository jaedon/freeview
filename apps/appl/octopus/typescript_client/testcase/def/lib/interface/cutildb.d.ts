/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CUtilDb extends dbusConn.CDBusInterface {
    constructor();
    public Remove(aKey: string, aCb: () => void): void;
    public SetInt(aKey: string, aValue: number, aCb: () => void): void;
    public GetInt(aKey: string, aCb: (aValue: number) => void): void;
    public SetStr(aKey: string, aValue: string, aCb: () => void): void;
    public GetStr(aKey: string, aCb: (aValue: string) => void): void;
}
export = CUtilDb;
