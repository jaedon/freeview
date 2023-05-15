/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CMediaAirplay extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void): void;
    public Start(aCb: () => void): void;
    public Stop(aCb: () => void): void;
    public SetPlaybackInfo(aPlaybackStatus: number, aDuration: number, aStartPosition: number, aBufferedRange: number, aSeekableRange: number, aRate: number, aCb: () => void): void;
}
export = CMediaAirplay;
