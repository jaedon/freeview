// file name : fw/service/HMX/dservice/dl_dsrvHistory.js
/*jslint sloppy:true, nomen:true, vars:true */
//    HISTORY: ['layer_domain/service/dl_dsrvHistory.js'],
/// <reference path="../../def/sugar.d.ts" />

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("model/model");

var KHistoryMax = 150;
var KItemMax = 30;
var KOnNowMax = 9;
var KRecordingsMax = 15;
class cHistoryInfoList extends __MODEL__.cArray<any> {
    mDService;
    constructor(dService) {
        super();
        this.mDService = dService;
    }
    findItem(historyItem) {
        var i;
        for ( i = 0; i < this.length; i += 1) {
            if (this[i].isEqual(historyItem)) {
                return i;
            }
        }
        return -1;
    }
    getMaxCount(aType) {
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
    }
    isOverLimit(historyItem) {
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
    }
    findItemByTypeAndId(type, id) {
        var i, item;
        for ( i = 0; i < this.length; i += 1) {
            item = this[i];
            if (item.getType() === type && item.getId() === id) {
                return i;
            }
        }
        return -1;
    }
    removeFirstSameTypeItem(historyItem) {
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
    }
    updateHistoryItem(historyItem) {
        var index = this.findItem(historyItem);
        if (index > -1) {
            this.splice(index, 1, historyItem);
        } else {
            this.unshift(historyItem);
        }
        this.mDService.saveData('historyList', this.getSerialized());
    }
    addHistoryItem(historyItem) {
        var index = this.findItem(historyItem);
        var overLimit = this.isOverLimit(historyItem);
        if (index === -1) {
            if (overLimit) {
                this.removeFirstSameTypeItem(historyItem);
            }
            this.unshift(historyItem);
        } else {
            this.splice(index, 1);
            this.unshift(historyItem);
        }

        if (KHistoryMax < this.length) {
            this.length = KHistoryMax;
        }

        var serialized = this.getSerialized();
        this.mDService.saveData('historyList', serialized);
        if (historyItem.getType() !== __MODEL__.THistoryType.Media) {
            //this.mDService.fireEvent('HistoryItemUpdated', this.mDService.getHistoryList());
        }
        else {
            if (historyItem.getContentType() === __MODEL__.IMediaItem.TContentType.EVideo) {
                //this.mDService.fireEvent('HistoryItemUpdated', this.mDService.getHistoryList());
            }
        }
    }
    remove(from, to) {
        var rest = this.slice((to || from) + 1 || this.length);
        this.length = from < 0 ? this.length + from : from;
        return this.push.apply(this, rest);
    }
    removeHistoryItem(type, id, withoutSync?) {
        var index = this.findItemByTypeAndId(type, id);
        if (index !== -1) {
            this.splice(index, 1);
        }
        if (!withoutSync) {
            this.sync();
        }
    }
    removeHistoryList(list) {
        var a = this;
        for (var i = 0; i < list.length; i += 1) {
            var item = list[i];
            var index = a.findItemByTypeAndId(item.type, item.id);
            if (index !== -1) {
                a.splice(index, 1);
            }
        }
        a.sync();
    }
    getSerialized() {
        var serialized = [], i;
        var item;
        for ( i = 0; i < this.length; i += 1) {
            item = this[i].serialize();
            serialized.push(item);
        }
        return serialized;
    }
    getHistoryInfoListByType(type) {
        var i, ret = [];
        for ( i = 0; i < this.length; i += 1) {
            if (this[i].getType() === type) {
                ret.push(this[i]);
            }
        }
        return ret;
    }
    sync() {
        var serialized = this.getSerialized();
        this.mDService.saveData('historyList', serialized);
        //this.mDService.fireEvent('HistoryItemUpdated', this.mDService.getHistoryList());
    }
    deleteHistory(deleteChList) {
        var i, j;

        for ( i = 0; i < deleteChList.length; i += 1) {

            for ( j = 0; j < this.length; j += 1) {
                if (deleteChList[i].ccid === this[j]._id) {
                    this.splice(j, 1);
                    break;
                }
            }
        }
        this.sync();
    }
    updateHistory2(newChList) {
        var i, j, strChNo, temp;

        for ( i = 0; i < newChList.length; i += 1) {
            for ( j = 0; j < this.length; j += 1) {

                if (newChList[i].ccid === this[j]._id) {
                    strChNo = hx.il.makeDigitNumber(newChList[i].majorChannel);
                    this[j]._no = strChNo;
                    this[j]._title = newChList[i].name;
                }
            }
        }
        this.sync();
    }
}
class cDLHistory extends __KERNEL__ implements ifSVCHistory {
    mVb = this.getPluginInstance('VideoBroadcast');
    mWatchingTimer = null;
    private historyInfoList: cHistoryInfoList;
    constructor(aServiceName: string) {
        super(aServiceName);
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
        hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, () => {
            this._refreshOnNowHistory();
        });
        //this.require('APP_MGR', (dsrvAppMgr) => {
        //    dsrvAppMgr.addEventCb('WidgetCreated', this, (widgetDescriptor) => {
        //        this.fnHandleAppLaunched(widgetDescriptor);
        //    });
        //});
        this.fnRestoreFromLocalStorage();
    }
    _refreshOnNowHistory(aList?) {
        var historyItem = null;
        var headChannels = hx.svc.CH_LIST.getChannels({
            'offset' : 'first',  // from a first channel
            'count' : KOnNowMax,
            'group' : 'TV'
        });
        var upperLength = headChannels.length;
        for (var i = 0; i < upperLength; i += 1) {
            historyItem = new __MODEL__.cOnNowHistoryItem(headChannels[i], i);
            this.historyInfoList.updateHistoryItem(historyItem);
            if (aList) {
                aList.push(historyItem);
            }
        }
    }
    fnStoreToLocalStorage() {
        this.saveData('historyList', this.historyInfoList.getSerialized());
    }
    fnRestoreFromLocalStorage() {
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
                        // do not support
                        //this.historyInfoList.push((new __MODEL__.CAppHistoryItem(null)).deSerialize(item));
                        break;
                    case __MODEL__.THistoryType.Media:
                        this.historyInfoList.push((new __MODEL__.cMediaPlaybackHistoryItem(null)).deSerialize(item));
                        break;
                    default:
                        break;
                }
            }
        }
    }
    fnHandleAppLaunched(widgetDescriptor) {
        this.historyInfoList.addHistoryItem(new __MODEL__.cAppHistoryItem(widgetDescriptor));
    }
    // API
    addChannelWatched(param): void {
        var channel = param.channel;
        this.historyInfoList.addHistoryItem(new __MODEL__.cChannelHistoryItem(channel));
    }
    addOnNowChannel(param): void {
        var channel = param.channel;
        var index = param.index;
        this.historyInfoList.updateHistoryItem(new __MODEL__.cOnNowHistoryItem(channel, index));
    }
    updateOnNowChannel(param): void {
        var channel = param.channel;
        var index = param.index;
        this.historyInfoList.updateHistoryItem(new __MODEL__.cOnNowHistoryItem(channel, index));
    }
    addRecordingPlayed(param): void {
        var playbackItem = param.playbackItem;
        this.historyInfoList.addHistoryItem(new __MODEL__.cPlaybackHistoryItem(playbackItem));
    }
    addRecordingStarted(param): void {
        var recordingItem = param.recordingItem;
        this.historyInfoList.addHistoryItem(new __MODEL__.cRecordingHistoryItem(recordingItem));
    }
    addAppLaunched(param): void {
        // do not support.
        // fnHandleAppLaunched(param.widgetDescriptor);
    }
    addMediaPlayed(param): void {
        var mediaItem = param.mediaItem;
        this.historyInfoList.addHistoryItem(new __MODEL__.cMediaPlaybackHistoryItem(mediaItem));
    }
    removeHistoryItem(param): void {
        var type = param.type, id = param.id;
        this.historyInfoList.removeHistoryItem(type, id);
    }
    removeHistoryList(list) {
        this.historyInfoList.removeHistoryList(list);
    }
    getHistoryList(): any {
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
    }
    getMediaList(): any {
        var i, length, historyInfo, mediaList = [];
        length = this.historyInfoList.length;
        for (i = 0; i < length; i += 1) {
            historyInfo = this.historyInfoList[i];
            if (historyInfo instanceof __MODEL__.cMediaPlaybackHistoryItem) {
                mediaList.push(historyInfo);
            }
        }
        return mediaList.slice(0, KItemMax);
    }
    getRecordingList(): any {
        var i, length, historyInfo, recordingList = [];
        length = this.historyInfoList.length;
        for (i = 0; i < length; i += 1) {
            historyInfo = this.historyInfoList[i];
            if (historyInfo instanceof __MODEL__.cRecordingHistoryItem) {
                recordingList.push(historyInfo);
            }
        }
        return recordingList;
    }
    getPlaybackList(): any {
        var i, length, historyInfo, playbackList = [];
        length = this.historyInfoList.length;
        for (i = 0; i < length; i += 1) {
            historyInfo = this.historyInfoList[i];
            if (historyInfo instanceof __MODEL__.cPlaybackHistoryItem) {
                playbackList.push(historyInfo);
            }
        }
        return playbackList.slice(0, KItemMax);
    }
    getContinueWatchingList(): any {
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
    }
    getOnNowChannels() : any {
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
        onNowList.sort((a, b) => {
            return a.getIndex() - b.getIndex();
        });
        return onNowList.slice(0, KOnNowMax);
    }
    findMediaItem(param): any {
        var id = param.id;
        var mediaItem = null;
        var index = this.historyInfoList.findItemByTypeAndId(__MODEL__.THistoryType.Media, id);
        if (index > -1) {
            mediaItem = this.historyInfoList[index];
        }
        return mediaItem;
    }
    reconcileHistory(): void {
        this.require('RECORDINGS', (dsrvRecordings2) => {
            var i, len, playbackHistory, removed = false;
            var playbackHistoryList = this.historyInfoList.getHistoryInfoListByType(__MODEL__.THistoryType.Playback);
            for ( i = playbackHistoryList.length - 1; i >= 0; i -= 1) {
                playbackHistory = playbackHistoryList[i];
                var recordingItem = dsrvRecordings2.getRecording({
                    id : playbackHistory.getId()
                });
                if (!recordingItem) {
                    this.historyInfoList.removeHistoryItem(playbackHistory.getType(), playbackHistory.getId());
                    removed = true;
                }
            }
            if (removed) {
                this.historyInfoList.sync();
            }
        });
    }
    removeHistoryItemByStorage(): void {
        var i, len, itemHistory, removed = false;
        var mediaItemHistoryList = this.historyInfoList.getHistoryInfoListByType(__MODEL__.THistoryType.Media);
        for ( i = mediaItemHistoryList.length - 1; i >= 0; i -= 1) {
            itemHistory = mediaItemHistoryList[i];
            var bExistItem = hx.svc.MEDIA_ITEM_MANAGER.isExist({
                path : itemHistory.getId()
            });
            if (!bExistItem) {
                this.historyInfoList.removeHistoryItem(itemHistory.getType(), itemHistory.getId());
                removed = true;
            }
        }
        if (removed) {
            this.historyInfoList.sync();
        }
    }
    updateHistory2(newChList): void {
        this.historyInfoList.updateHistory2(newChList);
    }
    deleteHistory(deleteChList): void {
        this.historyInfoList.deleteHistory(deleteChList);
    }
}
export = cDLHistory;