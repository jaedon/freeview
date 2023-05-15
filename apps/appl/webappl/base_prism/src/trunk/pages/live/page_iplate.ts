/**
 * Created by thkim2 on 2017-04-11.
 */
///<reference path="../../def/sugar.d.ts"/>
///<reference path="../../def/prism.d.ts"/>
///<reference path="../../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />
///<reference path="../../layer_domain/service/ip_epg/adapter.d.ts" />

import __PAGE_KERNEL__ = require("pages/page_kernel");
import __MODEL__ = require('model/model');
import __PROVIDER__ = require("pages/util/provider");

declare var hx: ifSugar;

class CPgIPlate extends __PAGE_KERNEL__ {
    /* timer */
    static TIMER = {
        EXPIRE: 10000,
        EPG_QUERY: 10000,
        PERFORMANCE: 400
    };
    static STATE = {
        BACKWARD: 0,
        FORWARD: 1,
    };
    static AVAILABLE = {
        NOT_AVAILABLE: 0,
        COMMINGSOON: 1,
        AVAILABLE: 2
    };
    LIVE_INDEX: number;
    DAY7: number;
    THUMBNAIL_WIDTH: number;
    DETAILINFO_THUMBNAIL_WIDTH: number;
    THRESHHOLD: number;


    active: boolean;

    now: any;

    state: number;
    eventIndex: number;
    showingChannel: any;
    liveChannel: any;
    forwardEvents: any;
    backwardEvents: any;
    liveStreamingTotal: number;
    recordingsTotal: number;
    hasBackward: boolean;
    isLocked: boolean;

    expireTimer;
    epgQueryTimer;
    performanceTimer;

    progressMsg;

    keyRepeat: boolean;

    /* related dom */
    $self: any;
    $dim: any;
    $banner: any;
    $background: any;
    $thumbnail: any;
    $lcn: any;
    $serviceName: any;
    $eventName: any;
    $secondary: any;
    $startTime: any;
    $endTime: any;
    $now: any;
    $group: any;
    $icons: any;
    $arrowLeft: any;
    $arrowRight: any;
    $arrowUp: any;
    $arrowDown: any;
    $progressBG: any;
    $progressBar: any;
    $footer: any;
    $liveStreamingIcon: any;
    $recordingsIcon: any;
    $liveStreamingText: any;
    $recordingsText: any;
    $siIcons: any;

    constructor(pageName: string) {
        super(pageName);
    }

    create() {
        var that = this;

        hx.logadd('iplate');

        that.init();
        that.draw();

        that.updateUICurrentTime();

        hx.al.compose(that);
    }

    destroy() {
        var that = this;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var dsrvClock = hx.svc.CLOCK;
        var dsrvChList = hx.svc.CH_LIST;

        that.killTimer();

        if (that.$self) {
            that.$self.remove();
            delete that.$self;
            that.$self = undefined;
        }

        if (that.forwardEvents) {
            delete that.forwardEvents;
            that.forwardEvents = undefined;
        }

        if (that.backwardEvents) {
            delete that.backwardEvents;
            that.backwardEvents = undefined;
        }

        dsrvClock.stopClock();

        dsrvRecordings.clearEventCb(that);
        dsrvClock.clearEventCb(that);
        dsrvChList.clearEventCb(that);
    }

    isActive() {
        var that = this;

        return that.active;
    }

    visible() {
        var that = this;

        return that.active;
    }

    init() {
        var that = this;

        that.active = false;
        that.state = CPgIPlate.STATE.FORWARD;
        that.eventIndex = 0;
        that.liveStreamingTotal = 0;
        that.recordingsTotal = 0;
        that.expireTimer = -1;
        that.epgQueryTimer = -1;
        that.performanceTimer = -1;
        that.forwardEvents = [];
        that.backwardEvents = [];
        that.LIVE_INDEX = 0;
        that.DAY7 = 7 * 24 * 60 * 60 * 1000; // 7DAYS(ms)
        that.THRESHHOLD = 2;
        that.THUMBNAIL_WIDTH = 226;
        that.DETAILINFO_THUMBNAIL_WIDTH = 400;
        that.hasBackward = false;
        that.keyRepeat = false;
        that.isLocked = false;
        hx.svc.CLOCK.startClock();
        that.initEventHandler();
    }

    initEventHandler() {
        var that = this;
        var dsrvChList = hx.svc.CH_LIST;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var dsrvClock = hx.svc.CLOCK;

        dsrvRecordings.addEventCb('RecordingItemAdded', that, (param) => {
            that.updateUIRecordingsInfo();
            that.onRecordingEventListener();
        });
        dsrvRecordings.addEventCb('RecordingCompleted', that, (param) => {
            that.updateUIRecordingsInfo();
            that.onRecordingEventListener();
        });
        dsrvRecordings.addEventCb('RecordingItemRemoved', that, (param) => {
            that.updateUIRecordingsInfo();
            that.onRecordingEventListener();
        });
        dsrvRecordings.addEventCb('NotiLiveStreamingStart', that, (param) => {
            that.updateUIRecordingsInfo();
        });
        dsrvRecordings.addEventCb('NotiLiveStreamingStop', that, (param) => {
            that.updateUIRecordingsInfo();
        });
        dsrvRecordings.addEventCb('ScheduledItemAdded', that, (param) => {
            if (param) {
                that.onScheduleEventListener(param.item);
            }
            that.updateUIRecordingsInfo();
        });
        dsrvRecordings.addEventCb('ScheduledItemRemoved', that, (param) => {
            if (param) {
                that.onScheduleEventListener(param.item);
            }
            that.updateUIRecordingsInfo();
        });
        dsrvRecordings.addEventCb('ScheduledItemUpdated', that, (param) => {
            if (param) {
                that.onScheduleEventListener(param.item);
            }
            that.updateUIRecordingsInfo();
        });
        dsrvClock.addEventCb('MinuteChanged', that, (param) => {
            that.updateUICurrentTime();
        });
        dsrvChList.addEventCb('ChannelListUpdate', that, (param) => {
            if (that.isActive() === true) {
                that.hide();
            }
        });
    }

    resetExpireTimer() {
        var that = this;

        clearTimeout(that.expireTimer);
        that.expireTimer = setTimeout(() => {
            that.hide();
        }, CPgIPlate.TIMER.EXPIRE);
    }

    resetEpgQueryTimer() {
        //var that = this;
        //var dsrvAvCtrl = hx.svc.AV_CTRL;
        //var dsrvTsr = hx.svc.TSR;
        //var showingChannel;
        //var startTime, endTime;
        //
        //clearInterval(that.epgQueryTimer);
        //that.epgQueryTimer = setInterval(() => {
        //    showingChannel = that.showingChannel;
        //
        //    startTime = Math.floor(that.now.getTime() / 1000);
        //    if (dsrvTsr.isTsrOngoing() === true) {
        //        var VBChannel = dsrvAvCtrl.getVideoBroadcastCurrentChannel();
        //        if (!!VBChannel && VBChannel.ccid === showingChannel.channel.ccid) {
        //            var TSRstartTime = dsrvTsr.getStartTime();
        //            var TSRplaybackOffset = dsrvTsr.getPlaybackOffset();
        //            hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
        //            startTime = TSRstartTime + TSRplaybackOffset;
        //        }
        //    }
        //    endTime = startTime + 3600 * 12;
        //
        //    that.searchEPG(showingChannel.channel, startTime, endTime).then((param: any) => {
        //        if (showingChannel.channel.ccid === that.showingChannel.channel.ccid) {
        //            if (that.LIVE_INDEX === that.eventIndex) {
        //                that.setForwardEvents(param.result[0]);
        //                if (that.forwardEvents) {
        //                    that.updateUIEventInfo(that.forwardEvents[that.LIVE_INDEX]);
        //                } else {
        //                    that.updateUIEventInfo(null);
        //                }
        //            }
        //        }
        //    }, () => {
        //        hx.log('error', '[page_iplate.js][epg query timer] searchEPG Fail!!');
        //    });
        //}, CPgIPlate.TIMER.EPG_QUERY);
    }

    resetPerformanceTimer(channel?) {
        var that = this;

        clearTimeout(that.performanceTimer);
        that.performanceTimer = setTimeout(() => {
            that.performanceTimer = -1;
            that.loadChannelData(channel);
        }, CPgIPlate.TIMER.PERFORMANCE);
    }

    killEpgQueryTimer() {
        var that = this;

        if (that.epgQueryTimer !== -1) {
            clearInterval(that.epgQueryTimer);
            that.epgQueryTimer = -1;
        }
    }

    killTimer() {
        var that = this;

        if (that.expireTimer !== -1) {
            clearTimeout(that.expireTimer);
            that.expireTimer = -1;
        }
        if (that.epgQueryTimer !== -1) {
            clearInterval(that.epgQueryTimer);
            that.epgQueryTimer = -1;
        }
        if (that.performanceTimer !== -1) {
            clearTimeout(that.performanceTimer);
            that.performanceTimer = -1;
        }
    }

    /* related UI */
    draw() {
        var that = this;
        var $wrap: any;
        var $progress: any;
        var $moreIcon: any;
        var $chListIcon: any;
        var $moreText: any;
        var $chListText: any;
        var $splitLine: any;

        $wrap = $('#wrap');

        that.$self = $('<div>', {
            'id': '-iplate'
        });
        that.$dim = $('<div>', {
            'id': '-iplate-dim'
        });
        that.$banner = $('<div>', {
            'id': '-iplate-banner'
        });
        that.$background = $('<div>', {
            'id': '-iplate-background'
        });
        that.$thumbnail = $('<img>', {
            'id': '-iplate-thumbnail'
        });
        that.$lcn = $('<div>', {
            'id': '-iplate-lcn'
        });
        $splitLine = $('<div>', {
            'id': '-iplate-splitLine'
        });
        that.$serviceName = $('<div>', {
            'id': '-iplate-svcname'
        });
        that.$eventName = $('<div>', {
            'id': '-iplate-evtname'
        });
        that.$secondary = $('<div>', {
            'id': '-iplate-secondary'
        });
        that.$startTime = $('<div>', {
            'id': '-iplate-starttime'
        });
        that.$endTime = $('<div>', {
            'id': '-iplate-endtime'
        });
        that.$now = $('<div>', {
            'id': '-iplate-now'
        });
        that.$group = $('<div>', {
            'id': '-iplate-group'
        });
        that.$icons = $('<div>', {
            'id': '-iplate-icons'
        });
        that.$arrowLeft = $('<div>', {
            'id': '-iplate-arrowleft'
        });
        that.$arrowRight = $('<div>', {
            'id': '-iplate-arrowright'
        });
        that.$arrowUp = $('<div>', {
            'id': '-iplate-arrowup'
        });
        that.$arrowDown = $('<div>', {
            'id': '-iplate-arrowdown'
        });
        that.$footer = $('<div>', {
            'id': '-iplate-footer'
        });
        that.$progressBG = $('<div>', {
            'id': '-iplate-progressBG'
        });
        that.$progressBar = $('<div>', {
            'id': '-iplate-progressBar'
        });
        $progress = $('<div>', {
            'id': '-iplate-progress'
        }).append(that.$progressBG).append(that.$progressBar);

        that.$siIcons = $('<div>', {
            'id': '-iplate-siIcons'
        });
        that.drawSiIcons();

        $moreIcon = $('<img>', {
            'id': '-iplate-more'
        });
        $chListIcon = $('<img>', {
            'id': '-iplate-chlist'
        });
        $moreText = $('<div>').text('MORE INFO');
        $chListText = $('<div>').text('CHANNEL LIST');

        that.$liveStreamingIcon = $('<img>', {
            'id': '-iplate-livestreaming'
        }).addClass('align-right');
        that.$recordingsIcon = $('<img>', {
            'id': '-iplate-recordings'
        }).addClass('align-right');
        that.$liveStreamingText = $('<div>').text('Live Streaming').addClass('align-right');
        that.$recordingsText = $('<div>').text(that.recordingsTotal + (that.recordingsTotal > 1 ? ' Recordings' : ' Recording')).addClass('align-right');

        that.$banner.append(that.$background).append(that.$thumbnail).append(that.$lcn).
            append($splitLine).append(that.$serviceName).append(that.$eventName).
            append(that.$secondary).append(that.$startTime).append(that.$endTime).
            append(that.$now).append(that.$group).append(that.$icons).append(that.$arrowLeft).
            append(that.$arrowRight).append(that.$arrowUp).append(that.$arrowDown).
            append($progress).append(that.$siIcons);

        that.$footer.append($moreIcon).append($moreText).
            append($chListIcon).append($chListText).append(that.$recordingsText).
            append(that.$recordingsIcon).append(that.$liveStreamingText).append(that.$liveStreamingIcon);

        that.$self.append(that.$dim).append(that.$banner).append(that.$footer);

        $wrap.append(that.$self);
    }

    drawSiIcons() {
        var that = this;
        var $hd, $pd_hd, $guidance, $recommendation, $split;
        var $ad, $lock, $ttx, $subDVB, $subEBU, $ma;
        var $available, $reminder, $recording, $schedule, $seriesSchedule;
        var $recorded;

        if (that.$siIcons) {
            $hd = $('<img>', {
                'id': 'hd'
            });
            $pd_hd = $('<img>', {
                'id': 'hd_linkage'
            });
            $guidance = $('<img>', {
                'id': 'guidance'
            });
            $recommendation = $('<img>', {
                'id': 'recommendation'
            });
            $split = $('<img>', {
                'id': 'split'
            });
            $ad = $('<img>', {
                'id': 'ad'
            });
            $lock = $('<img>', {
                'id': 'lock'
            });
            $ttx = $('<img>', {
                'id': 'ttx'
            });
            $subDVB = $('<img>', {
                'id': 'subDVB'
            });
            $subEBU = $('<img>', {
                'id': 'subEBU'
            });
            $ma = $('<img>', {
                'id': 'ma'
            });
            $available = $('<img>', {
                'id': 'available'
            });
            $reminder = $('<img>', {
                'id': 'reminder'
            });
            $recording = $('<img>', {
                'id': 'recording'
            });
            $schedule = $('<img>', {
                'id': 'schedule'
            });
            $seriesSchedule = $('<img>', {
                'id': 'series-schedule'
            });
            $recorded = $('<img>', {
                'id': 'recorded'
            });

            that.$siIcons.append($available).append($recorded).append($lock).append($hd).append($pd_hd).
                append($guidance).append($recommendation).append($split).append($ad).
                append($ttx).append($subDVB).append($subEBU).append($ma).append($reminder).
                append($recording).append($schedule).append($seriesSchedule);
        }
    }

    updateUICurrentTime() {
        var that = this;
        var now = new Date();
        var date = dateFormat(now, 'ddd dd mmm');
        var time = dateFormat(now, 'HH:MM').toUpperCase();

        if (that.$now) {
            that.$now.html(time + ' <span>|</span> ' + date);
            that.now = now;
        }
    }

    updateUIServiceInfo() {
        var that = this;
        if (!that.showingChannel) {
            return;
        }
        var channel = that.showingChannel.channel;
        var group = that.showingChannel.group;
        var lcn;
        if (channel) {
            lcn = hx.il.makeDigitNumber(channel.majorChannel);
            that.$lcn.text(lcn);
            that.$serviceName.text(channel.name);
        }
        that.$group.text(that.showingChannel.groupName || group);
    }

    updateUISiIcons(event) {
        var that = this;
        var $siIcons = that.$siIcons;
        var subtitleTypes;
        var i = 0, total = 0;
        var dvb: boolean = false, ebu: boolean = false;
        if ($siIcons) {
            $siIcons.removeClass();
        }
        if (event) {
            if (event.isHd && event.isHd()) {
                $siIcons.addClass('-iplate-hd');
            }
            if (event.getHDLinkage && event.getHDLinkage() && event.getHDLinkage().length) {
                $siIcons.addClass('-iplate-hd_linkage');
            }
            if (event.hasGuidance && event.hasGuidance()) {
                $siIcons.addClass('-iplate-guidance');
            }
            if (event.getRecommendationCRID && event.getRecommendationCRID()) {
                $siIcons.addClass('-iplate-recommendation');
            }
            if (event.getProgrammeCRID && event.getProgrammeCRID() && (event.getProgrammeCRID().search("#") > 0)) {
                $siIcons.addClass('-iplate-split');
            }
            if (event.isAd && event.isAd()) {
                $siIcons.addClass('-iplate-ad');
            }
            if (event.hasTeletext && event.hasTeletext()) {
                $siIcons.addClass('-iplate-ttx');
            }
            if (event.getSubtitleTypes) {
                subtitleTypes = event.getSubtitleTypes();
                if (subtitleTypes) {
                    total = subtitleTypes.length;
                    for (i = 0; i < total; i++) {
                        if (subtitleTypes[i] === "DVB" && dvb === false) {
                            $siIcons.addClass('-iplate-subDVB');
                            dvb = true;
                        } else if (subtitleTypes[i] === "EBU" && ebu === false) {
                            $siIcons.addClass('-iplate-subEBU');
                            ebu = true;
                        }
                    }
                }
            }
            if (event.isMultiAudio && event.isMultiAudio()) {
                $siIcons.addClass('-iplate-ma');
            }
            if ((that.eventIndex === that.LIVE_INDEX) && that.checkRecording()) {
                $siIcons.addClass('-iplate-recording');
            }
            if (that.eventIndex !== that.LIVE_INDEX) {
                if (event.hasReminder && event.hasReminder()) {
                    $siIcons.addClass('-iplate-reminder');
                }
                if (event.hasSchedule && event.hasSchedule()) {
                    $siIcons.addClass('-iplate-schedule');
                }
                if (event.hasSeriesSchedule && event.hasSeriesSchedule()) {
                    $siIcons.addClass('-iplate-series-schedule');
                }
            }
            if (event.getAvailability() === __PROVIDER__.EOndemandAvailable.EAvailable) {
                $siIcons.addClass('-iplate-available');
            } else if (event.getAvailability() === __PROVIDER__.EOndemandAvailable.EComingSoon) {
                $siIcons.addClass('-iplate-commingsoon');
            }
            if (event.hasRecording()) {
                $siIcons.addClass('-iplate-recorded');
            }
        }
        if (that.isLocked) {
            $siIcons.addClass('-iplate-lock');
            that.isLocked = true;
        }
    }

    checkRecording() {
        var that = this;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var channel;
        var recordingItem = null;

        if (that.showingChannel) {
            channel = that.showingChannel.channel;
            if (channel) {
                recordingItem = dsrvRecordings.getCurrentRecordingByCcid(channel.ccid);
            }
        }

        if (recordingItem) {
            return true;
        }

        return false;
    }

    onRecordingEventListener() {
        var that = this;

        if (that.$siIcons) {
            if ((that.eventIndex === that.LIVE_INDEX) && that.checkRecording()) {
                that.$siIcons.addClass('-iplate-recording');
            } else {
                that.$siIcons.removeClass('-iplate-recording');
            }
        }
    }

    onScheduleEventListener(item) {
        var that = this;
        var programmeID;
        var showingEvent;
        if (that.state === CPgIPlate.STATE.FORWARD) {
            if (item && item.getProgrammeID) {
                programmeID = item.getProgrammeID();
                if (that.forwardEvents) {
                    showingEvent = that.forwardEvents[that.eventIndex];
                    if (showingEvent) {
                        if (showingEvent.getProgrammeID() === programmeID) {
                            if (that.$siIcons) {
                                if (showingEvent.hasSchedule()) {
                                    that.$siIcons.addClass('-iplate-schedule');
                                } else {
                                    that.$siIcons.removeClass('-iplate-schedule');
                                }
                                if (showingEvent.hasSeriesSchedule()) {
                                    that.$siIcons.addClass('-iplate-series-schedule');
                                } else {
                                    that.$siIcons.removeClass('-iplate-series-schedule');
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    updateUIThumbnail(url) {
        var that = this;
        var defaultUrl = __MODEL__.TDefaultThumbnailList.TV;

        if (that.state === CPgIPlate.STATE.BACKWARD) {
            defaultUrl = __MODEL__.TDefaultThumbnailList.FREEVIEW;
        } else {
            var showingChannel = that.showingChannel;
            if (showingChannel && showingChannel.channel) {
                defaultUrl = (showingChannel.channel.channelType === 1) ? __MODEL__.TDefaultThumbnailList.RADIO : __MODEL__.TDefaultThumbnailList.TV;
            }
        }

        if (that.$thumbnail) {
            that.$thumbnail.removeClass();
            if (that.isLocked === false) {
                if (url) {
                    that.$thumbnail.attr('src', url);
                } else {
                    that.$thumbnail.attr('src', defaultUrl);
                }
            } else {
                that.$thumbnail.attr('src', defaultUrl);
            }
        }
    }

    onUpdateUIThumbnail(event, url) {
        var that = this;
        var showingEvent;
        var showingChannel;

        if (that.state === CPgIPlate.STATE.FORWARD) {
            showingEvent = that.forwardEvents[that.eventIndex];
        } else {
            showingEvent = that.backwardEvents[that.eventIndex];
        }

        if (event && showingEvent && that.showingChannel) {
            showingChannel = that.showingChannel.channel;
            if (showingChannel && event.channel) {
                if (showingChannel.sid === event.channel.sid) {
                    if (showingEvent.getProgrammeID() === event.getProgrammeID()) {
                        that.updateUIThumbnail(url);
                    }
                }
            }
        }
    }

    onUpdateUIAvailableIcon(event) {
        var that = this;
        var showingEvent;

        if (that.state === CPgIPlate.STATE.FORWARD) {
            showingEvent = that.forwardEvents[that.eventIndex];
            if (showingEvent.getProgrammeID() === event.getProgrammeID()) {
                that.updateUISiIcons(showingEvent);
            }
        } else {
            showingEvent = that.backwardEvents[that.eventIndex];
            if (showingEvent.getProgrammeID() === event.getProgrammeID()) {
                that.updateUISiIcons(showingEvent);
            }
        }
    }

    updateUIArrow() {
        var that = this;

        if (that.$arrowUp && that.$arrowDown && that.$arrowLeft && that.$arrowRight) {
            that.$arrowUp.removeClass('show');
            that.$arrowDown.removeClass('show');
            that.$arrowLeft.removeClass('show');
            that.$arrowRight.removeClass('show');

            if (that.liveChannel && that.liveChannel.result === true) {
                that.$arrowUp.addClass('show');
                that.$arrowDown.addClass('show');
            }

            // RIGHT
            if (that.state === CPgIPlate.STATE.FORWARD) {
                if (that.forwardEvents && (that.forwardEvents.length > 0)) {
                    if (that.eventIndex < (that.forwardEvents.length - 1)) {
                        that.$arrowRight.addClass('show');
                    }
                }
            } else {
                that.$arrowRight.addClass('show');
            }

            // LEFT
            if (that.state === CPgIPlate.STATE.FORWARD) {
                that.$arrowLeft.addClass('show');
            } else {
                if (that.backwardEvents && (that.backwardEvents.length > 0)) {
                    if (that.eventIndex < (that.backwardEvents.length -1)) {
                        that.$arrowLeft.addClass('show');
                    }
                }
            }
        }
    }

    updateUIBlankEventInfo() {
        var that = this;

        that.$eventName.text('');
        that.$secondary.text('');
        that.$startTime.text('');
        that.$endTime.text('');
        that.$progressBG.removeClass('show');
        that.$progressBar.removeClass('show');
        that.updateUISiIcons(null);
        that.updateUIThumbnail(null);
    }

    updateUIEventInfo(event) {
        var that = this;
        var dsrvTsr = hx.svc.TSR;
        var startDate = null, endDate = null, strStartTime = '', strEndTime = '', strDate = '';
        var percentage = 0;
        var needProgress: boolean = false;

        if (event) {
            if (that.state === CPgIPlate.STATE.FORWARD) {
                that.$eventName.text(event.getProgrammeName());
                if (event.longName) {
                    that.$secondary.text(event.longName);
                } else {
                    that.$secondary.text(event.getShortDescription());
                }
                startDate = new Date(event.getStartTime() * 1000);
                endDate = new Date(event.getEndTime() * 1000);
                strStartTime = dateFormat(startDate, "HH:MM");
                strEndTime = dateFormat(endDate, "HH:MM");
                if (event.isLive() || event.isPast()) {
                    that.$startTime.text(strStartTime);
                    that.$endTime.text(strEndTime);
                    percentage = event.getPercentage();
                    if (event.isPast && event.isPast()) {
                        if (dsrvTsr.isTsrOngoing() === true) {
                            var TSRstartTime : number = dsrvTsr.getStartTime();
                            var TSRplaybackOffset : number = dsrvTsr.getPlaybackOffset();
                            var TSRTime : number = TSRstartTime + TSRplaybackOffset;
                            percentage = event.getPercentageByTime(TSRTime);
                        }
                    }
                    percentage = (percentage > 100) ? 100 : percentage;
                    percentage = (percentage < 0) ? 0 : percentage;
                    that.$progressBG.addClass('show');
                    that.$progressBar.width(percentage + '%');
                    that.$progressBar.addClass('show');
                } else {
                    strDate = dateFormat(startDate, 'ddd dd mmm');
                    that.$startTime.html(strStartTime + ' - ' + strEndTime + ' <span>|</span> ' + strDate);
                    that.$endTime.text('');
                    that.$progressBG.removeClass('show');
                    that.$progressBar.removeClass('show');
                }
                that.updateUISiIcons(event);
            } else {
                that.$eventName.text(event.getProgrammeName());
                that.$secondary.text(event.getShortDescription());
                startDate = new Date(event.getStartTime() * 1000);
                endDate = new Date((event.getEndTime()) * 1000);
                strStartTime = dateFormat(startDate, "HH:MM");
                strEndTime = dateFormat(endDate, "HH:MM");
                strDate = dateFormat(startDate, 'ddd dd mmm');
                that.$startTime.html(strStartTime + ' - ' + strEndTime + ' <span>|</span> ' + strDate);
                that.$progressBG.removeClass('show');
                that.$progressBar.removeClass('show');
                that.updateUISiIcons(event);
            }
        } else {
            that.$eventName.text('No information');
            that.$secondary.text('');
            that.$startTime.text('');
            that.$endTime.text('');
            that.$progressBG.removeClass('show');
            that.$progressBar.width('0%');
            that.$progressBar.removeClass('show');
            that.updateUISiIcons(null);
        }
        that.updateUIArrow();
    }

    updateUIRecordingsInfo() {
        var that = this;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var liveStreaming, recordings;

        liveStreaming = dsrvRecordings.getAllStreamingList();
        if (liveStreaming) {
            that.liveStreamingTotal = liveStreaming.length;
        }

        recordings = dsrvRecordings.getCurrentRecordings();
        if (recordings) {
            that.recordingsTotal = recordings.length;
        }

        that.$liveStreamingText.text('Live Streaming');
        that.$recordingsText.text(that.recordingsTotal + (that.recordingsTotal > 1 ? ' Recordings' : ' Recording'));

        if (that.liveStreamingTotal > 0) {
            that.$liveStreamingIcon.addClass('show');
            that.$liveStreamingText.addClass('show');
        } else {
            that.$liveStreamingIcon.removeClass('show');
            that.$liveStreamingText.removeClass('show');
        }

        if (that.recordingsTotal > 0) {
            that.$recordingsIcon.addClass('show');
            that.$recordingsText.addClass('show');
        } else {
            that.$recordingsIcon.removeClass('show');
            that.$recordingsText.removeClass('show');
        }
    }

    show() {
        var that = this;

        that.eventIndex = 0;
        that.state = CPgIPlate.STATE.FORWARD;

        that.load();
        that.updateUIServiceInfo();
        that.updateUIBlankEventInfo();
        that.resetExpireTimer();
        that.resetEpgQueryTimer();

        that.active = true;
        if (that.$self) {
            that.$self.addClass('show');
        }

        this.sendEventToEntity({
            alEvent: 'alCheckPageVisible'
        });
    }

    hide() {
        var that = this;
        if (that.$self) {
            that.$self.removeClass('show');
        }

        that.showingChannel = that.liveChannel;
        that.active = false;
        that.eventIndex = 0;
        that.liveStreamingTotal = 0;
        that.recordingsTotal = 0;
        that.hasBackward = false;
        that.keyRepeat = false;
        that.isLocked = false;
        if (that.forwardEvents) {
            delete that.forwardEvents;
            that.forwardEvents = [];
        }
        if (that.backwardEvents) {
            delete that.backwardEvents;
            that.backwardEvents = [];
        }
        that.killTimer();

        this.sendEventToEntity({
            alEvent: 'alCheckPageVisible'
        });
    }

    /* related data */
    load() {
        var that = this;
        var dsrvChList = hx.svc.CH_LIST;
        var liveChannel: any;

        liveChannel= dsrvChList.getCurrentChannel();
        if (liveChannel && (liveChannel.result === true)) {
            that.liveChannel = liveChannel;
            that.showingChannel = liveChannel;
            if (liveChannel.channel && liveChannel.channel.manualBlock) {
                that.isLocked = true;
            } else {
                that.isLocked = false;
            }
            that.loadChannelData();
        } else {
            if (that.forwardEvents) {
                that.updateUIEventInfo(that.forwardEvents[that.LIVE_INDEX]);
            } else {
                that.updateUIEventInfo(null);
            }
        }

        that.updateUIRecordingsInfo();
    }

    loadChannelData(channel?) {
        var that = this;
        if (that.showingChannel === channel) {
            hx.log('debug', 'no need to loadChannelData with same channel : ' + channel.channel.name);
            return;
        }
        var dsrvTsr = hx.svc.TSR;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var startTime, endTime;

        startTime = Math.floor(that.now.getTime() / 1000);
        if (dsrvTsr.isTsrOngoing() === true) {
            var VBChannel = dsrvAvCtrl.getVideoBroadcastCurrentChannel();
            if (!!VBChannel && VBChannel.ccid === that.showingChannel.channel.ccid) {
                var TSRstartTime = dsrvTsr.getStartTime();
                var TSRplaybackOffset = dsrvTsr.getPlaybackOffset();
                hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
                startTime = TSRstartTime + TSRplaybackOffset;
            }
        }
        endTime = startTime + 3600 * 12; // 12hours
        ((reqChannel) => {
            that.searchEPG(reqChannel, startTime, endTime).then((param: any) => {
                if (that.state === CPgIPlate.STATE.FORWARD) {
                    if (reqChannel && that.showingChannel && that.showingChannel.channel) {
                        if (reqChannel.sid === that.showingChannel.channel.sid) {
                            that.setForwardEvents(param.result[0]);
                            if (that.forwardEvents) {
                                that.updateUIEventInfo(that.forwardEvents[that.LIVE_INDEX]);
                            } else {
                                that.updateUIEventInfo(null);
                            }
                            param.mergeMDS();
                        }
                    }
                }
            }, () => {
                hx.log('error', '[page_iplate.js][load] searchEPG Fail!!');
                if (that.state === CPgIPlate.STATE.FORWARD) {
                    if (reqChannel && that.showingChannel && that.showingChannel.channel) {
                        if (reqChannel.sid === that.showingChannel.channel.sid) {
                            that.setForwardEvents([]);
                            that.updateUIEventInfo(null);
                        }
                    }
                }
            });
        })(that.showingChannel.channel);
    }

    searchEPG(channel, startTime, endTime): Q.Promise<any> {
        var that = this;
        var retDef = Q.defer();

        hx.svc.EPG.getProgrammes({
            isOnAir: false,
            channel: channel,
            startTime: startTime,
            endTime: endTime,
            listener: this,
            notUI: true,
            cb: (result) => {
                if (result && result.length) {
                    if (result[0].length) {
                        var mergeMDS = (function (channel, startTime) {
                            return function () {
                                that.searchMDS(channel, startTime, 3).then((xmls) => {
                                    if (xmls && xmls.length > 0) {
                                        that.updateForwardMDS(xmls, channel);
                                        if (that.forwardEvents) {
                                            if (that.forwardEvents.length > 0) {
                                                that.updateUIEventInfo(that.forwardEvents[that.eventIndex]);
                                            }
                                        }
                                    } else {
                                        if (that.state === CPgIPlate.STATE.FORWARD) {
                                            if (channel && that.showingChannel && that.showingChannel.channel) {
                                                if (channel.sid === that.showingChannel.channel.sid) {
                                                    if (that.forwardEvents.length > 0) {
                                                        var event = that.forwardEvents[that.eventIndex];
                                                        if (event && event.getThumbnail) {
                                                            that.updateUIThumbnail(event.getThumbnail());
                                                        } else {
                                                            that.updateUIThumbnail(null);
                                                        }
                                                    } else {
                                                        that.updateUIThumbnail(null);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }, () => {
                                    if (that.state === CPgIPlate.STATE.FORWARD) {
                                        if (channel && that.showingChannel && that.showingChannel.channel) {
                                            if (channel.sid === that.showingChannel.channel.sid) {
                                                if (that.forwardEvents.length > 0) {
                                                    var event = that.forwardEvents[that.eventIndex];
                                                    if (event && event.getThumbnail) {
                                                        that.updateUIThumbnail(event.getThumbnail());
                                                    } else {
                                                        that.updateUIThumbnail(null);
                                                    }
                                                } else {
                                                    that.updateUIThumbnail(null);
                                                }
                                            }
                                        }
                                    }
                                });
                            }
                        })(channel, result[0][0].getStartTime() * 1000);

                        retDef.resolve({
                            result : result,
                            mergeMDS : mergeMDS
                        });
                    } else {
                        retDef.reject(null);
                    }
                } else {
                    retDef.reject(null);
                }
            }
        });

        return retDef.promise;
    }

    searchMDS(channel, timeBase, chunk): Q.Promise<any> {
        var retDef = Q.defer();
        var dsrvIpEpg = hx.svc.IP_EPG;
        var xmls = [];
        var queryList: IQuerySchedule[] = [];
        var serviceId = channel.sid;
        var i = 0;
        var HOUR6 = 6;

        function addXml(xml?) {
            if (xml) {
                xmls.push(dsrvIpEpg.parse.xml.schedule(xml));
            }
        }
        function addScheduleQueryToQueryList(serviceId : any, timeBase : number, hours : number){
            var _retDef = Q.defer();
            _retDef.promise.then(addXml);
            var query : IQuerySchedule ={
                serviceId : serviceId,
                startTime : timeBase,
                hours : hours,
                retDef : _retDef
            };
            queryList.push(query);
        }

        for (i = 0; i < chunk; i++) {
            addScheduleQueryToQueryList(serviceId, timeBase + ((HOUR6 * 60 * 60 * 1000) * i), HOUR6);
        }

        dsrvIpEpg.getScheduleList(queryList).then(() => {
            retDef.resolve(xmls);
        }, () => {
            retDef.reject(null);
        });

        return retDef.promise;
    }

    setForwardEvents(events) {
        var that = this;
        var i = 0, j = 0, total = 0, originTotal = 0;
        var originEvent;

        if (events) {
            if (events.length > 20) {
                events.length = 20;
            }
            total = events.length;
        }
        if (that.forwardEvents) {
            originTotal = that.forwardEvents.length;
        }

        for (i = 0; i < originTotal; i++) {
            originEvent = that.forwardEvents[i];
            for (j = 0; j < total; j++) {
                if (originEvent.getProgrammeID() === events[j].getProgrammeID()) {
                    events[j].thumbnailURL = originEvent.thumbnailURL;
                }
            }
        }

        if (that.forwardEvents) {
            delete that.forwardEvents;
            that.forwardEvents = undefined;
            that.forwardEvents = [];
        }

        for (i = 0 ; i < total ; i++) {
            event = events[i];
            that.forwardEvents.push(event);
        }
    }
    appendForwardEvents(events) {
        var that = this;
        var i = 0, total = 0;

        if (events) {
            if (events.length > 20) {
                events.length = 20;
            }
            total = events.length;

            for (i = 0 ; i < total ; i++) {
                that.forwardEvents.push(events[i]);
            }
        }
    }

    appendBackwardEvents(events) {
        var that = this;
        var i = 0, j = 0, total = 0, backwardsTotal = 0;
        var event, newEvent;
        var already = false;

        if (that.backwardEvents) {
            backwardsTotal = that.backwardEvents.length;
        }

        if (events) {
            total = events.length;

            for (i = 0; i < total; i++) {
                already = false;
                newEvent = events[i];
                if (newEvent) {
                    for (j = 0; j < backwardsTotal; j++) {
                        event = that.backwardEvents[j];
                        if (event) {
                            if (event.getProgrammeID() === newEvent.getProgrammeID()) {
                                already = true;
                                break;
                            }
                        }
                    }
                }

                if (already === false) {
                    that.backwardEvents.push(newEvent);
                }
            }
        }

        if (that.hasBackward === false) {
            if (that.backwardEvents) {
                if (that.backwardEvents.length > 0) {
                    that.hasBackward = true;
                }
            }
        }
    }

    updateBackwardMDS(channel, xmls, timeBase, threshold) {
        var that = this;
        var programmes = [];
        var mdsProgramme = null;
        var schedules = null, crids = null;
        var i = 0, len = 0, schedule = null, crid = null, data = null;
        var now = that.now.getTime();
        var HOUR6 = 6;

        if (xmls !== null) {
            xmls.forEach((info) => {
                schedules = info.schedules;
                crids = info.crids;
                for (i = 0, len = schedules.length; i < len; i++) {
                    schedule = schedules[i];
                    crid = schedule.querySelector('Program').getAttribute('crid');
                    data = crids[crid];
                    data.schedule = schedule;
                    data.available = false;
                    mdsProgramme = new __PROVIDER__.CProgrammeProvider({
                        mdsData : data,
                        channel : channel,
                        launchLocation : 'epg'
                    });
                    if ((mdsProgramme.getEndTime() * 1000) < now) {
                        programmes.push(mdsProgramme);
                        ((aProg) => {
                            setTimeout(() => {
                                hx.svc.IP_EPG.getAIT(aProg.getTemplateAitUrl()).then((xml) => {
                                    aProg.checkTemplateAIT(xml);
                                    that.onUpdateUIAvailableIcon(aProg);
                                }, (err) => {});
                            }, 10);
                        })(mdsProgramme);
                    }
                    mdsProgramme = null;
                }
            });
            if (programmes.length > 1) {
                programmes.sort(function(prev, after) {
                    return prev.getStartTime() - after.getStartTime() < 0 ? 1 : -1;
                });
                that.appendBackwardEvents(programmes);
                for (var i = 0 ; i < programmes.length ; i++) {
                    mdsProgramme = programmes[i];
                    mdsProgramme.getThumbnail({
                        updateCb : (function (aProg) {
                            return function(url) {
                                that.onUpdateUIThumbnail(aProg, url);
                            }
                        })(mdsProgramme),
                        width : that.THUMBNAIL_WIDTH
                    });
                }
            } else if (programmes.length === 1) {
                if ((programmes[0].startTime + programmes[0].duration) * 1000 < now) { // If not ON AIR
                    that.appendBackwardEvents(programmes);
                    mdsProgramme = programmes[0];
                    mdsProgramme.getThumbnail({
                        updateCb : (function (aProg) {
                            return function(url) {
                                that.onUpdateUIThumbnail(aProg, url);
                            }
                        })(mdsProgramme),
                        width : that.THUMBNAIL_WIDTH
                    });
                }
                if (timeBase - (HOUR6 * 60 * 60 * 1000) >= threshold) {
                    that.searchMDS(channel, timeBase - (HOUR6 * 60 * 60 * 1000), 2).then((xmls) => {
                        that.updateBackwardMDS(channel, xmls, timeBase - (HOUR6 * 60 * 60 * 1000), threshold);
                        that.updateUIArrow();
                    }, () => {
                        hx.log('error', 'error!! [updateBackwardMDS]');
                    });
                }
            } else {
                if (timeBase - (HOUR6 * 60 * 60 * 1000) >= threshold) {
                    that.searchMDS(channel, timeBase - (HOUR6 * 60 * 60 * 1000), 2).then((xmls) => {
                        that.updateBackwardMDS(channel, xmls, timeBase - (HOUR6 * 60 * 60 * 1000), threshold);
                        if (that.backwardEvents && that.backwardEvents.length > 0) {
                            var event = that.backwardEvents[that.eventIndex];
                            that.updateUIEventInfo(event);
                        } else {
                            that.updateUIArrow();
                        }
                    }, () => {
                        hx.log('error', 'error!! [updateBackwardMDS]');
                    });
                }
            }
            mdsProgramme = null;
            programmes = [];
        }
    }

    updateForwardMDS(xmls, channel) {
        var that = this;
        var dsrvIpEpg = hx.svc.IP_EPG;
        var oipfProgramme = null;
        var schedules = null, crids = null;
        var schedule = null, crid = null, data = null;
        var i = 0, j = 0, len = 0;
        var forwardTotal = that.forwardEvents ? that.forwardEvents.length : 0;
        var forwardEvent, mdsEvent;
        var parsedId, legacyId, eventId;
        if (xmls !== null) {
            xmls.forEach((info) => {
                schedules = Array.prototype.slice.call(info.schedules);
                crids = info.crids;
                for (i = 0; i < forwardTotal; i++) {
                    forwardEvent = that.forwardEvents[i];
                    if (forwardEvent instanceof __PROVIDER__.CProgrammeProvider) {
                        parsedId = dsrvIpEpg.parse.util.programIdParser(forwardEvent.getProgrammeID());
                        legacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                        for (j = 0, len = schedules.length; j < len; j++) {
                            schedule = schedules[j];
                            mdsEvent = dsrvIpEpg.parse.element.scheduleEvent(schedule);
                            parsedId = dsrvIpEpg.parse.util.programIdParser(mdsEvent.programURL);
                            eventId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                            if (eventId === legacyId) {
                                crid = schedule.querySelector('Program').getAttribute('crid');
                                data = crids[crid];
                                data.schedule = schedule;
                                data.available = false;
                                forwardEvent.setMdsData({
                                    mdsData : data,
                                    launchLocation : 'epg'
                                });
                                ((aProg) => {
                                    setTimeout(() => {
                                        dsrvIpEpg.getAIT(aProg.getTemplateAitUrl()).then((xml) => {
                                            aProg.checkTemplateAIT(xml);
                                            that.onUpdateUIAvailableIcon(aProg);
                                        }, (err) => {});
                                    }, 10);
                                })(forwardEvent);
                                data = null;
                                schedules.splice(j, 1);
                                break;
                            }
                        }
                    }
                }
            });
            oipfProgramme = null;
        }
        for (i = 0 ; i < forwardTotal ; i++) {
            forwardEvent = that.forwardEvents[i];
            forwardEvent.getThumbnail({
                updateCb : (function (aProg) {
                    return function(url) {
                        that.onUpdateUIThumbnail(aProg, url);
                    }
                })(forwardEvent),
                width : that.THUMBNAIL_WIDTH
            });
        }
    }

    /* related operation */
    changeChannel(channel) {
        var that = this;

        if (channel && (channel.result === true)) {
            that.showingChannel = channel;
            if (channel.channel && channel.channel.manualBlock) {
                that.isLocked = true;
            } else {
                that.isLocked = false;
            }
            that.eventIndex = 0;
            that.updateUIBlankEventInfo();
            if (!that.keyRepeat && that.performanceTimer === -1) {
                that.loadChannelData();
                that.resetPerformanceTimer(channel);
            } else {
                that.resetPerformanceTimer();
            }
        }
    }
    changeNextChannel() {
        var that = this;
        var dsrvChList = hx.svc.CH_LIST;
        var showingChannel = that.showingChannel;
        var nextChannel;

        if (showingChannel) {
            nextChannel = dsrvChList.getNextChannel({
                ccid: showingChannel.channel.ccid,
                group: showingChannel.group
            });
            that.hasBackward = false;
            that.state = CPgIPlate.STATE.FORWARD;
            that.changeChannel(nextChannel);
            that.updateUIServiceInfo();
            if (that.keyRepeat) {
                that.killEpgQueryTimer();
            } else {
                that.resetEpgQueryTimer();
            }
        }
    }

    changePrevChannel() {
        var that = this;
        var dsrvChList = hx.svc.CH_LIST;
        var showingChannel = that.showingChannel;
        var prevChannel;

        if (showingChannel) {
            prevChannel = dsrvChList.getPrevChannel({
                ccid: showingChannel.channel.ccid,
                group: showingChannel.group
            });
            that.hasBackward = false;
            that.state = CPgIPlate.STATE.FORWARD;
            that.changeChannel(prevChannel);
            that.updateUIServiceInfo();
            if (that.keyRepeat) {
                that.killEpgQueryTimer()
            } else {
                that.resetEpgQueryTimer();
            }
        }
    }

    changeNextEvent() {
        var that = this;
        var dsrvTsr = hx.svc.TSR;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var total = 0;
        var lastEvent, nextEvent;
        var startTime, endTime;
        var channel;

        if (that.eventIndex === 0 && that.state === CPgIPlate.STATE.BACKWARD) {
            that.state = CPgIPlate.STATE.FORWARD;
            if (that.showingChannel) {
                channel = that.showingChannel.channel;
                startTime = Math.floor(that.now.getTime() / 1000);
                if (dsrvTsr.isTsrOngoing() === true) {
                    var VBChannel = dsrvAvCtrl.getVideoBroadcastCurrentChannel();
                    if (!!VBChannel && VBChannel.ccid === channel.ccid) {
                        var TSRstartTime = dsrvTsr.getStartTime();
                        var TSRplaybackOffset = dsrvTsr.getPlaybackOffset();
                        hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
                        startTime = TSRstartTime + TSRplaybackOffset;
                    }
                }
                endTime = startTime + 3600 * 12; // 12hours
                ((reqChannel) => {
                    that.searchEPG(reqChannel, startTime, endTime).then((param: any) => {
                        if (that.state === CPgIPlate.STATE.FORWARD) {
                            if (reqChannel && that.showingChannel && that.showingChannel.channel) {
                               if (reqChannel.sid === that.showingChannel.channel.sid) {
                                   that.setForwardEvents(param.result[0]);
                                   if (that.forwardEvents) {
                                       that.updateUIEventInfo(that.forwardEvents[that.LIVE_INDEX]);
                                   } else {
                                       that.updateUIEventInfo(null);
                                   }
                                   param.mergeMDS();
                               }
                            }
                        }
                    }, () => {
                        hx.log('error', '[page_iplate.js][load] searchEPG Fail!!');
                    });
                })(channel);
                that.resetEpgQueryTimer();
            }
        } else {
            if (that.state === CPgIPlate.STATE.FORWARD) {
                if (that.forwardEvents) {
                    total = that.forwardEvents.length;
                }
                if (total > 0) {
                    if (total - (that.eventIndex + 1) < that.THRESHHOLD) {
                        lastEvent = that.forwardEvents[total - 1];
                        if (lastEvent) {
                            startTime = lastEvent.getEndTime() + 1;
                            endTime = startTime + (3600 * 12);
                            // search epg
                            if (that.showingChannel) {
                                channel = that.showingChannel.channel;
                                ((reqChannel) => {
                                    that.searchEPG(reqChannel, startTime, endTime).then((param: any) => {
                                        // append event
                                        if (that.state === CPgIPlate.STATE.FORWARD) {
                                            if (reqChannel && that.showingChannel && that.showingChannel.channel) {
                                                if (reqChannel.sid === that.showingChannel.channel.sid) {
                                                    that.appendForwardEvents(param.result[0]);
                                                    that.updateUIArrow();
                                                    param.mergeMDS();
                                                }
                                            }
                                        }
                                    }, () => {
                                        hx.log('error', '[page_iplate.js][changeNextEvent] searchEPG Fail!!');
                                    });
                                })(channel);
                            }
                        }
                    }
                }

                if (that.forwardEvents) {
                    if (total > 0) {
                        if (that.eventIndex < (total - 1)) {
                            nextEvent = that.forwardEvents[that.eventIndex + 1];
                            if (nextEvent) {
                                that.eventIndex++;
                                that.updateUIEventInfo(nextEvent);
                                if (nextEvent && nextEvent.getThumbnail) {
                                    that.updateUIThumbnail(nextEvent.getThumbnail());
                                }
                                that.killEpgQueryTimer();
                                if (that.eventIndex === that.LIVE_INDEX) {
                                    that.resetEpgQueryTimer();
                                }
                            }
                        }
                    }
                } else {
                    that.updateUIEventInfo(null);
                }
            } else {
                if (that.backwardEvents) {
                    if (that.eventIndex > 0) {
                        nextEvent = that.backwardEvents[that.eventIndex - 1];
                        if (nextEvent) {
                            that.eventIndex--;
                            that.updateUIEventInfo(nextEvent);
                            that.updateUIThumbnail(nextEvent.getThumbnail());
                        }
                    }
                }
            }
        }
    }

    changePrevEvent() {
        var that = this;
        var prevEvent;
        var channel;
        var firstEvent;
        var now = that.now.getTime();
        var timeBase = 0;
        var HOUR6 = 6;
        var total = 0;

        if (that.eventIndex === 0 && that.state === CPgIPlate.STATE.FORWARD) {
            that.state = CPgIPlate.STATE.BACKWARD;
            that.killEpgQueryTimer();
            that.updateUIBlankEventInfo();
            if (that.showingChannel) {
                channel = that.showingChannel.channel;
                timeBase = now - (HOUR6 * 60 * 60 * 1000);
                if (that.backwardEvents) {
                    delete that.backwardEvents;
                    that.backwardEvents = [];
                }
                if (channel) {
                    if (!that.progressMsg) {
                        that.progressMsg = hx.msg('progress', {
                            text : hx.l('LOC_PLEASE_WAIT_ID'),
                            auto_close : true,
                            timeout : 10000
                        });
                    }
                    that.searchMDS(channel, timeBase, 2).then((xmls) => {
                        that.updateBackwardMDS(channel, xmls, timeBase, now - that.DAY7);
                        if (that.backwardEvents) {
                            if (that.backwardEvents.length > 0) {
                                that.updateUIEventInfo(that.backwardEvents[that.eventIndex]);
                            } else {
                                that.updateUIEventInfo(null);
                            }
                        } else {
                            that.updateUIEventInfo(null);
                        }
                        hx.msg.close('progress', that.progressMsg);
                        that.progressMsg = undefined;
                    }, () => {
                        hx.log('error', 'error!! show searchMDS!!');
                        that.updateUIEventInfo(null);
                        hx.msg.close('progress', that.progressMsg);
                        that.progressMsg = undefined;
                    });
                }
            }
        }

        if (that.state === CPgIPlate.STATE.FORWARD) {
            if (that.forwardEvents) {
                if (that.eventIndex > 0) {
                    prevEvent = that.forwardEvents[that.eventIndex - 1];
                    if (prevEvent) {
                        that.eventIndex--;
                        that.updateUIEventInfo(prevEvent);
                        if (prevEvent && prevEvent.getThumbnail) {
                            that.updateUIThumbnail(prevEvent.getThumbnail());
                        }
                        that.killEpgQueryTimer();
                        if (that.eventIndex === that.LIVE_INDEX) {
                            that.resetEpgQueryTimer();
                        }
                    }
                }
            }
        } else {
            if (that.backwardEvents) {
                total = that.backwardEvents.length;
            }

            if (that.eventIndex === (total - that.THRESHHOLD)) {
                channel = that.showingChannel.channel;
                if (that.backwardEvents) {
                    firstEvent = that.backwardEvents[total - 1];
                    if (firstEvent) {
                        timeBase = (firstEvent.startTime * 1000);
                        if (!that.progressMsg) {
                            that.progressMsg = hx.msg('progress', {
                                text : hx.l('LOC_PLEASE_WAIT_ID'),
                                auto_close : true,
                                timeout : 10000
                            });
                        }
                        that.searchMDS(channel, timeBase - ((HOUR6 * 60 * 60 * 1000) * 2), 2).then((xmls) => {
                            that.updateBackwardMDS(channel, xmls, timeBase - ((HOUR6 * 60 * 60 * 1000) * 2), timeBase - that.DAY7);
                            that.updateUIArrow();
                            hx.msg.close('progress', that.progressMsg);
                            that.progressMsg = undefined;
                        }, () => {
                            hx.log('error', 'error!! show searchMDS!!');
                            hx.msg.close('progress', that.progressMsg);
                            that.progressMsg = undefined;
                        });
                    }
                }
            }

            if (that.backwardEvents) {
                if (that.eventIndex < (total - 1)) {
                    prevEvent = that.backwardEvents[that.eventIndex + 1];
                    if (prevEvent) {
                        that.eventIndex++;
                        that.updateUIEventInfo(prevEvent);
                        that.updateUIThumbnail(prevEvent.getThumbnail());
                    }
                }
            }
        }
    }

    setChannel() {
        var that = this;
        var changeable = false;
        var channel = null;
        var group = "TV";
        var result = false;

        if (that.showingChannel) {
            channel = that.showingChannel.channel;
            group = that.showingChannel.group;
            if (channel) {
                changeable = hx.svc.CH_LIST.checkChannelChangable({
                    'channel': channel
                });
                if (changeable) {
                    hx.svc.AV_CTRL.channelChange({
                        'channel': channel,
                        'group': group
                    });
                    result = true;
                }
            }
        }

        return result;
    }

    showDetailInfo() {
        var that = this;
        var event;
        if (that.state === CPgIPlate.STATE.FORWARD) {
            if (that.forwardEvents) {
                event = that.forwardEvents[that.eventIndex];
            }
        } else {
            if (that.backwardEvents) {
                event = that.backwardEvents[that.eventIndex];
            }
        }
        if (event) {
            var fnShowProgrammeDetail = function(aEvent) {
                hx.il.adhocPageParam.push({
                    programmeDetailItem : hx.svc.DETAIL_INFO.createProgrammeItem(aEvent),
                    channelGroup : that.showingChannel.group,
                    fnDoHideOnOpen: function(dlg) {
                        dlg.doOpen = function(param) {
                            if (that.$self) {
                                that.$self.removeClass('show');
                            }
                            that.killTimer();
                            prism.CProgrammeDetailDlg.prototype.doOpen.call(dlg, param);
                        };
                    },
                    onClosed : function() {
                        if (that.$self) {
                            that.$self.addClass('show');
                        }
                        that.resetExpireTimer();
                    }
                });
                that.sendEventToEntity({
                    'alEvent' : 'alMessageToPage',
                    'target' : 'CPgDetailInfo/iPlate',
                    'me' : that
                });
            }
            hx.svc.IP_EPG.getDetailInfo(event.getProgrammeCridMds()).then((info) => {
                event.updateMdsData('program', info.querySelector('ProgramInformation'));
                fnShowProgrammeDetail(event);
            }, (err) => {
                fnShowProgrammeDetail(event);
            });
        }
    }

    startRecording() {
        var that = this;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var isGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
        var showingChannel = that.showingChannel.channel;

        if (isGetPvrHDD === false) {
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgNotAvailable?show'
            });
        } else {
            if (that.showingChannel) {
                var recordings = dsrvRecordings.getCurrentRecordings();
                var i, length, recording;
                if (recordings && recordings.length) {
                    length = recordings.length;
                    for (i = 0; i < length; i += 1) {
                        recording = recordings[i];
                        if (recording.getChannelId() === showingChannel.ccid) {
                            dsrvRecordings.stopRecord({
                                item : recording
                            });
                            return;
                        }
                    }
                }
                hx.svc.BRIDGE.requestInstanceRecording(showingChannel);
            }
        }
    }

    cancelRecording() {
        var that = this;
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var currentRecordingList = dsrvRecordings2.getCurrentRecordings();
        var channel;

        if (currentRecordingList && currentRecordingList.length) {
            if (that.showingChannel) {
                channel = that.showingChannel.channel;
                if (channel) {
                    var ccid = channel.ccid;
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
        }
    }

    requestReservation(aItem) {
        var that = this;
        var fnCancelReservation = (data) => {
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
                        hx.log('iplate', 'finished removeScheduledRecording');
                    }
                });
            }
        };
        var fnCancelSeriesReservation = (data) => {
            hx.log('iplate', 'cancelSeriesReservation()');
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
                    items: ((targetItem) => {
                        var i, j, k;
                        var ret = [];
                        var dsrvRecordings2 = hx.svc.RECORDINGS;
                        var schedulelist = dsrvRecordings2.getScheduledList();
                        for (i = 0; i < schedulelist.length; i += 1) {
                            if (schedulelist[i].getGroupCRIDs() && targetItem.getGroupCRIDs()) {
                                if (targetItem.getGroupCRIDs().length > 0 && schedulelist[i].getGroupCRIDs().length > 0) {
                                    for (j = 0; j < targetItem.getGroupCRIDs().length; j += 1) {
                                        for (k = 0 ; k < schedulelist[i].getGroupCRIDs().length ; k++) {
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
                        hx.log('iplate', 'finished removeSeriesScheduledRecording');
                    }
                });
            }
        };
        var fnSelectRecordType = (aProgramme) => {
            var itemList = [];
            var fnMakeMenuItem = (aName: string) => {
                itemList.push({
                    _one: aName,
                    _programme: aProgramme
                })
            };
            fnMakeMenuItem(hx.l('LOC_TIMERREC_BTN1_ID'));
            fnMakeMenuItem(hx.l('LOC_SERIESREC_BTN_ID'));
            hx.msg('recordingConflict', {
                'title' : hx.l('LOC_RECORD_RECOMMENDATION_ID'),
                'text': hx.l('LOC_GUIDE_RECORD_SELECT_ID'),
                'list_arr' : itemList,
                'dia_class' : 'osd',
                'auto_close' : true,
                'timeout' : 30000,
                'callback_fn' : (_status, _data) => {
                    if (_status === 'stop') {
                        var eType = __MODEL__.eTScheduledFactoryType;
                        var factoryType = eType.ERECORDING;
                        if(_data._one === hx.l('LOC_TIMERREC_BTN1_ID')) {
                            factoryType = eType.ERECORDING;
                        } else if(_data._one === hx.l('LOC_SERIESREC_BTN_ID')) {
                            factoryType = eType.ESERIES_RECORDING;
                        } else {
                            return;
                        }
                        if (_data && _data._programme) {
                            fnDoReservation(_data._programme, factoryType);
                        }
                    }
                }
            });
        };
        var fnDoReservation = (data: __PROVIDER__.CProgrammeProvider, aFactoryType?) => {
            var efactoryType = aFactoryType || __MODEL__.eTScheduledFactoryType.ERECORDING;
            var scheduledList = hx.svc.RECORDINGS.getScheduledList();
            hx.log('iplate', 'doReservation()');
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
            var _fnRecord = function(aProgramme: Programme) {
                hx.svc.RECORDINGS.record({
                    programme: aProgramme,
                    factoryType: efactoryType,
                    finished: function () {
                        hx.log('iplate', 'finished record');
                    }
                });
            };
            var linkage: ifFreeviewLinkage[] = JSON.parse(data.pluginObject.getField("freeview/linkage"));
            if (linkage.length) {
                var channel = hx.svc.CH_LIST.getChannel({
                    ccid: linkage[0].channel,
                    group: 'ALL'
                });
                if (channel.result) {
                    hx.svc.EPG.findProgrammes({
                        channel: channel.channel,
                        clause: 'programmeID == ' + linkage[0].event,
                        cb: (aResult,b,c) => {
                            var programmes: __PROVIDER__.CProgrammeProvider[] = aResult[0];
                            if (programmes.length) {
                                var hdProgramme = programmes[0];
                                fnSelectSimulcast({
                                    hdProgramme: hdProgramme,
                                    sdProgramme: data,
                                    fnRecorder: _fnRecord
                                });
                            } else {
                                hx.log('iplate', "linkaged detected but can't find HD programme");
                                _fnRecord(data.pluginObject);
                            }
                        }
                    })
                }
            } else {
                _fnRecord(data.pluginObject);
            }
        };
        var fnSelectSimulcast = function(aData) {
            var hdProgramme: __PROVIDER__.CProgrammeProvider = aData.hdProgramme;
            var sdProgramme: __PROVIDER__.CProgrammeProvider = aData.sdProgramme;
            var fnRecorder = aData.fnRecorder;
            var dlg = prism.createHDAlternativeButtonDlg(null, [
                {
                    text: hdProgramme.getName(),
                    fn: function() {
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
                onClosed: () => {
                    that.setKeyHookingControl(dlg);
                }
            });
        };
        if (aItem.hasSchedule() || aItem.hasReminder()) {
            fnCancelReservation(aItem);
        } else if (aItem.hasSeriesSchedule()) {
            fnCancelSeriesReservation(aItem);
        } else {
            if (aItem.getGroupCRIDs() && aItem.getGroupCRIDs().length) {
                fnSelectRecordType(aItem);
            } else {
                fnDoReservation(aItem);
            }
        }
    }

    /* related sugar message */
    _on_alKey(param) {
        var that = this;
        var keyCode: number = param.alKey;
        var keyRepeat = param.alRepeat;
        var consumed: boolean = false;

        that.resetExpireTimer();
        switch(keyCode) {
            case hx.key.KEY_BACK:
            case hx.key.KEY_ESCAPE:
                that.hide();
                consumed = true;
                break;
            case hx.key.KEY_CHANNEL_UP:
            case hx.key.KEY_CHANNEL_DOWN:
                that.hide();
                break;
            case hx.key.KEY_UP:
                that.keyRepeat = !!keyRepeat;
                try {
                    that.changePrevChannel();
                } catch(e) {}
                consumed = true;
                break;
            case hx.key.KEY_DOWN:
                that.keyRepeat = !!keyRepeat;
                try {
                    that.changeNextChannel();
                } catch(e) {}
                consumed = true;
                break;
            case hx.key.KEY_LEFT:
                if (that.performanceTimer !== -1) {
                    clearTimeout(that.performanceTimer);
                    that.performanceTimer = -1;
                }
                try {
                    that.changePrevEvent();
                } catch (e) {}
                consumed = true;
                break;
            case hx.key.KEY_RIGHT:
                if (that.performanceTimer !== -1) {
                    clearTimeout(that.performanceTimer);
                    that.performanceTimer = -1;
                }
                try {
                    that.changeNextEvent();
                } catch (e) {}
                consumed = true;
                break;
            case hx.key.KEY_MENU:
                that.hide();
                break;
            case hx.key.KEY_STOP:
                if (that.state === CPgIPlate.STATE.FORWARD) {
                    if (that.eventIndex === that.LIVE_INDEX) {
                        that.cancelRecording();
                    }
                } else {
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgNotAvailable?show'
                    });
                }
                consumed = true;
                break;
            case hx.key.KEY_RECORD:
                if (that.state === CPgIPlate.STATE.FORWARD) {
                    if (that.eventIndex === that.LIVE_INDEX) {
                        that.startRecording();
                    } else {
                        var showingEvent = that.forwardEvents[that.eventIndex];
                        if (showingEvent) {
                            that.requestReservation(showingEvent);
                        }
                    }
                } else {
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgNotAvailable?show'
                    });
                }
                consumed = true;
                break;
            case hx.key.KEY_BLUE:
                that.hide();
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgInfoHub?channelList=toggle'
                });
                consumed = true;
                break;
            case hx.key.KEY_OK:
                //if (that.state === CPgIPlate.STATE.FORWARD) {
                //    if (that.eventIndex === that.LIVE_INDEX) {
                //        var ret = that.setChannel();
                //        if (ret === true) {
                //            that.hide();
                //        }
                //    } else {
                //        that.showDetailInfo();
                //    }
                //} else {
                    that.showDetailInfo();
                //}
                consumed = true;
                break;
            case hx.key.KEY_PLAY:
            case hx.key.KEY_PAUSE:
            case hx.key.KEY_PLAY_SPEED_UP:
            case hx.key.KEY_PLAY_SPEED_DOWN:
            case hx.key.KEY_REWIND:
            case hx.key.KEY_FAST_FWD:
            case hx.key.KEY_TVRADIO_TOGGLE:
            case hx.key.KEY_OPTION:
            case hx.key.KEY_RED:
            case hx.key.KEY_YELLOW:
            case hx.key.KEY_GREEN:
                that.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgNotAvailable?show'
                });
                consumed = true;
                break;
            default:
                break;
        }

        return consumed;
    }

    _on_alEndOfRepeatKey(param) {
        var that = this;
        var kc = param.alKey;

        switch (kc) {
            case hx.key.KEY_UP:
            case hx.key.KEY_DOWN:
                if (this.keyRepeat) {
                    var startTime, endTime;
                    var dsrvTsr = hx.svc.TSR;
                    var dsrvAvCtrl = hx.svc.AV_CTRL;

                    this.keyRepeat = false;

                    startTime = Math.floor(that.now.getTime() / 1000);
                    if (dsrvTsr.isTsrOngoing() === true) {
                        var VBChannel = dsrvAvCtrl.getVideoBroadcastCurrentChannel();
                        if (!!VBChannel && VBChannel.ccid === that.showingChannel.channel.ccid) {
                            var TSRstartTime = dsrvTsr.getStartTime();
                            var TSRplaybackOffset = dsrvTsr.getPlaybackOffset();
                            hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
                            startTime = TSRstartTime + TSRplaybackOffset;
                        }
                    }
                    endTime = startTime + 3600 * 12;
                    ((reqChannel) => {
                        that.searchEPG(reqChannel, startTime, endTime).then((param:any) => {
                            if (that.state === CPgIPlate.STATE.FORWARD) {
                                if (reqChannel && that.showingChannel && that.showingChannel.channel) {
                                    if (reqChannel.sid === that.showingChannel.channel.sid) {
                                        that.setForwardEvents(param.result[0]);
                                        if (that.forwardEvents) {
                                            that.updateUIEventInfo(that.forwardEvents[that.LIVE_INDEX]);
                                        } else {
                                            that.updateUIEventInfo(null);
                                        }
                                        param.mergeMDS();
                                    }
                                }
                            }
                        }, () => {
                            hx.log('error', '[page_iplate.js][changeNextChannel] searchEPG Fail!!');
                            if (that.state === CPgIPlate.STATE.FORWARD) {
                                if (reqChannel && that.showingChannel && that.showingChannel.channel) {
                                    if (reqChannel.sid === that.showingChannel.channel.sid) {
                                        that.setForwardEvents([]);
                                        that.updateUIEventInfo(null);
                                    }
                                }
                            }
                        });
                    })(that.showingChannel.channel);

                    that.resetEpgQueryTimer();
                    return true;
                }
                break;
            default:
                break;
        }
        return false;
    }

    _on_alMessageToPage(param) {
        var that = this;
        var query: any = param.query;

        hx.log('iplate', '[_on_alMessageToPage]');

        if (query === 'dcn') {
            return;
        } else if (query === 'show') {
            that.show();
        } else if (query === 'hide') {
            that.hide();
        } else if (query === 'backToLive') {// from programme detail
            that.hide();
        }
    }
}

hx.al.CPgIPlate = CPgIPlate;
export = CPgIPlate;
