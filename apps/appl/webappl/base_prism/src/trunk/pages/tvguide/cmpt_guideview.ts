//file name : pages/tvguide/cGuideView.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../def/sugar.d.ts" />
///<reference path="../../layer_domain/service/ip_epg/adapter.d.ts" />

declare var hx;
import __MODEL__ = require('model/model');
import __PROVIDER__ = require("pages/util/provider");
import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");
import __PRISM_CATEGORY__ = require("prismux/prism.category");
import __POOL__ = require("./object_pool");

interface IScheduleParam{
    schedules : any;
    crids : any;
    index : any;
}

class cGuideView extends __KEYNAVIGATOR__ {
    ROW_COUNT = 7;
    ROW_HEIGHT = 62;    // including SPACE
    SPACE = 4;   // Border
    VIEW_HEIGHT = this.ROW_HEIGHT * this.ROW_COUNT - this.SPACE;
    HOUR_WIDTH = 480;
    MAX_FOCUS_LEFT = this.HOUR_WIDTH * 2;
    CHLIST_WIDTH = 200; // including SPACE
    VIEW_WIDTH = 982;
    CELL_PADDING = 12;
    ANIMATION_EFFECT: boolean = false;
    TRatingType = [undefined, undefined, "p", "p", "c", "c", "g", "g", "pg", "pg", "m", "m", "ma", "ma", "av", "r", "multi"];
    TCasType = ['fta', 'na', 'ir', 'nd', 'va', 'cx', 'mg', 'cr'];
    defaultRowIndex = 0;
    focusInfo: any = {};
    lastSearchId: any = null;
    state: string = 'idle';
    _stopScrollTimerID: any = null;
    _showDetail: boolean = false;
    _receiveCallback: boolean = true;
    box: any;
    EDATA_TYPE = {
        EPROGRAMME: 0,
        ECHANNEL: 1
    };
    EVIEW_STATE = {
        EIDLE: 'idle',
        EBUSY: 'busy',
        EHIDE: 'hide'
    };
    TSwapLineType = {
        ENONE: 0,
        EUP: 1,
        EDOWN: 2
    };
    // minimum value has to be bigger than 1!! (0 is invalid)
    TGenreTarget = {
        EAll : 1,
        EChildren : 2,
        EDrama : 3,
        EEducation : 4,
        EEntertainment : 5,
        ELifestyle : 6,
        EMovie : 7,
        ENews : 8,
        ESport : 9,
    };
    SCategory: __PRISM_CATEGORY__.ICategoryItem[] = [
        {
            targetType : this.TGenreTarget.EAll,
            text : 'ALL'
        }, {
            targetType : this.TGenreTarget.EChildren,
            text : 'CHILDREN'
        }, {
            targetType : this.TGenreTarget.EDrama,
            text : 'DRAMA'
        }, {
            targetType : this.TGenreTarget.EEducation,
            text : 'EDUCATION'
        }, {
            targetType : this.TGenreTarget.EEntertainment,
            text : 'ENTERTAINMENT'
        }, {
            targetType : this.TGenreTarget.ELifestyle,
            text : 'LIFESTYLE'
        }, {
            targetType : this.TGenreTarget.EMovie,
            text : 'MOVIE'
        }, {
            targetType : this.TGenreTarget.ENews,
            text : 'NEWS&FACTUAL'
        }, {
            targetType : this.TGenreTarget.ESport,
            text : 'SPORT'
        }
    ];
    rowList: any;
    programmes: any;
    tmpProgrammes: any;
    allProgrammes: any;
    channelList: any;
    animation: any;
    _showFocusTimerID: any;
    recordings: any;
    guideDcn: any;
    _isShowFocus: boolean;
    dcn: any;
    currTime: any;
    timeline: any;
    _swapLine: any;
    _genreDlg: any; // CDlgGenre
    _detailDlg: any;
    _genreList: any;
    _onGoingRepeat: boolean;
    _prevScrollParam: any;
    _parent: any;
    constructor($, focusable, focused, param) {
        super($, focusable, focused);
        //hx.logadd('guideView');
        hx.log('guideView', hx.util.format('===== [{0}][init] ===== ', this.name));
        this.rowList = [];
        this.programmes = [];
        this.allProgrammes = [];
        this.tmpProgrammes = [];
        this.channelList = null;
        this.animation = null;
        this._genreDlg = null;
        this._genreList = [
            tr('LOC_ALL_ID'),
            tr('LOC_CHILDRENS_ID'),
            tr('LOC_GENRE_AUS_DRAMA_ID'),
            tr('LOC_GENRE_EDUCATION_ID'),
            tr('LOC_GENRE_AUS_ENTERTAINMENT_ID'),
            tr('LOC_LIFESTYLE_ID'),
            tr('LOC_GENRE_MOVIE_ID'),
            tr('LOC_GENRE_NEWS_AND_FACTUAL_ID'),
            tr('LOC_GENRE_AUS_SPORT_ID')];
//        this._setPrismAndPrismLiteMode();
        if (param.channelList) {
            this.channelList = param.channelList;
        }
        if (param.animation) {
            this.animation = param.animation;
        }
        if (param.recordings) {
            this.recordings = param.recordings;
        }
        if (param.guideDcn) {
            this.guideDcn = param.guideDcn;
        }
        if (param.parent) {
            this._parent = param.parent;
        }
    }
    destroy() {
        hx.log('guideView', hx.util.format('===== [{0}][destroy] ===== ', this.name));
        this.lastSearchId = '';
        if (this.programmes) {
            this.freeMem(this.programmes);
            this.programmes = [];
        }
        this.rowList = [];
        this._receiveCallback = false;
        if (this._showFocusTimerID !== null) {
            clearTimeout(this._showFocusTimerID);
            this._showFocusTimerID = null;
        }
        if (this._stopScrollTimerID !== null) {
            clearTimeout(this._stopScrollTimerID);
            this._stopScrollTimerID = null;
        }
        if (this._parent) {
            this._parent = null;
        }
        if (this._detailDlg) {
            this._detailDlg.destroy();
            delete this._detailDlg;
            this._detailDlg = null;
        }
        if (this._genreDlg !== null) {
            if (this._genreDlg.getParentLayer() && this._genreDlg.getParentLayer().$layer()) {
                this._genreDlg.getParentLayer().removeLayer();
            }
            this._genreDlg.destroy();
            delete this._genreDlg;
            this._genreDlg = null;
        }
        if (this.focusInfo) {
            if (this.focusInfo.programme){
                this.focusInfo.programme.pluginObject = null;
                delete this.focusInfo.programme;
            }
            this.focusInfo = null;
        }
        if (this.channelList) {
            delete this.channelList;
            this.channelList = null;
        }
        if (this.tmpProgrammes) {
            delete this.tmpProgrammes;
            this.tmpProgrammes = null;
        }
        if (this.allProgrammes) {
            delete this.allProgrammes;
            this.allProgrammes = null;
        }
        if (this.programmes) {
            delete this.programmes;
            this.programmes = null;
        }
        if (this.recordings) {
            delete this.recordings;
            this.recordings = null;
        }
        if (this.rowList) {
            delete this.rowList;
            this.rowList = null;
        }

        super.destroy();
    }
    hasClassForDom(el, clss) {
        return el.classList.contains(clss);
        //return el.className && new RegExp("(^|\\s)" + clss + "(\\s|$)").test(el.className);
    }
    isEmpty(value) {
        return ( typeof value === undefined || value === null || value === undefined || value.length === 0);
    }
    minMax(value) {
        var maxValue = this.ROW_COUNT - 1;
        var minValue = 0;
        return (value > maxValue ? maxValue : (value < minValue ? 0 : value));
    }
    setShowDetail(isShow: boolean) {
        this._showDetail = isShow;
    }
    getChannelType(channel) {
        return {
            'TV': 0,
            'RADIO': 1
        }[channel.type];
    }
    getChannelGroup(channel) {
        return {
            0: 'TV',
            1: 'RADIO'
        }[channel.channelType];
    }
    completeDownload(logoURL) {
        var imgObj;

        hx.log('guideView', hx.util.format('===== [{0}][completeDownload] ===== ', this.name));

        imgObj = new Image();
        if (logoURL) {
            imgObj.src = logoURL;
        }

        return (imgObj.height > 0 && imgObj.width > 0);
    }
    progProvider(iStartTime, iEndTime, iChannel) {
        var objProgramme, name = hx.l('LOC_NO_PROGRAMME_INFORMATION_ID');
        hx.log('guideView', hx.util.format('===== [{0}][progProvider] ===== ', this.name));

        objProgramme = new __PROVIDER__.CProgrammeProvider(null);
        objProgramme.setProgrammeName(name);

        if (!this.isEmpty(iChannel)) {
            objProgramme.setChannelInformation(iChannel);
        }

        if (!this.isEmpty(iStartTime)) {
            objProgramme.setStartTime(iStartTime);
        }

        if (!this.isEmpty(iEndTime)) {
            objProgramme.setEndTime(iEndTime);
        }

        return objProgramme;
    }
    srvDataToUi(srvDatas: __PROVIDER__.CChannelProvider[], reqCount, type, channels?) {
        var uiDatas, i, data, length;

        hx.log('guideView', hx.util.format('===== [{0}][srvDataToUi] ===== ', this.name));

        uiDatas = (this.isEmpty(srvDatas)) ? [] : srvDatas.slice(0);

        for (i = 0, length = uiDatas.length; i < length; i += 1) {
            if (uiDatas[i].length === 0) {
                data = (type === this.EDATA_TYPE.EPROGRAMME) ? this.progProvider(null, null, channels ? channels[i] : this.channelList.channels[i]) : [];
                uiDatas[i].push(data);
            }
        }
        return uiDatas;
    }
    uiDataToSrv(data) {
        var i, length, result = [];

        hx.log('guideView', hx.util.format('===== [{0}][uiDataToSrv] ===== ', this.name));

        for (i = 0, length = data.length; i < length; i += 1) {
            if (data[i].ccid) {
                result.push(data[i]);
            }
        }

        return result;
    }
    doMoveIn() {
        hx.log('guideView', hx.util.format('===== [{0}][doMoveIn] ===== ', this.name));
        this.state = this.EVIEW_STATE.EIDLE;
        this.freeMem(this.programmes);
        this.programmes = [];
        this.getProgrammes({});
        if (false) {//this.name !== 'guideCatchUp') {
            hx.svc.EPG.addEventListener({
                'event': 'MetadataUpdate',
                'objTarget': this,
                'priority': 0
            });
        }

    }
    doMoveOut() {
        var clearFocus = () => {
            var tID = this.animation.aniTimeoutID.focus;
            if (tID !== null) {
                clearTimeout(tID);
                this.animation.aniTimeoutID.focus = null;
            }
        }
        hx.log('guideView', hx.util.format('===== [{0}][doMoveOut] ===== ', this.name));
        this.state = this.EVIEW_STATE.EHIDE;
        clearFocus();
        this.updateBanner(false);
        if (false) {//this.name !== 'guideCatchUp') {
            hx.svc.EPG.removeEventListener(this);
        }
    }
    doDraw(isEnablePopUPFocus?, updateFlag?, bDontUpdateKeyBuilder?) {
        var styleMap = this.animation.styleMap, aniTimeout = this.animation.aniTimeoutID;
        hx.log('guideView', hx.util.format('===== [{0}][doDraw] updateFlag : {1} ===== ', this.name, updateFlag));
        if (!bDontUpdateKeyBuilder || bDontUpdateKeyBuilder !== true) {
            this.build(this.positionBuilder);
        }
        if (!updateFlag) {
            this.initFocus();
            this.animation.running = false;
            hx.log('guideView', '[GOGO] show!! directly');
            this.$.css(styleMap.fadeIn.end);
        } else {
            this.focusInfo.$item.addClass(this._focused);
            this.setFocus(this.focusInfo.$item);
        }
    }
    doNavi(param): boolean {
        var focusInfo = this.focusInfo;
        if (focusInfo && focusInfo.$item) {
            var el = focusInfo.$item[0];
            if (el) {
                var next = this._$focusable[el[param.alKey]];
                if (next) {
                    this.onFocusChanged(focusInfo.$item, $(next));
                    return true;
                }
            }
        }
        return false;
    }
    doClear() {
        var focused = this._focused;
        hx.log('guideView', hx.util.format('===== [{0}][doClear] ===== ', this.name));
        this.$.find('.' + focused).removeClass(focused);
    }
    cellRenewal(aHasThumbnail, item) {
        var r, c, rl, cl, row, col, focus, isFake, programme, channel;
        hx.log('guideView', hx.util.format('===== [{0}][cellRenewal] ===== ', this.name));
        if (item) {
            for (r = 0, rl = this.channelList.channels.length; r < rl; r += 1) {
                channel = this.channelList.channels[r];
                if ((channel.ccid === item.getChannelId()) && this.programmes.length) {
                    row = r;
                    if (item.getProgrammeID()) {
                        for (c = 0, cl = this.programmes[r].length; c < cl; c += 1) {
                            if (this.programmes[r][c].programmeID === item.getProgrammeID()) {
                                col = c;
                                break;
                            }
                        }
                    } else {
                        var $cell;
                        var focusable = this._focusable;
                        for (c = 0, cl = this.rowList[r].element.length; c < cl; c += 1) {
                            $cell = $(this.rowList[r].element[c]);
                            if ($cell.hasClass(focusable)) {
                                col = c;
                                break;
                            }
                        }
                    }
                    if (col !== undefined && this.programmes[row]) {
                        programme = this.programmes[row][col];
                    }
                    break;
                }
            }
            if (programme === undefined) {
                return;
            }
        } else {
            programme = this.focusInfo.programme;
            row = this.focusInfo.$item.attr('row');
            col = this.focusInfo.$item.attr('col');
        }
        if (aHasThumbnail && !programme.isOnAir) {
            programme.isOnAir = true;
        }
        isFake = (programme.isNullProgramme) ? programme.isNullProgramme() : (programme.getPluginObject() ? false : true);
        if (this.rowList && this.rowList[row]) {
            this.rowList[row].element[col].innerHTML = isFake ? this.getNoProgrammeInformationCellHTML(programme, aHasThumbnail, this.focusInfo.$item) : this.getCellHTML(programme, aHasThumbnail, this.focusInfo.$item);
            if (this.cellRenewalEnd) {
                this.cellRenewalEnd();
            }
        }
    }
    naviPageProcess(param) {
        hx.log('guideView', hx.util.format('===== [{0}][naviPageProcess] ===== ', this.name));
        if (!this.doNavi(param)) {
            var channelsCount = hx.svc.CH_LIST.getChannelCount({
                group: this.channelList.channelGroup
            }).chCnt;
            if (channelsCount < this.ROW_COUNT) {
                if (param.alKey === hx.key.KEY_UP || param.alKey === hx.key.KEY_DOWN) {
                    var togo = (param.alKey === hx.key.KEY_DOWN) ? hx.key.KEY_UP : hx.key.KEY_DOWN;
                    this.moveToEnd(togo);
                }
                return true;
            }
            return this._tryPageScroll(param);
        }
        return true;
    }
    naviLineProcess(param) {
        hx.log('guideView', hx.util.format('===== [{0}][naviLineProcess] ===== ', this.name));
        if (!this.doNavi(param)) {
            var channelsCount = hx.svc.CH_LIST.getChannelCount({
                group: this.channelList.channelGroup
            }).chCnt;
            if (channelsCount < this.ROW_COUNT) {
                if (param.alKey === hx.key.KEY_UP || param.alKey === hx.key.KEY_DOWN) {
                    var togo = (param.alKey === hx.key.KEY_DOWN) ? hx.key.KEY_UP : hx.key.KEY_DOWN;
                    this.moveToEnd(togo);
                }
                return true;
            }
            return this._tryLineScroll(param);
        }
        return true;
    }
    isSameGenre(mdsGenre: string, genre: string) {
        var convertGenre: string;
        switch (mdsGenre) {
            case "Movie":
                convertGenre = hx.l('LOC_GENRE_MOVIE_ID');
                break;
            case "New and factual":
            case "News/Current affairs":
            case "Arts/Culture (without music)":
            case "Social/Political Issues/Economics":
                convertGenre = hx.l('LOC_GENRE_NEWS_AND_FACTUAL_ID');
                break;
            case "Entertainment":
            case "Show/Game show":
            case "Music/Ballet/Dance":
                convertGenre = hx.l('LOC_GENRE_AUS_ENTERTAINMENT_ID');
                break;
            case "Sport":
                convertGenre = hx.l('LOC_GENRE_AUS_SPORT_ID');
                break;
            case "Children's/Youth's programming":
                convertGenre = hx.l('LOC_CHILDRENS_ID');
                break;
            case "Education/Science/Factual topics":
                convertGenre = hx.l('LOC_GENRE_EDUCATION_ID');
                break;
            case "Lifestyle/Leisure hobbies":
                convertGenre = hx.l('LOC_LIFESTYLE_ID');
                break;
            case "Drama":
                convertGenre = hx.l('LOC_GENRE_AUS_DRAMA_ID');
                break;
            default:
                convertGenre = "Unclassified";
                break;
        }
        if (convertGenre === genre) {
            return true;
        }
        return false;
    }
    _on_alEndOfRepeatKey(aParam) {
        var k = hx.key, kc = aParam.alKey;
        switch (kc) {
            case hx.key.KEY_UP:
            case hx.key.KEY_DOWN:
            case hx.key.KEY_CHANNEL_UP:
            case hx.key.KEY_CHANNEL_DOWN:
                if (this._onGoingRepeat) {
                    this.channelList.draw({
                        updateFlag : true
                    });
                    this._setStopScrollTimer();
                    this._onGoingRepeat = false;
                    return true;
                }
                break;
        }
        return false;
    }
    _on_alKey(aParam): boolean {
        var alKey = aParam.alKey, useAnimation = this.animation.use, isRunning = this.animation.running, isConsumed: any = false;
        if (this._genreDlg && this._genreDlg.isShown()) {
            isConsumed = this._genreDlg.getParentLayer().doKey(alKey);
            if (!isConsumed) {
                if (alKey === hx.key.KEY_BACK || alKey === hx.key.KEY_ESCAPE) {
                    this._genreDlg.destroy();
                    this._genreDlg = null;
                    isConsumed = true;
                }
            }
            return isConsumed;
        }
        if (this.state === this.EVIEW_STATE.EBUSY) {
            return super._on_alKey(aParam);
        }
        switch (alKey) {
            case hx.key.KEY_UP:
            case hx.key.KEY_DOWN:
                if (useAnimation && isRunning) {
                    this._swapLine = this.TSwapLineType.ENONE;
                    return;
                }
                if (this.state === 'idle') {
                    isConsumed = this.naviLineProcess(aParam);
                }
                break;

            case hx.key.KEY_CHANNEL_UP:
            case hx.key.KEY_CHANNEL_DOWN:
                if (!isRunning) {
                    isConsumed = this.naviPageProcess(aParam);
                } else {
                    isConsumed = true;
                }
                break;
            case hx.key.KEY_OK:
                if (this.changeChannel) {
                    isConsumed = this.changeChannel(aParam);
                }
                break;
            case hx.key.KEY_OPTION:
                //this.showGenreOption();
                isConsumed = true;
                break;
        }
        return (isConsumed || super._on_alKey(aParam));
    }
    isGenreDlgActivated() {
        return !!this._genreDlg;
    }
    showGenreOption() {
        var a = this;
        if (!this._genreDlg && this.state === this.EVIEW_STATE.EIDLE) {
            this.state = this.EVIEW_STATE.EBUSY;
            if (this.name === 'guideBackward') {
                setTimeout(() => {
                    var chList = hx.svc.CH_LIST.getChlist().chList;
                    var channels = chList.map(function(channel) {
                        return channel.sid;
                    });
                    var xmls = hx.svc.IP_EPG.getCachedBackward();
                    var now = new Date().getTime();
                    var parseBackwardXml = (obj) => {
                        var resultDef = Q.defer();
                        var param: IScheduleParam;
                        var xml = obj.data;
                        var info = hx.svc.IP_EPG.parse.xml.schedule(xml);
                        var schedules = info.schedules;
                        var crids = info.crids;
                        var serviceId = info.serviceId;
                        var index = channels.indexOf(parseInt(serviceId));
                        param = {
                            schedules : schedules,
                            crids : crids,
                            index : index
                        };
                        if (index !== -1) {
                            resultDef.resolve(param);
                        } else {
                            resultDef.reject(null);
                        }
                        xml = undefined;
                        info = undefined;
                        schedules = undefined;
                        crids = undefined;
                        return resultDef.promise;
                    };
                    xmls.forEach((obj) => {
                        parseBackwardXml(obj).then((param:IScheduleParam) => {
                            var schedules = param.schedules;
                            var crids = param.crids;
                            var index = param.index;
                            var scheduleQueryList = [];
                            var makeMdsProgramme = (schedule, crids, index) => {
                                var retDef = Q.defer();
                                var crid = schedule.querySelector('Program').getAttribute('crid');
                                var data = crids[crid];
                                data.schedule = schedule;
                                data.channel = chList[index];
                                setTimeout(() => {
                                    var mdsProgramme = new __PROVIDER__.CProgrammeProvider({
                                        mdsData : data,
                                        channel : chList[index],
                                        launchLocation : 'epg'
                                    });
                                    if (mdsProgramme.getStartTime() * 1000 >= now) {
                                        retDef.reject(null);
                                        return retDef.promise;
                                    }
                                    mdsProgramme.channelType = 0;
                                    ((aProg, aIndex) => {
                                        this.allProgrammes[aIndex] = this.allProgrammes[aIndex] || [];
                                        setTimeout(() => {
                                            hx.svc.IP_EPG.getAIT(aProg.getTemplateAitUrl()).then((xml) => {
                                                aProg.checkTemplateAIT(xml);
                                                a.allProgrammes[aIndex].push(aProg);
                                                retDef.resolve(aProg);
                                            }, () => {
                                                a.allProgrammes[aIndex].push(aProg);
                                                retDef.reject(null);
                                            });
                                        }, 10);
                                    })(mdsProgramme, index);
                                }, 10);
                                return retDef.promise;
                            };
                            var pushToAllProgrammes = (queryList, index) => {
                                var idx = 0;
                                var len = queryList.length;
                                var executeQuery = (query) => {
                                    var schedule = query.schedule;
                                    var crids = query.crids;
                                    makeMdsProgramme(schedule, crids, index).fin(() => {
                                        idx++;
                                        if (idx < len) {
                                            setTimeout(() => {
                                                if (this._genreDlg) {
                                                    executeQuery(queryList[idx]);
                                                }
                                            }, 10);
                                        }
                                    });
                                    schedule = undefined;
                                    crids = undefined;
                                }
                                setTimeout(() => {
                                    if (this._genreDlg) {
                                        executeQuery(queryList[idx]);
                                    }
                                }, 10);

                                //queryList.forEach((query) => {
                                //        var schedule = query.schedule;
                                //        var crids = query.crids;
                                //        var def = query.retDef;
                                //        makeMdsProgramme(schedule, crids, index, def).done(() => {
                                //
                                //        });
                                //});
                            };
                            var addScheduleQueryToQueryList = (schedule, crids) => {
                                var query:any = {
                                    schedule: schedule,
                                    crids: crids
                                };
                                scheduleQueryList.push(query);
                            };
                            for (var i = 0; i < schedules.length; i++) {
                                var schedule = schedules[i];
                                addScheduleQueryToQueryList(schedule, crids);
                            }
                            pushToAllProgrammes(scheduleQueryList, index);
                            schedules = undefined;
                            crids = undefined;
                            scheduleQueryList = undefined;
                        }, (err) => {});
                    });
                    xmls = undefined;
                }, 10);
            } else {    // for next view
                this.getAllProgrammes();
            }
            var fnDataDrawer = function (param) {
                var drawType = param.drawType;
                var item = param.item;
                var category = item.getCategory();
                var name = item.getName();
                var dummy = item.isDummy();
                var subItemList = item.getSubItemList();
                var figure = document.createElement('figure');
                param.$item.children().remove();
                switch (drawType) {
                    case 'group':
                        var categoryEl;
                        categoryEl = document.createElement('div');
                        categoryEl.className = 'category';
                        if (dummy) {
                            categoryEl.innerHTML = name;
                        } else {
                            categoryEl.innerHTML = category.text;
                            if (subItemList.length) {
                                categoryEl.innerHTML += ' (' + subItemList.length + ')';
                            }
                        }
                        param.$item.append($(categoryEl));
                        break;
                    case 'detail':
                        var firstRow, secondRow;
                        var updateThumbnail = function (receivedUrl) {
                            if (receivedUrl) {
                                var $img = param.$item.find('img');
                                if ($img && $img.length) {
                                    $img[0].src = receivedUrl;
                                }
                            }
                        };
                        var updateLogo = function (receivedUrl) {
                            if (receivedUrl) {
                                secondRow.innerHTML = hx.util.format('<img src="{0}" alt="" border="0"' +
                                    ' onload="this.style.display=\'inline\';" style="display:none;"/>', receivedUrl) + " " + dispTime + dispDate;
                            }
                        };

                        var addition = item.getAddition();
                        var channelNo = item.getChannelNo();
                        var channelName = item.getChannelName();
                        var thumbnail = item.getThumbnail(updateThumbnail);
                        var channelLogo = item.getChannelLogo(updateLogo);
                        var startTime = item.getStartTime() * 1000;
                        var endTime = item.getEndTime() * 1000;
                        var metaType = item.getMetadataType();
                        var classThumbIcon = item.getClassThumbIcon();
                        var defaultThumbnail = '';
                        if (metaType === "TV" || metaType === "HDTV" || metaType === hx.l('LOC_TV_PROGRAMME_ID') || metaType === 0) {
                            figure.className = 'thum tv';
                            defaultThumbnail = __MODEL__.TDefaultThumbnailList.TV;
                        } else if (metaType === "Radio" || metaType === hx.l('LOC_RADIO_PROGRAMME_ID') || metaType === 1) {
                            figure.className = 'thum radio';
                            defaultThumbnail = __MODEL__.TDefaultThumbnailList.RADIO;
                        } else {
                            figure.className = 'thum';
                            defaultThumbnail = __MODEL__.TDefaultThumbnailList.TV;
                        }
                        thumbnail = thumbnail || defaultThumbnail;
                        figure.innerHTML = hx.util.format('<img src="{0}" alt="" border="0" onerror="this.src=\'{1}\';"/>', thumbnail, defaultThumbnail);
                        firstRow = document.createElement('p');
                        firstRow.className = 'tit';
                        secondRow = document.createElement('span');
                        secondRow.className = 'addition';
                        firstRow.innerHTML = dummy ? '' : name;
                        var dispTime = '', dispDate = '', channelInfo = '';
                        if (startTime && endTime) {
                            dispTime = dateFormat(startTime, 'HH:MM') + ' ~ ' + dateFormat(endTime, 'HH:MM');
                            dispDate = ' ' + dateFormat(startTime, 'ddd dd mmm');
                        }
                        if (channelName && channelNo) {
                            channelInfo = hx.il.makeDigitNumber(channelNo) + ' ' + channelName + ' / ';
                        }
                        if (channelLogo) {
                            updateLogo(channelLogo);
                        } else {
                            secondRow.innerHTML = channelInfo + dispTime + dispDate;
                        }
                        if (classThumbIcon) {
                            param.$item.addClass(classThumbIcon);
                        }
                        param.$item.append($(figure)).append($(firstRow)).append($(secondRow));
                        break;
                    default:
                        break;
                }
            };
            a._genreDlg = prism.createCategoryControl({
                $el : a._parent._$wrap,
                withoutKeyboard : true,
                title : "GENRE",
                category : this.SCategory,
                target : this.TGenreTarget,
                dataDrawer : fnDataDrawer,
                sortDescent : !!(a.name === 'guideBackward')
            });
            a._genreDlg.onClosed = function() {
                a.state = a.EVIEW_STATE.EIDLE;
                a.allProgrammes = [];
                a._setFocusCssStyle('', 'on');
            };
            a._genreDlg.onSearch = function(text, searchType, isMoreSearch, isUsingSuggestion, param?) {
                if (!a._genreDlg) {
                    hx.log('debug', '[cGuideView] doSearch fail!! => !a._genreDlg');
                    return;
                }
                if (!a.allProgrammes) {
                    hx.log('debug', '[cGuideView] doSearch fail!! => !a.allProgrammes');
                    return;
                }
                if (isMoreSearch && param) {
                    var target = param.target;
                    var i, l, j, jl, k, kl, programmes, data = a.allProgrammes, genre, hasGenre;
                    var contentList: any = [];
                    var currentIndex = target - 1;
                    for (i = 0, l = data.length ; i < l ; i++) {
                        if (!data[i]) {
                            continue;
                        }
                        programmes = data[i];
                        var programme;
                        for (j = 0, jl = programmes.length ; j < jl ; j++) {
                            programme = programmes[j];
                            hasGenre = null;
                            genre = programme.getGenre();
                            if (genre) {
                                for (k = 0, kl = genre.length ; k < kl ; k++) {
                                    if (target === a.TGenreTarget.EAll && programme.name !== tr('LOC_NO_PROGRAMME_INFORMATION_ID')) { //ALL
                                        hasGenre = true;
                                        contentList.push(programme);
                                        break;
                                    }
                                    if (genre[k] === a._genreList[currentIndex]) {
                                        hasGenre = true;
                                        contentList.push(programme);
                                        break;
                                    }
                                }
                            }
                            if (a.isEmpty(hasGenre)) {
                                genre = programme._userGenres;
                                if (genre) {
                                    for (k = 0, kl = genre.length ; k < kl ; k++) {
                                        if (target === a.TGenreTarget.EAll && programme.name !== tr('LOC_NO_PROGRAMME_INFORMATION_ID')) { // ALL
                                            contentList.push(programme);
                                            break;
                                        }
                                        if (genre[k] === a._genreList[currentIndex]) {
                                            contentList.push(programme);
                                            break;
                                        }
                                    }
                                } else {
                                    if (target === a.TGenreTarget.EAll && programme.name !== tr('LOC_NO_PROGRAMME_INFORMATION_ID')) { // ALL
                                        contentList.push(programme);
                                    }
                                }
                            }
                        }
                    }
                    a._genreDlg.update({
                        items : contentList,
                        type : target
                    });
                }
            };
            a._genreDlg.show({
                dimm : '-full-black'
            });
            a._genreDlg.onPlayRequest = function(param) {
                a._parent.processProgrammeDetail(param);
            };
            a._genreDlg.onRecordRequest = function(param) {
                 return a._parent.processRecord(param);
            }
        }
    }
    onFocusChanged($old, $new) {
        hx.log('guideView', hx.util.format('===== [{0}][onFocusChanged] ===== ', this.name));
        var oldRow = $old.attr('row'), newRow = $new.attr('row');
        var classFocused = this._focused;
        if (oldRow !== newRow) {
            var channelList = this.channelList;
            channelList.deactivateRow(oldRow);
            channelList.activateRow(newRow);
        }
        if ($old.hasClass('active')) {
            $old.removeClass('active');
        }
        $old.removeClass(classFocused);
        $new.addClass(classFocused);
        this.setFocus($new);
    }
    getChannels(param) {
        hx.log('guideView', hx.util.format('===== [{0}][getChannels] ===== ', this.name));
        if (this.channelList) {
            this.channelList.getChannels(param);
            this.getProgrammes(param);
        }
    }
    recInProgress(programme) {
        var i, l, list = [], inProgress = false;
        var item;
        hx.log('guideView', hx.util.format('===== [{0}][recInProgress] ===== ', this.name));
        list = this.recordings.curRecordingList;
        for (i = 0, l = list.length; i < l; i += 1) {
            item = list[i];
            if (item) {
                if (item.getChannelId() === programme.ccid) {
                    inProgress = true;
                    break;
                }
            }
        }
        return inProgress;
    }
    makeSpanClassForEvent(data){
        var that = this;
        var tmpVal;
        var strVar = '', recording = false;
        if (data.isOnAir) {
            tmpVal = that.recInProgress(data);
            if (tmpVal) {
                strVar = '<span class="rec"/></span>';
            }
            return strVar;
        }
        if (data.pluginObject){
            recording = data.pluginObject.recording;
            if(!recording){
                recording = false;
            }
        }
        if (!recording){
            tmpVal = data.hasSeriesSchedule(recording);
            if (tmpVal) {
                strVar = '<span class="seri" /></span>';
            }
        } else{
            tmpVal = data.hasSchedule(recording);
            if (tmpVal) {
                strVar = '<span class="rec2" /></span>';
            } else {
                tmpVal = data.hasReminder(recording);
                if (tmpVal) {
                    strVar = '<span class="remi" /></span>';
                } else {
                    tmpVal = data.hasSeriesSchedule(recording);
                    if (tmpVal) {
                        strVar = '<span class="seri" /></span>';
                    }
                }
            }
        }
        return strVar;
    }
    getDispPercentage(data) {
        var dateObj = new Date(), now;
        now = dateObj.getTime() / 1000;
        return parseInt(((now - data.startTime) / data.duration * 100) + "", 10);
    }
    getCellHTML(data: any, aHasThumbnail?: boolean, $item?, parentWidth?): string {
        var str = "";
        hx.log('guideView', hx.util.format('===== [{0}][getCellHTML] ===== ', this.name));
        if (data) {
            str += this.getCellStrHTML(data, aHasThumbnail, $item, parentWidth);
            if (this.name === 'guideBackward') {
                if (true === data.hasRecItem) {
                    str += '<img class="availability" src="images/205_REC_Play.png"> </>';
                }
            }
        }
        return str;
    }
    getCellStrHTML(data, hasThumbnail, $item, parentWidth) {
        var that = this;
        var str = [], name, thumbnailSrc, lineWidth, lineWidth2, left, padding, lineLeft, pInfoWidth, currentWidth;
        var strVar;
        if (data) {
            name = data.getName();
            if (hasThumbnail) {
                if ($item) {
                    currentWidth = parseInt($item.css('width'), 10);
                    left = parseInt($item.css('left'), 10) + $(that.box).position().left + 20;
                    padding = 0;
                    lineLeft = (left < padding ? 0 : (left > that.MAX_FOCUS_LEFT ? left - padding - that.MAX_FOCUS_LEFT : 0));
                    if (currentWidth < 35) {
                        lineWidth = $item[0].clientLeft + $item[0].clientWidth;
                    } else {
                        lineWidth = currentWidth + 35;
                    }
                    lineWidth2 = lineWidth + 1;
                    pInfoWidth = currentWidth;
                    if (!parentWidth) {
                        parentWidth = pInfoWidth + 'px';
                    }
                    str.push('<em class="curLine mod" style="left: 0px; top:-1px; width:' + lineWidth + 'px;"></em>');
                    str.push('<div class="pInfo2 mod" style="left: 0px; top:2px; width:' + parentWidth + ';"</div>');
                    str.push('<div class="dataArea">');
                    if (data.backwardView) {
                        str.push('<em class="title mod" style="width:' + parentWidth + ';">' + name + '</em>');
                    } else {
                        str.push('<em class="mod icoRec">');
                        str.push(that.makeSpanClassForEvent(data));
                        str.push('</em>');
                        str.push('<em class="title mod" style="width:' + parentWidth + ';">' + name + '</em>');
                    }
                    str.push('</div>');
                }
            } else {
                if ($item) {
                    currentWidth = $item.css('width');
                    str.push('<em class="curLine mod" style="top:0px; left: 0px; width: ' + currentWidth + ';"></em>');
                } else {
                    str.push('<em class="curLine mod" style="top:0px;"></em>');
                }
                str.push('<div class="dataArea">');
                if (!data.backwardView) {
                    str.push('<em class="mod icoRec">');
                    str.push(that.makeSpanClassForEvent(data));
                    str.push('</em>');
                }
                var availabilityHtml = '';
                if ($item) {
                    var availability = $item.find('.availability');
                    if (availability.length) {
                        availabilityHtml = availability[0].outerHTML;
                    }
                }
                str.push('<em class="title mod">' + availabilityHtml + name + '</em>');
                str.push('</div>');
            }
        }
        return str.join('');
    }
    getNoProgrammeInformationCellHTML(data, hasThumbnail, $item?, parentWidth?) {
        var str = "", hasCAS;
        hx.log('guideView', hx.util.format('===== [{0}][getNoProgrammeInformationCellHTML] ===== ', this.name));
        if (this.name === 'guideCatchUp') {
            str += hx.util.format('<div>' + hx.l('LOC_NO_RECORDINGS_IN_THIS_CHANNEL_ID') + '</div>');
        } else {
            str += this.getNoProgrammeInformationCellStrHTML(data, hasThumbnail, $item, parentWidth);
            //str += dateFormat(data.getStartTime() * 1000, 'ddd.dd.mm');
            str += '</em>';
            str += '</div>';
        }
        return str;
    }
    getNoProgrammeInformationCellStrHTML(data, hasThumbnail, $item, parentWidth) {
        var that = this;
        var str = [], left, padding, lineLeft, lineWidth, lineWidth2, pInfoWidth, currentWidth;
        if ($item) {
            currentWidth = parseInt($item.css('width'), 10);
            left = parseInt($item.css('left'), 10) + $(that.box).position().left + 20;
            if (currentWidth < 35) {
                lineWidth = $item[0].clientLeft + $item[0].clientWidth;
            } else {
                lineWidth = currentWidth + 35;
            }
            lineWidth2 = lineWidth + 1;
            pInfoWidth = currentWidth;
            if (!parentWidth) {
                parentWidth = pInfoWidth + 'px';
            }
            str.push('<em class="curLine mod" style="left: 0px; top:-1px; width:' + lineWidth + 'px;"></em>');
            str.push('<div class="pInfo2 mod" style="left: 0px; top:2px; width:' + parentWidth + ';"</div>');
        }
        str.push('<div class="dataArea">');
        if (data.backwardView) {
            if (parentWidth) {
                str.push('<em class="title mod" style="width:' + parentWidth + ';">' + data.name + '</em></div>');
            } else {
                str.push('<em class="title mod">' + data.name + '</em></div>');
            }
        } else {
            str.push('<em class="mod icoRec"></em>');
            str.push('<em class="title mod">' + data.name + '</em>');
        }
        str.push((data.isOnAir && that.recInProgress(data)) ? ('<span class="rec"/></span>') : '');
        str.push('</div>');
        return str.join('');
    }
    initFocus() {
        var row = 0, col = 0, i, currentRowIndex, focusable, focused, length, cells, $next;
        var cell, first, last, next, position;
        hx.log('guideView', hx.util.format('===== [{0}][initFocus] ===== ', this.name));
        if (this.channelList) {
            focusable = this._focusable;
            focused = this._focused;
            currentRowIndex = this.channelList.currentRowIndex;
            cells = this.rowList[currentRowIndex].element;
            for (i = 0, length = cells.length; i < length; i += 1) {
                cell = cells[i];
                if (this.hasClassForDom(cells[i], focusable)) {
                    if (!first) {
                        first = cell;
                    }
                    last = cell;
                    if (this.focusInfo.$item && Number(this.focusInfo.$item.attr('col')) === i) {
                        position = cell;
                        break;
                    }
                }
            }
            if (position) {
                next = position;
            } else {
                if (this.name === 'guideBackward') {
                    if (this.getLastKey() === hx.key.KEY_RIGHT) {
                        next = first;
                    } else {
                        next = last;
                    }
                } else {
                    next = (this.getLastKey() === hx.key.KEY_RIGHT || this.getLastKey() === null) ? first : last;
                }
            }
            if (next) {
                if (this.name === 'guideOnAir') {
                    next.classList.add(focused);
                }
                $next = $(next);
                this.setFocus($next);
            }
        }
    }
    setFocus($new: JQuery) {
        var that = this;
        hx.log('guideView', hx.util.format('===== [{0}][setFocus] ===== ', this.name));
        var programme = this.getData($new.attr('row'), $new.attr('col'));
        if (programme) {
            //var width_row = 982;
            //var width_hour = 441;
            if (this.isEmpty(programme)) {
                hx.log('warning', 'programme data does not exist.');
                return;
            }
            var currentWidth = parseInt($new.css('width'));
            var isFake = false;
            if (programme.backwardView) {
                isFake = !!(programme.getName() === hx.l('LOC_NO_PROGRAMME_INFORMATION_ID'));
            } else {
                isFake = programme.isNullProgramme ? programme.isNullProgramme() : !programme.getPluginObject();
            }
            var $item;
            var width;
            var left;
            if (this.focusInfo) {
                $item = this.focusInfo.$item;
                if ($item) {
                    width = $item.attr('base-width');
                    if (width) {
                        $item.css('width', width);
                        $item.removeAttr('base-width');
                    }
                    left = $item.attr('base-left');
                    if (left) {
                        $item.css('left', left);
                        $item.removeAttr('base-left');
                    }
                }
            }
            this.focusInfo = {
                $item: $new,
                programme: programme
            };
            //width = Math.max(width_hour, currentWidth);
            //if (width !== currentWidth) {
                $new.attr('base-width', currentWidth);
                $new.css('width', currentWidth);
            //}
            //left = parseInt($new.css('left')) || 0;
            var durationIndicator = $new.children('.curLine');
            var durationIndicatorLeft = 0;
            //if (left + width_hour > width_row) {
            //    var focusedLeft = width_row - width_hour;
            //    $new.attr('base-left', left);
            //    $new.css('left', left);
            //    durationIndicatorLeft = left - focusedLeft;
            //}
            durationIndicator.css('width', currentWidth);
            durationIndicator.css('left', durationIndicatorLeft + 'px');
            //if (that._stayedFocusTimerId) {
            //    clearTimeout(that._stayedFocusTimerId);
            //    that._stayedFocusTimerId = null;
            //}
            //that._$activeNew = $new;
            //that._stayedFocusTimerId = setTimeout(function() {
            //    if (that._stayedFocusTimerId) {
            //        that._$activeNew.addClass('active');
            //        that._$activeNew.css('left', focusedLeft);
            //        that._$activeNew.css('width', width);
            //        that._stayedFocusTimerId = null;
            //    }
            //}, 1000);
            this.updateBanner(!isFake);
        }
    }
    updateBanner(bProgramme: boolean) {
        var that = this;
        var $bannerArea = that._parent._$banner;
        var bannerStyleObj = $bannerArea[0].style;
        if (bProgramme) {
            var $title = $bannerArea.find('.title');
            var $time = $bannerArea.find('.time');
            var $icon = $bannerArea.find('.icon');
            var titleText = '', timeHtml = '';
            var prog, startTime, endTime, dispType = '', dispTime = '', dispDate = '', iconHtml = '';
            var liveHtml = '<b class=\"live\">LIVE </b>';
            prog = this.focusInfo.programme;
            endTime = prog.getEndTime() * 1000;
            startTime = prog.getStartTime() * 1000;
            if (startTime && endTime) {
                dispType = ' - ';
                dispTime = dateFormat(startTime, 'HH:MM') + dispType + dateFormat(endTime, 'HH:MM');
                dispDate = ' <span>|</span> ' + dateFormat(startTime, 'ddd dd mmm');
                timeHtml = prog.isLive() ? liveHtml + dispTime + dispDate : dispTime + dispDate;
            }
            iconHtml = that._fnMakeIcon(prog);
            titleText = prog.getName();
            $title[0].innerHTML = titleText;
            $time[0].innerHTML = timeHtml;
            $icon[0].innerHTML = iconHtml;
            if (bannerStyleObj.opacity === '0') {
                bannerStyleObj.opacity = '1';
            }
        } else {
            if (bannerStyleObj.opacity === '1') {
                bannerStyleObj.opacity = '0';
            }
        }
    }
    _fnMakeIcon(item: __PROVIDER__.CProgrammeProvider) {
        var i, length, html = '', parentalRating, casIDs, ratingClass, casClass, isCas = false, subtitleTypes;
        var dvb = false, ebu = false;
        function makeHtml(klass) {
            var ret = "";
            if (klass) {
                ret =  "<span class=\"" + klass + "\"></span>";
            }
            return ret;
        }
        // Recording, Reminder
        //html = makeHtml(item.hasSchedule() ? 'ico recwatch' : '');
        // HD
        if (item.isHd()) {
            html += makeHtml('ico hd');
        }
        // Guidance
        var linkages = item.getHDLinkage() || null;
        if (linkages && linkages.length) {
            html += makeHtml("ico PD_HD");
        }
        if(item.hasGuidance()) {
            html += makeHtml('ico guidance');
        }
        //Recommandation
        if (item.getRecommendationCRID()) {
            html += makeHtml('ico recommendation');
        }
        //Split
        if(item.getProgrammeCRID()){
            if(item.getProgrammeCRID().search("#") > 0) {
                html += makeHtml('ico split');
            }
        }
        // AD
        if (item.isAd()) {
            html += makeHtml('ico AD');
        }
        // Parental Rating
        parentalRating = item.getParentalRatings() ? item.getParentalRatings()[0].value : 0;
        ratingClass = this.TRatingType[parentalRating > 15 ? 15 : parentalRating];
        if (ratingClass) {
            html += makeHtml('ico rat aus_' + ratingClass);
        }
        // teletext
        html += makeHtml(item.channel && item.channel.hasTeletext ? 'ico ttx' : '');
        // Subtitle
        subtitleTypes = item.getSubtitleTypes();
        if (subtitleTypes) {
            for (i = 0; i < subtitleTypes.length; i += 1) {
                if (subtitleTypes[i] === "DVB" && dvb === false) {
                    html += makeHtml('ico pdsubDVB');
                    dvb = true;
                } else if (subtitleTypes[i] === "EBU" && ebu === false) {
                    html += makeHtml('ico pdsubEBU');
                    ebu = true;
                }
            }
        }
        // Multi Audio
        html += makeHtml((item.isMultiAudio()) ? 'ico ma' : '');
        return html;
    }
    popFocusStyleLeft(aParam?) {
        throw 'k'
        return '0px';
    }
    getData(row, col) {
        var programme, programmes = this.programmes[row];
        if (programmes) {
            programme = programmes[col];
        }
        return programme;
    }
    /*
    setFocus($new) {
        var row, col, programme, isFake;
        var $old = this.focusInfo.$item;
        hx.log('guideView', hx.util.format('===== [{0}][setFocus] ===== ', this.name));
        row = $new.attr('row');
        col = $new.attr('col');
        programme = this.getData(row, col);
        if (programme) {
            isFake = (programme.isNullProgramme) ? programme.isNullProgramme() : (programme.getPluginObject() ? false : true);
            if (this.isEmpty(programme)) {
                hx.log('warning', 'programme data does not exist.');
                return;
            }
            this.focusInfo = {
                $item: $new,
                programme: programme
            };
        }
        if (this.setFocusEnd) {
            this.setFocusEnd($old, $new);
        }
    }
    */
    getFocusWidth(data, width, left) {
        var that = this, programme = data, focusWidth, currentWidth = width, backwardLeft = parseInt(left, 10);
        if (programme.isOnAir === true && !programme.nextView) {
            focusWidth = "548px";
        } else {
            if (programme.isOnAir === true && programme.nextView) {
                if (programme.duration > 3600) {
                    focusWidth = (currentWidth < 441 ? 441 : currentWidth) + 'px';
                } else {
                    focusWidth = "441px";
                }
            } else {
                if (programme.backwardView) {
                    backwardLeft = backwardLeft + 1;
                    focusWidth = currentWidth;
                    backwardLeft = backwardLeft;
                } else {
                    focusWidth = currentWidth;
                }
            }
        }
        return {
            'width' : focusWidth,
            'left' : backwardLeft + 'px'
        }
    }
    dcnRefresh() {
        var dcn = this.dcn;
        hx.log('guideView', hx.util.format('===== [{0}][dcnRefresh] ===== ', this.name));
        if (dcn._timerId !== null) {
            clearTimeout(dcn._timerId);
        }
        dcn._timerId = setTimeout(function () {
            dcn.clearDcn();
        }, 3000);
    }
    dcnClicked(param) {
        var i, length, ccid, group, dcn = this.dcn, channels = this.channelList.channels, recordings = this.channelList.recordings;
        hx.log('guideView', hx.util.format('===== [{0}][dcnClicked] ===== ', this.name));
        ccid = param.alData.ccid;
        group = param.alData.group;
        for (i = 0, length = recordings.length; i < length; i += 1) {
            if (ccid === recordings[i].channel.ccid && group === recordings[i].channel.type) {
                this.getChannels({
                    ccid: ccid,
                    group: group,
                    offset: 0,
                    count: this.ROW_COUNT
                });
                break;
            }
        }
        dcn.clearDcn();
        return true;
    }
    freeMem(data) {
        var that = this, i, il, j, jl;
        hx.log('guideView', hx.util.format('===== [{0}][freeMem] ===== ', that.name));
        for ( i = 0, il = data.length; i < il; i += 1) {
            if(data[i]){
                for ( j = 0, jl = data[i].length; j < jl; j += 1) {
                    if (data[i][j]) {
                        data[i].pluginObject = null;
                        delete data[i][j];
                    }
                }
            }
        }
    }
    _on_ChildNotify(param) {
        var action;
        hx.log('guideView', hx.util.format('===== [{0}][_on_ChildNotify] action : {1} ===== ', this.name, param.action));
        action = param.action;
        if (this[action]) {
            this[action](param);
        } else {
            this.sendEventToParent(param);
        }
    }
    _isWaitingScroll(direction?) {
        var index;
        hx.log('guideView', hx.util.format('===== [{0}][_isWaitingScroll]  ===== ', this.name));
        if (this._stopScrollTimerID === null) {
            return false;
        }
        hx.log('warning', '[_isWaitingScroll] on scrolling');
        return true;
    }
    _updateChannelFocus(direction) {
        var indexOld, indexNew, currentRowIndex = this.channelList.currentRowIndex;
        hx.log('guideView', hx.util.format('===== [{0}][_updateChannelFocus]  ===== ', this.name));
        indexOld = currentRowIndex;
        indexNew = (direction === 'up') ? this.minMax(currentRowIndex + 1) : this.minMax(currentRowIndex - 1);
        if (indexOld !== indexNew) {
            this.channelList.deactivateRow(indexOld);
            this.channelList.activateRow(indexNew);
        }
    }
    _setStopScrollTimer() {
        hx.log('guideView', hx.util.format('===== [{0}][_setStopScrollTimer]  ===== ', this.name));
        if (this._stopScrollTimerID) {
            clearTimeout(this._stopScrollTimerID);
            this._stopScrollTimerID = null;
        }
        this._stopScrollTimerID = setTimeout(() => {
            hx.log('warning', hx.util.format('[E] _stopScrollTimerID : {0}', this._stopScrollTimerID));
            if (this._stopScrollTimerID === null) {
                hx.log('warning', 'on scrolling!');
                return;
            }
            this._stopScrollTimerID = null;
            if (!this._swapLine || this._swapLine === this.TSwapLineType.ENONE) {
                this.clear();
            }
            this.getProgrammes({});
        }, 0 /*700*/);
        // <for optimize>
        hx.log('guideView', hx.util.format('[S] _stopScrollTimerID : {0}', this._stopScrollTimerID));
    }
    _tryPageScroll(key) {
        var direction, offset, movingCount, group, ccid, param, startIndex;
        var keyRepeatCount = key.alRepeat;
        hx.log('guideView', hx.util.format('===== [{0}][_tryPageScroll]  ===== ', this.name));
        switch (key.alKey) {
            case hx.key.KEY_CHANNEL_UP:
                direction = 'down';
                offset = -this.ROW_COUNT;
                movingCount = this.ROW_COUNT;
                startIndex = 0;
                break;
            case hx.key.KEY_CHANNEL_DOWN:
                direction = 'up';
                offset = 1;
                movingCount = this.ROW_COUNT;
                startIndex = this.ROW_COUNT - 1;
                break;
        }
        group = this.channelList.channelGroup;
        param = {
            'group': group,
            'offset': offset,
            'count': this.ROW_COUNT,
            'keyRepeatCount': keyRepeatCount
        };
        this.programmes = [];
        this.$.css('opacity', 0);
        this.updateBanner(false);
        if (!keyRepeatCount) {
            this.channelList.deactivateRow(this.channelList.currentRowIndex);
        }
        hx.log('guideView', hx.util.format('direction : {0}, offset : {1}', direction, offset));
        var onScrollStarted = (param) => {
            var startChannel = this.channelList.channels[startIndex];
            param.ccid = startChannel.ccid;
            hx.log('warning', hx.util.format('_tryPageScroll : direction - {0}, majorChannel : - {1}, movingCount - {2}', direction, startChannel.majorChannel, movingCount));
            this.initKey();
            this.channelList.getChannels(param);
        };
        var onScrollStopped = () => {
            this._prevScrollParam = null;
            if (keyRepeatCount) {
                hx.log('warning', '_tryPageScroll[keyRepeatCount] : ' + keyRepeatCount);
                this._onGoingRepeat = true;
                return;
            }
            this._onGoingRepeat = false;
            this._setStopScrollTimer();
            hx.log('warning', '_tryPageScroll[_setStopScrollTimer] : onScrollStopped');
            this.channelList.activateRow(this.channelList.currentRowIndex);
        };
        this.channelList.doScroll({
            'direction': direction,
            'movingCount': movingCount,
            'onScrollStarted': onScrollStarted,
            'onScrollStopped': onScrollStopped,
            'chParam': param
        });
        return true;
    }
    _tryLineScroll(key) {
        var direction, offset, movingCount, group, ccid, param, startIndex, requestScrollParam;
        var keyRepeatCount = key.alRepeat;
        hx.log('guideView', hx.util.format('===== [{0}][_tryLineScroll]  ===== ', this.name));
        switch (key.alKey) {
            case hx.key.KEY_UP:
                direction = 'down';
                if (this._isWaitingScroll(direction)) {
                    this._updateChannelFocus(direction);
                    hx.log('warning', '_tryLineScroll(KEY_UP) => _isWaitingScroll');
                    return true;
                }
                offset = -(this.ROW_COUNT - 1);
                movingCount = this.ROW_COUNT - 1;
                startIndex = this.ROW_COUNT - 1;
                break;
            case hx.key.KEY_DOWN:
                direction = 'up';
                if (this._isWaitingScroll(direction)) {
                    this._updateChannelFocus(direction);
                    hx.log('warning', '_tryLineScroll(KEY_DOWN) => _isWaitingScroll');
                    return true;
                }
                offset = 0;
                movingCount = this.ROW_COUNT - 1;
                startIndex = 0;
                break;
        }
        group = this.channelList.channelGroup;
        param = {
            'group': group,
            'offset': offset,
            'count': this.ROW_COUNT,
            'keyRepeatCount': keyRepeatCount
        };
        this.programmes = [];
        this.$.css('opacity', 0);
        this.updateBanner(false);
        var startDrawChannel = this.channelList.channels[this.channelList.currentRowIndex];
        if (!this.ANIMATION_EFFECT && !keyRepeatCount) {
            this.channelList.deactivateRow(this.channelList.currentRowIndex);
        }
        hx.log('guideView', hx.util.format('direction : {0}, offset : {1}', direction, offset));
        var onScrollStarted = (param) => {
            if (keyRepeatCount) {
                startDrawChannel = this.channelList.channels[direction==='up' ? startIndex+1 : startIndex-1];
            }
            param.ccid = startDrawChannel.ccid;
            hx.log('warning', hx.util.format('_tryLineScroll : direction - {0}, majorChannel : - {1}, movingCount - {2}', direction, startDrawChannel.majorChannel, movingCount));
            this.initKey();
            this.channelList.getChannels(param);
        };
        var onScrollStopped = () => {
            if (keyRepeatCount) {
                hx.log('warning', '_tryLineScroll[keyRepeatCount] : ' + keyRepeatCount);
                this._onGoingRepeat = true;
                this._prevScrollParam = null;
                return;
            }
            if (this._isSameRequestForScrolling(requestScrollParam)) {
                hx.log('warning', '_tryLineScroll[_isSameRequestForScrolling]');
                this._updateChannelFocus(direction);
                return;
            }
            this._prevScrollParam = requestScrollParam;
            this._onGoingRepeat = false;
            this._setStopScrollTimer();
            hx.log('warning', '_tryLineScroll[_setStopScrollTimer] : onScrollStopped');
            this.channelList.activateRow(startIndex);
        };
        requestScrollParam = {
            'direction': direction,
            'movingCount': movingCount,
            'onScrollStarted': onScrollStarted,
            'onScrollStopped': onScrollStopped,
            'chParam': param
        };
        this.channelList.doScroll(requestScrollParam);
        return true;
    }
    _isSameRequestForScrolling(aScrollParam) {
        if (this._prevScrollParam && aScrollParam) {
            if ((this._prevScrollParam.chParam.ccid === aScrollParam.chParam.ccid) &&
                (this._prevScrollParam.direction === aScrollParam.direction) &&
                (this._prevScrollParam.movingCount === aScrollParam.movingCount)) {
                return true;
            }
        }
        return false;
    }
    _setCurrentRowIndex(index) {
        this.channelList.currentRowIndex = index;
        hx.log('guideView', hx.util.format('_setChannelCurrentRowIndex : {0}', index));
    }
    _debug(programmes) {
        return;
        var i, l, j, ll, now = new Date(), nowTime, programme;
        nowTime = parseInt((now.getTime() / 1000) + "", 10);
        hx.log("guideView", '<================ GUIDE DEBUG VIEW ================');
        for (i = 0, l = programmes.length; i < l; i += 1) {
            for (j = 0, ll = programmes[i].length; j < ll; j += 1) {
                programme = programmes[i][j];
                if (programme.getPluginObject()) {
                    hx.log("guideOnAir", hx.util.format('channel {0}.{1} : name : {2}, currneTime {3}, {4}-{5}, MaxCount {6}', programme.channel.majorChannel, programme.ccid, programme.name, dateFormat(nowTime * 1000, 'HH:MM'), dateFormat(programme.getStartTime() * 1000, 'HH:MM'), dateFormat(programme.getEndTime() * 1000, 'HH:MM'), ll));
                }
            }
        }
        hx.log("guideView", '=========================================================>');
    }
    _setFocusCssStyle(oldClassName, newClassName) {
        var target = this.focusInfo.$item;
        hx.log('guideNext', 'cGuideView-_setFocusCssStyle()');
        if (target) {
            target.removeClass(oldClassName);
            target.addClass(newClassName);
        }
    }
    makeData(param) {
        var curGroup = hx.svc.CH_LIST.getCurrentGrpInfo();
        var channels = hx.svc.CH_LIST.getChlist({
            group : this.channelList.channelGroup
        });
        param.count = channels.chList.length;
        channels = this.uiDataToSrv(channels.chList);
        var now = new Date();
        return {
            'isOnAir': false,
            'channels': channels,
            'now': now,
            'filter': 'schedule'
        };
    }
    getAllProgrammes () {
        var objData: any = {};
        var param:any = {};
        objData = this.makeData(param);
        objData.cb = (result) => {
            var programmes = this.srvDataToUi(result, param.count, this.EDATA_TYPE.EPROGRAMME);
            //this._debug(programmes);
            this.allProgrammes = programmes;
            hx.log('guideView', 'Check');
        }
        hx.svc.EPG.getAllProgrammesOrderByTime(this.name, objData, 200 * 8);
    }
    cellRenewalEnd() {
//        TODO child implement this function
//        throw "TODO child implement this function";
    }
    getProgrammes(aParam) {
//        TODO child implement this function
//        throw "TODO child implement this function";
    }
    changeChannel(aParam): any {  // TODO FIX return type
//        TODO child implement this function
//        throw "TODO child implement this function";
        return false;
    }
    setFocusEnd($old, $new) {
//        TODO child implement this function
//        throw "TODO child implement this function";
    }
    dispTimeHTML(data): string {
//        TODO child implement this function
//        throw "TODO child implement this function";
        return '';
    }
    getThumbClass(data): string {
//        TODO child implement this function
//        throw "TODO child implement this function";
        return '<figure class="thum icoRec">';
    }
    getCurrentProgramme(): any {
        if (this._genreDlg && !this._genreDlg.getOptionDlg() && this.state === this.EVIEW_STATE.EBUSY) {
            return this._genreDlg.getCurrentProgramme();
        }
        return null;
    }
    getGenreDlg(): any {
        return this._genreDlg;
    }
}
export = cGuideView;