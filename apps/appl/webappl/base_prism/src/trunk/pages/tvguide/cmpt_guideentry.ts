//file name :  pages/tvguide/cGuideEntry.js
/*jslint sloppy:true, nomen:true, vars:true*/
///<reference path="../../def/sugar.d.ts" />
///<reference path="../../def/controls.d.ts" />
declare var hx;

import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");
import __GUIDE_CHANNEL_LIST__ = require("./cmpt_guidechannellist");
import __GUIDE_NEXT__ = require("./cmpt_guidenext");
import __GUIDE_BACKWARD__ = require("./cmpt_guidebackward");
import __GUIDE_GROUP_LIST__ = require("./cmpt_guidegrouplist");

class cGuideEntry extends __KEYNAVIGATOR__ {
    aniConfig = {
        use: hx.config.useAnimation,
        running: false,
        state: 'idle', // idle, sliding, scrolling, changingView
        styleMap: {
            fadeIn: {
                start: {
                    'opacity': 0
                },
                end: {
                    'opacity': 1
                }
            },
            fadeOut: {
                start: {
                    'opacity': 1
                },
                end: {
                    'opacity': 0
                }
            },
            title: {
                topOut: {
                    start: {
                        'top': 0
                    },
                    end: {
                        'top': '-176px'
                    }
                }
            },
            gItem: {
                fadeIn: {
                    start: {
                        'opacity': 0,
                        'left': '30px'
                    },
                    end: {
                        'opacity': 1,
                        'left': 0
                    }
                },
                onAirToNextIn: {
                    start: {
                        'opacity': 1/*,
                         'left' : '0px'*/  // <for optimize>
                    },
                    end: {
                        'opacity': 0/*,
                         'left' : '-594px'*/  // <for optimize>
                    }
                },
                onAirToCatchIn: {
                    start: {
                        'opacity': 1,
                        'left': '710px'
                    },
                    end: {
                        'opacity': 0,
                        'left': '1012px'
                    }
                },
                onAirToBackwardIn: {
                    start: {
                        'opacity': 1,
                        'left': '710px'
                    },
                    end: {
                        'opacity': 0,
                        'left': '1012px'
                    }
                },
                nextToOnAirIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                catchUpToOnAirIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                }
            },
            focus: {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                toOnAirIn: {
                    start: {
                        'opacity': 1,
                        'left': '560px'
                    },
                    end: {
                        'left': '260px'
                    }
                },
                toNextIn: {
                    start: {
                        'opacity': 1,
                        'left': '260px'
                    },
                    end: {
                        'left': '260px'
                    }
                },
                toCatchUpIn: {
                    start: {
                        'opacity': 1,
                        'left': '560px'
                    },
                    end: {
                        'left': '575px'
                    }
                }
            },
            timeline: {
                toNextIn: {
                    start: {
                        'opacity': 1
                    },
                    end: {
                        'left': '0px'
                    }
                },
                toNextOut: {
                    start: {
                        'left': '0px',
                        'opacity': 1
                    },
                    end: {
                        'left': '946px'
                    }
                },
                onAirToNext: {
                    start: {
                        'opacity': 1,
                        'left': '946px'
                    },
                    end: {
                        'left': '0px'
                    }
                },
                onAirToCatch: {
                    start: {
                        'left': '-300px'
                    },
                    end: {
                        'left': '0px'
                    }
                },
                onAirToBackward: {
                    start: {
                        'left': '-300px'
                    },
                    end: {
                        'left': '0px'
                    }
                },
                catchUpToOnAir: {
                    start: {
                        'left': '1200px'
                    },
                    end: {
                        'left': '0px'
                    }
                },
                moveLeft: {
                    start: {
                        'opacity': 1,
                        'left': '50px'
                    },
                    end: {
                        'left': '0px'
                    }
                },
                moveRight: {
                    start: {
                        'opacity': 1,
                        'left': '-40px'
                    },
                    end: {
                        'left': '0px'
                    }
                }
            },
            next: {
                toNextIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                toNextOut: {
                    start: {
                        'opacity': 1/*,
                         'left' : '173px'*/  // <for optimize>
                    },
                    end: {
                        'opacity': 0/*,
                         'left' : '946px'*/  // <for optimize>
                    }
                }
            },
            catchUp: {
                toCatchUpIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1,
                        'left': '0px'
                    }
                },
                catchUpToOnAir: {
                    start: {
                        'opacity': 1,
                        'left': '0px'
                    },
                    end: {
                        'opacity': 0,
                        'left': '-1000px'
                    }
                }
            },
            chList: {
                toCatchUpIn: {
                    start: {
                        'left': '710px'
                    },
                    end: {
                        'left': '1012px'
                    }
                },
                toOnAirOut: {
                    start: {
                        'left': '398px'
                    },
                    end: {
                        'left': '0'
                    }
                },
                moveUp: {
                    start: {
                        'top': '-108px'
                    },
                    end: {
                        'top': '0px'
                    }
                },
                moveDown: {
                    start: {
                        'top': '108px'
                    },
                    end: {
                        'top': '0px'
                    }
                }
            }
        },
        aniTimeoutID: {
            fadeIn: null,
            fadeOut: null,
            title: null,
            gItem: null,
            focus: null,
            timeline: null,
            next: null,
            catchUp: null,
            chList: null,
            backward: null
        }
    };
    recordings: any = {};
    //viewChangeTimer = null;
    _$wrap: JQuery;
    _destroying: boolean;
    guideChannelList: any;  // CGuideChannelList
    guideBackward: any;     // CGuideBackward
    guideOnAir: any;        // CGuideOnAir
    guideNext: any;         // CGuideNext
    sugarPage: any;
    _$tvg: JQuery;
    _$header: JQuery;
    _$view: JQuery;
    _$banner: JQuery;
    _$indicator: JQuery;
    _$timeline: JQuery;
    //_$bgCell: JQuery;
    //_$tvgDim: JQuery;
    _$current: JQuery;
    _$focus: JQuery;
    _$opt: JQuery;
    _title: any;
    _dateTime: any;
    _VIEW = {
        BACKWARD: 'guideBackward',
        ONAIR: 'guideOnAir',
        NEXT: 'guideNext'
    };
    $container: JQuery;
    _actView: any;
    _mainLayer: any;
    _detailDlg: any;
    _contextMenu: any;
    _listDlg: any;
    _btnDlg: any;
    _genreDlg: any;
    _group: any;
    _shownCellFocus: any;

    constructor(param) {
        super($('#wrap'), 'focusable', 'focused');
        this._$wrap = this._$;
        delete this._destroying;
        var p, guideDcn;
        if (param.guideDcn) {
            guideDcn = param.guideDcn;
        }
        this.guideChannelList = new __GUIDE_CHANNEL_LIST__({
            'animation': this.aniConfig
        });
        p = {
            'channelList': this.guideChannelList,
            'guideDcn': guideDcn,
            'animation': this.aniConfig,
            'recordings': this.recordings,
            'parent': this
        };
        this.guideBackward = new __GUIDE_BACKWARD__(p);
        this.guideBackward.name = this._VIEW.BACKWARD;
        this.guideNext = new __GUIDE_NEXT__(p);
        this.guideNext.name = this._VIEW.NEXT;
        if (param.sugarPage) {
            this.sugarPage = param.sugarPage;
        }
        this.create("next");
    }
    create(state?) {
        switch(state) {
            case "next":
                this._actView = this.guideNext;
                break;
            case "onair":
                //this._actView = this.guideOnAir;
                break;
            case "backward":
                this._actView = this.guideBackward;
                break;
            default:
                this._actView = this.guideNext;
                break;
        }
        this._$tvg = $('<section>', {
            'id': 'tvg',
            'class': state || "next"
        });
        this._$header = $('<div>', {
            'class': 'wrap_hgroup'
        });
        this._$view = $('<section>', {
            'id': 'viewArea',
            'style': 'height:' + this._actView.VIEW_HEIGHT + 'px;' + 'clip: rect(0px, 1182px, ' + this._actView.VIEW_HEIGHT + 'px, 0px)'
        });
        this._$banner = $('<section>', {
            'id': 'bannerArea',
            'style': 'opacity:0',
            'html': '<div class=\"title\"></div><div class=\"info\"><em class=\"episode\"></em><em class=\"season\"></em><em class=\"time\"></em><em class=\"icon\"></em></div>'
        });
        this._$indicator = $('<div>', {
            'id': 'indicator',
            'html': '<div class=\"Arrow\"></div><div class=\"indi_text\"></div>'
        });
        this._$timeline = $('<aside>', {
            'id': 'timeline',
            'class': 'indDis',
            'html': '<div class=\"next indicator\"><p class=\"date\">' + "TODAY" + '</p><ul class=\"wrap_time\"><li class=\"time first\"></li><li class=\"time second\"></li><li class=\"time third\"></li></ul>' + '</div><div class=\"backward\" ><p class=\"date\">' + "TODAY" + '</p><ul class=\"wrap_time\"><li class=\"time first\"></li><li class=\"time second\"></li></ul></div>'
        });
        this._$current = $('<div>', {
            'class': 'current',
            'style': 'opacity:0'
        });
        this._$opt = $('<section>', {
            'id' : 'opt',
            'class' : 'focusable newGuideOptBg',
            'name' : this.name
        });
        this._title = this._cmptTitle();
        // append basic DOM elements
        this._$header.append(this._title.$);
        this._$tvg
            .append(this._$header)
            .append(this._$indicator)
            .append(this._$timeline)
            .append(this._$focus)
            .append(this._$view)
            .append(this._$banner);
        var child = new CViewGroupControl(this._$tvg);
        child.setChildControl([]);
        this.$container = $('<div>').css({
            'width': 'inherit',
            'height': 'inherit'
        });
        $('#wrap').prepend(this.$container.addClass('-afd prism-page show'));
        hx.il.updatePageBackground();
        this._mainLayer = new CLayeredGroupControl(this.$container);
        this._mainLayer.createLayer({
            fnAfterCreate: function () {
                var self = this;
                self.setChildControl([child]);
                self.draw();
            }
        });
        this._$view.append(this._$current).append(this.guideChannelList.$);//.append(this._$bgCell);
        //this._$view.append(this.guideBackward.$).append(this.guideOnAir.$).append(this.guideNext.$);
        this._$view.append(this.guideBackward.$).append(this.guideNext.$);
        this.build(this.leftRightBuilder);
        //this._$tvg.after(this.guideOnAir.opt.$);
        if (this.guideNext.opt) {
            this._$tvg.after(this.guideNext.opt.$);
        }
        //this.guideOnAir.opt.goBuild();
        // add components as child
        this.add(this.guideBackward);
        //this.add(this.guideOnAir);
        this.add(this.guideNext);
        //this._actView = this.guideOnAir;

        var ulBox = document.createElement('ul');
        var divBox = document.createElement('div');
        divBox.className = 'menu';
        var $title = $('<p/>', {
            'id' : 'group',
            'class' : 'tit left',
            'text' : 'OPTION'
        });
        this._$opt.append($title);
        divBox.appendChild(ulBox);
        this._$opt.append(divBox);
        this._$tvg.after(this._$opt);
        //this.$container.append(this._$opt);
        this._actView.getChannels({
            'ccid': null,
            'group': undefined,
            'offset': 0,
            'count': this._actView.ROW_COUNT
        });
        this._actView.moveIn();
        /*
        hx.svc.EPG.addEventListener({
            'event': 'MetadataUpdate',
            'objTarget': this.guideOnAir,
            'priority': 0
        });
        */
        //this.opt = new __GUIDE_GROUP_LIST__();
        this.make_FullListOfRecordings();
        this._bindRecordingEvents();
        this._execTitleAni();
    }
    _bindRecordingEvents() {
        var dsrvRecordings = hx.svc.RECORDINGS;
        var _fnRecordingEvent = (eventName, item) => {
            if (this._actView.RecordingEvent) {
                this._actView.RecordingEvent(eventName, item);
            }
        };

        function _fnSearchItem(dataProvider, eventItem) {
            var i, ti, item, key = -1;
            for (i = 0, ti = dataProvider.getCount(); i < ti; i += 1) {
                item = dataProvider.getItem(i);
                if (item.isEqual(eventItem)) {
                    key = i;
                    break;
                }
            }
            return key;
        }
        dsrvRecordings.addEventCb('RecordingItemAdded', this, (param) => {
            var recordingProvider: any = this.recordings.recordingProvider;
            var currentRecordingList = hx.svc.RECORDINGS.getCurrentRecordings();
            hx.log('guideEntry', hx.util.format('===== [{0}][RecordingItemAdded] ===== ', this.name));
            recordingProvider.prependItem(param.item);
            this.recordings.curRecordingList = currentRecordingList;
            _fnRecordingEvent('RecordingItemAdded', param.item);
        });
        dsrvRecordings.addEventCb('RecordingCompleted', this, (param) => {
            var currentRecordingList = hx.svc.RECORDINGS.getCurrentRecordings();
            var recordingProvider = this.recordings.recordingProvider;
            var key = _fnSearchItem(recordingProvider, param.item);
            hx.log('guideEntry', hx.util.format('===== [{0}][RecordingCompleted] ===== ', this.name));
            if (key !== -1) {
                recordingProvider.updateItem(key, param.item);
            }
            this.recordings.curRecordingList = currentRecordingList;
            _fnRecordingEvent('RecordingCompleted', param.item);
        });
        dsrvRecordings.addEventCb('RecordingItemRemoved', this, (param) => {
            var recordingProvider = this.recordings.recordingProvider;
            var key = _fnSearchItem(recordingProvider, param.item);
            var currentRecordingList = hx.svc.RECORDINGS.getCurrentRecordings();
            hx.log('guideEntry', hx.util.format('===== [{0}][RecordingItemRemoved] ===== ', this.name));
            if (key !== -1) {
                recordingProvider.removeItem(key);
            }
            this.recordings.curRecordingList = currentRecordingList;
            _fnRecordingEvent('RecordingItemRemoved', param.item);
        });
        /*
        dsrvRecordings.addEventCb('RecordingItemUpdated', this, (param) => {
            var recordingProvider = this.recordings.recordingProvider;
            var key = _fnSearchItem(recordingProvider, param.item);
            if (this._actView.name === this._VIEW.CATCHUP && this._actView.state === 'idle') {
                hx.log('guideEntry', hx.util.format('===== [{0}][RecordingItemUpdated] key = {1} ===== ', this.name, key));
                recordingProvider.updateItem(key, param.item);
                _fnRecordingEvent('RecordingItemUpdated', param.item);
            }
        });
        */
        dsrvRecordings.addEventCb('ScheduledItemAdded', this, (param) => {
            hx.log('guideEntry', hx.util.format('===== [{0}][ScheduledItemAdded] ===== ', this.name));
            if (this._actView.name === this._VIEW.NEXT) {
                _fnRecordingEvent('ScheduledItemAdded', param.item);
                if (this._actView.getGenreDlg()) {
                    this._actView.getGenreDlg().onScheduledItemAdded(param.item);
                }
            }
        });
        dsrvRecordings.addEventCb('ScheduledItemRemoved', this, (param) => {
            hx.log('guideEntry', hx.util.format('===== [{0}][ScheduledItemRemoved] ===== ', this.name));
            if (this._actView.name === this._VIEW.NEXT) {
                _fnRecordingEvent('RecordingItemRemoved', param.item);
                if (this._actView.getGenreDlg()) {
                    this._actView.getGenreDlg().onScheduledItemRemoved(param.item);
                }
            }
        });
    }
    destroy(cb?) {
        this._destroying = true;
        if (this._detailDlg) {
            this._detailDlg.close();
            delete this._detailDlg;
        }
        if (this._shownCellFocus) {
            this._shownCellFocus = null;
        }
        if (this._contextMenu) {
            this._contextMenu.close();
            this._contextMenu = null;
        }
        if (this._listDlg) {
            this._listDlg.close();
            this._listDlg = null;
        }
        if (this._mainLayer) {
            while (this._mainLayer.stackSize()) {
                this._mainLayer.removeLayer();
            }
        }
        if (this.$container) {
            this.$container.remove();
        }
        if (this._actView) {
            this._actView.clear();
            this._actView = null;
        }
        if (this._$opt) {
            this._$opt.remove();
            this._$opt = null;
        }
        //if (this.viewChangeTimer !== null) {
        //    clearTimeout(this.viewChangeTimer);
        //    this.viewChangeTimer = null;
        //}
        //hx.svc.EPG.removeEventListener(this.guideOnAir);
        hx.svc.RECORDINGS.clearEventCb(this);
        this.hide(() => {
            this._$tvg.remove();
            this._clearAnimationConfig();
            this._childDestroy();
            if (cb) {
                cb();
            }
        });
    }
    show() {
        var $tvg: JQuery = $('#tvg'), $opt: JQuery = $('#opt'), cfg = this.aniConfig;
        $tvg.css('opacity', 1);
        $opt.css('opacity', 1);
    }
    hide(aCallback) {
        var $tvg: JQuery = $('#tvg'), $opt: JQuery = $('#opt'), cfg = this.aniConfig;
        $opt.css('opacity', 0);
        $tvg.css('opacity', 0);
        if (aCallback) {
            aCallback();
        }
    }
    isEmpty(aValue) {
        return ( typeof aValue === undefined || aValue === null || aValue === undefined || aValue.length === 0);
    }
    onFocusChanged($old, $new) {
        var oldViewName: string = $old.attr('name'), newViewName: string = $new.attr('name'), oldView = this[oldViewName], newView = this[newViewName];
        var moveInCB = () => {
            console.debug('newView: ' + newViewName);
            oldView.clear();
        };
        var moveOutCB = () => {
            console.debug('newView: ' + name);
            if (oldViewName === this._VIEW.NEXT || oldViewName === this._VIEW.BACKWARD) {
                this._$tvg[0].className = $new.attr('id');
            }
            //newView.moveIn(moveInCB);
            newView.moveIn();
        };
        if (oldView && newView) {
            //$('#focus').css('opacity', 0);
            if (oldViewName !== this._VIEW.NEXT && oldViewName !== this._VIEW.BACKWARD) {
                this._$tvg[0].className = $new.attr('id');
            }
            /* update current view */
            this._actView = newView;
            this._$view.append(newView.$);
            oldView.clear();
            oldView.moveOut(moveOutCB, newViewName);
        }
    }
    make_FullListOfRecordings() {
        var recordingItemList = hx.svc.RECORDINGS.getRecordingList();
        var currentRecordingList = hx.svc.RECORDINGS.getCurrentRecordings();
        var recordingProvider = new CDataProvider(recordingItemList);
        hx.log('guideEntry', hx.util.format('===== [{0}][make_FullListOfRecordings] ===== ', this.name));
        this.recordings.recordingProvider = recordingProvider;
        this.recordings.curRecordingList = currentRecordingList;
    }
    doChangeView(aParam) {
        var titleTimer = this.aniConfig.aniTimeoutID.title, $tvgTitle = $('#tvg .wrap_hgroup'), retValue = true;
        if (titleTimer !== null) {
            clearTimeout(titleTimer);
            this.aniConfig.aniTimeoutID.title = null;
            $tvgTitle.css('top', '-176px');
        }
        //if (!this.aniConfig.running && this.viewChangeTimer === null) {
        //    retValue = this.doNavi(aParam);
            //if (retValue) {
            //    this.viewChangeTimer = setTimeout(() => {
            //        this.viewChangeTimer = null;
            //    }, 500/*2000*/);
            //}
        //}
        //return retValue;
        return this.doNavi(aParam);
    }
    doInitView() {
        if (this._actView.timeline) {
            this._actView.timeline.init({
                'animation': this._actView.animation
            });
            this._actView.timeline.draw();
        }
        this.initKey();
        this._actView.clear();
        this._actView.getProgrammes({});
        //this._actView.moveIn();
        return true;
    }
    isPossible_ApproachCatchupView() {
        var $timeline = $(document.getElementById('timeline')), isPossible = true;
        if ($timeline.hasClass('indDis')) {
            isPossible = false;
        }
        return isPossible;
    }
    checkNextViewTimeline() {
        var actView = this._actView, now = new Date(), curTime;
        curTime = (parseInt(now.getTime() / 1000 + "", 10));
        return (curTime > actView.timeline.startTime);
    }
    dcnClicked(param) {
        var i, length, ccid, group, type, channel, channels = this.guideChannelList.channels, doSearchFlag = true;
        hx.log('guideEntry', hx.util.format('===== [{0}][dcnClicked] ===== ', this.name));
        ccid = param.alData.ccid;
        if (ccid) {
            group = param.alData.group;
            for (i = 0, length = channels.length - 1; i < length; i += 1) {
                channel = channels[i];
                if (channel.ccid === ccid && group === this.guideChannelList.channelGroup) {
                    var focusRow = this._actView.focusInfo.$item.attr('row');
                    if (i !== Number(focusRow)) {
                        var $new = $(this._actView.rowList[i].element[0]);
                        var $old = this._actView.focusInfo.$item;
                        //var focused = (this._actView.name === this._VIEW.NEXT) ? 'onFocus' : 'on';
                        var focused = 'on';
                        $old.removeClass(focused);
                        $new.addClass(focused);
                        this._actView.onFocusChanged($old, $new);
                    }
                    doSearchFlag = false;
                    break;
                }
            }
            if (doSearchFlag) {
                this.guideChannelList.clear();
                if (this.guideChannelList.channelGroup !== group) {
                    this.guideChannelList.changeChannelGroup(group);
                    if (this._actView.opt && this._actView.opt.changeGroupForced) {
                        this._actView.opt.changeGroupForced(group);
                    }
                }
                this._actView.clear();
                this._actView.getChannels({
                    ccid: ccid,
                    group: group,
                    offset: 0,
                    count: this._actView.ROW_COUNT
                });
            }
        }
    }
    _on_alKey(param) {
        var alKey = param.alKey, isConsumed = false;
        var currentViewName = this._actView.name;
        switch (alKey) {
            case hx.key.KEY_LEFT:
                if (currentViewName === this._VIEW.NEXT) {
                    isConsumed = true;
                    if (!this._actView._on_alKey(param)) {
                        function createPopupCannotChangeView() {
                            hx.msg('confirm', {
                                text : hx.l('LOC_CATCH_UP_INTERNET_CONFIRM_MSG_ID'),
                                auto_close : false,
                                btn_title_arr : [hx.l('LOC_OK_ID')],
                                dia_class : 'osd'
                            });
                        }
                        if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                                console.debug('this.isPossible_ApproachCatchupView() true');
                                this.doChangeView(param);
                        } else {
                            if (hx.svc.SETTING_UTIL.getNetworkAvailable()) {
                                hx.svc.SETTING_UTIL.checkInternetConnection();
                            }
                            createPopupCannotChangeView();
                        }
                    }
                }
                break;
            case hx.key.KEY_RIGHT:
                if (currentViewName === this._VIEW.BACKWARD) {
                    isConsumed = true;
                    if (!this._actView._on_alKey(param)) {
                        this.doChangeView(param);
                    }
                } else if (currentViewName === this._VIEW.ONAIR) {
                    isConsumed = this.doChangeView(param);
                }
                break;
            case hx.key.KEY_BACK:
                if (this._actView.state === 'idle') {
                    if (currentViewName === this._VIEW.NEXT) {
                        isConsumed = this.doInitView();
                    } else if (currentViewName === this._VIEW.BACKWARD) {
                        this.initKey();
                        param.alKey = hx.key.KEY_RIGHT;
                        isConsumed = this.doChangeView(param);
                    }
                }
                break;
            case hx.key.KEY_OK:
                if ((currentViewName === this._VIEW.NEXT || currentViewName === this._VIEW.BACKWARD) && this._actView.state === 'idle') {
                    this._showProgrammeDetail();
                    isConsumed = true;
                }
                break;
            case hx.key.KEY_OPTION:
                if (!this._contextMenu) {
                    if (!this._actView.quickNavi.getVisible() && !this._actView.isGenreDlgActivated()) {
                        //this._optBgShow();
                        this._openOptListMenu();
                        isConsumed = true;
                    }
                }
                break;
            case hx.key.KEY_ESCAPE:
                if (this._contextMenu) {
                    this._contextMenu.close();
                    isConsumed = true;
                }
                break;
        }
        return (isConsumed || this._actView._on_alKey(param));
    }
    _on_alEndOfRepeatKey(aParam) {
        return this._actView._on_alEndOfRepeatKey(aParam);
    }
    processProgrammeDetail(param?) {
        var isConsumed = false;
        var currentViewName = this._actView.name;
        if ((currentViewName === this._VIEW.NEXT || currentViewName === this._VIEW.BACKWARD)) {
            var item : any = param ? param.item : null;
            var metadata : any = item ? item.getMetadata() : null;
            var programme = metadata || this._actView.getCurrentProgramme();
            if (programme) {
                this._showProgrammeDetail(programme);
            }
        }
        return isConsumed;
    }
    processRecord(param) {
        var targetProg = param ? param.item : null;
        if (!targetProg || !targetProg.getPluginObject()) {
            hx.log('error', '[GUIDE] processRecord() - (!targetProg || !targetProg.getPluginObject())');
            return;
        }
        var that = this;
        if (that._actView.name === that._VIEW.NEXT) {
            if (targetProg.hasSchedule() || targetProg.hasReminder()) {
                that._actView.cancelReservation(targetProg);
            } else if (targetProg.hasSeriesSchedule()) {
                that._actView.cancelSeriesReservation(targetProg);
            } else {
                if (targetProg.getGroupCRIDs() && targetProg.getGroupCRIDs().length) {
                    that._actView.selectRecordType(targetProg);
                } else {
                    that._actView.doReservation(targetProg);
                }
            }
            return true;
        }
        return false;
    }
    getCurrentView() {
        return this._actView;
    }
    isLiveChannelClicked(cmpGroup) {
        var focusedChannel = this.guideChannelList.channels[this.guideChannelList.currentRowIndex], channelGroup = this.guideChannelList.channelGroup;
        var chinfo = hx.svc.CH_LIST.getCurrentChannel();
        cmpGroup = (cmpGroup) ? (chinfo.group === channelGroup) : true;
        return (chinfo.channel.ccid === focusedChannel.ccid && cmpGroup);
    }
    isLiveProgClicked(cmpGroup) {
        var focusedChannel = this.guideChannelList.channels[this.guideChannelList.currentRowIndex], channelGroup = this.guideChannelList.channelGroup;
        var chinfo = hx.svc.CH_LIST.getCurrentChannel();
        cmpGroup = (cmpGroup) ? (chinfo.group === channelGroup) : true;
        return (chinfo.channel.ccid === focusedChannel.ccid && cmpGroup);
    }
    getCurrentChannelGroup() {
        return this.guideChannelList.channelGroup;
    }
    getCurrentChannel() {
        var focusedChannel = this.guideChannelList.channels[this.guideChannelList.currentRowIndex];
        return focusedChannel;
    }
    getCurrentFocusProgramme() {
        var focusedProgramme = this._actView.focusInfo.programme;
        return focusedProgramme;
    }
    setHistoryFocus() {
        if (this._actView.clearStayedFocusTimer) {
            this._actView.clearStayedFocusTimer();
        }
    }
    showCellFocus() {
        if (this._shownCellFocus) {
            this._shownCellFocus.addClass('on');
            this._shownCellFocus = null;
        }
    }
    hideCellFocus() {
        var shownCellFocus = $('#tvg #viewArea .pInfo.on');
        if (shownCellFocus && shownCellFocus.length) {
            shownCellFocus.removeClass('on');
            this._shownCellFocus = shownCellFocus;
        }
    }
    showDetailInfo(programmeDetailItem, waitingDlg? : Q.Deferred<any>){
        //hx.svc.FRONT_PANEL.setFrontPanelText('-TV GUIDE-');
        var self = this;
        var query = {
            layeredGroupControl: self._mainLayer,
            sugarPage: self.sugarPage,
            pdeItem: programmeDetailItem,
            waitingDlg : waitingDlg,
            fnDoShow: () => {
                if (!self._destroying) {
                    self._mainLayer._$self.show();
                }
            },
            fnDoHide: () => {
                this._mainLayer._$self.hide();
            },
            onClosed: () => {
                hx.log('guideEntry', 'detailDlg.onClosed');
                if (self._actView.getGenreDlg()) {
                    self._actView.getGenreDlg().show({
                        dimm: '-full-black'
                    });
                }
                self._actView.setShowDetail(false);
                delete self._detailDlg;
                self._detailDlg = null;
            }
        };
        if (self._actView.getGenreDlg()) {
            self._actView.getGenreDlg().hide({
                bKeepData : true
            });
        }
        self._detailDlg = hx.il.createProgrammeDetailForSugar(query);
    }
    _showProgrammeDetail(aProgramme?) {
        var self = this;
        function fnShowDetailInfo(programmeDetailItem, waitingDlg? : Q.Deferred<any>){
            self._actView.setShowDetail(true);
            var query = {
                layeredGroupControl: self._mainLayer,
                sugarPage: self.sugarPage,
                pdeItem: programmeDetailItem,
                waitingDlg : waitingDlg,
                channelGroup: self.guideChannelList.channelGroup,
                fnDoShow: () => {
                    if (!self._destroying) {
                        if (self._mainLayer && self._mainLayer._$self) {
                            self._mainLayer._$self.show();
                        }
                    }

                },
                fnDoHide: () => {
                    if (self._mainLayer && self._mainLayer._$self) {
                        self._mainLayer._$self.hide();
                    }
                },
                onClosed: () => {
                    hx.log('guideEntry', 'detailDlg.onClosed');
                    if (self._actView && self._actView.getGenreDlg()) {
                        self._actView.getGenreDlg().show({
                            dimm : '-full-black'
                        });
                    }
                    if (self._actView) {
                        self._actView.setShowDetail(false);
                    }
                    delete self._detailDlg;
                    self._detailDlg = null;
                }
            };
            if (self._actView.getGenreDlg()) {
                self._actView.getGenreDlg().hide({
                    bKeepData : true
                });
            }
            self._detailDlg = hx.il.createProgrammeDetailForSugar(query);
        };
        function fnMakeDetailInfo(sourceProgramme) {
            var waitingDlgDef = hx.il.openWaitingDlg(self._mainLayer);//open Waiting Dialog
            if (sourceProgramme.getProgrammeCridMds()) {
                setTimeout(function(){
                    hx.svc.IP_EPG.getDetailInfo(sourceProgramme.getProgrammeCridMds()).then((info) => {
                        sourceProgramme.updateMdsData('program', info.querySelector('ProgramInformation'));
                        fnShowDetailInfo(hx.svc.DETAIL_INFO.createProgrammeItem(sourceProgramme), waitingDlgDef);
                    }, (err) => {
                        fnShowDetailInfo(hx.svc.DETAIL_INFO.createProgrammeItem(sourceProgramme), waitingDlgDef);
                    });
                });
            } else {
                fnShowDetailInfo(hx.svc.DETAIL_INFO.createProgrammeItem(sourceProgramme), waitingDlgDef);
            }
        }
        var programme = aProgramme || this.getCurrentFocusProgramme();
        if (this.isEmpty(programme) || this.isEmpty(programme.getProgrammeID())) {
            return true;
        }
        fnMakeDetailInfo(programme);
    }
    _on_ChildNotify(param) {
        var action;
        action = param.action;
        if (this[action]) {
            this[action](param);
        }
    }
    _openOptListMenu() {
        var that = this;
        var selectedSubItemName = '';
        var makeChGroupListItems = function() {
            var channelGroupItems = [];
            var channelGroupList = hx.svc.CH_LIST.getChannelGroupList();
            var i, channelGroup, length = channelGroupList.length;
            for ( i = 0; i < length; i += 1) {
                if (channelGroupList[i].id === 'AGALL') {
                    continue;
                }
                channelGroup = channelGroupList[i];
                channelGroupItems.push({
                    name : channelGroup.name,
                    id : channelGroup.id
                });
            }
            return channelGroupItems;
        };
        var menuItems = [{
            name : "CHANGE GROUP",
            noclose : true,
            redraw : false,
            cssClass : 'optCg',
            type : 'normal'
        }, {
            name : "GENRE SEARCH",
            noclose : true,
            redraw : false,
            cssClass : 'optGenre',
            type : 'normal'
        }, {
            name : "JUMP TO DATE",
            noclose : true,
            redraw : false,
            cssClass : 'optJumpDay',
            type : 'normal'
        }];
        that.hideCellFocus();
        that._$opt.css('z-index', 3);
        that._actView.state = that._actView.EVIEW_STATE.EBUSY;
        that._contextMenu = prism.createContextMenu(that._mainLayer, {
            text : '',
            cssClass : 'newGuideOpt ' + that._actView.name,
            menuItems : menuItems
        }, function(param) {
            selectedSubItemName = menuItems[param.index].name;
            switch (selectedSubItemName) {
                case "CHANGE GROUP" :
                    var channelGroupItems = makeChGroupListItems(), i, curGroupIndex;
                    for ( i = 0; i < channelGroupItems.length; i++) {
                        if (channelGroupItems[i].id === that.guideChannelList.channelGroup) {
                            curGroupIndex = i;
                            break;
                        }
                    }
                    that._listDlg = prism.createListDlg(null, {
                        groupItems : channelGroupItems,
                        textHtml : 'Select a group to list the channels thereof.',
                        cls : 'onChList'
                    }, function(param) {
                        if (param && param.id) {
                            that.closeChannelGroupOption(param);
                        }
                    });
                    that._listDlg.open({
                        curGroupIndex : curGroupIndex,
                        onClosed : function(param) {
                            that._listDlg = null;
                            //that._optBgHide();
                            that.showCellFocus();
                            if (that._actView) {
                                that._actView.state = that._actView.EVIEW_STATE.EIDLE;
                            }
                        },
                    });
                    that._listDlg.doKeyEscape = function() {
                        return that._listDlg.doKeyBack();
                    };
                    that._contextMenu.close();
                    break;
                case "GENRE SEARCH" :
                    that._contextMenu.close();
                    //that.initGenreList();
                    //that._genreDlg = prism.createGenreDlg(that._mainLayer, that._genreList);
                    //that.hide();
                    //that._genreDlg.open({
                    //    onClosed : function() {
                    //        that.show();
                    //        //that._optBgHide();
                    //        //that._actView.setFocus();
                    //        that._actView.state = that._actView.EVIEW_STATE.EIDLE;
                    //        that._genreDlg = null;
                    //        that._actView._allProgrammes = null;
                    //    }
                    //});
                    //that._genreDlg.doKeyEscape = function() {
                    //    return that._genreDlg.doKeyBack();
                    //};
                    that._actView.showGenreOption();
                    break;
                case "JUMP TO DATE":
                    that._contextMenu.close();
                    //that._optBgHide();
                    that._actView.showQuickNavi();
                    break;
                default:
                    break;
            }
        });
        that._contextMenu.setDisplayDepth(2);
        that._contextMenu.open({
            onClosed : function(param) {
                var globalDialogOpened = prism.isGlobalDialogOpened();
                that._contextMenu = null;
                //that._optBgHide();
                if (!globalDialogOpened) {
                    if (!selectedSubItemName) {
                        that.showCellFocus();
                    }
                    if (that._actView) {
                        that._actView.state = that._actView.EVIEW_STATE.EIDLE;
                    }
                }
                that._$opt.css('z-index', 0);
            },
        });
    }
    _optBgShow() {
        var that = this;
        //$('#focus').css('opacity', 0);
        if (that._$opt) {
            that._$opt.addClass('newUi');
        }
        that._actView.state = that._actView.EVIEW_STATE.EBUSY;
    }
    _optBgHide() {
        var that = this;
        if (that._$opt && that._$opt.hasClass('newUi')) {
            that._$opt.removeClass('newUi');
        }
    }
    closeChannelGroupOption(param) {
        var that = this;
        var group = param.id, channelSearchParam;
        that._actView.state = that._actView.EVIEW_STATE.EIDLE;
        that._actView._swapLine = that._actView.TSwapLineType.ENONE;
        channelSearchParam = that.guideChannelList.changeChannelGroup(group);
        if (channelSearchParam) {
            that._actView.freeMem(that._actView.programmes);
            that._actView.programmes = [];
            // clear viewArea
            that._actView.clear();
            that.guideChannelList.clear();
            that._actView.getChannels(channelSearchParam);
        }
        if (that._actView.name == 'guideOnAir') {
            that._actView.changeChannel();
        }
        //that._setTitleInnerHTML();
    }
    _setTitleInnerHTML(genre) {
        var that = this;
        var strGroup = hx.l('LOC_OPTION_ID');
        var dispGroup = 'TV';//, i, channelGroupList = hx.dl.dsrvChlist.getChannelGroupList();
        //that._title.html(strGroup + ' - <em>' + dispGroup + '</em>');
    }
    /**
     * private : make the title 'TV GUIDE'
     * @param  {}
     * @return {}
     */
    _cmptTitle() {
        return hx.al.component('text', {
            'tag': 'h1',
            'text': hx.l('LOC_CTVGUIDE_ID'),
            'attr': {
                'data-langID': false,
                'class': 'on'
            }
        });
    }
    /**
     * private : 3sec timeout title and dateTime
     * @param  {}
     * @return {}
     */
    _execTitleAni() {
        var $tvgTitle = $('#tvg .wrap_hgroup'), cfg = this.aniConfig;
        if (cfg.use) {
            cfg.aniTimeoutID.title = $tvgTitle.ani({
                'style': cfg.styleMap.title.topOut,
                'trans': {
                    'dur': 500,
                    'delay': 2500
                },
                'cb': function () {
                    cfg.aniTimeoutID.title = null;
                }
            });
        } else {
            $tvgTitle.css('opacity', 1);
        }
    }
    _clearAnimationConfig() {
        var prop, objTimeOut = this.aniConfig.aniTimeoutID;
        this.aniConfig.running = false;
        this.aniConfig.state = 'idle';
        for (prop in objTimeOut) {
            if (objTimeOut.hasOwnProperty(prop)) {
                if (objTimeOut[prop] !== null) {
                    clearTimeout(objTimeOut[prop]);
                    objTimeOut[prop] = null;
                }
            }
        }
    }
    _childDestroy() {
        if (this.guideBackward) {
            this.guideBackward.destroy();
            delete this.guideBackward;
            this.guideBackward = null;
        }
        if (this.guideOnAir) {
            this.guideOnAir.destroy();
            delete this.guideOnAir;
            this.guideOnAir = null;
        }
        if (this.guideNext) {
            this.guideNext.destroy();
            delete this.guideNext;
            this.guideNext = null;
        }
        if (this.guideChannelList) {
            this.guideChannelList.destroy();
            delete this.guideChannelList;
            this.guideChannelList = null;
        }
        if (this.recordings) {
            //this.recordings.list = [];
            if (this.recordings.recordingProvider) {
                this.recordings.recordingProvider.destroy();
                this.recordings.recordingProvider = null;
            }
            this.recordings.curRecordingList = null;
        }
        this.clear();
    }
    alDialogRequested(param) {
        this.sugarPage._on_alDialogRequested(param);
    }
}
export = cGuideEntry;
