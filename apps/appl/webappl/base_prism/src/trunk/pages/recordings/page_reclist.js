var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", 'model/model'], function (require, exports, __PAGE_KERNEL__, __MODEL__) {
    // file name : pages/recordings/recList.js
    /*jslint sloppy:true, nomen:true, vars:true */
    /* CPgRecordings construction function */
    var cCPgRecordings = (function (_super) {
        __extends(cCPgRecordings, _super);
        function cCPgRecordings() {
            _super.call(this);
            this.TScheduledFactoryType = {
                ERECORDING: 0,
                EREMIND: 1,
                EHTTP_LIVE_STREAMING: 4,
                EPOWER_ON_TIMER: 6,
                EREGULAR_OTA: 7,
                ESATRECORD: 8,
                ESERIES_RECORDING: 10
            };
        }
        cCPgRecordings.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            var _fnSearchItem = function (dataProvider, eventItem) {
                var i, ti, item, key = -1;
                for (i = 0, ti = dataProvider.getCount(); i < ti; i += 1) {
                    item = dataProvider.getItem(i);
                    if (item.isEqual(eventItem)) {
                        key = i;
                        break;
                    }
                }
                return key;
            };
            var storageUpdateTimeoutId;
            var _fnUpdateStorageInfo = function () {
                if (storageUpdateTimeoutId !== undefined) {
                    clearTimeout(storageUpdateTimeoutId);
                }
                storageUpdateTimeoutId = setTimeout(function () {
                    storageUpdateTimeoutId = undefined;
                    if (recordingsPage) {
                        var storageInfo = dsrvStorageMass.getRecPVRSizeInfo();
                        recordingsPage.setStorageInfo(storageInfo.availableSize, storageInfo.totalSize);
                    }
                }, 1000);
            };
            var _fnUpdateFileOperationInfo = function () {
                var stFileOperation = hx.svc.RECORDINGS.getFileOperationInfo();
                var szOperation = 'NONE';
                if (stFileOperation) {
                    switch (stFileOperation.operation) {
                        case 'COPY':
                            szOperation = 'RecordingCopying';
                            break;
                        case 'MOVE':
                            szOperation = 'RecordingMoving';
                            break;
                    }
                    if (szOperation !== 'NONE') {
                        recordingsPage.onRecordingFileOperation({
                            eventName: szOperation,
                            item: null
                        });
                    }
                }
            };
            //hx.logadd('CPgRecordings');
            hx.log("CPgRecordings", "[CPgRecordings][create]");
            var that = this;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            var dsrvStorageMass = hx.svc.STORAGE_MASS;
            var dsrvEpg = hx.svc.EPG;
            var dsrvHistory = hx.svc.HISTORY;
            dsrvFrontPanel.setFrontPanelText('-RECORDINGS-');
            //set prismux recording page
            var recordingsPage = prism.createRecordingsPage($('#wrap'));
            var recordingProvider = new CDataProvider(dsrvRecordings.getRecordingList());
            var scheduledProvider = new CDataProvider(dsrvRecordings.getScheduledList());
            hx.il.updatePageBackground();
            recordingsPage.onRequestRecordingData = function (finished) {
                finished(recordingProvider);
            };
            recordingsPage.onRequestScheduledData = function (finished) {
                'use strict';
                var _fnUpdateItem = function (list) {
                    var scheduledItemKey = list.pop();
                    if (scheduledProvider) {
                        scheduledProvider.updateItem(scheduledItemKey);
                        if (list.length) {
                            setTimeout(function () {
                                _fnUpdateItem(list);
                            }, 100);
                        }
                    }
                };
                var _fnProcessQueryResult = function (result, scheduledKeyTbl) {
                    var i, ti, eventArray;
                    var j, tj, eventItem;
                    var thumbnailUrl, programmeID, scheduledItemKey, scheduledItem;
                    var updatedItemList = [];
                    for (i = 0, ti = result.length; i < ti; i += 1) {
                        eventArray = result[i];
                        if (eventArray && eventArray.length) {
                            for (j = 0, tj = eventArray.length; j < tj; j += 1) {
                                eventItem = eventArray[j];
                                if (eventItem) {
                                    programmeID = eventItem.programmeID;
                                    scheduledItemKey = scheduledKeyTbl[programmeID];
                                    if (scheduledItemKey !== undefined) {
                                        scheduledItem = scheduledProvider.getItem(scheduledItemKey);
                                        if (scheduledItem) {
                                            thumbnailUrl = eventItem.getThumbnailURL(true);
                                            if (thumbnailUrl && thumbnailUrl.length) {
                                                scheduledItem.setThumbnailUrl(thumbnailUrl);
                                                updatedItemList.push(scheduledItemKey);
                                            }
                                            else {
                                                scheduledItem.setThumbnailUrl(null);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    return updatedItemList;
                };
                var _fnQueryEpgFindProgrammes = function (aChannel, aClause, aScheduledKeyTbl) {
                    setTimeout(function () {
                        dsrvEpg.findProgrammes({
                            channel: aChannel,
                            clause: aClause,
                            isOnAir: false,
                            listener: that,
                            cb: function (result) {
                                if (result.length > 0) {
                                    var updatedItemList = _fnProcessQueryResult(result, aScheduledKeyTbl);
                                    if (updatedItemList.length) {
                                        _fnUpdateItem(updatedItemList);
                                    }
                                }
                                result = undefined;
                            }
                        });
                    }, 0);
                };
                var array = scheduledProvider.getDataArray();
                var channelTbl = {};
                var i, ti, scheduledItem, programmeID, channelId, tblItem;
                for (i = 0, ti = array.length; i < ti; i += 1) {
                    scheduledItem = array[i];
                    if (scheduledItem.getThumbnailUrl() === undefined) {
                        programmeID = scheduledItem.getProgrammeID();
                        if (programmeID) {
                            channelId = scheduledItem.getChannelId();
                            if (!channelTbl[channelId]) {
                                channelTbl[channelId] = {
                                    channel: scheduledItem.getChannelInstance(),
                                    clause: [],
                                    scheduledKeyTbl: {}
                                };
                            }
                            tblItem = channelTbl[channelId];
                            tblItem.clause.push('programmeID == ' + programmeID);
                            tblItem.scheduledKeyTbl[programmeID] = i;
                        }
                    }
                }
                for (channelId in channelTbl) {
                    if (channelTbl.hasOwnProperty(channelId)) {
                        tblItem = channelTbl[channelId];
                        if (tblItem && tblItem.clause.length) {
                            _fnQueryEpgFindProgrammes(tblItem.channel, tblItem.clause.join(' OR '), tblItem.scheduledKeyTbl);
                        }
                    }
                }
                if (finished) {
                    finished(scheduledProvider);
                }
            };
            recordingsPage.onRequestFileOperationInfo = function (finished) {
                finished(dsrvRecordings.getFileOperationInfo());
            };
            recordingsPage.onVerifyFileOperationLockPasswd = function (param, finished) {
                var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                    password: param.password
                });
                if (result && (result.result === 0)) {
                    finished(true);
                }
                else {
                    finished(false);
                }
            };
            recordingsPage.doRecordingItemDelete = function (finished, param) {
                dsrvRecordings.removeRecording({
                    items: param.items,
                    finished: function (result) {
                        setTimeout(function () {
                            finished();
                        }, 1000);
                    }
                });
            };
            recordingsPage.doRecentItemDelete = function (finished, param) {
                var i = 0, len = 0;
                var items, item, id;
                if (!!param) {
                    items = param.items;
                    if (!!items) {
                        len = items.length;
                    }
                }
                for (i = 0; i < len; i++) {
                    item = items[i];
                    if (!!item) {
                        if (item.getId) {
                            id = item.getId();
                            dsrvHistory.removeHistoryItem({
                                type: __MODEL__.THistoryType.Playback,
                                id: id
                            });
                        }
                    }
                }
                setTimeout(function () {
                    finished();
                }, 1000);
            };
            recordingsPage.doRecordingItemLock = function (finished, param) {
                var lock = param.lock;
                var _reqSetLockfn = function (items, lock) {
                    dsrvRecordings.setManualLocked({
                        items: items,
                        lock: lock
                    });
                    setTimeout(function () {
                        finished(true);
                    }, 1000);
                };
                if (lock) {
                    //lock
                    _reqSetLockfn(param.items, param.lock);
                }
                else {
                    //unlock
                    var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                        password: param.password
                    });
                    if (result && (result.result === 0)) {
                        _reqSetLockfn(param.items, param.lock);
                    }
                    else {
                        setTimeout(function () {
                            finished(false);
                        }, 1000);
                    }
                }
            };
            recordingsPage.doRecordingItemKeep = function (finished, param) {
                var keep = param.keep;
                var _reqSetKeepfn = function (items, keep) {
                    dsrvRecordings.setKeep({
                        items: items,
                        keep: keep
                    });
                    setTimeout(function () {
                        finished(true);
                    }, 1000);
                };
                _reqSetKeepfn(param.items, param.keep);
            };
            recordingsPage.doRecordingItemStop = function (finished, param) {
                dsrvRecordings.stopRecord({
                    items: param.items,
                    finished: function (result) {
                        hx.log("CPgRecordings", "finished doRecordingItemStop");
                    }
                });
                setTimeout(function () {
                    finished();
                }, 1000);
            };
            recordingsPage.doRecordingItemCopy = function (finished, param) {
                dsrvRecordings.copyRecording({
                    items: param.items,
                    targetStorageId: param.targetStorageId,
                    finished: function (result) {
                        setTimeout(function () {
                            finished(result);
                        }, 100);
                    }
                });
            };
            recordingsPage.doRecordingItemMove = function (finished, param) {
                dsrvRecordings.moveRecording({
                    items: param.items,
                    targetStorageId: param.targetStorageId,
                    finished: function (result) {
                        setTimeout(function () {
                            finished(result);
                        }, 100);
                    }
                });
            };
            recordingsPage.doRecordingItemStopCopying = function (finished, param) {
                dsrvRecordings.stopFileOperation({
                    items: param.items,
                    finished: function (result) {
                        setTimeout(function () {
                            finished();
                        }, 100);
                    }
                });
            };
            recordingsPage.doRecordingItemStopMoving = function (finished, param) {
                dsrvRecordings.stopFileOperation({
                    items: param.items,
                    finished: function (result) {
                        setTimeout(function () {
                            finished();
                        }, 100);
                    }
                });
            };
            recordingsPage.onRequestChannelGroupInfo = function (finished) {
                var dsrvChlist = hx.svc.CH_LIST;
                var channelListGroupId, channelListGroupIds = ['TV', 'RADIO', 'Favourite1', 'Favourite2', 'Favourite3', 'Favourite4', 'Favourite5'];
                var i, ti, channelGroup, channelList, channel;
                var oChannelGroupInfo = [], oChannelList;
                var currentChannel;
                var ccid;
                for (i = 0, ti = channelListGroupIds.length; i < ti; i += 1) {
                    channelListGroupId = channelListGroupIds[i];
                    channelGroup = dsrvChlist.getChlist({
                        group: channelListGroupId
                    });
                    channelList = channelGroup.chList;
                    if (channelList && channelList.length) {
                        oChannelList = [];
                        var j, tj;
                        for (j = 0, tj = channelList.length; j < tj; j += 1) {
                            channel = channelList[j];
                            if (channel) {
                                oChannelList.push({
                                    ccid: channel.ccid,
                                    no: channel.majorChannel,
                                    name: channel.name,
                                    thumbnail: channel.logoURL
                                });
                            }
                        }
                        oChannelGroupInfo.push({
                            groupName: channelGroup.name,
                            channelList: oChannelList
                        });
                    }
                }
                channelGroup = undefined;
                currentChannel = hx.svc.WEB_STORAGE.getLastService();
                ccid = (currentChannel && currentChannel.ccid) ? currentChannel.ccid : undefined;
                prism.log('currentChannel ccid ' + ccid);
                if (finished) {
                    finished(oChannelGroupInfo, ccid);
                }
            };
            recordingsPage.onRequestTargetStorageInfo = function (param, finished) {
                var exceptPvrHdd = param.exceptPvrHdd;
                var sourceStorageId = param.sourceStorageId;
                var storageInfo;
                var retWholeStorage = dsrvStorageMass.getWholeStorageInfo();
                var i, ti, storage, outList = [];
                var uiStorageType;
                var label;
                for (i = 0, ti = retWholeStorage.length; i < ti; i += 1) {
                    storage = retWholeStorage[i];
                    switch (storage.type) {
                        case 'internalPvrHDD':
                        case 'externalPvrHDD':
                            if (!exceptPvrHdd && (sourceStorageId !== storage.uuid)) {
                                if (hx.config.useInternalPvr === 1) {
                                    outList.push({
                                        type: (storage.type === 'internalPvrHDD') ? 'hdd' : 'dtc',
                                        name: (storage.type === 'internalPvrHDD') ? tr('LOC_INTERNALHDD_ID') : tr('LOC_DETACHHDD_ID'),
                                        uuid: storage.uuid,
                                        id: storage.id
                                    });
                                }
                                else {
                                    outList.push({
                                        type: (storage.type === 'internalPvrHDD') ? 'usbp' : 'usb',
                                        name: (storage.label && storage.label.length) ? storage.label : tr('LOC_UNKNOWN_ID'),
                                        uuid: storage.uuid,
                                        id: storage.id
                                    });
                                }
                            }
                            break;
                        default:
                            switch (storage.type) {
                                case 'externalHDD':
                                case 'usbMemory':
                                case 'unknown':
                                    uiStorageType = 'usb';
                                    break;
                                case 'sdMemory':
                                    uiStorageType = 'sdm';
                                    break;
                            }
                            if (storage.label && storage.label.length) {
                                label = storage.label;
                            }
                            else {
                                label = tr('LOC_UNKNOWN_ID');
                                if (storage.type === "sdMemory") {
                                    label = tr('LOC_SDCARD_ID');
                                }
                            }
                            outList.push({
                                type: uiStorageType,
                                name: label,
                                uuid: storage.uuid,
                                id: storage.id
                            });
                            break;
                    }
                }
                finished(outList);
            };
            recordingsPage.doScheduledItemDelete = function (finished, param) {
                hx.log("CPgRecordings", "doScheduledItemDelete [" + param.items[0].getTitle() + "]");
                dsrvRecordings.removeWholeSeriesSchedule({
                    items: param.items,
                    finished: function () {
                        setTimeout(function () {
                            finished();
                        }, 1000);
                    }
                });
            };
            recordingsPage.doScheduledItemAdd = function (finished, param) {
                var scheduleInfo = param.newScheduleInfo;
                var scheduleParam = {
                    startTime: Math.floor(scheduleInfo.startTime.getTime() / 1000),
                    duration: scheduleInfo.duration,
                    repeatDays: scheduleInfo.repeatDays,
                    ccid: scheduleInfo.ccid,
                    finished: function (result) {
                        setTimeout(function () {
                            finished();
                        }, 1000);
                    }
                };
                if (scheduleInfo.reminder) {
                    dsrvRecordings.recordAtReminder(scheduleParam);
                }
                else {
                    dsrvRecordings.recordAtRecording(scheduleParam);
                }
                /*var i, ti, startTime = Math.floor(scheduleInfo.startTime.getTime() / 1000), reqList = [];
                 for(i = 0, ti = 100;i < ti;i+=1){
                 startTime = startTime + scheduleInfo.duration + 120;
                 reqList.push({
                 startTime : startTime
                 });
                 }
    
                 var _fnReqAddSchedule = function(info){
                 dsrvRecordings.recordAt({
                 startTime : info.startTime,
                 duration : scheduleInfo.duration,
                 repeatDays : scheduleInfo.repeat,
                 ccid : scheduleInfo.ccid,
                 factoryType : (scheduleInfo.reminder) ? 'reminder' : 'recording',
                 finished : function(result) {
                 if(reqList.length){
                 setTimeout(function() {
                 _fnReqAddSchedule(reqList.shift());
                 }, 1000);
                 } else {
                 setTimeout(function() {
                 finished();
                 }, 1000);
                 }
                 }
                 });
                 };
                 _fnReqAddSchedule(reqList.shift());*/
            };
            recordingsPage.doScheduledItemEdit = function (finished, param) {
                var that = _this, item = param.item;
                var rescheduleInfo = param.rescheduleInfo;
                setTimeout(function () {
                    dsrvRecordings.updateScheduledRecording({
                        item: item //
                        ,
                        startTime: Math.floor(rescheduleInfo.startTime.getTime() / 1000) //
                        ,
                        duration: rescheduleInfo.duration //
                        ,
                        ccid: rescheduleInfo.ccid //
                        ,
                        repeatDays: rescheduleInfo.repeatDays //
                        ,
                        factoryType: (rescheduleInfo.reminder) ? that.TScheduledFactoryType.EREMIND : that.TScheduledFactoryType.ERECORDING,
                        finished: function (result) {
                            setTimeout(function () {
                                finished();
                            }, 1000);
                        }
                    });
                }, 1);
            };
            var dsrvDetailInfo = hx.svc.DETAIL_INFO;
            recordingsPage.onRecordingItemSelected = function (finished, param) {
                var dsrvFrontPanel = hx.svc.FRONT_PANEL;
                var programmeDetailRecordingItem = dsrvDetailInfo.createRecordingItem(param.item);
                var onClosed = param.onClosed;
                if (that.getCenterPageName() === 'CPgRecordings') {
                    dsrvFrontPanel.setFrontPanelText('-RECORDINGS-');
                }
                else if (that.getCenterPageName() === 'CPgLiveController') {
                    dsrvFrontPanel.clearFrontPanelText();
                }
                setTimeout(function () {
                    finished();
                    that._detailDlg = hx.il.createProgrammeDetailForSugar({
                        layeredGroupControl: recordingsPage,
                        sugarPage: that,
                        pdeItem: programmeDetailRecordingItem,
                        fnDoShow: function () {
                            recordingsPage._$self.show();
                        },
                        fnDoHide: function () {
                            recordingsPage._$self.hide();
                        },
                        onClosed: function () {
                            delete that._detailDlg;
                            if (onClosed) {
                                onClosed();
                            }
                        }
                    });
                }, 1);
            };
            recordingsPage.onScheduledItemSelected = function (finished, param) {
                var programmeDetailScheduleItem = dsrvDetailInfo.createScheduledItem(param.item);
                var onClosed = param.onClosed;
                dsrvFrontPanel.setFrontPanelText('-RECORDINGS-');
                setTimeout(function () {
                    finished();
                    that._detailDlg = hx.il.createProgrammeDetailForSugar({
                        layeredGroupControl: recordingsPage,
                        sugarPage: that,
                        pdeItem: programmeDetailScheduleItem,
                        fnDoShow: function () {
                            recordingsPage._$self.show();
                        },
                        fnDoHide: function () {
                            recordingsPage._$self.hide();
                        },
                        onClosed: function () {
                            delete that._detailDlg;
                            if (onClosed) {
                                onClosed();
                            }
                        }
                    });
                }, 1);
            };
            if (hx.config.useInternalPvr === 1) {
                recordingsPage.draw({
                    //storageMenu : ((dsrvStorageMass.getRecPVRList()).length > 0)
                    storageMenu: true
                });
            }
            else {
                recordingsPage.draw({
                    storageMenu: ((dsrvStorageMass.getRecPVRList()).length >= 1)
                });
            }
            _fnUpdateStorageInfo();
            _fnUpdateFileOperationInfo();
            dsrvRecordings.addEventCb('RecordingItemAdded', that, function (param) {
                prism.log('[CPgRecordings] RecordingItemAdded');
                recordingProvider.prependItem(param.item);
            });
            dsrvRecordings.addEventCb('RecordingListLoadingCompleted', that, function (param) {
                prism.log('[CPgRecordings] RecordingListLoadingCompleted');
                dsrvStorageMass.getRecPVRList(true);
                recordingProvider.appendItem(param.item);
                _fnUpdateStorageInfo();
            });
            dsrvRecordings.addEventCb('RecordingItemUpdated', that, function (param) {
                prism.log('[CPgRecordings] RecordingItemUpdated');
                var item = param.item;
                var key = _fnSearchItem(recordingProvider, item);
                recordingProvider.updateItem(key, item);
            });
            dsrvRecordings.addEventCb('RecordingCompleted', that, function (param) {
                prism.log('[CPgRecordings] RecordingCompleted');
                var item = param.item;
                var key = _fnSearchItem(recordingProvider, item);
                recordingProvider.updateItem(key, item);
            });
            dsrvRecordings.addEventCb('RecordingItemRemoved', that, function (param) {
                prism.log('[CPgRecordings] RecordingItemRemoved');
                setTimeout(function () {
                    var key = _fnSearchItem(recordingProvider, param.item);
                    if (key !== -1) {
                        recordingProvider.removeRecordingItem(key);
                        _fnUpdateStorageInfo();
                    }
                }, 0);
            });
            dsrvRecordings.addEventCb('ScheduledItemAdded', that, function (param) {
                prism.log('[CPgRecordings] ScheduledItemAdded');
                scheduledProvider.prependItem(param.item);
            });
            dsrvRecordings.addEventCb('ScheduledItemRemoved', that, function (param) {
                prism.log('[CPgRecordings] ScheduledItemRemoved');
                setTimeout(function () {
                    var key = _fnSearchItem(scheduledProvider, param.item);
                    if (key !== -1) {
                        scheduledProvider.removeItem(key);
                    }
                }, 0);
            });
            dsrvRecordings.addEventCb('ScheduledItemUpdated', that, function (param) {
                prism.log('[CPgRecordings] ScheduledItemUpdated');
                var item = param.item;
                var key = _fnSearchItem(scheduledProvider, item);
                scheduledProvider.updateItem(key, item);
            });
            dsrvRecordings.addEventCb('RecordingCopying', that, function (param) {
                prism.log('[CPgRecordings] RecordingCopying');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingCopying',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingCopyComplete', that, function (param) {
                prism.log('[CPgRecordings] RecordingCopyComplete');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingCopyComplete',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingCopyError', that, function (param) {
                prism.log('[CPgRecordings] RecordingCopyError');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingCopyError',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingCopyStoped', that, function (param) {
                prism.log('[CPgRecordings] RecordingCopyStoped');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingCopyStoped',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingMoving', that, function (param) {
                prism.log('[CPgRecordings] RecordingMoving');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingMoving',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingMoveComplete', that, function (param) {
                prism.log('[CPgRecordings] RecordingMoveComplete');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingMoveComplete',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingMoveError', that, function (param) {
                prism.log('[CPgRecordings] RecordingMoveError');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingMoveError',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingMoveStoped', that, function (param) {
                prism.log('[CPgRecordings] RecordingMoveStoped');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingMoveStoped',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingCopyFailNotEnoughSpace', that, function (param) {
                prism.log('[CPgRecordings] RecordingCopyFailNotEnoughSpace');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingCopyFailNotEnoughSpace',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('RecordingCopyFailSizeLimit4G', that, function (param) {
                prism.log('[CPgRecordings] RecordingCopyFailSizeLimit4G');
                recordingsPage.onRecordingFileOperation({
                    eventName: 'RecordingCopyFailSizeLimit4G',
                    item: param.item
                });
            });
            dsrvRecordings.addEventCb('NotiRecordingListRefresh', that, function (param) {
                prism.log('[CPgRecordings] NotiRecordingListRefresh');
                _this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgRecordings?refresh=true',
                    me: _this
                });
            });
            that._recordingsPage = recordingsPage;
            var fileOpState = dsrvRecordings.getFileOpState();
            if (fileOpState !== "IDLE") {
                switch (fileOpState) {
                    case "COPY":
                        prism.log('[CPgRecordings] FileOpState-RecordingCopying');
                        recordingsPage.onRecordingFileOperation({
                            eventName: 'RecordingCopying'
                        });
                        break;
                    case "MOVE":
                        prism.log('[CPgRecordings] FileOpState-RecordingMoving');
                        recordingsPage.onRecordingFileOperation({
                            eventName: 'RecordingMoving'
                        });
                        break;
                    default:
                        break;
                }
            }
            hx.al.compose(that);
            //help promotion when channel exist
            that.startHelp(that);
            //terminate all apps
            var dsrvAppMgr = hx.svc.APP_MGR;
            dsrvAppMgr.destroyHbbTvDeeplinkApplication();
        };
        cCPgRecordings.prototype.destroy = function () {
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            dsrvRecordings.clearEventCb(this);
            if (this._detailDlg) {
                this._detailDlg.close();
                delete this._detailDlg;
                this._detailDlg = undefined;
            }
            if (this._recordingsPage) {
                this._recordingsPage.destroy();
                delete this._recordingsPage;
                this._recordingsPage = undefined;
            }
            dsrvFrontPanel.clearFrontPanelText();
        };
        // EVENT HANDLER
        cCPgRecordings.prototype._on_alKey = function (aParam) {
            var bConsumed = false, k = hx.key, kc = aParam.alKey, selectedItem;
            bConsumed = this._recordingsPage.doKey(kc);
            if ((this._recordingsPage._isMultiSelect || this._recordingsPage._isExtMultiSelect) && kc === k.KEY_BACK) {
                this._recordingsPage.CloseMultiCheck();
                return true;
            }
            hx.log('CPgRecordings', '_on_alKey: keyCode=' + kc + ' consumed=' + bConsumed);
            if (!bConsumed) {
                switch (kc) {
                    case k.KEY_BACK:
                    case k.KEY_ESCAPE:
                    case k.KEY_PLAYER:
                        this._exit_to_live();
                        bConsumed = true;
                        break;
                    case k.KEY_SEARCH:
                        selectedItem = this._recordingsPage.getSelectedItem();
                        hx.svc.SMART_SEARCH.setKeyword(selectedItem[0] ? selectedItem[0].getTitle() : '');
                        this.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgSmartSearch'
                        });
                        bConsumed = true;
                        break;
                    default:
                        break;
                }
            }
            return bConsumed;
        };
        cCPgRecordings.prototype._exit_to_live = function () {
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController',
                me: this
            });
        };
        cCPgRecordings.prototype.startHelp = function (aTargetObject) {
            //help & promotion Start
            if (!hx.config.useHelpPage) {
                hx.log('debug', '[startHelp] !hx.config.useHelpPage');
                return;
            }
            var helpCmpt;
            var helpViewObj;
            var result = hx.svc.CH_LIST.getChannelCount({
                group: undefined,
                cb: undefined
            });
            if (result.chCnt !== 0) {
                helpViewObj = hx.svc.WEB_STORAGE.getHelp();
                //helpViewObj.rec = false;
                if (!helpViewObj.rec) {
                    helpCmpt = hx.al.component('HelpView', {
                        'parent': aTargetObject,
                        '_type': 'rec'
                    });
                    this.append(helpCmpt);
                    $('#wrap').append(helpCmpt.$);
                    helpCmpt.on();
                }
            }
            //help & promotion End
        };
        return cCPgRecordings;
    })(__PAGE_KERNEL__);
    hx.al.CPgRecordings = cCPgRecordings;
    return cCPgRecordings;
});
