/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CUtilExtdev extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (type: number, param1: number, param2: number, param3: number) => void): void;
    public MIC_Start(aCb: () => void): void;
    public MIC_Stop(aCb: () => void): void;
    public MIC_SetVolume(aValue: number, aCb: () => void): void;
    public MIC_GetVolume(aCb: (aValue: number) => void): void;
    public MIC_GetCount(aCb: (aValue: number) => void): void;
}
export = CUtilExtdev;
