/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class COutputVideo extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Output.Video', '/Octopus/Appkit/Output/Video');
    }

    SupportComposite(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.SupportComposite['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.SupportComposite();
        });
    }

    SupportTvScart(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.SupportTvScart['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.SupportTvScart();
        });
    }

    SupportVcrScart(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.SupportVcrScart['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.SupportVcrScart();
        });
    }

    GetSupportTvScartFormat(aCb: (aScartFormat: number) => void) {
        this._call( function (iface){
            iface.GetSupportTvScartFormat['finish'] = function(aScartFormat: number) {
                aCb(aScartFormat);
            };
            iface.GetSupportTvScartFormat();
        });
    }

    GetSupportVcrScartFormat(aCb: (aScartFormat: number) => void) {
        this._call( function (iface){
            iface.GetSupportVcrScartFormat['finish'] = function(aScartFormat: number) {
                aCb(aScartFormat);
            };
            iface.GetSupportVcrScartFormat();
        });
    }

    SetTvScartFormat(aScartFormat: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetTvScartFormat['finish'] = function() {
                aCb();
            };
            iface.SetTvScartFormat(aScartFormat);
        });
    }

    GetTvScartFormat(aCb: (aScartFormat: number) => void) {
        this._call( function (iface){
            iface.GetTvScartFormat['finish'] = function(aScartFormat: number) {
                aCb(aScartFormat);
            };
            iface.GetTvScartFormat();
        });
    }

    SetVcrScartFormat(aScartFormat: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetVcrScartFormat['finish'] = function() {
                aCb();
            };
            iface.SetVcrScartFormat(aScartFormat);
        });
    }

    GetVcrScartFormat(aCb: (aScartFormat: number) => void) {
        this._call( function (iface){
            iface.GetVcrScartFormat['finish'] = function(aScartFormat: number) {
                aCb(aScartFormat);
            };
            iface.GetVcrScartFormat();
        });
    }

    SetScartPath(aScartPath: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetScartPath['finish'] = function() {
                aCb();
            };
            iface.SetScartPath(aScartPath);
        });
    }

    GetScartPath(aCb: (aScartPath: number) => void) {
        this._call( function (iface){
            iface.GetScartPath['finish'] = function(aScartPath: number) {
                aCb(aScartPath);
            };
            iface.GetScartPath();
        });
    }

    SetResolutionSelect(aResolutionSelect: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetResolutionSelect['finish'] = function() {
                aCb();
            };
            iface.SetResolutionSelect(aResolutionSelect);
        });
    }

    GetResolutionSelect(aCb: (aResolutionSelect: number) => void) {
        this._call( function (iface){
            iface.GetResolutionSelect['finish'] = function(aResolutionSelect: number) {
                aCb(aResolutionSelect);
            };
            iface.GetResolutionSelect();
        });
    }

    GetResolutionStatus(aCb: (aResolutionStatus: number) => void) {
        this._call( function (iface){
            iface.GetResolutionStatus['finish'] = function(aResolutionStatus: number) {
                aCb(aResolutionStatus);
            };
            iface.GetResolutionStatus();
        });
    }

    SetTvAspectRatio(aTvAspectRatio: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetTvAspectRatio['finish'] = function() {
                aCb();
            };
            iface.SetTvAspectRatio(aTvAspectRatio);
        });
    }

    GetTvAspectRatio(aCb: (aTvAspectRatio: number) => void) {
        this._call( function (iface){
            iface.GetTvAspectRatio['finish'] = function(aTvAspectRatio: number) {
                aCb(aTvAspectRatio);
            };
            iface.GetTvAspectRatio();
        });
    }

    GetPictureRate(aCb: (aPictureRate: number) => void) {
        this._call( function (iface){
            iface.GetPictureRate['finish'] = function(aPictureRate: number) {
                aCb(aPictureRate);
            };
            iface.GetPictureRate();
        });
    }

    SetDfcSelect(aDfcSelect: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetDfcSelect['finish'] = function() {
                aCb();
            };
            iface.SetDfcSelect(aDfcSelect);
        });
    }

    GetDfcSelect(aCb: (aDfcSelect: number) => void) {
        this._call( function (iface){
            iface.GetDfcSelect['finish'] = function(aDfcSelect: number) {
                aCb(aDfcSelect);
            };
            iface.GetDfcSelect();
        });
    }

    GetDfcStatus(aCb: (aDfcStatus: number) => void) {
        this._call( function (iface){
            iface.GetDfcStatus['finish'] = function(aDfcStatus: number) {
                aCb(aDfcStatus);
            };
            iface.GetDfcStatus();
        });
    }

    SetBackGroundColor(aRed: number, aGreen: number, aBlue: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetBackGroundColor['finish'] = function() {
                aCb();
            };
            iface.SetBackGroundColor(aRed, aGreen, aBlue);
        });
    }

    GetBackGroundColor(aCb: (aRed: number, aGreen: number, aBlue: number) => void) {
        this._call( function (iface){
            iface.GetBackGroundColor['finish'] = function(data) {
                aCb(data[0], data[1], data[2]);
            };
            iface.GetBackGroundColor();
        });
    }

    SetAvEnable(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAvEnable['finish'] = function() {
                aCb();
            };
            iface.SetAvEnable(aEnable);
        });
    }

    GetAvEnable(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetAvEnable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetAvEnable();
        });
    }

    GetCompositeStandard(aCb: (aCompositeStandard: number) => void) {
        this._call( function (iface){
            iface.GetCompositeStandard['finish'] = function(aCompositeStandard: number) {
                aCb(aCompositeStandard);
            };
            iface.GetCompositeStandard();
        });
    }
}

export = COutputVideo