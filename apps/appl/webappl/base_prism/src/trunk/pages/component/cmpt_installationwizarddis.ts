///<reference path="../../def/sugar.d.ts" />
// file name : fw/common/component/cInstallationWizardDis.js : support item en/dis
/*jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class wizList2 extends __COMPONENT_KERNEL__ {
    _arr: any;
    _supportItem: any;
    _keyTable: any;
    _viewCount: number;

    constructor() {
        super();
    }
    create(aParam: any): any {
        var that = this, i: number;
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
            var i: number;
            for (i = 0; i < that._supportItem.length; i += 1) {
                if (that._supportItem[i] === true) {
                    break;
                }
            }
            that.setItemFocus(i);
        });
        return this;
    }

    _set_list(aUl, aArr, aFrom: number, aTo: number) {
        var that = this;
        var i: number, classVal: string;
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
                } else {
                    classVal = 'focusable';
                    aUl.append($('<li>', {
                        'class': classVal,
                        'id': i,
                        'html': aArr[i]
                    }));
                }
            }
        }
    }

    _on_alKey(aParam: {alKey: number}): boolean {
        var bConsumed: boolean = false;
        var fx: any = this._keyTable[aParam.alKey];
        if (fx) {
            fx.call(this, aParam);
        }
        return bConsumed;
    }

    getLength(): number {
        return this._arr.length;
    }

    setItemFocus(aIdx: number): void {
        if (this._arr.length < aIdx) {
            return;
        }
        if (0 > aIdx) {
            return;
        }
        this.$.children().removeClass('on');
        this.$.children().eq(aIdx).addClass('on');
    }

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
    destroy(): void {
        this.$.remove();
    }

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
    _doDown(aParam: {alData: any; alEvent: string}) {
        var that = this;
        if (that._arr.length === 1) {
            return;
        }
        var target_1: number = that.$.children('.on').prevAll('.focusable').length;
        var target_2: number = that.$.children('.on').prevAll('.disable').length;
        var target: number = target_1 + target_2;
        var i: number, ind: number;
        var _ul = that.$;
        var _ch = _ul.children();
        var _len: number = that._arr.length;
        var bnextFocusFound: boolean = false;
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
        } else {
            var noWork: any;
            // todo.. total list items > items per page
        }
        aParam.alEvent = 'alScrolldown';
        aParam.alData = {
            index: ind
        };
    }

    _doUp(aParam: {alData: any; alEvent: string}): void {
        var that = this;
        if (that._arr.length === 1) {
            return;
        }
        var target_1: number = that.$.children('.on').prevAll('.focusable').length;
        var target_2: number = that.$.children('.on').prevAll('.disable').length;
        var target: number = target_1 + target_2;
        var bnextFocusFound: boolean = false;
        var i: number, ind: number;
        var _ul = that.$;
        var _ch = _ul.children();
        var _len: number = that._arr.length;
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
    }

    _doOk(aParam) {
        var that = this;
        var o: any = that.$.find('.on');
        var result: any = o.html();
        var i: number = o.prevAll('.focusable').length + o.prevAll('.disable').length;
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
    }
}
