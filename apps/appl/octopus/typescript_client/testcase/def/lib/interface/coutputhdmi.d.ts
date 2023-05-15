/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class COutputHdmi extends dbusConn.CDBusInterface {
    constructor();
    public SupportCec(aCb: (aSupport: number) => void): void;
    public GetHdcpStatus(aCb: (aHdcpStatus: number) => void): void;
    public Set3DSelect(aHdmi3DSelect: number, aCb: () => void): void;
    public Get3DSelect(aCb: (aHdmi3DSelect: number) => void): void;
    public GetEdid3DFormatCapability(aCb: (aEdid3DFormatCapability: number) => void): void;
    public Get3DStatus(aCb: (aHdmi3DStatus: number) => void): void;
    public GetEdidResolutionCapability(aCb: (aResolutionCapability: number) => void): void;
    public SetCecEnable(aSupport: number, aCb: () => void): void;
    public GetCecEnable(aCb: (aSupport: number) => void): void;
}
export = COutputHdmi;
