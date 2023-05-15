///<reference path="../../def/sugar.d.ts" />
// file name : fw/common/component/cInstallationWizard.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var wizList = (function (_super) {
        __extends(wizList, _super);
        function wizList() {
            _super.call(this);
        }
        wizList.prototype.create = function (aParam) {
            var _this = this;
            var i;
            $.extend(this, aParam);
            $.extend(this, {
                $: $('<ul>', aParam.ul_attr),
                _arr: aParam.arr,
                _viewCount: aParam.viewCount
            });
            this._set_list(this.$, this._arr, 0, this._viewCount);
            this._keyTable = [];
            //this._keyTable[hx.key.KEY_LEFT] = this._doPrev;
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_UP] = this._doUp;
            this._keyTable[hx.key.KEY_CHANNEL_DOWN] = this._doChannelDown;
            this._keyTable[hx.key.KEY_CHANNEL_UP] = this._doChannelUp;
            this.$.focus(function () {
                _this.setItemFocus(0);
            });
            return this;
        };
        wizList.prototype._set_list = function (aUl, aArr, aFrom, aTo) {
            var i;
            aUl.children().remove();
            for (i = aFrom; i < aTo; i += 1) {
                if (aArr[i]) {
                    aUl.append($('<li>', {
                        'class': 'focusable',
                        'id': i,
                        'html': aArr[i]
                    }));
                }
            }
        };
        wizList.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                bConsumed = fx.call(this, aParam);
            }
            return bConsumed;
        };
        wizList.prototype.getLength = function () {
            return this._arr.length;
        };
        wizList.prototype.setItemFocus = function (aIdx) {
            if (this._arr.length < aIdx) {
                return;
            }
            if (0 > aIdx) {
                return;
            }
            this.$.children().removeClass('on');
            this.$.children().eq(aIdx).addClass('on');
        };
        wizList.prototype.getItemFocus = function () {
            var i, current = this.$.children('.on').prevAll('.focusable').length;
            var tmp = this.$.children().eq(current).html(), _len = this._arr.length, index = 0;
            for (i = 0; i < _len; i += 1) {
                if (this._arr[i] === tmp) {
                    index = i;
                    break;
                }
            }
            return index;
        };
        wizList.prototype.getShowItemFocus = function () {
            return this.$.children('.on').prevAll('.focusable').length;
        };
        wizList.prototype.destroy = function () {
            this.$.remove();
        };
        wizList.prototype._doChannelDown = function (aParam) {
            var target, i, j, ind, _ul, _ch, _len;
            if (this._arr.length <= this._viewCount) {
                return true;
            }
            for (j = 0; j < 5; j += 1) {
                target = this.$.children('.on').prevAll('.focusable').length;
                _ul = this.$;
                _ch = _ul.children();
                _len = this._arr.length;
                if (target < (this._viewCount - 2)) {
                    this.setItemFocus(target + 1);
                    ind = target + 1;
                }
                else {
                    if (_ch.length === this._viewCount) {
                        _ch.eq(0).remove();
                        var tmp = _ch.last().html();
                        for (i = 0; i < _len; i += 1) {
                            if (this._arr[i] === tmp && i === parseInt(_ch.last()[0].id, 10)) {
                                if ((i + 1) < _len) {
                                    this.$.append($('<li>', {
                                        'class': 'focusable',
                                        'id': i + 1,
                                        'html': this._arr[i + 1]
                                    }));
                                }
                                this.setItemFocus(this._viewCount - 2);
                                ind = this._viewCount - 2;
                                break;
                            }
                        }
                    }
                    else {
                        this._set_list(_ul, this._arr, 0, this._viewCount);
                        this.setItemFocus(0);
                        ind = 0;
                    }
                }
            }
            return true;
        };
        wizList.prototype._doChannelUp = function (aParam) {
            var target, i, j, ind, _ul, _ch, _len;
            if (this._arr.length <= this._viewCount) {
                return true;
            }
            for (j = 0; j < 5; j += 1) {
                target = this.$.children('.on').prevAll('.focusable').length;
                _ul = this.$;
                _ch = _ul.children();
                _len = this._arr.length;
                if (target > 0) {
                    this.setItemFocus(target - 1);
                    ind = target - 1;
                }
                else {
                    var tmp = _ch.eq(0).html();
                    for (i = 0; i < _len; i += 1) {
                        if (tmp === this._arr[i] && i === parseInt(_ch.eq(0)[0].id, 10)) {
                            if (i === 0) {
                                this._set_list(_ul, this._arr, (_len - this._viewCount + 1), _len);
                                this.setItemFocus(this._viewCount - 2);
                                ind = this._viewCount - 2;
                            }
                            else {
                                if (_ch.length === this._viewCount) {
                                    _ch.eq(this._viewCount - 1).remove();
                                }
                                this.$.prepend($('<li>', {
                                    'class': 'focusable',
                                    'id': i - 1,
                                    'html': this._arr[i - 1]
                                }));
                                this.setItemFocus(0);
                                ind = 0;
                            }
                            break;
                        }
                    }
                }
            }
            return true;
        };
        wizList.prototype._doDown = function (aParam) {
            if (this._arr.length === 1) {
                return;
            }
            var target = this.$.children('.on').prevAll('.focusable').length;
            var i, ind;
            var _ul = this.$;
            var _ch = _ul.children();
            var _len = this._arr.length;
            if (target < (this._viewCount - 2)) {
                this.setItemFocus(target + 1);
                ind = target + 1;
            }
            else {
                if (_ch.length === this._viewCount) {
                    _ch.eq(0).remove();
                    var tmp = _ch.last().html();
                    for (i = 0; i < _len; i += 1) {
                        if (this._arr[i] === tmp && i === parseInt(_ch.last()[0].id, 10)) {
                            if ((i + 1) < _len) {
                                this.$.append($('<li>', {
                                    'class': 'focusable',
                                    'id': i + 1,
                                    'html': this._arr[i + 1]
                                }));
                            }
                            this.setItemFocus(this._viewCount - 2);
                            ind = this._viewCount - 2;
                            break;
                        }
                    }
                }
                else {
                    this._set_list(_ul, this._arr, 0, this._viewCount);
                    this.setItemFocus(0);
                    ind = 0;
                }
            }
            aParam.alEvent = 'alScrolldown';
            aParam.alData = {
                index: ind
            };
        };
        wizList.prototype._doUp = function (aParam) {
            if (this._arr.length === 1) {
                return;
            }
            var target = this.$.children('.on').prevAll('.focusable').length;
            var i, ind;
            var _ul = this.$;
            var _ch = _ul.children();
            var _len = this._arr.length;
            if (target > 0) {
                this.setItemFocus(target - 1);
                ind = target - 1;
            }
            else {
                var tmp = _ch.eq(0).html();
                for (i = 0; i < _len; i += 1) {
                    if (tmp === this._arr[i] && i === parseInt(_ch.eq(0)[0].id, 10)) {
                        if (i === 0) {
                            this._set_list(_ul, this._arr, (_len - this._viewCount + 1), _len);
                            this.setItemFocus(this._viewCount - 2);
                            ind = this._viewCount - 2;
                        }
                        else {
                            if (_ch.length === this._viewCount) {
                                _ch.eq(this._viewCount - 1).remove();
                            }
                            this.$.prepend($('<li>', {
                                'class': 'focusable',
                                'id': i - 1,
                                'html': this._arr[i - 1]
                            }));
                            this.setItemFocus(0);
                            ind = 0;
                        }
                        break;
                    }
                }
            }
            aParam.alEvent = 'alScrollup';
            aParam.alData = {
                index: ind
            };
        };
        wizList.prototype._doOk = function (aParam) {
            var o = this.$.find('.on');
            var result = o.html();
            var i = o.prevAll('.focusable').length;
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData = {
                result: result,
                index: i,
                me: this
            };
            this.sendEventToParent(aParam);
            return true;
        };
        return wizList;
    })(__COMPONENT_KERNEL__);
    exports.wizList = wizList;
});
