var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "model/model", 'pages/util/controls', 'pages/util/provider'], function (require, exports, __PAGE_KERNEL__, __MODEL__, __CONTROLS__, __PROVIDER__) {
    /// <reference path="../../def/sugar.d.ts" />
    /// <reference path="../../def/controls.d.ts" />
    ///<reference path="../../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />
    ///<reference path="../../layer_domain/service/ip_epg/adapter.d.ts" />
    var state = 'infoHub';
    var focusedChannel = null;
    var focusedProgramme = null;
    var createDateTimeIfNecessary = function (a) {
        if (!a._$dateTimePane) {
            var $dateTimePane = $('<div>', {
                'class': 'dateTimePane'
            }), $date = $('<div>', {
                'class': 'date'
            }).appendTo($dateTimePane), $time = $('<div>', {
                'class': 'time'
            }).appendTo($dateTimePane);
            $dateTimePane.$date = $date;
            $dateTimePane.$time = $time;
            a._$dateTimePane = $dateTimePane;
        }
        a._$self.append(a._$dateTimePane);
    };
    var createInfoHubIfNecessary = function (a) {
        if (!a._$infoHubPane) {
            var $infoHubPane = $('<div>', {
                'id': 'entryPane'
            }), $arrowUp = $('<div>', {
                'class': 'arrowUp'
            }).appendTo($infoHubPane), $highlight = $('<div>', {
                'class': 'highlight'
            }).appendTo($infoHubPane), $arrowRight = $('<div>', {
                'class': 'arrowRight'
            }).appendTo($highlight), $channelItem = $('<div>', {
                'class': 'chItem'
            }).appendTo($highlight), $channelNum = $('<div>', {
                'class': 'chNum'
            }).appendTo($channelItem), $channelName = $('<div>', {
                'class': 'chName'
            }).appendTo($channelItem), $channelItemDummy = $('<div>', {
                'class': 'chItem'
            }).appendTo($infoHubPane), $programmeName = $('<div>', {
                'class': 'progName'
            }).appendTo($infoHubPane), $programmeProgress = $('<div>', {
                'class': 'progProgress'
            }).appendTo($infoHubPane), $programmeStartTime = $('<div>', {
                'class': 'progStartTime'
            }).appendTo($programmeProgress), $programmeEndTime = $('<div>', {
                'class': 'progEndTime'
            }).appendTo($programmeProgress), $programmeBar = $('<div>', {
                'class': 'progBar'
            }).appendTo($programmeProgress), $programmePos = $('<div>', {
                'class': 'progPos'
            }).appendTo($programmeBar), $arrowDown = $('<div>', {
                'class': 'arrowDown'
            }).appendTo($infoHubPane);
            $infoHubPane.$arrows = $arrowUp.add($arrowDown).add($arrowRight);
            $infoHubPane.$channelNum = $channelNum;
            $infoHubPane.$channelName = $channelName;
            $infoHubPane.$programmeProgress = $programmeProgress;
            $infoHubPane.$programmeName = $programmeName;
            $infoHubPane.$programmeStartTime = $programmeStartTime;
            $infoHubPane.$programmeEndTime = $programmeEndTime;
            $infoHubPane.$programmePos = $programmePos;
            $infoHubPane.$programmeBar = $programmeBar;
            $infoHubPane.$highlight = $highlight;
            a._$infoHubPane = $infoHubPane;
        }
        a._$self.append(a._$infoHubPane);
    };
    var createInfoHubIconIfNecessary = function (a) {
        if (!a._$statusPan) {
            var $statusPan = $('<div>', {
                'class': 'statusPan'
            }), $statusText1 = $('<p>', {
                'class': 'statusText'
            }).appendTo($statusPan), $statusText2 = $('<p>', {
                'class': 'statusText'
            }).appendTo($statusPan);
            $statusPan.$statusText1 = $statusText1;
            $statusPan.$statusText2 = $statusText2;
            a._$statusPan = $statusPan;
        }
        a._$self.append(a._$statusPan);
    };
    var createTimeBarIfNecessary = function (a) {
        if (!a._$timeBarEntry) {
            var $timeBarEntry = $('<div>', {
                'class': 'timeBarEntry'
            });
            var $timeBar = $('<div>', {
                'class': 'timeBar'
            }).appendTo($timeBarEntry);
            var $timeBar2 = $('<div>', {
                'class': 'timeBar2'
            }).appendTo($timeBar);
            var $timeBarStart = $('<div>', {
                'class': 'timeBarStart'
            }).appendTo($timeBar);
            var $timeBarCursor = $('<p>', {
                'class': 'timeBarCursor'
            }).appendTo($timeBar);
            var $timeBarDate = $('<em>', {
                'class': 'timeBarDate'
            }).appendTo($timeBarCursor);
            var $catchUp = $('<div>', {
                'class': 'catchupInfo'
            }).appendTo($timeBarEntry);
            var $catchUp2 = $('<div>', {
                'class': 'arrIco'
            }).appendTo($catchUp);
            if (hx.config.useBackwardEpg === true) {
                var $catchUp3 = $('<div>', {
                    'class': 'arrTxt',
                    'text': hx.l('LOC_CATCHUP_VIEW_ID')
                }).appendTo($catchUp);
            }
            else {
                var $catchUp3 = $('<div>', {
                    'class': 'arrTxt',
                    'text': hx.l('LOC_CRECORDINGS_ID')
                }).appendTo($catchUp);
            }
            $timeBarEntry.$timeBar = $timeBar;
            $timeBarEntry.$timeBar2 = $timeBar2;
            $timeBarEntry.$timeBarStart = $timeBarStart;
            $timeBarEntry.$timeBarCursor = $timeBarCursor;
            $timeBarEntry.$timeBarDate = $timeBarDate;
            $timeBarEntry.$catchUp = $catchUp;
            $timeBarEntry.$catchUp2 = $catchUp2;
            $timeBarEntry.$catchUp3 = $catchUp3;
            a._$timeBarEntry = $timeBarEntry;
        }
        a._$self.append(a._$timeBarEntry);
    };
    var createCursorIfNecessary = function (a) {
        if (!a._$cursorEntry) {
            var $cursorEntry = $('<div>', {
                'class': 'cursor'
            });
            a._$cursorEntry = $cursorEntry;
        }
        a._$self.append(a._$cursorEntry);
    };
    var removeDateTime = function (a) {
        if (a._$dateTimePane) {
            a._$dateTimePane.detach();
            hx.svc.CLOCK.stopClock();
        }
    };
    var removeInfoHub = function (a) {
        if (a._$infoHubPane) {
            a._$infoHubPane.detach();
        }
    };
    var removeInfoHubIcon = function (a) {
        if (a._$statusPan) {
            a._$statusPan.detach();
        }
    };
    var removeTimeBar = function (a) {
        if (a._$timeBarEntry !== undefined) {
            a._$timeBarEntry.detach();
        }
    };
    var removeCursor = function (a) {
        if (a._$cursorEntry !== undefined) {
            a._$cursorEntry.detach();
        }
    };
    var updateEntryPane = function (a) {
        if (a._$infoHubPane !== undefined) {
            if (state === 'channelList') {
                a._$infoHubPane.$programmeName.detach();
                a._$infoHubPane.$programmeProgress.detach();
                a._$infoHubPane.$arrows.removeClass('show');
                a._$infoHubPane.$highlight.removeClass('on');
            }
            else if (state === 'iPlate') {
                if (!a._$infoHubPane.find('.progName').length) {
                    a._$infoHubPane.$programmeName[0].innerHTML = '';
                    a._$infoHubPane.append(a._$infoHubPane.$programmeName);
                }
                if (!a._$infoHubPane.find('.progProgress').length) {
                    //a._$infoHubPane.$programmeProgress[0].innerHTML = '';
                    a._$infoHubPane.append(a._$infoHubPane.$programmeProgress);
                }
                a._$infoHubPane.$arrows.removeClass('show');
            }
            else {
                if (!a._$infoHubPane.find('.progName').length) {
                    a._$infoHubPane.append(a._$infoHubPane.$programmeName);
                }
                if (!a._$infoHubPane.find('.progProgress').length) {
                    a._$infoHubPane.append(a._$infoHubPane.$programmeProgress);
                }
                a._$infoHubPane.$arrows.addClass('show');
                a._$infoHubPane.$highlight.addClass('on');
            }
        }
    };
    var updateDateTime = function (a) {
        if (!a._$dateTimePane) {
            return;
        }
        var now = new Date();
        var date = dateFormat(now, "dd.mm.yyyy");
        var time = dateFormat(now, "ddd HH:MM").toUpperCase();
        a._$dateTimePane.$date.text(date);
        a._$dateTimePane.$time.text(time);
    };
    var updateCurrentChannel = function (a) {
        if (a._currentChannel) {
            var channelNum = hx.il.makeDigitNumber(a._currentChannel.majorChannel);
            a._$infoHubPane.$channelNum.text(channelNum);
            a._$infoHubPane.$channelName.text(a._currentChannel.name);
        }
    };
    var updateCurrentProgramme = function (a) {
        // check icon
        var isCas = false;
        var isLock = false, bPrismLiteSMode = a.isPrismLiteSModeOn();
        var channel = a._currentChannel, casInfo = '';
        var evtSchedule = '';
        if (channel) {
            if (channel.manualBlock === true) {
                isLock = true;
            }
            if (channel.casIDs) {
                if (channel.casIDs.length > 0) {
                    casInfo = a.getCasInformation();
                    isCas = true;
                }
            }
        }
        if ((bPrismLiteSMode === true) && (a._currentProgramme)) {
            var curP = a._currentProgramme;
            if (curP.getPluginObject()) {
                if (curP.hasReminder()) {
                    evtSchedule = '<div class="scheduleReminder"></div>';
                }
                else if (curP.hasSchedule()) {
                    evtSchedule = '<div class="scheduleRecord"></div>';
                }
            }
        }
        var chCas = (isCas === true) ? '<div class="' + casInfo + '"></div>' : '';
        var chLock = (isLock === true) ? '<div class="channelLock"></div>' : '';
        var space = (evtSchedule + chCas + chLock !== '') ? ' ' : '';
        if (a._currentProgramme) {
            var p = a._currentProgramme;
            var sT = 0;
            var eT = 0;
            var names = '';
            var startTime = '';
            var endTime = '';
            var programmeDate;
            if (p.catchup) {
                if (p.noCatchup) {
                }
                else if (hx.config.useBackwardEpg === true) {
                    sT = p.startTime;
                    eT = p.startTime + p.duration;
                    names = p.name;
                    startTime = dateFormat(new Date(sT * 1000), "ddd dd.mm.yyyy / HH:MM") + ' - ' + dateFormat(new Date(eT * 1000), "HH:MM");
                    var duration = p.duration;
                    var hour = Math.floor(duration / 3600);
                    var min = Math.floor((duration % 3600) / 60);
                    if (hour !== 0 && min !== 0) {
                        startTime += ' (' + hour + 'h ' + min + 'm)';
                    }
                    else if (hour !== 0) {
                        startTime += ' (' + hour + 'h)';
                    }
                    else {
                        startTime += ' (' + min + 'm)';
                    }
                }
                else {
                    sT = p.getStartTime();
                    eT = p.getEndTime();
                    names = p.getTitle();
                    var dur = p.getDuration();
                    var h = Math.floor((dur / 3600)).toString() + 'h';
                    var m = Math.floor((dur % 3600 + 30) / 60).toString() + 'm';
                    // +30 is round off
                    startTime = h + m + ' / ' + dateFormat(new Date(sT), "ddd.dd.mm.yyyy");
                }
            }
            else {
                sT = p.getStartTime();
                eT = p.getEndTime();
                names = p.getName();
                if (p.isLive() === true || p.isPast() === true) {
                    startTime = dateFormat(new Date(sT * 1000), "HH:MM");
                    endTime = dateFormat(new Date(eT * 1000), "HH:MM");
                }
                else {
                    startTime = dateFormat(new Date(sT * 1000), "HH:MM") + ' - ' + dateFormat(new Date(eT * 1000), "HH:MM");
                    if (bPrismLiteSMode === true) {
                        programmeDate = dateFormat(new Date(sT * 1000), "ddd. dd.mm").toUpperCase();
                        startTime = startTime + '  ' + programmeDate;
                    }
                }
            }
            a._$infoHubPane.$programmeName[0].innerHTML = evtSchedule + chLock + chCas + space + names;
            a._$infoHubPane.$programmeStartTime.text(startTime);
            a._$infoHubPane.$programmeEndTime.text(endTime);
        }
        else {
            a._$infoHubPane.$programmeName[0].innerHTML = chLock + chCas;
            a._$infoHubPane.$programmeStartTime.text('');
            a._$infoHubPane.$programmeEndTime.text('');
        }
        updateCurrentProgress(a);
        updateTimeBarPosition(a);
        updateCursor(a);
    };
    var updateCurrentProgress = function (a) {
        if (a._currentProgramme) {
            var p = a._currentProgramme;
            var pos = 0;
            var needProgressbar = false;
            if (p.catchup) {
                if (p.noCatchup) {
                    // no recordings
                    var noWork;
                }
                else {
                    pos = 0;
                }
            }
            else {
                pos = p.getPercentage();
                if (!!p.isLive && p.isLive() === true) {
                    needProgressbar = true;
                }
                if (p.isPast() === true) {
                    needProgressbar = true;
                    if (hx.svc.TSR.isTsrOngoing() === true) {
                        var TSRstartTime = hx.svc.TSR.getStartTime();
                        var TSRplaybackOffset = hx.svc.TSR.getPlaybackOffset();
                        var TSRTime = TSRstartTime + TSRplaybackOffset;
                        //TSRTime  =  ((new Date()).getTime() / 1000) - (60*30); // test only
                        pos = p.getPercentageByTime(TSRTime);
                    }
                }
            }
            pos = (pos > 100) ? 100 : pos;
            pos = (pos < 0) ? 0 : pos;
            a._$infoHubPane.$programmePos.width(pos + '%');
            if (needProgressbar === true) {
                a._$infoHubPane.$programmeBar.addClass('show');
                if (!!a._$timeBarEntry && !!a._$timeBarEntry.$catchUp) {
                    a._$timeBarEntry.$catchUp.addClass('show');
                }
            }
            else {
                a._$infoHubPane.$programmeBar.removeClass('show');
                if (!!a._$timeBarEntry && !!a._$timeBarEntry.$catchUp) {
                    a._$timeBarEntry.$catchUp.removeClass('show');
                }
            }
        }
        else {
            a._$infoHubPane.$programmeBar.removeClass('show');
            if (!!a._$timeBarEntry && !!a._$timeBarEntry.$catchUp) {
                a._$timeBarEntry.$catchUp.removeClass('show');
            }
        }
    };
    var updateTimeBarPosition = function (a) {
        var loc = ['left:0px', 'left:288px', 'left:524px', 'left:760px', 'left:996px', 'left:1232px'];
        var timeBarLoc, i, drawnItems;
        var now = new Date();
        var ct = Math.floor(now.valueOf() / 1000);
        var p = a._currentProgramme;
        if (a._currentProgramme) {
            // check live
            if (p.catchup === undefined) {
                if (ct >= p.getStartTime() && ct <= p.getEndTime()) {
                    timeBarLoc = 0;
                }
            }
            // find position
            if (timeBarLoc === undefined && a._iPlate) {
                if (a._iPlate._drawnItems) {
                    drawnItems = a._iPlate._drawnItems.getItemsByArray();
                    if (drawnItems) {
                        for (i = 0; i < drawnItems.length; i += 1) {
                            if (drawnItems[i].find('.iPlateItemIcoLive').length) {
                                timeBarLoc = i;
                                timeBarLoc = timeBarLoc - a._iPlate._currentIndex;
                                timeBarLoc = (timeBarLoc > 4) ? undefined : timeBarLoc;
                                timeBarLoc = (timeBarLoc < 0) ? 0 : timeBarLoc;
                                break;
                            }
                        }
                    }
                }
            }
            // check schedule
            if (timeBarLoc === undefined) {
                if (p.catchup === undefined) {
                    if (ct < p.getEndTime()) {
                        timeBarLoc = 0;
                    }
                }
            }
        }
        if (a._currentProgramme && timeBarLoc !== undefined) {
            var startTime;
            if (p.catchup) {
                if (p.noCatchup) {
                }
                else {
                    startTime = ct;
                }
                startTime = ct;
            }
            else {
                startTime = p.getStartTime();
            }
            var pos = (startTime - ct) / (3600 * 24 * 8) * 100;
            // 8 days
            pos = Math.floor(pos);
            pos = (pos > 100) ? 100 : pos;
            pos = (pos < 0) ? 0 : pos;
            var pro = 'left:' + (pos * 0.88) + '%';
            var proW = 'width:' + (pos * 0.88) + '%';
            // 0.88 : to avoid hiding date
            var strStartTime = dateFormat(new Date(startTime * 1000), "ddd dd.mm");
            if (a._$timeBarEntry) {
                if (!!p.isLive && p.isLive()) {
                    a._$timeBarEntry.$timeBarCursor.addClass('onAir');
                    a._$timeBarEntry.$timeBarDate.addClass('onAir');
                }
                else {
                    a._$timeBarEntry.$timeBarCursor.removeClass('onAir');
                    a._$timeBarEntry.$timeBarDate.removeClass('onAir');
                    a._$timeBarEntry.$timeBarDate.text(strStartTime);
                }
                a._$timeBarEntry.$timeBar[0].style.cssText = loc[timeBarLoc];
                a._$timeBarEntry.$timeBar2[0].style.cssText = proW;
                a._$timeBarEntry.$timeBarCursor[0].style.cssText = pro;
                a._$timeBarEntry.$timeBar.addClass('show');
            }
        }
        else {
            if (a._$timeBarEntry) {
                a._$timeBarEntry.$timeBar.removeClass('show');
            }
        }
    };
    var updateCursor = function (a) {
        if (a._currentProgramme) {
            if (a._$cursorEntry) {
                a._$cursorEntry.addClass('show');
            }
        }
        else {
            if (a._$cursorEntry) {
                a._$cursorEntry.removeClass('show');
            }
        }
    };
    var updateStatusIcon = function (a) {
        var rec = '', liv = '';
        if (a._$statusPan) {
            if (a._recordings) {
                var len = a._recordings.length;
                if (len > 0) {
                    var recIcon = '<em class="statusIconRec"></em>';
                    rec = recIcon + len + ' ' + hx.l('LOC_RECORDING_ID');
                }
            }
            a._$statusPan.$statusText1[0].innerHTML = rec;
            var streamingList = hx.svc.RECORDINGS.getAllStreamingList();
            if (streamingList.length > 0) {
                var livIcon = '<em class="statusIconLiv"></em>';
                liv = livIcon + streamingList.length + ' ' + hx.l('LOC_LIVESTREAMING_ID');
            }
            a._$statusPan.$statusText2[0].innerHTML = liv;
        }
    };
    var checkChannelsChangeable = function (viewingItemInfos) {
        var i, channelChangable;
        viewingItemInfos = viewingItemInfos instanceof Array ? viewingItemInfos : [viewingItemInfos];
        for (i = 0; i < viewingItemInfos.length; i += 1) {
            var viewingItemInfo = viewingItemInfos[i];
            if (viewingItemInfo) {
                channelChangable = hx.svc.CH_LIST.checkChannelChangable({
                    channel: viewingItemInfo.item
                });
                if (viewingItemInfo.$item) {
                    if (channelChangable) {
                        viewingItemInfo.$item.removeClass('dis');
                    }
                    else {
                        viewingItemInfo.$item.addClass('dis');
                    }
                }
            }
        }
    };
    var $recIcon = $('<div>', {
        'class': 'chIconRec'
    });
    var TScheduledFactoryType;
    (function (TScheduledFactoryType) {
        TScheduledFactoryType[TScheduledFactoryType["ERECORDING"] = 0] = "ERECORDING";
        TScheduledFactoryType[TScheduledFactoryType["EREMIND"] = 1] = "EREMIND";
        TScheduledFactoryType[TScheduledFactoryType["ESERIES_RECORDING"] = 10] = "ESERIES_RECORDING";
    })(TScheduledFactoryType || (TScheduledFactoryType = {}));
    var CMainGroupControl = (function (_super) {
        __extends(CMainGroupControl, _super);
        function CMainGroupControl($el, parent) {
            _super.call(this, $el);
            this._parent = parent;
            this.createChannelList();
            this.createIPlate();
            this.setChildControl([]);
        }
        CMainGroupControl.prototype.destroy = function () {
            var a = this;
            if (a._iPlate) {
                if (a._iPlate._dataProvider) {
                    a._iPlate._dataProvider.destroy();
                    delete a._iPlate._dataProvider;
                    a._iPlate._dataProvider = undefined;
                }
                a._iPlate.destroy();
                delete a._iPlate;
                a._iPlate = undefined;
            }
            if (a._viewIPlate) {
                a._viewIPlate.destroy();
                delete a._viewIPlate;
                a._viewIPlate = undefined;
            }
            if (a._channelList) {
                if (a._channelList._rollDataProvider) {
                    a._channelList._rollDataProvider.destroy();
                    delete a._channelList._rollDataProvider;
                    a._channelList._rollDataProvider = undefined;
                }
                a._channelList.destroy();
                delete a._channelList;
                a._channelList = undefined;
            }
            if (a._channelListData) {
                delete a._channelListData;
                a._channelListData = undefined;
            }
            if (a._channelListGroupOptBtn) {
                delete a._channelListGroupOptBtn;
                a._channelListGroupOptBtn = undefined;
            }
            if (a._parent) {
                delete a._parent;
                a._parent = undefined;
            }
            if (a._currentChannel) {
                delete a._currentChannel;
                a._currentChannel = undefined;
            }
            if (a._currentProgramme) {
                delete a._currentProgramme;
                a._currentProgramme = undefined;
            }
            if (a._updatedprogrammes) {
                delete a._updatedprogrammes;
                a._updatedprogrammes = undefined;
            }
            if (a._receivedProgrammes) {
                delete a._receivedProgrammes;
                a._receivedProgrammes = undefined;
            }
            if (a._receivedCatchUps) {
                delete a._receivedCatchUps;
                a._receivedCatchUps = undefined;
            }
            if (a._updatedCatchUps) {
                delete a._updatedCatchUps;
                a._updatedCatchUps = undefined;
            }
            if (a._receivedBackwards) {
                delete a._receivedBackwards;
                a._receivedBackwards = undefined;
            }
            if (a._updatedBackwards) {
                delete a._updatedBackwards;
                a._updatedBackwards = undefined;
            }
            if (focusedProgramme) {
                focusedProgramme = undefined;
            }
            if (focusedChannel) {
                focusedChannel = undefined;
            }
            if (a._$dateTimePane) {
                a._$dateTimePane.remove();
                delete a._$dateTimePane;
                a._$dateTimePane = undefined;
            }
            if (a._$infoHubPane) {
                a._$infoHubPane.remove();
                delete a._$infoHubPane;
                a._$infoHubPane = undefined;
            }
            if (a._$statusPan) {
                a._$statusPan.remove();
                delete a._$statusPan;
                a._$statusPan = undefined;
            }
            if (a._$timeBarEntry) {
                a._$timeBarEntry.remove();
                delete a._$timeBarEntry;
                a._$timeBarEntry = undefined;
            }
            if (a._$cursorEntry) {
                a._$cursorEntry.remove();
                delete a._$cursorEntry;
                a._$cursorEntry = undefined;
            }
            _super.prototype.destroy.call(a);
        };
        CMainGroupControl.prototype.createChannelList = function () {
            var a = this;
            var b = this._parent;
            var channelList = new __CONTROLS__.CCarouselControl(null);
            channelList.setId('channelList');
            channelList.setDrawParam({
                viewCount: 7,
                anchorIndex: 2,
                itemHeight: 54,
                anchorHeight: 270,
                align: 'vertical',
                animation: true,
                animationInterval: 0.15,
                maxKeyQueueCount: 1,
                transparentAnchor: true
            });
            channelList.setAnchorDrawer(function (param) {
                var that = this, bPrismLiteSMode = false;
                bPrismLiteSMode = a.isPrismLiteSModeOn();
                if (bPrismLiteSMode === true) {
                    that.drawPrismLiteSChannelList(param);
                }
                else {
                    that.drawPrismChannelList(param);
                }
            });
            channelList.setDataDrawer(function (param) {
                var channelNum = '', channelName = '';
                if (param.item) {
                    channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                    channelName = param.item.name;
                }
                param.$item.addClass('chItem');
                param.$item.append($('<div>', {
                    'class': 'chNum'
                }).text(channelNum));
                param.$item.append($('<div>', {
                    'class': 'chName'
                }).text(channelName));
                checkChannelsChangeable(param);
            });
            channelList.onFocusStartToChange = function (param) {
                if (!param.item) {
                    a.showChannelList();
                    // handle an execption
                    return;
                }
                var channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                if (a._$infoHubPane) {
                    a._$infoHubPane.$channelNum.text(channelNum);
                    a._$infoHubPane.$channelName.text(param.item.name);
                }
                focusedChannel = a._currentChannel = param.item;
                b._setCurrentChannel(focusedChannel);
                var bPrismLiteSMode = a.isPrismLiteSModeOn();
                if (bPrismLiteSMode === true) {
                    // initialize
                    a._channelList._$anchor.find('.chEvtName').html('');
                    a._channelList._$anchor.find('.chEvtTime').html('');
                    a._channelList._$anchor.find('.chThumbnail').html('');
                }
                else {
                    // initialize
                    var defaultThum = '<img src="' + a.getDefaultThumbnail() + '">';
                    a._channelList._$anchor.find('.chEvtName').html('');
                    a._channelList._$anchor.find('.chThumbnail').html(defaultThum);
                }
                b._resetTimer();
            };
            channelList.onFocusChanged = function (param) {
                if (!param.item) {
                    a.showChannelList();
                    // handle an execption
                    return;
                }
                var channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                if (a._$infoHubPane) {
                    a._$infoHubPane.$channelNum.text(channelNum);
                    a._$infoHubPane.$channelName.text(param.item.name);
                    if (hx.config.useSlideText) {
                        b._doSlideAnimation(a._$infoHubPane.$channelName, param.item.name);
                    }
                }
                focusedChannel = a._currentChannel = param.item;
                focusedProgramme = undefined;
                b._getChListEvt(focusedChannel);
            };
            channelList.onItemSelected = function (param) {
                var that = this;
                var channel = that._rollDataProvider.curItem();
                var curObj = hx.svc.CH_LIST.getCurrentChannel();
                if ((curObj.result === true) && (curObj.channel.ccid === channel.ccid) && (curObj.group === b._currentGroupDB)) {
                    // exit list
                    if (b._bActivate) {
                        b._hide();
                    }
                    return;
                }
                // check tunable
                var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
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
                    return;
                }
                if (channel) {
                    hx.svc.AV_CTRL.channelChange({
                        channel: channel,
                        group: b._currentGroupDB
                    });
                    if (b._bActivate) {
                        b._hide();
                    }
                }
            };
            channelList.doKeyRec = function () {
                b._startRec(focusedChannel);
                return true;
            };
            channelList.drawPrismChannelList = function (param) {
                focusedChannel = a._currentChannel = param.currentItem.data;
                var channelNum = hx.il.makeDigitNumber(focusedChannel.majorChannel);
                var isCas, isLock;
                var programmeName = '', thumbnail = '', logo = '';
                var bRecording = false, thumbnailUrl = '';
                if (focusedChannel.casIDs) {
                    if (focusedChannel.casIDs.length > 0) {
                        isCas = true;
                    }
                }
                if (focusedChannel.manualBlock) {
                    isLock = true;
                }
                if (!!focusedProgramme) {
                    if (focusedProgramme.catchup === undefined && hx.config.useBackwardEpg !== true) {
                        thumbnail = '<img src="' + a.getDefaultThumbnail() + '">';
                    }
                    else {
                        if (!!focusedProgramme.getTitle) {
                            programmeName = focusedProgramme.getTitle();
                        }
                        if (focusedProgramme.getThumbnailUrl) {
                            thumbnailUrl = focusedProgramme.getThumbnailUrl();
                        }
                        else {
                            thumbnailUrl = a.getDefaultThumbnail();
                        }
                        thumbnail = '<img src="' + thumbnailUrl + '" ' + 'onerror="this.src=\'' + a.getDefaultThumbnail() + '\';" />';
                    }
                }
                else {
                    thumbnail = '<img src="' + a.getDefaultThumbnail() + '">';
                }
                var $channelItem = $('<div>', {
                    'class': 'chItem'
                }).append($('<div>', {
                    'class': 'chEvtName'
                }).text(programmeName)).append($('<div>', {
                    'class': 'chThumOutline'
                })).append($('<div>', {
                    'class': 'chThumbnail'
                }).html(thumbnail));
                if (a.checkRecordings(focusedChannel.ccid)) {
                    $channelItem.find('.chThumbnail').append($recIcon);
                }
                if (ENV.op != ENV.listOP.UKDTT) {
                    if (focusedChannel.logoURL) {
                        logo = '<img src="' + focusedChannel.logoURL + '"onerror="this.src=\'\';" />';
                        $channelItem.find('.chThumbnail').append($('<div>', {
                            'class': 'chLogo'
                        }).html(logo));
                    }
                }
                if (isLock) {
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': 'channelLock'
                    }));
                }
                if (isCas) {
                    var casInfo = a.getCasInformation();
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': casInfo
                    }));
                }
                param.$anchor.append($channelItem);
            };
            channelList.drawPrismLiteSChannelList = function (param) {
                focusedChannel = a._currentChannel = param.currentItem.data;
                var channelNum = hx.il.makeDigitNumber(focusedChannel.majorChannel), isCas, isLock;
                if (focusedChannel.casIDs) {
                    if (focusedChannel.casIDs.length > 0) {
                        isCas = true;
                    }
                }
                if (focusedChannel.manualBlock) {
                    isLock = true;
                }
                var programmeName = '', programmeTime = '';
                var thumbnail = '', bRecording = false;
                var $channelItem = $('<div>', {
                    'class': 'chItem'
                }).append($('<div>', {
                    'class': 'chEvtName'
                }).text(programmeName)).append($('<div>', {
                    'class': 'chEvtTime'
                }).text(programmeTime)).append($('<div>', {
                    'class': 'chThumbnail'
                }).html(thumbnail));
                if (a.checkRecordings(focusedChannel.ccid)) {
                    $channelItem.find('.chThumbnail').append($recIcon);
                }
                if (isLock) {
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': 'channelLock'
                    }));
                }
                if (isCas) {
                    var casInfo = a.getCasInformation();
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': casInfo
                    }));
                }
                param.$anchor.append($channelItem);
                return true;
            };
            a._channelList = channelList;
            a._TYPE = ['TV', 'RADIO'];
            a._IMAGE = ['images/212_Thumb_TV.png', 'images/212_Thumb_Radio.png'];
        };
        CMainGroupControl.prototype.createIPlate = function () {
            var a = this;
            var b = this._parent;
            var iPlate = new __CONTROLS__.CListControl(null);
            iPlate.setId('iPlate');
            iPlate.setDrawParam({
                align: 'horizontal',
                itemWidth: 236 //264 <- old
            });
            iPlate.setDataDrawer(function (param) {
                //param.$item.text(param.item.name);
                var isCatchUp = false;
                var isLive = false;
                var isPast = false;
                var isNewRec = false;
                var isLock = false;
                var isRecNow = false;
                var isReservation = false;
                var isReservationSeries = false;
                var isReminder = false;
                var scrambled = false;
                var isNoCatchup = false;
                var thumbnailImg;
                var availableIcon = param.item.raw ? param.item.raw.availableIconUrl : param.item.availableIconUrl;
                if (param.item.catchup) {
                    if (param.item.noCatchup) {
                        thumbnailImg = 'images/350_LTV_Norec_BG.png';
                        isNoCatchup = true;
                    }
                    else if (hx.config.useBackwardEpg === true) {
                        thumbnailImg = param.item.getField('thumbnailURL');
                        if (!thumbnailImg) {
                            thumbnailImg = a.getDefaultThumbnail();
                        }
                        isPast = true;
                    }
                    else {
                        var rec = param.item;
                        var makeThumbnail = function () {
                            var defaultThumbnail;
                            if (rec.isBroken()) {
                                defaultThumbnail = 'images/212_Thumb_REC_Fail.png';
                            }
                            else {
                                defaultThumbnail = 'images/212_Thumb_Radio.png';
                            }
                            if (rec.isLocked()) {
                                return defaultThumbnail;
                            }
                            return (rec.getThumbnailUrl() || defaultThumbnail);
                        };
                        thumbnailImg = makeThumbnail();
                        isCatchUp = true;
                        if (!!rec) {
                            if (rec.isNew()) {
                                isNewRec = true;
                            }
                            if (rec.isLocked()) {
                                isLock = true;
                            }
                            if (rec.isScrambled()) {
                                scrambled = true;
                            }
                        }
                    }
                }
                else {
                    thumbnailImg = param.item.getThumbnail();
                    var lastWrapper = getLastWrapper(param.item);
                    var now = new Date();
                    var ct = Math.floor(now.valueOf() / 1000);
                    var st = lastWrapper.startTime;
                    var et = lastWrapper.startTime + lastWrapper.duration;
                    if (ct >= st && ct <= et) {
                        isLive = true;
                        if (a.checkRecordings(lastWrapper.ccid) === true) {
                            isRecNow = true;
                        }
                    }
                    else if (et < ct) {
                        isPast = true;
                    }
                    else {
                        if (lastWrapper.getPluginObject()) {
                            if (lastWrapper.hasReminder()) {
                                isReminder = true;
                            }
                            else if (lastWrapper.hasSchedule()) {
                                isReservation = true;
                            }
                            else if (lastWrapper.hasSeriesSchedule()) {
                                isReservationSeries = true;
                            }
                        }
                    }
                }
                var $div = $('<div>', {
                    'class': 'iPlateItem',
                    'style': 'background-image : url(' + thumbnailImg + ');' + 'background-size: cover;' + 'background-repeat:no-repeat'
                });
                if (!!availableIcon) {
                    $div.append($('<img>', {
                        'class': 'availability',
                        'src': availableIcon
                    }));
                }
                if (isCatchUp === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoCatchup'
                    }));
                }
                if (isLive === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoLive'
                    }));
                }
                if (isPast === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoPast'
                    }));
                }
                if (isNewRec === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoNew'
                    }));
                }
                if (isRecNow === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoRec'
                    }));
                }
                if (isReservationSeries === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoRecRsvSer'
                    }));
                }
                else if (isReservation === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoRecRsv'
                    }));
                }
                else if (isReminder === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoReminder'
                    }));
                }
                if (isLock === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoLock'
                    }));
                }
                if (scrambled === true) {
                    $div.append($('<div>', {
                        'class': 'iPlateItemIcoScrambled'
                    }));
                }
                if (isNoCatchup === true) {
                    if (hx.config.useBackwardEpg === true) {
                        $div.append($('<div>', {
                            'class': 'iPlateItemNoRec',
                            'text': hx.l('LOC_NO_PROGRAMME_INFORMATION_ID')
                        }));
                    }
                    else {
                        $div.append($('<div>', {
                            'class': 'iPlateItemNoRec',
                            'text': hx.l('LOC_NO_RECORDINGS_IN_THIS_CHANNEL_ID')
                        }));
                    }
                }
                //param.$item.append($div);
                param.$item[0].innerHTML = $div[0].outerHTML;
                lastWrapper = undefined;
            });
            iPlate.onItemSelected = function (param) {
                var that = b;
                var dsrvDetailInfo2 = hx.svc.DETAIL_INFO;
                var dsrvRecordings2 = hx.svc.RECORDINGS;
                if (param.item.catchup) {
                    if (param.item.noCatchup) {
                        that.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgNotAvailable?show'
                        });
                        return;
                    }
                    that._$infoHub[0].style.display = 'none';
                    that._bActivate = false;
                    clearTimeout(that._timerId);
                    that._timerId = -1;
                    if (hx.config.useBackwardEpg === true) {
                        var programmeDetailItem = dsrvDetailInfo2.createProgrammeItem(param.item);
                        if (programmeDetailItem.setOipfProgrammeInstance) {
                            programmeDetailItem.setOipfProgrammeInstance(param.item);
                        }
                        hx.il.adhocPageParam.push({
                            programmeDetailItem: programmeDetailItem
                        });
                        that.sendEventToEntity({
                            'alEvent': 'alMessageToPage',
                            'target': 'CPgDetailInfo/infoHub',
                            'me': that
                        });
                    }
                    else {
                        var recordingDetailItem = dsrvDetailInfo2.createRecordingItem(param.item);
                        hx.il.adhocPageParam.push({
                            programmeDetailItem: recordingDetailItem
                        });
                        that.sendEventToEntity({
                            'alEvent': 'alMessageToPage',
                            'target': 'CPgDetailInfo/infoHub',
                            'me': that
                        });
                    }
                }
                else {
                    that._$infoHub[0].style.display = 'none';
                    that._bActivate = false;
                    clearTimeout(that._timerId);
                    that._timerId = -1;
                    var programmeDetailItem = dsrvDetailInfo2.createProgrammeItem(param.item.pluginObject);
                    if (param.item.raw) {
                        var mdsProgramme = hx.svc.IP_EPG.convert.to.programme(param.item.raw, param.item.channel, null);
                        programmeDetailItem.setOipfProgrammeInstance(mdsProgramme);
                    }
                    hx.il.adhocPageParam.push({
                        programmeDetailItem: programmeDetailItem
                    });
                    that.sendEventToEntity({
                        'alEvent': 'alMessageToPage',
                        'target': 'CPgDetailInfo/infoHub',
                        'me': that
                    });
                }
            };
            var getLastWrapper = function (item) {
                var tmp, pluginObj, lastWrapper;
                tmp = item.pluginObject || item._pluginObject;
                if (tmp) {
                    lastWrapper = item;
                    pluginObj = tmp.pluginObject || tmp._pluginObject;
                    if (pluginObj) {
                        lastWrapper = tmp;
                        tmp = pluginObj;
                        pluginObj = tmp.pluginObject || tmp._pluginObject;
                        if (pluginObj) {
                            lastWrapper = tmp;
                        }
                    }
                }
                tmp = undefined;
                pluginObj = undefined;
                return lastWrapper;
            };
            iPlate.doKeyRec = function () {
                var that = b;
                var lastWrapper = getLastWrapper(focusedProgramme);
                if (lastWrapper.isLive()) {
                    b._startRec(focusedChannel);
                }
                else {
                    if (lastWrapper.hasSchedule() || lastWrapper.hasSeriesSchedule()) {
                        that._cancelReservation(lastWrapper);
                    }
                    else if (lastWrapper.hasReminder()) {
                        that._updateScheduledRecording(lastWrapper);
                    }
                    else {
                        that._doReservation(lastWrapper);
                    }
                }
                lastWrapper = undefined;
                return true;
            };
            iPlate.doKeyStop = function () {
                var that = b;
                var lastWrapper = getLastWrapper(focusedProgramme);
                if (lastWrapper.isLive()) {
                    that._cancelRecording(focusedChannel);
                }
                else {
                    if (lastWrapper.hasSchedule() || lastWrapper.hasReminder()) {
                        that._cancelReservation(lastWrapper);
                    }
                }
                lastWrapper = undefined;
                return true;
            };
            iPlate.onFocusChanged = function (pOld, pNew) {
                var a = this;
                var that = b;
                var idx = parseInt(pNew.attr('data'), 10);
                var idxOld = parseInt(pOld.attr('data'), 10);
                var selProgramme = a._dataProvider.getItem(idx);
                if (selProgramme) {
                    that._infoHub.setCurrentProgramme(selProgramme);
                    updateCurrentProgramme(that._infoHub);
                }
                var dProvider = that._infoHub._iPlate._dataProvider;
                if (dProvider) {
                    var tot = dProvider.getCount();
                    if (tot - idx < 6) {
                        // get more EPG
                        var item = dProvider.getItem(tot - 1);
                        if (item && item.startTime) {
                            var sTime = item.startTime + item.duration + 1;
                            var eTime = sTime + 3600 * 12;
                            that._getNextEvtsAndAttach(sTime, eTime);
                        }
                    }
                }
                if (hx.config.useBackwardEpg === true) {
                    if (dProvider && a._catchUpsCount && a._catchUpsCount > 0 && idx < 1) {
                        var firstItem = dProvider.getItem(0);
                        var timeBase = firstItem.startTime * 1000;
                        that._getMoreBackwardEvtAndInsert(timeBase, timeBase - that.THRESHOLD);
                    }
                }
                // prevent overwrap betwin selected & previous image : trick :)
                if (idxOld < idx) {
                    pOld[0].children[0].className = 'iPlateItem previous';
                }
            };
            var viewIPlate = new __CONTROLS__.CViewGroupControl(null);
            viewIPlate.setId('viewIPlate');
            viewIPlate.setChildControl([iPlate]);
            viewIPlate.setDrawParam({
                anchorPositionLeft: 0
            });
            a._iPlate = iPlate;
            a._viewIPlate = viewIPlate;
        };
        CMainGroupControl.prototype.doShow = function () {
            var a = this;
            if (state === 'infoHub') {
                a._$self.one(a.CONST.cssEvntTransitionEnd, function () {
                    if (state === 'infoHub') {
                        a._$infoHubPane.$arrows.addClass('show');
                    }
                });
            }
            setTimeout(function () {
                if (a._$self) {
                    a._$self.addClass('show');
                }
            }, 1);
        };
        CMainGroupControl.prototype.doHide = function () {
            var a = this;
            var b = this._parent;
            state = 'infoHub';
            if (a._$infoHubPane) {
                a._$infoHubPane.$arrows.removeClass('show');
            }
            a._$self.removeClass('show');
            if (hx.config.useSlideText) {
                b._stopSlideAnimation(null);
            }
        };
        CMainGroupControl.prototype.doDraw = function () {
            var _this = this;
            var a = this;
            var drawHandler = {
                'infoHub': function () {
                    removeTimeBar(a);
                    removeCursor(a);
                    a.hideOptbtn();
                    a.setChildControl([]);
                    a._channelList._$self.remove();
                    a._iPlate._$self.empty();
                    a._viewIPlate._$self.remove();
                    createDateTimeIfNecessary(a);
                    createInfoHubIfNecessary(a);
                    createInfoHubIconIfNecessary(a);
                    updateEntryPane(a);
                    updateDateTime(a);
                    updateCurrentChannel(a);
                    updateCurrentProgramme(a);
                    updateStatusIcon(a);
                    a.displayOptbtn();
                    return [];
                },
                'channelList': function () {
                    removeDateTime(a);
                    removeInfoHubIcon(a);
                    removeTimeBar(a);
                    removeCursor(a);
                    a.hideOptbtn();
                    a._iPlate._$self.empty();
                    a._viewIPlate._$self.remove();
                    createInfoHubIfNecessary(a);
                    updateEntryPane(a);
                    updateCurrentChannel(a);
                    //a._channelList._$self.appendTo(a._$self);
                    a.displayOptbtn();
                    return _super.prototype.doDraw.call(a);
                },
                'iPlate': function () {
                    removeInfoHubIcon(a);
                    a.hideOptbtn();
                    a._channelList._$self.remove();
                    createDateTimeIfNecessary(a);
                    createInfoHubIfNecessary(a);
                    createTimeBarIfNecessary(a);
                    createCursorIfNecessary(a);
                    updateEntryPane(a);
                    updateDateTime(a);
                    updateCurrentChannel(a);
                    updateCurrentProgramme(a);
                    var pos = 0;
                    if (a._iplateLivePos) {
                        pos = a._iplateLivePos;
                    }
                    //a._iPlate.setFocusedItemByKey(pos);
                    //a._viewIPlate.setLeft(236 * pos, true);
                    hx.log('infohub', '[doDraw] : iPlate ' + 236 * pos);
                    a._viewIPlate.setDrawParam({
                        applyOnce: {
                            left: 236 * pos,
                            withoutAnim: true
                        }
                    });
                    a.setActiveFocus(true);
                    //a._viewIPlate._$self.appendTo(a._$self);
                    return _super.prototype.doDraw.call(a);
                }
            };
            return drawHandler[state]();
        };
        CMainGroupControl.prototype.displayOptbtn = function () {
            var a = this;
            var b = this._parent;
            if (a._channelListGroupOptBtn === undefined) {
                a._optIcon = $('<img>', {
                    'class': 'img',
                    'src': 'images/201_Option.png'
                });
                a._optText = $('<div>', {
                    'class': 'tit',
                    'html': hx.l('LOC_OPT_GROUP_ID') + ' - '
                });
                a._optData = $('<em>', {
                    'html': b._getLastGroupStr()
                });
                a._optText.append(a._optData);
                a._channelListGroupOptBtn = $('<div>', {
                    'id': 'channelListOptBtn'
                }).append(a._optIcon).append(a._optText);
                //$('#channelList').after(a._channelListGroupOptBtn);
                $('#-test-infoHub').append(a._channelListGroupOptBtn);
            }
            else {
                a._channelListGroupOptBtn.show();
            }
        };
        CMainGroupControl.prototype.hideOptbtn = function () {
            var a = this;
            if (a._channelListGroupOptBtn) {
                a._channelListGroupOptBtn.hide();
            }
        };
        CMainGroupControl.prototype.showChannelList = function (param) {
            var i, a = this, bPrismLiteSMode = false;
            a.setPrismAndPrismLiteMode();
            state = 'channelList';
            if (param) {
                focusedChannel = a._currentChannel = param;
            }
            for (i = 0; i < a._channelListData.length; i += 1) {
                var c = a._channelListData[i];
                if (c.ccid === a._currentChannel.ccid) {
                    a._channelList.setDrawParam('startIndex', i);
                    break;
                }
            }
            a._channelList.setDrawParam('drawEffect', 'spreadOut', true);
            a.setChildControl([a._channelList]);
            bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                a.setChannelListPrismLiteSsetDrawParam();
            }
            else {
                a.setChannelListPrismsetDrawParam();
            }
            a.draw();
        };
        CMainGroupControl.prototype.showIPlate = function () {
            var a = this;
            a.setPrismAndPrismLiteMode();
            state = 'iPlate';
            a._iPlate.clearDrawnItems();
            a._iPlate.clearPreviousDrawItems();
            a._iPlate.setDrawParam('drawEffect', 'spreadOut', true);
            a.setChildControl([a._viewIPlate]);
            a.draw();
        };
        CMainGroupControl.prototype.showEntry = function () {
            var a = this;
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE": "OSDIN",
                    "TRS": "I",
                    "OSD": "infoHub"
                });
            }
            hx.svc.CLOCK.startClock();
            a.setPrismAndPrismLiteMode();
            state = 'infoHub';
            a.setChildControl([a._iPlate]);
            a.draw();
        };
        CMainGroupControl.prototype.keyBack = function () {
            var a = this;
            var b = this._parent;
            var that = b;
            if (state === 'infoHub') {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "infoHub"
                    });
                }
                if (b._bActivate) {
                    b._hide();
                }
            }
            else if (state === 'iPlate') {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "iPlate"
                    });
                }
                if (that._entryPoint !== 'entry') {
                    if (b._bActivate) {
                        b._hide();
                    }
                    return;
                }
                if (that._currentEvent) {
                    that._infoHub.setCurrentProgramme(that._currentEvent);
                }
                else {
                    that._infoHub.setCurrentProgramme(undefined);
                }
                a.showEntry();
            }
            else if (state === 'channelList') {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "channelList"
                    });
                }
                if (that._entryPoint !== 'entry') {
                    if (b._bActivate) {
                        b._hide();
                    }
                    return;
                }
                if (that._currentEvent) {
                    that._infoHub.setCurrentProgramme(that._currentEvent);
                }
                else {
                    that._infoHub.setCurrentProgramme(undefined);
                }
                a.showEntry();
            }
            else {
                a.showEntry();
            }
        };
        CMainGroupControl.prototype.getCurrentState = function () {
            return state;
        };
        CMainGroupControl.prototype.setCurrentChannel = function (channel) {
            var a = this, c;
            var i;
            if (state === 'channelList') {
                for (i = 0; i < a._channelListData.length; i += 1) {
                    c = a._channelListData[i];
                    if (c.ccid === channel.ccid) {
                        a._channelList.setDrawParam('startIndex', i);
                        break;
                    }
                }
            }
            focusedChannel = a._currentChannel = channel;
        };
        CMainGroupControl.prototype.setCurrentGroup = function (group) {
            var a = this;
            a._currentGroup = group;
        };
        CMainGroupControl.prototype.setCurrentProgramme = function (programme) {
            var a = this;
            hx.log('infohub', '[setCurrentProgramme]');
            focusedProgramme = a._currentProgramme = programme;
        };
        CMainGroupControl.prototype.setiPlateProgrammes = function (programmes, channel) {
            hx.log('infohub', '[setiPlateProgrammes] :');
            var a = this;
            var bPrismLiteSMode = false;
            if (programmes === undefined) {
                a.setIplateList(undefined, undefined);
                a._updatedprogrammes = null;
                a._receivedProgrammes = false;
                return;
            }
            if (programmes.length > 20) {
                programmes.length = 20;
            }
            a._updatedprogrammes = programmes;
            a._receivedProgrammes = true;
            bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                a.setPrismLiteSInitDataIdx(0);
            }
            //if (a._receivedCatchUps === true && a._receivedProgrammes === true) {
            if (a._receivedBackwards === true && a._receivedProgrammes === true) {
                a.setIplateList(a._updatedBackwards, a._updatedprogrammes, channel);
                a._updatedprogrammes = null;
                a._receivedProgrammes = false;
                //a._updatedCatchUps = null;
                //a._receivedCatchUps = false;
                a._updatedBackwards = null;
                a._receivedBackwards = false;
            }
        };
        CMainGroupControl.prototype.insertPastProgrammes = function (programmes) {
            hx.log('infohub', '[insertPastProgrammes] : ');
            var a = this, cnt;
            var newProgrammes;
            //if (programmes.length > 20) {// limit to prevent overhead
            //    newProgrammes = programmes.slice(programmes.length - 20);
            //} else {
            newProgrammes = programmes;
            //}
            var lenNewProg = newProgrammes.length;
            for (cnt = 0; cnt < lenNewProg; cnt += 1) {
                newProgrammes[cnt] = $.extend(newProgrammes[cnt], {
                    'catchup': true
                });
            }
            if (a._iPlate._dataProvider) {
                var firstItem = a._iPlate._dataProvider.getItem(0);
                var p = newProgrammes.pop();
                if (p.startTime + p.duration <= firstItem.startTime) {
                    newProgrammes.push(p);
                }
                a._viewIPlate.setDrawParam({
                    applyOnce: {
                        left: 236 * lenNewProg,
                        withoutAnim: true,
                        after: function () {
                            a._iPlate.setFocusedItem(0, true);
                        }
                    }
                });
                a._iPlate._drawnItems.removeAll();
                a._iPlate._dataProvider.insertItems(0, newProgrammes, true);
                a.updateCurrentProgramme();
                a._iplateLivePos = lenNewProg;
            }
            newProgrammes = null;
        };
        CMainGroupControl.prototype.appendProgrammes = function (programmes) {
            hx.log('infohub', '[appendProgrammes] : ');
            var a = this;
            var b = a._parent;
            if (programmes.length > 20) {
                programmes.length = 20;
            }
            var iPlate = a._iPlate;
            if (iPlate._dataProvider) {
                var startIndex = iPlate._dataProvider.getCount();
                iPlate._dataProvider.appendItems(programmes);
                if (programmes[0]) {
                    b.updateWithIpEpg(a._currentChannel, b.callbackIpEpgResultForiPlate.bind(b), programmes[0].getStartTime() * 1000, 3, startIndex);
                }
            }
            programmes = null;
        };
        CMainGroupControl.prototype.setiPlateCatchUps = function (catchUps) {
            hx.log('infohub', '[setiPlateCatchUps] : ');
            var a = this;
            a._updatedCatchUps = catchUps;
            a._receivedCatchUps = true;
            // append no catch up
            //if (a._updatedCatchUps === null) {
            //    var noCatchup = [{
            //        'noCatchup' : true
            //    }];
            //    a._updatedCatchUps = noCatchup;
            //}
            if (a._receivedCatchUps === true && a._receivedProgrammes === true) {
                a.setIplateList(a._updatedCatchUps, a._updatedprogrammes);
                a._updatedprogrammes = null;
                a._receivedProgrammes = false;
                a._updatedCatchUps = null;
                a._receivedCatchUps = false;
            }
        };
        CMainGroupControl.prototype.setiPlateBackwards = function (backwards) {
            hx.log('infohub', '[setiPlateBackwards]: ');
            var a = this;
            if (!backwards) {
                a.setIplateList(undefined, undefined);
                a._updatedBackwards = null;
                a._receivedBackwards = false;
                return;
            }
            a._updatedBackwards = backwards;
            a._receivedBackwards = true;
            if (a._receivedBackwards === true && a._receivedProgrammes === true) {
                a.setIplateList(a._updatedBackwards, a._updatedprogrammes);
                a._updatedprogrammes = null;
                a._receivedProgrammes = false;
                a._updatedBackwards = null;
                a._receivedBackwards = false;
            }
        };
        CMainGroupControl.prototype.setIplateList = function (catchUps, programmes, channel) {
            var a = this;
            var b = this._parent;
            var that = b;
            var cnt;
            var pos = 0;
            // clear iplate
            if (catchUps === undefined && programmes === undefined) {
                a._iPlate.setDataProvider(new __CONTROLS__.CDataProvider(undefined));
                a._iplateLivePos = 0;
                return;
            }
            if (catchUps && catchUps.length > 0) {
                // no catch up is valid when schedule exist
                if ((!programmes || programmes.length === 0) && catchUps[0].noCatchup) {
                    return;
                }
                var lenC = catchUps.length;
                for (cnt = 0; cnt < lenC; cnt += 1) {
                    catchUps[cnt] = $.extend(catchUps[cnt], {
                        'catchup': true
                    });
                }
                a._iPlate.setDataProvider(new __CONTROLS__.CDataProvider(catchUps));
                a._iPlate._catchUpsCount = catchUps.length;
                pos = catchUps.length - 1;
            }
            if (programmes && programmes.length > 0) {
                if (catchUps && catchUps.length > 0) {
                    pos += 1;
                    a._viewIPlate.setDrawParam({
                        applyOnce: {
                            left: 236 * pos,
                            withoutAnim: true
                        }
                    });
                    a.appendProgrammes(programmes);
                }
                else {
                    pos = 0;
                    a._viewIPlate.setDrawParam({
                        applyOnce: {
                            left: 236 * pos,
                            withoutAnim: true
                        }
                    });
                    a._iPlate.setDataProvider(new __CONTROLS__.CDataProvider(programmes));
                }
            }
            //a._iPlate.setFocusedItemByKey(pos);
            //a._viewIPlate.setLeft(236 * pos, true);
            hx.log('infohub', '[setIplateList] : setIplateList ' + 236 * pos);
            a._viewIPlate.draw();
            a._iplateLivePos = pos;
            a.setActiveFocus(true);
            // update selected item
            if (state === 'iPlate') {
                if (a._iPlate._dataProvider && a._iPlate._dataProvider._count > 0) {
                    var selProgramme = a._iPlate._dataProvider.getItem(pos);
                    if (selProgramme) {
                        that._infoHub.setCurrentProgramme(selProgramme);
                        updateCurrentProgramme(that._infoHub);
                    }
                }
            }
        };
        CMainGroupControl.prototype.updateCurrentProgramme = function () {
            var a = this;
            updateCurrentProgramme(a);
        };
        CMainGroupControl.prototype.removeProgramme = function (index) {
            var a = this;
            a._iPlate._dataProvider.removeItem(index);
        };
        CMainGroupControl.prototype.removeIplateFocusedItem = function () {
            var a = this;
            var o = a._currentProgramme;
            if (o === undefined) {
                return;
            }
            if (o.catchup) {
                if (o.noCatchup) {
                    return;
                }
                a.removeIplateItem(o.getId());
            }
        };
        CMainGroupControl.prototype.removeIplateItem = function (id) {
            var a = this;
            var i;
            var o;
            var index = -1;
            var totCnt = a._iPlate._dataProvider._count;
            for (i = 0; i < totCnt; i += 1) {
                o = a._iPlate._dataProvider.getItem(i);
                if (o.catchup) {
                    if (!o.noCatchup) {
                        if (o.getId() === id) {
                            index = i;
                            break;
                        }
                    }
                }
            }
            if (index < 0 || index >= totCnt) {
                return;
            }
            a._iPlate._dataProvider.removeItem(index);
            // update
            if (a._iPlate._dataProvider._count === 0) {
                a.setCurrentProgramme(undefined);
            }
            else {
                var item = a._iPlate._dataProvider.getItem(index);
                a.setCurrentProgramme(item);
                // if last item move 1 item
                if (index === a._iPlate._dataProvider._count) {
                    a._iPlate.setFocusedItemByKey(index - 1);
                    a._viewIPlate.setLeft(236 * (index - 1), false);
                }
            }
            a.updateCurrentProgramme();
        };
        CMainGroupControl.prototype.setFocusedChannelEvt = function (programme) {
            var a = this;
            var bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                a.setChannelListPrismLiteSsetFocusEvent(programme);
            }
            else {
                a.setChannelListPrismsetFocusEvent(programme);
            }
        };
        CMainGroupControl.prototype.updateCurrentRecStatus = function (ccid, bStart) {
            var a = this;
            var b = this._parent;
            var bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (!!focusedChannel) {
                if (state === 'infoHub') {
                    var noWork;
                }
                else if (state === 'channelList') {
                    if (ccid === focusedChannel.ccid) {
                        if (bPrismLiteSMode === true) {
                            if (bStart) {
                                b._getChListEvt(a._currentChannel);
                            }
                            else {
                                $('.chIconRecLite').detach();
                            }
                        }
                        else {
                            if (bStart) {
                                $('.chThumbnail').append($recIcon);
                            }
                            else {
                                $('.chIconRec').detach();
                            }
                        }
                    }
                    checkChannelsChangeable(a._channelList.getCurrentViewingItemInfos());
                }
                else {
                    if (ccid === focusedChannel.ccid) {
                        if (bStart) {
                            var $live = $('.-fd.-afd-leaf');
                            if (focusedProgramme.isLive()) {
                                if ($live.length > 0) {
                                    $live.append($('<div>', {
                                        'class': 'iPlateItemIcoRec'
                                    }));
                                }
                            }
                            else {
                                if ($live.length > 0) {
                                    $live.append($('<div>', {
                                        'class': 'iPlateItemIcoRecRsv'
                                    }));
                                }
                            }
                        }
                        else {
                            var $recIco;
                            var $recSerIco;
                            var $reminderIco;
                            if (focusedProgramme.isLive()) {
                                $recIco = $('.-fd.-afd-leaf .iPlateItemIcoRec');
                                if ($recIco.length > 0) {
                                    $recIco.remove();
                                }
                            }
                            else {
                                $recIco = $('.-fd.-afd-leaf .iPlateItemIcoRecRsv');
                                if ($recIco.length > 0) {
                                    $recIco.remove();
                                }
                                $recSerIco = $('.-fd.-afd-leaf .iPlateItemIcoRecRsvSer');
                                if ($recSerIco.length > 0) {
                                    $recSerIco.remove();
                                }
                                $reminderIco = $('.-fd.-afd-leaf .iPlateItemIcoReminder');
                                if ($reminderIco.length > 0) {
                                    $reminderIco.remove();
                                }
                            }
                        }
                    }
                }
            }
        };
        CMainGroupControl.prototype.updateRecordings = function (recordings) {
            var a = this, update = false;
            if (state === 'infoHub') {
                if (recordings) {
                    if (a._recordings) {
                        if (a._recordings.length !== recordings.length) {
                            update = true;
                        }
                    }
                    a._recordings = recordings;
                }
                else {
                    a._recordings = [];
                }
                if (update) {
                    updateStatusIcon(a);
                    var bPrismLiteSMode = a.isPrismLiteSModeOn();
                    if ((bPrismLiteSMode === true) && (state === 'infoHub')) {
                        updateCurrentProgramme(a);
                    }
                }
            }
            else {
                a._recordings = recordings;
            }
        };
        CMainGroupControl.prototype.checkRecordings = function (ccid) {
            var a = this;
            if (a._recordings) {
                var len = a._recordings.length, i;
                for (i = 0; i < len; i += 1) {
                    if (ccid === a._recordings[i].getChannelId()) {
                        return true;
                    }
                }
            }
            return false;
        };
        CMainGroupControl.prototype.findProgrammeInRecordings = function (ccid, programmeID) {
            var a = this;
            if (a._recordings) {
                var len = a._recordings.length, i;
                for (i = 0; i < len; i += 1) {
                    if (ccid === a._recordings[i].getChannelId()) {
                        if (programmeID === a._recordings[i].getProgrammeID()) {
                            return true;
                        }
                    }
                }
            }
            return false;
        };
        CMainGroupControl.prototype.setChannelListData = function (channelList) {
            var a = this;
            a._channelListData = channelList;
            a._channelList.setData(channelList);
        };
        CMainGroupControl.prototype.getDefaultThumbnail = function () {
            var a = this;
            var channel = focusedChannel, defaultThumbnail = a._IMAGE[0];
            if (channel) {
                defaultThumbnail = a._IMAGE[channel.channelType];
            }
            hx.log("warning", "#### " + defaultThumbnail + "####");
            return defaultThumbnail;
        };
        CMainGroupControl.prototype.getCasInformation = function () {
            var that = this, i, casArr = [], length, casIDs;
            var b = this._parent;
            if (focusedChannel.ccid) {
                if (!!focusedChannel.casIDs) {
                    length = focusedChannel.casIDs.length;
                    for (i = 0; i < length; i += 1) {
                        casArr.push(focusedChannel.casIDs[i]);
                    }
                }
            }
            if (casArr.length > 0) {
                casIDs = 'cas ';
                if (casArr.length > 1) {
                    casIDs += 'multi';
                }
                else {
                    for (i = 0, length = casArr.length; i < length; i += 1) {
                        if (casArr[i] === b._strCasID.indexOf('CAS_NAGRA')) {
                            casIDs += 'na ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_IRDETO')) {
                            casIDs += 'ir ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_NDS')) {
                            casIDs += 'nd ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_VIACCESS')) {
                            casIDs += 'va ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_CONAX')) {
                            casIDs += 'cx ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_MEDIAGUARD')) {
                            casIDs += 'mg ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_CRYPTOWORKS')) {
                            casIDs += 'cr ';
                        }
                    }
                }
            }
            return casIDs;
        };
        // Used for PRISM Lite Simple
        CMainGroupControl.prototype.setPrismAndPrismLiteMode = function () {
            var a = this;
            var bIpEpgOn = false, bPrismLiteSmode = false;
            if ((hx.config.usePrismLiteS === undefined) || (hx.config.usePrismLiteS === false)) {
                bPrismLiteSmode = false;
            }
            else if (hx.config.usePrismLiteS === true) {
                if (ENV.listModel.IR4000HD === ENV.model || ENV.listModel.HDFOXIRCONNECT === ENV.model) {
                    bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                    if (bIpEpgOn === true) {
                        bPrismLiteSmode = false;
                    }
                    else {
                        bPrismLiteSmode = true;
                    }
                }
                else if (ENV.listModel.HMS1000S === ENV.model) {
                    var countryId;
                    countryId = hx.svc.SETTING_UTIL.getCountryId();
                    bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                    switch (countryId) {
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                            bPrismLiteSmode = false;
                            break;
                        default:
                            bPrismLiteSmode = false;
                    }
                }
            }
            else {
                bPrismLiteSmode = false;
            }
            a.bPrismLiteSmode = bPrismLiteSmode;
        };
        CMainGroupControl.prototype.isPrismLiteSModeOn = function (ulIdx) {
            var a = this;
            return a.bPrismLiteSmode;
        };
        CMainGroupControl.prototype.setChannelListPrismLiteSsetFocusEvent = function (programme) {
            var a = this;
            focusedProgramme = a._currentProgramme = programme;
            var programmeName = '', programmeTime = '';
            var programmeDate = '', thumbnail = '', logo = '', casIcon = '';
            var sT, eT;
            var lockHtml = '', casHtml = '', progHtml = '', eventInfo, progTimeHtml = '';
            if (programme) {
                programmeName = programme.getName();
                sT = programme.getStartTime();
                eT = programme.getEndTime();
                programmeTime = dateFormat(new Date(sT * 1000), "HH:MM") + ' ~ ' + dateFormat(new Date(eT * 1000), "HH:MM");
            }
            if (a.checkRecordings(focusedChannel.ccid)) {
                thumbnail = '<div class="chIconRecLite"></div>';
            }
            var casInfo = a.getCasInformation();
            if (focusedChannel.manualBlock) {
                lockHtml = '<div class="channelLock"></div>';
            }
            if (casInfo) {
                casHtml = '<div class=\"' + casInfo + '\"></div>';
            }
            if (programmeName) {
                progHtml = programmeName;
            }
            else {
                progHtml = hx.l('LOC_NO_PROGRAMME_INFORMATION_ID');
            }
            if (programmeTime) {
                progTimeHtml = programmeTime;
            }
            eventInfo = lockHtml + casHtml + progHtml;
            a._channelList._$anchor.find('.chEvtName').html(eventInfo);
            a._channelList._$anchor.find('.chEvtTime').html(progTimeHtml);
            a._channelList._$anchor.find('.chThumbnail').html(thumbnail);
        };
        CMainGroupControl.prototype.setChannelListPrismsetFocusEvent = function (programme) {
            var a = this;
            if (focusedChannel && programme) {
                if (focusedChannel.ccid !== programme.ccid) {
                    return;
                }
            }
            if (focusedProgramme && focusedProgramme.load_thumbnail) {
                return;
            }
            focusedProgramme = a._currentProgramme = programme;
            var programmeName = '', thumbnail = '', logo = '', casIcon = '';
            if (programme && !focusedChannel.manualBlock) {
                programmeName = programme.getName();
                thumbnail = '<img src="' + programme.getThumbnail() + '" ' + 'onerror="this.src=\'' + a.getDefaultThumbnail() + '\';" />';
            }
            else {
                thumbnail = '<img src="' + a.getDefaultThumbnail() + '">';
            }
            if (a.checkRecordings(focusedChannel.ccid)) {
                thumbnail = thumbnail + '<div class="chIconRec"></div>';
            }
            if (ENV.op != ENV.listOP.UKDTT) {
                if (focusedChannel.logoURL) {
                    logo = '<img src="' + focusedChannel.logoURL + '"onerror="this.src=\'\';" />';
                    thumbnail = thumbnail + '<div class="chLogo">' + logo + '</div>';
                }
            }
            var casInfo = a.getCasInformation();
            var lockHtml = '', casHtml = '', progHtml = '', eventInfo;
            if (focusedChannel.manualBlock) {
                lockHtml = '<div class="channelLock"></div>';
            }
            if (casInfo) {
                casHtml = '<div class=\"' + casInfo + '\"></div>';
            }
            if (programmeName) {
                progHtml = programmeName;
            }
            eventInfo = lockHtml + casHtml + progHtml;
            if (a._channelList && a._channelList._$anchor) {
                if (programme) {
                    if (!programme.programmeIdMds) {
                        a._channelList._$anchor.find('.chEvtName').html(eventInfo);
                    }
                }
                a._channelList._$anchor.find('.chThumbnail').html(thumbnail);
            }
        };
        CMainGroupControl.prototype.setChannelListPrismLiteSsetDrawParam = function () {
            var that = this;
            that._channelList._drawParam.viewCount = 10;
            that._channelList._drawParam.anchorHeight = 115;
        };
        CMainGroupControl.prototype.setChannelListPrismsetDrawParam = function () {
            var that = this;
            that._channelList._drawParam.viewCount = 7;
            that._channelList._drawParam.anchorHeight = 270;
        };
        CMainGroupControl.prototype.keyLeftRight = function (keyCode) {
            var a = this;
            var ulIdx = 0;
            var dProvider = a._iPlate._dataProvider, ulMoveIdx = 0, selProgramme;
            if (a._ulEventCount < 1) {
                hx.log('infohub', 'keyLeftRight() Error!! Does not has event data!!');
                return;
            }
            ulMoveIdx = a._ulDispEventIdx;
            switch (keyCode) {
                case hx.key.KEY_LEFT:
                    if (ulMoveIdx > 0) {
                        ulMoveIdx -= 1;
                    }
                    else {
                        hx.log('infohub', 'keyLeftRight()!! Do not move the prevs event : ' + ulMoveIdx);
                    }
                    break;
                case hx.key.KEY_RIGHT:
                    if (ulMoveIdx < (a._ulEventCount - 1)) {
                        ulMoveIdx += 1;
                    }
                    else {
                        hx.log('infohub', 'keyLeftRight()!! Do not move the next event : ' + ulMoveIdx + 'Max Event:' + a._ulEventCount);
                    }
                    break;
                default:
                    hx.log('infohub', 'keyLeftRight() Error!! Invalid key value :' + keyCode);
                    return;
            }
            //if ((dProvider._count > 0) && (a._ulEventCount > 0)) {
            //    ulIdx = ulMoveIdx + a._ulCatchUpsCount;
            //    selProgramme = dProvider._data[ulIdx];
            //}
            if (selProgramme) {
                a.setPrismLiteSCurrentEventIdx(ulMoveIdx);
                a.setCurrentProgramme(selProgramme);
                updateCurrentProgramme(a);
            }
            if (dProvider) {
                var tot = dProvider._count;
                if (tot - ulIdx < 6) {
                    // get more EPG
                    var item = dProvider._data[tot - 1];
                    if (item && item.startTime) {
                        var sTime = item.startTime + item.duration + 1;
                        var eTime = sTime + 3600 * 12;
                        a.getPrismLiteSNextEvtsAndAttach(sTime, eTime);
                    }
                }
            }
        };
        CMainGroupControl.prototype.setPrismLiteSCurrentEventIdx = function (ulIdx) {
            var a = this;
            // Current Disp Event Index of iplate provider list
            a._ulDispEventIdx = ulIdx;
        };
        CMainGroupControl.prototype.setPrismLiteSInitDataIdx = function (ulIdx) {
            var a = this;
            // Current Disp Event Index of iplate provider list
            a._ulDispEventIdx = ulIdx;
            // Event Count
            a._ulEventCount = 0;
            if (a._receivedProgrammes === true) {
                a._ulEventCount = a._updatedprogrammes.length;
            }
            // Catch ups Count
            //a._ulCatchUpsCount = 0;
            //if (a._receivedCatchUps === true) {
            //    a._ulCatchUpsCount = a._updatedCatchUps.length;
            //}
        };
        CMainGroupControl.prototype.setPrismLiteSUpdateDataIdx = function (ulEvent, ulCatchUps) {
            var a = this;
            a._ulEventCount = ulEvent;
            a._ulCatchUpsCount = ulCatchUps;
        };
        CMainGroupControl.prototype.getPrismLiteSNextEvtsAndAttach = function (startTime, endTime) {
            var a = this, ulNextEventCount = 0, channel;
            channel = a._currentChannel;
            //debugger;
            hx.svc.EPG.getProgrammes({
                isOnAir: false,
                channel: channel,
                startTime: startTime,
                endTime: endTime,
                listener: a,
                cb: function (result) {
                    if (result[0].length > 0) {
                        ulNextEventCount = a._ulEventCount + result[0].length;
                        a.setPrismLiteSUpdateDataIdx(ulNextEventCount, a._ulCatchUpsCount);
                        a.appendProgrammes(result[0]);
                    }
                    result = undefined;
                }
            });
        };
        CMainGroupControl.prototype.showDetailInfo = function () {
            var a = this;
            var b = this._parent;
            var that = b;
            var dsrvDetailInfo2 = hx.svc.DETAIL_INFO;
            var ulIdx = 0, selProgramme = null;
            var dProvider = a._iPlate._dataProvider;
            var result = false;
            if (a._ulEventCount < 1) {
                hx.log('infohub', 'showDetailInfo() Error!! Does not has event data!!');
                return result;
            }
            if ((dProvider._count > 0) && (a._ulEventCount > 0)) {
                ulIdx = a._ulDispEventIdx + a._ulCatchUpsCount;
                selProgramme = dProvider._data[ulIdx];
            }
            if (that._infoHub.isPrismLiteSModeOn() === false) {
                selProgramme = a._currentProgramme;
            }
            if (!!selProgramme) {
                result = true;
                that._$infoHub[0].style.display = 'none';
                that._bActivate = false;
                clearTimeout(that._timerId);
                that._timerId = -1;
                var programmeDetailItem = dsrvDetailInfo2.createProgrammeItem(selProgramme.pluginObject);
                if (selProgramme.raw) {
                    var mdsProgramme = hx.svc.IP_EPG.convert.to.programme(selProgramme.raw, selProgramme.channel, null);
                    programmeDetailItem.setOipfProgrammeInstance(mdsProgramme);
                }
                hx.il.adhocPageParam.push({
                    programmeDetailItem: programmeDetailItem
                });
                that.sendEventToEntity({
                    'alEvent': 'alMessageToPage',
                    'target': 'CPgDetailInfo/infoHub',
                    'me': that
                });
            }
            return result;
        };
        CMainGroupControl.prototype.actionReservaion = function (action) {
            var a = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var p = a._currentProgramme;
            var efactoryType = 0, bIsGetPvrHDD = false, bIsLiveEvt = false;
            var scheduledList = dsrvRecordings2.getScheduledList();
            if (a._ulEventCount < 1) {
                hx.log('infohub', 'actionReservaion() Error!! Does not has event data!!');
                return false;
            }
            function maxReservation() {
                // Check Max reservation count
                var ret = false;
                if (scheduledList.length >= 300) {
                    var now = new Date();
                    var ct = Math.floor(now.valueOf() / 1000);
                    if (p) {
                        // check live
                        if (p.catchup === undefined) {
                            if (ct >= p.getStartTime() && ct <= p.getEndTime()) {
                                bIsLiveEvt = true;
                            }
                            else {
                                bIsLiveEvt = false;
                            }
                        }
                    }
                    if (bIsLiveEvt === false) {
                        hx.log('infohub', 'actionReservaion() Error!! Does not action reservaion because max schedule count !!');
                        var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                        hx.msg('response', {
                            text: msg,
                            auto_close: true,
                            close_time: 3000
                        });
                        ret = true;
                    }
                    else {
                        hx.log('infohub', 'actionReservaion() Error!! Does not action reservaion because max schedule count!! But live Event');
                        ret = false;
                    }
                }
                return ret;
            }
            function cancelReservation(data) {
                hx.log('infohub', 'actionReservaion() - cancelReservation()');
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
                    dsrvRecordings2.removeScheduledRecording({
                        items: [scheduledItem],
                        finished: function () {
                            hx.log('infohub', 'actionReservaion() finished removeScheduledRecording');
                        }
                    });
                }
            }
            function isRecordingEvent(data) {
                var bIsRecordingEvent = false;
                var item = null, recCCid = 0, i, ti;
                var cRecList = dsrvRecordings2.getCurrentRecordings();
                var ulStartTimeValue = 0, ulEndTimeValue = 0;
                var ulstartTime = 0, ulendTime = 0;
                if (cRecList.length === 0) {
                    hx.log('infohub', 'isRecordingEvent() no recording channel');
                    bIsRecordingEvent = false;
                }
                for (i = 0, ti = cRecList.length; i < ti; i += 1) {
                    item = cRecList[i];
                    if (item && (item.getChannelId() === data.ccid)) {
                        hx.log('infohub', 'isRecordingEvent() find same recording channel : ' + item.getChannelId());
                        ulStartTimeValue = item.getStartTime();
                        ulEndTimeValue = item.getEndTime();
                        ulstartTime = Math.floor(ulStartTimeValue / 1000);
                        ulendTime = Math.floor(ulEndTimeValue / 1000);
                        if ((ulstartTime <= data.startTime) && (ulendTime >= data.startTime)) {
                            bIsRecordingEvent = true;
                        }
                        else {
                            bIsRecordingEvent = false;
                        }
                        break;
                    }
                }
                return bIsRecordingEvent;
            }
            function onRequestInstanceRecording(data) {
                var param = {};
                if (data) {
                    if (hx.config.useDelayedRec) {
                        param.tsrPosOffset = hx.svc.TSR.getPlaybackOffset();
                        param.tsrBufferSize = hx.svc.TSR.getMaxOffset();
                    }
                    else {
                        param.tsrPosOffset = 0;
                        param.tsrBufferSize = 0;
                    }
                    param.finished = function (result) {
                        hx.log('infohub', hx.util.format('onRequestInstanceRecording - result = {0}', result));
                    };
                    if (data.ccid) {
                        hx.log('infohub', hx.util.format('onRequestInstanceRecording - channel.ccid = {0}', data.ccid));
                        param.ccid = data.ccid;
                    }
                    var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                    if (bIsGetPvrHDD === true) {
                        hx.svc.RECORDINGS.requestInstanceRecording(param);
                    }
                    else {
                        this.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgNotAvailable?show'
                        });
                    }
                }
            }
            if (p && (p.isLive() !== true)) {
                var ret = false;
                if (!p.hasSchedule() && !p.hasReminder()) {
                    var fullReserved = maxReservation();
                    if (fullReserved === true) {
                        return true;
                    }
                    bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                    if (bIsGetPvrHDD === false) {
                        efactoryType = 1;
                    }
                    dsrvRecordings2.record({
                        programme: p.pluginObject,
                        factoryType: efactoryType,
                        finished: function () {
                            hx.log('infohub', 'actionReservaion() finished record');
                        }
                    });
                    ret = true;
                }
                else if (p.hasSchedule() || p.hasReminder()) {
                    var bIsRecording = isRecordingEvent(p);
                    if (bIsRecording === false) {
                        cancelReservation(p);
                        ret = true;
                    }
                    else {
                        onRequestInstanceRecording(p);
                        ret = true;
                    }
                }
                else {
                    ret = false;
                }
            }
            else {
                if (p && (p.isLive() === true)) {
                    onRequestInstanceRecording(p);
                    ret = true;
                }
                else {
                    ret = false;
                }
            }
            return ret;
        };
        return CMainGroupControl;
    })(__CONTROLS__.CGroupControl);
    var cCPgInfoHub = (function (_super) {
        __extends(cCPgInfoHub, _super);
        function cCPgInfoHub(aPageName) {
            _super.call(this, aPageName);
            this._responseObj = null;
            this._currentChannel = null;
            this._currentEvent = null;
            this._remChannel = null;
            this._recordings = [];
            this._groupContextMenu = null;
            this._strCurrentGroup = '';
            this._iPlateIconUpdate = false;
            this._favlistname = [];
            this._strChannelGroupName = [];
            this._currentGroupDB = null;
            this._strCasID = hx.config.CPgInfoHub.chGroup.strCasID;
            this._groupDB = hx.config.CPgInfoHub.chGroup.groupDB;
            this._favGroupDB = hx.config.CPgInfoHub.chGroup.favGroupDB;
            // Alphabet Group
            this._alphabetgroupContextMenu = null;
            this._strAlphabetGroupName = [];
            this._alphabetGroupDB = hx.config.CPgInfoHub.chGroup.alphabetGroupsDB;
            this._entryPoint = null;
            this.TextAniValueOffset = 11;
            this.TextAniIntervalOffset = 15;
            this.THRESHOLD = 7 * 24 * 60 * 60 * 1000; // 7DAYS(ms)
            this.callbackIpEpgResultForChannelList = function (channel, res) {
                var _this = this;
                if (this._currentChannel !== channel) {
                    return;
                }
                if (this._infoHub._currentProgramme && this._infoHub._currentProgramme.isWrapper) {
                    this._infoHub.setFocusedChannelEvt(this._infoHub._currentProgramme);
                    return;
                }
                var currentProgramme = this._infoHub._currentProgramme;
                if (!currentProgramme) {
                    return;
                }
                var parsedId = hx.svc.IP_EPG.parse.util.programIdParser(currentProgramme.getProgrammeID());
                var legacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                var info = hx.svc.IP_EPG.parse.xml.schedule(res);
                var schedules = info.schedules;
                var data = null, eventId = null;
                var oipfProgramme = null, pp = null, i = 0, aEvent = null;
                for (i = 0; i < schedules.length; i++) {
                    aEvent = hx.svc.IP_EPG.parse.element.scheduleEvent(schedules[i]);
                    parsedId = hx.svc.IP_EPG.parse.util.programIdParser(aEvent.programURL);
                    eventId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                    if (eventId === legacyId) {
                        data = info.crids[aEvent.crid];
                        data.schedule = schedules[i];
                        data.available = false;
                        oipfProgramme = hx.svc.IP_EPG.convert.to.programme(data, channel, currentProgramme);
                        pp = new __PROVIDER__.CProgrammeProvider(oipfProgramme);
                        pp.raw = data;
                        var callback = function () {
                            _this._infoHub.setFocusedChannelEvt(pp);
                        };
                        pp.setProgrammeIdMds(aEvent.crid);
                        if (ENV.op !== ENV.listOP.UKDTT) {
                            oipfProgramme.onchannellogo = callback;
                        }
                        oipfProgramme.onthumbnail = callback;
                        this._infoHub.setFocusedChannelEvt(pp);
                        break;
                    }
                }
            };
            this.callbackIpEpgResultForiPlate = function (channel, res, prevIdx) {
                var _this = this;
                if (prevIdx === void 0) { prevIdx = 0; }
                var a = this;
                var iPlate = a._infoHub._iPlate;
                var dp = iPlate._dataProvider;
                var dpLen = dp.getCount();
                var info = hx.svc.IP_EPG.parse.xml.schedule(res);
                var schedules = Array.prototype.slice.call(info.schedules);
                var i, currentProgramme;
                var oipfProgramme = null;
                var eventData, parsedId, legacyId, eventId;
                for (i = prevIdx; i < dpLen; i += 1) {
                    currentProgramme = dp.getItem(i);
                    if (currentProgramme instanceof __PROVIDER__.CProgrammeProvider) {
                        parsedId = hx.svc.IP_EPG.parse.util.programIdParser(currentProgramme.getProgrammeID());
                        legacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                        var j, aEvent;
                        for (j = 0; j < schedules.length; j++) {
                            aEvent = hx.svc.IP_EPG.parse.element.scheduleEvent(schedules[j]);
                            parsedId = hx.svc.IP_EPG.parse.util.programIdParser(aEvent.programURL);
                            eventId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                            if (eventId === legacyId) {
                                eventData = info.crids[aEvent.crid];
                                eventData.schedule = schedules[j];
                                eventData.available = false;
                                eventData.availableIconUrl = "";
                                currentProgramme.raw = eventData;
                                oipfProgramme = hx.svc.IP_EPG.convert.to.programme(eventData, channel, currentProgramme);
                                (function (aProg, aData) {
                                    var prog = aProg;
                                    var data = aData;
                                    var ondemand = oipfProgramme.ondemand;
                                    if (ondemand) {
                                        hx.svc.IP_EPG.getAIT(_this._getTemplateUrl(ondemand)).then(function (xml) {
                                            a._checkTemplateAIT(xml, prog).then(function (param) {
                                                data.availableIconUrl = param.url;
                                                prog.onavailable(param.url);
                                            }, function (err) {
                                            });
                                        }, function (err) {
                                        });
                                    }
                                })(currentProgramme, eventData);
                                eventData = null;
                                oipfProgramme.onthumbnail = (function (aIndex) {
                                    return function () {
                                        var prog = dp.getItem(aIndex);
                                        prog.thumbnailURL = this.getField('thumbnailURL');
                                        a._updateThumbnailForiPlate(this, aIndex);
                                        prog = undefined;
                                    };
                                })(i);
                                currentProgramme.onavailable = (function (aIndex) {
                                    return function (aUrl) {
                                        a._updateOnDemandAvailableIconForiPlate(aIndex, aUrl);
                                    };
                                })(i);
                                schedules.splice(j, 1);
                                break;
                            }
                        }
                    }
                }
            };
            this._getTemplateUrl = function (ondemand) {
                var params = hx.svc.SETTING_UTIL.getNetworkId().map(function (nid) {
                    return 'nids[]=' + nid;
                });
                params.push('lloc=epg');
                return [
                    ondemand.auxiliaryURL,
                    params.join('&')
                ].join(ondemand.auxiliaryURL.indexOf('?') !== -1 ? '&' : '?');
            };
            this._checkTemplateAIT = function (xml, aProg) {
                var retDef = Q.defer();
                var timeCurrent = new Date().getTime();
                var availabilityIconUrl = null;
                var iPlate = this._infoHub._iPlate;
                var prog = aProg;
                if (xml) {
                    //check hbbtv application
                    var type = xml.querySelector('OtherApp').textContent;
                    //check version
                    var major = xml.querySelector('versionMajor').textContent;
                    var minor = xml.querySelector('versionMinor').textContent;
                    var micro = xml.querySelector('versionMicro').textContent;
                    var version = (major + minor + micro) * 1;
                    if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') {
                        var ondemand = prog.raw ? prog.raw.ondemand : prog._ondemand;
                        if (ondemand) {
                            ondemand = hx.svc.IP_EPG.parse.element.ondemand(ondemand);
                            if (ondemand.startOfAvailability * 1000 < timeCurrent && ondemand.endOfAvailability * 1000 >= timeCurrent) {
                                if (ondemand.mediaAvailabilityCS) {
                                    availabilityIconUrl = "images/205_REC_Play.png";
                                    if (prog.raw) {
                                        prog.raw.available = true;
                                    }
                                    else {
                                        prog.available = true;
                                    }
                                }
                                else {
                                    availabilityIconUrl = "images/614_EPG_Comingsoon.png";
                                }
                            }
                            else if (timeCurrent > (ondemand.startOfAvailability - 7200) * 1000 && timeCurrent <= ondemand.startOfAvailability * 1000) {
                                availabilityIconUrl = "images/614_EPG_Comingsoon.png";
                            }
                        }
                    }
                }
                retDef.resolve({
                    url: availabilityIconUrl
                });
                prog = null;
                return retDef.promise;
            };
            var a = this;
            var b = this;
            //        var CInfoHubControl = new CMainGroupControl()
            //        a.CLASSES = {
            //            'CInfoHubControl' : CInfoHubControl
            //        };
        }
        cCPgInfoHub.prototype.create = function () {
            var _this = this;
            var that = this;
            that._bActivate = false;
            that._timerId = -1;
            hx.logadd('infohub');
            that._init_Data();
            // Main control: info hub control
            $.extend(this, {
                $container: $("#wrap")
            });
            //        var CInfoHubControl = that.CLASSES.CInfoHubControl;
            var $infoHub = $('<div>', {
                'id': '-test-infoHub',
                'class': '-afd'
            });
            var infoHub = new CMainGroupControl($infoHub, this);
            $('#ltv').after($infoHub);
            that._$infoHub = $infoHub;
            that._infoHub = infoHub;
            hx.al.compose(that);
            var fnUpdate = function () {
                var cRecList = hx.svc.RECORDINGS.getCurrentRecordings();
                if (!!cRecList) {
                    that._recordings = cRecList;
                    that._infoHub.updateRecordings(cRecList);
                }
                cRecList = undefined;
            };
            hx.svc.RECORDINGS.addEventCb('ScheduledItemAdded', that, function (param) {
                var ccid = param.item.getChannelId();
                if (that._infoHub.getCurrentState() === 'iPlate' && !!that._infoHub._iPlate) {
                    that._infoHub.updateCurrentRecStatus(ccid, true);
                    fnUpdate();
                    that._iPlateIconUpdate = true;
                }
            });
            hx.svc.RECORDINGS.addEventCb('ScheduledItemRemoved', that, function (param) {
                var ccid = param.item.getChannelId();
                if (that._infoHub.getCurrentState() === 'iPlate' && !!that._infoHub._iPlate) {
                    that._infoHub.updateCurrentRecStatus(ccid, false);
                    fnUpdate();
                    that._iPlateIconUpdate = true;
                }
            });
            hx.svc.RECORDINGS.addEventCb('ScheduledItemUpdated', that, function (param) {
                var ccid = param.item.getChannelId();
                if (that._infoHub.getCurrentState() === 'iPlate' && !!that._infoHub._iPlate) {
                    that._infoHub.updateCurrentRecStatus(ccid, true);
                    fnUpdate();
                    that._iPlateIconUpdate = true;
                }
            });
            hx.svc.RECORDINGS.addEventCb('RecordingItemAdded', that, function (param) {
                var ccid = param.item.getChannelId();
                that._infoHub.updateCurrentRecStatus(ccid, true);
                fnUpdate();
                that._iPlateIconUpdate = true;
            });
            hx.svc.RECORDINGS.addEventCb('RecordingCompleted', that, function (param) {
                var ccid = param.item.getChannelId();
                that._infoHub.updateCurrentRecStatus(ccid, false);
                fnUpdate();
                that._iPlateIconUpdate = true;
            });
            hx.svc.RECORDINGS.addEventCb('RecordingItemRemoved', that, function (param) {
                var ccid = param.item.getChannelId();
                that._infoHub.updateCurrentRecStatus(ccid, false);
                fnUpdate();
                that._iPlateIconUpdate = true;
                if (that._infoHub && that._bActivate !== true) {
                    if (that._$infoHub[0].style.display === 'none') {
                        that._$infoHub[0].style.display = 'block';
                        that._bActivate = true;
                        that._resetTimer();
                        that._infoHub.removeIplateFocusedItem();
                    }
                }
            });
            // channel change listener
            hx.svc.AV_CTRL.addEventCb('evtSetChannel', that, function (aParam) {
                that._on_evtSetChannel(aParam);
            });
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, function () {
                _this._on_ChannelListUpdate();
            });
            hx.svc.CLOCK.addEventCb('MinuteChanged', this, function (aNow) {
                updateDateTime(infoHub);
            });
            if (ENV.op === ENV.listOP.UKDTT) {
                hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, function (aButtonInfo) {
                    if (_this.getPageVisible(_this.name) === true) {
                        var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                        dlg.open({
                            onClosed: function () {
                                _this.setKeyHookingControl(dlg);
                            }
                        });
                        _this.setKeyHookingControl(dlg, true);
                    }
                });
            }
        };
        cCPgInfoHub.prototype.destroy = function () {
            hx.svc.RECORDINGS.clearEventCb(this);
            // remove event listeners
            hx.svc.AV_CTRL.clearEventCb(this);
            hx.svc.CLOCK.clearEventCb(this);
            // remove subWrap
            if (this._bActivate === true) {
                this._hide();
            }
            if (this._$infoHub) {
                this._$infoHub.remove();
                delete this._$infoHub;
                this._$infoHub = undefined;
            }
            if (this._infoHub) {
                this._infoHub.destroy();
                delete this._infoHub;
                this._infoHub = undefined;
            }
            if (this._recordings) {
                delete this._recordings;
                this._recordings = undefined;
            }
            if (this._currentEvent) {
                delete this._currentEvent;
                this._currentEvent = undefined;
            }
            if (this._currentChannel) {
                delete this._currentChannel;
                this._currentChannel = undefined;
            }
            if (this._remChannel) {
                delete this._remChannel;
                this._remChannel = undefined;
            }
        };
        cCPgInfoHub.prototype.isActive = function () {
            return this._bActivate;
        };
        cCPgInfoHub.prototype.visible = function () {
            return this._bActivate;
        };
        cCPgInfoHub.prototype.getSelectedText = function () {
            var that = this;
            var currentItem, currentIndex;
            var infoHub = that._infoHub;
            var channelList = infoHub._channelList;
            var iPlate = infoHub._iPlate;
            var focusedItem = iPlate.getFocusedItem();
            currentItem = (focusedItem && infoHub.getCurrentState() === 'iPlate') ? focusedItem.item : channelList.getCurrentItem();
            return currentItem.name;
        };
        cCPgInfoHub.prototype._on_alMessageToPage = function (aParam) {
            var that = this;
            var retObj, retObj2, retObj3;
            var state = that._infoHub.getCurrentState();
            var dsrvChList = hx.svc.CH_LIST;
            hx.log('infohub', '[_on_alMessageToPage]');
            if (aParam.query === 'dcn') {
                if (state === 'channelList') {
                    if (aParam.param !== 'clear') {
                        var ch = dsrvChList.getCurrentChannel();
                        if (ch.group === "TV" || ch.group === "RADIO") {
                            var data = {};
                            data.ccid = aParam.param;
                            data = dsrvChList.getAvailableGroupForCcid(data);
                            this._infoHub._optData[0].innerHTML = this._getLastGroupStr(data.group);
                            retObj3 = dsrvChList.getChannel({
                                ccid: aParam.param,
                                group: data.group
                            });
                        }
                        else {
                            retObj3 = dsrvChList.getChannel({
                                ccid: aParam.param,
                                group: this._currentGroupDB
                            });
                        }
                        that._infoHub.setCurrentProgramme(undefined);
                        that._getChlist(retObj3);
                        that._infoHub.showChannelList(retObj3.channel);
                    }
                    that._resetTimer();
                }
                else if (state === 'iPlate') {
                    if (aParam.param !== 'clear') {
                        var ch = dsrvChList.getCurrentChannel();
                        if (ch.group === "TV" || ch.group === "RADIO") {
                            var data = {};
                            data.ccid = aParam.param;
                            data = dsrvChList.getAvailableGroupForCcid(data);
                            retObj3 = dsrvChList.getChannel({
                                ccid: aParam.param,
                                group: data.group
                            });
                        }
                        else {
                            retObj3 = dsrvChList.getChannel({
                                ccid: aParam.param,
                                group: this._currentGroupDB
                            });
                        }
                        that._infoHub.setCurrentProgramme(retObj3);
                        that._infoHub.setCurrentChannel(retObj3.channel);
                        that._infoHub.setCurrentGroup(retObj3.group);
                        var pra = {
                            group: retObj3.group,
                            channel: retObj3.channel
                        };
                        that._draw_ChInfo(pra, 'iPlate', false);
                        that._infoHub.showIPlate();
                    }
                }
                return;
            }
            retObj = dsrvChList.getCurrentChannel();
            if (true === retObj.result) {
                // for fast load
                if (aParam.query === 'backToParent') {
                    that._$infoHub[0].style.display = 'block';
                    that._bActivate = true;
                    that._resetTimer();
                    //that._hide();
                    that._infoHub._iPlate.setFocus(true);
                    this.sendEventToEntity({
                        alEvent: 'alCheckPageVisible'
                    });
                    // update icon
                    if (that._iPlateIconUpdate === true) {
                        var iplate = that._infoHub._iPlate;
                        if (iplate) {
                            var item = iplate.getFocusedItem();
                            if (item) {
                                if (that._infoHub._viewIPlate) {
                                    iplate.clearDrawnItems();
                                    that._infoHub._viewIPlate.draw();
                                    iplate.setFocusedItemByKey(item.itemKey);
                                }
                            }
                        }
                        that._iPlateIconUpdate = false;
                    }
                }
                else if (aParam.query === 'backToLive') {
                    that._$infoHub[0].style.display = 'block';
                    that._bActivate = true;
                    that._hide();
                }
                else {
                    if (that._$infoHub[0].style.display === 'none') {
                        that._$infoHub[0].style.display = 'block';
                        that._bActivate = true;
                        that._hide();
                    }
                    that._getChlist(retObj);
                    that._draw_ChInfo(retObj, aParam.query, false);
                    that._entryPoint = aParam.query;
                }
            }
        };
        cCPgInfoHub.prototype._draw_ChInfo = function (p, aMode, aIsUpdate) {
            var that = this;
            var newChannel = true;
            that._currentChannel = p.channel;
            that._currentGroupDB = p.group;
            // use cache ???
            if (that._currentChannel && that._remChannel) {
                if (that._currentChannel.ccid === that._remChannel.ccid) {
                    newChannel = false;
                }
            }
            if (aMode === 'channelList') {
                newChannel = false;
            }
            that._remChannel = that._currentChannel;
            var startTime = Math.floor(new Date().getTime() / 1000);
            if (hx.svc.TSR.isTsrOngoing() === true) {
                var VBChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                if (!!VBChannel && VBChannel.ccid === that._currentChannel.ccid) {
                    var TSRstartTime = hx.svc.TSR.getStartTime();
                    var TSRplaybackOffset = hx.svc.TSR.getPlaybackOffset();
                    hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
                    startTime = TSRstartTime + TSRplaybackOffset;
                }
            }
            var endTime = startTime + 3600 * 12;
            if (aMode === 'iPlate') {
                var timeBase = startTime * 1000;
                that._getBackwardEvt(that._currentChannel, newChannel, timeBase, timeBase - that.THRESHOLD);
            }
            that._getEvt(that._currentChannel, newChannel, startTime, endTime, aMode);
            var cRecList = hx.svc.RECORDINGS.getCurrentRecordings();
            if (cRecList) {
                that._recordings = cRecList;
                that._infoHub.updateRecordings(cRecList);
            }
            if (that._bActivate === false || aIsUpdate === true) {
                that._show(aMode);
                $('#channelListOptBtn').find('em').text(that._getLastGroupStr(that._currentGroupDB));
                if (hx.config.useSlideText) {
                    that._doSlideAnimation($('#entryPane .highlight .chItem .chName'), that._currentChannel.name);
                }
            }
        };
        cCPgInfoHub.prototype._doSlideAnimation = function (target, text) {
            var that = this;
            that._stopSlideAnimation(target);
            var textAniValue = text.length * that.TextAniValueOffset;
            var textAniInterval = textAniValue * that.TextAniIntervalOffset;
            hx.il.slideTextAnimate(target, textAniValue, textAniInterval);
            that._textAnimate = setInterval(function () {
                hx.il.slideTextAnimate(target, textAniValue, textAniInterval);
            }, textAniInterval);
        };
        cCPgInfoHub.prototype._stopSlideAnimation = function (target) {
            var that = this;
            if (that._textAnimate) {
                clearInterval(that._textAnimate);
                that._textAnimate = null;
            }
            if (target) {
                target.stop();
                target[0].scrollLeft = 0;
            }
        };
        cCPgInfoHub.prototype._show = function (aMode) {
            // reset timer
            this._resetTimer();
            this._iPlateIconUpdate = false;
            // hide pin
            this.sendEventToEntity({
                alEvent: 'alMessageToPage',
                target: 'CPgLiveController?hidepin=pin'
            });
            this._infoHub.setCurrentGroup(this._currentGroupDB);
            this._infoHub.setCurrentChannel(this._currentChannel);
            if (aMode === 'channelList') {
                this._infoHub.showChannelList();
            }
            else if (aMode === 'iPlate') {
                this._infoHub.showIPlate();
            }
            else {
                this._infoHub.showEntry();
            }
            this._infoHub.show();
            // show & animation .doShow
            // set flag
            this._bActivate = true;
            this.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
        };
        cCPgInfoHub.prototype._resetTimer = function () {
            var _this = this;
            clearTimeout(this._timerId);
            this._timerId = setTimeout(function () {
                _this._hide();
            }, 30000);
        };
        cCPgInfoHub.prototype._hide = function () {
            var that = this;
            var state = that._infoHub.getCurrentState();
            if (state === "channelList") {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "channelList"
                    });
                }
            }
            else if (state === "infoHub") {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "infoHub"
                    });
                }
                hx.svc.CLOCK.stopClock();
            }
            else if (state === "iPlate") {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "iPlate"
                    });
                }
            }
            that._destroyGroupListMenu();
            if (hx.config.useAlphabetGroup === 1) {
                that._ag_destroyAlphabetGroupListMenu();
            }
            that._infoHub.hide();
            if (that._infoHub._channelListGroupOptBtn) {
                that._infoHub._channelListGroupOptBtn.hide();
            }
            // kill timer
            if (that._timerId !== -1) {
                clearTimeout(that._timerId);
                that._timerId = -1;
            }
            if (that.getCenterPageName() === 'CPgLiveController') {
                that.sendEventToEntity({
                    'alEvent': 'alMessageToPage',
                    'target': 'CPgLiveController?hideInfoHub'
                });
                // show pin
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgLiveController?showpin=pin'
                });
            }
            // un-set flag
            that._bActivate = false;
            that.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
        };
        cCPgInfoHub.prototype._on_alKey = function (aParam) {
            var that = this;
            var keyCode = aParam.alKey;
            var bConsumed = false, bActionPvr = false;
            var pra, bPrismLiteSMode = false;
            var ulTvSvcCount = 0, ulRadioSvcCount = 0;
            var state;
            hx.log('infohub', '[_on_alKey] = ' + keyCode);
            that._resetTimer();
            bConsumed = that._infoHub.doKey(keyCode);
            if (bConsumed) {
                return true;
            }
            state = that._infoHub.getCurrentState();
            if (hx.config.useSlideText) {
                if ((state === 'channelList') && ((keyCode === hx.key.KEY_UP) || (keyCode === hx.key.KEY_DOWN))) {
                    if (that._infoHub._$infoHubPane) {
                        that._stopSlideAnimation(that._infoHub._$infoHubPane.$channelName);
                    }
                }
            }
            switch (keyCode) {
                case hx.key.KEY_0:
                case hx.key.KEY_1:
                case hx.key.KEY_2:
                case hx.key.KEY_3:
                case hx.key.KEY_4:
                case hx.key.KEY_5:
                case hx.key.KEY_6:
                case hx.key.KEY_7:
                case hx.key.KEY_8:
                case hx.key.KEY_9:
                    that.sendEventToEntity({
                        alEvent: 'alMessageToPage',
                        target: 'CPgDcn?active=' + (keyCode - hx.key.KEY_0).toString()
                    });
                    clearTimeout(that._timerId);
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                    if (state !== 'infoHub') {
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_BACK:
                    that._infoHub.keyBack();
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (that._bActivate) {
                        that._hide();
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_OK:
                    if (state === 'infoHub') {
                        // when chaged ch graup from infohub option, if different service tune to target channel
                        var curObj = hx.svc.CH_LIST.getCurrentChannel();
                        if ((curObj.result === true) && (curObj.group !== that._currentGroupDB)) {
                            var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                                channel: that._currentChannel
                            });
                            if (bChangeable === true) {
                                hx.svc.AV_CTRL.channelChange({
                                    channel: that._currentChannel,
                                    group: that._currentGroupDB
                                });
                            }
                            else {
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
                            }
                        }
                        else {
                            if (that._infoHub.showDetailInfo() === false) {
                                var noWork;
                            }
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_UP:
                case hx.key.KEY_DOWN:
                    if (state !== 'channelList') {
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "OSDOUT",
                                "TRS": "I",
                                "OSD": state
                            });
                        }
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "OSDIN",
                                "TRS": "I",
                                "OSD": "channelList"
                            });
                        }
                        pra = {
                            group: that._currentGroupDB,
                            channel: that._currentChannel
                        };
                        that._draw_ChInfo(pra, 'channelList', false);
                        that._infoHub.showChannelList();
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                case hx.key.KEY_RIGHT:
                    bPrismLiteSMode = that._infoHub.isPrismLiteSModeOn();
                    if (bPrismLiteSMode === false) {
                        if (state !== 'iPlate') {
                            if (hx.svc.STATISTICS) {
                                hx.svc.STATISTICS.statisticsLog({
                                    "CODE": "OSDOUT",
                                    "TRS": "I",
                                    "OSD": state
                                });
                            }
                            if (hx.svc.STATISTICS) {
                                hx.svc.STATISTICS.statisticsLog({
                                    "CODE": "OSDIN",
                                    "TRS": "I",
                                    "OSD": "iPlate"
                                });
                            }
                            pra = {
                                group: that._currentGroupDB,
                                channel: that._currentChannel
                            };
                            that._draw_ChInfo(pra, 'iPlate', false);
                            that._infoHub.showIPlate();
                        }
                    }
                    else {
                        that._infoHub.keyLeftRight(keyCode);
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_OPTION:
                    if (state === 'infoHub') {
                        // channel group select
                        hx.ui.overlay.left($('#-test-infoHub'));
                        that._groupContextMenu = that._createOptListMenu();
                    }
                    else if (state === 'channelList') {
                        // channel group select
                        hx.ui.overlay.left($('#-test-infoHub'));
                        that._groupContextMenu = that._createOptListMenu();
                    }
                    else if (state === 'iPlate') {
                        that.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgNotAvailable?show'
                        });
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_PLAY:
                case hx.key.KEY_PAUSE:
                case hx.key.KEY_PLAY_SPEED_UP:
                case hx.key.KEY_PLAY_SPEED_DOWN:
                case hx.key.KEY_REWIND:
                case hx.key.KEY_FAST_FWD:
                    if (state === 'channelList') {
                        bConsumed = true;
                    }
                    else {
                        that._hide();
                        bConsumed = false;
                    }
                    break;
                case hx.key.KEY_STOP:
                    if (state === 'channelList') {
                        that._stopCurrentChannelRecord(that._currentChannel.ccid);
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_RECORD:
                    bPrismLiteSMode = that._infoHub.isPrismLiteSModeOn();
                    if ((bPrismLiteSMode === true) && (state === 'infoHub')) {
                        bActionPvr = that._infoHub.actionReservaion(keyCode);
                        if (bActionPvr === true) {
                            setTimeout(function () {
                                if (that._infoHub) {
                                    that._infoHub.updateCurrentProgramme(that._infoHub);
                                }
                            }, 300);
                            bConsumed = true;
                        }
                    }
                    break;
                case hx.key.KEY_TVRADIO_TOGGLE:
                    if (state === 'channelList') {
                        ulTvSvcCount = hx.svc.CH_LIST.getChannelCount({
                            group: 'TV'
                        }).chCnt;
                        ulRadioSvcCount = hx.svc.CH_LIST.getChannelCount({
                            group: 'RADIO'
                        }).chCnt;
                        if (that._currentGroupDB === 'RADIO') {
                            if (ulTvSvcCount > 0) {
                                that._changeChannelGroup('TV');
                            }
                        }
                        else {
                            if (ulRadioSvcCount > 0) {
                                that._changeChannelGroup('RADIO');
                            }
                        }
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_RED:
                    if (hx.config.useLiveTuningInfoShow) {
                        if (state === 'channelList') {
                            if (that._currentChannel) {
                                var _tuningInfos = that._currentChannel.tuningInfo;
                                var _tuningInfoArr = [];
                                if (_tuningInfos) {
                                    _tuningInfoArr = _tuningInfos.split(',');
                                    //var _convName = hx.il.convertSatelliteNameToShow(_tuningInfoArr[0]);
                                    _tuningInfos = _tuningInfoArr[0] + ',' + _tuningInfoArr[1] + ',' + _tuningInfoArr[2] + ',' + _tuningInfoArr[3] + ',' + _tuningInfoArr[4];
                                    var msgKey = hx.msg('codeConfirm', {
                                        first_line: hx.l('LOC_CHANNEL_INFORMATION_ID'),
                                        thirdLineFont: true,
                                        nolinejump: true,
                                        second_line: that._currentChannel.majorChannel + ', ' + that._currentChannel.name + '<br>' + _tuningInfos,
                                        auto_close: false,
                                        btn_title_arr: [hx.l('LOC_OK_ID')]
                                    });
                                    setTimeout(function () {
                                        hx.msg.close('codeConfirm', msgKey);
                                    }, 5000);
                                }
                            }
                            bConsumed = true;
                        }
                    }
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgInfoHub.prototype._stopCurrentChannelRecord = function (aData) {
            var item, recCCid = 0, findItem, i, ti;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var currentRecordingList = dsrvRecordings.getCurrentRecordings();
            var ret = false;
            if (currentRecordingList.length === 0) {
                hx.log('infohub', '_stopCurrentChannelRecord() no recording channel');
                return false;
            }
            for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                item = currentRecordingList[i];
                if (item) {
                    if (item.getChannelId() === aData.ccid) {
                        hx.log('infohub', '_stopCurrentChannelRecord() find same recording channel : ' + item.getChannelId());
                        findItem = item;
                        break;
                    }
                }
            }
            if (findItem) {
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
                ret = true;
            }
            else {
                ret = false;
            }
            return ret;
        };
        cCPgInfoHub.prototype._getEvt = function (aChannel, aRenew, aStartTime, aEndTime, aMode) {
            var _this = this;
            //chlist
            hx.svc.EPG.getProgrammes({
                isOnAir: false,
                channel: aChannel,
                startTime: aStartTime,
                endTime: aEndTime,
                listener: this,
                notUI: true,
                cb: function (result) {
                    if (result.length) {
                        _this._currentEvent = result[0][0];
                        if (_this._currentEvent) {
                            _this._infoHub.setCurrentProgramme(_this._currentEvent);
                            if (aMode === 'iPlate') {
                                _this._infoHub.updateCurrentProgramme();
                                _this._infoHub.setiPlateProgrammes(result[0], aChannel);
                                _this.updateWithIpEpg(aChannel, _this.callbackIpEpgResultForiPlate.bind(_this), Date.now(), 3);
                            }
                            else if (aMode === 'channelList') {
                                _this._infoHub.setFocusedChannelEvt(_this._currentEvent);
                                _this.updateWithIpEpg(aChannel, _this.callbackIpEpgResultForChannelList.bind(_this), _this._currentEvent.getStartTime() * 1000);
                            }
                            else {
                                _this._infoHub.updateCurrentProgramme();
                                _this.updateWithIpEpg(aChannel, _this.callbackIpEpgResultForChannelList.bind(_this), _this._currentEvent.getStartTime() * 1000);
                            }
                        }
                    }
                    result = undefined;
                }
            });
            // clear cached event
            if (aRenew === true) {
                this._currentEvent = null;
                this._infoHub.setCurrentProgramme(undefined);
                this._infoHub.setiPlateProgrammes(undefined);
            }
        };
        cCPgInfoHub.prototype._getNextEvtsAndAttach = function (aStartTime, aEndTime) {
            var _this = this;
            var bPrismLiteSMode = false, channel;
            channel = this._currentChannel;
            //debugger;
            hx.svc.EPG.getProgrammes({
                isOnAir: false,
                channel: channel,
                startTime: aStartTime,
                endTime: aEndTime,
                listener: this,
                cb: function (result) {
                    if (result[0].length > 0) {
                        bPrismLiteSMode = _this._infoHub.isPrismLiteSModeOn();
                        if (bPrismLiteSMode === true) {
                            _this._infoHub.setPrismLiteSInitDataIdx(_this._infoHub._ulDispEventIdx);
                        }
                        _this._infoHub.appendProgrammes(result[0]);
                    }
                    result = undefined;
                }
            });
        };
        cCPgInfoHub.prototype._getChListEvt = function (aChannel) {
            var _this = this;
            this._currentChannel = aChannel;
            //chlist
            var result = hx.svc.EPG.getProgrammes({
                isOnAir: true,
                filter: "P/F",
                channel: aChannel,
                listener: this,
                cb: function (result) {
                    if (result) {
                        var len = result[0].length;
                        if (len > 0) {
                            _this._infoHub.setFocusedChannelEvt(result[0][0]);
                            _this.updateWithIpEpg(aChannel, _this.callbackIpEpgResultForChannelList.bind(_this), result[0][0].getStartTime() * 1000);
                        }
                        else {
                            _this._infoHub.setFocusedChannelEvt(undefined);
                        }
                    }
                    else {
                        _this._infoHub.setFocusedChannelEvt(undefined);
                    }
                    result = undefined;
                }
            });
            //this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForChannelList.bind(this), Date.now());
        };
        cCPgInfoHub.prototype._updateOnDemandAvailableIconForiPlate = function (aIndex, aUrl) {
            var a = this;
            var iPlate = a._infoHub._iPlate;
            var focusedItem = iPlate.getFocusedItem();
            if (focusedItem) {
                var $item = iPlate._drawnItems.getItem(aIndex);
                if ($item) {
                    var div = $item.children('.iPlateItem')[0];
                    if (!!aUrl) {
                        hx.log('debug', '[_updateOnDemandAvailableIconForiPlate][' + aIndex + '] url(' + aUrl + ') updated');
                        div.append($('<img>', {
                            'class': 'availability2',
                            'src': aUrl
                        })[0]);
                    }
                }
            }
        };
        cCPgInfoHub.prototype._updateThumbnailForiPlate = function (progContext, aIndex) {
            var a = this;
            var iPlate = a._infoHub._iPlate;
            var regUrl = /url\((.+)\)/;
            var focusedItem = iPlate.getFocusedItem();
            if (focusedItem) {
                var $item = iPlate._drawnItems.getItem(aIndex);
                if ($item) {
                    var div = $item.children('.iPlateItem')[0];
                    var style = div.getAttribute('style');
                    var parsed = regUrl.exec(style)[1];
                    if (parsed) {
                        var url = progContext.getField('thumbnailURL');
                        if (url) {
                            hx.log('debug', '[_updateThumbnailForiPlate][' + aIndex + '] url(' + url + ') updated');
                            div.setAttribute('style', style.replace(regUrl, 'url(' + url + ')'));
                        }
                    }
                }
            }
            progContext = null;
        };
        cCPgInfoHub.prototype.updateWithIpEpg = function (channel, callback, time, chunk, startIdx) {
            if (time === void 0) { time = Date.now(); }
            if (chunk === void 0) { chunk = 1; }
            if (startIdx === void 0) { startIdx = 0; }
            var HOUR6 = 60 * 60 * 6 * 1000;
            var i;
            if (channel) {
                for (i = 0; i < chunk; i += 1) {
                    hx.svc.IP_EPG.getSchedules(channel.sid, time + (HOUR6 * i), 6).then(function (res) {
                        callback(channel, res, startIdx);
                    });
                }
            }
        };
        cCPgInfoHub.prototype._getChlist = function (aCurrentChannel) {
            var retObj;
            retObj = hx.svc.CH_LIST.getChlist({
                group: aCurrentChannel.group
            });
            var list = [];
            if (ENV.op === ENV.listOP.UKDTT) {
                var listChannel = retObj.chList;
                var channel;
                for (var i = 0, len = listChannel.length; i < len; i++) {
                    channel = listChannel[i];
                    if (!channel.hidden) {
                        list.push(channel);
                    }
                }
                listChannel = undefined;
                channel = undefined;
            }
            else {
                list = retObj.chList;
            }
            this._infoHub.setChannelListData(list);
            list = undefined;
            retObj = undefined;
        };
        cCPgInfoHub.prototype._getPastEvt = function (channel, renew, startTime, endTime) {
            var _this = this;
            hx.svc.EPG.getProgrammes({
                isOnAir: false,
                channel: channel,
                startTime: startTime,
                endTime: endTime,
                listener: this,
                isBackward: true,
                cb: function (result) {
                    if (result.length && result[0].length > 0) {
                        var now = new Date();
                        var ct = Math.floor(now.valueOf() / 1000);
                        var p = result[0].pop();
                        if (p.getEndTime() < ct) {
                            result[0].push(p);
                        }
                        _this._infoHub.setiPlateCatchUps(result[0]);
                    }
                    else {
                        _this._infoHub.setiPlateCatchUps(null);
                    }
                    result = undefined;
                }
            });
            // clear cached event
            if (renew === true) {
                this._infoHub.setiPlateCatchUps(null);
            }
        };
        cCPgInfoHub.prototype._getBackwardEvt = function (channel, renew, timeBase, threshold) {
            var a = this;
            var queryList = [];
            var xmls = [];
            var serviceId = channel.sid;
            var HOUR6 = 6;
            var now = new Date().getTime();
            var callback = function (aXmls) {
                var programmes = [];
                var mdsProgramme = null;
                var schedules = null, crids = null;
                var i = 0, len = 0, schedule = null, crid = null, data = null;
                if (aXmls !== null) {
                    aXmls.forEach(function (info) {
                        schedules = info.schedules;
                        crids = info.crids;
                        for (i = 0, len = schedules.length; i < len; i++) {
                            schedule = schedules[i];
                            crid = schedule.querySelector('Program').getAttribute('crid');
                            data = crids[crid];
                            data.schedule = schedule;
                            data.available = false;
                            mdsProgramme = hx.svc.IP_EPG.convert.to.programme(data, channel, null);
                            if ((mdsProgramme.startTime + mdsProgramme.duration) * 1000 < now) {
                                programmes.push(mdsProgramme);
                                (function (aProg) {
                                    setTimeout(function () {
                                        if (a._infoHub) {
                                            var prog = aProg;
                                            var ondemand = prog.ondemand;
                                            if (ondemand) {
                                                hx.svc.IP_EPG.getAIT(a._getTemplateUrl(ondemand)).then(function (xml) {
                                                    a._checkTemplateAIT(xml, prog).then(function (param) {
                                                        prog.availableIconUrl = param.url;
                                                        prog.onavailable(param.url);
                                                    }, function (err) {
                                                    });
                                                }, function (err) {
                                                });
                                            }
                                        }
                                    }, 10);
                                })(mdsProgramme);
                            }
                            mdsProgramme = null;
                        }
                    });
                    if (programmes.length > 1) {
                        programmes.sort(function (prev, after) {
                            return prev.startTime - after.startTime >= 0 ? 1 : -1;
                        });
                        for (var i = 0; i < programmes.length; i++) {
                            mdsProgramme = programmes[i];
                            mdsProgramme.onthumbnail = (function (aIndex) {
                                return function () {
                                    a._updateThumbnailForiPlate(this, aIndex);
                                };
                            })(i);
                            mdsProgramme.onavailable = (function (aIndex) {
                                return function (aUrl) {
                                    a._updateOnDemandAvailableIconForiPlate(aIndex, aUrl);
                                };
                            })(i);
                        }
                        a._infoHub.setiPlateBackwards(programmes);
                    }
                    else if (programmes.length === 1) {
                        if ((programmes[0].startTime + programmes[0].duration) * 1000 >= now) {
                            if (timeBase - (HOUR6 * 60 * 60 * 1000) >= threshold) {
                                a._getMoreBackwardEvtAndInsert(timeBase - (HOUR6 * 60 * 60 * 1000), threshold);
                            }
                            else {
                                a._infoHub.setiPlateBackwards(programmes);
                            }
                        }
                        else {
                            a._infoHub.setiPlateBackwards(programmes);
                        }
                    }
                    else {
                        if (timeBase - (HOUR6 * 60 * 60 * 1000) >= threshold) {
                            a._getBackwardEvt(channel, renew, timeBase - (HOUR6 * 60 * 60 * 1000), threshold);
                        }
                        else {
                            a._infoHub.setiPlateBackwards([]);
                        }
                    }
                    mdsProgramme = null;
                    programmes = [];
                }
                else {
                    a._infoHub.setiPlateBackwards([]);
                }
            };
            function addXml(xml) {
                if (xml) {
                    xmls.push(hx.svc.IP_EPG.parse.xml.schedule(xml));
                }
            }
            function addScheduleQueryToQueryList(serviceId, timeBase, hours) {
                var retDef = Q.defer();
                retDef.promise.then(addXml);
                var query = {
                    serviceId: serviceId,
                    startTime: timeBase,
                    hours: hours,
                    retDef: retDef
                };
                queryList.push(query);
            }
            addScheduleQueryToQueryList(serviceId, timeBase, HOUR6);
            addScheduleQueryToQueryList(serviceId, timeBase - (HOUR6 * 60 * 60 * 1000), HOUR6);
            hx.svc.IP_EPG.getScheduleList(queryList).then(function () {
                callback(xmls);
            }, function (err) {
                callback(null);
            });
            if (renew === true) {
                this._infoHub.setiPlateBackwards(null);
            }
            queryList = [];
            xmls = [];
        };
        cCPgInfoHub.prototype._getMorePastEvtAndInsert = function (aStartTime, aEndTime) {
            var that = this, channel;
            channel = that._currentChannel;
            hx.svc.EPG.getProgrammes({
                isOnAir: false,
                channel: channel,
                startTime: aStartTime,
                endTime: aEndTime,
                listener: that,
                isBackward: true,
                cb: function (result) {
                    if (result.length && result[0].length > 0) {
                        that._infoHub.insertPastProgrammes(result[0]);
                    }
                    result = undefined;
                }
            });
        };
        cCPgInfoHub.prototype._getMoreBackwardEvtAndInsert = function (aTimeBase, aThreshold) {
            var _this = this;
            var a = this, channel;
            channel = a._currentChannel;
            var queryList = [];
            var xmls = [];
            var serviceId = channel.sid;
            var HOUR6 = 6;
            var firstProgramme = a._infoHub._iPlate._dataProvider.getItem(0);
            var callback = function (aXmls) {
                var programmes = [];
                var mdsProgramme = null;
                var schedules = null, crids = null;
                var i = 0, len = 0, schedule = null, crid = null, data = null;
                if (aXmls !== null) {
                    aXmls.forEach(function (info) {
                        schedules = info.schedules;
                        crids = info.crids;
                        for (i = 0, len = schedules.length; i < len; i++) {
                            schedule = schedules[i];
                            crid = schedule.querySelector('Program').getAttribute('crid');
                            data = crids[crid];
                            data.schedule = schedule;
                            data.available = false;
                            mdsProgramme = hx.svc.IP_EPG.convert.to.programme(data, channel, null);
                            if (mdsProgramme.startTime < firstProgramme.startTime) {
                                programmes.push(mdsProgramme);
                                (function (aProg) {
                                    setTimeout(function () {
                                        if (a._infoHub) {
                                            var prog = aProg;
                                            var ondemand = prog.ondemand;
                                            //var dp = this._infoHub._iPlate._dataProvider;
                                            if (ondemand) {
                                                hx.svc.IP_EPG.getAIT(_this._getTemplateUrl(ondemand)).then(function (xml) {
                                                    _this._checkTemplateAIT(xml, prog).then(function (param) {
                                                        prog.availableIconUrl = param.url;
                                                        prog.onavailable(param.url);
                                                        //var pp = dp.getItem(param.index);
                                                        //if (pp) {
                                                        //    pp.availableIconUrl = param.url;
                                                        //    dp.updateItem(param.index, pp);
                                                        //}
                                                    }, function (err) {
                                                    });
                                                }, function (err) {
                                                });
                                            }
                                        }
                                    }, 10);
                                })(mdsProgramme);
                            }
                            mdsProgramme = null;
                        }
                    });
                    if (programmes.length > 0) {
                        programmes.sort(function (prev, after) {
                            return prev.startTime - after.startTime >= 0 ? 1 : -1;
                        });
                        for (var i = 0; i < programmes.length; i++) {
                            mdsProgramme = programmes[i];
                            mdsProgramme.onthumbnail = (function (aIndex) {
                                return function () {
                                    a._updateThumbnailForiPlate(this, aIndex);
                                };
                            })(i);
                            mdsProgramme.onavailable = (function (aIndex) {
                                return function (aUrl) {
                                    a._updateOnDemandAvailableIconForiPlate(aIndex, aUrl);
                                };
                            })(i);
                        }
                        _this._infoHub.insertPastProgrammes(programmes);
                    }
                    else {
                        if (aTimeBase - (HOUR6 * 60 * 60 * 1000) >= aThreshold) {
                            _this._getMoreBackwardEvtAndInsert(aTimeBase - (HOUR6 * 60 * 60 * 1000), aThreshold);
                        }
                        else {
                            _this._infoHub.insertPastProgrammes(programmes);
                        }
                    }
                }
                else {
                    _this._infoHub.insertPastProgrammes(programmes);
                }
                programmes = [];
                mdsProgramme = null;
            };
            function addXml(xml) {
                if (xml) {
                    xmls.push(hx.svc.IP_EPG.parse.xml.schedule(xml));
                }
            }
            function addScheduleQueryToQueryList(serviceId, timeBase, hours) {
                var retDef = Q.defer();
                retDef.promise.then(addXml);
                var query = {
                    serviceId: serviceId,
                    startTime: timeBase,
                    hours: hours,
                    retDef: retDef
                };
                queryList.push(query);
            }
            addScheduleQueryToQueryList(serviceId, aTimeBase, HOUR6);
            addScheduleQueryToQueryList(serviceId, aTimeBase - (HOUR6 * 60 * 60 * 1000), HOUR6);
            hx.svc.IP_EPG.getScheduleList(queryList).then(function () {
                callback(xmls);
            }, function (err) {
                callback(null);
            });
            queryList = [];
            xmls = [];
        };
        cCPgInfoHub.prototype._getCatchUp = function (aChannel, aRenew) {
            var that = this;
            var i;
            var recs = [];
            var list = hx.svc.RECORDINGS.getRecordingList();
            for (i = 0; i < list.length; i += 1) {
                if (aChannel.ccid === list[i].getChannelId()) {
                    recs.push(list[i]);
                }
            }
            if (recs.length > 0) {
                recs.sort(that._sortByNewerTime);
                that._infoHub.setiPlateCatchUps(recs);
            }
            else {
                that._infoHub.setiPlateCatchUps(null);
            }
            list = [];
            recs = [];
        };
        cCPgInfoHub.prototype._sortByNewerTime = function (a, b) {
            var aa = a.getStartTime();
            var bb = b.getStartTime();
            if (aa < bb) {
                return -1;
            }
            if (bb < aa) {
                return 1;
            }
            return 0;
        };
        cCPgInfoHub.prototype._startRec = function (aFocusedChannel) {
            var that = this;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
            if (bIsGetPvrHDD === false) {
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgNotAvailable?show'
                });
                return;
            }
            if (aFocusedChannel) {
                var recordings = dsrvRecordings.getCurrentRecordings();
                var i, length, recording;
                if (recordings && recordings.length) {
                    length = recordings.length;
                    for (i = 0; i < length; i += 1) {
                        recording = recordings[i];
                        if (recording.getChannelId() === focusedChannel.ccid) {
                            dsrvRecordings.stopRecord({
                                item: recording
                            });
                            return;
                        }
                    }
                }
                hx.svc.BRIDGE.requestInstanceRecording(focusedChannel);
            }
        };
        cCPgInfoHub.prototype._doReservation = function (data) {
            var that = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var scheduledList = dsrvRecordings2.getScheduledList();
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
            var tmp, pluginObj;
            tmp = data.pluginObject || data._pluginObject;
            if (tmp) {
                pluginObj = tmp.pluginObject || tmp._pluginObject;
                if (pluginObj) {
                    tmp = pluginObj;
                    pluginObj = tmp.pluginObject || tmp._pluginObject;
                    if (!pluginObj) {
                        pluginObj = tmp;
                    }
                }
                else {
                    pluginObj = tmp;
                }
            }
            dsrvRecordings2.record({
                programme: pluginObj,
                factoryType: 0,
                finished: function () {
                }
            });
        };
        cCPgInfoHub.prototype._cancelReservation = function (data) {
            var that = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var scheduledList = dsrvRecordings2.getScheduledList();
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
                dsrvRecordings2.removeScheduledRecording({
                    items: [scheduledItem],
                    finished: function () {
                    }
                });
            }
        };
        cCPgInfoHub.prototype._updateScheduledRecording = function (data) {
            var that = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var scheduledList = dsrvRecordings2.getScheduledList();
            var i, ti, scheduledItem, programmeID, foundItem;
            for (i = 0, ti = scheduledList.length; i < ti; i += 1) {
                scheduledItem = scheduledList[i];
                programmeID = scheduledItem.getProgrammeID();
                if (programmeID && (programmeID === data.programmeID)) {
                    foundItem = scheduledItem;
                    break;
                }
            }
            var rescheduleInfo = {};
            rescheduleInfo.ccid = foundItem.getChannelId();
            rescheduleInfo.startTime = foundItem.getStartTime();
            rescheduleInfo.duration = foundItem.getDuration();
            rescheduleInfo.repeatDays = foundItem.getRepeat();
            rescheduleInfo.reminder = foundItem.isReminder();
            dsrvRecordings2.updateScheduledRecording({
                item: foundItem,
                startTime: Math.floor(rescheduleInfo.startTime.getTime() / 1000),
                duration: rescheduleInfo.duration,
                ccid: rescheduleInfo.ccid,
                repeatDays: rescheduleInfo.repeatDays,
                factoryType: 0 /* ERECORDING */,
                finished: function (result) {
                    // hx.log('infohub', '_______updateScheduledRecording : ' + result);
                }
            });
        };
        cCPgInfoHub.prototype._cancelRecording = function (focusedChannel) {
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var currentRecordingList = dsrvRecordings2.getCurrentRecordings();
            if (currentRecordingList && currentRecordingList.length) {
                var ccid = focusedChannel.ccid;
                var i, ti, recordingItem;
                for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                    recordingItem = currentRecordingList[i];
                    if (recordingItem.getChannelId() === ccid) {
                        dsrvRecordings2.stopRecord({
                            'item': recordingItem
                        });
                        break;
                    }
                }
            }
        };
        cCPgInfoHub.prototype._responseMsgHide = function () {
            if (this._responseObj) {
                hx.msg.close('response', this._responseObj);
                this._responseObj = null;
            }
        };
        cCPgInfoHub.prototype._responseMsgShow = function (aText) {
            this._responseMsgHide();
            this._responseObj = hx.msg('response', {
                text: aText,
                auto_close: true,
                timeout: 3000,
                dia_class: 'res vaTop',
                callback_fn: function (p) {
                    var noWork;
                }
            });
        };
        cCPgInfoHub.prototype._changeChannelGroup = function (aGroup) {
            var retObj = hx.svc.CH_LIST.getChlist({
                group: aGroup
            });
            if (retObj.chList.length > 0) {
                var list = [];
                if (ENV.op === ENV.listOP.UKDTT) {
                    var listChannel = retObj.chList;
                    var channel = null;
                    for (var i = 0, len = listChannel.length; i < len; i++) {
                        channel = listChannel[i];
                        if (!channel.hidden) {
                            list.push(channel);
                        }
                    }
                }
                else {
                    list = retObj.chList;
                }
                this._infoHub.setChannelListData(list);
                var ch = (retObj.chList.length === 0) ? null : this._currentChannel;
                var pra = {
                    group: aGroup,
                    channel: ch
                };
                if (this._infoHub.getCurrentState() === 'infoHub') {
                    pra.channel = retObj.chList[0];
                    // set to first channel of target group
                    this._draw_ChInfo(pra, 'infoHub', true);
                }
                else {
                    this._draw_ChInfo(pra, 'channelList', true);
                }
            }
            retObj = null;
        };
        cCPgInfoHub.prototype._createOptListMenu = function () {
            var that = this, itemList = [], groupList = [], data, i, length, startIndex, groupName;
            var bIsFavroureGroup = false, ulCurGroupDB, count;
            for (i = 0, length = that._groupDB.length; i < length; i += 1) {
                count = hx.svc.CH_LIST.getChannelCount({
                    group: that._groupDB[i]
                }).chCnt;
                if ((hx.config.useAlphabetGroup === 1) && (that._groupDB[i] === 'A-Z')) {
                    count = hx.svc.CH_LIST.getChannelCount({
                        group: 'AGALL'
                    }).chCnt;
                }
                if (count > 0) {
                    bIsFavroureGroup = that._optListMenu_checkFavourite(that._groupDB[i]);
                    if (bIsFavroureGroup === true) {
                        groupName = hx.svc.CH_LIST.getFavListName({
                            favtype: that._groupDB[i]
                        });
                        if (groupName) {
                            that._strChannelGroupName[i] = groupName;
                        }
                    }
                    else {
                        groupName = that._strChannelGroupName[i];
                    }
                    data = {
                        text: groupName,
                        msg: '',
                        value: i
                    };
                    itemList.push(data);
                    groupList.push(that._groupDB[i]);
                }
            }
            ulCurGroupDB = that._currentGroupDB;
            if (hx.config.useAlphabetGroup === 1) {
                ulCurGroupDB = that._ag_checkAlphabetGroup(ulCurGroupDB);
            }
            startIndex = groupList.indexOf(ulCurGroupDB);
            var relationTarget = '#channelListOptBtn';
            var contextMenu = hx.ui.ContextMenu({
                compose: this,
                renderTo: $(that.$container),
                relation: $(relationTarget),
                direction: 'up',
                startIndex: startIndex,
                items: itemList,
                onOk: function (index, argObject) {
                    var dbIndex = that._strChannelGroupName.indexOf(argObject.text);
                    var bChgAlphabetGroup = that._ag_getSupportAlphabetChlist(dbIndex);
                    if (bChgAlphabetGroup === false) {
                        that._changeChannelGroup(that._groupDB[dbIndex]);
                        $('#channelListOptBtn').find('em').text(argObject.text);
                    }
                    this.onCancel();
                    if (bChgAlphabetGroup === true) {
                        // channel Alphabet group select
                        hx.ui.overlay.left($('#-test-infoHub'));
                        that._alphabetgroupContextMenu = that._ag_createOptAlphabetListMenu();
                        var $detach = $('#channelListOptBtn').detach();
                        $('#-test-infoHub').after($detach);
                    }
                },
                onCancel: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._groupContextMenu = null;
                },
                onHideOption: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._groupContextMenu = null;
                },
                onKeyUpdated: function () {
                    that._resetTimer();
                }
            });
            contextMenu.show({});
            return contextMenu;
        };
        cCPgInfoHub.prototype._destroyGroupListMenu = function () {
            if (this._groupContextMenu) {
                hx.ui.overlay.hide();
                this._groupContextMenu.destroy();
                this._groupContextMenu = undefined;
            }
        };
        cCPgInfoHub.prototype._getLastGroupStr = function (aGroup) {
            var that = this, i, lastGroup;
            if (aGroup) {
                lastGroup = aGroup;
            }
            else {
                var curObj = hx.svc.CH_LIST.getCurrentChannel();
                lastGroup = curObj.group;
            }
            for (i = 0; i < that._groupDB.length; i += 1) {
                if (lastGroup === that._groupDB[i]) {
                    return that._strChannelGroupName[i];
                }
            }
            if (hx.config.useAlphabetGroup === 1) {
                for (i = 0; i < that._alphabetGroupDB.length; i += 1) {
                    if (lastGroup === that._alphabetGroupDB[i]) {
                        var szAlphabetText = hx.l('LOC_A_Z_ID');
                        return szAlphabetText;
                    }
                }
            }
            return that._strChannelGroupName[0];
            // exception
        };
        cCPgInfoHub.prototype._setCurrentChannel = function (aChannel) {
            this._currentChannel = aChannel;
        };
        cCPgInfoHub.prototype._on_dlDetailInfo = function (aParam) {
            throw "this is no longer valid";
        };
        cCPgInfoHub.prototype._on_dlUpdateList = function (aParam) {
            var that = this, list = aParam.alParam.list;
            var channel = aParam.channel;
            var i;
            var recordings = null;
            var sortRecs = [];
            var idx = 0;
            if (that._lKeyRecGetList) {
                that._lKeyRecGetList = undefined;
            }
            if (aParam.action === 'getList') {
                for (i = 0; i < list.length; i += 1) {
                    if (channel.ccid === list[i].channel.ccid) {
                        recordings = list[i].recordings;
                        break;
                    }
                }
                if (recordings && recordings.length > 0) {
                    for (i = 0; i < recordings.length; i += 1) {
                        idx = recordings.length - 1 - i;
                        sortRecs.push(recordings[idx]);
                    }
                }
                else {
                    // "No Recording in this channel."
                    sortRecs = recordings;
                }
                that._infoHub.setiPlateCatchUps(sortRecs);
            }
        };
        cCPgInfoHub.prototype._on_evtSetChannel = function (aParam) {
            var p = aParam;
            var state = this._infoHub.getCurrentState();
            if (this._bActivate === true && state === 'infoHub') {
                this._draw_ChInfo({
                    'channel': p.channel,
                    'group': this._currentGroupDB
                }, 'infoHub', true);
            }
        };
        cCPgInfoHub.prototype._on_ChannelListUpdate = function () {
            if (this._bActivate) {
                this._hide();
            }
        };
        cCPgInfoHub.prototype._init_Data = function () {
            var that = this, i = 0, bIsFavGroup = false, groupName;
            for (i = 0; i < hx.config.CPgInfoHub.chGroup.groupName.length; i += 1) {
                that._strChannelGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.groupName[i]);
            }
            for (i = 0, length = that._groupDB.length; i < length; i += 1) {
                bIsFavGroup = that._optListMenu_checkFavourite(that._groupDB[i]);
                if (bIsFavGroup) {
                    groupName = hx.svc.CH_LIST.getFavListName({
                        favtype: that._groupDB[i]
                    });
                    if (groupName) {
                        that._strChannelGroupName[i] = groupName;
                    }
                }
            }
            for (i = 0; i < hx.config.CPgInfoHub.chGroup.favlistName.length; i += 1) {
                that._favlistname[i] = hx.l(hx.config.CPgInfoHub.chGroup.favlistName[i]);
            }
            // Init. Alphabet List Name
            if (hx.config.useAlphabetGroup === 1) {
                for (i = 0; i < hx.config.CPgInfoHub.chGroup.alphabetGroupName.length; i += 1) {
                    if (hx.config.CPgInfoHub.chGroup.alphabetGroupsDB[i] === 'AGALL') {
                        that._strAlphabetGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.alphabetGroupName[i]);
                    }
                    else {
                        that._strAlphabetGroupName[i] = hx.config.CPgInfoHub.chGroup.alphabetGroupName[i];
                    }
                }
            }
        };
        cCPgInfoHub.prototype._optListMenu_checkFavourite = function (aParam) {
            var ret = false;
            if (aParam === 'Favourite1') {
                ret = true;
            }
            else if (aParam === 'Favourite2') {
                ret = true;
            }
            else if (aParam === 'Favourite3') {
                ret = true;
            }
            else if (aParam === 'Favourite4') {
                ret = true;
            }
            else if (aParam === 'Favourite5') {
                ret = true;
            }
            else {
                ret = false;
            }
            return ret;
        };
        //------------------------------------------------------------------------------------------------------
        // Alphabet Group List Function
        //------------------------------------------------------------------------------------------------------
        cCPgInfoHub.prototype._ag_createOptAlphabetListMenu = function () {
            var that = this, itemList = [], itemListView = [], groupList = [], data;
            var i, length, startIndex, groupName;
            var bIsFavroureGroup = false, ulCurIndex = 0, curObject, count;
            for (i = 0, length = that._alphabetGroupDB.length; i < length; i += 1) {
                count = hx.svc.CH_LIST.getChannelCount({
                    group: that._alphabetGroupDB[i]
                }).chCnt;
                if (count > 0) {
                    groupName = that._strAlphabetGroupName[i];
                    data = {
                        text: groupName,
                        msg: '',
                        value: i
                    };
                    itemList.push(data);
                    groupList.push(that._alphabetGroupDB[i]);
                }
            }
            startIndex = groupList.indexOf(that._currentGroupDB);
            if (startIndex < 0) {
                startIndex = 0;
            }
            var contextMenu = hx.ui.ContextMenu({
                compose: this,
                renderTo: $(that.$container),
                relation: $('#channelListOptBtn'),
                direction: 'up',
                startIndex: startIndex,
                items: itemList,
                mode: 'scroll',
                column: 11,
                onOk: function (index, argObject) {
                    var dbIndex = that._strAlphabetGroupName.indexOf(argObject.text);
                    var szAlphabetText = hx.l('LOC_A_Z_ID');
                    that._ag_changeAlphabetChannelGroup(that._alphabetGroupDB[dbIndex]);
                    $('#channelListOptBtn').find('em').text(szAlphabetText);
                    this.onCancel();
                },
                onCancel: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._ag_refreshOptAlphabetBtn();
                    that._alphabetgroupContextMenu = null;
                },
                onHideOption: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._ag_refreshOptAlphabetBtn();
                    that._alphabetgroupContextMenu = null;
                },
                onKeyUpdated: function (param) {
                    that._resetTimer();
                }
            });
            contextMenu.show({});
            return contextMenu;
        };
        cCPgInfoHub.prototype._ag_destroyAlphabetGroupListMenu = function () {
            if (this._alphabetgroupContextMenu) {
                hx.ui.overlay.hide();
                this._ag_refreshOptAlphabetBtn();
                this._alphabetgroupContextMenu.destroy();
                this._alphabetgroupContextMenu = undefined;
            }
        };
        cCPgInfoHub.prototype._ag_refreshOptAlphabetBtn = function () {
            //var $detach = $('#channelListOptBtn').detach();
            //$('#channelList').after($detach);
        };
        cCPgInfoHub.prototype._ag_getSupportAlphabetChlist = function (aParam) {
            var bReturn = false;
            if (hx.config.useAlphabetGroup === 1) {
                if (this._groupDB[aParam] === 'A-Z') {
                    bReturn = true;
                }
                else {
                    bReturn = false;
                }
            }
            else {
                bReturn = false;
            }
            return bReturn;
        };
        cCPgInfoHub.prototype._ag_checkAlphabetGroup = function (aParam) {
            var i = 0;
            for (i = 0; i < this._alphabetGroupDB.length; i += 1) {
                if (this._alphabetGroupDB[i] === aParam) {
                    return 'A-Z';
                }
            }
            return aParam;
        };
        cCPgInfoHub.prototype._ag_changeAlphabetChannelGroup = function (aGroup) {
            var retObj;
            retObj = hx.svc.CH_LIST.getChlist({
                group: aGroup
            });
            if (retObj.chList.length > 0) {
                this._infoHub.setChannelListData(retObj.chList);
                var ch = (retObj.chList.length === 0) ? null : retObj.chList[0];
                var pra = {
                    group: aGroup,
                    channel: ch
                };
                this._draw_ChInfo(pra, 'channelList', true);
            }
            retObj = null;
        };
        return cCPgInfoHub;
    })(__PAGE_KERNEL__);
    hx.al.CPgInfoHub = cCPgInfoHub;
    return cCPgInfoHub;
});
