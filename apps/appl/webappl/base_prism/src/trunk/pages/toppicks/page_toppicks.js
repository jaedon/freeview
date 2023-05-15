var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", 'model/model', "pages/util/provider"], function (require, exports, __PAGE_KERNEL__, __MODEL__, __PROVIDER__) {
    var CPgTopPicks = (function (_super) {
        __extends(CPgTopPicks, _super);
        function CPgTopPicks() {
            var _this = this;
            _super.call(this);
            this._dataAPI = {
                getOnDemandCategories: function (updateCb, waitingCb) {
                    if (waitingCb) {
                        waitingCb();
                    }
                    hx.svc.IP_EPG.getRecommendationCategories().then(function (categoryInfo) {
                        if (updateCb) {
                            updateCb(categoryInfo ? hx.svc.IP_EPG.parse.xml.category(categoryInfo) : null);
                        }
                    }, function () {
                        if (updateCb) {
                            updateCb(null);
                        }
                    });
                },
                getRecentlyRecordings: function () {
                    var KRecordingsMax = 14;
                    var recentlyList = [];
                    if (hx.emulMode) {
                        return [];
                        recentlyList = hx.svc.RECORDINGS.getRecordingList().slice(0, KRecordingsMax);
                        if (recentlyList && recentlyList.length) {
                            recentlyList = recentlyList.slice(0, KRecordingsMax);
                            recentlyList.push({
                                type: 'more'
                            });
                        }
                        return recentlyList;
                    }
                    if (!hx.svc.RECORDINGS.getRecordingList().length) {
                        if (!hx.svc.RECORDINGS.isRecordingListLoaded()) {
                            hx.log('debug', '[page_toppicks][getRecentlyRecordings] Recording list is not prepared yet');
                            return;
                        }
                    }
                    var list = hx.svc.HISTORY.getRecordingList();
                    var length = list ? list.length : 0;
                    var item = null;
                    for (var i = 0; i < length; i++) {
                        item = hx.svc.RECORDINGS.getRecording({ 'id': list[i].getId() });
                        if (item && item.getDuration()) {
                            if (item.isNew() && !item.isBroken()) {
                                recentlyList.push(item);
                            }
                        }
                        else {
                            hx.svc.HISTORY.removeHistoryItem({
                                type: __MODEL__.THistoryType.Recording,
                                id: list[i].getId()
                            });
                        }
                    }
                    if (recentlyList && recentlyList.length) {
                        recentlyList = recentlyList.slice(0, KRecordingsMax);
                        recentlyList.push({
                            type: 'more'
                        });
                    }
                    return recentlyList;
                },
                getContinueWatchRecordings: function () {
                    var KRecordingsMax = 14;
                    var watchingList = [];
                    if (hx.emulMode) {
                        return [];
                        watchingList = hx.svc.RECORDINGS.getRecordingList().slice(0, KRecordingsMax);
                        if (watchingList && watchingList.length) {
                            watchingList = watchingList.slice(0, KRecordingsMax);
                            watchingList.push({
                                type: 'more'
                            });
                        }
                        return watchingList;
                    }
                    if (!hx.svc.RECORDINGS.getRecordingList().length) {
                        if (!hx.svc.RECORDINGS.isRecordingListLoaded()) {
                            hx.log('debug', '[page_toppicks][getContinueWatchRecordings] Recording list is not prepared yet');
                            return;
                        }
                    }
                    var list = hx.svc.HISTORY.getPlaybackList();
                    var length = list ? list.length : 0;
                    var item = null;
                    for (var i = 0; i < length; i++) {
                        item = hx.svc.RECORDINGS.getRecording({ 'id': list[i].getId() });
                        if (item && item.getDuration()) {
                            if (item.wasWatching()) {
                                watchingList.push(item);
                            }
                        }
                        else {
                            hx.svc.HISTORY.removeHistoryItem({
                                type: __MODEL__.THistoryType.Playback,
                                id: list[i].getId()
                            });
                        }
                    }
                    if (watchingList && watchingList.length) {
                        watchingList = watchingList.slice(0, KRecordingsMax);
                        watchingList.push({
                            type: 'more'
                        });
                    }
                    return watchingList;
                },
                getOnDemandGroup: function (groupId, updateCb, waitingCb) {
                    if (waitingCb) {
                        waitingCb(groupId);
                    }
                    _this.getRecommendationGroup(groupId, updateCb);
                },
                getOnNowChannels: function () {
                    if (hx.emulMode) {
                        return [];
                    }
                    if (_this._channelCount === 0) {
                        return [];
                    }
                    return hx.svc.HISTORY.getOnNowChannels();
                },
                getChannelCount: function () {
                    var that = _this;
                    if (that._channelCount === undefined) {
                        that._channelCount = hx.svc.CH_LIST.getChannelCount({ 'group': 'TV' }).chCnt;
                    }
                    return that._channelCount || 0;
                },
                searchOnNowEPGByCcid: function (ccid, updateCb) {
                    var that = _this;
                    var channel;
                    var startTime = 0, endTime = 0;
                    if (hx.svc.TSR.isTsrOngoing() === true) {
                        var vbChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                        if (!!vbChannel) {
                            var tsrStartTime = hx.svc.TSR.getStartTime();
                            var tsrPlaybackOffset = hx.svc.TSR.getPlaybackOffset();
                            startTime = tsrStartTime + tsrPlaybackOffset;
                        }
                        else {
                            var now = new Date();
                            startTime = Math.floor(now.getTime() / 1000);
                        }
                    }
                    else {
                        var now = new Date();
                        startTime = Math.floor(now.getTime() / 1000);
                    }
                    endTime = startTime + 3600 * 1; // 1hour
                    channel = hx.svc.CH_LIST.getChannel({
                        'ccid': ccid,
                        'group': 'TV'
                    });
                    channel = channel.channel;
                    hx.svc.EPG.getProgrammes({
                        channel: channel,
                        startTime: startTime,
                        endTime: endTime,
                        listener: that,
                        notUI: true,
                        cb: function (result) {
                            if (result) {
                                result = result[0];
                                if (result) {
                                    result = result[0];
                                }
                            }
                            var getThumbnail = (function (channel, startTime) {
                                return function (onNow, width, updateCb) {
                                    that.getSchedule(channel, startTime, onNow, width, updateCb);
                                };
                            })(channel, (result && result.getStartTime) ? result.getStartTime() : startTime);
                            var linkage = result ? result.getHDLinkage() : null;
                            if (false) {
                                //if (linkage && linkage.length) {
                                var hdChannel = hx.svc.CH_LIST.getChannelByCcidExceptALL({
                                    ccid: linkage[0].channel
                                });
                                if (hdChannel) {
                                    hx.svc.EPG.findProgrammes({
                                        channel: hdChannel.channel,
                                        clause: 'programmeID == ' + linkage[0].event,
                                        cb: function (aResult, b, c) {
                                            var programmes = aResult[0];
                                            if (hx.emulMode) {
                                                programmes.push(item);
                                            }
                                            if (programmes.length) {
                                                result = programmes[0];
                                            }
                                            updateCb({
                                                result: result,
                                                getThumbnail: (function (channel, startTime) {
                                                    return function (onNow, width, updateCb) {
                                                        that.getSchedule(channel, startTime, onNow, width, updateCb);
                                                    };
                                                })(hdChannel.channel, startTime),
                                                needUpdateChannel: true,
                                                channel: channel
                                            });
                                        }
                                    });
                                }
                                else {
                                    updateCb({
                                        result: result,
                                        getThumbnail: getThumbnail,
                                        channel: channel
                                    });
                                }
                            }
                            else {
                                updateCb({
                                    result: result,
                                    getThumbnail: getThumbnail,
                                    channel: channel
                                });
                            }
                        }
                    });
                },
                getChannelLogo: function (ccid, width, updateCb) {
                    var splitStr = ccid.split('.');
                    if (splitStr.length > 0) {
                        var sid = splitStr[splitStr.length - 1];
                        if (sid) {
                            hx.svc.IP_EPG.getChannelLogo(sid, width).then(function (receivedUrl) {
                                updateCb(receivedUrl);
                            }, function () {
                                updateCb(null);
                            });
                        }
                    }
                },
                getChannels: function () {
                    var ret = [];
                    var retObj = hx.svc.CH_LIST.getChlist({
                        group: "TV"
                    });
                    var chList = retObj.chList;
                    var channel;
                    for (var i = 0; i < chList.length; i++) {
                        channel = chList[i];
                        if (!channel.hidden) {
                            ret.push({
                                'id': channel.ccid,
                                'name': hx.il.makeDigitNumber(channel.majorChannel) + ' &nbsp;' + '<strong>' + channel.name + '</strong>'
                            });
                        }
                    }
                    chList = undefined;
                    channel = undefined;
                    retObj = undefined;
                    return ret;
                },
                updateOnNowChannel: function (index, ccid) {
                    var channel = hx.svc.CH_LIST.getChannel({
                        'group': "TV",
                        'ccid': ccid
                    });
                    hx.svc.HISTORY.updateOnNowChannel({
                        'index': index,
                        'channel': channel.channel
                    });
                },
                setChannelByCcid: function (ccid) {
                    var that = _this;
                    var channel = hx.svc.CH_LIST.getChannel({
                        'group': "TV",
                        'ccid': ccid
                    });
                    if (channel && channel.channel) {
                        var changeable = hx.svc.CH_LIST.checkChannelChangable({
                            'channel': channel.channel
                        });
                        if (changeable) {
                            hx.svc.AV_CTRL.channelChange({
                                'channel': channel.channel,
                                'group': "TV"
                            });
                            that._exit_to_live();
                        }
                        else {
                            hx.msg('response', {
                                text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                    }
                },
                openDeepLink: function (ondemandItem) {
                    var that = _this;
                    hx.il.openDeepLink(ondemandItem.name, ondemandItem.deeplinkUrl, undefined, undefined, undefined, that);
                },
                showProgrammeDetail: function (aItem) {
                    var that = _this;
                    var detailItem = null;
                    if (aItem instanceof __MODEL__.COipfRecordingItem) {
                        that.showProgrammeDetail(detailItem = hx.svc.DETAIL_INFO.createRecordingItem(aItem));
                    }
                    else if (aItem instanceof __PROVIDER__.COnDemandProvider) {
                        hx.svc.IP_EPG.getDetailInfo(aItem.getProgrammeCridMds()).then(function (info) {
                            aItem.updateMdsData('program', info.querySelector('ProgramInformation'));
                            that.showProgrammeDetail(detailItem = hx.svc.DETAIL_INFO.createOnDemandItem(aItem));
                        }, function (err) {
                            that.showProgrammeDetail(detailItem = hx.svc.DETAIL_INFO.createOnDemandItem(aItem));
                        });
                    }
                    else if (aItem instanceof __PROVIDER__.CProgrammeProvider) {
                        hx.svc.IP_EPG.getDetailInfo(aItem.getProgrammeCridMds()).then(function (info) {
                            aItem.updateMdsData('program', info.querySelector('ProgramInformation'));
                            that.showProgrammeDetail(detailItem = hx.svc.DETAIL_INFO.createProgrammeItem(aItem));
                        }, function (err) {
                            that.showProgrammeDetail(detailItem = hx.svc.DETAIL_INFO.createProgrammeItem(aItem));
                        });
                    }
                },
                requestPlayback: function (aItem) {
                    var that = _this;
                    var fnCreateQueryPasswordDlg = function () {
                        var passwordDlg = prism.createQueryPasswordDlg(that._page);
                        passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                        return passwordDlg;
                    };
                    var DTG_ask_password = false;
                    var guidancePolicy = hx.svc.SETTING_UTIL.getGuidancePolicy();
                    var watersheds = false;
                    var now = new Date();
                    var min = now.getMinutes();
                    var hour = now.getHours();
                    var time = hour * 60 + min;
                    if (time >= 1260 || time < 330) {
                        watersheds = true;
                    }
                    if (watersheds === true) {
                        DTG_ask_password = false;
                    }
                    else {
                        switch (guidancePolicy) {
                            case 0:
                                DTG_ask_password = false;
                                break;
                            case 1:
                                DTG_ask_password = !!aItem.hasGuidance();
                                break;
                            case 2:
                                DTG_ask_password = false;
                                break;
                            case 3:
                                DTG_ask_password = true;
                                break;
                            default:
                                break;
                        }
                    }
                    if (aItem.isLocked() || (DTG_ask_password === true)) {
                        var passwordDlg = fnCreateQueryPasswordDlg();
                        var CTsrItem = new __MODEL__.CTsrItem(null);
                        passwordDlg.open({
                            dimm: true,
                            zindex: 1,
                            onPasswordEntered: function (password) {
                                var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                                    password: password
                                });
                                if (result && (result.result === 0)) {
                                    passwordDlg.close();
                                    //if (CTsrItem.isChase()) {
                                    //    setPCStatus(password, false);
                                    //}
                                    hx.svc.BRIDGE.requestVideoPlayback([aItem], 0);
                                }
                                else {
                                    passwordDlg.reset();
                                }
                            }
                        });
                    }
                    else {
                        hx.svc.BRIDGE.requestVideoPlayback([aItem], 0);
                    }
                },
                seeMore: function (aFocusedIndex) {
                    var that = _this;
                    var eTopPicks = that._page.getEnumTopPicks();
                    var pageToChange = '';
                    switch (aFocusedIndex) {
                        case eTopPicks.eRECORDINGS:
                            pageToChange = "CPgRecordings";
                            break;
                        default:
                            break;
                    }
                    if (pageToChange) {
                        that.sendEventToEntity({
                            'alEvent': 'alChangePage',
                            'target': pageToChange + '?refresh=true',
                            'me': that
                        });
                    }
                },
                getHumaxRecommendation: function (aType, aUpdateCb, aWaitingCb) {
                    var that = _this;
                    if (that._processingHmxContents[aType]) {
                        hx.log('error', '[getHumaxRecommendation] that._processingHmxContents[aType]');
                        if (aWaitingCb) {
                            aWaitingCb(aType);
                        }
                        return;
                    }
                    if (that._hmxContents[aType] && that._hmxContents[aType].length) {
                        aUpdateCb(that._hmxContents[aType], aType);
                        return;
                    }
                    if (aWaitingCb) {
                        aWaitingCb(aType);
                    }
                    that._processingHmxContents[aType] = true;
                    hx.svc.TV_PORTAL.getRecommendations({
                        fn: function (data) {
                            that.searchProgrammes(aType, data, aUpdateCb);
                        },
                        fnError: function () {
                            if (aUpdateCb) {
                                aUpdateCb(null);
                            }
                        },
                        userParam: {
                            type: aType,
                        }
                    });
                },
                getThumbnail: function (prog, width, updateCb) {
                    return _this.getSchedule(prog.channel, prog.getStartTime(), prog, width, updateCb);
                },
                requestRecord: function (ccid) {
                    var that = _this;
                    var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                    if (bIsGetPvrHDD === false) {
                        that.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgNotAvailable?show'
                        });
                        return false;
                    }
                    if (!ccid) {
                        return false;
                    }
                    var recordings = hx.svc.RECORDINGS.getCurrentRecordings();
                    var i, length, recording;
                    if (recordings && recordings.length) {
                        length = recordings.length;
                        for (i = 0; i < length; i += 1) {
                            recording = recordings[i];
                            if (recording.getChannelId() === ccid) {
                                hx.svc.RECORDINGS.stopRecord({
                                    item: recording
                                });
                                return true;
                            }
                        }
                    }
                    hx.svc.BRIDGE.requestInstanceRecording(hx.svc.CH_LIST.getChannel({
                        'ccid': ccid
                    }).channel);
                    return true;
                },
                requestStopRecord: function (ccid) {
                    if (!ccid) {
                        return false;
                    }
                    var recordings = hx.svc.RECORDINGS.getCurrentRecordings();
                    var i, length, recording;
                    if (recordings && recordings.length) {
                        length = recordings.length;
                        for (i = 0; i < length; i += 1) {
                            recording = recordings[i];
                            if (recording.getChannelId() === ccid) {
                                hx.svc.RECORDINGS.stopRecord({
                                    item: recording
                                });
                                return true;
                            }
                        }
                    }
                },
                requestReservation: function (aItem) {
                    var that = _this;
                    var fnCancelReservation = function (data) {
                        var scheduledList = hx.svc.RECORDINGS.getScheduledList();
                        var i, ti, scheduledItem, programmeID, foundItem;
                        for (i = 0, ti = scheduledList.length; i < ti; i += 1) {
                            scheduledItem = scheduledList[i];
                            programmeID = scheduledItem.getProgrammeID();
                            if (programmeID && (programmeID === data.programmeID)) {
                                foundItem = scheduledItem;
                                break;
                            }
                        }
                        if (foundItem) {
                            hx.svc.RECORDINGS.removeScheduledRecording({
                                items: [scheduledItem],
                                finished: function () {
                                    hx.log('CPgTopPicks', 'finished removeScheduledRecording');
                                }
                            });
                        }
                    };
                    var fnCancelSeriesReservation = function (data) {
                        hx.log('CPgTopPicks', 'cancelSeriesReservation()');
                        var scheduledList = hx.svc.RECORDINGS.getScheduledList();
                        var i, ti, scheduledItem, programmeID, foundItem;
                        for (i = 0, ti = scheduledList.length; i < ti; i += 1) {
                            scheduledItem = scheduledList[i];
                            programmeID = scheduledItem.getProgrammeID();
                            if (programmeID && (programmeID === data.programmeID)) {
                                foundItem = scheduledItem;
                                break;
                            }
                        }
                        if (foundItem) {
                            hx.svc.RECORDINGS.removeWholeSeriesSchedule({
                                items: (function (targetItem) {
                                    var i, j, k;
                                    var ret = [];
                                    var dsrvRecordings2 = hx.svc.RECORDINGS;
                                    var schedulelist = dsrvRecordings2.getScheduledList();
                                    for (i = 0; i < schedulelist.length; i += 1) {
                                        if (schedulelist[i].getGroupCRIDs() && targetItem.getGroupCRIDs()) {
                                            if (targetItem.getGroupCRIDs().length > 0 && schedulelist[i].getGroupCRIDs().length > 0) {
                                                for (j = 0; j < targetItem.getGroupCRIDs().length; j += 1) {
                                                    for (k = 0; k < schedulelist[i].getGroupCRIDs().length; k++) {
                                                        if (targetItem.getGroupCRIDs()[j] === schedulelist[i].getGroupCRIDs()[k]) {
                                                            ret.push(schedulelist[i]);
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    return ret;
                                })(foundItem),
                                finished: function () {
                                    hx.log('CPgTopPicks', 'finished removeSeriesScheduledRecording');
                                }
                            });
                        }
                    };
                    var fnSelectRecordType = function (aProgramme) {
                        var itemList = [];
                        var fnMakeMenuItem = function (aName) {
                            itemList.push({
                                _one: aName,
                                _programme: aProgramme
                            });
                        };
                        fnMakeMenuItem(hx.l('LOC_TIMERREC_BTN1_ID'));
                        fnMakeMenuItem(hx.l('LOC_SERIESREC_BTN_ID'));
                        hx.msg('recordingConflict', {
                            'title': hx.l('LOC_RECORD_RECOMMENDATION_ID'),
                            'text': hx.l('LOC_GUIDE_RECORD_SELECT_ID'),
                            'list_arr': itemList,
                            'dia_class': 'osd',
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
                                    if (_data && _data._programme) {
                                        fnDoReservation(_data._programme, factoryType);
                                    }
                                }
                            }
                        });
                    };
                    var fnDoReservation = function (data, aFactoryType) {
                        var efactoryType = aFactoryType || 0 /* ERECORDING */;
                        var scheduledList = hx.svc.RECORDINGS.getScheduledList();
                        hx.log('CPgTopPicks', 'doReservation()');
                        // Check Max reservation count
                        if (scheduledList.length >= 300) {
                            var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                            hx.msg('response', {
                                text: msg,
                                auto_close: true,
                                close_time: 3000
                            });
                            return;
                        }
                        var _fnRecord = function (aProgramme) {
                            hx.svc.RECORDINGS.record({
                                programme: aProgramme,
                                factoryType: efactoryType,
                                finished: function () {
                                    hx.log('CPgTopPicks', 'finished record');
                                }
                            });
                        };
                        var linkage = JSON.parse(data.pluginObject.getField("freeview/linkage"));
                        if (linkage.length) {
                            var channel = hx.svc.CH_LIST.getChannel({
                                ccid: linkage[0].channel,
                                group: 'ALL'
                            });
                            if (channel.result) {
                                hx.svc.EPG.findProgrammes({
                                    channel: channel.channel,
                                    clause: 'programmeID == ' + linkage[0].event,
                                    cb: function (aResult, b, c) {
                                        var programmes = aResult[0];
                                        if (programmes.length) {
                                            var hdProgramme = programmes[0];
                                            fnSelectSimulcast({
                                                hdProgramme: hdProgramme,
                                                sdProgramme: data,
                                                fnRecorder: _fnRecord
                                            });
                                        }
                                        else {
                                            hx.log('CPgTopPicks', "linkaged detected but can't find HD programme");
                                            _fnRecord(data.pluginObject);
                                        }
                                    }
                                });
                            }
                        }
                        else {
                            _fnRecord(data.pluginObject);
                        }
                    };
                    var fnSelectSimulcast = function (aData) {
                        var hdProgramme = aData.hdProgramme;
                        var sdProgramme = aData.sdProgramme;
                        var fnRecorder = aData.fnRecorder;
                        var dlg = prism.createHDAlternativeButtonDlg(null, [
                            {
                                text: hdProgramme.getName(),
                                fn: function () {
                                    hx.log("debug", hdProgramme.getName() + " HD Programme recording");
                                    fnRecorder(hdProgramme.getPluginObject());
                                }
                            },
                            {
                                text: sdProgramme.getName(),
                                fn: function () {
                                    hx.log("debug", sdProgramme.getName() + " SD Programme recording");
                                    fnRecorder(sdProgramme.getPluginObject());
                                }
                            }
                        ]);
                        that.setKeyHookingControl(dlg, true);
                        dlg.open({
                            onClosed: function () {
                                that.setKeyHookingControl(dlg);
                            }
                        });
                    };
                    if (aItem.hasSchedule() || aItem.hasReminder()) {
                        fnCancelReservation(aItem);
                    }
                    else if (aItem.hasSeriesSchedule()) {
                        fnCancelSeriesReservation(aItem);
                    }
                    else {
                        if (aItem.getGroupCRIDs() && aItem.getGroupCRIDs().length) {
                            fnSelectRecordType(aItem);
                        }
                        else {
                            fnDoReservation(aItem);
                        }
                    }
                    return true;
                }
            };
        }
        CPgTopPicks.prototype.create = function (aEntity, aPageUrl) {
            var that = this;
            hx.al.compose(that);
            hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
            that.init(aPageUrl);
        };
        CPgTopPicks.prototype.destroy = function () {
            var that = this;
            if (that._page) {
                that._page.destroy();
                delete that._page;
                that._page = undefined;
            }
            if (that._hmxContents) {
                that._hmxContents = null;
            }
            if (that._processingHmxContents) {
                that._processingHmxContents = null;
            }
            if (that._detailDlg) {
                that._detailDlg.close();
                that._detailDlg = null;
            }
            that._channelCount = null;
            hx.svc.RECORDINGS.clearEventCb(that);
            // send a message to homemenu to refresh ondemand contents
            //that.sendEventToEntity({
            //    alEvent : 'alMessageToPage',
            //    target : 'CPgHubProxy?refresh=ondemand'
            //});
            hx.svc.FRONT_PANEL.clearFrontPanelText();
        };
        CPgTopPicks.prototype.init = function (aPageUrl) {
            var that = this;
            var page = prism.createTopPicksPage($('#wrap'), aPageUrl ? aPageUrl.path : '');
            hx.il.updatePageBackground();
            hx.logadd("CPgTopPicks");
            hx.log("CPgTopPicks", "[CPgTopPicks][create]");
            that._hmxContents = {};
            that._processingHmxContents = {};
            that._channelCount = undefined;
            that._page = page;
            page.setDataAPI(that._dataAPI);
            that._page.draw();
            hx.svc.RECORDINGS.addEventCb('RecordingItemAdded', that, function (param) {
                var item = param ? param.item : null;
                if (that._page && item) {
                    that._page.onRecordingItemAdded(item);
                }
            });
            //hx.svc.RECORDINGS.addEventCb('RecordingCompleted', that, (param) => {
            //    var item = param ? param.item : null;
            //    if (that._page && item) {
            //        that._page.onRecordingStopped(item);
            //    }
            //});
            hx.svc.RECORDINGS.addEventCb('NotiRecordingStop', that, function (param) {
                var item = param ? param.item : null;
                if (that._page && item) {
                    that._page.onRecordingItemRemoved(item);
                }
            });
            //hx.svc.RECORDINGS.addEventCb('RecordingItemUpdated', that, (param) => {
            //    var item = param ? param.item : null;
            //    if (that._page && item) {
            //        that._page.onRecordingItemUpdated(item);
            //    }
            //});
            hx.svc.RECORDINGS.addEventCb('ScheduledItemAdded', that, function (param) {
                var item = param ? param.item : null;
                if (that._page && item) {
                    that._page.onScheduledItemAdded(item);
                }
            });
            hx.svc.RECORDINGS.addEventCb('ScheduledItemRemoved', that, function (param) {
                var item = param ? param.item : null;
                if (that._page && item) {
                    that._page.onScheduledItemRemoved(item);
                }
            });
            hx.svc.FRONT_PANEL.setFrontPanelText('-TOPPICKS-');
        };
        CPgTopPicks.prototype.getRecommendationGroup = function (groupId, updateUICb) {
            var that = this;
            hx.svc.IP_EPG.getRecommendationResults({
                'groupId': groupId
            }).then(function (recommendationInfo) {
                if (!recommendationInfo) {
                    if (updateUICb) {
                        updateUICb(groupId, null);
                    }
                    return;
                }
                var recommendations = hx.svc.IP_EPG.parse.xml.recommendation(recommendationInfo);
                // this is an ordered list
                var crids = recommendations.crids;
                var odProgs = [], i, odItem = null;
                var queryList = [];
                function addAITQueryToQueryList(aOndemand) {
                    var subRetDef = Q.defer();
                    subRetDef.promise.then(function (aXml) {
                        if (aOndemand.checkTemplateAIT(aXml)) {
                            odProgs.push(aOndemand);
                        }
                    });
                    var query = {
                        url: aOndemand.getTemplateAitUrl(),
                        retDef: subRetDef
                    };
                    queryList.push(query);
                }
                if (crids) {
                    for (i in crids) {
                        odItem = new __PROVIDER__.COnDemandProvider(crids[i], 'recommendations');
                        if (odItem.isPlayAvailable()) {
                            addAITQueryToQueryList(odItem);
                        }
                    }
                    hx.log('debug', '[MDS] Recommendations! queryList.length : ' + queryList.length);
                }
                var fnSort = function (array) {
                    array.sort(function (a, b) {
                        a = a instanceof Array ? a[0] : a;
                        b = b instanceof Array ? b[0] : b;
                        return a.index - b.index;
                    });
                };
                var processRetObj = function () {
                    if (queryList.length - odProgs.length) {
                        hx.log('debug', '[MDS] Recommendations! missing AIT list length : ' + (queryList.length - odProgs.length));
                    }
                    var resultLength = odProgs.length;
                    if (resultLength) {
                        fnSort(odProgs);
                        if (updateUICb) {
                            updateUICb(groupId, odProgs);
                        }
                    }
                    else {
                        if (updateUICb) {
                            updateUICb(groupId, null);
                        }
                    }
                    odProgs = null;
                };
                hx.svc.IP_EPG.getAITList(queryList).then(function () {
                    hx.log('debug', '[MDS] Recommendations! Success? odProgs.length : ' + odProgs.length);
                    processRetObj();
                }, function () {
                    hx.log('debug', '[MDS] Recommendations! Fail? odProgs.length : ' + odProgs.length);
                    processRetObj();
                });
            }, function () {
                if (updateUICb) {
                    updateUICb(groupId, null);
                }
            });
        };
        CPgTopPicks.prototype.getSchedule = function (channel, startTime, prog, width, updateCb) {
            if (channel) {
                var splitStr = channel.ccid.split('.');
                if (splitStr.length > 0) {
                    var sid = splitStr[splitStr.length - 1];
                    hx.svc.IP_EPG.getSchedules(sid, startTime * 1000, 6).then(function (res) {
                        var info = hx.svc.IP_EPG.parse.xml.schedule(res);
                        var schedules = Array.prototype.slice.call(info.schedules);
                        var crids = info.crids;
                        var length = 0;
                        var parsedId;
                        var onNowLegacyId, mdsLegacyId;
                        var mdsEvent;
                        var schedule = null, crid = null, data = null;
                        var checkThumbnailRequest = false;
                        if (schedules) {
                            length = schedules.length;
                        }
                        if (prog && prog.getProgrammeID) {
                            parsedId = hx.svc.IP_EPG.parse.util.programIdParser(prog.getProgrammeID());
                            onNowLegacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                            if (onNowLegacyId) {
                                for (var i = 0; i < length; i++) {
                                    schedule = schedules[i];
                                    mdsEvent = hx.svc.IP_EPG.parse.element.scheduleEvent(schedule);
                                    parsedId = hx.svc.IP_EPG.parse.util.programIdParser(mdsEvent.programURL);
                                    mdsLegacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                                    if (onNowLegacyId === mdsLegacyId) {
                                        crid = schedule.querySelector('Program').getAttribute('crid');
                                        data = crids[crid];
                                        data.schedule = schedule;
                                        prog.setMdsData({
                                            mdsData: data,
                                            launchLocation: 'recommendations'
                                        });
                                        prog.getThumbnail({
                                            width: width,
                                            updateCb: updateCb
                                        });
                                        checkThumbnailRequest = true;
                                    }
                                }
                            }
                        }
                        if (!checkThumbnailRequest) {
                            updateCb(prog.getDefaultThumbnail());
                        }
                    }, function () {
                        updateCb(prog.getDefaultThumbnail());
                    });
                }
                else {
                    updateCb(null);
                }
            }
            else {
                updateCb(null);
            }
        };
        CPgTopPicks.prototype.searchProgrammes = function (aType, data, updateCb) {
            if (!data) {
                hx.log('error', '[searchProgrammes] (!data)');
                updateCb(null);
                return;
            }
            if (!updateCb) {
                hx.log('error', '[searchProgrammes] (!updateCb)');
                updateCb(null);
                return;
            }
            var that = this;
            var MAX_RECOMMENDATION_COUNT = 15;
            var recommendations = data.events;
            var recommendationsLength = recommendations ? recommendations.length : 0;
            if (!recommendationsLength) {
                hx.log('error', '[searchProgrammes] (!eventsLength)');
                updateCb(null);
                that._processingHmxContents[aType] = false;
                return;
            }
            var eventTimes = null;
            var channelsLength = 0;
            var retArr = [];
            var requestCnt = 0;
            var channels = hx.svc.CH_LIST.getChlist({
                group: "TV"
            }).chList;
            var channelIds = channels.map(function (aChannel) {
                if (!aChannel.onid || !aChannel.sid) {
                    return '';
                }
                return [aChannel.onid.toString(16), aChannel.sid.toString(16)].join('.');
            });
            var fnHandleUpdateCb = function () {
                if (!that._hmxContents) {
                    hx.log('error', '[searchProgrammes/fnHandleUpdateCb] (!that._hmxContents) already destroyed..!');
                    return;
                }
                retArr.sort(function (a, b) {
                    return a.getStartTime() - b.getStartTime();
                });
                updateCb(that._hmxContents[aType] = retArr, aType);
                that._processingHmxContents[aType] = false;
                recommendations = null;
                retArr = null;
            };
            var fnSearchProgramme = function (targetOne) {
                if (requestCnt >= recommendationsLength) {
                    hx.log('debug', '[searchProgrammes] (requestCnt > recommendationsLength)');
                    fnHandleUpdateCb();
                    return;
                }
                eventTimes = targetOne.eventTimes;
                if (!eventTimes) {
                    hx.log('error', '[searchProgrammes] (!eventTimes)');
                    fnSearchProgramme(recommendations[++requestCnt]);
                    return;
                }
                var compareProgID = '';
                for (var i = 0, len = eventTimes.length, aEvent; i < len; i += 1) {
                    aEvent = eventTimes[i];
                    channelsLength = aEvent.channels ? aEvent.channels.length : 0;
                    if (!channelsLength) {
                        hx.log('error', '[searchProgrammes] (!channelsLength)');
                        fnSearchProgramme(recommendations[++requestCnt]);
                        return;
                    }
                    compareProgID = (function (aTargetEvent) {
                        var targetChannels = aTargetEvent.channels;
                        var channelIdx = -1;
                        for (var targetChannel, targetIdx = 0, len = targetChannels.length; targetIdx < len; targetIdx += 1) {
                            targetChannel = targetChannels[targetIdx];
                            if (!targetChannel.onid || !targetChannel.sid || !targetChannel.eventID) {
                                continue;
                            }
                            channelIdx = channelIds.indexOf([targetChannel.onid.toString(16), targetChannel.sid.toString(16)].join('.'));
                            if (channelIdx >= 0) {
                                break;
                            }
                        }
                        if (channelIdx === -1) {
                            hx.log('error', '[searchProgrammes] There is no matched channel of this event : ' + aTargetEvent.eventName);
                            return '';
                        }
                        var targetTsID = channels[channelIdx].tsid;
                        if (!targetTsID) {
                            hx.log('error', '[searchProgrammes] (!targetTsID) of this event : ' + aTargetEvent.eventName);
                            return '';
                        }
                        var targetSvcUID = channels[channelIdx].ccid.split(":")[1].split(".")[0];
                        return hx.util.format("dvb://{0}.{1}.{2};{3};;{4}", targetChannel.onid.toString(16), targetTsID.toString(16), targetChannel.sid.toString(16), targetChannel.eventID.toString(16), targetSvcUID ? Number(targetSvcUID).toString(16) : '');
                    })(aEvent);
                    if (compareProgID) {
                        hx.log('debug', '[searchProgrammes] compareProgID : ' + compareProgID);
                        break;
                    }
                }
                if (!compareProgID) {
                    hx.log('error', '[searchProgrammes] (!compareProgID) : ' + aEvent.eventName);
                    fnSearchProgramme(recommendations[++requestCnt]);
                    return;
                }
                requestCnt += 1;
                setTimeout(function () {
                    if (!targetOne.eventName) {
                        hx.log('error', '[searchProgrammes] (!targetOne.eventName)');
                        if (requestCnt >= MAX_RECOMMENDATION_COUNT) {
                            fnHandleUpdateCb();
                        }
                        else {
                            fnSearchProgramme(recommendations[requestCnt]);
                        }
                        return;
                    }
                    hx.svc.EPG.findProgrammes({
                        clause: 'programmeID == ' + compareProgID,
                        cb: function (result) {
                            var programmes = result ? result[0] : null;
                            var programmesLength = programmes ? programmes.length : null;
                            if (!programmesLength) {
                                hx.log('error', '[searchProgrammes] (!programmesLength)' + targetOne.eventName + ', throw it away!');
                                if (requestCnt >= MAX_RECOMMENDATION_COUNT) {
                                    fnHandleUpdateCb();
                                }
                                else {
                                    fnSearchProgramme(recommendations[requestCnt]);
                                }
                                return;
                            }
                            for (var j = 0; j < programmesLength; j += 1) {
                                var key = (function (aList, aItem) {
                                    var i, ti, index = -1;
                                    for (i = 0, ti = aList.length; i < ti; i += 1) {
                                        if ((aItem.getProgrammeID() && (aList[i].getProgrammeID() === aItem.getProgrammeID())) || (aItem.getSeriesID() && (aList[i].getSeriesID() === aItem.getSeriesID())) || (aItem.getProgrammeCRID() && (aList[i].getProgrammeCRID() === aItem.getProgrammeCRID()))) {
                                            index = i;
                                            break;
                                        }
                                    }
                                    return index;
                                })(retArr, programmes[j]);
                                if (key === -1) {
                                    retArr.push(programmes[j]);
                                }
                                else {
                                    hx.log('error', '[searchProgrammes] already got this programme : [' + programmes[j].getName() + '], throw it away!');
                                }
                            }
                            if (retArr.length >= MAX_RECOMMENDATION_COUNT) {
                                hx.log('CPgToppicks', '[searchProgrammes] got recommendations completely');
                                fnHandleUpdateCb();
                            }
                            else {
                                fnSearchProgramme(recommendations[requestCnt]);
                            }
                        }
                    });
                }, 0);
            };
            fnSearchProgramme(recommendations[0]);
        };
        CPgTopPicks.prototype.showProgrammeDetail = function (aProgrammeDetailItem) {
            var that = this;
            var parentLayer = that._page;
            that._detailDlg = hx.il.createProgrammeDetailForSugar({
                layeredGroupControl: parentLayer,
                sugarPage: that,
                pdeItem: aProgrammeDetailItem,
                fnDoShow: function () {
                    parentLayer._$self.show();
                },
                fnDoHide: function () {
                    parentLayer._$self.hide();
                },
                onClosed: function () {
                    delete that._detailDlg;
                    that._detailDlg = null;
                }
            });
        };
        CPgTopPicks.prototype._on_alKey = function (param) {
            var that = this;
            var keyCode = param.alKey;
            var consumed = that._page.doKey(keyCode);
            if (!consumed) {
                switch (keyCode) {
                    case hx.key.KEY_BACK:
                    case hx.key.KEY_ESCAPE:
                    case hx.key.KEY_ON_DEMAND:
                        that._exit_to_live();
                        consumed = true;
                        break;
                    default:
                        break;
                }
            }
            return consumed;
        };
        CPgTopPicks.prototype._exit_to_live = function () {
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController',
                me: this
            });
        };
        return CPgTopPicks;
    })(__PAGE_KERNEL__);
    hx.al.CPgTopPicks = CPgTopPicks;
    return CPgTopPicks;
});
