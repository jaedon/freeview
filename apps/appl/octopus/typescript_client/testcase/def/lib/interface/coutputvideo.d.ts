/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class COutputVideo extends dbusConn.CDBusInterface {
    constructor();
    public SupportComposite(aCb: (aSupport: number) => void): void;
    public SupportTvScart(aCb: (aSupport: number) => void): void;
    public SupportVcrScart(aCb: (aSupport: number) => void): void;
    public GetSupportTvScartFormat(aCb: (aScartFormat: number) => void): void;
    public GetSupportVcrScartFormat(aCb: (aScartFormat: number) => void): void;
    public SetTvScartFormat(aScartFormat: number, aCb: () => void): void;
    public GetTvScartFormat(aCb: (aScartFormat: number) => void): void;
    public SetVcrScartFormat(aScartFormat: number, aCb: () => void): void;
    public GetVcrScartFormat(aCb: (aScartFormat: number) => void): void;
    public SetScartPath(aScartPath: number, aCb: () => void): void;
    public GetScartPath(aCb: (aScartPath: number) => void): void;
    public SetResolutionSelect(aResolutionSelect: number, aCb: () => void): void;
    public GetResolutionSelect(aCb: (aResolutionSelect: number) => void): void;
    public GetResolutionStatus(aCb: (aResolutionStatus: number) => void): void;
    public SetTvAspectRatio(aTvAspectRatio: number, aCb: () => void): void;
    public GetTvAspectRatio(aCb: (aTvAspectRatio: number) => void): void;
    public GetPictureRate(aCb: (aPictureRate: number) => void): void;
    public SetDfcSelect(aDfcSelect: number, aCb: () => void): void;
    public GetDfcSelect(aCb: (aDfcSelect: number) => void): void;
    public GetDfcStatus(aCb: (aDfcStatus: number) => void): void;
    public SetBackGroundColor(aRed: number, aGreen: number, aBlue: number, aCb: () => void): void;
    public GetBackGroundColor(aCb: (aRed: number, aGreen: number, aBlue: number) => void): void;
    public SetAvEnable(aEnable: number, aCb: () => void): void;
    public GetAvEnable(aCb: (aEnable: number) => void): void;
    public GetCompositeStandard(aCb: (aCompositeStandard: number) => void): void;
}
export = COutputVideo;
