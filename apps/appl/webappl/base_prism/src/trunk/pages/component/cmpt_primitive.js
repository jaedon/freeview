var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    /// <reference path="../../def/sugar.d.ts" />
    var text = (function (_super) {
        __extends(text, _super);
        function text() {
            _super.apply(this, arguments);
        }
        text.prototype.create = function (param) {
            $.extend(this, param);
            if (this.tag === undefined) {
                this.tag = 'div';
            }
            this.tag = '<' + this.tag + '>';
            this.$ = $(this.tag, this.attr);
            var langID = this.attr['data-langID'];
            if (!!langID) {
                this.$.html(hx.l(langID));
            }
            else {
                this.$.html(this.text);
            }
            return this;
        };
        text.prototype.destroy = function () {
            this.$.remove();
        };
        return text;
    })(__COMPONENT_KERNEL__);
    exports.text = text;
    var button = (function (_super) {
        __extends(button, _super);
        function button() {
            _super.apply(this, arguments);
        }
        button.prototype.create = function (param) {
            $.extend(this, param);
            if (this.attr['class'] === undefined) {
                this.attr['class'] = 'button';
            }
            this.usePointerKey = false;
            if (param.usePointerKey) {
                this.usePointerKey = param.usePointerKey;
            }
            if (param.notSelfDestroy) {
                this.notSelfDestroy = param.notSelfDestroy;
            }
            this.$ = $('<p>', this.attr);
            this.$.addClass('focusable');
            if (this.$item) {
                this.$.append(this.$item);
            }
            var langID = this.attr['data-langID'];
            if (!!langID) {
                if (param.upper === true)
                    this.$.html(hx.l(langID).toUpperCase());
                else
                    this.$.html(hx.l(langID));
            }
            return this;
        };
        button.prototype.destroy = function () {
            if (this.notSelfDestroy) {
                return;
            }
            this.$.remove();
        };
        button.prototype._on_alKey = function (param) {
            if (param.alKey == hx.key.KEY_OK) {
                param.alEvent = 'alClicked';
            }
            else if (param.alKey == hx.key.KEY_DOWN && this.usePointerKey) {
                param.alEvent = 'alScrolldown';
            }
            else if (param.alKey == hx.key.KEY_UP && this.usePointerKey) {
                param.alEvent = 'alScrollup';
            }
            else if (param.alKey == hx.key.KEY_LEFT && this.usePointerKey) {
                param.alEvent = 'alScrollLeft';
            }
            else if (param.alKey == hx.key.KEY_RIGHT && this.usePointerKey) {
                param.alEvent = 'alScrollRight';
            }
            else if (param.alKey == hx.key.KEY_BACK && this.usePointerKey) {
                param.alEvent = 'alExit';
            }
        };
        return button;
    })(__COMPONENT_KERNEL__);
    exports.button = button;
    var input = (function (_super) {
        __extends(input, _super);
        function input() {
            _super.apply(this, arguments);
        }
        input.prototype.create = function (param) {
            var that = this;
            var attr = param.attr;
            $.extend(that, param);
            that.$ = $('<input>', {
                'class': 'wrap-input',
                'id': attr.id,
                'type': attr.type,
                'value': '',
                'maxlength': '32',
                'style': 'input-format:' + attr.style
            });
            return that;
        };
        input.prototype.destroy = function () {
            this.$.remove();
        };
        return input;
    })(__COMPONENT_KERNEL__);
    exports.input = input;
    var numbers = (function (_super) {
        __extends(numbers, _super);
        function numbers() {
            _super.apply(this, arguments);
        }
        numbers.prototype.create = function (param) {
            $.extend(this, param);
            this.$ = $('<p>', this.attr);
            if (this.baseText === undefined) {
                this.baseText = "0";
            }
            if (this.count === undefined) {
                this.count = 1;
            }
            this._$items = [];
            var n;
            for (n = 0; n < this.count; n += 1) {
                this._$items.push($('<span>').text(this.baseText));
                this.$.append(this._$items[n]);
            }
            return this;
        };
        numbers.prototype.destroy = function () {
            this.$.empty();
        };
        numbers.prototype.position = function (n) {
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
        numbers.prototype.clear = function () {
            this.$.children().removeClass('on');
        };
        numbers.prototype._on_alKey = function (param) {
            var result;
            if (param.alKey >= hx.key.KEY_0 && param.alKey <= hx.key.KEY_9) {
                var num = param.alKey - hx.key.KEY_0;
                result = this._move(param, function (index, obj) {
                    obj.text(num);
                    return index + 1;
                });
            }
            else if (param.alKey === hx.key.KEY_LEFT) {
                result = this._move(param, function (index) {
                    return index - 1;
                });
            }
            else if (param.alKey === hx.key.KEY_RIGHT) {
                result = this._move(param, function (index) {
                    return index + 1;
                });
            }
            return result;
        };
        numbers.prototype._move = function (param, cb) {
            var on = this.$.children('.on');
            if (on.length === 0) {
                return;
            }
            var index = cb(on.prevAll().length, on);
            if (index < 0) {
                on.removeClass('on');
                param.alEvent = 'alNumberUnderflow';
            }
            else if (index >= this.count) {
                on.removeClass('on');
                param.alEvent = 'alNumberOverflow';
            }
            this.position(index);
        };
        return numbers;
    })(__COMPONENT_KERNEL__);
    exports.numbers = numbers;
    var frame = (function (_super) {
        __extends(frame, _super);
        function frame() {
            _super.apply(this, arguments);
        }
        frame.prototype.create = function (param) {
            $.extend(this, param);
            if (this.tag === undefined) {
                this.tag = 'div';
            }
            this.tag = '<' + this.tag + '>';
            this.$ = $(this.tag, this.attr);
        };
        frame.prototype.destroy = function () {
            this.$.empty();
        };
        return frame;
    })(__COMPONENT_KERNEL__);
    exports.frame = frame;
});
