/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CUtilSwupdate extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (type: number, param1: number, param2: number, param3: number) => void): void;
    public Create(aSwUpdateSpec: number, aCb: (aHandle: number) => void): void;
    public CreateManualConnect(aSwUpdateSpec: number, aTunningInfo: tdata.TTransponderInfo, aCb: (aHandle: number) => void): void;
    public StartDetect(aHandle: number, aCb: () => void): void;
    public StopDetect(aHandle: number, aCb: () => void): void;
    public StartDownload(aHandle: number, aCb: () => void): void;
    public StopDownload(aHandle: number, aCb: () => void): void;
    public StartInstall(aHandle: number, aCb: () => void): void;
    public StopInstall(aHandle: number, aCb: () => void): void;
    public Cancel(aHandle: number, aCb: () => void): void;
}
export = CUtilSwupdate;
