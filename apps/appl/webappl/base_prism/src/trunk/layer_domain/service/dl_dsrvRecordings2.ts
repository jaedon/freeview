/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvRecordings2.js
/*jslint sloppy:true, nomen:true, vars:true */
/*global hx */
/*global $ */
//    RECORDINGS: ['layer_domain/service/dl_dsrvRecordings2.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("model/model");
import __PROVIDER__ = require("pages/util/provider");

/**********************************************************
 * Definitions
 **********************************************************/
var _targetStorage = [//
    'MANUAL', //0
    'INTHDD', //1
    'INTSUBHDD', //2
    'EXTHDD'//3
];
var _repeatDaysTlb = {
    'NONE': 0,
    'SUN': 0x01, //0
    'MON': 0x02, //1
    'TUE': 0x04, //2
    'WED': 0x08, //3
    'THU': 0x10, //4
    'FRI': 0x20, //5
    'SAT': 0x40, //6
    'WEEKEND': 0x41,
    'WEEKDAYS': 0x3E,
    'ALL': 0x7F
};
var _factoryType = [//
    'RECORDING', //0
    'REMIND', //1
    'UNDEFINED', //2
    'UNDEFINED', //3
    'HTTP_LIVE_STREAMING', //4
    'UNDEFINED', //5
    'UNDEFINED', //6
    'UNDEFINED', //7
    'UNDEFINED', //8
    'UNDEFINED', //9
    'SERIES_RECORDING'//10
];
var _showType = [//
    'LIVE', //0
    'FIRST_RUN', //1
    'RERUN'//2
];
var _programmeIDType = [//
    'ID_TVA_CRID', //0
    'ID_DVB_EVENT', //1
    'ID_TVA_GROUP_CRID'//2
];
var _recordingStates = [//
    'RECORDING_UNREALIZED', //0
    'RECORDING_SCHEDULED', //1
    'RECORDING_DEL_SCHEDULED', //2
    'RECORDING_REC_PRESTART', //3
    'RECORDING_REC_ACQUIRING_RESOURCES', //4
    'RECORDING_REC_STARTED', //5
    'RECORDING_REC_UPDATED', //6
    'RECORDING_REC_COMPLETED', //7
    'RECORDING_REC_PARTIALLY_COMPLETED', //8
    'RECORDING_TS_ACQUIRING_RESOURCES', //9
    'RECORDING_TS_STARTED', //10
    'RECORDING_ERROR', //11
    'RECORDING_REC_START_ERROR', //12
    'RECORDING_REC_START_CONFLICT' //13
];
var _recordingError = [//
    'ERROR_REC_RESOURCE_LIMITATION', //0
    'ERROR_INSUFFICIENT_STORAGE', //1
    'ERROR_TUNER_CONFLICT', //2
    'ERROR_REC_DRM_RESTRICTION', //3
    'ERROR_REC_UNKNOWN', //4
    'ERROR_TS_RESOURCE_LIMITATION', //5
    'ERROR_TS_DRM_RESTRICTION', //6
    'ERROR_HAVE_ALTERNATIVE', //7
    'ERROR_TS_UNKNOWN', //8
    'ERROR_RFOTA', //9
    'ERROR_NO_AV', //10
    'ERROR_POWER_OFF',//11
    'ERROR_REC_CAS' //12
];
var fileOpState = 'IDLE';
//IDLE, REQ, COPY, MOVE, DELETE
var fnIsScheduled = function (obj) {
    return (!obj.id) ? true : false;
};
/**********************************************************
 * Functions
 **********************************************************/
class cRemovingList extends __MODEL__.cArray<any> {
    _ids = [];
    _items = [];

    constructor() {
        super();
    }

    push(item): number {
        this._ids.push(item.getId());
        return Array.prototype.push.call(this, item);
    }

    remove(index) {
        this._ids.splice(index, 1);
        Array.prototype.splice.call(this, index, 1);
    }

    indexOf(id) {
        return this._ids.indexOf(id);
    }
}
class cDLRecordings extends __KERNEL__ implements ifSVCRecordings {
    private oipfRecSchedulerObj: any = this.getPluginInstance('RecordingScheduler');
    private oipfVbObj: any = this.getPluginInstance('VideoBroadcast');
    private oipfConfigObj: any = this.getPluginInstance('Configuration');
    private mRecList;
    private mScheduledList;
    private mFileOpTicketQue = [];
    private mRecCtlStatEm: any = {
        'state': 'IDLE' //IDLE->REQ->FAIL/SUCCESS
    };
    private mRemovingTimer = null;
    private mDelayedRec = false;
    private mPvrReady = false;
    private mRemovingList = new cRemovingList();
    private mRecordingStopped = false;
    private __tpConflictParam = null;//checking for selecting tp conflicting http://svn:3000/issues/105958
    private mPrevRecommendationProgrammeId = null;
    private mRecordingListLoaded = false;

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.log('dsvc', 'init dsvc - hx.dl.dsrvRecordings');
        hx.logadd('dsrvRecordings2');
        hx.log('dsrvRecordings2', 'init dsvc - hx.dl.dsrvRecordings');
        this.defineEvents([//
            'RecordingItemAdded'//
            , 'RecordingItemUpdated'//
            , 'RecordingItemRemoved'//
            , 'RecordingListLoadingCompleted'//
            , 'ScheduledItemAdded'//
            , 'ScheduledItemUpdated'//
            , 'ScheduledItemRemoved'//
            , 'ScheduledWillBeStart'//
            , 'RecordingCompleted'//
            //recording control event
            , 'NotiRecordingStartError'//
            , 'NotiRecordingStarted'//
            , 'NotiRecordingStop'//
            , 'NotiScheduledStartError'//
            , 'NotiRecordingListRefresh'//
            , 'ShowChangeDuration'//
            , 'ShowRecordingStopWindow'//
            , 'ShowConflictWindow'//
            , 'NotiLiveStreamingStart'//
            , 'NotiLiveStreamingStop'//
            , 'ShowAlternativeWindow'
            , 'ShowHDAlternativeRecordWindow'
            //file operation events
            , 'RecordingCopying'//
            , 'RecordingCopyComplete'//
            , 'RecordingCopyError'//
            , 'RecordingCopyStoped'//
            , 'RecordingMoving'//
            , 'RecordingMoveComplete'//
            , 'RecordingMoveError'//
            , 'RecordingMoveStoped'//
            , 'RecordingCopyFailNotEnoughSpace'//
            , 'RecordingCopyFailSizeLimit4G'//
            , 'NotiFoundRecommendationProgramme'
            , 'NCDChanged'
            , 'ShowupTPConflictPopup'
            , 'ScheduledItemSet'
        ]);
        this.fnRefreshRecordingList();
        this.fnRefreshScheduleList();
        this.oipfRecSchedulerObj.addEventListener('FileOpEvent', (e) => {
            var ticket = e.ticket;
            var state = e.state;
            this.onFileOpEvent(ticket, state);
        });
        this.oipfRecSchedulerObj.addEventListener('PVREvent', (e) => {
            var state = e.state;
            var recording = e.recording;
            var error = e.error;
            var conflicts = e.scheduledRecordingConflicts;
            var recConflict = e.recordingConflicts;
            var alternatives = e.scheduledRecordingAlternatives;
            var altevents = e.scheduledRecordingAltEvents;
            this.onPVREvent(state, recording, error, conflicts, recConflict, alternatives, altevents);
        });
    }

    //private
    private fnTransitReqState(next, input?): boolean {
        hx.log('dsrvRecordings2', 'prev ' + this.mRecCtlStatEm.state + ' next ' + next + ' input ' + input);
        var bTransit = false;
        switch (this.mRecCtlStatEm.state) {
            case 'IDLE':
                if (next === 'REQ') {
                    hx.log('dsrvRecordings2', '[[IDLE->REQ]]');
                    this.mRecCtlStatEm.state = next;
                    this.mRecCtlStatEm.time = hx.il.nowSec();
                    this.mRecCtlStatEm.check = input.check;
                    this.mRecCtlStatEm.finished = input.finished;
                    this.mRecCtlStatEm.userdata = input.userdata;
                    this.mRecCtlStatEm.delay = input.delay;
                    this.mRecCtlStatEm.name = input.name;
                    this.mRecCtlStatEm.timeout = setTimeout(() => {
                        if (this.mRecCtlStatEm.state === 'REQ') {
                            this.fnTransitReqState('IDLE');
                        }
                    }, 6000);
                    bTransit = true;
                }
                break;
            case 'REQ':
                switch (next) {
                    case 'IDLE':
                        hx.log('dsrvRecordings2', '[[REQ->IDLE]]');
                        this.mRecCtlStatEm.state = next;
                        this.mRecCtlStatEm.check = undefined;
                        this.mRecCtlStatEm.finished = undefined;
                        this.mRecCtlStatEm.userdata = undefined;
                        this.mRecCtlStatEm.delay = undefined;
                        this.mRecCtlStatEm.name = undefined;
                        if (this.mRecCtlStatEm.timeout) {
                            clearTimeout(this.mRecCtlStatEm.timeout);
                            this.mRecCtlStatEm.timeout = undefined;
                        }
                        bTransit = true;
                        break;
                    case 'REQ':
                        hx.log('dsrvRecordings2', '[[REQ->...]]' + this.mRecCtlStatEm.name);
                        if (input.finished) {
                            input.finished(false);
                        }
                        break;
                    case 'FIN':
                        if (!this.mRecCtlStatEm.check || this.mRecCtlStatEm.check(input)) {
                            hx.log('dsrvRecordings2', '[[REQ->FIN]]');
                            var fnMakeIDLE = () => {
                                var delay = this.mRecCtlStatEm.delay;
                                if (delay) {
                                    this.mRecCtlStatEm.check = undefined;
                                    this.mRecCtlStatEm.finished = undefined;
                                    this.mRecCtlStatEm.userdata = undefined;
                                    this.mRecCtlStatEm.delay = undefined;
                                    if (this.mRecCtlStatEm.timeout) {
                                        clearTimeout(this.mRecCtlStatEm.timeout);
                                        this.mRecCtlStatEm.timeout = undefined;
                                    }
                                    setTimeout(() => {
                                        this.mRecCtlStatEm.name = undefined;
                                        this.mRecCtlStatEm.state = 'IDLE';
                                    }, delay);
                                } else {
                                    this.fnTransitReqState('IDLE');
                                }
                            };
                            if (this.mRecCtlStatEm.finished) {
                                var fn = this.mRecCtlStatEm.finished;
                                fnMakeIDLE();
                                fn(true);
                                bTransit = true;
                            } else {
                                fnMakeIDLE();
                            }
                        }
                        break;
                }
                break;
        }
        return bTransit;
    }

    private fnGetRecording(id, list?) {
        var i, ti, item, recList;
        if (list) {
            recList = list;
        } else {
            recList = this.mRecList;
        }
        for (i = 0, ti = recList.length; i < ti; i += 1) {
            item = recList[i];
            if (item.getId() === id) {
                return item;
            }
        }
    }

    private fnGetCurrentRecordings() {
        var i, ti, item, list = [];
        for (i = 0, ti = this.mRecList.length; i < ti; i += 1) {
            item = this.mRecList[i];
            if (item.isRecordingOnGoing()) {
                list.push(item);
            }
        }
        return list;
    }

    private fnGetSeriesList(groupCRIDs) {
        var i, j, k, ti, item, list = [];
        for (i = 0, ti = this.mRecList.length; i < ti; i += 1) {
            item = this.mRecList[i];
            var length = item.getGroupCRIDs() ? item.getGroupCRIDs().length : 0;
            var refLength = groupCRIDs ? groupCRIDs.length : 0;
            for (j = 0 ; j < length ; j++) {
                for (k = 0 ; k < refLength ; k++) {
                    if (item.getGroupCRIDs()[j] === groupCRIDs[k]) {
                        list.push(item);
                    }
                }
            }
        }
        return list;
    }

    private fnArrangeSeriesList(list) {
        var db = TAFFY([]);
        var i, item, startTime;
        var length = list.length;
        for (i = 0; i < length; i += 1) {
            item = list[i];
            startTime = item.getStartTime().getTime();
            db.insert({
                item: item,
                time: startTime,
                sort: startTime
            });
        }
        var fnMakeArray = (aDb) => {
            var i, length = aDb.length;
            var list = [];
            for (i = 0; i < length; i += 1) {
                list.push(aDb[i].item);
            }
            return list;
        };
        return fnMakeArray(db().order('sort').get());
    }

    private fnRefreshRecordingList() {
        var list;
        var _fnLoadList = (list) => {
            hx.log('dsrvRecordings2', '[fnRefreshRecordingList] Recording List Loading ... ');
            this.mRecList = new __MODEL__.COipfScheduledItemList(list, (item) => {
                return (item.getId()) ? true : false;
            }, (obj) => {
                return new __MODEL__.COipfRecordingItem(obj);
            });
        };
        //list = this.oipfRecSchedulerObj.recordings;
        list = this.oipfRecSchedulerObj.getRecordingList();
        if (list) {
            _fnLoadList(list);
        } else {
            _fnLoadList([]);
        }
        hx.log('dsrvRecordings2', '[fnRefreshRecordingList] this.mRecList.length : ' + this.mRecList.length);
    }

    private fnRefreshScheduleList() {
        var list;
        var _fnLoadList = (list) => {
            hx.log('dsrvRecordings2', '[fnRefreshScheduleList] Schedule List Loading ... ');
            var newItem;
            this.mScheduledList = new __MODEL__.COipfScheduledItemList(list, (item) => {
                return (!item.getId()) ? true : false;
            }, (obj) => {
                newItem = new __MODEL__.COipfScheduledItem(obj);
                if (!newItem.getProgrammeID()) {
                    newItem.setThumbnailUrl(null);
                }
                return newItem;
            });
        };
        //list = this.oipfRecSchedulerObj.recordings;
        list = this.oipfRecSchedulerObj.getScheduleList();
        if (list) {
            _fnLoadList(list);
        } else {
            _fnLoadList([]);
        }
        hx.log('dsrvRecordings2', '[fnRefreshScheduleList] this.mScheduledList.length : ' + this.mScheduledList.length);
        return;
    }

    private fnGetAllStreamingList() {
        var list = [];
        var i, ti = this.mScheduledList.length, item;
        for (i = 0; i < ti; i += 1) {
            item = this.mScheduledList[i];
            if (item.isStreaming()) {
                list.push(item);
            }
        }
        return list;
    }

    private fnRemoveScheduledRecording(param) {
        var items = param.items.slice(0);
        var finished = param.finished;
        //var delFirstSeries = param.delFirstSeries;
        var i, ti, item;
        for (i = 0, ti = items.length; i < ti; i += 1) {
            item = items.shift();
            var bRecStop;
            if (!item.isStreaming() && item.isRecordingOnGoing()) {
                var currentRecordingList = this.fnGetCurrentRecordings();
                var ccid = item.getChannelId();
                var j, tj, recording;
                for (j = 0, tj = currentRecordingList.length; j < tj; j += 1) {
                    recording = currentRecordingList[j];
                    if (recording.getChannelId() === ccid) {
                        this.oipfRecSchedulerObj.stop(recording.getInstance());
                        bRecStop = true;
                        break;
                    }
                }
            }
            if (!bRecStop) {
                //if (delFirstSeries) {
                //    this.oipfRecSchedulerObj.remove(item.getInstance(), item.isSeries());
                //} else {
                    this.oipfRecSchedulerObj.remove(item.getInstance());
                //}
            }
        }
        if (finished) {
            finished(true);
        }
    }

    private fnRemoveWholeSeriesSchedule(param) {
        var items = param.items.slice(0);
        var finished = param.finished;
        //var delFirstSeries = param.delFirstSeries;
        var i, ti, item;
        for (i = 0, ti = items.length; i < ti; i += 1) {
            item = items.shift();
            var bRecStop;
            if (!item.isStreaming() && item.isRecordingOnGoing()) {
                var currentRecordingList = this.fnGetCurrentRecordings();
                var ccid = item.getChannelId();
                var j, tj, recording;
                for (j = 0, tj = currentRecordingList.length; j < tj; j += 1) {
                    recording = currentRecordingList[j];
                    if (recording.getChannelId() === ccid) {
                        this.oipfRecSchedulerObj.stop(recording.getInstance());
                        bRecStop = true;
                        break;
                    }
                }
            }
            if (!bRecStop) {
                if (item._property.isScheduledAsSeries) {
                    this.oipfRecSchedulerObj.remove(item.getInstance(), true);
                } else {
                    this.oipfRecSchedulerObj.remove(item.getInstance());
                }
            }
        }
        if (finished) {
            finished(true);
        }
    }

    private fnStopRecording(param): void {
        var that = this;
        var finished = param.finished;
        var inputItems = (param.items) ? param.items.slice(0) : [param.item];
        var i, ti, items = [];
        var stopTimer;
        this.mRecordingStopped = false;
        for (i = 0, ti = inputItems.length; i < ti; i += 1) {
            if ((inputItems[i] instanceof __MODEL__.IScheduledItem) && inputItems[i].isRecordingOnGoing()) {
                items.push(inputItems[i]);
            }
        }
        var itemsLength = items.length;
        if (itemsLength === 0) {
            if (finished) {
                finished(false);
            }
            if (inputItems.length) {
                hx.log('dsrvRecordings2', 'no-sync => forced STOP');
                this.mRecList.remove(inputItems[0]);
                this.fireEvent('NotiRecordingStop', {
                    item : inputItems[0],
                    deleted : true
                });
                this.mRecordingStopped = true;
            }
            return;
        }
        var transit = this.fnTransitReqState('REQ', {
            name: 'StopRecording',
            check: (input) => {
                var state = input.state;
                var eventItem = input.eventItem;
                var items = this.mRecCtlStatEm.userdata.items;
                var max = this.mRecCtlStatEm.userdata.max;
                var userdata = this.mRecCtlStatEm.userdata;
                hx.log('dsrvRecordings2', '[this.fnStopRecording] state ' + state + ' input.bScheduled ' + input.bScheduled + ' max ' + max + ' validCount ' + userdata.validCount);
                if (!input.bScheduled) {
                    switch (state) {
                        case __MODEL__.eTRecordingState.ERECORDING_UNREALIZED:
                        case __MODEL__.eTRecordingState.ERECORDING_REC_PARTIALLY_COMPLETED:
                        case __MODEL__.eTRecordingState.ERECORDING_REC_COMPLETED:
                        case __MODEL__.eTRecordingState.ERECORDING_DEL_SCHEDULED:
                            clearTimeout(stopTimer);
                            this.mRecordingStopped = true;
                            this.fireEvent('NotiRecordingStop', {
                                item: eventItem,
                                deleted: (state === __MODEL__.eTRecordingState.ERECORDING_DEL_SCHEDULED)
                            });
                            userdata.validCount += 1;
                            hx.log('dsrvRecordings2', '\t userdata.validCount ' + userdata.validCount);
                            if (items.length) {
                                this.oipfRecSchedulerObj.stop((items.shift()).getInstance());
                            }
                            break;
                    }
                }
                return (userdata.validCount === max);
            },
            userdata: {
                items: items,
                max: items.length,
                validCount: 0
            },
            finished: finished
        });
        if (transit) {
            this.oipfRecSchedulerObj.stop((items.shift()).getInstance());
        }
        if (this.mRecordingStopped === false) {
            stopTimer = setTimeout(function() {
                if (inputItems.length && !that.mRecordingStopped) {
                    that.mRecList.remove(inputItems[0]);
                    that.fireEvent('NotiRecordingStop', {
                        item : inputItems[0],
                        deleted : true
                    });
                    that.mRecordingStopped = true;
                }
            }, 3000);
        }
    }

    private fnRecordAt(param): boolean {
        var finished = param.finished;
        var transit = this.fnTransitReqState('REQ', {
            name: 'RecordAt',
            delay: 1000,
            check: (input) => {
                /*var state = input.state;
                 var TRecordingState = __MODEL__.eTRecordingState;
                 switch(state) {
                 case TRecordingState.ERECORDING_SCHEDULED:
                 case TRecordingState.ERECORDING_DEL_SCHEDULED:
                 case TRecordingState.ERECORDING_UNREALIZED:
                 case TRecordingState.ERECORDING_ERROR:
                 case TRecordingState.ERECORDING_REC_START_CONFLICT:
                 case TRecordingState.ERECORDING_REC_START_ERROR:
                 break;
                 }*/
                return true;
            },
            finished: finished,
            userdata: {
                requestedAction: 'recordAt',
                scheduleInfo: param,
                retryWithDuration: (AdjustedDuration, scheduleInfo) => {
                    this.fnRecordAt({
                        startTime: scheduleInfo.startTime,
                        duration: AdjustedDuration,
                        repeatDays: scheduleInfo.repeatDays,
                        ccid: scheduleInfo.ccid,
                        factoryType: scheduleInfo.factoryType
                    });
                },
                resolveConflict: (cancelItems, scheduleInfo) => {
                    var stopItems = ( cancelItems instanceof Array) ? cancelItems.slice(0) : [cancelItems];
                    this.fnRemoveScheduledRecording({
                        items: stopItems,
                        //delFirstSeries: true,
                        finished: () => {
                            setTimeout(() => {
                                this.fnRecordAt({
                                    startTime: scheduleInfo.startTime,
                                    duration: scheduleInfo.duration,
                                    repeatDays: scheduleInfo.repeatDays,
                                    ccid: scheduleInfo.ccid,
                                    factoryType: scheduleInfo.factoryType
                                });
                            }, 500);
                        }
                    });
                }
            }
        });
        if (transit) {
            var startTime = param.startTime;
            var duration = param.duration;
            var repeatDays = param.repeatDays;
            var volume = param.volume;
            var ccid = param.ccid;
            var factoryType = param.factoryType;
            var returnScheduledRecording;
            hx.log('dsrvRecordings2', '[RecordAt] startTime ' + new Date(startTime * 1000));
            hx.log('dsrvRecordings2', '\t duration ' + duration);
            hx.log('dsrvRecordings2', '\t repeatDays ' + repeatDays);
            hx.log('dsrvRecordings2', '\t ccid ' + ccid);
            hx.log('dsrvRecordings2', '\t factoryType ' + factoryType);
            hx.log('dsrvRecordings2', '\t volume ' + volume);
            returnScheduledRecording = this.oipfRecSchedulerObj.recordAt(//
                startTime//
                , duration//
                , repeatDays//
                , ccid//
                , factoryType, 1, volume);
            if ((returnScheduledRecording === null) || (returnScheduledRecording === undefined)) {
                hx.log('dsrvRecordings2', '[RecordAt] invalid returnScheduledRecording');
                //this.fnTransitReqState('IDLE');
            }
        }
        return transit;
    }

    private fnRequestStartRecording(param): void {
        var hasHDAlternative = false;
        var _findMatchEvtfx = (tsrCurrentPos, result, callback) => {
            var firstProg, lastProg;
            if (result) {
                var i, ti;
                var programme;
                var timeInfo;
                var programmeTbl = [];
                var programmes: __PROVIDER__.CProgrammeProvider[] = result[0];
                var progLen = programmes.length;
                hx.log('dsrvRecordings2', '[findMatchEvtfx] @@@ tsrCurrentPos ' + (new Date(tsrCurrentPos * 1000)) + '(' + tsrCurrentPos + ')');
                if (programmes && progLen) {
                    hx.log('dsrvRecordings2', '[findMatchEvtfx] *** REFER EVENT LIST ***');
                    for (i = 0, ti = progLen; i < ti; i += 1) {
                        programme = programmes[i];
                        if (programme) {
                            programmeTbl.push({
                                'index': i,
                                'startTime': programme.getStartTime(),
                                'endTime': programme.getEndTime(),
                                'duration': programme.getDuration(),
                                'programme': programme
                            });
                            hx.log('dsrvRecordings2', '\t <' + i + '> [' + programme.getName() + ']');
                            hx.log('dsrvRecordings2', '\t startTime ' + (new Date(programme.getStartTime() * 1000)) + '(' + programme.getStartTime() + ')');
                            hx.log('dsrvRecordings2', '\t endTime ' + (new Date(programme.getEndTime() * 1000)) + '(' + programme.getEndTime() + ')');
                        }
                    }
                    for (i = 0, ti = programmeTbl.length; i < ti; i += 1) {
                        timeInfo = programmeTbl[i];
                        if (timeInfo) {
                            if ((tsrCurrentPos >= timeInfo.startTime) && (tsrCurrentPos <= timeInfo.endTime)) {
                                firstProg = timeInfo;
                                break;
                            }
                        }
                    }

                    if (!firstProg) {
                        var recStartTime = -1;
                        for (i = 0, ti = programmeTbl.length; i < ti; i += 1) {
                            timeInfo = programmeTbl[i];
                            if (timeInfo) {
                                if (tsrCurrentPos < timeInfo.endTime) {
                                    break;
                                }
                                recStartTime = timeInfo.endTime;
                            }
                        }
                        if (recStartTime !== -1) {
                            firstProg = {
                                'index': 0,
                                'startTime': recStartTime
                            };
                        }
                    }
                    var nowSec = hx.il.nowSec();
                    hx.log('dsrvRecordings2', '[_findMatchEvtfx] ' + (new Date(nowSec * 1000)) + '(' + nowSec + ')');
                    for (i = 0, ti = programmeTbl.length; i < ti; i += 1) {
                        timeInfo = programmeTbl[i];
                        if (timeInfo) {
                            if (nowSec >= timeInfo.startTime && nowSec <= timeInfo.endTime) {
                                //minimum duration 10min && pr_endtime remain over 10min
                                if ((timeInfo.duration >= 600) && ((timeInfo.endTime - tsrCurrentPos) >= 600)) {
                                    // 10min
                                    lastProg = timeInfo;
                                    break;
                                }
                            }
                        }
                    }
                    if (!lastProg) {
                        //after 2 hour
                        var lastLimitTime = nowSec + 7200;
                        var startIdx = 0;
                        if (firstProg) {
                            startIdx = firstProg.index;
                        }
                        for (i = startIdx, ti = programmeTbl.length; i < ti; i += 1) {
                            timeInfo = programmeTbl[i];
                            if (timeInfo) {
                                if (lastLimitTime < timeInfo.startTime) {
                                    break;
                                }
                                if ((nowSec < timeInfo.startTime)//
                                    && (timeInfo.duration >= 600))//
                                    //&& (lastLimitTime > timeInfo.endTime)) {
                                {
                                    lastProg = timeInfo;
                                    break;
                                }
                            }
                        }
                    }
                }
                programmeTbl = undefined;
                programme = undefined;
                programmes = undefined;
                result = undefined;
            }
            var countWait = 0;
            var ret = {
                firstProgramme: firstProg,
                lastProgramme: lastProg
            };
            var _fnGetLinkageInfo = function(aIdentifier: string, aProgramme: __PROVIDER__.CProgrammeProvider, aCallback): boolean {
                var willRunCallback = false;
                var __fnIsSupport = function(aOP: number): boolean {
                    return aOP === ENV.listOP.UKDTT;
                };
                if (__fnIsSupport(ENV.op)) {
                    var linkage = aProgramme.getHDLinkage();
                    if (linkage.length) {
                        var channel = hx.svc.CH_LIST.getChannel({
                            ccid: linkage[0].channel
                        });
                        if (channel.result) {
                            willRunCallback = true;
                            hx.svc.EPG.findProgrammes({
                                channel: channel.channel,
                                clause: 'programmeID == ' + linkage[0].event,
                                cb: function(aResult,b,c) {
                                    var programmes: __PROVIDER__.CProgrammeProvider[] = aResult[0];
                                    if (hx.emulMode) {
                                        programmes.push(item);
                                    }
                                    if (programmes.length) {
                                        aCallback(aIdentifier, programmes[0], linkage[0].simul);
                                    } else {
                                        prism.log("linkaged detected but can't find HD programme");
                                        aCallback(aIdentifier, null, linkage[0].simul);
                                    }
                                }
                            })
                        }
                    }
                }
                return willRunCallback;
            };
            var _fnIfLinkageInfoExist = function(aIdentifier: string, aHDProgramme: __PROVIDER__.CProgrammeProvider, aSimulcast: boolean) {
                var ref = ret[aIdentifier];
                ref.programmeSD = ref.programme;
                ref.programme = aHDProgramme;
                ref.simul = aSimulcast;
                if (!aSimulcast) {
                    ref.startTime = aHDProgramme.getStartTime();
                    ref.endTime = aHDProgramme.getEndTime();
                    ref.duration = aHDProgramme.getDuration();
                }
                if (--countWait === 0) {
                    hx.log("debug", "--countWait: " + countWait + " found HD Programme, simulcast" + aSimulcast);
                    hx.log("debug", "typeof simulcast" + typeof aSimulcast);
                    callback(ret);
                }
            };
            if (firstProg &&
                firstProg.programme &&
                _fnGetLinkageInfo("firstProgramme", firstProg.programme, _fnIfLinkageInfoExist)) {
                hasHDAlternative = true;
//                targetChannelId = firstProg.programme.ccid;
                countWait++;
            }
            if (lastProg &&
                lastProg.programme &&
                _fnGetLinkageInfo("lastProgramme", lastProg.programme, _fnIfLinkageInfoExist)) {
                countWait++;
            }
            if (countWait === 0) {
                callback(ret);
            }
        };
        var _fnRecordNow = (param) => {
            var duration = param.duration;
            var finished = param.finished;
            var transit = this.fnTransitReqState('REQ', {
                name: 'RecordNow',
                delay: 1000,
                check: (input) => {
                    var state = input.state;
                    var TRecordingState = __MODEL__.eTRecordingState;
                    var valid = false;
                    switch (state) {
                        case TRecordingState.ERECORDING_REC_STARTED:
                        case TRecordingState.ERECORDING_DEL_SCHEDULED:
                        case TRecordingState.ERECORDING_UNREALIZED:
                        case TRecordingState.ERECORDING_ERROR:
                        case TRecordingState.ERECORDING_REC_START_CONFLICT:
                        case TRecordingState.ERECORDING_REC_START_ERROR:
                            valid = true;
                            break;
                    }
                    return valid;
                },
                finished: finished,
                userdata: {
                    requestedAction: 'recordNow',
                    scheduleInfo: {
                        duration: duration
                    },
                    retryWithDuration: (AdjustedDuration, scheduleInfo) => {
                        _fnRecordNow({
                            duration: AdjustedDuration
                        });
                    },
                    resolveConflict: (cancelItems, scheduleInfo) => {
                        var stopItems = ( cancelItems instanceof Array) ? cancelItems.slice(0) : [cancelItems];
                        var ulSetTimeOut = 0;
                        this.fnRemoveScheduledRecording({
                            items: stopItems,
                            //delFirstSeries: true,
                            finished: () => {
                                setTimeout(() => {
                                    var duration = scheduleInfo.duration;
                                    _fnRecordNow({
                                        duration: duration
                                    });
                                }, 300);
                            }
                        });
                    }
                }
            });
            if (transit) {
                var recordId = this.oipfVbObj.recordNow(duration);
                if ((recordId === null) || (recordId === undefined)) {
                    hx.log('dsrvRecordings2', '[RecordNow] invalid returnScheduledRecording');
                    //this.fnTransitReqState('IDLE');
                }
            }
            return transit;
        };
        var _fnDelayedRecording = (param) => {
            var tsrPosOffset = param.tsrPosOffset;
            var duration = param.duration;
            var finished = param.finished;
            var transit = this.fnTransitReqState('REQ', {
                name: 'DelayedRecording',
                delay: 1000,
                check: (input) => {
                    var state = input.state;
                    var TRecordingState = __MODEL__.eTRecordingState;
                    var valid = false;
                    switch (state) {
                        case TRecordingState.ERECORDING_REC_STARTED:
                        case TRecordingState.ERECORDING_DEL_SCHEDULED:
                        case TRecordingState.ERECORDING_UNREALIZED:
                        case TRecordingState.ERECORDING_ERROR:
                        case TRecordingState.ERECORDING_REC_START_CONFLICT:
                        case TRecordingState.ERECORDING_REC_START_ERROR:
                            valid = true;
                            break;
                    }
                    return valid;
                },
                finished: finished,
                userdata: {
                    requestedAction: 'recordDelayed',
                    scheduleInfo: {
                        tsrPosOffset: tsrPosOffset,
                        duration: duration
                    },
                    retryWithDuration: (AdjustedDuration, scheduleInfo) => {
                        _fnDelayedRecording({
                            tsrPosOffset: scheduleInfo.tsrPosOffset,
                            duration: AdjustedDuration
                        });
                    },
                    resolveConflict: (cancelItems, scheduleInfo) => {
                        var stopItems = ( cancelItems instanceof Array) ? cancelItems.slice(0) : [cancelItems];
                        this.fnRemoveScheduledRecording({
                            items: stopItems,
                            //delFirstSeries: true,
                            finished: () => {
                                setTimeout(() => {
                                    var tsrPosOffset = scheduleInfo.tsrPosOffset;
                                    var duration = scheduleInfo.duration;
                                    _fnDelayedRecording({
                                        tsrPosOffset: tsrPosOffset,
                                        duration: duration
                                    });
                                }, 300);
                            }
                        });
                    }
                }
            });
            if (transit) {
                var recordId = this.oipfVbObj.recordDelayed(tsrPosOffset, duration);
                if ((recordId === null) || (recordId === undefined)) {
                    hx.log('dsrvRecordings2', '[recordDelayed] invalid returnScheduledRecording');
                    //this.fnTransitReqState('IDLE');
                }
            }
            return transit;
        };
        var _fnQueryEventInfo = (tsrBufferSize, tsrPosOffset, targetChannel, requestCallback) => {
            var nowSec = hx.il.nowSec();
            hx.log('dsrvRecordings2', '[_fnQueryEventInfo] ' + (new Date(nowSec * 1000)) + '(' + nowSec + ')');
            var tsrBufStart = nowSec - tsrBufferSize;
            var tsrCurrentPos = tsrBufStart + tsrPosOffset;
            var epgQueryRet;

            epgQueryRet = hx.svc.EPG.getLiveProgrammes({
                'channel': targetChannel,
                'startTime': nowSec - tsrBufferSize,
                'endTime': nowSec + ((2 * 60 + 30) * 60)
            });


            var isTragetChannelWithCurrentChannel = false;
            if (epgQueryRet && epgQueryRet.length && epgQueryRet[0].length&&epgQueryRet[0][0]) {
                if(targetChannel.ccid  == epgQueryRet[0][0].channel.ccid){
                    isTragetChannelWithCurrentChannel = true;//check current channel
                }
            }

            if (epgQueryRet && epgQueryRet.length && epgQueryRet[0].length && isTragetChannelWithCurrentChannel) {
                _findMatchEvtfx(tsrCurrentPos, epgQueryRet, function(eventInfo) {
                    requestCallback(eventInfo);
                });
            } else {
                epgQueryRet = hx.svc.EPG.getProgrammes({
                    'channel': targetChannel,
                    'isOnAir': false,
                    'startTime': nowSec - tsrBufferSize,
                    'endTime': nowSec + ((2 * 60 + 30) * 60),
                    'listener': this,
                    'cb': (result) => {
                        if (epgQueryRet) {
                            _findMatchEvtfx(tsrCurrentPos, result, function(eventInfo) {
                                requestCallback(eventInfo);
                            });
                        }
                        result = undefined;
                    }
                });
                if (!epgQueryRet) {
                    requestCallback();
                }
            }
        };
        var _fnShowChangeDurationWindow = (relatedRecordingItem) => {
            hx.log('dsrvRecordings2', 'cur channel is recording');
            var i, ti, item, relatedScheduledRecording;
            var targetChannel = relatedRecordingItem.getChannelInstance();
            var targetChannelId = relatedRecordingItem.getChannelId();
            for (i = 0, ti = this.mScheduledList.length; i < ti; i += 1) {
                item = this.mScheduledList[i];
                if (item.isRecordingOnGoing() && (item.getChannelId() === targetChannelId)) {
                    relatedScheduledRecording = item;
                    break;
                }
            }
            if (!relatedScheduledRecording) {
                hx.log('error', 'NOT found schedule');
            } else {
                var epgQueryData, epgQueryRet;
                //TODO: need check padding time
                //TODO: need check padding time
                //TODO: need check padding time
                //TODO: need check padding time
                //TODO: need check padding time
                var recordingStartTimeSec = Math.floor((relatedScheduledRecording.getStartTime() / 1000) + 30);
                epgQueryData = {
                    'listener': this,
                    'channel': targetChannel,
                    'startTime': recordingStartTimeSec,
                    'endTime': recordingStartTimeSec + 86400, //24 hour
                    'isOnAir': false,
                    'cb': (result) => {
                        if (!epgQueryRet) {
                            return;
                        }
                        var evtList;
                        var currentEvt, currentEvtIdx = -1;
                        if (result && result.length) {
                            var endTime = recordingStartTimeSec + 86400;
                            var evt;
                            var i, ti;
                            evtList = result[0].slice(0);
                            //emit after 24hour event
                            for (i = evtList.length - 1; i >= 0; i -= 1) {
                                evt = evtList[i];
                                if (evt) {
                                    if (evt.getEndTime() <= endTime) {
                                        break;
                                    }
                                    evtList.pop();
                                }
                            }
                            //find current event
                            var endPaddingTime = hx.svc.SETTING_UTIL.getPvrEndPadding();
                            var recEndTime = recordingStartTimeSec + relatedScheduledRecording.getDuration() - endPaddingTime;
                            for (i = 0, ti = evtList.length; i < ti; i += 1) {
                                evt = evtList[i];
                                if (Math.abs(recEndTime - evt.getEndTime()) < 60) {
                                    currentEvtIdx = i;
                                    currentEvt = evt;
                                    break;
                                }
                            }
                        }
                        this.fireEvent('ShowChangeDuration', {
                            relatedScheduledRecording: relatedScheduledRecording,
                            relatedRecordingItem: relatedRecordingItem,
                            targetChannel: targetChannel,
                            eventList: evtList,
                            currentEvnet: currentEvt,
                            currentEventIndex: currentEvtIdx
                        });
                    }
                };
                epgQueryRet = hx.svc.EPG.getProgrammes(epgQueryData);
                if (!epgQueryRet) {
                    this.fireEvent('ShowChangeDuration', {
                        relatedScheduledRecording: relatedScheduledRecording,
                        relatedRecordingItem: relatedRecordingItem,
                        targetChannel: targetChannel,
                        eventList: []
                    });
                }
            }
        };
        var finished = param.finished;
        var targetChannelId = param.ccid;
        var bCurrentChRec = false;
        var tsrPosOffset = 0, tsrBufferSize = 0;
        var targetChannel = undefined, currentChannel = undefined;
        if (!targetChannelId) {
            bCurrentChRec = true;
            currentChannel = (hx.svc.CH_LIST.getCurrentChannel()).channel;
            targetChannel = currentChannel;
            targetChannelId = targetChannel.ccid;
            /* If Delaed Recording is not supported, set tsrPosOffset and tsrBufferSize to zero*/
            if (hx.config.useDelayedRec) {
                tsrPosOffset = param.tsrPosOffset;
                tsrBufferSize = param.tsrBufferSize;
            } else {
                tsrPosOffset = 0;
                tsrBufferSize = 0;
            }
            hx.log('dsrvRecordings2', '@@@ request target channelid ' + targetChannelId);
            hx.log('dsrvRecordings2', '@@@ request target channel name ' + targetChannel.name);
        } else {
            var getChannelRet = (hx.svc.CH_LIST.getChannel({
                'group': 'ALL',
                'ccid': targetChannelId
            }));
            if (getChannelRet && getChannelRet.result) {
                targetChannel = getChannelRet.channel;
                targetChannelId = targetChannel.ccid;
            }
            if (targetChannel) {
                hx.log('dsrvRecordings2', '@@@ request target channelid ' + targetChannelId);
                hx.log('dsrvRecordings2', '@@@ request target channel name ' + targetChannel.name);
                currentChannel = (hx.svc.CH_LIST.getCurrentChannel()).channel;
                if (targetChannel.ccid === currentChannel.ccid) {
                    bCurrentChRec = true;
                }
            }
        }
        if (!targetChannel) {
            hx.log('error', 'NOT found channel info');
            if (finished) {
                finished(false, 'NO_CHANNEL');
            }
            return;
        }
        var relatedRecordingItem;
        var currentRecordings = this.fnGetCurrentRecordings();
        var i, ti, item;
        for (i = 0, ti = currentRecordings.length; i < ti; i += 1) {
            item = currentRecordings[i];
            if (targetChannelId === item.getChannelId()) {
                relatedRecordingItem = item;
                break;
            }
        }

        if (relatedRecordingItem) {
            if (finished) {
                finished(false, 'RECORDING_NOW');
            }
            _fnShowChangeDurationWindow(relatedRecordingItem);
            return;
        }

        var needToShowupTuneWindow = false;
        if(this.oipfVbObj.checkChannelRecordable(targetChannel,false)){
            needToShowupTuneWindow = false;
        }else{
            if(this.oipfVbObj.checkChannelRecordable(targetChannel,true)){
                needToShowupTuneWindow = true;
            }else{
                needToShowupTuneWindow = false;//unavailable to do reservation , check scenario. maybe it will be failed after trying record. follow MW scenario.
            }
        }

        var def = Q.defer();
        var promise = def.promise;
        var bDelayedRec = (tsrBufferSize >= 10);

        this.mDelayedRec = bDelayedRec;
        _fnQueryEventInfo(tsrBufferSize, tsrPosOffset, targetChannel, (eventInfo) => {
            var duration = 7200;
            var Trashduration = 10;
            var firstProgramme = eventInfo.firstProgramme;
            var lastProgramme = eventInfo.lastProgramme;
            hx.log('dsrvRecordings2', '[reqRecStartfx] display param ---> ');
            hx.log('dsrvRecordings2', '\t duration ' + duration + ' sec');
            hx.log('dsrvRecordings2', '\t bDelayedRec ' + bDelayedRec);
            hx.log('dsrvRecordings2', '\t bCurrentChRec ' + bCurrentChRec + ' targetChannelId ' + targetChannelId);
            hx.log('dsrvRecordings2', '\t tsrPosOffset ' + tsrPosOffset + ' sec');
            hx.log('dsrvRecordings2', '\t tsrBufferSize ' + tsrBufferSize + ' sec');
            if (firstProgramme) {
                if (firstProgramme.programme) {
                    hx.log('dsrvRecordings2', '\t firstProg ' + firstProgramme.programme.name);
                }
                hx.log('dsrvRecordings2', '\t\t duration ' + firstProgramme.duration + 'sec');
                hx.log('dsrvRecordings2', '\t\t startTime ' + firstProgramme.startTime + 'sec ' + (new Date(firstProgramme.startTime * 1000)));
                hx.log('dsrvRecordings2', '\t\t endTime ' + firstProgramme.endTime + 'sec ' + (new Date(firstProgramme.endTime * 1000)));
            }
            if (lastProgramme) {
                if (lastProgramme.programme) {
                    hx.log('dsrvRecordings2', '\t lastProg ' + lastProgramme.programme.name);
                }
                hx.log('dsrvRecordings2', '\t\t duration ' + lastProgramme.duration + 'sec');
                hx.log('dsrvRecordings2', '\t\t startTime ' + lastProgramme.startTime + 'sec ' + (new Date(lastProgramme.startTime * 1000)));
                hx.log('dsrvRecordings2', '\t\t endTime ' + lastProgramme.endTime + 'sec ' + (new Date(lastProgramme.endTime * 1000)));
            }
            hx.log('dsrvRecordings2', '\t bCurrentChRec ' + bCurrentChRec + ' targetChannelId ' + targetChannelId);
            function getStartTime(time){
                return new Date(time * 1000);
            }

            function getEndTIme(startTIme, duration){
                return new Date((startTIme + duration) * 1000);
            }

            var _fnRecord = () => {
                if (bDelayedRec) {
                    var startOffset = 0;
                    if (!firstProgramme && !lastProgramme) {
                        hx.log('dsrvRecordings2', '\t >>> NO EVENT INFO <<<');
                        duration = tsrBufferSize + 7200;
                        hx.log('dsrvRecordings2', '\t\t Normal Delay Recording CASE ... adjust startOffset ' + startOffset + ' duration ' + duration);
                    } else {
                        hx.log('dsrvRecordings2', '\t >>> REFER EVENT <<<');
                        if (firstProgramme) {
                            startOffset = tsrBufferSize - (hx.il.nowSec() - Trashduration - firstProgramme.startTime);
                            hx.log('dsrvRecordings2', '\t\t refer firstProg adjust startOffset ' + startOffset + ' OR ZERO');
                            if (startOffset < 0) {
                                startOffset = 0;
                                hx.log('dsrvRecordings2', '\t\t adjust startOffset ' + startOffset);
                            } else {
                                if (tsrPosOffset < startOffset) {
                                    startOffset = tsrPosOffset;
                                    hx.log('dsrvRecordings2', '\t\t adjust startOffset ' + startOffset);
                                }
                            }
                        } else {
                            startOffset = tsrPosOffset;
                            hx.log('dsrvRecordings2', '\t\t adjust startOffset ' + startOffset);
                        }
                        if (lastProgramme) {
                            duration = lastProgramme.endTime - hx.il.nowSec() - Trashduration;
                            hx.log('dsrvRecordings2', '\t\t 1 refer lastProg adjust duration ' + duration);
                            duration = (tsrBufferSize - startOffset) + duration;
                            hx.log('dsrvRecordings2', '\t\t 3 refer lastProg adjust duration ' + duration);
                        } else {
                            duration = duration - (tsrBufferSize - startOffset);
                            hx.log('dsrvRecordings2', '\t\t adjust duration ' + duration);
                        }
                        if (duration <= (tsrBufferSize - startOffset)) {
                            hx.log('error', '\t invalid duration ' + duration);
                            duration = (tsrBufferSize - startOffset) + 600;
                            hx.log('error', '\t adjust invalid duration ' + duration);
                        }
                    }
                    _fnDelayedRecording({
                        tsrPosOffset: startOffset,
                        duration: duration,
                        finished: finished
                    });
                } else {
                    var nowSec;
                    if (firstProgramme && lastProgramme && firstProgramme.programme && lastProgramme.programme//
                        && (firstProgramme.programme.programmeID === lastProgramme.programme.programmeID)) {
                        hx.log('dsrvRecordings2', '\t >>> TBR REC with EVENT INFO <<<');
                        duration = firstProgramme.programme.startTime + firstProgramme.programme.duration - hx.il.nowSec() - Trashduration;
                        //var remainSec = duration % 60;
                        //hx.log('dsrvRecordings2', '\t\t remainSec ' + remainSec + ' duration ' + duration);
                        //duration = (duration - remainSec) + ((remainSec > 30) ? 60 : 0);
                        hx.log('dsrvRecordings2', '\t\t adjust duration ' + duration);
                        if (bCurrentChRec && hx.svc.AV_CTRL.isLiveOnGoing()) { //check VideoBroadcast Status
                            hx.log('dsrvRecordings2', '\t\t CURRENT CHANNEL RECORDNOW duration ' + duration);
                            _fnRecordNow({
                                duration: duration,
                                finished: finished
                            });
                        } else {
                            //check tuning
                            nowSec = hx.il.nowSec() - Trashduration;
                            var self = this;
                            promise.done(function(result){
                                hx.log('dsrvRecordings2', '\t\t ADDSCHEDULE duration ' + duration);
                                self.__tpConflictParam = result;
                                self.fnRecordAt({
                                    startTime: nowSec,
                                    duration: duration,
                                    repeatDays: 0,
                                    ccid: targetChannelId,
                                    factoryType: __MODEL__.eTScheduledFactoryType.ERECORDING,
                                    finished: finished,
                                });
                            });
                            if(needToShowupTuneWindow){
                                this.fireEvent('ShowupTPConflictPopup',{
                                    title : hx.l('LOC_RECWILLSTARTMESSAGE_ID'),
                                    eventName : firstProgramme.programme.name,
                                    channelNo : firstProgramme.programme.getChannelNum(),
                                    channelName : firstProgramme.programme.channel.name,
                                    startTime : getStartTime(nowSec),
                                    endTime : getEndTIme(nowSec, duration),
                                    resultDef : def
                                });
                            }else{
                                def.resolve(null);
                            }
                        }
                    } else {
                        hx.log('dsrvRecordings2', '\t >>> NO EVENT INFO <<<');
                        if (lastProgramme) {
                            duration = lastProgramme.endTime - hx.il.nowSec() - Trashduration;
                            hx.log('dsrvRecordings2', '\t\t adjust duration ' + duration);
                        }
                        if (bCurrentChRec && hx.svc.AV_CTRL.isLiveOnGoing()) {
                            hx.log('dsrvRecordings2', '\t\t CURRENT CHANNEL RECORDNOW duration ' + duration);
                            _fnRecordNow({
                                duration: duration,
                                finished: finished
                            });
                        } else {
                            nowSec = hx.il.nowSec() - Trashduration;
                            hx.log('dsrvRecordings2', '\t\t ADDSCHEDULE duration ' + duration);
                            var self = this;
                            promise.done(function(result){
                                self.__tpConflictParam = result;
                                self.fnRecordAt({
                                    startTime: nowSec,
                                    duration: duration,
                                    repeatDays: 0,
                                    ccid: targetChannelId,
                                    factoryType: __MODEL__.eTScheduledFactoryType.ERECORDING,
                                    finished: finished
                                });
                            });
                            if(needToShowupTuneWindow){
                                this.fireEvent('ShowupTPConflictPopup',{
                                    title : hx.l('LOC_RECWILLSTARTMESSAGE_ID'),
                                    eventName : firstProgramme.programme.name,
                                    channelNo : firstProgramme.programme.getChannelNum(),
                                    channelName : firstProgramme.programme.channel.name,
                                    startTime : getStartTime(nowSec),
                                    endTime : getEndTIme(nowSec, duration),
                                    resultDef : def
                                });
                            }else{
                                def.resolve(null);
                            }
                        }
                    }
                }
            };
            /* Live HD Alternative */
            //if (hasHDAlternative) {
            //hx.log('debug', '\t\t hasHDAlternative: ' + hasHDAlternative);
            //hx.log('debug', '\t\t simulcast: ' + firstProgramme.simul);
            //this.fireEvent("ShowHDAlternativeRecordWindow", [
            //    {
            //        text: firstProgramme.programme.getName(),
            //        fn: ()=> {
            //            this.record({
            //                programme: firstProgramme.programme.getPluginObject(),
            //                factoryType: __MODEL__.eTScheduledFactoryType.ERECORDING,
            //                finished: function () {
            //                    hx.log('debug', '\t\t instance recording HD Alternative');
            //                }
            //            });
            //        }
            //    },
            //    {
            //        text: firstProgramme.programmeSD.getName(),
            //        fn: function () {
            //            hx.log('debug', '\t\t instance recording select SD');
            //    _fnRecord();
            //        }
            //    }
            //]);
            /* Live HD Alternative */
            //} else {
            _fnRecord();
            //}
        });
    }

    private fnSearchTicket(list, fnComp): any {
        var i, ti, ticketInfo;
        for (i = 0, ti = list.length; i < ti; i += 1) {
            ticketInfo = list[i];
            if (fnComp(ticketInfo)) {
                return ticketInfo;
            }
        }
    }

    private fnGetErrorText(error): string {
        var TRecordingError = __MODEL__.eTRecordingError;
        var errText = '';
        switch (error) {
            case TRecordingError.ERROR_INSUFFICIENT_STORAGE:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_STORAGE_FULL');
                break;
            case TRecordingError.ERROR_TUNER_CONFLICT:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_CONFLICT');
                break;
            case TRecordingError.ERROR_REC_DRM_RESTRICTION:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_UNKNOWN'); //temporary //hx.l('LOC_VIDEO_RECORD_FAIL_DRM');
                break;
            case TRecordingError.ERROR_REC_UNKNOWN:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_UNKNOWN');
                break;
            case TRecordingError.ERROR_NO_AV:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_NO_SIGNAL');
                break;
            case TRecordingError.ERROR_POWER_OFF:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_POWER_FAILURE');
                break;
            case TRecordingError.ERROR_REC_CAS:
                if ((ENV.listCAS.IRUC === ENV.cas) && (hx.config.useUCRecordingLimit !== undefined)) {
                    if (hx.config.useUCRecordingLimit === true) {
                        errText = hx.l('LOC_CAS_IR_UC_MSG_E130_PVR_NOT_ALLOWED');
                    } else {
                        errText = hx.l('LOC_VIDEO_RECORD_FAIL_UNKNOWN');
                    }
                } else {
                    errText = hx.l('LOC_VIDEO_RECORD_FAIL_UNKNOWN');
                }
                break;
            case TRecordingError.ERROR_RFOTA:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_UNKNOWN');
                break;
            case TRecordingError.ERROR_HIGHER_PRIORITY:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_HIGHER');
                break;
            case TRecordingError.ERROR_TRACKING_FAIL:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_TRACKING');
                break;
            case TRecordingError.ERROR_OFF_AIR:
                errText = hx.l('LOC_VIDEO_RECORD_FAIL_OFFAIR');
                break;
            case TRecordingError.ERROR_REC_RESOURCE_LIMITATION:
            case TRecordingError.ERROR_TS_RESOURCE_LIMITATION:
            case TRecordingError.ERROR_TS_DRM_RESTRICTION:
            case TRecordingError.ERROR_HAVE_ALTERNATIVE:
            case TRecordingError.ERROR_TS_UNKNOWN:
                break;
        }
        return errText;
    }

    //public
    isRecordingListLoaded() : boolean {
        return this.mRecordingListLoaded;
    }
    refreshRecordingList(): void {
        var refreshStartTime = (new Date()).getTime();
        this.oipfRecSchedulerObj.refresh();
        hx.log('dsrvRecordings2', 'refresh ' + ((new Date()).getTime() - refreshStartTime));
        this.fnRefreshRecordingList();
    }

    refreshScheduleList(): void {
        this.fnRefreshScheduleList();
    }

    getScheduledList(): any {
        return this.mScheduledList;
    }

    getRecordingList(): any {
        return this.mRecList;
    }

    getRecording(param): any {
        var id = param.id;
        return this.fnGetRecording(id);
    }

    getCurrentRecordings(): any {
        return this.fnGetCurrentRecordings();
    }

    getSeriesList(groupCRIDs): any {
        return this.fnGetSeriesList(groupCRIDs);
    }

    arrangeSeriesList(list): any {
        return this.fnArrangeSeriesList(list);
    }

    getAllStreamingList(): any {
        return this.fnGetAllStreamingList();
    }

    getFileOperationInfo(): any {
        var fileOperationInfo;
        if (this.mFileOpTicketQue.length) {
            fileOperationInfo = this.mFileOpTicketQue[0];
        }
        return fileOperationInfo;
    }

    getErrorText(recError): string {
        return this.fnGetErrorText(recError);
    }

    isDelayedRecStarted(): boolean {
        return this.mDelayedRec;
    }

    setManualLocked(param): boolean {
        var items = param.items.slice(0);
        var finished = param.finished;
        var lock = param.lock;
        var transit = this.fnTransitReqState('REQ', {
            name: 'setManualLocked',
            check: (state) => {
                var items = this.mRecCtlStatEm.userdata.items;
                var lock = this.mRecCtlStatEm.userdata.lock;
                if (items.length) {
                    (items.shift()).setManualLock(lock, this.oipfRecSchedulerObj);
                }
                return (items.length === 0);
            },
            userdata: {
                items: items,
                lock: lock
            },
            finished: finished
        });
        if (transit) {
            (items.shift()).setManualLock(lock, this.oipfRecSchedulerObj);
        }
        //TODO:lock/unlock related series items
        return transit;
    }

    setKeep(param): boolean {
        var items = param.items.slice(0);
        var finished = param.finished;
        var keep = param.keep;
        var i;
        var transit = this.fnTransitReqState('REQ', {
            name: 'setKeep',
            check: (state) => {
                return true;
            },
            userdata: {
                items: items,
                keep: keep
            },
            finished: finished
        });
        if (transit) {
            var len = items.length;
            for ( i = 0; i < len; i++) {
                this.oipfRecSchedulerObj.setDoNotDelete(items[i].getInstance(), keep);
            }
        }
        if (finished) {
            finished(true);
        }
        return transit;
    }

    updateScheduledRecording(param): boolean {
        var finished = param.finished;
        var transit = this.fnTransitReqState('REQ', {
            name: 'updateScheduledRecording',
            check: (state) => {
                return true;
            },
            finished: finished,
            userdata: {
                requestedAction: 'updateScheduledRecording',
                scheduleInfo: param,
                retryWithDuration: (AdjustedDuration, scheduleInfo) => {
                    this.oipfRecSchedulerObj.update(scheduleInfo.item.getInstance()//
                        , scheduleInfo.startTime//
                        , AdjustedDuration//
                        , scheduleInfo.repeatDays//
                        , scheduleInfo.ccid//
                        , scheduleInfo.factoryType//
                        , 1);
                },
                resolveConflict: (cancelItems, scheduleInfo) => {
                    var stopItems = ( cancelItems instanceof Array) ? cancelItems.slice(0) : [cancelItems];
                    this.fnRemoveScheduledRecording({
                        items: stopItems,
                        //delFirstSeries: true,
                        finished: () => {
                            setTimeout(() => {
                                this.oipfRecSchedulerObj.update(scheduleInfo.item.getInstance()//
                                    , scheduleInfo.startTime//
                                    , scheduleInfo.duration//
                                    , scheduleInfo.repeatDays//
                                    , scheduleInfo.ccid//
                                    , scheduleInfo.factoryType//
                                    , 1);
                            }, 300);
                        }
                    });
                }
            }
        });
        if (transit) {
            var item = param.item;
            var startTime = param.startTime;
            var duration = param.duration;
            var repeatDays = param.repeatDays;
            var ccid = param.ccid;
            var factoryType = param.factoryType;
            hx.log('dsrvRecordings2', 'update schedule ---');
            hx.log('dsrvRecordings2', '\t startTime ' + startTime);
            hx.log('dsrvRecordings2', '\t duration ' + duration);
            hx.log('dsrvRecordings2', '\t repeatDays ' + repeatDays);
            hx.log('dsrvRecordings2', '\t ccid ' + ccid);
            hx.log('dsrvRecordings2', '\t factoryType ' + factoryType);
            var returnScheduledRecording = this.oipfRecSchedulerObj.update(item.getInstance(), startTime, duration, repeatDays, ccid, factoryType, 1);
            if ((returnScheduledRecording === null) || (returnScheduledRecording === undefined)) {
                hx.log('dsrvRecordings2', '[updateScheduledRecording] invalid returnScheduledRecording');
                //this.fnTransitReqState('IDLE');
            }
        }
        return transit;
    }

    stopRecord(param): void {
        this.fnStopRecording(param);
    }

    stopRecordingByID(param): void {
        var idList = param.id;
        var currentRecordingList = this.fnGetCurrentRecordings();
        if (!( idList instanceof Array)) {
            idList = [idList];
        }
        if (idList.length === 0) {
            return;
        }
        var i, ti, j, tj, item;
        for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
            item = currentRecordingList[i];
            if (item) {
                for (j = 0, tj = idList.length; j < tj; j += 1) {
                    if (item.getId() === idList[j]) {
                        this.oipfRecSchedulerObj.stop(item.getInstance());
                        idList.splice(j, 1);
                        break;
                    }
                }
            }
            if (idList.length === 0) {
                break;
            }
        }
    }

    removeWholeSeriesSchedule(param): void {
        this.fnRemoveWholeSeriesSchedule(param);
    }

    removeScheduledRecording(param): void {
        this.fnRemoveScheduledRecording(param);
    }

    removeRecording(param): void {
        var item, items = param.items;
        var finished = param.finished;
        var i, ti;
        for (i = 0, ti = items.length; i < ti; i += 1) {
            //item = (items[i] instanceof Array) ? items[i][0] : items[i];
            item = items[i];
            hx.log('dsrvRecordings2', ' ' + i + ' remove recording ' + item.getTitle());
            this.mRemovingList.push(item);
            this.oipfRecSchedulerObj.remove(item.getInstance());
        }
        if (finished) {
            finished(true);
        }
    }

    recordConfirm(param): void {
        this.oipfRecSchedulerObj.recordConfirm(param.scheduledRecording.getInstance(), param.type, param.check);
    }

    setDuration(param): boolean {
        var item = param.scheduledRecording;
        var duration = param.duration;
        var finished = param.finished;
        var transit = this.fnTransitReqState('REQ', {
            name: 'setDuration',
            delay: 1000,
            check: (input) => {
                var state = input.state;
                var TRecordingState = __MODEL__.eTRecordingState;
                switch (state) {
                    case TRecordingState.ERECORDING_SCHEDULED:
                    case TRecordingState.ERECORDING_DEL_SCHEDULED:
                    case TRecordingState.ERECORDING_UNREALIZED:
                    case TRecordingState.ERECORDING_ERROR:
                    case TRecordingState.ERECORDING_REC_START_CONFLICT:
                    case TRecordingState.ERECORDING_REC_START_ERROR:
                        break;
                }
                return true;
            },
            finished: finished,
            userdata: {
                requestedAction: 'setDuration',
                scheduleInfo: {
                    scheduledRecordingItem: item,
                    duration: duration
                },
                retryWithDuration: (AdjustedDuration, scheduleInfo) => {
                    var item = scheduleInfo.scheduledRecordingItem;
                    item.getInstance().duration = AdjustedDuration;
                },
                resolveConflict: (cancelItems, scheduleInfo) => {
                    var stopItems = ( cancelItems instanceof Array) ? cancelItems.slice(0) : [cancelItems];
                    this.fnRemoveScheduledRecording({
                        items: stopItems,
                        //delFirstSeries: true,
                        finished: () => {
                            setTimeout(() => {
                                this.setDuration({
                                    scheduledRecording: item,
                                    duration: duration
                                })
                            }, 300);
                        }
                    });
                }
            }
        });
        if (transit) {
            item.getInstance().duration = duration;
        }
        return transit;
    }

    record(param): void {
        var programme = param.programme;
        var factoryType = param.factoryType;
        var finished = param.finished;
        var transit = this.fnTransitReqState('REQ', {
            name: 'record',
            check: (input) => {
                var state = input.state;
                var TRecordingState = __MODEL__.eTRecordingState;
                switch (state) {
                    case TRecordingState.ERECORDING_SCHEDULED:
                    case TRecordingState.ERECORDING_DEL_SCHEDULED:
                    case TRecordingState.ERECORDING_UNREALIZED:
                    case TRecordingState.ERECORDING_ERROR:
                    case TRecordingState.ERECORDING_REC_START_CONFLICT:
                    case TRecordingState.ERECORDING_REC_START_ERROR:
                        break;
                }
                return true;
            },
            finished: finished,
            userdata: {
                requestedAction: 'record',
                scheduleInfo: {
                    programme: programme,
                    factoryType: factoryType
                },
                resolveConflict: (cancelItems, scheduleInfo) => {
                    var stopItems = ( cancelItems instanceof Array) ? cancelItems.slice(0) : [cancelItems];
                    this.fnRemoveScheduledRecording({
                        items: stopItems,
                        //delFirstSeries: true,
                        finished: () => {
                            setTimeout(() => {
                                var programme = scheduleInfo.programme;
                                var factoryType = scheduleInfo.factoryType;
                                this.oipfRecSchedulerObj.record(programme, factoryType, 1);
                            }, 300);
                        }
                    });
                }
            }
        });
        if (transit) {
            if (programme instanceof __MODEL__.COipfRecordingItem && programme.getGroupCRIDs()) {
                this.recordCRID(10, programme.getGroupCRIDs()[0], programme.getTitle(), programme.getChannelId());
            } else {
                var returnScheduledRecording = this.oipfRecSchedulerObj.record(programme, factoryType, 1);
                if ((returnScheduledRecording === null) || (returnScheduledRecording === undefined)) {
                    hx.log('dsrvRecordings2', '[record] invalid returnScheduledRecording');
                    //this.fnTransitReqState('IDLE');
                }
            }
        }
    }

    recordAt(param): void {
        this.fnRecordAt(param);
    }

    recordAtPowerOnTimer(param): void {
        param.factoryType = __MODEL__.eTScheduledFactoryType.EPOWER_ON_TIMER;
        this.fnRecordAt(param);
    }

    recordAtReminder(param): void {
        param.factoryType = __MODEL__.eTScheduledFactoryType.EREMIND;
        this.fnRecordAt(param);
    }

    recordAtRecording(param): void {
        param.factoryType = __MODEL__.eTScheduledFactoryType.ERECORDING;
        this.fnRecordAt(param);
    }

    recordAtRegularOTA(param): void {
        param.factoryType = __MODEL__.eTScheduledFactoryType.EREGULAR_OTA;
        this.fnRecordAt(param);
    }

    recordCRID(aFactoryType: number, aCRID: string, aTitle: string, aChannelID: string): void {
        hx.log('dsrvRecordings2', "recordCrid(aFactoryType, aCRID, aTitle, aChannelID)");
        hx.log('dsrvRecordings2', "aFactoryType" + aFactoryType);
        hx.log('dsrvRecordings2', "aCRID" + aCRID);
        hx.log('dsrvRecordings2', "aTitle" + aTitle);
        hx.log('dsrvRecordings2', "aChannelID" + aChannelID);
        this.oipfRecSchedulerObj.recordCrid(aFactoryType, aCRID, aTitle, aChannelID);
    }

    requestInstanceRecording(param): void {
        this.fnRequestStartRecording(param);
    }

    requestStopRecording(param?): void {
        //var currentRecordings = this.fnGetCurrentRecordings();
        //var stopList = hlsList.concat(currentRecordings);
        this.fireEvent('ShowRecordingStopWindow');
    }

    setTargetStorage(param): void {
        var storage = param.storage;
        this.oipfConfigObj.targetStorage = storage;
    }

    getTargetStorage(param): any {
        return this.oipfConfigObj.targetStorage;
    }

    copyRecording(param): void {
        var finished = param.finished;
        var TRecordingCopyControl = __MODEL__.eTRecordingCopyControl;
        var TRecordingFileOperationState = __MODEL__.eTRecordingFileOperationState;
        var items = param.items;
        var targetStorageId = param.targetStorageId;
        var i, ti, item, ticket, tickets = [];
        for (i = 0, ti = items.length; i < ti; i += 1) {
            item = items[i];
            ticket = this.oipfRecSchedulerObj.copyRecording(item.getInstance(), targetStorageId);
            if (ticket !== undefined && ticket !== -1) {
                tickets.push({
                    item: item,
                    ticket: ticket,
                    state: TRecordingFileOperationState.ERECORDING_FILEOP_STATE_UNKNOWN
                });
            }
        }
        if (tickets.length) {
            this.mFileOpTicketQue.push({
                operation: 'COPY',
                tickets: tickets
            });
            if (finished) {
                finished(true);
            }
        } else {
            if (finished) {
                finished(false);
            }
        }
    }

    moveRecording(param): void {
        var finished = param.finished;
        var TRecordingCopyControl = __MODEL__.eTRecordingCopyControl;
        var TRecordingFileOperationState = __MODEL__.eTRecordingFileOperationState;
        var items = param.items;
        var targetStorageId = param.targetStorageId;
        var i, ti, item, ticket, tickets = [];
        for (i = 0, ti = items.length; i < ti; i += 1) {
            item = items[i];
            ticket = this.oipfRecSchedulerObj.moveRecording(item.getInstance(), targetStorageId);
            if (ticket !== undefined && ticket !== -1) {
                tickets.push({
                    item: item,
                    ticket: ticket,
                    state: TRecordingFileOperationState.ERECORDING_FILEOP_STATE_UNKNOWN
                });
            }
        }
        if (tickets.length) {
            this.mFileOpTicketQue.push({
                operation: 'MOVE',
                tickets: tickets
            });
            if (finished) {
                finished(true);
            }
        } else {
            if (finished) {
                finished(false);
            }
        }
    }

    stopFileOperation(param): void {
        var finished = param.finished;
        var TRecordingFileOperationState = __MODEL__.eTRecordingFileOperationState;
        var fileOperationInfo = this.mFileOpTicketQue[0];
        var i, ti, j, tickets = fileOperationInfo.tickets, ticket;
        var eventName;
        if (fileOperationInfo.operation === 'COPY') {
            eventName = 'RecordingCopyStoped';
        } else {
            eventName = 'RecordingMoveStoped';
        }
        fileOpState = "IDLE";
        for (i = 0, ti = tickets.length; i < ti; i += 1) {
            this.oipfRecSchedulerObj.stopFileOp(tickets[i].ticket);
            this.fireEvent(eventName, {
                item: tickets[i].item
            });
        }
        fileOperationInfo.tickets.length = 0;
        this.mFileOpTicketQue.pop();
        if (finished) {
            finished();
        }
    }

    createScheduledItem(oipfScheduledObj): any {
        return new __MODEL__.COipfScheduledItem(oipfScheduledObj);
    }

    createRecordingItem(oipfRecordingObj): any {
        return new __MODEL__.COipfRecordingItem(oipfRecordingObj);
    }

    checkRemainedRequestedRemove(): any {
        return this.oipfRecSchedulerObj.checkRemainedRequestRemove();
    }

    getRecordingById(param): any {
        var id = param.id, isSeries = param.isSeries;
        var item;
        // TODO: working
        if (!isSeries) {
            item = this.fnGetRecording(id, null);
        }
        return item;
    }

    startSatRecord(): void {
        this.oipfRecSchedulerObj.startSatRecord();
    }

    getFileOpState(): string {
        return fileOpState;
    }

    getCurrentRecordingByCcid(aCcid: string): __PROVIDER__.cPdeRecordingItem {
        //COipfRecordingItem, COipfScheduledItemList
        var list = this.getCurrentRecordings();
        if (list) {
            for (var i = 0; i < list.length; i++) {
                if (list[i].getChannelId() === aCcid) {
                     return new __PROVIDER__.cPdeRecordingItem(list[i]);
                }
            }
        }
        return null;
    }
    //Event
    onFileOpEvent(ticket, state): void {
        if (!this.mFileOpTicketQue.length) {
            hx.log('error', 'not operation' + ticket);
            return;
        }
        var TRecordingFileOperationState = __MODEL__.eTRecordingFileOperationState;
        var fileOperationInfo = this.mFileOpTicketQue[0];
        var ticketInfo = this.fnSearchTicket(fileOperationInfo.tickets, (ticketInfo) => {
            if (ticketInfo.ticket === ticket) {
                return true;
            }
        });
        if (!ticketInfo) {
            hx.log('error', 'unknown ticket info ' + ticket);
            return;
        }
        var eventName;
        if (state === TRecordingFileOperationState.ERECORDING_FILEOP_STATE_ING) {
            if (fileOperationInfo.operation === 'COPY') {
                eventName = 'RecordingCopying';
                fileOpState = "COPY";
            } else {
                eventName = 'RecordingMoving';
                fileOpState = "MOVE";
            }
            this.fireEvent(eventName, {
                item: ticketInfo.item
            });
        } else {
            //
            fileOpState = "IDLE";
            fileOperationInfo.tickets.splice(fileOperationInfo.tickets.indexOf(ticketInfo), 1);
            switch (state) {
                case TRecordingFileOperationState.ERECORDING_FILEOP_STATE_COMPLETE:
                    hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_FILEOP_STATE_COMPLETE');
                    if (fileOperationInfo.tickets.length === 0) {
                        if (fileOperationInfo.operation === 'COPY') {
                            eventName = 'RecordingCopyComplete';
                        } else {
                            eventName = 'RecordingMoveComplete';
                            if (ticketInfo.onComplete) {
                                ticketInfo.onComplete(ticketInfo.item);
                            }
                        }
                        this.fireEvent(eventName, {
                            item: ticketInfo.item
                        });
                        this.mFileOpTicketQue.pop();
                    }
                    break;
                case TRecordingFileOperationState.ERECORDING_FILEOP_STATE_ERROR:
                    hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_FILEOP_STATE_ERROR');
                    if (fileOperationInfo.tickets.length === 0) {
                        if (fileOperationInfo.operation === 'COPY') {
                            eventName = 'RecordingCopyError';
                        } else {
                            eventName = 'RecordingMoveError';
                        }
                        this.fireEvent(eventName, {
                            item: ticketInfo.item
                        });
                        this.mFileOpTicketQue.pop();
                    }
                    break;
                case TRecordingFileOperationState.ERECORDING_FILEOP_STATE_STOP:
                    hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_FILEOP_STATE_STOP');
                    if (fileOperationInfo.tickets.length === 0) {
                        if (fileOperationInfo.operation === 'COPY') {
                            eventName = 'RecordingCopyStoped';
                        } else {
                            eventName = 'RecordingMoveStoped';
                        }
                        this.fireEvent(eventName, {
                            item: ticketInfo.item
                        });
                        this.mFileOpTicketQue.pop();
                    }
                    break;
                case TRecordingFileOperationState.ERECORDING_FILEOP_STATE_UNKNOWN:
                    hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_FILEOP_STATE_UNKNOWN');
                    break;
                case TRecordingFileOperationState.ERECORDING_FILEOP_NOT_ENOUGH_SPACE:
                    hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_FILEOP_NOT_ENOUGH_SPACE');
                    if (ticketInfo.onComplete) {
                        ticketInfo.onComplete(ticketInfo.item);
                    }
                    this.fireEvent("RecordingCopyFailNotEnoughSpace", {
                        item : ticketInfo.item
                    });
                    this.mFileOpTicketQue.pop();
                    break;
                case TRecordingFileOperationState.ERECORDING_FILEOP_STATE_SIZE_LIMIT_4G:
                    hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_FILEOP_STATE_SIZE_LIMIT_4G');
                    if (ticketInfo.onComplete) {
                        ticketInfo.onComplete(ticketInfo.item);
                    }
                    this.fireEvent("RecordingCopyFailSizeLimit4G", {
                        item : ticketInfo.item
                    });
                    this.mFileOpTicketQue.pop();
                    break;
            }
        }
    }
    fnmakeDispTime(data) {
        var startTime, endTime;
        var strTime, tempStr;

        if (data.startTime === undefined) {
            startTime = new Date();
        } else {
            startTime = new Date((data.startTime) * 1000);
        }
        endTime = new Date(startTime.valueOf() + (data.duration + 30) * 1000);
        strTime = startTime.format("HH:MM") + '-' + endTime.format("HH:MM") + ', ' + startTime.format("mm.dd");

        if (data.name === undefined || data.name.length === 0) {
            tempStr = strTime;
        } else {
            tempStr = data.name + ', ' + strTime + ')';
        }

        return tempStr;
    }
    onPVREvent(state: number, recording: ScheduledRecording, error: number, conflicts: ScheduledRecordingCollection, recConflict: number, alternatives: ScheduledRecordingCollection, altevents: ProgrammeCollection): void {
        var updatedIndex;
        var TRecordingState = __MODEL__.eTRecordingState;
        var TRecordingError = __MODEL__.eTRecordingError;
        var eventItem, bScheduled = fnIsScheduled(recording);
        var i, j, ti;
        var that = this;
        if (bScheduled) {
            eventItem = new __MODEL__.COipfScheduledItem(recording);
        } else {
            eventItem = new __MODEL__.COipfRecordingItem(recording);
        }
        switch (state) {
            case TRecordingState.ERECORDING_UNREALIZED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_UNREALIZED');
                break;
            case TRecordingState.ERECORDING_SCHEDULED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_SCHEDULED');
                if (bScheduled) {
                    var scheduledItem, isExist = false;
                    for (i = 0, ti = this.mScheduledList.length; i < ti; i += 1) {
                        scheduledItem = this.mScheduledList[i];
                        if (scheduledItem.isEqual(eventItem)) {
                            isExist = true;
                        }
                    }
                    if (isExist) {
                        updatedIndex = this.mScheduledList.update(eventItem);
                        if (updatedIndex === -1) {
                            hx.log('error', 'scheduled list update fail');
                        } else {
                            this.fireEvent('ScheduledItemUpdated', {
                                item: eventItem
                            });
                        }
                    } else {
                        var programmeID;
                        this.mScheduledList.add(eventItem);
                        this.fireEvent('ScheduledItemAdded', {
                            item: eventItem
                        });
                        programmeID = eventItem.getProgrammeID();
                        if (!programmeID) {
                            eventItem.setThumbnailUrl(null);
                        } else {
                            setTimeout(() => {
                                var dsrvEpg = hx.svc.EPG;
                                dsrvEpg.findProgrammes({
                                    channel: [eventItem.getChannelInstance()],
                                    clause: 'programmeID == ' + programmeID,
                                    isOnAir: false,
                                    //listener : a,
                                    cb: (result) => {
                                        if (result.length <= 0) {
                                            result = undefined;
                                            return;
                                        }
                                        var eventArray, programmeItem, thumbnailUrl;
                                        eventArray = result[0];
                                        if (eventArray && eventArray.length) {
                                            programmeItem = eventArray[0];
                                            if (programmeItem) {
                                                thumbnailUrl = programmeItem.getThumbnailURL(true);
                                                if (thumbnailUrl && thumbnailUrl.length) {
                                                    eventItem.setThumbnailUrl(thumbnailUrl);
                                                } else {
                                                    eventItem.setThumbnailUrl(null);
                                                }
                                                eventItem.setShortDescription(programmeItem.getShortDescription());
                                                eventItem.setLongDescription(programmeItem.getLongDescription());
                                                if(!!programmeItem.getGuidance()) {
                                                    eventItem.setGuidance(programmeItem.getGuidance());//.guidance[0].text
                                                } else {
                                                    eventItem.setGuidance("");
                                                }
                                                if (this.mPrevRecommendationProgrammeId === programmeID) {
                                                    // (temporary) display Recommendation popup only once.
                                                    hx.log('error', 'display Recommendation popup only once.');
                                                    this.mPrevRecommendationProgrammeId = null;
                                                    return;
                                                }
                                                this.mPrevRecommendationProgrammeId = null;
                                                var recommendationCRID = programmeItem.getRecommendationCRID();
                                                if (recommendationCRID) {
                                                    dsrvEpg.findProgrammes({
                                                        clause: "keyword %% %" + recommendationCRID + ",%",
                                                        cb: (aResult) => {
                                                            var eventArray = aResult[0];
                                                            var rcProgramme: __PROVIDER__.CProgrammeProvider;
                                                            if (eventArray) {
                                                                rcProgramme = eventArray[0];
                                                                if (rcProgramme) {
                                                                    this.mPrevRecommendationProgrammeId = rcProgramme.programmeID;
                                                                    this.fireEvent('NotiFoundRecommendationProgramme', rcProgramme);
                                                                }
                                                            }
                                                        }
                                                    });
                                                }

                                            }
                                        }
                                        result = undefined;
                                    }
                                });
                            }, 1000);
                        }
                        if (this.mRecCtlStatEm.state === 'REQ') {
                            this.fireEvent('ScheduledItemSet', {
                                item: eventItem
                            });
                        }
                    }
                } else {
                    if (eventItem.isRecordingOnGoing()) {
                        hx.log('error', 'invalid case on going recording item scheduled..');
                    }
                    this.mRecList.add(eventItem);
                    this.fireEvent('RecordingListLoadingCompleted', {
                        item: eventItem
                    });
                }
                break;
            case TRecordingState.ERECORDING_DEL_SCHEDULED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_DEL_SCHEDULED');
                hx.log('dsrvRecordings2', eventItem.getTitle());
                if (bScheduled) {
                    if (eventItem.isStreaming()) {
                        this.fireEvent('NotiLiveStreamingStop', {
                            item : eventItem
                        });
                    }
                }
                var fnRemove = (targetItem) => {
                    var removedItem = targetItem;
                    if (!removedItem) {
                        return;
                    }
                    var itemId = removedItem.getId();
                    if (itemId) {   // Recording item
                        var itemIndex = this.mRemovingList.indexOf(itemId);
                        while (itemIndex > -1) {
                            this.mRemovingList.remove(itemIndex);
                            itemIndex = this.mRemovingList.indexOf(itemId);
                        }
                        this.mRecordingStopped = true;
                        this.mRecList.remove(removedItem);
                        this.fireEvent('RecordingItemRemoved', {
                            item: removedItem
                        });
                        // TODO: should check if it is OK without below timer that is a kind of defense code
                        //clearTimeout(this.mRemovingTimer);
                        //this.mRemovingTimer = setTimeout(() => {
                        //    var i, length, item, itemIndex, recItem, recordings, id, intersections = [];
                        //    recordings = this.oipfRecSchedulerObj.getRecordingList();
                        //    length = recordings ? recordings.length : 0;
                        //    if (length) {
                        //        for (i = 0; i < length; i += 1) {
                        //            recItem = recordings[i];
                        //            id = recItem.id;
                        //            itemIndex = this.mRemovingList.indexOf(id);
                        //            if (itemIndex > -1) {
                        //                intersections.push(id);
                        //            }
                        //        }
                        //        length = this.mRemovingList.length;
                        //        for (i = 0; i < length; i += 1) {
                        //            item = this.mRemovingList[i];
                        //            if (intersections.indexOf(item.getId()) === -1) {
                        //                fnRemove(item);
                        //            }
                        //        }
                        //    }
                        //    item = null;
                        //    recItem = null;
                        //    recordings = null;
                        //    intersections = null;
                        //}, 3000);
                    } else {    // Schedule item
                        this.mScheduledList.remove(removedItem);
                        this.fireEvent('ScheduledItemRemoved', {
                            item: removedItem
                        });
                    }
                    removedItem = undefined;
                };
                fnRemove(eventItem);
                break;
            case TRecordingState.ERECORDING_REC_PRESTART:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_PRESTART');
                if (bScheduled) {
                    if (eventItem.isPowerOnTimer()) {
                        hx.log('dsrvRecordings2', 'power on timer event..!');
                        this.fireEvent('ScheduledWillBeStart', {
                            item: eventItem
                        });
                        return;
                    }
                    if (eventItem.isReminder()) {
                        hx.log('dsrvRecordings2', 'PVREvent state - Check Reminder channel');
                        var scheduleCCID = null, retObjCurSvc = null, curSvcCCID = null;
                        scheduleCCID = eventItem.getChannelId();
                        // 2. check current service type
                        retObjCurSvc = hx.svc.CH_LIST.getCurrentChannel();
                        if ((true === retObjCurSvc.result) && (retObjCurSvc.channel !== undefined)) {
                            curSvcCCID = retObjCurSvc.channel.ccid;
                        } else {
                            curSvcCCID = null;
                        }
                        hx.log('dsrvRecordings2', 'PVREvent state - Check Reminder channel schedule:' + scheduleCCID + 'curchannel:' + curSvcCCID);
                        var powerValueObj = hx.svc.SETTING_UTIL.getPowerStateValues();
                        if (powerValueObj.curPowerState !== 1 /* if not normal mode.... */) {
                            hx.log('dsrvRecordings2', 'PVREvent state - this system power mode is standby... fire pre start event to standby page.....\n');
                            this.fireEvent('ScheduledWillBeStart', {
                                item: eventItem
                            });
                            return;
                        }
                        if ((ENV.op === ENV.listOP.MIDDLE_EAST) && (hx.il.criticalTasking.crOTATasking === true)) {
                            hx.log('dsrvRecordings2', 'PVREvent state - process reminder during OTA according to ENV.listOP.MIDDLE_EAST spec !!!!! (-)');
                            eventItem.setRecordingOnGoing();
                            this.fireEvent('ScheduledWillBeStart', {
                                item: eventItem
                            });
                            return;
                        }
                        if ((!hx.svc.AV_CTRL.isLiveOnGoing()) || (scheduleCCID !== curSvcCCID)) {
                            hx.log('dsrvRecordings2', 'PVREvent state -  Start Channel change !!!!! (-)');
                            this.fireEvent('NotiScheduledStartError', {
                                item: eventItem
                            });
                        }
                    } else {
                        eventItem.setRecordingOnGoing();
                        updatedIndex = this.mScheduledList.update(eventItem);
                        if (updatedIndex === -1) {
                            hx.log('error', 'scheduled list update fail');
                        }
                        this.fireEvent('ScheduledWillBeStart', {
                            item: eventItem
                        });
                    }
                }
                break;
            case TRecordingState.ERECORDING_REC_ACQUIRING_RESOURCES:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_ACQUIRING_RESOURCES');
                break;
            case TRecordingState.ERECORDING_REC_STARTED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_STARTED');
                if (eventItem.isNCDChanged()) {
                    hx.log('dsrvRecordings2', 'hx.svc.SETTING_UTIL.getNCDNetworkMsg();');
                    hx.svc.SETTING_UTIL.getNCDNetworkMsg();
                } else {
                    if (bScheduled) {
                        if (eventItem.isPowerOnTimer() || eventItem.isReminder()) {
                            hx.log('dsrvRecordings2', 'power on timer event..!');
                            var powerValueObj2 = hx.svc.SETTING_UTIL.getPowerStateValues();
                            if (powerValueObj2.curPowerState !== 1 /* if not normal mode.... */) {
                                this.fireEvent('NotiRecordingStarted', {
                                    item: eventItem
                                });
                                return;
                            }
                        } else if (eventItem.isStreaming()) {
                            hx.log('dsrvRecordings2', 'Live streming Start event !!!');
                            this.fireEvent('NotiLiveStreamingStart', {
                                item: eventItem
                            });
                        }
                    } else {
                        this.fireEvent('NotiRecordingStarted', {
                            item: eventItem
                        });
                        this.mRecList.add(eventItem);
                        this.fireEvent('RecordingItemAdded', {
                            item: eventItem
                        });
                    }
                }
                break;
            case TRecordingState.ERECORDING_REC_UPDATED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_UPDATED');
                if (bScheduled) {
                    updatedIndex = this.mScheduledList.update(eventItem);
                    if (updatedIndex === -1) {
                        hx.log('error', 'scheduled list update fail');
                    } else {
                        this.fireEvent('ScheduledItemUpdated', {
                            item: eventItem
                        });
                    }
                } else {
                    updatedIndex = this.mRecList.update(eventItem);
                    if (updatedIndex === -1) {
                        hx.log('error', 'recording list update fail');
                        this.mRecList.add(eventItem);
                        this.fireEvent('RecordingListLoadingCompleted', {
                            item: eventItem
                        });
                    } else {
                        this.fireEvent('RecordingItemUpdated', {
                            item: eventItem
                        });
                    }
                }
                break;
            case TRecordingState.ERECORDING_REC_COMPLETED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_COMPLETED');
                if (!bScheduled) {
                    updatedIndex = this.mRecList.update(eventItem);
                    if (updatedIndex !== -1) {
                        this.fireEvent('RecordingCompleted', {
                            item: eventItem
                        });
                    }
                }
                break;
            case TRecordingState.ERECORDING_REC_PARTIALLY_COMPLETED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_PARTIALLY_COMPLETED');
                if (bScheduled) {
                    updatedIndex = this.mScheduledList.update(eventItem);
                    if (updatedIndex === -1) {
                        hx.log('error', 'scheduled list update fail');
                    } else {
                        this.fireEvent('ScheduledItemUpdated', {
                            item: eventItem
                        });
                    }
                } else {
                    updatedIndex = this.mRecList.update(eventItem);
                    if (updatedIndex !== -1) {
                        this.fireEvent('RecordingCompleted', {
                            item: eventItem
                        });
                    }
                }
                break;
            case TRecordingState.ERECORDING_TS_ACQUIRING_RESOURCES:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_TS_ACQUIRING_RESOURCES');
                break;
            case TRecordingState.ERECORDING_TS_STARTED:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_TS_STARTED');
                break;
            case TRecordingState.ERECORDING_REC_START_CONFLICT:
            case TRecordingState.ERECORDING_ERROR:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_START_CONFLICT');
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_ERROR');
                var userdata = this.mRecCtlStatEm.userdata;
                switch (error) {
                    case TRecordingError.ERROR_INSUFFICIENT_STORAGE:
                        this.fireEvent('NotiRecordingStartError', {
                            item: eventItem,
                            reason: 'INSUFFICIENT_STORAGE'
                        });
                        break;
                    case TRecordingError.ERROR_HAVE_ALTERNATIVE:
                        hx.log('dsrvRecordings2', 'PVREvent error - ERROR_HAVE_ALTERNATIVE');
                        /* TODO
                         * use alternatives and make model
                         * bglee@humaxdigital.com, 17:07, 2014. 6. 25.
                         */
                        var alList = [];
                        var coList = [];
                        var item;
                        if (conflicts){
                            for (i = 0; i < conflicts.length; i++) {
                                item = new __MODEL__.COipfScheduledItem(conflicts.item(i));
                                coList[i] = item;
                                hx.log("debug", 'conflict: ' + item.getProgrammeID() + ' : ' + item.getTitle() + ' : ' + __MODEL__.eTScheduledFactoryType[item.getFactoryType()] + ' : ' + item.isSeries());
                            }
                        }
                        if (alternatives) {
                            for (i = 0; i < alternatives.length; i++) {
                                item = new __MODEL__.COipfScheduledItem(alternatives.item(i));
                                alList[i] = {
                                    schedule: item,
                                    programme: altevents.item(i)
                                };
                                hx.log("debug", 'alternatives: ' + item.getProgrammeID() + ' : ' + item.getTitle() + ' : ' + __MODEL__.eTScheduledFactoryType[item.getFactoryType()] + ' : ' + item.isSeries());
                                hx.log("debug", 'altevents: ' + alList[i].programme.programmeID + ' : ' + alList[i].name + ' : ' + alList[i].factoryType + ' : ' + alList[i].isScheduledAsSerie);
                            }
                        }
                        eventItem.getProgrammeID();
                        this.fireEvent('ShowAlternativeWindow', {
                            conflictList: coList,
                            alternativeList: alList,
                            requestedScheduledItem: eventItem,
                            requestedAction: (userdata) ? userdata.requestedAction : undefined,
                            retryWithDuration: (userdata) ? userdata.retryWithDuration : undefined,
                            resolveConflict: (userdata) ? userdata.resolveConflict : undefined,
                            scheduleInfo: (userdata) ? userdata.scheduleInfo : undefined
                        });
                        break;
                    case TRecordingError.ERROR_MAX_RECORD_LIST:
                        var scheduledItem, isExist = false;
                        for (i = 0, ti = this.mScheduledList.length; i < ti; i += 1) {
                            scheduledItem = this.mScheduledList[i];
                            if (scheduledItem.isEqual(eventItem) && scheduledItem.isRecordingOnGoing(true)) {
                                isExist = true;
                                break;
                            }
                        }
                        this.fireEvent('NotiRecordingStartError', {
                            item: eventItem,
                            reason: 'MAX_RECORD_LIST',
                            bScheduled: isExist

                        });
                        break;
                    default:
                        if (conflicts && conflicts.length) {
                            hx.log("dsrvRecordings", "\t conflicts " + conflicts.length);
                            var conflictList = [], index = 0, newItem;
                            if (error === TRecordingError.ERROR_TUNER_CONFLICT) {
                                //TP Conflict
                                for (i = 0, ti = conflicts.length; i < ti; i += 1) {
                                    var subListLength = conflicts.item(i);
                                    if (subListLength) {
                                        var tj, list = [];
                                        for (j = 0; j < subListLength; j += 1) {
                                            newItem = new __MODEL__.COipfScheduledItem(conflicts.item(index));
                                            list.push(newItem);
                                            index += 1;
                                        }
                                        conflictList.push(list);
                                    }
                                }
                            } else {
                                for (i = 0, ti = conflicts.length; i < ti; i += 1) {
                                    newItem = new __MODEL__.COipfScheduledItem(conflicts.item(i));
                                    conflictList.push([newItem]);
                                }
                            }
                            this.fireEvent('ShowConflictWindow', {
                                conflictList: conflictList,
                                requestedScheduledItem: eventItem,
                                requestedAction: (userdata) ? userdata.requestedAction : undefined,
                                retryWithDuration: (userdata) ? userdata.retryWithDuration : undefined,
                                resolveConflict: (userdata) ? userdata.resolveConflict : undefined,
                                scheduleInfo: (userdata) ? userdata.scheduleInfo : undefined
                            });
                        }
                        break;
                }
                //if (error === TRecordingError.ERROR_INSUFFICIENT_STORAGE) {
                //} else if (error === TRecordingError.ERROR_HAVE_ALTERNATIVE) {
                //} else {
                //}
                break;
            case TRecordingState.ERECORDING_REC_START_ERROR:
                hx.log('dsrvRecordings2', 'PVREvent state - ERECORDING_REC_START_ERROR');
                if (bScheduled) {
                    var tpConflictParam = this.__tpConflictParam;
                    this.fireEvent('NotiScheduledStartError', {
                        item: eventItem,
                        tpConflictParam : tpConflictParam
                    });
                } else {
                    if (error === TRecordingError.ERROR_INSUFFICIENT_STORAGE) {
                        this.fireEvent('NotiRecordingStartError', {
                            item: eventItem,
                            reason: 'INSUFFICIENT_STORAGE'
                        });
                    }
                    this.mRecList.add(eventItem);
                    this.fireEvent('RecordingItemAdded', {
                        item: eventItem
                    });
                }
                break;
            case TRecordingState.ERECORDING_REC_LOAD:
                hx.log('dsrvRecordings2', '[PVREvent] state - ERECORDING_REC_LOAD');
                if (bScheduled) {
                    this.fnRefreshScheduleList();
                } else {
                    this.mRecordingListLoaded = true;
                    this.fnRefreshRecordingList();
                    this.fireEvent('NotiRecordingListRefresh', {});
                }
                break;
        }
        this.fnTransitReqState('FIN', {
            state: state,
            eventItem: eventItem,
            bScheduled: bScheduled
        });
    }
}
export = cDLRecordings;
