/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class COutputHdmi extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Output.Hdmi', '/Octopus/Appkit/Output/Hdmi');
    }

    SupportCec(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.SupportCec['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.SupportCec();
        });
    }

    GetHdcpStatus(aCb: (aHdcpStatus: number) => void) {
        this._call( function (iface){
            iface.GetHdcpStatus['finish'] = function(aHdcpStatus: number) {
                aCb(aHdcpStatus);
            };
            iface.GetHdcpStatus();
        });
    }

    Set3DSelect(aHdmi3DSelect: number, aCb: () => void) {
        this._call( function (iface){
            iface.Set3DSelect['finish'] = function() {
                aCb();
            };
            iface.Set3DSelect(aHdmi3DSelect);
        });
    }

    Get3DSelect(aCb: (aHdmi3DSelect: number) => void) {
        this._call( function (iface){
            iface.Get3DSelect['finish'] = function(aHdmi3DSelect: number) {
                aCb(aHdmi3DSelect);
            };
            iface.Get3DSelect();
        });
    }

    GetEdid3DFormatCapability(aCb: (aEdid3DFormatCapability: number) => void) {
        this._call( function (iface){
            iface.GetEdid3DFormatCapability['finish'] = function(aEdid3DFormatCapability: number) {
                aCb(aEdid3DFormatCapability);
            };
            iface.GetEdid3DFormatCapability();
        });
    }

    Get3DStatus(aCb: (aHdmi3DStatus: number) => void) {
        this._call( function (iface){
            iface.Get3DStatus['finish'] = function(aHdmi3DStatus: number) {
                aCb(aHdmi3DStatus);
            };
            iface.Get3DStatus();
        });
    }

    GetEdidResolutionCapability(aCb: (aResolutionCapability: number) => void) {
        this._call( function (iface){
            iface.GetEdidResolutionCapability['finish'] = function(aResolutionCapability: number) {
                aCb(aResolutionCapability);
            };
            iface.GetEdidResolutionCapability();
        });
    }

    SetCecEnable(aSupport: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetCecEnable['finish'] = function() {
                aCb();
            };
            iface.SetCecEnable(aSupport);
        });
    }

    GetCecEnable(aCb: (aSupport: number) => void) {
        this._call( function (iface){
            iface.GetCecEnable['finish'] = function(aSupport: number) {
                aCb(aSupport);
            };
            iface.GetCecEnable();
        });
    }
}

export = COutputHdmi