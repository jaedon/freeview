/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CMetaRecord extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public GetCID(aUrl: string, aCb: (aCid: number) => void): void;
    public Count(aCb: (aCount: number) => void): void;
    public GetByIdx(aIndex: number, aCb: (aRecData: tdata.TRecListData) => void): void;
    public GetByCID(aCid: number, aCb: (aRecData: tdata.TRecListData) => void): void;
    public GetContentsIdByUrl(aUrl: string, aCb: (aCid: number) => void): void;
    public Release(aRecData: tdata.TRecListData, aCb: () => void): void;
    public GetItems(aDirect: string, aCb: () => void): void;
    public Move(aCid: number, aPath: string, aIsDecrypt: number, aCb: (aToken: number) => void): void;
    public CancelFileOperation(aCid: number, aCb: () => void): void;
    public Scan(aUrl: string, aCb: () => void): void;
    public Copy(aCid: number, aPath: string, bDecrypt: number, aCb: () => void): void;
    public Cancel(aCb: () => void): void;
    public Clear(aCb: () => void): void;
}
export = CMetaRecord;
