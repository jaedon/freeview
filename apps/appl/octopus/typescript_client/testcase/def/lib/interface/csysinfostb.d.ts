/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CSysinfoStb extends dbusConn.CDBusInterface {
    constructor();
    public GetModelName(aCb: (aModelName: string) => void): void;
    public GetUpdatedList(aCb: (aUpdateList: string) => void): void;
    public GetSoftwareVersion(aCb: (aSoftwareVersion: string) => void): void;
    public GetLoaderName(aCb: (aLoaderName: string) => void): void;
    public GetLoaderVersion(aCb: (aLoaderVersion: string) => void): void;
    public GetManufacturerName(aCb: (aManufacturerName: string) => void): void;
    public GetSystemId(aCb: (aSystemId: string) => void): void;
    public GetDeviceId(aCb: (aDeviceId: string) => void): void;
    public GetChipId(aCb: (aChipId: string) => void): void;
    public GetObamaVersion(aCb: (aObamaVersion: string) => void): void;
    public GetMicomVersion(aCb: (aMicomVersion: string) => void): void;
    public GetUpdateDate(aCb: (aUpdateDate: string) => void): void;
    public GetSerialNumber(aCb: (aSerialNo: string) => void): void;
    public GetCustomCode(aCb: (aCustomCode: number) => void): void;
    public GetColdBootDone(aCb: (aCheckColdBootDone: number) => void): void;
    public SetFirstBootOnProduction(aFirstBootOnProduction: number, aCb: () => void): void;
    public GetFirstBootOnProduction(aCb: (aFirstBootOnProduction: number) => void): void;
    public SetAntennaPower(aAntPower: number, aCb: () => void): void;
    public GetAntennaPower(aCb: (aAntPower: number) => void): void;
    public SetServiceUpdateFlag(aEnable: number, aCb: () => void): void;
    public GetServiceUpdateFlag(aCb: (aEnable: number) => void): void;
    public GetNandBadBlocks(aCb: (aCntLdr: number, aCntUBI: number, aCntUBIFS: number) => void): void;
    public GetFusingInfo(aCb: (aScs: number, aJtag: number, aOtp: number, aCwe: number) => void): void;
}
export = CSysinfoStb;
