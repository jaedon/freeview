/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDetailInfo2.js
/*jslint nomen:true, vars:true, browser:true*/
//    DETAIL_INFO: ['layer_domain/service/dl_dsrvDetailInfo2.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "model/model", "pages/util/provider"], function (require, exports, __KERNEL__, __MODEL__, __PROVIDER__) {
    var dsrvRecordings = hx.svc.RECORDINGS;
    var dsrvEpg = hx.svc.EPG;
    var isSeriesRecording = function (recordingitem, programmeDetailItem) {
        var i, j;
        if (programmeDetailItem.getGroupCRIDs() && programmeDetailItem.getGroupCRIDs().length > 0) {
            for (i = 0; i < recordingitem.getGroupCRIDs().length; i += 1) {
                for (j = 0; j < programmeDetailItem.getGroupCRIDs().length; j += 1) {
                    if (programmeDetailItem.getGroupCRIDs()[j] === recordingitem.getGroupCRIDs()[i]) {
                        return true;
                    }
                }
            }
        }
        else {
            if (recordingitem.getTitle() === programmeDetailItem.getTitle() && ((ENV.listOP.AUSDTT === ENV.op) || (ENV.listOP.UKDTT === ENV.op))) {
                if (programmeDetailItem.getGroupCRIDs() && programmeDetailItem.getGroupCRIDs().length === 0 && recordingitem.getChannelId() === programmeDetailItem.getChannelId()) {
                    if (programmeDetailItem.getGroupCRIDs().length === 0 && recordingitem.getGroupCRIDs().length === 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    };
    // todo: test.
    var fnMakeRecordingListReferenceItem = function (recordingItemList, channelId, finished) {
        var i = 0, j = 0, clause, Id = null;
        var programmeItemList, now;
        var eventIDList = [];
        for (i = 0; i < recordingItemList.length; i += 1) {
            Id = recordingItemList[i].getEventIDFromProgrammeID();
            if (Id && !recordingItemList[i].getRefProgrammeItem()) {
                eventIDList.push(Id);
            }
        }
        if (eventIDList.length <= 0) {
            finished(recordingItemList);
        }
        else {
            now = new Date().toSeconds();
            // make a clause sentence.
            clause = ' eventid IN ( ';
            for (i = 0; i < eventIDList.length; i += 1) {
                if (i === 0) {
                    clause += eventIDList[i];
                }
                else {
                    clause += ", " + eventIDList[i];
                }
            }
            clause = clause + ') AND startTime < ' + now;
            hx.log('dsrvDetailInfo2', 'fnMakeRecordingListReferenceItem clause : ' + clause);
            dsrvEpg.findProgrammes({
                channel: channelId,
                filter: "rfepg",
                // therefore rfepg is not considered.
                clause: clause,
                cb: function (result) {
                    var ProgrammeLen = 0, recordingLen = 0;
                    var programmeItem;
                    programmeItemList = result[0];
                    ProgrammeLen = programmeItemList.length;
                    recordingLen = recordingItemList.length;
                    if (recordingLen > 0 && ProgrammeLen > 0) {
                        for (i = 0; i < ProgrammeLen; i += 1) {
                            for (j = 0; j < recordingLen; j += 1) {
                                if (programmeItemList[i].programmeID && programmeItemList[i].programmeID === recordingItemList[j].getProgrammeID()) {
                                    programmeItem = new __PROVIDER__.cPdeProgrammeItem(programmeItemList[i]);
                                    //set the programmeItem as a Recording item's reference item for deeplink.
                                    recordingItemList[j].setRefProgrammeItem(programmeItem);
                                    break;
                                }
                            }
                        }
                    }
                    hx.log('dsrvDetailInfo2', 'fnMakeRecordingListReferenceItem len : ' + recordingItemList.length);
                    finished(recordingItemList);
                }
            });
        }
    };
    var fnIsSameSeries = function (aGroupCRIDs1, aGroupCRIDs2) {
        var i, j;
        var ret = false;
        if (aGroupCRIDs1 && aGroupCRIDs2) {
            if (aGroupCRIDs1.length > 0 && aGroupCRIDs2.length > 0) {
                for (i = 0; i < aGroupCRIDs1.length; i++) {
                    for (j = 0; j < aGroupCRIDs2.length; j++) {
                        if (aGroupCRIDs1[i] === aGroupCRIDs2[j]) {
                            return true;
                        }
                    }
                }
            }
        }
        return ret;
    };
    var fnMakeRecordingList = function (item, groupCRIDs, channel, finished) {
        var i, recording;
        var recordingList = dsrvRecordings.getRecordingList();
        var recordingItem, recordingItemList = [];
        var length = recordingList.length;
        for (i = 0; i < length; i += 1) {
            recording = recordingList[i];
            recordingItem = new __PROVIDER__.cPdeRecordingItem(recording);
            if (item.isSame(recordingItem)) {
                if (item instanceof __PROVIDER__.cPdeProgrammeItem) {
                    recordingItem.setRefProgrammeItem(item.getSourceItem());
                }
                recordingItemList.push(recordingItem);
            }
            else if (fnIsSameSeries(groupCRIDs, recording.getGroupCRIDs())) {
                recordingItemList.push(recordingItem);
            }
            else if (isSeriesRecording(recordingItem, item)) {
                recordingItemList.push(recordingItem);
            }
            else {
            }
        }
        //for ( i = 0; i < recordingItemList.length; i += 1) {
        //    recordingItem = recordingItemList[i];
        //    hx.log('dsrvDetailInfo2', "[" + i + "] " + recordingItem.getTitle());
        //}
        hx.log('dsrvDetailInfo2', 'fnMakeRecordingList len : ' + recordingItemList.length);
        recordingList = null;
        finished(recordingItemList);
    };
    var fnMakeScheduledList = function (item, groupCRIDs, channelId, finished) {
        var i, schedule;
        var scheduledList = dsrvRecordings.getScheduledList();
        var scheduledItem, scheduledItemList = [];
        var length = scheduledList.length;
        for (i = 0; i < length; i += 1) {
            schedule = scheduledList[i];
            scheduledItem = new __PROVIDER__.cPdeScheduledItem(schedule);
            if (scheduledItem.isOnAir()) {
                continue;
            }
            if (item.isSeries()) {
                if (schedule.isSeries() && groupCRIDs && groupCRIDs.length > 0) {
                    if (fnIsSameSeries(groupCRIDs, item.getGroupCRIDs())) {
                        scheduledItemList.push(scheduledItem);
                    }
                }
                else if (item.isSame(scheduledItem)) {
                    scheduledItemList.push(scheduledItem);
                }
                else if (isSeriesRecording(scheduledItem, item)) {
                    scheduledItemList.push(scheduledItem);
                }
                else if (!scheduledItem.isPendingSeriesSchedule()) {
                    scheduledItemList.push(scheduledItem);
                }
            }
            else {
                if (item.isSame(scheduledItem)) {
                    scheduledItemList.push(scheduledItem);
                }
            }
        }
        //for ( i = 0; i < scheduledItemList.length; i += 1) {
        //    scheduledItem = scheduledItemList[i];
        //    hx.log('dsrvDetailInfo2', "[" + i + "] " + scheduledItem.getTitle());
        //}
        hx.log('dsrvDetailInfo2', 'fnMakeScheduledList len : ' + scheduledItemList.length);
        scheduledList = null;
        finished(scheduledItemList);
    };
    var fnMakeProgrammeListAsKeyword = function (item, keyword, channelId, finished) {
        hx.log('dsrvDetailInfo2', 'fnMakeProgrammeListAsKeyword keyword > ' + keyword);
        var programmeItem;
        if (keyword && keyword !== '0') {
            var programmeItemList = [];
            var now = new Date().toSeconds();
            if (item instanceof __PROVIDER__.cPdeProgrammeItem) {
                programmeItemList.push(item);
            }
            dsrvEpg.findProgrammes({
                filter: "rfepg",
                clause: 'endTime > ' + now + ' AND ' + keyword,
                cb: function (result) {
                    var programmeList = result ? result[0] : [];
                    var length = programmeList.length;
                    hx.log('dsrvDetailInfo2', 'fnMakeProgrammeListAsKeyword length : ' + length);
                    for (var i = 0; i < length; i += 1) {
                        programmeItem = new __PROVIDER__.cPdeProgrammeItem(programmeList[i]);
                        //update MDS Object
                        if (item instanceof __PROVIDER__.cPdeProgrammeItem) {
                            if (item.getProgrammeCRID() === programmeItem.getProgrammeCRID()) {
                                if (!item.getPluginObject()) {
                                    programmeItem.setMdsData(item.getMdsData());
                                    programmeItemList.splice(0, programmeItem);
                                }
                            }
                            else {
                                programmeItemList.push(programmeItem);
                            }
                        }
                        else {
                            programmeItemList.push(programmeItem);
                        }
                    }
                    finished(programmeItemList);
                }
            });
        }
        else {
            var isTimebased = item.isTimebased ? item.isTimebased() : false;
            if (isTimebased) {
                programmeItem = new __PROVIDER__.cPdeScheduledItem(item.getSourceItem());
            }
            else {
                programmeItem = item;
            }
            finished([programmeItem]);
        }
    };
    var fnGetActiveIndex = function (list, programmeDetailItem) {
        var i, currentItem, targetStartTime, targetEndTime, targetProgrammeID;
        var isOnAir = programmeDetailItem.isOnAir();
        var isBackward = programmeDetailItem instanceof __PROVIDER__.cPdeProgrammeItem ? !programmeDetailItem.getPluginObject() : false;
        var sourceProgrammeID = programmeDetailItem.getProgrammeID();
        var sourceStartTime = programmeDetailItem.getStartTime().getTime();
        var sourceEndTime = programmeDetailItem.getEndTime().getTime();
        hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] isOnAir: ' + isOnAir);
        hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] isBackward: ' + isBackward);
        hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] type: ' + programmeDetailItem.getType());
        hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] sourceProgrammeID: ' + sourceProgrammeID);
        hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] sourceStartTime  : ' + programmeDetailItem.getStartTime() + '(' + sourceStartTime + ')');
        hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] sourceEndTime    : ' + programmeDetailItem.getEndTime() + '(' + sourceEndTime + ')');
        var activeIndex = 0;
        for (i = 0; i < list.length; i += 1) {
            currentItem = list[i];
            if (isOnAir && currentItem instanceof __PROVIDER__.cPdeRecordingItem) {
                targetStartTime = currentItem.getStartTime().getTime();
                targetEndTime = currentItem.getEndTime().getTime();
                if ((sourceStartTime <= targetStartTime && targetStartTime <= sourceEndTime) || (sourceStartTime <= targetEndTime && targetEndTime <= sourceEndTime)) {
                    activeIndex = i;
                    break;
                }
            }
            else {
                targetProgrammeID = currentItem.getProgrammeID();
                hx.log('dsrvDetailInfo2', '[fnGetActiveIndex] targetProgrammeID : ' + targetProgrammeID);
                if (isBackward) {
                    if (hx.il.getCompareID(programmeDetailItem) === hx.il.getCompareID(currentItem)) {
                        activeIndex = i;
                        break;
                    }
                }
                else {
                    if (sourceProgrammeID === targetProgrammeID) {
                        activeIndex = i;
                        break;
                    }
                }
            }
        }
        return activeIndex;
    };
    var cDLDetailInfo = (function (_super) {
        __extends(cDLDetailInfo, _super);
        function cDLDetailInfo(aServiceName) {
            _super.call(this, aServiceName);
            hx.logadd('dsrvDetailInfo2');
            hx.log('dsrvDetailInfo2', 'create');
            this.defineEvents(['RecordingItemRemoved']);
        }
        // API
        cDLDetailInfo.prototype.createProgrammeItem = function (programmeItem) {
            //TODO: need to fvp version
            return new __PROVIDER__.cPdeProgrammeItem(programmeItem);
        };
        cDLDetailInfo.prototype.createRecordingItem = function (recordingItem) {
            return new __PROVIDER__.cPdeRecordingItem(recordingItem);
        };
        cDLDetailInfo.prototype.createScheduledItem = function (scheduledItem) {
            return new __PROVIDER__.cPdeScheduledItem(scheduledItem);
        };
        cDLDetailInfo.prototype.createOnDemandItem = function (ondemandItem) {
            return new __PROVIDER__.cPdeOndemandItem(ondemandItem);
        };
        cDLDetailInfo.prototype.createProgrammeScheduledItem = function (param) {
            return new __PROVIDER__.cPdeProgrammeScheduledItem(param.programme, param.scheduledItem);
        };
        cDLDetailInfo.prototype.getProgrammeDetailList = function (param) {
            var _this = this;
            var programmeDetailItem = param.programmeDetailItem;
            var i, cb = param.cb;
            hx.log('dsrvDetailInfo2', "---------------------------------------------------");
            hx.log('dsrvDetailInfo2', '[getProgrammeDetailList]');
            hx.log('dsrvDetailInfo2', "Title: " + programmeDetailItem.getTitle());
            hx.log('dsrvDetailInfo2', "Type: " + programmeDetailItem.getType());
            hx.log('dsrvDetailInfo2', "CannelNO: " + programmeDetailItem.getChannelNo() + ", ccid: " + programmeDetailItem.getChannelId());
            hx.log('dsrvDetailInfo2', "IsSeries: " + programmeDetailItem.isSeries());
            hx.log('dsrvDetailInfo2', "Programme CRID: " + programmeDetailItem.getProgrammeCRID());
            if (programmeDetailItem.getGroupCRIDs()) {
                hx.log('dsrvDetailInfo2', "Series CRID len: " + programmeDetailItem.getGroupCRIDs().length);
                for (i = 0; i < programmeDetailItem.getGroupCRIDs().length; i += 1) {
                    hx.log('dsrvDetailInfo2', "Series CRID: " + programmeDetailItem.getGroupCRIDs()[i]);
                }
            }
            hx.log('dsrvDetailInfo2', "---------------------------------------------------");
            var recordingItemList, scheduledItemList;
            var groupCRIDs = programmeDetailItem.getGroupCRIDs();
            var channel = programmeDetailItem.getChannelInstance();
            var channelId = programmeDetailItem.getChannelId();
            var keyword = null;
            var fnMakeProgrammeItemList = function (list) {
                var i, j, recordingItem, scheduledItem, programmeItem, activeIndex;
                var programmeDetailItemList = [];
                var programmeItemList = list;
                if (programmeItemList.length > 1 && ((ENV.listOP.AUSDTT === ENV.op) || (ENV.listOP.UKDTT === ENV.op))) {
                    for (i = programmeItemList.length - 1; i >= 0; i -= 1) {
                        if (programmeItemList[i].hasGroupCRIDs() === false) {
                            if (programmeItemList[i].getChannelId() === channelId) {
                                programmeItemList[i].setDummyGroup();
                            }
                            else {
                                programmeItemList.splice(i, 1);
                            }
                        }
                    }
                }
                for (i = 0; i < recordingItemList.length; i += 1) {
                    recordingItem = recordingItemList[i];
                    for (j = programmeItemList.length - 1; j >= 0; j -= 1) {
                        programmeItem = programmeItemList[j];
                        if (programmeItem.isSame(recordingItem)) {
                            programmeItemList.splice(j, 1);
                        }
                    }
                }
                for (i = 0; i < scheduledItemList.length; i += 1) {
                    scheduledItem = scheduledItemList[i];
                    for (j = programmeItemList.length - 1; j >= 0; j -= 1) {
                        programmeItem = programmeItemList[j];
                        if (programmeItem instanceof __MODEL__.IPdeProgrammeItem) {
                            if (programmeItem.isSame(scheduledItem)) {
                                var currentItem = new __PROVIDER__.cPdeProgrammeScheduledItem(programmeItem.getSourceItem(), scheduledItem.getOipfScheduledInstance());
                                if (programmeItem.isSeries() === true && ((ENV.listOP.AUSDTT === ENV.op) || (ENV.listOP.UKDTT === ENV.op))) {
                                    currentItem.setDummyGroup();
                                }
                                programmeDetailItemList.push(currentItem);
                                programmeItemList.splice(j, 1);
                            }
                        }
                    }
                }
                programmeDetailItemList = programmeDetailItemList.concat(programmeItemList, recordingItemList);
                if (programmeDetailItemList.length > 1) {
                    programmeDetailItemList = _this.sortProgrammeDetailList(programmeDetailItemList);
                    activeIndex = fnGetActiveIndex(programmeDetailItemList, programmeDetailItem);
                }
                else {
                    activeIndex = 0;
                }
                hx.log('dsrvDetailInfo2', "*******************************************************");
                hx.log('dsrvDetailInfo2', "Finally fnGetProgrammeDetailList  Len : " + programmeDetailItemList.length + ", ActiveIndex : " + activeIndex);
                //for ( i = 0; i < programmeDetailItemList.length; i += 1) {
                //    item = programmeDetailItemList[i];
                //    hx.log('dsrvDetailInfo2', "---------------------------------------------------");
                //    hx.log('dsrvDetailInfo2', "[" + i + "]" + "Title: " + item.getTitle());
                //    hx.log('dsrvDetailInfo2', "[" + i + "]" + "Type: " + item.getType());
                //    hx.log('dsrvDetailInfo2', "[" + i + "]" + "CannelNO: " + item.getChannelNo() + ", ccid: " + item.getChannelId());
                //    hx.log('dsrvDetailInfo2', "[" + i + "]" + "IsSeries: " + item.isSeries());
                //    hx.log('dsrvDetailInfo2', "[" + i + "]" + "Programme CRID: " + item.getProgrammeCRID());
                //    if(item.getGroupCRIDs() !== undefined) {
                //    hx.log('dsrvDetailInfo2', "[" + i + "]" + "Series CRID len: " + item.getGroupCRIDs().length);
                //        for ( j = 0; j < item.getGroupCRIDs().length; j += 1) {
                //            hx.log('dsrvDetailInfo2', "[" + i + "]" + "Series CRID: " + item.getGroupCRIDs()[j]);
                //        }
                //    }
                //    hx.log('dsrvDetailInfo2', "---------------------------------------------------");
                //}
                hx.log('dsrvDetailInfo2', "*******************************************************");
                cb(programmeDetailItemList.length ? programmeDetailItemList : [programmeDetailItem], activeIndex);
                recordingItemList = null;
                scheduledItemList = null;
                programmeItemList = null;
            };
            if (programmeDetailItem instanceof __MODEL__.IPdeOndemandItem) {
                cb([programmeDetailItem], 0);
            }
            else {
                fnMakeRecordingList(programmeDetailItem, groupCRIDs, channel, function (list) {
                    recordingItemList = list;
                    fnMakeScheduledList(programmeDetailItem, groupCRIDs, channelId, function (list) {
                        scheduledItemList = list;
                        if (hx.config.useRfCRID === true) {
                            if (programmeDetailItem.getType() === "recording" && programmeDetailItem.isScheduledAsSeries() === false) {
                                fnMakeProgrammeItemList([programmeDetailItem]);
                            }
                            else {
                                keyword = programmeDetailItem.getKeywordQuery();
                                fnMakeProgrammeListAsKeyword(programmeDetailItem, keyword, channel, fnMakeProgrammeItemList);
                            }
                        }
                    });
                });
            }
        };
        cDLDetailInfo.prototype.sortProgrammeDetailList = function (list) {
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
            var fnMakeArray = function (aDb) {
                var i, length = aDb.length;
                var list = [];
                for (i = 0; i < length; i += 1) {
                    list.push(aDb[i].item);
                }
                return list;
            };
            return fnMakeArray(db().order('sort').get());
        };
        return cDLDetailInfo;
    })(__KERNEL__);
    return cDLDetailInfo;
});
