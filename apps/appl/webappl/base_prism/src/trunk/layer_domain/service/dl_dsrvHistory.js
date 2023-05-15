// file name : fw/service/HMX/dservice/dl_dsrvHistory.js
/*jslint sloppy:true, nomen:true, vars:true */
//    HISTORY: ['layer_domain/service/dl_dsrvHistory.js'],
/// <reference path="../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var KHistoryMax = 150;
    var KItemMax = 30;
    var KOnNowMax = 9;
    var KRecordingsMax = 15;
    var cHistoryInfoList = (function (_super) {
        __extends(cHistoryInfoList, _super);
        function cHistoryInfoList(dService) {
            _super.call(this);
            this.mDService = dService;
        }
        cHistoryInfoList.prototype.findItem = function (historyItem) {
            var i;
            for (i = 0; i < this.length; i += 1) {
                if (this[i].isEqual(historyItem)) {
                    return i;
                }
            }
            return -1;
        };
        cHistoryInfoList.prototype.getMaxCount = function (aType) {
            var maxCount = 0;
            switch (aType) {
                case __MODEL__.THistoryType.OnNow:
                    maxCount = KOnNowMax;
                    break;
                case __MODEL__.THistoryType.Recording:
                    maxCount = KRecordingsMax;
                    break;
                case __MODEL__.THistoryType.App:
                case __MODEL__.THistoryType.Channel:
                case __MODEL__.THistoryType.Media:
                case __MODEL__.THistoryType.Playback:
                default:
                    maxCount = KItemMax;
                    break;
            }
            return maxCount;
        };
        cHistoryInfoList.prototype.isOverLimit = function (historyItem) {
            var item, i, j = 0;
            var maxCount = this.getMaxCount(historyItem.getType());
            for (i = 0; i < this.length; i += 1) {
                item = this[i];
                if (historyItem.getType() === item.getType()) {
                    if (historyItem.getContentType) {
                        if (item.getContentType && historyItem.getContentType() === item.getContentType()) {
                            j += 1;
                        }
                    }
                    else {
                        j += 1;
                    }
                }
                if (j >= maxCount) {
                    return true;
                }
            }
            return false;
        };
        cHistoryInfoList.prototype.findItemByTypeAndId = function (type, id) {
            var i, item;
            for (i = 0; i < this.length; i += 1) {
                item = this[i];
                if (item.getType() === type && item.getId() === id) {
                    return i;
                }
            }
            return -1;
        };
        cHistoryInfoList.prototype.removeFirstSameTypeItem = function (historyItem) {
            var item, i, j = 0;
            for (i = this.length - 1; i >= 0; i -= 1) {
                item = this[i];
                if (historyItem.getType() === item.getType()) {
                    if (historyItem.getContentType) {
                        if (item.getContentType && historyItem.getContentType() === item.getContentType()) {
                            this.splice(i, 1);
                            return;
                        }
                    }
                    else {
                        this.splice(i, 1);
                        return;
                    }
                }
            }
        };
        cHistoryInfoList.prototype.updateHistoryItem = function (historyItem) {
            var index = this.findItem(historyItem);
            if (index > -1) {
                this.splice(index, 1, historyItem);
            }
            else {
                this.unshift(historyItem);
            }
            this.mDService.saveData('historyList', this.getSerialized());
        };
        cHistoryInfoList.prototype.addHistoryItem = function (historyItem) {
            var index = this.findItem(historyItem);
            var overLimit = this.isOverLimit(historyItem);
            if (index === -1) {
                if (overLimit) {
                    this.removeFirstSameTypeItem(historyItem);
                }
                this.unshift(historyItem);
            }
            else {
                this.splice(index, 1);
                this.unshift(historyItem);
            }
            if (KHistoryMax < this.length) {
                this.length = KHistoryMax;
            }
            var serialized = this.getSerialized();
            this.mDService.saveData('historyList', serialized);
            if (historyItem.getType() !== __MODEL__.THistoryType.Media) {
            }
            else {
                if (historyItem.getContentType() === __MODEL__.IMediaItem.TContentType.EVideo) {
                }
            }
        };
        cHistoryInfoList.prototype.remove = function (from, to) {
            var rest = this.slice((to || from) + 1 || this.length);
            this.length = from < 0 ? this.length + from : from;
            return this.push.apply(this, rest);
        };
        cHistoryInfoList.prototype.removeHistoryItem = function (type, id, withoutSync) {
            var index = this.findItemByTypeAndId(type, id);
            if (index !== -1) {
                this.splice(index, 1);
            }
            if (!withoutSync) {
                this.sync();
            }
        };
        cHistoryInfoList.prototype.removeHistoryList = function (list) {
            var a = this;
            for (var i = 0; i < list.length; i += 1) {
                var item = list[i];
                var index = a.findItemByTypeAndId(item.type, item.id);
                if (index !== -1) {
                    a.splice(index, 1);
                }
            }
            a.sync();
        };
        cHistoryInfoList.prototype.getSerialized = function () {
            var serialized = [], i;
            var item;
            for (i = 0; i < this.length; i += 1) {
                item = this[i].serialize();
                serialized.push(item);
            }
            return serialized;
        };
        cHistoryInfoList.prototype.getHistoryInfoListByType = function (type) {
            var i, ret = [];
            for (i = 0; i < this.length; i += 1) {
                if (this[i].getType() === type) {
                    ret.push(this[i]);
                }
            }
            return ret;
        };
        cHistoryInfoList.prototype.sync = function () {
            var serialized = this.getSerialized();
            this.mDService.saveData('historyList', serialized);
            //this.mDService.fireEvent('HistoryItemUpdated', this.mDService.getHistoryList());
        };
        cHistoryInfoList.prototype.deleteHistory = function (deleteChList) {
            var i, j;
            for (i = 0; i < deleteChList.length; i += 1) {
                for (j = 0; j < this.length; j += 1) {
                    if (deleteChList[i].ccid === this[j]._id) {
                        this.splice(j, 1);
                        break;
                    }
                }
            }
            this.sync();
        };
        cHistoryInfoList.prototype.updateHistory2 = function (newChList) {
            var i, j, strChNo, temp;
            for (i = 0; i < newChList.length; i += 1) {
                for (j = 0; j < this.length; j += 1) {
                    if (newChList[i].ccid === this[j]._id) {
                        strChNo = hx.il.makeDigitNumber(newChList[i].majorChannel);
                        this[j]._no = strChNo;
                        this[j]._title = newChList[i].name;
                    }
                }
            }
            this.sync();
        };
        return cHistoryInfoList;
    })(__MODEL__.cArray);
    var cDLHistory = (function (_super) {
        __extends(cDLHistory, _super);
        function cDLHistory(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mVb = this.getPluginInstance('VideoBroadcast');
            this.mWatchingTimer = null;
            hx.logadd('dsrvHistory');
            hx.log('dsrvHistory', 'create');
            //this.defineEvents(['HistoryItemUpdated']);
            this.historyInfoList = new cHistoryInfoList(this);
            //this.mVb.addEventListener('ChannelChangeSucceeded', (e) => {
            //    clearTimeout(this.mWatchingTimer);
            //    this.mWatchingTimer = setTimeout(() => {
            //        this.historyInfoList.addHistoryItem(new __MODEL__.cChannelHistoryItem(e.channel));
            //    }, 10 * 60 * 1000);
            //});
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, function () {
                _this._refreshOnNowHistory();
            });
            //this.require('APP_MGR', (dsrvAppMgr) => {
            //    dsrvAppMgr.addEventCb('WidgetCreated', this, (widgetDescriptor) => {
            //        this.fnHandleAppLaunched(widgetDescriptor);
            //    });
            //});
            this.fnRestoreFromLocalStorage();
        }
        cDLHistory.prototype._refreshOnNowHistory = function (aList) {
            var historyItem = null;
            var headChannels = hx.svc.CH_LIST.getChannels({
                'offset': 'first',
                'count': KOnNowMax,
                'group': 'TV'
            });
            var upperLength = headChannels.length;
            for (var i = 0; i < upperLength; i += 1) {
                historyItem = new __MODEL__.cOnNowHistoryItem(headChannels[i], i);
                this.historyInfoList.updateHistoryItem(historyItem);
                if (aList) {
                    aList.push(historyItem);
                }
            }
        };
        cDLHistory.prototype.fnStoreToLocalStorage = function () {
            this.saveData('historyList', this.historyInfoList.getSerialized());
        };
        cDLHistory.prototype.fnRestoreFromLocalStorage = function () {
            var data = this.readData('historyList');
            if (data) {
                this.historyInfoList.length = 0;
                for (var i = 0, length = data.length, item = null; i < length; i += 1) {
                    item = data[i];
                    switch (item.type) {
                        case __MODEL__.THistoryType.Channel:
                            this.historyInfoList.push((new __MODEL__.cChannelHistoryItem(null)).deSerialize(item));
                            break;
                        case __MODEL__.THistoryType.OnNow:
                            this.historyInfoList.push((new __MODEL__.cOnNowHistoryItem(null, null)).deSerialize(item));
                            break;
                        case __MODEL__.THistoryType.Playback:
                            this.historyInfoList.push((new __MODEL__.cPlaybackHistoryItem(null)).deSerialize(item));
                            break;
                        case __MODEL__.THistoryType.Recording:
                            this.historyInfoList.push((new __MODEL__.cRecordingHistoryItem(null)).deSerialize(item));
                            break;
                        case __MODEL__.THistoryType.App:
                            break;
                        case __MODEL__.THistoryType.Media:
                            this.historyInfoList.push((new __MODEL__.cMediaPlaybackHistoryItem(null)).deSerialize(item));
                            break;
                        default:
                            break;
                    }
                }
            }
        };
        cDLHistory.prototype.fnHandleAppLaunched = function (widgetDescriptor) {
            this.historyInfoList.addHistoryItem(new __MODEL__.cAppHistoryItem(widgetDescriptor));
        };
        // API
        cDLHistory.prototype.addChannelWatched = function (param) {
            var channel = param.channel;
            this.historyInfoList.addHistoryItem(new __MODEL__.cChannelHistoryItem(channel));
        };
        cDLHistory.prototype.addOnNowChannel = function (param) {
            var channel = param.channel;
            var index = param.index;
            this.historyInfoList.updateHistoryItem(new __MODEL__.cOnNowHistoryItem(channel, index));
        };
        cDLHistory.prototype.updateOnNowChannel = function (param) {
            var channel = param.channel;
            var index = param.index;
            this.historyInfoList.updateHistoryItem(new __MODEL__.cOnNowHistoryItem(channel, index));
        };
        cDLHistory.prototype.addRecordingPlayed = function (param) {
            var playbackItem = param.playbackItem;
            this.historyInfoList.addHistoryItem(new __MODEL__.cPlaybackHistoryItem(playbackItem));
        };
        cDLHistory.prototype.addRecordingStarted = function (param) {
            var recordingItem = param.recordingItem;
            this.historyInfoList.addHistoryItem(new __MODEL__.cRecordingHistoryItem(recordingItem));
        };
        cDLHistory.prototype.addAppLaunched = function (param) {
            // do not support.
            // fnHandleAppLaunched(param.widgetDescriptor);
        };
        cDLHistory.prototype.addMediaPlayed = function (param) {
            var mediaItem = param.mediaItem;
            this.historyInfoList.addHistoryItem(new __MODEL__.cMediaPlaybackHistoryItem(mediaItem));
        };
        cDLHistory.prototype.removeHistoryItem = function (param) {
            var type = param.type, id = param.id;
            this.historyInfoList.removeHistoryItem(type, id);
        };
        cDLHistory.prototype.removeHistoryList = function (list) {
            this.historyInfoList.removeHistoryList(list);
        };
        cDLHistory.prototype.getHistoryList = function () {
            var serializedList = this.historyInfoList.getSerialized();
            var i, length, historyList = [];
            length = serializedList.length;
            for (i = 0; i < length; i += 1) {
                var serialized = serializedList[i];
                if (serialized.type !== __MODEL__.THistoryType.Media) {
                    historyList.push(serialized);
                }
                else {
                    if (serialized.contentType === __MODEL__.IMediaItem.TContentType.EVideo) {
                        historyList.push(serialized);
                    }
                }
            }
            return historyList.slice(0, KItemMax);
        };
        cDLHistory.prototype.getMediaList = function () {
            var i, length, historyInfo, mediaList = [];
            length = this.historyInfoList.length;
            for (i = 0; i < length; i += 1) {
                historyInfo = this.historyInfoList[i];
                if (historyInfo instanceof __MODEL__.cMediaPlaybackHistoryItem) {
                    mediaList.push(historyInfo);
                }
            }
            return mediaList.slice(0, KItemMax);
        };
        cDLHistory.prototype.getRecordingList = function () {
            var i, length, historyInfo, recordingList = [];
            length = this.historyInfoList.length;
            for (i = 0; i < length; i += 1) {
                historyInfo = this.historyInfoList[i];
                if (historyInfo instanceof __MODEL__.cRecordingHistoryItem) {
                    recordingList.push(historyInfo);
                }
            }
            return recordingList;
        };
        cDLHistory.prototype.getPlaybackList = function () {
            var i, length, historyInfo, playbackList = [];
            length = this.historyInfoList.length;
            for (i = 0; i < length; i += 1) {
                historyInfo = this.historyInfoList[i];
                if (historyInfo instanceof __MODEL__.cPlaybackHistoryItem) {
                    playbackList.push(historyInfo);
                }
            }
            return playbackList.slice(0, KItemMax);
        };
        cDLHistory.prototype.getContinueWatchingList = function () {
            // USAGE : if a page get each plugin data from this list, use hx.svc.RECORDINGS.getRecording({'id' : ID}) API!
            var historyInfo = null, recItem = null;
            var continueWatchingList = [];
            for (var i = 0, length = this.historyInfoList.length; i < length; i += 1) {
                historyInfo = this.historyInfoList[i];
                if (historyInfo instanceof __MODEL__.cPlaybackHistoryItem) {
                    if (historyInfo.wasWatching()) {
                        continueWatchingList.push(historyInfo);
                    }
                }
            }
            return continueWatchingList;
        };
        cDLHistory.prototype.getOnNowChannels = function () {
            // USAGE : use getId() to get ccid
            var i, historyInfo, onNowList = [];
            var wholeLength = this.historyInfoList.length;
            for (i = 0; i < wholeLength; i += 1) {
                historyInfo = this.historyInfoList[i];
                if (historyInfo instanceof __MODEL__.cOnNowHistoryItem) {
                    onNowList.push(historyInfo);
                }
            }
            var nowLength = onNowList.length;
            if (nowLength < KOnNowMax) {
                onNowList = [];
                this._refreshOnNowHistory(onNowList);
            }
            onNowList.sort(function (a, b) {
                return a.getIndex() - b.getIndex();
            });
            return onNowList.slice(0, KOnNowMax);
        };
        cDLHistory.prototype.findMediaItem = function (param) {
            var id = param.id;
            var mediaItem = null;
            var index = this.historyInfoList.findItemByTypeAndId(__MODEL__.THistoryType.Media, id);
            if (index > -1) {
                mediaItem = this.historyInfoList[index];
            }
            return mediaItem;
        };
        cDLHistory.prototype.reconcileHistory = function () {
            var _this = this;
            this.require('RECORDINGS', function (dsrvRecordings2) {
                var i, len, playbackHistory, removed = false;
                var playbackHistoryList = _this.historyInfoList.getHistoryInfoListByType(__MODEL__.THistoryType.Playback);
                for (i = playbackHistoryList.length - 1; i >= 0; i -= 1) {
                    playbackHistory = playbackHistoryList[i];
                    var recordingItem = dsrvRecordings2.getRecording({
                        id: playbackHistory.getId()
                    });
                    if (!recordingItem) {
                        _this.historyInfoList.removeHistoryItem(playbackHistory.getType(), playbackHistory.getId());
                        removed = true;
                    }
                }
                if (removed) {
                    _this.historyInfoList.sync();
                }
            });
        };
        cDLHistory.prototype.removeHistoryItemByStorage = function () {
            var i, len, itemHistory, removed = false;
            var mediaItemHistoryList = this.historyInfoList.getHistoryInfoListByType(__MODEL__.THistoryType.Media);
            for (i = mediaItemHistoryList.length - 1; i >= 0; i -= 1) {
                itemHistory = mediaItemHistoryList[i];
                var bExistItem = hx.svc.MEDIA_ITEM_MANAGER.isExist({
                    path: itemHistory.getId()
                });
                if (!bExistItem) {
                    this.historyInfoList.removeHistoryItem(itemHistory.getType(), itemHistory.getId());
                    removed = true;
                }
            }
            if (removed) {
                this.historyInfoList.sync();
            }
        };
        cDLHistory.prototype.updateHistory2 = function (newChList) {
            this.historyInfoList.updateHistory2(newChList);
        };
        cDLHistory.prototype.deleteHistory = function (deleteChList) {
            this.historyInfoList.deleteHistory(deleteChList);
        };
        return cDLHistory;
    })(__KERNEL__);
    return cDLHistory;
});
