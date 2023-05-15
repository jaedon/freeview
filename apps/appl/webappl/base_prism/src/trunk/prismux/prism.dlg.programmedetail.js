/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.programmedetail.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'model/model', "pages/util/provider", 'pages/util/controls', 'prismux/prism.dlg'], function (require, exports, __MODEL__, __PROVIDER__, __CONTROLS__, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var TRatingType = [undefined, undefined, "p", "p", "c", "c", "g", "g", "pg", "pg", "m", "m", "ma", "ma", "av", "r", "multi"];
        var TCasType = ['fta', 'na', 'ir', 'nd', 'va', 'cx', 'mg', 'cr'];
        var TOptionMenuItem = {
            EDelete: 0,
            ECopy: 1,
            EMove: 2,
            ELock: 3,
            ESelect: 4,
            EKeep: 5,
            EStopCopying: 6,
            EStopMoving: 7,
            ECancel: 8,
            EStopRec: 9
        };
        var PlayString = 'PLAY';
        var findLinkageInfo = function (item, aCallback) {
            var linkage = item.getHDLinkage();
            if (linkage.length) {
                var channel = hx.svc.CH_LIST.getChannelByCcidExceptALL({
                    ccid: linkage[0].channel
                });
                if (channel) {
                    hx.svc.EPG.findProgrammes({
                        channel: channel.channel,
                        clause: 'programmeID == ' + linkage[0].event,
                        cb: function (aResult, b, c) {
                            var programmes = aResult[0];
                            if (hx.emulMode) {
                                programmes.push(item);
                            }
                            if (programmes.length) {
                                var hdProgramme = programmes[0];
                                if (!(hdProgramme instanceof __PROVIDER__.cPdeProgrammeItem)) {
                                    hdProgramme = new __PROVIDER__.cPdeProgrammeItem(hdProgramme);
                                }
                                aCallback(hdProgramme, item);
                            }
                            else {
                                prism.log("linkaged detected but can't find HD programme");
                                aCallback(null, item);
                            }
                        }
                    });
                }
                else {
                    aCallback(null, item);
                }
            }
            else {
                aCallback(null, item);
            }
        };
        var ActionUtil = {
            // * : focused action
            // ^ : not focusable
            TBuilderType: {
                EProgramme: '^THUMBNAIL|RECORD_THIS_PROGRAMME|SET_REMINDER',
                EProgrammeDeeplink: '^THUMBNAIL|RECORD_THIS_PROGRAMME|SET_REMINDER|JUMP_TO_TV_ONDEMAND',
                EProgrammeSeries: '^THUMBNAIL|RECORD_WHOLE_SERIES|RECORD_THIS_PROGRAMME|SET_REMINDER',
                EProgrammeSeriesDeeplink: '^THUMBNAIL|RECORD_WHOLE_SERIES|RECORD_THIS_PROGRAMME|SET_REMINDER|JUMP_TO_TV_ONDEMAND',
                EProgrammeOnAir: '^THUMBNAIL|*WATCH_LIVE_TV',
                EProgrammeOnAirDeeplink: '^THUMBNAIL|*WATCH_LIVE_TV|JUMP_TO_TV_ONDEMAND',
                EProgrammePast: '^THUMBNAIL|*CLOSE',
                EProgrammePastDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND',
                EProgrammeSeriesOnAir: '^THUMBNAIL|*WATCH_LIVE_TV|RECORD_WHOLE_SERIES',
                EProgrammeSeriesOnAirDeeplink: '^THUMBNAIL|*WATCH_LIVE_TV|RECORD_WHOLE_SERIES|JUMP_TO_TV_ONDEMAND',
                EProgrammeSeriesPast: '^THUMBNAIL|*CLOSE|RECORD_WHOLE_SERIES',
                EProgrammeSeriesPastDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND|RECORD_WHOLE_SERIES',
                EProgrammeSeriesScheduled: '^THUMBNAIL|*CANCEL_SERIES_RECORD',
                EProgrammeSeriesScheduledOnAir: '^THUMBNAIL|*WATCH_LIVE_TV|CANCEL_SERIES_RECORD',
                EProgrammeSeriesScheduledOnAirDeeplink: '^THUMBNAIL|*WATCH_LIVE_TV|CANCEL_SERIES_RECORD|JUMP_TO_TV_ONDEMAND',
                EProgrammeSeriesScheduledPast: '^THUMBNAIL|*CLOSE|CANCEL_SERIES_RECORD',
                EProgrammeSeriesScheduledPastDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND|CANCEL_SERIES_RECORD',
                ERecording: '^THUMBNAIL|*PLAY|DELETE_THIS_PROGRAMME',
                ERecordingDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND|PLAY|DELETE_THIS_PROGRAMME',
                ERecordingSeriesRecorded: '^THUMBNAIL|*PLAY|DELETE_THIS_EPISODE|CANCEL_SERIES_RECORD',
                ERecordingSeriesRecordedDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND|PLAY|DELETE_THIS_EPISODE|RECORD_WHOLE_SERIES',
                ERecordingNotSeriesRecorded: '^THUMBNAIL|*PLAY|DELETE_THIS_EPISODE|RECORD_WHOLE_SERIES',
                ERecordingNotSeriesRecordedDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND|PLAY|DELETE_THIS_EPISODE|RECORD_WHOLE_SERIES',
                ERecordingOnGoing: '^THUMBNAIL|*PLAY|WATCH_LIVE_TV|STOP_RECORDING',
                ERecordingNotSeriesRecordedOnGoing: '^THUMBNAIL|PLAY|*WATCH_LIVE_TV|STOP_RECORDING',
                ERecordingNotSeriesRecordedOnGoingDeeplink: '^THUMBNAIL|JUMP_TO_TV_ONDEMAND|PLAY|*WATCH_LIVE_TV|STOP_RECORDING',
                ERecordingSeriesRecordedOnGoing: '^THUMBNAIL|PLAY|*WATCH_LIVE_TV|CANCEL_SERIES_RECORD|STOP_RECORDING',
                ERecordingKeep: '^THUMBNAIL|*PLAY|DELETE_THIS_PROGRAMME|KEEP_THIS_PROGRAMME',
                ERecordingKeepSeriesRecorded: '^THUMBNAIL|*PLAY|DELETE_THIS_EPISODE|CANCEL_SERIES_RECORD|KEEP_THIS_EPISODE',
                ERecordingKeepNotSeriesRecorded: '^THUMBNAIL|*PLAY|DELETE_THIS_EPISODE|KEEP_THIS_EPISODE',
                EScheduled: '^THUMBNAIL|*CANCEL_RECORD',
                EScheduledDeeplink: '^THUMBNAIL|*CANCEL_RECORD|JUMP_TO_TV_ONDEMAND',
                EScheduledNotSeriesRecorded: '^THUMBNAIL|RECORD_WHOLE_SERIES|*CANCEL_RECORD',
                EScheduledNotSeriesRecordedDeeplink: '^THUMBNAIL|RECORD_WHOLE_SERIES|*CANCEL_RECORD|JUMP_TO_TV_ONDEMAND',
                EScheduledSeries: '^THUMBNAIL|*CANCEL_SERIES_RECORD',
                EScheduledSeriesDeeplink: '^THUMBNAIL|*CANCEL_SERIES_RECORD|JUMP_TO_TV_ONDEMAND',
                EScheduledReminder: '^THUMBNAIL|RECORD_THIS_PROGRAMME|*CANCEL_REMIND',
                EScheduledReminderTBR: '^THUMBNAIL|*CANCEL_REMIND',
                EScheduledReminderDeeplink: '^THUMBNAIL|RECORD_THIS_PROGRAMME|*CANCEL_REMIND|JUMP_TO_TV_ONDEMAND',
                EScheduledReminderSeries: '^THUMBNAIL|RECORD_WHOLE_SERIES|RECORD_THIS_PROGRAMME|*CANCEL_REMIND',
                EScheduledReminderSeriesDeeplink: '^THUMBNAIL|RECORD_WHOLE_SERIES|RECORD_THIS_PROGRAMME|*CANCEL_REMIND|JUMP_TO_TV_ONDEMAND',
                EOndemandDeeplink: '^THUMBNAIL|*JUMP_TO_TV_ONDEMAND'
            },
            handler: {
                'THUMBNAIL': function (item) {
                    return {
                        name: 'THUMBNAIL',
                        selected: false,
                        focusable: false
                    };
                },
                'PLAY': function (item) {
                    return {
                        name: PlayString,
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestPlayback(item, detailDlg);
                        }
                    };
                },
                'WATCH_LIVE_TV': function (item) {
                    return {
                        name: "WATCH LIVE",
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestLive(item);
                        }
                    };
                },
                'WATCH_LIVE_TV_HD': function (item) {
                    return {
                        name: "WATCH LIVE IN HD",
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestLive(item);
                        }
                    };
                },
                'JUMP_TO_TV_ONDEMAND': function (item) {
                    var bSelected = true;
                    return {
                        name: "WATCH VIA ON DEMAND",
                        selected: bSelected,
                        focusable: bSelected,
                        action: function (item, detailDlg) {
                            hx.svc.AV_CTRL.blockAV({
                                deepLink: true,
                                cb: function () {
                                    hx.svc.APP_MGR.createHbbTvDeeplinkApplication({
                                        name: item.getDeeplinkAppName(),
                                        url: item.getDeeplinkUrl()
                                    });
                                    detailDlg.onRequestGoLiveController();
                                    if (detailDlg) {
                                        detailDlg.onRequestCloseDetail();
                                    }
                                }
                            });
                        }
                    };
                },
                'STOP_RECORDING': function (item) {
                    return {
                        name: tr('LOC_STOP_RECORD_BTN_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestStopRecording(item);
                        }
                    };
                },
                'RECORD_THIS_PROGRAMME': function (item) {
                    return {
                        name: tr('LOC_TIMERREC_BTN1_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            var dsrvRecordings2 = hx.svc.RECORDINGS;
                            var scheduledList = dsrvRecordings2.getScheduledList();
                            if (scheduledList.length >= 300) {
                                var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                                hx.msg('response', {
                                    text: msg,
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                findLinkageInfo(item, function (aHD, aSD) {
                                    if (aHD) {
                                        detailDlg.openHDAlternative([
                                            {
                                                text: aHD.getTitle(),
                                                fn: function () {
                                                    detailDlg.onRequestRecord(aHD);
                                                }
                                            },
                                            {
                                                text: aSD.getTitle(),
                                                fn: function () {
                                                    detailDlg.onRequestRecord(aSD);
                                                }
                                            }
                                        ]);
                                    }
                                    else {
                                        detailDlg.onRequestRecord(aSD);
                                    }
                                });
                            }
                        }
                    };
                },
                'CANCEL_RECORD': function (item) {
                    return {
                        name: tr('LOC_CANCLE_REC_BTN1_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestRemoveRecording(item);
                        }
                    };
                },
                'RECORD_WHOLE_SERIES': function (item) {
                    return {
                        name: tr('LOC_SERIESREC_BTN_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            //detailDlg.onRequestRecordWholeSeries(item);
                            var dsrvRecordings2 = hx.svc.RECORDINGS;
                            var scheduleList = dsrvRecordings2.getScheduledList();
                            if (scheduleList.length >= 300) {
                                var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                                hx.msg('response', {
                                    text: msg,
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                findLinkageInfo(item, function (aHD, aSD) {
                                    if (aHD) {
                                        if (aHD.isSeries()) {
                                            detailDlg.openHDAlternative([
                                                {
                                                    text: aHD.getTitle(),
                                                    fn: function () {
                                                        detailDlg.onRequestRecordWholeSeries(aHD);
                                                    }
                                                },
                                                {
                                                    text: aSD.getTitle(),
                                                    fn: function () {
                                                        detailDlg.onRequestRecordWholeSeries(aSD);
                                                    }
                                                }
                                            ]);
                                        }
                                        else {
                                            detailDlg.onRequestRecordWholeSeries(aSD);
                                        }
                                    }
                                    else {
                                        detailDlg.onRequestRecordWholeSeries(aSD);
                                    }
                                });
                            }
                        }
                    };
                },
                'KEEP_THIS_PROGRAMME': function (item) {
                    return {
                        name: tr('LOC_KEEP_THIS_PROGRAMME_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            var btnY, btnN, textHtml, btnDlg;
                            btnY = tr('LOC_YES_ID');
                            btnN = tr('LOC_NO_ID');
                            textHtml = tr('LOC_WANT_KEEP_THIS_PROGRAMME_ID');
                            btnDlg = prism.createButtonDlg(null, {
                                textHtml: textHtml,
                                buttonList: [btnY, btnN],
                                timeOutValue: 30000
                            }, function (index) {
                                if (index === 0) {
                                    detailDlg.onRequestKeepRecording(item);
                                }
                            });
                            btnDlg.open({});
                        }
                    };
                },
                'KEEP_THIS_EPISODE': function (item) {
                    return {
                        name: tr('LOC_KEEP_THIS_EPISODE_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            var btnY, btnN, textHtml, btnDlg;
                            btnY = tr('LOC_YES_ID');
                            btnN = tr('LOC_NO_ID');
                            textHtml = tr('LOC_WANT_KEEP_THIS_EPISODE_ID');
                            btnDlg = prism.createButtonDlg(null, {
                                textHtml: textHtml,
                                buttonList: [btnY, btnN],
                                timeOutValue: 30000
                            }, function (index) {
                                if (index === 0) {
                                    detailDlg.onRequestKeepRecording(item);
                                }
                            });
                            btnDlg.open({});
                        }
                    };
                },
                'CANCEL_SERIES_RECORD': function (item) {
                    return {
                        name: tr('LOC_CANCEL_SERIESREC_BTN_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestCancelRecordWholeSeries(item);
                        }
                    };
                },
                'DELETE_THIS_PROGRAMME': function (item) {
                    return {
                        name: "DELETE THIS RECORDING",
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            var btnY, btnN, textHtml, btnDlg;
                            btnY = tr('LOC_YES_ID');
                            btnN = tr('LOC_NO_ID');
                            textHtml = tr('LOC_MSG_DELREC_ID');
                            btnDlg = prism.createButtonDlg(null, {
                                textHtml: textHtml,
                                buttonList: [btnY, btnN],
                                timeOutValue: 30000
                            }, function (index) {
                                if (index === 0) {
                                    detailDlg.onRequestRemoveRecording(item);
                                }
                            });
                            btnDlg.open({});
                        }
                    };
                },
                'DELETE_THIS_EPISODE': function (item) {
                    return {
                        name: "DELETE THIS RECORDING",
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            var btnY, btnN, textHtml, btnDlg;
                            btnY = tr('LOC_YES_ID');
                            btnN = tr('LOC_NO_ID');
                            textHtml = tr('LOC_MSG_DELREC_ID');
                            btnDlg = prism.createButtonDlg(null, {
                                textHtml: textHtml,
                                buttonList: [btnY, btnN],
                                timeOutValue: 30000
                            }, function (index) {
                                if (index === 0) {
                                    detailDlg.onRequestRemoveRecording(item);
                                }
                            });
                            btnDlg.open({});
                        }
                    };
                },
                'SET_REMINDER': function (item) {
                    return {
                        name: tr('LOC_SET_REMINDER_01_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            var dsrvRecordings2 = hx.svc.RECORDINGS;
                            var scheduledList = dsrvRecordings2.getScheduledList();
                            var mag = '';
                            if (scheduledList.length >= 300) {
                                var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                                hx.msg('response', {
                                    text: msg,
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                findLinkageInfo(item, function (aHD, aSD) {
                                    if (aHD) {
                                        detailDlg.openHDAlternative([
                                            {
                                                text: aHD.getTitle(),
                                                fn: function () {
                                                    detailDlg.onRequestSetReminder(aHD);
                                                }
                                            },
                                            {
                                                text: aSD.getTitle(),
                                                fn: function () {
                                                    detailDlg.onRequestSetReminder(aSD);
                                                }
                                            }
                                        ]);
                                    }
                                    else {
                                        detailDlg.onRequestSetReminder(aSD);
                                    }
                                });
                            }
                        }
                    };
                },
                'CANCEL_REMIND': function (item) {
                    return {
                        name: tr('LOC_CANCLE_WATCH_BTN_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestCancelReminder(item);
                        }
                    };
                },
                'CLOSE': function (item) {
                    return {
                        name: tr('LOC_CLOSE_ID'),
                        selected: false,
                        focusable: false,
                        action: function (item, detailDlg) {
                            detailDlg.onRequestClose();
                        }
                    };
                }
            }
        };
        var fnIsSeriesSchedule = function (item) {
            var i, j, k;
            var isSeriesSchedule = false;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var schedulelist = dsrvRecordings2.getScheduledList();
            for (i = 0; i < schedulelist.length; i++) {
                if (schedulelist[i].getGroupCRIDs() && item.getGroupCRIDs()) {
                    for (j = 0; j < item.getGroupCRIDs().length; j++) {
                        for (k = 0; k < schedulelist[i].getGroupCRIDs().length; k++) {
                            if (item.getGroupCRIDs()[j] === schedulelist[i].getGroupCRIDs()[k]) {
                                isSeriesSchedule = true;
                                return isSeriesSchedule;
                            }
                        }
                    }
                }
            }
            return isSeriesSchedule;
        };
        var fnGetActionList = function (item, previousItemType) {
            var i, length, selected, focusable, handler, handlerKey, handlerKeys = [], actionList = [];
            var key = 'E';
            var itemType = previousItemType || item.getType();
            var isOnAir = item.isOnAir();
            var isPast = item.isPast();
            var programmeItem = null;
            var isSeriesRecording = false;
            var isSeriesSchedule = false;
            switch (itemType) {
                case __MODEL__.IProgrammeDetailItem.type.ondemand:
                    key += 'OndemandDeeplink';
                    break;
                case __MODEL__.IProgrammeDetailItem.type.programme:
                    key += 'Programme';
                    if (item.getPluginObject()) {
                        if (item.isSeries()) {
                            key += 'Series';
                            isSeriesSchedule = fnIsSeriesSchedule(item);
                            if (isSeriesSchedule === true) {
                                key += 'Scheduled';
                            }
                        }
                    }
                    if (isPast) {
                        key += 'Past';
                    }
                    else if (isOnAir) {
                        key += 'OnAir';
                    }
                    // for processing of backwardEPG's media deeplink
                    if (item.getAvailability() === 2 /* EAvailable */) {
                        key += 'Deeplink';
                    }
                    break;
                case __MODEL__.IProgrammeDetailItem.type.recording:
                    key += 'Recording';
                    if (ENV.op === ENV.listOP.UKDTT) {
                        if (item.parent_activeViewType === "tobedeleted") {
                            key += 'Keep';
                        }
                    }
                    if (item.isSeries() === true) {
                        if (item.isScheduledAsSeries() === false) {
                            key += 'NotSeriesRecorded';
                        }
                        else {
                            isSeriesRecording = fnIsSeriesSchedule(item);
                            if (item.isPendingSeriesSchedule() || (item.isOnAir() && isSeriesRecording === false)) {
                                isSeriesRecording = true;
                            }
                            if (isSeriesRecording === true) {
                                key += 'SeriesRecorded';
                            }
                            else {
                                key += 'NotSeriesRecorded';
                            }
                        }
                    }
                    if (item.isRecordingOnGoing()) {
                        key += 'OnGoing';
                    }
                    programmeItem = item.getRefProgrammeItem();
                    if (programmeItem) {
                        if (programmeItem.getAvailability() === 2 /* EAvailable */) {
                            key += 'Deeplink';
                        }
                    }
                    break;
                case __MODEL__.IProgrammeDetailItem.type.scheduled:
                    key += 'Scheduled';
                    if (item.isReminder()) {
                        key += 'Reminder';
                        if (item.isSeries()) {
                            key += 'Series';
                        }
                        else if (item.isTimebased()) {
                            key += 'TBR';
                        }
                    }
                    else {
                        if (item.isSeries()) {
                            if (item.isSeriesButNotRecoredAsSeries()) {
                                key += 'NotSeriesRecorded';
                            }
                            else {
                                key += 'Series';
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            var handlerText = ActionUtil.TBuilderType[key];
            if (handlerText) {
                handlerKeys = handlerText.split('|');
                length = handlerKeys.length;
                for (i = 0; i < length; i += 1) {
                    handlerKey = handlerKeys[i];
                    selected = false;
                    focusable = true;
                    if (handlerKey.indexOf('*') > -1) {
                        handlerKey = handlerKey.replace('*', '');
                        selected = true;
                    }
                    else if (handlerKey.indexOf('^') > -1) {
                        handlerKey = handlerKey.replace('^', '');
                        focusable = false;
                    }
                    switch (key) {
                        case "ERecording":
                        case "ERecordingDeeplink":
                        case "ERecordingOnGoing":
                        case "ERecordingKeep":
                            PlayString = "PLAY THIS RECORDING";
                            break;
                        case "ERecordingNotSeriesRecorded":
                        case "ERecordingNotSeriesRecordedDeeplink":
                        case "ERecordingNotSeriesRecordedOnGoing":
                        case "ERecordingSeriesRecorded":
                        case "ERecordingSeriesRecordedDeeplink":
                        case "ERecordingSeriesRecordedOnGoing":
                        case "ERecordingKeepNotSeriesRecorded":
                            PlayString = "PLAY THIS RECORDING";
                            break;
                        default:
                            PlayString = "PLAY";
                            break;
                    }
                    handler = ActionUtil.handler[handlerKey](item);
                    handler.selected = selected;
                    handler.focusable = focusable;
                    actionList.push(handler);
                }
            }
            prism.log('CProgrammeDetailDlg', '[fnGetActionList] key = ' + key);
            return actionList;
        };
        var fnGetSeasonText = function (seasonNo, episodeNo) {
            var text1 = seasonNo ? hx.util.format('{0} {1}', tr('LOC_SEASON_ID'), seasonNo) : '';
            var text2 = seasonNo && episodeNo ? ', ' : '';
            var text3 = episodeNo ? hx.util.format('{0} {1}', tr('LOC_EPISODE_ID'), episodeNo) : '';
            return text1 + text2 + text3;
        };
        var fnGetEpisodeText = function (seasonNo, episodeNo, startTime, type) {
            var text;
            if (type === __MODEL__.IProgrammeDetailItem.type.recording) {
                if (seasonNo && episodeNo) {
                    text = hx.util.format('S{0} E{1} ', seasonNo, episodeNo);
                }
                else if (episodeNo) {
                    text = hx.util.format('E{0} ', episodeNo);
                }
                else {
                    text = dateFormat(startTime, 'dd mmm');
                }
            }
            else {
                text = dateFormat(startTime, 'dd mmm');
            }
            return text;
        };
        var fnGetTimeText = function (startTime, endTime) {
            if (startTime && endTime) {
                return hx.util.format('{0} ~ {1} {2} {3} {4}', startTime.format('HH:MM'), endTime.format('HH:MM'), startTime.format('ddd'), startTime.format('dd'), startTime.format('mmm')).toUpperCase();
            }
            return '';
        };
        var fnGetDurationText = function (duration) {
            if (duration) {
                var durationMins = Math.ceil(duration / 60);
                return hx.util.format('<strong style=\"color:white;\">Duration:</strong> {0} mins', durationMins);
            }
            return '';
        };
        var fnGetAvailableEndTime = function (availableEndTime) {
            if (availableEndTime) {
                var endDate = new Date(availableEndTime * 1000);
                return hx.util.format('<strong style=\"color:white;\">Available until:</strong> {0} {1}', endDate.format('HH:MM'), endDate.format('ddd dd mmm yyyy'));
            }
            return '';
        };
        var fnMakeIcon = function (item) {
            var i, length, html, parentalRating, casIDs, ratingClass, casClass, isCas = false, subtitleTypes;
            var dvb = false, ebu = false;
            function makeHtml(klass) {
                var ret = "";
                if (klass) {
                    ret = "<span class=\"" + klass + "\"></span>";
                }
                return ret;
            }
            // Recording Watch
            html = makeHtml(item.hasRecording() ? 'ico recwatch' : '');
            // HD
            if (item.isHD && item.isHD()) {
                html += makeHtml('ico hd');
            }
            // Encrypted
            if (item.isEncrypted && item.isEncrypted()) {
                html += makeHtml('ico PD_ENC');
            }
            // Guidance
            //var guidance : string = item.getGuidance();
            if (item.getHDLinkage && item.getHDLinkage()) {
                if (item.getHDLinkage().length) {
                    html += makeHtml("ico PD_HD");
                }
            }
            if (item.hasGuidance && item.hasGuidance()) {
                html += makeHtml('ico guidance');
            }
            //Recommandation
            if (item.getRecommendationCRID && item.getRecommendationCRID()) {
                html += makeHtml('ico recommendation');
            }
            //Split
            if (item.getProgrammeCRID() && (item.getProgrammeCRID().search("#") > 0)) {
                html += makeHtml('ico split');
            }
            // AD
            if (item.isAD && item.isAD()) {
                html += makeHtml('ico AD');
            }
            // Parental Rating
            if (item.getParentalRating) {
                parentalRating = item.getParentalRating();
                ratingClass = TRatingType[parentalRating > 15 ? 15 : parentalRating];
                if (ratingClass) {
                    html += makeHtml('ico rat aus_' + ratingClass);
                }
            }
            if (item.isScrambled && item.isScrambled()) {
                html += makeHtml('ico cas multi');
            }
            // DRM
            html += makeHtml(item.isDRM ? (item.isDRM() ? 'ico drm' : '') : '');
            // Lock
            html += makeHtml(item.isLocked ? (item.isLocked() ? 'ico lock' : '') : '');
            // teletext
            html += makeHtml(item.hasTeletext ? (item.hasTeletext() ? 'ico ttx' : '') : '');
            // Subtitle
            subtitleTypes = item.getSubtitleTypes ? item.getSubtitleTypes() : null;
            if (subtitleTypes) {
                for (i = 0; i < subtitleTypes.length; i += 1) {
                    if (subtitleTypes[i] === "DVB" && dvb === false) {
                        html += makeHtml('ico pdsubDVB');
                        dvb = true;
                    }
                    else if (subtitleTypes[i] === "EBU" && ebu === false) {
                        html += makeHtml('ico pdsubEBU');
                        ebu = true;
                    }
                }
            }
            // Multi Audio
            html += makeHtml(item.isMultiAudio ? (item.isMultiAudio() ? 'ico ma' : '') : '');
            return html;
        };
        var fnMakeDescription = function (item) {
            var shortDescription = item.getDescription();
            var longDescription = item.getLongDescription();
            var description = "";
            var bHasShort = false, bHasLong = false;
            if ((shortDescription && shortDescription !== "")) {
                bHasShort = true;
            }
            if ((longDescription && longDescription !== "")) {
                bHasLong = true;
            }
            if (bHasLong === true) {
                description = longDescription;
            }
            else if (bHasShort === true) {
                description = shortDescription;
            }
            return description;
        };
        var fnOptionMenuHandleClick = function (a, item) {
            var $menuList = $('.programmeDetailDlg .contextMenu');
            if (!$menuList.length) {
                return;
            }
            //var $menuList = a._contextMenuDlg.contextMenu().$contextMenu();
            var $itemList = $menuList.children().children();
            var controls = a._contextMenuDlg.contextMenu()._controls;
            var itemNo;
            if (controls.length > 0) {
                itemNo = controls[0]._selectedIndex * 2;
            }
            var $selectItem = $($itemList[itemNo]);
            if ($selectItem.hasClass('checked')) {
                $selectItem.removeClass('checked');
            }
            else {
                $selectItem.addClass('checked');
            }
        };
        var fnOptionMenuHandleSelect = function (a) {
            var i;
            var episodeList = a._ctrlEpisodeList._dataProvider;
            var episodeText;
            var curEpisodeItemIndex = a._ctrlEpisodeList.getFocusedItem().itemKey;
            var menuItemIndex = 0;
            a.menuStep = 1;
            a._activeOptionMenuInfo.menuItems = [];
            var menuItems = [];
            var episodeItem;
            for (i = 0; i < episodeList.getCount(); i++) {
                episodeItem = episodeList.getItem(i);
                if (episodeItem.getType() === "recording") {
                    menuItems.push({
                        item: episodeItem,
                        //name : fnGetEpisodeText(episodeItem.getSeasonNo(), episodeItem.getEpisodeNo(), episodeItem.getStartTime(), episodeItem.getType())
                        name: dateFormat(episodeItem.getStartTime(), 'dd mmm')
                    });
                    if (curEpisodeItemIndex === i) {
                        menuItemIndex = menuItems.length - 1;
                    }
                }
            }
            a._comboDlg = prism.createListComboDlg(a._parent, {
                textHtml: "Select episodes and then a button to edit.",
                listItems: menuItems,
                buttonList: [tr('LOC_DELETE_ID').toUpperCase(), tr('LOC_COPY_ID').toUpperCase(), tr('LOC_MOVE_ID').toUpperCase()],
            }, function (index, param) {
                var checkList = param, moveItems = [];
                var fn = function (idx, items) {
                    switch (idx) {
                        case TOptionMenuItem.EDelete:
                            fnDeleteMsg(a, items);
                            break;
                        case TOptionMenuItem.ECopy:
                            fnCopyMsg(a, items);
                            break;
                        case TOptionMenuItem.EMove:
                            fnMoveMsg(a, items);
                            break;
                        default:
                            break;
                    }
                };
                if (checkList) {
                    for (i = 0; i < param.length; i++) {
                        if (checkList[i]) {
                            moveItems.push(menuItems[i].item);
                        }
                    }
                    fn(index, moveItems);
                }
            });
            a._contextMenuDlg.close();
            a._comboDlg.open({
                dimm: true,
                onClosed: function (param) {
                    delete a._comboDlg;
                }
            });
            a._comboDlg.doKeyEscape = function () {
                return a._comboDlg.doKeyBack();
            };
        };
        var fnOptionSubMenuHandle = function (a) {
            a._activeOptionMenuInfo.menuItems = [];
            a._deleteNum = [];
            var $menuList = $('.programmeDetailDlg .contextMenu');
            if (!$menuList.length) {
                return;
            }
            //var $menuList = a._contextMenuDlg.contextMenu().$contextMenu();
            var $itemList = $menuList.children().children();
            var i, selectedItems = [];
            for (i = 0; i < $itemList.length; i += 1) {
                var $targetItem = $($itemList[i * 2]);
                if ($targetItem.hasClass('checked')) {
                    selectedItems.push(a._contextMenuDlg.contextMenu().getFocusedChild()._dataProvider.getItem(i).data);
                    a._deleteNum.push(i);
                }
            }
            a._checkedItem = selectedItems;
            a.$optionMenuInd().text(tr('LOC_EDIT_ID'));
            a._contextMenuDlg.contextMenu().$optionMenuInd().text(tr('LOC_EDIT_ID'));
            $menuList.removeClass('checkItem');
            if (!!a._ctrlEpisodeList.getFocusedItem()) {
                if (!!a._ctrlEpisodeList.getFocusedItem().item) {
                    a._activeOptionMenuInfo.menuItems[0] = {
                        name: tr('LOC_DELETE_ID'),
                        fn: function () {
                            fnDeleteMsg(a);
                        },
                        noclose: true,
                        redraw: false
                    };
                    a._activeOptionMenuInfo.menuItems[1] = {
                        name: tr('LOC_COPY_ID'),
                        fn: function () {
                            fnCopyMsg(a);
                        },
                        noclose: true,
                        redraw: false
                    };
                    a._activeOptionMenuInfo.menuItems[2] = {
                        name: tr('LOC_MOVE_ID'),
                        fn: function () {
                            fnMoveMsg(a);
                        },
                        noclose: true,
                        redraw: false
                    };
                }
            }
            var dataProvider = new __CONTROLS__.CDataProvider(a._activeOptionMenuInfo.menuItems);
            var listControl = a._contextMenuDlg.contextMenu().getFocusedChild();
            listControl.setDataProvider(dataProvider);
            listControl.redraw();
            listControl.setFocusedItem(0, true);
        };
        var fnOptionMenuHandleDelete = function (a, item, text, fn) {
            var btnY, btnN, textHtml, btnDlg, callback;
            btnY = tr('LOC_YES_ID');
            btnN = tr('LOC_NO_ID');
            textHtml = text || tr('LOC_MSG_DELREC_ID');
            callback = fn || function (index) {
                if (index === 0) {
                    if (!item) {
                        hx.log('error', '[fnOptionMenuHandleDelete] !item');
                        return;
                    }
                    var episodeList = a._ctrlEpisodeList._dataProvider;
                    var i;
                    var currentItemStartTime = item._prismRecordingItem._property.recordingStartTime;
                    a._deleteNum = [];
                    a._multiDelete = true;
                    for (i = 0; i < episodeList._count; i += 1) {
                        var episodeItem = episodeList._data[i];
                        var episodeItemStartTime = episodeItem._prismRecordingItem._property.recordingStartTime;
                        if (episodeItemStartTime === currentItemStartTime) {
                            a._deleteNum.push(i);
                            a.onRequestRemoveRecording(episodeItem);
                            break;
                        }
                    }
                    a._contextMenuDlg.close();
                }
            };
            btnDlg = prism.createButtonDlg(null, {
                textHtml: textHtml,
                buttonList: [btnY, btnN]
            }, function (index) {
                if (callback) {
                    callback(index);
                }
            });
            btnDlg.open({});
        };
        var fnOptionMenuHandleKeep = function (a, item) {
            function fn(_status) {
                /*if (_status === 'key_power' || _status === hx.l('LOC_NO_ID')) {
    
                 } else*/
                if (_status === hx.l('LOC_YES_ID')) {
                    var episodeList = a._ctrlEpisodeList._dataProvider;
                    var i;
                    var currentItemStartTime = item._prismRecordingItem._property.recordingStartTime;
                    a._keepNum = [];
                    a._multiKeep = true;
                    for (i = 0; i < episodeList._count; i += 1) {
                        var episodeItem = episodeList._data[i];
                        var episodeItemStartTime = episodeItem._prismRecordingItem._property.recordingStartTime;
                        if (episodeItemStartTime === currentItemStartTime) {
                            a._keepNum.push(i);
                            a.onRequestKeepRecording(episodeItem);
                            break;
                        }
                    }
                    a._contextMenuDlg.close();
                }
                /*else if (_status === 'key_back' || _status === 'key_esc') {
    
                 }*/
            }
            a._confirm = hx.msg('confirm', {
                text: hx.l('LOC_WANT_KEEP_THIS_PROGRAMME_ID'),
                auto_close: false,
                //timeout : 5000,
                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                dia_class: 'osd',
                callback_fn: fn
            });
        };
        var fnDeleteMsg = function (a, items) {
            var i;
            var hasChecked = false, lockFlag = false, text = '';
            var $menuList = $('.programmeDetailDlg .contextMenu');
            if (!items && !$menuList.length) {
                return;
            }
            var deleteItem = items ? items : a._checkedItem;
            var deleteItemCount = deleteItem.length;
            function fn(index) {
                if (index === 0) {
                    a._multiDelete = true;
                    for (i = 0; i < deleteItemCount; i += 1) {
                        a._delete = true;
                        a.onRequestRemoveRecording(deleteItem.pop(), true);
                    }
                    if (a._contextMenuDlg) {
                        a._contextMenuDlg.close();
                    }
                }
                if (a._comboDlg) {
                    a._comboDlg.close();
                }
            }
            if (deleteItemCount === 0) {
                text = hx.l('LOC_SELECT_ONE_ITEM_ID');
            }
            else if (deleteItemCount > 1) {
                text = hx.l('LOC_DELETE_THESE_PROGRAMMES_ID');
            }
            else {
                text = hx.l('LOC_DELETE_THIS_PROGRAMME_ID');
            }
            for (i = 0; i < deleteItemCount; i += 1) {
                if (deleteItem[i].isLocked()) {
                    lockFlag = true;
                }
            }
            if (lockFlag) {
                var passwordDlg = prism.createQueryPasswordDlg(a._parent);
                passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                passwordDlg.open({
                    dimm: true,
                    onPasswordEntered: function (password) {
                        var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                            password: password
                        });
                        if (result && (result.result === 0)) {
                            passwordDlg.close();
                            fnOptionMenuHandleDelete(a, null, text, fn);
                        }
                        else {
                            passwordDlg.reset();
                        }
                    }
                });
            }
            else {
                fnOptionMenuHandleDelete(a, null, text, fn);
            }
        };
        var fnCopyMsg = function (a, items) {
            if (a._contextMenuDlg) {
                a._contextMenuDlg.close();
            }
            fnOptionMenuHandleCopy(a, items ? items : a._checkedItem);
        };
        var fnMoveMsg = function (a, items) {
            if (a._contextMenuDlg) {
                a._contextMenuDlg.close();
            }
            fnOptionMenuHandleMove(a, items ? items : a._checkedItem);
        };
        var fnRecordingItemCopy = function (a, finished, param) {
            hx.svc.RECORDINGS.copyRecording({
                items: param.items,
                targetStorageId: param.targetStorageId,
                finished: function (result) {
                    if (finished) {
                        finished(result);
                    }
                    if (result) {
                        if (a._comboDlg) {
                            a._comboDlg.close();
                        }
                    }
                }
            });
        };
        var fnRecordingItemMove = function (a, finished, param) {
            hx.svc.RECORDINGS.moveRecording({
                items: param.items,
                targetStorageId: param.targetStorageId,
                finished: function (result) {
                    if (finished) {
                        finished(result);
                    }
                    if (result) {
                        if (a._comboDlg) {
                            a._comboDlg.close();
                        }
                    }
                }
            });
        };
        var fnRequestTargetStorageInfo = function (param, finished) {
            var exceptPvrHdd = param.exceptPvrHdd;
            var sourceStorageId = param.sourceStorageId;
            var storageInfo;
            var retWholeStorage = hx.svc.STORAGE_MASS.getWholeStorageInfo();
            var i, ti, storage, outList = [];
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
                        var uiStorageType;
                        var label;
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
        var fnListHasLockedItem = function (list) {
            var hasLock = false;
            for (var i = 0, len = list.length; i < len; i += 1) {
                if (list[i].isLocked()) {
                    hasLock = true;
                }
            }
            return hasLock;
        };
        var fnListHasNoCopyItem = function (list) {
            var hasNoCopyItem = false;
            for (var i = 0, len = list.length; i < len; i += 1) {
                if (list[i].isNoCopy()) {
                    hasNoCopyItem = true;
                }
            }
            return hasNoCopyItem;
        };
        var fnVerifyFileOperationLockPasswd = function (param, finished) {
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
        var fnOptionFileHandleOperation = function (a, items, OnGoning, processRequest, NoStroageNoti, Protect, Protect2) {
            var itemList = [];
            if (items.length == undefined) {
                itemList.push(items._prismRecordingItem);
            }
            else {
                for (var i = 0; i < items.length; i += 1) {
                    itemList.push(items[i]._prismRecordingItem);
                }
            }
            var _fnRequset = function (opItems, targetId) {
                a._optionMenuOnGoing = OnGoning;
                if (OnGoning == "copy") {
                    fnRecordingItemCopy(a, null, {
                        'items': opItems,
                        'targetStorageId': targetId
                    });
                }
                else if (OnGoning == "move") {
                    fnRecordingItemMove(a, null, {
                        'items': opItems,
                        'targetStorageId': targetId
                    });
                }
                //Always close contextMenu
                setTimeout(function () {
                    if (a._contextMenuDlg) {
                        a._contextMenuDlg.close();
                        a.menuStep = 0;
                    }
                }, 10);
            };
            var param, exceptPvrHdd;
            if (a._parent._activeViewType === 'storage') {
                exceptPvrHdd = false;
            }
            else {
                exceptPvrHdd = true;
            }
            param = {
                exceptPvrHdd: exceptPvrHdd,
                sourceStorageId: (itemList.length > 0) ? itemList[0].getStorageId() : itemList
            };
            fnRequestTargetStorageInfo(param, function (storageList) {
                if (!storageList.length) {
                    //a.doKeyOption();
                    //a.CloseMultiCheck();
                    prism.createNotificationDlg(a._parent, NoStroageNoti).open();
                    return;
                }
                var selectionDlg = prism.createStorageSelectionDlg(a._parent);
                selectionDlg.setSelectionItems(storageList);
                selectionDlg.setSelectionIndex(0);
                selectionDlg.onSelectionChanged = function (param) {
                    selectionDlg.close();
                    // a.doKeyOption();
                    //a.CloseMultiCheck();
                    var selectedStorageItem = param.item;
                    var decryptionCopy;
                    switch (selectedStorageItem.type) {
                        case 'hdd':
                        case 'dtc':
                            decryptionCopy = false;
                            break;
                        default:
                            decryptionCopy = true;
                            break;
                    }
                    if (!decryptionCopy) {
                        _fnRequset(itemList.slice(0), selectedStorageItem.id);
                    }
                    else {
                        //lock flag setting
                        var hasLockedItem = fnListHasLockedItem(itemList);
                        var multiFileCopy = (itemList.length > 1);
                        if (hasLockedItem) {
                            var passwordDlg = prism.createQueryPasswordDlg(a._parent);
                            if (multiFileCopy) {
                                passwordDlg.setTextHtml('<span>' + tr('LOC_REC_SOME_PROTECTED_BY_LOCK') + '</span>' + '<br><span>' + tr('LOC_ENTER_PASSWORD_ID') + '</span>');
                            }
                            else {
                                passwordDlg.setTextHtml('<span>' + tr('LOC_REC_PROTECTED_BY_LOCK') + '</span>' + '<br><span>' + tr('LOC_ENTER_PASSWORD_ID') + '</span>');
                            }
                            passwordDlg.open({
                                dimm: true,
                                onPasswordEntered: function (password) {
                                    fnVerifyFileOperationLockPasswd({
                                        password: password
                                    }, function (response) {
                                        if (response) {
                                            passwordDlg.close();
                                            if (!fnListHasNoCopyItem(itemList)) {
                                                _fnRequset(itemList, selectedStorageItem.id);
                                            }
                                            else {
                                                if (!multiFileCopy) {
                                                    var notificationDlg = prism.createNotificationDlg(a._parent);
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
                                                            _fnRequset(itemList, selectedStorageItem.id);
                                                        }
                                                    });
                                                    btnDlg.open({
                                                        onClosed: function () {
                                                            delete a._selectionDlg;
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
                                    delete a._selectionDlg;
                                }
                            });
                        }
                        else {
                            if (!fnListHasNoCopyItem(itemList)) {
                                _fnRequset(itemList, selectedStorageItem.id);
                            }
                            else {
                                if (!multiFileCopy) {
                                    var notificationDlg = prism.createNotificationDlg(a._parent);
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
                                            _fnRequset(itemList, selectedStorageItem.id);
                                        }
                                    });
                                    btnDlg.open({
                                        onClosed: function () {
                                            delete a._selectionDlg;
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
                        delete a._selectionDlg;
                    }
                });
            });
        };
        var fnOptionMenuHandleCopy = function (a, item) {
            fnOptionFileHandleOperation(a, item, 'copy', 'fnRecordingItemCopy', tr('LOC_REC_NOSTORAGE_COPYTO_ID'), tr('LOC_REC_CNCOPY_BECOZ_PROTECT_ID'), tr('LOC_REC_CNCOPY_SOMEREC_BECOZ_PROTECT_ID'));
        };
        var fnOptionMentHandleStopCopy = function (a, item) {
            var items = [];
            items.push(item);
            hx.svc.RECORDINGS.stopFileOperation({
                items: items,
                finished: function (result) {
                    setTimeout(function () {
                        a._optionMenuOnGoing = undefined;
                        a._contextMenuDlg.close();
                        a.menuStep = 0;
                    }, 100);
                }
            });
        };
        var fnOptionMentHandleStopMove = function (a, item) {
            var items = [];
            items.push(item);
            hx.svc.RECORDINGS.stopFileOperation({
                items: items,
                finished: function (result) {
                    setTimeout(function () {
                        a._optionMenuOnGoing = undefined;
                        a._contextMenuDlg.close();
                        a.menuStep = 0;
                    }, 100);
                }
            });
        };
        var fnOptionMenuHandleMove = function (a, item) {
            fnOptionFileHandleOperation(a, item, 'move', 'fnRecordingItemMove', tr('LOC_REC_NOSTORAGE_MOVETO_ID'), tr('LOC_REC_CNCOPY_BECOZ_PROTECT_ID'), tr('LOC_REC_CNCOPY_SOMEREC_BECOZ_PROTECT_ID'));
        };
        var fnOptionMenuHandleLock = function (a, item) {
            var lock = false;
            var fnCreateQueryPasswordDlg = function () {
                var passwordDlg = prism.createQueryPasswordDlg(a._parent);
                passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                return passwordDlg;
            };
            if (item.isLocked()) {
                lock = false;
                var passwordDlg = fnCreateQueryPasswordDlg();
                passwordDlg.open({
                    dimm: true,
                    onPasswordEntered: function (password) {
                        var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                            password: password
                        });
                        if (result && (result.result === 0)) {
                            passwordDlg.close();
                            item.setEpisodeLock(lock);
                            a._actionListView.update(item, a);
                            a._descriptionView.update(item);
                            a._contextMenuDlg.close();
                        }
                        else {
                            passwordDlg.reset();
                        }
                    }
                });
            }
            else {
                lock = true;
                item.setEpisodeLock(lock);
                a._actionListView.update(item, a);
                a._descriptionView.update(item);
                a._contextMenuDlg.close();
            }
        };
        var fnOptionMenuHandleStopRecording = function (a, item) {
            function fn(index) {
                if (index === 0) {
                    a.onRequestStopRecording(item);
                    a._contextMenuDlg.close();
                }
            }
            fnOptionMenuHandleDelete(a, null, null, fn);
        };
        var fnOptionMenuProvider = function (a, activeIndex) {
            var ret;
            //check copy&move status
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
            }
            if (!activeIndex.$item) {
                activeIndex.$item = a._ctrlEpisodeList.getFocusedItem().$item;
            }
            if (activeIndex.control) {
                if (!activeIndex.control._dataProvider || !a._ctrlEpisodeList) {
                    return ret;
                }
                if (!activeIndex.control._dataProvider._data[a._ctrlEpisodeList.getFocusedItem().itemKey].hasRecording()) {
                    return ret;
                }
            }
            else {
                return ret;
            }
            if (activeIndex) {
                var control = activeIndex.control;
                if (control === null) {
                    return ret;
                }
                var focusedItemInfo = control.getFocusedItem();
                var item = null;
                if (focusedItemInfo) {
                    item = focusedItemInfo.item;
                }
                var menuItemList = [];
                var menuItems = [];
                var fnMenuItemCreator = function (name, fn, noclose, redraw) {
                    return {
                        name: name,
                        fn: function () {
                            fn(a, item);
                        },
                        noclose: noclose,
                        redraw: redraw
                    };
                };
                if (szOperation !== 'NONE') {
                    if (szOperation == 'RecordingCopying') {
                        menuItemList[TOptionMenuItem.EStopCopying] = fnMenuItemCreator(tr('LOC_REC_STOP_COPYING_ID'), fnOptionMentHandleStopCopy, true, true);
                        if (item._prismRecordingItem.isLocked()) {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_UNLOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        else {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_LOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        menuItems.push(menuItemList[TOptionMenuItem.EStopCopying]);
                        menuItems.push(menuItemList[TOptionMenuItem.ELock]);
                    }
                    else if (szOperation == 'RecordingMoving') {
                        menuItemList[TOptionMenuItem.EStopMoving] = fnMenuItemCreator(tr('LOC_REC_STOP_MOVING_ID'), fnOptionMentHandleStopMove, true, true);
                        if (item._prismRecordingItem.isLocked()) {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_UNLOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        else {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_LOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        menuItems.push(menuItemList[TOptionMenuItem.EStopMoving]);
                        menuItems.push(menuItemList[TOptionMenuItem.ELock]);
                    }
                }
                else {
                    if (item._prismRecordingItem.isRecordingOnGoing()) {
                        menuItemList[TOptionMenuItem.EStopRec] = fnMenuItemCreator(tr('LOC_STOP_REC_BTN_ID'), fnOptionMenuHandleStopRecording, true, true);
                        if (item._prismRecordingItem.isLocked()) {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_UNLOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        else {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_LOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        menuItems.push(menuItemList[TOptionMenuItem.EStopRec]);
                        menuItems.push(menuItemList[TOptionMenuItem.ELock]);
                    }
                    else {
                        menuItemList[TOptionMenuItem.EDelete] = fnMenuItemCreator(tr('LOC_DELETE_ID'), fnOptionMenuHandleDelete, true, true);
                        menuItemList[TOptionMenuItem.ECopy] = fnMenuItemCreator(tr('LOC_COPY_ID'), fnOptionMenuHandleCopy, true, true);
                        menuItemList[TOptionMenuItem.EMove] = fnMenuItemCreator(tr('LOC_MOVE_ID'), fnOptionMenuHandleMove, true, true);
                        if (item._prismRecordingItem.isLocked()) {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_UNLOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        else {
                            menuItemList[TOptionMenuItem.ELock] = fnMenuItemCreator(tr('LOC_LOCK_ID'), fnOptionMenuHandleLock, true, true);
                        }
                        menuItemList[TOptionMenuItem.ESelect] = fnMenuItemCreator(tr('LOC_SELECT_ID'), fnOptionMenuHandleSelect, true, true);
                        menuItems.push(menuItemList[TOptionMenuItem.EDelete]);
                        menuItems.push(menuItemList[TOptionMenuItem.ECopy]);
                        menuItems.push(menuItemList[TOptionMenuItem.EMove]);
                        menuItems.push(menuItemList[TOptionMenuItem.ELock]);
                        menuItems.push(menuItemList[TOptionMenuItem.ESelect]);
                    }
                }
                if (a._parent) {
                    if (a._parent._activeViewType === "tobedeleted") {
                        menuItemList[TOptionMenuItem.EKeep] = fnMenuItemCreator(tr('LOC_KEEP_ID'), fnOptionMenuHandleKeep, true, true);
                        menuItems.push(menuItemList[TOptionMenuItem.EKeep]);
                    }
                }
            }
            ret = {
                text: tr('LOC_EDIT_ID'),
                menuItems: menuItems
            };
            return ret;
        };
        var CEpisodeListView = (function (_super) {
            __extends(CEpisodeListView, _super);
            function CEpisodeListView(dataProvider, activeIndex) {
                _super.call(this, null);
                this.addClass('menu icoRec');
                var a = this;
                a._ctrlEpisodeList = new __CONTROLS__.CListControl($('<div>', {
                    'class': 'wrap_menu'
                }));
                a._ctrlEpisodeList.setDataProvider(dataProvider);
                a._ctrlEpisodeList.setDataDrawer(function (param) {
                    /* Live : .live, history : .his */
                    /* Icon information
                     Play Programme : .play, Recording Programme : .rec,
                     reserved Series Recording : .seri, Reminder Programme : .remi, reserved Recording Programme : .rec2 */
                    var episodeIndex = param.index;
                    var episodeItem = param.item;
                    var $episodeItem = param.$item;
                    if (episodeItem instanceof __PROVIDER__.cPdeOndemandItem) {
                        return; // do not display anything yet (Series & Episodes are not prepared)
                    }
                    //$episodeItem.html(fnGetEpisodeText(episodeItem.getSeasonNo(), episodeItem.getEpisodeNo(), episodeItem.getStartTime(), episodeItem.getType()));
                    $episodeItem.html(dateFormat(episodeItem.getStartTime(), 'dd mmm'));
                    if (a._currentIndex > -1) {
                        if (a._ctrlEpisodeList._selectedIndex === episodeIndex) {
                            $episodeItem.addClass('his');
                        }
                    }
                    switch (episodeItem.getType()) {
                        case __MODEL__.IProgrammeDetailItem.type.programme:
                            break;
                        case __MODEL__.IProgrammeDetailItem.type.recording:
                            break;
                        case __MODEL__.IProgrammeDetailItem.type.scheduled:
                            break;
                        default:
                            break;
                    }
                    if (episodeItem.isOnAir()) {
                        $episodeItem.addClass('live');
                    }
                    if (episodeItem.isRecordingOnGoing()) {
                        $episodeItem.append($('<span>', {
                            'class': 'rec'
                        }));
                    }
                    else if (episodeItem.hasRecording()) {
                        if (episodeItem.wasWatching()) {
                            $episodeItem.append($('<span>', {
                                'class': 'partially'
                            }));
                        }
                        else {
                            $episodeItem.append($('<span>', {
                                'class': 'play'
                            }));
                        }
                    }
                    else if (episodeItem.isReminder()) {
                        $episodeItem.append($('<span>', {
                            'class': 'remi'
                        }));
                    }
                    else if (episodeItem.hasSchedule()) {
                        if (episodeItem.hasSeriesSchedule()) {
                            $episodeItem.append($('<span>', {
                                'class': 'seri'
                            }));
                        }
                        else {
                            $episodeItem.append($('<span>', {
                                'class': 'rec2'
                            }));
                        }
                    }
                });
                a._ctrlEpisodeList.setDrawParam({
                    itemHeight: 60,
                    scrollByItemSize: true,
                    doNotScrollFirstMaxCount: 8
                });
                a._ctrlEpisodeList.setFocusedItemByKey(activeIndex);
                a.setDrawParam({});
                a.setChildControl([a._ctrlEpisodeList]);
            }
            CEpisodeListView.prototype.getListControl = function () {
                return this._ctrlEpisodeList;
            };
            return CEpisodeListView;
        })(__CONTROLS__.CViewGroupControl);
        var CActionListView = (function (_super) {
            __extends(CActionListView, _super);
            function CActionListView(item) {
                _super.call(this, null);
                this.addClass('ep');
                this.setDrawParam({
                    setFocusRolling: true
                });
                this._item = item;
                this.update(item);
            }
            CActionListView.prototype.update = function (item, reason) {
                var a = this;
                var i, itemDrawers = [];
                if (!item) {
                    return;
                }
                a.setData(item);
                var strThumbClass = '';
                var isNoThumbnail = true;
                var isOnAir = item.isOnAir();
                var isPast = item.isPast();
                var isNew = item.isNew();
                var itemType = item.getType();
                var bIsRadio = false;
                var bRecorded = false;
                var bOndemand = false;
                if (reason) {
                    //Thumbnail , Program , ChannelLog
                    if (!a._lastItemType) {
                        a._lastItemType = itemType;
                        a._actionList = fnGetActionList(item);
                    }
                    else {
                        itemType = a._lastItemType;
                        a._actionList = fnGetActionList(item, itemType);
                    }
                }
                else {
                    a._lastItemType = itemType;
                    a._actionList = fnGetActionList(item);
                }
                var length = a._actionList.length;
                switch (itemType) {
                    case __MODEL__.IProgrammeDetailItem.type.programme:
                        if (item._item.channel.channelType !== 0) {
                            bIsRadio = true;
                        }
                        break;
                    case __MODEL__.IProgrammeDetailItem.type.recording:
                        bRecorded = true;
                        break;
                    case __MODEL__.IProgrammeDetailItem.type.scheduled:
                        if (item._oipfScheduledObj && (item._oipfScheduledObj.channel.channelType !== 0)) {
                            bIsRadio = true;
                        }
                        break;
                    case __MODEL__.IProgrammeDetailItem.type.ondemand:
                        bOndemand = true;
                        break;
                    default:
                        break;
                }
                var itemDrawer = function (index, $item, that) {
                    var action = that._actionList[index];
                    var name = action.name;
                    if (index === 0) {
                        var actionItem = that.getData();
                        var thumbnailUrl = actionItem ? (actionItem.getThumbnailUrl(function (receivedUrl) {
                            if (receivedUrl) {
                                var $img = $item.find('img');
                                if ($img && $img.length) {
                                    $img[0].src = receivedUrl;
                                }
                            }
                        }) || '') : '';
                        if (hx.il.checkMdsUrl(thumbnailUrl)) {
                            $item.html(hx.util.format('<figure tabindex="0" class="vod "><img src="{0}" alt=""></figure>', thumbnailUrl));
                        }
                        else if (hx.il.checkHttpUrl(thumbnailUrl)) {
                            hx.log('debug', 'Thumbnail URL is not prepared yet (does not have width)');
                            $item.html(hx.util.format('<figure tabindex="0" class="vod "><img src="{0}" alt=""></figure>', ''));
                        }
                        else {
                            $item.html(hx.util.format('<figure tabindex="0" class="vod "><img src="{0}" alt=""></figure>', thumbnailUrl || ''));
                        }
                    }
                    else {
                        $item.addClass('-list-item');
                        $item.css('top', (index - 1) * 60 + 225 + 15 + 'px');
                        $item.html('<div><span>' + name + '</span></div>');
                    }
                    if (!action.focusable) {
                        $item.removeClass('-f');
                    }
                    return action.selected;
                };
                for (i = 0; i < length; i += 1) {
                    itemDrawers.push(itemDrawer);
                }
                a.setItemDrawers(itemDrawers);
                if (bIsRadio) {
                    a._$self.removeClass('onAir recPlay noThumRadio new past');
                }
                else {
                    a._$self.removeClass('onAir recPlay noThum new past');
                }
                if (bOndemand) {
                    a._$self.addClass('noThumOndemand');
                }
                else {
                    if (bRecorded) {
                        strThumbClass = 'noThumRecord';
                    }
                    else if (bIsRadio) {
                        strThumbClass = 'noThumRadio';
                    }
                    else {
                        strThumbClass = 'noThum';
                    }
                    if ($('#pde2 .ep .vod img').attr('src')) {
                        a._$self.addClass(hx.util.format('{0} {1} ', isOnAir ? 'onAir' : (isNew ? 'new' : ''), isNoThumbnail ? strThumbClass : ''));
                    }
                    else {
                        a._$self.addClass(hx.util.format('{0} {1} {2}', isOnAir ? 'onAir' : (isNew ? 'new' : ''), strThumbClass, isPast ? 'past' : ''));
                    }
                }
                a.draw();
            };
            CActionListView.prototype.getCurrentActionList = function () {
                return this._actionList;
            };
            CActionListView.prototype.getData = function () {
                return this._item;
            };
            CActionListView.prototype.setData = function (item) {
                if (item) {
                    this._item = item;
                }
            };
            CActionListView.prototype.destroy = function () {
                this._actionList = null;
                hx.log('error', 'CActionListView destroy');
                _super.prototype.destroy.call(this);
            };
            return CActionListView;
        })(__CONTROLS__.CVerticalLayoutControl);
        var CTitleView = (function (_super) {
            __extends(CTitleView, _super);
            function CTitleView(item) {
                _super.call(this, null);
                var a = this;
                a.addClass('wrap_title');
                a.setItemDrawers([
                    function (index, $item) {
                        if (a._currentItem) {
                            var item = a._currentItem;
                            var channelNo = hx.il.makeDigitNumber(item.getChannelNo()) || '';
                            var logoUrl = item.getLogoUrl(function (receivedUrl) {
                                if (receivedUrl) {
                                    var $figure = $item.find('figure');
                                    if ($figure && $figure.length) {
                                        $figure.html(hx.util.format('<img src="{0}">', receivedUrl));
                                    }
                                }
                            }) || '';
                            var title = item.getTitle();
                            var secondary = item.getSecondary ? item.getSecondary() : '';
                            $item.removeClass('-f');
                            $item.css('width', '1280px');
                            if (item instanceof __PROVIDER__.cPdeOndemandItem) {
                                if (logoUrl) {
                                    $item.html(hx.util.format('<span class="chArea"><figure class="logo"><img src="{0}"></figure></span>', logoUrl));
                                }
                                else {
                                    $item.html('<span class="chArea"><figure class="logo"></figure></span>');
                                }
                            }
                            else {
                                if (logoUrl) {
                                    $item.html(hx.util.format('<span class="chArea">{0}<figure class="logo"><img src="{1}"></figure></span>', channelNo, logoUrl));
                                }
                                else {
                                    $item.html(hx.util.format('<span class="chArea">{0}<figure class="logo"></figure></span>', channelNo));
                                }
                            }
                            $item.append($('<h1>').addClass('tit').html(title));
                            $item.append($('<div>')).append($('<div>').addClass('secondary').html(secondary));
                            $item.find('.secondary').css('left', '240px');
                        }
                        return false;
                    }
                ]);
                a.update(item);
            }
            CTitleView.prototype.update = function (item) {
                var a = this;
                a._currentItem = item;
                a.draw();
            };
            CTitleView.prototype.destroy = function () {
                this._currentItem = null;
                hx.log('error', 'CTitleView destroy');
                _super.prototype.destroy.call(this);
            };
            return CTitleView;
        })(__CONTROLS__.CVerticalLayoutControl);
        var CDescriptionView = (function (_super) {
            __extends(CDescriptionView, _super);
            function CDescriptionView(item) {
                _super.call(this, null);
                var a = this;
                a.addClass('cont');
                var $desp = $('<div>', {
                    'class': 'desp'
                });
                var ctrlView = new __CONTROLS__.CViewControl($desp);
                var ctrlVertical = new __CONTROLS__.CVerticalLayoutControl(null);
                ctrlVertical.setItemDrawers([
                    function (index, $item) {
                        if (a._currentItem) {
                            var item = a._currentItem;
                            var startTime = item.getStartTime();
                            var endTime = item.getEndTime();
                            var channelNo = hx.il.makeDigitNumber(item.getChannelNo());
                            var channelName = item.getChannelName();
                            var genres = item.getGenreList();
                            var timeText = fnGetTimeText(startTime, endTime);
                            var iconList = "";
                            try {
                                iconList = fnMakeIcon(item);
                            }
                            catch (e) {
                                hx.log('error', '[prismdlg.programmedetail][fnMakeIcon] exception!!!');
                            }
                            var durationText = item.getDuration ? fnGetDurationText(item.getDuration()) : '';
                            var availableText = item.getAvailableEndTime ? fnGetAvailableEndTime(item.getAvailableEndTime()) : '';
                            var genreHtml = null;
                            var iconListHtml = null;
                            var offsetFixel = 0;
                            var $genre = null;
                            if (genres && genres.length > 0) {
                                genreHtml = '<li><span class="genre">' + genres[0] + '</span></li>';
                                offsetFixel += 34;
                            }
                            if (iconList && iconList.length > 0) {
                                iconListHtml = '<li>' + iconList + '</li>';
                                offsetFixel += 38;
                            }
                            $item.css('width', '558px');
                            if (item instanceof __PROVIDER__.cPdeOndemandItem) {
                                $item.html(hx.util.format('<ul><li style=\"font-family:PrismFontL;\">{0}</li><li style=\"font-family:PrismFontL;\">{1}</li>{2} {3}</ul>', durationText, availableText, (!!genreHtml) ? genreHtml : "", (!!iconListHtml) ? iconListHtml : ""));
                            }
                            else {
                                $item.html(hx.util.format('<ul><li>{0} {1}</li><li>{2}</li>{3} {4}</ul>', channelNo, channelName, timeText, (!!genreHtml) ? genreHtml : "", (!!iconListHtml) ? iconListHtml : ""));
                            }
                            $desp.css('top', 55 + offsetFixel + 'px');
                            if (genres && genres.length > 0) {
                                $genre = $item.find('.genre');
                                $genre.css('width', 500 + 'px');
                            }
                        }
                    }
                ]);
                ctrlView.setDrawParam({
                    scrollClass: 'prism-scroll',
                    scrollUnit: 35 * 3
                });
                ctrlView.doDrawItem = function () {
                    var $item = $('<p>');
                    if (a._currentItem) {
                        var i;
                        var item = a._currentItem;
                        var channel, programmeCRID, SeriesCRID, seriesid;
                        var description = fnMakeDescription(item);
                        var guidance = null;
                        try {
                            guidance = item.getGuidance();
                        }
                        catch (e) {
                            hx.log('error', '[prism.dlg.programmedetail.js][getGuidance] exception!!!');
                        }
                        if (guidance) {
                            guidance = guidance.text || "";
                        }
                        if (ENV.op === ENV.listOP.UKDTT && !!guidance) {
                            $item.html("<li style='color:#ff0000'>" + guidance + "</li>");
                            $item.append(description);
                        }
                        else {
                            $item.html(description);
                        }
                        if (hx.config.showSeriesInfo) {
                            programmeCRID = a._currentItem.getProgrammeCRID();
                            SeriesCRID = a._currentItem.getGroupCRIDs();
                            channel = a._currentItem.getChannelInstance();
                            seriesid = a._currentItem.getSeriesId();
                            if (seriesid === "" || seriesid === undefined) {
                                seriesid = "null";
                            }
                            if (a._currentItem.getType() === "programme") {
                                $item.append("<li style='color:#FF0000'>EPG Type: " + a._currentItem._item.type + "</li>");
                            }
                            $item.append("<li style='color:#FF0000'>Series ID: " + seriesid + "</li>");
                            $item.append("<li style='color:#FF0000'>Default authority: " + channel.defaultAuth + "</li>");
                            $item.append("<li style='color:#FF0000'>Programme CRID: " + programmeCRID + "</li>");
                            for (i = 0; i < SeriesCRID.length; i += 1) {
                                $item.append("<li style='color:#FF0000'>Series CRID: " + SeriesCRID[i] + "</li>");
                            }
                        }
                    }
                    return $item;
                };
                a.setDrawParam({});
                a.setChildControl([ctrlVertical, ctrlView], 1);
                a.view = ctrlView;
                a.update(item);
            }
            CDescriptionView.prototype.update = function (item) {
                var a = this;
                a._currentItem = item;
                a.draw();
            };
            CDescriptionView.prototype.destroy = function () {
                this._currentItem = null;
                hx.log('error', 'CDescriptionView destroy');
                _super.prototype.destroy.call(this);
            };
            return CDescriptionView;
        })(__CONTROLS__.CVerticalGroupControl);
        var CProgrammeDetailDlg = (function (_super) {
            __extends(CProgrammeDetailDlg, _super);
            function CProgrammeDetailDlg(layeredGroupControl, isHideBack, destroyParam) {
                _super.call(this, layeredGroupControl);
                this._cancelWholeSeries = false;
                this._isHideBack = isHideBack;
                this._destroyParam = destroyParam;
                this._activeIndex = 0;
                // Global Variables
            }
            CProgrammeDetailDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a._$pde = $('<section>', {
                    'id': 'pde2'
                });
                a._$self.append(a._$pde);
                a.addClass('programmeDetailDlg');
                a._$wrap = $('#wrap');
                a.setOptionMenuProvider(function (activeFocusInfo) {
                    return fnOptionMenuProvider(a, activeFocusInfo);
                });
            };
            CProgrammeDetailDlg.prototype.setProgrammeDetailList = function (programmeDetailProvider, activeIndex) {
                var a = this;
                a._programmeDetailProvider = programmeDetailProvider;
                a._activeIndex = activeIndex || 0;
            };
            CProgrammeDetailDlg.prototype.getActiveIndex = function () {
                return this._activeIndex;
            };
            CProgrammeDetailDlg.prototype.setActiveIndex = function (activeIndex) {
                this._activeIndex = activeIndex;
            };
            CProgrammeDetailDlg.prototype.getProgrammeDetailProvider = function () {
                return this._programmeDetailProvider;
            };
            CProgrammeDetailDlg.prototype.setProgrammeDetailProvider = function (programmeDetailList) {
                this._programmeDetailList = programmeDetailList;
            };
            CProgrammeDetailDlg.prototype.doOpen = function (param) {
                var a = this;
                var programmeDetailProvider = a.getProgrammeDetailProvider() || [];
                var activeIndex = a.getActiveIndex();
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDIN",
                        "TRS": "I",
                        "OSD": "CPgDetail"
                    });
                }
                a._create(programmeDetailProvider, activeIndex);
            };
            CProgrammeDetailDlg.prototype.destroy = function () {
                var a = this;
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "CPgDetail"
                    });
                }
                if (a._destroyParam) {
                    a._destroyParam();
                }
                if (a._programmeDetailProvider) {
                    a._programmeDetailProvider.destroy();
                    a._programmeDetailProvider = null;
                }
                if (a._ctrlEpisodeList) {
                    delete a._ctrlEpisodeList;
                    a._ctrlEpisodeList = null;
                }
                if (a._descriptionView) {
                    a._descriptionView.destroy();
                    delete a._descriptionView;
                    a._descriptionView = null;
                }
                if (a._actionListView) {
                    a._actionListView.destroy();
                    delete a._actionListView;
                    a._actionListView = null;
                }
                if (a._viewTitle) {
                    a._viewTitle.destroy();
                    delete a._viewTitle;
                    a._viewTitle = null;
                }
                if (a._$pde) {
                    a._$pde.remove();
                    a._$pde = null;
                }
                if (a._$wrap) {
                    a._$wrap = null;
                }
                hx.log('error', '[detail] destroy CProgrammeDetailDlg');
                _super.prototype.destroy.call(a);
            };
            CProgrammeDetailDlg.prototype.fnAfterDraw = function () {
                var a = this;
                if (a._ctrlEpisodeList) {
                    var selectedIdx = a._ctrlEpisodeList.getSelectedIndex();
                    if (selectedIdx < 10) {
                        var focusedItem = a._ctrlEpisodeList.getDrawnItems().getItem(selectedIdx);
                        if (focusedItem) {
                            focusedItem.addClass('his');
                        }
                    }
                }
            };
            CProgrammeDetailDlg.prototype.doKeyOption = function () {
                var a = this;
                if (a._activeOptionMenuInfo) {
                    if (!a._contextMenuDlg) {
                        //update ContextMenu
                        a._ctrlEpisodeList.onFocusChanged(a._ctrlEpisodeList.getFocusedItem().$item, a._ctrlEpisodeList.getFocusedItem().$item);
                        a.menuStep = 0;
                        _super.prototype.doKeyOption.call(this);
                        a._contextMenuDlg.doKeyOption = function () {
                            if (a.menuStep != 0) {
                                fnOptionSubMenuHandle(a);
                            }
                            else {
                                a._contextMenuDlg.close();
                                a.menuStep = 0;
                            }
                            return true;
                        };
                    }
                }
                return true;
            };
            CProgrammeDetailDlg.prototype.onContextMenuClose = function () {
                var a = this;
                delete a._contextMenuDlg;
                if (a._ctrlEpisodeList) {
                    a._ctrlEpisodeList.onFocusChanged(a._ctrlEpisodeList.getFocusedItem().$item, a._ctrlEpisodeList.getFocusedItem().$item);
                }
            };
            CProgrammeDetailDlg.prototype.addOptionMenuTarget = function (control) {
                var a = this;
                (function (c) {
                    c.onActiveFocusedGained = function (newTarget) {
                        a._loading = true;
                        a._onFocusHandlers(c);
                        if (!a._delete) {
                            a._onFocusHandlers(c);
                        }
                        else {
                            a._delete = false;
                        }
                    };
                    c.onActiveFocusedLost = function () {
                        a._deleteNum = [];
                        a._multiDelete = true;
                        a._onFocusHandlers(null);
                    };
                    c.onFocusChanged = function (oldTarget, newTarget) {
                        if (oldTarget !== null && a._loading === true) {
                            a._onFocusHandlers(c, newTarget);
                        }
                        else {
                            a._loading = true;
                            a._onFocusHandlers(null);
                        }
                    };
                }(control));
            };
            CProgrammeDetailDlg.prototype.openNotification = function (aText) {
                prism.createNotificationDlg(this._parent, aText).open();
            };
            CProgrammeDetailDlg.prototype.openHDAlternative = function (aButtonInfo) {
                prism.createHDAlternativeButtonDlg(this._parent, aButtonInfo).open();
            };
            CProgrammeDetailDlg.prototype.updateEvent = function (aProgramme, reason) {
                var provider = this.getProgrammeDetailProvider();
                if (provider) {
                    var len = provider.getCount();
                    var i;
                    for (i = 0; i < len; i++) {
                        if (provider.getItem(i).getProgrammeID() === aProgramme.getProgrammeID()) {
                            hx.log("debug", "onProgrammesChanged matched");
                            hx.log("debug", "this.getActiveIndex() === i:" + this.getActiveIndex() + " === " + i);
                            if (this.getActiveIndex() === i) {
                                hx.log("debug", "onProgrammesChanged updating");
                                hx.log("debug", "provider.getItem(i).getProgrammeID() === aProgramme.getProgrammeID()");
                                this._viewTitle.update(aProgramme);
                                this._descriptionView.update(aProgramme);
                                this._actionListView.update(aProgramme, reason);
                                this.setActiveFocus(true);
                            }
                        }
                    }
                }
            };
            // Private
            CProgrammeDetailDlg.prototype._create = function (programmeDetailProvider, activeIndex) {
                var a = this;
                var activeItem = programmeDetailProvider ? programmeDetailProvider.getItem(activeIndex) : null;
                var viewEpisodeList = new CEpisodeListView(programmeDetailProvider, activeIndex);
                var ctrlEpisodeList = viewEpisodeList.getListControl();
                if (ENV.op === ENV.listOP.UKDTT) {
                    if (activeItem) {
                        activeItem.parent_activeViewType = this._parent._activeViewType;
                    }
                }
                var viewActionList = new CActionListView(activeItem);
                var viewDescription = new CDescriptionView(activeItem);
                var viewTitle = new CTitleView(activeItem);
                var views = [viewActionList, viewDescription, viewTitle];
                var ctrlVGroup = new __CONTROLS__.CVerticalGroupControl(a._$pde);
                var ctrlHGroup = new __CONTROLS__.CHorizontalGroupControl(null);
                a._ctrlEpisodeList = ctrlEpisodeList;
                a._descriptionView = viewDescription;
                a._actionListView = viewActionList;
                a._viewTitle = viewTitle;
                a._loading = false;
                a._delete = false;
                a._multiDelete = true;
                a._refresh = false;
                ctrlHGroup.setChildControl([viewEpisodeList, viewActionList, viewDescription], 1);
                ctrlHGroup.doKey = function (keyCode) {
                    var self = this;
                    if (keyCode === 39 && !viewDescription.view.isFocusable() && self.getFocusedChildIndex() === 1) {
                        return true;
                    }
                    else if (keyCode === hx.key.KEY_OK && viewDescription.view.isFocusable() && self.getFocusedChildIndex() === 2) {
                        return true;
                    }
                    return CHorizontalGroupControl.prototype.doKey.call(self, keyCode);
                };
                ctrlVGroup.setChildControl([viewTitle, ctrlHGroup], 1);
                a.setChildControl([ctrlVGroup]);
                a.setDrawParam({
                    $parent: a._parent.$layer()
                });
                a.addOptionMenuTarget(a._ctrlEpisodeList);
                if (activeItem instanceof __MODEL__.IPdeProgrammeItem) {
                    if (!activeItem.getMdsData()) {
                        a.onRequestMdsSchedule(activeItem, activeIndex);
                    }
                }
                ctrlEpisodeList.onItemSelected = function (param) {
                    var self = this;
                    var selectedItem = param.item;
                    var $selectedItem = param.$item;
                    var selectedIndex = param.itemKey;
                    var view;
                    var i;
                    if (self._$prev) {
                        self._$prev.removeClass('his');
                    }
                    else {
                        a._$self.find('.his').removeClass('his');
                    }
                    $selectedItem.addClass('his');
                    self._$prev = $selectedItem;
                    if (selectedItem instanceof __MODEL__.IPdeProgrammeItem) {
                        if (!selectedItem.getMdsData()) {
                            a.onRequestMdsSchedule(selectedItem, selectedIndex);
                        }
                    }
                    for (i = 0; i < views.length; i += 1) {
                        view = views[i];
                        view.update(selectedItem);
                    }
                };
                viewActionList.onItemSelected = function (i, $item) {
                    var self = this;
                    var selectedIndex = ctrlEpisodeList.getSelectedIndex();
                    var selectedItem = programmeDetailProvider.getItem(selectedIndex);
                    var actionList = self.getCurrentActionList();
                    a._refresh = false;
                    if (actionList[i].name == tr('LOC_CANCEL_SERIESREC_BTN_ID')) {
                        a._cancelWholeSeries = true;
                    }
                    if (actionList[i].name === 'RECORD WHOLE SERIES' && selectedItem instanceof __PROVIDER__.cPdeRecordingItem) {
                        for (var j = selectedIndex + 1; j < programmeDetailProvider.getCount(); j++) {
                            var item = programmeDetailProvider.getItem(j);
                            if (!(item instanceof __PROVIDER__.cPdeRecordingItem)) {
                                actionList[i].action(item, a);
                                return;
                            }
                        }
                    }
                    actionList[i].action(selectedItem, a);
                };
                programmeDetailProvider.addEventHandler('ItemChanged', a, function (keys) {
                    var view;
                    if (keys.length) {
                        var i, key = keys[0];
                        if (key === ctrlEpisodeList.getSelectedIndex() || a._refresh === false) {
                            var item = programmeDetailProvider.getItem(ctrlEpisodeList.getSelectedIndex());
                            if (!!a._cancelWholeSeries) {
                                var schedulelist = hx.svc.RECORDINGS.getScheduledList();
                                var i, j, k;
                                for (i = 0; i < schedulelist.length; i += 1) {
                                    if (schedulelist[i].getChannelId() !== item.getChannelId()) {
                                        continue;
                                    }
                                    if (schedulelist[i].getGroupCRIDs() !== undefined && item.getGroupCRIDs() !== undefined) {
                                        if (schedulelist[i].getGroupCRIDs().length > 0 && item.getGroupCRIDs().length > 0) {
                                            for (j = 0; j < schedulelist[i].getGroupCRIDs().length; j += 1) {
                                                for (k = 0; k < item.getGroupCRIDs().length; k += 1) {
                                                    if (schedulelist[i].getGroupCRIDs()[j] === item.getGroupCRIDs()[k]) {
                                                        return;
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (schedulelist[i].getTitle() === item.getTitle()) {
                                                return;
                                            }
                                        }
                                    }
                                }
                                a._cancelWholeSeries = false;
                            }
                            for (i = 0; i < views.length; i += 1) {
                                view = views[i];
                                if (item instanceof __MODEL__.IPdeRecordingItem) {
                                    if (!(view instanceof CDescriptionView) && !(view instanceof CTitleView)) {
                                        view.update(item);
                                    }
                                }
                                else {
                                    view.update(item);
                                }
                            }
                            if (ctrlHGroup.getFocusedChild() === viewActionList) {
                                viewActionList.setActiveFocus(true);
                            }
                            a._refresh = true;
                        }
                    }
                });
                programmeDetailProvider.addEventHandler('ItemRemoved', a, function (keys, items) {
                    var count = programmeDetailProvider.getCount();
                    if (count === 0) {
                        a.onRequestClose();
                    }
                    else if (keys.length) {
                        var key = keys[0];
                        var idx;
                        var i, len = ctrlEpisodeList._keyMap.length;
                        if (a._multiDelete) {
                            if (a._deleteNum && a._deleteNum.length > 0) {
                                for (i = 0; i < len; i += 1) {
                                    if (a._deleteNum.length - 1 < i) {
                                        a._deleteIdx = i;
                                        break;
                                    }
                                    if (i !== a._deleteNum[i]) {
                                        a._deleteIdx = i;
                                        break;
                                    }
                                }
                            }
                            else {
                                //only button delete
                                if (len - 1 > a._activeIndex) {
                                    a._deleteIdx = a._activeIndex + 1;
                                }
                                else {
                                    a._deleteIdx = a._activeIndex - 1;
                                }
                            }
                            if (len > a._deleteIdx) {
                                var $focusedItem = ctrlEpisodeList._keyMap[a._deleteIdx].$;
                                if ($focusedItem) {
                                    var itemKey = parseInt($focusedItem.attr('data'), 10);
                                    var item = ctrlEpisodeList._dataProvider.getItem(itemKey);
                                    ctrlEpisodeList._selectedIndex = itemKey;
                                    ctrlEpisodeList.onItemSelected({
                                        $item: $focusedItem,
                                        itemKey: itemKey,
                                        item: item
                                    });
                                }
                                //loop control
                                a._multiDelete = false;
                            }
                        }
                    }
                    if (a._activeOptionMenuInfo !== undefined || a._delete === true) {
                        //ctrlEpisodeList.setFocusedItemByKey(count <= key ? count - 1 : key);
                        viewActionList.setActiveFocus(false);
                    }
                    else {
                        viewActionList.setActiveFocus(true);
                        ctrlEpisodeList.setActiveFocus(false);
                    }
                });
            };
            return CProgrammeDetailDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CProgrammeDetailDlg = CProgrammeDetailDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CProgrammeDetailDlg = CProgrammeDetailDlg;
        prism.createProgrammeDetailDlg = function (layeredGroupControl) {
            return new CProgrammeDetailDlg(layeredGroupControl);
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
