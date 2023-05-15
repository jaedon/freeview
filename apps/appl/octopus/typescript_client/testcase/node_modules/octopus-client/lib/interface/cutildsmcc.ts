/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilDsmcc extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Dsmcc', '/Octopus/Appkit/Util/Dsmcc');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    Create(aType: number, aCb:(aHandle: number, aCachePath: string) => void) {
        this._call(function (iface) {
            iface.Create['finish'] = function (data) {
                aCb(data[0], data[1]);
            };
            iface.Createa(aType);
        });
    }

    Destroy(aHandle: number, aCb:() => void) {
        this._call(function (iface) {
            iface.Destroy['finish'] = function () {
                aCb();
            };
            iface.Destroy(aHandle);
        });
    }

    GetCarouselList(aHandle: number, aCb:(aList: tdata.TCarouselItem[]) => void) {
        this._call(function (iface) {
            iface.GetCarouselList['finish'] = function (data) {
                var list = [];
                data[1].forEach((s) => {
                    list.push(tdata.convert_carouselitem(s));
                });
                aCb(list);
            };
            iface.GetCarouselList(aHandle);
        });
    }

    Start(aHandleDsmcc: number, aHandleCarousel: number, aCachePath: string, aCb:() => void) {
        this._call(function (iface) {
            iface.Start['finish'] = function () {
                aCb();
            };
            iface.Start(aHandleDsmcc, aHandleCarousel, aCachePath);
        });
    }

    ReStart(aHandleDsmcc: number, aHandleCarousel: number, aCb:() => void) {
        this._call(function (iface) {
            iface.ReStart['finish'] = function () {
                aCb();
            };
            iface.ReStart(aHandleDsmcc, aHandleCarousel);
        });
    }

    Stop(aHandleDsmcc: number, aHandleCarousel: number, aCb:() => void) {
        this._call(function (iface) {
            iface.Stop['finish'] = function () {
                aCb();
            };
            iface.Stop(aHandleDsmcc, aHandleCarousel);
        });
    }

    RequestEvent(aHandleDsmcc: number, aAll: number, aCb:() => void) {
        this._call(function (iface) {
            iface.RequestEvent['finish'] = function () {
                aCb();
            };
            iface.RequestEvent(aHandleDsmcc, aAll);
        });
    }

    Open(aHandleDsmcc: number, aHandleSession: number, aCb:() => void) {
        this._call(function (iface) {
            iface.Open['finish'] = function () {
                aCb();
            };
            iface.Open(aHandleDsmcc, aHandleSession);
        });
    }

    Close(aHandleDsmcc: number, aHandleSession: number, aCb:() => void) {
        this._call(function (iface) {
            iface.Close['finish'] = function () {
                aCb();
            };
            iface.Close(aHandleDsmcc, aHandleSession);
        });
    }
}

export = CUtilDsmcc