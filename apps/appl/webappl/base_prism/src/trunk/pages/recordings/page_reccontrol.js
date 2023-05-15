var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "model/model", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __MODEL__, __NETFLIX__) {
    // file name : pages/recordings/recControl.js
    /*jslint sloppy:true, nomen:true, vars:true */
    /*global $ */
    var cCPgRecControl = (function (_super) {
        __extends(cCPgRecControl, _super);
        function cCPgRecControl() {
            _super.call(this);
            this.NUM_MAX_RECORD_LIST = 1500;
            this._reminderMsg = null;
            this._popup = null;
        }
        cCPgRecControl.prototype.create = function (aEntity) {
            'use strict';
            var _this = this;
            var that = this;
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var dsrvEpg = hx.svc.EPG;
            var item, name;
            var textList = [{
                '_one': hx.l('LOC_DO_NOT_STOP_ID')
            }];
            var prefixText = hx.l('LOC_STOP_ID') + ' : ';
            var i, ti;
            hx.logadd('CPgRecControl');
            hx.log('CPgRecControl', '[create] create recording control page');
            hx.al.compose(that);
            that.entity = aEntity;
            that._popup = null;
            var _fnGetEllipsisText = function (orgText, maxLength) {
                var outputText = orgText;
                if (outputText.length >= maxLength) {
                    outputText = orgText.substring(0, maxLength - 3) + '...';
                }
                return outputText;
            };
            var _fnShowStopRec = function (param, finished) {
                var currentRecordingList = dsrvRecordings.getCurrentRecordings();
                var streamingList = dsrvRecordings.getAllStreamingList();
                if (currentRecordingList.length === 0 && streamingList.length === 0) {
                    hx.log('CPgRecControl', 'recording/HLS list empty');
                    if (finished) {
                        finished();
                    }
                    return;
                }
                hx.log('CPgRecControl', 'create cancel recording dialog');
                var item, name;
                var textList = [{
                    '_one': hx.l('LOC_DO_NOT_STOP_ID')
                }];
                var prefixText = hx.l('LOC_STOP_ID') + ' : ';
                var i, ti;
                for (i = 0, ti = streamingList.length; i < ti; i += 1) {
                    item = streamingList[i];
                    if (item) {
                        name = item.getTitle();
                        if (!name || (name.length === 0)) {
                            name = item.getChannelNo() + ' ' + item.getChannelName();
                        }
                        textList.push({
                            '_one': prefixText,
                            '_two': _fnGetEllipsisText(name, 36),
                            '_three': 'lis',
                            '_index': i,
                            '_obj': item
                        });
                    }
                }
                var startTime, endTime;
                for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                    item = currentRecordingList[i];
                    if (item) {
                        name = item.getTitle();
                        if (!name || (name.length === 0)) {
                            name = item.getChannelNo() + ' ' + item.getChannelName();
                        }
                        startTime = item.getStartTime();
                        endTime = new Date(startTime.getTime() + (item.getDuration() * 1000));
                        textList.push({
                            '_one': prefixText,
                            '_two': _fnGetEllipsisText(name, 22) + ', ' + startTime.format('HH:MM') + '-' + endTime.format('HH:MM') + ', ' + startTime.format('mm.dd'),
                            '_three': 'rec',
                            '_index': i,
                            '_obj': item
                        });
                    }
                }
                var cancelRecText, msgCancelRecText;
                if (ENV.listModel.IR4000HD === ENV.model) {
                    cancelRecText = hx.l('LOC_CANCEL_ONLY_RECORD_ID');
                    msgCancelRecText = hx.l('LOC_MSG_CANCEL_ONLY_REC_ID');
                }
                else {
                    cancelRecText = hx.l('LOC_STOP_RECORD_1_ID');
                    msgCancelRecText = hx.l('LOC_MSG_CANCEL_REC_ID');
                }
                var cancelWhichRecText = hx.l('LOC_STOP_RECORD_DESP_ID');
                var cancelDlgParam = {
                    'title': cancelRecText,
                    'text': (currentRecordingList.length > 1) ? cancelWhichRecText : msgCancelRecText,
                    'list_arr': textList,
                    'auto_close': true,
                    'timeout': 30000,
                    'callback_fn': function (_status, _data) {
                        that._popup = null;
                        if (finished) {
                            finished();
                        }
                        if (_status === 'stop') {
                            var item;
                            if (_data) {
                                item = _data._obj;
                                if (item) {
                                    if (item instanceof __MODEL__.IRecordingItem) {
                                        dsrvRecordings.stopRecord({
                                            item: item
                                        });
                                    }
                                    else if (item instanceof __MODEL__.IScheduledItem) {
                                        dsrvRecordings.removeScheduledRecording({
                                            items: [item]
                                        });
                                    }
                                }
                            }
                        }
                    }
                };
                if (!that._popup) {
                    that._popup = hx.msg('cancelRecording', cancelDlgParam);
                }
            };
            var _actionExceptionHandling = function (cancelItem, requestedScheduledItem) {
                if (cancelItem) {
                    var tempItem = (cancelItem instanceof Array) ? cancelItem[0] : cancelItem;
                    if (tempItem.isRecordingOnGoing && tempItem.isRecordingOnGoing()) {
                        hx.log("CPgRecControl", "_actionExceptionHandling Stop title : [" + tempItem.getTitle() + "]");
                        hx.log("CPgRecControl", "_actionExceptionHandling Stop ch id : [" + tempItem.getChannelId() + "]");
                        hx.log("CPgRecControl", "_actionExceptionHandling Stop start Time : [" + Math.floor(tempItem.getStartTime() / 1000) + "]");
                        var currentRecordingList = dsrvRecordings.getCurrentRecordings();
                        var i = 0, ti = 0, recitem, recChID, itemChID;
                        if (currentRecordingList && currentRecordingList.length) {
                            itemChID = tempItem.getChannelId();
                            for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                                recitem = currentRecordingList[i];
                                if (recitem) {
                                    recChID = recitem.getChannelId();
                                    if (recChID === itemChID) {
                                        if (recitem instanceof __MODEL__.IRecordingItem) {
                                            dsrvRecordings.stopRecord({
                                                item: recitem
                                            });
                                        }
                                        else if (recitem instanceof __MODEL__.IScheduledItem) {
                                            dsrvRecordings.removeScheduledRecording({
                                                items: [recitem]
                                            });
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (cancelItem && requestedScheduledItem) {
                    hx.log("CPgRecControl", "_actionExceptionHandling Start title : [" + requestedScheduledItem.getTitle() + "]");
                    hx.log("CPgRecControl", "_actionExceptionHandling Start ch id : [" + requestedScheduledItem.getChannelId() + "]");
                    hx.log("CPgRecControl", "_actionExceptionHandling Start start Time : [" + Math.floor(requestedScheduledItem.getStartTime() / 1000) + "]");
                    var factoryType = requestedScheduledItem.isRecording() ? 0 : 1;
                    var bSeries = requestedScheduledItem.isSeries() ? 10 : 0;
                    var bTimeBased = requestedScheduledItem.getProgrammeID() ? false : true;
                    var cancelItems = (cancelItem instanceof Array) ? cancelItem.slice(0) : [cancelItem];
                    dsrvRecordings.removeScheduledRecording({
                        items: cancelItems,
                        delFirstSeries: true,
                        finished: function () {
                            setTimeout(function () {
                                if (bTimeBased) {
                                    var requestParam = {
                                        startTime: Math.floor(requestedScheduledItem.getStartTime() / 1000),
                                        duration: requestedScheduledItem.getDuration(),
                                        repeatDays: requestedScheduledItem.getRepeat(),
                                        ccid: requestedScheduledItem.getChannelId(),
                                        finished: function (result) {
                                        }
                                    };
                                    if (factoryType !== 0 /* ERECORDING */) {
                                        dsrvRecordings.recordAtReminder(requestParam);
                                    }
                                    else {
                                        dsrvRecordings.recordAtRecording(requestParam);
                                    }
                                }
                                else {
                                    dsrvEpg.findProgrammes({
                                        channel: [requestedScheduledItem.getChannelInstance()],
                                        clause: 'programmeID == ' + requestedScheduledItem.getProgrammeID(),
                                        isOnAir: false,
                                        cb: function (result) {
                                            if (result.length <= 0) {
                                                result = undefined;
                                                return;
                                            }
                                            var eventArray, programmeItem, thumbnailUrl;
                                            eventArray = result[0];
                                            if (eventArray && eventArray.length) {
                                                programmeItem = eventArray[0];
                                                if (programmeItem) {
                                                    if (requestedScheduledItem.isSeries && requestedScheduledItem.isSeries()) {
                                                        dsrvRecordings.record({
                                                            programme: programmeItem.getPluginObject(),
                                                            factoryType: bSeries,
                                                            finished: function () {
                                                            }
                                                        });
                                                    }
                                                    else {
                                                        dsrvRecordings.record({
                                                            programme: programmeItem.getPluginObject(),
                                                            factoryType: factoryType,
                                                            finished: function () {
                                                            }
                                                        });
                                                    }
                                                }
                                            }
                                            result = undefined;
                                        }
                                    });
                                }
                            }, 1000);
                        }
                    });
                }
            };
            var _showConflictWindow = function (aParam, aFinished) {
                var _calcModifyEndTime = function (aNewSchedule, aConflictList) {
                    var i, ti, cftSubList, cftSchedule;
                    var nowSec = hx.il.nowSec();
                    var startTime = Math.floor(aNewSchedule.getStartTime() / 1000);
                    var endTime = startTime + aNewSchedule.getDuration();
                    var modifyEndTime = endTime;
                    var bModify = true;
                    for (i = 0, ti = aConflictList.length; i < ti; i += 1) {
                        if (!bModify) {
                            break;
                        }
                        cftSubList = aConflictList[i];
                        if (cftSubList && cftSubList.length) {
                            cftSchedule = cftSubList[0];
                            var cftst = Math.floor(cftSchedule.getStartTime() / 1000);
                            if (cftst < startTime) {
                                bModify = false;
                                break;
                            }
                            else {
                                if ((cftst > nowSec) && (cftst < modifyEndTime)) {
                                    modifyEndTime = cftst;
                                }
                                if (cftSubList.length > 1) {
                                    var j, tj, subSchedule;
                                    for (j = 1, tj = cftSubList.length; j < tj; j += 1) {
                                        subSchedule = cftSubList[j];
                                        cftst = Math.floor(subSchedule.getStartTime() / 1000);
                                        if (cftst < startTime) {
                                            bModify = false;
                                            break;
                                        }
                                        if ((cftst > nowSec) && (cftst < modifyEndTime)) {
                                            modifyEndTime = cftst;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (modifyEndTime === endTime) {
                        modifyEndTime = 0;
                    }
                    else {
                        if ((modifyEndTime - startTime) < (5 * 60)) {
                            modifyEndTime = 0;
                        }
                        else {
                            modifyEndTime = modifyEndTime - 60;
                        }
                    }
                    return modifyEndTime;
                };
                var _getIcon = function (item) {
                    var icon;
                    if (item.isStreaming()) {
                        icon = 'lis';
                    }
                    else {
                        if (item.isReminder()) {
                            icon = 'remi';
                        }
                        else if (item.isRecordingOnGoing()) {
                            icon = 'rec';
                        }
                        else if (item.isSeries()) {
                            icon = 'seri';
                        }
                        else {
                            icon = 'rec2';
                        }
                    }
                    return icon;
                };
                var requestedScheduledItem = aParam.requestedScheduledItem;
                var conflictList = aParam.conflictList;
                var alternativeList = aParam.alternativeList;
                var fnResolveConflict = aParam.resolveConflict;
                var fnRetryWithDuration = aParam.retryWithDuration;
                var scheduleInfo = aParam.scheduleInfo;
                var requestedAction = aParam.requestedAction;
                var head, modifyEndTime;
                switch (requestedAction) {
                    case 'setDuration':
                        head = {
                            '_one': tr('LOC_CANCEL_CHAGE_RECDURATION_ID')
                        };
                        break;
                    case 'record':
                        if (requestedScheduledItem.isReminder()) {
                            head = {
                                '_one': tr('LOC_REC_CANCEL_NEW_REMINDER')
                            };
                        }
                        else {
                            head = {
                                '_one': tr('LOC_CANCEL_NEW_RECORDING_ID')
                            };
                        }
                        break;
                    default:
                        modifyEndTime = 0;
                        if (requestedScheduledItem.isReminder()) {
                            head = {
                                '_one': tr('LOC_REC_CANCEL_NEW_REMINDER')
                            };
                        }
                        else if (requestedScheduledItem.getProgrammeID() || (dsrvRecordings.getCurrentRecordings().length >= 4)) {
                            head = {
                                '_one': tr('LOC_CANCEL_NEW_RECORDING_ID')
                            };
                        }
                        else {
                            modifyEndTime = _calcModifyEndTime(requestedScheduledItem, conflictList);
                            if (!modifyEndTime) {
                                head = {
                                    '_one': tr('LOC_CANCEL_NEW_RECORDING_ID')
                                };
                            }
                            else {
                                head = {
                                    '_one': tr('LOC_RECORD_MODIFIED_DURATION_ID'),
                                    '_two': ' ( ~ ' + (new Date(modifyEndTime * 1000)).format("HH:MM") + ')'
                                };
                            }
                        }
                        break;
                }
                var text = hx.l('LOC_RECORDING_CONFLICT_TEXT_2_ID');
                var itemList = [head];
                var i, ti, headScheduledItem, subList;
                var recName;
                var item;
                var alProgramme;
                if (alternativeList) {
                    text = tr('LOC_CANNOT_BE_SCHEDULED_PROGRAMME_ID');
                    head._one += " : ";
                    head._two = "<span>" + _this.__makeDispTime({
                        'startTime': Math.floor(requestedScheduledItem.getStartTime() / 1000),
                        'duration': requestedScheduledItem.getDuration(),
                        'name': _fnGetEllipsisText(requestedScheduledItem.getTitle(), 22)
                    }, true) + "</span>";
                    var group = [];
                    var mapCRID = {};
                    var CRID;
                    var index;
                    var fnPush = function (aItem, aPrefix, aType, aProgramme) {
                        if (aProgramme === void 0) { aProgramme = null; }
                        var startTime;
                        var duration;
                        var text;
                        if (aProgramme) {
                            startTime = aProgramme.startTime;
                            duration = aProgramme.duration;
                            text = aProgramme.name;
                        }
                        else {
                            startTime = Math.floor(aItem.getStartTime() / 1000);
                            duration = aItem.getDuration();
                            text = aItem.getTitle();
                        }
                        var title = "<span>" + _this.__makeDispTime({
                            'startTime': startTime,
                            'duration': duration,
                            'name': _fnGetEllipsisText(text, 22)
                        }, true) + "</span>";
                        return itemList.push({
                            '_one': aPrefix,
                            '_two': title,
                            '_three': _getIcon(aItem),
                            '_subList': [],
                            '_subListNum': 0,
                            '_index': i,
                            '_recording': aItem,
                            '_type': aType,
                            '_series': [],
                            '_programme': aProgramme
                        }) - 1;
                    };
                    for (i = 0; i < conflictList.length; i++) {
                        item = conflictList[i];
                        if (item.isSeries()) {
                            CRID = item.getGroupCRIDs();
                            if (CRID) {
                                CRID = CRID[0];
                                index = group.indexOf(CRID);
                                if (index === -1) {
                                    group.push(CRID);
                                    mapCRID[CRID] = fnPush(item, hx.util.format('{0} : ', tr('LOC_CANCEL_ID')), "conflict");
                                }
                                else {
                                    itemList[mapCRID[CRID]]._series.push(item);
                                }
                            }
                        }
                        else {
                            fnPush(item, hx.util.format('{0} : ', tr('LOC_CANCEL_ID')), "conflict");
                        }
                    }
                    group.length = 0;
                    mapCRID = {};
                    for (i = 0; i < alternativeList.length; i++) {
                        item = alternativeList[i].schedule;
                        alProgramme = alternativeList[i].programme;
                        if (item.isSeries()) {
                            CRID = item.getGroupCRIDs();
                            if (CRID) {
                                CRID = CRID[0];
                                index = group.indexOf(CRID);
                                if (index === -1) {
                                    group.push(CRID);
                                    mapCRID[CRID] = fnPush(item, tr('LOC_RECORD_ALTERNATIVE_ID') + " : ", "alternative", alProgramme);
                                }
                                else {
                                    itemList[mapCRID[CRID]]._series.push(item);
                                }
                            }
                        }
                        else {
                            fnPush(item, tr('LOC_RECORD_ALTERNATIVE_ID') + " : ", "alternative", alProgramme);
                        }
                    }
                }
                else {
                    for (i = 0, ti = conflictList.length; i < ti; i += 1) {
                        subList = conflictList[i];
                        headScheduledItem = subList[0];
                        var oSubList = [];
                        if (subList.length > 1) {
                            var j, tj, subRec;
                            for (j = 0, tj = subList.length; j < tj; j += 1) {
                                subRec = subList[j];
                                oSubList.push({
                                    '_one': subRec.getTitle(),
                                    '_two': _getIcon(subRec),
                                    '_recording': subRec
                                });
                            }
                        }
                        if ((subList.length <= 1) && !(headScheduledItem.isStreaming())) {
                            recName = that.__makeDispTime({
                                'startTime': Math.floor(headScheduledItem.getStartTime() / 1000),
                                'duration': headScheduledItem.getDuration(),
                                'name': _fnGetEllipsisText(headScheduledItem.getTitle(), 22)
                            });
                        }
                        else {
                            recName = _fnGetEllipsisText(headScheduledItem.getTitle(), 24);
                        }
                        itemList.push({
                            '_one': hx.util.format('{0} : ', tr('LOC_CANCEL_ID')),
                            '_two': recName,
                            '_three': _getIcon(headScheduledItem),
                            '_subList': oSubList,
                            '_subListNum': oSubList.length,
                            '_index': i,
                            '_recording': headScheduledItem
                        });
                    }
                }
                var _conflictMsg = hx.msg('recordingConflict', {
                    'title': (requestedScheduledItem.isReminder() ? hx.l('LOC_REMINDER_CONFLICT_ID') : hx.l('LOC_RECORDING_CONFLICT_ID')),
                    'text': text,
                    'list_arr': itemList,
                    'dia_class': ((that.entity.getCenterPageName() === 'CPgLiveController')) ? undefined : 'osd',
                    'auto_close': true,
                    'timeout': 30000,
                    'callback_fn': function (_status, _data) {
                        that.sendEventToEntity({
                            alEvent: 'alReFocus',
                            prev: true
                        });
                        var handled = false;
                        if (aFinished) {
                            handled = !!aFinished(_status, _data);
                        }
                        if (!handled) {
                            if (_status === 'stop') {
                                //OK key
                                var schedule = _data._recording;
                                if (!schedule) {
                                    if (modifyEndTime) {
                                        setTimeout(function () {
                                            if (fnRetryWithDuration) {
                                                var modifyDuration = modifyEndTime - Math.floor(requestedScheduledItem.getStartTime() / 1000);
                                                fnRetryWithDuration(modifyDuration, scheduleInfo, requestedScheduledItem.getChannelId(), requestedScheduledItem.isReminder() ? 'reminder' : 'recording');
                                            }
                                        }, 0);
                                    }
                                    else {
                                        if (_data._cb) {
                                            _data._cb();
                                        }
                                        else {
                                            if ((fnResolveConflict === undefined) && (fnRetryWithDuration === undefined) && (scheduleInfo === undefined) && (requestedAction === undefined)) {
                                                item = requestedScheduledItem;
                                                if (item) {
                                                    hx.log("CPgRecControl", "ScheduledItem Delete titel : [" + item.getTitle() + "]");
                                                    hx.log("CPgRecControl", "ScheduledItem Delete ch id : [" + item.getChannelId() + "]");
                                                    hx.log("CPgRecControl", "ScheduledItem Delete start Time : [" + Math.floor(item.getStartTime() / 1000) + "]");
                                                    dsrvRecordings.removeScheduledRecording({
                                                        items: [item]
                                                    });
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (_data._subList.length <= 1) {
                                        setTimeout(function () {
                                            if (fnResolveConflict) {
                                                fnResolveConflict(schedule, scheduleInfo);
                                            }
                                            else {
                                                if ((fnResolveConflict === undefined) && (fnRetryWithDuration === undefined) && (scheduleInfo === undefined) && (requestedAction === undefined)) {
                                                    if (_data) {
                                                        item = schedule;
                                                        _actionExceptionHandling(item, requestedScheduledItem);
                                                    }
                                                }
                                            }
                                        }, 0);
                                    }
                                    else {
                                        var yesBtn = hx.l('LOC_YES_ID'), noBtn = hx.l('LOC_NO_ID');
                                        var subList = _data._subList;
                                        setTimeout(function () {
                                            that._subConflictMsg = hx.msg('recordingSubConflict', {
                                                'title': hx.l('LOC_RECORDING_CONFLICT_ID'),
                                                'text': hx.l('LOC_RECORDING_CONFLICT_DESP_ID'),
                                                'list_arr': _data._subList,
                                                'dia_class': (that.entity.getCenterPageName() === 'CPgLiveController') ? undefined : 'osd',
                                                'btn_title_arr': [yesBtn, noBtn],
                                                'auto_close': true,
                                                'timeout': 30000,
                                                'callback_fn': function (_status, _data) {
                                                    that.sendEventToEntity({
                                                        alEvent: 'alReFocus',
                                                        prev: true
                                                    });
                                                    switch (_status) {
                                                        case yesBtn:
                                                            var cancelList = [];
                                                            $.each(subList, function (i, v) {
                                                                if (v) {
                                                                    cancelList.push(v._recording);
                                                                }
                                                            });
                                                            setTimeout(function () {
                                                                if (fnResolveConflict) {
                                                                    fnResolveConflict(cancelList, scheduleInfo);
                                                                }
                                                            }, 0);
                                                            break;
                                                        case noBtn:
                                                        case 'key_back':
                                                            setTimeout(function () {
                                                                _showConflictWindow(aParam, aFinished);
                                                            }, 0);
                                                            break;
                                                    }
                                                }
                                            });
                                        }, 0);
                                    }
                                }
                            }
                        }
                    }
                });
            };
            var _showDurationChangeDlg = function (aParam, aFinished) {
                var that = _this;
                var nowSec = hx.il.nowSec();
                var scheduledItem = aParam.relatedScheduledRecording //
                , targetCh = aParam.targetChannel //
                , evtList = aParam.eventList //
                , currentEvtIdx = aParam.currentEventIndex //
                , currentEvt = aParam.currentEvnet;
                //param.relatedRecordingItem
                var endEvt = currentEvt;
                var recDurDlg, recDurDlgParam;
                var adjDurTime = scheduledItem.getDuration(), adjDurTimeByEvt;
                var scheduleStartTimeSec = Math.floor(scheduledItem.getStartTime() / 1000);
                var scheduleEndTime = scheduleStartTimeSec + scheduledItem.getDuration();
                var scheduleEndTimeStr = (new Date(scheduleEndTime * 1000)).format('HH:MM');
                var referTime;
                var first_arrow;
                var p_text;
                hx.log('CPgRecControl', 'scheduledItem -------------------');
                hx.log('CPgRecControl', 'getStartTime ' + scheduledItem.getStartTime());
                hx.log('CPgRecControl', 'getDuration ' + scheduledItem.getDuration());
                hx.log('CPgRecControl', 'scheduleEndTimeStr ' + scheduleEndTimeStr);
                if (nowSec < scheduleStartTimeSec) {
                    referTime = scheduleStartTimeSec;
                }
                else {
                    referTime = nowSec;
                }
                hx.log('CPgRecControl', 'referTime -------------------');
                hx.log('CPgRecControl', 'nowSec ' + nowSec);
                hx.log('CPgRecControl', 'referTime ' + referTime);
                p_text = targetCh.majorChannel + ' ' + targetCh.name + ' (' + (scheduledItem.getStartTime()).format('HH:MM') + '-' + scheduleEndTimeStr + ', ' + Math.round(scheduledItem.getDuration() / 60) + hx.l('LOC_MINS_ID') + ')';
                //duration less then 10 minutes OR schedule remaining time less then 10 minutes
                if ((scheduledItem.getDuration() <= 600) || (referTime > (scheduleEndTime - 600))) {
                    first_arrow = 'right';
                }
                else if ((scheduledItem.getDuration() + 600) >= 86400) {
                    first_arrow = 'left';
                }
                else {
                    first_arrow = '';
                }
                recDurDlgParam = {
                    'title': hx.l('LOC_CHANGE_RECDUR_ID'),
                    'text': hx.l('LOC_MSG_CHANGE_RECDUR_ID'),
                    'p_text': p_text,
                    'first_arrow': first_arrow,
                    'first_text': hx.l('LOC_CHANGE_RECDUR_TIME_ID') + ' (~' + scheduleEndTimeStr + ')',
                    'first_step': '10',
                    'second_show': false,
                    'auto_close': true,
                    'timeout': 30000,
                    'callback_fn': function (_status, _value) {
                        switch (_status) {
                            case 'first_left':
                            case 'first_right':
                                if (_status === 'first_left') {
                                    adjDurTime -= 600;
                                }
                                else {
                                    adjDurTime += 600;
                                }
                                var endTimeSec = scheduleStartTimeSec + adjDurTime;
                                var arrowDirection;
                                if (((adjDurTime - 600) <= 0) || ((endTimeSec - 660) < referTime)) {
                                    arrowDirection = 'right';
                                }
                                else {
                                    if ((adjDurTime + 600) >= 86400) {
                                        arrowDirection = 'left';
                                    }
                                    else {
                                        arrowDirection = '';
                                    }
                                }
                                recDurDlg.setListTextArrow('first', hx.l('LOC_CHANGE_RECDUR_TIME_ID') + ' (~' + (new Date(endTimeSec * 1000)).format('HH:MM') + ')', arrowDirection);
                                break;
                            case 'second_left':
                            case 'second_right':
                                if (_status === 'second_left') {
                                    currentEvtIdx -= 1;
                                }
                                else {
                                    currentEvtIdx += 1;
                                }
                                var programme = evtList[currentEvtIdx];
                                if (programme) {
                                    adjDurTimeByEvt = programme.getEndTime() - scheduleStartTimeSec;
                                    if (currentEvtIdx >= evtList.length - 1) {
                                        arrowDirection = 'left';
                                    }
                                    else {
                                        if (currentEvtIdx <= 0) {
                                            arrowDirection = 'right';
                                        }
                                        else {
                                            if (evtList[currentEvtIdx - 1].getEndTime() < referTime) {
                                                arrowDirection = 'right';
                                            }
                                            else {
                                                arrowDirection = '';
                                            }
                                        }
                                    }
                                    recDurDlg.setListTextArrow('second', hx.l('LOC_CHANGE_RECDUR_PROG_ID') + ' "' + programme.getName() + '"', arrowDirection, ' (~' + (new Date(programme.getEndTime() * 1000)).format('HH:MM') + ')');
                                }
                                break;
                            case 'ok':
                                var chDurTime = 0;
                                if (_value.index === 0) {
                                    chDurTime = adjDurTime;
                                }
                                else {
                                    chDurTime = adjDurTimeByEvt;
                                }
                                for (var i = 0; i < evtList.length; i++) {
                                    if (evtList[i].endTime < endTimeSec) {
                                        endEvt = evtList[i];
                                    }
                                    else {
                                        break;
                                    }
                                }
                                if (chDurTime !== scheduledItem.getDuration()) {
                                    hx.log('CPgRecControl', 'chDurTime----------- ' + chDurTime);
                                    dsrvRecordings.setDuration({
                                        scheduledRecording: scheduledItem,
                                        duration: chDurTime,
                                        finished: aFinished
                                    });
                                    that.sendEventToEntity({
                                        alEvent: 'alMessageToPage',
                                        target: 'CPgMain/changeDuration?' + chDurTime
                                    });
                                }
                                else {
                                    if (aFinished) {
                                        aFinished();
                                    }
                                }
                                break;
                            default:
                                if (aFinished) {
                                    aFinished();
                                }
                                break;
                        }
                    }
                };
                if (evtList && evtList.length) {
                    if (!currentEvt) {
                        currentEvt = evtList[0];
                        endEvt = currentEvt;
                        currentEvtIdx = 0;
                    }
                    recDurDlgParam.second_show = true;
                    recDurDlgParam.second_text = hx.l('LOC_CHANGE_RECDUR_PROG_ID') + ' "' + endEvt.getName();
                    recDurDlgParam.second_time = '" (~' + scheduleEndTimeStr + ')';
                    if (currentEvtIdx <= 0) {
                        recDurDlgParam.second_arrow = 'right';
                    }
                    else {
                        if (evtList[evtList.length - 1].getEndTime() < referTime) {
                            recDurDlgParam.second_arrow = 'right';
                        }
                        else {
                            recDurDlgParam.second_arrow = '';
                        }
                    }
                    adjDurTimeByEvt = endEvt.getEndTime() - scheduleStartTimeSec;
                }
                recDurDlg = hx.msg('recordDuration', recDurDlgParam);
                return recDurDlg;
            };
            dsrvRecordings.addEventCb('ShowRecordingStopWindow', that, function (param) {
                _fnShowStopRec(param, function () {
                    var noWork;
                });
            });
            dsrvRecordings.addEventCb('ShowConflictWindow', that, function (param) {
                _showConflictWindow(param, function (_status, _data) {
                    var noWork;
                });
            });
            dsrvRecordings.addEventCb('ShowupTPConflictPopup', that, function (param) {
                showupTPConflictPopup(param);
            });
            dsrvRecordings.addEventCb('NotiFoundRecommendationProgramme', that, function (aProgramme) {
                var itemList = [];
                var fnPush = function (aItem, aPrefix, aType, aProgramme) {
                    if (aProgramme === void 0) { aProgramme = null; }
                    var title = "<span>" + _this.__makeDispTime({
                        'startTime': Math.floor(aItem.getStartTime() / 1000),
                        'duration': aItem.getDuration(),
                        'name': _fnGetEllipsisText(aItem.getTitle(), 22)
                    }, true) + "</span>";
                    return itemList.push({
                        '_one': aPrefix,
                        '_two': title,
                        '_subList': [],
                        '_subListNum': 0,
                        '_index': i,
                        '_recording': aItem,
                        '_type': aType,
                        '_series': [],
                        '_programme': aProgramme
                    }) - 1;
                };
                var fnMakeMenuItem = function (aName) {
                    itemList.push({
                        _one: aName,
                        _programme: aProgramme
                    });
                };
                if (aProgramme.pluginObject.getField("SeriesID")) {
                    fnMakeMenuItem(hx.l('LOC_SERIESREC_BTN_ID'));
                }
                fnMakeMenuItem(hx.l('LOC_TIMERREC_BTN1_ID'));
                fnMakeMenuItem(hx.l('LOC_DO_NOT_RECORD_ID'));
                var channelStr = hx.il.makeDigitNumber(aProgramme.getChannelNum()) + ' ' + aProgramme.getChannelName();
                var startStr = '', startTime = aProgramme.getStartTime(), startDateStr, startDateTime;
                if (startTime) {
                    startDateTime = new Date(startTime * 1000);
                    startStr = startDateTime.format('HH:MM');
                    startDateStr = startDateTime.format('dd.mm');
                }
                var endStr = '', endTime = aProgramme.getEndTime(), endDateTime;
                if (endTime) {
                    endDateTime = new Date(endTime * 1000);
                    endStr = endDateTime.format('HH:MM');
                }
                var eventStr = aProgramme.getName() + ' (' + startStr + ' - ' + endStr + ', ' + startDateStr + ')';
                var text2 = channelStr + '<br>' + eventStr;
                hx.msg('recordingConflict', {
                    'title': hx.l('LOC_RECORD_RECOMMENDATION_ID'),
                    'text': hx.l('LOC_RECOMMENDED_AVAILABLE_ID'),
                    'text2': text2,
                    'list_arr': itemList,
                    'dia_class': ((that.entity.getCenterPageName() === 'CPgLiveController')) ? undefined : 'osd',
                    'auto_close': true,
                    'timeout': 30000,
                    'callback_fn': function (_status, _data) {
                        if (_status === 'stop') {
                            var eType = __MODEL__.eTScheduledFactoryType;
                            var factoryType = 0 /* ERECORDING */;
                            if (_data._one === hx.l('LOC_TIMERREC_BTN1_ID')) {
                                factoryType = 0 /* ERECORDING */;
                            }
                            else if (_data._one === hx.l('LOC_SERIESREC_BTN_ID')) {
                                factoryType = 10 /* ESERIES_RECORDING */;
                            }
                            else {
                                return;
                            }
                            dsrvRecordings.record({
                                programme: _data._programme.pluginObject,
                                factoryType: factoryType,
                                finished: function () {
                                    hx.log("info", "[RECOMMENDATION HANDLER] recommendation END");
                                }
                            });
                        }
                    }
                });
            });
            dsrvRecordings.addEventCb('ShowAlternativeWindow', that, function (param) {
                _showConflictWindow(param, function (_status, _data) {
                    var handled = false;
                    var newRec = param.requestedScheduledItem;
                    var fnCancelRecording = function (aSchedule) {
                        if (!aSchedule) {
                            hx.log("debug", "[ALTERNATIVE HANDLER] : cancel fail! aSchedule is null");
                            return;
                        }
                        hx.log("info", "[ALTERNATIVE HANDLER] : _type === undefined : Cancel new recording");
                        hx.log("info", "[ALTERNATIVE HANDLER] hx.svc.RECORDINGS.removeScheduledRecording");
                        hx.log("info", "[ALTERNATIVE HANDLER] newRec.getTitle() : " + aSchedule.getTitle());
                        dsrvRecordings.removeScheduledRecording({
                            items: [aSchedule]
                        });
                    };
                    hx.log("info", "[ALTERNATIVE HANDLER] : _status : " + _status);
                    if (_status === 'stop') {
                        var selected = _data._recording;
                        var type = _data._type;
                        hx.log("info", "[ALTERNATIVE HANDLER] : _type : " + type);
                        if (!type) {
                            fnCancelRecording(newRec);
                        }
                        else {
                            var willBeRemoved = _data._series.concat(selected);
                            var factoryType = 11 /* EALTERNATIVE_RECORDING */;
                            if (selected.isSeries()) {
                                factoryType = 12 /* EALTERNATIVE_SERIES_RECORDING */;
                            }
                            hx.log("info", "[ALTERNATIVE HANDLER] hx.svc.RECORDINGS.removeScheduledRecording");
                            for (var i = 0; i < willBeRemoved.length; i++) {
                                hx.log("info", "[ALTERNATIVE HANDLER] willBeRemoved[" + i + "].getTitle() : " + willBeRemoved[i].getTitle());
                            }
                            if (type === "conflict") {
                                if (!param.scheduleInfo) {
                                    hx.log("warning", "[ALTERNATIVE HANDLER] something wrong may hx.svc.RECORDINGS's private state variable mRecCtlStatEm.state is REQ");
                                    hx.log("warning", "[ALTERNATIVE HANDLER] Therefore we make a new data by using param.requestedScheduledItem");
                                    _actionExceptionHandling(willBeRemoved, newRec);
                                    return;
                                }
                                dsrvRecordings.removeScheduledRecording({
                                    items: willBeRemoved,
                                    finished: function () {
                                        if (!param || !param.scheduleInfo) {
                                            hx.log("debug", "[ALTERNATIVE HANDLER] record(conflict) fail!! param.scheduleInfo is null");
                                            return;
                                        }
                                        hx.log("info", "[ALTERNATIVE HANDLER] hx.svc.RECORDINGS.record");
                                        hx.log("info", "[ALTERNATIVE HANDLER] param.scheduleInfo.programme.name : " + param.scheduleInfo.programme.name);
                                        hx.log("info", "[ALTERNATIVE HANDLER] param.scheduleInfo.factoryType : " + param.scheduleInfo.factoryType);
                                        dsrvRecordings.record({
                                            programme: param.scheduleInfo.programme,
                                            factoryType: param.scheduleInfo.factoryType,
                                            finished: function () {
                                                hx.log("info", "[ALTERNATIVE HANDLER] conflict END");
                                            }
                                        });
                                    }
                                });
                            }
                            else if (type === "alternative") {
                                dsrvRecordings.removeScheduledRecording({
                                    items: willBeRemoved,
                                    finished: function () {
                                        //var alternativeSchedule: __MODEL__.COipfScheduledItem = _data._recording;
                                        var fnRecordAlternative = function () {
                                            if (!_data || !_data._programme) {
                                                hx.log("debug", "[ALTERNATIVE HANDLER] record(alternative) fail!! _data._programme is null");
                                                return;
                                            }
                                            hx.log("info", "[ALTERNATIVE HANDLER] hx.svc.RECORDINGS.record");
                                            hx.log("info", "[ALTERNATIVE HANDLER] _data._programme.name : " + _data._programme.name);
                                            hx.log("info", "[ALTERNATIVE HANDLER] _data._programme.programmeID : " + _data._programme.programmeID);
                                            hx.log("info", "[ALTERNATIVE HANDLER] factoryType : " + factoryType);
                                            dsrvRecordings.record({
                                                programme: _data._programme,
                                                factoryType: factoryType,
                                                finished: function () {
                                                    hx.log("info", "[ALTERNATIVE HANDLER] alternative END");
                                                }
                                            });
                                        };
                                        var fnRecordUserSelectedProgramme = function () {
                                            if (!param || !param.scheduleInfo) {
                                                hx.log("debug", "[ALTERNATIVE HANDLER] record(alternative) fail!! param.scheduleInfo is null");
                                                return;
                                            }
                                            hx.log("info", "[ALTERNATIVE HANDLER] hx.svc.RECORDINGS.record");
                                            hx.log("info", "[ALTERNATIVE HANDLER] param.scheduleInfo.programme.name : " + param.scheduleInfo.programme.name);
                                            hx.log("info", "[ALTERNATIVE HANDLER] param.scheduleInfo.factoryType : " + param.scheduleInfo.factoryType);
                                            dsrvRecordings.record({
                                                programme: param.scheduleInfo.programme,
                                                factoryType: param.scheduleInfo.factoryType,
                                                finished: function () {
                                                    fnRecordAlternative();
                                                }
                                            });
                                        };
                                        //if (alternativeSchedule.getProgrammeID() === _data._recording.getProgrammeID()) {
                                        fnRecordAlternative();
                                        //} else {
                                        //    fnRecordUserSelectedProgramme();
                                        //}
                                    }
                                });
                            }
                        }
                        handled = true;
                    }
                    else if (_status === "auto_close") {
                        fnCancelRecording(newRec);
                        handled = true;
                    }
                    return handled;
                });
            });
            dsrvRecordings.addEventCb('ShowChangeDuration', that, function (param) {
                if (!that._popup) {
                    that._popup = _showDurationChangeDlg(param, function () {
                        var noWork;
                        that._popup = null;
                    });
                }
            });
            var _fnLivePageNow = function () {
                return (that.getCenterPageName() === 'CPgLiveController');
            };
            dsrvRecordings.addEventCb('NotiRecordingStarted', that, function (param) {
                var startedItem = param.item;
                if (that.getCenterPageName() === 'CPgStandby') {
                    return;
                }
                if (that._startMsg) {
                    hx.il.ani.reset(that._startMsg.$, '');
                    hx.msg.close('recordStatus', that._startMsg);
                    that._startMsg = undefined;
                }
                that._startMsg = hx.msg('recordStatus', {
                    'withIcon': "icoRec",
                    'text': hx.l('LOC_START_RECORD_ID'),
                    'text_detail': that.__makeDispTime({
                        'startTime': startedItem.getStartTime() / 1000,
                        'duration': startedItem.getDuration(),
                        'name': startedItem.getTitle()
                    }),
                    'auto_close': true,
                    'timeout': 3000,
                    'callback_fn': function (_msg) {
                        if (_msg === 'auto_close') {
                            that._startMsg = undefined;
                        }
                    }
                });
                if (dsrvRecordings.getRecordingList().length >= (that.NUM_MAX_RECORD_LIST - 100)) {
                    if (!that._notiNumOfRecordingsIsReachingLimitMsg) {
                        that._notiNumOfRecordingsIsReachingLimitMsg = hx.msg('confirm', {
                            text: "The number of recordings reaches the limit that up to 1500 recordings can be saved. Free up the storage.",
                            auto_close: false,
                            btn_title_arr: ['OK'],
                            callback_fn: function (_msg) {
                            }
                        });
                    }
                }
                else {
                    that._notiNumOfRecordingsIsReachingLimitMsg = null;
                }
                if (_fnLivePageNow()) {
                    var szText;
                    if (hx.config.usefrontPanel === 'segment7') {
                        szText = 'REC';
                    }
                    else {
                        szText = 'START RECORD';
                    }
                    dsrvSettingUtil.setFrontPanelString({
                        text: szText
                    });
                    setTimeout(function () {
                        if (_fnLivePageNow()) {
                            dsrvSettingUtil.setFrontPanelString({
                                text: ''
                            });
                        }
                    }, 2000);
                }
                // check channel validity when recording start (prevent black screen)
                if (hx.svc.AV_CTRL.isLiveOnGoing() === true) {
                    hx.log('CPgRecControl', '>> Recording started >> Check validate of Live !!!');
                    var curChObj = hx.svc.CH_LIST.getCurrentChannel();
                    if (curChObj.channel) {
                        var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel: curChObj.channel
                        });
                        if (bChangeable === false) {
                            hx.log('warning', '>> current channel is not available try tune to valid channel !!!!!!!');
                            // tune to availalbe channel
                            var channel = startedItem.getChannelInstance();
                            if (!!channel) {
                                var chInfo = hx.svc.CH_LIST.getAvailableGroupForChannel({
                                    'channel': channel
                                });
                                if (!!chInfo.group) {
                                    // check tunable
                                    bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                                        channel: chInfo.channel
                                    });
                                    if (bChangeable === true) {
                                        hx.svc.AV_CTRL.channelChange({
                                            channel: chInfo.channel,
                                            group: chInfo.group
                                        });
                                    }
                                }
                            }
                        }
                        else {
                            hx.log('CPgRecControl', '>> Recording started >> Live is valid !!!');
                        }
                    }
                }
            });
            dsrvRecordings.addEventCb('NotiRecordingStop', that, function (param) {
                var item = param.item;
                var isRecordingDeleted = param.deleted;
                if (that.getCenterPageName() === 'CPgStandby') {
                    return;
                }
                if (isRecordingDeleted) {
                    if (item && item.isRecordingOnGoing()) {
                        if (item.getRecordingDuration() < 30) {
                            hx.log('CPgRecControl', '\t fail to record >> rec time is less than 30 sec');
                            that._startMsg = hx.msg('recordStatus', {
                                withIcon: false,
                                text: hx.l('LOC_MSG_REC_30SEC_ID'),
                                auto_close: true,
                                timeout: 3000
                            });
                        }
                        else {
                            hx.log('CPgRecControl', '\t fail to record >> unknown error');
                            that._startMsg = hx.msg('recordStatus', {
                                withIcon: false,
                                text: hx.l('LOC_VIDEO_RECORD_FAIL_UNKNOWN'),
                                auto_close: true,
                                timeout: 3000
                            });
                        }
                    }
                }
                else {
                    if (that._startMsg) {
                        hx.il.ani.reset(that._startMsg.$, '');
                        hx.msg.close('recordStatus', that._startMsg);
                        that._startMsg = undefined;
                    }
                    that._startMsg = hx.msg('recordStatus', {
                        'withIcon': false,
                        'text': hx.l('LOC_STOP_RECORD_ID'),
                        'text_detail': that.__makeDispTime({
                            'startTime': item.getStartTime() / 1000,
                            'duration': item.getRecordingDuration(),
                            'name': item.getTitle()
                        }),
                        'auto_close': true,
                        'timeout': 3000,
                        'callback_fn': function (_msg) {
                            if (_msg === 'auto_close') {
                                that._startMsg = undefined;
                            }
                        }
                    });
                }
            });
            dsrvRecordings.addEventCb('NotiRecordingStartError', that, function (param) {
                var item = param.item;
                var reason = param.reason;
                var bScheduled = param.bScheduled;
                if (that.getCenterPageName() === 'CPgStandby') {
                    return;
                }
                switch (reason) {
                    case 'INSUFFICIENT_STORAGE':
                        if (that._statusMsg) {
                            hx.msg.close('response', that._statusMsg);
                        }
                        if (ENV.listModel.FVP4000T === ENV.model) {
                            that._statusMsg = hx.msg('response', {
                                text: hx.l('LOC_HDD_ALMOST_FULL_ID'),
                                auto_close: true,
                                timeout: 3000
                            });
                        }
                        else {
                            that._statusMsg = hx.msg('response', {
                                text: hx.l('LOC_REC_CANNOTSTART_DUETO_HDDFULL'),
                                auto_close: true,
                                timeout: 3000
                            });
                        }
                        break;
                    case 'MAX_RECORD_LIST':
                        if (that._failScheduledRecordMsg) {
                            hx.msg.close('ssuDialog', that._failScheduledRecordMsg);
                        }
                        if (that._failRecordMsg) {
                            hx.msg.close('confirm', that._failRecordMsg);
                        }
                        if (bScheduled) {
                            that._failScheduledRecordMsg = hx.msg('ssuDialog', {
                                text: "Recording your scheduled programme has failed because up to 1500" + " recordings are created only. Go to RECORDINGS menu and free up the storage and try again.",
                                first_line_arr: [_this.__makeDispTime({
                                    'startTime': Math.floor(item.getStartTime() / 1000),
                                    'duration': item.getDuration(),
                                    'name': _fnGetEllipsisText(item.getTitle(), 22)
                                }, true)],
                                auto_close: false,
                                btn_title_arr: ['OK'],
                                callback_fn: function (param) {
                                    if (param === 'OK') {
                                        // jump to Recordings
                                        setTimeout(function () {
                                            that.sendEventToEntity({
                                                alEvent: 'alChangePage',
                                                target: 'CPgRecordings',
                                                me: that
                                            });
                                        }, 0);
                                    }
                                    else if (param === 'key_esc' || param === 'key_back') {
                                    }
                                }
                            });
                        }
                        else {
                            that._failRecordMsg = hx.msg('confirm', {
                                text: "You cannot record this programme because up to 1500 recordings are created only. Go to" + " RECORDINGS and free up the storage and try again.",
                                auto_close: false,
                                btn_title_arr: ['OK'],
                                callback_fn: function (param) {
                                    if (param === 'OK') {
                                        // jump to Recordings
                                        setTimeout(function () {
                                            that.sendEventToEntity({
                                                alEvent: 'alChangePage',
                                                target: 'CPgRecordings',
                                                me: that
                                            });
                                        }, 0);
                                    }
                                    else if (param === 'key_esc' || param === 'key_back') {
                                    }
                                }
                            });
                        }
                        break;
                    default:
                        break;
                }
            });
            function showupTPConflictPopup(param) {
                var def = param.resultDef;
                that._reminderMsg = prism.createReminderDlg(null, {
                    title: param.title,
                    eventName: param.eventName,
                    channelNo: hx.il.makeDigitNumber(param.channelNo),
                    channelName: param.channelName,
                    startTime: param.startTime,
                    endTime: param.endTime
                }, function (yes) {
                    hx.log('CPgRecControl', '[ScheduleEvent] reminder yes :' + yes);
                    if (yes == 1) {
                        def.resolve(param);
                    }
                    else {
                        def.reject(null);
                    }
                });
                that._reminderMsg.open({
                    onClosed: function (reason) {
                        hx.log('CPgRecControl', '[ScheduleEvent] reminder onClose :' + reason);
                        if (reason === 'timedOut') {
                            def.resolve(param);
                        }
                        that._reminderMsg = null;
                    }
                });
            }
            dsrvRecordings.addEventCb('NotiScheduledStartError', that, function (param) {
                var item = param.item;
                var text = null;
                var dlg = null;
                var tpConflictParam = param.tpConflictParam;
                var bValidOnNetflix;
                //var ch = recording.channel;
                hx.log('CPgRecControl', '[NotiScheduledStartError] startTime ' + item.getStartTime() + ' duration ' + item.getDuration() + ' name ' + item.getTitle());
                if (that.getCenterPageName() === 'CPgStandby') {
                    return;
                }
                if (that._reminderMsg !== null) {
                    dsrvRecordings.recordConfirm({
                        scheduledRecording: item,
                        type: 0,
                        check: 1
                    });
                    return;
                }
                if (!item.isReminder()) {
                    if (hx.il.criticalTasking.crOTATasking === true) {
                        // ScheduledWillBeStart shows pop-up dialog as well.
                        hx.log('CPgRecControl', '[ScheduleEvent] recording CRITICAL TASKING !!!!! (-)');
                        return;
                    }
                    //this scenario be implmented on dl_dsrvRecordings2.ts
                    //check ch Num & start Time & duration & ch Num
                    if (tpConflictParam) {
                        var channelNo = hx.il.makeDigitNumber(item.getChannelNo());
                        var startTime = item.getStartTime();
                        var endTime = item.getEndTime();
                        if ((tpConflictParam.channelNo == channelNo) && (tpConflictParam.startTime.getTime() == startTime.getTime()) && (tpConflictParam.endTime.getTime() == endTime.getTime())) {
                            text = null;
                        }
                        else {
                            text = hx.l('LOC_RECWILLSTARTMESSAGE_ID');
                        }
                    }
                    else {
                        text = hx.l('LOC_RECWILLSTARTMESSAGE_ID');
                    }
                }
                else {
                    bValidOnNetflix = true;
                    text = hx.l('LOC_REMINDERMESSAGE_ID') + ' ' + hx.l('LOC_REMINDERMESSAGE2_ID');
                    if (hx.il.isCriticalTasking() === true) {
                        hx.log('CPgRecControl', '[ScheduleEvent] reminder CRITICAL TASKING !!!!! (-)');
                        dsrvRecordings.recordConfirm({
                            scheduledRecording: item,
                            type: 0,
                            check: 0 //no
                        });
                        return;
                    }
                }
                var setResult = function (item, res) {
                    var channel, bChangeable = false;
                    if ((res === 1) && item) {
                        hx.log('CPgRecControl', '[ScheduleEvent] reminder result : yes');
                        channel = item.getChannelInstance();
                        // check tunable
                        bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                            channel: channel
                        });
                        if (bChangeable === false) {
                            if (hx.config.useTVStreaming === false) {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            dsrvRecordings.recordConfirm({
                                scheduledRecording: item,
                                type: 0,
                                check: res
                            });
                            return;
                        }
                        // set last channel
                        var o = hx.svc.CH_LIST.getAvailableGroupForChannel({
                            'channel': channel
                        });
                        if (o.channel !== null && o.group !== null) {
                            hx.svc.WEB_STORAGE.updateLastService({
                                'ccid': o.channel.ccid,
                                'group': o.group
                            });
                        }
                        var dsrvAppMgr = hx.svc.APP_MGR;
                        var dsrvAvCtrl = hx.svc.AV_CTRL;
                        var dsrvTVPortal = hx.svc.TV_PORTAL;
                        dsrvAppMgr.destroyAllApps();
                        dsrvTVPortal.destroyActiveApp();
                        dsrvAppMgr.destroyHbbTvDeeplinkApplication();
                        if (dsrvAvCtrl.isIdle()) {
                            dsrvAppMgr.createHbbTVApplication(true);
                            dsrvAvCtrl.unBlockAV();
                        }
                        hx.svc.FRONT_PANEL.clearFrontPanelText();
                        ///////////////////////////////
                        // exit page
                        hx.log('CPgRecControl', '[ScheduleEvent] reminder result : page change to liveController');
                        that.sendEventToEntity({
                            'alEvent': 'alChangePage',
                            'target': 'CPgLiveController',
                            'me': that
                        });
                        ///////////////////////////////
                        // stop playback
                        hx.svc.BRIDGE.requestStopPlayback();
                        ///////////////////////////////
                        // channel change
                        if (o.channel !== null && o.group !== null) {
                            var fnChChange = function () {
                                hx.log('CPgRecControl', '[ScheduleEvent] reminder result : channel change!!!!');
                                var VBcurrentChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                                if (VBcurrentChannel) {
                                    if (VBcurrentChannel.ccid === o.channel.ccid) {
                                        hx.log('CPgRecControl', '[ScheduleEvent] reminder result : return!! same channel!!!');
                                        var isMediaPlay = hx.svc.PLAYBACK.isMpbOnGoing();
                                        var isTsrPlay = hx.svc.TSR.isTsrOngoing();
                                        if (isTsrPlay === true) {
                                            hx.svc.TSR.stop();
                                            hx.log('CPgRecControl', '[ScheduleEvent] Stop TSR Play!!!');
                                        }
                                        if (isMediaPlay === true) {
                                            hx.svc.PLAYBACK.stop();
                                            hx.log('CPgRecControl', '[ScheduleEvent] Stop MEDIA Play!!!');
                                        }
                                        dsrvRecordings.recordConfirm({
                                            scheduledRecording: item,
                                            type: 0,
                                            check: res
                                        });
                                        return;
                                    }
                                }
                                hx.svc.AV_CTRL.channelChange({
                                    channel: o.channel,
                                    group: o.group
                                });
                                dsrvRecordings.recordConfirm({
                                    scheduledRecording: item,
                                    type: 0,
                                    check: res
                                });
                            };
                            setTimeout(fnChChange, 0);
                        }
                    }
                    else if ((res === 0) && item) {
                        dsrvRecordings.recordConfirm({
                            scheduledRecording: item,
                            type: 0,
                            check: res
                        });
                        dsrvRecordings.removeScheduledRecording({
                            items: [item]
                        });
                    }
                    else {
                        //do recording
                        dsrvRecordings.recordConfirm({
                            scheduledRecording: item,
                            type: 0,
                            check: res
                        });
                    }
                };
                if (text) {
                    that._reminderMsg = prism.createReminderDlg(null, {
                        title: text,
                        eventName: item.getTitle(),
                        channelNo: hx.il.makeDigitNumber(item.getChannelNo()),
                        channelName: item.getChannelName(),
                        startTime: item.getStartTime(),
                        endTime: item.getEndTime(),
                        validOnNetflix: bValidOnNetflix
                    }, function (yes) {
                        hx.log('CPgRecControl', '[ScheduleEvent] reminder yes :' + yes);
                        if (hx.config.netflix) {
                            if (item.isReminder() && hx.svc.NETFLIX.isRunning()) {
                                var param = __NETFLIX__.OVERLAY_POPUP_STATE.CLOSE;
                                if (yes === true) {
                                    param = __NETFLIX__.OVERLAY_POPUP_STATE.CLOSE_WITH_NETFLIX_EXIT;
                                }
                                hx.svc.NETFLIX.notifyOverlayState(param);
                            }
                        }
                        setResult(item, (yes) ? 1 : 0);
                    });
                    that._reminderMsg.open({
                        onClosed: function (reason) {
                            hx.log('CPgRecControl', '[ScheduleEvent] reminder onClose :' + reason);
                            if (reason === 'timedOut') {
                                if (hx.config.netflix) {
                                    if (item.isReminder() && hx.svc.NETFLIX.isRunning()) {
                                        hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE_WITH_NETFLIX_EXIT);
                                    }
                                }
                                setResult(item, 1);
                            }
                            else if (reason === null) {
                                if (hx.config.netflix) {
                                    if (item.isReminder() && hx.svc.NETFLIX.isRunning()) {
                                        hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                                    }
                                }
                                setResult(item, 0);
                            }
                            that._reminderMsg = null;
                            that._setVisible(false);
                        }
                    });
                    that._setVisible(true);
                    hx.log('CPgRecControl', '[ScheduleEvent] reminder Create dialogue!!!');
                }
            });
            dsrvRecordings.addEventCb('ScheduledItemSet', that, function (param) {
                hx.log('CPgRecControl', '[ScheduledItemSet] a popup for alarm');
                var scheduledItem = param ? param.item : null;
                if (scheduledItem && scheduledItem.isRecording() && !scheduledItem.isRecordingOnGoing()) {
                    if (that._prevScheduledItem && scheduledItem.isSeries()) {
                        if (that._prevScheduledItem.getSeriesId() === scheduledItem.getSeriesId()) {
                            return;
                        }
                    }
                    that._prevScheduledItem = scheduledItem;
                    if (that._scheduledMsg) {
                        hx.il.ani.reset(that._scheduledMsg.$, '');
                        hx.msg.close('recordStatus', that._scheduledMsg);
                        that._scheduledMsg = undefined;
                    }
                    var iconHtml = scheduledItem.getChannelInstance().isHD ? "<img src='images/203_Resolition_HD.png'/>" : "";
                    that._scheduledMsg = hx.msg('recordStatus', {
                        'text': 'Your recording is set.',
                        'text_detail': iconHtml + that.__makeDispTime({
                            'startTime': scheduledItem.getStartTime() / 1000,
                            'duration': scheduledItem.getDuration(),
                            'name': scheduledItem.getTitle()
                        }),
                        'auto_close': true,
                        'timeout': 3000,
                        'callback_fn': function (_msg) {
                            if (_msg === 'auto_close') {
                                that._scheduledMsg = undefined;
                            }
                        }
                    });
                }
            });
        };
        cCPgRecControl.prototype.destroy = function () {
            'use strict';
            hx.log('CPgRecControl', '[destroy] destroy recording control page');
        };
        cCPgRecControl.prototype.isActive = function () {
            'use strict';
            //because background service
            return false;
        };
        //visible () : boolean {
        //    return this._isVisible;
        //}
        cCPgRecControl.prototype._setVisible = function (isVisible) {
            hx.log('CPgRecControl', '[setVisible] previous setVisible ' + this._isVisible + ', set ' + isVisible);
            if (isVisible !== this._isVisible) {
                this._isVisible = isVisible;
                this.sendEventToEntity({
                    alEvent: 'alCheckPageVisible'
                });
            }
        };
        cCPgRecControl.prototype.__makeDispTime = function (aData, aFormatAlternative) {
            'use strict';
            if (aFormatAlternative === void 0) { aFormatAlternative = false; }
            var startTime, endTime;
            var strTime, tempStr;
            if (aData.startTime === undefined) {
                startTime = new Date();
            }
            else {
                startTime = new Date((aData.startTime) * 1000);
            }
            endTime = new Date(startTime.valueOf() + (aData.duration + 30) * 1000);
            strTime = startTime.format("HH:MM") + '-' + endTime.format("HH:MM") + ', ' + startTime.format("mm.dd");
            if (aData.name === undefined || aData.name.length === 0) {
                tempStr = strTime;
            }
            else {
                if (aFormatAlternative) {
                    tempStr = aData.name + ' (' + strTime + ')';
                }
                else {
                    tempStr = aData.name + ', ' + strTime;
                }
            }
            return tempStr;
        };
        return cCPgRecControl;
    })(__PAGE_KERNEL__);
    return cCPgRecControl;
});
