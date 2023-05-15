/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class COutputPanel extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Output.Panel', '/Octopus/Appkit/Output/Panel');
    }

    SetAppPriority(aPriority: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAppPriority['finish'] = function() {
                aCb();
            };
            iface.SetAppPriority(aPriority);
        });
    }

    SetLed(aLedId: number, aLedStatus: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetLed['finish'] = function() {
                aCb();
            };
            iface.SetLed(aLedId, aLedStatus);
        });
    }

    SetLedAuto(aCb: () => void) {
        this._call( function (iface){
            iface.SetLedAuto['finish'] = function() {
                aCb();
            };
            iface.SetLedAuto();
        });
    }

    SetLedCategory(aCategory: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetLedCategory['finish'] = function() {
                aCb();
            };
            iface.SetLedCategory(aCategory);
        });
    }

    ClearLed(aCb: () => void) {
        this._call( function (iface){
            iface.ClearLed['finish'] = function() {
                aCb();
            };
            iface.ClearLed();
        });
    }

    SetPanel(aPriority: number, aText: string, aPanelAlign: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPanel['finish'] = function() {
                aCb();
            };
            iface.SetPanel(aPriority, aText, aPanelAlign);
        });
    }

    SetPanelAuto(aPriority: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPanelAuto['finish'] = function() {
                aCb();
            };
            iface.SetPanelAuto(aPriority);
        });
    }

    SetPanelDimLevel(aPriority: number, aDimLevel: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPanelDimLevel['finish'] = function() {
                aCb();
            };
            iface.SetPanelDimLevel(aPriority, aDimLevel);
        });
    }

    SetPanelAnimation(aPriority: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPanelAnimation['finish'] = function() {
                aCb();
            };
            iface.SetPanelAnimation(aPriority);
        });
    }
}

export = COutputPanel