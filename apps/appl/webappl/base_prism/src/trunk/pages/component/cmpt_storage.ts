///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cStorage.js
/*jslint sloppy:true, nomen:true, vars:true */


declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class storageInfo extends __COMPONENT_KERNEL__ {
    $: JQuery;
    _$title: any;
    _$title2: any;
    _$title3: any;
    _$title4: any;
    _$totalStorage: any;
    _$userUsage: any;
    _$available: any;
    _$systemUsage: any;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam) {
        $.extend(this, aParam, {
            $: $('<dl>', {
                'class': ''
            }),
            _$title: $('<dt>', {
                'text': hx.l('LOC_TOTAL_STORAGE_ID')
            }),
            _$totalStorage: $('<dd>', {
                'text': aParam.totalStorage
            }),
            _$title3: $('<dt>', {
                'text': hx.l('LOC_USER_USAGE_ID')
            }),
            _$userUsage: $('<dd>', {
                'text': aParam.userUsage
            }),
            _$title4: $('<dt>', {
                'class': 'foot',
                'text': hx.l('LOC_AVAILABLE_ID')
            }),
            _$available: $('<dd>', {
                'class': 'foot',
                'text': aParam.available
            })
        });
        this.$.append(this._$title).append(this._$totalStorage).append(this._$title3).append(this._$userUsage).append(this._$title4).append(this._$available);
        if (aParam.systemUsage !== 'external') {
            $.extend(this, {
                _$title2: $('<dt>', {
                    'text': hx.l('LOC_SYSTEM_USAGE_ID')
                }),
                _$systemUsage: $('<dd>', {
                    'text': aParam.systemUsage
                })
            });
            this._$totalStorage.after(this._$title2.after(this._$systemUsage));
        }
        return this;
    }

    destroy() {
        this.$.remove();
    }
}
export class storageGraph extends __COMPONENT_KERNEL__ {
    $: JQuery;
    _$used: any;
    _$available: any;
    _$figure: any;
    _$canvas: any;

    constructor() {
        super();
    }

    init() {
        var ini;
    }

    create(aParam) {
        $.extend(this, aParam, {
            $: $('<aside>'),
            _$figure: $('<figure>'),
            _$canvas: $('<canvas>', {
                'id': 'divStorageDlgRight',
                'style': 'width:350px; height:180px; top:70px;'
            }),
            _$used: $('<p>'),
            _$available: $('<p>')
        });
        this._$used.html('<em>' + aParam.used + '</em>' + hx.l('LOC_USED_ID'));
        this._$available.html('<em>' + aParam.available + '</em>' + hx.l('LOC_SMALLAVAILABLE_ID'));
        this.$.append(this._$figure.append(this._$canvas)).append(this._$used).append(this._$available);
        return this;
    }

    destroy() {
        this.$.remove();
    }
}
export class strPincode extends __COMPONENT_KERNEL__ {
    $: JQuery;
    count: number;
    _$items: any;
    _$pin: any;
    baseText: string;
    _$txt: any;

    constructor() {
        super();
    }

    create(aParam) {
        $.extend(this, aParam);
        this.$ = $('<div>', {
            'class': 'pas',
            'style': 'top:270px;'
        });
        this._$txt = $('<p>');
        this._$txt.html(hx.l('LOC_ENTER_PASSWORD_ID'));
        this._$pin = $('<p>', {
            'class': 'wrap_pw'
        });
        if (this.baseText === undefined) {
            this.baseText = "-";
        }
        if (this.count === undefined) {
            this.count = 1;
        }
        this._$items = [];
        var n: any;
        for (n = 0; n < this.count; n += 1) {
            this._$items.push($('<em>').text(this.baseText));
            this._$items[n].addClass('button');
            if (n === 0) {
                this._$items[n].addClass('on');
            }
            this._$pin.append(this._$items[n]);
        }
        this.$.append(this._$txt).append(this._$pin);
        return this;
    }

    destroy() {
        this.$.empty();
    }

    getNum() {
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

    position(aIndex: number): any {
        if (aIndex === undefined) {//GET
            var on: any = this._$pin.children('.on');
            if (on.length === 0) {
                return undefined;
            }
            return on.prevAll().length;
        }
        if (aIndex >= 0 && aIndex < this.count) {//SET
            this._$pin.children().removeClass('on');
            this._$items[aIndex].addClass('on');
        }
    }

    clear() {
        this._$pin.children().removeClass('on');
    }

    //PRIVATE
    _on_alKey(aParam: {alKey: number; alEvent: string}) {
        if (aParam.alKey >= hx.key.KEY_0 && aParam.alKey <= hx.key.KEY_9) {
            var num: number = aParam.alKey - hx.key.KEY_0;
            return this._move(aParam, function (index, obj) {
                obj.text(num);
                return index + 1;
            });
        } else if (aParam.alKey === hx.key.KEY_LEFT) {
            return this._move(aParam, function (index) {
                return index - 1;
            });
        } else if (aParam.alKey === hx.key.KEY_RIGHT) {
            return this._move(aParam, function (index) {
                return index + 1;
            });
        } else if (aParam.alKey === hx.key.KEY_OK) {
            aParam.alEvent = 'alClicked';
        } else if (aParam.alKey === hx.key.KEY_BACK) {
            aParam.alEvent = 'alExit';
        }
        return false;
    }

    _move(aParam, cb) {
        var on: any = this._$pin.children('.on');
        if (on.length === 0) {
            return true;
        }
        var index: number = cb(on.prevAll().length, on);

        this.position(index);
        return false;
    }
}
