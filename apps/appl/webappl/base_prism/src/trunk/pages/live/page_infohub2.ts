/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/controls.d.ts" />
///<reference path="../../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />
///<reference path="../../layer_domain/service/ip_epg/adapter.d.ts" />
// file name : pages/live/infohub2.js
/*jslint sloppy:true, nomen:true, vars:true */
declare var hx;
declare var prism;

import __PAGE_KERNEL__ = require("pages/page_kernel");
import __MODEL__ = require("model/model");
import __CONTROLS__ = require('pages/util/controls');
import __PROVIDER__ = require('pages/util/provider');

var state : string = 'infoHub';
var focusedChannel : any = null;
var focusedProgramme : any = null;
var createDateTimeIfNecessary = function(a) {
    if (!a._$dateTimePane) {
        var $dateTimePane : any = $('<div>', {
            'class' : 'dateTimePane'
        }), $date = $('<div>', {
            'class' : 'date'
        }).appendTo($dateTimePane), $time = $('<div>', {
            'class' : 'time'
        }).appendTo($dateTimePane);
        $dateTimePane.$date = $date;
        $dateTimePane.$time = $time;
        a._$dateTimePane = $dateTimePane;
    }
    a._$self.append(a._$dateTimePane);
};
var createInfoHubIfNecessary = function(a) {
    if (!a._$infoHubPane) {
        var $infoHubPane : any = $('<div>', {
            'id' : 'entryPane'
        }), $arrowUp = $('<div>', {
            'class' : 'arrowUp'
        }).appendTo($infoHubPane), $highlight = $('<div>', {
            'class' : 'highlight'
        }).appendTo($infoHubPane), $arrowRight = $('<div>', {
            'class' : 'arrowRight'
        }).appendTo($highlight), $channelItem = $('<div>', {
            'class' : 'chItem'
        }).appendTo($highlight), $channelNum = $('<div>', {
            'class' : 'chNum'
        }).appendTo($channelItem), $channelName = $('<div>', {
            'class' : 'chName'
        }).appendTo($channelItem), $channelItemDummy = $('<div>', {
            'class' : 'chItem'
        }).appendTo($infoHubPane), $programmeName = $('<div>', {
            'class' : 'progName'
        }).appendTo($infoHubPane), $programmeProgress = $('<div>', {
            'class' : 'progProgress'
        }).appendTo($infoHubPane), $programmeStartTime = $('<div>', {
            'class' : 'progStartTime'
        }).appendTo($programmeProgress), $programmeEndTime = $('<div>', {
            'class' : 'progEndTime'
        }).appendTo($programmeProgress), $programmeBar = $('<div>', {
            'class' : 'progBar'
        }).appendTo($programmeProgress), $programmePos = $('<div>', {
            'class' : 'progPos'
        }).appendTo($programmeBar), $arrowDown = $('<div>', {
            'class' : 'arrowDown'
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
var createInfoHubIconIfNecessary = function(a) {
    if (!a._$statusPan) {
        var $statusPan : any = $('<div>', {
            'class' : 'statusPan'
        }), $statusText1 = $('<p>', {
            'class' : 'statusText'
        }).appendTo($statusPan), $statusText2 = $('<p>', {
            'class' : 'statusText'
        }).appendTo($statusPan);
        $statusPan.$statusText1 = $statusText1;
        $statusPan.$statusText2 = $statusText2;
        a._$statusPan = $statusPan;
    }
    a._$self.append(a._$statusPan);
};
var createTimeBarIfNecessary = function(a) {
    if (!a._$timeBarEntry) {
        var $timeBarEntry : any = $('<div>', {
            'class' : 'timeBarEntry'
        });
        var $timeBar : any = $('<div>', {
            'class' : 'timeBar'
        }).appendTo($timeBarEntry);
        var $timeBar2 : any = $('<div>', {
            'class' : 'timeBar2'
        }).appendTo($timeBar);
        var $timeBarStart : any = $('<div>', {
            'class' : 'timeBarStart'
        }).appendTo($timeBar);
        var $timeBarCursor : any = $('<p>', {
            'class' : 'timeBarCursor'
        }).appendTo($timeBar);
        var $timeBarDate = $('<em>', {
            'class' : 'timeBarDate'
        }).appendTo($timeBarCursor);
        var $catchUp : any = $('<div>', {
            'class' : 'catchupInfo'
        }).appendTo($timeBarEntry);
        var $catchUp2 : any = $('<div>', {
            'class' : 'arrIco'
        }).appendTo($catchUp);
        if (hx.config.useBackwardEpg === true) {
            var $catchUp3 = $('<div>', {
                'class' : 'arrTxt',
                'text' : hx.l('LOC_CATCHUP_VIEW_ID')
            }).appendTo($catchUp);
        } else {
            var $catchUp3 = $('<div>', {
                'class' : 'arrTxt',
                'text' : hx.l('LOC_CRECORDINGS_ID')
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
var createCursorIfNecessary = function(a) {
    if (!a._$cursorEntry) {
        var $cursorEntry = $('<div>', {
            'class' : 'cursor'
        });
        a._$cursorEntry = $cursorEntry;
    }
    a._$self.append(a._$cursorEntry);
};
var removeDateTime = function(a) {
    if (a._$dateTimePane) {
        a._$dateTimePane.detach();
        hx.svc.CLOCK.stopClock();
    }
};
var removeInfoHub = function(a) {
    if (a._$infoHubPane) {
        a._$infoHubPane.detach();
    }
};
var removeInfoHubIcon = function(a) {
    if (a._$statusPan) {
        a._$statusPan.detach();
    }
};
var removeTimeBar = function(a) {
    if (a._$timeBarEntry !== undefined) {
        a._$timeBarEntry.detach();
    }
};
var removeCursor = function(a) {
    if (a._$cursorEntry !== undefined) {
        a._$cursorEntry.detach();
    }
};
var updateEntryPane = function(a) {
    if (a._$infoHubPane !== undefined) {
        if (state === 'channelList') {
            a._$infoHubPane.$programmeName.detach();
            a._$infoHubPane.$programmeProgress.detach();
            a._$infoHubPane.$arrows.removeClass('show');
            a._$infoHubPane.$highlight.removeClass('on');
        } else if (state === 'iPlate') {
            if (!a._$infoHubPane.find('.progName').length) {
                a._$infoHubPane.$programmeName[0].innerHTML = '';
                a._$infoHubPane.append(a._$infoHubPane.$programmeName);
            }
            if (!a._$infoHubPane.find('.progProgress').length) {
                //a._$infoHubPane.$programmeProgress[0].innerHTML = '';
                a._$infoHubPane.append(a._$infoHubPane.$programmeProgress);
            }
            a._$infoHubPane.$arrows.removeClass('show');
        } else {
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
var updateDateTime = function(a) {
    if (!a._$dateTimePane) {
        return;
    }
    var now = new Date();
    var date = dateFormat(now, "dd.mm.yyyy");
    var time = dateFormat(now, "ddd HH:MM").toUpperCase();
    a._$dateTimePane.$date.text(date);
    a._$dateTimePane.$time.text(time);
};
var updateCurrentChannel = function(a) {
    if (a._currentChannel) {
        var channelNum = hx.il.makeDigitNumber(a._currentChannel.majorChannel);
        a._$infoHubPane.$channelNum.text(channelNum);
        a._$infoHubPane.$channelName.text(a._currentChannel.name);
    }
};
var updateCurrentProgramme = function(a) {// info hub & iplate
    // check icon
    var isCas : boolean = false;
    var isLock : boolean = false, bPrismLiteSMode : boolean = a.isPrismLiteSModeOn();
    var channel : any = a._currentChannel, casInfo : string = '';
    var evtSchedule : string = '';

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
            } else if (curP.hasSchedule()) {
                evtSchedule = '<div class="scheduleRecord"></div>';
            }
        }
    }

    var chCas : string = (isCas === true) ? '<div class="' + casInfo + '"></div>' : '';
    var chLock : string = (isLock === true) ? '<div class="channelLock"></div>' : '';
    var space : string = (evtSchedule + chCas + chLock !== '') ? ' ' : '';

    if (a._currentProgramme) {
        var p = a._currentProgramme;
        var sT : number = 0;
        var eT : number = 0;
        var names : string = '';
        var startTime : string = '';
        var endTime : string = '';
        var programmeDate;

        if (p.catchup) {// is catch up
            if (p.noCatchup) {
                // no recordings message
            } else if (hx.config.useBackwardEpg === true) {
                sT = p.startTime;
                eT = p.startTime + p.duration;
                names = p.name;
                startTime = dateFormat(new Date(sT * 1000), "ddd dd.mm.yyyy / HH:MM") + ' - ' + dateFormat(new Date(eT * 1000), "HH:MM");
                var duration = p.duration;
                var hour = Math.floor(duration / 3600);
                var min = Math.floor((duration % 3600) / 60);
                if (hour !== 0 && min !== 0) {
                    startTime += ' (' + hour + 'h ' + min + 'm)';
                } else if (hour !== 0) {
                    startTime += ' (' + hour + 'h)';
                } else {
                    startTime += ' (' + min + 'm)';
                }
            } else {
                sT = p.getStartTime();
                eT = p.getEndTime();
                names = p.getTitle();
                var dur : number = p.getDuration();
                var h : string = Math.floor((dur / 3600)).toString() + 'h';
                var m : string = Math.floor((dur % 3600 + 30) / 60).toString() + 'm';
                // +30 is round off
                startTime = h + m + ' / ' + dateFormat(new Date(sT), "ddd.dd.mm.yyyy");
            }
        } else {
            sT = p.getStartTime();
            eT = p.getEndTime();
            names = p.getName();
            if (p.isLive() === true || p.isPast() === true) {
                startTime = dateFormat(new Date(sT * 1000), "HH:MM");
                endTime = dateFormat(new Date(eT * 1000), "HH:MM");
            } else {
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
    } else {
        a._$infoHubPane.$programmeName[0].innerHTML = chLock + chCas;
        a._$infoHubPane.$programmeStartTime.text('');
        a._$infoHubPane.$programmeEndTime.text('');
    }

    updateCurrentProgress(a);
    updateTimeBarPosition(a);
    updateCursor(a);
};

var updateCurrentProgress = function(a) {
    if (a._currentProgramme) {
        var p = a._currentProgramme;
        var pos : number = 0;
        var needProgressbar : boolean = false;
        if (p.catchup) {// is catch up
            if (p.noCatchup) {
                // no recordings
                var noWork;
            } else {
                pos = 0;
            }
        } else {
            pos = p.getPercentage();
            if (!!p.isLive && p.isLive() === true) {
                needProgressbar = true;
            }
            if (p.isPast() === true) {
                needProgressbar = true;
                if (hx.svc.TSR.isTsrOngoing() === true) {
                    var TSRstartTime : number = hx.svc.TSR.getStartTime();
                    var TSRplaybackOffset : number = hx.svc.TSR.getPlaybackOffset();
                    var TSRTime : number = TSRstartTime + TSRplaybackOffset;
                    //TSRTime  =  ((new Date()).getTime() / 1000) - (60*30); // test only
                    pos = p.getPercentageByTime(TSRTime);
                }
            }
        }
        pos = (pos > 100 ) ? 100 : pos;
        pos = (pos < 0 ) ? 0 : pos;
        a._$infoHubPane.$programmePos.width(pos + '%');
        if (needProgressbar === true) {
            a._$infoHubPane.$programmeBar.addClass('show');
            if (!!a._$timeBarEntry && !!a._$timeBarEntry.$catchUp) {
                a._$timeBarEntry.$catchUp.addClass('show');
            }
        } else {
            a._$infoHubPane.$programmeBar.removeClass('show');
            if (!!a._$timeBarEntry && !!a._$timeBarEntry.$catchUp) {
                a._$timeBarEntry.$catchUp.removeClass('show');
            }
        }

    } else {
        a._$infoHubPane.$programmeBar.removeClass('show');
        if (!!a._$timeBarEntry && !!a._$timeBarEntry.$catchUp) {
            a._$timeBarEntry.$catchUp.removeClass('show');
        }
    }
};
var updateTimeBarPosition = function(a) {
    var loc : any  = ['left:0px', 'left:288px', 'left:524px', 'left:760px', 'left:996px', 'left:1232px'];
    var timeBarLoc : number , i : number, drawnItems : any ;
    var now : any  = new Date();
    var ct : number = Math.floor(now.valueOf() / 1000);
    var p : any  = a._currentProgramme;
    if (a._currentProgramme) {
        // check live
        if (p.catchup === undefined) {// is schedule ??
            if (ct >= p.getStartTime() && ct <= p.getEndTime()) {// live
                timeBarLoc = 0;
            }
        }
        // find position
        if (timeBarLoc === undefined && a._iPlate) {// get live position
            if (a._iPlate._drawnItems) {
                drawnItems = a._iPlate._drawnItems.getItemsByArray();
                if (drawnItems) {
                    for ( i = 0; i < drawnItems.length; i += 1) {
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
            if (p.catchup === undefined) {// is schedule ??
                if (ct < p.getEndTime()) {
                    timeBarLoc = 0;
                }
            }
        }
    }
    if (a._currentProgramme && timeBarLoc !== undefined) {
        var startTime : number;
        if (p.catchup) {// is catch up
            if (p.noCatchup) {
                // do not update anything
            } else {
                startTime = ct;
            }
            startTime = ct;
        } else {
            startTime = p.getStartTime();
        }
        var pos : number = (startTime - ct) / (3600 * 24 * 8) * 100;
        // 8 days
        pos = Math.floor(pos);
        pos = (pos > 100 ) ? 100 : pos;
        pos = (pos < 0 ) ? 0 : pos;
        var pro : string = 'left:' + (pos * 0.88) + '%';
        var proW : string = 'width:' + (pos * 0.88) + '%';
        // 0.88 : to avoid hiding date
        var strStartTime = dateFormat(new Date(startTime * 1000), "ddd dd.mm");
        if (a._$timeBarEntry) {
            if (!!p.isLive && p.isLive()) {
                a._$timeBarEntry.$timeBarCursor.addClass('onAir');
                a._$timeBarEntry.$timeBarDate.addClass('onAir');
            } else {
                a._$timeBarEntry.$timeBarCursor.removeClass('onAir');
                a._$timeBarEntry.$timeBarDate.removeClass('onAir');
                a._$timeBarEntry.$timeBarDate.text(strStartTime);
            }
            a._$timeBarEntry.$timeBar[0].style.cssText = loc[timeBarLoc];
            a._$timeBarEntry.$timeBar2[0].style.cssText = proW;
            a._$timeBarEntry.$timeBarCursor[0].style.cssText = pro;
            a._$timeBarEntry.$timeBar.addClass('show');
        }
    } else {
        if (a._$timeBarEntry) {
            a._$timeBarEntry.$timeBar.removeClass('show');
        }
    }
};
var updateCursor = function(a) {
    if (a._currentProgramme) {
        if (a._$cursorEntry) {
            a._$cursorEntry.addClass('show');
        }
    } else {
        if (a._$cursorEntry) {
            a._$cursorEntry.removeClass('show');
        }
    }
};
var updateStatusIcon = function(a) {
    var rec : string = '', liv : string = '';
    if (a._$statusPan) {
        if (a._recordings) {
            var len : number = a._recordings.length;
            if (len > 0) {
                var recIcon : string = '<em class="statusIconRec"></em>';
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
var checkChannelsChangeable = function(viewingItemInfos) {
    var i : number, channelChangable : any ;
    viewingItemInfos = viewingItemInfos instanceof Array ? viewingItemInfos : [viewingItemInfos];
    for ( i = 0; i < viewingItemInfos.length; i += 1) {
        var viewingItemInfo = viewingItemInfos[i];
        if (viewingItemInfo) {
            channelChangable = hx.svc.CH_LIST.checkChannelChangable({
                channel : viewingItemInfo.item
            });
            if (viewingItemInfo.$item) {
                if (channelChangable) {
                    viewingItemInfo.$item.removeClass('dis');
                } else {
                    viewingItemInfo.$item.addClass('dis');
                }
            }
        }
    }
};
var $recIcon = $('<div>', {
    'class' : 'chIconRec'
});
enum TScheduledFactoryType {
    ERECORDING = 0,
    EREMIND = 1,
    ESERIES_RECORDING = 10
}
class CMainGroupControl extends __CONTROLS__.CGroupControl {
    private _$infoHubPane: any;
    private _$dateTimePane: any;
    private _$statusPan: any;
    private _$timeBarEntry: any;
    private _$cursorEntry: any;
    private _currentChannel;
    private _channelList;
    private _parent: cCPgInfoHub;
    private _iPlate;
    private _viewIPlate;
    private _iplateLivePos;
    private _optIcon;
    private _optText;
    private _optData;
    private _channelListGroupOptBtn;
    private _channelListData;
    private _currentGroup;
    private _currentProgramme;
    private _updatedprogrammes;
    private _receivedProgrammes;
    private _ulEventCount;
    private _ulDispEventIdx;
    private _ulCatchUpsCount;
    private _receivedCatchUps;
    private _receivedBackwards;
    private _updatedCatchUps;
    private _updatedBackwards;
    private _recordings;
    private bPrismLiteSmode;
    private _TYPE;
    private _IMAGE;
    constructor($el, parent) {
        super($el);
        this._parent = parent;
        this.createChannelList();
        this.createIPlate();
        this.setChildControl([]);
    }
    destroy() {
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
        super.destroy.call(a);
    }
    createChannelList() {
        var a = this;
        var b = this._parent;
        var channelList = new __CONTROLS__.CCarouselControl(null);

        channelList.setId('channelList');
        channelList.setDrawParam({
            viewCount : 7,
            anchorIndex : 2,
            itemHeight : 54,
            anchorHeight : 270,
            align : 'vertical',
            animation : true,
            animationInterval : 0.15,
            maxKeyQueueCount : 1,
            transparentAnchor : true
        });
        channelList.setAnchorDrawer(function(param) {
            var that = this, bPrismLiteSMode : boolean = false;

            bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                that.drawPrismLiteSChannelList(param);
            } else {
                that.drawPrismChannelList(param);
            }
        });
        channelList.setDataDrawer(function(param) {
            var channelNum : string = '', channelName : string = '';
            if (param.item) {
                channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                channelName = param.item.name;
            }
            param.$item.addClass('chItem');
            param.$item.append($('<div>', {
                'class' : 'chNum'
            }).text(channelNum));
            param.$item.append($('<div>', {
                'class' : 'chName'
            }).text(channelName));
            checkChannelsChangeable(param);
        });
        channelList.onFocusStartToChange = function(param) {
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

            var bPrismLiteSMode : boolean = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                // initialize
                a._channelList._$anchor.find('.chEvtName').html('');
                a._channelList._$anchor.find('.chEvtTime').html('');
                a._channelList._$anchor.find('.chThumbnail').html('');
            } else {
                // initialize
                var defaultThum = '<img src="' + a.getDefaultThumbnail() + '">';
                a._channelList._$anchor.find('.chEvtName').html('');
                a._channelList._$anchor.find('.chThumbnail').html(defaultThum);
            }
            b._resetTimer();
        };
        channelList.onFocusChanged = function(param) {
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
        channelList.onItemSelected = function(param) {
            var that = this;
            var channel : any  = that._rollDataProvider.curItem();
            var curObj : any  = hx.svc.CH_LIST.getCurrentChannel();

            if ((curObj.result === true) && (curObj.channel.ccid === channel.ccid) && (curObj.group === b._currentGroupDB)) {
                // exit list
                if (b._bActivate) {
                    b._hide();
                }
                return;
            }

            // check tunable
            var bChangeable : any  = hx.svc.CH_LIST.checkChannelChangable({
                channel : channel
            });
            if (bChangeable === false) {
                if (hx.config.useTVStreaming === false) {
                    hx.msg('response', {
                        text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                        auto_close : true,
                        close_time : 3000
                    });
                } else {
                    hx.msg('response', {
                        text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                        auto_close : true,
                        close_time : 3000
                    });
                }
                return;
            }
            if (channel) {
                hx.svc.AV_CTRL.channelChange({
                    channel : channel,
                    group : b._currentGroupDB
                });
                if (b._bActivate) {
                    b._hide();
                }
            }
        };
        (<any>channelList).doKeyRec = function() {
            b._startRec(focusedChannel);
            return true;
        };
        (<any>channelList).drawPrismChannelList = function(param) {
            focusedChannel = a._currentChannel = param.currentItem.data;
            var channelNum = hx.il.makeDigitNumber(focusedChannel.majorChannel);
            var isCas : boolean, isLock : boolean;
            var programmeName : string = '', thumbnail : string = '', logo : string = '';
            var bRecording : boolean = false, thumbnailUrl : string = '';

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
                } else {
                    if (!!focusedProgramme.getTitle) {
                        programmeName = focusedProgramme.getTitle();
                    }
                    if (focusedProgramme.getThumbnailUrl) {
                        thumbnailUrl = focusedProgramme.getThumbnailUrl();
                    } else {
                        thumbnailUrl = a.getDefaultThumbnail();
                    }
                    thumbnail = '<img src="' + thumbnailUrl + '" ' + 'onerror="this.src=\'' + a.getDefaultThumbnail() + '\';" />';
                }
            } else {
                thumbnail = '<img src="' + a.getDefaultThumbnail() + '">';
            }
            var $channelItem = $('<div>', {
                'class' : 'chItem'
            }).append($('<div>', {
                'class' : 'chEvtName'
            }).text(programmeName)).append($('<div>', {
                'class' : 'chThumOutline'
            })).append($('<div>', {
                'class' : 'chThumbnail'
            }).html(thumbnail));

            if (a.checkRecordings(focusedChannel.ccid)) {
                $channelItem.find('.chThumbnail').append($recIcon);
            }

            if (ENV.op != ENV.listOP.UKDTT) {
                if (focusedChannel.logoURL) {
                    logo = '<img src="' + focusedChannel.logoURL + '"onerror="this.src=\'\';" />';
                    $channelItem.find('.chThumbnail').append($('<div>', {
                        'class' : 'chLogo'
                    }).html(logo));
                }
            }

            if (isLock) {
                $channelItem.find('.chEvtName').append($('<div>', {
                    'class' : 'channelLock'
                }));
            }
            if (isCas) {
                var casInfo = a.getCasInformation();
                $channelItem.find('.chEvtName').append($('<div>', {
                    'class' : casInfo
                }));
            }
            param.$anchor.append($channelItem);
        };
        (<any>channelList).drawPrismLiteSChannelList = function(param) {
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
            var programmeName : string = '', programmeTime : string = '';
            var thumbnail : string = '', bRecording : boolean = false;

            var $channelItem = $('<div>', {
                'class' : 'chItem'
            }).append($('<div>', {
                'class' : 'chEvtName'
            }).text(programmeName)).append($('<div>', {
                'class' : 'chEvtTime'
            }).text(programmeTime)).append($('<div>', {
                'class' : 'chThumbnail'
            }).html(thumbnail));

            if (a.checkRecordings(focusedChannel.ccid)) {
                $channelItem.find('.chThumbnail').append($recIcon);
            }
            if (isLock) {
                $channelItem.find('.chEvtName').append($('<div>', {
                    'class' : 'channelLock'
                }));
            }
            if (isCas) {
                var casInfo = a.getCasInformation();
                $channelItem.find('.chEvtName').append($('<div>', {
                    'class' : casInfo
                }));
            }
            param.$anchor.append($channelItem);
            return true;
        };

        a._channelList = channelList;
        a._TYPE = ['TV', 'RADIO'];
        a._IMAGE = ['images/212_Thumb_TV.png', 'images/212_Thumb_Radio.png'];
    }
    createIPlate() {
        var a = this;
        var b = this._parent;
        var iPlate = new __CONTROLS__.CListControl(null);
        iPlate.setId('iPlate');
        iPlate.setDrawParam({
            align : 'horizontal',
            itemWidth : 236 //264 <- old
        });
        iPlate.setDataDrawer(function(param) {
            //param.$item.text(param.item.name);
            var isCatchUp : boolean = false;
            var isLive : boolean = false;
            var isPast : boolean = false;
            var isNewRec : boolean = false;
            var isLock : boolean = false;
            var isRecNow : boolean = false;
            var isReservation : boolean = false;
            var isReservationSeries : boolean = false;
            var isReminder : boolean = false;
            var scrambled : boolean = false;
            var isNoCatchup : boolean = false;
            var thumbnailImg : string;
            var availableIcon : string = param.item.raw ? param.item.raw.availableIconUrl : param.item.availableIconUrl;

            if (param.item.catchup) {// is catchup
                if (param.item.noCatchup) {
                    thumbnailImg = 'images/350_LTV_Norec_BG.png';
                    isNoCatchup = true;
                } else if (hx.config.useBackwardEpg === true) {
                    thumbnailImg = param.item.getField('thumbnailURL');
                    if (!thumbnailImg) {
                        thumbnailImg = a.getDefaultThumbnail();
                    }
                    isPast = true;
                    //if (a.findProgrammeInRecordings(param.item.ccid, param.item.programmeID) === true) {
                    //    isCatchUp = true;
                    //} else if (param.item.mediaDeeplinkExpireDate && param.item.mediaDeeplinkExpireDate !== 0) {
                    //    var now = new Date();
                    //    var ct = Math.floor(now.valueOf() / 1000);
                    //
                    //    if (ct < param.item.mediaDeeplinkExpireDate) {
                    //        isCatchUp = true;
                    //    }
                    //}
                } else {
                    var rec = param.item;
                    var makeThumbnail = function() {
                        var defaultThumbnail;
                        if (rec.isBroken()) {
                            defaultThumbnail = 'images/212_Thumb_REC_Fail.png';
                        } else {
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
            } else {// programme
                thumbnailImg = param.item.getThumbnail();
                var lastWrapper = getLastWrapper(param.item);

                var now = new Date();
                var ct : number = Math.floor(now.valueOf() / 1000);
                var st : number = lastWrapper.startTime;
                var et : number = lastWrapper.startTime + lastWrapper.duration;
                if (ct >= st && ct <= et) {// now
                    isLive = true;
                    if (a.checkRecordings(lastWrapper.ccid) === true) {
                        isRecNow = true;
                    }
                } else if (et < ct) {
                    isPast = true;
                } else {// schedule
                    if (lastWrapper.getPluginObject()) {
                        if (lastWrapper.hasReminder()) {
                            isReminder = true;
                        } else if (lastWrapper.hasSchedule()) {
                            isReservation = true;
                        } else if (lastWrapper.hasSeriesSchedule()) {
                            isReservationSeries = true;
                        }
                    }
                }
            }



            var $div = $('<div>', {
                'class' : 'iPlateItem',
                'style' : 'background-image : url(' + thumbnailImg + ');' + 'background-size: cover;' + 'background-repeat:no-repeat'
            });

            if (!!availableIcon) {
                $div.append($('<img>', {
                    'class' : 'availability',
                    'src' : availableIcon
                }));
            }

            if (isCatchUp === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoCatchup'
                }));
            }
            if (isLive === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoLive'
                }));
            }
            if (isPast === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoPast'
                }));
            }
            if (isNewRec === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoNew'
                }));
            }
            if (isRecNow === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoRec'
                }));
            }

            if (isReservationSeries === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoRecRsvSer'
                }));
            } else if (isReservation === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoRecRsv'
                }));
            } else if (isReminder === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoReminder'
                }));
            }

            if (isLock === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoLock'
                }));
            }

            if (scrambled === true) {
                $div.append($('<div>', {
                    'class' : 'iPlateItemIcoScrambled'
                }));
            }

            if (isNoCatchup === true) {
                if (hx.config.useBackwardEpg === true) {
                    $div.append($('<div>', {
                        'class' : 'iPlateItemNoRec',
                        'text' : hx.l('LOC_NO_PROGRAMME_INFORMATION_ID')
                    }));
                } else {
                    $div.append($('<div>', {
                        'class' : 'iPlateItemNoRec',
                        'text' : hx.l('LOC_NO_RECORDINGS_IN_THIS_CHANNEL_ID')
                    }));
                }
            }

            //param.$item.append($div);
            param.$item[0].innerHTML = $div[0].outerHTML;

            lastWrapper = undefined;
        });
        iPlate.onItemSelected = function(param) {
            var that = b;
            var dsrvDetailInfo2 : any = hx.svc.DETAIL_INFO;
            var dsrvRecordings2 : any = hx.svc.RECORDINGS;
            if (param.item.catchup) {// is catchup
                if (param.item.noCatchup) {
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgNotAvailable?show'
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
                        programmeDetailItem : programmeDetailItem
                    });
                    that.sendEventToEntity({
                        'alEvent' : 'alMessageToPage',
                        'target' : 'CPgDetailInfo/infoHub',
                        'me' : that
                    });
                } else {
                    var recordingDetailItem = dsrvDetailInfo2.createRecordingItem(param.item);

                    hx.il.adhocPageParam.push({
                        programmeDetailItem : recordingDetailItem
                    });
                    that.sendEventToEntity({
                        'alEvent' : 'alMessageToPage',
                        'target' : 'CPgDetailInfo/infoHub',
                        'me' : that
                    });
                }
            } else {
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
                    programmeDetailItem : programmeDetailItem
                });
                that.sendEventToEntity({
                    'alEvent' : 'alMessageToPage',
                    'target' : 'CPgDetailInfo/infoHub',
                    'me' : that
                });
            }
        };
        var getLastWrapper = (item) => {
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
        }
        (<any>iPlate).doKeyRec = function() {
            var that = b;
            var lastWrapper = getLastWrapper(focusedProgramme);
            if (lastWrapper.isLive()) {
                b._startRec(focusedChannel);
            } else {
                if (lastWrapper.hasSchedule() || lastWrapper.hasSeriesSchedule()) {
                    that._cancelReservation(lastWrapper);

                } else if (lastWrapper.hasReminder()) {
                    that._updateScheduledRecording(lastWrapper);

                } else {
                    that._doReservation(lastWrapper);
                }
            }
            lastWrapper = undefined;
            return true;
        };

        (<any>iPlate).doKeyStop = function() {
            var that = b;
            var lastWrapper = getLastWrapper(focusedProgramme);
            if (lastWrapper.isLive()) {
                that._cancelRecording(focusedChannel);
            } else {
                if (lastWrapper.hasSchedule() || lastWrapper.hasReminder()) {
                    that._cancelReservation(lastWrapper);
                }
            }
            lastWrapper = undefined;
            return true;
        };

        iPlate.onFocusChanged = function(pOld, pNew) {
            var a = this;
            var that = b;
            var idx : number = parseInt(pNew.attr('data'), 10);
            var idxOld : number = parseInt(pOld.attr('data'), 10);

            var selProgramme = a._dataProvider.getItem(idx);
            if (selProgramme) {
                that._infoHub.setCurrentProgramme(selProgramme);
                updateCurrentProgramme(that._infoHub);
            }

            var dProvider = that._infoHub._iPlate._dataProvider;
            if (dProvider) {
                var tot : number = dProvider.getCount();
                if (tot - idx < 6) {
                    // get more EPG
                    var item = dProvider.getItem(tot - 1);
                    if (item && item.startTime) {
                        var sTime : any = item.startTime + item.duration + 1;
                        var eTime : any = sTime + 3600 * 12;
                        that._getNextEvtsAndAttach(sTime, eTime);
                    }
                }
            }

            if (hx.config.useBackwardEpg === true) {
                if (dProvider && a._catchUpsCount && a._catchUpsCount > 0 && idx < 1) {
                    var firstItem = dProvider.getItem(0);
                    var timeBase : any = firstItem.startTime * 1000;
                    that._getMoreBackwardEvtAndInsert(timeBase, timeBase - that.THRESHOLD);
                }
            }

            // prevent overwrap betwin selected & previous image : trick :)
            if (idxOld < idx) {
                (<HTMLElement>pOld[0].children[0]).className = 'iPlateItem previous';
            }
        };

        var viewIPlate = new __CONTROLS__.CViewGroupControl(null);
        viewIPlate.setId('viewIPlate');
        viewIPlate.setChildControl([iPlate]);
        viewIPlate.setDrawParam({
            anchorPositionLeft : 0
        });
        a._iPlate = iPlate;
        a._viewIPlate = viewIPlate;
    }
    doShow() {
        var a = this;
        if (state === 'infoHub') {
            a._$self.one(a.CONST.cssEvntTransitionEnd, function() {
                if (state === 'infoHub') {
                    a._$infoHubPane.$arrows.addClass('show');
                }
            });
        }
        setTimeout(function() {
            if (a._$self) {
                a._$self.addClass('show');
            }
        }, 1);
    }
    doHide() {
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
    }
    doDraw() {
        var a = this;
        var drawHandler = {
            'infoHub' : () => {
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
            'channelList' : () => {
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

                return super.doDraw.call(a);
            },
            'iPlate' : () => {
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
                    applyOnce : {
                        left : 236 * pos,
                        withoutAnim : true
                    }
                });

                a.setActiveFocus(true);
                //a._viewIPlate._$self.appendTo(a._$self);

                return super.doDraw.call(a);
            }
        };
        return drawHandler[state]();
    }
    displayOptbtn() {
        var a = this;
        var b = this._parent;
        if (a._channelListGroupOptBtn === undefined) {
            a._optIcon = $('<img>', {
                'class' : 'img',
                'src' : 'images/201_Option.png'
            });
            a._optText = $('<div>', {
                'class' : 'tit',
                'html' : hx.l('LOC_OPT_GROUP_ID') + ' - '
            });
            a._optData = $('<em>', {
                'html' : b._getLastGroupStr()
            });
            a._optText.append(a._optData);
            a._channelListGroupOptBtn = $('<div>', {
                'id' : 'channelListOptBtn'
            }).append(a._optIcon).append(a._optText);
            //$('#channelList').after(a._channelListGroupOptBtn);
            $('#-test-infoHub').append(a._channelListGroupOptBtn);
        } else {
            a._channelListGroupOptBtn.show();
        }
    }
    hideOptbtn() {
        var a = this;
        if (a._channelListGroupOptBtn) {
            a._channelListGroupOptBtn.hide();
        }
    }
    showChannelList(param?) {
        var i : number, a = this, bPrismLiteSMode : boolean = false;

        a.setPrismAndPrismLiteMode();
        state = 'channelList';
        if (param) {
            focusedChannel = a._currentChannel = param;
        }
        for ( i = 0; i < a._channelListData.length; i += 1) {
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
        } else {
            a.setChannelListPrismsetDrawParam();
        }
        a.draw();
    }
    showIPlate() {
        var a = this;

        a.setPrismAndPrismLiteMode();
        state = 'iPlate';
        a._iPlate.clearDrawnItems();
        a._iPlate.clearPreviousDrawItems();
        a._iPlate.setDrawParam('drawEffect', 'spreadOut', true);
        a.setChildControl([a._viewIPlate]);
        a.draw();
    }
    showEntry() {
        var a = this;

        if (hx.svc.STATISTICS) {
            hx.svc.STATISTICS.statisticsLog({
                "CODE" : "OSDIN",
                "TRS" : "I",
                "OSD" : "infoHub"
            });
        }
        hx.svc.CLOCK.startClock();
        a.setPrismAndPrismLiteMode();
        state = 'infoHub';
        a.setChildControl([a._iPlate]);
        a.draw();
    }
    keyBack() {
        var a = this;
        var b = this._parent;
        var that = b;
        if (state === 'infoHub') {
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "OSDOUT",
                    "TRS" : "I",
                    "OSD" : "infoHub"
                });
            }
            if (b._bActivate) {
                b._hide();
            }
        } else if (state === 'iPlate') {
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "OSDOUT",
                    "TRS" : "I",
                    "OSD" : "iPlate"
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
            } else {
                that._infoHub.setCurrentProgramme(undefined);
            }
            a.showEntry();
        } else if (state === 'channelList') {
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "OSDOUT",
                    "TRS" : "I",
                    "OSD" : "channelList"
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
            } else {
                that._infoHub.setCurrentProgramme(undefined);
            }
            a.showEntry();
        } else {
            a.showEntry();
        }
    }
    getCurrentState() {
        return state;
    }
    setCurrentChannel(channel) {
        var a = this, c : any;
        var i : number;
        if (state === 'channelList') {
            for ( i = 0; i < a._channelListData.length; i += 1) {
                c = a._channelListData[i];
                if (c.ccid === channel.ccid) {
                    a._channelList.setDrawParam('startIndex', i);
                    break;
                }
            }
        }
        focusedChannel = a._currentChannel = channel;
    }
    setCurrentGroup(group) {
        var a = this;
        a._currentGroup = group;
    }
    setCurrentProgramme(programme) {
        var a = this;
        hx.log('infohub', '[setCurrentProgramme]');
        focusedProgramme = a._currentProgramme = programme;
    }
    setiPlateProgrammes(programmes, channel?: Channel) {
        hx.log('infohub', '[setiPlateProgrammes] :');
        var a = this;
        var bPrismLiteSMode : boolean = false;

        if (programmes === undefined) {
            a.setIplateList(undefined, undefined);
            a._updatedprogrammes = null;
            a._receivedProgrammes = false;
            return;
        }
        if (programmes.length > 20) {// limit to prevent overhead
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
    }
    insertPastProgrammes(programmes) {
        hx.log('infohub', '[insertPastProgrammes] : ');
        var a = this, cnt : number;
        var newProgrammes;

        //if (programmes.length > 20) {// limit to prevent overhead
        //    newProgrammes = programmes.slice(programmes.length - 20);
        //} else {
            newProgrammes = programmes;
        //}

        var lenNewProg : number = newProgrammes.length;
        for ( cnt = 0; cnt < lenNewProg; cnt += 1) {
            newProgrammes[cnt] = $.extend(newProgrammes[cnt], {
                'catchup' : true
            });
        }

        if (a._iPlate._dataProvider) {
            var firstItem = a._iPlate._dataProvider.getItem(0);
            var p = newProgrammes.pop();

            if (p.startTime + p.duration <= firstItem.startTime) {
                newProgrammes.push(p);
            }

            a._viewIPlate.setDrawParam({
                applyOnce : {
                    left : 236 * lenNewProg,
                    withoutAnim : true,
                    after: () => {
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
    }
    appendProgrammes(programmes) {
        hx.log('infohub', '[appendProgrammes] : ');
        var a = this;
        var b = a._parent;
        if (programmes.length > 20) {// limit to prevent overhead
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
    }
    setiPlateCatchUps(catchUps) {
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
    }
    setiPlateBackwards(backwards) {
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
    }
    setIplateList(catchUps, programmes, channel?: Channel) {
        var a = this;
        var b = this._parent;
        var that = b;
        var cnt : number;
        var pos : number = 0;
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
            for ( cnt = 0; cnt < lenC; cnt += 1) {
                catchUps[cnt] = $.extend(catchUps[cnt], {
                    'catchup' : true
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
                    applyOnce : {
                        left : 236 * pos,
                        withoutAnim : true
                    }
                });
                a.appendProgrammes(programmes);
            } else {
                pos = 0;
                a._viewIPlate.setDrawParam({
                    applyOnce : {
                        left : 236 * pos,
                        withoutAnim : true
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
    }
    updateCurrentProgramme() {
        var a = this;
        updateCurrentProgramme(a);
    }
    removeProgramme(index) {
        var a = this;
        a._iPlate._dataProvider.removeItem(index);
    }
    removeIplateFocusedItem() {
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
    }
    removeIplateItem(id) {
        var a = this;
        var i : number;
        var o : any;
        var index : number = -1;
        var totCnt : number = a._iPlate._dataProvider._count;

        for ( i = 0; i < totCnt; i += 1) {
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
        } else {
            var item = a._iPlate._dataProvider.getItem(index);
            a.setCurrentProgramme(item);

            // if last item move 1 item
            if (index === a._iPlate._dataProvider._count) {
                a._iPlate.setFocusedItemByKey(index - 1);
                a._viewIPlate.setLeft(236 * (index - 1), false);
            }
        }
        a.updateCurrentProgramme();
    }
    setFocusedChannelEvt(programme) {
        var a = this;
        var bPrismLiteSMode = a.isPrismLiteSModeOn();
        if (bPrismLiteSMode === true) {
            a.setChannelListPrismLiteSsetFocusEvent(programme);
        } else {
            a.setChannelListPrismsetFocusEvent(programme);
        }
    }
    updateCurrentRecStatus(ccid, bStart) {
        var a = this;
        var b = this._parent;
        var bPrismLiteSMode : boolean = a.isPrismLiteSModeOn();
        if (!!focusedChannel) {
            if (state === 'infoHub') {
                var noWork;
            } else if (state === 'channelList') {
                if (ccid === focusedChannel.ccid) {
                    if (bPrismLiteSMode === true) {
                        if (bStart) {
                            b._getChListEvt(a._currentChannel);
                        } else {
                            $('.chIconRecLite').detach();
                        }
                    } else {
                        if (bStart) {
                            $('.chThumbnail').append($recIcon);
                        } else {
                            $('.chIconRec').detach();
                        }
                    }
                }
                checkChannelsChangeable(a._channelList.getCurrentViewingItemInfos());
            } else {// iPlae
                if (ccid === focusedChannel.ccid) {
                    if (bStart) {
                        var $live = $('.-fd.-afd-leaf');
                        if (focusedProgramme.isLive()) {
                            if ($live.length > 0) {
                                $live.append($('<div>', {
                                    'class' : 'iPlateItemIcoRec'
                                }));
                            }
                        } else {
                            if ($live.length > 0) {
                                $live.append($('<div>', {
                                    'class' : 'iPlateItemIcoRecRsv'
                                }));
                            }
                        }
                    } else {
                        var $recIco;
                        var $recSerIco;
                        var $reminderIco;
                        if (focusedProgramme.isLive()) {
                            $recIco = $('.-fd.-afd-leaf .iPlateItemIcoRec');
                            if ($recIco.length > 0) {
                                $recIco.remove();
                            }

                        } else {
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
    }
    updateRecordings(recordings) {
        var a = this, update : boolean = false;
        if (state === 'infoHub') {
            if (recordings) {
                if (a._recordings) {
                    if (a._recordings.length !== recordings.length) {
                        update = true;
                    }
                }
                a._recordings = recordings;
            } else {
                a._recordings = [];
            }
            if (update) {
                updateStatusIcon(a);
                var bPrismLiteSMode : boolean = a.isPrismLiteSModeOn();
                if ((bPrismLiteSMode === true) && (state === 'infoHub')) {
                    updateCurrentProgramme(a);
                }
            }
        } else {
            a._recordings = recordings;
        }
    }
    checkRecordings(ccid) {
        var a = this;
        if (a._recordings) {
            var len = a._recordings.length, i;
            for ( i = 0; i < len; i += 1) {
                if (ccid === a._recordings[i].getChannelId()) {
                    return true;
                }
            }
        }
        return false;
    }
    findProgrammeInRecordings(ccid, programmeID) {
        var a = this;
        if (a._recordings) {
            var len = a._recordings.length, i;
            for ( i = 0; i < len; i += 1) {
                if (ccid === a._recordings[i].getChannelId()) {
                    if (programmeID === a._recordings[i].getProgrammeID()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    setChannelListData(channelList) {
        var a = this;
        a._channelListData = channelList;
        a._channelList.setData(channelList);
    }
    getDefaultThumbnail() {
        var a = this;
        var channel : any = focusedChannel, defaultThumbnail : string = a._IMAGE[0];

        if (channel) {
            defaultThumbnail = a._IMAGE[channel.channelType];
        }

        hx.log("warning", "#### " + defaultThumbnail + "####");

        return defaultThumbnail;
    }
    getCasInformation() {
        var that = this, i : number, casArr : any = [], length : number, casIDs : string;
        var b = this._parent;

        if (focusedChannel.ccid) {
            if (!!focusedChannel.casIDs) {
                length = focusedChannel.casIDs.length;
                for ( i = 0; i < length; i += 1) {
                    casArr.push(focusedChannel.casIDs[i]);
                }
            }
        }
        if (casArr.length > 0) {
            casIDs = 'cas ';
            if (casArr.length > 1) {
                casIDs += 'multi';
            } else {
                for ( i = 0, length = casArr.length; i < length; i += 1) {
                    if (casArr[i] === b._strCasID.indexOf('CAS_NAGRA')) {
                        casIDs += 'na ';
                    } else if (casArr[i] === b._strCasID.indexOf('CAS_IRDETO')) {
                        casIDs += 'ir ';
                    } else if (casArr[i] === b._strCasID.indexOf('CAS_NDS')) {
                        casIDs += 'nd ';
                    } else if (casArr[i] === b._strCasID.indexOf('CAS_VIACCESS')) {
                        casIDs += 'va ';
                    } else if (casArr[i] === b._strCasID.indexOf('CAS_CONAX')) {
                        casIDs += 'cx ';
                    } else if (casArr[i] === b._strCasID.indexOf('CAS_MEDIAGUARD')) {
                        casIDs += 'mg ';
                    } else if (casArr[i] === b._strCasID.indexOf('CAS_CRYPTOWORKS')) {
                        casIDs += 'cr ';
                    }
                }
            }
        }
        return casIDs;
    }

    // Used for PRISM Lite Simple
    setPrismAndPrismLiteMode() {
        var a = this;
        var bIpEpgOn : boolean = false, bPrismLiteSmode : boolean = false;

        if ((hx.config.usePrismLiteS === undefined) || (hx.config.usePrismLiteS === false)) {
            bPrismLiteSmode = false;
        } else if (hx.config.usePrismLiteS === true) {
            if (ENV.listModel.IR4000HD === ENV.model || ENV.listModel.HDFOXIRCONNECT === ENV.model) {
                bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                if (bIpEpgOn === true) {
                    bPrismLiteSmode = false;
                } else {
                    bPrismLiteSmode = true;
                }
            } else if (ENV.listModel.HMS1000S === ENV.model) {
                var countryId;

                countryId = hx.svc.SETTING_UTIL.getCountryId();

                bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();

                switch(countryId) {
                    //germany
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                    //austria
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                    //switzerland
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                    //italy
                    case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                        bPrismLiteSmode = false;
                        break;
                    default :
                        bPrismLiteSmode = false;
                }
            }
        } else {
            bPrismLiteSmode = false;
        }

        a.bPrismLiteSmode = bPrismLiteSmode;
    }

    isPrismLiteSModeOn(ulIdx?) {
        var a = this;
        return a.bPrismLiteSmode;
    }
    setChannelListPrismLiteSsetFocusEvent(programme) {
        var a = this;
        focusedProgramme = a._currentProgramme = programme;
        var programmeName : string = '', programmeTime : string = '';
        var programmeDate : string = '', thumbnail : string = '', logo : string = '', casIcon : string = '';
        var sT : number, eT : number;
        var lockHtml : string = '', casHtml : string = '', progHtml : string = '', eventInfo : string, progTimeHtml : string = '';

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
        } else {
            progHtml = hx.l('LOC_NO_PROGRAMME_INFORMATION_ID');
        }
        if (programmeTime) {
            progTimeHtml = programmeTime;
        }

        eventInfo = lockHtml + casHtml + progHtml;
        a._channelList._$anchor.find('.chEvtName').html(eventInfo);
        a._channelList._$anchor.find('.chEvtTime').html(progTimeHtml);
        a._channelList._$anchor.find('.chThumbnail').html(thumbnail);
    }
    setChannelListPrismsetFocusEvent(programme) {
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
        var programmeName : string = '', thumbnail : string = '', logo : string = '', casIcon : string = '';

        if (programme && !focusedChannel.manualBlock) {
            programmeName = programme.getName();
            thumbnail = '<img src="' + programme.getThumbnail() + '" ' + 'onerror="this.src=\'' + a.getDefaultThumbnail() + '\';" />';
        } else {
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
        var lockHtml : string = '', casHtml : string = '', progHtml : string = '', eventInfo : string;
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

        if(a._channelList && a._channelList._$anchor){
            if (programme) {
                if (!programme.programmeIdMds) {
                    a._channelList._$anchor.find('.chEvtName').html(eventInfo);
                }
            }
            a._channelList._$anchor.find('.chThumbnail').html(thumbnail);
        }
    }
    setChannelListPrismLiteSsetDrawParam() {
        var that = this;

        that._channelList._drawParam.viewCount = 10;
        that._channelList._drawParam.anchorHeight = 115;
    }
    setChannelListPrismsetDrawParam() {
        var that = this;

        that._channelList._drawParam.viewCount = 7;
        that._channelList._drawParam.anchorHeight = 270;
    }
    keyLeftRight(keyCode) {
        var a = this;
        var ulIdx : number = 0;
        var dProvider : any = a._iPlate._dataProvider, ulMoveIdx : number = 0, selProgramme : any;

        if (a._ulEventCount < 1) {
            hx.log('infohub', 'keyLeftRight() Error!! Does not has event data!!');
            return;
        }

        ulMoveIdx = a._ulDispEventIdx;
        switch (keyCode) {
            case hx.key.KEY_LEFT:
                if (ulMoveIdx > 0) {
                    ulMoveIdx -= 1;
                } else {
                    hx.log('infohub', 'keyLeftRight()!! Do not move the prevs event : ' + ulMoveIdx);
                }
                break;
            case hx.key.KEY_RIGHT:
                if (ulMoveIdx < (a._ulEventCount - 1)) {
                    ulMoveIdx += 1;
                } else {
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
    }
    setPrismLiteSCurrentEventIdx(ulIdx) {
        var a = this;

        // Current Disp Event Index of iplate provider list
        a._ulDispEventIdx = ulIdx;
    }
    setPrismLiteSInitDataIdx(ulIdx) {
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
    }
    setPrismLiteSUpdateDataIdx(ulEvent, ulCatchUps) {
        var a = this;
        a._ulEventCount = ulEvent;
        a._ulCatchUpsCount = ulCatchUps;
    }
    getPrismLiteSNextEvtsAndAttach(startTime, endTime) {
        var a = this, ulNextEventCount : number = 0, channel : any;

        channel = a._currentChannel;
        //debugger;
        hx.svc.EPG.getProgrammes({
            isOnAir : false,
            channel : channel,
            startTime : startTime,
            endTime : endTime,
            listener : a,
            cb : function(result) {
                if (result[0].length > 0) {
                    ulNextEventCount = a._ulEventCount + result[0].length;
                    a.setPrismLiteSUpdateDataIdx(ulNextEventCount, a._ulCatchUpsCount);
                    a.appendProgrammes(result[0]);
                }
                result = undefined;
            }
        });
    }
    showDetailInfo() {
        var a = this;
        var b = this._parent;
        var that = b;
        var dsrvDetailInfo2 = hx.svc.DETAIL_INFO;
        var ulIdx : number = 0, selProgramme = null;
        var dProvider : any = a._iPlate._dataProvider;
        var result : boolean = false;

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
                programmeDetailItem : programmeDetailItem
            });
            that.sendEventToEntity({
                'alEvent' : 'alMessageToPage',
                'target' : 'CPgDetailInfo/infoHub',
                'me' : that
            });
        }
        return result;
    }

    actionReservaion(action) {
        var a = this;
        var dsrvRecordings2 : any = hx.svc.RECORDINGS;
        var p : any = a._currentProgramme;
        var efactoryType : number = 0, bIsGetPvrHDD : boolean = false, bIsLiveEvt : boolean = false;
        var scheduledList : any = dsrvRecordings2.getScheduledList();

        if (a._ulEventCount < 1) {
            hx.log('infohub', 'actionReservaion() Error!! Does not has event data!!');
            return false;
        }

        function maxReservation() {
            // Check Max reservation count
            var ret : boolean = false;
            if (scheduledList.length >= 300) {
                var now = new Date();
                var ct : number = Math.floor(now.valueOf() / 1000);
                if (p) {
                    // check live
                    if (p.catchup === undefined) {// is schedule ??
                        if (ct >= p.getStartTime() && ct <= p.getEndTime()) {// live
                            bIsLiveEvt = true;
                        } else {
                            bIsLiveEvt = false;
                        }
                    }
                }
                if (bIsLiveEvt === false) {
                    hx.log('infohub', 'actionReservaion() Error!! Does not action reservaion because max schedule count !!');
                    var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                    hx.msg('response', {
                        text : msg,
                        auto_close : true,
                        close_time : 3000
                    });
                    ret = true;
                } else {
                    hx.log('infohub', 'actionReservaion() Error!! Does not action reservaion because max schedule count!! But live Event');
                    ret = false;
                }
            }
            return ret;
        }

        function cancelReservation(data) {
            hx.log('infohub', 'actionReservaion() - cancelReservation()');
            var i : number, ti : number, scheduledItem : any, programmeID : any, foundItem : any;

            for ( i = 0, ti = scheduledList.length; i < ti; i += 1) {
                scheduledItem = scheduledList[i];
                programmeID = scheduledItem.getProgrammeID();
                if (programmeID && (programmeID === data.programmeID)) {
                    foundItem = scheduledItem;
                    break;
                }
            }

            if (foundItem) {
                dsrvRecordings2.removeScheduledRecording({
                    items : [scheduledItem],
                    finished : function() {
                        hx.log('infohub', 'actionReservaion() finished removeScheduledRecording');
                    }
                });
            }
        }

        function isRecordingEvent(data) {
            var bIsRecordingEvent : boolean = false;
            var item = null, recCCid : number = 0, i : number , ti : number;
            var cRecList = dsrvRecordings2.getCurrentRecordings();
            var ulStartTimeValue : number = 0 , ulEndTimeValue : number = 0;
            var ulstartTime : number = 0, ulendTime : number = 0;

            if (cRecList.length === 0) {
                hx.log('infohub', 'isRecordingEvent() no recording channel');
                bIsRecordingEvent = false;
            }

            for ( i = 0, ti = cRecList.length; i < ti; i += 1) {
                item = cRecList[i];
                if (item && (item.getChannelId() === data.ccid)) {
                    hx.log('infohub', 'isRecordingEvent() find same recording channel : ' + item.getChannelId());
                    ulStartTimeValue = item.getStartTime();
                    ulEndTimeValue = item.getEndTime();
                    ulstartTime = Math.floor(ulStartTimeValue/1000);
                    ulendTime = Math.floor(ulEndTimeValue/1000);
                    if ((ulstartTime <= data.startTime) && (ulendTime >= data.startTime)) {
                        bIsRecordingEvent = true;
                    } else {
                        bIsRecordingEvent = false;
                    }
                    break;
                }
            }

            return	bIsRecordingEvent;
        }

        function onRequestInstanceRecording(data) {
            var param : any = {};

            if (data) {
                if (hx.config.useDelayedRec) {
                    param.tsrPosOffset = hx.svc.TSR.getPlaybackOffset();
                    param.tsrBufferSize = hx.svc.TSR.getMaxOffset();
                } else {
                    param.tsrPosOffset = 0;
                    param.tsrBufferSize = 0;
                }
                param.finished = function(result) {
                    hx.log('infohub', hx.util.format('onRequestInstanceRecording - result = {0}', result));
                };
                if (data.ccid) {
                    hx.log('infohub', hx.util.format('onRequestInstanceRecording - channel.ccid = {0}', data.ccid));
                    param.ccid = data.ccid;
                }
                var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                if (bIsGetPvrHDD === true) {
                    hx.svc.RECORDINGS.requestInstanceRecording(param);
                } else {
                    this.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgNotAvailable?show'
                    });
                }
            }
        }

        if (p && (p.isLive() !== true)) {
            var ret : boolean = false;
            if (!p.hasSchedule() && !p.hasReminder()) {
                var fullReserved : boolean = maxReservation();
                if (fullReserved === true) {
                    return true;
                }

                bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                if (bIsGetPvrHDD === false) {
                    efactoryType = 1;
                }
                dsrvRecordings2.record({
                    programme : p.pluginObject,
                    factoryType : efactoryType, // 0 : Recording,  1 : Reminder
                    finished : function() {
                        hx.log('infohub', 'actionReservaion() finished record');
                    }
                });
                ret = true;
            } else if (p.hasSchedule() || p.hasReminder()) {
                var bIsRecording = isRecordingEvent(p);
                if (bIsRecording === false) {
                    cancelReservation(p);
                    ret = true;
                } else {
                    onRequestInstanceRecording(p);
                    ret = true;
                }
            } else {
                ret = false;
            }
        } else {
            if (p && (p.isLive() === true)) {
                onRequestInstanceRecording(p);
                ret = true;
            } else {
                ret = false;
            }
        }
        return ret;
    }
}

class cCPgInfoHub extends __PAGE_KERNEL__ {

    _responseObj : any = null;
    _currentChannel : any = null;
    _currentEvent : any = null;
    _remChannel : any = null;
    _recordings : any = [];
    _groupContextMenu : any = null;
    _strCurrentGroup : string = '';
    _iPlateIconUpdate : boolean = false;
    _favlistname : any = [];
    _strChannelGroupName : any = [];
    _currentGroupDB : any = null;
    _strCasID : any = hx.config.CPgInfoHub.chGroup.strCasID;
    _groupDB : any = hx.config.CPgInfoHub.chGroup.groupDB;
    _favGroupDB : any = hx.config.CPgInfoHub.chGroup.favGroupDB;
    // Alphabet Group
    _alphabetgroupContextMenu : any = null;
    _strAlphabetGroupName : any = [];
    _alphabetGroupDB : any = hx.config.CPgInfoHub.chGroup.alphabetGroupsDB;
    _entryPoint : any = null;
    _infoHub : any;
    _$infoHub : any;
    _lKeyRecGetList : any;
    $container : any;
    _bActivate : boolean;
    _timerId : number;
    CLASSES : any;
    _textAnimate: any;
    TextAniValueOffset : number = 11;
    TextAniIntervalOffset : number = 15;
    THRESHOLD : number = 7 * 24 * 60 * 60 * 1000; // 7DAYS(ms)

    constructor(aPageName: string) {
        super(aPageName)
        var a = this;
        var b = this;
//        var CInfoHubControl = new CMainGroupControl()
//        a.CLASSES = {
//            'CInfoHubControl' : CInfoHubControl
//        };
    }

    create () {
        var that = this;
        that._bActivate = false;
        that._timerId = -1;

        hx.logadd('infohub');
        that._init_Data();
        // Main control: info hub control
        $.extend(this, {
            $container : $("#wrap")
        });
//        var CInfoHubControl = that.CLASSES.CInfoHubControl;
        var $infoHub = $('<div>', {
            'id' : '-test-infoHub',
            'class' : '-afd'
        });
        var infoHub = new CMainGroupControl($infoHub, this);

        $('#ltv').after($infoHub);
        that._$infoHub = $infoHub;
        that._infoHub = infoHub;
        hx.al.compose(that);

        var fnUpdate = function() {
            var cRecList = hx.svc.RECORDINGS.getCurrentRecordings();
            if (!!cRecList) {
                that._recordings = cRecList;
                that._infoHub.updateRecordings(cRecList);
            }
            cRecList = undefined;
        };

        hx.svc.RECORDINGS.addEventCb('ScheduledItemAdded', that, (param) => {
            var ccid = param.item.getChannelId();
            if (that._infoHub.getCurrentState() === 'iPlate' && !!that._infoHub._iPlate) {
                that._infoHub.updateCurrentRecStatus(ccid, true);
                fnUpdate();
                that._iPlateIconUpdate = true;
            }
        });

        hx.svc.RECORDINGS.addEventCb('ScheduledItemRemoved', that, (param) => {
            var ccid = param.item.getChannelId();
            if (that._infoHub.getCurrentState() === 'iPlate' && !!that._infoHub._iPlate) {
                that._infoHub.updateCurrentRecStatus(ccid, false);
                fnUpdate();
                that._iPlateIconUpdate = true;
            }
        });

        hx.svc.RECORDINGS.addEventCb('ScheduledItemUpdated', that, function(param) {
            var ccid = param.item.getChannelId();
            if (that._infoHub.getCurrentState() === 'iPlate' && !!that._infoHub._iPlate) {
                that._infoHub.updateCurrentRecStatus(ccid, true);
                fnUpdate();
                that._iPlateIconUpdate = true;
            }
        });

        hx.svc.RECORDINGS.addEventCb('RecordingItemAdded', that, (param) => {
            var ccid = param.item.getChannelId();
            that._infoHub.updateCurrentRecStatus(ccid, true);
            fnUpdate();
            that._iPlateIconUpdate = true;
        });

        hx.svc.RECORDINGS.addEventCb('RecordingCompleted', that, (param) => {
            var ccid = param.item.getChannelId();
            that._infoHub.updateCurrentRecStatus(ccid, false);
            fnUpdate();
            that._iPlateIconUpdate = true;
        });

        hx.svc.RECORDINGS.addEventCb('RecordingItemRemoved', that, (param) => {
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
        hx.svc.AV_CTRL.addEventCb('evtSetChannel', that, (aParam) => {
            that._on_evtSetChannel(aParam);
        });
        hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, () => {
            this._on_ChannelListUpdate();
        });
        hx.svc.CLOCK.addEventCb('MinuteChanged', this, (aNow) => {
            updateDateTime(infoHub);
        });

        if (ENV.op === ENV.listOP.UKDTT) {
            hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, (aButtonInfo) => {
                if (this.getPageVisible(this.name) === true) {
                    var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                    dlg.open({
                        onClosed: () => {
                            this.setKeyHookingControl(dlg);
                        }
                    });
                    this.setKeyHookingControl(dlg, true);
                }
            });
        }
    }
    destroy () {
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
    }
    isActive () {
        return this._bActivate;
    }
    visible () {
        return this._bActivate;
    }
    getSelectedText () {
        var that = this;
        var currentItem : any, currentIndex : number;
        var infoHub = that._infoHub;
        var channelList = infoHub._channelList;
        var iPlate = infoHub._iPlate;
        var focusedItem = iPlate.getFocusedItem();

        currentItem = (focusedItem && infoHub.getCurrentState() === 'iPlate') ? focusedItem.item : channelList.getCurrentItem();
        return currentItem.name;
    }
    _on_alMessageToPage (aParam) {
        var that = this;
        var retObj, retObj2, retObj3;
        var state : string = that._infoHub.getCurrentState();
        var dsrvChList = hx.svc.CH_LIST;
        hx.log('infohub', '[_on_alMessageToPage]');
        if (aParam.query === 'dcn') {
            if (state === 'channelList') {
                if (aParam.param !== 'clear') {
                    var ch = dsrvChList.getCurrentChannel();
                    if (ch.group === "TV" || ch.group === "RADIO") {
                        var data: any = {};
                        data.ccid = aParam.param;
                        data = dsrvChList.getAvailableGroupForCcid(data);
                        this._infoHub._optData[0].innerHTML = this._getLastGroupStr(data.group);
                        retObj3 = dsrvChList.getChannel({
                            ccid : aParam.param,
                            group : data.group
                        });
                    } else {
                        retObj3 = dsrvChList.getChannel({
                            ccid : aParam.param,
                            group : this._currentGroupDB
                        });
                    }
                    that._infoHub.setCurrentProgramme(undefined);
                    that._getChlist(retObj3);
                    that._infoHub.showChannelList(retObj3.channel);
                }
                that._resetTimer();
            } else if (state === 'iPlate') {
                if (aParam.param !== 'clear') {
                    var ch = dsrvChList.getCurrentChannel();
                    if (ch.group === "TV" || ch.group === "RADIO") {
                        var data: any = {};
                        data.ccid = aParam.param;
                        data = dsrvChList.getAvailableGroupForCcid(data);
                        retObj3 = dsrvChList.getChannel({
                            ccid : aParam.param,
                            group : data.group
                        });
                    } else {
                        retObj3 = dsrvChList.getChannel({
                            ccid : aParam.param,
                            group : this._currentGroupDB
                        });
                    }
                    that._infoHub.setCurrentProgramme(retObj3);
                    that._infoHub.setCurrentChannel(retObj3.channel);
                    that._infoHub.setCurrentGroup(retObj3.group);
                    var pra = {
                        group: retObj3.group,
                        channel: retObj3.channel
                    }
                    that._draw_ChInfo(pra, 'iPlate', false);
                    that._infoHub.showIPlate();
                }
            }
            return;
        }
        retObj = dsrvChList.getCurrentChannel();
        if (true === retObj.result) {
            // for fast load
            if (aParam.query === 'backToParent') {// from programme detail
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
            } else if (aParam.query === 'backToLive') {// from programme detail
                that._$infoHub[0].style.display = 'block';
                that._bActivate = true;
                that._hide();
            } else {
                if (that._$infoHub[0].style.display === 'none') {
                    that._$infoHub[0].style.display = 'block';
                    that._bActivate = true;
                    that._hide();
                }
                that._getChlist(retObj);
                that._draw_ChInfo(retObj, aParam.query, false);
                that._entryPoint = aParam.query;
                // remember entry point
            }
        }
    }
    _draw_ChInfo (p, aMode : string, aIsUpdate : boolean) {
        var that = this;
        var newChannel : boolean = true;

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

        var startTime : number = Math.floor(new Date().getTime() / 1000);
        if (hx.svc.TSR.isTsrOngoing() === true) {
            var VBChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
            if (!!VBChannel && VBChannel.ccid === that._currentChannel.ccid) {
                var TSRstartTime = hx.svc.TSR.getStartTime();
                var TSRplaybackOffset = hx.svc.TSR.getPlaybackOffset();
                hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
                startTime = TSRstartTime + TSRplaybackOffset;
                //startTime =  startTime - (60*60); // test only
            }
        }
        var endTime : number = startTime + 3600 * 12;
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
    }
    _doSlideAnimation (target, text) {
        var that = this;

        that._stopSlideAnimation(target);

        var textAniValue = text.length * that.TextAniValueOffset;
        var textAniInterval = textAniValue * that.TextAniIntervalOffset;
        hx.il.slideTextAnimate(target, textAniValue, textAniInterval);
        that._textAnimate = setInterval(function() {
            hx.il.slideTextAnimate(target, textAniValue, textAniInterval);
        }, textAniInterval);
    }

    _stopSlideAnimation (target) {
        var that = this;

        if (that._textAnimate) {
            clearInterval(that._textAnimate);
            that._textAnimate = null;
        }
        if (target) {
            target.stop();
            target[0].scrollLeft = 0;
        }
    }
    _show (aMode) : void {
        // reset timer
        this._resetTimer();
        this._iPlateIconUpdate = false;

        // hide pin
        this.sendEventToEntity({
            alEvent : 'alMessageToPage',
            target : 'CPgLiveController?hidepin=pin'
        });
        this._infoHub.setCurrentGroup(this._currentGroupDB);
        this._infoHub.setCurrentChannel(this._currentChannel);

        if (aMode === 'channelList') {
            this._infoHub.showChannelList();
        } else if (aMode === 'iPlate') {
            this._infoHub.showIPlate();
        } else {
            this._infoHub.showEntry();
        }
        this._infoHub.show();

        // show & animation .doShow
        // set flag
        this._bActivate = true;
        this.sendEventToEntity({
            alEvent : 'alCheckPageVisible'
        });
    }
    _resetTimer () {
        clearTimeout(this._timerId);
        this._timerId = setTimeout(()=> {
            this._hide();
        }, 30000);
    }
    _hide () {
        var that = this;
        var state : string = that._infoHub.getCurrentState();

        if (state === "channelList") {

            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "OSDOUT",
                    "TRS" : "I",
                    "OSD" : "channelList"
                });
            }

        } else if (state === "infoHub") {
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "OSDOUT",
                    "TRS" : "I",
                    "OSD" : "infoHub"
                });
            }
            hx.svc.CLOCK.stopClock();
        } else if (state === "iPlate") {
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "OSDOUT",
                    "TRS" : "I",
                    "OSD" : "iPlate"
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
                'alEvent' : 'alMessageToPage',
                'target' : 'CPgLiveController?hideInfoHub'
            });
            // show pin
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgLiveController?showpin=pin'
            });
        }
        // un-set flag
        that._bActivate = false;
        that.sendEventToEntity({
            alEvent : 'alCheckPageVisible'
        });
    }
    _on_alKey (aParam) : boolean {
        var that = this;
        var keyCode : number = aParam.alKey;
        var bConsumed : boolean = false, bActionPvr : boolean = false;
        var pra : any, bPrismLiteSMode : boolean = false;
        var ulTvSvcCount : number = 0, ulRadioSvcCount : number = 0;
        var state : string;

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
                    alEvent : 'alMessageToPage',
                    target : 'CPgDcn?active=' + (keyCode - hx.key.KEY_0).toString()
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
                            channel : that._currentChannel
                        });
                        if (bChangeable === true) {
                            hx.svc.AV_CTRL.channelChange({
                                channel : that._currentChannel,
                                group : that._currentGroupDB
                            });
                        } else {
                            if (hx.config.useTVStreaming === false) {
                                hx.msg('response', {
                                    text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            } else {
                                hx.msg('response', {
                                    text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                    auto_close : true,
                                    close_time : 3000
                                });
                            }
                        }
                    } else {
                        if (that._infoHub.showDetailInfo() === false) {
                            var noWork;
                            //that._hide();
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
                            "CODE" : "OSDOUT",
                            "TRS" : "I",
                            "OSD" : state
                        });
                    }
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE" : "OSDIN",
                            "TRS" : "I",
                            "OSD" : "channelList"
                        });
                    }
                    pra = {
                        group : that._currentGroupDB,
                        channel : that._currentChannel
                    };
                    that._draw_ChInfo(pra, 'channelList', false);
                    that._infoHub.showChannelList();
                    //that._getChListEvt(that._currentChannel);
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
                                "CODE" : "OSDOUT",
                                "TRS" : "I",
                                "OSD" : state
                            });
                        }
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE" : "OSDIN",
                                "TRS" : "I",
                                "OSD" : "iPlate"
                            });
                        }
                        pra = {
                            group : that._currentGroupDB,
                            channel : that._currentChannel
                        };
                        that._draw_ChInfo(pra, 'iPlate', false);
                        that._infoHub.showIPlate();
                        //that._getChListEvt(that._currentChannel);
                    }
                } else {
                    that._infoHub.keyLeftRight(keyCode);
                }
                bConsumed = true;
                break;
            case hx.key.KEY_OPTION:
                if (state === 'infoHub') {
                    // channel group select
                    hx.ui.overlay.left($('#-test-infoHub'));
                    that._groupContextMenu = that._createOptListMenu();
                } else if (state === 'channelList') {
                    // channel group select
                    hx.ui.overlay.left($('#-test-infoHub'));
                    that._groupContextMenu = that._createOptListMenu();
                } else if (state === 'iPlate') {
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgNotAvailable?show'
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
                } else {
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
                        setTimeout(()=> {
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
                        group : 'TV'
                    }).chCnt;
                    ulRadioSvcCount = hx.svc.CH_LIST.getChannelCount({
                        group : 'RADIO'
                    }).chCnt;
                    if (that._currentGroupDB === 'RADIO') {
                        if (ulTvSvcCount > 0) {
                            that._changeChannelGroup('TV');
                        }
                    } else {
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
                                    first_line : hx.l('LOC_CHANNEL_INFORMATION_ID'),
                                    thirdLineFont : true,
                                    nolinejump : true,
                                    second_line :  that._currentChannel.majorChannel + ', ' + that._currentChannel.name + '<br>' + _tuningInfos,
                                    auto_close : false,
                                    btn_title_arr : [hx.l('LOC_OK_ID')]
                                });
                                setTimeout(()=> {
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
    }
    _stopCurrentChannelRecord (aData) : boolean {
        var item : any, recCCid : number = 0, findItem : any, i : number, ti : number;
        var dsrvRecordings : any = hx.svc.RECORDINGS;
        var currentRecordingList : any = dsrvRecordings.getCurrentRecordings();
        var ret : boolean = false;

        if (currentRecordingList.length === 0) {
            hx.log('infohub', '_stopCurrentChannelRecord() no recording channel');
            return false;
        }

        for ( i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
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
            if ( item instanceof __MODEL__.IRecordingItem) {
                dsrvRecordings.stopRecord({
                    item : item
                });
            } else if ( item instanceof __MODEL__.IScheduledItem) {
                dsrvRecordings.removeScheduledRecording({
                    items : [item]
                });
            }
            ret = true;
        } else {
            ret = false;
        }
        return ret;
    }
    _getEvt (aChannel, aRenew, aStartTime, aEndTime, aMode) {
        //chlist
        hx.svc.EPG.getProgrammes({
            isOnAir : false,
            channel : aChannel,
            startTime : aStartTime,
            endTime : aEndTime,
            listener : this,
            notUI : true,
            cb : (result)=> {
                if (result.length) {
                    this._currentEvent = result[0][0];
                    if (this._currentEvent) {
                        this._infoHub.setCurrentProgramme(this._currentEvent);
                        if (aMode === 'iPlate') {
                            this._infoHub.updateCurrentProgramme();
                            this._infoHub.setiPlateProgrammes(result[0], aChannel);
                            this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForiPlate.bind(this), Date.now(), 3);
                        } else if (aMode === 'channelList') {
                            this._infoHub.setFocusedChannelEvt(this._currentEvent);
                            this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForChannelList.bind(this), this._currentEvent.getStartTime() * 1000);
                        } else {
                            this._infoHub.updateCurrentProgramme();
                            this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForChannelList.bind(this), this._currentEvent.getStartTime() * 1000);
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
    }
    _getNextEvtsAndAttach (aStartTime, aEndTime) {
        var bPrismLiteSMode : boolean = false, channel : any;

        channel = this._currentChannel;

        //debugger;
        hx.svc.EPG.getProgrammes({
            isOnAir : false,
            channel : channel,
            startTime : aStartTime,
            endTime : aEndTime,
            listener : this,
            cb : (result)=> {
                if (result[0].length > 0) {
                    bPrismLiteSMode = this._infoHub.isPrismLiteSModeOn();
                    if (bPrismLiteSMode === true) {
                        this._infoHub.setPrismLiteSInitDataIdx(this._infoHub._ulDispEventIdx);
                    }
                    this._infoHub.appendProgrammes(result[0]);
                }
                result = undefined;
            }
        });
    }
    _getChListEvt (aChannel) {
        this._currentChannel = aChannel;

        //chlist
        var result = hx.svc.EPG.getProgrammes({
            isOnAir : true,
            filter: "P/F",
            channel : aChannel,
            listener : this,
            cb : (result)=> {
                if (result) {
                    var len = result[0].length;
                    if (len > 0) {
                        this._infoHub.setFocusedChannelEvt(result[0][0]);
                        this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForChannelList.bind(this), result[0][0].getStartTime() * 1000);
                    } else {
                        this._infoHub.setFocusedChannelEvt(undefined);
                    }
                } else {
                    this._infoHub.setFocusedChannelEvt(undefined);
                }
                result = undefined;
            }
        });
        //this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForChannelList.bind(this), Date.now());
    }
    public callbackIpEpgResultForChannelList = function(channel: Channel, res) {
        if (this._currentChannel !== channel) {
            return;
        }
        if (this._infoHub._currentProgramme && this._infoHub._currentProgramme.isWrapper) {
            this._infoHub.setFocusedChannelEvt(this._infoHub._currentProgramme);
            return;
        }
        var currentProgramme = this._infoHub._currentProgramme;

        if(!currentProgramme){
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
                (<any>pp).raw = data;
                var callback = () => {
                    this._infoHub.setFocusedChannelEvt(pp);
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
    }
    public callbackIpEpgResultForiPlate = function(channel: Channel, res, prevIdx: number = 0) {
        var a = this;
        var iPlate = a._infoHub._iPlate;
        var dp = iPlate._dataProvider;
        var dpLen = dp.getCount();
        var info = hx.svc.IP_EPG.parse.xml.schedule(res);
        var schedules = Array.prototype.slice.call(info.schedules);
        var i, currentProgramme : __PROVIDER__.CProgrammeProvider;
        var oipfProgramme = null;
        var eventData, parsedId, legacyId, eventId;
        for (i = prevIdx; i < dpLen; i+=1) {
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
                        (<any>currentProgramme).raw = eventData;
                        oipfProgramme = hx.svc.IP_EPG.convert.to.programme(eventData, channel, currentProgramme);
                        ((aProg, aData) => {
                            var prog = aProg;
                            var data = aData;
                            var ondemand = oipfProgramme.ondemand;
                            if (ondemand) {
                                hx.svc.IP_EPG.getAIT(this._getTemplateUrl(ondemand)).then((xml) => {
                                    a._checkTemplateAIT(xml, prog).then((param : {'url': string;}) => {
                                        data.availableIconUrl = param.url;
                                        prog.onavailable(param.url);
                                    }, (err) => {});
                                }, (err) => {});
                            }
                        })(currentProgramme, eventData);
                        eventData = null;
                        oipfProgramme.onthumbnail = (function (aIndex) {
                            return function() {
                                var prog = dp.getItem(aIndex);
                                prog.thumbnailURL = this.getField('thumbnailURL');
                                a._updateThumbnailForiPlate(this, aIndex);
                                prog = undefined;
                            }
                        })(i);
                        (<any>currentProgramme).onavailable = (function (aIndex) {
                            return function(aUrl) {
                                a._updateOnDemandAvailableIconForiPlate(aIndex, aUrl);
                            }
                        })(i);
                        schedules.splice(j, 1);
                        break;
                    }
                }
            }
        }
    }
    private _updateOnDemandAvailableIconForiPlate(aIndex, aUrl) {
        var a = this;
        var iPlate = a._infoHub._iPlate;
        var focusedItem = iPlate.getFocusedItem();
        if (focusedItem) {
            var $item = iPlate._drawnItems.getItem(aIndex);
            if ($item) {
                var div = $item.children('.iPlateItem')[0];
                if (!!aUrl) {
                    hx.log('debug', '[_updateOnDemandAvailableIconForiPlate]['+aIndex+'] url(' + aUrl + ') updated');
                    div.append($('<img>', {
                        'class' : 'availability2',
                        'src' : aUrl
                    })[0]);
                }
            }
        }
    }
    private _updateThumbnailForiPlate(progContext, aIndex) {
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
                    var url = (<any>progContext).getField('thumbnailURL');
                    if (url) {
                        hx.log('debug', '[_updateThumbnailForiPlate]['+aIndex+'] url(' + url + ') updated');
                        div.setAttribute('style', style.replace(regUrl, 'url(' + url + ')'));
                    }
                }
            }
        }
        progContext = null;
    }
    private _getTemplateUrl = function (ondemand) {
        var params = hx.svc.SETTING_UTIL.getNetworkId().map(function(nid) {
            return 'nids[]=' + nid;
        });
        params.push('lloc=epg');

        return [
            ondemand.auxiliaryURL,
            params.join('&')
        ].join(ondemand.auxiliaryURL.indexOf('?') !== -1 ? '&' : '?');
    }
    private _checkTemplateAIT = function (xml, aProg) {
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

            if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') { // B2C Spec (v1.0) 7.1 Device Criteria available version 1.3.1

                var ondemand = prog.raw ? prog.raw.ondemand : prog._ondemand;
                if (ondemand) {
                    ondemand = hx.svc.IP_EPG.parse.element.ondemand(ondemand);
                    if (ondemand.startOfAvailability * 1000 < timeCurrent && ondemand.endOfAvailability * 1000 >= timeCurrent) { // availability window
                        if (ondemand.mediaAvailabilityCS) {
                            availabilityIconUrl = "images/205_REC_Play.png";
                            if (prog.raw) {
                                prog.raw.available = true;
                            } else {
                                prog.available = true;
                            }
                        } else {
                            availabilityIconUrl = "images/614_EPG_Comingsoon.png";
                        }
                    } else if (timeCurrent > (ondemand.startOfAvailability - 7200) * 1000 && timeCurrent <= ondemand.startOfAvailability * 1000) {
                        availabilityIconUrl = "images/614_EPG_Comingsoon.png";
                    }
                }
            }
        }
        retDef.resolve({
            url : availabilityIconUrl
        });
        prog = null;
        return retDef.promise;
    }
    updateWithIpEpg(channel: Channel, callback, time: number = Date.now(), chunk = 1, startIdx: number = 0) {
        var HOUR6 = 60 * 60 * 6 * 1000;
        var i;
        if (channel) {
            for (i = 0; i < chunk; i+=1) {
                hx.svc.IP_EPG.getSchedules(channel.sid, time + (HOUR6 * i), 6).then(function(res) {
                    callback(channel, res, startIdx);
                });
            }
        }
    }
    _getChlist (aCurrentChannel) {
        var retObj : any;
        retObj = hx.svc.CH_LIST.getChlist({
            group : aCurrentChannel.group
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
        } else {
            list = retObj.chList;
        }
        this._infoHub.setChannelListData(list);
        list = undefined;
        retObj = undefined;
    }
    _getPastEvt (channel, renew, startTime, endTime) {
        hx.svc.EPG.getProgrammes({
            isOnAir : false,
            channel : channel,
            startTime : startTime,
            endTime : endTime,
            listener : this,
            isBackward : true,
            cb : (result)=> {
                if (result.length && result[0].length > 0) {
                    var now = new Date();
                    var ct = Math.floor(now.valueOf() / 1000);
                    var p = result[0].pop();
                    if (p.getEndTime() < ct) {
                        result[0].push(p);
                    }
                    this._infoHub.setiPlateCatchUps(result[0]);
                } else {
                    this._infoHub.setiPlateCatchUps(null);
                }
                result = undefined;
            }
        });
        // clear cached event
        if (renew === true) {
            this._infoHub.setiPlateCatchUps(null);
        }
    }
    _getBackwardEvt(channel, renew, timeBase, threshold) {
        var a = this;
        var queryList: IQuerySchedule[] = [];
        var xmls = [];
        var serviceId = channel.sid;
        var HOUR6 = 6;
        var now = new Date().getTime();
        var callback = (aXmls) => {
            var programmes = [];
            var mdsProgramme = null;
            var schedules = null, crids = null;
            var i = 0, len = 0, schedule = null, crid = null, data = null;
            if (aXmls !== null) {
                aXmls.forEach((info) => {
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
                            ((aProg) => {
                                setTimeout(() => {
                                    if (a._infoHub) {
                                        var prog = aProg;
                                        var ondemand = prog.ondemand;
                                        if (ondemand) {
                                            hx.svc.IP_EPG.getAIT(a._getTemplateUrl(ondemand)).then((xml) => {
                                                a._checkTemplateAIT(xml, prog).then((param : {'url': string;}) => {
                                                    prog.availableIconUrl = param.url;
                                                    prog.onavailable(param.url);
                                                }, (err) => {});
                                            }, (err) => {});
                                        }
                                    }
                                }, 10);
                            })(mdsProgramme);
                        }
                        mdsProgramme = null;
                    }
                });
                if (programmes.length > 1) {
                    programmes.sort(function(prev, after) {
                        return prev.startTime - after.startTime >=0 ? 1 : -1;
                    });
                    for (var i = 0 ; i < programmes.length ; i++) {
                        mdsProgramme = programmes[i];
                        mdsProgramme.onthumbnail = (function (aIndex) {
                            return function() {
                                a._updateThumbnailForiPlate(this, aIndex);
                            }
                        })(i);
                        mdsProgramme.onavailable = (function (aIndex) {
                            return function(aUrl) {
                                a._updateOnDemandAvailableIconForiPlate(aIndex, aUrl);
                            }
                        })(i);
                    }
                    a._infoHub.setiPlateBackwards(programmes);
                } else if (programmes.length === 1) {
                    if ((programmes[0].startTime + programmes[0].duration) * 1000 >= now) { // ON AIR
                        if (timeBase - (HOUR6 * 60 * 60 * 1000) >= threshold) {
                            a._getMoreBackwardEvtAndInsert(timeBase - (HOUR6 * 60 * 60 * 1000), threshold);
                        } else {
                            a._infoHub.setiPlateBackwards(programmes);
                        }
                    } else {
                        a._infoHub.setiPlateBackwards(programmes);
                    }
                } else {
                    if (timeBase - (HOUR6 * 60 * 60 * 1000) >= threshold) {
                        a._getBackwardEvt(channel, renew, timeBase - (HOUR6 * 60 * 60 * 1000), threshold);
                    } else {
                        a._infoHub.setiPlateBackwards([]);
                    }
                }
                mdsProgramme = null;
                programmes = [];
            } else {
                a._infoHub.setiPlateBackwards([]);
            }
        }

        function addXml(xml?){
            if (xml) {
                xmls.push(hx.svc.IP_EPG.parse.xml.schedule(xml));
            }
        }
        function addScheduleQueryToQueryList(serviceId : any, timeBase : number, hours : number){
            var retDef = Q.defer();
            retDef.promise.then(addXml);
            var query : IQuerySchedule ={
                serviceId : serviceId,
                startTime : timeBase,
                hours : hours,
                retDef : retDef
            };
            queryList.push(query);
        }

        addScheduleQueryToQueryList(serviceId,timeBase, HOUR6);
        addScheduleQueryToQueryList(serviceId,timeBase - (HOUR6 * 60 * 60 * 1000), HOUR6);

        hx.svc.IP_EPG.getScheduleList(queryList).then(function(){
                callback(xmls);
            },
            function(err){
                callback(null);
            });
        if (renew === true) {
            this._infoHub.setiPlateBackwards(null);
        }
        queryList = [];
        xmls = [];
    }
    _getMorePastEvtAndInsert (aStartTime, aEndTime) {
        var that = this, channel : any;
        channel = that._currentChannel;
        hx.svc.EPG.getProgrammes({
            isOnAir : false,
            channel : channel,
            startTime : aStartTime,
            endTime : aEndTime,
            listener : that,
            isBackward : true,
            cb : function(result) {
                if (result.length && result[0].length > 0) {
                    that._infoHub.insertPastProgrammes(result[0]);
                }
                result = undefined;
            }
        });
    }
    _getMoreBackwardEvtAndInsert (aTimeBase, aThreshold) {
        var a = this, channel : any;
        channel = a._currentChannel;
        var queryList: IQuerySchedule[] = [];
        var xmls = [];
        var serviceId = channel.sid;
        var HOUR6 = 6;
        var firstProgramme = a._infoHub._iPlate._dataProvider.getItem(0);
        var callback = (aXmls) => {
            var programmes = [];
            var mdsProgramme = null;
            var schedules = null, crids = null;
            var i = 0, len = 0, schedule = null, crid = null, data = null;
            if (aXmls !== null) {
                aXmls.forEach((info) => {
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
                            ((aProg) => {
                                setTimeout(() => {
                                    if (a._infoHub) {
                                        var prog = aProg;
                                        var ondemand = prog.ondemand;
                                        //var dp = this._infoHub._iPlate._dataProvider;
                                        if (ondemand) {
                                            hx.svc.IP_EPG.getAIT(this._getTemplateUrl(ondemand)).then((xml) => {
                                                this._checkTemplateAIT(xml, prog).then((param : {'url': string;}) => {
                                                    prog.availableIconUrl = param.url;
                                                    prog.onavailable(param.url);
                                                    //var pp = dp.getItem(param.index);
                                                    //if (pp) {
                                                    //    pp.availableIconUrl = param.url;
                                                    //    dp.updateItem(param.index, pp);
                                                    //}
                                                }, (err) => {});
                                            }, (err) => {});
                                        }
                                    }
                                }, 10);
                            })(mdsProgramme);
                        }
                        mdsProgramme = null;
                    }
                });
                if (programmes.length > 0 ) {
                    programmes.sort(function(prev, after) {
                        return prev.startTime - after.startTime >=0 ? 1 : -1;
                    });
                    for (var i = 0 ; i < programmes.length ; i++) {
                        mdsProgramme = programmes[i];
                        mdsProgramme.onthumbnail = (function (aIndex) {
                            return function() {
                                a._updateThumbnailForiPlate(this, aIndex);
                            }
                        })(i);
                        mdsProgramme.onavailable = (function (aIndex) {
                            return function(aUrl) {
                                a._updateOnDemandAvailableIconForiPlate(aIndex, aUrl);
                            }
                        })(i);
                    }
                    this._infoHub.insertPastProgrammes(programmes);
                } else {
                    if (aTimeBase - (HOUR6 * 60 * 60 * 1000) >= aThreshold) {
                        this._getMoreBackwardEvtAndInsert(aTimeBase - (HOUR6 * 60 * 60 * 1000), aThreshold);
                    } else {
                        this._infoHub.insertPastProgrammes(programmes);
                    }
                }
            } else {
                this._infoHub.insertPastProgrammes(programmes);
            }
            programmes = [];
            mdsProgramme = null;
        }

        function addXml(xml?){
            if (xml) {
                xmls.push(hx.svc.IP_EPG.parse.xml.schedule(xml));
            }
        }
        function addScheduleQueryToQueryList(serviceId : any, timeBase : number, hours : number){
            var retDef = Q.defer();
            retDef.promise.then(addXml);
            var query : IQuerySchedule ={
                serviceId : serviceId,
                startTime : timeBase,
                hours : hours,
                retDef : retDef
            };
            queryList.push(query);
        }

        addScheduleQueryToQueryList(serviceId,aTimeBase, HOUR6);
        addScheduleQueryToQueryList(serviceId,aTimeBase - (HOUR6 * 60 * 60 * 1000), HOUR6);

        hx.svc.IP_EPG.getScheduleList(queryList).then(function(){
            callback(xmls);
        }, function(err){
            callback(null);
        });
        queryList = [];
        xmls = [];
    }
    _getCatchUp (aChannel, aRenew) {
        var that = this;
        var i : number;
        var recs : any = [];
        var list : any = hx.svc.RECORDINGS.getRecordingList();
        for ( i = 0; i < list.length; i += 1) {
            if (aChannel.ccid === list[i].getChannelId()) {
                recs.push(list[i]);
            }
        }
        if (recs.length > 0) {
            recs.sort(that._sortByNewerTime);
            that._infoHub.setiPlateCatchUps(recs);
        } else {
            that._infoHub.setiPlateCatchUps(null);
        }
        list = [];
        recs = [];
    }
    _sortByNewerTime (a, b) : number {
        var aa = a.getStartTime();
        var bb = b.getStartTime();
        if (aa < bb) {
            return -1;
        }
        if (bb < aa) {
            return 1;
        }
        return 0;
    }
    _startRec (aFocusedChannel) {
        var that = this;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();

        if (bIsGetPvrHDD === false) {
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgNotAvailable?show'
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
                            item : recording
                        });
                        return;
                    }
                }
            }
            hx.svc.BRIDGE.requestInstanceRecording(focusedChannel);
        }
    }

    _doReservation (data) {
        var that = this;
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var scheduledList = dsrvRecordings2.getScheduledList();

        // Check Max reservation count
        if (scheduledList.length >= 300) {
            var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
            hx.msg('response', {
                text : msg,
                auto_close : true,
                close_time : 3000
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

            } else {
                pluginObj = tmp;
            }
        }

        dsrvRecordings2.record({
            programme : pluginObj,
            factoryType : 0,
            finished : function() {
            }
        });
    }

    _cancelReservation (data) {
        var that = this;
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var scheduledList = dsrvRecordings2.getScheduledList();
        var i, ti, scheduledItem, programmeID, foundItem;
        for ( i = 0, ti = scheduledList.length; i < ti; i += 1) {
            scheduledItem = scheduledList[i];
            programmeID = scheduledItem.getProgrammeID();
            if (programmeID && (programmeID === data.programmeID)) {
                foundItem = scheduledItem;
                break;
            }
        }

        if (foundItem) {
            dsrvRecordings2.removeScheduledRecording({
                items : [scheduledItem],
                finished : function() {
                }
            });
        }
    }

    _updateScheduledRecording (data) {
        var that = this;
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var scheduledList = dsrvRecordings2.getScheduledList();
        var i, ti, scheduledItem, programmeID, foundItem;
        for ( i = 0, ti = scheduledList.length; i < ti; i += 1) {
            scheduledItem = scheduledList[i];
            programmeID = scheduledItem.getProgrammeID();
            if (programmeID && (programmeID === data.programmeID)) {
                foundItem = scheduledItem;
                break;
            }
        }

        var rescheduleInfo: any = {};
        rescheduleInfo.ccid = foundItem.getChannelId();
        rescheduleInfo.startTime = foundItem.getStartTime();
        rescheduleInfo.duration = foundItem.getDuration();
        rescheduleInfo.repeatDays = foundItem.getRepeat();
        rescheduleInfo.reminder = foundItem.isReminder();

        dsrvRecordings2.updateScheduledRecording({
            item : foundItem
            ,
            startTime : Math.floor(rescheduleInfo.startTime.getTime() / 1000)
            ,
            duration : rescheduleInfo.duration
            ,
            ccid : rescheduleInfo.ccid
            ,
            repeatDays : rescheduleInfo.repeatDays
            ,
            factoryType : TScheduledFactoryType.ERECORDING
            ,
            finished : function(result) {
                // hx.log('infohub', '_______updateScheduledRecording : ' + result);
            }
        });
    }

    _cancelRecording (focusedChannel) {
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var currentRecordingList = dsrvRecordings2.getCurrentRecordings();

        if (currentRecordingList && currentRecordingList.length) {
            var ccid = focusedChannel.ccid;
            var i, ti, recordingItem;
            for ( i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                recordingItem = currentRecordingList[i];
                if (recordingItem.getChannelId() === ccid) {
                    dsrvRecordings2.stopRecord({
                        'item' : recordingItem
                    });
                    break;
                }
            }
        }
    }
    _responseMsgHide () {
        if (this._responseObj) {
            hx.msg.close('response', this._responseObj);
            this._responseObj = null;
        }
    }
    _responseMsgShow (aText) {
        this._responseMsgHide();

        this._responseObj = hx.msg('response', {
            text : aText,
            auto_close : true,
            timeout : 3000,
            dia_class : 'res vaTop',
            callback_fn : function(p) {
                var noWork;
            }
        });
    }
    _changeChannelGroup (aGroup) {
        var retObj : any = hx.svc.CH_LIST.getChlist({
            group : aGroup
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
            } else {
                list = retObj.chList;
            }
            this._infoHub.setChannelListData(list);
            var ch : any = (retObj.chList.length === 0 ) ? null : this._currentChannel;
            var pra : any = {
                group : aGroup,
                channel : ch
            };
            if (this._infoHub.getCurrentState() === 'infoHub') {
                pra.channel = retObj.chList[0];
                // set to first channel of target group
                this._draw_ChInfo(pra, 'infoHub', true);
            } else {
                this._draw_ChInfo(pra, 'channelList', true);
            }
        }
        retObj = null;
    }
    _createOptListMenu () {
        var that = this, itemList : any = [], groupList : any = [], data : any, i : number, length : number, startIndex : number, groupName : string;
        var bIsFavroureGroup : boolean = false, ulCurGroupDB : any, count : number;

        for ( i = 0, length = that._groupDB.length; i < length; i += 1) {
            count = hx.svc.CH_LIST.getChannelCount({
                group : that._groupDB[i]
            }).chCnt;

            if ((hx.config.useAlphabetGroup === 1) && (that._groupDB[i] === 'A-Z')) {
                count = hx.svc.CH_LIST.getChannelCount({
                    group : 'AGALL'
                }).chCnt;
            }

            if (count > 0) {
                bIsFavroureGroup = that._optListMenu_checkFavourite(that._groupDB[i]);
                if (bIsFavroureGroup === true) {// favorite
                    groupName = hx.svc.CH_LIST.getFavListName({
                        favtype : that._groupDB[i]
                    });
                    if (groupName) {
                        that._strChannelGroupName[i] = groupName;
                    }
                } else {
                    groupName = that._strChannelGroupName[i];
                }
                data = {
                    text : groupName,
                    msg : '',
                    value : i
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
        var relationTarget : string = '#channelListOptBtn';
        var contextMenu = hx.ui.ContextMenu({
            compose : this,
            renderTo : $(that.$container), //$(that._infoHub._$self),
            relation : $(relationTarget),
            direction : 'up',
            startIndex : startIndex,
            items : itemList,
            onOk : function(index, argObject) {
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
            onCancel : function() {
                this.destroy();
                hx.ui.overlay.hide();
                that._groupContextMenu = null;
            },
            onHideOption : function() {
                this.destroy();
                hx.ui.overlay.hide();
                that._groupContextMenu = null;
            },
            onKeyUpdated : function() {
                that._resetTimer();
            }
        });
        contextMenu.show({});
        return contextMenu;
    }
    _destroyGroupListMenu () {
        if (this._groupContextMenu) {
            hx.ui.overlay.hide();
            this._groupContextMenu.destroy();
            this._groupContextMenu = undefined;
        }
    }
    _getLastGroupStr (aGroup?) : string {
        var that = this, i : number, lastGroup : any;
        if (aGroup) {
            lastGroup = aGroup;
        } else {
            var curObj = hx.svc.CH_LIST.getCurrentChannel();
            lastGroup = curObj.group;
        }
        for ( i = 0; i < that._groupDB.length; i += 1) {
            if (lastGroup === that._groupDB[i]) {
                return that._strChannelGroupName[i];
            }
        }

        if (hx.config.useAlphabetGroup === 1) {
            for ( i = 0; i < that._alphabetGroupDB.length; i += 1) {
                if (lastGroup === that._alphabetGroupDB[i]) {
                    var szAlphabetText : string = hx.l('LOC_A_Z_ID');
                    return szAlphabetText;
                }
            }
        }

        return that._strChannelGroupName[0];
        // exception
    }
    _setCurrentChannel (aChannel) {
        this._currentChannel = aChannel;
    }
    _on_dlDetailInfo (aParam) {
        throw "this is no longer valid";
    }
    _on_dlUpdateList (aParam) {
        var that = this, list : any = aParam.alParam.list;
        var channel : any = aParam.channel;
        var i : number;
        var recordings : any = null;
        var sortRecs : any = [];
        var idx : number = 0;

        if (that._lKeyRecGetList) {
            that._lKeyRecGetList = undefined;
        }

        if (aParam.action === 'getList') {
            for ( i = 0; i < list.length; i += 1) {
                if (channel.ccid === list[i].channel.ccid) {
                    recordings = list[i].recordings;
                    break;
                }
            }

            if (recordings && recordings.length > 0) {
                // reverse data array
                for ( i = 0; i < recordings.length; i += 1) {
                    idx = recordings.length - 1 - i;
                    sortRecs.push(recordings[idx]);
                }
            } else {
                // "No Recording in this channel."
                sortRecs = recordings;
            }
            that._infoHub.setiPlateCatchUps(sortRecs);
        }
    }
    _on_evtSetChannel (aParam) {
        var p = aParam;
        var state : string = this._infoHub.getCurrentState();
        if (this._bActivate === true && state === 'infoHub') {
            this._draw_ChInfo({
                'channel' : p.channel,
                'group' : this._currentGroupDB
            }, 'infoHub', true);
        }
    }
    _on_ChannelListUpdate () {
        if (this._bActivate) {
            this._hide();
        }
    }
    _init_Data () {
        var that = this, i : number = 0, bIsFavGroup : boolean = false, groupName : string;
        // Init. Group List Name
        for ( i = 0; i < hx.config.CPgInfoHub.chGroup.groupName.length; i += 1) {
            that._strChannelGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.groupName[i]);
        }

        for ( i = 0, length = that._groupDB.length; i < length; i += 1) {
            bIsFavGroup = that._optListMenu_checkFavourite(that._groupDB[i]);
            if (bIsFavGroup) {
                groupName = hx.svc.CH_LIST.getFavListName({
                    favtype : that._groupDB[i]
                });

                if (groupName) {
                    that._strChannelGroupName[i] = groupName;
                }
            }
        }

        // Init. Favourite List Name
        for ( i = 0; i < hx.config.CPgInfoHub.chGroup.favlistName.length; i += 1) {
            that._favlistname[i] = hx.l(hx.config.CPgInfoHub.chGroup.favlistName[i]);
        }

        // Init. Alphabet List Name
        if (hx.config.useAlphabetGroup === 1) {
            for ( i = 0; i < hx.config.CPgInfoHub.chGroup.alphabetGroupName.length; i += 1) {
                if (hx.config.CPgInfoHub.chGroup.alphabetGroupsDB[i] === 'AGALL') {
                    that._strAlphabetGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.alphabetGroupName[i]);
                } else {
                    that._strAlphabetGroupName[i] = hx.config.CPgInfoHub.chGroup.alphabetGroupName[i];
                }
            }
        }
    }
    _optListMenu_checkFavourite (aParam : string) {
        var ret : boolean = false;

        if (aParam === 'Favourite1') {
            ret = true;
        } else if (aParam === 'Favourite2') {
            ret = true;
        } else if (aParam === 'Favourite3') {
            ret = true;
        } else if (aParam === 'Favourite4') {
            ret = true;
        } else if (aParam === 'Favourite5') {
            ret = true;
        } else {
            ret = false;
        }
        return ret;
    }

    //------------------------------------------------------------------------------------------------------
    // Alphabet Group List Function
    //------------------------------------------------------------------------------------------------------
    _ag_createOptAlphabetListMenu () {
        var that = this, itemList : any = [], itemListView : any = [], groupList : any = [], data : any;
        var i : number, length : number, startIndex : number, groupName : string;
        var bIsFavroureGroup : boolean = false, ulCurIndex : number = 0, curObject : any, count : number;

        for ( i = 0, length = that._alphabetGroupDB.length; i < length; i += 1) {
            count = hx.svc.CH_LIST.getChannelCount({
                group : that._alphabetGroupDB[i]
            }).chCnt;

            if (count > 0) {
                groupName = that._strAlphabetGroupName[i];

                data = {
                    text : groupName,
                    msg : '',
                    value : i
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
            compose : this,
            renderTo : $(that.$container), //$(that._infoHub._$self),
            relation : $('#channelListOptBtn'),
            direction : 'up',
            startIndex : startIndex,
            items : itemList,
            mode : 'scroll',
            column : 11,
            onOk : function(index, argObject) {
                var dbIndex = that._strAlphabetGroupName.indexOf(argObject.text);
                var szAlphabetText = hx.l('LOC_A_Z_ID');

                that._ag_changeAlphabetChannelGroup(that._alphabetGroupDB[dbIndex]);
                $('#channelListOptBtn').find('em').text(szAlphabetText);
                this.onCancel();
            },
            onCancel : function() {
                this.destroy();
                hx.ui.overlay.hide();
                that._ag_refreshOptAlphabetBtn();
                that._alphabetgroupContextMenu = null;
            },
            onHideOption : function() {
                this.destroy();
                hx.ui.overlay.hide();
                that._ag_refreshOptAlphabetBtn();
                that._alphabetgroupContextMenu = null;
            },
            onKeyUpdated : function(param) {
                that._resetTimer();
            }
        });
        contextMenu.show({});
        return contextMenu;
    }
    _ag_destroyAlphabetGroupListMenu () {
        if (this._alphabetgroupContextMenu) {
            hx.ui.overlay.hide();
            this._ag_refreshOptAlphabetBtn();
            this._alphabetgroupContextMenu.destroy();
            this._alphabetgroupContextMenu = undefined;
        }
    }
    _ag_refreshOptAlphabetBtn () {// for dim
        //var $detach = $('#channelListOptBtn').detach();
        //$('#channelList').after($detach);
    }
    _ag_getSupportAlphabetChlist (aParam) {
        var bReturn : boolean = false;

        if (hx.config.useAlphabetGroup === 1) {
            if (this._groupDB[aParam] === 'A-Z') {
                bReturn = true;
            } else {
                bReturn = false;
            }
        } else {
            bReturn = false;
        }
        return bReturn;
    }
    _ag_checkAlphabetGroup (aParam) {
        var i : number = 0;

        for ( i = 0; i < this._alphabetGroupDB.length; i += 1) {
            if (this._alphabetGroupDB[i] === aParam) {
                return 'A-Z';
            }
        }
        return aParam;
    }
    _ag_changeAlphabetChannelGroup (aGroup) {
        var retObj : any;

        retObj = hx.svc.CH_LIST.getChlist({
            group : aGroup
        });

        if (retObj.chList.length > 0) {
            this._infoHub.setChannelListData(retObj.chList);
            var ch : any = (retObj.chList.length === 0 ) ? null : retObj.chList[0];
            var pra : any = {
                group : aGroup,
                channel : ch
            };
            this._draw_ChInfo(pra, 'channelList', true);
        }
        retObj = null;
    }
    //------------------------------------------------------------------------------------------------------
    // End of Alphabet Group List Function
    //------------------------------------------------------------------------------------------------------
}
hx.al.CPgInfoHub = cCPgInfoHub;
export = cCPgInfoHub;
