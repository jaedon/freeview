/// <reference path="../../../def/node.d.ts" />
import dbusConn = require('../util/dbus_connect');
declare class COutputAudio extends dbusConn.CDBusInterface {
    constructor();
    public SupportSpdif(aCb: (aSupport: number) => void): void;
    public GetMasterMute(aCb: (aMute: number) => void): void;
    public SetMasterMute(aMute: number, aCb: () => void): void;
    public GetVolumeMax(aCb: (aMaxVolume: number) => void): void;
    public GetVolume(aCb: (aVolume: number) => void): void;
    public SetVolume(aVolume: number, aCb: () => void): void;
    public GetLipSyncDelay(aCb: (aMSec: number) => void): void;
    public SetLipSyncDelay(aMSec: number, aCb: () => void): void;
    public SetHdmiFormat(aDigitalAudioFormat: number, aCb: () => void): void;
    public GetHdmiFormat(aCb: (aDigitalAudioFormat: number) => void): void;
    public SetSpdifFormat(aDigitalSpdifFormat: number, aCb: () => void): void;
    public GetSpdifFormat(aCb: (aDigitalSpdifFormat: number) => void): void;
    public GetAudioDescription(aCb: (aEnable: number) => void): void;
    public SetAudioDescription(aEnable: number, aCb: () => void): void;
    public SetStereoSelect(aStereoSelect: number, aCb: () => void): void;
    public GetStereoSelect(aCb: (aStereoSelect: number) => void): void;
    public SetDualMonoSelect(aDualMonoSelect: number, aCb: () => void): void;
    public GetDualMonoSelect(aCb: (aDualMonoSelect: number) => void): void;
}
export = COutputAudio;
