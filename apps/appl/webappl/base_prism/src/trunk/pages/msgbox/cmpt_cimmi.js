// file name : pages/component/cCiMmi.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var mComponent;
    (function (mComponent) {
        var cimmi = (function (_super) {
            __extends(cimmi, _super);
            function cimmi() {
                _super.call(this);
            }
            cimmi.prototype.create = function (param) {
                var that = this;
                $.extend(that, param);
                var uidata = that.uidata, attr = uidata.attribute;
                $.extend(that, {
                    _$title: $('<h1>', {
                        'text': param.title
                    }),
                    _$box: $('<aside>'),
                    _$items: [],
                    $: $('<div>', {
                        'class': 'con vaBot taLft'
                    }),
                    count: 0,
                    fx: function (i, o) {
                        var that = this, data = that.data;
                        var pyidx = i + that.start;
                        if (pyidx < data.length) {
                            if (data[pyidx].string) {
                                var displayTxt = decodeURIComponent(data[pyidx].string);
                                o.html(displayTxt);
                                o.uid = data.index;
                            }
                        }
                        else {
                            hx.log('cimmi', 'invalid index');
                        }
                        if (that.type === 'inputdialog') {
                            if (i && (((i + 1) % 6) === 0)) {
                                that._$list.append('</br>');
                            }
                        }
                    }
                });
                that.$.append(that._$title);
                if (attr) {
                    if (attr.indexOf('title') !== -1) {
                        that.$.append('<p>' + decodeURIComponent(uidata.title) + '</p>');
                    }
                    if (attr.indexOf('subtitle') !== -1) {
                        that.$.append('<p>' + decodeURIComponent(uidata.subtitle) + '</p>');
                    }
                }
                that._keyTable = [];
                that._keyTable[hx.key.KEY_BACK] = that._doBack;
                that._keyTable[hx.key.KEY_ESCAPE] = that._doExit;
                that._$box.css('height', '313px');
                switch (that.type) {
                    case 'menulist':
                        that._keyTable[hx.key.KEY_UP] = that._doUp;
                        that._keyTable[hx.key.KEY_DOWN] = that._doDown;
                        that._keyTable[hx.key.KEY_OK] = that._doOkList;
                        that.$.addClass('btn2');
                        that._$list = $('<ul>');
                        that._$src = $('<li>');
                        if (attr.indexOf('selectable') === -1) {
                            that._$box.addClass('box');
                            that._$list.css('margin-top', '0px');
                            that.viewCol = 9;
                            that._justList = true;
                        }
                        else {
                            that._$src.addClass('button focusable');
                            that.viewCol = 5;
                        }
                        if (uidata.listitem) {
                            that.data = uidata.listitem;
                            if (that._justList) {
                                that.count = that.data.length;
                            }
                            else {
                                that.count = (that.data.length > that.viewCol) ? that.viewCol : that.data.length;
                            }
                        }
                        break;
                    case 'inputdialog':
                        that._keyTable[hx.key.KEY_LEFT] = that._doLeft;
                        var bBlind = false, pinCnt;
                        if (uidata.inputdata) {
                            var itemAttr = uidata.inputdata.attribute;
                            if (itemAttr) {
                                if (itemAttr.indexOf('hiddeninput') !== -1) {
                                    bBlind = true;
                                }
                            }
                            pinCnt = uidata.inputdata.answerlength;
                            if (pinCnt > 36) {
                                if (pinCnt === 255) {
                                    that._ffPin = true;
                                    that._keyTable[hx.key.KEY_OK] = that._doOkPin;
                                }
                                pinCnt = 36;
                            }
                            that.count = pinCnt;
                            that.data = [];
                            var i, ti;
                            for (i = 0, ti = that.count; i < ti; i += 1) {
                                that.data.push({
                                    'index': i,
                                    'string': (bBlind) ? '' : '0'
                                });
                            }
                        }
                        that._$box.addClass('pas');
                        //that._$box.css('height', '313px');
                        that._$list = $('<p>', {
                            'class': 'wrap_pw ' + ((bBlind) ? 'blind' : 'num')
                        });
                        that._$src = $('<em>');
                        that._$src.addClass('button focusable');
                        that._bBlind = bBlind;
                        that._insertedKey = '';
                        hx.il.ani.action({
                            type: 'in',
                            targets: that._$box
                        });
                        break;
                }
                that.$.append(that._$box.append(that._$list));
                if (attr.indexOf('bottom') !== -1) {
                    that.$.append($('<p>', {
                        'class': 'noti',
                        'text': decodeURIComponent(uidata.bottom)
                    }));
                }
                that.start = 0;
                this._setData(that.start);
                hx.il.ani.action({
                    type: 'in',
                    targets: that.$
                });
            };
            cimmi.prototype.destroy = function () {
                var that = this;
                that._$list.css('margin-top', '0px');
                that.start = 0;
                that.$.remove();
            };
            cimmi.prototype.getIndex = function (obj) {
                var itm = this._$items;
                var n;
                for (n = 0; n < itm.length; n += 1) {
                    if (itm[n].is(obj)) {
                        return n;
                    }
                }
            };
            cimmi.prototype.getItem = function (index) {
                if (this._$items.length < index) {
                    return undefined;
                }
                if (0 > index) {
                    return undefined;
                }
                return this._$items[index];
            };
            cimmi.prototype.setItemFocus = function (index) {
                if (this._$items.length < index) {
                    return;
                }
                if (0 > index) {
                    return;
                }
                this._setFocus(index);
            };
            cimmi.prototype.setData = function (data, fx, count) {
                var that = this;
                if (!data) {
                    hx.log("error", "data or callback function not defined");
                    return;
                }
                that._$list.empty();
                that.data = data;
                that.fx = fx || that.fx;
                that.count = count || that.count;
                that._setData(0);
                that._$list.css('margin-top', '0px');
            };
            cimmi.prototype.appendData = function (data) {
                if (!data) {
                    hx.log("error", "data not defined");
                    return;
                }
                if (!$.isArray(data)) {
                    data = [data];
                }
                if (data.length !== 0) {
                    var that = this;
                    var prevEndPos;
                    that.count += data.length;
                    prevEndPos = that.data.length;
                    that.data = that.data.concat(data);
                    that._setData(prevEndPos);
                }
            };
            cimmi.prototype.refresh = function () {
                this._setFocusEvent();
            };
            cimmi.prototype._doFocus = function () {
                if (this._$list.children('.on').length < 1) {
                    if (this._$items.length < 1) {
                        hx.log("error", "Must to have one or more items");
                        return;
                    }
                    this._setFocus(0);
                }
            };
            cimmi.prototype._on_alKey = function (param) {
                var that = this;
                var bConsumed = false;
                var k = hx.key, kc = param.alKey;
                if ((that.type === 'inputdialog') && (kc >= k.KEY_0) && (kc <= k.KEY_9)) {
                    if (that._ffPin && (that.count === that._insertedKey.length)) {
                        hx.log("cimmi", "pin end");
                    }
                    else {
                        var o, inputNum = kc - k.KEY_0;
                        that._insertedKey += inputNum;
                        o = that._$items[that._insertedKey.length - 1];
                        if (that.count > that._insertedKey.length) {
                            if (that._bBlind) {
                                o.addClass('pw');
                            }
                            else {
                                o.html(inputNum);
                            }
                            this._setFocus(that._insertedKey.length);
                        }
                        else {
                            if (that.count === that._insertedKey.length) {
                                if (that._bBlind) {
                                    o.addClass('pw');
                                }
                                else {
                                    o.html(inputNum);
                                }
                                if (!that._ffPin) {
                                    this._setUnfocus();
                                    that.fxInput('number', that._insertedKey);
                                }
                            }
                        }
                    }
                    bConsumed = true;
                }
                else {
                    throw "getCenterPageName in hx.al.entity check it";
                    //                var center = this.getPage().getCenterPageName();
                    var center;
                    switch (kc) {
                        case k.KEY_POWER:
                            that._doExit();
                            bConsumed = false;
                            break;
                        case k.KEY_GUIDE:
                        case k.KEY_ON_DEMAND:
                        case k.KEY_MENU:
                        case k.KEY_PLAYER:
                            if (center !== 'CPgCommonInterface') {
                                that._doExit();
                                bConsumed = false;
                            }
                            break;
                        case k.KEY_CHANNEL_UP:
                        case k.KEY_CHANNEL_DOWN:
                            if (center !== 'CPgCommonInterface') {
                                that._doExit();
                                bConsumed = false;
                            }
                            break;
                        default:
                            bConsumed = true;
                            var fx = this._keyTable[kc];
                            if (fx) {
                                bConsumed = fx.call(this, param);
                            }
                            break;
                    }
                }
                if (hx.config.netflix) {
                    switch (kc) {
                        case k.KEY_NETFLIX:
                            if (center !== 'CPgCommonInterface') {
                                that._doExit();
                                bConsumed = false;
                            }
                            break;
                    }
                }
                return bConsumed;
            };
            cimmi.prototype._doUp = function () {
                var that = this, i = that._index;
                var p = i - 1;
                if (!that._justList) {
                    if (i === 0) {
                        that.__scrollup();
                    }
                    else {
                        that._setFocus(p);
                    }
                }
                else {
                    if (that.viewCol) {
                        if (that.start === 0) {
                            that._$list.css('margin-top', '0px');
                        }
                        else {
                            var top = -(Math.abs(parseInt(that._$list.css('margin-top'), 10)) - this._$list.children().eq(0).outerHeight());
                            that._$list.css('margin-top', top + 'px');
                            that.start -= 1;
                        }
                    }
                }
                return true;
            };
            cimmi.prototype._doDown = function () {
                var that = this, i = that._index;
                var n = i + 1;
                if (!that._justList) {
                    var cnt = this._$list.children('.focusable').length - 1;
                    if (i >= cnt) {
                        that.__scrolldown();
                    }
                    else {
                        that._setFocus(n);
                    }
                }
                else {
                    if (that.viewCol) {
                        if ((that.start + that.viewCol) < that.count) {
                            var top = -(Math.abs(parseInt(that._$list.css('margin-top'), 10)) + this._$list.children().eq(0).outerHeight());
                            that._$list.css('margin-top', top + 'px');
                            that.start += 1;
                        }
                    }
                }
                return true;
            };
            cimmi.prototype._doLeft = function () {
                var that = this;
                that._insertedKey = that._insertedKey.substr(0, that._insertedKey.length - 1);
                var o = that._$items[that._insertedKey.length];
                if (that._bBlind) {
                    o.removeClass('pw');
                }
                else {
                    o.html('0');
                }
                that._setFocus(that._insertedKey.length);
                return true;
            };
            cimmi.prototype._doOkList = function () {
                var that = this;
                var o = that._$list.children('.on');
                var index = o.prevAll('.focusable').length;
                that.fxInput('selection', index + that.start);
                return true;
            };
            cimmi.prototype._doOkPin = function () {
                var that = this;
                that.fxInput('number', that._insertedKey);
                return true;
            };
            cimmi.prototype._doBack = function () {
                var that = this;
                that.fxInput('etc', 'previous');
                return true;
            };
            cimmi.prototype._doExit = function () {
                var that = this;
                that.fxInput('exit');
                return false;
            };
            cimmi.prototype._setFocus = function (index) {
                var that = this //
                , o = that._$items[index];
                that._setUnfocus();
                o.addClass('on');
                that._index = index;
            };
            cimmi.prototype._setUnfocus = function () {
                var that = this //
                , flist = that._$list.children('.on');
                if (flist.length) {
                    flist.removeClass('on');
                }
            };
            cimmi.prototype._setData = function (pos) {
                var that = this, i;
                for (i = pos; i < that.count; i += 1) {
                    var o = that._$items[i];
                    if (!o) {
                        o = that._$src.clone();
                        that._$items.push(o);
                    }
                    that._$list.append(o);
                    that.fx(i, o);
                }
                if (that.viewCol < that.count) {
                    that._$box.append('<p class="scr" style="top:0px;"></p>');
                }
                if (that.count !== 0) {
                    that._setFocusEvent();
                }
            };
            cimmi.prototype._setFocusEvent = function () {
                var that = this;
                if (that._justList) {
                    that._index = 0;
                }
                else {
                    this.$.focus(function () {
                        that._doFocus();
                    });
                }
            };
            cimmi.prototype.__scrolldown = function () {
                var list = this;
                var ti = list.data.length;
                if (ti <= list.count) {
                    list.setItemFocus(0);
                }
                else {
                    if ((list.start + list.count) === ti) {
                        list.count = (ti > list.viewCol) ? list.viewCol : ti;
                        list.start = 0;
                        list.setData(list.data);
                        list.setItemFocus(0);
                    }
                    else {
                        list.start = list.start + 1;
                        var phyStartPos = ti - list.start;
                        list.count = (phyStartPos > list.viewCol) ? list.viewCol : phyStartPos;
                        list.setData(list.data);
                        list.setItemFocus(list.count - 1);
                    }
                }
                return true;
            };
            cimmi.prototype.__scrollup = function () {
                var list = this;
                var ti = list.data.length;
                if (ti <= list.count) {
                    list.setItemFocus(ti - 1);
                }
                else {
                    var phyStartPos = 0;
                    if (list.start === 0) {
                        var remain = ti % list.count;
                        if (remain === 0) {
                            list.start = ti - list.viewCol;
                        }
                        else {
                            list.start = ti - remain;
                        }
                        phyStartPos = ti - list.start;
                        list.count = (phyStartPos > list.viewCol) ? list.viewCol : phyStartPos;
                        list.setData(list.data, list.fx);
                        list.setItemFocus(list.count - 1);
                    }
                    else {
                        list.start = list.start - 1;
                        phyStartPos = ti - list.start;
                        list.count = (phyStartPos > list.viewCol) ? list.viewCol : phyStartPos;
                        list.setData(list.data, list.fx);
                        list.setItemFocus(0);
                    }
                }
                return true;
            };
            return cimmi;
        })(__COMPONENT_KERNEL__);
        mComponent.cimmi = cimmi;
    })(mComponent || (mComponent = {}));
    return mComponent;
});
