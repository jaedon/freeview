/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class COutputAudio extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Output.Audio', '/Octopus/Appkit/Output/Audio');
    }

    SupportSpdif(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.SupportSpdif['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.SupportSpdif();
        });
    }

    GetMasterMute(aCb: (aMute: number) => void) {
        this._call( function (iface){
            iface.GetMasterMute['finish'] = function(aMute: number) {
                aCb(aMute);
            };
            iface.GetMasterMute();
        });
    }

    SetMasterMute(aMute: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetMasterMute['finish'] = function() {
                aCb();
            };
            iface.SetMasterMute(aMute);
        });
    }

    GetVolumeMax(aCb: (aMaxVolume: number) => void) {
        this._call( function (iface){
            iface.GetVolumeMax['finish'] = function(aMaxVolume: number) {
                aCb(aMaxVolume);
            };
            iface.GetVolumeMax();
        });
    }

    GetVolume(aCb: (aVolume: number) => void) {
        this._call( function (iface){
            iface.GetVolume['finish'] = function(aVolume: number) {
                aCb(aVolume);
            };
            iface.GetVolume();
        });
    }

    SetVolume(aVolume: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetVolume['finish'] = function() {
                aCb();
            };
            iface.SetVolume(aVolume);
        });
    }

    GetLipSyncDelay(aCb: (aMSec: number) => void) {
        this._call( function (iface){
            iface.GetLipSyncDelay['finish'] = function(aMSec: number) {
                aCb(aMSec);
            };
            iface.GetLipSyncDelay();
        });
    }

    SetLipSyncDelay(aMSec: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetLipSyncDelay['finish'] = function() {
                aCb();
            };
            iface.SetLipSyncDelay(aMSec);
        });
    }

    SetHdmiFormat(aDigitalAudioFormat: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetHdmiFormat['finish'] = function() {
                aCb();
            };
            iface.SetHdmiFormat(aDigitalAudioFormat);
        });
    }

    GetHdmiFormat(aCb: (aDigitalAudioFormat: number) => void) {
        this._call( function (iface){
            iface.GetHdmiFormat['finish'] = function(aDigitalAudioFormat: number) {
                aCb(aDigitalAudioFormat);
            };
            iface.GetHdmiFormat();
        });
    }

    SetSpdifFormat(aDigitalSpdifFormat: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSpdifFormat['finish'] = function() {
                aCb();
            };
            iface.SetSpdifFormat(aDigitalSpdifFormat);
        });
    }

    GetSpdifFormat(aCb: (aDigitalSpdifFormat: number) => void) {
        this._call( function (iface){
            iface.GetSpdifFormat['finish'] = function(aDigitalSpdifFormat: number) {
                aCb(aDigitalSpdifFormat);
            };
            iface.GetSpdifFormat();
        });
    }

    GetAudioDescription(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetAudioDescription['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetAudioDescription();
        });
    }

    SetAudioDescription(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAudioDescription['finish'] = function() {
                aCb();
            };
            iface.SetAudioDescription(aEnable);
        });
    }

    SetStereoSelect(aStereoSelect: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetStereoSelect['finish'] = function() {
                aCb();
            };
            iface.SetStereoSelect(aStereoSelect);
        });
    }

    GetStereoSelect(aCb: (aStereoSelect: number) => void) {
        this._call( function (iface){
            iface.GetStereoSelect['finish'] = function(aStereoSelect: number) {
                aCb(aStereoSelect);
            };
            iface.GetStereoSelect();
        });
    }

    SetDualMonoSelect(aDualMonoSelect: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetDualMonoSelect['finish'] = function() {
                aCb();
            };
            iface.SetDualMonoSelect(aDualMonoSelect);
        });
    }

    GetDualMonoSelect(aCb: (aDualMonoSelect: number) => void) {
        this._call( function (iface){
            iface.GetDualMonoSelect['finish'] = function(aDualMonoSelect: number) {
                aCb(aDualMonoSelect);
            };
            iface.GetDualMonoSelect();
        });
    }
}

export = COutputAudio