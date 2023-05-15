///<reference path="../../def/controls.d.ts" />
///<reference path="../../def/prism.d.ts" />
///<reference path="../../def/sugar.d.ts" />
// file name : fw/common/infra/ilUtil.js
/*jslint sloppy:true, nomen:true, vars:true, regexp:true*/
declare var hx: ifSugar;
//declare var prism: any;
declare var tr: any;
import __MODEL__ = require("model/model");
import __PROVIDER__ = require("pages/util/provider");
import __CONTROLS__ = require("pages/util/controls");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");

module il {

    export var castNumber = function (n) {
        var one = 1;
        return n * one;
    };
    export var sequence = function (ar) {
        var len = ar.length;
        ( function _nextCall(n) {
            if (len < n + 1) {
                return;
            }
            ar[n](function () {
                _nextCall(n + 1);
            });
        }(0));
    };
    export var makeDigitNumber = function (aData:String, digitNumber = 4) {
        var str = '', i = 0;
        for (i=0; i<digitNumber; i+=1) {
            str += '0';
        }
        str += aData;
        return str.substr(str.length - digitNumber, digitNumber);
    };
    export var ani = {
        /**
         * action function
         * @class hx.il.ani
         * @param {string} type - 'in' or 'out'
         * @param {jQuery obj Array} targets - animation target jQuery Obj
         * @param {number} delay - animation start delay time
         * @return undefined
         * */
        getTransition: (): string => {
            var transitionCmd: string = '-o-transition';
            if (hx.emulMode || $.browser.webkit) {
                transitionCmd = '-webkit-transition';
            }
            return transitionCmd;
        },
        getTransitionDuration: (): string => {
            var transitionDuration: string = '-o-transition-duration';
            if (hx.emulMode || $.browser.webkit) {
                transitionDuration = '-webkit-transition-duration';
            }
            return transitionDuration;
        },
        getTransitionEnd: (): string => {
            var transitionEnd: string = 'otransitionend';
            if (hx.emulMode || $.browser.webkit) {
                transitionEnd = 'webkitTransitionEnd';
            }
            return transitionEnd;
        },
        //  Use otransitionend For animation end Callback trigger
        action: (aParam /* {targets, cls, cb, type} */): number => {
            var _targets = aParam.targets, cls = aParam.cls, cb = aParam.cb, isClsArray = Array.isArray(cls), type = (aParam.type === 'in' || !aParam.type) ? 'add' : 'remove', targets = Array.isArray(_targets) ? _targets : [_targets], endStyle = aParam.style || aParam.endStyle, styleStr, startStyle = aParam.startStyle, aniCount = targets.length, aniCls, aniTimeoutID, transition = aParam.transition || null;
            if (startStyle) {
                targets[0].css(ani.getTransitionDuration(), '0ms');
                targets[0].css(startStyle);
            }
            aniTimeoutID = setTimeout(
                (function (targets) {
                    return function() {
                        var i, elem;
                        for (i = 0; i < aniCount; i += 1) {
                            if (!targets[i]) {
                                continue;
                            }
                            elem = targets[i][0];
                            aniCls = (isClsArray) ? cls[i] : cls;
                            // bind transitionend event handler
                            if (cb) {
                                elem.addEventListener(ani.getTransitionEnd(), aniCB(elem, cb), false);
                            }
                            if (transition) {
                                targets[i].css(ani.getTransition(), transition);
                            }
                            // add style
                            if (endStyle && typeof endStyle === 'string') {
                                styleStr = endStyle.split(':');
                                targets[i].css(styleStr[0], styleStr[1]);
                            } else if (endStyle && typeof endStyle === 'object') {
                                targets[i].css(endStyle);
                            }
                            // add class
                            elem.classList[type](aniCls || 'aniActive');
                        }
                    }
                })(targets), aParam.delay || 1);
            function aniCB(elem, cb) {
                return function handler(e) {
                    aniCount -= 1;
                    if (aniCount === 0) {
                        cb();
                    }
                    elem.removeEventListener(ani.getTransitionEnd(), handler, false);
                };
            }

            return aniTimeoutID;
        },
        action2: (aParam /* {targets, cls, cb, type} */): number => {
            var target = aParam.target, cb = aParam.cb, aniInfo = aParam.aniInfo, aniTimeoutID;
            target.css(ani.getTransitionDuration(), '0ms');
            target.css(aniInfo.startStyle || undefined);
            aniTimeoutID = setTimeout(function () {
                var elem = target[0];
                if (cb) {
                    elem.addEventListener(ani.getTransitionEnd(), aniCB(elem, cb), false);
                }
                target.css(ani.getTransition(), aniInfo.transition);
                target.css(aniInfo.endStyle);
            }, aParam.delay || 1);
            function aniCB(elem, cb) {
                return function handler(e) {
                    cb();
                    elem.removeEventListener(ani.getTransitionEnd(), handler, false);
                };
            }

            return aniTimeoutID;
        },
        sequence: (aParam): number[] => {
            var that = ani, seq = that.sequence, handler = [];
            sequenceLoop(aParam);
            function sequenceLoop(q) {
                var elem: any = q.shift(), aniTimeoutID;
                aniTimeoutID = that.action({
                    type: elem.type,
                    targets: elem.targets,
                    cls: elem.cls,
                    delay: elem.delay,
                    cb: function () {
                        if (typeof elem.cb === 'function') {
                            elem.cb();
                        }
                        if (q.length !== 0) {
                            sequenceLoop(q);
                        }
                    }
                });
                handler.push(aniTimeoutID);
            }

            return handler;
        },
        /**
         * bind function
         * @class hx.il.ani
         * @param {object} param - {target : , start : , end : }
         * @return undefined
         * */
        reset: ($target, cssMap): void => {
            $target.css(ani.getTransitionDuration(), '0ms');
            $target.css(cssMap);
        },
        /**
         * bind function
         * @class hx.il.ani
         * @param {object} aParam - {target : , start : , end : }
         * @return undefined
         * */
        bind: (aParam: any): void => {
            var start = aParam.start, end = aParam.end, aniClass = aParam.aniClass || 'aniActive';
            aParam.target.bind(ani.getTransitionEnd(), function (e) {
                var that = this, isActive;
                if (that !== e.target) {
                    return;
                }
                e.stopPropagation();
                isActive = that['classList'].contains(aniClass);
                if (start && isActive) {
                    start.call(this, e);
                } else if (end && !isActive) {
                    end.call(this, e);
                }
            });
        }
    };
    export var circularArray = function (array) {
        if (array) {
            this.setArray(array);
        }
    };
    circularArray.prototype = {
        getArray: function () {
            return this._array;
        },
        setArray: function (array) {
            this._array = array;
            this._idxCur = 0;
            this._idxLast = array.length - 1;
        },
        appendArray: function (array) {
            this._array = this._array.concat(array);
            this._idxLast = array.length - 1;
        },
        cur: function () {
            return this._idxCur;
        },
        curItem: function () {
            return this._array[this._idxCur];
        },
        setCur: function (index) {
            this._idxCur = this.indexing(index);
        },
        getNextItem: function (count) {
            count = typeof count != 'undefined' ? count : 1;
            return this._array[this.indexing(this._idxCur + count)];
        },
        getPrevItem: function (count) {
            count = typeof count != 'undefined' ? count : 1;
            return this._array[this.indexing(this._idxCur - count)];
        },
        getNextItemWithCurIdx: function (index, count) {
            count = typeof count != 'undefined' ? count : 1;
            return this._array[this.indexing(index + count)];
        },
        inc: function (count) {
            count = typeof count != 'undefined' ? count : 1;
            this._idxCur = this.indexing(this._idxCur + count);
        },
        dec: function (count) {
            count = typeof count != 'undefined' ? count : 1;
            this._idxCur = this.indexing(this._idxCur - count);
        },
        indexing: function (index) {
            if (index === 0) {
                return 0;
            } else {
                return index >= 0 ? index % this._array.length : this._array.length + index % this._array.length;
            }
        },
        at: function (index) {
            return index >= 0 ? this._array[index % this._array.length] : this._array[this._array.length + index % this._array.length];
        },
        get: function (count, offset) {
            var CResult = function () {
            };
            CResult.prototype = new Array();
            CResult.prototype.firstAvailIndex = function () {
                var i, a = this;
                for (i = 0; i < this.length; i += 1) {
                    if (a[i])
                        return i;
                }
                return -1;
            };
            CResult.prototype.lastAvailIndex = function () {
                var i, a = this;
                for (i = this.length - 1; i >= 0; i -= 1) {
                    if (a[i])
                        return i;
                }
                return -1;
            };
            var ret = new CResult();
            offset = typeof offset != 'undefined' ? offset : 0;
            var len = this._array.length;
            var i;
            if (count > len) {
                var halfCount = Math.floor(len / 2);
                var centering = halfCount <= offset && halfCount <= count - offset;
                for (i = 0; i < count; i += 1) {
                    ret.push(null);
                }
                if (centering) {
                    var start = offset - halfCount;
                    for (i = 0; i < len; i += 1) {
                        ret[i + start] = this.at(this._idxCur - halfCount + i);
                    }
                } else if (len - 1 <= offset) {// has enough space in upper index.
                    var start = offset - len - 1;
                    for (i = 0; i < len; i += 1) {
                        ret[i + start] = this.at(this._idxCur + i + 1);
                    }
                } else if (len <= count - offset) {// has enough space in lower index.
                    var start: number = offset;
                    for (i = 0; i < len; i += 1) {
                        ret[i + start] = this.at(this._idxCur + i);
                    }
                } else {
                    for (i = 0; i < len; i += 1) {
                        ret[i] = this.at(this._idxCur - offset + i);
                    }
                }
            } else {
                for (i = 0; i < count; i += 1) {
                    ret.push(this.at(this._idxCur - offset + i));
                }
            }
            return ret;
        },
        del: function (index) {
            var compromised = this.indexing(index);
            if (isNaN(compromised)) {
                return;
            }
            this._array.splice(compromised, 1);
        },
        delCurrent: function () {
            var compromised = this.indexing(this.cur());
            if (isNaN(compromised)) {
                return;
            }
            this._array.splice(compromised, 1);
        },
        each: function (fn, offset) {
            var index = offset ? this.indexing(this._idxCur + offset) : this._idxCur;
            var need = false;
            do {
                need = fn(index, this.at(index));
                index = this.indexing(index + 1);
            } while (need && index != this._idxCur);
        },
        eachReverse: function (fn) {
            var index = this._idxCur;
            var need = false;
            do {
                need = fn(index, this.at(index));
                index = this.indexing(index - 1);
            } while (need && index != this._idxCur);
        },
        eachArray: function (fn) {
            var index = this._idxCur;
            var need = false;
            do {
                if (index < this._array.length) {
                    need = fn(index, this.at(index));
                } else {
                    need = fn(-1, undefined);
                }
                index++;
            } while (need);
        },
        length: function () {
            var ret = 0;
            if (this._array) {
                ret = this._array.length;
            }
            return ret;
        },
        clone: function () {
            var ret = new circularArray(this._array.slice(0));
            ret.setCur(this._idxCur);
            return ret;
        }
    };
    export var drawCanvas = function (used, available, xPos, yPos, radiusValue, canvasId) {
        var that = this, rightblock, usednum = Math.round(used), availnum = Math.round(available);
        var x = xPos, y = yPos, radius = radiusValue;
        var canvas;
        var ctx;
        if (canvasId instanceof HTMLCanvasElement) {
            canvas = canvasId;
        } else {
            canvas = document.getElementById(canvasId);
        }
        ctx = canvas.getContext("2d");
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        var myColor = ["#FFFF00", "#414141"];
        var myData = [((usednum / availnum) * 100), (100 - (usednum / availnum) * 100)];

        function getTotal() {
            var j;
            var myTotal = 0;
            for (j = 0; j < myData.length; j += 1) {
                myTotal += ( typeof myData[j] == 'number') ? myData[j] : 0;
            }
            return myTotal;
        }

        function plotData() {
            var lastend = Math.PI * 1.5, myTotal = getTotal(), i;
            for (i = 0; i < myData.length; i += 1) {
                ctx.fillStyle = myColor[i];
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.shadowColor = "#000000";
                ctx.shadowBlur = 10;
                ctx.arc(x, y, radius, lastend, lastend + (Math.PI * 2 * (myData[i] / myTotal)), false);
                ctx.lineTo(x, y);
                ctx.fill();
                lastend += Math.PI * 2 * (myData[i] / myTotal);
            }
        }

        plotData();
        myColor = ["#000000"];
        myData = [100];
        radius = 28;
        plotData();
    };
    export var nowSec = function () {
        var d = new Date();
        return parseInt((d.getTime() / 1000) + '', 10);
    };
    //from utility.js
    export var isInWizardPage = false;
    export var isGoingOperationFromStandby = false;
    export var criticalTasking = {
        crHDDTasking: false,
        crChannelSearchTasking: false,
        crOTATasking: false
    };
    export var isCriticalTasking = function () {
        var isCr = false, a;
        for (a in hx.il.criticalTasking) {
            if (hx.il.criticalTasking[a]) {
                isCr = true;
            }
        }
        return isCr;
    };
    export var currentStatusMsg = {
        vbStatusMsg: false,
        vbStatusPin: false,
        casNagraErrorMsg: false,
        casNagraConfirmMsg: false,
        casNagraPin: false
    };
    export var iso639toString = function (p) {
        switch (p) {
            case 'ara':
                // 'Arabic';
                return hx.l('LOC_LANG_ARABIC_ID');
            case 'cat':
                // 'Catalan';
                return hx.l('LOC_LANG_CATALAN_ID');
            case 'ces':
                // 'Czech';
                return hx.l('LOC_LANG_CZECH_ID');
            case 'dan':
                // 'Danish';
                return hx.l('LOC_LANG_DANISH_ID');
            case 'deu':
            case 'ger':
                // 'German';
                return hx.l('LOC_LANG_GERMAN_ID');
            case 'dut':
            case 'nla':
            case 'nld':
                // 'Dutch';
                return hx.l('LOC_LANG_DUTCH_ID');
            case 'ell':
            case 'gre':
                // 'Greek';
                return hx.l('LOC_LANG_GREEK_ID');
            case 'eng':
                // 'English';
                return hx.l('LOC_LANG_ENGLISH_ID');
            case 'esl':
            case 'esp':
            case 'spa':
                // 'Spanish';
                return hx.l('LOC_LANG_SPANISH_ID');
            case 'fao':
                // 'Faroese';
                return hx.l('LOC_LANG_FAROESE_ID');
            case 'fas':
            case 'per':
                // 'Persian';
                return hx.l('LOC_LANG_PERSIAN_ID');
            case 'fin':
                // 'Finnish';
                return hx.l('LOC_LANG_FINNISH_ID');
            case 'fra':
            case 'fre':
                // 'French';
                return hx.l('LOC_LANG_FRENCH_ID');
            case 'gai':
            case 'gle':
            case 'iri':
                // 'Irish';
                return hx.l('LOC_LANG_IRISH_ID');
            case 'gla':
                // 'Gaelic';
                return hx.l('LOC_LANG_GARLIC_ID');
            case 'hrv':
            case 'scr':
                // 'Croatian';
                return hx.l('LOC_LANG_CROATIAN_ID');
            case 'hun':
                // 'Hungarian';
                return hx.l('LOC_LANG_HUNGARIAN_ID');
            case 'ice':
            case 'isl':
                // 'Icelandic';
                return hx.l('LOC_LANG_ICELANDIC_ID');
            case 'ita':
                // 'Italian';
                return hx.l('LOC_LANG_ITALIAN_ID');
            case 'kor':
                // 'Korean';
                return hx.l('LOC_LANG_KOREAN_ID');
            case 'nor':
                // 'Norwegian';
                return hx.l('LOC_LANG_NORWEGIAN_ID');
            case 'pol':
                // 'Polish';
                return hx.l('LOC_LANG_POLISH_ID');
            case 'por':
                // 'Portuguese';
                return hx.l('LOC_LANG_PORTUGUESE_ID');
            case 'ron':
            case 'rum':
                // 'Romanian';
                return hx.l('LOC_LANG_ROMANIAN_ID');
            case 'rus':
                // 'Russian';
                return hx.l('LOC_LANG_RUSSIAN_ID');
            case 'slk':
            case 'slo':
                // 'Slovak';
                return hx.l('LOC_LANG_SLOVAK_ID');
            case 'slv':
                // 'Slovenian';
                return hx.l('LOC_LANG_SLOVENIAN_ID');
            case 'sqi':
                // 'Albanian';
                return hx.l('LOC_LANG_ALBANIAN_ID');
            case 'sve':
            case 'swe':
                // 'Swedish';
                return hx.l('LOC_LANG_SWEDISH_ID');
            case 'tha':
                // 'Thai';
                return hx.l('LOC_COUNTRY_THAILAND_ID');
            case 'tur':
                // 'Turkish';
                return hx.l('LOC_LANG_TURKISH_ID');
            case 'alb':
            case 'wel':
            case 'cym':
                // 'Welsh';
                return hx.l('LOC_LANG_WELSH_ID');
            case 'und':
                return 'Undefined';
            default:
                return p;
        }
    };
    export var makeChNumToStr = function (chNum) {
        var a = this;
        var strNum = chNum.toString();
        var strLength = strNum.length;
        var tmp_idx;
        for (tmp_idx = 0; tmp_idx < (4 - strLength); tmp_idx += 1) {
            strNum = '0' + strNum;
            // set "0000" (4 order)
        }
        return strNum;
    };
    export var slideTextAnimate = function(target, value, interval) {
        target.animate({scrollLeft: value}, interval, function(){
            target[0].scrollLeft = 0;
        });
    };
    export var convertCountryId = function (countryName) {
        /**********************************************/
        /*******stored this id "hlib_iso3611_1.c"*******/
        /**********************************************/
        var name = countryName;
        var ret = null;
        switch (name) {
            case hx.l('LOC_COUNTRY_GERMANY_ID'):
                ret = "DEU";
                break;
            case hx.l('LOC_COUNTRY_AUSTRIA_ID'):
                ret = "AUT";
                break;
            case hx.l('LOC_COUNTRY_SWITZERLAND_ID'):
                ret = "CHE";
                break;
            case hx.l('LOC_COUNTRY_ITALY_ID'):
                ret = "ITA";
                break;
            default :
                ret = "DEU";
                hx.log('warning', '[convertCountryId] This CountryName is not unknown.');
        }
        return ret;
    };

    export var openWaitingDlg=function(layeredGroupControl, callback?){
        var def = Q.defer();
        var promise = def.promise;

        var waitingDlg = prism.createWaitingNotificationDlg(layeredGroupControl);
        waitingDlg.setTextHtml(tr('LOC_PLEASE_WAIT_ID'));
        waitingDlg.open();
        promise.done(function(){
            if (waitingDlg) {
                if (callback) {
                    callback();
                }
                waitingDlg.close();
                waitingDlg.destroy();
                delete waitingDlg;
            }
        });
        return def;
    };

    export var createProgrammeDetailForSugar = function (param) {
        var parent = param.layeredGroupControl;
        var sugarPage = param.sugarPage;
        var programmeDetailItem = param.pdeItem;
        var channelGroup = param.channelGroup;
        var fnHideOnOpen = param.fnDoHideOnOpen;
        var fnHide = param.fnDoHide;
        var fnShow = param.fnDoShow;
        var onClosed = param.onClosed;
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var dsrvChlist = hx.svc.CH_LIST;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var dsrvParentalCtrl = hx.svc.PARENTAL_CTRL;
        var dsrvDetailInfo = hx.svc.DETAIL_INFO;
        var dsrvBridge = hx.svc.BRIDGE;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        var detailDlg = prism.createProgrammeDetailDlg(parent);
        var dataProvider;
        var i, length;
        var eventName;
        var TScheduledFactoryType = {
            ERECORDING: 0,
            EREMIND: 1,
            EHTTP_LIVE_STREAMING: 4,
            ESERIES_RECORDING: 10
        };
        var TPlayPosition = {
            EBEGIN: 0,
            ELAST_POSITION: 1
        };
        var fnFindItemIndex = function (list, item, instanceType?) {
            var i, length;
            length = list.length;
            for (i = 0; i < length; i += 1) {
                if (instanceType) {
                    if (list[i] instanceof instanceType) {
                        if (list[i].isSame(item)) {
                            return i;
                        }
                    }
                } else {
                    if (list[i].isSame(item)) {
                        return i;
                    }
                }
            }
            return -1;
        };
        if (fnHideOnOpen) {
            fnHideOnOpen(detailDlg);
        }
        var playBackPreviouseFunc: any;
        var self = this;
        var waitingDlg : Q.Deferred<any>;

        if (!(programmeDetailItem instanceof __PROVIDER__.cPdeRecordingItem)) {
            //if live rec, show recording information
            var programmePresent = hx.svc.AV_CTRL.getProgrammePresent();
            if (programmePresent && programmePresent.programmeID === programmeDetailItem.getProgrammeID()) {
                var recording = hx.svc.RECORDINGS.getCurrentRecordingByCcid(programmeDetailItem.getChannelId());
                if (recording) {
                    programmeDetailItem = recording;
                }
            }
        }
        if (!param.waitingDlg){
            waitingDlg = openWaitingDlg(parent);
        } else{
            waitingDlg = param.waitingDlg;
        }
        if (detailDlg.fnAfterTransition) {
            detailDlg.fnAfterTransition = null;
        }
        var query = {
            programmeDetailItem: programmeDetailItem,
            cb: (programmeDetailList, activeIndex) => {
                dataProvider = new __CONTROLS__.CDataProvider(programmeDetailList || []);
                waitingDlg.promise.fin(function(){
                    detailDlg.setProgrammeDetailList(dataProvider, activeIndex);
                    dsrvFrontPanel.setFrontPanelText('');
                    if (!detailDlg._parent) {
                        return;
                    }
                    detailDlg.open({
                        transition: {
                            prevLayer: 'fadeOut',
                            newLayer: 'fadeIn'
                        },
                        onClosed: () => {
                            dsrvRecordings2.clearEventCb(detailDlg);
                            hx.svc.AV_CTRL.clearEventCb(self);
                            if(dataProvider){
                                dataProvider.destroy();
                                delete dataProvider;
                                dataProvider = null;
                            }
                            if (playBackPreviouseFunc) {
                                fnShow();
                                sugarPage.isActive = playBackPreviouseFunc.isActive;
                                sugarPage.visible = playBackPreviouseFunc.visible;
                                sugarPage._on_alMessageToPage = playBackPreviouseFunc._on_alMessageToPage;
                                sugarPage.sendEventToEntity({
                                    alEvent: 'alCheckPageVisible'
                                });
                                delete playBackPreviouseFunc;
                            }
                            if (onClosed) {
                                onClosed();
                            }
                        }
                    });
                });
                if (waitingDlg) {
                    waitingDlg.resolve(null);
                    waitingDlg = null;
                }
                dsrvRecordings2.addEventCb('RecordingCompleted', detailDlg, (param) => {
                    var item = param.item;
                    var pdeRecordingItem = dsrvDetailInfo.createRecordingItem(item);
                        var dataArray = dataProvider.getDataArray();
                        var index = fnFindItemIndex(dataArray, pdeRecordingItem, __MODEL__.IPdeRecordingItem);
                        if (index > -1) {
                            pdeRecordingItem.setRefProgrammeItem(dataArray[index].getRefProgrammeItem ? dataArray[index].getRefProgrammeItem() : null);
                            dataProvider.updateItem(index, pdeRecordingItem);
                        }
                });
                dsrvRecordings2.addEventCb('RecordingItemAdded', detailDlg, (param) => {
                    var item = param.item;
                    var pdeRecordingItem = dsrvDetailInfo.createRecordingItem(item);
                    var dataArray = dataProvider.getDataArray();
                    var index = fnFindItemIndex(dataArray, pdeRecordingItem, __MODEL__.IPdeProgrammeItem);
                    if (index > -1) {
                        pdeRecordingItem.setRefProgrammeItem(dataArray[index].getRefProgrammeItem ? dataArray[index].getRefProgrammeItem() : null);
                        dataProvider.updateItem(index, pdeRecordingItem);
                    }
                });
                dsrvRecordings2.addEventCb('ScheduledItemAdded', detailDlg, (param) => {
                    var item = param.item;
                    var pdeScheduledItem = dsrvDetailInfo.createScheduledItem(item);
                    var fnUpdate = function (targetIndex) {
                        var prevItem = dataProvider.getItem(targetIndex);
                        var pdeProgrammeScheduledItem = dsrvDetailInfo.createProgrammeScheduledItem({
                            programme: prevItem.getSourceItem(),
                            scheduledItem: pdeScheduledItem.getOipfScheduledInstance(),
                        });
                        if (prevItem.isSeries() === true) {
                            pdeProgrammeScheduledItem.setDummyGroup();
                        }
                        dataProvider.updateItem(targetIndex, pdeProgrammeScheduledItem);
                    };
                    var dataArray = dataProvider.getDataArray();
                    if (pdeScheduledItem.isScheduledAsSeries()) {
                        var i;
                        for (i = 0; i < dataArray.length; i += 1) {
                            var pdeItem = dataArray[i];
                            if (pdeItem instanceof __MODEL__.IPdeProgrammeItem) {
                                if (pdeItem.isSame(pdeScheduledItem)) {
                                    fnUpdate(i);
                                }
                            }
                        }
                        if (dataArray.length === 1) {
                            dataProvider.updateItem(0, null);
                        }
                    } else {
                        var index = fnFindItemIndex(dataArray, pdeScheduledItem, __MODEL__.IPdeProgrammeItem);
                        if (index > -1) {
                            fnUpdate(index);
                        }
                        if (dataArray.length === 1) {
                            dataProvider.updateItem(0, null);
                        }
                    }
                });
                dsrvRecordings2.addEventCb('RecordingItemUpdated', detailDlg, (param) => {
                    var item = param.item;
                    var pdeRecordingItem = dsrvDetailInfo.createRecordingItem(item);
                    var dataArray = dataProvider.getDataArray();
                    var index = fnFindItemIndex(dataArray, pdeRecordingItem, __MODEL__.IPdeRecordingItem);
                    if (index > -1) {
                        var oldItem = dataArray[index];
                        if (oldItem.getLastPlayPosition() !== pdeRecordingItem.getLastPlayPosition()) {
                            pdeRecordingItem.setRefProgrammeItem(oldItem.getRefProgrammeItem ? oldItem.getRefProgrammeItem() : null);
                            dataProvider.updateItem(index, pdeRecordingItem);
                        }
                    }
                });
                dsrvRecordings2.addEventCb('RecordingItemRemoved', detailDlg, (param) => {
                    var item = param.item;
                    var pdeRecordingItem = dsrvDetailInfo.createRecordingItem(item);
                    var dataArray = dataProvider.getDataArray();
                    var index = fnFindItemIndex(dataArray, pdeRecordingItem, __MODEL__.IPdeRecordingItem);
                    if (index > -1) {
                        var prevProgItem = dataArray[index].getRefProgrammeItem ? dataArray[index].getRefProgrammeItem() : null;
                        if (prevProgItem) {
                            dataProvider.updateItem(index, dsrvDetailInfo.createProgrammeItem(prevProgItem));
                        } else {
                            dataProvider.removeItem(index);
                        }
                    }
                });
                dsrvRecordings2.addEventCb('ScheduledItemRemoved', detailDlg, (param) => {
                    var item = param.item;
                    var pdeScheduledItem = dsrvDetailInfo.createScheduledItem(item);
                    var fnRemove = function (targetIndex) {
                        var prevItem = dataProvider.getItem(targetIndex);
                        if (prevItem instanceof __MODEL__.IPdeProgrammeItem) {
                            var oipfProgramme = prevItem.getSourceItem();
                            var programItem = dsrvDetailInfo.createProgrammeItem(oipfProgramme);
                            if (prevItem.isSeries() === true) {
                                programItem.setDummyGroup();
                            }
                            programItem.setMdsData(prevItem.getMdsData());
                            dataProvider.updateItem(targetIndex, programItem);
                        } else if (prevItem instanceof __MODEL__.IPdeRecordingItem) {
                            var noWork;
                        } else if (prevItem instanceof __MODEL__.IPdeScheduledItem) {
                            dataProvider.removeItem(targetIndex);
                        }
                    };
                    var dataArray = dataProvider.getDataArray();
                    if (pdeScheduledItem.isScheduledAsSeries()) {
                        var i;
                        if (dataArray.length === 1) {
                            dataProvider._fireEvent('ItemChanged', [0]);
                        }
                        for (i = 0; i < dataArray.length; i += 1) {
                            var pdeItem = dataArray[i];
                            if (pdeItem.isSame(pdeScheduledItem)) {
                                fnRemove(i);
                            }
                        }
                    } else {
                        var index = fnFindItemIndex(dataArray, pdeScheduledItem);
                        if (dataArray.length === 1) {
                            dataProvider._fireEvent('ItemChanged', [0]);
                        }
                        if (index > -1) {
                            fnRemove(index);
                        }
                    }
                });
            }
        };
        dsrvDetailInfo.getProgrammeDetailList(query);
        detailDlg.onRequestPlayback = function (item, detailDlg) {
            var id, uid, data;
            var btnY, btnN, textHtml, btnDlg, isLastPosition = TPlayPosition.EBEGIN;
            var i, length;
            var DTG_ask_password : boolean = false;

            id = item.getId();
            var fnCreateQueryPasswordDlg = function () {
                var passwordDlg = prism.createQueryPasswordDlg(detailDlg._parent);
                passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                return passwordDlg;
            };
            var fnCreateButtonDlg = function (fn) {
                btnY = tr('LOC_YES_ID');
                btnN = tr('LOC_BTN_PLAYREC_ID');
                textHtml = tr('LOC_MSG_PLAYREC_ID');
                btnDlg = prism.createButtonDlg(detailDlg._parent, {
                    textHtml: textHtml,
                    buttonList: [btnY, btnN],
                    timeOutValue: 30000
                }, fn);
                btnDlg.setButtonLayoutType('vertical');
                return btnDlg;
            };
            var fnStartPlayback = function (isLastPosition?) {
                var recordingItem = item.getSourceItem();
                var items = [];
                var startIndex = 0;
                var groupCRIDs, seriesList, id, i, length;
                //dsrvRecordings2
                if (recordingItem.isSeries()) {
                    groupCRIDs = recordingItem.getGroupCRIDs();
                    seriesList = dsrvRecordings2.getSeriesList(groupCRIDs);
                    id = recordingItem.getId();
                    if (seriesList.length !== 0) {
                        items = dsrvRecordings2.arrangeSeriesList(seriesList);
                        length = items.length;
                        for (i = 0; i < length; i += 1) {
                            if (id === items[i].getId()) {
                                startIndex = i;
                                break;
                            }
                        }
                    } else {
                        items = [recordingItem];
                    }
                } else {
                    items = [recordingItem];
                }
                dsrvBridge.requestVideoPlayback(items, startIndex, isLastPosition);
            };
            var fnShowRecErrorMessage = function (error) {
                var errorText, bValidOnNetflix;
                errorText = dsrvRecordings2.getErrorText(error);
                if (errorText ===  hx.l('LOC_VIDEO_RECORD_FAIL_STORAGE_FULL')) {
                    bValidOnNetflix = true;
                }
                if (errorText !== '') {
                    hx.msg('response', {
                        text: errorText,
                        auto_close: true,
                        close_time: 3000,
                        validOnNetflix: bValidOnNetflix
                        //dia_class : string( 'osd' : background dim transaction )
                    });
                }
            };
            if (ENV.op === ENV.listOP.UKDTT) {
                var guidancePolicy : number = hx.svc.SETTING_UTIL.getGuidancePolicy();
                var watersheds : boolean = false;
                var now : any = new Date();
                var min : number = now.getMinutes();
                var hour : number = now.getHours();
                var time : number = hour * 60 + min;
                if (time >= 1260 || time < 330) {   //before 05:30 or after 21:00
                    watersheds = true;
                }
                if (watersheds === true) {
                    DTG_ask_password = false;
                } else {
                    switch (guidancePolicy) {
                        case 0: // eDxGUIDANCEPOLICY_OFF
                            DTG_ask_password = false;
                            break;
                        case 1: // eDxGUIDANCEPOLICY_MARKEDCONTENTS
                            DTG_ask_password = !!item.hasGuidance();
                            break;
                        case 2: // eDxGUIDANCEPOLICY_INTERNETCONTENTS
                            DTG_ask_password = false;
                            break;
                        case 3: // eDxGUIDANCEPOLICY_ALLCONTENTS
                            DTG_ask_password = true;
                            break;
                        default:
                            break;
                    }
                }
            }
            if (item.isLocked() || (DTG_ask_password === true)) {
                var passwordDlg = fnCreateQueryPasswordDlg();
                var CTsrItem = new __MODEL__.CTsrItem(null);
                passwordDlg.open({
                    dimm : true,
                    onPasswordEntered : function(password) {
                        var result = dsrvParentalCtrl.verifyParentalControlPIN({
                            password : password
                        });
                        if (result && (result.result === 0)) {
                            //detailDlg._$self.find('.vod img').attr('src', item.getThumbnailUrl());
                            passwordDlg.close();
                            if (CTsrItem.isChase()) {
                                setPCStatus(password, false);
                            }
                            fnStartPlayback();
                        } else {
                            passwordDlg.reset();
                        }
                    }
                });
                var setPCStatus = function(pcPIN, enable) {
                    dsrvParentalCtrl.setParentalControlStatus({
                        pcPIN : pcPIN,
                        enable : enable
                    });
                };
            } else {
                var bBroken = item._prismRecordingItem.isBroken();
                var bNew = item._prismRecordingItem.isNew();
                if (bBroken && bNew) {
                    var error = item._prismRecordingItem.getError();
                    fnShowRecErrorMessage(error);
                }
                fnStartPlayback();
            }
        };
        detailDlg.onRequestLive = function (item) {
            var oipfChannel, changeable;
            oipfChannel = item.getChannelInstance();
            changeable = dsrvChlist.checkChannelChangable({
                channel: oipfChannel
            });
            if (!changeable) {
                var notiDlg;
                if (hx.config.useTVStreaming === false) {
                    notiDlg = prism.createNotificationDlg(null, hx.l('LOC_CANNOTCHANGEWHILEREC_ID'));
                } else {
                    notiDlg = prism.createNotificationDlg(null, hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'));
                }
                notiDlg.open();
                return;
            }
            hx.svc.BRIDGE.requestStopPlayback();
            sugarPage.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController?refresh=true',
                me: sugarPage
            });
            dsrvAvCtrl.channelChange({
                channel: oipfChannel,
                group: channelGroup || ((oipfChannel.channelType === 0) ? 'TV' : 'RADIO')
            });
            //sugarPage.destroy();
        };
        detailDlg.onRequestRemoveRecording = function (item, forced?) {
            var fnCreateQueryPasswordDlg = function() {
                var passwordDlg = prism.createQueryPasswordDlg(detailDlg._parent);
                passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                return passwordDlg;
            };
            var fnDeleteRecording = () => {
                if (!item) {
                    hx.log('error', 'detailDlg.onRequestRemoveRecording : ' + 'error no item !!');
                    return;
                }
                var type = item.getType();
                if (type === 'recording') {
                    dsrvRecordings2.removeRecording({
                        items: [item.getSourceItem()],
                        finished: function () {
                            prism.log('finished removeRecording');
                        }
                    });
                } else if (type === 'scheduled') {
                    var oipfScheduled = item.getOipfScheduledInstance();
                    dsrvRecordings2.removeScheduledRecording({
                        items: [dsrvRecordings2.createScheduledItem(oipfScheduled)],
                        finished: function () {
                            prism.log('finished removeScheduledRecording');
                        }
                    });
                }
            }
            if (item.isLocked() && !forced) {
                var passwordDlg = fnCreateQueryPasswordDlg();
                passwordDlg.open({
                    dimm : true,
                    onPasswordEntered : function(password) {
                        var result = dsrvParentalCtrl.verifyParentalControlPIN({
                            password : password
                        });
                        if (result && (result.result === 0)) {
                            detailDlg._$self.find('.vod img').attr('src', item.getThumbnailUrl());
                            passwordDlg.close();
                            fnDeleteRecording();
                        } else {
                            passwordDlg.reset();
                        }
                    }
                });
            } else {
                fnDeleteRecording();
            }

        };
        detailDlg.onRequestKeepRecording = function (item) {
            if (!item) {
                hx.log('error', 'detailDlg.onRequestKeepRecording : ' + 'error no item !!');
                return;
            }
            var type = item.getType();
            if (type === 'recording') {
                dsrvRecordings2.setKeep({
                    items : [item.getSourceItem()],
                    keep : true,
                    finished: function () {
                        prism.log('finished keepRecording');
                    }
                });
                /*
                dsrvRecordings2.removeRecording({
                    items: [item.getSourceItem()],
                    finished: function () {
                        prism.log('finished keepRecording');
                    }
                });  */
            }
        };
        detailDlg.onRequestStopRecording = function (item) {
            var sourceItem = item.getSourceItem();
            dsrvRecordings2.stopRecord({
                item: sourceItem,
                finished: function () {
                    prism.log('finished stopRecord');
                }
            });
        };
        detailDlg.onRequestClose = function () {
            detailDlg.close();
        };
        var fnRequestReservation = function (param, finished?) {
            var isTimebased = param.isTimebased;
            var programme = param.programme;
            var factoryType = param.factoryType;
            if (!( programme instanceof __MODEL__.IScheduledItem)) {
                dsrvRecordings2.record({
                    programme: programme,
                    factoryType: factoryType,
                    finished: function () {
                        if (finished) {
                            finished();
                        }
                    }
                });
            } else {
                if (isTimebased) {
                    var scheduleParam = {
                        startTime: Math.floor(programme.getStartTime() / 1000),
                        duration: programme.getDuration(),
                        repeatDays: programme.getRepeat(),
                        ccid: programme.getChannelId(),
                        finished: function (result) {
                            if (finished) {
                                finished();
                            }
                        }
                    };
                    if (factoryType !== TScheduledFactoryType.ERECORDING) {
                        dsrvRecordings2.recordAtReminder(scheduleParam);
                    } else {
                        dsrvRecordings2.recordAtRecording(scheduleParam);
                    }
                } else {
                    var dsrvEpg = hx.svc.EPG;
                    dsrvEpg.findProgrammes({
                        channel: [programme.getChannelInstance()],
                        clause: 'programmeID == ' + programme.getProgrammeID(),
                        isOnAir: false,
                        cb: function (result) {
                            if (result.length <= 0) {
                                result = undefined;
                                return;
                            }
                            var eventArray, programmeItem, thumbnailUrl;
                            eventArray = result[0];
                            if (eventArray && eventArray.length) {
                                programmeItem = eventArray[0];
                                if (programmeItem) {
                                    dsrvRecordings2.record({
                                        programme: programmeItem.getPluginObject(),
                                        factoryType: factoryType,
                                        finished: function () {
                                            if (finished) {
                                                finished();
                                            }
                                        }
                                    });
                                }
                            }
                            result = undefined;
                        }
                    });
                }
            }
        };

        detailDlg.onRequestRecord = function (item) {
            var type = item.getType();
            var programme = item.getPluginObject ? item.getPluginObject() : null;
            if (!programme) {
                hx.log('error', '[onRequestRecord] (!programme)');
                return;
            }
            if ((type === 'scheduled') && item.isReminder()) {
                var oipfScheduled = item.getOipfScheduledInstance();
                dsrvRecordings2.removeScheduledRecording({
                    items: [dsrvRecordings2.createScheduledItem(oipfScheduled)],
                    finished: function () {
                        prism.log('finished removeScheduledRecording');
                        setTimeout(function () {
                            fnRequestReservation({
                                isTimebased: item.isTimebased(),
                                programme: programme,
                                factoryType: TScheduledFactoryType.ERECORDING
                            });
                        }, 1000);
                    }
                });
            } else {
                fnRequestReservation({
                    programme: programme,
                    factoryType: TScheduledFactoryType.ERECORDING
                });
            }
        };
        detailDlg.onRequestCancelRecord = function (item) {
            var type = item.getType();
            dsrvRecordings2.stopRecord({
                item: (type === __MODEL__.IProgrammeDetailItem.type.scheduled ? dsrvRecordings2.createScheduledItem(item.getOipfScheduledInstance()) : dsrvRecordings2.createRecordingItem(item.getOipfRecordingInstance())),
                finished: function () {
                    prism.log('finished stopRecord');
                }
            });
        };
        detailDlg.onRequestRecordWholeSeries = function (item) {
            var programme = item.getPluginObject ? item.getPluginObject() : null;
            if (!programme) {
                hx.log('error', '[onRequestRecordWholeSeries] (!programme)');
                return;
            }
            var i, length, tempItem;
            var oipfScheduled;
            var removeScheduledList = [];
            length = dataProvider.getCount();
            for (i = 0; i < length; i += 1) {
                tempItem = dataProvider.getItem(i);

                if (tempItem.getType() === 'recording') {
                    prism.log('[onRequestRecordWholeSeries] [ ' + i + '], Recording Item Skip...');
                    continue;
                }
                switch (tempItem.getType()) {
                    case 'scheduled':
                        oipfScheduled = tempItem.getOipfScheduledInstance();
                        removeScheduledList.push(dsrvRecordings2.createScheduledItem(oipfScheduled));
                        break;
                }
            }
            if (removeScheduledList.length) {
                dsrvRecordings2.removeScheduledRecording({
                    items: removeScheduledList,
                    finished: () => {
                        prism.log('finished removeScheduledRecording');
                        setTimeout(() => {
                            dsrvRecordings2.record({
                                programme: programme,
                                factoryType: TScheduledFactoryType.ESERIES_RECORDING,
                                finished: function () {
                                    prism.log('finished record');
                                }
                            });
                        }, 1000);
                    }
                });
            } else {
                dsrvRecordings2.record({
                    programme: programme,
                    factoryType: TScheduledFactoryType.ESERIES_RECORDING,
                    finished: function () {
                        prism.log('finished record');
                    }
                });
            }
        };
        detailDlg.fnGetSameSeriesScheduleList = function(item): any {
            var i, j, k;
            var ret = [];
            var schedulelist = dsrvRecordings2.getScheduledList();
            for (i = 0; i < schedulelist.length; i += 1) {
                if (schedulelist[i].getGroupCRIDs() && item.getGroupCRIDs()) {
                    if (item.getGroupCRIDs().length > 0 && schedulelist[i].getGroupCRIDs().length > 0) {
                        for (j = 0; j < item.getGroupCRIDs().length; j += 1) {
                            for (k = 0 ; k < schedulelist[i].getGroupCRIDs().length ; k++) {
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
        }
        detailDlg.onRequestCancelRecordWholeSeries = function (item) {
            if (!item) {
                hx.log('error', 'detailDlg.onRequestRemoveRecording : ' + 'error no item !!');
                return;
            }
            var scheduleList = [];

            if (item.getGroupCRIDs().length > 0) {
                scheduleList = this.fnGetSameSeriesScheduleList(item);
                if (scheduleList.length > 0) {
                    dsrvRecordings2.removeWholeSeriesSchedule({
                        items: scheduleList,
                        finished: function () {
                            prism.log('finished removeScheduledRecording');
                        }
                    });
                }
            }
        };
        detailDlg.onRequestSetReminder = function (item) {
            var programme = item.getPluginObject ? item.getPluginObject() : null;
            if (!programme) {
                hx.log('error', '[onRequestSetReminder] (!programme)');
                return;
            }
            dsrvRecordings2.record({
                programme: programme,
                factoryType: TScheduledFactoryType.EREMIND,
                finished: function () {
                    prism.log('finished record');
                }
            });
        };
        detailDlg.onRequestCancelReminder = function (item) {
            var type = item.getType();
            if (type === 'scheduled') {
                var oipfScheduled = item.getOipfScheduledInstance();
                dsrvRecordings2.removeScheduledRecording({
                    items: [dsrvRecordings2.createScheduledItem(oipfScheduled)],
                    finished: function () {
                        prism.log('finished removeScheduledRecording');
                    }
                });
            } else {
                throw 'Invalid Param';
            }
        };
        detailDlg.onRequestGoLiveController = function() {
            sugarPage.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController?refresh=true',
                me: sugarPage
            });
        };
        detailDlg.onRequestCloseDetail = function() {
            if (sugarPage._calledPath === "CPgInfoHub") {
                sugarPage._hideAndDestroy();
            }
        };
        //hx.svc.AV_CTRL.addEventCb("ProgrammesChanged", this, function(aProgramme: __MODEL__.cPdeProgrammeItem) {
        //    if (!(programmeDetailItem instanceof __MODEL__.cPdeRecordingItem)) {
        //        detailDlg.updateEvent(aProgramme);
        //    }
        //});
        detailDlg.onRequestMdsSchedule = function(aPdeItem, aIndex) {
            var sourceItem = aPdeItem.getSourceItem();
            if (sourceItem && sourceItem.channel) {
                hx.svc.IP_EPG.getSchedules(sourceItem.channel.sid, sourceItem.getStartTime() * 1000, 6).then((res) => {
                    var info = hx.svc.IP_EPG.parse.xml.schedule(res);
                    var schedules = Array.prototype.slice.call(info.schedules);
                    var crids = info.crids;
                    var length = 0;
                    var parsedId;
                    var onNowLegacyId, mdsLegacyId;
                    var mdsEvent;
                    var schedule = null, crid = null, data = null;
                    if (schedules) {
                        length = schedules.length;
                    }
                    parsedId = hx.svc.IP_EPG.parse.util.programIdParser(sourceItem.getProgrammeID());
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
                                hx.svc.IP_EPG.getDetailInfo(crid).then((info) => {
                                    data.program = info.querySelector('ProgramInformation');
                                    aPdeItem.setMdsData({
                                        mdsData : data,
                                        launchLocation : 'epg'
                                    });
                                    aPdeItem.getThumbnailUrl(() => {
                                        detailDlg.getProgrammeDetailProvider().updateItem(aIndex);
                                    });
                                }, (err) => {
                                    aPdeItem.setMdsData({
                                        mdsData : data,
                                        launchLocation : 'epg'
                                    });
                                    aPdeItem.getThumbnailUrl(() => {
                                        detailDlg.getProgrammeDetailProvider().updateItem(aIndex);
                                    });
                                });
                                break;
                            }
                        }
                    }
                });
            }
        };
        return detailDlg;
    };
    export var adhocPageParam = ( function () {
        var messageQ = [];
        return {
            push: function (param) {
                if (messageQ.length) {
                    throw "arguements already exists";
                }
                messageQ.push(param);
            },
            pop: function () {
                return messageQ.shift();
            }
        };
    }());
    export var clearPage = function (where) {
        if (where && where.sendEventToEntity) {
            var result = hx.svc.CH_LIST.getChannelCount();
            if (result.chCnt > 0) {// not no sdb
                where.sendEventToEntity({
                    'alEvent' : 'alChangePage',
                    'target' : 'CPgLiveController?refresh=true',
                    'me' : where
                });
                where.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgInfoHub?backToLive',
                    me : where
                });
                where.sendEventToEntity({
                    alEventl : 'alMessageToPage',
                    target : 'CPgChInfo?hide',
                    me : where
                });
                where.sendEventToEntity({
                    'alEvent' : 'alMessageToPage',
                    'target' : 'CPgLiveController?hideerrorstate',
                    'me' : where
                });
            } else {
                where.sendEventToEntity({
                    'alEvent' : 'alMessageToPage',
                    'target' : 'CPgLiveController?hideNoChannelConfirm',
                    'me' : where
                });
            }
        }
    }
    export var launchApp = function (name, appId?, fnError?, where?, data?) {
        if (hx.config.netflix && name === "Netflix") {
            var launchType = data || __NETFLIX__.START_TYPE.DEDICATED_ON_SCREEN_ICON;
            clearPage(where);
            hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
            hx.svc.NETFLIX.start({
                type: launchType
            });
        } else {
            hx.svc.TV_PORTAL.getApplicationUrl({
                applicationId : appId,
                fn : function(app) {
                    if (app) {
                        if (app.Status === true) {
                            if (hx.config.netflix && app.Url.substr(0, 6) === 'native') {
                                clearPage(where);
                                hx.svc.APP_MGR.destroyHbbTvDeeplinkApplication();
                                if (name === "Netflix") {
                                    hx.svc.NETFLIX.start({
                                        type: __NETFLIX__.START_TYPE.DEDICATED_ON_SCREEN_ICON
                                    });
                                }
                            } else {
                                hx.il.openDeepLink(name, app.Url, undefined, undefined, undefined, where);
                            }
                        } else {
                            // show message when fail to get linked application URL
                            hx.log('info', 'MessageType: ' + app.MessageType);
                            var showMessage = function() {
                                hx.log('info', 'len: ' + app.Messages.length);
                                var message = app.Messages.shift();
                                if (message) {
                                    var buttonDlg = prism.createButtonDlg(null, {
                                        textHtml : message
                                    });
                                    buttonDlg.open({
                                        dimm : true,
                                        onClosed : function() {
                                            setTimeout(function() {
                                                showMessage();
                                            }, 1);
                                        }
                                    });
                                }
                            };
                            showMessage();
                        }
                    }
                },
                fnError : function(result) {
                    if (fnError) {
                        fnError();
                    }
                }
            });
        }
    };
    export var openDeepLink = function (name, url?, onLoaded?, onUnloaded?, onRequestChangePage?, where?) {
        clearPage(where);
        hx.svc.AV_CTRL.blockAV({
            'deepLink' : true,
            'cb': function () {
                hx.svc.APP_MGR.createHbbTvDeeplinkApplication({
                    name :name,
                    url : url
                });
            }
        });
    };
    export var isOverMaxResolution = function (resolution) {
        var ret = false;
        if (resolution) {
            var res = resolution.split('x');
            var x = parseInt(res[0], 10) || 1;
            var y = parseInt(res[1], 10) || 1;
            if (x >= 8000 || y >= 8000 || (x * y) >= (3300 * 2500)) {
                ret = true;
            }
        }
        return ret;
    };
    export var getSubtitleLanguageText = function (code, index) {
        var text = '';
        switch (code) {
            case 'en-EN':
            case 'en-US':
                text = hx.l('LOC_LANG_ENGLISH_ID');
                break;
            case 'de-DE':
                text = hx.l('LOC_LANG_GERMAN_ID');
                break;
            case 'ko-KR':
                text = hx.l('LOC_LANG_KOREAN_ID');
                break;
            default:
                text = hx.l('LOC_SUBTITLE_ID') + ' ' + index;
                break;
        }
        return text;
    };
    export var genreList = {
        bInit: false,
        _images : {
            "0" : 'images/614_Thumb_Movies.jpg',
            "1" : 'images/614_Thumb_News.jpg',
            "2" : 'images/614_Thumb_Entertainment.jpg',
            "3" : 'images/614_Thumb_Sport1.jpg',
            "4" : 'images/614_Thumb_Children.jpg',
            "5" : 'images/614_Thumb_Entertainment.jpg',
            "6" : 'images/614_Thumb_News.jpg',
            "7" : 'images/614_Thumb_News.jpg',
            "8" : 'images/614_Thumb_Education.jpg',
            "9" : 'images/614_Thumb_Lifestyle.jpg',
            "14" : 'images/614_Thumb_Drama.jpg',
            "Unclassified": 'images/614_Thumb_Genre.jpg',
            "Movie": 'images/614_Thumb_Movies.jpg',
            "News and Factual": 'images/614_Thumb_News.jpg',
            "News/Current affairs": 'images/614_Thumb_News.jpg',
            "Arts/Culture (without music)": 'images/614_Thumb_News.jpg',
            "Social/Political Issues/Economics": 'images/614_Thumb_News.jpg',
            "Entertainment": 'images/614_Thumb_Entertainment.jpg',
            "Show/Game show": 'images/614_Thumb_Entertainment.jpg',
            "Music/Ballet/Dance": 'images/614_Thumb_Entertainment.jpg',
            "Sport": 'images/614_Thumb_Sport1.jpg',
            "Children's/Youth's programming": 'images/614_Thumb_Children.jpg',
            "Education/Science/Factual topics": 'images/614_Thumb_Education.jpg',
            "Lifestyle/Leisure hobbies": 'images/614_Thumb_Lifestyle.jpg',
            "Drama": 'images/614_Thumb_Drama.jpg'
        },
        majorGenreList: null,
        minorGenreList: null,
        majorGenreAllIDList: null,
        minorGenreAllIDList: null,
        getGenreList: function (type, bNeedAllId) {
            var that = this;
            if (that.bInit === false) {
                ( function () {
                    if (ENV.op === ENV.listOP.AUSDTT) {
                        that.majorGenreList = ['LOC_GENRE_AUS_MOVIE_ID', 'LOC_GENRE_AUS_NEWS_ID', 'LOC_GENRE_AUS_ENTERTAINMENT_ID', 'LOC_GENRE_AUS_SPORT_ID',
                            // //
                            'LOC_GENRE_AUS_CHILDRENS_ID', 'LOC_GENRE_AUS_MUSIC_ID', 'LOC_GENRE_AUS_ARTS_CULTURE_ID', 'LOC_GENRE_AUS_CURRENT_AFFAIRS_ID',
                            // //
                            'LOC_GENRE_AUS_EDU_INFO_ID', 'LOC_GENRE_AUS_INFOTAINMENT_ID', 'LOC_GENRE_AUS_SPECIAL_ID', 'LOC_GENRE_AUS_COMEDY_ID',
                            // //
                            'LOC_GENRE_AUS_DRAMA_ID', 'LOC_GENRE_AUS_DOCUMENTARY_ID'];
                        // not use minorGenre
                        that.minorGenreList = [];
                        that.majorGenreAllIDList = ['LOC_ALL_ID', 'LOC_GENRE_AUS_MOVIE_ID', 'LOC_GENRE_AUS_NEWS_ID', 'LOC_GENRE_AUS_ENTERTAINMENT_ID', 'LOC_GENRE_AUS_SPORT_ID',
                            // //
                            'LOC_GENRE_AUS_CHILDRENS_ID', 'LOC_GENRE_AUS_MUSIC_ID', 'LOC_GENRE_AUS_ARTS_CULTURE_ID', 'LOC_GENRE_AUS_CURRENT_AFFAIRS_ID',
                            // //
                            'LOC_GENRE_AUS_EDU_INFO_ID', 'LOC_GENRE_AUS_INFOTAINMENT_ID', 'LOC_GENRE_AUS_SPECIAL_ID', 'LOC_GENRE_AUS_COMEDY_ID',
                            // //
                            'LOC_GENRE_AUS_DRAMA_ID', 'LOC_GENRE_AUS_DOCUMENTARY_ID'];
                        that.minorGenreAllIDList = [];
                    } else if (ENV.op === ENV.listOP.UKDTT) {
                        that.majorGenreList = [ 'LOC_GENRE_MOVIE_ID', 'LOC_GENRE_NEWS_AND_FACTUAL_ID', 'LOC_GENRE_AUS_ENTERTAINMENT_ID', 'LOC_GENRE_AUS_SPORT_ID',
                            // //
                            'LOC_GENRE_AUS_CHILDRENS_ID', 'LOC_GENRE_AUS_ENTERTAINMENT_ID', 'LOC_GENRE_NEWS_AND_FACTUAL_ID', 'LOC_GENRE_NEWS_AND_FACTUAL_ID',
                            // //
                            'LOC_GENRE_EDUCATION_ID', 'LOC_GENRE_LIFESTYLE_ID', '', '',
                            // //
                            '', '', 'LOC_GENRE_AUS_DRAMA_ID'];
                        // not use minorGenre
                        that.minorGenreList = [];
                        that.majorGenreAllIDList = ['LOC_ALL_ID', 'LOC_GENRE_MOVIE_ID', 'LOC_GENRE_NEWS_AND_FACTUAL_ID', 'LOC_GENRE_AUS_ENTERTAINMENT_ID', 'LOC_GENRE_AUS_SPORT_ID',
                            // //
                            'LOC_GENRE_AUS_CHILDRENS_ID',
                            // //
                            'LOC_GENRE_EDUCATION_ID', 'LOC_GENRE_LIFESTYLE_ID',
                            // //
                             'LOC_GENRE_AUS_DRAMA_ID'];
                        that.minorGenreAllIDList = [];

                    } else {
                        that.majorGenreList = ['LOC_GENRE_MOVIE_ID', 'LOC_GENRE_NEWS_ID', 'LOC_GENRE_SHOW_ID', 'LOC_GENRE_SPORTS_ID', 'LOC_GENRE_CHILDREN_ID', 'LOC_GENRE_MUSIC_ID', 'LOC_GENRE_ARTS_ID', 'LOC_GENRE_SOCIAL_ID', 'LOC_GENRE_EDUCATION_ID', 'LOC_GENRE_LEISURE_ID', 'LOC_GENRE_SPECIAL_CHARACTER_ID'];
                        // //
                        that.minorGenreList = [
                            ['LOC_GENRE_MOVIE_DRAMA_ID', 'LOC_GENRE_DETECTIVE_THRILLER_ID', 'LOC_GENRE_ADVENTURE_WESTERN_WAR_ID', 'LOC_GENRE_SF_FANSTASY_HORROR_ID', 'LOC_GENRE_COMEDY_ID', 'LOC_GENRE_SOAP_MELO_FOLKLORIC_ID', 'LOC_GENRE_ROMANCE_ID', 'LOC_GENRE_SERIOUS_DRAMA_ID', 'LOC_GENRE_ADULT_MOVIE_DRAMA_ID'],
                            ['LOC_GENRE_NEWS_CURRENT_AFFAIRS_ID', 'LOC_GENRE_NEWS_WEATHER_REPORT_ID', 'LOC_GENRE_NEWS_MAGAZINE_ID', 'LOC_GENRE_DOCUMENTARY_ID', 'LOC_GENRE_INTERVIEW_ID'],
                            ['LOC_GENRE_SHOW_GAME_ID', 'LOC_GENRE_SHOW_QUIZ_CONTEST_ID', 'LOC_GENRE_VARIETY_SHOW_ID', 'LOC_GENRE_TALK_SHOW_ID'],
                            ['LOC_GENRE_SPECIAL_EVENTS_ID', 'LOC_GENRE_SPORTS_MAGAZINE_ID', 'LOC_GENRE_FOOTBALL_SOCCER_ID', 'LOC_GENRE_TENNIS_SQUASH_ID', 'LOC_GENRE_TEAM_SPROTS_ID', 'LOC_GENRE_ATHLETICS_ID', 'LOC_GENRE_MOTOR_SPORT_ID', 'LOC_GENRE_WATER_SPORT_ID', 'LOC_GENRE_WINTER_SPORTS_ID', 'LOC_GENRE_EQUESTRIAN_ID', 'LOC_GENRE_MARTIAL_SPORTS_ID'],
                            ['LOC_GENRE_CHILDREN_YOUTH_ID', 'LOC_GENRE_PRESCHOOL_CHILDREN_ID', 'LOC_GENRE_6TO4_ENTERTAIN_ID', 'LOC_GENRE_10TO16_ENTERTAIN_ID', 'LOC_GENRE_INFO_EDU_SCHOOL_ID', 'LOC_GENRE_CARTOONS_PUPPETS_ID'],
                            ['LOC_GENRE_MUSIC_DANCE_ID', 'LOC_GENRE_ROCK_POP_ID', 'LOC_GENRE_CLASSIC_ID', 'LOC_GENRE_FOLK_TRADITIONAL_ID', 'LOC_GENRE_JAZZ_ID', 'LOC_GENRE_MUSICAL_OPERA_ID', 'LOC_GENRE_BALLET_ID'],
                            ['LOC_GENRE_ARTS_CULTURE_ID', 'LOC_GENRE_PREFORMING_ARTS_ID', 'LOC_GENRE_FINE_ARTS_ID', 'LOC_GENRE_RELIGION_ID', 'LOC_GENRE_POP_TRADITIONAL_ID', 'LOC_GENRE_LITERATURE_ID', 'LOC_GENRE_FILM_CINEMA_ID', 'LOC_GENRE_EXPERIMENTAL_ID', 'LOC_GENRE_BROADCASTING_PRESS_ID', 'LOC_GENRE_NEW_MEDIA_ID', 'LOC_GENRE_ARTS_MAGAZINE_ID', 'LOC_GENRE_FASHION_ID'],
                            ['LOC_GENRE_SOCIAL_ECONOMICS_ID', 'LOC_GENRE_MAGAZINE_ID', 'LOC_GENRE_ADVISORY_ID', 'LOC_GENRE_REMARKABLE_ID'],
                            ['LOC_GENRE_EDU_SCIENCE_FACTUAL_ID', 'LOC_GENRE_ENVIRONMENT_ID', 'LOC_GENRE_TECH_SCIENCE_ID', 'LOC_GENRE_MEDI_PHYSI_PSYCH_ID', 'LOC_GENRE_FOREIGN_ID', 'LOC_GENRE_SOCIAL_SPIRITUAL_ID', 'LOC_GENRE_FURTHER_EDU_ID', 'LOC_GENRE_LANGUAGE_ID'],
                            ['LOC_GENRE_LEISURE_HOBBIES_ID', 'LOC_GENRE_TOURISM_TRAVEL_ID', 'LOC_GENRE_HANDICRAFT', 'LOC_GENRE_MOTORING_ID', 'LOC_GENRE_FITNESS_HEALTH_ID', 'LOC_GENRE_COOKING_ID', 'LOC_GENRE_SHOPPING_ID', 'LOC_GENRE_GARDENING_ID'],
                            ['LOC_GENRE_ORIGIN_LANG_ID', 'LOC_GENRE_BLACK_WHITE_ID', 'LOC_GENRE_UNPUBLISHED_ID', 'LOC_GENRE_LIVE_BROADCAST_ID']
                        ];
                        that.majorGenreAllIDList = ['LOC_ALL_ID', 'LOC_GENRE_MOVIE_ID', 'LOC_GENRE_NEWS_ID', 'LOC_GENRE_SHOW_ID', 'LOC_GENRE_SPORTS_ID', 'LOC_GENRE_CHILDREN_ID', 'LOC_GENRE_MUSIC_ID', 'LOC_GENRE_ARTS_ID', 'LOC_GENRE_SOCIAL_ID', 'LOC_GENRE_EDUCATION_ID', 'LOC_GENRE_LEISURE_ID', 'LOC_GENRE_SPECIAL_CHARACTER_ID'];
                        that.minorGenreAllIDList = [
                            ['LOC_ALL_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_MOVIE_DRAMA_ID', 'LOC_GENRE_DETECTIVE_THRILLER_ID', 'LOC_GENRE_ADVENTURE_WESTERN_WAR_ID', 'LOC_GENRE_SF_FANSTASY_HORROR_ID', 'LOC_GENRE_COMEDY_ID', 'LOC_GENRE_SOAP_MELO_FOLKLORIC_ID', 'LOC_GENRE_ROMANCE_ID', 'LOC_GENRE_SERIOUS_DRAMA_ID', 'LOC_GENRE_ADULT_MOVIE_DRAMA_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_NEWS_CURRENT_AFFAIRS_ID', 'LOC_GENRE_NEWS_WEATHER_REPORT_ID', 'LOC_GENRE_NEWS_MAGAZINE_ID', 'LOC_GENRE_DOCUMENTARY_ID', 'LOC_GENRE_INTERVIEW_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_SHOW_GAME_ID', 'LOC_GENRE_SHOW_QUIZ_CONTEST_ID', 'LOC_GENRE_VARIETY_SHOW_ID', 'LOC_GENRE_TALK_SHOW_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_SPECIAL_EVENTS_ID', 'LOC_GENRE_SPORTS_MAGAZINE_ID', 'LOC_GENRE_FOOTBALL_SOCCER_ID', 'LOC_GENRE_TENNIS_SQUASH_ID', 'LOC_GENRE_TEAM_SPROTS_ID', 'LOC_GENRE_ATHLETICS_ID', 'LOC_GENRE_MOTOR_SPORT_ID', 'LOC_GENRE_WATER_SPORT_ID', 'LOC_GENRE_WINTER_SPORTS_ID', 'LOC_GENRE_EQUESTRIAN_ID', 'LOC_GENRE_MARTIAL_SPORTS_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_CHILDREN_YOUTH_ID', 'LOC_GENRE_PRESCHOOL_CHILDREN_ID', 'LOC_GENRE_6TO4_ENTERTAIN_ID', 'LOC_GENRE_10TO16_ENTERTAIN_ID', 'LOC_GENRE_INFO_EDU_SCHOOL_ID', 'LOC_GENRE_CARTOONS_PUPPETS_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_MUSIC_DANCE_ID', 'LOC_GENRE_ROCK_POP_ID', 'LOC_GENRE_CLASSIC_ID', 'LOC_GENRE_FOLK_TRADITIONAL_ID', 'LOC_GENRE_JAZZ_ID', 'LOC_GENRE_MUSICAL_OPERA_ID', 'LOC_GENRE_BALLET_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_ARTS_CULTURE_ID', 'LOC_GENRE_PREFORMING_ARTS_ID', 'LOC_GENRE_FINE_ARTS_ID', 'LOC_GENRE_RELIGION_ID', 'LOC_GENRE_POP_TRADITIONAL_ID', 'LOC_GENRE_LITERATURE_ID', 'LOC_GENRE_FILM_CINEMA_ID', 'LOC_GENRE_EXPERIMENTAL_ID', 'LOC_GENRE_BROADCASTING_PRESS_ID', 'LOC_GENRE_NEW_MEDIA_ID', 'LOC_GENRE_ARTS_MAGAZINE_ID', 'LOC_GENRE_FASHION_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_SOCIAL_ECONOMICS_ID', 'LOC_GENRE_MAGAZINE_ID', 'LOC_GENRE_ADVISORY_ID', 'LOC_GENRE_REMARKABLE_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_EDU_SCIENCE_FACTUAL_ID', 'LOC_GENRE_ENVIRONMENT_ID', 'LOC_GENRE_TECH_SCIENCE_ID', 'LOC_GENRE_MEDI_PHYSI_PSYCH_ID', 'LOC_GENRE_FOREIGN_ID', 'LOC_GENRE_SOCIAL_SPIRITUAL_ID', 'LOC_GENRE_FURTHER_EDU_ID', 'LOC_GENRE_LANGUAGE_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_LEISURE_HOBBIES_ID', 'LOC_GENRE_TOURISM_TRAVEL_ID', 'LOC_GENRE_HANDICRAFT', 'LOC_GENRE_MOTORING_ID', 'LOC_GENRE_FITNESS_HEALTH_ID', 'LOC_GENRE_COOKING_ID', 'LOC_GENRE_SHOPPING_ID', 'LOC_GENRE_GARDENING_ID'],
                            ['LOC_ALL_ID', 'LOC_GENRE_ORIGIN_LANG_ID', 'LOC_GENRE_BLACK_WHITE_ID', 'LOC_GENRE_UNPUBLISHED_ID', 'LOC_GENRE_LIVE_BROADCAST_ID']
                        ];
                    }
                }());
                that.bInit = true;
            }
            if (type === 'major') {
                if (bNeedAllId === true) {
                    return that.majorGenreAllIDList;
                }
                return that.majorGenreList;
            } else if (type === 'minor') {
                if (bNeedAllId === true) {
                    return that.minorGenreAllIDList;
                }
                return that.minorGenreList;
            }
        },
        getGenreImage : function(aIndentifier) {
            return this._images[String(aIndentifier)];
        }
    };
    export var getBGColor = function(aImgUrl : string, aCoordinate_X? : number, aCoordinate_Y? : number) {
        var canvas = document.createElement('canvas').getContext('2d');
        var img = new Image();
        img.src = aImgUrl;
        canvas.drawImage(img,0,0);
        var img_data = canvas.getImageData(aCoordinate_X || 1, aCoordinate_Y || 1, 1, 1).data;
        return {
            r:img_data[0],
            g:img_data[1],
            b:img_data[2],
            a:img_data[3]
        };
    };
    export var highlightInnerHTML = function(aTarget:string, aKeyword:string, aTag?:string, aColor?:any) {
        if (!aTarget || !aKeyword) {
            return null;
        }
        var retInnerHTML = aTarget;
        var targetString = retInnerHTML.toLowerCase();
        var textArr = [];
        var startIdx = 0,   // for retInnerHTML
            keywordIdx = 0, // for targetString
            targetLen = targetString.length,
            keyLen = aKeyword.length,
            remainedText = targetString;
        while ((keywordIdx = remainedText.indexOf(aKeyword)) > -1) {
            textArr.push(retInnerHTML.substr(startIdx, keywordIdx));
            textArr.push("<strong>"+retInnerHTML.substr(startIdx + keywordIdx, keyLen)+"</strong>");
            startIdx += keywordIdx + keyLen;
            remainedText = targetString.substr(startIdx);
        }
        textArr.push(retInnerHTML.substr(startIdx));
        retInnerHTML = textArr.join('');
        hx.log("debug", "[highlightInnerHTML] aTarget : " + aTarget);
        hx.log("debug", "[highlightInnerHTML] retInnerHTML : " + retInnerHTML);
        return retInnerHTML;
    }
    export var checkHttpUrl = function(aUrl) {
        if (aUrl && aUrl.search && (aUrl.search("http://")>=0 || aUrl.search("https://")>=0)) {
            return true;
        }
        return false;
    }
    export var checkMdsUrl = function(aUrl, aWidth?) {
        if (aUrl && aUrl.search && (aUrl.search("http://")>=0 || aUrl.search("https://")>=0) && aUrl.search("w=" + (aWidth || ''))>=0) {
            return true;
        }
        return false;
    }
    export var makeMdsImgUrl = function(aUrl, aWidth) {
        if (!aUrl) {
            hx.log("error", "[makeMdsImgUrl] not ready to make this");
            return "";
        }
        if (aUrl.indexOf('?w=') >= 0) {
            hx.log("debug", "[makeMdsImgUrl] already done with aWidth : " + aUrl);
            return aUrl;
        }
        if (!aWidth) {
            hx.log("error", "[makeMdsImgUrl] need to width value for this");
            return aUrl;
        }
        return aUrl + "?w=" + aWidth;
    }
    export var updatePageBackground = () => {
        var transparency = hx.svc.WEB_STORAGE.getBackgroundTransparency();
        var $bgPage = $('.prism-page').not('#main');
        var StrClassON = 'transparency100';
        if (transparency === 1) { // on
            $bgPage.addClass(StrClassON);
        } else {    // off
            if ($bgPage.hasClass(StrClassON)) {
                $bgPage.removeClass(StrClassON);
            }
        }
    }
    var fnProgramIdParser = (programId: string) => {
        var ret = {
            onid: '',
            tsid: '',
            svcid: '',
            eventid: ''
        };
        if (!programId) {
            return ret;
        }
        var splited = programId.split('://');
        if (splited[1]) {
            splited = splited[1].split(';');
            if (splited[1]) {
                ret.eventid = splited[1] || '';
            }
            if (splited[0]) {
                splited = splited[0].split('.');
                ret.onid = splited[0] || '';
                ret.tsid = splited[1] || '';
                ret.svcid = splited[2] || '';
            }
        }
        return ret;
    }
    export var compareProgrammeID = (programmeID1: string, programmeID2: string): boolean => {
        var bSame = false;
        if (programmeID1 && programmeID2) {
            var parsedID1 = fnProgramIdParser(programmeID1);
            var parsedID2 = fnProgramIdParser(programmeID2);
            var compareID1 = [parsedID1.onid, parsedID1.svcid, parsedID1.eventid].join('.');
            var compareID2 = [parsedID2.onid, parsedID2.svcid, parsedID2.eventid].join('.');
            if (compareID1 === compareID2) {
                hx.log('debug', '[UTIL][compareProgrammeID] progID1 vs progID2 : [' + programmeID1 + ' | ' + programmeID2 + ']');
                hx.log('debug', '[UTIL][compareProgrammeID] SAME!!!!!!!!!!!!');
                bSame = true;
            }
        }
        return bSame;
    }
    export var getCompareID = (item: any) => {
        if (!item || !item.getProgrammeID) {
            hx.log('error', '[getCompareID] (!item || !item.getProgrammID)');
            return '';
        }
        var programmeID = item.getProgrammeID() || null;
        var compareID = '';
        var parsedId = fnProgramIdParser(programmeID);
        if (parsedId.onid && parsedId.svcid && parsedId.eventid) {
            compareID = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
        } else if (parsedId.eventid) {
            var channel = item.getChannelInstance ? item.getChannelInstance() : null;
            if (channel && channel.onid && channel.sid) {
                compareID = [channel.onid.toString(16), channel.sid.toString(16), parsedId.eventid].join('.');
            } else {
                hx.log('error', '[getCompareID] no on/svc ID, no channel as well..');
            }
        } else {
            hx.log('error', '[getCompareID] no event ID....');
        }
        return compareID;
    }
}
export = il;
/**
 * $.getScript  wrapper for sequentially script loading.
 *
 * @class jQuery.hxGetScript
 * @param url - url of file, load.
 * @param cb - callback function for loading complete
 * @return jQuery object(or namespace)
 */
( function ($) {
    $.hxLog = function (log) {
        //console.info(log);
    };
    $.hxLogErr = function (log) {
        if (window.debug) {
            window.debug("\033[30m\033[41m ERROR:" + log + "\033[39m\033[49m");
        } else {
            console.error(log);
        }
    };
    $.hxGetScript = function (url, cb, cls) {
        if (!$.isArray(url)) {
            url = [url];
        }
        if (cls === undefined) {
            cls = "base";
        }
        var inst = $.hxGetScript._inst[cls];
        if (inst === undefined) {
            inst = $.hxGetScript._inst[cls] = new loadObject(cls);
        }
        var i;
        for (i = 0; i < url.length; i += 1) {
            inst.push({
                url: url[i],
                cls: cls
            });
            $.hxLog("URL:" + url[i]);
        }
        inst.setCallback(cb);
        inst.load();
        return this;
    };
    function loadObject(aCLS) {
        this._cnt = 0;
        this._complete = 0;
        this._cls = aCLS;
        this.list = [];
        this.downState = false;
    }

    loadObject.prototype = {
        push: function (data) {
            this.list.push(data);
        },
        setCallback: function (cb) {
            if (!cb) {
            }
            var l = this.list.length;
            this.list[l - 1].cb = cb;
        },
        load: function () {
            if (this.downState) {
                return;
            }
            this.downState = true;
            var data = this.list.shift();
            if (data === undefined) {
                return;
            }
            /**DEVCODE*/
            $.hxLog("loader:" + this._cls + ":" + "[start loading (" + (this._cnt += 1) + ")] :" + data.url);
            /*DEVCODE**/
            var that = this;
            $.hxLog("loader:" + this._cls + ":" + data.url);
            $.getScript(data.url, function () {
                that.complete(data);
            }).fail(function (jqxhr, settings, exception) {
                that.fail(jqxhr, settings, exception, data);
            });
        },
        complete: function (data) {
            if (data.cb) {
                $.hxLog("loader:" + this._cls + "----start callback----");
                data.cb(data);
            }
            var list = this.list;
            var arr = [];
            var n;
            for (n = 0; n < list.length; n += 1) {
                if (list[n].url === data.url) {
                    var cb = list[n].cb;
                    if (cb) {
                        cb(list[n]);
                    }
                    arr.push(n);
                }
            }
            var i;
            for (i = arr.length - 1; i >= 0; i -= 1) {
                list.splice(arr[i], 1);
            }
            /**DEVCODE*/
            $.hxLog("loader: [loading complete (" + (this._complete += 1) + ")]:URL:" + data.url);
            /*DEVCODE**/
            this._next(data);
        },
        fail: function (jqxhr, settings, exception, data) {
            /**DEVCODE*/
            $.hxLogErr("loader: loading fail :" + exception + ":URL:" + data.url + ":#######");
            this._next(data);
            /*DEVCODE**/
        },
        _next: function (data) {
            this.downState = false;
            if (this.list.length > 0) {
                var that = this;
                setTimeout(function () {
                    that.load();
                }, 1);
            }
        }
    }
    $.hxGetScript._inst = [];
    //list of class
    /* getMulti Script */
    $.hxGetMultiScript = function (url, cb, cls) {
        if (!$.isArray(url)) {
            url = [url];
        }
        var loadingScript = url.length;

        function _loadComplete() {
            if ((loadingScript -= 1) <= 0) {
                cb();
            }
        }

        var i;
        for (i = 0; i < url.length; i += 1) {
            $.hxGetScript(url[i], _loadComplete, cls + "_" + i);
        }
    };
}(jQuery));
( function ($) {
    var files = {}, downProcess = {}, execContent = {}, postProcess = {};

    function getscript(url, cb, cls) {
        getMscript(url, cb, cls);
    }

    function getMscript(url, cb, cls, repeat?) {
        if (cls === undefined) {
            cls = "base";
        }
        if (!$.isArray(url)) {
            url = [url];
        }
        var d = downProcess[cls];
        if (d === undefined) {
            d = downProcess[cls] = [];
        }
        var f = files, p = postProcess, l = url.length, index = d.push("loading") - 1, inFx = _makeMto1CallBack(l, _classOrdering, cls, index, function () {
            setTimeout(_launchScript, 0, url, cb, repeat);
        }), n, o, u;
        for (n = 0; n < l; n += 1) {
            o = f[ u = url[n]];
            if (o === undefined) {
                f[u] = "loading";
                xhrRequest(u, _loadingComplete, _errDisplay, inFx);
            } else if (o === 'loading') {
                if (p[u] === undefined) {
                    p[u] = [];
                }
                p[u].push(inFx);
            } else if (o === "loading done") {
                inFx();
            } else {
                hx.log("error", "un");
            }
        }
    }

    function _makeMto1CallBack(num, output, ...aArgs) {
        var n = 0, param = Array.prototype.slice.call(arguments, 2);
        return function () {
            n += 1;
            if (n === num) {
                output.apply(window, param);
            }
        };
    }

    function _classOrdering(cls, index, cb) {
        var d = downProcess[cls], l = d.length, n;
        d[index] = cb;
        for (n = 0; n < l; n += 1) {
            if (typeof (d[n]) === 'function') {
                d[n]();
                d[n] = "complete";
            } else if (d[n] === "loading") {
                break;
            }
        }
    }

    function _launchScript(url, cb, repeat) {
        var f = files, e = execContent, a = url, l = a.length, n, o, u;
        for (n = 0; n < l; n += 1) {
            u = a[n];
            if (f[u] === "loading done") {
                f[u] = "processing";
                //eval.call(window, '/*[FILE:'+u+']*/'+ e[u].replace(/\/\*\*DEVCODE\*[^\*]*\*DEVCODE\*\*\//gi,"") );
                eval.call(window, '/*[FILE:' + u + ']*/' + e[u]);
                if (repeat) {
                    cb.call(window, {
                        url: u
                    });
                }
                f[a[n]] = "process";
            }
        }
        if (!repeat) {
            cb.call(window, {
                url: url
            });
        }
    }

    function _loadingComplete(completedUrl, content, inFx) {
        execContent[completedUrl] = content;
        files[completedUrl] = "loading done";
        inFx.call(window, completedUrl);
        var a = postProcess[completedUrl];
        if (a !== undefined) {
            var n, l = a.length;
            for (n = 0; n < l; n += 1) {
                a[n].call(window, completedUrl);
            }
        }
    }

    function _errDisplay(err) {
        $.hxLogErr(err.error);
    }

    function xhrRequest(url, succesCb, errCb, inFx) {
        var x: XMLHttpRequest = null, h;
        try {
            x = new XMLHttpRequest();
            x.onreadystatechange = function () {
                if (x.readyState === 4 /*&& x.status === 200*/) {
                    clearTimeout(h);
                    succesCb(url, x.responseText, inFx);
                }
            };
            x.open("GET", url, true);
            x.send();
            h = setTimeout(function () {
                x.abort();
                errCb({
                    error: 'timeout'
                }, x);
            }, 10 * 1000);
        } catch (err) {
            errCb({
                error: err
            }, x);
        }
    }
}(jQuery));
/**
 * $.hxUpgrade  copy instance properties (not deep)
 *      if a same name property exist in the dest instance,
 *      then '_' prefix adding
 *
 * @class jQuery.hxUpgrade
 * @param dest - destination
 * @param src - source instance
 * @return none
 */
( function ($) {
    $.hxUpgrade = function (dest, src) {
        var n;
        for (n in src) {
            if (src.hasOwnProperty(n)) {
                if (dest[n] !== undefined) {
                    /**DEVCODE*/
                    if (dest["_" + n] !== undefined) {
                        $.hxLogErr("defined this function -" + n + "- in " + dest.name);
                    }
                    /*DEVCODE**/
                    dest["_" + n] = src[n];
                } else {
                    dest[n] = src[n];
                }
            }
        }
    };
}(jQuery));
( function ($) {
    $.isObject = function (target) {
        return (null !== target && typeof target === 'object');
    };
}(jQuery));
( function ($) {
    $.extend({
        imgPreload: function (options) {
            var loadcounter = 0, i, settings = $.extend({
                images: null,
                onLoad: null,
                onLoadAll: null
            }, options);
            for (i = 0; i < settings.images.length; i += 1) {
                var img = $('<img/>').addClass('preloading').css('display', 'none').attr('src', settings.images[i]).load(onloadEvent);
            }
            function onloadEvent() {
                loadcounter += 1;
                if (settings.onLoad !== null) {
                    settings.onLoad($(this));
                }
                if (loadcounter === settings.images.length) {
                    if (settings.onLoadAll !== null) {
                        settings.onLoadAll();
                    }
                }
            }
        }
    });
}(jQuery));
$.fn.preload = function () {
    this.each(function () {
        (new Image()).src = this;
    });
};
( function ($) {
    //hx.il.qEvent.kernel
    $.fn.extend({
        hxTrigger: function (dest, data) {
            this.trigger(dest, data);
        },
        hxBind: function (name, cb) {
            this.bind(name, function (e, p) {
                cb(p);
            });
        },
        hxUnbind: function (name, handler) {
            this.unbind(name);
        }
    });
}(jQuery));
interface if$Ani {
    style?: {
        start?: any;
        end?: any;
    };
    trans: {
        dur: any;
        delay: any;
        timing: any;
    };
    cb?: Function;
    cls?: {
        name: string;
        fn?: string;
    };
    log: boolean;
    noEvent: boolean;
}
( function ($) {
    var getTransition = (): string => {
        var transitionCmd: string = '-o-transition';
        if (hx.emulMode || $.browser.webkit) {
            transitionCmd = '-webkit-transition';
        }
        return transitionCmd;
    };
    var getTransitionDuration = (): string => {
        var transitionDuration: string = '-o-transition-duration';
        if (hx.emulMode || $.browser.webkit) {
            transitionDuration = '-webkit-transition-duration';
        }
        return transitionDuration;
    };
    var getTransitionEnd = (): string => {
        var transitionEnd: string = 'otransitionend';
        if (hx.emulMode || $.browser.webkit) {
            transitionEnd = 'webkitTransitionEnd';
        }
        return transitionEnd;
    };
    $.fn.ani = function (param: if$Ani) {
        var target: JQuery = <JQuery>this, cb = param.cb, style = param.style, trans = param.trans, cls = param.cls, transVal, delay = trans ? trans.delay : null, aniTimeoutID;
        // Apply Start Style
        if (style && style.start) {
            target.css(getTransitionDuration(), '');
            target.css(style.start);
            if (param.log) {
                console.log('startStyle');
                console.log(style.start);
            }
        }
        aniTimeoutID = setTimeout(function () {
            var elem = target[0];
            if (elem) {
                if (trans) {
                    var transitionVal = ['all', (trans.dur || 0) + 'ms',
                        // (trans.delay || ''),
                        (trans.timing || 'cubic-bezier(0.675, 0.290, 0.000, 0.930)')].join(' ');
                    //cubic-bezier(0.675, 0.290, 0.000, 0.930)
                    target.css(getTransition(), transitionVal);
                }
                if (style && style.end) {
                    target.css(style.end);
                    if (param.log) {
                        console.log('endStyle');
                        console.log(style.end);
                    }
                }
                if (cls) {
                    if (cls.fn === 'new') {
                        target[0].className = cls.name;
                    } else {
                        target[0].classList[cls.fn](cls.name || '');
                    }
                }
                if (param.noEvent) {
                    var element = target[0];
                    var durations = ['0'];
                    if (getComputedStyle && getComputedStyle(element).transitionDuration) {
                        durations = getComputedStyle(element).transitionDuration.split(',');
                    } else {
                        durations = element.style.transitionDuration.split(',');
                    }
                    var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
                    var timeout;
                    if (delay) {
                        timeout = setTimeout(cb, delay * 3);
                    } else {
                        throw "Invalid transiton duration";
                    }
                    target.one(getTransitionEnd(), function () {
                        clearTimeout(timeout);
                        cb();
                    });
                } else {
                    elem.addEventListener(getTransitionEnd(), aniCB(elem, cb), false);
                }
            } else {
                hx.log("info", "5");
                hx.log("warning", '--------------------- [ilUtil.js, 1109]($.fn.ani).aniTimeoutID : elem is null ----------------- ');
            }
        }, delay || 1);
        function aniCB(elem, cb) {
            return function handler(e) {
                if (param.log) {
                    console.log('aniEnd');
                }
                target.css(getTransitionDuration(), '');
                if (typeof cb === 'function') {
                    cb.call(elem);
                }
                elem.removeEventListener(getTransitionEnd(), handler, false);
            };
        }

        return aniTimeoutID;
    };
}(jQuery));
// moved from alEntity
( function ($) {
    if (hx.config.hxcomm) {
        $.fn.extend({
            hxRun: function (name, data) {
                //hx.log("warning",'hxRun:'+name + ":"+this[0].name);
                var fx = this[0]['_keep_' + name];
                if (fx) {
                    fx.call(this, data);
                }
            },
            hxKeep: function (name, cb) {
                // hx.log("warning","hxKeep:"+name + ":"+this[0].name);
                this[0]['_keep_' + name] = cb;
            }
        });
//    } else {
//        $.fn.extend({
//            _keepCb : {},
//            hxRun : function(name, data) {
//                this.hxTrigger(name, data);
//            },
//            hxKeep : function(name, cb) {
//                this.hxBind(name, cb);
//            }
//        });
    }
}(jQuery));
( function($) {
    var KCssEvntTransitionEnd = 'transitionend';
    $.fn.cssBefore = function() {
        var args = $.makeArray(arguments);
        var self = $(this);
        self.css.apply(self, args);
        return self;
    };
    $.fn.cssLater = function() {
        var args = $.makeArray(arguments);
        var self = $(this);
        setTimeout(function() {
            self.css.apply(self, args);
        }, 1);
        return self;
    };
//    $.fn.afterTransitionOneShot = function(handler) {
//        var transitionEnd = 'oTransitionEnd otransitionend';
//        if ($.browser.webkit) {
//            transitionEnd = 'webkitTransitionEnd';
//        }
//        var target = this;
//        target.bind(transitionEnd, function(e) {
//            if (e.srcElement === this) {
//                target.unbind(transitionEnd);
//                setTimeout(function() {
//                    handler(e);
//                }, 1);
//            }
//        });
//        return target;
//    };
    $.fn.afterTransitionOneShot = function (aCallback: Function, aWithEvent: boolean = false) {
        var target = this;
        if (aWithEvent) {
            var element = target[0];
            var durations = ['0'];
            if (getComputedStyle && getComputedStyle(element).transitionDuration) {
                durations = getComputedStyle(element).transitionDuration.split(',');
            } else {
                durations = element.style.transitionDuration.split(',');
            }

            var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
            var timeout;
            if (delay) {
                hx.log('debug', '[afterTransitionOneShot] delay: ' + delay);
                timeout = setTimeout(aCallback, delay * 3);
            } else {
                timeout = setTimeout(aCallback, 300 * 3);
                //throw "Invalid transiton duration";
            }

            target.one(KCssEvntTransitionEnd, function () {
                clearTimeout(timeout);
                aCallback();
            });
        } else {
            var element = target[0];
            var durations = ['0'];
            if (getComputedStyle && getComputedStyle(element).transitionDuration) {
                durations = getComputedStyle(element).transitionDuration.split(',');
            } else {
                durations = element.style.transitionDuration.split(',');
            }

            var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
            if (delay) {
                setTimeout(aCallback, delay);
            } else {
                throw "Invalid transiton duration";
            }
        }
        return target;
    };
    $.fn.anim = function(args, cb) {
        var $self = this;
        $self.cssLater(args);
        $self.afterTransitionOneShot(function() {
            if (cb) {
                cb();
            }
        });
    };
}(jQuery)); ( function($) {
    $.getUrlParams = function(url) {
        var i, length;
        var params = {};
        url = url.split("?")[1];
        if (url) {
            url = url.split("&");
            length = url.length;
            for ( i = 0; i < length; i += 1) {
                var splitCache = url[i].split("=");
                params[splitCache[0]] = splitCache[1];
            }
        }
        return params;
    };
}(jQuery));

(function(win: any) {
    var depth = 0;
    var tab = "    ";
    var log = function(msg) {
        var loop = depth;
        var ret = "";
        while (loop && loop--) {
            msg = tab + msg;
        }
        console.log(msg)
    };
    function plugin_object_dump(aTargetObject, aObject, aProperties, aSpecityObjectProperties) {
        aTargetObject = aTargetObject || {};
        for (var i = 0; i < aProperties.length; i++) {
            var key = aProperties[i];
            var value = aObject[key];
            if (aSpecityObjectProperties.hasOwnProperty(key)) {
                // object
                var props = aSpecityObjectProperties[key];
                depth++;
                log("[Object]: " + key);
                aTargetObject[key] = plugin_object_dump(null, value, props, aSpecityObjectProperties);
                depth --;
            } else {
                log("[" + key + "]: " + value);
                aTargetObject[key] = value;
                // primitive
            }
        }
        return aTargetObject;
    }
    win.plugin_object_dump = plugin_object_dump;
    return plugin_object_dump;
})(window);
