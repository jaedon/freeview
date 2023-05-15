/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CCasMmi extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (param1: number, param2: number, param3: number) => void): void;
    public GetEvtInfo(aCb: (aEventInfo: tdata.TCASMMIEvent) => void): void;
    public GetUiEvent(aSessionType: number, aHandleSession: number, aCb: (aEventInfo: tdata.TCASMMIEvent) => void): void;
    public SendUiResponse(aGroupId: number, aSlot: number, aSessionType: number, aHandleSession: number, aInputData: number[], aCb: () => void): void;
    public CloseSession(aGroupId: number, aSlot: number, aSessionType: number, aHandleSession: number, aCb: () => void): void;
    public OpenSession(aGroupId: number, aSlot: number, aSessionType: number, aParamData: number[], aCb: (aHandleSession: number) => void): void;
}
export = CCasMmi;
