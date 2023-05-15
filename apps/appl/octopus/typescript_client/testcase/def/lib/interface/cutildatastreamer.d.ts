/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CUtilDatastreamer extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (data: number[]) => void): void;
    public ConnectDataStreamer(aDataType: string, aCb: () => void): void;
    public DisconnectDataStreamer(aDataType: string, aCb: () => void): void;
}
export = CUtilDatastreamer;
