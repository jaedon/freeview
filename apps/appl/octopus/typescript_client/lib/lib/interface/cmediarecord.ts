/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMediaRecord extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Media.Record', '/Octopus/Appkit/Media/Record');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    IsRecording(aSvcUid: number, aCb: (aIsRecording: number) => void) {
        this._call( function (iface){
            iface.IsRecording['finish'] = function(aIsRecording: number) {
                aCb(aIsRecording);
            };
            iface.IsRecording(aSvcUid);
        });
    }

    GetCID(aSvcUid: number, aCb: (aCid: number) => void) {
        this._call( function (iface){
            iface.GetCID['finish'] = function(aCid: number) {
                aCb(aCid);
            };
            iface.GetCID(aSvcUid);
        });
    }

    Start(aMasterSvcUid: number, aSuppleSvcUid: number, aSuppSvcType: number, aStartOffset: number, aStartTime: number, aDuration : number, aStorageType: number, aStorageId: string, aEventId: number, aCb: (aSessionid: number) => void) {
        this._call( function (iface){
            iface.Start['finish'] = function(aSessionid: number) {
                aCb(aSessionid);
            };
            iface.Start((aMasterSvcUid,aSuppleSvcUid,aSuppSvcType,aStartOffset,aStartTime,aDuration,aStorageType,aStorageId,aEventId));
        });
    }

    Stop(aSessionid: number, aCb: () => void) {
        this._call( function (iface){
            iface.Stop['finish'] = function() {
                aCb();
            };
            iface.Stop(aSessionid);
        });
    }

    StopByUID(aSvcUid: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopByUID['finish'] = function() {
                aCb();
            };
            iface.StopByUID(aSvcUid);
        });
    }

    GetRecTime(aSessionid: number, aCb: (aRecTime: number) => void) {
        this._call( function (iface){
            iface.GetRecTime['finish'] = function(aRecTime: number) {
                aCb(aRecTime);
            };
            iface.GetRecTime(aSessionid);
        });
    }

    GetStartTime(aSessionid: number, aCb: (aStartTime: number) => void) {
        this._call( function (iface){
            iface.GetRecTime['finish'] = function(aStartTime: number) {
                aCb(aStartTime);
            };
            iface.GetStartTime(aSessionid);
        });
    }

    GetURL(aSessionid: number, aCb: (aUrl: string) => void) {
        this._call( function (iface){
            iface.GetURL['finish'] = function(aUrl: string) {
                aCb(aUrl);
            };
            iface.GetURL(aSessionid);
        });
    }

    GetRsvSlotByUID(aSvcUid: number, aCb: (aRsvSlot: number) => void) {
        this._call( function (iface){
            iface.GetRsvSlotByUID['finish'] = function(aRsvSlot: number) {
                aCb(aRsvSlot);
            };
            iface.GetRsvSlotByUID(aSvcUid);
        });
    }
}

export = CMediaRecord