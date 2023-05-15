/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CMetaAntenna extends dbusConn.CDBusInterface {
    constructor();
    public Add(aAntInfo: tdata.TAntennaInfo, aCb: (aUid: number) => void): void;
    public RemoveAll(aCb: () => void): void;
    public Count(aCb: (aCount: number) => void): void;
    public GetUidAt(aIndex: number, aCb: (aUid: number) => void): void;
    public GetInfo(aUid: number, aCb: (aAntInfo: tdata.TAntennaInfo) => void): void;
    public SetInfo(aUid: number, aAntInfo: tdata.TAntennaInfo, aCb: () => void): void;
    public Load(aCb: () => void): void;
    public Save(aCb: () => void): void;
}
export = CMetaAntenna;
