/**
 * Created by hsseo on 14. 5. 15.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMetaSchedule extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Meta.Schedule', '/Octopus/Appkit/Meta/Schedule');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    GetSchedule(aSlot: number, aCb: (aSchedule: tdata.TSchedule) => void) {
        this._call( function (iface){
            iface.GetSchedule['finish'] = function(data) {
                aCb(tdata.convert_schedule(data));
            };
            iface.GetSchedule(aSlot);
        });
    }

    GetScheduleByIndex(aIndex: number, aCb: (aSchedule: tdata.TSchedule) => void) {
        this._call( function (iface){
            iface.GetScheduleByIndex['finish'] = function(data) {
                aCb(tdata.convert_schedule(data));
            };
            iface.GetScheduleByIndex(aIndex);
        });
    }

    GetScheduleByType(aRsvType: number, aIndex: number, aCb: (aSchedule: tdata.TSchedule) => void) {
        this._call( function (iface){
            iface.GetScheduleByType['finish'] = function(data) {
                aCb(tdata.convert_schedule(data));
            };
            iface.GetScheduleByType(aRsvType, aIndex);
        });
    }

    CountSchedule(aCb: (aCount: number) => void) {
        this._call( function (iface){
            iface.CountSchedule['finish'] = function(aCount: number) {
                aCb(aCount);
            };
            iface.CountSchedule();
        });
    }

    AddSchedule(aSchedule: tdata.TSchedule, aCb: (aSlot: number) => void) {
        this._call( function (iface){
            iface.AddSchedule['finish'] = function(aSlot: number) {
                aCb(aSlot);
            };
            iface.AddSchedule(aSchedule);
        });
    }

    UpdateSchedule(aSlot: number, aSchedule: tdata.TSchedule, aCb: () => void) {
        this._call( function (iface){
            iface.UpdateSchedule['finish'] = function() {
                aCb();
            };
            iface.UpdateSchedule(aSlot, aSchedule);
        });
    }

    CancelSchedule(aSlot: number, aCb: () => void) {
        this._call( function (iface){
            iface.CancelSchedule['finish'] = function() {
                aCb();
            };
            iface.CancelSchedule(aSlot);
        });
    }

    StopSchedule(aSlot: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopSchedule['finish'] = function() {
                aCb();
            };
            iface.StopSchedule(aSlot);
        });
    }

    PauseSchedule(aSlot: number, aCb: () => void) {
        this._call( function (iface){
            iface.PauseSchedule['finish'] = function() {
                aCb();
            };
            iface.PauseSchedule(aSlot);
        });
    }

    ResumeSchedule(aSlot: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResumeSchedule['finish'] = function() {
                aCb();
            };
            iface.ResumeSchedule(aSlot);
        });
    }

    GetConflictResult(aSlot: number, aCb: (aType: number, aSlotList: number[]) => void) {
        this._call( function (iface){
            iface.GetConflictResult['finish'] = function(data) {
                var slotList = [];
                data[1].forEach((g) => {
                    slotList.push(g);
                });
                aCb(data[0], slotList);
            };
            iface.GetConflictResult(aSlot);
        });
    }
}

export = CMetaSchedule