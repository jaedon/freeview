/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class COutputPanel extends dbusConn.CDBusInterface {
    constructor();
    public SetAppPriority(aPriority: number, aCb: () => void): void;
    public SetLed(aLedId: number, aLedStatus: number, aCb: () => void): void;
    public SetLedAuto(aCb: () => void): void;
    public SetLedCategory(aCategory: number, aCb: () => void): void;
    public ClearLed(aCb: () => void): void;
    public SetPanel(aPriority: number, aText: string, aPanelAlign: number, aCb: () => void): void;
    public SetPanelAuto(aPriority: number, aCb: () => void): void;
    public SetPanelDimLevel(aPriority: number, aDimLevel: number, aCb: () => void): void;
    public SetPanelAnimation(aPriority: number, aCb: () => void): void;
}
export = COutputPanel;
