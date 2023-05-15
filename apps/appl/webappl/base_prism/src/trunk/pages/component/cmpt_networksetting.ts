///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cNetworkSetting.js
/*jslint sloppy:true, nomen:true, vars:true */
/**
 * @author mskang2
 */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class NetworkAddressType extends __COMPONENT_KERNEL__ {
    _firstNum: any;
    _secondNum: any;
    _thirdNum: any;
    _fourthNum: any;
    _inputNumCnt: number;
    attr: any;

    constructor() {
        super();
    }

    create(aParam) {
        $.extend(this, aParam);
        if (this.attr['class'] === undefined) {
            this.attr['class'] = 'wrap_pw';
        }
        this.$ = $('<p>', this.attr);
        this._inputNumCnt = 3;
        this._firstNum = this.__makeFirstNum(this._inputNumCnt);
        this._secondNum = this.__makeSecondNum(this._inputNumCnt);
        this._thirdNum = this.__makeThirdNum(this._inputNumCnt);
        this._fourthNum = this.__makeFourthNum(this._inputNumCnt);
        this.$.append(this._firstNum.$).append(this._secondNum.$).append(this._thirdNum.$).append(this._fourthNum.$);
        //this.append(this._firstNum).append(this._secondNum).append(this._thirdNum).append(this._fourthNum);
        this.$.focus(()=> {
            this._doFocus();
        });
        return this;
    }

    destroy() {
        //TODO destroy _btnPrev, _btnNext
        this._destroy();
    }

    setFirstNumValue(aParam): void {
        var $firstNum = this._firstNum.$.children();
        var i: number;
        var len: number = aParam.length;
        var j: number = 2;
        for (i = 0; i < 3; i += 1) {
            $firstNum.eq(i).html('0');
        }
        for (i = (len - 1); i >= 0; i -= 1) {
            $firstNum.eq(j).html(aParam.charAt(i));
            j -= 1;
        }
    }

    setSecondNumValue(aParam): void {
        var $secondNum = this._secondNum.$.children();
        var len: number = aParam.length;
        var i: number;
        var j: number = 2;
        for (i = 0; i < 3; i += 1) {
            $secondNum.eq(i).html('0');
        }
        for (i = (len - 1); i >= 0; i -= 1) {
            $secondNum.eq(j).html(aParam.charAt(i));
            j -= 1;
        }
    }

    setThirdNumValue(aParam): void {
        var $thirdNum = this._thirdNum.$.children();
        var len: number = aParam.length;
        var i: number;
        var j = 2;
        for (i = 0; i < 3; i += 1) {
            $thirdNum.eq(i).html('0');
        }
        for (i = (len - 1); i >= 0; i -= 1) {
            $thirdNum.eq(j).html(aParam.charAt(i));
            j -= 1;
        }
    }

    setFourthNumValue(aParam): void {
        var $fourthNum = this._fourthNum.$.children(), //
            len: number = aParam.length, i: number;
        var j = 2;
        for (i = 0; i < 3; i += 1) {
            $fourthNum.eq(i).html('0');
        }
        for (i = (len - 1); i >= 0; i -= 1) {
            $fourthNum.eq(j).html(aParam.charAt(i));
            j -= 1;
        }
    }

    getFirstNumValue(): number {
        var $firstNum = this._firstNum.$.children();
        var number: string = "";
        number = $firstNum.eq(0).text();
        number += $firstNum.eq(1).text();
        number += $firstNum.eq(2).text();
        return parseInt(number, 10);
    }

    getSecondNumValue(): number {
        var $secondNum = this._secondNum.$.children();
        var number: string = "";
        number = $secondNum.eq(0).text();
        number += $secondNum.eq(1).text();
        number += $secondNum.eq(2).text();
        return parseInt(number, 10);
    }

    getThirdNumValue(): number {
        var $thirdNum = this._thirdNum.$.children();
        var number: string = "";
        number = $thirdNum.eq(0).text();
        number += $thirdNum.eq(1).text();
        number += $thirdNum.eq(2).text();
        return parseInt(number, 10);
    }

    getFourtNumValue(): number {
        var $fourthNum = this._fourthNum.$.children();
        var number: string = "";
        number = $fourthNum.eq(0).text();
        number += $fourthNum.eq(1).text();
        number += $fourthNum.eq(2).text();
        return parseInt(number, 10);
    }

    getAllNumValue(): string {
        var number: string = "";
        number = (this.getFirstNumValue() + ".");
        number += (this.getSecondNumValue() + ".");
        number += (this.getThirdNumValue() + ".");
        number += this.getFourtNumValue();
        return number;
    }

    _doFocus(): void {
        //hx.log("info","dofocus:::");
        this._fourthNum.clear();
        this._thirdNum.clear();
        this._secondNum.clear();
        this._firstNum.on();
        this._firstNum.position(0);
    }

    __makeFirstNum(aNumCnt: any): any {
        return hx.al.component('networknumbers', {
            attr: {
                'class': 'wrap_num dot'
            },
            count: aNumCnt
        });
    }

    __makeSecondNum(aNumCnt: any): any {
        return hx.al.component('networknumbers', {
            attr: {
                'class': 'wrap_num dot'
            },
            count: aNumCnt
        });
    }

    __makeThirdNum(aNumCnt: any): any {
        return hx.al.component('networknumbers', {
            attr: {
                'class': 'wrap_num dot'
            },
            count: aNumCnt
        });
    }

    __makeFourthNum(aNumCnt: any): any {
        return hx.al.component('networknumbers', {
            attr: {
                'class': 'wrap_num'
            },
            count: aNumCnt
        });
    }

    _currentChild(): any {
        this._firstNum.position(0);
        return this._firstNum;
    }
}
/**
 * number component
 */
export class networknumbers extends __COMPONENT_KERNEL__ {
    cnt: number;
    count: number;
    _$items: any;
    baseText: string;
    attr: any;
    _timer: any;

    constructor() {
        super();
    }

    create(aParam: any) {
        var n: number;
        $.extend(this, aParam);
        this.$ = $('<em>', this.attr);
        if (this.baseText === undefined) {
            this.baseText = "0";
        }
        if (this.count === undefined) {
            this.count = 1;
        }
        this._$items = [];
        for (n = 0; n < this.count; n += 1) {
            this._$items.push($('<span>').text(this.baseText).addClass('button'));
            this.$.append(this._$items[n]);
        }
        /*for Ipv6 key input*/
        this.cnt = 0;
        return this;
    }

    destroy() {
        this.$.empty();
    }

    position(n): any {
        if (n === undefined) {//GET
            var on = this.$.children('.on');
            if (on.length === 0) {
                return undefined;
            }
            return on.prevAll().length;
        }
        if (n >= 0 && n < this.count) {//SET
            this.$.children().removeClass('on');
            this._$items[n].addClass('on');
        }
    }

    clear() {
        this.$.children().removeClass('on');
    }

    /*for Ipv6 key input*/
    timeout(aParam, that) {
        clearTimeout(this._timer);
        return that._move(aParam, function (index) {
            return index + 1;
        });
    }

    _on_alKey(aParam): any {
        var num: number = aParam.alKey - hx.key.KEY_0;
        /*for Ipv6 key input*/
        if (aParam.alKey >= hx.key.KEY_0 && aParam.alKey <= hx.key.KEY_9) {
            this.cnt = 0;
            var on: any = this.$.children('.on'), allNum: number = 0;
            if (on.prevAll().length === 0) {
                allNum = parseInt(num + this._$items[1].text() + this._$items[2].text(), 10);
                if (allNum > 255) {
                    return true;
                }
            } else if (on.prevAll().length === 1) {
                allNum = parseInt(this._$items[0].text() + num + this._$items[2].text(), 10);
                if (allNum > 255) {
                    return true;
                }
            } else if (on.prevAll().length === 2) {
                allNum = parseInt(this._$items[0].text() + this._$items[1].text() + num, 10);
                if (allNum > 255) {
                    return true;
                }
            }
            return this._move(aParam, function (index, obj) {
                obj.text(num);
                return index + 1;
            });
        } else if (aParam.alKey === hx.key.KEY_LEFT) {
            this.cnt = 0;
            return this._move(aParam, function (index) {
                return index - 1;
            });
        } else if (aParam.alKey === hx.key.KEY_RIGHT) {
            this.cnt = 0;
            return this._move(aParam, function (index) {
                return index + 1;
            });
        } else if (aParam.alKey === hx.key.KEY_OK) {
            aParam.alEvent = 'algetIpAddr';
            this.sendEventToParent(aParam);
        } else if (aParam.alKey === hx.key.KEY_BACK) {
            aParam.alEvent = 'alExit';
            return false;
        }
    }

    _move(aParam, cb: any) {
        var on: any = this.$.children('.on');
        this.cnt = 0;
        if (on.length === 0) {
            return;
        }
        var index: number = cb(on.prevAll().length, on);
        if (index < 0) {
            on.removeClass('on');
            aParam.alEvent = 'alNumberUnderflow';
        } else if (index >= this.count) {
            on.removeClass('on');
            aParam.alEvent = 'alNumberOverflow';
        }
        this.position(index);
        this.sendEventToParent(aParam);
        return true;
    }
}
