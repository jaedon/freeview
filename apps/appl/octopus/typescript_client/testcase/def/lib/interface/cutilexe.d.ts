/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class CUtilExe extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public BATCH_Stop(aCb: () => void): void;
    public BGEIT_Start_FreeSat(aCb: () => void): void;
    public BGEIT_Stop_FreeSat(aCb: () => void): void;
    public FTP_Start(aCb: () => void): void;
    public FTP_Stop(aCb: () => void): void;
    public FTP_GetStatus(aCb: (aFtpRun: number) => void): void;
    public StartFactoryDefault(aFactoryDefaultMode: number, aCb: () => void): void;
    public RemoveLcnUpdateInfo(aSyncMode: number, aCb: () => void): void;
    public BML_Set(aBMLKeyControlMode: number, aCb: () => void): void;
    public RFVOD_Start(aVodMode: number, aUrl: string, aCb: () => void): void;
    public RFVOD_Stop(aCb: () => void): void;
    public MHEG_Start(aWndId: number, aStillWndId: number, aCb: () => void): void;
    public MHEG_SetKey(aKey: number, aCb: () => void): void;
    public MHEG_DeepLink(aUrl: string, aPrevLcn: number, aCb: () => void): void;
    public MHEG_ControlOP(aControl: number, aCb: () => void): void;
    public MHEGIB_RequestFile(aSessionId: number, aRequestNameLen: number, aRequestName: string, aCb: () => void): void;
    public MHEGIB_RequestData(aSessionId: number, aRequestNameLen: number, aRequestName: string, aCb: () => void): void;
    public MHEGIB_AbortByUser(aSessionId: number, aCb: () => void): void;
    public MHEGIB_AbortBySystem(aSessionId: number, aCb: () => void): void;
    public StartSatRecord(aCb: () => void): void;
    public TVTVEPG_Initdirectory(aCb: () => void): void;
    public Load_BaseEPG(aCb: () => void): void;
    public Load_TVTVEPG(aCb: () => void): void;
}
export = CUtilExe;
