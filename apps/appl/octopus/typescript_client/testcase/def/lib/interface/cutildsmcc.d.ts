/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CUtilDsmcc extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public Create(aType: number, aCb: (aHandle: number, aCachePath: string) => void): void;
    public Destroy(aHandle: number, aCb: () => void): void;
    public GetCarouselList(aHandle: number, aCb: (aList: tdata.TCarouselItem[]) => void): void;
    public Start(aHandleDsmcc: number, aHandleCarousel: number, aCachePath: string, aCb: () => void): void;
    public ReStart(aHandleDsmcc: number, aHandleCarousel: number, aCb: () => void): void;
    public Stop(aHandleDsmcc: number, aHandleCarousel: number, aCb: () => void): void;
    public RequestEvent(aHandleDsmcc: number, aAll: number, aCb: () => void): void;
    public Open(aHandleDsmcc: number, aHandleSession: number, aCb: () => void): void;
    public Close(aHandleDsmcc: number, aHandleSession: number, aCb: () => void): void;
}
export = CUtilDsmcc;
