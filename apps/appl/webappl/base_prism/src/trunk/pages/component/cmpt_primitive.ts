/// <reference path="../../def/sugar.d.ts" />
// file name : fw/common/component/cPrimitive.js
/* jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class text extends __COMPONENT_KERNEL__ {
    create(param) {
        $.extend(this, param);
        if (this.tag === undefined) {
            this.tag = 'div';
        }
        this.tag = '<' + this.tag + '>';
        this.$ = $(this.tag, this.attr);

        var langID = this.attr['data-langID'];
        if (!!langID) {
            this.$.html(hx.l(langID));
        } else {
            this.$.html(this.text);
        }
        return this;
    }
    destroy() {
        this.$.remove();
    }
}
export class button extends __COMPONENT_KERNEL__ {
    usePointerKey: boolean;
    $item: JQuery;
    notSelfDestroy: boolean;
    create(param: any) {
        $.extend(this, param);
        if (this.attr['class'] === undefined) {
            this.attr['class'] = 'button';
        }
        this.usePointerKey = false;
        if(param.usePointerKey) {
            this.usePointerKey = param.usePointerKey;
        }
        if(param.notSelfDestroy) {
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
    }
    destroy() {
        if (this.notSelfDestroy) {
            return;
        }
        this.$.remove();
    }
    _on_alKey(param) {
        if (param.alKey == hx.key.KEY_OK) {// && this.sendEventToPage
            param.alEvent = 'alClicked';
        } else if (param.alKey == hx.key.KEY_DOWN && this.usePointerKey) {
            param.alEvent = 'alScrolldown';
        } else if (param.alKey == hx.key.KEY_UP && this.usePointerKey) {
            param.alEvent = 'alScrollup';
        } else if (param.alKey == hx.key.KEY_LEFT && this.usePointerKey) {
            param.alEvent = 'alScrollLeft';
        } else if (param.alKey == hx.key.KEY_RIGHT && this.usePointerKey) {
            param.alEvent = 'alScrollRight';
        } else if (param.alKey == hx.key.KEY_BACK && this.usePointerKey) {
            param.alEvent = 'alExit';
        }
    }
}
export class input extends __COMPONENT_KERNEL__ {
    create(param) {
        var that = this;
        var attr = param.attr;
        $.extend(that, param);
        that.$ = $('<input>', {
            'class' : 'wrap-input',
            'id' : attr.id,
            'type' : attr.type,
            'value' : '',
            'maxlength' : '32',
            'style' : 'input-format:' + attr.style
        });
        return that;
    }
    destroy() {
        this.$.remove();
    }
}
export class numbers extends __COMPONENT_KERNEL__ {
    baseText: string;
    count: number;
    _$items: JQuery[];
    create(param) {
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
        for ( n = 0; n < this.count; n += 1) {
            this._$items.push($('<span>').text(this.baseText));
            this.$.append(this._$items[n]);
        }
        return this;
    }
    destroy() {
        this.$.empty();
    }
    position(n) {
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
    _on_alKey(param) {
        var result;
        if (param.alKey >= hx.key.KEY_0 && param.alKey <= hx.key.KEY_9) {
            var num = param.alKey - hx.key.KEY_0;
            result = this._move(param, function(index, obj) {
                obj.text(num);
                return index + 1;
            });
        } else if (param.alKey === hx.key.KEY_LEFT) {
            result = this._move(param, function(index) {
                return index - 1;
            });
        } else if (param.alKey === hx.key.KEY_RIGHT) {
            result = this._move(param, function(index) {
                return index + 1;
            });
        }
        return result;
    }
    _move(param, cb) {
        var on = this.$.children('.on');
        if (on.length === 0) {
            return;
        }

        var index = cb(on.prevAll().length, on);

        if (index < 0) {
            on.removeClass('on');
            param.alEvent = 'alNumberUnderflow';
        } else if (index >= this.count) {
            on.removeClass('on');
            param.alEvent = 'alNumberOverflow';
        }

        this.position(index);
    }
}
export class frame extends __COMPONENT_KERNEL__ {
    create(param) {
        $.extend(this, param);
        if (this.tag === undefined) {
            this.tag = 'div';
        }
        this.tag = '<' + this.tag + '>';
        this.$ = $(this.tag, this.attr);
    }
    destroy() {
        this.$.empty();
    }
}
