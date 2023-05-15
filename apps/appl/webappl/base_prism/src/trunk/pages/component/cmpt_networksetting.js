///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cNetworkSetting.js
/*jslint sloppy:true, nomen:true, vars:true */
/**
 * @author mskang2
 */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var NetworkAddressType = (function (_super) {
        __extends(NetworkAddressType, _super);
        function NetworkAddressType() {
            _super.call(this);
        }
        NetworkAddressType.prototype.create = function (aParam) {
            var _this = this;
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
            this.$.focus(function () {
                _this._doFocus();
            });
            return this;
        };
        NetworkAddressType.prototype.destroy = function () {
            //TODO destroy _btnPrev, _btnNext
            this._destroy();
        };
        NetworkAddressType.prototype.setFirstNumValue = function (aParam) {
            var $firstNum = this._firstNum.$.children();
            var i;
            var len = aParam.length;
            var j = 2;
            for (i = 0; i < 3; i += 1) {
                $firstNum.eq(i).html('0');
            }
            for (i = (len - 1); i >= 0; i -= 1) {
                $firstNum.eq(j).html(aParam.charAt(i));
                j -= 1;
            }
        };
        NetworkAddressType.prototype.setSecondNumValue = function (aParam) {
            var $secondNum = this._secondNum.$.children();
            var len = aParam.length;
            var i;
            var j = 2;
            for (i = 0; i < 3; i += 1) {
                $secondNum.eq(i).html('0');
            }
            for (i = (len - 1); i >= 0; i -= 1) {
                $secondNum.eq(j).html(aParam.charAt(i));
                j -= 1;
            }
        };
        NetworkAddressType.prototype.setThirdNumValue = function (aParam) {
            var $thirdNum = this._thirdNum.$.children();
            var len = aParam.length;
            var i;
            var j = 2;
            for (i = 0; i < 3; i += 1) {
                $thirdNum.eq(i).html('0');
            }
            for (i = (len - 1); i >= 0; i -= 1) {
                $thirdNum.eq(j).html(aParam.charAt(i));
                j -= 1;
            }
        };
        NetworkAddressType.prototype.setFourthNumValue = function (aParam) {
            var $fourthNum = this._fourthNum.$.children(), len = aParam.length, i;
            var j = 2;
            for (i = 0; i < 3; i += 1) {
                $fourthNum.eq(i).html('0');
            }
            for (i = (len - 1); i >= 0; i -= 1) {
                $fourthNum.eq(j).html(aParam.charAt(i));
                j -= 1;
            }
        };
        NetworkAddressType.prototype.getFirstNumValue = function () {
            var $firstNum = this._firstNum.$.children();
            var number = "";
            number = $firstNum.eq(0).text();
            number += $firstNum.eq(1).text();
            number += $firstNum.eq(2).text();
            return parseInt(number, 10);
        };
        NetworkAddressType.prototype.getSecondNumValue = function () {
            var $secondNum = this._secondNum.$.children();
            var number = "";
            number = $secondNum.eq(0).text();
            number += $secondNum.eq(1).text();
            number += $secondNum.eq(2).text();
            return parseInt(number, 10);
        };
        NetworkAddressType.prototype.getThirdNumValue = function () {
            var $thirdNum = this._thirdNum.$.children();
            var number = "";
            number = $thirdNum.eq(0).text();
            number += $thirdNum.eq(1).text();
            number += $thirdNum.eq(2).text();
            return parseInt(number, 10);
        };
        NetworkAddressType.prototype.getFourtNumValue = function () {
            var $fourthNum = this._fourthNum.$.children();
            var number = "";
            number = $fourthNum.eq(0).text();
            number += $fourthNum.eq(1).text();
            number += $fourthNum.eq(2).text();
            return parseInt(number, 10);
        };
        NetworkAddressType.prototype.getAllNumValue = function () {
            var number = "";
            number = (this.getFirstNumValue() + ".");
            number += (this.getSecondNumValue() + ".");
            number += (this.getThirdNumValue() + ".");
            number += this.getFourtNumValue();
            return number;
        };
        NetworkAddressType.prototype._doFocus = function () {
            //hx.log("info","dofocus:::");
            this._fourthNum.clear();
            this._thirdNum.clear();
            this._secondNum.clear();
            this._firstNum.on();
            this._firstNum.position(0);
        };
        NetworkAddressType.prototype.__makeFirstNum = function (aNumCnt) {
            return hx.al.component('networknumbers', {
                attr: {
                    'class': 'wrap_num dot'
                },
                count: aNumCnt
            });
        };
        NetworkAddressType.prototype.__makeSecondNum = function (aNumCnt) {
            return hx.al.component('networknumbers', {
                attr: {
                    'class': 'wrap_num dot'
                },
                count: aNumCnt
            });
        };
        NetworkAddressType.prototype.__makeThirdNum = function (aNumCnt) {
            return hx.al.component('networknumbers', {
                attr: {
                    'class': 'wrap_num dot'
                },
                count: aNumCnt
            });
        };
        NetworkAddressType.prototype.__makeFourthNum = function (aNumCnt) {
            return hx.al.component('networknumbers', {
                attr: {
                    'class': 'wrap_num'
                },
                count: aNumCnt
            });
        };
        NetworkAddressType.prototype._currentChild = function () {
            this._firstNum.position(0);
            return this._firstNum;
        };
        return NetworkAddressType;
    })(__COMPONENT_KERNEL__);
    exports.NetworkAddressType = NetworkAddressType;
    /**
     * number component
     */
    var networknumbers = (function (_super) {
        __extends(networknumbers, _super);
        function networknumbers() {
            _super.call(this);
        }
        networknumbers.prototype.create = function (aParam) {
            var n;
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
        };
        networknumbers.prototype.destroy = function () {
            this.$.empty();
        };
        networknumbers.prototype.position = function (n) {
            if (n === undefined) {
                var on = this.$.children('.on');
                if (on.length === 0) {
                    return undefined;
                }
                return on.prevAll().length;
            }
            if (n >= 0 && n < this.count) {
                this.$.children().removeClass('on');
                this._$items[n].addClass('on');
            }
        };
        networknumbers.prototype.clear = function () {
            this.$.children().removeClass('on');
        };
        /*for Ipv6 key input*/
        networknumbers.prototype.timeout = function (aParam, that) {
            clearTimeout(this._timer);
            return that._move(aParam, function (index) {
                return index + 1;
            });
        };
        networknumbers.prototype._on_alKey = function (aParam) {
            var num = aParam.alKey - hx.key.KEY_0;
            /*for Ipv6 key input*/
            if (aParam.alKey >= hx.key.KEY_0 && aParam.alKey <= hx.key.KEY_9) {
                this.cnt = 0;
                var on = this.$.children('.on'), allNum = 0;
                if (on.prevAll().length === 0) {
                    allNum = parseInt(num + this._$items[1].text() + this._$items[2].text(), 10);
                    if (allNum > 255) {
                        return true;
                    }
                }
                else if (on.prevAll().length === 1) {
                    allNum = parseInt(this._$items[0].text() + num + this._$items[2].text(), 10);
                    if (allNum > 255) {
                        return true;
                    }
                }
                else if (on.prevAll().length === 2) {
                    allNum = parseInt(this._$items[0].text() + this._$items[1].text() + num, 10);
                    if (allNum > 255) {
                        return true;
                    }
                }
                return this._move(aParam, function (index, obj) {
                    obj.text(num);
                    return index + 1;
                });
            }
            else if (aParam.alKey === hx.key.KEY_LEFT) {
                this.cnt = 0;
                return this._move(aParam, function (index) {
                    return index - 1;
                });
            }
            else if (aParam.alKey === hx.key.KEY_RIGHT) {
                this.cnt = 0;
                return this._move(aParam, function (index) {
                    return index + 1;
                });
            }
            else if (aParam.alKey === hx.key.KEY_OK) {
                aParam.alEvent = 'algetIpAddr';
                this.sendEventToParent(aParam);
            }
            else if (aParam.alKey === hx.key.KEY_BACK) {
                aParam.alEvent = 'alExit';
                return false;
            }
        };
        networknumbers.prototype._move = function (aParam, cb) {
            var on = this.$.children('.on');
            this.cnt = 0;
            if (on.length === 0) {
                return;
            }
            var index = cb(on.prevAll().length, on);
            if (index < 0) {
                on.removeClass('on');
                aParam.alEvent = 'alNumberUnderflow';
            }
            else if (index >= this.count) {
                on.removeClass('on');
                aParam.alEvent = 'alNumberOverflow';
            }
            this.position(index);
            this.sendEventToParent(aParam);
            return true;
        };
        return networknumbers;
    })(__COMPONENT_KERNEL__);
    exports.networknumbers = networknumbers;
});
