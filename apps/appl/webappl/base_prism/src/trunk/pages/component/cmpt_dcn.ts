///<reference path="../../def/sugar.d.ts" />

import __COMPONENT_KERNEL__ = require("pages/component_kernel");
declare var hx: ifSugar;
// file name : pages/component/cDcn.js
/*jslint sloppy:true, nomen:true, vars:true */

/**
 * dcn component
 * @param count : count of shown item
 *        data : array user data
 *        fx : function setting each items function that argument is $ object of item
 *      attr: attribute for list
 *      itemAttr : attribute for itemd
 *      $item : $object for content
 */
export class dcn extends __COMPONENT_KERNEL__ {
    $: JQuery;
    _match: any;
    _cachedList: any;
    _num: any;
    _index: number;
    _reinputTimerId: number;
    data: any;
    createdData: any;
    startnum: number;
    frontPanelText: string;
    getIndex: any;
    group: any;
    _$items: any;
    fx: any;
    count: number;
    _$src: any;
    _doFocus: any;

    constructor() {
        super();
    }
    create(aParam: any): void {
        var that = this;
        //hx.logadd('CDcn');
        hx.log('CDcn', 'create');
        $.extend(that, aParam);
        that._create(aParam);
        that.$.focus(() => {
            this._on_focus();
        });
        that._cachedList = {
            list: [],
            num: that.startnum,
            _backup: []
        };
        that._reinputTimerId = -1;
        if (that._num === undefined) {
            that._num = hx.al.component('tab', {
                attr: {
                    'class': 'num'
                },
                itemAttr: {},
                data: '----',
                fx:  function (i, o) {
                    o.html(this.data[i]);
                },
                count: 4
            });
            // append components
            this.append(this._num);
        }
        if (that._match === undefined) {
            that._match = hx.al.component('list', {
                attr: {
                    'class': 'match'
                },
                itemAttr: {
                    'class': 'focusable'
                }
            });
            //that._match.setData = that.__matchList_setData;
            // append components
            that//
                .append(that._match);
        }
        if (aParam.frontPanelText) {
            that.frontPanelText = aParam.frontPanelText;
        }
        // append elements
        that.$//
            .append(that._num.$)//
            .append(that._match.$.hide());
        that.__update_numDisplay(that._cachedList.num);
        hx.svc.DCN.addEventListener({
            'event': 'dcnUpdateListFail',
            'objTarget': that
        });
        that.createdData = hx.svc.DCN.create({
            'startnum': that._cachedList.num,
            'group': that.group
        });
        that._process_initData(that.createdData);
    }

    destroy(): void {
        var frontPanelText: string;
        hx.svc.DCN.removeEventListener(this);
        this.createdData = null;
        this.__clear_dcn();
        this.$.remove();
        this._num = undefined;
        this._match = undefined;
        frontPanelText = this.frontPanelText || '';
        hx.svc.SETTING_UTIL.setFrontPanelString({
            'text': frontPanelText
        });
    }

    getFocusedChannelId(): any {
        var list: any = this._match//
            , ccid: any;
        if (!this._cachedList.list.length) {
            ccid = '';
        } else {
            if (this._cachedList._backup) {
                var matchChannel = this._cachedList._backup[list.start + list.getFocusIndex()];
                if (matchChannel) {
                    ccid = matchChannel.ccid;
                }
            }
        }
        return ccid;
    }

    getFocusedChannelInfo(): any {
        var list: any = this._match;
        var channelInfo: any = {};
        if (this._cachedList.list.length) {
            if (this._cachedList._backup) {
                var matchChannel = this._cachedList._backup[list.start + list.getFocusIndex()];
                if (matchChannel) {
                    channelInfo.ccid = matchChannel.ccid;
                    channelInfo.lcn = matchChannel.digiNum;
                    channelInfo.group = matchChannel.group;
                }
            }
        }
        return channelInfo;
    }

    setChannel(): boolean {
        var bSet: boolean, ret: any;
        if (!this._cachedList.list.length) {
            bSet = false;
        } else {
            var idx: any = this._match.start + this._match.getFocusIndex();
            ret = hx.svc.DCN.setchannel({
                'index': idx
            });
            if (true === ret) {
                this._process_SetChannelSuccess();
            } else {
                this._process_SetChannelFail();
            }
            bSet = true;
        }
        return bSet;
    }

    // EVENT HANDLER
    _on_focus(): void {
        if (this._cachedList.list.length) {
            this._match.on();
        }
    }

    _on_alKey(aParam: {alKey: number; alData: any; alEvent: string}): boolean {
        var k: any = hx.key, kc: number = aParam.alKey;
        var bConsumed: boolean = false;
        var num: string;
        if ((kc >= k.KEY_0) && (kc <= k.KEY_9)) {
            if (this._cachedList.num.length >= 4) {
                return true;
            }
            num = (kc - k.KEY_0).toString();
            this._cachedList.num += num;
            if (this.createdData === null) {
                //reinput case
                this.createdData = hx.svc.DCN.create({
                    'startnum': this._cachedList.num
                });
                this._process_initData(this.createdData);
            } else {
                var retObj: any = hx.svc.DCN.putNumber({
                    'number': num
                });
                this._process_UpdateList(retObj);
            }
            this.__send_event(0, aParam);
            bConsumed = true;
        } else {
            if (!aParam || !aParam.alData) {
                hx.log('error', 'param.alData undefiend');
            } else {
                switch (kc) {
                    case k.KEY_LEFT:
                    case k.KEY_RIGHT:
                        hx.log('CDcn', 'left/rigth');
                        bConsumed = true;
                        break;
                    case k.KEY_DOWN:
                    case k.KEY_UP:
                        if (aParam.alData.me === this._match) {
                            aParam.alEvent = 'alUpdate';
                            if (aParam.alData === undefined) {
                                aParam.alData = {};
                            }
                            aParam.alData.me = this;
                            this.sendEventToParent(aParam);
                        }
                        bConsumed = true;
                        break;
                    case k.KEY_OK:
                        aParam.alEvent = 'alClicked';
                        aParam.alData = {
                            index: -1
                        };
                        bConsumed = true;
                        break;
                    case k.KEY_BACK:
                    case k.KEY_ESCAPE:
                        break;
                }
            }
        }
        return bConsumed;
    }

    _on_alScrolldown(aParam: {alData: any; alEvent: string}): boolean {
        var ti: number = this._cachedList.list.length;
        var list: any = this._match;
        var phyStartPos: number;
        if (ti <= list.count) {
            list.setItemFocus(0);
        } else {
            if ((list.start + list.count) === ti) {
                list.count = (ti > 4) ? 4 : ti;
                list.start = 0;
                list.setData(list.data, list.fx);
                list.setItemFocus(0);
            } else {
                list.start = list.start + 1;
                phyStartPos = ti - list.start;
                list.count = (phyStartPos > 4) ? 4 : phyStartPos;
                list.setData(list.data, list.fx);
                list.setItemFocus(list.count - 1);
            }
        }
        if (aParam.alData.me === this._match) {
            aParam.alEvent = 'alUpdate';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        }
        return true;
    }

    _on_alScrollup(aParam: {alData: any; alEvent: string}): boolean {
        var ti: number = this._cachedList.list.length;
        var list: any = this._match;
        var phyStartPos: number = 0;
        var remain: number;
        if (ti <= list.count) {
            list.setItemFocus(ti - 1);
        } else {
            if (list.start === 0) {
                remain = ti % list.count;
                if (remain === 0) {
                    list.start = ti - 4;
                } else {
                    list.start = ti - remain;
                }
                phyStartPos = ti - list.start;
                list.count = (phyStartPos > 4) ? 4 : phyStartPos;
                list.setData(list.data, list.fx);
                list.setItemFocus(list.count - 1);
            } else {
                list.start = list.start - 1;
                phyStartPos = ti - list.start;
                list.count = (phyStartPos > 4) ? 4 : phyStartPos;
                list.setData(list.data, list.fx);
                list.setItemFocus(0);
            }
        }
        if (aParam.alData.me === this._match) {
            aParam.alEvent = 'alUpdate';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        }
        return true;
    }

    _on_alChangeItem(aParam: {alData: any; alEvent: string}): void {
        if (aParam.alData.me === this._match) {
            hx.log('CDcn', 'param.alData.index ' + aParam.alData.index);
            aParam.alEvent = 'alUpdate';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        }
    }

    _on_alClicked(aParam: {alData: any; alEvent: string}): boolean {
        if (aParam.alData.me === this._match) {
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            if (aParam.alData.index !== undefined && this._cachedList._backup) {
                var list: any = this._match;
                var matchChannel: any = this._cachedList._backup[list.start + aParam.alData.index];
                if (matchChannel) {
                    aParam.alData.ccid = matchChannel.ccid;
                    aParam.alData.lcn = matchChannel.digiNum;
                    aParam.alData.group = matchChannel.group;
                }
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
            return true;
        }
        return false;
    }

    /*
     * Event & Data Processing Routines...
     *
     */
    _process_initData(aSvcData: {number: any; list: any}) {
        var count: number;
        this.__update_numDisplay(aSvcData.number);
        this.__update_matchListDisplay(aSvcData.number, aSvcData.list);
        count = this._cachedList.list.length;
        if (count) {
            this._match.on();
        } else {
            if (this._reinputTimerId === -1) {
                this._reinputTimerId = setTimeout(()=> {
                    this._cachedList.num = '';
                    this.__update_numDisplay(this._cachedList.num);
                    hx.svc.DCN.removeEventListener(this);
                    this.createdData = null;
                    this.__send_event(0);
                    this._reinputTimerId = -1;
                }, 1000);
            }
        }
    }

    _process_UpdateList(aParam: {number: any; list: any}): void {
        var count: number;
        if (aParam.number !== this._cachedList.num) {
            hx.log('CDcn', 'invalid number ' + aParam.number + ' : ' + this._cachedList.num);
            return;
        }
        this.__update_numDisplay(aParam.number);
        this.__update_matchListDisplay(aParam.number, aParam.list);
        count = this._cachedList.list.length;
        if (count) {
            this._match.on();
        } else {
            if (this._reinputTimerId !== -1) {
                clearTimeout(this._reinputTimerId);
                this._reinputTimerId = -1;
            }
            this._reinputTimerId = setTimeout(()=> {
                this._cachedList.num = '';
                this.__update_numDisplay(this._cachedList.num);
                hx.svc.DCN.removeEventListener(this);
                this.createdData = null;
                this.__send_event(0);
                this._reinputTimerId = -1;
            }, 1000);
        }
    }

    _process_SetChannelSuccess(): void {
        var param: any = {
            alEvent: 'alUpdate',
            alData: {
                result: 'setChannelSuccess'
            }
        };
        this.sendEventToParent(param);
    }

    _process_SetChannelFail(): void {
        var param: any = {
            alEvent: 'alUpdate',
            alData: {
                result: 'setChannelFail'
            }
        };
        this.sendEventToParent(param);
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    __send_event(aCount: number, aParam?: {alData?: any; alEvent?: string}): void {
        aParam = (aParam) || {};
        aParam.alEvent = 'alUpdate';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData = (aParam.alData) || {};
        aParam.alData.me = this;
        aParam.alData.count = aCount;
        this.sendEventToParent(aParam);
    }

    __clear_dcn(): void {
        var c: any = this._cachedList;
        c.num = '';
        c.list.length = 0;
        this.off();
        if (this.$.hasClass('noMatch')) {
            this.$.removeClass('noMatch');
        }
        if (this._reinputTimerId !== -1) {
            clearTimeout(this._reinputTimerId);
            this._reinputTimerId = -1;
        }
    }

    __matchList_setData(aData: any, aFx: any): void {
        if (!aData || !aFx) {
            hx.log("error", "data or callback function not defined");
            return;
        }
        var i: number, that = this;
        this.$.empty();
        this._$items.length = 0;
        this.data = aData;
        this.fx = aFx;
        for (i = 0; i < this.count; i += 1) {
            var o = this._$src.clone();
            this._$items.push(o);
            this.$.append(o);
            this.fx(i, o);
        }
        this.$.focus(()=> {
            this._doFocus();
        });
        that.$.children().focus(function () {
            var index: number = that.getIndex(this);
            that.sendEventToParent({
                alEvent: 'alChangeItem',
                alData: {
                    me: that,
                    index: index
                }
            });
            that._index = index;
        });
    }

    __update_numDisplay(aNum: any): void {
        var i: number, tl: number, that = this, szText;
        var withBar: string = '';
        for (i = 0, tl = 4 - aNum.length; i < tl; i += 1) {
            withBar += '-';
        }
        withBar += aNum;
        that._num.setData(withBar, function (i, o) {
            o.html(this.data[i]);
        });
        if (hx.config.usefrontPanel === 'segment7') {
            szText = '' + withBar.replace(/-/g, "_");
        } else {
            szText = '  CH' + withBar.replace(/-/g, "_");
        }
        hx.svc.SETTING_UTIL.setFrontPanelString({
            'text': szText
        });
    }

    __update_matchListDisplay(aNum: string, aList: any) {
        var i: any, j: number, k: number, ct: number, that = this;
        var clist = that._cachedList.list;
        var item: any, listChNum: string, listChName: string, c: number, output: string;
        var temp: any;
        clist.length = 0;
        for (j = 0; j < 4; j += 1) {
            k = aList.length;
            for (i = 0; i < k; i += 1) {
                item = aList[i];
                listChNum = item.major.toString();
                if (listChNum.indexOf(aNum) === j) {
                    temp = aList.splice(i, 1);
                    aList.push(temp[0]);
                    k -= 1;
                    i -= 1;
                }
            }
        }
        var result = /[^0][0-9]*/.exec(aNum);
        var num = result ? result[0] : null;
        var nonSelectableChannel : boolean = false;
        if (aNum[0] !== "0") {
            for (i = 0; i < aList.length; i++) {
                nonSelectableChannel = false;
                item = aList[i];
                listChNum = item.major.toString();
                listChName = item.name;
                if (ENV.op === ENV.listOP.UKDTT) {
                    if(num === listChNum){
                        if(!!item.hidden && item.selectable === 'false') {
                            nonSelectableChannel = true;
                        }
                    }
                }
                if (!item.hidden || (num === listChNum && nonSelectableChannel === false)) {
                    output = '<span>';
                    for (c = 0, ct = 4 - listChNum.length; c < ct; c += 1) {
                        output += '0';
                    }
                    if (listChNum.indexOf(aNum) !== 0) {
                        temp = listChNum.substr(0, listChNum.indexOf(aNum));
                        output += temp;
                    }
                    output += '<em>' + aNum + '</em>';
                    if (listChNum.indexOf(aNum) + aNum.length < listChNum.length) {
                        temp = listChNum.substr(listChNum.indexOf(aNum) + aNum.length, listChNum.length - aNum.length);
                        output += temp;
                    }
                    output += '</span> ' + listChName;
                    clist.push(output);
                }
            }
        } else {
            for (i = 0; i < aList.length; i++) {
                nonSelectableChannel = false;
                item = aList[i];
                listChNum = item.major.toString();
                listChName = item.name;
                if (ENV.op === ENV.listOP.UKDTT) {
                    if(num === listChNum){
                        if(!!item.hidden && item.selectable === 'false') {
                            nonSelectableChannel = true;
                        }
                    }
                }
                if (!item.hidden || (num === listChNum && nonSelectableChannel === false)) {
                    output = '<span>';
                    var gap = 4 - listChNum.length;
                    if (gap <= 0) {
                        output += listChNum.substr(Math.abs(gap), listChNum.length) + '</span> ' + listChName;
                    } else {
                        for (c = 0, ct = gap; c < ct; c += 1) {
                            output += '0';
                        }
                        output += listChNum + '</span> ' + listChName;
                    }
                    clist.push(output);
                }
            }
        }
        that._cachedList._backup = [];
        that._cachedList._backup = aList;
        if (!clist.length) {
            if (!that.$.hasClass('noMatch')) {
                that.$.addClass('noMatch');
            }
        } else {
            if (that.$.hasClass('noMatch')) {
                that.$.removeClass('noMatch');
            }
            if (that._match.$.css('display') === 'none') {
                that._match.$.show();
            }
            that._match.count = (clist.length > 4) ? 4 : clist.length;
            that._match.start = 0;
            that._match.setData(clist, function (i, o) {
                var self = this, data = self.data;
                var pyidx: number = i + self.start;
                if (pyidx < data.length) {
                    o.html(data[pyidx]);
                } else {
                    hx.log('CDcn', 'invalid index');
                }
            });
        }
    }
}
