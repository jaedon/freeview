/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CMediaRecord extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public IsRecording(aSvcUid: number, aCb: (aIsRecording: number) => void): void;
    public GetCID(aSvcUid: number, aCb: (aCid: number) => void): void;
    public Start(aMasterSvcUid: number, aSuppleSvcUid: number, aSuppSvcType: number, aStartOffset: number, aStartTime: number, aDuration: number, aStorageType: number, aStorageId: string, aEventId: number, aCb: (aSessionid: number) => void): void;
    public Stop(aSessionid: number, aCb: () => void): void;
    public StopByUID(aSvcUid: number, aCb: () => void): void;
    public GetRecTime(aSessionid: number, aCb: (aRecTime: number) => void): void;
    public GetStartTime(aSessionid: number, aCb: (aStartTime: number) => void): void;
    public GetURL(aSessionid: number, aCb: (aUrl: string) => void): void;
    public GetRsvSlotByUID(aSvcUid: number, aCb: (aRsvSlot: number) => void): void;
}
export = CMediaRecord;
