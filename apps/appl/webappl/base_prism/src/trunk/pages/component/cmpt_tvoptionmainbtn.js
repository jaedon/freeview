var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var tvOptionMainbtn = (function (_super) {
        __extends(tvOptionMainbtn, _super);
        function tvOptionMainbtn() {
            _super.call(this);
        }
        tvOptionMainbtn.prototype.create = function (aParam) {
            $.extend(this, aParam);
            this._btnType = aParam;
            if (this._btnType === 'subtitle') {
                $.extend(this, {
                    $: $('<article>', {
                        'class': 'wrap_btn'
                    }),
                    _$btn: $('<p>', {
                        'class': 'button',
                        'text': hx.l('LOC_CLOSE_ID')
                    }),
                    _keyTable: []
                });
                this.$.append(this._$btn);
                this._keyTable[hx.key.KEY_OK] = this._doOk;
                this._keyTable[hx.key.KEY_DOWN] = this._doDown;
                this._keyTable[hx.key.KEY_UP] = this._doUp;
            }
            else {
                $.extend(this, {
                    $: $('<article>', {
                        'class': 'wrap_btn'
                    }),
                    _$btn_one: $('<p>', {
                        'class': 'button',
                        'text': 'OK'
                    }),
                    _$btn_two: $('<p>', {
                        'class': 'button',
                        'text': 'CANCEL'
                    }),
                    _keyTable: []
                });
                this.$.append(this._$btn_one).append(this._$btn_two);
                this._keyTable[hx.key.KEY_LEFT] = this._doNext;
                this._keyTable[hx.key.KEY_RIGHT] = this._doNext;
                this._keyTable[hx.key.KEY_OK] = this._doOk;
                this._keyTable[hx.key.KEY_DOWN] = this._doDown;
                this._keyTable[hx.key.KEY_UP] = this._doUp;
            }
            //this._keyTable = [];
            return this;
        };
        tvOptionMainbtn.prototype.destroy = function () {
            this.$.remove();
        };
        tvOptionMainbtn.prototype.clearFocus = function () {
            this.$.children(".on").removeClass('on');
        };
        tvOptionMainbtn.prototype.setItemFocus = function (aIndex) {
            if (0 > aIndex) {
                return;
            }
            if (this._btnType === 'subtitle') {
                this._$btn.addClass('on');
            }
            else {
                if (aIndex === 0) {
                    this._$btn_two.removeClass('on');
                    this._$btn_one.addClass('on');
                }
                else {
                    this._$btn_one.removeClass('on');
                    this._$btn_two.addClass('on');
                }
            }
        };
        tvOptionMainbtn.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                fx.call(this, aParam);
                bConsumed = true;
                // send key update
                this.sendEventToParent({
                    alEvent: 'alUpdate'
                });
            }
            return bConsumed;
        };
        tvOptionMainbtn.prototype._doDown = function (aParam) {
            this.$.children(".on").removeClass('on');
            this.sendEventToParent({
                alEvent: 'alChangeFocus',
                alData: {
                    me: this,
                    position: 'down'
                }
            });
        };
        tvOptionMainbtn.prototype._doUp = function (aParam) {
            this.$.children(".on").removeClass('on');
            this.sendEventToParent({
                alEvent: 'alChangeFocus',
                alData: {
                    me: this,
                    position: 'up'
                }
            });
        };
        tvOptionMainbtn.prototype._doNext = function (aParam) {
            if (this.$.children('.on').is(this._$btn_one)) {
                this.setItemFocus(1);
            }
            else {
                this.setItemFocus(0);
            }
        };
        tvOptionMainbtn.prototype._doOk = function (aParam) {
            aParam.alEvent = 'alChange';
            aParam.alData = {};
            aParam.alData.me = this;
            if (this.$.children('.on').is(this._$btn_one)) {
                aParam.alData.status = 'save';
            }
            else {
                aParam.alData.status = 'cancel';
            }
            this.sendEventToParent(aParam);
        };
        return tvOptionMainbtn;
    })(__COMPONENT_KERNEL__);
    exports.tvOptionMainbtn = tvOptionMainbtn;
});
