// file name : pages/tvguide/cGuideBackward.js
/*jslint sloppy:true, nomen:true, vars:true*/
///<reference path="../../def/sugar.d.ts" />
///<reference path="../../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />
///<reference path="../../layer_domain/service/ip_epg/adapter.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/tvguide/cmpt_guideview", "pages/tvguide/cmpt_guidequicknavi", "pages/tvguide/cmpt_guidedcn", "pages/util/provider"], function (require, exports, __VIEW__, __QUICKNAVI__, __DCN__, __PROVIDER__) {
    /*
     * Timeline
     */
    var cTimeline = (function () {
        function cTimeline(aParam) {
            this._HOUR = 3600;
            this._DAY = 86400;
            this._PAGE_DURATION = 7200;
            this._PIXEL_PER_HOUR = 480; // + 52; // 52 is correction value
            this.init(aParam);
        }
        cTimeline.prototype.init = function (aParam) {
            var now = new Date();
            hx.log('guideBackward', 'cguideBackward-TIMELINE-init()');
            now.setMinutes(now.getMinutes() < 30 ? 0 : 30);
            now.setSeconds(0, 0);
            this.startTime = now.toSeconds() - this._PAGE_DURATION;
            this.endTime = this.startTime + this._PAGE_DURATION;
            this.offset = 0;
            this.$ = null;
            this.animation = (aParam && aParam.animation) ? aParam.animation : {};
        };
        cTimeline.prototype.setTimeAndUpdate = function (aStartTime) {
            var offset = 0;
            hx.log('guideBackward', 'cguideBackward-TIMELINE-setTimeAndUpdate()');
            this.startTime = aStartTime;
            this.timeUpdate(offset);
        };
        cTimeline.prototype.timeUpdate = function (aOffset) {
            hx.log('guideBackward', 'cguideBackward-TIMELINE-timeUpdate()');
            var now = new Date().getTime();
            this.offset = aOffset;
            this.startTime = this.startTime + this.offset;
            this.endTime = this.startTime + this._PAGE_DURATION;
            //if (this.endTime * 1000 < now - (7 * this._DAY * 1000)) {
            //    hx.msg('response', {
            //        text : hx.l('LOC_MIGHT_BE_NO_EPG_MSG_ID'),
            //        auto_close : true,
            //        close_time : 3000
            //    });
            //}
        };
        cTimeline.prototype.draw = function () {
            hx.log('guideBackward', 'cguideBackward-TIMELINE-draw()');
            if (this.$ === null) {
                this.$ = $('#tvg #timeline .backward .wrap_time');
            }
            this._drawTimeline();
            this._updateTimelineDate();
        };
        cTimeline.prototype.clear = function () {
            var timeline, $timeline = $('#tvg #timeline .backward .wrap_time');
            hx.log('guideBackward', 'cguideBackward-TIMELINE-clear()');
            timeline = $timeline.children();
            timeline.eq(0).html('');
            timeline.eq(0).css('left', '0px');
            timeline.eq(1).html('');
            timeline.eq(1).css('left', '532px');
            $timeline.css('left', '0px');
        };
        cTimeline.prototype.hide = function () {
            var $timeline = $('#tvg #timeline .backward .wrap_time');
            hx.log('guideBackward', 'cguideBackward-TIMELINE-hide()');
            $timeline.hide();
        };
        cTimeline.prototype.show = function () {
            var $timeline = $('#tvg #timeline .backward .wrap_time');
            hx.log('guideBackward', 'cguideBackward-TIMELINE-show()');
            $timeline.show();
        };
        cTimeline.prototype.timeUpdateAndDraw = function (aOffset) {
            hx.log('guideBackward', 'cguideBackward-TIMELINE-timeUpdateAndDraw()');
            this.timeUpdate(aOffset);
            this.draw();
        };
        cTimeline.prototype.doSlide = function (aParam) {
            var _this = this;
            var start, end, startTime, endTime, left, offset;
            var base, $first, $second;
            var bAnimation = aParam.bAnimation;
            hx.log("guideBackward", hx.util.format('+++++++ ####### doSlide[Start] this.animation.running = {0} ####### +++++++', this.animation.running));
            var anim = function () {
                if (_this.animation.running) {
                    hx.log("guideBackward", '+++++++ ####### anim return; ####### +++++++');
                    return;
                }
                if (!_this.$) {
                    hx.log("guideBackward", '+++++++ ####### !this.$ return; ####### +++++++');
                    return;
                }
                _this.animation.running = true;
                _this.animation.state = 'sliding';
                base = _this.$.position().left;
                $first = _this.$.find('.first');
                $second = _this.$.find('.second');
                _this.timeUpdate(offset);
                //this._updateCurrentPositionBar();
                startTime = new Date((_this.startTime + _this._HOUR) * 1000);
                endTime = new Date((_this.startTime + (2 * _this._HOUR)) * 1000);
                if (offset >= 0) {
                    left = base - (_this._PIXEL_PER_HOUR * 2);
                    start = $second.position().left + _this._PIXEL_PER_HOUR;
                    end = start + _this._PIXEL_PER_HOUR;
                    _this.$.append($('<li>', {
                        'class': 'time first',
                        'html': startTime.format('HH:MM')
                    }).css('left', start + 'px')).append($('<li>', {
                        'class': 'time second',
                        'html': endTime.format('HH:MM')
                    }).css('left', end + 'px'));
                }
                else {
                    left = base + (_this._PIXEL_PER_HOUR * 2);
                    end = $first.position().left - _this._PIXEL_PER_HOUR;
                    start = end - _this._PIXEL_PER_HOUR;
                    _this.$.prepend($('<li>', {
                        'class': 'time second',
                        'html': endTime.format('HH:MM')
                    }).css('left', end + 'px')).prepend($('<li>', {
                        'class': 'time first',
                        'html': startTime.format('HH:MM')
                    }).css('left', start + 'px'));
                }
                $first.addClass('old');
                $second.addClass('old');
                hx.log("guideBackward", hx.util.format('+++++++ ####### doSlide left = {0} ####### +++++++', left));
                hx.log("guideBackward", hx.util.format('+++++++ ####### doSlide this.$.position().left = {0} ####### +++++++', _this.$.position().left));
                _this.$.addClass('move');
                _this.$.anim({
                    'left': left + 'px'
                }, function () {
                    _this.animation.running = false;
                    _this.animation.state = 'idle';
                    _this.$.removeClass('move');
                    _this.$.find('li.old').remove();
                    hx.log("guideBackward", '+++++++ ####### doSlide Animation Finished; ####### +++++++');
                    if (_this.onScrollStarted) {
                        _this.onScrollStarted();
                    }
                    if (_this._animParam) {
                        _this.doSlide(_this._animParam);
                        _this._animParam = null;
                    }
                    else {
                        if (_this.onScrollStopped) {
                            _this.onScrollStopped();
                        }
                    }
                });
            };
            if (bAnimation && this.animation.use) {
                if (this.animation.running) {
                    this._animParam = aParam;
                }
                else {
                    offset = aParam.offset;
                    this.onScrollStarted = aParam.onScrollStarted;
                    this.onScrollStopped = aParam.onScrollStopped;
                    anim();
                }
            }
            else {
                offset = aParam.offset;
                this.timeUpdateAndDraw(offset);
                if (aParam.onScrollStarted) {
                    aParam.onScrollStarted();
                }
                if (aParam.onScrollStopped) {
                    aParam.onScrollStopped();
                }
            }
            hx.log("guideBackward", hx.util.format('+++++++ ####### doSlide[End] this.animation.running = {0} ####### +++++++', this.animation.running));
            return true;
        };
        cTimeline.prototype._drawTimeline = function () {
            var time, timeline, $timeline = $('#tvg #timeline .backward .wrap_time');
            hx.log('guideBackward', 'cguideBackward-TIMELINE-_drawTimeline()');
            $timeline.position().left = 0;
            time = new Date(this.startTime * 1000);
            time.setMinutes(time.getMinutes() + 60);
            timeline = $timeline.children();
            timeline.eq(0).html(time.format('HH:MM'));
            time.setMinutes(time.getMinutes() + 60);
            timeline.eq(1).html(time.format('HH:MM'));
        };
        cTimeline.prototype._updateTimelineDate = function () {
            hx.log('guideNext', 'cGuideNext-TIMELINE-_updateTimelineDate()');
            var now = new Date();
            var $timelineDate = $('#tvg .backward.indicator .date');
            $timelineDate.text(dateFormat(this.startTime * 1000, 'ddd dd mmm'));
        };
        cTimeline.prototype._getCurrentTime = function () {
            var now = new Date();
            hx.log('guideBackward', 'cguideBackward-TIMELINE-_getCurrentTime()');
            return now.toSeconds();
        };
        cTimeline.prototype._getBackwardGuideTime = function () {
            return (this.endTime + this._PAGE_DURATION);
        };
        cTimeline.prototype._getDateOfStartTime = function () {
            var tempTime, day;
            tempTime = new Date(this.startTime * 1000);
            day = tempTime.format('ddd dd.mm');
            return day;
        };
        cTimeline.prototype._getCssStyleLeft = function (aTime) {
            hx.log('guideBackward', 'cguideBackward-TIMELINE-_getCssStyleLeft()');
            return parseInt(this._PIXEL_PER_HOUR * ((aTime - this.startTime) / this._HOUR) + "", 10) + 'px';
        };
        return cTimeline;
    })();
    var cGuideBackward = (function (_super) {
        __extends(cGuideBackward, _super);
        function cGuideBackward(param) {
            _super.call(this, $('<article>', {
                'id': 'backward',
                'class': 'focusable',
                'style': 'opacity:0;',
                'name': 'guideBackward'
            }), 'focusable', 'on', param);
            this._lastUsedDateAsDateIndicator = null;
            this.IntervalKeyDown = 850;
            this.PADDING_TIME = 1800;
            this.pixelPerMin = 8;
            this.currTime = 0;
            this._stayedFocusTimerId = null;
            this._isCleared = false;
            this.TScheduledFactoryType = {
                ERECORDING: 0,
                EREMIND: 1,
                EHTTP_LIVE_STREAMING: 4,
                ESERIES_RECORDING: 10
            };
            this.$ = this._$;
            //hx.logadd('guideBackward');
            hx.log('guideBackward', 'cguideBackward-init()');
            var box = document.createElement('ul');
            box.className = 'gItem';
            this._$dateIndicator = $('<div>', {
                'id': 'changeDateLayout',
                'style': 'opacity:0;'
            }).append($('<div>', {
                'id': 'changeDateText'
            }));
            this.box = box;
            this._$tvg = $('#tvg');
            // added 'usePast' property for support backwardGuide
            param.usePast = true;
            this.timeline = new cTimeline(param);
            this._lastUsedDateAsDateIndicator = this.timeline._getDateOfStartTime();
            //this.opt = document.createElement('div');
            //this.opt.innerHTML = hx.l('LOC_OPT_GENRE_ID');
            //this.opt.id = 'genreOpt';
            //this.$.append(this.opt);
            this.quickNavi = new __QUICKNAVI__(param);
            this.dcn = new __DCN__();
            if (hx.config.tvGuide2ndScenario) {
                this.MAX_FOCUS_LEFT = 552;
            }
            else {
                this.MAX_FOCUS_LEFT = 360;
            }
            /* add child components */
            this.add(this.dcn);
            this.add(this.quickNavi);
            this.setName('guideBackward');
            this.create();
        }
        cGuideBackward.prototype.create = function () {
            var i, ROW_COUNT, top, li, fragment = document.createDocumentFragment();
            hx.log('guideBackward', 'cguideBackward-create()');
            for (i = 0, ROW_COUNT = this.ROW_COUNT; i < ROW_COUNT; i += 1) {
                top = i * this.ROW_HEIGHT;
                li = document.createElement('li');
                li.setAttribute('row', i);
                li.setAttribute('col', 0);
                li.style.top = top + 'px';
                li.style.height = this.ROW_HEIGHT + 'px';
                this.rowList.push({
                    'element': [li],
                    'top': top
                });
                fragment.appendChild(li);
            }
            /* this.box : <ul> */
            this.box.appendChild(fragment);
            this.$[0].appendChild(this.box);
        };
        cGuideBackward.prototype.destroy = function () {
            hx.log('cguideBackward', 'cguideBackward-destroy()');
            if (this._stayedFocusTimerId !== null) {
                clearTimeout(this._stayedFocusTimerId);
                this._stayedFocusTimerId = null;
            }
            if (this.$) {
                this.$.remove();
            }
            if (this.timeline) {
                delete this.timeline;
                this.timeline = null;
            }
            if (this.opt) {
                delete this.opt;
                this.opt = null;
            }
            if (this.quickNavi) {
                this.quickNavi.$.remove();
                delete this.quickNavi;
                this.quickNavi = null;
            }
            _super.prototype.destroy.call(this);
        };
        cGuideBackward.prototype._isRunningAnimationOfViewChange = function () {
            var aniTimeout = this.animation.aniTimeoutID, isRunning = false;
            hx.log('guideBackward', 'cGuideBackward-_isRunningAnimationOfViewChange()');
            if (aniTimeout.gItem !== null || aniTimeout.timeline !== null || aniTimeout.backward !== null) {
                hx.log("warning", hx.util.format('animation of "onAirView" is running. {0}', aniTimeout.gItem));
                isRunning = true;
            }
            if (true === this.animation.running) {
                isRunning = true;
            }
            return isRunning;
        };
        cGuideBackward.prototype._on_alKey = function (aParam) {
            var alKey = aParam.alKey, useAnimation = this.animation.use, isRunning = this.animation.running, isConsumed = false;
            var currentTime;
            hx.log('guideBackward', 'cguideBackward-_on_alKey()');
            hx.log('guideBackward', hx.util.format('state : {0}', this.state));
            if (ENV.op === ENV.listOP.UKDTT) {
                if (this._genreDlg || (this.state === this.EVIEW_STATE.EBUSY)) {
                    return _super.prototype._on_alKey.call(this, aParam);
                }
            }
            else {
                if (this.state === this.EVIEW_STATE.EBUSY) {
                    return _super.prototype._on_alKey.call(this, aParam);
                }
            }
            switch (alKey) {
                case hx.key.KEY_UP:
                case hx.key.KEY_DOWN:
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                    this._swapLine = this.TSwapLineType.ENONE;
                    break;
                case hx.key.KEY_LEFT:
                case hx.key.KEY_RIGHT:
                case hx.key.KEY_FAST_FWD:
                case hx.key.KEY_REWIND:
                case hx.key.KEY_PLAY_SPEED_UP:
                case hx.key.KEY_PLAY_SPEED_DOWN:
                    this._swapLine = this.TSwapLineType.ENONE;
                    if (this._isRunningAnimationOfViewChange()) {
                        isConsumed = true;
                    }
                    else {
                        isConsumed = true;
                        if (aParam.alRepeat === 0) {
                            //this._setStayedFocus();
                            this._setFocusCssStyle('', 'on');
                            if (!this.doNavi(aParam)) {
                                isConsumed = this._trySlide(aParam);
                            }
                        }
                        else if (aParam.alRepeat > 2) {
                            this.showQuickNavi();
                        }
                    }
                    break;
            }
            return (isConsumed || _super.prototype._on_alKey.call(this, aParam));
        };
        cGuideBackward.prototype.getProgrammes = function (aParam) {
            var _this = this;
            var channels = this.channelList.channels;
            var timeline = this.timeline;
            var lChannels = [];
            //var tz = new Date().getTimezoneOffset() * 60;
            hx.log('guideBackward', 'cGuideBackward-_parse()');
            if (this._isCleared) {
                this._swapLine = this.TSwapLineType.ENONE;
            }
            if (this._swapLine === this.TSwapLineType.ENONE) {
                lChannels = channels;
            }
            else if (this._swapLine === this.TSwapLineType.EUP) {
                lChannels = [channels[0]];
            }
            else if (this._swapLine === this.TSwapLineType.EDOWN) {
                lChannels = [channels[this.ROW_COUNT - 1]];
            }
            var reqSwapLine = this._swapLine || this.TSwapLineType.ENONE;
            var callback = function (xmls) {
                if (!_this._receiveCallback || !_this.channelList || _this.state === _this.EVIEW_STATE.EHIDE) {
                    hx.log("warning", hx.util.format('You have arrived too late. {0} was already gone.', _this.name));
                    return;
                }
                _this._swapLine = reqSwapLine;
                var svdIds = lChannels.map(function (channel) {
                    return channel.getServiceId();
                });
                var noProgramme = hx.l('LOC_NO_PROGRAMME_INFORMATION_ID');
                var result = [];
                var info;
                var crids;
                var schedules;
                var serviceId;
                var index;
                xmls.forEach(function (info) {
                    //info = hx.svc.IP_EPG.parse.xml.schedule(xml);
                    schedules = info.schedules;
                    crids = info.crids;
                    serviceId = info.serviceId;
                    index = svdIds.indexOf(parseInt(serviceId));
                    if (index !== -1) {
                        hx.log('info', 'serviceId: ' + serviceId);
                        var programmes = [];
                        for (var i = 0, len = schedules.length, schedule; i < len; i++) {
                            schedule = schedules[i];
                            var crid = schedule.querySelector('Program').getAttribute('crid');
                            var data = crids[crid];
                            var startTime = new Date(schedule.querySelector('PublishedStartTime').textContent).valueOf() / 1000;
                            var duration = hx.svc.IP_EPG.parse.util.duration(schedule.querySelector('PublishedDuration').textContent);
                            if (startTime < _this.timeline.endTime && (startTime + duration) > _this.timeline.startTime) {
                                data.schedule = schedule;
                                data.available = false;
                                var programme = new __PROVIDER__.CProgrammeProvider({
                                    mdsData: data,
                                    channel: _this.channelList.channels[index].getPluginObject(),
                                    launchLocation: 'epg'
                                });
                                //hx.log('info', '= joined');
                                //hx.log('info', '= title: ' + title);
                                //hx.log('info', '= start time: ' + new Date(startTime * 1000 ));
                                //hx.log('info', '= duration: ' + schedule.querySelector('PublishedDuration').textContent);
                                programmes.push(programme);
                            }
                            else {
                            }
                        }
                        result[index] = result[index] || [];
                        result[index] = result[index].concat(programmes);
                    }
                });
                var i = 0, len = lChannels.length;
                for (i = 0; i < len; i += 1) {
                    var columns = result[i];
                    if (!columns || columns.length === 0) {
                        columns = [(new __PROVIDER__.CProgrammeProvider(null)).setProgrammeName(noProgramme).setChannelInformation(lChannels[i]).setStartTime(_this.timeline.startTime).setEndTime(_this.timeline.endTime + _this.PADDING_TIME)];
                    }
                    columns.sort(function (prev, after) {
                        return prev.getStartTime() - after.getStartTime() >= 0 ? 1 : -1;
                    });
                    if (columns[0].getStartTime() > (_this.timeline.startTime + 50)) {
                        columns.unshift((new __PROVIDER__.CProgrammeProvider(null)).setProgrammeName(noProgramme).setChannelInformation(lChannels[i]).setStartTime(_this.timeline.startTime).setEndTime(columns[0].getStartTime()));
                    }
                    if ((columns[columns.length - 1].getEndTime() + 50) < _this.timeline.endTime) {
                        columns.push((new __PROVIDER__.CProgrammeProvider(null)).setProgrammeName(noProgramme).setChannelInformation(lChannels[i]).setStartTime(columns[columns.length - 1].getEndTime()).setEndTime(_this.timeline.endTime + _this.PADDING_TIME));
                    }
                    result[i] = columns;
                }
                var toDrawing = result;
                if (_this._swapLine === _this.TSwapLineType.ENONE) {
                    _this.programmes = result;
                }
                else if (_this._swapLine === _this.TSwapLineType.EUP) {
                    _this.programmes.pop();
                    _this.programmes.unshift(result[0]);
                    toDrawing = [result[0]];
                }
                else if (_this._swapLine === _this.TSwapLineType.EDOWN) {
                    _this.programmes.shift();
                    _this.programmes.push(result[channels.length - 1]);
                    toDrawing = [result[channels.length - 1]];
                }
                _this.draw(toDrawing, aParam.updateFlag, channels);
            };
            var minus7day = this.quickNavi.getBackwardLimitTime();
            var timeBase = this.timeline.endTime;
            var hours = 6;
            if (timeBase < minus7day) {
                //hx.log('debug', '> no request');
                callback([]);
            }
            else {
                //hx.log('debug', '> timeline endTime time: ' + new Date(timeBase * 1000).toUTCString());
                timeBase = Math.max(timeBase, minus7day);
                timeBase *= 1000;
                //hx.log('debug', '> request time: ' + new Date(timeBase).toUTCString());
                var queryList = [];
                var xmls = [];
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
                lChannels.forEach(function (channel) {
                    var serviceId = channel.getServiceId();
                    addScheduleQueryToQueryList(serviceId, timeBase, hours);
                    addScheduleQueryToQueryList(serviceId, timeBase - (hours * 60 * 60 * 1000), hours);
                });
                hx.log('debug', '[BACKWARD][getProgrammes] queryList-count : ' + queryList.length);
                hx.svc.IP_EPG.getScheduleList(queryList).then(function () {
                    hx.log('debug', '[BACKWARD][getProgrammes] Success! xmls-count : ' + xmls.length);
                    callback(xmls);
                }, function () {
                    hx.log('debug', '[BACKWARD][getProgrammes] Fail? xmls-count : ' + xmls.length);
                    callback(xmls);
                });
            }
        };
        cGuideBackward.prototype.draw = function (aData, aUpdateFlag, channels) {
            var _this = this;
            var i, j, k, m, row, rowLen, colLen, li, date, temp, updateRow;
            var programme, programmes, programmeCssStyle, dispGenre = false;
            var isEnablePopUPFocus = false;
            var $indicator, base, complement;
            var bDontUpdateKeyBuilder = false;
            var idxOfTop = 0, idxOfBottom = this.ROW_COUNT - 1;
            var heightOfBottom = this.ROW_HEIGHT * idxOfBottom;
            hx.log('guideBackward', 'cguideBackward-draw()');
            if (this.animation.running && (this.animation.state === 'scrolling' || this.animation.state === 'sliding')) {
                hx.log("guideBackward", hx.util.format('+++++++ ####### draw this.animation.running = {0} state = {1} ####### +++++++', this.animation.running, this.animation.state));
                return;
            }
            if (this._curGenre !== '') {
                dispGenre = true;
            }
            date = new Date();
            this.currTime = date.toSeconds();
            base = -$(this.box).position().left + 26; // will be fixed by something
            complement = 0;
            var noProgramme = hx.l('LOC_NO_PROGRAMME_INFORMATION_ID');
            var fnDrawRow = function (channel, programmes, currentRow) {
                colLen = programmes.length;
                var fnUpdateAvailability = function (aProg, aEl) {
                    var availabilityImg = '';
                    if (aProg.getAvailability() === 2 /* EAvailable */) {
                        availabilityImg = "images/205_REC_Play.png";
                    }
                    else if (aProg.getAvailability() === 1 /* EComingSoon */) {
                        availabilityImg = "images/614_EPG_Comingsoon.png";
                    }
                    else {
                    }
                    if (availabilityImg) {
                        var img = aEl.querySelector('.availability');
                        var exist = !!img;
                        if (!exist) {
                            img = document.createElement('img');
                            img.classList.add('availability');
                            img.src = availabilityImg;
                            img.style.marginRight = '3px';
                            img.style.position = 'relative';
                            img.style.top = '17px';
                            var em = aEl.querySelector('.title');
                            em.insertBefore(img, em.firstChild);
                        }
                    }
                };
                var fnUpdateHasRecording = function (aProg, aEl) {
                    var hasRecImg = '';
                    if (aProg.hasRecording()) {
                        hasRecImg = "images/205_REC_Recording_Watch.png";
                    }
                    if (hasRecImg) {
                        var img = aEl.querySelector('.recorded');
                        var exist = !!img;
                        if (!exist) {
                            img = document.createElement('img');
                            img.classList.add('recorded');
                            img.src = hasRecImg;
                            img.style.marginRight = '3px';
                            img.style.position = 'relative';
                            img.style.top = '17px';
                            var em = aEl.querySelector('.title');
                            em.insertBefore(img, em.firstChild);
                        }
                    }
                };
                if (colLen && channel.ccid) {
                    complement = 0;
                    for (j = 0; j < colLen; j += 1) {
                        programme = programmes[j];
                        _this._getProgrammeAttr(programme, i);
                        li = currentRow.element[j];
                        if (!li) {
                            li = document.createElement('li');
                            currentRow.element[j] = li;
                            // COMMENT < this.box : <ul> >
                            _this.box.appendChild(li);
                        }
                        programmeCssStyle = _this._getCssStyle(programme, base, currentRow.top, complement);
                        complement = programmeCssStyle.Complement;
                        li.style.left = programmeCssStyle.left;
                        li.style.width = programmeCssStyle.width;
                        li.style.height = _this.ROW_HEIGHT;
                        li.style.top = programmeCssStyle.top;
                        li.style.padding = programmeCssStyle.padding;
                        li.setAttribute('row', updateRow);
                        li.setAttribute('col', j);
                        li.className = _this._getClassName(programme, updateRow);
                        //if (programme.getPluginObject()) {
                        if (programme.getName() !== noProgramme) {
                            li.innerHTML = _this.getCellHTML(programme, false);
                            (function (aProg, aEl) {
                                setTimeout(function () {
                                    hx.svc.IP_EPG.getAIT(aProg.getTemplateAitUrl()).then(function (xml) {
                                        aProg.checkTemplateAIT(xml);
                                        fnUpdateAvailability(aProg, aEl);
                                        fnUpdateHasRecording(aProg, aEl);
                                    }, function (err) {
                                        fnUpdateHasRecording(aProg, aEl);
                                    });
                                }, 10);
                            })(programme, li);
                        }
                        else {
                            li.className = li.className.concat(' noProgramme');
                            li.innerHTML = _this.getNoProgrammeInformationCellHTML(programme, false);
                            li.style.textIndent = "620px";
                        }
                        if (dispGenre) {
                            _this._genre(programme, $(li));
                        }
                        hx.log("guideBackward", hx.util.format('[{0}][{1}] name:{2}  l:{3},w:{4},t{5},p{6}, innerHtml:{7}', i, j, programme.name, li.style.left, li.style.width, li.style.top, li.style.padding, li.innerHTML));
                    }
                }
                else {
                    li = currentRow.element[0];
                    li.innerHTML = '';
                }
            };
            switch (this._swapLine) {
                case this.TSwapLineType.EUP:
                    temp = this.rowList.splice(this.ROW_COUNT - 1, 1);
                    this.rowList.splice(0, 0, temp[0]);
                    for (k = this.box.childNodes.length - 1; k >= 0; k -= 1) {
                        if (parseInt(this.box.childNodes[k].getAttribute('row'), 10) === idxOfBottom) {
                            this.box.removeChild(this.box.childNodes.item(k));
                        }
                    }
                    for (k = 0; k < this.rowList.length; k += 1) {
                        if (this.rowList[k].top === heightOfBottom) {
                            this.rowList[k].element = [];
                            row = this.rowList[k];
                            updateRow = 0;
                            row.top = 0;
                        }
                        else {
                            this.rowList[k].top += this.ROW_HEIGHT;
                            for (m = 0; m < this.rowList[k].element.length; m += 1) {
                                this.rowList[k].element[m].setAttribute('row', parseInt(this.rowList[k].element[m].getAttribute('row'), 10) + 1);
                                this.rowList[k].element[m].style.top = (this.ROW_HEIGHT * parseInt(this.rowList[k].element[m].getAttribute('row'), 10)) + 'px';
                                if ($(this.rowList[k].element[m]).hasClass('onFocus')) {
                                    this.rowList[k].element[m].className = 'pInfo focusable on';
                                }
                                else {
                                    this.rowList[k].element[m].className = 'pInfo focusable';
                                }
                                if (this.rowList[k].top === heightOfBottom) {
                                    this.rowList[k].element[m].className = 'pInfo';
                                }
                            }
                        }
                    }
                    for (k = this._$focusable.length - 1; k >= 0; k -= 1) {
                        if (this._$focusable[k][hx.key.KEY_DOWN] === null) {
                            this._$focusable.splice(k, 1);
                        }
                    }
                    fnDrawRow(channels[idxOfTop], aData[0], row);
                    this._swapLine = this.TSwapLineType.ENONE;
                    break;
                case this.TSwapLineType.EDOWN:
                    temp = this.rowList.splice(0, 1);
                    this.rowList.push(temp[0]);
                    for (k = this.box.childNodes.length - 1; k >= 0; k -= 1) {
                        if (parseInt(this.box.childNodes[k].getAttribute('row'), 10) === 0) {
                            this.box.removeChild(this.box.childNodes.item(k));
                        }
                    }
                    for (k = 0; k < this.rowList.length; k += 1) {
                        if (this.rowList[k].top === 0) {
                            this.rowList[k].element = [];
                            row = this.rowList[k];
                            updateRow = idxOfBottom;
                            row.top = heightOfBottom;
                        }
                        else {
                            this.rowList[k].top -= this.ROW_HEIGHT;
                            for (m = 0; m < this.rowList[k].element.length; m += 1) {
                                this.rowList[k].element[m].setAttribute('row', parseInt(this.rowList[k].element[m].getAttribute('row'), 10) - 1);
                                this.rowList[k].element[m].style.top = (this.ROW_HEIGHT * parseInt(this.rowList[k].element[m].getAttribute('row'), 10)) + 'px';
                                if ($(this.rowList[k].element[m]).hasClass('onFocus')) {
                                    this.rowList[k].element[m].className = 'pInfo focusable on';
                                }
                                else {
                                    this.rowList[k].element[m].className = 'pInfo focusable';
                                }
                            }
                        }
                    }
                    for (k = this._$focusable.length - 1; k >= 0; k -= 1) {
                        if (this._$focusable[k][hx.key.KEY_UP] === null) {
                            this._$focusable.splice(k, 1);
                        }
                    }
                    fnDrawRow(channels[idxOfBottom], aData[0], row);
                    this._swapLine = this.TSwapLineType.ENONE;
                    break;
                case this.TSwapLineType.ENONE:
                    var channel = null;
                    for (i = 0, rowLen = aData.length; i < rowLen; i += 1) {
                        for (k = this.box.childNodes.length - 1; k >= 0; k -= 1) {
                            if (parseInt(this.box.childNodes[k].getAttribute('row'), 10) === i) {
                                this.box.removeChild(this.box.childNodes.item(k));
                            }
                        }
                        row = this.rowList[i];
                        channel = channels[i];
                        this.rowList[i].element = [];
                        updateRow = i;
                        fnDrawRow(channel, aData[i], row);
                    }
                    break;
            }
            if (this._swapLine === this.TSwapLineType.EUP) {
                for (k = 0; k < row.element.length; k += 1) {
                    this._$focusable.push(row.element[k]);
                }
                this.build(this.positionBuilder);
                this.doNavi({
                    alKey: hx.key.KEY_UP
                });
                bDontUpdateKeyBuilder = true;
            }
            else if (this._swapLine === this.TSwapLineType.EDOWN) {
                for (k = 0; k < row.element.length; k += 1) {
                    this._$focusable.push(row.element[k]);
                }
                this.build(this.positionBuilder);
                this.doNavi({
                    alKey: hx.key.KEY_DOWN
                });
                bDontUpdateKeyBuilder = true;
            }
            this.doDraw(isEnablePopUPFocus, aUpdateFlag, bDontUpdateKeyBuilder);
            if (aUpdateFlag) {
                hx.log('guideBackward', 'cGuideBackward-draw(), updateFlag!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!');
            }
            else {
                //this._setStayedFocus();
                this._setFocusCssStyle('', 'on');
            }
            this.$.css('opacity', 1);
            $indicator = $('#tvg #indicator .indi_text');
            $indicator[0].innerHTML = hx.l('LOC_NEXT_VIEW_ID');
            this._isCleared = false;
            hx.log("guideBackward", '+++++++ ####### draw FINISHED ####### +++++++');
        };
        cGuideBackward.prototype.clear = function () {
            var i, j, il, jl;
            hx.log('guideBackward', 'cGuideBackward-clear()');
            if (this._isCleared === false) {
                //TODO: ul class="gItem" display none first;
                var display = this.box.style.display;
                for (i = 0, il = this.ROW_COUNT; i < il; i += 1) {
                    for (j = 0, jl = this.rowList[i].element.length; j < jl; j += 1) {
                        this.rowList[i].element[j].innerHTML = '';
                        this.rowList[i].element[j].style = '';
                        this.rowList[i].element[j].className = '';
                    }
                }
                //this.box.style.display = display;
                this.focusInfo = {};
                this.doClear();
                this._isCleared = true;
            }
        };
        cGuideBackward.prototype.popFocusStyleLeft = function ($item) {
            var left, padding, style_left, left, padding;
            var backwardPadding = 60;
            var itemWidth = 0, itemStartX = 0, itemLeft = 0, itemPadding = 0, maxWidth = 0;
            hx.log('guideBackward', 'cGuideBackward-popFocusStyleLeft()');
            if (hx.config.tvGuide2ndScenario) {
                left = parseInt($item.css('left'), 10) + $(this.box).position().left + 20;
                padding = 0;
                style_left = (left < padding ? 0 : (left > this.MAX_FOCUS_LEFT ? this.MAX_FOCUS_LEFT : left - padding)) + 'px';
            }
            else {
                itemLeft = parseInt($item.css('left'), 10);
                itemPadding = parseInt($item.css('padding-left'), 10) > 0 ? 10 : 0;
                itemWidth = parseInt($item.css('width'), 10);
                itemStartX = itemWidth + itemLeft;
                // when backwardPadding is bigger than item's width.
                if (itemWidth <= backwardPadding) {
                    if (itemLeft > this.MAX_FOCUS_LEFT) {
                        style_left = this.MAX_FOCUS_LEFT + 'px';
                    }
                    else {
                        if (itemWidth < 10) {
                            itemPadding = itemPadding * 3;
                        }
                        style_left = itemLeft + itemPadding + itemWidth + 'px';
                    }
                }
                else {
                    if (itemLeft > itemStartX) {
                        style_left = itemStartX + 'px';
                    }
                    else {
                        itemLeft += backwardPadding;
                        if (itemLeft >= this.MAX_FOCUS_LEFT) {
                            style_left = this.MAX_FOCUS_LEFT + 'px';
                        }
                        else {
                            maxWidth = this.MAX_FOCUS_LEFT + itemLeft + itemPadding * 6;
                            //if( itemStartX > maxWidth ){
                            if (itemStartX > 780) {
                                style_left = this.MAX_FOCUS_LEFT + 'px';
                            }
                            else {
                                style_left = (itemLeft - itemPadding) + 'px';
                            }
                        }
                    }
                }
            }
            return style_left;
        };
        cGuideBackward.prototype.RecordingEvent = function (param, item) {
            var channels = this.channelList.channels;
            hx.log('guideBackward', 'cGuideBackward-RecordingEvent()');
            this.cellRenewal(false, item);
            this.channelList.checkTunes(channels);
        };
        cGuideBackward.prototype.cellRenewalEnd = function () {
            hx.log('guideBackward', 'cGuideBackward-cellRenewalEnd()');
            //this._drawPopupFocusPositionIndicator();
        };
        cGuideBackward.prototype.showQuickNavi = function () {
            hx.log('guideBackward', 'cGuideBackward-showQuickNavi()');
            this.quickNavi.config('backward');
            this.state = this.EVIEW_STATE.EBUSY;
            if (this._stayedFocusTimerId !== null) {
                clearTimeout(this._stayedFocusTimerId);
                this._stayedFocusTimerId = null;
            }
            //this.getFocus().css('opacity', 0);
            //this._setFocusCssStyle('on', 'onFocus');
            this.quickNavi.show(this.timeline.startTime);
        };
        cGuideBackward.prototype.hideQuickNavi = function () {
            hx.log('guideBackward', 'cGuideBackward-hideQuickNavi()');
            this.state = this.EVIEW_STATE.EIDLE;
            //this._setStayedFocus();
            this._setFocusCssStyle('', 'on');
        };
        cGuideBackward.prototype.quickNaviClicked = function (param) {
            var date = param.data;
            hx.log('guideBackward', 'cguideBackward-quickNaviClicked()');
            if (date) {
                this.timeline.setTimeAndUpdate(date.toSeconds());
                this.timeline.draw();
                this._lastUsedDateAsDateIndicator = this.timeline._getDateOfStartTime();
                this.initKey();
                this.freeMem(this.programmes);
                this.programmes = [];
                this.clear();
                this.getProgrammes({});
                this.onChangeDateIndication();
            }
            else {
                this._setFocusCssStyle('', 'on');
            }
            this.state = this.EVIEW_STATE.EIDLE;
        };
        cGuideBackward.prototype._genreHighlighted = function () {
            var i, l, j, li, jl, programmes, element, data = this.programmes;
            hx.log('guideBackward', 'cGuideBackward-_genreHighlighted()');
            for (i = 0, l = data.length; i < l; i += 1) {
                programmes = data[i];
                element = this.rowList[i];
                for (j = 0, jl = programmes.length; j < jl; j += 1) {
                    li = element.element[j];
                    this._genre(programmes[j], $(li));
                }
            }
        };
        cGuideBackward.prototype._parse = function (param) {
            var channels = this.channelList.channels, timeline = this.timeline;
            var lChannels = [];
            var bChanged = false;
            var index = 0;
            hx.log('guideBackward', 'cGuideBackward-_parse()');
            if (this._isCleared === true) {
                this._swapLine = this.TSwapLineType.ENONE;
            }
            switch (this._swapLine) {
                case this.TSwapLineType.ENONE:
                    lChannels = channels;
                    break;
                case this.TSwapLineType.EUP:
                    lChannels.push(channels[0]);
                    break;
                case this.TSwapLineType.EDOWN:
                    lChannels.push(channels[this.ROW_COUNT - 1]);
                    break;
            }
            if (this.isEmpty(param.count)) {
                param.count = lChannels.length;
            }
            lChannels = this.uiDataToSrv(lChannels);
            return {
                'filter': "ipepg",
                'isOnAir': false,
                'isBackward': true,
                'channel': lChannels,
                'startTime': timeline.startTime,
                'endTime': timeline.endTime,
                'info': param.info
            };
        };
        cGuideBackward.prototype.getDateString = function (dateAsTimeMs) {
            var date = new Date(dateAsTimeMs);
            return date.format("isoDateTime");
        };
        cGuideBackward.prototype._getProgrammeAttr = function (programme, row) {
            hx.log('guideBackward', 'cGuideBackward-_getProgrammeAttr()');
            programme.backwardView = true;
            programme.isCAS = this.channelList.casIDs[row] || '';
        };
        cGuideBackward.prototype._timeToPixel = function (t) {
            hx.log('guideBackward', 'cGuideBackward-_timeToPixel()');
            return (parseInt((t / 60) + "", 10) * this.pixelPerMin);
        };
        cGuideBackward.prototype._getCssStyle = function (programme, base, top, beforeComplement) {
            var timeline = this.timeline, startTime = timeline.startTime, endTime = timeline.endTime;
            var margin = 1, padding = 30, border = 4;
            var start, duration, left, width, style = {}, str, offset;
            hx.log('guideNext', 'cGuideNext-_getCssStyle()');
            if (programme.getStartTime() < startTime) {
                start = startTime;
            }
            else {
                start = programme.getStartTime();
                start -= beforeComplement;
                offset = start % 60;
                start = start - offset;
            }
            duration = programme.getEndTime() - start;
            // initialize for style's padding value
            left = (base + this._timeToPixel(start - startTime));
            style.padding = "";
            style.left = left + 'px';
            width = this._timeToPixel(duration) - border;
            if (width < 0) {
                width = 0;
            }
            if (duration < 300) {
                style.padding = '0';
                style.width = width + 'px';
            }
            else {
                if (left + width > this.VIEW_WIDTH) {
                    width = this.VIEW_WIDTH - left;
                }
                //style.width = (width - padding) + 'px';
                style.width = width + 'px';
            }
            style.top = top + 'px';
            style.Complement = duration % 60;
            return style;
        };
        cGuideBackward.prototype._getClassName = function (programme, row) {
            var isFocusable = false;
            hx.log('guideBackward', 'cguideBackward-_getClassName()');
            //if (row >= 0 && row < (this.ROW_COUNT - 1) && this.channelList.channels[row].ccid) {
            if (row >= 0 && row < this.ROW_COUNT && this.channelList.channels[row].ccid) {
                isFocusable = true;
            }
            return (isFocusable ? 'pInfo focusable' : 'pInfo');
        };
        cGuideBackward.prototype._genre = function (programme, $elem) {
            var i, length, genre, hasGenre = false;
            hx.log('guideBackward', 'cguideBackward-_genre()');
            if (!programme) {
                hx.log('guideBackward', 'cguideBackward-_genre() : !programme');
                return;
            }
            genre = programme.getGenre();
            if (genre && genre.length > 0) {
                for (i = 0, length = genre.length; i < length; i += 1) {
                    if (genre[i] === this._curGenre) {
                        hasGenre = true;
                        if ($elem[0].className.indexOf('genre') < 0) {
                            $elem.addClass('genre');
                        }
                        break;
                    }
                }
            }
            if (false === hasGenre) {
                genre = programme._userGenres;
                if (genre && genre.length > 0) {
                    for (i = 0, length = genre.length; i < length; i += 1) {
                        if (genre[i] === this._curGenre) {
                            hasGenre = true;
                            if ($elem[0].className.indexOf('genre') < 0) {
                                $elem.addClass('genre');
                            }
                            break;
                        }
                    }
                }
            }
            if (false === hasGenre) {
                if ($elem[0].className.indexOf('genre') >= 0) {
                    $elem.removeClass('genre');
                }
            }
        };
        cGuideBackward.prototype.moveIn = function (fnCB) {
            var $timeline = $('#tvg #timeline .backward').addClass('indicator'), backwardStyle, objTimer = this.animation.aniTimeoutID;
            var $backward = $('#backward'), $chList = $('#chList'), styleMap = this.animation.styleMap, aniTimeout = this.animation.aniTimeoutID;
            this._swapLine = this.TSwapLineType.ENONE;
            hx.log('guideBackward', 'cGuideBackward-moveIn()');
            this.initKey();
            this.timeline.draw();
            this.doMoveIn();
            //if (this.isEmpty(this._$dateTime)) {
            //    this._$dateTime = $('#tvg .dateTime');
            //}
            if (this.animation.use) {
                this.animation.running = true;
                this.sortByChannelList();
                backwardStyle = this.animation.styleMap.catchUp.toCatchUpIn;
                //aniTimeout.backward = $backward.ani({
                //    'style': styleMap.catchUp.toCatchUpIn,
                //    'trans': {
                //        dur: 10 /*300*/ // <for optimize>
                //    },
                //    'cb': () => {
                $backward.css(styleMap.catchUp.toCatchUpIn.end);
                var $viewArea = $('#viewArea'), $optTitle = $('.newGuideOptBg .tit');
                this.animation.running = false;
                aniTimeout.backward = null;
                // add?
                $viewArea.after(this._$dateIndicator).after(this.quickNavi.$);
                $optTitle.addClass('backward');
                //this._setStayedFocusTimeout();
                if (fnCB) {
                    fnCB();
                }
            }
            else {
                $backward.css('opacity', 1);
                $chList.css('opacity', 1);
            }
        };
        cGuideBackward.prototype.sortByChannelList = function () {
            //var i, il, recordingList, ccidList = [];
            //var channelLists = hx.svc.CH_LIST.getChlist({}).chList;
            hx.log('guideBackward', hx.util.format('===== [{0}][sortByChannelList] ===== ', this.name));
            //recordingList = this.recordings.list;
            //for (i = 0, il = channelLists.length; i < il; i += 1) {
            //    ccidList.push(channelLists[i].ccid);
            //}
            //channelLists = null;
        };
        cGuideBackward.prototype.timelineMoveOut = function (afterCB) {
            $('#tvg #timeline .backward').removeClass('indicator').css(this.animation.styleMap.timeline.toNextOut);
            this.timeline.init({
                'animation': this.animation
            });
            if (afterCB) {
                afterCB();
            }
            //return;
            //var objTimer = this.animation.aniTimeoutID, timelineStyle;
            //var $timeline = $('#tvg #timeline .backward').removeClass('indicator');
            //
            //hx.log('guideBackward', 'cGuideBackward-timelineMoveOut()');
            //
            //$timeline.css('opacity', 0);
            //this.timeline.clear();
            //
            //timelineStyle = this.animation.styleMap.timeline.toNextOut;
            //objTimer.timeline = $timeline.ani({
            //    'style': timelineStyle,
            //    'trans': {
            //        'dur': 1/*400*/, // <for optimize>
            //        'timing': 'ease-in-out'
            //    },
            //    'cb': () => {
            //        if (objTimer.timeline === null) {
            //            hx.log('warning', hx.util.format('{0}.timelineTimerId is null.', this.name));
            //            return;
            //        }
            //        objTimer.timeline = null;
            //
            //        if (this.timeline !== null) {
            //            this.timeline.init({
            //                'animation': this.animation
            //            });
            //
            //            if (afterCB) {
            //                afterCB();
            //            }
            //        }
            //    }
            //});
        };
        cGuideBackward.prototype.viewMoveOut = function (afterCB) {
            var objTimer = this.animation.aniTimeoutID, nextStyle;
            var $backward = $('#backward');
            hx.log('guideBackward', 'cGuideBackward-viewMoveOut()');
            nextStyle = this.animation.styleMap.next.toNextOut;
            $backward.css(nextStyle.end);
            objTimer.next = null;
            $backward.css('left', this.CHLIST_WIDTH + 'px');
            if (this.quickNavi) {
                this.quickNavi.destroy();
            }
            if (this._$dateIndicator) {
                this._$dateIndicator.remove();
            }
            if (afterCB) {
                afterCB();
            }
        };
        cGuideBackward.prototype.moveOut = function (fnCB) {
            var $current = $('#tvg #viewArea .current'), $optTitle = $('.newGuideOptBg .tit'), objTimer = null;
            hx.log('guideBackward', 'cGuideBackward-moveOut()');
            this.doMoveOut();
            this.initKey();
            this.clear();
            this.focusInfo = {};
            this.clearStayedFocusTimer();
            $current.css('opacity', 0);
            this.timelineMoveOut();
            this.viewMoveOut(fnCB);
            $optTitle.removeClass('backward');
        };
        cGuideBackward.prototype.clearStayedFocusTimer = function () {
            hx.log('guideBackward', 'cGuideBackward-clearStayedFocusTimer()');
            if (this._stayedFocusTimerId !== null) {
                clearTimeout(this._stayedFocusTimerId);
                this._stayedFocusTimerId = null;
            }
        };
        cGuideBackward.prototype._trySlide = function (key) {
            var _this = this;
            var startTime, endTime, offset = 0, $backward = $('#backward'), $next = $('#next'), aniTimeoutID = this.animation.aniTimeoutID;
            hx.log('guideBackward', 'cGuideBackward-_trySlide()');
            if (aniTimeoutID.fadeIn !== null) {
                hx.log('warning', 'Browser have painted NEXT-VIEW. So this key is ignored.');
                return;
            }
            switch (key.alKey) {
                case hx.key.KEY_LEFT:
                case hx.key.KEY_REWIND:
                    offset = -2;
                    break;
                case hx.key.KEY_RIGHT:
                case hx.key.KEY_FAST_FWD:
                    offset = 2;
                    break;
                case hx.key.KEY_PLAY_SPEED_DOWN:
                    offset = -2;
                    break;
                case hx.key.KEY_PLAY_SPEED_UP:
                    offset = 2;
                    break;
            }
            if (offset > 0) {
                var currTime = this.timeline._getCurrentTime();
                var backwardGuideTime = this.timeline._getBackwardGuideTime();
                if (backwardGuideTime > currTime) {
                    return false;
                }
            }
            this.clear();
            this.freeMem(this.programmes);
            this.programmes = [];
            $backward.css('opacity', 0);
            var onScrollStarted = function () {
                _this.onChangeDateIndication();
            };
            var onScrollStopped = function () {
                _this.updateBanner(false);
                _this.getProgrammes({});
            };
            return this.timeline.doSlide({
                offset: offset * 60 * 60,
                onScrollStarted: onScrollStarted,
                onScrollStopped: onScrollStopped,
                bAnimation: this.ANIMATION_EFFECT
            });
        };
        cGuideBackward.prototype.onChangeDateIndication = function () {
            var $di = this._$dateIndicator, tl = this.timeline, dispTime, date;
            date = tl._getDateOfStartTime();
            if (this._lastUsedDateAsDateIndicator === null) {
                this._lastUsedDateAsDateIndicator = date;
            }
            else if (this._lastUsedDateAsDateIndicator !== date) {
                this._lastUsedDateAsDateIndicator = date;
                dispTime = date;
                $di.children().eq(0).html(dispTime);
                $di.addClass('on');
                $di.anim({
                    'opacity': 1
                }, function () {
                    $di.anim({
                        'opacity': 0
                    }, function () {
                        $di.removeClass('on');
                    });
                });
            }
        };
        cGuideBackward.prototype._checkBoundary = function (aData) {
            var i, rowLen, colLen, programme, channel;
            var startTime = this.timeline.startTime, endTime = this.timeline.endTime, PADDING_TIME = this.PADDING_TIME;
            hx.log('guideBackward', 'cguideBackward-_checkBoundary()');
            for (i = 0, rowLen = aData.length; i < rowLen; i += 1) {
                switch (this._swapLine) {
                    case this.TSwapLineType.ENONE:
                        channel = this.channelList.channels[i];
                        break;
                    case this.TSwapLineType.EUP:
                        channel = this.channelList.channels[0];
                        break;
                    case this.TSwapLineType.EDOWN:
                        channel = this.channelList.channels[this.ROW_COUNT - 1];
                        break;
                }
                if (aData[i].length === 1 && aData[i][0].name === hx.l('LOC_NO_PROGRAMME_INFORMATION_ID')) {
                    aData[i][0] = this.progProvider(startTime, endTime + PADDING_TIME, channel);
                }
                if (aData[i][0].startTime > (startTime + 50)) {
                    programme = this.progProvider(startTime, aData[i][0].startTime, channel);
                    aData[i] = [programme].concat(aData[i]);
                }
                colLen = aData[i].length;
                if ((aData[i][colLen - 1].endTime + 50) < endTime) {
                    programme = this.progProvider(aData[i][colLen - 1].endTime, endTime + PADDING_TIME, channel);
                    aData[i] = aData[i].concat([programme]);
                }
            }
            return aData;
        };
        return cGuideBackward;
    })(__VIEW__);
    return cGuideBackward;
});
