var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var wizList2 = (function (_super) {
        __extends(wizList2, _super);
        function wizList2() {
            _super.call(this);
        }
        wizList2.prototype.create = function (aParam) {
            var that = this, i;
            $.extend(this, aParam);
            $.extend(that, {
                $: $('<ul>', aParam.ul_attr),
                _arr: aParam.arr,
                _supportItem: aParam.supportItem,
                _viewCount: aParam.viewCount
            });
            that._set_list(that.$, that._arr, 0, that._viewCount);
            this._keyTable = [];
            //this._keyTable[hx.key.KEY_LEFT] = this._doPrev;
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_UP] = this._doUp;
            //this._keyTable[hx.key.KEY_CHANNEL_DOWN] = this._doChannelDown;
            //this._keyTable[hx.key.KEY_CHANNEL_UP] = this._doChannelUp;
            this.$.focus(function () {
                var i;
                for (i = 0; i < that._supportItem.length; i += 1) {
                    if (that._supportItem[i] === true) {
                        break;
                    }
                }
                that.setItemFocus(i);
            });
            return this;
        };
        wizList2.prototype._set_list = function (aUl, aArr, aFrom, aTo) {
            var that = this;
            var i, classVal;
            aUl.children().remove();
            for (i = aFrom; i < aTo; i += 1) {
                if (aArr[i]) {
                    if (that._supportItem[i] === false) {
                        classVal = 'disable';
                        aUl.append($('<li>', {
                            'class': classVal,
                            'id': i,
                            'html': aArr[i],
                            'style': 'color:#A1A1A1'
                        }));
                    }
                    else {
                        classVal = 'focusable';
                        aUl.append($('<li>', {
                            'class': classVal,
                            'id': i,
                            'html': aArr[i]
                        }));
                    }
                }
            }
        };
        wizList2.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                fx.call(this, aParam);
            }
            return bConsumed;
        };
        wizList2.prototype.getLength = function () {
            return this._arr.length;
        };
        wizList2.prototype.setItemFocus = function (aIdx) {
            if (this._arr.length < aIdx) {
                return;
            }
            if (0 > aIdx) {
                return;
            }
            this.$.children().removeClass('on');
            this.$.children().eq(aIdx).addClass('on');
        };
        /*
         getItemFocus : function() {
         var that = this;
         var current = that.$.children('.on').prevAll('.focusable').length;
         var tmp = that.$.children().eq(current).html(), _len = that._arr.length, index = 0;
         for ( i = 0; i < _len; i += 1) {
         if (that._arr[i] === tmp) {
         index = i;
         break;
         }
         }
    
         if (index === that._arr.length - 1) {
         index = -1;
         }
    
         return index;
         },
         */
        /*
         getShowItemFocus : function() {
         var that = this;
         return that.$.children('.on').prevAll('.focusable').length;
         },
         */
        wizList2.prototype.destroy = function () {
            this.$.remove();
        };
        /*
         _doChannelDown : function(param) {
         var that = this, target, i, j, ind, _ul, _ch, _len;
    
         if (that._arr.length <= that._viewCount) {
         return true;
         }
    
         for ( j = 0; j < 5; j += 1) {
         target = that.$.children('.on').prevAll('.focusable').length;
         _ul = that.$;
         _ch = _ul.children();
         _len = that._arr.length;
    
         if (target < (that._viewCount - 2)) {
         that.setItemFocus(target + 1);
         ind = target + 1;
         } else {
         if (_ch.length === that._viewCount) {
         _ch.eq(0).remove();
         var tmp = _ch.last().html();
         for ( i = 0; i < _len; i += 1) {
         if (that._arr[i] === tmp && i === parseInt(_ch.last()[0].id, 10)) {
         if ((i + 1) < _len) {
         that.$.append($('<li>', {
         'class' : 'focusable',
         'id' : i + 1,
         'html' : that._arr[i + 1]
         }));
         }
         that.setItemFocus(that._viewCount - 2);
         ind = that._viewCount - 2;
         break;
         }
         }
         } else {
         that._set_list(_ul, that._arr, 0, that._viewCount);
         that.setItemFocus(0);
         ind = 0;
         }
         }
         }
         return true;
         },
         _doChannelUp : function(param) {
         var that = this, target, i, j, ind, _ul, _ch, _len;
    
         if (that._arr.length <= that._viewCount) {
         return true;
         }
    
         for ( j = 0; j < 5; j += 1) {
         target = that.$.children('.on').prevAll('.focusable').length;
         _ul = that.$;
         _ch = _ul.children();
         _len = that._arr.length;
         if (target > 0) {
         that.setItemFocus(target - 1);
         ind = target - 1;
         } else {
         var tmp = _ch.eq(0).html();
         for ( i = 0; i < _len; i += 1) {
         if (tmp === that._arr[i] && i === parseInt(_ch.eq(0)[0].id, 10)) {
         if (i === 0) {
         that._set_list(_ul, that._arr, (_len - that._viewCount + 1), _len);
         that.setItemFocus(that._viewCount - 2);
         ind = that._viewCount - 2;
         } else {
         if (_ch.length === that._viewCount) {
         _ch.eq(that._viewCount - 1).remove();
         }
         that.$.prepend($('<li>', {
         'class' : 'focusable',
         'id' : i - 1,
         'html' : that._arr[i - 1]
         }));
         that.setItemFocus(0);
         ind = 0;
         }
         break;
         }
         }
         }
         }
         return true;
         },
         */
        wizList2.prototype._doDown = function (aParam) {
            var that = this;
            if (that._arr.length === 1) {
                return;
            }
            var target_1 = that.$.children('.on').prevAll('.focusable').length;
            var target_2 = that.$.children('.on').prevAll('.disable').length;
            var target = target_1 + target_2;
            var i, ind;
            var _ul = that.$;
            var _ch = _ul.children();
            var _len = that._arr.length;
            var bnextFocusFound = false;
            if (target < (that._viewCount - 2)) {
                if (target < _len - 1) {
                    for (i = target + 1; i < _len; i += 1) {
                        if (that._supportItem[i] === true) {
                            ind = i;
                            bnextFocusFound = true;
                            break;
                        }
                    }
                }
                if (bnextFocusFound !== true) {
                    for (i = 0; i <= target; i += 1) {
                        if (that._supportItem[i] === true) {
                            ind = i;
                            bnextFocusFound = true;
                            break;
                        }
                    }
                }
            }
            else {
                var noWork;
            }
            aParam.alEvent = 'alScrolldown';
            aParam.alData = {
                index: ind
            };
        };
        wizList2.prototype._doUp = function (aParam) {
            var that = this;
            if (that._arr.length === 1) {
                return;
            }
            var target_1 = that.$.children('.on').prevAll('.focusable').length;
            var target_2 = that.$.children('.on').prevAll('.disable').length;
            var target = target_1 + target_2;
            var bnextFocusFound = false;
            var i, ind;
            var _ul = that.$;
            var _ch = _ul.children();
            var _len = that._arr.length;
            for (i = target - 1; i >= 0; i -= 1) {
                if (that._supportItem[i] === true) {
                    ind = i;
                    bnextFocusFound = true;
                    break;
                }
            }
            if (bnextFocusFound === false) {
                for (i = _len - 1; i >= target; i -= 1) {
                    if (that._supportItem[i] === true) {
                        ind = i;
                        bnextFocusFound = true;
                        break;
                    }
                }
            }
            aParam.alEvent = 'alScrollup';
            aParam.alData = {
                index: ind
            };
        };
        wizList2.prototype._doOk = function (aParam) {
            var that = this;
            var o = that.$.find('.on');
            var result = o.html();
            var i = o.prevAll('.focusable').length + o.prevAll('.disable').length;
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData = {
                result: result,
                index: i,
                me: that
            };
            that.sendEventToParent(aParam);
        };
        return wizList2;
    })(__COMPONENT_KERNEL__);
    exports.wizList2 = wizList2;
});
