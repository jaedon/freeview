/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CMetaSchedule extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public GetSchedule(aSlot: number, aCb: (aSchedule: tdata.TSchedule) => void): void;
    public GetScheduleByIndex(aIndex: number, aCb: (aSchedule: tdata.TSchedule) => void): void;
    public GetScheduleByType(aRsvType: number, aIndex: number, aCb: (aSchedule: tdata.TSchedule) => void): void;
    public CountSchedule(aCb: (aCount: number) => void): void;
    public AddSchedule(aSchedule: tdata.TSchedule, aCb: (aSlot: number) => void): void;
    public UpdateSchedule(aSlot: number, aSchedule: tdata.TSchedule, aCb: () => void): void;
    public CancelSchedule(aSlot: number, aCb: () => void): void;
    public StopSchedule(aSlot: number, aCb: () => void): void;
    public PauseSchedule(aSlot: number, aCb: () => void): void;
    public ResumeSchedule(aSlot: number, aCb: () => void): void;
    public GetConflictResult(aSlot: number, aCb: (aType: number, aSlotList: number[]) => void): void;
}
export = CMetaSchedule;
