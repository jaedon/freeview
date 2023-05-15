// file name :  pages/tvguide/cGuideNext.js
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
define(["require", "exports", "model/model", "pages/util/provider", "pages/tvguide/cmpt_guideview", "pages/tvguide/cmpt_guidequicknavi", "pages/tvguide/cmpt_guidedcn"], function (require, exports, __MODEL__, __PROVIDER__, __VIEW__, __QUICKNAVI__, __DCN__) {
    var cTimeLine = (function () {
        function cTimeLine(param) {
            this._HOUR = 3600;
            this._DAY = 86400;
            this._PAGE_DURATION = 7200;
            this._PIXEL_PER_HOUR = 480;
            this.init(param);
        }
        cTimeLine.prototype.init = function (param) {
            var now = new Date();
            hx.log('guideNext', 'cGuideNext-TIMELINE-init()');
            now.setMinutes(now.getMinutes() < 30 ? 0 : 30);
            now.setSeconds(0, 0);
            this.startTime = parseInt(now.getTime() / 1000 + "", 10);
            this.endTime = this.startTime + this._PAGE_DURATION;
            this.offset = 0;
            this.$ = null;
            this.animation = param && param.animation ? param.animation : {};
        };
        cTimeLine.prototype.setTimeAndUpdate = function (startTime) {
            var offset;
            hx.log('guideNext', 'cGuideNext-TIMELINE-setTimeAndUpdate()');
            this.startTime = startTime;
            offset = 0;
            this.timeUpdate(offset);
        };
        cTimeLine.prototype.timeUpdate = function (offset) {
            hx.log('guideNext', 'cGuideNext-TIMELINE-timeUpdate()');
            var now = new Date().getTime();
            this.offset = offset;
            this.startTime = this.startTime + this.offset;
            this.endTime = this.startTime + this._PAGE_DURATION;
            if (this.startTime * 1000 > now + (7 * this._DAY * 1000)) {
                hx.msg('response', {
                    text: hx.l('LOC_MIGHT_BE_NO_EPG_MSG_ID'),
                    auto_close: true,
                    close_time: 3000
                });
            }
        };
        cTimeLine.prototype.draw = function () {
            hx.log('guideNext', 'cGuideNext-TIMELINE-draw()');
            if (this.$ === null) {
                this.$ = $('#tvg #timeline .next .wrap_time');
            }
            this._drawTimeline();
            this._updateCurrentPositionBar();
            this._updateTimelineDate();
        };
        cTimeLine.prototype.clear = function () {
            var timeline, $timeline = $('#tvg #timeline .next .wrap_time');
            hx.log('guideNext', 'cGuideNext-TIMELINE-clear()');
            timeline = $timeline.children();
            timeline.eq(0).html('');
            timeline.eq(0).css('left', '0px');
            timeline.eq(1).html('');
            timeline.eq(1).css('left', this._PIXEL_PER_HOUR + 'px');
            timeline.eq(2).html('');
            timeline.eq(2).css('left', this._PIXEL_PER_HOUR * 2 + 'px');
            $timeline.css('left', '0px');
        };
        cTimeLine.prototype.hide = function () {
            var $timeline = $('#tvg #timeline .next .wrap_time');
            hx.log('guideNext', 'cGuideNext-TIMELINE-hide()');
            $timeline.hide();
        };
        cTimeLine.prototype.show = function () {
            var $timeline = $('#tvg #timeline .next .wrap_time');
            hx.log('guideNext', 'cGuideNext-TIMELINE-show()');
            $timeline.show();
        };
        cTimeLine.prototype.timeUpdateAndDraw = function (offset) {
            hx.log('guideNext', 'cGuideNext-TIMELINE-timeUpdateAndDraw()');
            this.timeUpdate(offset);
            this.draw();
        };
        cTimeLine.prototype.doSlide = function (param) {
            var _this = this;
            var start, end, last, startTime, endTime, lastTime, left, offset;
            var base, $first, $second, $third;
            var bAnimation = param.bAnimation;
            hx.log("guideNext", hx.util.format('+++++++ ####### doSlide[Start] this.animation.running = {0} ####### +++++++', this.animation.running));
            var anim = function () {
                if (_this.animation.running) {
                    hx.log("guideNext", '+++++++ ####### anim return; ####### +++++++');
                    return;
                }
                if (!_this.$) {
                    hx.log("guideNext", '+++++++ ####### !this.$ return; ####### +++++++');
                    return;
                }
                _this.animation.running = true;
                _this.animation.state = 'sliding';
                base = _this.$.position().left;
                $first = _this.$.find('.first');
                $second = _this.$.find('.second');
                $third = _this.$.find('.third');
                _this.timeUpdate(offset);
                _this._updateCurrentPositionBar();
                startTime = new Date(_this.startTime * 1000);
                endTime = new Date((_this.startTime + 60 * 60) * 1000);
                lastTime = new Date((_this.startTime + 60 * 60 * 2) * 1000);
                if (offset >= 0) {
                    left = base - (_this._PIXEL_PER_HOUR * 2);
                    start = $third.position().left;
                    end = start + _this._PIXEL_PER_HOUR;
                    last = end + _this._PIXEL_PER_HOUR;
                    _this.$.append($('<li>', {
                        'class': 'time first',
                        'html': startTime.format('HH:MM')
                    }).css('left', start + 'px')).append($('<li>', {
                        'class': 'time second',
                        'html': endTime.format('HH:MM')
                    }).css('left', end + 'px')).append($('<li>', {
                        'class': 'time third',
                        'html': lastTime.format('HH:MM')
                    }).css('left', last + 'px'));
                }
                else {
                    left = base + (_this._PIXEL_PER_HOUR * 2);
                    last = $first.position().left;
                    end = last - _this._PIXEL_PER_HOUR;
                    start = end - _this._PIXEL_PER_HOUR;
                    _this.$.prepend($('<li>', {
                        'class': 'time third',
                        'html': lastTime.format('HH:MM')
                    }).css('left', last + 'px')).prepend($('<li>', {
                        'class': 'time second',
                        'html': endTime.format('HH:MM')
                    }).css('left', end + 'px')).prepend($('<li>', {
                        'class': 'time first',
                        'html': startTime.format('HH:MM')
                    }).css('left', start + 'px'));
                }
                $first.addClass('old');
                $second.addClass('old');
                $third.addClass('old');
                hx.log("guideNext", hx.util.format('+++++++ ####### doSlide left = {0} ####### +++++++', left));
                hx.log("guideNext", hx.util.format('+++++++ ####### doSlide this.$.position().left = {0} ####### +++++++', _this.$.position().left));
                _this.$.addClass('move');
                _this.$.anim({
                    'left': left + 'px'
                }, function () {
                    _this.animation.running = false;
                    _this.animation.state = 'idle';
                    _this.$.removeClass('move');
                    _this.$.find('li.old').remove();
                    hx.log("guideNext", '+++++++ ####### doSlide Animation Finished; ####### +++++++');
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
                    this._animParam = param;
                }
                else {
                    offset = param.offset;
                    this.onScrollStarted = param.onScrollStarted;
                    this.onScrollStopped = param.onScrollStopped;
                    anim();
                }
            }
            else {
                offset = param.offset;
                this.timeUpdateAndDraw(offset);
                if (param.onScrollStarted) {
                    param.onScrollStarted();
                }
                if (param.onScrollStopped) {
                    param.onScrollStopped();
                }
            }
            hx.log("guideNext", hx.util.format('+++++++ ####### doSlide[End] this.animation.running = {0} ####### +++++++', this.animation.running));
            return true;
        };
        cTimeLine.prototype._drawTimeline = function () {
            var time, timeline, $timeline = $('#tvg #timeline .next .wrap_time');
            hx.log('guideNext', 'cGuideNext-TIMELINE-_drawTimeline()');
            $timeline.position().left = 0;
            time = new Date(this.startTime * 1000);
            timeline = $timeline.children();
            timeline.eq(0).html(time.format('HH:MM'));
            time.setMinutes(time.getMinutes() + 60);
            timeline.eq(1).html(time.format('HH:MM'));
            time.setMinutes(time.getMinutes() + 60);
            timeline.eq(2).html(time.format('HH:MM'));
        };
        cTimeLine.prototype._getCurrentTime = function () {
            var now = new Date();
            hx.log('guideNext', 'cGuideNext-TIMELINE-_getCurrentTime()');
            return (parseInt(now.getTime() / 1000 + "", 10));
        };
        cTimeLine.prototype._getCssStyleLeft = function (t) {
            hx.log('guideNext', 'cGuideNext-TIMELINE-_getCssStyleLeft()');
            return parseInt(this._PIXEL_PER_HOUR * ((t - this.startTime) / 3600) + "", 10) + 'px';
        };
        cTimeLine.prototype._updateCurrentPositionBar = function () {
            var nowTime = this._getCurrentTime(), $yellowLine = $('#tvg #viewArea .current'), cssObj;
            hx.log('guideNext', 'cGuideNext-TIMELINE-_updateCurrentPositionBar()');
            if (nowTime >= this.startTime && this.endTime > nowTime) {
                cssObj = {
                    'left': this._getCssStyleLeft(nowTime),
                    'opacity': $('#tvg').hasClass('next') ? 1 : 0
                };
                $yellowLine.css(cssObj);
            }
            else {
                $yellowLine.css('opacity', 0);
            }
        };
        cTimeLine.prototype._updateTimelineDate = function () {
            hx.log('guideNext', 'cGuideNext-TIMELINE-_updateTimelineDate()');
            var now = new Date();
            var $timelineDate = $('#tvg .next.indicator .date');
            if (now.format('dd.mm') === dateFormat(this.startTime * 1000, 'dd.mm')) {
                $timelineDate.text("TODAY");
            }
            else {
                $timelineDate.text(dateFormat(this.startTime * 1000, 'ddd dd mmm'));
            }
        };
        cTimeLine.prototype.updateCurrentPositionBar = function () {
            hx.log('guideNext', 'cGuideNext-TIMELINE-updateCurrentPositionBar()');
            this._updateCurrentPositionBar();
        };
        cTimeLine.prototype._getDateOfStartTime = function () {
            var tempTime, day;
            tempTime = new Date(this.startTime * 1000);
            day = tempTime.format('ddd dd.mm');
            return day;
        };
        return cTimeLine;
    })();
    var cGuideNext = (function (_super) {
        __extends(cGuideNext, _super);
        function cGuideNext(param) {
            _super.call(this, $('<article>', {
                'id': 'next',
                'class': 'focusable focused',
                'style': 'opacity:0;',
                'name': 'guideNext'
            }), 'focusable', 'on', param);
            this.PADDING_TIME = 1800;
            this.pixelPerHour = 480;
            this.pixelPerMin = 8;
            this.IntervalKeyDown = 850;
            this._stayedFocusTimerId = null;
            this.TScheduledFactoryType = {
                ERECORDING: 0,
                EREMIND: 1,
                EHTTP_LIVE_STREAMING: 4,
                ESERIES_RECORDING: 10
            };
            this._lastUsedDateAsDateIndicator = null;
            this.$ = this._$;
            var box;
            //hx.logadd('guideNext');
            hx.log('guideNext', 'cGuideNext-init()');
            box = document.createElement('ul');
            box.className = 'gItem';
            this._$dateIndicator = $('<div>', {
                'id': 'changeDateLayout',
                'style': 'opacity:0;'
            }).append($('<div>', {
                'id': 'changeDateText'
            }));
            this._swapLine = this.TSwapLineType.ENONE;
            this.box = box;
            this.timeline = new cTimeLine(param);
            this._lastUsedDateAsDateIndicator = this.timeline._getDateOfStartTime();
            //this.opt = new __GENRELIST__();
            this.quickNavi = new __QUICKNAVI__(param);
            this.dcn = new __DCN__();
            //this.add(this.opt);
            this.add(this.dcn);
            this.add(this.quickNavi);
            this.create();
        }
        cGuideNext.prototype.create = function () {
            var i, ROW_COUNT, top, li, fragment = document.createDocumentFragment();
            hx.log('guideNext', 'cGuideNext-create()');
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
        cGuideNext.prototype.destroy = function () {
            hx.log('guideNext', 'cGuideNext-destroy()');
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
                if (this.opt.$) {
                    this.opt.$.remove();
                }
                this.opt.destroy();
                delete this.opt;
                this.opt = null;
            }
            if (this.quickNavi) {
                if (this.quickNavi.$) {
                    this.quickNavi.$.remove();
                }
                delete this.quickNavi;
                this.quickNavi = null;
            }
            if (this.dcn) {
                delete this.dcn;
                this.dcn = null;
            }
            this.programmes = [];
            _super.prototype.destroy.call(this);
        };
        cGuideNext.prototype._isRunningAnimationOfViewChange = function () {
            var aniTimeout = this.animation.aniTimeoutID, isRunning = false;
            hx.log('guideNext', 'cGuideNext-_isRunningAnimationOfViewChange()');
            if (aniTimeout.gItem !== null || aniTimeout.timeline !== null || aniTimeout.next !== null) {
                hx.log("warning", hx.util.format('animation of "onAirView" is running. {0}', aniTimeout.gItem));
                isRunning = true;
                this._isRepeat = true;
            }
            if (true === this.animation.running) {
                isRunning = true;
                this._isRepeat = true;
            }
            return isRunning;
        };
        cGuideNext.prototype._on_MetadataUpdate = function (param) {
            //var updateFlag;
            //
            //hx.log('guideNext', '=================================================================');
            //hx.log('guideNext', hx.util.format('===== [{0}][_on_MetadataUpdate] search ID : {1}, info : {2} ===== ', this.name, this.lastSearchId.id, param.info));
            //hx.log('guideNext', '=================================================================');
            //hx.log('error','_on_MetadataUpdate cmpt_guidenext');
            //if (this.lastSearchId.id === param.info) {
            //    if (this.focusInfo.$item === null) {
            //        updateFlag = false;
            //    } else {
            //        updateFlag = true;
            //    }
            //
            //    $.extend(param, {
            //        'updateFlag': false
            //    });
            //    this.getProgrammes(param);
            //}
        };
        cGuideNext.prototype._on_alKey = function (param) {
            var alKey = param.alKey, useAnimation = this.animation.use, isRunning = this.animation.running, isConsumed = false;
            var currentTime;
            hx.log('guideNext', 'cGuideNext-_on_alKey()');
            hx.log('guideNext', hx.util.format('state : {0}', this.state));
            if (ENV.op === ENV.listOP.UKDTT) {
                if (this._genreDlg || (this.state === this.EVIEW_STATE.EBUSY)) {
                    return _super.prototype._on_alKey.call(this, param);
                }
            }
            else {
                if (this.state === this.EVIEW_STATE.EBUSY) {
                    return _super.prototype._on_alKey.call(this, param);
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
                        if (param.alRepeat === 0) {
                            //this._setStayedFocus();
                            this._setFocusCssStyle('', 'on');
                            if (!this.doNavi(param)) {
                                isConsumed = this._trySlide(param);
                            }
                        }
                        else if (param.alRepeat > 5) {
                            this.showQuickNavi();
                        }
                    }
                    break;
                case hx.key.KEY_OPTION:
                    var option = this.opt, isShow;
                    if (this._stayedFocusTimerId !== null) {
                        clearTimeout(this._stayedFocusTimerId);
                        this._stayedFocusTimerId = null;
                    }
                    if (ENV.op !== ENV.listOP.UKDTT) {
                        isShow = option.showHideToggle();
                        if (!isShow) {
                            this._setFocusCssStyle('', 'on');
                            this.state = this.EVIEW_STATE.EIDLE;
                        }
                        else {
                            this.state = this.EVIEW_STATE.EBUSY;
                        }
                        isConsumed = true;
                    }
                    break;
                case hx.key.KEY_RECORD:
                    if (!this._showDetail) {
                        var now = new Date();
                        var nowTime = parseInt(now.getTime() / 1000 + "", 10);
                        var focus_channel = this.channelList.channels[this.channelList.currentRowIndex];
                        var provider_programme = this.focusInfo.programme;
                        if (this.isEmpty(provider_programme) || this.isEmpty(focus_channel.ccid) || (nowTime > provider_programme.getStartTime()) || this.isEmpty(provider_programme.programmeID)) {
                            isConsumed = true;
                        }
                        else {
                            if (provider_programme.hasSchedule() || provider_programme.hasReminder()) {
                                this.cancelReservation(provider_programme);
                            }
                            else if (provider_programme.hasSeriesSchedule()) {
                                this.cancelSeriesReservation(provider_programme);
                            }
                            else {
                                if (provider_programme.getGroupCRIDs() && provider_programme.getGroupCRIDs().length) {
                                    this.selectRecordType(provider_programme);
                                }
                                else {
                                    this.doReservation(provider_programme);
                                }
                            }
                            isConsumed = true;
                        }
                    }
                    break;
            }
            return (isConsumed || _super.prototype._on_alKey.call(this, param));
        };
        cGuideNext.prototype.getProgrammes = function (param) {
            var _this = this;
            var objData = {};
            var drawProgrammes = [];
            var i, resultProgrammes;
            var self = this;
            if (this._isWaitingScroll()) {
                hx.log("warning", hx.util.format('This getProgramme function is waiting for the last data'));
                return;
            }
            hx.log("debug", 'cGuideNext +++++++ ####### getProgrammes START ####### +++++++');
            this._currentRow = 0;
            var reqSwapLine = this._swapLine;
            var channels = this.channelList.channels;
            objData = this._parse(param);
            objData.cb = function (result) {
                if (_this._isWaitingScroll()) {
                    hx.log("warning", hx.util.format('This draw function is waiting for the last data'));
                    return;
                }
                if (!_this._receiveCallback || !_this.channelList || _this.state === _this.EVIEW_STATE.EHIDE) {
                    hx.log("warning", hx.util.format('You have arrived too late. {0} was already gone.', _this.name));
                    return;
                }
                if (_this.timeline === null) {
                    hx.log("warning", hx.util.format('this.timeline === null'));
                    return;
                }
                var programmes = _this.srvDataToUi(result, param.count, _this.EDATA_TYPE.EPROGRAMME);
                hx.log('guideNext', '\tcGuideNext-getProgrammes()-cb ' + ' programmes.length' + programmes.length);
                _this._debug(programmes);
                resultProgrammes = _this._checkBoundary(programmes, reqSwapLine, channels);
                hx.log('guideNext', '\tcGuideNext-getProgrammes()-cb ' + ' resultProgramme.length' + resultProgrammes.length);
                switch (reqSwapLine) {
                    case _this.TSwapLineType.ENONE:
                        _this.freeMem(_this.programmes);
                        _this.programmes = [];
                        _this.programmes = resultProgrammes;
                        drawProgrammes = _this.programmes;
                        break;
                    case _this.TSwapLineType.EUP:
                        _this.programmes.splice(_this.ROW_COUNT - 1, 1);
                        _this.programmes.splice(0, 0, resultProgrammes[0]);
                        drawProgrammes = _this.programmes;
                        break;
                    case _this.TSwapLineType.EDOWN:
                        _this.programmes.splice(0, 1);
                        _this.programmes.push(resultProgrammes[0]);
                        drawProgrammes = _this.programmes;
                        break;
                }
                self.draw(drawProgrammes, param.updateFlag, reqSwapLine, channels);
                drawProgrammes = undefined;
                resultProgrammes = undefined;
                programmes = undefined;
                result = undefined;
            };
            this.lastSearchId = hx.svc.EPG.getProgrammes(objData);
        };
        cGuideNext.prototype.draw = function (programmesArray, updateFlag, reqSwapLine, channels) {
            var drawProgrammes = programmesArray;
            var self = this;
            var $indicator = $('#tvg #indicator .indi_text');
            hx.log("debug", 'cGuideNext +++++++ ####### draw START ####### +++++++');
            if (this.animation.running && (this.animation.state === 'scrolling' || this.animation.state === 'sliding')) {
                hx.log("guideNext", hx.util.format('+++++++ ####### draw this.animation.running = {0} ####### +++++++', this.animation.running));
                hx.log("guideNext", hx.util.format('+++++++ ####### draw this.animation.state = {0} ####### +++++++', this.animation.state));
                return;
            }
            // General variables
            var i, j, k, m, row, rowLen, elemLen, colLen, li, div, elem, temp, updateRow;
            // Data variables
            var programme, dispGenre = (this._curGenre !== '');
            var base = -$(this.box).position().left;
            var complement = 0;
            var plus7day = this.quickNavi.getForwardLimitTime();
            function fnDrawRow(channel, programmes, currentRow) {
                if (!programmes) {
                    hx.log('error', '[guidenext] fnDrawRow programmes NULL');
                    return;
                }
                colLen = programmes.length;
                if (colLen && channel.ccid) {
                    complement = 0;
                    for (j = 0; j < colLen; j += 1) {
                        programme = programmes[j];
                        self._getProgrammeAttr(programme, updateRow);
                        li = currentRow.element[j];
                        if (!li) {
                            li = document.createElement('li');
                            currentRow.element[j] = li;
                            self.box.appendChild(li);
                        }
                        var stgetCssStyle = self._getCssStyle(programme, base, currentRow.top, complement);
                        complement = stgetCssStyle.Complement;
                        li.style.left = stgetCssStyle.left;
                        li.style.width = stgetCssStyle.width;
                        li.style.height = self.ROW_HEIGHT;
                        li.style.top = stgetCssStyle.top;
                        li.style.padding = stgetCssStyle.padding;
                        li.setAttribute('row', updateRow);
                        li.setAttribute('col', j);
                        li.className = self._getClassName(programme, updateRow);
                        li.innerHTML = programme.getPluginObject() ? self.getCellHTML(programme, false) : self.getNoProgrammeInformationCellHTML(programme, false);
                        if (dispGenre) {
                            self._genre(programme, $(li));
                        }
                    }
                    //MDS
                    function mdsFunc(programmes, elements) {
                        var p = programmes[0];
                        var channel;
                        var i = 0;
                        for (i = 0; i < programmes.length; i++) {
                            if (programmes[i].channel) {
                                channel = programmes[i].channel;
                                break;
                            }
                        }
                        if (!channel) {
                            hx.log('mds', 'pass, channel not found');
                            return;
                        }
                        var ids = programmes.map(function (programme) {
                            var splitedId = hx.svc.IP_EPG.parse.util.programIdParser(programme.getProgrammeID());
                            return [splitedId.onid, splitedId.svcid, splitedId.eventid].join('.');
                        });
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
                        //var callback = (xmls) => {
                        function callback(xmls) {
                            xmls.forEach(function (info) {
                                var schedules = info.schedules;
                                for (var i = 0, scheduleEvent; i < schedules.length; i++) {
                                    scheduleEvent = schedules[i];
                                    var event = hx.svc.IP_EPG.parse.element.scheduleEvent(scheduleEvent);
                                    var splitedId = hx.svc.IP_EPG.parse.util.programIdParser(event.programURL);
                                    var id = [splitedId.onid, splitedId.svcid, splitedId.eventid].join('.');
                                    var index = ids.indexOf(id);
                                    if (index !== -1) {
                                        hx.log('mds', 'ids index: ' + index);
                                        var data = info.crids[event.crid];
                                        data.schedule = scheduleEvent;
                                        data.available = false;
                                        programmes[index].setMdsData({
                                            mdsData: data,
                                            launchLocation: 'epg'
                                        });
                                        (function (aProg, aEl) {
                                            setTimeout(function () {
                                                hx.svc.IP_EPG.getAIT(aProg.getTemplateAitUrl()).then(function (xml) {
                                                    aProg.checkTemplateAIT(xml);
                                                    fnUpdateAvailability(aProg, aEl);
                                                }, function (err) {
                                                });
                                            }, 10);
                                        })(programmes[index], elements[index]);
                                    }
                                }
                            });
                        }
                        var time = new Date(p.getStartTime() * 1000).toSeconds();
                        var hours = 6;
                        if (time > plus7day) {
                            callback([]);
                        }
                        else {
                            time *= 1000;
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
                            addScheduleQueryToQueryList(channel.sid, time, hours);
                            addScheduleQueryToQueryList(channel.sid, time + (hours * 60 * 60 * 1000), hours);
                            hx.log('debug', '[NEXT][getProgrammes] queryList-count : ' + queryList.length);
                            hx.svc.IP_EPG.getScheduleList(queryList).then(function () {
                                hx.log('debug', '[NEXT][getProgrammes] Success! xmls-count : ' + xmls.length);
                                callback(xmls);
                            }, function () {
                                hx.log('debug', '[NEXT][getProgrammes] Fail? xmls-count : ' + xmls.length);
                                callback(xmls);
                            });
                        }
                    }
                    mdsFunc(programmes, currentRow.element);
                }
                else {
                    li = currentRow.element[0];
                    li.innerHTML = '';
                }
            }
            ;
            switch (reqSwapLine) {
                case this.TSwapLineType.EUP:
                    temp = this.rowList.splice(this.ROW_COUNT - 1, 1);
                    this.rowList.splice(0, 0, temp[0]);
                    for (k = this.box.childNodes.length - 1; k >= 0; k -= 1) {
                        if (parseInt(this.box.childNodes[k].getAttribute('row'), 10) === this.ROW_COUNT - 1) {
                            this.box.removeChild(this.box.childNodes.item(k));
                        }
                    }
                    for (k = 0; k < this.rowList.length; k += 1) {
                        if (this.rowList[k].top === this.ROW_HEIGHT * (this.ROW_COUNT - 1)) {
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
                                if (dispGenre && drawProgrammes[k]) {
                                    this._genre(drawProgrammes[k][m], $(this.rowList[k].element[m]));
                                }
                                if (this.rowList[k].top === this.ROW_HEIGHT * (this.ROW_COUNT - 1)) {
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
                    fnDrawRow(channels[0], drawProgrammes[0], row);
                    this._swapLine = this.TSwapLineType.ENONE;
                    break;
                case this.TSwapLineType.EDOWN:
                    temp = this.rowList.splice(0, 1);
                    this.rowList.push(temp[0]);
                    rowLen = this.rowList.length;
                    for (k = this.box.childNodes.length - 1; k >= 0; k -= 1) {
                        if (parseInt(this.box.childNodes[k].getAttribute('row'), 10) === 0) {
                            this.box.removeChild(this.box.childNodes.item(k));
                        }
                    }
                    for (k = 0; k < rowLen; k += 1) {
                        if (this.rowList[k].top === 0) {
                            this.rowList[k].element = [];
                            row = this.rowList[k];
                            updateRow = this.ROW_COUNT - 1;
                            row.top = this.ROW_HEIGHT * (this.ROW_COUNT - 1);
                        }
                        else {
                            this.rowList[k].top -= this.ROW_HEIGHT;
                            for (m = 0, colLen = this.rowList[k].element.length; m < colLen; m += 1) {
                                this.rowList[k].element[m].setAttribute('row', parseInt(this.rowList[k].element[m].getAttribute('row'), 10) - 1);
                                this.rowList[k].element[m].style.top = (this.ROW_HEIGHT * parseInt(this.rowList[k].element[m].getAttribute('row'), 10)) + 'px';
                                if ($(this.rowList[k].element[m]).hasClass('onFocus')) {
                                    this.rowList[k].element[m].className = 'pInfo focusable on';
                                }
                                else {
                                    this.rowList[k].element[m].className = 'pInfo focusable';
                                }
                                if (dispGenre && drawProgrammes[k]) {
                                    this._genre(drawProgrammes[k][m], $(this.rowList[k].element[m]));
                                }
                            }
                        }
                    }
                    for (k = this._$focusable.length - 1; k >= 0; k -= 1) {
                        if (this._$focusable[k][hx.key.KEY_UP] === null) {
                            this._$focusable.splice(k, 1);
                        }
                    }
                    fnDrawRow(channels[this.ROW_COUNT - 1], drawProgrammes[rowLen - 1], row);
                    this._swapLine = this.TSwapLineType.ENONE;
                    break;
                case this.TSwapLineType.ENONE:
                    var channel = null;
                    for (i = 0, rowLen = drawProgrammes.length; i < rowLen; i += 1) {
                        for (k = this.box.childNodes.length - 1; k >= 0; k -= 1) {
                            if (parseInt(this.box.childNodes[k].getAttribute('row'), 10) === i) {
                                this.box.removeChild(this.box.childNodes.item(k));
                            }
                        }
                        row = this.rowList[i];
                        channel = channels[i];
                        this.rowList[i].element = [];
                        updateRow = i;
                        fnDrawRow(channel, drawProgrammes[i], row);
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
            }
            else if (this._swapLine === this.TSwapLineType.EDOWN) {
                for (k = 0; k < row.element.length; k += 1) {
                    this._$focusable.push(row.element[k]);
                }
                this.build(this.positionBuilder);
                this.doNavi({
                    alKey: hx.key.KEY_DOWN
                });
            }
            var isEnablePopUPFocus = false;
            this.doDraw(isEnablePopUPFocus, updateFlag);
            if (!updateFlag) {
                this._setFocusCssStyle('', 'on');
            }
            this.$.css('opacity', 1);
            $indicator[0].innerHTML = hx.l('LOC_CATCHUP_VIEW_ID');
            hx.log("debug", 'cGuideNext +++++++ ####### draw FINISHED ####### +++++++');
        };
        cGuideNext.prototype.dispTimeHTML = function () {
            hx.log('guideNext', 'cGuideNext-dispTimeHTML()');
            return (' - ');
        };
        cGuideNext.prototype.getThumbClass = function (data) {
            hx.log('guideNext', 'cGuideNext-getThumbClass()');
            return data.isOnAir ? '<figure class="thum onAir icoRec">' : '<figure class="thum icoRec">';
        };
        cGuideNext.prototype.clear = function () {
            var i, j, il, jl;
            hx.log('guideNext', 'cGuideNext-clear()');
            for (i = 0, il = this.ROW_COUNT; i < il; i += 1) {
                for (j = 0, jl = this.rowList[i].element.length; j < jl; j += 1) {
                    this.rowList[i].element[j].innerHTML = '';
                    this.rowList[i].element[j].style = '';
                    this.rowList[i].element[j].className = '';
                }
            }
            this.focusInfo = {};
            this.doClear();
        };
        cGuideNext.prototype.popFocusStyleLeft = function ($item) {
            throw 'this function.. is..';
            var left, padding, style_left;
            hx.log('guideNext', 'cGuideNext-popFocusStyleLeft()');
            if (hx.config.tvGuide2ndScenario) {
                left = parseInt($item.css('left'), 10) + $(this.box).position().left + 20;
                padding = 0;
            }
            else {
                left = parseInt($item.css('left'), 10) + $(this.box).position().left;
                padding = parseInt($item.css('padding-left'), 10) > 0 ? 10 : 0;
            }
            style_left = (left < padding ? 0 : (left > this.MAX_FOCUS_LEFT ? this.MAX_FOCUS_LEFT : left - padding)) + 'px';
            return style_left;
        };
        cGuideNext.prototype.RecordingEvent = function (param, item) {
            var channels = this.channelList.channels;
            hx.log('guideNext', 'cGuideNext-RecordingEvent()');
            this.cellRenewal(false, item);
            this.channelList.checkTunes(channels);
        };
        cGuideNext.prototype.doReservation = function (data, aFactoryType) {
            var _this = this;
            var efactoryType = aFactoryType || this.TScheduledFactoryType.ERECORDING;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var scheduledList = dsrvRecordings2.getScheduledList();
            hx.log('guideNext', 'cGuideNext-doReservation()');
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
                dsrvRecordings2.record({
                    programme: aProgramme,
                    factoryType: efactoryType,
                    finished: function () {
                        hx.log('guideNext', 'finished record');
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
                                _this.sendEventToParent({
                                    action: "alDialogRequested",
                                    alData: {
                                        hdProgramme: hdProgramme,
                                        sdProgramme: data,
                                        fnRecorder: _fnRecord
                                    }
                                });
                            }
                            else {
                                hx.log("linkaged detected but can't find HD programme");
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
        cGuideNext.prototype.selectRecordType = function (aProgramme) {
            var _this = this;
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
                            _this.doReservation(_data._programme, factoryType);
                        }
                    }
                }
            });
        };
        cGuideNext.prototype.cancelReservation = function (data) {
            hx.log('guideNext', 'cGuideNext-cancelReservation()');
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
                        hx.log('guideNext', 'finished removeScheduledRecording');
                    }
                });
            }
        };
        cGuideNext.prototype.fnGetSameSeriesScheduleList = function (item) {
            var i, j, k;
            var ret = [];
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var schedulelist = dsrvRecordings2.getScheduledList();
            for (i = 0; i < schedulelist.length; i += 1) {
                if (schedulelist[i].getGroupCRIDs() && item.getGroupCRIDs()) {
                    if (item.getGroupCRIDs().length > 0 && schedulelist[i].getGroupCRIDs().length > 0) {
                        for (j = 0; j < item.getGroupCRIDs().length; j += 1) {
                            for (k = 0; k < schedulelist[i].getGroupCRIDs().length; k++) {
                                if (item.getGroupCRIDs()[j] === schedulelist[i].getGroupCRIDs()[k]) {
                                    ret.push(schedulelist[i]);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            return ret;
        };
        cGuideNext.prototype.cancelSeriesReservation = function (data) {
            hx.log('guideNext', 'cGuideNext-cancelSeriesReservation()');
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
                var scheduledList = this.fnGetSameSeriesScheduleList(foundItem);
                dsrvRecordings2.removeWholeSeriesSchedule({
                    items: scheduledList,
                    finished: function () {
                        hx.log('guideNext', 'finished removeSeriesScheduledRecording');
                    }
                });
            }
        };
        cGuideNext.prototype.changeChannel = function (param) {
            var state, flag = true;
            hx.log('guideNext', hx.util.format('===== [{0}][changeChannel] ===== ', this.name));
            state = this.channelList.changeChannel(param);
            switch (state) {
                case 'clickedDisabledChannel':
                    this._showMessage();
                    flag = false;
                    break;
                case 'changeChannel':
                    var r, c, programme = this.focusInfo.programme, focus;
                    if (programme) {
                        r = this.focusInfo.$item.attr('row');
                        c = this.focusInfo.$item.attr('col');
                        this.rowList[r].element[c].innerHTML = programme.getPluginObject() ? this.getCellHTML(programme, true) : this.getNoProgrammeInformationCellHTML(programme, true);
                    }
                    break;
            }
            return flag;
        };
        cGuideNext.prototype._showMessage = function () {
            hx.log('guideNext', hx.util.format('===== [{0}][_showMessage] ===== ', this.name));
            this._hideMessage();
            if (hx.config.useTVStreaming === false) {
                this._msgObj = hx.msg('response', {
                    'text': hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                    'auto_close': true,
                    'timeout': 3000,
                    'dia_class': 'res vaTop'
                });
            }
            else {
                this._msgObj = hx.msg('response', {
                    'text': hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                    'auto_close': true,
                    'timeout': 3000,
                    'dia_class': 'res vaTop'
                });
            }
        };
        cGuideNext.prototype._hideMessage = function () {
            hx.log('guideNext', hx.util.format('===== [{0}][_hideMessage] ===== ', this.name));
            if (this._msgObj !== null) {
                hx.msg.close('response', this._msgObj);
                this._msgObj = null;
            }
        };
        cGuideNext.prototype.closeGenreOption = function (param) {
            hx.log('guideNext', 'cGuideNext-closeGenreOption()');
            this.state = this.EVIEW_STATE.EIDLE;
            this._setFocusCssStyle('', 'on');
            if (param.isGenreChange) {
                this._curGenre = this.opt ? this.opt.selectedGenre : '';
                this._genreHighlighted();
            }
        };
        cGuideNext.prototype.showQuickNavi = function () {
            hx.log('guideNext', 'cGuideNext-showQuickNavi()');
            this.quickNavi.config('forward');
            this.state = this.EVIEW_STATE.EBUSY;
            if (this._stayedFocusTimerId !== null) {
                clearTimeout(this._stayedFocusTimerId);
                this._stayedFocusTimerId = null;
            }
            this.quickNavi.show(this.timeline.startTime);
        };
        cGuideNext.prototype.hideQuickNavi = function () {
            hx.log('guideNext', 'cGuideNext-hideQuickNavi()');
            this.state = this.EVIEW_STATE.EIDLE;
            this._setFocusCssStyle('', 'on');
        };
        cGuideNext.prototype.quickNaviClicked = function (param) {
            var date = param.data;
            hx.log('guideNext', 'cGuideNext-quickNaviClicked()');
            this._lastUsedDateAsDateIndicator = this.timeline._getDateOfStartTime();
            if (date) {
                this.timeline.setTimeAndUpdate(date.toSeconds());
                this.timeline.draw();
                this.initKey();
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
        cGuideNext.prototype._genreHighlighted = function () {
            var i, l, j, li, jl, programmes, element, className, index, data = this.programmes;
            hx.log('guideNext', 'cGuideNext-_genreHighlighted()');
            for (i = 0, l = data.length; i < l; i += 1) {
                programmes = data[i];
                element = this.rowList[i];
                for (j = 0, jl = programmes.length; j < jl; j += 1) {
                    li = element.element[j];
                    this._genre(programmes[j], $(li));
                }
            }
        };
        cGuideNext.prototype._parse = function (param) {
            var channels = this.channelList.channels, timeline = this.timeline, startTime, endTime;
            var lChannels = [];
            hx.log('guideNext', 'cGuideNext-_parse()');
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
            startTime = (timeline._getCurrentTime() > timeline.startTime) ? timeline._getCurrentTime() : (timeline.startTime + 30);
            endTime = timeline.endTime;
            return {
                'isOnAir': false,
                'channel': lChannels,
                'startTime': startTime,
                'endTime': endTime,
                'info': param.info
            };
        };
        cGuideNext.prototype.isLive = function (programme) {
            if (!programme) {
                return false;
            }
            var now = new Date(), nowTime, ret = false;
            hx.log('guideNext', 'cGuideNext-isLive()');
            nowTime = parseInt(now.getTime() / 1000 + "", 10);
            if (nowTime > programme.getStartTime() && nowTime < programme.getEndTime()) {
                ret = true;
            }
            return ret;
        };
        cGuideNext.prototype._getProgrammeAttr = function (programme, row) {
            hx.log('guideNext', 'cGuideNext-_getProgrammeAttr()');
            programme.nextView = true;
            programme.isOnAir = this.isLive(programme);
            programme.isCAS = this.channelList.casIDs[row] || '';
        };
        cGuideNext.prototype._timeToPixel = function (t) {
            hx.log('guideNext', 'cGuideNext-_timeToPixel()');
            return (parseInt(t / 60 + "", 10) * this.pixelPerMin);
        };
        cGuideNext.prototype._getCssStyle = function (programme, base, top, beforeComplement) {
            var timeline = this.timeline, startTime = timeline.startTime, endTime = timeline.endTime;
            var margin = 1, padding = 30, border = 4;
            var start, duration, left, width, style = {}, str, offset;
            hx.log('guideNext', 'cGuideNext-_getCssStyle()');
            start = (programme.getStartTime() < startTime) ? startTime : programme.getStartTime();
            offset = 0;
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
            style.padding = "";
            style.left = (base + this._timeToPixel(start - startTime)) + 'px';
            width = this._timeToPixel(duration) - border;
            if (width < 0) {
                width = 0;
            }
            if (duration < 300) {
                style.padding = '0';
                style.width = width + 'px';
            }
            else {
                //style.width = (width - padding) + 'px';
                style.width = width + 'px';
            }
            style.top = top + 'px';
            style.Complement = duration % 60;
            return style;
        };
        cGuideNext.prototype._getClassName = function (programme, row) {
            var timeline = this.timeline, startTime = timeline.startTime + 60, endTime = timeline.endTime + 60;
            var now = new Date(), currentTime = parseInt(now.getTime() / 1000 + "", 10);
            var isFocusable = false;
            var classStr = '';
            hx.log('guideNext', 'cGuideNext-_getClassName()');
            //if (row >= 0 && row < (this.ROW_COUNT - 1) && this.channelList.channels[row].ccid) {
            if (row >= 0 && row < this.ROW_COUNT && this.channelList.channels[row].ccid) {
                if (programme.getEndTime() > currentTime) {
                    if (programme.getStartTime() < startTime && programme.getEndTime() > startTime) {
                        isFocusable = true;
                    }
                    else if (programme.getStartTime() < endTime && programme.getEndTime() > startTime) {
                        isFocusable = true;
                    }
                    else if (programme.getStartTime() < endTime && programme.getEndTime() > endTime) {
                        isFocusable = true;
                    }
                }
            }
            classStr = isFocusable ? 'pInfo focusable' : 'pInfo';
            classStr = classStr.concat(programme.getPluginObject() ? '' : ' noProgramme');
            return classStr;
        };
        cGuideNext.prototype._genre = function (programme, $elem) {
            var i, length, genre, hasGenre;
            hx.log('guideNext', 'cGuideNext-_genre()');
            if (!programme) {
                hx.log('guideNext', 'cGuideNext-_genre() : !programme');
                return;
            }
            genre = programme.getGenre();
            if (genre) {
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
            if (this.isEmpty(hasGenre)) {
                genre = programme._userGenres;
                if (genre) {
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
            if (this.isEmpty(hasGenre)) {
                if ($elem[0].className.indexOf('genre') >= 0) {
                    $elem.removeClass('genre');
                }
            }
        };
        cGuideNext.prototype.moveIn = function (fnCB) {
            hx.log('guideNext', 'cGuideNext-moveIn()');
            var $timeline = $('#tvg #timeline .next').addClass('indicator'), nextStyle, objTimer = this.animation.aniTimeoutID;
            //$timeline.css('left', '0px');
            this._swapLine = this.TSwapLineType.ENONE;
            this.initKey();
            this.doMoveIn();
            this.timeline.draw();
            //this.animation.running = true;
            nextStyle = this.animation.styleMap.next.toNextIn;
            hx.log('guideNext', hx.util.format('===== [{0}][moveIn] this.$.opacity : {1} ===== ', this.name, this.$.css('opacity')));
            var $current = $('#tvg #viewArea .current'), $tvg = $('#tvg'), $viewArea = $('#viewArea');
            objTimer.next = null;
            if (this.opt) {
                $tvg.after(this.opt.$);
            }
            $viewArea.after(this._$dateIndicator).after(this.quickNavi.$);
            $timeline.css('opacity', 1);
            $current.css('opacity', 1);
            this._curGenre = this.opt ? this.opt.selectedGenre : '';
            if (fnCB) {
                fnCB();
            }
            this.$.css('opacity', 1);
        };
        cGuideNext.prototype.timelineMoveOut = function (afterCB) {
            var objTimer = this.animation.aniTimeoutID, timelineStyle;
            var $timeline = $('#tvg #timeline .next').removeClass('indicator');
            hx.log('guideNext', 'cGuideNext-timelineMoveOut()');
            $timeline.css('opacity', 0);
            this.timeline.clear();
            timelineStyle = this.animation.styleMap.timeline.toNextOut;
            objTimer.timeline = null;
            if (this.timeline !== null) {
                this.timeline.init({
                    'animation': this.animation
                });
                if (afterCB) {
                    afterCB();
                }
            }
        };
        cGuideNext.prototype.viewMoveOut = function (afterCB) {
            var objTimer = this.animation.aniTimeoutID, nextStyle;
            var $next = $('#next');
            hx.log('guideNext', 'cGuideNext-viewMoveOut()');
            nextStyle = this.animation.styleMap.next.toNextOut;
            $next.css(nextStyle.end);
            objTimer.next = null;
            if (this.opt) {
                this.opt.close();
            }
            if (this.quickNavi) {
                this.quickNavi.destroy();
            }
            if (this._$dateIndicator) {
                this._$dateIndicator.remove();
            }
            $next.css('left', this.CHLIST_WIDTH + 'px');
            this._curGenre = (this.opt) ? this.opt.selectedGenre : '';
            if (afterCB) {
                afterCB();
            }
        };
        cGuideNext.prototype.moveOut = function (fnCB) {
            var _this = this;
            hx.log('guideNext', 'cGuideNext-moveOut()');
            var execCb = function () {
                var objTimer = _this.animation.aniTimeoutID;
                if (objTimer.timeline === null && objTimer.next === null) {
                    _this.animation.running = false;
                    if (fnCB) {
                        fnCB();
                    }
                }
            };
            this.doMoveOut();
            this.timelineMoveOut();
            this.viewMoveOut(execCb);
        };
        cGuideNext.prototype._trySlide = function (key) {
            var _this = this;
            var startTime, endTime, offset = 0, $next = $('#next'), aniTimeoutID = this.animation.aniTimeoutID;
            hx.log('guideNext', 'cGuideNext-_trySlide()');
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
            if (offset < 0) {
                if (this.timeline._getCurrentTime() > this.timeline.startTime) {
                    return false;
                }
            }
            this.clear();
            this.programmes = [];
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
        cGuideNext.prototype.onChangeDateIndication = function () {
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
        cGuideNext.prototype._checkBoundary = function (data, reqSwapLine, channels) {
            if (!this.timeline) {
                hx.log('error', 'Timeline object is NULL.');
                return;
            }
            var i, j, colLen, programme, channel;
            var rowLen = data.length;
            var startTime = this.timeline.startTime, endTime = this.timeline.endTime, PADDING_TIME = this.PADDING_TIME;
            hx.log('guideNext', 'cGuideNext-_checkBoundary()');
            for (i = 0; i < rowLen; i += 1) {
                switch (reqSwapLine) {
                    case this.TSwapLineType.ENONE:
                        channel = channels[i];
                        break;
                    case this.TSwapLineType.EUP:
                        channel = channels[0];
                        break;
                    case this.TSwapLineType.EDOWN:
                        channel = channels[this.ROW_COUNT - 1];
                        break;
                }
                colLen = data[i].length;
                if (colLen === 1 && data[i][0].name === hx.l('LOC_NO_PROGRAMME_INFORMATION_ID')) {
                    data[i][0] = this.progProvider(startTime, endTime + this.PADDING_TIME, channel);
                }
                if (data[i][0].startTime > (startTime + 50)) {
                    programme = this.progProvider(startTime, data[i][0].startTime, channel);
                    data[i] = [programme].concat(data[i]);
                    colLen = data[i].length;
                }
                if ((data[i][colLen - 1].endTime + 50) < endTime) {
                    programme = this.progProvider(data[i][colLen - 1].endTime, endTime + this.PADDING_TIME, channel);
                    data[i] = data[i].concat([programme]);
                    colLen = data[i].length;
                }
                for (j = 0; j + 1 < colLen; j += 1) {
                    if (data[i][j + 1].startTime - data[i][j].endTime > 30) {
                        programme = this.progProvider(data[i][j].endTime, data[i][j + 1].startTime, channel);
                        data[i].splice(j + 1, 0, programme);
                    }
                }
            }
            return data;
        };
        return cGuideNext;
    })(__VIEW__);
    return cGuideNext;
});
