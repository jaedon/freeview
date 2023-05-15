/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.page.recordings.js
/*jslint sloppy:true, nomen:true, vars:true */
/*global tr */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'model/model', 'pages/util/controls', './prism.page'], function (require, exports, __MODEL__, __CONTROLS__, __PRISM_CPAGE__) {
    var mPrism;
    (function (mPrism) {
        var CRecordUtil = (function () {
            function CRecordUtil() {
                this.TOptionMenuItem = {
                    EDelete: 0,
                    ECopy: 1,
                    EMove: 2,
                    EStopCopying: 3,
                    EStopMoving: 4,
                    ESelect: 5,
                    ESelectAll: 6,
                    ECancel: 7,
                    EStopRec: 8,
                    ELock: 9,
                    EUnLock: 10,
                    EReSchedule: 11,
                    EScheduleDelete: 12,
                    EScheduleDeletes: 13,
                    ESelectSchedule: 14,
                    ESelectAllSchedule: 15,
                    EKeep: 16,
                    ESelectTobedeleted: 17,
                    ESelectAllTobedeleted: 18,
                    ESortBy: 19,
                    EKeeps: 20
                };
                this.TMenuItem = {
                    EPROGRAMME: 0,
                    ERECENT: 1,
                    ECHANNEL: 2,
                    EDAYOFWEEK: 3,
                    ESTORAGE: 4,
                    ESCHEDULE: 5,
                    ETOBEDELETED: 6
                };
                this.WIDTH_GRID_CHANNEL_LOGO = 50;
                this.WIDTH_LIST_CHANNEL_LOGO = 85;
            }
            CRecordUtil.prototype.fnMatchSeries = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return a.isSeries() && b.isSeries() && (a.getSeriesId() === b.getSeriesId());
            };
            CRecordUtil.prototype.fnMatchChannel = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (a.getChannelId() === b.getChannelId());
            };
            CRecordUtil.prototype.fnMatchDayOfWeek = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (a.getStartTime().getDay() === b.getStartTime().getDay());
            };
            CRecordUtil.prototype.fnMatchStorage = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (a.getStorageId() === b.getStorageId());
            };
            CRecordUtil.prototype.fnCompareStartTimeDescent = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (b.getStartTime()) - (a.getStartTime());
            };
            CRecordUtil.prototype.fnCompareStartTimeAscent = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (a.getStartTime()) - (b.getStartTime());
            };
            CRecordUtil.prototype.fnCompareStartTimeGreatThen = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (a.getStartTime()) > (b.getStartTime());
            };
            CRecordUtil.prototype.fnCompareStartTimeLessThen = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                return (a.getStartTime()) < (b.getStartTime());
            };
            CRecordUtil.prototype.fnCompareAlphabet = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                if (a.getTitle().toLowerCase() > b.getTitle().toLowerCase()) {
                    return 1;
                }
                else if (a.getTitle().toLowerCase() < b.getTitle().toLowerCase()) {
                    return -1;
                }
                else {
                    return 0;
                }
            };
            CRecordUtil.prototype.fnCompareAlphabetReverse = function (a, b) {
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                if (a.getTitle().toLowerCase() > b.getTitle().toLowerCase()) {
                    return -1;
                }
                else if (a.getTitle().toLowerCase() < b.getTitle().toLowerCase()) {
                    return 1;
                }
                else {
                    return 0;
                }
            };
            CRecordUtil.prototype.fnSortByStartTimeDescent = function (recordingList) {
                recordingList.sort(CRecUtil.fnCompareStartTimeDescent);
            };
            CRecordUtil.prototype.fnSortByStartTimeAscent = function (recordingList) {
                recordingList.sort(CRecUtil.fnCompareStartTimeAscent);
            };
            CRecordUtil.prototype.fnSortByTitle = function (recordingList) {
                recordingList.sort(CRecUtil.fnCompareAlphabet);
            };
            CRecordUtil.prototype.fnSortByTitleReverse = function (recordingList) {
                recordingList.sort(CRecUtil.fnCompareAlphabetReverse);
            };
            CRecordUtil.prototype.fnSortByChannelNumber = function (recordingList) {
                recordingList.sort(function (a, b) {
                    a = a instanceof Array ? a[0] : a;
                    b = b instanceof Array ? b[0] : b;
                    return a.getChannelNo() - b.getChannelNo();
                });
            };
            CRecordUtil.prototype.fnMakeScheduledList = function (scheduledItemList, sortAscent) {
                var ret = [], i, j, len, scheduledItem;
                var sortedList = [];
                var addedCRIDs = [];
                var isFirstAddedSeries;
                for (i = 0, len = scheduledItemList.length; i < len; i += 1) {
                    sortedList.push(scheduledItemList[i]);
                }
                if (sortAscent) {
                    CRecUtil.fnSortByStartTimeAscent(sortedList);
                }
                else {
                    CRecUtil.fnSortByStartTimeDescent(sortedList);
                }
                prism.log('************************************************');
                prism.log('Schedule List Length :' + sortedList.length);
                for (i = 0, len = sortedList.length; i < len; i += 1) {
                    scheduledItem = sortedList[i];
                    prism.log('[' + i + '] -[' + scheduledItem.getFactoryTypeStr() + ']\t Title: ' + scheduledItem.getTitle());
                    prism.log('[' + i + '] -\t IsRecording :' + scheduledItem.isRecordingOnGoing());
                    prism.log('[' + i + '] -\t ChannelName :' + scheduledItem.getChannelName() + ', ChannelNo: ' + scheduledItem.getChannelNo());
                    prism.log('[' + i + '] - \t IsSeries :' + scheduledItem.isSeries() + ', SeriesID : ' + scheduledItem.getSeriesId());
                    if (scheduledItem.getGroupCRIDs()) {
                        for (j = 0; j < scheduledItem.getGroupCRIDs().length; j += 1) {
                            prism.log('[' + i + '] - \t CRIDs :' + scheduledItem.getGroupCRIDs()[j]);
                        }
                    }
                    prism.log('---------------------------------------------');
                    isFirstAddedSeries = true;
                    if (!scheduledItem.isRecordingOnGoing() && scheduledItem.getGroupCRIDs() && scheduledItem.getGroupCRIDs().length > 0) {
                        for (j = 0; j < addedCRIDs.length; j += 1) {
                            if (addedCRIDs[j] === scheduledItem.getGroupCRIDs()[0]) {
                                isFirstAddedSeries = false;
                                break;
                            }
                        }
                        addedCRIDs.push(scheduledItem.getGroupCRIDs()[0]);
                    }
                    if (!scheduledItem.isRecordingOnGoing() && ((scheduledItem.isRecording() && !scheduledItem.isPendingSeriesSchedule()) || scheduledItem.isReminder()) && isFirstAddedSeries) {
                        ret.push([scheduledItem]);
                    }
                    else if (scheduledItem.isRecordingOnGoing() && (scheduledItem.getRepeat() & 0xFF) !== 0) {
                        ret.push([scheduledItem]);
                    }
                }
                for (i = 0, len = sortedList.length; i < len; i++) {
                    scheduledItem = sortedList[i];
                    if (scheduledItem.isPendingSeriesSchedule()) {
                        ret.push([scheduledItem]);
                    }
                }
                prism.log('************************************************');
                return ret;
            };
            CRecordUtil.prototype.fnIsSameSeries = function (aGroupCRIDs1, aGroupCRIDs2) {
                var i, j;
                var ret = false;
                if (aGroupCRIDs1 && aGroupCRIDs2) {
                    if (aGroupCRIDs1.length > 0 && aGroupCRIDs2.length > 0) {
                        for (i = 0; i < aGroupCRIDs1.length; i++) {
                            for (j = 0; j < aGroupCRIDs2.length; j++) {
                                if (aGroupCRIDs1[i] === aGroupCRIDs2[j]) {
                                    return true;
                                    break;
                                }
                            }
                        }
                    }
                }
                return ret;
            };
            CRecordUtil.prototype.fnMakeListByRecent = function (recordingItemList) {
                var i = 0, j = 0, recordingTotal = 0, recentTotal = 0, ret = [];
                var recordingItem = null, recentItem = null;
                var recentList = hx.svc.HISTORY.getPlaybackList();
                if (!!recentList) {
                    recentTotal = recentList.length;
                }
                if (recentTotal <= 0) {
                    return ret;
                }
                if (!!recordingItemList) {
                    recordingTotal = recordingItemList.length;
                }
                if (recordingTotal <= 0) {
                    return ret;
                }
                for (i = 0; i < recentTotal; i++) {
                    recentItem = recentList[i];
                    if (!!recentItem) {
                        for (j = 0; j < recordingTotal; j++) {
                            recordingItem = recordingItemList[j];
                            if (!!recordingItem) {
                                if (recentItem.getId && recordingItem.getId) {
                                    if (recentItem.getId() == recordingItem.getId()) {
                                        ret.push([recordingItem]);
                                    }
                                }
                            }
                        }
                    }
                }
                return ret;
            };
            CRecordUtil.prototype.fnMakeListByProgramme = function (a, recordingItemList, sortAscent) {
                var ret = [], i, j, k, len, recordingItem, list = [];
                var bySeriesCrid = {};
                var seriesCrid;
                for (i = 0, len = recordingItemList.length; i < len; i += 1) {
                    recordingItem = recordingItemList[i];
                    if (recordingItem.isSeries()) {
                        seriesCrid = recordingItem.getGroupCRIDs()[0];
                        if (!bySeriesCrid[seriesCrid]) {
                            bySeriesCrid[seriesCrid] = [];
                        }
                        list = bySeriesCrid[seriesCrid];
                        if (list.length && list[0].getStartTime() < recordingItem.getStartTime()) {
                            list.unshift(recordingItem);
                        }
                        else {
                            list.push(recordingItem);
                        }
                    }
                    else {
                        ret.push([recordingItem]);
                    }
                }
                for (seriesCrid in bySeriesCrid) {
                    if (bySeriesCrid.hasOwnProperty(seriesCrid)) {
                        ret.push(bySeriesCrid[seriesCrid]);
                    }
                }
                if (a._sortType === a.ESortType.EALPHABET) {
                    CRecUtil.fnSortByTitle(ret);
                }
                else {
                    CRecUtil.fnSortByStartTimeDescent(ret);
                }
                return ret;
            };
            CRecordUtil.prototype.fnMakeListByChannel = function (recordingItemList) {
                var ret = [], i, len, recordingItem, channelId, byChannelId = {}, list;
                for (i = 0, len = recordingItemList.length; i < len; i += 1) {
                    recordingItem = recordingItemList[i];
                    channelId = recordingItem.getChannelId();
                    if (!byChannelId[channelId]) {
                        byChannelId[channelId] = [];
                    }
                    list = byChannelId[channelId];
                    if (list.length && list[0].getStartTime() < recordingItem.getStartTime()) {
                        list.unshift(recordingItem);
                    }
                    else {
                        list.push(recordingItem);
                    }
                }
                for (channelId in byChannelId) {
                    if (byChannelId.hasOwnProperty(channelId)) {
                        ret.push(byChannelId[channelId]);
                    }
                }
                CRecUtil.fnSortByChannelNumber(ret);
                return ret;
            };
            CRecordUtil.prototype.fnMakeListByDayOfWeek = function (recordingItemList) {
                var ret = [], i, len, recordingItem, startTime, dayOfWeek, byDayOfWeek = {}, dayOfWeekList, hasSunday;
                for (i = 0, len = recordingItemList.length; i < len; i += 1) {
                    recordingItem = recordingItemList[i];
                    startTime = recordingItem.getStartTime();
                    dayOfWeek = startTime.getDay();
                    if (dayOfWeek === 0) {
                        hasSunday = true;
                    }
                    if (!byDayOfWeek[dayOfWeek]) {
                        byDayOfWeek[dayOfWeek] = [];
                    }
                    dayOfWeekList = byDayOfWeek[dayOfWeek];
                    if (dayOfWeekList.length && dayOfWeekList[0].getStartTime() < startTime) {
                        dayOfWeekList.unshift(recordingItem);
                    }
                    else {
                        dayOfWeekList.push(recordingItem);
                    }
                }
                for (dayOfWeek in byDayOfWeek) {
                    if (byDayOfWeek.hasOwnProperty(dayOfWeek)) {
                        ret.push(byDayOfWeek[dayOfWeek]);
                    }
                }
                if (hasSunday) {
                    ret.push(ret.shift());
                }
                return ret;
            };
            CRecordUtil.prototype.fnMakeListByStorage = function (recordingItemList) {
                var ret = [], i, len, recordingItem, startTime, byStorageId = {}, storageId, storageList;
                for (i = 0, len = recordingItemList.length; i < len; i += 1) {
                    recordingItem = recordingItemList[i];
                    startTime = recordingItem.getStartTime();
                    storageId = recordingItem.getStorageId();
                    if (!byStorageId[storageId]) {
                        byStorageId[storageId] = [];
                    }
                    storageList = byStorageId[storageId];
                    if (storageList.length && storageList[0].getStartTime() < startTime) {
                        storageList.unshift(recordingItem);
                    }
                    else {
                        storageList.push(recordingItem);
                    }
                }
                for (storageId in byStorageId) {
                    if (byStorageId.hasOwnProperty(storageId)) {
                        ret.push(byStorageId[storageId]);
                    }
                }
                return ret;
            };
            CRecordUtil.prototype.fnMakeListByTobedeleted = function (recordingItemList, sortAscent) {
                var ret = [], i, j, len, recordingItem, seriesUid, seriesId, channelId, title, crid, bySeriesUid = {}, list;
                for (i = 0, len = recordingItemList.length; i < len; i += 1) {
                    recordingItem = recordingItemList[i];
                    if (recordingItem.isWillbedeleted() && !recordingItem.isLocked()) {
                        ret.push([recordingItem]);
                    }
                }
                return ret;
            };
            CRecordUtil.prototype.fnListHasOngoingItem = function (list) {
                var i, ti, item, bOnGoing = false;
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    item = list[i];
                    if (item.isRecordingOnGoing()) {
                        bOnGoing = true;
                        break;
                    }
                }
                return bOnGoing;
            };
            CRecordUtil.prototype.fnListHasLockedItem = function (list) {
                var i, ti, item, bLocked = false;
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    item = list[i];
                    if (item.isLocked()) {
                        bLocked = true;
                        break;
                    }
                }
                return bLocked;
            };
            CRecordUtil.prototype.fnListHasLockedWholeItem = function (list) {
                var i, ti, item, bAllLocked = true;
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    item = list[i];
                    if (!item.isLocked()) {
                        bAllLocked = false;
                        break;
                    }
                }
                return bAllLocked;
            };
            CRecordUtil.prototype.fnGroupingItemByCopyControl = function (list) {
                var i, ti, item, copyOkItems = [], noCopyItems = [];
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    item = list[i];
                    if (item.isNoCopy()) {
                        noCopyItems.push(item);
                    }
                    else {
                        copyOkItems.push(item);
                    }
                }
                return {
                    noCopyItems: noCopyItems,
                    copyOkItems: copyOkItems
                };
            };
            CRecordUtil.prototype.prependTagWithClass = function (prepend, $parent, className) {
                var $target = $parent.children('.' + className);
                if (prepend) {
                    if ($target.length === 0) {
                        //return $parent.prepend($('<div>', {
                        //    'class': className
                        //}));
                        return $('<div>', {
                            'class': className
                        }).prependTo($parent);
                    }
                    else {
                        return $target;
                    }
                }
                else {
                    $target.remove();
                    return null;
                }
            };
            CRecordUtil.prototype.appendTagWithClass = function (append, $parent, className, tag) {
                var $target = $parent.children('.' + className);
                if (append) {
                    if ($target.length === 0) {
                        //return $parent.append($('<div>', {
                        //    'class': className
                        //}));
                        return $(tag || '<div>', {
                            'class': className
                        }).appendTo($parent);
                    }
                    else {
                        return $target;
                    }
                }
                else {
                    $target.remove();
                    return null;
                }
            };
            CRecordUtil.prototype.fnListHasNewItem = function (list) {
                var i, ti, isNew = false;
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    if (list[i].isNew()) {
                        isNew = true;
                        break;
                    }
                }
                return isNew;
            };
            CRecordUtil.prototype.fnStringDayOfWeek = function (dayOfWeek) {
                return tr([
                    'LOC_SUNDAY_ID',
                    'LOC_MONDAY_ID',
                    'LOC_TUESDAY_ID',
                    'LOC_WEDNESDAY_ID',
                    'LOC_THURSDAY_ID',
                    'LOC_FRIDAY_ID',
                    'LOC_SATURDAY_ID'
                ][dayOfWeek]);
            };
            CRecordUtil.prototype.fnUpdateStorageInfo = function (a) {
                if (a._storageInfo && a._$storageStatusArea) {
                    var hdFreeSize = Math.floor(a._storageInfo.free / 4), sdFreeSize = Math.floor(a._storageInfo.free / 1.5);
                    var recordingItemList = hx.svc.RECORDINGS.getRecordingList();
                    if (hx.config.recStorageStatusType2 === true) {
                        a._$storageStatusArea.find('.info').html('<span style="font-weight:bold">HD</span>' + '<span> ' + hdFreeSize + ((hdFreeSize <= 1) ? 'hr' : 'hrs') + ' / </span>' + '<span style="font-weight:bold">SD</span>' + '<span> ' + sdFreeSize + ((sdFreeSize <= 1) ? 'hr' : 'hrs') + '</span><br>' + '<span style="font-weight:bold">Total:</span><span> ' + recordingItemList.length + ' Recordings</span>');
                        prism.drawStorageGraphCircleType2(a._storageInfo.total - a._storageInfo.free, a._storageInfo.total, 42, 42, 17, 28, a._$storageStatusArea.find('canvas')[0]);
                    }
                    else {
                        a._$storageStatusArea.find('.info').html('HD' + ' ' + hdFreeSize + ' ' + ((hdFreeSize <= 1) ? 'hr' : 'hrs') + ' / ' + 'SD' + ' ' + sdFreeSize + ' ' + ((sdFreeSize <= 1) ? 'hr' : 'hrs') + '<br>' + 'Total : ' + recordingItemList.length + ' Recordings');
                        prism.drawStorageGraphRect(a._storageInfo.total - a._storageInfo.free, a._storageInfo.total, 0, 0, a._$storageStatusArea.find('canvas')[0]);
                    }
                }
            };
            CRecordUtil.prototype.fnGetSelectedItems = function (a) {
                if (a._activeViewType === 'tobedeleted') {
                    var data = a._dpTobedeleted;
                }
                else {
                    var data = (a._dpRecordingExtView || a._dpRecordings);
                }
                var i, ti, items, selectedList = [];
                for (i = 0, ti = data.getCount(); i < ti; i += 1) {
                    items = data.getItem(i);
                    if (items._isSelected || items._isChecked) {
                        selectedList = selectedList.concat(items);
                    }
                }
                return selectedList;
            };
            CRecordUtil.prototype.fnGetSelectedKeepItems = function (a) {
                var data = a._dpTobedeleted;
                var i, ti, items, selectedList = [];
                if (data) {
                    for (i = 0, ti = data.getCount(); i < ti; i += 1) {
                        items = data.getItem(i);
                        if (items._isChecked) {
                            selectedList = selectedList.concat(items);
                        }
                    }
                }
                return selectedList;
            };
            CRecordUtil.prototype.fnPropagateItemInsertedEvent = function (dpSource, item, dpTarget, fnGroupMatch, fnCompare) {
                prism.log('fnPropagateItemInsertedEvent');
                var dataArray = dpTarget.getDataArray(), i, len = dataArray.length, items, added = false;
                for (i = 0; i < len; i += 1) {
                    items = dataArray[i];
                    if (fnGroupMatch(item, items)) {
                        if (fnCompare(items, item)) {
                            items.unshift(item);
                        }
                        else {
                            items.push(item);
                        }
                        prism.log('fnPropagateItemInsertedEvent updateItem ' + i);
                        dpTarget.updateItem(i);
                        added = true;
                        break;
                    }
                }
                if (!added) {
                    for (i = 0; i < len; i += 1) {
                        items = dataArray[i];
                        if (fnCompare(items, item)) {
                            prism.log('fnPropagateItemInsertedEvent insertItem ' + i);
                            dpTarget.insertItem(i, [item]);
                            added = true;
                            break;
                        }
                    }
                    if (!added) {
                        dpTarget.appendItem([item]);
                    }
                }
            };
            CRecordUtil.prototype.fnPropagateItemRemovedEvent = function (dpSource, items, dpTarget, ascent) {
                prism.log('fnPropagateItemRemovedEvent');
                var i, iLen, item, funcName, index = -1;
                for (i = 0, iLen = items.length; i < iLen; i += 1) {
                    item = items[i];
                    funcName = null;
                    index = dpTarget.getItemIndex(item, function (targetItems) {
                        var i, len;
                        for (i = 0, len = targetItems.length; i < len; i += 1) {
                            if (targetItems[i].isEqual(item)) {
                                if (i === 0) {
                                    targetItems.shift();
                                    if (len === 1) {
                                        funcName = 'removeItem';
                                    }
                                    else {
                                        if (ascent) {
                                            CRecUtil.fnSortByStartTimeAscent(targetItems);
                                        }
                                        else {
                                            CRecUtil.fnSortByStartTimeDescent(targetItems);
                                        }
                                        funcName = 'updateItem';
                                    }
                                }
                                else {
                                    funcName = 'updateItem';
                                    targetItems.splice(i, 1);
                                }
                                return true;
                            }
                        }
                        return false;
                    });
                    if (index !== -1 && funcName) {
                        prism.log('fnPropagateItemRemovedEvent ' + funcName + ' ' + index);
                        dpTarget[funcName](index);
                    }
                }
            };
            CRecordUtil.prototype.fnPropagateRecItemRemovedEvent = function (dpSource, items, dpTarget, ascent) {
                prism.log('fnPropagateRecItemRemovedEvent');
                var i, iLen, item, funcName, index = -1;
                for (i = 0, iLen = items.length; i < iLen; i += 1) {
                    item = items[i];
                    funcName = null;
                    index = dpTarget.getItemIndex(item, function (targetItems) {
                        var i, len;
                        for (i = 0, len = targetItems.length; i < len; i += 1) {
                            if (targetItems[i].isEqual(item)) {
                                if (i === 0) {
                                    targetItems.shift();
                                    if (len === 1) {
                                        funcName = 'removeRecordingItem';
                                    }
                                    else {
                                        if (ascent) {
                                            CRecUtil.fnSortByStartTimeAscent(targetItems);
                                        }
                                        else {
                                            CRecUtil.fnSortByStartTimeDescent(targetItems);
                                        }
                                        funcName = 'updateItem';
                                    }
                                }
                                else {
                                    funcName = 'updateItem';
                                    targetItems.splice(i, 1);
                                }
                                return true;
                            }
                        }
                        return false;
                    });
                    if (index !== -1 && funcName) {
                        prism.log('fnPropagateRecItemRemovedEvent ' + funcName + ' ' + index);
                        dpTarget[funcName](index);
                    }
                }
            };
            CRecordUtil.prototype.fnPropagateItemUpdateEvent = function (dpSource, keys, dpTarget) {
                prism.log('fnPropagateItemUpdateEvent');
                var i, iLen, item, func, index;
                var funcName;
                for (i = 0, iLen = keys.length; i < iLen; i += 1) {
                    item = dpSource.getItem(keys[i]);
                    funcName = null;
                    index = dpTarget.getItemIndex(item, function (targetItems) {
                        var i, len;
                        for (i = 0, len = targetItems.length; i < len; i += 1) {
                            if (targetItems[i].isEqual(item)) {
                                targetItems[i] = item;
                                funcName = 'updateItem';
                                return true;
                            }
                        }
                        return false;
                    });
                    if (index !== -1 && funcName) {
                        prism.log('fnPropagateItemUpdateEvent ' + funcName + ' ' + index);
                        dpTarget[funcName](index);
                    }
                }
            };
            CRecordUtil.prototype.fnPropagateTobedeletedItemUpdateEvent = function (dpSource, keys, dpTarget) {
                prism.log('fnPropagateTobedeletedItemUpdateEvent');
                var i, iLen, item, func, index;
                var funcName;
                for (i = 0, iLen = keys.length; i < iLen; i += 1) {
                    item = dpSource.getItem(keys[i]);
                    funcName = null;
                    index = dpTarget.getItemIndex(item, function (targetItems) {
                        var i, len;
                        for (i = 0, len = targetItems.length; i < len; i += 1) {
                            if (targetItems[i].isEqual(item)) {
                                if (item.isWillbedeleted() === false) {
                                    if (i === 0) {
                                        targetItems.shift();
                                        if (len === 1) {
                                            funcName = 'removeItem';
                                        }
                                        else {
                                            funcName = 'updateItem';
                                        }
                                    }
                                }
                                else {
                                    targetItems[i] = item;
                                    funcName = 'updateItem';
                                }
                                return true;
                            }
                        }
                        return false;
                    });
                    if (index !== -1 && funcName) {
                        prism.log('fnPropagateItemUpdateEvent ' + funcName + ' ' + index);
                        dpTarget[funcName](index);
                    }
                }
            };
            CRecordUtil.prototype.fnMenuLayout_onSchedule = function (a, index, $item) {
                a.onRequestScheduledData(function (scheduledDataProvider) {
                    var array = scheduledDataProvider.getDataArray();
                    var listByProgramme = CRecUtil.fnMakeScheduledList(array, true);
                    var dataProvider = new CDataProvider(listByProgramme);
                    a._ScheduleProvider = dataProvider;
                    a._dpRecordingExtView = null;
                    var viewDlg = prism.createViewDlg(a);
                    viewDlg.setTitleHtml(tr('LOC_REC_TITLESCHEDULE_ID'));
                    viewDlg.setListInfoDrawer(function ($listInfoArea) {
                        var focusedIndex;
                        var totalCount = ('00' + dataProvider.getCount()).slice(-3);
                        var $new;
                        var view = viewDlg.getView();
                        if (view && view.hasFocus()) {
                            var focusedControl = view.getFocusedChild();
                            if (focusedControl instanceof CListControl) {
                                var focusedItem = focusedControl.getFocusedItem();
                                if (focusedItem) {
                                    $new = focusedItem.$item;
                                }
                            }
                        }
                        if ($new) {
                            focusedIndex = ('00' + (parseInt($new.attr('data'), 10) + 1)).slice(-3);
                        }
                        else {
                            focusedIndex = (dataProvider.getCount() > 0) ? '001' : '000';
                        }
                        $listInfoArea.text(hx.util.format('{0} / {1}', focusedIndex, totalCount));
                    });
                    viewDlg.setButtonInfo([
                        {
                            text: tr('LOC_REC_TITLEADDASCH_ID'),
                            fn: function () {
                                prism.log('scheduled count ' + scheduledDataProvider.getCount());
                                CRecFOP.fnOptionMentHandleSelectAllCancel(a);
                                if (scheduledDataProvider.getCount() >= 300) {
                                    var notificationDlg = prism.createNotificationDlg(a);
                                    notificationDlg.setTextHtml(tr('LOC_RESERVE_ONLY_300_ID'));
                                    notificationDlg.setTimeout(3000);
                                    notificationDlg.open();
                                }
                                else {
                                    CRecDraw.fnOpenScheduledDlg(a);
                                }
                            }
                        }
                    ]);
                    scheduledDataProvider.addEventHandler('ItemInserted', a, function (key, item) {
                        if (!item.isRecordingOnGoing() && (item.isRecording() || item.isReminder())) {
                            CRecUtil.fnPropagateItemInsertedEvent(scheduledDataProvider, item, dataProvider, CRecUtil.fnMatchSeries, CRecUtil.fnCompareStartTimeGreatThen);
                            viewDlg.getView().draw();
                            if (viewDlg.doKeyDown) {
                                viewDlg.doKeyDown = undefined;
                            }
                            viewDlg.updateListInfo();
                        }
                    });
                    scheduledDataProvider.addEventHandler('ItemRemoved', a, function (keys, items) {
                        CRecUtil.fnPropagateItemRemovedEvent(scheduledDataProvider, items, dataProvider, true);
                        viewDlg.getView().draw();
                        if (dataProvider.getCount() === 0) {
                            viewDlg.doNavi('u');
                            viewDlg.doKeyDown = function () {
                                return true;
                            };
                            a.reloadContextMenuItems();
                        }
                        viewDlg.updateListInfo();
                    });
                    scheduledDataProvider.addEventHandler('RecItemRemoved', a, function (keys, items) {
                        CRecUtil.fnPropagateRecItemRemovedEvent(scheduledDataProvider, items, dataProvider, true);
                        viewDlg.getView().draw();
                        if (dataProvider.getCount() === 0) {
                            viewDlg.doNavi('u');
                            viewDlg.doKeyDown = function () {
                                return true;
                            };
                            a.reloadContextMenuItems();
                        }
                        viewDlg.updateListInfo();
                    });
                    scheduledDataProvider.addEventHandler('ItemChanged', a, function (keys) {
                        CRecUtil.fnPropagateItemUpdateEvent(scheduledDataProvider, keys, dataProvider);
                    });
                    viewDlg.setDataProvider(dataProvider);
                    viewDlg.setDataDrawer(CRecDraw.fnScheduledItemDrawer);
                    viewDlg.setOptionMenuTarget(a);
                    viewDlg.onDataItemSelected = function (param) {
                        viewDlg.setActiveFocus(false);
                        var item = param.item[0];
                        if (a._isMultiSelect === true) {
                            viewDlg.setActiveFocus(true);
                            var data = a._ScheduleProvider;
                            var dataArray = data.getDataArray();
                            var i, ti;
                            for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                if (param.item === dataArray[i]) {
                                    param.item._isChecked = !param.item._isChecked;
                                    data.updateItem(i, param.item);
                                    a.reloadContextMenuItems();
                                    break;
                                }
                            }
                            return;
                        }
                        prism.processRequest(a, null, 'onScheduledItemSelected', {
                            'item': param.item[0],
                            onClosed: function () {
                                setTimeout(function () {
                                    viewDlg.setActiveFocus(true);
                                }, 500);
                            }
                        }, function (response) {
                            prism.log('finish onDataItemSelected');
                        });
                    };
                    if (dataProvider.getCount() === 0) {
                        viewDlg.doKeyDown = function () {
                            return true;
                        };
                    }
                    viewDlg.setDataControlDrawParam('list', {
                        itemHeight: 56,
                        scrollByItemSize: true
                    });
                    viewDlg.open({
                        transition: {
                            prevLayer: 'fadeOut',
                            newLayer: 'fadeIn'
                        },
                        onClosed: function () {
                            CRecFOP.fnOptionMentHandleSelectAllCancel(a);
                            scheduledDataProvider.removeEventHandler(a);
                            dataProvider.destroy();
                            a._ScheduleProvider = null;
                        },
                        focusableItemLength: 9
                    });
                });
            };
            CRecordUtil.prototype.fnMenuLayout_onTobedeleted = function (a, index, $item) {
                a.onRequestRecordingData(function (recordingDataProvider) {
                    var array = _.filter(recordingDataProvider.getDataArray(), function (aItem) {
                        return aItem.getWillBeDeleted();
                    });
                    var listByProgramme = CRecUtil.fnMakeListByTobedeleted(array, true);
                    var dataProvider = new CDataProvider(listByProgramme);
                    //                a._ScheduleProvider = null;//dataProvider;
                    a._dpTobedeleted = dataProvider;
                    var viewDlg = prism.createViewDlg(a);
                    viewDlg.setTitleHtml(tr("LOC_TO_BE_DELETED_ID"));
                    viewDlg.setListInfoDrawer(function ($listInfoArea) {
                        var focusedIndex;
                        var totalCount = ('00' + dataProvider.getCount()).slice(-3);
                        var $new;
                        var view = viewDlg.getView();
                        if (view && view.hasFocus()) {
                            var focusedControl = view.getFocusedChild();
                            if (focusedControl instanceof CListControl) {
                                var focusedItem = focusedControl.getFocusedItem();
                                if (focusedItem) {
                                    $new = focusedItem.$item;
                                }
                            }
                        }
                        if ($new) {
                            focusedIndex = ('00' + (parseInt($new.attr('data'), 10) + 1)).slice(-3);
                        }
                        else {
                            focusedIndex = (dataProvider.getCount() > 0) ? '001' : '000';
                        }
                        $listInfoArea.text(hx.util.format('{0} / {1}', focusedIndex, totalCount));
                    });
                    recordingDataProvider.addEventHandler('ItemInserted', a, function (key, item) {
                        if (!item.isRecordingOnGoing() && (item.isRecording() || item.isReminder() || item.isSatRecord())) {
                            CRecUtil.fnPropagateItemInsertedEvent(recordingDataProvider, item, dataProvider, CRecUtil.fnMatchSeries, CRecUtil.fnCompareStartTimeGreatThen);
                            viewDlg.getView().draw();
                            if (viewDlg.doKeyDown) {
                                viewDlg.doKeyDown = undefined;
                            }
                            viewDlg.updateListInfo();
                        }
                    });
                    recordingDataProvider.addEventHandler('ItemRemoved', a, function (keys, items) {
                        CRecUtil.fnPropagateItemRemovedEvent(recordingDataProvider, items, dataProvider, true);
                        if (dataProvider.getCount() === 0) {
                            viewDlg.doNavi('u');
                            viewDlg.doKeyDown = function () {
                                return true;
                            };
                            a.reloadContextMenuItems();
                        }
                        viewDlg.updateListInfo();
                    });
                    recordingDataProvider.addEventHandler('RecItemRemoved', a, function (keys, items) {
                        CRecUtil.fnPropagateRecItemRemovedEvent(recordingDataProvider, items, dataProvider, true);
                        if (dataProvider.getCount() === 0) {
                            viewDlg.doNavi('u');
                            viewDlg.doKeyDown = function () {
                                return true;
                            };
                            a.reloadContextMenuItems();
                        }
                        viewDlg.updateListInfo();
                    });
                    recordingDataProvider.addEventHandler('ItemChanged', a, function (keys) {
                        CRecUtil.fnPropagateTobedeletedItemUpdateEvent(recordingDataProvider, keys, dataProvider);
                    });
                    viewDlg.setDataProvider(dataProvider);
                    viewDlg.setDataDrawer(CRecDraw.fnTobedeletedItemDrawer);
                    viewDlg.setOptionMenuTarget(a);
                    viewDlg.onDataItemSelected = function (param) {
                        viewDlg.setActiveFocus(false);
                        var item = param.item[0];
                        if (a._isMultiSelect === true) {
                            viewDlg.setActiveFocus(true);
                            var data = a._dpTobedeleted;
                            var dataArray = data.getDataArray();
                            var i, ti;
                            for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                if (param.item === dataArray[i]) {
                                    param.item._isChecked = !param.item._isChecked;
                                    data.updateItem(i, param.item);
                                    a.reloadContextMenuItems();
                                    break;
                                }
                            }
                            return;
                        }
                        prism.processRequest(a, null, 'onRecordingItemSelected', {
                            'item': param.item[0],
                            onClosed: function () {
                                setTimeout(function () {
                                    viewDlg.setActiveFocus(true);
                                }, 500);
                            }
                        }, function (response) {
                            prism.log('finish onDataItemSelected');
                        });
                    };
                    if (dataProvider.getCount() === 0) {
                        viewDlg.doKeyDown = function () {
                            return true;
                        };
                    }
                    viewDlg.setDataControlDrawParam('list', {
                        itemHeight: 56,
                        scrollByItemSize: true
                    });
                    viewDlg.open({
                        transition: {
                            prevLayer: 'fadeOut',
                            newLayer: 'fadeIn'
                        },
                        onClosed: function () {
                            CRecFOP.fnOptionMentHandleSelectAllCancel(a);
                            //                        recordingDataProvider.removeEventHandler(a);
                            dataProvider.destroy();
                            a._dpTobedeleted = null; //a._ScheduleProvider = null;
                            a._activeViewType = a._tempViewType;
                        },
                        focusableItemLength: 9
                    });
                });
            };
            CRecordUtil.prototype.fnRecordingGrid_onItemSelected = function (a, recordingGrid, param) {
                var items = param.item;
                var title;
                if (items === undefined) {
                    return;
                }
                if (items._isChecked !== undefined) {
                    items._isChecked = !items._isChecked;
                    var data = (a._dpRecordingExtView || a._dpRecordings);
                    var dataArray = data.getDataArray();
                    var i, ti;
                    for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                        if (items === dataArray[i]) {
                            data.updateItem(i, items);
                            return;
                        }
                    }
                    return;
                }
                var viewTypeHandler = {
                    'channel': function () {
                        title = (items instanceof Array) ? items[0].getChannelName() : items.getChannelName();
                    },
                    'dayOfWeek': function () {
                        title = CRecUtil.fnStringDayOfWeek((items instanceof Array) ? items[0].getStartTime().getDay() : items.getStartTime().getDay());
                    },
                    'storage': function () {
                        title = (items instanceof Array) ? items[0].getStorageName() : items.getStorageName();
                    }
                }[a._activeViewType];
                if (viewTypeHandler) {
                    if (a._isMultiSelect || a._isExtMultiSelect) {
                        a.CloseMultiCheck();
                    }
                    recordingGrid.setActiveFocus(false);
                    viewTypeHandler();
                    var listByProgramme = CRecUtil.fnMakeListByProgramme(a, items);
                    var viewDlg = prism.createViewDlg(a);
                    viewDlg.setTitleHtml(title);
                    var dataProvider = new CDataProvider(listByProgramme);
                    a._dpRecordingExtView = dataProvider;
                    dataProvider.addEventHandler('RecItemRemoved', a, function (keys, items) {
                        if (dataProvider.getCount() === 0) {
                            viewDlg.close();
                        }
                    });
                    viewDlg.setDataProvider(dataProvider);
                    viewDlg.setDataDrawer(CRecDraw.fnRecordingItemDrawerByExtention);
                    viewDlg.setDataControlDrawParam('list');
                    viewDlg.setOptionMenuTarget(a);
                    viewDlg.onDataItemSelected = function (param) {
                        if (a._isExtMultiSelect) {
                            param.item._isChecked = !param.item._isChecked;
                            var data = a._dpRecordingExtView;
                            var dataArray = data.getDataArray();
                            var i, ti;
                            for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                if (param.item === dataArray[i]) {
                                    data.updateItem(i, param.item);
                                    return;
                                }
                            }
                        }
                        else {
                            viewDlg.setActiveFocus(false);
                            prism.processRequest(a, null, 'onRecordingItemSelected', {
                                'item': param.item[0],
                                onClosed: function () {
                                    setTimeout(function () {
                                        viewDlg.setActiveFocus(true);
                                    }, 1);
                                }
                            }, function (response) {
                                prism.log('viewDlg DataItemSelected');
                            });
                        }
                    };
                    viewDlg.setDataControlDrawParam('list', {
                        itemHeight: 56,
                        scrollByItemSize: true
                    });
                    viewDlg.open({
                        transition: {
                            prevLayer: 'fadeOut',
                            newLayer: 'fadeIn'
                        },
                        onClosed: function () {
                            var multi = a._isExtMultiSelect;
                            if (a._isExtMultiSelect) {
                                var data = a._dpRecordingExtView;
                                var dataArray = data.getDataArray();
                                var i, ti;
                                a._isExtMultiSelect = undefined;
                                for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                    if (dataArray[i]._isChecked !== undefined) {
                                        dataArray[i]._isChecked = undefined;
                                        multi = a._isExtMultiSelect;
                                        if (a._isExtMultiSelect) {
                                            data = a._dpRecordingExtView;
                                            dataArray = data.getDataArray();
                                            a._isExtMultiSelect = undefined;
                                            for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                                if (dataArray[i]._isChecked !== undefined) {
                                                    dataArray[i]._isChecked = undefined;
                                                    data.updateItem(i, dataArray[i]);
                                                }
                                            }
                                        }
                                        data.updateItem(i, dataArray[i]);
                                    }
                                }
                            }
                            dataProvider.removeEventHandler(a);
                            if (a._dpRecordingExtView) {
                                a._dpRecordingExtView.destroy();
                                delete a._dpRecordingExtView;
                            }
                            setTimeout(function () {
                                recordingGrid.setActiveFocus(true);
                            }, 1);
                            /*
                             if (multi) {
                             recordingGrid.onItemSelected({
                             item : items[0]
                             });
                             }
                             */
                        }
                    });
                }
                else {
                    recordingGrid.setActiveFocus(false);
                    prism.processRequest(a, null, 'onRecordingItemSelected', {
                        'item': items[0],
                        onClosed: function () {
                            setTimeout(function () {
                                recordingGrid.setActiveFocus(true);
                            }, 1);
                        }
                    }, function (response) {
                        prism.log('finish onRecordingItemSelected');
                    });
                }
            };
            CRecordUtil.prototype.fnTobedeletedList_onItemSelected = function (a, recordingGrid, param) {
            };
            return CRecordUtil;
        })();
        /////////////////////////////////////////////////////////////////////////////////
        // Member
        /////////////////////////////////////////////////////////////////////////////////
        // Method
        var CRecordDraw = (function () {
            function CRecordDraw() {
            }
            CRecordDraw.prototype.fnRecordingListItemDrawer = function ($item, item, title, isSeries, count, isNew, isLocked, isSelected, isRecording, className, isChecked) {
                var $title, $episodes, $icons;
                var $channel, $recording, $play, $new, $series, $lock, $broken, $channelName, $logo;
                var episodeText = 'programmes';
                if (className === 'byProgramme' || className === 'byChannel') {
                    $channel = CRecUtil.appendTagWithClass(true, $item, 'channel');
                }
                $title = CRecUtil.appendTagWithClass(true, $item, 'title');
                $episodes = CRecUtil.appendTagWithClass(true, $item, 'episodes');
                $title.text(title);
                if (isSeries && count > 1) {
                    $episodes.text(count + ' ' + episodeText);
                }
                $item.addClass(className);
                if (className === 'byProgramme' || className === 'byChannel') {
                    if (item.getServiceId()) {
                        if ($channel) {
                            var logoUrl = item.getLogoUrl(CRecUtil.WIDTH_LIST_CHANNEL_LOGO);
                            if (logoUrl) {
                                $logo = $channel.children('.logo');
                                if ($logo.length) {
                                    $logo[0].src = logoUrl;
                                }
                                else {
                                    $channel.append($('<img>', {
                                        'class': 'logo',
                                        'src': logoUrl,
                                        'alt': ''
                                    }));
                                }
                            }
                            else {
                                var channel = item.getChannelInstance();
                                if (channel) {
                                    setTimeout(function () {
                                        hx.svc.IP_EPG.getChannelLogo(channel.sid, CRecUtil.WIDTH_LIST_CHANNEL_LOGO).then(function (url) {
                                            $logo = $channel.children('.logo');
                                            item.setLogoUrl(url, CRecUtil.WIDTH_LIST_CHANNEL_LOGO);
                                            if ($logo.length) {
                                                $logo[0].src = url;
                                            }
                                            else {
                                                $channel.append($('<img>', {
                                                    'class': 'logo',
                                                    'src': url,
                                                    'alt': ''
                                                }));
                                            }
                                        }, function () {
                                            $channelName = CRecUtil.appendTagWithClass(true, $channel, 'name', '<span>');
                                            $channelName.text(item.getChannelName() || '----');
                                        });
                                    }, 0);
                                }
                                else {
                                    $channelName = CRecUtil.appendTagWithClass(true, $channel, 'name', '<span>');
                                    $channelName.text(item.getChannelName() || '----');
                                }
                            }
                        }
                    }
                    else {
                        if ($channel) {
                            $channelName = CRecUtil.appendTagWithClass(true, $channel, 'name', '<span>');
                            $channelName.text(item.getChannelName() || '----');
                        }
                    }
                }
                $icons = $item.children('.icons');
                if ($icons.length === 0) {
                    $icons = $('<div>', {
                        'class': 'icons'
                    });
                    $item.append($icons);
                }
                $icons.empty();
                if (isRecording) {
                    $recording = $('<img>', {
                        'class': 'recording'
                    });
                    $icons.append($recording);
                }
                else if (item.isNew() && item.isBroken()) {
                    $broken = $('<img>', {
                        'class': 'broken'
                    });
                    $icons.append($broken);
                }
                else {
                    if (item.wasWatching()) {
                        $play = $('<img>', {
                            'class': 'partially'
                        });
                        $icons.append($play);
                    }
                    else {
                        $play = $('<img>', {
                            'class': 'play'
                        });
                        $icons.append($play);
                    }
                }
                if (isNew) {
                    $new = $('<img>', {
                        'class': 'new'
                    });
                    $icons.append($new);
                }
                if (isSeries && (className !== 'byChannel' && className !== 'byDayOfWeek')) {
                    $series = $('<img>', {
                        'class': 'series'
                    });
                    $icons.append($series);
                }
                if (isLocked) {
                    $lock = $('<img>', {
                        'class': 'lock'
                    });
                    $icons.append($lock);
                }
                if (isSelected) {
                    $item.addClass('checked');
                }
                else {
                    $item.removeClass('checked');
                    if (isChecked !== undefined) {
                        if (isChecked === true) {
                            $item.removeClass('unchecked');
                            $item.addClass('checked');
                        }
                        else {
                            $item.removeClass('checked');
                            $item.addClass('unchecked');
                        }
                    }
                    else {
                        $item.removeClass('checked');
                        $item.removeClass('unchecked');
                    }
                } // end of else
            };
            CRecordDraw.prototype.fnRecordingGridItemDrawer = function ($item, item, title, isSeries, count, isNew, locked, isSelected, isRecording, className, isChecked) {
                var casIDs, length, casClass, i;
                var TCasType = ['fta', 'na', 'ir', 'nd', 'va', 'cx', 'mg', 'cr'];
                var $thumbnail = CRecUtil.appendTagWithClass(true, $item, 'thumbnail');
                var $channelLogo = CRecUtil.appendTagWithClass(true, $item, 'channelLogo');
                var $title = CRecUtil.appendTagWithClass(true, $item, 'title');
                var $secondary = CRecUtil.appendTagWithClass(true, $item, 'secondary');
                if (isSeries) {
                    $item.addClass('series');
                }
                else {
                    $item.removeClass('series');
                }
                // thumbnail
                if (item.isNew() && item.isBroken()) {
                    $thumbnail.addClass('broken');
                }
                else {
                    if (item.isLocked()) {
                        $thumbnail.addClass('lock');
                        $thumbnail.css('background', '');
                    }
                    else {
                        $thumbnail.removeClass('broken lock');
                        if (item.getThumbnailUrl()) {
                            $thumbnail.css('background', 'url("' + item.getThumbnailUrl() + '") no-repeat center/100% 100%');
                        }
                        else {
                        }
                    }
                }
                // channel logo
                var logoUrl = item.getLogoUrl(CRecUtil.WIDTH_GRID_CHANNEL_LOGO);
                if (logoUrl) {
                    var $logo = $channelLogo.children('.logo');
                    if ($logo.length) {
                        $logo[0].src = logoUrl;
                    }
                    else {
                        $channelLogo.append($('<img>', {
                            'class': 'logo',
                            'src': logoUrl,
                            'alt': ''
                        }));
                    }
                }
                else {
                    setTimeout(function () {
                        hx.svc.IP_EPG.getChannelLogo(item.getServiceId(), CRecUtil.WIDTH_GRID_CHANNEL_LOGO).then(function (url) {
                            if (url) {
                                item.setLogoUrl(url, CRecUtil.WIDTH_GRID_CHANNEL_LOGO);
                                var $logo = $channelLogo.children('.logo');
                                if ($logo.length) {
                                    $logo[0].src = url;
                                }
                                else {
                                    $channelLogo.append($('<img>', {
                                        'class': 'logo',
                                        'src': url,
                                        'alt': ''
                                    }));
                                }
                            }
                            else {
                                $channelLogo.text((item.getChannelName()).toUpperCase());
                            }
                        }, function () {
                            $channelLogo.text((item.getChannelName()).toUpperCase());
                        });
                    }, 0);
                }
                // title
                var titleHtml = "<p>" + title + "</p>";
                var iconHtml = "<div class='icoRbArea'>";
                $title.html(titleHtml + iconHtml);
                // icons
                var $iconArea = $title.children('.icoRbArea');
                $iconArea.empty();
                var $episodeNum = CRecUtil.prependTagWithClass(isSeries, $iconArea, 'episodeNum');
                if ($episodeNum) {
                    $episodeNum.text('(' + count + ')');
                }
                CRecUtil.prependTagWithClass(locked, $iconArea, 'icoLocked');
                var $titleName = $title.children('p');
                if (isSeries && locked) {
                    $titleName.css('width', '100px');
                }
                else if (locked) {
                    $titleName.css('width', '140px');
                }
                else if (isSeries) {
                    $titleName.css('width', '125px');
                }
                else {
                }
                casIDs = item.getCasIDs();
                length = casIDs.length;
                if (length) {
                    for (i = 0; i < length; i += 1) {
                        casClass = TCasType[casIDs[i]];
                        if (casClass && item.isScrambled()) {
                            var classname = 'icoCas ' + casClass;
                            CRecUtil.prependTagWithClass(true, $iconArea, classname);
                        }
                    }
                }
                if (isRecording) {
                    CRecUtil.prependTagWithClass(true, $iconArea, 'icoRecOnGoing');
                }
                else {
                    if (item.wasWatching()) {
                        CRecUtil.prependTagWithClass(true, $iconArea, 'icoRecPartially');
                    }
                    else {
                        CRecUtil.prependTagWithClass(true, $iconArea, 'icoRecPlay');
                    }
                }
                // secondary
                var dispDate = dateFormat(item.getStartTime(), 'ddd dd mmm') + ' <span>|</span> ';
                var dispTime = dateFormat(item.getStartTime(), 'HH:MM') + ' - ' + dateFormat(item.getEndTime(), 'HH:MM');
                $secondary.html(dispDate + dispTime);
                if (isSelected) {
                    $item.addClass('checked');
                }
                else {
                    $item.removeClass('checked');
                    if (isChecked !== undefined) {
                        if (isChecked === true) {
                            $item.removeClass('unchecked');
                            $item.addClass('checked');
                        }
                        else {
                            $item.removeClass('checked');
                            $item.addClass('unchecked');
                        }
                    }
                    else {
                        $item.removeClass('checked');
                        $item.removeClass('unchecked');
                    }
                } // end of else
                CRecUtil.appendTagWithClass(isNew, $item, 'icoNew');
            };
            CRecordDraw.prototype.fnRecordingItemDrawerByRecent = function (param) {
                var items = param.item;
                if (!(items instanceof Array)) {
                    items = [items];
                }
                var item = items[0];
                var count = items.length;
                var isSelected = (items._isSelected) ? true : false;
                var isRecording = CRecUtil.fnListHasOngoingItem(items);
                var i, len, isLocked = false, isNew = false;
                for (i = 0, len = items.length; i < len; i += 1) {
                    if (items[i].isLocked()) {
                        isLocked = true;
                        break;
                    }
                }
                isNew = item.isNew();
                if (SViewType === 0 /* ELIST */) {
                    CRecDraw.fnRecordingListItemDrawer(param.$item, item, item.getTitle(), false, count, isNew, isLocked, isSelected, isRecording, 'byProgramme', items._isChecked);
                }
                else {
                    CRecDraw.fnRecordingGridItemDrawer(param.$item, item, item.getTitle(), false, count, isNew, isLocked, isSelected, isRecording, 'byProgramme', items._isChecked);
                }
            };
            CRecordDraw.prototype.fnRecordingItemDrawerByProgramme = function (param) {
                var items = param.item;
                if (!(items instanceof Array)) {
                    items = [items];
                }
                var item = items[0];
                var count = items.length;
                var isSelected = (items._isSelected) ? true : false;
                var isSeries = item.isSeries();
                var isRecording = CRecUtil.fnListHasOngoingItem(items);
                var i, len, isLocked = false, isNew = false;
                for (i = 0, len = items.length; i < len; i += 1) {
                    if (items[i].isLocked()) {
                        isLocked = true;
                        break;
                    }
                }
                if (isSeries) {
                    isNew = CRecUtil.fnListHasNewItem(items);
                }
                else {
                    isNew = item.isNew();
                }
                if (SViewType === 0 /* ELIST */) {
                    CRecDraw.fnRecordingListItemDrawer(param.$item, item, isSeries ? item.getSeriesTitle() : item.getTitle(), isSeries && (count > 0), count, isNew, isLocked, isSelected, isRecording, 'byProgramme', items._isChecked);
                }
                else {
                    CRecDraw.fnRecordingGridItemDrawer(param.$item, item, isSeries ? item.getSeriesTitle() : item.getTitle(), isSeries && (count > 0), count, isNew, isLocked, isSelected, isRecording, 'byProgramme', items._isChecked);
                }
            };
            CRecordDraw.prototype.fnRecordingItemDrawerByChannel = function (param) {
                var items = param.item;
                var isSelected = (items._isSelected) ? true : false;
                var isNew = CRecUtil.fnListHasNewItem(items);
                var item = items[0];
                var count = items.length;
                var isRecording = CRecUtil.fnListHasOngoingItem(items);
                var htmlTitle = '<span>' + prism.makeChannelNumberString(item.getChannelNo()) + '&nbsp</span>' + item.getChannelName();
                if (SViewType === 0 /* ELIST */) {
                    CRecDraw.fnRecordingListItemDrawer(param.$item, item, prism.makeChannelNumberString(item.getChannelNo()) + ' ' + item.getChannelName(), (count > 0), count, isNew, false, isSelected, isRecording, 'byChannel', items._isChecked);
                }
                else {
                    CRecDraw.fnRecordingGridItemDrawer(param.$item, item, htmlTitle, (count > 0), count, isNew, false, isSelected, isRecording, 'byChannel', items._isChecked);
                }
            };
            CRecordDraw.prototype.fnRecordingItemDrawerDayOfWeek = function (param) {
                var items = param.item;
                var isSelected = (items._isSelected) ? true : false;
                var isNew = CRecUtil.fnListHasNewItem(items);
                var item = items[0];
                var isRecording = CRecUtil.fnListHasOngoingItem(items);
                var count = items.length;
                if (item) {
                    if (SViewType === 0 /* ELIST */) {
                        CRecDraw.fnRecordingListItemDrawer(param.$item, item, CRecUtil.fnStringDayOfWeek(item.getStartTime().getDay()), (count > 0), count, isNew, false, isSelected, isRecording, 'byDayOfWeek', items._isChecked);
                    }
                    else {
                        CRecDraw.fnRecordingGridItemDrawer(param.$item, item, CRecUtil.fnStringDayOfWeek(item.getStartTime().getDay()), (count > 0), count, isNew, false, isSelected, isRecording, null, items._isChecked);
                    }
                }
            };
            CRecordDraw.prototype.fnRecordingItemDrawerStorage = function (param) {
                var items = param.item;
                var isSelected = (items._isSelected) ? true : false;
                var isNew = CRecUtil.fnListHasNewItem(items);
                var item = items[0];
                var isRecording = CRecUtil.fnListHasOngoingItem(items);
                var count = items.length;
                if (SViewType === 0 /* ELIST */) {
                    CRecDraw.fnRecordingListItemDrawer(param.$item, item, item.getStorageName(), (count > 0), count, isNew, false, isSelected, isRecording, 'byStorage', items._isChecked);
                }
                else {
                    CRecDraw.fnRecordingGridItemDrawer(param.$item, item, item.getStorageName(), (count > 0), count, isNew, false, isSelected, isRecording, null, items._isChecked);
                }
            };
            CRecordDraw.prototype.fnTobedeletedItemDrawer = function (param) {
                var item = param.item[0];
                var $item = param.$item;
                var $thumbnail, $title, $icons;
                var $broken, $play, $new, $series, $lock;
                if (!$item.hasClass('tobedeletedItem')) {
                    $item.addClass('tobedeletedItem');
                    $thumbnail = $('<div>', {
                        'class': 'thumbnail'
                    });
                    $title = $('<div>', {
                        'class': 'title'
                    });
                    $icons = $('<div>', {
                        'class': 'icons'
                    });
                    $item.append($thumbnail).append($title).append($icons);
                }
                else {
                    $title = $item.find('.title');
                    $icons = $item.find('.icons');
                }
                if (item.getServiceId()) {
                    if ($thumbnail) {
                        var logoUrl = item.getLogoUrl(CRecUtil.WIDTH_LIST_CHANNEL_LOGO);
                        if (logoUrl) {
                            var $logo = $thumbnail.children('.logo');
                            if ($logo.length) {
                                $logo[0].src = logoUrl;
                            }
                            else {
                                $thumbnail.append($('<img>', {
                                    'class': 'logo',
                                    'src': logoUrl,
                                    'alt': ''
                                }));
                            }
                        }
                        else {
                            var channel = item.getChannelInstance();
                            if (channel) {
                                setTimeout(function () {
                                    hx.svc.IP_EPG.getChannelLogo(channel.sid, CRecUtil.WIDTH_LIST_CHANNEL_LOGO).then(function (url) {
                                        item.setLogoUrl(url, CRecUtil.WIDTH_LIST_CHANNEL_LOGO);
                                        $thumbnail.append($('<img>', {
                                            'class': 'logo',
                                            'src': url,
                                            'alt': ''
                                        }));
                                    }, function () {
                                        $thumbnail.append($('<span>', {
                                            'class': 'name',
                                            'text': item.getChannelName() || '----'
                                        }));
                                    });
                                }, 0);
                            }
                            else {
                                $thumbnail.append($('<span>', {
                                    'class': 'name',
                                    'text': item.getChannelName() || '----'
                                }));
                            }
                        }
                    }
                }
                else {
                    if ($thumbnail) {
                        $thumbnail.append($('<span>', {
                            'class': 'name',
                            'text': item.getChannelName() || '----'
                        }));
                    }
                }
                $title.text(item.getTitle());
                $icons.empty();
                if (item.isNew() && item.isBroken()) {
                    $broken = $('<img>', {
                        'class': 'broken'
                    });
                    $icons.append($broken);
                }
                else {
                    if (item.wasWatching()) {
                        $play = $('<img>', {
                            'class': 'partially'
                        });
                        $icons.append($play);
                    }
                    else {
                        $play = $('<img>', {
                            'class': 'play'
                        });
                        $icons.append($play);
                    }
                }
                if (item.isNew()) {
                    $new = $('<img>', {
                        'class': 'new'
                    });
                    $icons.append($new);
                }
                if (item.isSeries()) {
                    $series = $('<img>', {
                        'class': 'series'
                    });
                    $icons.append($series);
                }
                if (item.isLocked()) {
                    $lock = $('<img>', {
                        'class': 'lock'
                    });
                    $icons.append($lock);
                }
                if (param.item._isChecked !== undefined) {
                    if (param.item._isChecked === true) {
                        $item.removeClass('unchecked');
                        $item.addClass('checked');
                    }
                    else {
                        $item.removeClass('checked');
                        $item.addClass('unchecked');
                    }
                }
                else {
                    $item.removeClass('checked');
                    $item.removeClass('unchecked');
                }
            };
            CRecordDraw.prototype.fnScheduledItemDrawer = function (param) {
                var item = param.item[0];
                var $item = param.$item;
                var $thumbnail, $title, $time, $remain, $icon;
                var startTime, endTime, duration, remainSec, now;
                var timeText, remainText;
                var defaultUrl = __MODEL__.TDefaultThumbnailList.TV;
                if (!$item.hasClass('scheduledItem')) {
                    $item.addClass('scheduledItem');
                    $thumbnail = $('<div>', {
                        'class': 'thumbnail'
                    });
                    $title = $('<div>', {
                        'class': 'title'
                    });
                    $time = $('<div>', {
                        'class': 'time'
                    });
                    $remain = $('<div>', {
                        'class': 'remain'
                    });
                    $icon = $('<div>', {
                        'class': 'icon'
                    });
                    $item.append($thumbnail).append($title).append($time).append($remain).append($icon);
                }
                else {
                    $title = $item.find('.title');
                    $time = $item.find('.time');
                    $remain = $item.find('.remain');
                    $icon = $item.find('.icon');
                }
                if (item.getServiceId()) {
                    if ($thumbnail) {
                        var logoUrl = item.getLogoUrl(CRecUtil.WIDTH_LIST_CHANNEL_LOGO);
                        if (logoUrl) {
                            var $logo = $thumbnail.children('.logo');
                            if ($logo.length) {
                                $logo[0].src = logoUrl;
                            }
                            else {
                                $thumbnail.append($('<img>', {
                                    'class': 'logo',
                                    'src': logoUrl,
                                    'alt': ''
                                }));
                            }
                        }
                        else {
                            var channel = item.getChannelInstance();
                            if (channel) {
                                setTimeout(function () {
                                    hx.svc.IP_EPG.getChannelLogo(channel.sid, CRecUtil.WIDTH_LIST_CHANNEL_LOGO).then(function (url) {
                                        item.setLogoUrl(url, CRecUtil.WIDTH_LIST_CHANNEL_LOGO);
                                        $thumbnail.append($('<img>', {
                                            'class': 'logo',
                                            'src': url,
                                            'alt': ''
                                        }));
                                    }, function () {
                                        $thumbnail.append($('<span>', {
                                            'class': 'name',
                                            'text': item.getChannelName() || '----'
                                        }));
                                    });
                                }, 0);
                            }
                            else {
                                $thumbnail.append($('<span>', {
                                    'class': 'name',
                                    'text': item.getChannelName() || '----'
                                }));
                            }
                        }
                    }
                }
                else {
                    if ($thumbnail) {
                        $thumbnail.append($('<span>', {
                            'class': 'name',
                            'text': item.getChannelName() || '----'
                        }));
                    }
                }
                $title.text(item.getTitle());
                startTime = item.getStartTime();
                duration = item.getDuration() * 1000;
                endTime = new Date(startTime.getTime() + duration);
                if (item.isPendingSeriesSchedule() === true) {
                    $time.text(tr('LOC_GET_NEXT_EPISODE_ID'));
                }
                else {
                    timeText = startTime.format('dd mmm HH:MM') + ' - ' + endTime.format('HH:MM');
                    $time.text(timeText);
                    now = new Date();
                    remainSec = Math.floor((startTime - now.getTime()) / 1000);
                    if (remainSec > 0) {
                        if (remainSec > 86400) {
                            remainText = 'In ' + Math.floor(remainSec / 86400) + " days";
                        }
                        else {
                            if (remainSec > 3600) {
                                var remainMins = remainSec % 3600;
                                if (remainMins > 30) {
                                    remainText = 'In ' + Math.ceil(remainSec / 3600) + " hours";
                                }
                                else {
                                    remainText = 'In ' + Math.floor(remainSec / 3600) + " hours";
                                }
                            }
                            else {
                                remainText = 'In ' + Math.ceil(remainSec / 60) + ' mins';
                            }
                        }
                    }
                    $remain.text(remainText);
                }
                if (item.isReminder()) {
                    $icon.addClass('reminder');
                }
                else if (item.isSeries()) {
                    $icon.addClass('series');
                }
                else {
                    switch (item.getRepeat() & 0xFF) {
                        case 0:
                            $icon.addClass('onceRec');
                            break;
                        case 0x7F:
                            $icon.addClass('dailyRec');
                            break;
                        default:
                            $icon.addClass('weeklyRec');
                            break;
                    }
                }
                if (param.item._isChecked !== undefined) {
                    if (param.item._isChecked === true) {
                        $item.removeClass('unchecked');
                        $item.addClass('checked');
                    }
                    else {
                        $item.removeClass('checked');
                        $item.addClass('unchecked');
                    }
                }
                else {
                    $item.removeClass('checked');
                    $item.removeClass('unchecked');
                }
            };
            CRecordDraw.prototype.fnRecordingItemDrawerByExtention = function (param) {
                var items = param.item;
                if (!(items instanceof Array)) {
                    items = [items];
                }
                var item = items[0];
                var count = items.length;
                var isSelected = (items._isSelected) ? true : false;
                var isSeries = item.isSeries();
                var isRecording = CRecUtil.fnListHasOngoingItem(items);
                var i, len, isLocked = false, isNew = false;
                for (i = 0, len = items.length; i < len; i += 1) {
                    if (items[i].isLocked()) {
                        isLocked = true;
                        break;
                    }
                }
                if (isSeries) {
                    isNew = CRecUtil.fnListHasNewItem(items);
                }
                else {
                    isNew = item.isNew();
                }
                CRecDraw.fnRecordingListItemDrawer(param.$item, item, isSeries ? item.getSeriesTitle() : item.getTitle(), isSeries && (count > 1), count, isNew, isLocked, isSelected, isRecording, 'byProgramme', items._isChecked);
            };
            CRecordDraw.prototype.fnOpenScheduledDlg = function (a, item) {
                a.onRequestChannelGroupInfo(function (channelGroupInfo, currentChannelId) {
                    if ((channelGroupInfo.length === 0) || (currentChannelId === undefined)) {
                        var notificationDlg = prism.createNotificationDlg(a);
                        notificationDlg.setTextHtml(tr('LOC_NO_CHANNEL_ID'));
                        notificationDlg.setTimeout(3000);
                        notificationDlg.open();
                        return;
                    }
                    var settingDlg = prism.createFullSettingDlg(a);
                    var reschedule = false, repeatActiveIndex, duration;
                    var scheduleInfo = {};
                    var now = new Date();
                    var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                    if (item) {
                        reschedule = true;
                        scheduleInfo.ccid = item.getChannelId();
                        scheduleInfo.startTime = item.getStartTime();
                        scheduleInfo.duration = item.getDuration();
                        scheduleInfo.repeatDays = item.getRepeat();
                        scheduleInfo.reminder = item.isReminder();
                    }
                    else {
                        scheduleInfo.ccid = currentChannelId;
                        // +10 min default
                        now.setSeconds(0);
                        now.setMinutes(now.getMinutes() + 11);
                        scheduleInfo.startTime = now;
                        //60 min default
                        scheduleInfo.duration = 3600;
                        scheduleInfo.repeatDays = 0;
                        if (bIsGetPvrHDD === true) {
                            scheduleInfo.reminder = false;
                        }
                        else {
                            scheduleInfo.reminder = true;
                        }
                    }
                    duration = scheduleInfo.duration / 60;
                    switch (scheduleInfo.repeatDays & 0xFF) {
                        case 0:
                            repeatActiveIndex = 0;
                            break;
                        case 0x7F:
                            repeatActiveIndex = 1;
                            break;
                        default:
                            repeatActiveIndex = 2;
                            break;
                    }
                    settingDlg.setTitleHtml((reschedule) ? tr('LOC_REC_RESCHEDULEPROG_ID') : tr('LOC_REC_SETSCHMANUALLY_ID'));
                    if (bIsGetPvrHDD === true) {
                        settingDlg.setSettingListItems([
                            {
                                type: 'channel',
                                text: tr('LOC_CHANNEL_ID'),
                                config: {
                                    ccid: scheduleInfo.ccid,
                                    channelGroupList: channelGroupInfo
                                }
                            },
                            {
                                type: 'date',
                                text: tr('LOC_DATE_ID'),
                                config: {
                                    date: scheduleInfo.startTime,
                                    period: 6
                                }
                            },
                            {
                                type: 'time',
                                text: tr('LOC_REC_STARTTIME_ID'),
                                config: {
                                    time: scheduleInfo.startTime
                                }
                            },
                            {
                                type: 'number',
                                text: tr('LOC_REC_DURATION_ID'),
                                string: tr('LOC_REC_ENTERDURATION_ID'),
                                config: {
                                    value: duration,
                                    unitText: tr('LOC_MINS_ID')
                                }
                            },
                            {
                                type: 'multiSelection',
                                text: tr('LOC_REPEAT_ID'),
                                config: {
                                    selectionList: [
                                        {
                                            text: tr('LOC_ONCE_ID')
                                        },
                                        {
                                            text: tr('LOC_DAILY_ID')
                                        },
                                        {
                                            text: tr('LOC_WEEKLY_ID')
                                        }
                                    ],
                                    activeIndex: repeatActiveIndex
                                }
                            },
                            {
                                type: 'multiSelection',
                                text: tr('LOC_MODE_ID'),
                                config: {
                                    selectionList: [
                                        {
                                            text: tr('LOC_RECORDING_ID')
                                        },
                                        {
                                            text: tr('LOC_REC_REMINDER_ID')
                                        }
                                    ],
                                    activeIndex: scheduleInfo.reminder ? 1 : 0
                                }
                            }
                        ]);
                    }
                    else {
                        settingDlg.setSettingListItems([
                            {
                                type: 'channel',
                                text: tr('LOC_CHANNEL_ID'),
                                config: {
                                    ccid: scheduleInfo.ccid,
                                    channelGroupList: channelGroupInfo
                                }
                            },
                            {
                                type: 'date',
                                text: tr('LOC_DATE_ID'),
                                config: {
                                    date: scheduleInfo.startTime,
                                    period: 6
                                }
                            },
                            {
                                type: 'time',
                                text: tr('LOC_REC_STARTTIME_ID'),
                                config: {
                                    time: scheduleInfo.startTime
                                }
                            },
                            {
                                type: 'number',
                                text: tr('LOC_REC_DURATION_ID'),
                                string: tr('LOC_REC_ENTERDURATION_ID'),
                                config: {
                                    value: duration,
                                    unitText: tr('LOC_MINS_ID')
                                }
                            },
                            {
                                type: 'multiSelection',
                                text: tr('LOC_REPEAT_ID'),
                                config: {
                                    selectionList: [
                                        {
                                            text: tr('LOC_ONCE_ID')
                                        },
                                        {
                                            text: tr('LOC_DAILY_ID')
                                        },
                                        {
                                            text: tr('LOC_WEEKLY_ID')
                                        }
                                    ],
                                    activeIndex: repeatActiveIndex
                                }
                            },
                            {
                                type: 'multiSelection',
                                text: tr('LOC_MODE_ID'),
                                config: {
                                    selectionList: [
                                        {
                                            text: tr('LOC_REC_REMINDER_ID')
                                        }
                                    ],
                                    activeIndex: 0
                                }
                            }
                        ]);
                    }
                    settingDlg.onItemChanged = function (index, settingItem, changedValue) {
                        prism.log('onItemChanged');
                    };
                    settingDlg.onInputChanged = function (input) {
                        prism.log('onInputChanged');
                        var maxDuration = 1440;
                        // max 1440 mins
                        var bValid = (input < maxDuration) || false;
                        if (!bValid) {
                            hx.msg('response', {
                                text: hx.l('LOC_REC_INVALID_DURATION_INPUT_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                        return bValid;
                    };
                    settingDlg.onOk = function (changedInfoList) {
                        var i, ti, info;
                        for (i = 0, ti = changedInfoList.length; i < ti; i += 1) {
                            info = changedInfoList[i];
                            switch (info.index) {
                                case 0:
                                    //channel
                                    scheduleInfo.ccid = info.changedValue;
                                    break;
                                case 1:
                                    //date
                                    scheduleInfo.startTime.setYear(info.changedValue.getFullYear());
                                    scheduleInfo.startTime.setMonth(info.changedValue.getMonth());
                                    scheduleInfo.startTime.setDate(info.changedValue.getDate());
                                    break;
                                case 2:
                                    //startTime
                                    scheduleInfo.startTime.setHours(info.changedValue.getHours());
                                    scheduleInfo.startTime.setMinutes(info.changedValue.getMinutes());
                                    scheduleInfo.startTime.setSeconds(0);
                                    scheduleInfo.startTime.setMilliseconds(0);
                                    break;
                                case 3:
                                    //duration
                                    scheduleInfo.duration = info.changedValue * 60;
                                    break;
                                case 4:
                                    if (!info.changedValue) {
                                        //once
                                        scheduleInfo.repeatDays = 0;
                                    }
                                    else {
                                        switch (info.changedValue) {
                                            case 0:
                                                //once
                                                scheduleInfo.repeatDays = 0;
                                                break;
                                            case 1:
                                                //daily
                                                scheduleInfo.repeatDays = 0x7F;
                                                break;
                                            case 2:
                                                //weekly
                                                scheduleInfo.repeatDays = Math.pow(2, ((new Date(scheduleInfo.startTime * 1000)).getDay()));
                                                break;
                                        }
                                    }
                                    break;
                                case 5:
                                    if (info.changedValue === 1) {
                                        scheduleInfo.reminder = true;
                                    }
                                    else {
                                        scheduleInfo.reminder = false;
                                    }
                                    break;
                            }
                        }
                        if ((scheduleInfo.startTime.getTime() - new Date().getTime()) < 60000) {
                            var notificationDlg = prism.createNotificationDlg(a);
                            notificationDlg.setTextHtml(tr('LOC_REC_INVALIDTIMEVAL_ID'));
                            notificationDlg.setTimeout(3000);
                            notificationDlg.open();
                        }
                        else {
                            if (reschedule) {
                                if (changedInfoList.length) {
                                    prism.processRequest(a, null, 'doScheduledItemEdit', {
                                        item: item,
                                        rescheduleInfo: scheduleInfo
                                    }, function (response) {
                                        prism.log('finish doScheduledItemEdit');
                                        a.reloadContextMenuItems();
                                    });
                                }
                            }
                            else {
                                prism.processRequest(a, null, 'doScheduledItemAdd', {
                                    newScheduleInfo: scheduleInfo
                                }, function (response) {
                                    prism.log('finish doScheduledItemAdd');
                                    //a.reloadContextMenuItems();
                                });
                            }
                        }
                    };
                    settingDlg.open({
                        dimm: true
                    });
                });
            };
            CRecordDraw.prototype.fnLoadingMsgOnOff = function (a, onOff, text) {
                var $loading = a._$self.find('.nonblockLoading');
                if (onOff) {
                    if (!$loading.length) {
                        $loading = $('<div>', {
                            'class': 'nonblockLoading',
                            'text': text
                        });
                        a._$self.append($loading);
                    }
                }
                else {
                    if ($loading.length) {
                        $loading.remove();
                    }
                }
            };
            CRecordDraw.prototype.fnNoRecordingMsgOnOff = function (a, onOff) {
                var $contentArea = a.$contentArea();
                var $noContent = $contentArea.find('*.noContent');
                var handled = false;
                var text;
                if (a._activeViewType === 'recent') {
                    text = tr('LOC_NO_RECENT_RECORDING_MSG_ID');
                }
                else {
                    text = tr('LOC_REC_YOUWANTTORECORDING_ID');
                }
                if (onOff) {
                    if (!$noContent.length) {
                        $contentArea.append($('<div>', {
                            'class': 'noContent img'
                        })).append($('<div>', {
                            'class': 'noContent title',
                            'html': tr('LOC_REC_NORECORDINGS_ID')
                        })).append($('<div>', {
                            'class': 'noContent context',
                            'html': text
                        }));
                        var doKeyRight = function () {
                            if (a.stackSize() === 0) {
                                a.doKeyRight = doKeyRight;
                                handled = true;
                            }
                            else {
                                a.doKeyRight = undefined;
                                handled = CLayeredGroupControl.prototype.doKey.call(a, hx.key.KEY_RIGHT);
                            }
                            return handled;
                        };
                        a.CloseMultiCheck();
                        a.doKeyRight = doKeyRight;
                    }
                    else {
                        $('.noContent.context').html(text);
                    }
                }
                else {
                    if ($noContent.length) {
                        $noContent.remove();
                        a._viewTypeControl.addClass('show');
                        a.doKeyRight = undefined;
                    }
                }
            };
            CRecordDraw.prototype.fnUpdateBadgeCount = function (a, count) {
                var $focusedMenu = a._menuVLayout.getFocusedItem();
                if ($focusedMenu && $focusedMenu.length) {
                    $focusedMenu.addClass('selected');
                    var $badge = $focusedMenu.children('#badge');
                    if ($badge && $badge.length) {
                        $badge.text(count);
                    }
                    else {
                        $focusedMenu.append($('<div>', {
                            'id': 'badge'
                        }).text(count));
                    }
                }
            };
            CRecordDraw.prototype.fnContentsIndexOnOff = function (a, onOff) {
                var $layer = a.$layer();
                var $contentsIndex = a._$contentsIndex;
                var dpRecordings = a._dpRecordings;
                var focusedItem, $focusedItem;
                var total, focusedIndex, text;
                if (dpRecordings) {
                    total = dpRecordings.getCount();
                    if (a._recordingView) {
                        focusedItem = a._recordingView.getFocusedItem();
                        if (focusedItem) {
                            $focusedItem = focusedItem.$item;
                            if ($focusedItem) {
                                focusedIndex = $focusedItem.attr('data');
                                text = 'Total ' + '<span>' + (parseInt(focusedIndex) + 1) + ' / ' + total + '</span>';
                            }
                        }
                    }
                }
                if (onOff) {
                    if (!$contentsIndex) {
                        $contentsIndex = $('<div>', {
                            'class': 'contentsIndex'
                        }).html(text);
                        $layer.append($contentsIndex);
                        a._$contentsIndex = $contentsIndex;
                    }
                    else {
                        $contentsIndex.html(text);
                    }
                }
                else {
                    if ($contentsIndex) {
                        $contentsIndex.remove();
                        a._$contentsIndex = null;
                    }
                }
            };
            return CRecordDraw;
        })();
        var CRecordFileOperation = (function () {
            function CRecordFileOperation() {
                this.fnOptionMentHandleDeleteCheckedSchedules = function (a, items) {
                    var data = a._ScheduleProvider;
                    var confirm = prism.createButtonDlg(a);
                    var deleteMsgText = tr('LOC_REC_DOYOUWANTSTOPSCH_ID');
                    var i, ti;
                    prism.log('fnOptionMentHandleDeleteCheckedSchedules ScheduleProvider.length ' + data.getCount());
                    confirm.setTextHtml(deleteMsgText);
                    confirm.setButtonInfoList([{
                        text: tr('LOC_YES_ID'),
                        fn: function () {
                            setTimeout(function () {
                                a._optionMenuOnGoing = 'schduled.delete';
                                var dataArray = data.getDataArray();
                                var items;
                                var request = [];
                                for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                    items = dataArray[i];
                                    if (items._isChecked === true && !CRecUtil.fnListHasOngoingItem(items)) {
                                        request.push(items[0]);
                                    }
                                }
                                if (request.length > 0) {
                                    prism.processRequest(a, null, 'doScheduledItemDelete', {
                                        'items': request
                                    }, function (response) {
                                        var items;
                                        if (a._isMultiSelect && a._ScheduleProvider !== null) {
                                            a._isMultiSelect = false;
                                            data = a._ScheduleProvider;
                                            for (i = 0, ti = data.getCount(); i < ti; i += 1) {
                                                items = data.getItem(i);
                                                items._isChecked = undefined;
                                                data.updateItem(i, items);
                                            }
                                        }
                                        a._optionMenuOnGoing = undefined;
                                        a.reloadContextMenuItems();
                                    });
                                }
                            }, 1);
                        }
                    }, {
                        text: tr('LOC_NO_ID')
                    }]);
                    confirm.open({
                        dimm: true,
                        onClosed: function () {
                            setTimeout(function () {
                                a.reloadContextMenuItems();
                            }, 1);
                        }
                    });
                };
                this.fnOptionMentHandleSelectSchedule = function (a, itemList) {
                    var item = itemList[0];
                    var data, dataArray;
                    var i, ti, items, keys = [];
                    a._isMultiSelect = true;
                    data = a._ScheduleProvider;
                    dataArray = data.getDataArray();
                    var isSameSchedule = function (Sche1, Sche2) {
                        if (Sche1.getChannelId() !== Sche2.getChannelId()) {
                            return false;
                        }
                        if (Sche1.getInstance().factoryType !== Sche2.getInstance().factoryType) {
                            return false;
                        }
                        if (Sche1.getProgrammeID() && Sche2.getProgrammeID()) {
                            return (Sche1.getProgrammeID() === Sche2.getProgrammeID());
                        }
                        return (Sche1.getStartTime().getTime() === Sche2.getStartTime().getTime());
                    };
                    for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                        items = dataArray[i];
                        if (isSameSchedule(items[0], item) === true) {
                            items._isChecked = true;
                        }
                        else {
                            items._isChecked = false;
                        }
                        data.updateItem(i, items);
                    }
                    a.reloadContextMenuItems();
                };
                this.fnOptionMentHandleSelectAllSchedule = function (a, itemList) {
                    var item = itemList[0];
                    if (!a._isSelectAll) {
                        var data = a._ScheduleProvider;
                        var dataArray = data.getDataArray();
                        var i, ti, items, keys = [], selectedItemCount = 0;
                        for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                            items = dataArray[i];
                            if (!CRecUtil.fnListHasOngoingItem(items)) {
                                items._isChecked = true;
                                data.updateItem(i, items);
                                selectedItemCount += 1;
                            }
                        }
                        if (selectedItemCount) {
                            a._isSelectAll = true;
                            a.reloadContextMenuItems();
                        }
                    }
                };
                this.fnOptionMentHandleSelectTobedeleted = function (a, itemList) {
                    var item = itemList[0];
                    var data, dataArray;
                    var i, ti, items, keys = [];
                    a._isMultiSelect = true;
                    data = a._dpTobedeleted;
                    dataArray = data.getDataArray();
                    var isSameRecording = function (Rec1, Rec2) {
                        if (Rec1.getChannelId() !== Rec2.getChannelId()) {
                            return false;
                        }
                        if (Rec1.getInstance().factoryType !== Rec2.getInstance().factoryType) {
                            return false;
                        }
                        if (Rec1.getProgrammeID() && Rec2.getProgrammeID()) {
                            return (Rec1.getProgrammeID() === Rec2.getProgrammeID());
                        }
                        return (Rec1.getStartTime().getTime() === Rec2.getStartTime().getTime());
                    };
                    for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                        items = dataArray[i];
                        if (isSameRecording(items[0], item) === true) {
                            items._isChecked = true;
                        }
                        else {
                            items._isChecked = false;
                        }
                        data.updateItem(i, items);
                    }
                    a.reloadContextMenuItems();
                };
                this.fnOptionMentHandleSelectAllTobedeleted = function (a, itemList) {
                    var item = itemList[0];
                    if (!a._isSelectAll) {
                        var data = a._dpTobedeleted; //a._ScheduleProvider;
                        var dataArray = data.getDataArray();
                        var i, ti, items, keys = [], selectedItemCount = 0;
                        for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                            items = dataArray[i];
                            if (!CRecUtil.fnListHasOngoingItem(items)) {
                                items._isChecked = true;
                                data.updateItem(i, items);
                                selectedItemCount += 1;
                            }
                        }
                        if (selectedItemCount) {
                            a._isSelectAll = true;
                            a.reloadContextMenuItems();
                        }
                    }
                };
                this.fnOptionMentHandleSortBy = function (a) {
                    a._sortType = (a._sortType === a.ESortType.ETIME) ? a.ESortType.EALPHABET : a.ESortType.ETIME;
                    hx.svc.WEB_STORAGE.setSortTypeInRecordings(a._sortType);
                    a.drawContentsView();
                    a.reloadContextMenuItems();
                };
            }
            CRecordFileOperation.prototype.fnOptionMenuProvider = function (a, activeFocusInfo) {
                var ret;
                if (activeFocusInfo) {
                    var activeControl = activeFocusInfo.control;
                    if (!activeControl) {
                        return null;
                    }
                    if (activeControl instanceof CGridControl || activeControl instanceof CListControl) {
                        var focusedItemInfo = activeControl.getFocusedItem();
                        if (!focusedItemInfo || !focusedItemInfo.item) {
                            return null;
                        }
                        var menuItems = [], item;
                        var menuItemList = [];
                        item = focusedItemInfo.item;
                        console.log(item);
                        prism.log('fnOptionMenuProvider');
                        var fnMenuItemCreator = function (name, fn, selectitem, noclose, redraw) {
                            return {
                                name: name,
                                fn: function () {
                                    prism.log(hx.util.format('prism.page.recordings.js: fnMenuItemCreator - name = {0}', name));
                                    fn(a, selectitem, name);
                                },
                                noclose: noclose,
                                redraw: redraw
                            };
                        };
                        menuItemList[CRecUtil.TOptionMenuItem.EDelete] = fnMenuItemCreator(tr('LOC_DELETE_ID'), CRecFOP.fnOptionMentHandleDelete, (a._isSelectAll || (a._dpRecordingExtView ? a._isExtMultiSelect : a._isMultiSelect)) ? CRecUtil.fnGetSelectedItems(a) : item, true, false);
                        menuItemList[CRecUtil.TOptionMenuItem.ECopy] = fnMenuItemCreator(tr('LOC_COPY_ID'), CRecFOP.fnOptionMentHandleCopy, (a._isSelectAll || (a._dpRecordingExtView ? a._isExtMultiSelect : a._isMultiSelect)) ? CRecUtil.fnGetSelectedItems(a) : item, true, true);
                        menuItemList[CRecUtil.TOptionMenuItem.EMove] = fnMenuItemCreator(tr('LOC_MOVE_ID'), CRecFOP.fnOptionMentHandleMove, (a._isSelectAll || (a._dpRecordingExtView ? a._isExtMultiSelect : a._isMultiSelect)) ? CRecUtil.fnGetSelectedItems(a) : item, true, true);
                        menuItemList[CRecUtil.TOptionMenuItem.EStopCopying] = fnMenuItemCreator(tr('LOC_REC_STOP_COPYING_ID'), CRecFOP.fnOptionMentHandleStopCopy, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EStopMoving] = fnMenuItemCreator(tr('LOC_REC_STOP_MOVING_ID'), CRecFOP.fnOptionMentHandleStopMove, item);
                        menuItemList[CRecUtil.TOptionMenuItem.ESelect] = fnMenuItemCreator(tr('LOC_CAS_IR_MAIL_MSG_SELECT_ID'), CRecFOP.fnOptionMentHandleSelect, item);
                        menuItemList[CRecUtil.TOptionMenuItem.ESelectAll] = fnMenuItemCreator(tr('LOC_SELECT_ALL_ID'), CRecFOP.fnOptionMentHandleSelectAll, item, true, true);
                        menuItemList[CRecUtil.TOptionMenuItem.ECancel] = fnMenuItemCreator(tr('LOC_CANCEL2_ID'), CRecFOP.fnOptionMentHandleSelectAllCancel, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EStopRec] = fnMenuItemCreator(tr('LOC_STOP_REC_BTN_ID'), CRecFOP.fnOptionMentHandleStopRecording, item);
                        menuItemList[CRecUtil.TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_LOCK_ID'), CRecFOP.fnOptionMentHandleLock, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EUnLock] = fnMenuItemCreator(tr('LOC_UNLOCK_ID'), CRecFOP.fnOptionMentHandleLock, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EReSchedule] = fnMenuItemCreator(tr('LOC_REC_RESCHEDULE_ID'), CRecFOP.fnOptionMentHandleReschedule, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EScheduleDelete] = fnMenuItemCreator(tr('LOC_DELETE_ID'), CRecFOP.fnOptionMentHandleDeleteSchedule, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EScheduleDeletes] = fnMenuItemCreator(tr('LOC_DELETE_ID'), CRecFOP.fnOptionMentHandleDeleteCheckedSchedules, item);
                        menuItemList[CRecUtil.TOptionMenuItem.ESelectSchedule] = fnMenuItemCreator(tr('LOC_CAS_IR_MAIL_MSG_SELECT_ID'), CRecFOP.fnOptionMentHandleSelectSchedule, item);
                        menuItemList[CRecUtil.TOptionMenuItem.ESelectAllSchedule] = fnMenuItemCreator(tr('LOC_SELECT_ALL_ID'), CRecFOP.fnOptionMentHandleSelectAllSchedule, item, true, true);
                        menuItemList[CRecUtil.TOptionMenuItem.EKeep] = fnMenuItemCreator(tr('LOC_KEEP_ID'), CRecFOP.fnOptionMentHandleKeep, item);
                        menuItemList[CRecUtil.TOptionMenuItem.EKeeps] = fnMenuItemCreator(tr('LOC_KEEP_ID'), CRecFOP.fnOptionMenuHandleKeepCheckedItems, (a._isSelectAll || a._isMultiSelect) ? CRecUtil.fnGetSelectedKeepItems(a) : item);
                        menuItemList[CRecUtil.TOptionMenuItem.ESelectTobedeleted] = fnMenuItemCreator(tr('LOC_CAS_IR_MAIL_MSG_SELECT_ID'), CRecFOP.fnOptionMentHandleSelectTobedeleted, item);
                        menuItemList[CRecUtil.TOptionMenuItem.ESelectAllTobedeleted] = fnMenuItemCreator(tr('LOC_SELECT_ALL_ID'), CRecFOP.fnOptionMentHandleSelectAllTobedeleted, item, true, true);
                        menuItemList[CRecUtil.TOptionMenuItem.ESortBy] = fnMenuItemCreator(a.STR_TARGET_SORT_TYPE_ARR[a._sortType], CRecFOP.fnOptionMentHandleSortBy);
                        var moreitemMSG = function () {
                            if (a._dpRecordingExtView ? a._isExtMultiSelect : a._isMultiSelect) {
                                var data;
                                if (a._ScheduleProvider !== null) {
                                    data = a._ScheduleProvider;
                                }
                                else {
                                    if (a._activeViewType === 'tobedeleted') {
                                        data = a._dpTobedeleted;
                                    }
                                    else {
                                        data = (a._dpRecordingExtView || a._dpRecordings);
                                    }
                                }
                                var dataArray = data.getDataArray();
                                var flag = false;
                                var i, ti, items;
                                for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                    items = dataArray[i];
                                    if (items._isChecked === true) {
                                        return true;
                                    }
                                }
                                hx.msg('response', {
                                    text: hx.l('LOC_SELECT_ONE_ITEM_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                                return false;
                            }
                            return true;
                        };
                        if ((a._ScheduleProvider === null && (a._isSelectAll || a._isMultiSelect)) || item[0] instanceof __MODEL__.IRecordingItem || item instanceof __MODEL__.IRecordingItem) {
                            if (a._isSelectAll || (a._dpRecordingExtView ? a._isExtMultiSelect : a._isMultiSelect)) {
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EDelete]);
                                switch (a._activeViewType) {
                                    case 'tobedeleted':
                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EKeeps]);
                                        break;
                                    case 'recent':
                                        break;
                                    default:
                                        if (a._ScheduleProvider === null) {
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ECopy]);
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EMove]);
                                        }
                                        break;
                                }
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ECancel]);
                            }
                            else {
                                var stopRecItem;
                                var fnCheckIsLocked = function (items) {
                                    var i, len, isLock = false;
                                    for (i = 0, len = items.length; i < len; i += 1) {
                                        if (items[i].isLocked()) {
                                            isLock = true;
                                            break;
                                        }
                                    }
                                    return isLock;
                                };
                                switch (a._optionMenuOnGoing) {
                                    case 'copy':
                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EStopCopying]);
                                        if ((a._activeViewType === 'programme') || a._dpRecordingExtView) {
                                            if (item instanceof Array) {
                                                if (fnCheckIsLocked(item)) {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                }
                                                else {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                }
                                            }
                                            else {
                                                if (item.isLocked()) {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                }
                                                else {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                }
                                            }
                                        }
                                        break;
                                    case 'move':
                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EStopMoving]);
                                        if ((a._activeViewType === 'programme') || a._dpRecordingExtView) {
                                            if (item instanceof Array) {
                                                if (fnCheckIsLocked(item)) {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                }
                                                else {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                }
                                            }
                                            else {
                                                if (item.isLocked()) {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                }
                                                else {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                }
                                            }
                                        }
                                        break;
                                    default:
                                        if (!(item instanceof Array)) {
                                            stopRecItem = item.isRecordingOnGoing();
                                        }
                                        else {
                                            stopRecItem = CRecUtil.fnListHasOngoingItem(item);
                                        }
                                        if (!stopRecItem) {
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EDelete]);
                                            switch (a._activeViewType) {
                                                case 'tobedeleted':
                                                case 'recent':
                                                    break;
                                                default:
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ECopy]);
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EMove]);
                                                    break;
                                            }
                                        }
                                        else {
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EStopRec]);
                                        }
                                        if ((a._activeViewType === 'programme') || a._dpRecordingExtView) {
                                            if (item instanceof Array) {
                                                if (item[0].isSeries()) {
                                                    if (CRecUtil.fnListHasLockedWholeItem(item)) {
                                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                    }
                                                    else if (CRecUtil.fnListHasLockedItem(item)) {
                                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                    }
                                                    else {
                                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                    }
                                                }
                                                else {
                                                    if (fnCheckIsLocked(item)) {
                                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                    }
                                                    else {
                                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                    }
                                                }
                                            }
                                            else {
                                                if (item.isLocked()) {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EUnLock]);
                                                }
                                                else {
                                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ELock]);
                                                }
                                            }
                                        }
                                        else if (a._activeViewType === 'tobedeleted') {
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EKeep]);
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESelectTobedeleted]);
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESelectAllTobedeleted]);
                                        }
                                        if (a._activeViewType !== 'tobedeleted') {
                                            if (stopRecItem === false || (item instanceof Array && item.length > 2 && stopRecItem === true)) {
                                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESelect]);
                                            }
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESelectAll]);
                                        }
                                        if (a._activeViewType === 'programme') {
                                            menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESortBy]);
                                        }
                                        break;
                                }
                            } // end of else
                            /*
                            var moreitemMSG = function () {
                                var data = (a._dpRecordingExtView) ? (a._isExtMultiSelect ? a._dpRecordingExtView : null) : (a._isMultiSelect ? a._dpRecordings : null);
                                if (data) {
                                    var dataArray = data.getDataArray();
                                    var flag = false;
                                    var i, ti, items;
                                    for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                                        items = dataArray[i];
                                        if (items._isChecked === true) {
                                            return true;
                                        }
                                    }
                                    hx.msg('response', {
                                        text: hx.l('LOC_SELECT_MORE_THAN_1_ITEM_ID'),
                                        auto_close: true,
                                        close_time: 3000
                                    });
                                    return false;
                                }
                                return true;
                            };     */
                            ret = {
                                text: tr('LOC_EDIT_ID'),
                                menuItems: menuItems,
                                control: activeControl,
                                accept: moreitemMSG
                            };
                        }
                        else if (a._ScheduleProvider !== null) {
                            prism.log('fnOptionMenuProvider======================');
                            if (item.length === 0) {
                                throw "INVALID";
                            }
                            if (a._isSelectAll === true || a._isMultiSelect === true) {
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EScheduleDeletes]);
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ECancel]);
                            }
                            else {
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EScheduleDelete]);
                                if (item[0] instanceof __MODEL__.IScheduledItem) {
                                    if (!item[0].isPendingSeriesSchedule()) {
                                        menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EReSchedule]);
                                    }
                                }
                                else {
                                    menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.EReSchedule]);
                                }
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESelectSchedule]);
                                menuItems.push(menuItemList[CRecUtil.TOptionMenuItem.ESelectAllSchedule]);
                            }
                            ret = {
                                text: tr('LOC_EDIT_ID'),
                                menuItems: menuItems,
                                control: activeControl,
                                accept: moreitemMSG
                            };
                        }
                    }
                }
                return ret;
            };
            CRecordFileOperation.prototype.fnOptionMentHandleDelete = function (a, items) {
                if (a._activeViewType === 'recent') {
                    a.doKeyOption();
                    a._optionMenuOnGoing = 'delete';
                    prism.processRequest(a, null, 'doRecentItemDelete', {
                        'items': items.slice(0)
                    }, function (response) {
                        var recentList = hx.svc.HISTORY.getPlaybackList();
                        a._optionMenuOnGoing = undefined;
                        a.reloadContextMenuItems();
                        a.CloseMultiCheck();
                        if (recentList && recentList.length > 0) {
                            a.drawContentsView(a._activeViewType);
                        }
                        else {
                            a.drawContentsView(a._activeViewType);
                            CRecDraw.fnNoRecordingMsgOnOff(a, true);
                            a.doNavi('l');
                            a._menuVLayout.setFocusedItem(CRecUtil.TMenuItem.ERECENT, true);
                            a._menuVLayout._$self.find('.prism-button').removeClass('selected');
                            var $button = a._menuVLayout._$self.find('.recent');
                            $button.addClass('selected');
                            a._menuVLayout.setActiveIndex(a._activeViewIndex);
                        }
                    });
                }
                else {
                    var checkIsLocked = function (deleteCallbackFn) {
                        var i, ti, locked = false;
                        for (i = 0, ti = items.length; i < ti; i += 1) {
                            if (items[i].isLocked()) {
                                locked = true;
                                break;
                            }
                        }
                        if (locked) {
                            setTimeout(function () {
                                var passwordDlg = prism.createQueryPasswordDlg(a);
                                passwordDlg.setTextHtml('<span>' + tr('LOC_REC_ENTYOURPIN_ID') + '</span>');
                                passwordDlg.open({
                                    dimm: true,
                                    onPasswordEntered: function (password) {
                                        prism.processRequest(a, null, 'doRecordingItemLock', {
                                            'items': items,
                                            'lock': false,
                                            'password': password
                                        }, function (response) {
                                            if (response) {
                                                passwordDlg.close();
                                                deleteCallbackFn();
                                            }
                                            else {
                                                passwordDlg.reset();
                                            }
                                            a.reloadContextMenuItems();
                                        });
                                    },
                                    onClosed: function () {
                                        a.reloadContextMenuItems();
                                    }
                                });
                            }, 1);
                        }
                        else {
                            setTimeout(function () {
                                deleteCallbackFn();
                            }, 1);
                        }
                    };
                    var confirm = prism.createButtonDlg(a);
                    var deleteMsgText = ((items.length > 1) ? tr('LOC_DELETE_THESE_PROGRAMMES_ID') : tr('LOC_DELETE_THIS_PROGRAMME_ID'));
                    confirm.setTextHtml(deleteMsgText);
                    confirm.setButtonInfoList([
                        {
                            text: tr('LOC_YES_ID'),
                            fn: function () {
                                checkIsLocked(function () {
                                    a._optionMenuOnGoing = 'delete';
                                    prism.processRequest(a, null, 'doRecordingItemDelete', {
                                        'items': items.slice(0)
                                    }, function (response) {
                                        a.reloadContextMenuItems();
                                        a.CloseMultiCheck();
                                    });
                                });
                            }
                        },
                        {
                            text: tr('LOC_NO_ID')
                        }
                    ]);
                    a.doKeyOption();
                    confirm.open({
                        dimm: true,
                        onClosed: function () {
                            a.reloadContextMenuItems();
                        }
                    });
                }
            };
            CRecordFileOperation.prototype.fnOptionMentHandleLock = function (a, items, name) {
                var i, ti, locked = false;
                if (items[0].isSeries()) {
                    if (name === tr('LOC_LOCK_ID')) {
                        locked = false;
                    }
                    else if (name === tr('LOC_UNLOCK_ID')) {
                        locked = true;
                    }
                }
                else {
                    for (i = 0, ti = items.length; i < ti; i += 1) {
                        if (items[i].isLocked()) {
                            locked = true;
                            break;
                        }
                    }
                }
                if (!locked) {
                    //lock
                    prism.processRequest(a, null, 'doRecordingItemLock', {
                        'items': items.slice(0),
                        'lock': true
                    }, function (response) {
                        a.reloadContextMenuItems();
                    });
                }
                else {
                    //unlock
                    var passwordDlg = prism.createQueryPasswordDlg(a);
                    passwordDlg.setTextHtml('<span>' + tr('LOC_REC_ENTYOURPIN_ID') + '</span>');
                    passwordDlg.open({
                        dimm: true,
                        onPasswordEntered: function (password) {
                            prism.processRequest(a, null, 'doRecordingItemLock', {
                                'items': items,
                                'lock': false,
                                'password': password
                            }, function (response) {
                                if (response) {
                                    passwordDlg.close();
                                }
                                else {
                                    passwordDlg.reset();
                                }
                                a.reloadContextMenuItems();
                            });
                        },
                        onClosed: function () {
                            a.reloadContextMenuItems();
                        }
                    });
                }
            };
            CRecordFileOperation.prototype.fnOptionMentHandleKeep = function (a, items) {
                var confirm = prism.createButtonDlg(a);
                var deleteMsgText = ((items.length > 1) ? tr('LOC_WANT_KEEP_THIS_EPISODES_ID') : tr('LOC_WANT_KEEP_THIS_EPISODE_ID'));
                confirm.setTextHtml(deleteMsgText);
                confirm.setButtonInfoList([
                    {
                        text: tr('LOC_YES_ID'),
                        fn: function () {
                            setTimeout(function () {
                                prism.processRequest(a, null, 'doRecordingItemKeep', {
                                    'items': items.slice(0),
                                    'keep': true
                                }, function (response) {
                                });
                            }, 1);
                            a.CloseMultiCheck();
                        }
                    },
                    {
                        text: tr('LOC_NO_ID')
                    }
                ]);
                confirm.open({
                    dimm: true,
                    onClosed: function () {
                        a.reloadContextMenuItems();
                    }
                });
            };
            CRecordFileOperation.prototype.fnOptionMenuHandleKeepCheckedItems = function (a, items) {
                var data = a._dpTobedeleted;
                var confirm = prism.createButtonDlg(a);
                var deleteMsgText = tr('LOC_WANT_KEEP_THIS_EPISODES_ID');
                var i, ti;
                confirm.setTextHtml(deleteMsgText);
                confirm.setButtonInfoList([{
                    text: tr('LOC_YES_ID'),
                    fn: function () {
                        a._optionMenuOnGoing = 'keeps';
                        if (items.length > 0) {
                            prism.processRequest(a, null, 'doRecordingItemKeep', {
                                'items': items,
                                'keep': true
                            }, function (response) {
                                var items;
                                if (a._isMultiSelect && a._dpTobedeleted !== null) {
                                    a._isMultiSelect = false;
                                    data = a._dpTobedeleted;
                                    for (i = 0, ti = data.getCount(); i < ti; i += 1) {
                                        items = data.getItem(i);
                                        items._isChecked = undefined;
                                        data.updateItem(i, items);
                                    }
                                }
                                a._optionMenuOnGoing = undefined;
                                prism.log('[fnOptionMenuHandleKeepCheckedItems] Reload Item!!');
                                a.reloadContextMenuItems();
                            });
                        }
                    }
                }, {
                    text: tr('LOC_NO_ID')
                }]);
                confirm.open({
                    dimm: true,
                    onClosed: function () {
                        setTimeout(function () {
                            a.reloadContextMenuItems();
                        }, 1);
                    }
                });
            };
            CRecordFileOperation.prototype.fnOptionMentHandleStopRecording = function (a, items) {
                var confirm = prism.createButtonDlg(a);
                confirm.setTextHtml(tr('LOC_REC_DOYOUWANTSTOPREC_ID'));
                confirm.setButtonInfoList([
                    {
                        text: tr('LOC_YES_ID'),
                        fn: function () {
                            a._optionMenuOnGoing = 'stop_recording';
                            prism.processRequest(a, null, 'doRecordingItemStop', {
                                'items': items.slice(0)
                            }, function (response) {
                                a._optionMenuOnGoing = undefined;
                                a.reloadContextMenuItems();
                            });
                        }
                    },
                    {
                        text: tr('LOC_NO_ID')
                    }
                ]);
                confirm.open({
                    dimm: true
                });
            };
            // copy or move
            CRecordFileOperation.prototype.fnOptionMentHandleOperation = function (a, items, OnGoning, processRequest, NoStroageNoti, Protect, Protect2) {
                var _fnRequset = function (opItems, targetId) {
                    a._optionMenuOnGoing = OnGoning;
                    prism.processRequest(a, null, processRequest, {
                        'items': opItems,
                        'targetStorageId': targetId
                    }, function (response) {
                        if (response) {
                            a.reloadContextMenuItems();
                        }
                        else {
                            a._optionMenuOnGoing = undefined;
                        }
                    });
                };
                var param, exceptPvrHdd;
                if (a._activeViewType === 'storage') {
                    exceptPvrHdd = false;
                }
                else {
                    exceptPvrHdd = true;
                }
                param = {
                    exceptPvrHdd: exceptPvrHdd,
                    sourceStorageId: (items.length > 0) ? items[0].getStorageId() : items
                };
                a.onRequestTargetStorageInfo(param, function (storageList) {
                    if (!storageList.length) {
                        a.doKeyOption();
                        a.CloseMultiCheck();
                        prism.createNotificationDlg(a, NoStroageNoti).open();
                        return;
                    }
                    var selectionDlg = prism.createStorageSelectionDlg(a);
                    selectionDlg.setSelectionItems(storageList);
                    selectionDlg.setSelectionIndex(0);
                    selectionDlg.onSelectionChanged = function (param) {
                        selectionDlg.close();
                        a.doKeyOption();
                        a.CloseMultiCheck();
                        var selectedStorageItem = param.item;
                        var decryptionCopy;
                        switch (selectedStorageItem.type) {
                            case 'hdd':
                            case 'dtc':
                            case 'usbp':
                                decryptionCopy = false;
                                break;
                            default:
                                decryptionCopy = true;
                                break;
                        }
                        if (!decryptionCopy) {
                            _fnRequset(items.slice(0), selectedStorageItem.id);
                        }
                        else {
                            var groupRet = CRecUtil.fnGroupingItemByCopyControl(items);
                            var hasLockedItem = CRecUtil.fnListHasLockedItem(groupRet.copyOkItems);
                            var multiFileCopy = (items.length > 1);
                            if (hasLockedItem) {
                                var passwordDlg = prism.createQueryPasswordDlg(a);
                                if (multiFileCopy) {
                                    passwordDlg.setTextHtml('<span>' + tr('LOC_REC_SOME_PROTECTED_BY_LOCK') + '</span>' + '<br><span>' + tr('LOC_ENTER_PASSWORD_ID') + '</span>');
                                }
                                else {
                                    passwordDlg.setTextHtml('<span>' + tr('LOC_REC_PROTECTED_BY_LOCK') + '</span>' + '<br><span>' + tr('LOC_ENTER_PASSWORD_ID') + '</span>');
                                }
                                passwordDlg.open({
                                    dimm: true,
                                    onPasswordEntered: function (password) {
                                        a.onVerifyFileOperationLockPasswd({
                                            password: password
                                        }, function (response) {
                                            if (response) {
                                                passwordDlg.close();
                                                if (!groupRet.noCopyItems.length) {
                                                    _fnRequset(groupRet.copyOkItems, selectedStorageItem.id);
                                                }
                                                else {
                                                    if (!multiFileCopy) {
                                                        var notificationDlg = prism.createNotificationDlg(a);
                                                        notificationDlg.setTextHtml(Protect);
                                                        notificationDlg.setTimeout(3000);
                                                        notificationDlg.open();
                                                    }
                                                    else {
                                                        var textHtml = Protect2;
                                                        var btnDlg = prism.createButtonDlg(null, {
                                                            textHtml: textHtml,
                                                            buttonList: [tr('LOC_OK_ID')]
                                                        }, function (index) {
                                                            if (index === 0) {
                                                                _fnRequset(groupRet.copyOkItems, selectedStorageItem.id);
                                                            }
                                                        });
                                                        btnDlg.open({
                                                            onClosed: function () {
                                                                a.reloadContextMenuItems();
                                                            }
                                                        });
                                                    }
                                                }
                                            }
                                            else {
                                                passwordDlg.reset();
                                            }
                                        });
                                    },
                                    onClosed: function () {
                                        a.reloadContextMenuItems();
                                    }
                                });
                            }
                            else {
                                if (!groupRet.noCopyItems.length) {
                                    _fnRequset(groupRet.copyOkItems, selectedStorageItem.id);
                                }
                                else {
                                    if (!multiFileCopy) {
                                        var notificationDlg = prism.createNotificationDlg(a);
                                        notificationDlg.setTextHtml(Protect);
                                        notificationDlg.setTimeout(3000);
                                        notificationDlg.open();
                                    }
                                    else {
                                        var textHtml = Protect2;
                                        var btnDlg = prism.createButtonDlg(null, {
                                            textHtml: textHtml,
                                            buttonList: [tr('LOC_OK_ID')]
                                        }, function (index) {
                                            if (index === 0) {
                                                _fnRequset(groupRet.copyOkItems, selectedStorageItem.id);
                                            }
                                        });
                                        btnDlg.open({
                                            onClosed: function () {
                                                a.reloadContextMenuItems();
                                            }
                                        });
                                    }
                                }
                            }
                        }
                    };
                    selectionDlg.open({
                        dimm: true,
                        onClosed: function () {
                            a.reloadContextMenuItems();
                        }
                    });
                });
            };
            CRecordFileOperation.prototype.fnOptionMentHandleCopy = function (a, items) {
                //copy..
                CRecFOP.fnOptionMentHandleOperation(a, items, 'copy', 'doRecordingItemCopy', tr('LOC_REC_NOSTORAGE_COPYTO_ID'), tr('LOC_REC_CNCOPY_BECOZ_PROTECT_ID'), tr('LOC_REC_CNCOPY_SOMEREC_BECOZ_PROTECT_ID'));
            };
            CRecordFileOperation.prototype.fnOptionMentHandleMove = function (a, items) {
                //move..
                CRecFOP.fnOptionMentHandleOperation(a, items, 'move', 'doRecordingItemMove', tr('LOC_REC_NOSTORAGE_MOVETO_ID'), tr('LOC_REC_CNMOVE_BECOZ_PROTECT_ID'), tr('LOC_REC_CNMOVE_SOMEREC_BECOZ_PROTECT_ID'));
            };
            CRecordFileOperation.prototype.fnOptionMentHandleStopCopy = function (a, items) {
                prism.processRequest(a, null, 'doRecordingItemStopCopying', {
                    'items': items.slice(0)
                }, function (response) {
                    a._optionMenuOnGoing = undefined;
                    a.reloadContextMenuItems();
                });
            };
            CRecordFileOperation.prototype.fnOptionMentHandleStopMove = function (a, items) {
                prism.processRequest(a, null, 'doRecordingItemStopMoving', {
                    'items': items.slice(0)
                }, function (response) {
                    a._optionMenuOnGoing = undefined;
                    a.reloadContextMenuItems();
                });
            };
            CRecordFileOperation.prototype.fnOptionMentHandleSelect = function (a) {
                var data, dataArray;
                var i, ti, items, keys = [];
                if (a._dpRecordingExtView) {
                    data = a._dpRecordingExtView;
                    a._isExtMultiSelect = true;
                }
                else if (a._dpRecordings) {
                    data = a._dpRecordings;
                    a._isMultiSelect = true;
                    a._menuVLayout.addClass('dis');
                    a._viewTypeControl.addClass('dis');
                    //a.removeChildControl(a._menuVLayout);
                    a._contentView.setChildControl([a._recordingViewContainer]);
                    a.setChildControl([a._contentView]);
                    a.draw({
                        setActiveFocus: true
                    });
                }
                else {
                    return;
                }
                dataArray = data.getDataArray();
                var fnSelect = function (items) {
                    var i;
                    if (!(items instanceof Array) && items.isRecordingOnGoing()) {
                        return false;
                    }
                    for (i = 0; i < items.length; i += 1) {
                        if (items[i].isRecordingOnGoing()) {
                            return false;
                        }
                    }
                    return true;
                };
                for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                    items = dataArray[i];
                    if (!items._isChecked && fnSelect(items)) {
                        items._isChecked = false;
                        data.updateItem(i, items);
                    }
                }
                a.reloadContextMenuItems();
            };
            CRecordFileOperation.prototype.fnOptionMentHandleSelectAll = function (a) {
                if (!a._isSelectAll) {
                    var data = (a._dpRecordingExtView || a._dpRecordings);
                    var dataArray = data.getDataArray();
                    var i, ti, items, keys = [], selectedItemCount = 0;
                    for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                        items = dataArray[i];
                        if (!items._isSelected && !CRecUtil.fnListHasOngoingItem(items)) {
                            items._isSelected = true;
                            data.updateItem(i, items);
                            selectedItemCount += 1;
                        }
                    }
                    if (selectedItemCount) {
                        a._isSelectAll = true;
                        a.reloadContextMenuItems();
                    }
                }
            };
            CRecordFileOperation.prototype.fnOptionMentHandleSelectAllCancel = function (a) {
                var data;
                var i, ti, items;
                if (a._isSelectAll || a._isMultiSelect) {
                    if (a._ScheduleProvider !== null) {
                        data = a._ScheduleProvider;
                    }
                    else {
                        if (a._activeViewType === 'tobedeleted') {
                            data = a._dpTobedeleted;
                        }
                        else {
                            data = (a._dpRecordingExtView || a._dpRecordings);
                        }
                        if (a._isMultiSelect === true) {
                            a.CloseMultiCheck();
                        }
                    }
                    a._isSelectAll = false;
                    a._isMultiSelect = false;
                    var i, ti, items;
                    for (i = 0, ti = data.getCount(); i < ti; i += 1) {
                        items = data.getItem(i);
                        if (items._isSelected || items._isChecked !== undefined) {
                            items._isSelected = false;
                            items._isChecked = undefined;
                            data.updateItem(i, items);
                        }
                    }
                }
                a.reloadContextMenuItems();
            };
            CRecordFileOperation.prototype.fnOptionMentHandleDeleteSchedule = function (a, items) {
                prism.log('fnOptionMentHandleDeleteSchedule items.length ' + items.length);
                var confirm = prism.createButtonDlg(a);
                var deleteMsgText = tr('LOC_REC_DOYOUWANTSTOPSCH_ID');
                confirm.setTextHtml(deleteMsgText);
                confirm.setButtonInfoList([
                    {
                        text: tr('LOC_YES_ID'),
                        fn: function () {
                            setTimeout(function () {
                                a._optionMenuOnGoing = 'schduled.delete';
                                prism.processRequest(a, null, 'doScheduledItemDelete', {
                                    'items': items.slice(0)
                                }, function (response) {
                                    a._optionMenuOnGoing = undefined;
                                    a.reloadContextMenuItems();
                                });
                            }, 1);
                        }
                    },
                    {
                        text: tr('LOC_NO_ID')
                    }
                ]);
                confirm.open({
                    dimm: true,
                    onClosed: function () {
                        a.reloadContextMenuItems();
                    }
                });
            };
            CRecordFileOperation.prototype.fnOptionMentHandleReschedule = function (a, itemList) {
                var item = itemList[0];
                a.reloadContextMenuItems();
                CRecDraw.fnOpenScheduledDlg(a, item);
            };
            return CRecordFileOperation;
        })();
        var CRecUtil = new CRecordUtil();
        var CRecDraw = new CRecordDraw();
        var CRecFOP = new CRecordFileOperation();
        var SViewType = 0 /* ELIST */;
        var CRecordingsPage = (function (_super) {
            __extends(CRecordingsPage, _super);
            function CRecordingsPage($el) {
                _super.call(this, $el);
                this._ScheduleProvider = null;
                this.ESortType = {
                    ETIME: 0,
                    EALPHABET: 1
                };
                this._sortType = hx.svc.WEB_STORAGE.getSortTypeInRecordings() || this.ESortType.ETIME;
                this.STR_TARGET_SORT_TYPE_ARR = ["Sort A-Z", "Sort by Time"];
            }
            CRecordingsPage.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a.setId('recordingsPage');
                a._activeViewIndex = 0;
                a.setOptionMenuProvider(function (activeFocusInfo) {
                    return CRecFOP.fnOptionMenuProvider(a, activeFocusInfo);
                });
            };
            CRecordingsPage.prototype.getSelectedItem = function () {
                var a = this;
                var selected = a._recordingView.getFocusedItem() || {};
                return (selected.item) || [];
            };
            CRecordingsPage.prototype.setStorageInfo = function (freeSizeGb, totalSizeGb) {
                var a = this;
                a._storageInfo = {
                    free: freeSizeGb,
                    total: totalSizeGb
                };
                CRecUtil.fnUpdateStorageInfo(a);
            };
            CRecordingsPage.prototype.draw = function (param) {
                var a = this;
                var storageMenu;
                if (!a.$layer()) {
                    a.createLayer({
                        fnAfterCreate: function () {
                            if (hx.config.recStorageStatusType2 !== true) {
                                a.setTitleText(tr('LOC_CRECORDINGS_ID'));
                            }
                            else {
                                var $storageStatusArea = $('<div>', {
                                    'class': 'storageInfoArea'
                                });
                                var $storageGraphCanvas = $('<canvas>', {
                                    'class': 'graph'
                                }).attr({
                                    'width': '84px',
                                    'height': '84px'
                                });
                                ;
                                $storageStatusArea.append($storageGraphCanvas);
                                $storageStatusArea.append($('<div>', {
                                    'class': 'info'
                                }));
                                a._$storageStatusArea = $storageStatusArea;
                                a.setTitleTextWithStorageStatus(tr('LOC_CRECORDINGS_ID'), $storageStatusArea);
                                CRecUtil.fnUpdateStorageInfo(a);
                            }
                            var menuVLayout = new CVerticalLayoutControl(a.$menuArea());
                            var buttonInfo = {};
                            var menuDrawerList;
                            var fnStorageInfoDrawer = null;
                            var fnButtonDrawer = function (index, $item, text, type, aFocusable) {
                                if (aFocusable === void 0) { aFocusable = true; }
                                $item.addClass('prism-button ' + type);
                                $item.text(text);
                                buttonInfo[index] = {
                                    type: type,
                                    $item: $item
                                };
                                if (a._isMultiSelect) {
                                    $item.addClass('dim30');
                                }
                                else {
                                    $item.removeClass('dim30');
                                }
                                if (!aFocusable) {
                                    $item.removeClass("-f");
                                    $item.addClass("-dis");
                                }
                            };
                            if (hx.config.recStorageStatusType2 !== true) {
                                fnStorageInfoDrawer = function (index, $item) {
                                    var $storageGraphCanvas = $('<canvas>', {
                                        'class': 'graph'
                                    }).attr({
                                        'width': '230px',
                                        'height': '30px'
                                    });
                                    $item.addClass('storageStatusArea');
                                    $item.append($('<div>', {
                                        'class': 'txtTitle'
                                    }).text(tr('LOC_REC_HDDSTATUS_ID')));
                                    $item.append($storageGraphCanvas);
                                    // hole: 40, inner 86, focused outer: 102
                                    $item.append($('<div>', {
                                        'class': 'info'
                                    }));
                                    a._$storageStatusArea = $item;
                                    CRecUtil.fnUpdateStorageInfo(a);
                                    $item.removeClass('-f');
                                    return false;
                                };
                            }
                            if (hx.config.useInternalPvr !== 0) {
                                menuDrawerList = [
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLEPROGRAMME_ID'), 'programme');
                                    },
                                    //function (index, $item) {
                                    //    fnButtonDrawer(index, $item, tr('LOC_CRECENT_ID'), 'recent');
                                    //},
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_BIGCHANNEL_ID'), 'channel');
                                    },
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLEDAYOFWEEK_ID'), 'dayOfWeek');
                                    },
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLESTORAGE_ID'), 'storage');
                                    },
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLESCHEDULE_ID'), 'schedule');
                                    },
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr("LOC_TO_BE_DELETED_ID"), 'tobedeleted', hx.svc.SETTING_UTIL.getAutoDeleteOnOff());
                                    },
                                    null,
                                    fnStorageInfoDrawer
                                ];
                            }
                            else {
                                menuDrawerList = [
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLEPROGRAMME_ID'), 'programme');
                                    },
                                    //function (index, $item) {
                                    //    fnButtonDrawer(index, $item, tr('LOC_CRECENT_ID'), 'recent');
                                    //},
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_BIGCHANNEL_ID'), 'channel');
                                    },
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLEDAYOFWEEK_ID'), 'dayOfWeek');
                                    },
                                    function (index, $item) {
                                        fnButtonDrawer(index, $item, tr('LOC_REC_TITLESCHEDULE_ID'), 'schedule');
                                    },
                                    null,
                                    fnStorageInfoDrawer
                                ];
                            }
                            if (hx.config.useInternalPvr === 1) {
                                //storageMenu = ((hx.svc.STORAGE_MASS.getRecPVRList()).length > 0);
                                storageMenu = true;
                            }
                            else {
                                storageMenu = ((hx.svc.STORAGE_MASS.getRecPVRList()).length >= 1);
                            }
                            if (!storageMenu) {
                                menuDrawerList.splice(3, 1);
                            }
                            menuVLayout.setItemDrawers(menuDrawerList);
                            menuVLayout.onItemSelected = function (index, $item) {
                                if (buttonInfo[index].type === 'schedule') {
                                    CRecUtil.fnMenuLayout_onSchedule(a, index, $item);
                                }
                                else if (buttonInfo[index].type === 'tobedeleted') {
                                    a._tempViewType = a._activeViewType;
                                    a._activeViewType = 'tobedeleted';
                                    CRecUtil.fnMenuLayout_onTobedeleted(a, index, $item);
                                }
                                else {
                                    a._activeViewIndex = index;
                                    a.drawContentsView(buttonInfo[index].type);
                                }
                            };
                            menuVLayout.addClass('show');
                            a._menuVLayout = menuVLayout;
                            var viewTypeBtnDrawer = function (index, $item) {
                                var $icon, $text;
                                var menuText;
                                if (index === 0) {
                                    menuText = 'LIST';
                                }
                                else {
                                    menuText = 'GRID';
                                }
                                if (SViewType === 0 /* ELIST */) {
                                    if (index === 0) {
                                        $item.addClass('selected');
                                    }
                                }
                                else {
                                    if (index === 1) {
                                        $item.addClass('selected');
                                    }
                                }
                                if (a._isMultiSelect) {
                                    $item.addClass('dim30');
                                }
                                else {
                                    $item.removeClass('dim30');
                                }
                                $item.addClass(menuText.toLowerCase());
                                $icon = $('<img>');
                                $text = $('<div>').text(menuText);
                                $item.append($icon).append($text);
                                return true;
                            };
                            var viewTypeControl = new CHorizontalLayoutControl(null);
                            viewTypeControl.addClass('viewType');
                            viewTypeControl.setDrawParam({
                                padding: -1
                            });
                            viewTypeControl.setItemDrawers([viewTypeBtnDrawer, viewTypeBtnDrawer]);
                            viewTypeControl.onItemSelected = function (index, $item) {
                                a._viewTypeControl._$self.children().removeClass('selected');
                                $item.addClass('selected');
                                a._recordingView.destroy();
                                a._recordingView = null;
                                if (index === 0) {
                                    a.drawListRecordings();
                                }
                                else {
                                    a.drawGridRecordings();
                                }
                            };
                            viewTypeControl.doShow = function () {
                                viewTypeControl.addClass('show');
                            };
                            viewTypeControl.doHide = function () {
                                viewTypeControl.removeClass('show');
                            };
                            viewTypeControl.setFocusedItem(0);
                            a._viewTypeControl = viewTypeControl;
                            var recordingView;
                            if (SViewType === 1 /* EGRID */) {
                                recordingView = new CGridControl(null);
                                recordingView.setDrawParam({
                                    maxColCount: 4,
                                    itemWidth: 210 + 10,
                                    itemHeight: 118 + 10,
                                    scrollByItemSize: true,
                                    scrollPageSizeByPageUpDownKey: true
                                });
                                recordingView.setDataProvider(new CDataProvider([]));
                                recordingView.setDataDrawer(function () {
                                    prism.log('called recordingGrid data drawer');
                                });
                                recordingView.onItemSelected = function (param) {
                                    CRecUtil.fnRecordingGrid_onItemSelected(a, recordingView, param);
                                };
                                a.addOptionMenuTarget(recordingView);
                                a._recordingView = recordingView;
                            }
                            else {
                                recordingView = new CListControl(null);
                                recordingView.setDrawParam({
                                    align: 'vertical',
                                    itemWidth: 870,
                                    itemHeight: 56,
                                    scrollByItemSize: true
                                });
                                recordingView.addClass('recordingListView');
                                recordingView.setDataProvider(new CDataProvider([]));
                                recordingView.setDataDrawer(function () {
                                    prism.log('called recordingList data drawer');
                                });
                                recordingView.onItemSelected = function (param) {
                                    CRecUtil.fnRecordingGrid_onItemSelected(a, recordingView, param);
                                };
                                a.addOptionMenuTarget(recordingView);
                                a._recordingView = recordingView;
                            }
                            recordingView.addEventHandler('FocusChanged', recordingView, function ($old, $new) {
                                var dpRecordings = a._dpRecordings;
                                if (dpRecordings) {
                                    var newIndex = parseInt($new.attr('data'));
                                    var oldIndex = parseInt($old.attr('data'));
                                    if (isNaN(newIndex)) {
                                        newIndex = 0;
                                    }
                                    if (isNaN(oldIndex)) {
                                        oldIndex = 0;
                                    }
                                    if (recordingView instanceof __CONTROLS__.CListControl) {
                                        if (oldIndex === 1 && newIndex === 0) {
                                            a._viewTypeControl.show();
                                        }
                                        else if (oldIndex === 7 && newIndex === 8) {
                                            a._viewTypeControl.hide();
                                        }
                                        else if (newIndex >= 8) {
                                            a._viewTypeControl.hide();
                                        }
                                    }
                                    else {
                                        if (oldIndex > 3 && newIndex <= 3) {
                                            a._viewTypeControl.show();
                                        }
                                        else if (oldIndex <= 7 && newIndex > 7) {
                                            a._viewTypeControl.hide();
                                        }
                                    }
                                }
                            });
                            recordingView.addEventHandler('ActiveFocusedGained', recordingView, function ($item) {
                                var dpRecordings = a._dpRecordings;
                                if (dpRecordings) {
                                    var focusIndex = parseInt($item.attr('data'));
                                    if (isNaN(focusIndex)) {
                                        focusIndex = 0;
                                    }
                                    if (recordingView instanceof __CONTROLS__.CListControl) {
                                        if (focusIndex <= 7) {
                                            a._viewTypeControl.show();
                                        }
                                        else {
                                            a._viewTypeControl.hide();
                                        }
                                    }
                                    else {
                                        if (focusIndex <= 7) {
                                            a._viewTypeControl.show();
                                        }
                                        else {
                                            a._viewTypeControl.hide();
                                        }
                                    }
                                }
                            });
                            var recordingViewContainer = new CViewGroupControl(a.$contentArea());
                            recordingViewContainer.setDrawParam({
                                showScroll: true
                            });
                            recordingViewContainer.setChildControl([recordingView]);
                            a._recordingViewContainer = recordingViewContainer;
                            var view = new CVerticalGroupControl(null);
                            view.setChildControl([viewTypeControl, recordingViewContainer]);
                            a._contentView = view;
                            a.setChildControl([menuVLayout, view]);
                            a.draw({
                                setActiveFocus: true
                            });
                            view.setFocusedChld(recordingViewContainer);
                            a._activeViewType = null;
                            a.drawContentsView();
                            if (SViewType === 0 /* ELIST */) {
                                viewTypeControl.setActiveIndex(0);
                            }
                            else {
                                viewTypeControl.setActiveIndex(1);
                            }
                        }
                    });
                }
                else {
                    _super.prototype.draw.call(a, param);
                }
            };
            CRecordingsPage.prototype.setViewConfig = function (aRecordingDataProvider, type) {
                var a = this;
                a._menuVLayout._$self.find('.prism-button').removeClass('selected');
                var $button = a._menuVLayout._$self.find('.' + type);
                $button.addClass('selected');
                a._menuVLayout.setActiveIndex(a._activeViewIndex);
                var array = aRecordingDataProvider.getDataArray();
                prism.log('setViewConfig array.length ' + array.length);
                var sortedList = [];
                if (a._recordingView instanceof __CONTROLS__.CListControl) {
                    SViewType = 0 /* ELIST */;
                }
                else {
                    SViewType = 1 /* EGRID */;
                }
                var handler = {
                    'programme': function () {
                        sortedList = CRecUtil.fnMakeListByProgramme(a, array);
                        a._recordingView.setDataDrawer(CRecDraw.fnRecordingItemDrawerByProgramme);
                    },
                    'recent': function () {
                        sortedList = CRecUtil.fnMakeListByRecent(array);
                        a._recordingView.setDataDrawer(CRecDraw.fnRecordingItemDrawerByRecent);
                    },
                    'channel': function () {
                        sortedList = CRecUtil.fnMakeListByChannel(array);
                        a._recordingView.setDataDrawer(CRecDraw.fnRecordingItemDrawerByChannel);
                    },
                    'dayOfWeek': function () {
                        sortedList = CRecUtil.fnMakeListByDayOfWeek(array);
                        a._recordingView.setDataDrawer(CRecDraw.fnRecordingItemDrawerDayOfWeek);
                    },
                    'storage': function () {
                        sortedList = CRecUtil.fnMakeListByStorage(array);
                        a._recordingView.setDataDrawer(CRecDraw.fnRecordingItemDrawerStorage);
                    },
                    'tobedeleted': function () {
                        sortedList = CRecUtil.fnMakeListByTobedeleted(array);
                        a._recordingView.setDataDrawer(CRecDraw.fnTobedeletedItemDrawer);
                    }
                }[type]();
                var dataProvider = new CDataProvider(sortedList);
                CRecDraw.fnUpdateBadgeCount(a, sortedList.length);
                if (a._dpRecordings) {
                    a._dpRecordings.destroy();
                    a._dpRecordings = null;
                }
                if (a._activeViewType === 'recent' && a._activeViewType !== type) {
                    if (aRecordingDataProvider.getCount() !== 0) {
                        CRecDraw.fnNoRecordingMsgOnOff(a, false);
                    }
                }
                a._dpRecordings = dataProvider;
                a._recordingView.setDataProvider(dataProvider);
                a._recordingViewContainer.setTop(0, true);
                //a._recordingViewContainer.draw();
                a._activeViewType = type;
                if (sortedList.length > 0) {
                    a._viewTypeControl.show();
                }
                else {
                    a._viewTypeControl.hide();
                }
                if (type === 'recent') {
                    var recentList = hx.svc.HISTORY.getPlaybackList();
                    if (!!recentList) {
                        if (recentList.length === 0) {
                            CRecDraw.fnNoRecordingMsgOnOff(a, true);
                            a.doNavi('l');
                            a._menuVLayout.setFocusedItem(CRecUtil.TMenuItem.ERECENT, true);
                            a._menuVLayout._$self.find('.prism-button').removeClass('selected');
                            $button = a._menuVLayout._$self.find('.' + type);
                            $button.addClass('selected');
                            a._menuVLayout.setActiveIndex(a._activeViewIndex);
                        }
                    }
                }
                else {
                    if (aRecordingDataProvider.getCount() === 0) {
                        CRecDraw.fnNoRecordingMsgOnOff(a, true);
                        a.doNavi('l');
                    }
                }
            };
            CRecordingsPage.prototype.drawContentsView = function (type) {
                var a = this;
                var recordingDataProvider = null;
                a.onRequestRecordingData(function (dataProvider) {
                    if (recordingDataProvider) {
                        recordingDataProvider.removeEventHandler(a);
                    }
                    if (dataProvider) {
                        dataProvider.removeEventHandler(a);
                    }
                    recordingDataProvider = dataProvider;
                    recordingDataProvider.addEventHandler('ItemInserted', a, function (key, item) {
                        CRecDraw.fnNoRecordingMsgOnOff(a, false);
                        var fnGroupMatch, fnCompare;
                        var funcSetter = {
                            'programme': function () {
                                fnGroupMatch = CRecUtil.fnMatchSeries;
                                fnCompare = CRecUtil.fnCompareStartTimeLessThen;
                            },
                            'recent': function () {
                                fnGroupMatch = CRecUtil.fnMatchSeries;
                                fnCompare = CRecUtil.fnCompareStartTimeLessThen;
                            },
                            'channel': function () {
                                fnGroupMatch = CRecUtil.fnMatchChannel;
                                fnCompare = CRecUtil.fnCompareStartTimeLessThen;
                            },
                            'dayOfWeek': function () {
                                fnGroupMatch = CRecUtil.fnMatchDayOfWeek;
                                fnCompare = CRecUtil.fnCompareStartTimeLessThen;
                            },
                            'storage': function () {
                                fnGroupMatch = CRecUtil.fnMatchStorage;
                                fnCompare = CRecUtil.fnCompareStartTimeLessThen;
                            },
                            'tobedeleted': function () {
                                fnGroupMatch = CRecUtil.fnMatchSeries;
                                fnCompare = CRecUtil.fnCompareStartTimeLessThen;
                            }
                        }[a._activeViewType]();
                        if (a._dpRecordingExtView) {
                            var extViewItem = a._dpRecordingExtView.getItem(0);
                            if (extViewItem) {
                                if (fnGroupMatch(extViewItem, item)) {
                                    CRecUtil.fnPropagateItemInsertedEvent(recordingDataProvider, item, a._dpRecordingExtView, CRecUtil.fnMatchSeries, CRecUtil.fnCompareStartTimeLessThen);
                                }
                            }
                        }
                        if (a._dpRecordings) {
                            CRecUtil.fnPropagateItemInsertedEvent(recordingDataProvider, item, a._dpRecordings, fnGroupMatch, fnCompare);
                            a._recordingViewContainer.draw();
                            CRecDraw.fnUpdateBadgeCount(a, a._dpRecordings.getCount());
                        }
                    });
                    recordingDataProvider.addEventHandler('ItemChanged', a, function (keys) {
                        if (a._dpRecordingExtView) {
                            CRecUtil.fnPropagateItemUpdateEvent(recordingDataProvider, keys, a._dpRecordingExtView);
                        }
                        if (a._dpRecordings) {
                            CRecUtil.fnPropagateItemUpdateEvent(recordingDataProvider, keys, a._dpRecordings);
                        }
                    });
                    var waitingQue = [];
                    recordingDataProvider.addEventHandler('RecItemRemoved', a, function (keys, items) {
                        prism.log('recordingDataProvider- RecItemRemoved');
                        var _fnProcessRemoveEvent = function () {
                            if (waitingQue.length) {
                                var QueData = waitingQue.shift();
                                var items = QueData.items;
                                if (a._dpRecordingExtView) {
                                    CRecUtil.fnPropagateRecItemRemovedEvent(recordingDataProvider, items, a._dpRecordingExtView);
                                }
                                if (a._dpRecordings) {
                                    CRecUtil.fnPropagateRecItemRemovedEvent(recordingDataProvider, items, a._dpRecordings);
                                }
                                setTimeout(function () {
                                    _fnProcessRemoveEvent();
                                }, 10);
                            }
                            else {
                                if (a._optionMenuOnGoing === 'delete') {
                                    CRecDraw.fnLoadingMsgOnOff(a, false);
                                    a._optionMenuOnGoing = undefined;
                                }
                                a.reloadContextMenuItems();
                                setTimeout(function () {
                                    if (recordingDataProvider.getCount() === 0) {
                                        if (hx.config.useInternalPvr === 0) {
                                            var i = 0, ulLayer = a.stackSize();
                                            for (i = 0; i < ulLayer; i++) {
                                                a.removeLayer();
                                            }
                                        }
                                        a._recordingView.clearDrawnItems();
                                        CRecDraw.fnNoRecordingMsgOnOff(a, true);
                                        a.doNavi('l');
                                    }
                                    CRecDraw.fnUpdateBadgeCount(a, a._dpRecordings.getCount());
                                }, 100);
                            }
                        };
                        if (waitingQue.length === 0) {
                            if (a._optionMenuOnGoing === 'delete') {
                                CRecDraw.fnLoadingMsgOnOff(a, true, tr('LOC_REC_DELETING_ID'));
                            }
                            setTimeout(function () {
                                _fnProcessRemoveEvent();
                            }, 1000);
                        }
                        waitingQue.push({
                            keys: keys,
                            items: items
                        });
                    });
                    a.setViewConfig(recordingDataProvider, type || 'programme');
                });
            };
            CRecordingsPage.prototype.doKeyBackIfNotHandled = function () {
                var a = this;
                if (a.getFocusedChild() === a._contentView) {
                    a.setFocusedChld(a._menuVLayout);
                    return true;
                }
                return false;
            };
            CRecordingsPage.prototype.drawGridRecordings = function () {
                var a = this;
                var recordingGrid = new CGridControl(null);
                recordingGrid.setDrawParam({
                    maxColCount: 4,
                    itemWidth: 210 + 10,
                    itemHeight: 118 + 10,
                    scrollByItemSize: true,
                    scrollPageSizeByPageUpDownKey: true
                });
                recordingGrid.setDataProvider(new CDataProvider([]));
                recordingGrid.setDataDrawer(function () {
                    prism.log('called recordingGrid data drawer');
                });
                recordingGrid.onItemSelected = function (param) {
                    CRecUtil.fnRecordingGrid_onItemSelected(a, recordingGrid, param);
                };
                recordingGrid.addEventHandler('FocusChanged', recordingGrid, function ($old, $new) {
                    var dpRecordings = a._dpRecordings;
                    if (dpRecordings) {
                        var newIndex = parseInt($new.attr('data'));
                        var oldIndex = parseInt($old.attr('data'));
                        if (isNaN(newIndex)) {
                            newIndex = 0;
                        }
                        if (isNaN(oldIndex)) {
                            oldIndex = 0;
                        }
                        if (oldIndex > 3 && newIndex <= 3) {
                            a._viewTypeControl.show();
                        }
                        else if (oldIndex <= 7 && newIndex > 7) {
                            a._viewTypeControl.hide();
                        }
                    }
                });
                recordingGrid.addEventHandler('ActiveFocusedGained', recordingGrid, function ($item) {
                    var dpRecordings = a._dpRecordings;
                    if (dpRecordings) {
                        var focusIndex = parseInt($item.attr('data'));
                        if (isNaN(focusIndex)) {
                            focusIndex = 0;
                        }
                        if (focusIndex <= 7) {
                            a._viewTypeControl.show();
                        }
                        else {
                            a._viewTypeControl.hide();
                        }
                    }
                });
                a.addOptionMenuTarget(recordingGrid);
                a._recordingView = recordingGrid;
                a._recordingViewContainer.setChildControl([recordingGrid]);
                a.drawContentsView(a._activeViewType);
            };
            CRecordingsPage.prototype.drawListRecordings = function () {
                var a = this;
                var recordingList = new CListControl(null);
                recordingList.setDrawParam({
                    align: 'vertical',
                    itemWidth: 870,
                    itemHeight: 56,
                    scrollByItemSize: true
                });
                recordingList.addClass('recordingListView');
                recordingList.setDataProvider(new CDataProvider([]));
                recordingList.setDataDrawer(function () {
                    prism.log('called recordingList data drawer');
                });
                recordingList.onItemSelected = function (param) {
                    CRecUtil.fnRecordingGrid_onItemSelected(a, recordingList, param);
                };
                recordingList.addEventHandler('FocusChanged', recordingList, function ($old, $new) {
                    var dpRecordings = a._dpRecordings;
                    if (dpRecordings) {
                        var newIndex = parseInt($new.attr('data'));
                        var oldIndex = parseInt($old.attr('data'));
                        if (isNaN(newIndex)) {
                            newIndex = 0;
                        }
                        if (isNaN(oldIndex)) {
                            oldIndex = 0;
                        }
                        if (oldIndex === 1 && newIndex === 0) {
                            a._viewTypeControl.show();
                        }
                        else if (oldIndex === 7 && newIndex === 8) {
                            a._viewTypeControl.hide();
                        }
                        else if (newIndex >= 8) {
                            a._viewTypeControl.hide();
                        }
                    }
                });
                recordingList.addEventHandler('ActiveFocusedGained', recordingList, function ($item) {
                    var dpRecordings = a._dpRecordings;
                    if (dpRecordings) {
                        var focusIndex = parseInt($item.attr('data'));
                        if (isNaN(focusIndex)) {
                            focusIndex = 0;
                        }
                        if (focusIndex <= 7) {
                            a._viewTypeControl.show();
                        }
                        else {
                            a._viewTypeControl.hide();
                        }
                    }
                });
                a.addOptionMenuTarget(recordingList);
                a._recordingView = recordingList;
                a._recordingViewContainer.setChildControl([recordingList]);
                a.drawContentsView(a._activeViewType);
            };
            CRecordingsPage.prototype.onRecordingFileOperation = function (param) {
                var a = this;
                var eventName = param.eventName;
                var item = param.item;
                prism.log('onRecordingFileOperation  event=' + eventName);
                switch (eventName) {
                    case 'RecordingCopying':
                        CRecDraw.fnLoadingMsgOnOff(a, true, tr('LOC_REC_COPYING_ID'));
                        a._optionMenuOnGoing = "copy";
                        break;
                    case 'RecordingMoving':
                        CRecDraw.fnLoadingMsgOnOff(a, true, tr('LOC_REC_MOVING_ID'));
                        a._optionMenuOnGoing = "move";
                        break;
                    case 'RecordingCopyComplete':
                    case 'RecordingCopyError':
                    case 'RecordingCopyStoped':
                    case 'RecordingMoveComplete':
                    case 'RecordingMoveError':
                    case 'RecordingMoveStoped':
                        CRecDraw.fnLoadingMsgOnOff(a, false);
                        a._optionMenuOnGoing = undefined;
                        a.reloadContextMenuItems();
                        break;
                    case 'RecordingCopyFailNotEnoughSpace':
                        CRecDraw.fnLoadingMsgOnOff(a, false);
                        a._optionMenuOnGoing = undefined;
                        a.reloadContextMenuItems();
                        CRecDraw.fnLoadingMsgOnOff(a, false);
                        setTimeout(function () {
                            a.showErrorMsg(hx.l('LOC_STR_MESG_900_ID'));
                        }, 500);
                        break;
                    case 'RecordingCopyFailSizeLimit4G':
                        CRecDraw.fnLoadingMsgOnOff(a, false);
                        a._optionMenuOnGoing = undefined;
                        a.reloadContextMenuItems();
                        setTimeout(function () {
                            a.showErrorMsg(hx.l('LOC_STR_MESG_1650_ID'));
                        }, 500);
                        break;
                }
            };
            CRecordingsPage.prototype.onContextMenuClose = function (param) {
                prism.log('onContextMenuClose');
                var a = this;
                if ((a._activeViewType === 'tobedeleted' && a._dpTobedeleted) || a._ScheduleProvider !== null) {
                    if ((param === undefined && a._isSelectAll !== true) || (param !== undefined && param.item.name === "Delete")) {
                        a.reloadContextMenuItems();
                        return;
                    }
                    CRecFOP.fnOptionMentHandleSelectAllCancel(a);
                    return;
                }
                if (!param || (param.item.type !== 'select_all')) {
                    if (a._isSelectAll === true) {
                        CRecFOP.fnOptionMentHandleSelectAllCancel(a);
                    }
                }
            };
            CRecordingsPage.prototype.CloseMultiCheck = function () {
                var a = this;
                var data;
                //var checkedItems: number = 0;
                if (a._ScheduleProvider !== null) {
                    return;
                }
                if (a._activeViewType === 'tobedeleted') {
                    data = a._dpTobedeleted;
                }
                else {
                    data = (a._dpRecordingExtView || a._dpRecordings);
                }
                var dataArray = data.getDataArray();
                var i, ti, items;
                a._isMultiSelect = false;
                a._isExtMultiSelect = false;
                for (i = 0, ti = dataArray.length; i < ti; i += 1) {
                    items = dataArray[i];
                    if (items._isChecked !== undefined) {
                        //checkedItems++;
                        items._isChecked = undefined;
                        data.updateItem(i, items);
                    }
                }
                if (a._activeViewType !== "tobedeleted") {
                    if (a._dpRecordings && !a._dpRecordingExtView) {
                        a._menuVLayout.removeClass('dis');
                        a._viewTypeControl.removeClass('dis');
                        a._menuVLayout.addClass('show');
                        if (a._dpRecordings.getCount()) {
                            a._viewTypeControl.addClass('show');
                            a._contentView.setChildControl([a._viewTypeControl, a._recordingViewContainer], 1);
                            a.setChildControl([a._menuVLayout, a._contentView], 1);
                        }
                        a.draw({
                            //checkedItems : checkedItems,
                            setActiveFocus: true
                        });
                        a._menuVLayout.setActiveIndex(a._activeViewIndex);
                        CRecDraw.fnUpdateBadgeCount(a, a._dpRecordings.getCount());
                    }
                }
                a.reloadContextMenuItems();
            };
            CRecordingsPage.prototype.showErrorMsg = function (text) {
                var a = this;
                var buttonDlg = prism.createButtonDlg(a);
                buttonDlg.setTextHtml(text);
                buttonDlg.setButtonInfoList([{
                    text: tr('LOC_OK_ID'),
                    fn: function () {
                    }
                }]);
                buttonDlg.open({
                    dimm: true
                });
            };
            return CRecordingsPage;
        })(__PRISM_CPAGE__.CPage);
        if (!window.prism) {
            window.prism = {};
        }
        prism.createRecordingsPage = function ($parent) {
            var $page = $('<div>', {
                'class': 'prism-page'
            });
            $parent.append($page);
            var recordingsPage = new CRecordingsPage($page);
            return recordingsPage;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
