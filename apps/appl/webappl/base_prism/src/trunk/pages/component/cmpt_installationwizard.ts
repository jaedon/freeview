///<reference path="../../def/sugar.d.ts" />
// file name : fw/common/component/cInstallationWizard.js
/*jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class wizList extends __COMPONENT_KERNEL__ {
    _arr: any;
    _viewCount: number;
    _keyTable: any;

    constructor() {
        super();
    }

    create(aParam: {ul_attr: any; arr: any; viewCount: any}) {
        var i: number;
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
        this.$.focus(()=> {
            this.setItemFocus(0);
        });
        return this;
    }

    _set_list(aUl: any, aArr: any, aFrom: number, aTo: number) {
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
    }

    _on_alKey(aParam: {alKey: number}): boolean {
        var bConsumed: boolean = false;
        var fx: any = this._keyTable[aParam.alKey];
        if (fx) {
            bConsumed = fx.call(this, aParam);
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

    getItemFocus(): number {
        var i: number, current: number = this.$.children('.on').prevAll('.focusable').length;
        var tmp: any = this.$.children().eq(current).html(), _len: number = this._arr.length, index: number = 0;
        for (i = 0; i < _len; i += 1) {
            if (this._arr[i] === tmp) {
                index = i;
                break;
            }
        }

        return index;
    }

    getShowItemFocus(): number {
        return this.$.children('.on').prevAll('.focusable').length;
    }

    destroy() {
        this.$.remove();
    }

    _doChannelDown(aParam): boolean {
        var target: number, i: number, j: number, ind: number, _ul: any, _ch: any, _len: number;
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
            } else {
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
                } else {
                    this._set_list(_ul, this._arr, 0, this._viewCount);
                    this.setItemFocus(0);
                    ind = 0;
                }
            }
        }
        return true;
    }

    _doChannelUp(aParam: any): boolean {
        var target: number, i: number, j: number, ind: number, _ul: any, _ch: any, _len: number;
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
            } else {
                var tmp = _ch.eq(0).html();
                for (i = 0; i < _len; i += 1) {
                    if (tmp === this._arr[i] && i === parseInt(_ch.eq(0)[0].id, 10)) {
                        if (i === 0) {
                            this._set_list(_ul, this._arr, (_len - this._viewCount + 1), _len);
                            this.setItemFocus(this._viewCount - 2);
                            ind = this._viewCount - 2;
                        } else {
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
    }

    _doDown(aParam: {alData: any; alEvent: string}): void {
        if (this._arr.length === 1) {
            return;
        }
        var target = this.$.children('.on').prevAll('.focusable').length;
        var i: number, ind: number;
        var _ul: any = this.$;
        var _ch: any = _ul.children();
        var _len: number = this._arr.length;
        if (target < (this._viewCount - 2)) {
            this.setItemFocus(target + 1);
            ind = target + 1;
        } else {
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
            } else {
                this._set_list(_ul, this._arr, 0, this._viewCount);
                this.setItemFocus(0);
                ind = 0;
            }
        }
        aParam.alEvent = 'alScrolldown';
        aParam.alData = {
            index: ind
        };
    }

    _doUp(aParam: {alData: any; alEvent: string}): void {
        if (this._arr.length === 1) {
            return;
        }
        var target = this.$.children('.on').prevAll('.focusable').length;
        var i: number, ind: number;
        var _ul: any = this.$;
        var _ch: any = _ul.children();
        var _len: number = this._arr.length;
        if (target > 0) {
            this.setItemFocus(target - 1);
            ind = target - 1;
        } else {
            var tmp = _ch.eq(0).html();
            for (i = 0; i < _len; i += 1) {
                if (tmp === this._arr[i] && i === parseInt(_ch.eq(0)[0].id, 10)) {
                    if (i === 0) {
                        this._set_list(_ul, this._arr, (_len - this._viewCount + 1), _len);
                        this.setItemFocus(this._viewCount - 2);
                        ind = this._viewCount - 2;
                    } else {
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
    }

    _doOk(aParam: {alData: any; alEvent: string}): boolean {
        var o: any = this.$.find('.on');
        var result: any = o.html();
        var i: number = o.prevAll('.focusable').length;
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
    }
}
