var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var txt2 = (function (_super) {
        __extends(txt2, _super);
        function txt2() {
            _super.call(this);
        }
        txt2.prototype.create = function (aParam) {
            $.extend(this, aParam);
            if (this.tag === undefined) {
                this.tag = 'p';
            }
            this.tag = '<' + this.tag + '>';
            if (this.attr['class'] === undefined) {
                this.attr['class'] = 'srcResult';
            }
            this.$ = $(this.tag, this.attr);
            this.$.html(this.data[0]).append($('<em>').html(this.data[1]));
            return this;
        };
        txt2.prototype.destroy = function () {
            this.$.empty();
        };
        return txt2;
    })(__COMPONENT_KERNEL__);
    exports.txt2 = txt2;
    var Search_result = (function (_super) {
        __extends(Search_result, _super);
        function Search_result() {
            _super.call(this);
        }
        Search_result.prototype.create = function (aParam) {
            $.extend(this, aParam);
            $.extend(this, {
                _$src: $('<li>', this.itemAttr),
                _$items: [],
                $: $('<ul>', this.attr)
            });
            var i;
            for (i = 0; i < this.count; i += 1) {
                var o = this._$src.clone();
                this._$items.push(o);
                this.$.append(o);
                this.fx(i, o);
            }
            return this;
        };
        Search_result.prototype.destroy = function () {
            this.$.remove();
        };
        Search_result.prototype.addItem = function (aParam) {
            var o = this._$src.clone();
            this._$items.push(o);
            this.$.append(o);
            o.html(aParam);
        };
        Search_result.prototype.getItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        return Search_result;
    })(__COMPONENT_KERNEL__);
    exports.Search_result = Search_result;
    var searchProgress = (function (_super) {
        __extends(searchProgress, _super);
        function searchProgress() {
            _super.call(this);
        }
        searchProgress.prototype.create = function (aParam) {
            $.extend(this, aParam);
            if (this.tag === undefined) {
                this.tag = 'dl';
            }
            this.tag = '<' + this.tag + '>';
            if (this.attr['class'] === undefined) {
                this.attr['class'] = 'progress type1';
            }
            this.$ = $(this.tag, this.attr);
            this.$.append($('<dt>').html((this.data[0]))).append($('<dd>').html(this.data[1])).append($('<dd>').addClass('graph').append($('<span>', {
                'style': this.data[2]
            })));
            return this;
        };
        searchProgress.prototype.destroy = function () {
            this.$.empty();
        };
        return searchProgress;
    })(__COMPONENT_KERNEL__);
    exports.searchProgress = searchProgress;
    var Search_list = (function (_super) {
        __extends(Search_list, _super);
        function Search_list() {
            _super.call(this);
        }
        Search_list.prototype.create = function (aParam) {
            var _this = this;
            var i;
            $.extend(this, aParam);
            $.extend(this, {
                _$src: $('<li>', {
                    'class': 'focusable'
                }),
                _$items: [],
                $: $('<ul>', this.attr)
            });
            for (i = 0; i < this.count; i += 1) {
                var o = this._$src.clone();
                this._$items.push(o);
                this.$.append(o);
                this.fx(i, o);
            }
            this._keyTable = [];
            this._keyTable[hx.key.KEY_UP] = this._doUp;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_LEFT] = this._doLeft;
            this._keyTable[hx.key.KEY_RIGHT] = this._doRight;
            this._keyTable[hx.key.KEY_BACK] = this._doBack;
            this._keyTable[hx.key.KEY_CHANNEL_UP] = this._doPageUp;
            this._keyTable[hx.key.KEY_CHANNEL_DOWN] = this._doPageDown;
            this.$.focus(function () {
                _this._doFocus();
            });
            return this;
        };
        Search_list.prototype.destroy = function () {
            this.$.remove();
        };
        Search_list.prototype.addItem = function (aParam) {
            var o = this._$src.clone();
            this._$items.push(o);
            this.$.append(o);
            o.html(aParam);
        };
        Search_list.prototype.getItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        Search_list.prototype.setItemFocus = function (aIindex) {
            if (this._$items.length < aIindex) {
                return;
            }
            if (0 > aIindex) {
                return;
            }
            this.$.children().removeClass('on');
            this._$items[aIindex].addClass('on');
        };
        Search_list.prototype.removeFocus = function () {
            this.$.children().removeClass('on');
        };
        Search_list.prototype.setData = function (aData, aFx) {
            var i;
            if (!aData || !aFx) {
                hx.log("error", "data or callback function not defined");
                return;
            }
            this.data = aData;
            for (i = 0; i < this.count; i += 1) {
                aFx.call(this, i, this._$items[i]);
            }
        };
        Search_list.prototype._doFocus = function () {
            if (this.$.children('.on').length < 1) {
                if (this._$items.length < 1) {
                    hx.log("error", "Must to have one or more items");
                    return;
                }
                this._$items[0].addClass('on');
            }
        };
        Search_list.prototype.getLength = function () {
            return this._$items.length;
        };
        Search_list.prototype.getCurrentIndex = function () {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            return i;
        };
        Search_list.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                fx.call(this, aParam);
                if (aParam.alData === undefined) {
                    aParam.alData = {};
                }
                aParam.alData.me = this;
                this.sendEventToParent(aParam);
                bConsumed = true;
            }
            return bConsumed;
        };
        Search_list.prototype._doUp = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            this.$.children().removeClass('on');
            hx.log("warning", "i:" + i);
            if (i === 0) {
                if (this.roll === 'notroll') {
                    aParam.alEvent = 'alScrollup';
                    return false;
                }
                else {
                    i = this.count;
                }
            }
            if (this.$.children().eq(i - 1).hasClass('dis')) {
                i -= 1;
                if (i === 0) {
                    aParam.alEvent = 'alScrollup';
                    return true;
                }
            }
            for (i; i >= 0; i -= 1) {
                if (this.$.children().eq(i - 1).hasClass('dis')) {
                    if (i === 0) {
                        aParam.alEvent = 'alScrollup';
                        return true;
                    }
                }
                else {
                    break;
                }
            }
            this.$.children().eq(i -= 1).addClass('on');
            return false;
        };
        Search_list.prototype._doDown = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            var cnt = this.$.children('.focusable').length - 1;
            this.$.children().removeClass('on');
            // hx.log("warning", "i:" + i + " & cnt:" + cnt);
            if (i >= cnt) {
                if (this.roll === 'notroll') {
                    aParam.alEvent = 'alScrolldown';
                    return false;
                }
                else {
                    i = -1;
                }
            }
            if ((this.$.children().eq(i + 1).hasClass('transp')) && !(this.$.children().eq(i + 1).hasClass('active'))) {
                aParam.alEvent = 'alScrolldown';
                return true;
            }
            if (this.$.children().eq(i).hasClass('arrL') || this.$.children().eq(i).hasClass('arrR') || this.$.children().eq(i).html().substring(0, 4) === 'STOP') {
                aParam.alEvent = 'alScrolldown';
                return true;
            }
            for (i; i < this.count; i += 1) {
                if (this.$.children().eq(i + 1).hasClass('dis')) {
                    if ((i + 1) >= cnt) {
                        aParam.alEvent = 'alScrolldown';
                        return true;
                    }
                }
                else {
                    break;
                }
            }
            this.$.children().eq(i += 1).addClass('on');
            return false;
        };
        Search_list.prototype._doPageUp = function (param) {
            param.alEvent = 'alPageup';
            return true;
        };
        Search_list.prototype._doPageDown = function (param) {
            param.alEvent = 'alPagedown';
            return true;
        };
        Search_list.prototype._doLeft = function (param) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            param.alEvent = 'alScrollLeft';
            param.alData = {
                index: i
            };
            return true;
        };
        Search_list.prototype._doRight = function (param) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            param.alEvent = 'alScrollRight';
            param.alData = {
                index: i
            };
            return true;
        };
        Search_list.prototype._doOk = function (param) {
            var o = this.$.children(".on");
            this.$.children().removeClass('on');
            var i = o.prevAll('.focusable').length;
            param.alEvent = 'alClicked';
            param.alData = {
                index: i
            };
            return true;
        };
        Search_list.prototype._doBack = function (param) {
            param.alEvent = 'alExit';
            return true;
        };
        return Search_list;
    })(__COMPONENT_KERNEL__);
    exports.Search_list = Search_list;
    var scnumbers = (function (_super) {
        __extends(scnumbers, _super);
        function scnumbers() {
            _super.call(this);
        }
        scnumbers.prototype.create = function (aParam) {
            var n;
            $.extend(this, aParam);
            this.$ = $('<em>', this.attr);
            if (this.baseText === undefined) {
                this.baseText = "";
            }
            if (this.count === undefined) {
                this.count = 1;
            }
            this._$items = [];
            for (n = 0; n < this.count; n += 1) {
                this._$items.push($('<span>').text(this.baseText));
                this._$items[n].addClass('button def');
                if (n === 0) {
                    this._$items[n].text(0);
                    this._$items[n].addClass('on');
                    this._$items[n].removeClass('def');
                }
                this.$.append(this._$items[n]);
            }
            return this;
        };
        scnumbers.prototype.destroy = function () {
            this.$.empty();
        };
        scnumbers.prototype.getNum = function () {
            var i;
            for (i = 0; i < this.count; i += 1) {
                if (i === 0) {
                    var Num = this._$items[i].text();
                }
                else {
                    Num += this._$items[i].text();
                }
            }
            return Num;
        };
        scnumbers.prototype.resetNum = function () {
            var i;
            for (i = 0; i < this.count; i += 1) {
                if (i === 0) {
                    this._$items[i].text(0);
                }
                else {
                    this._$items[i].text('-');
                }
            }
        };
        scnumbers.prototype.position = function (aParam) {
            if (aParam === undefined) {
                var on = this.$.children('.on');
                if (on.length === 0) {
                    return undefined;
                }
                return on.prevAll().length;
            }
            if (aParam >= 0 && aParam <= this.count) {
                this.$.children().removeClass('on');
                if (aParam === 0) {
                    this._$items[aParam].addClass('on');
                    this._$items[aParam].removeClass('def');
                }
                else if (aParam === this.count) {
                    this._$items[aParam - 1].addClass('on');
                    if (this._$items[aParam - 1].text().length !== 0) {
                        this._$items[aParam - 1].removeClass('def');
                    }
                }
                else {
                    this._$items[aParam].addClass('on');
                    if (this._$items[aParam - 1].text().length !== 0) {
                        this._$items[aParam - 1].removeClass('def');
                    }
                }
            }
        };
        scnumbers.prototype.clear = function () {
            this.$.children().removeClass('on');
        };
        //PRIVATE
        scnumbers.prototype._on_alKey = function (aParam) {
            if (aParam.alKey >= hx.key.KEY_0 && aParam.alKey <= hx.key.KEY_9) {
                var num = aParam.alKey - hx.key.KEY_0;
                return this._move(aParam, function (index, obj) {
                    obj.text(num);
                    return index + 1;
                });
            }
            else if (aParam.alKey == hx.key.KEY_LEFT) {
                return this._move(aParam, function (index) {
                    return index - 1;
                });
            }
            else if (aParam.alKey == hx.key.KEY_RIGHT) {
                return this._move(aParam, function (index) {
                    return index + 1;
                });
            }
            else if (aParam.alKey == hx.key.KEY_OK) {
                aParam.alEvent = 'alClicked';
            }
            else if (aParam.alKey == hx.key.KEY_BACK) {
                aParam.alEvent = 'alExit';
            }
            else {
                return false;
            }
            return false;
        };
        scnumbers.prototype._move = function (aParam, cb) {
            var on = this.$.children('.on');
            if (on.length === 0) {
                return true;
            }
            var index = cb(on.prevAll().length, on);
            /*
             if(index < 0) {
             on.removeClass('on');
    
             param.alEvent = 'alNumberUnderflow';
             return true;
             } else if(index >= this.count) {
             on.removeClass('on');
    
             param.alEvent = 'alNumberOverflow';
             return true;
             }
             */
            this.position(index);
            return true;
        };
        return scnumbers;
    })(__COMPONENT_KERNEL__);
    exports.scnumbers = scnumbers;
});
