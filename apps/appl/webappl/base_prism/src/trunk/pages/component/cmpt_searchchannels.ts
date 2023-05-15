///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cSearchChannels.js
/*jslint sloppy:true, nomen:true, vars:true */
/**
 * @author mskang2
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class txt2 extends __COMPONENT_KERNEL__ {
    tag: string;
    attr: any;
    data: any;

    constructor() {
        super();
    }

    create(aParam) {
        $.extend(this, aParam);
        if (this.tag === undefined) {
            this.tag = 'p';
        }
        this.tag = '<' + this.tag + '>';
        if (this.attr['class'] === undefined) {
            this.attr['class'] = 'srcResult';
        }
        this.$ = $(this.tag, this.attr);
        this.$.html(this.data[0]).append($('<em>').html(this.data[1]))
        return this;
    }

    destroy() {
        this.$.empty();
    }
}
export class Search_result extends __COMPONENT_KERNEL__ {
    count: number;
    attr: any;
    itemAttr: any;
    _$src: any;
    fx: any;
    _$items: any;
    constructor() {
        super();
    }
    create(aParam) {
        $.extend(this, aParam);
        $.extend(this, {
            _$src: $('<li>', this.itemAttr),
            _$items: [],
            $: $('<ul>', this.attr)
        });
        var i: number;
        for (i = 0; i < this.count; i += 1) {
            var o = this._$src.clone();
            this._$items.push(o);
            this.$.append(o);
            this.fx(i, o);
        }
        return this;
    }
    destroy() {
        this.$.remove();
    }
    addItem(aParam) {
        var o: any = this._$src.clone();
        this._$items.push(o);
        this.$.append(o);
        o.html(aParam);
    }
    getItem(aIndex: number) {
        if (this._$items.length < aIndex) {
            return undefined;
        }
        if (0 > aIndex) {
            return undefined;
        }
        return this._$items[aIndex];
    }
}
export class searchProgress extends __COMPONENT_KERNEL__ {
    tag: any;
    attr: any;
    data: any;

    constructor() {
        super();
    }

    create(aParam) {
        $.extend(this, aParam);
        if (this.tag === undefined) {
            this.tag = 'dl';
        }
        this.tag = '<' + this.tag + '>';
        if (this.attr['class'] === undefined) {
            this.attr['class'] = 'progress type1';
        }
        this.$ = $(this.tag, this.attr);
        this.$//
            .append($('<dt>').html((this.data[0]))).append($('<dd>').html(this.data[1]))//
            .append($('<dd>').addClass('graph').append($('<span>', {
                'style': this.data[2]
            })));
        return this;
    }

    destroy() {
        this.$.empty();
    }
}
export class Search_list extends __COMPONENT_KERNEL__ {
    count: number;
    roll: any;
    _$items: any;
    _keyTable: any;
    _$src: any;
    data: any;
    fx: any;
    attr: any;

    constructor() {
        super();
    }

    create(aParam) {
        var i: number;
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
        this.$.focus(()=> {
            this._doFocus();
        });
        return this;
    }

    destroy() {
        this.$.remove();
    }

    addItem(aParam) {
        var o = this._$src.clone();
        this._$items.push(o);
        this.$.append(o);
        o.html(aParam);
    }

    getItem(aIndex: number) {
        if (this._$items.length < aIndex) {
            return undefined;
        }
        if (0 > aIndex) {
            return undefined;
        }
        return this._$items[aIndex];
    }

    setItemFocus(aIindex: number) {
        if (this._$items.length < aIindex) {
            return;
        }
        if (0 > aIindex) {
            return;
        }
        this.$.children().removeClass('on');
        this._$items[aIindex].addClass('on');
    }

    removeFocus() {
        this.$.children().removeClass('on');
    }

    setData(aData, aFx): void {
        var i: number;
        if (!aData || !aFx) {
            hx.log("error", "data or callback function not defined");
            return;
        }
        this.data = aData;
        for (i = 0; i < this.count; i += 1) {
            aFx.call(this, i, this._$items[i]);
        }
    }

    _doFocus(): void {
        if (this.$.children('.on').length < 1) {
            if (this._$items.length < 1) {
                hx.log("error", "Must to have one or more items");
                return;
            }
            this._$items[0].addClass('on');
        }
    }

    getLength(): any {
        return this._$items.length;
    }

    getCurrentIndex(): number {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        return i;
    }

    _on_alKey(aParam: {alKey: number; alData: any}): boolean {
        var bConsumed: boolean = false;
        var fx: any = this._keyTable[aParam.alKey];
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
    }

    _doUp(aParam: {alEvent: string}): boolean {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        this.$.children().removeClass('on');
        hx.log("warning", "i:" + i);
        if (i === 0) {
            if (this.roll === 'notroll') {
                aParam.alEvent = 'alScrollup';
                return false;
            } else {
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
        for (i; i >= 0; i -= 1) {//pass dis
            if (this.$.children().eq(i - 1).hasClass('dis')) {
                if (i === 0) {
                    aParam.alEvent = 'alScrollup';
                    return true;
                }
            } else {
                break;
            }
        }
        this.$.children().eq(i -= 1).addClass('on');
        return false;
    }

    _doDown(aParam: {alEvent: string}): boolean {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        var cnt: number = this.$.children('.focusable').length - 1;
        this.$.children().removeClass('on');
        // hx.log("warning", "i:" + i + " & cnt:" + cnt);
        if (i >= cnt) {
            if (this.roll === 'notroll') {
                aParam.alEvent = 'alScrolldown';
                return false;
            } else {
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
        for (i; i < this.count; i += 1) {//pass dis
            if (this.$.children().eq(i + 1).hasClass('dis')) {
                if ((i + 1) >= cnt) {
                    aParam.alEvent = 'alScrolldown';
                    return true;
                }
            } else {
                break;
            }
        }
        this.$.children().eq(i += 1).addClass('on');
        return false;
    }

    _doPageUp(param: {alEvent: string}): boolean {
        param.alEvent = 'alPageup';
        return true;
    }

    _doPageDown(param: {alEvent: string}): boolean {
        param.alEvent = 'alPagedown';
        return true;
    }

    _doLeft(param: {alData: any; alEvent: string}): boolean {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        param.alEvent = 'alScrollLeft';
        param.alData = {
            index: i
        };
        return true;
    }

    _doRight(param: {alData: any; alEvent: string}): boolean {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        param.alEvent = 'alScrollRight';
        param.alData = {
            index: i
        };
        return true;
    }

    _doOk(param: {alData: any; alEvent: string}): boolean {
        var o: any = this.$.children(".on");
        this.$.children().removeClass('on');
        var i: number = o.prevAll('.focusable').length;
        param.alEvent = 'alClicked';
        param.alData = {
            index: i
        };
        return true;
    }

    _doBack(param: {alEvent: string}): boolean {
        param.alEvent = 'alExit';
        return true;
    }
}
export class scnumbers extends __COMPONENT_KERNEL__ {
    count : number;
    _$items : any;
    baseText : string;
    attr : any;
    constructor() {
        super();
    }
    create(aParam)
    {
        var n: number;
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
    }
    destroy()
    {
        this.$.empty();
    }
    getNum()
    :
    string
    {
        var i: number;
        for (i = 0; i < this.count; i += 1) {
            if (i === 0) {
                var Num = this._$items[i].text();
            } else {
                Num += this._$items[i].text();
            }
        }
        return Num;
    }
    resetNum()
    {
        var i: number;
        for (i = 0; i < this.count; i += 1) {
            if (i === 0) {
                this._$items[i].text(0);
            } else {
                this._$items[i].text('-');
            }
        }
    }
    position(aParam)
    :
    any
    {
        if (aParam === undefined) {//GET
            var on: any = this.$.children('.on');
            if (on.length === 0) {
                return undefined;
            }
            return on.prevAll().length;
        }
        if (aParam >= 0 && aParam <= this.count) {//SET
            this.$.children().removeClass('on');
            if (aParam === 0) {
                this._$items[aParam].addClass('on');
                this._$items[aParam].removeClass('def');
            } else if (aParam === this.count) {
                this._$items[aParam - 1].addClass('on');
                if (this._$items[aParam - 1].text().length !== 0) {
                    this._$items[aParam - 1].removeClass('def');
                }
            } else {
                this._$items[aParam].addClass('on');
                if (this._$items[aParam - 1].text().length !== 0) {
                    this._$items[aParam - 1].removeClass('def');
                }
            }
        }
    }
    clear()
    {
        this.$.children().removeClass('on');
    }
    //PRIVATE
    _on_alKey(aParam
                  :
                  {
                      alKey: number;
                      alEvent: string
                  }
    ) :
    boolean
    {
        if (aParam.alKey >= hx.key.KEY_0 && aParam.alKey <= hx.key.KEY_9) {
            var num = aParam.alKey - hx.key.KEY_0;
            return this._move(aParam, function (index, obj) {
                obj.text(num);
                return index + 1;
            });
        } else if (aParam.alKey == hx.key.KEY_LEFT) {
            return this._move(aParam, function (index) {
                return index - 1;
            });
        } else if (aParam.alKey == hx.key.KEY_RIGHT) {
            return this._move(aParam, function (index) {
                return index + 1;
            });
        } else if (aParam.alKey == hx.key.KEY_OK) {
            aParam.alEvent = 'alClicked';
        } else if (aParam.alKey == hx.key.KEY_BACK) {
            aParam.alEvent = 'alExit';
        } else {
            return false;
        }
        return false;
    }
    _move(aParam, cb)
    {
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
    }
}
